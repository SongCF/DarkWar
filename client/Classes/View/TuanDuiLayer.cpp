
#include "TuanDuiLayer.h"
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
#include "ClientFightLayer_TF.h"
#include "ImageManager.h"
#include "LFAlert.h"
#include "openBoxLayer.h"
#include "MessageDisplay.h"
#include "GetTiLiLayer.h"
#include "MapsManager.h"
#include "TimeoutMonitor.h"

#define TimeOut 30

enum TAG{

	Tag_InfoWindow_Label = 1
	, Tag_HeroInfo_Label

	, Tag_Hero_FullPhoto = 10
};


TuanDuiLayer::TuanDuiLayer()
	:mBg(NULL)
	,mWaitActorTime(TimeOut)
	,mWaitFightTime(3)
{

	{
		//长时间死等监听
		addObserver(callfuncO_selector(TuanDuiLayer::error_TimeOut), MSG_WAIT_TIME_OUT);
	}
}

TuanDuiLayer::~TuanDuiLayer()
{
	CC_SAFE_RELEASE(mBg);
}

bool TuanDuiLayer::init()
{
	if( !BaseLayer::init() )
		return false;

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();


	WaitLayer* layer = WaitLayer::create("");
	layer->removeAllChildren();
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1000, TAG_Waiting_Layer);

	//footer bg
	mBg = new BaseSprite;
	mBg->initWithFile("tuanduifuben_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(winSize.width/2, winSize.height/2));

	//biankuang
	BaseSprite *biankuang = BaseSprite::create("tuanduifuben_biankuang.png");
	mBg->addChild(biankuang, 10);
	biankuang->setPosition(ccp(320,851-500));

	//info window
	mInfoWindow = BaseSprite::create("tuanduifuben_xinxi.png");
	mBg->addChild(mInfoWindow);
	mInfoWindowSize = mInfoWindow->getContentSize();
	mCurDeltaHeight = mInfoWindowSize.height;
	mInfoWindow->initWithFile("PVP_xinxi.png", CCRect(0,mCurDeltaHeight,mInfoWindowSize.width, mInfoWindowSize.height));
	schedule(schedule_selector(TuanDuiLayer::infoWindowDown), 0.01f);
	mInfoWindow->setAnchorPoint(ccp(0.5,1));
	mInfoWindow->setPosition(ccp(320, 851-272+mInfoWindowSize.height/2));

	//
	{	
		SPCmd_TeamBossEnter enter;
		enter.hardLevel = 0;
		addObserver(callfuncO_selector(TuanDuiLayer::teamBossEnterRsp), MSG_teamBossEnterRsp_F);
		addObserver(callfuncO_selector(TuanDuiLayer::teamBossUpdateRoomRsp), MSG_teamBossUpdateRoomRsp_F);
		addObserver(callfuncO_selector(TuanDuiLayer::teamBossGetBattleScriptAndGiftRsp), MSG_teamBossGetBattleScriptAndGiftRsp_F);
		mCmdHlp->cmdTeamBossEnter(enter);
	}

	return true;
}

void TuanDuiLayer::infoWindowDown(float dt)
{
	//避免用户输入
	if (NULL==CCDirector::sharedDirector()->getRunningScene()->getChildByTag(TAG_Waiting_Layer))
	{
		WaitLayer* layer = WaitLayer::create("");
		layer->removeAllChildren();
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1000, TAG_Waiting_Layer);
	}

	mCurDeltaHeight -= 10;
	if(mCurDeltaHeight <= 0)
	{
		unschedule(schedule_selector(TuanDuiLayer::infoWindowDown));
		mCurDeltaHeight = 0;
	}
	mInfoWindow->initWithFile("tuanduifuben_xinxi.png", CCRect(0, mCurDeltaHeight, mInfoWindowSize.width, mInfoWindowSize.height));
	mInfoWindow->setAnchorPoint(ccp(0.5,1));
	mInfoWindow->setPosition(ccp(320, 851-162));

	if(mCurDeltaHeight == 0)
	{
		CCLabelTTF* label = CCLabelTTF::create("", fontStr_kaiti, 24);
		mInfoWindow->addChild(label, 1, Tag_InfoWindow_Label);
		label->setAnchorPoint(ccp(0,0.5));
		label->setPosition(ccp(mInfoWindow->getContentSize().width/2-50, mInfoWindow->getContentSize().height/2));
		label->setColor(ccc3(0x3d, 0x3d, 0x3d));

		schedule(schedule_selector(TuanDuiLayer::updateWindowInfomation), 0.75);
	}
}

