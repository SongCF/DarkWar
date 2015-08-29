#include "MapsLayer.h"
//#include "ChooseFightBody.h"
#include "JiuGongGeLayer.h"
#include "GameDirector.h"
#include "MapsManager.h"
#include "JGG_FootprintManager.h"
#include "LFAlert.h"
#include "WuPinManager.h"
#include "font.h"
#include "ImageManager.h"
#include "MyselfManager.h"
#include "GetTiLiLayer.h"
#include "ChatContentManager.h"
#include "MapsLayer_DiaoLuoChaXun.h"
#include "FightPackage.h"
#include "MessageDisplay.h"
#include "UnblockManager.h"
#include "History.h"
#include "GuideLayerBase.h"
#include "CS_Common.h"
#include "ClientFightLayer.h"
#include "ChessBoardStateManager.h"
#include "ShenBingPool.h"
#include "SuitPool.h"
#include "FX_CommonFunc.h"
#include "ShouHuZhiLingManager.h"
#include "AwardDisplay.h"
#include "BaseActorPropertyParser.h"
#include "ZhuangBeiMianBanOther.h"
#include "ClientErrorCode.h"
#include "FXToast.h"
#include "GiftSysDisplay.h"

using namespace FX_CommonFunc;

#define TouchDelta_X 180
#define CanNotTouch_X 50



#define Label_Size_Min 20
#define Label_Size_Max 24
#define Label_Color_DanHuang ccc3(195, 145, 96)

// 封装在MapsManager里面   getCurrentFight TaskId/GridId
// mapsLayer 、 JiuGongGeLayer 、 FightSetlementLayer
// int  g_taskId = -1;  // 当前战斗的id
// int  g_gridIndex = -1;

int MapsLayer::mCurZhangjie = 0;   //刚登陆地图、下次进来  自动普配
int MapsLayer::mCurDifficulty = difficult_normal;  //必须先normal  用于保存translation Maintask


//任务按钮的显示与不显示 是通过 mainId和mainState来确定的，那么地狱副本 更改curDifficulty之后 再更改前面两个变量
//即可实现 之前 普通副本 的复用。  ---> resetZhuangJie_AddUI  即可刷新整个ui    (g_task 在fightselttlementLayer有用到)

// g_taskId 是多少就是多少
// mMainId mMainState都是将地狱副本的id - 100 ， 
// drawRenWu -> menuTag id  也不区分 normal和hard

enum 
{
	Tag_HeadMenu_Normal = 1,
	Tag_HeadMenu_Hard,

//	TAG_XinRenWu_QiZhi = 200,
	TAG_QiZhi = 40,
	TAG_MapName,
	Tag_TaskMenu    = 50,

	//npc board
	Tag_NpcBianhui_Layer = 60,
	Tag_Npc_Window    ,
	Tag_Npc_Menu      ,
	Tag_NpcMenuItem_ZBDiaoLuo ,
	Tag_NpcMenuItem_KaiShi    ,
	Tag_LingJiang_Item_RenWu = 70,
	Tag_LingJiang_Item_ZhangJie = 75,
	Tag_LingJiang_Item_FanHui,
	Tag_ShouHuZhiLingBoard,
	Tag_ShouHuZhiLing_LingJiang,
	Tag_ShouHuZhiLing_TiaoZhan,
	Tag_ShouHuZhiLing_MoBai,
	Tag_ShouHuZhiLing_GuiZe,
	Tag_ShouHuZhiLing_GuiZeBoard,
	Tag_ShouHuZhiLing_GuiZeFanHui,
	Tag_ShouHuZhiLing_HeroImg,
	Tag_SaoDang_Item  ,
	Tag_NpcAwardBoard_,   // 1-3
	Tag_Lingjiang_Wacheng,

	Tag_MainMenuItem_TongGuan  = 100 , //通关
	Tag_MainMenuItem_ShouHuZhiLing, //守护之灵

	//saodang board
	Tag_SaoDang_Window = 120,
	Tag_ItemSaoDang_numAdd   ,
	Tag_ItemSaoDang_numSub   ,
	Tag_ItemSaoDang_BaiZhuang,
	Tag_ItemSaoDang_LanZhuang,
	Tag_ItemSaoDang_HuangZhuang,
	Tag_ItemSaoDang_Begin      ,
	Tag_ItemSaoDang_Close      ,
	Tag_Label_SaoDang_Num     ,
	Tag_Label_SaoDang_TiLi    ,
	Tag_Label_SaoDangCount    ,
	Tag_ItemSaoDang_BeiBao    ,
	//掉落查询
	Tag_DiaoLuo_Window =200,
//	Tag_DiaoLuo_Menu,
	Tag_DiaoLuoItem_FanHui,
	Tag_Board_EquipDetail,
	Tag_Menu_EquipDetail,  //装备详情的menu
	Tag_MenuItem_EquipDetail_Close,
	Tag_MenuItem_TaoZhuang,
	Tag_Board_TaoZhuangShuxing,

	//是否有通关奖励可领取
	Tag_TongguanjiangliInfo,

	//章节board  title bar
	Tag_Label_CurMapName,
	Tag_CircleImg_World,
	Tag_CircleImg_Captrue,
};

extern  void addLabelToNode_AncPosColorSize(CCNode *node, CCString str, string font, int size, CCPoint anc, CCPoint pos, ccColor3B color);

MapsLayer::MapsLayer(GoTo_UI_Type ui_type /* = Default_UI_Type*/)
	:m_GoTo_UI_Type(ui_type)
	,m_iWillGoToLevel(1)
	,m_bClickTongguanBtn(false)
{
	m_bIsTeachIng = false;
	mRenwuMenu = NULL;
	mFight = NULL;
	mNameArray = CCArray::create();
	mNameArray->retain();

	mSection5TranslationMap = false;
	mChatSection = 0;
	mTongGuanAwardWindowEnd = false;

	m_bDisplayWindow_CannotTouch = false;

	memset(m_fightItem, 0, sizeof(m_fightItem));

	mMainId = TaskManager::getManager()->getLastActivedMainTaskId();
	mMainState = TaskStatus_NotActived;


	//1. bg
	mBg = BaseSprite::create("ditu_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));

	//2  一帧之后初始化 FXScrollMenu， 不然footer调整坐标后会导致FXScrollMenu里面的rect位置不对
	scheduleOnce(schedule_selector(MapsLayer::schedule_initLayer), 0);
	GameDirector::getDirector()->showWaiting();
}

MapsLayer::~MapsLayer(void)
{
	CC_SAFE_RELEASE(mMap);
	CC_SAFE_RELEASE(mRenwuMenu);
	CC_SAFE_DELETE(mFight);
	CC_SAFE_RELEASE(mNameArray);
}

void MapsLayer::schedule_initLayer(float dt)
{
	//2. 
	CCSprite *zgBg = CCSprite::create("ditu_bg.png");
	addChild(zgBg, 1);
	zgBg->setPosition(mBg->getPosition());
	mHeadMenu = CCMenu::create();
	zgBg->addChild(mHeadMenu, 1);
	mHeadMenu->setPosition(CCPointZero);
	CCMenuItemImage *itemNor = CCMenuItemImage::create(
		"zhandou_biaoti_ditu.png",
		"zhandou_biaoti_ditu_select.png",
		this, menu_selector(MapsLayer::menuItem_Clicked_HeadMenu));
	mHeadMenu->addChild(itemNor, 1, Tag_HeadMenu_Normal);
	itemNor->setPosition(ccp(233, 806));
	CCMenuItemImage *itemHard = CCMenuItemImage::create(
		"zhandou_biaoti_diyu.png",
		"zhandou_biaoti_diyu_select.png",
		this, menu_selector(MapsLayer::menuItem_Clicked_HeadMenu));
	mHeadMenu->addChild(itemHard, 1, Tag_HeadMenu_Hard);
	itemHard->setPosition(ccp(413, 806));
	// main menu
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 5);
	mMainMenu->setPosition(ccp(0,0));
	mMainMenu->setVisible(false);
	CCMenuItemImage *tongGuanItem = CCMenuItemImage::create(
		"ditu_anniu_tongguanjiangli.png",
		"ditu_anniu_tongguanjiangli_select.png",
		this, menu_selector(MapsLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(tongGuanItem, 0, Tag_MainMenuItem_TongGuan);
	tongGuanItem->setPosition(ccp(574, 64));
	CCMenuItemImage *shouHuItem = CCMenuItemImage::create(
		"ditu_anniu_shouhuzhiling.png",
		"ditu_anniu_shouhuzhiling_select.png",
		this, menu_selector(MapsLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(shouHuItem, 0, Tag_MainMenuItem_ShouHuZhiLing);
	shouHuItem->setPosition(ccp(463, 64));

	//3. map data
	mMap = CCSprite::create();
	mBg->addChild(mMap,1);
	mMap->setPosition(ccp(mBg->getContentSize().width/2, 11+736/2));  //边框的宽为11
	mMap->retain();
	//
	mMapCache = CCSprite::create();
	mBg->addChild(mMapCache, 0);
	mMapCache->setPosition(mMap->getPosition());
	mMapCache->setVisible(false);


	//4. 任务 menu
	mRenwuMenu = FXScrollMenu::create(mMap->getParent()->convertToWorldSpace(ccp(11,11)), CCSize(618, 736));
	mRenwuMenu->retain();
	mMap->addChild(mRenwuMenu, 1, Tag_TaskMenu);
	mRenwuMenu->setPosition(CCPointZero);

	//5. 章节title bar
	mTitleBar = BaseSprite::create("ditu_dibiao_biaoti.png");
	mBg->addChild(mTitleBar, 3);
	mTitleBar->setPosition(ccp(mBg->getContentSize().width/2, 718));
	{
		CCLabelTTF *title = CCLabelTTF::create(LFStrings::getValue("WorldMapName").c_str(), fontStr_kaiti, 24);
		title->setColor(ccBLACK);
		mTitleBar->addChild(title, 1, Tag_Label_CurMapName);
		title->setPosition(ccp(mTitleBar->getContentSize().width/2, 40));
		//缩放等级
		CCLabelTTF *la = CCLabelTTF::create(LFStrings::getValue("SuoFangDengJi").c_str(), fontStr_kaiti, 20);
		mTitleBar->addChild(la);
		la->setPosition(ccp(mTitleBar->getContentSize().width/2, mTitleBar->getContentSize().height/2-25));
		// world -> captrue
		CCSprite *w_di = CCSprite::create("ditu_dengji_bg.png");
		mTitleBar->addChild(w_di);
		w_di->setPosition(ccp(mTitleBar->getContentSize().width/2 - 20, la->getPositionY()-25));
		CCSprite *world = CCSprite::create("jineng_chufalunci_icon_chufa.png");
		mTitleBar->addChild(world, 1, Tag_CircleImg_World);
		world->setPosition(w_di->getPosition());
		CCSprite *c_di = CCSprite::create("ditu_dengji_bg.png");
		mTitleBar->addChild(c_di);
		c_di->setPosition(ccp(mTitleBar->getContentSize().width/2 + 20, world->getPositionY()));
		CCSprite *capture = CCSprite::create("jineng_chufalunci_icon.png");
		mTitleBar->addChild(capture, 1, Tag_CircleImg_Captrue);
		capture->setPosition(c_di->getPosition());
		CCSprite *_img = CCSprite::create("ditu_dengji_dayu.png");
		mTitleBar->addChild(_img);
		_img->setPosition(ccp(mTitleBar->getContentSize().width/2, world->getPositionY()));
	}


	resetZhangJie_AddRefreshUI(mCurZhangjie);

	////////////////////////////////////////////////////////////////////////////////////
	if (mCurDifficulty == difficult_hard) // static field
	{
		menuItem_Clicked_HeadMenu(itemHard);
	}
	else
	{
		menuItem_Clicked_HeadMenu(itemNor);
	}

}

void MapsLayer::menuItem_Clicked_HeadMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_HeadMenu_Normal)
	{
//		if (mCurDifficulty == difficult_normal) return;
		mCurDifficulty = difficult_normal;
		//head item
		{
			CCMenuItemImage *itemNor = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenu_Normal);
			itemNor->initWithNormalImage(
				"zhandou_biaoti_ditu_select.png",
				"zhandou_biaoti_ditu.png",
				NULL,
				this, menu_selector(MapsLayer::menuItem_Clicked_HeadMenu));
			CCMenuItemImage *itemHard = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenu_Hard);
			itemHard->initWithNormalImage(
				"zhandou_biaoti_diyu.png",
				"zhandou_biaoti_diyu_select.png",
				NULL,
				this, menu_selector(MapsLayer::menuItem_Clicked_HeadMenu));
		}
		//draw task
		addObserver(callfuncO_selector(MapsLayer::rsp_QueyNormal_HardTask), MSG_getTaskListRsp);
		mCmdHlp->cmdGetTaskList(1);  //无用的参数
	}
	else if (tag == Tag_HeadMenu_Hard)
	{
		//地狱副本暂时不开放
		{
//			MessageDisplay *layer = MessageDisplay::create(LFStrings::getValue("WeiKaiQi"));
//			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			//menuItem_Clicked_HeadMenu(mHeadMenu->getChildByTag(Tag_HeadMenu_Normal));

			UnblockWarnDialog* player = UnblockWarnDialog::create(LFStrings::getValue("Diyu_WeiKaiFang"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(player);
			return;
		}

//		if (mCurDifficulty == difficult_hard) return;
		mCurDifficulty = difficult_hard;
		//
		{
			CCMenuItemImage *itemHard = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenu_Hard);
			itemHard->initWithNormalImage(
				"zhandou_biaoti_diyu_select.png",
				"zhandou_biaoti_diyu.png",
				NULL,
				this, menu_selector(MapsLayer::menuItem_Clicked_HeadMenu));
			CCMenuItemImage *itemNor = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenu_Normal);
			itemNor->initWithNormalImage(
				"zhandou_biaoti_ditu.png",
				"zhandou_biaoti_ditu_select.png",
				NULL,
				this, menu_selector(MapsLayer::menuItem_Clicked_HeadMenu));
		}
		//
		addObserver(callfuncO_selector(MapsLayer::rsp_QueyNormal_HardTask), MSG_queryHellTaskHeroInfoRsp);
		mCmdHlp->cmdQueryHellTaskHeroInfo();
	}


	removeChildByTag(Tag_NpcBianhui_Layer);
	removeChildByTag(Tag_Npc_Window);
	removeChildByTag(Tag_SaoDang_Window);
	removeChildByTag(Tag_DiaoLuo_Window);
	disableAllTouchEnd();
}

//任务按钮的显示与不显示 是通过 mainId和mainState来确定的，那么地狱副本 更改curDifficulty之后 再更改前面两个变量
//即可实现 之前 普通副本 的复用。  ---> resetZhuangJie_AddUI  即可刷新整个ui    (g_task 在fightselttlementLayer有用到)
// mMainId mMainState都是将地狱副本的id - 100 ， g_taskId 是多少就是多少
void MapsLayer::rsp_QueyNormal_HardTask(CCObject *msg)
{
	GameDirector::getDirector()->hideWaiting();

	if (mCurDifficulty == difficult_normal)
	{
		removeObserver(MSG_getTaskListRsp);
		SPCmd_GetTaskListRsp* rsp = (SPCmd_GetTaskListRsp*)((MSG_Rsp*)msg)->getData();
		if (rsp->err_code != Success)
		{
			MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
			return;
		}
		//
		// 更新数据
		mMainId = TaskManager::getManager()->getLastActivedMainTaskId();
		mMainState = TaskManager::getManager()->getLastActivedMainTaskStatus();
//		MapsManager::getManager()->setCurMainTask(mMainId, mMainState);
	}
	else if (mCurDifficulty == difficult_hard)
	{
		removeObserver(MSG_queryHellTaskHeroInfoRsp);
		SPCmd_QueryHellTaskHeroInfoRsp *data = (SPCmd_QueryHellTaskHeroInfoRsp *)((MSG_Rsp*)msg)->getData();
		if (data->err_code)
		{
			MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
			return;
		}
		//
		// 更新数据
		mMainId = TaskManager::getManager()->getLastActiveHellTaskId() - HellTaskId_Min + 1;
		mMainState = TaskStatus_Got;
	}


	//如果是 普通副本， 还要获取关卡评级
	if (mCurDifficulty == difficult_hard)
	{
		resetZhangJie_AddRefreshUI(mCurZhangjie);
	}
	else
	{
		addObserver(callfuncO_selector(MapsLayer::rsp_QueryTaskRate), MSG_queryRateTaskRsp_F);
		mCmdHlp->cmdQueryTaskRate();	
	}
	
}

void MapsLayer::rsp_QueryTaskRate(CCObject *msg_f)
{
	removeObserver(MSG_queryRateTaskRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_QueryRateTaskRsp* data = (Flat_QueryRateTaskRsp*) ((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	
	mTaskRate = data->taskRate;
	mChapterRate = data->chapterRate;

	resetZhangJie_AddRefreshUI(mCurZhangjie);
	if (mCurZhangjie == 2)
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstXueSeJiaoTang))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstXueSeJiaoTang);
			

			UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("TiShi_XueSeJiaoTang"), true);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		}
	}
	//加载成功直接跳转到界面
	if (m_GoTo_UI_Type == GoTo_Shouhuzhiling_UI_Type)
	{
		menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_MainMenuItem_ShouHuZhiLing));
	}
	else if (m_GoTo_UI_Type == GoTo_LevelTiaozhen_UI_Type || m_GoTo_UI_Type == GoTo_BossTiaozhen_UI_Type)
	{
		int iZhangjie = 1;
		if (m_iWillGoToLevel <= 10)
		{
			iZhangjie = 1;
		}
		else if (m_iWillGoToLevel > 10 && m_iWillGoToLevel <= 20)
		{
			iZhangjie = 2;
		}
		else if (m_iWillGoToLevel > 20 && m_iWillGoToLevel <= 30)
		{
			iZhangjie = 3;
		}
		else if (m_iWillGoToLevel > 30 && m_iWillGoToLevel <= 40)
		{
			iZhangjie = 4;
		}
		int iLevelIndex = (m_iWillGoToLevel-1)%10;
		resetZhangJie_AddRefreshUI(iZhangjie);
		if (m_GoTo_UI_Type == GoTo_LevelTiaozhen_UI_Type)
		{
			menuItemClicked_Task(mRenwuMenu->getChildByTag(mCurZhangjie*10-9+iLevelIndex));
		}	
	}

	this->updateTongGuanJiangliInfo();
}

void MapsLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	if (m_bDisplayWindow_CannotTouch) return;

	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MainMenuItem_TongGuan)
	{
		{
			m_bClickTongguanBtn = true;
			addObserver(callfuncO_selector(MapsLayer::rsp_QueryRateChapterBonus), MSG_queryRateTaskBonus_F);
			mCmdHlp->cmdQueryRateTaskBonus(RateStarType_Chapter, mCurZhangjie);
		}
	}
	else if (tag == Tag_MainMenuItem_ShouHuZhiLing)
	{
		addObserver(callfuncO_selector(MapsLayer::rsp_QueryShouHuZhiLing), MSG_queryProtectSpiritRsp);
		mCmdHlp->cmdQueryProtectSpirit(mCurZhangjie);
	}
}

void MapsLayer::rsp_ChangeTaskState(CCObject* obj)
{

	removeObserver(MSG_ChangeTaskStatRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_ChangeTaskStatRsp* data = (Flat_ChangeTaskStatRsp*)((Flat_MSG_Rsp*)obj)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}


	//地狱副本打过的还是用以前的xx， 普通副本换成旗帜上加星星
	if(mChatSection == 5 && mCurDifficulty == difficult_hard)
	{
		 //section 5 animation
		CCMenuItem *item = (CCMenuItem*)mRenwuMenu->getChildByTag(mMainId);
		item->runAction(CCSequence::create(
			CCFadeTo::create(0.25, 0)
			, CCCallFuncN::create(this, callfuncN_selector(MapsLayer::section5EndAnimation))
			, NULL));
		 return;
	}
	else if (mChatSection == 5 && mCurDifficulty == difficult_normal && TaskManager::getManager()->getCurrentFightTaskId() != -1)
	{
		//玩家通过几星就播放几星的镶嵌动画
		//只要未领取奖励，领奖按钮一直闪烁显示
		//玩家关闭这个界面后，落下下一关卡的路牌
		//当玩家通过一章所有关卡后，自动弹出章节通关奖励
		menuItemClicked_Task(mRenwuMenu->getChildByTag(TaskManager::getManager()->getCurrentFightTaskId()));

// 		// menuItemClicked_NpcDetailBack之后 判断如果当前state是 finish 则直接发消息！
// 		addObserver(SEL_CallFuncO(&MapsLayer::rsp_FlushTaskList), MSG_getTaskListRsp);
// 		mCmdHlp->cmdGetTaskList(1);  //无用的参数
	}
	else
	{
		addObserver(SEL_CallFuncO(&MapsLayer::rsp_FlushTaskList), MSG_getTaskListRsp);
		mCmdHlp->cmdGetTaskList(1);  //无用的参数
	}
}

void MapsLayer::section5EndAnimation(CCNode *node)
{
	mChatSection = 0;

	CCMenuItemImage *item = (CCMenuItemImage*)node;
	item->initWithNormalImage(
		"ditu_dibiao_renwu_wancheng.png"
		, "ditu_dibiao_renwu_wancheng_select.png"
		, NULL,this, SEL_MenuHandler(&MapsLayer::menuItemClicked_Task));
	item->setOpacity(0);
	item->runAction(CCSequence::create(
		CCDelayTime::create(0.3f)
		, CCFadeTo::create(0.25f, 255)
		, CCCallFunc::create(this, callfunc_selector(MapsLayer::section5EndAnimationEnd_ToFlushTaskState))
		, NULL));

	BaseSprite *ef1 = BaseSprite::create("ditu_dibiao_renwu_wancheng_1.png");
	BaseSprite *ef2 = BaseSprite::create("ditu_dibiao_renwu_wancheng_2.png");
	ef1->setOpacity(0);
	ef2->setOpacity(0);
	mMap->addChild(ef1);
	mMap->addChild(ef2);
	ef1->setPosition(item->getPosition());
	ef2->setPosition(item->getPosition());
	ef1->runAction(CCSequence::create(
		CCDelayTime::create(0.1f)
		, CCFadeTo::create(0.05f, 255)
		, CCDelayTime::create(2.0f)
		, CCFadeTo::create(0.25, 0)
		, CCHide::create()
		, NULL));
	ef2->runAction(CCSequence::create(
		CCDelayTime::create(0.2f)
		, CCFadeTo::create(0.05f, 255)
		, CCDelayTime::create(1.9f)
		, CCFadeTo::create(0.25f, 0)
		, CCHide::create()
		, NULL));
}

void MapsLayer::section5EndAnimationEnd_ToFlushTaskState()
{
	addObserver(SEL_CallFuncO(&MapsLayer::rsp_FlushTaskList), MSG_getTaskListRsp);
	mCmdHlp->cmdGetTaskList(1);  //无用的参数
}

void MapsLayer::rsp_FlushTaskList(CCObject* obj)
{
	int oldId = mMainId;

	removeObserver(MSG_getTaskListRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_GetTaskListRsp* rsp = (SPCmd_GetTaskListRsp*)((MSG_Rsp*)obj)->getData();

	if (rsp->err_code != Success)
	{
		MessageDisplay *displayDialog = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(displayDialog);
		return;
	}

	// 更新数据
	mMainId = TaskManager::getManager()->getLastActivedMainTaskId();
	mMainState = TaskManager::getManager()->getLastActivedMainTaskStatus();
	//
//	MapsManager::getManager()->setCurMainTask(mMainId, mMainState);

	//如果是 普通副本， 还要获取关卡评级
	if (mCurDifficulty == difficult_normal)
	{
		mCurZhangjie = (mMainId+9) / 10;
		addObserver(callfuncO_selector(MapsLayer::rsp_QueryTaskRate), MSG_queryRateTaskRsp_F);
		mCmdHlp->cmdQueryTaskRate();	
		return;  //之后再刷新UI
	}


	//地狱副本才会执行到这里
	resetZhangJie_AddRefreshUI((mMainId+9) / 10);

	if(mChatSection == 1)
	{
		mChatSection = 0;

		menuItemClicked_Task(mRenwuMenu->getChildByTag(mMainId));
	}

}

void MapsLayer::menuItemClicked_NewTask(CCObject* pSender)
{
	mChatSection = 1;
	ChatLayer* layer = ChatLayer::create(
		ChatContentManager::sharedChatContentManager()->getMainTaskChatContentById(mMainId, mChatSection), this); // 5 1
	if(layer != NULL)
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ChatLayer);

	// 对话完成后 会直接调用 chatendcallback
}

void MapsLayer::disableAllTouchBegin()
{
	mRenwuMenu->setEnabled(false);
	mMainMenu->setEnabled(false);
	mHeadMenu->setEnabled(false);

	m_bDisplayWindow_CannotTouch = true;

}


void MapsLayer::setShouhuzhilingMenuEnable(bool e)
{
	CCSprite *bg = (CCSprite*)this->getChildByTag(Tag_Npc_Window);
	if (bg && bg->getChildByTag(Tag_Npc_Menu) && bg->getChildByTag(Tag_ShouHuZhiLingBoard))
	{
		((CCMenu*)bg->getChildByTag(Tag_Npc_Menu))->setEnabled(e);
	}
}

