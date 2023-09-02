#pragma once
#include "Object.h"

enum STATE
{
	Idle = 0,
	Patrol,
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
	
	virtual void IdleTick();
	virtual void PatrolTick();
	virtual void ChaseTick();
	virtual void AttackTick();
	virtual void DeadTick();
	virtual void HitTick();
	
	virtual void UpdateState(STATE newState);


	long StartMoveTick = 0;
	long currentMoveTick;
	STATE state;
};

