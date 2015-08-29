
#include "XueSeJiaoTangFightPrepareLayer.h"
#include "MapsManager.h"
#include "LFStrings.h"
#include "font.h"
#include "XueZhanManager.h"
#include "LFAlert.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "AwardDisplay.h"
#include "MessageDisplay.h"
#include "GameDirector.h"
#include "CmdHelper.h"
#include "FightDirector.h"
#include "ClientFightLayer.h"
#include "UnblockManager.h"

enum 
{
	Tag_MenuItem_Close =10,
	Tag_ChoseAwardMenuItem_card_ = 20,
	Tag_CardOpened = 30,
	Tag_CardClosed,
	Tag_Img_AwardItem,
	Tag_Label_CardName,
	Tag_XueSeJiaoTang_baoxiang,
	Tag_Window_ChoseAward = 160,

	Tag_Window_JiQuNengLiang = 200,
	//以下6个不能改变顺序
	Tag_JiQuMenuItem_GongJi_1,
	Tag_JiQuMenuItem_GongJi_2,
	Tag_JiQuMenuItem_HuJia_1,
	Tag_JiQuMenuItem_HuJia_2,
	Tag_JiQuMenuItem_ShengMing_1,
	Tag_JiQuMenuItem_ShengMing_2,

};

XueSeJiaoTangFightPrepareLayer::XueSeJiaoTangFightPrepareLayer()
{
	CCLayerColor::initWithColor(ccc4(255,255,255,0));
	mWinSize = CCDirector::sharedDirector()->getWinSize();
	CCString temp;

	int curTask = XueZhanManager::getManager()->getPassedNum() + 1; //getPassedNum是闯过的关卡

	//1.back layer
	CCSprite *mBg= MapsManager::getManager()->getXueSeJiaoTangMapImage(curTask);	
	addChild(mBg);
	mBg->setAnchorPoint(ccp(0.5, 1));
	mBg->setPosition(ccp(mWinSize.width/2, mWinSize.height));

// 	//1.5 半透明背景
// 	CCSprite* btm = CCSprite::create("zhandou_jiugongge_touming.png");
// 	mBg->addChild(btm);
// 	btm->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
// 	btm->setOpacity(200);

	//九宫格
	BaseSprite *jggMap = BaseSprite::create("zhandou_jiugongge_ditu1.png");
	addChild(jggMap, 10);
	jggMap->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	//格子按钮
	CCMenu *menu = CCMenu::create();
	addChild(menu, 11);
	menu->setPosition(CCPointZero);
	CCMenuItemImage *circle = CCMenuItemImage::create(
		"zhandou_jiugongge_didian_weizhi.png",
		"zhandou_jiugongge_didian_yitance.png",
		this, menu_selector(XueSeJiaoTangFightPrepareLayer::menuItemClicked_fightCircle));
	menu->addChild(circle);
	circle->setPosition(jggMap->getPosition());
	circle->runAction(CCRepeatForever::create(CCSequence::create(
		CCFadeOut::create(1), CCFadeIn::create(1), NULL)));
	//fanhui
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"zhuangbei_xiangxi_fanhui.png",
		"zhuangbei_xiangxi_fanhui_select.png",
		this, menu_selector(XueSeJiaoTangFightPrepareLayer::menuItemClicked_FanHui));
	menu->addChild(fanhui, 1);
	fanhui->setPosition(ccp(mWinSize.width/2, 260));

	//宝箱按钮
	CCMenuItemImage *baoxiang = CCMenuItemImage::create(
		"xuesejiaotang_box.png",
		"xuesejiaotang_box_select.png",
		this, menu_selector(XueSeJiaoTangFightPrepareLayer::menuItemClicked_BaoXiang));
	menu->addChild(baoxiang, 0, Tag_XueSeJiaoTang_baoxiang);
	baoxiang->setScale(0.8f);
	baoxiang->setPosition(ccp(mWinSize.width-baoxiang->getContentSize().width/2, mWinSize.height-160));
	
	//宝箱显示字符
	CCLabelTTF* pTip = CCLabelTTF::create(
		CCString::createWithFormat(LFStrings::getValue("xuezhan_scene_baoxiang_string").c_str(), getLevelValue())->getCString(),fontStr_kaiti,  25);
	baoxiang->addChild(pTip);
	pTip->setColor(ccORANGE);
	pTip->setPosition(ccp(baoxiang->getContentSize().width/2, -10));

	//head
	BaseSprite *headBar = BaseSprite::create("xuesejiaotang_zhandou_jiqunengliang_guanqia_bg.png");
	mBg->addChild(headBar, 1);
	headBar->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-headBar->getContentSize().height/2));
	{
		//关卡名字
		CCLabelTTF *nameLabel = CCLabelTTF::create(XueZhanManager::getManager()->getCurrentBackgroundDescri().c_str(), fontStr_kaiti, 24);
		headBar->addChild(nameLabel);
		nameLabel->setColor(fonColor_PuTong);
		nameLabel->setPosition(ccp(headBar->getContentSize().width/2, 104-27));
		//关卡数
		temp.initWithFormat("%s%d%s", LFStrings::getValue("di").c_str(), curTask, LFStrings::getValue("guan").c_str());
		CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
		headBar->addChild(numLabel);
		numLabel->setPosition(ccp(headBar->getContentSize().width/2, 104-73));
	}

	//foot
	BaseSprite *footBar = BaseSprite::create("xuesejiaotang_zhandou_jiqunengliang_bg.png");
	mBg->addChild(footBar, 2);
	footBar->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-mWinSize.height+footBar->getContentSize().height/2));
	{
		//通关将获得%d点能量，战斗也有几率拾取到额外能量！
		temp.initWithFormat(LFStrings::getValue("xuesejiaotang_NengLiangHuoDeShuoMing").c_str(), 15);
		CCLabelTTF *title = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
		footBar->addChild(title);
		title->setPosition(ccp(footBar->getContentSize().width/2, 156));
		// 拥有能量
		BaseSprite *icon = BaseSprite::create("xuesejiaotang_nengliang.png");
		footBar->addChild(icon);
		icon->setScale(0.3f);
		icon->setPosition(ccp(478, 196-92));
		//
		temp.initWithFormat("%u", XueZhanManager::getManager()->getCurrentPowerNum());
		mAllNengLiangL = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
		mAllNengLiangL->setAnchorPoint(ccp(0,0.5));
		footBar->addChild(mAllNengLiangL);
		mAllNengLiangL->setPosition(ccp(498, 196-92));
		// 血量加成
		int bloodAddNum = XueZhanManager::getManager()->getCurrentHpAddPercent();
		temp.initWithFormat("+%d%%", bloodAddNum);
		mAddXueLiangL = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
		footBar->addChild(mAddXueLiangL);
		mAddXueLiangL->setAnchorPoint(ccp(0, 0.5));
		mAddXueLiangL->setPosition(ccp(84, 196-152));
		if (bloodAddNum > 0)
		{
			mAddXueLiangL->setColor(fonColor_ChengGong);
		}
		// 攻击加成
		int gongJiAddNum = XueZhanManager::getManager()->getCurrentGongJiAddPercent();
		temp.initWithFormat("+%d%%", gongJiAddNum);
		mAddGongJiL = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
		footBar->addChild(mAddGongJiL);
		mAddGongJiL->setAnchorPoint(ccp(0, 0.5));
		mAddGongJiL->setPosition(ccp(224, 196-152));
		if (gongJiAddNum > 0)
		{
			mAddGongJiL->setColor(fonColor_ChengGong);
		}
		// 防御加成
		int fangYuAddNum = XueZhanManager::getManager()->getCurrentHuJiaAddPercent();
		temp.initWithFormat("+%d%%", fangYuAddNum);
		mAddFangYuL = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
		footBar->addChild(mAddFangYuL);
		mAddFangYuL->setAnchorPoint(ccp(0, 0.5));
		mAddFangYuL->setPosition(ccp(366, 196-152));
		if (fangYuAddNum > 0)
		{
			mAddFangYuL->setColor(fonColor_ChengGong);
		}
		//menu
		CCMenu *menu = CCMenu::create();
		footBar->addChild(menu);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *item = CCMenuItemImage::create(
			"xuesejiaotang_anniu_jiqunengliang.png",
			"xuesejiaotang_anniu_jiqunengliang_select.png",
			this, menu_selector(XueSeJiaoTangFightPrepareLayer::menuItemClicked_XiQuNengLiang));
		menu->addChild(item);
		item->setPosition(ccp(544, 196-152));
	}


	scheduleOnce(schedule_selector(XueSeJiaoTangFightPrepareLayer::displayAward), 0);

}

