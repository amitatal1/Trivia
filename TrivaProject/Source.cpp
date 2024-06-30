#pragma comment (lib, "ws2_32.lib")
#include "WSAInitializer.h"
#include "Server.h"
#include <iostream>
#include <exception>
#include <string>
#include "Serialization.h"


using std::string;




int main()
{
	try
	{
		WSAInitializer wsaInit;
		Server& myServer = Server::getInstance();

		myServer.run();
	}
	catch (std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
	}
	return 0;
}

