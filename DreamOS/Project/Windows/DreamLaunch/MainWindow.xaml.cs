using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using Squirrel;

namespace DreamLaunch {

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    
    public partial class MainWindow : Window {

        public MainWindow() {
            InitializeComponent();

            SetStatusText("Loading");
        }

        public void SetStatusText(string strStatus)
        {
            m_textBlockStatus.Dispatcher.BeginInvoke(
                (Action)(() => { m_textBlockStatus.Text = strStatus; })
            );
        }

        public void SetDownloadProgressBarValue(int value)
        {
            m_progressBarDownload.Dispatcher.BeginInvoke(
                (Action)(() => { m_progressBarDownload.Value = value; })
            );
        }

        public void SetDownloadProgressBarPercentage(int percentage)
        {
            if (percentage > 100)
                percentage = 100;
            else if (percentage < 0)
                percentage = 0;

            m_progressBarDownload.Dispatcher.BeginInvoke(
                (Action)(() => {
                    int val = (int)((float)(m_progressBarDownload.Maximum) * ((float)(percentage) / 100.0f));
                    m_progressBarDownload.Value = val;
                }) 
            );
        }

        //TextBlock m_textBlockStatus = null;
    }
}
