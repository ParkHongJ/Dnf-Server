#pragma once
#include "Server_Defines.h"

union COLLIDER_ID
{
    struct {
        unsigned int Left_ID;
        unsigned int Right_ID;
    };
    LONGLONG ID;
};

struct Collider
{
    //������ ID
    int OwnerId;

    //�ڽ��� ID
    int ColliderId;

    float WorldMax[3];
    float WorldMin[3];    
};

void CollisionLoop(int Row, int Col, list<Collider>* CollisionObjects, unordered_map<LONGLONG, bool>& ColliderInfo);

bool IsCollisionAABB(Collider Sour, Collider Dest);

class CollisionThread
{
public:
    void Init();

    void Launch();
    void AddCollisionObject(Collider ColliderObject, int Group);

    void SetArrayCheck();

private:
    mutex lock;

    list<Collider> CollisionObjects[2];

    unordered_map<LONGLONG, bool> m_ColInfo;
    int arrCheck[2];
};