void TuanDuiLayer::reloadInfoWindow(float dt)
{
	if(m_bUp)
		mCurDeltaHeight += 10;
	else 
		mCurDeltaHeight -= 10;

	if(mCurDeltaHeight <= 0)
	{
		unschedule(schedule_selector(TuanDuiLayer::reloadInfoWindow));
		mCurDeltaHeight = 0;
	}
	else if (mCurDeltaHeight >= mInfoWindowSize.height)
	{
		mCurDeltaHeight = mInfoWindowSize.height;

		m_bUp = false;
	}

	mInfoWindow->initWithFile("tuanduifuben_xinxi.png", CCRect(0, mCurDeltaHeight, mInfoWindowSize.width, mInfoWindowSize.height));
	mInfoWindow->setAnchorPoint(ccp(0.5,1));
	mInfoWindow->setPosition(ccp(320, 851-162));

	if(mCurDeltaHeight == 0)
	{
		string str = LFStrings::getValue("tuanduizhizhankaishitishi");

		CCLabelTTF* label = CCLabelTTF::create(str.c_str(), fontStr_kaiti, 24);
		mInfoWindow->addChild(label, 1, Tag_InfoWindow_Label);
		label->setPosition(ccp(mInfoWindow->getContentSize().width/2, mInfoWindow->getContentSize().height/2));
		label->setColor(ccc3(0x3d, 0x3d, 0x3d));
	}
}

void TuanDuiLayer::updateWindowInfomation(float dt)
{
	{	
		string str;
		str = LFStrings::getValue("dengdaizhong");

		// 0 1 2 3 省略号
		mCurDeltaHeight = ++mCurDeltaHeight % 3;
		for(int i=0; i<=mCurDeltaHeight; ++i)
			str += ".";

		CCLabelTTF *label = (CCLabelTTF*)mInfoWindow->getChildByTag(Tag_InfoWindow_Label);
		label->setString(str.c_str());
	}
}

vector<ZhuangbeiSummary> TuanDuiLayer::getHeroEquipSumById(int heroId)
{
	vector<ZhuangbeiSummary> vec;
	for(int i=0; i!=mHero.size(); ++i)
	{
		if(mHeroEquipData[i].size() == 0 )
			continue;
		else
		{
			if(mHeroEquipData[i].at(0).zhuangbei_hero_id == heroId)
			{
				for (int j=0; j!=mHeroEquipData[i].size(); ++j)
				{
					ZhuangbeiSummary sum;
					sum.zhuangbei_colour = mHeroEquipData[i].at(j).zhuangbei_colour;
					strcpy(sum.zhuangbei_pic , mHeroEquipData[i].at(j).equipPic.c_str());
					sum.zhuangbei_type = mHeroEquipData[i].at(j).zhuangbei_type;
					vec.push_back(sum);
				}
				return vec;
			}
		}
	}

	return vec;
}

