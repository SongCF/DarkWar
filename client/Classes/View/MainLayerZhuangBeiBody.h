#pragma once
#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
#include "MainLayerZhuangBeiBaseBody.h"

using namespace cocos2d::extension;



// 装备界面的全部选项内容
class MainLayerZhuangBeiAllBody : public MainLayerZhuangBeiBaseBody
{
private:
	ZhuangBeiType mType;
public:
	MainLayerZhuangBeiAllBody(ZhuangBeiType type)
		:MainLayerZhuangBeiBaseBody(From_ZhuangBeiList)
	{
		mType = type;
		switch (mType)
		{
		case Type_All:
			setEquipmentData(MyselfManager::getManager()->getAllEquipments(Filter_All));
			break;
		case Type_WuQi:
			setEquipmentData(MyselfManager::getManager()->getAllEquipments(Filter_All,Filter_Type_WuQi));
			break;
		case Type_FangJu:
			setEquipmentData(MyselfManager::getManager()->getAllEquipments(Filter_All,Filter_Type_FangJu));
			break;
		case Type_ShiPin:
			setEquipmentData(MyselfManager::getManager()->getAllEquipments(Filter_All,Filter_Type_ShiPin));
			break;
		default:
			CCAssert(false,"");
			break;
		}
	}

	virtual void disableAllTouchEnd()
	{
		switch (mType)
		{
		case Type_All:
			setEquipmentData(MyselfManager::getManager()->getAllEquipments(Filter_All));
			break;
		case Type_WuQi:
			setEquipmentData(MyselfManager::getManager()->getAllEquipments(Filter_All,Filter_Type_WuQi));
			break;
		case Type_FangJu:
			setEquipmentData(MyselfManager::getManager()->getAllEquipments(Filter_All,Filter_Type_FangJu));
			break;
		case Type_ShiPin:
			setEquipmentData(MyselfManager::getManager()->getAllEquipments(Filter_All,Filter_Type_ShiPin));
			break;
		default:
			CCAssert(false,"");
			break;
		}
		MainLayerZhuangBeiBaseBody::disableAllTouchEnd();
	}

	~MainLayerZhuangBeiAllBody(void)
	{

	}
};

// 主界面点击装备选项看到的界面
class MainLayerZhuangBeiBody : public BaseLayer,public IMainLayerBody,public CCTableViewDataSource, public CCTableViewDelegate
{
private:
	BaseSprite* container;
	CCTableView* tableView;
	int selectedIndex;
	BaseLayer* selectedContaint;

	void itemClicked(CCObject* sender);

	// 创建第index页的内容
	BaseLayer* createContentAtIndex(unsigned int index);

	// CCTableView
	virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);

	// CCScrollViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell);


	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);

public:

	// 拦截，用于在这里再设置选项卡
	virtual void initFoot(MainLayerBase* footerLayer);

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	// 选中第几个
	void selectAtIndex(unsigned int index);
	void menuItemClicked(CCObject *pSender);
	//
	virtual void refreshLayer();

	// 得到body的大小
	virtual CCSize getBodySize(){return container->getContentSize();};
	//
	virtual CCPoint getBodyCenterPos(){return container->getPosition();};

	MainLayerZhuangBeiBody(void);
	~MainLayerZhuangBeiBody(void);
};

