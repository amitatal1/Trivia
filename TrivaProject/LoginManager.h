#pragma once
#include "SqliteDataBase.h"
#include "LoggedUser.h"
#include <vector>

class LoginManager
	: public  Singleton<LoginManager>
{
public:
	friend class Singleton< LoginManager>;
	~LoginManager();


	bool SignUp(const string userName, const string password, const string email);
	bool login(const string userName, const string password);
	bool logout(const string userName);

private:
	IDataBase* _db;
	std::vector<LoggedUser> _loggedUsers;
protected:
	LoginManager();

};

