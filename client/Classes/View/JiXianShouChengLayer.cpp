
#include "JiXianShouChengLayer.h"
#include "LFStrings.h"
#include "font.h"
#include "WorldBossCityGuardManager.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "flat.h"
#include "FightDirector.h"
#include "ClientFightLayer.h"
#include "MapsManager.h"
#include "ImageManager.h"
#include "FX_CommonFunc.h"
#include "MyselfManager.h"
#include "FXToast.h"

using namespace FX_CommonFunc;

#define Label_Size_Min 20
#define Label_Size_Max 24

#define Label_Color_DanHuang ccc3(195, 145, 96)

enum TAG
{
	Tag_MainMenuItem_ZhanBao, //战报
	Tag_MainMenuItem_ZhanDou, //开始战斗
	Tag_MainMenuItem_FuHuo,   //复活

	Tag_Label_BossName,
	Tag_Label_BossLevel,
	Tag_Bar_BossBlood,
	Tag_Label_BossBloodNum,

	Tag_Label_Time,
	Tag_Label_CDTime,

	Tag_Window_Report,
	Tag_ReMenuItem_Close,
	Tag_ReMenuItem_GetReward,
};

JiXianShouChengLayer::~JiXianShouChengLayer()
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_CLIENT_SHOW_BOSS_HP_SUB);
}

