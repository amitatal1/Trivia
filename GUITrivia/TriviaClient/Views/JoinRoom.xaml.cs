using MahApps.Metro.IconPacks;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net.Sockets;
using System.Numerics;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading;
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
    /// Interaction logic for JoinRoom.xaml
    /// </summary>
    public partial class JoinRoom : Window
    {
        protected Socket _socket;
        public ObservableCollection<RoomDataStruct> Rooms { get; set; }

        private bool movedRoom;
        private Thread _refreshThread;
        public JoinRoom(Socket socket)
        {
            InitializeComponent();
            this._socket = socket;

            movedRoom = false;
            Rooms = new ObservableCollection<RoomDataStruct>();
            this.DataContext = this;

            LoadRoomsToScreen();

            _refreshThread = new Thread(new ThreadStart(Refresh));
            _refreshThread.Start();
        }

        private void RefreshRooms_Click(object sender, RoutedEventArgs e)
        {
            LoadRoomsToScreen();
        }

        private void Refresh()
        {
            while (!movedRoom)
            {
                Thread.Sleep(1000);
                LoadRoomsToScreen();
            }
        }
        private void Home_Botton_Click(object sender, RoutedEventArgs e)
        {
            movedRoom = true;
            _refreshThread.Join();

            var mainPage = new MainWindow(_socket);
            mainPage.Show();

            var app = Application.Current as App;
            app.ProgrammaticClose(this);
        }
        public void JoinRoomServer(RoomDataStruct room)
        {
            if (_socket.Connected)
            {
                var data = new JoinRoomRequest { roomId = room.id };
                var serializedMsg = Help.SerializationHelper.SerializePacket(SerializationHelper.MessageCode.JOIN_ROOM_CODE, data);

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
                        var response = JsonSerializer.Deserialize<GetRoomsResponse>(bufferString);
                        if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                        {
                            MessageBox.Show("Failed to Join the room", "Room Error", MessageBoxButton.OK, MessageBoxImage.Error); // should not go here but maybe in future versions
                        }
                        else
                        {
                            MessageBox.Show("Joined the Room", "Joined Room", MessageBoxButton.OK, MessageBoxImage.Exclamation); //Joined room   
                        }
                    }
                }
            }
        }
        private void JoinRoomBtn(object sender, RoutedEventArgs e)
        {
            // Get the button that triggered the event
            Button button = (Button)sender;

            RoomDataStruct room = (RoomDataStruct)button.DataContext;

            RoomData _room = new RoomData { roomName = room.name, answerTimeout = room.timePerQuestion, maxUsers = room.maxPlayers, questionsCount = room.numOfQuestionsInGame };
            // Get the DataContext of the button, which is the RoomDataStruct object

            movedRoom = true;
            _refreshThread.Join();

            JoinRoomServer(room);

            // Now you have access to the room information, and you can use it as needed
            var Room = new RoomView(_socket, _room);
            Room.Show();

            var app = Application.Current as App;
            app.ProgrammaticClose(this); ;
        }

        private void LoadRoomsToScreen()
        {
            if (!movedRoom)
            {
                if (_socket.Connected)
                {
                    var serializedMsg = Help.SerializationHelper.SerializePacket(SerializationHelper.MessageCode.GET_ROOMS_CODE, "Get Rooms");

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
                            var response = JsonSerializer.Deserialize<GetRoomsResponse>(bufferString);
                            if (response.status != (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                            {
                                if (response.rooms == null)
                                {
                                    return;
                                }

                                Dispatcher.Invoke(() =>
                                {
                                    Rooms.Clear();

                                    // Handle successful response
                                    foreach (var room in response.rooms)
                                    {
                                        Rooms.Add(room);
                                    }
                                });
                            }
                        }
                    }
                }
            }
        }
    }


    public struct RoomDataStruct
    {
        public int id { get; set; }

        public string name { get; set; }

        public uint maxPlayers { get; set; }

        public uint numOfQuestionsInGame { get; set; }

        public uint timePerQuestion { get; set; }

        public bool isActive { get; set; }
    }


    public class GetRoomsResponse
    {
        public int status { get; set; }

        public List<RoomDataStruct> rooms { get; set; }
    }

    public class JoinRoomRequest
    {
        public int roomId { get; set; }
    };
}