int XueSeJiaoTangFightPrepareLayer::getLevelValue()
{
	int curTask = XueZhanManager::getManager()->getPassedNum() + 1; //getPassedNum是闯过的关卡
	int value = 20;
	if (curTask <= 40 && curTask >= 21)
	{
		value = 40;
	}
	else if (curTask <= 60 && curTask >= 41)
	{
		value = 60;
	}
	else if (curTask <= 80 && curTask >= 61)
	{
		value = 80;
	}
	else if (curTask <= 100 && curTask >= 81)
	{
		value = 100;
	}

	return value;
}

void XueSeJiaoTangFightPrepareLayer::menuItemClicked_BaoXiang(CCObject* pObj)
{
	UnblockWarnDialog *layer = UnblockWarnDialog::create(
		CCString::createWithFormat(LFStrings::getValue("xuezhan_scene_tip_string").c_str(), getLevelValue())->getCString(), false);
	layer->setLabelDisplaySize(CCSizeMake(360, 0));
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
}

void XueSeJiaoTangFightPrepareLayer::menuItemClicked_FanHui(CCObject *pSender)
{
	GameDirector::getDirector()->mainScene(mainlayer_HuoDong);
}

void XueSeJiaoTangFightPrepareLayer::displayAward(float dt)
{
	//弹领奖框  或者  抽奖框
	if (XueZhanManager::getManager()->canOpenBox())
	{
		displayChoseAwardBox();



	}
	else if (XueZhanManager::getManager()->canShowRewards()) //抽奖 必须在 弹框奖励之前
	{
		displayTaskAward();
	}
}

