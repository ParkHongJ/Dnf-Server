#pragma once
#include "Server_Defines.h"
#include "NetAddress.h"
class Listener
{
public:
	/* �ܺο��� ��� */
	
	bool StartAccept(NetAddress netAddress);
	void CloseSocket();

	HANDLE GetHandle();
private:
	bool CreateListenSocket(NetAddress netAddress);

private:
	SOCKET listenSocket;
};

