using System;
using System.Drawing;
using System.Reflection;
using System.Resources;
using System.Windows.Forms;

namespace Updater
{
    public partial class FormUpdate : Form, UpdateListener
    {

        private readonly ResourceManager rm;
        private bool expanded = false;

        public FormUpdate()
        {
            rm = new ResourceManager("Updater.Properties.UpdateResource", typeof(FormUpdate).Assembly);
            InitializeComponent();
        }

        private void StartUpdate()
        {
            UpdateHelper helper = new UpdateHelper
            {
                Listener = this
            };
            helper.StartUpdate();
        }

        public void HandleUpdateEvent(string text)
        {
            if (text == null)
            {
                return;
            }

            BeginInvoke(new Action(() =>
            {
                progressBox.AppendText(text);
                progressBox.AppendText(Environment.NewLine);
            }));
        }

        public void SetProgress(int percentage)
        {
            BeginInvoke(new Action(() =>
            {
                progressBar.Value = percentage;
            }));
        }

        private void FormUpdate_Load(object sender, EventArgs e)
        {
        }

        private void Button1_Click(object sender, EventArgs e)
        {
            StartUpdate();
        }

        private void ShowInformation_Click(object sender, EventArgs e)
        {
            progressBox.Visible = !expanded;
            String line;

            if (!expanded)
            {
                // expand
                line = rm.GetString("lessInformation");
                if (Height < 500)
                {
                    Height = 500;
                }
            }
            else
            {
                // collapse
                line = rm.GetString("moreInformation");
                Height = 200;
                Width = 500;
            }

            showInformation.Text = line;

            expanded = !expanded;

        }

        public void ExecuteUpdate()
        {
            updateProgressLabel("executeUpdate");
        }

        public void Exit(int exitCode)
        {
            if (exitCode == 0)
            {
                updateProgressLabel("updateSuccess");
            }
            else
            {
                updateProgressLabel("updateError");
            }
        }

        private void updateProgressLabel(string textId)
        {
            BeginInvoke(new Action(() =>
            {
                progressLabel.Text = rm.GetString(textId);
            }));
        }

    }

    interface UpdateListener
    {
        void HandleUpdateEvent(string text);
        void SetProgress(int percentage);

        void ExecuteUpdate();

        void Exit(int exitCode);

    }

}
