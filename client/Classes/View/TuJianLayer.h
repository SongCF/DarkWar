#pragma once
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "flat.h"
//#include "MyMenu.h"
#include "FX_CommonFunc.h"
#include "EquipmentTaoZhuangXiangQing.h"

using namespace FX_CommonFunc;

class TuJianLayer : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
	, public TaoZhuangLayerInterface
	//, public MyMenuCallBackInterface
{
public:
	TuJianLayer();
	~TuJianLayer();


public:
	virtual void onEnter();
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//MyMenuCallBackInterface
	//virtual void menuItemLongTimeTouched(CCObject* pSender){}
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
/////////////////////////////////////////////////////////////////////////////////////////////////////
	inline void setChatGuide()
	{
		m_bIsChatGuide = true;
	}
protected:
	void rsp_getEquipAlbumData(CCObject *msg_f);

	void menuItemClicked_ListBeforeNext(CCObject *pSender);
	void menuItemClicked_HeroClicked(CCObject *pSender);

	void menuItemClicked_DengJiBoardItem(CCObject *pSender);

	void menuItemClicked_zbClicked(CCObject *pSender);
	void displayTaoZhuang(Flat_DuanzaoGroupZBInfo zbInfo); // 能升级的传奇装备
	void displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo); // 装备详情

	void menuItemClicked_equipDetailMenu(CCObject *pSender);
//	void menuItemClicked_taoZhuangFanhui(CCObject *pSender);

	void callfunc_removeFromParent(CCNode *node);
private:
	inline CCPoint getScencePoint(CCNode *node,CCPoint pos)
	{
		CCPoint point = CCPointZero;
		/*
		if (node->getParent() && node->getParent() != CCDirector::sharedDirector()->getRunningScene())
		{
			point = getScencePoint(node->getParent(),node->getParent()->convertToWorldSpace(pos));
		}
		else
		{
			point = pos;
		}*/
		while (node->getParent()  && node->getParent() != CCDirector::sharedDirector()->getRunningScene())
		{
			node = node->getParent();
			point = node->convertToWorldSpace(pos);
			pos = point;
		}
		return point;
	}

	void playGuideLayer(CCMenuItem *menuItem);
	void goToTheLayer(CCObject* pObj);
	void rsp_Fight(CCObject* pObj);
	void QueryEquipFallRsp(CCObject* msg_rsp);
protected:
	BaseSprite *mBg;
	CCTableView *mTableView;
	CCTableView *mTableViewHeroList;
	CCMenu *mHeroListMenu;

	bool mComeIn;

	//切换 头像上的边框选中情况
	unsigned int mCurrCellIdx;
	unsigned int mOldCellIdx;
//	CCMenuItemImage *mCurHeroMenuItem;

	//图鉴消息
	vector<Flat_DuanzaoGroupZBInfo> mChuanqiSuitData;
	// 单件稀有
	vector<Flat_DuanzaoGroupZBInfo> mXiYouVec;

	//套装详情
	Flat_DuanzaoGroupZBInfo mCurTaoZhuang_Info;  //方便 套装查看， 当改装备是套装时才用， 在显示装备详情时赋值、 套装查看时使用

	//稀有装详情
	Flat_DuanzaoGroupZBInfo mCur_Info;		//点击稀有装后赋值，用于存放当前点击的稀有装信息
	bool			m_bIsChatGuide;
	CCMenuItem		*m_iGuideLayerDiwenItem;
	CCTableViewCell *m_tTableViewCell;
	CCTableView		*m_tTableViewTable;

	CCMenu			*m_mGuideLayerMenu;
};