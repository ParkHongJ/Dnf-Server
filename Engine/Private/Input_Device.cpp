#include "..\Public\Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
{
}

_bool CInput_Device::GetDIKDownState(_uchar eKeyID)
{
	m_pKeyboard->GetDeviceState(256, m_byKeyState);

	if (!m_keyDownState[eKeyID] && m_byKeyState[eKeyID] & 0x80) {
		m_keyDownState[eKeyID] = true;
		m_keyUpState[eKeyID] = true;

		return true;
	}

	return false;
}

_bool CInput_Device::GetDIKUPState(_uchar eKeyID)
{
	m_pKeyboard->GetDeviceState(256, m_byKeyState);

	if (m_keyUpState[eKeyID] && !(m_byKeyState[eKeyID] & 0x80)) {
		m_keyUpState[eKeyID] = false;
		m_keyDownState[eKeyID] = false;

		return true;
	}

	return false;
}

_bool CInput_Device::GetDIMKeyDownState(DIMK eMouseKeyID)
{
	if (!m_mouseDown[eMouseKeyID] && m_MouseState.rgbButtons[eMouseKeyID] & 0x80) {
		m_mouseDown[eMouseKeyID] = true;

		return true;
	}

	return false;
}

HRESULT CInput_Device::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	/* m_pInputSDK */
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;


	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

void CInput_Device::Update()
{
	m_pKeyboard->GetDeviceState(256, m_byKeyState);

	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

	for (int i = 0; i < 256; ++i)
	{
		if (m_keyDownState[i] && !(m_byKeyState[i] & 0x80))
		{
			m_keyDownState[i] = false;
			m_keyUpState[i] = true;
		}
		else if (m_keyUpState[i] && !(m_byKeyState[i] & 0x80))
		{
			m_keyUpState[i] = false;
		}
	}

	for (int i = 0; i < DIMM::DIMM_END; ++i)
	{
		if (m_mouseDown[i] && !(m_MouseState.rgbButtons[i] & 0x80))
		{
			m_mouseDown[i] = false;
		}
	}
}

void CInput_Device::Free()
{
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pMouse);

	Safe_Release(m_pInputSDK);
}
