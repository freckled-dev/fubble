using System;
using System.Diagnostics;
using System.Configuration;
using System.Text.RegularExpressions;

namespace Updater
{
    public class UpdateHelper
    {

        internal UpdateListener Listener { get; set; }

        public Boolean StartUpdate()
        {
            Listener.SetProgress(0);

            ProcessStartInfo startInfo = CreateStartInfo();
            Process process = CreateProcess(startInfo);

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

        private Process CreateProcess(ProcessStartInfo startInfo)
        {
            Process process = new Process
            {
                StartInfo = startInfo
            };

            process.OutputDataReceived += new DataReceivedEventHandler(OutputHandler);
            process.ErrorDataReceived += new DataReceivedEventHandler(OutputHandler);
            process.EnableRaisingEvents = true;
            process.Exited += new EventHandler(ExitHandler);

            return process;
        }

        private ProcessStartInfo CreateStartInfo()
        {
            string url = GetUpdateUrl();
            string rclonePath = ConfigurationManager.AppSettings.Get("RClonePath");
            string execParams = ConfigurationManager.AppSettings.Get("Params");
            string command = ConfigurationManager.AppSettings.Get("Command");
            string targetDir = ConfigurationManager.AppSettings.Get("TargetDir");

            return new ProcessStartInfo
            {
                UseShellExecute = false,
                FileName = rclonePath,
                CreateNoWindow = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                Arguments = $"{command} {execParams} --http-url {url} {targetDir}"
            };
        }

        private string GetUpdateUrl()
        {
            string host = ConfigurationManager.AppSettings.Get("Host");
            string port = ConfigurationManager.AppSettings.Get("Port");
            return $"{host}:{port}";
        }

        private void HandleUpdateException(string message)
        {
            Listener.HandleUpdateEvent(message);
        }

        private void OutputHandler(object sendingProcess, DataReceivedEventArgs outLine)
        {
            string line = outLine.Data;

            if (IsStatusLine(line))
            {
                ReportPercentage(line);
            }

            Listener.HandleUpdateEvent(line);
        }

        private void ReportPercentage(string line)
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

        private bool IsStatusLine(string line)
        {
            return line != null && (line.Contains("Transferred:") || line.Contains("Checks:"));
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
