#include "IRequestHandler.h"

IRequestHandler::IRequestHandler()
	:_user("")
{

}
IRequestHandler::IRequestHandler(const string& username)
	:_user(username)
{

}
IRequestHandler::~IRequestHandler()
{
}

const LoggedUser& IRequestHandler::getUser() const
{
	return _user;
}
RequestResult IRequestHandler::generateErrorResponse(const string error)
{
    ErrorResponse response;
    response.message = error;
    response.status = FAILURE_STATUS;

    return createResult(response, this);
}

