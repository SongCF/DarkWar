#pragma once
#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"

using namespace cocos2d::extension;

//head 玩家名字、金币、银币
#define STORE_FRAM_TAG_HEAD_NAME 1000
#define STORE_FRAM_TAG_HEAD_JINBI 1001
#define STORE_FRAM_TAG_HEAD_YINBI 1002	


enum enumStore
{
	store_ShenBing,
	store_DaoJu,
	store_LiBao,
	store_ChongZhi,
};

// 主界面点击装备选项看到的界面
class MainLayerStoreFrame : public BaseLayer,public IMainLayerBody
{
private:
	CCScale9Sprite* mBg;
	CCMenu* mMenu;
	enumStore mFlag;

	BaseLayer* mContentLayer;

	void menuItemClicked(CCObject* sender);

	void getMarketInfoCallBack(CCObject* msg);

public:
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	// 得到body的大小
	virtual CCSize getBodySize(){return CCSize(640, 842);}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(320, mBg->getContentSize().height/2);}


	MainLayerStoreFrame(enumStore flag = store_ChongZhi);
	~MainLayerStoreFrame(void);
};

