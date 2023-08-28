#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	HRESULT Render(const _tchar* pTextm, _float2 vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), 
		_float fAngle = 0.f, _float2 vOrigin = _float2(0.f, 0.f), _float2 vScale = _float2(1.f, 1.f));

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

private:
	SpriteBatch*				m_pBatch = nullptr;
	SpriteFont*					m_pFont = nullptr;

public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual void Free() override;
};

END