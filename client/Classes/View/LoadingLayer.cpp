#include "LoadingLayer.h"
#include "ResourcePathManager.h"
#include "GameInforEditer.h"
#include "SpriteHelper.h"
#include "SpriteHelper.h"
#include "Music.h"
//#include "LFShareKit.h"
//#include "DeviceAdapter.h"
#include "GameDirector.h"
//#include "SkillProducer.h"
#include "BaseActorPropertyParser.h"
#include "ImageManager.h"
#include "MyselfManager.h"
#include "EquipmentManager.h"
#include "LFStrings.h"
#include "LoginLayer.h"
#include "PreLoadPareser.h"
#include "History.h"
#include "ItemInformationHelper.h"
#include "SystemGiftBagManager.h"

//Intent_Section2GameIn 的情况需要传入int seasonId
LoadingLayer::LoadingLayer(LoadingIntent intent,int seasonId /*= -1*/)
{
	Music::sharedMusicPlayer()->pauseBackgroundSound();

	needLoadImageNum = 0;
	mIntent = intent;
	mSeasonId = seasonId;
	bar = NULL;
	barMoveSide = NULL;

	//setScale(DeviceAdapter::getAdapter()->getSceneScale());  

	// 先释放
	TexturePreLoad::getInstance()->clearCurrentLoadedTexture();

	switch(mIntent)
	{
	case Intent_AppDelegate2Main:
		createOutterLoading();
		schedule(schedule_selector(LoadingLayer::startOuterLoading),0.5f);
		break;
	case Intent_GameIn2Section:
	case Intent_GameIn2Main:
	case Intent_GameIn2Season:
		createInnerLoading();
		schedule(schedule_selector(LoadingLayer::startOuterLoading),0.5f);
		break;
	case Intent_Section2GameIn:
		CCAssert(mSeasonId!=-1,"必须传入SeasonId");
		createInnerLoading();
		schedule(schedule_selector(LoadingLayer::startInnerLoading),0.5f);
		break;
	}
}
void LoadingLayer::createInnerLoading()
{
	ResourcePathManager* resMgr = ResourcePathManager::getManager();
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	CCPoint pos = ccp(winSize.width/2,378);

	CCSprite* timerBorder = CCSprite::create(resMgr->getPath("game_in_loading_clock_board.png").c_str());
	addChild(timerBorder);
	bar = createProgressTimer_R(resMgr->getPath("game_in_loading_clock_time.png").c_str());
	addChild(bar);

	CCSprite* barStaticSide = CCSprite::create(resMgr->getPath("game_in_loading_clock_needle2.png").c_str());
	addChild(barStaticSide);

	barMoveSide = CCSprite::create(resMgr->getPath("game_in_loading_clock_needle1.png").c_str());
	addChild(barMoveSide);


	CCSprite* timerMask = CCSprite::create(resMgr->getPath("game_in_loading_clock_cover.png").c_str());
	addChild(timerMask);

	CCSprite* loading = CCSprite::create(resMgr->getPath("game_in_loading_loading1.png").c_str());
	string format = ResourcePathManager::getManager()->getFilePrefixPath() + "game_in_loading_loading%d.png";
	CCAction* loadingAction = SpriteHelper::easyCreateAction(format.c_str(),1,3,0.5f,true);
	loading->runAction(loadingAction);
	addChild(loading);

	timerBorder->setPosition(pos);
	timerMask->setPosition(pos);
	bar->setPosition(ccp(pos.x,pos.y - 9 - 4));
	bar->setPercentage(0.0f);
	barStaticSide->setPosition(ccp(bar->getPositionX(),bar->getPositionY() + barStaticSide->getContentSize().height/2));
	barMoveSide->setPosition(ccp(bar->getPositionX(),bar->getPositionY() + barMoveSide->getContentSize().height/2));
	loading->setPosition(ccpAdd(pos,ccp(0,-150)));

	schedule(schedule_selector(LoadingLayer::barSideFallowTick));
}
void LoadingLayer::barSideFallowTick(float t)
{
	// 跟随
	{
		float angle = bar->getPercentage()/100.0f *360;
		float x = bar->getContentSize().width/4 * sin(CC_DEGREES_TO_RADIANS(angle));
		float y = bar->getContentSize().width/4 * cos(CC_DEGREES_TO_RADIANS(angle));

		//CCLog("angle = %f",angle);

		barMoveSide->setRotation(angle);
		barMoveSide->setPosition(ccp(bar->getPositionX() + x ,bar->getPositionY() + y));
	}
}
void LoadingLayer::createOutterLoading()
{
	ResourcePathManager* resMgr = ResourcePathManager::getManager();
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	CCPoint progressBarPos = ccp(winSize.width/2,winSize.height/10);
	CCSprite* progressBarBoarder = CCSprite::create(resMgr->getPath("progress_bar_border.png").c_str());
	addChild(progressBarBoarder,1);
	progressBarBoarder->setPosition(progressBarPos);
	bar = createProgressTimer(resMgr->getPath("progress_bar.png").c_str());
	addChild(bar,2);
	bar->setPosition(progressBarPos);
	bar->setPercentage(0.0f);
	CCSprite* bg = CCSprite::create(resMgr->getPath("main_progress_bar_bg.jpg").c_str());
	addChild(bg,-1);
	bg->setPosition(ccp(winSize.width/2,winSize.height/2));

	bar->setVisible(false);
	progressBarBoarder->setVisible(false);
}
void LoadingLayer::startInnerLoading(float time)
{
	unschedule(schedule_selector(LoadingLayer::startInnerLoading));
	bar->setPercentage(0);
	loadInnterEffectiveSound();
	TexturePreLoad::getInstance()->startAsync(this);
}
void LoadingLayer::loadingInnerOver()
{
	bar->setPercentage(100);
	scheduleOnce(schedule_selector(LoadingLayer::goToSection),0.2f);
}
void LoadingLayer::goToSection(float time)
{
}
void LoadingLayer::startOuterLoading(float time)
{
	unschedule(schedule_selector(LoadingLayer::startOuterLoading));
	// 先释放
	TexturePreLoad::getInstance()->clearCurrentLoadedTexture();

	if (mIntent == Intent_AppDelegate2Main)
	{
		//GameLayerFactory::getInstance()->loadAllGameLayerBySeasonInfo();
		//GameInforEditer::getGameInfor();//载入文件初始化;
		//AchievementManager::getInstance();

	}
	bar->setPercentage(0);

	loadOuterEffectiveSound();

	TexturePreLoad::getInstance()->addTask(PreLoadPareser::getParser()->getAllPicFile());
	TexturePreLoad::getInstance()->startAsync(this);
}
void LoadingLayer::loadOuterEffectiveSound()
{
	Music* music = Music::sharedMusicPlayer();
	// 先释放
	music->unLoadAllEffectSound();

	// 下面添加自己的代码......
}
void LoadingLayer::loadInnterEffectiveSound()
{
	Music* music = Music::sharedMusicPlayer();
	// 先释放
	music->unLoadAllEffectSound();

	// 下面添加自己的代码......


}
void LoadingLayer::loadingOuterOver(CCNode* node)
{
	////再同步的载入一次 (不能，iOS会崩溃)
	//TexturePreLoad::getInstance()->preLoadOuterTexture();
	//TexturePreLoad::getInstance()->startSync();

	bar->setPercentage(100);
	if (mIntent == Intent_AppDelegate2Main)
	{
		scheduleOnce(schedule_selector(LoadingLayer::goToMainPage),0.2f);
		//// 载入完毕之后再校验，以免出错
		//{
		//	// 会自动验证GameCenter
		//	LFShareKit::getKit();
		//}
	}
	else if (mIntent == Intent_GameIn2Main)
	{
		scheduleOnce(schedule_selector(LoadingLayer::goToMainPage),0.2f);
	}
	else if (mIntent == Intent_GameIn2Section)
	{
		scheduleOnce(schedule_selector(LoadingLayer::goToSectionPage),0.2f);
	}else if (mIntent == Intent_GameIn2Season)
	{
		scheduleOnce(schedule_selector(LoadingLayer::goToSeasonPage),0.2f);
	}
}
void LoadingLayer::goToSectionPage(float time)
{
}
void LoadingLayer::goToSeasonPage(float time)
{
}

