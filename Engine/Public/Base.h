#pragma once

#include "Engine_Defines.h"

/* 모든 클래스들의 부모가 되는 클래스이다. */
/* 레퍼런스 카운트를 관리한다. 증가, 감소 or 삭제 */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase() = default;
	virtual ~CBase() = default;

public:
	/* 레.카를 증가시키낟. */
	/* unsigned long  : 증가시키고 난 이후의 레.카 */
	unsigned long AddRef();

	/* 레.카를 감소시키낟. or 삭제한다. */	
	/* unsigned long  : 감소시키고 난 이전의 레.카 */
	unsigned long Release();

protected:
	unsigned long			m_dwRefCnt = 0;

public:
	virtual void Free() = 0;

};

END