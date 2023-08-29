#include "Room.h"
#include "Player.h"
#include "Session.h"

Room GRoom;

void Room::Enter(Player* player)
{
	lock_guard<mutex> lockguard(lock);
	_players[player->PlayerId] = player;
}

void Room::Leave(Player* player)
{
	lock_guard<mutex> lockguard(lock);
	_players.erase(player->PlayerId);
}

void Room::Broadcast(BYTE* sendBuffer)
{
	lock_guard<mutex> lockguard(lock);
	for (auto& player : _players)
	{
		player.second->ownerSession->Send(sendBuffer);
	}
}

void Room::BroadcastWithOutMe(int playerId, BYTE* sendBuffer)
{
	//자신을 제외한 브로드캐스트
	lock_guard<mutex> lockguard(lock);
	for (auto& player : _players)
	{
		if (player.first != playerId)
		{
			player.second->ownerSession->Send(sendBuffer);
		}
	}
}

void Room::SendRoomInfo(int playerId, BYTE* sendBuffer, int bufferOffset)
{
	{
		lock_guard<mutex> lockguard(lock);
		unsigned int PlayerCount = _players.size() - 1;

		memcpy(sendBuffer + bufferOffset, &PlayerCount, sizeof(PlayerCount));

		bufferOffset += sizeof(PlayerCount);

		for (auto& player : _players)
		{
			if (player.second->PlayerId != playerId)
			{
				int id = player.second->PlayerId;

				memcpy(sendBuffer + bufferOffset, &id, sizeof(id));
				bufferOffset += sizeof(id);
			}
		}
		_players[playerId]->ownerSession->Send(sendBuffer);
	}
}
