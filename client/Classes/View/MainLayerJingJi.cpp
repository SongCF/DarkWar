#include "MainLayerJingJi.h"
#include "font.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "MyselfManager.h"
#include "LFStrings.h"
#include "ImageManager.h"
#include "JingJichangLayer.h"
#include "WuPinManager.h"
#include "UnblockManager.h"
#include "FightDirector.h"
#include "ClientFightLayer.h"
#include "MapsManager.h"
#include "CS_Common.h"
#include "LFAlert.h"
#include "ZhuangBeiMianBanOther.h"
#include "GuideLayerBase.h"
#include "FXScrollMenu.h"

int g_iGetRongYuDianShu = 12;			//战斗胜利后获取的荣誉点数

#define LABEL_SIZE 20

enum 
{
	Tag_MenuItem_Challenge = 1,
	Tag_MenuItem_Random,
	Tag_MenuItem_Buy,
	Tag_MenuItem_Exchange,
	Tag_MenuItem_Refresh,

	Tag_Board_ResidueNum,
	Tag_Board_FameNum,

	Tag_Board_Random,
	Tag_Menu_BegainRandom,
	Tag_Img_Random,

	Tag_Img_GoldIcon,
	Tag_Label_GoldNum,

	Tag_MenuItem_Back,
	Tag_Window_BuyResedueNum,
	Tag_Window_BuyItems,
	Tag_Window_BuyEnd,
	Tag_MarketItemBuyNum,

	Tag_TableCellBg = 1000,
	Tag_TableCellMenu,
};

MainLayerJingJi* MainLayerJingJi::create(int flag)
{
	MainLayerJingJi *layer = new MainLayerJingJi;
	if (layer && layer->init(flag))
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

bool MainLayerJingJi::init(int flag)
{
	mCurFlag = flag;
	mTableView = NULL;
	//
	mBg = BaseSprite::create("jingjichang_ziyou_bg.jpg");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
	//
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(ccp(0,0));
	CCMenuItemImage *itemFree = CCMenuItemImage::create(
		"jingjichang_anniu_ziyoumoshi.png",
		"jingjichang_anniu_ziyoumoshi_select.png",
		this, menu_selector(MainLayerJingJi::menuItemClicked_MainMenu));
	mMainMenu->addChild(itemFree, 0, Tag_MenuItem_Challenge);
	itemFree->setPosition(ccp(226, 851-61));
	CCMenuItemImage *itemRandom = CCMenuItemImage::create(
		"jingjichang_anniu_suijimoshi.png",
		"jingjichang_anniu_suijimoshi_select.png",
		this, menu_selector(MainLayerJingJi::menuItemClicked_MainMenu));
	mMainMenu->addChild(itemRandom, 0, Tag_MenuItem_Random);
	itemRandom->setPosition(ccp(426, 851-61));
	CCMenuItemImage *itemExchange = CCMenuItemImage::create(
		"jingjichang_anniu_duihuan.png",
		"jingjichang_anniu_duihuan_select.png",
		this, menu_selector(MainLayerJingJi::menuItemClicked_MainMenu));
	mMainMenu->addChild(itemExchange, 0, Tag_MenuItem_Exchange);
	itemExchange->setPosition(ccp(509, 851-186));
	CCMenuItemImage *itemBuy = CCMenuItemImage::create(
		"jingjichang_anniu_goumai.png",
		"jingjichang_anniu_goumai_select.png",
		this, menu_selector(MainLayerJingJi::menuItemClicked_BuyResedueNum));
	mMainMenu->addChild(itemBuy, 0, Tag_MenuItem_Buy);
	itemBuy->setPosition(ccp(219, 851-186));
	CCMenuItemImage *itemRefresh = CCMenuItemImage::create(
		"jingjichang_anniu_shuaxin.png",
		"jingjichang_anniu_shuaxin_select.png",
		this, menu_selector(MainLayerJingJi::menuItemClicked_RefreshList));
	mMainMenu->addChild(itemRefresh, 0, Tag_MenuItem_Refresh);
	itemRefresh->setPosition(ccp(370, 851-806));
	itemRefresh->setVisible(false);

	BaseSprite *board1 = BaseSprite::create("jingjichang_biaoti_shengyucishu.png");
	mBg->addChild(board1, 0, Tag_Board_ResidueNum);
	board1->setPosition(ccp(121, 851-171));
	CCLabelTTF *label1 = CCLabelTTF::create("", fontStr_BookAntiqua, LABEL_SIZE);
	board1->addChild(label1, 1, 1);
	label1->setPosition(ccp(44, 18));
	BaseSprite *board2 = BaseSprite::create("jingjichang_biaoti_rongyudian.png");
	mBg->addChild(board2, 0, Tag_Board_FameNum);
	board2->setPosition(ccp(406, 851-171));
	CCLabelTTF *label2 = CCLabelTTF::create("", fontStr_BookAntiqua, LABEL_SIZE);
	board2->addChild(label2, 1, 1);
	label2->setColor(ccGREEN);
	label2->setPosition(ccp(44, 18));

	scheduleOnce(schedule_selector(MainLayerJingJi::initLayerData), 0.1f);
	return true;
}

void MainLayerJingJi::initLayerData(float dt)
{
	if (mCurFlag == model_Challenge)
		menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_MenuItem_Challenge));
	else if (mCurFlag == model_random)
		menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_MenuItem_Random));
	else if (mCurFlag == model_store)
		menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_MenuItem_Exchange));
}

