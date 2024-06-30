#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"

class RequestHandlerFactory;

class LoginRequestHandler :
    public IRequestHandler
{

public:
    LoginRequestHandler(const string& username = "");
    virtual ~LoginRequestHandler();

    virtual bool isRequestRelevant(const RequestInfo& requestInfo) const override;
    virtual RequestResult handleNewRequest(RequestInfo& requestInfo) override;

private:
    RequestResult handleLoginRequest(RequestInfo& requestInfo);
    RequestResult handleSignupRequest(RequestInfo& requestInfo);

};


