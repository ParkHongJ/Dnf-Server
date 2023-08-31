#include "CollisionThread.h"
#include "CollisionMessageQueue.h"

#include "Object.h"
CollisionMessageQueue CollisionThread::colliderQueue;

void CollisionThread::Init()
{
}

void CollisionThread::Simulate()
{
    thread collisionThread([this]() {

        while (bSimulate)
        {
            if (colliderQueue.IsEmpty()) //WHITE_LOCK
            {
                //충돌체 추가
            }
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

    if (collisionThread.joinable())
        collisionThread.join();
}

void CollisionThread::AddCollisionObject(Collider ColliderObject, int Group)
{
    lock_guard<mutex> lockguard(lock);
    CollisionObjects[Group].push_back(ColliderObject);
}

void CollisionThread::Release()
{
    lock_guard<mutex> lockguard(lock);
    bSimulate = false;
}

void CollisionLoop(int Row, int Col, list<Collider>* CollisionObjects, unordered_map<LONGLONG, bool>& ColliderInfo)
{
    //충돌 검사
    //unordered_map<LONGLONG, bool>::iterator iter;

    //for (Collider& Sour : CollisionObjects[Row])
    //{
    //    for (Collider& Dest : CollisionObjects[Col])
    //    {
    //        if (Dest.ColliderId == Sour.ColliderId)
    //            continue;

    //        COLLIDER_ID ID;
    //        ID.Left_ID = Sour.ColliderId;
    //        ID.Right_ID = Dest.ColliderId;
    //        iter = ColliderInfo.find(ID.ID);

    //        if (ColliderInfo.end() == iter)
    //        {
    //            ColliderInfo.insert(make_pair(ID.ID, false));
    //            iter = ColliderInfo.find(ID.ID);
    //        }

    //        if (IsCollisionAABB(Sour, Dest))
    //        {
    //            if (iter->second)
    //            {
    //                //Stay
    //            }
    //            else
    //            {
    //                //Enter
    //                iter->second = true;
    //            }
    //        }
    //        else
    //        {
    //            if (iter->second)
    //            {
    //                //Exit
    //                iter->second = false;
    //            }
    //        }
    //    }
    //}
}

bool IsCollisionAABB(Collider Sour, Collider Dest)
{
    if (Sour.WorldMax[0] < Dest.WorldMin[0] || Sour.WorldMin[0] > Dest.WorldMax[0]) return false;
    if (Sour.WorldMax[1] < Dest.WorldMin[1] || Sour.WorldMin[0] > Dest.WorldMax[1]) return false;
    if (Sour.WorldMax[2] < Dest.WorldMin[2] || Sour.WorldMin[2] > Dest.WorldMax[2]) return false;

    return true;
}

void Collider::UpdateMinMax()
{
    if (Owner == nullptr)
        return;
    
    //Owner의 위치가 갱신이 되었다고 가정하고,
    
    float ColliderPos[3];
    ColliderPos[0] = Owner->x + Center[0];
    ColliderPos[1] = Owner->y + Center[1];
    ColliderPos[2] = Owner->z + Center[2];

    for (int i = 0; i < 3; i++)
    {
        WorldMin[i] = ColliderPos[i] - Size[i] / 2.f;
        WorldMax[i] = ColliderPos[i] + Size[i] / 2.f;
    }
}
