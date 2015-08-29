#include "FightSettlementLayer.h"
#include "font.h"
#include "GameDirector.h"
#include "JiuGongGeLayer.h"
#include "WuPinManager.h"
#include "MapsLayer.h"
#include "EquipmentManager.h"
#include "ImageManager.h"
#include "EquipmentXiangQingBaseBody.h"
#include "MyselfManager.h"
#include "MapsManager.h"
#include "JGG_FootprintManager.h"
#include "Music.h"
#include "FightPackage.h"
#include "FightPackage.h"
#include "History.h"
#include "GuideLayerBase.h"
#include "SuitPool.h"
#include "ShenBingPool.h"
#include "MessageDisplay.h"
#include "LevelUpLayer.h"
#include "ClientFightLayer.h"
#include "GongJiangBase.h"
#include "ZhuangbeiScanner.h"
#include "ChatContentManager.h"
#include "UnblockManager.h"
#include "CS_Common.h"
#include "FXToast.h"
#include "AddFriendWarnLayer.h"
#include "NetTextListShowUI.h"
#include "NetLabelTTF.h"
#include "EquipHelper.h"
#include "BackgroudLayer.h"
#include "GameScene.h"
#include "EquipFallManger.h"
#include "ZhuangBeiMianBanBase.h"
#include "AddFriendUI.h"
#include "ChessBoardStateManager.h"
#define Label_Size 24

const int deltaHeight  =  75;
const float levUpInterval = 1.5;

enum 
{
	Tag_MenuItem_ZaiCiChuangGuan,
	Tag_MenuItem_TuiChuZhanDou,
	Tag_MenuItem_GuanBi,
	Tag_MenuItem_Qianwang,
	Tag_MenuItem_FightAgain,
	Tag_MenuItem_QuQiangHua,
	Tag_MenuItem_BeiBao,


	TAG_BlackBack =10, //-1
	TAG_Back	  = 20,     //0
	TAG_Title     = 21,   //1
	Tag_HeroMenu,

	TAG_TaskName	= 30,
	TAG_BossName,
	TAG_TaskWin	,
	TAG_EXP		,
	TAG_Silver	,


	Tag_JingYan   = 50,
	Tag_LevelNum  = 100,
	TAG_ARMY_1    = 200,
};

FightSettlementLayer* FightSettlementLayer::create(Settlement* taskInfo)
{
	FightSettlementLayer* layer = new FightSettlementLayer;
	if(layer && layer->init(taskInfo))
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


bool FightSettlementLayer::init(Settlement* task_Info)
{
	mTouched = false;
	mTaskInfo = new Settlement(task_Info);//析构函数删除
	m_bIsFinish = false;
	m_bLevelUp = false;
	mMenu = NULL;

	//获得当前任务有多少个格子
	unsigned int numInCol, numInRow;
	CS::getTaskChessboardFormat(TaskManager::getManager()->getCurrentFightTaskId(), numInCol, numInRow);
	mCurTaskGridNum = numInCol * numInRow;

	do 
	{
		// init layer;
		mWinSize = CCDirector::sharedDirector()->getWinSize();

		// 半透明背景
		CCSprite* btm = BaseSprite::create("zhandou_jiugongge_touming.png");
		addChild(btm, -1, TAG_BlackBack);
		btm->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
		btm->setScaleX(mWinSize.width / btm->getContentSize().width);
		btm->setScaleY(mWinSize.height / btm->getContentSize().height);
		btm->setOpacity(0);
		btm->runAction(CCFadeTo::create(0.5,220));
		//0. background
		mBg = BaseSprite::create("zhandou_jiesuan_bg.png");
		addChild(mBg,0,TAG_Back);
//		mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2-20));
		mBg->setPosition(ccp(mWinSize.width/2, mWinSize.height - 260 - 144));
		mBg->setOpacity(0);
		mBg->runAction(CCFadeTo::create(0.5, 255));

		//1. 成功失败！！？
		std::string music, tishiStr = LFStrings::getValue("zhanDou");
		CCSprite *imageBiaoTi;

		if(mTaskInfo->win == 1)
		{//成功
			imageBiaoTi = BaseSprite::create("zhandou_shengli.png");
			music = "zhandou_jiesuan_shengli.wav";
			tishiStr += LFStrings::getValue("shengli");
		}else
		{//失败
			imageBiaoTi = BaseSprite::create("zhandou_shibai.png");
			music = "zhandou_jiesuan_shibai.wav";
			tishiStr += LFStrings::getValue("shibai");
		}

		//2. boss否
//		if (g_taskId < HellTaskId_Min && g_gridIndex != 0)
		if (TaskManager::getManager()->getCurrentFightTaskId() < HellTaskId_Min && TaskManager::getManager()->getCurrentFightGridIdx() != 0)
		{
			CCLabelTTF *label = CCLabelTTF::create(tishiStr.c_str(), fontStr_kaiti, 24);
			mBg->addChild(label);
			label->setPosition(ccp(mBg->getContentSize().width/2, 535+75));
			label->setOpacity(0);
			label->runAction(CCFadeTo::create(0.5, 255));

			scheduleOnce(schedule_selector(FightSettlementLayer::displayInfomation_Ordinary), 0.05f);
		}
/////////////////////////////////////////  index == boss
		else 
		{
			//boos关卡，失败发消息，成功则直接显示
			if (mTaskInfo->win != 1)
			{
				addObserver(callfuncO_selector(FightSettlementLayer::QueryEquipFall_rsp), MSG_QueryEquipFallRsp);
				CmdHelper::getHelper()->cmdQueryEquipFall();
				return true;
			}

			imageBiaoTi->setPosition(ccp(mWinSize.width/2, mWinSize.height-260));
			imageBiaoTi->setOpacity(100);
			addChild(imageBiaoTi, 1, TAG_Title);

			//闪烁效果
			imageBiaoTi->setScale(5);
			CCScaleTo* scale = CCScaleTo::create(0.25f, 0.8f, 0.8f);
			CCFadeTo * fade  = CCFadeTo::create(0.25f, 255);
			CCDelayTime *dt = CCDelayTime::create(0.25f);
			CCScaleTo *scaleBack = CCScaleTo::create(0.15f, 1, 1);

			CCCallFunc *callfun = CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::displayInfomation_Boss));
			imageBiaoTi->runAction(CCSequence::create(CCSpawn::create(scale, fade, NULL), callfun, scaleBack, NULL));
	
		}

		Music::sharedMusicPlayer()->playEffectSound(music);

	} while (0);

	this->setTag(TAG_FIGHT_END_LAYER);

	return true;
}

void FightSettlementLayer::QueryEquipFall_rsp(CCObject* pMsg)
{
	removeObserver(MSG_QueryEquipFallRsp);
	GameDirector::getDirector()->hideWaiting();
	Flat_QueryEquipFallRsp *data = (Flat_QueryEquipFallRsp*)((Flat_MSG_Rsp*)pMsg)->getData();
	if (data->err_code != Success)
	{
		this->showBoosFailedUI();
		return;
	}

	int curTaskId = TaskManager::getManager()->getCurrentFightTaskId();
	//发送消息
	addObserver(callfuncO_selector(FightSettlementLayer::QueryFubenDiaoluo_rsp), MSG_QueryFubenDiaoluoRsp_F);
	for (int i = curTaskId-1; i > 0 && curTaskId <= i+5; i--)
	{
		mQueryTimes++;
	}
	mCurQueryTiems = mQueryTimes;
	for (int i = 0; i < mQueryTimes ; i++)
	{
		CmdHelper::getHelper()->cmdQueryFubenDiaoluo(curTaskId-1-i);
	}
}

