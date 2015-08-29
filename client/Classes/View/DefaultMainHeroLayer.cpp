#include "DefaultMainHeroLayer.h"
#include "ImageManager.h"
#include "LFUtile.h"
#include "font.h"
#include "GameDirector.h"
#include "ChooseHerosLayer.h"
#include "MainHeroManager.h"
#include "LFMessageBox.h"
#include "GameInforEditer.h"
#include "flat.h"
#include "TimeoutMonitor.h"
#include "LFStrings.h"
#include "TaskManager.h"
#include "MessageDisplay.h"
#include "NetSysUpMsgUI.h"
#include "CreateHeroLayer.h"
#include "History.h"
#include "VersionControl.h"
//extern int g_taskId;  // 当前战斗的id

static string zhiYeNamePic = "kaishi_jiaose_xinxi_";

#define TAG_ITEM_START		11111
#define TAG_ITEM_CHANGE		11112
#define TAG_ITEM_CREATRENWU	11113
#define TAG_UI_UpdataMsgUI	11114

extern bool isInGame;

// 需要同步修改后台的reload接口
enum RequestIndex
{
	STEP_GetHero = 0,
	STEP_GetSuit,// 必须先获取套装，再获取神兵。因为神兵获取完毕之后就会调用套装的数据，注入依赖.神兵也用到了套装的数据
	STEP_GetShenBingPool,
	STEP_GetEquip,
	STEP_GetItem,
	STEP_GetSysGift,
	STEP_GetSkill,
	STEP_MofaJing,
	STEP_GetItemSuiPian,
	STEP_GetShenBingSuiPian,
	STEP_ArenaTeamBasicInfo,
	STEP_QueryBonus,
	STEP_QueryVip,
	STEP_QueryChargeParket,
	STEP_GetTaskList,
	STEP_QueryGiftActivity,
	STEP_QueryKilledMonsterBonus,
	STEP_QueryDianFengDuiJue,
	STEP_QueryKilledMonster,
	STEP_QueryXueZhan,
	STEP_GetCityGuardCurrentStat,

	STEP_QueryFreePurifyTimes,
	STEP_QueryGuideProcess,//教学进度

	// 步数
	STEP_NUM,
	//	STEP_QueryLevelBonus, // 不用了
	// 	STEP_QueryDailyLoginBonus,
	// 	STEP_QuerySystemBonus,
	//STEP_QueryVip,
	//STEP_QueryDianFengDuiJue,
};

