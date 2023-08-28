#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeighitMapFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	void Culling(const class CTransform* pTransform);

private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;
	FACEINDICES32*	m_pFaceIndices = nullptr;
	class CQuadTree*		m_pQuadTree = nullptr;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeighitMapFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

END