void MainLayerJingJi::rsp_QueryChallengeList(CCObject *msg_f)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_queryArenaChlgListRsp_F);

	Flat_QueryArenaChallengeListRsp *data = (Flat_QueryArenaChallengeListRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	mChaHeroList = data->hero_list;
	mChlgRefreshNeedGoldNum = data->item_need.count;
	CCString temp;
	if (data->item_need.type != 0)
	{
		BaseSprite *goldIcon = NULL;
		if (data->item_need.type == ItemsType_Gold)
			goldIcon = BaseSprite::create("jinbi_tubiao.png");
		else if (data->item_need.type == ItemsType_Silver)
			goldIcon = BaseSprite::create("yinbi_tubiao.png");
		else goldIcon = BaseSprite::create();
		mBg->addChild(goldIcon, 0, Tag_Img_GoldIcon);
		goldIcon->setPosition(ccp(260, 851-806));
		temp.initWithFormat(" %u", data->item_need.count);
		CCLabelTTF *goldNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, LABEL_SIZE);
		mBg->addChild(goldNumLabel, 0, Tag_Label_GoldNum);
		goldNumLabel->setPosition(ccp(goldIcon->getPositionX()+goldIcon->getContentSize().width/2+goldNumLabel->getContentSize().width/2-10
			, goldIcon->getPositionY()));
	}
	else 
		((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Refresh))->setPositionX(mWinSize.width/2);

	((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Refresh))->setVisible(true);

	mTableView->reloadData();
}

void MainLayerJingJi::rsp_QueryMarketList(CCObject *msg_f)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_queryArenaMarketRsp_F);

	Flat_QueryArenaMarketRsp *data = (Flat_QueryArenaMarketRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	mMarketList = data->item_list;
	CCString temp;
	//162 147 139
	temp.initWithFormat("%s", LFStrings::getValue("jingji_howToGetFameNum").c_str());
	CCLabelTTF *label = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(label, 0, Tag_Label_GoldNum);
	label->setColor(ccc3(162, 147, 139));
	label->setPosition(ccp(mBg->getContentSize().width/2, 851-186));

	mTableView->reloadData();
}

void MainLayerJingJi::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	CCMenuItemImage *itemFree = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Challenge);
	CCMenuItemImage *itemRandom = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Random);
	CCMenuItemImage *itemBuy = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Buy);
	CCMenuItemImage *itemExchange = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Exchange);
	CCMenuItemImage *itemRefresh = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Refresh);
	CCMenuItemImage *itemBack = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Back);
	BaseSprite *board1 = (BaseSprite *)mBg->getChildByTag(Tag_Board_ResidueNum);
	BaseSprite *board2 = (BaseSprite *)mBg->getChildByTag(Tag_Board_FameNum);
	CCString temp;
	ArenaTeamBasicInfo info;
	MyselfManager::getManager()->getArenaTeamBasicInfo(info);
	{
		itemFree->initWithNormalImage(
			"jingjichang_anniu_ziyoumoshi.png",
			"jingjichang_anniu_ziyoumoshi_select.png",
			NULL, this, menu_selector(MainLayerJingJi::menuItemClicked_MainMenu));
		itemRandom->initWithNormalImage(
			"jingjichang_anniu_suijimoshi.png",
			"jingjichang_anniu_suijimoshi_select.png",
			NULL, this, menu_selector(MainLayerJingJi::menuItemClicked_MainMenu));

		itemRefresh->setVisible(false);
		mBg->removeChildByTag(Tag_Img_GoldIcon);
		mBg->removeChildByTag(Tag_Label_GoldNum);
		mBg->removeChildByTag(Tag_Board_Random);
		mBg->removeChildByTag(Tag_Img_Random);
		mBg->removeChildByTag(Tag_Menu_BegainRandom);
		if (itemBack) itemBack->setVisible(false);
		
		itemBuy->setVisible(true);
		itemExchange->setVisible(true);
		board1->setVisible(true);
		board1->setPosition(ccp(121, 851-171));
		board2->setVisible(true);
		board2->initWithFile("jingjichang_biaoti_rongyudian.png");
		board2->setPosition(ccp(406, 851-171));

		if (mTableView != NULL)
		{
			mTableView->removeFromParent();
			mTableView = NULL;
		}

		temp.initWithFormat("%u", info.jifen);
		((CCLabelTTF*)board2->getChildByTag(1))->setString(temp.getCString());
		((CCLabelTTF*)board2->getChildByTag(1))->setPosition(ccp(44,18));
	}
	//
	if (tag == Tag_MenuItem_Random)
	{
		mCurFlag = model_random;
		itemRandom->initWithNormalImage(
			"jingjichang_anniu_suijimoshi_select.png",
			"jingjichang_anniu_suijimoshi.png",
			NULL, this, menu_selector(MainLayerJingJi::menuItemClicked_MainMenu));

		temp.initWithFormat("%u", info.free_fight_remainder_time);
		((CCLabelTTF*)board1->getChildByTag(1))->setString(temp.getCString());
		itemBuy->setVisible(false);

		BaseSprite *board = BaseSprite::create("jingjichang_suiji_bg.jpg");
		mBg->addChild(board, 1, Tag_Board_Random);
		board->setPosition(ccp(mBg->getContentSize().width/2, 851-494));
		BaseSprite *img = BaseSprite::create("jingjichang_suiji_biaoti.png");
		mBg->addChild(img, 2, Tag_Img_Random);
		img->setPosition(ccp(mBg->getContentSize().width/2, 851-443));
		CCMenu *menu = CCMenu::create();
		mBg->addChild(menu, 2, Tag_Menu_BegainRandom);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *item = CCMenuItemImage::create(
			"jingjichang_suiji_anniu_suijipipei.png",
			"jingjichang_suiji_anniu_suijipipei_select.png",
			this, menu_selector(MainLayerJingJi::menuItemClicked_BeginRandomChallenge));
		menu->addChild(item);
		item->setPosition(ccp(mBg->getContentSize().width/2, 851-626));
	}
	else if (tag == Tag_MenuItem_Challenge)
	{
		mCurFlag = model_Challenge;
		itemFree->initWithNormalImage(
			"jingjichang_anniu_ziyoumoshi_select.png",
			"jingjichang_anniu_ziyoumoshi.png",
			NULL, this, menu_selector(MainLayerJingJi::menuItemClicked_MainMenu));

		temp.initWithFormat("%u", info.challenge_remainder_time);
		((CCLabelTTF*)board1->getChildByTag(1))->setString(temp.getCString());

//		itemRefresh->setVisible(true);//延后

		mTableView = CCTableView::create(this, CCSizeMake(616, 551));
		mBg->addChild(mTableView);
		mTableView->setDirection(kCCScrollViewDirectionVertical);
		mTableView->setPosition(ccp(12, 851-770));
		mTableView->setDelegate(this);
		mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);

		addObserver(callfuncO_selector(MainLayerJingJi::rsp_QueryChallengeList), MSG_queryArenaChlgListRsp_F);
		mCmdHlp->cmdQueryArenaChlgList();

	}
	else if (tag == Tag_MenuItem_Exchange)
	{
		board2->initWithFile("jingjichang_biaoti_rongyudian2.png");
		board2->setPosition(ccp(320, 851-146));
		temp.initWithFormat("%u", info.jifen);
		((CCLabelTTF*)board2->getChildByTag(1))->setString(temp.getCString());
		((CCLabelTTF*)board2->getChildByTag(1))->setPosition(ccp(127,33));
		if (itemBack == NULL)
		{
			itemBack = CCMenuItemImage::create(
				"shuxing_renwu_fanhui.png",
				"shuxing_renwu_fanhui_select.png",
				this, menu_selector(MainLayerJingJi::menuItemClicked_BackRandomLayer));
			mMainMenu->addChild(itemBack, 1, Tag_MenuItem_Back);
			itemBack->setPosition(ccp(320, 851-804));
		}
		itemBack->setVisible(true);

		//
		mCurFlag = model_store;
		itemBuy->setVisible(false);
		itemExchange->setVisible(false);
		board1->setVisible(false);
//		board2->setPositionX(mBg->getContentSize().width/2);

		mTableView = CCTableView::create(this, CCSizeMake(616, 551));
		mBg->addChild(mTableView);
		mTableView->setDirection(kCCScrollViewDirectionVertical);
		mTableView->setPosition(ccp(12, 851-770));
		mTableView->setDelegate(this);
		mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);

		addObserver(callfuncO_selector(MainLayerJingJi::rsp_QueryMarketList), MSG_queryArenaMarketRsp_F);
		mCmdHlp->cmdQueryArenaMarket();
	}
}

