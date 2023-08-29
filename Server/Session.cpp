#include "Session.h"
#include "ClientPacketHandler.h"
Session::Session(SOCKET clientSocket)
	: socket(clientSocket)
{
}

void Session::Send(BYTE* sendBuffer)
{
	send(socket, (const char*)sendBuffer, PACKET_SIZE, 0);
}

bool Session::Recv()
{
	BYTE packet[PACKET_SIZE] = "";
	int length = recv(GetSocket(), (char*)packet, sizeof(packet), 0);

	PacketHeader* header = (PacketHeader*)packet;

	if (header->protocol == PKT_C_EXIT)
		return false;

	ClientPacketHandler::HandlePacket(GetSocket(), (BYTE*)packet, PACKET_SIZE);
	return true;
}
