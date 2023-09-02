#pragma once
#include "Server_Defines.h"
class Object;
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
public:
    //주인의 오브젝트
    Object* Owner;

    //자신이 속한 그룹
    int Group;
    
    float WorldMax[3];
    float WorldMin[3];

    float Size[3];    
    float Center[3];    

    //AABB박스의 min, max를 업데이트 하는 함수
    void UpdateMinMax();
    void UpdateMinMax(float* vPos);
};

void CollisionLoop(int Row, int Col, list<Collider>* CollisionObjects, unordered_map<LONGLONG, bool>& ColliderInfo);

bool IsCollisionAABB(Collider Sour, Collider Dest);

class CollisionMessageQueue
{
public:
    // 메시지를 큐에 추가
    void PushMessage(Collider* collider) {
        lock_guard<std::mutex> lockguard(mutex);
        colliders.push(collider);
    }

    // 메시지를 큐에서 팝
    Collider* PopMessage() {
        lock_guard<std::mutex> lockguard(mutex);
        if (colliders.empty()) {
            throw std::runtime_error("Queue is empty");
        }
        Collider* collider = colliders.front();
        colliders.pop();
        return collider;
    }

    bool IsEmpty() {
        lock_guard<std::mutex> lockguard(mutex);
        return colliders.empty();
    }

private:
    queue<Collider*> colliders;
    mutex lock;
};

class CollisionThread
{
public:
    void Init();

    void Simulate();
    void AddCollisionObject(Collider ColliderObject, int Group);

    void SetArrayCheck();
    void Release();

public:
    static CollisionMessageQueue colliderQueue;

private:
    bool bSimulate = true;
    mutex lock;

    list<Collider> CollisionObjects[2];

    unordered_map<LONGLONG, bool> m_ColInfo;
    int arrCheck[2];    
};

