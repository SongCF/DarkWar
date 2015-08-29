//@ jiugongge layer
//@ a  map to player choose destnation
//@ by _Richard
#pragma once

#include "cocos2d.h"
#include "FightPrepare.h"
#include "FightBagLayer.h"
#include "AwardDisplay.h"
#include "GetTiLiLayer.h"
#include "ZhuShouChatLayer.h"
const int TAG_JiuGongGe_Layer = 4643 + 100 ;

USING_NS_CC;

class JiuGongGeLayer : public CCLayerColor, public FightPrepareCallback, public BagCallBack
	, public AwardDisplayCallBack_Interface, public GetTiLiCallBack, public ZhuShouChatLayerCallBack
{
public:
	JiuGongGeLayer();
	~JiuGongGeLayer();


	//传入当前关卡的完成信息
	static JiuGongGeLayer* create();

	//传入当前关卡的完成信息
	bool init();

	void menuItemClicked_GridCircle(CCObject* pSender);
	void menuItemClicked_BoxWindowClicked(CCObject *pSender);
	void openChessBoardBox(CCNode *node);
	void rsp_getChessBoardRandomBox(CCObject *msg);

	void menuItemClicked_JiFenGetAward(CCObject *pSender);
	void menuItemClicked_GetBonus(CCObject *pSender);
	void rsp_GetBonus(CCObject *msg);

	void fadeBack(int grid);//设置队伍图标位置
	void timeToFight();

	void disableMenuClicked();
	void enableMenuClicked();


	virtual void onEnter();
	virtual void onExit();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);


	void menuItemClicked_MainMenu(CCObject* pSender);
	void menuItemClicked_HuanZhuang(CCObject *pSender);

	void schedule_displayNewZone(float dt);
	void displayNewZone();

	virtual void ZhuShouchatEndCallBack();
	enum BtnState
	{
		Normal_State = 1,
		Disable_State,
	};
	void teachGuide();
public:
	//fight prepare call back
	// 失败，返回一个Code，就是协议定义的SimpleProtocolErrorCode
	virtual void prepareError(int code);
	// 提供背景
	virtual CCSprite* getBg(int taskId,int gridIndex);
	virtual void prepareSuccess();
	/////////////////////////////////////////////// BagCallBack
	virtual void disableAllTouch()
	{
		disableMenuClicked();
		mMainMenu->setVisible(false);
		mJiFenBoard->setVisible(false);
	}
	virtual void enableAllTouch()
	{
		enableMenuClicked();
		mMainMenu->setVisible(true);
		mJiFenBoard->setVisible(true);
	}

	virtual void awardDisplayCallback_func();

	virtual void getTiliCallBackFunc();
	void btnCallback(CCObject* pObj);
	void clickDone(CCMenuItemImage* pItem);
	void startFight(CCNode* pSpr = NULL);
	void setBtnState(int index, BtnState state);
	CCMenuItemImage* getBtnItem(int index);
	void showUnLockSpr(TaskGridInfo info, bool bFirst = false);
	void showPoSuiEffect();
	void openBoxRsp(CCObject* pObj);
	void setTiliTTF();
	void showKouChuTiLiAni();
	////////////////////////////////////////////////
private:
	

	void addBossEffects(CCMenuItem *bossItem);
	void addBianKuang(CCMenuItem *bossItem);

	void startState(float dt);
	inline void removeSelf(CCNode *node)
	{
		bool isRemoveParent = false;
		if (node->getParent()->getChildrenCount() == 1)
		{
			isRemoveParent = true;
		}
		node->removeFromParent();
		m_nActionCount--;
		if (m_nActionCount == 0)
		{
			//startState(0);
			//node->getParent()->removeFromParent();
		}
	}

	void addEyeFire();
	void colorAdd(CCParticleFire *particleSpr,CCNode *node);

	void GuiDeLayerCallBack();
protected:
	CCSize   mWinSize;

	FightPrepare *mFight;
	int mGrid;

	int mTouchedIndex;
	vector<TaskGridInfo> mTaskGridInfoList;

	bool mStart;

	CCMenu *mMainMenu;
	CCSprite *mJiFenBoard;

	BaseSprite *jggMap;
	CCMenuItemImage* circle[9];	

	CCMenuItem			*m_iWenhaoMenuItem;
	CCMenuItem			*m_iBossMenuItem;
	vector<CCSprite *>	m_lBianKuangSprList;

	CCSprite			*m_sEyeFireSpr;
	unsigned int		m_nActionCount;

	CCRect				m_rJiugonggeMenuRect;


};