#pragma once
class Collider;
enum ESkill
{
	ID_SKILL_1 = 0, //1타
	ID_SKILL_2, //2타
	ID_SKILL_3, //3타
	ID_SKILL_4, //4타

	ID_SKILL_A, //공참타
	ID_SKILL_S, //칼던지기
	ID_SKILL_D, //고저스
	ID_SKILL_F, //더킹
	ID_SKILL_G, //스웨이
	ID_SKILL_H, //승천진
};

class Skill
{

public:
	int playerId;
	int SkillId;
	
	//위치
	float vPos[3];

	//범위
	Collider* collider;
};

