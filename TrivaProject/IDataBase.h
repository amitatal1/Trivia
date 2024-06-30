#pragma once
#include <string>
using std::string;


class IDataBase
{
public:

	virtual ~IDataBase() {}
	virtual bool open() = 0;
	virtual bool close() =0;
	virtual bool doesUserExist(const string& userName) = 0;
	virtual bool doesPasswordMatch(const string& userName, const string& password) =0;
	virtual void addNewUser(const string& userName, const string& password, const string& email) = 0;

	virtual float getPlayerAvarageAnswerTime(const std::string userName) = 0;
	virtual int getNumberOfCorrectAnswers(const std::string userName) = 0;
	virtual int getNumberOfTotalAnswers(const std::string userName) = 0;
	virtual int getNumOfPlayerGames(const std::string userName) = 0;

};