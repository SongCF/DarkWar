#include "GongJiangBase.h"
#include "GongJiangQianghua.h"
#include "GongJiangFenJie.h"
//#include "GongJiangCuiQuFuMo.h"
//#include "GongJiangJingLianZhuanKe.h"
#include "GongJiangBaoShiHeCheng.h"
#include "GongJiangBaoShiXiangQian.h"
//#include "GongJiangDuanZao.h"
#include "TaskManager.h"
#include "MyselfManager.h"
#include "GuideLayerBase.h"
#include "History.h"
#include "UnblockManager.h"
#include "ZhuShouChatLayer.h"

#define STR_MenuFile_BaoShiHeCheng_Normal "gongjiang_zhuye_anniu_baoshihecheng.png"
#define STR_MenuFile_BaoShiHeCheng_Select "gongjiang_zhuye_anniu_baoshihecheng_select.png"
#define STR_MenuFile_BaoShiHeCheng_Disable "gongjiang_zhuye_anniu_baoshihecheng_suoding.png"

#define STR_MenuFile_BaoShiXiangQian_Normal "gongjiang_zhuye_anniu_baoshixiangqian.png"
#define STR_MenuFile_BaoShiXiangQian_Select "gongjiang_zhuye_anniu_baoshixiangqian_select.png"
#define STR_MenuFile_BaoShiXiangQian_Disable "gongjiang_zhuye_anniu_baoshixiangqian_suoding.png"

// #define STR_MenuFile_CuiQiuFuMo_Normal "gongjiang_zhuye_anniu_cuiqufumo.png"
// #define STR_MenuFile_CuiQiuFuMo_Select "gongjiang_zhuye_anniu_cuiqufumo_select.png"
// #define STR_MenuFile_CuiQiuFuMo_Disable "gongjiang_zhuye_anniu_cuiqufumo_suoding.png"

// #define STR_MenuFile_JingLianZhuanKe_Normal "gongjiang_zhuye_anniu_jinglianzhuanke.png"
// #define STR_MenuFile_JingLianZhuanKe_Select "gongjiang_zhuye_anniu_jinglianzhuanke_select.png"
// #define STR_MenuFile_JingLianZhuanKe_Disable "gongjiang_zhuye_anniu_jinglianzhuanke_suoding.png"

// #define STR_MenuFile_ZhuangBeiDuanZao_Normal "gongjiang_zhuye_anniu_zhuangbeiduanzao.png"
// #define STR_MenuFile_ZhuangBeiDuanZao_Select "gongjiang_zhuye_anniu_zhuangbeiduanzao_select.png"
// #define STR_MenuFile_ZhuangBeiDuanZao_Disable "gongjiang_zhuye_anniu_zhuangbeiduanzao_suoding.png"

#define STR_MenuFile_ZhuangBeiFenJie_Normal "gongjiang_zhuye_anniu_zhuangbeifenjie.png"
#define STR_MenuFile_ZhuangBeiFenJie_Select "gongjiang_zhuye_anniu_zhuangbeifenjie_select.png"
#define STR_MenuFile_ZhuangBeiFenJie_Disable "gongjiang_zhuye_anniu_zhuangbeifenjie_suoding.png"

#define STR_MenuFile_ZhuangBeiQiangHua_Normal "gongjiang_zhuye_anniu_zhuangbeiqianghua.png"
#define STR_MenuFile_ZhuangBeiQiangHua_Select "gongjiang_zhuye_anniu_zhuangbeiqianghua_select.png"
#define STR_MenuFile_ZhuangBeiQiangHua_Disable "gongjiang_zhuye_anniu_zhuangbeiqianghua_suoding.png"