void MainLayerJingJi::menuItemClicked_RefreshList(CCObject *pSender)
{
// 	unsigned int goldNum = WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount();
// 	if (goldNum < mChlgRefreshNeedGoldNum)
// 	{
// 		MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue("suoxujinbibuzu"));
// 		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
// 		return;
// 	}

	addObserver(callfuncO_selector(MainLayerJingJi::rsp_RefreshChallengeList), MSG_refreshArenaChlgList_F);
	mCmdHlp->cmdRefreshArenaChlgList();
}

void MainLayerJingJi::menuItemClicked_BuyResedueNum(CCObject *pSender)
{
	//弹框
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	//确认 取消按键  
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	{
		CCMenuItemImage* menuItemMax = CCMenuItemImage::create(
			"jingjichang_duihuan_anniu_zuidagoumai.png",
			"jingjichang_duihuan_anniu_zuidagoumai_select.png",
			this,SEL_MenuHandler(&MainLayerJingJi::menuItemClicked_BuyNumMakeSure));
		menu->addChild(menuItemMax, 1, 10);
		menuItemMax->setPosition(ccp(dialog->getContentSize().width/2, 70));
		CCMenuItemImage* menuItemOne = CCMenuItemImage::create(
			"jingjichang_duihuan_anniu_goumaiyici.png",
			"jingjichang_duihuan_anniu_goumaiyici_select.png",
			this,SEL_MenuHandler(&MainLayerJingJi::menuItemClicked_BuyNumMakeSure));
		menu->addChild(menuItemOne, 1, 20);
		menuItemOne->setPosition(ccp(menuItemMax->getPositionX()-menuItemMax->getContentSize().width-40, 70));
		CCMenuItemImage* menuItemCancel = CCMenuItemImage::create(
			"shangpin_goumai_quxiao.png",
			"shangpin_goumai_quxiao_select.png",
			this,SEL_MenuHandler(&MainLayerJingJi::menuItemClicked_BuyNumMakeSure));
		menu->addChild(menuItemCancel, 1, 30);
		menuItemCancel->setPosition(ccp(menuItemMax->getPositionX()+menuItemMax->getContentSize().width+40, 70));
	}
	//content
	{
		char buf[20];
		sprintf(buf, "%u", CS::arenaBuyOneChlgTimeNeedGold());
		string temp = LFStrings::getValue("shiFouHuaFei") + buf + LFStrings::getValue("jinbi") 
			+ LFStrings::getValue("goumai") + LFStrings::getValue("yici") + "?";
		//
		ArenaTeamBasicInfo info;
		MyselfManager::getManager()->getArenaTeamBasicInfo(info);
		unsigned int goldNum = WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount();
		int num = goldNum / CS::arenaBuyOneChlgTimeNeedGold();
		if (num >= info.buy_challenge_remainder_time) num = info.buy_challenge_remainder_time;
		sprintf(buf, "%d", num *  CS::arenaBuyOneChlgTimeNeedGold());
		temp += LFStrings::getValue("xianZaiNengHuaFei") + buf + LFStrings::getValue("jinbi");
		sprintf(buf, "%d", num);
		temp += LFStrings::getValue("goumai") + buf + LFStrings::getValue("ci") + ".";
		CCLabelTTF *label1 = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, 28, CCSize(430, 0), kCCTextAlignmentLeft);
		dialog->addChild(label1);
		label1->setAnchorPoint(ccp(0,1));
		label1->setPosition(ccp(60, 290));
		label1->setColor(ccc3(195, 145, 96));
		//
		sprintf(buf, "%u", info.buy_challenge_remainder_time);
		temp = LFStrings::getValue("shengyuGouMaiCishu");
		temp += buf;
		CCLabelTTF *label2 = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, 28, CCSize(430, 0), kCCTextAlignmentLeft);
		label2->setAnchorPoint(ccp(0, 1));
		dialog->addChild(label2);
		label2->setPosition(ccp(label1->getPositionX(), label1->getPositionY()-label1->getContentSize().height*2));
		label2->setColor(ccc3(195, 145, 96));
	}
	//
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setTag(Tag_Window_BuyResedueNum);
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_Window_BuyResedueNum);
		lfAlert->release();
	}
}

