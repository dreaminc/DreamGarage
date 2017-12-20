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

            if(m_dreamRegistryController.Initialize() == -1) {
                Environment.Exit(0);
            }

            // Kick off an Update
            int result = await m_squirrelUpdateController.CheckForSquirrelUpdate();

            // Launch DreamOS Process

            // Exit
            Environment.Exit(0);

        }

        private void Application_Startup(object sender, StartupEventArgs e) {

            m_squirrelUpdateController = new SquirrelUpdateController();
            m_dreamRegistryController = new DreamRegistryController();

            // First launch the main window
            m_mainWindow = new MainWindow();

            m_mainWindow.ContentRendered += HandleMainWindowContentRenderedAsync;

            m_mainWindow.Show();

            
        }

        //private bool m_fShowTheWelcomeWizard = true;
        private MainWindow m_mainWindow = null;
        private SquirrelUpdateController m_squirrelUpdateController = null;
        private DreamRegistryController m_dreamRegistryController = null;
    }
}
