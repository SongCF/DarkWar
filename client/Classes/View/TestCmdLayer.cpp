#include "TestCmdLayer.h"
#include "GameInforEditer.h"
#include "GameDirector.h"
#include "NetworkWaitingUI.h"
// 测试用的
#include "CS_Common.h"
#include "WuPinManager.h"
#include "EquipmentManager.h"
#include "MarketManager.h"
#include "FightScriptBuilder.h"
#include "LFToast.h"
#include "LFUtile.h"
#include "LFMessageBox.h"
#include "ClientFightLayer.h"
#include "FightSettlementLayer.h"
#include "MyselfManager.h"
#include "MapsLayer.h"
#include "ClientFightLayer_TF.h"
#include "flat.h"
#include "GetTiLiLayer.h"
#include "LFDelayToMusic.h"
#include "BackgroudLayer.h"
#include "GameScene.h"
#include "ZhuangBeiMianBanBase.h"
#include "MofaJinghuaManager.h"
#include "DuanZaoManager.h"
#include "LevelUpLayer.h"
#include "SuiPianManager.h"
#include "LFAutoReleasePool.h"
#include "GuideScriptMaker.h"
#include "ZhuangbeiScanner.h"
//#include "memguard.h"
#include "GlobalTickTrigle.h"
#include "FightScriptUserData.h"
#include "SystemGiftBagManager.h"

#define MENU_TAG				1024

#define TAG_Login				1
#define TAG_GetItems			2
#define TAG_CreateZhuShou		3
#define TAG_GetHerosInTeam		4
#define TAG_GetZhuangBei		5
#define TAG_BuZheng				0
#define TAG_ZBQiangHua			6
#define TAG_ZBZhuankong			7
#define TAG_ZBChaijie			8
#define TAG_ZBXiangBaoshi		9
#define TAG_ZBChaiBaoshi		10
#define TAG_BaoshiHecheng		11
#define TAG_GetMarketInfor		12
#define TAG_ByItemInMarket		13
#define TAG_GetTaskList			14
#define TAG_GenScript			15
#define TAG_GenScript_BOOSNOMOVE 15101
#define TAG_GenScript_Team		15100
#define TAG_GetChessBoard		16
#define TAG_Battle				17
#define TAG_GotTask				18
#define TAG_SubmiteTask			19
#define TAG_TiLi				20	// 恢复体力
#define TAG_ZH_MSG				21
#define TAG_KEY_ZH_MSG			22
#define TAG_UTF8				23
#define TAG_TOAST				24

#define TAG_ArenaEnter			25	// 进入竞技场
#define TAG_TeamBossEnter		26	//
#define TAG_GetGift				27	//
#define Tag_Level_Up 28

#define TAG_CityGuardEnter		30
#define TAG_CityGuardGetNextMonster		31
#define TAG_CityGuardFight_Yes		32
#define TAG_CityGuardFight_NO		33

#define TAG_topHeroGetList		60
#define TAG_topHeroChallenge		61

#define TAG_topArenaEnter		81
#define TAG_topArenaGuess		82
#define TAG_topArenaWatchTV		83
#define TAG_topArenaGetGift		84

#define TAG_chargeVitality		100

#define TAG_systemGiftQuery		120
#define TAG_systemGiftGetGift	121

#define TAG_DelayMusic			141

#define TAG_DuanzaoListQuery	142
#define TAG_Duanzao				143
#define TAG_Duanzao_10			144
#define TAG_DuanzaoZBGroupAuery	145

#define TAG_QueryAllSuitName	150
#define TAG_ZBCuiqu				151
#define TAG_ZBFumo				152
#define TAG_QueryAllMofaJinghua	153
#define TAG_ZBQianghuaHuiTui	154	
#define TAG_ZBQianghuaNew		155	

#define TAG_FBDiaoLuo_1			161
#define TAG_FBDiaoLuo_2			162

#define TAG_querySubTaskList	180

#define TAG_CLOSE_NETWORK		190

#define TAG_SuiPianManager_TEST		200
#define TAG_HeChengItemSuiPian		210
#define TAG_HeChengSHenBinSuiPian	211

#define TAG_SaoDang				220

#define TAG_JieFengYing					231
#define TAG_ShenbingJianding			232
#define TAG_ZBJinglian					233
#define TAG_ShenbingZhuankeJingwen		234

#define TAG_QueryArenaTeamBasicInfo		240
#define TAG_QueryArenaMarketRsp			241
#define TAG_RefreshArenaChlgList		242
#define TAG_QueryArenaChlgListRsp		243
#define TAG_ArenaChallengeRsp			244
#define TAG_ArenaMarketBuyRsp			245
#define TAG_ArenaBuyChlgTimeRsp			246
#define TAG_queryZahuopu				250
#define TAG_refreshZahuopu				251
#define TAG_zahuopuGetOneItem			252
#define TAG_HeroUpdate					260
#define TAG_HeroUpdateStage				261
#define TAG_QueryHellTaskHeroInfo		270
#define TAG_HellTaskBattle				271
#define TAG_QueryYuanshenOfHero			281
#define TAG_ChurchLevelUp				290
#define TAG_SkillLevelUp				291

#define TAG_GetZhuangbeiAlbum			300

#define TAG_AutoReleasePool				310

#define TAG_getCityGuardCurrentStat		320
#define TAG_cityGuardBattle				321
#define TAG_cityGuardBattleSkipCD		322
#define TAG_getCityGuardLastDayReport	323
#define TAG_getMyCityGuardReward		324

#define TAG_queryRateTask				330
#define TAG_getRateTaskBonus			331

#define TAG_GuideScriptMaker			340

#define TAG_ZhuangbeiScanner			350

#define TAG_MEMGUARD					360

#define TAG_EquipPurify					400

enum TAG
{
	TAG_Email_Query = 410,
	TAG_Email_Send,
	TAG_QueryGiftBag,
	TAG_GetGiftBag,
};

  
class Sub1Object : public CCObject
{
protected:
	unsigned int _id;
public:
	Sub1Object()
	{
		CCLog("%s",__FUNCTION__);
		OneTrigle trigle = OneTrigle::makeTrigleDelay("++++",1,true);
		_id = GlobalTickTrigle::getTrigle()->addOneTrigleAndListen(trigle,this,SEL_CallFuncO(&Sub1Object::notified));
	}

	virtual ~Sub1Object()
	{
		CCLog("%s",__FUNCTION__);
		//GlobalTickTrigle::getTrigle()->removeTrigleAndRemoveListen(_id,this);
	}
private:
	void notified(CCObject* obj)
	{
		CCLog("%s",__FUNCTION__);
	}
};

class Sub2Object : public Sub1Object
{
protected:
	unsigned int _id;
public:
	Sub2Object()
	{
		_id = 20000;
		CCLog("%s",__FUNCTION__);
	}

	// 不virtual
	~Sub2Object()
	{
		CCLog("%s",__FUNCTION__);
	}
private:
	// 覆盖父类的方法
	void notified(CCObject* obj)
	{
		CCLog("%s",__FUNCTION__);
	}
};

class Sub3Object : public Sub2Object
{
protected:
	unsigned int _id;
public:
	Sub3Object()
	{
		_id = 30000;
		CCLog("%s",__FUNCTION__);
	}

	// 不写析构函数，那么默认就会调用父类的
	// 写了，那么就用自己的，如果父类是虚函数，那么就会再调用父类的
	virtual ~Sub3Object()
	{
		CCLog("%s",__FUNCTION__);
	}

private:
	// 覆盖父类的方法
	void notified(CCObject* obj)
	{
		CCLog("%s",__FUNCTION__);
	}
};



void testEquipVec();
void testGenScript(bool bossNoMove);