void MapsLayer::disableAllTouchEnd()
{

	this->setShouhuzhilingMenuEnable(true);
	mRenwuMenu->setEnabled(true);
	mMainMenu->setEnabled(true);
	mHeadMenu->setEnabled(true);

	m_bDisplayWindow_CannotTouch = false;
}

void MapsLayer::onEnter()
{
	BaseLayer::onEnter();	
	this->setTouchEnabled(true);
}

void MapsLayer::onExit()
{
	this->setTouchEnabled(false);
	BaseLayer::onExit();
}

void MapsLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	if (m_bDisplayWindow_CannotTouch) return;

	int touchesCount = pTouches->count();
	
    CCTouch *pTouch = (CCTouch*)*pTouches->begin();
    if(pTouch->getID() == 0){
        m_canotMultTouch = false;
        m_mapCacheHasInit = false;
        m_bMultTouchBeginHasInit = false;
        mMultTouchEndPos[0].x = -100;
        mMultTouchEndPos[1].x = -100;
        
        if (touchesCount == 1){
            m_bMultTouch = false;
            mBeginPos = pTouch->getLocation();
            mMoveEndPos = mBeginPos;
        }
    }

}

void MapsLayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
	if (m_bDisplayWindow_CannotTouch) return;

	int touchesCount = pTouches->count();

	if (touchesCount > 1 && mCurZhangjie > 0 && ! m_canotMultTouch)
	{
		m_bMultTouch = true;

		if (! m_bMultTouchBeginHasInit)
		{
			m_bMultTouchBeginHasInit = true;
			CCSetIterator ite = pTouches->begin();
			for(int i=0; ite != pTouches->end() && i < 2; ++i, ++ite)
			{
				CCTouch *touch = (CCTouch*)*ite;
				mMultTouchBeginPos[i] = touch->getLocation();
			}

			initCacheMapByIndx(0);
			mMapCache->setVisible(true);
			mMapCache->setPosition(ccp(mBg->getContentSize().width/2, 11+736/2));
			setAllChildrenColorOrOpacity(mMapCache, ccWHITE, 0, 2);
			return;
		}

		CCSetIterator iter = pTouches->begin();
		CCPoint point1 = ((CCTouch*)(*iter))->getLocation();
		iter++;
		CCPoint point2 = ((CCTouch*)(*iter))->getLocation();
		double distance = sqrt((point1.x-point2.x)*(point1.x-point2.x)+(point1.y-point2.y)*(point1.y-point2.y));
		double oldDistance = sqrt((mMultTouchBeginPos[0].x-mMultTouchBeginPos[1].x)*(mMultTouchBeginPos[0].x-mMultTouchBeginPos[1].x)
			+(mMultTouchBeginPos[0].y-mMultTouchBeginPos[1].y)*(mMultTouchBeginPos[0].y-mMultTouchBeginPos[1].y));
		double scale = distance/oldDistance;

		if (scale < 1 && mCurZhangjie > 0)
		{
			mMap->setScale(scale);

			setAllChildrenColorOrOpacity(mMap, ccWHITE, 255 * scale, 2);
			setAllChildrenColorOrOpacity(mMapCache, ccWHITE, 255 * (1-scale), 2);

			{
				CCPoint moveToPos = getTaskMenuPos(0, mCurZhangjie);
				CCPoint midPos = ccp(mBg->getContentSize().width/2, 11+736/2);
				CCPoint delPos = ccpSub(moveToPos, midPos);
				mMap->setPosition(ccp(moveToPos.x - scale*delPos.x, moveToPos.y - scale*delPos.y));
			}
		}

		mMultTouchEndPos[0] = point1;
		mMultTouchEndPos[1] = point2;

	}
	else if ((touchesCount==1 || m_canotMultTouch) && ! m_bMultTouch)  //已经响应了多点 则不能再触发单点了
	{
		CCSetIterator ite = pTouches->begin();
		for (; ite!=pTouches->end(); ++ite)
		{
			if (((CCTouch*)*ite)->getID() == 0)
				break;
		}
		if (mCurZhangjie > 0 && ite!=pTouches->end())
		{
			CCTouch *pTouch = (CCTouch*)*ite;
			CCPoint curPos = pTouch->getLocation();
			CCPoint deltaPos = ccpSub(curPos, mBeginPos);

			if (fabs(deltaPos.x) > CanNotTouch_X)
				m_canotMultTouch = true;

			if (deltaPos.x < 0 && mCurZhangjie < (mMainId+9)/10)  //you -> zuo     next chapter
			{
				mMap->setPositionX(mBg->getContentSize().width/2 + deltaPos.x);

				if (mMoveEndPos.x > mBeginPos.x) m_mapCacheHasInit = false;
				mMoveEndPos = curPos;
				if (! m_mapCacheHasInit)
				{
					m_mapCacheHasInit = true;

					initCacheMapByIndx(mCurZhangjie + 1);
					mMapCache->setVisible(true);
				}
				mMapCache->setPositionX(mMap->getPositionX()+mMap->getContentSize().width);
			}
			else if (deltaPos.x > 0 && mCurZhangjie > 1)
			{
				mMap->setPositionX(mBg->getContentSize().width/2 + deltaPos.x);

				if (mMoveEndPos.x < mBeginPos.x) m_mapCacheHasInit = false;
				mMoveEndPos = curPos;
				if (! m_mapCacheHasInit)
				{
					m_mapCacheHasInit = true;

					initCacheMapByIndx(mCurZhangjie - 1);
					mMapCache->setVisible(true);
				}
				mMapCache->setPositionX(mMap->getPositionX()-mMap->getContentSize().width);
			}
			else if (deltaPos.x != 0)//
			{
				mMapCache->setVisible(false);
				m_mapCacheHasInit = false;

				mMap->setPositionX(mBg->getContentSize().width/2 + deltaPos.x/fabs(deltaPos.x)*log(fabs(deltaPos.x))*10);
			}
		}
	}
}

void MapsLayer::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	if (m_bDisplayWindow_CannotTouch) return;

	int touchesCount = pTouches->count();

	if (m_bMultTouch && m_bMultTouchBeginHasInit)
	{
		m_bMultTouchBeginHasInit = false;
		//to do
		CCPoint point1 = mMultTouchEndPos[0];
		CCPoint point2 = mMultTouchEndPos[1];
		
		double distance = sqrt((point1.x-point2.x)*(point1.x-point2.x)+(point1.y-point2.y)*(point1.y-point2.y));
		double oldDistance = sqrt((mMultTouchBeginPos[0].x-mMultTouchBeginPos[1].x)*(mMultTouchBeginPos[0].x-mMultTouchBeginPos[1].x)
			+(mMultTouchBeginPos[0].y-mMultTouchBeginPos[1].y)*(mMultTouchBeginPos[0].y-mMultTouchBeginPos[1].y));
		double scale = distance/oldDistance;

		if (mCurZhangjie > 0)
		{
			if (scale < 0.8)
			{
				CCPoint moveToPos = getTaskMenuPos(0, mCurZhangjie);
				mMap->runAction(CCSequence::create(
					CCSpawn::create(CCScaleTo::create(0.2f, 0.01f), CCMoveTo::create(0.2f, moveToPos), NULL),
					CCCallFunc::create(this, callfunc_selector(MapsLayer::callfunc_mapsScrollEnd)),
					NULL));
				nodeAndAllChildRunAction(mMap, CCFadeTo::create(0.18f, 0));

				nodeAndAllChildRunAction(mMapCache, CCFadeTo::create(0.2f, 255));

				mCurZhangjie = 0;
			}
			else  //还原
			{
				CCPoint midPos = ccp(mBg->getContentSize().width/2, 11+736/2);
				mMap->runAction(CCSequence::create(
					CCSpawn::create(CCScaleTo::create(0.2f, 1), CCMoveTo::create(0.2f, midPos), NULL),
					CCCallFunc::create(this, callfunc_selector(MapsLayer::callfunc_mapsScrollEnd)),
					NULL));
				nodeAndAllChildRunAction(mMap, CCFadeTo::create(0.2f, 255));

				nodeAndAllChildRunAction(mMapCache, CCFadeTo::create(0.18f, 0));
			}
		}
		else  //放大 世界地图？？
		{

		}
	}
	else if ((touchesCount==1 || m_canotMultTouch) && ! m_bMultTouch)  //已经响应了多点 则不能再触发单点了
	{
		CCSetIterator ite = pTouches->begin();
		for (; ite!=pTouches->end(); ++ite)
		{
			if (((CCTouch*)*ite)->getID() == 0)
				break;
		}
		if (mCurZhangjie > 0 && ite!=pTouches->end())
		{
			CCTouch *pTouch = (CCTouch*)*ite;

			CCPoint curPos = pTouch->getLocation();
			CCPoint deltaPos = ccpSub(curPos, mBeginPos);
			if (fabs(deltaPos.x) < TouchDelta_X)
			{
				mMap->runAction(CCSequence::create(
					CCEaseSineOut::create(CCMoveTo::create(0.2f, ccp(11+mMap->getContentSize().width/2, mMap->getPositionY()))),
					NULL));

				CCPoint _pos;
				if (deltaPos.x < 0) _pos = ccp(640-11+mMap->getContentSize().width/2, mMap->getPositionY());
				else _pos = ccp(11-mMap->getContentSize().width/2, mMap->getPositionY());

				mMapCache->runAction(CCSequence::create(
					CCEaseSineOut::create(CCMoveTo::create(0.2f, _pos)),
					CCCallFunc::create(this, callfunc_selector(MapsLayer::callfunc_mapsScrollEnd)),
					NULL));
			}
			else if (deltaPos.x < 0 && mCurZhangjie < (mMainId+9)/10)  //you -> zuo     next chapter
			{
				mCurZhangjie += 1;
				mMap->runAction(CCSequence::create(
					CCEaseSineOut::create(CCMoveTo::create(0.2f, ccp(11-mMap->getContentSize().width/2, mMap->getPositionY()))),
					NULL));
				mMapCache->runAction(CCSequence::create(
					CCEaseSineOut::create(CCMoveTo::create(0.2f, ccp(mBg->getContentSize().width/2, mMap->getPositionY()))),
					CCCallFunc::create(this, callfunc_selector(MapsLayer::callfunc_mapsScrollEnd)),
					NULL));
			}
			else if (deltaPos.x > 0 && mCurZhangjie > 1)
			{
				mCurZhangjie -= 1;
				mMap->runAction(CCSequence::create(
					CCEaseSineOut::create(CCMoveTo::create(0.2f, ccp(640-11+mMap->getContentSize().width/2, mMap->getPositionY()))),
					NULL));
				mMapCache->runAction(CCSequence::create(
					CCEaseSineOut::create(CCMoveTo::create(0.2f, ccp(mBg->getContentSize().width/2, mMap->getPositionY()))),
					CCCallFunc::create(this, callfunc_selector(MapsLayer::callfunc_mapsScrollEnd)),
					NULL));
			}
			else
			{
				mMap->runAction(CCSequence::create(
					CCEaseSineOut::create(CCMoveTo::create(0.2f, ccp(11+mMap->getContentSize().width/2, mMap->getPositionY()))),
					NULL));
			}
		}
	}
}

void MapsLayer::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
	if (m_bDisplayWindow_CannotTouch) return;

	CCTouch *pTouch = (CCTouch*)*pTouches->begin();
	if (pTouch->getID() != 0)
		return;

	mMapCache->setVisible(false);
	mMap->runAction(CCSequence::create(
		CCSpawn::create(CCEaseSineOut::create(CCMoveTo::create(0.1f, ccp(11+mMap->getContentSize().width/2, mMap->getPositionY()))),
						CCScaleTo::create(0.1f, 1), NULL),
		NULL));
	nodeAndAllChildRunAction(mMap, CCFadeTo::create(0.1f, 255));
}

void MapsLayer::callfunc_mapsScrollEnd()
{
	mMap->setPosition(ccp(mBg->getContentSize().width/2, 11+mMap->getContentSize().height/2));
	mMapCache->setVisible(false);
	mMap->setScale(1);
	resetZhangJie_AddRefreshUI(mCurZhangjie);
}

void MapsLayer::teachGuide()
{
	if (!m_bIsTeachIng)
	{
		if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstMaoXian))
		{
			//bool isTeach = GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstMaoXian);
			//if (isTeach)
			{
				m_bIsTeachIng = true;
				if (GameGuideManager::getManager()->getCurGuideState() == 1)
				{
					GameGuideManager::getManager()->setCurGuideState(2);
					CCMenuItemImage* qzItem = (CCMenuItemImage*)mRenwuMenu->getChildByTag(1);
					//CCSprite * qizhi = (CCSprite*)mMap->getChildByTag(TAG_QiZhi);
					if(qzItem)
					{
						//GameGuideManager::getManager()->setCurGuideState(2);
						GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
						guideLayer->setChatEndBack(this,SEL_CallFunc(&MapsLayer::teachEnd));
						guideLayer->init(qzItem->getContentSize(), qzItem->getParent()->convertToWorldSpace(qzItem->getPosition())
							, LFStrings::getValue("kaishidiyicirenwu"), NULL, false);
						CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);

						qzItem->setVisible(true);
						qzItem->stopAllActions();
					}
					return;
				}
				else if (GameGuideManager::getManager()->getCurGuideState() == 2)
				{
					m_bIsTeachIng = true;
					GameGuideManager::getManager()->setCurGuideState(3);
					CCMenuItemImage* qzItem = (CCMenuItemImage*)mRenwuMenu->getChildByTag(mMainId);

					if(qzItem)
					{
						CCSprite *node = (CCSprite *)qzItem->getChildByTag(10);
						if (node)
						{
							node->removeFromParent();
						}

						GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
						guideLayer->setChatEndBack(this,SEL_CallFunc(&MapsLayer::teachEnd));
						guideLayer->init(qzItem->getContentSize(), qzItem->getParent()->convertToWorldSpace(qzItem->getPosition())
							, LFStrings::getValue("feicuishenlin"), NULL, false);
						CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);

						qzItem->setVisible(true);
						qzItem->stopAllActions();
					}
					return;
				}

			}
		}
	}
	
	
	//centerMaintask  --> 刚通关 新旗帜掉落的时候会调用teachGuide
	if (mMainState == TaskStatus_NotGot)
	{
		unblockNewZone_TaskEnd();
	}
	//unblockNewZone_TaskEnd();
	/*
	if (g_ChuanQiFuBen_Opend)
	{
		g_ChuanQiFuBen_Opend = false;
		g_ChuanQiFuBen_Guide = true;

		CCMenuItem *item = (CCMenuItem*)mHeadMenu->getChildByTag(Tag_HeadMenu_Hard);
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		guideLayer->init(item->getContentSize(),
			item->getParent()->convertToWorldSpace(item->getPosition()),
			LFStrings::getValue("chuanqifuben_kaiqixuanze"), NULL, false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}
	else if (g_ChuanQiFuBen_Guide)
	{
		ChatLayer* layer = ChatLayer::create(
			ChatContentManager::sharedChatContentManager()->getGuideChatContent("ChuanQiFuBen_ZhenZhang_Section1"), this);
		if(layer != NULL)
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ChatLayer);
	}
	else if (g_ChuanQiSuiPianHeCheng_Guide)
	{
		ChatLayer* layer = ChatLayer::create(
			ChatContentManager::sharedChatContentManager()->getGuideChatContent("ChuanQiFuBen_ZhenZhang_Section5"), this);
		if(layer != NULL)
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ChatLayer);
	}*/
	if (TaskManager::getManager()->getLastActivedMainTaskId() >= 6 && TaskManager::getManager()->getLastActivedMainTaskStatus() == TaskStatus_Finished)
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstYingXiongBang))
		{
			g_YingXiongBang_Opend = true;
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstYingXiongBang);
		}
	}
}
void MapsLayer::ZhuShouchatEndCallBack()
{

}
//此时 的id 不区分 hard 和 normal
void MapsLayer::menuItemClicked_Task(CCObject* pSender)
{
	if (m_bDisplayWindow_CannotTouch) return;

	//记录按下的那个 任务
	m_task_menuItem = pSender;
	int iId = ((CCMenuItem*)m_task_menuItem)->getTag(); //地狱副本 需要 + 100

	if (iId >= 25)
	{
		vector<ShowMsg> list;
		ShowMsg msg1;
		msg1.msg = LFStrings::getValue("WeiKaiTongDiTu");
		msg1.color = ccWHITE;

		ShowMsg msg2;
		msg2.msg = LFStrings::getValue("QiDaiHouXuGengXin");
		msg2.color = fonColor_PuTong;

		list.push_back(msg1);

		list.push_back(msg2);
		ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
			list, 1,this);
		if(layer != NULL)
		{

			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
		}
		return;
	}

	if (iId == mMainId && mMainState == TaskStatus_NotGot)
	{
		menuItemClicked_NewTask(NULL);
		return;
	}



	if (mCurDifficulty == difficult_normal)
	{
		addObserver(callfuncO_selector(MapsLayer::rsp_QueryRateTaskBonus), MSG_queryRateTaskBonus_F);
		mCmdHlp->cmdQueryRateTaskBonus(RateStarType_Task, iId);
	}
	//地狱副本没有 关卡评级
	else if (mCurDifficulty == difficult_hard)
	{
		addObserver(callfuncO_selector(MapsLayer::rsp_QueryEquipLoot_HardDificult), MSG_QueryFubenDiaoluoRsp_F);
		mCmdHlp->cmdQueryFubenDiaoluo(iId + 100);
	}
}

void MapsLayer::rsp_QueryEquipLoot_HardDificult(CCObject *msg_f)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_QueryFubenDiaoluoRsp_F);

	Flat_QueryFubenDiaoluoRsp *diaoLuoData = (Flat_QueryFubenDiaoluoRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if(diaoLuoData->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(diaoLuoData->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	mDiaoLuoRsp = *diaoLuoData;

	//
	//用大的board
	CCSize bgSize;
	BaseSprite *bg = BaseSprite::create("ditu_renwu_tanchukuang_bg.png");
	bgSize = bg->getContentSize();
	addChild(bg, 10, Tag_Npc_Window);
	bg->setAnchorPoint(ccp(0.5f, 1));
	bg->setPosition(ccp(320, 748));
	//地图背景 变灰，不可触摸
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	addChild(menu, 9, Tag_NpcBianhui_Layer); 
	CCSprite* touming1 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,640,736-bgSize.height+20));
	CCSprite* touming2 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,640,736-bgSize.height+20));
	CCMenuItemSprite* touming = CCMenuItemSprite::create(touming1, touming2, this, menu_selector(MapsLayer::menuItemClicked_NpcDetailBack));
	menu->addChild(touming);
	touming->setPosition(ccp(320, 10+(736-bgSize.height)/2.0));
	touming->setOpacity(0);
	touming->runAction(CCFadeTo::create(0.05f, 160));

	disableAllTouchBegin();

	int curId = ((CCMenuItem*)m_task_menuItem)->getTag(); //真正的id 需要 + 100

	//zhangjie    --->   地狱副本第一章 
	char buf[50];
	sprintf(buf, "zhangjie%d", mCurZhangjie);
	string tempStri = LFStrings::getValue("DiYuFuBen") + LFStrings::getValue(buf);
	CCLabelTTF *zjLabel = CCLabelTTF::create(tempStri.c_str(), fontStr_kaiti, Label_Size_Max);
	bg->addChild(zjLabel);
	zjLabel->setColor(Label_Color_DanHuang);
	zjLabel->setPosition(ccp(bgSize.width/2, 557));
	// 任务10 西蒙斯
	BaseSprite *bbb = BaseSprite::create("ditu_diyufuben_biaoti.png");
	bg->addChild(bbb);
	bbb->setPosition(ccp(bgSize.width/2, 496));
	sprintf(buf, "%s%d ", LFStrings::getValue("RenWu").c_str(), curId - (mCurZhangjie-1)*10);
	tempStri = buf + ChatContentManager::sharedChatContentManager()->getMainTaskNameById(curId);
	CCLabelTTF *rwLabel = CCLabelTTF::create(tempStri.c_str(), fontStr_kaiti, Label_Size_Min);
	bg->addChild(rwLabel, 1);
	rwLabel->setColor(Label_Color_DanHuang);
	rwLabel->setPosition(ccp(bgSize.width/2, 496));
	//次数消耗说明
	BaseSprite *titleImg = BaseSprite::create("ditu_diyufuben_biaoti_bg.png");
	bg->addChild(titleImg);
	titleImg->setPosition(ccp(bg->getContentSize().width/2, 421));
	{
		CCString temp;
		temp.initWithFormat((LFStrings::getValue("DiYuFuBen")+LFStrings::getValue("meiRiKeTiaoZhanCiShu")).c_str(), 5);
		CCLabelTTF *t1 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
		titleImg->addChild(t1);
		t1->setPosition(ccp(titleImg->getContentSize().width/2, 60));
		t1->setColor(ccBLACK);
		//
		temp.initWithFormat((LFStrings::getValue("jinri")+LFStrings::getValue("shengyucishu")+": %u").c_str(), 
			TaskManager::getManager()->getHellBattelTimeRemainder());
		CCLabelTTF *t2 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
		titleImg->addChild(t2);
		t2->setColor(ccBLACK);
		t2->setPosition(ccp(titleImg->getContentSize().width/2, 102-67));
	}
	//掉装说明
	BaseSprite *dzBarImg = BaseSprite::create("ditu_diyufuben_wenzi.png");
	bg->addChild(dzBarImg);
	dzBarImg->setPosition(ccp(bgSize.width/2, 337));
	//menu
	CCMenu* npcMenu = CCMenu::create();
	bg->addChild(npcMenu, 1, Tag_Npc_Menu);
	npcMenu->setPosition(CCPointZero);

	//开始
	CCMenuItemImage* kaishi = CCMenuItemImage::create(
		"zhandou_ditu_renwushuoming_kaishi.png"
		, "zhandou_ditu_renwushuoming_kaishi_select.png"
		, this, menu_selector(MapsLayer::menuItemClicked_BeginFight));
	npcMenu->addChild(kaishi, 1, Tag_NpcMenuItem_KaiShi);
	kaishi->setPosition(ccp(bg->getContentSize().width/2, bgSize.height - 562));

	//装备
	int scap = 140;
	int eqNum = mDiaoLuoRsp.diaoluo_vec.size();
	for (int i=0; i!=eqNum; ++i)
	{
		//diwen
		string cardFile = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(
			(ZhuangbeiColour)mDiaoLuoRsp.diaoluo_vec.at(i).zhuangbei_colour, 
			false,//ChuanqiPool::getPool()->isShenBing(mDiaoLuoRsp.diaoluo_vec.at(i).group_id), 
			SuitPool::getPool()->isSuit(mDiaoLuoRsp.diaoluo_vec.at(i).group_id));
		CCMenuItemImage *diwen = CCMenuItemImage::create(cardFile.c_str(), cardFile.c_str()
			, this, menu_selector(MapsLayer::menuItemClicked_NpcEquipLoot));
		npcMenu->addChild(diwen, 0, i);

		if (eqNum % 2 == 0)
			diwen->setPositionX(bgSize.width/2 + scap/2 + (i-eqNum/2)*scap);
		else 
			diwen->setPositionX(bgSize.width/2 + (i-eqNum/2)*scap);
		diwen->setPositionY(238);
		//投影
		BaseSprite *tyImg = BaseSprite::create("ditu_diyufuben_zhuangbei_bg.png");
		bg->addChild(tyImg);
		tyImg->setPosition(ccp(diwen->getPositionX()+1, diwen->getPositionY()-9));
		//equip
		string equipfile = mDiaoLuoRsp.diaoluo_vec.at(i).zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		BaseSprite *equipImg = BaseSprite::create(equipfile);
		diwen->addChild(equipImg, 10);
		equipImg->setScale(0.23f);
		equipImg->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
		//name
		CCLabelTTF *nameLabel = CCLabelTTF::create(mDiaoLuoRsp.diaoluo_vec.at(i).zhuangbei_name.c_str(),
			fontStr_kaiti, Label_Size_Min);
		diwen->addChild(nameLabel);
		nameLabel->setColor(ccBLACK);
		nameLabel->setPosition(ccp(diwen->getContentSize().width/2, -nameLabel->getContentSize().height));
	}


	//第一次地狱副本教学
	if (g_ChuanQiFuBen_Guide)
	{
		g_ChuanQiFuBen_Guide = false;
		g_ChuanQiSuiPianHeCheng_Guide = true;
		//2 kaishi 
		GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;
		guideLayer2->init(kaishi->getContentSize(), kaishi->getParent()->convertToWorldSpace(kaishi->getPosition())
			, LFStrings::getValue("diyigerenwudianjikaishi"), NULL, false);
		//1 次数 说明
		GuideLayerBase *guideLayer1 = new GuideLayer_SimpleLayer;
		guideLayer1->init(titleImg->getContentSize(), titleImg->getParent()->convertToWorldSpace(titleImg->getPosition())
			, LFStrings::getValue("chuanqifuben_CiShuShuoMing"), guideLayer2, true);
		//
		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer1, 1024);
	}
}

