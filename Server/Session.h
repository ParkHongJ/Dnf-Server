#pragma once
#include "Server_Defines.h"

class Player;
class Session
{
public:
	Session(SOCKET clientSocket);
	~Session();
	void Send(BYTE* sendBuffer);

	SOCKET GetSocket() { return socket; }

	bool Recv();
	void AddPlayer(Player* player);
	vector<Player*> players;
	int CurrentPlayerIndex;
private:
	SOCKET socket = INVALID_SOCKET;
};

