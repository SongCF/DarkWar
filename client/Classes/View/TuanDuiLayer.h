
#pragma once

#include "cocos2d.h"
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "flat.h"

using namespace cocos2d;

class TuanDuiLayer : public BaseLayer, public IMainLayerBody
{
public:
	TuanDuiLayer();
	~TuanDuiLayer();
	// flag = 1 第一场结束  
	// flag = 2 第二场结束
	bool init();
	CREATE_FUNC(TuanDuiLayer);

	vector<ZhuangbeiSummary> getHeroEquipSumById(int heroId);
	void drawHeroImgAndInfo(int i);
	void teamBossEnterRsp(CCObject* f_msg_rsp);
	void teamBossUpdateRoomRsp(CCObject* f_msg_rsp);
	void teamBossGetBattleScriptAndGiftRsp(CCObject* f_msg_rsp);


	void updateWaitingFightTime(float dt);


	void gotoFight();

	void infoWindowDown(float dt);
	void reloadInfoWindow(float dt);
	void updateWindowInfomation(float dt);
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
	CCSprite* mInfoWindow;
	CCSize mInfoWindowSize;
	int mCurDeltaHeight;
	bool m_bUp;

	vector<HeroInfo> mHero;
	Flat_TeamBossGetBattleScriptGiftRsp mScript;
	vector<EquipData> mHeroEquipData[4];


	int mWaitActorTime;    //初始化为15
	int mWaitFightTime;    //初始化为3
};