using CircularBuffer;

namespace Monitor
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            this.sendRecLabel = new System.Windows.Forms.Label();
            this.viewBox = new System.Windows.Forms.TextBox();
            this.comBox = new System.Windows.Forms.ComboBox();
            this.baudRateBox = new System.Windows.Forms.ComboBox();
            this.COMLabel = new System.Windows.Forms.Label();
            this.baudRateLabel = new System.Windows.Forms.Label();
            this.openBtn = new System.Windows.Forms.Button();
            this.closeBtn = new System.Windows.Forms.Button();
            this.serialPort = new System.IO.Ports.SerialPort(this.components);
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.statusLabel = new System.Windows.Forms.Label();
            this.viewStatusLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // sendRecLabel
            // 
            this.sendRecLabel.AutoSize = true;
            this.sendRecLabel.Location = new System.Drawing.Point(16, 11);
            this.sendRecLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.sendRecLabel.Name = "sendRecLabel";
            this.sendRecLabel.Size = new System.Drawing.Size(136, 17);
            this.sendRecLabel.TabIndex = 0;
            this.sendRecLabel.Text = "Send/Received data";
            // 
            // viewBox
            // 
            this.viewBox.Location = new System.Drawing.Point(25, 34);
            this.viewBox.Margin = new System.Windows.Forms.Padding(4);
            this.viewBox.Multiline = true;
            this.viewBox.Name = "viewBox";
            this.viewBox.Size = new System.Drawing.Size(759, 360);
            this.viewBox.TabIndex = 1;
            // 
            // comBox
            // 
            this.comBox.FormattingEnabled = true;
            this.comBox.Location = new System.Drawing.Point(809, 57);
            this.comBox.Margin = new System.Windows.Forms.Padding(4);
            this.comBox.Name = "comBox";
            this.comBox.Size = new System.Drawing.Size(160, 24);
            this.comBox.TabIndex = 2;
            // 
            // baudRateBox
            // 
            this.baudRateBox.FormattingEnabled = true;
            this.baudRateBox.Location = new System.Drawing.Point(809, 118);
            this.baudRateBox.Margin = new System.Windows.Forms.Padding(4);
            this.baudRateBox.Name = "baudRateBox";
            this.baudRateBox.Size = new System.Drawing.Size(160, 24);
            this.baudRateBox.TabIndex = 3;
            // 
            // COMLabel
            // 
            this.COMLabel.AutoSize = true;
            this.COMLabel.Location = new System.Drawing.Point(805, 37);
            this.COMLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.COMLabel.Name = "COMLabel";
            this.COMLabel.Size = new System.Drawing.Size(39, 17);
            this.COMLabel.TabIndex = 4;
            this.COMLabel.Text = "COM";
            // 
            // baudRateLabel
            // 
            this.baudRateLabel.AutoSize = true;
            this.baudRateLabel.Location = new System.Drawing.Point(805, 98);
            this.baudRateLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.baudRateLabel.Name = "baudRateLabel";
            this.baudRateLabel.Size = new System.Drawing.Size(70, 17);
            this.baudRateLabel.TabIndex = 5;
            this.baudRateLabel.Text = "Baud rate";
            // 
            // openBtn
            // 
            this.openBtn.Location = new System.Drawing.Point(837, 214);
            this.openBtn.Margin = new System.Windows.Forms.Padding(4);
            this.openBtn.Name = "openBtn";
            this.openBtn.Size = new System.Drawing.Size(100, 28);
            this.openBtn.TabIndex = 6;
            this.openBtn.Text = "Open";
            this.openBtn.UseVisualStyleBackColor = true;
            this.openBtn.Click += new System.EventHandler(this.OpenBtn_Click);
            // 
            // closeBtn
            // 
            this.closeBtn.Location = new System.Drawing.Point(837, 268);
            this.closeBtn.Margin = new System.Windows.Forms.Padding(4);
            this.closeBtn.Name = "closeBtn";
            this.closeBtn.Size = new System.Drawing.Size(100, 28);
            this.closeBtn.TabIndex = 7;
            this.closeBtn.Text = "Close";
            this.closeBtn.UseVisualStyleBackColor = true;
            this.closeBtn.Click += new System.EventHandler(this.CloseBtn_Click);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 1;
            this.timer.Tick += new System.EventHandler(this.Timer_Tick);
            // 
            // statusLabel
            // 
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(837, 326);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(48, 17);
            this.statusLabel.TabIndex = 8;
            this.statusLabel.Text = "Status";
            // 
            // viewStatusLabel
            // 
            this.viewStatusLabel.AutoSize = true;
            this.viewStatusLabel.ForeColor = System.Drawing.Color.Red;
            this.viewStatusLabel.Location = new System.Drawing.Point(839, 343);
            this.viewStatusLabel.Name = "viewStatusLabel";
            this.viewStatusLabel.Size = new System.Drawing.Size(94, 17);
            this.viewStatusLabel.TabIndex = 9;
            this.viewStatusLabel.Text = "Disconnected";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(995, 554);
            this.Controls.Add(this.viewStatusLabel);
            this.Controls.Add(this.statusLabel);
            this.Controls.Add(this.closeBtn);
            this.Controls.Add(this.openBtn);
            this.Controls.Add(this.baudRateLabel);
            this.Controls.Add(this.COMLabel);
            this.Controls.Add(this.baudRateBox);
            this.Controls.Add(this.comBox);
            this.Controls.Add(this.viewBox);
            this.Controls.Add(this.sendRecLabel);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label sendRecLabel;
        private System.Windows.Forms.TextBox viewBox;
        private System.Windows.Forms.ComboBox comBox;
        private System.Windows.Forms.ComboBox baudRateBox;
        private System.Windows.Forms.Label COMLabel;
        private System.Windows.Forms.Label baudRateLabel;
        private System.Windows.Forms.Button openBtn;
        private System.Windows.Forms.Button closeBtn;
        private System.IO.Ports.SerialPort serialPort;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.Label viewStatusLabel;
    }
}

