#ifndef FISHAPP_H
#define FISHAPP_H

#include "ServerApp.h"
#include <vector>
#include <map>
#include <string>

#include "ServerConnector.h"
#include "ServerAcceptor.h"
#include "FishMongo.h"
#include "ClientSession.h"

class FishApp:public ServerApp
{
public:
	FishApp(std::string file);

	~FishApp(void);

	virtual int Init();

	virtual void ConnectMongo(const char* host,int port);

	virtual void ListenClient(const char* host,int port);

	virtual int Fina();

	virtual int Run();

	virtual int HandleTimeout();

private:
	std::string _file;
	ServerConnector<FishMongo> _mongoConnector;
	ServerAcceptor<ClientSession> _clientAcceptor;
};

#endif