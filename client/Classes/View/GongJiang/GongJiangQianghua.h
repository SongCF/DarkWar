#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include "Equipment.h"
#include "MessageDisplay.h"
#include "FXToast.h"
#include "GongJiang_EquipSelect.h"

enum Gongjiang_Qianghua_Status
{
	gongjiang_qianghua = 1
	, gongjiang_huitui = 2
};

class GongJiangQianghua : public BaseLayer, public IMainLayerBody
	, public MessageDisplayCallBackInterface
	, public FXToast_CallbackInterface
	, public GongJiangEquipSelectCallbackInterface
{
public:
	bool init(int flag, Equipment *equip);
	static GongJiangQianghua* create(int flag = gongjiang_qianghua, Equipment *equip = NULL);


	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}

	virtual void onEnter();
	virtual void messageDialogEndCallback();

public:
	~GongJiangQianghua(void);
	void menuItemHeadClicked(CCObject *pSender);
	void menuItemClicked(CCObject *pSender);
	void menuItemClicked_Huitui_OkCancel(CCObject *pSender);
	void menuItemClicked_CailiaoClicked(CCObject *psender);

	void clearnBackGround(int status);

	//select equip call back
	virtual void selectEquipmentCallback(unsigned long long eqId, Flag_SelectTo_GongJiang flag);


	void qianghua_SuoXu(Equipment *equip, int status);
	void huitui_SuoXu(Equipment *equip, int status);

	//调用时，先设置mHuiTui_BaoLiu_Select 和 mHuiTui_SelectCount的值
	void refreshHuiTuiItem(Equipment *equip, int status);

	void menuClicked_HuiTuiBaoLiu_Select(CCObject *pSender);


	void ZB_QiangHua_Rsp(CCObject *msg);
	void ZB_HuiTui_Rsp(CCObject *msg);
	void HuiTuiEffectEnd(CCNode *node);
	void refreshBK_WhenRsp(int status);

	//callfunc
	virtual void fxToast_callback();

	//强化动画
	int ishuidui;
	int isAnimation;
	int ishuidian;
	BaseSprite *currentzb;
	CCArmature *ar;
	CCArmature *ar1;
	void Qianhuadh(BaseSprite *xingxing,int level);
	void callfunc_AnimateEnd(CCArmature *, MovementEventType, const char *);
	void onFrameEvent(CCBone  *bone, const char *evt, int originFrameIndex, int currentFrameIndex);  


private:
	void huitui_SuoxuGlob();
	void refreshHuitui_SuoXu(Equipment *equip, int status);
protected:
	BaseSprite *mBg;
	CCMenu *mMainMenu;

	int mCurStatus;
	Equipment *mCurEquip;

	unsigned int mHuiTui_SelectCount;
	bool mHuiTui_BaoLiu_Select[9];
	int mQianghuaLevel[9]; //回退成功后特效用的、 5级不能回退

	//不能强化 所提示的信息
	string mErrorInfomation;

	int mRongShiNum;

	//从装备详情来到工匠
	bool mComeFromEquipDetail;
	Equipment *mComFromGJ_equip;

	int mUI_Guide_FenJieOrHuiTui;  // 10分解   20回退
	BaseSprite				*m_sglobbiIcon;
	CCLabelTTF				*m_tSuoxuGlobeTTF;
	CCLabelTTF				*m_tSuoyouGlobeTTF;

	CCRect					m_rHuituiSuoxuRect;

	CCMenuItem				*m_iQianghuaItem;
	CCMenuItem				*m_iHuituiItem;

};
