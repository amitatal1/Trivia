#pragma once
#include "LoginManager.h"
#include "IDataBase.h"
#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"
#include "RoomManager.h"`
#include "StatisticsManager.h"
#include "GameRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"



class LoginRequestHandler;

class RequestHandlerFactory
	:public Singleton<RequestHandlerFactory>
{
public:
	friend class Singleton<RequestHandlerFactory>;
	~RequestHandlerFactory();


	static MenuRequestHandler* createMenuRequestHandler(const string& username);
	static LoginRequestHandler* createLoginRequestHandler();

	template<typename T>
	static T* createRoomRequestHandler(const string& username, Room& room)
	{
		return new T(username, room);
	}
	static GameRequestHandler* createGameRequestHandler(const string& username, Game& game);


	LoginManager& getLoginManager();
	StatisticsManager& getStatisticsManager();
	RoomManager& getRoomManager();


private:
	LoginManager& _loginManager;
	IDataBase* _db;
	RoomManager& _roomManager;
	StatisticsManager& _statisticsManager;


protected:
	RequestHandlerFactory();
};