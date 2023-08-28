#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	virtual HRESULT Render();

protected:
	ID3D11Buffer*					m_pVB = nullptr;
	ID3D11Buffer*					m_pIB = nullptr;

	D3D11_BUFFER_DESC				m_BufferDesc;
	D3D11_SUBRESOURCE_DATA			m_SubResourceData;

	D3D11_PRIMITIVE_TOPOLOGY		m_eTopology;

	_float3*						m_pVerticesPos = nullptr;

protected:
	_uint						m_iNumVertexBuffers = 0;
	_uint						m_iNumVertices = 0;
	_uint						m_iStride = 0;
	_uint						m_iNumPrimitives = 0;
	_uint						m_iIndexSizeofPrimitive = 0;
	_uint						m_iNumIndicesofPrimitive = 0;
	DXGI_FORMAT					m_eIndexFormat;


protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free();
};

END