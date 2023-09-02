#include "stdafx.h"
#include "ServerPacketHandler.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "GameObject.h"
#include "ServerManager.h"
#include "Player.h"
#include "Monster.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool ServerPacketHandler::HandlePacket(SOCKET socket, BYTE* buffer, INT32 len)
{
	PacketHeader* header = (PacketHeader*)buffer;

	GPacketHandler[header->protocol](socket, buffer, len);

	return false;
}

bool Handle_INVALID(SOCKET socket, BYTE* buffer, INT32 len)
{
	return false;
}

bool Handle_S_LOGIN(SOCKET socket, BYTE* buffer)
{
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->GetDeviceContext(&pDevice, &pContext);

	if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(pDevice, pContext, LEVEL_GAMEPLAY))))
	{
		return false;
	}

	int id;
	memcpy(&id, buffer + sizeof(PacketHeader), sizeof(id));

	ServerManager* ServerMgr = ServerManager::GetInstance();
	ServerMgr->SetClientId(id);

	return true;
}

bool Handle_S_ENTER_GAME(SOCKET socket, BYTE* buffer)
{
	unsigned int PlayerCount;

	int bufferOffset = sizeof(PacketHeader);

	memcpy(&PlayerCount, buffer + bufferOffset, sizeof(PlayerCount));
	
	bufferOffset += sizeof(PlayerCount);
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	for (unsigned int i = 0; i < PlayerCount; i++)
	{
		int id;
		memcpy(&id, buffer + bufferOffset, sizeof(id));
		bufferOffset += sizeof(id);
		
		CGameObject* pGameObject = pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		
		if (pGameObject != nullptr)
		{
			pGameObject->SetId(id);
			ServerManager* ServerMgr = ServerManager::GetInstance();

			ServerMgr->AddGameObject(id, pGameObject);
		}		
	}

	unsigned int MonsterCount;

	memcpy(&MonsterCount, buffer + bufferOffset, sizeof(MonsterCount));

	bufferOffset += sizeof(MonsterCount);

	for (unsigned int i = 0; i < MonsterCount; i++)
	{
		int id;
		memcpy(&id, buffer + bufferOffset, sizeof(id));
		bufferOffset += sizeof(id);

		CGameObject* pGameObject = pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Monster"), LEVEL_GAMEPLAY, TEXT("Layer_Player"));

		if (pGameObject != nullptr)
		{
			pGameObject->SetId(id);
			ServerManager* ServerMgr = ServerManager::GetInstance();

			ServerMgr->AddGameObject(id, pGameObject);
			CMonster* Monster = (CMonster*)pGameObject;
			Monster->AddColliderPacket();
		}
	}

	BYTE packet[PACKET_SIZE] = "";

	PacketHeader header;

	header.protocol = PKT_C_CREATE_PLAYER;
	header.size = PACKET_SIZE;
	(*(PacketHeader*)packet) = header;

	send(socket, (const char*)packet, PACKET_SIZE, 0);

	return true;
}

bool Handle_S_CREATE_PLAYER(SOCKET socket, BYTE* buffer)
{
	int id;

	int bufferOffset = sizeof(PacketHeader);
	memcpy(&id, buffer + bufferOffset, sizeof(int));

	bufferOffset += sizeof(int);

	bool bIsPlayer;
	memcpy(&bIsPlayer, buffer + bufferOffset, sizeof(bool));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CGameObject* pGameObject = pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	ServerManager* ServerMgr = ServerManager::GetInstance();
	pGameObject->SetId(id);

	ServerMgr->AddGameObject(id, pGameObject);
	
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameObject);
	if (pPlayer && bIsPlayer)
	{
		pPlayer->SetId(ServerMgr->GetClientId());
		pPlayer->SetLocallyControlled(bIsPlayer);
		pPlayer->AddColliderPacket();
	}

	return true;
}