void XueSeJiaoTangFightPrepareLayer::displayChoseAwardBox()
{
	//弹框
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	dialog->setOpacity(0);
	BaseSprite* window = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->addChild(window, -1);
	window->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
	window->setScale(1.2f);
	//确认 取消按键 
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	CCMenuItemImage *closeItem = CCMenuItemImage::create(
		"shangpin_goumai_guanbi.png"
		, "shangpin_goumai_guanbi_select.png"
		, this, menu_selector(XueSeJiaoTangFightPrepareLayer::menuItemClicked_ChoseAward));
	menu->addChild(closeItem, 1, Tag_MenuItem_Close);
	closeItem->setPosition(ccp(dialog->getContentSize().width/2,40));
	closeItem->setVisible(false);
	//content
	{
		CCSize size = dialog->getContentSize();
		//card
		for (int i=0; i<3; ++i)
		{
			CCMenuItemImage *cardItem = CCMenuItemImage::create(
				"zahuopu_pai_zhengmian.png",
				"zahuopu_pai_zhengmian.png",
				this, menu_selector(XueSeJiaoTangFightPrepareLayer::menuItemClicked_ChoseAward));
			menu->addChild(cardItem, 0, Tag_ChoseAwardMenuItem_card_+i);
			cardItem->setPosition(ccp(89 + i*172, size.height-157));
			cardItem->setOpacity(0);
			//opened board
			BaseSprite *board = BaseSprite::create("zahuopu_pai_zhengmian.png");
			cardItem->addChild(board, 1, Tag_CardOpened);
			board->setPosition(ccp(cardItem->getContentSize().width/2, cardItem->getContentSize().height/2));
			//mask( closed board)
			BaseSprite *maskImg = BaseSprite::create("zahuopu_pai_zbeimian.png");
			cardItem->addChild(maskImg, board->getZOrder()+1, Tag_CardClosed);
			maskImg->setPosition(board->getPosition());
			maskImg->setVisible(false);
			//item img
			BaseSprite *itemImg = BaseSprite::create();
			board->addChild(itemImg, 0, Tag_Img_AwardItem);
			itemImg->setPosition(ccp(70, 205-87));
			// item name label
			CCLabelTTF *nameLabel = CCLabelTTF::create("", fontStr_kaiti, 20, CCSize(142,0), kCCTextAlignmentCenter);
			cardItem->addChild(nameLabel, 1, Tag_Label_CardName);
			nameLabel->setAnchorPoint(ccp(0, 1));
			nameLabel->setPosition(ccp(10, 6));
			{
				board->setVisible(false);
				maskImg->setVisible(true);
				nameLabel->setOpacity(0);
			}
		}
	}
	//
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setTag(Tag_Window_ChoseAward);
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_Window_ChoseAward);
		lfAlert->release();
	}
}

