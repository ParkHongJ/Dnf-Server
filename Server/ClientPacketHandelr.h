#pragma once
#include "Server_Defines.h"

using PacketHandlerFunc = std::function<bool(SOCKET, BYTE*, INT32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(SOCKET socket, BYTE* buffer, INT32 len);
bool Handle_S_LOGIN(SOCKET socket, BYTE* buffer);
bool Handle_S_ENTER_GAME(SOCKET socket, BYTE* buffer);
bool Handle_S_CREATE_PLAYER(SOCKET socket, BYTE* buffer);
bool Handle_S_MOVE(SOCKET socket, BYTE* buffer);

class ServerPacketHandler
{

public:

    static void Init()
    {
        for (int i = 0; i < UINT16_MAX; i++)
        {
            GPacketHandler[i] = Handle_INVALID;
        }
        GPacketHandler[PKT_S_LOGIN] = [](SOCKET socket, BYTE* buffer, INT32 len)
        {
            return Handle_S_LOGIN(socket, buffer);
        };
        GPacketHandler[PKT_S_ENTER_GAME] = [](SOCKET socket, BYTE* buffer, INT32 len)
        {
            return Handle_S_ENTER_GAME(socket, buffer);
        };
        GPacketHandler[PKT_S_CREATE_PLAYER] = [](SOCKET socket, BYTE* buffer, INT32 len)
        {
            return Handle_S_CREATE_PLAYER(socket, buffer);
        };
        GPacketHandler[PKT_S_MOVE] = [](SOCKET socket, BYTE* buffer, INT32 len)
        {
            return Handle_S_MOVE(socket, buffer);
        };
    }
    static bool HandlePacket(SOCKET socket, BYTE* buffer, INT32 len);
};
