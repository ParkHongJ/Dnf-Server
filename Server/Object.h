#pragma once
#include "Server_Defines.h"
enum Type
{
    PLAYER = 0,
    MONSTER,
    SKILL,
    PROJECTILE
};
struct StatInfo
{
    float hp = 100.f;
    float maxhp;
};
class Object
{
public:
    virtual void Update() {};
    virtual void OnDamaged(Object* attacker, float Damage) {};
    virtual void OnDead(Object* attacker) {};
public:
    unsigned int ObjectId;
    float x, y, z;

    float vPos[3];
    Type type;
    
    class Collider* collider = nullptr;

    class Session* ownerSession = nullptr;

    StatInfo stat;
};

