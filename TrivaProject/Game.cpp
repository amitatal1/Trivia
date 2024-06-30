#include "Game.h"


Game::Game(std::vector<string> players, const RoomData& roomData)
	:_gameId(roomData.id)
{
	_questions = SqliteDataBase::getInstance().getQuestions(roomData.numOfQuestionsInGame);

	for (auto& player : players)
	{
		_players.emplace( player, GameData());
	}
}


void Game::submitGameStatsToDB(string username)
{
	if (!_players.at(username).dataSubmitted )
	{
		SqliteDataBase::getInstance().submitGameStats(username, _players.at(username));
		_players.at(username).dataSubmitted = true;
	}
}



Question& Game::getQuestionForUser(string username)
{

		auto& question = _questions[(_players.at(username).questionId)++]; //gets the question that is suitable for the user.

		return question; 

}

bool Game::submitAnswer(const string username, int answerId, double time)
{
	auto& gameData = _players.at(username);
	gameData.averageAnswerTime = (double)(gameData.averageAnswerTime * (gameData.correctAnswerCount + gameData.wrongAnswerCount) + time) / (gameData.correctAnswerCount + gameData.wrongAnswerCount + 1);

	if (answerId == RIGHT_ANSWER_INDEX)
	{
		gameData.correctAnswerCount++;
		return true;
	}
	else
	{
		gameData.wrongAnswerCount++;
		return false;
	}
	

}

void Game::removePlayer(string player)
{
	_players.erase(player);
}

std::vector<PlayerResults> Game::getGameResults()
{
	std::vector<PlayerResults> results;
	for (auto& player : _players)
	{
		PlayerResults playerResults;
		auto& gameData = player.second;

		playerResults.username = player.first;
		playerResults.correctAnswerCount = gameData.correctAnswerCount;
		playerResults.wrongAnswerCount = gameData.wrongAnswerCount;
		playerResults.averageAnswerTime = gameData.averageAnswerTime;

		playerResults.score = SqliteDataBase::calculateScore(
			gameData.correctAnswerCount,
			gameData.wrongAnswerCount,
			gameData.averageAnswerTime
		);


		results.push_back(playerResults);
	}
	return results;
}

bool Game::isEmpty() const
{
	return _players.empty();
}
