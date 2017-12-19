using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

using Squirrel;

namespace DreamLaunch {

    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    
    public partial class App : Application
    {

        // Update Manager Handlers 
        private void HandleOnFirstRun()
        {
            // do nothing
            //m_fShowTheWelcomeWizard = true;
        }

        private void HandleOnInitialInstall(Version ver)
        {
            if (m_squirrelUpdateManager != null)
                m_squirrelUpdateManager.CreateShortcutForThisExe();
        }

        private void HandleOnAppUpdate(Version ver)
        {
            // Do nothing for now
            //if (m_squirrelUpdateManager != null)
            //    m_squirrelUpdateManager.CreateShortcutForThisExe()
        }

        private void HandleOnAppObsoleted(Version ver)
        {
            // Do nothing for now
        }

        private void HandleOnAppUninstall(Version ver)
        {
            // Nothing for now
            //if (m_squirrelUpdateManager != null)
            //  m_squirrelUpdateManager.RemoveShortcutForThisExe()
        }

        async private void CheckForSquirrelUpdate()
        {
            try
            {
                await m_squirrelUpdateManager.UpdateApp();
            }
            catch(Exception ex)
            {
                throw new ApplicationException(string.Format("Failed to call UpdatEManager::UpdateApp"), ex);
            }
        }

        private void Application_Startup(object sender, StartupEventArgs e)
        {
            // Figure out where to look for RELEASES

#if (_TEST_LOCAL_LAUNCHER)
            m_strReleasesURI = "C:\\dev\\DreamGarage\\DreamOS\\Project\\Windows\\DreamOS\\DevReleases";
#endif

            if (m_squirrelUpdateManager == null) {
                try
                {
                    m_squirrelUpdateManager = new UpdateManager(m_strReleasesURI);
                }
                catch(Exception ex)
                {
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

            CheckForSquirrelUpdate();

        }

        //private bool m_fShowTheWelcomeWizard = true;
        private UpdateManager m_squirrelUpdateManager = null;
        private string m_strReleasesURI = null;
    }
}
