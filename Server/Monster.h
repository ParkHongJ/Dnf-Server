#pragma once
#include "Object.h"

enum STATE
{
	Idle = 0,
	Chase,
	Attack,
	Dead,
	Hit,
	End
};

class Monster : public Object
{
public:
	Monster();
	virtual void Update() override;
	virtual void OnDamaged(Object* attacker, float Damage) override;
	virtual void OnDead(Object* attacker) override;
	virtual void Initialzie();
	virtual void IdleTick();
	virtual void ChaseTick();
	virtual void AttackTick();
	virtual void DeadTick();
	virtual void HitTick();
	
	virtual void UpdateState(STATE newState);	

	long StartMoveTick = 0;
	long currentMoveTick;
	float IdleWait = 0.f;
	STATE state;
	float fSpeed = 0.05f;

	float StartPos[3];
	bool bLeft = true;
};

