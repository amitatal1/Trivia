#pragma once
#include "IRequestHandler.h"
#include "Room.h"

class GeneralRoomMemberHandler
	: public IRequestHandler
{
public:
	GeneralRoomMemberHandler(const string& username, Room& room);
	virtual ~GeneralRoomMemberHandler();
	virtual bool isRequestRelevant(const RequestInfo& requestInfo) const = 0;
	virtual RequestResult handleNewRequest(RequestInfo& requestInfo) override = 0;

protected:
	RequestResult handleGetRoomState();
	Room& _room;

};


