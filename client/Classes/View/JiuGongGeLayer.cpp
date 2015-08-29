
#include "JiuGongGeLayer.h"
#include "MapsLayer.h"
#include "GameDirector.h"
#include "MapsManager.h"
#include "JGG_FootprintManager.h"
#include "GetTiLiLayer.h"
#include "FightPackage.h"
#include "GuideLayerBase.h"
#include "ChessBoardStateManager.h"
#include "History.h"
#include "MessageDisplay.h"
#include "CS_Common.h"
#include "ZhuangbeiScanner.h"
#include "GetAwardLayer.h"
#include "FXToast.h"
#include "ClientErrorCode.h"
#include "UnblockManager.h"
#include "ZhuShouChatLayer.h"
//#include "FightMapsTranslation.h"


static bool s_lastBossOK = false;
static bool s_needDisplayBossEffect = false;

const int TAG_Maps_BG  = 4643 + 10;
const int TAG_Menu   = 4643;
const int TAG_Maps1  = 4643 + 1;
const int TAG_Maps2  = 4643 + 2;
const int TAG_Maps3  = 4643 + 3;
const int TAG_Maps4  = 4643 + 4;
const int TAG_Maps5  = 4643 + 5;
const int TAG_Maps6  = 4643 + 6;
const int TAG_Maps7  = 4643 + 7;
const int TAG_Maps8  = 4643 + 8;
const int TAG_Maps9  = 4643 + 9;
const int TAG_Flag   = 4643 + 11;

enum 
{
	Tag_restart,
	Tag_beibao,
	Tag_MenuItem_LevUp,

	Tag_Box,
	Tag_BoxWindow_Item,


	Tag_TiLiTimer = 100,
	Tag_LabelTiLiNum = 105,

	Tag_JiFenBoard = 110,
	Tag_JiFenMenu,
	Tag_JiFenItem_Min,  //小怪 min
	Tag_JiFenItem_Max,  //精英怪 max

	Tag_Label_MinNeedKilled,
	Tag_Label_MaxNeedKilled,
	Tag_Label_MinCurKilled,
	Tag_Label_MaxCurKilled,

	Tag_BossBtn = 340,
};

JiuGongGeLayer::JiuGongGeLayer()
	:mFight(NULL)
	,mStart(false)
	,mTouchedIndex(-1)
	,m_sEyeFireSpr(NULL)
	,m_nActionCount(0)
	,m_rJiugonggeMenuRect(CCRectZero)
{


}

JiuGongGeLayer::~JiuGongGeLayer()
{
	CC_SAFE_DELETE(mFight);
}

