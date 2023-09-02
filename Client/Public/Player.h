#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CCollider;
class CVIBuffer_Rect;
class CPipeLine;
END

/*
	IDLE = 3,
	WALK = 8,
	RUN = 6,
	SKILL_1 = 3, //1타
	SKILL_2 = 4, //2타
	SKILL_3 = 8, //3타
	SKILL_4 = 7, //4타
	
	SKILL_A = 9, //공참타
	SKILL_S = 13, //칼던지기
	SKILL_D = 12, //고저스
	SKILL_F = 3, //더킹
	SKILL_G = 3, //스웨이
	SKILL_H = 9, //승천진
*/
enum DIR 
{
	LEFT,
	RIGHT
};

enum STATE
{
	IDLE = 0,
	WALK,
	RUN,
	SKILL_1, //1타
	SKILL_2, //2타
	SKILL_3, //3타
	SKILL_4, //4타
	
	SKILL_A, //공참타
	SKILL_S, //칼던지기
	SKILL_D, //고저스
	SKILL_F, //더킹
	SKILL_G, //스웨이
	SKILL_H, //승천진


	HIT,
	Dead
};
enum SKILLID
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
enum ANIMATION
{
	ANIM_IDLE = 0,
	ANIM_WALK = 64,
	ANIM_RUN = 72,
	ANIM_SKILL_1 = 8,
	ANIM_SKILL_2 = 11,
	ANIM_SKILL_3 = 15,
	ANIM_SKILL_4 = 23,

	ANIM_SKILL_A = 30,
	ANIM_SKILL_S = 39,
	ANIM_SKILL_D = 52,
	ANIM_SKILL_F = 131,
	ANIM_SKILL_G = 134,
	ANIM_SKILL_H = 144,

	ANIM_HIT = 96
};
BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnDamaged(CGameObject* attacker, float fDamage) override;
public:
	void AddColliderPacket();
	void SetLocallyControlled(_bool LocallyControlled) { bLocallyControlled = LocallyControlled; };
	void SendMovementPacket();
	void SetDestination(_fvector vPos, int State, int Dir);
	void PlayAnimation(_float fTimeDelta);

	void IdleTick(_float fTimeDelta);
	void WalkTick(_float fTimeDelta);
	void RunTick(_float fTimeDelta);
	void HitTick(_float fTimeDelta);

	void Skill_1Tick(_float fTimeDelta);

	void Skill_2Tick(_float fTimeDelta);
	void Skill_3Tick(_float fTimeDelta);
	void Skill_4Tick(_float fTimeDelta);
	
	void Skill_ATick(_float fTimeDelta);
	void Skill_STick(_float fTimeDelta);
	void Skill_DTick(_float fTimeDelta);
	void Skill_FTick(_float fTimeDelta);
	void Skill_GTick(_float fTimeDelta);
	void Skill_HTick(_float fTimeDelta);


	/*
	* SKILL_1, //1타
	SKILL_2, //2타
	SKILL_3, //3타
	SKILL_4, //4타
	
	SKILL_A, //공참타
	SKILL_S, //칼던지기
	SKILL_D, //고저스
	SKILL_F, //더킹
	SKILL_G, //스웨이
	SKILL_H, //승천진
	*/
	void UpdateState(STATE NewState);

	void ActivateSkill(int Skillid, _float3 vPos);
	void CloneHPRender();
private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CTransform* m_pTransformHPCom = nullptr;
	CCollider* m_pColliderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTexture* m_pTextureHPCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_float4x4 m_ViewMatrix, m_ProjMatrix;
	_bool bLocallyControlled = false;

	// 주기적으로 패킷을 전송
	_float elapsedTime = 0.0f;
	_float packetSendInterval = 0.2f; // 0.2초에 한 번 패킷을 전송

	DIR m_eDir = DIR::RIGHT;

	_float3 vDestPos;
	_bool bAutoRunning = false;

	_float3 vBeforePos;

	STATE m_eState = IDLE;
	
	_float m_fFrame = 180.f;

	_float fHp = 100.f;
	_bool bAnimEnd = false;
private:
	HRESULT Ready_Components();
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END