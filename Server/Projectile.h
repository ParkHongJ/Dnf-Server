#pragma once
#include "Object.h"
class Projectile : public Object
{
public:
	Projectile();
	virtual void Update() override;

private:
	long StartMoveTick = 0;
	long currentMoveTick;
	mutex lock;

public:
	//스킬을 사용한 오너의 정보
	Object* Owner;
	
};

