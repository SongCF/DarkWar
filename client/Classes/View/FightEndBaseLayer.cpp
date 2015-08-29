#include "FightEndBaseLayer.h"
#include "Music.h"
#include "font.h"
#include "BaseSprite.h"
#include "LFStrings.h"
#include "GameDirector.h"
#include "MainLayerBase.h"
#include "BackgroudLayer.h"
#include "GameScene.h"
#include "HeroRankList.h"
#include "CmdHelper.h"
#include "MapsManager.h"
#include "MessageDisplay.h"
#include "ClientFightLayer.h"
#include "WaitLayer.h"
#include "ClientErrorCode.h"
#include "CS_Common.h"
#include "MyselfManager.h"
#include "XueSeJiaoTangFightPrepareLayer.h"
#include "XueZhanManager.h"
#include "UnblockManager.h"
#include "XueSeJiaoTangLayer.h"
#include "NetTextListShowUI.h"

#define Label_Size 24
#define TouchPriority	-300

extern int g_iGetRongYuDianShu;

enum 
{
	Tag_MenuItem_ChallengeAgain = 1,	//再次挑战
	Tag_MenuItem_End,					//结束战斗
	Tag_MenuItem_Close,					//关闭
	Tag_MenuItem_GoToQiangHua,			//去强化
	Tag_MenuItem_NextLevel,				//挑战下一关
};

FightEndBaseLayer* FightEndBaseLayer::create(bool bWin, ComeFromType type)
{
	FightEndBaseLayer* layer = new FightEndBaseLayer(bWin, type);
	if(layer)
	{
		layer->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(layer);
		layer = NULL;
	}

	return layer;
}


FightEndBaseLayer::FightEndBaseLayer(bool bWin, ComeFromType type)
	:m_bWin(bWin)
	,m_ComeFromType(type)
	,m_pTipLabel(NULL)
	,m_iSuijiJingjiIndex(0)
{
	// init layer;
	CCLayer::init();
	m_WinSize = CCDirector::sharedDirector()->getWinSize();
	this->setTag(Tag_FightEndBase_Layer);
	this->setZOrder(1025);
	// 半透明背景
	CCSprite* btm = BaseSprite::create("zhandou_jiugongge_touming.png");
	addChild(btm, -1);
	btm->setPosition(ccp(m_WinSize.width/2, m_WinSize.height/2));
	btm->setScaleX(m_WinSize.width / btm->getContentSize().width);
	btm->setScaleY(m_WinSize.height / btm->getContentSize().height);
	btm->setOpacity(0);
	btm->runAction(CCFadeTo::create(0.5,220));

	//初始化菜单
	m_pMenu = CCMenu::create();
	addChild(m_pMenu, 10);
	m_pMenu->setPosition(ccp(0,0));
	m_pMenu->setTouchPriority(TouchPriority-1);


	//1. 成功失败！！？
	std::string music;
	CCSprite *imageBiaoTi;

	if(bWin)
	{//成功
		imageBiaoTi = BaseSprite::create("zhandou_shengli.png");
		music = "zhandou_jiesuan_shengli.wav";

	}else
	{//失败
		imageBiaoTi = BaseSprite::create("zhandou_shibai.png");
		music = "zhandou_jiesuan_shibai.wav";
	}

	{
		imageBiaoTi->setPosition(ccp(m_WinSize.width/2, m_WinSize.height-260));
		imageBiaoTi->setOpacity(100);
		addChild(imageBiaoTi, 1);

		//闪烁效果
		imageBiaoTi->setScale(5);
		CCScaleTo* scale = CCScaleTo::create(0.25f, 0.8f, 0.8f);
		CCFadeTo * fade  = CCFadeTo::create(0.25f, 255);
		CCDelayTime *dt = CCDelayTime::create(0.25f);
		CCScaleTo *scaleBack = CCScaleTo::create(0.15f, 1, 1);

		CCCallFunc *callfun = CCCallFunc::create(this, callfunc_selector(FightEndBaseLayer::showMenuItem));
		imageBiaoTi->runAction(CCSequence::create(CCSpawn::create(scale, fade, NULL), callfun, scaleBack, NULL));
	}

	Music::sharedMusicPlayer()->playEffectSound(music);
}

