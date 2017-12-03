
using System.Diagnostics;
using System.Runtime.InteropServices;
//using System.Windows.Interop;

//public class HammerHost : HwndHost
//{

//}

//using System;
//using System.Collections.Generic;
//using System.Diagnostics;
//using System.Linq;
//using System.Runtime.InteropServices;
//using System.Text;
//using System.Threading.Tasks;
//using System.Windows.Interop;
//using System.Windows.Controls;
//using System.Windows.Forms.Integration;
//using System.Threading;
//using System.Windows;
//using Panel = System.Windows.Forms.Panel;

//namespace HammerWrapper
//{
//    //[DllImport("user32.dll", EntryPoint = "CreateWindowEx", CharSet = CharSet.Unicode)]
//    //internal static extern IntPtr CreateWindowEx(int dwExStyle,
//    //                                            string lpszClassName,
//    //                                            string lpszWindowName
//    public class HammerHost : ContentControl
//    {
//        [DllImport("user32.dll")]
//        static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

//        [DllImport("user32.dll")]
//        static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int x, int y, int cx, int cy, uint flags);

//        [DllImport("user32.dll")]
//        static extern IntPtr SetWindowLong(IntPtr hWnd, int nIndex, IntPtr dwNewLong);

//        [DllImport("user32.dll", SetLastError = true)]
//        static extern IntPtr GetWindowLong(IntPtr hWnd, int nIndex);

//        const UInt32 SWP_FRAMECHANGED = 0x0020;

//        static readonly int GWL_STYLE = (-16);

//        public const uint WS_CAPTION = 0x00C00000;

//        public const uint WS_THICKFRAME = 0x00040000;

//        public const uint WS_CLIPCHILDREN = 0x02000000;

//        public const uint WS_OVERLAPPED = 0x00000000;

//        private WindowsFormsHost host = new WindowsFormsHost();
//        private Panel panel = new Panel();
//        private IntPtr windowHandle;

//        public Process Process
//        {
//            set
//            {
//                try
//                {
//                    value.Start();

//                    if (!value.WaitForInputIdle(3000))
//                    {
//                        throw new ArgumentException();
//                    }

//                    Thread.Sleep(1000);

//                    windowHandle = value.MainWindowHandle;

//                    long dwStyle = GetWindowLong(windowHandle, GWL_STYLE);
//                    //IntPtr test = new IntPtr(dwStyle & WS_CAPTION & WS_THICKFRAME);
//                    SetWindowLong(windowHandle, GWL_STYLE, new IntPtr(dwStyle & WS_CAPTION & WS_THICKFRAME));
//                    SetWindowPos(windowHandle, IntPtr.Zero, 0, 0,
//                        Convert.ToInt32(Math.Floor(this.ActualWidth)),
//                        Convert.ToInt32(Math.Floor(this.ActualHeight)),
//                        SWP_FRAMECHANGED);

//                    SetParent(windowHandle, panel.Handle);


//                }
//                catch (Exception e)
//                {
//                    Console.WriteLine(e.Message.ToString());
//                }
//            }
//        }

//        public void WindowHostHelper()
//        {
//            this.Content = host;
//            this.panel.CreateControl();
//            host.Child = this.panel;
//        }

//        protected override void OnPropertyChanged(DependencyPropertyChangedEventArgs e)
//        {
//            base.OnPropertyChanged(e);
//        }

//        protected override Size ArrangeOverride(Size arrangeBounds)
//        {
//            base.ArrangeOverride(arrangeBounds);
//            SetWindowPos(windowHandle, IntPtr.Zero, 0, 0,
//                Convert.ToInt32(Math.Floor(this.ActualWidth)),
//                Convert.ToInt32(Math.Floor(this.ActualHeight)),
//                0);

//            return arrangeBounds;
//        }

//        protected override void OnContentChanged(object oldContent, object newContent)
//        {
//            Content = host;
//        }
//    }
    

//}


