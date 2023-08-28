#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
public:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:	
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4* pClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);

public:
	HRESULT Bind_SRV(const _tchar* pTargetTag, class CShader* pShader, const char* pConstantName);

	/* ������ �ִ� ����Ÿ���� ����, ������ ����Ÿ�ٵ�(mrt)�� ��ġ�� ������� ���ε��Ѵ�. */
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const _tchar* pMRTTag);

	/* ���� ���·� �����Ѵ�.(BackBuffer�� ��ġ�� ���Ѵ�.) */
	HRESULT End_MRT(ID3D11DeviceContext* pContext);

#ifdef _DEBUG
public:	
	HRESULT Initialize_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(const _tchar* pMRTTag, class CVIBuffer* pVIBuffer, class CShader* pShader);
#endif // _DEBUG


private: /* ������ ����Ÿ�ٵ��� ��ü �� ��Ƴ��´�. */
	map<const _tchar*, class CRenderTarget*>			m_RenderTargets;
	typedef map<const _tchar*, class CRenderTarget*>	RENDERTARGETS;

private: /* ���ÿ� ���ε��Ǿ���� ����Ÿ�ٵ��� LIST�� ������´�. (Diffuse + Normal + Depth, Shader + Specular) */
	map<const _tchar*, list<class CRenderTarget*>>			m_MRTs;
	typedef map<const _tchar*, list<class CRenderTarget*>>	MRTS;

private:
	ID3D11RenderTargetView*					m_pOldRenderTargets[8] = { nullptr };
	ID3D11DepthStencilView*					m_pOldDepthStencil = nullptr;



private:
	class CRenderTarget* Find_RenderTarget(const _tchar* pTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END