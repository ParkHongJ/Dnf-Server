#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"
#include "../Default/ServerManager.h"
CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case M_Idle:
		IdleTick(fTimeDelta);
		break;
	case M_Chase:
		ChaseTick(fTimeDelta);
		break;
	case M_Attack:
		AttackTick(fTimeDelta);
		break;
	case M_Dead:
		DeadTick(fTimeDelta);
		break;
	case M_Hit:
		HitTick(fTimeDelta);
		break;
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CMonster::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	PlayAnimation(fTimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_bool		isDraw = pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);

	RELEASE_INSTANCE(CGameInstance);

	if (true == isDraw)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		m_pRendererCom->Add_DebugGroup(m_pColliderCom);
#endif // _DEBUG
	}

}

HRESULT CMonster::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", _uint(m_fFrame))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	RELEASE_INSTANCE(CGameInstance);

	RenderHP();
	return S_OK;
}

void CMonster::OnDamaged(CGameObject* attacker, float damage)
{
	fHp = damage;
}

void CMonster::AddColliderPacket()
{
	BYTE sendBuffer[PACKET_SIZE] = "";

	PacketHeader header;
	header.protocol = PKT_C_ADD_COLLIDER;
	header.size = PACKET_SIZE;

	*(PacketHeader*)sendBuffer = header;

	int bufferOffset = sizeof(PacketHeader);

	memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

	bufferOffset += sizeof(m_iId);

	Type type = Type::MONSTER	;

	memcpy(sendBuffer + bufferOffset, &type, sizeof(type));

	bufferOffset += sizeof(type);

	CCollider::COLLIDERDESC ColliderDesc = m_pColliderCom->GetColliderDesc();

	memcpy(sendBuffer + bufferOffset, &ColliderDesc.vSize, sizeof(ColliderDesc.vSize));

	bufferOffset += sizeof(ColliderDesc.vSize);

	memcpy(sendBuffer + bufferOffset, &ColliderDesc.vCenter, sizeof(ColliderDesc.vCenter));

	bufferOffset += sizeof(ColliderDesc.vCenter);

	ServerManager* ServerMgr = ServerManager::GetInstance();
	ServerMgr->Send(sendBuffer, PACKET_SIZE);
}

void CMonster::RenderHP()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pTransformHPCom->Set_Scale(XMVectorSet(0.148f, 0.01f, 1.f, 0.f));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformHPCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(vPos), XMVectorGetY(vPos) + 0.085f, 0.f, 1.f));

	float percentage2 = fHp / 100.f;
	m_pShaderCom->Set_RawValue("percentage2", &percentage2, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformHPCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));

	if (FAILED(pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return;

	if (FAILED(m_pTextureHPCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return;

	if (FAILED(m_pShaderCom->Begin(4)))
		return;

	if (FAILED(m_pVIBufferCom->Render()))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CMonster::UpdateMove(_float3 vPos, int NewState)
{
	_vector CurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//목적지가 나보다 왼쪽에 있을때
	if (vPos.x < XMVectorGetX(CurrentPos))
	{
		if (m_eDir == MONSTER_DIR::M_RIGHT)
		{
			//방향전환
			m_pTransformCom->ReverseScaleX();
			m_eDir = MONSTER_DIR::M_LEFT;
		}
	}
	else
	{
		//나보다 오른쪽에 있을때
		if (m_eDir == MONSTER_DIR::M_LEFT)
		{
			m_pTransformCom->ReverseScaleX();
			m_eDir = MONSTER_DIR::M_RIGHT;
		}
	}
	vDestPos = vPos;

	UpdateState((MONSTER_STATE)NewState);
	
}

void CMonster::IdleTick(_float fTimeDelta)
{
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMoveDir = XMLoadFloat3(&vDestPos) - vCurrentPos;

	if (XMVectorGetX(XMVector3Length(vMoveDir)) < fTimeDelta * fSpeed)
	{
		vCurrentPos = XMLoadFloat3(&vDestPos);
	}
	else
	{
		vCurrentPos += XMVector3Normalize(vMoveDir) * fTimeDelta * fSpeed;
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCurrentPos, 1.f));
}

void CMonster::ChaseTick(_float fTimeDelta)
{
}

void CMonster::AttackTick(_float fTimeDelta)
{
}

void CMonster::DeadTick(_float fTimeDelta)
{
	if (bAnimEnd)
	{
		ServerManager* ServerMgr = ServerManager::GetInstance();

		ServerMgr->DestroyObjectById(m_iId);
	}
}

void CMonster::HitTick(_float fTimeDelta)
{
}

void CMonster::PlayAnimation(_float fTimeDelta)
{
	_float AllFrame;
	float fSpeed = 1.f;
	switch (m_eState)
	{
	case M_Idle:
		AllFrame = 8.f;
		break;
	case M_Chase:
		AllFrame = 8.f;
		break;
	case M_Attack:
		AllFrame = 6.f;
		break;
	case M_Dead:
		AllFrame = 1.f;
		bAnimEnd = true;
		break;
	case M_Hit:
		AllFrame = 3.f;
		fSpeed = 2.f;
		break;
	}

	m_fFrame += AllFrame * fTimeDelta * fSpeed;
	switch (m_eState)
	{
	case M_Idle:
		if (m_fFrame >= 28.f)
		{
			m_fFrame = 21.f;
		}
		break;
	case M_Chase:
		if (m_fFrame >= 28.f)
		{
			m_fFrame = 21.f;
		}
		break;
	case M_Attack:
		if (m_fFrame >= 7.f)
		{
			m_fFrame = 7.f;
		}
		break;
	case M_Dead:
		m_fFrame = 11.f;
		break;
	case M_Hit:
		if (m_fFrame >= 10.f)
		{
			m_fFrame = 10.f;
		}
		break;
	}
}

void CMonster::UpdateState(MONSTER_STATE NewState)
{
	if (NewState == M_Hit)
		m_fFrame = 8.f;

	if (m_eState != NewState)
	{
		switch (NewState)
		{
		case M_Idle:
			m_fFrame = 21.f;
			break;
		case M_Chase:
			m_fFrame = 21.f;
			break;
		case M_Attack:
			m_fFrame = 2.f;
			break;
		case M_Dead:
			m_fFrame = 11.f;
			break;
		case M_Hit:
			m_fFrame = 8.f;
			break;
		}
		m_eState = NewState;
	}
}

HRESULT CMonster::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pTransformHPCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
		
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HP"), TEXT("Com_Texture2"), (CComponent**)&m_pTextureHPCom)))
		return E_FAIL;

	/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(0.3f, 0.3f, 0.05f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster*		pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonster::Clone(void * pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureHPCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformHPCom);
}