void XueSeJiaoTangFightPrepareLayer::displayTaskAward()
{
	CCString temp;//恭喜你   闯过   name   ，获得  以下 奖励:
	temp.initWithFormat("%s%s%s,%s%s%s:", 
		LFStrings::getValue("gongxini").c_str(), LFStrings::getValue("ChuangGuo").c_str(),
		XueZhanManager::getManager()->getCurrentBackgroundDescri().c_str(), LFStrings::getValue("huode").c_str(), 
		LFStrings::getValue("YiXia").c_str(), LFStrings::getValue("jiangli").c_str());

	vector<ItemInfo> items;
	ItemInfo one;
	one.type = ItemsType_Silver;
	one.count = XueZhanManager::getManager()->getGettedSilverNum();
	items.push_back(one);

	GiftBag* bag=(GiftBag *)XueZhanManager::getManager()->getGettedGiftBag();
	AwardDisplay *layer = AwardDisplay::create(temp, items,bag);

	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	XueZhanManager::getManager()->setRewardShowed();
}

void XueSeJiaoTangFightPrepareLayer::menuItemClicked_ChoseAward(CCObject *pSender)
{
	mChoseCardMenuItem = (CCMenuItem *)pSender;
	int tag = mChoseCardMenuItem->getTag();

	if (tag == Tag_MenuItem_Close) //关闭 10
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_ChoseAward);
		if (XueZhanManager::getManager()->canShowRewards()) //抽奖 必须在 弹框奖励之前
		{
			displayTaskAward();
		}
	}
	else if (tag >= Tag_ChoseAwardMenuItem_card_ )
	{
		CCMenu* menu = ((CCMenu*)mChoseCardMenuItem->getParent());
		for (int i=0; i<3; ++i)
		{
			CCMenuItem *item = (CCMenuItem*)menu->getChildByTag(Tag_ChoseAwardMenuItem_card_ + i);
			if (item) item->setEnabled(false);
		}

		CCNotificationCenter::sharedNotificationCenter()->addObserver(this, 
			callfuncO_selector(XueSeJiaoTangFightPrepareLayer::rsp_getChoseAward),
			MSG_getSelfChallengeBoxRsp, NULL);
		CmdHelper::getHelper()->cmdGetSelfChallengeBox();
	}
}

