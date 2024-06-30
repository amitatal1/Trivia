#pragma once

#include "Communicator.h"
#include "Singleton.h"

//singleton

class Server
	:public Singleton<Server>
{
	friend class Singleton<Server>;
private:
	Communicator& m_communicator;

public:
	void run();
	~Server() =default;

protected:
	Server();


};

