#pragma once
class Collider;
enum ESkill
{
	ID_SKILL_1 = 0, //1Ÿ
	ID_SKILL_2, //2Ÿ
	ID_SKILL_3, //3Ÿ
	ID_SKILL_4, //4Ÿ

	ID_SKILL_A, //����Ÿ
	ID_SKILL_S, //Į������
	ID_SKILL_D, //������
	ID_SKILL_F, //��ŷ
	ID_SKILL_G, //������
	ID_SKILL_H, //��õ��
};

class Skill
{

public:
	int playerId;
	int SkillId;
	
	//��ġ
	float vPos[3];

	//����
	Collider* collider;
};

