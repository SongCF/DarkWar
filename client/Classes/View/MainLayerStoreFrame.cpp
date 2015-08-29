#include "MainLayerStoreFrame.h"
#include "MainLayerStoreDaoju.h"
#include "MarketManager.h"
#include "GameDirector.h"
#include "font.h"
#include "ChongZhiLayer.h"
#include "ShenBingShangDian.h"
#include "MainLayerBase.h"
#include "LibaoLayer.h"

enum 
{
	TAG_ITEM_ShenBing = 1,
	TAG_ITEM_DAOJU,
	TAG_ITEM_LiBao,
	TAG_ITEM_CHONGZHI
};


MainLayerStoreFrame::MainLayerStoreFrame(enumStore flag)
{
	mFlag = flag;
	mContentLayer = NULL;

	mBg = CCScale9Sprite::create("shangdian_bg.jpg");
	addChild(mBg);
	float h = mWinSize.height - BaseSprite::create("zhuangtailan_jinqian.png")->getContentSize().height
		- MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height;
	mBg->setContentSize(CCSize(mBg->getContentSize().width, h));
	mBg->setPosition(ccp(mWinSize.width/2,mBg->getContentSize().height/2));

	//创建按钮
	{
		CCMenuItemImage* shenbingItem = CCMenuItemImage::create(
			"shangdian_shenbin_dengdai.png",
			"shangdian_shenbin_anxia.png",
			"shangdian_shenbin_select.png",
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
// 		CCMenuItemImage* daojuItem = CCMenuItemImage::create(
// 			"shangdian_daoju_dengdai.png",
// 			"shangdian_daoju_anxia.png",
// 			"shangdian_daoju_select.png",
// 			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
		CCMenuItemImage* libaoItem = CCMenuItemImage::create(
			"shangdian_libao_dengdai.png",
			"shangdian_libao_anxia.png",
			"shangdian_libao_select.png", 
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
		CCMenuItemImage* chongzhiItem = CCMenuItemImage::create(
			"shangdian_chongzhi_dengdai.png",
			"shangdian_chongzhi_anxia.png",
			"shangdian_chongzhi_select.png", 
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));


		mMenu = CCMenu::create(shenbingItem, libaoItem, chongzhiItem, NULL);
//		mMenu->addChild(daojuItem);
		mBg->addChild(mMenu,1);
		mMenu->setPosition(CCPointZero);


		shenbingItem->setPosition(ccp(mBg->getContentSize().width/2-shenbingItem->getContentSize().width - 20, mBg->getContentSize().height-144));
		shenbingItem->setTag(TAG_ITEM_ShenBing);

//		daojuItem->setPosition(ccp(259,shenbingItem->getPositionY()));
//		daojuItem->setTag(TAG_ITEM_DAOJU);

		libaoItem->setPosition(ccp(mBg->getContentSize().width/2,shenbingItem->getPositionY()));
		libaoItem->setTag(TAG_ITEM_LiBao);

		chongzhiItem->setPosition(ccp(mBg->getContentSize().width/2+chongzhiItem->getContentSize().width + 20,shenbingItem->getPositionY()));
		chongzhiItem->setTag(TAG_ITEM_CHONGZHI);

	}

	//
	if(MarketManager::getManager()->itemCount() == 0)
	{
		//
		mMenu->setEnabled(false);

		//加载市场数据
		addObserver(SEL_CallFuncO(&MainLayerStoreFrame::getMarketInfoCallBack),MSG_getMarketInfoRsp);
		CmdHelper::getHelper()->cmdGetMarketInfo();
	}
	else
	{
		mFooterLayer = MainLayerBase::getCurrentMainBase();
		if (mFlag == store_ShenBing)
			menuItemClicked(mMenu->getChildByTag(TAG_ITEM_ShenBing));
		else if (mFlag == store_DaoJu)
			CCLOG("TAG_ITEM_DAOJU");
//			menuItemClicked(mMenu->getChildByTag(TAG_ITEM_DAOJU));
		else if (mFlag == store_LiBao)
			menuItemClicked(mMenu->getChildByTag(TAG_ITEM_LiBao));
		else if (mFlag == store_ChongZhi)
			menuItemClicked(mMenu->getChildByTag(TAG_ITEM_CHONGZHI));
	}
	
}

void MainLayerStoreFrame::getMarketInfoCallBack(CCObject* msg)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_getMarketInfoRsp);

	//初始化为道具界面
	{
		mMenu->setEnabled(true);

		if (mFlag == store_ShenBing)
			menuItemClicked(mMenu->getChildByTag(TAG_ITEM_ShenBing));
		else if (mFlag == store_DaoJu)
			menuItemClicked(mMenu->getChildByTag(TAG_ITEM_DAOJU));
		else if (mFlag == store_LiBao)
			menuItemClicked(mMenu->getChildByTag(TAG_ITEM_LiBao));
		else if (mFlag == store_ChongZhi)
			menuItemClicked(mMenu->getChildByTag(TAG_ITEM_CHONGZHI));
	}
}

