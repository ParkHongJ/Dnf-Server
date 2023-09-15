#include "Monster.h"
#include "Room.h"
Monster::Monster()
{
	StartMoveTick = GetTickCount64();
}

void Monster::Update()
{
	switch (state)
	{
	case Idle:
		IdleTick();
		break;
	case Chase:
		ChaseTick();
		break;
	case Attack:
		AttackTick();
		break;
	case Hit:
		HitTick();
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
		cout << "몬스터 사망" << endl;
		stat.hp = 0.f;
		OnDead(attacker);
	}
	else
	{
		UpdateState(STATE::Hit);
	}
}

void Monster::OnDead(Object* attacker)
{
	UpdateState(STATE::Dead);
	GRoom.Leave(this);
}

void Monster::Initialzie()
{
	StartPos[0] = x;
	StartPos[1] = y;
	StartPos[2] = z;
}

void Monster::IdleTick()
{
	//왼쪽갔다 대기하고 오른쪽 가기
	currentMoveTick = GetTickCount64();
	if (currentMoveTick - StartMoveTick < 1000 )
		return;

	StartMoveTick = currentMoveTick;
	if (bLeft)
	{
		float destX = StartPos[0] - 0.2f;

		if (fabs(destX - (x - fSpeed)) <= 0.05f)
		{
			x = destX;
			bLeft = false;
		}
		else
		{
			x -= fSpeed;
		}
	}
	else
	{
		float destX = StartPos[0] + 0.2f;
		if (fabs(destX - (x + fSpeed)) <= 0.05f)
		{
			x = destX;
			bLeft = true;
		}
		else
		{
			x += fSpeed;
		}
	}

	//플레이어의 위치를 받아와서 비교한후 Attack으로 전환

	BYTE sendBuffer[PACKET_SIZE] = "";
	
	PacketHeader header;
	header.protocol = PKT_S_MONSTER_CONTROL;
	header.size = PACKET_SIZE;

	*(PacketHeader*)sendBuffer = header;	

	int bufferOffset = sizeof(PacketHeader);
	memcpy(sendBuffer + bufferOffset, &ObjectId, sizeof(ObjectId));
	bufferOffset += sizeof(ObjectId);

	float vPos[3];
	vPos[0] = x;
	vPos[1] = y;
	vPos[2] = z;

	memcpy(sendBuffer + bufferOffset, &vPos, sizeof(float) * 3);
	bufferOffset += sizeof(float) * 3;

	memcpy(sendBuffer + bufferOffset, &state, sizeof(state));
	bufferOffset += sizeof(state);

	GRoom.Broadcast(sendBuffer);
}

void Monster::ChaseTick()
{
	//플레이어가 일정거리에 왔다면 추적
}

void Monster::AttackTick()
{
	//플레이어가 일정거리에 왔다면 공격
}

void Monster::DeadTick()
{
}

void Monster::HitTick()
{
	currentMoveTick = GetTickCount64();
	if (currentMoveTick - StartMoveTick < 2000)
		return;

	StartMoveTick = currentMoveTick;
	UpdateState(STATE::Idle);
}

void Monster::UpdateState(STATE newState)
{
	//새로 바뀌는 상태가
	switch (newState)
	{
	case Idle:
		break;
	case Chase:
		break;
	case Attack:
		break;
	case Dead:
		break;
	case Hit:
		break;
	case End:
		break;
	default:
		break;
	}
	state = newState;

	//Send
	
	BYTE sendBuffer[PACKET_SIZE] = "";

	PacketHeader header;
	header.protocol = PKT_S_MONSTER_CONTROL;
	header.size = PACKET_SIZE;

	*(PacketHeader*)sendBuffer = header;

	int bufferOffset = sizeof(PacketHeader);
	memcpy(sendBuffer + bufferOffset, &ObjectId, sizeof(ObjectId));
	bufferOffset += sizeof(ObjectId);

	float vPos[3];
	vPos[0] = x;
	vPos[1] = y;
	vPos[2] = z;

	memcpy(sendBuffer + bufferOffset, &vPos, sizeof(float) * 3);
	bufferOffset += sizeof(float) * 3;

	memcpy(sendBuffer + bufferOffset, &state, sizeof(state));
	bufferOffset += sizeof(state);

	GRoom.Broadcast(sendBuffer);
}
