//系统奖励合体
#ifndef __NetSystemRewardUI_H__
#define __NetSystemRewardUI_H__

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
class NetSystemRewardUI : public BaseLayer,public IMainLayerBody,public TaoZhuangLayerInterface
{
public:
	NetSystemRewardUI();
	virtual ~NetSystemRewardUI();

	static CCScene* scene();
	virtual bool init();
	CREATE_FUNC(NetSystemRewardUI);
	virtual void onEnter();

private:

	// 得到这个Body的Size
	virtual CCSize getBodySize(){return CCSize(mWinSize.width, mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height);} // 106是底的高度
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return ccp(mWinSize.width/2, (mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height)/2);}

	void menuItemClicked_GetGift(CCObject *obj);
	void rsp_getGift(CCObject *msg_f);
	void menuItemClicked_Equip(CCObject *obj);

	void clickInfo(CCObject *obj);
	void clickCancel(CCObject *obj);

	CCPoint addDengluJiangli(CCLayer *layer,CCPoint pos);

	CCPoint addDengjiJiangli(CCLayer *layer,CCPoint pos);

	CCPoint addRiChangJiangli(CCLayer *layer,CCPoint pos);

	CCPoint addXuanShangJiangli(CCLayer *layer,CCPoint pos);

	void menuItemClicked_XuanShangGoTo(CCObject *obj);
	void menuItemClicked_RiChangGoTo(CCObject *obj);

	void sortRiChangList();

	void resetDengLuJiangLi();
	void resetDengJiJiangLi();
private:

	void displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo);
	void menuItemClicked_equipDetailMenu(CCObject *obj);
	void menuItemClicked_close(CCObject *obj);

	virtual void callbackInterface_TaoZhuangLayer();

	inline CCSprite* getVipImg(int level)
	{
		char buf[40];
		sprintf(buf, "Vip_icon_lv%d.png", level);

		return CCSprite::create(buf);
	}
private:
	vector<Flat_BonusInfo> mGiftsLoginVec;//每日登录奖励
	vector<Flat_BonusInfo> mGiftsLevelVec;//等级奖励
	vector<Flat_BonusInfo> mGiftsSysVec;//日常任务奖励
	vector<Flat_BonusInfo> mGiftsChooseVec;//悬赏奖励

	CCSize					m_sBodySize;

	unsigned int			m_nSelectJiangliIndex;

	vector<CCNode *>		m_lYiLingQuSprList;

	vector<CCMenuItem *>	m_lLingQuItemList;

	map<short,CCSprite*>	m_lInfoBgSprList;

	vector<CCPoint>			m_lInfoPos;

	CCLayer					*m_lScrollLayer;

	unsigned int			m_nSelectLevelIndex;

	ListMenu					*m_mMenu;
	BaseSprite				*m_dEquimentDialog;
	EquipmentTaoZhuangXiangQing		*m_dEquimentInfoLayer;
	ListMenu						*m_mDEquipmentMenu;
	Flat_DuanzaoGroupZBInfo	mCurTaoZhuang_Info;

	int						m_nYiJingOverRenWu;
	//CCMenu					*m_mParentMenu;

};

#endif // __NetSystemRewardUI_H__