JiXianShouChengLayer* JiXianShouChengLayer::create()
{
	JiXianShouChengLayer* layer = new JiXianShouChengLayer();
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

JiXianShouChengLayer::JiXianShouChengLayer()
{
	
	mBg = BaseSprite::create("shoucheng_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));
	//
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(ccp(0,0));
	//战报
	CCMenuItemImage *jgItem = CCMenuItemImage::create(
		"yxb_anniu_zhanbao.png",
		"yxb_anniu_zhanbao_select.png",
		this, menu_selector(JiXianShouChengLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(jgItem, 0, Tag_MainMenuItem_ZhanBao);
	jgItem->setPosition(ccp(574, 851-263));
	jgItem->setVisible(false);
	//开始战斗
	CCMenuItemImage *zdItem = CCMenuItemImage::create(
		"shoucheng_anniu_zhandou.png",
		"shoucheng_anniu_zhandou_select.png",
		this, menu_selector(JiXianShouChengLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(zdItem, 0, Tag_MainMenuItem_ZhanDou);
	zdItem->setPosition(ccp(320, 851-757));
	zdItem->setVisible(false);
	//复活
	CCMenuItemImage *fhItem = CCMenuItemImage::create(
		"shoucheng_anniu_fuhuo.png",
		"shoucheng_anniu_fuhuo_select.png",
		this, menu_selector(JiXianShouChengLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(fhItem, 0, Tag_MainMenuItem_FuHuo);
	fhItem->setPosition(ccp(320, 851-757));
	fhItem->setVisible(false);

	//boss
	mBossInfoBar = BaseSprite::create("shoucheng_boss_mingzi_bg.png");
	mBg->addChild(mBossInfoBar, 10);
	mBossInfoBar->setPosition(ccp(320, 851-197));
	//name
	CCLabelTTF *nameLabel = CCLabelTTF::create("BossName", fontStr_kaiti, Label_Size_Min);
	mBossInfoBar->addChild(nameLabel, 0, Tag_Label_BossName);
	nameLabel->setAnchorPoint(ccp(0, 0.5));
	nameLabel->setPosition(ccp(20, 78-30));
// 	//level
// 	CCLabelTTF *lvLabel = CCLabelTTF::create("Lv30", fontStr_BookAntiqua, Label_Size_Min);
// 	mBossInfoBar->addChild(lvLabel, 0, Tag_Label_BossLevel);
// 	lvLabel->setAnchorPoint(ccp(0, 0.5));
// 	lvLabel->setPosition(ccp(144, nameLabel->getPositionY()));
	//blood bar
	BaseSprite *bloodBg = BaseSprite::create("shoucheng_boss_xuetiao_bg.png");
	mBossInfoBar->addChild(bloodBg);
	bloodBg->setPosition(ccp(376, nameLabel->getPositionY()));
	//progress
	CCProgressTimer *progress = CCProgressTimer::create(CCSprite::create("shoucheng_boss_xuetiao.png"));
	mBossInfoBar->addChild(progress, 1, Tag_Bar_BossBlood);
	progress->setPosition(bloodBg->getPosition());
	progress->setType( kCCProgressTimerTypeBar );//
	progress->setMidpoint(ccp(0,0));//
	progress->setBarChangeRate(ccp(1, 0));//
	progress->setPercentage(0);
	//num
	CCLabelTTF *bloodNumLabel = CCLabelTTF::create("", fontStr_BookAntiqua, 18);
	mBossInfoBar->addChild(bloodNumLabel, 2, Tag_Label_BossBloodNum);
	bloodNumLabel->setOpacity(255/2);
	bloodNumLabel->setPosition(bloodBg->getPosition());

	//
	addObserver(callfuncO_selector(JiXianShouChengLayer::rsp_getCurrentState), MSG_getCityGuardCurrentStatRsp);
	mCmdHlp->cmdGetCityGuardCurrentStat();

	if (WorldBossCityGuardManager::getManager()->isManagerOk())
		refreshBackGround();
}

void JiXianShouChengLayer::onExit()
{
	BaseLayer::onExit();
	mCmdHlp->cmdQuitCityGuard();
	GameDirector::getDirector()->hideWaiting();
}

void JiXianShouChengLayer::rsp_getCurrentState(CCObject *msg)
{
	removeObserver(MSG_getCityGuardCurrentStatRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_getCityGuardCurrentStatRsp* data = (SPCmd_getCityGuardCurrentStatRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	//刷新boss血量
	unschedule(schedule_selector(JiXianShouChengLayer::update_BossBlood)); //update cd time处跳过来的
	schedule(schedule_selector(JiXianShouChengLayer::update_BossBlood), 0.1f);
	refreshBackGround();
}

void JiXianShouChengLayer::refreshBackGround()
{
	WorldBossCityGuardManager *manager = WorldBossCityGuardManager::getManager();
	if (! manager->isOpening())
		mCurState = 0;
	else if (manager->getTimeNeedWaitForCD() <= 0)
		mCurState = 1;
	else 
		mCurState = 2;

	mMainMenu->retain();
	mBossInfoBar->retain();

	mBg->removeAllChildren();

	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(ccp(0,0));
	mMainMenu->release();

	mBg->addChild(mBossInfoBar, 10);
	mBossInfoBar->setPosition(ccp(320, 851-197));
	mBossInfoBar->release();

	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_CLIENT_SHOW_BOSS_HP_SUB);

	//
	if (mCurState == 0)
	{
		mMainMenu->getChildByTag(Tag_MainMenuItem_FuHuo)->setVisible(false);
		mMainMenu->getChildByTag(Tag_MainMenuItem_ZhanDou)->setVisible(false);
		mMainMenu->getChildByTag(Tag_MainMenuItem_ZhanBao)->setVisible(true);
		//
		//ui data
		//
		BaseSprite *infoBoard = BaseSprite::create("shoucheng_shouye_xinxi.png");
		mBg->addChild(infoBoard);
		infoBoard->setPosition(ccp(320,851-663));
		//
		CCString temp = LFStrings::getValue("JuLiKaiZhanHaiSheng");
		CCLabelTTF *timeTishiLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
		mBg->addChild(timeTishiLabel, 1);
		timeTishiLabel->setAnchorPoint(ccp(1, 0.5f));
		timeTishiLabel->setPosition(ccp(357, 851-663));
		//
		temp.initWithFormat("%2d:%2d:%2d", 0,0,0);
		CCLabelTTF *timeLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
		mBg->addChild(timeLabel, 1, Tag_Label_Time);
		timeLabel->setAnchorPoint(ccp(0, 0.5));
		timeLabel->setPosition(timeTishiLabel->getPosition());
		//
		schedule(schedule_selector(JiXianShouChengLayer::update_WaitOpenTime), 0.5f);
	}
	else if (mCurState == 1)
	{
		mMainMenu->getChildByTag(Tag_MainMenuItem_FuHuo)->setVisible(false);
		mMainMenu->getChildByTag(Tag_MainMenuItem_ZhanDou)->setVisible(true);
		mMainMenu->getChildByTag(Tag_MainMenuItem_ZhanBao)->setVisible(false);

		//显示其他玩家造成的伤害
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this
			, callfuncO_selector(JiXianShouChengLayer::msg_displayOtherHero), MSG_CLIENT_SHOW_BOSS_HP_SUB, NULL);

		//////////////////boss img
		BaseSprite *bossImg = BaseSprite::create("shoucheng_boss.png");
		mBg->addChild(bossImg);
		bossImg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height - 440));
		//ui data
		BaseSprite *img = BaseSprite::create("shoucheng_zhandouyikaishi.png");
		mBg->addChild(img);
		img->setPosition(ccp(320, 851-590));
	}
	else if (mCurState == 2)  //等待复活
	{
		mMainMenu->getChildByTag(Tag_MainMenuItem_FuHuo)->setVisible(true);
		mMainMenu->getChildByTag(Tag_MainMenuItem_ZhanDou)->setVisible(false);
		mMainMenu->getChildByTag(Tag_MainMenuItem_ZhanBao)->setVisible(false);

		//显示其他玩家造成的伤害
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this
			, callfuncO_selector(JiXianShouChengLayer::msg_displayOtherHero), MSG_CLIENT_SHOW_BOSS_HP_SUB, NULL);

		//boss img
		BaseSprite *bossImg = BaseSprite::create("shoucheng_boss.png");
		mBg->addChild(bossImg);
		bossImg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height - 440));
		CCString temp;
		//伤害排名
		{
			BaseSprite *board = BaseSprite::create("shoucheng_zhandouxinxi.png");
			mBg->addChild(board);
			board->setPosition(ccp(320, 851-588));
			//
			temp.initWithFormat("%s%u(%.1f%%)", LFStrings::getValue("LeiJiShangHai").c_str()
				, manager->getMyTotalShangHai(), manager->getMyTotalShangHai()/(float)manager->getBossTotalBlood()*100);
			CCLabelTTF *label1 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			label1->setPosition(ccp(board->getContentSize().width/2, 105-32));
			board->addChild(label1);
			//
			temp.initWithFormat("%s%s: %u", LFStrings::getValue("DangQian").c_str(), LFStrings::getValue("paiming").c_str()
				, manager->getMyCurrentShangHaiOrder());
			CCLabelTTF *label2 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			board->addChild(label2);
			label2->setPosition(ccp(board->getContentSize().width/2, 105-72));
		}
		//复活剩余时间
		{
			BaseSprite *infoBoard = BaseSprite::create("shoucheng_shouye_xinxi.png");
			mBg->addChild(infoBoard);
			infoBoard->setPosition(ccp(320,851-663));
			//
			CCString temp = LFStrings::getValue("shengyuFuHuoShiJian");
			CCLabelTTF *timeTishiLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Max);
			mBg->addChild(timeTishiLabel, 1);
			timeTishiLabel->setColor(ccc3(59,8,0));
			timeTishiLabel->setAnchorPoint(ccp(1, 0.5f));
			timeTishiLabel->setPosition(ccp(372, 851-663));
			//
			temp.initWithFormat("%d%s", 0, LFStrings::getValue("miao").c_str());
			CCLabelTTF *timeLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Max);
			mBg->addChild(timeLabel, 1, Tag_Label_CDTime);
			timeLabel->setAnchorPoint(ccp(0, 0.5));
			timeLabel->setPosition(timeTishiLabel->getPosition());
			//
			schedule(schedule_selector(JiXianShouChengLayer::update_WaitCDTime), 0.5f);
		}
		//快速复活
		{
			CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("JiXianShouCheng_FuHuoTiShi").c_str(), fontStr_kaiti, Label_Size_Min);
			mBg->addChild(tishiLabel);
			tishiLabel->setPosition(ccp(320, 851-710));
			//
			BaseSprite *goldIcon = BaseSprite::create("jinbi_tubiao.png");
			mBg->addChild(goldIcon);
			goldIcon->setPosition(ccp(297, 851-812));
			//
			temp.initWithFormat("%u", CS::getCityGuardBattleSkipCDNeedGold());
			CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
			goldIcon->addChild(numLabel);
			numLabel->setPosition(ccp(goldIcon->getContentSize().width+numLabel->getContentSize().width/2, goldIcon->getContentSize().height/2));
		}
	}
}

