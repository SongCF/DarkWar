
#include "HeroRankList.h"
#include "MessageDisplay.h"
#include "flat.h"
#include "GameDirector.h"
#include "MyselfManager.h"
#include "MainHeroManager.h"
#include "LFStrings.h"
#include "font.h"
#include "FightDirector.h"
#include "ClientFightLayer.h"
#include "flat.h"
#include "History.h"
#include "ZhuangBeiMianBanOther.h"
#include "MapsManager.h"
#include "GuideLayerBase.h"
#include "WaitLayer.h"
#include "FXScrollMenu.h"
#include "WaitLayer.h"
#include "ServerTime.h"
#include "CS_Common.h"


#define SIZE_STR 24

enum TAG
{
	  Tag_HeadMenu_HeroList
	, Tag_HeadMenu_Report

	,Tag_HeroListInfoBg
	,Tag_Label_Qian8TiShi

	, Tag_Board
	, Tag_Menu
	, Tag_MenuItem_Challege
	, Tag_MenuItem_Query
	, Tag_Label_Order
	, Tag_Label_Name
	, Tag_Label_Profession
//	, Tag_Label_Level
	, Tag_Label_ZhanDouLi

	, Tag_Label_Time
};

unsigned int HeroRankList::s_ChallengeHeroId;
unsigned int HeroRankList::s_ChallengeOrder;

HeroRankList::HeroRankList(int flag, unsigned int heroId)
	:mFlag(flag)
{
	s_ChallengeHeroId = heroId;

	mTableView = NULL;
	mBg = NULL;
	mHeadMenu = NULL;

	//
	mBg = BaseSprite::create();
	mBg->initWithFile("yingxiongbang_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));

	//
	mHeadMenu = CCMenu::create();
	mHeadMenu->init();
	mBg->addChild(mHeadMenu, 10);
	mHeadMenu->setPosition(CCPointZero);
	//
	CCMenuItemImage* listItem = CCMenuItemImage::create(
		"yxb_anniu_yingxiongpaiming.png",
		"yxb_anniu_yingxiongpaiming_select.png",
		this, menu_selector(HeroRankList::menuItemClicked_HeadMenu));
	mHeadMenu->addChild(listItem, 0, Tag_HeadMenu_HeroList);
	listItem->setPosition(ccp(210, 860-105));
	//
	CCMenuItemImage* zhanbaoItem = CCMenuItemImage::create(
		"yxb_anniu_zhanbao.png",
		"yxb_anniu_zhanbao_select.png",
		this, menu_selector(HeroRankList::menuItemClicked_HeadMenu));
	mHeadMenu->addChild(zhanbaoItem, 0, Tag_HeadMenu_Report);
	zhanbaoItem->setPosition(ccp(429, 860-105));


	scheduleOnce(schedule_selector(HeroRankList::delaySchedule), 0);
}

void HeroRankList::delaySchedule(float dt)
{
	{
		addObserver(callfuncO_selector(HeroRankList::rsp_TopHeroList), MSG_topHeroGetListRsp_F);
		mCmdHlp->cmdTopHeroGetList();
	}
}

void HeroRankList::rsp_TopHeroList(CCObject* msg_f)
{
	removeObserver(MSG_topHeroGetListRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_TopHeroGetListRsp *data = (Flat_TopHeroGetListRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if(data->err_code != Success)
	{
		//error
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	mNextChlgTime = data->next_chlg_time;
	if (mNextChlgTime > ServerTime::getTime() )
	{
		schedule(schedule_selector(HeroRankList::schedule_updateChlgTime), 1.0, mNextChlgTime+10, 0);
	}
	
	//init data
	mMyOrder = data->my_order;
	for (int i=0; i<data->top30_hero.size() && i!=10; ++i)
	{
		mHeroVec.push_back(data->top30_hero.at(i));
	}
	for (int i=0; i<data->hero_list_surrounding.size(); ++i)
	{
		mHeroVec.push_back(data->hero_list_surrounding.at(i));
	}
	mReport = data->reports;

	//first come in effect
	{
		mFirstIn = false;
		if(! History::getHistory()->getBoolFromCurrentHero(Key_FirstOpenHeroRankList, mFirstIn))
		{
			mFirstIn = true;
		}
		if (mFirstIn)
		{
			mHeadMenu->setVisible(false);
			firstComeInWithAnimate();
			History::getHistory()->putInCurrentHero(Key_FirstOpenHeroRankList, false);
		}
	}

	if (mFlag == 0)
	{
		menuItemClicked_HeadMenu(mHeadMenu->getChildByTag(Tag_HeadMenu_HeroList));
	}
	else if (mFlag == 1)
	{
		menuItemClicked_HeadMenu(mHeadMenu->getChildByTag(Tag_HeadMenu_Report));
	}
	else if (mFlag == 10)  
	{
		//放在list加载后面
		menuItemClicked_HeadMenu(mHeadMenu->getChildByTag(Tag_HeadMenu_HeroList));
		//挑战后回来  点击的查看阵容
		mClickedHeroId = s_ChallengeHeroId;
		this->addObserver(callfuncO_selector(HeroRankList::queryAnotherTeamInfoRsp), MSG_queryAnotherTeamInfoRsp_F);
		mCmdHlp->cmdQueryAnotherTeamInfo(mClickedHeroId);
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_Waiting_Layer);
	}
}

void HeroRankList::menuItemClicked_HeadMenu(CCObject* pSender)
{
	CCMenuItemImage* listItem = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenu_HeroList);
	CCMenuItemImage* zhanbaoItem = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenu_Report);
	listItem->initWithNormalImage(
		"yxb_anniu_yingxiongpaiming.png",
		"yxb_anniu_yingxiongpaiming_select.png",
		NULL, 
		this, menu_selector(HeroRankList::menuItemClicked_HeadMenu));
	zhanbaoItem->initWithNormalImage(
		"yxb_anniu_zhanbao.png",
		"yxb_anniu_zhanbao_select.png",
		NULL, 
		this, menu_selector(HeroRankList::menuItemClicked_HeadMenu));

	if (! mFirstIn) clearBg();

	int tag = ((CCMenuItemImage*)pSender)->getTag();
	if (tag == Tag_HeadMenu_HeroList)
	{
		listItem->initWithNormalImage(
			"yxb_anniu_yingxiongpaiming_select.png",
			"yxb_anniu_yingxiongpaiming.png",
			NULL, 
			this, menu_selector(HeroRankList::menuItemClicked_HeadMenu));
		mFlag = 0;
		displayRankList();
	}
	else if (tag == Tag_HeadMenu_Report)
	{
		zhanbaoItem->initWithNormalImage(
			"yxb_anniu_zhanbao_select.png",
			"yxb_anniu_zhanbao.png",
			NULL, 
			this, menu_selector(HeroRankList::menuItemClicked_HeadMenu));
		mFlag = 1;
		displayReport();
	}

}

void HeroRankList::clearBg()
{
	mHeadMenu->retain();
	mBg->removeAllChildren();
	mBg->addChild(mHeadMenu);
	mHeadMenu->setPosition(ccp(0,0));
	mHeadMenu->release();

	mTableView = NULL;
}

void HeroRankList::displayRankList()
{
	//table view

	if(mTableView == NULL)
	{
		BaseSprite *b = BaseSprite::create("yingxiongbang_zhuangtailan.png");
		mBg->addChild(b, 0, Tag_HeroListInfoBg);
		b->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-473));
	
		CCLabelTTF *tishi = CCLabelTTF::create(LFStrings::getValue("YingXiongBang_DianFengDuiJueTiShi").c_str(), fontStr_kaiti, 22);
		b->addChild(tishi, 0, Tag_Label_Qian8TiShi);
		tishi->setPosition(ccp(b->getContentSize().width/2, b->getContentSize().height-24));
		tishi->setColor(ccc3(255,255,255));
		tishi->setOpacity(0); // 教学的时候 要找这个label
		//
		CCLabelTTF *timeLabel = CCLabelTTF::create("", fontStr_kaiti, 20);
		mBg->addChild(timeLabel, 10, Tag_Label_Time);
		timeLabel->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-802));
		//
		mTableView = CCTableView::create(this, CCSizeMake(530, 536));
		mBg->addChild(mTableView);
		mTableView->setDirection(kCCScrollViewDirectionVertical);
		mTableView->setPosition(ccp(55, mBg->getContentSize().height-784));
		mTableView->setDelegate(this);
		mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	}
	
	mTableView->reloadData();
	if (mMyOrder > 7)
	{
		int offset = 0;
		for (int i=0; i!=mHeroVec.size(); ++i)
		{
			if (mHeroVec.at(i).order == mMyOrder)
			{
				offset = (i-6)*tableCellSizeForIndex(mTableView, 0).height;
				break;
			}
		}
		mTableView->setContentOffset(ccp(0, mTableView->getContentOffset().y+offset));
	}

	if(mFirstIn)
	{
		disableAllTouchBegin();
		mFirstIn = false;
	}
}