DefaultMainHeroLayer::DefaultMainHeroLayer(unsigned int defaultId)
{
//	g_taskId = -1;  // 当前用户被挤掉后，换个角色登录，避免进入冒险的时候会跳到上次被挤掉那个英雄所在的章节。
	TaskManager::getManager()->setCurrentFightTaskId(-1);

	currentHeroId = defaultId;
	mRequest = new LinkedRequest(this);

	if (defaultId == INVALID_ID)
		return;

	vector<HeroInfo> inforsAll = MainHeroManager::getManager()->getAllMainHero();

	HeroInfo currentHero;
	bool hited = false;
	int num = inforsAll.size();
	for (int i=0;i<num;i++)
	{
		if (inforsAll[i].hero_id == defaultId)
		{
			currentHero = inforsAll[i];
			hited = true;
			break;
		}
	}

	CCAssert(hited,"没有找到对应Id的英雄");

	// 分三段
	setHeadTopMost();
	setFootButtomMost();
	setBodyCenterMost();

	CCSprite* bg = CCSprite::create("main_bg.png");
	addChild(bg,-1);
	bg->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));


	BaseSprite* diZuo = BaseSprite::create("chuangjian_renwu_dizuo.png");
	addChildToBody(diZuo);
	diZuo->setVisible(false);
	diZuo->setPosition(ccp(mWinSize.width/2,250));
	

	ImageData data = ImageManager::getManager()->getImageData_JueSheQuanShen((Profession)currentHero.profession_id);
    jueShe = BaseSprite::create(data.getCurrentImageFile());
	addChildToBody(jueShe);
	jueShe->setPosition(ccp(mWinSize.width/2,960/2+10));

	string fileName = zhiYeNamePic + 
		ImageManager::getManager()->getBaseImageName((Profession)currentHero.profession_id) + 
		".png";
	jueSheName = BaseSprite::create(fileName);
	addChildToBody(jueSheName);
	jueSheName->setPosition(ccp(100,702));

	BaseSprite* xinxi_bg = BaseSprite::create("kaishi_renwu_xinxi_bg .png");
	xinxi_bg->setScale(1.1f);
	addChildToBody(xinxi_bg);
	xinxi_bg->setPosition(ccp(jueSheName->getPositionX(),jueSheName->getPositionY()-jueSheName->getContentSize().height/2- 79));


    gamename=currentHero.name;
	ganmelevel=CCString::createWithFormat("%d",currentHero.level)->getCString()+LF::getString("ji");

    namelabel=CCLabelTTF::create(gamename.c_str(), fontStr_kaiti, 26);
	namelabel->setPosition(ccp(xinxi_bg->getPositionX(),xinxi_bg->getPositionY()));
	addChildToBody(namelabel);
    levelabel=CCLabelTTF::create(ganmelevel.c_str(), fontStr_kaiti, 26);
	levelabel->setPosition(ccp(xinxi_bg->getPositionX(),namelabel->getPositionY()-namelabel->getContentSize().height/2-levelabel->getContentSize().height/2-10));
	levelabel->setColor(fonColor_PuTong);
	addChildToBody(levelabel);

	//角色背景jiaose_liebiao_biaoti.png
	BaseSprite* juese_bg = BaseSprite::create("jiaose_liebiao_biaoti.png");
	juese_bg->setPosition(ccp(541,702));
	addChildToBody(juese_bg);

    //角色列表
	vector<HeroInfo>  mData=MainHeroManager::getManager()->getAllMainHero();
	for (int i=0;i<num;i++)
	{
		if ( mData[i].hero_id == defaultId)
		{
			 HeroInfo info= mData[i];
			 std::vector<HeroInfo>::iterator it =  mData.begin()+i; 
		     mData.erase(it);
			 mData.push_back(info);
			 break;
		}
	}


	int listsize= mData.size();
	HeroListDefaltView* listView = new HeroListDefaltView(this,defaultId);
	CCTableView *view = listView->getTableView();
	int heroindex=0;
	for(int i=0;i<mData.size();i++){
		if(gamename.compare(mData[i].name)==0){

			heroindex=i;
			break;
		}
	}
	heroindex=mData.size()-heroindex;
	float byl=0;
	int Suoxucz=listView->data.listViewContainer->getContentSize().height/listView->data.itemDefaultSize.height;
	//偏移量
	if(mData.size()>=3&&heroindex+(Suoxucz-1)<mData.size()){
		byl=((mData.size()-(heroindex+2))*(listView->data.itemDefaultSize.height));
		byl=-byl;
		view->setContentOffset(ccp(0,byl));
	
	}else if(mData.size()>=3&&heroindex+(Suoxucz-1)>=mData.size()){
		byl=0;//((heroindex+(Suoxucz-1))-mData.size())*listView->data.itemDefaultSize.height;
		view->setContentOffset(ccp(0,byl));
		
	}else{


	}
	
	listView->setPosition(ccp(juese_bg->getPositionX(),juese_bg->getPositionY()-juese_bg->getContentSize().height-95));
	addChildToBody(listView);

	BaseSprite *jiantous=BaseSprite::create("gongjiang_cuiqu_fumo_chenggong_jiantou.png");
	jiantous->setRotation(-90.0);
	jiantous->setPosition(ccp(juese_bg->getPositionX(),juese_bg->getPositionY()-juese_bg->getContentSize().height/2-15));
	addChildToBody(jiantous);

	BaseSprite *jiantoux=BaseSprite::create("gongjiang_cuiqu_fumo_chenggong_jiantou.png");
	jiantoux->setRotation(90.0);
	jiantoux->setPosition(ccp(juese_bg->getPositionX(),jiantous->getPositionY()-listView->data.listViewContainer->getContentSize().height-jiantous->getContentSize().height));
	addChildToBody(jiantoux);

	CCMenuItemImage* chuangjianrenwu = CCMenuItemImage::create(
		"qiehuan_chuangjian.png",
		"qiehuan_chuangjian_select.png",
		this,SEL_MenuHandler(&DefaultMainHeroLayer::itemClicked)
		);


	CCMenuItemImage* kaiShi = CCMenuItemImage::create(
		"kaishi.png",
		"kaishi_select.png",
		this,SEL_MenuHandler(&DefaultMainHeroLayer::itemClicked)
		);
	CCMenuItemImage* qieHuan = CCMenuItemImage::create(
		"kaishi_qiehuan_jiaose.png",
		"kaishi_qiehuan_jiaose_select.png",
		this,SEL_MenuHandler(&DefaultMainHeroLayer::itemClicked)
		);

	kaiShi->setTag(TAG_ITEM_START);
	qieHuan->setTag(TAG_ITEM_CHANGE);
	chuangjianrenwu->setTag(TAG_ITEM_CREATRENWU);
	qieHuan->setVisible(false);

	menu = CCMenu::create(kaiShi,qieHuan,chuangjianrenwu,NULL);
	addChildToFoot(menu);
	menu->setPosition(CCPointZero);
	kaiShi->setPosition(ccp(mWinSize.width/2,65));
	qieHuan->setPosition(ccp(mWinSize.width/2,137));
	chuangjianrenwu->setPosition(ccp(juese_bg->getPositionX(),bg->getPositionY()-160));
}

