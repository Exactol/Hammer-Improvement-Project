using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms.Integration;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Panel = System.Windows.Forms.Panel;

namespace HammerWrapper
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        const UInt32 WS_VISIBLE = 0x10000000;
        const UInt32 WS_BORDER = 0x00800000;
        const uint WM_CLOSE = 0x0010;
        const int GWL_STYLE = -16;

        public MainWindow()
        {
            InitializeComponent();

            

            //var procInfo = new ProcessStartInfo(@"C:\Program Files(x86)\Steam\steamapps\common\Team Fortress 2\bin\hammer.exe");
            //procInfo.WindowStyle = ProcessWindowStyle.Maximized;
            

            //var childProc = Process.Start(procInfo);
            //Panel hammerPanel = new Panel();

            //WindowsFormsHost winFormHost = new WindowsFormsHost();

            //winFormHost.Child = hammerPanel;



        }

        //class Init
        //{
        //    [DllImport("user32.dll")]
        //    static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);


        //}
        [DllImport("user32.dll")]
        static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [DllImport("user32.dll")]
        static extern int SetWindowLong(IntPtr hWnd, int index, UInt32 newLong);

        [DllImport("user32.dll")]
        static extern bool MoveWindow(IntPtr hWnd, int x, int y, int nWidth, int nHeight, bool repaint);

        [DllImport("user32.dll")]
        static extern bool PostMessage(IntPtr hWnd, uint message, IntPtr wParam, IntPtr lParam);

        //[DllImport("SHCore.dll")]
        //static extern bool SetProcessDpiAwareness(PROCESS_DPI_AWARENESS awareness);

        private enum PROCESS_DPI_AWARENESS
        {
            Process_DPI_Unaware = 0,
            Process_System_DPI_Aware = 1,
            Process_Per_Monitor_DPI_Aware = 2
        }

        private Process p;
        void ButtonBase_OnClick(object sender, RoutedEventArgs e)
        {
            
            try
            {
                p = Process.Start(
                    @"C:\Program Files (x86)\Steam\steamapps\common\Team Fortress 2\bin\hammer.exe");
                p.WaitForInputIdle();
                Thread.Sleep(1500);
                SetParent(p.MainWindowHandle, new WindowInteropHelper(this).Handle); //TODO store handle returned by this function



                SetWindowLong(p.MainWindowHandle, GWL_STYLE, WS_VISIBLE);
                //SetWindowLong(p.MainWindowHandle, GWL_STYLE, WS_BORDER);
                Console.WriteLine("Width:" + Width, "Height:"+Height);
                MoveWindow(p.MainWindowHandle, 0, 0, 1920, 1080, true);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void MainWindow_OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (p != null)
            {
                if (p.MainWindowHandle != IntPtr.Zero)
                {
                    MoveWindow(p.MainWindowHandle, 0, 0, (int)Width, (int)Height, true);
                }
            }

        }

        private void MainWindow_OnClosing(object sender, CancelEventArgs e)
        {
            if (p != null)
            {
                if (p.MainWindowHandle != IntPtr.Zero)
                {
                    PostMessage(p.MainWindowHandle, WM_CLOSE, IntPtr.Zero, IntPtr.Zero);
                    Thread.Sleep(1000);
                }
            }
        }
    }
}

//using System;
//using System.Collections.Generic;
//using System.Diagnostics;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Windows;
//using System.Windows.Controls;
//using System.Windows.Data;
//using System.Windows.Documents;
//using System.Windows.Input;
//using System.Windows.Media;
//using System.Windows.Media.Imaging;
//using System.Windows.Navigation;
//using System.Windows.Shapes;

//namespace HammerWrapper
//{
//    /// <summary>
//    /// Interaction logic for MainWindow.xaml
//    /// </summary>
//    public partial class MainWindow : Window
//    {
//        private Process process;
//        public MainWindow()
//        {
//            InitializeComponent();
//            process = new Process();
//            process.StartInfo = new ProcessStartInfo(@"C:\Program Files (x86)\Steam\steamapps\common\Team Fortress 2\bin\hammer.exe");
//            host.Process = process;

//            if (Environment.Is64BitProcess)
//            {
//                Console.WriteLine("64bit");
//            }
//            else
//            {
//                Console.WriteLine("32bit");
//            }

//        }

//        private void Host_OnUnloaded(object sender, RoutedEventArgs e)
//        {
//            Console.WriteLine("Host unloaded");
//            try
//            {
//                process.Kill();
//            }
//            catch (Exception ex)
//            {
//                Console.WriteLine(ex.ToString());
//            }

//        }
//    }
//}
