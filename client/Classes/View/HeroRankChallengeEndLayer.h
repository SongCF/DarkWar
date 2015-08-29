
#pragma once

#include "cocos2d.h"

USING_NS_CC;

#define Tag_HeroRankChallengeEnd_Layer 4643+30

// 任务结算 层          //把改层添加到战斗层之上
class HeroRankChallengeEndLayer : public CCLayer
{
public:
	static HeroRankChallengeEndLayer* create(bool bWin);
	
	HeroRankChallengeEndLayer(bool bWin);
	~HeroRankChallengeEndLayer();

protected:
	void displayInfomation();
	void menuItemClicked_MainMenu(CCObject *pSender);
	void rsp_challengeAgain(CCObject *msg_f);

	void menuItemClicked_ClearTime(CCObject *pSender);
	void rsp_clearCDTime(CCObject *msg);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onEnter();
	virtual void onExit();
		
private:
	CCSize mWinSize;

	CCMenu *mMenu;

	bool mWin;
};