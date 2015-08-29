
#pragma once



#include "cocos2d.h"
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "flat.h"
#include "BaseSprite.h"

using namespace cocos2d;


class JingJiChangLayer : public BaseLayer, public IMainLayerBody
{
public:
	JingJiChangLayer();
	~JingJiChangLayer();
	// flag = 1 第一场结束  
	// flag = 2 第二场结束
	bool init(int flag = 0);
	static JingJiChangLayer* create(int flag = 0);

	void addWaitingLayer(float dt);

	vector<ZhuangbeiSummary> getHeroEquipSumById(int heroId);
	void drawHeroImgAndInfo(int i);
	void areaEnterRsp(CCObject* f_msg_rsp);
	void areaUpdateRoomRsp(CCObject* f_msg_rsp);
	void arenaGetBattleScriptAndGiftRsp(CCObject* f_msg_rsp);

	void changeZOrder_callbackfun(CCNode *node);
//	void namebackground_callbackfun(CCNode *node);
//	void namelabel_callbackfun(CCNode *node, void *data);
	void vsImgChange(CCNode *node, void *flag);

	void waitingForFightWithDisplayInfo(float dt);
	void timeToFight();

// 	void infoWindowDown(float dt);
 	void resetInfoLabel(CCNode* node);
 	void updateWindowInfomation(float dt);

	//第一轮对阵图
	void AgainstPlan1(float dt);

	//第一场或第二场结束后 玩家的基本对阵图  1 第一场结束   2 第二场结束
	void everyFightEndDraw(int flag);
	void swapPosition(int flag, CCNode* node1, CCNode* node2);

	//第二轮对阵图
	void AgainstPlan2(float dt);

	//战斗结算
	void CombatSettlement(float dt);

	//开宝箱
	void menuItemClicked_tishiInfo_Ok(CCObject *pSender);
	void openAwardBox(CCNode* pSender, void *data);

	//1  第一场
	// 2 第二场
	void prepareFight(int flag);
	void gotoFight(CCNode* node, void* flag);

	void error_TimeOut(CCObject *msg);

//footlayer
	virtual void disableAllTouchBegin(){};
	virtual void disableAllTouchEnd(){};
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();};
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return mBg->getPosition();};

protected:
	BaseSprite* mBg;

	int mUpdataInfoNum;   //请等待 的冒号个数
//	CCSprite* mInfoWindow;
//	CCSize mInfoWindowSize;
//	int mCurrentDeltaHeight;

	//当第一轮战斗结束后要跳回来  等待 第二轮   所以是static
	static vector<HeroInfo> s_mHero;
	static vector<EquipData> s_mHeroEquipData[4];
	static Flat_ArenaGetBattleScriptGiftRsp s_mBattle;
	

	int mFlag;
//	bool m_bUp;
	
	int mFightNum;

// 	int mWaitActorTime;    //初始化为15
// 	int mWaitFightTime;    //初始化为3
	pair<int, int> mChangePosIndx1, mChangePosIndx2;
	CCPoint mPos[4];
};