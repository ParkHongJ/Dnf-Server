#include <iostream>
#include <winsock2.h>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <list>
#include <unordered_map>
#include <set>
#include "DnfServer.h"
#include "ClientPacketHandler.h"
#define PACKET_SIZE 512
#define PORT 7778

using namespace std;

//struct Player {
//    SOCKET socket;
//    unsigned int PlayerId;
//    float x, y, z;
//
//    float vRight[3];
//    float vUp[3];
//    float vLook[3];
//
//    float vPos[3];
//};
//struct PacketHeader
//{
//    UINT16 size;
//    UINT16 protocol; // ��������ID (ex. 1=�α���, 2=�̵���û)
//};
//
//enum : UINT16
//{
//    PKT_C_LOGIN = 1000,
//    PKT_S_LOGIN = 1001,
//    PKT_C_ENTER_GAME = 1002,
//    PKT_S_ENTER_GAME = 1003,
//    PKT_C_CHAT = 1004,
//    PKT_S_CHAT = 1005,
//    PKT_C_CREATE_PLAYER = 1006,
//    PKT_S_CREATE_PLAYER = 1007,
//
//    PKT_C_MOVE = 1008,
//    PKT_S_MOVE = 1009,
//
//    PKT_C_EXIT = 9998,
//    PKT_S_EXIT = 9999
//};
//
//int g_ObjectId = 0;
//
//mutex g_lock;
//
//vector<Player> Clients;
//
//enum COLLISIONGROUP
//{
//    COLLISION_PLAYER,
//    COLLISION_MONSTER, //�ӽ�
//    COLLISION_GROUP_MAX
//};
//
//
//union COLLIDER_ID
//{
//    struct {
//        unsigned int Left_ID;
//        unsigned int Right_ID;
//    };
//    LONGLONG ID;
//};
//
//struct Collider 
//{
//    //������ ID
//    int OwnerId;
//    //�ڽ��� ID
//    int ColliderId;
//
//};
//unordered_map<LONGLONG, bool> m_ColInfo;

