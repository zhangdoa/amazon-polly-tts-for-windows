/*  Copyright 2017 - 2018 Amazon.com, Inc. or its affiliates.All Rights Reserved.
Licensed under the Amazon Software License(the "License").You may not use
this file except in compliance with the License.A copy of the License is
located at

http://aws.amazon.com/asl/

and in the "LICENSE" file accompanying this file.This file is distributed
on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express
or implied.See the License for the specific language governing
permissions and limitations under the License. */
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

//--- Additional includes
#include "stdafx.h"
#include "TtsEngObj.h"
#include "tchar.h"
#include "atlstr.h"
#include <aws/core/Aws.h>
#include <aws/polly/PollyClient.h>
#include <aws/polly/model/DescribeVoicesRequest.h>
#include "PollyManager.h"
#include "spdlog/spdlog.h"
#include "tinyxml2.h"
#include <aws/core/platform/Environment.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <boost/algorithm/string.hpp>

//--- Local
using namespace Aws::Polly;
using namespace Model;
using namespace Aws::Utils;
using namespace tinyxml2;

TCHAR* CTTSEngObj::GetPath()
{
	TCHAR buf[MAX_PATH];

	GetTempPath(MAX_PATH, buf);
	return buf;
}

/*****************************************************************************
* CTTSEngObj::FinalConstruct *
*----------------------------*
*   Description:
*       Constructor
*****************************************************************************/
HRESULT CTTSEngObj::FinalConstruct()
{
	auto temp_folder = GetPath();
	char logPath[MAX_PATH];
	sprintf_s(logPath, MAX_PATH, "%lspolly-tts.log", temp_folder);
	m_logger = std::make_shared<spdlog::logger>("msvc_logger", std::make_shared<spdlog::sinks::msvc_sink_mt>());
	m_logger->set_level(spdlog::level::debug);

	spdlog::set_pattern("[%H:%M:%S %z] %v");
#ifdef DEBUG
	spdlog::set_level(spdlog::level::debug); //Set global log level to info
#endif
	HRESULT hr = S_OK;
	wcscpy(m_voiceOveride, L"");

    return hr;
} /* CTTSEngObj::FinalConstruct */

/*****************************************************************************
* CTTSEngObj::FinalRelease *
*--------------------------*
*   Description:
*       destructor
*****************************************************************************/
void CTTSEngObj::FinalRelease()
{
} /* CTTSEngObj::FinalRelease */

//
//=== ISpObjectWithToken Implementation ======================================
//

/*****************************************************************************
* CTTSEngObj::SetObjectToken *
*----------------------------*
*   Description:
*       This function performs the majority of the initialization of the voice.
*   Once the object token has been provided, the filenames are read from the
*   token key and the files are mapped.
*****************************************************************************/
STDMETHODIMP CTTSEngObj::SetObjectToken(ISpObjectToken * pToken)
{
	m_logger->info("SetObjectToken");
	HRESULT hr;
	m_logger->info("Setting object token");
	hr = SpGenericSetObjectToken(pToken, m_cpToken);
	m_logger->info("SpGenericSetObjectToken Response: {0}" , hr);
	return hr;
} /* CTTSEngObj::SetObjectToken */

//
//

