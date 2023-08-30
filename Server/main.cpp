#include "DnfServer.h"
#include "ClientPacketHandler.h"
#include "CollisionThread.h"
using namespace std;

int main()
{
    ClientPacketHandler::Init();

    /*CollisionThread collisionThread;

    collisionThread.Init();

    collisionThread.Simulate();*/

    DnfServer server(NetAddress(L"127.0.0.1", PORT), 10);

    server.StartServer();

    return 0;
}
