#pragma once
class Player
{
public:
    Player();
    ~Player();
    unsigned int PlayerId;
    float x, y, z;

    float vRight[3];
    float vUp[3];
    float vLook[3];

    float vPos[3];

    class Session* ownerSession = nullptr;
};

