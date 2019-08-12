namespace PollyPlayer
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.voicesListBox = new System.Windows.Forms.ListBox();
            this.speechTextBox = new System.Windows.Forms.TextBox();
            this.sayItButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // voicesListBox
            // 
            this.voicesListBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.voicesListBox.FormattingEnabled = true;
            this.voicesListBox.ItemHeight = 16;
            this.voicesListBox.Location = new System.Drawing.Point(3, 3);
            this.voicesListBox.Name = "voicesListBox";
            this.voicesListBox.Size = new System.Drawing.Size(344, 532);
            this.voicesListBox.TabIndex = 0;
            // 
            // speechTextBox
            // 
            this.speechTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.speechTextBox.Location = new System.Drawing.Point(353, 3);
            this.speechTextBox.Multiline = true;
            this.speechTextBox.Name = "speechTextBox";
            this.speechTextBox.Size = new System.Drawing.Size(494, 492);
            this.speechTextBox.TabIndex = 1;
            // 
            // sayItButton
            // 
            this.sayItButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.sayItButton.Location = new System.Drawing.Point(355, 501);
            this.sayItButton.Name = "sayItButton";
            this.sayItButton.Size = new System.Drawing.Size(492, 33);
            this.sayItButton.TabIndex = 2;
            this.sayItButton.Text = "Say It";
            this.sayItButton.UseVisualStyleBackColor = true;
            this.sayItButton.Click += new System.EventHandler(this.SayItButton_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(13, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(132, 24);
            this.label1.TabIndex = 3;
            this.label1.Text = "Pick a Voice:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(354, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(230, 24);
            this.label2.TabIndex = 4;
            this.label2.Text = "Type Some Text/SSML:";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.voicesListBox);
            this.panel1.Controls.Add(this.speechTextBox);
            this.panel1.Controls.Add(this.sayItButton);
            this.panel1.Location = new System.Drawing.Point(3, 49);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(860, 544);
            this.panel1.TabIndex = 5;
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(856, 595);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.panel1);
            this.Name = "MainForm";
            this.Text = "PollyPlayer";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox voicesListBox;
        private System.Windows.Forms.TextBox speechTextBox;
        private System.Windows.Forms.Button sayItButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel panel1;
    }
}