/*****************************************************************************
* CTTSEngObj::Speak *
*-------------------*
*   Description:
*       This is the primary method that SAPI calls to render text.
*-----------------------------------------------------------------------------
*   Input Parameters
*
*   pUser
*       Pointer to the current user profile object. This object contains
*       information like what languages are being used and this object
*       also gives access to resources like the SAPI master lexicon object.
*
*   dwSpeakFlags
*       This is a set of flags used to control the behavior of the
*       SAPI voice object and the associated engine.
*
*   VoiceFmtIndex
*       Zero based index specifying the output format that should
*       be used during rendering.
*
*   pTextFragList
*       A linked list of text fragments to be rendered. There is
*       one fragement per XML state change. If the input text does
*       not contain any XML markup, there will only be a single fragment.
*
*   pOutputSite
*       The interface back to SAPI where all output audio samples and events are written.
*
*   Return Values
*       S_OK - This should be returned after successful rendering or if
*              rendering was interrupted because *pfContinue changed to FALSE.
*       E_INVALIDARG 
*       E_OUTOFMEMORY
*
*****************************************************************************/
STDMETHODIMP CTTSEngObj::Speak( DWORD dwSpeakFlags,
                                REFGUID rguidFormatId,
                                const WAVEFORMATEX * pWaveFormatEx,
                                const SPVTEXTFRAG* pTextFragList,
                                ISpTTSEngineSite* pOutputSite )
{
	Aws::SDKOptions options;
	m_logger->debug("Starting Speak\n");

	if (wcslen(m_voiceOveride) == 0)
	{
		LPWSTR data;
		CComPtr<ISpDataKey> attributesKey;
		m_logger->debug("Reading attributes key to get the voice\n");
		m_cpToken->OpenKey(L"Attributes", &attributesKey);
		attributesKey->GetStringValue(L"VoiceId", &m_pPollyVoice);
		attributesKey->GetStringValue(L"IsNeural", &data);
		m_isNeural = wcscmp(L"1", data) == 0;
		attributesKey->GetStringValue(L"IsNews", &data);
		m_isNews = wcscmp(L"1", data) == 0;
		attributesKey->GetStringValue(L"IsConversational", &data);
		m_isConversational = wcscmp(L"1", data) == 0;
	}
	m_logger->debug("Initializing AWS\n");
	InitAPI(options);
	HRESULT hr = S_OK;

	//--- Check args
    if( SP_IS_BAD_INTERFACE_PTR( pOutputSite ) ||
        SP_IS_BAD_READ_PTR( pTextFragList )  )
    {
        hr = E_INVALIDARG;
    }
    else
    {
        //--- Init some vars
        m_pCurrFrag   = pTextFragList;
        m_pNextChar   = m_pCurrFrag->pTextStart;
        m_pEndChar    = m_pNextChar + m_pCurrFrag->ulTextLen;
        m_ullAudioOff = 0;

        CItemList ItemList;

		m_logger->debug("Starting work processing\n");
		while( SUCCEEDED( hr ) && !(pOutputSite->GetActions() & SPVES_ABORT) )
        {
            //--- Do skip?
            if( pOutputSite->GetActions() & SPVES_SKIP )
            {
				m_logger->debug("ACTION: SKIP\n");
				long lSkipCnt;
                SPVSKIPTYPE eType;
                hr = pOutputSite->GetSkipInfo( &eType, &lSkipCnt );
                if( SUCCEEDED( hr ) )
                {
                    //--- Notify SAPI how many items we skipped. We're returning zero
                    //    because this feature isn't implemented.
                    hr = pOutputSite->CompleteSkip( 0 );
                }
            }
			
            //--- Build the text item list
            if( SUCCEEDED( hr ) && (hr = GetNextSentence( ItemList )) != S_OK )
            {
				m_logger->debug("ERROR Getting the next sentence from ItemList\n");
				break;
            }
			
            //--- We aren't going to do any part of speech determination,
            //    prosody, or pronunciation determination. If you were, one thing
            //    you will need is access to the SAPI lexicon. You can get that with
            //    the following call.
            //    CComPtr<ISpLexicon> cpLexicon;
            //    hr = pUser->GetLexicon( &cpLexicon );

            if( !(pOutputSite->GetActions() & SPVES_ABORT) )
            {
				//--- Fire begin sentence event
                CSentItem& FirstItem = ItemList.GetHead();
                CSentItem& LastItem  = ItemList.GetTail();
                CSpEvent Event;
                Event.eEventId             = SPEI_SENTENCE_BOUNDARY;
                Event.elParamType          = SPET_LPARAM_IS_UNDEFINED;
                Event.ullAudioStreamOffset = m_ullAudioOff;
                Event.lParam               = (LPARAM)FirstItem.ulItemSrcOffset;
                Event.wParam               = (WPARAM)LastItem.ulItemSrcOffset +
                                                     LastItem.ulItemSrcLen -
                                                     FirstItem.ulItemSrcOffset;
				hr = pOutputSite->AddEvents( &Event, 1 );

                //--- Output
                if( SUCCEEDED( hr ) )
                {
                    hr = OutputSentence( ItemList, pOutputSite );
					return hr;
                }
            }
        }

        //--- S_FALSE just says that we hit the end, return okay
        if( hr == S_FALSE )
        {
            hr = S_OK;
        }
    }
    return hr;
} /* CTTSEngObj::Speak */

