#include "..\Public\QuadTree.h"
#include "Frustum.h"
#include "PipeLine.h"

CQuadTree::CQuadTree()
	: m_pPipeLine(CPipeLine::Get_Instance())
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	if (iRT - iLT == 1)
		return S_OK;

	m_iCenterIndex = (iLT + iRB) >> 1;

	_uint		iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;	

	m_pChild[CHILD_LT] = CQuadTree::Create(iLT, iTC, m_iCenterIndex, iLC);
	m_pChild[CHILD_RT] = CQuadTree::Create(iTC, iRT, iRC, m_iCenterIndex);
	m_pChild[CHILD_RB] = CQuadTree::Create(m_iCenterIndex, iRC, iRB, iBC);
	m_pChild[CHILD_LB] = CQuadTree::Create(iLC, m_iCenterIndex, iBC, iLB);

	return S_OK;
}

void CQuadTree::Culling(const _float3* pVerticesPos, CFrustum* pFrustum, FACEINDICES32* pFaceIndices, _uint* pNumFaces)
{
	/* 가장 작은 쿼트트리일 경우. */
	/* 카메라로 부터 멀리있다면. */
	if (nullptr == m_pChild[CHILD_LB] ||
		true == LevelofDetail(pVerticesPos))
	{
		_uint		iIndices[] = {
			m_iCorners[CORNER_LT],
			m_iCorners[CORNER_RT],
			m_iCorners[CORNER_RB],
			m_iCorners[CORNER_LB],
		};

		_bool		isIn[] = {
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[0]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[1]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[2]])),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[3]])),
		};

		_bool		isDraw[NEIGHBOR_END] = { true, true, true, true };		

		if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])		
			isDraw[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->LevelofDetail(pVerticesPos);
		if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
			isDraw[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->LevelofDetail(pVerticesPos);
		if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
			isDraw[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->LevelofDetail(pVerticesPos);
		if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
			isDraw[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->LevelofDetail(pVerticesPos);
		
		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			/* 오른쪽 위 삼각형을 그려야한다. */
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pFaceIndices[*pNumFaces]._0 = iIndices[0];
				pFaceIndices[*pNumFaces]._1 = iIndices[1];
				pFaceIndices[*pNumFaces]._2 = iIndices[2];
				++*pNumFaces;
			}

			/* 왼쪽 하단 삼각형을 그려야한다. */
			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pFaceIndices[*pNumFaces]._0 = iIndices[0];
				pFaceIndices[*pNumFaces]._1 = iIndices[2];
				pFaceIndices[*pNumFaces]._2 = iIndices[3];
				++*pNumFaces;
			}

			return;
		}

		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pFaceIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pFaceIndices[*pNumFaces]._1 = m_iCenterIndex;
				pFaceIndices[*pNumFaces]._2 = iLC;
				++*pNumFaces;

				pFaceIndices[*pNumFaces]._0 = iLC;
				pFaceIndices[*pNumFaces]._1 = m_iCenterIndex;
				pFaceIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}
			else
			{
				pFaceIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pFaceIndices[*pNumFaces]._1 = m_iCenterIndex;
				pFaceIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}

			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pFaceIndices[*pNumFaces]._0 = m_iCorners[CORNER_LB];
				pFaceIndices[*pNumFaces]._1 = m_iCenterIndex;
				pFaceIndices[*pNumFaces]._2 = iBC;
				++*pNumFaces;

				pFaceIndices[*pNumFaces]._0 = iBC;
				pFaceIndices[*pNumFaces]._1 = m_iCenterIndex;
				pFaceIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}
			else
			{
				pFaceIndices[*pNumFaces]._0 = m_iCorners[CORNER_LB];
				pFaceIndices[*pNumFaces]._1 = m_iCenterIndex;
				pFaceIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}
		}

		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pFaceIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pFaceIndices[*pNumFaces]._1 = iTC;
				pFaceIndices[*pNumFaces]._2 = m_iCenterIndex;
				++*pNumFaces;

				pFaceIndices[*pNumFaces]._0 = m_iCenterIndex;
				pFaceIndices[*pNumFaces]._1 = iTC;
				pFaceIndices[*pNumFaces]._2 = m_iCorners[CORNER_RT];
				++*pNumFaces;
			}
			else
			{
				pFaceIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pFaceIndices[*pNumFaces]._1 = m_iCorners[CORNER_RT];
				pFaceIndices[*pNumFaces]._2 = m_iCenterIndex;
				++*pNumFaces;
			}

			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pFaceIndices[*pNumFaces]._0 = m_iCorners[CORNER_RT];
				pFaceIndices[*pNumFaces]._1 = iRC;
				pFaceIndices[*pNumFaces]._2 = m_iCenterIndex;
				++*pNumFaces;

				pFaceIndices[*pNumFaces]._0 = m_iCenterIndex;
				pFaceIndices[*pNumFaces]._1 = iRC;
				pFaceIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}
			else
			{
				pFaceIndices[*pNumFaces]._0 = m_iCorners[CORNER_RT];
				pFaceIndices[*pNumFaces]._1 = m_iCorners[CORNER_RB];
				pFaceIndices[*pNumFaces]._2 = m_iCenterIndex;
				++*pNumFaces;
			}

		}

		return;
	}	

	_float		fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenterIndex])));

	if (true == pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenterIndex]), fRadius))
	{
		for (_uint i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Culling(pVerticesPos, pFrustum, pFaceIndices,pNumFaces);
		}
	}

	return;
}



HRESULT CQuadTree::Make_Neighbor()
{
	if (nullptr == m_pChild[CHILD_LT]->m_pChild[CHILD_LT])
		return S_OK;


	m_pChild[CHILD_LT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
	m_pChild[CHILD_LT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

	m_pChild[CHILD_RT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
	m_pChild[CHILD_RT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

	m_pChild[CHILD_RB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];
	m_pChild[CHILD_RB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChild[CHILD_RT];

	m_pChild[CHILD_LB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];
	m_pChild[CHILD_LB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChild[CHILD_LT];

	if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
	{
		m_pChild[CHILD_LB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RT];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
	{
		m_pChild[CHILD_LT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
		m_pChild[CHILD_LB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
		m_pChild[CHILD_RT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
	}


	for (_uint i = 0; i < CHILD_END; ++i)
	{
		if (nullptr != m_pChild[i])
			m_pChild[i]->Make_Neighbor();
	}

	

	
	return S_OK;
}

_bool CQuadTree::LevelofDetail(const _float3 * pVerticesPos)
{
	if (nullptr == m_pPipeLine)
		return false;

	_vector		vCamPosition = XMLoadFloat4(&m_pPipeLine->Get_CamPosition());	

	_float		fDistance = XMVectorGetX(XMVector3Length(vCamPosition - XMLoadFloat3(&pVerticesPos[m_iCenterIndex])));

	_float		fWidth = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]])- XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]])));

	if (fDistance * 0.1f > fWidth)
		return true;

	return false;	
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX(TEXT("Failed To Created : CQuadTree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	for (auto& pChild : m_pChild)
		Safe_Release(pChild);

	Safe_Release(m_pPipeLine);
}
