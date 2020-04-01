using System;
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace Updater
{
    public class UpdateHelper
    {
        private const string syncToolPath = "../../rclone.exe";
        private const string syncURL = "http://192.168.0.101:9080";

        internal UpdateListener Listener { get; set; }

        public Boolean StartUpdate()
        {
            Listener.SetProgress(0);

            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                UseShellExecute = false,
                FileName = syncToolPath,
                CreateNoWindow = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                Arguments = $"sync --progress --http-url {syncURL} :http: /bin/bla"
            };

            Process process = new Process
            {
                StartInfo = startInfo
            };

            process.OutputDataReceived += new DataReceivedEventHandler(OutputHandler);
            process.ErrorDataReceived += new DataReceivedEventHandler(OutputHandler);
            process.EnableRaisingEvents = true;
            process.Exited += new EventHandler(ExitHandler);

            try
            {
                process.Start();
                Listener.ExecuteUpdate();

                process.BeginOutputReadLine();
                process.BeginErrorReadLine();
            }
            catch (Exception ex)
            {
                HandleUpdateException(ex.Message);
            }

            return true;
        }

        private void HandleUpdateException(string message)
        {
            // Listener.setProgress(100);
            Listener.HandleUpdateEvent(message);
        }

        private void OutputHandler(object sendingProcess, DataReceivedEventArgs outLine)
        {
            string line = outLine.Data;

            if (line != null && (line.Contains("Transferred:") || line.Contains("Checks:")))
            {
                Regex rx = new Regex("\\d+(?:\\.\\d+)?%");
                MatchCollection matches = rx.Matches(line);
                // Report the number of matches found.
                if (matches.Count > 0)
                {
                    int percentage = int.Parse(matches[0].Groups[0].Value.Replace("%", ""));
                    Listener.SetProgress(percentage);
                }
            }

            Listener.HandleUpdateEvent(line);
        }

        private void ExitHandler(object sender, EventArgs e)
        {
            Process p = (Process)sender;
            int exitCode = p.ExitCode;
            Listener.Exit(exitCode);
        }

        private void LogSuccess()
        {
            //Listener.setProgress(100);
            Listener.HandleUpdateEvent("Update finished succesfully!");
        }

        private void StartFubble()
        {

        }

    }

}
