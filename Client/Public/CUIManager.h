#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

enum UI_TYPE
{
	BackGround = 0,
	HP,
	MP,
	Stamina,
	Exp
};

BEGIN(Client)
class CUIManager final : public CGameObject
{
	//DECLARE_SINGLETON(CUIManager)

private:
	CUIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIManager(const CUIManager& rhs);
	virtual ~CUIManager() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();
	
public:
	void HPRender();
	void MPRender();
	void BackgroundRender();
	void SkillRender();

private:
	CTexture* m_pTextureCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;

	_float4x4 m_ViewMatrix, m_ProjMatrix;

public:
	virtual void Free() override;

	static CUIManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// CGameObject을(를) 통해 상속됨
	virtual CGameObject* Clone(void* pArg) override;
};
END

