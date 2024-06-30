#pragma once
#include "GeneralRoomMemberHandler.h"

class RoomAdminRequestHandler
	: public GeneralRoomMemberHandler
{
public:
	RoomAdminRequestHandler(const string& username, Room& room);
	virtual ~RoomAdminRequestHandler();
	virtual bool isRequestRelevant(const RequestInfo& requestInfo) const override;
	virtual RequestResult handleNewRequest(RequestInfo& requestInfo) override;
private:
	RequestResult handleCloseRoom();
	RequestResult handleStartGame();


	void wait5SecondsAndCloseRoom();
};


