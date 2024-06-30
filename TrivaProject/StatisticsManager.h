#pragma once
#include "SqliteDataBase.h"
#include <vector>

using std::string;

class StatisticsManager
	:public Singleton<StatisticsManager>
{

	friend class Singleton<StatisticsManager>;

public:
	~StatisticsManager();
	std::vector<string> getHighScore() const;
	std::vector<string> getUserStatistics(string username);
		 
protected:
	StatisticsManager();
	

};