void DefaultMainHeroLayer::itemClicked(CCObject* sender)
{
	CCMenuItem* item = (CCMenuItem*)sender;

	if (item->getTag() == TAG_ITEM_START)
	{
		//isInGame = false; // 这里修改值，让cmdHelper的状态机停掉
		//chooseHero(currentHeroId);
		bool first = false;
		if (!History::getHistory()->getBoolFromCurrentUser("FIRST_COME_IN_GAME",first))
		{
			first = true;
		}

		if (first)
		{
			History::getHistory()->putInCurrentUser("FIRST_COME_IN_GAME",false);
			isInGame = false; // 这里修改值，让cmdHelper的状态机停掉
			chooseHero(currentHeroId);
		}
		else
		{
			addObserver(callfuncO_selector(DefaultMainHeroLayer::systemVersionCallBack), MSG_QueryNotifyInfoRsp);
			CmdHelper::getHelper()->cmdQuerySystemNotifyInfo();
		}
		
	}
	else if(item->getTag() == TAG_ITEM_CHANGE)
	{
		ChooseHerosLayer* choose = new ChooseHerosLayer(currentHeroId);
		CCScene* scene = CCScene::create();
		scene->addChild(choose);
		choose->release();
		CCDirector::sharedDirector()->replaceScene(scene);
	}else if(item->getTag() == TAG_ITEM_CREATRENWU){

		CreateHeroLayer* layer = new CreateHeroLayer(CreateHero_Dest_ChooseHerosLayer);
		CCScene* scene = CCScene::create();
		scene->addChild(layer);
		layer->release();
		CCDirector::sharedDirector()->pushScene(scene);

	}
}
void  DefaultMainHeroLayer::systemVersionCallBack(CCObject *obj)
{
	removeObserver(MSG_QueryNotifyInfoRsp);
	GameDirector::getDirector()->hideWaiting();
	
	VersionControlHelper *helper = VersionControlHelper::getHelper();
	Flat_SystempNotifyInfoRsp* data = (Flat_SystempNotifyInfoRsp*)((Flat_MSG_Rsp*)obj)->getData();
	if (data->err_code)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	NetSysUpMsgUI *layer = NetSysUpMsgUI::create();
	layer->setActivityMsg(data->activityVec);

	if (helper->isDisplayNewVersionInfo())
	{
		layer->setUpdataMsg(*helper->getUpdateInfo(),helper->getCurrentVersion());
	}
	
	layer->setCallBackHandler(this,SEL_CallFuncN(&DefaultMainHeroLayer::UpMsgUICallBack));
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1,TAG_UI_UpdataMsgUI);
	//CCDirector::sharedDirector()->getRunningScene()->addChild(NetSysUpMsgUI::create());
}
void DefaultMainHeroLayer::UpMsgUICallBack(CCNode *node)
{
	if (node->getTag() == 1)
	{
		isInGame = false; // 这里修改值，让cmdHelper的状态机停掉
		chooseHero(currentHeroId);
	}
}
void DefaultMainHeroLayer::disableAllTouchBegin()
{
	setTouchEnabled(false);
	menu->setEnabled(false);
}
void DefaultMainHeroLayer::disableAllTouchEnd()
{
	setTouchEnabled(false);
	menu->setEnabled(true);
}

