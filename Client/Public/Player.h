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

enum DIR 
{
	LEFT,
	RIGHT
};

enum STATE
{
	IDLE = 4,
	WALK = 8,
	RUN,
	SKILL,
	HIT
};
enum ANIMATION
{
	ANIM_IDLE = 176,
	ANIM_WALK = 180,
	ANIM_RUN,
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
	void SkillTick(_float fTimeDelta);
	void HitTick(_float fTimeDelta);


	void UpdateState(STATE NewState);

	void ActivateSkill(int id, _float3 vPos);
private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CCollider* m_pColliderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
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
private:
	HRESULT Ready_Components();
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END