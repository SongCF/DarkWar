#pragma once
#include "BaseLayer.h"
#include "BaseSprite.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
#include "MainLayerHerosBaseBody.h"
#include "ZhuShouChatLayer.h"
using namespace cocos2d::extension;

class MainLayerTeam;

// 主界面点击装备选项看到的界面
class MainLayerTeamList : public MainLayerHerosBaseBody,ZhuShouChatLayerCallBack
{
private:
	BaseSprite* mBg;
	CCMenu* mHeadMenu;
	CCMenu *mMainMenu;


protected:
	void menuItemClicked_HeadMenu(CCObject* pSender);
	void menuItemClicked_MainMenu(CCObject* pSender);
	virtual void rsp_JieSuoYingXiong(CCObject *msg);

	void rsp_BuZhen(CCObject* msg);

public:
	virtual void onEnter();
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();


	// 得到body的大小
	virtual CCSize getBodySize(){return CCSize(640, 842);};
	//
	virtual CCPoint getBodyCenterPos(){return ccp(320, mBg->getContentSize().height/2);};

	virtual void ZhuShouchatEndCallBack(){}
	//1已招募   2未招募
	MainLayerTeamList(int flag = 1);
	~MainLayerTeamList(void);
};