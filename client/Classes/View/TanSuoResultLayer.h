#pragma once

#include "cocos2d.h"
#include "LFAlert.h"
#include "BaseSprite.h"
#include "protocol.h"
#include "flat.h"
using namespace cocos2d;

#define TAG_TanSuoResultLayerer_Layer 4643+10002


class TanSuoResultLayer : public LFAlert
{
public:
	static TanSuoResultLayer* create(CCString info, vector<ItemInfo> items,bool isJieFeng = false);

	TanSuoResultLayer(CCString info, vector<ItemInfo> items,bool isJieFeng);

	//virtual void onEnter();

protected:
	void menuItemOkClicked(CCObject* pSender);
private:
	CCString					m_sInfo;
	vector<ItemInfo>			m_iItems;
	bool						m_bIsJieFeng;
};