/*****************************************************************************
* CTTSEngObj::OutputSentence *
*----------------------------*
*   This method is used to output an item list.
****************************************************************************/
HRESULT CTTSEngObj::OutputSentence( CItemList& ItemList, ISpTTSEngineSite* pOutputSite )
{
    HRESULT hr = S_OK;
//    ULONG WordIndex;
	m_logger->debug(__FUNCTION__);

    //--- Lookup words in our voice
    SPLISTPOS ListPos = ItemList.GetHeadPosition();
	CSentItem& Item = ItemList.GetNext(ListPos);
	DescribeVoicesRequest request;
	std::string speech = CW2A(Item.pItem);
    std::map<char, std::string> transformations;
    transformations['&'] = std::string("&amp;");
    transformations['\''] = std::string("&apos;");
    transformations['"'] = std::string("&quot;");
    transformations['>'] = std::string("&gt;");
    transformations['<'] = std::string("&lt;");
    std::string reserved_chars;
    for (auto ti = transformations.begin(); ti != transformations.end(); ti++)
    {
        reserved_chars += ti->first;
    }

    size_t pos = 0;
    while (std::string::npos != (pos = speech.find_first_of(reserved_chars, pos)))
    {
        speech.replace(pos, 1, transformations[speech[pos]]);
        pos++;
    };
	boost::trim(speech);

	if (speech.find("<voice name=") != std::string::npos)
	{
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError res = doc.Parse(speech.c_str());
		auto voice_node = doc.RootElement()->FirstChildElement();
		auto voice_name = voice_node->Attribute("name");
		mbstowcs(m_voiceOveride, voice_name, strlen(voice_name) + 1);
		m_pPollyVoice = m_voiceOveride;
	}

	if (speech.find("<voice name=") != std::string::npos ||
		!speech.compare("</speak>"))
	{
		return S_OK;
	}

	ListPos = ItemList.GetHeadPosition();
	PollyManager pm = PollyManager(m_pPollyVoice, m_isNeural, m_isNews, m_isConversational);
	auto resp = pm.GenerateSpeech(Item);
	if (!resp.IsSuccess)
	{
		std::stringstream message;
		message << "Error generating speech:\n\n" << resp.ErrorMessage << "\nSpeech:\n" << speech;
		MessageBoxA(NULL, message.str().c_str(), "Error", MB_OK);
		return FAILED(ERROR_SUCCESS);
	}
	PollySpeechMarksResponse generateSpeechMarksResp = pm.GenerateSpeechMarks(Item, resp.Length);
	
	hr = pOutputSite->Write(reinterpret_cast<char*>(&resp.AudioData[0]), resp.Length, NULL);
	return hr;
	auto i = generateSpeechMarksResp.SpeechMarks.begin();
	auto wordOffset = 0;
    while(ListPos && i != generateSpeechMarksResp.SpeechMarks.end() && !(pOutputSite->GetActions() & SPVES_ABORT) )
    {
		SpeechMark sm = *i;
        CSentItem& Item = ItemList.GetNext( ListPos );
		m_logger->debug("ListPos={}, current word={}", ListPos, sm.Text);


        //--- Process sentence items
		switch( Item.pXmlState->eAction )
        {
          //--- Speak some text ---------------------------------------
          case SPVA_Speak:
          {
            if( iswalpha( Item.pItem[0] ) || iswdigit( Item.pItem[0] ) )
            {
				//--- Queue the event
                CSpEvent Event;
                Event.eEventId             = SPEI_WORD_BOUNDARY;
                Event.elParamType          = SPET_LPARAM_IS_UNDEFINED;
                Event.ullAudioStreamOffset = wordOffset;
				Event.lParam               = Item.ulItemSrcOffset,
                Event.wParam               = sm.Text.length();
				m_logger->debug("Writing word boundary for '{}', offset={}, length={}", sm.Text, Item.ulItemSrcOffset, sm.Text.length());
                pOutputSite->AddEvents( &Event, 1 );

				std::vector<unsigned char> word = std::vector<unsigned char>(&resp.AudioData[wordOffset], &resp.AudioData[wordOffset + sm.LengthInBytes]);
				hr = pOutputSite->Write(reinterpret_cast<char*>(&word[0]), sm.LengthInBytes, NULL);
				++i;
				m_ullAudioOff += sm.LengthInBytes;
				wordOffset += sm.LengthInBytes;
			}
          }
          break;

          //--- Output some silence for a pause -----------------------
          case SPVA_Silence:
          {
            BYTE Buff[1000];
            memset( Buff, 0, 1000 );
            ULONG NumSilenceBytes = Item.pXmlState->SilenceMSecs * 22;

            //--- Queue the audio data in chunks so that we can get
            //    interrupted if necessary.
            while( !(pOutputSite->GetActions() & SPVES_ABORT) )
            {
                if( NumSilenceBytes > 1000 )
                {
                    hr = pOutputSite->Write( Buff, 1000, NULL );
                    NumSilenceBytes -= 1000;
                }
                else
                {
                    hr = pOutputSite->Write( Buff, NumSilenceBytes, NULL );
                    break;
                }
            }

            //--- Update the audio offset
            m_ullAudioOff += NumSilenceBytes;
          }
          break;

          //--- Fire a bookmark event ---------------------------------
          case SPVA_Bookmark:
          {
            //--- The bookmark is NOT a null terminated string in the Item, but we need
            //--- to convert it to one.  Allocate enough space for the string.
            WCHAR * pszBookmark = (WCHAR *)_malloca((Item.ulItemLen + 1) * sizeof(WCHAR));
            memcpy(pszBookmark, Item.pItem, Item.ulItemLen * sizeof(WCHAR));
            pszBookmark[Item.ulItemLen] = 0;
            //--- Queue the event
            SPEVENT Event;
            Event.eEventId             = SPEI_TTS_BOOKMARK;
            Event.elParamType          = SPET_LPARAM_IS_STRING;
            Event.ullAudioStreamOffset = m_ullAudioOff;
            Event.lParam               = (LPARAM)pszBookmark;
            Event.wParam               = _wtol(pszBookmark);
            hr = pOutputSite->AddEvents( &Event, 1 );
            //--- Free the space for the string.
            _freea(pszBookmark);
          }
          break;

          case SPVA_Pronounce:
            //--- Our sample engine doesn't handle this. If it
            //    did, you would use the associated pronunciation in
            //    the XmlState structure instead of the lexicon.
            break;

          case SPVA_ParseUnknownTag:
            //--- This will reference an XML tag that is unknown to SAPI
            //    if your engine has private tags to control state, you
            //    would examine these tags and see if you recognize it. This
            //    would also be the point that you would make the rendering
            //    state change.
            break;
        }
    }

    return hr;
} /* CTTSEngObj::OutputSentence */