void JiXianShouChengLayer::update_BossBlood(float dt)
{
	WorldBossCityGuardManager *manager = WorldBossCityGuardManager::getManager();
	CCString temp;
	
	//CCLog("BossBlood---------%u", manager->getBossCurrentBlood());
	//1.血量
	CCProgressTimer *progress = (CCProgressTimer*)mBossInfoBar->getChildByTag(Tag_Bar_BossBlood);
	if (progress) progress->setPercentage(manager->getBossCurrentBlood() / (float)manager->getBossTotalBlood() * 100.f);

	//2. num
	CCLabelTTF *numLabel = (CCLabelTTF*)mBossInfoBar->getChildByTag(Tag_Label_BossBloodNum);
	if (numLabel) 
	{
		temp.initWithFormat("%u/%u", manager->getBossCurrentBlood(), manager->getBossTotalBlood());
		numLabel->setString(temp.getCString());
	}
}

//state == 0才会触发
void JiXianShouChengLayer::update_WaitOpenTime(float dt)
{
	if (mCurState == 0)
	{
		CCString temp;

		unsigned int waitTime = WorldBossCityGuardManager::getManager()->getTimeNeedWaitForOpen();
		temp.initWithFormat("%02d:%02d:%02d", waitTime/3600, waitTime%3600/60, waitTime%60);
		CCLabelTTF *timeLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Time);
		if (timeLabel)	
		{
			timeLabel->setString(temp.getCString());
			if (waitTime <= 10) timeLabel->setColor(ccRED);
		}

		if (WorldBossCityGuardManager::getManager()->getTimeNeedWaitForOpen() <= 0)
		{
			unschedule(schedule_selector(JiXianShouChengLayer::update_WaitOpenTime));
			refreshBackGround();
		}
	}
	else
	{
		unschedule(schedule_selector(JiXianShouChengLayer::update_WaitOpenTime));
		refreshBackGround();
	}
}

