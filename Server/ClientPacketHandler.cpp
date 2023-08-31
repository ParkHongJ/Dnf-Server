#include "ClientPacketHandler.h"
#include "Session.h"
#include "Player.h"
#include "Room.h"
#include "DnfServer.h"
#include "Skill.h"
#include "CollisionThread.h"
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
	{
		player->ObjectId = DnfServer::idGenerator++;
		player->ownerSession = session;
		session->AddPlayer(player);
	}

	memcpy(sendBuffer + sizeof(PacketHeader), &player->ObjectId, sizeof(player->ObjectId));

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
    player->type = PLAYER;

    GRoom.Enter(player);

    GRoom.SendRoomInfo(player->ObjectId, sendBuffer, bufferOffset);

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
    
    memcpy(sendBuffer + BufferSize, &session->players[session->CurrentPlayerIndex]->ObjectId, sizeof(int));

    BufferSize += sizeof(int);

    bool bLocallyControlled = false;
    memcpy(sendBuffer + BufferSize, &bLocallyControlled, sizeof(bool));

    GRoom.BroadcastWithOutMe(session->players[session->CurrentPlayerIndex]->ObjectId, sendBuffer);

    bLocallyControlled = true;

    memcpy(sendBuffer + BufferSize, &bLocallyControlled, sizeof(bool));

    session->Send(sendBuffer);
	return true;
}

bool Handle_C_MOVE(Session* session, BYTE* buffer)
{
    (*(PacketHeader*)buffer).protocol = PKT_S_MOVE;

    int bufferOffset = sizeof(PacketHeader);

    //id    
    int id;
    memcpy(&id, buffer + bufferOffset, sizeof(id));
    bufferOffset += sizeof(id);

    //state
    int state;
    memcpy(&state, buffer + bufferOffset, sizeof(state));
    bufferOffset += sizeof(state);
    
    //dir
    int dir;
    memcpy(&dir, buffer + bufferOffset, sizeof(dir));
    bufferOffset += sizeof(dir);

    //pos    
    float vPos[3];
    memcpy(&vPos, buffer + bufferOffset, sizeof(float) * 3);
    bufferOffset += sizeof(float) * 3;

    Object* object = GRoom.FindPlayerById(id);

    if (object == nullptr)
    {
        cout << "Handle_C_MOVE : Room 오브젝트 nullptr" << endl;
        return false;
    }

    //위치 갱신
    object->x = vPos[0];
    object->y = vPos[1];
    object->z = vPos[2];

    GRoom.BroadcastWithOutMe(session->players[session->CurrentPlayerIndex]->ObjectId, buffer);

	return true;
}

bool Handle_C_EXIT(Session* session, BYTE* buffer)
{
    BYTE sendBuffer[PACKET_SIZE] = "";

    PacketHeader header;

    header.protocol = PKT_S_EXIT;
    header.size = PACKET_SIZE;
    (*(PacketHeader*)sendBuffer) = header;

    int id = session->players[session->CurrentPlayerIndex]->ObjectId;

    memcpy(sendBuffer + sizeof(PacketHeader), &id, sizeof(id));

    GRoom.BroadcastWithOutMe(id, sendBuffer);
    GRoom.Leave(session->players[session->CurrentPlayerIndex]);

    return false;
}

bool Handle_C_SKILL(Session* session, BYTE* buffer)
{
    int bufferOffset = sizeof(PacketHeader);

    int id;
    
    memcpy(&id, buffer + bufferOffset, sizeof(int));

    bufferOffset += sizeof(int);

    float vPos[3];

    memcpy(&vPos, buffer + bufferOffset, sizeof(float) * 3);

    bufferOffset += sizeof(float) * 3;

    Skill* skill = new Skill();
    Object* Owner = GRoom.FindPlayerById(id);

    if (Owner == nullptr)
        return false;

    skill->ObjectId = DnfServer::idGenerator++;
    skill->type = SKILL;
    skill->ownerSession = session;
    skill->Owner = Owner;
    skill->x = vPos[0];
    skill->y = vPos[1];
    skill->z = vPos[2];

    GRoom.Enter(skill);

    cout << id << " 번 플레이어가 스킬 사용" << endl;
    cout << "스킬 위치 : " << vPos[0] << " " << vPos[1] << " " << vPos[2] << endl;
    
    BYTE sendBuffer[PACKET_SIZE] = "";

    PacketHeader header;
    header.protocol = PKT_S_SKILL;
    header.size = PACKET_SIZE;

    int sendBufferOffset = sizeof(PacketHeader);

    *(PacketHeader*)sendBuffer = header;

    //몇번 플레이어가
    memcpy(sendBuffer + sendBufferOffset, &Owner->ObjectId, sizeof(Owner->ObjectId));
    
    sendBufferOffset += sizeof(Owner->ObjectId);

    //스킬 id
    memcpy(sendBuffer + sendBufferOffset, &skill->ObjectId, sizeof(skill->ObjectId));

    sendBufferOffset += sizeof(skill->ObjectId);

    //어디에
    memcpy(sendBuffer + sendBufferOffset, &vPos, sizeof(float) * 3);

    sendBufferOffset += sizeof(float) * 3;

    GRoom.Broadcast(sendBuffer);

    return true;
}

bool Handle_C_ADD_COLLIDER(Session* session, BYTE* buffer)
{
    int bufferOffset = sizeof(PacketHeader);

    int id;

    memcpy(&id, buffer + bufferOffset, sizeof(id));

    bufferOffset += sizeof(id);

    Type type;

    memcpy(&type, buffer + bufferOffset, sizeof(type));

    bufferOffset += sizeof(type);

    Object* object = nullptr;

    if (type == Type::PLAYER)
    {
        object = GRoom.FindPlayerById(id); //Lock
    }
    else if (type == Type::SKILL)
    {
        object = GRoom.FindSkillById(id); //Lock
    }

    if (object == nullptr)
    {
        cout << "Error : Handle_C_ADD_COLLIDER" << endl;
        return false;
    }
    float Size[3];

    memcpy(&Size, buffer + bufferOffset, sizeof(float) * 3);

    bufferOffset += sizeof(float) * 3;

    float Center[3];

    memcpy(&Center, buffer + bufferOffset, sizeof(float) * 3);

    bufferOffset += sizeof(float) * 3;

    if (object->collider == nullptr)
    {
        Collider* collider = new Collider();

        collider->Owner = object;
        
        collider->Center[0] = Center[0];
        collider->Center[1] = Center[1];
        collider->Center[2] = Center[2];

        collider->Size[0] = Size[0];
        collider->Size[1] = Size[1];
        collider->Size[2] = Size[2];

        object->collider = collider;
    }

    //굳이 broadcast를 해야할까?

    return true;
}