int main()
{
    ClientPacketHandler::Init();
    DnfServer server(NetAddress(L"127.0.0.1", PORT), 10);

    server.StartServer();

    //WSADATA wsaData; 
    //WSAStartup(MAKEWORD(2, 2), &wsaData); 

    //SOCKET hListen; 
    //hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    /*SOCKADDR_IN ListenAddr = {};
    ListenAddr.sin_family = AF_INET;
    ListenAddr.sin_port = htons(PORT);
    ListenAddr.sin_addr.s_addr = INADDR_ANY;*/

    //bind(hListen, (SOCKADDR*)&ListenAddr, sizeof(ListenAddr));
    //listen(hListen, SOMAXCONN);

    //queue<BYTE*> Packets;

    //vector<thread> ClientThread;
    //while (true)
    //{
    //    SOCKADDR_IN SockAddr = {};
    //    int SockSize = sizeof(SockAddr);
    //    SOCKET hClient = accept(hListen, (SOCKADDR*)&SockAddr, &SockSize);

    //    if (hClient == INVALID_SOCKET) 
    //    {
    //        int error = WSAGetLastError();
    //        cout << "Accept Error : " << error << endl;
    //        continue;
    //    }
    //    cout << "Ŭ���̾�Ʈ ���� �Ϸ�" << "\n";   

    //    thread WorkerThread = thread([=]()
    //        {
    //            bool bThreadLoop = true;
    //            while (bThreadLoop)
    //            {
    //                BYTE packet[PACKET_SIZE] = "";
    //                int length = recv(hClient, (char*)packet, sizeof(packet), 0);

    //                PacketHeader* header = (PacketHeader*)packet;
    //                switch (header->protocol)
    //                {
    //                case PKT_C_LOGIN:
    //                {
    //                    BYTE packet[PACKET_SIZE] = "";

    //                    PacketHeader header;

    //                    header.protocol = PKT_S_LOGIN;
    //                    header.size = PACKET_SIZE;
    //                    (*(PacketHeader*)packet) = header;

    //                    Player player;

    //                    player.socket = hClient;
    //                    {
    //                        lock_guard<mutex> lockguard(g_lock);
    //                        player.PlayerId = g_ObjectId++;
    //                        Clients.push_back(player);
    //                    }

    //                    memcpy(packet + sizeof(PacketHeader), &player.PlayerId, sizeof(player.PlayerId));

    //                    send(hClient, (const char*)packet, PACKET_SIZE, 0);
    //                }
    //                break;
    //                case PKT_C_ENTER_GAME:
    //                {
    //                    //ĳ����â���� ���° ĳ���͸� ��� �����Ѵ�(0) ��������
    //                    BYTE packet[PACKET_SIZE] = "";

    //                    PacketHeader header;

    //                    header.protocol = PKT_S_ENTER_GAME;
    //                    header.size = PACKET_SIZE;
    //                    (*(PacketHeader*)packet) = header;

    //                    int bufferOffset = sizeof(PacketHeader);
    //                    //�濡 ���� ��� �ִ��� ��������

    //                    //���Ǽ���, �ڱ� �ڽ��� ������ ������ �ο���
    //                    unsigned int PlayerCount = Clients.size() - 1;

    //                    memcpy(packet + bufferOffset, &PlayerCount, sizeof(PlayerCount));

    //                    bufferOffset += sizeof(PlayerCount);

    //                    int id;
    //                    {
    //                        lock_guard<mutex> lockguard(g_lock);

    //                        for (auto& user : Clients)
    //                        {
    //                            if (user.socket != hClient)
    //                            {
    //                                id = user.PlayerId;
    //                                memcpy(packet + bufferOffset, &id, sizeof(id));
    //                                bufferOffset += sizeof(id);
    //                            }
    //                        }
    //                    }

    //                    send(hClient, (const char*)packet, PACKET_SIZE, 0);
    //                }
    //                break;
    //                case PKT_C_CREATE_PLAYER:
    //                {
    //                    BYTE SendBuffer[PACKET_SIZE] = "";

    //                    PacketHeader header;

    //                    header.protocol = PKT_S_CREATE_PLAYER;
    //                    header.size = PACKET_SIZE;
    //                    *(PacketHeader*)SendBuffer = header;

    //                    int BufferSize = sizeof(PacketHeader);

    //                    int id;

    //                    {
    //                        lock_guard<mutex> lockguard(g_lock);
    //                        
    //                        for (auto& user : Clients )
    //                        {
    //                            if (user.socket == hClient)
    //                            {
    //                                id = user.PlayerId;
    //                                break;
    //                            }
    //                        }
    //                    }

    //                    memcpy(SendBuffer + BufferSize, &id, sizeof(int));

    //                    BufferSize += sizeof(int);

				//		bool bIsPlayer = false;
				//		memcpy(SendBuffer + BufferSize, &bIsPlayer, sizeof(bool));
    //                    
				//		//BroadCast
				//		{
    //                        lock_guard<mutex> lockguard(g_lock);
				//			for (const Player& user : Clients)
				//			{
				//				if (user.socket != hClient)
				//				{
				//					send(user.socket, (const char*)SendBuffer, PACKET_SIZE, 0);
				//				}
				//			}
				//		}
				//		bIsPlayer = true;
    //                    
    //                    memcpy(SendBuffer + BufferSize, &bIsPlayer, sizeof(bool));

    //                    send(hClient, (const char*)SendBuffer, PACKET_SIZE, 0);
    //                }
    //                    break;
    //                case PKT_C_MOVE:
    //                {
    //                    BYTE SendBuffer[PACKET_SIZE] = "";

    //                    PacketHeader header;

    //                    header.protocol = PKT_S_MOVE;
    //                    header.size = PACKET_SIZE;

    //                    memcpy(SendBuffer, packet, PACKET_SIZE);

    //                    (*(PacketHeader*)SendBuffer).protocol = PKT_S_MOVE;

    //                    //BroadCast
    //                    {
    //                        lock_guard<mutex> lockguard(g_lock);
    //                        for (const Player& user: Clients)
    //                        {
    //                            if (user.socket != hClient)
    //                            {
    //                                send(user.socket, (const char*)SendBuffer, PACKET_SIZE, 0);
    //                            }
    //                        }
    //                    }
    //                }
    //                    break;
    //                case PKT_C_EXIT:
    //                {
    //                    BYTE packet[PACKET_SIZE] = "";

    //                    PacketHeader header;

    //                    header.protocol = PKT_S_EXIT;
    //                    header.size = PACKET_SIZE;
    //                    (*(PacketHeader*)packet) = header;

    //                    bThreadLoop = false;
    //                    cout << "Ŭ���̾�Ʈ ����" << endl;

    //                    int id = 0;

    //                    int iIndex = 0;
    //                    {
    //                        lock_guard<mutex> lockguard(g_lock);
    //                        for (auto& user : Clients)
    //                        {
    //                            if (user.socket == hClient)
    //                            {
    //                                id = user.PlayerId;
    //                                break;
    //                            }
    //                            iIndex++;
    //                        }
    //                    }

    //                    memcpy(packet + sizeof(PacketHeader), &id, sizeof(id));

    //                    {
    //                        lock_guard<mutex> lockguard(g_lock);
    //                        for (auto& user : Clients)
    //                        {
    //                            if (user.socket != hClient)
    //                            {
    //                                send(user.socket, (const char*)packet, PACKET_SIZE, 0);
    //                            }
    //                        }
    //                        Clients.erase(Clients.begin() + iIndex);
    //                    }
    //                    break;
    //                }
    //                }
    //            }
    //        });
    //    ClientThread.push_back(std::move(WorkerThread));
    //}

    //for (thread& Client : ClientThread)
    //{
    //    if (Client.joinable())
    //        Client.join();
    //}

    //closesocket(hListen);

    //WSACleanup();

    return 0;
}