void FightSettlementLayer::QueryFubenDiaoluo_rsp(CCObject* pMsg)
{
	mCurQueryTiems--;
	if (mCurQueryTiems <= 0)
	{
		removeObserver(MSG_QueryFubenDiaoluoRsp_F);
		GameDirector::getDirector()->hideWaiting();
	}
	Flat_QueryFubenDiaoluoRsp* rsp = (Flat_QueryFubenDiaoluoRsp*)((Flat_MSG_Rsp*)pMsg)->getData();
	if (rsp->err_code != Success)
	{
		CCLog("Error:%s ---> call function error, errorcode is %d", __FUNCTION__, rsp->err_code);
//		return;
	}

	if (mCurQueryTiems <= 0)
	{
		this->showBoosFailedUI();
	}

}

void FightSettlementLayer::showBoosFailedUI()
{
	//1. 成功失败！！？
	std::string music, tishiStr = LFStrings::getValue("zhanDou");
	CCSprite *imageBiaoTi;

	imageBiaoTi = BaseSprite::create("zhandou_shibai.png");
	music = "zhandou_jiesuan_shibai.wav";
	tishiStr += LFStrings::getValue("shibai");

	imageBiaoTi->setPosition(ccp(mWinSize.width/2, mWinSize.height-260));
	imageBiaoTi->setOpacity(100);
	addChild(imageBiaoTi, 1, TAG_Title);

	//闪烁效果
	imageBiaoTi->setScale(5);
	CCScaleTo* scale = CCScaleTo::create(0.25f, 0.8f, 0.8f);
	CCFadeTo * fade  = CCFadeTo::create(0.25f, 255);
	CCDelayTime *dt = CCDelayTime::create(0.25f);
	CCScaleTo *scaleBack = CCScaleTo::create(0.15f, 1, 1);

	CCCallFunc *callfun = CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::displayInfomation_Boss));
	imageBiaoTi->runAction(CCSequence::create(CCSpawn::create(scale, fade, NULL), callfun, scaleBack, NULL));

	Music::sharedMusicPlayer()->playEffectSound(music);


	this->setTag(TAG_FIGHT_END_LAYER);
}

void FightSettlementLayer::displayInfomation_Ordinary(float dt)
{
	/////////////////////////////////
	//非boss关卡
	////////////////////////////////

//	if (g_taskId < HellTaskId_Min && g_gridIndex != 0)
	if (TaskManager::getManager()->getCurrentFightTaskId() < HellTaskId_Min && TaskManager::getManager()->getCurrentFightGridIdx() != 0)
	{
		//4. 经验
		for(int i=0; i!=4; ++i)
		{
			if(mTaskInfo->curArmy[i].hero_id)// == MyselfManager::getManager()->getMyZhuJueData()->getId())
			{

				mTaskInfo->exp = mTaskInfo->curArmy[i].exp_current - mTaskInfo->oldArmy[i].exp_current 
					+ (mTaskInfo->curArmy[i].level-mTaskInfo->oldArmy[i].level)*mTaskInfo->oldArmy[i].exp_need;
			}
		}
		//5. 银币
//		int silver = 0;
// 		for(int i=0; i!=mTaskInfo->items.size(); ++i)
// 			if(mTaskInfo->items[i].type == ItemsType_Silver)
// 				silver = mTaskInfo->items[i].count;
		//非boss关只有经验
		if(mTaskInfo->exp > 0)
		{
			CCLabelTTF *exp = CCLabelTTF::create(LFStrings::getValue("jingyan").c_str(), fontStr_kaiti, 24);
			mBg->addChild(exp);
			exp->setPosition(ccp(mBg->getContentSize().width/2 - exp->getContentSize().width/2, 535+25));
			exp->setOpacity(0);
			exp->runAction(CCFadeTo::create(0.75, 255));

			char expcount[20];
			sprintf(expcount,"%u", mTaskInfo->exp);
			CCLabelTTF *expCount = CCLabelTTF::create(expcount, fontStr_BookAntiqua, 24);
			mBg->addChild(expCount,1,TAG_EXP);
			expCount->setAnchorPoint(ccp(0, 0.5));
			expCount->setPosition(ccp(exp->getPositionX()+exp->getContentSize().width/2+10,exp->getPositionY()));
			expCount->setOpacity(0);
			expCount->runAction(CCFadeTo::create(0.75f, 255));
		}
		else if (mTaskInfo->win == 1)
		{
			CCLabelTTF *exp = CCLabelTTF::create(LFStrings::getValue("JingYan_Max").c_str(), fontStr_kaiti, 24);
			mBg->addChild(exp);
			exp->setPosition(ccp(mBg->getContentSize().width/2, 535+25));
			exp->setOpacity(0);
			exp->runAction(CCFadeTo::create(0.75, 255));
		}
// 		if (silver > 0)
// 		{
// 			CCLabelTTF *yinbi = CCLabelTTF::create(LFStrings::getValue("yinbi").c_str(),fontStr_kaiti, 24);
// 			mBg->addChild(yinbi);
// 			yinbi->setPosition(ccp(mBg->getContentSize().width/3*2, 535+25));
// 			if(mTaskInfo->exp == 0)
// 			{
// 				yinbi->setPositionX(mWinSize.width/2 - 5 - yinbi->getContentSize().width/2);
// 			}
// 			yinbi->setOpacity(0);
// 			yinbi->runAction(CCFadeTo::create(0.75f, 255));
// 
// 			char count[20];
// 			sprintf(count,"%d", silver);
// 			CCLabelTTF *yinbiCount = CCLabelTTF::create(count, fontStr_kaiti, 24);
// 			mBg->addChild(yinbiCount,1,TAG_Silver);
// 			yinbiCount->setAnchorPoint(ccp(0,0.5));
// 			yinbiCount->setPosition(ccp(yinbi->getPositionX()+yinbi->getContentSize().width/2+10,yinbi->getPositionY()));
// 			yinbiCount->setOpacity(0);
// 			yinbiCount->runAction(CCFadeTo::create(0.75, 255));
// 		}

		//6. 添加英雄头像 （如果升级 。。。）
		addArmy();


		mTouched = true;
	}
	else
	{
		CCLog("Error:%s ---> call function error, should call displayInfomation_Boss", __FUNCTION__);
	}
}

