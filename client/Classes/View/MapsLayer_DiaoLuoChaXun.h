#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include "MapsLayer.h"
#include "cocos-ext.h"
#include "flat.h"
#include "EquipmentTaoZhuangXiangQing.h"
using namespace cocos2d::extension;

class MapsLayer;

class MapsLayer_DiaoLuoChaXun : public BaseLayer//, public CCTableViewDataSource, public CCTableViewDelegate
	, public TaoZhuangLayerInterface
{
public:
	bool init(int zhangjie, MapsLayer *callbackLayer, int curDifficulty = difficult_normal);
	static MapsLayer_DiaoLuoChaXun* create(int curZhangJie, MapsLayer *callbackLayer, int curDifficulty);


	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_XuanZeMenu(CCObject *pSender);
	void menuItemClicked_zbClicked(CCObject *pSender);
	void menuItemClicked_equipDetailMenu(CCObject *pSender);
//	void menuItemClicked_taoZhuangFanhui(CCObject *pSender);

	void diaoluChaXun_Rsp(CCObject *msg_f);
	void menuItemClicked_ChaXunZB(CCObject *pSender);
	void refresh_ChaXunZB_ZhuangBei(BaseSprite* board, vector<Flat_FubenDiaoluoZBInfo>& diaoluoVec);
	void refresh_ChaXunZB_DaoJu(BaseSprite* board, vector<ItemsType>& daojuVec);


	//套装返回 回调
	virtual void callbackInterface_TaoZhuangLayer();
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin()
	{
		mMainMenu->setVisible(false);
		mChaXunMenu->setEnabled(false);
	}
	virtual void disableAllTouchEnd()
	{
		mMainMenu->setVisible(true);
		mChaXunMenu->setEnabled(true);
	}


	void refreshChaXunItem();
/*
public:

	// CCScrollViewDelegate
	virtual void scrollViewDidScroll(CCScrollView* view){}
	virtual void scrollViewDidZoom(CCScrollView* view){}

	// CCTableViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell){}

	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx){return CCSize(564, 49);}
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table){return 10;}
*/
protected:
	int mCurZhangJie;
	int mCurTask;
	int mCurDifficulty;
	Flat_QueryFubenDiaoluoRsp mCurTaskData;

	BaseSprite *mBg;
	CCMenu *mMainMenu;
	CCMenu *mChaXunMenu;

	MapsLayer *mCallbackLayer;    //mCallbackLayer->disableAllTouchEnd

//	CCTableView *mTableView;

	Flat_FubenDiaoluoZBInfo mCurTaoZhuang_Info;  //方便 套装查看， 当改装备是套装时才用， 在显示装备详情时赋值、 套装查看时使用
};