//10 max   20 one   30 cancel
void MainLayerJingJi::menuItemClicked_BuyNumMakeSure(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	if (tag == 30)
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_BuyResedueNum);
		return;
	}

	if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() < CS::arenaBuyOneChlgTimeNeedGold())
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_BuyResedueNum);
		MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue("suoxujinbibuzu"));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	bool isMaxNum = false;
	if (tag == 10) isMaxNum = true;
	else if (tag == 20) isMaxNum = false;
	addObserver(callfuncO_selector(MainLayerJingJi::rsp_BuyChallengeTime), MSG_arenaBuyChlgTimeRsp);
	mCmdHlp->cmdArenaBuyChallengeTime(isMaxNum);
}

void MainLayerJingJi::rsp_BuyChallengeTime(CCObject *msg)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_BuyResedueNum);
	removeObserver(MSG_arenaBuyChlgTimeRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_ArenaBuyChallengeTimeRsp *data  = (SPCmd_ArenaBuyChallengeTimeRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	char buf[20];
	sprintf(buf, "%u", data->challenge_remainder_time);
	BaseSprite *board1 = (BaseSprite *)mBg->getChildByTag(Tag_Board_ResidueNum);
	((CCLabelTTF*)board1->getChildByTag(1))->setString(buf);

	//刷新竞技剩余次数判断是否显示效果
	MainLayerBase::getCurrentMainBase()->getFooter()->checkIsShowLiziAni(NULL);
}

void MainLayerJingJi::menuItemClicked_BeginRandomChallenge(CCObject *pSender)
{
	ArenaTeamBasicInfo info;
	MyselfManager::getManager()->getArenaTeamBasicInfo(info);
	if (info.free_fight_remainder_time <= 0)
	{
		MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue("suijicishuYiYongWan"));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	JingJiChangLayer *layer = JingJiChangLayer::create(0);
	mFooterLayer->setContaintLayer(layer);
}

CCSize MainLayerJingJi::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	if (mCurFlag == model_Challenge)
		return CCSize(627, 154);
	else if (mCurFlag == model_store)
		return CCSize(622, 148);

	return CCSize(0,0);
}

