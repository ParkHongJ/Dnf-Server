#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "../Default/ServerManager.h"
CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;	

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case IDLE:
		IdleTick(fTimeDelta);
		break;
	case WALK:
		WalkTick(fTimeDelta);
		break;
	case RUN:
		break;
	default:
		break;
	}

	HWND focusedWindow = GetFocus();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (bLocallyControlled && focusedWindow)
	{
		SendMovementPacket(); // 패킷을 전송하는 함수를 호출

		/* 예측 패킷보내기 */
		//elapsedTime += fTimeDelta;

		//if (elapsedTime >= packetSendInterval)
		//{
		//	// 패킷을 생성하고 서버로 전송하는 코드를 여기에 추가
		//	// 예를 들어, 이동 정보를 담은 패킷을 만들고 서버로 전송하는 함수를 호출

		//	//SendMovementPacket(); // 패킷을 전송하는 함수를 호출
		//	elapsedTime = 0.0f; // 타이머 초기화
		//}
	}

	/*if (bAutoRunning)
	{
		_vector vDir = XMLoadFloat3(&vDestPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vDir = XMVector3Normalize(vDir);

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vDir * fTimeDelta;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));

		if (10.f > XMVectorGetX(XMVector3Length(vPos - XMLoadFloat3(&vDestPos))))
		{
			bAutoRunning = false;
		}
	}*/
}

void CPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	/*m_pModelCom->Play_Animation(fTimeDelta);

	for (auto& pPart : m_Parts)
		pPart->LateTick(fTimeDelta);

	for (auto& pPart : m_Parts)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, pPart);
	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	for (_uint i = 0; i < COLLILDERTYPE_END; ++i)
	{
		if (nullptr != m_pColliderCom[i])
			m_pRendererCom->Add_DebugGroup(m_pColliderCom[i]);
	}

	m_pRendererCom->Add_DebugGroup(m_pNavigationCom);*/
	PlayAnimation(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
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

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", _uint(m_fFrame))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

#ifdef _DEBUG
	/*for (_uint i = 0; i < COLLILDERTYPE_END; ++i)
	{
		if(nullptr != m_pColliderCom[i])
			m_pColliderCom[i]->Render();
	}

	m_pNavigationCom->Render();*/
#endif

	return S_OK;
}

void CPlayer::SendMovementPacket()
{
	ServerManager* ServerMgr = ServerManager::GetInstance();

	BYTE packet[PACKET_SIZE] = "";

	_int packetOffset = sizeof(PacketHeader);

	PacketHeader header;
	header.protocol = PKT_C_MOVE;
	header.size = PACKET_SIZE;

	*(PacketHeader*)packet = header;

	memcpy(packet + packetOffset, &m_iId, sizeof(m_iId));

	packetOffset += sizeof(m_iId);

	memcpy(packet + packetOffset, &m_eState, sizeof(m_eState));
	
	packetOffset += sizeof(m_eState);

	memcpy(packet + packetOffset, &m_eDir, sizeof(m_eDir));

	packetOffset += sizeof(m_eDir);

	_float3 vPos;

	//_vector vDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat3(&vBeforePos));
	
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) /*+ vDir * 20.f*/);

	memcpy(packet + packetOffset, &vPos, sizeof(vPos));

	ServerMgr->Send(packet, header.size);
}

void CPlayer::SetDestination(_fvector vPos, int State, int Dir)
{
	UpdateState(STATE(State));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));

	if (m_eDir != Dir)
	{
		m_pTransformCom->ReverseScaleX();
		m_eDir = (DIR)Dir;
	}
}

void CPlayer::PlayAnimation(_float fTimeDelta)
{
	m_fFrame += _float(m_eState) * fTimeDelta;

	switch (m_eState)
	{
	case IDLE:
		if (m_fFrame >= 179.f)
		{
			m_fFrame = 176.f;
		}
		break;
	case WALK:
		if (m_fFrame >= 187.f)
		{
			m_fFrame = 180.f;
		}
		break;
	case RUN:
		break;
	default:
		break;
	}
}

void CPlayer::IdleTick(_float fTimeDelta)
{
	HWND focusedWindow = GetFocus();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (bLocallyControlled && focusedWindow)
	{
		if (pGameInstance->Get_DIKState(DIK_DOWN) || pGameInstance->Get_DIKState(DIK_UP) || pGameInstance->Get_DIKState(DIK_LEFT) || pGameInstance->Get_DIKState(DIK_RIGHT))
		{
			UpdateState(STATE::WALK);
		}
	}
}

void CPlayer::WalkTick(_float fTimeDelta)
{
	HWND focusedWindow = GetFocus();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (bLocallyControlled && focusedWindow)
	{
		if (pGameInstance->Get_DIKState(DIK_DOWN))
		{
			//m_pTransformCom->Go_Backward(fTimeDelta);
			//m_pTransformCom->Go_Right
		}
		else if (pGameInstance->Get_DIKState(DIK_UP))
		{
		}
		else if (pGameInstance->Get_DIKState(DIK_LEFT))
		{
			if (m_eDir != DIR::LEFT)
			{
				m_pTransformCom->ReverseScaleX();
				m_eDir = DIR::LEFT;
			}
			m_pTransformCom->Go_Left(fTimeDelta);

			XMStoreFloat3(&vBeforePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
		else if (pGameInstance->Get_DIKState(DIK_RIGHT))
		{
			if (m_eDir != DIR::RIGHT)
			{
				m_pTransformCom->ReverseScaleX();
				m_eDir = DIR::RIGHT;
			}
			m_pTransformCom->Go_Right(fTimeDelta);

			XMStoreFloat3(&vBeforePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
		else
		{
			UpdateState(STATE::IDLE);
		}
	}
}

void CPlayer::RunTick(_float fTimeDelta)
{
	HWND focusedWindow = GetFocus();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (bLocallyControlled && focusedWindow)
	{
		if (pGameInstance->Get_DIKState(DIK_DOWN))
		{
			
		}
		else if (pGameInstance->Get_DIKState(DIK_UP))
		{
		}
		else if (pGameInstance->Get_DIKState(DIK_LEFT))
		{
			if (m_eDir != DIR::LEFT)
			{
				m_pTransformCom->ReverseScaleX();
				m_eDir = DIR::LEFT;
			}
			m_pTransformCom->Go_Left(fTimeDelta);

			XMStoreFloat3(&vBeforePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
		else if (pGameInstance->Get_DIKState(DIK_RIGHT))
		{
			if (m_eDir != DIR::RIGHT)
			{
				m_pTransformCom->ReverseScaleX();
				m_eDir = DIR::RIGHT;
			}
			m_pTransformCom->Go_Right(fTimeDelta);

			XMStoreFloat3(&vBeforePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
		else
		{
			UpdateState(STATE::IDLE);
		}
	}
}

void CPlayer::UpdateState(STATE NewState)
{
	if (m_eState != NewState)
	{
		switch (NewState)
		{
		case IDLE:
			m_fFrame = _float(ANIM_IDLE);
			break;
		case WALK:
			m_fFrame = _float(ANIM_WALK);
			break;
		case RUN:
			break;
		default:
			break;
		}
		m_eState = NewState;
	}
}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
