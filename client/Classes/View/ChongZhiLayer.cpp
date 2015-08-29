#include "ChongZhiLayer.h"
#include "font.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "WuPinManager.h"
#include "MainLayerStoreFrame.h"
#include "VipExplainLayer.h"
#include "ImageManager.h"
#include "MyselfManager.h"
#include "CS_Common.h"
#include "FXToast.h"
#include "FXScrollMenu.h"

#define LabelSize 20

enum 
{
	Tag_TitleBar = 1,
	Tag_Label_NextVipNeed,   //再冲6元享受、、、
	Tag_Label_NextVip,

	Tag_ShouChongBar,

	Tag_CellBoard,
	Tag_Menu,
	Tag_MenuItem_GouMai,
};

ChongZhiLayer::ChongZhiLayer(MainLayerBase* footerLayer, CCSize contentSize)
{
	mContentHeight = contentSize.height;
	mFooterLayer = footerLayer;
	mHasCharged = true;
	//title
	BaseSprite *titleBar = BaseSprite::create("shangdian_vip_bg.png");
	addChild(titleBar, 1, Tag_TitleBar);
	titleBar->setPosition(ccp(mWinSize.width/2, contentSize.height-titleBar->getContentSize().height/2+13));
	//如果已是最高vip就不提示了   已是满级就返回当前等级
	if (MyselfManager::getManager()->getCurrentVipLevel() < MyselfManager::getManager()->getNextVipLevel())
	{
		//再充值6元享受(white)VIP3级特权(yellow)
		char buf[20];
		sprintf(buf, "%u", MyselfManager::getManager()->getNeedChargeToGetNextVip());
		string temp = LFStrings::getValue("zai") + LFStrings::getValue("chongzhi") + buf 
			+ LFStrings::getValue("yuan") + LFStrings::getValue("XiangShou");
		CCLabelTTF *titleLabel1 = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LabelSize);
		titleBar->addChild(titleLabel1, 0, Tag_Label_NextVipNeed);
		titleLabel1->setAnchorPoint(ccp(0, 0.5f));
		titleLabel1->setPosition(ccp(14,33));
		sprintf(buf, "VIP%u", MyselfManager::getManager()->getNextVipLevel());
		temp = buf + LFStrings::getValue("ji") + LFStrings::getValue("TeQuan");
		CCLabelTTF *label2 = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LabelSize);
		titleBar->addChild(label2, 0, Tag_Label_NextVip);
		label2->setColor(ccYELLOW);
		label2->setPosition(ccp(titleLabel1->getPositionX()+titleLabel1->getContentSize().width+label2->getContentSize().width/2,
			titleLabel1->getPositionY()));
	}
	//menu
	CCMenu *menu = CCMenu::create();
	titleBar->addChild(menu);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *queryVipItem = CCMenuItemImage::create(
		"b1.png", "b2.png", this, menu_selector(ChongZhiLayer::menuItemClicked_QueryVip));
	menu->addChild(queryVipItem);
	queryVipItem->setPosition(ccp(526, 33));

	//
	int tabelViewHeight = contentSize.height-titleBar->getContentSize().height;
	//首冲奖励提示   height   124
	if (! MyselfManager::getManager()->haveChargedBefore())
	{
		mHasCharged = false;

		BaseSprite *scBar = BaseSprite::create("shangdian_shouchong_bg.png");
		addChild(scBar, 1, Tag_ShouChongBar);
		scBar->setPosition(ccp(mWinSize.width/2, titleBar->getPositionY() - titleBar->getContentSize().height/2 - scBar->getContentSize().height/2));
		//award
		CCString temp;
		vector<ItemInfo> itemVec = MyselfManager::getManager()->getFirstChargePresent();

		int num = 0;
		for (int i=0; i<itemVec.size(); ++i)
		{
			if (itemVec.at(i).type > 0) ++num;
		}
		++num;  //首冲金币翻倍

		int xOffset = 70;
		for (int i=0; i!=itemVec.size(); ++i)
		{
			if (itemVec.at(i).type <= 0) continue;

// 			BaseSprite *board = BaseSprite::create("daoju_kuang_bg.png");
// 			scBar->addChild(board);
// 			board->setScale(0.5f);
// 
// 			int delta = 110;
// 			int posX = scBar->getContentSize().width/2;
// 			if (num % 2 == 0)
// 				posX = scBar->getContentSize().width/2 + delta/2 + (i-num/2)*delta;
// 			else
// 				posX = scBar->getContentSize().width/2 + (i-num/2)*delta;
// 			board->setPosition(ccp(posX, scBar->getContentSize().height/2 - 12));
//
//
// 			BaseSprite *img = BaseSprite::create(ImageManager::getManager()->getItemFilename(itemVec.at(i).type));
// 			board->addChild(img);
// 			img->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
// 
			temp.initWithFormat("%s\n*%u", CS::getItemName(itemVec.at(i).type).c_str(), itemVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			nameLabel->setHorizontalAlignment(kCCTextAlignmentCenter);
			scBar->addChild(nameLabel);
			if (i == 2)
			{
				nameLabel->setPosition(ccp(xOffset+110*(i+1)+10, scBar->getContentSize().height/4-10));
			}
			else
			{
				nameLabel->setPosition(ccp(xOffset+110*(i+1), scBar->getContentSize().height/4-10));
			}
		}

		CCLabelTTF *nameLabel = CCLabelTTF::create(LFStrings::getValue("ShouChongJiangLi").c_str(), fontStr_kaiti, 20, CCSizeMake(40, 0), kCCTextAlignmentCenter);
		//nameLabel->setHorizontalAlignment(kCCTextAlignmentCenter);
		scBar->addChild(nameLabel);
		nameLabel->setPosition(ccp(xOffset, scBar->getContentSize().height/4-10));

		tabelViewHeight -= scBar->getContentSize().height;

// 		//首冲
// 
// 		BaseSprite *board = BaseSprite::create("daoju_kuang_bg.png");
// 		scBar->addChild(board);
// 		board->setScale(0.5f);
// 
// 		int delta = 110;
// 		int posX;
// 		if (num % 2 == 0)
// 			posX = scBar->getContentSize().width/2 + delta/2 + (num-1-num/2)*delta;
// 		else
// 			posX = scBar->getContentSize().width/2 + (num-1-num/2)*delta;
// 		board->setPosition(ccp(posX, scBar->getContentSize().height/2 - 12));
// 
// 		BaseSprite *img = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_Gold));
// 		board->addChild(img);
// 		img->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
// 
// 		CCLabelTTF *scjlLabel = CCLabelTTF::create(LFStrings::getValue("ShouChongJiangLi").c_str(), fontStr_kaiti, 30);
// 		board->addChild(scjlLabel, 1);
// 		scjlLabel->setPosition(ccp(board->getContentSize().width/2, -scjlLabel->getContentSize().height/2/board->getScale()+6));
// 		scjlLabel->setColor(fonColor_FaGuang);

	}

	CCLOG("CCTableView------>width %d",tabelViewHeight);
	mTableView = CCTableView::create(this, CCSizeMake(613, tabelViewHeight));
	addChild(mTableView);
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setPosition(ccp(13.5,13));
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);

	WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
	if (wp != NULL)
	{
		mYuanShiJinBi = wp->getCount();
	}

}

