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
using static TriviaClient.Views.JoinRoom;

namespace TriviaClient.Views
{
    public partial class AdminRoomView : Window
    {
        public Socket _socket;
        public RoomData _room;
        public ObservableCollection<string> Players { get; set; }
        private CancellationTokenSource _cancellationTokenSource;

        public AdminRoomView(Socket socket, RoomData room, List<string> players)
        {
            InitializeComponent();

            _cancellationTokenSource = new CancellationTokenSource();

            _room = room;
            _socket = socket;

            Players = new ObservableCollection<string>();
            this.DataContext = this;

            foreach (var p in players)
            {
                Players.Add(p);
            }

            loadRoom();
        }

        private async void StartGameBtnClick(object sender, RoutedEventArgs e)
        {
            if (_socket != null && _socket.Connected)
            {
                var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.START_GAME_CODE, "start game");

                int rs = _socket.Send(serializedMsg);
                if (rs > 0)
                {
                    var resp = await Task.Run(() => SerializationHelper.ReceiveRequestFromClient(_socket));
                    string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                    if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                    {
                        var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                        Dispatcher.Invoke(() =>
                        {
                            MessageBox.Show(error.message, "Joining room error", MessageBoxButton.OK, MessageBoxImage.Error);
                        });
                    }
                    else
                    {
                        var response = JsonSerializer.Deserialize<StartGameResponse>(bufferString);
                        if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                        {
                            Dispatcher.Invoke(() =>
                            {
                                MessageBox.Show("Failed to start the game", "Room Error", MessageBoxButton.OK, MessageBoxImage.Error);
                            });
                        }
                        else
                        {
                            _cancellationTokenSource.Cancel();

                            Dispatcher.Invoke(() =>
                            {
                                var gameView = new Game(_socket, _room);
                                gameView.Show();
                                var app = Application.Current as App;
                                app.ProgrammaticClose(this);
                            });
                        }
                    }
                }
            }
        }

        private async Task RefreshPlayersListAsync(CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                try
                {
                    await Task.Delay(1000, token); // Check for cancellation every second
                    if (_socket.Connected)
                    {
                        var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.GET_ROOM_STATE_CODE, "Get data");

                        int rs = _socket.Send(serializedMsg);
                        if (rs > 0)
                        {
                            var resp = await Task.Run(() => SerializationHelper.ReceiveRequestFromClient(_socket), token);
                            string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                            if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                            {
                                var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                                Dispatcher.Invoke(() =>
                                {
                                    MessageBox.Show(error.message, "refresh room error", MessageBoxButton.OK, MessageBoxImage.Error);
                                });
                            }
                            else
                            {
                                var response = JsonSerializer.Deserialize<GetRoomStateResponse>(bufferString);
                                if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                                {
                                    Dispatcher.Invoke(() =>
                                    {
                                        MessageBox.Show("Failed to Join the room", "Room Error", MessageBoxButton.OK, MessageBoxImage.Error);
                                    });
                                }
                                else
                                {
                                    Dispatcher.Invoke(() =>
                                    {
                                        Players.Clear();
                                        foreach (var p in response.players)
                                        {
                                            Players.Add(p);
                                        }
                                    });
                                }
                            }
                        }
                    }
                }
                catch (OperationCanceledException)
                {
                    break;
                }
                catch (Exception ex)
                {
                    Dispatcher.Invoke(() =>
                    {
                        MessageBox.Show(ex.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    });
                }
            }
        }

        public async void loadRoom()
        {
            OwnerName.Text = "Owner: " + Players.FirstOrDefault();
            await RefreshPlayersListAsync(_cancellationTokenSource.Token); // Start refreshing the players list
        }

        private async void CloseRoom()
        {
            if (_socket != null && _socket.Connected)
            {
                var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.CLOSE_ROOM_CODE, "close room");

                int rs = _socket.Send(serializedMsg);
                if (rs > 0)
                {
                    var resp = await Task.Run(() => SerializationHelper.ReceiveRequestFromClient(_socket));
                    string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                    if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                    {
                        var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                        Dispatcher.Invoke(() =>
                        {
                            MessageBox.Show(error.message, "closing room error", MessageBoxButton.OK, MessageBoxImage.Error);
                        });
                    }
                    else
                    {
                        var response = JsonSerializer.Deserialize<CloseRoomResponse>(bufferString);
                        if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                        {
                            Dispatcher.Invoke(() =>
                            {
                                MessageBox.Show("Failed to close the room", "Room Error", MessageBoxButton.OK, MessageBoxImage.Error);
                            });
                        }
                        else
                        {
                            _cancellationTokenSource.Cancel();

                            Dispatcher.Invoke(() =>
                            {
                                var mainPage = new MainWindow(_socket);
                                mainPage.Show();

                                var app = Application.Current as App;
                                app.ProgrammaticClose(this);
                            });
                        }
                    }
                }
            }
        }

        private void CloseRoomBtnClick(object sender, RoutedEventArgs e)
        {
            CloseRoom();
        }

        private void Home_Botton_Click(object sender, RoutedEventArgs e)
        {
            CloseRoom();
        }
    }

    public class GetPlayersInRoomResponse
    {
        public int status { get; set; }
        public List<string> players { get; set; }
    }

    public class CloseRoomResponse
    {
        public int status { get; set; }
    }

    public class StartGameResponse
    {
        public int status { get; set; }
    }
}
