using System;
using System.Diagnostics;

namespace Updater
{
    public class UpdateHelper
    {
        private const string syncToolPath = "../../rclone.exe";
        private const string syncURL = "http://192.168.0.101:9080";

        internal UpdateListener Listener { get; set; }

        public Boolean StartUpdate()
        {
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                UseShellExecute = false,
                FileName = syncToolPath,
                CreateNoWindow = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                //Arguments = $"sync --progress --http-url {syncURL} :http: ."
            };

            Process process = new Process
            {
                StartInfo = startInfo
            };

            process.OutputDataReceived += new DataReceivedEventHandler(OutputHandler);
            process.ErrorDataReceived += new DataReceivedEventHandler(OutputHandler);

            try
            {
                process.Start();
                process.BeginOutputReadLine();
                process.BeginErrorReadLine();
                process.WaitForExit();

                int exitCode = process.ExitCode;

                if (exitCode == 0)
                {
                    logSuccess();
                    startFubble();
                }
                else
                {
                    handleUpdateException("Exit code " + exitCode);
                }
            }
            catch (Exception ex)
            {
                handleUpdateException(ex.Message);
            }

            return true;
        }

        private void handleUpdateException(string message)
        {
            Listener.setProgress(100);
            Listener.handleUpdateEvent(message);
        }

        private void OutputHandler(object sendingProcess, DataReceivedEventArgs outLine)
        {
            Listener.handleUpdateEvent(outLine.Data);
        }

        private void logSuccess()
        {
            Listener.setProgress(100);
            Listener.handleUpdateEvent("Update finished succesfully!");
        }

        private void startFubble()
        {

        }

    }

}