CCTableViewCell* MainLayerJingJi::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();

	if (cell == NULL)
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}
	else
	{
		cell->removeAllChildren();
	}

	if (table != mTableView) return cell;

	if (mCurFlag == model_Challenge)
	{
		BaseSprite *bg = BaseSprite::create("jingjichang_wanjiazhuangtailan.png");
		cell->addChild(bg, 0, Tag_TableCellBg);
		bg->setPosition(ccp(tableCellSizeForIndex(mTableView, idx).width/2, tableCellSizeForIndex(mTableView, idx).height/2));
		//
		ArenaChlgerHeroInfo info = mChaHeroList.at(idx);

		//name
		CCLabelTTF *nameLabel = CCLabelTTF::create(info.hero_info.name, fontStr_kaiti, LABEL_SIZE);
		bg->addChild(nameLabel);
		nameLabel->setPosition(ccp(213, 154-39));
		//award
		CCLabelTTF *awardTishiLabel = CCLabelTTF::create(LFStrings::getValue("shengLiJiangLi").c_str(), fontStr_kaiti, LABEL_SIZE);
		bg->addChild(awardTishiLabel);
		awardTishiLabel->setColor(ccBLACK);
		awardTishiLabel->setPosition(ccp(174, 154-80));
		//
		CCString temp;
		temp.initWithFormat("%u%s", info.jifen, LFStrings::getValue("dian").c_str());
		CCLabelTTF *fameNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE+2.5);
		bg->addChild(fameNumLabel);
		if (info.jifen == 20)
		{
			fameNumLabel->setColor(ccRED);
		}
		else if (info.jifen == 16)
		{
			fameNumLabel->setColor(ccBLUE);
		}
		else
		{
			fameNumLabel->setColor(ccGREEN);
		}
		
		fameNumLabel->setAnchorPoint(ccp(0, 0.5f));
		fameNumLabel->setPosition(ccp(135, 154-110));
		CCLabelTTF *rongYuLabel = CCLabelTTF::create(LFStrings::getValue("rongYuZhi").c_str(), fontStr_kaiti, LABEL_SIZE);
		bg->addChild(rongYuLabel);
		rongYuLabel->setAnchorPoint(ccp(0, 0.5));
		rongYuLabel->setPosition(ccp(fameNumLabel->getPositionX()+fameNumLabel->getContentSize().width, fameNumLabel->getPositionY()));
		//level
		temp.initWithFormat("%u%s", info.hero_info.level, LFStrings::getValue("ji").c_str());
		CCLabelTTF *lvLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE);
		bg->addChild(lvLabel);
		lvLabel->setPosition(ccp(330, 154-39));
		lvLabel->setColor(ccBLACK);
		//
		FXScrollMenu *menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		bg->addChild(menu, 0, Tag_TableCellMenu);
		menu->setPosition(ccp(0,0));

		//head img
		string file = ImageManager::getManager()->getImageData_ProfessionHead((Profession)info.hero_info.profession_id).getCurrentImageFile();
		CCMenuItemImage *imgItem = CCMenuItemImage::create(
			file.c_str(),file.c_str(), this, menu_selector(MainLayerJingJi::menuItemClicked_HeroImg));
		menu->addChild(imgItem, 0, info.hero_info.hero_id);
		imgItem->setPosition(ccp(70, 154-70));
		imgItem->setScale(0.9f);

		if (info.has_win)
		{
			BaseSprite *winImg = BaseSprite::create("jingjichang_wanjiazhuangtailan_ko.png");
			bg->addChild(winImg);
			winImg->setPosition(ccp(532, 154-80));
			//
			BaseSprite *heroImgBg = BaseSprite::create("renxing_renwu_touxiang.png");
			bg->addChild(heroImgBg, 2);
			heroImgBg->setPosition(imgItem->getPosition());
		}
		else
		{
			CCMenuItemImage *item = CCMenuItemImage::create(
				"jingjichang_wanjiazhuangtailan_tiaozhan.png",
				"jingjichang_wanjiazhuangtailan_tiaozhan_select.png",
				this, menu_selector(MainLayerJingJi::menuItemClicked_Challenge));
			menu->addChild(item, 0, info.hero_info.hero_id);
			item->setPosition(ccp(544, 154-81));
			item->setUserData((void*)&mChaHeroList.at(idx));
			//
			BaseSprite *heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
			bg->addChild(heroImgBg, 2);
			heroImgBg->setPosition(imgItem->getPosition());
		}

	}
	else if (mCurFlag == model_store)
	{
		BaseSprite *bg = BaseSprite::create("shangdian_shangpin_bg_select.png");
		cell->addChild(bg, 0, Tag_TableCellBg);
		bg->setPosition(ccp(tableCellSizeForIndex(mTableView, idx).width/2, tableCellSizeForIndex(mTableView, idx).height/2));
		//
		ArenaMarketItem info = mMarketList.at(idx);
		char str[32];
		//item图片
		BaseSprite *sprite = BaseSprite::create(ImageManager::getManager()->getItemFilename(info.item_type));
		sprite->setPosition(ccp(72, bg->getContentSize().height/2));
		bg->addChild(sprite);
		// 名字
		LF::lable(bg,CS::getItemName(info.item_type),ccp(142,112),LABEL_SIZE,fontStr_kaiti);
		//属性
		CCLabelTTF* lableShuXing = CCLabelTTF::create(CS::getItemShuxing(info.item_type).c_str(), fontStr_kaiti,LABEL_SIZE
			,CCSize(330, 0), kCCTextAlignmentLeft);
		bg->addChild(lableShuXing);
		lableShuXing->setAnchorPoint(ccp(0, 1));
		lableShuXing->setPosition(ccp(142, 99));
		lableShuXing->setColor(ccc3(127,127,127));
		//数量
		CCLabelTTF* lableNum = LF::lable(bg, (LFStrings::getValue("shuliang")+": ").c_str(),ccp(142,36),LABEL_SIZE,fontStr_kaiti);
		lableNum->setColor(ccc3(89,111,183));
		//数量数值
		sprintf(str, "%d",info.item_count);
		LF::lable(bg, str,ccp(190,36),LABEL_SIZE,fontStr);
		//荣誉点
		CCLabelTTF *labelPIc = CCLabelTTF::create(LFStrings::getValue("rongYuDian").c_str(), fontStr_kaiti, LABEL_SIZE);
		labelPIc->setColor(ccGREEN);
		bg->addChild(labelPIc);
		labelPIc->setAnchorPoint(ccp(1, 0.5));
		labelPIc->setPosition(ccp(594, 148-111));
		//价格数值
		sprintf(str, "%d",info.jifen_count);
		CCLabelTTF *labelPrice = CCLabelTTF::create(str, fontStr_kaiti, 20);
		bg->addChild(labelPrice);
		labelPrice->setAnchorPoint(ccp(1, 0.5f));
		labelPrice->setPosition(ccp(labelPIc->getPositionX()-labelPIc->getContentSize().width,148-111));
		//购买按钮
		{
			FXScrollMenu* menu = FXScrollMenu::create(
				mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
			bg->addChild(menu, 0, Tag_TableCellMenu);
			menu->setPosition(CCPointZero);

			CCMenuItemImage* menuItem = CCMenuItemImage::create(
				"shangdian_goumai.png",
				"shangdian_goumai_select.png",
				this,SEL_MenuHandler(&MainLayerJingJi::menuItemClicked_MarketBuy));
			menu->addChild(menuItem, 0, idx);
			menuItem->setPosition(ccp(534, 148-57));
		}
	}

	return cell;
}

unsigned int MainLayerJingJi::numberOfCellsInTableView(CCTableView *table)
{
	unsigned int num = 0;
	if (mCurFlag == model_Challenge)
		num = mChaHeroList.size();
	else if (mCurFlag == model_store)
		num = mMarketList.size();
	
	return num;
}

void MainLayerJingJi::menuItemClicked_HeroImg(CCObject *pSender)
{
	int heroId = ((CCMenuItem*)pSender)->getTag();

	this->addObserver(callfuncO_selector(MainLayerJingJi::rsp_QueryAnotherTeamInfo), MSG_queryAnotherTeamInfoRsp_F);
	mCmdHlp->cmdQueryAnotherTeamInfo(heroId);
}

