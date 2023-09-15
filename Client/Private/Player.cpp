#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "../Default/ServerManager.h"
#include "Effect.h"
#include "CUIManager.h"
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

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

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
	case SKILL_1:
		Skill_1Tick(fTimeDelta);
		break;
	case SKILL_2:
		Skill_2Tick(fTimeDelta);
		break;
	case SKILL_3:
		Skill_3Tick(fTimeDelta);
		break;
	case SKILL_4:
		Skill_4Tick(fTimeDelta);
		break;
	case SKILL_A:
		Skill_ATick(fTimeDelta);
		break;
	case SKILL_S:
		Skill_STick(fTimeDelta);
		break;
	case SKILL_D:
		Skill_DTick(fTimeDelta);
		break;
	case SKILL_F:
		Skill_FTick(fTimeDelta);
		break;
	case SKILL_G:
		Skill_GTick(fTimeDelta);
		break;
	case SKILL_H:
		Skill_HTick(fTimeDelta);
		break;
	case HIT:
		break;
	case Dead:
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

	PlayAnimation(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_DebugGroup(m_pColliderCom);
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

	CloneHPRender();

	return S_OK;
}

void CPlayer::OnDamaged(CGameObject* attacker, float fDamage)
{
	if (bLocallyControlled)
	{
		CUIManager::Instance->SetHP(fDamage);
	}
	else
	{
		//다른 hp갱신
		fHp = fDamage;
	}
}

void CPlayer::AddColliderPacket()
{
	BYTE sendBuffer[PACKET_SIZE] = "";

	PacketHeader header;
	header.protocol = PKT_C_ADD_COLLIDER;
	header.size = PACKET_SIZE;

	*(PacketHeader*)sendBuffer = header;

	int bufferOffset = sizeof(PacketHeader);

	memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

	bufferOffset += sizeof(m_iId);

	Type type = Type::PLAYER;

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
	_float AllFrame;
	float fSpeed = 1.f;
	switch (m_eState)
	{
	case IDLE:
		AllFrame = 3.f;
		break;
	case WALK:
		AllFrame = 8.f;
		break;
	case RUN:
		AllFrame = 6.f;
		break;
	case SKILL_1:
		AllFrame = 3.f;
		fSpeed = 1.5f;
		break;
	case SKILL_2:
		AllFrame = 4.f;
		fSpeed = 1.5f;
		break;
	case SKILL_3:
		AllFrame = 8.f;
		fSpeed = 1.5f;
		break;
	case SKILL_4:
		AllFrame = 7.f;
		fSpeed = 1.5f;
		break;
	case SKILL_A:
		AllFrame = 9.f;
		fSpeed = 1.5f;
		break;
	case SKILL_S:
		AllFrame = 13.f;
		break;
	case SKILL_D:
		AllFrame = 12.f;
		break;
	case SKILL_F:
		AllFrame = 3.f;
		fSpeed = 2.5f;
		break;
	case SKILL_G:
		AllFrame = 3.f;
		fSpeed = 2.5f;
		break;
	case SKILL_H:
		AllFrame = 9.f;
		break;
	case HIT:
		break;
	case Dead:
		break;
	default:
		break;
	}

	m_fFrame += AllFrame * fTimeDelta * fSpeed;
	switch (m_eState)
	{
	case IDLE:
		if (m_fFrame >= 3.f)
		{
			m_fFrame = 0.f;
		}
		break;
	case WALK:
		if (m_fFrame >= 71.f)
		{
			m_fFrame = 64.f;
		}
		break;
	case RUN:
		if (m_fFrame >= 77.f)
		{
			m_fFrame = 72.f;
		}
		break;
	case SKILL_1:
		if (m_fFrame >= 10.f)
		{
			m_fFrame = 10.f;
			bAnimEnd = true;
		}
		break;
	case SKILL_2:
		if (m_fFrame >= 14.f)
		{
			m_fFrame = 14.f;
			bAnimEnd = true;
		}
		break;
	case SKILL_3:
		if (m_fFrame >= 22.f)
		{
			m_fFrame = 22.f;
			bAnimEnd = true;
		}
		break;
	case SKILL_4:
		if (m_fFrame >= 29.f)
		{
			m_fFrame = 29.f;
			bAnimEnd = true;
		}
		break;
	case SKILL_A:
		if (m_fFrame >= 38.f)
		{
			m_fFrame = 38.f;
			bAnimEnd = true;
		}
		break;
	case SKILL_S:
		if (m_fFrame >= 51.f)
		{
			m_fFrame = 51.f;
			bAnimEnd = true;
		}
		break;
	case SKILL_D:
		if (m_fFrame >= 63.f)
		{
			m_fFrame = 63.f;
			bAnimEnd = true;
		}
		break;
	case SKILL_F:
		if (m_fFrame >= 133.f)
		{
			m_fFrame = 133.f;
			bAnimEnd = true;
		}
		break;
	case SKILL_G:
		if (m_fFrame >= 136.f)
		{
			m_fFrame = 136.f;
			bAnimEnd = true;
		}
		break;
	case SKILL_H:
		if (m_fFrame >= 152.f)
		{
			m_fFrame = 152.f;
			bAnimEnd = true;
		}
		break;
	case HIT:
		break;
	case Dead:
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
		else if (pGameInstance->GetDIKDownState(DIK_X))
		{
			//스킬 요청
			//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_1;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
		}
		//키를 눌렀으면 2번
		else if (pGameInstance->GetDIKDownState(DIK_A))
		{
			//스킬 요청
			//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_A;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
			return;
		}
		//키를 눌렀으면 2번
		else if (pGameInstance->GetDIKDownState(DIK_S))
		{
			//스킬 요청
				//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_S;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
			return;
		}
		else if (pGameInstance->GetDIKDownState(DIK_D))
		{
			//스킬 요청
				//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_D;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
			return;
		}
		//키를 눌렀으면 2번
		else if (pGameInstance->GetDIKDownState(DIK_F))
		{
		//스킬 요청
			//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_F;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
			return;
		}
		else if (pGameInstance->GetDIKDownState(DIK_G))
		{
		//스킬 요청
			//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_G;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
			return;
		}
		//키를 눌렀으면 2번
		else if (pGameInstance->GetDIKDownState(DIK_H))
		{
			//스킬 요청
				//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_H;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
			return;
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
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float AddY = XMVectorGetY(vPos) - fTimeDelta;
			_float AddZ = XMVectorGetZ(vPos) - fTimeDelta;
			vPos = XMVectorSetY(vPos, AddY);
			vPos = XMVectorSetZ(vPos, AddZ);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
		}
		else if (pGameInstance->Get_DIKState(DIK_UP))
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float AddY = XMVectorGetY(vPos) + fTimeDelta;
			_float AddZ = XMVectorGetZ(vPos) + fTimeDelta;
			vPos = XMVectorSetY(vPos, AddY);
			vPos = XMVectorSetZ(vPos, AddZ);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));
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

