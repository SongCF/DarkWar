
#include "JingJichangLayer.h"
#include "GameDirector.h"
#include "LFStrings.h"
#include "font.h"
#include "BackgroudLayer.h"
#include "GameScene.h"
#include "ImageManager.h"
#include "MyselfManager.h"
#include "WaitLayer.h"
#include "FightDirector.h"
#include "GameDirector.h"
#include "ClientFightLayer.h"
#include "ImageManager.h"
#include "LFAlert.h"
#include "openBoxLayer.h"
#include "MessageDisplay.h"
#include "GetTiLiLayer.h"
#include "MapsManager.h"
#include "TimeoutMonitor.h"
#include "FX_CommonFunc.h"

using namespace FX_CommonFunc;

#define TimeOut 30

#define POS(x) ccp(158+x%2*312, 452-x/2*230)
// 0 1
// 2 3

#define FONT_SIZE 24
#define NAME_LABEL_SIZE 20
#define TAG_TiShi_DIALOG 1024

#define mineFall_label_color ccc3(0x66, 0x99, 0x7B)
#define otherFall_label_color ccc3(127,127,127)
#define mine_label_color ccc3(0, 200, 0)

enum TAG{
	Tag_Board_Content
	, Tag_Hero_FullPhoto = 10

	, Tag_InfoWindow_Label = 20

	, Tag_HeroInfo_Label = 30

	//
	, Tag_Board_Name = 40

	, Tag_Label_Name = 50
	
	, Tag_Img_Vs_ = 60
	,TAG_TiShi_DIALOG_ok = 70
};


vector<HeroInfo> JingJiChangLayer::s_mHero;
Flat_ArenaGetBattleScriptGiftRsp JingJiChangLayer::s_mBattle;
vector<EquipData> JingJiChangLayer::s_mHeroEquipData[4];


JingJiChangLayer::JingJiChangLayer()
	:mBg(NULL)
// 	,mWaitActorTime(TimeOut)
// 	,mWaitFightTime(3)
	,mFightNum(-1)
{
	{
		//长时间死等监听
		addObserver(callfuncO_selector(JingJiChangLayer::error_TimeOut), MSG_WAIT_TIME_OUT);
	}
}

JingJiChangLayer::~JingJiChangLayer()
{
	CC_SAFE_RELEASE(mBg);
}

JingJiChangLayer* JingJiChangLayer::create(int flag)
{
	JingJiChangLayer * layer = new JingJiChangLayer;
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

bool JingJiChangLayer::init(int flag)
{
	if( !BaseLayer::init() )
		return false;

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	//footer bg
	mBg = new BaseSprite;
	mBg->initWithFile("jingjichang_ziyou_bg.jpg");
	addChild(mBg);
	mBg->setPosition(ccp(winSize.width/2, winSize.height/2));
	BaseSprite *contentBoard = BaseSprite::create("jingjichang_suiji_bg.jpg");
	mBg->addChild(contentBoard, 0, Tag_Board_Content);
	contentBoard->setPosition(ccp(mBg->getContentSize().width/2, 851-494));
	//
	BaseSprite *itemFree = BaseSprite::create("jingjichang_anniu_ziyoumoshi.png");
	mBg->addChild(itemFree);
	itemFree->setPosition(ccp(226, 851-61));
	BaseSprite *itemRandom = BaseSprite::create("jingjichang_anniu_suijimoshi_select.png");
	mBg->addChild(itemRandom);
	itemRandom->setPosition(ccp(426, 851-61));
// 	BaseSprite *itemBuy = BaseSprite::create("jingjichang_anniu_goumai.png");
// 	mBg->addChild(itemBuy);
// 	itemBuy->setPosition(ccp(219, 851-186));
	BaseSprite *itemExchange = BaseSprite::create("jingjichang_anniu_duihuan.png");
	mBg->addChild(itemExchange);
	itemExchange->setPosition(ccp(509, 851-186));
	//
	CCString temp;
	ArenaTeamBasicInfo infoD;
	MyselfManager::getManager()->getArenaTeamBasicInfo(infoD);
	BaseSprite *board1 = BaseSprite::create("jingjichang_biaoti_shengyucishu.png");
	mBg->addChild(board1);
	board1->setPosition(ccp(121, 851-171));
	temp.initWithFormat("%u", infoD.free_fight_remainder_time);
	CCLabelTTF *label1 = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, NAME_LABEL_SIZE);
	board1->addChild(label1, 1, 1);
	label1->setPosition(ccp(44, 18));
	BaseSprite *board2 = BaseSprite::create("jingjichang_biaoti_rongyudian.png");
	mBg->addChild(board2);
	board2->setPosition(ccp(406, 851-171));
	temp.initWithFormat("%u", infoD.jifen);
	CCLabelTTF *label2 = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, NAME_LABEL_SIZE);
	board2->addChild(label2, 1, 1);
	label2->setColor(ccGREEN);
	label2->setPosition(ccp(44, 18));
	//
