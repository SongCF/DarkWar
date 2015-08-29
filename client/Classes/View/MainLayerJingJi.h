#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "flat.h"
#include "Equipment.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;


enum 
{
	model_Challenge = 1,
	model_random = 2,
	model_store = 2,
};

class MainLayerJingJi : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	bool init(int flag);
	static MainLayerJingJi* create(int flag = model_Challenge);

protected:
	void initLayerData(float dt);
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_BuyResedueNum(CCObject *pSender);
	void menuItemClicked_BuyNumMakeSure(CCObject *pSender);
	void menuItemClicked_BeginRandomChallenge(CCObject *pSender);
	void menuItemClicked_RefreshList(CCObject *pSender);

	void menuItemClicked_Challenge(CCObject *pSender);
	void menuItemClicked_HeroImg(CCObject *pSender);
	void menuItemClicked_MarketBuy(CCObject *pSender);
	void menuItemClicked_MarketOkCancel(CCObject *pSender);
	void menuItemClicked_MarketAddSub(CCObject *pSender);
	void menuItemClicked_MarketOkEnd(CCObject *pSender);

	void menuItemClicked_BackRandomLayer(CCObject *pSender);

	void rsp_QueryChallengeList(CCObject *msg_f);
	void rsp_RefreshChallengeList(CCObject *msg_f);
	void rsp_Challenge(CCObject *msg_f);
	void rsp_BuyChallengeTime(CCObject *msg);
	void rsp_QueryMarketList(CCObject *msg_f);
	void rsp_MarketBuyEnd(CCObject *msg);

	void rsp_QueryAnotherTeamInfo(CCObject *msg_f);
	void rsp_queryAnotherTeamYuanshen(CCObject *msg_f);

///////////////////////////////////////////////////////////////////////////////////////////
	virtual void onEnter(); //教学
	///////////////////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}
	///////////////////////////////////////////////////////////////////////////////////////////
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
	/////////////////////////////////////////////////////////////////////////////////////////////////////////


protected:
	CCMenu * mMainMenu;
	BaseSprite *mBg;
	CCTableView *mTableView;

	int mCurFlag;

	vector<ArenaChlgerHeroInfo> mChaHeroList;
	unsigned int mChlgRefreshNeedGoldNum;

	vector<ArenaMarketItem> mMarketList;
	int mIdx_BuyItem;
	int mMatketBuyItem_Num;
	CCLabelTTF *mMarketNumLabel;
	CCLabelTTF *mMarketBuyNeedNum;
	CCMenuItem *_addItem, *_subItem;

	//查询角色详细面板时使用。
	int mNum_YuanShen;
	CCArray *mQueryHeroActors;
	CCArray *mQueryYuanShen; //actor在setYuanshen时没有retain，所以要自己保存并释放（和equipment一样）
	vector<Equipment*> mQueryActorsEquip; //

};