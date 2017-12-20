using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Windows;
using Microsoft.Win32;
using System.Diagnostics;

using Squirrel;

namespace DreamLaunch {

    class SquirrelUpdateController {

        // Update Manager Handlers 
        private void HandleOnFirstRun() {
            // do nothing
            //m_fShowTheWelcomeWizard = true;
            if (m_mainWindow != null)
                m_mainWindow.SetStatusText("First run");
        }

        private void HandleOnInitialInstall(Version ver) {
            if (m_squirrelUpdateManager != null)
                m_squirrelUpdateManager.CreateShortcutForThisExe();
        }

        private void HandleOnAppUpdate(Version ver) {
            // Do nothing for now
            //if (m_squirrelUpdateManager != null)
            //    m_squirrelUpdateManager.CreateShortcutForThisExe()

            if (m_mainWindow != null)
                m_mainWindow.SetStatusText("Updating");

        }

        private void HandleOnAppObsoleted(Version ver) {
            // Do nothing for now
            if (m_mainWindow != null)
                m_mainWindow.SetStatusText("OnObsolete");
        }

        private void HandleOnAppUninstall(Version ver) {
            // Nothing for now
            //if (m_squirrelUpdateManager != null)
            //  m_squirrelUpdateManager.RemoveShortcutForThisExe()

            if (m_mainWindow != null)
                m_mainWindow.SetStatusText("App Uninstall");
        }

        int s_downloadCounter = 0;

        private void HandleOnUpdateAppProgress(int value) {
            if (value == 100) {
                if (m_mainWindow != null) {
                    m_mainWindow.SetStatusText("Finished Downloading");
                    m_mainWindow.SetDownloadProgressBarPercentage(100);
                }
            }
            else {

                // Download progress
                s_downloadCounter++;
                if (s_downloadCounter > 3)
                    s_downloadCounter = 0;

                if (m_mainWindow != null) {
                    // Getting fancy
                    string strDownload = string.Format("Downloading {0}% ", value);
                    for (int i = 0; i < s_downloadCounter; i++)
                        strDownload += ".";

                    m_mainWindow.SetStatusText(strDownload);
                    m_mainWindow.SetDownloadProgressBarPercentage(value);
                }
            }
        }

        async public Task<int> CheckForSquirrelUpdate() {
            try {
                if (m_mainWindow != null)
                    m_mainWindow.SetStatusText("Checking for Update");

                UpdateInfo updateInfo = await m_squirrelUpdateManager.CheckForUpdate();
                
                if((updateInfo.CurrentlyInstalledVersion == null) ||
                   (updateInfo.FutureReleaseEntry != null && updateInfo.FutureReleaseEntry.Version > updateInfo.CurrentlyInstalledVersion.Version)) 
                {

                    if (m_mainWindow != null)
                        m_mainWindow.SetStatusText(string.Format("Downloading New Version: {0}", updateInfo.FutureReleaseEntry.Version));

                    ReleaseEntry releaseEntry = await m_squirrelUpdateManager.UpdateApp(progress: HandleOnUpdateAppProgress);

                    if (m_mainWindow != null) {
                        HandleOnUpdateAppProgress(100);

                        m_mainWindow.SetStatusText("Update Complete");

                        // Launch the app here
                    }
                }
                else {
                    if (m_mainWindow != null)
                        m_mainWindow.SetStatusText(string.Format("Version {0} Up To Date", updateInfo.CurrentlyInstalledVersion.Version));
                }

                
                //*/
            }
            catch (Exception ex) {

                if (m_mainWindow != null)
                    m_mainWindow.SetStatusText("Error");

                MessageBox.Show(ex.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                throw new ApplicationException(string.Format("Failed to call UpdatEManager::UpdateApp"), ex);
            }

            return 0;
        }

        public int SetMainWindowController(MainWindow mainWindow) {
            m_mainWindow = mainWindow;
            return 0;
        }

        public int InitializeSquirrel() {
            // Figure out where to look for RELEASES
#if (_TEST_LOCAL_LAUNCHER)
            //m_strReleasesURI = "C:\\dev\\DreamGarage\\DreamOS\\Project\\Windows\\DreamOS\\DevReleases";
            m_strReleasesURI = "https://github.com/dreaminc/Dream/releases/download/DevReleases/";
#elif (_DEV_RELEASE)
            m_strReleasesURI = "https://github.com/dreaminc/Dream/releases/download/DevReleases/";
#elif (_PROD_RELEASE)
            m_strReleasesURI = "https://github.com/dreaminc/Dream/releases/download/Releases/";
#endif

            if (m_squirrelUpdateManager == null) {
                try {
                    if (m_mainWindow != null)
                        m_mainWindow.SetStatusText("Initializing");

                    m_squirrelUpdateManager = new UpdateManager(m_strReleasesURI);
                }
                catch (Exception ex) {
                    if (m_mainWindow != null)
                        m_mainWindow.SetStatusText("Checking for Update");

                    MessageBox.Show(ex.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    throw new ApplicationException(string.Format("Failed to create Squirrel UpdateManager"), ex);
                }
            }

            // Set up helpers and handlers
            SquirrelAwareApp.HandleEvents(
              onInitialInstall: HandleOnInitialInstall,
              onAppUpdate: HandleOnAppUpdate,
              onAppUninstall: HandleOnAppUninstall,
              onFirstRun: HandleOnFirstRun,
              onAppObsoleted: HandleOnAppObsoleted);

            return 0;
        }

        public string GetSquirrelRootAppDirectory() {
            if (m_squirrelUpdateManager != null)
                return m_squirrelUpdateManager.RootAppDirectory;

            return null;
        }

        private MainWindow m_mainWindow = null;
        private UpdateManager m_squirrelUpdateManager = null;
        private string m_strReleasesURI = null;
    }
}
