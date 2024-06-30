#include "Serialization.h"


//JsonRequestPacketDeserializer
//////////////////////////////////////////////////////////////////////////////////////////////////////
LoginRequest JsonRequestPacketDeserializer::deserializeLoginRequest(const Buffer& buffer)
{
	LoginRequest req;
	json data = deserializeRequest(buffer);

	req.username = data["username"];
	req.password = data["password"];


	return req;
}

SignupRequest JsonRequestPacketDeserializer::deserializeSignupRequest(const Buffer& buffer)
{
	SignupRequest req;

	json data = deserializeRequest(buffer);

	req.username = data["username"];
	req.password = data["password"];
	req.email = data["email"];


	return req;
}
JoinRoomRequest JsonRequestPacketDeserializer::deserializeJoinRoomRequest(const Buffer& buffer)
{
	json data = deserializeRequest(buffer);
	JoinRoomRequest req;

	req.roomId = data["roomId"];

	return req;

}
GetPlayersInRoomRequest JsonRequestPacketDeserializer::deserializeGetPlayersRequest(const Buffer& buffer)
{
	json data = deserializeRequest(buffer);
	GetPlayersInRoomRequest req;

	req.roomId = data["roomId"];

	return req;
}
CreateRoomRequest JsonRequestPacketDeserializer::deserializeCreateRoomRequest(const Buffer& buffer)
{
	json data = deserializeRequest(buffer);
	CreateRoomRequest req;

	req.answerTimeout = data["answerTimeout"];
	req.maxUsers = data["maxUsers"];
	req.questionsCount = data["questionsCount"];
	req.roomName = data["roomName"];

	return req;
}
SubmitAnswerRequest JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(const Buffer& buffer)
{
	json data = deserializeRequest(buffer);
	SubmitAnswerRequest req;

	req.answerId = data["answerId"];
	req.time = data["time"];

	return req;
}

json JsonRequestPacketDeserializer::deserializeRequest(const Buffer& buffer)
{

	string jsonStr(buffer.begin(), buffer.end());
	json data = json::parse(jsonStr);

	return data;

}



//JsonResponsePacketSerializer:
/////////////////////////////////////////////////////////////////////////////////////////////////////
Buffer JsonResponsePacketSerializer::serializeResponse(const ErrorResponse& resp)
{
	json data;
	data["message"] = resp.message;

	//send message to serialization
	return serializeResponse(ERROR_CODE, to_string(data));
}

Buffer JsonResponsePacketSerializer::serializeResponse(const LoginResponse& resp)
{
	json data;
	data["status"] = resp.status;

	//send message to serialization
	return serializeResponse(LOGIN_CODE, to_string(data));
}

Buffer JsonResponsePacketSerializer::serializeResponse(const SignupResponse& resp)
{
	json data;
	data["status"] = resp.status;

	//send message to serialization
	return serializeResponse(SIGN_UP_CODE, to_string(data));

}

//version 2 
Buffer JsonResponsePacketSerializer::serializeResponse(const LogoutResponse& resp)
{
	json data;
	data["status"] = resp.status;

	//send message to serialization
	return serializeResponse(LOGOUT_CODE, to_string(data));
}
Buffer JsonResponsePacketSerializer::serializeResponse(const GetRoomsResponse& resp)
{
	json data;
	data["status"] = resp.status;

	//converts the rooms data vector to a json
	json roomsArrayJson;
	for (const RoomData& room : resp.rooms)
	{
		roomsArrayJson.push_back(turnRoomDataToJson(room));
	}

	data["rooms"] = roomsArrayJson;

	//send message to serialization
	return serializeResponse(GET_ROOMS_CODE, to_string(data));
}
Buffer JsonResponsePacketSerializer::serializeResponse(const GetPlayersInRoomResponse& resp)
{
	json data;
	json playersArr;

	for (auto& player : resp.players)
	{
		playersArr.push_back(player);
	}
	data["players"] = playersArr;

	//send message to serialization
	return serializeResponse(GET_PLAYERS_CODE, to_string(data));
}
Buffer JsonResponsePacketSerializer::serializeResponse(const JoinRoomResponse& resp)
{
	json data;
	data["status"] = resp.status;

	//send message to serialization
	return serializeResponse(JOIN_ROOM_CODE, to_string(data));
}
Buffer JsonResponsePacketSerializer::serializeResponse(const CreateRoomResponse& resp)
{
	json data;
	data["status"] = resp.status;

	//send message to serialization
	return serializeResponse(CREATE_ROOM_CODE, to_string(data));
}
Buffer JsonResponsePacketSerializer::serializeResponse(const getHighScoreResponse& resp)
{
	json data;
	data["status"] = resp.status;


	json statisticsArr;

	for (auto& stat : resp.statistics)
	{
		statisticsArr.push_back(stat);
	}

	data["statistics"] = statisticsArr;

	//send message to serialization
	return serializeResponse(GET_HIGH_SCORE_CODE, to_string(data));
}
Buffer JsonResponsePacketSerializer::serializeResponse(const GetPersonalStatsResponse& resp)
{
	json data;
	data["status"] = resp.status;

	json statisticsArr = json::array();

	for (const auto& stat : resp.statistics)
	{
		statisticsArr.push_back(stat);
	}

	data["statistics"] = statisticsArr;

	// Use dump() to convert JSON object to string
	return serializeResponse(GET_HIGH_SCORE_CODE, data.dump());
}

