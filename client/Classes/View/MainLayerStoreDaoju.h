#pragma once
#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
#include "MainLayerItemsBaseBody.h"

using namespace cocos2d::extension;

#define TAG_BaseBody_LABLE_PRICE			12111
#define TAG_BaseBody_LABLE_PRICE_NUM		12112
#define TAG_BaseBody_LABLE_PRICE_PIC		12113
#define TAG_BaseBody_MENU_BUY				12114
#define TAG_BaseBody_BUY_QUERENKUANG		12115

// 主界面点击装备选项看到的界面
class MainLayerStoreDaoju : public MainLayerItemsBaseBody,public IMainLayerBody
{
private:

protected:
	virtual BaseSprite* getItemContentByIndex(unsigned int index);

	MainLayerBase* mFooterLayer;

	//价格
	std::vector<ItemInfo> mPriceVec;

	void rsp_BuyItems(CCObject* msg);
	void refreshMoneyItem();

	unsigned int mItemIndexToBuy;

public:
	virtual void onEnter();

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	void menuItemClicked_Buy(CCObject* obj);
	

	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx){return CCSize(622,144);}
	// 得到body的大小
	virtual CCSize getBodySize(){return CCSize(640, 842);};
	//
	virtual CCPoint getBodyCenterPos(){return ccp(320, mTableView->getContentSize().height/2);};

	MainLayerStoreDaoju(MainLayerBase* footLayer, CCSize contentSize);
	~MainLayerStoreDaoju(void);
};