/*****************************************************************************
* CTTSEngObj::GetVoiceFormat *
*----------------------------*
*   Description:
*       This method returns the output data format associated with the
*   specified format Index. Formats are in order of quality with the best
*   starting at 0.
*****************************************************************************/
STDMETHODIMP CTTSEngObj::GetOutputFormat( const GUID * pTargetFormatId, const WAVEFORMATEX * pTargetWaveFormatEx,
                                          GUID * pDesiredFormatId, WAVEFORMATEX ** ppCoMemDesiredWaveFormatEx )
{

    HRESULT hr = S_OK;

    hr = SpConvertStreamFormatEnum(SPSF_16kHz16BitMono, pDesiredFormatId, ppCoMemDesiredWaveFormatEx);

	m_logger->debug("End Speak");
	return hr;
} /* CTTSEngObj::GetVoiceFormat */

//
//=== This code is just a simplified parser ==================================
//
/*****************************************************************************
* CTTSEngObj::GetNextSentence *
*-----------------------------*
*   This method is used to create a list of items to be spoken.
****************************************************************************/
HRESULT CTTSEngObj::GetNextSentence( CItemList& ItemList )
{
    HRESULT hr = S_OK;
	m_logger->debug(__FUNCTION__);
	m_logger->debug("Clearing the item list\n");
	//--- Clear all items in the list
    ItemList.RemoveAll();

    //--- Is there any work to do
    if( m_pCurrFrag == NULL )
    {
		m_logger->debug("CurrFrag is null, nothing to do");
		hr = S_FALSE;
    }
    else
    {
        BOOL fSentDone = false;
        BOOL fGoToNextFrag = false;

        while( m_pCurrFrag && !fSentDone )
        {
            if( m_pCurrFrag->State.eAction == SPVA_Speak )
            {
                fSentDone = AddNextSentenceItem( ItemList );

                //--- Advance fragment?
                if( m_pNextChar >= m_pEndChar )
                {
                    fGoToNextFrag = true;
                }
            }
            else
            {
                //--- Add non spoken fragments
                CSentItem Item;
                Item.pItem           = m_pCurrFrag->pTextStart;
                Item.ulItemLen       = m_pCurrFrag->ulTextLen;
                Item.ulItemSrcOffset = m_pCurrFrag->ulTextSrcOffset;
                Item.ulItemSrcLen    = Item.ulItemLen;
                Item.pXmlState       = &m_pCurrFrag->State;
                ItemList.AddTail( Item );
                fGoToNextFrag = true;
            }

            if( fGoToNextFrag )
            {
                fGoToNextFrag = false;
                m_pCurrFrag = m_pCurrFrag->pNext;
                if( m_pCurrFrag )
                {
                    m_pNextChar = m_pCurrFrag->pTextStart;
                    m_pEndChar  = m_pNextChar + m_pCurrFrag->ulTextLen;
                }
                else
                {
                    m_pNextChar = NULL;
                    m_pEndChar  = NULL;
                }
            }
        } // end while

        if( ItemList.IsEmpty() )
        {
            hr = S_FALSE;
        }
    }
    return hr;
} /* CTTSEngObj::GetNextSentence */

