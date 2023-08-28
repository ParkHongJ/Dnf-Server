#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollider abstract : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

public:
	typedef struct tagColliderDesc
	{
		_float3			vCenter;
		_float3			vSize;
		_float3			vRotation;
	}COLLIDERDESC;

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	TYPE Get_ColliderType() const {
		return m_eColliderType;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix) = 0;
	virtual _bool Collision(CCollider* pTargetCollider) = 0;


#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

protected:
	TYPE					m_eColliderType = TYPE_END;
	COLLIDERDESC			m_ColliderDesc;
	_bool					m_isColl = false;
	

#ifdef _DEBUG
protected:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = nullptr;
	BasicEffect*								m_pEffect = nullptr;	
	ID3D11InputLayout*							m_pInputLayout = nullptr;
	_float4										m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
#endif

protected:
	_matrix Remove_Rotation(_fmatrix Matrix);

public:	
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;


};

END