#pragma once
#include "Server_Defines.h"
class Object;
class Projectile;
class Collider;
class Player;
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
	Room();
	~Room();	
	void Enter(Object* object);
	void Leave(Object* object);
	void Broadcast(BYTE* sendBuffer);
	void BroadcastWithOutMe(int objectId, BYTE* sendBuffer);
	void SendRoomInfo(int objectId, BYTE* sendBuffer, int bufferOffset);

	void Update();

	void CollisionToPlayer(Projectile* skill, vector<Object*>& collisionId);
	void CollisionToMonster(Collider* collider, vector<Object*>& collisionId);


	Object* FindPlayerById(int id);
	Object* FindSkillById(int id);
	Object* FindMonsterById(int id);
	void HandleSkill(Player* player, BYTE* packet);
private:
	mutex lock;
	mutex collisionLock;
	mutex sendLock;
	map<uint64, Object*> _players;
	map<uint64, Projectile*> _projectiles;
	map<uint64, Object*> _monsters;
};

extern Room GRoom;