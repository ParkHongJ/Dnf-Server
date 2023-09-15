#include "Room.h"
#include "Player.h"
#include "Session.h"
#include "Projectile.h"
#include "CollisionThread.h"
#include "Player.h"
#include "Monster.h"
#include "Skill.h"
#include "DnfServer.h"
Room GRoom;

Room::Room()
{
	for (size_t i = 0; i < 2; i++)
	{
		Monster* monster = new Monster();
		monster->ObjectId = DnfServer::idGenerator++;
		monster->type = MONSTER;
		monster->x = -0.5f + i * 0.2f;
		monster->y = 0.0f + -1.f * (i % 2) * 0.3f;
		monster->z = 0.0f + -1.f * (i % 2) * 0.3f;
		monster->Initialzie();
		Enter(monster);
	}
}

Room::~Room()
{
	for (int i = ColliderGroup::COL_PLAYER; i < ColliderGroup::MAX; i++)
	{
		lock_guard<mutex> lockguard(lock);
		/*for (auto& collider :colliderObjects[i])
		{
			delete collider;
		}*/
	}
}

void Room::Enter(Object* object)
{
	if (object == nullptr)
		return;

	lock_guard<mutex> lockguard(lock);
	if (object->type == PLAYER)
	{
		_players[object->ObjectId] = object;
	}
	/*else if (object->type == SKILL)
	{
		_projectiles[object->ObjectId] = dynamic_cast<Projectile*>(object);
	}*/
	else if (object->type == MONSTER)
	{
		_monsters[object->ObjectId] = object;
	}
	
}

void Room::Leave(Object* object)
{
	if (object == nullptr)
		return;

	lock_guard<mutex> lockguard(lock);
	if (object->type == PLAYER)
	{
		_players.erase(object->ObjectId);
	}
	/*else if (object->type == SKILL)
	{
		_projectiles.erase(object->ObjectId);
	}*/
	else if (object->type == MONSTER)
	{
		_monsters.erase(object->ObjectId);
	}
}

void Room::Broadcast(BYTE* sendBuffer)
{
	lock_guard<mutex> lockguard(sendLock);
	for (auto& player : _players)
	{
		player.second->ownerSession->Send(sendBuffer);
	}
}

void Room::BroadcastWithOutMe(int objectId, BYTE* sendBuffer)
{
	//�ڽ��� ������ ��ε�ĳ��Ʈ
	lock_guard<mutex> lockguard(sendLock);
	for (auto& player : _players)
	{
		if (player.first != objectId)
		{
			player.second->ownerSession->Send(sendBuffer);
		}
	}
}

void Room::SendRoomInfo(int objectId, BYTE* sendBuffer, int bufferOffset)
{
	lock_guard<mutex> lockguard(sendLock);
	unsigned int PlayerCount = _players.size() - 1;

	memcpy(sendBuffer + bufferOffset, &PlayerCount, sizeof(PlayerCount));

	bufferOffset += sizeof(PlayerCount);

	for (auto& player : _players)
	{
		if (player.second->ObjectId != objectId)
		{
			int id = player.second->ObjectId;

			memcpy(sendBuffer + bufferOffset, &id, sizeof(id));
			bufferOffset += sizeof(id);
		}
	}

	unsigned int MonsterCount = _monsters.size();

	memcpy(sendBuffer + bufferOffset, &MonsterCount, sizeof(MonsterCount));

	bufferOffset += sizeof(MonsterCount);

	for (auto& monster : _monsters)
	{
		int id = monster.second->ObjectId;

		memcpy(sendBuffer + bufferOffset, &id, sizeof(id));
		bufferOffset += sizeof(id);

		float vPos[3];
		vPos[0] = monster.second->x;
		vPos[1] = monster.second->y;
		vPos[2] = monster.second->z;

		memcpy(sendBuffer + bufferOffset, &vPos, sizeof(float) * 3);
		bufferOffset += sizeof(float) * 3;
	}
	_players[objectId]->ownerSession->Send(sendBuffer);
}

void Room::Update()
{
	lock_guard<mutex> lockguard(lock);
	for (auto monster : _monsters)
	{
		monster.second->Update();
	}
}

