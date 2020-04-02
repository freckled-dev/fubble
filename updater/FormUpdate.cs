using System;
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


        public void HandleUpdateException(string text)
        {
            HandleUpdateEvent(text);
            UpdateProgressLabel("updateError");
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
                Height = 220;
                Width = 500;
            }

            showInformation.Text = line;

            expanded = !expanded;

        }

        public void ExecuteUpdate()
        {
            UpdateProgressLabel("executeUpdate");
        }

        public void Exit(int exitCode)
        {
            if (exitCode == 0)
            {
                UpdateProgressLabel("updateSuccess");
            }
            else
            {
                UpdateProgressLabel("updateError");
            }
        }

        private void UpdateProgressLabel(string textId)
        {
            UpdateProgressLabel(textId, null);
        }

        private void UpdateProgressLabel(string textId, string parameter)
        {
            BeginInvoke(new Action(() =>
            {
                if (parameter != null)
                {
                    progressLabel.Text = String.Format(rm.GetString(textId), parameter);
                }
                else
                {
                    progressLabel.Text = rm.GetString(textId);
                }
            }));
        }

        public void FubbleStart(Exception ex)
        {
            if (ex != null)
            {
                UpdateProgressLabel("fubbleStartError", ex.Message);
            }
            else
            {
                UpdateProgressLabel("fubbleStart");
            }
        }
    }

    interface UpdateListener
    {
        void HandleUpdateEvent(string text);
        void SetProgress(int percentage);

        void ExecuteUpdate();

        void Exit(int exitCode);

        void FubbleStart(Exception ex);

        void HandleUpdateException(string message);
    }

}
