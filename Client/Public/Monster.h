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

enum MONSTER_DIR
{
	M_LEFT,
	M_RIGHT
};
enum MONSTER_STATE
{
	M_Idle = 0,
	M_Chase,
	M_Attack,
	M_Dead,
	M_Hit,
	M_End
};
BEGIN(Client)
class CMonster final : public CGameObject
{
private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnDamaged(CGameObject* attacker, float damage) override;
	void AddColliderPacket();
	void RenderHP();
	void UpdateMove(_float3 vPos, int NewState);

	void IdleTick(_float fTimeDelta);
	void ChaseTick(_float fTimeDelta);
	void AttackTick(_float fTimeDelta);
	void DeadTick(_float fTimeDelta);
	void HitTick(_float fTimeDelta);
	void PlayAnimation(_float fTimeDelta);
	void UpdateState(MONSTER_STATE NewState);
private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTexture* m_pTextureHPCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CCollider* m_pColliderCom = nullptr;
	CTransform* m_pTransformHPCom = nullptr;

private:
	HRESULT Ready_Components();

	_float fHp = 100.f;
	_float m_fFrame = 21.f;
	MONSTER_DIR m_eDir = MONSTER_DIR::M_RIGHT;
	MONSTER_STATE m_eState = MONSTER_STATE::M_Idle;
	_float3 vDestPos;
	_float fSpeed = 0.05f;
	_bool bAnimEnd = false;
public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END