void JiXianShouChengLayer::update_WaitCDTime(float dt)
{
	if (mCurState == 2)
	{
		CCString temp;

		unsigned int waitTime = WorldBossCityGuardManager::getManager()->getTimeNeedWaitForCD();
		temp.initWithFormat("%u%s", waitTime, LFStrings::getValue("miao").c_str());
		CCLabelTTF *timeLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_CDTime);
		if (timeLabel)	
		{
			timeLabel->setString(temp.getCString());
//			if (waitTime <= 3) timeLabel->setColor(ccRED);
			CCLog("%s", timeLabel->getString());
		}

		//1.boss死了，弹框提示， query 新的state
		if (WorldBossCityGuardManager::getManager()->getBossCurrentBlood() <= 0)
		{
			unschedule(schedule_selector(JiXianShouChengLayer::update_WaitCDTime));
			CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_CLIENT_SHOW_BOSS_HP_SUB);

			MessageDisplay *layer = MessageDisplay::create(LFStrings::getValue("boss_isDead"), this);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		//2.时间到boss活着 自动去打
		else if (WorldBossCityGuardManager::getManager()->getTimeNeedWaitForCD() <= 0)
		{
			unschedule(schedule_selector(JiXianShouChengLayer::update_WaitCDTime));

			if (WorldBossCityGuardManager::getManager()->getBossCurrentBlood() > 0)
			{
				addObserver(callfuncO_selector(JiXianShouChengLayer::rsp_beginFight), MSG_cityGuardBattleRsp_F);
				mCmdHlp->cmdCityGuardBattle();
			}
		}
	}
	else
	{
		unschedule(schedule_selector(JiXianShouChengLayer::update_WaitCDTime));
		refreshBackGround();
	}
}

void JiXianShouChengLayer::messageDialogEndCallback()
{
	addObserver(callfuncO_selector(JiXianShouChengLayer::rsp_getCurrentState), MSG_getCityGuardCurrentStatRsp);
	mCmdHlp->cmdGetCityGuardCurrentStat();
}

