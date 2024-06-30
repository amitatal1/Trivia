using System;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Windows;
using TriviaClient.Help;

namespace TriviaClient
{
    public partial class App : Application
    {
        public Socket AppSocket { get; set; }
        private bool _isProgrammaticClose = false;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            // Attach the Closed event handler to all current windows
            foreach (Window window in this.Windows)
            {
                AttachWindowEvents(window);
            }

            // Monitor for new windows being activated
            this.Activated += App_Activated;
        }

        private void App_Activated(object sender, EventArgs e)
        {
            foreach (Window window in this.Windows)
            {
                AttachWindowEvents(window);
            }
        }

        private void AttachWindowEvents(Window window)
        {
            if (window != null)
            {
                window.Closed -= OnAnyWindowClosed;
                window.Closed += OnAnyWindowClosed;
            }
        }

        public void ProgrammaticClose(Window window)
        {
            _isProgrammaticClose = true;
            window.Close();
            _isProgrammaticClose = false;
        }

        private void OnAnyWindowClosed(object sender, EventArgs e)
        {
            if (!_isProgrammaticClose)
            {
                logout();
            }
        }

        private void logout()
        {
            if (AppSocket != null && AppSocket.Connected)
            {
                var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.LOGOUT_CODE, "natek oti");
                int rs = AppSocket.Send(serializedMsg);
            }
        }
    }
}
 