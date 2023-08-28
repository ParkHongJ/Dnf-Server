#pragma once

#include "Engine_Defines.h"

/* ��� Ŭ�������� �θ� �Ǵ� Ŭ�����̴�. */
/* ���۷��� ī��Ʈ�� �����Ѵ�. ����, ���� or ���� */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase() = default;
	virtual ~CBase() = default;

public:
	/* ��.ī�� ������Ű��. */
	/* unsigned long  : ������Ű�� �� ������ ��.ī */
	unsigned long AddRef();

	/* ��.ī�� ���ҽ�Ű��. or �����Ѵ�. */	
	/* unsigned long  : ���ҽ�Ű�� �� ������ ��.ī */
	unsigned long Release();

protected:
	unsigned long			m_dwRefCnt = 0;

public:
	virtual void Free() = 0;

};

END