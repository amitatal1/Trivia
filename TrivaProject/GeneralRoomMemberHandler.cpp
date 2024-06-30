#include "GeneralRoomMemberHandler.h"
#include "RequestHandlerFactory.h"


GeneralRoomMemberHandler::GeneralRoomMemberHandler(const string& username, Room& room)
	:IRequestHandler(username), _room(room)

{
}
GeneralRoomMemberHandler::~GeneralRoomMemberHandler()
{
}
RequestResult GeneralRoomMemberHandler::handleGetRoomState()
{
	RequestResult result;
	try
	{
		const RoomData& data = _room.getRoomData();

		GetRoomStateResponse resp =
		{
			SUCCESS_STATUS,
			data.isActive,
			_room.getAllUsers(),
			data.numOfQuestionsInGame,
			data.timePerQuestion
		};

		if (data.isActive)
		{
			return createResult(resp,
				RequestHandlerFactory::createGameRequestHandler(_user.getUsername(),GameManager::getInstance().getGame(data.id)));
		}
		else
		{
			return createResult(resp, this);
		}
	}
	catch (const std::exception&) // room was closed or whatEver
	{
		GetRoomStateResponse resp = { FAILURE_STATUS };  // leaved room because it was closed.
		return createResult(resp, 
			RequestHandlerFactory::createMenuRequestHandler(_user.getUsername()));
	}
	

}

