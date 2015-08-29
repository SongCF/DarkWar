#pragma once

#include "cocos2d.h"
#include "LFAlert.h"
#include "BaseSprite.h"
#include "protocol.h"
#include "flat.h"
using namespace cocos2d;

#define TAG_GiftDisplayer_Layer 4643+10002

class GiftSysDisplayCallBack_Interface
{
public: virtual void giftDisplayCallback_func() = 0;
};

class GiftDisplay : public LFAlert
{
public:
	static GiftDisplay* create(CCString info, Flat_BonusInfo items
		, GiftSysDisplayCallBack_Interface* callbackfunc = NULL);

	GiftDisplay(CCString info, Flat_BonusInfo items, GiftSysDisplayCallBack_Interface* callbackfunc = NULL);

protected:
	void menuItemOkClicked(CCObject* pSender);

protected:
	GiftSysDisplayCallBack_Interface *mCallback;
};
