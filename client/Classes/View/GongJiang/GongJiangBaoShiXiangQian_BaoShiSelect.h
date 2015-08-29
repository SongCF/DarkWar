#pragma once
#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
#include "MainLayerItemsBaseBody.h"
#include "GongJiangBaoShiXiangQian.h"

using namespace cocos2d::extension;


// 主界面点击装备选项看到的界面
class GongJiangBaoShiXiangQian_BaoShiSelect : public MainLayerItemsBaseBody,public IMainLayerBody
{
private:
	GongJiangBaoShiXiangQian* mXiangqianLayer;

	BaseSprite* tabBar;

	CCMenu* mMenu;
	CCMenuItemImage* mMenuItemCancel;
	CCMenuItemImage* mMenuItemMakeSure;

	int selectedIndex;

protected:
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);

public:
	void menuItemCancelClicked(CCObject* obj);
	void menuItemMakeSureClicked(CCObject* obj);

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();


	// 得到body的大小
	virtual CCSize getBodySize(){return CCSize(640, 842);};
	//
	virtual CCPoint getBodyCenterPos(){return ccp(320, tabBar->getContentSize().height/2);};

	GongJiangBaoShiXiangQian_BaoShiSelect(GongJiangBaoShiXiangQian* xiangqianLayer);
	~GongJiangBaoShiXiangQian_BaoShiSelect(void);
	static GongJiangBaoShiXiangQian_BaoShiSelect* create(GongJiangBaoShiXiangQian* xiangqianLayer);
};