#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"

using namespace cocos2d::extension;



class ChongZhiLayer : public BaseLayer , public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	ChongZhiLayer(MainLayerBase* footerLayer, CCSize contentSize);

	void menuItemClicked_GouMai(CCObject *pSender);
	void menuItemClicked_QueryVip(CCObject *pSender);
	void rsp_ChargeGold(CCObject *msg);

	virtual void onEnter();
protected:
	///////////////////////////////////////////////////////////////////////////////////////////
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
	///////////////////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	///////////////////////////////////////////////////////////////////////////////////////////

protected:
	CCTableView *mTableView;
	MainLayerBase* mFooterLayer;


	int mYuanShiJinBi;

	bool mHasCharged;
	float mContentHeight;
};

