#pragma once

#include "LoginGiftLayer.h"

class SystemGiftLayer : public LoginGiftLayer
{
public:
	static SystemGiftLayer * create();
	virtual bool init();

/*	virtual void menuItemClicked_MainMenu(CCObject *pSender);*/
	virtual void menuItemClicked_GetGift(CCObject *pSender);
	virtual void rsp_getGift(CCObject *msg_f);

	inline void setCallBakc(CCObject *obj,SEL_CallFunc resetDengJi)
	{
		m_oTarger = obj;
		m_hResetDengJi = resetDengJi;
		//m_hResetDengJi = resetDengJi;
	}
public:
// 	virtual void disableAllTouchBegin();
// 	virtual void disableAllTouchEnd();
	// 得到这个Body的Size
// 	virtual CCSize getBodySize(){return mFooterLayer->getMaxContentBodySize();}
// 	// 获得body的中点位置
// 	virtual CCPoint getBodyCenterPos() {return ccp(mFooterLayer->getMaxContentBodySize().width/2, mFooterLayer->getMaxContentBodySize().height/2);}
public:
	// CCTableViewDataSource
//	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
//	virtual unsigned int numberOfCellsInTableView(CCTableView *table);

protected:
// 	CCTableView* mTableView;
// 	//	BaseSprite *mBg;
// 
// 	CCMenu *mMainMenu;
// 
// 	vector<Flat_BonusInfo> mGiftsVec;
private:
	void sortLevelGif();

private:
	CCObject				*m_oTarger;
	SEL_CallFunc			m_hResetDengJi;
};