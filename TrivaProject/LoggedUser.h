#pragma once
#include <string>

using std::string;

class LoggedUser
{
public:
	LoggedUser(string userName);
	~LoggedUser();

	string getUsername() const;
	void setUserName(const string& username);

	bool operator==(const LoggedUser& other) const;

private:
	string _username;
};

