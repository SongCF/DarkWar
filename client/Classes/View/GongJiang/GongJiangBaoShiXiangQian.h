#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include "Equipment.h"
#include "GongJiang_EquipSelect.h"

enum Gongjiang_XiangQian_Status
{
	gongjiang_xiangqian = 1
	, gongjiang_caixie = 2
};

class GongJiangBaoShiXiangQian : public BaseLayer, public IMainLayerBody
	, public GongJiangEquipSelectCallbackInterface
{
public:
	bool init(int flag, Equipment *equip);
	static GongJiangBaoShiXiangQian* create(int flag = gongjiang_xiangqian, Equipment *equip = NULL);

	void menuItemClicked_HeadMenu(CCObject *pSender);
	void menuItemClicked_MainMenu(CCObject *pSender);

	void cleanBackGround();

	void refreshXiangQian_UI();
	void refreshCaiXie_UI();

	void XiangQian_Rsp(CCObject *msg);
	void ChaiXie_Rsp(CCObject *msg);

	void menuItemClicked_WanCheng(CCObject *pSender);

	//////////////////////////////////////////////////////////////////////////////////
//	void setSelectedEquipmentId(unsigned long long eqID);
	void setSelectedBaoshiId(unsigned int baoshiID);
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

	vector<int> Xiangjindex;
	vector<CCLabelTTF*> Labeltfarry;


protected:
	BaseSprite *mBg;
	CCMenu *mMainMenu;

	int mCurStatus;

	Equipment *mCurEquip;
	long equipid;
	std::string getBaoshiShuxing(unsigned int baoshi_type);
	unsigned int mCurBaoShiType;
	string mErrorInfo;

	//baoshi
	bool mHasEquip;
	bool mHasBaoshi;


	//从装备详情来到工匠
	bool mComeFromEquipDetail;
	Equipment *mComFromGJ_equip;

	int mCaiXieBaoShi_Num;//拆卸的宝石数量
	unsigned int mCaiXieBaoShi_Type[3]; //一个装备最多能镶嵌3个宝石
};
