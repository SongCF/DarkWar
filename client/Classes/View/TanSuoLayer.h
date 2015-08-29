#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "BaseActor.h"
#include "cocos-ext.h"
#include "cocos2d.h"
#include "ZhuShouChatLayer.h"
using namespace cocos2d;
using namespace cocos2d::ui;

class TanSuoLayer : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate,ZhuShouChatLayerCallBack
{
public:
	TanSuoLayer();


	virtual void onEnter();

	void teachGuide();
	virtual void ZhuShouchatEndCallBack();
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
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	void clickXunBao(CCObject *obj,TouchEventType type);

protected:
	void refreshDisconverTime(unsigned int count, unsigned int tansuoCount);
	void refreshDestroyFenyin(int allNum);

	void rsp_query(CCObject *msg);
	void rsp_tansuo(CCObject *msg);
	void rsp_jiefen(CCObject *msg);

	void menuItemClicked_TanSuo(CCObject *pSender);
	void menuItemClicked_JieFenHeroSelect(CCObject *pSender);

	void menuItemClicked_removeAwardLayer(CCObject *pSender);
	void showGetItem(float dt);

	void selectJieFengHero(CCObject *msg);
private:
	inline string getTimeForString(int time)
	{
		time = time % 360;
		CCString *str = NULL;
		int sec = time % 60;
		int min = time / 60;
		if (sec < 10)
		{
			str = CCString::createWithFormat("%d:0%d",min,sec);
		}
		else
		{
			str = CCString::createWithFormat("%d:%d",min,sec);
		}
		return str->getCString();
	}
	void refreshTime(float dt);
protected:
	BaseSprite *mBg;
	CCMenu *mMainMenu;

	bool m_bDiscoverAll;
	unsigned int mDestroyCount;

	vector<ItemInfo> mItems;
	vector<BaseActor*> mHeros;

	short				m_nFontSize;

	CCSprite			*m_sXunBaoSpr;

	short				m_nMaxCountDestroy;

	bool				m_bXunBaoMoved;

	bool				m_bIsJieFeng;

	vector<CCMenuItem*> m_lMenuItemList;

	int					m_nSelectProId;

	CCLabelTTF			*m_tRefreshTimeTTF;//刷新时间显示

	CCLabelTTF			*m_tHaixuTTF;

	int					m_nHadCount;//拥有的寻宝次数
	int					m_nRefreshTime;

};