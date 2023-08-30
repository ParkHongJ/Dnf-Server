#include "Skill.h"
#include "Room.h"
#include "CollisionThread.h"
Skill::Skill()
{
	StartMoveTick = GetTickCount64();
}
void Skill::Update()
{
	currentMoveTick = GetTickCount64();
	if (currentMoveTick - StartMoveTick < 1000)
		return;

	StartMoveTick = currentMoveTick;

	cout << "Skill Update!!" << endl;
	//다른 오브젝트들과 충돌했는가
	//플레이어와 충돌함

	vector<int> collisionId;
	GRoom.CollisionToPlayer(this/*, collisionId*/);
	
	if (!collisionId.empty())
	{
		cout << "충돌함" << endl;
	}
}
