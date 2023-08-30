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
	//�ٸ� ������Ʈ��� �浹�ߴ°�
	//�÷��̾�� �浹��

	vector<int> collisionId;
	GRoom.CollisionToPlayer(this/*, collisionId*/);
	
	if (!collisionId.empty())
	{
		cout << "�浹��" << endl;
	}
}