//添加4经验， 5银币  6.升级
void FightSettlementLayer::displayInfomation_Boss()
{
	int curFightTaskId = TaskManager::getManager()->getCurrentFightTaskId();
	////////////////////
	//boss关卡
	////////////////////
	//是失败   则不显示之后的东西
	if(mTaskInfo->win != 1)
	{
// 		if(mTaskInfo->win == -1)
// 		{//步数太长失败 提示
// 			CCLabelTTF* tishi = CCLabelTTF::create(LFStrings::getValue("StepType_FailByTakeTooMuchTime").c_str()
// 				,fontStr_kaiti , Label_Size, CCSize(380,0), kCCTextAlignmentLeft);
// 			addChild(tishi, 1);
// 			tishi->setColor(ccc3(0xab, 0xab, 0xab));
// 			tishi->setPosition(ccp(mWinSize.width/2, mWinSize.height/2-20));
// 		}
		mBg->stopAllActions();
		mBg->setOpacity(0);

		mMenu = CCMenu::create();
		mBg->addChild(mMenu);
		mMenu->setPosition(CCPointZero);

		//再次闯关
		CCMenuItemImage *pFightAgain = CCMenuItemImage::create(
			"zhandou_jiesuan_anniu_zaicitiaozhan.png"
			, "zhandou_jiesuan_anniu_zaicitiaozhan_select.png"
			, this, menu_selector(FightSettlementLayer::menuItemClicked_MainMenu));
		mMenu->addChild(pFightAgain, 1, Tag_MenuItem_FightAgain);
		pFightAgain->setPosition(ccp(mWinSize.width/2-pFightAgain->getContentSize().width+20, 3*pFightAgain->getContentSize().height));

		//结束战斗
		CCMenuItemImage *guanbi = CCMenuItemImage::create(
			"zhandou_jiesuan_anniu_jieshu.png"
			, "zhandou_jiesuan_anniu_jieshu_select.png"
			, this, menu_selector(FightSettlementLayer::menuItemClicked_MainMenu));
		mMenu->addChild(guanbi, 1, Tag_MenuItem_GuanBi);
		guanbi->setPosition(ccp(mWinSize.width/2+guanbi->getContentSize().width-20, pFightAgain->getPositionY()));

		//前往刷新
		CCMenuItemImage *pQianwang = CCMenuItemImage::create(
			"xinguanqia_qianwang.png"
			, "xinguanqia_qianwang_select.png"
			, this, menu_selector(FightSettlementLayer::menuItemClicked_MainMenu));
		pQianwang->setPosition(ccp(mWinSize.width/2, pFightAgain->getPositionY()+3*pQianwang->getContentSize().height/2 - 20));

		//去强化
		CCMenuItemImage *qhItem = CCMenuItemImage::create(
			"zhandou_jiesuan_anniu_quqianghua.png"
			, "zhandou_jiesuan_anniu_quqianghua_select.png"
			, this, menu_selector(FightSettlementLayer::menuItemClicked_MainMenu));
		qhItem->setPosition(ccp(mWinSize.width/2, pQianwang->getPositionY()));
		//mTouched = true;

// 		//战斗结果
// 		CCLabelTTF* pTopTip = CCLabelTTF::create(LFStrings::getValue("zhandoujieguo").c_str(), fontStr_kaiti, Label_Size+6);
// 		pTopTip->setHorizontalAlignment(kCCTextAlignmentCenter);
// 		pTopTip->setColor(ccGRAY);
// 		mBg->addChild(pTopTip, 1);
// 		pTopTip->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2 + 70));
// 		
		BaseSprite* pFengexian = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
		mBg->addChild(pFengexian);
		//战斗结果
		CCLabelTTF* pTopTip = CCLabelTTF::create(LFStrings::getValue("zhandoujieguo").c_str(), fontStr_kaiti, Label_Size+6);
		pTopTip->setHorizontalAlignment(kCCTextAlignmentCenter);
		pTopTip->setColor(ccGRAY);
		mBg->addChild(pTopTip, 1);
		pTopTip->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2 + 70));

		pFengexian->setPosition(ccp(mBg->getContentSize().width/2, pTopTip->getPositionY()+pTopTip->getContentSize().height/2 + pFengexian->getContentSize().height/2));

		BaseSprite* pFengexian1 = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
		mBg->addChild(pFengexian1);
		pFengexian1->setPosition(ccp(mBg->getContentSize().width/2,  pTopTip->getPositionY()-pTopTip->getContentSize().height/2 - pFengexian->getContentSize().height/2));


		bool bShowQianwang = false;

		float fZhandouli = 0;
		float tempZhandouli = 0;
		int index = -1;
		vector<BaseActor*> baseactorlist = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
		for (int i = 0; i < baseactorlist.size(); ++i)
		{
			tempZhandouli = baseactorlist.at(i)->getMyZhanDouLi(Range_Average);
			if (i == 0)
			{
				fZhandouli = tempZhandouli;
				index = i;
			}
			else
			{
				if (tempZhandouli < fZhandouli)
				{
					fZhandouli = tempZhandouli;
					index = i;
				}
			}
		}
		pair<unsigned int, Flat_FubenDiaoluoZBInfo> info;
		if (index >= 0)
		{
			int heroId = baseactorlist.at(index)->getCurrentProperty().hero_id;
			info = EquipHelper::getHelper()->getOneBetterEquip(heroId, TaskManager::getManager()->getCurrentFightTaskId());
			if (info.first != 0)
			{
				mCur_ZBinfo = info.second;
				bShowQianwang = true;
			}
		}

		if (bShowQianwang)
		{
			mMenu->addChild(pQianwang, 1, Tag_MenuItem_Qianwang);

			int iDiaoLuoLevelIndex = EquipFallManager::getManager()->getAlbumEquipFallTaskId(mCur_ZBinfo.group_id);
			int iLevelIndex = iDiaoLuoLevelIndex-(iDiaoLuoLevelIndex/10)*10;
			if (iLevelIndex == 0)
			{
				iLevelIndex = 10;
			}
			std::string fullLevelName = CCString::createWithFormat("%d.", iLevelIndex)->getCString()
				+ ChatContentManager::sharedChatContentManager()->getMainTaskNameById(iDiaoLuoLevelIndex);

			NetTextListShowUI* pText = NetTextListShowUI::create();
			std::string fullStr = pText->getConversionMsgStr(LFStrings::getValue("ZhanDouShiBai_BOOS_Level_Tip1"), ccWHITE)
				+pText->getConversionMsgStr(fullLevelName, fonColor_FaGuang)
				+pText->getConversionMsgStr(LFStrings::getValue("ZhanDouShiBai_BOOS_Level_Tip2"), ccWHITE);
			pText->setTextWidth(370);
			pText->setTextSize(20);
			pText->addMessage(fullStr);

			NetLabelTTF* pLable = NetLabelTTF::create(pText->getMsgString().c_str(), fontStr_kaiti, 20);
			pLable->setMsgColorTexCoords(pText->getMsgColorTexCoordsList());
			pLable->setHorizontalAlignment(kCCTextAlignmentLeft);
			pLable->setHeight(pText->getHeight());

			mBg->addChild(pLable, 1);
			pLable->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2-40));

			
			//装备底纹
			BaseSprite* pDiwen = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_WithBianKuang((ZhuangbeiColour)mCur_ZBinfo.zhuangbei_colour));
			mBg->addChild(pDiwen);
			pDiwen->setScale(0.8f);
			pDiwen->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2-pDiwen->getContentSize().height/2*pDiwen->getScaleY() - 90));
			

			//装备图片
			string equipfile = mCur_ZBinfo.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *pZhuangbeiSpr = BaseSprite::create(equipfile);
			pDiwen->addChild(pZhuangbeiSpr);
			pZhuangbeiSpr->setScale(0.25f);
			pZhuangbeiSpr->setPosition(ccp(pDiwen->getContentSize().width/2, pDiwen->getContentSize().height/2));

			//装备名字
			CCLabelTTF* pName = CCLabelTTF::create(mCur_ZBinfo.zhuangbei_name.c_str(), fontStr_kaiti, 20);
			pName->setHorizontalAlignment(kCCTextAlignmentLeft);
			pDiwen->addChild(pName);
			pName->setPosition(ccp(pDiwen->getContentSize().width/2*pDiwen->getScaleX()+10, -pName->getContentSize().height));
			pName->setColor(ImageManager::getManager()->getShowColor((ZhuangbeiColour)mCur_ZBinfo.zhuangbei_colour, false, false));

			CCLabelTTF* pThird = CCLabelTTF::create(LFStrings::getValue("ZhanDouShiBai_BOOS_Level_Tip3").c_str(), fontStr_kaiti, 24);
			pThird->setAnchorPoint(ccp(0.0f, 0.5f));
			pThird->setHorizontalAlignment(kCCTextAlignmentLeft);
			pDiwen->addChild(pThird, 1);
			pThird->setColor(fonColor_FaGuang);
			pThird->setPosition(ccp(- pThird->getContentSize().width - 20, pDiwen->getContentSize().height/2));

			BaseSprite* pFengexian = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
			mBg->addChild(pFengexian);
			pFengexian->setPosition(ccp(mBg->getContentSize().width/2,
				pDiwen->getPositionY()-pDiwen->getContentSize().height/2 - pFengexian->getContentSize().height/2 - 40));
		}
		else
		{
			mMenu->addChild(qhItem, 1, Tag_MenuItem_QuQiangHua);
			CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("ZhanDouShiBai_Tip").c_str(), fontStr_kaiti, Label_Size);//, CCSizeMake(370, 0), kCCTextAlignmentCenter);
			mBg->addChild(tishiLabel, 1);
			tishiLabel->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2 - 70));
			BaseSprite* pFengexian = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
			addChild(pFengexian);
			pFengexian->setPosition(ccp(mBg->getContentSize().width/2, 
				tishiLabel->getPositionY()-tishiLabel->getContentSize().height/2 - pFengexian->getContentSize().height/2 - 70));

		}
		return;
	}
	else   //win
	{
		//-2
		CCSprite* fightMap = MapsManager::getManager()->getBackGroundMapImage(
			curFightTaskId>=HellTaskId_Min ? (curFightTaskId-HellTaskId_Min+1) : (curFightTaskId));
		addChild(fightMap, -2);
		fightMap->setAnchorPoint(ccp(0.5, 1));
		fightMap->setPosition(ccp(mWinSize.width/2, mWinSize.height));
		fightMap->setOpacity(0);
		fightMap->runAction(CCFadeTo::create(0.25, 255));

		//1. 副本名  //127  1152-543
//		string nameStr = ChatContentManager::sharedChatContentManager()->getMainTaskNameById(
// 		curFightTaskId>=HellTaskId_Min ? (curFightTaskId-HellTaskId_Min+1) : (curFightTaskId));
		string nameStr = LFStrings::getValue("zhandoujieguo");
		CCLabelTTF *taskName = CCLabelTTF::create(nameStr.c_str(), fontStr_kaiti, Label_Size);
		mBg->addChild(taskName,1,TAG_TaskName);
		taskName->setOpacity(0);
		taskName->runAction(CCFadeTo::create(0.5f, 255));
		taskName->setAnchorPoint(ccp(0.5,0.5));
		taskName->setPosition(ccp(mBg->getContentSize().width/2, 535+75));
		taskName->setColor(fonColor_PuTong);

		scheduleOnce(schedule_selector(FightSettlementLayer::displayBossInfoDelaySchedule), 0.15f);
	}

}

