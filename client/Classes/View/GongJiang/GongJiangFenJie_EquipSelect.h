#pragma once
#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
#include "MainLayerZhuangBeiBaseBody.h"
#include "GongJiangFenJie.h"

using namespace cocos2d::extension;


// 主界面点击装备选项看到的界面
class GongJiangFenJie_EquipSelect : public MainLayerZhuangBeiBaseBody,public IMainLayerBody
{
private:
	GongJiangFenJie* mChaijieLayer;

	BaseSprite* tabBar;

	CCMenu* mMenu;
	CCMenuItemImage* mMenuItemCancel;
	CCMenuItemImage* mMenuItemMakeSure;

	//int selectedIndex;
	map<unsigned long long,Equipment*> selectedEqList;

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
	virtual CCSize getBodySize(){return CCSize(640, 842);};
	//
	virtual CCPoint getBodyCenterPos(){return ccp(320, tabBar->getContentSize().height/2);};

	GongJiangFenJie_EquipSelect(GongJiangFenJie* chaijieLayer);
	~GongJiangFenJie_EquipSelect(void);

	static GongJiangFenJie_EquipSelect* create(GongJiangFenJie* chaijieLayer);
};