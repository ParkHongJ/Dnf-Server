#include "ServerManager.h"
#include <thread>
ServerManager::ServerManager()
{
	//Connect();
}

ServerManager::~ServerManager()
{
}

//void ServerManager::Connect()
//{
//    WSADATA wsaData;
//    SOCKADDR_IN serverAddress;
//
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // Winsock���ʱ�ȭ�մϴ�.
//    {
//        // ShowErrorMessage("WSAStartup()");
//    }
//   
//    clientSocket = socket(PF_INET, SOCK_STREAM, 0); // TCP �����������մϴ�.
//   
//    if (clientSocket == INVALID_SOCKET)
//    {
//        //ShowErrorMessage("socket()");
//    }
//
//    memset(&serverAddress, 0, sizeof(serverAddress));
//    serverAddress.sin_family = AF_INET;
//    //serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // ���ڿ�IP����Ʈ��ũ����Ʈ��������
//    serverAddress.sin_port = htons(PORT); // 2����Ʈ������Ʈ��ũ����Ʈ��������
//
//    if (connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
//    {
//        //ShowErrorMessage("connect()");
//    }
//
//    
//    //thread WorkerThread = thread([=, &g_ThreadLoop]()
//    //    {
//    //        while (true)
//    //        {
//    //            if (!g_ThreadLoop)
//    //                break;
//
//    //            BYTE packet[PACKET_SIZE] = "";
//    //            int length = recv(clientSocket, (char*)packet, sizeof(packet), 0);
//
//    //            //PacketHandler::HandlePacket(clientSocket, packet, length);
//    //        }
//    //    });
//}
//
//void ServerManager::RecvThread(SOCKET Socket)
//{
//    while (true)
//    {
//        BYTE packet[PACKET_SIZE] = "";
//        int length = recv(Socket, (char*)packet, sizeof(packet), 0);

        //PacketHandler::HandlePacket(clientSocket, packet, length);
//        Packets.push(packet);
//    }
//}
