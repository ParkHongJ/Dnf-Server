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
    //������ ������Ʈ
    Object* Owner;

    //�ڽ��� ���� �׷�
    int Group;
    
    float WorldMax[3];
    float WorldMin[3];

    float Size[3];    
    float Center[3];    

    //AABB�ڽ��� min, max�� ������Ʈ �ϴ� �Լ�
    void UpdateMinMax();
    void UpdateMinMax(float* vPos);
};

void CollisionLoop(int Row, int Col, list<Collider>* CollisionObjects, unordered_map<LONGLONG, bool>& ColliderInfo);

bool IsCollisionAABB(Collider Sour, Collider Dest);

class CollisionMessageQueue
{
public:
    // �޽����� ť�� �߰�
    void PushMessage(Collider* collider) {
        lock_guard<std::mutex> lockguard(mutex);
        colliders.push(collider);
    }

    // �޽����� ť���� ��
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

