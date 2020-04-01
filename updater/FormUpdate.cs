using System;
using System.Windows.Forms;

namespace Updater
{
    public partial class FormUpdate : Form, UpdateListener
    {
        public FormUpdate()
        {
            InitializeComponent();
        }

        private void startUpdate()
        {
            UpdateHelper helper = new UpdateHelper();
            helper.Listener = this;
            helper.StartUpdate();
        }

        public void handleUpdateEvent(string text)
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

        public void setProgress(int percentage)
        {
            progressBar.Value = percentage;
        }

        private void FormUpdate_Load(object sender, EventArgs e)
        {
            startUpdate();
        }
    }

    interface UpdateListener
    {
        void handleUpdateEvent(string text);
        void setProgress(int percentage);
    }

}
