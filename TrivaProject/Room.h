#pragma once
#include "LoggedUser.h"
#include <vector>
#include "Structs.h"

class Room
{
private:
	RoomData _metaData;
	std::vector<LoggedUser> _users;

public:
	Room() = default;
	Room(RoomData& data, LoggedUser& creator);
	~Room();

	const RoomData& getRoomData() const ;
	bool addUser(const LoggedUser& user);
	void removeUser(const LoggedUser& user);
	std::vector<string> getAllUsers() const;
	void activateRoom(const bool mode);
};

