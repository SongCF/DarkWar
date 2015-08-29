#include "UnblockManager.h"
#include "BaseSprite.h"
#include "MyselfManager.h"
#include "TaskManager.h"
#include "font.h"
#include "GuideLayerBase.h"
#include "GameDirector.h"
#include "MainLayerTeamList.h"
#include "BackgroudLayer.h"
#include "GameScene.h"
#include "ServerTime.h"

#define Tag_UnblockWarn_Dialog 4643+1600


#define UnblockDialog_DelayTime 1.0f

UnblockManager* UnblockManager::s_mManager = NULL;

UnblockManager *UnblockManager::getManager()
{
	if (s_mManager == NULL)
	{
		s_mManager = new UnblockManager();
	}

	return s_mManager;
}

UnblockInfo UnblockManager::getBlockInfo(enumUnblockZone zone)
{
	UnblockInfo temp;
	temp.zone = zone;

	switch (zone)
	{
		//主页上的按钮
	case unblock_GongJiang:
		temp.type = unblockType_TaskProcess;
		temp.value1 = 5;//第四个任务通关开放工匠强化与回退、分解。
		break;
	case unblock_JiuGuan:
		temp.type = unblockType_Level;
		temp.value1 = 10;//酒馆10级开放
		break;
	case unblock_JiaoTang:
		///////////////////////////////////////////////////////////////
		break;
	case unblock_YingXiongBang:
		temp.type = unblockType_Level;
		temp.value1 = 6; //英雄榜6级开放
		break;
	case unblock_ZaHuoPu:
		//////////////////////////////////////////////////////////////////
		break;
	case unblock_TuJian:
		//////////////////////////////////////////////////////////////////
		break;

		//活动、副本、竞技
	case unblock_DiYuFuBen:
		//////////////////////////////////////////////////////////////////
		break;
	case unblock_ChuanQiLu:
		temp.type = unblockType_Level;
		temp.value1 = 5;
		break;
	case unblock_JingJiChang:
		temp.type = unblockType_Level;
		temp.value1 = 8;
		break;
	case unblock_TuanDuiFuBen:
		/////////////////////////////////////////////////////////////////
		break;
	case unblock_XueSeJiangTang:
		temp.type = unblockType_TaskProcess;
		temp.value1 = 10;
		break;
	case unblock_JiXianShouCheng:
		temp.type = unblockType_Level;
		temp.value1 = 15;
		break;
	case unblock_DianFengDuiJue:
		temp.type = unblockType_Level;
		temp.value1 = 25;
		break;
	case unblock_DiaoLuoChaXun:
		temp.type = unblockType_TaskProcess;
		temp.value1 = 6;
		break;

		//工匠系统
	case unblock_GongJiang_QiangHuaHuiTui://第四个任务通关开放工匠强化与回退、分解。
	case unblock_GongJiang_FenJie:
		temp.type = unblockType_TaskProcess;
		temp.value1 = 5;
		break;
	case unblock_GongJiang_BaoShiXiangQian:
		{
			temp.type = unblockType_Switch;
			if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GetFirstBaoShi) || GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GetFirstBaoShi))
			{
				
				temp.value1 = true;
			}
			else
			{
				temp.value1 = false;
			}
		}
		break;
	case unblock_GongJiang_BaoShiHeCheng:
		temp.type = unblockType_TaskProcess;
		temp.value1 = 15;
		break;
	case unblock_GongJiang_DuanZao:
		temp.type = unblockType_Level;
		temp.value1 = 30;
		break;
	case unblock_GongJiang_JingLianZhuanKe:
		temp.type = unblockType_Level;
		temp.value1 = 10;
		break;
	case unblock_GongJiang_CuiQuFuMo:
		////////////////////////////////////////////////////////////////
		break;
	default:
		CCAssert(false, "what's this?");
		break;
	}
	return temp;
}
bool UnblockManager::isDeblocking(enumUnblockZone cond)
{
	int level = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level;
	int lastTaskId = TaskManager::getManager()->getLastActivedMainTaskId();
	int taskIdState = TaskManager::getManager()->getLastActivedMainTaskStatus();
	tm* _tmData = ServerTime::getTime_tm();
	unsigned int curTime = _tmData->tm_hour*3600 + _tmData->tm_min*60 + _tmData->tm_sec;
	

	bool ret = false;
	UnblockInfo info = getBlockInfo(cond);
	switch (info.type)
	{
	case unblockType_Switch:
		if (info.value1 != 0) ret = true;
		break;
	case unblockType_Level:
		if (info.value1 <= level) ret = true;
		break;
	case unblockType_TaskProcess:
		if (info.value1 <= lastTaskId) ret = true;
		break;
	case unblockType_Time:
		if (info.value1 <= curTime && curTime <= info.value2) ret = true;
		break;
	default:
		CCAssert(false, "what's this?");
		break;
	}

	return ret;
}
bool UnblockManager::canOpen(enumUnblockZone cond)
{
	int level = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level;
	int lastTaskId = TaskManager::getManager()->getLastActivedMainTaskId();
	int taskIdState = TaskManager::getManager()->getLastActivedMainTaskStatus();
// 	tm* _tmData = ServerTime::getTime_tm();
// 	unsigned int curTime = _tmData->tm_hour*3600 + _tmData->tm_min*60 + _tmData->tm_sec;


	bool ret = false;
	UnblockInfo info = getBlockInfo(cond);
	switch (info.type)
	{
// 	case unblockType_Switch:
// 		if (info.value1 != 0) ret = true;
// 		break;
	case unblockType_Level:
		if (info.value1 == level) ret = true;
		break;
	case unblockType_TaskProcess:
		if (info.value1 == lastTaskId) ret = true;
		break;
// 	case unblockType_Time:
// 		if (info.value1 <= curTime && curTime <= info.value2) ret = true;
// 		break;
	default:
		CCAssert(false, "can not call this");
		break;
	}

	return ret;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UnblockWarnDialog* UnblockWarnDialog::create(string infoStr, bool nowOpen, bool canNotGo,bool isGuide)
{
	UnblockWarnDialog *layer = new UnblockWarnDialog(infoStr, nowOpen, canNotGo,isGuide);
	if(layer)
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


UnblockWarnDialog::UnblockWarnDialog(string infoStr, bool bOpen, bool canNotGo,bool isGuide)
{
	m_nTagGoTo = 0;
	CCLayerColor::initWithColor(ccc4(0,0,0,110));

	m_oTarger = NULL;
	mClicked = false;
	mOpen = bOpen;
	mCanNotGo = canNotGo;
	m_bIsGuide = isGuide;
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	int Label_Size = 23;


	BaseSprite* dialog;
	CCLabelTTF* label = CCLabelTTF::create(infoStr.c_str(), fontStr_kaiti, Label_Size, CCSize(340, 0), kCCTextAlignmentCenter);
	if(bOpen)
	{
		dialog = BaseSprite::create("jiaoxue_tanchukuang_jiesuo.png");
		dialog->addChild(label, 1, Tag_Label);
		label->setPosition(ccp(dialog->getContentSize().width/2, 48));
		//按键
		CCMenu* menu = CCMenu::create();
		dialog->addChild(menu);
		menu->setPosition(CCPointZero);
		{
			CCMenuItemImage* item = CCMenuItemImage::create(
				"xinguanqia_qianwang.png",
				"xinguanqia_qianwang_select.png",
				this, menu_selector(UnblockWarnDialog::menuItemClicked_Go));
			menu->addChild(item, 0);
			item->setPosition(ccp(dialog->getContentSize().width/2, label->getPositionY() - label->getContentSize().height/2
				- item->getContentSize().height/2 - 16));
			menu->setTouchPriority(-150);
		}
	}
	else 
	{
		dialog = BaseSprite::create("jiaoxue_tanchukuang_weijiesuo.png");
		dialog->addChild(label);
		label->setTag(Tag_Label);
		label->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
		//按键
		CCMenu* menu = CCMenu::create();
		dialog->addChild(menu);
		menu->setPosition(CCPointZero);
		{
			CCMenuItemImage* item = CCMenuItemImage::create(
				"jiaoxue_tanchukuang_weijiesuo.png",
				"jiaoxue_tanchukuang_weijiesuo.png",
				this, menu_selector(UnblockWarnDialog::menuItemClicked));
			menu->addChild(item, 0);
			item->setScaleX(winSize.width / item->getContentSize().width);
			item->setScaleY(winSize.height / item->getContentSize().height);
			item->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
			item->setOpacity(0);
			menu->setTouchPriority(-150);
		}
	}
	dialog->setPosition(ccp(winSize.width/2, winSize.height/2));
	dialog->setTag(Tag_UnblockWarn_Dialog);
	this->addChild(dialog);


	this->setTag(Tag_UnblockWarn_Dialog);
	this->setZOrder(110);

	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstXunBaoTeach))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstXunBaoTeach);
		m_nTagGoTo = 1;
	}
	else if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstYingXiongBang))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstYingXiongBang);
		m_nTagGoTo = 2;
	}
	else if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstXueSeJiaoTang))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstXueSeJiaoTang);
		m_nTagGoTo = 3;
	}
	else if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GongJiang_QhHt))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_QhHt);
		m_nTagGoTo = 4;
	}
}