void MainLayerJingJi::rsp_QueryAnotherTeamInfo(CCObject *msg_f)
{
	removeObserver(MSG_queryAnotherTeamInfoRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_QueryAnotherTeamInfoRsp* data = (Flat_QueryAnotherTeamInfoRsp*) ((Flat_MSG_Rsp*)msg_f)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		this->addChild(layer, 1025);
		return;
	}

	vector<HeroDetail> heroDetailVec;
	for(int i=0; i!=4; ++i)
	{
		if(data->hero_arr[i].basic_info.hero_id != 0) 
			heroDetailVec.push_back(data->hero_arr[i]);
		else continue;
	}
	vector<EquipData> heroEquipVec[4]; //按照 heroDetail排序
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
	mQueryYuanShen = new CCArray;       //----remember delete
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
		addObserver(callfuncO_selector(MainLayerJingJi::rsp_queryAnotherTeamYuanshen), MSG_queryYuanshenOfHeroRsp_F);
		for (int i=0; i!=heroDetailVec.size(); ++i)
		{
			if (heroDetailVec.at(i).basic_info.level >= 30)
				mCmdHlp->cmdQueryYuanshenOfHero(heroDetailVec.at(i).basic_info.hero_id);
		}
	}
	else
	{
		rsp_queryAnotherTeamYuanshen(NULL);
	}
}

void MainLayerJingJi::rsp_queryAnotherTeamYuanshen(CCObject *msg_f)
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

void MainLayerJingJi::menuItemClicked_Challenge(CCObject *pSender)
{
	ArenaTeamBasicInfo info;
	MyselfManager::getManager()->getArenaTeamBasicInfo(info);
	if (info.challenge_remainder_time <= 0)
	{
		MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue("tiaozhancishuYiYongWan"));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	CCMenuItemImage* pItem = (CCMenuItemImage*)pSender;
	int heroId = pItem->getTag();
	g_iGetRongYuDianShu = ((ArenaChlgerHeroInfo*)pItem->getUserData())->jifen;

	addObserver(callfuncO_selector(MainLayerJingJi::rsp_Challenge), MSG_arenaChallengeRsp_F);
	mCmdHlp->cmdArenaChallenge(heroId);
}

