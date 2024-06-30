#pragma once
#include "IRequestHandler.h"
#include "GameManager.h"


class GameRequestHandler :
    public IRequestHandler
{

public:
    GameRequestHandler(const string& username,  Game& game);
    virtual ~GameRequestHandler();

    virtual bool isRequestRelevant(const RequestInfo& requestInfo) const override;
    virtual RequestResult handleNewRequest(RequestInfo& requestInfo) override;


private:
    RequestResult getQuestion(const RequestInfo& requestInfo);
    RequestResult submitAnswer(const RequestInfo& requestInfo);
    RequestResult getGameResults(const RequestInfo& requestInfo);
    RequestResult leaveGame(const RequestInfo& requestInfo);
    Game& _game;
};

