#pragma once
#include "Server_Defines.h"
#include "NetAddress.h"
class Listener
{
public:
	/* 외부에서 사용 */
	
	bool StartAccept(NetAddress netAddress);
	void CloseSocket();

	HANDLE GetHandle();
private:
	bool CreateListenSocket(NetAddress netAddress);

private:
	SOCKET listenSocket;
};