void HeroRankList::displayReport()
{
	//table view
	if(mTableView == NULL)
	{
		BaseSprite *b = BaseSprite::create("yingxiongbang_zhanbao_zhuangtailan.png");
		mBg->addChild(b, 0);
		b->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-473));
		//
		mTableView = CCTableView::create(this, CCSizeMake(530, 620));
		mBg->addChild(mTableView);
		mTableView->setPosition(ccp(55, mBg->getContentSize().height-784));
		mTableView->setDelegate(this);
		mTableView->setDirection(kCCScrollViewDirectionVertical);
		mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	}

	mTableView->reloadData();
}

void HeroRankList::firstComeInWithAnimate()
{
	scheduleOnce(schedule_selector(HeroRankList::firstInWaiting), 0);


	BaseSprite* bg_img = BaseSprite::create("yingxiongbang_donghua_shu.png");
	mBg->addChild(bg_img, 1000);
	bg_img->setPosition(ccp(bg_img->getContentSize().width/2, bg_img->getContentSize().height/2));

	BaseSprite *bj_img = BaseSprite::create("yingxiongbang_donghua_jian.png");
	bg_img->addChild(bj_img);
	bj_img->setPosition(ccp(319, 860-432));
	bj_img->runAction(CCFadeOut::create(1.5));

	CCLabelTTF *label = CCLabelTTF::create(LFStrings::getValue("yingxiongbanglabel").c_str(), fontStr_kaiti, 23, CCSize(400,0), kCCTextAlignmentLeft);
	bg_img->addChild(label);
	label->setColor(ccc3(0,0,0));
	label->setPosition(bj_img->getPosition());
	label->setOpacity(0);
	label->runAction(CCSequence::create(
		CCDelayTime::create(1.5)
		, CCFadeIn::create(2)
		, CCDelayTime::create(2.5)
		, CCFadeOut::create(1.0)
		, NULL));

	bg_img->runAction(CCSequence::create(
		CCDelayTime::create(6.0)
		, CCFadeOut::create(1.5)
		, CCCallFuncN::create(this, callfuncN_selector(HeroRankList::callfuncActionRemoveFromParent))
		, NULL));
}

