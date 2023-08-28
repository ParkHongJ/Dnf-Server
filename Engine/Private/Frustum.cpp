#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{

}

HRESULT CFrustum::Initialize()
{
	m_vOriginalPoint[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginalPoint[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginalPoint[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginalPoint[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginalPoint[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginalPoint[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginalPoint[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginalPoint[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Tick()
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix			ViewMatrixInv = pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);
	_matrix			ProjMatrixInv = pPipeLine->Get_TransformMatrixInverse(CPipeLine::D3DTS_PROJ);

	_vector			vPoints[8];

	for (_uint i = 0; i < 8; ++i)
	{
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoint[i]), ProjMatrixInv);
		vPoints[i] = XMVector3TransformCoord(vPoints[i], ViewMatrixInv);

		XMStoreFloat3(&m_vWorldPoint[i], vPoints[i]);
	}

	XMStoreFloat4(&m_WorldPlane[0], XMPlaneFromPoints(vPoints[1], vPoints[5], vPoints[6]));
	XMStoreFloat4(&m_WorldPlane[1], XMPlaneFromPoints(vPoints[4], vPoints[0], vPoints[3]));
	XMStoreFloat4(&m_WorldPlane[2], XMPlaneFromPoints(vPoints[4], vPoints[5], vPoints[1]));
	XMStoreFloat4(&m_WorldPlane[3], XMPlaneFromPoints(vPoints[3], vPoints[2], vPoints[6]));
	XMStoreFloat4(&m_WorldPlane[4], XMPlaneFromPoints(vPoints[5], vPoints[4], vPoints[7]));
	XMStoreFloat4(&m_WorldPlane[5], XMPlaneFromPoints(vPoints[0], vPoints[1], vPoints[2]));
	
	RELEASE_INSTANCE(CPipeLine);
}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrixInv)
{
	_vector			vPoints[8];

	for (_uint i = 0; i < 8; ++i)	
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoint[i]), WorldMatrixInv);		

	XMStoreFloat4(&m_LocalPlane[0], XMPlaneFromPoints(vPoints[1], vPoints[5], vPoints[6]));
	XMStoreFloat4(&m_LocalPlane[1], XMPlaneFromPoints(vPoints[4], vPoints[0], vPoints[3]));
	XMStoreFloat4(&m_LocalPlane[2], XMPlaneFromPoints(vPoints[4], vPoints[5], vPoints[1]));
	XMStoreFloat4(&m_LocalPlane[3], XMPlaneFromPoints(vPoints[3], vPoints[2], vPoints[6]));
	XMStoreFloat4(&m_LocalPlane[4], XMPlaneFromPoints(vPoints[5], vPoints[4], vPoints[7]));
	XMStoreFloat4(&m_LocalPlane[5], XMPlaneFromPoints(vPoints[0], vPoints[1], vPoints[2]));	
}

_bool CFrustum::isIn_WorldSpace(_fvector vWorldPos, _float fRadius)
{/*
	a b c d
	x y z 

	ax + by + cz + d*/

	for (_uint i = 0; i < 6; ++i)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlane[i]), vWorldPos)))
			return false;		
	}

	return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vLocalPos, _float fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_LocalPlane[i]), vLocalPos)))
			return false;
	}
	return true;
}

void CFrustum::Free()
{

}
