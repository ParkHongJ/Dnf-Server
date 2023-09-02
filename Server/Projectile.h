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
	//��ų�� ����� ������ ����
	Object* Owner;
	
};

