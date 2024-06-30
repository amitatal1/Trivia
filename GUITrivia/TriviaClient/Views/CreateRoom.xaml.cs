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
using TriviaClient.Help;
using static TriviaClient.Views.JoinRoom;

namespace TriviaClient.Views
{
    /// <summary>
    /// Interaction logic for CreateRoom.xaml
    /// </summary>
    public partial class CreateRoom : Window
    {
        Socket _socket;
        public CreateRoom(Socket socket)
        {
            InitializeComponent();
            _socket = socket;
        }

        private void GetRoomId(RoomData data)
        {
            if (_socket.Connected)
            {
                var serializedMsg = Help.SerializationHelper.SerializePacket(SerializationHelper.MessageCode.GET_ROOM_STATE_CODE, "Get RoomId");

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
                        var response = JsonSerializer.Deserialize<GetRoomStateResponse>(bufferString);
                        if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                        {
                            MessageBox.Show("Failed to load room data", "Room Error", MessageBoxButton.OK, MessageBoxImage.Information); // should not go here but maybe in future versions
                        }
                        else
                        {

                            var Room = new AdminRoomView(_socket, data, response.players);
                            Room.Show();

                            var app = Application.Current as App;
                            app.ProgrammaticClose(this);
                        }
                    }
                }
            }
        }

        private void CreateRoomClick(object sender, RoutedEventArgs e)
        {
            string MaxPlayers = MaxPlayersInput.Text;
            string name = NameInput.Text;
            string NumberOfQuestions = NumberOfInput.Text;
            string TimePerQuestion = TimePerInput.Text;

            if (TimePerQuestion != "" && NumberOfQuestions != "" && name != "" && MaxPlayers != "" && _socket.Connected)
            {
                try
                {

                    uint maxPlayers = uint.Parse(MaxPlayers);
                    uint NumberOfQ = uint.Parse(NumberOfQuestions);
                    uint TimePerQ = uint.Parse(TimePerQuestion);

                    var _data = new RoomData { maxUsers = maxPlayers, roomName = name, questionsCount = NumberOfQ, answerTimeout = TimePerQ };

                    var serializedMsg = Help.SerializationHelper.SerializePacket(SerializationHelper.MessageCode.CREATE_ROOM_CODE, _data);

                    int rs = _socket.Send(serializedMsg);
                    if (rs > 0)
                    {
                        var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                        string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                        if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                        {
                            var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);

                            MessageBox.Show(error.message, "Creating room error", MessageBoxButton.OK, MessageBoxImage.Error);

                        }
                        else // a legitimmate reposnse came back
                        {
                            //deserialize the respose to json.
                            StatusOnlyResponse response = JsonSerializer.Deserialize<StatusOnlyResponse>(bufferString);
                            if (response.status == (int)SerializationHelper.ResponseStatuses.FAILURE_STATUS)
                            {
                                MessageBox.Show("Failed to create the room", "Room Error", MessageBoxButton.OK, MessageBoxImage.Error); //should not go here but maybe in fututre versions
                            }
                            else
                            {
                                GetRoomId(_data);
                            }
                        }
                    }
                }
                catch(FormatException)
                {
                    MessageBox.Show("Ilegal input", "Input Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                catch (Exception)
                {
                    MessageBox.Show("Error in the proceess", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }

            }
            else
            {
                MessageBox.Show("All fields should be filled", "Input Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void Home_Button_Click(object sender, RoutedEventArgs e)
        {
            var mainPage = new MainWindow(_socket);
            mainPage.Show();

            var app = Application.Current as App;
            app.ProgrammaticClose(this);
        }
    }

    public class GetRoomStateResponse
    {
        public int status { get; set; }
        public bool hasGameBegun { get; set; }
        public List<string> players { get; set; }
        public int questionCount { get; set; }
        public int answerTimeout { get; set; }
    };


    public class RoomData
    {
        public string roomName { get; set; }
        public uint maxUsers { get; set; }
        public uint questionsCount { get; set; }
        public uint answerTimeout { get; set; }

    }
}
