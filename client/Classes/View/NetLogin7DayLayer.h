//七天奖励界面
#ifndef __NetLogin7DayLayer_H__
#define __NetLogin7DayLayer_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "BonusManager.h"
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "LoginGiftLayer.h"
#include "ListMenu.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;
extern bool compareByBonusId(const Flat_BonusInfo &a, const Flat_BonusInfo &b);
bool compareByItemId(const ItemInfo& a, const ItemInfo& b);
class NetLogin7DayLayer : public CCLayer,public TaoZhuangLayerInterface
{
public:
	NetLogin7DayLayer();
	virtual ~NetLogin7DayLayer();

	static CCScene* scene();
	virtual bool init();
	CREATE_FUNC(NetLogin7DayLayer);
	virtual void onEnter();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	inline void setCallBack(CCObject *obj,SEL_CallFunc handler)
	{
		m_oTarger = obj;
		m_hCallBackHandler = handler;
	}
private:
	// 添加消息监听
	void addObserver(SEL_CallFuncO selector,const char *name);
	// 移除消息监听
	void removeObserver(const char *name);

	void menuItemClicked_GetGift(CCObject *obj);
	void rsp_getGift(CCObject *msg_f);
	void menuItemClicked_Equip(CCObject *obj);

	void clickInfo(CCObject *obj);
	void clickCancel(CCObject *obj);
private:

	void displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo);
	void menuItemClicked_equipDetailMenu(CCObject *obj);
	void menuItemClicked_close(CCObject *obj);

	virtual void callbackInterface_TaoZhuangLayer();

	CCPoint addDengluJiangli(CCLayer *layer,CCPoint pos,int index);

	void disableAllTouchBegin();
	void disableAllTouchEnd();
private:
	vector<Flat_BonusInfo> mGiftsLoginVec;//每日登录奖励

	CCLayer					*m_lScrollLayer;

	unsigned int			m_nSelectJiangliIndex;

	CCSize					m_sBodySize;

	CCMenu					*m_mMenu;
	BaseSprite				*m_dEquimentDialog;
	EquipmentTaoZhuangXiangQing		*m_dEquimentInfoLayer;
	ListMenu						*m_mDEquipmentMenu;
	Flat_DuanzaoGroupZBInfo	mCurTaoZhuang_Info;

	CCSprite					*m_sBackSpr;

	vector<CCMenuItem *>		m_lItemList;

	//CCMenu					*m_mParentMenu;


	CCMenuItem					*m_mLingQuItem;
	CCSprite					*m_sLingQuOverSpr;
	CCScrollView				*m_cScroll;

	CCMenuItem					*m_mCloseItem;

	unsigned int				m_nWitchSelect;

	CCObject					*m_oTarger;
	SEL_CallFunc				m_hCallBackHandler;
};

#endif // __NetSystemRewardUI_H__