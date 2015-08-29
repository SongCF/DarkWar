
#include "MainLayerStoreDaoju.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "MarketManager.h"
#include "GameDirector.h"
#include "MainLayerStoreFrame.h"
#include "LFAlert.h"
#include "MessageDisplay.h"
#include "FXToast.h"
#include "FXScrollMenu.h"
#include "FX_CommonFunc.h"

#define TAG_BG_MASK_PIC 22221
#define LABEL_COLOR ccc3(127,127,127)


#include "font.h"

//06-14 20:50:24.949: D/cocos2d-x debug info(6365): MainLayerGongjiangFrame::tableCellAtIndex new --> index = [4],file = [function_knapsack.png]

MainLayerStoreDaoju::MainLayerStoreDaoju(MainLayerBase* footLayer, CCSize contentSize)
{
	//CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(
	//	"baoshi.plist",
	//	"baoshi.png");

	//CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("baoshi.plist");

	//
	mFooterLayer = footLayer;


	mTableView->retain();
	mTableView->initWithViewSize(CCSize(613, contentSize.height));
	mTableView->setPosition(ccp(13.5, 13));
	mTableView->release();


	unsigned int vipLevel = MyselfManager::getManager()->getMyZhuJueData()->getVipLevel();
	//
	{
		std::vector<MarketItem> markItemVec = MarketManager::getManager()->getAllItems();

		std::vector<ItemInfo> itemVec;
		for(int i=0; i<markItemVec.size(); ++i)
		{
			//约束
			//熔金石去掉
// 			if (markItemVec[i].item_sell.type == ItemsType_GoldStar_Qianghua)
// 				continue;
// 
// 			itemVec.push_back(markItemVec[i].item_sell);
// 			mPriceVec.push_back(markItemVec[i].item_price);
		}

		setItemsData(itemVec);
	}

}

void MainLayerStoreDaoju::onEnter()
{
	MainLayerItemsBaseBody::onEnter();
	mTableView->reloadData();
}

BaseSprite* MainLayerStoreDaoju::getItemContentByIndex(unsigned int index)
{
	BaseSprite* cellContent = MainLayerItemsBaseBody::getItemContentByIndex(index);
	//名字
	{
		CCLabelTTF* nameLabel = (CCLabelTTF*)cellContent->getChildByTag(TAG_BaseBody_LABLE_NAME);
		if(nameLabel){
			unsigned int item_type = mItemsVec[index].type;
			ccColor3B color = FX_CommonFunc::getCaiLiaoColor(item_type);
			nameLabel->setColor(color);
		}
	}
	//属性
	{
		CCLabelTTF* lableShuXing = (CCLabelTTF*)cellContent->getChildByTag(TAG_BaseBody_LABLE_SHUXING);
		lableShuXing->setColor(ccc3(201, 156, 18));
		lableShuXing->setDimensions(CCSize(330, 0));
		lableShuXing->setAnchorPoint(ccp(0, 1));
		lableShuXing->setPosition(ccp(142, 99));
	}
	//数量
	{
		CCLabelTTF* lable = (CCLabelTTF*)cellContent->getChildByTag(TAG_BaseBody_LABLE_COUNT_NUM);
		char str[64];
		sprintf(str, "%d", mItemsVec[index].count);
		lable->setString(str);

	}

	//价格图标
	{
		BaseSprite* pic = NULL;
		if (mPriceVec.at(index).type == ItemsType_Gold)
			pic = BaseSprite::create("jinbi_tubiao.png");
		else if (mPriceVec.at(index).type == ItemsType_Silver)
			pic = BaseSprite::create("yinbi_tubiao.png");
		else
		{
			pic = BaseSprite::create();
			CCLog("Error : %s --> error prince type [only gold or silver]", __FUNCTION__);
		}
		cellContent->addChild(pic);
		pic->setPosition(ccp(580, 148-111));
		pic->setTag(TAG_BaseBody_LABLE_PRICE_PIC);
		pic->setScale(0.8f);
	}

	//价格数值
	{
		char str[32];
		sprintf(str, "%d",mPriceVec[index].count);
		CCLabelTTF *label = CCLabelTTF::create(str, fontStr_kaiti, 20);
		cellContent->addChild(label,1,TAG_BaseBody_LABLE_PRICE_NUM);
		label->setAnchorPoint(ccp(1, 0.5f));
		label->setPosition(ccp(566,148-111));
	}

	//购买按钮
	{
		FXScrollMenu* menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		cellContent->addChild(menu, 0, TAG_BaseBody_MENU_BUY);
		menu->setPosition(CCPointZero);

		CCMenuItemImage* menuItem = CCMenuItemImage::create(
			"shangdian_goumai.png",
			"shangdian_goumai_select.png",
			this,SEL_MenuHandler(&MainLayerStoreDaoju::menuItemClicked_Buy));
		menu->addChild(menuItem, 0, index);
		menuItem->setPosition(ccp(534, 148-57));

//		//降低menu优先级
//		changeMenuHandlerPriority(menu, false);
	}


	return cellContent;
}