void MapsLayer::rsp_QueryRateTaskBonus(CCObject *msg_f)
{
	removeObserver(MSG_queryRateTaskBonus_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_QueryRateTaskBonusRsp *data = (Flat_QueryRateTaskBonusRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *displayDialog = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(displayDialog);
		return;		
	}
	// 数据已经存入 ChessBoardStateManager

	addObserver(callfuncO_selector(MapsLayer::rsp_QueryEquipLoot), MSG_QueryFubenDiaoluoRsp_F);
	mCmdHlp->cmdQueryFubenDiaoluo(((CCMenuItem*)m_task_menuItem)->getTag());
}

void MapsLayer::rsp_QueryEquipLoot(CCObject *msg_f)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_QueryFubenDiaoluoRsp_F);

	Flat_QueryFubenDiaoluoRsp *diaoLuoData = (Flat_QueryFubenDiaoluoRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if(diaoLuoData->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(diaoLuoData->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	mDiaoLuoRsp = *diaoLuoData;

	int curTaskId = ((CCMenuItem*)m_task_menuItem)->getTag();
	Flat_QueryRateTaskBonusRsp* rsp_bonus = ChessBoardStateManager::getManager()->getTaskRateBonus(curTaskId);
	if (!rsp_bonus)
	{
		return;
	}

	//掉装 或技能书 用大的board
	CCSize bgSize;
	BaseSprite *bg;
	if (mDiaoLuoRsp.diaoluo_vec.size() > 0 || mDiaoLuoRsp.items.size() > 0) 
	{
		bg = BaseSprite::create("ditu_renwu_tanchukuang_bg.png");
		bgSize = bg->getContentSize();
	}
	else
	{
		bg = BaseSprite::create("ditu_renwu_tanchukuang_bg2.png");
		bgSize = bg->getContentSize();
	}
	//地图背景 变灰，不可触摸
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	addChild(menu, 9, Tag_NpcBianhui_Layer); 
	CCSprite* touming1 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,640,736-bgSize.height+20));
	CCSprite* touming2 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,640,736-bgSize.height+20));
	CCMenuItemSprite* touming = CCMenuItemSprite::create(touming1, touming2, this, menu_selector(MapsLayer::menuItemClicked_NpcDetailBack));
	menu->addChild(touming);
	touming->setPosition(ccp(320, 11+(736-bgSize.height)/2.0));
	touming->setOpacity(0);
	touming->runAction(CCFadeTo::create(0.05f, 160));

	disableAllTouchBegin();


	//弹出任务信息框
	addChild(bg, 10, Tag_Npc_Window);
	bg->setAnchorPoint(ccp(0.5f, 1));
	bg->setPosition(ccp(320, 748));

	//zhangjie    --->   第一章 10 西蒙斯
	char buf[20];
	sprintf(buf, "zhangjie%d", mCurZhangjie);
	string tempStri = LFStrings::getValue(buf);
	sprintf(buf, " %d ", curTaskId - (mCurZhangjie-1)*10);
	tempStri += buf;
	tempStri += ChatContentManager::sharedChatContentManager()->getMainTaskNameById(curTaskId);
	CCLabelTTF *titleLabel = CCLabelTTF::create(tempStri.c_str(), fontStr_kaiti, 22);
	bg->addChild(titleLabel);
	titleLabel->setColor(ccc3(195, 145, 96));
	titleLabel->setPosition(ccp(bgSize.width/2, bgSize.height-60));
	//体力消耗说明
	BaseSprite *titleImg = BaseSprite::create("tidu_renwu_tanchukuang_xinxi.png");
	bg->addChild(titleImg);
	titleImg->setPosition(ccp(bg->getContentSize().width/2, bgSize.height-134));

	CCLabelTTF* pTopTip =  NULL;
	bool bChange = false;
	int levelIndex = curTaskId;//(mCurZhangjie-1)*10+curTaskId;
	if (levelIndex == 15 || levelIndex == 25 || levelIndex == 35)
	{
		pTopTip = CCLabelTTF::create(LFStrings::getValue("TongGuan_top_tip").c_str(), fontStr_kaiti, 20);
		bChange = true;
	}
	else
	{
		pTopTip = CCLabelTTF::create(LFStrings::getValue("TongGuan_top_normal_tip").c_str(), fontStr_kaiti, 20);
	}
	if (pTopTip)
	{
		bg->addChild(pTopTip);
		pTopTip->setPosition(ccp(titleImg->getPositionX()+5, titleImg->getPositionY() - 3));
		pTopTip->setColor(ccc3(135, 17, 0));
	}

	//menu
	CCMenu* npcMenu = CCMenu::create();
	bg->addChild(npcMenu, 1, Tag_Npc_Menu);
	npcMenu->setPosition(CCPointZero);
	//刚打完一个任务时， 弹出npc提示奖励   menu 只显示  下一关
	if (mChatSection == 5 && mCurDifficulty == difficult_normal && TaskManager::getManager()->getCurrentFightTaskId() != -1)
	{
		CCMenuItemImage* xiyiguan = CCMenuItemImage::create(
			"zhandou_ditu_renwushuoming_xiayiguan.png"
			, "zhandou_ditu_renwushuoming_xiayiguan_select.png"
			, this, menu_selector(MapsLayer::menuItemClicked_NpcDetailBack));
		npcMenu->addChild(xiyiguan);
		xiyiguan->setPosition(ccp(bg->getContentSize().width/2, bgSize.height - 449));
		if (mDiaoLuoRsp.diaoluo_vec.size() > 0 || mDiaoLuoRsp.items.size() > 0) xiyiguan->setPositionY(bgSize.height - 562);
	}
	else 
	{
		//开始
		CCMenuItemImage* kaishi = CCMenuItemImage::create(
			"zhandou_ditu_renwushuoming_kaishi.png"
			, "zhandou_ditu_renwushuoming_kaishi_select.png"
			, this, menu_selector(MapsLayer::menuItemClicked_BeginFight));
		npcMenu->addChild(kaishi, 1, Tag_NpcMenuItem_KaiShi);
		kaishi->setPosition(ccp(bg->getContentSize().width/2, bgSize.height - 449));
		if (mDiaoLuoRsp.diaoluo_vec.size() > 0 || mDiaoLuoRsp.items.size() > 0) kaishi->setPositionY(bgSize.height - 562);
		//扫荡
		if (mCurDifficulty == difficult_normal &&
			(curTaskId < mMainId || (curTaskId == mMainId && mMainState >= TaskStatus_Finished)))
		{
			int iId = ((CCMenuItem*)m_task_menuItem)->getTag(); //地狱副本 需要 + 100
			
			if (iId > 8 && mTaskRate.at(iId-1).star >= 3 && iId != 15 && iId != 25 && iId !=35 && iId != 40)
			{
				kaishi->setPositionX(462);
				CCMenuItemImage *itemSaoDang = CCMenuItemImage::create(
					"zhandou_ditu_renwushuoming_saodang.png",
					"zhandou_ditu_renwushuoming_saodang_select.png",
					"zhandou_ditu_renwushuoming_saodang_suoding.png",
					this, menu_selector(MapsLayer::menuItemClicked_SaoDang));
				npcMenu->addChild(itemSaoDang, 1, Tag_SaoDang_Item);
				itemSaoDang->setPosition(ccp(178, bgSize.height - 449));
				if (mDiaoLuoRsp.diaoluo_vec.size() > 0 || mDiaoLuoRsp.items.size() > 0) itemSaoDang->setPositionY(bgSize.height - 562);
			}
		}
	}

	//关卡通关奖励说明
	for (int i=0; i!=3; ++i)
	{
		BaseSprite *kuang;
		if (i == 0) kuang = BaseSprite::create("ditu_renwu_tanchukuang_tongguan_bg_tong.png");
		else if (i == 1) kuang = BaseSprite::create("ditu_renwu_tanchukuang_tongguan_bg_yin.png");
		else if (i == 2) kuang = BaseSprite::create("ditu_renwu_tanchukuang_tongguan_bg_jin.png");
		bg->addChild(kuang, 0, Tag_NpcAwardBoard_ + i);
		kuang->setPosition(ccp(bgSize.width/2, bgSize.height-201-i*70));
		//星星 、 具体任务 、 奖励 、 状态（已领-勾， 可领-item， 进行中。。。）
		BaseSprite *xx = NULL;
		string thingStr, awardStr;
		if (i == 0)			
		{
			//
			//if (rsp_bonus->get_stat >= 1)	xx = BaseSprite::create("gongjiang_qianghua_jieguo_tong.png");
			if (mTaskRate.at(curTaskId-1).star >= 1) xx = BaseSprite::create("gongjiang_qianghua_jieguo_tong.png");
			else xx = BaseSprite::create();
			//
			if (!bChange)
			{
				thingStr = LFStrings::getValue("TongGuan_thing1_tong");
			}
			else
			{
				thingStr = LFStrings::getValue("TongGuan_tong");
			}
			//
			if (rsp_bonus->exp1 > 0) 
			{
				awardStr = LFStrings::getValue("jingyan");
				sprintf(buf, " %u   ", rsp_bonus->exp1);
				awardStr += buf;
			}
			for (int num=0; num!=rsp_bonus->items1.size(); ++num)
			{
				awardStr += CS::getItemName(rsp_bonus->items1.at(num).type);
				sprintf(buf, " %u   ", rsp_bonus->items1.at(num).count);
				awardStr += buf;
			}
			//状态
			//if (mTaskRate.at(curTaskId-1) < 1)   //进行中
			if (mTaskRate.at(curTaskId-1).star < 1)   //进行中
			{
				CCLabelTTF *label = CCLabelTTF::create((LFStrings::getValue("jinxingzhong")+"...").c_str(), fontStr_kaiti, 20);
				kuang->addChild(label);
				label->setColor(ccc3(195, 145, 96));
				label->setAnchorPoint(ccp(0, 0.5f));
				label->setPosition(ccp(416, kuang->getContentSize().height-42));
			}
			//else if (rsp_bonus->get_stat < 1 && mTaskRate.at(curTaskId-1) >= 1)  //可领取
			else if (mTaskRate.at(curTaskId-1).star >= 1  && !mTaskRate.at(curTaskId-1).is_get_cuprum)  //可领取
			{
				CCMenuItemImage *lingqu = CCMenuItemImage::create(
					"ditu_renwu_anniu_lingjiang.png",
					"ditu_renwu_anniu_lingjiang_select.png",
					this, menu_selector(MapsLayer::menuItemClicked_GetAward));
				npcMenu->addChild(lingqu, 1, Tag_LingJiang_Item_RenWu + i);
				lingqu->setPosition(bg->convertToNodeSpace(kuang->convertToWorldSpace(ccp(458, kuang->getContentSize().height-41))));
				lingqu->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(0.75f), CCFadeIn::create(0.75f), NULL)));
				//
				BaseSprite *xxEffect = BaseSprite::create("gongjiang_qianghua_jieguo_tong.png");
				kuang->addChild(xxEffect);
				xxEffect->setPosition(ccp(41, 75-34));
				xxEffect->setOpacity(0);
				xxEffect->setScale(6);
				xxEffect->runAction(CCSequence::create(
					CCDelayTime::create(0.1*i), CCSpawn::create(CCFadeTo::create(0.15f, 255), CCScaleTo::create(0.25f, 1), NULL), NULL));
			}
			//else //if (rsp_bonus->get_stat >= 1)   //已领取
			else if (mTaskRate.at(curTaskId-1).is_get_cuprum)
			{
				BaseSprite *gou = BaseSprite::create("ditu_renwu_tanchukuang_wancheng.png");
				kuang->addChild(gou);
				gou->setTag(Tag_Lingjiang_Wacheng);
				gou->setPosition(ccp(474, kuang->getContentSize().height-36));
			}
		}
		else if (i == 1)	
		{
			//if (rsp_bonus->get_stat >= 2)	
			if (mTaskRate.at(curTaskId-1).star >= 2)
				xx = BaseSprite::create("gongjiang_qianghua_jieguo_yin.png");
			else xx = BaseSprite::create();
			if (!bChange)
			{
				thingStr = LFStrings::getValue("TongGuan_thing2_yin");
			}
			else
			{
				thingStr = LFStrings::getValue("TongGuan_yin");
			}
			if (rsp_bonus->exp2 > 0) 
			{
				awardStr = LFStrings::getValue("jingyan");
				sprintf(buf, " %u   ", rsp_bonus->exp2);
				awardStr += buf;
			}
			for (int num=0; num!=rsp_bonus->items2.size(); ++num)
			{
				awardStr += CS::getItemName(rsp_bonus->items2.at(num).type);
				sprintf(buf, " %u   ", rsp_bonus->items2.at(num).count);
				awardStr += buf;
			}
			//状态
			//if (mTaskRate.at(curTaskId-1) < 2)   //进行中
			if (mTaskRate.at(curTaskId-1).star < 2)
			{
				CCLabelTTF *label = CCLabelTTF::create((LFStrings::getValue("jinxingzhong")+"...").c_str(), fontStr_kaiti, 20);
				kuang->addChild(label);
				label->setColor(ccc3(195, 145, 96));
				label->setAnchorPoint(ccp(0, 0.5f));
				label->setPosition(ccp(416, kuang->getContentSize().height-42));
			}
			//else if (rsp_bonus->get_stat < 2 && mTaskRate.at(curTaskId-1) >= 2)  //可领取
			else if (mTaskRate.at(curTaskId-1).star >= 2 && !mTaskRate.at(curTaskId-1).is_get_silver)
			{
				CCMenuItemImage *lingqu = CCMenuItemImage::create(
					"ditu_renwu_anniu_lingjiang.png",
					"ditu_renwu_anniu_lingjiang_select.png",
					this, menu_selector(MapsLayer::menuItemClicked_GetAward));
				npcMenu->addChild(lingqu, 1, Tag_LingJiang_Item_RenWu + i);
				lingqu->setPosition(bg->convertToNodeSpace(kuang->convertToWorldSpace(ccp(458, kuang->getContentSize().height-41))));
				lingqu->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(0.75f), CCFadeIn::create(0.75f), NULL)));
				//
				BaseSprite *xxEffect = BaseSprite::create("gongjiang_qianghua_jieguo_yin.png");
				kuang->addChild(xxEffect);
				xxEffect->setPosition(ccp(41, 75-34));
				xxEffect->setOpacity(0);
				xxEffect->setScale(6);
				xxEffect->runAction(CCSequence::create(
					CCDelayTime::create(0.1*i), CCSpawn::create(CCFadeTo::create(0.15f, 255), CCScaleTo::create(0.25f, 1), NULL), NULL));
			}
			//else //if (rsp_bonus->get_stat >= 1)   //已领取
			else if (mTaskRate.at(curTaskId-1).is_get_silver)
			{
				BaseSprite *gou = BaseSprite::create("ditu_renwu_tanchukuang_wancheng.png");
				kuang->addChild(gou);
				gou->setTag(Tag_Lingjiang_Wacheng);
				gou->setPosition(ccp(474, kuang->getContentSize().height-36));
			}
		}
		else if (i == 2)
		{
			//if (rsp_bonus->get_stat >= 3)	
			if (mTaskRate.at(curTaskId-1).star >= 3)
				xx = BaseSprite::create("gongjiang_qianghua_jieguo_jin.png");
			else xx = BaseSprite::create();
			if (!bChange)
			{
				thingStr = LFStrings::getValue("TongGuan_thing3_jin");
			}
			else
			{
				thingStr = LFStrings::getValue("TongGuan_jin");
			}
			if (rsp_bonus->exp3 > 0) 
			{
				awardStr = LFStrings::getValue("jingyan");
				sprintf(buf, " %u   ", rsp_bonus->exp3);
				awardStr += buf;
			}
			for (int num=0; num!=rsp_bonus->items3.size(); ++num)
			{
				awardStr += CS::getItemName(rsp_bonus->items3.at(num).type);
				sprintf(buf, " %u   ", rsp_bonus->items3.at(num).count);
				awardStr += buf;
			}
			//状态
			//if (mTaskRate.at(curTaskId-1) < 3)   //进行中
			if (mTaskRate.at(curTaskId-1).star < 3)
			{
				CCLabelTTF *label = CCLabelTTF::create((LFStrings::getValue("jinxingzhong")+"...").c_str(), fontStr_kaiti, 20);
				kuang->addChild(label);
				label->setColor(ccc3(195, 145, 96));
				label->setAnchorPoint(ccp(0, 0.5f));
				label->setPosition(ccp(416, kuang->getContentSize().height-42));
			}
			//else if (rsp_bonus->get_stat < 3 && mTaskRate.at(curTaskId-1) >= 3)  //可领取
			else if (mTaskRate.at(curTaskId-1).star >= 3  && !mTaskRate.at(curTaskId-1).is_get_gold)
			{
				CCMenuItemImage *lingqu = CCMenuItemImage::create(
					"ditu_renwu_anniu_lingjiang.png",
					"ditu_renwu_anniu_lingjiang_select.png",
					this, menu_selector(MapsLayer::menuItemClicked_GetAward));
				npcMenu->addChild(lingqu, 1, Tag_LingJiang_Item_RenWu + i);
				lingqu->setPosition(bg->convertToNodeSpace(kuang->convertToWorldSpace(ccp(458, kuang->getContentSize().height-41))));
				lingqu->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(0.75f), CCFadeIn::create(0.75f), NULL)));
				//
				BaseSprite *xxEffect = BaseSprite::create("gongjiang_qianghua_jieguo_jin.png");
				kuang->addChild(xxEffect);
				xxEffect->setPosition(ccp(41, 75-34));
				xxEffect->setOpacity(0);
				xxEffect->setScale(6);
				xxEffect->runAction(CCSequence::create(
					CCDelayTime::create(0.1*i), CCSpawn::create(CCFadeTo::create(0.15f, 255), CCScaleTo::create(0.25f, 1), NULL), NULL));
			}
			//else //if (rsp_bonus->get_stat >= 1)   //已领取
			else if (mTaskRate.at(curTaskId-1).is_get_gold)
			{
				BaseSprite *gou = BaseSprite::create("ditu_renwu_tanchukuang_wancheng.png");
				kuang->addChild(gou);
				gou->setTag(Tag_Lingjiang_Wacheng);
				gou->setPosition(ccp(474, kuang->getContentSize().height-36));
			}
		}
		kuang->addChild(xx);
		xx->setPosition(ccp(41, 75-34));
		CCLabelTTF *thingLabel = CCLabelTTF::create(thingStr.c_str(), fontStr_kaiti, 20);
		kuang->addChild(thingLabel);
		thingLabel->setAnchorPoint(ccp(0, 0.5f));
		thingLabel->setPosition(ccp(74, 75-29));
		thingLabel->setColor(ccc3(195, 145, 96));
		//奖励
		CCLabelTTF *awardLabel = CCLabelTTF::create(awardStr.c_str(), fontStr_kaiti, 20);
		kuang->addChild(awardLabel);
		awardLabel->setAnchorPoint(ccp(0, 0.5f));
		awardLabel->setPosition(ccp(74, 75-52));
	}

	//掉落    太多就不要完全显示
	if (mDiaoLuoRsp.diaoluo_vec.size() > 0 || mDiaoLuoRsp.items.size() > 0)
	{
		CCMenuItemImage *imgBar = CCMenuItemImage::create(
			"ditu_renwu_tanchukuang_bossdiaoluo_bg.png",
			"ditu_renwu_tanchukuang_bossdiaoluo_bg.png",
			this, menu_selector(MapsLayer::menuItemClicked_NpcEquipLoot));
		npcMenu->addChild(imgBar, 0, Tag_NpcMenuItem_ZBDiaoLuo);
		imgBar->setPosition(ccp(bgSize.width/2, bgSize.height - 457));
		//
		tempStri = "BOSS" + LFStrings::getValue("diaoluo");
		CCLabelTTF *boLabel = CCLabelTTF::create(tempStri.c_str(), fontStr_kaiti, 20);
		boLabel->setColor(ccBLACK);
		boLabel->setPosition(ccp(bgSize.width/2, bgSize.height - 394));
		bg->addChild(boLabel);
		//
		// 装备 道具
		int displayCount = mDiaoLuoRsp.diaoluo_vec.size();
		if (displayCount > 6) displayCount = 6;               //最多展示6个装备
		if (mDiaoLuoRsp.items.size() > 0) displayCount += 1;  //书只用显示一本就OK
		int scap = 70;
		//1. 装备
		for(int i = 0; i!=mDiaoLuoRsp.diaoluo_vec.size() && i!=6; ++i)
		{
			//diwen
			string cardFile = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(
				(ZhuangbeiColour)mDiaoLuoRsp.diaoluo_vec.at(i).zhuangbei_colour, 
				false,//ChuanqiPool::getPool()->isShenBing(mDiaoLuoRsp.diaoluo_vec.at(i).group_id), 
				SuitPool::getPool()->isSuit(mDiaoLuoRsp.diaoluo_vec.at(i).group_id));
			BaseSprite *diwen = BaseSprite::create(cardFile);
			bg->addChild(diwen, 2);

			if (displayCount % 2 == 0)
				diwen->setPositionX(bgSize.width/2 + scap/2 + (i-displayCount/2)*scap);
			else 
				diwen->setPositionX(bgSize.width/2 + (i-displayCount/2)*scap);
			diwen->setPositionY(bgSize.height - 457);
			diwen->setScale(0.6f);
			//equip
			string equipfile = mDiaoLuoRsp.diaoluo_vec.at(i).zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			diwen->addChild(equipImg, 10);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
		}
		//2. 道具    技能书。显示一本就行了
		if (mDiaoLuoRsp.items.size() > 0)
		{
			BaseSprite *di = BaseSprite::create("daoju_kuang_bg.png");
			bg->addChild(di, 2);
			if (displayCount % 2 == 0)
				di->setPosition(ccp(bgSize.width/2 + scap/2 + scap*(displayCount-1-displayCount/2), bgSize.height-457));
			else
				di->setPosition(ccp(bgSize.width/2 + scap*(displayCount-1-displayCount/2), bgSize.height-457));
			di->setScale(0.7f);
			//
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mDiaoLuoRsp.items.at(0)));
			di->addChild(itemImg);
			itemImg->setPosition(ccp(di->getContentSize().width/2, di->getContentSize().height/2));
		}
	}


	///////////////////////////////////////////////////////////////
	//教学
	if (isUI_Guide_MainLine)
	{
		bool isFirstIn = false;
		if (mMainId == 1 && mMainState == TaskStatus_Got)
			isFirstIn = true;

		CCMenuItem *kaishi = (CCMenuItem*)npcMenu->getChildByTag(Tag_NpcMenuItem_KaiShi);
		if(isFirstIn && kaishi != NULL)
		{
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(kaishi->getContentSize(), kaishi->getParent()->convertToWorldSpace(kaishi->getPosition())
				, LFStrings::getValue("diyigerenwudianjikaishi"), NULL, false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
	}
}

void MapsLayer::menuItemClicked_NpcEquipLoot(CCObject *pSender)
{
	{
		dynamic_cast<CCMenu*>(this->getChildByTag(Tag_NpcBianhui_Layer))->setEnabled(false);
		dynamic_cast<CCMenu*>(this->getChildByTag(Tag_Npc_Window)->getChildByTag(Tag_Npc_Menu))->setEnabled(false);
	}

	BaseSprite *dlBg = BaseSprite::create("zhandou_ditu_diaoluochaxun_xuanzerenwu_bg.png");
	addChild(dlBg, 20, Tag_DiaoLuo_Window);
	dlBg->setPosition(ccp(mWinSize.width/2, 17 + dlBg->getContentSize().height/2));
	CCSize size = dlBg->getContentSize();
	//zhangjie    --->   第一章 10 西蒙斯
	char buf[20];
	sprintf(buf, "zhangjie%d", mCurZhangjie);
	string tempStri = LFStrings::getValue(buf);
	sprintf(buf, " %d ", ((CCMenuItem*)m_task_menuItem)->getTag() - (mCurZhangjie-1)*10);
	tempStri += buf;
	tempStri += ChatContentManager::sharedChatContentManager()->getMainTaskNameById(((CCMenuItem*)m_task_menuItem)->getTag());
	CCLabelTTF *titleLabel = CCLabelTTF::create(tempStri.c_str(), fontStr_kaiti, 22);
	dlBg->addChild(titleLabel);
	titleLabel->setColor(ccc3(195, 145, 96));
	titleLabel->setPosition(ccp(size.width/2, 608));
	//3.menu
	CCMenu *menu = CCMenu::create();
	dlBg->addChild(menu, 1, 10);
	menu->setPosition(ccp(0,0));
	//close
	CCMenuItemImage *fhItem = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png", 
		"shuxing_renwu_fanhui_select.png",
		NULL, this, menu_selector(MapsLayer::menuItemDiaoLuo_Item));
	menu->addChild(fhItem, 0, Tag_DiaoLuoItem_FanHui);
	fhItem->setPosition(ccp(dlBg->getContentSize().width/2, 723-658));

	int scap = 114;
	//1.equip
	float scale = 0.23f;
	int eqNum = mDiaoLuoRsp.diaoluo_vec.size();
	for (int i=0; i!=eqNum; ++i)
	{
		//diwen
		string cardFile = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(
			(ZhuangbeiColour)mDiaoLuoRsp.diaoluo_vec.at(i).zhuangbei_colour, 
			false,//ChuanqiPool::getPool()->isShenBing(mDiaoLuoRsp.diaoluo_vec.at(i).group_id), 
			SuitPool::getPool()->isSuit(mDiaoLuoRsp.diaoluo_vec.at(i).group_id));
		CCMenuItemImage *diwen = CCMenuItemImage::create(
			cardFile.c_str(),
			cardFile.c_str(),
			this, menu_selector(MapsLayer::menuItemDiaoLuo_Item));
		diwen->setUserData((void*)i);
		menu->addChild(diwen, 0, Tag_DiaoLuoItem_FanHui + 10 + i);
		
		if (eqNum % 2 == 0)
			diwen->setPositionX(size.width/2 + scap/2 + scap*(i-eqNum/2));
		else 
			diwen->setPositionX(size.width/2 + scap*(i-eqNum/2));
		diwen->setPositionY(467);
		//equip
		string equipfile = mDiaoLuoRsp.diaoluo_vec.at(i).zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		BaseSprite *equipImg = BaseSprite::create(equipfile);
		diwen->addChild(equipImg, 10);
		equipImg->setScale(scale);
		equipImg->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
		//name label
		string name = mDiaoLuoRsp.diaoluo_vec.at(i).zhuangbei_name;
		CCLabelTTF *labelName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, Label_Size_Min, CCSize(96, 0), kCCTextAlignmentCenter);
		diwen->addChild(labelName, 10);
		labelName->setPosition(ccp(diwen->getContentSize().width/2, -labelName->getContentSize().height/2-5));
		ccColor3B namecolor = ImageManager::getManager()->getShowColor(
			(ZhuangbeiColour)mDiaoLuoRsp.diaoluo_vec.at(i).zhuangbei_colour
			, false
			, SuitPool::getPool()->isSuit(mDiaoLuoRsp.diaoluo_vec.at(i).group_id));
		labelName->setColor(namecolor);
	}
	//2.items
	scap = 180;
	int itemNum = mDiaoLuoRsp.items.size();
	for (int i=0; i!=itemNum; ++i)
	{
		//board
		CCMenuItemImage *diwen = CCMenuItemImage::create(
			"daoju_kuang_bg.png",
			"daoju_kuang_bg.png",
			this, NULL);
		diwen->setUserData((void*)(i+eqNum));
		menu->addChild(diwen, 0, Tag_DiaoLuoItem_FanHui + 10 + eqNum + i);
		if (itemNum % 2 == 0)
			diwen->setPositionX(size.width/2 + scap/2 + scap*(i-itemNum/2));
		else 
			diwen->setPositionX(size.width/2 + scap*(i-itemNum/2));
		diwen->setPositionY(234);
		//daoju
		string djFile = ImageManager::getManager()->getItemFilename(mDiaoLuoRsp.items.at(i));
		BaseSprite *djImg = BaseSprite::create(djFile);
		diwen->addChild(djImg, 10);
		djImg->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
		//name label
		string name;
		if(mDiaoLuoRsp.items.at(i) > ItemsType_SkillBook_MinId && mDiaoLuoRsp.items.at(i) < ItemsType_SkillBook_MaxId)
			name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(mDiaoLuoRsp.items.at(i) - ItemsType_SkillBook_MinId));
		else 
			name = CS::getItemName(mDiaoLuoRsp.items.at(i));
		CCLabelTTF *labelName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, Label_Size_Min, CCSize(0, 0), kCCTextAlignmentCenter);
		diwen->addChild(labelName, 10);
		labelName->setPosition(ccp(diwen->getContentSize().width/2, -labelName->getContentSize().height/2-5));
		labelName->setColor(ccc3(127,127,127));
	}
}

