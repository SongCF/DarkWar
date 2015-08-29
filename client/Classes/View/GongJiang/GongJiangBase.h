#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include "ZhuShouChatLayer.h"
enum GongJiangFlag
{
	  flag_BaoShi_HeCheng = 1
	, flag_BaoShi_XiangQian
//	, flag_CuiQu_FuMo
// 	, flag_JingLian_ZhuanKe
// 	, flag_ZhuangBei_DuanZao
	, flag_ZhuangBei_FenJie
	, flag_ZhuangBei_Qianghua

};

class GongJiangBase : public BaseLayer, public IMainLayerBody,ZhuShouChatLayerCallBack
{
public:
// 	MainLayerGongJiangBase();
// 	~MainLayerGongJiangBase();
	bool init(int flag);
	static GongJiangBase * create(int flag = 0);

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}

	virtual void onEnter();

public:
	void GoToSubGongJiang(float dt);
	void menuItemClicked(CCObject* pSender);
	virtual void ZhuShouchatEndCallBack();
protected:
	BaseSprite *mBg;

	CCMenu *mMainMenu;

	int mGoToFlag;
};