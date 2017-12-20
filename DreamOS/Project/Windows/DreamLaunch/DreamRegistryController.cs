using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Microsoft.Win32;
using System.Diagnostics;
using System.IO;
using System.Security.AccessControl;
using System.Security.Principal;
using System.Windows;
using System.Reflection;

namespace DreamLaunch {

    class DreamRegistryController {

        public class RegistryEntry {
            public string[] subkeys;
            public string strName;
            public string strData;

            public void Print(string strRootRegistryKey = s_dreamRootKey, bool fReadRegistry = false) {
                string strData = (fReadRegistry) ? (string) GetRegistryData(strRootRegistryKey) : GetData();
                Debug.WriteLine("\"{0}\":\"{1}\":{2}", GetKeyString(), GetName(), strData);
            }

            public string GetKeyString(string strRootRegistryKey = s_dreamRootKey) {
                // Set up the registry key from the subkey array
                string strRegistryKey = strRootRegistryKey;

                if (subkeys != null) {
                    foreach (string subkey in subkeys) {
                        strRegistryKey += "\\" + subkey;
                    }
                }

                return strRegistryKey;
            }

            // Get the registry value for the given registry entry
            public object GetRegistryData(string strRootRegistryKey = s_dreamRootKey) {

                // Set up the registry key from the subkey array
                string strRegistryKey = GetKeyString(strRootRegistryKey);

                // Get the reg entry value
                object retVal = Registry.GetValue(strRegistryKey, GetName(), "invalid");

                return retVal;
            }

            public int UpdateDataInRegistry(string strRootRegistryKey = s_dreamRootKey) {

                // Set up the registry key from the subkey array
                string strRegistryKey = GetKeyString(strRootRegistryKey);

                // Set the value to the data on hand
                Registry.SetValue(strRegistryKey, GetName(), GetData());

                return 0;
            }

            public string GetName() {
                if (strName != null)
                    return strName;
                else
                    return "";
            }

            public string GetData() {
                if (strData != null)
                    return strData;
                else
                    return "";
            }
        }

        public RegistryEntry[] m_dreamRegistryEntries = {
            new RegistryEntry{ strData = "\"URL:Dream OS Protocol\"" },
            new RegistryEntry{ strName = "URL Protocol", strData = "\"\"" },
            new RegistryEntry{ subkeys = new string[] {"DefaultIcon" }, strData = "\"dreamos.exe,1\"" },
            new RegistryEntry{ subkeys = new string[] {"shell", "open", "command" }, strData = "\"[UPDATEPATH]\\Update.exe\" --processStart \"DreamLaunch.exe\" --process-start-args \"%1\"'" },
        };

        public int SetMainWindowController(MainWindow mainWindow) {
            m_mainwindow = mainWindow;
            return 0;
        }

        public int UpdatePathInDreamRegistryEntries() {

            foreach(RegistryEntry regEntry in m_dreamRegistryEntries) {
                string strNewData = regEntry.strData.Replace("[UPDATEPATH]", m_dreamRootAppDirectory);
                regEntry.strData = strNewData;
            }

            return 0;
        }

        
        // Utility to check if we're admin
        public bool IsUserAdministrator() {
            bool fAdmin;

            try {
                WindowsIdentity user = WindowsIdentity.GetCurrent();
                WindowsPrincipal principal = new WindowsPrincipal(user);
                fAdmin = principal.IsInRole(WindowsBuiltInRole.Administrator);
            }
            catch (UnauthorizedAccessException ex) {
                fAdmin = false;
            }
            catch (Exception ex) {
                fAdmin = false;
            }

            return fAdmin;
    }

