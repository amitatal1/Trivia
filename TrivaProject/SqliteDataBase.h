#pragma once

#include "sqlite3.h"
#include "IDataBase.h"
#include "Singleton.h"
#include "Structs.h"
#include <string>
#include <vector>
#include "Question.h"



class SqliteDataBase : public IDataBase, public Singleton<SqliteDataBase> 
{
public:
    friend class Singleton<SqliteDataBase>;

    // Open the database
    virtual  bool open() override;
    void reloadQuestionsTable();

    // Close the database
    virtual bool close() override;

    // Check if the user exists
    virtual bool doesUserExist(const std::string& userName) override;

    // Check if the password matches
    virtual  bool doesPasswordMatch(const std::string& userName, const std::string& password) override;

    // Add a new user
    virtual void addNewUser(const std::string& userName, const std::string& password, const std::string& email) override;

    
    virtual float getPlayerAvarageAnswerTime(const std::string userName) override;
    virtual int getNumberOfCorrectAnswers(const std::string userName) override ;
    virtual int getNumberOfTotalAnswers(const std::string userName) override ;
    virtual int getNumOfPlayerGames(const std::string userName) override;

    //statistics manager functions
    std::vector<std::string> topFiveScores() ;
    std::vector<std::string> playerStats(std::string userName) ;

    std::vector<Question> getQuestions(int questionsNumber) const;
    

    //games stats insert functions 
    void submitGameStats(std::string username, GameData gameData);
    static int calculateScore(int correctAnswers, int wrongAnswers, double time);

    void updateUsersLogs(std::string username);
    int fetchUserLogs(std::string username);

    // Destructor
    virtual ~SqliteDataBase();

protected:
    // Constructor
    SqliteDataBase();

private:
    // Execute a command on the database
    void exec_cmd(const std::string& cmd, void* data, int (*callback)(void*, int, char**, char**));

    // Create the users table
    void createUsersTable();

    void createStatisticsTable();

    std::string HTPPSRequest(std::string url);


    sqlite3* _db; // SQLite database handle
};

