#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CAABB final : public CCollider
{
protected:
	CAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAABB(const CAABB& rhs);
	virtual ~CAABB() = default;

public:
	BoundingBox Get_Collider() {
		return *m_pAABB;
	}
	

public:
	virtual HRESULT Initialize_Prototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);

public:
	_bool Collision_AABB(CCollider* pTargetCollider);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG

private:
	BoundingBox*			m_pOriginal_AABB = nullptr;
	BoundingBox*			m_pAABB = nullptr;

private:
	_float3 Compute_Min();
	_float3 Compute_Max();


public:
	static CAABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;


};

END