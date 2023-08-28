#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CNavigation;


class CHierarchyNode;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum PARTTYPE { PART_WEAPON, PART_END };
	enum COLLIDERTYPE { COLLIDERTYPE_AABB, COLLIDERTYPE_OBB, COLLIDERTYPE_SPHERE, COLLILDERTYPE_END };
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
public:
	void SetLocallyControlled(_bool LocallyControlled) { bLocallyControlled = LocallyControlled; };
	void SendMovementPacket();
private:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLILDERTYPE_END] = { nullptr } ;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

	vector<class CHierarchyNode*>		m_Sockets;

	_bool bLocallyControlled = false;

	// 주기적으로 패킷을 전송
	_float elapsedTime = 0.0f;
	_float packetSendInterval = 1.0f; // 1초에 한 번 패킷을 전송

private:
	HRESULT Ready_Components();

	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();

	HRESULT Update_Weapon();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END