void XueSeJiaoTangFightPrepareLayer::rsp_getChoseAward(CCObject *msg)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_getSelfChallengeBoxRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_GetSelfChallengeBoxRsp * data = (SPCmd_GetSelfChallengeBoxRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	//close Item
	CCMenuItem *closeItem = (CCMenuItem*)mChoseCardMenuItem->getParent()->getChildByTag(Tag_MenuItem_Close);
	if (closeItem)
	{
		closeItem->setOpacity(0);
		closeItem->setVisible(true);
		closeItem->runAction(CCSequence::create(CCDelayTime::create(0.5f), CCFadeTo::create(0.5f,200), NULL));
	}

	//card
	int choseTag = mChoseCardMenuItem->getTag() - Tag_ChoseAwardMenuItem_card_;
	//获得的  放在choseTag位置
	ItemInfo items[3];
	int idx1,idx2;
	if (choseTag == 0) {idx1 = 1; idx2 = 2;}
	else if (choseTag == 1) {idx1= 0; idx2 = 2;}
	else if (choseTag == 2) {idx1 = 0; idx2 = 1;}
	{
		items[choseTag].type = data->item.type;
		items[choseTag].count = data->item.count;
		items[idx1].type = data->item_arr[0].type;
		items[idx1].count = data->item_arr[0].count;
		items[idx2].type = data->item_arr[1].type;
		items[idx2].count = data->item_arr[1].count;
	}

	CCString temp;
	for (int i=0; i<3; ++i)
	{
		CCMenuItem *cardItem = (CCMenuItem*)mChoseCardMenuItem->getParent()->getChildByTag(Tag_ChoseAwardMenuItem_card_+i);
		float delayTime = 0.75f;
		//item img
		BaseSprite *itemImg = (BaseSprite*)cardItem->getChildByTag(Tag_CardOpened)->getChildByTag(Tag_Img_AwardItem);
		itemImg->initWithFile(ImageManager::getManager()->getItemFilename(items[i].type));
		//名字 和 数量出现
		CCLabelTTF *nameLabel = (CCLabelTTF*)cardItem->getChildByTag(Tag_Label_CardName);
		string name = CS::getItemName(items[i].type);
		temp.initWithFormat("%s x%u", name.c_str(), items[i].count);
		nameLabel->setString(temp.getCString());
		nameLabel->setPositionX((cardItem->getContentSize().width-nameLabel->getContentSize().width)/2);
		if (choseTag == i)
		{
			nameLabel->setColor(fonColor_ChengGong);
			delayTime = 0;
		}
		nameLabel->runAction(CCSequence::create(CCDelayTime::create(delayTime), CCFadeIn::create(0.5f), NULL));
		//正面卡牌开始翻转
		BaseSprite *openedCard = (BaseSprite*)cardItem->getChildByTag(Tag_CardOpened);
		openedCard->runAction(CCSequence::create(
			CCDelayTime::create(1 * .5 + delayTime),  
			CCShow::create(),  
			CCOrbitCamera::create(1 * .5, 1, 0, 270, 90, 0, 0), 
			NULL));
		//背面卡牌开始翻转
		BaseSprite *closedCard = (BaseSprite*)cardItem->getChildByTag(Tag_CardClosed);
		closedCard->runAction(CCSequence::create(
			CCDelayTime::create(delayTime), 
			CCOrbitCamera::create(1 * .5, 1, 0, 0, 90, 0, 0),  
			CCHide::create(),  
			CCDelayTime::create(1 * .5),  
			NULL));
	}
	//tishi
	temp.initWithFormat("%s%s: %sx%u", LFStrings::getValue("gongxini").c_str(), LFStrings::getValue("huode").c_str(),
		CS::getItemName(items[choseTag].type).c_str(), items[choseTag].count);
	CCLabelTTF *tishiLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
	mChoseCardMenuItem->getParent()->getParent()->addChild(tishiLabel);
	tishiLabel->setPosition(ccp(260, 348));
	tishiLabel->setOpacity(0);
	tishiLabel->runAction(CCSequence::create(CCDelayTime::create(0.5f), CCFadeTo::create(0.5f, 255), NULL));
}

