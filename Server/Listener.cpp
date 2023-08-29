#include "Listener.h"

bool Listener::StartAccept(NetAddress netAddress)
{
    CreateListenSocket(netAddress);
	return true;
}

void Listener::CloseSocket()
{
    if (listenSocket != INVALID_SOCKET)
    {
        closesocket(listenSocket);
    }
}

HANDLE Listener::GetHandle()
{
	return (HANDLE)listenSocket;
}

bool Listener::CreateListenSocket(NetAddress netAddress)
{
    listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    bind(listenSocket, (SOCKADDR*)&netAddress.GetSockAddr(), sizeof(SOCKADDR_IN));
    
    listen(listenSocket, SOMAXCONN);

    return true;
}
