#pragma once
#include <winsock2.h>
#include <stdint.h>
#include <functional>
#include "Packet.h"

struct PacketHeader
{
    UINT16 size;
    UINT16 protocol; // ��������ID (ex. 1=�α���, 2=�̵���û)
};

enum : UINT16
{
    PKT_C_LOGIN = 1000,
    PKT_S_LOGIN = 1001,
    PKT_C_ENTER_GAME = 1002,
    PKT_S_ENTER_GAME = 1003,
    PKT_C_CHAT = 1004,
    PKT_S_CHAT = 1005,
    PKT_C_CREATE_PLAYER = 1006,
    PKT_S_CREATE_PLAYER = 1007,

    PKT_C_MOVE = 1008,
    PKT_S_MOVE = 1009,

    PKT_C_EXIT = 9998,
    PKT_S_EXIT = 9999
};