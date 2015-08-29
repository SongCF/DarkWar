#pragma once

#include "cocos2d.h"
#include "MapsLayer.h"
#include "NetLabelTTF.h"

USING_NS_CC;

#define Tag_FightEndBase_Layer 4043+30

// 任务结算 层          //把改层添加到战斗层之上
class FightEndBaseLayer : public CCLayer
{
public:
	typedef enum _ComeFromType
	{
		ComeFrom_Jingji_Type = 1,				//竞技
		ComeFrom_Jingji_Suiji_Type,				//随机竞技
		ComeFrom_Shouhuzhiling_Type,			//守护之灵
		ComeFrom_Xuesejiaotang_Type,			//血色教堂
		ComeFrom_Jixianshoucheng_Type,			//极限守城
		ComeFrom_Yingxiongbang_Type,			//英雄榜
	}ComeFromType;

public:
	static FightEndBaseLayer* create(bool bWin, ComeFromType type);

	FightEndBaseLayer(bool bWin, ComeFromType type);
	~FightEndBaseLayer(){}

	void backToShouhuzhiling();			//跳转到守护之灵（挑战开始界面）
	void backToXuesejiaotang();			//跳转到血色教堂（闯关界面）
	void setSuijiJingjiIndex(int index){m_iSuijiJingjiIndex = index;};

private:
	void showMenuItem();
	void menuItemClicked_MainMenu(CCObject *pSender);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onEnter();
	virtual void onExit();
	

private:
	int						m_iSuijiJingjiIndex;			//随机竞技的场数索引

	bool					m_bWin;
	CCSize					m_WinSize;

	ComeFromType			m_ComeFromType;
	CCMenu*					m_pMenu;
	NetLabelTTF*			m_pTipLabel;
};