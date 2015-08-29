#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "Equipment.h"
#include "BaseActor.h"


class HeroRankDuel : public BaseLayer , public IMainLayerBody
{
public:
	HeroRankDuel();
	void initLayer();
	~HeroRankDuel();
	static HeroRankDuel* create();

protected:
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_GuessMenu(CCObject *pSender);
	void menuItemClicked_PlayBack(CCObject *pSender);
	void menuItemClicked_HeroImg(CCObject *pSender);

	void menuItemClicked_CloseBoard(CCObject *pSender);
	void menuItemClicked_GuessBoard(CCObject *pSender);
	void menuItemClicked_PlayBackBoard(CCObject *pSender);

	void updateBeginTime(float dt);

	void getHeroRankDuleState(float dt); //信息保存在 dianfengduijueManager里面的、cmdHelper只会用到两个消息接口、
	BaseSprite* getLuXianByIdx(int round, int idx);   //获取绿色路线  已设置好位置
	CCPoint getPlayBackOrGuessItemPositionByIdx(int round, int idx);  

	void rsp_QueryShenJia(CCObject *msg); // 查询身价
	void rsp_guessHero(CCObject *msg);
	void rsp_playBack(CCObject *msg_f);

	void rsp_queryAnotherTeamInfo(CCObject* msg_f);
	void rsp_queryAnotherTeamYuanshen(CCObject *msg_f);

public:
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return mBg->getPosition();}

protected:
	BaseSprite* mBg;
	CCMenu *mMainMenu;
	CCMenu *mHeroImgMenu;
	CCMenu *mGuessMenu;
	CCMenu *mPlayBackMenu;

	bool mbOpen;  //突然开启、 或 突然结束   那么还timeUpdate中就要立刻刷新layer

	vector<HeroInfo> mTop8HeroInfoVec;

	//竞猜img
	int mGuessHero; //left = 1, right = 2
	int mGuessSilverIdx; // 1 left , 2 right
	pair<HeroInfo, HeroInfo> mCurGuessHero;

	//查询角色详细面板时使用。
	int mNum_YuanShen;
	CCArray *mQueryHeroActors;
	CCArray *mQueryYuanShen; //actor在setYuanshen时没有retain，所以要自己保存并释放（和equipment一样）
	vector<Equipment*> mQueryActorsEquip; //

};