void FightSettlementLayer::displayBossInfoDelaySchedule(float)
{

	float time = 0.75;
	//4. 经验
	for(int i=0; i!=4; ++i)
	{
		if(mTaskInfo->curArmy[i].hero_id)// == MyselfManager::getManager()->getMyZhuJueData()->getId())
		{

			mTaskInfo->exp = mTaskInfo->curArmy[i].exp_current - mTaskInfo->oldArmy[i].exp_current 
				+ (mTaskInfo->curArmy[i].level-mTaskInfo->oldArmy[i].level)*mTaskInfo->oldArmy[i].exp_need;
		}
	}
	//5. 银币
	int silver = 0;
	for(int i=0; i!=mTaskInfo->items.size(); ++i)
		if(mTaskInfo->items[i].type == ItemsType_Silver)
			silver = mTaskInfo->items[i].count;

	if(mTaskInfo->exp > 0)
	{
		if (isNeedExp())
		{
			CCLabelTTF *exp = CCLabelTTF::create(LFStrings::getValue("jingyan").c_str(), fontStr_kaiti, 24);
			mBg->addChild(exp);
			exp->setPosition(ccp(mBg->getContentSize().width/4, 535+25));
			if (silver <= 0)
			{
				exp->setPosition(ccp(mBg->getContentSize().width/2-exp->getContentSize().width/2, 535+25));
			}
			char expcount[20];
			sprintf(expcount,"%u", mTaskInfo->exp);
			CCLabelTTF *expCount = CCLabelTTF::create(expcount, fontStr_kaiti, Label_Size);
			mBg->addChild(expCount,1,TAG_EXP);
			expCount->setAnchorPoint(ccp(0, 0.5f));
			expCount->setPosition(ccp(exp->getPositionX()+exp->getContentSize().width/2+10,exp->getPositionY()));//224

			exp->setOpacity(0);
			exp->runAction(CCFadeTo::create(time, 255));
			expCount->setOpacity(0);
			expCount->runAction(CCFadeTo::create(time, 255));
			exp->runAction(CCSequence::create(
				CCFadeTo::create(0.15f, 100),
				// 			CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::addArmy)),
				// 			CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::addMenu)),
				CCFadeTo::create(0.5f, 255), NULL));
		}
		else
		{
			CCLabelTTF *exp = CCLabelTTF::create(LFStrings::getValue("JingYan_Max").c_str(), fontStr_kaiti, 24);
			mBg->addChild(exp);
			exp->setPosition(ccp(mBg->getContentSize().width/2- exp->getContentSize().width/2- 30, 535+25));
			exp->setOpacity(0);
			exp->runAction(CCSequence::create(
				CCFadeTo::create(0.15f, 100), 
				//			CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::addArmy)),
				//			CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::addMenu)),
				CCFadeTo::create(0.5f, 255), NULL));
		}
	}
	else
	{
		CCLabelTTF *exp = CCLabelTTF::create(LFStrings::getValue("NoNeed").c_str(), fontStr_kaiti, 24);
		mBg->addChild(exp);
		exp->setPosition(ccp(mBg->getContentSize().width/2- exp->getContentSize().width/2- 30, 535+25));
		exp->setOpacity(0);
		exp->runAction(CCSequence::create(
			CCFadeTo::create(0.15f, 100), 
			//			CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::addArmy)),
			//			CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::addMenu)),
			CCFadeTo::create(0.5f, 255), NULL));
	}
	{
		CCLabelTTF *yinbi = CCLabelTTF::create(LFStrings::getValue("yinbi").c_str(),fontStr_kaiti, 24);
		mBg->addChild(yinbi);
		yinbi->setPosition(ccp(mBg->getContentSize().width/3*2+yinbi->getContentSize().width/2, 535+25));
		
		if (mTaskInfo->exp <= 0)
		{
			yinbi->setPosition(ccp(mBg->getContentSize().width/2-yinbi->getContentSize().width/2, 535+25));
		}

		char count[20];
		sprintf(count,"%d", silver);
		CCLabelTTF *yinbiCount = CCLabelTTF::create(count, fontStr_kaiti, Label_Size);
		mBg->addChild(yinbiCount,1,TAG_Silver);
		yinbiCount->setAnchorPoint(ccp(0,0.5));
		yinbiCount->setPosition(ccp(yinbi->getPositionX()+yinbi->getContentSize().width/2+10,yinbi->getPositionY()));//462

		yinbi->setOpacity(0);
		yinbi->runAction(CCFadeTo::create(time, 255));
		yinbiCount->setOpacity(0);
		yinbiCount->runAction(CCSequence::create(
			CCFadeTo::create(0.15f, 100), 
			CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::addArmy)),
			CCCallFunc::create(this, callfunc_selector(FightSettlementLayer::addMenu)),
			CCFadeTo::create(0.5f, 255), NULL));

		if (silver <= 0)
		{
			yinbi->setVisible(false);
			yinbiCount->setVisible(false);
		}
	}
}


