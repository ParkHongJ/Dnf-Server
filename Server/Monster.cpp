#include "Monster.h"
#include "Room.h"
Monster::Monster()
{
	StartMoveTick = GetTickCount64();
}

void Monster::Update()
{
	currentMoveTick = GetTickCount64();
	if (currentMoveTick - StartMoveTick < 1000)
		return;

	StartMoveTick = currentMoveTick;

	switch (state)
	{
	case Idle:
		IdleTick();
		break;
	case Patrol:
		PatrolTick();
		break;
	case Chase:
		ChaseTick();
		break;
	case Attack:
		AttackTick();
		break;
	case Dead:
		DeadTick();
		break;
	}
}

void Monster::OnDamaged(Object* attacker, float Damage)
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
	else
	{
		UpdateState(STATE::Hit);
	}
}

void Monster::IdleTick()
{
}

void Monster::PatrolTick()
{
}

void Monster::ChaseTick()
{
}

void Monster::AttackTick()
{
}

void Monster::DeadTick()
{
}

void Monster::UpdateState(STATE newState)
{
}
