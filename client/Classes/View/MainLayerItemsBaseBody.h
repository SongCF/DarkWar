#pragma once

#include "BaseLayer.h"
#include "cocos-ext.h"
#include "Equipment.h"
#include "MyselfManager.h"
#include "ImageManager.h"
#include "EquipmentXiangQingBody.h"

using namespace cocos2d::extension;

#define TAG_BaseBody_TABLE_ITEM         11112
#define TAG_BaseBody_TABLE_ITEM_Image   11113
#define TAG_BaseBody_LABLE_NAME			11114
#define TAG_BaseBody_LABLE_COUNT		11115
#define TAG_BaseBody_LABLE_COUNT_NUM	11116
#define TAG_BaseBody_LABLE_SHUXING      11117
#define TAG_BaseBody_MENU			    11118
#define TAG_BaseBody_TABLE_ITEM_DiWen	11119


class MainLayerBase;

// 物品界面的全部选项内容
class MainLayerItemsBaseBody : public BaseLayer , public CCTableViewDataSource, public CCTableViewDelegate
{
protected:
	MainLayerBase* mFooterForPop;

	CCTableView* mTableView;
	vector<ItemInfo> mItemsVec;
	vector<GiftBag>  giftbags;
	int id;
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

protected:
	// 刷新数据
	virtual void setItemsData(vector<ItemInfo> itemsVec)
	{
		this->mItemsVec = itemsVec; 
		mTableView->reloadData();
	};
	

	virtual BaseSprite* getItemContentByIndex(unsigned int index);

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
public:
	MainLayerItemsBaseBody(void);

	void setFooter(MainLayerBase* footerLayer)
	{
		mFooterForPop = footerLayer;
	}
	~MainLayerItemsBaseBody(void);
};

