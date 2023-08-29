#include "Session.h"
#include "ClientPacketHandler.h"
Session::Session(SOCKET clientSocket)
	: socket(clientSocket)
{
}

Session::~Session()
{
	for (auto& player : players)
	{
		delete player;
	}
	players.clear();
}

void Session::Send(BYTE* sendBuffer)
{
	send(socket, (const char*)sendBuffer, PACKET_SIZE, 0);
}

bool Session::Recv()
{
	BYTE packet[PACKET_SIZE] = "";
	int length = recv(GetSocket(), (char*)packet, sizeof(packet), 0);

	return ClientPacketHandler::HandlePacket(this, (BYTE*)packet, PACKET_SIZE);
}

void Session::AddPlayer(Player* player)
{
	players.push_back(player);
}