void FightEndBaseLayer::showMenuItem()
{
// 	//-2
// 	CCSprite* fightMap = MapsManager::getManager()->getBackGroundMapImage(
// 		g_taskId>=HellTaskId_Min ? (g_taskId-HellTaskId_Min+1) : (g_taskId));
// 	addChild(fightMap, -2);
// 	fightMap->setAnchorPoint(ccp(0.5, 1));
// 	fightMap->setPosition(ccp(mWinSize.width/2, mWinSize.height));
// 	fightMap->setOpacity(0);
// 	fightMap->runAction(CCFadeTo::create(0.25, 255));

	//战斗结束按钮
	CCMenuItemImage *pEndItem = CCMenuItemImage::create(
		"zhandou_jiesuan_anniu_jieshu.png",
		"zhandou_jiesuan_anniu_jieshu_select.png",
		this, menu_selector(FightEndBaseLayer::menuItemClicked_MainMenu));
	pEndItem->setPosition(ccp(m_WinSize.width/2, m_WinSize.height/2 - 170));

	//下一关按钮
	CCMenuItemImage *pNextItem = CCMenuItemImage::create(
		"zhandou_ditu_renwushuoming_xiayiguan.png",
		"zhandou_ditu_renwushuoming_xiayiguan_select.png",
		this, menu_selector(FightEndBaseLayer::menuItemClicked_MainMenu));
	pNextItem->setPosition(ccp(m_WinSize.width/2, m_WinSize.height/2 - 170));

	//关闭按钮
	CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
		"zhandou_jiesuan_anniu_guanbi.png",
		"zhandou_jiesuan_anniu_guanbi_select.png",
		this, menu_selector(FightEndBaseLayer::menuItemClicked_MainMenu));
	pCloseItem->setPosition(ccp(m_WinSize.width/3, m_WinSize.height/2 - 170));

	//去强化按钮
	CCMenuItemImage *pGoToQianghuaItem = CCMenuItemImage::create(
		"zhandou_jiesuan_anniu_quqianghua.png",
		"zhandou_jiesuan_anniu_quqianghua_select.png",
		this, menu_selector(FightEndBaseLayer::menuItemClicked_MainMenu));
	pGoToQianghuaItem->setPosition(ccp(m_WinSize.width/3*2, pCloseItem->getPositionY()));
	string tishiStr;
	//成功，血色教堂显示下一关按钮或者退出战斗按钮，其他都显示退出按钮
	if (m_bWin)
	{
		tishiStr = LFStrings::getValue("shengli");
		if (m_ComeFromType == ComeFrom_Xuesejiaotang_Type)
		{
			m_pMenu->addChild(pNextItem, 0, Tag_MenuItem_NextLevel);
			tishiStr = LFStrings::getValue("Xuesejiaotang_Shengli_Tip");
		}
		else if (m_ComeFromType == ComeFrom_Jingji_Type)
		{
			m_pMenu->addChild(pEndItem, 0, Tag_MenuItem_End);
			tishiStr = CCString::createWithFormat(LFStrings::getValue("Jingji_Shengli_Tip").c_str(), g_iGetRongYuDianShu)->getCString();
		}
		else if (m_ComeFromType == ComeFrom_Jingji_Suiji_Type)
		{
			m_pMenu->addChild(pEndItem, 0, Tag_MenuItem_End);
			tishiStr = LFStrings::getValue("Jingji_Suiji_Shengli_Tip");
		}
		else if (m_ComeFromType == ComeFrom_Shouhuzhiling_Type)
		{
			m_pMenu->addChild(pEndItem, 0, Tag_MenuItem_End);
			tishiStr = LFStrings::getValue("Shouhuzhiling_Shengli_Tip");
		}
		else if (m_ComeFromType == ComeFrom_Jixianshoucheng_Type)
		{
			m_pMenu->addChild(pEndItem, 0, Tag_MenuItem_End);
			tishiStr = LFStrings::getValue("Jixianshoucheng_Shengli_Tip01");
		}
		else if (m_ComeFromType == ComeFrom_Yingxiongbang_Type)
		{
			m_pMenu->addChild(pEndItem, 0, Tag_MenuItem_End);
			tishiStr = LFStrings::getValue("Yingxiongbang_Shengli_Tip");
		}
	}
	//失败，显示关闭按钮，强化按钮
	else
	{
		tishiStr = LFStrings::getValue("ZhanDouShiBai_Tip").c_str();
		//极限守城失败只显示结束战斗按钮
 		if (m_ComeFromType == ComeFrom_Jixianshoucheng_Type)
 		{
 			tishiStr = LFStrings::getValue("Jixianshoucheng_Shibai_Tip").c_str();
			m_pMenu->addChild(pEndItem, 0, Tag_MenuItem_End);
			pEndItem->setPosition(ccp(m_WinSize.width/2, m_WinSize.height/2 - 170));
 		}
		//血色教堂显示关闭按钮，其他都显示退出战斗和去强化按钮
		else if (m_ComeFromType == ComeFrom_Xuesejiaotang_Type)
		{
			m_pMenu->addChild(pCloseItem, 0, Tag_MenuItem_Close);
			pCloseItem->setPosition(ccp(m_WinSize.width/2, m_WinSize.height/2 - 170));
		}
		//随机竞技失败只显示关闭按钮
		else if (m_ComeFromType == ComeFrom_Jingji_Suiji_Type)
		{
			m_pMenu->addChild(pEndItem, 0, Tag_MenuItem_End);
			pCloseItem->setPosition(ccp(m_WinSize.width/2, m_WinSize.height/2 - 170));
		}
		else
		{
			m_pMenu->addChild(pEndItem, 0, Tag_MenuItem_End);
			m_pMenu->addChild(pGoToQianghuaItem, 0, Tag_MenuItem_GoToQiangHua);
		}

		if (m_ComeFromType == ComeFrom_Shouhuzhiling_Type || m_ComeFromType == ComeFrom_Jingji_Type)
		{
			pEndItem->setPosition(ccp(m_WinSize.width/3, m_WinSize.height/2 - 170));
		}
	}
	NetTextListShowUI* pText = NetTextListShowUI::create();
	pText->setTextWidth(this->getContentSize().width*10);
	pText->setTextSize(Label_Size);
	tishiStr = pText->getConversionMsgStr(tishiStr, ccWHITE);

	if (m_bWin)
	{
		 if (m_ComeFromType == ComeFrom_Jixianshoucheng_Type)
		 {
			 tishiStr += pText->getConversionMsgStr(LFStrings::getValue("Jixianshoucheng_Shengli_Tip02"), ccc3(235, 106, 12));
		 }
	}
	pText->addMessage(tishiStr);

	m_pTipLabel = NetLabelTTF::create(pText->getMsgString().c_str(), fontStr_kaiti, Label_Size);
	m_pTipLabel->setMsgColorTexCoords(pText->getMsgColorTexCoordsList());
	m_pTipLabel->setHorizontalAlignment(kCCTextAlignmentCenter);
	m_pTipLabel->setHeight(pText->getHeight());
	addChild(m_pTipLabel, 1);
 	m_pTipLabel->setPosition(ccp(m_WinSize.width/2, m_WinSize.height/2));
	
