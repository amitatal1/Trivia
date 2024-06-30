#include "StatisticsManager.h"

StatisticsManager::StatisticsManager()
{
}

StatisticsManager::~StatisticsManager()
{
}

std::vector<string> StatisticsManager::getHighScore() const
{
	return SqliteDataBase::getInstance().topFiveScores();
}

std::vector<string> StatisticsManager::getUserStatistics(string username)
{
	return SqliteDataBase::getInstance().playerStats(username);
}


