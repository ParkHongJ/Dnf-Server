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
    WSACleanup();
}

HANDLE Listener::GetHandle()
{
	return (HANDLE)listenSocket;
}

bool Listener::CreateListenSocket(NetAddress netAddress)
{
    listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listenSocket == INVALID_SOCKET)
    {
        cout << "Listen Socket Error" << endl;
        return false;
    }

    if (SOCKET_ERROR == ::bind(listenSocket, (SOCKADDR*)&netAddress.GetSockAddr(), sizeof(SOCKADDR_IN)))
    {
        cout << "Bind Error" << endl;
        return false;
    }
    
    listen(listenSocket, SOMAXCONN);

    return true;
}