// 	mInfoWindow =CCSprite::create("PVP_xinxi.png");
// 	mBg->addChild(mInfoWindow);
// 	mInfoWindowSize = mInfoWindow->getContentSize();
// 	mCurrentDeltaHeight = mInfoWindowSize.height;
// 	mInfoWindow->initWithFile("PVP_xinxi.png", CCRect(0,mCurrentDeltaHeight,mInfoWindowSize.width, mInfoWindowSize.height));
// 	schedule(schedule_selector(JingJiChangLayer::infoWindowDown), 0.01);
// 	mInfoWindow->setAnchorPoint(ccp(0.5,1));
// 	mInfoWindow->setPosition(ccp(320, 851-272+mInfoWindowSize.height/2));
	CCLabelTTF *infoLabel = CCLabelTTF::create("", fontStr_kaiti, FONT_SIZE);
	mBg->addChild(infoLabel, 1, Tag_InfoWindow_Label);
	infoLabel->setPosition(ccp(mBg->getContentSize().width/2, 851-806));
//	infoLabel->setColor(ccRED);
	mUpdataInfoNum = 0;
	schedule(schedule_selector(JingJiChangLayer::updateWindowInfomation), 0.75f);

	mFlag = flag;
	if(flag == 0)
	{	
		//
		scheduleOnce(schedule_selector(JingJiChangLayer::addWaitingLayer), 0);

		SPCmd_ArenaEnter enter;
		enter.hardLevel = 0;
		addObserver(callfuncO_selector(JingJiChangLayer::areaEnterRsp), MSG_arenaEnterRsp_F);
		addObserver(callfuncO_selector(JingJiChangLayer::areaUpdateRoomRsp), MSG_arenaUpdateRoomRsp_F);
		addObserver(callfuncO_selector(JingJiChangLayer::arenaGetBattleScriptAndGiftRsp), MSG_arenaGetBattleScriptAndGiftRsp_F);
		mCmdHlp->cmdArenaEnter(enter);

	}
	else if(flag == 1)
	{
		everyFightEndDraw(1);

		//赢了就去第二场，输了就去结算  4秒等待显示战果
		if(s_mBattle.scriptVec.at(0).endResult.myResult == Result_Win)
			scheduleOnce(schedule_selector(JingJiChangLayer::AgainstPlan2), 4);
		else
			scheduleOnce(schedule_selector(JingJiChangLayer::CombatSettlement), 4);
	}
	else if(flag == 2)
	{
		everyFightEndDraw(2);
		scheduleOnce(schedule_selector(JingJiChangLayer::CombatSettlement), 4);
	}
	else
	{
		CCLog("Error: JingJiChangLayer::init ---> flag Error [flag = %d]", flag);
		return false;
	}

	return true;
}

void JingJiChangLayer::resetInfoLabel(CCNode *node)
{
	CCLabelTTF *label = (CCLabelTTF *)mBg->getChildByTag(Tag_InfoWindow_Label);

	if(mFightNum == 1)  //第一场开始
	{
		label->setString((LFStrings::getValue("di1chang")+LFStrings::getValue("zhandoukaishi")).c_str());
		label->setOpacity(0);
		label->runAction(CCFadeIn::create(0.5f));
	}
	else if(mFightNum == 2)
	{
		label->setString((LFStrings::getValue("di2chang")+LFStrings::getValue("zhandoukaishi")).c_str());
		label->setOpacity(0);
		label->runAction(CCFadeIn::create(0.5f));
	}
}

void JingJiChangLayer::updateWindowInfomation(float dt)
{
	CCLabelTTF *label = (CCLabelTTF *)mBg->getChildByTag(Tag_InfoWindow_Label);
	if(mFlag == 0)
	{	
		string str = LFStrings::getValue("dengdaizhong");

		// 0 1 2 3 省略号
		for(int i=0; i<=mUpdataInfoNum; ++i)
			str += ".";
		label->setString(str.c_str());

		++mUpdataInfoNum;
		mUpdataInfoNum %= 3;
	}
	else if(mFlag == 1)
	{
		unschedule(schedule_selector(JingJiChangLayer::updateWindowInfomation));
		//第一场结束的战果
		label->setString((LFStrings::getValue("di1chang")+LFStrings::getValue("zhandoujieshu")).c_str());
		label->setOpacity(0);
		label->runAction(CCFadeIn::create(0.5f));
	}
	else if(mFlag == 2)
	{
		//第二场结束的战果
		unschedule(schedule_selector(JingJiChangLayer::updateWindowInfomation));
		//第一场结束的战果
		label->setString((LFStrings::getValue("di2chang")+LFStrings::getValue("zhandoujieshu")).c_str());
		label->setOpacity(0);
		label->runAction(CCFadeIn::create(0.25f));
	}else
	{
		CCLog("Error: JingJiChangLayer::updateWaiting ---> flag error![flag = %d]", mFlag);
	}
}

