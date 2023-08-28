#include "..\Public\RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4 * pClearColor)
{
	m_vClearColor = *pClearColor;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = eFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}

HRESULT CRenderTarget::Bind_SRV(CShader * pShader, const char * pConstantName)
{
	return pShader->Set_ShaderResourceView(pConstantName, m_pSRV);	
}

#ifdef _DEBUG

HRESULT CRenderTarget::Initialize_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_uint		iNumViewport = 1;
	D3D11_VIEWPORT		ViewportDesc;
	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, 
		XMMatrixTranspose(XMMatrixScaling(fSizeX, fSizeY, 0.f) * XMMatrixTranslation(fX - ViewportDesc.Width * 0.5f, -fY + ViewportDesc.Height * 0.5f, 0.f)));
	
	return S_OK;
}


HRESULT CRenderTarget::Render_Debug(CShader* pShader, CVIBuffer * pVIBuffer)
{
	pShader->Set_RawValue("g_WorldMatrix", &m_WorldMatrix, sizeof(_float4x4));

	pShader->Set_ShaderResourceView("g_DiffuseTexture", m_pSRV);

	pShader->Begin(0);	

	return pVIBuffer->Render();
}
#endif // _DEBUG

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4 * pClearColor)
{
	CRenderTarget*		pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, eFormat, pClearColor)))
	{
		MSG_BOX(TEXT("Failed To Created : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pTexture2D);
	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
