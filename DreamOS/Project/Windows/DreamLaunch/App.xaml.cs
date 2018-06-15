using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Threading;

namespace DreamLaunch {

    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    
    public partial class App : Application {
        private async void HandleMainWindowContentRenderedAsync(object sender, System.EventArgs eventArgs) {

            // Set up Squirrel Controller
            m_squirrelUpdateController.SetMainWindowController(m_mainWindow);
            m_squirrelUpdateController.InitializeSquirrel();

            // Set up Registry
            m_dreamRegistryController.SetMainWindowController(m_mainWindow);
            m_dreamRegistryController.SetDreamRootAppDirectory(m_squirrelUpdateController.GetSquirrelRootAppDirectory());

            if (m_dreamRegistryController.Initialize() == -1) {
                Environment.Exit(0);
            }



            // Don't launch on first install or if registry updated
            // Not sure if this is the best way (if registry is updated, app will not launch post initial install)
            //if (m_squirrelUpdateController.IsInitialInstall() || m_dreamRegistryController.DidRegistryUpdate()) {

            // This is a bit of a hack but should work
            string strCommandLineArgs = "";
            string[] commandLineArguments = Environment.GetCommandLineArgs();
            commandLineArguments = commandLineArguments.Skip(1).ToArray();
            foreach (string arg in commandLineArguments) {
                strCommandLineArgs += String.Format("\"{0}\" ", arg);
            }

            strCommandLineArgs.Trim();
            strCommandLineArgs.ToLower();
            strCommandLineArgs = System.Net.WebUtility.UrlDecode(strCommandLineArgs);

            string strSquirrelFirstRun = "--squirrel-firstrun";
            strSquirrelFirstRun.Trim();
            strSquirrelFirstRun.ToLower();
            
            // Kick off an Update
            //if (strCommandLineArgs.IndexOf(strSquirrelFirstRun) == -1)
            int result = await m_squirrelUpdateController.CheckForSquirrelUpdate();
            
            if (result == -1) {
                //MessageBox.Show("Squirrel Update Failed", "DreamLaunch", MessageBoxButton.OK, MessageBoxImage.Information);
                Shutdown();
                return;
            }

            // DEBUG
            //MessageBox.Show(string.Format("{0} is {1} to {2}", strCommandLineArgs, strCommandLineArgs.IndexOf(strSquirrelFirstRun), strSquirrelFirstRun), 
            //    "DreamLaunch", MessageBoxButton.OK, MessageBoxImage.Information);

            if (m_squirrelUpdateController.IsInitialInstall() || strCommandLineArgs.IndexOf(strSquirrelFirstRun) != -1) {

                m_mainWindow.SetStatusText("Dream Installed!");

                // TEST
                //MessageBox.Show("Initial Install Complete", "DreamLaunch", MessageBoxButton.OK, MessageBoxImage.Information);

                // Persist install message
                Thread.Sleep(1250);
            }
            else {
                // Launch DreamOS Process
                m_dreamOSLauncher.SetMainWindowController(m_mainWindow);
                m_dreamOSLauncher.Initialize();

                // Launch Dream OS (not on first install)
                string strWorkingDirectory = m_squirrelUpdateController.GetSquirrelRootAppDirectory();
                m_dreamOSLauncher.LaunchDreamOS(strWorkingDirectory);
            }

            // Exit
            //Environment.Exit(0);
            Shutdown();
        }

        private void Application_Startup(object sender, StartupEventArgs e) {

            m_squirrelUpdateController = new SquirrelUpdateController();
            m_dreamRegistryController = new DreamRegistryController();
            m_dreamOSLauncher = new DreamOSLauncher();

            // First launch the main window
            m_mainWindow = new MainWindow();

            m_mainWindow.ContentRendered += HandleMainWindowContentRenderedAsync;

            m_mainWindow.Show();

            
        }

        //private bool m_fShowTheWelcomeWizard = true;
        private MainWindow m_mainWindow = null;

        private SquirrelUpdateController m_squirrelUpdateController = null;
        private DreamRegistryController m_dreamRegistryController = null;
        private DreamOSLauncher m_dreamOSLauncher = null;
    }
}
