#pragma once

/* 클라이언트에서 생성할 레벨들이 반드시 상속받아야 할 부코클래스다. */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();
	void GetDeviceContext(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
	{
		*ppDevice = m_pDevice;
		*ppContext = m_pContext;
	}
protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;


public:
	virtual void Free() override;
};

END