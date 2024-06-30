#pragma once
#include "GeneralRoomMemberHandler.h"
class RoomMemberRequestHandler :
    public GeneralRoomMemberHandler
{
public:
	RoomMemberRequestHandler(const string& username, Room& room);
	virtual ~RoomMemberRequestHandler();
	virtual bool isRequestRelevant(const RequestInfo& requestInfo) const override;
	virtual RequestResult handleNewRequest(RequestInfo& requestInfo) override;


private:
	RequestResult handleLeaveRoom();


};

