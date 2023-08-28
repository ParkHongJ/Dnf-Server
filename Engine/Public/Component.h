#pragma once

#include "Base.h"

/*  ���� ������Ʈ���� �θ� �Ǵ� Ŭ������. */

BEGIN(Engine)

class ENGINE_DLL CComponent  abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
#ifdef _DEBUG
public:
	virtual HRESULT Render() { return S_OK; }
#endif // _DEBUG

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	_bool							m_isCloned = false;

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END