void MapsLayer::menuItemDiaoLuo_Item(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_DiaoLuoItem_FanHui)
	{
		{
			CCMenu *menu = dynamic_cast<CCMenu*>(this->getChildByTag(Tag_NpcBianhui_Layer));
			if (menu) menu->setEnabled(true);
			CCSprite *bg = (CCSprite*)this->getChildByTag(Tag_Npc_Window);
			if (bg) 
			{
				menu = dynamic_cast<CCMenu*>(bg->getChildByTag(Tag_Npc_Menu));
				if (menu) menu->setEnabled(true);
			}
		}
		removeChildByTag(Tag_DiaoLuo_Window);
		return;
	}
	else  //点击了装备   查看详情
	{
		int idx = (int)((CCMenuItem*)pSender)->getUserData();
		if (idx >= mDiaoLuoRsp.diaoluo_vec.size())//道具
		{

		}
		else//装备  （点击之后要屏幕 headMenu）
		{
			mHeadMenu->setEnabled(false);
			((CCMenuItem*)pSender)->getParent()->setVisible(false); //Tag_DiaoLuo_Window   menu
			Flat_FubenDiaoluoZBInfo eqData = mDiaoLuoRsp.diaoluo_vec.at((int) ((CCMenuItem*)pSender)->getUserData());
			//
			BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_mianban.png");
			addChild(board, 30, Tag_Board_EquipDetail);
			board->setPosition(ccp(mWinSize.width/2, board->getContentSize().height/2-106));
			{
				Flat_DuanzaoGroupZBInfo info;
				convertFubenDiaoluoZBInfo2DuanzaoGroupZBInfo(eqData, info);
				refreshEquipBoard(board, info);
			}

			CCMenu *menu = CCMenu::create();
			board->addChild(menu, 10, Tag_Menu_EquipDetail);
			menu->setPosition(CCPointZero);
			BaseSprite *norImg = BaseSprite::create("close.png");
			BaseSprite *selImg = BaseSprite::create("close_select.png");
			CCMenuItemSprite *item = CCMenuItemSprite::create(norImg, selImg, NULL, this, menu_selector(MapsLayer::menuItemClicked_equipDetailMenu));
			menu->addChild(item, 0, Tag_MenuItem_EquipDetail_Close);
			item->setPosition(ccp(board->getContentSize().width-44+12, board->getContentSize().height-36+2));
			//底纹
			ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_XiangQing(
				(ZhuangbeiColour)eqData.zhuangbei_colour
				, false
				, SuitPool::getPool()->isSuit(eqData.group_id));
			BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
			diWen->setScale(0.7f);
			board->addChild(diWen);
			diWen->setPosition(ccp(123, 838-240+122));
			// 装备图片
			string equipfile = eqData.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *s = BaseSprite::create(equipfile);
			diWen->addChild(s);
			s->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));
			//套装按钮
			if(SuitPool::getPool()->isSuit(eqData.group_id))
			{
				string norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang.png";
				string selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_select.png";
// 				if (ChuanqiPool::getPool()->isShenBing(eqData.group_id))
// 				{
// 					norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing.png";
// 					selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing_select.png";
// 				}   
				CCMenuItemImage *taozhuangItem = CCMenuItemImage::create(
					norStr.c_str()
					, selStr.c_str()
					, this ,menu_selector(MapsLayer::menuItemClicked_equipDetailMenu));
				menu->addChild(taozhuangItem, 0, Tag_MenuItem_TaoZhuang);
				taozhuangItem->setAnchorPoint(ccp(1, 0));
				taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()
					, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()));
				taozhuangItem->setEnabled(false);

				mCurTaoZhuang_Info = eqData;
			}
		}
	}
}

void MapsLayer::menuItemClicked_equipDetailMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	if(tag == Tag_MenuItem_EquipDetail_Close)
	{
		//
		mHeadMenu->setEnabled(true);
		CCSprite *diaoluoBg = (CCSprite*)getChildByTag(Tag_DiaoLuo_Window);
		if (diaoluoBg) diaoluoBg->getChildByTag(10)->setVisible(true); 
		//
		removeChildByTag(Tag_Board_EquipDetail);
		MainLayerBase::getCurrentMainBase()->getFooter()->setVisible(true);
	}
	else if(tag == Tag_MenuItem_TaoZhuang)
	{
		((CCMenu*)((CCMenuItem*)pSender)->getParent())->setEnabled(false);

		//穿在身上的能触发套装属性的套装件数、 groupId
		vector<unsigned int> _vec;
		EquipmentTaoZhuangXiangQing *layer = new EquipmentTaoZhuangXiangQing(
			mCurTaoZhuang_Info.group_id, mCurTaoZhuang_Info.zhuangbei_name, _vec, this);
		addChild(layer, 40, Tag_Board_TaoZhuangShuxing);
		layer->release();
	}
}

void MapsLayer::callbackInterface_TaoZhuangLayer()
{
	((CCMenu*)getChildByTag(Tag_Board_EquipDetail)->getChildByTag(Tag_Menu_EquipDetail))->setEnabled(true);

	removeChildByTag(Tag_Board_TaoZhuangShuxing);
}

void MapsLayer::rsp_QueryRateChapterBonus(CCObject *msg_f)
{
	removeObserver(MSG_queryRateTaskBonus_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_QueryRateTaskBonusRsp* data = (Flat_QueryRateTaskBonusRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *displayDialog = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(displayDialog);
		return;		
	}

	mTaskRate = ChessBoardStateManager::getManager()->getCurrentRate()->taskRate;
	mChapterRate = ChessBoardStateManager::getManager()->getCurrentRate()->chapterRate;

 	this->showTongguanJiangliUI();
}

void MapsLayer::updateLevelJiangliInfo(CCMenuItem* pItem)
{
	if (!pItem)
	{
		return;
	}

	int curTaskId = ((CCMenuItem*)pItem)->getTag();
	//Flat_QueryRateTaskBonusRsp* rsp_bonus = ChessBoardStateManager::getManager()->getTaskRateBonus(curTaskId);
	//if (!rsp_bonus ||  (curTaskId-1) >= mTaskRate.size())
	if ((curTaskId-1) >= mTaskRate.size())
	{
		return;
	}

	bool bShowTip = false;
	int iStar = mTaskRate.at(curTaskId-1).star;
//	if (rsp_bonus->get_stat > 2 && mTaskRate.at(curTaskId-1) >= 2)
	if (iStar == 3)
	{
		if (!mTaskRate.at(curTaskId-1).is_get_gold || !mTaskRate.at(curTaskId-1).is_get_silver || !mTaskRate.at(curTaskId-1).is_get_cuprum)
		{
			bShowTip = true;
		}
	}
	else if (iStar == 2)
	{
		if (! mTaskRate.at(curTaskId-1).is_get_silver || !mTaskRate.at(curTaskId-1).is_get_cuprum)
		{
			bShowTip = true;
		}
	}
	else if (iStar == 1)
	{
		if (!mTaskRate.at(curTaskId-1).is_get_cuprum)
		{
			bShowTip = true;
		}
	}	


	CCSprite* pTip = (CCSprite*)pItem->getChildByTag(Tag_TongguanjiangliInfo);
	if (pTip)
	{
		if (!bShowTip)
		{
			pTip->removeFromParent();
		}
	}
	else
	{
		
		if (bShowTip)
		{
			//奖励提示
			pTip = CCSprite::create("sphc_shuliang_tishi.png");
			pTip->setPosition(ccp(pItem->getContentSize().width/2,
				pItem->getContentSize().height+pTip->getContentSize().height/2-10));
			pItem->addChild(pTip, 1, Tag_TongguanjiangliInfo);
		}
	}

}
void MapsLayer::updateTongGuanJiangliInfo()
{
	if (mCurZhangjie <= 0)
	{
		return;
	}

	bool bShowTip = false;

	CCMenuItemImage* pTongguanItem = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MainMenuItem_TongGuan);
	if (!pTongguanItem)
	{
		return;
	}
	//有通关奖励但是还没有领取
	int rate = 0;
	if (mChapterRate.size() >= mCurZhangjie) 
		rate = mChapterRate.at(mCurZhangjie - 1).star;

	//	if (rsp_bonus->get_stat > 2 && mTaskRate.at(curTaskId-1) >= 2)
	if (rate == 3)
	{
		if (!mChapterRate.at(mCurZhangjie-1).is_get_gold  || !mChapterRate.at(mCurZhangjie-1).is_get_cuprum)
		{
			bShowTip = true;
		}
	}
	else if (rate == 1)
	{
		if (!mChapterRate.at(mCurZhangjie-1).is_get_cuprum)
		{
			bShowTip = true;
		}
	}	


	CCSprite* pTip = (CCSprite*)pTongguanItem->getChildByTag(Tag_TongguanjiangliInfo);
	if (pTip)
	{
		if (!bShowTip)
		{
			pTip->removeFromParent();
		}
	}
	else
	{
		if (bShowTip)
		{
			//奖励提示
			pTip = CCSprite::create("sphc_shuliang_tishi.png");

			pTip->setPosition(ccp(pTongguanItem->getContentSize().width/2,
				pTongguanItem->getContentSize().height+pTip->getContentSize().height/2-10));
			pTongguanItem->addChild(pTip, 1, Tag_TongguanjiangliInfo);
		}
	}
}

void MapsLayer::showTongguanJiangliUI()
{
	Flat_QueryRateTaskBonusRsp *rsp_bonus = ChessBoardStateManager::getManager()->getSeasonRateBonus(mCurZhangjie);
	if (! rsp_bonus) return;

	//弹出任务信息框
	BaseSprite* bg = BaseSprite::create("ditu_tongguanjiangli_bg.png");
	addChild(bg, 10, Tag_Npc_Window);
	bg->setAnchorPoint(ccp(0.5f, 1));
	bg->setPosition(ccp(320, 748 + 8));
	//地图背景 变灰，不可触摸
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	addChild(menu, 9, Tag_NpcBianhui_Layer); 
	CCSprite* touming1 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,640,736-bg->getContentSize().height+28));
	CCSprite* touming2 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,640,736-bg->getContentSize().height+28));
	CCMenuItemSprite* touming = CCMenuItemSprite::create(touming1, touming2, this, menu_selector(MapsLayer::menuItemClicked_NpcDetailBack));
	menu->addChild(touming);
	touming->setPosition(ccp(320, 18+(736-bg->getContentSize().height)/2.0f));
	touming->setOpacity(0);
	touming->runAction(CCFadeTo::create(0.05f, 160));
	//
	disableAllTouchBegin();

	CCString temp;
	//zhangjie
	char buf[20];
	sprintf(buf, "zhangjie%d", mCurZhangjie);
	string title = LFStrings::getValue(buf);
	sprintf(buf, "zhangjieName%d", mCurZhangjie);
	title = title + "  " + LFStrings::getValue(buf);
	CCLabelTTF *titleLabel = CCLabelTTF::create(title.c_str(), fontStr_kaiti, 24);
	bg->addChild(titleLabel);
	titleLabel->setColor(ccc3(195, 145, 96));
	titleLabel->setPosition(ccp(319, 628-64));
	//title
	title = LFStrings::getValue("TongGuan") + LFStrings::getValue("jiangli");
	CCLabelTTF *tLabel = CCLabelTTF::create(title.c_str(), fontStr_kaiti, 20);
	bg->addChild(tLabel);
	tLabel->setPosition(ccp(319, 628-120));
	tLabel->setColor(ccc3(195, 145, 96));

	//普通章节通关
	for (int i=0; i!=rsp_bonus->items1.size(); ++i)
	{
		BaseSprite *kuang = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(rsp_bonus->items1.at(i).type).c_str());
		bg->addChild(kuang);
//		kuang->setScale(0.5f);
//		kuang->setPosition(ccp(125, 628-280-i*60));
		//kuang->setPosition(ccp(405, 628-280-i*60));
		kuang->setPosition(ccp(180, 628-330-i*60));
		//
		BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(rsp_bonus->items1.at(i).type));
		kuang->addChild(itemImg);
		itemImg->setPosition(ccp(kuang->getContentSize().width/2, kuang->getContentSize().height/2));
		//172
		temp.initWithFormat("%s x%u", CS::getItemName(rsp_bonus->items1.at(i).type).c_str(), rsp_bonus->items1.at(i).count);
		CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
		bg->addChild(nameLabel);
		nameLabel->setColor(ccBLACK);
//		nameLabel->setAnchorPoint(ccp(0, 0.5f));
//		nameLabel->setPosition(ccp(172, kuang->getPositionY()));
		nameLabel->setPosition(ccp(kuang->getPositionX(), kuang->getPositionY()-kuang->getScaleY()*kuang->getContentSize().height/2-nameLabel->getContentSize().height/2));
	}
	//完美章节通关
	for (int i=0; i!=rsp_bonus->items3.size(); ++i)
	{
		BaseSprite *kuang = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(rsp_bonus->items3.at(i).type).c_str());
		bg->addChild(kuang);
//		kuang->setScale(0.5f);
		//kuang->setPosition(ccp(405, 628-280-i*60));
		kuang->setPosition(ccp(455, 628-330-i*60));

		//
		BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(rsp_bonus->items3.at(i).type));
		kuang->addChild(itemImg);
		itemImg->setPosition(ccp(kuang->getContentSize().width/2, kuang->getContentSize().height/2));
		//172
		temp.initWithFormat("%s x%u", CS::getItemName(rsp_bonus->items3.at(i).type).c_str(), rsp_bonus->items3.at(i).count);
		CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
		bg->addChild(nameLabel);
		nameLabel->setColor(ccBLACK);
//		nameLabel->setAnchorPoint(ccp(0, 0.5f));
//		nameLabel->setPosition(ccp(450, kuang->getPositionY()));
		nameLabel->setPosition(ccp(kuang->getPositionX(), kuang->getPositionY()-kuang->getScaleY()*kuang->getContentSize().height/2-nameLabel->getContentSize().height/2));
	}
	//ko 字样 ---> 是否已通关   //勾， 是否已领奖
	int rate = 0;
	if (mChapterRate.size() >= mCurZhangjie) rate = mChapterRate.at(mCurZhangjie - 1).star;
	if (rate >= 2)	//金星章节通关的奖可以领  ko
	{
		BaseSprite *koImg = BaseSprite::create("jingjichang_wanjiazhuangtailan_ko.png");
		bg->addChild(koImg);
		koImg->setPosition(ccp(460, 628-369));
		if (mChapterRate.at(mCurZhangjie-1).is_get_gold)  // == 2表示已领取金星的
		{
			BaseSprite *gouImg = BaseSprite::create("ditu_renwu_tanchukuang_wancheng.png");
			bg->addChild(gouImg);
			gouImg->setPosition(ccp(528, 628-460));
		}
	}
	if (rate >= 1)  //铜星章节通关的奖可以领  ko
	{
		BaseSprite *koImg = BaseSprite::create("jingjichang_wanjiazhuangtailan_ko.png");
		bg->addChild(koImg);
		koImg->setPosition(ccp(180, 628-369));
		if (mChapterRate.at(mCurZhangjie-1).is_get_cuprum)  // == 1表示已领取铜星的
		{
			BaseSprite *gouImg = BaseSprite::create("ditu_renwu_tanchukuang_wancheng.png");
			bg->addChild(gouImg);
			gouImg->setPosition(ccp(250, 628-460));
		}
	}
	//还没达到要求的 显示灰色蒙版
	if (rate < 2)
	{
		BaseSprite *zeGaiImg = BaseSprite::create("ditu_tongguanjiangli_cover.png");
		bg->addChild(zeGaiImg, 10);
		zeGaiImg->setPosition(ccp(468, 298));
		zeGaiImg->setOpacity(120);
	}
	if (rate < 1)
	{
		BaseSprite *zeGaiImg = BaseSprite::create("ditu_tongguanjiangli_cover.png");
		bg->addChild(zeGaiImg, 10);
		zeGaiImg->setPosition(ccp(188, 298));
		zeGaiImg->setOpacity(120);
	}
	
	//menu
	CCMenu *npcMenu = CCMenu::create();
	bg->addChild(npcMenu, 1, Tag_Npc_Menu);
	npcMenu->setPosition(ccp(0,0));
	if (   (rate >= 1 && !mChapterRate.at(mCurZhangjie -1).is_get_cuprum)   //铜星以上通关、且没有领取铜星
		|| (rate >= 2 && !mChapterRate.at(mCurZhangjie -1).is_get_gold))  //金星通关，且没有领取金星
	{
		CCMenuItemImage *itemLingJiang = CCMenuItemImage::create(
			"ditu_renwu_anniu_lingjiang_tongguan.png",
			"ditu_renwu_anniu_lingjiang_tongguan_select.png",
			this, menu_selector(MapsLayer::menuItemClicked_GetAward));
		npcMenu->addChild(itemLingJiang, 1, Tag_LingJiang_Item_ZhangJie);
		itemLingJiang->setPosition(ccp(319, 628-566));
	}
	else
	{
		CCMenuItemImage *itemFanHui = CCMenuItemImage::create(
			"shuxing_renwu_fanhui.png",
			"shuxing_renwu_fanhui_select.png",
			this, menu_selector(MapsLayer::menuItemClicked_JiangJieLingJiangFanHui));
		npcMenu->addChild(itemFanHui, 1, Tag_LingJiang_Item_FanHui);
		itemFanHui->setPosition(ccp(319, 628-566));
	}
}

void MapsLayer::rsp_QueryShouHuZhiLing(CCObject *msg)
{
	removeObserver(MSG_queryProtectSpiritRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_QueryProtectSpiritRsp* data = (SPCmd_QueryProtectSpiritRsp*)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	data = NULL;

	mCurSHZLData = ShouHuZhiLingManager::getManager()->getSHZHLData(mCurZhangjie);

	//弹出任务信息框
	BaseSprite* bg = BaseSprite::create("ditu_renwu_tanchukuang_bg.png");
	addChild(bg, 10, Tag_Npc_Window);
	bg->setAnchorPoint(ccp(0.5f, 1));
	bg->setPosition(ccp(320, 748));
	//地图背景 变灰，不可触摸
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	addChild(menu, 9, Tag_NpcBianhui_Layer); 
	CCSprite* touming1 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,640,736-bg->getContentSize().height+20));
	CCSprite* touming2 = CCSprite::create("zhandou_jiugongge_touming.png", CCRect(0,0,640,736-bg->getContentSize().height+20));
	CCMenuItemSprite* touming = CCMenuItemSprite::create(touming1, touming2, this, menu_selector(MapsLayer::menuItemClicked_NpcDetailBack));
	menu->addChild(touming);
	touming->setPosition(ccp(320, 10+(736-bg->getContentSize().height)/2));
	touming->setOpacity(0);
	touming->runAction(CCFadeTo::create(0.05f, 160));
	//
	disableAllTouchBegin();

	CCString temp;
//	BaseSprite *board = BaseSprite::create("ditu_shouweizhiling_bg.png");
//	bg->addChild(board, 0, Tag_ShouHuZhiLingBoard);
//	board->setPosition(ccp(315, 311));
	//zhangjie
	char buf[20];
	sprintf(buf, "zhangjie%d", mCurZhangjie);
	string title = LFStrings::getValue(buf);
	title = title + "  " + LFStrings::getValue("ShouHuZhiLing");
	CCLabelTTF *titleLabel = CCLabelTTF::create(title.c_str(), fontStr_kaiti, 24);
	bg->addChild(titleLabel, 1);
	titleLabel->setColor(ccc3(195, 145, 96));
	titleLabel->setPosition(ccp(319, 628-64));
	//title
// 	title = LFStrings::getValue("ZuiChuShouHu");
// 	if (mCurSHZLData.firstDefendHero.hero_id != INVALID_ID)
// 		title += "(" + string(mCurSHZLData.firstDefendHero.name) + ")";
// 	CCLabelTTF *tLabel = CCLabelTTF::create(title.c_str(), fontStr_kaiti, 20);
// 	board->addChild(tLabel);
// 	tLabel->setPosition(ccp(287, 407));

//	{
// 		CCLabelTTF *la1 = CCLabelTTF::create(LFStrings::getValue("SHZL_ShiLiShuoMing").c_str(), fontStr_kaiti, 20);
// 		bg->addChild(la1);
// 		la1->setColor(fonColor_CiYao);
// 		la1->setPosition(ccp(287, 428-316));
// 		CCLabelTTF *la2 = CCLabelTTF::create(LFStrings::getValue("SHZL_KaiQiShuoMing").c_str(), fontStr_kaiti, 20
// 			, CCSize(320, 0), kCCTextAlignmentLeft);
// 		bg->addChild(la2);
// 		la2->setColor(fonColor_CiYao);
// 		la2->setAnchorPoint(ccp(0,0.5));
// 		la2->setPosition(ccp(247, 428-225));
//	}

	//menu
