#include "MenuRequestHandler.h"
#include "RoomManager.h"
#include "LoginManager.h"
#include "RequestHandlerFactory.h"


MenuRequestHandler::MenuRequestHandler(const std::string& username)
    : IRequestHandler(username)
{
}

MenuRequestHandler::~MenuRequestHandler()
{
}

bool MenuRequestHandler::isRequestRelevant(const RequestInfo& requestInfo) const
{
    return (requestInfo.requestCode == GET_ROOMS_CODE ||
        requestInfo.requestCode == CREATE_ROOM_CODE ||
        requestInfo.requestCode == GET_PERSONAL_STATS_CODE ||
        requestInfo.requestCode == JOIN_ROOM_CODE ||
        requestInfo.requestCode == GET_PLAYERS_CODE ||
        requestInfo.requestCode == LOGOUT_CODE || 
        requestInfo.requestCode == GET_HIGH_SCORE_CODE);
}



RequestResult MenuRequestHandler::handleNewRequest(RequestInfo& requestInfo)
{
    switch (requestInfo.requestCode)
    {
    case GET_ROOMS_CODE:
        return handleGetRooms();
    case CREATE_ROOM_CODE:
        return handleCreateRoom(requestInfo);
    case GET_PLAYERS_CODE:
        return handleGetPlayers(requestInfo);
    case JOIN_ROOM_CODE:
        return handleJoinRoom(requestInfo);
    case LOGOUT_CODE:
        return handleLogout();
    case GET_PERSONAL_STATS_CODE:
        return handleGetPersonalStats();
    case GET_HIGH_SCORE_CODE:
        return handleGetTopScores();
    default:
        return generateErrorResponse("Request Didn't match");
    }
}

RequestResult MenuRequestHandler::handleGetTopScores()
{
    getHighScoreResponse response;
    response.statistics = StatisticsManager::getInstance().getHighScore();
    response.status = SUCCESS_STATUS;

    return createResult(response, this);
}

RequestResult MenuRequestHandler::handleGetRooms()
{
    GetRoomsResponse response;
    response.rooms = RoomManager::getInstance().getRooms();
    response.status = SUCCESS_STATUS;

    return createResult(response, this);
}

RequestResult MenuRequestHandler::handleCreateRoom(const RequestInfo& requestInfo)
{
    CreateRoomResponse response;
    auto req = JsonRequestPacketDeserializer::deserializeCreateRoomRequest(requestInfo.buffer);
    RoomData roomData;

    // Setting room data
    RoomManager& roomManager = RoomManager::getInstance();
    roomData.id = roomManager.getLastRoomId();
    roomData.maxPlayers = req.maxUsers;
    roomData.name = req.roomName;
    roomData.numOfQuestionsInGame = req.questionsCount;
    roomData.timePerQuestion = req.answerTimeout;
    roomData.isActive = false;


    Room& newRoom = roomManager.createRoom(_user, roomData);
    response.status = SUCCESS_STATUS;

    return createResult(response, RequestHandlerFactory::createRoomRequestHandler<RoomAdminRequestHandler>(_user.getUsername(), newRoom));
}

RequestResult MenuRequestHandler::handleGetPlayers(const RequestInfo& requestInfo)
{
    GetPlayersInRoomResponse response;
    auto req = JsonRequestPacketDeserializer::deserializeGetPlayersRequest(requestInfo.buffer);
    try
    {
        response.players = RoomManager::getInstance().getRoom(req.roomId).getAllUsers();
        response.status = SUCCESS_STATUS;
        return createResult(response, this);
    }
    catch (const std::exception& e)
    {
        return generateErrorResponse("Failed to get the room players");

    }

}

RequestResult MenuRequestHandler::handleJoinRoom(const RequestInfo& requestInfo)
{
    JoinRoomResponse response;
    auto req = JsonRequestPacketDeserializer::deserializeJoinRoomRequest(requestInfo.buffer);

    if (RoomManager::getInstance().joinUserToRoom(_user, req.roomId))
    {
        auto& room = RoomManager::getInstance().getRoom(req.roomId);
        response.status = SUCCESS_STATUS;
        return createResult(response, RequestHandlerFactory::createRoomRequestHandler<RoomMemberRequestHandler>(_user.getUsername(), room));

    }
    else
    {
        return generateErrorResponse("Room is full.");
    }

}

RequestResult MenuRequestHandler::handleLogout()
{
    LogoutResponse response;
    if (LoginManager::getInstance().logout(_user.getUsername()))
    {
        response.status = SUCCESS_STATUS;
        return createResult(response, RequestHandlerFactory::createLoginRequestHandler());
    }
    else
    {
        return generateErrorResponse("Failed to logout.");
  
    }
}

RequestResult MenuRequestHandler::handleGetPersonalStats()
{
    GetPersonalStatsResponse response;
    response.statistics = StatisticsManager::getInstance().getUserStatistics(_user.getUsername());
    response.status = SUCCESS_STATUS;

    return createResult(response, this);
}



