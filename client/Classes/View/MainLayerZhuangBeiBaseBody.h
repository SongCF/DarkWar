#pragma once

#include "BaseLayer.h"
#include "cocos-ext.h"
#include "Equipment.h"
#include "MyselfManager.h"
#include "ImageManager.h"
#include "EquipmentXiangQingBody.h"

using namespace cocos2d::extension;

#define TAG_EquipBaseBody_TABLE_ITEM 11112
#define TAG_BaseBody_TABLE_ITEM_Image   11113
#define TAG_BaseBody_LABLE_NAME			11114
#define TAG_BaseBody_LABLE_OWNTO			11115	// 装备给了谁
#define TAG_BaseBody_MENU			11116	// 
#define TAG_BaseBody_TABLE_ITEM_DiWen			11117	// 
#define TAG_BaseBody_LABLE_TYPE         1118
#define TAG_BaseBody_LABLE_BAIZI         1119
#define TAG_BaseBody_LABLE_SHANGHAIXIANGXI_NUM        11120
#define TAG_BaseBody_LABLE_SHANGHAIXIANGXI_TXT         11121
#define TAG_BaseBody_LABLE_Level         11122
#define TAG_BaseBody_LABLE_JUNSHANG_TXT         11123
#define TAG_BaseBody_TABLE_FengYin_Img          11124


const int SORT_IN_GongJiang  = 4643+150 + 0;
const int SORT_IN_ZhuangBei  = 4643+150 + 1;
const int SORT_IN_HuanZhuang = 4643+150 + 2;
const int SORT_IN_FenJie     = 4643+150 + 3;


class MainLayerBase;

// 装备界面的全部选项内容
class MainLayerZhuangBeiBaseBody : public BaseLayer , public CCTableViewDataSource, public CCTableViewDelegate
{
protected:
	ZhuangbeiXiangQingFrom mFrom;

	MainLayerBase* mFooterForPop;

	CCTableView* mTableView;
	vector<Equipment*> mEquipmentVec;

	//temp.initWithFormat("%s: %u/%u", LFStrings::getValue("zhuangbei").c_str(), mEquipmentVec.size(), MAX_COUNTS_IN_KNAPSACK);
	CCLabelTTF *m_zbTotalLabel;
	CCLabelTTF *m_zbFenJieTiShi;
	CCMenu *m_zbFenJieMenu;

	int mClickedIdx;
	CCPoint mOffset;
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
	void setEquipmentData(vector<Equipment*> equipmentVec, int flag = SORT_IN_ZhuangBei, int specialEquip_GroupId = 0);
	void sortEquipment(int flag);
	virtual BaseSprite* getItemContentByIndex(unsigned int index);

	virtual void itemImageClicked(CCObject* obj);
	void menuitemClicked_GotoFenJie(CCObject *pSender);

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
public:
	virtual void onEnter();
	// from用于传给点击装备图片跳向的装备详细界面
	MainLayerZhuangBeiBaseBody(ZhuangbeiXiangQingFrom from);

	// 滚动到第几个选项，这个选项尽量的置顶
	void scrollToIndex(int index);
	// 获取 数据源
	const vector<Equipment*> getData(){return mEquipmentVec;};

	void setTableViewPos(CCPoint pos){mTableView->setPosition(pos);};
	void setFooter(MainLayerBase* footerLayer)
	{
		mFooterForPop = footerLayer;
	}
	~MainLayerZhuangBeiBaseBody(void);
};

enum ZhuangBeiType
{
	Type_All = 0,
	Type_WuQi,
	Type_FangJu,
	Type_ShiPin
};

