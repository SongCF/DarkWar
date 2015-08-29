
#pragma once

#include "cocos2d.h"
USING_NS_CC;

class LevelUpEndCallBackInterface
{
public:
	virtual void LevelUpEndCallBackFunc() = 0;
};

class LevelUpLayer : public CCLayerColor
{
public:
	//最后一个参数是战斗结算的时候，材料不足点击英雄头像后传true，还是能显示该layer
	bool init(unsigned int hero_id, LevelUpEndCallBackInterface *func, bool fightEndDisplay);
	static LevelUpLayer* create(unsigned int hero_id, LevelUpEndCallBackInterface *func=NULL, bool fightEndDisplay=false);

	void setNextLevelUpLayer(LevelUpLayer *nextLayer)
	{
		nextLayer->retain(); 
		mNextLayer = nextLayer;
	} //ccTouchEnded/onCancel 添加到scene的时候release , 

	void menuItemClicked(CCObject *pSender);
	void rsp_HeroLevelUp(CCObject *msg);
	void levelUpAnimate(int liliang, int minjie, int zhili, int tineng);

	void callbackRemove(CCNode* pSender);
	CCSprite* getProfLabel(int num, int prof);


protected:
	CCLabelTTF *mAllCaiLiaoNumLabel;
	void refreshLayer();  //刷新材料个数、 多个一起升级的时候，会出来前一个升级了，后一个的勋章没刷新


	int mNewLev;
	void teachGuide();

public:
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);


private:
	bool mDisplayerEnd;
	void setDisplayerEnd();

	LevelUpEndCallBackInterface *mCallbackFun;

	CCSize mWinSize;
	CCMenu *mMainMenu;

	unsigned int mHeroId;
	int mLiliang;
	int mMinJie;
	int mZhili;
	int mTineng;
	int mOldLv;

	LevelUpLayer *mNextLayer;   //同时存在多个角色升级
	//多个layer同时在时 等级小与7的会自动调用menuitemclicked，监听消息，不来不是它的也在用了   默认false
};