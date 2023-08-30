#pragma once
#include <winsock2.h>
#include <Windows.h>
#include <stdint.h>
#include <functional>
#include <string>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <set>
#include <atomic>
#include <map>
#include <queue>
#include <list>
#include <vector>
#include "Packet.h"
#include "Types.h"

#define PACKET_SIZE 512
#define PORT 7778

using namespace std;

struct PacketHeader
{
    UINT16 size;
    UINT16 protocol; // 프로토콜ID (ex. 1=로그인, 2=이동요청)
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

    PKT_C_SKILL = 1010,
    PKT_S_SKILL = 1011,

    PKT_C_ADD_COLLIDER = 1012,
    PKT_S_ADD_COLLIDER = 1013,

    PKT_C_EXIT = 9998,
    PKT_S_EXIT = 9999
};