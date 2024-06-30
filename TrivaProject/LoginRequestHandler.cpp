#include "LoginRequestHandler.h"



bool LoginRequestHandler::isRequestRelevant(const RequestInfo& requestInfo) const
{
    return (requestInfo.requestCode == LOGIN_CODE || requestInfo.requestCode == SIGN_UP_CODE);
}

RequestResult LoginRequestHandler::handleNewRequest(RequestInfo& requestInfo)
{

    RequestResult result;

    if (requestInfo.requestCode == LOGIN_CODE)
    {
        result = handleLoginRequest(requestInfo);
    }
    else if(requestInfo.requestCode == SIGN_UP_CODE)
    {
        result = handleSignupRequest(requestInfo);
    }
    
    return result;

    
}


LoginRequestHandler::LoginRequestHandler(const string& username)
    :IRequestHandler(username)
{
}


LoginRequestHandler::~LoginRequestHandler()
{

}

RequestResult LoginRequestHandler::handleLoginRequest(RequestInfo& requestInfo)
{
    auto& loginManager = LoginManager::getInstance();

    LoginRequest loginReq = JsonRequestPacketDeserializer::deserializeLoginRequest(requestInfo.buffer);
    try
    {
        if (loginManager.login(loginReq.username, loginReq.password))
        {
            //login succeded, returned success response and new handle to menu
            LoginResponse successResponse{ SUCCESS_STATUS };
            _user.setUserName(loginReq.username);
            return createResult(successResponse, RequestHandlerFactory::createMenuRequestHandler(loginReq.username));
        }
        else
        {
            //failed login
            return  generateErrorResponse("Couldn't login.");

        }
    }
    catch (const std::exception& ex)
    {
       return generateErrorResponse("Couldn't login." +std::string(ex.what()));

    }
 
}

RequestResult LoginRequestHandler::handleSignupRequest(RequestInfo& requestInfo)
{
    RequestResult result;
    auto& loginManager = LoginManager::getInstance();

    SignupRequest signupReq = JsonRequestPacketDeserializer::deserializeSignupRequest(requestInfo.buffer);
    if (loginManager.SignUp(signupReq.username, signupReq.password, signupReq.email))
    {
        //Sign up succeded, returned success response and new handle to menu
        SignupResponse successResponse{ SUCCESS_STATUS };
     
        _user.setUserName(signupReq.username);
        return createResult(successResponse, RequestHandlerFactory::createMenuRequestHandler(signupReq.username));

    }
    else
    {
        //failed
       return  generateErrorResponse("The sign up proccess failed.");
     
    }
}

