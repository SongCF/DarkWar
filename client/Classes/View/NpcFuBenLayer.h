
#pragma once

#include "cocos2d.h"
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "TaskManager.h"
#include "MainLayerBase.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;

using namespace cocos2d;

class NpcFuBenLayer : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	NpcFuBenLayer();
	~NpcFuBenLayer();

	static NpcFuBenLayer* create();

	void fubenMenuCallback(CCObject* pSender);


	//////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////


	void setAllTouchesDisable();
	void setAllTouchesEnable();
	virtual void disableAllTouchBegin(){setAllTouchesDisable();}
	virtual void disableAllTouchEnd(){setAllTouchesEnable();}
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return mBg->getPosition();}

protected:
	//vector<SubTaskItem> mSubTaskVec;


//	CCLayer* mLayer;
	CCTableView *mTableView;
	BaseSprite *mBg;
};