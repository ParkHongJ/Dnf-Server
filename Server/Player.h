#pragma once
#include "Object.h"
class Player : public Object
{
public:
    Player();
    ~Player();
    virtual void OnDamaged(Object* attacker, float Damage) override;
    virtual void OnDead(Object* attacker) override;

};