vector<ZhuangbeiSummary> JingJiChangLayer::getHeroEquipSumById(int heroId)
{
	vector<ZhuangbeiSummary> vec;
	for(int i=0; i!=s_mHero.size(); ++i)
	{
		if(s_mHeroEquipData[i].size() == 0 )
			continue;
		else
		{
			if(s_mHeroEquipData[i].at(0).zhuangbei_hero_id == heroId)
			{
				for (int j=0; j!=s_mHeroEquipData[i].size(); ++j)
				{
					ZhuangbeiSummary sum;
					sum.zhuangbei_colour = s_mHeroEquipData[i].at(j).zhuangbei_colour;
					strcpy(sum.zhuangbei_pic , s_mHeroEquipData[i].at(j).equipPic.c_str());
					sum.zhuangbei_type = s_mHeroEquipData[i].at(j).zhuangbei_type;
					vec.push_back(sum);
				}
				return vec;
			}
		}
	}

	return vec;
}

void JingJiChangLayer::drawHeroImgAndInfo(int i)
{
	CCNode *heroImg;
	if(s_mHero.at(i).hero_id == MyselfManager::getManager()->getMyZhuJueData()->getId())
	{
		heroImg = ImageManager::getManager()->getFullPhotoOfActor(MyselfManager::getManager()->getMyZhuJueData());
	}
	else
	{
		heroImg = ImageManager::getManager()->getFullPhotoOfActor((Profession)s_mHero.at(i).profession_id
			, getHeroEquipSumById(s_mHero.at(i).hero_id), false);
	}
	mBg->addChild(heroImg, 1, Tag_Hero_FullPhoto + i);
	heroImg->setPosition(POS(i));
	heroImg->setScale(0.9f);
	heroImg->setUserData((void*)i);

	//Mingzi kuang
	BaseSprite *mingziKuang = BaseSprite::create("jingjichang_id_shengli_bg.png");
	heroImg->addChild(mingziKuang, 100, Tag_Board_Name);
	mingziKuang->setPosition(ccp(heroImg->getContentSize().width/2, -30));
	//name
	CCLabelTTF *labelName = CCLabelTTF::create(s_mHero.at(i).name, fontStr_kaiti, NAME_LABEL_SIZE);
	mingziKuang->addChild(labelName, 1, Tag_Label_Name);
	labelName->setPosition(ccp(86, 21));

	if(s_mHero.at(i).hero_id == MyselfManager::getManager()->getMyZhuJueData()->getId())
		labelName->setColor(mine_label_color);

	if(mFlag != 0)
	{
		if(mFlag != 2 && i%2 == 1)  // 1 3画出vs
		{
			BaseSprite *vsImg = BaseSprite::create("jingjichang_id_vs_icon.png");
			mBg->addChild(vsImg, 100, Tag_Img_Vs_ + i);
			vsImg->setPosition(ccp(mBg->getContentSize().width/2, 430 - i/2*230));
		}
	}
}

void JingJiChangLayer::areaEnterRsp(CCObject* f_msg_rsp)
{
	removeObserver(MSG_arenaEnterRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_MSG_Rsp* rsp  =  (Flat_MSG_Rsp*)f_msg_rsp;

	Flat_ArenaEnterRsp* data =  (Flat_ArenaEnterRsp*)rsp->getData();
	if( data->err_code != Success)
	{
		if(data->err_code == TiliNotEnought)
		{
			GetTiLiLayer* layer = GetTiLiLayer::create();
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);

			CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_Waiting_Layer);

			mFooterLayer->dismmisPopUp();
		}
		return;
	}

	s_mHero = data->already_waited;

	HeroInfo mine;
	BaseActorProperty prof = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty();
	mine.hero_id = prof.hero_id;
	mine.level = prof.level;
	strcpy(mine.name, prof.customName.c_str());
	mine.profession_id = prof.profession;
	s_mHero.push_back(mine);

	for(int i=0; i<s_mHero.size(); ++i)
	{
		s_mHeroEquipData[i] = data->getEquipsByHeroId(s_mHero.at(i).hero_id);
		drawHeroImgAndInfo(i);	
	}

}

