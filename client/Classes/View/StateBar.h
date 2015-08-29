#pragma once

#include "BaseSprite.h"
#include "GetTiLiLayer.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;

// 状态条
class StateBar : public BaseSprite, public GetTiLiCallBack
{
private:
	void testClicked(CCObject* sender);
	inline CCSprite* getVipImg(int level)
	{
		char buf[40];
		sprintf(buf, "Vip_icon_lv%d.png", level);

		return CCSprite::create(buf);
	}
public:
	static StateBar* createBar();
	BaseSprite *getVipImg();
	StateBar(void);
	~StateBar(void);

	CCLabelTTF* getSilverLabel();
	CCLabelTTF* getLevelLabel();
	CCProgressTimer* getExpBar();

	void menuItemClicked_TiLiBuy(CCObject *pSender);
	virtual void getTiliCallBackFunc();

	void refreshTiLiBar();
	void refreshGoldAndSilver();
	void refreshVipMig(int level);
	void clickHero(CCObject *obj);
private:
	CCSprite			*m_sBgSpr;
};