GongJiangBase * GongJiangBase::create(int flag)
{
	GongJiangBase * layer = new GongJiangBase;
	if(layer && layer->init(flag))
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

bool GongJiangBase::init(int flag)
{
	mGoToFlag = flag;
	bool bRet = false;

	int lastTaskId = TaskManager::getManager()->getLastActivedMainTaskId();
	int level = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level;

	do 
	{
		mBg = BaseSprite::create("gongjiang_zhuye_bg.png");
		addChild(mBg);
		mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

		mMainMenu = CCMenu::create();
		mBg->addChild(mMainMenu);
		mMainMenu->setPosition(CCPointZero);

//		CCMenuItemImage *itemBaoShiHeCheng, *itemBaoShiXiangQian, *itemJingLianZhuanKe, *itemZhuangBeiDuanZao, *itemZhuangBeiFenJie, *itemZhuangBeiQiangHua;
		CCMenuItemImage *itemBaoShiHeCheng = CCMenuItemImage::create(
			STR_MenuFile_BaoShiHeCheng_Normal
			, STR_MenuFile_BaoShiHeCheng_Select
			, STR_MenuFile_BaoShiHeCheng_Disable
			, this, menu_selector(GongJiangBase::menuItemClicked));
		mMainMenu->addChild(itemBaoShiHeCheng, 1, flag_BaoShi_HeCheng);
		itemBaoShiHeCheng->setPosition(ccp(427, 838-537));
		itemBaoShiHeCheng->setEnabled(false);
		//解锁否
		{
			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_GongJiang_BaoShiHeCheng);

// 			{
// 				//1.    2.正在教学
// 				if(g_gongjiang_BsHc_Opend)
// 					bUnblock = true;
// 				//3.  刚完成宝石合成的教学、任务还没提交    g_gongjiang_BsHc_Opend已是false    但是History里面标记量已有、且为false
// 				bool isFirstBsHc = true;
// 				if(History::getHistory()->getBoolFromCurrentHero(Key_FirstGongJiang_BsHc, isFirstBsHc))
// 					if(! isFirstBsHc)
// 						bUnblock = true;
// 			}

			if (! bUnblock)
				itemBaoShiHeCheng->setEnabled(false);
		}

		CCMenuItemImage *itemBaoShiXiangQian = CCMenuItemImage::create(
			STR_MenuFile_BaoShiXiangQian_Normal
			, STR_MenuFile_BaoShiXiangQian_Select
			, STR_MenuFile_BaoShiXiangQian_Disable
			, this, menu_selector(GongJiangBase::menuItemClicked));
		mMainMenu->addChild(itemBaoShiXiangQian, 1, flag_BaoShi_XiangQian);
		itemBaoShiXiangQian->setPosition(ccp(213, 838-537));
		//解锁否
		{
			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_GongJiang_BaoShiXiangQian);

// 			{
// 				//1.     2.正在教学
// 				if(g_gongjiang_BsXq_Opend)
// 					bUnblock = true;
// 				//3.  刚完成教学、任务还没提交    g_gongjiang_BsHc_Opend已是false    但是History里面标记量已有、且为false
// 				bool isFirstBsXq = true;
// 				if(History::getHistory()->getBoolFromCurrentHero(Key_FirstGongJiang_BsXq, isFirstBsXq))
// 					if(! isFirstBsXq)
// 						bUnblock = true;
// 			}

			if (! bUnblock)
				itemBaoShiXiangQian->setEnabled(false);
		}

/*		itemJingLianZhuanKe = CCMenuItemImage::create(
			STR_MenuFile_JingLianZhuanKe_Normal
			, STR_MenuFile_JingLianZhuanKe_Select
			, STR_MenuFile_JingLianZhuanKe_Disable
			, this, menu_selector(GongJiangBase::menuItemClicked));
		mMainMenu->addChild(itemJingLianZhuanKe, 1, flag_JingLian_ZhuanKe);
		itemJingLianZhuanKe->setPosition(ccp(229, 838-581));
		//解锁否
		{
			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_GongJiang_JingLianZhuanKe);
			if (! bUnblock)
				itemJingLianZhuanKe->setEnabled(false);
		}

		itemZhuangBeiDuanZao = CCMenuItemImage::create(
			STR_MenuFile_ZhuangBeiDuanZao_Normal
			, STR_MenuFile_ZhuangBeiDuanZao_Select
			, STR_MenuFile_ZhuangBeiDuanZao_Disable
			, this, menu_selector(GongJiangBase::menuItemClicked));
		mMainMenu->addChild(itemZhuangBeiDuanZao, 1, flag_ZhuangBei_DuanZao);
		itemZhuangBeiDuanZao->setPosition(ccp(409, 838-581));
		//解锁否
		{
			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_GongJiang_DuanZao);
			if (! bUnblock)
				itemZhuangBeiDuanZao->setEnabled(false);
		}
*/
		CCMenuItemImage *itemZhuangBeiFenJie = CCMenuItemImage::create(
			STR_MenuFile_ZhuangBeiFenJie_Normal
			, STR_MenuFile_ZhuangBeiFenJie_Select
			, STR_MenuFile_ZhuangBeiFenJie_Disable
			, this, menu_selector(GongJiangBase::menuItemClicked));
		mMainMenu->addChild(itemZhuangBeiFenJie, 1, flag_ZhuangBei_FenJie);
		itemZhuangBeiFenJie->setPosition(ccp(427, 838-301));
		//解锁否
		{
			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_GongJiang_FenJie);
			
			{
				if(g_gongjiang_QhHt_Opend)
					bUnblock = true;
				//3.强化  --- 教学完、还没提交任务   此时 g_gongjiang_QhHt_Opend为false，但History标记量已经有了、且为false
				bool isFirstQiangHua = true;
				if(History::getHistory()->getBoolFromCurrentHero(Key_FirstGongJiang_QiangHua, isFirstQiangHua))
					if(! isFirstQiangHua)
						bUnblock = true;
			}

			if (! bUnblock)
				itemZhuangBeiFenJie->setEnabled(false);
		}

		CCMenuItemImage *itemZhuangBeiQiangHua = CCMenuItemImage::create(
			STR_MenuFile_ZhuangBeiQiangHua_Normal
			, STR_MenuFile_ZhuangBeiQiangHua_Select
			, STR_MenuFile_ZhuangBeiQiangHua_Disable
			, this, menu_selector(GongJiangBase::menuItemClicked));
		mMainMenu->addChild(itemZhuangBeiQiangHua, 1, flag_ZhuangBei_Qianghua);
		itemZhuangBeiQiangHua->setPosition(ccp(213, 838-301));
		//解锁否
		{
			bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_GongJiang_QiangHuaHuiTui);
			
			{
				//1.     2.正在教学
				if(g_gongjiang_QhHt_Opend)
					bUnblock = true;
				//3.刚教学完、还没提交任务   此时 g_gongjiang_QhHt_Opend为false，但History标记量已经有了、且为false
				bool isFirstQiangHua = true;
				if(History::getHistory()->getBoolFromCurrentHero(Key_FirstGongJiang_QiangHua, isFirstQiangHua))
					if(! isFirstQiangHua)
						bUnblock = true;
			}

			if (! bUnblock)
				itemZhuangBeiQiangHua->setEnabled(false);
		}


		if (mGoToFlag != 0)
		{
			scheduleOnce(schedule_selector(GongJiangBase::GoToSubGongJiang), 0.005f);
		}



		bRet = true;
	} while (0);

	return bRet;
}

