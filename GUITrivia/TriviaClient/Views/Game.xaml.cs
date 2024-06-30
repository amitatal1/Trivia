using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Threading;
using TriviaClient.Help;

namespace TriviaClient.Views
{
    public partial class Game : Window
    {
        private const int MAX_QUESTIONS = 50;
        private const int RIGHT_ANSWER_ID = 0, WRONG_ANSWER_ID = 3;

        private Socket _socket;
        private RoomData _room;
        private DispatcherTimer _timer;
        private int _timeRemaining;
        private bool _answerSelected;
        private Button _correctAnswerButton;
        private bool _hasChosen;
        private CancellationTokenSource _cancellationTokenSource;

        public Game(Socket socket, RoomData room)
        {
            InitializeComponent();
            InitializeGame(socket, room);
            this.Closing += Window_Closing; // Handle window closing event
        }

        private void InitializeGame(Socket socket, RoomData room)
        {
            _hasChosen = false;
            _socket = socket;
            _room = room;

            totalQuestions.Text = _room.questionsCount.ToString();
            currentQuestion.Text = "0";
            _timeRemaining = (int)_room.answerTimeout;
            timerObj.Text = _timeRemaining.ToString();

            _timer = new DispatcherTimer { Interval = TimeSpan.FromSeconds(1) };
            _timer.Tick += Timer_Tick;

            _cancellationTokenSource = new CancellationTokenSource();

            GetNextQuestion();
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            _timeRemaining--;

            if (!_answerSelected)
            {
                timerObj.Text = _timeRemaining.ToString();
            }

            if (_timeRemaining <= 0)
            {
                _timer.Stop();
                if (!_hasChosen)
                    SubmitAnswer(WRONG_ANSWER_ID);

                GetNextQuestion();
            }
        }

        private void Home_Button_Click(object sender, RoutedEventArgs e)
        {
            _timer.Stop(); // Stop the timer
            _cancellationTokenSource.Cancel(); // Cancel any ongoing tasks
            LeaveGame(_socket, this);
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            _timer.Stop(); // Stop the timer
            _cancellationTokenSource.Cancel(); // Cancel any ongoing tasks
        }

        public static void LeaveGame(Socket socket, Window window)
        {
            if (socket == null || !socket.Connected) return;

            var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.LEAVE_GAME_CODE, "Leave Game");

            if (socket.Send(serializedMsg) > 0)
            {
                var resp = SerializationHelper.ReceiveRequestFromClient(socket);
                string bufferString = Encoding.ASCII.GetString(resp.Buffer);

                if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                {
                    var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                    MessageBox.Show(error.message, "Error leaving game", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    var mainPage = new MainWindow(socket);
                    mainPage.Show();
                    (Application.Current as App).ProgrammaticClose(window);
                }
            }
        }

        private async void GetNextQuestion()
        {
            _hasChosen = false;
            if (IsLastQuestion())
            {
                ShowGameResults();
                return;
            }

            if (_socket.Connected)
            {
                var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.GET_QUESTION_CODE, "Get question");

                try
                {
                    if (_socket.Send(serializedMsg) > 0)
                    {
                        var resp = await Task.Run(() => SerializationHelper.ReceiveRequestFromClient(_socket), _cancellationTokenSource.Token);
                        if (_cancellationTokenSource.Token.IsCancellationRequested) return;
                        string bufferString = Encoding.ASCII.GetString(resp.Buffer);

                        if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                        {
                            var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                            MessageBox.Show(error.message, "Error getting question", MessageBoxButton.OK, MessageBoxImage.Error);
                        }
                        else
                        {
                            HandleQuestionResponse(bufferString);
                        }
                    }
                }
                catch (TaskCanceledException)
                {
                    // Task was canceled, simply return or handle if necessary
                    return;
                }
            }
        }

        private bool IsLastQuestion() => int.Parse(currentQuestion.Text) >= _room.questionsCount || int.Parse(currentQuestion.Text) >= MAX_QUESTIONS;

        private void ShowGameResults()
        {
            var mainPage = new GameResults(_socket);
            mainPage.Show();
            (Application.Current as App).ProgrammaticClose(this);
        }

        private void HandleQuestionResponse(string bufferString)
        {
            var resp = JsonSerializer.Deserialize<GetQuestionResponse>(bufferString);
            if (resp == null) return;

            DisplayQuestion(resp);
        }

        private void DisplayQuestion(GetQuestionResponse response)
        {
            question.Text = response.question;
            LoadQuestionsToScreen(response.answers);
            currentQuestion.Text = (int.Parse(currentQuestion.Text) + 1).ToString();

            SetButtonsColorToBlack();

            _timeRemaining = (int)_room.answerTimeout;
            _answerSelected = false;
            _timer.Start();
        }

        private void LoadQuestionsToScreen(Dictionary<int, string> answers)
        {
            var answerList = ShuffleAnswers(answers.Values.ToList());
            string correctAnswer = answers[RIGHT_ANSWER_ID];

            firstOp.Content = answerList[0];
            secondOp.Content = answerList[1];
            thirdOp.Content = answerList[2];
            fourthOp.Content = answerList[3];

            _correctAnswerButton = new Button[] { firstOp, secondOp, thirdOp, fourthOp }
                .FirstOrDefault(btn => (string)btn.Content == correctAnswer);
        }

        private static List<string> ShuffleAnswers(List<string> answerList)
        {
            Random rnd = new Random();
            for (int i = answerList.Count - 1; i > 0; i--)
            {
                int j = rnd.Next(i + 1);
                string temp = answerList[i];
                answerList[i] = answerList[j];
                answerList[j] = temp;
            }

            return answerList;
        }

        private void SetButtonsColorToBlack()
        {
            firstOp.Background = Brushes.Black;
            secondOp.Background = Brushes.Black;
            thirdOp.Background = Brushes.Black;
            fourthOp.Background = Brushes.Black;
        }

        private void AnswerButton_Click(object sender, RoutedEventArgs e)
        {
            if (_hasChosen) return;

            _hasChosen = true;
            Button selectedButton = sender as Button;

            if (selectedButton == _correctAnswerButton)
            {
                selectedButton.Background = Brushes.Green;
                SubmitAnswer(RIGHT_ANSWER_ID);
            }
            else
            {
                selectedButton.Background = Brushes.Red;
                SubmitAnswer(WRONG_ANSWER_ID);
            }
        }

        private void SubmitAnswer(int answerId)
        {
            var answer = new SubmitAnswerRequest
            {
                answerId = answerId,
                time = _room.answerTimeout - _timeRemaining
            };

            if (_socket.Connected)
            {
                var serializedMsg = SerializationHelper.SerializePacket(SerializationHelper.MessageCode.SUBMIT_ANSWER_CODE, answer);

                if (_socket.Send(serializedMsg) > 0)
                {
                    var resp = SerializationHelper.ReceiveRequestFromClient(_socket);
                    string bufferString = Encoding.ASCII.GetString(resp.Buffer);
                    if (resp.ResponseCode == (byte)SerializationHelper.MessageCode.ERROR_CODE)
                    {
                        var error = JsonSerializer.Deserialize<ErrorResponse>(bufferString);
                        MessageBox.Show(error.message, "Error sending answer", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                }
            }
        }
    }

    public class LeaveGameResponse
    {
        public int status { get; set; }
    }

    public class GetQuestionResponse
    {
        public int status { get; set; }
        public string question { get; set; }
        public Dictionary<int, string> answers { get; set; }
    }

    public class SubmitAnswerRequest
    {
        public int answerId { get; set; }
        public double time { get; set; }
    }
}