void UnblockWarnDialog::setLabelDisplaySize(CCSize size)
{
	CCNode* pDlg = (CCNode*)this->getChildByTag(Tag_UnblockWarn_Dialog);
	if (pDlg)
	{
		CCLabelTTF* pLabel = (CCLabelTTF*)pDlg->getChildByTag(Tag_Label);
		if (pLabel)
		{
			pLabel->setDimensions(size);
		}
	}
}

void UnblockWarnDialog::menuItemClicked_Go(CCObject *pSender)
{

	if (m_nTagGoTo == 1)
	{
		//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstXunBaoTeach);
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}
	else if (m_nTagGoTo == 2)
	{
		//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstYingXiongBang);
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}
	else if (m_nTagGoTo == 3)
	{
		//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstYingXiongBang);
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}
	else if (m_nTagGoTo == 4)
	{
		//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstYingXiongBang);
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}
	if (g_gongjiang_QhHt_Opend) //工匠强化
	{
		g_gongjiang_QhHt_Opend = false;
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	}
	else if (g_ChuanQiFuBen_Opend) //传奇副本
	{
		GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	}
	else if (g_ChuanQiLu_Opend)  //传奇录
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	}
	else if (g_YingXiongBang_Opend)  //英雄榜
	{
		GameDirector::getDirector()->mainScene(mainlayer_YingXiongBang);
	}
	else if (g_JingJiChang_Opend) // 竞技场
	{
		//GameDirector::getDirector()->mainScene(mainlayer_JingJiChang_challenge);
		g_JingJiChang_Opend = false;
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	}
	else if (g_JiuGuan_Opend) // 酒馆
	{
		/*MainLayerBase* footer = new MainLayerBase();
		BackgroudLayer* bgLayer = new BackgroudLayer();
		MainLayerTeamList* layer = new MainLayerTeamList;
		footer->setContaintLayer(layer);
		GameScene* scene = GameScene::createScene();
		scene->addChild(footer,1);
		scene->addChild(bgLayer,-1);
		footer->release();
		bgLayer->release();
		layer->release();
		CCDirector::sharedDirector()->replaceScene(scene);*/
		g_JiuGuan_Opend = false;
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	}
	else if (g_XueSeJiaoTang_Opend || g_DianFengDuiJue_Opend) 
	{
		g_XueSeJiaoTang_Opend = false;
		
		g_DianFengDuiJue_Opend = false;
		//
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	}
	else if (g_JiXianShouCheng_Opend)
	{
		g_JiXianShouCheng_Opend = false;
		GameDirector::getDirector()->mainScene(mainlayer_HuoDong);
	}
	else 
	{
		this->removeFromParent();
		CCAssert(false, "error branch");
	}
}

