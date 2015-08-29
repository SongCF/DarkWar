#pragma once

#include "cocos2d.h"
#include "BaseSprite.h"
#include <string>

using std::string;
using namespace cocos2d;

class XueSeJiaoTangFightPrepareLayer : public CCLayerColor
{
public:
	XueSeJiaoTangFightPrepareLayer();

protected:
	void displayAward(float dt);
	void displayChoseAwardBox();
	void displayTaskAward();
	void refreshJiQuNengLiangItem(CCMenu *dialogMenu);
	void refreshJiQuNengLiangDataLabel();

	void menuItemClicked_FanHui(CCObject *pSender);
	void menuItemClicked_fightCircle(CCObject* pSender);
	void menuItemClicked_XiQuNengLiang(CCObject *pSender);
	void menuItemClicked_ChoseAward(CCObject *pSender);
	void menuItemClicked_JiQuNengLiangBoard(CCObject *pSender);

	void rsp_getChoseAward(CCObject *msg);
	void rsp_JiQuNengLiang(CCObject *msg);
	void rsp_Challenge(CCObject *msg_f);
	void menuItemClicked_BaoXiang(CCObject* pObj);
	int getLevelValue();
protected:
	BaseSprite *mBg;
	CCSize mWinSize;

	//方便
	CCMenuItem *mChoseCardMenuItem;
	CCMenu *mJiQuDialogMenu;
	CCLabelTTF *mAllNengLiang_JiQuDialog;
	//为了方便
	CCLabelTTF *mAllNengLiangL; // 拥有能量
	CCLabelTTF *mAddXueLiangL; // 血量加成
	CCLabelTTF *mAddGongJiL;   // 攻击加成
	CCLabelTTF *mAddFangYuL; // 防御加成
};