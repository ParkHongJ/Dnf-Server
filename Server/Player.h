#pragma once
#include "Object.h"
class Player : public Object
{

public:
	enum STATE
	{
		IDLE = 4,
		WALK = 8,
		RUN,
		SKILL,
		HIT,
		Dead
	};

    Player();
    ~Player();
    virtual void OnDamaged(Object* attacker, float Damage) override;
    virtual void OnDead(Object* attacker) override;

};

