#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"

class GongJiangBaoShiHeCheng : public BaseLayer, public IMainLayerBody
{
public:
	bool init();
	static GongJiangBaoShiHeCheng* create();

	void clearnBackGround();

	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_JiaJianBaoShi(CCObject *pSender);
	void BaoShiHeCheng_Rsp(CCObject *pSender);
	void menuItemClicked_HeChengChengGong(CCObject *pSender);

	/////////////////////////////////////////////////////////////////////////
	void setSelectedBaoshiTypeId(unsigned int baoshiType, int baoshiCount);
	//////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin(){mMainMenu->setEnabled(false);}
	virtual void disableAllTouchEnd(){mMainMenu->setEnabled(true);}

	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}
	//////////////////////////////////////////////////////////////////////////////////
	virtual void onEnter();

private:
	BaseSprite *mBg;
	CCMenu *mMainMenu;

	unsigned int mCurHeChengBaoShiType;
	unsigned int mCurNeedBaoShiType;
	unsigned int mCurHeChengBaoShiNum;

	string mErrorInfo;
};
