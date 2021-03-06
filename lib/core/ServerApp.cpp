#include "ServerApp.h"
#include "lualib/LuaAcceptor.h"
#include "lualib/LuaConnector.h"
#include "lualib/LuaSocket.h"
#include "lualib/LuaServer.h"
#include "lualib/LuaTimer.h"
#include "lualib/LuaMongo.h"
#include "lualib/LuaZeropack.h"
#include "lualib/LuaMessageHelper.h"
#include "lualib/LuaProf.h"
#include "util/format.h"
#include "time/Timestamp.h"

#include "Logger.h"

ServerApp::ServerApp()
{
	_poller = Network::EventPoller::Create();
	_poller->Timer().Register(this,100);
	_LuaManager = new LuaFish();
	_mongo = NULL;
	_now = ::Now();
	_state = AppRun;
}

ServerApp::~ServerApp()
{
	delete _LuaManager;
	delete _poller;
}

int ServerApp::Init()
{
	_LuaManager->Init(this);

#if defined (WIN32)
	_LuaManager->LuaPath("..\\script\\?.lua;");	
	_LuaManager->LuaPath("..\\lib\\3rd\\pbc\\binding\\lua\\?.lua;");
	_LuaManager->LuaPath("..\\lib\\3rd\\luaprofiler\\analyzer\\?.lua;");
#else
	_LuaManager->LuaPath("../script/?.lua;");
	_LuaManager->LuaPath("../lib/3rd/pbc/binding/lua/?.lua;");
	_LuaManager->LuaPath("../lib/3rd/luaprofiler/analyzer/?.lua;");
#endif
	
	_LuaManager->Require("Core",LuaFish::Register);
	_LuaManager->Require("Acceptor",LuaAcceptor::Register);
	_LuaManager->Require("Connector",LuaConnector::Register);
	_LuaManager->Require("SocketCore",LuaSocket::Register);
	_LuaManager->Require("Server",LuaServer::Register);
	_LuaManager->Require("Timer",LuaTimer::Register);
	_LuaManager->Require("MongoCore",LuaMongo::Register);
	_LuaManager->Require("Zeropack",LuaZeropack::Register);
	_LuaManager->Require("MessageHelper",LuaMessageHelper::Register);
	_LuaManager->Require("LuaProf",LuaProf::luaopen_prof);
	return 0;
}

int ServerApp::Fina()
{
	return 0;
}

void ServerApp::Stop()
{
	_state = AppStop;
}

int ServerApp::Run()
{
	while (_state == AppRun)
	{
		this->_poller->Process();
		this->MainTick();
	}
	return 0; 
}

int ServerApp::MainTick()
{
	return this->LuaManager()->MainTick();
}

int ServerApp::HandleTimeout()
{
	_now = ::Now();
	return 0;
}

void ServerApp::SessionEnter(int source,Network::Session* session)
{
	_sessionMap[source] = session;
}

void ServerApp::SessionLeave(int source)
{
	_sessionMap.erase(source);
}

Network::Session* ServerApp::FindSession(int source)
{
	return _sessionMap[source];
}

LuaFish* ServerApp::LuaManager()
{
	return _LuaManager;
}

Network::EventPoller* ServerApp::Poller()
{
	return _poller;
}

uint64 ServerApp::Now()
{
	return _now;
}

void ServerApp::Mongo(Network::Session* session)
{
	_mongo = session;
}

Network::Session* ServerApp::Mongo()
{
	return _mongo;
}


