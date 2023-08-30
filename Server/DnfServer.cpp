#include "DnfServer.h"
#include "Session.h"
#include "Room.h"
atomic<uint64> DnfServer::idGenerator = 0;

DnfServer::DnfServer(NetAddress targetAddress, int32 maxUser)
	: netAddress(targetAddress), maxUserCount(maxUser),
	listener()
{
}

void DnfServer::StartServer()
{
	WSADATA wsaData; 
	WSAStartup(MAKEWORD(2, 2), &wsaData); 

	listener.StartAccept(netAddress);

	thread RoomThread([]() {
		//todo
		while (true)
		{
			GRoom.Update();
		}
		});

	while (true)
	{
		int SockSize = sizeof(SOCKADDR);
		SOCKET hClient = accept((SOCKET)listener.GetHandle(), (SOCKADDR*)&netAddress.GetSockAddr(), &SockSize);

		if (hClient == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			cout << "Accept Error : " << error << endl;
			continue;
		}
		cout << "클라이언트 연결 완료" << endl;

		Session* session = CreateSession(hClient);
		AddSession(session);

		thread WorkerThread = thread([=]()
			{
				while (true)
				{
					if (!session->Recv())
					{
						//세션이 종료되거나 오류가 뜨면 스레드 종료
						ReleaseSession(session);
						break;
					};
				}
			});

		WokerThreads.push_back(move(WorkerThread));
	}

	if (RoomThread.joinable())
		RoomThread.join();

	ReleaseWorkerThread();

	listener.CloseSocket();
}

Session* DnfServer::CreateSession(SOCKET clientSocket)
{
	Session* session = new Session(clientSocket);
	return session;
}

void DnfServer::AddSession(Session* session)
{
	if (session != nullptr)
	{
		lock_guard<mutex> lockguard(lock);
		sessions.insert(session);
	}
}

void DnfServer::ReleaseSession(Session* session)
{
	if (session != nullptr)
	{
		lock_guard<mutex> lockguard(lock);
		sessions.erase(session);
				
		delete session;
		cout << "클라이언트 세션 종료" << endl;
	}
}

void DnfServer::ReleaseWorkerThread()
{
	for (auto& workerThread : WokerThreads)
	{
		if (workerThread.joinable())
			workerThread.join();
	}
	WokerThreads.clear();
}
