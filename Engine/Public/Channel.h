#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

public:
	HRESULT Initialize(aiNodeAnim*	pAIChannel);
	_uint Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode);


private:
	char							m_szName[MAX_PATH] = "";

	_uint							m_iNumKeyFrames = 0;
	vector<KEYFRAME>				m_KeyFrames;	


public:
	static CChannel* Create(aiNodeAnim*	pAIChannel);
	virtual void Free() override;
};

END