#include "HeroRankChallengeEndLayer.h"
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
#include "NetTextListShowUI.h"
#include "NetLabelTTF.h"

#define Label_Size 24


enum 
{
	Tag_MenuItem_ChallengeAgain = 1,  //再次挑战
	Tag_MenuItem_End,                 //结束战斗
	Tag_MenuItem_Query,				  //查看阵容


};

HeroRankChallengeEndLayer* HeroRankChallengeEndLayer::create(bool bWin)
{
	HeroRankChallengeEndLayer* layer = new HeroRankChallengeEndLayer(bWin);
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


HeroRankChallengeEndLayer::HeroRankChallengeEndLayer(bool bWin)
	:mWin(bWin)
{
	// init layer;
	CCLayer::init();
	mWinSize = CCDirector::sharedDirector()->getWinSize();
	this->setTag(Tag_HeroRankChallengeEnd_Layer);
	this->setZOrder(100);

	// 半透明背景
	CCSprite* btm = BaseSprite::create("zhandou_jiugongge_touming.png");
	addChild(btm, -1);
	btm->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	btm->setScaleX(mWinSize.width / btm->getContentSize().width);
	btm->setScaleY(mWinSize.height / btm->getContentSize().height);
	btm->setOpacity(0);
	btm->runAction(CCFadeTo::create(0.5,220));


	//1. 成功失败！！？
	std::string music, tishiStr = LFStrings::getValue("zhanDou");
	CCSprite *imageBiaoTi;

	if(bWin)
	{//成功
		imageBiaoTi = BaseSprite::create("zhandou_shengli.png");
		music = "zhandou_jiesuan_shengli.wav";
		tishiStr += LFStrings::getValue("shengli");
	}else
	{//失败
		imageBiaoTi = BaseSprite::create("zhandou_shibai.png");
		music = "zhandou_jiesuan_shibai.wav";
		tishiStr += LFStrings::getValue("shibai");
	}


	{
		imageBiaoTi->setPosition(ccp(mWinSize.width/2, mWinSize.height-260));
		imageBiaoTi->setOpacity(100);
		addChild(imageBiaoTi, 1);

		//闪烁效果
		imageBiaoTi->setScale(5);
		CCScaleTo* scale = CCScaleTo::create(0.25f, 0.8f, 0.8f);
		CCFadeTo * fade  = CCFadeTo::create(0.25f, 255);
		CCDelayTime *dt = CCDelayTime::create(0.25f);
		CCScaleTo *scaleBack = CCScaleTo::create(0.15f, 1, 1);

		CCCallFunc *callfun = CCCallFunc::create(this, callfunc_selector(HeroRankChallengeEndLayer::displayInfomation));
		imageBiaoTi->runAction(CCSequence::create(CCSpawn::create(scale, fade, NULL), callfun, scaleBack, NULL));
	}

	Music::sharedMusicPlayer()->playEffectSound(music);



	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
		callfuncO_selector(HeroRankChallengeEndLayer::rsp_challengeAgain), MSG_topHeroChallengeRsp_F, NULL);
}
HeroRankChallengeEndLayer::~HeroRankChallengeEndLayer()
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_topHeroChallengeRsp_F);
}