JiuGongGeLayer* JiuGongGeLayer::create()
{
	JiuGongGeLayer* layer = new JiuGongGeLayer;

	if (layer && layer->init())
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

bool JiuGongGeLayer::init()
{
	if (! CCLayerColor::initWithColor(ccc4(0,0,0,0)))
	{
		return false;
	}


	do 
	{
		mMainMenu = CCMenu::create();
		addChild(mMainMenu, 10);
		mMainMenu->setPosition(CCPointZero); // beibao   fanhui

		mWinSize = CCDirector::sharedDirector()->getWinSize();
//		vector<bool> jggFoot = JGG_FootprintManager::getManager()->getFootprintById(g_taskId);

		//1.back layer
		CCSprite *maps;
		int id = TaskManager::getManager()->getCurrentFightTaskId();

		maps = MapsManager::getManager()->getBackGroundMapImage(id);	
		addChild(maps, 0, TAG_Maps_BG);
		maps->setAnchorPoint(ccp(0.5, 1));
		maps->setPosition(ccp(mWinSize.width/2, mWinSize.height));
		
		//1.5 半透明背景
		CCSprite* btm = CCSprite::create("zhandou_jiugongge_touming.png");
		addChild(btm, 0, TAG_Maps_BG+10);
		btm->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
		btm->setOpacity(200);

		//获得当前任务有多少个格子
		TaskChessBoard chessboard;
		if(! ChessBoardStateManager::getManager()->getChessBoard(TaskManager::getManager()->getCurrentFightTaskId(), chessboard))
		{
		//	CCAssert(false, "");
			return false;
		}
		int taskGridNum = chessboard.numInCol * chessboard.numInRow;

		mTaskGridInfoList = chessboard.gridState;

		// 2. map
//		jggMap = NULL;
		int tiliBar_deltaY = 20;
		//BOSS按钮背景
		CCSprite* pBossBtnBg = NULL;
		CCSize size = CCSizeMake(116, 120);
		CCPoint startPoint = ccp(mWinSize.width/2, mWinSize.height/2);
		int xCount = 1;
		int yCount = 1;
		int yOffset = 0;
		if (taskGridNum == 0)
		{
//			jggMap = BaseSprite::create("zhandou_jiugongge_ditu1.png");
			pBossBtnBg = CCSprite::create("jiugongge_shikuai_weitansuo_boss_biankuang_2.png");
		}
		else if(taskGridNum == 3)
		{
//			jggMap = BaseSprite::create("zhandou_jiugongge_ditu3.png");
			startPoint = ccp(startPoint.x - size.width, startPoint.y + size.height);
			pBossBtnBg = CCSprite::create("jiugongge_shikuai_weitansuo_boss_biankuang_1.png");
			xCount = 3;
		}
		else if (taskGridNum == 4)
		{
//			jggMap = BaseSprite::create("zhandou_jiugongge_ditu4.png");
			startPoint = ccp(startPoint.x - size.width*0.5f, startPoint.y + size.height*0.5f);
			pBossBtnBg = CCSprite::create("jiugongge_shikuai_weitansuo_boss_biankuang_1.png");
			xCount = 2;
			yCount = 2;
		}
		else if (taskGridNum == 6)
		{
//			jggMap = BaseSprite::create("zhandou_jiugongge_ditu6.png");
			startPoint = ccp(startPoint.x - size.width, startPoint.y + size.height);
			pBossBtnBg = CCSprite::create("jiugongge_shikuai_weitansuo_boss_biankuang_1.png");
			xCount = 3;
			yCount = 2;
		}
		else if(taskGridNum == 9)
		{
//			jggMap = BaseSprite::create("zhandou_jiugongge_ditu9.png");
			tiliBar_deltaY = 0;
			startPoint = ccp(startPoint.x - size.width, startPoint.y + size.height);
			pBossBtnBg = CCSprite::create("jiugongge_shikuai_weitansuo_boss_biankuang_1.png");
			xCount = 3;
			yCount = 3;
		}
		else if(taskGridNum == 16)
		{
//			jggMap = BaseSprite::create("zhandou_jiugongge_ditu9.png");
			tiliBar_deltaY = 0;
			startPoint = ccp(startPoint.x - size.width*1.5f, startPoint.y + size.height*1.5f);
			pBossBtnBg = CCSprite::create("jiugongge_shikuai_weitansuo_boss_biankuang_2.png");
			xCount = 4;
			yCount = 4;
			yOffset = 50;
		}
		else
		{
			CCAssert(false, "");
		}
//		addChild(jggMap);
//		jggMap->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));

		if (pBossBtnBg)
		{
			addChild(pBossBtnBg);
			pBossBtnBg->setPosition(ccp(mWinSize.width/2, startPoint.y+pBossBtnBg->getContentSize().height/2-yOffset));

			CCMenu *menu = CCMenu::create();
			addChild(menu,2,TAG_Menu);
			menu->setPosition(CCPointZero);

			CCMenuItemImage* pBossBtn = CCMenuItemImage::create(
				"jiugongge_shikuai_yitansuo_boss.png",
				"jiugongge_shikuai_weitansuo_boss.png",
				"jiugongge_shikuai_siwang_boss.png",
				this, menu_selector(JiuGongGeLayer::btnCallback));
			m_iBossMenuItem = pBossBtn;
			menu->addChild(pBossBtn);
			pBossBtn->setTag(Tag_BossBtn);
			if (taskGridNum == 0 || chessboard.gridState.at(0).stat == TaskGridStat_Floped)
			{
				pBossBtn->setEnabled(true);
			}
			else
			{
				pBossBtn->setEnabled(false);
			}
			
			pBossBtn->setPosition(ccp(pBossBtnBg->getPositionX(), pBossBtnBg->getPositionY()+10));

			CCPoint tempPoint = startPoint;
			tempPoint.y = tempPoint.y - size.height/2 - yOffset +10;

			//创建九宫格
			for (int i = 1; i <= taskGridNum; ++i)
			{
				string normal = "jiugongge_shikuai_weitansuo.png";
				string disable = "jiugongge_shikuai_yitansuo_kong.png";
				//已翻牌
				if (chessboard.gridState.at(i).stat == TaskGridStat_Floped)
				{
					normal = "jiugongge_shikuai_yitansuo_kong.png";
				}

				CCMenuItemImage* pItem = CCMenuItemImage::create(
					normal.c_str(),
					normal.c_str(),
					disable.c_str(),
					this, menu_selector(JiuGongGeLayer::btnCallback));
				pItem->setTag(Tag_BossBtn+i);
				menu->addChild(pItem);
				if (i == 1)
				{
					m_iWenhaoMenuItem = pItem;
					pItem->setPosition(ccp(tempPoint.x, tempPoint.y));
				}
				else if ((i-1) % xCount == 0 && i != 1)
				{
					pItem->setPosition(ccp(startPoint.x, tempPoint.y-pItem->getContentSize().height));
				}
				else
				{
					pItem->setPosition(ccp(tempPoint.x+pItem->getContentSize().width, tempPoint.y));
				}
				tempPoint = pItem->getPosition();

				//已扣牌设置成false
				if (chessboard.gridState.at(i).stat == TaskGridStat_Folded)
				{
					pItem->setEnabled(false);
					mTouchedIndex = i;
					this->showUnLockSpr(chessboard.gridState.at(i), true);
				}
				//已翻牌
				else if (chessboard.gridState.at(i).stat == TaskGridStat_Floped)
				{
					mTouchedIndex = i;
					this->showUnLockSpr(chessboard.gridState.at(i), true);
				}

				if (i == 1)
				{
					m_rJiugonggeMenuRect.origin.x = pItem->getPositionX();
					m_rJiugonggeMenuRect.origin.y = pItem->getPositionY();
				}

				if (m_rJiugonggeMenuRect.origin.x < pItem->getPositionX())
				{
					m_rJiugonggeMenuRect.size.width = (pItem->getPositionX() - m_rJiugonggeMenuRect.origin.x) * 2;
					m_rJiugonggeMenuRect.origin.x = pItem->getPositionX();
				}
				if (m_rJiugonggeMenuRect.origin.y > pItem->getPositionY())
				{
					m_rJiugonggeMenuRect.size.height = (m_rJiugonggeMenuRect.origin.y - pItem->getPositionY()) * 2;
					m_rJiugonggeMenuRect.origin.y = pItem->getPositionY();
				}
			}

			//3.体力
			{
// 				BaseSprite *labelT = BaseSprite::create("zhuye_tili_wenzi.png");
// 				addChild(labelT, 1);
// 				labelT->setAnchorPoint(ccp(1, 0.5f));
				{
					BaseSprite* tiLiBarBoarderBg = BaseSprite::create("jiugongge_shikuai_weitansuo_tili_biankuang.png");
					addChild(tiLiBarBoarderBg);
					tiLiBarBoarderBg->setPosition(ccp(mWinSize.width/2, tempPoint.y - size.height/2 - tiLiBarBoarderBg->getContentSize().height/2));

					if (taskGridNum == 0)
					{
						tiLiBarBoarderBg->setPosition(ccp(mWinSize.width/2, tempPoint.y + size.height/2 - tiLiBarBoarderBg->getContentSize().height/2));
					}

					float currentTili = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current;
					CCProgressTimer* tiLiBar = LF::createProgressTimer(BaseSprite::create("zhuye_tili.png"));
					addChild(tiLiBar, 1, Tag_TiLiTimer);
					tiLiBar->setPosition(tiLiBarBoarderBg->getPosition());
					tiLiBar->setPercentage((currentTili)/(float)Vitality_Total * 100.0f);
//					labelT->setPosition(ccp(jggMap->getPositionX()-80, tiLiBar->getPositionY()));

					BaseSprite* tiLiBarBoarder = BaseSprite::create("zhuye_tili_bg.png");
					addChild(tiLiBarBoarder);
					tiLiBarBoarder->setPosition(tiLiBar->getPosition());

					//num
					CCString temp;
					temp.initWithFormat("%d/%d", (int)currentTili, Vitality_Total);
					CCLabelTTF *tiliLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 15);
					tiliLabel->setPosition(this->convertToNodeSpace(
						tiLiBarBoarder->convertToWorldSpace(ccp(tiLiBarBoarder->getContentSize().width/2, tiLiBarBoarder->getContentSize().height/2))));
					addChild(tiliLabel, 2, Tag_LabelTiLiNum);
				}
			}
		}

		//3.5 积分
		{
			mJiFenBoard = BaseSprite::create("zhandou_jiugongge_qingjiao_bg.png");
			addChild(mJiFenBoard, 1, Tag_JiFenBoard);
			mJiFenBoard->setPosition(ccp(mWinSize.width/2, mWinSize.height - 75));
			//menu
			CCMenu *jfMenu = CCMenu::create();
			mJiFenBoard->addChild(jfMenu, 1, Tag_JiFenMenu);
			jfMenu->setPosition(ccp(0,0));
			CCMenuItemImage *minItem = CCMenuItemImage::create(
				"zhandou_jiugongge_lingqu.png",
				"zhandou_jiugongge_lingqu_select.png",
				this, menu_selector(JiuGongGeLayer::menuItemClicked_JiFenGetAward));
			jfMenu->addChild(minItem, 0, Tag_JiFenItem_Min);
			minItem->setPosition(ccp(504, 93));
			CCMenuItemImage *maxItem = CCMenuItemImage::create(
				"zhandou_jiugongge_lingqu.png",
				"zhandou_jiugongge_lingqu_select.png",
				this, menu_selector(JiuGongGeLayer::menuItemClicked_JiFenGetAward));
			jfMenu->addChild(maxItem, 0, Tag_JiFenItem_Max);
			maxItem->setPosition(ccp(504, 42));
			//普通 kill
			CCString temp;
			unsigned int minNeedKilled = CS::cashBonusNeedKilledMonsters(1);
			unsigned int minCurKilled  = ChessBoardStateManager::getManager()->getKilledPutongMonsterNum();
			temp.initWithFormat("%s%s%s:%u/", LFStrings::getValue("JGG_JiShaJiangLi_").c_str(), LFStrings::getValue("xiaoGuai").c_str(),
				LFStrings::getValue("shuliang").c_str(), minNeedKilled);
			CCLabelTTF *minLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			mJiFenBoard->addChild(minLabel, 0, Tag_Label_MinNeedKilled);
			minLabel->setAnchorPoint(ccp(0, 0.5));
			minLabel->setPosition(ccp(84, 93));
			//
			temp.initWithFormat("%u", minCurKilled);
			CCLabelTTF *minCurLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
			mJiFenBoard->addChild(minCurLabel, 0, Tag_Label_MinCurKilled);
			minCurLabel->setPosition(ccp(minLabel->getPositionX()+minLabel->getContentSize().width+minCurLabel->getContentSize().width/2,
				minLabel->getPositionY()));
			if (minNeedKilled > minCurKilled)
			{
				minLabel->setColor(fonColor_CiYao);
				minCurLabel->setColor(fonColor_JingGao);
			}
			else
			{
				minCurLabel->setColor(fonColor_ChengGong);
			}
			//精英 kill
			unsigned int maxNeedKilled = CS::cashBonusNeedKilledMonsters(2);
			unsigned int maxCurKilled  = ChessBoardStateManager::getManager()->getKilledJingYinMonsterNum();
			temp.initWithFormat("%s%s%s:%u/", LFStrings::getValue("JGG_JiShaJiangLi_").c_str(), LFStrings::getValue("jingYingGuai").c_str(),
				LFStrings::getValue("shuliang").c_str(), maxNeedKilled);
			CCLabelTTF *maxLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			mJiFenBoard->addChild(maxLabel, 0, Tag_Label_MaxNeedKilled);
			maxLabel->setAnchorPoint(ccp(0, 0.5));
			maxLabel->setPosition(ccp(84, 42));
			//
			temp.initWithFormat("%u", maxCurKilled);
			CCLabelTTF *maxCurLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
			mJiFenBoard->addChild(maxCurLabel, 0, Tag_Label_MaxCurKilled);
			maxCurLabel->setPosition(ccp(maxLabel->getPositionX()+maxLabel->getContentSize().width+maxCurLabel->getContentSize().width/2,
				maxLabel->getPositionY()));
			if (maxNeedKilled > maxCurKilled)
			{
				maxLabel->setColor(fonColor_CiYao);
				maxCurLabel->setColor(fonColor_JingGao);
			}
			else
			{
				maxCurLabel->setColor(fonColor_ChengGong);
			}
		}

 		float visWidth = CCDirector::sharedDirector()->getVisibleSize().width;
		//退出按钮

		CCMenuItemImage *item = CCMenuItemImage::create(
			"zhandou_jiesuan_anniu_tuichu.png"
			,"zhandou_jiesuan_anniu_tuichu_select.png"
			,this, SEL_MenuHandler(&JiuGongGeLayer::menuItemClicked_MainMenu));	
		mMainMenu->addChild(item, 1, Tag_restart);
		item->setPosition(ccp(visWidth/2, item->getContentSize().height/2+30));
		item->setVisible(true);
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstMaoXian))
		{
			item->setVisible(false);
		}
		//5.背包 
		CCMenuItemImage *bag = CCMenuItemImage::create(
			"zhandou_beibao.png"
			, "zhandou_beibao_select.png"
			, this, menu_selector(JiuGongGeLayer::menuItemClicked_MainMenu));
		mMainMenu->addChild(bag, 1, Tag_beibao);
		bag->setAnchorPoint(ccp(1, 0.5));
		bag->setPosition(ccp(visWidth, 10+bag->getContentSize().height/2));
		//若果有好装备  则背包按钮换成换装提示
		vector<BaseActor*> heroVec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
		bool bMarked = false;
		for (int heroIdx=0; heroIdx<heroVec.size(); ++heroIdx)
		{
			if (bMarked) break;
			TestReport actReport = ZhuangbeiScanner::getScanner()->getTestReportByActor(heroVec.at(heroIdx));
			if (actReport.reportList.size() > 0 && actReport.reportList.at(0).actor == heroVec.at(heroIdx))
			{
				vector<Equipment*> repEqVec = actReport.reportList.at(0).betterEquipVec;
				for (int iidx=0; iidx<repEqVec.size(); ++iidx)
				{
					if (bMarked) break;
					vector<EquipRequirePlace> placeVec = repEqVec.at(iidx)->getRequiredPlaces();
					for (int placeIdx=0; placeIdx<placeVec.size(); ++placeIdx)
					{
						//如果没有忽略
						//并且报表中这件装备就是跟当前位置在做比较，那么就算是有
						if (! ZhuangbeiScanner::getScanner()->isIgnoreHint(actReport.reportList.at(0).actor, placeVec.at(placeIdx))&& 
							actReport.reportList[0].oldEquipVec[iidx] == actReport.reportList.at(0).actor->getEquipmentAt(placeVec[placeIdx]))
						{
							bag->initWithNormalImage(
								"zhandou_huanzhuang.png"
								, "zhandou_huanzhuang_select.png"
								, NULL, this, menu_selector(JiuGongGeLayer::menuItemClicked_HuanZhuang));
							bag->setAnchorPoint(ccp(1, 0.5));
							bag->setPosition(ccp(visWidth, 10+bag->getContentSize().height/2));

							//箭头
							BaseSprite *tishiImg = BaseSprite::create("zhuye_gongjitisheng.png");
							bag->addChild(tishiImg);
							tishiImg->setPosition(ccp(bag->getContentSize().width/2, bag->getContentSize().height+tishiImg->getContentSize().height/2-20));
							tishiImg->runAction(CCRepeatForever::create(
								CCSequence::create(CCMoveBy::create(0.5f, ccp(0, 20)), CCMoveBy::create(0.5f, ccp(0, -20)), NULL)));
							//
							bMarked = true;
							break;
						}
					}
				}
			}
		}

 	} while (0);

	TaskId lastId = TaskManager::getManager()->getLastActivedMainTaskId();
	TaskStatus lastStat = TaskManager::getManager()->getLastActivedMainTaskStatus();
	
	if (TaskManager::getManager()->getCurrentFightTaskId() == lastId && lastStat == TaskStatus_Got && !s_lastBossOK && mTaskGridInfoList.at(0).stat == TaskGridStat_Floped ){
		s_needDisplayBossEffect = true;
		s_lastBossOK = true;
	}else if (TaskManager::getManager()->getCurrentFightTaskId() == lastId && lastStat == TaskStatus_Got && mTaskGridInfoList.at(0).stat == TaskGridStat_PreFlop){
		s_lastBossOK = false;
	}else {
		s_lastBossOK = true;
	}

	if (s_needDisplayBossEffect) {
		s_needDisplayBossEffect = false;
		addBossEffects(m_iBossMenuItem);
	}
	else {
		scheduleOnce(schedule_selector(JiuGongGeLayer::schedule_displayNewZone), 0); //有boss动画就在boss动画完了之后展示
	}

	return true;
}

