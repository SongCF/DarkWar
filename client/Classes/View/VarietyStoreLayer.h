#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"


class VarietyStoreLayer : public BaseLayer, public IMainLayerBody
{
public: 
	CREATE_FUNC(VarietyStoreLayer);
	bool init();


	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onEnter();
	virtual void onExit();
protected:
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_ChoseCard(CCObject *pSender);
	void menuItemClicked_ok(CCObject *pSender);

	void menuItemClicked_CaiLiaoShuoMingEnd(CCObject *pSender);

	void schedule_ShuffleCard(float dt);
	void callback_shuffleEnd();

	void rsp_QueryZaHuoPu(CCObject *msg);
	void rsp_RefreshItems(CCObject *msg);
	void rsp_GetOneItem(CCObject *msg_f);

	void calback_refreshNeedDaiGouJuan();

	///////////////////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin(){mMainMenu->setEnabled(false);mCardMenu->setEnabled(false);}
	virtual void disableAllTouchEnd(){mMainMenu->setEnabled(true);mCardMenu->setEnabled(true);}
	// 得到body的大小
	virtual CCSize getBodySize(){return CCSize(mBg->getContentSize().width, mBg->getContentSize().height + mHead->getContentSize().height);}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2+mHead->getContentSize().height/2);}
	///////////////////////////////////////////////////////////////////////////////////////////

protected:
	BaseSprite *mBg;
	BaseSprite *mHead;
	CCMenu *mMainMenu;
	CCMenu *mCardMenu;

	bool mCurStatus;  //正在抽取true      一个都没抽false

	CCMenuItemImage *mChoseItem;
};