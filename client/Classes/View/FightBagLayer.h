
#pragma once
 
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "Equipment.h"
#include "WuPinManager.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;

struct Bag_WuPinInfo
{
	unsigned int type;
	unsigned int count;

	bool isSuiPian;
	unsigned int suipian_id;
	Bag_WuPinInfo(unsigned int type_in=0, unsigned int count_in=0, bool isSuiPian_in=false, unsigned int suipian_is_in=0)
	{
		type = type_in;
		count = count_in;
		isSuiPian = isSuiPian_in;
		suipian_id = suipian_is_in;
	}
};

class BagCallBack
{
public:
	virtual void disableAllTouch() = 0;
	virtual void enableAllTouch() = 0;
};

class FightBagLayer : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	static FightBagLayer* create(vector<Equipment> equips, vector<Bag_WuPinInfo> wupin, BagCallBack *callback, bool isFooterPop=false);
	bool init(vector<Equipment> equips, vector<Bag_WuPinInfo> cailiao, BagCallBack *callback, bool isFooterPop=false);

	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_ItemImg(CCObject *pSender);

	BaseSprite *getEquipCellBody(int idx);
	BaseSprite *getWupinCellBody(int idx);
	/////////////////////////////////////////////////////////////////////////////
	// CCScrollViewDelegate
	virtual void scrollViewDidScroll(CCScrollView* view){}
	virtual void scrollViewDidZoom(CCScrollView* view){}
	// CCTableViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell){}
	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);
	///////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin(){mMainMenu->setEnabled(false); mTableView->setTouchEnabled(false);}
	virtual void disableAllTouchEnd(){mMainMenu->setEnabled(true); mTableView->setTouchEnabled(true);}

	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return CCPoint(mBg->getPositionX(), mBg->getPositionY());}
	//////////////////////////////////////////////////////////////////////////////////////////
	virtual void onEnter();

protected:
	CCTableView *mTableView;
	int mCurStatus;

	CCMenu *mMainMenu;

	BaseSprite *mBg;

	vector<Equipment> mEquips;
	vector<Bag_WuPinInfo> mWupins;

	BagCallBack *mCallback;
	bool m_isFooterPop;
};