//添加 6队伍
void FightSettlementLayer::addArmy()
{
	float heroScale = 0.6f;


	CCMenu *heroMenu = CCMenu::create();
	mBg->addChild(heroMenu, 2, Tag_HeroMenu);
	heroMenu->setPosition(ccp(0,0));

	vector<BaseActor*> actVec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
	int actNum = actVec.size();
	for (int i=0; i<actNum; ++i)
	{
		BaseActor* actor = actVec.at(i);

		BaseSprite* sprite = BaseSprite::create(ImageManager::getManager()->getImageData_ProfessionHead(
			actor->getActorType()).getCurrentImageFile());
		sprite->setScale(heroScale);
		mBg->addChild(sprite, 1, TAG_ARMY_1 + actor->getId());
		if (actNum % 2 == 0)
			sprite->setPosition(ccp(mBg->getContentSize().width/2+52 + (i-actNum/2)*104, 535-60));
		else 
			sprite->setPosition(ccp(mBg->getContentSize().width/2 + (i-actNum/2)*104, 535-60));

		BaseActorProperty prop = actor->getCurrentProperty();
		//lv   lvNum
		char levBuf[10] = "";
		sprintf(levBuf, "%u", prop.level);
		CCLabelTTF* lv = CCLabelTTF::create("Lv", fontStr_kaiti, 20);
		CCLabelTTF* lev = CCLabelTTF::create(levBuf, fontStr_kaiti, 20);
		lv->setAnchorPoint(ccp(0, 1));
		lev->setAnchorPoint(ccp(0, 1));
		mBg->addChild(lv, 1);
		mBg->addChild(lev, 1, Tag_LevelNum + i);
		lev->setUserData(actor);
		lv->setColor(ccc3(0xff,0x8c,0x00));
		lv->setPosition(ccp(sprite->getPositionX()-sprite->getContentSize().width/2*heroScale, 
			sprite->getPositionY()-sprite->getContentSize().height/2*heroScale - 4));		
		lev->setPosition(ccp(lv->getPositionX()+lv->getContentSize().width, lv->getPositionY()));

		//exp bar
		CCSprite* expBar = BaseSprite::create("zhuye_jinyan_bg.png");
		mBg->addChild(expBar, 1);
		expBar->setScale(0.5f);
		expBar->setPosition(ccp(sprite->getPositionX(), sprite->getPositionY()-62));
		CCProgressTimer* curExp = LF::createProgressTimer(BaseSprite::create("zhuye_jinyan.png"));
		mBg->addChild(curExp, 1, Tag_JingYan + i);
		curExp->setUserData(actor);
		curExp->setPosition(expBar->getPosition());
		curExp->setScale(0.5f);
		curExp->setPercentage(prop.exp_current / (float)prop.exp_need * 100.0f);

		if (prop.level >= 30)
		{
			curExp->setVisible(false);
			expBar->setVisible(false);
		}


		//action
		float time = 1.0;
		sprite->setOpacity(0);
		lv->setOpacity(0);
		lev->setOpacity(0);
		expBar->setOpacity(0);
		curExp->setOpacity(0);
		sprite->runAction(CCFadeTo::create(time, 255));
		lv->runAction(CCFadeTo::create(time, 255));
		lev->runAction(CCFadeTo::create(time, 255));
		expBar->runAction(CCFadeTo::create(time, 255));
		curExp->runAction(CCFadeTo::create(time, 255));


		CCMenuItemImage *biankuang = CCMenuItemImage::create(
			"renxing_renwu_touxiang_select.png",
			"renxing_renwu_touxiang_select.png",
			this, menu_selector(FightSettlementLayer::menuItemClicked_HeroImg));
		heroMenu->addChild(biankuang, 0, actor->getId());
		biankuang->setPosition(sprite->getPosition());
		biankuang->setOpacity(0);
		biankuang->setScale(heroScale);
		biankuang->runAction(CCFadeTo::create(time, 255));
	}




	//6. 如果升级了
	armyLevelUp();

}

void FightSettlementLayer::addMenu()
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	mBeibaoWupins = FightPackage::getPakage()->getAllEquipments();
	mWupins = FightPackage::getPakage()->getAllWupingExceptSilverAndGold();

	//去除铁锤
	for (int i = 0; i < mWupins.size(); i++)
	{
		if (mWupins.at(i).getType() == ItemsType_Hammer)
		{
			mWupins.erase(mWupins.begin()+i);
			break;
		}
	}

// 	WuPin wupininfo(ItemsType_Purify_Scroll, 2);
// 	mWupins.push_back(wupininfo);
//  mWupins.push_back(wupininfo);
// 	mWupins.push_back(wupininfo);
	int iToalcount = mBeibaoWupins.size()+mWupins.size();
	CCTableView* pTableView = NULL;
	CCSize size = CCSizeMake(150, 150);
	if (mBeibaoWupins.size() > 0 || mWupins.size() > 0)
	{

		//道具拾取
		CCLabelTTF* pDojuShiqu = CCLabelTTF::create(LFStrings::getValue("shiqudaoju").c_str(), fontStr_kaiti, Label_Size);
		pDojuShiqu->setColor(fonColor_PuTong);
		mBg->addChild(pDojuShiqu);
		pDojuShiqu->setPosition(ccp(mBg->getContentSize().width/2, 535-60-110));

		BaseSprite* pFengexian = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
		mBg->addChild(pFengexian);
		pFengexian->setPosition(ccp(mBg->getContentSize().width/2, pDojuShiqu->getPositionY()-pDojuShiqu->getContentSize().height/2-pFengexian->getContentSize().height/2));

		pTableView = CCTableView::create(this, CCSizeMake(size.width*iToalcount, size.height));
		pTableView->setDirection(kCCScrollViewDirectionHorizontal);
		pTableView->setPosition(ccp(mBg->getContentSize().width/2 - size.width*2.0f, pFengexian->getPositionY()-size.height));
		if (iToalcount == 1)
		{
			pTableView->setPosition(ccp(mBg->getContentSize().width/2-size.width*0.5f, pFengexian->getPositionY()-size.height));
		}
		else if (iToalcount == 2)
		{
			pTableView->setPosition(ccp(mBg->getContentSize().width/2 - size.width, pFengexian->getPositionY()-size.height));
		}
		else if (iToalcount == 3)
		{
			pTableView->setPosition(ccp(mBg->getContentSize().width/2 - size.width*1.5F, pFengexian->getPositionY()-size.height));
		}

		if (iToalcount > 4)
		{
			mBg->addChild(pTableView);
			pTableView->setViewSize(CCSizeMake(size.width*4, size.height));
			pTableView->setTouchPriority(kCCMenuHandlerPriority-1000);
		}
		else
		{
			mBg->addChild(pTableView);
			pTableView->setViewSize(CCSizeMake(size.width*iToalcount, size.height));
			pTableView->setTouchEnabled(false);
		}
		pTableView->reloadData();

		BaseSprite* pFengexian1 = BaseSprite::create("FB_tuandui_zhandou_xinxi_fenge.png");
		mBg->addChild(pFengexian1);
		pFengexian1->setPosition(ccp(mBg->getContentSize().width/2, pTableView->getPositionY()-pFengexian1->getContentSize().height/2));
	}

	mMenu = CCMenu::create();
	mBg->addChild(mMenu);
	mMenu->setPosition(CCPointZero);

	//
	CCMenuItemImage *tuichu = CCMenuItemImage::create(
		"zhandou_jiesuan_anniu_jieshu.png"
		, "zhandou_jiesuan_anniu_jieshu_select.png"
		, this, menu_selector(FightSettlementLayer::menuItemClicked_MainMenu));
	mMenu->addChild(tuichu, 1, Tag_MenuItem_TuiChuZhanDou);
	if (iToalcount > 0)
	{
		tuichu->setPosition(ccp(mBg->getContentSize().width/3*2, pTableView->getPositionY()-tuichu->getContentSize().height - 20));
	}
	else
	{
		tuichu->setPosition(ccp(mBg->getContentSize().width/3*2, 1152-886));
	}

	CCMenuItemImage *again = CCMenuItemImage::create(
		"zhandou_jiesuan_anniu_zaici.png"
		, "zhandou_jiesuan_anniu_zaici_select.png"
		, this, menu_selector(FightSettlementLayer::menuItemClicked_MainMenu));
	mMenu->addChild(again, 1, Tag_MenuItem_ZaiCiChuangGuan);
	again->setPosition(ccp(mBg->getContentSize().width/3, tuichu->getPositionY()));

	//屏蔽背包显示相关功能

	//beibao
	CCMenuItemImage *bag = CCMenuItemImage::create(
		"zhandou_beibao.png"
		, "zhandou_beibao_select.png"
		, this, menu_selector(FightSettlementLayer::menuItemClicked_MainMenu));
	mMenu->addChild(bag, 1, Tag_MenuItem_BeiBao);
	bag->setVisible(false);
