#include "RoomAdminRequestHandler.h"
#include "RoomManager.h"
#include "RequestHandlerFactory.h"
#include "GameManager.h"
#include <Windows.h>
#include <thread>

RoomAdminRequestHandler::RoomAdminRequestHandler(const string& username, Room& room)
	:GeneralRoomMemberHandler(username, room)

{
}

RoomAdminRequestHandler::~RoomAdminRequestHandler()
{
}

bool RoomAdminRequestHandler::isRequestRelevant(const RequestInfo& requestInfo) const
{
	return (requestInfo.requestCode == CLOSE_ROOM_CODE
		|| requestInfo.requestCode == START_GAME_CODE
		|| requestInfo.requestCode == GET_ROOM_STATE_CODE);


}

RequestResult RoomAdminRequestHandler::handleNewRequest(RequestInfo& requestInfo)
{
	switch (requestInfo.requestCode)
	{
	case CLOSE_ROOM_CODE:
		return handleCloseRoom();
	case START_GAME_CODE:
		return handleStartGame();
	case GET_ROOM_STATE_CODE:
		return handleGetRoomState();
	default:
		return generateErrorResponse("Request Didn't match");
	}
}

RequestResult RoomAdminRequestHandler::handleCloseRoom()
{
	RoomManager::getInstance().deleteRoom(_room.getRoomData().id); // deletes the room
	CloseRoomResponse resp = { SUCCESS_STATUS };

	return 	createResult(resp, RequestHandlerFactory::createMenuRequestHandler(_user.getUsername()));

}

RequestResult RoomAdminRequestHandler::handleStartGame()
{
	try
	{
		_room.activateRoom(true); // activates the room
		Game& game = GameManager::getInstance().createGame(_room);
		StartGameResponse resp = { SUCCESS_STATUS };


		std::thread roomClosingThread(&RoomAdminRequestHandler::wait5SecondsAndCloseRoom, this);
		roomClosingThread.detach();

		return 	createResult(resp, RequestHandlerFactory::createGameRequestHandler(_user.getUsername(),game));
	}
	catch (const std::exception&)
	{
		return generateErrorResponse("Couldnt start game");
	}
	
}

void RoomAdminRequestHandler::wait5SecondsAndCloseRoom()
{
	Sleep(3200); //waiting for everybody to join the game before erasing room.
	RoomManager::getInstance().deleteRoom(_room.getRoomData().id); // deletes the room
}