void LoadingLayer::goToMainPage(float time)
{
	// 初始化
	LFStrings::init();
	BaseActorPropertyParser::getParser()->init();
	ImageManager::getManager()->initManager();
	History::getHistory()->init();
 	ItemInformationHelper::getHelper();
 	SystemGiftBagManager::getManager();
	GameDirector::getDirector()->index();
}
CCProgressTimer* LoadingLayer::createProgressTimer(const char *fileName)
{
	CCSprite* sprite = CCSprite::create(fileName);
	CCProgressTimer *progress = CCProgressTimer::create(sprite);
	progress->setType( kCCProgressTimerTypeBar );
	progress->setMidpoint(ccp(0,0));
	progress->setBarChangeRate(ccp(1, 0));
	return progress;
}
CCProgressTimer* LoadingLayer::createProgressTimer_R(const char *fileName)
{
	CCSprite* sprite = CCSprite::create(fileName);
	//CCProgressTimer *progress = CCProgressTimer::create(sprite);
	//progress->setType( kCCProgressTimerTypeRadial);
	//progress->setMidpoint(ccp(0,0));
	//progress->setBarChangeRate(ccp(1, 0));

	CCProgressTimer* timer = CCProgressTimer::create(sprite);
	timer->setType(kCCProgressTimerTypeRadial);
	timer->setPercentage(0);
	return timer;
}
void LoadingLayer::loadOver()
{
	float end = 90;
	// 取消回调
	TexturePreLoad::getInstance()->cancelCallback();

	switch(mIntent)
	{
	case  Intent_AppDelegate2Main:
	case  Intent_GameIn2Section:
	case  Intent_GameIn2Main:
	case  Intent_GameIn2Season:
		bar->setPercentage(end);
		loadingOuterOver(NULL);
		break;
	case Intent_Section2GameIn:
		bar->setPercentage(end);
		loadingInnerOver();
		break;
	}
}
void LoadingLayer::textureLoaded(int waitingTaskNum,string fileName)
{
	float end = 90;
	float percent = (float)(needLoadImageNum - waitingTaskNum)/(float)needLoadImageNum;
	bar->setPercentage(percent * end);
}
