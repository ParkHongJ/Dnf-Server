#pragma once
#include "Server_Defines.h"

class Session;
using PacketHandlerFunc = std::function<bool(Session*, BYTE*, INT32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(Session* session, BYTE* buffer, INT32 len);
bool Handle_C_LOGIN(Session* session, BYTE* buffer);
bool Handle_C_ENTER_GAME(Session* session, BYTE* buffer);
bool Handle_C_CREATE_PLAYER(Session* session, BYTE* buffer);
bool Handle_C_MOVE(Session* session, BYTE* buffer);
bool Handle_C_EXIT(Session* session, BYTE* buffer);

class ClientPacketHandler
{
public:
    static void Init()
    {
        for (int i = 0; i < UINT16_MAX; i++)
        {
            GPacketHandler[i] = Handle_INVALID;
        }
        GPacketHandler[PKT_C_LOGIN] = [](Session* session, BYTE* buffer, INT32 len)
        {
            return Handle_C_LOGIN(session, buffer);
        };
        GPacketHandler[PKT_C_ENTER_GAME] = [](Session* session, BYTE* buffer, INT32 len)
        {
            return Handle_C_ENTER_GAME(session, buffer);
        };
        GPacketHandler[PKT_C_CREATE_PLAYER] = [](Session* session, BYTE* buffer, INT32 len)
        {
            return Handle_C_CREATE_PLAYER(session, buffer);
        };
        GPacketHandler[PKT_C_MOVE] = [](Session* session, BYTE* buffer, INT32 len)
        {
            return Handle_C_MOVE(session, buffer);
        };
        GPacketHandler[PKT_C_EXIT] = [](Session* session, BYTE* buffer, INT32 len)
        {
            return Handle_C_EXIT(session, buffer);
        };
    }
    static bool HandlePacket(Session* session, BYTE* buffer, INT32 len);
};
