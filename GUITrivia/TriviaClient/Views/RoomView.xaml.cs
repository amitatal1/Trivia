using MahApps.Metro.IconPacks;
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
using System.Windows.Controls;
using TriviaClient.Help;
using static TriviaClient.Views.JoinRoom;

namespace TriviaClient.Views
{
    /// <summary>
    /// Interaction logic for RoomView.xaml
    /// </summary>
    public partial class RoomView : Window
    {
        private Socket _socket;
        private RoomData _room;
        private Thread _refreshThread;
        private CancellationTokenSource _cancellationTokenSource;
        public ObservableCollection<string> Players { get; set; }

        public RoomView(Socket socket, RoomData room)
        {
            InitializeComponent();
            _room = room;
            _socket = socket;

            Players = new ObservableCollection<string>();
            this.DataContext = this;

            LoadRoom();
        }

        private void RefreshPlayersList(object cancellationToken)
        {
            var token = (CancellationToken)cancellationToken;

            while (!token.IsCancellationRequested)
            {
                if (_socket.Connected)
                {
                    var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.GET_ROOM_STATE_CODE, "Get data");

                    int rs = _socket.Send(serializedMsg);
                    if (rs > 0)
                    {
                        var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                        string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                        if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                        {
                            var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                            Dispatcher.Invoke(() =>
                            {
                                MessageBox.Show(error.message, "Room error", MessageBoxButton.OK, MessageBoxImage.Error);
                            });
                        }
                        else // a legitimate response came back
                        {
                            var response = JsonSerializer.Deserialize<GetRoomStateResponse>(bufferString);
                            if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                            {
                                Dispatcher.Invoke(() =>
                                {
                                    MessageBox.Show("Room was Closed", "BYE BYE", MessageBoxButton.OK, MessageBoxImage.Information);
                                    _cancellationTokenSource.Cancel();

                                    var mainPage = new MainWindow(_socket);
                                    mainPage.Show();

                                    var app = Application.Current as App;
                                    app.ProgrammaticClose(this);
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
                                    OwnerName.Text = "Owner: " + Players.FirstOrDefault();
                                });

                                if (response.hasGameBegun)
                                {
                                    _cancellationTokenSource.Cancel();
                                    Dispatcher.Invoke(() =>
                                    {
                                        var gamePage = new Game(_socket, _room);
                                        gamePage.Show();

                                        var app = Application.Current as App;
                                        app.ProgrammaticClose(this);
                                    });
                                }
                                Thread.Sleep(500);
                            }
                        }
                    }
                }
            }
        }


        private void CloseRefreashThread()
        {
            _cancellationTokenSource.Cancel();


            if (_refreshThread != null && _refreshThread.IsAlive)
            {
                _refreshThread.Join();
            }

        }
        private void LoadRoom()
        {
            RoomName.Text = _room.roomName;

            _cancellationTokenSource = new CancellationTokenSource();
            _refreshThread = new Thread(new ParameterizedThreadStart(RefreshPlayersList));
            _refreshThread.Start(_cancellationTokenSource.Token);
        }

        private void LeavRoom()
        {
            if (_socket == null || !_socket.Connected) return;

            var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.LEAVE_ROOM_CODE, "Leave Game");

            if (_socket.Send(serializedMsg) > 0)
            {
                var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                string bufferString = Encoding.ASCII.GetString(resp.Buffer);

                if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                {
                    var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                    MessageBox.Show(error.message, "Error leaving Room", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

        private void Home_Button_Click(object sender, RoutedEventArgs e)
        {

            CloseRefreashThread();

            if (_refreshThread != null && _refreshThread.IsAlive)
            {
                _refreshThread.Join();
            }

            LeavRoom();

            var mainPage = new MainWindow(_socket);
            mainPage.Show();

            var app = Application.Current as App;
            app.ProgrammaticClose(this);
        }

        public class GetRoomStateResponse
        {
            public int status { get; set; }
            public List<string> players { get; set; }
            public bool hasGameBegun { get; set; }
        }

        public class GetPlayersInRoomRequest
        {
            public int roomId { get; set; }
        }
    }
}
