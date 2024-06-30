#pragma once
#include "json.hpp"
#include "Structs.h"

using namespace nlohmann;


class JsonRequestPacketDeserializer
{
public:
	//version 1:
	static LoginRequest deserializeLoginRequest(const Buffer& buffer);
	static SignupRequest deserializeSignupRequest(const Buffer& buffer);

	//version 2:
	static  JoinRoomRequest deserializeJoinRoomRequest(const Buffer& buffer);
	static  GetPlayersInRoomRequest deserializeGetPlayersRequest(const Buffer& buffer);
	static  CreateRoomRequest deserializeCreateRoomRequest(const Buffer& buffer);
	

	//version 4:
	static SubmitAnswerRequest deserializeSubmitAnswerRequest(const Buffer& buffer);
private:
	static json deserializeRequest(const Buffer& buffer);
	JsonRequestPacketDeserializer() = delete; //disables instance creating abillity
};


class JsonResponsePacketSerializer
{
public:
	//version 1:
	static Buffer serializeResponse(const ErrorResponse& resp);
	static Buffer serializeResponse(const LoginResponse& resp);
	static Buffer serializeResponse(const SignupResponse& resp);
	

	//version 2:
	static Buffer serializeResponse(const LogoutResponse& resp);
	static Buffer serializeResponse(const GetRoomsResponse& resp);
	static Buffer serializeResponse(const GetPlayersInRoomResponse& resp);
	static Buffer serializeResponse(const JoinRoomResponse& resp);
	static Buffer serializeResponse(const CreateRoomResponse& resp);
	static Buffer serializeResponse(const getHighScoreResponse& resp);
	static Buffer serializeResponse(const GetPersonalStatsResponse& resp);

	//version 3:
	static Buffer serializeResponse(const CloseRoomResponse& resp);
	static Buffer serializeResponse(const StartGameResponse& resp);
	static Buffer serializeResponse(const GetRoomStateResponse& resp);
	static Buffer serializeResponse(const LeaveRoomResponse& resp);

	//version 4:
	static Buffer serializeResponse(const GetGameResultsResponse& resp);
	static Buffer serializeResponse(const SubmitAnswerResponse& resp);
	static Buffer serializeResponse(const GetQuestionResponse& resp);
	static Buffer serializeResponse(const LeaveGameResponse& resp);

private:
	static Buffer serializeResponse(MessageCode code, string data);
	
	static json turnRoomDataToJson(const RoomData& roomData);

	JsonResponsePacketSerializer() = delete;
};


