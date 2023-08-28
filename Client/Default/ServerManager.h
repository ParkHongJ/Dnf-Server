#pragma once
#include <queue>
#include <WinSock2.h>
#include <mutex>
#include <map>
#include "Base.h"
#include "Client_Defines.h"

using namespace std;
BEGIN(Engine)
class CGameObject;
END

class PacketHandler;

BEGIN(Client)
class ServerManager
{
public:
	static ServerManager* GetInstance(){
		if (m_pInstance == nullptr)
		{
			m_pInstance = new ServerManager();
		}
		return m_pInstance;
	}

	void AddPacket(BYTE* Packet);

	void Destroy() 
	{
		while (!PacketQueue.empty())
		{
			BYTE* packet = PacketQueue.front();
			PacketQueue.pop();
			delete[] packet;
		}
		delete m_pInstance; 
	}
	HANDLE ProcessPacket();

	void Initialize(SOCKET socket)
	{
		ClientSocket = socket;
	}

	CGameObject* FindGameObjectById(int id);

	void AddGameObject(int id, CGameObject* Object);

	void Send(BYTE* buffer, INT32 len);
	void SetClientId(int id) { ClientId = id; }
	int GetClientId() { return ClientId; }
	void DestroyObjectById(int id);
private:
	mutex lock;
	queue<BYTE*> PacketQueue;
	SOCKET ClientSocket = INVALID_SOCKET;
	static ServerManager* m_pInstance;

	//서버 매니저는 클론될 모든 오브젝트를 갖고있어야함.
	map<int, CGameObject*> Objects;
	
	int ClientId;
};

END