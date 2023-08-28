#pragma once

#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Light_Manager.h"
#include "PipeLine.h"
#include "Frustum.h"
#include "Font_Manager.h"
#include "Target_Manager.h"

/* 클라이언트로 보여주기위한 가장 대표적인 클래스이다. */
/* 각종 매니져클래스들의 주요함수를 클라로 보여준다.  */
/* 엔진초기화. */
/* 엔진정리. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(_uint iNumLevel, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();


public: /* For.Level_Manager */				
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	HRESULT Render_Level();

	void GetDeviceContext(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);

public: /* For.Object_Manager */
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iLayerIndex);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	CGameObject* Add_GameObjectToLayer(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);

public: /*For.Component_Manager*/
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public: /* for.Timer_Manager */
	_float Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT Add_Timer(const _tchar* pTimerTag);
	HRESULT Update_Timer(const _tchar* pTimerTag);	

public: /* For.Input_Device */
	_char Get_DIKState(_uchar eKeyID);
	_char Get_DIMKeyState(DIMK eMouseKeyID);
	_long Get_DIMMoveState(DIMM eMouseMoveID);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const;		
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4 Get_CamPosition();

public: /* For.Light_Manager */
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);

public: /* For.Frustum */
	_bool isIn_Frustum_WorldSpace(_fvector vWorldPos, float fRadius = 0.f);

public: /* For.Font_Manager */
	HRESULT Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Fonts(const _tchar* pFontTag, const _tchar* pTextm, _float2 vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),
		_float fAngle = 0.f, _float2 vOrigin = _float2(0.f, 0.f), _float2 vScale = _float2(1.f, 1.f));

public:  /* For.Target_Manager*/ 
	HRESULT Bind_RenderTargetSRV(const _tchar* pTargetTag, class CShader* pShader, const char* pConstantName);

private:
	CGraphic_Device*				m_pGraphic_Device = nullptr;
	CInput_Device*					m_pInput_Device = nullptr;
	CLevel_Manager*					m_pLevel_Manager = nullptr;
	CObject_Manager*				m_pObject_Manager = nullptr;
	CComponent_Manager*				m_pComponent_Manager = nullptr;
	CTimer_Manager*					m_pTimer_Manager = nullptr;
	CPipeLine*						m_pPipeLine = nullptr;
	CLight_Manager*					m_pLight_Manager = nullptr;
	CFrustum*						m_pFrustum = nullptr;
	CFont_Manager*					m_pFont_Manager = nullptr;
	CTarget_Manager*				m_pTarget_Manager = nullptr;


public:
	static void Release_Engine();
	virtual void Free() override;
};

END