void GongJiangBase::GoToSubGongJiang(float dt)
{
	menuItemClicked(mMainMenu->getChildByTag(mGoToFlag));
}

void GongJiangBase::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
}

void GongJiangBase::disableAllTouchEnd()
{
	mMainMenu->setEnabled(true);
}

void GongJiangBase::menuItemClicked(CCObject* pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	BaseLayer* subLayer = NULL;

	switch (tag)
	{
	case flag_BaoShi_HeCheng:
		
		//subLayer = GongJiangBaoShiHeCheng::create();
		{
		string bshcunblockmsg=LFStrings::getValue("jinrushibai")+LFStrings::getValue("BanBen_ZhanWeiKaiQi");
        UnblockWarnDialog *dialog = UnblockWarnDialog::create( bshcunblockmsg, false);
        CCDirector::sharedDirector()->getRunningScene()->addChild(dialog);
		return;
		}
		break;
	case flag_BaoShi_XiangQian:
		subLayer = GongJiangBaoShiXiangQian::create();
		break;
//	case flag_CuiQu_FuMo:
//		subLayer = GongJiangCuiQuFuMo::create();
//		break;
// 	case flag_JingLian_ZhuanKe:
// 		subLayer = GongJiangJingLianZhuanKe::create();
// 		break;
// 	case flag_ZhuangBei_DuanZao:
// 		subLayer = GongjiangDuanzhao::create();
// 		break;
	case flag_ZhuangBei_FenJie:
		subLayer = GongJiangFenJie::create();
		break;
	case flag_ZhuangBei_Qianghua:
		subLayer = GongJiangQianghua::create();
		break;
	default:
		CCLog("Error: MainLayerGongJiangBase::menuItemClicked ---> item tag = [%d]", tag);
		break;
	}

	mFooterLayer->popUpContaintLayer(subLayer);
}

void GongJiangBase::onEnter()
{
	BaseLayer::onEnter();

	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GongJiang_QiangHua))
	{
		//GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_QhHt);
		//if (GameGuideManager::getManager()->getCurGuideState() == 3)
		{
			vector<ShowMsg> list;
			ShowMsg msg1;
			msg1.msg = LFStrings::getValue("FirstQiangHuaShuoMing");
			msg1.color = ccWHITE;
			list.push_back(msg1);
			ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
				list, 2,this);
			if(layer != NULL)
			{

				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
			}
			return;
		}
	}
// 	else if (isUI_Guide_GongJiang && g_gongjiang_BsXq_Opend)
// 	{
// 		History::getHistory()->putInCurrentHero(Key_FirstGongJiang_BsXq, false);
// 		g_gongjiang_BsXq_Opend = false; //clear   只用带到宝石镶嵌界面   不用强制玩家选装备、选宝石
// 
// 		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
// 		CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(flag_BaoShi_XiangQian);
// 		guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
// 			, LFStrings::getValue(""), NULL, false);
// 
// 		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
// 	}
// 	else if (isUI_Guide_GongJiang && g_gongjiang_BsHc_Opend)
// 	{
// 		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
// 		CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(flag_BaoShi_HeCheng);
// 		guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
// 			, LFStrings::getValue(""), NULL, false);
// 
// 		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
// 	}
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_FenJie))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_FenJie);
		//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_FenJie);
		//g_gongjiang_FenJie_Guide = false; //clear

		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(flag_ZhuangBei_FenJie);
		guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue("FenJieHuoDeCaiLiao"), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		return;
	}
	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GetFirstBaoShi))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GetFirstBaoShi);
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(flag_BaoShi_XiangQian);
		guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue(""), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		return;
	}
}
 void GongJiangBase::ZhuShouchatEndCallBack()
 {
	 //GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_QiangHua);
	 //GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_QhHt);
	 GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
	 CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(flag_ZhuangBei_Qianghua);
	 guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
		 , LFStrings::getValue(""), NULL, false);

	 CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
 }