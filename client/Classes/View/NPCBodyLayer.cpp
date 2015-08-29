#include "NPCBodyLayer.h"
#include "MapsLayer.h"
#include "LFUtile.h"
#include "GameDirector.h"
#include "TaskManager.h"
#include "NpcFuBenLayer.h"
#include "flat.h"
#include "MessageDisplay.h"


//#define TAG_ITEM_TIE_JIANG		1233421		// 铁匠
//#define TAG_ITEM_TIE_FIGHT		1233422		// 打仗，以后会分成日常副本，时段副本等

//#define TAG_LABLE 213445	// 说明的lable

//////////
#define TAG_ITEM_NPC            1233423
#define TAG_ITEM_MAPS           1233424

//#define  fontStr  "Helvetica"
#define  lableNameSize  30.0f

NPCBodyLayer::NPCBodyLayer(int flag)
	:mFlag(flag)
{
	mContentLayer = NULL;

// 	mBg = BaseSprite::create("NPC_bg.png");
// 	mBg->retain();
// 	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));
// 	addChild(mBg);
// 
// 	// 头	
// 	mHead = BaseSprite::create("zhandou_biaoti_bg.png");
// 	mHead->retain();
// 	mBg->addChild(mHead, 10);
// 	CCPoint headPos = ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-mHead->getContentSize().height/2);
// 	mHead->setPosition(headPos);
	
	//创建按钮
// 	{
// 		CCMenuItemImage* npcItem = CCMenuItemImage::create(
// 			"zhandou_biaoti_fuben.png",
// 			"zhandou_biaoti_fuben_select.png",
// 			"zhandou_biaoti_fuben.png",
// 			this,SEL_MenuHandler(&NPCBodyLayer::menuItemClicked));
// 		CCMenuItemImage* mapsItem = CCMenuItemImage::create(
// 			"zhandou_biaoti_ditu.png",
// 			"zhandou_biaoti_ditu_select.png",
// 			"zhandou_biaoti_ditu.png",
// 			this,SEL_MenuHandler(&NPCBodyLayer::menuItemClicked));
// 
// 		mMenu = CCMenu::create(npcItem, mapsItem, NULL);
// 		mMenu->retain();/////////
// 		mHead->addChild(mMenu,1);
// 		mMenu->setPosition(CCPointZero);
// 
// 		npcItem->setPosition(ccp(mWinSize.width/3*2, mHead->getContentSize().height/2));
// 		npcItem->setTag(TAG_ITEM_NPC);
// 
// 		mapsItem->setPosition(ccp(mWinSize.width/3, mHead->getContentSize().height/2));
// 		mapsItem->setTag(TAG_ITEM_MAPS);
// 
// 		mMenu->setEnabled(false);
// 
// 		if (mFlag == 0) 
// 		{
// 			npcItem->setVisible(false);
// 			mapsItem->setEnabled(false);
// 			mapsItem->setPositionX(mHead->getContentSize().width/2);
// 		}
// 		else
// 		{
// 			mapsItem->setVisible(false);
// 			npcItem->setEnabled(false);
// 			npcItem->setPositionX(mHead->getContentSize().width/2);
// 		}
// 	}

	
	if(TaskManager::getManager()->getMainTaskRewardVec().size() == 0)
	{
		addObserver(callfuncO_selector(NPCBodyLayer::queryMainTaskReward_Rsp), MSG_QueryMainTaskRewardRsp_F);
		mCmdHlp->cmdQueryMainTaskReward();
	}

}

void NPCBodyLayer::queryMainTaskReward_Rsp(CCObject *f_msg)
{
	removeObserver(MSG_QueryMainTaskRewardRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_QueryMainTaskRewardRsp* data = (Flat_QueryMainTaskRewardRsp*)((Flat_MSG_Rsp*)f_msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
}

void NPCBodyLayer::initFoot(MainLayerBase* footerLayer)
{
	IMainLayerBody::initFoot(footerLayer);

	setContentLayer(mFlag);	
}

void NPCBodyLayer::disableAllTouchBegin()
{
	mContentLayer->disableAllTouchBegin();
}

void NPCBodyLayer::disableAllTouchEnd()
{
	if(mContentLayer)
		mContentLayer->disableAllTouchEnd();	
}

void NPCBodyLayer::setContentLayer(int tag)
{
// 	if (tag == 0) //地图
// 	{
// 		if(mContentLayer != NULL)
// 		{
// 			mContentLayer->removeFromParent();
// 			mContentLayer = NULL;
// 		}
// 		//...地图
// 		MapsLayer* layer = new MapsLayer(mFooterLayer);
// 		layer->autorelease();
// 		mContentLayer = layer;
// 
// 		addChild(mContentLayer, 1);
// 		mContentLayer->setPosition(0,-8);
// 	}
// 	else
// 	{
// 		CCAssert(false, "go to npc fuben interface has changed !");
// 	}
}

NPCBodyLayer::~NPCBodyLayer(void)
{
}
