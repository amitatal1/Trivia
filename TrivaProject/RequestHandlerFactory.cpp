#include "RequestHandlerFactory.h"
#include "SqliteDataBase.h"
RequestHandlerFactory::RequestHandlerFactory()
	:_db(&(SqliteDataBase::getInstance())), _loginManager(LoginManager::getInstance()), _statisticsManager(StatisticsManager::getInstance())
	, _roomManager(RoomManager::getInstance())
{

}

RequestHandlerFactory::~RequestHandlerFactory()
{

}

MenuRequestHandler* RequestHandlerFactory::createMenuRequestHandler(const string& username)
{
	return new MenuRequestHandler(username);
}

LoginRequestHandler* RequestHandlerFactory::createLoginRequestHandler()
{
	return new LoginRequestHandler;
}


GameRequestHandler* RequestHandlerFactory::createGameRequestHandler(const string& username, Game& game)
{
	return new GameRequestHandler(username, game);
}

LoginManager& RequestHandlerFactory::getLoginManager()
{
	return _loginManager;
}

StatisticsManager& RequestHandlerFactory::getStatisticsManager()
{
	return _statisticsManager;
}

RoomManager& RequestHandlerFactory::getRoomManager()
{
	return _roomManager;
}
