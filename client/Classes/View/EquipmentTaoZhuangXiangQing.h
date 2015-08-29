#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"


//interface
class TaoZhuangLayerInterface
{
public:
	virtual void callbackInterface_TaoZhuangLayer() = 0;
};


class EquipmentTaoZhuangXiangQing : public BaseLayer, public IMainLayerBody
{
public:
	//只穿这一件equipedGroupIdVec.size == 1  和  没有穿它equipedGroupIdVec.size == 0
	EquipmentTaoZhuangXiangQing(unsigned int curGroupId, string curName, vector<unsigned int> equipedGroupIdVec, 
		TaoZhuangLayerInterface *callfunc=NULL);

public:
	virtual void onEnter();
//////////////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin(){mMenu->setEnabled(false);}
	virtual void disableAllTouchEnd(){mMenu->setEnabled(true);}
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return mBg->getPosition();}
//////////////////////////////////////////////////////////////////////////////////////////////////////////


protected:
	void menuItemClicked_MainMenu(CCObject *pSender);

private:
	BaseSprite *mBg;
	CCMenu *mMenu;

	TaoZhuangLayerInterface *mCallfunc;

	int				m_nShuxingHeight;
};