void JingJiChangLayer::areaUpdateRoomRsp(CCObject* msg_rsp)
{
	//update 每次只有一个
	Flat_ArenaUpdateRoomRsp* rsp = (Flat_ArenaUpdateRoomRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if(rsp->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		this->addChild(layer, 1025);
		return;
	}

	s_mHero.push_back(rsp->new_hero_info);
	s_mHeroEquipData[s_mHero.size()-1] = rsp->equipsOnHero;

	{
		drawHeroImgAndInfo(s_mHero.size()-1);
	}
}

void JingJiChangLayer::arenaGetBattleScriptAndGiftRsp(CCObject* f_msg_rsp)
{
	removeObserver(MSG_arenaUpdateRoomRsp_F);
	removeObserver(MSG_arenaGetBattleScriptAndGiftRsp_F);
	Flat_ArenaGetBattleScriptGiftRsp* data = (Flat_ArenaGetBattleScriptGiftRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		this->addChild(layer, 1025);
		return;
	}

	//保存下来   
	s_mBattle = *data;
	data = NULL;


	//
	for(int i=0; i!=4; ++i)
	{
		for (int j=0; j!=4; ++j)
		{
			if(s_mBattle.first_turn_hero_id_arr[i] == s_mHero.at(j).hero_id && i!= j)
			{
				//把 j 移到 位置 i
				CCNode *node = mBg->getChildByTag(Tag_Hero_FullPhoto + j);
				node->runAction(CCSequence::create(
					CCDelayTime::create(1)
					, CCMoveTo::create(0.35f, POS(i))
					, CCCallFuncN::create(this, callfuncN_selector(JingJiChangLayer::changeZOrder_callbackfun)), NULL));
			}
		}
	}
	// 数据交换
	for(int i=0; i!=4; ++i)
	{
		for (int j=i; j!=4; ++j)
		{
			if(s_mBattle.first_turn_hero_id_arr[i] == s_mHero.at(j).hero_id && i!= j)
			{
				//把 j 移到 位置 i
				HeroInfo tempInfo = s_mHero.at(i);
				s_mHero.at(i) = s_mHero.at(j);
				s_mHero.at(j) = tempInfo;
				break;
			}
		}
	}


	//vs  img
	{
		BaseSprite *vsImg1 = BaseSprite::create("jingjichang_id_vs_icon.png");
		mBg->addChild(vsImg1, 100, Tag_Img_Vs_ + 1);
		vsImg1->setPosition(ccp(mBg->getContentSize().width/2, 430));

		BaseSprite *vsImg2 = BaseSprite::create("jingjichang_id_vs_icon.png");
		mBg->addChild(vsImg2, 100, Tag_Img_Vs_ + 3);
		vsImg2->setPosition(ccp(mBg->getContentSize().width/2, 430-230));
	}


	unschedule(schedule_selector(JingJiChangLayer::updateWindowInfomation));
	scheduleOnce(schedule_selector(JingJiChangLayer::AgainstPlan1), 0.5);
}

void JingJiChangLayer::changeZOrder_callbackfun(CCNode *node)
{
	if (node->getPositionY() <= POS(2).y+10 && node->getPositionY() >= POS(2).y-10)
	{
		mBg->reorderChild(node, 10);
//		node->setZOrder(10);
	}
	else
	{
		mBg->reorderChild(node, 1);
//		node->setZOrder(1); 
	}
}

void JingJiChangLayer::waitingForFightWithDisplayInfo(float dt)
{
	//战斗前的激情效果
	Profession profMine = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;
	Profession profAnother;
	string nameMine, nameAnother;
	if(mFightNum==1)
	{
		//
		for (int i=0; i!=4; ++i)
		{
			int id = MyselfManager::getManager()->getMyZhuJueData()->getId();
			if(id == s_mBattle.first_turn_hero_id_arr[i])
			{
				//mine name
				for (int xx = 0; xx < 4; xx++)
				{
					if(id == s_mHero.at(xx).hero_id)
						nameMine = s_mHero.at(xx).name;
				}

				//another name     and     profession
				switch (i)
				{
				case 0:
					for (int j=0; j!=s_mHero.size(); ++j)
					{
						if(s_mBattle.first_turn_hero_id_arr[1] == s_mHero.at(j).hero_id)
						{
							profAnother = (Profession)s_mHero.at(j).profession_id;
							nameAnother = s_mHero.at(j).name;
						}
					}				
					break;
				case 1:
					for (int j=0; j!=s_mHero.size(); ++j)
					{
						if(s_mBattle.first_turn_hero_id_arr[0] == s_mHero.at(j).hero_id)
						{
							profAnother = (Profession)s_mHero.at(j).profession_id;
							nameAnother = s_mHero.at(j).name;
						}
					}				
					break;
				case 2:
					for (int j=0; j!=s_mHero.size(); ++j)
					{
						if(s_mBattle.first_turn_hero_id_arr[3] == s_mHero.at(j).hero_id)
						{
							profAnother = (Profession)s_mHero.at(j).profession_id;
							nameAnother = s_mHero.at(j).name;
						}
					}				
					break;
				case 3:
					for (int j=0; j!=s_mHero.size(); ++j)
					{
						if(s_mBattle.first_turn_hero_id_arr[2] == s_mHero.at(j).hero_id)
						{
							profAnother = (Profession)s_mHero.at(j).profession_id;
							nameAnother = s_mHero.at(j).name;
						}
					}				
					break;
				default:
					break;
				}

				break;
			}
		}
		
	}
	else if(mFightNum==2)
	{
		int id = MyselfManager::getManager()->getMyZhuJueData()->getId();
		for (int xx=0; xx<4; ++xx)
		{
			if(s_mHero.at(xx).hero_id == id)
				nameMine = s_mHero.at(xx).name;
		}

		if(id == s_mBattle.second_turn_hero_id_arr[0])
		{
			for (int j=0; j!=s_mHero.size(); ++j)
			{
				if(s_mBattle.second_turn_hero_id_arr[1] == s_mHero.at(j).hero_id)
				{
					profAnother = (Profession)s_mHero.at(j).profession_id;
					nameAnother = s_mHero.at(j).name;
				}
			}		
		}
		else
		{
			for (int j=0; j!=s_mHero.size(); ++j)
			{
				if(s_mBattle.second_turn_hero_id_arr[0] == s_mHero.at(j).hero_id)
				{
					profAnother = (Profession)s_mHero.at(j).profession_id;
					nameAnother = s_mHero.at(j).name;
				}
			}	
		}
	}
	else
	{
		CCLog("update waiting fingt time goto fignt error [mFightNum : %d]", mFightNum);
		CCAssert(false, "error flag");
	}


	{
		CCSize winSize = CCDirector::sharedDirector()->getVisibleSize();
		winSize.height -= 100;

		CCLayerColor *bg = CCLayerColor::create(ccc4(0,0,0,255), 640, 1152);
		CCDirector::sharedDirector()->getRunningScene()->addChild(bg, 1000);
		bg->setPosition(ccp(0,0));

		//vs img
		BaseSprite *vsImg = BaseSprite::create("vs.png");
		bg->addChild(vsImg, 10);
		vsImg->setPosition(ccp(winSize.width/2, winSize.height/2));
		vsImg->setOpacity(0);
		vsImg->runAction(CCFadeTo::create(0.5, 255));

		//vs effect
		BaseSprite *vsEf = BaseSprite::create("vs.png");
		bg->addChild(vsEf, 11);
		vsEf->setPosition(vsImg->getPosition());
		vsEf->setOpacity(0);
		vsEf->setScale(20);
		vsEf->runAction(CCSequence::create(
			CCFadeTo::create(0.15f, 60)
			, CCSpawn::create(CCFadeTo::create(0.35f, 200), CCScaleTo::create(0.35f, 1), NULL)
			, CCSpawn::create(CCFadeTo::create(0.15f, 0), NULL)
			, NULL));

		//mine   left
		//head pic  84
		{
			string file;
			if(profMine == Profession_Yemanren)
				file = "juqing_duihua_barbarian_VS.png";
			else if(profMine == Profession_Fashi)
				file = "juqing_duihua_wizard_VS.png";
			else 
				file = "juqing_duihua_hunter_VS.png";
			BaseSprite *headPic = BaseSprite::create(file);
			headPic->setFlipX(true);
			bg->addChild(headPic);
			headPic->setPosition(ccp(60, winSize.height/2+252));
			headPic->setOpacity(0);
			headPic->runAction(CCSequence::create(
				CCSpawn::create(CCFadeTo::create(0.15f, 255), CCMoveBy::create(3, ccp(60, 0)), NULL)	
				, NULL));
			//name
			CCLabelTTF *mineName = CCLabelTTF::create(nameMine.c_str(), fontStr_kaiti, 26);
			bg->addChild(mineName);
			mineName->setPosition(ccp(118, winSize.height/2-146));
			mineName->setOpacity(0);
			mineName->runAction(CCFadeTo::create(0.15f, 255));
		}

		//another pic 561
		{
			string file;
			if(profAnother == Profession_Yemanren)
				file = "juqing_duihua_barbarian_VS.png";
			else if(profAnother == Profession_Fashi)
				file = "juqing_duihua_wizard_VS.png";
			else 
				file = "juqing_duihua_hunter_VS.png";
			BaseSprite *headPic = BaseSprite::create(file);
			bg->addChild(headPic);
			headPic->setPosition(ccp(581, winSize.height/2+252));
			headPic->setOpacity(0);
			headPic->runAction(CCSequence::create(
				CCSpawn::create(CCFadeTo::create(0.15f, 255), CCMoveBy::create(3, ccp(-60, 0)), NULL)
				, NULL));
			//name
			CCLabelTTF *anotherName = CCLabelTTF::create(nameAnother.c_str(), fontStr_kaiti, 26);
			bg->addChild(anotherName);
			anotherName->setPosition(ccp(526, winSize.height/2-146));
			anotherName->setOpacity(0);
			anotherName->runAction(CCFadeTo::create(0.15f, 255));
		}
	}
	
	this->runAction(CCSequence::create(
		CCDelayTime::create(2.5)
		, CCCallFunc::create(this, callfunc_selector(JingJiChangLayer::timeToFight))
		, NULL));
}

void JingJiChangLayer::timeToFight()
{
	prepareFight(mFightNum);
}

void JingJiChangLayer::AgainstPlan1(float dt)
{
	mFightNum = 1;

	CCLabelTTF *label = (CCLabelTTF *)mBg->getChildByTag(Tag_InfoWindow_Label);
	label->runAction(CCSequence::create(
		CCDelayTime::create(1.0f),
		CCFadeOut::create(0.5f),
		CCCallFuncN::create(this, callfuncN_selector(JingJiChangLayer::resetInfoLabel)),
		NULL));
	scheduleOnce(schedule_selector(JingJiChangLayer::waitingForFightWithDisplayInfo), 4);
}


void JingJiChangLayer::AgainstPlan2(float dt)
{
	mFightNum = 2;

	CCLabelTTF *label = (CCLabelTTF *)mBg->getChildByTag(Tag_InfoWindow_Label);
	label->runAction(CCSequence::create(
		CCDelayTime::create(1.0f),
		CCFadeOut::create(0.5f),
		CCCallFuncN::create(this, callfuncN_selector(JingJiChangLayer::resetInfoLabel)),
		NULL));

	BaseSprite* vsImg = (BaseSprite*)mBg->getChildByTag(Tag_Img_Vs_ + 3);
	vsImg->runAction(CCSequence::create(
		CCFadeTo::create(0.25, 0)
		, CCCallFuncND::create(this, callfuncND_selector(JingJiChangLayer::vsImgChange), (void*)3)
		, NULL));
	scheduleOnce(schedule_selector(JingJiChangLayer::waitingForFightWithDisplayInfo), 3);
}

void JingJiChangLayer::everyFightEndDraw(int flag)
{
	int iWin = 0, iFall = 3;
	CCPoint winHeroPos[4];
	int idx[4];

	for(int i=0; i!=s_mHero.size(); ++i)
	{
		drawHeroImgAndInfo(i);
		// 1. 先绘制第一场的
		bool bWin = false; // 第一场输赢
		for(int j=0; j!=2; ++j)
		{
			if(s_mBattle.second_turn_hero_id_arr[j] == s_mHero.at(i).hero_id)
			{	
				bWin = true;
				CCNode* heroImg = mBg->getChildByTag(Tag_Hero_FullPhoto + i);
				winHeroPos[iWin] = heroImg->getPosition();
				idx[iWin] = i;
				++iWin;
			}
		}

		if(! bWin) // 第一场输了就变灰
		{
			CCNode* heroImg = mBg->getChildByTag(Tag_Hero_FullPhoto + i);		
			CCArray * childen = heroImg->getChildren();
			if(childen != NULL)
				for(int num=0; num!=childen->count(); ++num)
					setAllChildrenColorOrOpacity(dynamic_cast<CCSprite*>(childen->objectAtIndex(num)), otherFall_label_color, 150, 3);

			winHeroPos[iFall] = heroImg->getPosition();
			idx[iFall] = i;
			--iFall;

			if (s_mHero.at(i).hero_id == MyselfManager::getManager()->getMyZhuJueData()->getId())
				((CCLabelTTF*)heroImg->getChildByTag(Tag_Board_Name)->getChildByTag(Tag_Label_Name))->setColor(mineFall_label_color);
			else 
				((CCLabelTTF*)heroImg->getChildByTag(Tag_Board_Name)->getChildByTag(Tag_Label_Name))->setColor(otherFall_label_color);
		}


		// 2. 如果是第二场结束  且该hero第一场赢了
		if(flag == 2 && bWin)
		{
			if( s_mHero.at(i).hero_id == MyselfManager::getManager()->getMyZhuJueData()->getId()
				&& s_mBattle.scriptVec.at(1).endResult.myResult != Result_Win)
			{//第二场自己输
				CCNode* heroImg = mBg->getChildByTag(Tag_Hero_FullPhoto + i);		
				CCArray * childen = heroImg->getChildren();
				if(childen != NULL)
					for(int num=0; num!=childen->count(); ++num)
						setAllChildrenColorOrOpacity(dynamic_cast<CCSprite*>(childen->objectAtIndex(num)), otherFall_label_color, 150, 3);

				((CCLabelTTF*)heroImg->getChildByTag(Tag_Board_Name)->getChildByTag(Tag_Label_Name))->setColor(mineFall_label_color);
			}
			else if( s_mHero.at(i).hero_id != MyselfManager::getManager()->getMyZhuJueData()->getId() 
				&& s_mBattle.scriptVec.at(1).endResult.myResult == Result_Win)
			{//第二场别人输
				CCNode* heroImg = mBg->getChildByTag(Tag_Hero_FullPhoto + i);		
				CCArray * childen = heroImg->getChildren();
				if(childen != NULL)
					for(int num=0; num!=childen->count(); ++num)
						setAllChildrenColorOrOpacity(dynamic_cast<CCSprite*>(childen->objectAtIndex(num)), otherFall_label_color, 150, 3);

				((CCLabelTTF*)heroImg->getChildByTag(Tag_Board_Name)->getChildByTag(Tag_Label_Name))->setColor(otherFall_label_color);
			}
		}
	}
	if(flag == 1)  //vs换为  失败 和 胜利
	{
		BaseSprite* shu = (BaseSprite*)mBg->getChildByTag(Tag_Img_Vs_ + 1);
		BaseSprite* yin = (BaseSprite*)mBg->getChildByTag(Tag_Img_Vs_ + 3);

		shu->runAction(CCSequence::create(
			CCFadeTo::create(0.25, 0)
			, CCCallFuncND::create(this, callfuncND_selector(JingJiChangLayer::vsImgChange), (void*)1)
			, NULL));
		yin->runAction(CCSequence::create(
			CCFadeTo::create(0.25, 0)
			, CCCallFuncND::create(this, callfuncND_selector(JingJiChangLayer::vsImgChange), (void*)2)
			, NULL));
	}


	//输赢位置切换
	bool usedPos[2] = {false, false};
	for(int i=0; i!=2; ++i)
	{
		CCNode *winHeroImg, *fallHeroImg;
		if(winHeroPos[i].y == POS(0).y || winHeroPos[i].y == POS(1).y)
		{
			winHeroImg = mBg->getChildByTag(Tag_Hero_FullPhoto + idx[i]);
			fallHeroImg = mBg->getChildByTag(Tag_Hero_FullPhoto + idx[3]);
			if (fallHeroImg->getPositionY() == POS(3).y && !usedPos[1])
			{
				usedPos[1] = true;

				mChangePosIndx1.first = idx[i];
				mChangePosIndx1.second = idx[3];
			}
			else
			{
				fallHeroImg = mBg->getChildByTag(Tag_Hero_FullPhoto + idx[2]);
				usedPos[0] = true;

				mChangePosIndx2.first = idx[1];
				mChangePosIndx2.second = idx[2];
			}

			//全身像 位置交换
			swapPosition(flag, winHeroImg, fallHeroImg);
		}

	}
}

void JingJiChangLayer::vsImgChange(CCNode *node, void *data)
{
	int flag = (int)data;
	string file;
	if(flag == 1)
		file = "jingjichang_id_shibai_icon.png";
	else if(flag == 2)
		file = "jingjichang_id_shengli_icon.png";
	else if(flag == 3)
		file = "jingjichang_id_vs_icon.png";

	BaseSprite* img = (BaseSprite*)node;
	img->initWithFile(file);
	img->setOpacity(0);
	img->runAction(CCFadeTo::create(0.25, 255));
}

//node1 从里到外
void JingJiChangLayer::swapPosition(int flag, CCNode* node1, CCNode* node2)
{
	float time = 0.35f;
	CCPoint pos1 = node1->getPosition();
	CCPoint pos2 = node2->getPosition();
	GLubyte opacity = 150;

	if(flag == 1) //第一场结束
	{
		node1->runAction(CCSequence::create(
			CCDelayTime::create(1)
			, CCMoveTo::create(time, pos2), NULL));
		if (pos2.y > POS(0).y-50) mBg->reorderChild(node1, 10);
		else mBg->reorderChild(node1, 1);

		node2->runAction(CCSequence::create(
			CCDelayTime::create(1)
			, CCMoveTo::create(time, pos1), NULL));
		if (pos1.y > POS(0).y-50) mBg->reorderChild(node2, 10);
		else mBg->reorderChild(node2, 1);

		if(flag != 0)
		{
			CCArray * childen = node2->getChildren();
			if(childen != NULL)
				for(int num=0; num!=childen->count(); ++num)
					setAllChildrenColorOrOpacity(dynamic_cast<CCSprite*>(childen->objectAtIndex(num)), ((CCSprite*)childen->objectAtIndex(num))->getColor(), 150, 2);
		}
	}
	else if(flag == 2)
	{
		node1->setPosition(pos2);
		if (pos2.y > POS(0).y-50) mBg->reorderChild(node1, 10);
		else mBg->reorderChild(node1, 1);

		node2->setPosition(pos1);
		if (pos1.y > POS(0).y-50) mBg->reorderChild(node2, 10);
		else mBg->reorderChild(node2, 1);
		
		CCArray * childen = node2->getChildren();
		if(childen != NULL)
			for(int num=0; num!=childen->count(); ++num)
				setAllChildrenColorOrOpacity(dynamic_cast<CCSprite*>(childen->objectAtIndex(num)), ((CCSprite*)childen->objectAtIndex(num))->getColor(), 150, 2); 
	}
}

void JingJiChangLayer::prepareFight(int flag)
{

	CCCallFuncND* callfight = CCCallFuncND::create(this, callfuncND_selector(JingJiChangLayer::gotoFight), (void*)flag);
	this->runAction(CCSequence::create(callfight, NULL));

}

void JingJiChangLayer::gotoFight(CCNode* node, void* pflag)
{

	int flag = (int)pflag;

	FightDirector dir;
	vector<FightDirector> dir_vec;

	Bundle bundle;
	if(flag == 1)
	{
		dir.initScript(s_mBattle.scriptVec[0]);
		dir_vec.push_back(dir);
		bundle.putString("JumpToWhenEnd","JingJi_2");
	}
	else 
	{
		dir.initScript(s_mBattle.scriptVec[1]);
		dir_vec.push_back(dir);
		bundle.putString("JumpToWhenEnd","JingJi_KaiBaoXiang");
	}

	// 直接换scene
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec
		, MapsManager::getManager()->getJingJiChangMapImage(), bundle);
}