void XueSeJiaoTangFightPrepareLayer::menuItemClicked_XiQuNengLiang(CCObject *pSender)
{
	CCString temp;
	//弹框
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	dialog->setOpacity(0);
	BaseSprite* window = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->addChild(window, -1);
	window->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
	window->setScale(1.2f);
	//确认 取消按键 
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	CCMenuItemImage *closeItem = CCMenuItemImage::create(
		"shangpin_goumai_guanbi.png"
		, "shangpin_goumai_guanbi_select.png"
		, this, menu_selector(XueSeJiaoTangFightPrepareLayer::menuItemClicked_JiQuNengLiangBoard));
	menu->addChild(closeItem, 1, Tag_MenuItem_Close);
	closeItem->setPosition(ccp(dialog->getContentSize().width/2,40));
	//content
	{
		CCSize size = dialog->getContentSize();
		//拥有能量
		BaseSprite *yynlLabel = BaseSprite::create("xuesejiaotang_zhandou_jiqunengliang_wenzi.png");
		dialog->addChild(yynlLabel);
		yynlLabel->setPosition(ccp(50, 40));
		temp.initWithFormat("%u", XueZhanManager::getManager()->getCurrentPowerNum());
		mAllNengLiang_JiQuDialog = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
		dialog->addChild(mAllNengLiang_JiQuDialog, 1);
		mAllNengLiang_JiQuDialog->setAnchorPoint(ccp(0,0.5));
		mAllNengLiang_JiQuDialog->setPosition(ccp(yynlLabel->getPositionX()+yynlLabel->getContentSize().width/2, yynlLabel->getPositionY()));
		//title bar
		BaseSprite *bar = BaseSprite::create("xuesejiaotang_zhandou_jiqunengliang_shuxing_biaoshi.png");
		dialog->addChild(bar, 1);
		bar->setPosition(ccp(dialog->getContentSize().width/2, 340));
		//
		unsigned int expend, gain;
		string str1[6], str2[6];
		//攻击
		CS::getSelfChlgGain(SelfChlgGainType_Attack, SelfChlgGainDegree_Low, expend, gain);
		temp.initWithFormat("%s+%u%%", LFStrings::getValue("gongJi").c_str(), expend);
		str1[0] = temp.getCString();
		temp.initWithFormat("%s%s:%u", LFStrings::getValue("XiaoHao").c_str(), LFStrings::getValue("NengLiang").c_str(), gain);
		str2[0] = temp.getCString();
		CS::getSelfChlgGain(SelfChlgGainType_Attack, SelfChlgGainDegree_High, expend, gain);
		temp.initWithFormat("%s+%u%%", LFStrings::getValue("gongJi").c_str(), expend);
		str1[1] = temp.getCString();
		temp.initWithFormat("%s%s:%u", LFStrings::getValue("XiaoHao").c_str(), LFStrings::getValue("NengLiang").c_str(), gain);
		str2[1] = temp.getCString();
		//护甲
		CS::getSelfChlgGain(SelfChlgGainType_Defense, SelfChlgGainDegree_Low, expend, gain);
		temp.initWithFormat("%s+%u%%", LFStrings::getValue("huJia").c_str(), expend);
		str1[2] = temp.getCString();
		temp.initWithFormat("%s%s:%u", LFStrings::getValue("XiaoHao").c_str(), LFStrings::getValue("NengLiang").c_str(), gain);
		str2[2] = temp.getCString();
		CS::getSelfChlgGain(SelfChlgGainType_Defense, SelfChlgGainDegree_High, expend, gain);
		temp.initWithFormat("%s+%u%%", LFStrings::getValue("huJia").c_str(), expend);
		str1[3] = temp.getCString();
		temp.initWithFormat("%s%s:%u", LFStrings::getValue("XiaoHao").c_str(), LFStrings::getValue("NengLiang").c_str(), gain);
		str2[3] = temp.getCString();
		//生命
		CS::getSelfChlgGain(SelfChlgGainType_HP, SelfChlgGainDegree_Low, expend, gain);
		temp.initWithFormat("%s+%u%%", LFStrings::getValue("shengMin").c_str(), expend);
		str1[4] = temp.getCString();
		temp.initWithFormat("%s%s:%u", LFStrings::getValue("XiaoHao").c_str(), LFStrings::getValue("NengLiang").c_str(), gain);
		str2[4] = temp.getCString();
		CS::getSelfChlgGain(SelfChlgGainType_HP, SelfChlgGainDegree_High, expend, gain);
		temp.initWithFormat("%s+%u%%", LFStrings::getValue("shengMin").c_str(), expend);
		str1[5] = temp.getCString();
		temp.initWithFormat("%s%s:%u", LFStrings::getValue("XiaoHao").c_str(), LFStrings::getValue("NengLiang").c_str(), gain);
		str2[5] = temp.getCString();
		for (int i=0; i<6; ++i)
		{
			CCSprite *s[3];
			s[0] = CCSprite::create("xuesejiaotang_zhandou_jiqunengliang_anniu_jiacheng.png");
			s[1] = CCSprite::create("xuesejiaotang_zhandou_jiqunengliang_anniu_jiacheng_select.png");
			s[2] = CCSprite::create("xuesejiaotang_zhandou_jiqunengliang_anniu_jiacheng_suoding.png");
			for (int num=0; num<3; ++num)
			{
				CCLabelTTF *addNumLabel = CCLabelTTF::create(str1[i].c_str(), fontStr_kaiti, 24);
				addNumLabel->setColor(fonColor_FaGuang);
				s[num]->addChild(addNumLabel);
				addNumLabel->setPosition(ccp(s[num]->getContentSize().width/2, s[num]->getContentSize().height-34));
				CCLabelTTF *needNumLabel = CCLabelTTF::create(str2[i].c_str(), fontStr_kaiti, 20);
				needNumLabel->setColor(fonColor_PuTong);
				s[num]->addChild(needNumLabel);
				needNumLabel->setPosition(ccp(addNumLabel->getPositionX(), s[num]->getContentSize().height-63));
				if (num == 1)
				{
					addNumLabel->setScale(0.92f);
					needNumLabel->setScale(0.92f);
				}
			}
			CCMenuItemSprite *item = CCMenuItemSprite::create(s[0], s[1], s[2], 
				this, menu_selector(XueSeJiaoTangFightPrepareLayer::menuItemClicked_JiQuNengLiangBoard));
			menu->addChild(item, 1, Tag_JiQuMenuItem_GongJi_1 + i);
			item->setPosition(ccp(80+i/2*180, 264-i%2*110));
		}
	}
	mJiQuDialogMenu = menu;
	refreshJiQuNengLiangItem(menu); //能量不足的setenable(false)
	//
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setTag(Tag_Window_JiQuNengLiang);
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_Window_JiQuNengLiang);
		lfAlert->release();
	}
}