    public int Initialize() {

            Debug.WriteLine("Initializing Dream Registry");

            UpdatePathInDreamRegistryEntries();

            // DEBUG
            //PrintDreamRegistry(false);
            //PrintDreamRegistry(true);

            // Check that values check out

            m_mainwindow.SetStatusText("Verifying Registry");

            if (CheckDreamRegistryVariables() == false) {

                m_mainwindow.SetStatusText("Updating Registry");

                // If we're here we need to run as admin                   
                // TODO: Move this up to App (to maintain args)
                if (IsUserAdministrator() == false) {

                    m_mainwindow.SetStatusText("Admin Privileges Required");

                    // If we're not admin we need to restart as admin
                    ProcessStartInfo proc = new ProcessStartInfo();
                    proc.UseShellExecute = true;
                    proc.WorkingDirectory = Environment.CurrentDirectory;
                    proc.FileName = Assembly.GetEntryAssembly().CodeBase;

                    string[] commandLineArguments = Environment.GetCommandLineArgs();

                    // Skip first argument which is the name of the process
                    commandLineArguments = commandLineArguments.Skip(1).ToArray();

                    // Arguments
                    foreach (string arg in commandLineArguments) {
                        proc.Arguments += String.Format("\"{0}\" ", arg);
                    }

                    proc.Verb = "runas";

                    try {
                        Process.Start(proc);
                    }
                    catch {
                        // The user refused the elevation.
                        return -1;
                    }

                    Environment.Exit(0);
                    return -1;
                }

                // Set up security

                RegistryKey localClasses = Registry.ClassesRoot;

                RegistrySecurity regSecurity = new RegistrySecurity();

                string strCurrentUser = Environment.UserDomainName + "\\" + Environment.UserName;

                RegistryAccessRule regAccessRule = new RegistryAccessRule(strCurrentUser, 
                    RegistryRights.ReadKey | RegistryRights.WriteKey | RegistryRights.Delete | RegistryRights.CreateSubKey,
                    InheritanceFlags.ContainerInherit,
                    PropagationFlags.None,
                    AccessControlType.Allow
                );

                regSecurity.AddAccessRule(regAccessRule);

                regAccessRule = new RegistryAccessRule(strCurrentUser,
                    RegistryRights.ChangePermissions | RegistryRights.CreateSubKey,
                    InheritanceFlags.ContainerInherit,
                    PropagationFlags.InheritOnly | PropagationFlags.NoPropagateInherit,
                    AccessControlType.Allow);

                regSecurity.AddAccessRule(regAccessRule);

                //localClasses.SetAccessControl(regSecurity);

                // If key exists just wipe it out
                RegistryKey dreamRegistryKey = localClasses.OpenSubKey(s_strDreamKeyName, RegistryKeyPermissionCheck.ReadWriteSubTree);
                if(dreamRegistryKey != null) {
                    //MessageBox.Show("deleting dreamos key", "DreamRegistryController", MessageBoxButton.OK, MessageBoxImage.Error);
                    localClasses.DeleteSubKeyTree(s_strDreamKeyName);
                    dreamRegistryKey = null;
                }

                // Create the key
                if (dreamRegistryKey == null) {
                    //MessageBox.Show("creating dreamos key", "DreamRegistryController", MessageBoxButton.OK, MessageBoxImage.Error);
                    dreamRegistryKey = localClasses.CreateSubKey(s_strDreamKeyName, RegistryKeyPermissionCheck.ReadWriteSubTree, regSecurity);
                }

                //MessageBox.Show("updating dreamos key", "DreamRegistryController", MessageBoxButton.OK, MessageBoxImage.Error);
                UpdateDreamRegistry();

                m_mainwindow.SetStatusText("Registry Updated");
            }

            return 0;
        }

        private int PrintDreamRegistry(bool fReadRegistryValues = false) {

            Debug.WriteLine(string.Format("Dream Registry {0}", fReadRegistryValues ? "Entries" : "Data"));

            foreach (RegistryEntry entry in m_dreamRegistryEntries) {
                entry.Print(fReadRegistry:fReadRegistryValues);
            }

            return 0;
        }

        private int UpdateDreamRegistry() {

            // Set up Registry Security
            RegistrySecurity userSecurity = new RegistrySecurity();
            RegistryAccessRule userRule = new RegistryAccessRule("Everyone", RegistryRights.FullControl, AccessControlType.Allow);
            userSecurity.AddAccessRule(userRule);

            foreach (RegistryEntry entry in m_dreamRegistryEntries)
                entry.UpdateDataInRegistry();

            return 0;
        }

        private bool CheckDreamRegistryVariables() {

            foreach (RegistryEntry entry in m_dreamRegistryEntries)
                if (CheckRegistryEntry(entry) == false)
                    return false;

            return true;
        }

        private bool CheckRegistryEntry(RegistryEntry regEntry, string strRootRegistryKey = s_dreamRootKey) {
            string strRegistryValue = (string) regEntry.GetRegistryData(strRootRegistryKey);

            if (regEntry.GetData() == strRegistryValue)
                return true;

            return false;
        }

        public int SetDreamRootAppDirectory(string strDreamRootAppDirectory) {
            m_dreamRootAppDirectory = strDreamRootAppDirectory;
            return 0;
        }

        private string GetDreamRootKey() {
            return s_dreamRootKey;
        }

        private MainWindow m_mainwindow = null;

        const string s_classesRoot = "HKEY_CLASSES_ROOT";

        

#if (_DEV_RELEASE)
        const string s_strDreamKeyName = "dreamosdev";
#else
        const string s_strDreamKeyName = "dreamos";
#endif

        const string s_dreamRootKey = s_classesRoot + "\\" + s_strDreamKeyName;

        private string m_dreamRootAppDirectory = null;
    }
}
