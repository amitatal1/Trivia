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
using TriviaClient.Views;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Socket _socket;
        public MainWindow(Socket socket)
        {
            InitializeComponent();
            _socket = socket;

        }

        private void CreateRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            var nextPage = new CreateRoom(_socket);
            nextPage.Show();

            var app = Application.Current as App;
            app.ProgrammaticClose(this);
        }

        private void JoinRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            var nextPage = new JoinRoom(_socket);
            nextPage.Show();

            var app = Application.Current as App;
            app.ProgrammaticClose(this);
        }

        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void ScoreBourdBtn_click(object sender, RoutedEventArgs e)
        {
            var nextPage = new TopScores(_socket);
            nextPage.Show();

            var app = Application.Current as App;
            app.ProgrammaticClose(this);
        }

        private void PersonalStatisticsBtn_click(object sender, RoutedEventArgs e)
        {
            var nextPage = new PersonalStatistics(_socket);
            nextPage.Show();

            var app = Application.Current as App;
            app.ProgrammaticClose(this);
        }
        private void LogoutButton_Click(object sender, RoutedEventArgs e)
        {

            var serializedMsg = Help.SerializationHelper.SerializePacket(Help.SerializationHelper.MessageCode.LOGOUT_CODE, "natek oti");
            int rs = _socket.Send(serializedMsg);
            if (rs > 0)
            {
                var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                {
                    var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);

                    MessageBox.Show(error.message, "Logout Error", MessageBoxButton.OK, MessageBoxImage.Error);

                }
                else // a legitimmate reposnse came back
                {
                    //deserialize the respose to json.
                    StatusOnlyResponse response = JsonSerializer.Deserialize<StatusOnlyResponse>(bufferString);
                    if (response.status == (int)SerializationHelper.ResponseStatuses.SUCCESS_STATUS)
                    {
                        MessageBox.Show("Logged out", "Logout OK", MessageBoxButton.OK, MessageBoxImage.Information); //should not go here but maybe in fututre versions
                                                                                                                                  //login success flow, move to the next page basically
                        var loginPage = new LoginView(_socket);
                        loginPage.Show();

                        this.Close();

                    }


                }
            }
        }
    }
}
