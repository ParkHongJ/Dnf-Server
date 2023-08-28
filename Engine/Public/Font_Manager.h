#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
	DECLARE_SINGLETON(CFont_Manager)
public:
	CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Fonts(const _tchar* pFontTag, const _tchar* pTextm, _float2 vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fAngle = 0.f, _float2 vOrigin = _float2(0.f, 0.f), _float2 vScale = _float2(1.f, 1.f));

private:
	map<const _tchar*, class CCustomFont*>			m_Fonts;
	typedef map<const _tchar*, class CCustomFont*>	FONTS;

private:
	class CCustomFont* Find_Fonts(const _tchar* pFontTag);

public:
	virtual void Free() override;
};

END