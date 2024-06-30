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
using System.Windows.Shapes;
using TriviaClient.Help;

namespace TriviaClient.Views
{
    /// <summary>
    /// Interaction logic for PersonalStatistics.xaml
    /// </summary>
    public partial class PersonalStatistics : Window
    {
        public Socket _socket { get; set; }
        public PersonalStatistics(Socket socket)
        {
            InitializeComponent();

            _socket = socket;
            loadStatistics();
        }

        private void Home_Botton_Click(object sender, RoutedEventArgs e)
        {
            var mainPage = new MainWindow(_socket);
            mainPage.Show();

            var app = Application.Current as App;
            app.ProgrammaticClose(this);
        }

        private void loadStatistics()
        {
            if (_socket != null)
            {
                var serializedMsg = Help.SerializationHelper.SerializePacket(SerializationHelper.MessageCode.GET_PERSONAL_STATS_CODE, "Get stats");

                int rs = _socket.Send(serializedMsg);
                if (rs > 0)
                {
                    var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                    string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                    if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                    {
                        var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                        MessageBox.Show(error.message, "Joining room error", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                    else // a legitimate response came back
                    {
                        // Deserialize the response to JSON.      
                        var response = JsonSerializer.Deserialize<GetPersonalStatsResponse>(bufferString);
                        if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                        {
                            MessageBox.Show("Failed to Join the room", "Room Error", MessageBoxButton.OK, MessageBoxImage.Error); // should not go here but maybe in future versions
                        }
                        else
                        {
                            List<string> stats = response.statistics;

                            if (stats.Count > 0)
                            {
                                GamesPlayed.Text = response.statistics.Count > 0 ? "Games played: " + stats[0] : "N/A";
                                TotalAnswers.Text = stats.Count > 1 ? "Number of Total Answers: " + stats[1] : "N/A";
                                correctAnswers.Text = stats.Count > 2 ? "Number of correct answers: " + stats[2] : "N/A";
                                AvarageTime.Text = stats.Count > 3  ? "Avarage Answer Time: " + stats[3] : "N/A";
                            }
                            else
                            {
                                GamesPlayed.Text = "N/A";
                                TotalAnswers.Text = "N/A";
                                correctAnswers.Text = "N/A";
                                AvarageTime.Text = "N/A";
                            }

                        }
                    }
                }
            }
        }
        public class GetPersonalStatsResponse
        {
            public int status { get; set; }
            public List<string> statistics { get; set; }
        };
    }
}