void HeroRankChallengeEndLayer::displayInfomation()
{
// 	//-2
// 	CCSprite* fightMap = MapsManager::getManager()->getBackGroundMapImage(
// 		g_taskId>=HellTaskId_Min ? (g_taskId-HellTaskId_Min+1) : (g_taskId));
// 	addChild(fightMap, -2);
// 	fightMap->setAnchorPoint(ccp(0.5, 1));
// 	fightMap->setPosition(ccp(mWinSize.width/2, mWinSize.height));
// 	fightMap->setOpacity(0);
// 	fightMap->runAction(CCFadeTo::create(0.25, 255));

	CCMenu *menu = CCMenu::create();
	addChild(menu, 10);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *endItem = CCMenuItemImage::create(
		"zhandou_jiesuan_anniu_tuichu.png",
		"zhandou_jiesuan_anniu_tuichu_select.png",
		this, menu_selector(HeroRankChallengeEndLayer::menuItemClicked_MainMenu));
	menu->addChild(endItem, 0, Tag_MenuItem_End);
	endItem->setPosition(ccp(mWinSize.width/3*2, mWinSize.height/2 - 170));
	CCMenuItemImage *queryItem = CCMenuItemImage::create(
		"zhandou_jiesuan_anniu_chakanzhenrong.png",
		"zhandou_jiesuan_anniu_chakanzhenrong_select.png",
		this, menu_selector(HeroRankChallengeEndLayer::menuItemClicked_MainMenu));
	menu->addChild(queryItem, 0, Tag_MenuItem_Query);
	queryItem->setPosition(ccp(mWinSize.width/3, mWinSize.height/2 - 170));

	CCString temp;
	NetTextListShowUI* pText = NetTextListShowUI::create();
	pText->setTextWidth(this->getContentSize().width*10);
	pText->setTextSize(Label_Size);
	
	if (mWin)
	{
//		temp.initWithFormat("%s:%d", LFStrings::getValue("paiMingShangSheng_").c_str(), HeroRankList::s_ChallengeOrder);
		temp.initWithFormat(LFStrings::getValue("Yingxiongbang_Shengli_Tip").c_str());
		temp = pText->getConversionMsgStr(temp.getCString(), ccWHITE)
			+pText->getConversionMsgStr(CCString::createWithFormat("%d",  HeroRankList::s_ChallengeOrder)->getCString(), ccc3(79, 110, 217));
	}
	else
	{
		CCMenuItemImage *againItem = CCMenuItemImage::create(
			"zhandou_jiesuan_anniu_zaicitiaozhan.png",
			"zhandou_jiesuan_anniu_zaicitiaozhan_select.png",
			this, menu_selector(HeroRankChallengeEndLayer::menuItemClicked_MainMenu));
		menu->addChild(againItem, 0, Tag_MenuItem_ChallengeAgain);
		againItem->setPosition(ccp(mWinSize.width/2, mWinSize.height/2-100));

// 		temp.initWithFormat("%s%s:%s!", LFStrings::getValue("TiaoZhan").c_str(), LFStrings::getValue("shibai").c_str()
// 			, LFStrings::getValue("PaiMingBuBian").c_str());
		temp.initWithFormat(LFStrings::getValue("Yingxiongbang_Shibai_Tip").c_str());
		temp = pText->getConversionMsgStr(temp.getCString(), ccWHITE);
	}
	pText->addMessage(temp.getCString());

	NetLabelTTF* pTipLabel = NetLabelTTF::create(pText->getMsgString().c_str(), fontStr_kaiti, Label_Size);
	pTipLabel->setMsgColorTexCoords(pText->getMsgColorTexCoordsList());
	pTipLabel->setHorizontalAlignment(kCCTextAlignmentCenter);
	pTipLabel->setHeight(pText->getHeight());
	addChild(pTipLabel, 1);
	pTipLabel->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));

	BaseSprite* pFengexian0 = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
	addChild(pFengexian0);
	pFengexian0->setPosition(ccp(mWinSize.width/2,  pTipLabel->getPositionY()-pTipLabel->getContentSize().height/2 - pFengexian0->getContentSize().height/2 - 20));

	BaseSprite* pFengexian = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
	addChild(pFengexian);
	//战斗结果
	CCLabelTTF* pTopTip = CCLabelTTF::create(LFStrings::getValue("zhandoujieguo").c_str(), fontStr_kaiti, Label_Size+6);
	pTopTip->setHorizontalAlignment(kCCTextAlignmentCenter);
	if (mWin)
	{
		pTopTip->setColor(fonColor_PuTong);
	}
	else
	{
		pTopTip->setColor(ccGRAY);
	}
	addChild(pTopTip, 1);
	pTopTip->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 + 70));

	pFengexian->setPosition(ccp(mWinSize.width/2, pTopTip->getPositionY()+pTopTip->getContentSize().height/2 + pFengexian->getContentSize().height/2));

	BaseSprite* pFengexian1 = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
	addChild(pFengexian1);
	pFengexian1->setPosition(ccp(mWinSize.width/2,  pTopTip->getPositionY()-pTopTip->getContentSize().height/2 - pFengexian1->getContentSize().height/2));
}

void HeroRankChallengeEndLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if( tag == Tag_MenuItem_End)
	{
		GameDirector::getDirector()->mainScene(mainlayer_YingXiongBang);
	}
	else if(tag == Tag_MenuItem_ChallengeAgain)
	{
		CmdHelper::getHelper()->cmdTopHeroChallenge(HeroRankList::s_ChallengeHeroId);
	}
	else if(tag == Tag_MenuItem_Query)
	{
		MainLayerBase* footer = new MainLayerBase();
		BackgroudLayer* bgLayer = new BackgroudLayer();

		BaseLayer* layer = new HeroRankList(10, HeroRankList::s_ChallengeHeroId);
		footer->setContaintLayer(layer);

		GameScene* scene = GameScene::createScene();
		scene->addChild(footer,1);
		scene->addChild(bgLayer,-1);

		WaitLayer *waitLayer = WaitLayer::create();
		waitLayer->removeAllChildren();
		scene->addChild(waitLayer);

		footer->release();
		bgLayer->release();

		CCDirector::sharedDirector()->replaceScene(scene);
	}
}

void HeroRankChallengeEndLayer::rsp_challengeAgain(CCObject *msg_f)
{
	GameDirector::getDirector()->hideWaiting();

	Flat_TopHeroChallengeRsp* data = (Flat_TopHeroChallengeRsp*) ((Flat_MSG_Rsp*)msg_f)->getData();

	if(data->err_code != Success)
	{
		if (CEC_TOPHeroISCDTime == data->err_code)
		{
			//弹框背景
			BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
			dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
			//label
			{
				CCLabelTTF *title = CCLabelTTF::create((LFStrings::getValue("TiaoZhanLengQue")+"...").c_str(), fontStr_kaiti, 24);
				dialog->addChild(title);
				title->setColor(fonColor_JingGao);
				title->setPosition(ccp(dialog->getContentSize().width/2, 270));
				//
				CCString temp;
				temp.initWithFormat(LFStrings::getValue("QingChuLengQuXuYao").c_str(), CS::clearTopHeroCDTimeNeedGold());
				CCLabelTTF *conLa = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20, CCSize(0,0), kCCTextAlignmentCenter);
				dialog->addChild(conLa);
				conLa->setAnchorPoint(ccp(0.5, 1));
				conLa->setPosition(ccp(dialog->getContentSize().width/2, 230));
			}

			//按键
			CCMenu* menu;
			{
				CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
					"shangpin_goumai_queren.png",
					"shangpin_goumai_queren_select.png",
					this,SEL_MenuHandler(&HeroRankChallengeEndLayer::menuItemClicked_ClearTime));


				CCMenuItemImage* menuItemCacel = CCMenuItemImage::create(
					"shangpin_goumai_quxiao.png",
					"shangpin_goumai_quxiao_select.png",
					this,SEL_MenuHandler(&HeroRankChallengeEndLayer::menuItemClicked_ClearTime));

				menu = CCMenu::create(menuItemMakeSure, menuItemCacel, NULL);
				menu->setPosition(CCPointZero);

				menuItemMakeSure->setTag(10);
				menuItemCacel->setTag(20);
				menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/3*2,70));
				menuItemCacel->setPosition(ccp(dialog->getContentSize().width/3, 70));
			}

			//弹框
			{
				LFAlert* lfAlert = new LFAlert();
				lfAlert->setBaseContainer(dialog,menu);
				CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert, this->getZOrder()+1, 144);
				lfAlert->release();
			}
			return;
		}

		MessageDisplay* layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		this->addChild(layer, 1025);
		return;
	}

	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(data->fightScript);
	dir_vec.push_back(dir);

	Bundle bundle;
	bundle.putString("JumpToWhenEnd","PaiHangBang");
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec
		,MapsManager::getManager()->getJingJiChangMapImage(),bundle);
}

void HeroRankChallengeEndLayer::menuItemClicked_ClearTime(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(144);

	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == 10) //make sure
	{
		CCNotificationCenter::sharedNotificationCenter()->addObserver(
			this, callfuncO_selector(HeroRankChallengeEndLayer::rsp_clearCDTime), MSG_TopHeroClearCDTimeRsp, NULL);
		CmdHelper::getHelper()->cmdTopHeroClearCDTime();
	}
}

void HeroRankChallengeEndLayer::rsp_clearCDTime(CCObject *msg)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_TopHeroClearCDTimeRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_TopHeroClearCDTimeRsp* data = (SPCmd_TopHeroClearCDTimeRsp*) ((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1025);
		return;
	}



	CmdHelper::getHelper()->cmdTopHeroChallenge(HeroRankList::s_ChallengeHeroId);
}

void HeroRankChallengeEndLayer::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -127, true);
}

void HeroRankChallengeEndLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}

bool HeroRankChallengeEndLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