// 	CCMenu *npcMenu = CCMenu::create();
// 	bg->addChild(npcMenu, 1, Tag_Npc_Menu);
// 	npcMenu->setPosition(ccp(0,0));
// 	CCMenuItemImage *itemLingJiang = CCMenuItemImage::create(
// 		"ditu_shouweizhiling_anniu_guize.png",
// 		"ditu_shouweizhiling_anniu_guize_select.png",
// 		this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
// 	npcMenu->addChild(itemLingJiang, 1, Tag_ShouHuZhiLing_GuiZe);
// 	itemLingJiang->setPosition(ccp(319, 628-486));
// 	if (mCurSHZLData.currentDefendHero.isHero() 
// 		&& mCurSHZLData.currentDefendHero.heroId == MyselfManager::getManager()->getMyZhuJueData()->getId())  //shi wo 自己  
// 	{
// 		CCMenuItemImage *itemLingJiang = CCMenuItemImage::create(
// 			"ditu_renwu_anniu_lingjiang_tongguan.png",
// 			"ditu_renwu_anniu_lingjiang_tongguan_select.png",
// 			this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
// 		npcMenu->addChild(itemLingJiang, 1, Tag_ShouHuZhiLing_LingJiang);
// 		itemLingJiang->setPosition(ccp(319, 628-566));
// 	}
// 	else
// 	{
// 		CCMenuItemImage *mobaiItem = CCMenuItemImage::create(
// 			"ditu_shouweizhiling_anniu_mobai.png",
// 			"ditu_shouweizhiling_anniu_mobai_select.png",
// 			this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
// 		npcMenu->addChild(mobaiItem, 1, Tag_ShouHuZhiLing_MoBai);
// 		mobaiItem->setPosition(ccp(bg->getContentSize().width/3, 628-566));
// 		CCMenuItemImage *tiaozItem = CCMenuItemImage::create(
// 			"ditu_shouweizhiling_anniu_tiaozhan.png",
// 			"ditu_shouweizhiling_anniu_tiaozhan_select.png",
// 			"ditu_shouweizhiling_anniu_suoding.png",
// 			this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
// 		npcMenu->addChild(tiaozItem, 1, Tag_ShouHuZhiLing_TiaoZhan);
// 		tiaozItem->setPosition(ccp(bg->getContentSize().width/3*2, 628-566));
// 
// 
// 		int myLv = MyselfManager::getManager()->getMyZhuJueData()->getLevel();
// 		if (mCurSHZLData.minRquireLevel <= myLv && mCurSHZLData.maxRquireLevel >= myLv)
// 		{
// 			if (ShouHuZhiLingManager::EC_OK != ShouHuZhiLingManager::getManager()->canIChlgSHZL(mCurZhangjie))  //等级够了 却不能挑战  --->  未完美通关
// 			{
// 				tiaozItem->setEnabled(false);
// 			}
// 			if (mCurSHZLData.isWorshiped)
// 			{
// 				mobaiItem->setVisible(false);
// 				tiaozItem->setPosition(ccp(bg->getContentSize().width/2, 628-566));
// 			}
// 		}
// 		else //等级不在该守护之灵参与范围内
// 		{
// 			mobaiItem->setVisible(false);
// 			tiaozItem->setPosition(ccp(bg->getContentSize().width/2, 628-566));
// 		}
// 	}

	//hero info
	{

		//背景
		BaseSprite* pBg = BaseSprite::create("ditu_shouweizhiling_bg.png");
		bg->addChild(pBg, 1, Tag_ShouHuZhiLingBoard);
//		pBg->setScale(0.75f);
		pBg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));

		//顶部文字显示
		CCLabelTTF* pTopTip01 = CCLabelTTF::create(LFStrings::getValue("ShouHuZhiLing_TopTip_01").c_str(), fontStr_kaiti, 24);
		pTopTip01->setAnchorPoint(ccp(0, 0));
		pTopTip01->setPosition(ccp(pBg->getContentSize().width/2-pTopTip01->getContentSize().width/2,
			pBg->getContentSize().height-40-pTopTip01->getContentSize().height));
		pTopTip01->setColor(ccc3(221,171,20));
		pBg->addChild(pTopTip01);

		CCLabelTTF* pTopTip02 = CCLabelTTF::create(LFStrings::getValue("ShouHuZhiLing_TopTip_02").c_str(), fontStr_kaiti, 24);
		pTopTip02->setAnchorPoint(ccp(0, 0));
		pTopTip02->setPosition(ccp(pTopTip01->getPositionX()-pTopTip02->getContentSize().width/2+10,
			pTopTip01->getPositionY()-pTopTip02->getContentSize().height - 5));
		pTopTip02->setColor(fonColor_PuTong);
		pBg->addChild(pTopTip02);

		CCLabelTTF* pTopTipUsername = CCLabelTTF::create(mCurSHZLData.firstDefendHero.name, fontStr_kaiti, 25);
		pTopTipUsername->setAnchorPoint(ccp(0, 0));
		pTopTipUsername->setPosition(ccp(pTopTip02->getPositionX()+pTopTip02->getContentSize().width,
			pTopTip02->getPositionY()));
		pTopTipUsername->setColor(fonColor_FaGuang);
		pBg->addChild(pTopTipUsername);

		CCLabelTTF* pTopTip03 = CCLabelTTF::create(LFStrings::getValue("ShouHuZhiLing_TopTip_03").c_str(), fontStr_kaiti, 24);
		pTopTip03->setAnchorPoint(ccp(0, 0));
		pTopTip03->setPosition(ccp(pTopTipUsername->getPositionX()+pTopTipUsername->getContentSize().width, pTopTipUsername->getPositionY()));
		pTopTip03->setColor(fonColor_PuTong);
		pBg->addChild(pTopTip03);


		CCMenu* pMenu = CCMenu::create();
		bg->addChild(pMenu, 0, Tag_Npc_Menu);
		pMenu->setPosition(CCPointZero);

		//头像
//		BaseSprite * headImg = NULL;
		std::string name = "touxiang_guaiwu_boss.png";
		if (mCurSHZLData.currentDefendHero.profession <= 10)
// 			headImg = BaseSprite::create(ImageManager::getManager()->getImageData_ProfessionHead(
// 			(Profession)mCurSHZLData.currentDefendHero.profession).getCurrentImageFile());
		{
			name = ImageManager::getManager()->getImageData_ProfessionHead(
				(Profession)mCurSHZLData.currentDefendHero.profession).getCurrentImageFile();
		}
//		else 
//			headImg = BaseSprite::create("touxiang_guaiwu_boss.png");
//		pBg->addChild(headImg, -1);
//		headImg->setPosition(ccp(pBg->getContentSize().width/2, 400));

		//头像按钮
		CCMenuItemImage *pTouxiang = CCMenuItemImage::create(
			name.c_str(),
			name.c_str(),
			name.c_str(),
			this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
		pMenu->addChild(pTouxiang, 1, Tag_ShouHuZhiLing_HeroImg);
		pTouxiang->setPosition(ccp(bg->getContentSize().width/2, pBg->getPositionY()+pTouxiang->getContentSize().height/2 - 25));
//		pTouxiang->setScale(0.75f);
		pTouxiang->setUserData((void*)INVALID_ID);  //id
		if (mCurSHZLData.currentDefendHero.isHero())
		{
			pTouxiang->setUserData((void*)mCurSHZLData.currentDefendHero.heroId);  //id
		}

		//当前守护者
		CCLabelTTF* pCurShouhuzhe = CCLabelTTF::create(LFStrings::getValue("Shouhuzhiling_dangqianshouhuzhe").c_str(), fontStr_kaiti, 24);
		pBg->addChild(pCurShouhuzhe);	
		pCurShouhuzhe->setPosition(ccp(pBg->getContentSize().width/2, pBg->getContentSize().height/2 - pCurShouhuzhe->getContentSize().height - 75));
 
		//玩家名字
		string zhuangBeiStr = mCurSHZLData.currentDefendHero.name;
		CCLabelTTF* pName = CCLabelTTF::create(zhuangBeiStr.c_str(), fontStr_kaiti, 30);
		pName->setPosition(ccp(pBg->getContentSize().width/2, pBg->getContentSize().height/2 + pName->getContentSize().height - 85));
		pBg->addChild(pName);
		pName->setColor(fonColor_FaGuang);

		//已守护天数
		CCLabelTTF* pTianshu = CCLabelTTF::create(CCString::createWithFormat(LFStrings::getValue("Shouhuzhiling_Yishouhutianshu").c_str(), 
			ShouHuZhiLingManager::getManager()->getCurrentDefenderDay(mCurZhangjie))->getCString(), fontStr_kaiti, 26);
		pTianshu->setPosition(ccp(pBg->getContentSize().width/2, 50));
		pBg->addChild(pTianshu);
		pTianshu->setColor(fonColor_ChengGong);

		//膜拜按钮
		CCMenuItemImage *mobaiItem = CCMenuItemImage::create(
			"ditu_shouweizhiling_anniu_mobai.png",
			"ditu_shouweizhiling_anniu_mobai_select.png",
			this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
//		pMenu->addChild(mobaiItem, 1, Tag_ShouHuZhiLing_MoBai);
		mobaiItem->setPosition(ccp(bg->getContentSize().width/3, 628-566));

		//挑战按钮
		CCMenuItemImage *tiaozItem = CCMenuItemImage::create(
			"ditu_shouweizhiling_anniu_tiaozhan.png",
 			"ditu_shouweizhiling_anniu_tiaozhan_select.png",
			"ditu_shouweizhiling_anniu_suoding.png",
			this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
		tiaozItem->setPosition(ccp(bg->getContentSize().width/3*2,mobaiItem->getPositionY()));


		int myLv = MyselfManager::getManager()->getMyZhuJueData()->getLevel();
		if (mCurSHZLData.minRquireLevel <= myLv && mCurSHZLData.maxRquireLevel >= myLv)
		{
			if (ShouHuZhiLingManager::EC_OK != ShouHuZhiLingManager::getManager()->canIChlgSHZL(mCurZhangjie))  //等级够了 却不能挑战  --->  未完美通关
			{
				tiaozItem->setEnabled(false);
			}
			if (mCurSHZLData.isWorshiped)
			{
				mobaiItem->setVisible(false);
				tiaozItem->setPosition(ccp(bg->getContentSize().width/2, mobaiItem->getPositionY()));
			}
		}
		else //等级不在该守护之灵参与范围内
		{
			mobaiItem->setVisible(false);
			tiaozItem->setPosition(ccp(bg->getContentSize().width/2, mobaiItem->getPositionY()));
		}


		//领取按钮
		CCMenuItemImage *pLingqu = CCMenuItemImage::create(
			"a1.png",
			"a2.png",
			"a3.png",
			this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
		pLingqu->setPosition(ccp(bg->getContentSize().width/2,mobaiItem->getPositionY()));

 	if (mCurSHZLData.currentDefendHero.isHero() 
 		&& mCurSHZLData.currentDefendHero.heroId == MyselfManager::getManager()->getMyZhuJueData()->getId())  //shi wo 自己  
	{
		pMenu->addChild(pLingqu, 1, Tag_ShouHuZhiLing_LingJiang);
	}
	else
	{
		pMenu->addChild(mobaiItem, 1, Tag_ShouHuZhiLing_MoBai);
		pMenu->addChild(tiaozItem, 1, Tag_ShouHuZhiLing_TiaoZhan);
	}

	//规则
	CCMenuItemImage *itemLingJiang = CCMenuItemImage::create(
		"ditu_shouweizhiling_anniu_guize.png",
		"ditu_shouweizhiling_anniu_guize_select.png",
		this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
	pMenu->addChild(itemLingJiang, 1, Tag_ShouHuZhiLing_GuiZe);
	itemLingJiang->setPosition(ccp(pBg->getContentSize().width-itemLingJiang->getContentSize().width/2 - 20, 628-486));
// 		BaseSprite * headImg = NULL;
// 		if (mCurSHZLData.currentDefendHero.profession <= 10)
// 			headImg = BaseSprite::create(ImageManager::getManager()->getImageData_ProfessionHead(
// 				(Profession)mCurSHZLData.currentDefendHero.profession).getCurrentImageFile());
// 		else 
// 			headImg = BaseSprite::create("touxiang_guaiwu_boss.png");
// 		bg->addChild(headImg);
// 		headImg->setPosition(ccp(322, 428-119));
// 		headImg->setScale(0.65f);
// 		BaseSprite *kuang = BaseSprite::create("renxing_renwu_touxiang_select.png");
// 		headImg->addChild(kuang);
// 		kuang->setPosition(ccp(headImg->getContentSize().width/2, headImg->getContentSize().height/2));
		//name
// 		CCLabelTTF *nameLabel = CCLabelTTF::create(mCurSHZLData.currentDefendHero.name.c_str(), fontStr_kaiti, 20);
// 		bg->addChild(nameLabel);
// 		nameLabel->setAnchorPoint(ccp(0, 0.5));
// 		nameLabel->setPosition(ccp(368, 428-96));
// 		nameLabel->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Xiyou, false, false));
		//lv
// 		temp.initWithFormat("Lv%u", mCurSHZLData.currentDefendHero.level);
// 		CCLabelTTF *lvLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
// 		bg->addChild(lvLabel);
// 		lvLabel->setAnchorPoint(ccp(0, 0.5));
// 		lvLabel->setPosition(ccp(368, 428-120));
// 		//连续守护
// 		temp.initWithFormat("%s%u%s", LFStrings::getValue("LianXuShouHu").c_str(), 
// 			ShouHuZhiLingManager::getManager()->getCurrentDefenderDay(mCurZhangjie), LFStrings::getValue("tian").c_str());
// 		CCLabelTTF *la = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
// 		la->setAnchorPoint(ccp(0, 0.5));
// 		bg->addChild(la);
// 		la->setPosition(ccp(368, 428-146));

// 		//menu
// 		CCMenuItemImage *item = CCMenuItemImage::create(
// 			"renxing_renwu_touxiang_select.png", 
// 			"renxing_renwu_touxiang_select.png",
// 			this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
// 		item->setOpacity(0);
// 		item->setScaleY(0.7f);
// 		item->setScaleX(2);
// 		npcMenu->addChild(item, 1, Tag_ShouHuZhiLing_HeroImg);
// 		item->setPosition(ccp(436, 428-19));
// 		item->setUserData((void*)INVALID_ID);  //id
// 		if (mCurSHZLData.currentDefendHero.isHero())
// 			item->setUserData((void*)mCurSHZLData.currentDefendHero.heroId);  //id
	}
}

void MapsLayer::menuItemClicked_JiangJieLingJiangFanHui(CCObject *pSender)
{
	menuItemClicked_NpcDetailBack(0);
}

void MapsLayer::menuItemClicked_ShouHuZhiLing(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_ShouHuZhiLing_GuiZe)
	{
		this->disableAllTouchBegin();
		CCSprite *bg = (CCSprite*)this->getChildByTag(Tag_Npc_Window);
		if (bg && bg->getChildByTag(Tag_Npc_Menu) && bg->getChildByTag(Tag_ShouHuZhiLingBoard))
		{
			bg->getChildByTag(Tag_Npc_Menu)->setVisible(false);
			bg->getChildByTag(Tag_ShouHuZhiLingBoard)->setVisible(false);

			BaseSprite *board = BaseSprite::create("ditu_renwu_tanchukuang_bg.png");
			bg->addChild(board, 10, Tag_ShouHuZhiLing_GuiZeBoard);
			board->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));

			CCTableView* pTableView = CCTableView::create(this, CCSizeMake(board->getContentSize().width - 50, board->getContentSize().height+300));
			pTableView->setDirection(kCCScrollViewDirectionVertical);
			pTableView->setViewSize( CCSizeMake(board->getContentSize().width - 50, board->getContentSize().height - 200));
			pTableView->setPosition(ccp(20, 100));
			board->addChild(pTableView, 3);
			pTableView->reloadData();

			//tilte
			string title = LFStrings::getValue("ShouHuZhiLing");
			title = title + "  " + LFStrings::getValue("GuiZeShuoMing");
			CCLabelTTF *titleLabel = CCLabelTTF::create(title.c_str(), fontStr_kaiti, 24);
			board->addChild(titleLabel);
			titleLabel->setColor(ccc3(195, 145, 96));
			titleLabel->setPosition(ccp(319, 628-64));

// 			//说明1
// 			CCLabelTTF *pShuoming1 = CCLabelTTF::create(LFStrings::getValue("ShouHuZhiLing_GuiZeShuoMing_01").c_str(), fontStr_kaiti, 20
// 				,CCSize(540,0), kCCTextAlignmentCenter);
// 			pShuoming1->setColor(ccBLACK);
// 			board->addChild(pShuoming1);
// 			pShuoming1->setAnchorPoint(ccp(0, 1));
// 			pShuoming1->setPosition(ccp(40, 628-115));
// 
// 			//说明2
// 			CCLabelTTF *pShuoming2 = CCLabelTTF::create(LFStrings::getValue("ShouHuZhiLing_GuiZeShuoMing_02").c_str(), fontStr_kaiti, 20
// 				,CCSize(540,0), kCCTextAlignmentLeft);
// 			pShuoming2->setColor(ccBLACK);
// 			board->addChild(pShuoming2);
// 			pShuoming2->setAnchorPoint(ccp(0, 1));
// 			pShuoming2->setPosition(ccp(pShuoming1->getPositionX(), pShuoming1->getPositionY()-pShuoming1->getContentSize().height - 10));
// 
// 			//shuomning
// 			CCLabelTTF *smLabel = CCLabelTTF::create(LFStrings::getValue("ShouHuZhiLing_GuiZeShuoMing_03").c_str(), fontStr_kaiti, 20
// 				,CCSize(540,0), kCCTextAlignmentLeft);
// 			smLabel->setColor(ccBLACK);
// 			board->addChild(smLabel);
// 			smLabel->setAnchorPoint(ccp(0, 1));
// 			smLabel->setPosition(ccp(pShuoming2->getPositionX(), pShuoming2->getPositionY()-pShuoming2->getContentSize().height));
			//menu
			CCMenu *menu = CCMenu::create();
			board->addChild(menu);
			menu->setPosition(ccp(0,0));
			CCMenuItemImage *fanhui = CCMenuItemImage::create(
				"zhuangbei_xiangxi_fanhui.png",
				"zhuangbei_xiangxi_fanhui_select.png",
				this, menu_selector(MapsLayer::menuItemClicked_ShouHuZhiLing));
			menu->addChild(fanhui, 1, Tag_ShouHuZhiLing_GuiZeFanHui);
			fanhui->setPosition(ccp(319, 628-566));
		}
	}
	else if (tag == Tag_ShouHuZhiLing_GuiZeFanHui)
	{
		CCSprite *bg = (CCSprite*)this->getChildByTag(Tag_Npc_Window);
		if (bg && bg->getChildByTag(Tag_Npc_Menu) && bg->getChildByTag(Tag_ShouHuZhiLingBoard))
		{
			bg->getChildByTag(Tag_Npc_Menu)->setVisible(true);
			bg->getChildByTag(Tag_ShouHuZhiLingBoard)->setVisible(true);
			bg->removeChildByTag(Tag_ShouHuZhiLing_GuiZeBoard);
		}
	}
	else if (tag == Tag_ShouHuZhiLing_TiaoZhan)
	{
		int heroLv = MyselfManager::getManager()->getMyZhuJueData()->getLevel();
		if (ShouHuZhiLingManager::EC_OK != ShouHuZhiLingManager::getManager()->canIChlgSHZL(mCurZhangjie)
			|| (mCurSHZLData.minRquireLevel > heroLv || mCurSHZLData.maxRquireLevel < heroLv))
		{
			CCString temp;//只有主角等级低于%u级的玩家才可挑战该守护之灵。
			temp.initWithFormat(LFStrings::getValue("SHZL_DengJiTishi_01").c_str());//, mCurSHZLData.maxRquireLevel+1);
// 			MessageDisplay *layer = MessageDisplay::create(temp.getCString());
// 			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			UnblockWarnDialog* player = UnblockWarnDialog::create(temp.getCString());
			CCDirector::sharedDirector()->getRunningScene()->addChild(player);
			return;
		}
		else
		{
			addObserver(callfuncO_selector(MapsLayer::rsp_ShouHuZhiLing_TiaoZhan), MSG_challengeProtectSpiritRsp_F);
			mCmdHlp->cmdChallengeProtectSpirit(mCurZhangjie);
		}
	}
	else if (tag == Tag_ShouHuZhiLing_MoBai)
	{
		addObserver(callfuncO_selector(MapsLayer::rsp_ShouHuZhiLing_MoBai), MSG_worshiProtectSpiritRsp);
		mCmdHlp->cmdWorshiProtectSpirit(mCurZhangjie);
	}
	else if (tag == Tag_ShouHuZhiLing_LingJiang)
	{
		addObserver(callfuncO_selector(MapsLayer::rsp_ShouHuZhiLing_LingJiang), MSG_getProtectSpiritBonusRsp);
		mCmdHlp->cmdGetProtectSpiritBonus(mCurZhangjie);
	}
	else if (tag == Tag_ShouHuZhiLing_HeroImg)
	{
		unsigned int heroId = (unsigned int)((CCMenuItem*)pSender)->getUserData();
		if (heroId != INVALID_ID)
		{
			this->addObserver(callfuncO_selector(MapsLayer::rsp_SHZLHero), MSG_queryAnotherTeamInfoRsp_F);
			mCmdHlp->cmdQueryAnotherTeamInfo(heroId);
		}

		this->setShouhuzhilingMenuEnable(false);
	}
}

void MapsLayer::menuItemClicked_GetAward(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if (tag == Tag_LingJiang_Item_ZhangJie)
	{
		//有通关奖励但是还没有领取
		int rate = 0;
		if (mChapterRate.at(mCurZhangjie-1).star >= RateStarLevel_Cuprum)
		{
			if (!mChapterRate.at(mCurZhangjie-1).is_get_cuprum)
			{
				rate = 1;
			}
			else if (!mChapterRate.at(mCurZhangjie-1).is_get_gold)
			{
				rate = 3;
			}
		}

		if (rate > 0)
		{
			addObserver(callfuncO_selector(MapsLayer::rsp_GetedChapterBonus), MSG_getRateTaskBonusRsp);
			mCmdHlp->cmdGetTaskRateBonus(RateStarType_Chapter, (RateStarLevel)rate, mCurZhangjie);
		}
	}
	else //if (tag == Tag_LingJiang_Item_RenWu) 铜+1 银+2 金+3
	{
		int curId = ((CCMenuItem*)m_task_menuItem)->getTag();
		addObserver(callfuncO_selector(MapsLayer::rsp_GetedTaskBonus), MSG_getRateTaskBonusRsp);
		mCmdHlp->cmdGetTaskRateBonus(RateStarType_Task, (RateStarLevel)(tag-Tag_LingJiang_Item_RenWu+1), curId);
	}
}

void MapsLayer::rsp_GetedTaskBonus(CCObject *msg)
{
	removeObserver(MSG_getRateTaskBonusRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_GetRateTaskBonusRsp *data = (SPCmd_GetRateTaskBonusRsp *)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *displayDialog = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(displayDialog);
		return;		
	}

	int curId = ((CCMenuItem*)m_task_menuItem)->getTag();
// 	Flat_QueryRateTaskBonusRsp *rsp_bonus = ChessBoardStateManager::getManager()->getTaskRateBonus(curId);
// 	if (!rsp_bonus)
// 	{
// 		return;
// 	}

	mTaskRate = ChessBoardStateManager::getManager()->getCurrentRate()->taskRate;
	mChapterRate = ChessBoardStateManager::getManager()->getCurrentRate()->chapterRate;

	//refresh board
	BaseSprite *bg = dynamic_cast<BaseSprite*>(getChildByTag(Tag_Npc_Window));
	if (bg != NULL)
	{
		CCMenu *npcMenu = (CCMenu *)bg->getChildByTag(Tag_Npc_Menu);

		//完成图标
		BaseSprite *gouImg = BaseSprite::create("ditu_renwu_tanchukuang_wancheng.png");
//		bg->getChildByTag(Tag_NpcAwardBoard_ + 0)->addChild(gouImg);
		gouImg->setTag(Tag_Lingjiang_Wacheng);
		gouImg->setPosition(ccp(474, 75-36));
		gouImg->setScale(0);
		gouImg->runAction(CCSequence::create(
			CCScaleTo::create(0.5f, 1.15f),
			CCScaleTo::create(0.15f, 1.0f),
			NULL));

		//if (rsp_bonus->get_stat == 1)       //领取铜星奖励
		if (mTaskRate.at(curId-1).is_get_cuprum)       //领取铜星奖励
		{
			if (npcMenu != NULL) 
				npcMenu->removeChildByTag(Tag_LingJiang_Item_RenWu + 0);
			
			CCNode* pNode = bg->getChildByTag(Tag_NpcAwardBoard_ + 0);
			if (pNode && !pNode->getChildByTag(Tag_Lingjiang_Wacheng))
			{
				pNode->addChild(gouImg);
			}

		}
		//else if (rsp_bonus->get_stat == 2)  //领取银星奖励
		if (mTaskRate.at(curId-1).is_get_silver)       //领取银星奖励
		{
			if (npcMenu != NULL)
				npcMenu->removeChildByTag(Tag_LingJiang_Item_RenWu + 1);
			CCNode* pNode = bg->getChildByTag(Tag_NpcAwardBoard_ + 1);
			if (pNode && !pNode->getChildByTag(Tag_Lingjiang_Wacheng))
			{
				pNode->addChild(gouImg);
			}

		}
		//else if (rsp_bonus->get_stat == 3)  //领取金星奖励	
		if (mTaskRate.at(curId-1).is_get_gold)
		{
			if (npcMenu != NULL)
				npcMenu->removeChildByTag(Tag_LingJiang_Item_RenWu + 2);
			CCNode* pNode = bg->getChildByTag(Tag_NpcAwardBoard_ + 2);
			if (pNode && !pNode->getChildByTag(Tag_Lingjiang_Wacheng))
			{
				pNode->addChild(gouImg);
			}
		}
	}

	updateLevelJiangliInfo((CCMenuItem*)m_task_menuItem);
}