void MainLayerStoreFrame::menuItemClicked(CCObject* sender)
{
	CCMenuItemImage* sbItem = (CCMenuItemImage*)mMenu->getChildByTag(TAG_ITEM_ShenBing);
//	CCMenuItemImage* djItem = (CCMenuItemImage*)mMenu->getChildByTag(TAG_ITEM_DAOJU);
	CCMenuItemImage* lbItem = (CCMenuItemImage*)mMenu->getChildByTag(TAG_ITEM_LiBao);
	CCMenuItemImage* czItem = (CCMenuItemImage*)mMenu->getChildByTag(TAG_ITEM_CHONGZHI);

	//初始化所有按钮的图片
	{
		sbItem->initWithNormalImage(
			"shangdian_shenbin_dengdai.png",
			"shangdian_shenbin_anxia.png",
			"shangdian_shenbin_select.png",
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
// 		djItem->initWithNormalImage(
// 			"shangdian_daoju_dengdai.png",
// 			"shangdian_daoju_anxia.png",
// 			"shangdian_daoju_select.png",
// 			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
		lbItem->initWithNormalImage(
			"shangdian_libao_dengdai.png",
			"shangdian_libao_anxia.png",
			"shangdian_libao_select.png", 
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
		czItem->initWithNormalImage(
			"shangdian_chongzhi_dengdai.png",
			"shangdian_chongzhi_anxia.png",
			"shangdian_chongzhi_select.png", 
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
	}

	if(mContentLayer != NULL)
	{
		mContentLayer->removeFromParent();
		mContentLayer = NULL;
	}
	//
	CCSize contentSize = CCSize(mWinSize.width, mBg->getContentSize().height-195-15);
	CCMenuItemImage* item = (CCMenuItemImage*)(sender);
	int tag = item->getTag();
	if (tag == TAG_ITEM_ShenBing)
	{
		ShenBingShangDian *layer = new ShenBingShangDian(mFooterLayer, contentSize);
		mContentLayer = layer;
		addChild(mContentLayer, 1);
		layer->release();
		mContentLayer->setPosition(0, 0);

		//调换图片
		item->initWithNormalImage(
			"shangdian_shenbin_select.png",
			"shangdian_shenbin_anxia.png",
			NULL,
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
	}
	else if (tag == TAG_ITEM_DAOJU)
	{
		MainLayerStoreDaoju* layer = new MainLayerStoreDaoju(mFooterLayer, contentSize);
		mContentLayer = layer;
		addChild(mContentLayer, 1);
		layer->release();
		mContentLayer->setPosition(0, 0);
		
		//调换图片
		item->initWithNormalImage(
			"shangdian_daoju_select.png",
			"shangdian_daoju_anxia.png",
			NULL,
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
	}
	else if (tag == TAG_ITEM_LiBao)
	{
		LibaoLayer* layer = new LibaoLayer(mFooterLayer, contentSize);
		mContentLayer = layer;
		addChild(mContentLayer, 1);
		layer->release();
		mContentLayer->setPosition(0, 0);

		//调换图片
		item->initWithNormalImage(
			"shangdian_libao_select.png",
			"shangdian_libao_anxia.png",
			NULL,
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
	}
	else if (tag == TAG_ITEM_CHONGZHI)
	{
		ChongZhiLayer *layer = new ChongZhiLayer(mFooterLayer, contentSize);
		mContentLayer = layer;
		addChild(mContentLayer, 1);
		layer->release();
		mContentLayer->setPosition(ccp(0,0));

		//调换图片
		item->initWithNormalImage(
			"shangdian_chongzhi_select.png",
			"shangdian_chongzhi_anxia.png",
			NULL,
			this,SEL_MenuHandler(&MainLayerStoreFrame::menuItemClicked));
	}
	
}

void MainLayerStoreFrame::disableAllTouchBegin()
{
	mMenu->setEnabled(false);

	mContentLayer->disableAllTouchBegin();
}
void MainLayerStoreFrame::disableAllTouchEnd()
{
	mMenu->setEnabled(true);

	if(mContentLayer)
		mContentLayer->disableAllTouchEnd();
}


MainLayerStoreFrame::~MainLayerStoreFrame(void)
{
}
