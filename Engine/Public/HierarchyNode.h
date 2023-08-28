#pragma once

#include "Base.h"

/* 계층구조(뼈끼리의 상속관계)를 표현하기위한 aiNode이다. */

BEGIN(Engine)

class ENGINE_DLL CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}
	_uint Get_Depth() const {
		return m_iDepth;
	}

	_matrix Get_OffSetMatrix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombinedTransformation() {
		return XMLoadFloat4x4(&m_CombinedTransformation);
	}


public:
	void Set_Transformation(_fmatrix Transformation) {
		XMStoreFloat4x4(&m_Transformation, Transformation);
	}

public:
	HRESULT Initialize(aiNode* pAINode, class CHierarchyNode* pParent, _uint iDepth);
	void Set_CombinedTransformation();
	void Set_OffsetMatrix(_fmatrix OffsetMatrix);
private:
	char				m_szName[MAX_PATH] = "";
	_float4x4			m_OffsetMatrix;
	_float4x4			m_Transformation;
	_float4x4			m_CombinedTransformation;
	CHierarchyNode*		m_pParent = nullptr;
	_uint				m_iDepth = 0;

public:
	static CHierarchyNode* Create(aiNode* pAINode, class CHierarchyNode* pParent, _uint iDepth);
	virtual void Free();
};

END