Buffer JsonResponsePacketSerializer::serializeResponse(const CloseRoomResponse& resp)
{
	json data;
	data["status"] = resp.status;
	return serializeResponse(CLOSE_ROOM_CODE, to_string(data));

}

Buffer JsonResponsePacketSerializer::serializeResponse(const StartGameResponse& resp)
{
	json data;
	data["status"] = resp.status;
	return serializeResponse(GET_PERSONAL_STATS_CODE, to_string(data));
}

Buffer JsonResponsePacketSerializer::serializeResponse(const GetRoomStateResponse& resp)
{
	json data;

	data["status"] = resp.status;
	data["hasGameBegun"] = resp.hasGameBegun;
	data["answerTimeout"] = resp.answerTimeout;
	data["questionCount"] = resp.questionCount;

	// Create a JSON array for players
	json players = json::array();
	for (const auto& player : resp.players)
	{
		players.push_back(player);
	}

	data["players"] = players;


	return serializeResponse(GET_ROOM_STATE_CODE, to_string(data));
}

Buffer JsonResponsePacketSerializer::serializeResponse(const LeaveRoomResponse& resp)
{
	json data;
	data["status"] = resp.status;
	return serializeResponse(GET_PERSONAL_STATS_CODE, to_string(data));
}

Buffer JsonResponsePacketSerializer::serializeResponse(const GetGameResultsResponse& resp)
{
	json data;
	data["status"] = resp.status;

	for (const auto& result : resp.results)  //insert results
	{
		data["results"].push_back({ {"username", result.username}, {"correctAnswerCount", result.correctAnswerCount}, {"wrongAnswerCount", result.wrongAnswerCount}, {"averageAnswerTime", result.averageAnswerTime}, {"score", result.score} });
	}

	return serializeResponse(GET_GAME_RESULTS_CODE, to_string(data));
}

Buffer JsonResponsePacketSerializer::serializeResponse(const SubmitAnswerResponse& resp)
{
	json data;
	data["status"] = resp.status;
	data["correctAnswerId"] = resp.correctAnswerId;
	return serializeResponse(SUBMIT_ANSWER_CODE, to_string(data));

}

Buffer JsonResponsePacketSerializer::serializeResponse(const GetQuestionResponse& resp)
{
	json data;
	data["status"] = resp.status;
	data["question"] = resp.question;

	for (const auto& answer : resp.answers) 
	{
		data["answers"][std::to_string(answer.first)] = answer.second;
	}
	return serializeResponse(GET_QUESTION_CODE, to_string(data));

}

Buffer JsonResponsePacketSerializer::serializeResponse(const LeaveGameResponse& resp)
{
	json data;
	data["status"] = resp.status;
	return serializeResponse(LEAVE_ROOM_CODE, to_string(data));

}


Buffer JsonResponsePacketSerializer::serializeResponse(const MessageCode code, string data) {
	Buffer buffer;
	buffer.push_back(code); // First byte - Message code.

	// Copy the length field to the end of the buffer
	int len = data.length();
	buffer.resize(buffer.size() + LENGTH_FIELD_SIZE);
	memcpy(&buffer[buffer.size() - LENGTH_FIELD_SIZE], &len, LENGTH_FIELD_SIZE);

	// Copy the data to the buffer
	buffer.insert(buffer.end(), data.begin(), data.end());


	return buffer;
}

json JsonResponsePacketSerializer::turnRoomDataToJson(const RoomData& roomData)
{
	json roomObject;
	roomObject["id"] = roomData.id;
	roomObject["name"] = roomData.name;
	roomObject["maxPlayers"] = roomData.maxPlayers;
	roomObject["numOfQuestionsInGame"] = roomData.numOfQuestionsInGame;
	roomObject["timePerQuestion"] = roomData.timePerQuestion;
	roomObject["isActive"] = roomData.isActive;

	return roomObject;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////
