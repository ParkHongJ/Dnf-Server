#include "..\Public\Font_Manager.h"
#include "CustomFont.h"

IMPLEMENT_SINGLETON(CFont_Manager)

CFont_Manager::CFont_Manager()
{
}

HRESULT CFont_Manager::Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr != Find_Fonts(pFontTag))
		return E_FAIL;

	CCustomFont*		pFont = CCustomFont::Create(pDevice, pContext, pFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(pFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render_Fonts(const _tchar * pFontTag, const _tchar * pText, _float2 vPosition, _fvector vColor, _float fAngle, _float2 vOrigin, _float2 vScale)
{
	CCustomFont*		pFont = Find_Fonts(pFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(pText, vPosition, vColor, fAngle, vOrigin, vScale);	
}

CCustomFont * CFont_Manager::Find_Fonts(const _tchar * pFontTag)
{
	auto	iter = find_if(m_Fonts.begin(), m_Fonts.end(), CTag_Finder(pFontTag));

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;	
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();
}
