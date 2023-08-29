#include "DnfServer.h"
#include "Session.h"
DnfServer::DnfServer(NetAddress targetAddress, int32 maxUser)
	: netAddress(targetAddress), maxUserCount(maxUser)
{
}

void DnfServer::StartServer()
{
	listener.StartAccept(netAddress);

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
		cout << "클라이언트 연결 완료" << "\n";

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
					/*BYTE packet[PACKET_SIZE] = "";
					int length = recv(session->GetSocket(), (char*)packet, sizeof(packet), 0);
					
					PacketHeader* header = (PacketHeader*)packet;
					if (header->protocol == PKT_C_EXIT)
					{
						ReleaseSession(session);
						break;
					}
					ClientPacketHandler::HandlePacket(session->GetSocket(), (BYTE*)packet, PACKET_SIZE);*/
				}
			});

		WokerThreads.push_back(move(WorkerThread));
	}

	for (auto& workerThread : WokerThreads)
	{
		if (workerThread.joinable())
			workerThread.join();
	}
	WokerThreads.clear();

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
	}
}
