
#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "Equipment.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;



class HeroRankList : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	//flag (0 list   1 战报    10直接查看阵容) 
	HeroRankList(int flag=0, unsigned int heroId=0);   

protected:
	void delaySchedule(float dt);
	bool canChallenge(unsigned int beChallengerOrder, unsigned int challengerOrder);

	void clearBg();
	void firstInWaiting(float dt);
	void firstComeInWithAnimate();
	void callfuncActionRemoveFromParent(CCNode* node);
	void displayRankList();
	void displayReport();

	void menuItemClicked_HeadMenu(CCObject* pSender);
	void menuItemClicked_Challenge(CCObject* pSender);
	void menuItemClicked_Query(CCObject *pSender);
	void menuItemClicked_ReportBarMenu(CCObject *pSender);

	BaseSprite* getTableCellCountent(BaseSprite* bg, int idx);

	void rsp_TopHeroList(CCObject* msg_f);
	void rsp_topHeroChallege(CCObject* msg_f);
	void queryAnotherTeamInfoRsp(CCObject* msg_f);
	void queryAnotherTeamYuanshenRsp(CCObject *msg_f);

	int getOrderByHeroId(unsigned int id);

	void schedule_updateChlgTime(float dt);
	void menuItemClicked_ClearTime(CCObject *pSender);
	void rsp_clearCDTime(CCObject *msg);

public:
	void teachGuide();//教学使用
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return mBg->getPosition();}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CCScrollViewDelegate
	virtual void scrollViewDidScroll(CCScrollView* view){}
	virtual void scrollViewDidZoom(CCScrollView* view){}

	// CCTableViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell){}

	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	CCTableView* mTableView;
	//
	vector<HeroAndOrder> mHeroVec;
	vector<TopHeroReport> mReport;
	int mMyOrder;

	unsigned int mNextChlgTime;

	bool mFirstIn;
	unsigned int mClickedHeroId; //查看别人装备 所点击的那个cell



	//查询角色详细面板时使用。
	int mNum_YuanShen;
	CCArray *mQueryHeroActors;
	CCArray *mQueryYuanShen; //actor在setYuanshen时没有retain，所以要自己保存并释放（和equipment一样）
	vector<Equipment*> mQueryActorsEquip; //

	BaseSprite* mBg;
	CCMenu* mHeadMenu;

	int mFlag;
	//挑战结算回来才会用的  构造函数参数
public: static unsigned int s_ChallengeHeroId;
		static unsigned int s_ChallengeOrder;
};