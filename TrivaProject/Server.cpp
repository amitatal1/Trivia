#include "Server.h"
#include <iostream>
#include <string>
#include "SqliteDataBase.h"

void Server::run()
{
	std::thread communicatorTh(&Communicator::startHandleRequests, &m_communicator);
	communicatorTh.detach();


	std::string command = "";
	while (true)
	{
		try
		{

			const std::string count = "COUNT";


			std::getline(std::cin, command); 
			if (command == "Hello") std::cout << "Hello client!\n";
			else if (command == "EXIT") exit(1);
			
			else if (command.find("COUNT") == 0)
			{
				std::string username = command.substr(command.find("COUNT") + count.size()+1);
				std::cout<<"User has logged "+ std::to_string(SqliteDataBase::getInstance().fetchUserLogs(username)) + " times \n";

			}
		}
		catch (std::exception&)	
		{
		}
		
	}

}



Server::Server()
	:m_communicator(Communicator::getInstance())
{
}
