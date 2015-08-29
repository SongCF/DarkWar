#include "GameDirector.h"
#include "GameScene.h"
#include "MainLayerBase.h"
#include "BackgroudLayer.h"
#include "NetworkWaitingUI.h"
#include "LoginLayer.h"
#include "ChooseHerosLayer.h"
#include "ClientFightLayer.h"
#include "ClientFightLayer_TF.h"
#include "CreateHeroLayer.h"
#include "DefaultMainHeroLayer.h"
#include "MainHeroManager.h"
#include "GameInforEditer.h"
#include "FightDirector.h"
//#include "NPCBodyLayer.h"
#include "JingJichangLayer.h"
#include "MyselfManager.h"
#include "UnblockManager.h"
#include "GongJiangBase.h"
#include "JiXianShouChengLayer.h"
#include "ZhuangBeiMianBanBase.h"
#include "HeroRankDuel.h"
#include "ChuanQiLuLayer.h"
#include "WaitLayer.h"
#include "GongJiangFenJie.h"

#define TAG_FIGHT_LAYER   12345
#define TAG_WAITING		  12346

GameDirector* GameDirector::g_dirctor = new GameDirector();

GameDirector::GameDirector(void)
{
}

GameDirector* GameDirector::getDirector()
{
	return g_dirctor;
}

void GameDirector::showWaiting()
{
	CCScene* runningScene = CCDirector::sharedDirector()->getRunningScene();
	if (runningScene == NULL || runningScene->getChildByTag(TAG_WAITING))
		return;

	NetworkWaitingUI* w = NetworkWaitingUI::create();
	runningScene->addChild(w,128,TAG_WAITING);
}
void GameDirector::hideWaiting()
{
	CCScene* runningScene = CCDirector::sharedDirector()->getRunningScene();
	if (runningScene == NULL)
		return;
	CCNode* wai = runningScene->getChildByTag(TAG_WAITING);
	if (wai)
	{
		wai->removeFromParent();
	}
}

void GameDirector::action()
{
	//GameScene* scene = GameScene::createScene();
	//FightingLayer* layer = new FightingLayer();
	//scene->addChild(layer,0,TAG_FIGHT_LAYER);
	//layer->release();
	//CCDirector::sharedDirector()->replaceScene(scene);
}

BaseLayer* GameDirector::fight(vector<FightDirector> dirctor_vec,CCSprite* bg,Bundle bundle /* = Bundle */)
{
	GameScene* scene = GameScene::createScene();
	ClientFightLayer* layer = new ClientFightLayer(dirctor_vec,bg,bundle);
	scene->addChild(layer,0,TAG_FIGHT_LAYER);
	layer->release();
	CCDirector::sharedDirector()->replaceScene(scene);

	return layer;
}

BaseLayer* GameDirector::fight_team(vector<FightDirector> dirctor_vec,CCSprite* bg,Bundle bundle /* = Bundle */)
{
	GameScene* scene = GameScene::createScene();
	ClientFightLayer_TF* layer = new ClientFightLayer_TF(dirctor_vec,bg,bundle);
	scene->addChild(layer,0,TAG_FIGHT_LAYER);
	layer->release();
	CCDirector::sharedDirector()->replaceScene(scene);

	return layer;
}

