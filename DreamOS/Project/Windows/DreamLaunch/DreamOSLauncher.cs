using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Windows;
using System.Diagnostics;
using System.IO;

namespace DreamLaunch {

    class DreamOSLauncher {

        public static string k_strDreamClientFilename = "DreamClient.exe";

        public int Initialize() {

            return 0;
        }

        public int SetMainWindowController(MainWindow mainWindow) {
            m_mainWindow = mainWindow;
            return 0;
        }

        public void UpdateDreamOSExecutablePath() {
            m_strDreamOSPath = Environment.CurrentDirectory + "\\" + k_strDreamClientFilename;
        }

        private bool DreamOSExecutableFound() {
            UpdateDreamOSExecutablePath();

            return File.Exists(m_strDreamOSPath);
        }

        // This will simply launch DreamOS as pass through
        // TODO: Set up custom arguments 
        public int LaunchDreamOS(string workingDirectory) {

            if(DreamOSExecutableFound() == false) {
                MessageBox.Show(string.Format("{0} not found - please reinstall Dream from Setup", k_strDreamClientFilename), "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return -1;
            }

            // Prepare the process to run
            ProcessStartInfo dreamOSProcessStartInfo = new ProcessStartInfo();

            //dreamOSProcessStartInfo.WorkingDirectory = Environment.CurrentDirectory;
            dreamOSProcessStartInfo.WorkingDirectory = workingDirectory;
            dreamOSProcessStartInfo.UseShellExecute = true;
            dreamOSProcessStartInfo.FileName = k_strDreamClientFilename;

            // Skip first argument which is the name of the process
            string[] commandLineArguments = Environment.GetCommandLineArgs();
            commandLineArguments = commandLineArguments.Skip(1).ToArray();
            foreach (string arg in commandLineArguments) {
                dreamOSProcessStartInfo.Arguments += String.Format("\"{0}\" ", arg);
            }

            // Deal with weird parsing
            dreamOSProcessStartInfo.Arguments = System.Net.WebUtility.UrlDecode(dreamOSProcessStartInfo.Arguments);

            //if (m_strDreamOSPath == null) return -1;

            //dreamOSProcessStartInfo.FileName = m_strDreamOSPath;

            //dreamOsProcessStartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            //dreamOsProcessStartInfo.CreateNoWindow = true;

            // Run the external process & wait for it to finish
            //int exitCode;
            //using (Process proc = Process.Start(dreamOSProcessStartInfo)) {
            //    proc.WaitForExit();
            //
            //    // Retrieve the app's exit code
            //    exitCode = proc.ExitCode;
            //}

            // DEBUG
            //MessageBox.Show(string.Format("DEBUG: launching with {0}", dreamOSProcessStartInfo.Arguments), "DreamLaunch", MessageBoxButton.OK, MessageBoxImage.Information);

            try {
                Process.Start(dreamOSProcessStartInfo);
            }
            catch {
                MessageBox.Show(string.Format("{0} failed to launch - please reinstall Dream from Setup", k_strDreamClientFilename), "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return -1;
            }            

            return 0;
        }

        public string m_strDreamOSPath = null;
        public MainWindow m_mainWindow = null;
    }
}
