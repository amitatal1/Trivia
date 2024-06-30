#include "RoomMemberRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "RoomManager.h"

RoomMemberRequestHandler::RoomMemberRequestHandler(const string& username, Room& room)
	:GeneralRoomMemberHandler(username, room)

{
}

RoomMemberRequestHandler::~RoomMemberRequestHandler()
{

}

bool RoomMemberRequestHandler::isRequestRelevant(const RequestInfo& requestInfo) const
{
	return
		(
			requestInfo.requestCode == LEAVE_ROOM_CODE
			|| requestInfo.requestCode == GET_ROOM_STATE_CODE
			);
}

RequestResult RoomMemberRequestHandler::handleNewRequest(RequestInfo& requestInfo)
{
	switch (requestInfo.requestCode)
	{
	case LEAVE_ROOM_CODE:
		return handleLeaveRoom();
	case GET_ROOM_STATE_CODE:
		return handleGetRoomState();
	default:
		return generateErrorResponse("Request Didn't match");
	}
}

RequestResult RoomMemberRequestHandler::handleLeaveRoom()
{
	LeaveRoomResponse resp = { SUCCESS_STATUS }; // leaving room
	_room.removeUser(_user);
	return createResult(resp, RequestHandlerFactory::createMenuRequestHandler(_user.getUsername()));
}
