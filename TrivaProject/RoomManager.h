#pragma once
#include "Room.h"
#include "map"
#include "Singleton.h"

class RoomManager
	: public  Singleton<RoomManager>
{
friend class Singleton<RoomManager>;
private:
	std::map<int, Room> _rooms;
	int _lastID;

public:
	friend class Singleton<RoomManager>;
	~RoomManager();

	Room& createRoom(LoggedUser creator , RoomData data);
	void deleteRoom(const int ID);

	bool joinUserToRoom(LoggedUser user, const int ID);
	int getLastRoomId() const; 

	unsigned int getRoomState(const int ID);
	std::vector<RoomData> getRooms() const;
	Room& getRoom(const int ID);
	void logoutUserFromRooms(const string username);


protected:
	RoomManager();
};

