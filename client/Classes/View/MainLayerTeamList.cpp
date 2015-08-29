#include "MainLayerTeamList.h"
#include "CS_Common.h"
#include "MyselfManager.h"
#include "GameDirector.h"
#include "MainLayerRenWuBody.h"
#include "History.h"
#include "MessageDisplay.h"
#include "ClientErrorCode.h"
#include "GuideLayerBase.h"
#define TAG_BG_MASK_PIC 22221

enum 
{
	Tag_HeadMenuItem_YiZhao = 1,
	Tag_HeadMenuItem_WeiZhao = 2,

	Tag_MainMenuItem_MakeSure,
	Tag_MainMenuItem_Cacel,

};



MainLayerTeamList::MainLayerTeamList(int flag)
	:MainLayerHerosBaseBody(flag)//	mFlag = flag;  // parent
{
	mBg = BaseSprite::create("buzhen_jiuguan_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2,mBg->getContentSize().height/2));

	mTableView->initWithViewSize(CCSize(610, 671));
	mTableView->retain();
	mTableView->removeFromParent();
	mBg->addChild(mTableView);
	mTableView->setPosition(ccp(15,100));
	mTableView->release();

	// head menu
	{
		CCMenuItemImage* yz = CCMenuItemImage::create(
			"buzhen_jiuguan_anniu_yizhaomu.png"
			, "buzhen_jiuguan_anniu_yizhaomu_select.png"
			, this, menu_selector(MainLayerTeamList::menuItemClicked_HeadMenu));
		CCMenuItemImage* wz = CCMenuItemImage::create(
			"buzhen_jiuguan_anniu_weizhaomu.png"
			, "buzhen_jiuguan_anniu_weizhaomu_select.png"
			, this, menu_selector(MainLayerTeamList::menuItemClicked_HeadMenu));
		yz->setPosition(ccp(138, mBg->getContentSize().height-42));
		yz->setTag(Tag_HeadMenuItem_YiZhao);
		wz->setPosition(ccp(500, mBg->getContentSize().height-42));
		wz->setTag(Tag_HeadMenuItem_WeiZhao);

		mHeadMenu = CCMenu::create(yz, wz, NULL);
		mBg->addChild(mHeadMenu);
		mHeadMenu->setPosition(CCPointZero);

		
	}

	//menu ok  cancel   if came from BuZhen
	{
		CCMenuItemImage* makeSure = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&MainLayerTeamList::menuItemClicked_MainMenu));
		makeSure->setTag(Tag_MainMenuItem_MakeSure);
		CCMenuItemImage* cacel = CCMenuItemImage::create(
			"shangpin_goumai_quxiao.png",
			"shangpin_goumai_quxiao_select.png",
			this,SEL_MenuHandler(&MainLayerTeamList::menuItemClicked_MainMenu));
		makeSure->setPosition(mWinSize.width/3, 50);
		cacel->setPosition(mWinSize.width/3*2, makeSure->getPositionY());
		cacel->setTag(Tag_MainMenuItem_Cacel);

		mMainMenu = CCMenu::create(makeSure, cacel, NULL);
		mBg->addChild(mMainMenu);
		mMainMenu->setPosition(ccp(0,0));

	}

	//
	{
		vector<BaseActor*> allActorVec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All);

		setHerosData(allActorVec);
	}

	if (mFlag == 1)
		menuItemClicked_HeadMenu(mHeadMenu->getChildByTag(Tag_HeadMenuItem_YiZhao));
	else 
		menuItemClicked_HeadMenu(mHeadMenu->getChildByTag(Tag_HeadMenuItem_WeiZhao));

	//酒馆出新助手 ---> 和工匠闪烁一样的效果   6、10级
	//int 类型    =6 6级提示过了   =10 10级提示过了
	int lv = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level;
	if (lv >= 6)
	{
		int newJiuGuan = 0;
		if(History::getHistory()->getIntFromCurrentHero(Key_JiuGuanZhuShou, newJiuGuan))
		{
			if (newJiuGuan < 6 && lv >= 6 && lv <= 10)
			{
				History::getHistory()->putInCurrentHero(Key_JiuGuanZhuShou, 6);
			}
			else if (newJiuGuan == 6 && lv >= 10)
			{
				History::getHistory()->putInCurrentHero(Key_JiuGuanZhuShou, 10);
			}
		}
		else
		{
			if (newJiuGuan < 6 && lv >= 6 && lv <= 10)
			{
				History::getHistory()->putInCurrentHero(Key_JiuGuanZhuShou, 6);
			}
		}
	}
}
void MainLayerTeamList::onEnter()
{
	MainLayerHerosBaseBody::onEnter();
	bool firstJiuGuan = false;
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJiuGuanIn) && GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJiuGuanPlay))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstJiuGuanIn);
		CCMenuItem *wz = (CCMenuItem *)mHeadMenu->getChildByTag(Tag_HeadMenuItem_WeiZhao);
		CCPoint pos = wz->getPosition();
		
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		guideLayer->init(wz->getContentSize(),ccp(pos.x , pos.y + 110)
			, LFStrings::getValue("JiuGuanJiaoXue"), NULL, false);
		mFooterLayer->getFooter()->addChild(guideLayer, 1024);
	}
}

