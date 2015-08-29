
#ifndef __LoadingLayer_H__
#define __LoadingLayer_H__

#include "LFLayer.h"
#include "TexturePreLoad.h"

using namespace cocos2d;

enum LoadingIntent
{
	Intent_AppDelegate2Main,	// AppDelegate到Main，第一次启动
	Intent_Section2GameIn,		// section旋转界面到游戏内部
	Intent_GameIn2Section,		// 游戏内部到Section选择页面
	Intent_GameIn2Main,			// 游戏内部到Main
	Intent_GameIn2Season,			// 游戏内部到Main
};

class LoadingLayer : public LFLayer,TexturePreLoadCallBack
{
private:
	CCProgressTimer* bar;
	CCSprite* barMoveSide;
	int needLoadImageNum;
	LoadingIntent mIntent;
	int mSeasonId;

	CCProgressTimer* createProgressTimer(const char *fileName);
	CCProgressTimer* createProgressTimer_R(const char *fileName);	// 圆形的

	void startOuterLoading(float time);
	void loadOuterEffectiveSound();
	void loadInnterEffectiveSound();
	void loadingOuterOver(CCNode* node);
	void goToMainPage(float time);
	void goToSectionPage(float time);
	void goToSeasonPage(float time);

	void startInnerLoading(float time);
	void loadingInnerOver();
	void goToSection(float time);		// 进入游戏


	void createInnerLoading();
	void createOutterLoading();

	void barSideFallowTick(float t);		// 时钟bar的指针 跟随

	// 回调接口
	void textureLoaded(int waitingTaskNum,string fileName);
	void loadOver();
public:
	LoadingLayer(LoadingIntent intent,int seasonId = -1);
};

#endif