//	bag->setAnchorPoint(ccp(1, 0.5));
	bag->setPosition(ccp(mBg->getContentSize().width-bag->getContentSize().width/2, bag->getContentSize().height/2));
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
							, NULL, this, menu_selector(FightSettlementLayer::menuItemClicked_HuanZhuang));
//						bag->setAnchorPoint(ccp(1, 0.5));
						bag->setPosition(ccp(mBg->getContentSize().width-bag->getContentSize().width/2, bag->getContentSize().height/2+50));

						//箭头
						BaseSprite *tishiImg = BaseSprite::create("zhuye_gongjitisheng.png");
						bag->addChild(tishiImg);
						tishiImg->setPosition(ccp(bag->getContentSize().width/2, bag->getContentSize().height+tishiImg->getContentSize().height/2));
						tishiImg->runAction(CCRepeatForever::create(
							CCSequence::create(CCMoveBy::create(0.5f, ccp(0, 20)), CCMoveBy::create(0.5f, ccp(0, -20)), NULL)));

						//
						bMarked = true;

						bag->setVisible(true);

						break;
					}
 				}
 			}
		}
	}

////////////////////////////////////////////////////////////////////////////////////////////////
	//教学
	teachGuide();

}


CCTableViewCell* FightSettlementLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();
	if (cell) 
	{
		cell->removeFromParent();
	}

	cell = new CCTableViewCell();
	cell->autorelease();

	if (idx < mBeibaoWupins.size())
	{
		//底纹
		//装备底纹
		BaseSprite* pDiwen = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_WithBianKuang((ZhuangbeiColour)mBeibaoWupins.at(idx).getEquipData().zhuangbei_colour));
		cell->addChild(pDiwen);
		pDiwen->setAnchorPoint(ccp(0.0f,0));
		pDiwen->setScale(0.8f);
		pDiwen->setPosition(ccp(35, 30));

		//装备图片
		string equipfile = mBeibaoWupins.at(idx).getEquipData().equipPic;
		equipfile = "minute_" + equipfile + ".png";
		BaseSprite *pZhuangbeiSpr = BaseSprite::create(equipfile);
		pDiwen->addChild(pZhuangbeiSpr);
		pZhuangbeiSpr->setScale(0.25f);
		pZhuangbeiSpr->setPosition(ccp(pDiwen->getContentSize().width/2, pDiwen->getContentSize().height/2));

		//装备名字
		string fullname = mBeibaoWupins.at(idx).getEquipData().equipName;

		CCLabelTTF* pName = CCLabelTTF::create(fullname.c_str(), fontStr_kaiti, 20);
		pName->setHorizontalAlignment(kCCTextAlignmentLeft);
		pDiwen->addChild(pName, 2);
		pName->setPosition(ccp(pDiwen->getContentSize().width/2*pDiwen->getScaleX(), -pName->getContentSize().height));
		pName->setColor(ImageManager::getManager()->getShowColor((ZhuangbeiColour)mBeibaoWupins.at(idx).getEquipData().zhuangbei_colour, false, false));

		CCLabelTTF* pName1 = CCLabelTTF::create(CCString::createWithFormat(" *%d",1)->getCString(), fontStr_kaiti, 20);
		pName1->setHorizontalAlignment(kCCTextAlignmentLeft);
		pName1->setAnchorPoint(ccp(0.0f, 0.5f));
		pDiwen->addChild(pName1, 2);
		pName1->setPosition(ccp(pName->getPositionX()+pName->getContentSize().width/2, pName->getPositionY()));
	}
	else
	{
		int index = idx-mBeibaoWupins.size();
		//道具底纹
		BaseSprite* pDiwen = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mWupins.at(index).getType()));
		cell->addChild(pDiwen);
		pDiwen->setAnchorPoint(ccp(0,0));
		pDiwen->setScale(0.8f);
		pDiwen->setPosition(ccp(15, 30));

		//道具图片
		BaseSprite *pZhuangbeiSpr = BaseSprite::create(ImageManager::getManager()->getItemFilename(mWupins.at(index).getType()));
		pDiwen->addChild(pZhuangbeiSpr);
		pZhuangbeiSpr->setScale(0.8f);
		pZhuangbeiSpr->setPosition(ccp(pDiwen->getContentSize().width/2, pDiwen->getContentSize().height/2));

		//道具名字
		string fullname = CS::getItemName(mWupins.at(index).getType());
		CCLabelTTF* pName = CCLabelTTF::create(fullname.c_str(), fontStr_kaiti, 20);
		pName->setHorizontalAlignment(kCCTextAlignmentLeft);
		pDiwen->addChild(pName, 2);
		pName->setPosition(ccp(pDiwen->getContentSize().width/2*pDiwen->getScaleX(), -pName->getContentSize().height));
		pName->setColor(FX_CommonFunc::getCaiLiaoColor(mWupins.at(index).getType()));

		CCLabelTTF* pName1 = CCLabelTTF::create(CCString::createWithFormat(" *%d",mWupins.at(index).getCount())->getCString(), fontStr_kaiti, 20);
		pName1->setHorizontalAlignment(kCCTextAlignmentLeft);
		pName1->setAnchorPoint(ccp(0.0f, 0.5f));
		pDiwen->addChild(pName1, 2);
		pName1->setPosition(ccp(pName->getPositionX()+pName->getContentSize().width/2, pName->getPositionY()));
	}
	

	return cell;
}

CCSize FightSettlementLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSizeMake(150, 150);
}

unsigned int FightSettlementLayer::numberOfCellsInTableView(CCTableView *table)
{
	return  mWupins.size()+mBeibaoWupins.size();
}


