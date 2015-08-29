
#pragma once

#include "cocos2d.h"
#include "LFAlert.h"
   
using namespace cocos2d;

class GetTiLiCallBack
{
public:
	virtual void getTiliCallBackFunc() = 0;
};

class GetTiLiLayer : public LFAlert
{
public:
	GetTiLiLayer(GetTiLiCallBack *callback, bool notEnough);
	~GetTiLiLayer();

	static GetTiLiLayer *create(GetTiLiCallBack *callback=NULL, bool notEnough=true);


	void menuItemClicked_BuyOrClose(CCObject* pSender);
	void menuItemClicked_UpVip(CCObject *pSender);
	void huifuTiLi(CCObject* msg);

protected:

	CCLabelTTF* mLabel;
	CCMenu* mMenu;

	GetTiLiCallBack *mCallBack;
};
