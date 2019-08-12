using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Reflection.Emit;
using System.Speech.Synthesis;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
//using System.Windows.Forms;

namespace PollyPlayer
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void LoadVoices()
        {
            voicesListBox.DisplayMember = "Description";
            voicesListBox.Sorted = true;
            using (var synth = new SpeechSynthesizer())
            {
                if (synth.GetInstalledVoices().Count == 0)
                    return;
                // Output information about all of the installed voices.   
                foreach (var voice in synth.GetInstalledVoices())
                {
                    if (voice.VoiceInfo.Name.StartsWith("Amazon Polly"))
                    {
                        voicesListBox.Items.Add(voice.VoiceInfo);
                    }
                }
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            LoadVoices();
        }

        private void SayItButton_Click(object sender, EventArgs e)
        {
            using (var synth = new SpeechSynthesizer())
            {
                var voice = (VoiceInfo) voicesListBox.SelectedItem;
                synth.SelectVoice(voice.Name);
                synth.Speak(speechTextBox.Text);
            }
        }
    }
}
