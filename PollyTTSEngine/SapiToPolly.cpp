#include "stdafx.h"
#include "tinyxml2.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <map>
using namespace std;

class SAPIToPolly
{
	std::map<string, string> sapi_to_polly_pron = {
		{ "aa", "A" },
		{ "ae", "{" },
		{ "ah", "V" },
		{ "ao", "O" },
		{ "aw", "aU" },
		{ "ax", "@" },
		{ "ay", "aI" },
		{ "b", "b" },
		{ "ch", "tS" },
		{ "d", "d" },
		{ "dh", "D" },
		{ "eh", "E" },
		{ "er", "3'" },
		{ "ey", "el" },
		{ "f", "f" },
		{ "g", "g" },
		{ "h", "h" },
		{ "ih", "I" },
		{ "iy", "i" },
		{ "jh", "dZ" },
		{ "k", "k" },
		{ "l", "l" },
		{ "m", "m" },
		{ "n", "n" },
		{ "ng", "N" },
		{ "ow", "oU" },
		{ "oy", "OI" },
		{ "p", "p" },
		{ "r", "r\\" },
		{ "s", "s" },
		{ "sh", "S" },
		{ "t", "t" },
		{ "th", "T" },
		{ "uh", "U" },
		{ "uw", "u" },
		{ "v", "v" },
		{ "w", "w" },
		{ "y", "j" },
		{ "z", "z" },
		{ "zh", "Z" },
		{ "1", "\"" },
		{ "2", "%" },
		{ "-", "." }
	};

private:
	tinyxml2::XMLDocument sapiDoc;
	tinyxml2::XMLDocument pollyDoc;

public:
	SAPIToPolly(const char* xml)
	{
		auto res = sapiDoc.Parse(xml);
	}

	void ToPolly()
	{
		pollyDoc.Clear();
		traverse(sapiDoc.RootElement(), NULL);
	}

	template<typename Out>
	void split(const string &s, char delim, Out result) {
		stringstream ss(s);
		string item;
		while (getline(ss, item, delim)) {
			*(result++) = item;
		}
	}

	vector<string> split(const string &s, char delim) {
		vector<string> elems;
		split(s, delim, back_inserter(elems));
		return elems;
	}

	void traverse(tinyxml2::XMLNode* sapiParent, tinyxml2::XMLNode* pollyParent)
	{
		if (!sapiParent) { return; }

		tinyxml2::XMLDocument *doc = sapiParent->ToDocument();
		if (doc)
		{
			printf("Document");
		}

		tinyxml2::XMLText *text = sapiParent->ToText();
		if (text)
		{
			auto t = pollyDoc.NewText(text->Value());
			pollyParent->LinkEndChild(t);
		}

		tinyxml2::XMLElement *element = sapiParent->ToElement();
		if (element)
		{
			tinyxml2::XMLElement *e = pollyDoc.NewElement("");
			if (strcmp(element->Value(), "pron") == 0)
			{
				auto sym = element->Attribute("sym");
				e = pollyDoc.NewElement("phoneme");
				auto chars = split(sym, ' ');
				using Iter = vector<string>::const_iterator;
				string polly_pron = "";
				for (Iter it = chars.begin(); it != chars.end(); ++it) {
					auto val = sapi_to_polly_pron.find(it->c_str());
					if (val != sapi_to_polly_pron.end())
					{
						stringstream ss(val->second.c_str());
						polly_pron = polly_pron + ss.str();
					}
					else
					{
						printf("Invalid pron: '%s'\n", it->c_str());
					}
				}
				e->SetAttribute("alphabet", "x-sampa");
				e->SetAttribute("ph", polly_pron.c_str());
			}
			if (strcmp(element->Value(), "volume") == 0)
			{
				auto vol = element->Attribute("level");
				e = pollyDoc.NewElement("prosody");
				e->SetAttribute("volume", vol);
			}
			if (pollyDoc.NoChildren() || pollyParent == NULL)
			{
				pollyDoc.LinkEndChild(e);
			}
			else
			{
				pollyParent->LinkEndChild(e);
			}
			auto sapiChild = element->FirstChild();
			while (sapiChild)
			{
				traverse(sapiChild, e);
				sapiChild = sapiChild->NextSibling();
			}
		}
	}

	void Print()
	{
		printf("SAPI:\n-----");
		sapiDoc.Print();

		printf("\n\nPolly:\n------");
		pollyDoc.Print();
	}
};
