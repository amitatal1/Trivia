#include "GameManager.h"

Game& GameManager::createGame(Room room)
{
	const RoomData& data = room.getRoomData();
	_games.emplace(data.id, Game(room.getAllUsers(), data));

	return _games.at(data.id);
}

void GameManager::deleteGame(int gameId)
{
	_games.erase(gameId); 
}

Game& GameManager::getGame(int gameId)
{
	return _games.at(gameId);
}

