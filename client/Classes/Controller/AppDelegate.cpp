#include "cocos2d.h"
#include "CCEGLView.h"
#include "AppDelegate.h"
#include "AppMacros.h"
#include "SimpleAudioEngine.h"
#include "Music.h"
#include <vector>
#include "VersionControl.h"
//#include "AdManager.h"
#include "LoadingLayer.h"
#include "GameInforEditer.h"
#include "GameScene.h"
#include "GlobalNetworkMsgDispacher.h"
#include "GlobalTickTrigle.h"
#include "NetWorkThreadProc.h"
#include "LFUtile.h"
#include "PreLoadPareser.h"
#include "History.h"
//lua
//#include "CCLuaEngine.h"
//#include "Lua_extensions_CCB.h"
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//#include "Lua_web_socket.h"
//#endif


USING_NS_CC;
using namespace CocosDenshion;


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern void InitCrashReport();
#endif


char g_BoundleResourcePath[256];
bool g_NeedRetina = false;
bool g_IsRetina = false;

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
bool ByteOrder_In_Big = false;
#endif

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();
}

void initIOSLoginTag()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#endif
}
bool AppDelegate::applicationDidFinishLaunching()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	ByteOrder_In_Big = (LF::checkCPUendian() == 0);
#endif

	// 使用png的压缩格式cng
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		//CCTextureCache::sharedTextureCache()->setUseCng(true);
#endif	

    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
	CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
    pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());

	// Set the design resolution
    pEGLView->setDesignResolutionSize(
		designResolutionSize.width, 
		designResolutionSize.height, 
		kResolutionFixedWidth);

	CCSize frameSize = pEGLView->getFrameSize();
    
    vector<string> searchPath;
     /*In this demo, we select resource according to the frame's height.
     If the resource size is different from design resolution size, you need to set contentScaleFactor.
     We use the ratio of resource's height to the height of design resolution,
     this can make sure that the resource's height could fit for the height of design resolution.

     if the frame's height is larger than the height of medium resource size, select large resource.
	if (frameSize.width > mediumResource.size.width)
	{
        searchPath.push_back(largeResource.directory);

        pDirector->setContentScaleFactor(MIN(largeResource.size.height/designResolutionSize.height, largeResource.size.width/designResolutionSize.width));
	}
    // if the frame's height is larger than the height of small resource size, select medium resource.
    else if (frameSize.width > smallResource.size.width)
    {
        searchPath.push_back(mediumResource.directory);
        
        pDirector->setContentScaleFactor(MIN(mediumResource.size.height/designResolutionSize.height, mediumResource.size.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium resource size, select small resource.
	else
    {
        searchPath.push_back(smallResource.directory);

        pDirector->setContentScaleFactor(MIN(smallResource.size.height/designResolutionSize.height, smallResource.size.width/designResolutionSize.width));
    }*/
	// 直接只使用960*640的
	searchPath.push_back(mediumResource.directory);
	searchPath.push_back("particle"); //粒子 和 shader
	pDirector->setContentScaleFactor(MIN(mediumResource.size.height/designResolutionSize.height, mediumResource.size.width/designResolutionSize.width));

    
    // set searching path
    CCFileUtils::sharedFileUtils()->setSearchPaths(searchPath);

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		// turn on display FPS
		pDirector->setDisplayStats(false);
	}else
	{
		// turn on display FPS
		pDirector->setDisplayStats(false);
	}
    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);



//	//=============================================================
//	// register lua engine
//	//=============================================================
//    CCLuaEngine* pEngine = CCLuaEngine::defaultEngine();
//    CCScriptEngineManager::sharedManager()->setScriptEngine(pEngine);
//
//    CCLuaStack *pStack = pEngine->getLuaStack();
//    lua_State *tolua_s = pStack->getLuaState();
//    tolua_extensions_ccb_open(tolua_s);
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//    pStack = pEngine->getLuaStack();
//    tolua_s = pStack->getLuaState();
//    tolua_web_socket_open(tolua_s);
//#endif
//    
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
//    CCFileUtils::sharedFileUtils()->addSearchPath("script");
//#endif
///*
//    std::string path = CCFileUtils::sharedFileUtils()->fullPathForFilename("hello.lua");
//    pEngine->executeScriptFile(path.c_str());*/
//
//	//=============================================================
//	// register lua engine end
//	//=============================================================



	// 初始话
	GlobalNetworkMsgDispacher::getMsgDispacher();
	NetWorkThreadProc::getProc()->startThread();
	PreLoadPareser::getParser()->init();
	GlobalTickTrigle::getTrigle();

	CCScene* scene = CCScene::create();
	LoadingLayer* layer = new LoadingLayer(Intent_AppDelegate2Main);
	scene->addChild(layer);
	layer->release();
    pDirector->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void appDelegate_applicationDidEnterBackground()
{
	CCDirector::sharedDirector()->stopAnimation();

	if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();

	//保存
	GameInforEditer::getGameInfor()->saveSessonInforList();
	History::getHistory()->save();

}

void AppDelegate::applicationDidEnterBackground()
{
	appDelegate_applicationDidEnterBackground();
}


// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();

	if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
		SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();

	bool isBgMute = Music::sharedMusicPlayer()->isBackgroundMute();
	bool isEffectMute = Music::sharedMusicPlayer()->isEffectiveMute();
	Music::sharedMusicPlayer()->setBackgroundMute(isBgMute);
	Music::sharedMusicPlayer()->setEffectiveMute(isEffectMute);
}