void JiuGongGeLayer::btnCallback(CCObject* pObj)
{
	if(mStart)
		return;

	CCMenuItemImage* pItem = (CCMenuItemImage*)pObj;
	int iTag = pItem->getTag();

	bool bCanPlay = true;
	float currentTili = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current;
	if (iTag == Tag_BossBtn)
	{
		if (currentTili < 12)
		{
			bCanPlay = false;
		}
	}
	else
	{
		if (currentTili < 2)
		{
			bCanPlay = false;
		}
	}

	if (!bCanPlay)
	{
		GetTiLiLayer* layer = GetTiLiLayer::create(this);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);
		return;
	}

	if (! EquipmentManager::getManager()->isSpareEnoughToSaveEquip(1))
	{
		EquipBagNotEnoughDialog *layer = new EquipBagNotEnoughDialog;
		layer->showGoToFenJieDialog();
		return;
	}

	disableMenuClicked();

	pItem->setEnabled(false);

	mTouchedIndex = iTag-Tag_BossBtn;
	switch (iTag)
	{
	case Tag_BossBtn:
		{
			CCLOG("......Tag_BossBtn is click......");
			mGrid = mTouchedIndex;
			CC_SAFE_DELETE(mFight);
			mFight = new FightPrepare(this);
			mFight->prepareData(TaskManager::getManager()->getCurrentFightTaskId(), mTouchedIndex);
			this->showKouChuTiLiAni();
		}
		break;

	default:
		{
			this->clickDone(pItem);
		}
		break;
	}
}

void JiuGongGeLayer::clickDone(CCMenuItemImage* pItem)
{
	int iTag = pItem->getTag();
	pItem->setEnabled(false);

	if (mTaskGridInfoList.at(mTouchedIndex).stat == TaskGridStat_Floped)
	{
		this->startFight();
	}
	else
	{
 		this->showPoSuiEffect();
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(JiuGongGeLayer::openBoxRsp), MSG_FlopTaskGridRsp, NULL);
		CmdHelper::getHelper()->cmdFlopTaskGrid(TaskManager::getManager()->getCurrentFightTaskId(), mTouchedIndex);
	}

