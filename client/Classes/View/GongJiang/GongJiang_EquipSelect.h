
//@fx   2014/3/21

#pragma once
#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
#include "MainLayerZhuangBeiBaseBody.h"

using namespace cocos2d::extension;

enum Flag_SelectTo_GongJiang
{
	selectTo_QiangHua,
	selectTo_HuiTui,

	selectTo_JingLian,
	selectTo_JingLianNeed,
	selectTo_ZhuanKe,
	selectTo_JianDingNeed,

	selectTo_FenJie,

	selectTo_BaoShiXiangQian,
	selectTo_BaoShiCaiXie,
};


class GongJiangEquipSelectCallbackInterface
{
public:
	//
	virtual void selectEquipmentCallback(unsigned long long eqId, Flag_SelectTo_GongJiang flag) = 0;
};


class GongJiang_EquipSelect : public MainLayerZhuangBeiBaseBody,public IMainLayerBody
{
public:
	static GongJiang_EquipSelect* create(GongJiangEquipSelectCallbackInterface* callbackLayer, 
		Flag_SelectTo_GongJiang flag, Equipment *equip=NULL);

	GongJiang_EquipSelect(GongJiangEquipSelectCallbackInterface* callbackLayer, 
		Flag_SelectTo_GongJiang flag, Equipment *equip=NULL);

protected:
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
	virtual BaseSprite* getItemContentByIndex(unsigned int index);

public:
	void menuItemCancelClicked(CCObject* obj);
	void menuItemMakeSureClicked(CCObject* obj);

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();


	virtual void initFoot(MainLayerBase* footerLayer)
	{
		mFooterLayer = footerLayer;
		setFooter(footerLayer);
	}
	// 得到body的大小
	virtual CCSize getBodySize(){return CCSize(640, 842);}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(320, tabBar->getContentSize().height/2);}

private:
	void addTeach();
	void select();
private:
	GongJiangEquipSelectCallbackInterface* mSetEquipCallback;

	BaseSprite* tabBar;
	Flag_SelectTo_GongJiang mSelectTo;

	CCMenu* mMenu;
	CCMenuItemImage* mMenuItemCancel;
	CCMenuItemImage* mMenuItemMakeSure;

	int selectedIndex;

};