#include "SqliteDataBase.h"

#include <windows.h>  
#include <wininet.h>
#include <wingdi.h>

#include <stdexcept>
#include <iostream>
#include <io.h>
#include "SqlException.h"
#include "json.hpp"
#include <algorithm>
#include <sstream>

#include "HTMLParser.h"

#pragma comment(lib, "wininet.lib")


using namespace nlohmann;

#define QUESTIONS_API_URL "https://opentdb.com/api.php?amount=50&difficulty=easy&type=multiple"

#define QUESTIONS "{\"response_code\":0,\"results\":[{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"History\",\"question\":\"The original Roman alphabet lacked the following letters EXCEPT:\",\"correct_answer\":\"X\",\"incorrect_answers\":[\"W\",\"U\",\"J\"]},{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"Entertainment: Television\",\"question\":\"Who is the star of the AMC series Breaking Bad?\",\"correct_answer\":\"Walter White\",\"incorrect_answers\":[\"Saul Goodman\",\"Jesse Pinkman\",\"Skyler White\"]},{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"Entertainment: Cartoon & Animations\",\"question\":\"In the show \\\"Steven Universe\\\", who are the main two employees of The Big Donut?\",\"correct_answer\":\"Sadie and Lars\",\"incorrect_answers\":[\"Steven and James\",\"Erik and Julie\",\"Bob and May\"]},{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"Entertainment: Television\",\"question\":\"In the show, Doctor Who, what does T.A.R.D.I.S stand for?\",\"correct_answer\":\"Time And Relative Dimensions In Space\",\"incorrect_answers\":[\"Time And Resting Dimensions In Space\",\"Time And Relative Dimensions In Style\",\"Toilet Aid Rope Dog Is Soup\"]},{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"Science: Computers\",\"question\":\"What does the Prt Sc button do?\",\"correct_answer\":\"Captures what's on the screen and copies it to your clipboard\",\"incorrect_answers\":[\"Nothing\",\"Saves a .png file of what's on the screen in your screenshots folder in photos\",\"Closes all windows\"]},{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"Sports\",\"question\":\"What was the final score of the Germany vs. Brazil 2014 FIFA World Cup match?\",\"correct_answer\":\"7 - 1\",\"incorrect_answers\":[\"0 - 1\",\"3 - 4\",\"16 - 0\"]},{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"Science & Nature\",\"question\":\"What is the powerhouse of the cell?\",\"correct_answer\":\"Mitochondria\",\"incorrect_answers\":[\"Ribosome\",\"Redbull\",\"Nucleus\"]},{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"Entertainment: Television\",\"question\":\"In the TV show \\\"Mad Men\\\", what was Donald Draper's birthname?\",\"correct_answer\":\"Richard \\\"Dick\\\" Whitman\",\"incorrect_answers\":[\"Donald Draper\",\"John Ashbury\",\"Michael \\\"Mikey\\\" Wilhelm\"]},{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"Vehicles\",\"question\":\"Where are the cars of the brand \\\"Ferrari\\\" manufactured?\",\"correct_answer\":\"Italy\",\"incorrect_answers\":[\"Romania\",\"Germany\",\"Russia\"]},{\"type\":\"multiple\",\"difficulty\":\"easy\",\"category\":\"Sports\",\"question\":\"What team won the 2016 MLS Cup?\",\"correct_answer\":\"Seattle Sounders\",\"incorrect_answers\":[\"Colorado Rapids\",\"Toronto FC\",\"Montreal Impact\"]}]}"

#define DATABASE_NAME "TriviaUsersDB.sqlite"

#define FIRST_LOG 1
void SqliteDataBase::submitGameStats(std::string username, GameData gameData)
{
	// SQL statement to insert game statistics into the STATISTICS table
	const char* sql = "INSERT INTO STATISTICS (USER_NAME, GAME_NUMBER, CORRECT_ANSWERS, TOTAL_ANSWERS, TOTAL_TIME, SCORE) VALUES (?, ?, ?, ?, ?, ?);";

	sqlite3_stmt* stmt = nullptr;

	// Prepare the SQL statement
	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		// Bind values to the SQL statement
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, gameData.questionId); // Assuming gameData.questionId is used as GAME_NUMBER
		sqlite3_bind_int(stmt, 3, gameData.correctAnswerCount);
		sqlite3_bind_int(stmt, 4, gameData.correctAnswerCount + gameData.wrongAnswerCount); // Total answers
		sqlite3_bind_int(stmt, 5, gameData.averageAnswerTime); // Average answer time
		int score = calculateScore(gameData.correctAnswerCount, gameData.wrongAnswerCount, gameData.averageAnswerTime); // Implement your score calculation logic here
		sqlite3_bind_int(stmt, 6, score);


		// Execute the SQL statement
		if (sqlite3_step(stmt) != SQLITE_DONE)
		{
			std::string errMsg = sqlite3_errmsg(_db);
			sqlite3_finalize(stmt);
			throw SQLException("Error inserting game stats: " + errMsg, sql);
		}

		// Finalize the statement
		sqlite3_finalize(stmt);
	}
	else 
	{
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}
}

