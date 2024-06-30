#include "RoomManager.h"
#include <algorithm>

#define ROOM_CREATOR_INDEX 0

RoomManager::~RoomManager()
{
}

Room& RoomManager::createRoom(LoggedUser creator, RoomData data)
{
	data.id = _lastID++;
	_rooms.emplace(data.id, Room(data, creator)); // Insert the room directly into the map
	return _rooms.at(data.id); // Return a reference to the room stored in the map
}

void RoomManager::deleteRoom(int ID)
{
	_rooms.erase(ID);
}

bool RoomManager::joinUserToRoom(LoggedUser user,const int ID)
{
	Room& room = getRoom(ID);
	return room.addUser(user);
}

int RoomManager::getLastRoomId() const
{
	return _lastID;
}

unsigned int RoomManager::getRoomState(int ID)
{
	auto data = _rooms[ID].getRoomData();
	return data.isActive;
}

std::vector<RoomData> RoomManager::getRooms() const
{
	std::vector<RoomData> roomsVec;
	for (auto& pair : _rooms)
	{
		roomsVec.push_back(pair.second.getRoomData()); //change to only active rooms might be needed here 

	}
	return roomsVec;
}

Room& RoomManager::getRoom(const int ID) 
{
	return _rooms.at(ID);
}

void RoomManager::logoutUserFromRooms(const string username)
{
	for(auto& room : _rooms)
	{
		auto roomUsers = room.second.getAllUsers();
		if (roomUsers[ROOM_CREATOR_INDEX]==username) //if user has created the room and he is the admin
		{
			deleteRoom(room.first); // deletes that room
			return;
		}
		else if(std::find(roomUsers.begin(), roomUsers.end(), username) != roomUsers.end())
		{
			room.second.removeUser(username); //deletes the room member from appearing connected
			return;

		}
	}

}

RoomManager::RoomManager()
	:_lastID(1)
{
}


