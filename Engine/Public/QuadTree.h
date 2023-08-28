#pragma once

#include "Base.h"

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	enum CHILD { CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END };
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };
private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	void Culling(const _float3* pVerticesPos, class CFrustum* pFrustum, FACEINDICES32* pFaceIndices, _uint* pNumFaces);
	HRESULT Make_Neighbor();
private:
	CQuadTree*				m_pChild[CHILD_END] = { nullptr };
	_uint					m_iCenterIndex = 0;
	_uint					m_iCorners[CORNER_END];
	CQuadTree*				m_pNeighbors[NEIGHBOR_END] = { nullptr };
	class CPipeLine*				m_pPipeLine = nullptr;

private:

	_bool LevelofDetail(const _float3* pVerticesPos);

public:
	static CQuadTree* Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB);
	virtual void Free() override;
};

END