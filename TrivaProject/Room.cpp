#include "Room.h"


Room::Room(RoomData& data, LoggedUser& creator)
: _metaData(data)
    
{
    _users.push_back(creator);
}

Room::~Room() {}

const RoomData& Room::getRoomData()  const
{
    return _metaData;
}

bool Room::addUser(const LoggedUser& user)
{
    if (_users.size() < getRoomData().maxPlayers)
    {
        _users.push_back(user);
        return true;
    }
    else
    {
        return false;
    }
}

void Room::removeUser(const LoggedUser& user)
{   
    _users.erase(std::remove_if(_users.begin(), _users.end(),
        [&](const LoggedUser& connectedUser) { return user.getUsername() == connectedUser.getUsername(); }), _users.end());

}

std::vector<string> Room::getAllUsers() const 
{
    std::vector<string> usernames;
    for (auto& user : _users)
    {
        usernames.push_back(user.getUsername());
    }
    return usernames;
}


void Room::activateRoom(const bool mode)
{
    _metaData.isActive = mode;
}

