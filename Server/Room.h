#pragma once
#include "Server_Defines.h"
class Player;
class Room
{
public:
	void Enter(Player* player);
	void Leave(Player* player);
	void Broadcast(BYTE* sendBuffer);
	void BroadcastWithOutMe(int playerId, BYTE* sendBuffer);
	void SendRoomInfo(int playerId, BYTE* sendBuffer, int bufferOffset);

private:
	mutex lock;
	map<uint64, Player*> _players;
};

extern Room GRoom;