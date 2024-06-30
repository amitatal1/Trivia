#pragma once
#include <string>
#include <vector>
#include <map>



using std::string;
using Buffer = std::vector<char>;


//constants 
#define MESSAGE_CODE_INDEX 0
#define LENGTH_FIELD_SIZE 4
#define DATA_FIRST_BYTE 5
#define LEN_STARTING_INDEX 1




enum MessageCode
{
	ERROR_CODE = 0,

	LOGIN_CODE = 1,
	SIGN_UP_CODE,
	LOGOUT_CODE,

	GET_ROOMS_CODE,
	GET_PLAYERS_CODE,
	JOIN_ROOM_CODE,
	CREATE_ROOM_CODE,
	GET_HIGH_SCORE_CODE,
	GET_PERSONAL_STATS_CODE,

	CLOSE_ROOM_CODE,
	START_GAME_CODE,
	GET_ROOM_STATE_CODE,
	LEAVE_ROOM_CODE,

	SUBMIT_ANSWER_CODE,
	GET_GAME_RESULTS_CODE,
	GET_QUESTION_CODE,
	LEAVE_GAME_CODE,

};

enum Error_Messages
{
	LOGIN_ERROR = 1
};

enum Request_Statuses
{
	SUCCESS_STATUS = 1,
	FAILURE_STATUS = 2
};
const std::map<Error_Messages, string> ErrorsList =
{
	{ LOGIN_ERROR , "Couldn't login, wrong format"}

};


//part 1 structs
struct  LoginResponse
{
	int status;
};

struct  SignupResponse
{
	int status;

};

struct  LoginRequest
{
	string username;
	string password;
};

struct  SignupRequest
{
	string username;
	string password;
	string email;
};


struct RequestInfo
{

	char requestCode;
	char id;
	Buffer buffer;
};

struct ErrorResponse
{
	int status;
	string message;
};


//part 2 structs

struct RoomData
{
	int id;
	string name;
	unsigned int maxPlayers;
	unsigned int numOfQuestionsInGame;
	unsigned int timePerQuestion;
	bool isActive;
};



struct LogoutResponse
{
	int status;
};

struct JoinRoomResponse
{
	int status;
};  

struct CreateRoomResponse
{
	int status;
};

struct GetRoomsResponse
{
	int status;
	std::vector<RoomData> rooms;
};

struct GetPlayersInRoomResponse
{
	int status;
	std::vector<string> players;
};

struct getHighScoreResponse
{
	int status;
	std::vector<string> statistics;
};

struct GetPersonalStatsResponse
{
	int status;
	std::vector<string> statistics;
};


//Requests
struct GetPlayersInRoomRequest
{
	int roomId;
};

struct JoinRoomRequest
{
	int roomId;
};


struct CreateRoomRequest
{
	string roomName;
	int maxUsers;
	int questionsCount;
	int answerTimeout;
};


//Part 3

//responses

struct CloseRoomResponse
{
	int status;
};
struct StartGameResponse
{
	int status;
};
struct GetRoomStateResponse
{
	int status;
	bool hasGameBegun;
	std::vector<string> players;
	int questionCount;
	int answerTimeout;
};

struct LeaveRoomResponse
{
	int status;
};

//part 4

//data structs

struct PlayerResults
{
	string username;

	int correctAnswerCount ;
	int wrongAnswerCount;
	double averageAnswerTime;
	int score;

};

struct GameData
{
	int correctAnswerCount =0;
	int wrongAnswerCount = 0;
	double averageAnswerTime = 0;
	int questionId = 0;

	bool dataSubmitted = false;
};

//responses
struct LeaveGameResponse
{
	int status;
};

struct GetQuestionResponse
{
	int status;
	string question;
	std::map<int, string> answers;
};

struct SubmitAnswerResponse
{
	int status;
	int correctAnswerId  ;
};

struct GetGameResultsResponse
{
	int status;
	std::vector<PlayerResults> results;
};

//requests
struct SubmitAnswerRequest
{
	int answerId;
	double time;
};