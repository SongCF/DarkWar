#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"

using namespace cocos2d::extension;



class ShenBingShangDian : public BaseLayer , public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	ShenBingShangDian(MainLayerBase* footerLayer, CCSize contentSize);

protected:
	void menuItemClicked_DuanZao(CCObject *pSender);
	void rsp_DuanZao(CCObject *msg_f);

public:
	virtual void onEnter();
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
	void changePageIndex(CCObject* pObj);
	void changePageIndexEnd(int iPageIndex);
	///////////////////////////////////////////////////////////////////////////////////////////

protected:
	CCTableView *mTableView;
	MainLayerBase* mFooterLayer;

	vector<BaseActor*> mActors;
};
