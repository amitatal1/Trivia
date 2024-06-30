using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
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
using TriviaClient.Help;

namespace TriviaClient.Views
{
    /// <summary>
    /// Interaction logic for SignupView.xaml
    /// </summary>
    public partial class SignupView : Window
    {
        protected Socket _socket;

        public SignupView(Socket socket)
        {

            InitializeComponent();
            this._socket = socket;
        }


        private void LoginBlock_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {   // Create an instance of the new window
            var newWindow = new LoginView(_socket);
            // Show the new window
            newWindow.Show();
            // Optionally close the current window
            this.Close();
        }

        


        private void SignupBtn_Click(object sender, RoutedEventArgs e)
        {
            string username = UsernameInput.Text;
            string password = PasswordInput.Password;
            string email = EmailInput.Text;

            if (username != "" && password != "" && email!= "")
            {
                var signupData = new SignUpData { username = username, password = password ,email =email};

                var serializedMsg = SerializationHelper.SerializePacket(Help.SerializationHelper.MessageCode.SIGN_UP_CODE, signupData);

                int rs = _socket.Send(serializedMsg);
                if (rs > 0)
                {
                    var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                    string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                    if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                    {
                        var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);

                        MessageBox.Show(error.message, "Sign Up Error", MessageBoxButton.OK, MessageBoxImage.Error);

                    }
                    else // a legitimmate reposnse came back
                    {
                        //deserialize the respose to json.
                        StatusOnlyResponse response = JsonSerializer.Deserialize<StatusOnlyResponse>(bufferString);
                        if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                        {
                            MessageBox.Show("Failed to sign up", "Sign Up Error", MessageBoxButton.OK, MessageBoxImage.Error); //should not go here but maybe in fututre versions

                        }
                        else
                        {
                            MessageBox.Show("Signed up and logged in ", "Sign Up OK", MessageBoxButton.OK, MessageBoxImage.Exclamation); //logged in.

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
    public class SignUpData
    {
        public string username { get; set; }
        public string password { get; set; }
        public string  email { get; set; }
    }
}
