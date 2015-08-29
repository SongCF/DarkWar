#include "NpcFuBenLayer.h"
#include "GameDirector.h"
#include "TaskManager.h"
#include "LFStrings.h"
#include "font.h"
//#include "NPCBodyLayer.h"
//#include "JingJichangLayer.h"
#include "TuanDuiLayer.h"
#include "JiXianShouChengLayer.h"
#include "HeroRankDuel.h"
#include "GuideLayerBase.h"
#include "UnblockManager.h"
#include "MyselfManager.h"
#include "MapsManager.h"
#include "XueSeJiaoTangLayer.h"
#include "XueSeJiaoTangFightPrepareLayer.h"
#include "XueZhanManager.h"
#include "FXToast.h"

enum 
{
	Tag_menu   = 10,
	Tag_ShouCheng  , //极限守城
	Tag_TuanDui    , //
	Tag_XueZhan    , //血战
	Tag_DianFeng   , //巅峰

	Tag_Discribe_Img = 30,
};


NpcFuBenLayer* NpcFuBenLayer::create()
{
	NpcFuBenLayer* layer  = new NpcFuBenLayer();
	if(layer && layer->init())
	{
		layer->autorelease();
		return layer;
	}
	else
	{
		CC_SAFE_DELETE(layer);
		layer = NULL;
		return NULL;
	}
}