// 	TaskGridInfo* info = (TaskGridInfo*)(pItem->getUserData());
// 	if (info && info->type == TaskGridType_Box)
// 	{
// 		CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(JiuGongGeLayer::openBoxRsp), MSG_FlopTaskGridRsp, NULL);
// 		CmdHelper::getHelper()->cmdFlopTaskGrid(g_taskId, mTouchedIndex+1);
// 	}
// 	else if (info && info->type == TaskGridType_Empty)
// 	{
// 		enableMenuClicked();
// 	}
// 	else if (info && info->type == TaskGridType_Monster)
// 	{
// 		this->showUnLockSpr(*info);
// 		pItem->runAction(CCSequence::create(
// 							CCDelayTime::create(10.5f),
// 			 				CCCallFuncN::create(this, callfuncN_selector(JiuGongGeLayer::startFight)),
// 			 				NULL));
// 	}
}

void JiuGongGeLayer::showPoSuiEffect()
{

	CCPointArray* pPointArray = CCPointArray::create(25);
	pPointArray->addControlPoint(ccp(53.417934417724609,-39.747043609619141));
	pPointArray->addControlPoint(ccp(28.799232482910156,-47.354881286621094));
	pPointArray->addControlPoint(ccp(33.736530303955078,-27.054588317871094));
	pPointArray->addControlPoint(ccp(6.5074748992919922,-26.911520004272461));
	pPointArray->addControlPoint(ccp(-15.638858795166016,-37.567729949951172));

	pPointArray->addControlPoint(ccp(-39.544391632080078,-41.617088317871094));
	pPointArray->addControlPoint(ccp(-44.6129035949707,-22.074634552001953));
	pPointArray->addControlPoint(ccp(52.874477386474609,-16.98021125793457));
	pPointArray->addControlPoint(ccp(17.386754989624023,-15.795111656188965));
	pPointArray->addControlPoint(ccp(-8.3644847869873047,-10.657751083374023));

	pPointArray->addControlPoint(ccp(-27.668827056884766,-17.115524291992188));
	pPointArray->addControlPoint(ccp(40.1406135559082,-1.4771313667297363));
	pPointArray->addControlPoint(ccp(9.8472747802734375,1.9302158355712891));
	pPointArray->addControlPoint(ccp(-12.111011505126953,9.0894832611083984));
	pPointArray->addControlPoint(ccp(-39.458812713623047,3.5603690147399902));

	pPointArray->addControlPoint(ccp(-48.499832153320312,16.675754547119141));
	pPointArray->addControlPoint(ccp(-0.35918045043945312,31.995716094970703));
	pPointArray->addControlPoint(ccp(-24.771081924438477,28.502117156982422));
	pPointArray->addControlPoint(ccp(54.952419281005859,17.080930709838867));
	pPointArray->addControlPoint(ccp(29.214466094970703,26.543331146240234));

	pPointArray->addControlPoint(ccp(50.427833557128906,47.780181884765625));
	pPointArray->addControlPoint(ccp(24.195096969604492,44.53826904296875));
	pPointArray->addControlPoint(ccp(6.5607714653015137,55.921302795410156));
	pPointArray->addControlPoint(ccp(-22.055282592773438,51.647911071777344));
	pPointArray->addControlPoint(ccp(-43.15118408203125,37.924953460693359));

	CCMenuItemImage* pItem = this->getBtnItem(mTouchedIndex);
	if (!pItem)
	{
		return;
	}
	CCPoint point;
	CCPoint relativeBombCenterPos = ccp(mWinSize.width/2, mWinSize.height/2);
	float fadeOutTime = 0.15f, fadetime = 0.75f;
	for (int i = 0; i < 25; ++i)
	{
		CCSprite* pSpr = CCSprite::create(CCString::createWithFormat("jiugongge_shikuai_suilie_%d.png", i+1)->getCString());
		if (pSpr)
		{
			point = pPointArray->getControlPointAtIndex(24-i);
			this->addChild(pSpr, 11);
			pSpr->setPosition(ccp(pItem->getPositionX()+point.x, pItem->getPositionY()+point.y));
 			float dt = 0.35f;
			pSpr->runAction(
				CCSequence::create(
					CCSpawn::create(
						CCMoveBy::create(dt, ccp(point.x*0.4, point.y*0.4)),
						CCFadeOut::create(dt),
						NULL),
					CCRemoveSelf::create(),
					NULL));
		}
	}

	CCParticleSystem* emitter = new CCParticleFire();
	// 48*48 的面积 用100个
	float num = pItem->getContentSize().width*pItem->getContentSize().height / (48*48) * 100;
	emitter->initWithTotalParticles(int(num));
	emitter->setBlendAdditive(false);
	emitter->setAutoRemoveOnFinish(true);
	emitter->setDuration(0.3f);
	this->addChild(emitter, 10);
	emitter->setTexture( CCTextureCache::sharedTextureCache()->addImage("fire.png"));
	emitter->setPosition(pItem->getPosition());
	emitter->setPosVar(ccp( pItem->getContentSize().width/2, pItem->getContentSize().height/2));
	emitter->setStartColor(ccc4f(0.2f,0.2f,0.2f,0.1f));
	emitter->setEndColor(ccc4f(0.65f,0.65f,0.65f,0.03f));
	emitter->setEndColorVar(ccc4f(0.0f,0.0f,0.0f,0.0f));
	emitter->setLife(0.5f);
	emitter->setLifeVar(0.2f);
	emitter->setSpeed(30.0f);
	emitter->setSpeedVar(20.0f);
	emitter->setScale(getScale());
	emitter->setRotation(getRotation());

}

void JiuGongGeLayer::showUnLockSpr(TaskGridInfo info,  bool bFirst /* = false*/)
{
	if (info.stat == TaskGridType_Box)
	{
		enableMenuClicked();
	}
	mGrid = mTouchedIndex;
	CCMenuItemImage* pItem = this->getBtnItem(mTouchedIndex);
	if (pItem)
	{
		if (info.stat == TaskGridStat_Folded)
		{
			pItem->setEnabled(false);
		}
		else
		{
			pItem->setEnabled(true);
		}

		CCSprite* pSpr = NULL;
		if (info.type == TaskGridType_Box)
		{
			pSpr = CCSprite::create("jiugongge_shikuai_yitansuo_baoxiang_lock.png");
		}
		else if (info.type == TaskGridType_Empty)
		{
			enableMenuClicked();
		}
		else if (info.type == TaskGridType_Monster)
		{
			if (info.monster_type == GuanQiaType_JingYing)
			{
				if (info.stat == TaskGridStat_Folded)
				{
					pSpr = CCSprite::create("jiugongge_shikuai_yitansuo_guaiwu_jingying_lock.png");
				}
				else
				{
					pSpr = CCSprite::create("jiugongge_shikuai_yitansuo_guaiwu_jingying.png");
				}
			}
			else if (info.monster_type == GuanQiaType_Normal)
			{
				if (info.stat == TaskGridStat_Folded)
				{
					pSpr = CCSprite::create("jiugongge_shikuai_yitansuo_guaiwu_lock.png");
				}
				else
				{
					pSpr = CCSprite::create("jiugongge_shikuai_yitansuo_guaiwu.png");
				}
			}
		}

		if (!bFirst)
		{
			//不是未翻牌状态的就不用扣除体力
			if (info.stat != TaskGridStat_Floped)
			{
				if (info.stat != TaskGridType_Box)
				{
					return;
				}
			}
			this->showKouChuTiLiAni();
			this->setTiliTTF();
		}

		if (pSpr)
		{
			this->addChild(pSpr, 3);
			pSpr->setPosition(pItem->getPosition());
			if (bFirst)
			{
				return;
			}

			if (info.type != TaskGridType_Box)
			{
				pSpr->runAction(CCSequence::create(
					CCDelayTime::create(1.0f),
					CCCallFuncN::create(this, callfuncN_selector(JiuGongGeLayer::startFight)),
					NULL));
			}
		}
	}
}