/*****************************************************************************
* IsSpace *
*---------*
*   Returns true if the character is a space, tab, carriage return, or line feed.
****************************************************************************/
static BOOL IsSpace( WCHAR wc )
{
    return ( ( wc == 0x20 ) || ( wc == 0x9 ) || ( wc == 0xD  ) || ( wc == 0xA ) );
}

/*****************************************************************************
* SkipWhiteSpace *
*----------------*
*   Returns the position of the next non-whitespace character.
****************************************************************************/
static const WCHAR* SkipWhiteSpace( const WCHAR* pPos )
{
    while( IsSpace( *pPos ) ) ++pPos;
    return pPos;
}

/*****************************************************************************
* FindNextToken *
*---------------*
*   Locates the next space delimited token in the stream
****************************************************************************/
static const WCHAR* 
    FindNextToken( const WCHAR* pStart, const WCHAR* pEnd, const WCHAR*& pNext )
{
    const WCHAR* pPos = SkipWhiteSpace( pStart );
    pNext = pPos;
    if( pNext == pEnd )
    {
        pPos = NULL;
    }
    else
    {
        while( *pNext && !IsSpace( *pNext ) )
        {
            if( ++pNext == pEnd )
            {
                //--- This can happen when a text fragment is
                //    tight up against a following xml tag.
                break;
            }
        }
    }
    return pPos;
} /* FindNextToken */

/*****************************************************************************
* SearchSet *
*-----------*
*   Finds the character in the specified array.
****************************************************************************/
BOOL SearchSet( WCHAR wc, const WCHAR* Set, ULONG Count, ULONG* pIndex )
{
    for( ULONG i = 0; i < Count; ++i )
    {
        if( wc == Set[i] )
        {
            *pIndex = i;
            return true;
        }
    }
    return false;
}