void DefaultMainHeroLayer::chooseHero(unsigned int heroId)
{
	currentHeroId = heroId;
	GameInforEditer::getGameInfor()->setLastLoginedHeroId(heroId);
	mRequest->begin(STEP_NUM*2+10); // 开始
}
bool DefaultMainHeroLayer::getActorsOver(CCObject* msg_rsp)
{
	SPCmd_GetHerosInTeamRsp* rsp = (SPCmd_GetHerosInTeamRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code == Error)
	{
		LFMessageBox::alert("","Get Heros In Team Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getEquipsOver(CCObject* msg_rsp)
{
	Flat_GetZhuangbeiInTeamRsp* rsp = (Flat_GetZhuangbeiInTeamRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code == Error)
	{
		LFMessageBox::alert("","Get Equips In Team Error!");
		CCLog("Get Equips In Team Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getItemsOver(CCObject* msg_rsp)
{
	SPCmd_GetItemsInTeamRsp* rsp = (SPCmd_GetItemsInTeamRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get Equips In Team Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getSysGiftOver(CCObject* f_msg_rsp)
{
	Flat_SystemGiftQueryRsp* rsp = (Flat_SystemGiftQueryRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get System Gift In Team Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getSuitOver(CCObject* msg_rsp)
{
	SPCmd_QueryAllSuitNameRsp* rsp = (SPCmd_QueryAllSuitNameRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get Suit Data Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getSkillOver(CCObject* msg_rsp)
{
	SPCmd_QueryTeamSkillListRsp* rsp = (SPCmd_QueryTeamSkillListRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get Skill Data Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getMoFajinghuaOver(CCObject* msg_rsp)
{
	SPCmd_QueryAllMofaJinghuaRsp* rsp = (SPCmd_QueryAllMofaJinghuaRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get MoFaJingHua Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getShenBingPoolOver(CCObject* f_msg_rsp)
{
	Flat_QueryAllShenbingListRsp* rsp = (Flat_QueryAllShenbingListRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get ShenBingPool Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getShenBingSuiPianOver(CCObject* msg_rsp)
{
	SPCmd_QueryTeamShenbingSuipianRsp* rsp = (SPCmd_QueryTeamShenbingSuipianRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get ShenBingSuiPian Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getItemSuiPianOver(CCObject* msg_rsp)
{
	SPCmd_QueryTeamItemSuipianRsp* rsp = (SPCmd_QueryTeamItemSuipianRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get ItemSuiPian Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::getArenaTeanBasicInfoOver(CCObject* msg_rsp)
{
	SPCmd_QueryArenaTeamBasicInfoRsp* rsp = (SPCmd_QueryArenaTeamBasicInfoRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get ArenaTeanBasicInfo Error!");
		return false;
	}
	return true;
}
/*bool DefaultMainHeroLayer::queryDailyLoginBonusOver(CCObject* f_msg_rsp)
{
	Flat_QueryDailyLoginBonusRsp* rsp = (Flat_QueryDailyLoginBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get DailyLoginBonus Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::queryLevelBonusOver(CCObject* f_msg_rsp)
{
	Flat_QueryLevelBonusRsp* rsp = (Flat_QueryLevelBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get LevelBonus Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::querySystemBonusOver(CCObject* f_msg_rsp)
{
	Flat_QuerySystemBonusRsp* rsp = (Flat_QuerySystemBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get SystemBonus Error!");
		return false;
	}
	return true;
}*/
bool DefaultMainHeroLayer::queryBonusOver(CCObject* f_msg_rsp)
{
	Flat_QueryBonusRsp* rsp = (Flat_QueryBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get LevelBonus Error!");
		return false;
	}
	return true;
}

bool DefaultMainHeroLayer::queryChargetParketOver(CCObject* f_msg_rsp)
{
	Flat_QueryChargePacketRsp* rsp = (Flat_QueryChargePacketRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get ChargeParket Error!");
		return false;
	}
	return true;
}

bool DefaultMainHeroLayer::getTaskListOver(CCObject* msg_rsp)
{
	SPCmd_GetTaskListRsp* rsp = (SPCmd_GetTaskListRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","GetTaskListRsp Error!");
		return false;
	}
	return true;
}

bool DefaultMainHeroLayer::queryGiftActivityOver(CCObject* f_msg_rsp)
{
	Flat_QueryGiftActivityRsp* rsp = (Flat_QueryGiftActivityRsp*)((MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","QueryGiftActivity Error!");
		return false;
	}
	return true;
}
bool DefaultMainHeroLayer::queryKilledMonsterBonusOver(CCObject* f_msg_rsp)
{  
	Flat_QueryKilledMonsterBonusRsp* rsp = (Flat_QueryKilledMonsterBonusRsp*)((MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","QueryKilledMonsterBonus Error!");
		return false;
	}
	return true;
}

#pragma pack(1) //按照单字节对齐,跟协议要一致
struct WildcardError
{
	int err_code;
};
#pragma pack() //结束

bool DefaultMainHeroLayer::queryOver(CCObject* msg_rsp,string log)
{  
	WildcardError* rsp = (WildcardError*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("Error",log.c_str());
		return false;
	}
	return true;
}


bool DefaultMainHeroLayer::queryVipOver(CCObject* msg_rsp)
{
	SPCmd_QueryVipPrivilegeRsp* rsp = (SPCmd_QueryVipPrivilegeRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		LFMessageBox::alert("","Get VIP Error!");
		return false;
	}
	return true;
}

// 获取总共需要请求的个数
int DefaultMainHeroLayer::getRequestNum()
{
	return STEP_NUM;
}
// 开始发第index个请求
void DefaultMainHeroLayer::request(int index)
{
	RequestIndex rIndex = (RequestIndex)index;
	switch (rIndex)
	{
	case STEP_GetHero:
		mCmdHlp->cmdGetHerosInTeam(currentHeroId);
		break;
	case STEP_GetEquip:
		mCmdHlp->cmdGetZhuangbeiInTeam();
		break;
	case STEP_GetItem:
		mCmdHlp->cmdGetItemsInTeam();
		break;
	case STEP_GetSysGift:
		mCmdHlp->cmdSystemGiftQuery();
		break;
	case STEP_GetSuit:
		mCmdHlp->cmdQueryAllSuitName();
		break;
	case STEP_GetSkill:
		mCmdHlp->cmdQueryTeamSkillList();
		break;
	case STEP_MofaJing:
		mCmdHlp->cmdQueryAllMofaJinghua();
		break;
	case STEP_GetItemSuiPian:
		mCmdHlp->cmdQueryTeamItemSuipian();
		break;
	case STEP_GetShenBingPool:
		mCmdHlp->cmdQueryAllShenbingList();
		break;
	case STEP_GetShenBingSuiPian:
		mCmdHlp->cmdQueryTeamShenbingSuipian();
		break;
	case STEP_ArenaTeamBasicInfo:
		mCmdHlp->cmdQueryArenaTeamBasicInfo();
		break;
//20140613 奖励系统变更（奖励与悬赏任务合并）
// 	case STEP_QueryDailyLoginBonus:
// 		mCmdHlp->cmdQueryDailyLoginBonus();
// 		break;
// 	case STEP_QueryLevelBonus:
// 		mCmdHlp->cmdQueryLevelBonus();
// 		break;
// 	case STEP_QuerySystemBonus:
// 		mCmdHlp->cmdQuerySystemBonus();
// 		break;
	case STEP_QueryBonus:
		mCmdHlp->cmdQueryBonus();
		break;
	case STEP_QueryVip:
		mCmdHlp->cmdQueryVipPrivilege();
		break;
	case STEP_QueryChargeParket:
		mCmdHlp->cmdQueryChargePacket();
		break;
	case STEP_GetTaskList:
		mCmdHlp->cmdGetTaskList(0);
		break;
	case STEP_QueryGiftActivity:
		mCmdHlp->cmdQueryGiftActivity();
		break;
	case STEP_QueryKilledMonsterBonus:
		mCmdHlp->cmdQueryKilledMonsterBonus();
		break;
	case STEP_QueryDianFengDuiJue:
		mCmdHlp->cmdTopArenaQueryStatus();
		break;
	case STEP_QueryKilledMonster:
		mCmdHlp->cmdQueryKilledMonster();
		break;
	case STEP_QueryXueZhan:
		mCmdHlp->cmdQuerySelfChallengeInfo();
		break;
	case STEP_GetCityGuardCurrentStat:
		mCmdHlp->cmdGetCityGuardCurrentStat();
		break;

	case STEP_QueryFreePurifyTimes:
		mCmdHlp->cmdQueryPurify();
		break;
	case STEP_QueryGuideProcess:
		mCmdHlp->cmdGetGuideState();
		break;
	default:
		CCAssert(false,"不支持");
		break;
	}

	// 不监视，LinkedRequest会监视
	TimeoutMonitor::getMonitor()->cancleMonitor();
}
// 获取当前需要监听的消息MSG_****
string DefaultMainHeroLayer::getMessgaeToListening(int index)
{
	string ret = "";
	RequestIndex rIndex = (RequestIndex)index;
	switch (rIndex)
	{
	case STEP_GetHero:
		ret = MSG_getHerosInTeamRsp;
		break;
	case STEP_GetEquip:
		ret = MSG_getZhuangbeiInTeamRsp_F;
		break;
	case STEP_GetItem:
		ret = MSG_getItemsInTeamRsp;
		break;
	case STEP_GetSysGift:
		ret = MSG_systemGiftQueryRsp_F;
		break;
	case STEP_GetSuit:
		ret = MSG_QueryAllSuitNameRsp;
		break;
	case STEP_GetSkill:
		ret = MSG_QueryTeamSkillListRsp;
		break;
	case STEP_MofaJing:
		ret = MSG_QueryAllMofaJinghuaRsp;
		break;
	case STEP_GetShenBingPool:
		ret = MSG_QueryAllShenbingListRsp_F;
		break;
	case STEP_GetShenBingSuiPian:
		ret = MSG_QueryTeamShenbingSuipianRsp;
		break;
	case STEP_GetItemSuiPian:
		ret = MSG_QueryTeamItemSuipianRsp;
		break;
	case STEP_ArenaTeamBasicInfo:
		ret = MSG_queryArenaTeamBasicInfoRsp;
		break;
// 	case STEP_QueryDailyLoginBonus:
// 		ret = MSG_queryDailyLoginBonusRsp_F;
// 		break;
// 	case STEP_QueryLevelBonus:
// 		ret = MSG_queryLevelBonusRsp_F;
// 		break;
// 	case STEP_QuerySystemBonus:
// 		ret = MSG_querySystemBonusRsp_F;
// 		break;
	case STEP_QueryBonus:
		ret = MSG_queryBonusRsp;
		break;
	case STEP_QueryVip:
		ret = MSG_queryVipPrivilegeRsp;
		break;
	case STEP_QueryChargeParket:
		ret = MSG_queryChargePacketRsp_F;
		break;
	case STEP_GetTaskList:
		ret = MSG_getTaskListRsp;
		break;
	case STEP_QueryGiftActivity:
		ret = MSG_queryGiftActivityRsp_F;
		break;
	case STEP_QueryKilledMonsterBonus:
		ret = MSG_QueryKilledMonsterBonusRsp_F;
		break;
	case STEP_QueryDianFengDuiJue:
		ret = MSG_topArenaQueryStatusRsp;
		break;
	case STEP_QueryKilledMonster:
		ret = MSG_queryKilledMonsterRsp;
		break;
	case STEP_QueryXueZhan:
		ret = MSG_querySelfChallengeInfoRsp;
		break;
	case STEP_GetCityGuardCurrentStat:
		ret = MSG_getCityGuardCurrentStatRsp;
		break;

	case STEP_QueryFreePurifyTimes:
		ret = MSG_QueryPurifyRsp;
		break;
	case STEP_QueryGuideProcess:
		ret = MSG_GetGuideStateRsp;
		break;

	default:
		CCAssert(false,"不支持");
		break;
	}

	return ret;
}

void DefaultMainHeroLayer::timeout()
{
	GameDirector::getDirector()->hideWaiting();
	LFMessageBox::alert(LFStrings::getValue("timeout_title"),LFStrings::getValue("wait_timeout2"));
}

// 收到消息
// msg_data : 就是MSG_Data或者是flat的，这个随具体的命令
// index	: 当前消息是属于第几个的
// 返回true就代表正常，可以继续，false就代表错误，终端
bool DefaultMainHeroLayer::receivedData(CCObject* msg_data,int index)
{
	bool ret = false;

	RequestIndex rIndex = (RequestIndex)index;
	switch (rIndex)
	{
	case STEP_GetHero:
		ret = getActorsOver(msg_data);
		break;
	case STEP_GetEquip:
		ret = getEquipsOver(msg_data);
		break;
	case STEP_GetItem:
		ret = getItemsOver(msg_data);
		break;
	case STEP_GetSysGift:
		ret = getSysGiftOver(msg_data);
		break;
	case STEP_GetSuit:
		ret = getSuitOver(msg_data);
		break;
	case STEP_GetSkill:
		ret = getSkillOver(msg_data);
		break;
	case STEP_MofaJing:
		ret = getMoFajinghuaOver(msg_data);
		break;
	case STEP_GetShenBingPool:
		ret = getShenBingPoolOver(msg_data);
		break;
	case STEP_GetShenBingSuiPian:
		ret = getShenBingSuiPianOver(msg_data);
		break;
	case STEP_GetItemSuiPian:
		ret = getItemSuiPianOver(msg_data);
		break;
	case STEP_ArenaTeamBasicInfo:
		ret = getArenaTeanBasicInfoOver(msg_data);
		break;
// 	case STEP_QueryLevelBonus:
// 		ret = queryLevelBonusOver(msg_data);
// 		break;
// 	case STEP_QueryDailyLoginBonus:
// 		ret = queryDailyLoginBonusOver(msg_data);
// 		break;
// 	case STEP_QuerySystemBonus:
// 		ret = querySystemBonusOver(msg_data);
// 		break;
	case STEP_QueryBonus:
		ret = queryBonusOver(msg_data);
		break;
	case STEP_QueryVip:
		ret = queryVipOver(msg_data);
		break;
	case STEP_QueryChargeParket:
		ret = queryChargetParketOver(msg_data);
		break;
	case STEP_GetTaskList:
		ret = getTaskListOver(msg_data);
		break;
	case STEP_QueryGiftActivity:
		ret = queryGiftActivityOver(msg_data);
		break;
	case STEP_QueryKilledMonsterBonus:
		ret = queryKilledMonsterBonusOver(msg_data);
		break;
	case STEP_QueryDianFengDuiJue:
		ret = queryOver(msg_data,"STEP_QueryDianFengDuiJue");
		break;
	case STEP_QueryKilledMonster:
		ret = queryOver(msg_data,"STEP_QueryKilledMonster");
		break;
	case STEP_QueryXueZhan:
		ret = queryOver(msg_data,"STEP_QueryXueZhan");
		break;
	case STEP_GetCityGuardCurrentStat:
		mCmdHlp->cmdQuitCityGuard();//不发退出消息、会一直收到boss血量的推送、~~烦得很
		//GameDirector::getDirector()->hideWaiting();
		ret = queryOver(msg_data,"STEP_GetCityGuardCurrentStat");
		break;

	case STEP_QueryFreePurifyTimes:
		ret = queryOver(msg_data,"STEP_QueryFreePurifyTimes");
		break;
	case STEP_QueryGuideProcess:
		ret = queryOver(msg_data,"STEP_QueryGuideProcess");
		break;

	default:
		CCAssert(false,"不支持");
		break;
	}
	return ret;
}
// 全部完毕，结束了
void DefaultMainHeroLayer::end()
{
	VersionControlHelper::getHelper()->setDisplayNewVersionInfo(true);
	isInGame = true;
	GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	GameDirector::getDirector()->hideWaiting();
}


DefaultMainHeroLayer::~DefaultMainHeroLayer(void)
{
	delete mRequest;
}

void DefaultMainHeroLayer::selectHeroById(unsigned int heroId){

	mSelectedHeroId = heroId;
	HeroInfo hero = MainHeroManager::getManager()->getHeroByHeroId(heroId);
	ImageData data = ImageManager::getManager()->getImageData_JueSheQuanShen((Profession)hero.profession_id);
	jueShe->initWithFile(data.getCurrentImageFile());
	gamename=hero.name;
	ganmelevel=CCString::createWithFormat("%d",hero.level)->getCString()+LF::getString("ji");
	namelabel->setString(gamename.c_str());
	levelabel->setString(ganmelevel.c_str());
	string fileName = zhiYeNamePic + 
		ImageManager::getManager()->getBaseImageName((Profession)hero.profession_id) + 
		".png";
	jueSheName->initWithFile(fileName) ;
}

// 前缀
static string listItemPic = "jiaose_liebiao_";
#define TAG_ITEM_LABLE_NAME		222222
#define TAG_ITEM_LABLE_LEVEL	222223
HeroListDefaltView::HeroListDefaltView(DefaultMainHeroLayer* layer,unsigned int selectedHeroId)
{
	mLayer = layer;
	mDataVec = MainHeroManager::getManager()->getAllMainHero();
	selectedIndex = -1;
	int num = mDataVec.size();

	
	for (int i=0;i<num;i++)
	{
		if (mDataVec[i].hero_id == selectedHeroId)
		{
			 selectedIndex = i;
			 HeroInfo info=mDataVec[i];
			 std::vector<HeroInfo>::iterator it = mDataVec.begin()+i; 
		     mDataVec.erase(it);
			 mDataVec.push_back(info);
			 CCLOG("size------->%d   index----->%d",num,selectedIndex);
		}
	}

	

	for (int i=0;i<num;i++)
	{
		if (mDataVec[i].hero_id == selectedHeroId)
		{
			selectedIndex = i;
		}
	}
	CCAssert(selectedIndex != -1,"没有找到指定Id的Hero");

	
	data.creator = this;
	data.dir = kCCScrollViewDirectionVertical;
	data.itemDefaultSize = CCSizeMake(213,66);
	data.listItemNum = mDataVec.size();
	data.listViewContainer->setContentSize(CCSizeMake(214,198));
	
	reloadData(data);
}

// 创建第i个元素
BaseSprite* HeroListDefaltView::getItemContentByIndex(unsigned int index)
{
	HeroInfo infor = mDataVec[index];
	HeroFixedPro pro = BaseActorPropertyParser::getParser()->getExtraProerty((Profession)infor.profession_id);
	
	string fileName = 
		listItemPic + 
		ImageManager::getManager()->getBaseImageName((Profession)infor.profession_id)+
		(selectedIndex == index?"_select.png":".png");
	BaseSprite* item = BaseSprite::create(fileName);

	CCLabelTTF* label=LF::lable(item,mDataVec[index].name,ccp(55,30),20,fontStr_kaiti);
	label->setTag(TAG_ITEM_LABLE_NAME);
	label->setColor(fonColor_PuTong);
	/*
	LF::lable(item,CCString::createWithFormat(
		"%s%d %s",LF::getString("dengji").c_str(),infor.level,pro.nameInChinese.c_str()),
		ccp(61,20),15,fontStr_kaiti)->setTag(TAG_ITEM_LABLE_LEVEL);
	*/
	return item;
};
// 刷新第i个元素，也可以简单的用getItemContentByIndex，再创建一个就是了
void HeroListDefaltView::refreshItem(CCNode* item,int index)
{
	HeroInfo infor = mDataVec[index];
	HeroFixedPro pro = BaseActorPropertyParser::getParser()->getExtraProerty((Profession)infor.profession_id);

	BaseSprite* s = (BaseSprite*)item;
	string fileName = 
		listItemPic + 
		ImageManager::getManager()->getBaseImageName((Profession)infor.profession_id)+
		(selectedIndex == index?"_select.png":".png");
	s->initWithFile(fileName);

	CCLabelTTF* labeName = (CCLabelTTF*)s->getChildByTag(TAG_ITEM_LABLE_NAME);
	CCLabelTTF* labeLevel = (CCLabelTTF*)s->getChildByTag(TAG_ITEM_LABLE_LEVEL);
	labeName->setString(infor.name);
	labeName->setColor(fonColor_PuTong);
	
}


void HeroListDefaltView::listItemTouched(CCNode* listItem,int index)
{
	if (index != selectedIndex)
	{
		// 上次的还原
		if (selectedIndex != -1)
		{
			string fileName = 
				listItemPic + 
				ImageManager::getManager()->getBaseImageName((Profession)mDataVec[selectedIndex].profession_id)+
				".png";

			BaseSprite* last = (BaseSprite*)getListItemByIndex(selectedIndex);
			if (last != NULL)
			{
				last->initWithFile(fileName);
				CCLabelTTF* labelp=(CCLabelTTF*)last->getChildByTag(TAG_ITEM_LABLE_NAME);
		        labelp->setColor(fonColor_PuTong);
			}	
		}
		// 刷新这次的
		string fileNameNow = 
			listItemPic + 
			ImageManager::getManager()->getBaseImageName((Profession)mDataVec[index].profession_id)+
			"_select.png";
		BaseSprite* now = (BaseSprite*)getListItemByIndex(index);
		now->initWithFile(fileNameNow);
		BaseSprite* p=(BaseSprite*)listItem;
		CCLabelTTF* labelp=(CCLabelTTF*)p->getChildByTag(TAG_ITEM_LABLE_NAME);
		labelp->setColor(fonColor_FaGuang);
		mLayer->selectHeroById(mDataVec[index].hero_id);
	}
	selectedIndex = index;
	HeroInfo infor = mDataVec[index];
	mLayer->currentHeroId=infor.hero_id;

	
	/*
	int heroindex=mDataVec.size()-index;
	float byl=0;
	//偏移量
	if(heroindex+2<mDataVec.size()){
		byl=((mDataVec.size()-(heroindex+2))*(data.itemDefaultSize.height));
		byl=-byl;
	}else{
		byl=((heroindex+2)-mDataVec.size())*data.itemDefaultSize.height;
	}
	getTableView()->setContentOffset(ccp(0,byl));
	*/
}
