#pragma once
#include "Server_Defines.h"
class Object;
class Skill;
class Collider;

enum ColliderGroup
{
	COL_PLAYER = 0,
	COL_MONSTER,
	COL_SKILL,
	MAX
};

class Room
{
public:
	~Room();
	void Enter(Object* object);
	void Leave(Object* object);
	void Broadcast(BYTE* sendBuffer);
	void BroadcastWithOutMe(int objectId, BYTE* sendBuffer);
	void SendRoomInfo(int objectId, BYTE* sendBuffer, int bufferOffset);

	void Update();

	void CollisionToPlayer(Skill* skill, vector<Object*>& collisionId);
	void CollisionToMonster(Skill* skill, vector<Object*>& collisionId);
	Object* FindPlayerById(int id);
	Object* FindSkillById(int id);

private:
	mutex lock;
	mutex collisionLock;
	mutex sendLock;
	map<uint64, Object*> _players;
	map<uint64, Skill*> _skills;
	map<uint64, Object*> _monsters;
};

extern Room GRoom;