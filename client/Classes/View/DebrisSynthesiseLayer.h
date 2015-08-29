#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include "SuiPianManager.h"
#include "cocos-ext.h"

using namespace cocos2d::extension;

enum 
{
	flag_material,
	flag_book,
	flag_equip,
};

class DebrisSynthesiseLayer : public BaseLayer, public IMainLayerBody
	, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	static DebrisSynthesiseLayer* create();
	bool init();
	DebrisSynthesiseLayer();

protected:
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_MainBack(CCObject *pSender);
	void menuItemClicked_PageChange(CCObject *pSender);
	void menuItemClicked_SelectItem(CCObject *pSender);
	void menuItemClicked_SelectEquip(CCObject *pSender);
	void menuItemClicked_SynthesiseOrBack(CCObject *pSender);
	void menuItemClicked_rspBack(CCObject *pSender);

	void refreshContentDebris();

	void rsp_heCheng(CCObject *msg);
	void heChengEnd_display(CCNode *node, void *data);
	void action_removeFromParent(CCNode *node);

public:
	virtual void onEnter();  //教学
	CCSprite *mGuide_Sprite; //教学专用
	/////////////////////////////////////////////
	virtual void disableAllTouchBegin(){mMainMenu->setEnabled(false);}
	virtual void disableAllTouchEnd(){mMainMenu->setEnabled(true);}
	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	virtual CCPoint getBodyCenterPos(){return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}
//////////////////////////////////////////////
	// CCTableView
	virtual void scrollViewDidScroll(CCScrollView* view){}
	virtual void scrollViewDidZoom(CCScrollView* view){}
	// CCScrollViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell){}
	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);
	/////////////////////////////////////////////////////////////////////////////

protected:
	BaseSprite *mBg;
	CCMenu *mMainMenu;
	int mCurFlag;

	vector<ShenbingSuipianGroupData> mCurShenBingVec;
	int mSelEquipIdx;
	vector<ItemSuipianGroupData> mCurItemVec;
	int mSelItemIdx;

	CCTableView *mTableView;
};