void JiuGongGeLayer::showKouChuTiLiAni()
{
	//扣除体力
	string valeStr = "jiugongge_tili_jian2.png";
	if (mTouchedIndex == 0)
	{
		valeStr = "jiugongge_tili_jian12.png";
	}

	CCNode* pItem = this->getBtnItem(mTouchedIndex);

//	valeStr = LFStrings::getValue("tili") + valeStr;
//	CCLabelTTF* pTili = CCLabelTTF::create(valeStr.c_str(), fontStr_kaiti, 48);
	CCSprite* pTiliSpr = CCSprite::create(valeStr.c_str());
	pTiliSpr->setPosition(pItem->getPosition());
	this->addChild(pTiliSpr, 15);
	CCScaleTo *scale = CCScaleTo::create(0.2f, 0.8f);
	CCMoveBy *move = CCMoveBy::create(2, ccp(0, 90));
	CCFadeTo *fade = CCFadeTo::create(1.5f, 0);
	pTiliSpr->runAction(CCSequence::create(
		CCDelayTime::create(0.4f)
		, CCSpawn::create(scale, NULL)
		, CCDelayTime::create(0.1f)
		, CCSpawn::create(move, fade, NULL)
		, CCRemoveSelf::create()
		, NULL));
}

void JiuGongGeLayer::openBoxRsp(CCObject* pObj)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_FlopTaskGridRsp);
	GameDirector::getDirector()->hideWaiting();

	Flat_FlopTaskGridRsp* flat_data = (Flat_FlopTaskGridRsp*)((Flat_MSG_Rsp*)pObj)->getData();
	if (flat_data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(flat_data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		enableMenuClicked();
		this->setBtnState(mTouchedIndex, Normal_State);
		return;
	}

	//是否该显示boss效果
	{
		TaskId lastId = TaskManager::getManager()->getLastActivedMainTaskId();
		TaskStatus lastStat = TaskManager::getManager()->getLastActivedMainTaskStatus();
		if (TaskManager::getManager()->getCurrentFightTaskId() == lastId && lastStat == TaskStatus_Got && !s_lastBossOK && flat_data->boss_stat == TaskGridStat_Floped ){
			s_needDisplayBossEffect = true;
			s_lastBossOK = true;
		}else if (TaskManager::getManager()->getCurrentFightTaskId() == lastId && lastStat == TaskStatus_Got && flat_data->boss_stat == TaskGridStat_PreFlop){
			s_lastBossOK = false;
		}else {
			s_lastBossOK = true;
		}
	}

	if (flat_data->items.size() > 0)
	{
		AwardDisplay *layer = AwardDisplay::create(LFStrings::getValue("JiuGongGe_KaiQiBox"), flat_data->items);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 10);
		this->setBtnState(mTouchedIndex, Disable_State);
	}

	if (mTouchedIndex < 0)
	{
		enableMenuClicked();
		return;
	}

	TaskChessBoard chessboard;
	if(! ChessBoardStateManager::getManager()->getChessBoard(TaskManager::getManager()->getCurrentFightTaskId(), chessboard))
	{
		//	CCAssert(false, "");
		return;
	}
	mTaskGridInfoList.clear();
	mTaskGridInfoList = chessboard.gridState;
	this->showUnLockSpr(mTaskGridInfoList.at(mTouchedIndex));

	if (mTaskGridInfoList.at(0).stat != TaskGridStat_PreFlop)
	{
		if (s_needDisplayBossEffect) {
			s_needDisplayBossEffect = false;
			addBossEffects(m_iBossMenuItem);
		}
	}


	TaskId id = TaskManager::getManager()->getLastActivedMainTaskId();
	TaskStatus statu = TaskManager::getManager()->getLastActivedMainTaskStatus();
	
	if (id == TaskManager::getManager()->getCurrentFightTaskId() && statu == TaskStatus_Got) {
		
	}
	

// 	if (pItem)
// 	{
// 		CCSprite* pSpr = CCSprite::create("jiugongge_shikuai_yitansuo_baoxiang.png");
// 		pSpr->setPosition(pItem->getPosition());
// 
// 		pSpr->runAction(CCSequence::create(
// 			CCDelayTime::create(1.0f),
// 			CCCallFuncN::create(this, callfuncN_selector(JiuGongGeLayer::startFight)),
// 			NULL));
// 	}
}

void JiuGongGeLayer::startFight(CCNode* pSpr /*= NULL*/)
{
	disableMenuClicked();
	CC_SAFE_DELETE(mFight);
	mFight = new FightPrepare(this);
	mFight->prepareData(TaskManager::getManager()->getCurrentFightTaskId(), mTouchedIndex);
}

void JiuGongGeLayer::menuItemClicked_HuanZhuang(CCObject *pSender)
{
	if(mStart)
		return;
	GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
}
void JiuGongGeLayer::teachGuide()
{
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstMaoXian))
	{
		//bool isTeach = GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstMaoXian);
		//if (isTeach)
		{
			if (GameGuideManager::getManager()->getCurGuideState() == 3)
			{
				vector<ShowMsg> list;
				ShowMsg msg1;
				msg1.msg = LFStrings::getValue("FirstFight");
				msg1.color = ccWHITE;

				ShowMsg msg2;
				msg2.msg = LFStrings::getValue("ZhuangBei");
				msg2.color = ccc3(181,93,6);

				ShowMsg msg3;
				msg3.msg = LFStrings::getValue("GoToMaoXian");
				msg3.color = ccWHITE;

				list.push_back(msg1);
				list.push_back(msg2);
				list.push_back(msg3);

				ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
					list, 1,this);
				if(layer != NULL)
				{
					
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
				}
				return;
			}
			else
			{
				if (m_iBossMenuItem)
				{
					GameGuideManager::getManager()->setCurGuideState(1);
					CCPoint pos = m_iBossMenuItem->getParent()->convertToWorldSpace(ccp(m_iBossMenuItem->getPositionX(),m_iBossMenuItem->getPositionY()));
					pos.y -= (m_iBossMenuItem->getContentSize().height + 25.0f);
					GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;					

					guideLayer2->init(m_rJiugonggeMenuRect.size,pos
						, LFStrings::getValue("jiugongge_TongguanSuoming"), NULL, true);

					GuideLayerBase *guideLayer1 = new GuideLayer_SimpleLayer;
					guideLayer1->init(m_iBossMenuItem->getContentSize(), m_iBossMenuItem->getParent()->convertToWorldSpace(m_iBossMenuItem->getPosition())
						, LFStrings::getValue("jiugongge_Bossshuoming"), guideLayer2, true);
					this->addChild(guideLayer1, 128);
					GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstMaoXian);
				}
			}
		}
	}
}
void JiuGongGeLayer::onEnter()
{
	CCLayerColor::onEnter();
	teachGuide();
}
void JiuGongGeLayer::ZhuShouchatEndCallBack()
{
	if (m_iWenhaoMenuItem)
	{
		GuideLayerBase *guideLayer1 = new GuideLayer_SimpleLayer;
		guideLayer1->init(m_iWenhaoMenuItem->getContentSize(), m_iWenhaoMenuItem->getParent()->convertToWorldSpace(m_iWenhaoMenuItem->getPosition())
			, LFStrings::getValue("gonggongge_shuoming"), NULL, false);
		//((GuideLayer_SimpleLayer*)guideLayer1)->setCallBack(this,SEL_CallFunc(&JiuGongGeLayer::GuiDeLayerCallBack));
		this->addChild(guideLayer1, 128);
		GameGuideManager::getManager()->setCurGuideState(4);
	}
}
void JiuGongGeLayer::onExit()
{
	CCLayerColor::onExit();
}

bool JiuGongGeLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	
	return true;
}

void JiuGongGeLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}

void JiuGongGeLayer::menuItemClicked_GridCircle(CCObject* pSender)
{
	if(mStart)
		return;

	disableMenuClicked();

	CCMenuItemImage *item = (CCMenuItemImage*)pSender;
	int tag = item->getTag();
	mGrid = tag - (TAG_Maps1+1);

	//宝箱
	TaskChessBoard data;
	ChessBoardStateManager::getManager()->getChessBoard(TaskManager::getManager()->getCurrentFightTaskId(), data);
/*	if (mGrid == data.box_id)
	{
		CCMenu *menu = CCMenu::create();
		addChild(menu, 100, Tag_Box);
		menu->setPosition(ccp(0,0));
		menu->setEnabled(false);
		//
		CCMenuItemImage *boxItem = CCMenuItemImage::create(
			"zhandou_jiugongge_baoxiang_1.png",
			"zhandou_jiugongge_baoxiang_1.png",
			this, menu_selector(JiuGongGeLayer::menuItemClicked_BoxWindowClicked));
		menu->addChild(boxItem, 0, Tag_Box);
		boxItem->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
		boxItem->setScale(0);
		boxItem->runAction(CCSequence::create(
			CCScaleTo::create(0.5f, 1.15f),
			CCScaleTo::create(0.15f, 1.0f),
			CCCallFuncN::create(this, callfuncN_selector(JiuGongGeLayer::openChessBoardBox)),
			NULL));
	}
	else */
	{
		CC_SAFE_DELETE(mFight);
		mFight = new FightPrepare(this);
		mFight->prepareData(TaskManager::getManager()->getCurrentFightTaskId(), mGrid);
	}
}