void HeroRankList::firstInWaiting(float dt)
{
	WaitLayer* layer = WaitLayer::create("");
	layer->removeAllChildren();
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1000, TAG_Waiting_Layer);
}

void HeroRankList::callfuncActionRemoveFromParent(CCNode* node)
{
	node->removeFromParent();
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_Waiting_Layer);
	//

	disableAllTouchEnd();
	mHeadMenu->setVisible(true);
	CCArray *child = mHeadMenu->getChildren();
	if(child != NULL)
	{
		for(int i=0; i!=child->count(); ++i)
		{
			CCMenuItem* item = ((CCMenuItem*)child->objectAtIndex(i));
			item->setOpacity(60);
			item->runAction(CCFadeIn::create(0.25));
		}
	}


	//教学
	if (g_YingXiongBang_Opend)
		teachGuide();
}

void HeroRankList::menuItemClicked_Challenge(CCObject* pSender)
{

	CCMenuItemImage* item = (CCMenuItemImage*) pSender;
	int heroId = (int)item->getUserData();
	s_ChallengeHeroId = heroId; // HeroRankChallengeEndLayer -> 再次挑战、查看阵容
	s_ChallengeOrder  = getOrderByHeroId(heroId);
	if (s_ChallengeOrder == 0)
	{
//		CCAssert(false, "list challenge cannot found hero in heroList");
		CCLog("Error: %s --> cannot found hero in heroList, hero id = [%u], order=0", __FUNCTION__, heroId);
	}


	if (mNextChlgTime > ServerTime::getTime())
	{
		//弹框背景
		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
		dialog->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
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
				this,SEL_MenuHandler(&HeroRankList::menuItemClicked_ClearTime));


			CCMenuItemImage* menuItemCacel = CCMenuItemImage::create(
				"shangpin_goumai_quxiao.png",
				"shangpin_goumai_quxiao_select.png",
				this,SEL_MenuHandler(&HeroRankList::menuItemClicked_ClearTime));

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
			CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert, 128, 144);
			lfAlert->release();
		}
		return;
	}



	addObserver(callfuncO_selector(HeroRankList::rsp_topHeroChallege), MSG_topHeroChallengeRsp_F);
	mCmdHlp->cmdTopHeroChallenge(s_ChallengeHeroId);

}

void HeroRankList::rsp_topHeroChallege(CCObject* msg_f)
{
	removeObserver(MSG_topHeroChallengeRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_TopHeroChallengeRsp* data = (Flat_TopHeroChallengeRsp*) ((Flat_MSG_Rsp*)msg_f)->getData();

	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1025);
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

void HeroRankList::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	if (mFlag == 1) return;

	mClickedHeroId = mHeroVec.at(cell->getIdx()).hero_info.hero_id;
//	BaseSprite* board = (BaseSprite*)cell->getChildren()->objectAtIndex(0);

	this->addObserver(callfuncO_selector(HeroRankList::queryAnotherTeamInfoRsp), MSG_queryAnotherTeamInfoRsp_F);
	mCmdHlp->cmdQueryAnotherTeamInfo(mClickedHeroId);

}

void HeroRankList::tableCellHighlight(CCTableView* table, CCTableViewCell* cell)
{
	if (mFlag == 1) return;

	BaseSprite* board = (BaseSprite*)cell->getChildren()->objectAtIndex(0);
	ccColor3B color = {0xa2,0xa2,0xa2};
	board->setColor(color);
}

void HeroRankList::tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell)
{
	if (mFlag == 1) return;

	BaseSprite* board = (BaseSprite*)cell->getChildren()->objectAtIndex(0);
	ccColor3B color = {0xff,0xff,0xff};
	board->setColor(color);
}