void MainLayerStoreDaoju::rsp_BuyItems(CCObject* msg)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_buyItemInMarketRsp);

	SPCmd_BuyItemInMarketRsp* rsp = (SPCmd_BuyItemInMarketRsp*)((MSG_Rsp*)msg)->getData();
	if (rsp->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

//	FXToast *layer = FXToast::create(LFStrings::getValue("goumaichenggong"), ccc3(201, 156, 18));
	FXToast *layer = FXToast::create(LFStrings::getValue("goumaichenggong"));

	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);

	refreshMoneyItem();
}

void MainLayerStoreDaoju::refreshMoneyItem()
{	
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

	}
	//刷新银币数量
	{
		int silver = 0;
		WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
		if (wp != NULL)
		{
			silver = wp->getCount();
		}

		BaseSprite* header = mFooterLayer->getHeader()->getHeaderContent();

		CCLabelTTF* lable = (CCLabelTTF*)header->getChildByTag(STORE_FRAM_TAG_HEAD_YINBI);
		char str[64];
		sprintf(str, "%d", silver);
		lable->setString(str);
	}
}

void MainLayerStoreDaoju::menuItemClicked_Buy(CCObject* obj)
{
	//获取被点击的购买按钮所对应的index
	int index = ((CCNode*)obj)->getTag();
	mItemIndexToBuy = index;

	{
		int allGold = WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount();
		int allSilver = WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount();

		if (mPriceVec.at(mItemIndexToBuy).type == ItemsType_Gold && allGold < mPriceVec[mItemIndexToBuy].count)
		{
			MessageDisplay *layer = MessageDisplay::create((LFStrings::getValue("suoxujinbibuzu")));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);
			return;
		}	
		else if (mPriceVec.at(mItemIndexToBuy).type == ItemsType_Silver && allSilver < mPriceVec[mItemIndexToBuy].count)
		{
			MessageDisplay *layer = MessageDisplay::create((LFStrings::getValue("suoxuyinbibuzu")));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);
			return;
		}
	}

	//购买
	{
		CCAssert(false, "20140619 协议改了，商店重做。");
// 		addObserver(SEL_CallFuncO(&MainLayerStoreDaoju::rsp_BuyItems),MSG_buyItemInMarketRsp);
// 
// 		SPCmd_BuyItemInMarket req;
// 		req.id = mItemsVec
// 		req.item_type = mItemsVec[mItemIndexToBuy].type;
// 		req.item_count = mItemsVec[mItemIndexToBuy].count;
// 		req.buy_times = 1;
// 
// 		CmdHelper::getHelper()->cmdBuyItemInMarket(req);
	}
}

void MainLayerStoreDaoju::disableAllTouchBegin()
{
	MainLayerItemsBaseBody::disableAllTouchBegin();

	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
			if (bg)
			{
				//购买按键 
				CCMenu* menu = (CCMenu*)bg->getChildByTag(TAG_BaseBody_MENU_BUY);
				if (menu)
				{
					menu->setEnabled(false);
				}
			}
		}
	}
}
void MainLayerStoreDaoju::disableAllTouchEnd()
{
	MainLayerItemsBaseBody::disableAllTouchEnd();

	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
			if (bg)
			{
				//购买按键 
				CCMenu* menu = (CCMenu*)bg->getChildByTag(TAG_BaseBody_MENU_BUY);
				if (menu)
				{
					menu->setEnabled(true);
				}
			}
		}
	}
}


MainLayerStoreDaoju::~MainLayerStoreDaoju(void)
{
}
