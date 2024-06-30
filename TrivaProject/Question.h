#pragma once
#include <string>
#include <vector>
#include <algorithm>



#define RIGHT_ANSWER_INDEX 0


using std::string;

class Question
{
public:
	Question(const string question, const string correctAnswer, const std::vector<string> falseAnswers)
		:_question(question)
	{
		_possibleAnswers.push_back(correctAnswer);
		_possibleAnswers.insert(_possibleAnswers.end(), falseAnswers.begin(), falseAnswers.end());
	}

	string getQuestion() const
	{
		return _question;
	}

	std::vector<string> getPossibleAnswers()
	{
		//shuffles questions order.
		return _possibleAnswers;
	}

	string getCorrectAnswer() const
	{
		return _possibleAnswers[RIGHT_ANSWER_INDEX];
	}


private:
	string _question;
	std::vector<string> _possibleAnswers;	
};