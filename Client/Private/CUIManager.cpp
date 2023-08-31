#include "stdafx.h"
#include "..\Public\CUIManager.h"
#include "GameInstance.h"

//IMPLEMENT_SINGLETON(CUIManager)
CUIManager* CUIManager::Instance = nullptr;
CUIManager::CUIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUIManager::CUIManager(const CUIManager& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUIManager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIManager::Initialize(void* pArg)
{
	Ready_Components();

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));
	return S_OK;
}

void CUIManager::Tick(_float fTimeDelta)
{
}


void CUIManager::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUIManager::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	BackgroundRender();
	HPRender();
	MPRender();
	SkillRender();
	return S_OK;
}
HRESULT CUIManager::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MainUI"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HP"), TEXT("Com_Texture2"), (CComponent**)&m_pTextureHPCom)))
		return E_FAIL;

	return S_OK;
}
void CUIManager::HPRender()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	m_pTransformCom->Set_Scale(XMVectorSet(112.f, 112.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(80.f - g_iWinSizeX * 0.5f, -632.f + g_iWinSizeY * 0.5f, 0.0f, 1.f));

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	
	_float percentage = fPlayerHP / fPlayerMaxHP;
	m_pShaderCom->Set_RawValue("percentage", &percentage, sizeof(_float));

	if (FAILED(pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", UI_TYPE::HP)))
		return;

	if (FAILED(m_pShaderCom->Begin(3)))
		return;

	if (FAILED(m_pVIBufferCom->Render()))
		return;
}
void CUIManager::MPRender()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Set_Scale(XMVectorSet(112.f, 112.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1200.f - g_iWinSizeX * 0.5f, -632.f + g_iWinSizeY * 0.5f, 0.0f, 1.f));

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	_float percentage = fPlayerMP / fPlayerMaxMP;
	m_pShaderCom->Set_RawValue("percentage", &percentage, sizeof(_float));

	if (FAILED(pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", UI_TYPE::MP)))
		return;

	if (FAILED(m_pShaderCom->Begin(3)))
		return;

	if (FAILED(m_pVIBufferCom->Render()))
		return;
}
void CUIManager::BackgroundRender()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	m_pTransformCom->Set_Scale(XMVectorSet(1280.f, 176.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(640.f - g_iWinSizeX * 0.5f, -632.f + g_iWinSizeY * 0.5f, 0.0f, 1.f));

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	if (FAILED(pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", UI_TYPE::BackGround)))
		return;

	if (FAILED(m_pShaderCom->Begin(1)))
		return;

	if (FAILED(m_pVIBufferCom->Render()))
		return;
}
void CUIManager::SkillRender()
{
}

void CUIManager::SetHP(_float hp)
{
	fPlayerHP = hp;
}

CGameObject* CUIManager::Clone(void* pArg)
{
	CUIManager* pInstance = new CUIManager(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUIManager"));
		Safe_Release(pInstance);
	}

	Instance = pInstance;
	return pInstance;
}

void CUIManager::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureHPCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}

CUIManager* CUIManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIManager* pInstance = new CUIManager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUIManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
