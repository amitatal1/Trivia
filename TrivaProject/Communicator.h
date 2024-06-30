#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>
#include "Singleton.h"
#include "RequestHandlerFactory.h"


//singleton implentation
class Communicator
	:public Singleton<Communicator>
{
public:

	friend class Singleton<Communicator>;

	~Communicator();

	void startHandleRequests();

	


protected:
	Communicator();


private:
	//class members
	SOCKET _serverSocket;
	std::map<SOCKET, IRequestHandler*> _clients;

	//counters
	int _lastId;

	//handlers 
	RequestHandlerFactory& _handlerFactory;

	void bindAndListen();
	void handleNewClient(const SOCKET clientSocket);
	void acceptClient();

	//
	RequestInfo recieveRequestFromClient(SOCKET clientSocket) const;

	static void wait5SecondsAndDeleteHandler(IRequestHandler* handler);

	//Helpers
	void sendPacket(const SOCKET clientSocket, string message);
	void sendPacket(const SOCKET clientSocket, Buffer& message);

};