int SqliteDataBase::calculateScore(int correctAnswers, int wrongAnswers, double time) 
{
	return (correctAnswers * 10 - wrongAnswers * 5)/time;
}

void SqliteDataBase::updateUsersLogs(std::string username)
{
	const char* sql = "UPDATE Users SET LOGS = LOGS + 1 WHERE USER_NAME = ?;";

	sqlite3_stmt* stmt = nullptr;

	// Prepare the SQL statement
	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		// Bind values to the SQL statement
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	

		// Execute the SQL statement
		if (sqlite3_step(stmt) != SQLITE_DONE)
		{
			std::string errMsg = sqlite3_errmsg(_db);
			sqlite3_finalize(stmt);
			throw SQLException("Error Updating user logs: " + errMsg, sql);
		}

		// Finalize the statement
		sqlite3_finalize(stmt);
	}
	else
	{
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}
}

int SqliteDataBase::fetchUserLogs(std::string username)
{
	const char* sql = "SELECT LOGS FROM Users WHERE USER_NAME = ?;";


	sqlite3_stmt* stmt = nullptr;
	int logs = 0;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) 
	{
		sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) 
		{
			logs = sqlite3_column_int(stmt, 0);
		}

		sqlite3_finalize(stmt);
	}
	else {
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}

	return logs;
}

SqliteDataBase::~SqliteDataBase()
{
	close();
}

SqliteDataBase::SqliteDataBase()
{
	open();
}

void SqliteDataBase::createUsersTable() 
{
	std::string sqlStatement =
		"CREATE TABLE IF NOT EXISTS USERS ( \
            USER_NAME TEXT PRIMARY KEY NOT NULL, \
            PASSWORD TEXT NOT NULL, \
            EMAIL TEXT NOT NULL ,\
			LOGS INTEGER NOT NULL\
        );";
	exec_cmd(sqlStatement, nullptr, nullptr);
}



bool SqliteDataBase::open()
{

	int file_exist = _access(DATABASE_NAME, 0);
	int res = sqlite3_open(DATABASE_NAME, &_db);
	if (res != SQLITE_OK)
	{
		_db = nullptr;
		std::cout << "Failed to open DB" << std::endl;
		return false;
	}

	exec_cmd("PRAGMA foreign_keys = ON;", nullptr, nullptr);

	reloadQuestionsTable(); //Reloads questions

	if (file_exist != 0)
	{
		createStatisticsTable();
		createUsersTable();
	}

	//in future
	//createQuestionsTable();
	return true;
}
bool SqliteDataBase::close() 
{
	if (_db) {
		int res = sqlite3_close(_db);
		_db = nullptr;
		return res == SQLITE_OK;
	}
	return false;
}

bool SqliteDataBase::doesUserExist(const std::string& userName) 
{
	const char* sql = "SELECT 1 FROM USERS WHERE USER_NAME = ? LIMIT 1;";

	sqlite3_stmt* stmt = nullptr;
	bool exists = false;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			exists = true;
		}

		sqlite3_finalize(stmt);
	}
	else {
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}

	return exists;
}

bool SqliteDataBase::doesPasswordMatch(const std::string& userName, const std::string& password)
{
	const char* sql = "SELECT 1 FROM USERS WHERE USER_NAME = ? AND PASSWORD = ?;";

	sqlite3_stmt* stmt = nullptr;
	bool match = false;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			match = true;
		}

		sqlite3_finalize(stmt);
	}
	else {
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}

	return match;
}