void MapsLayer::rsp_GetedChapterBonus(CCObject *msg)
{
	removeObserver(MSG_getRateTaskBonusRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_GetRateTaskBonusRsp *data = (SPCmd_GetRateTaskBonusRsp *)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *displayDialog = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(displayDialog);
		return;		
	}

	mTaskRate = ChessBoardStateManager::getManager()->getCurrentRate()->taskRate;
	mChapterRate = ChessBoardStateManager::getManager()->getCurrentRate()->chapterRate;

	this->updateTongGuanJiangliInfo();

 	Flat_QueryRateTaskBonusRsp *rsp_bonus = ChessBoardStateManager::getManager()->getSeasonRateBonus(mCurZhangjie);
	if (!rsp_bonus)
	{
		return;
	}
// 
	//refresh board
	BaseSprite *bg = dynamic_cast<BaseSprite*>(getChildByTag(Tag_Npc_Window));
	CCMenu *npcMenu = (CCMenu*)bg->getChildByTag(Tag_Npc_Menu);
	if (bg != NULL)
	{
		bool bShowFanHui = false;
		if (mChapterRate.at(mCurZhangjie-1).star == RateStarLevel_Gold)       //领取金星奖励
		{
			if (mChapterRate.at(mCurZhangjie - 1).is_get_cuprum && mChapterRate.at(mCurZhangjie-1).is_get_gold) 
			{
				bShowFanHui = true;
			}
		}
		else if (mChapterRate.at(mCurZhangjie-1).star == RateStarLevel_Cuprum)       //领取铜星奖励
		{
			if (mChapterRate.at(mCurZhangjie - 1).is_get_cuprum) 
			{
				bShowFanHui = true;
			}
			
		}

		int rate = 0;
		if (mChapterRate.at(mCurZhangjie-1).star >= RateStarLevel_Cuprum)
		{
			if (mChapterRate.at(mCurZhangjie-1).is_get_gold)
			{
				rate = RateStarLevel_Gold;
			}
			else if (mChapterRate.at(mCurZhangjie-1).is_get_cuprum)
			{
				rate = RateStarLevel_Cuprum;
			}
		}

		if (rate >= RateStarLevel_Cuprum)
		{
			BaseSprite *gouImg = BaseSprite::create("ditu_renwu_tanchukuang_wancheng.png");
			bg->addChild(gouImg);
			gouImg->setScale(0);
			gouImg->runAction(CCSequence::create(
				CCScaleTo::create(0.5f, 1.15f),
				CCScaleTo::create(0.15f, 1.0f),
				NULL));
			gouImg->setPosition(ccp(250, 628-460));
			if (rate == RateStarLevel_Gold)
			{
				gouImg->setPosition(ccp(528, 628-460));
			}
		}
		if (bShowFanHui)
		{
			npcMenu->removeChildByTag(Tag_LingJiang_Item_ZhangJie);
			CCMenuItemImage *itemFanHui = CCMenuItemImage::create(
				"shuxing_renwu_fanhui.png",
				"shuxing_renwu_fanhui_select.png",
				this, menu_selector(MapsLayer::menuItemClicked_JiangJieLingJiangFanHui));
			npcMenu->addChild(itemFanHui, 1, Tag_LingJiang_Item_FanHui);
			itemFanHui->setPosition(ccp(319, 628-566));
		}

			
			AwardDisplay *layer = AwardDisplay::create(LFStrings::getValue("HuoDe") + LFStrings::getValue("jiangli"),
				rsp_bonus->items1);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
//		}
// 		else if (rsp_bonus->get_stat == 2)  //领取金星奖励
// 		{
// 			npcMenu->removeChildByTag(Tag_LingJiang_Item_ZhangJie);
// 			CCMenuItemImage *itemFanHui = CCMenuItemImage::create(
// 				"shuxing_renwu_fanhui.png",
// 				"shuxing_renwu_fanhui_select.png",
// 				this, menu_selector(MapsLayer::menuItemClicked_JiangJieLingJiangFanHui));
// 			npcMenu->addChild(itemFanHui, 1, Tag_LingJiang_Item_FanHui);
// 			itemFanHui->setPosition(ccp(319, 628-566));
// 
// 			//
// 			BaseSprite *gouImg = BaseSprite::create("ditu_renwu_tanchukuang_wancheng.png");
// 			bg->addChild(gouImg);
// 			gouImg->setTag(Tag_Lingjiang_Wacheng);
// 			gouImg->setPosition(ccp(528, 628-460));
// 			gouImg->setScale(0);
// 			gouImg->runAction(CCSequence::create(
// 				CCScaleTo::create(0.5f, 1.15f),
// 				CCScaleTo::create(0.15f, 1.0f),
// 				NULL));
// 
// 			//
// 			AwardDisplay *layer = AwardDisplay::create(LFStrings::getValue("HuoDe") + LFStrings::getValue("jiangli"),
// 				rsp_bonus->items2);
// 			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
// 		}
	}
}

void MapsLayer::menuItemClicked_BeginFight(CCObject* pSender)
{
	pSender = m_task_menuItem;
	int id = ((CCMenuItem*)pSender)->getTag();
	FightPackage::getPakage()->clearPackage();


///////////////////////////////////////////////////////////////////////////////////////////
	//等级卡点
	//9 级才能进入任务11
	//20              25
	//30              35
	int level = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level;
	if (   (id == 11 && level < 9 )
		|| (id == 25 && level < 20)
		|| (id == 35 && level < 30))
	{
		int needLv;
		if (id == 11)      needLv = 9;
		else if (id == 25) needLv = 20;
		else if (id == 35) needLv = 30;
		CCString temp;
		temp.initWithFormat("%s%d%s%s", LFStrings::getValue("ZhuJue").c_str(), needLv
			, LFStrings::getValue("ji").c_str(), LFStrings::getValue("KaiQi").c_str());
		UnblockWarnDialog *layer = UnblockWarnDialog::create(temp.getCString(), false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
		return;
	}
///////////////////////////////////////////////////////////////////////////////////////////////

	JGG_FootprintManager::getManager()->clearJGGFootprint();
	//1-50
	if (mCurDifficulty == difficult_normal)
	{
		addObserver(SEL_CallFuncO(&MapsLayer::rsp_FightBeginNormal), MSG_GetTaskChessBoardRsp);
		mCmdHlp->cmdGetTaskChessboard(id, true);

//		g_taskId = id;
		TaskManager::getManager()->setCurrentFightTaskId(id);
	}
	//101-150
	else if (mCurDifficulty == difficult_hard)
	{
		addObserver(callfuncO_selector(MapsLayer::rsp_FightBeginHard), MSG_hellTaskBattleRsp_F);
		mCmdHlp->cmdHellTaskBattle(id - 1 + HellTaskId_Min);

// 		g_taskId = id - 1 + HellTaskId_Min;
// 		g_gridIndex = 0;
		TaskManager::getManager()->setCurrentFightTaskId(id - 1 + HellTaskId_Min);
		TaskManager::getManager()->setCurrentFightGridIdx(0);
	}
}

void MapsLayer::menuItemClicked_NpcDetailBack(CCObject* psender)
{
	CCSprite *bg = (CCSprite*)this->getChildByTag(Tag_Npc_Window);
	if (bg && bg->getChildByTag(Tag_Npc_Menu) && bg->getChildByTag(Tag_ShouHuZhiLingBoard))
	{
		if (((CCMenu*)bg->getChildByTag(Tag_Npc_Menu))->isEnabled())
		{
			this->removeChildByTag(Tag_NpcBianhui_Layer);
			this->removeChildByTag(Tag_Npc_Window);
			disableAllTouchEnd();
		}
	}
	else
	{
		this->removeChildByTag(Tag_NpcBianhui_Layer);
		this->removeChildByTag(Tag_Npc_Window);
		disableAllTouchEnd();
	}

	// menuItemClicked_NpcDetailBack之后 判断如果当前state是 finish 则直接发消息！
	if (mChatSection == 5 && mCurDifficulty == difficult_normal && TaskManager::getManager()->getCurrentFightTaskId() != -1
		&& mMainId % 10 == 0 && ! mTongGuanAwardWindowEnd) //如果是章节通关了 还要弹章节奖励的board
	{
		menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_MainMenuItem_TongGuan));
		mTongGuanAwardWindowEnd = true;
	}
	else if (mChatSection == 5 && mCurDifficulty == difficult_normal && TaskManager::getManager()->getCurrentFightTaskId() != -1)
	{
		mChatSection = 0;
		mTongGuanAwardWindowEnd = false;  //mTongGuanAwardWindowEnd 

		addObserver(SEL_CallFuncO(&MapsLayer::rsp_FlushTaskList), MSG_getTaskListRsp);
		mCmdHlp->cmdGetTaskList(1);  //无用的参数
	}
}

void MapsLayer::rsp_FightBeginNormal(CCObject* obj)
{
	removeObserver(MSG_GetTaskChessBoardRsp);
	GameDirector::getDirector()->hideWaiting();
	// 登陆的返回
	SPCmd_GetTaskChessBoardRsp* rsp = (SPCmd_GetTaskChessBoardRsp*)((MSG_Rsp*)obj)->getData();

	if (rsp->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	{
		CCScene *runS = CCDirector::sharedDirector()->getRunningScene();
		runS->removeAllChildren();
		JiuGongGeLayer *jggLayer = JiuGongGeLayer::create(); //传入-1 让位置点在格子外面
		runS->addChild(jggLayer,1, TAG_JiuGongGe_Layer);	
	}

}

void MapsLayer::rsp_FightBeginHard(CCObject *msg_f)
{
	removeObserver(MSG_hellTaskBattleRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_HellTaskBattleRsp* data = (Flat_HellTaskBattleRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}


	vector<FightDirector> dir_vec;
	for (int i=0; i!=data->fightScriptVec.size(); ++i)
	{
		FightDirector dir;
		dir.initScript(data->fightScriptVec.at(i));
		dir_vec.push_back(dir);
	}

	Bundle bundle;
	bundle.putString("JumpToWhenEnd","Default");
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec
		,MapsManager::getManager()->getBackGroundMapImage(TaskManager::getManager()->getCurrentFightTaskId() - HellTaskId_Min + 1),bundle);
}

// 失败，返回一个Code，就是协议定义的SimpleProtocolErrorCode
void MapsLayer::prepareError(int code)
{

	if(code == TiliNotEnought)
	{
		GetTiLiLayer* layer = GetTiLiLayer::create();
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);
	}
	else if (code == CEC_NoMoreFreeSpaceToStoreEquip)
	{
		EquipBagNotEnoughDialog *layer = new EquipBagNotEnoughDialog;
		layer->showGoToFenJieDialog();
	}
	else
	{
		MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);
	}
}