void XueSeJiaoTangFightPrepareLayer::menuItemClicked_JiQuNengLiangBoard(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_Close)
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_JiQuNengLiang);
	}
	else if (tag >= Tag_JiQuMenuItem_GongJi_1 && tag <= Tag_JiQuMenuItem_ShengMing_2)
	{
		SelfChlgGainType type;
		SelfChlgGainDegree lv;
		if (tag == Tag_JiQuMenuItem_GongJi_1 || tag == Tag_JiQuMenuItem_GongJi_2)
			type = SelfChlgGainType_Attack;
		else if (tag == Tag_JiQuMenuItem_HuJia_1 || tag == Tag_JiQuMenuItem_HuJia_2)
			type = SelfChlgGainType_Defense;
		else if (tag == Tag_JiQuMenuItem_ShengMing_1 || tag == Tag_JiQuMenuItem_ShengMing_2)
			type = SelfChlgGainType_HP;
		else 
			CCAssert(false,"");

		if ((tag - Tag_JiQuMenuItem_GongJi_1) % 2 == 1)
			lv = SelfChlgGainDegree_High;
		else 
			lv = SelfChlgGainDegree_Low;

		CCNotificationCenter::sharedNotificationCenter()->addObserver(this, 
			callfuncO_selector(XueSeJiaoTangFightPrepareLayer::rsp_JiQuNengLiang),
			MSG_selfChallengeGainRsp, NULL);

		CmdHelper::getHelper()->cmdSelfChallengeGain(type, lv);
	}
}

void XueSeJiaoTangFightPrepareLayer::rsp_JiQuNengLiang(CCObject *msg)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_selfChallengeGainRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_SelfChallengeGainRsp * data = (SPCmd_SelfChallengeGainRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	//
	refreshJiQuNengLiangItem(mJiQuDialogMenu);
	refreshJiQuNengLiangDataLabel();
}

