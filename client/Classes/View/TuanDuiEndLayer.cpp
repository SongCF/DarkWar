
#include "TuanDuiEndLayer.h"
#include "Music.h"
#include "openBoxLayer.h"
#include "ImageManager.h"
#include "font.h"
#include "GameDirector.h"
#include "LFAlert.h"

#define TAG_TiShi_DIALOG 1000


enum TAG{
	Tag_blackBack
	, Tag_Title

	, TAG_TiShi_DIALOG_ok
};

TuanDuiEndLayer::TuanDuiEndLayer(Flat_TeamBossGetBattleScriptGiftRsp& script, vector<HeroInfo>& heros)
{
	mScript = script;
	mHeroVec = heros;

	bTouch = false;
}

TuanDuiEndLayer::~TuanDuiEndLayer()
{

}

TuanDuiEndLayer* TuanDuiEndLayer::create(Flat_TeamBossGetBattleScriptGiftRsp script, vector<HeroInfo> heros)
{
	TuanDuiEndLayer* layer = new TuanDuiEndLayer(script, heros);
	if(layer && layer->init())
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

bool TuanDuiEndLayer::init()
{
	if( !CCLayer::init() )
		return false;


	// init layer;
	mWinSize = CCDirector::sharedDirector()->getWinSize();

	// 半透明背景
	CCSprite* btm = CCSprite::create("zhandou_jiugongge_touming.png");
	addChild(btm, -1, Tag_blackBack);
	btm->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	btm->setOpacity(0);
	btm->runAction(CCFadeTo::create(0.5,100));


	//成功  失败
	std::string music;
	CCSprite *imageBiaoTi;
	if(mScript.fightScript.endResult.myResult == Result_Win)
	{//成功
		imageBiaoTi = CCSprite::create("zhandou_shengli.png");
		music = "zhandou_jiesuan_shengli.wav";
	}else
	{//失败
		imageBiaoTi = CCSprite::create("zhandou_shibai.png");
		music = "zhandou_jiesuan_shibai.wav";
	}
	imageBiaoTi->setPosition(ccp(mWinSize.width/2, mWinSize.height-220));
	imageBiaoTi->setOpacity(100);
	addChild(imageBiaoTi, 2, Tag_Title);

	//闪烁效果
	imageBiaoTi->setScale(5);
	CCScaleTo* scale = CCScaleTo::create(0.25f, 0.8f, 0.8f);
	CCFadeTo * fade  = CCFadeTo::create(0.25f, 255);
	CCDelayTime *dt = CCDelayTime::create(0.25f);
	CCScaleTo *scaleBack = CCScaleTo::create(0.15f, 1, 1);

	CCCallFunc *callfun = CCCallFunc::create(this, callfunc_selector(TuanDuiEndLayer::step1));
	imageBiaoTi->runAction(CCSequence::create(CCSpawn::create(scale, fade, NULL), callfun, scaleBack, NULL));
	Music::sharedMusicPlayer()->playEffectSound(music);


	return true;
}

void TuanDuiEndLayer::step1()
{
	//hero
	mCount = 0;
	schedule(schedule_selector(TuanDuiEndLayer::oneHeroInfo), 0.1f, 3, 0.5f);

	CCSprite* bar = CCSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
	addChild(bar, 1);
	bar->setPosition(ccp(mWinSize.width/2, 1152-450));
	bar->setOpacity(0);
	bar->runAction(CCFadeIn::create(0.1f));

	CCSprite* bar2 = CCSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
	addChild(bar2, 1);
	bar2->setPosition(ccp(mWinSize.width/2, 1152-500));
	bar2->setOpacity(0);
	bar2->runAction(CCFadeIn::create(0.1f));

	CCLabelTTF* paihang = CCLabelTTF::create(LFStrings::getValue("paiming").c_str(), fontStr_kaiti, 28);
	addChild(paihang);
	paihang->setPosition(ccp(mWinSize.width/4, 1152-475));
	paihang->setOpacity(0);
	paihang->runAction(CCFadeIn::create(0.1f));
	paihang->setColor(ccc3(0xff, 0xa5, 0x00));
	//
	CCLabelTTF* shanghai = CCLabelTTF::create(LFStrings::getValue("zaochengshanghai").c_str(), fontStr_kaiti, 28);
	addChild(shanghai);
	shanghai->setPosition(ccp(mWinSize.width/4*3, 1152-475));
	shanghai->setOpacity(0);
	shanghai->runAction(CCFadeIn::create(0.1f));
	shanghai->setColor(ccc3(0xff, 0xa5, 0x00));
}
 
void TuanDuiEndLayer::oneHeroInfo(float dt)
{
	// x 1/4   3/4
	//CCSprite* img = CCSprite::create(ImageManager::getManager()->getImageData_Actor_Origin(					
	//	(Profession)mHeroVec.at(mCount).profession_id).getCurrentImageFile().c_str());

	CCSprite* img = CCSprite::create(ImageManager::getManager()->getImageData_ProfessionHead(					
		(Profession)mHeroVec.at(mCount).profession_id).getCurrentImageFile().c_str());

	addChild(img, 1);
	img->setScale(0.9f);
	img->setPosition(ccp(mWinSize.width/4, 1152-500+70-(mCount+1)*130));
	img->setOpacity(0);
	img->runAction(CCFadeIn::create(0.1f));

	CCLabelTTF* name = CCLabelTTF::create(mHeroVec.at(mCount).name, fontStr_kaiti, 21);
	img->addChild(name);
	name->setPosition(ccp(img->getContentSize().width/2, 0));
	name->setScale(1/img->getScale());


	char buf[20] = "";
	sprintf(buf, "%d", mScript.score_arr[mCount].boss_hp);
	CCLabelTTF* hunt = CCLabelTTF::create(buf, fontStr_kaiti, 25);
	addChild(hunt, 1);
	hunt->setPosition(ccp(mWinSize.width/4*3, img->getPositionY()));
	hunt->setOpacity(0);
	hunt->runAction(CCFadeIn::create(0.1f));


	CCSprite* bar = CCSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
	addChild(bar, 1);
	bar->setPosition(ccp(mWinSize.width/2, 1152-500-(mCount+1)*130));


	++mCount; // 0123

	if(mCount == 4)
	{
		mCount = 0;
		unschedule(schedule_selector(TuanDuiEndLayer::oneHeroInfo));
		bTouch = true;
	}
}

void TuanDuiEndLayer::openBox(int num)
{



	OpenBoxLayer* layer = OpenBoxLayer::create(fromTuanDui, num, mScript.gettedData, mScript.missedData);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);
}