void HeroRankList::queryAnotherTeamInfoRsp(CCObject* msg_f)
{
	removeObserver(MSG_queryAnotherTeamInfoRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_QueryAnotherTeamInfoRsp* data = (Flat_QueryAnotherTeamInfoRsp*) ((Flat_MSG_Rsp*)msg_f)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1025);
		return;
	}

	vector<HeroDetail> heroDetailVec;
	for(int i=0; i!=4; ++i)
	{
		if(data->hero_arr[i].basic_info.hero_id != 0) 
			heroDetailVec.push_back(data->hero_arr[i]);
		else continue;
	}
	vector<EquipData> heroEquipVec[4];  //按照 heroDetail排序
	for (int i=0; i!=data->zhuangbei_vec.size(); ++i)
	{
		for(int j=0; j!=heroDetailVec.size(); ++j)
		{
			if(data->zhuangbei_vec.at(i).zhuangbei_hero_id == heroDetailVec.at(j).basic_info.hero_id)
			{
				heroEquipVec[j].push_back(data->zhuangbei_vec.at(i));
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//创建actor  并查询元神
	mQueryYuanShen = new CCArray;
	mQueryYuanShen->init();
	mQueryHeroActors = new CCArray;     //----remember delete (获取元神数据后，创建并传递给ZhuangBeiMianBanOther后，再release)
	mQueryHeroActors->initWithCapacity(heroDetailVec.size());
	mQueryActorsEquip.clear();
	mNum_YuanShen = 0;
	for (int i=0; i!=heroDetailVec.size(); ++i)
	{
		BaseActor *heroActor = NULL;
		HeroBaseProperty propData = HeroBaseProperty(&heroDetailVec.at(i));
		propData.initFixedProValue(BaseActorPropertyParser::getParser()->getExtraProerty(propData.profession));
		heroActor = new BaseActor(propData); // 1. remember delete actor
		vector<Equipment*> equipVec;   //2. will delete equips
		for(int eq_idx=0; eq_idx!=heroEquipVec[i].size(); ++eq_idx)
		{
			equipVec.push_back(new Equipment(heroEquipVec[i].at(eq_idx)));
		}
		heroActor->initEquipment(equipVec);
		//skill
		for (int skillIdx=0; skillIdx<4; ++skillIdx)
		{
			if (data->skill_arr[skillIdx].skill_id != 0 && data->skill_arr[skillIdx].owner_hero_id == heroActor->getId()){
				SkillData *skill_data = new SkillData(data->skill_arr[skillIdx]);
				heroActor->setCurrentUseSkill(skill_data);
				delete skill_data;
			}
		}
		
		mQueryHeroActors->addObject(heroActor);
		heroActor->release();  // delete actor
		for (int eq_idx=0; eq_idx!=equipVec.size(); ++eq_idx) // delete equips 传递给mianbanOther，等它用完之后再删除、 
			mQueryActorsEquip.push_back(equipVec.at(eq_idx));

		if (heroDetailVec.at(i).basic_info.level >= 30) ++mNum_YuanShen; 
	}

	/////////  query 元神info  
	if (mNum_YuanShen > 0)
	{
		addObserver(callfuncO_selector(HeroRankList::queryAnotherTeamYuanshenRsp), MSG_queryYuanshenOfHeroRsp_F);
		for (int i=0; i!=heroDetailVec.size(); ++i)
		{
			if (heroDetailVec.at(i).basic_info.level >= 30)
				mCmdHlp->cmdQueryYuanshenOfHero(heroDetailVec.at(i).basic_info.hero_id);
		}
	}
	else
	{
		queryAnotherTeamYuanshenRsp(NULL);
	}
}

void HeroRankList::queryAnotherTeamYuanshenRsp(CCObject *msg_f)
{
	if (msg_f != NULL)  //==NULL表示 没有元神
	{
		Flat_QueryYuanshenOfHeroRsp *data = (Flat_QueryYuanshenOfHeroRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
		if (data->err_code != Success)
		{
			removeObserver(MSG_queryYuanshenOfHeroRsp_F);
			GameDirector::getDirector()->hideWaiting();
			MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		BaseActor *yuanshen_actor = NULL;
		HeroBaseProperty propData = HeroBaseProperty(&data->hero_detail);
		propData.initFixedProValue(BaseActorPropertyParser::getParser()->getExtraProerty(propData.profession));
		yuanshen_actor = new BaseActor(propData); // 1. remember delete actor
		vector<Equipment*> equipVec;   //2. will delete equips
		for(int eq_idx=0; eq_idx!=data->equips.size(); ++eq_idx)
		{
			equipVec.push_back(new Equipment(data->equips.at(eq_idx)));
		}
		yuanshen_actor->initEquipment(equipVec);
		for (int i=0; i!=mQueryHeroActors->count(); ++i)
		{
			BaseActor *curhero = (BaseActor*)mQueryHeroActors->objectAtIndex(i);
			if (curhero->getCurrentProperty().hero_id == data->hero_detail.basic_info.zhenshen_id)
			{
				((BaseActor*)mQueryHeroActors->objectAtIndex(i))->setYuanShen(yuanshen_actor);
				break;
			}
		}

		//内存管理
		mQueryYuanShen->addObject(yuanshen_actor);
		yuanshen_actor->release();        // delete actor 1
		for (int i=0; i!=equipVec.size(); ++i)
			mQueryActorsEquip.push_back(equipVec.at(i)); // delete equips 2

		--mNum_YuanShen;
		if (mNum_YuanShen != 0) return;

		removeObserver(MSG_queryYuanshenOfHeroRsp_F);
		GameDirector::getDirector()->hideWaiting();
	}


	///////////////
	ZhuangBeiMianBanOther *equipDetailLayer = ZhuangBeiMianBanOther::create(mQueryHeroActors, mQueryYuanShen, mQueryActorsEquip, 0);
	mFooterLayer->popUpContaintLayer(equipDetailLayer);

	mQueryHeroActors->release();   //查询hero info创建actor的时候 new的、
	mQueryHeroActors = NULL;
	mQueryYuanShen->release();
	mQueryYuanShen = NULL;
}

void HeroRankList::disableAllTouchBegin()
{
	mHeadMenu->setEnabled(false); 
	mTableView->setTouchEnabled(false);
	for(int i=0; i!=numberOfCellsInTableView(mTableView); ++i)
	{
		CCTableViewCell *cell = mTableView->cellAtIndex(i);
		if(cell != NULL)
		{
			BaseSprite *bg = (BaseSprite*)cell->getChildByTag(Tag_Board);
			if(bg != NULL)
			{
				CCMenu *menu = (CCMenu*)bg->getChildByTag(Tag_Menu);
				if (menu) menu->setEnabled(false);
				
			}
		}
	}
}

void HeroRankList::disableAllTouchEnd()
{
	mHeadMenu->setEnabled(true); 
	mTableView->setTouchEnabled(true);
	for(int i=0; i!=numberOfCellsInTableView(mTableView); ++i)
	{
		CCTableViewCell *cell = mTableView->cellAtIndex(i);
		if(cell != NULL)
		{
			BaseSprite *bg = (BaseSprite*)cell->getChildByTag(Tag_Board);
			if(bg != NULL)
			{
				CCMenu *menu = (CCMenu*)bg->getChildByTag(Tag_Menu);
				if (menu) menu->setEnabled(true);
			}
		}
	}
}

unsigned int HeroRankList::numberOfCellsInTableView(CCTableView *table)
{
	if (mFlag == 1)
		return mReport.size();
	else 
		return mHeroVec.size();
}

CCSize HeroRankList::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	if (mFlag == 1)
		return CCSize(530, 144);
	else 
		return CCSize(530, 76);
}

CCTableViewCell* HeroRankList::tableCellAtIndex(CCTableView *table, unsigned int idx)
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

	if (table != mTableView) return cell;
	CCString temp;

	//0 list   1 战报
	if (mFlag == 0)
	{
		BaseSprite *board = NULL;
		if (idx == 0)		board = BaseSprite::create("yingxiongbang_geren_xinxi_1.png");
		else if (idx == 1)  board = BaseSprite::create("yingxiongbang_geren_xinxi_2.png");
		else if (idx == 2)  board = BaseSprite::create("yingxiongbang_geren_xinxi_3.png");
		else if (idx>2 && idx<10) board = BaseSprite::create("yingxiongbang_geren_xinxi_qianshi.png");
		else				board = BaseSprite::create("yingxiongbang_geren_xinxi.png");

		cell->addChild(getTableCellCountent(board, idx), 0, Tag_Board);

		CCSize size = tableCellSizeForIndex(table, idx);
		board->setPosition(ccp(size.width/2, size.height/2));
	}
	else if (mFlag == 1)
	{
		TopHeroReport oneRep = mReport.at(idx);

		BaseSprite *board = BaseSprite::create("yingxiongbang_zhanbao_xinxi_bg.png");
		cell->addChild(board, 0, Tag_Board);
		CCSize size = tableCellSizeForIndex(table, idx);
		board->setPosition(ccp(size.width/2, size.height/2));
		FXScrollMenu* menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		board->addChild(menu, 1, Tag_Menu);
		menu->setPosition(CCPointZero);
		//查看阵容
		CCMenuItemImage *ite = CCMenuItemImage::create(
			"zhandou_jiesuan_anniu_chakanzhenrong.png",
			"zhandou_jiesuan_anniu_chakanzhenrong_select.png",
			this, menu_selector(HeroRankList::menuItemClicked_ReportBarMenu));
		menu->addChild(ite, 1, 10);
		ite->setPosition(ccp(415, 46));
		ite->setScale(0.8f);
		//时间
		CCLabelTTF *timeLabel = CCLabelTTF::create(ServerTime::getTimeString(oneRep.fight_time).c_str(), fontStr_kaiti, 20);
		board->addChild(timeLabel);
		timeLabel->setAnchorPoint(ccp(0, 0.5));
		timeLabel->setPosition(ccp(26, 34));

		//被挑战 还是去挑战
		bool bChlg = true; //默认去挑战
		if(MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().hero_id != oneRep.challenger.hero_info.hero_id)
			bChlg = false; //挑战者不是我  那么我就是被挑战了
		if (bChlg)
		{
			ite->setUserData((void*)oneRep.be_challenger.hero_info.hero_id);
			//你挑战XXX（被挑战者名）胜利，排名升至   ！
			//你挑战XXX（被挑战者名）失利，排名不变！\n提升装备再去试试吧！  
			temp.initWithFormat("%s%s", LFStrings::getValue("ni").c_str(), LFStrings::getValue("TiaoZhan").c_str());
			CCLabelTTF *ntzL = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			ntzL->setColor(ccBLACK);
			board->addChild(ntzL);
			ntzL->setAnchorPoint(ccp(0, 0.5));
			ntzL->setPosition(ccp(26, 104));
			//name
			CCLabelTTF *nameLabel = CCLabelTTF::create(oneRep.be_challenger.hero_info.name, fontStr_kaiti, 20);
			board->addChild(nameLabel);
			nameLabel->setPosition(ccp(ntzL->getPositionX()+ntzL->getContentSize().width+nameLabel->getContentSize().width/2, ntzL->getPositionY()));
			if (oneRep.challenger_win)
			{//挑战别人 胜利
				temp.initWithFormat("%s!, %s%s", LFStrings::getValue("shengli").c_str(), LFStrings::getValue("paiming").c_str(), 
					LFStrings::getValue("ShengZhi").c_str());
				CCLabelTTF *la = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
				board->addChild(la);
				la->setColor(ccBLACK);
				la->setPosition(ccp(nameLabel->getPositionX()+nameLabel->getContentSize().width/2+la->getContentSize().width/2, nameLabel->getPositionY()));
				temp.initWithFormat("%u", oneRep.challenger.order);
				CCLabelTTF *orL = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
				board->addChild(orL);
				orL->setPosition(ccp(la->getPositionX()+la->getContentSize().width/2+orL->getContentSize().width/2, la->getPositionY()));
			}
			else
			{//挑战别人 失败
				temp.initWithFormat("%s!,%s%s!", LFStrings::getValue("shibai").c_str(), LFStrings::getValue("paiming").c_str(), 
					LFStrings::getValue("BuBian").c_str());
				CCLabelTTF *la = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
				board->addChild(la);
				la->setColor(ccBLACK);
				la->setPosition(ccp(nameLabel->getPositionX()+nameLabel->getContentSize().width/2+la->getContentSize().width/2, nameLabel->getPositionY()));
				temp.initWithFormat("%s", LFStrings::getValue("TiShengZBZaiShi").c_str());
				CCLabelTTF *orL = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
				board->addChild(orL);
				orL->setAnchorPoint(ccp(0, 0.5));
				orL->setPosition(ccp(ntzL->getPositionX(), ntzL->getPositionY() - 25));
				orL->setColor(ccBLACK);
			}
		}
		else
		{
			ite->setUserData((void*)oneRep.challenger.hero_info.hero_id);
			//XXX（挑战者名）挑战了你，你实力强大，守住了排名！
			//XXX（挑战者名）挑战了你，很遗憾，你被击败了!\n排名降至   ！
			//name
			CCLabelTTF *nameLabel = CCLabelTTF::create(oneRep.challenger.hero_info.name, fontStr_kaiti, 20);
			board->addChild(nameLabel);
			nameLabel->setAnchorPoint(ccp(0,0.5));
			nameLabel->setPosition(ccp(26, 104));
			if (oneRep.challenger_win)
			{// 被人挑战 我输了
				temp.initWithFormat("%s,%s,%s!", (LFStrings::getValue("TiaoZhan")+LFStrings::getValue("LE")+LFStrings::getValue("ni")).c_str()
					,LFStrings::getValue("yihan").c_str(), LFStrings::getValue("NiBeiJiBai").c_str());
				CCLabelTTF *la = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
				board->addChild(la);
				la->setPosition(ccp(nameLabel->getPositionX()+nameLabel->getContentSize().width+la->getContentSize().width/2, nameLabel->getPositionY()));
				la->setColor(ccBLACK);
				//
				temp.initWithFormat("%s", (LFStrings::getValue("paiming")+LFStrings::getValue("JiangZhi")).c_str());
				CCLabelTTF *pmjzLa = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
				board->addChild(pmjzLa);
				pmjzLa->setAnchorPoint(ccp(0,0.5));
				pmjzLa->setColor(ccBLACK);
				pmjzLa->setPosition(ccp(nameLabel->getPositionX(), nameLabel->getPositionY() - 25));
				temp.initWithFormat("%u", oneRep.be_challenger.order);
				CCLabelTTF *numL = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
				board->addChild(numL);
				numL->setPosition(ccp(pmjzLa->getPositionX()+pmjzLa->getContentSize().width+numL->getContentSize().width/2, pmjzLa->getPositionY()));
				//menu
				CCMenuItemImage *fanji = CCMenuItemImage::create(
					"yingxiongbang_anniu_fanji.png",
					"yingxiongbang_anniu_fanji_select.png",
					this, menu_selector(HeroRankList::menuItemClicked_ReportBarMenu));
				menu->addChild(fanji, 1, 100);
				fanji->setPosition(ccp(316, 46));
				fanji->setScale(0.82f);
				fanji->setUserData((void*)oneRep.challenger.hero_info.hero_id);
				{
					if (! canChallenge(oneRep.other_order, mMyOrder)) fanji->setVisible(false);
				}
			}
			else
			{//被人挑战  我赢了
				temp.initWithFormat("%s,%s%s!", (LFStrings::getValue("TiaoZhan")+LFStrings::getValue("LE")+LFStrings::getValue("ni")).c_str()
					,(LFStrings::getValue("ni")+LFStrings::getValue("ShiLiQiangDa")).c_str(), LFStrings::getValue("ShouZhuPaiMing").c_str());
				CCLabelTTF *la = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
				board->addChild(la);
				la->setPosition(ccp(nameLabel->getPositionX()+nameLabel->getContentSize().width+la->getContentSize().width/2, nameLabel->getPositionY()));
				la->setColor(ccBLACK);
			}
		}
	}
	

	return cell;
}

BaseSprite* HeroRankList::getTableCellCountent(BaseSprite* bg, int idx)
{
	//如果是自己 就显示白色字体,默认是黑色字体
	ccColor3B color = ccBLACK;//{39,0,0};
	//排名号
	int orderNum = 0;
	//name
	string nameStr;
	// zhiye 
	string zhiyeStr;
	//level
	int level = 0;
	//战斗力
	int zhandouLi = 0;
	bool bMine = false;

	unsigned int myid = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().hero_id;
	for (int i = 0; i < mReport.size(); ++i)
	{
		bool bChange = false;
		int curheroId = mHeroVec.at(idx).hero_info.hero_id ;
		if (curheroId== mReport.at(i).challenger.hero_info.hero_id
			|| curheroId == mReport.at(i).be_challenger.hero_info.hero_id)
		{
			bChange = true;
		}
		if (bChange)
		{
			//被挑战 还是去挑战
			bool bChlg = true; //默认被挑战
			if(myid == mReport.at(i).challenger.hero_info.hero_id)
			{
				bChlg = false; //挑战者是我
			}
			else
			{
				bChlg = true;  //挑战者不是我，被挑战
			}
			if (bChange)
			{
				//被挑战
				if (bChlg)
				{
					if (mReport.at(i).challenger_win)
					{//我失败
						color = ccc3(135, 17, 0);
					}
				}
				//挑战别人
				else
				{
					if (!mReport.at(i).challenger_win)
					{//我失败
						color = ccc3(135, 17, 0);
					}
				}
			}
			break;
		}
	}

	//前三名使用白色字体
	if (idx <= 2)
	{
		color = fonColor_ChengGong;
	}

	HeroAndOrder temp = mHeroVec.at(idx);
	if(temp.order == mMyOrder)
	{//是我  使用成功字体
		color    = ccWHITE;//ccc3(255,255,255);
		bMine = true;
	}

	orderNum   = mHeroVec.at(idx).order;
	nameStr    = mHeroVec.at(idx).hero_info.name;
	level      = mHeroVec.at(idx).hero_info.level;
	zhandouLi  = mHeroVec.at(idx).zhandouli;
	switch (mHeroVec.at(idx).hero_info.profession_id)
	{
	case Profession_Yemanren:
		zhiyeStr = LFStrings::getValue("yeManRen");
		break;
	case Profession_Chike:
		zhiyeStr = LFStrings::getValue("ciKe");
		break;
	case Profession_Fashi:
		zhiyeStr = LFStrings::getValue("faShi");
		break;
	default:
		CCLog("Error: HeroRankList::getTableCellCountent(BaseSprite* bg, int idx) ---> zhiye name error! [switch default]");
		break;
	}
	
	CCSize size = bg->getContentSize();
	char buf[20];
	//order
	sprintf(buf, "%d", orderNum);
	CCLabelTTF* orderLabel = CCLabelTTF::create(buf, fontStr_kaiti, SIZE_STR);
	bg->addChild(orderLabel, 1, Tag_Label_Order);
	orderLabel->setAnchorPoint(ccp(0,0.5));
	orderLabel->setPosition(ccp(48, size.height - 46));
	orderLabel->setColor(color);
	//name
	CCLabelTTF* nameLabel = CCLabelTTF::create(nameStr.c_str(), fontStr_kaiti, SIZE_STR);
	bg->addChild(nameLabel, 1, Tag_Label_Name);
	nameLabel->setAnchorPoint(ccp(0,0.5));
	nameLabel->setPosition(ccp(105, orderLabel->getPositionY()));
	nameLabel->setColor(color);
	//profession
	CCLabelTTF* professionLabel = CCLabelTTF::create(zhiyeStr.c_str(), fontStr_kaiti, SIZE_STR);
	bg->addChild(professionLabel, 1, Tag_Label_Profession);
	professionLabel->setAnchorPoint(ccp(0,0.5));
	professionLabel->setPosition(ccp(270, orderLabel->getPositionY()));
	professionLabel->setColor(color);
// 	//level
// 	sprintf(buf, "Lv%d", level);
// 	CCLabelTTF* levelLabel = CCLabelTTF::create(buf, fontStr_kaiti, SIZE_STR);
// 	bg->addChild(levelLabel, 1, Tag_Label_Level);
// 	levelLabel->setAnchorPoint(ccp(0,0.5));
// 	levelLabel->setPosition(ccp(338, size.height/2));
// 	levelLabel->setColor(color);
	//战力值
	sprintf(buf, "%d", zhandouLi);
	CCLabelTTF* zlzLabel = CCLabelTTF::create(buf, fontStr_kaiti, SIZE_STR);
	bg->addChild(zlzLabel, 1, Tag_Label_ZhanDouLi);
	zlzLabel->setAnchorPoint(ccp(0,0.5));
	zlzLabel->setPosition(ccp(354, orderLabel->getPositionY()));
	zlzLabel->setColor(color);
	//menu
	if (! bMine)
	{
		FXScrollMenu* menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		bg->addChild(menu, 1, Tag_Menu);
		menu->setPosition(CCPointZero);
		//挑战
		CCMenuItemImage* item = CCMenuItemImage::create(
			"yingxiongbang_geren_xinxi_tiaozhan.png"
			, "yingxiongbang_geren_xinxi_tiaozhan_select.png"
			, this, menu_selector(HeroRankList::menuItemClicked_Challenge));
		menu->addChild(item, 1, Tag_MenuItem_Challege);
		item->setUserData((void*)mHeroVec.at(idx).hero_info.hero_id);
		item->setPosition(ccp(492, orderLabel->getPositionY()));
		//查看
		CCMenuItemImage* queryItem = CCMenuItemImage::create(
			"yingxiongbang_geren_xinxi_chakan.png"
			, "yingxiongbang_geren_xinxi_chakan_select.png"
			, this, menu_selector(HeroRankList::menuItemClicked_Query));
			//, this, NULL);
		menu->addChild(queryItem, 1, Tag_MenuItem_Query);
		queryItem->setUserData((void*)mHeroVec.at(idx).hero_info.hero_id);
		queryItem->setPosition(ccp(492, orderLabel->getPositionY()));
		queryItem->setVisible(false);
		if (! canChallenge(temp.order, mMyOrder))
		{
			item->setVisible(false);
			queryItem->setVisible(true);
		}

		if (temp.order == mMyOrder) 
		{
			queryItem->setVisible(false);
		}

	}
	

	return bg;
}

void HeroRankList::menuItemClicked_Query(CCObject *pSender)
{
	unsigned int heroId = (unsigned int)((CCMenuItem*)pSender)->getUserData();
	mClickedHeroId = heroId;

	this->addObserver(callfuncO_selector(HeroRankList::queryAnotherTeamInfoRsp), MSG_queryAnotherTeamInfoRsp_F);
	mCmdHlp->cmdQueryAnotherTeamInfo(mClickedHeroId);
}

void HeroRankList::teachGuide()
{
	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstYingXiongBangPlay))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstYingXiongBangPlay);
		g_YingXiongBang_Opend = false;

		CCLabelTTF *label = (CCLabelTTF*)mBg->getChildByTag(Tag_HeroListInfoBg)->getChildByTag(Tag_Label_Qian8TiShi);
		if (label)
		{
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(
				CCSize(label->getContentSize().width+20, label->getContentSize().height+20)
				, label->getParent()->convertToWorldSpace(label->getPosition())
				, LFStrings::getValue("yingXiongBang_QianBaTiShi"), NULL, true);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
	}
}

