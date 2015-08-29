#pragma once

#include "BaseLayer.h"
#include "MainFooterLayer.h"
#include "Bundle.h"
class FightDirector;

/**
 *	游戏的导演类，负责游戏场景的切换
 */
class GameDirector
{
private:
	static GameDirector* g_dirctor;
	GameDirector(void);
	~GameDirector(void);
public:
	static GameDirector* getDirector();

	// 得到战斗的Layer
	BaseLayer* getFightLayer();

	// 显示等待界面
	void showWaiting();
	// 关闭等待界面
	void hideWaiting();
	void index();
	// 作战
	BaseLayer* fight(vector<FightDirector> dirctor_vec,CCSprite* bg,Bundle bundle = Bundle());
	BaseLayer* fight_team(vector<FightDirector> dirctor_vec,CCSprite* bg,Bundle bundle = Bundle());

	void action();
	// 主选择界面
	void mainScene(enum_MainLayerIndex selectedIndex = mainlayer_ZhuYe);

	// 竞技场
	//flag = 1 第一场结束
	//flag = 2 第二场结束
	void goToJingJiChangFromFight(int flag);
	void goToJiXianShouChengFromFight();
	void goToTopDuelFight(int flag = 0);
	void goToChuanQiLu(vector<ShenbingSuipianInfo> vec); //bundle.putString("JumpToWhenEnd","ChuanQiLu");
	void goToGongJiangFenJie();
	//跳到人行面板 --> 战斗输了 前往
	void mainSceneRenXingMianBan(int heroId);

	// 去主英雄的登陆界面,-1 就是使用默认的0号位或者以后是上次登陆的Id
	void goToDefaultMainHeroLayer(int defaultId = -1);
};