void SqliteDataBase::addNewUser(const std::string& userName, const std::string& password, const std::string& email) 
{
	const char* sql = "INSERT INTO USERS (USER_NAME, PASSWORD, EMAIL,LOGS) VALUES (?, ?, ?,?);";

	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 4,FIRST_LOG);

		if (sqlite3_step(stmt) != SQLITE_DONE) {
			std::string errMsg = sqlite3_errmsg(_db);
			sqlite3_finalize(stmt);
			throw SQLException("Error inserting user: " + errMsg, sql);
		}
		sqlite3_finalize(stmt);
	}
	else
	{
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}
}

float SqliteDataBase::getPlayerAvarageAnswerTime(const std::string userName)
{
	const char* sql = "SELECT AVG(TOTAL_TIME) / AVG(TOTAL_ANSWERS) FROM STATISTICS WHERE USER_NAME = ?;";


	sqlite3_stmt* stmt = nullptr;
	float avg = 0;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			avg += sqlite3_column_int(stmt, 0);
		}

		sqlite3_finalize(stmt);
	}
	else {
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}

	return avg;
}

int SqliteDataBase::getNumberOfCorrectAnswers(const std::string userName)
{
	const char* sql = "SELECT sum(correct_answers) FROM STATISTICS WHERE USER_NAME = ?;";


	sqlite3_stmt* stmt = nullptr;
	int num = 0;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			num += sqlite3_column_int(stmt, 0);
		}

		sqlite3_finalize(stmt);
	}
	else {
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}

	return num;
}

int SqliteDataBase::getNumberOfTotalAnswers(const std::string userName)
{
	const char* sql = "SELECT sum(TOTAL_ANSWERS) FROM STATISTICS WHERE USER_NAME = ?;";


	sqlite3_stmt* stmt = nullptr;
	int num = 0;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			num += sqlite3_column_int(stmt, 0);
		}

		sqlite3_finalize(stmt);
	}
	else {
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}

	return num;
}

int SqliteDataBase::getNumOfPlayerGames(const std::string userName)
{
	const char* sql = "SELECT count(ID) FROM STATISTICS WHERE USER_NAME = ?;";


	sqlite3_stmt* stmt = nullptr;
	int num = 0;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(stmt) == SQLITE_ROW) {
			num += sqlite3_column_int(stmt, 0);
		}

		sqlite3_finalize(stmt);
	}
	else {
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}

	return num;
}

std::vector<std::string> SqliteDataBase::topFiveScores()
{
	const char* sql = "SELECT USER_NAME, SCORE FROM STATISTICS ORDER BY SCORE DESC LIMIT 5;";
	std::vector<std::string> result;

	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));  // Fetch name as a string
			int score = sqlite3_column_int(stmt, 1);  // Fetch score as an integer
			std::ostringstream temp;  // Create a string stream
			temp << name << ": " << score;  // Concatenate name and score
			result.push_back(temp.str());  // Push string representation into result vector
		}

		sqlite3_finalize(stmt);
	}
	else {
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}

	return result;
}

std::vector<std::string> SqliteDataBase::playerStats(std::string userName)
{
	std::vector<std::string> result;

	result.push_back(std::to_string(getNumOfPlayerGames(userName)));
	result.push_back(std::to_string(getNumberOfTotalAnswers(userName)));
	result.push_back(std::to_string(getNumberOfCorrectAnswers(userName)));
	result.push_back(std::to_string(getPlayerAvarageAnswerTime(userName)));

	return result;
}



std::vector<Question> SqliteDataBase::getQuestions(int questionsNumber) const
{
	std::vector<Question> questions;

	const char* sql = "SELECT QUESTION, CORRECT_ANSWER, INCORRECT_ANSWER_1, INCORRECT_ANSWER_2, INCORRECT_ANSWER_3 FROM QUESTIONS LIMIT ?";
	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, questionsNumber); 

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			string question(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
			string correctAnswer(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
			string falseAnswer1(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
			string falseAnswer2(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
			string falseAnswer3(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));

			std::vector<string> falseAnswers = { falseAnswer1, falseAnswer2, falseAnswer3 };

			questions.emplace_back(question, correctAnswer, falseAnswers); //adding the question to the vector of results.
		}

		std::random_shuffle(questions.begin(), questions.end());
		sqlite3_finalize(stmt);
	}
	else
	{
		throw SQLException(std::string(sqlite3_errmsg(_db)), sql);
	}

	return questions;
}



