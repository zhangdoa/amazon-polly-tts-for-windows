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
    public partial class mainForm : Form
    {
        public mainForm()
        {
            InitializeComponent();
        }

        private void LoadVoices()
        {
            voicesListBox.DisplayMember = "Description";
            using (SpeechSynthesizer synth = new SpeechSynthesizer())
            {
                if (synth.GetInstalledVoices().Count == 0)
                    return;
                // Output information about all of the installed voices.   
                foreach (InstalledVoice voice in synth.GetInstalledVoices())
                {
                    voicesListBox.Items.Add(voice.VoiceInfo);
                }
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            LoadVoices();
        }

        private void SayItButton_Click(object sender, EventArgs e)
        {
            using (SpeechSynthesizer synth = new SpeechSynthesizer())
            {
                var voice = (VoiceInfo) voicesListBox.SelectedItem;
                Debug.WriteLine($"Selected Voice: {0}", voice.Description);
                synth.SelectVoice(voice.Name);
                synth.Speak(speechTextBox.Text);
            }
        }
    }
}
