
#pragma once

#include "cocos2d.h"

USING_NS_CC;

#define Tag_XueSeJiaoTangEnd_Layer 4643+40

// 任务结算 层          //把改层添加到战斗层之上
class XueSeJiaoTangEndLayer : public CCLayer
{
public:
	static XueSeJiaoTangEndLayer* create(bool bWin);
	
	XueSeJiaoTangEndLayer(bool bWin);

protected:
	void displayInfomation();
	void menuItemClicked_MainMenu(CCObject *pSender);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onEnter();
	virtual void onExit();
		
private:
	CCSize mWinSize;

	CCMenu *mMenu;

	bool mWin;
};