
#pragma once

#include "cocos2d.h"
USING_NS_CC;

class DegreeUpEndCallBackInterface
{
public:
	virtual void DegreeUpEndCallBackFunc() = 0;
};

class DegreeUpLayer : public CCLayerColor
{
public:
	bool init(unsigned int hero_id, DegreeUpEndCallBackInterface *func);
	static DegreeUpLayer* create(unsigned int hero_id, DegreeUpEndCallBackInterface *func=NULL);

	void menuItemClicked(CCObject *pSender);
	void rsp_YuanShenDegreeUp(CCObject *msg);
	void degreeUpAnimate(int stage);

	void callbackRemove(CCNode* pSender);
	CCSprite* getProfLabel(int num, int prof);

public:
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);


private:
	bool mDisplayerEnd;
	void setDisplayerEnd();

	DegreeUpEndCallBackInterface *mCallbackFun;

	CCSize mWinSize;
	CCMenu *mMainMenu;

	unsigned int mHeroId;
};