void JingJiChangLayer::CombatSettlement(float dt)
{
	CCLog("JingJiChang win");

	int mici = 3;
	string infStr;
	int flag = 0;
	if(s_mBattle.second_turn_hero_id_arr[0]==MyselfManager::getManager()->getMyZhuJueData()->getId() 
		|| s_mBattle.second_turn_hero_id_arr[1]==MyselfManager::getManager()->getMyZhuJueData()->getId())
	{
		if(s_mBattle.scriptVec.at(1).endResult.myResult == Result_Win)
		{
			infStr = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().customName + "  " + LFStrings::getValue("shengchu")+"!";//第二场赢
			flag = 3; // 开三个箱子
			mici = 1;
		}
		else
		{
			infStr = LFStrings::getValue("gongxini") + "\n" + LFStrings::getValue("cilunshibai");//第二场输
			flag = 2;
			mici = 2;
		}
	}
	else
	{
		infStr = LFStrings::getValue("gongxini") + "\n" + LFStrings::getValue("shoulunshibai");//第一场输
		flag = 1;
	}
	
	{
		//弹框提示
		//弹框背景
		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
		dialog->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
		dialog->setTag(TAG_TiShi_DIALOG);
		//label
		char buf[20];
		string temp = LFStrings::getValue("jingjichangJieSuan");
		sprintf(buf, "%d", mici);
		temp += (buf + LFStrings::getValue("ming") + "\n" + LFStrings::getValue("jiangli"));
		sprintf(buf, "%d", flag);
		temp += (buf + LFStrings::getValue("ge") + LFStrings::getValue("baoxiang"));

		CCLabelTTF *label = CCLabelTTF::create(temp.c_str(), 
			fontStr_kaiti, 30, CCSize(360, 0), kCCTextAlignmentCenter);
		dialog->addChild(label);
		label->setAnchorPoint(ccp(0.5, 1));
		label->setPosition(ccp(dialog->getContentSize().width/2, 258));

		//按键
		CCMenu* menu;
		{
			CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
				"shangpin_goumai_queren.png",
				"shangpin_goumai_queren_select.png",
				this,SEL_MenuHandler(&JingJiChangLayer::menuItemClicked_tishiInfo_Ok));
			menuItemMakeSure->setUserData((void*)flag);

			menu = CCMenu::create(menuItemMakeSure, NULL);
			menu->setPosition(CCPointZero);

			menuItemMakeSure->setTag(TAG_TiShi_DIALOG_ok);
			menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/2,70));
		}

		//弹框
		{
			LFAlert* lfAlert = new LFAlert();
			lfAlert->setTag(TAG_TiShi_DIALOG);
			lfAlert->setBaseContainer(dialog,menu);
			CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert, 128, TAG_TiShi_DIALOG);
			lfAlert->release();
		}
	}


