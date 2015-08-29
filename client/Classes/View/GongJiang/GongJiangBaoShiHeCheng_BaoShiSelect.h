#pragma once
#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
#include "MainLayerItemsBaseBody.h"
#include "GongJiangBaoShiHeCheng.h"

using namespace cocos2d::extension;

#define TAG_BaseBody_TABLE_ITEM         11112
#define TAG_BaseBody_TABLE_ITEM_Image   11113
#define TAG_BaseBody_LABLE_NAME			11114
#define TAG_BaseBody_LABLE_COUNT		11115	
#define TAG_BaseBody_LABLE_SHUXING      11116
#define TAG_BaseBody_MENU			    11117
#define TAG_BaseBody_TABLE_ITEM_DiWen	11118

// 主界面点击装备选项看到的界面
class GongJiangBaoShiHeCheng_BaoShiSelect 
	: public BaseLayer ,public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
{
private:
	GongJiangBaoShiHeCheng* mHechengLayer;

	BaseSprite* tabBar;

	CCMenu* mMenu;
	CCMenuItemImage* mMenuItemCancel;
	CCMenuItemImage* mMenuItemMakeSure;

	int selectedIndex;

protected:
	MainLayerBase* mFooterForPop;

	CCTableView* mTableView;
	vector<ItemInfo> mItemsVec;
	// CCTableView
	virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);

	// CCScrollViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell);

	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);

	// 刷新数据
	void setItemsData(vector<ItemInfo> itemsVec)
	{
		this->mItemsVec = itemsVec; 
		mTableView->reloadData();
	};
	vector<ItemInfo> getBaoShi();


	virtual BaseSprite* getItemContentByIndex(unsigned int index);

public:
	void menuItemCancelClicked(CCObject* obj);
	void menuItemMakeSureClicked(CCObject* obj);

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	void setTableViewPos(CCPoint pos){mTableView->setPosition(pos);};
	void setFooter(MainLayerBase* footerLayer)
	{
		mFooterForPop = footerLayer;
	}

	// 得到body的大小
	virtual CCSize getBodySize(){return CCSize(640, 842);};
	//
	virtual CCPoint getBodyCenterPos(){return ccp(320, tabBar->getContentSize().height/2);};

	GongJiangBaoShiHeCheng_BaoShiSelect(GongJiangBaoShiHeCheng* hechengLayer);
	~GongJiangBaoShiHeCheng_BaoShiSelect(void);

	static GongJiangBaoShiHeCheng_BaoShiSelect *create(GongJiangBaoShiHeCheng* hechengLayer);
};