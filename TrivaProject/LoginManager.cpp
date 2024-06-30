#include "LoginManager.h"
#include <iostream>
#include <algorithm>
#include "SqlException.h"
#include "RoomManager.h"


LoginManager::LoginManager()
	:_db(&(SqliteDataBase::getInstance()))
{
}

LoginManager::~LoginManager()
{
}

bool LoginManager::SignUp(const string userName, const string password, const string email)
{
	try
	{
		_db->addNewUser(userName, password, email);
		_loggedUsers.push_back(LoggedUser(userName));
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
		return false;
	}

	return true;

}

bool LoginManager::login(const string userName, const string password)
{
	try
	{
		if (_db->doesUserExist(userName) && _db->doesPasswordMatch(userName,password) )
		{
			LoggedUser newUser(userName);
			if (std::find(_loggedUsers.begin(), _loggedUsers.end(), newUser) != _loggedUsers.end()) //prevents user logging in twice
			{
				throw std::runtime_error("User Already logged in! ");
			}
			else
			{
				_loggedUsers.push_back(newUser);
				SqliteDataBase::getInstance().updateUsersLogs(newUser.getUsername());
				return true;
			}
			
			
	
		}
		else
		{
			return false;
		}
	}

	catch (const SQLException& e)
	{
		string error=( e.what());
		std::cout << error;
		return false;
	}

	return true;

}

bool LoginManager::logout(const string userName)
{
	auto it = std::remove_if(_loggedUsers.begin(), _loggedUsers.end(),
		[&](const LoggedUser& user) { return user.getUsername() == userName; });

	if (it != _loggedUsers.end())
	{
		RoomManager::getInstance().logoutUserFromRooms(userName); //diconnect from rooms, if joined
		_loggedUsers.erase(it, _loggedUsers.end());

		return true;  // Successfully found and removed the user
	}
	
	return false;  // User was not found
}