void Room::CollisionToMonster(Collider* collider, vector<Object*>& collisionId)
{
	for (auto monster : _monsters)
	{
		//��ų�� ������ �÷��̾�� ����
		if (monster.second == collider->Owner)
			continue;

		Collider* Sour = monster.second->collider;
		if (Sour == nullptr || collider == nullptr)
			continue;

		Sour->UpdateMinMax();

		if (IsCollisionAABB(*Sour, *collider))
		{
			collisionId.push_back(monster.second);
		}
	}
}

void Room::CollisionToPlayer(Projectile* projectile, OUT vector<Object*>& collisionId)
{
	for (auto player : _players)
	{
		//��ų�� ������ �÷��̾�� ����
		if (player.second == projectile->Owner)
			continue;

		Collider* Sour = player.second->collider;
		Collider* Dest = projectile->collider;
		if (Sour == nullptr || Dest == nullptr)
			continue;

		Sour->UpdateMinMax();
		Dest->UpdateMinMax();

		if (IsCollisionAABB(*Sour, *Dest))
		{
			cout << player.first << "�� �÷��̾�� " << projectile->ObjectId << "�� ��ų �浹" << endl;
			collisionId.push_back(player.second);
		}
	}
}

Object* Room::FindPlayerById(int id)
{
	lock_guard<mutex> lockguard(lock);

	map<uint64, Object*>::iterator iter;

	iter = _players.find(id);
	if (iter != _players.end())
	{
		return iter->second;
	}

	return nullptr;
}

Object* Room::FindSkillById(int id)
{
	lock_guard<mutex> lockguard(lock);

	map<uint64, Projectile*>::iterator iter;

	/*iter = _projectiles.find(id);
	if (iter != _projectiles.end())
	{
		return iter->second;
	}*/
	return nullptr;
}

Object* Room::FindMonsterById(int id)
{
	lock_guard<mutex> lockguard(lock);

	map<uint64, Object*>::iterator iter;

	iter = _monsters.find(id);
	if (iter != _monsters.end())
	{
		return iter->second;
	}
	return nullptr;
}

void Room::HandleSkill(Player* player, BYTE* packet)
{
	//���� ��ų�� �����
	//��ų�� ��� �����
	//� ��ų�� �����

	int SkillId;
	float vPos[3];
	{
		int bufferOffset = sizeof(PacketHeader);

		int id;

		memcpy(&id, packet + bufferOffset, sizeof(int));

		bufferOffset += sizeof(int);

		memcpy(&vPos, packet + bufferOffset, sizeof(float) * 3);

		bufferOffset += sizeof(float) * 3;

		memcpy(&SkillId, packet + bufferOffset, sizeof(SkillId));

		bufferOffset += sizeof(SkillId);
	}
	{
		BYTE sendBuffer[PACKET_SIZE] = "";

		//���� ��ų�� �����
		//� ��ų�� �����
		//��� �����
		int bufferOffset = sizeof(PacketHeader);

		PacketHeader header;
		header.protocol = PKT_S_SKILL;
		header.size = PACKET_SIZE;

		*(PacketHeader*)sendBuffer = header;

		memcpy(sendBuffer + bufferOffset, &player->ObjectId, sizeof(player->ObjectId));

		bufferOffset += sizeof(player->ObjectId);

		memcpy(sendBuffer + bufferOffset, &SkillId, sizeof(SkillId));

		bufferOffset += sizeof(SkillId);

		memcpy(sendBuffer + bufferOffset, &vPos, sizeof(float) * 3);

		bufferOffset += sizeof(float) * 3;

		cout << player->ObjectId << "�� �÷��̾ " << SkillId << "�� ��ų ���" << endl;
		//��ų �����Ŷ
		Broadcast(sendBuffer);
	}
	{
		//������ ����
		ESkill skill = (ESkill)SkillId;
		Collider collider;
		collider.Owner = player;
		collider.Center[0] = 0.f;
		collider.Center[1] = 0.f;
		collider.Center[2] = 0.f;

		collider.Size[0] = 0.2f;
		collider.Size[1] = 0.2f;
		collider.Size[2] = 0.05f;
		
		collider.UpdateMinMax(vPos);

		if (skill != ID_SKILL_F && skill != ID_SKILL_G)
		{
			vector<Object*> collisionObjects;
			CollisionToMonster(&collider, collisionObjects);

			//�浹�Ѱ� �ִٸ�
			if (!collisionObjects.empty())
			{
				//�浹!!
				cout << "�浹��!!" << endl;
				for (auto monster : collisionObjects)
				{
					monster->OnDamaged(player, 10.f);
				}
			}
		}		
	}
}