// 提供背景
CCSprite* MapsLayer::getBg(int taskId,int gridIndex)
{
	return MapsManager::getManager()->getBackGroundMapImage(taskId);
}

 void MapsLayer::prepareSuccess()
{
//	TransLationMapsManager::sharedTranslationMapsManager()->setFootprint(g_taskId, 0);
	mFight->fight();	
}

 void MapsLayer::chatEndCallBack()
 {

	 if(mChatSection == 5)  // 如果是完成任务时的对话
	 {
		 mSection5TranslationMap = true;

		 //如果任务已经完成 则自动提交
		 addObserver(SEL_CallFuncO(&MapsLayer::rsp_ChangeTaskState), MSG_ChangeTaskStatRsp_F);
		 mCmdHlp->cmdChangeTaskState(mMainId, TaskStatus_Submitted);
//		 mChatSection = 0;  //下一步清除

		 return;
	 }

	 if(mChatSection == 1)
	 {
		 addObserver(SEL_CallFuncO(&MapsLayer::rsp_ChangeTaskState), MSG_ChangeTaskStatRsp_F);
		 mCmdHlp->cmdChangeTaskState(mMainId, TaskStatus_Got);
 
//		 mChatSection = 0;  //下一步清除
	 }
	 

	 if (g_ChuanQiFuBen_Guide && mCurDifficulty == difficult_hard)
	 {
// 		 CCMenuItemImage* qzItem = (CCMenuItemImage*)mRenwuMenu->getChildByTag(TAG_XinRenWu_QiZhi);
// 		 if (qzItem == NULL) qzItem = (CCMenuItemImage*)mRenwuMenu->getChildByTag(1);
// 		 if(qzItem)
// 		 {
// 			 GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
// 			 guideLayer->init(
// 				 qzItem->getContentSize(),
// 				 qzItem->getParent()->convertToWorldSpace(qzItem->getPosition()), 
// 				 "", NULL, false);
// 			 CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
// 
// 			 qzItem->setVisible(true);
// 			 qzItem->stopAllActions();
// 		 }
	 }
	 else if (g_ChuanQiSuiPianHeCheng_Guide && mCurDifficulty == difficult_hard)
	 {
		 //跳主页碎片合成
		 GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	 }
 }

 void MapsLayer::menuItemClicked_ZhangJie_DiaoLuo(CCObject *pSender)
 {
/*	 disableAllTouchBegin();

	 int tag = ((CCMenuItemImage*)pSender)->getTag();
	 if(tag == Tag_MenuItem_diaoluoCX)
	 {
		 bool bUnblock =  UnblockManager::getManager()->isDeblocking(unblock_DiaoLuoChaXun);
		
		 if (bUnblock)
		 {
			 MapsLayer_DiaoLuoChaXun *layer = MapsLayer_DiaoLuoChaXun::create(mCurZhangjie, this, mCurDifficulty); 
			 this->addChild(layer, 10, Tag_Board_DiaoLuoChaXun);
			 layer->setAnchorPoint(ccp(0,0));
			 layer->setPosition(ccp(13, 13));
		 }
		 else
		 {
			 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("diaoluochaxun_weijiesuo"), false);
			 CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
			 disableAllTouchEnd();
			 return;
		 }
	 }
	 else if(tag == Tag_MenuItem_xuanzeZJ)
	 {
		 //board
		 BaseSprite *board = BaseSprite::create("zhandou_ditu_zhangjiexuanze_bg.png");
		 this->addChild(board, 10, Tag_Board_ZhangJieXuanZe);
		 board->setPosition(ccp(320, 13 + board->getContentSize().height/2));
		 //menu
		 CCMenu *menu = CCMenu::create();
		 board->addChild(menu);
		 menu->setPosition(CCPointZero);
		 //fanhui
		 CCMenuItemImage *fanhui = CCMenuItemImage::create(
			 "shuxing_renwu_fanhui.png"
			 , "shuxing_renwu_fanhui_select.png"
			 , this, menu_selector(MapsLayer::menuItemClicked_zhangjie));
		 menu->addChild(fanhui, 1, Tag_MenuItem_xuanzeZJ_FanHui);
		 fanhui->setPosition(ccp(board->getContentSize().width/2, 54));
		 //
		 int zjCur = (mMainId+9)/10;
		 char buf[20];
		 for(int i=1; i<=4; ++i)
		 {
			 CCMenuItemImage *item = CCMenuItemImage::create(
				 "zhandou_ditu_zhangjiexuanze.png"
				 , "zhandou_ditu_zhangjiexuanze_select.png"
				 , "zhandou_ditu_zhangjiexuanze_suoding.png"
				 , this, menu_selector(MapsLayer::menuItemClicked_zhangjie));
			 menu->addChild(item, 0, i);
			 item->setPosition(ccp(board->getContentSize().width/2, 703-i*130));
			 //
			 sprintf(buf, "zhangjie%d", i);
			 CCLabelTTF *zhang = CCLabelTTF::create(LFStrings::getValue(buf).c_str(), fontStr_kaiti, 20);
			 item->addChild(zhang);
			 zhang->setColor(ccc3(195, 145, 96));
			 zhang->setAnchorPoint(ccp(0, 0.5));
			 zhang->setPosition(ccp(40, item->getContentSize().height/2));
			 //
			 sprintf(buf, "zhangjieName%d", i);
			 CCLabelTTF *name = CCLabelTTF::create(LFStrings::getValue(buf).c_str(), fontStr_kaiti, 20);
			 item->addChild(name);
			 name->setColor(ccc3(195, 145, 96));
			 name->setPosition(ccp(item->getContentSize().width/2, item->getContentSize().height/2));
			 //
			 CCLabelTTF *zhuangtai = CCLabelTTF::create(LFStrings::getValue("yitongguan").c_str(), fontStr_kaiti, 20);
			 item->addChild(zhuangtai);
			 zhuangtai->setColor(ccc3(195, 145, 96));
			 zhuangtai->setAnchorPoint(ccp(1, 0.5));
			 zhuangtai->setPosition(ccp(440, item->getContentSize().height/2));
			 if(i == zjCur)
			 {
				 zhuangtai->setString(LFStrings::getValue("jinxingzhong").c_str());
				 zhuangtai->setColor(ccc3(255, 255, 255));
			 }
			 else if(i > zjCur)
			 {
				 zhuangtai->setString(LFStrings::getValue("weijiesuo").c_str());
				 item->setEnabled(false);
				 zhang->setColor(ccc3(127, 127, 127));
				 name->setColor(ccc3(127, 127, 127));
				 zhuangtai->setColor(ccc3(127, 127, 127));
			 }

			 if(mCurZhangjie == i)
			 {

				 BaseSprite *biankuang = BaseSprite::create("zhandou_ditu_zhangjiexuanze_select_kuang.png");
				 item->addChild(biankuang);
				 biankuang->setPosition(ccp(item->getContentSize().width/2, item->getContentSize().height/2));
			 }
		 }

	 }
	*/
 }

 void MapsLayer::menuItemClicked_zhangjie(CCObject *pSender)
 {
	 if (m_bDisplayWindow_CannotTouch) return;

	 int zhangjie = ((CCMenuItemImage*)pSender)->getTag();

	 if(zhangjie >= 1 && zhangjie <= 4)
	 {		 
		 mCurZhangjie = zhangjie;

		 initCacheMapByIndx(zhangjie);
		 mMapCache->setPosition(getTaskMenuPos(0, mCurZhangjie));
		 mMapCache->setScale(0.1f);
		 mMapCache->setVisible(true);
		 setAllChildrenColorOrOpacity(mMapCache, ccWHITE, 60, 2);

		 nodeAndAllChildRunAction(mMapCache, CCFadeTo::create(0.3f, 255));
		 nodeAndAllChildRunAction(mMap, CCFadeTo::create(0.28f, 60));
		 mMapCache->runAction(CCSequence::create(
			 CCSpawn::create(CCMoveTo::create(0.3f, ccp(mBg->getContentSize().width/2, mMap->getPositionY())),CCScaleTo::create(0.3f, 1), NULL),
			 CCCallFunc::create(this, callfunc_selector(MapsLayer::schedule_resetChapter)),
			 NULL));
	 }
	 this->updateTongGuanJiangliInfo();
 }

 void MapsLayer::schedule_resetChapter()
 {
	 resetZhangJie_AddRefreshUI(mCurZhangjie);
 }

 void MapsLayer::resetZhangJie_AddRefreshUI(int zj)
 {
	 mCurZhangjie = zj;
	 string temp;

	 //title bar
	 CCLabelTTF *curMapnameLabel = (CCLabelTTF*) mTitleBar->getChildByTag(Tag_Label_CurMapName);
	 if (curMapnameLabel)
	 {
		 if (mCurZhangjie == 0)
		 {
			 mMainMenu->setVisible(false);
			 curMapnameLabel->setString(LFStrings::getValue("WorldMapName").c_str());
			 CCSprite *world = (CCSprite*)mTitleBar->getChildByTag(Tag_CircleImg_World);
			 if (world) world->initWithFile("jineng_chufalunci_icon_chufa.png");
			 CCSprite *capture = (CCSprite*)mTitleBar->getChildByTag(Tag_CircleImg_Captrue);
			 if (capture) capture->initWithFile("jineng_chufalunci_icon.png");
		 }
		 else 
		 {
			 mMainMenu->setVisible(true);
			 char buf[20];
			 sprintf(buf, "zhangjie%d", mCurZhangjie);
			 temp = LFStrings::getValue(buf) + LFStrings::getValue("DIAN");
			 sprintf(buf, "zhangjieName%d", mCurZhangjie);
			 temp += LFStrings::getValue(buf).c_str();
			 curMapnameLabel->setString(temp.c_str());
			 CCSprite *world = (CCSprite*)mTitleBar->getChildByTag(Tag_CircleImg_World);
			 if (world) world->initWithFile("jineng_chufalunci_icon.png");
			 CCSprite *capture = (CCSprite*)mTitleBar->getChildByTag(Tag_CircleImg_Captrue);
			 if (capture) capture->initWithFile("jineng_chufalunci_icon_chufa.png");
		 }
	 }

	 if (mCurDifficulty == difficult_hard)
		 mMainMenu->setVisible(false);

	 //ditu
	 {
		 mMap->stopAllActions();
		 mMapCache->stopAllActions();

		 char buf[60];
		 if (mCurZhangjie == 0)
			 sprintf(buf, "ditu_shijie.png");
		 else 
			 sprintf(buf, "ditu_%d.jpg", mCurZhangjie);
		 mMap->initWithFile(buf);
		 mMap->setVisible(true);
		 mMap->setScale(1);
		 setAllChildrenColorOrOpacity(mMap, ccWHITE, 255, 2);
		 mMap->setPosition(ccp(mBg->getContentSize().width/2, 11 + mMap->getContentSize().height/2));
		 mMapCache->setVisible(false);
		 setAllChildrenColorOrOpacity(mMapCache, ccWHITE, 255, 2);
	 }

	 drawRenWu();
	 CenterMainTask(mMainId, mMainState);



	 if (mCurZhangjie > 0)
	 {
		 //normal 的state才能使 finish
		 if(mMainState == TaskStatus_Finished)
		 {
			 mChatSection = 5;

			 ChatLayer* layer = ChatLayer::create(
				 ChatContentManager::sharedChatContentManager()->getMainTaskChatContentById(mMainId, 5), this);
			 if(layer != NULL)
				 CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ChatLayer);
			 // 对话完成后 会直接调用 chatendcallback
		 }

		
		 if(mChatSection == 1)
		 {
			 mChatSection = 0;

			 menuItemClicked_Task(mRenwuMenu->getChildByTag(mMainId));
		 }
	 }

	 this->updateTongGuanJiangliInfo();
 }

 void MapsLayer::initCacheMapByIndx(int zj)
 {
	 CCAssert(zj >= 0 && zj <= 4, "");

	 char buf[60];

	 mMapCache->removeAllChildren();
	 mMapCache->setScale(1);
	 setAllChildrenColorOrOpacity(mMapCache, ccWHITE, 255, 2);
	 if (zj == 0)  //world map
	 {
		 mMapCache->retain();
		 mMapCache->removeFromParent();
		 mBg->addChild(mMapCache, 2);
		 mMapCache->release();

		 sprintf(buf, "ditu_shijie.png");
		 mMapCache->initWithFile(buf);
		 //menu
		 CCPoint pos[4] = {ccp(143, 510), ccp(183, 277), ccp(396, 193), ccp(467, 552)};
		 for (int i=0; i<4; ++i)
		 {
			 char buf[128];
			 CCSprite *item = NULL;
			 if ((mMainId + 9) / 10 < i+1)
			 {
				 sprintf(buf, "ditu_guanqia_act_%d_suoding.png", i+1);
				 item = CCSprite::create(buf);
			 }
			 else
			 {
				 sprintf(buf, "ditu_guanqia_act_%d.png", i+1);
				 item = CCSprite::create(buf);
			 }
			 mMapCache->addChild(item);
			 item->setPosition(getTaskMenuPos(zj, i+1));
		 }
	 }
	 else
	 {
		 mMapCache->retain();
		 mMapCache->removeFromParent();
		 mBg->addChild(mMapCache, 0);
		 mMapCache->release();

		 sprintf(buf, "ditu_%d.jpg", zj);
		 mMapCache->initWithFile(buf);
		 //menu
		 for (int i=0; i<10; ++i)
		 {
			 char buf[128];
			 CCSprite *item = NULL;
			 CCSprite *name = NULL;

			 int iId = zj*10-9 + i;
			 if (iId < mMainId || (iId == mMainId && mMainState >= TaskStatus_Finished))
			 {
				 item = CCSprite::create("ditu_dibiao_xinrenwu_select.png");
				 sprintf(buf, "ditu_didian_act_%d_%d.png", zj, i+1); ////////////////////////
				 name = CCSprite::create(buf);
			 }
			 else if (iId == mMainId && mMainState < TaskStatus_Finished)
			 {
				 item = CCSprite::create("ditu_dibiao_xinrenwu.png");
				 sprintf(buf, "ditu_didian_act_%d_%d_new.png", zj, i+1); ////////////////////////
				 name = CCSprite::create(buf);
			 }
			 else
			 {
				 item = CCSprite::create("ditu_dibiao_xinrenwu_select.png");
				 sprintf(buf, "ditu_didian_act_%d_%d_new.png", zj, i+1); ////////////////////////
				 name = CCSprite::create(buf);
				 name->setVisible(false);
			 }
			 mMapCache->addChild(item);
			 item->setPosition(getTaskMenuPos(zj, i+1));
			 mMapCache->addChild(name);
			 name->setPosition(ccp(item->getPositionX(), item->getPositionY()-43));
		 }
	 }
 }

 CCPoint MapsLayer::getTaskMenuPos(int zj, int idx)
 {
	 if (zj == 0)
	 {
		 //CCPoint pos[4] = {ccp(143, 510), ccp(183, 277), ccp(396, 193), ccp(467, 552)};
		 if (idx == 1)
			 return ccp(143, 510);
		 else if (idx == 2)
			 return ccp(183, 277);
		 else if (idx == 3)
			 return ccp(396, 193);
		 else if (idx == 4)
			 return ccp(467, 552);
	 }
	 else if (zj == 1)
	 {
		 if (idx == 1)
			 return ccp(94, 448);
		 else if (idx == 2)
			 return ccp(194, 582);
		 else if (idx == 3)
			 return ccp(307, 487);
		 else if (idx == 4)
			 return ccp(476, 540);
		 else if (idx == 5)
			 return ccp(354, 380);
		 else if (idx == 6)
			 return ccp(478, 306);
		 else if (idx == 7)
			 return ccp(109, 280);
		 else if (idx == 8)
			 return ccp(152, 107);
		 else if (idx == 9)
			 return ccp(316, 205);
		 else if (idx == 10)
			 return ccp(452, 170);
	 }
	 else if (zj == 2)
	 {
		 if (idx == 1)
			 return ccp(120, 532);
		 else if (idx == 2)
			 return ccp(68, 392);
		 else if (idx == 3)
			 return ccp(261, 422);
		 else if (idx == 4)
			 return ccp(365, 582);
		 else if (idx == 5)
			 return ccp(520, 488);
		 else if (idx == 6)
			 return ccp(366, 370);
		 else if (idx == 7)
			 return ccp(109, 252);
		 else if (idx == 8)
			 return ccp(152, 96);
		 else if (idx == 9)
			 return ccp(422, 260);
		 else if (idx == 10)
			 return ccp(336, 116);
	 }
	 else if (zj == 3)
	 {
		 if (idx == 1)
			 return ccp(79, 69);
		 else if (idx == 2)
			 return ccp(180, 178);
		 else if (idx == 3)
			 return ccp(122, 298);
		 else if (idx == 4)
			 return ccp(177, 516);
		 else if (idx == 5)
			 return ccp(318, 362);
		 else if (idx == 6)
			 return ccp(334, 114);
		 else if (idx == 7)
			 return ccp(421, 269);
		 else if (idx == 8)
			 return ccp(520, 422);
		 else if (idx == 9)
			 return ccp(374, 487);
		 else if (idx == 10)
			 return ccp(513, 566);
	 }
	 else if (zj == 4)
	 {
		 if (idx == 1)
			 return ccp(241, 72);
		 else if (idx == 2)
			 return ccp(93, 155);
		 else if (idx == 3)
			 return ccp(124, 297);
		 else if (idx == 4)
			 return ccp(352, 332);
		 else if (idx == 5)
			 return ccp(514, 181);
		 else if (idx == 6)
			 return ccp(527, 428);
		 else if (idx == 7)
			 return ccp(356, 485);
		 else if (idx == 8)
			 return ccp(96, 452);
		 else if (idx == 9)
			 return ccp(250, 560);
		 else if (idx == 10)
			 return ccp(504, 609);
	 }
	 else
	 {
		 CCAssert(false, "");
	 }

	 return ccp(0,0);
 }


 void MapsLayer::drawRenWu()
 {
	 //清空 任务按钮  和  任务名
	 mRenwuMenu->removeAllChildren();
	 for(int i=0; i!=mNameArray->count(); ++i)
		 ((CCSprite*)mNameArray->objectAtIndex(i))->removeFromParent();
	 mNameArray->removeAllObjects();

	 //世界地图
	 if (mCurZhangjie <= 0 || mCurZhangjie > 4)
	 {
		 if (mCurZhangjie != 0) return;
		 
		 for (int i=0; i<4; ++i)
		 {
			 char buf[3][128];
			 sprintf(buf[0], "ditu_guanqia_act_%d.png", i+1);
			 sprintf(buf[1], "ditu_guanqia_act_%d_select.png", i+1);
			 sprintf(buf[2], "ditu_guanqia_act_%d_suoding.png", i+1);
			 CCMenuItemImage *chapterItem = CCMenuItemImage::create(buf[0], buf[1], buf[2]
				, this, menu_selector(MapsLayer::menuItemClicked_zhangjie));
			 mRenwuMenu->addChild(chapterItem, 1, i+1);
			 chapterItem->setPosition(getTaskMenuPos(0, i+1));

			 if ((mMainId + 9) / 10 < i+1)
				 chapterItem->setEnabled(false);
			 else if (!m_bIsTeachIng && (mMainId + 9) / 10 == i+1)
			 {
				 CCSprite *jiantou = CCSprite::create("juqing_duihua_xiayiye.png");
				 chapterItem->addChild(jiantou, 10,10);
				 jiantou->setPosition(ccp(chapterItem->getContentSize().width/2, chapterItem->getContentSize().height/2+30));
				 jiantou->runAction(CCRepeatForever::create(CCSequence::create(
					 CCMoveBy::create(0.5f, ccp(0,20)), CCMoveBy::create(0.5f, ccp(0, -20)), NULL)));
			 }
		 }
		 return;
	 }

	 //章节地图
	 //1. 已完成的任务
	 
	 CCSprite *name[Task_NUM];
	 for(int i=0; i<Task_NUM; ++i)
	 {
		 int iId = mCurZhangjie*10-9 + i;

		 //1. task name
		 char buf[128] = "";
		 sprintf(buf, "ditu_didian_act_%d_%d.png", mCurZhangjie, i+1); ////////////////////////
		 name[i] = BaseSprite::create(buf);
		 mNameArray->addObject(name[i]);
		 mMap->addChild(name[i], 0, TAG_MapName+i);

		 //2. fight item
		 if (mCurDifficulty == difficult_hard)
		 {
			 //to do
		 }
		 else if (mCurDifficulty == difficult_normal)
		 {
			 if (   (mMainId > iId) 
				 || (mMainId == iId  && mMainState >= TaskStatus_Finished))
			 {
				 CCSprite *s[2];
				 for (int nor_sel=0; nor_sel<2; ++nor_sel)
				 {
					 s[nor_sel] = BaseSprite::create("ditu_dibiao_xinrenwu_select.png");
					 int rate = 0;
					 if (mTaskRate.size() >= iId) rate = mTaskRate.at(iId - 1).star;
					 CCSprite *x[3];
					 if (rate >= 1) x[0] = CCSprite::create("ditu_xing_tong.png");
					 else           x[0] = CCSprite::create();

					 if (rate >= 2) x[1] = CCSprite::create("ditu_xing_yin.png");
					 else           x[1] = CCSprite::create();

					 if (rate >= 3) x[2] = CCSprite::create("ditu_xing_jin.png");
					 else           x[2] = CCSprite::create();
					 s[nor_sel]->addChild(x[0]);
					 x[0]->setPosition(ccp(32, 95-18));
					 x[0]->setRotation(30);
					 s[nor_sel]->addChild(x[1]);
					 x[1]->setPosition(ccp(51.5, 95-12));
					 s[nor_sel]->addChild(x[2]);
					 x[2]->setPosition(ccp(71, 95-18));
					 x[2]->setRotation(-30);
				 }
				 s[1]->setScale(0.95f);
				 m_fightItem[i] = CCMenuItemSprite::create(s[0], s[1], this, menu_selector(MapsLayer::menuItemClicked_Task));
			 }
			 else if (mMainId == iId  && mMainState < TaskStatus_Finished)
			 {
				 CCSprite *s[2];
				 s[0] = CCSprite::create("ditu_dibiao_xinrenwu.png");
				 s[1] = CCSprite::create("ditu_dibiao_xinrenwu.png");
				 s[1]->setScale(0.95f);
				 m_fightItem[i] = CCMenuItemSprite::create(s[0], s[1], this, SEL_MenuHandler(&MapsLayer::menuItemClicked_Task));

				 sprintf(buf, "ditu_didian_act_%d_%d_new.png", mCurZhangjie, i+1); ////////////////////////
				 name[i]->initWithFile(buf);
			 }
			 else
			 {
				 CCSprite *s[2];
				 s[0] = CCSprite::create("ditu_dibiao_xinrenwu_select.png");
				 s[1] = CCSprite::create("ditu_dibiao_xinrenwu_select.png");
				 s[1]->setScale(0.95f);
				 m_fightItem[i] = CCMenuItemSprite::create(s[0], s[1], this, SEL_MenuHandler(&MapsLayer::menuItemClicked_Task));

				 sprintf(buf, "ditu_didian_act_%d_%d_new.png", mCurZhangjie, i+1); ////////////////////////
				 name[i]->initWithFile(buf);
				 name[i]->setVisible(false);

				 m_fightItem[i]->setEnabled(false);
			 }
		 }
		 else
		 {
			 CCAssert(false, "drawRenWu ---> error task hard");
		 }
		 //
		 mRenwuMenu->addChild(m_fightItem[i], 1, iId);
		 m_fightItem[i]->setPosition(getTaskMenuPos(mCurZhangjie, i+1));
	 }
	 //新关卡提示
	 if(mMainState < TaskStatus_Finished && (mMainId + 9)/10 == mCurZhangjie
		 && dynamic_cast<CCMenuItem*>(mRenwuMenu->getChildByTag(mMainId)) != NULL)
	 {
		 CCMenuItem *item = dynamic_cast<CCMenuItem*>(mRenwuMenu->getChildByTag(mMainId));
		 CCSprite *jiantou = CCSprite::create("juqing_duihua_xiayiye.png");
		 item->addChild(jiantou, 10,10);
		 jiantou->setPosition(ccp(item->getContentSize().width/2, item->getContentSize().height-20));
		 jiantou->runAction(CCRepeatForever::create(CCSequence::create(
			 CCMoveBy::create(0.5f, ccp(0,20)), CCMoveBy::create(0.5f, ccp(0, -20)), NULL)));
	 }

	 //name set position
	 for(int i=0; i!=Task_NUM; ++i)
	 {
		 name[i]->setPosition(ccp(m_fightItem[i]->getPositionX(), m_fightItem[i]->getPositionY()-43));
	 }


	 //更新当前关卡是否有奖励可领取
	 for (int i = 0; i < Task_NUM; ++i)
	 {
		 if (m_fightItem[i])
		 {
			 updateLevelJiangliInfo(m_fightItem[i]);
		 }
	 }
 }

 void MapsLayer::CenterMainTask(TaskId id, TaskStatus state)
 {

/*	 CCMenuItem *curItem = NULL;

	 if (mCurDifficulty == difficult_hard)
	 {
		 if (g_taskId < HellTaskId_Min)  //有普通副本跳到地狱副本， 直接走到最新关卡
			 curItem = (CCMenuItem*)mRenwuMenu->getChildByTag(mMainId);
		 else if (g_taskId != -1 && mMainId != g_taskId-HellTaskId_Min+1 && mMainId != g_taskId-HellTaskId_Min+1+1)//以上次打得关卡为中心
			 curItem = (CCMenuItem *)mRenwuMenu->getChildByTag(g_taskId-HellTaskId_Min+1);
		 else
			 curItem = (CCMenuItem*)mRenwuMenu->getChildByTag(mMainId);
	 }
	 else
	 {
		 if (g_taskId >= HellTaskId_Min) //由地狱副本 跳到普通副本， 直接走到最新关卡
			 curItem = (CCMenuItem*)mRenwuMenu->getChildByTag(mMainId);
		 else if (g_taskId != -1 && mMainId != g_taskId && mMainId != g_taskId+1)//以上次打得关卡为中心
			 curItem = (CCMenuItem *)mRenwuMenu->getChildByTag(g_taskId);
		 else
			 curItem = (CCMenuItem*)mRenwuMenu->getChildByTag(mMainId);
	 }

	 CCPoint pos = curItem->getPosition();*/


	 //第一次教学
	 teachGuide();
 }

 void MapsLayer::unblock_diaoluoChaXun()
 {
	 //掉落查询暂时不要

// 	 bool bUnblock = bUnblock =  UnblockManager::getManager()->isDeblocking(unblock_DiaoLuoChaXun);
// 	 
// 	 //
// 	 if (bUnblock)  //1. 当前情况该提示了
// 	 {
// 		  bool opened = true;  //2. 是否提示过
// 		  if(History::getHistory()->getBoolFromCurrentHero(Key_DiaoLuoChaXunOpend, opened))
// 		  {
// 			  if(! opened)
// 			  {
// 				   History::getHistory()->putInCurrentHero(Key_DiaoLuoChaXunOpend, true);
// 				   //弹框通知
// 				   //
// 				   UnblockWarnDialog *layer = new UnblockWarnDialog;
// 				   layer->init(LFStrings::getValue("diaoluochaxun_jiesuo"), true);
// 				   CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
// 				   layer->release();
// 			  }
// 		  }
// 		  else
// 		  {
// 			  History::getHistory()->putInCurrentHero(Key_DiaoLuoChaXunOpend, true);
// 			  //弹框通知
// 			  //
// 			  UnblockWarnDialog *layer = new UnblockWarnDialog;
// 			  layer->init(LFStrings::getValue("diaoluochaxun_jiesuo"), true);
// 			  CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
// 			  layer->release();
// 		  }
// 	 }

 }

 void MapsLayer::menuItemClicked_SaoDang(CCObject *pSender)
 {
	 unsigned int curid = ((CCMenuItem*)m_task_menuItem)->getTag(), r, l;
	 CS::getTaskChessboardFormat(curid, r, l);
	 if (r * l < 9)  //小于9个不能扫荡  弹框提示
	 {
		 MessageDisplay *layer = MessageDisplay::create(LFStrings::getValue("SaoDangTiShi_9ge"));
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		 return;
	 }
	 else if (MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft < 0)
	 {
		 MessageDisplay *layer = MessageDisplay::create(LFStrings::getValue("SaoDangTiShi_CiShuYongWan"));
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		 return;
	 }


	 {
		 dynamic_cast<CCMenu*>(this->getChildByTag(Tag_NpcBianhui_Layer))->setEnabled(false);
		 dynamic_cast<CCMenu*>(this->getChildByTag(Tag_Npc_Window)->getChildByTag(Tag_Npc_Menu))->setEnabled(false);
	 }

	 BaseSprite *sdBg = BaseSprite::create("zhandou_ditu_diaoluochaxun_saodang_bg.png");
	 addChild(sdBg, 20, Tag_SaoDang_Window);
	 sdBg->setAnchorPoint(ccp(0.5, 1));
	 sdBg->setPosition(ccp(mWinSize.width/2, 748));

	 //
	 CCMenu *menu = CCMenu::create();
	 sdBg->addChild(menu, 1, 10);
	 menu->setPosition(ccp(0,0));
	 //close
	 CCMenuItemImage *itemClose = CCMenuItemImage::create(
		 "close.png",
		 "close_select.png",
		 NULL, this, menu_selector(MapsLayer::menuItemSaoDang_item));
	 menu->addChild(itemClose, 0, Tag_ItemSaoDang_Close);
	 itemClose->setPosition(ccp(560, 723-51));
	 //add sub    max=10
	 CCMenuItemImage *jia = CCMenuItemImage::create(
		 "gongjiang_duanzao_zengjia_baohu_jia.png"
		 , "gongjiang_duanzao_zengjia_baohu_jia_select.png"
		 , "gongjiang_duanzao_zengjia_baohu_jia_select.png"
		 , this, menu_selector(MapsLayer::menuItemSaoDang_item));
	 menu->addChild(jia, 1, Tag_ItemSaoDang_numAdd);

	 if (MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft <= 1)
	 {
		 jia->setEnabled(false);
	 }
	 jia->setPosition(ccp(538, 723-187));
	 CCMenuItemImage *jian = CCMenuItemImage::create(
		 "gongjiang_duanzao_zengjia_baohu_jian.png"
		 , "gongjiang_duanzao_zengjia_baohu_jian_select.png"
		 , "gongjiang_duanzao_zengjia_baohu_jian_select.png"
		 , this, menu_selector(MapsLayer::menuItemSaoDang_item));
	 menu->addChild(jian, 1, Tag_ItemSaoDang_numSub);
	 jian->setPosition(ccp(411, 723-187));
	 jian->setEnabled(false);
	 //bai lan huang zhuang
	 CCMenuItemImage *itemBai = CCMenuItemImage::create(
		 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_putong.png",
		 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_putong_select.png",
		 this, menu_selector(MapsLayer::menuItemSaoDang_item));
	 menu->addChild(itemBai, 1, Tag_ItemSaoDang_BaiZhuang);
	 itemBai->setPosition(ccp(305, 315));
	 CCMenuItemImage *itemLan = CCMenuItemImage::create(
		 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_mofa.png",
		 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_mofa_select.png",
		 this, menu_selector(MapsLayer::menuItemSaoDang_item));
	 menu->addChild(itemLan, 1, Tag_ItemSaoDang_LanZhuang);
	 itemLan->setPosition(ccp(305, 243));
	 CCMenuItemImage *itemHuang = CCMenuItemImage::create(
		 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_xiyou.png",
		 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_xiyou_select.png",
		 this, menu_selector(MapsLayer::menuItemSaoDang_item));
	 menu->addChild(itemHuang, 1, Tag_ItemSaoDang_HuangZhuang);
	 itemHuang->setPosition(ccp(305, 723-531));
	 itemHuang->setVisible(false);

	//begin
	CCMenuItemImage *itemSd = CCMenuItemImage::create(
		"zhandou_ditu_renwushuoming_saodang.png",
		"zhandou_ditu_renwushuoming_saodang_select.png",
		"zhandou_ditu_renwushuoming_saodang_suoding.png",
		this, menu_selector(MapsLayer::menuItemSaoDang_item));
	menu->addChild(itemSd, 1, Tag_ItemSaoDang_Begin);
	itemSd->setPosition(ccp(305, 723-658));
	 // num
	 CCLabelTTF *labelNum = CCLabelTTF::create("1", fontStr_BookAntiqua, 23);
	 sdBg->addChild(labelNum, 1, Tag_Label_SaoDang_Num);
	 labelNum->setPosition(ccp(474, 723-188));
	 // tili 
	 char buf[20];
	 int need_tili = Vitality_GuanQia_Floped * r * l;
	 sprintf(buf, "%d", need_tili);
	 CCLabelTTF *labelTili = CCLabelTTF::create(buf, fontStr_BookAntiqua, 23);
	 sdBg->addChild(labelTili, 1, Tag_Label_SaoDang_TiLi);
	 labelTili->setAnchorPoint(ccp(0.5, 0.5));
	 labelTili->setPosition(ccp(474, 723-256));
	 {
		 int cur_tili = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current;
		 if (need_tili > cur_tili) 
		 {
			 labelTili->setColor(ccRED);
//			 itemSd->setEnabled(false);
			 jia->setEnabled(false);
		 }
	 }
	 //剩余次数
	 sprintf(buf, "%u", MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft);
	 CCLabelTTF *countLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
	 sdBg->addChild(countLabel, 1, Tag_Label_SaoDangCount);
	 countLabel->setAnchorPoint(ccp(0, 0.5));
	 countLabel->setPosition(ccp(210, 153));

	 //init data
	 for (int i=0; i!=3; ++i) mAuto_FenJie[i] = false;
	 mSaoDangNum = 1;
 }

 void MapsLayer::menuItemSaoDang_item(CCObject *pSender)
 {
	 unsigned int curid = ((CCMenuItem*)m_task_menuItem)->getTag(), r, l;
	 CS::getTaskChessboardFormat(curid, r, l);

	 CCMenuItemImage *item = dynamic_cast<CCMenuItemImage*>(pSender);
	 BaseSprite *sdBg = dynamic_cast<BaseSprite*>(getChildByTag(Tag_SaoDang_Window));
	 int tag = item->getTag();

	 if (tag == Tag_ItemSaoDang_Close)
	 {
		 CCMenu *menu = dynamic_cast<CCMenu*>(this->getChildByTag(Tag_NpcBianhui_Layer));
		 if (menu) menu->setEnabled(true);
		 CCSprite *bg = (CCSprite*)this->getChildByTag(Tag_Npc_Window);
		 if (bg) 
		 {
			 menu = dynamic_cast<CCMenu*>(bg->getChildByTag(Tag_Npc_Menu));
			 if (menu) menu->setEnabled(true);
		 }
		 removeChildByTag(Tag_SaoDang_Window);
		 return;
	 }
	 else if (tag == Tag_ItemSaoDang_numAdd)
	 {
		 ++mSaoDangNum;
		 int need_tili = Vitality_GuanQia_Floped * r * l * mSaoDangNum;
		 int cur_tili  = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current;

		 //
		 char buf[20];
		 sprintf(buf, "%d", mSaoDangNum);
		 dynamic_cast<CCLabelTTF*>(sdBg->getChildByTag(Tag_Label_SaoDang_Num))->setString(buf);

		 CCLabelTTF *labelTili = dynamic_cast<CCLabelTTF*>(sdBg->getChildByTag(Tag_Label_SaoDang_TiLi));
		 sprintf(buf, "%d", need_tili);
		 labelTili->setString(buf);

		 if (need_tili > cur_tili) 
		 {
			 labelTili->setColor(ccRED);
//			 dynamic_cast<CCMenuItemImage*>(sdBg->getChildByTag(10)->getChildByTag(Tag_ItemSaoDang_Begin))->setEnabled(false);
			 item->setEnabled(false);
		 }
		 else if (mSaoDangNum >= 10 || mSaoDangNum >= MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft)  //max = 10
		 {
			 item->setEnabled(false);
		 }
		 else
		 {
			 labelTili->setColor(ccWHITE);
			 dynamic_cast<CCMenuItemImage*>(sdBg->getChildByTag(10)->getChildByTag(Tag_ItemSaoDang_Begin))->setEnabled(true);
		 }
		 ((CCMenuItemImage*)item->getParent()->getChildByTag(Tag_ItemSaoDang_numSub))->setEnabled(true);
	 }
	 else if (tag == Tag_ItemSaoDang_numSub)
	 {
		 --mSaoDangNum;
		 int need_tili = Vitality_GuanQia_Floped * r * l * mSaoDangNum;
		 int cur_tili  = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current;

		 //
		 char buf[20];
		 sprintf(buf, "%d", mSaoDangNum);
		 dynamic_cast<CCLabelTTF*>(sdBg->getChildByTag(Tag_Label_SaoDang_Num))->setString(buf);

		 CCLabelTTF *labelTili = dynamic_cast<CCLabelTTF*>(sdBg->getChildByTag(Tag_Label_SaoDang_TiLi));
		 sprintf(buf, "%d", need_tili);
		 labelTili->setString(buf);
		 if (need_tili > cur_tili) 
		 {
			 labelTili->setColor(ccRED);
//			 dynamic_cast<CCMenuItemImage*>(sdBg->getChildByTag(10)->getChildByTag(Tag_ItemSaoDang_Begin))->setEnabled(false);
			 item->setEnabled(false);
		 }
		 else
		 {
			 labelTili->setColor(ccWHITE);
			 dynamic_cast<CCMenuItemImage*>(sdBg->getChildByTag(10)->getChildByTag(Tag_ItemSaoDang_Begin))->setEnabled(true);
		 }
		 ((CCMenuItemImage*)item->getParent()->getChildByTag(Tag_ItemSaoDang_numAdd))->setEnabled(true);

		 if (mSaoDangNum == 1)
			 item->setEnabled(false);
	 }
	 else if (tag == Tag_ItemSaoDang_BaiZhuang)
	 {
		 if (mAuto_FenJie[0])
		 {
			 mAuto_FenJie[0] = false;
			 item->initWithNormalImage(
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_putong.png",
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_putong_select.png",
				 NULL, this, menu_selector(MapsLayer::menuItemSaoDang_item));
		 }
		 else 
		 {
			 mAuto_FenJie[0] = true;
			 item->initWithNormalImage(
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_putong_select.png",
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_putong.png",
				 NULL, this, menu_selector(MapsLayer::menuItemSaoDang_item));
		 }
	 }
	 else if (tag == Tag_ItemSaoDang_LanZhuang)
	 {
		 if (mAuto_FenJie[1])
		 {
			 mAuto_FenJie[1] = false;
			 item->initWithNormalImage(
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_mofa.png",
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_mofa_select.png",
				 NULL, this, menu_selector(MapsLayer::menuItemSaoDang_item));
		 }
		 else 
		 {
			 mAuto_FenJie[1] = true;
			 item->initWithNormalImage(
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_mofa_select.png",
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_mofa.png",
				 NULL, this, menu_selector(MapsLayer::menuItemSaoDang_item));
		 }
	 }
	 else if (tag == Tag_ItemSaoDang_HuangZhuang)
	 {
		 if (mAuto_FenJie[2])
		 {
			 mAuto_FenJie[2] = false;
			 item->initWithNormalImage(
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_xiyou.png",
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_xiyou_select.png",
				 NULL, this, menu_selector(MapsLayer::menuItemSaoDang_item));
		 }
		 else 
		 {
			 mAuto_FenJie[2] = true;
			 item->initWithNormalImage(
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_xiyou_select.png",
				 "zhandou_ditu_diaoluochaxun_saodang_gouxuan_xiyou.png",
				 NULL, this, menu_selector(MapsLayer::menuItemSaoDang_item));
		 }
	 }
	 else if (tag == Tag_ItemSaoDang_Begin)
	 {
		 //体力不足，弹框提示买体力
		 int need_tili = mSaoDangNum * Vitality_GuanQia_Floped * r * l;
		 int cur_tili = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current;
		 if (cur_tili < need_tili)
		 {
			 GetTiLiLayer* layer = GetTiLiLayer::create(this);
			 CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);
			 return;
		 }
		 else if (MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft <= 0)
		 {
			 MessageDisplay *layer = MessageDisplay::create(LFStrings::getValue("SaoDangCiShuNoNeed"));
			 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			 return;
		 }

		 if (MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft < mSaoDangNum)
		 {
			 MessageDisplay *layer = MessageDisplay::create(LFStrings::getValue("CEC_NoSaoDangTime"));
			 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			 return;
		 }

		 SPCmd_Saodang rep;
		 rep.task_id = ((CCMenuItem*)m_task_menuItem)->getTag();
		 rep.saodang_time = mSaoDangNum;
		 rep.auto_fenjie_baizhuang = mAuto_FenJie[0];
		 rep.auto_fenjie_lanzhuang = mAuto_FenJie[1];
		 rep.auto_fenjie_huangzhuang = mAuto_FenJie[2];

		 addObserver(callfuncO_selector(MapsLayer::rsp_SaoDangEnd), MSG_saodangFubenRsp_F);
		 mCmdHlp->cmdSaodangFuben(rep);
	 }
 }

 void MapsLayer::rsp_SaoDangEnd(CCObject *msg_f)
 {
	 GameDirector::getDirector()->hideWaiting();
	 removeObserver(MSG_saodangFubenRsp_F);

	 Flat_SaodangRsp *data = (Flat_SaodangRsp*)dynamic_cast<Flat_MSG_Rsp*>(msg_f)->getData();
	 if(data->err_code != Success)
	 {
		 if (data->err_code == CEC_NoMoreFreeSpaceToStoreEquip)
		 {
			 EquipBagNotEnoughDialog *layer = new EquipBagNotEnoughDialog;
			 layer->showGoToFenJieDialog();
		 }
		 else
		 {
			 MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
			 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		 }
		 return;
	 }
	 mSaoDangData = *data;
	 BaseSprite *sdBg = (BaseSprite*)getChildByTag(Tag_SaoDang_Window);
	 sdBg->removeAllChildren();
	 sdBg->initWithFile("zhandou_ditu_diaoluochaxun_saodang_jieshu_bg.png");
	 sdBg->setAnchorPoint(ccp(0.5, 1));

	 CCMenu *menu = CCMenu::create();
	 sdBg->addChild(menu, 1, 10);
	 menu->setPosition(ccp(0,0));
	 //close
	 CCMenuItemImage *itemClose = CCMenuItemImage::create(
		 "close.png",
		 "close_select.png",
		 NULL, this, menu_selector(MapsLayer::menuItemClicked_SaoDangEnd));
	 menu->addChild(itemClose, 1, Tag_ItemSaoDang_Close);
	 itemClose->setPosition(ccp(560, 723-51));
	 //bag
	 CCMenuItemImage *itemBag = CCMenuItemImage::create(
		 "zhandou_ditu_renwushuoming_beibao.png",
		 "zhandou_ditu_renwushuoming_beibao_select.png",
		 NULL, this, menu_selector(MapsLayer::menuItemClicked_SaoDangEnd));
	 menu->addChild(itemBag, 1, Tag_ItemSaoDang_BeiBao);
	 itemBag->setPosition(ccp(sdBg->getContentSize().width/2, 723-658));

	 //
	 CCString tempStr;
	 //剩余扫荡次数
	 tempStr.initWithFormat("%u", MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft);
	 CCLabelTTF *countLabel = (CCLabelTTF* )sdBg->getChildByTag(Tag_Label_SaoDangCount);
	 if (countLabel) countLabel->setString(tempStr.getCString());

	 //
	 int label_size = 23;
	 ccColor3B color_hui = ccc3(127,127,127);
	 ccColor3B color_huang = ccc3(195, 145, 96);
	 CCSize bgSize = sdBg->getContentSize();
	 float gap_max = 40.0f, gap_min = 24.0f;
	 float posY = 723-276;
	 // exp
	 tempStr.initWithFormat("%s", (LFStrings::getValue("huode") + LFStrings::getValue("jingyan")).c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_huang);
	 tempStr.initWithFormat("%u", data->exp);
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), ccWHITE);
	 posY -= gap_max;

	 //20140529 扫荡去掉银币
	 //all silver
