using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using TriviaClient.Help;

namespace TriviaClient.Views
{
    public partial class GameResults
        :Window
    {
        private Socket _socket;
        private ObservableCollection<PlayerResults> _results;
        private CancellationTokenSource _cancellationTokenSource;

        public ObservableCollection<PlayerResults> Results
        {
            get { return _results; }
        }

        public GameResults(Socket socket)
        {
            _socket = socket;
            _results = new ObservableCollection<PlayerResults>();
            DataContext = this;

            _cancellationTokenSource = new CancellationTokenSource();
            StartRefreshingStats(_cancellationTokenSource.Token);

                        InitializeComponent();

        }

        private async void StartRefreshingStats(CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                await Task.Delay(2000);
                if (!token.IsCancellationRequested)
                {
                    await LoadStatsAsync();
                }
            }
        }

        private async Task LoadStatsAsync()
        {
            if (_socket.Connected)
            {
                var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.GET_GAME_RESULTS_CODE, "get Results");
                int rs = _socket.Send(serializedMsg);

                if (rs > 0)
                {
                    var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                    string bufferString = Encoding.ASCII.GetString(resp.Buffer);

                    if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                    {
                        var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                        await Dispatcher.InvokeAsync(() => MessageBox.Show(error.message, "Stats error", MessageBoxButton.OK, MessageBoxImage.Error));
                    }
                    else
                    {
                        var response = JsonSerializer.Deserialize<GetGameResultsResponse>(bufferString);
                        if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                        {
                            await Dispatcher.InvokeAsync(() => MessageBox.Show("Failed to get the stats", "Stats Error", MessageBoxButton.OK, MessageBoxImage.Error));
                        }
                        else
                        {
                            await Dispatcher.InvokeAsync(() => UpdateResults(response.results));
                        }
                    }
                }
            }
        }

        private void UpdateResults(List<PlayerResults> playerResults)
        {
            _results.Clear();
            foreach (var result in playerResults)
            {
                _results.Add(result);
            }

            DisplayPlayerWithHighestScore(); // Update the display of highest scorer
        }

        private void DisplayPlayerWithHighestScore()
        {
            // Find player with highest score using LINQ
            var playerWithHighestScore = _results.OrderByDescending(r => r.score).FirstOrDefault();

            if (playerWithHighestScore != null)
            {
                firstPlace.Text = playerWithHighestScore.username; // Display username of highest scorer
            }
            else
            {
                firstPlace.Text = string.Empty; 
            }
        }
        private void Home_Button_Click(object sender, RoutedEventArgs e)
        {
            _cancellationTokenSource.Cancel();
            Game.LeaveGame(_socket, this);
        }

        public class GetGameResultsResponse
        {
            public List<PlayerResults> results { get; set; }
            public int status { get; set; }
        }

        public class PlayerResults
        {
            public string username { get; set; }
            public int correctAnswerCount { get; set; }
            public int wrongAnswerCount { get; set; }
            public double averageAnswerTime { get; set; }

            public int score { get; set; }
        }
    }
}