void ChongZhiLayer::onEnter()
{
	BaseLayer::onEnter();
	mTableView->reloadData();
}

void ChongZhiLayer::disableAllTouchBegin()
{
	mTableView->setTouchEnabled(false);
}
void ChongZhiLayer::disableAllTouchEnd()
{
	mTableView->setTouchEnabled(true);
}

CCSize ChongZhiLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSizeMake(613, 144);
}

CCTableViewCell* ChongZhiLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCString *string = CCString::createWithFormat("%d", idx);
	CCTableViewCell *cell = table->dequeueCell();

	if (cell != NULL) 
	{
		cell->removeAllChildren();
	}
	else			  
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}

	if (table == mTableView)
	{
		BaseSprite* board = BaseSprite::create("shangdian_shangpin_bg.png");
		cell->addChild(board, 0, Tag_CellBoard);
		board->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));

		//gold img
		BaseSprite *goldImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_Gold));
		board->addChild(goldImg,2);
		goldImg->setPosition(ccp(72, board->getContentSize().height/2));

		//金币背景
		BaseSprite *goldImgBg = BaseSprite::create("daoju_kuang_bg.png");
		board->addChild(goldImgBg,1);
		goldImgBg->setPosition(ccp(72, board->getContentSize().height/2));

		//
		vector<ChargePacketInfo> chargeVec = MyselfManager::getManager()->getChargePacketInfo();
		unsigned int price = chargeVec.at(idx).charge;
		unsigned int goldCount = chargeVec.at(idx).gold_count;
		unsigned int presentCount = chargeVec.at(idx).present_count;

		//获得金币数量
		char buf[128];
		//sprintf(buf, "%s%u%s", LFStrings::getValue("huode").c_str(), goldCount, LFStrings::getValue("jinbi").c_str());
		sprintf(buf, "%u%s",goldCount, LFStrings::getValue("jinbi").c_str());
		CCLabelTTF *goldLabel = CCLabelTTF::create(buf, fontStr_kaiti, 24, CCSize(330, 0), kCCTextAlignmentLeft);
		board->addChild(goldLabel);
		goldLabel->setAnchorPoint(ccp(0, 1));
		goldLabel->setPosition(ccp(142, 148-32));
		goldLabel->setColor(ccc3(253,180,40));
		//赠送金币数量
		std::string ewjinbi=LFStrings::getValue("ewai")+LFStrings::getValue("ZengSong");

		sprintf(buf, "%s%u%s", ewjinbi.c_str(), presentCount, LFStrings::getValue("jinbi").c_str());
		CCLabelTTF *zsNumLabel = CCLabelTTF::create(buf, fontStr_kaiti, LabelSize, CCSize(330,0), kCCTextAlignmentLeft);
		board->addChild(zsNumLabel);
		zsNumLabel->setAnchorPoint(ccp(0, 0.5));
		zsNumLabel->setPosition(ccp(goldLabel->getPositionX(), 
			goldLabel->getPositionY()-goldLabel->getContentSize().height-zsNumLabel->getContentSize().height));
		zsNumLabel->setColor(ccc3(74,115,255));
		//需要冲多少钱
		sprintf(buf, "%s ", LFStrings::getValue("jiaoge").c_str());
		CCLabelTTF *label = CCLabelTTF::create(buf, fontStr_kaiti, 22);
		board->addChild(label);
		label->setColor(ccc3(74,115,255));
		label->setAnchorPoint(ccp(0, 0.5f));
		label->setPosition(ccp(142,32));

		std::string getPrice=CCString::createWithFormat("%u.00",price)->getCString();
		//sprintf(buf, "%u", price);
		CCLabelTTF *yuanLabel = CCLabelTTF::create(getPrice.c_str(), fontStr_kaiti, 20);
		board->addChild(yuanLabel);
		yuanLabel->setColor(ccc3(255,255,255));
		yuanLabel->setPosition(ccp(label->getPositionX()+label->getContentSize().width+yuanLabel->getContentSize().width/2,
			label->getPositionY()));

		FXScrollMenu *menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		board->addChild(menu, 0, Tag_Menu);
		menu->setPosition(CCPointZero);
		CCMenuItemImage *item = CCMenuItemImage::create(
			"shangdian_goumai.png"
			, "shangdian_goumai_select.png"
			, this, menu_selector(ChongZhiLayer::menuItemClicked_GouMai));
		menu->addChild(item, 0, Tag_MenuItem_GouMai);
		item->setPosition(ccp(534, 148-57));
		item->setUserData((void*)chargeVec.at(idx).id);
	}

	return cell;
}