//1. 列表挑战
//2. 战报反击
int HeroRankList::getOrderByHeroId(unsigned int id)
{
	int num = mHeroVec.size();
	//
	for ( int i=0; i< num; ++i)
	{
		if (mHeroVec.at(i).hero_info.hero_id == id)
			return mHeroVec.at(i).order;
	}

	//列表里面没找到、 那么就是在战报里面
	for (int i=0; num=mReport.size(); ++i)
	{
		if (mReport.at(i).challenger.hero_info.hero_id == id)
			return mReport.at(i).other_order;
	}


	return 0;
}

void HeroRankList::menuItemClicked_ReportBarMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	unsigned int heroId = (unsigned int)((CCMenuItem*)pSender)->getUserData();

	if (tag == 10)
	{//查看阵容
		this->addObserver(callfuncO_selector(HeroRankList::queryAnotherTeamInfoRsp), MSG_queryAnotherTeamInfoRsp_F);
		mCmdHlp->cmdQueryAnotherTeamInfo(heroId);
	}
	else if (tag == 100)//反击
	{
		s_ChallengeHeroId = heroId; // HeroRankChallengeEndLayer -> 再次挑战、查看阵容
		s_ChallengeOrder  = getOrderByHeroId(heroId);
		if (s_ChallengeOrder == 0)
		{
//			CCAssert(false, "report challenge cannot found hero in heroList");
			CCLog("Error: %s --> cannot found hero in heroList, hero id = [%u], order=0", __FUNCTION__, heroId);
		}

		addObserver(callfuncO_selector(HeroRankList::rsp_topHeroChallege), MSG_topHeroChallengeRsp_F);
		mCmdHlp->cmdTopHeroChallenge(heroId);
	}
}