string FightSettlementLayer::getNewFirendName(unsigned int id)
{
	vector<BaseActor *> list = MyselfManager::getManager()->getTeamActors_ZhenShen();
	for (vector<BaseActor *>::iterator iter = list.begin(); iter != list.end(); iter ++)
	{
		if ((*iter)->getCurrentProperty().profession == id)
		{
			return (*iter)->getCurrentProperty().nameInChinese;
		}
	}
	return "";
}
void FightSettlementLayer::teachGuide()
{

	CCMenuItemImage *tuichu = (CCMenuItemImage*)mMenu->getChildByTag(Tag_MenuItem_TuiChuZhanDou);
	


	//技能书教学
	{	/*
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstLearnSkillBook))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstLearnSkillBook);
			if (isUI_Guide_LearnSkill && curTask == 12)
			{
				GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
				guideLayer->init(tuichu->getContentSize(), tuichu->getParent()->convertToWorldSpace(tuichu->getPosition())
					, LFStrings::getValue("learnSkill_zhanDouJieSuan"), NULL, false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1030);

				g_firstLearnSkillBook = true;
				GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstLearnSkillBook);
				return;
			}
		}*/
	}

	//新玩法 提示 ---> 由指定任务完成后激活
	//工匠教学   已触发
	if(TaskManager::getManager()->getCurrentFightTaskId() == 4)
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_QhHt))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_QhHt);
			g_gongjiang_QhHt_Opend = true;
		}
	}
	else if(TaskManager::getManager()->getCurrentFightTaskId() == 5)
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstXunBaoTeach))
		{
			//g_YingXiongBang_Opend = true;
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstXunBaoTeach);
		}
	}
	else if(TaskManager::getManager()->getCurrentFightTaskId() == 2)
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_NewFriendAddOne))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_NewFriendAddOne);
			
		}
	}
	else if (TaskManager::getManager()->getCurrentFightTaskId() == 3)
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_NewFriendAddTwo))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_NewFriendAddTwo);
			
		}
	}
	else if(TaskManager::getManager()->getCurrentFightTaskId() == 6)
	{
		if (m_bLevelUp)
		{
			return;
		}
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstYingXiongBang))
		{
			g_YingXiongBang_Opend = true;
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstYingXiongBang);
		}
	}
	else if(TaskManager::getManager()->getCurrentFightTaskId() == 10)
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstXueSeJiaoTang))
		{
			g_XueSeJiaoTang_Opend = true;
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstXueSeJiaoTang);
		}
	}
	if (!m_bLevelUp)
	{
		playNewFriendAdd();
	}
// 	else if (curTask == 5 && g_taskId == 5)
// 	{
// 		bool isFirstBsXq = false;
// 		if(History::getHistory()->getBoolFromCurrentHero(Key_FirstGongJiang_BsXq, isFirstBsXq))
// 		{
// 			if(isFirstBsXq)
// 				isFirstBsXq = true;
// 		}
// 		else
// 		{
// 			isFirstBsXq = true;
// 		}
// 		if(isFirstBsXq)
// 			g_gongjiang_BsXq_Opend = true;
// 	}
// 	else if(curTask == 7 && g_taskId == 7)
// 	{
// 		bool isFirstBsHc = false;
// 		if(History::getHistory()->getBoolFromCurrentHero(Key_FirstGongJiang_BsHc, isFirstBsHc))
// 		{
// 			if(isFirstBsHc)
// 				isFirstBsHc = true;
// 		}
// 		else
// 		{
// 			isFirstBsHc = true;
// 		}
// 		if(isFirstBsHc)
// 			g_gongjiang_BsHc_Opend = true;
// 	}


//	if (isUI_Guide_GongJiang && (g_gongjiang_QhHt_Opend || g_gongjiang_BsXq_Opend || g_gongjiang_BsHc_Opend))
	
}

void FightSettlementLayer::armyLevelUp()
{
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
	{
		m_bLevelUp = true;
		CCDirector::sharedDirector()->getRunningScene()->addChild((LevelUpLayer*)lvUpArray->objectAtIndex(0), 2000); //高于guidelayer
	}
	else
	{
		playNewFriendAdd();
	}

	lvUpArray->release(); 

}

void FightSettlementLayer::onEnter()
{
	BaseLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -127, true);
}

void FightSettlementLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	BaseLayer::onExit();
}

void FightSettlementLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();
	mFightAgain = false;

	if( tag == Tag_MenuItem_TuiChuZhanDou)
	{
		FightPackage::getPakage()->clearPackage();

		//暂时没有各种教学

		if(g_firstTaskEnd_to_changeEquip)  //主线教学
			GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
// 		else if(g_firstLearnSkillBook)     //技能教学
// 			GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
// 										   //工匠教学
// 		else if (g_gongjiang_QhHt_Opend || g_gongjiang_BsXq_Opend || g_gongjiang_BsHc_Opend || g_gongjiang_CqFm_Opend)
// 			GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		else 
			GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	}
	else if(tag == Tag_MenuItem_ZaiCiChuangGuan)
	{
		mFightAgain = true;

		JGG_FootprintManager::getManager()->clearJGGFootprint();
		FightPackage::getPakage()->clearPackage();

		if (TaskManager::getManager()->getCurrentFightTaskId() < HellTaskId_Min)
		{
			//从服务器  刷新  九宫格
			addObserver(SEL_CallFuncO(&FightSettlementLayer::callback_BeginFightNormal), MSG_GetTaskChessBoardRsp);
			mCmdHlp->cmdGetTaskChessboard(TaskManager::getManager()->getCurrentFightTaskId(), true);//
		}
		else if (TaskManager::getManager()->getCurrentFightTaskId() >= HellTaskId_Min)
		{
			addObserver(callfuncO_selector(FightSettlementLayer::callback_BeginFightHard), MSG_hellTaskBattleRsp_F);
			mCmdHlp->cmdHellTaskBattle(TaskManager::getManager()->getCurrentFightTaskId());
		}
	}
	else if(tag == Tag_MenuItem_BeiBao)
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
	else if (tag == Tag_MenuItem_FightAgain)
	{
		mTouched = true;
		ccTouchBegan(NULL, NULL);
	}
	else if (tag == Tag_MenuItem_GuanBi)
	{
		FightPackage::getPakage()->clearPackage();
		GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	}
	else if (tag == Tag_MenuItem_Qianwang)
	{
		MainLayerBase* footer = new MainLayerBase();
		BackgroudLayer* bgLayer = new BackgroudLayer();

		MapsLayer* mapslayer = new MapsLayer(GoTo_LevelTiaozhen_UI_Type);
		footer->setContaintLayer(mapslayer);
		//装备掉落的关卡
		int iDiaoLuoLevelIndex = EquipFallManager::getManager()->getAlbumEquipFallTaskId(mCur_ZBinfo.group_id);
		mapslayer->setWillGoToLevel(iDiaoLuoLevelIndex);

		GameScene* scene = GameScene::createScene();
		scene->addChild(footer,1);
		scene->addChild(bgLayer,-1);

		footer->release();
		bgLayer->release();
		mapslayer->release();

		CCDirector::sharedDirector()->replaceScene(scene);
	}
	else if (tag == Tag_MenuItem_QuQiangHua)
	{
// 		int heroId = MyselfManager::getManager()->getMyZhuJueData()->getId();
// 		GameDirector::getDirector()->mainSceneRenXingMianBan(heroId);
		vector<BaseActor*> baseactorlist = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);

		float fZhandouli = 0;
		float tempZhandouli = 0;
		int index = -1;
		for (int i = 0; i < baseactorlist.size(); ++i)
		{
			tempZhandouli = baseactorlist.at(i)->getMyZhanDouLi(Range_Average);
			if (i == 0)
			{
				fZhandouli = tempZhandouli;
				index = i;
			}
			else
			{
				if (tempZhandouli < fZhandouli)
				{
					fZhandouli = tempZhandouli;
					index = i;
				}
			}
		}

		if (index >= 0)
		{
			int heroId = baseactorlist.at(index)->getCurrentProperty().hero_id;
			//GameDirector::getDirector()->mainSceneRenXingMianBan(heroId);
			MainLayerBase* footer = new MainLayerBase();
			BackgroudLayer* bgLayer = new BackgroudLayer();

			BaseLayer* layer = ZhuangBeiMianBanBase::create(heroId, true);
			footer->setContaintLayer(layer);

			GameScene* scene = GameScene::createScene();
			scene->addChild(footer,1);
			scene->addChild(bgLayer,-1);

			footer->release();
			bgLayer->release();

			CCDirector::sharedDirector()->replaceScene(scene);
		}
	}
}

void FightSettlementLayer::callback_BeginFightNormal(CCObject* obj)
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

	//为什么加一个mFightAgain标记量 —— vip发来的grid[0] 不会是false
