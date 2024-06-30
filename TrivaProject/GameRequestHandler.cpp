#include "GameRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "GameManager.h"


GameRequestHandler::GameRequestHandler(const string& username, Game& game)
    :IRequestHandler(username), _game(game)
{
}

GameRequestHandler::~GameRequestHandler()
{
}

bool GameRequestHandler::isRequestRelevant(const RequestInfo& requestInfo) const
{
    return (requestInfo.requestCode == SUBMIT_ANSWER_CODE
        || requestInfo.requestCode == GET_GAME_RESULTS_CODE
        || requestInfo.requestCode == LEAVE_GAME_CODE
        || requestInfo.requestCode == GET_QUESTION_CODE);
}

RequestResult GameRequestHandler::handleNewRequest(RequestInfo& requestInfo)
{
	switch (requestInfo.requestCode)
	{
	case SUBMIT_ANSWER_CODE:
		return submitAnswer(requestInfo);
	case GET_GAME_RESULTS_CODE:
		return getGameResults(requestInfo);
	case LEAVE_GAME_CODE:
		return leaveGame(requestInfo);
	case GET_QUESTION_CODE:
		return getQuestion(requestInfo);
	default:
		return generateErrorResponse("Request Didn't match");
	}
}

RequestResult GameRequestHandler::getQuestion(const RequestInfo& requestInfo)
{
	try
	{
		GetQuestionResponse resp;
		Question& question = _game.getQuestionForUser(_user.getUsername());

		resp.question = question.getQuestion();
		auto answers = question.getPossibleAnswers();

		for (int i = 0; i < answers.size(); i++)
		{
			resp.answers[i] = answers[i];
		}

		resp.status = { SUCCESS_STATUS };
		return createResult(resp, this);

	}
	catch (const std::exception& ex)
	{
		return generateErrorResponse("Failed to retrieve question" + std::string( ex.what()));
	}
	
	
}

RequestResult GameRequestHandler::submitAnswer(const RequestInfo& requestInfo)
{
	auto req = JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(requestInfo.buffer);

	_game.submitAnswer(_user.getUsername(), req.answerId, req.time);

	SubmitAnswerResponse resp;
	resp.status = SUCCESS_STATUS;
	resp.correctAnswerId = RIGHT_ANSWER_INDEX;
	return createResult(resp, this);
}

RequestResult GameRequestHandler::getGameResults(const RequestInfo& requestInfo)
{
	try
	{
		_game.submitGameStatsToDB(_user.getUsername());
		GetGameResultsResponse resp;
		resp.status = { SUCCESS_STATUS };
		resp.results = _game.getGameResults();
		return createResult(resp, this);
	}
	catch (const std::exception&)
	{
		return generateErrorResponse("failed to get results");

	}
}

RequestResult GameRequestHandler::leaveGame(const RequestInfo& requestInfo)
{
	try
	{
		LeaveGameResponse resp;
		_game.removePlayer(_user.getUsername());
		resp.status = { SUCCESS_STATUS };

		//CHECKS IF game still with players 
		if (_game.isEmpty()) GameManager::getInstance().deleteGame(_game.getGameId());


		return createResult(resp, RequestHandlerFactory::createMenuRequestHandler(_user.getUsername()));
	}
	catch (const std::exception&)
	{
		return generateErrorResponse("failed to leave");
	}
	
}
