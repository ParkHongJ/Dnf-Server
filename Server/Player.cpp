#include "Player.h"
#include "Session.h"
#include "Room.h"
Player::Player()
{
	stat.maxhp = 100.f;
	stat.hp = 100.f;
}

Player::~Player()
{
	ownerSession = nullptr;
}

void Player::OnDamaged(Object* attacker, float Damage)
{
	cout << "OnDamaged : " << Damage << endl;
	
	stat.hp -= Damage;

	BYTE sendBuffer[PACKET_SIZE] = "";

	PacketHeader header;
	header.protocol = PKT_S_CHANGE_HP;
	header.size = PACKET_SIZE;

	*(PacketHeader*)sendBuffer = header;

	int bufferOffset = sizeof(PacketHeader);

	memcpy(sendBuffer + bufferOffset, &ObjectId, sizeof(ObjectId));
	
	//Object ID
	bufferOffset += sizeof(ObjectId);

	memcpy(sendBuffer + bufferOffset, &stat.hp, sizeof(stat.hp));

	bufferOffset += sizeof(stat.hp);

	GRoom.Broadcast(sendBuffer);

	if (stat.hp <= 0.f)
	{
		stat.hp = 0.f;
		OnDead(attacker);
	}
}

void Player::OnDead(Object* attacker)
{
	cout << "»ç¸Á" << endl;
}
