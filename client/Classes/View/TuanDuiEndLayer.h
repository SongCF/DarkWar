
#pragma once

#include "cocos2d.h"
#include "flat.h"
#include <vector>

using std::vector;

using namespace cocos2d;

class TuanDuiEndLayer : public CCLayer
{
public:
	TuanDuiEndLayer(Flat_TeamBossGetBattleScriptGiftRsp& script, vector<HeroInfo>& heros);
	~TuanDuiEndLayer();

	static TuanDuiEndLayer* create(Flat_TeamBossGetBattleScriptGiftRsp script, vector<HeroInfo> heros);
	bool init();

	void menuItemClicked_tishiInfo_Ok(CCObject *pSender);
	void openBox(int num);


	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);

protected:
	void step1();
	void oneHeroInfo(float dt);

protected:

	CCSize mWinSize;

	vector<HeroInfo> mHeroVec;
	Flat_TeamBossGetBattleScriptGiftRsp mScript;


	int mCount;  //schedule 调用的次数

	bool bTouch ;
};