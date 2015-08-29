
#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;

//客户端直接写死。

class VipExplainLayer : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
//	static VipExplainLayer* create();
//	virtual bool init();
	VipExplainLayer();
	~VipExplainLayer();

protected:
	void menuItemClicked_MainMenu(CCObject *pSender);
	CCSize getCellSizeByIdx(int idx);
	CCString getCellTitleByIdx(int idx);
	CCString getCellContentStringByIdx(int idx);

public:
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return CCSize(mWinSize.width, mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height);} // 106是底的高度
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return ccp(mWinSize.width/2, (mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height)/2);}
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


protected:
	CCTableView* mTableView;

	CCMenu *mMainMenu;

};