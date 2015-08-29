#include "MainFooterLayer.h"
#include "Music.h"
#include "MyselfManager.h"
#include "UnblockManager.h"
#include "SystemPushMessagePool.h"
#include "NetLabelTTF.h"
#include "NetTextListShowUI.h"
#include "font.h"
#include "ShenBingPool.h"
#include "CS_Common.h"
#include "FX_CommonFunc.h"
#include "ImageManager.h"
#include "WorldBossCityGuardManager.h"

MainFooterLayer::MainFooterLayer()
{
	
	mFootContainer = BaseSprite::create("zhuye_anniu_bg.png");
	addChild(mFootContainer);
	CCPoint originPos = CCDirector::sharedDirector()->getVisibleOrigin();
	mFootContainer->setPosition(ccp(mWinSize.width/2,originPos.y + mFootContainer->getContentSize().height/2));

	//menu
// 	{
// 		mMenuItemHome = CCMenuItemImage::create(
// 			"function_main.png",
// 			"function_main_select.png",
// 			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
// 		mMenuItemMaoxian = CCMenuItemImage::create(
// 			"function_adventure.png",
// 			"function_adventure_select.png",
// 			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
// 		mMenuItemZhuangbei = CCMenuItemImage::create(
// 			"function_equipment.png",
// 			"function_equipment_select.png",
// 			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
// 		mMenuItemJinji = CCMenuItemImage::create(
// 			"function_pk.png",
// 			"function_pk_select.png",
// 			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
// 		mMenuItemMengyou = CCMenuItemImage::create(
// 			"function_friend.png",
// 			"function_friend_select.png",
// 			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
// 		mMenuItemStore = CCMenuItemImage::create(
// 			"function_shop.png",
// 			"function_shop_select.png",
// 			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
// 		
// 
// 		mMenu = CCMenu::create(mMenuItemHome, mMenuItemMaoxian, mMenuItemZhuangbei
// 			, mMenuItemJinji,mMenuItemMengyou , mMenuItemStore, NULL);
// 		addChild(mMenu,1);
// 		mMenu->setPosition(CCPointZero);
// 
// 		mMenuItemHome->setPosition(ccp(45+2,50));
// 		mMenuItemHome->setTag(TAG_MENU_HOME);
// 
// 		mMenuItemMaoxian->setPosition(ccp(45+90+4,50));
// 		mMenuItemMaoxian->setTag(TAG_MENU_MAOXIAN);
// 
// 		mMenuItemZhuangbei->setPosition(ccp(45+90*2+6,50));
// 		mMenuItemZhuangbei->setTag(TAG_MENU_ZHUANGBEI);
// 
// 		mMenuItemJinji->setPosition(ccp(640-45-90*2-6,50));
// 		mMenuItemJinji->setTag(TAG_MENU_JINJI);
// 
// 		mMenuItemMengyou->setPosition(ccp(640-45-90-4,50));
// 		mMenuItemMengyou->setTag(TAG_MENU_MENGYOU);
// 
// 		mMenuItemStore->setPosition(ccp(640-45-2,50));
// 		mMenuItemStore->setTag(TAG_MENU_STORE);
// 	}

	{
		CCMenuItemImage *itemMaoxian = CCMenuItemImage::create(
			"zhuye_anniu_maoxian.png",
			"zhuye_anniu_maoxian_select.png",
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
		CCMenuItemImage *itemJingji = CCMenuItemImage::create(
			"zhuye_anniu_jingji.png",
			"zhuye_anniu_jingji_select.png",
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
		CCMenuItemImage *itemZhuye = CCMenuItemImage::create(
			"zhuye_anniu_zhuye_select.png",
			"zhuye_anniu_zhuye.png",
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
		CCMenuItemImage *itemHuodong = CCMenuItemImage::create(
			"zhuye_anniu_huodong.png",
			"zhuye_anniu_huodong_select.png",
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
		CCMenuItemImage *itemShangdian = CCMenuItemImage::create(
			"zhuye_anniu_shangdian.png",
			"zhuye_anniu_shangdian_select.png",
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));

		mMenu = CCMenu::create(itemMaoxian, itemShangdian, itemZhuye, itemJingji, itemHuodong, NULL);
		mFootContainer->addChild(mMenu,1);
		mMenu->setPosition(CCPointZero);

		itemZhuye->setPosition(ccp(320, 122-68));
		itemZhuye->setTag(TAG_MENU_HOME);

		itemJingji->setPosition(ccp(itemZhuye->getPositionX()-itemJingji->getContentSize().width/2-itemZhuye->getContentSize().width/2, 122-68));
		itemJingji->setTag(TAG_MENU_JINGJI);

		itemMaoxian->setPosition(ccp(itemJingji->getPositionX()-itemJingji->getContentSize().width/2-itemJingji->getContentSize().width/2, 122-68));
		itemMaoxian->setTag(TAG_MENU_MAOXIAN);

		itemHuodong->setPosition(ccp(itemZhuye->getPositionX()+itemZhuye->getContentSize().width/2+itemHuodong->getContentSize().width/2, 122-68));
		itemHuodong->setTag(TAG_MENU_HUODONG);

		itemShangdian->setPosition(ccp(itemHuodong->getPositionX()+itemHuodong->getContentSize().width/2+itemShangdian->getContentSize().width/2, 122-68));
		itemShangdian->setTag(TAG_MENU_STORE);

		
	}

	//初始化是打开第一个界面
	selectedIndex = mainlayer_ZhuYe;

	this->getNewMessageInfo(NULL);
	this->unschedule(schedule_selector(MainFooterLayer::updateMessage));
	this->schedule(schedule_selector(MainFooterLayer::updateMessage), 4);

	//显示竞技效果
	checkIsShowLiziAni(NULL);

	//检查极限守城活动是否开启
	this->unschedule(schedule_selector(MainFooterLayer::checkJixianshouchengIsOpen));
	this->schedule(schedule_selector(MainFooterLayer::checkJixianshouchengIsOpen), 1);
}

void MainFooterLayer::checkJixianshouchengIsOpen(float dt)
{
	bool bIsOpen = WorldBossCityGuardManager::getManager()->isOpening();

	CCMenuItemImage* pItem = (CCMenuItemImage*)mMenu->getChildByTag(TAG_MENU_HUODONG);
	if (pItem)
	{
		if (bIsOpen)
		{
			showLiziXiaoguo(pItem, TAG_Jixianshoucheng_lizi_Ani);
		}
		else
		{
			removeLiziXiaoguo(pItem, TAG_Jixianshoucheng_lizi_Ani);
		}
	}
}

void MainFooterLayer::showLiziXiaoguo(CCNode* pParent, int iTag)
{
	//极限守城20级开启，所以效果20级才显示
	//竞技8级开启，所以效果8级才显示
	int iMinLevel = 8;
	if (iTag == TAG_Jixianshoucheng_lizi_Ani)
	{
		iMinLevel = 20;
	}
	if (!pParent || MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < iMinLevel)
	{
		return;
	}

	CCNode* pNode = pParent->getChildByTag(iTag);
	if (pNode)
	{
		return;
	}

	CCParticleSystemQuad* par = CCParticleSystemQuad::create("jingji_xiaoguo.plist");
	pParent->addChild(par, 2, iTag);
	//par->setScale(0.5f);
	//par->setSpeed(10);
	par->setPosition(ccp(0, 0));

	CCPointArray *array = CCPointArray::create(20);

	array->addControlPoint(ccp(0, 0));
	array->addControlPoint(ccp(pParent->getContentSize().width, 0));
	array->addControlPoint(ccp(pParent->getContentSize().width, pParent->getContentSize().height));
	array->addControlPoint(ccp(0, pParent->getContentSize().height));

	float dt = 0.5f;
	CCMoveTo* action1 = CCMoveTo::create(dt, array->getControlPointAtIndex(1));
	CCMoveTo* action2 = CCMoveTo::create(dt, array->getControlPointAtIndex(2));
	CCMoveTo* action3 = CCMoveTo::create(dt, array->getControlPointAtIndex(3));
	CCMoveTo* action4 = CCMoveTo::create(dt, array->getControlPointAtIndex(0));

	CCSequence* quece = CCSequence::create(action1, action2, action3, action4, NULL);

	par->runAction(
		CCRepeatForever::create(
		//FXCircleBy::create(1.0f, ccp(pJingjiItem->getContentSize().width/2, pJingjiItem->getContentSize().height/2 + 5), 24, 360)
		quece
		)
		);
}

void MainFooterLayer::removeLiziXiaoguo(CCNode* pParent, int iTag)
{
	if (pParent)
	{
		CCNode* pNode = pParent->getChildByTag(iTag);
		if (pNode)
		{
			pNode->removeFromParent();
		}
	}
}

void MainFooterLayer::checkIsShowLiziAni(CCObject* pObj)
{
	ArenaTeamBasicInfo info;
	if (MyselfManager::getManager()->getArenaTeamBasicInfo(info))
	{
		int icount = info.challenge_remainder_time + info.free_fight_remainder_time;
		CCMenuItemImage* pJingjiItem = (CCMenuItemImage*)mMenu->getChildByTag(TAG_MENU_JINGJI);
		if (pJingjiItem)
		{
			if (icount <= 0)
			{
				removeLiziXiaoguo(pJingjiItem, TAG_Jingji_lizi_Ani);
			}
			else
			{
				showLiziXiaoguo(pJingjiItem, TAG_Jingji_lizi_Ani);
			}
		}
	}
}

void MainFooterLayer::updateMessage(float delta)
{
	this->getNewMessageInfo(NULL);
}

void MainFooterLayer::showMessageBar(MarqueeMessage info)
{
	//system message info barbg
	BaseSprite *messageBar = (BaseSprite*)this->getChildByTag(Tag_MessageInfoBar);
	if (!messageBar)
	{
		messageBar = BaseSprite::create("zhuye_xitongtonggao.png");
		this->addChild(messageBar, 100, Tag_MessageInfoBar);
		messageBar->setPosition(ccp(320, 1152-1039));
	}
		
	//context
	NetLabelTTF* pLable = (NetLabelTTF*)messageBar->getChildByTag(Tag_MessageTipStr);
	if (pLable)
	{
		return;
	}

	NetTextListShowUI* pText = NetTextListShowUI::create();
	//普通字符串
	string firstStr = info.subject;
	string secondStr = "";
	string thirdlyStr = "";
	string fullStr = "";
	string name = "";
	int	iCount = 0;
	ChuanQi chuanqiinfo;
	//传奇锻造
	if (info.type == MarqueeType_ForgeLegend)
	{
		secondStr = LFStrings::getValue("Paomadeng_DuanZao_Chuanqi");
		
		if (ChuanqiPool::getPool()->getChuanQiData(info.object.group_id, chuanqiinfo))
		{
			name = chuanqiinfo.allChuanqiFenShenInLifeSpan.at(0).zhuangbei_name;
		}
		iCount = info.count;
		thirdlyStr = CCString::createWithFormat("*%d", iCount)->getCString();
		fullStr = pText->getConversionMsgStr(LFStrings::getValue("Paomadeng_Wanjia"), ccWHITE)
				 +pText->getConversionMsgStr(firstStr, fonColor_FaGuang)
				 +pText->getConversionMsgStr(secondStr, ccWHITE)
				 +pText->getConversionMsgStr(name, ImageManager::getManager()->getShowColor(ZhuangbeiColour_Chuanqi, true))
				 +pText->getConversionMsgStr(thirdlyStr, ccWHITE);
	}
	//杂货铺
	else if (info.type == MarqueeType_VarietyStore)
	{
		secondStr = LFStrings::getValue("Paomadeng_Zahuopu");
		string name = CS::getItemName(info.object.item_type);
		iCount = info.count;
		thirdlyStr = CCString::createWithFormat("*%d", iCount)->getCString();
		fullStr = pText->getConversionMsgStr(LFStrings::getValue("Paomadeng_Wanjia"), ccWHITE)
			+pText->getConversionMsgStr(firstStr, fonColor_FaGuang)
			+pText->getConversionMsgStr(secondStr, ccWHITE)
			+pText->getConversionMsgStr(name, FX_CommonFunc::getCaiLiaoColor(info.object.item_type))
			+pText->getConversionMsgStr(thirdlyStr, ccWHITE);
	}
	//传奇合成
	else if (info.type == MarqueeType_Compound)
	{
		secondStr =  LFStrings::getValue("Paomadeng_Hecheng_Chuanqi");
		if (ChuanqiPool::getPool()->getChuanQiData(info.object.group_id, chuanqiinfo))
		{
			name = chuanqiinfo.allChuanqiFenShenInLifeSpan.at(0).zhuangbei_name;
		}
		iCount = info.count;
		thirdlyStr = CCString::createWithFormat("*%d", iCount)->getCString();
		fullStr = pText->getConversionMsgStr(LFStrings::getValue("Paomadeng_Wanjia"), ccWHITE)
			+pText->getConversionMsgStr(firstStr, fonColor_FaGuang)
			+pText->getConversionMsgStr(secondStr, ccWHITE)
			+pText->getConversionMsgStr(name, fonColor_PuTong)
			+pText->getConversionMsgStr(thirdlyStr, ccWHITE);
	}
	//巅峰对决
	else if (info.type == MarqueeType_TopArena)
	{
		//四强，八强
		if (info.object.round == 0)
		{
			secondStr =  LFStrings::getValue("Paomadeng_Dianfengduijue");
			fullStr = pText->getConversionMsgStr(LFStrings::getValue("Paomadeng_Wanjia"), ccWHITE)
				+pText->getConversionMsgStr(firstStr, fonColor_FaGuang)
				+pText->getConversionMsgStr(secondStr, ccWHITE);
		}
		//决赛
		else if (info.object.round == 1)
		{
			secondStr =  LFStrings::getValue("Paomadeng_Juesai");
			fullStr = pText->getConversionMsgStr(LFStrings::getValue("Paomadeng_Wanjia"), ccWHITE)
				+pText->getConversionMsgStr(firstStr, fonColor_FaGuang)
				+pText->getConversionMsgStr(secondStr, ccWHITE);
		}
		//冠军
		else if (info.object.round == 2)
		{
			secondStr = LFStrings::getValue("Paomadeng_Guanjun");
			fullStr = pText->getConversionMsgStr(LFStrings::getValue("Paomadeng_Wanjia"), ccWHITE)
				+pText->getConversionMsgStr(firstStr, fonColor_FaGuang)
				+pText->getConversionMsgStr(secondStr, ccWHITE);
		}
	}
	//英雄榜
	else if (info.type == MarqueeType_TopList)
	{

		if (info.object.top_order == 1)
		{
			firstStr = LFStrings::getValue("Paomadeng_Top1");
		}
		else if (info.object.top_order == 2)
		{
			firstStr = LFStrings::getValue("Paomadeng_Top2");
		}
		else if (info.object.top_order == 3)
		{
			firstStr = LFStrings::getValue("Paomadeng_Top3");
		}
		secondStr = info.subject;
		thirdlyStr = LFStrings::getValue("Paomadeng_Shangxian");

		fullStr = pText->getConversionMsgStr(firstStr, ccWHITE)
			+pText->getConversionMsgStr(secondStr, fonColor_FaGuang)
			+pText->getConversionMsgStr(thirdlyStr, ccWHITE);
	}
	//守护之灵
	else if (info.type == MarqueeType_ProtectSpirit)
	{
		firstStr = "";
//		secondStr = "MarqueeType_ProtectSpirit";
	}
	//寻宝 破外封印
	else if (info.type == MarqueeType_Discover_Destroy)
	{
		secondStr = LFStrings::getValue("Paomadeng_Xunbao_Pohuaifengyin");
		fullStr = pText->getConversionMsgStr(LFStrings::getValue("Paomadeng_Wanjia"), ccWHITE)
			+pText->getConversionMsgStr(firstStr, fonColor_FaGuang)
			+pText->getConversionMsgStr(secondStr, ccWHITE)
			+pText->getConversionMsgStr(thirdlyStr, ccWHITE);
	}
	//寻宝 解封装备
	else if (info.type == MarqueeType_Discover_Unlock)
	{
		secondStr = LFStrings::getValue("Paomadeng_Xunbao_Jiefeng");
		if (ChuanqiPool::getPool()->getChuanQiData(info.object.group_id, chuanqiinfo))
		{
			name = chuanqiinfo.allChuanqiFenShenInLifeSpan.at(0).zhuangbei_name;
		}
		iCount = info.count;
		thirdlyStr = CCString::createWithFormat("*%d", iCount)->getCString();
		fullStr = pText->getConversionMsgStr(LFStrings::getValue("Paomadeng_Wanjia"), ccWHITE)
			+pText->getConversionMsgStr(firstStr, fonColor_FaGuang)
			+pText->getConversionMsgStr(secondStr, ccWHITE)
			+pText->getConversionMsgStr(name, fonColor_PuTong)
			+pText->getConversionMsgStr(thirdlyStr, ccWHITE);
	}
	
	if (firstStr == "")
	{
		return;	
	}
	if (!pLable)
	{
		pText->setTextWidth(this->getContentSize().width*10);
		pText->setTextSize(20);
		fullStr = pText->getConversionMsgStr(LFStrings::getValue("Paomadeng_XiTong"), ccWHITE) + fullStr;
		pText->addMessage(fullStr);

		NetLabelTTF* pLable = NetLabelTTF::create(pText->getMsgString().c_str(), fontStr_kaiti, 20);
		pLable->setMsgColorTexCoords(pText->getMsgColorTexCoordsList());
		pLable->setHorizontalAlignment(kCCTextAlignmentLeft);
		pLable->setHeight(pText->getHeight());

		messageBar->addChild(pLable, 101, Tag_MessageTipStr);
		pLable->setAnchorPoint(ccp(0.0f, 0.5f));
		pLable->setPosition(ccp(messageBar->getContentSize().width, messageBar->getContentSize().height/2-7));

		float distance = messageBar->getContentSize().width+pLable->getContentSize().width;
		float speed = 100.0f;
		float dt = 4.0f;
		dt = distance/speed;

		pLable->runAction(CCSequence::create(
			CCMoveBy::create(dt, ccp(-distance, 0)),
			CCCallFuncN::create(this, callfuncN_selector(MainFooterLayer::messageMoveDone)),
			CCDelayTime::create(3.0f),
			CCCallFuncN::create(this, callfuncN_selector(MainFooterLayer::getNewMessageInfo)),
			NULL));
	}
}

void MainFooterLayer::messageMoveDone(CCNode* pInfoStr)
{
	BaseSprite *messageBar = (BaseSprite*)this->getChildByTag(Tag_MessageInfoBar);
	if (messageBar)
	{
// 		NetLabelTTF* pLable = (NetLabelTTF*)messageBar->getChildByTag(Tag_MessageTipStr);
// 		if (!pLable)
// 		{
			if (SystemPushMessagePool::getPool()->getCurMsgCount() < 1)
			{
				messageBar->removeFromParent();
			}
//		}
	}
}

void MainFooterLayer::getNewMessageInfo(CCNode* pInfoStr)
{
	if (pInfoStr)
	{
		pInfoStr->removeFromParent();
	}

	BaseSprite *messageBar = (BaseSprite*)this->getChildByTag(Tag_MessageInfoBar);
	if (messageBar)
	{
		NetLabelTTF* pLable = (NetLabelTTF*)messageBar->getChildByTag(Tag_MessageTipStr);
		if (!pLable)
		{
			if (SystemPushMessagePool::getPool()->getCurMsgCount() < 1)
			{
				messageBar->removeFromParent();
			}
			else
			{
				MarqueeMessage info;
				if (SystemPushMessagePool::getPool()->getOneMessage(info))
				{
					showMessageBar(info);
				}
			}
		}
	}
	else
	{
		MarqueeMessage info;
//		info.uid=186384394; 
// 		info.type = 5;
// 		info.count=1;
// 		sprintf(info.subject, "%s", "assdf");
		if (SystemPushMessagePool::getPool()->getOneMessage(info))
		{
			showMessageBar(info);
		}
	}
}

void MainFooterLayer::removeMessageBar(CCNode* pNode)
{
	pNode->getParent()->removeFromParent();
}

void MainFooterLayer::initAllMenuItemPic()
{
	
}

void MainFooterLayer::menuItemClicked(CCObject* item)
{
	int tag = ((CCNode*)item)->getTag();

	if(tag == TAG_MENU_HOME)
	{
		selectAtIndex(mainlayer_ZhuYe);
	}
	else if(tag == TAG_MENU_MAOXIAN)
	{
		selectAtIndex(mainlayer_MaoXian);
	}
	else if(tag == TAG_MENU_JINGJI)
	{
		//竞技场8级开放
		if (MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < 8)
		{
			CCString temp;
			temp.initWithFormat("%s8%s%s", LFStrings::getValue("ZhuJue").c_str()
				, LFStrings::getValue("ji").c_str(), LFStrings::getValue("KaiQi").c_str());
			UnblockWarnDialog *layer = UnblockWarnDialog::create(temp.getCString(), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
			return;
		}
		selectAtIndex(mainlayer_JingJiChang_challenge);
	}
	else if(tag == TAG_MENU_HUODONG)
	{
		selectAtIndex(mainlayer_HuoDong);
	}
	else if(tag == TAG_MENU_STORE)
	{
		selectAtIndex(mainlayer_ShangDian);
	}
}


CCSize MainFooterLayer::getFooterSize()
{
	//return mFootContainer->getContentSize();
	return CCSizeMake(mFootContainer->getContentSize().width,mFootContainer->getContentSize().height - 16);
}

void MainFooterLayer::selectAtIndex(enum_MainLayerIndex index,bool reSelect /* = false */)
{
	//sound
	Music::sharedMusicPlayer()->buttonClicked();

	selectedIndex = index;

	CCMenuItemImage *itemMaoxian = (CCMenuItemImage*)mMenu->getChildByTag(TAG_MENU_MAOXIAN);
	CCMenuItemImage *itemJingji = (CCMenuItemImage*)mMenu->getChildByTag(TAG_MENU_JINGJI);
	CCMenuItemImage *itemZhuye = (CCMenuItemImage*)mMenu->getChildByTag(TAG_MENU_HOME);
	CCMenuItemImage *itemHuodong = (CCMenuItemImage*)mMenu->getChildByTag(TAG_MENU_HUODONG);
	CCMenuItemImage *itemShangdian = (CCMenuItemImage*)mMenu->getChildByTag(TAG_MENU_STORE);
	//初始化所有按钮的图片
	{
		itemZhuye->initWithNormalImage(
			"zhuye_anniu_zhuye.png",
			"zhuye_anniu_zhuye_select.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));

		itemMaoxian->initWithNormalImage(
			"zhuye_anniu_maoxian.png",
			"zhuye_anniu_maoxian_select.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));

		itemJingji->initWithNormalImage(
			"zhuye_anniu_jingji.png",
			"zhuye_anniu_jingji_select.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));

		itemHuodong->initWithNormalImage(
			"zhuye_anniu_huodong.png",
			"zhuye_anniu_huodong_select.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));

		itemShangdian->initWithNormalImage(
			"zhuye_anniu_shangdian.png",
			"zhuye_anniu_shangdian_select.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
	}

	if(index == mainlayer_MaoXian)
	{
		itemMaoxian->initWithNormalImage(
			"zhuye_anniu_maoxian_select.png",
			"zhuye_anniu_maoxian.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
	}
	else if(index == mainlayer_JingJiChang_challenge || index == mainlayer_JingJiChang_random)
	{
		itemJingji->initWithNormalImage(
			"zhuye_anniu_jingji_select.png",
			"zhuye_anniu_jingji.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
	}
	else if(index == mainlayer_HuoDong)
	{
		itemHuodong->initWithNormalImage(
			"zhuye_anniu_huodong_select.png",
			"zhuye_anniu_huodong.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
	}
	else if(index == mainlayer_ShangDian || index == mainlayer_ChongZhi)
	{
		itemShangdian->initWithNormalImage(
			"zhuye_anniu_shangdian_select.png",
			"zhuye_anniu_shangdian.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
	}
	else 
	{
		itemZhuye->initWithNormalImage(
			"zhuye_anniu_zhuye_select.png",
			"zhuye_anniu_zhuye.png",
			NULL,
			this,SEL_MenuHandler(&MainFooterLayer::menuItemClicked));
	}

	mCallback->footerItemSelected(index);
}

void MainFooterLayer::setCallBack(FooterCallback* callback)
{
	mCallback = callback;
	if (mCallback)
	{
//		mCallback->footerItemSelected(selectedIndex);
	}
}

MainFooterLayer::~MainFooterLayer()
{

}



//#define TAG_TABLE_ITEM 1112
//
//// 元素的图片
//static string itemFiles[] = {
//	"function_main.png",
//	"function_adventure.png",
//	"function_equipment.png",
//	"function_pk.png",
//	"function_friend.png",
//	"function_shop.png"
//};
//static string itemFiles_select[] = {
//	"function_main_select.png",
//	"function_adventure_select.png",
//	"function_equipment_select.png",
//	"function_pk_select.png",
//	"function_friend_select.png",
//	"function_shop_select.png"
//};
//
//MainFooterLayer::MainFooterLayer()
//{
//	mCallback = NULL;
//	BaseSprite* container = BaseSprite::create("function_frame.png");
//	addChild(container,-1);
//	CCPoint originPos = CCDirector::sharedDirector()->getVisibleOrigin();
//	container->setPosition(ccp(mWinSize.width/2,originPos.y + container->getContentSize().height/2));
//	selectedIndex = -1;
//
//	tableView = CCTableView::create(this, CCSizeMake(container->getContentSize().width, container->getContentSize().height));
//	tableView->setDirection(kCCScrollViewDirectionHorizontal);
//	tableView->setPosition(ccp(2,6));
//	tableView->setDelegate(this);
//	container->addChild(tableView);
//	// 禁用滑动
//	tableView->setBounceable(false);
//	tableView->reloadData();
//
//	selectAtIndex(0);
//}
//
//void MainFooterLayer::selectAtIndex(unsigned int index,bool reSelect /* = false */)
//{
//	CCTableViewCell* currentCell = tableView->cellAtIndex(index);
//	CCTableViewCell* lastCell = tableView->cellAtIndex(selectedIndex);
//
//	if (selectedIndex != index)
//	{
//		if (lastCell!=NULL)
//		{
//			BaseSprite* itemLast = (BaseSprite*)lastCell->getChildByTag(TAG_TABLE_ITEM);
//			itemLast->initWithFile(itemFiles[lastCell->getIdx()]);
//			itemLast->setAnchorPoint(CCPointZero);
//		}
//
//		if (currentCell!=NULL)
//		{
//			BaseSprite* itemNow = (BaseSprite*)currentCell->getChildByTag(TAG_TABLE_ITEM);
//			itemNow->initWithFile(itemFiles_select[currentCell->getIdx()]);
//			itemNow->setAnchorPoint(CCPointZero);
//		}
//		selectedIndex = index;
//
//		if (mCallback)
//		{
//			mCallback->footerItemSelected(selectedIndex);
//		}
//	}else
//	{
//		if (reSelect)
//		{
//			if (mCallback)
//			{
//				mCallback->footerItemSelected(selectedIndex);
//			}
//		}
//	}
//}
//
//CCSize MainFooterLayer::getFooterSize()
//{
//	return tableView->getViewSize();
//}
//
//void MainFooterLayer::setCallBack(FooterCallback* callback)
//{
//	mCallback = callback;
//	if (mCallback)
//	{
//		mCallback->footerItemSelected(selectedIndex);
//	}
//}
//
//void MainFooterLayer::scrollViewDidScroll(CCScrollView* view) 
//{
//}
//void MainFooterLayer::scrollViewDidZoom(CCScrollView* view) 
//{
//}
//
//void MainFooterLayer::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
//{
//	selectAtIndex(cell->getIdx());
//	Music::sharedMusicPlayer()->buttonClicked();
//}
//
//void MainFooterLayer::tableCellHighlight(CCTableView* table, CCTableViewCell* cell)
//{
//	if (selectedIndex!=cell->getIdx())
//	{
//		BaseSprite* item = (BaseSprite*)cell->getChildByTag(TAG_TABLE_ITEM);
//		item->initWithFile(itemFiles_select[cell->getIdx()]);
//		item->setAnchorPoint(CCPointZero);
//	}
//}
//
//void MainFooterLayer::tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell)
//{
//	if (selectedIndex!=cell->getIdx())
//	{
//		BaseSprite* item = (BaseSprite*)cell->getChildByTag(TAG_TABLE_ITEM);
//		item->initWithFile(itemFiles[cell->getIdx()]);
//		item->setAnchorPoint(CCPointZero);
//	}
//}
//
//CCSize MainFooterLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
//{
//	//CCLog("MainLayerFooter::tableCellSizeForIndex --> index = [%d]",idx);
//	if (idx == 2) {
//		return CCSizeMake(170, 86);
//	}
//	return CCSizeMake(94, 86);
//}
//
//CCTableViewCell* MainFooterLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
//{
//	CCString *string = CCString::createWithFormat("%d", idx);
//	CCTableViewCell *cell = table->dequeueCell();
//	if (cell == NULL) {
//		cell = new CCTableViewCell();
//		cell->autorelease();
//
//		BaseSprite *sprite = BaseSprite::create(itemFiles[idx]);
//		//CCLog("MainLayerZhuangBeiBody::tableCellAtIndex new --> index = [%d],file = [%s]",idx,itemFiles[idx].c_str());
//		sprite->setAnchorPoint(CCPointZero);
//		sprite->setPosition(ccp(0, 0));
//		cell->addChild(sprite,0,TAG_TABLE_ITEM);
//
//		//CCLabelTTF *label = CCLabelTTF::create(string->getCString(), "Helvetica", 20.0);
//		//label->setPosition(CCPointZero);
//		//label->setAnchorPoint(CCPointZero);
//		//label->setTag(123);
//		//cell->addChild(label);
//	}
//	else
//	{
//		//CCLog("MainLayerZhuangBeiBody::tableCellAtIndex new --> index = [%d],file = [%s]",idx,itemFiles[idx].c_str());
//		BaseSprite* temp = (BaseSprite*)cell->getChildByTag(TAG_TABLE_ITEM);
//		if (selectedIndex==idx)
//		{
//			temp->initWithFile(itemFiles_select[idx].c_str());
//		}else
//		{
//			temp->initWithFile(itemFiles[idx].c_str());
//		}
//		temp->setAnchorPoint(CCPointZero);
//
//		/*CCLabelTTF *label = (CCLabelTTF*)cell->getChildByTag(123);
//		label->setString(string->getCString());*/
//	}
//
//	return cell;
//}
//
//unsigned int MainFooterLayer::numberOfCellsInTableView(CCTableView *table)
//{
//	// 第一步
//	return sizeof(itemFiles)/sizeof(string);
//}
//
//MainFooterLayer::~MainFooterLayer()
//{
//
//}
//