void TuanDuiEndLayer::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1000, true);
}

void TuanDuiEndLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}

bool TuanDuiEndLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if( !bTouch )
		return true;



	int boxNum = mScript.gettedData.size();
	string displayStr;
	if(boxNum > 0)
	{
//		mScript.
		int mici = mScript.score_arr[0].order;
		
		//label
		char buf[20];
		string temp = LFStrings::getValue("tuanduifubenJieSuan");
		sprintf(buf, "%d", mici);
		temp += (buf + LFStrings::getValue("ming") + "\n" + LFStrings::getValue("jiangli"));
		sprintf(buf, "%d", boxNum);
		temp += (buf + LFStrings::getValue("ge") + LFStrings::getValue("baoxiang"));
		displayStr = temp;
	}
	else 
	{
		displayStr = LFStrings::getValue("tuanduifubenJieSuanShibai");
	}

	{
		//弹框提示
		//弹框背景
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
		dialog->setPosition(ccp(winSize.width/2, winSize.height/2));
		dialog->setTag(TAG_TiShi_DIALOG);
		

		CCLabelTTF *label = CCLabelTTF::create(displayStr.c_str(), 
			fontStr_kaiti, 30, CCSize(420, 0), kCCTextAlignmentCenter);
		dialog->addChild(label);
		label->setAnchorPoint(ccp(0.5, 1));
		label->setPosition(ccp(dialog->getContentSize().width/2, 258));
		label->setColor(ccRED);

		//按键
		CCMenu* menu;
		{
			CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
				"shangpin_goumai_queren.png",
				"shangpin_goumai_queren_select.png",
				this,SEL_MenuHandler(&TuanDuiEndLayer::menuItemClicked_tishiInfo_Ok));
			menuItemMakeSure->setUserData((void*)boxNum);

			menu = CCMenu::create(menuItemMakeSure, NULL);
			menu->setPosition(CCPointZero);

			menuItemMakeSure->setTag(TAG_TiShi_DIALOG_ok);
			menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/2,70));
		}

		//弹框
		{
			LFAlert* lfAlert = new LFAlert();
			lfAlert->setTag(TAG_TiShi_DIALOG);
			lfAlert->setBaseContainer(dialog,menu);
			CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert, 128, TAG_TiShi_DIALOG);
			lfAlert->release();
		}
	}




	return true;
}

void TuanDuiEndLayer::menuItemClicked_tishiInfo_Ok(CCObject *pSender)
{
	int num = (int )((CCMenuItemImage*)pSender)->getUserData();
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_TiShi_DIALOG);

	if(num > 0)
		openBox(num);
	else
		GameDirector::getDirector()->mainScene(mainlayer_HuoDong);
}

void TuanDuiEndLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}