void CPlayer::HitTick(_float fTimeDelta)
{
	UpdateState(STATE::IDLE);
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
			m_fFrame = _float(ANIM_RUN);
			break;
		case SKILL_1:
			m_fFrame = _float(ANIM_SKILL_1);
			break;
		case SKILL_2:
			m_fFrame = _float(ANIM_SKILL_2);
			break;
		case SKILL_3:
			m_fFrame = _float(ANIM_SKILL_3);
			break;
		case SKILL_4:
			m_fFrame = _float(ANIM_SKILL_4);
			break;
		case SKILL_A:
			m_fFrame = _float(ANIM_SKILL_A);
			break;
		case SKILL_S:
			m_fFrame = _float(ANIM_SKILL_S);
			break;
		case SKILL_D:
			m_fFrame = _float(ANIM_SKILL_D);
			break;
		case SKILL_F:
			m_fFrame = _float(ANIM_SKILL_F);
			break;
		case SKILL_G:
			m_fFrame = _float(ANIM_SKILL_G);
			break;
		case SKILL_H:
			m_fFrame = _float(ANIM_SKILL_H);
			break;
		case HIT:
			break;
		case Dead:
			break;
		}
		m_eState = NewState;
		bAnimEnd = false;
	}
}

void CPlayer::ActivateSkill(int skillid, _float3 vPos)
{
	SKILLID skillId = (SKILLID)skillid;

	STATE NewState;
	switch (skillId)
	{
	case ID_SKILL_1:
		NewState = SKILL_1;
		break;
	case ID_SKILL_2:
		NewState = SKILL_2;
		break;
	case ID_SKILL_3:
		NewState = SKILL_3;
		break;
	case ID_SKILL_4:
		NewState = SKILL_4;
		break;
	case ID_SKILL_A:
		NewState = SKILL_A;
		break;
	case ID_SKILL_S:
		NewState = SKILL_S;
		break;
	case ID_SKILL_D:
		NewState = SKILL_D;
		break;
	case ID_SKILL_F:
	{
		NewState = SKILL_F;
		XMStoreFloat3(&vDestSkillPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		XMStoreFloat3(&vTempPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (m_eDir == RIGHT)
		{
			vDestSkillPos.x += 0.2f;
		}
		else
		{
			vDestSkillPos.x -= 0.2f;
		}
		
	}
		break;
	case ID_SKILL_G:
	{
		NewState = SKILL_G;
		XMStoreFloat3(&vDestSkillPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		XMStoreFloat3(&vTempPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (m_eDir == RIGHT)
		{
			vDestSkillPos.x -= 0.2f;
		}
		else
		{
			vDestSkillPos.x += 0.2f;
		}
	}
		break;
	case ID_SKILL_H:
		NewState = SKILL_H;
		break;
	}

	UpdateState(NewState);
}

void CPlayer::CloneHPRender()
{
	if (!bLocallyControlled)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		m_pTransformHPCom->Set_Scale(XMVectorSet(0.148f, 0.01f, 1.f, 0.f));

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_pTransformHPCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(vPos), XMVectorGetY(vPos) + 0.09f, 0.f, 1.f));

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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pTransformHPCom, &TransformDesc)))
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
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HP"), TEXT("Com_Texture2"), (CComponent**)&m_pTextureHPCom)))
		return E_FAIL;

	/* For.Com_AABB */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(0.15f, 0.25f, 0.05f);
	ColliderDesc.vCenter = _float3(0.f, -0.05f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureHPCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformHPCom);
}

