#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include "Equipment.h"
#include <map>
#include "ZhuShouChatLayer.h"

class GongJiangFenJie : public BaseLayer, public IMainLayerBody,ZhuShouChatLayerCallBack
{
public:
	bool init(Equipment *equip);
	static GongJiangFenJie *create(Equipment *equip = NULL);

	void menuItemClicked(CCObject *pSender);
	void menuItemClicked_Auto(CCObject *pSender);
	void menuItemClicked_fenjie_OkCancel(CCObject *pSender);
	void setSelectedEquipmentId(map<unsigned long long,Equipment*> eqVec);
	void rsp_zhuangbeiFenJie(CCObject *msg);


	void clearnBackGround(int equipNum);


	virtual void onEnter();
	//////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}
	//////////////////////////////////////////////////////////////////////////////////
	virtual void ZhuShouchatEndCallBack();
private:

	BaseSprite *mBg;
	CCMenu *mMenu;

	vector<Equipment*> mCurrentNeedFenjieEquipFromSelListVec;
	//Equipment *mCurEquip;
	bool mAuto_PuTong;//自动分解普通装
	bool mAuto_FuMo;//自动分解附魔装


	//从装备详情来到工匠
	bool mComeFromEquipDetail;
	Equipment *mComFromGJ_equip;

	bool				m_bIsHadXiLianJingHua;
};
