namespace Updater
{
    partial class FormUpdate
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormUpdate));
            this.progressLabel = new System.Windows.Forms.Label();
            this.progressBar = new System.Windows.Forms.ProgressBar();
            this.progressBox = new System.Windows.Forms.TextBox();
            this.update = new System.Windows.Forms.Button();
            this.showInformation = new System.Windows.Forms.Label();
            this.divider = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // progressLabel
            // 
            resources.ApplyResources(this.progressLabel, "progressLabel");
            this.progressLabel.Name = "progressLabel";
            // 
            // progressBar
            // 
            resources.ApplyResources(this.progressBar, "progressBar");
            this.progressBar.Name = "progressBar";
            this.progressBar.Step = 1;
            // 
            // progressBox
            // 
            resources.ApplyResources(this.progressBox, "progressBox");
            this.progressBox.Name = "progressBox";
            // 
            // update
            // 
            resources.ApplyResources(this.update, "update");
            this.update.Name = "update";
            this.update.UseVisualStyleBackColor = true;
            this.update.Click += new System.EventHandler(this.Button1_Click);
            // 
            // showInformation
            // 
            resources.ApplyResources(this.showInformation, "showInformation");
            this.showInformation.ForeColor = System.Drawing.SystemColors.HotTrack;
            this.showInformation.Name = "showInformation";
            this.showInformation.Click += new System.EventHandler(this.ShowInformation_Click);
            // 
            // divider
            // 
            resources.ApplyResources(this.divider, "divider");
            this.divider.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.divider.Name = "divider";
            // 
            // FormUpdate
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.divider);
            this.Controls.Add(this.showInformation);
            this.Controls.Add(this.update);
            this.Controls.Add(this.progressBox);
            this.Controls.Add(this.progressBar);
            this.Controls.Add(this.progressLabel);
            this.Name = "FormUpdate";
            this.Load += new System.EventHandler(this.FormUpdate_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label progressLabel;
        private System.Windows.Forms.ProgressBar progressBar;
        private System.Windows.Forms.TextBox progressBox;
        private System.Windows.Forms.Button update;
        private System.Windows.Forms.Label showInformation;
        private System.Windows.Forms.Label divider;
    }
}

