#include "stdafx.h"
#include "ServerManager.h"
#include "ServerPacketHandler.h"
#include "GameObject.h"

ServerManager* ServerManager::m_pInstance = nullptr;

void ServerManager::AddPacket(BYTE* Packet)
{
	PacketHeader* header = (PacketHeader*)Packet;
	BYTE* Data = new BYTE[header->size];
	memcpy(Data, Packet, header->size);
	
	{
		lock_guard<mutex> lock_guard(lock);
		PacketQueue.push(Data);
	}
}

HANDLE ServerManager::ProcessPacket()
{
	while (true)
	{
		BYTE* packet = nullptr;
		{
			lock_guard<mutex> lock_guard(lock);
			if (!PacketQueue.empty())
			{
				packet = PacketQueue.front();
				PacketQueue.pop();
			}
			else
			{
				break;
			}
		}

		PacketHeader* header = (PacketHeader*)packet;
		ServerPacketHandler::HandlePacket(ClientSocket, packet, header->size);

		delete[] packet;
	}
	return S_OK;
}

CGameObject* ServerManager::FindGameObjectById(int id)
{
	map<int, CGameObject*>::iterator iter;
	
	iter = Objects.find(id);
	
	if (iter != Objects.end())
	{
		return iter->second;
	}

	return nullptr;
}

void ServerManager::AddGameObject(int id, CGameObject* Object)
{
	if (Object != nullptr)
	{
		Objects.insert(make_pair(id, Object));
	}
}

void ServerManager::Send(BYTE* buffer, INT32 len)
{
	send(ClientSocket, (const char*)buffer, len, 0);
}

void ServerManager::DestroyObjectById(int id)
{
	CGameObject* pObject = FindGameObjectById(id);
	if (pObject != nullptr)
	{
		pObject->Destroy();
		Objects.erase(id);
	}
}
