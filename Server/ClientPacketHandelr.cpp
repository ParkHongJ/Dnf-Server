#include "ClientPacketHandelr.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool ServerPacketHandler::HandlePacket(SOCKET socket, BYTE* buffer, INT32 len)
{
	PacketHeader* header = (PacketHeader*)buffer;

	GPacketHandler[header->protocol](socket, buffer, len);

	return false;
}

bool Handle_INVALID(SOCKET socket, BYTE* buffer, INT32 len)
{
	return false;
}

bool Handle_S_LOGIN(SOCKET socket, BYTE* buffer)
{
	return true;
}

bool Handle_S_ENTER_GAME(SOCKET socket, BYTE* buffer)
{
	return true;
}

bool Handle_S_CREATE_PLAYER(SOCKET socket, BYTE* buffer)
{
	return true;
}

bool Handle_S_MOVE(SOCKET socket, BYTE* buffer)
{

	return true;
}
