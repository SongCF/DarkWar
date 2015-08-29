
#pragma once

#include "cocos2d.h"
#include "LFAlert.h"
#include "BaseSprite.h"
#include "protocol.h"
#include "flat.h"
using namespace cocos2d;

#define TAG_AwardDisplayer_Layer 4643+10002

class AwardDisplayCallBack_Interface
{
public: virtual void awardDisplayCallback_func() = 0;
};

class AwardDisplay : public LFAlert
{
public:
	int id;
	static AwardDisplay* create(CCString info, vector<ItemInfo> items, vector<Flat_DuanzaoGroupZBInfo> equips = vector<Flat_DuanzaoGroupZBInfo>()
		, AwardDisplayCallBack_Interface* callbackfunc = NULL);

	static AwardDisplay* create(CCString info, vector<ItemInfo> items,GiftBag* bag, vector<Flat_DuanzaoGroupZBInfo> equips = vector<Flat_DuanzaoGroupZBInfo>()
		, AwardDisplayCallBack_Interface* callbackfunc = NULL);

	static AwardDisplay* create(Flat_GetSysTemGiftBagRsp *data);

	AwardDisplay(CCString info, vector<ItemInfo> items, vector<Flat_DuanzaoGroupZBInfo> equips, AwardDisplayCallBack_Interface* callbackfunc = NULL);
	AwardDisplay(CCString info, vector<ItemInfo> items,GiftBag* bag, vector<Flat_DuanzaoGroupZBInfo> equips, AwardDisplayCallBack_Interface* callbackfunc = NULL);
	AwardDisplay(Flat_GetSysTemGiftBagRsp *data);

	int getLioBaoNum(Flat_GetSysTemGiftBagRsp *data);
	void menuItemClicked_Equip(CCObject *obj);

protected:
	void menuItemOkClicked(CCObject* pSender);
	void menuItemOkClicked1(CCObject* pSender);

protected:
	AwardDisplayCallBack_Interface *mCallback;
};
