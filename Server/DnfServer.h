#pragma once
#include "NetAddress.h"
#include "Listener.h"
class Session;
class DnfServer
{
public:
	DnfServer(NetAddress targetAddress, int32 maxUser = 1);

	void StartServer();

	Session* CreateSession(SOCKET clientSocket);
	void AddSession(Session* session);
	void ReleaseSession(Session* session);

	void ReleaseWorkerThread();

public:
	static atomic<uint64> idGenerator;

private:
	mutex lock;

	Listener listener;
	NetAddress netAddress = {};
	int32 maxUserCount;

	vector<thread> WokerThreads;

	set<Session*> sessions;
};