bool HeroRankList::canChallenge(unsigned int beChallengerOrder, unsigned int challengerOrder)
{
	//挑战逻辑
	// 			4.	高于20位的玩家信息显示前10名排名的玩家信息（排名未到11时，前10只能查看，不能挑战）
	// 			5.	只有玩家排名=11时，才可挑战第10名的英雄玩家
	// 			6.	第10名玩家可以挑战4-9位的任意玩家(第9名玩家可挑战4-8位的任意玩家，以此类推)
	// 			7.	只有玩家排名=4时，才可挑战第3名的史诗英雄玩家
	// 			8.	只有玩家排名=3时，才可挑战第2名的史诗英雄玩家
	// 			9.	只有玩家排名=2时，才可挑战第1名的史诗英雄玩家
	// 			10.	当玩家10=<排名<=1时，显示1-20的玩家信息 
	if (challengerOrder > 11)
	{
		if (beChallengerOrder <= 10) 
		{//前10不能挑战
			return false;
		}
	}
	else if (challengerOrder == 11)
	{
		if (beChallengerOrder <= 9) 
		{// order=10   前9不能挑战
			return false;
		}
	}
	else if (challengerOrder <= 10 && challengerOrder > 4)
	{
		if (beChallengerOrder <= 3) 
		{// 前3不能挑战
			return false;
		}
	}
	else if (challengerOrder == 4)
	{
		if (beChallengerOrder <= 2) 
		{
			return false;
		}
	}
	else if (challengerOrder == 3)
	{
		if (beChallengerOrder <= 1) 
		{
			return false;
		}
	}

	//排在自己后面的 也不能挑战
	if (beChallengerOrder >= challengerOrder) 
	{
		return false;
	}


	return true;
}

