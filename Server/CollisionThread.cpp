#include "CollisionThread.h"

void CollisionThread::Launch()
{
    thread collisionThread([this]() {
        while (true)
        {
            for (size_t Row = 0; Row < 2; ++Row)
            {
                for (size_t Col = Row; Col < 2; ++Col)
                {
                    if (arrCheck[Row] & (1 << Col))
                    {
                        CollisionLoop(Row, Col, CollisionObjects, m_ColInfo);
                    }
                }
            }
        }
        });
}

void CollisionThread::AddCollisionObject(Collider ColliderObject, int Group)
{
    lock_guard<mutex> lockguard(lock);
    CollisionObjects[Group].push_back(ColliderObject);
}

void CollisionLoop(int Row, int Col, list<Collider>* CollisionObjects, unordered_map<LONGLONG, bool>& ColliderInfo)
{
    //충돌 검사
    unordered_map<LONGLONG, bool>::iterator iter;

    for (Collider& Sour : CollisionObjects[Row])
    {
        for (Collider& Dest : CollisionObjects[Col])
        {
            if (Dest.ColliderId == Sour.ColliderId)
                continue;

            COLLIDER_ID ID;
            ID.Left_ID = Sour.ColliderId;
            ID.Right_ID = Dest.ColliderId;
            iter = ColliderInfo.find(ID.ID);

            if (ColliderInfo.end() == iter)
            {
                ColliderInfo.insert(make_pair(ID.ID, false));
                iter = ColliderInfo.find(ID.ID);
            }

            if (IsCollisionAABB(Sour, Dest))
            {
                if (iter->second)
                {
                    //Stay
                }
                else
                {
                    //Enter
                    iter->second = true;
                }
            }
            else
            {
                if (iter->second)
                {
                    //Exit
                    iter->second = false;
                }
            }
        }
    }
}

bool IsCollisionAABB(Collider Sour, Collider Dest)
{
    // X 축에서의 겹침 확인
    bool xOverlap = Sour.WorldMax[0] >= Dest.WorldMin[0] && Dest.WorldMax[0] >= Sour.WorldMin[0];

    // Y 축에서의 겹침 확인
    bool yOverlap = Sour.WorldMax[1] >= Dest.WorldMin[1] && Dest.WorldMax[1] >= Sour.WorldMin[1];

    // Z 축에서의 겹침 확인
    bool zOverlap = Sour.WorldMax[2] >= Dest.WorldMin[2] && Dest.WorldMax[2] >= Sour.WorldMin[2];

    // X 축, Y 축, Z 축에서 모두 겹친다면 충돌
    return xOverlap && yOverlap && zOverlap;
}
