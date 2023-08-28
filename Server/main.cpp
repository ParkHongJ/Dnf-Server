#include <iostream>
#include <winsock2.h>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#define PACKET_SIZE 512
#define PORT 7778

using namespace std;

struct Player {
    SOCKET socket;
    unsigned int PlayerId;
    float x, y, z;
};
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

    PKT_C_EXIT = 9998,
    PKT_S_EXIT = 9999
};

int g_ObjectId = 0;

mutex g_lock;

vector<Player> Clients;
int main()
{
    WSADATA wsaData; 
    WSAStartup(MAKEWORD(2, 2), &wsaData); 

    SOCKET hListen; 
    hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN ListenAddr = {};
    ListenAddr.sin_family = AF_INET;
    ListenAddr.sin_port = htons(PORT);
    ListenAddr.sin_addr.s_addr = INADDR_ANY;

    bind(hListen, (SOCKADDR*)&ListenAddr, sizeof(ListenAddr));
    listen(hListen, SOMAXCONN);


    queue<BYTE*> Packets;

    //g_ObjectInfo.resize(PP_END);
   // thread collisionThread(Collision_AABB);

    vector<thread> ClientThread;
    while (true)
    {
        SOCKADDR_IN SockAddr = {};
        int SockSize = sizeof(SockAddr);
        SOCKET hClient = accept(hListen, (SOCKADDR*)&SockAddr, &SockSize);

        if (hClient == INVALID_SOCKET) 
        {
            int error = WSAGetLastError();
            cout << "Accept Error : " << error << endl;
            continue;
        }
        cout << "클라이언트 연결 완료" << "\n";

        BYTE packet[PACKET_SIZE] = "";

        PacketHeader header;
        
        header.protocol = PKT_S_LOGIN;
        header.size = PACKET_SIZE;
        (*(PacketHeader*)packet) = header;

        Player player;

        player.PlayerId = g_ObjectId++;
        player.socket = hClient;

        memcpy(packet + sizeof(PacketHeader), &player.PlayerId, sizeof(player.PlayerId));

        send(hClient, (const char*)packet, PACKET_SIZE, 0);

        Clients.push_back(player);

        thread WorkerThread = thread([=]()
            {
                bool bThreadLoop = true;
                while (bThreadLoop)
                {
                    BYTE packet[PACKET_SIZE] = "";
                    int length = recv(hClient, (char*)packet, sizeof(packet), 0);

                    PacketHeader* header = (PacketHeader*)packet;
                    switch (header->protocol)
                    {
                    case PKT_C_LOGIN:
                    {

                    }
                    break;
                    case PKT_C_ENTER_GAME:
                    {
                        BYTE packet[PACKET_SIZE] = "";

                        PacketHeader header;

                        header.protocol = PKT_S_ENTER_GAME;
                        header.size = PACKET_SIZE;
                        (*(PacketHeader*)packet) = header;

                        int bufferOffset = sizeof(PacketHeader);
                        //방에 누가 몇명 있는지 생성정보

                        //임의설정, 자기 자신을 제외한 나머지 인원수
                        unsigned int PlayerCount = Clients.size() - 1;

                        memcpy(packet + bufferOffset, &PlayerCount, sizeof(PlayerCount));

                        bufferOffset += sizeof(PlayerCount);

                        int id;
                        {
                            lock_guard<mutex> lockguard(g_lock);

                            for (auto& user : Clients)
                            {
                                if (user.socket != hClient)
                                {
                                    id = user.PlayerId;
                                    memcpy(packet + bufferOffset, &id, sizeof(id));
                                    bufferOffset += sizeof(id);
                                }
                            }
                        }


                        send(hClient, (const char*)packet, PACKET_SIZE, 0);
                    }
                    break;
                    case PKT_C_CREATE_PLAYER:
                    {
                        BYTE SendBuffer[PACKET_SIZE] = "";

                        PacketHeader header;

                        header.protocol = PKT_S_CREATE_PLAYER;
                        header.size = PACKET_SIZE;
                        *(PacketHeader*)SendBuffer = header;

                        int BufferSize = sizeof(PacketHeader);

                        int id;

                        {
                            lock_guard<mutex> lockguard(g_lock);
                            
                            for (auto& user : Clients )
                            {
                                if (user.socket == hClient)
                                {
                                    id = user.PlayerId;
                                    break;
                                }
                            }
                        }

                        memcpy(SendBuffer + BufferSize, &id, sizeof(int));

                        BufferSize += sizeof(int);

						bool bIsPlayer = false;
						memcpy(SendBuffer + BufferSize, &bIsPlayer, sizeof(bool));
                        
						//BroadCast
						{
                            lock_guard<mutex> lockguard(g_lock);
							for (const Player& user : Clients)
							{
								if (user.socket != hClient)
								{
									send(user.socket, (const char*)SendBuffer, PACKET_SIZE, 0);
								}
							}
						}
						bIsPlayer = true;
                        
                        memcpy(SendBuffer + BufferSize, &bIsPlayer, sizeof(bool));

                        send(hClient, (const char*)SendBuffer, PACKET_SIZE, 0);
                    }
                        break;
                    case PKT_C_MOVE:
                    {
                        BYTE SendBuffer[PACKET_SIZE] = "";

                        PacketHeader header;

                        header.protocol = PKT_S_MOVE;
                        header.size = PACKET_SIZE;

                        memcpy(SendBuffer, packet, PACKET_SIZE);

                        (*(PacketHeader*)SendBuffer).protocol = PKT_S_MOVE;

                        //BroadCast
                        {
                            lock_guard<mutex> lockguard(g_lock);
                            for (const Player& user: Clients)
                            {
                                if (user.socket != hClient)
                                {
                                    send(user.socket, (const char*)SendBuffer, PACKET_SIZE, 0);
                                }
                            }
                        }
                    }
                        break;
                    case PKT_C_EXIT:
                    {
                        BYTE packet[PACKET_SIZE] = "";

                        PacketHeader header;

                        header.protocol = PKT_S_EXIT;
                        header.size = PACKET_SIZE;
                        (*(PacketHeader*)packet) = header;

                        send(hClient, (const char*)packet, PACKET_SIZE, 0);
                        bThreadLoop = false;
                        cout << "클라이언트 종료" << endl;

                        {
                            lock_guard<mutex> lockguard(g_lock);
                            for (auto& Client : Clients)
                            {
                                /*if (Client == hClient)
                                {

                                }*/
                            }
                        }
                        break;
                    }
                    default:
                        break;
                    }
                }
            });
        //WorkerThread.detach();
        ClientThread.push_back(std::move(WorkerThread));
    }


    // if (collisionThread.joinable())
    //    collisionThread.join();
    for (thread& Client : ClientThread)
    {
        if (Client.joinable())
        {
            Client.join();
        }
    }

    closesocket(hListen);

    WSACleanup();

    return 0;
}