//	this->runAction(CCCallFuncND::create(this, callfuncND_selector(JingJiChangLayer::openAwardBox), (void*)flag));
}

void JingJiChangLayer::menuItemClicked_tishiInfo_Ok(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_TiShi_DIALOG);

	int num = (int)((CCMenuItemImage*)pSender)->getUserData();
	this->runAction(CCCallFuncND::create(this, callfuncND_selector(JingJiChangLayer::openAwardBox), (void*)num));
}

void JingJiChangLayer::openAwardBox(CCNode *pSender, void *data)
{
	int boxNum = (int)data;

	//s_mBattle
	vector<GiftData> gettedData;
	GiftData temp;
	for (int i=0; i!=s_mBattle.gettedData.size(); ++i)
	{
		temp.type             = GiftType_Jifen;
		temp.gift_id          = s_mBattle.gettedData.at(i).gift_id;
		temp.data_union.jifen = s_mBattle.gettedData.at(i);
		gettedData.push_back(temp);
	}
	vector<GiftData> missedData;
	for (int i=0; i!=s_mBattle.missedData.size(); ++i)
	{
		temp.type             = GiftType_Jifen;
		temp.gift_id          = s_mBattle.missedData.at(i).gift_id;
		temp.data_union.jifen = s_mBattle.missedData.at(i);
		missedData.push_back(temp);
	}
	OpenBoxLayer* layer = OpenBoxLayer::create(fromJingJiChang, boxNum, gettedData, missedData);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);

}

void JingJiChangLayer::error_TimeOut(CCObject *msg)
{
	GameDirector::getDirector()->mainScene(mainlayer_JingJiChang_random);
}

void JingJiChangLayer::addWaitingLayer(float dt)
{
	WaitLayer* layer = WaitLayer::create("");
	layer->removeAllChildren();
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1000, TAG_Waiting_Layer);
}