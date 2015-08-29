
#pragma once

#include "cocos2d.h"
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "flat.h"
#include "cocos-ext.h"
#include "ZhuShouChatLayer.h"
using namespace cocos2d::extension;

using namespace cocos2d;

class XueSeJiaoTangLayer : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate,ZhuShouChatLayerCallBack
{
public:
	bool init();
	static XueSeJiaoTangLayer* create();


	///////////////////////////////////////////////////////////////////////////////////////
	//footlayer
	virtual void disableAllTouchBegin(){};
	virtual void disableAllTouchEnd(){};
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();};
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return mBg->getPosition();};
	///////////////////////////////////////////////////////////////////////////////////////
public:
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
	///////////////////////////////////////////////////////////////////////////////////////
	virtual void ZhuShouchatEndCallBack();
protected:
	int getCurrentDifficult();

	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_PaiMingMenu(CCObject *pSender);
	void menuItemClicked_ShuoMingMenu(CCObject *pSender);

	void rsp_queryPaiMing(CCObject *msg);

protected:
	BaseSprite* mBg;
	CCTableView *mTableView;

	vector<SelfChlgOrder> mOrder;
};