void MainLayerTeamList::disableAllTouchBegin()
{
	MainLayerHerosBaseBody::disableAllTouchBegin();
}
void MainLayerTeamList::disableAllTouchEnd()
{
	MainLayerHerosBaseBody::disableAllTouchEnd();
}


MainLayerTeamList::~MainLayerTeamList(void)
{
}

void MainLayerTeamList::menuItemClicked_HeadMenu(CCObject* pSender)
{
	CCMenuItemImage* item = ((CCMenuItemImage*)pSender);
	if(item->getTag() == Tag_HeadMenuItem_YiZhao)
	{
		CCMenuItemImage* wz = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenuItem_WeiZhao);
		wz->initWithNormalImage(
			"buzhen_jiuguan_anniu_weizhaomu.png",
			"buzhen_jiuguan_anniu_weizhaomu_select.png", 
			NULL, this, menu_selector(MainLayerTeamList::menuItemClicked_HeadMenu));
		item->initWithNormalImage(
			"buzhen_jiuguan_anniu_yizhaomu_select.png", 
			"buzhen_jiuguan_anniu_yizhaomu.png",
			NULL, this, menu_selector(MainLayerTeamList::menuItemClicked_HeadMenu));

		mFlag = 1;
		mMainMenu->setVisible(true);

		vector<BaseActor*> allActorVec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All, LF_UnLock);
		setHerosData(allActorVec);
	}
	else if(item->getTag() == Tag_HeadMenuItem_WeiZhao)
	{

		CCMenuItemImage* yz = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenuItem_YiZhao);
		item->initWithNormalImage(
			"buzhen_jiuguan_anniu_weizhaomu_select.png", 
			"buzhen_jiuguan_anniu_weizhaomu.png",
			NULL, this, menu_selector(MainLayerTeamList::menuItemClicked_HeadMenu));
		yz->initWithNormalImage(
			"buzhen_jiuguan_anniu_yizhaomu.png",
			"buzhen_jiuguan_anniu_yizhaomu_select.png", 
			NULL, this, menu_selector(MainLayerTeamList::menuItemClicked_HeadMenu));

		mFlag = 2;
		mMainMenu->setVisible(false);

		vector<BaseActor*> allActorVec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All, LF_Locked);
		setHerosData(allActorVec);

		if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstJiuGuanIn))
		{
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstJiuGuanIn);
			vector<ShowMsg> list;
			ShowMsg msg;
			msg.msg = LFStrings::getValue("JiuGuanShuoMing");
			msg.color = fonColor_PuTong;

			list.push_back(msg);

			ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
				list, 3,this);
			if(layer != NULL)
			{
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
			}
			return;
		}
	}
}

void MainLayerTeamList::menuItemClicked_MainMenu(CCObject* pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MainMenuItem_MakeSure)
	{
		//向服务器提交选中的助手， 并跳转到 主界面
		addObserver(SEL_CallFuncO(&MainLayerTeamList::rsp_BuZhen),MSG_adjustFormationRsp);

		SPCmd_AdjustFormation req;
		for (int i=0; i<FormationHeroAcountMax; ++i)
		{
			req.hero_id_arr[i] = 0;
		}

		for(int i=0; i!=mChoseHeroState.size(); ++i)
		{
			switch(mChoseHeroState.at(i))
			{
			case 1:
				req.hero_id_arr[0] = mHerosVec.at(i)->getId();
				break;
			case 2:
				req.hero_id_arr[1] = mHerosVec.at(i)->getId();
				break;
			case 3:
				req.hero_id_arr[2] = mHerosVec.at(i)->getId();
				break;
			case 4:
				req.hero_id_arr[3] = mHerosVec.at(i)->getId();
				break;
			default:
				break;
			}
		}
		int ZhuJuePos = MyselfManager::getManager()->getMyZhuJueData()->getChessBoardPos();
		req.hero_id_arr[ZhuJuePos-1] = MyselfManager::getManager()->getMyZhuJueData()->getId();
		CmdHelper::getHelper()->cmdAdjustFormation(req.hero_id_arr);
	}
	else if (tag == Tag_MainMenuItem_Cacel)
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	}
}

void MainLayerTeamList::rsp_JieSuoYingXiong(CCObject *msg)
{
	removeObserver(MSG_unlockAssistantRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_UnlockAssistantRsp* data = (SPCmd_UnlockAssistantRsp*) ((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	MessageDisplay *layer = MessageDisplay::create(LFStrings::getValue("jieSuoChengGong"));
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);

	menuItemClicked_HeadMenu(mHeadMenu->getChildByTag(Tag_HeadMenuItem_YiZhao));
}

void MainLayerTeamList::rsp_BuZhen(CCObject* msg)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_adjustFormationRsp);

	SPCmd_AdjustFormationRsp* rsp = (SPCmd_AdjustFormationRsp*) ((MSG_Rsp*)msg)->getData();
	if (rsp->err_code == Success){
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}
	else {
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	
}
