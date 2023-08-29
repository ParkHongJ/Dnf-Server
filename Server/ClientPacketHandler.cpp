#include "ClientPacketHandler.h"
#include "Session.h"
#include "Player.h"
#include "Room.h"
PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool ClientPacketHandler::HandlePacket(Session* session, BYTE* buffer, INT32 len)
{
	PacketHeader* header = (PacketHeader*)buffer;

	return GPacketHandler[header->protocol](session, buffer, len);
}

bool Handle_INVALID(Session* session, BYTE* buffer, INT32 len)
{
	return false;
}

bool Handle_C_LOGIN(Session* session, BYTE* buffer)
{
	BYTE sendBuffer[PACKET_SIZE] = "";

	PacketHeader header;

	header.protocol = PKT_S_LOGIN;
	header.size = PACKET_SIZE;
	(*(PacketHeader*)sendBuffer) = header;

	Player* player = new Player();
	static atomic<uint64> idGenerator = 0;
	{
		player->PlayerId = idGenerator++;
		player->ownerSession = session;
		session->AddPlayer(player);
	}

	memcpy(sendBuffer + sizeof(PacketHeader), &player->PlayerId, sizeof(player->PlayerId));

	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_GAME(Session* session, BYTE* buffer)
{
    BYTE sendBuffer[PACKET_SIZE] = "";
    PacketHeader header;

    header.protocol = PKT_S_ENTER_GAME;
    header.size = PACKET_SIZE;
    (*(PacketHeader*)sendBuffer) = header;

    int bufferOffset = sizeof(PacketHeader);

    //방에 누가 몇명 있는지 생성정보
    int playerIndex;
    memcpy(&playerIndex, buffer + bufferOffset, sizeof(playerIndex));

    session->CurrentPlayerIndex = playerIndex;
    Player* player = session->players[playerIndex];

    GRoom.Enter(player);

    GRoom.SendRoomInfo(player->PlayerId, sendBuffer, bufferOffset);

	return true;
}

bool Handle_C_CREATE_PLAYER(Session* session, BYTE* buffer)
{
    BYTE sendBuffer[PACKET_SIZE] = "";

    PacketHeader header;

    header.protocol = PKT_S_CREATE_PLAYER;
    header.size = PACKET_SIZE;
    *(PacketHeader*)sendBuffer = header;

    int BufferSize = sizeof(PacketHeader);
    
    memcpy(sendBuffer + BufferSize, &session->players[session->CurrentPlayerIndex]->PlayerId, sizeof(int));

    BufferSize += sizeof(int);

    bool bLocallyControlled = false;
    memcpy(sendBuffer + BufferSize, &bLocallyControlled, sizeof(bool));

    GRoom.BroadcastWithOutMe(session->players[session->CurrentPlayerIndex]->PlayerId, sendBuffer);

    bLocallyControlled = true;

    memcpy(sendBuffer + BufferSize, &bLocallyControlled, sizeof(bool));

    session->Send(sendBuffer);
	return true;
}

bool Handle_C_MOVE(Session* session, BYTE* buffer)
{
    (*(PacketHeader*)buffer).protocol = PKT_S_MOVE;

    GRoom.BroadcastWithOutMe(session->players[session->CurrentPlayerIndex]->PlayerId, buffer);

	return true;
}

bool Handle_C_EXIT(Session* session, BYTE* buffer)
{
    BYTE sendBuffer[PACKET_SIZE] = "";

    PacketHeader header;

    header.protocol = PKT_S_EXIT;
    header.size = PACKET_SIZE;
    (*(PacketHeader*)sendBuffer) = header;

    int id = session->players[session->CurrentPlayerIndex]->PlayerId;

    memcpy(sendBuffer + sizeof(PacketHeader), &id, sizeof(id));

    GRoom.BroadcastWithOutMe(id, sendBuffer);
    GRoom.Leave(session->players[session->CurrentPlayerIndex]);

    return false;
}