void MainLayerJingJi::rsp_Challenge(CCObject *msg_f)
{
	removeObserver(MSG_arenaChallengeRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_ArenaChallengeRsp *data = (Flat_ArenaChallengeRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	
	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(data->fightScript);
	dir_vec.push_back(dir);

	Bundle bundle;
	bundle.putString("JumpToWhenEnd","JingJi_Free");
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec
		, MapsManager::getManager()->getJingJiChangMapImage(), bundle);
}

void MainLayerJingJi::rsp_RefreshChallengeList(CCObject *msg_f)
{
	removeObserver(MSG_refreshArenaChlgList_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_RefreshArenaChallengeListRsp* data = (Flat_RefreshArenaChallengeListRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
// 	UnblockWarnDialog *layer = new UnblockWarnDialog;
// 	layer->init(LFStrings::getValue("shuaxinChengGong"), false);
// 	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
// 	layer->release();
	
	mChaHeroList = data->hero_list;
	mChlgRefreshNeedGoldNum = data->item_need.count;
	CCString temp;
 	BaseSprite *goldIcon = (BaseSprite*)mBg->getChildByTag(Tag_Img_GoldIcon);
	if (goldIcon == NULL)
	{
		goldIcon = BaseSprite::create();
		mBg->addChild(goldIcon, 1, Tag_Img_GoldIcon);
		goldIcon->setPosition(ccp(260, 851-806));
	}
	if (data->item_need.type != 0)
	{
		((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Refresh))->setPositionX(370);

		if (data->item_need.type == ItemsType_Gold)
			goldIcon->initWithFile("jinbi_tubiao.png");
		else if (data->item_need.type == ItemsType_Silver)
			goldIcon->initWithFile("yinbi_tubiao.png");
	}
	CCLabelTTF *label = dynamic_cast<CCLabelTTF*>(mBg->getChildByTag(Tag_Label_GoldNum));
	temp.initWithFormat(" %u", data->item_need.count);
	if (label == NULL)
	{
		label = CCLabelTTF::create("", fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(label, 1, Tag_Label_GoldNum);
		label->setColor(ccc3(162, 147, 139));
		label->setPosition(ccp(goldIcon->getPositionX()+goldIcon->getContentSize().width/2+label->getContentSize().width/2-10
			, goldIcon->getPositionY()));
	}
	if (data->item_need.type != 0)
	{
		label->setString(temp.getCString());
		label->setPosition(ccp(goldIcon->getPositionX()+goldIcon->getContentSize().width/2+label->getContentSize().width/2-10
			, goldIcon->getPositionY()));
	}
// 	CCLabelTTF *goldNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, LABEL_SIZE);
// 	mBg->addChild(goldNumLabel, 0, Tag_Label_GoldNum);
// 	goldNumLabel->setPosition(ccp(goldIcon->getPositionX()+goldIcon->getContentSize().width/2+goldNumLabel->getContentSize().width/2
// 		, goldIcon->getPositionY()));

	mTableView->reloadData();
}

void MainLayerJingJi::menuItemClicked_MarketBuy(CCObject *pSender)
{
	//获取被点击的购买按钮所对应的index
	mIdx_BuyItem = ((CCNode*)pSender)->getTag();

	//弹框背景
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->setPosition(ccp(320, mTableView->getViewSize().height/2 + 50));
	dialog->setTag(Tag_Window_BuyItems);
	//提示
	{
// 		char buf[20];
// 		sprintf(buf, "%u",  mMarketList.at(mIdx_BuyItem).jifen_count );
// 		string temp = LFStrings::getValue("shiFouHuaFei") + buf + LFStrings::getValue("rongYuDian")
// 			+ LFStrings::getValue("goumai") + LFStrings::getValue("yi") + LFStrings::getValue("ge") 
// 			+ CS::getItemName(mMarketList.at(mIdx_BuyItem).item_type);
// 
// 		CCLabelTTF* lable = CCLabelTTF::create(temp.c_str(), fontStr_kaiti,30,CCSize(400, 0), kCCTextAlignmentLeft);
// 		dialog->addChild(lable);
// 		lable->setPosition(ccp(dialog->getContentSize().width/2, 228));
// 		lable->setColor(ccc3(201, 156, 18));
		//购买个数
		CCLabelTTF *gmgsLabel = CCLabelTTF::create((LFStrings::getValue("goumai")+LFStrings::getValue("CiShu")).c_str(), 
			fontStr_kaiti, LABEL_SIZE);
		dialog->addChild(gmgsLabel);
		gmgsLabel->setPosition(ccp(152, 275));
		gmgsLabel->setColor(fonColor_PuTong);
		//花费荣誉
		CCLabelTTF *hfryLabel = CCLabelTTF::create(LFStrings::getValue("HuaFeiRongYu").c_str(), fontStr_kaiti, LABEL_SIZE);
		dialog->addChild(hfryLabel);
		hfryLabel->setPosition(ccp(gmgsLabel->getPositionX(), gmgsLabel->getPositionY()-74));
		hfryLabel->setColor(fonColor_PuTong);
		//
		char buf[20];
		mMatketBuyItem_Num = 1;
		sprintf(buf, "%u",  mMarketList.at(mIdx_BuyItem).jifen_count * mMatketBuyItem_Num);
		mMarketBuyNeedNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		dialog->addChild(mMarketBuyNeedNum, 1);
		mMarketBuyNeedNum->setColor(ccWHITE);
		mMarketBuyNeedNum->setPosition(ccp(340, 275-74));
	}
	//按键
	CCMenu* menu = CCMenu::create();
	menu->setPosition(ccp(0,0));
	{
		CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			"shangpin_goumai_queren_suoding.png",
			this,SEL_MenuHandler(&MainLayerJingJi::menuItemClicked_MarketOkCancel));
		menu->addChild(menuItemMakeSure, 0, 10);
		menuItemMakeSure->setPosition(ccp(150,70));
		CCMenuItemImage* menuItemCancel = CCMenuItemImage::create(
			"shangpin_goumai_quxiao.png",
			"shangpin_goumai_quxiao_select.png",
			this,SEL_MenuHandler(&MainLayerJingJi::menuItemClicked_MarketOkCancel));
		menu->addChild(menuItemCancel, 0, 20);
		menuItemCancel->setPosition(ccp(368,70));
		// add sub
		// 数量
		BaseSprite *dizuo = BaseSprite::create("gongjiang_hecheng_jindutiao_bg.png");
		dialog->addChild(dizuo);
		dizuo->setPosition(ccp(340, 275));
		//item
		_addItem = CCMenuItemImage::create(
			"gongjiang_duanzao_zengjia_baohu_jia.png"
			, "gongjiang_duanzao_zengjia_baohu_jia_select.png"
			, "gongjiang_duanzao_zengjia_baohu_jia_select.png"
			, this, menu_selector(MainLayerJingJi::menuItemClicked_MarketAddSub));
		menu->addChild(_addItem, 1, 2);
		_addItem->setPosition(ccp(dizuo->getPositionX()+dizuo->getContentSize().width/2+_addItem->getContentSize().width/2, dizuo->getPositionY()));
		_subItem = CCMenuItemImage::create(
			"gongjiang_duanzao_zengjia_baohu_jian.png"
			, "gongjiang_duanzao_zengjia_baohu_jian_select.png"
			, "gongjiang_duanzao_zengjia_baohu_jian_select.png"
			, this, menu_selector(MainLayerJingJi::menuItemClicked_MarketAddSub));
		menu->addChild(_subItem, 1, 1);
		_subItem->setPosition(ccp(dizuo->getPositionX()-dizuo->getContentSize().width/2-_subItem->getContentSize().width/2, dizuo->getPositionY()));
		_subItem->setEnabled(false);
		//
		mMatketBuyItem_Num = 1;
		mMarketNumLabel = CCLabelTTF::create("1", fontStr_BookAntiqua, LABEL_SIZE);
		dialog->addChild(mMarketNumLabel, 1, Tag_MarketItemBuyNum);
		mMarketNumLabel->setPosition(dizuo->getPosition());

		ArenaTeamBasicInfo info;
		MyselfManager::getManager()->getArenaTeamBasicInfo(info);
		if (mMarketList.at(mIdx_BuyItem).jifen_count > info.jifen)
		{
			mMarketBuyNeedNum->setColor(fonColor_JingGao);
			_addItem->setEnabled(false);
		}
	}
	//弹框
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128,Tag_Window_BuyItems);
		lfAlert->release();
	}
}

void MainLayerJingJi::menuItemClicked_MarketAddSub(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	char buf[20];

	ArenaTeamBasicInfo info;
	MyselfManager::getManager()->getArenaTeamBasicInfo(info);
	int allNum = info.jifen;
	int needNum = allNum + 1; // ！

	if (tag == 2) // add
	{
		++mMatketBuyItem_Num;
		needNum = mMatketBuyItem_Num * mMarketList.at(mIdx_BuyItem).jifen_count;

		_subItem->setEnabled(true);
		//if (needNum + mMarketList.at(mIdx_BuyItem).jifen_count > allNum)
		if (needNum > allNum)
			_addItem->setEnabled(false);
	}
	else if (tag == 1) // sub
	{
		--mMatketBuyItem_Num;
		needNum = mMatketBuyItem_Num * mMarketList.at(mIdx_BuyItem).jifen_count;

		_addItem->setEnabled(true);
		if (mMatketBuyItem_Num <= 1) 
			_subItem->setEnabled(false);
	}

	sprintf(buf, "%d", mMatketBuyItem_Num);
	mMarketNumLabel->setString(buf);
	sprintf(buf, "%d", needNum);
	mMarketBuyNeedNum->setString(buf);

	if (needNum > allNum)
	{
		mMarketBuyNeedNum->setColor(fonColor_JingGao);
	}
	else
	{
		mMarketBuyNeedNum->setColor(ccWHITE);
	}
}

void MainLayerJingJi::menuItemClicked_MarketOkCancel(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	if (tag == 10)
	{
		ArenaMarketItem item = mMarketList.at(mIdx_BuyItem);
		ArenaTeamBasicInfo info;
		MyselfManager::getManager()->getArenaTeamBasicInfo(info);
		if (info.jifen < item.jifen_count * mMatketBuyItem_Num)
		{
			CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_BuyItems);
			MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue("jingji_suoXuRongYuDianBuZu"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		addObserver(callfuncO_selector(MainLayerJingJi::rsp_MarketBuyEnd), MSG_arenaMarketBuyRsp);
		mCmdHlp->cmdArenaMarketBuy(item.item_type, item.item_count * mMatketBuyItem_Num);
	}
	else if (tag == 20)
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_BuyItems);
	}
}

