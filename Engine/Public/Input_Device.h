#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)	
private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_char Get_DIKState(_uchar eKeyID) {
		return m_byKeyState[eKeyID];
	}

	_char Get_DIMKeyState(DIMK eMouseKeyID) {
		return m_MouseState.rgbButtons[eMouseKeyID];
	}

	_long Get_DIMMoveState(DIMM eMouseMoveID) {
		return ((_long*)&m_MouseState)[eMouseMoveID];
	}

	_bool GetDIKDownState(_uchar eKeyID);
	_bool GetDIKUPState(_uchar eKeyID);

	_bool GetDIMKeyDownState(DIMK eMouseKeyID);



public:
	HRESULT Initialize(HINSTANCE hInstance, HWND hWnd);
	void Update();

private:
	LPDIRECTINPUT8				m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;

private:	
	_char m_byKeyState[256] = { 0 };
	_bool m_keyDownState[256] = { false };
	_bool m_keyUpState[256] = { false };

	_bool m_mouseDown[DIMM::DIMM_END]{ false };
	DIMOUSESTATE m_MouseState;

public:
	virtual void Free();
};

END