BaseLayer* GameDirector::getFightLayer()
{
	return (BaseLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildByTag(TAG_FIGHT_LAYER);
}

void GameDirector::index()
{
	//{
	//	CreateHeroLayer* layer = new CreateHeroLayer();
	//	CCScene* scene = CCScene::create();
	//	scene->addChild(layer);
	//	layer->release();
	//	CCDirector::sharedDirector()->replaceScene(scene);
	//	return;
	//}


	LoginLayer* layer = new LoginLayer();
	CCScene* scene = CCScene::create();
	scene->addChild(layer);
	layer->release();
	CCDirector::sharedDirector()->replaceScene(scene);
}

void GameDirector::mainScene(enum_MainLayerIndex selectedIndex /* = 0 */)
{
	MainLayerBase* footer = new MainLayerBase();
	BackgroudLayer* bgLayer = new BackgroudLayer();
	footer->getFooter()->selectAtIndex(selectedIndex);

	GameScene* scene = GameScene::createScene();
	scene->addChild(footer,1);
	scene->addChild(bgLayer,-1);

	footer->release();
	bgLayer->release();

	CCDirector::sharedDirector()->popToRootScene();
	CCDirector::sharedDirector()->replaceScene(scene);
}

void GameDirector::mainSceneRenXingMianBan(int heroId)
{
	MainLayerBase* footer = new MainLayerBase();
	BackgroudLayer* bgLayer = new BackgroudLayer();

	BaseLayer* layer = ZhuangBeiMianBanBase::create(heroId);
	footer->setContaintLayer(layer);

	GameScene* scene = GameScene::createScene();
	scene->addChild(footer,1);
	scene->addChild(bgLayer,-1);

	footer->release();
	bgLayer->release();

	CCDirector::sharedDirector()->replaceScene(scene);
}

void GameDirector::goToJingJiChangFromFight(int flag)
{
	MainLayerBase* footer = new MainLayerBase();
	BackgroudLayer* bgLayer = new BackgroudLayer();

	footer->setContaintLayer(JingJiChangLayer::create(flag));

	GameScene* scene = GameScene::createScene();
	scene->addChild(footer,1);
	scene->addChild(bgLayer,-1);
	WaitLayer* layer = WaitLayer::create("");
	layer->removeAllChildren();
	scene->addChild(layer, 1000, TAG_Waiting_Layer);

	footer->release();
	bgLayer->release();

	CCDirector::sharedDirector()->replaceScene(scene);
}

void GameDirector::goToJiXianShouChengFromFight()
{
	MainLayerBase* footer = new MainLayerBase();
	BackgroudLayer* bgLayer = new BackgroudLayer();

	BaseLayer* layer =  JiXianShouChengLayer::create();
	footer->setContaintLayer(layer);

	GameScene* scene = GameScene::createScene();
	scene->addChild(footer,1);
	scene->addChild(bgLayer,-1);

	footer->release();
	bgLayer->release();

	CCDirector::sharedDirector()->replaceScene(scene);
}

void GameDirector::goToTopDuelFight(int flag)
{
	MainLayerBase* footer = new MainLayerBase();
	BackgroudLayer* bgLayer = new BackgroudLayer();

	BaseLayer* layer = HeroRankDuel::create();
	footer->setContaintLayer(layer);

	GameScene* scene = GameScene::createScene();
	scene->addChild(footer,1);
	scene->addChild(bgLayer,-1);

	footer->release();
	bgLayer->release();

	CCDirector::sharedDirector()->replaceScene(scene);
}

void GameDirector::goToChuanQiLu(vector<ShenbingSuipianInfo> vec)
{
	MainLayerBase* footer = new MainLayerBase();
	BackgroudLayer* bgLayer = new BackgroudLayer();

	ChuanQiLuLayer* layer = new ChuanQiLuLayer();
	footer->setContaintLayer(layer);
	layer->release();
	layer->addDiaoLuo(vec);

	GameScene* scene = GameScene::createScene();
	scene->addChild(footer,1);
	scene->addChild(bgLayer,-1);

	footer->release();
	bgLayer->release();

	CCDirector::sharedDirector()->replaceScene(scene);
}

void GameDirector::goToGongJiangFenJie()
{
	MainLayerBase* footer = new MainLayerBase();
	BackgroudLayer* bgLayer = new BackgroudLayer();

	GongJiangFenJie* layer = GongJiangFenJie::create();
	footer->setContaintLayer(layer);

	GameScene* scene = GameScene::createScene();
	scene->addChild(footer,1);
	scene->addChild(bgLayer,-1);

	footer->release();
	bgLayer->release();

	CCDirector::sharedDirector()->replaceScene(scene);
}

void GameDirector::goToDefaultMainHeroLayer(int defaultId /* = -1 */)
{
	int hId = defaultId;
	if (hId <= 0)
	{
		int lastLoginedHeroId = GameInforEditer::getGameInfor()->getLastLoginedHeroId();
		int heroNum = MainHeroManager::getManager()->getAllMainHero().size();
		bool _found = false;
		for (int i=0;i<heroNum;i++)
		{
			if (MainHeroManager::getManager()->getAllMainHero()[i].hero_id == lastLoginedHeroId)
			{
				_found = true;
				break;
			}
		}

		if (_found)
		{
			hId = lastLoginedHeroId;
		}else
		{
			hId = MainHeroManager::getManager()->getAllMainHero()[0].hero_id;
		}
	}
	DefaultMainHeroLayer* layer = new DefaultMainHeroLayer(hId);
	CCScene* scene = CCScene::create();
	scene->addChild(layer);
	layer->release();
	CCDirector::sharedDirector()->replaceScene(scene);
}


GameDirector::~GameDirector(void)
{
}
