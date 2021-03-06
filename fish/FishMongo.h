#ifndef FISHMONGO_H
#define FISHMONGO_H
#include "mongo/MongoSession.h"

class FishMongo : public MongoSession
{
public:
	typedef MongoSession Super;
public:
	FishMongo(Network::EventPoller * poller,int fd);
	~FishMongo(void);

	void LoadRole(int id);

	void LoadRoleReply(int id,const char* data,int size);
};

#endif