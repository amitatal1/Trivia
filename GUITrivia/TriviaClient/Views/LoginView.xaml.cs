using System;
using System.Configuration;
using System.Net;
using System.Net.Sockets;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Text.Json;
using TriviaClient.Help;
using System.Text;

namespace TriviaClient.Views
{


    public partial class LoginView :Window
    {

        protected Socket _socket;

        public LoginView()
        {
            InitializeComponent();

            Application.Current.Exit += OnApplicationExit; // Subscribe to the Exit event

            try
            {
                _socket = CreateConnection();
                ((App)Application.Current).AppSocket = _socket;
            }
            catch (Exception e)
            {
                HandleConnectionError(e);
            }
        }

        public LoginView(Socket serverSocket)
        {
            InitializeComponent();
            _socket = serverSocket;
        }


        private void SignupBlock_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            // Create an instance of the new window
            var newWindow = new SignupView(_socket);
            // Show the new window
           newWindow.Show();
            // Optionally close the current window
            var app = Application.Current as App;
            app.ProgrammaticClose(this);

        }




        private static Socket CreateConnection()
        {
            string ipString = ConfigurationManager.AppSettings["ServerIP"];
            IPAddress address = IPAddress.Parse(ipString);

            IPEndPoint localEndPoint = new IPEndPoint(address, int.Parse(ConfigurationManager.AppSettings["ServerPort"]));

            Socket sender = new Socket(address.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            sender.Connect(localEndPoint);

            Console.WriteLine("Socket connected to -> {0}", sender.RemoteEndPoint.ToString());

            return sender;
        }

        private void HandleConnectionError(Exception e)
        {
            Console.WriteLine(e.Message);

            MessageBox.Show("Failed to connect to the server. Please check your network settings.", "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
            Application.Current.Shutdown(); // Or any other logic to handle the application state
        }



        private void OnApplicationExit(object sender, ExitEventArgs e)
        {
            if (_socket != null && _socket.Connected)
            {
                _socket.Shutdown(SocketShutdown.Both);
                _socket.Close();
            }
        }

        private void LoginBtn_Click(object sender, RoutedEventArgs e)
        {
            string username = UsernameInput.Text;
            string password = PasswordInput.Password;

            if(username != "" && password != "")
            {
                var loginData = new LoginData {username= username, password = password };

                var serializedMsg = Help.SerializationHelper.SerializePacket(Help.SerializationHelper.MessageCode.LOGIN_CODE, loginData);
                int rs = _socket.Send(serializedMsg); 
                if(rs>0) 
                {
                    var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                    string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                    if (resp.ResponseCode == (byte) SerializationHelper.MessageCode.ERROR_CODE)
                    {
                        var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);

                        MessageBox.Show(error.message, "Login Error", MessageBoxButton.OK, MessageBoxImage.Error);
                        
                    }
                    else // a legitimmate reposnse came back
                    {
                        //deserialize the respose to json.
                        StatusOnlyResponse response = JsonSerializer.Deserialize<StatusOnlyResponse>(bufferString);
                        if(response.status==(int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                        {
                            MessageBox.Show("error", "Login Error", MessageBoxButton.OK, MessageBoxImage.Error); //should not go here but maybe in fututre versions

                        }
                        else
                        {
                            MessageBox.Show("Logged in successfully ", "Login OK", MessageBoxButton.OK, MessageBoxImage.Information); //should not go here but maybe in fututre versions
                                                                                                                                      //login success flow, move to the next page basically
                            var mainPage = new MainWindow(_socket);
                            mainPage.Show();

                            var app = Application.Current as App;
                            app.ProgrammaticClose(this);
                        }


                    }
                }
            }
            else
            {
                MessageBox.Show("All fields should be filled", "Input Error", MessageBoxButton.OK, MessageBoxImage.Error);

            }
        }

    }

    public class LoginData
    {
        public string username { get; set; }
        public string password { get; set; }
    }

    
}