void HeroRankList::schedule_updateChlgTime(float dt)
{
	unsigned int needTime = mNextChlgTime - ServerTime::getTime();
	CCLabelTTF *label = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Time);
	if (! label ) return;

	if (mNextChlgTime < ServerTime::getTime())
	{
		unschedule(schedule_selector(HeroRankList::schedule_updateChlgTime));
		label->setString("");
		return;
	}
	else
	{
		char buf[128];
		sprintf(buf, "%s %2u:%2u", LFStrings::getValue("TiaoZhanLengQue").c_str(), needTime/60, needTime % 60);
		label->setString(buf);
	}

}

void HeroRankList::menuItemClicked_ClearTime(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(144);

	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == 10) //make sure
	{
		addObserver(callfuncO_selector(HeroRankList::rsp_clearCDTime), MSG_TopHeroClearCDTimeRsp);
		mCmdHlp->cmdTopHeroClearCDTime();
	}
}

void HeroRankList::rsp_clearCDTime(CCObject *msg)
{
	removeObserver(MSG_TopHeroClearCDTimeRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_TopHeroClearCDTimeRsp* data = (SPCmd_TopHeroClearCDTimeRsp*) ((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1025);
		return;
	}
	addObserver(callfuncO_selector(HeroRankList::rsp_topHeroChallege), MSG_topHeroChallengeRsp_F);
	mCmdHlp->cmdTopHeroChallenge(s_ChallengeHeroId);
}