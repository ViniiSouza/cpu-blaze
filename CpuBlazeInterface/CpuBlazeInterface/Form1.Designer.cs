namespace CpuBlazeInterface
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            lblTitle = new Label();
            lblCoreSelection = new Label();
            cmbCoreSelection = new ComboBox();
            lblSelectedCores = new Label();
            txtSelectedCores = new TextBox();
            lblMode = new Label();
            rbModeSingle = new RadioButton();
            rbModeMulti = new RadioButton();
            lblPercent = new Label();
            numPercent = new NumericUpDown();
            lblTime = new Label();
            numTimeMinutes = new NumericUpDown();
            lblRotation = new Label();
            chkRotation = new CheckBox();
            numRotationSeconds = new NumericUpDown();
            btnStart = new Button();
            btnStop = new Button();
            txtStatus = new RichTextBox();
            lblStatus = new Label();
            ((System.ComponentModel.ISupportInitialize)numPercent).BeginInit();
            ((System.ComponentModel.ISupportInitialize)numTimeMinutes).BeginInit();
            ((System.ComponentModel.ISupportInitialize)numRotationSeconds).BeginInit();
            SuspendLayout();
            // 
            // lblTitle
            // 
            lblTitle.AutoSize = true;
            lblTitle.Font = new Font("Segoe UI", 16F, FontStyle.Bold);
            lblTitle.Location = new Point(12, 9);
            lblTitle.Name = "lblTitle";
            lblTitle.Size = new Size(118, 30);
            lblTitle.TabIndex = 0;
            lblTitle.Text = "CPU Blaze";
            // 
            // lblCoreSelection
            // 
            lblCoreSelection.AutoSize = true;
            lblCoreSelection.Location = new Point(12, 50);
            lblCoreSelection.Name = "lblCoreSelection";
            lblCoreSelection.Size = new Size(112, 15);
            lblCoreSelection.TabIndex = 1;
            lblCoreSelection.Text = "Seleção de Núcleos:";
            // 
            // cmbCoreSelection
            // 
            cmbCoreSelection.DropDownStyle = ComboBoxStyle.DropDownList;
            cmbCoreSelection.FormattingEnabled = true;
            cmbCoreSelection.Location = new Point(12, 68);
            cmbCoreSelection.Name = "cmbCoreSelection";
            cmbCoreSelection.Size = new Size(200, 23);
            cmbCoreSelection.TabIndex = 2;
            cmbCoreSelection.SelectedIndexChanged += CmbCoreSelection_SelectedIndexChanged;
            // 
            // lblSelectedCores
            // 
            lblSelectedCores.AutoSize = true;
            lblSelectedCores.Location = new Point(12, 104);
            lblSelectedCores.Name = "lblSelectedCores";
            lblSelectedCores.Size = new Size(97, 15);
            lblSelectedCores.TabIndex = 3;
            lblSelectedCores.Text = "Núcleos (0,1,2...):";
            // 
            // txtSelectedCores
            // 
            txtSelectedCores.Enabled = false;
            txtSelectedCores.Location = new Point(12, 122);
            txtSelectedCores.Name = "txtSelectedCores";
            txtSelectedCores.Size = new Size(200, 23);
            txtSelectedCores.TabIndex = 4;
            // 
            // lblMode
            // 
            lblMode.AutoSize = true;
            lblMode.Location = new Point(12, 158);
            lblMode.Name = "lblMode";
            lblMode.Size = new Size(42, 15);
            lblMode.TabIndex = 5;
            lblMode.Text = "Modo:";
            // 
            // rbModeSingle
            // 
            rbModeSingle.AutoSize = true;
            rbModeSingle.Checked = true;
            rbModeSingle.Location = new Point(12, 176);
            rbModeSingle.Name = "rbModeSingle";
            rbModeSingle.Size = new Size(57, 19);
            rbModeSingle.TabIndex = 6;
            rbModeSingle.TabStop = true;
            rbModeSingle.Text = "Single";
            rbModeSingle.UseVisualStyleBackColor = true;
            // 
            // rbModeMulti
            // 
            rbModeMulti.AutoSize = true;
            rbModeMulti.Location = new Point(76, 176);
            rbModeMulti.Name = "rbModeMulti";
            rbModeMulti.Size = new Size(53, 19);
            rbModeMulti.TabIndex = 7;
            rbModeMulti.Text = "Multi";
            rbModeMulti.UseVisualStyleBackColor = true;
            // 
            // lblPercent
            // 
            lblPercent.AutoSize = true;
            lblPercent.Location = new Point(12, 208);
            lblPercent.Name = "lblPercent";
            lblPercent.Size = new Size(131, 15);
            lblPercent.TabIndex = 8;
            lblPercent.Text = "Porcentagem (0-100%):";
            // 
            // numPercent
            // 
            numPercent.Location = new Point(12, 226);
            numPercent.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
            numPercent.Name = "numPercent";
            numPercent.Size = new Size(120, 23);
            numPercent.TabIndex = 9;
            numPercent.Value = new decimal(new int[] { 100, 0, 0, 0 });
            // 
            // lblTime
            // 
            lblTime.AutoSize = true;
            lblTime.Location = new Point(12, 262);
            lblTime.Name = "lblTime";
            lblTime.Size = new Size(102, 15);
            lblTime.TabIndex = 10;
            lblTime.Text = "Tempo (minutos):";
            // 
            // numTimeMinutes
            // 
            numTimeMinutes.DecimalPlaces = 2;
            numTimeMinutes.Increment = new decimal(new int[] { 1, 0, 0, 131072 });
            numTimeMinutes.Location = new Point(12, 280);
            numTimeMinutes.Maximum = new decimal(new int[] { 9999, 0, 0, 0 });
            numTimeMinutes.Minimum = new decimal(new int[] { 1, 0, 0, 131072 });
            numTimeMinutes.Name = "numTimeMinutes";
            numTimeMinutes.Size = new Size(120, 23);
            numTimeMinutes.TabIndex = 11;
            numTimeMinutes.Value = new decimal(new int[] { 1, 0, 0, 0 });
            // 
            // lblRotation
            // 
            lblRotation.AutoSize = true;
            lblRotation.Location = new Point(12, 316);
            lblRotation.Name = "lblRotation";
            lblRotation.Size = new Size(218, 15);
            lblRotation.TabIndex = 12;
            lblRotation.Text = "Rotação de Núcleos (segundos/núcleo):";
            // 
            // chkRotation
            // 
            chkRotation.AutoSize = true;
            chkRotation.Location = new Point(12, 334);
            chkRotation.Name = "chkRotation";
            chkRotation.Size = new Size(103, 19);
            chkRotation.TabIndex = 13;
            chkRotation.Text = "Ativar Rotação";
            chkRotation.UseVisualStyleBackColor = true;
            chkRotation.CheckedChanged += ChkRotation_CheckedChanged;
            // 
            // numRotationSeconds
            // 
            numRotationSeconds.Enabled = false;
            numRotationSeconds.Location = new Point(136, 332);
            numRotationSeconds.Maximum = new decimal(new int[] { 3600, 0, 0, 0 });
            numRotationSeconds.Minimum = new decimal(new int[] { 1, 0, 0, 0 });
            numRotationSeconds.Name = "numRotationSeconds";
            numRotationSeconds.Size = new Size(76, 23);
            numRotationSeconds.TabIndex = 14;
            numRotationSeconds.Value = new decimal(new int[] { 10, 0, 0, 0 });
            // 
            // btnStart
            // 
            btnStart.Location = new Point(12, 370);
            btnStart.Name = "btnStart";
            btnStart.Size = new Size(100, 35);
            btnStart.TabIndex = 15;
            btnStart.Text = "Iniciar";
            btnStart.UseVisualStyleBackColor = true;
            btnStart.Click += BtnStart_Click;
            // 
            // btnStop
            // 
            btnStop.Enabled = false;
            btnStop.Location = new Point(118, 370);
            btnStop.Name = "btnStop";
            btnStop.Size = new Size(100, 35);
            btnStop.TabIndex = 16;
            btnStop.Text = "Parar";
            btnStop.UseVisualStyleBackColor = true;
            btnStop.Click += BtnStop_Click;
            // 
            // txtStatus
            // 
            txtStatus.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            txtStatus.Location = new Point(240, 68);
            txtStatus.Name = "txtStatus";
            txtStatus.ReadOnly = true;
            txtStatus.Size = new Size(548, 337);
            txtStatus.TabIndex = 17;
            txtStatus.Text = "";
            // 
            // lblStatus
            // 
            lblStatus.AutoSize = true;
            lblStatus.Location = new Point(240, 50);
            lblStatus.Name = "lblStatus";
            lblStatus.Size = new Size(42, 15);
            lblStatus.TabIndex = 18;
            lblStatus.Text = "Status:";
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(800, 417);
            Controls.Add(lblStatus);
            Controls.Add(txtStatus);
            Controls.Add(btnStop);
            Controls.Add(btnStart);
            Controls.Add(numRotationSeconds);
            Controls.Add(chkRotation);
            Controls.Add(lblRotation);
            Controls.Add(numTimeMinutes);
            Controls.Add(lblTime);
            Controls.Add(numPercent);
            Controls.Add(lblPercent);
            Controls.Add(rbModeMulti);
            Controls.Add(rbModeSingle);
            Controls.Add(lblMode);
            Controls.Add(txtSelectedCores);
            Controls.Add(lblSelectedCores);
            Controls.Add(cmbCoreSelection);
            Controls.Add(lblCoreSelection);
            Controls.Add(lblTitle);
            MinimumSize = new Size(816, 456);
            Name = "Form1";
            Text = "CPU Blaze - Stress Tester";
            Load += Form1_Load;
            ((System.ComponentModel.ISupportInitialize)numPercent).EndInit();
            ((System.ComponentModel.ISupportInitialize)numTimeMinutes).EndInit();
            ((System.ComponentModel.ISupportInitialize)numRotationSeconds).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label lblTitle;
        private Label lblCoreSelection;
        private ComboBox cmbCoreSelection;
        private Label lblSelectedCores;
        private TextBox txtSelectedCores;
        private Label lblMode;
        private RadioButton rbModeSingle;
        private RadioButton rbModeMulti;
        private Label lblPercent;
        private NumericUpDown numPercent;
        private Label lblTime;
        private NumericUpDown numTimeMinutes;
        private Label lblRotation;
        private CheckBox chkRotation;
        private NumericUpDown numRotationSeconds;
        private Button btnStart;
        private Button btnStop;
        private RichTextBox txtStatus;
        private Label lblStatus;
    }
}