void TuanDuiLayer::drawHeroImgAndInfo(int i)
{
#define POS(x) ccp(158+x%2*312, 348-x/2*183)
#define LABEL_SIZE 20

	CCNode *heroImg;
	if(mHero.at(i).hero_id == MyselfManager::getManager()->getMyZhuJueData()->getId())
	{
		heroImg = ImageManager::getManager()->getFullPhotoOfActor(MyselfManager::getManager()->getMyZhuJueData());
	}
	else
	{
		heroImg = ImageManager::getManager()->getFullPhotoOfActor((Profession)mHero.at(i).profession_id
			, getHeroEquipSumById(mHero.at(i).hero_id), false);
	}
	mBg->addChild(heroImg, i, Tag_Hero_FullPhoto + i);
	heroImg->setPosition(POS(i));
	heroImg->setScale(0.9f);


	BaseSprite *infoBar = BaseSprite::create("tuanduifuben_id_bg.png");
	heroImg->addChild(infoBar);
	infoBar->setPosition(ccp(heroImg->getContentSize().width/2, -30));

	CCLabelTTF *labelName = CCLabelTTF::create(mHero.at(i).name, fontStr_kaiti, LABEL_SIZE);
	infoBar->addChild(labelName);
	labelName->setPosition(ccp(86, 21));

	if(mHero.at(i).hero_id == MyselfManager::getManager()->getMyZhuJueData()->getId())
		labelName->setColor(ccc3(0,200,0));

}

void TuanDuiLayer::teamBossEnterRsp(CCObject* f_msg_rsp)
{
	removeObserver(MSG_teamBossEnterRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_MSG_Rsp* rsp  =  (Flat_MSG_Rsp*)f_msg_rsp;

	Flat_TeamBossEnterRsp* data =  (Flat_TeamBossEnterRsp*)rsp->getData();
	if( data->err_code != Success)
	{
		if(data->err_code == TiliNotEnought)
		{
			GetTiLiLayer* layer = GetTiLiLayer::create();
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);

			GameDirector::getDirector()->mainScene(mainlayer_HuoDong);
		}
		return;
	}

	mHero = data->already_waited;

	HeroInfo mine;
	BaseActorProperty prof = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty();
	mine.hero_id = prof.hero_id;
	mine.level = prof.level;
	strcpy(mine.name, prof.customName.c_str());
	mine.profession_id = prof.profession;
	mHero.push_back(mine);

	for(int i=0; i<mHero.size(); ++i)
	{
		mHeroEquipData[i] = data->getEquipsByHeroId(mHero.at(i).hero_id);
		drawHeroImgAndInfo(i);
	}

}

void TuanDuiLayer::teamBossUpdateRoomRsp(CCObject* f_msg_rsp)
{
	//update 每次只有一个
	Flat_ArenaUpdateRoomRsp* rsp = (Flat_ArenaUpdateRoomRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if(rsp->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		this->addChild(layer, 1025);
		return;
	}

	mHero.push_back(rsp->new_hero_info);
	mHeroEquipData[mHero.size()-1] = rsp->equipsOnHero;

	{
		drawHeroImgAndInfo(mHero.size()-1);
	}
}

void TuanDuiLayer::teamBossGetBattleScriptAndGiftRsp(CCObject* f_msg_rsp)
{
	removeObserver(MSG_teamBossUpdateRoomRsp_F);
	removeObserver(MSG_teamBossGetBattleScriptAndGiftRsp_F);
	Flat_TeamBossGetBattleScriptGiftRsp* data = (Flat_TeamBossGetBattleScriptGiftRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		this->addChild(layer, 1025);
		return;
	}

	mScript = *data;
	data = NULL;

	unschedule(schedule_selector(TuanDuiLayer::updateWindowInfomation));
	m_bUp = true;
	mCurDeltaHeight = 0;
	mInfoWindow->removeAllChildrenWithCleanup(true);
	schedule(schedule_selector(TuanDuiLayer::reloadInfoWindow), 0.01f);


	scheduleOnce(schedule_selector(TuanDuiLayer::updateWaitingFightTime), 5);
}

void TuanDuiLayer::updateWaitingFightTime(float dt)
{
	gotoFight();
}


void TuanDuiLayer::gotoFight()
{
	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(mScript.fightScript);
	dir_vec.push_back(dir);

	ClientFightLayer_TF* layer = (ClientFightLayer_TF*)GameDirector::getDirector()->fight_team(dir_vec
		, MapsManager::getManager()->getTuanDuiFuBenMapImage());
	layer->setData(mScript, mHero);
}

void TuanDuiLayer::error_TimeOut(CCObject *msg)
{
	GameDirector::getDirector()->mainScene(mainlayer_HuoDong);
}