void XueSeJiaoTangFightPrepareLayer::refreshJiQuNengLiangItem(CCMenu *dialogMenu)
{
	CCMenuItemSprite *gongjiItem1 = (CCMenuItemSprite*)dialogMenu->getChildByTag(Tag_JiQuMenuItem_GongJi_1);
	CCMenuItemSprite *gongjiItem2 = (CCMenuItemSprite*)dialogMenu->getChildByTag(Tag_JiQuMenuItem_GongJi_2);
	CCMenuItemSprite *hujiaItem1 = (CCMenuItemSprite*)dialogMenu->getChildByTag(Tag_JiQuMenuItem_HuJia_1);
	CCMenuItemSprite *hujiaItem2 = (CCMenuItemSprite*)dialogMenu->getChildByTag(Tag_JiQuMenuItem_HuJia_2);
	CCMenuItemSprite *hpItem1 = (CCMenuItemSprite*)dialogMenu->getChildByTag(Tag_JiQuMenuItem_ShengMing_1);
	CCMenuItemSprite *hpItem2 = (CCMenuItemSprite*)dialogMenu->getChildByTag(Tag_JiQuMenuItem_ShengMing_2);

	unsigned int curAllNengLiang = XueZhanManager::getManager()->getCurrentPowerNum();
	unsigned int expend, gain;

	CS::getSelfChlgGain(SelfChlgGainType_Attack, SelfChlgGainDegree_Low, expend, gain);
	if (curAllNengLiang < expend) gongjiItem1->setEnabled(false);
	CS::getSelfChlgGain(SelfChlgGainType_Attack, SelfChlgGainDegree_High, expend, gain);
	if (curAllNengLiang < expend) gongjiItem2->setEnabled(false);

	CS::getSelfChlgGain(SelfChlgGainType_Defense, SelfChlgGainDegree_Low, expend, gain);
	if (curAllNengLiang < expend) hujiaItem1->setEnabled(false);
	CS::getSelfChlgGain(SelfChlgGainType_Defense, SelfChlgGainDegree_High, expend, gain);
	if (curAllNengLiang < expend) hujiaItem2->setEnabled(false);

	CS::getSelfChlgGain(SelfChlgGainType_HP, SelfChlgGainDegree_Low, expend, gain);
	if (curAllNengLiang < expend) hpItem1->setEnabled(false);
	CS::getSelfChlgGain(SelfChlgGainType_HP, SelfChlgGainDegree_High, expend, gain);
	if (curAllNengLiang < expend) hpItem2->setEnabled(false);
}

void XueSeJiaoTangFightPrepareLayer::refreshJiQuNengLiangDataLabel()
{
	CCString temp;

	//dialog all 能量
	temp.initWithFormat("%u", XueZhanManager::getManager()->getCurrentPowerNum());
	mAllNengLiang_JiQuDialog->setString(temp.getCString());

	//mbg
	// 拥有能量
	mAllNengLiangL->setString(temp.getCString());
	// 血量加成
	int bloodAddNum = XueZhanManager::getManager()->getCurrentHpAddPercent();
	if (bloodAddNum > 0)
	{
		temp.initWithFormat("+%d%%", bloodAddNum);
		mAddXueLiangL->setString(temp.getCString()); 
		mAddXueLiangL->setColor(fonColor_ChengGong);
	}
	// 攻击加成
	int gongJiAddNum = XueZhanManager::getManager()->getCurrentGongJiAddPercent();
	if (gongJiAddNum > 0)
	{
		temp.initWithFormat("+%d%%", gongJiAddNum);
		mAddGongJiL->setString(temp.getCString());
		mAddGongJiL->setColor(fonColor_ChengGong);
	}   
	// 防御加成
	int fangYuAddNum = XueZhanManager::getManager()->getCurrentHuJiaAddPercent();
	if (fangYuAddNum > 0)
	{
		temp.initWithFormat("+%d%%", fangYuAddNum);
		mAddFangYuL->setString(temp.getCString()); 
		mAddFangYuL->setColor(fonColor_ChengGong);
	}
}

void XueSeJiaoTangFightPrepareLayer::menuItemClicked_fightCircle(CCObject* pSender)
{
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this, 
		callfuncO_selector(XueSeJiaoTangFightPrepareLayer::rsp_Challenge),
		MSG_selfChallengeRsp_F, NULL);
	CmdHelper::getHelper()->cmdSelfChallenge();
}

void XueSeJiaoTangFightPrepareLayer::rsp_Challenge(CCObject *msg_f)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_selfChallengeRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_SelfChallengeRsp * data = (Flat_SelfChallengeRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(data->fightScript);
	dir_vec.push_back(dir);

	Bundle bundle;
	bundle.putString("JumpToWhenEnd","XueSeJiangTang");
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(
		dir_vec
		,MapsManager::getManager()->getXueSeJiaoTangMapImage(XueZhanManager::getManager()->getPassedNum() + 1)
		,bundle);
}