/*****************************************************************************
* CTTSEngObj::AddNextSentenceItem *
*-----------------------------*
*   Locates the next sentence item in the stream and adds it to the list.
*   Returns true if the last item added is the end of the sentence.
****************************************************************************/
BOOL CTTSEngObj::AddNextSentenceItem( CItemList& ItemList )
{
    //--- Get the token
    ULONG ulIndex;
    CSentItem Item;
    Item.pItem = FindNextToken( m_pNextChar, m_pEndChar, m_pNextChar );
	m_logger->debug("Next token: {}", (char *)Item.pItem);

    //--- This case can occur when we hit the end of a text fragment.
    //    Returning at this point will cause advancement to the next fragment.
    if( Item.pItem == NULL )
    {
		return false;
    }

    const WCHAR* pTrailChar = m_pNextChar-1;
    ULONG TokenLen = (ULONG)(m_pNextChar - Item.pItem);

    //--- Split off leading punction if any
    static const WCHAR LeadItems[] = { L'(', L'\"', L'{', L'\'', L'[' };
    while( TokenLen > 1 )
    {
        if( SearchSet( Item.pItem[0], LeadItems, sp_countof(LeadItems), &ulIndex ) )
        {
            CSentItem LItem;
            LItem.pItem           = Item.pItem;
            LItem.ulItemLen       = 1;
            LItem.pXmlState       = &m_pCurrFrag->State;
            LItem.ulItemSrcLen    = LItem.ulItemLen;
            LItem.ulItemSrcOffset = m_pCurrFrag->ulTextSrcOffset +
                                    (ULONG)( LItem.pItem - m_pCurrFrag->pTextStart );
            ItemList.AddTail( LItem );
            ++Item.pItem;
            --TokenLen;
        }
        else
        {
            break;
        }
    }

    //--- Get primary item insert position
    SPLISTPOS ItemPos = ItemList.AddTail( Item );

    //--- Split off trailing punction if any.
    static const WCHAR EOSItems[] = { L'.', L'!', L'?' };
    static const WCHAR TrailItems[] = { L',', L'\"', L';', L':', L')', L'}', L'\'', L']' };
    BOOL fIsEOS = false;
    while( TokenLen > 1 )
    {
        BOOL fAddTrailItem = false;
        if( SearchSet( *pTrailChar, EOSItems, sp_countof(EOSItems), &ulIndex ) )
        {
		m_logger->debug("Next token: {}", (char *)Item.pItem);
            fIsEOS = true;
            fAddTrailItem = true;
        }
        else if( SearchSet( *pTrailChar, TrailItems, sp_countof(TrailItems), &ulIndex ) )
        {
			fAddTrailItem = true;
        }

        if( fAddTrailItem )
        {
			wchar_t newstr[2];
			newstr[0] = *pTrailChar;
			newstr[1] = 0;

			m_logger->debug("Found trailing token: {}", (char)newstr);
			CSentItem TItem;
			TItem.pItem			  = newstr;
            TItem.ulItemLen       = 1;
            TItem.pXmlState       = &m_pCurrFrag->State;
            TItem.ulItemSrcLen    = TItem.ulItemLen;
            TItem.ulItemSrcOffset = m_pCurrFrag->ulTextSrcOffset +
                                    (ULONG)( TItem.pItem - m_pCurrFrag->pTextStart );
            ItemList.InsertAfter( ItemPos, TItem );
            --TokenLen;
            --pTrailChar;
        }
        else
        {
            break;
        }
    }

    //--- Abreviation or sentence end?
    //    If we are at the end of the buffer then EOS is implied.
    if( *m_pNextChar == NULL )
    {
        fIsEOS = true;
        if( !SearchSet( *(m_pNextChar-1), EOSItems, sp_countof(EOSItems), &ulIndex ) )
        {
            //--- Terminate with a period if we are at the end of a buffer
            //    and no end of sentence punction has been added.
            static const WCHAR* pPeriod = L".";
            CSentItem EOSItem;
            EOSItem.pItem           = pPeriod;
            EOSItem.ulItemLen       = 1;
            EOSItem.pXmlState       = &m_pCurrFrag->State;
            EOSItem.ulItemSrcLen    = EOSItem.ulItemLen;
            EOSItem.ulItemSrcOffset = m_pCurrFrag->ulTextSrcOffset +
                                    (ULONG)( (m_pNextChar-1) - m_pCurrFrag->pTextStart );
            ItemList.AddTail( EOSItem );
        }
    }
    else if( pTrailChar[1] == L'.' )
    {
        //--- Here is where you would try to prove that it's not EOS
        //    It might be an abreviation. That's a hard problem that
        //    we are not going to attempt here.
    }
    
    //--- Add the main item
    if( TokenLen > 0 )
    {
        Item.ulItemLen       = TokenLen;
        Item.pXmlState       = &m_pCurrFrag->State;
        Item.ulItemSrcLen    = Item.ulItemLen;
        Item.ulItemSrcOffset = m_pCurrFrag->ulTextSrcOffset +
                               (ULONG)( Item.pItem - m_pCurrFrag->pTextStart );
        ItemList.SetAt( ItemPos, Item );
    }

    return fIsEOS;
} /* CTTSEngObj::AddNextSentenceItem */

