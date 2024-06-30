using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
using static TriviaClient.Views.RoomView;
using TriviaClient.Help;

namespace TriviaClient.Views
{
    /// <summary>
    /// Interaction logic for TopScores.xaml
    /// </summary>
    public partial class TopScores : Window
    {
        Socket _socket;
        public TopScores(Socket socket)
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
            if (_socket.Connected)
            {
                var serializedMsg = Help.SerializationHelper.SerializePacket(SerializationHelper.MessageCode.GET_HIGH_SCORE_CODE, "Get scores");

                int rs = _socket.Send(serializedMsg);
                if (rs > 0)
                {
                    var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                    string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                    if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                    {
                        var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                        MessageBox.Show(error.message, "Failed to get Statistics", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                    else // a legitimate response came back
                    {
                        // Deserialize the response to JSON.      
                        var response = JsonSerializer.Deserialize<getHighScoreResponse>(bufferString);
                        if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                        {
                            MessageBox.Show("Failed to get Statistics", "Statistics Error", MessageBoxButton.OK, MessageBoxImage.Error); // should not go here but maybe in future versions
                        }
                        else
                        {
                            showStatistics(response.statistics);
                        }
                    }
                }
            }
        }

        private void showStatistics(List<string> l)
        {
            if (l != null)
            {
                firstPlace.Text = l.Count > 0 ? l[0] : string.Empty;
                secondPlace.Text = l.Count > 1 ? l[1] : string.Empty;
                thirdPlace.Text = l.Count > 2 ? l[2] : string.Empty;
                fourthPlace.Text = l.Count > 3 ? "4: " + l[3] : string.Empty;
                fifthPlace.Text = l.Count > 4 ?  l[4] + " :5" : string.Empty;
            }
        }


        public class getHighScoreResponse
        {
            public int status { get; set; }
            public List<string> statistics { get; set; }
        };
    }
}