TestCmdLayer::TestCmdLayer(void)
{
	setTouchEnabled(true);
	//	得到的输出是：
	//	Sub1Object::Sub1Object
	//	Sub2Object::Sub2Object
	//	Sub3Object::Sub3Object
	//	Sub3Object::~Sub3Object
	//	Sub2Object::~Sub2Object
	//	Sub1Object::~Sub1Object
	//	MainLayerBase::onExit
	//	Sub1Object::notified
	//	Sub1Object::notified
	//	Sub1Object::notified
	//Sub3Object tempTest; 


	fight = new FightPrepare(this);
	BaseSprite* bg = BaseSprite::create("background1.png");
	addChild(bg,-1);
	bg->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));

	// 测试
	{
		CCArray* items = CCArray::createWithCapacity(10);

		CCMenuItemFont::setFontSize(40); 

		//
		CCMenuItemFont* itemMEMGUARD = CCMenuItemFont::create(
			"TAG_MEMGUARD",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemZhuangbeiScanner = CCMenuItemFont::create(
			"TAG_ZhuangbeiScanner",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemGuideScriptMaker = CCMenuItemFont::create(
			"TAG_GuideScriptMaker",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemQueryRateTask = CCMenuItemFont::create(
			"TAG_queryRateTask",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemGetRateTaskBonus = CCMenuItemFont::create(
			"TAG_getRateTaskBonus",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked)); 

		CCMenuItemFont* itemgetCityGuardCurrentStat = CCMenuItemFont::create(
			"TAG_getCityGuardCurrentStat",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemcityGuardBattl = CCMenuItemFont::create(
			"TAG_cityGuardBattle",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemcityGuardBattleSkipCD = CCMenuItemFont::create(
			"TAG_cityGuardBattleSkipCD",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemgetCityGuardLastDayReport = CCMenuItemFont::create(
			"TAG_getCityGuardLastDayReport",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemgetMyCityGuardReward = CCMenuItemFont::create(
			"TAG_getMyCityGuardReward",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));


		CCMenuItemFont* itemAutoReleasePool = CCMenuItemFont::create(
			"TAG_AutoReleasePool",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemGetZhuangbeiAlbum = CCMenuItemFont::create(
			"TAG_GetZhuangbeiAlbum",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemChurchLevelUp = CCMenuItemFont::create(
			"TAG_ChurchLevelUp",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemSkillLevelUp = CCMenuItemFont::create(
			"TAG_SkillLevelUp",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemQueryYuanshenOfHero = CCMenuItemFont::create(
			"TAG_QueryYuanshenOfHero",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemQueryHellTaskHeroInfo = CCMenuItemFont::create(
			"TAG_QueryHellTaskHeroInfo",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemHellTaskBattle = CCMenuItemFont::create(
			"TAG_HellTaskBattle",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemHeroUpdate = CCMenuItemFont::create(
			"TAG_HeroUpdate",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemHeroUpdateStage = CCMenuItemFont::create(
			"TAG_HeroUpdateStage",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemQueryZahuopu = CCMenuItemFont::create(
			"TAG_queryZahuopu",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemRefreshZahuopu = CCMenuItemFont::create(
			"TAG_refreshZahuopu",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemZahuopuGetOneItem = CCMenuItemFont::create(
			"TAG_zahuopuGetOneItem",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemQueryArenaTeamBasicInfo = CCMenuItemFont::create(
			"TAG_QueryArenaTeamBasicInfo",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemShenbingZhuankeJingwen = CCMenuItemFont::create(
			"TAG_ShenbingZhuankeJingwen",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemQueryArenaMarketRsp = CCMenuItemFont::create(
			"TAG_QueryArenaMarketRsp",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemRefreshArenaChlgList = CCMenuItemFont::create(
			"TAG_RefreshArenaChlgList",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemQueryArenaChlgList = CCMenuItemFont::create(
			"TAG_QueryArenaChlgListRsp",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemArenaChallenge = CCMenuItemFont::create(
			"TAG_ArenaChallengeRsp",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* ItemArenaMarketBuy = CCMenuItemFont::create(
			"TAG_ArenaMarketBuyRsp",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemArenaBuyChlgTime = CCMenuItemFont::create(
			"TAG_ArenaBuyChlgTimeRsp",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));


		CCMenuItemFont* itemJieFengYing = CCMenuItemFont::create(
			"TAG_JieFengYing",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemShenbingJianding = CCMenuItemFont::create(
			"TAG_ShenbingJianding",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemZBJinglian = CCMenuItemFont::create(
			"TAG_ZBJinglian",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		//
		CCMenuItemFont* itemSaoDang = CCMenuItemFont::create(
			"TAG_SaoDang",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));


		CCMenuItemFont* itemSuiPianManager_TEST = CCMenuItemFont::create(
			"TAG_SuiPianManager_TEST",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemHeChengItemSuiPian = CCMenuItemFont::create(
			"TAG_HeChengItemSuiPian",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemHeChengSHenBinSuiPian = CCMenuItemFont::create(
			"TAG_HeChengSHenBinSuiPian",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemCloseNetwork = CCMenuItemFont::create(
			"TAG_CLOSE_NETWORK",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		//
		CCMenuItemFont* itemQuerySubTaskList = CCMenuItemFont::create(
			"TAG_querySubTaskList",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemFBDiaoLuo_1 = CCMenuItemFont::create(
			"TAG_FBDiaoLuo_1",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemFBDiaoLuo_2 = CCMenuItemFont::create(
			"TAG_FBDiaoLuo_2",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));


		CCMenuItemFont* itemQueryAllSuitName = CCMenuItemFont::create(
			"TAG_QueryAllSuitName",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemQueryAllMofaJinghua = CCMenuItemFont::create(
			"TAG_QueryAllMofaJinghua",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemZBCuiqu = CCMenuItemFont::create(
			"TAG_ZBCuiqu",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemZBFumo = CCMenuItemFont::create(
			"TAG_ZBFumo",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));


		CCMenuItemFont* itemQianghuaNew = CCMenuItemFont::create(
			"TAG_ZBQianghua",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemQianghuaHuiTui = CCMenuItemFont::create(
			"TAG_ZBQianghuaHuiTui",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemDuanzaoListQuery = CCMenuItemFont::create(
			"TAG_DuanzaoListQuery",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		//
		CCMenuItemFont* itemDuanzaoZBGroupQuery = CCMenuItemFont::create(
			"TAG_DuanzaoZBGroupAuery",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemDuanzao = CCMenuItemFont::create(
			"TAG_Duanzao",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemDuanzao10 = CCMenuItemFont::create(
			"TAG_Duanzao_10",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemDelayMusic = CCMenuItemFont::create(
			"TAG_DelayMusic",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemchargeVitality = CCMenuItemFont::create(
			"TAG_chargeVitality",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemsystemGiftQuery = CCMenuItemFont::create(
			"TAG_systemGiftQuery",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemsystemGiftGetGift = CCMenuItemFont::create(
			"TAG_systemGiftGetGift",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemTopArenaEnter = CCMenuItemFont::create(
			"TAG_topArenaEnter",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemTopArenaGuess = CCMenuItemFont::create(
			"TAG_topArenaGuess",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemTopArenaWatchTV = CCMenuItemFont::create(
			"TAG_topArenaWatchTV",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemTopArenaGetGift = CCMenuItemFont::create(
			"TAG_topArenaGetGift",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemTopHeroGetList = CCMenuItemFont::create(
			"TAG_topHeroGetList",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemTopHeroChallenge = CCMenuItemFont::create(
			"TAG_topHeroChallenge",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));


		CCMenuItemFont* itemCityGuardEnter = CCMenuItemFont::create(
			"TAG_CityGuardEnter",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));


		CCMenuItemFont* itemCityGuardGetNextMonster = CCMenuItemFont::create(
			"TAG_CityGuardGetNextMonster",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemCityGuardFight_Yes = CCMenuItemFont::create(
			"TAG_CityGuardFight_Yes",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemCityGuardFight_No = CCMenuItemFont::create(
			"TAG_CityGuardFight_No",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));


		CCMenuItemFont* itemToast = CCMenuItemFont::create(
			"TAG_Toast",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemLogin = CCMenuItemFont::create(
			"TAG_Login",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemGetItems = CCMenuItemFont::create(
			"TAG_GetItems",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemCreateZhuShou = CCMenuItemFont::create(
			"TAG_CreateZhuShou",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemGetHerosInTeam = CCMenuItemFont::create(
			"TAG_GetHerosInTeam",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemGetZhuangBei = CCMenuItemFont::create(
			"TAG_GetZhuangBei",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemHeroDetail = CCMenuItemFont::create(
			"TAG_HeroDetail",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemBuZheng = CCMenuItemFont::create(
			"TAG_BuZheng",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemZBQiangHua = CCMenuItemFont::create(
			"TAG_ZBQiangHua",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemZBZhuankong = CCMenuItemFont::create(
			"TAG_ZBZhuangKong",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemZBChaijie = CCMenuItemFont::create(
			"TAG_ZBChaijie",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//
		CCMenuItemFont* itemZBXiangBaoShi = CCMenuItemFont::create(
			"TAG_ZBXiangBaoshi",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//
		CCMenuItemFont* itemZBCaiBaoshi = CCMenuItemFont::create(
			"TAG_ZBChaiBaoshi",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));
		CCMenuItemFont* itemBaoshiHecheng = CCMenuItemFont::create(
			"TAG_BaoshiHecheng",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//

		CCMenuItemFont* itemGetMarketInfor = CCMenuItemFont::create(
			"TAG_GetMarketInfor",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//
		CCMenuItemFont* itemByItemInMarket = CCMenuItemFont::create(
			"TAG_ByItemInMarket",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//
		CCMenuItemFont* itemGetTaskList = CCMenuItemFont::create(
			"TAG_GetTaskList",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//
		CCMenuItemFont* itemGenScript = CCMenuItemFont::create(
			"TAG_GenScript",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//
		CCMenuItemFont* itemGenScriptBNM = CCMenuItemFont::create(
			"TAG_GenScript_BossNoMove",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//

		CCMenuItemFont* itemGenScript_Team = CCMenuItemFont::create(
			"TAG_GenScript_Team",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//

		CCMenuItemFont* itemFightEnd = CCMenuItemFont::create(////////////////////////////////
			"Tag_FIGHT_SETTLEMENT_LAYER",
			this, SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemLevUp = CCMenuItemFont::create(////////////////////////////////
			"Level_ UP",
			this, SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemGetTask = CCMenuItemFont::create(
			"TAG_GetTask",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//

		CCMenuItemFont* itemGetChessBoard = CCMenuItemFont::create(
			"TAG_GetChessBoard",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//

		CCMenuItemFont* itemBattel = CCMenuItemFont::create(
			"TAG_Battel",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//


		CCMenuItemFont* itemSub = CCMenuItemFont::create(
			"TAG_SubTask",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//

		CCMenuItemFont* itemTiLi = CCMenuItemFont::create(
			"TAG_TiLi",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));//

		CCMenuItemFont* itemZh = CCMenuItemFont::create(
			"itemZh",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemLFString = CCMenuItemFont::create(
			"itemLFString",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));


		CCMenuItemFont* itemUTF8 = CCMenuItemFont::create(
			"itemUTF8",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemArenaEnter = CCMenuItemFont::create(
			"itemArenaEnter",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemTeamBossEnter = CCMenuItemFont::create(
			"itemTeamBossEnter",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemGetGift = CCMenuItemFont::create(
			"itemGetGift",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		CCMenuItemFont* itemEquipPurify = CCMenuItemFont::create(
			"EquipPurify",
			this,SEL_MenuHandler(&TestCmdLayer::testClicked));

		float posX = mWinSize.width/2;
		float posY = mWinSize.height - 100;
		float dis = 60;


		itemGenScript->setTag(TAG_GenScript);
		items->addObject(itemGenScript);
		itemGenScript->setPosition(ccp(posX,posY)); posY -= dis;

		itemGenScriptBNM->setTag(TAG_GenScript_BOOSNOMOVE);
		items->addObject(itemGenScriptBNM);
		itemGenScriptBNM->setPosition(ccp(posX,posY)); posY -= dis;

		itemGenScript_Team->setTag(TAG_GenScript_Team);
		items->addObject(itemGenScript_Team);
		itemGenScript_Team->setPosition(ccp(posX,posY)); posY -= dis;

#define MENU_ITEM_ADD(ITEM,TAG) \
	ITEM->setTag(TAG); \
	items->addObject(ITEM); \
	ITEM->setPosition(ccp(posX,posY)); posY -= dis;

		//
		MENU_ITEM_ADD(itemMEMGUARD,TAG_MEMGUARD);

		MENU_ITEM_ADD(itemZhuangbeiScanner,TAG_ZhuangbeiScanner);
		MENU_ITEM_ADD(itemGuideScriptMaker,TAG_GuideScriptMaker);
		MENU_ITEM_ADD(itemQueryRateTask,TAG_queryRateTask);
		MENU_ITEM_ADD(itemGetRateTaskBonus,TAG_getRateTaskBonus);
		MENU_ITEM_ADD(itemgetCityGuardCurrentStat,TAG_getCityGuardCurrentStat);
		MENU_ITEM_ADD(itemcityGuardBattl,TAG_cityGuardBattle);
		MENU_ITEM_ADD(itemcityGuardBattleSkipCD,TAG_cityGuardBattleSkipCD);
		MENU_ITEM_ADD(itemgetCityGuardLastDayReport,TAG_getCityGuardLastDayReport);
		MENU_ITEM_ADD(itemgetMyCityGuardReward,TAG_getMyCityGuardReward);
		MENU_ITEM_ADD(itemAutoReleasePool,TAG_AutoReleasePool);
		MENU_ITEM_ADD(itemGetZhuangbeiAlbum,TAG_GetZhuangbeiAlbum);
		MENU_ITEM_ADD(itemChurchLevelUp,TAG_ChurchLevelUp);
		MENU_ITEM_ADD(itemSkillLevelUp,TAG_SkillLevelUp);
		MENU_ITEM_ADD(itemQueryYuanshenOfHero,TAG_QueryYuanshenOfHero);
		MENU_ITEM_ADD(itemHellTaskBattle,TAG_HellTaskBattle);
		MENU_ITEM_ADD(itemQueryHellTaskHeroInfo,TAG_QueryHellTaskHeroInfo);
		MENU_ITEM_ADD(itemHeroUpdate,TAG_HeroUpdate);
		MENU_ITEM_ADD(itemHeroUpdateStage,TAG_HeroUpdateStage);
		MENU_ITEM_ADD(itemQueryZahuopu,TAG_queryZahuopu);
		MENU_ITEM_ADD(itemRefreshZahuopu,TAG_refreshZahuopu);
		MENU_ITEM_ADD(itemZahuopuGetOneItem,TAG_zahuopuGetOneItem);
		MENU_ITEM_ADD(itemQueryArenaTeamBasicInfo,TAG_QueryArenaTeamBasicInfo);
		MENU_ITEM_ADD(itemShenbingZhuankeJingwen,TAG_ShenbingZhuankeJingwen);
		MENU_ITEM_ADD(itemQueryArenaMarketRsp,TAG_QueryArenaMarketRsp);
		MENU_ITEM_ADD(itemRefreshArenaChlgList,TAG_RefreshArenaChlgList);
		MENU_ITEM_ADD(itemQueryArenaChlgList,TAG_QueryArenaChlgListRsp);
		MENU_ITEM_ADD(itemArenaChallenge,TAG_ArenaChallengeRsp);
		MENU_ITEM_ADD(ItemArenaMarketBuy,TAG_ArenaMarketBuyRsp);
		MENU_ITEM_ADD(itemArenaBuyChlgTime,TAG_ArenaBuyChlgTimeRsp);
		MENU_ITEM_ADD(itemJieFengYing,TAG_JieFengYing);
		MENU_ITEM_ADD(itemShenbingJianding,TAG_ShenbingJianding);
		MENU_ITEM_ADD(itemZBJinglian,TAG_ZBJinglian);
		MENU_ITEM_ADD(itemSaoDang,TAG_SaoDang);
		MENU_ITEM_ADD(itemSuiPianManager_TEST,TAG_SuiPianManager_TEST);
		MENU_ITEM_ADD(itemHeChengItemSuiPian,TAG_HeChengItemSuiPian);
		MENU_ITEM_ADD(itemHeChengSHenBinSuiPian,TAG_HeChengSHenBinSuiPian);
		MENU_ITEM_ADD(itemCloseNetwork,TAG_CLOSE_NETWORK);
		MENU_ITEM_ADD(itemQuerySubTaskList,TAG_querySubTaskList);
		MENU_ITEM_ADD(itemFBDiaoLuo_1,TAG_FBDiaoLuo_1);
		MENU_ITEM_ADD(itemFBDiaoLuo_2,TAG_FBDiaoLuo_2);
		MENU_ITEM_ADD(itemQueryAllSuitName,TAG_QueryAllSuitName);
		MENU_ITEM_ADD(itemQueryAllMofaJinghua,TAG_QueryAllMofaJinghua);
		MENU_ITEM_ADD(itemZBCuiqu,TAG_ZBCuiqu);
		MENU_ITEM_ADD(itemZBFumo,TAG_ZBFumo);
		MENU_ITEM_ADD(itemQianghuaNew,TAG_ZBQianghuaNew);
		MENU_ITEM_ADD(itemQianghuaHuiTui,TAG_ZBQianghuaHuiTui);
		//MENU_ITEM_ADD(itemZBFumo,TAG_ZBFumo);
		MENU_ITEM_ADD(itemDuanzaoListQuery,TAG_DuanzaoListQuery);
		MENU_ITEM_ADD(itemDuanzaoZBGroupQuery,TAG_DuanzaoZBGroupAuery);
		MENU_ITEM_ADD(itemDuanzao,TAG_Duanzao);
		MENU_ITEM_ADD(itemDuanzao10,TAG_Duanzao_10);
		MENU_ITEM_ADD(itemDelayMusic,TAG_DelayMusic);
		MENU_ITEM_ADD(itemchargeVitality,TAG_chargeVitality);
		MENU_ITEM_ADD(itemsystemGiftQuery,TAG_systemGiftQuery);
		MENU_ITEM_ADD(itemsystemGiftGetGift,TAG_systemGiftGetGift);

		MENU_ITEM_ADD(itemEquipPurify,TAG_EquipPurify);


		itemTopArenaEnter->setTag(TAG_topArenaEnter);
		items->addObject(itemTopArenaEnter);
		itemTopArenaEnter->setPosition(ccp(posX,posY)); posY -= dis;

		itemTopArenaGuess->setTag(TAG_topArenaGuess);
		items->addObject(itemTopArenaGuess);
		itemTopArenaGuess->setPosition(ccp(posX,posY)); posY -= dis;

		itemTopArenaWatchTV->setTag(TAG_topArenaWatchTV);
		items->addObject(itemTopArenaWatchTV);
		itemTopArenaWatchTV->setPosition(ccp(posX,posY)); posY -= dis;

		itemTopArenaGetGift->setTag(TAG_topArenaGetGift);
		items->addObject(itemTopArenaGetGift);
		itemTopArenaGetGift->setPosition(ccp(posX,posY)); posY -= dis;

		itemTopHeroGetList->setTag(TAG_topHeroGetList);
		items->addObject(itemTopHeroGetList);
		itemTopHeroGetList->setPosition(ccp(posX,posY)); posY -= dis;

		itemTopHeroChallenge->setTag(TAG_topHeroChallenge);
		items->addObject(itemTopHeroChallenge);
		itemTopHeroChallenge->setPosition(ccp(posX,posY)); posY -= dis;

		itemCityGuardEnter->setTag(TAG_CityGuardEnter);
		items->addObject(itemCityGuardEnter);
		itemCityGuardEnter->setPosition(ccp(posX,posY)); posY -= dis;

		itemCityGuardGetNextMonster->setTag(TAG_CityGuardGetNextMonster);
		items->addObject(itemCityGuardGetNextMonster);
		itemCityGuardGetNextMonster->setPosition(ccp(posX,posY)); posY -= dis;

		itemCityGuardFight_Yes->setTag(TAG_CityGuardFight_Yes);
		items->addObject(itemCityGuardFight_Yes);
		itemCityGuardFight_Yes->setPosition(ccp(posX,posY)); posY -= dis;

		itemCityGuardFight_No->setTag(TAG_CityGuardFight_NO);
		items->addObject(itemCityGuardFight_No);
		itemCityGuardFight_No->setPosition(ccp(posX,posY)); posY -= dis;

		itemToast->setTag(TAG_TOAST);
		items->addObject(itemToast);
		itemToast->setPosition(ccp(posX,posY)); posY -= dis;

		itemGetItems->setTag(TAG_GetItems);
		items->addObject(itemGetItems);
		itemGetItems->setPosition(ccp(posX,posY)); posY -= dis;

		itemCreateZhuShou->setTag(TAG_CreateZhuShou);
		items->addObject(itemCreateZhuShou);
		itemCreateZhuShou->setPosition(ccp(posX,posY)); posY -= dis;

		itemGetHerosInTeam->setTag(TAG_GetHerosInTeam);
		items->addObject(itemGetHerosInTeam);
		itemGetHerosInTeam->setPosition(ccp(posX,posY)); posY -= dis;

		itemGetZhuangBei->setTag(TAG_GetZhuangBei);
		items->addObject(itemGetZhuangBei);
		itemGetZhuangBei->setPosition(ccp(posX,posY)); posY -= dis;

		itemBuZheng->setTag(TAG_BuZheng);
		items->addObject(itemBuZheng);
		itemBuZheng->setPosition(ccp(posX,posY)); posY -= dis;

		itemZBQiangHua->setTag(TAG_ZBQiangHua);
		items->addObject(itemZBQiangHua);
		itemZBQiangHua->setPosition(ccp(posX,posY)); posY -= dis;

		itemZBZhuankong->setTag(TAG_ZBZhuankong);
		items->addObject(itemZBZhuankong);
		itemZBZhuankong->setPosition(ccp(posX,posY)); posY -= dis;

		itemZBXiangBaoShi->setTag(TAG_ZBXiangBaoshi);
		items->addObject(itemZBXiangBaoShi);
		itemZBXiangBaoShi->setPosition(ccp(posX,posY)); posY -= dis;

		itemZBCaiBaoshi->setTag(TAG_ZBChaiBaoshi);
		items->addObject(itemZBCaiBaoshi);
		itemZBCaiBaoshi->setPosition(ccp(posX,posY)); posY -= dis;

		itemZBChaijie->setTag(TAG_ZBChaijie);
		items->addObject(itemZBChaijie);
		itemZBChaijie->setPosition(ccp(posX,posY)); posY -= dis;

		itemBaoshiHecheng->setTag(TAG_BaoshiHecheng);
		items->addObject(itemBaoshiHecheng);
		itemBaoshiHecheng->setPosition(ccp(posX,posY)); posY -= dis;

		itemGetMarketInfor->setTag(TAG_GetMarketInfor);
		items->addObject(itemGetMarketInfor);
		itemGetMarketInfor->setPosition(ccp(posX,posY)); posY -= dis;

		itemByItemInMarket->setTag(TAG_ByItemInMarket);
		items->addObject(itemByItemInMarket);
		itemByItemInMarket->setPosition(ccp(posX,posY)); posY -= dis;

		itemGetTaskList->setTag(TAG_GetTaskList);
		items->addObject(itemGetTaskList);
		itemGetTaskList->setPosition(ccp(posX,posY)); posY -= dis;

		itemFightEnd->setTag(TAG_FIGHT_END_LAYER);
		items->addObject(itemFightEnd);
		itemFightEnd->setPosition(ccp(posX,posY)); posY -= dis;////////////////

		itemLevUp->setTag(Tag_Level_Up);
		items->addObject(itemLevUp);
		itemLevUp->setPosition(ccp(posX,posY)); posY -= dis;////////////////

		itemGetTask->setTag(TAG_GotTask);
		items->addObject(itemGetTask);
		itemGetTask->setPosition(ccp(posX,posY)); posY -= dis;

		itemGetChessBoard->setTag(TAG_GetChessBoard);
		items->addObject(itemGetChessBoard);
		itemGetChessBoard->setPosition(ccp(posX,posY)); posY -= dis;

		itemBattel->setTag(TAG_Battle);
		items->addObject(itemBattel);
		itemBattel->setPosition(ccp(posX,posY)); posY -= dis;


		itemSub->setTag(TAG_SubmiteTask);
		items->addObject(itemSub);
		itemSub->setPosition(ccp(posX,posY)); posY -= dis;

		itemTiLi->setTag(TAG_TiLi);
		items->addObject(itemTiLi);
		itemTiLi->setPosition(ccp(posX,posY)); posY -= dis;

		itemZh->setTag(TAG_ZH_MSG);
		items->addObject(itemZh);
		itemZh->setPosition(ccp(posX,posY)); posY -= dis;

		itemLFString->setTag(TAG_KEY_ZH_MSG);
		items->addObject(itemLFString);
		itemLFString->setPosition(ccp(posX,posY)); posY -= dis;

		itemUTF8->setTag(TAG_UTF8);
		items->addObject(itemUTF8);
		itemUTF8->setPosition(ccp(posX,posY)); posY -= dis;

		itemArenaEnter->setTag(TAG_ArenaEnter);
		items->addObject(itemArenaEnter);
		itemArenaEnter->setPosition(ccp(posX,posY)); posY -= dis;

		itemTeamBossEnter->setTag(TAG_TeamBossEnter);
		items->addObject(itemTeamBossEnter);
		itemTeamBossEnter->setPosition(ccp(posX,posY)); posY -= dis;

		itemGetGift->setTag(TAG_GetGift);
		items->addObject(itemGetGift);
		itemGetGift->setPosition(ccp(posX,posY)); posY -= dis;

		CCMenu* testMenu = CCMenu::createWithArray(items);
		testMenu->setPosition(CCPointZero);
		addChild(testMenu,10,MENU_TAG);
	}

	CCMenuItemFont* itemBack = CCMenuItemFont::create(
		"Back",
		this,SEL_MenuHandler(&TestCmdLayer::back));//

	CCMenu* menu = CCMenu::createWithItem(itemBack);
	menu->setPosition(CCPointZero);
	itemBack->setPosition(ccp(mWinSize.width - 60,60));
	addChild(menu);

	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_clientLoginRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getItemsInTeamRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_createAssitRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getHerosInTeamRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getZhuangbeiInTeamRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_adjustFormationRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ZBQianghuaRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ZBZhuankongRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ZBChaijieRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ZBXiangBaoshiRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ZBChaiBaoshiRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_BaoshiHechengRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getMarketInfoRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_buyItemInMarketRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getTaskListRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_GetTaskChessBoardRsp);
	//addObserver(SEL_CallFuncO(&TestCmdLayer::battleCallBack),MSG_BattleRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ChangeTaskStatRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_restoreVitalityRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_arenaEnterRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_arenaGetGiftRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_teamBossEnterRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_teamBossGetGiftRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::jingJiBattleCallBack),MSG_arenaGetBattleScriptAndGiftRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::tuanduiBattleCallBack),MSG_teamBossGetBattleScriptAndGiftRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_cityGuardEnterRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_cityGuardGetNextMonsterRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::cityGuardBattleCallBack),MSG_cityGuardFightRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_topHeroGetListRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::topHeroBattleCallBack),MSG_topHeroChallengeRsp_F);

	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_topArenaQueryStatusRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_topArenaGuessRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::topArenaTV_BattleCallback),MSG_topArenaReplay_F);

	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_chargeVitalityRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_systemGiftQueryRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_systemGiftGetGiftRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_DuanzaoListQueryRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_DuanzaoGroupZBQueryRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_DuanzaoRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_QueryAllSuitNameRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ZBCuiquRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ZBFumoRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_QueryAllMofaJinghuaRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ZBQianghuaHuituiRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_QueryFubenDiaoluoRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_QuerySubTaskListRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ShenbingHechengRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_saodangFubenRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_shenbingJieFengyinRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_shenbingJiandingRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_ZBJinglianRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_queryArenaTeamBasicInfoRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_shenbingZhuankeJingwenRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_queryArenaMarketRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_refreshArenaChlgList_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_queryArenaChlgListRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_arenaChallengeRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_arenaMarketBuyRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_arenaBuyChlgTimeRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_queryZahuopuRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_refreshZahuopuRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_zahuopuGetOneItemRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_heroLevelUpRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_queryHellTaskHeroInfoRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_hellTaskBattleRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_queryYuanshenOfHeroRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_churchLevelUpRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_skillLevelUpRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getZhuangbeiAlbumRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getCityGuardCurrentStatRsp);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_cityGuardBattleRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getCityGuardLastDayReportRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getMyCityGuardRewardRsp);

	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_queryRateTaskRsp_F);
	addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_getRateTaskBonusRsp);


	//20140610 以后每一个测试就从这里往后写
	CCMenu* testMenu = (CCMenu*)this->getChildByTag(MENU_TAG);
	float posX = mWinSize.width/2;
	float posY = mWinSize.height;
	float dis = 60;
	//查询邮件
	CCMenuItemFont *emailQuery = CCMenuItemFont::create("TAG_EmailQuery", this, menu_selector(TestCmdLayer::testClicked));
	testMenu->addChild(emailQuery, 1, TAG_Email_Query);
	emailQuery->setPosition(ccp(posX, posY));
	posY += dis;
	//发送邮件
	//查询礼包
	CCMenuItemFont *querygiftBag = CCMenuItemFont::create("TAG_QueryGiftBag", this, menu_selector(TestCmdLayer::testClicked));
	testMenu->addChild(querygiftBag, 1, TAG_QueryGiftBag);
	querygiftBag->setPosition(ccp(posX, posY));
	posY += dis;
	//领取礼包
	CCMenuItemFont *getgiftBag = CCMenuItemFont::create("TAG_GetGiftBag", this, menu_selector(TestCmdLayer::testClicked));
	testMenu->addChild(getgiftBag, 1, TAG_GetGiftBag);
	getgiftBag->setPosition(ccp(posX, posY));
	posY += dis;

}

void TestCmdLayer::back(CCObject* obj)
{
	CCDirector::sharedDirector()->popScene();
}

void TestCmdLayer::testClicked(CCObject* obj) 
{
	unsigned int myId = 1;
	//unsigned long long zhuangBeiId = 1; 
	unsigned long long zhuangBeiId = 8;
	//unsigned long long zhuangBeiId = 26;
	//unsigned long long zhuangBeiId = 28;
	//unsigned long long zhuangBeiId = 29;
	CCMenuItem* item = (CCMenuItem*)obj;
	int tag = item->getTag();
	if (item->getTag() == TAG_Login)
	{
	}
	else if (item->getTag() == TAG_MEMGUARD)
	{
// 		EquipData data;
// 		Equipment* temp = new Equipment(data);
// 		OMemGuard<Equipment> auto_release(temp);
// 		temp = new Equipment(data);
// 		OMemGuard<Equipment> auto_release2(temp);
	}
	else if (item->getTag() == TAG_ZhuangbeiScanner)
	{
		ZhuangbeiScanner::getScanner()->getTestReport();
	}
	else if (item->getTag() == TAG_GuideScriptMaker)
	{
		GuideScriptMaker maker;
		//maker.testGenAll();
		maker.testLoadAll();
	}
	else if (item->getTag() == TAG_queryRateTask)
	{
		mCmdHlp->cmdQueryTaskRate();
		mCmdHlp->cmdQueryRateTaskBonus(RateStarType_Task,1);
		mCmdHlp->cmdQueryRateTaskBonus(RateStarType_Chapter,1);
	}
	else if (item->getTag() == TAG_getRateTaskBonus)
	{
		mCmdHlp->cmdGetTaskRateBonus(RateStarType_Task, RateStarLevel_Gold, 1);
		mCmdHlp->cmdGetTaskRateBonus(RateStarType_Chapter, RateStarLevel_Cuprum, 1);
	}
	else if (item->getTag() == TAG_getCityGuardCurrentStat)
	{
		mCmdHlp->cmdGetCityGuardCurrentStat();
	}
	else if (item->getTag() == TAG_cityGuardBattle)
	{
		mCmdHlp->cmdCityGuardBattle();
	}
	else if (item->getTag() == TAG_cityGuardBattleSkipCD)
	{
		//mCmdHlp->cmdCityGuardBattleSkipCD();
	}
	else if (item->getTag() == TAG_getCityGuardLastDayReport)
	{
		mCmdHlp->cmdGetCityGuardLastDayReport();
	}
	else if (item->getTag() == TAG_getMyCityGuardReward)
	{
		mCmdHlp->cmdGetMyCityGuardReward();
	}
	else if (item->getTag() == TAG_AutoReleasePool)
	{
		EquipData data;
		Equipment* temp1 = new Equipment(data);
		Equipment* temp2 = new Equipment(data);
		Equipment* temp3 = new Equipment(data);
		Equipment* temp4 = new Equipment(data);

		BaseActorProperty p;
		BaseActor* actor = new BaseActor(p);
		actor->retain();
		actor->release();
		actor->autorelease();


		LF_AUTORELEASE(temp1);
		LF_AUTORELEASE(temp2);
		LF_AUTORELEASE(temp3);
		LF_AUTORELEASE(temp4);

		LF_RETAIN(temp1); 

		LFAutoReleasePool::getPool();

		int a = 0;
	}
	else if (item->getTag() == TAG_GetZhuangbeiAlbum)
	{
		mCmdHlp->cmdGetZhuangbeiAlbum(Profession_Yemanren);
	}
	else if (item->getTag() == TAG_ChurchLevelUp)
	{
		mCmdHlp->cmdChurchLevelUp();
	}
	else if (item->getTag() == TAG_SkillLevelUp)
	{
		SkillData* skill = MyselfManager::getManager()->getMyZhuJueData()->getCurrentUseSkill();
		if (skill != NULL)
		{
			mCmdHlp->cmdSkillLevelUp(skill->getInfor()->owner_hero_id,skill->getInfor()->skill_id);
		}
	}
	else if (item->getTag() == TAG_QueryYuanshenOfHero)
	{
		mCmdHlp->cmdQueryYuanshenOfHero(MyselfManager::getManager()->getMyZhuJueData()->getId());
	}
	else if (item->getTag() == TAG_QueryHellTaskHeroInfo)
	{
		mCmdHlp->cmdQueryHellTaskHeroInfo();
	}
	else if (item->getTag() == TAG_HellTaskBattle)
	{
		mCmdHlp->cmdHellTaskBattle(HellTaskId_Min);
	}
	else if (item->getTag() == TAG_HeroUpdate)
	{
		unsigned int id = MyselfManager::getManager()->getMyZhuJueData()->getId();
		mCmdHlp->cmdHeroLevelUp(id);
	}else if (item->getTag() == TAG_HeroUpdateStage)
	{
		unsigned int id = MyselfManager::getManager()->getMyZhuJueData()->getId();
		mCmdHlp->cmdHeroUpgrade(id);
	}
	else if (item->getTag() == TAG_queryZahuopu)
	{
		mCmdHlp->cmdQueryZahuopu();
	}
	else if (item->getTag() == TAG_refreshZahuopu)
	{
		mCmdHlp->cmdRefreshZahuopu();
	}
	else if (item->getTag() == TAG_zahuopuGetOneItem)
	{
		mCmdHlp->cmdZahuopuGetOneItem();
	}
	else if (item->getTag() == TAG_QueryArenaTeamBasicInfo)
	{
		mCmdHlp->cmdQueryArenaTeamBasicInfo();
	}
	else if (item->getTag() == TAG_QueryArenaMarketRsp)
	{
		mCmdHlp->cmdQueryArenaMarket();
	}
	else if (item->getTag() == TAG_RefreshArenaChlgList)
	{
		mCmdHlp->cmdRefreshArenaChlgList();
	}
	else if (item->getTag() == TAG_QueryArenaChlgListRsp)
	{
		mCmdHlp->cmdQueryArenaChlgList();
	}
	else if (item->getTag() == TAG_ArenaChallengeRsp)
	{
		mCmdHlp->cmdArenaChallenge(93);
	}
	else if (item->getTag() == TAG_ArenaMarketBuyRsp)
	{
		mCmdHlp->cmdArenaMarketBuy(ItemsType_Shenbing_Jianding,1);
	}
	else if (item->getTag() == TAG_ArenaBuyChlgTimeRsp)
	{
		mCmdHlp->cmdArenaBuyChallengeTime(true);
	}


	else if (item->getTag() == TAG_JieFengYing)
	{
		mCmdHlp->cmdShenbingJieFengyin(3245);
	}
	else if (item->getTag() == TAG_ShenbingJianding)
	{
		mCmdHlp->cmdShenbingJianding(2688);
	}
	else if (item->getTag() == TAG_ZBJinglian)
	{
		mCmdHlp->cmdZBJinglian(3247,3269);
		mCmdHlp->cmdZBJinglian(3247,3270);
		mCmdHlp->cmdZBJinglian(3247,3271);
		mCmdHlp->cmdZBJinglian(3247,3272);
		mCmdHlp->cmdZBJinglian(3247,3273);
	}
	else if (item->getTag() == TAG_SaoDang)
	{
		SPCmd_Saodang req;
		req.task_id = 6;
		req.saodang_time = 1;
		req.auto_fenjie_baizhuang = false;
		req.auto_fenjie_lanzhuang = false;
		req.auto_fenjie_huangzhuang = false;

		mCmdHlp->cmdSaodangFuben(req);
	}
	else if (item->getTag() == TAG_SuiPianManager_TEST)
	{

		mCmdHlp->cmdShenbingHecheng(1391);
		//mCmdHlp->cmdShenbingHecheng(1412);

		SuiPianManager::getManager()->getAllItemSuiPian();
		SuiPianManager::getManager()->getAllShenBingSuiPian();
		SuiPianManager::getManager()->getAllItemSuiPianWithGrouped();
		SuiPianManager::getManager()->getAllShenbingSuiPianWithGrouped();

		ItemSuipianInfo item;
		item.item_id = 8989;
		item.suipian_count = 1;
		item.suipian_id = 1;
		SuiPianManager::getManager()->addOneItemSuiPian(item);
		SuiPianManager::getManager()->subOneItemSuiPian(item);

		ShenbingSuipianInfo itemShenBing;
		itemShenBing.zb_group_id = 1414;
		itemShenBing.suipian_count = 1;
		itemShenBing.suipian_id = 1;
		SuiPianManager::getManager()->addOneShenBingSuiPian(itemShenBing);
		SuiPianManager::getManager()->subOneShenBingSuiPian(itemShenBing);

		SuiPianManager::getManager()->addItemSuiPianNumByItemType(ItemsType_Baoshi_hong_1,10);
		SuiPianManager::getManager()->addItemSuiPianNumByItemType(ItemsType_Baoshi_hong_1,-10);

		SuiPianManager::getManager()->addShenBingSuiPianNumByZBGroupId(1414,10);
		SuiPianManager::getManager()->addShenBingSuiPianNumByZBGroupId(1414,-10);

	}
	if (item->getTag() == TAG_HeChengItemSuiPian)
	{
		//cmd
	}else 
	if (item->getTag() == TAG_HeChengSHenBinSuiPian)
	{

	}
	else if (item->getTag() == TAG_CLOSE_NETWORK)
	{
		mCmdHlp->forceCloseNetwork();
	}
	else if (item->getTag() == TAG_querySubTaskList)
	{
		mCmdHlp->cmdQuerySubTaskList();
	}
	else if (item->getTag() == TAG_FBDiaoLuo_1)
	{
		mCmdHlp->cmdQueryFubenDiaoluo(1+5);
	}
	else if (item->getTag() == TAG_FBDiaoLuo_2)
	{
		mCmdHlp->cmdQueryFubenDiaoluo(2+5);
	}
	else if (item->getTag() == TAG_QueryAllSuitName)
	{
		mCmdHlp->cmdQueryAllSuitName();
	}
	else if (item->getTag() == TAG_ZBCuiqu)
	{
		SPCmd_ZBCuiqu req;
		req.attr_type = 6;
		req.attr_value = 7;
		req.is_fumo_attr = true;
		req.zhuangbei_id = 3;
		mCmdHlp->cmdZBCuiqu(req);
	}
	else if (item->getTag() == TAG_ZBQianghuaNew)
	{
		SPCmd_ZBQianghua req;
		req.zhuangbei_id = 2;
		req.b_gold_star = false;
		mCmdHlp->cmdZBQianghua(req);
	}
	else if (item->getTag() == TAG_ZBQianghuaHuiTui)
	{
		unsigned long long _zID = 2;
		vector<unsigned char> keep;
		keep.push_back(3);
		mCmdHlp->cmdZBQianghuaHuitui(_zID,keep);
	}
	else if (item->getTag() == TAG_ZBFumo)
	{
		vector<MofaJinghua> all =  MofaJinghuaManager::getManager()->getAllMoFaJingHua();
		SPCmd_ZBFumo req;
		if (all.size() > 0)
			req.jinghua = all[0];
		else
			return;

		req.jinghua.attr_type = 6;
		req.jinghua.zb_type = 5;
		req.jinghua.level = 1;

		req.zhuangbei_id = 3;
		mCmdHlp->cmdZBFumo(req);
	}
	else if (item->getTag() == TAG_QueryAllMofaJinghua)
	{
		mCmdHlp->cmdQueryAllMofaJinghua();
	}

	else if (item->getTag() == TAG_DuanzaoListQuery)
	{
		mCmdHlp->cmdDuanzaoListQuery();
	}
	else if (item->getTag() == TAG_DuanzaoZBGroupAuery)
	{
		int num = DuanZaoManager::getManager()->getDuanzaoGroupList().size();
		if (num > 0)
		{
			mCmdHlp->cmdDuanzaoGroupZBQuery(DuanZaoManager::getManager()->getDuanzaoGroupList()[num*CCRANDOM_0_1()].duanzao_group_id);
		}
	}
	else if (item->getTag() == TAG_Duanzao)
	{
		mCmdHlp->cmdDuanzao(false,1);
	}
	else if (item->getTag() == TAG_Duanzao_10)
	{
		mCmdHlp->cmdDuanzao(true,1);
	}
	else if (item->getTag() == TAG_DelayMusic)
	{
		runAction(LFDelayToMusic::create(0.5f,"diaoZB_cq.wav"));
	}
	else if (item->getTag() == TAG_chargeVitality)
	{
		mCmdHlp->cmdchargeVitality();
	}
	else if (item->getTag() == TAG_systemGiftQuery)
	{
		mCmdHlp->cmdSystemGiftQuery();
	}
	else if (item->getTag() == TAG_systemGiftGetGift)
	{
		mCmdHlp->cmdSystemGiftGetGift(1);
	}
	else if (item->getTag() == TAG_topArenaEnter)
	{
		mCmdHlp->cmdTopArenaQueryStatus();
	}
	else if (item->getTag() == TAG_topArenaGuess)
	{
		//bool guess[5] = {false};
		//mCmdHlp->cmdTopArenaGuess(guess);
	}
	else if (item->getTag() == TAG_topArenaWatchTV)
	{
		//mCmdHlp->cmdTopArenaReplay(1);
	}
	else if (item->getTag() == TAG_topArenaGetGift)
	{
		//mCmdHlp->cmdTopArenaGetGift();
	}

	else if (item->getTag() == TAG_topHeroGetList)
	{
		mCmdHlp->cmdTopHeroGetList();
	}
	else if (item->getTag() == TAG_topHeroChallenge)
	{
		mCmdHlp->cmdTopHeroChallenge(1);
	}
	else if (item->getTag() == TAG_CityGuardEnter)
	{
		mCmdHlp->cmdCityGuardEnter();
	}
	else if (item->getTag() == TAG_CityGuardGetNextMonster)
	{
		mCmdHlp->cmdCityGuardGetNextMonster();
	}
	else if (item->getTag() == TAG_CityGuardFight_Yes)
	{
		mCmdHlp->cmdCityGuardFight(true);
	}
	else if (item->getTag() == TAG_CityGuardFight_NO)
	{
		mCmdHlp->cmdCityGuardFight(false);
	}
	else if (item->getTag() == TAG_GetItems)
	{
		mCmdHlp->cmdGetItemsInTeam();
	}
	else if (item->getTag() == TAG_CreateZhuShou)
	{
		// 我带一个技师
		mCmdHlp->cmdCreateAssit(myId,Profession_MiShu);
	}
	else if (item->getTag() == TAG_GetHerosInTeam)
	{
		mCmdHlp->cmdGetHerosInTeam(myId);
	}
	else if (item->getTag() == TAG_GetZhuangBei)
	{
		mCmdHlp->cmdGetZhuangbeiInTeam();
	}
	else if (item->getTag() == TAG_BuZheng)
	{
		// 布阵测试
		unsigned int attayData[] = {1,3,4,6};
		mCmdHlp->cmdAdjustFormation(attayData);
	}
	else if (item->getTag() == TAG_ZBQiangHua)
	{
		SPCmd_ZBQianghua req;
		req.zhuangbei_id = zhuangBeiId;
		req.b_gold_star = false;
		//req.gailv_cailiao_count = 0;	// 概率材料

		// 检测
		{
			WuPinManager* w_manager = WuPinManager::getManager();
			Equipment* item = EquipmentManager::getManager()->getOneEquipment(req.zhuangbei_id);
			CCAssert(item!=NULL,"");

			// 是否可以被强化
			if ( !CS::canZBTypeBeQianghua(item->getType()))
			{
				CCMessageBox("This Equipment can not be Qianghua","");
				return;
			}

			if ( item->getEquipData().qianghua_data.qianghua_count >= item->getEquipData().qianghua_data.max_qianghua_time)
			{
				CCLog("qianghua_count >= max_qianghua_time");
				return;
			}
			
			unsigned int normalCaiLiaoNum = 0;
			unsigned int xiYouCaiLiaoNum = 0;
			unsigned int chuanQiCaiLiaoNum = 0;
			unsigned int yinBi = 0;

			yinBi = CS::QianghuaZhuangbeiNeedSilver(item->getLevel(),item->getEquipColor());
			normalCaiLiaoNum = CS::ItemsNeedForQianghuaZhuangbei(item->getLevel(),item->getEquipColor());

			CCLog("xuyao yinBi = %d,CaiLiao : normalCaiLiaoNum = %d,xiYouCaiLiaoNum = %d,chuanQiCaiLiaoNum = %d",
				yinBi,normalCaiLiaoNum,xiYouCaiLiaoNum,chuanQiCaiLiaoNum);


			WuPin* temp = NULL;

			temp = w_manager->getWuPin(ItemsType_Silver);
			if ( (temp == NULL || temp->getCount() < yinBi) && yinBi > 0)
			{
				CCMessageBox("your money is too less","");
				return;
			}

			temp = w_manager->getWuPin(ItemsType_QianghuaNormal);
			if ( (temp == NULL || temp->getCount() < normalCaiLiaoNum) && normalCaiLiaoNum > 0)
			{
				CCMessageBox("normal - CaiLiaoNum too less","");
				return;
			}

			temp = w_manager->getWuPin(ItemsType_QianghuaXiyou);
			if ( (temp == NULL || temp->getCount() < xiYouCaiLiaoNum) && xiYouCaiLiaoNum > 0)
			{
				CCMessageBox("xiYou - CaiLiaoNum too less","");
				return;
			}

			temp = w_manager->getWuPin(ItemsType_QianghuaChuangqi);
			if ( (temp == NULL || temp->getCount() < chuanQiCaiLiaoNum) && chuanQiCaiLiaoNum > 0)
			{
				CCMessageBox("chuanQi - CaiLiaoNum too less","");
				return;
			}

			/*temp = w_manager->getWuPin(ItemsType_QianghuaGailvTianjia);
			if ( (temp == NULL || temp->getCount() < req.gailv_cailiao_count) && req.gailv_cailiao_count > 0)
			{
				CCMessageBox("gaiLv - too less","");
				return;
			}*/
		}

		mCmdHlp->cmdZBQianghua(req);
	}
	else if (item->getTag() == TAG_ZBZhuankong)
	{
		/*SPCmd_ZBZhuankong req;
		req.zhuangbei_id = zhuangBeiId;

		Equipment* item = EquipmentManager::getManager()->getOneEquipment(req.zhuangbei_id);
		CCAssert(item!=NULL,"");
		unsigned int jinBi = CS::ZhuangkongNeedGold(item->getLevel(),item->getEquipColor());

		CCLog("ZhuangkongNeedGold --> %d",jinBi);

		if(!CS::canZBTypeBeZhuangkong(item->getType(),item->getEquipData().hole))
		{
			CCMessageBox("Can not Make Hole on it!","");
			return;
		}


		WuPinManager* w_manager = WuPinManager::getManager();
		WuPin* temp = NULL;

		temp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		if ( (temp == NULL || temp->getCount() < jinBi) && jinBi > 0)
		{
			CCMessageBox("your money is too less","");
			return;
		}

		mCmdHlp->cmdZBZhuankong(req);*/
	}
	else if (item->getTag() == TAG_ZBChaijie)
	{
		vector<long long> equips;
		equips.push_back(zhuangBeiId);

		Equipment* item = EquipmentManager::getManager()->getOneEquipment(zhuangBeiId);
		CCAssert(item!=NULL,"");
		unsigned int yinbi = CS::ChaijieZhuangbeiNeedSilver(item->getLevel(),item->getEquipColor());

		WuPinManager* w_manager = WuPinManager::getManager();
		WuPin* temp = NULL;

		temp = w_manager->getWuPin(ItemsType_Silver);
		if ( (temp == NULL || temp->getCount() < yinbi) && yinbi > 0)
		{
			CCMessageBox("your money is too less","");
			return;
		}

		mCmdHlp->cmdZBChaijie(equips);
	}
	else if (item->getTag() == TAG_ZBXiangBaoshi)
	{
		SPCmd_ZBXiangBaoshi req;
		req.zhuangbei_id = zhuangBeiId;
		req.baoshi_type = ItemsType_Baoshi_lv_3;

		Equipment* item = EquipmentManager::getManager()->getOneEquipment(req.zhuangbei_id);
		CCAssert(item!=NULL,"");
		unsigned int yinbi = CS::XiangbaoshiNeedSilver(item->getLevel(),req.baoshi_type);

		if( item->getEquipData().hole <= item->getBaoShiNum() )
		{
			CCMessageBox("This Equip have No more Hole!","");
			return;
		}

		WuPinManager* w_manager = WuPinManager::getManager();
		WuPin* temp = NULL;

		temp = w_manager->getWuPin(ItemsType_Silver);
		if ( (temp == NULL || temp->getCount() < yinbi) && yinbi > 0)
		{
			CCMessageBox("your money is too less","");
			return;
		}

		temp = w_manager->getWuPin((ItemsType) req.baoshi_type);
		if ( (temp == NULL || temp->getCount() < 1))
		{
			CCMessageBox("your have no this bao shi","");
			return;
		}

		mCmdHlp->cmdZBXiangBaoshi(req);
	}
	else if (item->getTag() == TAG_ZBChaiBaoshi)
	{
		SPCmd_ZBChaiBaoshi req;
		req.zhuangbei_id = zhuangBeiId;


		Equipment* item = EquipmentManager::getManager()->getOneEquipment(zhuangBeiId);
		if (item->getBaoShiNum() <= 0)
		{
			CCMessageBox("Bao shi num <= 0","");
			return;
		}

		EquipData data =item->getEquipData();
		unsigned int yinBi = 0;
		if (data.diamond_type_1 != 0)
			yinBi += CS::XiangbaoshiNeedSilver(data.zhuangbei_level,data.diamond_type_1);
		if (data.diamond_type_2 != 0)
			yinBi += CS::XiangbaoshiNeedSilver(data.zhuangbei_level,data.diamond_type_2);
		if (data.diamond_type_3 != 0)
			yinBi += CS::XiangbaoshiNeedSilver(data.zhuangbei_level,data.diamond_type_3);

		WuPin* temp = NULL;

		temp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
		if ( (temp == NULL || temp->getCount() < yinBi) && yinBi > 0 )
		{
			CCMessageBox("your money is too less","");
			return;
		}

		mCmdHlp->cmdZBChaiBaoshi(req);
	}
	else if (item->getTag() == TAG_BaoshiHecheng)
	{
		SPCmd_BaoshiHecheng req;
		req.baoshi_type = ItemsType_Baoshi_hong_4;

		// 能否合成
		if (!CS::canBaoShiBeHeCheng(req.baoshi_type))
		{
			CCMessageBox("canBaoShiBeHeCheng = false","");
			return;
		}

		ItemsType needBaoShiType = (ItemsType)CS::HechengBaoshiNeedBaoshiType(req.baoshi_type);
		unsigned int needBaoShiNum = CS::HechengBaoshiNeedBaoshiCount(req.baoshi_type);
		unsigned int needGoldNum = CS::HechengBaoshiNeedGold(req.baoshi_type);

		WuPin* temp = NULL;
		
		temp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		if ( (temp == NULL || temp->getCount() < needGoldNum) && needGoldNum > 0 )
		{
			CCMessageBox("Gold Too Less","");
			return;
		}

		temp = WuPinManager::getManager()->getWuPin(needBaoShiType);
		if ( (temp == NULL || temp->getCount() < needBaoShiNum) && needBaoShiNum > 0 )
		{
			CCMessageBox("needed BaoShi Too Less","");
			return;
		}

		mCmdHlp->cmdBaoshiHecheng(req);
	}
	else if (item->getTag() == TAG_GetMarketInfor)
	{
		mCmdHlp->cmdGetMarketInfo();
	}
	else if (item->getTag() == TAG_ByItemInMarket)
	{
	}
	else if (item->getTag() == TAG_GetTaskList)
	{
		mCmdHlp->cmdGetTaskList(myId);
	}
	else if (item->getTag() == TAG_GenScript || item->getTag() == TAG_GenScript_BOOSNOMOVE)
	{
		testEquipVec();
		testGenScript(item->getTag() == TAG_GenScript_BOOSNOMOVE);
	}
	else if (item->getTag() == TAG_GenScript_Team)
	{
		FightScriptBuilder builder;
		vector<FightScript> scripts;
		
		Profession proA[] = {Profession_Yemanren,Profession_Qishi,Profession_YouXia,Profession_MiShu};
		SkillType skill_A[] = {SkillType_BaoLieZhan,SkillType_JianShou,SkillType_ShunYinTuXi,SkillType_ShaLu};


		TeamHero* teams[4] = {NULL};
		{
			TeamBaseProperty pro;
			pro.leaderProfression = Profession_Yemanren;	
			pro.mLeaderId = 1;							//
			pro.mHP = 100 * 4;							// 生命值,
			pro.mLevel = 5;								// 团队的等级，就用主角等级
			pro.mGongJiLi_MIN = 10*4;					// 攻击力
			pro.mGongJiLi_MAX = 20*4;					// 攻击力
			pro.mGongJiJuLi = 1;						// 这里就是恒定的1，因为根据棋盘的布局，大家都是1
			pro.mBaoJiLv = 30;							// 暴击率
			pro.mBaoJiShangHaiYinZi = 150;				// 暴击的伤害因子，比如150%，就是攻击的150
			pro.mJiNengFaChuLv = 80;					// 技能发出概率
			pro.mJiNengZhouQi = 4;					
			pro.mJiNengTurn = 2;					
			pro.mShangHaiLeiXing = SHANG_HAI_WU_LI;				// 伤害类型,默认是SHANG_HAI_MO_FA
			pro.mShanBiLv = 10;							// 闪避率
			pro.mGeDangLv = 5;							// 格挡率
			pro.mGeDangZhi = 100;						// 格挡值
			pro.mHuJiaZhi = 20*4;						// 护甲值
			pro.mKangXingZhi = 1*4;						// 抗性值
			pro.mDefaultFreezyNum = 4;					// 默认冷却时间

			teams[0] = new TeamHero();
			pro.leaderProfression = proA[0];
			teams[0]->initWithTeamBaseProperty(pro);
			teams[0]->skilVec.push_back(skill_A[0]);

			teams[1] = new TeamHero();
			pro.leaderProfression = proA[1];
			teams[1]->initWithTeamBaseProperty(pro);
			teams[1]->skilVec.push_back(skill_A[1]);

			teams[2] = new TeamHero();
			pro.leaderProfression = proA[2];
			teams[2]->initWithTeamBaseProperty(pro);
			teams[2]->skilVec.push_back(skill_A[2]);

			teams[3] = new TeamHero();
			pro.leaderProfression = proA[3];
			teams[3]->initWithTeamBaseProperty(pro);
			teams[3]->skilVec.push_back(skill_A[3]);
		}

		Guaiwu* boss = new Guaiwu();
		{
			GuaiWuData g_Data;
			g_Data.level = 1;
			g_Data.gongJiLi = 800 + 800*CCRANDOM_0_1();
			g_Data.xueLiang = 2600+ 2*CCRANDOM_0_1();
			g_Data.baoJiLv = 100 + 8*CCRANDOM_0_1();
			g_Data.baoShangYinZi = 120;
			g_Data.jiNengChuFaLv = 100 + 60*CCRANDOM_0_1();
			g_Data.huJiaJianMianLv = 20;
			g_Data.kangXingJianMianLv = 10 + 5*CCRANDOM_0_1();
			g_Data.gongJiJuLi = 1;
			g_Data.shangHaiLeiXing = SHANG_HAI_WU_LI;
			g_Data.profession = Profession_Big_Boss_1;
			boss->initWithGuaiwuData(g_Data);
			boss->setDefaultCdTime(0);
			boss->skilVec.push_back(SkillType_JianYu);
		}

		TeamEndParam endData;
		builder.genTeamFightScript(teams,boss,scripts,endData);
		CCLog("genScript over");

		vector<FightScript> myScriptVec;
		myScriptVec.push_back(scripts[0]);

		int len;
		char* cScript = builder.FightScriptVec_2_CharArr(myScriptVec,len);
		CCLog("msg build over");
		vector<FightScript> A_tempVec = builder.CharArr_2_FightScriptVec(cScript,myScriptVec.size());
		delete [] cScript;
		CCLog("msg parse over");
		// 校验
		{
			int num = A_tempVec.size();
			for (int i=0;i<num;i++)
			{
				if (!myScriptVec[i].isEqual(A_tempVec[i]))
				{
					CCLog("ChooseFightBody::tableCellTouched ----> !A_Script.isEqual(A_Temp) Assert!");
					CCMessageBox("Data InValide!","Error");
					return;
				}
			}
		}

		FightDirector dir;
		vector<FightDirector> dir_vec;
		dir.initScript(A_tempVec[0]);
		dir_vec.push_back(dir);

		Bundle bundle;
		bundle.putBool("testMode",true);

		BaseSprite* beiJing = BaseSprite::create("zhandou_beijing_chengzhenzhoubian.png");
		GameDirector::getDirector()->fight_team(dir_vec,beiJing,bundle);

		for (int i=0;i<4;i++)
		{
			if (teams[i])
			{
				delete teams[i];
			}
		}
		if (boss)
		{
			delete boss;
		}
	}
/////////////////////////////////////////////////////////////////////////////////////
	else if(item->getTag() == Tag_Level_Up)
	{
		LevelUpLayer *layer = LevelUpLayer::create(MyselfManager::getManager()->getMyZhuJueData()->getId());
		if (layer)
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
	}
	else if(item->getTag() == TAG_FIGHT_END_LAYER)
	{
//		int state = 1;
//		g_taskId = 12;
//		g_gridIndex = 5;
//		vector<BaseActor*> actors = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All);
//		vector<ItemInfo> gettedItemVec(1);
//		vector<EquipData> gettedEquipVec;
//		
//
/////////////////////////////////////////////////////////////////////////////
//		Settlement *taskInfo = new Settlement;
//		taskInfo->taskName = "NoName";
//		taskInfo->bossName = "NoName";
//		taskInfo->win = state;
//		//taskInfo->exp = mNowData.exp_current - mBeForedata.exp_current + levelUp*mBeForedata.exp_need;
//		taskInfo->exp = 0;	
//		for (int i=0;i<FormationHeroAcountMax;i++)
//		{
//			taskInfo->oldArmy[i] = actors[i]->getCurrentProperty();
//			taskInfo->curArmy[i] = actors[i]->getCurrentProperty();
//			gettedEquipVec.push_back(actors[i]->getEquipments().at(0)->getEquipData());
//		}
//		taskInfo->oldArmy[0].level -= 1;
//		taskInfo->oldArmy[1].level -= 1;
//		taskInfo->oldArmy[2].level -= 4;
//		taskInfo->oldArmy[3].level -= 1;
//		while (gettedEquipVec.size()>4)
//		{
//			gettedEquipVec.pop_back();
//		}
// 		taskInfo->items = gettedItemVec;
// 		taskInfo->equips = gettedEquipVec;
//
//
//		FightSettlementLayer *retLayer = FightSettlementLayer::create(taskInfo);
//		CCScene *runS = CCDirector::sharedDirector()->getRunningScene();
//		runS->addChild(retLayer, 1025, TAG_FIGHT_END_LAYER);
//
//		delete taskInfo;
	}
	else if (item->getTag() == TAG_GetChessBoard)
	{
		mCmdHlp->cmdGetTaskChessboard(1,false);
	}
	else if (item->getTag() == TAG_ArenaEnter)
	{
		SPCmd_ArenaEnter req;
		req.hardLevel = 0;
		mCmdHlp->cmdArenaEnter(req);
	}
	else if (item->getTag() == TAG_TeamBossEnter)
	{
		SPCmd_TeamBossEnter req;
		req.hardLevel = 0;
		mCmdHlp->cmdTeamBossEnter(req);
	}
	else if (item->getTag() == TAG_GetGift)
	{
		vector<unsigned int> _id_arr;
		int i = 7;
		//mCmdHlp->cmdArenaGetGift(i);
		mCmdHlp->cmdTeamBossGetGift(i);
	}
	else if (item->getTag() == TAG_Battle)
	{
		fight->setTest(true);
		fight->prepareData(1,8);
	}
	else if (item->getTag() == TAG_GotTask)
	{
		mCmdHlp->cmdChangeTaskState(1,TaskStatus_Got);
	}
	else if (item->getTag() == TAG_SubmiteTask)
	{
		mCmdHlp->cmdChangeTaskState(1,TaskStatus_Submitted);
	}
	else if (item->getTag() == TAG_TiLi)
	{
		/////////////////////////////////////////////////////////////////////////////
		//mCmdHlp->cmdRestoreVitality();
		/////////////////////////////////////////////////////////////////////////////
		GetTiLiLayer* layer = GetTiLiLayer::create();
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);

	}
	else if (item->getTag() == TAG_EquipPurify)
	{
		string info;
		vector<Equipment*> eqVec = EquipmentManager::getManager()->getAllEquipment();
		for (int i=0, count=eqVec.size(); i<count; ++i)
		{
			if (eqVec.at(i)->canPurify(info,false))
			{
				addObserver(SEL_CallFuncO(&TestCmdLayer::testCallBack),MSG_PurifyRsp);
				CmdHelper::getHelper()->cmdPurify(eqVec.at(i)->getEquipId(), 1);
				break;
			}
		}
	}
	else if (item->getTag() == TAG_Email_Query)
	{
		CmdHelper::getHelper()->cmdQueryEmail();
		GameDirector::getDirector()->hideWaiting();
	}
	else if (tag == TAG_QueryGiftBag)
	{
		CmdHelper::getHelper()->cmdQuerySystemGiftBag();
		GameDirector::getDirector()->hideWaiting();
	}
	else if (tag == TAG_GetGiftBag)
	{
		if (SystemGiftBagManager::getManager()->getAllGiftBag()->size() > 0)
		{
			CmdHelper::getHelper()->cmdGetSystemGiftBag(SystemGiftBagManager::getManager()->getAllGiftBag()->at(0).id);
			GameDirector::getDirector()->hideWaiting();
		}
	}
	else if (item->getTag() == TAG_TOAST)
	{
		LFToast::makeToast("this is a long long long long long long long long long long long long long long long long long long long long long string!");
	}
	else if (item->getTag() == TAG_ZH_MSG)
	{
		CCMessageBox("内容区","标题");//OK
		//LFMessageBox::alert("内容区","标题");
	}
	else if(item->getTag() == TAG_KEY_ZH_MSG)
	{
		// 乱码
		CCMessageBox(LFStrings::getValue("neirong").c_str(),LFStrings::getValue("biaoti").c_str());
		LFMessageBox::alert(LFStrings::getValue("neirong").c_str(),LFStrings::getValue("biaoti").c_str());
	}else
	{
		//// 乱码
		//string str1 = "标题";
		//LF::GBKToUTF8(str1,"gbk","utf-8");
		//string str2 = "内容";
		//LF::GBKToUTF8(str2,"gbk","utf-8");
		//CCMessageBox(str2.c_str(),str1.c_str());
		//CCLog("CCMessageBox-->str1 = [%s],str2 = [%s]",str1.c_str(),str2.c_str());
		//LFMessageBox::alert(str2.c_str(),str1.c_str());
	}

}
void TestCmdLayer::testCallBack(CCObject* msg_rsp)
{
	//
	GameDirector::getDirector()->hideWaiting();
}

void TestCmdLayer::jingJiBattleCallBack(CCObject* f_msg_rsp)
{
	Flat_ArenaGetBattleScriptGiftRsp* data = (Flat_ArenaGetBattleScriptGiftRsp*) ((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	GameDirector::getDirector()->hideWaiting();

	if(data->err_code != Success)
		return;

	FightDirector dir;
	vector<FightDirector> dir_vec;
	int num = data->scriptVec.size();
	for (int i=0;i<num;i++)
	{
		dir.initScript(data->scriptVec[i]);
		dir_vec.push_back(dir);
	}
	// 直接换scene
	Bundle bundle;
	bundle.putBool("setFightBarVisible",false);
	bundle.putBool("testMode",true);
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec,BaseSprite::create("zhandou_beijing_muyuan.png"),bundle);
	
}

void TestCmdLayer::tuanduiBattleCallBack(CCObject* f_msg_rsp)
{
	Flat_TeamBossGetBattleScriptGiftRsp* data = (Flat_TeamBossGetBattleScriptGiftRsp*) ((Flat_MSG_Rsp*)f_msg_rsp)->getData();

	GameDirector::getDirector()->hideWaiting();
	if(data->err_code != Success)
		return;

	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(data->fightScript);
	dir_vec.push_back(dir);

	Bundle bundle;
	bundle.putBool("testMode",true);
	bundle.putBool("setFightBarVisible",false);
	ClientFightLayer_TF* layer = (ClientFightLayer_TF*)GameDirector::getDirector()->fight_team(dir_vec,BaseSprite::create("zhandou_beijing_muyuan.png"),bundle);
}

void TestCmdLayer::cityGuardBattleCallBack(CCObject* f_msg_rsp)
{
	Flat_CityGuardFightRsp* data = (Flat_CityGuardFightRsp*) ((Flat_MSG_Rsp*)f_msg_rsp)->getData();

	GameDirector::getDirector()->hideWaiting();
	if(data->err_code != Success)
		return;

	if (data->isFighting == false)
	{
		return;
	}

	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(data->fightScript);
	dir_vec.push_back(dir);

	Bundle bundle;
	bundle.putBool("testMode",true);
	bundle.putBool("setFightBarVisible",false);
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec,BaseSprite::create("zhandou_beijing_muyuan.png"),bundle);
}

void TestCmdLayer::topHeroBattleCallBack(CCObject* f_msg_rsp)
{
	Flat_TopHeroChallengeRsp* data = (Flat_TopHeroChallengeRsp*) ((Flat_MSG_Rsp*)f_msg_rsp)->getData();

	GameDirector::getDirector()->hideWaiting();
	if(data->err_code != Success)
		return;

	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(data->fightScript);
	dir_vec.push_back(dir);

	Bundle bundle;
	bundle.putBool("setFightBarVisible",false);
	bundle.putBool("testMode",true);
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec,BaseSprite::create("zhandou_beijing_muyuan.png"),bundle);
}

void TestCmdLayer::topArenaTV_BattleCallback(CCObject* f_msg_rsp)
{
	Flat_topArenaReplayRsp* data = (Flat_topArenaReplayRsp*) ((Flat_MSG_Rsp*)f_msg_rsp)->getData();

	GameDirector::getDirector()->hideWaiting();
	if(data->err_code != Success)
		return;

	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(data->fightScript);
	dir_vec.push_back(dir);

	Bundle bundle;
	bundle.putBool("testMode",true);
	bundle.putBool("setFightBarVisible",false);
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec,BaseSprite::create("zhandou_beijing_muyuan.png"),bundle);
}

//void TestCmdLayer::battleCallBack(CCObject* msg_rsp)
//{
//	OneBattle* battle = (OneBattle*) ((MSG_Rsp*)msg_rsp)->getData();
//
//	if (battle->scriptVec.size() <= 0)
//	{
//		GameDirector::getDirector()->hideWaiting();
//		return;
//	}
//
//	vector<FightDirector> dir_vec;
//	int num = battle->scriptVec.size();
//	for (int i = 0;i<num;i++)
//	{
//		FightDirector dir;
//		dir.initScript(battle->scriptVec[i]);
//		dir_vec.push_back(dir);
//	}
//	
//	// 直接换scene
//	GameDirector::getDirector()->fight(dir_vec,BaseSprite::create("zhandou_beijing_muyuan.png"));
//	GameDirector::getDirector()->hideWaiting();
//}

void TestCmdLayer::lfTouchesBegan(CCPoint glPoint)
{
	lastPos = glPoint;
}
void TestCmdLayer::lfTouchesMoved(CCPoint glPoint)
{
	float y_offset = glPoint.y - lastPos.y;
	//setPositionY(getPositionY() + y_offset);
	CCNode* menu = getChildByTag(MENU_TAG);
	menu->setPositionY(menu->getPositionY() + y_offset);
	lastPos = glPoint;
}

// 失败，返回一个Code，就是协议定义的SimpleProtocolErrorCode
void TestCmdLayer::prepareError(int code)
{
	//  Success = 0,
	//	Error = 1,
	//	NotLogin = 2,
	//	AccountOrPswErr = 3,
	//	AccountAlreadyExist = 4,
	//	TiliNotEnought      = 5,
	if (code == TiliNotEnought)
	{
		LFToast::makeToast("TiliNotEnought");
		return;
	}

	if (code != Success)
	{
		LFToast::makeToast("Error");
		return;
	}
}
// 提供背景
CCSprite* TestCmdLayer::getBg(int taskId,int gridIndex)
{
	return BaseSprite::create("zhandou_beijing_muyuan.png");
}

void TestCmdLayer::prepareSuccess()
{
	fight->fight();
}

void testEquipVec()
{
	char temp[1024*10];
	vector<EquipData> test_data;

	EquipData zhuangbei;
	zhuangbei.diamond_type_1 = 0;
	zhuangbei.diamond_type_2 = 0;
	zhuangbei.diamond_type_3 = 0;
	zhuangbei.gongji_max = 0;
	zhuangbei.gongji_min = 0;
	zhuangbei.hole = 0;
	zhuangbei.hujia = 0;
	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	zhuangbei.equipDicrib = "equipDicrib";
	zhuangbei.zhuangbei_hero_id = 0;
	zhuangbei.zhuangbei_id = 0;
	zhuangbei.unknownHideAttriNum = 0;
	zhuangbei.zhuangbei_level = 0;
	zhuangbei.equipName = "equipName";
	zhuangbei.equipPic = "zb_fu_1";
	zhuangbei.zhuangbei_type = ZhuangbeiType_Dunpai;

	{
		ZBAttr atti;
		atti.attr_type = AttrType_mingjie;
		atti.cuiqu_suc_ratio = 10;
		atti.attr_value = 20;
		zhuangbei.attr_vec.push_back(atti);
	}

	{
		ZBAttr atti;
		atti.attr_type = AttrType_zhili;
		atti.cuiqu_suc_ratio = 5;
		atti.attr_value = 10;
		zhuangbei.attr_vec.push_back(atti);
	}

	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Baizhuang;
	//test_data.push_back(zhuangbei);
	test_data.push_back(zhuangbei);

	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Xiyou;
	test_data.push_back(zhuangbei);
	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Xiyou;
	test_data.push_back(zhuangbei);
	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Baizhuang;
	test_data.push_back(zhuangbei);
	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	zhuangbei.isShenBing = true;
	zhuangbei.unknownHideAttriNum = 5;
	test_data.push_back(zhuangbei);

	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	zhuangbei.isShenBing = true;
	zhuangbei.isInFengYin = false;

	ZBAttr zhuanke;  
	zhuanke.attr_type = 1;
	zhuanke.attr_value = 2;
	zhuanke.cuiqu_suc_ratio = 3;
	zhuangbei.zhuanke_attr_vec.push_back(zhuanke);
	zhuangbei.zhuanke_attr_vec.push_back(zhuanke);


	ZBAttr known; 
	zhuangbei.known_hide_attr_vec.push_back(known);
	zhuangbei.known_hide_attr_vec.push_back(known);
	zhuangbei.jinglian_attr_vec.push_back(known);
	zhuangbei.group_id = 1426;
	zhuangbei.unknownHideAttriNum = 5-zhuangbei.known_hide_attr_vec.size();
	test_data.push_back(zhuangbei);

	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	zhuangbei.group_id = 1427;
	test_data.push_back(zhuangbei);
	test_data.push_back(zhuangbei);
	test_data.push_back(zhuangbei);
	ZhuangbeiDetail* arry = (ZhuangbeiDetail*)temp;
	char* next1 = CS::hton_equipdata_2_zhuangbei_detail(test_data,arry);
	int lenght1 = (int)(next1 - (char*)arry);
	CCLog("lenght1 = %d",lenght1);
	vector<EquipData> out_data; 
	char* next = CS::ntoh_zhuangbei_detail_2_equipdata(arry,test_data.size(),out_data);
	int length2 = (int)(next - (char*)arry);
	CCLog("length2 = %d",length2);
	CCAssert(length2 < sizeof(temp),"不能越界");
	CCAssert(out_data.size() == test_data.size(),"");
	for (int i=0;i<test_data.size();i++)
	{
		CCAssert(out_data[i].zhuangbei_id == test_data[i].zhuangbei_id,"");
		CCAssert(out_data[i].group_id == test_data[i].group_id,"");
		CCAssert(out_data[i].zhuangbei_type == test_data[i].zhuangbei_type,"");
		CCAssert(out_data[i].equipName == test_data[i].equipName,"");
		CCAssert(out_data[i].zhuangbei_colour == test_data[i].zhuangbei_colour,"");
		CCAssert(out_data[i].equipPic == test_data[i].equipPic,"");
		CCAssert(out_data[i].isInFengYin == test_data[i].isInFengYin,"");
		CCAssert(out_data[i].isShenBing == test_data[i].isShenBing,"");
		CCAssert(out_data[i].zhuangbei_hero_id == test_data[i].zhuangbei_hero_id,"");
		CCAssert(out_data[i].zhuangbei_level == test_data[i].zhuangbei_level,"");
		CCAssert(out_data[i].hole == test_data[i].hole,"");
		CCAssert(out_data[i].diamond_type_1 == test_data[i].diamond_type_1,"");
		CCAssert(out_data[i].diamond_type_2 == test_data[i].diamond_type_2,"");
		CCAssert(out_data[i].diamond_type_3 == test_data[i].diamond_type_3,"");
		CCAssert(out_data[i].gongji_min == test_data[i].gongji_min,"");
		CCAssert(out_data[i].gongji_max == test_data[i].gongji_max,"");
		CCAssert(out_data[i].hujia == test_data[i].hujia,"");
		CCAssert(out_data[i].jinglian_times == test_data[i].jinglian_times,"");
		CCAssert(out_data[i].unknownHideAttriNum == test_data[i].unknownHideAttriNum,"");
		CCAssert(out_data[i].has_fumo == test_data[i].has_fumo,"");
		CCAssert(out_data[i].equipDicrib == test_data[i].equipDicrib,"");
		CCAssert(memcmp(&out_data[i].qianghua_data,&test_data[i].qianghua_data,sizeof(QianghuaData)) == 0,"");
		CCAssert(memcmp(&out_data[i].fumo_attr,&test_data[i].fumo_attr,sizeof(ZBAttr)) == 0,"");
		// 装备的魔法属性
		CCAssert(out_data[i].attr_vec.size() == test_data[i].attr_vec.size(),"");
		for (int att_index = 0,count = out_data[i].attr_vec.size(); att_index<count; att_index++)
		{
			CCAssert(memcmp(&out_data[i].attr_vec[att_index],&test_data[i].attr_vec[att_index],sizeof(ZBAttr)) == 0,"");
		}

		// 精炼所提供的属性
		CCAssert(out_data[i].jinglian_attr_vec.size() == test_data[i].jinglian_attr_vec.size(),"");
		for (int att_index = 0,count = out_data[i].jinglian_attr_vec.size(); att_index<count; att_index++)
		{
			CCAssert(memcmp(&out_data[i].jinglian_attr_vec[att_index],&test_data[i].jinglian_attr_vec[att_index],sizeof(ZBAttr)) == 0,"");
		}


		// 篆刻经文提供的数据
		CCAssert(out_data[i].zhuanke_attr_vec.size() == test_data[i].zhuanke_attr_vec.size(),"");
		for (int att_index = 0,count = out_data[i].zhuanke_attr_vec.size(); att_index<count; att_index++)
		{
			CCAssert(memcmp(&out_data[i].zhuanke_attr_vec[att_index],&test_data[i].zhuanke_attr_vec[att_index],sizeof(ZBAttr)) == 0,"");
		}

		// 已经解开的隐藏属性
		CCAssert(out_data[i].known_hide_attr_vec.size() == test_data[i].known_hide_attr_vec.size(),"");
		for (int att_index = 0,count = out_data[i].known_hide_attr_vec.size(); att_index<count; att_index++)
		{
			CCAssert(memcmp(&out_data[i].known_hide_attr_vec[att_index],&test_data[i].known_hide_attr_vec[att_index],sizeof(ZBAttr)) == 0,"");
		}
	}
	CCLog("validate over!");
}


void testGenScript(bool bossNoMove)
{
	FightScriptBuilder buider;
	unsigned int A_team[FormationHeroAcountMax] = {1,2,3,4};
	unsigned int B_team[FormationHeroAcountMax] = {5,6,7,8};
	vector<FightScript> scrips;
	ScriptBuilderUserData usrData;
	ClientTestProvider pro;
	HeroState first;
	HeroState second;
	HeroState third;

	//
	{
		FightSUD data;
		FightScript A_Script;
		FightScript B_Script;
		usrData.data = &data;
		data.fightIndex = 0;
		data.boss_freezy = bossNoMove;

		first = buider.genScript(A_team,B_team,A_Script,B_Script,&pro,usrData,FBB_BoardA);
		scrips.push_back(A_Script);
	}

	//CCLog("genScript over");
	//char* msg = buider.FightScript_2_Package(scrips);
	//CCLog("msg build over");   
	//int code;
	//vector<FightScript> A_tempVec = buider.SPCmd_BattleRsp_2_FightScript(msg+sizeof(SPHead),code);
	//delete [] msg;
	//CCLog("msg parse over");


	CCLog("genScript over");
	int length;
	char* msg = buider.FightScriptVec_2_CharArr(scrips,length);
	CCLog("msg build over");   
	vector<FightScript> A_tempVec = buider.CharArr_2_FightScriptVec(msg,scrips.size());
	delete [] msg;
	CCLog("msg parse over");

	// 校验
	{
		int num = scrips.size();
		for (int i=0;i<num;i++)
		{
			if (!scrips[i].isEqual(A_tempVec[i]))
			{
				CCLog("%s line:%d----> !A_Script.isEqual(A_Temp) Assert!",__FUNCTION__,__LINE__);
				CCMessageBox("Data InValide!","Error");
				return;
			}
		}
	}

	FightDirector dir;
	vector<FightDirector> dir_vec;

	for (int i=0;i<A_tempVec.size();i++)
	{
		dir.initScript(A_tempVec[i]);
		dir_vec.push_back(dir);
	}

	// 直接换scene
	Bundle bundle;
	bundle.putBool("testMode",true);

	BaseSprite* beiJing = BaseSprite::create("zhandou_beijing_chengzhenzhoubian.png");
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec,beiJing,bundle);
}

TestCmdLayer::~TestCmdLayer(void)
{
	delete fight;
}