void UnblockWarnDialog::menuItemClicked(CCObject *pSender)
{
	mClicked = true;

	displayEnd_RemoveThisDialog();
}

bool UnblockWarnDialog::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{	
	return true;
}

void UnblockWarnDialog::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if (mOpen && mCanNotGo)
	{
		CCPoint pos = pTouch->getLocation();
		CCRect rect = this->getChildByTag(Tag_UnblockWarn_Dialog)->boundingBox();
		if (! rect.containsPoint(pos))
		{
			this->getChildByTag(Tag_UnblockWarn_Dialog)->runAction(CCSequence::create(
				CCScaleTo::create(0.25f,0), CCRemoveSelf::create(), NULL));
			this->runAction(CCSequence::create(CCFadeTo::create(0.3f, 0), CCRemoveSelf::create(), NULL));

			if (g_XueSeJiaoTang_Opend || g_JiXianShouCheng_Opend || g_DianFengDuiJue_Opend) 
			{
				g_XueSeJiaoTang_Opend = false;
				g_JiXianShouCheng_Opend = false;
				g_DianFengDuiJue_Opend = false;
			}
		}
	}
}

void UnblockWarnDialog::onEnter()
{
	CCLayerColor::onEnter();
	//this->setZOrder(1024);
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -140, true);
	if (!m_bIsGuide)
	{
		if (! mOpen)
		{
			mHasDisplayTime = 0;
			schedule(schedule_selector(UnblockWarnDialog::updataTime), 0.5f);
		}

		if (mOpen && ! isUI_Guide_MainLine)
		{
			this->removeAllChildren();
			CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
		}
	}
	
}

void UnblockWarnDialog::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayerColor::onExit();
	
}

void UnblockWarnDialog::updataTime(float dt)
{
	mHasDisplayTime += 0.5f;

	if (mHasDisplayTime >= UnblockDialog_DelayTime)
	{
		unschedule(schedule_selector(UnblockWarnDialog::updataTime));
		displayEnd_RemoveThisDialog();
	}
}

void UnblockWarnDialog::displayEnd_RemoveThisDialog()
{
	if (mHasDisplayTime >= UnblockDialog_DelayTime || mClicked)
		this->getChildByTag(Tag_UnblockWarn_Dialog)->runAction(
		CCSequence::create(CCScaleTo::create(0.25f, 0.05f), CCHide::create(), 
		CCCallFunc::create(this, callfunc_selector(UnblockWarnDialog::callback_removeFromParent)), NULL));
}

void UnblockWarnDialog::callback_removeFromParent()
{
	this->removeFromParent();
	if (m_oTarger)
	{
		(m_oTarger->*m_hRemoveOverCallBack)();
	}
}