// 	//中间提示文字
// 	m_pTipLabel = CCLabelTTF::create(tishiStr.c_str(), fontStr_kaiti, Label_Size);
// 	m_pTipLabel->setHorizontalAlignment(kCCTextAlignmentCenter);
// 	addChild(m_pTipLabel, 1);
// 	m_pTipLabel->setPosition(ccp(m_WinSize.width/2, m_WinSize.height/2));

	BaseSprite* pFengexian0 = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
	addChild(pFengexian0);
	pFengexian0->setPosition(ccp(m_WinSize.width/2, 
		m_pTipLabel->getPositionY()-m_pTipLabel->getContentSize().height/2 - pFengexian0->getContentSize().height/2 - 20));


	BaseSprite* pFengexian = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
	addChild(pFengexian);
	
	//战斗结果
	CCLabelTTF* pTopTip = CCLabelTTF::create(LFStrings::getValue("zhandoujieguo").c_str(), fontStr_kaiti, Label_Size+6);
	pTopTip->setHorizontalAlignment(kCCTextAlignmentCenter);
	if (m_bWin)
	{
		pTopTip->setColor(fonColor_PuTong);
	}
	else
	{
		pTopTip->setColor(ccGRAY);
	}
	addChild(pTopTip, 1);
	pTopTip->setPosition(ccp(m_WinSize.width/2, m_WinSize.height/2 + 70));

	pFengexian->setPosition(ccp(m_WinSize.width/2, pTopTip->getPositionY()+pTopTip->getContentSize().height/2 + pFengexian->getContentSize().height/2));

	BaseSprite* pFengexian1 = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
	addChild(pFengexian1);
	pFengexian1->setPosition(ccp(m_WinSize.width/2,  pTopTip->getPositionY()-pTopTip->getContentSize().height/2 - pFengexian1->getContentSize().height/2));
}

void FightEndBaseLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if( tag == Tag_MenuItem_End || tag == Tag_MenuItem_Close)
	{
		if (m_ComeFromType == ComeFrom_Jingji_Type)
		{
			GameDirector::getDirector()->mainScene(mainlayer_JingJiChang_challenge);
		}
		else if (m_ComeFromType == ComeFrom_Jingji_Suiji_Type)
		{
			if (m_iSuijiJingjiIndex > 0 && m_iSuijiJingjiIndex <= 2)
			{
				GameDirector::getDirector()->goToJingJiChangFromFight(m_iSuijiJingjiIndex);
			}
			else
			{
				GameDirector::getDirector()->mainScene(mainlayer_JingJiChang_random);
			}
		}
		else if (m_ComeFromType == ComeFrom_Shouhuzhiling_Type)
		{
			this->backToShouhuzhiling();
		}
		else if (m_ComeFromType == ComeFrom_Xuesejiaotang_Type)
		{
			this->backToXuesejiaotang();
		}
		else if (m_ComeFromType == ComeFrom_Jixianshoucheng_Type)
		{
			GameDirector::getDirector()->goToJiXianShouChengFromFight();
		}
	}
	else if(tag == Tag_MenuItem_GoToQiangHua)
	{
		vector<BaseActor*> baseactorlist = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
		
		float fZhandouli = 0;
		float tempZhandouli = 0;
		int index = -1;
		for (int i = 0; i < baseactorlist.size(); ++i)
		{
			tempZhandouli = baseactorlist.at(i)->getMyZhanDouLi(Range_Average);
			if (i == 0)
			{
				fZhandouli = tempZhandouli;
				index = i;
			}
			else
			{
				if (tempZhandouli < fZhandouli)
				{
					fZhandouli = tempZhandouli;
					index = i;
				}
			}
		}

		if (index >= 0)
		{
			int heroId = baseactorlist.at(index)->getCurrentProperty().hero_id;
			GameDirector::getDirector()->mainSceneRenXingMianBan(heroId);
		}
	}
	else if (tag == Tag_MenuItem_NextLevel)
	{
		XueSeJiaoTangFightPrepareLayer *layer = new XueSeJiaoTangFightPrepareLayer();
		CCScene *scene = CCScene::create();
		scene->addChild(layer);
		layer->release();
		CCDirector::sharedDirector()->replaceScene(scene);
	}
}

void FightEndBaseLayer::backToShouhuzhiling()
{
	MainLayerBase* footer = new MainLayerBase();
	BackgroudLayer* bgLayer = new BackgroudLayer();

	MapsLayer* mapslayer = new MapsLayer(GoTo_Shouhuzhiling_UI_Type);
	footer->setContaintLayer(mapslayer);

	GameScene* scene = GameScene::createScene();
	scene->addChild(footer,1);
	scene->addChild(bgLayer,-1);

	footer->release();
	bgLayer->release();
	mapslayer->release();

	CCDirector::sharedDirector()->replaceScene(scene);
}

void FightEndBaseLayer::backToXuesejiaotang()
{
	XueZhanManager::ErrorCode state = XueZhanManager::getManager()->canIJoinXueZhan();
	if (state == XueZhanManager::EC_LevelNotEnough)
	{
		CCString temp;
		temp.initWithFormat((LFStrings::getValue("ZhuJue_d_JiKaiQi")
			+LFStrings::getValue("xuesejiaotang")+LFStrings::getValue("HuoDong")).c_str(), 11);
		UnblockWarnDialog *layer = UnblockWarnDialog::create(temp.getCString(), false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
		return;
	}
	else if (state == XueZhanManager::EC_TaskNotEnough)
	{
		CCString temp;
		temp.initWithFormat("must pass unit 1");
		UnblockWarnDialog *layer = UnblockWarnDialog::create(temp.getCString(), false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
		return;
	}
	else if (state == XueZhanManager::EC_NotOpenThisTime)
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("HuoDong")+LFStrings::getValue("WeiKaiQi"), false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
		return;
	}
	else 
	{
		MainLayerBase* footer = new MainLayerBase();
		BackgroudLayer* bgLayer = new BackgroudLayer();

		XueSeJiaoTangLayer* layer = XueSeJiaoTangLayer::create();
		footer->setContaintLayer(layer);

		GameScene* scene = GameScene::createScene();
		scene->addChild(footer,1);
		scene->addChild(bgLayer,-1);

		footer->release();
		bgLayer->release();

		CCDirector::sharedDirector()->replaceScene(scene);
	}  
}

void FightEndBaseLayer::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, TouchPriority, true);
}

void FightEndBaseLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}

bool FightEndBaseLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
