#include "..\Public\OBB.h"
#include "AABB.h"
#include "Sphere.h"

COBB::COBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

COBB::COBB(const COBB & rhs)
	: CCollider(rhs)
{

}

HRESULT COBB::Initialize_Prototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::Initialize_Prototype(eColliderType)))
		return E_FAIL;

	m_eColliderType = CCollider::TYPE_OBB;

	return S_OK;
}

HRESULT COBB::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 복제될때 셋팅하고자하는 상태로 갱신한다. */
	m_pOriginal_OBB = new BoundingOrientedBox(m_ColliderDesc.vCenter, _float3(m_ColliderDesc.vSize.x * 0.5f,
		m_ColliderDesc.vSize.y * 0.5f,
		m_ColliderDesc.vSize.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	/* 회전에 대한 상태. */
	_matrix		RotationMatrix = XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
		XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
		XMMatrixRotationZ(m_ColliderDesc.vRotation.z);

	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	m_pOBB = new BoundingOrientedBox(*m_pOriginal_OBB);

	return S_OK;
}

void COBB::Update(_fmatrix TransformMatrix)
{
	m_pOriginal_OBB->Transform(*m_pOBB, TransformMatrix);
}

_bool COBB::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_ColliderType();

	m_isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pOBB->Intersects(((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_OBB:
		m_isColl = m_pOBB->Intersects(((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		m_isColl = m_pOBB->Intersects(((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}

	return m_isColl;
}

_bool COBB::Collision_OBB(CCollider * pTargetCollider)
{
	if (CCollider::TYPE_SPHERE == pTargetCollider->Get_ColliderType())
		return E_FAIL;

	m_isColl = false;

	OBBDESC			OBBDesc[2] = {
		Compute_OBBDesc(),
		((COBB*)pTargetCollider)->Compute_OBBDesc(),
	};

	for (_uint i = 0; i < 2; ++i)
	{	
		for (_uint j = 0; j < 3; ++j)
		{
			_vector			vCenterDir = XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter);

			_float			fDistance[3] = { 0.0f };
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(vCenterDir, XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));
	
			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			if (fDistance[0] > fDistance[1] + fDistance[2])
				return m_isColl;

		}
	}

	m_isColl = true;

	return m_isColl;
}

HRESULT COBB::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}

COBB::OBBDESC COBB::Compute_OBBDesc()
{
	OBBDESC			OBBDesc;

	_float3			vPoints[8];

	m_pOBB->GetCorners(vPoints);

	XMStoreFloat3(&OBBDesc.vCenter, (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[4])) * 0.5f);

	XMStoreFloat3(&OBBDesc.vCenterAxis[0], (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[5])) * 0.5f - 
		XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vCenterAxis[1], (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[7])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vCenterAxis[2], (XMLoadFloat3(&vPoints[2]) + XMLoadFloat3(&vPoints[0])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));

	XMStoreFloat3(&OBBDesc.vAlignAxis[0], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[0])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[1], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[1])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[2], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[2])));

	return OBBDesc;
}

COBB * COBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	COBB*			pInstance = new COBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * COBB::Clone(void * pArg)
{
	COBB*			pInstance = new COBB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void COBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_OBB);
	Safe_Delete(m_pOBB);

}