void MainLayerJingJi::rsp_MarketBuyEnd(CCObject *msg)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_BuyItems);
	removeObserver(MSG_arenaMarketBuyRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_ArenaMarketBuyRsp *data = (SPCmd_ArenaMarketBuyRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	//刷新 荣誉点
	char buf[20];
	sprintf(buf, "%u", data->jifen_balance);
	BaseSprite *board2 = (BaseSprite *)mBg->getChildByTag(Tag_Board_FameNum);
	((CCLabelTTF*)board2->getChildByTag(1))->setString(buf);

	//弹框背景
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->setPosition(ccp(320, mTableView->getViewSize().height/2 + 50));
	dialog->setTag(Tag_Window_BuyEnd);
	//按键
	CCMenu* menu;
	{
		CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&MainLayerJingJi::menuItemClicked_MarketOkEnd));
		menu = CCMenu::create(menuItemMakeSure, NULL);
		menu->setPosition(CCPointZero);
		menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/2,70));
	}
	//提示
	{
		CCLabelTTF* lable = LF::lable(dialog, LFStrings::getValue("goumaichenggong"), ccp(0, 0), 30, fontStr_kaiti);
		lable->setPosition(ccp(dialog->getContentSize().width/2, 278));
		lable->setColor(ccRED);
	}
	//pic
	{
		BaseSprite* pic = BaseSprite::create(ImageManager::getManager()->getItemFilename(mMarketList.at(mIdx_BuyItem).item_type));
		dialog->addChild(pic);
		pic->setPosition(ccp(234, 194));

		char str[64];
		sprintf(str, "x%d", mMarketList.at(mIdx_BuyItem).item_count*mMatketBuyItem_Num);
		CCLabelTTF* lable = LF::lable(dialog, str, ccp(276, 194), 25, fontStr);
	}
	//弹框
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128,Tag_Window_BuyEnd);
		lfAlert->release();
	}
}

void MainLayerJingJi::menuItemClicked_MarketOkEnd(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_BuyEnd);
}

void MainLayerJingJi::menuItemClicked_BackRandomLayer(CCObject *pSender)
{
	menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_MenuItem_Challenge));
}

void MainLayerJingJi::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
	mTableView->setTouchEnabled(false);
	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(Tag_TableCellBg);
			if (bg)
			{
				CCMenu* menu = (CCMenu*)bg->getChildByTag(Tag_TableCellMenu);
				if (menu)
				{
					menu->setEnabled(false);
				}
			}
		}
	}
}

void MainLayerJingJi::disableAllTouchEnd()
{
	mMainMenu->setEnabled(true);
	mTableView->setTouchEnabled(true);
	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(Tag_TableCellBg);
			if (bg)
			{
				CCMenu* menu = (CCMenu*)bg->getChildByTag(Tag_TableCellMenu);
				if (menu)
				{
					menu->setEnabled(true);
				}
			}
		}
	}
}

void MainLayerJingJi::onEnter()
{
	BaseLayer::onEnter();

	//教学
	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstJingJiChangPlay))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstJingJiChangPlay);

		//2 点击兑换按钮
		CCMenuItem* item = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_Exchange);
		GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;
		guideLayer2->init(CCSize(item->getContentSize().width*item->getScaleX(), item->getContentSize().height*item->getScaleY())
			, item->getParent()->convertToWorldSpace(item->getPosition())
			, LFStrings::getValue("jingJiChang_ChaKanDuiHuan"), NULL, false);
		//1荣誉值说明
		CCSprite *board = (CCSprite*)mBg->getChildByTag(Tag_Board_FameNum);
		GuideLayerBase *guideLayer1 = new GuideLayer_SimpleLayer;
		guideLayer1->init(
			board->getContentSize(),
			board->getParent()->convertToWorldSpace(board->getPosition()), 
			LFStrings::getValue("jingJiChang_RongYuDianTiShi"), guideLayer2, true);

		mFooterLayer->getFooter()->addChild(guideLayer1, 1024);
	}
}