//	if( ! g_grid[0] && mFightAgain)
	if (mFightAgain)
	{
		//再次闯关
		TaskManager::getManager()->setCurrentFightGridIdx(-1);
	}
	else if(TaskManager::getManager()->getCurrentFightGridIdx()==0 && mTaskInfo->win==1 && !mFightAgain)
	{
// 		pair<TaskId, TaskStatus> info;
// 		info = MapsManager::getManager()->getCurMainTask();
// 		if(info.first==TaskManager::getManager()->getCurrentFightTaskId() && info.second<=TaskStatus_Got)
		if (TaskManager::getManager()->getLastActivedMainTaskId() == TaskManager::getManager()->getCurrentFightTaskId()
			&& TaskManager::getManager()->getLastActivedMainTaskStatus() <= TaskStatus_Got)
		{	
			GameDirector::getDirector()->mainScene(mainlayer_MaoXian);

			return;
		}
	}


	{
		CCScene *scene = CCScene::create();
		JiuGongGeLayer *jggLayer = JiuGongGeLayer::create();
		scene->addChild(jggLayer, 0, TAG_JiuGongGe_Layer);
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, scene));
	}

}

void FightSettlementLayer::callback_BeginFightHard(CCObject *msg_f)
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

bool FightSettlementLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	mFightAgain = false;

	if(mTouched && TaskManager::getManager()->getCurrentFightTaskId() < HellTaskId_Min) 
	{
// 		if(mTaskInfo->win == 1)
// 			JGG_FootprintManager::getManager()->setFootprint(g_taskId, g_gridIndex);

		//从服务器  刷新  九宫格
		addObserver(SEL_CallFuncO(&FightSettlementLayer::callback_BeginFightNormal), MSG_GetTaskChessBoardRsp);
		mCmdHlp->cmdGetTaskChessboard(TaskManager::getManager()->getCurrentFightTaskId(), false);
	}
	else if (mTouched && TaskManager::getManager()->getCurrentFightTaskId() >= HellTaskId_Min)
	{
		GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	}

	return true;
}

void FightSettlementLayer::menuItemClicked_HuanZhuang(CCObject *pSender)
{
	GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
}

void FightSettlementLayer::menuItemClicked_HeroImg(CCObject *pSender)
{
	return;

	int heroId = ((CCMenuItem*)pSender)->getTag();

	BaseActor *actor = MyselfManager::getManager()->getMyTeamActor(heroId);
	if (actor->getCurrentProperty().level >= 30) return;

	LevelUpLayer *layer = LevelUpLayer::create(heroId, this, true);
	if (layer) CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 2000);//高于guidelayer
}

void FightSettlementLayer::LevelUpEndCallBackFunc()
{
	vector<BaseActor*> vec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);

	for (int i=0; i<vec.size(); ++i)
	{
		//经验条
		CCProgressTimer* curExp = (CCProgressTimer*)mBg->getChildByTag(Tag_JingYan + i);
		if (curExp)
		{
			BaseActor *actor = (BaseActor*)curExp->getUserData();
			curExp->setPercentage(actor->getCurrentProperty().exp_current / (float)actor->getCurrentProperty().exp_need * 100.0f);
		}
		
		//等级
		CCLabelTTF *lev = (CCLabelTTF*)mBg->getChildByTag(Tag_LevelNum + i);
		if (lev)
		{
			BaseActor *actor = (BaseActor*)curExp->getUserData();
			char buf[20];
			sprintf(buf, "%d", actor->getLevel());
			lev->setString(buf);
		}
	}

	///////////////升级触发的 新玩法提示   
	//移到九宫格layer里面去

	{
		playNewFriendAdd();
	}
}

void FightSettlementLayer::disableAllTouch()
{
	//		mTouched = false;
	if (mMenu) mMenu->setVisible(false);

	CCMenu *heroMenu = (CCMenu*)mBg->getChildByTag(Tag_HeroMenu);
	if (heroMenu) heroMenu->setVisible(false);

	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

void FightSettlementLayer::enableAllTouch()
{
	//		mTouched = true;
	if (mMenu) mMenu->setVisible(true);

	CCMenu *heroMenu = (CCMenu*)mBg->getChildByTag(Tag_HeroMenu);
	if (heroMenu) heroMenu->setVisible(true);

	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -127, true);
}
void FightSettlementLayer::playNewFriendAdd()
{
	if (mMenu)
	{
		CCMenuItemImage *tuichu = (CCMenuItemImage*)mMenu->getChildByTag(Tag_MenuItem_TuiChuZhanDou);
		if (tuichu && isUI_Guide_GongJiang && g_gongjiang_QhHt_Opend)
		{
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(tuichu->getContentSize(), tuichu->getParent()->convertToWorldSpace(tuichu->getPosition())
				, "", NULL, false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1030);

			return;
		}
		CCMenuItemImage *huanZhuangItem = (CCMenuItemImage*)mMenu->getChildByTag(Tag_MenuItem_BeiBao);

		m_bIsFinish = true;
		//
		//	int curTask = MapsManager::getManager()->getCurMainTask().first;
		int curTask = TaskManager::getManager()->getLastActivedMainTaskId();
		TaskStatus state = TaskManager::getManager()->getLastActivedMainTaskStatus();
		int index = TaskManager::getManager()->getCurrentFightGridIdx();

		bool isBoss = ChessBoardStateManager::getManager()->isBossSection(curTask,index);
		//主线教学
		if (curTask == 1 && state == TaskStatus_Got && isBoss)
		{

			if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJinXingTongGuan) && GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstMaoXian))
			{
				//GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstJinXingTongGuan);
				GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstJinXingTongGuan);
				//bool isTeach = GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstJinXingTongGuan);
				//if (isTeach)
				if (huanZhuangItem)
				{
					GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
					guideLayer->init(huanZhuangItem->getContentSize(), huanZhuangItem->getParent()->convertToWorldSpace(huanZhuangItem->getPosition())
						, LFStrings::getValue("wanmeitongguan"), NULL, false);
					CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1030);

					g_firstTaskEnd_to_changeEquip = true;
				}
				else
				{
					GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstJinXingTongGuan);
				}

				//History::getHistory()->putInCurrentHero("JinXingTongGuanOver",true);
				return;
			}
		}
	}
	
	//TaskStatus state = MapsManager::getManager()->getCurMainTask().second;
	if(TaskManager::getManager()->getCurrentFightTaskId() == 2 && m_bIsFinish)
	{

		
	}
	if(TaskManager::getManager()->getCurrentFightTaskId() == 3 && m_bIsFinish)
	{
		
	}
}
bool FightSettlementLayer::isNeedExp()
{
	{
		vector<BaseActor*> vec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);

		for (vector<BaseActor*>::iterator iter = vec.begin() ; iter != vec.end() ; iter++)
		{
			if ((*iter)->getLevel() < HERO_MAX_LEVEL)
			{
				return true;
			}
		}
	}
	{
		vector<BaseActor*> vec = MyselfManager::getManager()->getTeamActors_YuanShen(Actor_ShangZheng);

		for (vector<BaseActor*>::iterator iter = vec.begin() ; iter != vec.end() ; iter++)
		{
			if ((*iter)->getLevel() < YUANSHEN_MAX_LEVEL)
			{
				return true;
			}
		}
	}

	return false;
}
void FightSettlementLayer::addFriendCallBack(CCNode *node)
{
	AddFriendWarnLayer *layer = AddFriendWarnLayer::create();
	layer->ignoreAnchorPointForPosition(false);
	layer->setAnchorPoint(CCPointZero);
	layer->setPosition(CCPointZero);
	layer->setFriendName(getNewFirendName(node->getTag()));
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);
}