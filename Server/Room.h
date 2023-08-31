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
	void AddCollision(Collider* collider, int Group);

	void CollisionToPlayer(Skill* skill, vector<int>& collisionId);
	Object* FindPlayerById(int id);
	Object* FindSkillById(int id);
	//list<Collider*>* GetColliderObjects(int Group);
private:
	mutex lock;
	mutex lock2;
	map<uint64, Object*> _players;
	map<uint64, Skill*> _skills;
};

extern Room GRoom;