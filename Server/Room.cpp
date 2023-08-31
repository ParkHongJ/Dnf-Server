#include "Room.h"
#include "Player.h"
#include "Session.h"
#include "Skill.h"
#include "CollisionThread.h"
Room GRoom;

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
	else if (object->type == SKILL)
	{
		_skills[object->ObjectId] = dynamic_cast<Skill*>(object);
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
	else if (object->type == SKILL)
	{
		_skills.erase(object->ObjectId);
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
	//자신을 제외한 브로드캐스트
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
		_players[objectId]->ownerSession->Send(sendBuffer);
	}
}

void Room::Update()
{
	lock_guard<mutex> lockguard(lock);
	for (auto skill : _skills)
	{
		skill.second->Update();
	}
	for (auto monster : _monsters)
	{
		monster.second->Update();
	}
}

void Room::CollisionToPlayer(Skill* skill, OUT vector<Object*>& collisionId)
{
	//데드락..
	lock_guard<mutex> lockguard(collisionLock);
	for (auto player : _players)
	{
		//스킬을 시전한 플레이어는 제외
		if (player.second == skill->Owner)
			continue;

		Collider* Sour = player.second->collider;
		Collider* Dest = skill->collider;
		if (Sour == nullptr || Dest == nullptr)
			continue;

		Sour->UpdateMinMax();
		Dest->UpdateMinMax();

		if (IsCollisionAABB(*Sour, *Dest))
		{
			cout << player.first << "번 플레이어와 " << skill->ObjectId << "번 스킬 충돌" << endl;
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

	map<uint64, Skill*>::iterator iter;

	iter = _skills.find(id);
	if (iter != _skills.end())
	{
		return iter->second;
	}
	return nullptr;
}
