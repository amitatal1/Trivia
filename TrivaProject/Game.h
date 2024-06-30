#pragma once
#include "SqliteDataBase.h"
#include "map"

class Game
{
private: //maybe we should add an indicator to check whether p;ayer finished or not
	std::vector<Question> _questions;
	std::map<string, GameData>_players;
	int _gameId;

public:
	Game(std::vector<string> players,const RoomData& roomData );
	Question& getQuestionForUser(string username);
	void submitGameStatsToDB(string username);

	bool submitAnswer(const string username, int answerId, double time);
	void removePlayer(string player);
	std::vector<PlayerResults> getGameResults();

	bool isEmpty() const;

	 int getGameId() const
	 { 
		 return _gameId; 
	 }

};

