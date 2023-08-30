#pragma once
#include "Server_Defines.h"
enum Type
{
    PLAYER = 0,
    MONSTER,
    SKILL
};
class Object
{
public:
    virtual void Update() {};

public:
    unsigned int ObjectId;
    float x, y, z;

    float vPos[3];
    Type type;
    
    class Collider* collider = nullptr;

    class Session* ownerSession = nullptr;
};

