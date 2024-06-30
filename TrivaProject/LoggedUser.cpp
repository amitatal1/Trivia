#include "LoggedUser.h"

LoggedUser::LoggedUser(string userName)
    : _username(userName)
{
}

LoggedUser::~LoggedUser()
{
}

string LoggedUser::getUsername() const
{
    return _username;
}


void LoggedUser::setUserName(const string& username)
{
    _username = username;

}

bool LoggedUser::operator==(const LoggedUser& other) const
{
    return other._username == _username;
}