NpcFuBenLayer::NpcFuBenLayer()
{
	mBg = BaseSprite::create("fuben_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));

	{
		// 210, 851-168
		mTableView = CCTableView::create(this, CCSizeMake(614, 670));
		this->addChild(mTableView);
		mTableView->setDirection(kCCScrollViewDirectionHorizontal);
		mTableView->setPosition(ccp(13, 13));

//		mTableView->setPosition(ccp(13 + 102, 13));  //两个的时候
		mTableView->setTouchEnabled(false);

		mTableView->setDelegate(this);
		mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	}

}

NpcFuBenLayer::~NpcFuBenLayer()
{
}

void NpcFuBenLayer::fubenMenuCallback(CCObject* pSender)
{
	int tag = ((CCMenuItemSprite*)pSender)->getTag();

	BaseLayer* layer = NULL;
	switch (tag)
	{
	case Tag_ShouCheng:
		{
			bool bUnblock =  UnblockManager::getManager()->isDeblocking(unblock_JiXianShouCheng);
			
			if (bUnblock)
			{
				//layer = ShouChengLayer::create();//
				layer = JiXianShouChengLayer::create();
			}
			else
			{
				UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("shouchengfuben_weijiesuo"), false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
				return;
			}
		}	
		break;
	case Tag_TuanDui:
		{
			/////////////
			CCAssert(false, "");
			return;
			///////


			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_TuanDuiFuBen);
			
			if (bUnblock)
			{
				layer = TuanDuiLayer::create();
			}
			else
			{
				UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("tuanduifuben_weijiesuo"), false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
				return;
			}
		}	
		break;
	case  Tag_DianFeng:
		{
			/*
			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_DianFengDuiJue);
			
			if (bUnblock)
			{
				layer = HeroRankDuel::create();
			}
			else
			{
				UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("dianfengduijue_weijiesuo"), false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
				return;
			}
			*/
			string unblockmsg=LFStrings::getValue("jinrushibai")+LFStrings::getValue("BanBen_ZhanWeiKaiQi");
            UnblockWarnDialog *dialog = UnblockWarnDialog::create(unblockmsg, false);
            CCDirector::sharedDirector()->getRunningScene()->addChild(dialog);
			return;


		}	
		break;
	case Tag_XueZhan:
		{
			XueZhanManager::ErrorCode state = XueZhanManager::getManager()->canIJoinXueZhan();
			if (state == XueZhanManager::EC_LevelNotEnough)
			{
				//temp.initWithFormat((LFStrings::getValue("ZhuJue_d_JiKaiQi")
				//	+LFStrings::getValue("xuesejiaotang")+LFStrings::getValue("HuoDong")).c_str(), 11);
				UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("TongGuanDiYiZhang").c_str(), false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
				return;
			}
			else if (state == XueZhanManager::EC_TaskNotEnough)
			{
				CCString temp;
				temp.initWithFormat("must pass unit 1");
				UnblockWarnDialog *layer = UnblockWarnDialog::create(temp.getCString(), false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
				return;
			}
			else if (state == XueZhanManager::EC_NotOpenThisTime)
			{
				UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("HuoDong")+LFStrings::getValue("WeiKaiQi"), false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
				return;
			}
			else 
			{
				XueZhanManager::getManager()->enter();
				if (State_In == XueZhanManager::getManager()->getCurrentState())
				{
					XueSeJiaoTangFightPrepareLayer *layer = new XueSeJiaoTangFightPrepareLayer();
					CCScene *scene = CCScene::create();
					scene->addChild(layer);
					layer->release();
					CCDirector::sharedDirector()->replaceScene(scene);
					return;
				}

				layer = XueSeJiaoTangLayer::create();
			}
		}
		break;
	default:
		break;
	}

	mFooterLayer->setContaintLayer(layer);/////
}

void  NpcFuBenLayer::setAllTouchesDisable()
{
	mTableView->setTouchEnabled(false);
	for (int i=0; i!=numberOfCellsInTableView(mTableView); ++i)
	{
		CCMenu *menu = NULL;
		if(mTableView->cellAtIndex((i) != NULL))
			menu = (CCMenu*)mTableView->cellAtIndex(i)->getChildByTag(Tag_menu);
		if(menu != NULL)
			menu->setTouchEnabled(false);
	}
}

void  NpcFuBenLayer::setAllTouchesEnable()
{
	mTableView->setTouchEnabled(true);
	for (int i=0; i!=numberOfCellsInTableView(mTableView); ++i)
	{
		CCMenu *menu = NULL;
		if(mTableView->cellAtIndex((i) != NULL))
			menu = (CCMenu*)mTableView->cellAtIndex(i)->getChildByTag(Tag_menu);
		if(menu != NULL)
			menu->setTouchEnabled(true);
	}
}

CCTableViewCell* NpcFuBenLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();

	int dscLabel_size = 22;
	{
		if (cell == NULL)
		{	
			cell = new CCTableViewCell();
			cell->autorelease();
		}
		else
		{
			cell->removeAllChildren();
		}

		CCString tempStr;
		CCSize size = tableCellSizeForIndex(table, idx);

		//////////////////////////////////////////////////////////////
		CCMenu* menu = CCMenu::create();
		cell->addChild(menu, 0, Tag_menu);
		menu->setPosition(CCPointZero);

// 		if(idx == 0) // tuandui
// 		{
// 			CCSprite* tdNor = CCSprite::create("zhandou_fuben_tuandui.png");
// 			CCSprite* tdSel = CCSprite::create("zhandou_fuben_tuandui_select.png");
// 			CCSprite* tdDis = CCSprite::create("zhandou_fuben_tuandui_suoding.png");
// 			CCMenuItemSprite* td = CCMenuItemSprite::create(
// 				tdNor, tdSel, tdDis, this, menu_selector(NpcFuBenLayer::fubenMenuCallback));
// 			menu->addChild(td, 1, Tag_TuanDui);
// 			td->setPosition(ccp(size.width/2, size.height - td->getContentSize().height/2));
// 			//
// 			//0.2团队副本
// 			{
// 				CCSprite* tdDiscImg = CCSprite::create("zhandou_fuben_shuoming.png");
// 				cell->addChild(tdDiscImg, 1, Tag_Discribe_Img);
// 				tdDiscImg->setAnchorPoint(ccp(0.5,1));
// 				tdDiscImg->setPosition(ccp(size.width/2, td->getPositionY() - 90));
// 				//副本名
// 				CCLabelTTF* nameLabel = CCLabelTTF::create(LFStrings::getValue("fuben_tuandui").c_str(), fontStr_kaiti, 21);
// 				nameLabel->setColor(ccc3(0xee, 0x9a, 0x00));
// 				tdDiscImg->addChild(nameLabel);
// 				nameLabel->setPosition(ccp(tdDiscImg->getContentSize().width/2, 450));
// 				//说明
// 				CCLabelTTF* DiscLabel = CCLabelTTF::create(LFStrings::getValue("tuanduifuben_dsc").c_str(), fontStr_kaiti
// 					, dscLabel_size,CCSizeMake(126,0),kCCTextAlignmentLeft);
// 				DiscLabel->setAnchorPoint(ccp(0, 1));
// 				//	jjDiscLabel->setColor(ccc3());
// 				tdDiscImg->addChild(DiscLabel);
// 				DiscLabel->setPosition(ccp(42, 420));
// 				//时间
// 				CCLabelTTF* XianzhiLabel = CCLabelTTF::create(LFStrings::getValue("fubenxianzhishuoming").c_str(), fontStr_kaiti, 20);
// 				XianzhiLabel->setAnchorPoint(ccp(0.5,1));
// 				XianzhiLabel->setColor(ccc3(0,255,0));
// 				tdDiscImg->addChild(XianzhiLabel);
// 				XianzhiLabel->setPosition(ccp(tdDiscImg->getContentSize().width/2, 162));
// 			}
// 		}
// 		else if(idx == 1) // shoucheng
		if (idx == 1)
		{
			CCSprite* scNor = CCSprite::create("zhandou_fuben_shoucheng.png");
			CCSprite* scSel = CCSprite::create("zhandou_fuben_shoucheng_select.png");
			CCSprite* scDis = CCSprite::create("zhandou_fuben_shoucheng_suoding.png");
			CCMenuItemSprite* sc = CCMenuItemSprite::create(
				scNor, scSel, scDis, this, menu_selector(NpcFuBenLayer::fubenMenuCallback));
			menu->addChild(sc, 1, Tag_ShouCheng);
			sc->setPosition(ccp(size.width/2, size.height - sc->getContentSize().height/2));
			//
			//0.3守城副本
			{
				CCSprite* scDiscImg = CCSprite::create("zhandou_fuben_shuoming.png");
				cell->addChild(scDiscImg, 1, Tag_Discribe_Img);
				scDiscImg->setAnchorPoint(ccp(0.5,1));
				scDiscImg->setPosition(ccp(size.width/2, sc->getPositionY() - 90));
				//副本名
				CCLabelTTF* nameLabel = CCLabelTTF::create(LFStrings::getValue("fuben_shoucheng").c_str(), fontStr_kaiti, 21);
				nameLabel->setColor(ccc3(0xee, 0x9a, 0x00));
				scDiscImg->addChild(nameLabel);
				nameLabel->setPosition(ccp(scDiscImg->getContentSize().width/2, 450));
				//说明
				CCLabelTTF* DiscLabel = CCLabelTTF::create(LFStrings::getValue("shouchengfuben_dsc").c_str(), fontStr_kaiti
					, dscLabel_size,CCSizeMake(126,0),kCCTextAlignmentLeft);
				DiscLabel->setAnchorPoint(ccp(0, 1));
				//	jjDiscLabel->setColor(ccc3());
				scDiscImg->addChild(DiscLabel);
				DiscLabel->setPosition(ccp(42, 420));
				//时间
				CCLabelTTF* XianzhiLabel = CCLabelTTF::create(LFStrings::getValue("fuben_jixianshoucheng_time").c_str(), fontStr_kaiti, 20);
				XianzhiLabel->setAnchorPoint(ccp(0.5,1));
				XianzhiLabel->setColor(ccc3(0,255,0));
				scDiscImg->addChild(XianzhiLabel);
				XianzhiLabel->setPosition(ccp(scDiscImg->getContentSize().width/2, 162));
			}
		}
		else if(idx == 0) // 血战
		{
			CCSprite* nor = CCSprite::create("zhandou_fuben_xuesejiaotang.png");
			CCSprite* sel = CCSprite::create("zhandou_fuben_xuesejiaotang_select.png");
			CCSprite* dis = CCSprite::create("zhandou_fuben_xuesejiaotang_suoding.png");
			CCMenuItemSprite* xz = CCMenuItemSprite::create(
				nor, sel, dis, this, menu_selector(NpcFuBenLayer::fubenMenuCallback));
			menu->addChild(xz, 1, Tag_XueZhan);

			

			xz->setPosition(ccp(size.width/2, size.height - xz->getContentSize().height/2));
			//
			{
				CCSprite* dfDiscImg = CCSprite::create("zhandou_fuben_shuoming.png");
				cell->addChild(dfDiscImg, 1, Tag_Discribe_Img);
				dfDiscImg->setAnchorPoint(ccp(0.5,1));
				dfDiscImg->setPosition(ccp(size.width/2, xz->getPositionY() - 90));
				//副本名
				CCLabelTTF* nameLabel = CCLabelTTF::create(LFStrings::getValue("xuesejiaotang").c_str(), fontStr_kaiti, 21);
				nameLabel->setColor(ccc3(0xee, 0x9a, 0x00));
				dfDiscImg->addChild(nameLabel);
				nameLabel->setPosition(ccp(dfDiscImg->getContentSize().width/2, 450));
				//说明
				CCLabelTTF* DiscLabel = CCLabelTTF::create(LFStrings::getValue("xuesejiaotang_dsc").c_str(), fontStr_kaiti
					, dscLabel_size,CCSizeMake(126,0),kCCTextAlignmentLeft);
				DiscLabel->setAnchorPoint(ccp(0, 1));
				dfDiscImg->addChild(DiscLabel);
				DiscLabel->setPosition(ccp(42, 420));
				//时间
				tempStr.initWithFormat("%s:8%s-24%s", LFStrings::getValue("KaiQi").c_str(), 
					LFStrings::getValue("time_shi").c_str(), 
					LFStrings::getValue("time_shi").c_str());
				CCLabelTTF* shijianLabel = CCLabelTTF::create(tempStr.getCString(), fontStr_kaiti, 20);
				shijianLabel->setAnchorPoint(ccp(0.5,1));
				shijianLabel->setColor(ccc3(0,255,0));
				dfDiscImg->addChild(shijianLabel);
				shijianLabel->setPosition(ccp(dfDiscImg->getContentSize().width/2, 481-331));
				//剩余次数
				tempStr.initWithFormat("%s:%u", LFStrings::getValue("shengyucishu").c_str(),
					XueZhanManager::getManager()->getCanChlgNum());
				CCLabelTTF* cishuLabel = CCLabelTTF::create(tempStr.getCString(), fontStr_kaiti, 20);
				cishuLabel->setAnchorPoint(ccp(0.5,1));
				cishuLabel->setColor(ccc3(0,255,0));
				dfDiscImg->addChild(cishuLabel);
				cishuLabel->setPosition(ccp(dfDiscImg->getContentSize().width/2, 481-362));
			}
			if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstXueSeJiaoTangPlay))
			{
				GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
				CCPoint pos = xz->getPosition();
				pos.x += 15;
				pos.y += 115;
				guideLayer->init(xz->getContentSize(), pos
					, LFStrings::getValue(""), NULL, false);

				CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
			}
		}
		else if(idx == 2) // 巅峰对决
		{
			CCSprite* dfNor = CCSprite::create("zhandou_fuben_dianfengduijue.png");
			CCSprite* dfSel = CCSprite::create("zhandou_fuben_dianfengduijue_select.png");
			CCSprite* dfDis = CCSprite::create("zhandou_fuben_dianfengduijue_suoding.png");
			CCMenuItemSprite* df = CCMenuItemSprite::create(
				dfNor, dfSel, dfDis, this, menu_selector(NpcFuBenLayer::fubenMenuCallback));
			menu->addChild(df, 1, Tag_DianFeng);
			df->setPosition(ccp(size.width/2, size.height - df->getContentSize().height/2));
			//
			//0.3守城副本
			{
				CCSprite* dfDiscImg = CCSprite::create("zhandou_fuben_shuoming.png");
				cell->addChild(dfDiscImg, 1, Tag_Discribe_Img);
				dfDiscImg->setAnchorPoint(ccp(0.5,1));
				dfDiscImg->setPosition(ccp(size.width/2, df->getPositionY() - 90));
				//副本名
				CCLabelTTF* nameLabel = CCLabelTTF::create(LFStrings::getValue("dianfengduijue").c_str(), fontStr_kaiti, 21);
				nameLabel->setColor(ccc3(0xee, 0x9a, 0x00));
				dfDiscImg->addChild(nameLabel);
				nameLabel->setPosition(ccp(dfDiscImg->getContentSize().width/2, 450));
				//说明
				CCLabelTTF* DiscLabel = CCLabelTTF::create(LFStrings::getValue("dianfengduijue_dsc").c_str(), fontStr_kaiti
					, dscLabel_size,CCSizeMake(126,0),kCCTextAlignmentLeft);
				DiscLabel->setAnchorPoint(ccp(0, 1));
				//	jjDiscLabel->setColor(ccc3());
				dfDiscImg->addChild(DiscLabel);
				DiscLabel->setPosition(ccp(42, 420));
				//时间
				CCLabelTTF* XianzhiLabel = CCLabelTTF::create(LFStrings::getValue("fuben_dianfengduijue_time").c_str(), fontStr_kaiti, 20);
				XianzhiLabel->setAnchorPoint(ccp(0.5,1));
				XianzhiLabel->setColor(ccc3(0,255,0));
				dfDiscImg->addChild(XianzhiLabel);
				XianzhiLabel->setPosition(ccp(dfDiscImg->getContentSize().width/2, 162));
			}
		}
	}

	return cell;
}

CCSize NpcFuBenLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSize(204, 670);
}

unsigned int NpcFuBenLayer::numberOfCellsInTableView(CCTableView *table)
{
	return 3;
}