#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "cocos2d.h" 
#include "SimpleAudioEngine.h" 
#include "cocos-ext.h"   
#include <queue>
USING_NS_CC;
USING_NS_CC_EXT; 



class LibaoLayer:public BaseLayer , public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	LibaoLayer(MainLayerBase* footerLayer, CCSize contentSize);

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

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	//领取
	void menuItemClicked_Li_Qu(CCObject *pSender);
	void menuItemClicked_wufaLi_Qu(CCObject *pSender);
	//购买后消息回调
	void rsp_ChargeGold(CCObject *msg);
	//计时器
	void createTable(float time);
	//获取所限时间
	string getXiaoshitime(int saletime);
	//是否超时
	bool   isguoleTime(int saletime);
	//更新数据
	void updateDataMarkitem(int isfirst);

	~LibaoLayer(void);

protected:
	CCTableView *mTableView;
	MainLayerBase* mFooterLayer;
	vector<MarketItem> GmLb;
	int Gmid;
	map<int,CCLabelTTF *> buynumap;
	map<int,CCTableViewCell *> cellmap;
	CCMenuItemImage *p;

	int mYuanShiJinBi;
	bool isgold;
	float mContentHeight;
	int tabelViewHeight;
};