void SqliteDataBase::exec_cmd(const std::string& cmd, void* data, int(*callback)(void*, int, char**, char**))
{
	int res = sqlite3_exec(_db, cmd.c_str(), callback, data, nullptr);
	if (res != SQLITE_OK) {
		std::string errorMessage = "Error executing statement: ";
		errorMessage += sqlite3_errmsg(_db);
		throw SQLException(errorMessage, cmd.c_str(), res);
	}
}

void SqliteDataBase::reloadQuestionsTable()
{
	// Creation of the table
	

	std::string sqlStatement =
		"DROP TABLE IF EXISTS QUESTIONS;";
	exec_cmd(sqlStatement, nullptr, nullptr);

	sqlStatement =
		"CREATE TABLE IF NOT EXISTS QUESTIONS("
		"QUESTION_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"QUESTION TEXT NOT NULL,"
		"INCORRECT_ANSWER_1 TEXT NOT NULL,"
		"INCORRECT_ANSWER_2 TEXT NOT NULL,"
		"INCORRECT_ANSWER_3 TEXT NOT NULL,"
		"CORRECT_ANSWER TEXT NOT NULL"
		");";
	exec_cmd(sqlStatement, nullptr, nullptr);

	// Adding the questions to the table
	std::string jsonQuestions = HTPPSRequest(QUESTIONS_API_URL);

	// Parse JSON string
	json questionsJson = json::parse(jsonQuestions);

	// Iterate over questions
	for (const auto& question : questionsJson["results"])
	{
		std::string questionText = question["question"];
		std::string correctAnswer = question["correct_answer"];
		std::string incorrectAnswer1 = question["incorrect_answers"][0];
		std::string incorrectAnswer2 = question["incorrect_answers"][1];
		std::string incorrectAnswer3 = question["incorrect_answers"][2];

		// Insert question into database
		std::string insertStatement = "INSERT INTO QUESTIONS (QUESTION, INCORRECT_ANSWER_1, INCORRECT_ANSWER_2, INCORRECT_ANSWER_3, CORRECT_ANSWER) VALUES (?, ?, ?, ?, ?);";
		sqlite3_stmt* stmt = nullptr;

		if (sqlite3_prepare_v2(_db, insertStatement.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
		{
			sqlite3_bind_text(stmt, 1, questionText.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 2, incorrectAnswer1.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 3, incorrectAnswer2.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 4, incorrectAnswer3.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 5, correctAnswer.c_str(), -1, SQLITE_STATIC);

			if (sqlite3_step(stmt) != SQLITE_DONE)
			{
				std::string errMsg = sqlite3_errmsg(_db);
				sqlite3_finalize(stmt);
				throw SQLException("Error inserting question: " + errMsg, insertStatement);
			}
			sqlite3_finalize(stmt);
		}
		else
		{
			throw SQLException(std::string(sqlite3_errmsg(_db)), insertStatement);
		}
	}
}

void SqliteDataBase::createStatisticsTable()
{
	std::string sqlStatement =
		"CREATE TABLE IF NOT EXISTS STATISTICS("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"USER_NAME TEXT NOT NULL REFERENCES \"USERS\"(\"USER_NAME\"),"
		"GAME_NUMBER INTEGER NOT NULL,"
		"CORRECT_ANSWERS INTEGER NOT NULL,"
		"TOTAL_ANSWERS INTEGER NOT NULL,"
		"TOTAL_TIME INTEGER NOT NULL,"
		"SCORE INTEGER NOT NULL"
		")";
	exec_cmd(sqlStatement, nullptr, nullptr);
}

std::string SqliteDataBase::HTPPSRequest(std::string url)
{

		HINTERNET hInternet = InternetOpen(L"HTTP Request", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (!hInternet)
		{
			throw std::exception("InternetOpen has failed");
		}

		HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_SECURE, 0);
		if (!hConnect)
		{
			InternetCloseHandle(hInternet);
			throw std::exception("InternetOpenUrlA has failed");
		}

		const DWORD DATA_SIZE = 15000;
		char DATA_RECIEVED[DATA_SIZE];
		std::string response;
		DWORD bytesRead = 0;

		while (InternetReadFile(hConnect, DATA_RECIEVED, DATA_SIZE, &bytesRead) && bytesRead > 0)
		{
			response.append(DATA_RECIEVED, bytesRead);
		}

		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);

		
		return HTMLParser::ConverHTMLToNormal(response);
	}