void CPlayer::Skill_1Tick(_float fTimeDelta)
{
	HWND focusedWindow = GetFocus();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (bLocallyControlled && focusedWindow)
	{
		//키를 눌렀으면 2번
		if (pGameInstance->GetDIKDownState(DIK_X))
		{
			//스킬 요청
				//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_2;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
			return;
		}
	}

	if (bAnimEnd)
		UpdateState(STATE::IDLE);
}

void CPlayer::Skill_2Tick(_float fTimeDelta)
{
	HWND focusedWindow = GetFocus();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (bLocallyControlled && focusedWindow)
	{
		//키를 눌렀으면 2번
		if (pGameInstance->GetDIKDownState(DIK_X))
		{
			//스킬 요청
				//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_3;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
			return;
		}
	}

	if (bAnimEnd)
		UpdateState(STATE::IDLE);
}

void CPlayer::Skill_3Tick(_float fTimeDelta)
{
	HWND focusedWindow = GetFocus();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (bLocallyControlled && focusedWindow)
	{
		//키를 눌렀으면 2번
		if (pGameInstance->GetDIKDownState(DIK_X))
		{
			//스킬 요청
			//스킬 패킷
			ServerManager* ServerMgr = ServerManager::GetInstance();

			BYTE sendBuffer[PACKET_SIZE] = "";

			PacketHeader header;
			header.protocol = PKT_C_SKILL;
			header.size = PACKET_SIZE;
			*(PacketHeader*)sendBuffer = header;

			//누가 스킬을 사용했는지?
			int bufferOffset = sizeof(PacketHeader);
			memcpy(sendBuffer + bufferOffset, &m_iId, sizeof(m_iId));

			bufferOffset += sizeof(m_iId);

			//어디에 사용했는지?
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			memcpy(sendBuffer + bufferOffset, &vPos, sizeof(vPos));
			bufferOffset += sizeof(vPos);

			//어떤 스킬을 사용했는지?
			int skillId = ID_SKILL_4;
			memcpy(sendBuffer + bufferOffset, &skillId, sizeof(skillId));
			bufferOffset += sizeof(skillId);

			ServerMgr->Send(sendBuffer, PACKET_SIZE);
			return;
		}
	}

	if (bAnimEnd)
		UpdateState(STATE::IDLE);
}

void CPlayer::Skill_4Tick(_float fTimeDelta)
{
	if (bAnimEnd)
		UpdateState(STATE::IDLE);
}

void CPlayer::Skill_ATick(_float fTimeDelta)
{
	if (bAnimEnd)
		UpdateState(STATE::IDLE);
}
void CPlayer::Skill_STick(_float fTimeDelta)
{
	if (bAnimEnd)
		UpdateState(STATE::IDLE);
}
void CPlayer::Skill_DTick(_float fTimeDelta)
{
	if (bAnimEnd)
		UpdateState(STATE::IDLE);
}
void CPlayer::Skill_FTick(_float fTimeDelta)
{
	fTime += fTimeDelta * 5.0f;
	_vector NewPos = XMVectorLerp(XMLoadFloat3(&vTempPos), XMLoadFloat3(&vDestSkillPos), fTime / fTimeMax);

	if (bAnimEnd && fTime >= fTimeMax)
	{
		fTime = 0.f;
		UpdateState(STATE::IDLE);
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(NewPos, 1.f));
}
void CPlayer::Skill_GTick(_float fTimeDelta)
{
	fTime += fTimeDelta * 5.0f;
	_vector NewPos = XMVectorLerp(XMLoadFloat3(&vTempPos), XMLoadFloat3(&vDestSkillPos), fTime / fTimeMax);

	if (bAnimEnd && fTime >= fTimeMax)
	{
		fTime = 0.f;
		UpdateState(STATE::IDLE);
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(NewPos, 1.f));
}
void CPlayer::Skill_HTick(_float fTimeDelta)
{
	if (bAnimEnd)
		UpdateState(STATE::IDLE);
}