bool Handle_S_MOVE(SOCKET socket, BYTE* buffer)
{
	int id;

	int bufferOffset = sizeof(PacketHeader);
	memcpy(&id, buffer + bufferOffset, sizeof(int));

	bufferOffset += sizeof(int);

	int State;
	memcpy(&State, buffer + bufferOffset, sizeof(State));

	bufferOffset += sizeof(State);

	int Dir;
	memcpy(&Dir, buffer + bufferOffset, sizeof(Dir));

	bufferOffset += sizeof(Dir);

	_float3 Pos;
	memcpy(&Pos.x, buffer + bufferOffset, sizeof(Pos));

	bufferOffset += sizeof(Pos);

	

	ServerManager* ServerMgr = ServerManager::GetInstance();

	CGameObject* pObject = ServerMgr->FindGameObjectById(id);

	if (pObject)
	{
		if (CPlayer* player = dynamic_cast<CPlayer*>(pObject))
		{
			player->SetDestination(XMLoadFloat3(&Pos), State, Dir);
		}
		/*CTransform* pTransform = (CTransform*)pObject->Get_ComponentPtr(TEXT("Com_Transform"));
		
		if (pTransform)
		{
			pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&Pos), 1.f));
		}*/
	}

	return true;
}

bool Handle_S_EXIT(SOCKET socket, BYTE* buffer)
{
	int id;

	int bufferOffset = sizeof(PacketHeader);
	memcpy(&id, buffer + bufferOffset, sizeof(int));

	bufferOffset += sizeof(int);

	ServerManager* ServerMgr = ServerManager::GetInstance();

	ServerMgr->DestroyObjectById(id);
	
	return true;
}

bool Handle_S_SKILL(SOCKET socket, BYTE* buffer)
{
	int Objectid;

	int bufferOffset = sizeof(PacketHeader);
	memcpy(&Objectid, buffer + bufferOffset, sizeof(Objectid));

	bufferOffset += sizeof(Objectid);

	int Skillid;

	memcpy(&Skillid, buffer + bufferOffset, sizeof(Skillid));

	bufferOffset += sizeof(Skillid);

	_float3 vPos;
	memcpy(&vPos, buffer + bufferOffset, sizeof(vPos));

	bufferOffset += sizeof(vPos);

	ServerManager* ServerMgr = ServerManager::GetInstance();

	CGameObject * pObject = ServerMgr->FindGameObjectById(Objectid);

	CPlayer * player = dynamic_cast<CPlayer*>(pObject);
	player->ActivateSkill(Skillid, vPos);

	return true;
}

bool Handle_S_CHANGE_HP(SOCKET socket, BYTE* buffer)
{
	int a = 10;
	//어떤 오브젝트id가
	//hp가 변했다
	int bufferOffset = sizeof(PacketHeader);

	int id;
	memcpy(&id, buffer + bufferOffset, sizeof(id));

	bufferOffset += sizeof(id);

	float hp;
	memcpy(&hp, buffer + bufferOffset, sizeof(hp));

	bufferOffset += sizeof(hp);

	ServerManager* ServerMgr = ServerManager::GetInstance();

	CGameObject* Object = ServerMgr->FindGameObjectById(id);

	if (Object != nullptr)
	{
		Object->OnDamaged(nullptr, hp);
	}

	return true;
}

bool Handle_S_MONSTER_CONTROL(SOCKET socket, BYTE* buffer)
{
	int bufferOffset = sizeof(PacketHeader);

	int id;
	memcpy(&id, buffer + bufferOffset, sizeof(id));

	bufferOffset += sizeof(id);

	_float3 vPos;

	memcpy(&vPos, buffer + bufferOffset, sizeof(vPos));
	bufferOffset += sizeof(vPos);

	int NewState;

	memcpy(&NewState, buffer + bufferOffset, sizeof(NewState));
	bufferOffset += sizeof(NewState);


	ServerManager* ServerMgr = ServerManager::GetInstance();
	CMonster* object = (CMonster*)ServerMgr->FindGameObjectById(id);
	object->UpdateMove(vPos, NewState);

	return true;
}