/*	 tempStr.initWithFormat("%s", (LFStrings::getValue("xianyou")+LFStrings::getValue("yinbi")).c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_huang);
	 tempStr.initWithFormat("%u", data->silver_end_total);
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), ccWHITE);
	 posY -= gap_min;
	 //award silver
	 tempStr.initWithFormat("%s", (LFStrings::getValue("guaiwu")+LFStrings::getValue("diaoluo")).c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_hui);
	 tempStr.initWithFormat("%u", data->silver_diaoluo);
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), color_hui);
	 posY -= gap_min;
	 //used silver
	 tempStr.initWithFormat("%s", LFStrings::getValue("fenjiexiaohao").c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size-3, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_hui);
	 tempStr.initWithFormat("%u", data->silver_used_for_auto_fenjie);
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size-3, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), color_hui);
	 posY -= gap_max;*/

	 //equip
	 tempStr.initWithFormat("%s", (LFStrings::getValue("shengyu")+LFStrings::getValue("zhuangbei")).c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_huang);
	 tempStr.initWithFormat("%d", data->equips.size() + data->decomposed_count);
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), ccWHITE);
	 posY -= gap_min;
	 //获得装备
	 tempStr.initWithFormat("%s", (LFStrings::getValue("HuoDe")+LFStrings::getValue("zhuangbei")).c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_hui);
	 tempStr.initWithFormat("%u", data->equips.size());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), color_hui);
	 posY -= gap_min;
	 //分解装备
	 tempStr.initWithFormat("%s", (LFStrings::getValue("FenJie")+LFStrings::getValue("zhuangbei")).c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size-3, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_hui);
	 tempStr.initWithFormat("%u", data->decomposed_count);
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size-3, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), color_hui);
	 posY -= gap_max;
	 //

	 //baoshi
	 int baoshiNum = 0;
	 for (int i=0; i!=data->items.size(); ++i)
		 if (data->items.at(i).type >= ItemsType_Baoshi_min && data->items.at(i).type <= ItemsType_Baoshi_max)
			 baoshiNum += data->items.at(i).count;
	 tempStr.initWithFormat("%s", (LFStrings::getValue("huode")+LFStrings::getValue("baoshi")).c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_huang);
	 tempStr.initWithFormat("%d", baoshiNum);
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), ccWHITE);
	 posY -= gap_max;

	 //cailiao
	 int caiLiaoNum = 0;
	 for (int i=0; i!=data->items.size(); ++i)
		caiLiaoNum += data->items.at(i).count;
	 caiLiaoNum -= baoshiNum;
	 tempStr.initWithFormat("%s", (LFStrings::getValue("huode")+LFStrings::getValue("cailiao")).c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_huang);
	 tempStr.initWithFormat("%d", caiLiaoNum);
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), ccWHITE);
	 posY -= gap_max;

	 //20140529 扫荡去掉碎片
	 //suipian
/*	 tempStr.initWithFormat("%s", (LFStrings::getValue("huode")+LFStrings::getValue("suipian")).c_str());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_kaiti, label_size, ccp(0, 0.5), ccp(bgSize.width/3, posY), color_huang);
	 tempStr.initWithFormat("%d", data->item_suipians.size());
	 addLabelToNode_AncPosColorSize(sdBg, tempStr, fontStr_BookAntiqua, label_size, ccp(0, 0.5), ccp(bgSize.width/3*2, posY), ccWHITE);*/

	 ///////////////////////////////////////////////////////////////
	 //如果升级了
	 ///////////////////////////////////////////////////////////////
	 vector<BaseActor*> vec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
	 CCArray *lvUpArray = new CCArray;
	 lvUpArray->init();
	 for (int i=0; i!=vec.size(); ++i)
	 {
		 if (vec.at(i)->getCurrentProperty().exp_current >= vec.at(i)->getCurrentProperty().exp_need)
		 {
			 LevelUpLayer *layer = LevelUpLayer::create(vec.at(i)->getId(), this);
			 if (layer) lvUpArray->addObject(layer);
		 }
	 }
	 for (int i=0; i!=lvUpArray->count(); ++i)
	 {
		 LevelUpLayer *layer = (LevelUpLayer*)lvUpArray->objectAtIndex(i);
		 if (i != lvUpArray->count() - 1)
			 //set 之后 会retain, array 可以放心的release
				 layer->setNextLevelUpLayer(dynamic_cast<LevelUpLayer*>(lvUpArray->objectAtIndex(i+1))); 
	 }
	 if (lvUpArray->count() > 0)
		 CCDirector::sharedDirector()->getRunningScene()->addChild((LevelUpLayer*)lvUpArray->objectAtIndex(0), 150);
	 lvUpArray->release(); 
 }

 void MapsLayer::menuItemClicked_SaoDangEnd(CCObject *pSender)
 {
	 int tag = dynamic_cast<CCMenuItem*>(pSender)->getTag();

	 if (tag == Tag_ItemSaoDang_Close)
	 {
		 {
			 dynamic_cast<CCMenu*>(this->getChildByTag(Tag_NpcBianhui_Layer))->setEnabled(true);
			 dynamic_cast<CCMenu*>(this->getChildByTag(Tag_Npc_Window)->getChildByTag(Tag_Npc_Menu))->setEnabled(true);
		 }
		 removeChildByTag(Tag_SaoDang_Window);
		 return;
	 }
	 else if (tag == Tag_ItemSaoDang_BeiBao)
	 {
		 vector<Bag_WuPinInfo> WupinVec;
		 vector<Equipment> EquipsVec;

		 for (int i=0; i!=mSaoDangData.equips.size(); ++i)
			 EquipsVec.push_back(Equipment(mSaoDangData.equips.at(i)));

		 Bag_WuPinInfo temp;
		 for (int i=0; i!=mSaoDangData.items.size(); ++i)
		 {
			 temp.type = mSaoDangData.items.at(i).type;
			 temp.count = mSaoDangData.items.at(i).count;
			 WupinVec.push_back(temp);
		 }

		 //20140529 扫荡去掉碎片
// 		 for (int i=0; i!=mSaoDangData.item_suipians.size(); ++i)
// 		 {
// 			 temp.type = mSaoDangData.item_suipians.at(i).suipian_id;
// 			 temp.count = mSaoDangData.item_suipians.at(i).suipian_count;
// 			 temp.isSuiPian = true;
// 			 temp.suipian_id = mSaoDangData.item_suipians.at(i).suipian_id;
// 			 WupinVec.push_back(temp);
// 		 }
		 FightBagLayer *beibaoLayer = new FightBagLayer;
		 beibaoLayer->init(EquipsVec, WupinVec, this, true);
		 mFooterLayer->popUpContaintLayer(beibaoLayer);
		 beibaoLayer->release();
		 beibaoLayer->setPosition(ccp(mWinSize.width/2-beibaoLayer->getBodySize().width/2, mFooterLayer->getFooter()->getFooterSize().height));
	 }
 }

 void MapsLayer::disableAllTouch()
 {
	 m_bDisplayWindow_CannotTouch = true;
	 BaseSprite* sdBg = (BaseSprite*)this->getChildByTag(Tag_SaoDang_Window);
	 dynamic_cast<CCMenu*>(sdBg->getChildByTag(10))->setEnabled(false);
 }

 void MapsLayer::enableAllTouch()
 {
	 BaseSprite* sdBg = (BaseSprite*)this->getChildByTag(Tag_SaoDang_Window);
	 dynamic_cast<CCMenu*>(sdBg->getChildByTag(10))->setEnabled(true);
	 m_bDisplayWindow_CannotTouch = false;
 }

 void MapsLayer::getTiliCallBackFunc()
 {
	 unsigned int curid = ((CCMenuItem*)m_task_menuItem)->getTag(), r, l;
	 CS::getTaskChessboardFormat(curid, r, l);

	 BaseSprite *sdBg = (BaseSprite*)getChildByTag(Tag_SaoDang_Window);
	 {
		 int cur_tili = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current;
		 int need_tili = mSaoDangNum * r * l * Vitality_GuanQia_Floped;
		 if (need_tili <= cur_tili) 
		 {
			 dynamic_cast<CCLabelTTF*>(sdBg->getChildByTag(Tag_Label_SaoDang_TiLi))->setColor(ccWHITE);
			 if (MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft <= mSaoDangNum || mSaoDangNum >= 10)
			 {
				 ((CCMenuItem*)sdBg->getChildByTag(10)->getChildByTag(Tag_ItemSaoDang_numAdd))->setEnabled(false);
			 }
			 else
			 {
				 ((CCMenuItem*)sdBg->getChildByTag(10)->getChildByTag(Tag_ItemSaoDang_numAdd))->setEnabled(true);
			 }
		 }
	 }
 }

 bool MapsLayer::unblockNewZone_TaskEnd()
 {
	 bool hasNewZone = false;

	 //通关 新玩法 提示：  (均由fightSetlementLayer讲全局变量置为true)
	 //通关铁匠的宝贝task=4 ---> 开启强化功能  
	 if (g_gongjiang_QhHt_Opend)
	 {
		 hasNewZone = true;
		 g_gongjiang_QhHt_Opend = false;
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("gongjiang_TieJiangPuQiangHuaKaiQi"), true,false,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	 }
	 else if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstXunBaoTeach))
	 {
		 hasNewZone = true;
		 //GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstXunBaoTeach);
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("XunBaoGongNengKaiQi"), true,false,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	 }
	 else if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_NewFriendAddOne))
	 {
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_NewFriendAddOne);
			switch (MyselfManager::getManager()->getMyZhuJueData()->getActorType())
			{
			case 1:
				{
					/*AddFriendWarnLayer *layer = AddFriendWarnLayer::create();
					layer->ignoreAnchorPointForPosition(false);
					layer->setAnchorPoint(CCPointZero);
					layer->setPosition(CCPointZero);
					layer->setFriendName(getNewFirendName(8));*/

					AddFriendUI *layer = AddFriendUI::create();
					layer->ignoreAnchorPointForPosition(false);
					layer->setAnchorPoint(CCPointZero);
					layer->setPosition(CCPointZero);
					layer->setFriendID(2);
					//layer->setFriendName(getNewFirendName(8));
					layer->setCallBack(this,SEL_CallFuncN(&MapsLayer::addFriendCallBack));
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);
				}
				break;
			case 2:
				{
					/*AddFriendWarnLayer *layer = AddFriendWarnLayer::create();
					layer->ignoreAnchorPointForPosition(false);
					layer->setAnchorPoint(CCPointZero);
					layer->setPosition(CCPointZero);
					layer->setFriendName(getNewFirendName(6));*/
					AddFriendUI *layer = AddFriendUI::create();
					layer->ignoreAnchorPointForPosition(false);
					layer->setAnchorPoint(CCPointZero);
					layer->setPosition(CCPointZero);
					layer->setFriendID(2);
					//layer->setFriendID(3);
					layer->setCallBack(this,SEL_CallFuncN(&MapsLayer::addFriendCallBack));
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);
				}
				break;
			case 3:
				{
					AddFriendUI *layer = AddFriendUI::create();
					layer->ignoreAnchorPointForPosition(false);
					layer->setAnchorPoint(CCPointZero);
					layer->setPosition(CCPointZero);
					layer->setFriendID(2);
					//layer->setFriendID(4);
					layer->setCallBack(this,SEL_CallFuncN(&MapsLayer::addFriendCallBack));
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);
				}
				break;
			default:
				break;
			}
	 }
	 else if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_NewFriendAddTwo))
	 {
		 GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_NewFriendAddTwo);
			switch (MyselfManager::getManager()->getMyZhuJueData()->getActorType())
			{
			case 1:
				{
					AddFriendUI *layer = AddFriendUI::create();
					layer->ignoreAnchorPointForPosition(false);
					layer->setAnchorPoint(CCPointZero);
					layer->setPosition(CCPointZero);
					//layer->setFriendID(9);
					layer->setFriendID(3);
					layer->setCallBack(this,SEL_CallFuncN(&MapsLayer::addFriendCallBack));
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);
				}
				break;
			case 2:
				{

					AddFriendUI *layer = AddFriendUI::create();
					layer->ignoreAnchorPointForPosition(false);
					layer->setAnchorPoint(CCPointZero);
					layer->setPosition(CCPointZero);
					//layer->setFriendID(4);
					layer->setFriendID(3);
					layer->setCallBack(this,SEL_CallFuncN(&MapsLayer::addFriendCallBack));
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);

					
				}
				break;
			case 3:
				{
					AddFriendUI *layer = AddFriendUI::create();
					layer->ignoreAnchorPointForPosition(false);
					layer->setAnchorPoint(CCPointZero);
					layer->setPosition(CCPointZero);
					//layer->setFriendID(9);
					layer->setFriendID(3);
					layer->setCallBack(this,SEL_CallFuncN(&MapsLayer::addFriendCallBack));
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);


					/*AddFriendWarnLayer *layer = AddFriendWarnLayer::create();
					layer->ignoreAnchorPointForPosition(false);
					layer->setAnchorPoint(CCPointZero);
					layer->setPosition(CCPointZero);
					layer->setFriendName(getNewFirendName(9));
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);*/
				}
				break;
			default:
				break;
			}
	 }
	 else if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstYingXiongBang))
	 {
		 hasNewZone = true;
		 //GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstYingXiongBang);
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_YingXiongBangKaiQi"), true,false,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	 }
	 return hasNewZone;
 }

 void MapsLayer::LevelUpEndCallBackFunc()
 {
	 ///////////////升级触发的 新玩法提示   
	 if (g_ChuanQiFuBen_Opend) //传奇副本
	 {
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_ChuanQiFuBenKaiQi"), true,false,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	 }
	 else if (g_ChuanQiLu_Opend)
	 {
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_ChuanQiLuKaiQi"), true,false,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	 }
	 else if (g_YingXiongBang_Opend)  //英雄榜
	 {
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_YingXiongBangKaiQi"), true,false,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	 }
	 else if (g_JingJiChang_Opend) // 竞技场
	 {
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_JingJiChangKaiQi"), true,false,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	 }
	 else if (g_JiuGuan_Opend) //酒馆
	 {
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("JiuGuanKaiqi"), true,false,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	 }
	 else if (g_XueSeJiaoTang_Opend) //血色教堂
	 {
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("TiShi_XueSeJiaoTang"), true, true,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	 }
	 else if (g_JiXianShouCheng_Opend)//极限守城
	 {
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("TiShi_JiXianShouCheng"), true, true,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	 }
	 else if (g_DianFengDuiJue_Opend)//巅峰对决
	 {
		 UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("TiShi_DianFengDuiJue"), true, true,true);
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	 }
 }

 void MapsLayer::rsp_ShouHuZhiLing_TiaoZhan(CCObject *msg_f)
 {
	 removeObserver(MSG_challengeProtectSpiritRsp_F);
	 GameDirector::getDirector()->hideWaiting();

	 Flat_ChallengeProtectSpiritRsp* data = (Flat_ChallengeProtectSpiritRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	 if(data->err_code != Success)
	 {
// 		 MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
// 		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		 UnblockWarnDialog* player = UnblockWarnDialog::create(mCmdHlp->getMessage(data->err_code));
		 CCDirector::sharedDirector()->getRunningScene()->addChild(player);
		 return;
	 }
	 
	 FightDirector dir;
	 vector<FightDirector> dir_vec;
	 dir.initScript(data->fightScript);
	 dir_vec.push_back(dir);

	 Bundle bundle;
	 bundle.putString("JumpToWhenEnd","MapsLayer");
	 ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec
		 ,MapsManager::getManager()->getBackGroundMapImage(mCurZhangjie*10),bundle);
 }

 void MapsLayer::rsp_ShouHuZhiLing_MoBai(CCObject *msg)
 {
	 removeObserver(MSG_worshiProtectSpiritRsp);
	 GameDirector::getDirector()->hideWaiting();

	 SPCmd_WorshiProtectSpiritRsp* data = (SPCmd_WorshiProtectSpiritRsp*)((MSG_Rsp*)msg)->getData();
	 if(data->err_code != Success)
	 {
// 		 MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
// 		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		
		 UnblockWarnDialog* player = UnblockWarnDialog::create(mCmdHlp->getMessage(data->err_code));
		 CCDirector::sharedDirector()->getRunningScene()->addChild(player);
		 return;
	 }


	 
	 //TODO:
	 vector<GiftBag> items;
	 items.push_back(data->gift_bag);
//	 CS::worshiProtectSpirit(MyselfManager::getManager()->getMyZhuJueData()->getLevel(), items);

//	 AwardDisplay *layer = AwardDisplay::create(LFStrings::getValue("SHZL_MoBaiJiangLi"), items);
//	 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	 CCString infoStr = LFStrings::getValue("SHZL_MoBaiJiangLi");

	 Flat_BonusInfo info;
	 info.giftBagVec = items;


	 GiftDisplay *layer = GiftDisplay::create(infoStr,info);
	 //AwardDisplay *layer = AwardDisplay::create(infoStr, mGiftsLoginVec.at(0).itemVec, mGiftsLoginVec.at(0).equipVec);
	 CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
 }

 void MapsLayer::rsp_ShouHuZhiLing_LingJiang(CCObject *msg)
 {
	 removeObserver(MSG_getProtectSpiritBonusRsp);
	 GameDirector::getDirector()->hideWaiting();

	 SPCmd_GetProtectSpiritBonusRsp* data = (SPCmd_GetProtectSpiritBonusRsp*)((MSG_Rsp*)msg)->getData();
	 if(data->err_code != Success)
	 {
		 MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		 return;
	 }


	 vector<ItemInfo> items;
	 for (int i=0; i<data->item_count; ++i)
	 {
		 items.push_back(data->item_arr[i]);
	 }
//	 CS::getProtectSpiritBonus(mCurZhangjie, 1, items);
	 AwardDisplay *layer = AwardDisplay::create(LFStrings::getValue("SHZL_LingJiangRuXia"), items);
	 CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
 }

 void MapsLayer::rsp_SHZLHero(CCObject *msg)
 {
	 removeObserver(MSG_queryAnotherTeamInfoRsp_F);
	 GameDirector::getDirector()->hideWaiting();

	 Flat_QueryAnotherTeamInfoRsp* data = (Flat_QueryAnotherTeamInfoRsp*) ((Flat_MSG_Rsp*)msg)->getData();
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
	 vector<EquipData> heroEquipVec[4];  //按照 heroDetail排序
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
	 mQueryYuanShen = new CCArray;
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
		 addObserver(callfuncO_selector(MapsLayer::rsp_queryYuanShen), MSG_queryYuanshenOfHeroRsp_F);
		 for (int i=0; i!=heroDetailVec.size(); ++i)
		 {
			 if (heroDetailVec.at(i).basic_info.level >= 30)
				 mCmdHlp->cmdQueryYuanshenOfHero(heroDetailVec.at(i).basic_info.hero_id);
		 }
	 }
	 else
	 {
		 rsp_queryYuanShen(NULL);
	 }
 }

 void MapsLayer::rsp_queryYuanShen(CCObject *msg_f)
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

 void MapsLayer::callfuncND_ChangeMenuPriority(CCNode *node, void *priority)
 {
	 CCMenu *menu = dynamic_cast<CCMenu*>(node);
	 if (menu)
	 {
		 menu->setHandlerPriority((int)priority);
	 }
 }






 void addLabelToNode_AncPosColorSize(CCNode *node, CCString str, string font, int size, CCPoint anc, CCPoint pos, ccColor3B color)
 {
	 CCLabelTTF *label = CCLabelTTF::create(str.getCString(), font.c_str(), size);
	 label->setColor(color);
	 node->addChild(label);
	 label->setAnchorPoint(anc);
	 label->setPosition(pos);
 }
 void MapsLayer::addFriendCallBack(CCNode *node)
 {
	 AddFriendWarnLayer *layer = AddFriendWarnLayer::create();
	 layer->ignoreAnchorPointForPosition(false);
	 layer->setAnchorPoint(CCPointZero);
	 layer->setPosition(CCPointZero);
	 layer->setFriendName(getNewFirendName(node->getTag()));
	 CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);
 }



 CCTableViewCell* MapsLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
 {
	 CCTableViewCell *cell = table->dequeueCell();
	 if (cell) 
	 {
		 cell->removeFromParent();
	 }

	 cell = new CCTableViewCell();
	 cell->autorelease();

	 //说明1
	 CCLabelTTF *pShuoming1 = CCLabelTTF::create(LFStrings::getValue("ShouHuZhiLing_GuiZeShuoMing_01").c_str(), fontStr_kaiti, 20
		 ,CCSize(540,0), kCCTextAlignmentCenter);
	 pShuoming1->setColor(ccBLACK);
	 cell->addChild(pShuoming1);
	 pShuoming1->setAnchorPoint(ccp(0, 1));
	 pShuoming1->setPosition(ccp(30, 470));

	 //说明2
	 CCLabelTTF *pShuoming2 = CCLabelTTF::create(LFStrings::getValue("ShouHuZhiLing_GuiZeShuoMing_02").c_str(), fontStr_kaiti, 20
		 ,CCSize(540,0), kCCTextAlignmentLeft);
	 pShuoming2->setColor(ccBLACK);
	 cell->addChild(pShuoming2);
	 pShuoming2->setAnchorPoint(ccp(0, 1));
	 pShuoming2->setPosition(ccp(pShuoming1->getPositionX(), pShuoming1->getPositionY()-pShuoming1->getContentSize().height - 10));

	 //shuomning
	 CCLabelTTF *smLabel = CCLabelTTF::create(LFStrings::getValue("ShouHuZhiLing_GuiZeShuoMing_03").c_str(), fontStr_kaiti, 20
		 ,CCSize(540,0), kCCTextAlignmentLeft);
	 smLabel->setColor(ccBLACK);
	 cell->addChild(smLabel);
	 smLabel->setAnchorPoint(ccp(0, 1));
	 smLabel->setPosition(ccp(pShuoming2->getPositionX(), pShuoming2->getPositionY()-pShuoming2->getContentSize().height));

	 return cell;
 }


 CCSize MapsLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
 {
	 return CCSizeMake(600, 500);
 }

 unsigned int MapsLayer::numberOfCellsInTableView(CCTableView *table)
 {
	 return  1;
 }