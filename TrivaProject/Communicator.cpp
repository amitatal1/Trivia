#include "Communicator.h"
#include <exception>
#include <iostream>
#include <string>
#include <thread>
#include <fstream>

#define PORT 101
#define HEADER_LEN 5



using std::string;

Communicator::Communicator()
	:_lastId(0), _handlerFactory(RequestHandlerFactory::getInstance())
{

	// Creating a TCP socket
	_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");
}

Communicator::~Communicator()
{
	try
	{
		// Closing the server socket in the destructor
		closesocket(_serverSocket);
	}
	catch (...) {}


	//clearing the memory allocated for the clients
	for (auto& iter : _clients)
	{
		delete iter.second;
	}
	_clients.clear();
}


void Communicator::startHandleRequests()
{
	//summoning thread to handle server-client.
	std::thread listeningThread(&Communicator::bindAndListen, this);

	listeningThread.join();
}



void Communicator::bindAndListen()
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(PORT); // Port that server will listen for
	sa.sin_family = AF_INET;   // Must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY; // Listen on any available network interface

	// Binding the socket to the specified configuration (port and address)
	if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	// Start listening for incoming client connections
	if (listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");

	std::cout << "Listening on port " << PORT << std::endl;
	std::cout << "Waiting for client connection request" << std::endl;

	while (true)
	{
		// Accepting new clients in the main loop and handling them in a separate thread
		try
		{
			acceptClient();
		}
		catch (...)
		{
		}
	}
}


void Communicator::handleNewClient(const SOCKET clientSocket)
{
	try
	{
		char header[HEADER_LEN];
		while (true)
		{
			RequestInfo info = recieveRequestFromClient(clientSocket);
			info.id = ++_lastId;

			IRequestHandler* handle = _clients[clientSocket];
			RequestResult requestResult;

			if (handle->isRequestRelevant(info))
			{
				requestResult = handle->handleNewRequest(info);

				//switch to the new handler 
				if (requestResult.newHandler != nullptr && requestResult.newHandler != handle)
				{
					//frees the handler memory. waits few seconds to not interfere some on going processes.
					std::thread deleteThread = std::thread(&Communicator::wait5SecondsAndDeleteHandler ,handle);
					deleteThread.detach();

					_clients[clientSocket] = requestResult.newHandler;
				}
			}
			else //The request type was irelevant.
			{
				requestResult = handle->generateErrorResponse("Request was irelevant");
			}
			sendPacket(clientSocket, requestResult.response);
		}
	}
	catch (const std::exception&)
	{
		string username = _clients[clientSocket]->getUser().getUsername();

		//disconnect user.
		if (!username.empty())
		{
			LoginManager::getInstance().logout(username);
			std::cout << "User has disconnected: " << username;
		}
		else
		{
			std::cout << "Unlogged user has disconnected";
		}


		_clients.erase(clientSocket);
		closesocket(clientSocket);
		return;
	}

}


void Communicator::acceptClient()
{
	// Accepting a client and creating a specific socket from the server to the client
	SOCKET client_socket = accept(_serverSocket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	std::cout << "Client accepted." << std::endl;


	//adding user to the map;
	_clients.insert({ client_socket, _handlerFactory.createLoginRequestHandler() });


	std::thread clientThread(&Communicator::handleNewClient, this, client_socket);

	clientThread.detach();
}

RequestInfo Communicator::recieveRequestFromClient(SOCKET clientSocket) const
{
	RequestInfo info;


	char header[HEADER_LEN];
	recv(clientSocket, header, HEADER_LEN, 0);

	info.requestCode = header[MESSAGE_CODE_INDEX];

	int len = 0;
	memcpy(&len, header + 1, LENGTH_FIELD_SIZE);

	if (len > 0) // if message contains data
	{


		char* data = new char[len + 1];
		recv(clientSocket, data, len, 0);
		data[len] = '\0'; // terminated the data with null for fine printing
		std::cout <<"C: " << data << std::endl;
		Buffer buffer(data, data + len);
		delete[] data;
		info.buffer = buffer;

	}

	return info;



}

void Communicator::wait5SecondsAndDeleteHandler(IRequestHandler* handler)
{
	Sleep(7000);
	delete handler;
}

void Communicator::sendPacket(const SOCKET clientSocket, string message)
{
	if (send(clientSocket, message.c_str(), message.size(), 0) == INVALID_SOCKET)
		throw std::runtime_error("Invalid Socket");

}

void Communicator::sendPacket(const SOCKET clientSocket, Buffer& message)
{
	std::cout<<"S: "+std::string(message.begin(), message.end()) << std::endl;
	const char* charMessage = reinterpret_cast<const char*>(message.data());
	if (send(clientSocket, charMessage, message.size(), 0) == INVALID_SOCKET)
		throw std::runtime_error("Invalid Socket");
}
