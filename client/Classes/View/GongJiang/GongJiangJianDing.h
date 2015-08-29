#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include "Equipment.h"
#include "MainLayerZhuangBeiBaseBody.h"
#include "GongJiang_EquipSelect.h"
#include "JieFengYinMapManager.h"

enum Gongjiang_Status
{
	gongjiang_jianding = 1,
};

class GongJiangXiYouZhuangBeiJianDing : public BaseLayer, public IMainLayerBody, public GongJiangEquipSelectCallbackInterface
{
public:
	bool init(int flag, Equipment *equip);
	static GongJiangXiYouZhuangBeiJianDing* create(int flag = gongjiang_jianding, Equipment *equip = NULL);

	void menuItemClicked_HeadMenu(CCObject *pSender);
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_addNeedEquip(CCObject *pSender);
	void menuItemClicked_WanChengItem(CCObject *pSender);
	void dlgBtnCallBack(CCObject* pObj);

	void initZhuangBei();

	void initCailiao(Equipment* info);
	void initCailiao(const Flat_DuanzaoGroupZBInfo* info);
	//////////////////////////////////////////////////////////////////////////////////
// 	void setSelectedEquipmentId(unsigned long long eqID);
// 	void setSelectedNeedEquipId(unsigned long long eqID);
	//select equip call back
	virtual void selectEquipmentCallback(unsigned long long eqId, Flag_SelectTo_GongJiang flag);
	///////////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin(){mMainMenu->setEnabled(false);}
	virtual void disableAllTouchEnd(){mMainMenu->setEnabled(true);}
	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}
	void QueryAssessWeaponRsp(CCObject* pObj);
	void AssessWeaponRsp(CCObject* pObj);
	void setJianDingBtnVisible(bool e);
	//////////////////////////////////////////////////////////////////////////////////////

protected:
	BaseSprite *mBg;
	CCMenu *mMainMenu;

	int mCurStatus;
	Equipment *mEquip;
	Equipment *mCailiaoEquip;

	string mErrorInfo;
};
