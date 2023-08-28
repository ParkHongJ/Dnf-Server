#include <iostream>
#include <winsock2.h>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <list>
#include <unordered_map>
#define PACKET_SIZE 512
#define PORT 7778

using namespace std;

struct Player {
    SOCKET socket;
    unsigned int PlayerId;
    float x, y, z;

    float vRight[3];
    float vUp[3];
    float vLook[3];

    float vPos[3];
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

enum COLLISIONGROUP
{
    COLLISION_PLAYER,
    COLLISION_MONSTER, //임시
    COLLISION_GROUP_MAX
};

list<Collider> CollisionObjects[COLLISION_GROUP_MAX];

union COLLIDER_ID
{
    struct {
        unsigned int Left_ID;
        unsigned int Right_ID;
    };
    LONGLONG ID;
};

struct Collider {
    
    //주인의 ID
    int OwnerId;
    //자신의 ID
    int ColliderId;

    //누구랑 충돌이 가능한지?
    list<COLLISIONGROUP> OtherGroup;
};
unordered_map<LONGLONG, bool> m_ColInfo;
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
    thread collisionThread([=]() {
        while (true)
        {
            //충돌검사
            unordered_map<LONGLONG, bool>::iterator iter;

            for (size_t SourceGroup = COLLISION_PLAYER; SourceGroup < COLLISION_GROUP_MAX; SourceGroup++)
            {
                for (size_t DestGroup = COLLISION_PLAYER; DestGroup < COLLISION_GROUP_MAX; DestGroup++)
                {
                    if (SourceGroup == DestGroup)
                        continue;

                    for (Collider& Sour : CollisionObjects[SourceGroup])
                    {
                        for (Collider& Dest : CollisionObjects[DestGroup])
                        {
                            if (Dest.ColliderId == Sour.ColliderId)
                                continue;

                            COLLIDER_ID ID;
                            ID.Left_ID = Sour.ColliderId;
                            ID.Right_ID = Dest.ColliderId;
                            iter = m_ColInfo.find(ID.ID);

                            if (m_ColInfo.end() == iter)
                            {
                                m_ColInfo.insert(make_pair(ID.ID, false));
                                iter = m_ColInfo.find(ID.ID);
                            }

                            //if (pSour.first->GetOwner()->GetEnabled() && pDest.first->GetOwner()->GetEnabled())
                            //{
                                //if (Check_SphereEx(pSour.first, pSour.second, pDest.first, pDest.second))
                                //{
                                //    if (iter->second)
                                //    {
                                //        //pSour.first->GetOwner()->OnTriggerStay(pDest.first->GetOwner(), fTimeDelta, DIR_END);
                                //        //pDest.first->GetOwner()->OnTriggerStay(pSour.first->GetOwner(), fTimeDelta, DIR_END);
                                //    }
                                //    else
                                //    {
                                //        //pSour.first->GetOwner()->OnTriggerEnter(pDest.first->GetOwner(), fTimeDelta);
                                //        //pDest.first->GetOwner()->OnTriggerEnter(pSour.first->GetOwner(), fTimeDelta);
                                //        iter->second = true;
                                //    }
                                //}
                                //else
                                //{
                                //    if (iter->second)
                                //    {
                                //        //pSour.first->GetOwner()->OnTriggerExit(pDest.first->GetOwner(), fTimeDelta);
                                //        //pDest.first->GetOwner()->OnTriggerExit(pSour.first->GetOwner(), fTimeDelta);
                                //        iter->second = false;
                                //    }
                                //}
                            //}
                        }
                    }
                }
            }
        }
        });

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

                        bThreadLoop = false;
                        cout << "클라이언트 종료" << endl;

                        int id = 0;

                        int iIndex = 0;
                        {
                            lock_guard<mutex> lockguard(g_lock);
                            for (auto& user : Clients)
                            {
                                if (user.socket == hClient)
                                {
                                    id = user.PlayerId;
                                    break;
                                }
                                iIndex++;
                            }
                        }

                        memcpy(packet + sizeof(PacketHeader), &id, sizeof(id));

                        {
                            lock_guard<mutex> lockguard(g_lock);
                            for (auto& user : Clients)
                            {
                                if (user.socket != hClient)
                                {
                                    send(user.socket, (const char*)packet, PACKET_SIZE, 0);
                                }
                            }
                            Clients.erase(Clients.begin() + iIndex);
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
