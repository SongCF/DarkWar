#pragma once
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "BaseActor.h"
#include "flat.h"
#include "ChatLayer.h"
#include "FX_CommonFunc.h"
#include "EquipmentTaoZhuangXiangQing.h"

using namespace FX_CommonFunc;

class ChuanQiLuLayer : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
	, public TaoZhuangLayerInterface, public ChatLayerCallBack
{
public:
	ChuanQiLuLayer();


public:
	virtual void onEnter();

	void addDiaoLuo(vector<ShenbingSuipianInfo> vec); // 战斗回来、 如果有掉落  则谈对话框提示
protected: vector<ShenbingSuipianInfo> mVec;
		   void schedule_displayDiaoLuo(float dt);
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}

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
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	//套装返回 回调
	virtual void callbackInterface_TaoZhuangLayer();
	//
	virtual void chatEndCallBack();
/////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	void menuItemClicked_ListBeforeNext(CCObject *pSender);
	void menuItemClicked_HeroClicked(CCObject *pSender);

	void menuItemClicked_zbClicked(CCObject *pSender);
	void menuItemClicked_equipDetailMenu(CCObject *pSender);
	void menuItemClicked_Challenge(CCObject *pSender);
	void menuItemClicked_HeCheng(CCObject *pSender);

	int getCardPositionIndexByZhuangBeiType(ZhuangbeiType type,bool hasDunpai,bool isFirstJiezhi);//通过装备类型获取装备卡片的位置
	void refreshEquipContentLayer();
	void displayTaoZhuang(Flat_DuanzaoGroupZBInfo zbInfo); // 能升级的传奇装备
	void displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo); // 装备详情

	void callfunc_removeFromParent(CCNode *node);

	void rsp_QueryChuanqiLu(CCObject *msg_f);
	void rsp_Challenge(CCObject *msg_f);

	CCPoint getPositionByIdx(int idx, int allCount);

	void teachGuide();
	void schedule_guide(float dt);

protected:
	static Profession s_ProfId;//保存战斗时 选的哪个prof， 回来后还在这里

	BaseSprite *mBg;
	CCSprite *mEqContentLayer;
	CCTableView *mTableViewHeroList;
	CCMenu *mHeroListMenu; //before  next
	CCLabelTTF *mTimeLabel;  //剩余次数

	//切换 头像上的边框选中情况
	unsigned int mCurrCellIdx;
	unsigned int mOldCellIdx;

	int mTimes;
	vector<vector<Flat_DuanzaoGroupZBInfo> > mEquipVec;
	vector<BaseActor*> mHeros;

	//套装详情
	Flat_DuanzaoGroupZBInfo mCurTaoZhuang_Info;  //方便 套装查看， 当改装备是套装时才用， 在显示装备详情时赋值、 套装查看时使用
	

	CCMenuItem *mChallengeMenuItem_forGuide; //主角第一次教学用。
};