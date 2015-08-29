#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include "Equipment.h"
#include "GongJiang_EquipSelect.h"


enum Gongjiang_JingLian_Status
{
	gongjiang_jinglian = 1
	, gongjiang_zhuanke = 2
};

class GongJiangJingLianZhuanKe : public BaseLayer, public IMainLayerBody, public GongJiangEquipSelectCallbackInterface
{
public:
	bool init(int flag, Equipment *equip);
	static GongJiangJingLianZhuanKe* create(int flag = gongjiang_jinglian, Equipment *equip = NULL);

	void menuItemClicked_HeadMenu(CCObject *pSender);
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_addNeedEquip(CCObject *pSender);
	void menuItemClicked_WanChengItem(CCObject *pSender);

	void cleanBackGround();

	void refresh_jingLianUI();
	void refresh_zhuanKeUI();
	void rsp_JingLian(CCObject *msg);
	void rsp_ZhuanKe(CCObject *msg);

	void menuItemClicked_WanCheng(CCObject *pSender);

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
	//////////////////////////////////////////////////////////////////////////////////////

	virtual void onEnter();
	BaseSprite *currentsp;
	int isAnimation;
	void Qianhuadh(BaseSprite *xingxing);
	void callfunc_AnimateEnd(CCArmature *, MovementEventType, const char *);

	~GongJiangJingLianZhuanKe();
protected:
	BaseSprite *mBg;
	CCMenu *mMainMenu;

	int mCurStatus;
	Equipment *mEquip;
	Equipment *mCailiaoEquip;

	string mErrorInfo;


	//从装备详情来到工匠
	bool mComeFromEquipDetail;
	Equipment *mComFromGJ_equip;
};
