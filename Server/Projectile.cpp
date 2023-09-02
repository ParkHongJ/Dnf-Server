#include "Projectile.h"
#include "Room.h"
#include "CollisionThread.h"
Projectile::Projectile()
{
	StartMoveTick = GetTickCount64();
}
void Projectile::Update()
{
	currentMoveTick = GetTickCount64();
	if (currentMoveTick - StartMoveTick < 1000)
		return;

	StartMoveTick = currentMoveTick;

	vector<Object*> collisionObject;
	GRoom.CollisionToPlayer(this, collisionObject);
	
	if (!collisionObject.empty())
	{
		for (auto object : collisionObject)
		{
			if (object != nullptr)
			{
				object->OnDamaged(this, 10.f);
			}			
		}
		cout << "Ãæµ¹ÇÔ" << endl;
	}

	//GRoom.Leave(this);
}