void JiuGongGeLayer::openChessBoardBox(CCNode *node)
{/*
	//
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this
		, callfuncO_selector(JiuGongGeLayer::rsp_getChessBoardRandomBox), MSG_getChessRandomBoxRsp, NULL);
	CmdHelper::getHelper()->cmdGetChessRandomBox(g_taskId, mGrid);
	*/
}

CCMenuItemImage* JiuGongGeLayer::getBtnItem(int index)
{
	CCMenuItemImage* pItem = NULL;
	CCMenu* pMenu = (CCMenu*)this->getChildByTag(TAG_Menu);
	if (pMenu)
	{
		pItem = (CCMenuItemImage*)pMenu->getChildByTag(Tag_BossBtn+index);
	}
	return pItem;
}

void JiuGongGeLayer::setBtnState(int index, BtnState state)
{
	CCMenu* pMenu = (CCMenu*)this->getChildByTag(TAG_Menu);
	if (pMenu)
	{
		CCMenuItemImage* pItem = (CCMenuItemImage*)pMenu->getChildByTag(Tag_BossBtn+index);
		if (pItem)
		{
			if (state == Normal_State)
			{
				pItem->setEnabled(true);
			}
			else
			{
				if (m_sEyeFireSpr)
				{
					m_sEyeFireSpr->removeFromParent();
				}
				pItem->setEnabled(false);
				this->showUnLockSpr(mTaskGridInfoList.at(index), true);
			}
		}
	}
}

void JiuGongGeLayer::rsp_getChessBoardRandomBox(CCObject *msg)
{/*
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_getChessRandomBoxRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_GetChessRandomBoxRsp *data = (SPCmd_GetChessRandomBoxRsp *) ((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		enableMenuClicked();
		this->setBtnState(mTouchedIndex, Normal_State);
		return;
	}

	CCMenuItemImage* pItem = this->getBtnItem(mTouchedIndex);
	if (pItem)
	{
		CCSprite* pSpr = CCSprite::create("jiugongge_shikuai_yitansuo_baoxiang.png");
		pSpr->setPosition(pItem->getPosition());
	}
	
	//
//	vector<GiftData> vec;
	vector<ItemInfo> items;
	for (int i=0; i!=data->count; ++i)
	{
		ItemInfo itemTemp;
		itemTemp.type = data->item_arr[i].type;
		itemTemp.count = data->item_arr[i].count;
		items.push_back(itemTemp);
	}
	//display award
	//该对话框的确定按钮被点击后 会回调 awardDisplayCallback_func
	AwardDisplay *displayDialog = AwardDisplay::create(LFStrings::getValue("SuiJiBaoXiangTiShi").c_str(), items, vector<Flat_DuanzaoGroupZBInfo>(), this);
	CCDirector::sharedDirector()->getRunningScene()->addChild(displayDialog);

	removeChildByTag(Tag_Box);*/
}

void JiuGongGeLayer::awardDisplayCallback_func()
{
	CC_SAFE_DELETE(mFight);
	mFight = new FightPrepare(this);
	mFight->prepareData(TaskManager::getManager()->getCurrentFightTaskId(), mGrid);
}

void JiuGongGeLayer::menuItemClicked_BoxWindowClicked(CCObject *pSender)
{

}

void JiuGongGeLayer::menuItemClicked_MainMenu(CCObject* pSender)
{
	if(mStart)
		return;

	int tag = ((CCMenuItemImage*)pSender)->getTag();
	if(tag == Tag_restart)
	{
		FightPackage::getPakage()->clearPackage();
		GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	}
	else if(tag == Tag_beibao)
	{
		FightBagLayer *beibaoLayer = new FightBagLayer;
		vector<Bag_WuPinInfo> WupinVec;
		{
			vector<WuPin> wupins = FightPackage::getPakage()->getAllWupingExceptSilverAndGold();
			Bag_WuPinInfo temp;
			for (int i=0; i!=wupins.size(); ++i)
			{
				temp.type = wupins.at(i).getType();
				temp.count = wupins.at(i).getCount();
				WupinVec.push_back(temp);
			}
		}
		beibaoLayer->init(FightPackage::getPakage()->getAllEquipments(), WupinVec, this);
		this->addChild(beibaoLayer, 1000);
		beibaoLayer->release();
		beibaoLayer->setPosition(ccp(mWinSize.width/2-beibaoLayer->getBodySize().width/2, mWinSize.height/2-beibaoLayer->getBodySize().height/2));
	}
	else if (tag == Tag_MenuItem_LevUp)
	{
	}
}

// 失败，返回一个Code，就是协议定义的SimpleProtocolErrorCode
void JiuGongGeLayer::prepareError(int code)
{
	enableMenuClicked();

	if(code == TiliNotEnought)
	{
		GetTiLiLayer* layer = GetTiLiLayer::create(this);
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

	this->setBtnState(mTouchedIndex, Normal_State);

	return;
}

void JiuGongGeLayer::setTiliTTF()
{
	this->getTiliCallBackFunc();
}

void JiuGongGeLayer::getTiliCallBackFunc()
{
	CCProgressTimer* tiLiBar = (CCProgressTimer*)this->getChildByTag(Tag_TiLiTimer);
	if (tiLiBar)
	{
		BaseActorProperty prop = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty();
		tiLiBar->setPercentage(prop.vit_current/(float)Vitality_Total * 100.0f);
	}

	CCLabelTTF *tiliLabel = (CCLabelTTF*)getChildByTag(Tag_LabelTiLiNum);
	if (tiliLabel)
	{
		//num
		CCString temp;
		temp.initWithFormat("%d/%d", (int)MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current, Vitality_Total);
		tiliLabel->setString(temp.getCString());
	}

}

// 提供背景
CCSprite* JiuGongGeLayer::getBg(int taskId,int gridIndex)
{
	return MapsManager::getManager()->getBackGroundMapImage(taskId);
}

void JiuGongGeLayer::prepareSuccess()
{
	GameDirector::getDirector()->hideWaiting();

	// 改变全局grid的值
//	g_gridIndex = mGrid;
	TaskManager::getManager()->setCurrentFightGridIdx(mGrid);

	fadeBack(mGrid);

	this->setTiliTTF();
}


void JiuGongGeLayer::fadeBack(int grid)
{
	CCFadeOut *fadeOut = CCFadeOut::create(0.35f);
	CCDelayTime *delay = CCDelayTime::create(0.125);
	CCCallFunc * callfun = CCCallFunc::create(this, callfunc_selector(JiuGongGeLayer::timeToFight));
	CCSequence *action = CCSequence::create(fadeOut, delay, callfun, NULL);
	this->runAction(action);
	if(getChildByTag(TAG_Flag) != NULL)
		getChildByTag(TAG_Flag)->runAction(action);
	 
	//地图背景
	if(getChildByTag(TAG_Maps_BG+10) != NULL)
		getChildByTag(TAG_Maps_BG+10)->runAction((CCFadeTo::create(0.5f, 0)));
}

void JiuGongGeLayer::disableMenuClicked()
{
	mStart = true;
	mMainMenu->setEnabled(false);
	
}

void JiuGongGeLayer::enableMenuClicked()
{
	mStart = false;
	mMainMenu->setEnabled(true);
}

void JiuGongGeLayer::timeToFight()
{
	mFight->fight();
}

void JiuGongGeLayer::menuItemClicked_JiFenGetAward(CCObject *pSender)
{
	if (mStart)
	{
		return;
	}
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	//
	CCMenuItem *item = CCMenuItemImage::create(
		"a1.png", "a2.png", "a3.png", this, menu_selector(JiuGongGeLayer::menuItemClicked_GetBonus));
	CCString infoStr;
	vector<ItemInfo> bonusVec;
	if (tag == Tag_JiFenItem_Min)
	{
		bonusVec = ChessBoardStateManager::getManager()->getKilledMonsterBonusData().commonBonus;
		infoStr.initWithFormat(LFStrings::getValue("JGG_JiShaGuiWuJiangLi_Tishi").c_str(), LFStrings::getValue("xiaoGuai").c_str());
		item->setTag(Tag_JiFenItem_Min);
		if (CS::cashBonusNeedKilledMonsters(1) > ChessBoardStateManager::getManager()->getKilledPutongMonsterNum())
			item->setEnabled(false);

	}
	else if (tag == Tag_JiFenItem_Max)
	{
		bonusVec = ChessBoardStateManager::getManager()->getKilledMonsterBonusData().eliteBonus;
		infoStr.initWithFormat(LFStrings::getValue("JGG_JiShaGuiWuJiangLi_Tishi").c_str(), LFStrings::getValue("jingYingGuai").c_str());
		item->setTag(Tag_JiFenItem_Max);
		if (CS::cashBonusNeedKilledMonsters(2) > ChessBoardStateManager::getManager()->getKilledJingYinMonsterNum())
			item->setEnabled(false);
	}

	if (item)
	{
		GetAwardLayer *layer = GetAwardLayer::create(infoStr, bonusVec);
		layer->setMenuContainer(item);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	}
}

void JiuGongGeLayer::menuItemClicked_GetBonus(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();


	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_GetAward_Layer);

	if (tag == Tag_JiFenItem_Min)
	{
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this
			, callfuncO_selector(JiuGongGeLayer::rsp_GetBonus), MSG_getKilledMonsterBonusRsp, NULL);
		CmdHelper::getHelper()->cmdGetKilledMonsterBonus(Accumulate_Common_Monster);
	}
	else if (tag == Tag_JiFenItem_Max)
	{
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this
			, callfuncO_selector(JiuGongGeLayer::rsp_GetBonus), MSG_getKilledMonsterBonusRsp, NULL);
		CmdHelper::getHelper()->cmdGetKilledMonsterBonus(Accumulate_Elite_Monster);
	}
}

