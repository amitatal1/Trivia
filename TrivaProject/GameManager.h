#pragma once
#include "Game.h"
#include "Room.h"
#include "unordered_map"
#include "Singleton.h"
class GameManager
	: public Singleton<GameManager>
{
	friend class Singleton<GameManager>;
private:
	std::unordered_map<int,Game> _games;
	
public:
	Game& createGame(Room room);

	void deleteGame(int gameId);
	Game& getGame(int gameId);

protected:
	GameManager() =default;
};

