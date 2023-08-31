#pragma once
#include "Object.h"
class Monster : public Object
{
public:
	virtual void OnDamaged(Object* attacker, float Damage) override;
};

