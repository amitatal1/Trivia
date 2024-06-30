#pragma once
#include "IRequestHandler.h"

class  MenuRequestHandler
	:public IRequestHandler
{
public:
	
	MenuRequestHandler(const string& username);
	virtual ~MenuRequestHandler();

	virtual bool isRequestRelevant(const RequestInfo& requestInfo) const override;
	virtual RequestResult handleNewRequest(RequestInfo& requestInfo) override;


private:
	RequestResult handleGetTopScores();
	RequestResult handleGetRooms();
	RequestResult handleCreateRoom(const RequestInfo& requestInfo);
	RequestResult handleGetPlayers(const RequestInfo& requestInfo);
	RequestResult handleJoinRoom(const RequestInfo& requestInfo);
	RequestResult handleLogout();
	RequestResult handleGetPersonalStats();

	

};