void JiuGongGeLayer::rsp_GetBonus(CCObject *msg)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_getKilledMonsterBonusRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_GetKilledMonsterBonusRsp *data = (SPCmd_GetKilledMonsterBonusRsp *) ((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	//刷新
	CCLabelTTF *minNeedLabel = (CCLabelTTF*)mJiFenBoard->getChildByTag(Tag_Label_MinNeedKilled);
	CCLabelTTF *minCurLabel = (CCLabelTTF*)mJiFenBoard->getChildByTag(Tag_Label_MinCurKilled);
	CCLabelTTF *maxNeedLabel = (CCLabelTTF*)mJiFenBoard->getChildByTag(Tag_Label_MaxNeedKilled);
	CCLabelTTF *maxCurLabel = (CCLabelTTF*)mJiFenBoard->getChildByTag(Tag_Label_MaxCurKilled);
	//
	char buf[32];
	unsigned int minNeedKilled = CS::cashBonusNeedKilledMonsters(1);
	unsigned int minCurKilled  = ChessBoardStateManager::getManager()->getKilledPutongMonsterNum();
	unsigned int maxNeedKilled = CS::cashBonusNeedKilledMonsters(2);
	unsigned int maxCurKilled  = ChessBoardStateManager::getManager()->getKilledJingYinMonsterNum();
	if (minNeedLabel && minCurLabel)
	{
		sprintf(buf, "%u", minCurKilled);
		minCurLabel->setString(buf);
		if (minNeedKilled > minCurKilled)
		{
			minNeedLabel->setColor(fonColor_CiYao);
			minCurLabel->setColor(fonColor_JingGao);
		}
		else
		{
			minCurLabel->setColor(fonColor_ChengGong);
		}
	}
	if (maxNeedLabel && maxCurLabel)
	{
		sprintf(buf, "%u", maxCurKilled);
		maxCurLabel->setString(buf);
		if (maxNeedKilled > maxCurKilled)
		{
			maxNeedLabel->setColor(fonColor_CiYao);
			maxCurLabel->setColor(fonColor_JingGao);
		}
		else
		{
			maxCurLabel->setColor(fonColor_ChengGong);
		}
	}

	//
	FXToast *layer = FXToast::create(LFStrings::getValue("LingQu") + LFStrings::getValue("chenggong"));
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
}
void JiuGongGeLayer::addBossEffects(CCMenuItem *bossItem)
{
	addBianKuang(bossItem);


	CCAnimation* pAnimation = CCAnimation::create();
	for (int i = 1; i <= 27; i++)
	{
		pAnimation->addSpriteFrameWithFileName(CCString::createWithFormat("jiugongge_texiao_boss%d.png", i)->getCString());
	}
	pAnimation->setDelayPerUnit(5.0f / 30.0f);    // 必须设置这个，要不就不会播放
	pAnimation->setRestoreOriginalFrame(true);
	pAnimation->setLoops(1);

	CCSprite* pSprite2 = CCSprite::create();
	bossItem->addChild(pSprite2, 1);
	pSprite2->setPosition(ccp(bossItem->getContentSize().width / 2 - 0.5, bossItem->getContentSize().height / 2 - 36.5));
	pSprite2->runAction(CCSequence::create(CCAnimate::create(pAnimation),CCCallFunc::create(this,SEL_CallFunc(&JiuGongGeLayer::addEyeFire)),NULL));

	//this->scheduleOnce(SEL_SCHEDULE(&JiuGongGeLayer::startState),2.7f);
}
void JiuGongGeLayer::addBianKuang(CCMenuItem *bossItem)
{
	CCSize win_size = CCDirector::sharedDirector()->getWinSize();
	CCSprite *spr = CCSprite::create("jiugongge_texiao_boss_hongyun_jiao.png");

	CCSprite *leftTopSpr = CCSprite::createWithTexture(spr->getTexture());
	//leftTopSpr->setAnchorPoint(ccp(0,1));
	leftTopSpr->setPosition(ccp(leftTopSpr->getContentSize().width / 2,win_size.height - leftTopSpr->getContentSize().height / 2));
	addChild(leftTopSpr);
	m_lBianKuangSprList.push_back(leftTopSpr);
	CCSprite *rightTopSpr = CCSprite::createWithTexture(spr->getTexture());
	//rightTopSpr->setAnchorPoint(ccp(1,1));
	rightTopSpr->setPosition(ccp(win_size.width - leftTopSpr->getContentSize().width / 2,win_size.height - leftTopSpr->getContentSize().height / 2));
	addChild(rightTopSpr);
	m_lBianKuangSprList.push_back(rightTopSpr);
	rightTopSpr->setRotation(90);

	CCSprite *leftBttomSpr = CCSprite::createWithTexture(spr->getTexture());
	//leftBttomSpr->setAnchorPoint(CCPointZero);
	leftBttomSpr->setPosition(ccp(leftTopSpr->getContentSize().width / 2,leftTopSpr->getContentSize().height / 2));
	addChild(leftBttomSpr);
	m_lBianKuangSprList.push_back(leftBttomSpr);
	leftBttomSpr->setRotation(-90);

	CCSprite *rightBttomSpr = CCSprite::createWithTexture(spr->getTexture());
	//rightBttomSpr->setAnchorPoint(ccp(1,0));
	rightBttomSpr->setPosition(ccp(win_size.width  - leftTopSpr->getContentSize().width / 2, leftTopSpr->getContentSize().height / 2));
	addChild(rightBttomSpr);
	m_lBianKuangSprList.push_back(rightBttomSpr);
	rightBttomSpr->setRotation(180);

	CCSpriteBatchNode *shuBatch = CCSpriteBatchNode::create("jiugongge_texiao_boss_hongyun_shu.png");  
	shuBatch->setPosition(CCPointZero);
	addChild(shuBatch);
	int width = win_size.width - spr->getContentSize().width;
	int start_x = spr->getContentSize().width;

	for (int i = start_x ; i < width ; i ++)
	{
		CCSprite *sprite = CCSprite::createWithTexture(shuBatch->getTexture());  
		sprite->setPosition(ccp(i + sprite->getContentSize().width / 2,win_size.height - sprite->getContentSize().height / 2));  
		shuBatch->addChild(sprite, 1);

		CCSprite *bttomSprite = CCSprite::createWithTexture(shuBatch->getTexture());  
		bttomSprite->setPosition(ccp(i + bttomSprite->getContentSize().width / 2,bttomSprite->getContentSize().height / 2));  
		shuBatch->addChild(bttomSprite, 1);
		bttomSprite->setRotation(180);

		m_lBianKuangSprList.push_back(sprite);
		m_lBianKuangSprList.push_back(bttomSprite);
	}

	CCSpriteBatchNode *hengBatch = CCSpriteBatchNode::create("jiugongge_texiao_boss_hongyun_heng.png");  
	hengBatch->setPosition(CCPointZero);
	addChild(hengBatch);
	int height = win_size.height - spr->getContentSize().height;
	int start_y = spr->getContentSize().height;

	for (int i = start_y ; i < height ; i ++)
	{
		CCSprite *sprite = CCSprite::createWithTexture(hengBatch->getTexture());  
		sprite->setPosition(ccp(sprite->getContentSize().width / 2,i + sprite->getContentSize().height / 2));  
		hengBatch->addChild(sprite, 1);

		CCSprite *bttomSprite = CCSprite::createWithTexture(hengBatch->getTexture());  
		bttomSprite->setPosition(ccp(win_size.width - sprite->getContentSize().width / 2,i + sprite->getContentSize().height / 2));  
		hengBatch->addChild(bttomSprite, 1);
		bttomSprite->setRotation(180);
		m_lBianKuangSprList.push_back(sprite);
		m_lBianKuangSprList.push_back(bttomSprite);
	}

	for (vector<CCSprite *>::iterator iter = m_lBianKuangSprList.begin(); iter != m_lBianKuangSprList.end(); iter ++)
	{
		m_nActionCount ++;
		(*iter)->runAction(CCSequence::create(CCFadeOut::create(0),CCFadeIn::create(0.9f),CCDelayTime::create(1.0f),CCFadeOut::create(0.9f),CCCallFuncN::create(this,SEL_CallFuncN(&JiuGongGeLayer::removeSelf)),NULL));
	}

}
void JiuGongGeLayer::startState(float dt)
{
	this->setBtnState(0, Normal_State);
}
void JiuGongGeLayer::addEyeFire()
{
	this->setBtnState(0, Normal_State);
	//火焰效果  
	/*{
		CCParticleFire * particleSystem = CCParticleFire ::create();  
		particleSystem->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire-test.png"));  
		particleSystem->setPosition(ccp(102,95));
		m_iBossMenuItem->addChild(particleSystem,999);

		particleSystem->setLife(0.5f);
		particleSystem->setLifeVar(0.5f);
		particleSystem->setPosVar(ccp(20,0));
		particleSystem->setScaleX( 10.0f / CCTextureCache::sharedTextureCache()->addImage("fire-test.png")->getContentSize().width );
		particleSystem->setScaleY( 10.0f / CCTextureCache::sharedTextureCache()->addImage("fire-test.png")->getContentSize().height );

		colorAdd(particleSystem,m_iBossMenuItem);
	}
	{
		CCParticleFire * particleSystem = CCParticleFire ::create();  
		particleSystem->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire-test.png"));  
		particleSystem->setPosition(ccp(78,95));
		m_iBossMenuItem->addChild(particleSystem,999);

		particleSystem->setLife(0.5f);
		particleSystem->setLifeVar(0.5f);
		particleSystem->setPosVar(ccp(20,0));
		particleSystem->setScaleX( 10.0f / CCTextureCache::sharedTextureCache()->addImage("fire-test.png")->getContentSize().width );
		particleSystem->setScaleY( 10.0f / CCTextureCache::sharedTextureCache()->addImage("fire-test.png")->getContentSize().height );
		colorAdd(particleSystem,m_iBossMenuItem);
	}*/

	CCAnimation* pAnimation = CCAnimation::create();
	for (int i = 1; i <= 3; i++)
	{
		pAnimation->addSpriteFrameWithFileName(CCString::createWithFormat("jiugongge_texiao_boss_huoyan%d.png", i)->getCString());
	}
	pAnimation->setDelayPerUnit(5.0f / 30.0f);    // 必须设置这个，要不就不会播放
	pAnimation->setRestoreOriginalFrame(true);
	pAnimation->setLoops(-1);

	CCSprite* pSprite2 = CCSprite::create();
	m_iBossMenuItem->addChild(pSprite2, 999);
	pSprite2->setPosition(ccp(m_iBossMenuItem->getContentSize().width / 2 - 0.5, m_iBossMenuItem->getContentSize().height / 2 + 0.5));
	pSprite2->runAction(CCAnimate::create(pAnimation));
	m_sEyeFireSpr = pSprite2;

	//新玩法
	//1. 结算后进入九宫格就提示
	//2. 如果有boss效果 则boss效果完后提示（此处）
	displayNewZone();
}
void JiuGongGeLayer::colorAdd(CCParticleFire *particleSpr,CCNode *node)
{
	CCRenderTexture* rt = CCRenderTexture::create(node->getContentSize().width, node->getContentSize().height);//创建纹理渲染范围</pre>
	ccBlendFunc blendFunc;

	blendFunc.src = GL_DST_ALPHA; //表示使用源颜色的alpha值来作为因子
	blendFunc.dst = GL_DST_ALPHA; //不使用目标颜色

	particleSpr->setBlendFunc(blendFunc);
	rt->begin();
	particleSpr->visit();//注意，这里需要先绘制B，被遮罩图片
	node->visit();//注意渲染顺序，先渲染的为的精灵的颜色为目标颜色，后渲染的精灵的颜色为源颜色。
	rt->end();
}
void JiuGongGeLayer::GuiDeLayerCallBack()
{
	bool isFirstIn = false;
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstMaoXian))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstMaoXian);
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstMaoXian);
		if (m_iBossMenuItem)
		{
			CCPoint pos = m_iBossMenuItem->getParent()->convertToWorldSpace(ccp(m_iBossMenuItem->getPositionX(),m_iBossMenuItem->getPositionY()));
			pos.y -= (m_iBossMenuItem->getContentSize().height + 25.0f);
			GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;					

			guideLayer2->init(m_rJiugonggeMenuRect.size,pos
				, LFStrings::getValue("jiugongge_TongguanSuoming"), NULL, false);

			GuideLayerBase *guideLayer1 = new GuideLayer_SimpleLayer;
			guideLayer1->init(m_iBossMenuItem->getContentSize(), m_iBossMenuItem->getParent()->convertToWorldSpace(m_iBossMenuItem->getPosition())
				, LFStrings::getValue("jiugongge_Bossshuoming"), guideLayer2, true);
			this->addChild(guideLayer1, 128);
		}
	}
}

void JiuGongGeLayer::schedule_displayNewZone(float dt)
{
	displayNewZone();
}
void JiuGongGeLayer::displayNewZone()
{
	///////////////升级触发的 新玩法提示   
	if (g_ChuanQiFuBen_Opend) //传奇副本
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_ChuanQiFuBenKaiQi"), true,false,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	}
	else if (g_ChuanQiLu_Opend)
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_ChuanQiLuKaiQi"), true,false,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	}
	//else if (g_YingXiongBang_Opend)  //英雄榜
	//{
		
	//}
	else if (g_JingJiChang_Opend) // 竞技场
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_JingJiChangKaiQi"), true,false,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
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