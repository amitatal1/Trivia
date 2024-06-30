#pragma once
#include "Serialization.h"
#include "LoggedUser.h"
#include "Structs.h"


class IRequestHandler;

struct RequestResult
{
	Buffer response;
	IRequestHandler* newHandler;
};


class IRequestHandler
{
public:

	IRequestHandler();
	IRequestHandler(const string& username);
	virtual ~IRequestHandler();
	virtual bool isRequestRelevant(const RequestInfo& requestInfo) const = 0;
	virtual RequestResult handleNewRequest(RequestInfo& requestInfo) = 0;

	const LoggedUser& getUser() const;

	RequestResult generateErrorResponse(const string error);

	template<typename T>
	static RequestResult createResult(const T& response, IRequestHandler* newHandler)
	{
		RequestResult result;
		result.response = JsonResponsePacketSerializer::serializeResponse(response);
		result.newHandler = newHandler;
		return result;
	}

protected:
	LoggedUser _user;
	
};
