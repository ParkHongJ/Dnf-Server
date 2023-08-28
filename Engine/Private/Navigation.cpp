#include "..\Public\Navigation.h"
#include "Cell.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
	, m_NavigationDesc(rhs.m_NavigationDesc)
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const _tchar * pNavigationDataFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3			vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*			pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(Ready_Neighbor()))
		return E_FAIL;

	// m_Cells[1]->Set_NeighborIndex(CCell::LINE_AB, nullptr);

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{

	if(nullptr != pArg)
		memcpy(&m_NavigationDesc, pArg, sizeof(NAVIGATIONDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	return S_OK;
}

_bool CNavigation::isMove(_fvector vPosition)
{
	_int	iNeighborIndex = -1;

	/* 현재 쎌 안에서 움직였다. */
	/* 나간방향에 이웃이 있다면. 이웃의 인ㄷ게스를 받아오고. 
	이웃이 없다면 안채워온다. */
	if (true == m_Cells[m_NavigationDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex))
		return true;

	/* 현재 셀을 나갔다. */
	else
	{
		/* 나간방향에 이웃이 있었다면. */
		if (0 <= iNeighborIndex)
		{
			while (true)
			{
				if (0 > iNeighborIndex)
					return false;				

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
				{
					/* 커런트 인덱스를 이웃의 인덱스로 바꿔준다. */
					m_NavigationDesc.iCurrentIndex = iNeighborIndex;

					return true;
				}
			}
			
		}

		/* 나간방향에 이웃이 없었다면. */
		else
			return false;


	}
	

	return _bool();
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (-1 == m_NavigationDesc.iCurrentIndex)
	{
		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render_Cell();
		}
	}
	else
		m_Cells[m_NavigationDesc.iCurrentIndex]->Render_Cell(0.05f, _float4(1.f, 0.f, 0.f, 1.f));
	

	return S_OK;
}

#endif // _DEBUG

HRESULT CNavigation::Ready_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				// 너(Dest)는 내(Sour) AB이웃. 
				pSourCell->Set_NeighborIndex(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{				
				pSourCell->Set_NeighborIndex(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_NeighborIndex(CCell::LINE_CA, pDestCell);
			}
		}
	}


	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationDataFilePath)
{
	CNavigation*			pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFilePath)))
	{
		MSG_BOX(TEXT("Failed To Created : CNavigation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*			pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNavigation"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}