unsigned int ChongZhiLayer::numberOfCellsInTableView(CCTableView *table)
{
	return MyselfManager::getManager()->getChargePacketInfo().size();
}

void ChongZhiLayer::menuItemClicked_QueryVip(CCObject *pSender)
{
	VipExplainLayer *layer = new VipExplainLayer;
	mFooterLayer->getHeader()->setContent(BaseSprite::create());
	mFooterLayer->setContaintLayer(layer,false);
	layer->release();
}

void ChongZhiLayer::menuItemClicked_GouMai(CCObject *pSender)
{
	unsigned int vipId = (unsigned int)((CCMenuItemImage*)pSender)->getUserData();

	{
		addObserver(callfuncO_selector(ChongZhiLayer::rsp_ChargeGold), MSG_ChargeGoldRsp);
		mCmdHlp->cmdChargeGold(vipId);
		//mCmdHlp->cmdChargeGold(10);
	}
}

void ChongZhiLayer::rsp_ChargeGold(CCObject *msg)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_ChargeGoldRsp);

	SPCmd_ChargeGoldRsp* data = (SPCmd_ChargeGoldRsp*)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	string temp = LFStrings::getValue("chongzhi") + LFStrings::getValue("chenggong");
	char buf[20];
	sprintf(buf, " : %d", data->gold_balance - mYuanShiJinBi);
	temp += buf;
	temp += LFStrings::getValue("jinbi");	