void JiXianShouChengLayer::msg_displayOtherHero(CCObject *msg)
{
	//收到一次消息  全部显示、  频率数据层已经固定
	BossHpSub *data = (BossHpSub*)(((MSG_Rsp*)msg)->getData());
	if (data && data->shangHaiVec.size() > 0)
	{
		char buf[20];
		CCSequence *action = CCSequence::create(
			CCFadeTo::create(0.5f, 255),
			CCDelayTime::create(1.5f),
			CCFadeTo::create(0.5f, 0),
			CCCallFuncN::create(this, callfuncN_selector(JiXianShouChengLayer::removeChildFromParent_WhenActionEnd)),
			NULL);
		for (int i=0; i!=data->shangHaiVec.size() && i!=5; ++i) 
		{
			CityGuardAnotherHeroShanghai obj = data->shangHaiVec.at(i);

			CCScale9Sprite *board = CCScale9Sprite::create("shoucheng_wanjia_id_bg.png");
			board->setRotation(getRandomRotation());
			board->setOpacity(0);
			//name
			CCLabelTTF *nameLabel = CCLabelTTF::create(obj.name, fontStr_kaiti, Label_Size_Max);
			board->addChild(nameLabel);
			board->setContentSize(CCSizeMake(20+nameLabel->getContentSize().width, board->getContentSize().height+10));
			nameLabel->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
			nameLabel->setOpacity(0);
			//blood
			CCLabelAtlas* bloodNumLabel = CCLabelAtlas::create(
				"0123456789",
				"bloodNum_shoushang.png",
				60, 94,//71
				'0');
			sprintf(buf, "%u", obj.shanghai);
			bloodNumLabel->setString(buf);
			board->addChild(bloodNumLabel);
			bloodNumLabel->setAnchorPoint(ccp(0.5f, 0));
			bloodNumLabel->setScale(0.6f);
			bloodNumLabel->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height));
			bloodNumLabel->setOpacity(0);

			//用copy有问题、、、
			nameLabel->runAction(CCSequence::create(
				CCFadeTo::create(0.5f, 255),
				CCDelayTime::create(1.0f),
				CCFadeTo::create(1.0f, 0),
				CCCallFuncN::create(this, callfuncN_selector(JiXianShouChengLayer::removeChildFromParent_WhenActionEnd)),
				NULL));
			bloodNumLabel->runAction(CCSequence::create(
				CCFadeTo::create(0.5f, 255),
				CCDelayTime::create(1.0f),
				CCFadeTo::create(1.0f, 0),
				CCCallFuncN::create(this, callfuncN_selector(JiXianShouChengLayer::removeChildFromParent_WhenActionEnd)),
				NULL));
			board->runAction(CCSequence::create(
				CCFadeTo::create(0.5f, 255),
				CCDelayTime::create(1.0f),
				CCFadeTo::create(1.0f, 0),
				CCCallFuncN::create(this, callfuncN_selector(JiXianShouChengLayer::removeChildFromParent_WhenActionEnd)),
				NULL));
			//
			mBg->addChild(board);
			board->setPosition(getRandomPosition(i));
		}
	}
}

void JiXianShouChengLayer::removeChildFromParent_WhenActionEnd(CCNode *node)
{
	node->removeFromParent();
}


void JiXianShouChengLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if (tag == Tag_MainMenuItem_FuHuo)
	{
		int allGold = WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount();
		if (allGold < CS::getCityGuardBattleSkipCDNeedGold())
		{
			FXToast *layer = FXToast::create(LFStrings::getValue("jinbibuzu"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		else
		{
			addObserver(callfuncO_selector(JiXianShouChengLayer::rsp_beginFight), MSG_cityGuardBattleRsp_F);
			mCmdHlp->cmdCityGuardBattle();
		}
	}
	else if (tag == Tag_MainMenuItem_ZhanBao)
	{
		if (WorldBossCityGuardManager::getManager()->isLastCityGuardDayReportOK()) //有
		{
			displayReport();
		}
		else //没有
		{
			addObserver(callfuncO_selector(JiXianShouChengLayer::rsp_getReport), MSG_getCityGuardLastDayReportRsp_F);
			mCmdHlp->cmdGetCityGuardLastDayReport();
		}
	}
	else if (tag == Tag_MainMenuItem_ZhanDou)
	{
		addObserver(callfuncO_selector(JiXianShouChengLayer::rsp_beginFight), MSG_cityGuardBattleRsp_F);
		mCmdHlp->cmdCityGuardBattle();
	}
}

//复活  或 直接开战
void JiXianShouChengLayer::rsp_beginFight(CCObject *msg_f)
{
	removeObserver(MSG_cityGuardBattleRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_cityGuardBattleRsp* data = (Flat_cityGuardBattleRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	if (data->battle_data.is_boss_dead)
	{//refresh
		refreshBackGround();
		return;
	}
	else 
	{
		FightDirector dir;
		vector<FightDirector> dir_vec;
		dir.initScript(data->battle_data.fightScript);
		dir_vec.push_back(dir);
		Bundle bundle;
		bundle.putString("JumpToWhenEnd","JiXianShouCheng");
		ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec
			,MapsManager::getManager()->getShouChengFuBenMapImage(),bundle);
	}
}

void JiXianShouChengLayer::rsp_getReport(CCObject *msg)
{
	removeObserver(MSG_getCityGuardLastDayReportRsp_F);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_getCityGuardLastDayReportRsp *data = (SPCmd_getCityGuardLastDayReportRsp *)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	displayReport();
}

void JiXianShouChengLayer::displayReport()
{
	disableAllTouchBegin();
	
	//
	BaseSprite *board = BaseSprite::create("shoucheng_zhanbao_bg.png");
	mBg->addChild(board, 100, Tag_Window_Report);
	board->setAnchorPoint(ccp(0.5f, 1));
	board->setPosition(ccp(mBg->getContentSize().width/2, 851-164));

	refreReportBoard();
}

void JiXianShouChengLayer::refreReportBoard()
{
	WorldBossCityGuardManager *manager = WorldBossCityGuardManager::getManager();
	Flat_getCityGuardLastDayReportRsp data = manager->getLastCityGuardDayReport();

	BaseSprite *board = (BaseSprite*)mBg->getChildByTag(Tag_Window_Report);
	board->removeAllChildren();

	CCString temp;
	//menu
	CCMenu *menu = CCMenu::create();
	board->addChild(menu);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *closeItem = CCMenuItemImage::create(
		"close.png",
		"close_select.png",
		this, menu_selector(JiXianShouChengLayer::menuItemClicked_resportMenu));
	menu->addChild(closeItem, 0, Tag_ReMenuItem_Close);
	closeItem->setPosition(ccp(564, 672-50));
	if (manager->haveReward())
	{
		CCMenuItemImage *item = CCMenuItemImage::create(
			"a1.png",
			"a2.png",
			this, menu_selector(JiXianShouChengLayer::menuItemClicked_resportMenu));
		menu->addChild(item, 0, Tag_ReMenuItem_GetReward);
		item->setPosition(ccp(520, 672-180));


		//
		temp = LFStrings::getValue("huode") + LFStrings::getValue("jiangli") + ":";
		CCLabelTTF *tishiLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
		board->addChild(tishiLabel);
		tishiLabel->setPosition(ccp(87, 672-180));
		//

		vector<ItemInfo> giftVec;
		unsigned int mineId = MyselfManager::getManager()->getMyZhuJueData()->getId();
		if (data.lastday_report.hero_kill_boss.hero_id == mineId)
		{
			giftVec.push_back(data.lastday_report.hero_kill_reward);
		}
		for (int i = 0; i < data.lastday_report.hero_order.size(); ++i)
		{
			if (data.lastday_report.hero_order.at(i).hero_id == mineId)
				giftVec.push_back(data.lastday_report.hero_order_reward.at(i));
		}
		for (int i=0; i < data.lastday_report.hero_lucky.size(); ++i)
		{
			if (data.lastday_report.hero_lucky.at(i).hero_id == mineId)
				giftVec.push_back(data.lastday_report.hero_lucky_reward);
		}
		//同类型合并
		vector<ItemInfo> tempVec;
		for (int i=0, num=giftVec.size(); i<num; ++i)
		{
			bool has = false;
			for (int j=0; j<tempVec.size(); ++j)
			{
				if (tempVec.at(j).type == giftVec.at(i).type)
				{
					tempVec.at(j).count += giftVec.at(i).count;
					has = true;
					break;
				}
			}

			if (! has) tempVec.push_back(giftVec.at(i));
		}
		giftVec = tempVec;
		//bar的宽度仅支持两个不同的type   
		for (int i=0, num=giftVec.size(); i<num; ++i)
		{
			BaseSprite *itemImg = NULL;
			if (giftVec.at(i).type == ItemsType_Gold) itemImg = BaseSprite::create("jinbi_tubiao.png");
			else if(giftVec.at(i).type == ItemsType_Silver) itemImg = BaseSprite::create("yinbi_tubiao.png");
			else itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(giftVec.at(i).type));
			board->addChild(itemImg);

			itemImg->setPosition(ccp(267, 672-180));
			if (num > 1)
			{
				if (i == 0)
					itemImg->setPositionX(167);
				if (i == 1)
					itemImg->setPositionX(327);
			}
			//
			temp.initWithFormat("%u", giftVec.at(i).count);
			CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
			board->addChild(numLabel);
			numLabel->setAnchorPoint(ccp(0, 0.5f));
			numLabel->setPosition(ccp(itemImg->getPositionX()+itemImg->getContentSize().width/2+6, itemImg->getPositionY()));
		}

		//
		mTableView = CCTableView::create(this, CCSizeMake(566,426));
		mTableView->setDirection(kCCScrollViewDirectionVertical);
		mTableView->setPosition(ccp(22, 22));
		mTableView->setDelegate(this);
		mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
		board->addChild(mTableView);
	}
	else
	{
		mTableView = CCTableView::create(this, CCSizeMake(566,507));
		mTableView->setDirection(kCCScrollViewDirectionVertical);
		mTableView->setPosition(ccp(22, 22));
		mTableView->setDelegate(this);
		mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
		board->addChild(mTableView);
	}

	//boss
	//name
	CCLabelTTF *nameLabel = CCLabelTTF::create("BossName", fontStr_kaiti, Label_Size_Min);
	board->addChild(nameLabel);
	nameLabel->setAnchorPoint(ccp(0, 0.5));
	nameLabel->setPosition(ccp(27, 672-115));
	nameLabel->setColor(ccc3(195, 145, 96));
	//time
	unsigned int time = data.lastday_report.cityguard_total_time;
	temp.initWithFormat("%s: %d%s%d%s%d%s", LFStrings::getValue("HaoShi").c_str(), time/3600, LFStrings::getValue("time_shi").c_str(), 
		time%3600/60, LFStrings::getValue("fen").c_str(), time%60, LFStrings::getValue("miao").c_str());
	CCLabelTTF *timeLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	board->addChild(timeLabel);
	timeLabel->setPosition(ccp(500, 672-115));
	timeLabel->setColor(ccc3(0, 201, 128));
	//
}

void JiXianShouChengLayer::menuItemClicked_resportMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if (tag == Tag_ReMenuItem_Close)
	{
		mBg->removeChildByTag(Tag_Window_Report);
		disableAllTouchEnd();
	}
	else if (tag == Tag_ReMenuItem_GetReward)
	{
		addObserver(callfuncO_selector(JiXianShouChengLayer::rsp_getReward), MSG_getMyCityGuardRewardRsp);
		mCmdHlp->cmdGetMyCityGuardReward();
	}
}

void JiXianShouChengLayer::rsp_getReward(CCObject *msg)
{
	removeObserver(MSG_getMyCityGuardRewardRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_GetMyCityGuardRewardRsp* data = (SPCmd_GetMyCityGuardRewardRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	refreReportBoard();
}

int JiXianShouChengLayer::getRandomRotation()
{
	float ranData = CCRANDOM_0_1();
	if (ranData < 0.3f) return 0;
	else if (ranData < 0.6f) return -30;
	else return 30;
}

CCPoint JiXianShouChengLayer::getRandomPosition(int i)
{
	switch (i)
	{
	case 1:
		return ccp(640/3, 851-306);
	case 2:
		return ccp(640/3*2, 851-410);
	case 3:
		return ccp(640/3, 851-514);
	case 4:
		return ccp(640/3*2, 851-306);
	default:
		return ccp(640/3, 851-410);;
	}
}


CCSize JiXianShouChengLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	Flat_CityGuardReport data = WorldBossCityGuardManager::getManager()->getLastCityGuardDayReport().lastday_report;
	if (idx < data.hero_order.size() + 1) //击杀者 + 伤害奖 用的相同board
		return CCSize(557, 85);
	else
		return CCSize(557, 43);         //幸运奖bar   +   幸运奖
}

unsigned int JiXianShouChengLayer::numberOfCellsInTableView(CCTableView *table)
{
	Flat_CityGuardReport data = WorldBossCityGuardManager::getManager()->getLastCityGuardDayReport().lastday_report;
	if (data.hero_lucky.size() > 0)
		return data.hero_order.size() + data.hero_lucky.size() + 1 + 1; //伤害奖 + 幸运奖 + 击杀者 + “幸运奖bar”
	else 
		return data.hero_order.size() + data.hero_lucky.size() + 1;
}

CCTableViewCell* JiXianShouChengLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell* cell = table->dequeueCell();
	if(cell == NULL)
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}
	else
	{
		cell->removeAllChildren();
	}

	Flat_CityGuardReport data = WorldBossCityGuardManager::getManager()->getLastCityGuardDayReport().lastday_report;
	CCString temp;
	float pos1_x = 20, pos2_x = 430, pos1_y = 83-18, pos2_y = 83-56;
	//
	if (idx == 0)  //击杀者奖
	{
		BaseSprite *board = BaseSprite::create("shoucheng_zhanbao_id_bg_1.png");
		cell->addChild(board);
		board->setPosition(ccp(557.0f/2, 85.0f/2));
		//提示 奖励类型
		CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("JiShaJiangLi").c_str(), fontStr_kaiti, Label_Size_Min);
		board->addChild(tishiLabel);
		tishiLabel->setAnchorPoint(ccp(0, 0.5f));
		tishiLabel->setPosition(ccp(pos1_x, pos1_y));
		tishiLabel->setColor(ccc3(191,93,6));
		//奖品
		temp.initWithFormat("%s: %u", CS::getItemName(data.hero_kill_reward.type).c_str(), data.hero_kill_reward.count);
		CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
		board->addChild(numLabel);
		numLabel->setAnchorPoint(ccp(0, 0.5f));
		numLabel->setPosition(ccp(pos2_x, pos1_y));
		numLabel->setColor(Label_Color_DanHuang);
		//name
		CCLabelTTF *nameLabel = CCLabelTTF::create(data.hero_kill_boss.name, fontStr_kaiti, Label_Size_Max);
		board->addChild(nameLabel);
		nameLabel->setAnchorPoint(ccp(0, 0.5f));
		nameLabel->setPosition(ccp(pos1_x, pos2_y));
	}
	else if (idx >= 1 && idx < data.hero_order.size() + 1)  //伤害奖
	{
		BaseSprite *board = BaseSprite::create("shoucheng_zhanbao_id_bg_1.png");
		cell->addChild(board);
		board->setPosition(ccp(557.0f/2, 85.0f/2));
		//提示 奖励类型
		if (idx <= 3)  //前三甲
			temp.initWithFormat("%s%s%s%s", LFStrings::getValue("ShangHai").c_str(), LFStrings::getValue("di").c_str(),
				getStrByInt(idx).c_str(), LFStrings::getValue("jiangli").c_str());
		else 
			temp = LFStrings::getValue("ShangHai") + LFStrings::getValue("jiangli");
		CCLabelTTF *tishiLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
		board->addChild(tishiLabel);
		tishiLabel->setAnchorPoint(ccp(0, 0.5f));
		tishiLabel->setPosition(ccp(pos1_x, pos1_y));
		if (idx == 1)
			tishiLabel->setColor(ccc3(177,54,207));
		else if (idx == 2)
			tishiLabel->setColor(ccc3(203,40,16));
		else if (idx == 3)
			tishiLabel->setColor(ccc3(123,20,28));
		else 
			tishiLabel->setColor(ccc3(253,180,40));
		//奖品
		temp.initWithFormat("%s: %u", CS::getItemName(data.hero_order_reward.at(idx-1).type).c_str(), data.hero_order_reward.at(idx-1).count);
		CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
		board->addChild(numLabel);
		numLabel->setAnchorPoint(ccp(0, 0.5f));
		numLabel->setPosition(ccp(pos2_x, pos1_y));
		numLabel->setColor(Label_Color_DanHuang);
		//name
		CCLabelTTF *nameLabel = CCLabelTTF::create(data.hero_order.at(idx-1).name, fontStr_kaiti, Label_Size_Max);
		board->addChild(nameLabel);
		nameLabel->setAnchorPoint(ccp(0, 0.5f));
		nameLabel->setPosition(ccp(pos1_x, pos2_y));
	}
	else if (idx >= data.hero_order.size() + 1 && idx < data.hero_order.size() + 2)  // 幸运奖bar
	{
		BaseSprite *board = BaseSprite::create("shoucheng_zhanbao_id_bg_2.png");
		cell->addChild(board);
		board->setPosition(ccp(557.0f/2, 43.0f/2));
		//
		CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("XingYunJiang").c_str(), fontStr_kaiti, Label_Size_Min);
		board->addChild(tishiLabel);
		tishiLabel->setAnchorPoint(ccp(0, 0.5f));
		tishiLabel->setPosition(ccp(pos1_x, board->getContentSize().height/2));
		tishiLabel->setColor(Label_Color_DanHuang);
		//奖品
		temp.initWithFormat("%s: %u", CS::getItemName(data.hero_lucky_reward.type).c_str(), data.hero_lucky_reward.count);
		CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
		board->addChild(numLabel);
		numLabel->setAnchorPoint(ccp(0, 0.5f));
		numLabel->setPosition(ccp(pos2_x, board->getContentSize().height/2));
		numLabel->setColor(Label_Color_DanHuang);
	}
	else if (idx >= data.hero_order.size() + 2 && idx < data.hero_order.size() + 2 + data.hero_lucky.size())//幸运奖
	{
		BaseSprite *board = BaseSprite::create("shoucheng_zhanbao_id_bg_2.png");
		cell->addChild(board);
		board->setPosition(ccp(557.0f/2, 43.0f/2));
		//name
		CCLabelTTF *nameLable = CCLabelTTF::create(data.hero_lucky.at(idx - data.hero_order.size() - 2).name, fontStr_kaiti, Label_Size_Min);
		board->addChild(nameLable);
		nameLable->setAnchorPoint(ccp(0, 0.5f));
		nameLable->setPosition(ccp(pos1_x, board->getContentSize().height/2));
		nameLable->setColor(Label_Color_DanHuang);
	}
	

	return cell;
}