//	FXToast* layer = FXToast::create(temp, ccc3(201, 156, 18));
	FXToast* layer = FXToast::create(temp);

	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);


	//刷新金币数量
	{
		int gold = 0;
		WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		if (wp != NULL)
		{
			gold = wp->getCount();
		}

		BaseSprite* header = mFooterLayer->getHeader()->getHeaderContent();

		CCLabelTTF* lable = (CCLabelTTF*)header->getChildByTag(STORE_FRAM_TAG_HEAD_JINBI);
		char str[64];
		sprintf(str, "%d", gold);
		lable->setString(str);

		mYuanShiJinBi = gold;
	}

	//刷新vip下一级提示
	BaseSprite *titleBar = (BaseSprite*)getChildByTag(Tag_TitleBar);
	if (titleBar != NULL)
	{
		CCLabelTTF *titleLabel1 = (CCLabelTTF*)titleBar->getChildByTag(Tag_Label_NextVipNeed);
		if (titleLabel1 != NULL)   
		{
			sprintf(buf, "%u", data->next_need_charge);
			string temp = LFStrings::getValue("zai") + LFStrings::getValue("chongzhi") + buf 
				+ LFStrings::getValue("yuan") + LFStrings::getValue("XiangShou");
			titleLabel1->setString(temp.c_str());

			CCLabelTTF *titleLabel2 = (CCLabelTTF*)titleBar->getChildByTag(Tag_Label_NextVip);
			if (titleLabel2 != NULL)
			{
				sprintf(buf, "VIP%u", data->next_vip_level);
				temp = buf + LFStrings::getValue("ji") + LFStrings::getValue("TeQuan");
				titleLabel2->setString(temp.c_str());

				titleLabel2->setPosition(ccp(titleLabel1->getPositionX()+titleLabel1->getContentSize().width+titleLabel2->getContentSize().width/2,
					titleLabel1->getPositionY()));
			}
		}
	}

	//首冲后去掉首冲bar  刷新tableView
	if (! mHasCharged)
	{
		mHasCharged = true;
		removeChildByTag(Tag_ShouChongBar);
		mTableView->setViewSize(CCSize(mTableView->getContentSize().width, mContentHeight-titleBar->getContentSize().height));
		mTableView->reloadData();
	}
}