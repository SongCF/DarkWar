#include "CmdHelper.h"
#include "GameInforEditer.h"
#include "GameDirector.h"
#include "LFUtile.h"
#include "BaseActor.h" 
#include "MyselfManager.h"
#include "EquipmentManager.h"
#include "Equipment.h"
#include "WuPinManager.h"
#include "MarketManager.h"
#include "CS_Common.h"
#include "ServerTime.h"
#include "TaskManager.h"
#include "NetWorkThreadProc.h"
#include "BaseActorPropertyParser.h"
#include "MainHeroManager.h"
#include "LFToast.h"
#include "LFMessageBox.h"
#include "ServerTime.h"
#include "FightScriptBuilder.h"
#include "flat.h"
#include "SystemGiftManager.h"
#include "DuanZaoManager.h"
#include "SuitPool.h"
#include "MofaJinghuaManager.h"
#include "CmdBuffer.h"
#include "FightPackage.h"
#include "ChessBoardStateManager.h"
#include "SkillManager.h"
#include "TimeoutMonitor.h"
#include "BaseActorPropertyParser.h"
#include "ShenBingPool.h"
#include "SuiPianManager.h"
#include "ZahuopuHelper.h"
#include "ClientErrorCode.h"
#include "WorldBossCityGuardManager.h"
#include "BonusManager.h"
#include "GiftActivtyManager.h"
#include "DianFengDuiJueManager.h"
#include "XueZhanManager.h"
#include "ShouHuZhiLingManager.h"
#include "TanSuoManager.h"
#include "JieFengYinMapManager.h"
#include "EmailManager.h"
#include "SystemGiftBagManager.h"
#include "EquipFallManger.h"
#include "SystemPushMessagePool.h"
#include "GuideLayerBase.h"
#include "EquipmentConfigInfoManager.h"


#define DATAPOOL_LOG 1

#if (DATAPOOL_LOG == 1)
#define CMD_LOG(format, ...)     cocos2d::CCLog(format, ##__VA_ARGS__)
#else
#define CMD_LOG(format, ...)     
#endif

#define ACTOR_MAX_LEVEL	30 // 最大30级

// 状态
enum DATAPOOL_STATE
{
	STATE_WORKING				= 0,		// 工作状态
	STATE_MODELESS_RECONN		= 1,		// 非模态重联
	STATE_MODE_RECONN			= 2,		// 模态重联
	STATE_MODELESS_NOT_LOGIN	= 3,		// 非模态未登录
	STATE_MODE_NOT_LOGIN		= 4,		// 模态未登录
	STATE_MODE_RELOAD			= 5,		// 模态Reload
};
// 初始是非模态重联的状态
static DATAPOOL_STATE mCurrentState = STATE_WORKING;
static bool state_enable = false;	// 默认开始不启用，直到进入主界面，也就是取完了物品之后
bool isInGame = false;	// 是否到了游戏里面了，这样的情况下才可以开始状态机

// 上次的发送命令的数据
static CmdData lastCmd;
CmdHelper* CmdHelper::helper = NULL;
static CmdBuffer* cmdBuffer = new CmdBuffer();
static string loginedName = "";
static string loginedPsw = "";
static unsigned int login_choosed_main_hero = 0;
static bool needSendReconnectDataWhenConn = false;	// 是否在链接上服务器后需要发送重连数据包，在选择了英雄之后就需要了
static bool needSendLoginDataWhenConn = false;	// 是否在链接上服务器后需要发送重连数据包，在login了之后，选择了英雄之前就需要

// addObserver时监听的网络消息
static vector<string> msg_vec;

// Buff Runtime
SPCmd_CityGuardEnterRsp buff_CityGuard;

// 重联需要发送的命令
// 主要是设计到Actor，Equip的都得重刷
// 因为重联之后这些数据全部刷新了。delete再new的
// 凡是有关联的都得重新请求，再完成数据的关联
// 按顺序获取 --> reload()
static const SimpleProtocolCmd reloadCmdLink[] = {
	getHerosInTeamRsp,
	getZhuangbeiInTeamRsp,
	getItemsInTeamRsp,
	//systemGiftQueryRsp,		// 这个不用获取，重联数据不会坏的
	//queryAllSuitNameRsp,		// 这个不用获取，重联数据不会坏的
	queryTeamSkillListRsp,
	queryAllMofaJinghuaRsp,
	queryTeamItemSuipianRsp,
	queryTeamShenbingSuipianRsp,
	queryArenaTeamBasicInfoRsp,
};
// 最后一个重载的命令
static const SimpleProtocolCmd endedReloadCmd = reloadCmdLink[sizeof(reloadCmdLink)/sizeof(SimpleProtocolCmd)-1];

//
// 标示竞技场是否可以广播数据
// -------------------------------------------------------------------------------------------
// 因为有这样的一个情况：
// A 进入了竞技场，发了Enter，然后服务器也回了Rsp，但是呢，网络故障
// 接收失败，于是关闭重联，跳向了主页，这时玩家再进竞技场，刚发(或者还没发)Enter
// 这时收到了上次的比如Update，getBattle之类的数据 那么其实这是脏数据，需要丢掉的，
// 这应该在这一层做处理，上层不用管的。
// 
// 所有我们应该设置这样的的一个变量，只有发送了Enter，然后再收到了Rsp之后的Update，getBattle之类的数据才是可信的。
// 于是就有了这个变量来控制数据的接受或者抛弃.
//
// jingJiChang_EnterSend : 是否发了Enter命令，再调用了cmdAreanEnter就是为true
//						   在收到rsp，就置为false
// 
// jingJiChang_EnterSuccess: 是否收到Enter的正确回应，在dealWithAreanEnterRsp时就为true
//						   在cmdAreanEnter时置为false
// 
// 疑惑1：如果第二次进来刚发了Enter，但是收到了上次的Rsp，那么怎么办呢？？
// 解答1：这个情况不可能，因为服务器是这样处理的：
//		 有请求的回应是直接拿请求的Socket来回应的，比如发了Enter，那么服务器就是直接对收到数据的Socket写数据
//		 所以，如果你上次发送了数据，没有收到，然后断开重联，那么新的链接是不可能收到上次链接的请求回应的，因为Socket已经变了。
//		 对于，没有请求的回应，即服务器主动发送比如Update数据就是服务器主动发的，那么他的做法是在连接池里面找到对应的账号的Socket
//		 然后发送数据，这就跟Socket无关
//
// 只要UI层在需要发命令的时候监听消息，那么就不会有问题！
static bool jingJiChang_EnterSend = false;		// 是否发了Enter，cmdAreanEnter时为true，dealWithAreanEnterRsp时为false
static bool jingJiChang_EnterSuccess = false;	// 是否收到正确回应,cmdAreanEnter时为false，dealWithAreanEnterRsp时为true

static bool tuanDuiBoss_EnterSend = false;	
static bool tuanDuiBoss_EnterSuccess = false;

void saveName_Psw(string name,string psw)
{
	loginedName = name;
	loginedPsw = psw;
}
void save_main_id(unsigned int _id)
{
	login_choosed_main_hero = _id;
}


#define  MSG_TYPE_CONN_ERROR			0
#define  MSG_TYPE_CONN_SUCC				1
#define  MSG_TYPE_SEND_ERROR			2
#define  MSG_TYPE_RECV_ERROR			3
#define  MSG_TYPE_UI_SEND				4
#define  MSG_TYPE_RECV_DATA				5


#pragma pack(1) //按照单字节对齐,跟协议要一致
struct WildcardError
{
	int err_code;
};
#pragma pack() //结束
#define POST_ERROR(ERROR_NUMBER,MSG_STRING) \
	WildcardError rsp; \
	rsp.err_code = ERROR_NUMBER; \
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_STRING,MSG_Rsp::createMsg(&rsp));

// 装备背包满了，就返回
#define CHECK_PKG(EQUIP_ADD_NUM,MSG_STRING) \
do{ \
	if (!EquipmentManager::getManager()->isSpareEnoughToSaveEquip(EQUIP_ADD_NUM)) \
	{ \
		POST_ERROR(CEC_NoMoreFreeSpaceToStoreEquip,MSG_STRING) \
		return; \
	} \
} while (false);

CmdHelper::CmdHelper(void)
{
	// 创建一个心跳包
	{
		unsigned int dataLength = sizeof(SPCmd_HeartBeat);
		SimpleProtocolCmd cmd = heartBeat;
		char* head = newHead(cmd,dataLength);
		char* bodyChar = new char[dataLength];
		g_HeartBeatMsg = new RequestMessage(head,sizeof(SPHead),bodyChar,dataLength);
	}

	m_SystemAlertNum = 0;

	addObserver(SEL_CallFuncO(&CmdHelper::loginCallBack),MSG_clientLoginRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::reconnectedCallback),MSG_reconnectRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::registerCallback),MSG_clientRegistRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::equipCallBack),MSG_equipZhuangbeiRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::createAHeroCallback),MSG_createHeroRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::createAAssitCallback),MSG_createAssitRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::getHerosInTeamCallBack),MSG_getHerosInTeamRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::getZhuangbeiInTeamCallback),MSG_getZhuangbeiInTeamRsp_F);
	addObserver(SEL_CallFuncO(&CmdHelper::adjustFormationCallback),MSG_adjustFormationRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::getTaskListCallback),MSG_getTaskListRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::getItemsInTeamCallback),MSG_getItemsInTeamRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdZBQianghuaCallback),MSG_ZBQianghuaRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdZBQianghuaHuituiCallback),MSG_ZBQianghuaHuituiRsp);
	//addObserver(SEL_CallFuncO(&CmdHelper::cmdZBZhuankongCallback),MSG_ZBZhuankongRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdZBChaijieCallback),MSG_ZBChaijieRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdZBXiangBaoshiCallback),MSG_ZBXiangBaoshiRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdZBChaiBaoshiCallback),MSG_ZBChaiBaoshiRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdBaoshiHechengCallback),MSG_BaoshiHechengRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetMarketInfoCallBack),MSG_getMarketInfoRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdBuyItemInMarketCallBack),MSG_buyItemInMarketRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdBattleCallBack),MSG_BattleRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetTaskChessBoardCallback),MSG_GetTaskChessBoardRsp);//MSG_restoreVitalityRsp
	addObserver(SEL_CallFuncO(&CmdHelper::cmdRestoreVitalityCallback),MSG_restoreVitalityRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdBeKickOutRsp),MSG_beKickOutRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdHeartBeatRsp),MSG_heartBeatRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdArenaEnterRsp),MSG_arenaEnterRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdArenaUpdateRoomRsp),MSG_arenaUpdateRoomRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdArenaGetBattleScriptAndGiftRsp),MSG_arenaGetBattleScriptAndGiftRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdArenaGetGiftRsp),MSG_arenaGetGiftRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTeamBossEnterRsp),MSG_teamBossEnterRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTeamBossUpdateRoomRsp),MSG_teamBossUpdateRoomRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTeamBossGetBattleScriptAndGiftRsp),MSG_teamBossGetBattleScriptAndGiftRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTeamBossGetGiftRsp),MSG_teamBossGetGiftRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdCityGuardEnterRsp),MSG_cityGuardEnterRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdCityGuardGetNextMonsterRsp),MSG_cityGuardGetNextMonsterRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdCityGuardFightRsp),MSG_cityGuardFightRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryAnotherTeamInfoRsp),MSG_queryAnotherTeamInfoRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTopHeroGetListRsp),MSG_topHeroGetListRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTopHeroChallengeRsp),MSG_topHeroChallengeRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdSystemGiftQueryRsp),MSG_systemGiftQueryRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdSystemGiftGetGiftRsp),MSG_systemGiftGetGiftRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdchargeVitalityRsp),MSG_chargeVitalityRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdDuanzaoListQueryRsp),MSG_DuanzaoListQueryRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdDuanzaoRsp),MSG_DuanzaoRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdDuanzaoGroupZBQueryRsp),MSG_DuanzaoGroupZBQueryRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryAllSuitNameRsp),MSG_QueryAllSuitNameRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdZBCuiquRsp),MSG_ZBCuiquRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdZBFumoRsp),MSG_ZBFumoRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryAllMofaJinghuaRsp),MSG_QueryAllMofaJinghuaRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryFubenDiaoluoRsp),MSG_QueryFubenDiaoluoRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdNewAssistantJoinRsp),MSG_NewAssistantJoinRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQuerySubTaskListRsp),MSG_QuerySubTaskListRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdNewSubTaskGotRsp),MSG_newSubTaskGotRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdSubTaskSubmitedRsp),MSG_subTaskSubmitedRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryMainTaskRewardRsp),MSG_QueryMainTaskRewardRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdChargeGoldRsp),MSG_ChargeGoldRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryTeamSkillListRsp),MSG_QueryTeamSkillListRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdEquipSkillRsp),MSG_EquipSkillRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdActorLearnSkillRsp),MSG_ActorLearnSkillRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdChangeTaskStateCallback),MSG_ChangeTaskStatRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryAllShenbingListRsp),MSG_QueryAllShenbingListRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryTeamShenbingSuipianRsp),MSG_QueryTeamShenbingSuipianRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdShenbingHechengRsp),MSG_ShenbingHechengRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryTeamItemSuipianRsp),MSG_QueryTeamItemSuipianRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdItemHechengRsp),MSG_ItemHechengRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdSaodangFubenRsp),MSG_saodangFubenRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdHeroLevelUpRsp),MSG_heroLevelUpRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdHeroUpgradeRsp),MSG_heroUpgradeRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdShenbingJieFengyinRsp),MSG_shenbingJieFengyinRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdShenbingJiandingRsp),MSG_shenbingJiandingRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdZBJinglianRsp),MSG_ZBJinglianRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdShenbingZhuankeJingwenRsp),MSG_shenbingZhuankeJingwenRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryArenaTeamBasicInfoRsp),MSG_queryArenaTeamBasicInfoRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryArenaMarketRsp),MSG_queryArenaMarketRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdRefreshArenaChlgListRsp),MSG_refreshArenaChlgList_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryArenaChlgListRsp),MSG_queryArenaChlgListRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdArenaChallengeRsp),MSG_arenaChallengeRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdArenaMarketBuyRsp),MSG_arenaMarketBuyRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdArenaBuyChallengeTimeRsp),MSG_arenaBuyChlgTimeRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryZahuopuRsp),MSG_queryZahuopuRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdRefreshZahuopuRsp),MSG_refreshZahuopuRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdZahuopuGetOneItemRsp),MSG_zahuopuGetOneItemRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryHellTaskHeroInfoRsp),MSG_queryHellTaskHeroInfoRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdHellTaskBattleRsp),MSG_hellTaskBattleRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryYuanshenOfHeroRsp),MSG_queryYuanshenOfHeroRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdChurchLevelUpRsp),MSG_churchLevelUpRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdSkillLevelUpRsp),MSG_skillLevelUpRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetZhuangbeiAlbumRsp),MSG_getZhuangbeiAlbumRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdCityGuardProcessNotifyRsp),MSG_cityGuardProcessNotifyRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetCityGuardCurrentStatRsp),MSG_getCityGuardCurrentStatRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdCityGuardBattleRsp),MSG_cityGuardBattleRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetCityGuardLastDayReportRsp),MSG_getCityGuardLastDayReportRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetMyCityGuardRewardRsp),MSG_getMyCityGuardRewardRsp);//
	//20140613 奖励系统变更（奖励与悬赏任务合并）
// 	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryDailyLoginBonusRsp),MSG_queryDailyLoginBonusRsp_F);//
// 	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryLevelBonusRsp),MSG_queryLevelBonusRsp_F);//
// 	addObserver(SEL_CallFuncO(&CmdHelper::cmdQuerySystemBonusRsp),MSG_querySystemBonusRsp_F);//
// 	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetDailyLoginBonusRsp),MSG_getDailyLoginBonusRsp_F);//
// 	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetLevelBonusRsp),MSG_getLevelBonusRsp_F);//
// 	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetSystemBonusRsp),MSG_getSystemBonusRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryBonusRsp),MSG_queryBonusRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetBonusRsp),MSG_getBonusRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdSystemPushBonusRsp),MSG_SystemPushBonusRsp);//

//	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetChessRandomBoxRsp),MSG_getChessRandomBoxRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryTaskRateRsp),MSG_queryRateTaskRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetTaskRateBonusRsp),MSG_getRateTaskBonusRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryRateTaskBonusRsp),MSG_queryRateTaskBonus_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryVipPrivilegeRsp),MSG_queryVipPrivilegeRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdUpgradeShenbingRsp),MSG_upgradeShenbingRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryChargePacketRsp),MSG_queryChargePacketRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdUnlockAssistantRsp),MSG_unlockAssistantRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryGiftActivityRsp),MSG_queryGiftActivityRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetGiftActivityRsp),MSG_getGiftActivityRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryKilledMonsterRsp),MSG_queryKilledMonsterRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryKilledMonsterBonusRsp),MSG_QueryKilledMonsterBonusRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetKilledMonsterBonusRsp),MSG_getKilledMonsterBonusRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTopArenaGuessRsp),MSG_topArenaGuessRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTopArenaQueryGuessRsp),MSG_TopArenaQueryGuessRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTopArenaReplayRsp),MSG_topArenaReplay_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTopArenaQueryStatusRsp),MSG_topArenaQueryStatusRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdCastSilverRsp),MSG_CastSilverRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryLegendNoteRsp),MSG_queryLegendNoteRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdChallengeLegendNoteRsp),MSG_challengeLegendNoteRsp_f);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQuerySelfChallengeInfoRsp),MSG_querySelfChallengeInfoRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQuerySelfChallengeOrderRsp),MSG_querySelfChallengeOrderRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdSelfChallengeRsp),MSG_selfChallengeRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdSelfChallengeGainRsp),MSG_selfChallengeGainRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetSelfChallengeBoxRsp),MSG_getSelfChallengeBoxRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryProtectSpiritRsp),MSG_queryProtectSpiritRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdChallengeProtectSpiritRsp),MSG_challengeProtectSpiritRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetProtectSpiritBonusRsp),MSG_getProtectSpiritBonusRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdWorshiProtectSpiritRsp),MSG_worshiProtectSpiritRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdTopHeroClearCDTimeRsp),MSG_TopHeroClearCDTimeRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdDiscoverRsp),MSG_discoverRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryDiscoverInfoRsp),MSG_queryDiscoverInfoRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdUnlockDiscoverSealRsp),MSG_unlockDiscoverSealRsp_F);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQuitCityGuardRsp),MSG_quitCityGuardRsp);//

	addObserver(SEL_CallFuncO(&CmdHelper::cmdFlopTaskGridRsp), MSG_FlopTaskGridRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryPurifyRsp), MSG_QueryPurifyRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdPurifyRsp), MSG_PurifyRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdConfirmPurifyRsp), MSG_ConfirmPurifyRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryAssessWeaponRsp), MSG_QueryAssessWeaponRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdAssessWeaponRsp), MSG_AssessWeaponRsp);//

	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryEmailRsp), MSG_QueryEmailRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdReadEmailRsp), MSG_ReadEmailRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdWriteEmailRsp), MSG_WriteEmailRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdDeleteEmailRsp), MSG_DeleteEmailRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::pushEmailRsp), MSG_SystemEmailPushRsp);//

	addObserver(SEL_CallFuncO(&CmdHelper::cmdQuerySystemGiftBagRsp), MSG_QuerySystemGiftBagRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetSystemGiftBagRsp), MSG_GetSystemGiftBagRsp);//
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryEquipFallRsp), MSG_QueryEquipFallRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQueryHeroOtherInfoRsp), MSG_QueryHeroOtherInfoRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdQuerySystemNotifyInfoRsp), MSG_QueryNotifyInfoRsp);
	//教学
	addObserver(SEL_CallFuncO(&CmdHelper::cmdGetGuideStateRsp), MSG_GetGuideStateRsp);
	addObserver(SEL_CallFuncO(&CmdHelper::cmdSetGuideStateRsp), MSG_SetGuideStateRsp);

}


CmdHelper::~CmdHelper(void)
{
	int msg_num = msg_vec.size();
	for (int i=0;i<msg_num;i++)
	{
		removeObserver(msg_vec[i].c_str());
	}
	msg_vec.clear();
}



CmdHelper* CmdHelper::getHelper()
{
	if (helper == NULL)
	{
		helper = new CmdHelper();
	}
	return helper;
}

void CmdHelper::setSystemAlert(int alertNum /* = 1 */)
{
	m_SystemAlertNum = alertNum;
}

// 返回是否还可以往下走正常流程
bool CmdHelper::reloadCheck(int err_code,SimpleProtocolCmd cmd,const char* post_msg, CCObject * postData)
{
	if (state_enable && isInGame)
	{
		if (mCurrentState == STATE_MODE_RELOAD)
		{
			if (err_code == Success)
			{
				if (cmd == endedReloadCmd)
				{// 当前命令是reload的最后一个命令
					CCAssert(post_msg != NULL,"");
					CCAssert(postData != NULL,"");
					CCNotificationCenter::sharedNotificationCenter()->postNotification(post_msg,postData);
					jumpToMain_reload();
					return false;
				}
			}
			else
			{
				jumpToIndex_error(); // 并关闭状态机
				return false;
			}
		}
	}

	// 是登陆的最后一个命令，就可以开启状态机了
	if (cmd == endedReloadCmd)
		state_enable = true;

	return true;
}

string CmdHelper::getMessage(int errorCode)
{
	string alert_msg_body = "";

	// 这期间的都是道具不够的错误CEC_ItemNotEnough_Begin+ItemsType_Gold就是金币不足
	if (errorCode > CEC_ItemNotEnough_Begin && errorCode < CEC_ItemNotEnough_End)
	{
		int itemType = errorCode-CEC_ItemNotEnough_Begin;
		alert_msg_body = string("[")+CS::getItemName(itemType) +string("]")+ LF::getString("ItemNotEnough");

		return alert_msg_body;
	}

	if (errorCode > CEC_EquipOnlyCanUseForProfesion_Begin && errorCode < CEC_EquipOnlyCanUseForProfesion_End)
	{
		int profession = errorCode - CEC_EquipOnlyCanUseForProfesion_Begin;
		HeroFixedPro pro = BaseActorPropertyParser::getParser()->getExtraProerty((Profession)profession);
		char temp[256];
		sprintf(temp,LF::getString("CEC_EquipOnlyCanUseForProfesion_x").c_str(),pro.nameInChinese.c_str());
		alert_msg_body = temp;
		return alert_msg_body;
	}

	switch (errorCode)
	{
	case Success:
		break;
	case Error:
		alert_msg_body = LF::getString("shujucuowu");
		break;
	case NotLogin:
		alert_msg_body = LF::getString("haimeidenglu");
		break;
	case AccountOrPswErr:
		alert_msg_body = LF::getString("yonghumingHuoMiMaCuoWu");
		break;
	case AccountAlreadyExist:
		alert_msg_body = LF::getString("AccountAlreadyExist");
		break;
	case TiliNotEnought:
		alert_msg_body = LF::getString("tilibugou");
		break;
	case CEC_GoldNotEnough:
		alert_msg_body = LF::getString("suoxujinbibuzu");
		break;
	case CEC_SilverNotEnough:
		alert_msg_body = LF::getString("suoxuyinbibuzu");
		break;
	case CEC_BaoShiNotEnough:
		alert_msg_body = LF::getString("baoshibuzu");
		break;
	case CEC_CaiLiaoNotEnough:
		alert_msg_body = LF::getString("cailiaobuzu");
		break;
	case CEC_ExpNotEnough:
		alert_msg_body = LF::getString("jinyanbuzu");
		break;
	case CEC_HeroUpdateLevelItemNotEnough:
		alert_msg_body = LF::getString("CEC_HeroUpdateItemNotEnough");
		break;
	case CEC_HeroUpdateStageItemNotEnough:
		alert_msg_body = LF::getString("CEC_HeroUpdateStageItemNotEnough");
		break;
	case CEC_Shenbing_JiefengyinItemNotEnough:
		alert_msg_body = LF::getString("CEC_Shenbing_JiefengyinItemNotEnough");
		break;
	case CEC_Shenbing_JiandingItemNotEnough:
		alert_msg_body = LF::getString("CEC_Shenbing_JiandingItemNotEnough");
		break;
	case CEC_CanNotJingLian:
		alert_msg_body = LF::getString("CEC_CanNotJingLian");
		break;
	case CEC_CanNotZhuanke:
		alert_msg_body = LF::getString("CEC_CanNotZhuanke");
		break;
	case CEC_JingwenZhuankeItemNotEnough:
		alert_msg_body = LF::getString("CEC_JingwenZhuankeItemNotEnough");
		break;
	case CEC_NoArenaClgTime:
		alert_msg_body = LF::getString("CEC_NoArenaClgTime");
		break;
	case CEC_ArenaMarketBuyJiFenNotEnough:
		alert_msg_body = LF::getString("CEC_ArenaMarketBuyJiFenNotEnough");
		break;
	case CEC_NoArenaBuyClgTime:
		alert_msg_body = LF::getString("CEC_NoArenaBuyClgTime");
		break;
	case CEC_ZahuopuGetOneItem_NoItem:
		alert_msg_body = LF::getString("CEC_ZahuopuGetOneItem_NoItem");
		break;
	case CEC_HeroLevelNotEnough:
		alert_msg_body = LF::getString("CEC_HeroLevelNotEnough");
		break;
	case CEC_NoHellBattleTime:
		alert_msg_body = LF::getString("CEC_NoHellBattleTime");
		break;
	case CEC_HeroIsMaxStage:
		alert_msg_body = LF::getString("CEC_HeroIsMaxStage");
		break;
	case CEC_NeedUpLevelJiangTang:
		alert_msg_body = LF::getString("CEC_NeedUpLevelJiangTang");
		break;
	case CEC_HeroIsMaxLevel:
		alert_msg_body = LF::getString("CEC_HeroIsMaxLevel");
		break;
	case CEC_NeedUpStageFirst:
		alert_msg_body = LF::getString("CEC_NeedUpStageFirst");
		break;
	case CEC_YuanshenUpstageNeed5Mode:
		alert_msg_body = LF::getString("CEC_YuanshenUpstageNeed5Mode");
		break;
	case CEC_JiaoTangIsMaxLevel:
		alert_msg_body = LF::getString("CEC_JiaoTangIsMaxLevel");
		break;
	case CEC_SkillIsMaxLevel:
		alert_msg_body = LF::getString("CEC_SkillIsMaxLevel");
		break;
	case CEC_CDTimeIsNotOver:
		alert_msg_body = LF::getString("CEC_CDTimeIsNotOver");
		break;
	case CEC_WorldBossIsNotOpen:
		alert_msg_body = LF::getString("CEC_WorldBossIsNotOpen");
		break;
	case CEC_NoReward:
		alert_msg_body = LF::getString("CEC_NoReward");
		break;
	case CEC_NoByTiLiTime:
		alert_msg_body = LF::getString("CEC_NoByTiLiTime");
		break;
	case CEC_NoSaoDangTime:
		alert_msg_body = LF::getString("CEC_NoSaoDangTime");
		break;
	case CEC_SaoDangTimeNotEnough:
		alert_msg_body = LF::getString("CEC_SaoDangTimeNotEnough");
		break;
	case CEC_SaoDangTimeMustBigger0:
		alert_msg_body = LF::getString("CEC_SaoDangTimeMustBigger0");
		break;
	case CEC_RewardIsAlreadyGetted:
		alert_msg_body = LF::getString("CEC_RewardIsAlreadyGetted");
		break;
	case CEC_NotShengBing:
		alert_msg_body = LF::getString("CEC_NotShengBing");
		break;
	case CEC_ShengBingIsMaxLevel:
		alert_msg_body = LF::getString("CEC_ShengBingIsMaxLevel");
		break;
	case CEC_ShengBingNextLevelIsTooHight:
		alert_msg_body = LF::getString("CEC_ShengBingNextLevelIsTooHight");
		break;
	case CEC_CanNotUseThisEquipToLevelUpShenBing:
		alert_msg_body = LF::getString("CEC_CanNotUseThisEquipToLevelUpShenBing");
		break;
	case CEC_EquipIsOnHero:
		alert_msg_body = LF::getString("CEC_EquipIsOnHero");
		break;
	case CEC_TiLiIsFull:
		alert_msg_body = LF::getString("CEC_TiLiIsFull");
		break;
	case CEC_NullPointerException:
		alert_msg_body = LF::getString("CEC_NullPointerException");
		break;
	case CEC_RuntimeException:
		alert_msg_body = LF::getString("CEC_RuntimeException");
		break;
	case CEC_ActorIsAlreadyUnlocked:
		alert_msg_body = LF::getString("CEC_ActorIsAlreadyUnlocked");
		break;
	case CEC_ActorIsDisableToUnlock:
		alert_msg_body = LF::getString("CEC_ActorIsDisableToUnlock");
		break;
	case CEC_RongYuDianJiFenBuGou:
		alert_msg_body = LF::getString("CEC_RongYuDianJiFenBuGou");
		break;
	case CEC_CanNotCaiJieChuanQi:
		alert_msg_body = LF::getString("CEC_CanNotCaiJieChuanQi");
		break;

	case CEC_TiLiIsGetted:
		alert_msg_body = LF::getString("CEC_TiLiIsGetted");
		break;
	case CEC_GettedTiLiTimeIsOver:
		alert_msg_body = LF::getString("CEC_GettedTiLiTimeIsOver");
		break;
	case CEC_GettedTiLiTimeIsNotOpen:
		alert_msg_body = LF::getString("CEC_GettedTiLiTimeIsNotOpen");
		break;
	case CEC_KilledPutongNotEnough:
		alert_msg_body = LF::getString("CEC_KilledPutongNotEnough");
		break;
	case CEC_KilledJingYinNotEnough:
		alert_msg_body = LF::getString("CEC_KilledJingYinNotEnough");
		break;
	case CEC_NoCastSilverTime:
		alert_msg_body = LF::getString("CEC_NoCastSilverTime");
		break;
	case CEC_CanNotJingCai_NotInRound:
		alert_msg_body = LF::getString("CEC_CanNotJingCai_NotInRound");
		break;
	case CEC_CanNotJingCai_AlreadyGuessed:
		alert_msg_body = LF::getString("CEC_CanNotJingCai_AlreadyGuessed");
		break;
	case CEC_DFDJ_NotInited:
		alert_msg_body = LF::getString("CEC_DFDJ_NotInited");
		break;
	case CEC_CanNotJingCai_HaveNotMatch:
		alert_msg_body = LF::getString("CEC_CanNotJingCai_HaveNotMatch");
		break;
	case CEC_ShenBinLuTimeOver:
		alert_msg_body = LF::getString("CEC_ShenBinLuTimeOver");
		break;
	case CEC_ShenBinLu_CanNotChlg_HeroIsLocked:
		alert_msg_body = LF::getString("CEC_ShenBinLu_CanNotChlg_HeroIsLocked");
		break;
	case CEC_HeroLevelIsNotEoungh:
		alert_msg_body = LF::getString("CEC_HeroLevelIsNotEoungh");
		break;
	case CEC_DFDJ_HERO_IS_MAX_VALUE:
		alert_msg_body = LF::getString("CEC_DFDJ_HERO_IS_MAX_VALUE");
		break;
	case CEC_CanNotXueZhan:
		alert_msg_body = LF::getString("CEC_CanNotXueZhan");
		break;
	case CEC_XueZhanNotOpen:
		alert_msg_body = LF::getString("CEC_XueZhanNotOpen");
		break;
	case CEC_XueZhanChlgTimeOver:
		alert_msg_body = LF::getString("CEC_XueZhanChlgTimeOver");
		break;
	case CEC_PPT_NotEnough:
		alert_msg_body = LF::getString("CEC_PPT_NotEnough");
		break;
	case CEC_XueZhanNoBox:
		alert_msg_body = LF::getString("CEC_XueZhanNoBox");
		break;
	case CEC_SHZL_CanNotTiaoZhan:
		alert_msg_body = LF::getString("CEC_SHZL_CanNotTiaoZhan");
		break;
	case CEC_SHZL_LevelNotMatch:
		alert_msg_body = LF::getString("CEC_SHZL_LevelNotMatch");
		break;
	case CEC_SHZL_SAFE:
		alert_msg_body = LF::getString("CEC_SHZL_SAFE");
		break;
	case CEC_SHZL_CanNotWarshipSelf:
		alert_msg_body = LF::getString("CEC_SHZL_CanNotWarshipSelf");
		break;
	case CEC_SHZL_CanNotWarshipGW:
		alert_msg_body = LF::getString("CEC_SHZL_CanNotWarshipGW");
		break;
	case CEC_SHZL_AreadyWarshiped:
		alert_msg_body = LF::getString("CEC_SHZL_AreadyWarshiped");
		break;
	case CEC_SHZL_CanNotWarshp:
		alert_msg_body = LF::getString("CEC_SHZL_CanNotWarshp");
		break;
	case CEC_SHZL_CanNotTiaoZhanSelf:
		alert_msg_body = LF::getString("CEC_SHZL_CanNotTiaoZhanSelf");
		break;
	case CEC_SHZL_CAN_NOT_GET_REWARD_NOT_THIS_SEASON:
		alert_msg_body = LF::getString("CEC_SHZL_CAN_NOT_GET_REWARD_NOT_THIS_SEASON");
		break;
	case CEC_SHZL_CanNotGetReward:
		alert_msg_body = LF::getString("CEC_SHZL_CanNotGetReward");
		break;
	case CEC_SHZL_NO_REWARD:
		alert_msg_body = LF::getString("CEC_SHZL_NO_REWARD");
		break;
	case CEC_SHZL_MUST_PERFECT_PASS:
		alert_msg_body = LF::getString("CEC_SHZL_MUST_PERFECT_PASS");
		break;
	case CEC_SHZL_CANNOT_CHLG_AGAIN_AFTER_LOSE:
		alert_msg_body = LF::getString("CEC_SHZL_CANNOT_CHLG_AGAIN_AFTER_LOSE");
		break;
	case CEC_NO_TANSUO_TIME:
		alert_msg_body = LF::getString("CEC_NO_TANSUO_TIME");
		break;
	case CEC_CAN_NOT_JIEFENG:
		alert_msg_body = LF::getString("CEC_CAN_NOT_JIEFENG");
		break;
	case CEC_TOPHeroISCDTime:
		alert_msg_body = LF::getString("CEC_TOPHeroISCDTime");
		break;
	case CEC_NoMoreFreeSpaceToStoreEquip:
		alert_msg_body = LF::getString("CEC_NoMoreFreeSpaceToStoreEquip");
		break;

	case CEC_NoPurifyTools:
		alert_msg_body = LF::getString("CEC_NoPurifyTools");
		break;
	case CEC_NoPurifyTimes:
		alert_msg_body = LF::getString("CEC_NoPurifyTimes");
		break;
	case CEC_ActorShangZhen_CannotSub:
		alert_msg_body = LF::getString("CEC_ActorShangZhen_CannotSub");
		break;
	case CEC_EmailContentOutSize:
		alert_msg_body = LF::getString("CEC_EmailContentOutSize");
		break;
		
		
	default:
		{
			char temp[128];
			sprintf(temp,"Unknown Error[%d]",errorCode);
			alert_msg_body = string(temp);
		}
		break;
	}
	return alert_msg_body;

	// 这种写法也行
	//CmdHelper::m_SystemAlertNum = 0;
}

void CmdHelper::try_alert(int errCode)
{
	string alert_msg_title = LF::getString("cuowu");
	string alert_msg_body = getMessage(errCode);

	if (errCode != Success)
	{
		if (m_SystemAlertNum > 0)
		{
			LFMessageBox::alert(alert_msg_title,alert_msg_body);
			m_SystemAlertNum --;
		}
		else
		{
			CCLog("%s : %s",__FUNCTION__,alert_msg_body.c_str());
		}
	}
}

char* CmdHelper::newHead(SimpleProtocolCmd cmd,unsigned int dataLength)
{
	char* headChar = new char[sizeof(SPHead)];
	SPHead* _head = (SPHead*)headChar;

	_head->protocol_tag = SimpleProtocol_Tag_Client;
	_head->protocol_ver = Current_ProtocolVersion;
	_head->cmd = cmd;
	_head->data_len = dataLength;

	//转换为网络字节序
	hton_SPHead(_head);

	return headChar;
}

// 添加消息监听
void CmdHelper::addObserver(SEL_CallFuncO selector,const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,selector,name,NULL);
	msg_vec.push_back(string(name));
}
// 移除消息监听
void CmdHelper::removeObserver(const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,name);
}
void CmdHelper::removeLastBuff()
{
	if (lastCmd.head)
	{
		delete [] lastCmd.head;
		lastCmd.body = NULL;
	}
	if (lastCmd.body)
	{
		delete [] lastCmd.body;
		lastCmd.head = NULL;
	}
}

void CmdHelper::forceCloseNetwork()
{
	NetWorkThreadProc::getProc()->closeNetwork();
}

/**
 *	注入经验,如果已经满级了，就注入给他的元神。
 *
 *	疑问：
 *	对于刚好触发满级。还没有元神的，是否需要自动创建元神？？
 */
static void addExpToActorOrHisYuanShenIfFullLevel(BaseActor* actor,unsigned int exp)
{
	if(actor->getCurrentProperty().level >= ACTOR_MAX_LEVEL)
	{
		//CCAssert(shangzhen_actors[i]->getMyYuanShen()!=NULL,"目前假设30级以后就有元神");
		if (actor->getMyYuanShen())
		{
			unsigned int _exp = actor->getMyYuanShen()->getCurrentProperty().exp_current;
			_exp += exp;
			actor->getMyYuanShen()->setExpInfor(_exp);
		}
		else
		{
			CCLog("\n\n*************************************************************************************");
			CCLog("Error : %s:%d-->actor is in maxlevel [%d] ,but we do not find his yuanshen---bug!",__FILE__,__LINE__,ACTOR_MAX_LEVEL);
			CCLog("*************************************************************************************\n\n");
		}
	}
	else
	{
		unsigned int _exp = actor->getCurrentProperty().exp_current;
		_exp += exp;
		actor->setExpInfor(_exp);
	}
}
// 存放战斗中获得的物品
void CmdHelper::storeAllGettedInFight(vector<FightScript> scripts,bool copyToFightPackage)
{
	int num = scripts.size();
	if (scripts[num-1].endResult.myResult == Result_Win)
	{
		for (int scriptIndex=0;scriptIndex<num;scriptIndex++)
		{
			// 物品
			{
				int wuPinNum = scripts[scriptIndex].endResult.gettedItem.size();
				if ( wuPinNum > 0)
				{
					for (int j = 0;j<wuPinNum;j++)
					{
						ItemInfo item = scripts[scriptIndex].endResult.gettedItem[j];
						WuPinManager::getManager()->addWuPin((ItemsType)item.type,item.count);

						// 放进背包
						if (copyToFightPackage)
						{
							WuPin getted((ItemsType)item.type,item.count);
							FightPackage::getPakage()->put(getted);
						}
					}
				}
			}

			// 装备
			{
				int zhuangbeiNum = scripts[scriptIndex].endResult.gettedZhuangBei.size();
				if (zhuangbeiNum > 0)
				{
					for (int j=0;j<zhuangbeiNum;j++)
					{
						EquipData equip = scripts[scriptIndex].endResult.gettedZhuangBei[j];
						EquipmentManager::getManager()->addOneEquipment(equip);

						// 放进背包
						if (copyToFightPackage)
						{
							FightPackage::getPakage()->put(equip);
						}
					}
				}
			}

			// 装备碎片
			{
				for (int j=0,count=scripts[scriptIndex].endResult.gettedShenBingSuiPian.size(); j<count; j++)
				{
					SuiPianManager::getManager()->addOneShenBingSuiPian(scripts[scriptIndex].endResult.gettedShenBingSuiPian[j]);
				}
			}

			// 道具碎片
			{
				for (int j=0,count=scripts[scriptIndex].endResult.gettedItemSuiPian.size(); j<count; j++)
				{
					SuiPianManager::getManager()->addOneItemSuiPian(scripts[scriptIndex].endResult.gettedItemSuiPian[j]);
				}
			}

			// 能量点
			{
				for (int j=0,count=scripts[scriptIndex].endResult.gettedPowerPoints.size(); j<count; j++)
				{
					XueZhanManager::getManager()->addPowerNum(scripts[scriptIndex].endResult.gettedPowerPoints[j].power);
				}
			}

			// 注入经验
			{
				for (int j=0;j<FormationHeroAcountMax;j++)
				{
					unsigned hId = scripts[scriptIndex].init.myTeam[j].heroId;
					// 有人
					if (hId > 0)
					{
						BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(hId);
						CCAssert(actor != NULL,"");
						addExpToActorOrHisYuanShenIfFullLevel(actor,scripts[scriptIndex].endResult.gettedExp);
					}
				}
			}


			//// 属性数据的修改
			//{
			//	for (int j=0;j<FormationHeroAcountMax;j++)
			//	{
			//		unsigned hId = scripts[i].init.myTeam[j].heroId;
			//		// 有人
			//		if (hId > 0)
			//		{
			//			HeroDetail data = scripts[i].endResult.heroDetailAttrAfterFight[j];
			//			BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(hId);
			//			CCAssert(actor != NULL,"");
			//			actor->refreshBaseProPerty(data);
			//		}
			//	}
			//}

		} // for end
	}
}

void CmdHelper::cmdLogin(const char userName[ClientAccountMaxLen+1],const char password[ClientPswMaxLen+1])
{
	GameDirector::getDirector()->showWaiting();

	//
	{
		saveName_Psw(userName,password);
	}
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::login ---> userName = %s,password = %s",userName,password);
#endif

	removeLastBuff();
	char* head = newHead(clientLogin,sizeof(SPCmd_ClientLogin));
	char* bodyChar = new char[sizeof(SPCmd_ClientLogin)];

	SPCmd_ClientLogin* _login = (SPCmd_ClientLogin*)bodyChar;
	memcpy(_login->client_account,userName,ClientAccountMaxLen+1);
	memcpy(_login->client_psw,password,ClientPswMaxLen+1);

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::login --->send to Server userName = %s,password = %s",_login->client_account,_login->client_psw);
#endif
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,sizeof(SPCmd_ClientLogin));

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = clientLogin;
}

void CmdHelper::loginCallBack(CCObject* msg_rsp)
{
	GameDirector::getDirector()->hideWaiting();

	SPCmd_ClientLoginRsp* rsp = (SPCmd_ClientLoginRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 刷新时间
	ServerTime::setCurrentServerTime(rsp->server_time);
	//needSendReconnectDataWhenConn = true;
	needSendLoginDataWhenConn = true;
	// 有英雄，就到选择英雄界面
	if (rsp->hero_count > 0)
	{
		vector<HeroInfo> infors;
		HeroInfo* inforArray = (HeroInfo*)rsp->hero_data;
		for (int i=0;i<rsp->hero_count;i++)
		{
			infors.push_back(inforArray[i]);
		}
		MainHeroManager::getManager()->init(infors);
	}
}

//
void CmdHelper::cmdReconnect()
{
	GameDirector::getDirector()->showWaiting();

	//// 不移除上次的数据？？？
	removeLastBuff();


	SimpleProtocolCmd cmd = Reconnect;
	char* head = newHead(cmd,sizeof(SPCmd_Reconnect));
	char* bodyChar = new char[sizeof(SPCmd_Reconnect)];

	SPCmd_Reconnect* _login = (SPCmd_Reconnect*)bodyChar;
	memcpy(_login->client_account,loginedName.c_str(),ClientAccountMaxLen+1);
	memcpy(_login->client_psw,loginedPsw.c_str(),ClientPswMaxLen+1);
	_login->main_hero_id = htonl(login_choosed_main_hero);

#if (DATAPOOL_LOG == 1)
	CCLog("%s --->send to Server userName = %s,password = %s,main_id = %u",__FUNCTION__,_login->client_account,_login->client_psw,login_choosed_main_hero);
#endif
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,sizeof(SPCmd_Reconnect));

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
//
void CmdHelper::reconnectedCallback(CCObject* msg_rsp)
{
	if (mCurrentState != STATE_MODE_RELOAD)
	{
		GameDirector::getDirector()->hideWaiting();
	}

	SPCmd_ReconnectRsp* rsp = (SPCmd_ReconnectRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 刷新时间
	ServerTime::setCurrentServerTime(rsp->server_time);
}

//
void CmdHelper::cmdRegist(const char userName[ClientAccountMaxLen+1],const char password[ClientPswMaxLen+1])
{
	GameDirector::getDirector()->showWaiting();

	//
	{
		saveName_Psw(userName,password);
	}

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdRegist ---> userName = %s,password = %s",userName,password);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_Register);

	SimpleProtocolCmd cmd = Register;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_Register* _data = (SPCmd_Register*)bodyChar;
	memcpy(_data->client_account,userName,ClientAccountMaxLen+1);
	memcpy(_data->client_psw,password,ClientPswMaxLen+1);

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdRegist send to server ---> userName = %s,password = %s",_data->client_account,_data->client_psw);
#endif
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::registerCallback(CCObject* msg_rsp)
{
	SPCmd_RegisterRsp* rsp = (SPCmd_RegisterRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 刷新时间
	ServerTime::setCurrentServerTime(rsp->server_time);

	// 必须得清理一下，因为会有如下情况：
	// 在弹出界面登陆了，那么MainHeroManager就有数据了
	// 然后奇葩的是，他不直接进去，而是再打开登陆框再注册一次
	// 然后再进去，那么就会用到上次登陆的数据，就是bug
	vector<HeroInfo> null_vec;
	MainHeroManager::getManager()->init(null_vec);

	// 什么都不用做
	// 因为注册之后就是创建英雄，创建了英雄之后需要调用选择英雄的一系列操作
	// 那些操作就会初始化这些数据的
	//
	return;

	//// 初始化为空
	//vector<BaseActorProperty> null_actor_vec;
	//MyselfManager::getManager()->initManager(null_actor_vec);
	//vector<EquipData> null_equip_vec;
	//EquipmentManager::getManager()->initManager(null_equip_vec);
	//vector<ItemInfo> null_wuPin_vec;
	//WuPinManager::getManager()->init(null_wuPin_vec);
}
static TempEquipZhuangbei lastEquipData;

void CmdHelper::cmdEquipZhuangBei(TempEquipZhuangbei data)
{
	CMD_LOG("CmdHelper::equipZhuangBei --->reported hero count = %d",data.hero_zb_arr.size());
#if (DATAPOOL_LOG == 1)
	int heroNum = data.hero_zb_arr.size();
	for (int i=0;i<heroNum;i++)
	{
		int equipedNum = data.hero_zb_arr[i].zb_id_arr.size();
		unsigned int heroId = data.hero_zb_arr[i].hero_id;

		string _idArrayString = ""; //%d,%d
		for (int j=0;j<equipedNum;j++)
		{
			char equipId[128];
			sprintf(equipId,"%lld",data.hero_zb_arr[i].zb_id_arr[j]);
			_idArrayString += string(equipId) + ",";
		}

		if (equipedNum > 0)
			_idArrayString = _idArrayString.substr(0,_idArrayString.length() - 1); // 去掉最后的","

		char log_char[256];
		sprintf(log_char,"hero[%d]--->{heroId = %u,[%s]}",i,heroId,_idArrayString.c_str());
		CCLog("%s",log_char);
	}

#endif
	// 校验
	{
		int heroNum = data.hero_zb_arr.size();
		for (int i=0;i<heroNum;i++)
		{
			BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(data.hero_zb_arr[i].hero_id);
			for (int equipIndex=0,equipCount=data.hero_zb_arr[i].zb_id_arr.size(); equipIndex<equipCount; equipIndex++)
			{
				Equipment* equip = EquipmentManager::getManager()->getOneEquipment(data.hero_zb_arr[i].zb_id_arr[equipIndex]);
				if (equip->getEquipData().profession_id != 0 && 
					actor->getCurrentProperty_R()->profession != equip->getEquipData().profession_id)
				{
					int errorCode = CEC_EquipOnlyCanUseForProfesion_Begin + equip->getEquipData().profession_id;
					POST_ERROR(errorCode,MSG_equipZhuangbeiRsp);
					return;
				}
			}
		}
	}



	GameDirector::getDirector()->showWaiting();
	lastEquipData = data;
	removeLastBuff();
	unsigned int dataLength = 0;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	// win32 编译器不识别[0]数组，会当做事[1]的大小处理
	dataLength = sizeof(SPCmd_EquipZhuangbei);//-sizeof(HeroAndZhuangbei);// + data.hero_zb_arr.size() * sizeof(HeroAndZhuangbei);
#else
	dataLength = sizeof(SPCmd_EquipZhuangbei) ;//+data.hero_zb_arr.size() * sizeof(HeroAndZhuangbei);
#endif

	// 计算真正的HeroAndZhuangbei的长度
	{
		int hearNum = data.hero_zb_arr.size();
		for (int i=0;i<hearNum;i++)
		{
			dataLength += sizeof(unsigned int) + sizeof(unsigned int); //hero_id + zb_count
			dataLength +=  data.hero_zb_arr[i].zb_id_arr.size() * sizeof(long long); // zb_id_arr的长度
		}
	}

	char* head = newHead(equipZhuangbei,dataLength);
	char* bodyChar = new char[dataLength];

	int changedHeroNum = data.hero_zb_arr.size();
	SPCmd_EquipZhuangbei* _data = (SPCmd_EquipZhuangbei*)bodyChar;
	_data->hero_count = htonl(changedHeroNum);

	HeroAndZhuangbei* p = _data->hero_zb_arr;

	for (int i=0;i<changedHeroNum;i++)
	{
		int zhuangBeiNum = data.hero_zb_arr[i].zb_id_arr.size();
		p->hero_id = htonl(data.hero_zb_arr[i].hero_id);
		p->zb_count = htonl(zhuangBeiNum);
		for (int j=0;j<zhuangBeiNum;j++)
		{
			p->zb_id_arr[j] = LF::lf_htonll(data.hero_zb_arr[i].zb_id_arr[j]);
		}

		p = (HeroAndZhuangbei*) (((char*)p) + sizeof(unsigned int) + sizeof(unsigned int) + data.hero_zb_arr[i].zb_id_arr.size() * sizeof(long long));
	}
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = equipZhuangbei;
}

void CmdHelper::equipCallBack(CCObject* msg_rsp)
{
	SPCmd_EquipZhuangbeiRsp* rsp = (SPCmd_EquipZhuangbeiRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		lastEquipData.hero_zb_arr.clear();
		try_alert(rsp->err_code);
		return;
	}

	int heroNum = lastEquipData.hero_zb_arr.size();
	for (int i=0;i<heroNum;i++)
	{
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(lastEquipData.hero_zb_arr[i].hero_id);
		CCAssert(actor!=NULL,"");
		actor->detatchAllEquipment(); // 移除所有，重新装备

		int equipNum = lastEquipData.hero_zb_arr[i].zb_id_arr.size();
		for (int j=0;j<equipNum;j++)
		{
			Equipment* oneEquip = EquipmentManager::getManager()->getOneEquipment(lastEquipData.hero_zb_arr[i].zb_id_arr[j]);
			CCAssert(oneEquip!=NULL,"");

			actor->attachEquipment(oneEquip);
		}
	}

	lastEquipData.hero_zb_arr.clear();
}

struct CreateHeroData
{
	unsigned int profession_id; 
	string name;
};
CreateHeroData createHeroData;
void CmdHelper::cmdCreateHero(unsigned int profession_id,string name)
{
	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::createAHero ---> profession_id = %d,name = %s",profession_id,name.c_str());
#endif

	{
		createHeroData.profession_id = profession_id;
		createHeroData.name = name;
	}


	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_CreateHero);

	char* head = newHead(createHero,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_CreateHero* _data = (SPCmd_CreateHero*)bodyChar;
	_data->profession_id = htonl(profession_id);
	// 名字复制
	strcpy(_data->name,name.c_str());

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = createHero;
}
void CmdHelper::createAHeroCallback(CCObject* msg_rsp)
{
	SPCmd_CreateHeroRsp* rsp = (SPCmd_CreateHeroRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code == Success)
	{
		HeroInfo infor;
		infor.hero_id = rsp->hero_id;
		infor.level = 1;
		infor.vip_level = 1;
		strcpy(infor.name,createHeroData.name.c_str());
		infor.profession_id = createHeroData.profession_id;
		MainHeroManager::getManager()->addOneHero(infor);
	}else
	{
		try_alert(rsp->err_code);
	}
}

void CmdHelper::cmdCreateAssit(unsigned int hero_id,unsigned int profession_id)
{
	//addObserver(SEL_CallFuncO(&CmdHelper::createAAssitCallback),MSG_createAssitRsp);
	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::createAAssit ---> hero_id = %d,profession_id = %d",hero_id,profession_id);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_CreateAssit);

	char* head = newHead(createAssit,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_CreateAssit* _data = (SPCmd_CreateAssit*)bodyChar;
	_data->profession_id = htonl(profession_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = createAssit;
}

void CmdHelper::createAAssitCallback(CCObject* msg_rsp)
{
	SPCmd_CreateAssitRsp* rsp = (SPCmd_CreateAssitRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	BaseActorProperty pro(&rsp->hero_detail);
	MyselfManager::getManager()->addOneActor(pro);
}

// 
void CmdHelper::cmdGetHerosInTeam(unsigned main_hero_id)
{
	GameDirector::getDirector()->showWaiting();
	//
	{
		save_main_id(main_hero_id);
	}

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdGetHerosInTeam ---> hero_id = %d",main_hero_id);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_GetHerosInTeam);

	char* head = newHead(getHerosInTeam,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_GetHerosInTeam* _data = (SPCmd_GetHerosInTeam*)bodyChar;
	_data->hero_id = htonl(main_hero_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = getHerosInTeam;
}
// 会刷新内部数据
void CmdHelper::getHerosInTeamCallBack(CCObject* msg_rsp)
{
	SPCmd_GetHerosInTeamRsp* rsp = (SPCmd_GetHerosInTeamRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code == Error)
	{
		return;
	}

	needSendReconnectDataWhenConn = true;
	needSendLoginDataWhenConn = false;

	// 初始化数据
	{
		// 教堂
		JiaoTang jiaotang;
		jiaotang.level = rsp->church_level;
		MyselfManager::getManager()->setCurrentJiaoTangData(jiaotang);

		// 队伍
		vector<BaseActorProperty> propertyVec;
		vector<MyselfManager::ActorMapping> mappings;

		HeroDetail* detailArray = rsp->hero_detail_arr;
		for (int i=0;i<rsp->hero_count;i++)
		{
			BaseActorProperty p(&detailArray[i]);
			HeroFixedPro extra = BaseActorPropertyParser::getParser()->getExtraProerty(p.profession);
			p.initFixedProValue(extra);

			propertyVec.push_back(p);

			// 元神，真身的依赖关系
			if (detailArray[i].basic_info.is_yuanshen)
			{
				MyselfManager::ActorMapping zhenShen_Yuanshen;
				zhenShen_Yuanshen.actorId = detailArray[i].basic_info.zhenshen_id;
				zhenShen_Yuanshen.yuanShenId = detailArray[i].basic_info.hero_id;
				CCAssert(zhenShen_Yuanshen.actorId != INVALID_ID,"");

				mappings.push_back(zhenShen_Yuanshen);
			}
		}

		// 初始化角色
		MyselfManager::getManager()->initManager(propertyVec,mappings);
	}

}

void CmdHelper::cmdGetZhuangbeiInTeam(/*unsigned int main_hero_id*/)
{
	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdGetZhuangbeiInTeam ---> hero_id = %d",login_choosed_main_hero);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_GetZhuangbeiInTeam);

	char* head = newHead(getZhuangbeiInTeam,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_GetZhuangbeiInTeam* _data = (SPCmd_GetZhuangbeiInTeam*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = getZhuangbeiInTeam;
}

// 会刷新内部数据
void CmdHelper::getZhuangbeiInTeamCallback(CCObject* f_msg_rsp)
{
	Flat_GetZhuangbeiInTeamRsp* rsp = (Flat_GetZhuangbeiInTeamRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code == Error)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 初始化
	EquipmentManager::getManager()->initManager(rsp->equips);
}

struct AdjustFormationData
{
	unsigned int hero_id_arr[FormationHeroAcountMax];
};

AdjustFormationData adjustData;
void CmdHelper::cmdAdjustFormation(unsigned int hero_id_arr[FormationHeroAcountMax])
{
	//检测
	{
		//上阵助手不能减少
		vector<BaseActor*> acVec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng, LF_UnLock);
		int num=0;
		for (int i=0; i<FormationHeroAcountMax; ++i) {
			if (hero_id_arr[i] > 0) ++num;
		}
		//除主角外，上阵角色不少于3个
		//当酒馆勾选助手角色少于3时，点击 确定，弹出提示框，上阵助手不能少于3人。
		if (num < acVec.size() && num < 4){
			POST_ERROR(CEC_ActorShangZhen_CannotSub,MSG_adjustFormationRsp);
			return;
		}
	}


	//addObserver(SEL_CallFuncO(&CmdHelper::adjustFormationCallback),MSG_adjustFormationRsp);
	GameDirector::getDirector()->showWaiting();

	// 保存数据,用于成功之后进行操作
	{
		for (int i=0;i<FormationHeroAcountMax;i++)
		{
			adjustData.hero_id_arr[i] = hero_id_arr[i];
		}
	}
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdAdjustFormation --->hero_id_arr[%d,%d,%d,%d]",hero_id_arr[0],hero_id_arr[1],hero_id_arr[2],hero_id_arr[3]);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_AdjustFormation);

	char* head = newHead(adjustFormation,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_AdjustFormation* _data = (SPCmd_AdjustFormation*)bodyChar;
	// 这种拷贝有问题
	// memcpy(_data->hero_id_arr,hero_id_arr,FormationHeroAcountMax);

	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		_data->hero_id_arr[i] = htonl(hero_id_arr[i]);
	}

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = adjustFormation;
}
// 会刷新内部数据
void CmdHelper::adjustFormationCallback(CCObject* msg_rsp)
{
	SPCmd_AdjustFormationRsp* rsp = (SPCmd_AdjustFormationRsp*) ((MSG_Rsp*)msg_rsp)->getData();
	rsp->err_code = rsp->err_code;

	if ( rsp->err_code != Success )
	{
		try_alert(rsp->err_code);
		return;
	}
		

	// 根据保存的数据，就进行布阵的修改
	{
		vector<BaseActor*> temps = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
		for (int i=0;i<temps.size();i++)
		{
			temps[i]->setActorBuZhengPos(0);	// 先都取下来
		}

		for (int i=0;i<FormationHeroAcountMax;i++)
		{
			if (adjustData.hero_id_arr[i] == INVALID_ID)
				continue;

			BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(adjustData.hero_id_arr[i]);
			CCAssert(actor!=NULL,"");
			CCAssert(!actor->isYuanShen(),"不可能是元神");
			// 这个索引就是位置
			actor->setActorBuZhengPos(i+1);
		}
	}

}
void CmdHelper::cmdGetTaskList(unsigned int main_hero_id)
{
	GameDirector::getDirector()->showWaiting();
#if (DATAPOOL_LOG == 1)
	CCLog("%s",__FUNCTION__);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_GetTaskList);

	char* head = newHead(getTaskList,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_GetTaskList* _data = (SPCmd_GetTaskList*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = getTaskList;
}

void CmdHelper::getTaskListCallback(CCObject* msg_rsp)
{
	SPCmd_GetTaskListRsp* rsp = (SPCmd_GetTaskListRsp*) ((MSG_Rsp*)msg_rsp)->getData();
	rsp->err_code = ntohl(rsp->err_code);

	if ( rsp->err_code != Success )
	{
		try_alert(rsp->err_code);
		return;
	}
		
	TaskManager::getManager()->initMainTask((TaskId)rsp->maintask_last_actived,(TaskStatus)rsp->maintask_status);

	//vector<SubTaskItem> tempVec;
	//for (int i=0;i<rsp->subtask_count;i++)
	//{
	//	SubTaskItem item(rsp->task_arr[i]);
	//	tempVec.push_back(item);
	//}
	// 初始化
	//TaskManager::getManager()->init((TaskId)rsp->maintask_last_actived,(TaskStatus)rsp->maintask_status,tempVec);
}
// 物品
void CmdHelper::cmdGetItemsInTeam()
{
	GameDirector::getDirector()->showWaiting();

	CMD_LOG("CmdHelper::cmdGetItemsInTeam ---> main_hero_id = %d",login_choosed_main_hero);

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_GetItemsInTeam);

	char* head = newHead(getItemsInTeam,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_GetItemsInTeam* _data = (SPCmd_GetItemsInTeam*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = getItemsInTeam;
}
// 会刷新内部数据
void CmdHelper::getItemsInTeamCallback(CCObject* msg_rsp)
{
	//removeObserver(MSG_getItemsInTeamRsp);
	SPCmd_GetItemsInTeamRsp* rsp = (SPCmd_GetItemsInTeamRsp*)((MSG_Rsp*)msg_rsp)->getData();

	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<ItemInfo> itemInforVec;

	for (int i=0;i<rsp->count;i++)
	{
		ItemInfo temp;
		temp.count = rsp->item_arr[i].count;
		temp.type = rsp->item_arr[i].type;
		itemInforVec.push_back(temp);
	}
	// 初始化
	WuPinManager::getManager()->init(itemInforVec);
}

struct ZBQianghuaData
{
	unsigned long long zhuangBeiId;
	// 请求前的color
	unsigned int zhuangbeiRequestColor;
	// 请求前的level
	unsigned int zhuangbeiRequestLevel;
	bool b_gold_star;
	//// 提升概率的材料数
	//unsigned int usedGaiLvItem;
};
ZBQianghuaData qiangHuaData;

// 强化装备
void CmdHelper::cmdZBQianghua(SPCmd_ZBQianghua request)
{
	// 保存请求状态的数据
	{
		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(request.zhuangbei_id);
		qiangHuaData.zhuangBeiId = request.zhuangbei_id;
		qiangHuaData.zhuangbeiRequestColor = temp->getEquipColor();
		qiangHuaData.zhuangbeiRequestLevel = temp->getLevel();
		qiangHuaData.b_gold_star = request.b_gold_star;

		if (request.b_gold_star && WuPinManager::getManager()->getWuPin(ItemsType_GoldStar_Qianghua)->getCount() < 1)
		{
			int errorCode = ItemsType_GoldStar_Qianghua+CEC_ItemNotEnough_Begin;
			POST_ERROR(errorCode,MSG_ZBQianghuaRsp);
			return;
		}
		
		string infor;
		if (!temp->canBeQianghua(infor))
		{
			CMD_LOG("CmdHelper:%s",infor.c_str());
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();

	CMD_LOG("CmdHelper::cmdZBQianghua ---> zhuangbeiId = %lld,b_gold_star = %s",request.zhuangbei_id,request.b_gold_star?"true":"false");

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ZBQianghua);

	SimpleProtocolCmd cmd = ZBQianghua;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ZBQianghua* _data = (SPCmd_ZBQianghua*)bodyChar;
	_data->zhuangbei_id = LF::lf_htonll(request.zhuangbei_id);
	_data->b_gold_star = request.b_gold_star;
	//_data->gailv_cailiao_count = htonl(request.gailv_cailiao_count);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

// 强化装备
// 会刷新内部数据
void CmdHelper::cmdZBQianghuaCallback(CCObject* msg_rsp)
{
	SPCmd_ZBQianghuaRsp* rsp = (SPCmd_ZBQianghuaRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 减去花费的物品
	{
		if (qiangHuaData.b_gold_star)
		{
			WuPin* temp = NULL;
			temp = WuPinManager::getManager()->getWuPin(ItemsType_GoldStar_Qianghua);
			CCAssert(temp!=NULL && temp->getCount() >= 1 ,NULL);	// 保证不会出错
			temp->setCount(temp->getCount() - 1);
		}
		else
		{
			unsigned int normalCaiLiaoNum = 0;
			unsigned int yinBi;
			yinBi = CS::QianghuaZhuangbeiNeedSilver(qiangHuaData.zhuangbeiRequestLevel,qiangHuaData.zhuangbeiRequestColor);
			normalCaiLiaoNum = CS::ItemsNeedForQianghuaZhuangbei(qiangHuaData.zhuangbeiRequestLevel,qiangHuaData.zhuangbeiRequestColor);

			CCLog("cmdZBQianghuaCallback --> ok ,will sub items");

			WuPin* temp = NULL;
			temp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
			CCAssert(temp!=NULL && temp->getCount() >= yinBi ,NULL);	// 保证不会出错
			temp->setCount(temp->getCount() - yinBi);	// 修改值

			temp = WuPinManager::getManager()->getWuPin(ItemsType_QianghuaNormal);
			CCAssert(temp!=NULL && temp->getCount() >= normalCaiLiaoNum ,NULL);	// 保证不会出错
			temp->setCount(temp->getCount() - normalCaiLiaoNum);	// 修改值
		}
		
	}

	// 强化成功,修改属性
	{
		CCLog("cmdZBQianghuaCallback --> sucess ,up properties!");

		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(qiangHuaData.zhuangBeiId);
		CCAssert(temp!=NULL,"");

		EquipType type = EquipmentManager::getManager()->getEquipType(temp->getType());
		temp->addOneQiangHuaInfor(rsp->qianghua_info);
		/*if (type == EquipType_FangJu)
		{
		temp->setQiangHuaTime(rsp->qianghua_info);
		temp->setHuJia(rsp->hujia);
		}
		else if (type == EquipType_WuQi)
		{
		temp->setQiangHuaTime(rsp->qianghua_time);
		temp->setMinGongJiLi(rsp->gongji_min);
		temp->setMinGongJiLi(rsp->gongji_max);
		}
		else
		{
		CCAssert(false,"不可能");
		}*/
	}
}

static unsigned long long lastHuiTuiZhuangBei_Id = 0;
static unsigned int lastHuiTuiKeepCount = 0;
void CmdHelper::cmdZBQianghuaHuitui(unsigned long long zhuangbeiId,vector<unsigned char> keep)
{
	{
		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(zhuangbeiId);
		unsigned char max_qianghua_time = temp->getEquipData().qianghua_data.max_qianghua_time;

		if (zhuangbeiId <= 0)
		{
			CMD_LOG("cmdZBQianghuaHuitui , zhuangbeiId <=0 ");
			return;
		}
		if (keep.size() > max_qianghua_time-1)
		{
			CMD_LOG("cmdZBQianghuaHuitui , keep.size() > %d ",max_qianghua_time-1);
			return;
		}

		if (temp->getLevel() < Qianghua_zhuangbei_min_level)//
		{
			CMD_LOG("cmdZBQianghuaHuitui , can not hui tui, the level must >= %d,currentLevel = %d",Qianghua_zhuangbei_min_level,temp->getLevel());
			return;
		}

		QianghuaData data = temp->getEquipData().qianghua_data;
		for (int i=0; i<data.qianghua_count; i++)
		{
			if (data.info_arr[i].level == QianghuaLevel_jin_special)
			{
				for (int keepIndex =0,keepCount=keep.size(); keepIndex<keepCount; keepIndex++)
				{
					if (keep[keepIndex] == data.info_arr[i].id)
					{
						CMD_LOG("Waring : cmdZBQianghuaHuitui , client not need to send keep QianghuaLevel_jin_special ");
						CCAssert(false,"UI-BUG??");
					}
				}
			}
		}

		// 要保留的比历史记录的还多，不能访问
		if (temp->getEquipData().qianghua_data.qianghua_count <= keep.size())
		{
			CMD_LOG("cmdZBQianghuaHuitui , you want keeped num[%d] is >= you just have[%d] ,so can not access network!",
				keep.size(),temp->getEquipData().qianghua_data.qianghua_count);
			return;
		}

		int silver_need = CS::HuituiZhuangbeiNeedSilver(temp->getLevel(),temp->getEquipColor());
		int silver_curr = WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount();
		if (silver_curr < silver_need)
		{
			CMD_LOG("cmdZBQianghuaHuitui , can not hui tui, the Silver must >= %d,Silver = %d",silver_need,silver_curr);
			return;
		}
	}
	

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastHuiTuiZhuangBei_Id = zhuangbeiId;
	lastHuiTuiKeepCount = keep.size();

	unsigned int dataLength = sizeof(SPCmd_ZBQianghuaHuitui) + sizeof(unsigned char) * keep.size();

	char* head = newHead(ZBQianghuaHuitui,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ZBQianghuaHuitui* _data = (SPCmd_ZBQianghuaHuitui*)bodyChar;
	_data->zhuangbei_id = LF::lf_htonll(zhuangbeiId);
	_data->keep_count = htonl(keep.size());
	for (int i=0;i<keep.size();i++)
	{
		_data->id_arr[i] = keep[i];
	}

	CMD_LOG("cmdZBQianghuaHuitui , zhuangbeiId = %lld, keep_count = %d, keep = [%d,%d,%d,%d] ",
		zhuangbeiId,keep.size(),_data->id_arr[0],_data->id_arr[1],_data->id_arr[2],_data->id_arr[3]);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = ZBQianghuaHuitui;

}

void CmdHelper::cmdZBQianghuaHuituiCallback(CCObject* msg_rsp)
{
	SPCmd_ZBQianghuaHuituiRsp* rsp = (SPCmd_ZBQianghuaHuituiRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	Equipment* equip = EquipmentManager::getManager()->getOneEquipment(lastHuiTuiZhuangBei_Id);
	if (equip != NULL)
	{
		// 扣钱
		int silver = CS::HuituiZhuangbeiNeedSilver(equip->getLevel(),equip->getEquipColor());
		WuPinManager::getManager()->addWuPin(ItemsType_Silver,-silver);

		//金币
		int gold = CS::HuituiZhuangbeiNeedGold(lastHuiTuiKeepCount);
		WuPinManager::getManager()->addWuPin(ItemsType_Gold,-gold);
		equip->setNewQiangHuaData(rsp->new_qianghua_data);
	}else
	{
		CCAssert(false,"不应该找不到Id");
	}
}



struct ZBZhuanKongData
{
	// id
	unsigned long long zhuangBeiId;
	// 请求前的color
	unsigned int zhuangbeiRequestColor;
	// 请求前的level
	unsigned int zhuangbeiRequestLevel;
};
ZBZhuanKongData zhuanKongData;
// 钻孔
void CmdHelper::cmdZBZhuankong(SPCmd_ZBZhuankong req)
{
	CCAssert(false,"不再有这个命令了....");
	/*
	GameDirector::getDirector()->showWaiting();

	// 保存访问数据
	{
		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(req.zhuangbei_id);
		CCAssert(temp!=NULL,"");
		zhuanKongData.zhuangBeiId = temp->getEquipId();
		zhuanKongData.zhuangbeiRequestColor = temp->getEquipColor();
		zhuanKongData.zhuangbeiRequestLevel = temp->getLevel();
	}
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdZBZhuankong ---> zhuangbeiId = %lld",req.zhuangbei_id);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ZBZhuankong);

	SimpleProtocolCmd cmd = ZBZhuankong;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ZBZhuankong* _data = (SPCmd_ZBZhuankong*)bodyChar;
	_data->zhuangbei_id = LF::lf_htonll(req.zhuangbei_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
	*/
}
//// 钻孔
//void CmdHelper::cmdZBZhuankongCallback(CCObject* msg_rsp)
//{
//	// 没有钻孔了
//	/*
//	SPCmd_ZBZhuankongRsp* rsp = (SPCmd_ZBZhuankongRsp*)((MSG_Rsp*)msg_rsp)->getData();
//	if (rsp->err_code != Success)
//	{
//		try_alert(rsp->err_code);
//		return;
//	}
//
//	CCLog("Make Hole Sucessful!");
//
//	// 花费
//	{
//		unsigned int jinBi = CS::ZhuangkongNeedGold(zhuanKongData.zhuangbeiRequestLevel,zhuanKongData.zhuangbeiRequestColor);
//		WuPin* temp = NULL;
//
//		temp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
//		CCAssert(temp!=NULL && temp->getCount() >= jinBi,"");
//		temp->setCount(temp->getCount() - jinBi);
//	}
//
//	// 属性
//	{
//		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(zhuanKongData.zhuangBeiId);
//		CCAssert(temp!=NULL,"");
//		temp->holeNumAdd();
//	}
//	*/
//}

struct ZBChaijieData
{
	// id
	unsigned long long zhuangBeiId;
	// 请求前的color
	unsigned int zhuangbeiRequestColor;
	// 请求前的level
	unsigned int zhuangbeiRequestLevel;
};
//ZBChaijieData caiJieData;
vector<ZBChaijieData> caijieData;
// 拆解
void CmdHelper::cmdZBChaijie(vector<long long> equips_id_vec)
{
#if (DATAPOOL_LOG == 1)
	char* temp = new char[64*equips_id_vec.size()];
	memset(temp,0,sizeof(temp));
	sprintf(temp,"%s:",__FUNCTION__);
	for (int i=0, equipNum = equips_id_vec.size(); i<equipNum; i++)
	{
		strcat(temp,CCString::createWithFormat("-- zhuangbeiId = %lld -- ",equips_id_vec[i])->getCString());
	}
	CCLog(temp);
	delete [] temp;
#endif

	// 校验钱币
	{
		int silverNeed = 0;
		int num = equips_id_vec.size();
		for (int i=0;i<num;i++)
		{
			Equipment* temp = EquipmentManager::getManager()->getOneEquipment(equips_id_vec[i]);
			CCAssert(temp!=NULL,"");
			if (temp != NULL)
			{
				if (temp->getActorId() != INVALID_ID)
				{
					POST_ERROR(CEC_EquipIsOnHero,MSG_ZBChaijieRsp);
					return;
				}
				if (temp->getEquipColor() >= ZhuangbeiColour_Chuanqi)
				{
					POST_ERROR(CEC_CanNotCaiJieChuanQi,MSG_ZBChaijieRsp);
					return;
				}
			}
			silverNeed += CS::ChaijieZhuangbeiNeedSilver(temp->getLevel(),temp->getEquipColor());
		}
	
		if (WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount() < silverNeed)
		{
			POST_ERROR(CEC_SilverNotEnough,MSG_ZBChaijieRsp);
			return;
		}
	}


	GameDirector::getDirector()->showWaiting();
	CCAssert(!equips_id_vec.empty(),"必须要有装备");

	// 保存访问数据
	{
		caijieData.clear();
		int num = equips_id_vec.size();
		for (int i=0;i<num;i++)
		{
			Equipment* temp = EquipmentManager::getManager()->getOneEquipment(equips_id_vec[i]);
			CCAssert(temp!=NULL,"");
			ZBChaijieData data;
			data.zhuangBeiId = temp->getEquipId();
			data.zhuangbeiRequestColor = temp->getEquipColor();
			data.zhuangbeiRequestLevel = temp->getLevel();

			caijieData.push_back(data);
		}
	}

	removeLastBuff();
	int num = equips_id_vec.size();
	unsigned int dataLength = sizeof(SPCmd_ZBChaijie);
	dataLength += num * sizeof(unsigned long long);
	SimpleProtocolCmd cmd = ZBChaijie;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	
	SPCmd_ZBChaijie* _data = (SPCmd_ZBChaijie*)bodyChar;
	_data->zb_count = htonl(num);
	for (int i=0;i<num;i++)
	{
		_data->zb_id_arr[i] = LF::lf_htonll(equips_id_vec[i]);
	}
	
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
// 拆解
void CmdHelper::cmdZBChaijieCallback(CCObject* msg_rsp)
{
	//removeObserver(MSG_ZBChaijieRsp);

	Flat_ZBChaijieRsp* rsp = (Flat_ZBChaijieRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	int num = caijieData.size();
	unsigned int yinbi = 0;
	for (int i=0;i<num;i++)
	{
		// 花费
		{
			yinbi += CS::ChaijieZhuangbeiNeedSilver(caijieData[i].zhuangbeiRequestLevel,caijieData[i].zhuangbeiRequestColor);
			WuPin* temp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
			CCAssert(temp!=NULL && temp->getCount() >= yinbi,"");
			temp->setCount(temp->getCount() - yinbi);
		}

		// 销毁拆解的装备
		{
			EquipmentManager::getManager()->destoryOneEquip(caijieData[i].zhuangBeiId);
		}
	}

	// 添加得到的物品
	{
		for (int i=0;i<rsp->item_arr.size();i++)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)rsp->item_arr[i].type,rsp->item_arr[i].count);
		}
		for (int i=0;i<rsp->re_item_arr.size();i++)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)rsp->re_item_arr[i].type,rsp->re_item_arr[i].count);
		}
	}

	// 重新准备下数据，把type归类
/*	vector<ItemInfo> groupedVec;
	for (int i=0;i<rsp->count;i++)
	{
		bool newFound = true;

		int groupedNum = groupedVec.size();
		for (int j=0;j<groupedNum;j++)
		{
			if (groupedVec[j].type == rsp->item_arr[i].type)
			{
				newFound = false;
				groupedVec[j].count += rsp->item_arr[i].count;
				break;
			}
		}

		if (newFound)
		{
			groupedVec.push_back(rsp->item_arr[i]);
		}
	}

	rsp->count = groupedVec.size();
	for (int i = 0; i < rsp->count; i++)
	{
		rsp->item_arr[i] = groupedVec[i];
	}*/
}

struct ZBXiangBaoShiData 
{
	// id
	unsigned long long zhuangBeiId;
	unsigned int baoShiLeiXing;
	// 请求前的level
	unsigned int zhuangbeiRequestLevel;
};
ZBXiangBaoShiData xiangBaoshiData;

// 镶宝石
void CmdHelper::cmdZBXiangBaoshi(SPCmd_ZBXiangBaoshi req)
{
	//addObserver(SEL_CallFuncO(&CmdHelper::cmdZBXiangBaoshiCallback),MSG_ZBXiangBaoshiRsp);
	GameDirector::getDirector()->showWaiting();

	// 保存访问数据
	{
		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(req.zhuangbei_id);
		CCAssert(temp!=NULL,"");
		xiangBaoshiData.zhuangBeiId = temp->getEquipId();
		xiangBaoshiData.baoShiLeiXing = req.baoshi_type;
		xiangBaoshiData.zhuangbeiRequestLevel = temp->getLevel();
	}


#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdZBXiangBaoshi --->zhuangbeiId = %lld,baoShiType = %d",req.zhuangbei_id,req.baoshi_type);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ZBXiangBaoshi);

	SimpleProtocolCmd cmd = ZBXiangBaoshi;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ZBXiangBaoshi* _data = (SPCmd_ZBXiangBaoshi*)bodyChar;
	_data->zhuangbei_id = LF::lf_htonll(req.zhuangbei_id);
	_data->baoshi_type = htonl(req.baoshi_type);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
// 镶宝石
void CmdHelper::cmdZBXiangBaoshiCallback(CCObject* msg_rsp)
{
	//removeObserver(MSG_ZBXiangBaoshiRsp);
	SPCmd_ZBXiangBaoshiRsp* rsp = (SPCmd_ZBXiangBaoshiRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 扣物品
	{
		unsigned int yinbi = CS::XiangbaoshiNeedSilver(xiangBaoshiData.zhuangbeiRequestLevel,xiangBaoshiData.baoShiLeiXing);

		WuPin* temp = NULL;

		temp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
		CCAssert(temp!=NULL && temp->getCount() >= yinbi,"");
		temp->setCount(temp->getCount() - yinbi);

		// 去掉一个宝石
		temp = WuPinManager::getManager()->getWuPin((ItemsType)xiangBaoshiData.baoShiLeiXing);
		CCAssert(temp!=NULL && temp->getCount() >= 1,"");
		temp->setCount(temp->getCount()-1);
	}

	// 改属性
	{
		Equipment* item = EquipmentManager::getManager()->getOneEquipment(xiangBaoshiData.zhuangBeiId);
		item->addOneBaoShi((ItemsType)xiangBaoshiData.baoShiLeiXing);
	}

}

struct ZBCaiBaoShiData
{
	// id
	unsigned long long zhuangBeiId;
	// 最多3个宝石
	unsigned int baoShiLeiXing1;
	unsigned int baoShiLeiXing2;
	unsigned int baoShiLeiXing3;
	// 请求前的level
	unsigned int zhuangbeiLevel;
};
ZBCaiBaoShiData caiBaoShiData;

// 拆宝石,全拆！
void CmdHelper::cmdZBChaiBaoshi(SPCmd_ZBChaiBaoshi req)
{
	//addObserver(SEL_CallFuncO(&CmdHelper::cmdZBChaiBaoshiCallback),MSG_ZBChaiBaoshiRsp);
	GameDirector::getDirector()->showWaiting();

	// 保存访问数据
	{
		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(req.zhuangbei_id);
		CCAssert(temp!=NULL,"");
		caiBaoShiData.zhuangBeiId = temp->getEquipId();
		caiBaoShiData.zhuangbeiLevel= temp->getLevel();
		caiBaoShiData.baoShiLeiXing1 = temp->getEquipData().diamond_type_1;
		caiBaoShiData.baoShiLeiXing2 = temp->getEquipData().diamond_type_2;
		caiBaoShiData.baoShiLeiXing3 = temp->getEquipData().diamond_type_3;
	}

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdZBChaiBaoshi --->zhuangbeiId = %lld",req.zhuangbei_id);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ZBChaiBaoshi);

	SimpleProtocolCmd cmd = ZBChaiBaoshi;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ZBChaiBaoshi* _data = (SPCmd_ZBChaiBaoshi*)bodyChar;
	_data->zhuangbei_id = LF::lf_htonll(req.zhuangbei_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
// 拆宝石
void CmdHelper::cmdZBChaiBaoshiCallback(CCObject* msg_rsp)
{
	//removeObserver(MSG_ZBChaiBaoshiRsp);

	SPCmd_ZBChaiBaoshiRsp* rsp = (SPCmd_ZBChaiBaoshiRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 扣钱，还宝石
	{
		unsigned int yinBi = 0;
		if (caiBaoShiData.baoShiLeiXing1 != 0)
			yinBi += CS::XiangbaoshiNeedSilver(caiBaoShiData.zhuangbeiLevel,caiBaoShiData.baoShiLeiXing1);
		if (caiBaoShiData.baoShiLeiXing2 != 0)
			yinBi += CS::XiangbaoshiNeedSilver(caiBaoShiData.zhuangbeiLevel,caiBaoShiData.baoShiLeiXing2);
		if (caiBaoShiData.baoShiLeiXing3 != 0)
			yinBi += CS::XiangbaoshiNeedSilver(caiBaoShiData.zhuangbeiLevel,caiBaoShiData.baoShiLeiXing3);

		WuPin* temp = NULL;

		temp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
		CCAssert(temp!=NULL && temp->getCount() >= yinBi,"");
		temp->setCount(temp->getCount() - yinBi);

		// 放回宝石
		if (caiBaoShiData.baoShiLeiXing1 != 0)
			WuPinManager::getManager()->addWuPin((ItemsType)caiBaoShiData.baoShiLeiXing1,1);
		// 放回宝石
		if (caiBaoShiData.baoShiLeiXing2 != 0)
			WuPinManager::getManager()->addWuPin((ItemsType)caiBaoShiData.baoShiLeiXing2,1);
		// 放回宝石
		if (caiBaoShiData.baoShiLeiXing3 != 0)
			WuPinManager::getManager()->addWuPin((ItemsType)caiBaoShiData.baoShiLeiXing3,1);
	}

	// 更新属性
	{
		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(caiBaoShiData.zhuangBeiId);
		CCAssert(temp!=NULL,"");
		temp->clearAllbaoShi();
	}
}

struct BaoshiHeChengData
{
	unsigned int xuyaoHeChengBaoShiLeixing;
	int count;
};
BaoshiHeChengData baoShiHeChengData;

// 宝石合成
void CmdHelper::cmdBaoshiHecheng(SPCmd_BaoshiHecheng req)
{
	CCAssert(req.count >= 1,"");

	{// 检测
		ItemsType needBaoShiType = (ItemsType)CS::HechengBaoshiNeedBaoshiType(req.baoshi_type);
		unsigned int needBaoShiNum = CS::HechengBaoshiNeedBaoshiCount(req.baoshi_type);
		unsigned int needGoldNum = CS::HechengBaoshiNeedGold(req.baoshi_type);

		if (WuPinManager::getManager()->getWuPin(needBaoShiType)->getCount() < needBaoShiNum * req.count)
		{
			SPCmd_BaoshiHechengRsp rsp;
			rsp.err_code = CEC_BaoShiNotEnough;
			CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_BaoshiHechengRsp,MSG_Rsp::createMsg(&rsp));
			CMD_LOG("%s-->baoshi needed not enough! retrun.",__FUNCTION__);
			return;
		}
		if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() < needGoldNum * req.count)
		{
			SPCmd_BaoshiHechengRsp rsp;
			rsp.err_code = CEC_GoldNotEnough;
			CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_BaoshiHechengRsp,MSG_Rsp::createMsg(&rsp));
			CMD_LOG("%s-->gold needed not enough! retrun.",__FUNCTION__);
			return;
		}
	}
		

	GameDirector::getDirector()->showWaiting();

	{
		baoShiHeChengData.xuyaoHeChengBaoShiLeixing = req.baoshi_type;
		baoShiHeChengData.count = req.count;
	}
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdBaoshiHecheng --->baoshiType = %d",req.baoshi_type);
#endif


	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_BaoshiHecheng);

	SimpleProtocolCmd cmd = BaoshiHecheng;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_BaoshiHecheng* _data = (SPCmd_BaoshiHecheng*)bodyChar;
	_data->baoshi_type = htonl(req.baoshi_type);
	_data->count = htonl(req.count);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
// 宝石合成
void CmdHelper::cmdBaoshiHechengCallback(CCObject* msg_rsp)
{
	//removeObserver(MSG_BaoshiHechengRsp);

	SPCmd_BaoshiHechengRsp* rsp = (SPCmd_BaoshiHechengRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 扣费用和用掉的宝石
	{
		ItemsType needBaoShiType = (ItemsType)CS::HechengBaoshiNeedBaoshiType(baoShiHeChengData.xuyaoHeChengBaoShiLeixing);
		unsigned int needBaoShiNum = CS::HechengBaoshiNeedBaoshiCount(baoShiHeChengData.xuyaoHeChengBaoShiLeixing);
		unsigned int needGoldNum = CS::HechengBaoshiNeedGold(baoShiHeChengData.xuyaoHeChengBaoShiLeixing);

		WuPin* temp = NULL;

		temp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		CCAssert(temp!=NULL&&temp->getCount() >= needGoldNum*baoShiHeChengData.count,"");
		temp->setCount(temp->getCount() - needGoldNum*baoShiHeChengData.count);

		temp = WuPinManager::getManager()->getWuPin(needBaoShiType);
		CCAssert(temp!=NULL&&temp->getCount() >= needBaoShiNum*baoShiHeChengData.count,"");
		temp->setCount(temp->getCount() - needBaoShiNum*baoShiHeChengData.count);
	}
	
	// 添加合成好的宝石
	{
		WuPinManager::getManager()->addWuPin((ItemsType)baoShiHeChengData.xuyaoHeChengBaoShiLeixing,baoShiHeChengData.count);
	}
}

void CmdHelper::cmdGetMarketInfo()
{
	if(MarketManager::getManager()->itemCount() > 0)
	{
		SPCmd_QueryMarketRsp rsp;
		rsp.err_code = Success;
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getMarketInfoRsp,MSG_Rsp::createMsg(&rsp));
		return;
	}

	GameDirector::getDirector()->showWaiting();
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdGetMarketInfo");
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryMarket);

	SimpleProtocolCmd cmd = queryMarket;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryMarket* _data = (SPCmd_QueryMarket*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

// 市场信息
void CmdHelper::cmdGetMarketInfoCallBack(CCObject* msg_rsp)
{
	SPCmd_QueryMarketRsp* rsp = (SPCmd_QueryMarketRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
		
	if (MarketManager::getManager()->itemCount() == 0)
	{
		vector<MarketItem> tempVec;

		for (int i =0;i<rsp->item_count;i++)
		{
			tempVec.push_back(rsp->item_arr[i]);
		}

		MarketManager::getManager()->init(tempVec);
	}
}


unsigned int g_buyItemMarketId;
unsigned int g_buyItemMarketCount;
void CmdHelper::cmdBuyItemInMarket(SPCmd_BuyItemInMarket req)
{
	CCAssert(req.buy_times > 0,"");
	CCAssert(req.buy_times < 0x7fffffff,"");

	GameDirector::getDirector()->showWaiting();

	{
		g_buyItemMarketId = req.id;
		g_buyItemMarketCount = req.buy_times;
	}
#if (DATAPOOL_LOG == 1)
	CCLog("%s --->itemId = %u,buy_times = %u",__FUNCTION__, req.id, req.buy_times);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_BuyItemInMarket);

	SimpleProtocolCmd cmd = buyItemInMarket;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_BuyItemInMarket* _data = (SPCmd_BuyItemInMarket*)bodyChar;
	_data->id = htonl(req.id);
	_data->buy_times = htons(req.buy_times);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
// 购买信息
void CmdHelper::cmdBuyItemInMarketCallBack(CCObject* msg_rsp)
{
	SPCmd_BuyItemInMarketRsp* rsp = (SPCmd_BuyItemInMarketRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success){
		try_alert(rsp->err_code);
		return;
	}

	//倍数
	unsigned int multi = g_buyItemMarketCount;
	MarketItem *item = const_cast<MarketItem *>(MarketManager::getManager()->getItemById(g_buyItemMarketId));
	if (item)
	{
		// 扣钱
		if (item->need_type == AttachType_Gold && item->need_item.item.type == ItemsType_Gold){
			WuPin* gold = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
			gold->setCount(gold->getCount() - item->need_item.item.count * multi);
		}
		else if (item->need_type == AttachType_Silver && item->need_item.item.type == ItemsType_Silver){
			WuPin* silver = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
			silver->setCount(silver->getCount() - item->need_item.item.count * multi);
		}
		// 减次数
		item->bought_times += g_buyItemMarketCount;

		// 加物品
		switch (item->sell_type)
		{
		case AttachType_Gold:
		case AttachType_Silver:
		case AttachType_Item:
		case AttachType_Diamond:
		case AttachType_Props:
			WuPinManager::getManager()->addWuPin((ItemsType)item->sell_item.item.type,item->sell_item.item.count * multi);
			break;
		case AttachType_PropsDebris:
			for (int i=0; i<multi; ++i)
				SuiPianManager::getManager()->addOneItemSuiPian(item->sell_item.item_debris);
			break;
		case AttachType_ArmsDebris:
			for (int i=0; i<multi; ++i)
				SuiPianManager::getManager()->addOneShenBingSuiPian(item->sell_item.zb_debris);
			break;
		case AttachType_GiftBag:
			for (int i=0; i<multi; ++i)
				SystemGiftBagManager::getManager()->addOneGiftBag(item->sell_item.gift);
			break;
		case AttachType_Vitality:
			MyselfManager::getManager()->getMyZhuJueData()->setVitality(MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current
				+ item->sell_item.vitality * multi);
			break;
			//暂无装备
		case AttachType_Arms:
		default:
			CCAssert(false, "error branch");
			break;
		}
	}

}

struct BattleData
{
	unsigned int task_id;
	unsigned int grid_id;
};
BattleData battelData;
void CmdHelper::cmdBattle(SPCmd_Battle req)
{
	GameDirector::getDirector()->showWaiting();
	{
		battelData.task_id = req.task_id;
		battelData.grid_id = req.grid_id;

		// 检查背包
		CHECK_PKG(1,MSG_BattleRsp);
	}
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdBattle --->task_id = %d,grid_id = %d",req.task_id,req.grid_id);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_Battle);

	SimpleProtocolCmd cmd = Battle;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_Battle* _data = (SPCmd_Battle*)bodyChar;
	_data->task_id = htonl(req.task_id);
	_data->grid_id = htonl(req.grid_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
//
void CmdHelper::cmdBattleCallBack(CCObject* msg_rsp)
{
	OneBattle* battle = (OneBattle*) ((MSG_Rsp*)msg_rsp)->getData();
	if (battle->err_code != Success)
	{
		try_alert(battle->err_code);
		return;
	}

	storeAllGettedInFight(battle->scriptVec,true);

	// 体力扣除
// 	unsigned int heng;
// 	unsigned int shu;
// 	CS::getTaskChessboardFormat(battelData.task_id,heng,shu);
// 
// 	int tiliCost = (battelData.grid_id==(heng*shu-1)?Vitality_GuanQia_Boss:Vitality_GuanQia_Normal_Jingying);
	//只有boss才是打的时候扣除体力，其余全是翻拍的时候扣除
	if (battelData.grid_id == 0)
	{
		BaseActor* zhujue = MyselfManager::getManager()->getMyZhuJueData();
		zhujue->setVitality(zhujue->getCurrentProperty().vit_current - Vitality_GuanQia_Boss);
	}

	// 更新击杀怪物的数据
	int monsterKilledPuTongAdd = 0;
	int monsterKilledJingYinAdd = 0;
	bool isBoss = ChessBoardStateManager::getManager()->isBossSection(battelData.task_id,battelData.grid_id);
	if (battelData.task_id >= MainTaskId_min &&  battelData.task_id <= MainTaskId_max && !isBoss)
	{
		if (battle->scriptVec[battle->scriptVec.size()-1].endResult.myResult == Result_Win)
		{
			for (int scriptIndex=0,count=battle->scriptVec.size(); scriptIndex<count; scriptIndex++)
			{
				for (int monsterIndex=0; monsterIndex < FormationHeroAcountMax; monsterIndex++)
				{
					if (battle->scriptVec[scriptIndex].init.oppositeTeam[monsterIndex].heroId > 0 &&
						battle->scriptVec[scriptIndex].init.oppositeTeam[monsterIndex].hero_type == Type_Guaiwu_jingying)
					{
						monsterKilledJingYinAdd ++;
					}
					else if (battle->scriptVec[scriptIndex].init.oppositeTeam[monsterIndex].heroId > 0 &&
						battle->scriptVec[scriptIndex].init.oppositeTeam[monsterIndex].hero_type == Type_Guaiwu_putong)
					{
						monsterKilledPuTongAdd ++;
					}
				}
			}
		}
	}
	ChessBoardStateManager::getManager()->addKilledJingyinMonster(monsterKilledJingYinAdd);
	ChessBoardStateManager::getManager()->addKilledPutongMonster(monsterKilledPuTongAdd);
}


struct GetTaskChessboardData
{
	unsigned int task_id;
};
GetTaskChessboardData getTaskChessboardData;
void CmdHelper::cmdGetTaskChessboard(unsigned int task_id,bool refreshBoard)
{
	GameDirector::getDirector()->showWaiting();
	{
		getTaskChessboardData.task_id = task_id;
	}
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdGetTaskChessboard --->task_id = %d,refreshBoard = %s",task_id,refreshBoard?"true":"false");
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_GetTaskChessBoard);

	SimpleProtocolCmd cmd = GetTaskChessboard;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_GetTaskChessBoard* _data = (SPCmd_GetTaskChessBoard*)bodyChar;
	_data->task_id = htonl(task_id);
	_data->need_refresh_chessboard = refreshBoard;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

//
void CmdHelper::cmdGetTaskChessBoardCallback(CCObject* msg_rsp)
{
	SPCmd_GetTaskChessBoardRsp* rsp = (SPCmd_GetTaskChessBoardRsp*)((MSG_Rsp*)msg_rsp)->getData();

	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	TaskChessBoard board;
// 	board.numInCol = rsp->grid_count_shu;
// 	board.numInRow = rsp->grid_count_heng;
// 	board.taskId = getTaskChessboardData.task_id;
// 	board.box_id = rsp->random_box_num;
// 	for (int i=0;i<board.numInCol*board.numInRow;i++)
// 	{
// 		board.gridstate.push_back(rsp->grid_stat[i]);
// 	}

	board.taskId = getTaskChessboardData.task_id;
	board.numInCol = rsp->vertical_count;
	board.numInRow = rsp->horizontal_count;
	for (int i=0; i<board.numInCol*board.numInRow+1; ++i) //boss在下标0
	{
		board.gridState.push_back(rsp->grid_arr[i]);
	}
	ChessBoardStateManager::getManager()->initOneChessboard(board);
}

unsigned int g_flopTaskId=0, g_flopGridId=0;
void CmdHelper::cmdFlopTaskGrid(unsigned int taskId, unsigned int gridId)
{
	{// 检测
		BaseActorProperty pro = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty();
		if (pro.vit_current < Vitality_GuanQia_Floped)
		{
			SPCmd_FlopTaskGridRsp rsp;
			rsp.err_code = TiliNotEnought;
			CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_FlopTaskGridRsp,MSG_Rsp::createMsg(&rsp));
			CMD_LOG("%s-->vitality needed not enough! retrun.",__FUNCTION__);
			return;
		}
	}

	g_flopTaskId = taskId;
	g_flopGridId = gridId;

	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("%s --->task_id = %u, gridId = %u", __FUNCTION__,taskId,gridId);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_FlopTaskGrid);

	SimpleProtocolCmd cmd = flopTaskGrid;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_FlopTaskGrid* _data = (SPCmd_FlopTaskGrid*)bodyChar;
	_data->task_id = htonl(taskId);
	_data->grid_id = htonl(gridId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdFlopTaskGridRsp(CCObject* msg_rsp)
{
	Flat_FlopTaskGridRsp* flat_data = (Flat_FlopTaskGridRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();

	if (flat_data->err_code != Success)
	{
		try_alert(flat_data->err_code);
		return;
	}

	/*体力扣除规则：
	  BOSS关保持不变：打一次扣12点体力
	  每翻一张牌 扣2点体力（打小怪和精英怪不扣体力）
	*/
	BaseActor* zhujue = MyselfManager::getManager()->getMyZhuJueData();
	zhujue->setVitality(zhujue->getCurrentProperty().vit_current - Vitality_GuanQia_Floped);

	//更新 chessboard
	TaskChessBoard _info;
	ChessBoardStateManager::getManager()->getChessBoard(g_flopTaskId, _info);
	_info.gridState.at(0).stat = flat_data->boss_stat;
	_info.gridState.at(g_flopGridId).stat = flat_data->grid_stat;
	ChessBoardStateManager::getManager()->initOneChessboard(_info);

	//如果翻到宝箱、 则后面有包箱数据
	for (int i=0; i<flat_data->count; ++i)
	{
		WuPinManager::getManager()->addWuPin((ItemsType)flat_data->items.at(i).type, flat_data->items.at(i).count);
	}
}

struct ChangeTaskStateData
{
	unsigned int task_id;
	TaskStatus state;
};
ChangeTaskStateData changeTaskStateData;
void CmdHelper::cmdChangeTaskState(unsigned int taskId,TaskStatus state)
{
	if (state != TaskStatus_Got && state != TaskStatus_Submitted)
	{
		LFToast::makeToast("cmdChangeTaskState --> can not send this state");
		CCLog("cmdChangeTaskState --> can not send this state");
		return;
	}

	GameDirector::getDirector()->showWaiting();
	{
		changeTaskStateData.task_id = taskId;
		changeTaskStateData.state = state;
	}
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdChangeTaskState --->task_id = %d，state = ",taskId,state);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ChangeTaskStat);

	SimpleProtocolCmd cmd = changeTaskStat;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ChangeTaskStat* _data = (SPCmd_ChangeTaskStat*)bodyChar;
	_data->task_id = htonl(taskId);
	_data->stat = htonl((unsigned int)state);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}


void CmdHelper::cmdChangeTaskStateCallback(CCObject* f_msg_rsp)
{
	Flat_ChangeTaskStatRsp* rsp = (Flat_ChangeTaskStatRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();

	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 加银币
	WuPinManager::getManager()->addWuPin(ItemsType_Silver,rsp->silver);

	int count = rsp->hero_new_attri_vec.size();
	for (int i=0;i<count;i++)
	{
		BaseActor* hero = MyselfManager::getManager()->getMyTeamActor(rsp->hero_new_attri_vec[i].hero_id);
		CCAssert(hero!=NULL,"");
		if(hero == NULL) continue;

		rsp->hero_old_attri_vec.push_back(hero->getCurrentProperty());
		// 刷新数据
		hero->refreshBaseProPerty(rsp->hero_new_attri_vec[i]);
	}


	//HeroDetail* detailArray = rsp->hero_arr;
	//for (int i=0;i<rsp->hero_count;i++)
	//{
	//	BaseActor* hero = MyselfManager::getManager()->getMyTeamActor(detailArray[i].basic_info.hero_id);
	//	CCAssert(hero!=NULL,"");
	//	if(hero == NULL) continue;
	//	// 刷新数据
	//	hero->refreshBaseProPerty(detailArray[i]);
	//}

}

void CmdHelper::cmdRestoreVitality()
{
	CCLog("%s:cmd is cancled!");
//	GameDirector::getDirector()->showWaiting();
//#if (DATAPOOL_LOG == 1)
//	CCLog("CmdHelper::cmdRestoreVitality");
//#endif
//
//	removeLastBuff();
//
//	unsigned int dataLength = sizeof(SPCmd_RestoreVitality);
//
//	SimpleProtocolCmd cmd = restoreVitality;
//	char* head = newHead(cmd,dataLength);
//	char* bodyChar = new char[dataLength];
//
//	SPCmd_RestoreVitality* _data = (SPCmd_RestoreVitality*)bodyChar;
//	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);
//
//	// 先缓冲，不要删除
//	lastCmd.head = head;
//	lastCmd.body = bodyChar;
//	lastCmd.cmd = cmd;
}
void CmdHelper::cmdRestoreVitalityCallback(CCObject* msg_rsp)
{
	SPCmd_RestoreVitalityRsp* rsp = (SPCmd_RestoreVitalityRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 恢复体力
	BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	actor->setVitality(Vitality_Total);
}

// 被服务器踢出了，客户端没有发送方式，收到就跳向login
void CmdHelper::cmdBeKickOutRsp(CCObject* msg_rsp)
{
	SPCmd_BeKickOutRsp* rsp = (SPCmd_BeKickOutRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	jumpToIndex_kickOut();
}
// 心跳包，只管接受即可
void CmdHelper::cmdHeartBeatRsp(CCObject* msg_rsp)
{
	Flat_HeartBeatRsp* rsp = (Flat_HeartBeatRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	//1. 刷新体力
	BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	if (actor != NULL)
	{
		actor->setVitality(rsp->current_vit);
	}else
	{
		CCLog("cmdHeartBeatRsp ---> If you are logined ,this is BUG");
	}
	//2. 推送消息 （跑马灯）
	SystemPushMessagePool::getPool()->addMessage(rsp->msgVec);
}

class GiftBuffer;
static GiftBuffer* gift_buffer = NULL;
// 缓存服务器返回的宝箱
class GiftBuffer
{
public:
	static GiftBuffer* getBuffer()
	{
		if (gift_buffer == NULL)
		{
			gift_buffer = new GiftBuffer();
		}
		return gift_buffer;
	}

	// 放入到缓存
	void put(vector<GiftData> gift)
	{
		giftPool.insert(giftPool.end(),gift.begin(),gift.end());
	}
	void put(GiftData gift)
	{
		giftPool.push_back(gift);
	}
	// 找到就返回true，并把数据放在gift里面
	bool get(unsigned int gift_id,GiftData & gift)
	{
		int num = giftPool.size();
		for (int i=0;i<num;i++)
		{
			if (giftPool[i].gift_id == gift_id)
			{
				gift = giftPool[i];

				return true;
			}
		}

		return false;
	}
	// 找到就返回true，并把数据放在gift里面
	bool getBuyJiFenId(unsigned int jifen_id,GiftJifen& gift)
	{
		int num = giftPool.size();
		for (int i=0;i<num;i++)
		{
			if (giftPool[i].gift_id == 0 && giftPool[i].type == GiftType_Jifen && giftPool[i].data_union.jifen.gift_id == jifen_id)
			{
				gift = giftPool[i].data_union.jifen;
				return true;
			}
		}
		return false;
	}
private:
	vector<GiftData> giftPool;
};


// 进入竞技场,这个一直需要持续到cmdArenaGetBattleScriptGiftRsp
void CmdHelper::cmdArenaEnter(SPCmd_ArenaEnter req)
{
	GameDirector::getDirector()->showWaiting();
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdArenaEnter --->hardLevel = %d",(int)req.hardLevel);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ArenaEnter);

	SimpleProtocolCmd cmd = arenaEnter;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ArenaEnter* _data = (SPCmd_ArenaEnter*)bodyChar;
	_data->hardLevel = req.hardLevel;	// 不需要转字节序

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;

	jingJiChang_EnterSend = true;
	jingJiChang_EnterSuccess = false;	// 设置为false，那么期间收到Update就会扔到
}
void CmdHelper::cmdArenaEnterRsp(CCObject* f_msg_rsp)
{
	// 不需要做任何事
	Flat_ArenaEnterRsp* rsp = (Flat_ArenaEnterRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
	}
}
void CmdHelper::cmdArenaUpdateRoomRsp(CCObject* f_msg_rsp)
{
	// 不需要做任何事
	Flat_ArenaUpdateRoomRsp* rsp = (Flat_ArenaUpdateRoomRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
	}
}
void CmdHelper::cmdArenaGetBattleScriptAndGiftRsp(CCObject* f_msg_rsp)
{
	// 把宝箱放入缓存
	Flat_ArenaGetBattleScriptGiftRsp* rsp = (Flat_ArenaGetBattleScriptGiftRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	storeAllGettedInFight(rsp->scriptVec,false);
	// 封装成GiftData保存
	for (int i=0,count = rsp->gettedData.size(); i<count; i++)
	{
		GiftData jifen;
		jifen.gift_id = 0;
		jifen.type = GiftType_Jifen;
		jifen.data_union.jifen = rsp->gettedData[i];

		GiftBuffer::getBuffer()->put(jifen);
	}
	//GiftBuffer::getBuffer()->put(rsp->missedData); // miss的可以不放，因为不会被领取的

	// 刷新次数
	MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->free_fight_remainder_time = rsp->battle_time_remainder;
}


struct GetGiftData
{
	unsigned int wantGetId;
};
GetGiftData getGiftData;
// 领取竞技场物品
void CmdHelper::cmdArenaGetGift(unsigned int wantGetId)
{
	GameDirector::getDirector()->showWaiting();
	getGiftData.wantGetId = wantGetId;	// 保存

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdArenaGetGift --->id = {%u}",wantGetId);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ArenaGetGift);

	SimpleProtocolCmd cmd = arenaGetGift;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ArenaGetGift* _data = (SPCmd_ArenaGetGift*)bodyChar;
	_data->gift_id = htonl(wantGetId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdArenaGetGiftRsp(CCObject* msg_rsp)
{
	SPCmd_ArenaGetGiftRsp* rsp = (SPCmd_ArenaGetGiftRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 把宝箱里面的Item放入到背包里面
	GiftJifen _gift;
	if (GiftBuffer::getBuffer()->getBuyJiFenId(getGiftData.wantGetId,_gift))
	{
		MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->jifen += _gift.jifen;
	}
	else
	{
		CCAssert(false,"异常！按理说这里是不会崩溃的，一定要查清楚！");
	}
}

// 进入团队战场,这个一直需要持续到cmdTeamBossGetBattleScriptGiftRsp
void CmdHelper::cmdTeamBossEnter(SPCmd_TeamBossEnter req)
{
	GameDirector::getDirector()->showWaiting();
#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdTeamBossEnter --->hardLevel = %d",(int)req.hardLevel);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_TeamBossEnter);

	SimpleProtocolCmd cmd = teamBossEnter;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_TeamBossEnter* _data = (SPCmd_TeamBossEnter*)bodyChar;
	_data->hardLevel = req.hardLevel;	// 不需要转字节序

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;

	tuanDuiBoss_EnterSend = true;
	tuanDuiBoss_EnterSuccess = false;
}
void CmdHelper::cmdTeamBossEnterRsp(CCObject* f_msg_rsp)
{
	// 不需要做任何事
	Flat_TeamBossEnterRsp* rsp = (Flat_TeamBossEnterRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
	}
}
void CmdHelper::cmdTeamBossUpdateRoomRsp(CCObject* f_msg_rsp)
{
	// 不需要做任何事
	Flat_TeamBossUpdateRoomRsp* rsp = (Flat_TeamBossUpdateRoomRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
	}
}
void CmdHelper::cmdTeamBossGetBattleScriptAndGiftRsp(CCObject* f_msg_rsp)
{
	// 把宝箱放入缓存
	Flat_TeamBossGetBattleScriptGiftRsp* rsp = (Flat_TeamBossGetBattleScriptGiftRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<FightScript> s;
	s.push_back(rsp->fightScript);
	storeAllGettedInFight(s,false);

	GiftBuffer::getBuffer()->put(rsp->gettedData);
	//GiftBuffer::getBuffer()->put(rsp->missedData); // miss的可以不放，因为不会被领取的
}
// 领取团队战物品
void CmdHelper::cmdTeamBossGetGift(unsigned int wantGetId)
{
	GameDirector::getDirector()->showWaiting();
	getGiftData.wantGetId = wantGetId;	// 保存

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdTeamBossGetGift --->id = {%u}",wantGetId);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_TeamBossGetGift);

	SimpleProtocolCmd cmd = teamBossGetGift;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_TeamBossGetGift* _data = (SPCmd_TeamBossGetGift*)bodyChar;
	_data->gift_id = htonl(wantGetId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdTeamBossGetGiftRsp(CCObject* msg_rsp)
{
	SPCmd_TeamBossGetGiftRsp* rsp = (SPCmd_TeamBossGetGiftRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 把宝箱里面的Item放入到背包里面
	GiftData _gift;
	if (GiftBuffer::getBuffer()->get(getGiftData.wantGetId,_gift))
	{
		if (_gift.type == GiftType_Zhuangbei)
		{
			CCAssert(false,"没有实现。因为数据量不够");
		}
		else if (_gift.type == GiftType_Item)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)_gift.data_union.item.type,_gift.data_union.item.count);
		}
		else if (_gift.type == GiftType_Jifen)
		{
			MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->jifen += _gift.data_union.jifen.jifen;
		}
		else
		{
			CCAssert(false,"异常，未知类型");
		}
	}
	else
	{
		CCAssert(false,"异常！按理说这里是不会崩溃的，一定要查清楚！");
	}


	/*int num = getGiftData.wantGetId_arr.size();
	for (int i=0;i<num;i++)
	{
		GiftData _gift;
		if (GiftBuffer::getBuffer()->get(getGiftData.wantGetId_arr[i],_gift))
		{
			if (_gift.type == GiftType_Zhuangbei)
			{
				CCAssert(false,"没有实现。因为数据量不够");
			}
			else if (_gift.type == GiftType_Item)
			{
				WuPinManager::getManager()->addWuPin((ItemsType)_gift.data_union.item.type,_gift.data_union.item.count);
			}
			else
			{
				CCAssert(false,"异常，未知类型");
			}
		}
		else
		{
			CCAssert(false,"异常！按理说这里是不会崩溃的，一定要查清楚！");
		}
	}*/
}


// 进入团队副本
void CmdHelper::cmdCityGuardEnter()
{
	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdCityGuardEnter");
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_CityGuardEnter);

	SimpleProtocolCmd cmd = cityGuardEnter;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_CityGuardEnter* _data = (SPCmd_CityGuardEnter*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdCityGuardEnterRsp(CCObject* msg_rsp)
{
	// 不需要做任何事
	SPCmd_CityGuardEnterRsp* rsp = (SPCmd_CityGuardEnterRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	buff_CityGuard = *rsp;
}

// 获取对战数据
void CmdHelper::cmdCityGuardGetNextMonster()
{
	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdCityGuardGetNextMonster");
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_CityGuardGetNextMonster);

	SimpleProtocolCmd cmd = cityGuardGetNextMonster;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_CityGuardGetNextMonster* _data = (SPCmd_CityGuardGetNextMonster*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdCityGuardGetNextMonsterRsp(CCObject* msg_rsp)
{
	// 不需要做任何事
	SPCmd_CityGuardGetNextMonsterRsp* rsp = (SPCmd_CityGuardGetNextMonsterRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
}

// 迎战 或者 放弃
void CmdHelper::cmdCityGuardFight(bool fight)
{
	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdCityGuardFight [%s]",fight?"true":"false");
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_CityGuardFight);

	SimpleProtocolCmd cmd = cityGuardFight;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_CityGuardFight* _data = (SPCmd_CityGuardFight*)bodyChar;
	_data->fight = fight;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}


void CmdHelper::cmdCityGuardFightRsp(CCObject* f_msg_rsp)
{
	// 在放弃，或者被打败的时候 保存钱币
	Flat_CityGuardFightRsp* rsp = (Flat_CityGuardFightRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 放弃或者被打败的时候保存银币
	if (rsp->isFighting == false || rsp->fightScript.endResult.myResult == Result_Lose)
	{
		WuPinManager::getManager()->addWuPin(ItemsType_Silver,rsp->silver);
	}

	vector<FightScript> s;
	s.push_back(rsp->fightScript);
	storeAllGettedInFight(s,false);


	buff_CityGuard.current_silver = rsp->silver;
	//buff_CityGuard.last_silver = 
	//buff_CityGuard.last_total_turns =  
	buff_CityGuard.sucess_turn += (rsp->isFighting == true && rsp->fightScript.endResult.myResult == Result_Win)?1:0;
}

void CmdHelper::getCityGruardRuntimeData(SPCmd_CityGuardEnterRsp& out_data)
{
	out_data.err_code = true;
	out_data.current_silver = buff_CityGuard.current_silver;
	out_data.last_silver = buff_CityGuard.last_silver;
	out_data.last_total_turns = buff_CityGuard.last_total_turns;
	out_data.sucess_turn = buff_CityGuard.sucess_turn;
}

void CmdHelper::cmdQueryAnotherTeamInfo(unsigned int hero_id)
{
	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdQueryAnotherTeamInfo [main_hero_id = %u]",hero_id);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryAnotherTeamInfo);

	SimpleProtocolCmd cmd = queryAnotherTeamInfo;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryAnotherTeamInfo* _data = (SPCmd_QueryAnotherTeamInfo*)bodyChar;
	_data->hero_id = htonl(hero_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryAnotherTeamInfoRsp(CCObject* f_msg_rsp)
{
	Flat_QueryAnotherTeamInfoRsp* rsp = (Flat_QueryAnotherTeamInfoRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
}

void CmdHelper::cmdTopHeroGetList()
{
	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdTopHeroGetList");
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_TopHeroGetList);

	SimpleProtocolCmd cmd = topHeroGetList;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_TopHeroGetList* _data = (SPCmd_TopHeroGetList*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;

}

unsigned int topHero_next_chlg_time = 0;
void CmdHelper::cmdTopHeroGetListRsp(CCObject* f_msg_rsp)
{
	Flat_TopHeroGetListRsp* rsp = (Flat_TopHeroGetListRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 保存时间
	topHero_next_chlg_time = rsp->next_chlg_time;
}

void CmdHelper::cmdTopHeroClearCDTime()
{
	CMD_LOG("%s",__FUNCTION__);

	{
		if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() < CS::clearTopHeroCDTimeNeedGold())
		{
			POST_ERROR(CEC_GoldNotEnough,MSG_TopHeroClearCDTimeRsp);
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_TopHeroClearCDTime REQUESR_DATA;
	SimpleProtocolCmd cmd = topHeroClearCDTime;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdTopHeroClearCDTimeRsp(CCObject* msg_rsp)
{
	SPCmd_TopHeroClearCDTimeRsp* rsp = (SPCmd_TopHeroClearCDTimeRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	topHero_next_chlg_time = rsp->next_chlg_time;
	WuPinManager::getManager()->setWuPinNum(ItemsType_Gold,rsp->gold_balance);
}

void CmdHelper::cmdQuitCityGuard()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QuitCityGuard REQUESR_DATA;
	SimpleProtocolCmd cmd = quitCityGuard;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQuitCityGuardRsp(CCObject* msg_rsp)
{
	SPCmd_QuitCityGuardRsp* rsp = (SPCmd_QuitCityGuardRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
}


void CmdHelper::cmdTopHeroChallenge(unsigned int hero_id)
{

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdTopHeroChallenge --> heroId = %u",hero_id);
#endif

	{
		if (topHero_next_chlg_time > ServerTime::getTime())
		{
			POST_ERROR(CEC_TOPHeroISCDTime,MSG_topHeroChallengeRsp_F);
			return;
		}
	}
	


	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_TopHeroChallenge);

	SimpleProtocolCmd cmd = topHeroChallenge;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_TopHeroChallenge* _data = (SPCmd_TopHeroChallenge*)bodyChar;
	_data->hero_id = htonl(hero_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;

}
void CmdHelper::cmdTopHeroChallengeRsp(CCObject* f_msg_rsp)
{
	Flat_TopHeroChallengeRsp* rsp = (Flat_TopHeroChallengeRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<FightScript> s;
	s.push_back(rsp->fightScript);
	storeAllGettedInFight(s,false);

	if (s.at(s.size()-1).endResult.myResult == Result_Lose)
		topHero_next_chlg_time = rsp->nextChallengeTime;
}
void CmdHelper::cmdSystemGiftQuery()
{
	GameDirector::getDirector()->showWaiting();

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdSystemGiftQuery");
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_SystemGiftQuery);

	SimpleProtocolCmd cmd = systemGiftQuery;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_SystemGiftQuery* _data = (SPCmd_SystemGiftQuery*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdSystemGiftQueryRsp(CCObject* f_msg_rsp)
{
	Flat_SystemGiftQueryRsp* rsp = (Flat_SystemGiftQueryRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 保存数据
	SystemGiftManager::getManager()->init(rsp->gift_arr);
}

struct GetSystemGiftData
{
	unsigned int gift_id;
};
GetSystemGiftData getSysGiftData;

void CmdHelper::cmdSystemGiftGetGift(unsigned int giftId)
{
	GameDirector::getDirector()->showWaiting();

	getSysGiftData.gift_id = giftId;// 保存

#if (DATAPOOL_LOG == 1)
	CCLog("CmdHelper::cmdSystemGiftGetGift id = %u",giftId);
#endif

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_SystemGiftGetGift);

	SimpleProtocolCmd cmd = systemGiftGetGift;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_SystemGiftGetGift* _data = (SPCmd_SystemGiftGetGift*)bodyChar;
	_data->gift_id = htonl(giftId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdSystemGiftGetGiftRsp(CCObject* msg_rsp)
{
	SPCmd_SystemGiftGetGiftRsp* rsp = (SPCmd_SystemGiftGetGiftRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 检验
	{
		systemGift theGift;
		bool found = SystemGiftManager::getManager()->getOneGiftById(getSysGiftData.gift_id,theGift);
		CCAssert(found,"Must Exist in Native");
		CCAssert(theGift.item_id == rsp->item_id,"Must is the SameOne");
		CCAssert(theGift.item_count == rsp->item_count,"Must is the SameOne");
	}


	// 从礼物列表中移除
	SystemGiftManager::getManager()->removeOneGiftById(getSysGiftData.gift_id);
	// 加入到背包
	WuPinManager::getManager()->addWuPin((ItemsType)rsp->item_id,rsp->item_count);
}

void CmdHelper::cmdchargeVitality()
{
	CMD_LOG("%s",__FUNCTION__);
	{
		if (MyselfManager::getManager()->getRuntionData()->buyTiLiTimeLeft <= 0)
		{
			POST_ERROR(CEC_NoByTiLiTime,MSG_chargeVitalityRsp);
			return;
		}

		if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() < Vitality_Price)
		{
			int errorCode = ItemsType_Gold+CEC_ItemNotEnough_Begin;
			POST_ERROR(errorCode,MSG_chargeVitalityRsp);
			return;
		}
	}
	
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();
	unsigned int dataLength = sizeof(SPCmd_SystemGiftGetGift);

	SimpleProtocolCmd cmd = chargeVitality;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ChargeVitality* _data = (SPCmd_ChargeVitality*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdchargeVitalityRsp(CCObject* msg_rsp)
{
	SPCmd_ChargeVitalityRsp* rsp = (SPCmd_ChargeVitalityRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	if (MyselfManager::getManager()->getRuntionData()->buyTiLiTimeLeft > 0)
	{
		MyselfManager::getManager()->getRuntionData()->buyTiLiTimeLeft -= 1;
	}
	else
	{
		CCLog("Error : %s --> buyTiLiTimeLeft < 0!!");
		CCAssert(false,"");
	}

	//// 恢复体力
	//BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	//actor->setVitality(Vitality_Total);

	//2014-03-03修正为增加体力，不恢复体力
	// 新增体力
	BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	actor->setVitality(Vitality_Total+actor->getCurrentProperty().vit_current);

	// 扣金币
	WuPinManager::getManager()->setWuPinNum(ItemsType_Gold,rsp->gold_balance);
}


// 锻造
// 查询可以锻造的东西
void CmdHelper::cmdDuanzaoListQuery()
{
	CCAssert(false,"废弃了");
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("CmdHelper::cmdDuanzaoListQuery");

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_DuanzaoListQuery);

	SimpleProtocolCmd cmd = duanzaoListQuery;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_DuanzaoListQuery* _data = (SPCmd_DuanzaoListQuery*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdDuanzaoListQueryRsp(CCObject* msg_rsp)
{
	SPCmd_DuanzaoListQueryRsp* rsp = (SPCmd_DuanzaoListQueryRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<DuanzaoGroup> datas;
	for (int i=0;i<rsp->group_count;i++)
	{
		datas.push_back(rsp->group_arr[i]);
	}

	DuanZaoManager::getManager()->init(datas);
}

struct DuanZaoData
{
	bool duanzao_10_times;
	unsigned int profession;
};
DuanZaoData lastDuanzaoData;
// 选择一项，进行锻造
//void CmdHelper::cmdDuanzao(bool duanzao_10_times,unsigned int duanzao_group_id)
void CmdHelper::cmdDuanzao(bool duanzao_10_times,unsigned int profession)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("CmdHelper::cmdDuanzao duanzao_10_times = %s,profession = %u",
		duanzao_10_times?"true":"false",
		profession);

	// 检查背包
	if (duanzao_10_times)
	{
		CHECK_PKG(10,MSG_DuanzaoRsp_F);
	}
	else
	{
		CHECK_PKG(1,MSG_DuanzaoRsp_F);
	}


	int needCostGold = DROW_SHENBING_NEED_GOLD * (duanzao_10_times?10:1);

	if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() < needCostGold)
	{
		CMD_LOG("CmdHelper ---> Waring : your gold[%u/%u] is not enough!",
			WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount(),needCostGold);
		POST_ERROR(CEC_GoldNotEnough,MSG_DuanzaoRsp_F);
		return;
	}

	// 保存
	lastDuanzaoData.duanzao_10_times = duanzao_10_times;
	lastDuanzaoData.profession = profession;

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_Duanzao);

	SimpleProtocolCmd cmd = duanzao;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_Duanzao* _data = (SPCmd_Duanzao*)bodyChar;
	_data->is_times_10 = duanzao_10_times;
	_data->profession = htonl(profession);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdDuanzaoRsp(CCObject* f_msg_rsp)
{
	Flat_DuanzaoRsp* rsp = (Flat_DuanzaoRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 添加装备
	int equipNum = rsp->equips.size();
	for (int i=0;i<equipNum;i++)
	{
		EquipmentManager::getManager()->addOneEquipment(rsp->equips[i]);
	}

	int needMoney = DROW_SHENBING_NEED_GOLD*(lastDuanzaoData.duanzao_10_times?10:1);
	// 扣钱
	WuPinManager::getManager()->addWuPin(ItemsType_Gold,-needMoney);
}

static unsigned int lastQueryDuanzaogroupId = 0;
void CmdHelper::cmdDuanzaoGroupZBQuery(unsigned int duanzao_group_id)
{
	GameDirector::getDirector()->showWaiting();

	Flat_DuanzaoGroupZBQueryRsp rsp;
	if (cmdBuffer->getDuanzaoGroup(duanzao_group_id,rsp))
	{
		lastQueryDuanzaogroupId = duanzao_group_id;
		CMD_LOG("CmdHelper::cmdDuanzaoGroupZBQuery duanzao_group_id = %u,we found in buffer,just return!",duanzao_group_id);
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_DuanzaoGroupZBQueryRsp_F,Flat_MSG_Rsp::createMsg(&rsp));
		return;
	}

	CMD_LOG("CmdHelper::cmdDuanzaoGroupZBQuery duanzao_group_id = %u",duanzao_group_id);
	removeLastBuff();
	lastQueryDuanzaogroupId = duanzao_group_id;
	unsigned int dataLength = sizeof(SPCmd_DuanzaoGroupZBQuery);
	SimpleProtocolCmd cmd = duanzaoGroupZBQuery;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_DuanzaoGroupZBQuery* _data = (SPCmd_DuanzaoGroupZBQuery*)bodyChar;
	_data->duanzao_group_id = htonl(duanzao_group_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdDuanzaoGroupZBQueryRsp(CCObject* f_msg_rsp)
{
	Flat_DuanzaoGroupZBQueryRsp* rsp = (Flat_DuanzaoGroupZBQueryRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 放入缓存.....
	cmdBuffer->putDuanzaoGroup(lastQueryDuanzaogroupId,*rsp);
}

void CmdHelper::cmdQueryAllSuitName()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("CmdHelper::cmdQueryAllSuitName");

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryAllSuitName);

	SimpleProtocolCmd cmd = queryAllSuitName;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryAllSuitName* _data = (SPCmd_QueryAllSuitName*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryAllSuitNameRsp(CCObject* msg_rsp)
{
	SPCmd_QueryAllSuitNameRsp* rsp = (SPCmd_QueryAllSuitNameRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<OneSuitInfo> suits;
	SuitNameInfo* suit_arry = (SuitNameInfo*)rsp->data;
	for (int i=0;i<rsp->suit_count;i++)
	{
		OneSuitInfo one_suit;
		one_suit.suit_id = suit_arry[i].suit_id;
		one_suit.suit_name = suit_arry[i].suit_name;

		if (suit_arry[i].group_id_1 > 0)
		{
			SuitPart one_part;
			one_part.group_ids.push_back(suit_arry[i].group_id_1);
			one_part.zhuanbei_type = (ZhuangbeiType)suit_arry[i].zhuangbei_type_1;
			one_part.zhuangbei_name = suit_arry[i].zhuangbei_name_1;

			one_suit.parts_of_suit.push_back(one_part);
		}

		if (suit_arry[i].group_id_2 > 0)
		{
			SuitPart one_part;
			one_part.group_ids.push_back(suit_arry[i].group_id_2);
			one_part.zhuanbei_type = (ZhuangbeiType)suit_arry[i].zhuangbei_type_2;
			one_part.zhuangbei_name = suit_arry[i].zhuangbei_name_2;

			one_suit.parts_of_suit.push_back(one_part);
		}

		if (suit_arry[i].group_id_3 > 0)
		{
			SuitPart one_part;
			one_part.group_ids.push_back(suit_arry[i].group_id_3);
			one_part.zhuanbei_type = (ZhuangbeiType)suit_arry[i].zhuangbei_type_3;
			one_part.zhuangbei_name = suit_arry[i].zhuangbei_name_3;

			one_suit.parts_of_suit.push_back(one_part);
		}
		if (suit_arry[i].group_id_4 > 0)
		{
			SuitPart one_part;
			one_part.group_ids.push_back(suit_arry[i].group_id_4);
			one_part.zhuanbei_type = (ZhuangbeiType)suit_arry[i].zhuangbei_type_4;
			one_part.zhuangbei_name = suit_arry[i].zhuangbei_name_4;

			one_suit.parts_of_suit.push_back(one_part);
		}
		if (suit_arry[i].group_id_5 > 0)
		{
			SuitPart one_part;
			one_part.group_ids.push_back(suit_arry[i].group_id_5);
			one_part.zhuanbei_type = (ZhuangbeiType)suit_arry[i].zhuangbei_type_5;
			one_part.zhuangbei_name = suit_arry[i].zhuangbei_name_5;

			one_suit.parts_of_suit.push_back(one_part);
		}

		if (suit_arry[i].group_id_6 > 0)
		{
			SuitPart one_part;
			one_part.group_ids.push_back(suit_arry[i].group_id_6);
			one_part.zhuanbei_type = (ZhuangbeiType)suit_arry[i].zhuangbei_type_6;
			one_part.zhuangbei_name = suit_arry[i].zhuangbei_name_6;

			one_suit.parts_of_suit.push_back(one_part);
		}
		//// 不能memcpy这样赋值
		//memcpy(one_suit.suit_attr,suit_arry[i].suit_attr,sizeof(one_suit.suit_attr));

		for (int j=0;j<5;j++)
		{
			for (int k=0;k<2;k++)
			{
				one_suit.suit_attr[j][k].attr_type = suit_arry[i].suit_attr[j][k].attr_type;
				one_suit.suit_attr[j][k].attr_value = suit_arry[i].suit_attr[j][k].attr_value;
				// 这个值对于套装来说没有用
				one_suit.suit_attr[j][k].cuiqu_suc_ratio = suit_arry[i].suit_attr[j][k].cuiqu_suc_ratio;
			}
		}


		suits.push_back(one_suit);
	}

	SuitPool::getPool()->initSuit(suits);
}


static long long lastCuiQuZb_Id = 0;
void CmdHelper::cmdZBCuiqu(SPCmd_ZBCuiqu req)
{
	{
		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(req.zhuangbei_id);
		string infor;
		if (!temp->canBeCuiquAttribute(req.attr_type,infor))
		{
			CMD_LOG("cmdZBCuiqu:%s",infor.c_str());
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("CmdHelper::cmdZBCuiqu,zbId = %lld,attr_type = %d,attr_value = %d,is_fumo_attr = %s",
		req.zhuangbei_id,req.attr_type,req.attr_value,req.is_fumo_attr?"true":"false");
	removeLastBuff();

	lastCuiQuZb_Id = req.zhuangbei_id;

	unsigned int dataLength = sizeof(SPCmd_ZBCuiqu);

	SimpleProtocolCmd cmd = ZBCuiqu;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ZBCuiqu* _data = (SPCmd_ZBCuiqu*)bodyChar;
	_data->zhuangbei_id = LF::lf_htonll(req.zhuangbei_id);
	_data->attr_type = htonl(req.attr_type);
	_data->attr_value = htonl(req.attr_value);
	_data->is_fumo_attr = req.is_fumo_attr;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdZBCuiquRsp(CCObject* msg_rsp)
{
	SPCmd_ZBCuiquRsp* rsp = (SPCmd_ZBCuiquRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 移除装备
	EquipmentManager::getManager()->destoryOneEquip(lastCuiQuZb_Id);

	if (rsp->sucess)
	{// 添加魔法精华
		MofaJinghuaManager::getManager()->addOneJingHua(rsp->jinghua);
	}
}

struct ZBFuMoData
{
	unsigned long long zb_Id;
	MofaJinghua jinghua;
};
static ZBFuMoData last_FuMoData;
void CmdHelper::cmdZBFumo(SPCmd_ZBFumo req)
{
	{
		Equipment* temp = EquipmentManager::getManager()->getOneEquipment(req.zhuangbei_id);
		string infor;
		if (!temp->canBeFomoAttribute(req.jinghua,infor))
		{
			CMD_LOG("cmdZBFumo:%s",infor.c_str());
			return;
		}
		if (req.jinghua.count != 1)
		{
			req.jinghua.count = 1;
			CMD_LOG("Waring : cmdZBFumo --> jinghua.count should be [=1],cmdhelper will change this force!");
		}
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("CmdHelper::cmdZBFumo,zbId = %lld,jinghua.zb_type = %d,jinghua.attr_type = %d,jinghua.level = %d,jinghua.count = %d",
		req.zhuangbei_id,
		req.jinghua.zb_type,req.jinghua.attr_type,req.jinghua.level,req.jinghua.count);
	removeLastBuff();

	last_FuMoData.zb_Id = req.zhuangbei_id;
	last_FuMoData.jinghua = req.jinghua;

	unsigned int dataLength = sizeof(SPCmd_ZBFumo);

	SimpleProtocolCmd cmd = ZBFumo;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ZBFumo* _data = (SPCmd_ZBFumo*)bodyChar;
	_data->zhuangbei_id = LF::lf_htonll(req.zhuangbei_id);
	_data->jinghua.attr_type = htonl(req.jinghua.attr_type);
	_data->jinghua.zb_type = htonl(req.jinghua.zb_type);
	_data->jinghua.level = htonl(req.jinghua.level);
	_data->jinghua.count = htonl(req.jinghua.count);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdZBFumoRsp(CCObject* msg_rsp)
{
	SPCmd_ZBFumoRsp* rsp = (SPCmd_ZBFumoRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 移除魔法精华
	MofaJinghuaManager::getManager()->removeOneJingHua(last_FuMoData.jinghua);

	if (rsp->sucess)
	{// 设置附魔信息

		Equipment* equip = EquipmentManager::getManager()->getOneEquipment(last_FuMoData.zb_Id);
		CCAssert(equip!=NULL,"不可能为null");
		if (equip != NULL)
		{
			equip->setNewFuMoAttribute(rsp->new_fumo_attr);
		}
	}
}
void CmdHelper::cmdQueryAllMofaJinghua()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("CmdHelper::cmdQueryAllMofaJinghua");
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryAllMofaJinghua);

	SimpleProtocolCmd cmd = queryAllMofaJinghua;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryAllMofaJinghua* _data = (SPCmd_QueryAllMofaJinghua*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryAllMofaJinghuaRsp(CCObject* msg_rsp)
{
	SPCmd_QueryAllMofaJinghuaRsp* rsp = (SPCmd_QueryAllMofaJinghuaRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<MofaJinghua> allJingHua;
	for (int i=0;i<rsp->count;i++)
	{
		allJingHua.push_back(rsp->jinghua_arr[i]);
	}

	MofaJinghuaManager::getManager()->init(allJingHua);
}

//unsigned int lastDiaoLuoTaskId = 0;//可能一次查3个
queue<unsigned int> lastDiaoLuoQueue;
void CmdHelper::cmdQueryFubenDiaoluo(unsigned int task_id)
{
	CMD_LOG("CmdHelper::cmdQueryFubenDiaoluo,task_id = %u",task_id);
	lastDiaoLuoQueue.push(task_id);

	GameDirector::getDirector()->showWaiting();
	Flat_QueryFubenDiaoluoRsp rsp;
	if (cmdBuffer->getDiaoLuo(task_id,rsp))
	{
		//lastDiaoLuoTaskId = task_id;
		CMD_LOG("CmdHelper::cmdQueryFubenDiaoluo task_id = %u,we found in buffer,just return!",task_id);
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryFubenDiaoluoRsp_F,Flat_MSG_Rsp::createMsg(&rsp));
		return;
	}

	
	removeLastBuff();
	//lastDiaoLuoTaskId = task_id;

	unsigned int dataLength = sizeof(SPCmd_QueryFubenDiaoluo);

	SimpleProtocolCmd cmd = queryFubenDiaoluo;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryFubenDiaoluo* _data = (SPCmd_QueryFubenDiaoluo*)bodyChar;
	_data->task_id = htonl(task_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdQueryFubenDiaoluoRsp(CCObject* f_msg_rsp)
{
	Flat_QueryFubenDiaoluoRsp* rsp = (Flat_QueryFubenDiaoluoRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		//lastDiaoLuoTaskId = 0;
		lastDiaoLuoQueue.pop();
		try_alert(rsp->err_code);
		return;
	}


	if (lastDiaoLuoQueue.front() > 0)
	{
		cmdBuffer->putDiaoLuo(lastDiaoLuoQueue.front(),*rsp);
		EquipFallManager::getManager()->addTaskEquipFall(lastDiaoLuoQueue.front(),*rsp);
	}
	lastDiaoLuoQueue.pop();
}

void CmdHelper::cmdNewAssistantJoinRsp(CCObject* f_msg_rsp)
{
	Flat_NewAssistantJoinRsp* data = (Flat_NewAssistantJoinRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (data->err_code != Success)
	{
		try_alert(data->err_code);
		return;
	}

	// 数据层添加数据
	{

		BaseActorProperty p(&data->hero_detail);
		HeroFixedPro extra = BaseActorPropertyParser::getParser()->getExtraProerty(p.profession);
		p.initFixedProValue(extra);

		// 先添加人
		MyselfManager::getManager()->addOneActor(p);

		// 添加装备
		int equipNum = data->equips.size();
		for (int i=0;i<equipNum;i++)
		{
			EquipmentManager::getManager()->addOneEquipment(data->equips[i]);
		}

		// 添加技能
		SkillManager::getManager()->addNewSkill(data->skillInfor);
	}
}

void CmdHelper::cmdQuerySubTaskList()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QuerySubTaskList);

	SimpleProtocolCmd cmd = querySubTaskList;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QuerySubTaskList* _data = (SPCmd_QuerySubTaskList*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdQuerySubTaskListRsp(CCObject* f_msg_rsp)
{
	Flat_QuerySubTaskListRsp* data = (Flat_QuerySubTaskListRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (data->err_code != Success)
	{
		try_alert(data->err_code);
		return;
	}
	TaskManager::getManager()->initSubTask(data->taskes);
}

void CmdHelper::cmdNewSubTaskGotRsp(CCObject* msg_rsp)
{
	CCAssert(false,"还么有做");
}

void CmdHelper::cmdSubTaskSubmitedRsp(CCObject* msg_rsp)
{
	SPCmd_SubTaskSubmitedRsp* rsp = (SPCmd_SubTaskSubmitedRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	WuPinManager::getManager()->addWuPin((ItemsType)rsp->reward_item_id,rsp->reward_item_count);
	TaskManager::getManager()->removeSubTask(rsp->task_id);
	SubTaskRewardItem subRward;
	subRward.task_id = rsp->task_id;
	subRward.reward_item_id = rsp->reward_item_id;
	subRward.reward_item_count = rsp->reward_item_count;
	TaskManager::getManager()->putSubTaskRewardItem(subRward);
}


void CmdHelper::cmdQueryMainTaskReward()
{
	if (TaskManager::getManager()->getMainTaskRewardVec().size() > 0)
	{
		CMD_LOG("%s : we find it in buffer,just return!",__FUNCTION__);

		Flat_QueryMainTaskRewardRsp rsp;
		rsp.err_code = Success;
		rsp.taskRewardVec = TaskManager::getManager()->getMainTaskRewardVec();
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryMainTaskRewardRsp_F,Flat_MSG_Rsp::createMsg(&rsp));

		return;
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryMainTaskReward);

	SimpleProtocolCmd cmd = queryMainTaskReward;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryMainTaskReward* _data = (SPCmd_QueryMainTaskReward*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryMainTaskRewardRsp(CCObject* f_msg_rsp)
{
	Flat_QueryMainTaskRewardRsp* rsp = (Flat_QueryMainTaskRewardRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 放入缓存
	TaskManager::getManager()->initMainTaskReward(rsp->taskRewardVec);
}
void CmdHelper::cmdChargeGold(unsigned int goldPacketId)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s,goldPacketId = %u",__FUNCTION__,goldPacketId);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ChargeGold);

	SimpleProtocolCmd cmd = chargeGold;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ChargeGold* _data = (SPCmd_ChargeGold*)bodyChar;
	_data->gold_packet_id = htonl(goldPacketId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdChargeGoldRsp(CCObject* msg_rsp)
{
	SPCmd_ChargeGoldRsp* rsp = (SPCmd_ChargeGoldRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 刷新金币
	WuPinManager::getManager()->setWuPinNum(ItemsType_Gold,rsp->gold_balance);
	// 刷新vip
	MyselfManager::getManager()->getMyZhuJueData()->setVipLevel(rsp->vip_level);
	MyselfManager::getManager()->setCharged(true);
	MyselfManager::getManager()->setNextVipLevel(rsp->next_vip_level);
	MyselfManager::getManager()->setNeedChargeToGetNextVip(rsp->next_need_charge);

	CMD_LOG("%s --> auto QueryVipPrivilege...",__FUNCTION__);
	// 查询vip数据
	cmdQueryVipPrivilege();
}

void CmdHelper::cmdQueryTeamSkillList()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryTeamSkillList);

	SimpleProtocolCmd cmd = queryTeamSkillList;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryTeamSkillList* _data = (SPCmd_QueryTeamSkillList*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryTeamSkillListRsp(CCObject* msg_rsp)
{
	SPCmd_QueryTeamSkillListRsp* rsp = (SPCmd_QueryTeamSkillListRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<SkillInfo> skills;
	for (int i=0;i<rsp->skill_count;i++)
	{
		skills.push_back(rsp->skill_arr[i]);
	}

	SkillManager::getManager()->initSkills(skills);
}

struct EquipSkillData
{
	unsigned int hero_id;
	unsigned int skill_id;
};
static EquipSkillData lastEquipSkillData;
void CmdHelper::cmdEquipSkill(unsigned int hero_id,unsigned int skill_id)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s,hero_id = %d,skill_id = %d",__FUNCTION__,hero_id,skill_id);
	removeLastBuff();

	lastEquipSkillData.hero_id = hero_id;
	lastEquipSkillData.skill_id = skill_id;

	unsigned int dataLength = sizeof(SPCmd_EquipSkill);

	SimpleProtocolCmd cmd = equipSkill;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_EquipSkill* _data = (SPCmd_EquipSkill*)bodyChar;
	_data->hero_id = htonl(hero_id);
	_data->skill_id = htonl(skill_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdEquipSkillRsp(CCObject* msg_rsp)
{
	SPCmd_QueryTeamSkillListRsp* rsp = (SPCmd_QueryTeamSkillListRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 更新技能
	BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(lastEquipSkillData.hero_id);
	SkillData* skill = SkillManager::getManager()->getSkillData(lastEquipSkillData.skill_id);
	CCAssert(actor!=NULL,"");
	CCAssert(skill!=NULL,"");
	actor->setCurrentUseSkill(skill);
}
void CmdHelper::cmdActorLearnSkill(unsigned int hero_id,unsigned int item_id)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s,hero_id = %d,item_id = %d",__FUNCTION__,hero_id,item_id);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ActorLearnSkill);

	SimpleProtocolCmd cmd = actorLearnSkill;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ActorLearnSkill* _data = (SPCmd_ActorLearnSkill*)bodyChar;
	_data->hero_id = htonl(hero_id);
	_data->item_id = htonl(item_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdActorLearnSkillRsp(CCObject* msg_rsp)
{
	SPCmd_ActorLearnSkillRsp* rsp = (SPCmd_ActorLearnSkillRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	SkillManager::getManager()->addNewSkill(rsp->new_skill);
}

void CmdHelper::cmdQueryAllShenbingList()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_QueryAllShenbingList REQUESR_DATA;
	SimpleProtocolCmd cmd = queryAllShenbingList;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdQueryAllShenbingListRsp(CCObject* f_msg_rsp)
{
	Flat_QueryAllShenbingListRsp* rsp = (Flat_QueryAllShenbingListRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	ChuanqiPool::getPool()->initChuanQiData(rsp->zbArray);
	ChuanqiPool::getPool()->modifySuitPoolToSurportShenBing();
}
void CmdHelper::cmdQueryTeamShenbingSuipian()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_QueryTeamShenbingSuipian REQUESR_DATA;
	SimpleProtocolCmd cmd = queryTeamShenbingSuipian;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryTeamShenbingSuipianRsp(CCObject* msg_rsp)
{
	SPCmd_QueryTeamShenbingSuipianRsp* rsp = (SPCmd_QueryTeamShenbingSuipianRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<ShenbingSuipianInfo> datas;
	for (int i=0;i<rsp->suipian_info_count;i++)
	{
		datas.push_back(rsp->suipian_into_arr[i]);
	}
	SuiPianManager::getManager()->initShenBingSuiPianData(datas);
}

static unsigned int lastShenBingHeCheng_zb_group_id = 0;
void CmdHelper::cmdShenbingHecheng(unsigned int zb_group_id)
{
	CMD_LOG("%s : zb_group_id = %u",__FUNCTION__,zb_group_id);
	// 检测装备
	CHECK_PKG(1,MSG_DuanzaoRsp_F);

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastShenBingHeCheng_zb_group_id = zb_group_id;

	typedef SPCmd_ShenbingHecheng REQUESR_DATA;
	SimpleProtocolCmd cmd = shenbingHecheng;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->zb_group_id = htonl(zb_group_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdShenbingHechengRsp(CCObject* f_msg_rsp)
{
	Flat_ShenbingHechengRsp* rsp = (Flat_ShenbingHechengRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 移除碎片
	int num = rsp->zhuangbei_vec.size();
	SuiPianManager::getManager()->addShenBingSuiPianNumByZBGroupId(lastShenBingHeCheng_zb_group_id,-num);

	// 添加装备
	for (int i=0,count = rsp->zhuangbei_vec.size(); i<count; i++)
	{
		EquipmentManager::getManager()->addOneEquipment(rsp->zhuangbei_vec[i]);
	}
}
void CmdHelper::cmdQueryTeamItemSuipian()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_QueryTeamItemSuipian REQUESR_DATA;
	SimpleProtocolCmd cmd = queryTeamItemSuipian;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryTeamItemSuipianRsp(CCObject* msg_rsp)
{
	SPCmd_QueryTeamItemSuipianRsp* rsp = (SPCmd_QueryTeamItemSuipianRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<ItemSuipianInfo> datas;
	for (int i=0;i<rsp->suipian_info_count;i++)
	{
		datas.push_back(rsp->suipian_into_arr[i]);
	}
	SuiPianManager::getManager()->initItemSuiPianData(datas);
}
void CmdHelper::cmdItemHecheng(unsigned int item_type)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s : item_type = %u",__FUNCTION__,item_type);
	removeLastBuff();

	typedef SPCmd_ItemHecheng REQUESR_DATA;
	SimpleProtocolCmd cmd = itemHecheng;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->item_id = htonl(item_type);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdItemHechengRsp(CCObject* msg_rsp)
{
	SPCmd_ItemHechengRsp* rsp = (SPCmd_ItemHechengRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}


	// 移除碎片
	int num = rsp->item_info.count;
	SuiPianManager::getManager()->addItemSuiPianNumByItemType((ItemsType)rsp->item_info.type,-num);

	// 添加Item
	WuPinManager::getManager()->addWuPin((ItemsType)rsp->item_info.type,rsp->item_info.count);
}

static SPCmd_Saodang lastSaoDangReq;
void CmdHelper::cmdSaodangFuben(SPCmd_Saodang req)
{
	CMD_LOG("%s",__FUNCTION__);
	{
		if ( req.saodang_time <= 0 )
		{
			POST_ERROR(CEC_SaoDangTimeMustBigger0,MSG_saodangFubenRsp_F);
			return;
		}

		if (MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft <= 0)
		{
			POST_ERROR(CEC_NoSaoDangTime,MSG_saodangFubenRsp_F);
			return;
		}

		if (MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft < req.saodang_time)
		{
			POST_ERROR(CEC_NoSaoDangTime,MSG_saodangFubenRsp_F);
			return;
		}

		// 检查背包
		CHECK_PKG(req.saodang_time,MSG_saodangFubenRsp_F);

	}
	
	if (req.auto_fenjie_huangzhuang == true)
	{
		CCLog("Waring : %s --->  auto_fenjie_huangzhuang is true,not supported, we will set it false");
		req.auto_fenjie_huangzhuang = false;
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("--------------------------------------");
	CMD_LOG("task_id = %u,saodang_time = %u,auto_fenjie_baizhuang = %s,auto_fenjie_lanzhuang = %s,auto_fenjie_huangzhuang = %s",
		req.task_id,req.saodang_time,
		req.auto_fenjie_baizhuang?"true":"false",
		req.auto_fenjie_lanzhuang?"true":"false",
		req.auto_fenjie_huangzhuang?"true":"false");
	CMD_LOG("---------------------------------------");

	removeLastBuff();

	lastSaoDangReq = req;

	typedef SPCmd_Saodang REQUESR_DATA;
	SimpleProtocolCmd cmd = saodangFuben;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->task_id = htonl(req.task_id);
	_data->saodang_time = htonl(req.saodang_time);
	_data->auto_fenjie_baizhuang = req.auto_fenjie_baizhuang;
	_data->auto_fenjie_lanzhuang = req.auto_fenjie_lanzhuang;
	_data->auto_fenjie_huangzhuang = req.auto_fenjie_huangzhuang;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdSaodangFubenRsp(CCObject* f_msg_rsp)
{
	Flat_SaodangRsp* rsp = (Flat_SaodangRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	if (MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft >= lastSaoDangReq.saodang_time)
	{
		MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft -= lastSaoDangReq.saodang_time;
	}
	else
	{
		MyselfManager::getManager()->getRuntionData()->saoDangTimeLeft = 0;
		CCLog("Error : %s --> saoDangTimeLeft is < 0!!");
		CCAssert(false,"");
	}
	

	// 刷新银币数据
//	WuPinManager::getManager()->setWuPinNum(ItemsType_Silver,rsp->silver_end_total);

	// 经验注入
	vector<BaseActor*> shangzhen_actors = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
	for (int i=0,count=shangzhen_actors.size(); i<count; i++)
	{
		addExpToActorOrHisYuanShenIfFullLevel(shangzhen_actors[i],rsp->exp);
	}

	// 物品添加
	for (int i=0,count=rsp->items.size(); i<count; i++)
	{
		WuPinManager::getManager()->addWuPin((ItemsType)rsp->items[i].type,rsp->items[i].count);
	}

// 	// 碎片添加
// 	for (int i=0,count=rsp->item_suipians.size(); i<count; i++)
// 	{
// 		SuiPianManager::getManager()->addOneItemSuiPian(rsp->item_suipians[i]);
// 	}
	
	// 装备添加
	for (int i=0,count=rsp->equips.size(); i<count; i++)
	{
		EquipmentManager::getManager()->addOneEquipment(rsp->equips[i]);
	}

	// 体力扣除
	unsigned int r, l;
	CS::getTaskChessboardFormat(lastSaoDangReq.task_id, r, l);
	int tiliCost = Vitality_GuanQia_Floped * r * l * lastSaoDangReq.saodang_time;
	BaseActor* zhujue = MyselfManager::getManager()->getMyZhuJueData();
	zhujue->setVitality(zhujue->getCurrentProperty().vit_current - tiliCost);

}
void CmdHelper::cmdHeroLevelUp(unsigned int hero_id)
{

	{
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(hero_id);
		if (actor == NULL)
		{
			CCAssert(false,"");
			return;
		}

		int code = 0;
		if (!actor->canUpdateLevel(code))
		{
			CCLog("%s : no need to send data...",__FUNCTION__);
			POST_ERROR(code,MSG_heroLevelUpRsp);
			return;
		}
	}


	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s hero_id = %d",__FUNCTION__,hero_id);
	removeLastBuff();

	typedef SPCmd_HeroLevelUp REQUESR_DATA;
	SimpleProtocolCmd cmd = heroLevelUp;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->hero_id = htonl(hero_id);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdHeroLevelUpRsp(CCObject* msg_rsp)
{
	SPCmd_HeroLevelUpRsp* rsp = (SPCmd_HeroLevelUpRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 刷新银币
	WuPinManager::getManager()->setWuPinNum(ItemsType_Silver,rsp->silver_end);
	// 刷新物品
	WuPinManager::getManager()->setWuPinNum((ItemsType)rsp->item_end.type,rsp->item_end.count);


	BaseActor* hero = MyselfManager::getManager()->getMyTeamActor(rsp->hero_detial.basic_info.hero_id);
	CCAssert(hero!=NULL,"");
	// 刷新数据
	hero->refreshBaseProPerty(rsp->hero_detial);

	// 主角
	if (hero->getParId() == 0 && hero->getCurrentProperty().level % 5 == 0)
	{
		CMD_LOG("%s --> auto query LevelBonus....",__FUNCTION__);
		//cmdQueryLevelBonus();
	}

	// 设置可以被解锁
	if (hero->getParId() == 0 && hero->getCurrentProperty_R()->level == CS::getUnlockAbleHeroLevel())
	{
		vector<BaseActor*> actors = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All,LF_Locked);
		for (int actorIndex=0,actorCount=actors.size(); actorIndex<actorCount; actorIndex++)
		{
			actors[actorIndex]->setCanUnlock(true);
		}
	}
}
void CmdHelper::cmdHeroUpgrade(unsigned int hero_id)
{
	{
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(hero_id);
		if (actor == NULL)
		{
			CCAssert(false,"");
			return;
		}
		int code = 0;
		if (!actor->canUpdateStage(code))
		{
			CCLog("%s : no need to send data...",__FUNCTION__);
			POST_ERROR(code,MSG_heroUpgradeRsp);
			return;
		}
	}


	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s hero_id = %d",__FUNCTION__,hero_id);
	removeLastBuff();

	typedef SPCmd_HeroUpgrade REQUESR_DATA;
	SimpleProtocolCmd cmd = heroUpgrade;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->hero_id = htonl(hero_id);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;


}
void CmdHelper::cmdHeroUpgradeRsp(CCObject* msg_rsp)
{
	SPCmd_HeroUpgradeRsp* rsp = (SPCmd_HeroUpgradeRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 刷新银币
	WuPinManager::getManager()->setWuPinNum(ItemsType_Silver,rsp->silver_end);
	// 刷新物品
	WuPinManager::getManager()->setWuPinNum((ItemsType)rsp->item_end.type,rsp->item_end.count);


	BaseActor* hero = MyselfManager::getManager()->getMyTeamActor(rsp->hero_detial.basic_info.hero_id);
	CCAssert(hero!=NULL,"");
	// 刷新数据
	hero->refreshBaseProPerty(rsp->hero_detial);
}
static unsigned long long last_jiefengying_zb_id = 0;
void CmdHelper::cmdShenbingJieFengyin(unsigned long long zb_id)
{
	// 检验
	{
		Equipment* equip = EquipmentManager::getManager()->getOneEquipment(zb_id);
		if (equip == NULL)
		{
			CMD_LOG("Error : [Will not send] %s equip[id = %lld] can not found!",__FUNCTION__,zb_id);
			return;
		}
		vector<CostData> cost_vec = equip->getShenBingJieFengyinCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			if( WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount() < cost_vec[i].count )
			{
				CMD_LOG("Waring : [Will not send] %s --> itemtype[%d] is not enough[%d < %d]",
					__FUNCTION__,
					cost_vec[i].item,
					WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount(),
					cost_vec[i].count);

				int code = Error;
				if (cost_vec[i].item == ItemsType_Shenbing_Jiefengyin)
				{
					code = CEC_Shenbing_JiefengyinItemNotEnough;
				}
				if (cost_vec[i].item == ItemsType_Silver)
				{
					code = CEC_SilverNotEnough;
				}
				POST_ERROR(code,MSG_shenbingJieFengyinRsp);
				return;
			}
		}
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s : zb_id = %lld",__FUNCTION__,zb_id);
	removeLastBuff();
	last_jiefengying_zb_id = zb_id;

	typedef SPCmd_ShenbingJieFengyin REQUESR_DATA;
	SimpleProtocolCmd cmd = shenbingJieFengyin;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->zb_id = LF::lf_htonll(zb_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdShenbingJieFengyinRsp(CCObject* msg_rsp)
{
	SPCmd_ShenbingJieFengyinRsp* rsp = (SPCmd_ShenbingJieFengyinRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}


	Equipment* equip = EquipmentManager::getManager()->getOneEquipment(last_jiefengying_zb_id);
	CCAssert(equip!=NULL,"");
	if (equip != NULL)
	{
		// 花费
		vector<CostData> cost_vec = equip->getShenBingJieFengyinCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			WuPinManager::getManager()->getWuPin(cost_vec[i].item)->setCount(
				WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount() - cost_vec[i].count);
		}


		equip->jieFengYing();
	}else
	{
		CCLog("Error : %s ---> equip can not found ,bug???",__FUNCTION__);
	}
}
static unsigned long long lastShenBingJiandingZbId = 0;
void CmdHelper::cmdShenbingJianding(unsigned long long zb_id)
{
	// 检验
	{
		Equipment* equip = EquipmentManager::getManager()->getOneEquipment(zb_id);
		if (equip == NULL)
		{
			CMD_LOG("Error : [Will not send] %s --> can not found the zb zb_id = %lld",__FUNCTION__,zb_id);
			return;
		}

		string why;
		if (!equip->canBeJianDing(why,false))
		{
			CMD_LOG("Waring : [Will not send] %s -->  this equip[Id = %lld] %s",
				__FUNCTION__,zb_id,why.c_str());

			POST_ERROR(Error,MSG_shenbingJiandingRsp);
			return;
		}

		// 花费
		vector<CostData> cost_vec = equip->getShenBingJianDingCurrentCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			if( WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount() < cost_vec[i].count )
			{
				CMD_LOG("Waring : [Will not send] %s --> itemtype[%d] is not enough[%d < %d]",
					__FUNCTION__,
					cost_vec[i].item,
					WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount(),
					cost_vec[i].count);

				int code = Error;
				if (cost_vec[i].item == ItemsType_Shenbing_Jianding)
				{
					code = CEC_Shenbing_JiandingItemNotEnough;
				}
				if (cost_vec[i].item == ItemsType_Silver)
				{
					code = CEC_SilverNotEnough;
				}

				POST_ERROR(code,MSG_shenbingJiandingRsp);
				return;
			}
		}
	}


	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s : zb_id = %lld",__FUNCTION__,zb_id);
	removeLastBuff();
	lastShenBingJiandingZbId = zb_id;

	typedef SPCmd_ShenbingJianding REQUESR_DATA;
	SimpleProtocolCmd cmd = shenbingJianding;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->zb_id = LF::lf_htonll(zb_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdShenbingJiandingRsp(CCObject* msg_rsp)
{
	SPCmd_ShenbingJiandingRsp* rsp = (SPCmd_ShenbingJiandingRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	Equipment* equip = EquipmentManager::getManager()->getOneEquipment(lastShenBingJiandingZbId);
	CCAssert(equip!=NULL,"");
	if (equip != NULL)
	{

		// 花费
		vector<CostData> cost_vec = equip->getShenBingJianDingCurrentCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			WuPinManager::getManager()->getWuPin(cost_vec[i].item)->setCount(
				WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount() - cost_vec[i].count);
		}

		// 刷新属性
		vector<ZBAttr> att_vec;
		for (int i=0;i<rsp->identified_attr_count;i++)
		{
			att_vec.push_back(rsp->identified_attr_vec[i]);
		}
		equip->setKownedHideAttribute(att_vec);

	}else
	{
		CCLog("Error : %s ---> equip can not found ,bug???",__FUNCTION__);
	}
	
}

struct ZBJinglianData
{
	unsigned long long zb_id;
	unsigned long long cailiao_zb_id;
};
static ZBJinglianData lastZBJinglianData;
void CmdHelper::cmdZBJinglian(unsigned long long zb_id,unsigned long long cailiao_zb_id)
{
	//
	{
		// 不能用自己
		if (zb_id == cailiao_zb_id)
		{
			CMD_LOG("Error : [Will not send] %s --> zb_id == cailiao_zb_id",__FUNCTION__);
			return;
		}

		Equipment* cailiao = EquipmentManager::getManager()->getOneEquipment(cailiao_zb_id);
		if (cailiao == NULL)
		{
			CMD_LOG("Error : [Will not send] %s --> can not found the zb cailiao_zb_id = %lld",__FUNCTION__,cailiao_zb_id);
			return;
		}
		Equipment* equip = EquipmentManager::getManager()->getOneEquipment(zb_id);
		if (equip == NULL)
		{
			CMD_LOG("Error : [Will not send] %s --> can not found the zb zb_id = %lld",__FUNCTION__,zb_id);
			return;
		}
		string infor_why;
		if (!equip->canJingLianUseTheEquip(infor_why,cailiao,false))
		{
			CMD_LOG("Waring : [Will not send] %s -->  %s!",
				__FUNCTION__,infor_why.c_str());

			POST_ERROR(CEC_CanNotJingLian,MSG_ZBJinglianRsp);
			return;
		}

		// 花费
		vector<CostData> cost_vec = equip->getZbJingLianCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			if(WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount() < cost_vec[i].count)
			{
				CMD_LOG("Waring : [Will not send] %s --> itemtype[%d] is not enough[%d < %d]",
					__FUNCTION__,
					cost_vec[i].item,
					WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount(),
					cost_vec[i].count);

				int code = Error;
				if (cost_vec[i].item == ItemsType_Silver)
				{
					code = CEC_SilverNotEnough;
				}

				POST_ERROR(code,MSG_ZBJinglianRsp);
				return;
			}
		}
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s : zb_id = %lld,cailiao_zb_id = %lld",__FUNCTION__,zb_id,cailiao_zb_id);
	removeLastBuff();
	lastZBJinglianData.zb_id = zb_id;
	lastZBJinglianData.cailiao_zb_id = cailiao_zb_id;

	typedef SPCmd_ZBJinglian REQUESR_DATA;
	SimpleProtocolCmd cmd = zbJinglian;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->zb_id = LF::lf_htonll(zb_id);
	_data->cailiao_zb_id = LF::lf_htonll(cailiao_zb_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdZBJinglianRsp(CCObject* msg_rsp)
{
	SPCmd_ZBJinglianRsp* rsp = (SPCmd_ZBJinglianRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	EquipmentManager::getManager()->destoryOneEquip(lastZBJinglianData.cailiao_zb_id);
	Equipment* equip = EquipmentManager::getManager()->getOneEquipment(lastZBJinglianData.zb_id);
	CCAssert(equip!=NULL,"");
	if (equip != NULL)
	{
		// 花费
		vector<CostData> cost_vec = equip->getZbJingLianCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			WuPinManager::getManager()->getWuPin(cost_vec[i].item)->setCount(
				WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount() - cost_vec[i].count);
		}

		// 材料回收
		for (int i=0;i<rsp->count;i++)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)rsp->item_arr[i].type,rsp->item_arr[i].count);
		}

		// 注入精炼的数据
		vector<ZBAttr> jingLianVec;
		for (int i=0; i<sizeof(rsp->jinglian_data.jinglian_attr_arr)/sizeof(ZBAttr); i++ )
		{
			if (rsp->jinglian_data.jinglian_attr_arr[i].attr_type > 0) // 只有大于0 才是有效的
			{
				jingLianVec.push_back(rsp->jinglian_data.jinglian_attr_arr[i]);
			}
		}
		// 下一次的数据
		vector<ZBAttr> next;
		//for (int i=0; i<sizeof(rsp->jinglian_data.next_jinglian_attr_arr)/sizeof(ZBAttr); i++ )
		//{
		//	if (rsp->jinglian_data.next_jinglian_attr_arr[i].attr_type > 0) // 只有大于0 才是有效的
		//	{
		//		next.push_back(rsp->jinglian_data.next_jinglian_attr_arr[i]);
		//	}
		//}

		equip->setJingLianAttribute(jingLianVec,rsp->jinglian_data.jinglian_times,next);

	}else
	{
		CCLog("Error : %s ---> equip can not found ,bug???",__FUNCTION__);
	}
}


static unsigned long long lastZhuankeZbId = 0;
void CmdHelper::cmdShenbingZhuankeJingwen(unsigned long long zb_id)
{
	//
	{
		Equipment* equip = EquipmentManager::getManager()->getOneEquipment(zb_id);
		if (equip == NULL)
		{
			CMD_LOG("Error : [Will not send] %s --> can not found the zb zb_id = %lld",__FUNCTION__,zb_id);
			return;
		}
		string infor_why;
		if (!equip->canBeZhuanKe(infor_why,false))
		{
			CMD_LOG("Waring : [Will not send] %s -->  %s!",
				__FUNCTION__,infor_why.c_str());

			POST_ERROR(CEC_CanNotZhuanke,MSG_shenbingZhuankeJingwenRsp);
			return;
		}

		// 花费
		vector<CostData> cost_vec = equip->getShenBingZhuankeCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			if(WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount() < cost_vec[i].count)
			{
				CMD_LOG("Waring : [Will not send] %s --> itemtype[%d] is not enough[%d < %d]",
					__FUNCTION__,
					cost_vec[i].item,
					WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount(),
					cost_vec[i].count);

				int code = Error;
				if (cost_vec[i].item == ItemsType_Silver)
				{
					code = CEC_SilverNotEnough;
				}
				if (cost_vec[i].item == ItemsType_JingwenZhuanke)
				{
					code = CEC_JingwenZhuankeItemNotEnough;
				}

				POST_ERROR(code,MSG_shenbingZhuankeJingwenRsp);
				return;
			}
		}
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s : zb_id = %lld",__FUNCTION__,zb_id);
	removeLastBuff();
	lastZhuankeZbId = zb_id;

	typedef SPCmd_ShenbingZhuankeJingwen REQUESR_DATA;
	SimpleProtocolCmd cmd = shenbingZhuankeJingwen;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->zb_id = LF::lf_htonll(zb_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdShenbingZhuankeJingwenRsp(CCObject* msg_rsp)
{
	SPCmd_ShenbingZhuankeJingwenRsp* rsp = (SPCmd_ShenbingZhuankeJingwenRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	Equipment* equip = EquipmentManager::getManager()->getOneEquipment(lastZhuankeZbId);
	CCAssert(equip!=NULL,"");
	if (equip != NULL)
	{
		// 花费
		vector<CostData> cost_vec = equip->getShenBingZhuankeCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			WuPinManager::getManager()->getWuPin(cost_vec[i].item)->setCount(
				WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount() - cost_vec[i].count);
		}

		// 注入篆刻的数据
		vector<ZBAttr> zhuanke_vec;
		for (int i=0; i<sizeof(rsp->jingwen_attr)/sizeof(ZBAttr); i++ )
		{
			if (rsp->jingwen_attr[i].attr_type > 0) // 只有大于0 才是有效的
			{
				zhuanke_vec.push_back(rsp->jingwen_attr[i]);
			}
		}
		equip->setZhuanKeAttribute(zhuanke_vec);

	}else
	{
		CCLog("Error : %s ---> equip can not found ,bug???",__FUNCTION__);
	}
}

void CmdHelper::cmdQueryArenaTeamBasicInfo()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_QueryArenaTeamBasicInfo REQUESR_DATA;
	SimpleProtocolCmd cmd = queryArenaTeamBasicInfo;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = LF::lf_htonll(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryArenaTeamBasicInfoRsp(CCObject* msg_rsp)
{
	SPCmd_QueryArenaTeamBasicInfoRsp* rsp = (SPCmd_QueryArenaTeamBasicInfoRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 初始化
	MyselfManager::getManager()->initArenaTeamBasicInfo(rsp->team_info);
}

void CmdHelper::cmdQueryArenaMarket()
{
	// 查询缓存
	GameDirector::getDirector()->showWaiting();
	if (MarketManager::getManager()->itemCountInArena())
	{
		Flat_QueryArenaMarketRsp rsp;
		rsp.err_code = Success;
		rsp.item_list = MarketManager::getManager()->getAllItemsInArena();
		CMD_LOG("%s,we found in buffer,just return!",__FUNCTION__);
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryArenaMarketRsp_F,Flat_MSG_Rsp::createMsg(&rsp));
		return;
	}


	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_QueryArenaMarket REQUESR_DATA;
	SimpleProtocolCmd cmd = queryArenaMarket;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = LF::lf_htonll(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryArenaMarketRsp(CCObject* f_msg_rsp)
{
	Flat_QueryArenaMarketRsp* rsp = (Flat_QueryArenaMarketRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	MarketManager::getManager()->initInArena(rsp->item_list);
}

static ItemInfo temp_RefreshArenaChlgList_needItem;
void CmdHelper::cmdQueryArenaChlgList()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_QueryArenaChallengeList REQUESR_DATA;
	SimpleProtocolCmd cmd = queryArenaChlgList;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryArenaChlgListRsp(CCObject* f_msg_rsp)
{
	Flat_QueryArenaChallengeListRsp* rsp = (Flat_QueryArenaChallengeListRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 保存值
	temp_RefreshArenaChlgList_needItem = rsp->item_need;
}
void CmdHelper::cmdRefreshArenaChlgList()
{
	CMD_LOG("%s",__FUNCTION__);

	if (WuPinManager::getManager()->getWuPin((ItemsType)temp_RefreshArenaChlgList_needItem.type)->getCount() < temp_RefreshArenaChlgList_needItem.count)
	{
		int erroCode = CEC_ItemNotEnough_Begin + temp_RefreshArenaChlgList_needItem.type;
		POST_ERROR(erroCode,MSG_refreshArenaChlgList_F);
		return;
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_RefreshArenaChallengeList REQUESR_DATA;
	SimpleProtocolCmd cmd = refreshArenaChlgList;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdRefreshArenaChlgListRsp(CCObject* f_msg_rsp)
{
	Flat_RefreshArenaChallengeListRsp* rsp = (Flat_RefreshArenaChallengeListRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 刷新金币数量
	WuPinManager::getManager()->setWuPinNum(ItemsType_Gold,rsp->gold_balance);
	WuPinManager::getManager()->setWuPinNum(ItemsType_Silver,rsp->silver_balance);
	// 下次刷新的金币
	// 保存值
	temp_RefreshArenaChlgList_needItem = rsp->item_need;
}


void CmdHelper::cmdArenaChallenge(unsigned int challenge_hero_id)
{
	if (MyselfManager::getManager()->getEdiableArenaTeamBasicInfo() == NULL)
	{
		CCLog("Error : %s --> getEdiableArenaTeamBasicInfo = NULL",__FUNCTION__);
		return;
	}
	// 检测是否还有次数打
	if (MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->challenge_remainder_time <= 0)
	{
		CCLog("Waring : %s  challenge_remainder_time <= 0",__FUNCTION__);

		POST_ERROR(CEC_NoArenaClgTime,MSG_arenaChallengeRsp_F);
		return;
	}


	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s --> challenge_hero_id = [%u]",__FUNCTION__,challenge_hero_id);
	removeLastBuff();

	typedef SPCmd_ArenaChallenge REQUESR_DATA;
	SimpleProtocolCmd cmd = arenaChallenge;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->challenge_hero_id = htonl(challenge_hero_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdArenaChallengeRsp(CCObject* f_msg_rsp)
{
	Flat_ArenaChallengeRsp* rsp = (Flat_ArenaChallengeRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<FightScript> scripts;
	scripts.push_back(rsp->fightScript);
	storeAllGettedInFight(scripts,false);

	if(MyselfManager::getManager()->getEdiableArenaTeamBasicInfo() != NULL )
	{
		// 赢了才扣次数
		if (scripts[scripts.size()-1].endResult.myResult == Result_Win)
		{
			if (MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->challenge_remainder_time > 0)
			{
				MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->challenge_remainder_time --;
			}
			else
			{
				CCLog("Error : %s ---> challenge_remainder_time <=0 !",__FUNCTION__);
				MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->challenge_remainder_time = 0;
			}
		}

		MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->jifen = rsp->jifen_after_fight;
	}else
	{
		CCLog("Error : %s getEdiableArenaTeamBasicInfo == NULL",__FUNCTION__);
	}
}

struct ArenaMarketBuyData
{
	unsigned int item_type;
	unsigned int item_count;
};
static ArenaMarketBuyData lastArenaMarketBuyData;
void CmdHelper::cmdArenaMarketBuy(unsigned int item_type,unsigned int item_count)
{
	int existIndexInMarket = -1;
	// 检测
	{
		if (item_count == 0)
		{
			CCLog("Error : %s ---> item_count = 0",__FUNCTION__);
			CCAssert(false,"");
			return;
		}

		MarketManager* market = MarketManager::getManager();
		for (int i=0,count = market->itemCountInArena(); i<count; i++ )
		{
			//if (market->getItemInArenaAtIndex(i).item_type == item_type && market->getItemInArenaAtIndex(i).item_count == item_count)
			if (market->getItemInArenaAtIndex(i).item_type == item_type)
			{
				CCAssert(existIndexInMarket == -1,"一定要之前没有找到过！不然就是bug了");
				existIndexInMarket = i;
				// 下面不break;为了就是要检验一下数据
			}
		}

		if (existIndexInMarket == -1)
		{
			CCLog("%s --> Error : the item[type = %d] is not found in the market!",__FUNCTION__,item_type);

			POST_ERROR(Error,MSG_arenaMarketBuyRsp);

			return;
		}

		ArenaTeamBasicInfo teamInfor;
		bool must_found = MyselfManager::getManager()->getArenaTeamBasicInfo(teamInfor);
		CCAssert(must_found,"");
		if (!must_found)
		{
			CCLog("%s --> Error : can not get current ArenaTeamBasicInfo",__FUNCTION__);
			POST_ERROR(Error,MSG_arenaMarketBuyRsp);
			return;
		}

		if (market->getItemInArenaAtIndex(existIndexInMarket).jifen_count > teamInfor.jifen)
		{
			CCLog("%s --> Error : jifen is not enough [%d < %d]",
				__FUNCTION__,
				teamInfor.jifen,
				market->getItemInArenaAtIndex(existIndexInMarket).jifen_count);
			POST_ERROR(CEC_ArenaMarketBuyJiFenNotEnough,MSG_arenaMarketBuyRsp);
			return;
		}

	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s --> item_type = [%u] item_count = [%u]",__FUNCTION__,item_type,item_count);
	removeLastBuff();

	MarketManager* market = MarketManager::getManager();
	int count = market->getItemInArenaAtIndex(existIndexInMarket).item_count *  item_count;

	lastArenaMarketBuyData.item_type = item_type;
	lastArenaMarketBuyData.item_count = count;

	typedef SPCmd_ArenaMarketBuy REQUESR_DATA;
	SimpleProtocolCmd cmd = arenaMarketBuy;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->item_type = htonl(item_type);
	_data->item_count = htonl(count); //

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdArenaMarketBuyRsp(CCObject* msg_rsp)
{
	SPCmd_ArenaMarketBuyRsp* rsp = (SPCmd_ArenaMarketBuyRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 添加Item
	WuPinManager::getManager()->addWuPin((ItemsType)lastArenaMarketBuyData.item_type,lastArenaMarketBuyData.item_count);
	// 刷新积分
	MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->jifen = rsp->jifen_balance;
}

void CmdHelper::cmdArenaBuyChallengeTime(bool is_max_buy)
{
	if (MyselfManager::getManager()->getEdiableArenaTeamBasicInfo() == NULL)
	{
		CCLog("Error : %s getEdiableArenaTeamBasicInfo = NULL",__FUNCTION__);
		return;
	}
	if (MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->buy_challenge_remainder_time <= 0)
	{
		CCLog("Error : %s buy_challenge_remainder_time <= 0 can not buy!",__FUNCTION__);
		POST_ERROR(CEC_NoArenaBuyClgTime,MSG_arenaBuyChlgTimeRsp);
		return;
	}

	int needGold = CS::arenaBuyOneChlgTimeNeedGold();
	int buyTime = 1;
	if (is_max_buy)
	{
		int time1 = MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->buy_challenge_remainder_time;
		int time2 = WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount()/needGold;
		buyTime = min(time1,time2);
	}
	needGold *= buyTime;

	if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() < needGold)
	{
		CCLog("Error : %s ItemsType_Gold is not enough [%d < %d],buy time = [%d]!",__FUNCTION__,
			WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount(),
			needGold,buyTime);
		POST_ERROR(CEC_GoldNotEnough,MSG_arenaBuyChlgTimeRsp);
		return;
	}


	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s --> buyTime = [%d]",__FUNCTION__,buyTime);
	removeLastBuff();

	typedef SPCmd_ArenaBuyChallengeTime REQUESR_DATA;
	SimpleProtocolCmd cmd = arenaBuyChlgTime;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->buy_time = htonl(buyTime);
	

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdArenaBuyChallengeTimeRsp(CCObject* msg_rsp)
{
	SPCmd_ArenaBuyChallengeTimeRsp* rsp = (SPCmd_ArenaBuyChallengeTimeRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 金币
	WuPinManager::getManager()->setWuPinNum(ItemsType_Gold,rsp->gold_balance);

	// 购买次数
	if (MyselfManager::getManager()->getEdiableArenaTeamBasicInfo() != NULL)
	{
		MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->buy_challenge_remainder_time = rsp->buy_challenge_remainder_time;
		MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->challenge_remainder_time = rsp->challenge_remainder_time;

		// 更新值，直接用竞技场数据
		MyselfManager::getManager()->getRuntionData()->buyArenaChallengeTimeLeft = MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->buy_challenge_remainder_time;
	}
	else
	{
		CCLog("Error : %s getEdiableArenaTeamBasicInfo is NULL",__FUNCTION__);
	}
}


void CmdHelper::cmdQueryZahuopu()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_QueryZahuopu REQUESR_DATA;
	SimpleProtocolCmd cmd = queryZahuopu;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryZahuopuRsp(CCObject* msg_rsp)
{
	SPCmd_QueryZahuopuRsp* rsp = (SPCmd_QueryZahuopuRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<ZahuopuItem> items;
	int count = 6;
	for (int i=0; i<count; i++)
	{
		items.push_back(rsp->item_arr[i]);
	}
	ZahuopuHelper::getHelper()->setRefreshNeedSilver(rsp->silver_need_for_refresh);
	ZahuopuHelper::getHelper()->setPickItemNeedDaiGouJuan(rsp->draw_need_coupons);
	ZahuopuHelper::getHelper()->setCurrentZaHuoList(items);
}

void CmdHelper::cmdRefreshZahuopu()
{
	{
		if (WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount() < ZahuopuHelper::getHelper()->getRefreshNeedSilver())
		{
			CMD_LOG("%s --> silver is not enough[%d < %u]",
				__FUNCTION__,
				WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount(),
				ZahuopuHelper::getHelper()->getRefreshNeedSilver());

			POST_ERROR(CEC_SilverNotEnough,MSG_refreshZahuopuRsp);

			return;
		}

	}
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_RefreshZahuopu REQUESR_DATA;
	SimpleProtocolCmd cmd = refreshZahuopu;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdRefreshZahuopuRsp(CCObject* msg_rsp)
{
	SPCmd_RefreshZahuopuRsp* rsp = (SPCmd_RefreshZahuopuRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<ZahuopuItem> items;
	int count = 6;
	for (int i=0; i<count; i++)
	{
		items.push_back(rsp->item_arr[i]);
	}

	WuPinManager::getManager()->setWuPinNum(ItemsType_Silver,rsp->silver_balance);
	WuPinManager::getManager()->setWuPinNum(ItemsType_DaiGouJuan,rsp->coupons_balance);
	ZahuopuHelper::getHelper()->setRefreshNeedSilver(rsp->silver_need_for_refresh);
	ZahuopuHelper::getHelper()->setPickItemNeedDaiGouJuan(rsp->draw_need_coupons);

	ZahuopuHelper::getHelper()->setCurrentZaHuoList(items);
}

void CmdHelper::cmdZahuopuGetOneItem()
{
	{
		if (ZahuopuHelper::getHelper()->getNumOfNotGetted() <= 0)
		{
			CMD_LOG("%s --> not item to get!",
				__FUNCTION__);
			POST_ERROR(CEC_ZahuopuGetOneItem_NoItem,MSG_zahuopuGetOneItemRsp_F);
			return;
		}

		int daigouquan_num = WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount();
		if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() < CS::zahuopuGetOneItemNeedGold(daigouquan_num))
		{
			CMD_LOG("%s --> gold is not enough[%d < %d]",
				__FUNCTION__,
				WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount(),
				CS::zahuopuGetOneItemNeedGold(daigouquan_num));

			POST_ERROR(CEC_GoldNotEnough,MSG_zahuopuGetOneItemRsp_F);

			return;
		}

	}
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_ZahuopuGetOneItem REQUESR_DATA;
	SimpleProtocolCmd cmd = zahuopuGetOneItem;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdZahuopuGetOneItemRsp(CCObject* f_msg_rsp)
{
	Flat_ZahuopuGetOneItemRsp* rsp = (Flat_ZahuopuGetOneItemRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	ZahuopuItem item = ZahuopuHelper::getHelper()->getItemByIndex(rsp->get_index);
	if (item.suipian_id == 0)
	{
		WuPinManager::getManager()->addWuPin((ItemsType)item.item_type,item.count);
	}
	else
	{
		ItemSuipianInfo suipian;
		suipian.item_id = item.item_type;
		suipian.suipian_id = item.suipian_id;
		suipian.suipian_count = item.count;
		SuiPianManager::getManager()->addOneItemSuiPian(suipian);
	}

	// 设置已经获取了
	ZahuopuHelper::getHelper()->setIsGettedByIndex(rsp->get_index);
	// 刷新金币
	WuPinManager::getManager()->setWuPinNum(ItemsType_Gold,rsp->gold_balance);
	// 刷新代购卷
	WuPinManager::getManager()->setWuPinNum(ItemsType_DaiGouJuan,rsp->coupons_balance);
}

void CmdHelper::cmdQueryHellTaskHeroInfo()
{
	{
		if (MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < HERO_LEVEL_HELL_TASK_OPEN)
		{
			POST_ERROR(CEC_HeroLevelNotEnough,MSG_queryHellTaskHeroInfoRsp);
			return;
		}
	}
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_QueryHellTaskHeroInfo REQUESR_DATA;
	SimpleProtocolCmd cmd = queryHellTaskHeroInfo;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = 0;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryHellTaskHeroInfoRsp(CCObject* msg_rsp)
{
	SPCmd_QueryHellTaskHeroInfoRsp* rsp = (SPCmd_QueryHellTaskHeroInfoRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 刷新
	TaskManager::getManager()->setHellBattelTimeRemainder(rsp->battle_time_remainder);
	TaskManager::getManager()->setLastActiveHellTaskId(rsp->last_active_task_id);
}

void CmdHelper::cmdHellTaskBattle(unsigned int task_id)
{
	{
		if (MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < HERO_LEVEL_HELL_TASK_OPEN)
		{
			POST_ERROR(CEC_HeroLevelNotEnough,MSG_hellTaskBattleRsp_F);
			return;
		}
		if (TaskManager::getManager()->getHellBattelTimeRemainder() < 1)
		{
			POST_ERROR(CEC_NoHellBattleTime,MSG_hellTaskBattleRsp_F);
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s : task_id = %u",__FUNCTION__,task_id);
	removeLastBuff();

	typedef SPCmd_HellTaskBattle REQUESR_DATA;
	SimpleProtocolCmd cmd = hellTaskBattle;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->task_id = htonl(task_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;

}
void CmdHelper::cmdHellTaskBattleRsp(CCObject* f_msg_rsp)
{
	Flat_HellTaskBattleRsp* rsp = (Flat_HellTaskBattleRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	storeAllGettedInFight(rsp->fightScriptVec,true);
	TaskManager::getManager()->subHellBattelTimeRemainder();
}

void CmdHelper::cmdQueryYuanshenOfHero(unsigned int heroId)
{
	{
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(heroId);
		// 如果是自己队伍里面的人，那么就判断他的等级
		// 等级不足就没有元神
		if(actor != NULL && actor->getLevel() < ACTOR_MAX_LEVEL)
		{
			POST_ERROR(CEC_HeroLevelNotEnough,MSG_queryYuanshenOfHeroRsp_F);
			return;
		}

		//if (MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < ACTOR_MAX_LEVEL)
		//{
		//	POST_ERROR(CEC_HeroLevelNotEnough,MSG_queryYuanshenOfHeroRsp_F);
		//	return;
		//}
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s : heroId = %u",__FUNCTION__,heroId);
	removeLastBuff();

	typedef SPCmd_QueryYuanshenOfHero REQUESR_DATA;
	SimpleProtocolCmd cmd = queryYuanshenOfHero;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->hero_id = htonl(heroId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryYuanshenOfHeroRsp(CCObject* f_msg_rsp)
{
	Flat_QueryYuanshenOfHeroRsp* rsp = (Flat_QueryYuanshenOfHeroRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}


	// 数据层添加数据
	{
		BaseActor* zhenshen = MyselfManager::getManager()->getMyTeamActor(rsp->hero_detail.basic_info.zhenshen_id);
		// 如果真身存在于我们自己的队伍里面，而且元神没有，那么就添加数据
		if (zhenshen != NULL && zhenshen->getMyYuanShen() == NULL)
		{
			BaseActorProperty p(&rsp->hero_detail);
			HeroFixedPro extra = BaseActorPropertyParser::getParser()->getExtraProerty(p.profession);
			p.initFixedProValue(extra);

			// 先添加人
			MyselfManager::getManager()->addOneActor(p,rsp->hero_detail.basic_info.zhenshen_id);

			// 添加装备
			int equipNum = rsp->equips.size();
			for (int i=0;i<equipNum;i++)
			{
				EquipmentManager::getManager()->addOneEquipment(rsp->equips[i]);
			}
		}
	}

}
void CmdHelper::cmdChurchLevelUp()
{
	{
		if (MyselfManager::getManager()->getJiaoTangLevel() >= HERO_MAX_STAGE)
		{
			POST_ERROR(CEC_JiaoTangIsMaxLevel,MSG_churchLevelUpRsp);
			return;
		}
		unsigned int dalishi_need = 0;
		unsigned int huagangyan_need = 0;
		unsigned int mocai_need = 0;
		unsigned int shuijinshi_need = 0;
		unsigned int silver_need = 0;
		CS::churchLevelUpNeedItemCount(MyselfManager::getManager()->getJiaoTangLevel()+1,
			dalishi_need,huagangyan_need,mocai_need,shuijinshi_need
			);
		silver_need = CS::churchLevelUpNeedSilver(MyselfManager::getManager()->getJiaoTangLevel()+1);

		if (WuPinManager::getManager()->getWuPin(ItemsType_Dalishi)->getCount() < dalishi_need)
		{
			int errorCode = ItemsType_Dalishi+CEC_ItemNotEnough_Begin;
			POST_ERROR(errorCode,MSG_churchLevelUpRsp);
			return;
		}
		if (WuPinManager::getManager()->getWuPin(ItemsType_Huagangyan)->getCount() < huagangyan_need)
		{
			int errorCode = ItemsType_Huagangyan+CEC_ItemNotEnough_Begin;
			POST_ERROR(errorCode,MSG_churchLevelUpRsp);
			return;
		}
		if (WuPinManager::getManager()->getWuPin(ItemsType_Mucai)->getCount() < mocai_need)
		{
			int errorCode = ItemsType_Mucai+CEC_ItemNotEnough_Begin;
			POST_ERROR(errorCode,MSG_churchLevelUpRsp);
			return;
		}
		if (WuPinManager::getManager()->getWuPin(ItemsType_Shuijingshi)->getCount() < shuijinshi_need)
		{
			int errorCode = ItemsType_Shuijingshi+CEC_ItemNotEnough_Begin;
			POST_ERROR(errorCode,MSG_churchLevelUpRsp);
			return;
		}
		if (WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount() < silver_need)
		{
			int errorCode = ItemsType_Silver+CEC_ItemNotEnough_Begin;
			POST_ERROR(errorCode,MSG_churchLevelUpRsp);
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s",__FUNCTION__);
	removeLastBuff();

	typedef SPCmd_ChurchLevelUp REQUESR_DATA;
	SimpleProtocolCmd cmd = churchLevelUp;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdChurchLevelUpRsp(CCObject* msg_rsp)
{
	SPCmd_ChurchLevelUpRsp* rsp = (SPCmd_ChurchLevelUpRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 更新数据
	JiaoTang jiaotang = MyselfManager::getManager()->getJiaoTangData();
	jiaotang.level = rsp->church_level;//
	MyselfManager::getManager()->setCurrentJiaoTangData(jiaotang);

	WuPinManager::getManager()->setWuPinNum(ItemsType_Dalishi,rsp->dalishi_balance);
	WuPinManager::getManager()->setWuPinNum(ItemsType_Huagangyan,rsp->huagangyan_balance);
	WuPinManager::getManager()->setWuPinNum(ItemsType_Mucai,rsp->mucai_balance);
	WuPinManager::getManager()->setWuPinNum(ItemsType_Shuijingshi,rsp->shuijinshi_balance);
	WuPinManager::getManager()->setWuPinNum(ItemsType_Silver,rsp->silver_balance);
}

static unsigned int lastSkillLevelUpId = 0;
void CmdHelper::cmdSkillLevelUp(unsigned int owner_hero_id,unsigned int skill_id)
{
	{
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(owner_hero_id);
		CCAssert(actor,"");
		if (actor == NULL)
			return;
		//SKILL_MAX_LEVEL
		SkillData* skillData = SkillManager::getManager()->getSkillData(skill_id);
		CCAssert(skillData!=NULL,"");
		if(skillData == NULL)
			return;

		if (skillData->getInfor()->skill_level >= SKILL_MAX_LEVEL)
		{
			POST_ERROR(CEC_SkillIsMaxLevel,MSG_skillLevelUpRsp);
			return;
		}

		// 消耗技能书
		ItemsType skillItem = (ItemsType)(ItemsType_SkillBook_MinId+skillData->getInfor()->skill_type);
		if (WuPinManager::getManager()->getWuPin(skillItem)->getCount() < 1)
		{
			int errorCode = CEC_ItemNotEnough_Begin+skillItem;
			POST_ERROR(errorCode,MSG_skillLevelUpRsp);
			return;
		}

		unsigned int silverNeed = CS::skillLevelUpNeedSilver(skillData->getInfor()->skill_level+1);
		if (WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount() < silverNeed)
		{
			int errorCode = CEC_ItemNotEnough_Begin + ItemsType_Silver;
			POST_ERROR(errorCode,MSG_skillLevelUpRsp);
			return;
		}

	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s owner_hero_id = %u,skill_id = %u",__FUNCTION__,owner_hero_id,skill_id);
	removeLastBuff();

	lastSkillLevelUpId = skill_id;

	typedef SPCmd_SkillLevelUp REQUESR_DATA;
	SimpleProtocolCmd cmd = skillLevelUp;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->skill_id = htonl(skill_id);
	_data->owner_hero_id = htonl(owner_hero_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdSkillLevelUpRsp(CCObject* msg_rsp)
{
	SPCmd_SkillLevelUpRsp* rsp = (SPCmd_SkillLevelUpRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	SkillData* skillData = SkillManager::getManager()->getSkillData(lastSkillLevelUpId);
	CCAssert(skillData!=NULL,"");
	if (skillData == NULL)
		return;
	// 刷新数据
	skillData->getInfor()->skill_level = rsp->new_skill_level;
	WuPinManager::getManager()->setWuPinNum(ItemsType_Silver,rsp->silver_balance);
	WuPinManager::getManager()->setWuPinNum((ItemsType)rsp->skill_book_end.type,rsp->skill_book_end.count);
}

static int lastGetZhuangbeiAlbumprofession = 0;
void CmdHelper::cmdGetZhuangbeiAlbum(int profession)
{
	CMD_LOG("%s,profession = %d",__FUNCTION__,profession);
	// 检查buff
	{
		Flat_GetZhuangbeiAlbumRsp rsp;
		if (cmdBuffer->getZhuangBeiAlbum(profession,rsp))
		{
			// 更新值
			lastGetZhuangbeiAlbumprofession = profession;
			CMD_LOG("%s:find in buff,just return.",__FUNCTION__);
			CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getZhuangbeiAlbumRsp_F,Flat_MSG_Rsp::createMsg(&rsp));
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastGetZhuangbeiAlbumprofession = profession;

	typedef SPCmd_GetZhuangbeiAlbum REQUESR_DATA;
	SimpleProtocolCmd cmd = getZhuangbeiAlbum;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->profession = htonl(profession);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetZhuangbeiAlbumRsp(CCObject* f_msg_rsp)
{
	Flat_GetZhuangbeiAlbumRsp* rsp = (Flat_GetZhuangbeiAlbumRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	CMD_LOG("%s --> save buff profession = %d",__FUNCTION__,lastGetZhuangbeiAlbumprofession);
	// 保存buff
	cmdBuffer->putZhuangBeiAlbum(lastGetZhuangbeiAlbumprofession,*rsp);
}


void CmdHelper::cmdGetCityGuardCurrentStat()
{
	CMD_LOG("%s",__FUNCTION__);
	{
		if (MyselfManager::getManager()->getMyZhuJueData() &&  //登录界面就发消息的话 可能自己主角的数据还没返回
			MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < Level_limit_cityguard )
		{
			POST_ERROR(CEC_HeroLevelNotEnough,MSG_getCityGuardCurrentStatRsp);
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_getCityGuardCurrentStat REQUESR_DATA;
	SimpleProtocolCmd cmd = getCityGuardCurrentStat;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetCityGuardCurrentStatRsp(CCObject* msg_rsp)
{
	SPCmd_getCityGuardCurrentStatRsp* rsp = (SPCmd_getCityGuardCurrentStatRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 初始化
	WorldBossCityGuardManager::getManager()->initState(rsp->stat,rsp->have_reward);
}
void CmdHelper::cmdCityGuardProcessNotifyRsp(CCObject* f_msg_rsp)
{
	Flat_CityGuardProcessNotify* rsp = (Flat_CityGuardProcessNotify*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 保存通知
	WorldBossCityGuardManager::getManager()->addCityGuardProcessNotify(*rsp);

}
void CmdHelper::cmdCityGuardBattle()
{
	CMD_LOG("%s",__FUNCTION__);

	{
		if (MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < Level_limit_cityguard )
		{
			POST_ERROR(CEC_HeroLevelNotEnough,MSG_cityGuardBattleRsp_F);
			return;
		}

		if (!WorldBossCityGuardManager::getManager()->isOpening())
		{
			POST_ERROR(CEC_WorldBossIsNotOpen,MSG_cityGuardBattleRsp_F);
			return;
		}
		//if (WorldBossCityGuardManager::getManager()->getTimeNeedWaitForCD() > 0)
		//{
		//	POST_ERROR(CEC_CDTimeIsNotOver,MSG_cityGuardBattleRsp_F);
		//	return;
		//}
	}
	

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_cityGuardBattle REQUESR_DATA;
	SimpleProtocolCmd cmd = cityGuardBattle;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->is_skil_cd = (WorldBossCityGuardManager::getManager()->getTimeNeedWaitForCD()>0?true:false);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdCityGuardBattleRsp(CCObject* f_msg_rsp)
{
	Flat_cityGuardBattleRsp* rsp = (Flat_cityGuardBattleRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 修正金币
	WuPinManager::getManager()->setWuPinNum(ItemsType_Gold,rsp->gold_balance);

	// boss之前没死，数据才是对的，不然都是乱的
	if (!rsp->battle_data.is_boss_dead)
	{
		// 更新伤害和排名
		WorldBossCityGuardManager::getManager()->setCurrentTotalShanghaiAndOrder(rsp->battle_data.total_shanghai,rsp->battle_data.shanghai_order);
		// 保存得到的东西
		vector<FightScript> scripts;
		scripts.push_back(rsp->battle_data.fightScript);
		storeAllGettedInFight(scripts,false);

		// 
		if (scripts[scripts.size()-1].endResult.myResult == Result_Win)
		{
			WorldBossCityGuardManager::getManager()->setBossDead(); // 立即刷新数据
		}
	}
	else
	{
		WorldBossCityGuardManager::getManager()->setBossDead(); // 立即刷新数据
	}
}
void CmdHelper::cmdGetCityGuardLastDayReport()
{
	CMD_LOG("%s",__FUNCTION__);
	if (MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < Level_limit_cityguard )
	{
		POST_ERROR(CEC_HeroLevelNotEnough,MSG_getCityGuardLastDayReportRsp_F);
		return;
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_getCityGuardLastDayReport REQUESR_DATA;
	SimpleProtocolCmd cmd = getCityGuardLastDayReport;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetCityGuardLastDayReportRsp(CCObject* f_msg_rsp)
{
	Flat_getCityGuardLastDayReportRsp* rsp = (Flat_getCityGuardLastDayReportRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 保存数据
	WorldBossCityGuardManager::getManager()->setCityGuardLastDayReport(*rsp);
}

void CmdHelper::cmdGetMyCityGuardReward()
{
	CMD_LOG("%s",__FUNCTION__);

	if (MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < Level_limit_cityguard )
	{
		POST_ERROR(CEC_HeroLevelNotEnough,MSG_getMyCityGuardRewardRsp);
		return;
	}
	if (!WorldBossCityGuardManager::getManager()->haveReward())
	{
		POST_ERROR(CEC_NoReward,MSG_getMyCityGuardRewardRsp);
		return;
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_GetMyCityGuardReward REQUESR_DATA;
	SimpleProtocolCmd cmd = getMyCityGuardReward;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetMyCityGuardRewardRsp(CCObject* msg_rsp)
{
	SPCmd_GetMyCityGuardRewardRsp* rsp = (SPCmd_GetMyCityGuardRewardRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	for (int i=0; i<rsp->count; i++)
	{
		// 保存数据
		WuPinManager::getManager()->addWuPin((ItemsType)rsp->item_arr[i].type,rsp->item_arr[i].count);
	}
	// 设置已经领了
	WorldBossCityGuardManager::getManager()->setHaveReward(false);
}
/*
void CmdHelper::cmdQueryDailyLoginBonus()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryDailyLoginBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = queryDailyLoginBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryDailyLoginBonusRsp(CCObject* f_msg_rsp)
{
	Flat_QueryDailyLoginBonusRsp* rsp = (Flat_QueryDailyLoginBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 保存数据
	BonusManager::getManager()->setBonus(BT_DailyLogin,rsp->bonus_arr);
}

void CmdHelper::cmdQueryLevelBonus()
{
	CMD_LOG("%s",__FUNCTION__);
	CCAssert(false,__FUNCTION__);
	return;


	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryLevelBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = queryLevelBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryLevelBonusRsp(CCObject* f_msg_rsp)
{
	Flat_QueryLevelBonusRsp* rsp = (Flat_QueryLevelBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 保存数据
	BonusManager::getManager()->setBonus(BT_Level,rsp->bonus_arr);
}

void CmdHelper::cmdQuerySystemBonus()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QuerySystemBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = querySystemBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQuerySystemBonusRsp(CCObject* f_msg_rsp)
{
	Flat_QuerySystemBonusRsp* rsp = (Flat_QuerySystemBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 保存数据
	BonusManager::getManager()->setBonus(BT_System,rsp->bonus_arr);
}

static unsigned int lastGetDailyId = 0;
void CmdHelper::cmdGetDailyLoginBonus(unsigned int _id)
{
	{
		Flat_BonusInfo temp;
		bool mustFound = BonusManager::getManager()->getBonus(BT_DailyLogin,_id,temp);
		CCAssert(mustFound,"必须找到");
		CCAssert(!temp.is_get,"必须没有领取过");

		for (int i=0,count=temp.items.size(); i<count; i++)
		{
			if (temp.items[i].type == ItemsType_PresentEquip_Xiyou ||
				temp.items[i].type == ItemsType_PresentEquip_Legend)
			{
				// 检测装备
				CHECK_PKG(1,MSG_getDailyLoginBonusRsp_F);
			}
		}
	}

	CMD_LOG("%s _id = %u",__FUNCTION__,_id);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();
	lastGetDailyId = _id;

	typedef SPCmd_GetDailyLoginBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = getDailyLoginBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->id = htonl(_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetDailyLoginBonusRsp(CCObject* f_msg_rsp)
{
	Flat_GetDailyLoginBonusRsp* rsp = (Flat_GetDailyLoginBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	Flat_BonusInfo data;
	bool mustFound = BonusManager::getManager()->getBonus(BT_DailyLogin,lastGetDailyId,data);
	if (!mustFound)
	{
		CCAssert(false,"不可能找不到");
		return;
	}
	// 保存数据
	BonusManager::getManager()->setIsGet(BT_DailyLogin,lastGetDailyId);
	for (int i=0,count=data.items.size(); i<count; i++)
	{
		if (data.items[i].type == ItemsType_PresentVIP1)
		{
			cmdQueryVipPrivilege();
			continue;
		}
		else if (data.items[i].type == ItemsType_PresentVitality)
		{
			CCAssert(false,"不应该有这个体力道具");
			continue;
		}
		else if (data.items[i].type == ItemsType_PresentEquip_Xiyou ||
			data.items[i].type == ItemsType_PresentEquip_Legend)
		{
			continue;
		}
		WuPinManager::getManager()->addWuPin((ItemsType)data.items[i].type,data.items[i].count);
	}

	for (int i=0,count=rsp->externGettedEquip.size(); i<count; i++)
	{
		EquipmentManager::getManager()->addOneEquipment(rsp->externGettedEquip[i]);
	}
}

static unsigned int lastGetLevelBonusId = 0;
void CmdHelper::cmdGetLevelBonus(unsigned int _id)
{
	{
		Flat_BonusInfo temp;
		bool mustFound = BonusManager::getManager()->getBonus(BT_Level,_id,temp);
		CCAssert(mustFound,"必须找到");
		CCAssert(!temp.is_get,"必须没有领取过");

		for (int i=0,count=temp.items.size(); i<count; i++)
		{
			if (temp.items[i].type == ItemsType_PresentEquip_Xiyou ||
				temp.items[i].type == ItemsType_PresentEquip_Legend)
			{
				// 检测装备
				CHECK_PKG(1,MSG_getLevelBonusRsp_F);
			}
		}
	}

	CMD_LOG("%s _id = %u",__FUNCTION__,_id);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();
	lastGetLevelBonusId = _id;

	typedef SPCmd_GetLevelBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = getLevelBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->id = htonl(_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetLevelBonusRsp(CCObject* f_msg_rsp)
{
	Flat_GetLevelBonusRsp* rsp = (Flat_GetLevelBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	Flat_BonusInfo data;
	bool mustFound = BonusManager::getManager()->getBonus(BT_Level,lastGetLevelBonusId,data);
	if (!mustFound)
	{
		CCAssert(false,"不可能找不到");
		return;
	}
	// 保存数据
	BonusManager::getManager()->setIsGet(BT_Level,lastGetLevelBonusId);
	for (int i=0,count=data.items.size(); i<count; i++)
	{
		if (data.items[i].type == ItemsType_PresentVIP1)
		{
			cmdQueryVipPrivilege();
			continue;
		}
		else if (data.items[i].type == ItemsType_PresentVitality)
		{
			CCAssert(false,"不应该有这个体力道具");
			continue;
		}
		else if (data.items[i].type == ItemsType_PresentEquip_Xiyou ||
			data.items[i].type == ItemsType_PresentEquip_Legend)
		{
			continue;
		}
		WuPinManager::getManager()->addWuPin((ItemsType)data.items[i].type,data.items[i].count);
	}

	for (int i=0,count=rsp->externGettedEquip.size(); i<count; i++)
	{
		EquipmentManager::getManager()->addOneEquipment(rsp->externGettedEquip[i]);
	}
}

static unsigned int lastGetSystemBonusId = 0;
void CmdHelper::cmdGetSystemBonus(unsigned int _id)
{
	{
		Flat_BonusInfo temp;
		bool mustFound = BonusManager::getManager()->getBonus(BT_System,_id,temp);
		CCAssert(mustFound,"必须找到");
		CCAssert(!temp.is_get,"必须没有领取过");


		for (int i=0,count=temp.items.size(); i<count; i++)
		{
			if (temp.items[i].type == ItemsType_PresentEquip_Xiyou ||
				temp.items[i].type == ItemsType_PresentEquip_Legend)
			{
				// 检测装备
				CHECK_PKG(1,MSG_getSystemBonusRsp_F);
			}
		}

	}

	CMD_LOG("%s _id = %u",__FUNCTION__,_id);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();
	lastGetSystemBonusId = _id;

	typedef SPCmd_GetSystemBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = getSystemBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->id = htonl(_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetSystemBonusRsp(CCObject* f_msg_rsp)
{
	Flat_GetSystemBonusRsp* rsp = (Flat_GetSystemBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	Flat_BonusInfo data;
	bool mustFound = BonusManager::getManager()->getBonus(BT_System,lastGetSystemBonusId,data);
	if (!mustFound)
	{
		CCAssert(false,"不可能找不到");
		return;
	}
	// 保存数据
	BonusManager::getManager()->setIsGet(BT_System,lastGetSystemBonusId);
	for (int i=0,count=data.items.size(); i<count; i++)
	{
		if (data.items[i].type == ItemsType_PresentVIP1)
		{
			cmdQueryVipPrivilege();
			continue;
		}
		else if (data.items[i].type == ItemsType_PresentVitality)
		{
			BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
			//actor->setVitality(Vitality_Total);
			actor->setVitality(actor->getCurrentProperty_R()->vit_current + Vitality_Total);
			continue;
		}
		else if (data.items[i].type == ItemsType_PresentEquip_Xiyou ||
			data.items[i].type == ItemsType_PresentEquip_Legend)
		{
			continue;
		}
		WuPinManager::getManager()->addWuPin((ItemsType)data.items[i].type,data.items[i].count);
	}

	for (int i=0,count=rsp->externGettedEquip.size(); i<count; i++)
	{
		EquipmentManager::getManager()->addOneEquipment(rsp->externGettedEquip[i]);
	}
}*/

/*static unsigned int lastGetChessRandomTaskId = 0;
void CmdHelper::cmdGetChessRandomBox(unsigned int task_id,unsigned int boxId)
{
	CMD_LOG("%s task_id = %d,boxId = %d",__FUNCTION__,task_id,boxId);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastGetChessRandomTaskId = task_id;

	typedef SPCmd_GetChessRandomBox REQUESR_DATA;
	SimpleProtocolCmd cmd = getChessRandomBox;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->task_id = htonl(task_id);
	_data->random_box_num = htonl(boxId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetChessRandomBoxRsp(CCObject* msg_rsp)
{
	SPCmd_GetChessRandomBoxRsp* rsp = (SPCmd_GetChessRandomBoxRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	//lastGetChessRandomTaskId
	TaskChessBoard board;
	if (!ChessBoardStateManager::getManager()->getChessBoard(lastGetChessRandomTaskId,board))
	{
		CCAssert(false,"");
	}else
	{
		// 修改宝箱
		board.box_id = 0xffffff;
		ChessBoardStateManager::getManager()->initOneChessboard(board);
	}

	// 保存得到的Item
	for (int i=0; i<rsp->count; i++)
	{
		WuPinManager::getManager()->addWuPin((ItemsType)rsp->item_arr[i].type,rsp->item_arr[i].count);
	}
}*/

void CmdHelper::cmdQueryTaskRate()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryRateTask REQUESR_DATA;
	SimpleProtocolCmd cmd = queryRateTask;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryTaskRateRsp(CCObject* f_msg_rsp)
{
	Flat_QueryRateTaskRsp* rsp = (Flat_QueryRateTaskRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	ChessBoardStateManager::getManager()->setCurrentTaskRate(*rsp);
}

static unsigned int lastQueryTaskRateBonusId = 0;
static RateStarType lastQueryTaskRateType;
void CmdHelper::cmdQueryRateTaskBonus(RateStarType type,unsigned int task_id)
{
	CMD_LOG("%s type = %d,id = %d",__FUNCTION__,type,task_id);

	{
		Flat_QueryRateTaskBonusRsp* buffered = NULL;
		if(type == RateStarType_Chapter)
		{
			buffered = ChessBoardStateManager::getManager()->getSeasonRateBonus(task_id);
		}
		else if (type == RateStarType_Task)
		{
			buffered = ChessBoardStateManager::getManager()->getTaskRateBonus(task_id);
		}
		else
		{
			CCLog("Error : %s --> type = %d",__FUNCTION__,type);
			CCAssert(false,"不支持");
		}
		
		if (buffered != NULL)
		{
			CMD_LOG("%s --> find in buff,just return.",__FUNCTION__);

			lastQueryTaskRateBonusId = task_id;
			lastQueryTaskRateType = type;
			// 发送消息
			CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryRateTaskBonus_F,Flat_MSG_Rsp::createMsg(buffered));
			return;
		}
	}


	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastQueryTaskRateBonusId = task_id;
	lastQueryTaskRateType = type;

	typedef SPCmd_QueryRateTaskBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = queryRateTaskBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->type = htons(type);
	_data->task_id = htons(task_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryRateTaskBonusRsp(CCObject* f_msg_rsp)
{
	Flat_QueryRateTaskBonusRsp* rsp = (Flat_QueryRateTaskBonusRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 缓存
	ChessBoardStateManager::getManager()->setTaskRateBonusList(lastQueryTaskRateBonusId,*rsp);
}

struct GetTaskRateBonusData
{
	RateStarType item_type;
	RateStarLevel level;
	unsigned int task_id;
};
static GetTaskRateBonusData lastGetTaskRateBonusData;
void CmdHelper::cmdGetTaskRateBonus(RateStarType type,RateStarLevel level,unsigned short task_id)
{
	CMD_LOG("%s type = %d,id = %d",__FUNCTION__,type,task_id);
	{
		Flat_QueryRateTaskBonusRsp* infor = NULL;
		if (type == RateStarType_Chapter)
		{
			infor = ChessBoardStateManager::getManager()->getSeasonRateBonus(task_id);
		}
		else if (type == RateStarType_Task)
		{
			infor = ChessBoardStateManager::getManager()->getTaskRateBonus(task_id);
		}
		else
		{
			CCLog("Error : %s ---> type = %d",__FUNCTION__,type);
			CCAssert(false,"不支持的RateStarType_Chapter");
		}
		CCAssert(infor!=NULL,"要有本地数据才能发送，不然成功了也不知道得到什么礼物");

		Flat_QueryRateTaskRsp* rate = ChessBoardStateManager::getManager()->getCurrentRate();
		CCAssert(rate,"必须存在，也就是之前已经调用过查询星级的接口了");

		if ( (type == RateStarType_Chapter && rate->chapterRate[task_id-1].star <= 0 )  || 
			 (type == RateStarType_Task && rate->taskRate[task_id-1].star <= 0 ) )
		{
			POST_ERROR(CEC_NoReward,MSG_getRateTaskBonusRsp);
			return;
		}
		if (type == RateStarType_Chapter){
			if ((level == RateStarLevel_Cuprum && rate->chapterRate[task_id-1].is_get_cuprum) ||
				(level == RateStarLevel_Silver && rate->chapterRate[task_id-1].is_get_silver) ||
				(level == RateStarLevel_Gold && rate->chapterRate[task_id-1].is_get_gold))
			{
				POST_ERROR(CEC_RewardIsAlreadyGetted,MSG_getRateTaskBonusRsp);
				return;
			}
		}
		else if (type == RateStarType_Task){
			if ((level == RateStarLevel_Cuprum && rate->taskRate[task_id-1].is_get_cuprum) ||
				(level == RateStarLevel_Silver && rate->taskRate[task_id-1].is_get_silver) ||
				(level == RateStarLevel_Gold && rate->taskRate[task_id-1].is_get_gold))
			{
				POST_ERROR(CEC_RewardIsAlreadyGetted,MSG_getRateTaskBonusRsp);
				return;
			}
		}
	}
	


	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastGetTaskRateBonusData.item_type = type;
	lastGetTaskRateBonusData.task_id = task_id;
	lastGetTaskRateBonusData.level = level;

	typedef SPCmd_GetRateTaskBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = getRateTaskBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->type = htons(type);
	_data->star = htons(level);
	_data->task_id = htons(task_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetTaskRateBonusRsp(CCObject* msg_rsp)
{
	SPCmd_GetRateTaskBonusRsp* rsp = (SPCmd_GetRateTaskBonusRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	Flat_QueryRateTaskBonusRsp* infor = NULL;
	if (lastGetTaskRateBonusData.item_type == RateStarType_Chapter)
	{
		infor = ChessBoardStateManager::getManager()->getSeasonRateBonus(lastGetTaskRateBonusData.task_id);
	}
	else if (lastGetTaskRateBonusData.item_type == RateStarType_Task)
	{
		infor = ChessBoardStateManager::getManager()->getTaskRateBonus(lastGetTaskRateBonusData.task_id);
	}
	else
	{
		CCLog("Error : %s ---> lastGetTaskRateBonusData.item_type = %d",__FUNCTION__,lastGetTaskRateBonusData.item_type);
		CCAssert(false,"不支持的RateStarType_Chapter");
	}

	CCAssert(infor!=NULL,"");
	Flat_QueryRateTaskRsp * rateData = ChessBoardStateManager::getManager()->getCurrentRate();
	CCAssert(rateData!=NULL,"");
	if (lastGetTaskRateBonusData.level == RateStarLevel_Cuprum) // 还没领，就是领取第一个
	{
		for (int itemIndex=0,count=infor->items1.size(); itemIndex<count; itemIndex++)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)infor->items1[itemIndex].type,infor->items1[itemIndex].count);
		}
		//
		if (lastGetTaskRateBonusData.item_type == RateStarType_Chapter)
		{
			rateData->chapterRate.at(lastGetTaskRateBonusData.task_id-1).is_get_cuprum = true;
		}
		else if (lastGetTaskRateBonusData.item_type == RateStarType_Task)
		{
			rateData->taskRate.at(lastGetTaskRateBonusData.task_id-1).is_get_cuprum = true;
		}
	}
	else if(lastGetTaskRateBonusData.level == RateStarLevel_Silver)
	{
		for (int itemIndex=0,count=infor->items2.size(); itemIndex<count; itemIndex++)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)infor->items2[itemIndex].type,infor->items2[itemIndex].count);
		}
		//
		if (lastGetTaskRateBonusData.item_type == RateStarType_Chapter)
		{
			rateData->chapterRate.at(lastGetTaskRateBonusData.task_id-1).is_get_silver = true;
		}
		else if (lastGetTaskRateBonusData.item_type == RateStarType_Task)
		{
			rateData->taskRate.at(lastGetTaskRateBonusData.task_id-1).is_get_silver = true;
		}
	}
	else if(lastGetTaskRateBonusData.level == RateStarLevel_Gold)
	{
		for (int itemIndex=0,count=infor->items3.size(); itemIndex<count; itemIndex++)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)infor->items3[itemIndex].type,infor->items3[itemIndex].count);
		}
		//
		if (lastGetTaskRateBonusData.item_type == RateStarType_Chapter)
		{
			rateData->chapterRate.at(lastGetTaskRateBonusData.task_id-1).is_get_gold = true;
		}
		else if (lastGetTaskRateBonusData.item_type == RateStarType_Task)
		{
			rateData->taskRate.at(lastGetTaskRateBonusData.task_id-1).is_get_gold = true;
		}
	}
	else
	{
		CCLog("Error : %s --> error star level = %d",__FUNCTION__,lastGetTaskRateBonusData.level);
		CCAssert(false,"错误的level");
	}
}


// 查询当前VIP特权数据
void CmdHelper::cmdQueryVipPrivilege()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryVipPrivilege REQUESR_DATA;
	SimpleProtocolCmd cmd = queryVipPrivilege;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub= htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdQueryVipPrivilegeRsp(CCObject* msg_rsp)
{
	SPCmd_QueryVipPrivilegeRsp* rsp = (SPCmd_QueryVipPrivilegeRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	{
		RuntimeData data;
		data.buyArenaChallengeTimeLeft = rsp->priv_info.buy_arena_num;
		data.buyTiLiTimeLeft = rsp->priv_info.buy_vitality_num;
		data.enableFastRongYuDuiHuan = rsp->priv_info.honor_exchange;
		data.CanJumpGridNum = rsp->priv_info.over_creep_num;
		data.cast_silver_num= rsp->priv_info.cast_silver_num;
		data.freeRefreshTimeLeft = rsp->priv_info.refresh_reward_num;
		data.saoDangTimeLeft = rsp->priv_info.sweep_num;
		data.cast_silver_num = rsp->priv_info.cast_silver_num;
		//
		if (MyselfManager::getManager()->isRunTimeOK())
			data.purify_free_times = MyselfManager::getManager()->getRuntionData()->purify_free_times;
		MyselfManager::getManager()->setRuntionData(data);
	}

	{
		RuntimeData data;
		data.buyArenaChallengeTimeLeft = rsp->deft_info.buy_arena_num;
		data.buyTiLiTimeLeft = rsp->deft_info.buy_vitality_num;
		data.enableFastRongYuDuiHuan = rsp->deft_info.honor_exchange;
		data.CanJumpGridNum = rsp->deft_info.over_creep_num;
		data.cast_silver_num= rsp->deft_info.cast_silver_num;
		data.freeRefreshTimeLeft = rsp->deft_info.refresh_reward_num;
		data.saoDangTimeLeft = rsp->deft_info.sweep_num;
		data.cast_silver_num = rsp->deft_info.cast_silver_num;
		//
		if (MyselfManager::getManager()->isRunTimeOK())
			data.purify_free_times = MyselfManager::getManager()->getRuntionData()->purify_free_times;
		MyselfManager::getManager()->setDefaultRuntimeData(data);
	}
}

struct UpgradeShenbingData
{
	unsigned long long zb_id;
	unsigned long long cailiao_zb_id;
};
static UpgradeShenbingData lastUpgradeShenbingData;
void CmdHelper::cmdUpgradeShenbing(unsigned long long zb_id,unsigned long long cailiao_zb_id)
{
	CMD_LOG("%s --> zb_id = %lld,cailiao_zb_id = %lld",__FUNCTION__,zb_id,cailiao_zb_id);
	// 不能用这个升级就返回。不能有人装，要是指定的装备
	{
		Equipment* toLevelUp = EquipmentManager::getManager()->getOneEquipment(zb_id);
		CCAssert(toLevelUp!=NULL,"");//&& toDismiis!=NULL
		Equipment* toDismiis = NULL;
		if (cailiao_zb_id != 0)
		{
			toDismiis = EquipmentManager::getManager()->getOneEquipment(cailiao_zb_id);
			CCLog("Error : %s --> cailiao_zb_id !=0 but not found in EquipmentManager");
			CCAssert(toDismiis!=NULL,"");
			return;
		}

		int erroCode = 0;
		if (!toLevelUp->canLevelUp(erroCode))
		{
			POST_ERROR(erroCode,MSG_upgradeShenbingRsp_F);
			return;
		}

		Flat_DuanzaoGroupZBInfo shenbing;
		if (!ChuanqiPool::getPool()->getChuanQiData(toLevelUp->getGroupId(),shenbing,toLevelUp->getLevel()))
		{
			CCLog("Error : %s --> the equip to level up must chuangqi,but not found in ChuanqiPool");
			CCAssert(false,"Bug");
			return;
		}

		if (shenbing.need_expend_group_id != 0 && cailiao_zb_id == NULL)
		{
			CCLog("Error : %s --> the equip to level up must need one zhuangbei,but cailiao_zb_id == 0");
			CCAssert(false,"升级需要装备，但是没有指定装备");
			return;
		}
		
		if (shenbing.need_expend_group_id != NULL && shenbing.need_expend_group_id != toDismiis->getGroupId())
		{
			POST_ERROR(CEC_CanNotUseThisEquipToLevelUpShenBing,MSG_upgradeShenbingRsp_F);
			return;
		}

		// 花费
		vector<CostData> cost_vec = toLevelUp->getShenBingLevelUpCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			if(WuPinManager::getManager()->getWuPin(cost_vec[i].item)->getCount() < cost_vec[i].count)
			{
				int code = CEC_ItemNotEnough_Begin+cost_vec[i].item;
				POST_ERROR(code,MSG_upgradeShenbingRsp_F);
				return;
			}
		}
	}

	lastUpgradeShenbingData.zb_id = zb_id;
	lastUpgradeShenbingData.cailiao_zb_id = cailiao_zb_id;

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_UpgradeShenbing REQUESR_DATA;
	SimpleProtocolCmd cmd = upgradeShenbing;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->zb_id= LF::lf_htonll(zb_id);
	_data->cailiao_zb_id= LF::lf_htonll(cailiao_zb_id);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdUpgradeShenbingRsp(CCObject* f_msg_rsp)
{
	Flat_UpgradeShenbingRsp* rsp = (Flat_UpgradeShenbingRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 移除材料装备
	EquipmentManager::getManager()->destoryOneEquip(lastUpgradeShenbingData.cailiao_zb_id);
	// 刷新新装备
	Equipment* euip = EquipmentManager::getManager()->getOneEquipment(lastUpgradeShenbingData.zb_id);
	CCAssert(euip!=NULL,"");
	if (euip != NULL)
	{

		// 扣物品
		vector<CostData> cost_vec = euip->getShenBingLevelUpCost();
		int count = cost_vec.size();
		for (int i=0; i<count; i++)
		{
			int count = cost_vec[i].count;
			WuPinManager::getManager()->addWuPin(cost_vec[i].item,-count);
		}

		// 刷新
		CCAssert(euip->getEquipId() == rsp->equipNewData.zhuangbei_id,"");
		CCAssert(euip->getActorId() == rsp->equipNewData.zhuangbei_hero_id,"");
		euip->refreshData(rsp->equipNewData);
	}

	// 回收材料
	for (int i=0; i<sizeof(rsp->item_arr)/sizeof(ItemInfo); i++)
	{
		if (rsp->item_arr[i].type == 0)
			continue;
		WuPinManager::getManager()->addWuPin((ItemsType)rsp->item_arr[i].type,(ItemsType)rsp->item_arr[i].count);
	}
}


void CmdHelper::cmdQueryChargePacket()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryChargePacket REQUESR_DATA;
	SimpleProtocolCmd cmd = queryChargePacket;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub= htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdQueryChargePacketRsp(CCObject* f_msg_rsp)
{
	Flat_QueryChargePacketRsp* rsp = (Flat_QueryChargePacketRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	MyselfManager::getManager()->setNextVipLevel(rsp->next_vip_level);
	MyselfManager::getManager()->setNeedChargeToGetNextVip(rsp->next_need_charge);
	MyselfManager::getManager()->setCharged(rsp->b_first_charged);
	MyselfManager::getManager()->setFirstChargePresent(rsp->first_present_items);
	MyselfManager::getManager()->setChargePacketInfo(rsp->chargePakets);
}


static unsigned int lastUnlockAssistantId = 0;
void CmdHelper::cmdUnlockAssistant(unsigned int hero_id)
{
	CMD_LOG("%s, hero_id = %u",__FUNCTION__,hero_id);
	{
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(hero_id);
		if (actor == NULL)
		{
			CCLog("Error : %s --> BaseActor is NULL,actor is not found");
			POST_ERROR(CEC_NullPointerException,MSG_unlockAssistantRsp);
			return;
		}

		if(!actor->getCurrentProperty_R()->isLocked)
		{
			POST_ERROR(CEC_ActorIsAlreadyUnlocked,MSG_unlockAssistantRsp);
			return;
		}

		if(!actor->getCurrentProperty_R()->canUnlock)
		{
			POST_ERROR(CEC_ActorIsDisableToUnlock,MSG_unlockAssistantRsp);
			return;
		}

		int lockedSize = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All,LF_Locked).size();
		unsigned int honorNeed = CS::AssistantUnLockNeedHonor(lockedSize);

		ArenaTeamBasicInfo* infor = MyselfManager::getManager()->getEdiableArenaTeamBasicInfo();
		if (infor == NULL)
		{
			CCLog("Error : %s --> ArenaTeamBasicInfo is NULL,ArenaTeamBasicInfo is not inited");
			POST_ERROR(CEC_NullPointerException,MSG_unlockAssistantRsp);
			return;
		}
		if (infor->jifen < honorNeed)
		{
			POST_ERROR(CEC_RongYuDianJiFenBuGou,MSG_unlockAssistantRsp);
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastUnlockAssistantId = hero_id;

	typedef SPCmd_UnlockAssistant REQUESR_DATA;
	SimpleProtocolCmd cmd = unlockAssistant;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->assistant_id= htonl(hero_id);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdUnlockAssistantRsp(CCObject* msg_rsp)
{
	SPCmd_UnlockAssistantRsp* rsp = (SPCmd_UnlockAssistantRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 扣除荣誉点
	ArenaTeamBasicInfo* infor = MyselfManager::getManager()->getEdiableArenaTeamBasicInfo();
	infor->jifen = rsp->honor;

	BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(lastUnlockAssistantId);
	if (actor != NULL)
	{
		actor->setLocked(false);
		// 等级改为5
		BaseActorProperty pro = actor->getCurrentProperty();
		pro.level = 5;
		actor->refreshBaseProPerty(pro);
	}
}

void CmdHelper::cmdQueryGiftActivity()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryGiftActivity REQUESR_DATA;
	SimpleProtocolCmd cmd = queryGiftActivity;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryGiftActivityRsp(CCObject* f_msg_rsp)
{
	Flat_QueryGiftActivityRsp* rsp = (Flat_QueryGiftActivityRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 保存。。。。
	GiftActivtyManager::getManager()->initActivityInfos(rsp->activities);
}

static unsigned int lastGetGiftActivityType = 0;
void CmdHelper::cmdGetGiftActivity(unsigned int activtyType)
{
	CMD_LOG("%s,activtyType = %u",__FUNCTION__,activtyType);
	//
	{
		const ActivityInfo* infor = GiftActivtyManager::getManager()->getActivity(activtyType);
		if (infor == NULL)
		{
			POST_ERROR(CEC_NullPointerException,MSG_getGiftActivityRsp);
			return;
		}

		GiftActivtyManager::ActivtyFilter filter = GiftActivtyManager::getManager()->getActivityState(infor);
		if (filter == GiftActivtyManager::AF_CanGetLater)
		{
			POST_ERROR(CEC_GettedTiLiTimeIsNotOpen,MSG_getGiftActivityRsp);
			return;
		}
		else if (filter == GiftActivtyManager::AF_Getted)
		{
			POST_ERROR(CEC_TiLiIsGetted,MSG_getGiftActivityRsp);
			return;
		}
		else if(filter == GiftActivtyManager::AF_TimeOver)
		{
			POST_ERROR(CEC_GettedTiLiTimeIsOver,MSG_getGiftActivityRsp);
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastGetGiftActivityType = activtyType;
	typedef SPCmd_GetGiftActivity REQUESR_DATA;
	SimpleProtocolCmd cmd = getGiftActivity;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->id = htonl(activtyType);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetGiftActivityRsp(CCObject* msg_rsp)
{
	SPCmd_GetGiftActivityRsp* rsp = (SPCmd_GetGiftActivityRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	ActivityInfo* infor = GiftActivtyManager::getManager()->getActivityByType(lastGetGiftActivityType);
	CCAssert(infor != NULL,"");
	if (infor != NULL)
	{
		infor->is_get = true;
	}

	// 刷体力
	BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	actor->setVitality(rsp->vit_current); 
}

void CmdHelper::cmdQueryKilledMonster()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryKilledMonster REQUESR_DATA;
	SimpleProtocolCmd cmd = queryKilledMonster;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryKilledMonsterRsp(CCObject* msg_rsp)
{
	SPCmd_QueryKilledMonsterRsp* rsp = (SPCmd_QueryKilledMonsterRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	ChessBoardStateManager::getManager()->setKilledMonsterNum(rsp->common_monster_count,rsp->elite_monster_count);
}

void CmdHelper::cmdQueryKilledMonsterBonus()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryKilledMonsterBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = queryKilledMonsterBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryKilledMonsterBonusRsp(CCObject* f_msg_rsp)
{
	Flat_QueryKilledMonsterBonusRsp* rsp = (Flat_QueryKilledMonsterBonusRsp*)((MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 保存
	ChessBoardStateManager::getManager()->setKilledMonsterBonus(*rsp);
}

static int lastGetKillBonusAccumulateType = 0;
void CmdHelper::cmdGetKilledMonsterBonus(AccumulateType type)
{
	CMD_LOG("%s AccumulateType = %u",__FUNCTION__,type);
	lastGetKillBonusAccumulateType = type;
	{
		if(type == Accumulate_Common_Monster)
		{
			if (ChessBoardStateManager::getManager()->getKilledPutongMonsterNum() < CS::cashBonusNeedKilledMonsters(type) )
			{
				POST_ERROR(CEC_KilledPutongNotEnough,MSG_getKilledMonsterBonusRsp);
				return;
			}
		}
		else if(type == Accumulate_Elite_Monster)
		{
			if (ChessBoardStateManager::getManager()->getKilledJingYinMonsterNum() < CS::cashBonusNeedKilledMonsters(type) )
			{
				POST_ERROR(CEC_KilledJingYinNotEnough,MSG_getKilledMonsterBonusRsp);
				return;
			}
		}
		else
		{
			CMD_LOG("%s ---> type = %d",__FUNCTION__,type);
			CCAssert(false,"");
			return;
		}
	}


	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_GetKilledMonsterBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = getKilledMonsterBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->type= htonl(type);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetKilledMonsterBonusRsp(CCObject* msg_rsp)
{
	SPCmd_GetKilledMonsterBonusRsp* rsp = (SPCmd_GetKilledMonsterBonusRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	Flat_QueryKilledMonsterBonusRsp bonus = ChessBoardStateManager::getManager()->getKilledMonsterBonusData();
	if (lastGetKillBonusAccumulateType == Accumulate_Common_Monster)
	{
		for (int i=0,count=bonus.commonBonus.size(); i<count; i++)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)bonus.commonBonus[i].type,bonus.commonBonus[i].count);
		}

		// 扣次数
		ChessBoardStateManager::getManager()->subKilledPutongMonster(CS::cashBonusNeedKilledMonsters(lastGetKillBonusAccumulateType));
	}
	else if(lastGetKillBonusAccumulateType == Accumulate_Elite_Monster)
	{
		for (int i=0,count=bonus.eliteBonus.size(); i<count; i++)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)bonus.eliteBonus[i].type,bonus.eliteBonus[i].count);
		}

		// 扣次数
		ChessBoardStateManager::getManager()->subKilledJingyinMonster(CS::cashBonusNeedKilledMonsters(lastGetKillBonusAccumulateType));
	}
	else
	{
		CCAssert(false,"");
	}
}
static int lastTopArenaQueryGuessGroupIndex = 0;
void CmdHelper::cmdTopArenaQueryGuess(int groupIndex)
{
	DianFengDuiJueManager::Round round = DianFengDuiJueManager::Round_Not_inited;
	DianFengDuiJueManager* manager = DianFengDuiJueManager::getManager();
	round = manager->getCurrentRound();

	CMD_LOG("%s->groupIndex = %d,round = %d,roundInPorotol = %d",
		__FUNCTION__,
		groupIndex,round,round+1
		);

	if (!manager->is_group_index_valide_in_round(groupIndex,round))
	{
		CCLog("Error : groupIndex[=%d] is error,in curRound[%=d]",groupIndex,round);
		CCAssert(false,"");
		return;
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastTopArenaQueryGuessGroupIndex = groupIndex;

	typedef SPCmd_TopArenaQueryGuess REQUESR_DATA;
	SimpleProtocolCmd cmd = topArenaQueryGuess;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->group = htons(groupIndex);
	_data->round = htons(round+1);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdTopArenaQueryGuessRsp(CCObject* msg_rsp)
{
	SPCmd_TopArenaQueryGuessRsp* rsp = (SPCmd_TopArenaQueryGuessRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 保存数据
	DianFengDuiJueManager::getManager()->setHeroValueInCurentRound(rsp->hero_value_a,rsp->hero_value_b,lastTopArenaQueryGuessGroupIndex);
}

void CmdHelper::cmdTopArenaQueryStatus()
{
	CMD_LOG("%s",__FUNCTION__);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_TopArenaQueryStatus REQUESR_DATA;
	SimpleProtocolCmd cmd = topArenaQueryStatus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdTopArenaQueryStatusRsp(CCObject* msg_rsp)
{
	SPCmd_TopArenaQueryStatusRsp* rsp = (SPCmd_TopArenaQueryStatusRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 保存
	DianFengDuiJueManager::getManager()->setCurrentStatus(*rsp);
}

struct TempTopArenaGuess
{
	unsigned short group;
	bool guess_winer;
	unsigned int wager;
};
static TempTopArenaGuess lastTopArenaGuess;
void CmdHelper::cmdTopArenaGuess(unsigned short group,bool guess_winer,unsigned int wager)
{
	DianFengDuiJueManager* manager = DianFengDuiJueManager::getManager();
	CMD_LOG("%s -->currentRound = %d,protocolRound = %d, group = %d,guess_winer = %s,wager = %u",
		__FUNCTION__,manager->getCurrentRound(),manager->getCurrentRound()+1,
		group,guess_winer?"A":"B",wager);

	lastTopArenaGuess.group = group;
	lastTopArenaGuess.guess_winer = guess_winer;
	lastTopArenaGuess.wager = wager;

	// 
	{
		if (manager->getCurrentRound() != DianFengDuiJueManager::Round_2_1 &&
			manager->getCurrentRound() != DianFengDuiJueManager::Round_4_2 &&
			manager->getCurrentRound() != DianFengDuiJueManager::Round_8_4)
		{
			POST_ERROR(CEC_CanNotJingCai_NotInRound,MSG_topArenaGuessRsp);
			return;
		}

		unsigned int heroIdA = DianFengDuiJueManager::getManager()->getOneGroupByCurrentRound(group).first.hero_id;
		unsigned int heroIdB = DianFengDuiJueManager::getManager()->getOneGroupByCurrentRound(group).second.hero_id;
		if (guess_winer && DianFengDuiJueManager::getManager()->getHeroValueInCurrentRoundByGroupIndex(heroIdA) > MAX_HERO_VALUE)
		{
			POST_ERROR(CEC_DFDJ_HERO_IS_MAX_VALUE,MSG_topArenaGuessRsp);
			return;
		}
		else if (DianFengDuiJueManager::getManager()->getHeroValueInCurrentRoundByGroupIndex(heroIdB) > MAX_HERO_VALUE)
		{
			POST_ERROR(CEC_DFDJ_HERO_IS_MAX_VALUE,MSG_topArenaGuessRsp);
			return;
		}


		if (wager != CS::getTopArenaWager(1) && wager != CS::getTopArenaWager(2))
		{
			CCAssert(false,"wager Error");
			return;
		}

		if ( !manager->is_group_index_valide_in_round(group,manager->getCurrentRound()))
		{
			CCAssert(false,"groupIndex Error");
			return;
		}

		if ( manager->am_i_guessed())
		{
			POST_ERROR(CEC_CanNotJingCai_AlreadyGuessed,MSG_topArenaGuessRsp);
			return;
		}

		if (wager > WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount())
		{
			POST_ERROR(CEC_SilverNotEnough,MSG_topArenaGuessRsp);
			return;
		}
	}



	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_TopArenaGuess REQUESR_DATA;
	SimpleProtocolCmd cmd = topArenaGuess;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];


	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->round = htons(manager->getCurrentRound()+1); // 跟协议定的之间差一个1
	_data->group = htons(group);
	_data->guess_winer = guess_winer;
	_data->wager = htonl(wager);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;

}
void CmdHelper::cmdTopArenaGuessRsp(CCObject* msg_rsp)
{
	SPCmd_TopArenaGuessRsp* rsp = (SPCmd_TopArenaGuessRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 银币
	WuPinManager::getManager()->setWuPinNum(ItemsType_Silver,rsp->silver_balance);
	// 竞猜
	DianFengDuiJueManager::getManager()->updateGuessData(lastTopArenaGuess.group,lastTopArenaGuess.guess_winer,lastTopArenaGuess.wager);
} 
void CmdHelper::cmdTopArenaReplay(unsigned short round,unsigned short group,unsigned short times)
{
	CMD_LOG("%s : round = %d,protocolRound = %d,group = %d,times = %d",__FUNCTION__,
		round,round+1,group,times);
	{
		DianFengDuiJueManager* manager = DianFengDuiJueManager::getManager();
		int curRound = manager->getCurrentRound();

		// 还没初始化
		if (curRound == DianFengDuiJueManager::Round_Not_inited)
		{
			POST_ERROR(CEC_DFDJ_NotInited,MSG_topArenaReplay_F);
			return;
		}
		// 还没比赛
		if (curRound <= round)
		{
			POST_ERROR(CEC_CanNotJingCai_HaveNotMatch,MSG_topArenaReplay_F);
			return;
		}

		CCAssert(times <= 3,"只能是3局中的一局");
		if (!manager->is_group_index_valide_in_round(group,(DianFengDuiJueManager::Round)round))
		{
			CCAssert(false,"round or group Error");
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_TopArenaReplay REQUESR_DATA;
	SimpleProtocolCmd cmd = topArenaReplay;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->round = htons(round+1);
	_data->group = htons(group);
	_data->times = htons(times);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdTopArenaReplayRsp(CCObject* f_msg_rsp)
{
	Flat_topArenaReplayRsp* rsp = (Flat_topArenaReplayRsp*)((MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
}


void CmdHelper::cmdCastSilver()
{
	CMD_LOG("%s",__FUNCTION__);
	{
		RuntimeData* defautData = MyselfManager::getManager()->getDefaultRuntimeData();
		RuntimeData* curData = MyselfManager::getManager()->getRuntionData();
		if (defautData == NULL || curData == NULL)
		{
			POST_ERROR(CEC_NullPointerException,MSG_getKilledMonsterBonusRsp);
			return;
		}
		// 没有次数了
		if (curData->cast_silver_num <= 0)
		{
			POST_ERROR(CEC_NoCastSilverTime,MSG_getKilledMonsterBonusRsp);
			return;
		}

		if (defautData->cast_silver_num < curData->cast_silver_num)
		{
			CCLog("Error : %s --> defautData->cast_silver_num < curData->cast_silver_num");
			CCAssert(false,"bug");
			return;
		}

		int needGold = CS::castSilverNeedGold( defautData->cast_silver_num - curData->cast_silver_num + 1);
		if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() < needGold)
		{
			POST_ERROR(CEC_GoldNotEnough,MSG_getKilledMonsterBonusRsp);
			return;
		}
	}


	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_CastSilver REQUESR_DATA;
	SimpleProtocolCmd cmd = castSilver;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdCastSilverRsp(CCObject* msg_rsp)
{
	SPCmd_CastSilverRsp* rsp = (SPCmd_CastSilverRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 减次数
	RuntimeData* data = MyselfManager::getManager()->getRuntionData();
	data->cast_silver_num--;
	// 更新
	WuPinManager::getManager()->setWuPinNum(ItemsType_Silver,rsp->silver_balance);
	WuPinManager::getManager()->setWuPinNum(ItemsType_Gold,rsp->gold_balance);
}

static bool isBufferedQueryLegendNoteRspOk = false;
static Flat_QueryLegendNoteRsp bufferedQueryLegendNoteRsp;
void CmdHelper::cmdQueryLegendNote()
{
	CMD_LOG("%s",__FUNCTION__);

	//if (MyselfManager::getManager()->getMyZhuJueData()->getLevel() < LEVEL_LIMIT_LEGEND_NOTE)
	//{
	//	CCLog("%s --> level is not enough",__FUNCTION__);
	//	POST_ERROR(CEC_HeroLevelIsNotEoungh,MSG_queryLegendNoteRsp_F);
	//	return;
	//}

	if (isBufferedQueryLegendNoteRspOk)
	{
		CMD_LOG("find in buff,return!");
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryLegendNoteRsp_F,Flat_MSG_Rsp::createMsg(&bufferedQueryLegendNoteRsp));
		return;
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryLegendNote REQUESR_DATA;
	SimpleProtocolCmd cmd = queryLegendNote;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdQueryLegendNoteRsp(CCObject* msg_rsp)
{
	Flat_QueryLegendNoteRsp* rsp = (Flat_QueryLegendNoteRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	bufferedQueryLegendNoteRsp = *rsp;
	isBufferedQueryLegendNoteRspOk = true;
}
void CmdHelper::cmdChallengeLegendNote(unsigned int groupId)
{
	CMD_LOG("%s,groupId = %u",__FUNCTION__,groupId);

	{
		if (MyselfManager::getManager()->getMyZhuJueData()->getLevel() < LEVEL_LIMIT_LEGEND_NOTE)
		{
			CCLog("%s --> level is not enough",__FUNCTION__);
			POST_ERROR(CEC_HeroLevelIsNotEoungh,MSG_queryLegendNoteRsp_F);
			return;
		}

		CCAssert(isBufferedQueryLegendNoteRspOk,"");
		if (bufferedQueryLegendNoteRsp.times <= 0)
		{
			POST_ERROR(CEC_ShenBinLuTimeOver,MSG_challengeLegendNoteRsp_f);
			return;
		}

		bool found = false;
		for(int pfs_index=0,count=bufferedQueryLegendNoteRsp.all_profession_zhuangbeis.size();pfs_index<count; pfs_index++)
		{
			for (int zb_index=0,zb_count=bufferedQueryLegendNoteRsp.all_profession_zhuangbeis[pfs_index].size(); zb_index<zb_count; zb_index++)
			{
				if (bufferedQueryLegendNoteRsp.all_profession_zhuangbeis[pfs_index][zb_index].zb_group_id == groupId)
				{
					found = true;
					int unlockedHeroNum = MyselfManager::getManager()->getUnlockedHeroNumByProfession(
						bufferedQueryLegendNoteRsp.all_profession_zhuangbeis[pfs_index][zb_index].profession_id);
					if (unlockedHeroNum <= 0)
					{
						POST_ERROR(CEC_ShenBinLu_CanNotChlg_HeroIsLocked,MSG_challengeLegendNoteRsp_f);
						return;
					}
					break;
				}
			}
		}
		CCAssert(found,"必须找到");
		if (!found)
		{
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_ChallengeLegendNote REQUESR_DATA;
	SimpleProtocolCmd cmd = challengeLegendNote;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->group_id = htonl(groupId);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdChallengeLegendNoteRsp(CCObject* f_msg_rsp)
{
	Flat_ChallengeLegendNoteRsp* rsp = (Flat_ChallengeLegendNoteRsp*)((MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 减时间次数
	bufferedQueryLegendNoteRsp.times --;

	// 保存获得的数据
	vector<FightScript> temp;
	temp.push_back(rsp->fightScript);
	storeAllGettedInFight(temp,false);
}


void CmdHelper::cmdQuerySelfChallengeInfo()
{
	CMD_LOG("%s",__FUNCTION__);

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QuerySelfChallengeInfo REQUESR_DATA;
	SimpleProtocolCmd cmd = querySelfChallengeInfo;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQuerySelfChallengeInfoRsp(CCObject* msg_rsp)
{
	SPCmd_QuerySelfChallengeInfoRsp* rsp = (SPCmd_QuerySelfChallengeInfoRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	// 保存数据
	XueZhanManager::getManager()->refreshData(*rsp);
}

void CmdHelper::cmdQuerySelfChallengeOrder(int difficut)
{
	int diff = XueZhanManager::getManager()->convertToSelfChlgDiffType((Difficult)difficut);
	CMD_LOG("%s,difficut = %d,SelfChlgDiffType = %d",__FUNCTION__,difficut,diff);

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QuerySelfChallengeOrder REQUESR_DATA;
	SimpleProtocolCmd cmd = querySelfChallengeOrder;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->difficulty = htonl(diff);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQuerySelfChallengeOrderRsp(CCObject* f_msg_rsp)
{
	Flat_QuerySelfChallengeOrderRsp* rsp = (Flat_QuerySelfChallengeOrderRsp*)((MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
}

void CmdHelper::cmdSelfChallenge()
{
	int diff = XueZhanManager::getManager()->convertToSelfChlgDiffType(XueZhanManager::getManager()->getCurrentDifficult());
	CMD_LOG("%s, SelfChlgDiffType = %d",__FUNCTION__,diff);

	{
		XueZhanManager::ErrorCode err = XueZhanManager::getManager()->canIJoinXueZhan();
		switch (err)
		{
		case XueZhanManager::EC_OK:
			// 可以
			break;
		case XueZhanManager::EC_Error:
			{
				POST_ERROR(CEC_CanNotXueZhan,MSG_selfChallengeRsp_F);
			}
			return;
			break;
		case XueZhanManager::EC_LevelNotEnough:
			{
				POST_ERROR(CEC_HeroLevelIsNotEoungh,MSG_selfChallengeRsp_F);
			}
			return;
			break;
		case XueZhanManager::EC_TaskNotEnough:
			{
				POST_ERROR(CEC_CanNotXueZhan, MSG_selfChallengeRsp_F);
			}
			return;
			break;
		case XueZhanManager::EC_NotOpenThisTime:
			{
				POST_ERROR(CEC_XueZhanNotOpen,MSG_selfChallengeRsp_F);
			}
			return;
			break;
		case XueZhanManager::EC_NoChlgTime:
			{
				POST_ERROR(CEC_XueZhanChlgTimeOver,MSG_selfChallengeRsp_F);
			}
			return;
			break;
		default:
			{
				POST_ERROR(CEC_RuntimeException,MSG_selfChallengeRsp_F);
			}
			CCAssert(false,"未知错误");
			return;
			break;
		}
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_SelfChallenge REQUESR_DATA;
	SimpleProtocolCmd cmd = selfChallenge;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->difficulty = htonl(diff);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;

}
void CmdHelper::cmdSelfChallengeRsp(CCObject* f_msg_rsp)
{
	Flat_SelfChallengeRsp* rsp = (Flat_SelfChallengeRsp*)((MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<FightScript> scripts;
	scripts.push_back(rsp->fightScript);
	storeAllGettedInFight(scripts,false);

	XueZhanManager::getManager()->setPowerNum(rsp->energy);
	XueZhanManager::getManager()->setCurrentPassedNum(rsp->checkpoints);
	XueZhanManager::getManager()->setCanOpenBox(rsp->is_have_box);
	XueZhanManager::getManager()->setLastGettedSilverNum(rsp->silver);
	XueZhanManager::getManager()->setLastGettedGiftBag(rsp->giftBag);


	if (scripts[scripts.size()-1].endResult.myResult == Result_Lose)
	{
		XueZhanManager::getManager()->subCanChlgNum(1);
		XueZhanManager::getManager()->setCurrentState(State_Out);
		//战斗失败后，还是不能置false
//		XueZhanManager::getManager()->setCurrentDiffFixed(false);
	}
	else
	{
		XueZhanManager::getManager()->setCurrentState(State_In);
		XueZhanManager::getManager()->setCurrentDiffFixed(true);
	}

	// 添加银币
	WuPinManager::getManager()->addWuPin(ItemsType_Silver,rsp->silver);
	//添加礼包
	SystemGiftBagManager::getManager()->addOneGiftBag(rsp->giftBag);
}

void CmdHelper::cmdSelfChallengeGain(SelfChlgGainType _gType,SelfChlgGainDegree _degree)
{
	CMD_LOG("%s, SelfChlgDiffType = %d,SelfChlgGainDegree = %d",__FUNCTION__,_gType,_degree);

	{
		unsigned int needPower;
		unsigned int gain;
		CS::getSelfChlgGain(_gType,_degree,needPower,gain);
		if (XueZhanManager::getManager()->getCurrentPowerNum() < needPower)
		{
			POST_ERROR(CEC_PPT_NotEnough,MSG_selfChallengeGainRsp);
			return;
		}
	}
	

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_SelfChallengeGain REQUESR_DATA;
	SimpleProtocolCmd cmd = selfChallengeGain;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->type = htonl(_gType);
	_data->degree = htonl(_degree);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdSelfChallengeGainRsp(CCObject* msg_rsp)
{
	SPCmd_SelfChallengeGainRsp* rsp = (SPCmd_SelfChallengeGainRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	XueZhanManager::getManager()->setPowerNum(rsp->energy);
	XueZhanManager::getManager()->setCurrentHpAddPercent(rsp->hp_gain);
	XueZhanManager::getManager()->setCurrentGongJiAddPercent(rsp->attack_gain);
	XueZhanManager::getManager()->setCurrentHuJiaAddPercent(rsp->defense_gain);
}

void CmdHelper::cmdGetSelfChallengeBox()
{
	CMD_LOG("%s",__FUNCTION__);
	{
		if (!XueZhanManager::getManager()->canOpenBox())
		{
			POST_ERROR(CEC_XueZhanNoBox,MSG_getSelfChallengeBoxRsp);
			return;
		}
	}


	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_GetSelfChallengeBox REQUESR_DATA;
	SimpleProtocolCmd cmd = getSelfChallengeBox;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetSelfChallengeBoxRsp(CCObject* msg_rsp)
{
	SPCmd_GetSelfChallengeBoxRsp* rsp = (SPCmd_GetSelfChallengeBoxRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	// 领过了
	XueZhanManager::getManager()->setCanOpenBox(false);
	// 添加
	WuPinManager::getManager()->addWuPin((ItemsType)rsp->item.type,rsp->item.count);
}

static int lastQueryProtectSpiritSeasonId = 0;
void CmdHelper::cmdQueryProtectSpirit(unsigned int seasonId)
{
	CMD_LOG("%s,seasonId = %d",__FUNCTION__,seasonId);

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastQueryProtectSpiritSeasonId = seasonId;

	typedef SPCmd_QueryProtectSpirit REQUESR_DATA;
	SimpleProtocolCmd cmd = queryProtectSpirit;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->chapter = htonl(seasonId);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryProtectSpiritRsp(CCObject* msg_rsp)
{
	SPCmd_QueryProtectSpiritRsp* rsp = (SPCmd_QueryProtectSpiritRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	ShouHuZhiLingManager::getManager()->updateSHZHL(lastQueryProtectSpiritSeasonId,rsp->first_spirit);
	CurrrentDefendHero current;
	current.heroId = rsp->current_spirit.spirit_id;
	current.level = rsp->current_spirit.level;
	current.name = rsp->current_spirit.name;
	current.profession = rsp->current_spirit.profession_id;
	current.defendTime = rsp->current_spirit.defend_time;
	ShouHuZhiLingManager::getManager()->updateSHZHL(lastQueryProtectSpiritSeasonId,current);
	ShouHuZhiLingManager::getManager()->setWorshiped(lastQueryProtectSpiritSeasonId,rsp->is_worship);
	ShouHuZhiLingManager::getManager()->setAmILoserInSeason(lastQueryProtectSpiritSeasonId,rsp->is_loser);

	BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	if (rsp->current_spirit.spirit_id == actor->getId() && 
		actor->getCurrentProperty_R()->profession)
	{
		if (!rsp->current_spirit.is_get)
		{
			vector<ItemInfo> items;
			CS::getProtectSpiritBonus(lastQueryProtectSpiritSeasonId, 1, items);
			CCAssert(items.size() == 1,"");
			ShouHuZhiLingManager::getManager()->setRewards(items[0]);
		}
		else
		{
			ItemInfo null_infor;
			null_infor.type = 0;
			null_infor.count = 0;
			ShouHuZhiLingManager::getManager()->setRewards(null_infor);
		}
	}
}

static int lastChallengeProtectSpiritSeasonId = 0;
void CmdHelper::cmdChallengeProtectSpirit(unsigned int seasonId)
{
	CMD_LOG("%s,seasonId = %d",__FUNCTION__,seasonId);
	{
		ShouHuZhiLingManager::ErrorCode errorCode = ShouHuZhiLingManager::getManager()->canIChlgSHZL(seasonId);

		switch (errorCode)
		{
		case ShouHuZhiLingManager::EC_OK:
			break;
		case ShouHuZhiLingManager::EC_ERROR:
			{
				POST_ERROR(CEC_SHZL_CanNotTiaoZhan,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_LEVEL_NOT_MATCH:
			{
				POST_ERROR(CEC_SHZL_LevelNotMatch,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_DEFENDER_IS_SAFE:
			{
				POST_ERROR(CEC_SHZL_SAFE,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CANNOT_CHLG_AGAIN_AFTER_LOSE:
			{
				POST_ERROR(CEC_SHZL_CANNOT_CHLG_AGAIN_AFTER_LOSE,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_WORSHIP_SELF:
			{
				POST_ERROR(CEC_SHZL_CanNotWarshipSelf,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_WORSHIP_GW:
			{
				POST_ERROR(CEC_SHZL_CanNotWarshipGW,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_ALREADY_WORSHIPED:
			{
				POST_ERROR(CEC_SHZL_AreadyWarshiped,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_CHLG_SELF:
			{
				POST_ERROR(CEC_SHZL_CanNotTiaoZhanSelf,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_GET_REWARD_NOT_THIS_SEASON:
			{
				POST_ERROR(CEC_SHZL_CAN_NOT_GET_REWARD_NOT_THIS_SEASON,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_NO_REWARD:
			{
				POST_ERROR(CEC_SHZL_NO_REWARD,MSG_challengeProtectSpiritRsp_F);
				return;
			}
			break;
		default:
			{
				POST_ERROR(CEC_SHZL_CanNotTiaoZhan,MSG_challengeProtectSpiritRsp_F);
				CCAssert(false,"未知错误");
				return;
			}
			break;
		}

		if (seasonId < 1 || seasonId > TaskManager::getManager()->getSeasonNum())
		{
			POST_ERROR(CEC_RuntimeException,MSG_challengeProtectSpiritRsp_F);
			return;
		}

		if (ChessBoardStateManager::getManager()->getCurrentRate()->chapterRate.size() < seasonId || 
			ChessBoardStateManager::getManager()->getCurrentRate()->chapterRate[seasonId-1].star != RateStarLevel_Gold)
		{
			POST_ERROR(CEC_SHZL_MUST_PERFECT_PASS,MSG_challengeProtectSpiritRsp_F);
			return;
		}

	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastChallengeProtectSpiritSeasonId = seasonId;

	typedef SPCmd_ChallengeProtectSpirit REQUESR_DATA;
	SimpleProtocolCmd cmd = challengeProtectSpirit;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->chapter = htonl(seasonId);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdChallengeProtectSpiritRsp(CCObject* f_msg_rsp)
{
	Flat_ChallengeProtectSpiritRsp* rsp = (Flat_ChallengeProtectSpiritRsp*)((MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	vector<FightScript> vec;
	vec.push_back(rsp->fightScript);
	storeAllGettedInFight(vec,false);
}


static int lastGetProtectSpiritBonusSeasonId = 0;
void CmdHelper::cmdGetProtectSpiritBonus(unsigned int seasonId)
{
	CMD_LOG("%s,seasonId = %d",__FUNCTION__,seasonId);

	{
		ShouHuZhiLingManager::ErrorCode errorCode = ShouHuZhiLingManager::getManager()->canIGetReward(seasonId);

		switch (errorCode)
		{
		case ShouHuZhiLingManager::EC_OK:
			break;
		case ShouHuZhiLingManager::EC_ERROR:
		case ShouHuZhiLingManager::EC_NO_REWARD:
			{
				POST_ERROR(CEC_SHZL_NO_REWARD,MSG_getProtectSpiritBonusRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_LEVEL_NOT_MATCH:
			{
				POST_ERROR(CEC_SHZL_LevelNotMatch,MSG_getProtectSpiritBonusRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_DEFENDER_IS_SAFE:
			{
				POST_ERROR(CEC_SHZL_SAFE,MSG_getProtectSpiritBonusRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_WORSHIP_SELF:
			{
				POST_ERROR(CEC_SHZL_CanNotWarshipSelf,MSG_getProtectSpiritBonusRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_WORSHIP_GW:
			{
				POST_ERROR(CEC_SHZL_CanNotWarshipGW,MSG_getProtectSpiritBonusRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_ALREADY_WORSHIPED:
			{
				POST_ERROR(CEC_SHZL_AreadyWarshiped,MSG_getProtectSpiritBonusRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_CHLG_SELF:
			{
				POST_ERROR(CEC_SHZL_CanNotTiaoZhanSelf,MSG_getProtectSpiritBonusRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_GET_REWARD_NOT_THIS_SEASON:
			{
				POST_ERROR(CEC_SHZL_CAN_NOT_GET_REWARD_NOT_THIS_SEASON,MSG_getProtectSpiritBonusRsp);
				return;
			}
			break;
		default:
			{
				POST_ERROR(CEC_SHZL_CanNotGetReward,MSG_getProtectSpiritBonusRsp);
				CCAssert(false,"未知错误");
				return;
			}
			break;
		}
	}


	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastGetProtectSpiritBonusSeasonId = seasonId;

	typedef SPCmd_GetProtectSpiritBonus REQUESR_DATA;
	SimpleProtocolCmd cmd = getProtectSpiritBonus;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->chapter = htonl(seasonId);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdGetProtectSpiritBonusRsp(CCObject* msg_rsp)
{
	SPCmd_GetProtectSpiritBonusRsp* rsp = (SPCmd_GetProtectSpiritBonusRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

 	ItemInfo item = ShouHuZhiLingManager::getManager()->getCurrentReword();
// 	if (item.type > 0 && item.count > 0)
// 	{
// 		WuPinManager::getManager()->addWuPin((ItemsType)item.type,item.count);
// 	}
// 	else
	if (rsp->item_count > 0)
	{
		for (int i=0; i<rsp->item_count; ++i)
		{
			WuPinManager::getManager()->addWuPin((ItemsType)rsp->item_arr[i].type,rsp->item_arr[i].count);
		}
	}
	else 
	{
		CCLog("Error : %s --> ShouHuZhiLingManager::getManager()->getCurrentReword, item is null",__FUNCTION__);
		CCAssert(false,"");
	}

	// 清空
	item.type = 0;
	item.count = 0;
	ShouHuZhiLingManager::getManager()->setRewards(item);
}


static int lastWorshiProtectSpiritSeasonId = 0;
void CmdHelper::cmdWorshiProtectSpirit(unsigned int seasonId)
{
	CMD_LOG("%s,seasonId = %d",__FUNCTION__,seasonId);
	{
		ShouHuZhiLingManager::ErrorCode errorCode = ShouHuZhiLingManager::getManager()->canIWorshipSHZL(seasonId);
		switch (errorCode)
		{
		case ShouHuZhiLingManager::EC_OK:
			break;
		case ShouHuZhiLingManager::EC_ERROR:
			{
				POST_ERROR(CEC_SHZL_CanNotWarshp,MSG_worshiProtectSpiritRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_LEVEL_NOT_MATCH:
			{
				POST_ERROR(CEC_SHZL_LevelNotMatch,MSG_worshiProtectSpiritRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_DEFENDER_IS_SAFE:
			{
				POST_ERROR(CEC_SHZL_SAFE,MSG_worshiProtectSpiritRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_WORSHIP_SELF:
			{
				POST_ERROR(CEC_SHZL_CanNotWarshipSelf,MSG_worshiProtectSpiritRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_WORSHIP_GW:
			{
				POST_ERROR(CEC_SHZL_CanNotWarshipGW,MSG_worshiProtectSpiritRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_ALREADY_WORSHIPED:
			{
				POST_ERROR(CEC_SHZL_AreadyWarshiped,MSG_worshiProtectSpiritRsp);
				return;
			}
			break;
		case ShouHuZhiLingManager::EC_CAN_NOT_CHLG_SELF:
			{
				POST_ERROR(CEC_SHZL_CanNotTiaoZhanSelf,MSG_worshiProtectSpiritRsp);
				return;
			}
			break;
		default:
			{
				POST_ERROR(CEC_SHZL_CanNotWarshp,MSG_worshiProtectSpiritRsp);
				CCAssert(false,"未知错误");
				return;
			}
			break;
		}
	}


	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	lastWorshiProtectSpiritSeasonId = seasonId;

	typedef SPCmd_WorshiProtectSpirit REQUESR_DATA;
	SimpleProtocolCmd cmd = worshiProtectSpirit;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->chapter = htonl(seasonId);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdWorshiProtectSpiritRsp(CCObject* msg_rsp)
{
	SPCmd_WorshiProtectSpiritRsp* rsp = (SPCmd_WorshiProtectSpiritRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

// 	//体力
// 	BaseActor* zhujue = MyselfManager::getManager()->getMyZhuJueData();
// 	zhujue->setVitality(zhujue->getCurrentProperty().vit_current + rsp->adapteData.vitality);
// 	//礼包
// 	for (int i=0, count=rsp->adapteData.giftBagVec.size(); i<count; ++i){
// 		SystemGiftBagManager::getManager()->addOneGiftBag(rsp->adapteData.giftBagVec[i]);
// 	}
// 	//道具
// 	for (int i=0, count=rsp->adapteData.itemVec.size(); i<count; ++i){
// 		WuPinManager::getManager()->addWuPin((ItemsType)rsp->adapteData.itemVec[i].type, rsp->adapteData.itemVec[i].count);
// 	}
// 	//道具碎片
// 	for (int i=0, count=rsp->adapteData.itemDebrisVec.size(); i<count; ++i){
// 		SuiPianManager::getManager()->addOneItemSuiPian(rsp->adapteData.itemDebrisVec[i]);
// 	}
// 	//装备碎片
// 	for (int i=0, count=rsp->adapteData.shenbingDebrisVec.size(); i<count; ++i){
// 		SuiPianManager::getManager()->addOneShenBingSuiPian(rsp->adapteData.shenbingDebrisVec[i]);
// 	}
// 	//装备
// 	for (int i=0, count=rsp->adapteData.equipVec.size(); i<count; ++i){
// 		EquipmentManager::getManager()->addOneEquipment(rsp->adapteData.equipVec[i]);
// 	}

// 	int level = MyselfManager::getManager()->getMyZhuJueData()->getLevel();
// 	std::vector<ItemInfo> items;
// 	CS::worshiProtectSpirit(level, items);
// 
// 	for (int i=0,count=items.size(); i<count; i++)
// 	{
// 		if (items[i].type > 0)
// 		{
// 			WuPinManager::getManager()->addWuPin((ItemsType)items[i].type,items[i].count);
// 		}
// 	}
// 

	ShouHuZhiLingManager::getManager()->setWorshiped(lastWorshiProtectSpiritSeasonId,true);

	SystemGiftBagManager::getManager()->addOneGiftBag(rsp->gift_bag);
}


void CmdHelper::cmdQueryDiscoverInfo()
{
	CMD_LOG("%s",__FUNCTION__);

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_QueryDiscoverInfo REQUESR_DATA;
	SimpleProtocolCmd cmd = queryDiscoverInfo;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->stub = htonl(0);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::cmdQueryDiscoverInfoRsp(CCObject* msg_rsp)
{
	SPCmd_QueryDiscoverInfoRsp* rsp = (SPCmd_QueryDiscoverInfoRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}
	int vipLevel = MyselfManager::getManager()->getMyZhuJueData()->getVipLevel();
	TanSuoData data;
	data.destoriedNum = rsp->destroy_seal;
	data.currentTimeCanUse = rsp->remainder_times;
	data.defaultTimeCanUse = CS::getDiscoverLimitTimes(vipLevel);
	data.unlocked_seal = rsp->unlocked_seal;
	data.last_modify_timestamp = rsp->last_modify_timestamp;
	TanSuoManager::getManager()->setTanSuoData(data);
}

void CmdHelper::cmdDiscover(bool akey_all)
{
	CMD_LOG("%s,akey_all = %s",__FUNCTION__,akey_all?"true":"false");

	{
		if (TanSuoManager::getManager()->getCurrentTimeLeft() <= 0)
		{
			POST_ERROR(CEC_NO_TANSUO_TIME,MSG_discoverRsp);
			return;
		}
	}



	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_Discover REQUESR_DATA;
	SimpleProtocolCmd cmd = discover;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->akey_all = akey_all;
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdDiscoverRsp(CCObject* msg_rsp)
{
	SPCmd_DiscoverRsp* rsp = (SPCmd_DiscoverRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	if (rsp->is_destroyed)
	{
		TanSuoManager::getManager()->addDistoriedNum(1);
	}
	TanSuoManager::getManager()->setCanTanSuoTime(rsp->remainder_times);
	for (int i=0;i<rsp->count; i++)
	{
		WuPinManager::getManager()->addWuPin((ItemsType)rsp->item_arr[i].type,rsp->item_arr[i].count);
	}
	TanSuoManager::getManager()->setLastModifyTansuoTime(rsp->last_modify_timestamp);
}

void CmdHelper::cmdUnlockDiscoverSeal(unsigned int profession)
{
	CMD_LOG("%s,profession = %u",__FUNCTION__,profession);

	{
		// 检测背包
		CHECK_PKG(1,MSG_unlockDiscoverSealRsp_F);

		if (!TanSuoManager::getManager()->canJieFeng())
		{
			POST_ERROR(CEC_CAN_NOT_JIEFENG,MSG_unlockDiscoverSealRsp_F);
			return;
		}
	}

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	typedef SPCmd_UnlockDiscoverSeal REQUESR_DATA;
	SimpleProtocolCmd cmd = unlockDiscoverSeal;

	unsigned int dataLength = sizeof(REQUESR_DATA);
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	REQUESR_DATA* _data = (REQUESR_DATA*)bodyChar;
	_data->profession = htonl(profession);
	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdUnlockDiscoverSealRsp(CCObject* f_msg_rsp)
{
	Flat_UnlockDiscoverSealRsp* rsp = (Flat_UnlockDiscoverSealRsp*)((MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	for (int i=0,count=rsp->equips.size(); i<count; i++)
	{
		EquipmentManager::getManager()->addOneEquipment(rsp->equips[i]);
	}
	// 清空
	TanSuoManager::getManager()->addUnlockedSealNum(1);
	if (TanSuoManager::getManager()->getDistoriedNum() == 6)
		TanSuoManager::getManager()->setDistoriedNum(0);
}

void CmdHelper::cmdQueryPurify()
{
	CMD_LOG("%s",__FUNCTION__);

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryPurify);
	SimpleProtocolCmd cmd = queryPurify;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryPurify* _data = (SPCmd_QueryPurify*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryPurifyRsp(CCObject* msg_rsp)
{
	SPCmd_QueryPurifyRsp* rsp = (SPCmd_QueryPurifyRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	MyselfManager::getManager()->getRuntionData()->purify_free_times = rsp->purify_free_times;
}
void CmdHelper::cmdPurify(unsigned long long zbID, unsigned int holeId)
{
	//检测
	{
		//
		string temp;
		if (! EquipmentManager::getManager()->getOneEquipment(zbID)->canPurify(temp, holeId, false))
		{
			POST_ERROR(CEC_NoPurifyTimes,MSG_PurifyRsp);
			return;
		}
		//
		unsigned int freePurifyTimes = MyselfManager::getManager()->getRuntionData()->purify_free_times;
		unsigned int purityTools = WuPinManager::getManager()->getWuPin(ItemsType_Purify_Scroll)->getCount();
		if (freePurifyTimes + purityTools < 1)
		{
			POST_ERROR(CEC_NoPurifyTools,MSG_PurifyRsp);
			return;
		}
	}


	CMD_LOG("%s ---> equipId=[%llu], holeId=[%u]",__FUNCTION__, zbID, holeId);

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_Purify);
	SimpleProtocolCmd cmd = purify;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_Purify* _data = (SPCmd_Purify*)bodyChar;
	_data->id = LF::lf_htonll(zbID);
	_data->hole = htonl(holeId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
bool g_purifyZBAttrSave = false;
unsigned long long g_purifyZBAttrSave_zbID;
vector<ZBAttr> g_purifyRspZBAttr;
void CmdHelper::cmdPurifyRsp(CCObject* f_msg_rsp)
{
	Flat_PurifyRsp* rsp = (Flat_PurifyRsp*)((Flat_MSG_Rsp*)f_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	g_purifyRspZBAttr = rsp->zbAttrVec;

	unsigned int freePurifyTimes = MyselfManager::getManager()->getRuntionData()->purify_free_times;
	unsigned int purityTools = WuPinManager::getManager()->getWuPin(ItemsType_Purify_Scroll)->getCount();
	if (freePurifyTimes > 0)
		MyselfManager::getManager()->getRuntionData()->purify_free_times -= 1;
	else if (purityTools > 0)
		WuPinManager::getManager()->getWuPin(ItemsType_Purify_Scroll)->setCount(purityTools - 1);
	else 
		CCAssert(freePurifyTimes + purityTools > 0, "bug");
}
void CmdHelper::cmdConfirmPurify(unsigned long long zbID, bool save)
{
	g_purifyZBAttrSave_zbID = zbID;
	g_purifyZBAttrSave = save;
	CMD_LOG("%s ---> zbid = %llu, save = %s",__FUNCTION__, zbID, save?"true":"false");

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ConfirmPurify);
	SimpleProtocolCmd cmd = confirmPurify;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_ConfirmPurify* _data = (SPCmd_ConfirmPurify*)bodyChar;
	_data->id = LF::lf_htonll(zbID);
	_data->is_confirm = save;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdConfirmPurifyRsp(CCObject* msg_rsp)
{
	SPCmd_ConfirmPurifyRsp* rsp = (SPCmd_ConfirmPurifyRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	if (g_purifyZBAttrSave)
	{
		Equipment *eq = EquipmentManager::getManager()->getOneEquipment(g_purifyZBAttrSave_zbID);
		eq->setPurifyAttribute(g_purifyRspZBAttr);
	}
	g_purifyZBAttrSave = false;
}
unsigned int g_QueryAssessWeapon_EqGroupId = 0;
void CmdHelper::cmdQueryAssessWeapon(unsigned long long zbID)
{
	Equipment *eq = EquipmentManager::getManager()->getOneEquipment(zbID);
	{//检测
		//只有稀有装才能坚定
		if (eq->getEquipColor() != ZhuangbeiColour_Xiyou)
		{
			CCLog("%s ---> only ZhuangbeiColour_Xiyou can be assess weapon", __FUNCTION__);
			POST_ERROR(Error, MSG_QueryAssessWeaponRsp);
			return;
		}
	}

	g_QueryAssessWeapon_EqGroupId = eq->getGroupId();

	//如果已有直接返回
	const Flat_DuanzaoGroupZBInfo* cailiaoInfo = JieFengYinMapManager::getManager()->getNeedEqMaterialInfo(g_QueryAssessWeapon_EqGroupId);
	if (cailiaoInfo != 0)
	{
		CMD_LOG("%s --->(in the cache) query group id = %u, need groupid = %u",__FUNCTION__, g_QueryAssessWeapon_EqGroupId, cailiaoInfo->zb_group_id);
		Flat_QueryAssessWeaponRsp falt_data;
		falt_data.err_code = Success;
		falt_data.eqVec.clear();
		falt_data.eqVec.push_back(*cailiaoInfo);
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryAssessWeaponRsp, MSG_Rsp::createMsg(&falt_data));
		return;
	}

	CMD_LOG("%s ---> query group id = %u",__FUNCTION__, g_QueryAssessWeapon_EqGroupId);
	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryAssessWeapon);
	SimpleProtocolCmd cmd = queryAssessWeapon;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryAssessWeapon* _data = (SPCmd_QueryAssessWeapon*)bodyChar;
	_data->group_id = htonl(g_QueryAssessWeapon_EqGroupId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdQueryAssessWeaponRsp(CCObject* msg_rsp)
{
	Flat_QueryAssessWeaponRsp* rsp = (Flat_QueryAssessWeaponRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();

	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	//
	for (int i=0; i<rsp->eqVec.size(); ++i)
	{
		JieFengYinMapManager::getManager()->addOneJieFengMap(g_QueryAssessWeapon_EqGroupId, rsp->eqVec.at(i));
	}
	
}
unsigned long long g_assessWeaponId=0, g_assessWeaponUsedId=0;
void CmdHelper::cmdAssessWeapon(unsigned long long zbId, unsigned long long usedZbId)
{
	//检测
	{
		Equipment *eq = EquipmentManager::getManager()->getOneEquipment(zbId);
		if (! eq->isInFengYin())
		{
			CCLog("%s ---> is not in fengyin", __FUNCTION__);
			POST_ERROR(Error, MSG_AssessWeaponRsp);
			return;
		}
	}

	g_assessWeaponId = zbId;
	g_assessWeaponUsedId = usedZbId;
	CMD_LOG("%s ---> zbId = %llu, meterId=%llu",__FUNCTION__, zbId, zbId);

	GameDirector::getDirector()->showWaiting();
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_AssessWeapon);
	SimpleProtocolCmd cmd = assessWeapon;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_AssessWeapon* _data = (SPCmd_AssessWeapon*)bodyChar;
	_data->zb_id = LF::lf_htonll(zbId);
	_data->material_id = LF::lf_htonll(usedZbId);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::cmdAssessWeaponRsp(CCObject* flat_msg_rsp)
{
	Flat_AssessWeaponRsp *rsp = (Flat_AssessWeaponRsp*)((Flat_MSG_Rsp*)flat_msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		try_alert(rsp->err_code);
		return;
	}

	//消耗扣除
	EquipmentManager::getManager()->destoryOneEquip(g_assessWeaponUsedId);
	//材料返回回收
	for (int i=0; i<rsp->itemsVec.size(); ++i){
		WuPinManager::getManager()->addWuPin((ItemsType)rsp->itemsVec.at(i).type, rsp->itemsVec.at(i).count);
	}
	//装备更新
	EquipmentManager::getManager()->getOneEquipment(g_assessWeaponId)->jieFengYing();
}




//////////////////////////////////////////////////////////////////////////
// 字节序和log
//////////////////////////////////////////////////////////////////////////
void CmdHelper::hton_HeroInfo(HeroInfo& infor)
{
	hton_hero_info(infor,infor);
	//infor.hero_id = htonl(infor.hero_id);
	//infor.zhenshen_id = htonl(infor.zhenshen_id);
	//infor.is_yuanshen = infor.is_yuanshen; // 不用转
	//infor.vip_level = htonl(infor.vip_level);
	//infor.level = htonl(infor.level);
	//infor.stage = htonl(infor.stage);
	//infor.profession_id = htonl(infor.profession_id);
}
void CmdHelper::hton_HeroDetail(HeroDetail& detail)
{
	hton_hero_detail(detail,detail);
}
void CmdHelper::hton_HeroAttrWithoutZhuangbei(HeroAttrWithoutZhuangbei& data)
{
	data.baoji = htonl(data.baoji);
	data.baoshang = htonl(data.baoshang);
	//data.gongji_max = htonl(data.gongji_max);
	//data.gongji_min = htonl(data.gongji_min);
	data.jinengchufa = htonl(data.jinengchufa);
	data.liliang = htonl(data.liliang);
	data.mingjie = htonl(data.mingjie);
	data.tineng = htonl(data.tineng);
	data.xueliang = htonl(data.xueliang);
	data.zhili = htonl(data.zhili);
}
void CmdHelper::hton_GiftData(GiftData& data)
{
	data = data.htonl_gift();
	/*data.type = htonl(data.type);
	data.gift_id = htonl(data.gift_id);
	if (data.type == GiftType_Zhuangbei)
	{
	data.data_union.zhuangbei.zhuangbei_colour = ntohl(data.data_union.zhuangbei.zhuangbei_colour);
	data.data_union.zhuangbei.zhuangbei_id = LF::lf_htonll(data.data_union.zhuangbei.zhuangbei_id);
	data.data_union.zhuangbei.zhuangbei_type = ntohl(data.data_union.zhuangbei.zhuangbei_type);
	}
	else if (data.type == GiftType_Item)
	{
	data.data_union.item.type = ntohl(data.data_union.item.type);
	data.data_union.item.count = ntohl(data.data_union.item.count);
	}
	else
	{
	CCAssert(false,"未知的类型");
	}*/
}
// 返回length
unsigned int ntoh_DuanzaoGroupZBInfoArrayToFlat_DuanzaoGroupZBInfoVec(DuanzaoGroupZBInfo* arr,unsigned int count,vector<Flat_DuanzaoGroupZBInfo>& out_data)
{
	EquipmentConfigInfoManager *_equipInfoConfig = EquipmentConfigInfoManager::getManager();

	const char* start = (const char*)arr;

	DuanzaoGroupZBInfo* detailArray = arr;
	for (int i=0;i<count;i++)
	{
		Flat_DuanzaoGroupZBInfo infor;
		infor.zb_group_id = ntohl(detailArray->zb_group_id);
		infor.zhuangbei_type = (ZhuangbeiType)ntohl(detailArray->zhuangbei_type);
		//infor.zhuangbei_name = string(detailArray->zhuangbei_name);
		infor.zhuangbei_name = _equipInfoConfig->getEquipName(infor.zb_group_id);
		infor.zhuangbei_colour = (ZhuangbeiColour)ntohl(detailArray->zhuangbei_colour);
		//infor.zhuangbei_pic = string(detailArray->zhuangbei_pic);
		infor.zhuangbei_pic = _equipInfoConfig->getEquipPicFile(infor.zb_group_id);
		infor.level = ntohl(detailArray->level);
		infor.ratio = ntohl(detailArray->ratio);

		infor.next_group_id = ntohl(detailArray->next_group_id);
		infor.need_expend_group_id = ntohl(detailArray->need_expend_group_id);
		infor.profession_id = ntohl(detailArray->profession_id);

		infor.gongji_min_min = ntohl(detailArray->gongji_min_min);
		infor.gongji_min_max = ntohl(detailArray->gongji_min_max);
		infor.gongji_max_min = ntohl(detailArray->gongji_max_min);
		infor.gongji_max_max = ntohl(detailArray->gongji_max_max);
		infor.wufang_min = ntohl(detailArray->wufang_min);
		infor.wufang_max = ntohl(detailArray->wufang_max);
		infor.hole1_percent = ntohl(detailArray->hole1_percent);
		infor.hole2_percent = ntohl(detailArray->hole2_percent);
		infor.hole3_percent = ntohl(detailArray->hole3_percent);

		//  char attr_str[0]; //attr_str_count个连续字符串，每个字符串以'\0'结束
		int attri_length = 0;
		int attr_count = ntohl(detailArray->attr_str_count);
		char* attr_string = detailArray->attr_str;
		for (int attr_index = 0;attr_index < attr_count;attr_index++)
		{
			string oneAttr = string(attr_string);
			AttrbuteItem item(oneAttr);
			infor.itemVec.push_back(item);
			attr_string += oneAttr.length() + 1; // 越过"\0"指向下一个
			attri_length += oneAttr.length() + 1;
		}
		// 描述  //desc_len用到了这里的长度、所以这句不能屏蔽
		infor.zhuangbei_desc = string(attr_string);
		int desc_len = infor.zhuangbei_desc.length() + 1;
		infor.zhuangbei_desc = _equipInfoConfig->getEquipDescribe(infor.zb_group_id);

		++ detailArray;
		detailArray = (DuanzaoGroupZBInfo*) (((char*)detailArray) + attri_length + desc_len);

		CMD_LOG("\n--------------------DuanzaoGroupZBInfo[%d]---------------------------------",i);
		CMD_LOG("zhuangbei_name = %s,profession_id = %u,zhuangbei_type = %d,zhuangbei_groupId = %d, zhuangbei_colour = %d,zhuangbei_level = %d,zhuangbei_desc = %s",
			infor.zhuangbei_name.c_str(),
			infor.profession_id,
			infor.zhuangbei_type,
			infor.zb_group_id,
			infor.zhuangbei_colour,infor.level,
			infor.zhuangbei_desc.c_str());
		CMD_LOG("\n--------------------DuanzaoGroupZBInfo[%d]---------------------------------",i);

		out_data.push_back(infor);
	}

	const char* end = (const char*)detailArray;

	return (end - start);
}

void CmdHelper::log(const HeroInfo& data)
{
	CMD_LOG("HeroInfo : heroId = %u, is_yuanshen = %s,zhenshenId = %u, vip_level = %u, level = %u ,stage = %u , profession_id = %d, name = %s",
		data.hero_id,data.is_yuanshen?"true":"false",data.zhenshen_id,data.vip_level,data.level,data.stage,data.profession_id,data.name);
}
void CmdHelper::log(const HeroDetail& data)
{
#if DATAPOOL_LOG == 1
	log(data.basic_info);
	CMD_LOG("exp_current = %d,exp_need = %d,parent_hero_id = %d,postion = %d,locked = %s,canUnlock = %s",
		data.exp_current,data.exp_need,data.parent_hero_id,data.postion,
		(!data.is_unlocked)?"true":"false",
		data.unlockAble?"true":"false"
		);
	log(data.hero_attr_without_zhuangbei);
#endif
}
void CmdHelper::log(const HeroAttrWithoutZhuangbei& data)
{
	CMD_LOG("baoji = %d,baoshang = %d,jinengchufa = %d,liliang = %d,mingjie = %d,tineng = %d,xueliang = %d,zhili = %d",
		data.baoji,data.baoshang,data.jinengchufa,data.liliang,data.mingjie,data.tineng,data.xueliang,data.zhili);
}
void CmdHelper::log(const ZhuangbeiDetail& data)
{
	EquipData eData;
	eData.initWithZhungbeiDetail(&data);
	log(eData);
}
void CmdHelper::log(const EquipData& data)
{
	CMD_LOG(
		"id = %lld, group_id = %d, hero_id = %d, justForHeroProfession = %u, baoji = %d,baoshang = %d,diamond_type_1 = %d,diamond_type_2 = %d,diamond_type_3 = %d,gedanglv = %d,gedangzhi = %d,\n		\
		gongji_max = %d,gongji_max_add = %d,gongji_min = %d,gongji_min_add = %d,\n		\
		hole = %d,hp_ratio = %d,hujia = %d,kangxing = %d,liliang = %d,mingjie = %d,\n		\
		qianghua_time = %d,tineng = %d,zhili = %d,zhuangbei_colour = %d,\n				\
		zhuangbei_desc = %s,zhuangbei_hero_id = %d,zhuangbei_level = %d,zhuangbei_name = %s,\n \
		zhuangbei_pic = %s,zhuangbei_type = %d",
		data.zhuangbei_id,data.group_id,data.zhuangbei_hero_id,data.profession_id,data.getAtrriValueByType(AttrType_baoji),data.getAtrriValueByType(AttrType_baoshang),
		data.diamond_type_1,data.diamond_type_2,data.diamond_type_3,data.getAtrriValueByType(AttrType_gedanglv),data.getAtrriValueByType(AttrType_gedangzhi),
		data.gongji_max,data.getAtrriValueByType(AttrType_gongji_max),data.gongji_min,data.getAtrriValueByType(AttrType_gongji_min),
		data.hole,data.getAtrriValueByType(AttrType_hp_percent),data.hujia,data.getAtrriValueByType(AttrType_kangxing),data.getAtrriValueByType(AttrType_liliang),data.getAtrriValueByType(AttrType_mingjie),
		data.qianghua_data.qianghua_count,data.getAtrriValueByType(AttrType_tineng),data.getAtrriValueByType(AttrType_zhili),data.zhuangbei_colour,
		data.equipDicrib.c_str(),data.zhuangbei_hero_id,data.zhuangbei_level,data.equipName.c_str(),
		data.equipPic.c_str(),data.zhuangbei_type);
	CMD_LOG("isShenBing = %s,isInFengYing = %s",data.isShenBing?"true":"false",data.isInFengYin?"true":"false");

	if (data.jinglian_times > 0)
	{
		CMD_LOG("jinglianTime = %d",data.jinglian_times);
		for (int i=0;i<data.jinglian_attr_vec.size();i++)
		{
			CMD_LOG("jinglian_attr_vec[%d] ->type = %d,value = %d",i,data.jinglian_attr_vec[i].attr_type,data.jinglian_attr_vec[i].attr_value);
		}
	}
	
	for (int i=0;i<data.zhuanke_attr_vec.size();i++)
	{
		CMD_LOG("zhuanke_attr_vec[%d] ->type = %d,value = %d",i,data.zhuanke_attr_vec[i].attr_type,data.zhuanke_attr_vec[i].attr_value);
	}

	CMD_LOG("unknownHideAttriNum = %d",data.unknownHideAttriNum);
	for (int i=0;i<data.known_hide_attr_vec.size();i++)
	{
		CMD_LOG("known_hide_attr_vec[%d] ->type = %d,value = %d",i,data.known_hide_attr_vec[i].attr_type,data.known_hide_attr_vec[i].attr_value);
	}

	for (int i=0; i<data.purify_attr_vec.size(); ++i)
	{
		CMD_LOG("purify_attr_vec[%d] ->type = %d,value = %d",i,data.purify_attr_vec[i].attr_type,data.purify_attr_vec[i].attr_value);
	}
}


void CmdHelper::changeState(int state)
{
	mCurrentState = (DATAPOOL_STATE)state;
}

void CmdHelper::jumpToMain_reload()
{
	changeState(STATE_WORKING);
	GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
}
void CmdHelper::jumpToIndex_error()
{
	state_enable = false;
	changeState(STATE_WORKING);
	LFMessageBox::alert(LF::getString("cuowu"),LF::getString("chongxindenglushibai"));// "重登陆失败","错误");
	GameDirector::getDirector()->index();
}
void CmdHelper::jumpToIndex_kickOut()
{
	state_enable = false;
	changeState(STATE_WORKING);
	LFMessageBox::alert(LF::getString("tishi"),LF::getString("yididenglu"));//"账户在异地登陆，请重新登陆");
	GameDirector::getDirector()->index();
}
void CmdHelper::logOff()
{
	state_enable = false;
	changeState(STATE_WORKING);
	GameDirector::getDirector()->index();
}

void CmdHelper::reload()
{
	CCLog("reload----->");
	MyselfManager::reCreate();

	DATAPOOL_STATE temp = mCurrentState;
	// 临时修改
	// 改成working才可以发送数据，在发送数据期间是不会收到数据的，
	// 因为是线程安全的，所以这里直接修改状态是没问题的
	mCurrentState = STATE_WORKING; 
	cmdGetHerosInTeam(login_choosed_main_hero);
	cmdGetZhuangbeiInTeam();
	cmdGetItemsInTeam();
	cmdQueryTeamSkillList();
	cmdQueryAllMofaJinghua();
	cmdQueryTeamItemSuipian();
	cmdQueryTeamShenbingSuipian();
	cmdQueryArenaTeamBasicInfo();
	mCurrentState = temp;
}

void CmdHelper::sendByteDataToServer(char* netHead,int headLength,char* netBody,int bodyLength)
{
	if (state_enable && isInGame)
	{
		if (mCurrentState == STATE_MODELESS_RECONN ||
			mCurrentState == STATE_MODELESS_NOT_LOGIN ||
			mCurrentState == STATE_MODE_RECONN ||
			mCurrentState == STATE_MODE_NOT_LOGIN ||
			mCurrentState == STATE_MODE_RELOAD)
		{
			CMD_LOG("Error : %s:%d--->%s ",__FUNCTION__,__LINE__,LF::getString("wangluoguzhang").c_str());
			LFMessageBox::alert(LF::getString("cuowu"),LF::getString("wangluoguzhang"));//"网络故障，请检查网络并稍后再试。");
			GameDirector::getDirector()->hideWaiting();
			return;
		}
	}


	CMD_LOG("%s-->%s",ServerTime::getTimeString(ServerTime::getTime()).c_str(),__FUNCTION__);
	NetWorkThreadProc::getProc()->sendData(new RequestMessage(netHead,headLength,netBody,bodyLength));

	// 开始超时监视
	TimeoutMonitor::getMonitor()->startMonitor();
}

//////////////////////////////////////////////////////////////////////////
// 接收回调
//////////////////////////////////////////////////////////////////////////
void CmdHelper::sendDataError()
{
	CCLog("CmdHelper::sendDataError");
	// 取消
	TimeoutMonitor::getMonitor()->cancleMonitor();

	if (state_enable && isInGame)
	{
		if (mCurrentState == STATE_MODELESS_NOT_LOGIN)
		{
			changeState(STATE_MODELESS_RECONN);
		}
		else if (mCurrentState == STATE_WORKING)
		{
			changeState(STATE_MODELESS_RECONN);
			LFMessageBox::alert(LF::getString("cuowu"),LF::getString("wangluoguzhang"));
		}
		else if (mCurrentState == STATE_MODE_NOT_LOGIN)
		{
			changeState(STATE_MODE_RECONN);
		}
		else if (mCurrentState == STATE_MODE_RELOAD)
		{
			changeState(STATE_MODE_RECONN);
		}
	}else
	{
		LFMessageBox::alert(LF::getString("cuowu"),LF::getString("wangluoguzhang"));

	}
	GameDirector::getDirector()->hideWaiting();
}
void CmdHelper::recvDataError()
{
	CCLog("CmdHelper::recvDataError");
	// 取消
	TimeoutMonitor::getMonitor()->cancleMonitor();

	if (state_enable && isInGame)
	{
		if (mCurrentState == STATE_MODELESS_RECONN)
		{
			changeState(STATE_MODE_RECONN);
		}
		else if (mCurrentState == STATE_MODELESS_NOT_LOGIN)
		{
			changeState(STATE_MODELESS_RECONN);
		}
		else if (mCurrentState == STATE_WORKING)
		{
			changeState(STATE_MODE_RECONN);
			LFMessageBox::alert(LF::getString("yihan"),LF::getString("wangluobuwending"));
			return;
		}
		else if (mCurrentState == STATE_MODE_NOT_LOGIN)
		{
			changeState(STATE_MODE_RECONN);
		}
		else if (mCurrentState == STATE_MODE_RELOAD)
		{
			changeState(STATE_MODE_RECONN);
		}
	}
	else
	{
		LFMessageBox::alert(LF::getString("cuowu"),LF::getString("huoqushujushibai"));//"");
	}

	GameDirector::getDirector()->hideWaiting();
}
void CmdHelper::connectServerError()
{
#if DATAPOOL_LOG
	CCLog("CmdHelper::connectServerError");
#endif

	// 取消
	TimeoutMonitor::getMonitor()->cancleMonitor();

	if (state_enable && isInGame)
	{
		if (mCurrentState == STATE_MODELESS_NOT_LOGIN)
		{
			changeState(STATE_MODELESS_RECONN);
		}

		if (mCurrentState == STATE_WORKING)
		{
			changeState(STATE_MODELESS_RECONN);
		}
	}

	if (mCurrentState == STATE_MODE_RECONN)
	{
		return;
	}

	GameDirector::getDirector()->hideWaiting();
}
void CmdHelper::connectServerSuccess()
{
#if DATAPOOL_LOG
	CCLog("CmdHelper::connectServerSuccess");
#endif

	// 取消
	TimeoutMonitor::getMonitor()->cancleMonitor();


	if (state_enable && isInGame)
	{
		if (mCurrentState == STATE_MODELESS_RECONN)
		{
			changeState(STATE_MODELESS_NOT_LOGIN);
			DATAPOOL_STATE temp = mCurrentState;
			mCurrentState = STATE_WORKING; // 临时修改
			cmdReconnect();
			mCurrentState = temp;
		}
		else if (mCurrentState == STATE_MODE_RECONN)
		{
			changeState(STATE_MODE_NOT_LOGIN);
			DATAPOOL_STATE temp = mCurrentState;
			mCurrentState = STATE_WORKING; // 临时修改
			cmdReconnect();
			mCurrentState = temp;
		}
	}else
	{
		if (needSendLoginDataWhenConn) // 重新登陆
		{
			DATAPOOL_STATE temp = mCurrentState;
			mCurrentState = STATE_WORKING; // 临时修改
			cmdLogin(loginedName.c_str(),loginedPsw.c_str());
			mCurrentState = temp;
		}else 
		if (needSendReconnectDataWhenConn)
		{
			DATAPOOL_STATE temp = mCurrentState;
			mCurrentState = STATE_WORKING; // 临时修改
			cmdReconnect();
			mCurrentState = temp;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// DealWith
//////////////////////////////////////////////////////////////////////////

void CmdHelper::dealWithPackage(char* head,char* body)
{
	CMD_LOG("%s : %u-->%s",ServerTime::getTimeString(ServerTime::getTime()).c_str(),ServerTime::getTime(),__FUNCTION__);
	SPHead* _head = (SPHead*)head;
	_head->protocol_tag = ntohs(_head->protocol_tag);
	_head->protocol_ver = ntohs(_head->protocol_ver);
	_head->cmd = ntohs(_head->cmd);
	_head->data_len = ntohs(_head->data_len);
	int cmd = _head->cmd;

	// 心跳包不管
	// 不然我们发了消息之后，收到的先是心跳，那么监视器就取消了
	// 所以心跳包不管
	if (cmd != heartBeatRsp)
	{
		// 取消,但是对于团队副本，竞技等，需要服务器通知的那么还需要再次打开监视
		TimeoutMonitor::getMonitor()->cancleMonitor();
	}
	

	if (state_enable && isInGame)
	{
		if (mCurrentState == STATE_MODELESS_RECONN || mCurrentState == STATE_MODE_RECONN)
			return;

		// 只处理重联的包
		if (mCurrentState == STATE_MODE_NOT_LOGIN && cmd != ReconnectRsp)
			return;

		// 在“模态reload”状态下： 只处理reload包
		if (mCurrentState == STATE_MODE_RELOAD)
		{
			bool isReloadPkg = false;
			int rloadCmdNum = sizeof(reloadCmdLink)/sizeof(SimpleProtocolCmd);
			for (int cmdIndex = 0;cmdIndex < rloadCmdNum;cmdIndex++)
			{
				if (cmd == reloadCmdLink[cmdIndex])
				{
					isReloadPkg = true;
					break;
				}
			}
			if (!isReloadPkg)
			{
				return;
			}
		}
	}

	char _code[64];
	sprintf(_code,"%d",cmd);

	switch (cmd) {
	case clientLoginRsp:
		this->dealWithClientLoginRsp(head,body);
		break;
	case ReconnectRsp:
		this->dealWithReconnectRsp(head,body);
		break;
	case RegisterRsp:
		this->dealWithClientRegistRsp(head,body);
		break;
	case createHeroRsp:
		this->dealWithCreateHeroRsp(head,body);
		break;
	case createAssitRsp :
		this->dealWithCreateAssitRsp(head,body);
		break;
	case getHerosInTeamRsp :
		this->dealWithGetHerosInTeamRsp(head,body);
		break;
	case getZhuangbeiInTeamRsp :
		this->dealWithGetZhuangbeiInTeamRsp(head,body);
		break;
	case equipZhuangbeiRsp :
		this->dealWithEquipZhuangbeiRsp(head,body);
		break;
	case adjustFormationRsp :
		this->dealWithAdjustFormationRsp(head,body);
		break;
	case getTaskListRsp :
		this->dealWithGetTaskListRsp(head,body);
		break;
	case getItemsInTeamRsp :
		this->dealWithGetItemsInTeamRsp(head,body);
		break;
	case ZBQianghuaRsp :
		this->dealWithZBQianghuaRsp(head,body);
		break;
	case ZBQianghuaHuituiRsp :
		this->dealWithZBQianghuaHuituiRsp(head,body);
		break;
	case ZBChaijieRsp :
		this->dealWithZBChaijieRsp(head,body);
		break;
	case ZBXiangBaoshiRsp :
		this->dealWithZBXiangBaoshiRsp(head,body);
		break;
	case ZBChaiBaoshiRsp :
		this->dealWithZBChaiBaoshiRsp(head,body);
		break;
	case BaoshiHechengRsp :
		this->dealWithBaoshiHechengRsp(head,body);
		break;
	case queryMarketRsp :
		this->dealWithGetMarketInfoRsp(head,body);
		break;
	case buyItemInMarketRsp :
		this->dealWithBuyItemInMarketRsp(head,body);
		break;
	case BattleRsp:
		this->dealWithBattleRsp(head,body);
		break;
	case GetTaskChessboardRsp:
		this->dealWithGetTaskChessBoardRsp(head,body);
		break;
	case changeTaskStatRsp:
		this->dealWithChangeTaskStateRsp(head,body);
		break;
	case heartBeatRsp:
		this->dealWithHeartBeatRsp(head,body);
		break;
	case beKickOutRsp:
		this->dealWithBeKickOutRsp(head,body);
		break;
	case arenaEnterRsp:
		this->dealWithArenaEnterRsp(head,body);
		break;
	case arenaUpdateRoomRsp:
		this->dealWithArenaUpdateRoomRsp(head,body);
		break;
	case arenaGetBattleScriptAndGiftRsp:
		this->dealWithArenaGetBattleScriptAndGiftRsp(head,body);
		break;
	case arenaGetGiftRsp:
		this->dealWithArenaGetGiftRsp(head,body);
		break;
	case teamBossEnterRsp:
		this->dealWithTeamBossEnterRsp(head,body);
		break;
	case teamBossUpdateRoomRsp:
		this->dealWithTeamBossUpdateRoomRsp(head,body);
		break;
	case teamBossGetBattleScriptAndGiftRsp:
		this->dealWithTeamBossGetBattleScriptAndGiftRsp(head,body);
		break;
	case teamBossGetGiftRsp:
		this->dealWithTeamBossGetGiftRsp(head,body);
		break;
	case cityGuardEnterRsp:
		this->dealWithCityGuardEnterRsp(head,body);
		break;
	case cityGuardGetNextMonsterRsp:
		this->dealWithCityGuardGetNextMonsterRsp(head,body);
		break;
	case cityGuardFightRsp:
		this->dealWithCityGuardFightRsp(head,body);
		break;
	case queryAnotherTeamInfoRsp:
		this->dealWithQueryAnotherTeamInfoRsp(head,body);
		break;
	case topHeroGetListRsp:
		this->dealWithTopHeroGetListRsp(head,body);
		break;
	case topHeroChallengeRsp:
		this->dealWithTopHeroChallengeRsp(head,body);
		break;
	case systemGiftQueryRsp:
		this->dealWithSystemGiftQueryRsp(head,body);
		break;
	case systemGiftGetGiftRsp:
		this->dealWithSystemGiftGetGiftRsp(head,body);
		break;
	case chargeVitalityRsp:
		this->dealWithchargeVitalityRsp(head,body);
		break;
	case duanzaoListQueryRsp:
		this->dealWithDuanzaoListQueryRsp(head,body);
		break;
	case duanzaoRsp:
		this->dealWithDuanzaoRsp(head,body);
		break;
	case duanzaoGroupZBQueryRsp:
		this->dealWithDuanzaoGroupZBQueryRsp(head,body);
		break;
	case queryAllSuitNameRsp:
		this->dealWithQueryAllSuitNameRsp(head,body);
		break;
	case ZBCuiquRsp:
		this->dealWithZBCuiquRsp(head,body);
		break;
	case ZBFumoRsp:
		this->dealWithZBFumoRsp(head,body);
		break;
	case queryAllMofaJinghuaRsp:
		this->dealWithQueryAllMofaJinghuaRsp(head,body);
		break;
	case queryFubenDiaoluoRsp:
		this->dealWithQueryFubenDiaoluoRsp(head,body);
		break;
	case newAssistantJoinRsp:
		this->dealWithNewAssistantJoinRsp(head,body);
		break;
	case querySubTaskListRsp:
		this->dealWithQuerySubTaskListRsp(head,body);
		break;
	case newSubTaskGotRsp:
		this->dealWithNewSubTaskGotRsp(head,body);
		break;
	case subTaskSubmitedRsp:
		this->dealWithSubTaskSubmitedRsp(head,body);
		break;
	case queryMainTaskRewardRsp:
		this->dealWithQueryMainTaskRewardRsp(head,body);
		break;
	case chargeGoldRsp:
		this->dealWithChargeGoldRsp(head,body);
		break;
	case queryTeamSkillListRsp:
		this->dealWithQueryTeamSkillListRsp(head,body);
		break;
	case equipSkillRsp:
		this->dealWithEquipSkillRsp(head,body);
		break;
	case actorLearnSkillRsp:
		this->dealWithActorLearnSkillRsp(head,body);
		break;
	case queryTeamShenbingSuipianRsp:
		this->dealWithQueryTeamShenbingSuipianRsp(head,body);
		break;
	case shenbingHechengRsp:
		this->dealWithShenbingHechengRsp(head,body);
		break;
	case queryTeamItemSuipianRsp:
		this->dealWithQueryTeamItemSuipianRsp(head,body);
		break;
	case itemHechengRsp:
		this->dealWithItemHechengRsp(head,body);
		break;
	case queryAllShenbingListRsp:
		this->dealWithQueryAllShenbingListRsp(head,body);
		break; 
	case saodangFubenRsp:
		this->dealWithSaodangFubenRsp(head,body);
		break;
	case heroLevelUpRsp:
		this->dealWithHeroLevelUpRsp(head,body);
		break;
	case heroUpgradeRsp:
		this->dealWithHeroUpgradeRsp(head,body);
		break;
	case shenbingJieFengyinRsp:
		this->dealWithShenbingJieFengyinRsp(head,body);
		break;
	case shenbingJiandingRsp:
		this->dealWithShenbingJiandingRsp(head,body);
		break;
	case zbJinglianRsp:
		this->dealWithZBJinglianRsp(head,body);
		break;
	case shenbingZhuankeJingwenRsp:
		this->dealWithShenbingZhuankeJingwenRsp(head,body);
		break;
	case queryArenaTeamBasicInfoRsp:
		this->dealWithQueryArenaTeamBasicInfoRsp(head,body);
		break;
	case queryArenaMarketRsp:
		this->dealWithQueryArenaMarketRsp(head,body);
		break;
	case refreshArenaChlgListRsp:
		this->dealWithRefreshArenaChlgListRsp(head,body);
		break;
	case queryArenaChlgListRsp:
		this->dealWithQueryArenaChlgListRsp(head,body);
		break;
	case arenaChallengeRsp:
		this->dealWithArenaChallengeRsp(head,body);
		break;
	case arenaMarketBuyRsp:
		this->dealWithArenaMarketBuyRsp(head,body);
		break;
	case arenaBuyChlgTimeRsp:
		this->dealWithArenaBuyChallengeTimeRsp(head,body);
		break;
	case queryZahuopuRsp:
		this->dealWithQueryZahuopuRsp(head,body);
		break;
	case refreshZahuopuRsp:
		this->dealWithRefreshZahuopuRsp(head,body);
		break;
	case zahuopuGetOneItemRsp:
		this->dealWithZahuopuGetOneItemRsp(head,body);
		break;
	case queryHellTaskHeroInfoRsp:
		this->dealWithQueryHellTaskHeroInfoRsp(head,body);
		break;
	case hellTaskBattleRsp:
		this->dealWithHellTaskBattleRspRsp(head,body);
		break;
	case queryYuanshenOfHeroRsp:
		this->dealWithQueryYuanshenOfHeroRsp(head,body);
		break;
	case churchLevelUpRsp:
		this->dealWithChurchLevelUpRsp(head,body);
		break;
	case skillLevelUpRsp:
		this->dealWithSkillLevelUpRsp(head,body);
		break;
	case getZhuangbeiAlbumRsp:
		this->dealWithGetZhuangbeiAlbumRsp(head,body);
		break;
	case cityGuardProcessNotify:
		this->dealWithCityGuardProcessNotifyRsp(head,body);
		break;
	case getCityGuardCurrentStatRsp:
		this->dealWithGetCityGuardCurrentStatRsp(head,body);
		break;
	case cityGuardBattleRsp:
		this->dealWithCityGuardBattleRsp(head,body);
		break;
	case getCityGuardLastDayReportRsp:
		this->dealWithGetCityGuardLastDayReportRsp(head,body);
		break;
	case getMyCityGuardRewardRsp:
		this->dealWithGetMyCityGuardRewardRsp(head,body);
		break;
//20140613 奖励系统变更（奖励与悬赏任务合并）
// 	case queryDailyLoginBonusRsp:
// 		this->dealWithQueryDailyLoginBonusRsp(head,body);
// 		break;
// 	case queryLevelBonusRsp:
// 		this->dealWithQueryLevelBonusRsp(head,body);
// 		break;
// 	case querySystemBonusRsp:
// 		this->dealWithQuerySystemBonusRsp(head,body);
// 		break;
// 	case getSystemBonusRsp:
// 		this->dealWithGetSystemBonusRsp(head,body);
// 		break;
// 	case getDailyLoginBonusRsp:
// 		this->dealWithGetDailyLoginBonusRsp(head,body);
// 		break;
// 	case getLevelBonusRsp:
// 		this->dealWithGetLevelBonusRsp(head,body);
// 		break;
	case queryBonusRsp:
		this->dealWithQueryBonusRsp(head,body);
		break;
	case getBonusRsp:
		this->dealWithGetBonusRsp(head,body);
		break;
	case sendBonusNotify://系统推送
		this->delWithSystemPushBonusRsp(head,body);
		break;

// 	case getChessRandomBoxRsp:
// 		this->dealWithGetChessRandomBoxRsp(head,body);
// 		break;
	case queryRateTaskRsp:
		this->dealWithQueryTaskRateRsp(head,body);
		break;
	case queryRateTaskBonusRsp:
		this->dealWithQueryRateTaskBonusRsp(head,body);
		break;
	case getRateTaskBonusRsp:
		this->dealWithGetTaskRateBonusRsp(head,body);
		break;
	case queryVipPrivilegeRsp:
		this->dealWithQueryVipPrivilegeRsp(head,body);
		break;
	case upgradeShenbingRsp:
		this->dealWithUpgradeShenbingRsp(head,body);
		break;
	case queryChargePacketRsp:
		this->dealWithQueryChargePacketRsp(head,body);
		break;
	case unlockAssistantRsp:
		this->dealWithUnlockAssistantRsp(head,body);
		break;
	case queryGiftActivityRsp:
		this->dealWithQueryGiftActivityRsp(head,body);
		break;
	case getGiftActivityRsp:
		this->dealWithGetGiftActivityRsp(head,body);
		break;
	case queryKilledMonsterRsp:
		this->dealWithQueryKilledMonsterRsp(head,body);
		break;
	case queryKilledMonsterBonusRsp:
		this->dealWithQueryKilledMonsterBonusRsp(head,body);
		break;
	case getKilledMonsterBonusRsp:
		this->dealWithGetKilledMonsterBonusRsp(head,body);
		break;
	case topArenaQueryGuessRsp:
		this->dealWithTopArenaQueryGuessRsp(head,body);
		break;
	case topArenaQueryStatusRsp:
		this->dealWithTopArenaQueryStatusRsp(head,body);
		break;
	case topArenaGuessRsp:
		this->dealWithTopArenaGuessRsp(head,body);
		break;
	case topArenaReplayRsp:
		this->dealWithTopArenaReplayRsp(head,body);
		break;
	case castSilverRsp:
		this->dealWithCastSilverRsp(head,body);
		break;
	case queryLegendNoteRsp:
		this->dealWithQueryLegendNoteRsp(head,body);
		break;
	case challengeLegendNoteRsp:
		this->dealWithChallengeLegendNoteRsp(head,body);
		break;
	case querySelfChallengeInfoRsp:
		this->dealWithQuerySelfChallengeInfoRsp(head,body);
		break;
	case querySelfChallengeOrderRsp:
		this->dealWithQuerySelfChallengeOrderRsp(head,body);
		break;
	case selfChallengeRsp:
		this->dealWithSelfChallengeRsp(head,body);
		break;
	case selfChallengeGainRsp:
		this->dealWithSelfChallengeGainRsp(head,body);
		break;
	case getSelfChallengeBoxRsp:
		this->dealWithGetSelfChallengeBoxRsp(head,body);
		break;
	case queryProtectSpiritRsp:
		this->dealWithQueryProtectSpiritRsp(head,body);
		break;
	case challengeProtectSpiritRsp:
		this->dealWithChallengeProtectSpiritRsp(head,body);
		break;
	case getProtectSpiritBonusRsp:
		this->dealWithGetProtectSpiritBonusRsp(head,body);
		break;
	case worshiProtectSpiritRsp:
		this->dealWithWorshiProtectSpiritRsp(head,body);
		break;
	case queryDiscoverInfoRsp:
		this->dealWithQueryDiscoverInfoRsp(head,body);
		break;
	case discoverRsp:
		this->dealWithDiscoverRsp(head,body);
		break;
	case unlockDiscoverSealRsp:
		this->dealWithUnlockDiscoverSealRsp(head,body);
		break;
	case topHeroClearCDTimeRsp:
		this->dealWithTopHeroClearCDTimeRsp(head,body);
		break;
	case quitCityGuardRsp:
		this->dealWithQuitCityGuardRsp(head,body);
		break;
	case flopTaskGridRsp:
		this->dealWithFlopTaskGridRsp(head,body);
		break;
	case queryPurifyRsp:
		this->dealWithQueryPurifyRsp(head,body);
		break;
	case purifyRsp:
		this->dealWithPurifyRsp(head,body);
		break;
	case confirmPurifyRsp:
		this->dealWithConfirmPurifyRsp(head,body);
		break;
	case queryAssessWeaponRsp:
		this->dealWithQueryAssessWeaponRsp(head,body);
		break;
	case assessWeaponRsp:
		this->dealWithAssessWeaponRsp(head,body);
		break;

	case queryEmailRsp:
		this->dealWithQueryEmailRsp(head,body);
		break;
	case readEmailRsp:
		this->dealWithReadEmailRsp(head,body);
		break;
	case deleteEmailRsp:
		this->dealWithDeleteEmailRsp(head,body);
		break;
	case pushEmail://系统邮件推送(系统邮件、玩家互发邮件)
		this->dealWithPushEmail(head,body);
		break;
	case queryGiftBagRsp:
		this->dealWithQuerySystemGiftBagRsp(head,body);
		break;
	case getGiftBagRsp:
		this->dealWithGetSystemGiftBagRsp(head,body);
		break;
	case queryOriginRsp:
		this->dealWithQueryEquipFallRsp(head,body);
		break;
	case queryHeroOtherInfoRsp:
		this->dealWithQueryHeroOtherInfoRsp(head,body);
		break;
	case queryNoticeRsp:
		this->dealWithSystemNotifyInfoRsp(head,body);
		break;
	case queryInstructionRsp:
		this->dealWithGetGuideStateRsp(head,body);
		break;
	case completeInstructionRsp:
		this->dealWithSetGuideStateRsp(head,body);
		break;

		
	default:
#if (DATAPOOL_LOG == 1)
		CCLog("Error : %s,this head cmd have no deal cmd = %d",__FUNCTION__,_code);
#endif
		LFMessageBox::alert("CmdHelper","Error:unknown SimpleProtocolCmd ["+string(_code)+"]");
		break;
	}
}

void CmdHelper::dealWithClientLoginRsp(char* head,char* body)
{
	SPCmd_ClientLoginRsp* rsp = (SPCmd_ClientLoginRsp*)body;

	rsp->err_code = ntohl(rsp->err_code);

	CCLog("DataPool::dealWithClientLoginRsp ----err_code = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->hero_count = ntohl(rsp->hero_count);
		rsp->server_time = ntohl(rsp->server_time);

		CCLog("DataPool::dealWithClientLoginRsp ---- hero_count = %d ,server_time = [%s]",rsp->hero_count,ServerTime::getTimeString(rsp->server_time).c_str());

		HeroInfo* zhuYinXiongArray = (HeroInfo*)rsp->hero_data;
		for (int i=0;i<rsp->hero_count;i++)
		{
			CMD_LOG("===================zhuYinXiongArray[%d] begin===========================",i);
			hton_HeroInfo(zhuYinXiongArray[i]);
			log(zhuYinXiongArray[i]);
			CMD_LOG("===================zhuYinXiongArray[%d] end===========================",i);

		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_clientLoginRsp,MSG_Rsp::createMsg(rsp));
}

//
void CmdHelper::dealWithReconnectRsp(char* head,char* body)
{
	SPCmd_ReconnectRsp* rsp = (SPCmd_ReconnectRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CCLog("DataPool::dealWithReconnectRsp ----err_code = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->server_time = ntohl(rsp->server_time);
		string t = ServerTime::getTimeInString(rsp->server_time);
		CCLog("DataPool::dealWithReconnectRsp ---- server_time = %d->[%s]",rsp->server_time,t.c_str());
	}

	if (state_enable && isInGame)
	{ 
		//如果为reconnect的成功响应包，则转换到“工作”状态。
		if (mCurrentState == STATE_MODELESS_NOT_LOGIN)
		{
			if (rsp->err_code == Success)
			{
				changeState(STATE_WORKING);
			}else
			{
				jumpToIndex_error();
				return;
			}
		}
		//接收到Type_RecvData消息，如果为reconnect的成功响应包，则转换到“模态reload”状态，
		else if(mCurrentState == STATE_MODE_NOT_LOGIN)
		{
			if (rsp->err_code == Success)
			{
				changeState(STATE_MODE_RELOAD);
				reload();
			}else
			{
				jumpToIndex_error();
				return;
			}
		}
	}
	

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_reconnectRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithClientRegistRsp(char* head,char* body)
{
	SPCmd_RegisterRsp* rsp = (SPCmd_RegisterRsp*)body;

	rsp->err_code = ntohl(rsp->err_code);

	CCLog("DataPool::dealWithClientRegistRsp ----err_code = %d",rsp->err_code);
	if (rsp->err_code == Success)
	{
		rsp->server_time = ntohl(rsp->server_time);
		CCLog("DataPool::dealWithClientRegistRsp ----server_time = %d->[%s]",rsp->server_time,ServerTime::getTimeInString(rsp->server_time));
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_clientRegistRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithEquipZhuangbeiRsp(char* head,char* body)
{
	SPCmd_EquipZhuangbeiRsp* rsp = (SPCmd_EquipZhuangbeiRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
#if (DATAPOOL_LOG == 1)
	CCLog("\ndealWithEquipZhuangbeiRsp --- err_code = %d",rsp->err_code);
#endif

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_equipZhuangbeiRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithCreateHeroRsp(char* head,char* body)
{
	SPCmd_CreateHeroRsp* rsp = (SPCmd_CreateHeroRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	rsp->hero_id = ntohl(rsp->hero_id);

#if (DATAPOOL_LOG == 1)
	CCLog("\n dealWithCreateHeroRsp --- err_code = %d,hero_id = %d",rsp->err_code,rsp->hero_id);
#endif

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_createHeroRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithCreateAssitRsp(char* head,char* body)
{
	SPCmd_CreateAssitRsp* rsp = (SPCmd_CreateAssitRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	if (rsp->err_code == Success)
	{
		hton_HeroDetail(rsp->hero_detail);
#if (DATAPOOL_LOG == 1)
		log(rsp->hero_detail);
#endif
	}


	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_createAssitRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithGetHerosInTeamRsp(char* head,char* body)
{
	SPCmd_GetHerosInTeamRsp* rsp = (SPCmd_GetHerosInTeamRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

#if (DATAPOOL_LOG == 1)
	CCLog("\n------------------------dealWithGetHerosInTeamRsp---------------------");
	CCLog("err_code = %d",Success);
#endif

	if (rsp->err_code == Success)
	{
		rsp->church_level = ntohl(rsp->church_level);
		CMD_LOG("church_level = %d",rsp->church_level);
		rsp->hero_count = ntohl(rsp->hero_count);
		HeroDetail* detailArray = rsp->hero_detail_arr;

		for (int i=0;i<rsp->hero_count;i++)
		{
			hton_HeroDetail(detailArray[i]);
#if (DATAPOOL_LOG == 1)
			CCLog("\n--------------------HeroDetail[%d]---------------------------------",i);
			log(detailArray[i]);
			CCLog("--------------------HeroDetail[%d]---------------------------------",i);
#endif
		}
	}

#if (DATAPOOL_LOG == 1)
	CCLog("------------------------dealWithGetHerosInTeamRsp---------------------");
#endif
	if (!reloadCheck(rsp->err_code,(SimpleProtocolCmd)((SPHead*)head)->cmd,MSG_getHerosInTeamRsp,MSG_Rsp::createMsg(rsp)))
	{
		return;
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getHerosInTeamRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithGetZhuangbeiInTeamRsp(char* head,char* body)
{
	SPCmd_GetZhuangbeiInTeamRsp* rsp = (SPCmd_GetZhuangbeiInTeamRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_GetZhuangbeiInTeamRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		ZhuangbeiDetail* detailArray = (ZhuangbeiDetail*)rsp->data;

		CS::ntoh_zhuangbei_detail_2_equipdata(detailArray,rsp->zhuangbei_count,flat_data.equips);

#if DATAPOOL_LOG
		for (int i=0;i<rsp->zhuangbei_count;i++)
		{
			log(flat_data.equips[i]);
		}
#endif
	}

	if (!reloadCheck(rsp->err_code,(SimpleProtocolCmd)((SPHead*)head)->cmd,MSG_getZhuangbeiInTeamRsp_F,Flat_MSG_Rsp::createMsg(&flat_data)))
	{
		return;
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getZhuangbeiInTeamRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithAdjustFormationRsp(char* head,char* body)
{
	SPCmd_AdjustFormationRsp* rsp = (SPCmd_AdjustFormationRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",rsp->err_code);
	CMD_LOG("------------------------%s---------------------",__FUNCTION__);

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_adjustFormationRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithGetTaskListRsp(char* head,char* body)
{
	SPCmd_GetTaskListRsp* rsp = (SPCmd_GetTaskListRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",rsp->err_code);

	// 成功！
	if (rsp->err_code == Success)
	{
		rsp->maintask_last_actived = ntohl(rsp->maintask_last_actived);
		rsp->maintask_status = ntohl(rsp->maintask_status);
		CMD_LOG("maintask_last_actived = %d,maintask_status = %d",
			rsp->maintask_last_actived,rsp->maintask_status);
	}

	CMD_LOG("------------------------%s---------------------",__FUNCTION__);

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getTaskListRsp,MSG_Rsp::createMsg(rsp));
}
//
void CmdHelper::dealWithGetItemsInTeamRsp(char* head,char* body)
{
	SPCmd_GetItemsInTeamRsp* rsp = (SPCmd_GetItemsInTeamRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s,%d",__FILE__,__LINE__);
	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("----%s--errorCode = %d,itemNum = %d",__FUNCTION__,rsp->err_code,rsp->count);
		for (int i=0;i<rsp->count;i++)
		{
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			CMD_LOG("--item[%d] : count = %d,type = %d---",i,rsp->item_arr[i].count,rsp->item_arr[i].type);
		}
	}

	if (!reloadCheck(rsp->err_code,(SimpleProtocolCmd)((SPHead*)head)->cmd,MSG_getItemsInTeamRsp,MSG_Rsp::createMsg(rsp)))
	{
		return;
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getItemsInTeamRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithZBQianghuaRsp(char* head,char* body)
{
	SPCmd_ZBQianghuaRsp* rsp = (SPCmd_ZBQianghuaRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s --> errorCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		// qianghuainfor全是char，不用转换
		CMD_LOG("----%s--id = %d,level = %d,percent = %d",__FUNCTION__,
			rsp->qianghua_info.id,rsp->qianghua_info.level,rsp->qianghua_info.percent);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ZBQianghuaRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithZBQianghuaHuituiRsp(char* head,char* body)
{
	SPCmd_ZBQianghuaHuituiRsp* rsp = (SPCmd_ZBQianghuaHuituiRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s --> errorCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		// 都是char，不用转字节序

		CMD_LOG("qianghuaData = {max_qiang_hua_time = %d,qianghua_count = %d}",
			rsp->new_qianghua_data.max_qianghua_time,rsp->new_qianghua_data.qianghua_count);
		CMD_LOG("--------qiang_hua_infor------------");
		for (int i=0;i<rsp->new_qianghua_data.qianghua_count;i++)
		{
			CMD_LOG("[%d]-->{id = %d,level = %d,percent = %d}",i,
				rsp->new_qianghua_data.info_arr[i].id,
				rsp->new_qianghua_data.info_arr[i].level,
				rsp->new_qianghua_data.info_arr[i].percent);
		}
		CMD_LOG("--------qiang_hua_infor------------");
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ZBQianghuaHuituiRsp,MSG_Rsp::createMsg(rsp));


}
void CmdHelper::dealWithZBZhuankongRsp(char* head,char* body)
{
	// 没有钻孔了
	/*
	SPCmd_ZBZhuankongRsp* rsp = (SPCmd_ZBZhuankongRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithZBZhuankongRsp---> erroCode = %d",rsp->err_code);
#endif
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ZBZhuankongRsp,MSG_Rsp::createMsg(rsp));
	*/
}
void CmdHelper::dealWithZBChaijieRsp(char* head,char* body)
{
	SPCmd_ZBChaijieRsp* rsp = (SPCmd_ZBChaijieRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	Flat_ZBChaijieRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("DataPool::dealWithZBChaijieRsp---> erroCode = %d",rsp->err_code);
	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);

		CCLog("DataPool::dealWithZBChaijieRsp---> rsp->count = %d",rsp->count);

		int i=0;
		for (;i<rsp->count;i++)
		{
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			flat_data.item_arr.push_back(rsp->item_arr[i]);
			CMD_LOG("ItemInfo num = %d, ItemInfo[%d] = {type = %d,count = %d}",rsp->count,i,rsp->item_arr[i].type,rsp->item_arr[i].count);
		}
		char *pData = (char *)&rsp->item_arr[i-1] + sizeof(unsigned int) * 2;
		int re_count = ntohl(*(int*)pData);
		pData += sizeof(int);
		ItemInfo *pItems = (ItemInfo*)pData;
		for (i=0; i<re_count; ++i)
		{
			pItems[i].count = ntohl(pItems[i].count);
			pItems[i].type = ntohl(pItems[i].type);
			flat_data.re_item_arr.push_back(pItems[i]);
			CMD_LOG("Return ItemInfo num = %d, ItemInfo[%d] = {type = %d,count = %d}",re_count,i,pItems[i].type,pItems[i].count);
		}
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ZBChaijieRsp,MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithZBXiangBaoshiRsp(char* head,char* body)
{
	SPCmd_ZBXiangBaoshiRsp* rsp = (SPCmd_ZBXiangBaoshiRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ZBXiangBaoshiRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithZBChaiBaoshiRsp(char* head,char* body)
{
	SPCmd_ZBChaiBaoshiRsp* rsp = (SPCmd_ZBChaiBaoshiRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ZBChaiBaoshiRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithBaoshiHechengRsp(char* head,char* body)
{
	SPCmd_BaoshiHechengRsp* rsp = (SPCmd_BaoshiHechengRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_BaoshiHechengRsp,MSG_Rsp::createMsg(rsp));
}

void ntoh_MarketAttachItem(AttachItem& item, AttachType type)
{
	switch (type)
	{
	case AttachType_Gold:
	case AttachType_Silver:
	case AttachType_Item:
	case AttachType_Diamond:
	case AttachType_Props:
		item.item.type = ntohl(item.item.type);
		item.item.count = ntohl(item.item.count);
		break;
	case AttachType_PropsDebris:
		item.item_debris.item_id = ntohl(item.item_debris.item_id);
		item.item_debris.suipian_id = ntohl(item.item_debris.suipian_id);
		item.item_debris.suipian_count = ntohl(item.item_debris.suipian_count);
		break;
	case AttachType_ArmsDebris:
		item.zb_debris.zb_group_id = ntohl(item.zb_debris.zb_group_id);
		item.zb_debris.suipian_id = ntohl(item.zb_debris.suipian_id);
		item.zb_debris.suipian_count = ntohl(item.zb_debris.suipian_count);
		break;
	case AttachType_GiftBag:
		item.gift.id = ntohl(item.gift.id);
		item.gift.count = ntohl(item.gift.count);
		break;
	case AttachType_Vitality:
		item.vitality = ntohl(item.vitality);
		break;
	case AttachType_Arms:
	default:
		CCAssert(false, "error branch");
		break;
	}
}
void CmdHelper::dealWithGetMarketInfoRsp(char* head,char* body)
{
	SPCmd_QueryMarketRsp* rsp = (SPCmd_QueryMarketRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		rsp->item_count = ntohs(rsp->item_count);
		CMD_LOG("%s---> item_count = %d",__FUNCTION__,rsp->item_count);
		for (int i=0;i<rsp->item_count;i++)
		{
			rsp->item_arr[i].id = ntohl(rsp->item_arr[i].id);
			rsp->item_arr[i].sell_type = ntohs(rsp->item_arr[i].sell_type);	
			ntoh_MarketAttachItem(rsp->item_arr[i].sell_item, (AttachType)rsp->item_arr[i].sell_type);
			rsp->item_arr[i].is_unlimit;
			rsp->item_arr[i].sell_count = ntohl(rsp->item_arr[i].sell_count);	
			rsp->item_arr[i].need_type = ntohs(rsp->item_arr[i].need_type);
			ntoh_MarketAttachItem(rsp->item_arr[i].need_item, (AttachType)rsp->item_arr[i].need_type);
			rsp->item_arr[i].discount = ntohs(rsp->item_arr[i].discount);
			rsp->item_arr[i].sale_time = ntohl(rsp->item_arr[i].sale_time);	
			rsp->item_arr[i].level_type = ntohs(rsp->item_arr[i].level_type);
			rsp->item_arr[i].level = ntohl(rsp->item_arr[i].level);	
			rsp->item_arr[i].bought_times = ntohl(rsp->item_arr[i].bought_times);
			CCLog("item[%d]: id=[%d], sell_type=[%d], sellItem=[%d,%d,%d], is_unlimit=[%d], sell_count=[%d], need_type=[%d], needItem=[%d,%d,%d], discount=[%d], sale_time=[%d], level_type=[%d], level=[%d], bought_times=[%d]",
				i, rsp->item_arr[i].id, rsp->item_arr[i].sell_type, 
				rsp->item_arr[i].sell_item.item_debris.item_id,rsp->item_arr[i].sell_item.item_debris.suipian_count,rsp->item_arr[i].sell_item.item_debris.suipian_id,
				rsp->item_arr[i].is_unlimit, rsp->item_arr[i].sell_count, rsp->item_arr[i].need_type,
				rsp->item_arr[i].need_item.item_debris.item_id,rsp->item_arr[i].need_item.item_debris.suipian_count,rsp->item_arr[i].need_item.item_debris.suipian_id,
				rsp->item_arr[i].discount, rsp->item_arr[i].sale_time, rsp->item_arr[i].level_type, rsp->item_arr[i].level, rsp->item_arr[i].bought_times);
		}
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getMarketInfoRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithBuyItemInMarketRsp(char* head,char* body)
{
	SPCmd_BuyItemInMarketRsp* rsp = (SPCmd_BuyItemInMarketRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		//no thing
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_buyItemInMarketRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithBattleRsp(char* head,char* body)
{
	FightScriptBuilder buider;
	int code;
	vector<FightScript> scripts = buider.SPCmd_BattleRsp_2_FightScript(body,code);

	OneBattle data;
	data.err_code = (SimpleProtocolErrorCode)code;
	data.scriptVec = scripts;

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,code);
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_BattleRsp,MSG_Rsp::createMsg(&data));
}
//
void CmdHelper::dealWithGetTaskChessBoardRsp(char* head,char* body)
{
	SPCmd_GetTaskChessBoardRsp* rsp = (SPCmd_GetTaskChessBoardRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		rsp->horizontal_count = ntohl(rsp->horizontal_count);
		rsp->vertical_count = ntohl(rsp->vertical_count);
		CMD_LOG("horizontal_count = %u,vertical_count = %u",rsp->horizontal_count,rsp->vertical_count);

		for (int i=0; i<rsp->horizontal_count*rsp->vertical_count+1; ++i)
		{
			rsp->grid_arr[i].type = ntohl(rsp->grid_arr[i].type);
			rsp->grid_arr[i].stat = ntohl(rsp->grid_arr[i].stat);
			rsp->grid_arr[i].monster_type = ntohl(rsp->grid_arr[i].monster_type);
		}

#if (DATAPOOL_LOG == 1)
		CCLog("%s---> grid_stat",__FUNCTION__);
		for (int i=0; i<rsp->horizontal_count*rsp->vertical_count+1; i++)
		{
			CCLog("%s---> grid[%d] = type[%d], state[%d], monster_type[%d]",__FUNCTION__, i,
				rsp->grid_arr[i].type, rsp->grid_arr[i].stat, rsp->grid_arr[i].monster_type);
		}
#endif
		
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_GetTaskChessBoardRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithFlopTaskGridRsp(char* head,char* body)
{
	SPCmd_FlopTaskGridRsp* rsp = (SPCmd_FlopTaskGridRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_FlopTaskGridRsp flat_data;
	flat_data.err_code = rsp->err_code;
	if (flat_data.err_code == Success)
	{
		rsp->boss_stat = ntohl(rsp->boss_stat);
		rsp->grid_stat = ntohl(rsp->grid_stat);
		rsp->count = ntohl(rsp->count);
		CMD_LOG("%s ---> boss_stat = %u, grid_stat = %u, boxCount = %d",__FUNCTION__,rsp->boss_stat,rsp->grid_stat, rsp->count);
		flat_data.boss_stat = rsp->boss_stat;
		flat_data.grid_stat = rsp->grid_stat;
		flat_data.count = rsp->count;

		for (int i=0; i<rsp->count; ++i)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			CMD_LOG("%s ---> itemType=%d, itemCount=%d",__FUNCTION__,rsp->item_arr[i].type,rsp->item_arr[i].count);
			flat_data.items.push_back(rsp->item_arr[i]);
		}
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_FlopTaskGridRsp,Flat_MSG_Rsp::createMsg(&flat_data));
}

//
void CmdHelper::dealWithChangeTaskStateRsp(char* head,char* body)
{
	SPCmd_ChangeTaskStatRsp* rsp = (SPCmd_ChangeTaskStatRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_ChangeTaskStatRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		flat_data.exp = ntohl(rsp->exp);
		flat_data.silver = ntohl(rsp->silver);
		rsp->hero_count = ntohl(rsp->hero_count);

		CMD_LOG("getted exp = %u,getted silver = %u,hero_count = %u",rsp->exp,rsp->silver,rsp->hero_count);

		HeroDetail* detailArray = rsp->hero_arr;

		for (int i=0;i<rsp->hero_count;i++)
		{
			hton_HeroDetail(detailArray[i]);
			CMD_LOG("\n--------------------HeroDetail[%d]---------------------------------",i);
			log(detailArray[i]);

			HeroBaseProperty pro(&detailArray[i]);
			HeroFixedPro fixPro = BaseActorPropertyParser::getParser()->getExtraProerty(pro.profession);
			pro.initFixedProValue(fixPro);
			flat_data.hero_new_attri_vec.push_back(pro);

			CMD_LOG("--------------------HeroDetail[%d]---------------------------------",i);
		}

	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ChangeTaskStatRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithRestoreVitalityRsp(char* head,char* body)
{
	SPCmd_RestoreVitalityRsp* rsp = (SPCmd_RestoreVitalityRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithRestoreVitalityRsp---> erroCode = %d",rsp->err_code);
#endif
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_restoreVitalityRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithBeKickOutRsp(char* head,char* body)
{
	SPCmd_BeKickOutRsp* rsp = (SPCmd_BeKickOutRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithBeKickOutRsp---> erroCode = %d",rsp->err_code);
#endif
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_beKickOutRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithHeartBeatRsp(char* head,char* body)
{
	SPCmd_HeartBeatRsp* rsp = (SPCmd_HeartBeatRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithHeartBeatRsp---> erroCode = %d",rsp->err_code);
#endif

	Flat_HeartBeatRsp flatData;
	flatData.err_code = rsp->err_code;
	if (rsp->err_code == Success)
	{
		//1. 体力
		rsp->current_vit = ntohl(rsp->current_vit);
		flatData.current_vit = rsp->current_vit;
#if (DATAPOOL_LOG == 1)
		CCLog("DataPool::dealWithHeartBeatRsp---> current_vit = %d",rsp->current_vit);
#endif

		//2. 推送消息（跑马灯）
		int count = rsp->msg_count;
		CMD_LOG("msg_count = %d", count);
		for (int i=0; i<count; ++i)
		{
			rsp->msg_arr[i].uid = ntohl(rsp->msg_arr[i].uid);
			rsp->msg_arr[i].type = ntohs(rsp->msg_arr[i].type);
			rsp->msg_arr[i].object.chapter_id = ntohl(rsp->msg_arr[i].object.chapter_id);
			rsp->msg_arr[i].count = ntohs(rsp->msg_arr[i].count);
			CMD_LOG("uid=%u, type=%d, msg=%s, object=%u, count=%d", rsp->msg_arr[i].uid, rsp->msg_arr[i].type, rsp->msg_arr[i].subject, 
				rsp->msg_arr[i].object.chapter_id, rsp->msg_arr[i].count);
			flatData.msgVec.push_back(rsp->msg_arr[i]);
		}
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_heartBeatRsp,Flat_MSG_Rsp::createMsg(&flatData));
}

void CmdHelper::dealWithArenaEnterRsp(char* head,char* body)
{
	// 重新监视，因为还要等待Update或者脚本的到来
	TimeoutMonitor::getMonitor()->startMonitor(MAX_TIME_WAIT_SERVER_CALL);

	SPCmd_ArenaEnterRsp* rsp = (SPCmd_ArenaEnterRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_ArenaEnterRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("DataPool::dealWithArenaEnterRsp---> erroCode = %d",rsp->err_code);
	if (rsp->err_code == Success)
	{
		CMD_LOG("%s---> people already here.",__FUNCTION__);
		for (int i=0;i<3;i++)
		{
			if (rsp->hero_info_arr[i].hero_id == INVALID_ID)
				continue;

			hton_HeroInfo(rsp->hero_info_arr[i]);
			flat_data.already_waited.push_back(rsp->hero_info_arr[i]);
			CMD_LOG("\n--------------------HeroInfo[%d]---------------------------------",i);
			log(rsp->hero_info_arr[i]);
			CMD_LOG("--------------------HeroInfo[%d]---------------------------------",i);
		}
		CMD_LOG("%s---> people already here.\n",__FUNCTION__);

		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,rsp->zhuangbei_count,flat_data.allEquips);
#if DATAPOOL_LOG == 1
		for (int i=0;i<rsp->zhuangbei_count;i++)
		{
			log(flat_data.allEquips[i]);
		}
#endif
	}

	jingJiChang_EnterSend = false;
	jingJiChang_EnterSuccess = rsp->err_code == Success;

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_arenaEnterRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithArenaUpdateRoomRsp(char* head,char* body)
{
	// 重新监视，因为还要等待Update或者脚本的到来
	TimeoutMonitor::getMonitor()->startMonitor(MAX_TIME_WAIT_SERVER_CALL);

	SPCmd_ArenaUpdateRoomRsp* rsp = (SPCmd_ArenaUpdateRoomRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_ArenaUpdateRoomRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		hton_HeroInfo(rsp->new_hero_info);
		flat_data.new_hero_info = rsp->new_hero_info;
		log(rsp->new_hero_info);

		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,rsp->zhuangbei_count,flat_data.equipsOnHero);

#if DATAPOOL_LOG == 1
		for (int equipeIndex = 0;equipeIndex<rsp->zhuangbei_count;equipeIndex++)
		{
			log(flat_data.equipsOnHero[equipeIndex]);
		}
#endif
	}
	// 是否之前成功进入了
	if (jingJiChang_EnterSuccess)
	{
		// 发送消息
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_arenaUpdateRoomRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
	}
	else
	{
		CCLog("Waring : %s--->jingJiChang_EnterSuccess == false,so network will throw away this package!",__FUNCTION__);
	}
}
void CmdHelper::dealWithArenaGetBattleScriptAndGiftRsp(char* head,char* body)
{
	//// 这里就不用取消的，因为收到数据会自动取消的
	//TimeoutMonitor::getMonitor()->cancleMonitor();

	SPCmd_ArenaGetBattleScriptGiftRsp* rsp = (SPCmd_ArenaGetBattleScriptGiftRsp*)body;

	Flat_ArenaGetBattleScriptGiftRsp flat_data;
	flat_data.err_code = ntohl(rsp->err_code);

#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithArenaGetBattleScriptAndGiftRsp---> erroCode = %d",rsp->err_code);
#endif
	if (rsp->err_code == Success)
	{
		//int total_count = rsp->gift_total_count;
		int getted_count = rsp->gift_available_count;	// 可以开宝箱的数目

		flat_data.battle_time_remainder = ntohl(rsp->battle_time_remainder);

		flat_data.first_turn_hero_id_arr[0] = ntohl(rsp->first_turn_hero_id_arr[0]);
		flat_data.first_turn_hero_id_arr[1] = ntohl(rsp->first_turn_hero_id_arr[1]);
		flat_data.first_turn_hero_id_arr[2] = ntohl(rsp->first_turn_hero_id_arr[2]);
		flat_data.first_turn_hero_id_arr[3] = ntohl(rsp->first_turn_hero_id_arr[3]);

		flat_data.second_turn_hero_id_arr[0] = ntohl(rsp->second_turn_hero_id_arr[0]);
		flat_data.second_turn_hero_id_arr[1] = ntohl(rsp->second_turn_hero_id_arr[1]);

#if (DATAPOOL_LOG == 1)
		CCLog("getted_baoxiang_count = %d,battle_time_remainder = %d",getted_count,flat_data.battle_time_remainder);
		CCLog("first_turn_hero_id_arr --> {%d <--vs--> %d},{%d <--vs--> %d}",
			flat_data.first_turn_hero_id_arr[0],
			flat_data.first_turn_hero_id_arr[1],
			flat_data.first_turn_hero_id_arr[2],
			flat_data.first_turn_hero_id_arr[3]
			);

		CCLog("second_turn_hero_id_arr --> {%d <--vs--> %d}",
				flat_data.second_turn_hero_id_arr[0],
				flat_data.second_turn_hero_id_arr[1]
			);
#endif
		// 宝箱
		for (int i=0;i<9;i++)
		{
			GiftJifen oneDta = rsp->gift_jifen_arr[i];
			//hton_GiftData(oneDta);
			oneDta.jifen = htonl(oneDta.jifen);
			oneDta.gift_id = htonl(oneDta.gift_id);

			if (getted_count > 0)
			{
				flat_data.gettedData.push_back(oneDta);
				getted_count--;

#if (DATAPOOL_LOG == 1)
				/*if (oneDta.type == GiftType_Item)
				{
				CCLog("getted_baoxiang_[%d] = item{type = %d,count = %d}",
				i,oneDta.data_union.item.type,oneDta.data_union.item.count);
				}
				else if (oneDta.type == GiftType_Zhuangbei)
				{
				CCLog("getted_baoxiang_[%d] = zhuangbei{******}",i);
				}
				else if (oneDta.type == GiftType_Jifen)
				{
				CCLog("getted_baoxiang_[%d] = jifen{count = %u}",i,oneDta.data_union.jifen.jifen);
				}
				else
				{
				CCAssert(false,"错误");
				}*/
				CCLog("getted_baoxiang_[%d] = jifen{id = %u,count = %u}",i,oneDta.gift_id,oneDta.jifen);
#endif
			}
			else
			{
				flat_data.missedData.push_back(oneDta);
			}
		}

		//// 移位
		//int shift_GiftData_num = MAX((total_count - 1),0);
		//rsp = (SPCmd_ArenaGetBattleScriptGiftRsp*) ((char*)rsp) + shift_GiftData_num * sizeof(GiftData);

		// 剧本
		int script_num = ntohl(rsp->fightNum);

		FightScriptBuilder builder;
		flat_data.scriptVec = builder.CharArr_2_FightScriptVec(rsp->fightScript_array,script_num);
	}

	// 是否之前成功进入了
	if (jingJiChang_EnterSuccess)
	{
		// 发送消息
		CCNotificationCenter::sharedNotificationCenter()->postNotification(
			MSG_arenaGetBattleScriptAndGiftRsp_F,
			Flat_MSG_Rsp::createMsg(&flat_data));
	}
	else
	{
		CCLog("Waring : dealWithArenaGetBattleScriptAndGiftRsp --> jingJiChang_EnterSuccess == false,so network will throw away this package!");
	}
}
void CmdHelper::dealWithArenaGetGiftRsp(char* head,char* body)
{
	SPCmd_ArenaGetGiftRsp* rsp = (SPCmd_ArenaGetGiftRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithArenaGetGiftRsp---> erroCode = %d",rsp->err_code);
#endif

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_arenaGetGiftRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithTeamBossEnterRsp(char* head,char* body)
{
	// 重新监视，因为还要等待Update或者脚本的到来
	TimeoutMonitor::getMonitor()->startMonitor(MAX_TIME_WAIT_SERVER_CALL);

	SPCmd_TeamBossEnterRsp* rsp = (SPCmd_TeamBossEnterRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_TeamBossEnterRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		CMD_LOG("%s--->people already here-----below-----",__FUNCTION__);
		HeroInfo* inforArray = rsp->hero_info_arr;
		for (int i=0;i<3;i++)
		{
			hton_HeroInfo(inforArray[i]);
			if (inforArray[i].hero_id == INVALID_ID)
				continue;

			flat_data.already_waited.push_back(inforArray[i]);
			CMD_LOG("\n--------------------HeroInfo[%d]---------------------------------",i);
			log(inforArray[i]);
			CMD_LOG("--------------------HeroInfo[%d]---------------------------------",i);
		}
		CMD_LOG("------------------------------------------------------");

		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,rsp->zhuangbei_count,flat_data.allEquips);
#if DATAPOOL_LOG == 1
		for (int i=0;i<rsp->zhuangbei_count;i++)
		{
			log(flat_data.allEquips[i]);
		}
#endif
	}


	tuanDuiBoss_EnterSend = false;
	tuanDuiBoss_EnterSuccess = rsp->err_code == Success;

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_teamBossEnterRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithTeamBossUpdateRoomRsp(char* head,char* body)
{
	// 重新监视，因为还要等待Update或者脚本的到来
	TimeoutMonitor::getMonitor()->startMonitor(MAX_TIME_WAIT_SERVER_CALL);

	SPCmd_TeamBossUpdateRoomRsp* rsp = (SPCmd_TeamBossUpdateRoomRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_TeamBossUpdateRoomRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		hton_HeroInfo(rsp->new_hero_info);
		flat_data.new_hero_info = rsp->new_hero_info;
		log(rsp->new_hero_info);

		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,rsp->zhuangbei_count,flat_data.equipsOnHero);

#if DATAPOOL_LOG == 1
		for (int equipeIndex = 0;equipeIndex<rsp->zhuangbei_count;equipeIndex++)
		{
			log(flat_data.equipsOnHero[equipeIndex]);
		}
#endif

	}


	// 是否之前成功进入了
	if (tuanDuiBoss_EnterSuccess)
	{
		// 发送消息
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_teamBossUpdateRoomRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
	}
	else
	{
		CCLog("Waring : %s --> tuanDuiBoss_EnterSuccess == false,so network will throw away this package!",__FUNCTION__);
	}
}
void CmdHelper::dealWithTeamBossGetBattleScriptAndGiftRsp(char* head,char* body)
{
	//// 这里就不用取消的，因为收到数据会自动取消的
	//TimeoutMonitor::getMonitor()->cancleMonitor();

	SPCmd_TeamBossGetBattleScriptGiftRsp* rsp = (SPCmd_TeamBossGetBattleScriptGiftRsp*)body;

	Flat_TeamBossGetBattleScriptGiftRsp flat_data;
	flat_data.err_code = ntohl(rsp->err_code);

#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithTeamBossGetBattleScriptAndGiftRsp---> erroCode = %d",rsp->err_code);
#endif
	if (rsp->err_code == Success)
	{
		//int total_count = rsp->gift_total_count;
		int getted_count = rsp->gift_available_count;	// 可以开宝箱的数目

		for (int i=0;i<4;i++)
		{
			flat_data.score_arr[i].boss_hp = ntohl(rsp->score_arr[i].boss_hp);
			flat_data.score_arr[i].hero_id = ntohl(rsp->score_arr[i].hero_id);
			// order不用转
			//flat_data.score_arr[i].order = ntohl(rsp->score_arr[i].order);
			flat_data.score_arr[i].order = rsp->score_arr[i].order;
		}
		
		// 有得到宝箱，再解析
		// 服务器在没有得到宝箱的时候，那么这里的所有数据都是乱的，所以不能解析
		if (getted_count > 0)
		{
			// 宝箱
			for (int i=0;i<9;i++)
			{
				GiftData oneDta = rsp->gift_data_arr[i];
				hton_GiftData(oneDta);

				if (getted_count > 0)
				{
					flat_data.gettedData.push_back(oneDta);
					getted_count--;
				}
				else
				{
					flat_data.missedData.push_back(oneDta);
				}
			}
		}

		// 剧本
		FightScriptBuilder builder;
		flat_data.fightScript = builder.CharArr_2_FightScriptVec(rsp->fightScript,1)[0];

	}


	// 是否之前成功进入了
	if (tuanDuiBoss_EnterSuccess) 
	{
		// 发送消息
		CCNotificationCenter::sharedNotificationCenter()->postNotification(
			MSG_teamBossGetBattleScriptAndGiftRsp_F,
			Flat_MSG_Rsp::createMsg(&flat_data));
	}
	else
	{
		CCLog("Waring : dealWithTeamBossGetBattleScriptAndGiftRsp --> tuanDuiBoss_EnterSuccess == false,so network will throw away this package!");
	}
}
void CmdHelper::dealWithTeamBossGetGiftRsp(char* head,char* body)
{
	SPCmd_TeamBossGetGiftRsp* rsp = (SPCmd_TeamBossGetGiftRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithTeamBossGetGiftRsp---> erroCode = %d",rsp->err_code);
#endif

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_teamBossGetGiftRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithCityGuardEnterRsp(char* head,char* body)
{
	SPCmd_CityGuardEnterRsp* rsp = (SPCmd_CityGuardEnterRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithCityGuardEnterRsp---> erroCode = %d",rsp->err_code);
#endif

	if (rsp->err_code == Success)
	{
		rsp->sucess_turn = ntohl(rsp->sucess_turn);
		rsp->current_silver = ntohl(rsp->current_silver);
		rsp->last_total_turns = ntohl(rsp->last_total_turns);
		rsp->last_silver = ntohl(rsp->last_silver);

#if (DATAPOOL_LOG == 1)
		CCLog("[ sucess_turn = %u,current_silver = %u,last_total_turns = %u,last_silver = %u ]",
			rsp->sucess_turn,
			rsp->current_silver,
			rsp->last_total_turns,
			rsp->last_silver
			);
#endif
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_cityGuardEnterRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithCityGuardGetNextMonsterRsp(char* head,char* body)
{
	SPCmd_CityGuardGetNextMonsterRsp* rsp = (SPCmd_CityGuardGetNextMonsterRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithCityGuardGetNextMonsterRsp---> erroCode = %d",rsp->err_code);
#endif

	if (rsp->err_code == Success)
	{
		rsp->monster_leader_profession = ntohl(rsp->monster_leader_profession);
		rsp->silver = ntohl(rsp->silver);

#if (DATAPOOL_LOG == 1)
		CCLog("[ monster_leader_profession = %u,silver = %u ]",
			rsp->monster_leader_profession,
			rsp->silver
			);
#endif
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_cityGuardGetNextMonsterRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithCityGuardFightRsp(char* head,char* body)
{
	SPCmd_CityGuardFightRsp* rsp = (SPCmd_CityGuardFightRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_CityGuardFightRsp flat_data;
	flat_data.err_code = rsp->err_code;

#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithCityGuardFightRsp---> erroCode = %d",rsp->err_code);
#endif

	if (rsp->err_code == Success)
	{
		flat_data.isFighting = rsp->isFighting;
		flat_data.silver = ntohl(rsp->silver);

		if (rsp->isFighting == true)
		{
			FightScriptBuilder builder;
			flat_data.fightScript = builder.CharArr_2_FightScriptVec(rsp->fightScript,1)[0];

#if (DATAPOOL_LOG == 1)
			CCLog("[ isFighting = %s,silver = %u ,fight reault = %s]",
				flat_data.isFighting?"true":"false",
				flat_data.silver,
				flat_data.fightScript.endResult.myResult == Result_Win?"win":"failed"
				);
#endif
		}
		else
		{
#if (DATAPOOL_LOG == 1)
			CCLog("[ isFighting = %s,silver = %u ]",
				flat_data.isFighting?"true":"false",
				flat_data.silver
				);
#endif
		}
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_cityGuardFightRsp_F,MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithQueryAnotherTeamInfoRsp(char* head,char* body)
{
	SPCmd_QueryAnotherTeamInfoRsp* rsp = (SPCmd_QueryAnotherTeamInfoRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_QueryAnotherTeamInfoRsp flat_data;
	flat_data.err_code = rsp->err_code;

#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithQueryAnotherTeamInfoRsp---> erroCode = %d",rsp->err_code);
#endif

	if (rsp->err_code == Success)
	{
		for (int i=0;i<4;i++)
		{
			hton_HeroDetail(rsp->hero_arr[i]);
			flat_data.hero_arr[i] = rsp->hero_arr[i];
			//skill
			flat_data.skill_arr[i].skill_id = ntohl(rsp->skill_arr[i].skill_id);
			flat_data.skill_arr[i].skill_type = ntohl(rsp->skill_arr[i].skill_type);
			flat_data.skill_arr[i].skill_level = ntohl(rsp->skill_arr[i].skill_level);
			flat_data.skill_arr[i].owner_hero_id = ntohl(rsp->skill_arr[i].owner_hero_id);
		}
		int zhuangbeiNum = ntohl(rsp->zhuangbei_count);

		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,zhuangbeiNum,flat_data.zhuangbei_vec);

#if (DATAPOOL_LOG == 1)
		CCLog("EquipNum = %d",zhuangbeiNum);
		for (int i=0;i<zhuangbeiNum;i++)
		{
			CCLog("\n--------------------ZhuangbeiDetail[%d]---------------------------------",i);
			log(flat_data.zhuangbei_vec[i]);
			CCLog("--------------------ZhuangbeiDetail[%d]---------------------------------",i);
		}
#endif

	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryAnotherTeamInfoRsp_F,MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithTopHeroGetListRsp(char* head,char* body)
{
	SPCmd_TopHeroGetListRsp* rsp = (SPCmd_TopHeroGetListRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_TopHeroGetListRsp flat_data;
	flat_data.err_code = rsp->err_code;
#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithTopHeroGetListRsp---> erroCode = %d",rsp->err_code);
#endif

	if (rsp->err_code == Success)
	{
		rsp->my_order = ntohl(rsp->my_order);
		rsp->next_chlg_time = ntohl(rsp->next_chlg_time);
		flat_data.my_order = rsp->my_order;
		flat_data.next_chlg_time = rsp->next_chlg_time;
		int surrounding_cnt = ntohl(rsp->hero_count);
		int report_cnt = ntohl(rsp->report_count);

#if (DATAPOOL_LOG == 1)
		CCLog("my_Order = %d,surrounding_cnt = %d,report_cnt = %d,next_chlg_time = %u,currentTime = %u",
			rsp->my_order,surrounding_cnt,report_cnt,flat_data.next_chlg_time,ServerTime::getTime());
#endif

		for (int i=0;i<10;i++)
		{
			hton_HeroInfo(rsp->top10_hero[i].hero_info);
			rsp->top10_hero[i].order = ntohl(rsp->top10_hero[i].order);
			rsp->top10_hero[i].zhandouli = ntohl(rsp->top10_hero[i].zhandouli);

			// 无效的hero
			if (rsp->top10_hero[i].hero_info.hero_id == 0)
			{
				continue;
			}
			flat_data.top30_hero.push_back(rsp->top10_hero[i]);

#if (DATAPOOL_LOG == 1)
			CCLog("\n--------------------Top[%d]---------------------------------",i);
			log(flat_data.top30_hero[i].hero_info);
			CMD_LOG("order = %d,zhandouli = %d",flat_data.top30_hero[i].order,flat_data.top30_hero[i].zhandouli);
			CCLog("--------------------Top[%d]---------------------------------",i);
#endif
		}

		HeroAndOrder* surrounding = (HeroAndOrder*)rsp->data;
		for (int i=0;i<surrounding_cnt;i++)
		{
			hton_HeroInfo(surrounding[i].hero_info);
			surrounding[i].order = ntohl(surrounding[i].order);
			surrounding[i].zhandouli = ntohl(surrounding[i].zhandouli);

			flat_data.hero_list_surrounding.push_back(surrounding[i]);

#if (DATAPOOL_LOG == 1)
			CCLog("\n--------------------surrounding[%d],order = %u---------------------------------",i,flat_data.hero_list_surrounding[i].order);
			log(flat_data.hero_list_surrounding[i].hero_info);
			CMD_LOG("order = %d,zhandouli = %d",flat_data.hero_list_surrounding[i].order,flat_data.hero_list_surrounding[i].zhandouli);
			CCLog("--------------------surrounding[%d] end---------------------------------",i);
#endif
		}

		TopHeroReport* reports = (TopHeroReport*)(surrounding + surrounding_cnt);

		for (int i=0;i<report_cnt;i++)
		{
			hton_HeroInfo(reports[i].challenger.hero_info);
			hton_HeroInfo(reports[i].be_challenger.hero_info);
			reports[i].challenger.order = ntohl(reports[i].challenger.order);
			reports[i].challenger.zhandouli = ntohl(reports[i].challenger.zhandouli);
			reports[i].be_challenger.order = ntohl(reports[i].be_challenger.order);
			reports[i].be_challenger.zhandouli = ntohl(reports[i].be_challenger.zhandouli);

			reports[i].fight_time = ntohl(reports[i].fight_time);
			reports[i].challenger_win = reports[i].challenger_win; // 不用转
			reports[i].other_order = ntohl(reports[i].other_order);


			flat_data.reports.push_back(reports[i]);

#if (DATAPOOL_LOG == 1)
			CCLog("\n--------------------reports[%d]---------------------------------",i);
			CCLog("challenger:");
			log(flat_data.reports[i].challenger.hero_info);
			CCLog("be-challenger:");
			log(flat_data.reports[i].be_challenger.hero_info);
			CCLog("challenger_win:%s,fight_time = %u",flat_data.reports[i].challenger_win?"true":"false",flat_data.reports[i].fight_time);
			CCLog("--------------------reports[%d] end---------------------------------",i);
#endif
		}

	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_topHeroGetListRsp_F,MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithTopHeroChallengeRsp(char* head,char* body)
{
	SPCmd_TopHeroChallengeRsp* rsp = (SPCmd_TopHeroChallengeRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_TopHeroChallengeRsp flat_data;
	flat_data.err_code = rsp->err_code;
#if (DATAPOOL_LOG == 1)
	CCLog("DataPool::dealWithTopHeroChallengeRsp---> erroCode = %d",rsp->err_code);
#endif

	if (rsp->err_code == Success)
	{
		rsp->order_after_fight = ntohl(rsp->order_after_fight);
		rsp->next_chlg_time = ntohl(rsp->next_chlg_time);
#if (DATAPOOL_LOG == 1)
		CCLog("order_after_fight = %d, next challenge time = %u",rsp->order_after_fight, rsp->next_chlg_time);
#endif
		flat_data.order_after_fight = rsp->order_after_fight;
		flat_data.nextChallengeTime = rsp->next_chlg_time;
		char* fightData = rsp->fightScript;

		FightScriptBuilder builder;
		flat_data.fightScript = builder.CharArr_2_FightScriptVec(fightData,1)[0];
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_topHeroChallengeRsp_F,MSG_Rsp::createMsg(&flat_data));
}

void CmdHelper::dealWithTopHeroClearCDTimeRsp(char* head,char* body)
{
	SPCmd_TopHeroClearCDTimeRsp* rsp = (SPCmd_TopHeroClearCDTimeRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->gold_balance = ntohl(rsp->gold_balance);
		rsp->next_chlg_time = ntohl(rsp->next_chlg_time);

		CMD_LOG("gold_balance = %d,next_chlg_time = %u,curentTime = %u",
			rsp->gold_balance,rsp->next_chlg_time,ServerTime::getTime());
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_TopHeroClearCDTimeRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithQuitCityGuardRsp(char* head,char* body)
{
	SPCmd_QuitCityGuardRsp* rsp = (SPCmd_QuitCityGuardRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_quitCityGuardRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithSystemGiftQueryRsp(char* head,char* body)
{
	SPCmd_SystemGiftQueryRsp* rsp = (SPCmd_SystemGiftQueryRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_SystemGiftQueryRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("DataPool::dealWithSystemGiftQueryRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->gift_count = ntohl(rsp->gift_count);
		
		CMD_LOG("getted gift_count = %u",rsp->gift_count);

		for (int i=0;i<rsp->gift_count;i++)
		{
			rsp->gift_arr[i].gift_id = ntohl(rsp->gift_arr[i].gift_id);
			rsp->gift_arr[i].is_actived = rsp->gift_arr[i].is_actived;
			rsp->gift_arr[i].hero_id = ntohl(rsp->gift_arr[i].hero_id);
			rsp->gift_arr[i].item_id = ntohl(rsp->gift_arr[i].item_id);
			rsp->gift_arr[i].item_count = ntohl(rsp->gift_arr[i].item_count);
			
			flat_data.gift_arr.push_back(rsp->gift_arr[i]);

			CMD_LOG("----------------------gift[%d]----------------------------",i);
			CMD_LOG("gift_id = %u,is_actived = %s,hero_id = %u,itemtype = %s,item_count = %d",
				rsp->gift_arr[i].gift_id,
				rsp->gift_arr[i].is_actived?"true":"false",
				rsp->gift_arr[i].hero_id,
				CS::getItemName(rsp->gift_arr[i].item_id).c_str(),
				rsp->gift_arr[i].item_count
				);
			CMD_LOG("----------------------gift[%d]----------------------------",i);
		}

	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_systemGiftQueryRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithSystemGiftGetGiftRsp(char* head,char* body)
{
	SPCmd_SystemGiftGetGiftRsp* rsp = (SPCmd_SystemGiftGetGiftRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("DataPool::dealWithSystemGiftGetGiftRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->item_id = ntohl(rsp->item_id);
		rsp->item_count = ntohl(rsp->item_count);

		CMD_LOG("getted item[%s],gift_count = %u",CS::getItemName(rsp->item_id).c_str(),rsp->item_count);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_systemGiftGetGiftRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithchargeVitalityRsp(char* head,char* body)
{
	SPCmd_ChargeVitalityRsp* rsp = (SPCmd_ChargeVitalityRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("DataPool::dealWithchargeVitalityRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->gold_balance = ntohl(rsp->gold_balance);
		CMD_LOG("gold_balance %u",rsp->gold_balance);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_chargeVitalityRsp,MSG_Rsp::createMsg(rsp));

}

void CmdHelper::dealWithDuanzaoListQueryRsp(char* head,char* body)
{
	SPCmd_DuanzaoListQueryRsp* rsp = (SPCmd_DuanzaoListQueryRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("DataPool::dealWithDuanzaoListQueryRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->group_count = ntohl(rsp->group_count);
		CMD_LOG("group_count %u",rsp->group_count);
		CMD_LOG("----------------------------Group-------------------------");
		for (int i = 0; i< rsp->group_count;i++)
		{
			rsp->group_arr[i].duanzao_group_id = ntohl(rsp->group_arr[i].duanzao_group_id);
			rsp->group_arr[i].open_level = ntohl(rsp->group_arr[i].open_level);
			rsp->group_arr[i].item_id = ntohl(rsp->group_arr[i].item_id);
			rsp->group_arr[i].item_count = ntohl(rsp->group_arr[i].item_count);
			rsp->group_arr[i].gold_need = ntohl(rsp->group_arr[i].gold_need);

			CMD_LOG("group[%d] --> {id = %u , open_level = %u,name = %s, gold_need = %u}",
				i,rsp->group_arr[i].duanzao_group_id,rsp->group_arr[i].open_level,rsp->group_arr[i].group_name,rsp->group_arr[i].gold_need);
			if (rsp->group_arr[i].item_id > 0)
			{
				CMD_LOG("       -->{need item_type = %d,count = %d}",rsp->group_arr[i].item_id,rsp->group_arr[i].item_count);
			}
		}
		CMD_LOG("----------------------------Group-------------------------");
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_DuanzaoListQueryRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithDuanzaoRsp(char* head,char* body)
{
	SPCmd_DuanzaoRsp* rsp = (SPCmd_DuanzaoRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_DuanzaoRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("DataPool::dealWithDuanzaoRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CMD_LOG("zhuangbei_count %u",rsp->zhuangbei_count);

		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,rsp->zhuangbei_count,flat_data.equips);

		#if (DATAPOOL_LOG == 1)
		CMD_LOG("----------------------------Zhuangbei-------------------------");
		for (int i=0;i<rsp->zhuangbei_count;i++)
		{
			CCLog("\n--------------------ZhuangbeiDetail[%d]---------------------------------",i);
			log(flat_data.equips[i]);
			CCLog("--------------------ZhuangbeiDetail[%d]---------------------------------",i);
		}
		#endif
		
		CMD_LOG("----------------------------Zhuangbei-------------------------");
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_DuanzaoRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}

void CmdHelper::dealWithDuanzaoGroupZBQueryRsp(char* head,char* body)
{
	SPCmd_DuanzaoGroupZBQueryRsp* rsp = (SPCmd_DuanzaoGroupZBQueryRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_DuanzaoGroupZBQueryRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("CmdHelper::dealWithDuanzaoGroupZBQueryRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->zb_count = ntohl(rsp->zb_count);
		CMD_LOG("zb_count %u",rsp->zb_count);
		CMD_LOG("----------------------------Zhuangbei-------------------------");
		ntoh_DuanzaoGroupZBInfoArrayToFlat_DuanzaoGroupZBInfoVec(
			(DuanzaoGroupZBInfo*)rsp->zb_arr,
			rsp->zb_count,
			flat_data.zbArray);
		CMD_LOG("----------------------------Zhuangbei-------------------------");
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_DuanzaoGroupZBQueryRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}


void CmdHelper::dealWithQueryAllSuitNameRsp(char* head,char* body)
{
	SPCmd_QueryAllSuitNameRsp* rsp = (SPCmd_QueryAllSuitNameRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("DataPool::dealWithQueryAllSuitNameRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->suit_count = ntohl(rsp->suit_count);
		CMD_LOG("suit_count %d",rsp->suit_count);
		CMD_LOG("----------------------------SuitNameInfo-------------------------\n");

		 EquipmentConfigInfoManager *equipConfigManager = EquipmentConfigInfoManager::getManager();

		SuitNameInfo* inforArray = (SuitNameInfo*)rsp->data;
		for (int i=0;i<rsp->suit_count;i++)
		{
			CMD_LOG("---------------------------SuitNameInfo[%d]-------------------------------",i);
			inforArray[i].suit_id = htonl(inforArray[i].suit_id);
			CMD_LOG("suit_id = %d,suit_name = %s",inforArray[i].suit_id,inforArray[i].suit_name);

			CMD_LOG("-----suit_part--------");
			inforArray[i].group_id_1 = htonl(inforArray[i].group_id_1);
			inforArray[i].zhuangbei_type_1 = htonl(inforArray[i].zhuangbei_type_1);
			if (inforArray[i].group_id_1 > 0)
				strcpy(inforArray[i].zhuangbei_name_1, equipConfigManager->getEquipName(inforArray[i].group_id_1).c_str());
			if(inforArray[i].group_id_1 > 0)
				CMD_LOG("group_id_1 = %d zhuangbei_type_1 = %d,zhuangbei_name = %s",inforArray[i].group_id_1,inforArray[i].zhuangbei_type_1,inforArray[i].zhuangbei_name_1);

			inforArray[i].group_id_2 = htonl(inforArray[i].group_id_2);
			inforArray[i].zhuangbei_type_2 = htonl(inforArray[i].zhuangbei_type_2);
			if (inforArray[i].group_id_2 > 0)
				strcpy(inforArray[i].zhuangbei_name_2, equipConfigManager->getEquipName(inforArray[i].group_id_2).c_str());
			if(inforArray[i].group_id_2 > 0)
				CMD_LOG("group_id_2 = %d zhuangbei_type_2 = %d,zhuangbei_name = %s",inforArray[i].group_id_2,inforArray[i].zhuangbei_type_2,inforArray[i].zhuangbei_name_2);

			inforArray[i].group_id_3 = htonl(inforArray[i].group_id_3);
			inforArray[i].zhuangbei_type_3 = htonl(inforArray[i].zhuangbei_type_3);
			if (inforArray[i].group_id_3 > 0)
				strcpy(inforArray[i].zhuangbei_name_3, equipConfigManager->getEquipName(inforArray[i].group_id_3).c_str());
			if(inforArray[i].group_id_3 > 0)
				CMD_LOG("group_id_3 = %d zhuangbei_type_3 = %d,zhuangbei_name = %s",inforArray[i].group_id_3,inforArray[i].zhuangbei_type_3,inforArray[i].zhuangbei_name_3);

			inforArray[i].group_id_4 = htonl(inforArray[i].group_id_4);
			inforArray[i].zhuangbei_type_4 = htonl(inforArray[i].zhuangbei_type_4);
			if (inforArray[i].group_id_4 > 0)
				strcpy(inforArray[i].zhuangbei_name_4, equipConfigManager->getEquipName(inforArray[i].group_id_4).c_str());
			if(inforArray[i].group_id_4 > 0)
				CMD_LOG("group_id_4 = %d zhuangbei_type_4 = %d,zhuangbei_name = %s",inforArray[i].group_id_4,inforArray[i].zhuangbei_type_4,inforArray[i].zhuangbei_name_4);

			inforArray[i].group_id_5 = htonl(inforArray[i].group_id_5);
			inforArray[i].zhuangbei_type_5 = htonl(inforArray[i].zhuangbei_type_5);
			if (inforArray[i].group_id_5 > 0)
				strcpy(inforArray[i].zhuangbei_name_5, equipConfigManager->getEquipName(inforArray[i].group_id_5).c_str());
			if(inforArray[i].group_id_5 > 0)
				CMD_LOG("group_id_5 = %d zhuangbei_type_5 = %d,zhuangbei_name = %s",inforArray[i].group_id_5,inforArray[i].zhuangbei_type_5,inforArray[i].zhuangbei_name_5);
			
			inforArray[i].group_id_6 = htonl(inforArray[i].group_id_6);
			inforArray[i].zhuangbei_type_6 = htonl(inforArray[i].zhuangbei_type_6);
			if (inforArray[i].group_id_6 > 0)
				strcpy(inforArray[i].zhuangbei_name_6, equipConfigManager->getEquipName(inforArray[i].group_id_6).c_str());
			if(inforArray[i].group_id_6 > 0)
				CMD_LOG("group_id_6 = %d zhuangbei_type_6 = %d,zhuangbei_name = %s",inforArray[i].group_id_6,inforArray[i].zhuangbei_type_6,inforArray[i].zhuangbei_name_6);

			
			CMD_LOG("-----suit_part--------\n");

			CMD_LOG("-----suit_attribute--------");

			for (int j=0;j<5;j++)
			{
				for (int k=0;k<2;k++)
				{
					inforArray[i].suit_attr[j][k].attr_type = htonl(inforArray[i].suit_attr[j][k].attr_type);
					inforArray[i].suit_attr[j][k].attr_value = htonl(inforArray[i].suit_attr[j][k].attr_value);
					// 这个值对于套装来说没有用
					inforArray[i].suit_attr[j][k].cuiqu_suc_ratio = htonl(inforArray[i].suit_attr[j][k].cuiqu_suc_ratio);

					if (inforArray[i].suit_attr[j][k].attr_type > 0)
					{
						CMD_LOG("suit_attr[%d][%d] -->type = %u,value = %u,cuiqu_ratio = %u",j,k,
							inforArray[i].suit_attr[j][k].attr_type,inforArray[i].suit_attr[j][k].attr_value,inforArray[i].suit_attr[j][k].cuiqu_suc_ratio);
					}
				}
			}

			//// 调试打印
			//{
			//	CMD_LOG("===============================================");
			//	ZBAttr* _array = (ZBAttr*)inforArray[i].suit_attr;
			//	for (int i=0;i<8;i++)
			//	{
			//		CMD_LOG("type = %u,value = %u",_array[i].attr_type,_array[i].attr_value);;
			//	}
			//	CMD_LOG("===============================================");
			//}

			CMD_LOG("-----suit_attribute--------");
			CMD_LOG("---------------------------SuitNameInfo[%d]-------------------------------\n",i);
		}

		CMD_LOG("----------------------------SuitNameInfo-------------------------");
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryAllSuitNameRsp,Flat_MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithZBCuiquRsp(char* head,char* body)
{
	SPCmd_ZBCuiquRsp* rsp = (SPCmd_ZBCuiquRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("CmdHelper::dealWithZBCuiquRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{

		CMD_LOG("cuiqu success = %s",rsp->sucess?"true":"false");
		if (rsp->sucess)
		{
			rsp->jinghua.zb_type = ntohl(rsp->jinghua.zb_type);
			rsp->jinghua.attr_type = ntohl(rsp->jinghua.attr_type);
			rsp->jinghua.level = ntohl(rsp->jinghua.level);
			rsp->jinghua.count = ntohl(rsp->jinghua.count);
			CMD_LOG("jinghua.zb_type = %d,jinghua.attr_type = %d,jinghua.level = %d,jinghua.count = %d",
				rsp->jinghua.zb_type,rsp->jinghua.attr_type,rsp->jinghua.level,rsp->jinghua.count
				);
		}
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ZBCuiquRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithZBFumoRsp(char* head,char* body)
{
	SPCmd_ZBFumoRsp* rsp = (SPCmd_ZBFumoRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("CmdHelper::dealWithZBFumoRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{

		CMD_LOG("fumo success = %s",rsp->sucess?"true":"false");
		if (rsp->sucess)
		{
			rsp->new_fumo_attr.attr_type = ntohl(rsp->new_fumo_attr.attr_type);
			rsp->new_fumo_attr.attr_value = ntohl(rsp->new_fumo_attr.attr_value);
			rsp->new_fumo_attr.cuiqu_suc_ratio = ntohl(rsp->new_fumo_attr.cuiqu_suc_ratio);
			CMD_LOG("new_fumo_attr.attr_type = %d,new_fumo_attr.attr_value = %d,new_fumo_attr.cuiqu_suc_ratio = %d",
				rsp->new_fumo_attr.attr_type,rsp->new_fumo_attr.attr_value,rsp->new_fumo_attr.cuiqu_suc_ratio
				);
		}
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ZBFumoRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithQueryAllMofaJinghuaRsp(char* head,char* body)
{
	SPCmd_QueryAllMofaJinghuaRsp* rsp = (SPCmd_QueryAllMofaJinghuaRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("CmdHelper::dealWithQueryAllMofaJinghuaRsp---> erroCode = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("count = %d",rsp->count);
		for (int i=0;i<rsp->count;i++)
		{
			rsp->jinghua_arr[i].attr_type = ntohl(rsp->jinghua_arr[i].attr_type);
			rsp->jinghua_arr[i].zb_type = ntohl(rsp->jinghua_arr[i].zb_type);
			rsp->jinghua_arr[i].level = ntohl(rsp->jinghua_arr[i].level);
			rsp->jinghua_arr[i].count = ntohl(rsp->jinghua_arr[i].count);

			CMD_LOG("jinghua[%d]:---> attr_type = %d,zb_type = %d,level = %d,count = %d",
				i,rsp->jinghua_arr[i].attr_type,rsp->jinghua_arr[i].zb_type,rsp->jinghua_arr[i].level,rsp->jinghua_arr[i].count);
		}
	}

	if (!reloadCheck(rsp->err_code,(SimpleProtocolCmd)((SPHead*)head)->cmd,MSG_QueryAllMofaJinghuaRsp,MSG_Rsp::createMsg(rsp)))
	{
		return;
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryAllMofaJinghuaRsp,MSG_Rsp::createMsg(rsp));

}

void CmdHelper::dealWithQueryFubenDiaoluoRsp(char* head,char* body)
{
	SPCmd_QueryFubenDiaoluoRsp* rsp = (SPCmd_QueryFubenDiaoluoRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("CmdHelper::dealWithQueryFubenDiaoluoRsp---> erroCode = %d",rsp->err_code);

	Flat_QueryFubenDiaoluoRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->item_count = ntohl(rsp->item_count);
		CMD_LOG("item_count = %d",rsp->item_count);

		ItemsType itemType;
		for (int itemIndex = 0;itemIndex < rsp->item_count;itemIndex++)
		{
			itemType = (ItemsType) ntohl(rsp->item_type_arr[itemIndex]);
			flat_data.items.push_back(itemType);
			CMD_LOG("item[%d] = [%d]",itemIndex,itemType);
		}

		// 做偏移
		unsigned int * zhuangbei_count = rsp->item_type_arr + rsp->item_count;
		unsigned int zb_count = ntohl(*zhuangbei_count);
		CMD_LOG("zb_count = %d",zb_count);
		FubenDiaoluoZBInfo* zbArray = (FubenDiaoluoZBInfo*)(zhuangbei_count+1);
		for (int i=0;i<zb_count;i++)
		{
			Flat_FubenDiaoluoZBInfo oneZB;
			oneZB.group_id = ntohl(zbArray->group_id);
			oneZB.is_suipian = zbArray->is_suipian;
			oneZB.zhuangbei_type = ntohl(zbArray->zhuangbei_type);
			oneZB.zhuangbei_name = zbArray->zhuangbei_name;
			oneZB.zhuangbei_colour = ntohl(zbArray->zhuangbei_colour);
			oneZB.zhuangbei_pic = zbArray->zhuangbei_pic;
			oneZB.level = ntohl(zbArray->level);
			oneZB.gongji_min_min = ntohl(zbArray->gongji_min_min);
			oneZB.gongji_min_max = ntohl(zbArray->gongji_min_max);
			oneZB.gongji_max_min = ntohl(zbArray->gongji_max_min);
			oneZB.gongji_max_max = ntohl(zbArray->gongji_max_max);
			oneZB.wufang_min = ntohl(zbArray->wufang_min);
			oneZB.wufang_max = ntohl(zbArray->wufang_max);
			oneZB.hole1_percent = ntohl(zbArray->hole1_percent);
			oneZB.hole2_percent = ntohl(zbArray->hole2_percent);
			oneZB.hole3_percent = ntohl(zbArray->hole3_percent);

			//  char attr_str[0]; //attr_str_count个连续字符串，每个字符串以'\0'结束
			int attri_length = 0;
			int attr_count = ntohl(zbArray->attr_str_count);
			char* attr_string = zbArray->attr_str;
			for (int attr_index = 0;attr_index < attr_count;attr_index++)
			{
				string oneAttr = string(attr_string);
				AttrbuteItem item(oneAttr);
				oneZB.itemVec.push_back(item);
				attr_string += oneAttr.length() + 1; // 越过"\0"指向下一个
				attri_length += oneAttr.length() + 1;
			}
			// 描述
			oneZB.desc = string(attr_string);
			int desc_len = oneZB.desc.length() + 1;

			++ zbArray;
			zbArray = (FubenDiaoluoZBInfo*) (((char*)zbArray) + attri_length + desc_len);
			// 放入flat
			flat_data.diaoluo_vec.push_back(oneZB);

			CMD_LOG("zhuangbei[%d]:---> name = %s,group_id = %d,is_suipian = %s,zhuangbei_type = %d,zhuangbei_colour = %d,level = %d",
				i,
				oneZB.zhuangbei_name.c_str(),
				oneZB.group_id,(oneZB.is_suipian?"true":"false"),
				oneZB.zhuangbei_type,oneZB.zhuangbei_colour,oneZB.level);
		}
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryFubenDiaoluoRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}

void CmdHelper::dealWithNewAssistantJoinRsp(char* head,char* body)
{
	SPCmd_NewAssistantJoinRsp* rsp = (SPCmd_NewAssistantJoinRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_NewAssistantJoinRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",Success);

	if (rsp->err_code == Success)
	{
		hton_HeroDetail(rsp->hero_detail);
		flat_data.hero_detail = rsp->hero_detail;
		log(rsp->hero_detail);
		flat_data.skillInfor.is_equiped = rsp->skill.is_equiped;
		flat_data.skillInfor.owner_hero_id = ntohl(rsp->skill.owner_hero_id);
		flat_data.skillInfor.skill_id = ntohl(rsp->skill.skill_id);
		flat_data.skillInfor.skill_type = ntohl(rsp->skill.skill_type);
		flat_data.skillInfor.skill_level = ntohl(rsp->skill.skill_level);

		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);

		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,rsp->zhuangbei_count,flat_data.equips);

#if (DATAPOOL_LOG == 1)
		for (int zb_index = 0;zb_index < rsp->zhuangbei_count;zb_index++)
		{
			CMD_LOG("\n--------------------ZhuangbeiDetail[%d]---------------------------------",zb_index);
			log(flat_data.equips[zb_index]);
			CMD_LOG("--------------------ZhuangbeiDetail[%d]---------------------------------",zb_index);
		}
#endif
		
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_NewAssistantJoinRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}

void CmdHelper::dealWithQuerySubTaskListRsp(char* head,char* body)
{
	SPCmd_QuerySubTaskListRsp* rsp = (SPCmd_QuerySubTaskListRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_QuerySubTaskListRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",Success);

	if (rsp->err_code == Success)
	{
		rsp->task_count = ntohl(rsp->task_count);

		for (int task_index = 0;task_index < rsp->task_count;task_index++)
		{

			rsp->task_arr[task_index].task_id = ntohl(rsp->task_arr[task_index].task_id);
			rsp->task_arr[task_index].task_type = ntohl(rsp->task_arr[task_index].task_type);
			rsp->task_arr[task_index].reward_item_id = ntohl(rsp->task_arr[task_index].reward_item_id);
			rsp->task_arr[task_index].reward_item_count = ntohl(rsp->task_arr[task_index].reward_item_count);

			Flat_SubTaskInfor infor;
			infor.task_id = rsp->task_arr[task_index].task_id;
			infor.task_type = (SubTaskType)rsp->task_arr[task_index].task_type;
			infor.reward_item_id = (ItemsType)rsp->task_arr[task_index].reward_item_id;
			infor.reward_item_count = rsp->task_arr[task_index].reward_item_count;
			infor.des = string(rsp->task_arr[task_index].desc);

			flat_data.taskes.push_back(infor);

			CMD_LOG("\n--------------------SubTask[%d]---------------------------------",task_index);
			CMD_LOG("task_id = %d,task_type = %d,reward_item_id = %d,reward_item_count = %d,des = %s",
				infor.task_id,infor.task_type,infor.reward_item_id,infor.reward_item_count,infor.des.c_str());
			CMD_LOG("--------------------SubTask[%d]---------------------------------",task_index);
		}
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QuerySubTaskListRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}

void CmdHelper::dealWithNewSubTaskGotRsp(char* head,char* body)
{
	CCAssert(false,"还没做");
}

void CmdHelper::dealWithSubTaskSubmitedRsp(char* head,char* body)
{
	SPCmd_SubTaskSubmitedRsp* rsp = (SPCmd_SubTaskSubmitedRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		rsp->task_id = ntohl(rsp->task_id);
		rsp->reward_item_id = ntohl(rsp->reward_item_id);
		rsp->reward_item_count = ntohl(rsp->reward_item_count);

		CMD_LOG("task_id = %d,reward_item_id = %d,reward_item_count = %d",
			rsp->task_id,rsp->reward_item_id,rsp->reward_item_count);
	}

	// 发送数据
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_subTaskSubmitedRsp,MSG_Rsp::createMsg(rsp));

}

void CmdHelper::dealWithQueryMainTaskRewardRsp(char* head,char* body)
{
	SPCmd_QueryMainTaskRewardRsp* rsp = (SPCmd_QueryMainTaskRewardRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_QueryMainTaskRewardRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",Success);

	if (rsp->err_code == Success)
	{
		rsp->task_count = ntohl(rsp->task_count);

		for (int task_index = 0;task_index < rsp->task_count;task_index++)
		{
			rsp->task_reward[task_index].task_id = ntohl(rsp->task_reward[task_index].task_id);
			rsp->task_reward[task_index].exp = ntohl(rsp->task_reward[task_index].exp);
			rsp->task_reward[task_index].silver = ntohl(rsp->task_reward[task_index].silver);

			flat_data.taskRewardVec.push_back(rsp->task_reward[task_index]);

			CMD_LOG("\n--------------------reward[%d]---------------------------------",task_index);
			CMD_LOG("task_id = %d,exp = %d,silver = %d",
				rsp->task_reward[task_index].task_id,
				rsp->task_reward[task_index].exp,
				rsp->task_reward[task_index].silver);
			CMD_LOG("--------------------reward[%d]---------------------------------",task_index);
		}
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryMainTaskRewardRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithChargeGoldRsp(char* head,char* body)
{
	SPCmd_ChargeGoldRsp* rsp = (SPCmd_ChargeGoldRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",Success);

	if (rsp->err_code == Success)
	{
		rsp->gold_balance = ntohl(rsp->gold_balance);
		rsp->vip_level = ntohl(rsp->vip_level);
		rsp->next_vip_level = ntohl(rsp->next_vip_level);
		rsp->next_need_charge = ntohl(rsp->next_need_charge);

		CMD_LOG("gold_balance = %d,vip_level = %d,next_vip_level = %d,next_need_charge = %d",
			rsp->gold_balance,rsp->vip_level,rsp->next_vip_level,rsp->next_need_charge);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ChargeGoldRsp,MSG_Rsp::createMsg(rsp));

}

void CmdHelper::dealWithQueryTeamSkillListRsp(char* head,char* body)
{
	SPCmd_QueryTeamSkillListRsp* rsp = (SPCmd_QueryTeamSkillListRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->skill_count = ntohl(rsp->skill_count);
		CMD_LOG("skill count = %d",rsp->skill_count);
		for (int i = 0; i < rsp->skill_count; i++)
		{
			rsp->skill_arr[i].skill_id = ntohl(rsp->skill_arr[i].skill_id);
			rsp->skill_arr[i].owner_hero_id = ntohl(rsp->skill_arr[i].owner_hero_id);
			rsp->skill_arr[i].skill_type = ntohl(rsp->skill_arr[i].skill_type);
			rsp->skill_arr[i].skill_level = ntohl(rsp->skill_arr[i].skill_level);

			CMD_LOG("skill[%d]--> skill_id = %d,skill_level = %d,owner_hero_id = %d,skill_type = %d,is_equip = %s",i,
				rsp->skill_arr[i].skill_id,rsp->skill_arr[i].skill_level,rsp->skill_arr[i].owner_hero_id,rsp->skill_arr[i].skill_type,rsp->skill_arr[i].is_equiped?"true":"false");
		}
	}

	if (!reloadCheck(rsp->err_code,(SimpleProtocolCmd)((SPHead*)head)->cmd,MSG_QueryTeamSkillListRsp,MSG_Rsp::createMsg(rsp)))
	{
		return;
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryTeamSkillListRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithEquipSkillRsp(char* head,char* body)
{
	SPCmd_EquipSkillRsp* rsp = (SPCmd_EquipSkillRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",Success);

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_EquipSkillRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithActorLearnSkillRsp(char* head,char* body)
{
	SPCmd_ActorLearnSkillRsp* rsp = (SPCmd_ActorLearnSkillRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("\n------------------------%s---------------------",__FUNCTION__);
	CMD_LOG("err_code = %d",Success);
	if (rsp->err_code == Success)
	{
		rsp->new_skill.owner_hero_id = ntohl(rsp->new_skill.owner_hero_id);
		rsp->new_skill.skill_id = ntohl(rsp->new_skill.skill_id);
		rsp->new_skill.skill_type = ntohl(rsp->new_skill.skill_type);
		rsp->new_skill.skill_level = ntohl(rsp->new_skill.skill_level);
		//rsp->new_skill.is_equiped = ;
		CMD_LOG("owner_hero_id = %u,skill_id = %u,skill_level = %u,skill_type = %u,is_equiped = %s",
			rsp->new_skill.owner_hero_id,rsp->new_skill.skill_id,rsp->new_skill.skill_level,rsp->new_skill.skill_type,rsp->new_skill.is_equiped?"true":"false"
			);
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ActorLearnSkillRsp,MSG_Rsp::createMsg(rsp));

}


void CmdHelper::dealWithQueryAllShenbingListRsp(char* head,char* body)
{
	SPCmd_QueryAllShenbingListRsp* rsp = (SPCmd_QueryAllShenbingListRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_QueryAllShenbingListRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->shenbing_count = ntohl(rsp->shenbing_count);
		CMD_LOG("shenbing_count %u",rsp->shenbing_count);
		CMD_LOG("----------------------------ShenBing-------------------------");
		ntoh_DuanzaoGroupZBInfoArrayToFlat_DuanzaoGroupZBInfoVec(
			(DuanzaoGroupZBInfo*)rsp->zb_arr,
			rsp->shenbing_count,
			flat_data.zbArray);
		CMD_LOG("----------------------------ShenBing-------------------------");
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryAllShenbingListRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithQueryTeamShenbingSuipianRsp(char* head,char* body)
{
	SPCmd_QueryTeamShenbingSuipianRsp* rsp = (SPCmd_QueryTeamShenbingSuipianRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->suipian_info_count = ntohl(rsp->suipian_info_count);
		CMD_LOG("suipian_info_count %u",rsp->suipian_info_count);
		CMD_LOG("----------------------------ShenBingSuiPian-------------------------");
		for (int i=0;i<rsp->suipian_info_count;i++)
		{
			rsp->suipian_into_arr[i].suipian_count = ntohl(rsp->suipian_into_arr[i].suipian_count);
			rsp->suipian_into_arr[i].suipian_id = ntohl(rsp->suipian_into_arr[i].suipian_id);
			rsp->suipian_into_arr[i].zb_group_id = ntohl(rsp->suipian_into_arr[i].zb_group_id);

			CMD_LOG("suipian_arr[%d]-->{count = %d,id = %d,group_id = %d}",i,
				rsp->suipian_into_arr[i].suipian_count,
				rsp->suipian_into_arr[i].suipian_id,
				rsp->suipian_into_arr[i].zb_group_id);
		}
		CMD_LOG("----------------------------ShenBingSuiPian-------------------------");
	}

	if (!reloadCheck(rsp->err_code,(SimpleProtocolCmd)((SPHead*)head)->cmd,MSG_QueryTeamShenbingSuipianRsp,MSG_Rsp::createMsg(rsp)))
	{
		return;
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryTeamShenbingSuipianRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithShenbingHechengRsp(char* head,char* body)
{
	SPCmd_ShenbingHechengRsp* rsp = (SPCmd_ShenbingHechengRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_ShenbingHechengRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CMD_LOG("zhuangbei_count %u",rsp->zhuangbei_count);
		CMD_LOG("----------------------------zhuangbei-------------------------");
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,rsp->zhuangbei_count,flat_data.zhuangbei_vec);
		
#if DATAPOOL_LOG == 1
		for (int i=0;i<rsp->zhuangbei_count;i++)
		{
			CMD_LOG("zhuangbei[%d]:",i);
			log(flat_data.zhuangbei_vec[i]);
		}
#endif
		CMD_LOG("----------------------------zhuangbei-------------------------");
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ShenbingHechengRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithQueryTeamItemSuipianRsp(char* head,char* body)
{
	SPCmd_QueryTeamItemSuipianRsp* rsp = (SPCmd_QueryTeamItemSuipianRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->suipian_info_count = ntohl(rsp->suipian_info_count);
		CMD_LOG("suipian_info_count %u",rsp->suipian_info_count);
		CMD_LOG("----------------------------ItemSuiPian-------------------------");
		for (int i=0;i<rsp->suipian_info_count;i++)
		{
			rsp->suipian_into_arr[i].suipian_count = ntohl(rsp->suipian_into_arr[i].suipian_count);
			rsp->suipian_into_arr[i].suipian_id = ntohl(rsp->suipian_into_arr[i].suipian_id);
			rsp->suipian_into_arr[i].item_id = ntohl(rsp->suipian_into_arr[i].item_id);

			CMD_LOG("suipian_arr[%d]-->{count = %d,id = %d,item_type = %d}",i,
				rsp->suipian_into_arr[i].suipian_count,
				rsp->suipian_into_arr[i].suipian_id,
				rsp->suipian_into_arr[i].item_id);
		}
		CMD_LOG("----------------------------ItemSuiPian-------------------------");
	}

	if (!reloadCheck(rsp->err_code,(SimpleProtocolCmd)((SPHead*)head)->cmd,MSG_QueryTeamItemSuipianRsp,MSG_Rsp::createMsg(rsp)))
	{
		return;
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryTeamItemSuipianRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithItemHechengRsp(char* head,char* body)
{
	SPCmd_ItemHechengRsp* rsp = (SPCmd_ItemHechengRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		CMD_LOG("----------------------------Item-------------------------");
		rsp->item_info.type = ntohl(rsp->item_info.type);
		rsp->item_info.count = ntohl(rsp->item_info.count);
		CMD_LOG("{type = %d,count = %d}",rsp->item_info.type,rsp->item_info.count);
		CMD_LOG("----------------------------Item-------------------------");
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ItemHechengRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithSaodangFubenRsp(char* head,char* body)
{
	SPCmd_SaodangRsp* rsp = (SPCmd_SaodangRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_SaodangRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		flat_data.exp = ntohl(rsp->exp);
		flat_data.decomposed_count = ntohl(rsp->decomposed_count);
		CMD_LOG("exp = %u, decomposed_count = %u",flat_data.exp, flat_data.decomposed_count);

		rsp->item_count = ntohl(rsp->item_count);
		CMD_LOG("item_count = %u",rsp->item_count);
		for (int i=0;i<rsp->item_count;i++)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			flat_data.items.push_back(rsp->item_arr[i]);
			CMD_LOG("item_arr[%d]-->{type = %d,count = %d}",i,rsp->item_arr[i].type,rsp->item_arr[i].count);
		}
		char* next = (char*)(rsp->item_arr + rsp->item_count); //指向item_suipian_count(没有碎片了)

/*		unsigned int item_suipian_count = *((unsigned int*)next);
		item_suipian_count = ntohl(item_suipian_count);
		next += sizeof(unsigned int); // 指向item_suipian_arr
		CMD_LOG("item_suipian_count = %u",item_suipian_count);

		ItemSuipianInfo* item_suipian_arr = (ItemSuipianInfo*)next;
		for (int i=0;i<item_suipian_count;i++)
		{
			item_suipian_arr[i].item_id = ntohl(item_suipian_arr[i].item_id);
			item_suipian_arr[i].suipian_id = ntohl(item_suipian_arr[i].suipian_id);
			item_suipian_arr[i].suipian_count = ntohl(item_suipian_arr[i].suipian_count);
			flat_data.item_suipians.push_back(item_suipian_arr[i]);

			CMD_LOG("item_suipian_arr[%d]-->{item_type = %d,suipian_id = %d,suipian_count = %d}",
				item_suipian_arr[i].item_id,
				item_suipian_arr[i].suipian_id,
				item_suipian_arr[i].suipian_count
				);
		}
		item_suipian_arr = item_suipian_arr + item_suipian_count;
		next = (char*)item_suipian_arr; // 指向zb_count*/

		unsigned int zhuanbei_count = *((unsigned int*)next);
		zhuanbei_count = ntohl(zhuanbei_count);
		next += sizeof(unsigned int); // 指向zb_detail_arr
		CMD_LOG("zhuanbei_count = %u",zhuanbei_count);
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)next,zhuanbei_count,flat_data.equips);

#if DATAPOOL_LOG == 1
		for (int i=0;i<zhuanbei_count;i++)
		{
			CMD_LOG("zhuangbei[%d]",i);
			log(flat_data.equips[i]);
		}
#endif
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_saodangFubenRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithHeroLevelUpRsp(char* head,char* body)
{
	SPCmd_HeroLevelUpRsp* rsp = (SPCmd_HeroLevelUpRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->silver_end = ntohl(rsp->silver_end);
		rsp->item_end.type = ntohl(rsp->item_end.type);
		rsp->item_end.count = ntohl(rsp->item_end.count);
		ntoh_hero_detail(rsp->hero_detial,rsp->hero_detial);

		CMD_LOG("silver_end = %u,item_end-->{type = %d,count = %d}",
			rsp->silver_end,
			rsp->item_end.type,
			rsp->item_end.count
			);
		log(rsp->hero_detial);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_heroLevelUpRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithHeroUpgradeRsp(char* head,char* body)
{
	SPCmd_HeroUpgradeRsp* rsp = (SPCmd_HeroUpgradeRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->silver_end = ntohl(rsp->silver_end);
		rsp->item_end.type = ntohl(rsp->item_end.type);
		rsp->item_end.count = ntohl(rsp->item_end.count);
		ntoh_hero_detail(rsp->hero_detial,rsp->hero_detial);

		CMD_LOG("silver_end = %u,item_end-->{type = %d,count = %d}",
			rsp->silver_end,
			rsp->item_end.type,
			rsp->item_end.count
			);
		log(rsp->hero_detial);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_heroUpgradeRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithShenbingJieFengyinRsp(char* head,char* body)
{
	SPCmd_ShenbingJieFengyinRsp* rsp = (SPCmd_ShenbingJieFengyinRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_shenbingJieFengyinRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithShenbingJiandingRsp(char* head,char* body)
{
	SPCmd_ShenbingJiandingRsp* rsp = (SPCmd_ShenbingJiandingRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->identified_attr_count = ntohl(rsp->identified_attr_count);
		CMD_LOG("identified_attr_count = %d",rsp->identified_attr_count);
		for (int i=0;i<rsp->identified_attr_count;i++)
		{
			rsp->identified_attr_vec[i].attr_type = ntohl(rsp->identified_attr_vec[i].attr_type);
			rsp->identified_attr_vec[i].attr_value = ntohl(rsp->identified_attr_vec[i].attr_value);
			rsp->identified_attr_vec[i].cuiqu_suc_ratio = ntohl(rsp->identified_attr_vec[i].cuiqu_suc_ratio);

			CMD_LOG("identified_attr_vec[%d] ---> {attr_type = %d,attr_value = %d,cuiqu_suc_ratio = %d}",
				i,rsp->identified_attr_vec[i].attr_type,rsp->identified_attr_vec[i].attr_value,rsp->identified_attr_vec[i].cuiqu_suc_ratio);
		}
		
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_shenbingJiandingRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithZBJinglianRsp(char* head,char* body)
{
	SPCmd_ZBJinglianRsp* rsp = (SPCmd_ZBJinglianRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->jinglian_data.jinglian_times = ntohl(rsp->jinglian_data.jinglian_times);
		CMD_LOG("jinglian_data.jinglian_times = %d",rsp->jinglian_data.jinglian_times);
		for (int i=0;i<sizeof(rsp->jinglian_data.jinglian_attr_arr)/sizeof(ZBAttr);i++)
		{
			rsp->jinglian_data.jinglian_attr_arr[i].attr_type = ntohl(rsp->jinglian_data.jinglian_attr_arr[i].attr_type);
			rsp->jinglian_data.jinglian_attr_arr[i].attr_value = ntohl(rsp->jinglian_data.jinglian_attr_arr[i].attr_value);
			rsp->jinglian_data.jinglian_attr_arr[i].cuiqu_suc_ratio = ntohl(rsp->jinglian_data.jinglian_attr_arr[i].cuiqu_suc_ratio);

			CMD_LOG("jinglian_attr_arr[%d] --> {attr_type = %d,attr_value = %d,cuiqu_suc_ratio = %d}",
				i,
				rsp->jinglian_data.jinglian_attr_arr[i].attr_type,
				rsp->jinglian_data.jinglian_attr_arr[i].attr_value,
				rsp->jinglian_data.jinglian_attr_arr[i].cuiqu_suc_ratio
				);
		}

		/*for (int i=0;i<sizeof(rsp->jinglian_data.next_jinglian_attr_arr)/sizeof(ZBAttr);i++)
		{
			rsp->jinglian_data.next_jinglian_attr_arr[i].attr_type = ntohl(rsp->jinglian_data.next_jinglian_attr_arr[i].attr_type);
			rsp->jinglian_data.next_jinglian_attr_arr[i].attr_value = ntohl(rsp->jinglian_data.next_jinglian_attr_arr[i].attr_value);
			rsp->jinglian_data.next_jinglian_attr_arr[i].cuiqu_suc_ratio = ntohl(rsp->jinglian_data.next_jinglian_attr_arr[i].cuiqu_suc_ratio);

			CMD_LOG("next_jinglian_attr_arr[%d] --> {attr_type = %d,attr_value = %d,cuiqu_suc_ratio = %d}",
				i,
				rsp->jinglian_data.next_jinglian_attr_arr[i].attr_type,
				rsp->jinglian_data.next_jinglian_attr_arr[i].attr_value,
				rsp->jinglian_data.next_jinglian_attr_arr[i].cuiqu_suc_ratio
				);
		}*/


		rsp->count = ntohl(rsp->count);
		CMD_LOG("item count give back = %d",rsp->count);
		for (int i=0;i<rsp->count;i++)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);

			CMD_LOG("item[%d] --> {type = %d,count = %d}",rsp->item_arr[i].type,rsp->item_arr[i].count);
		}
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ZBJinglianRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithShenbingZhuankeJingwenRsp(char* head,char* body)
{
	SPCmd_ShenbingZhuankeJingwenRsp* rsp = (SPCmd_ShenbingZhuankeJingwenRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		for (int i = 0;i<sizeof(rsp->jingwen_attr)/sizeof(ZBAttr); i++)
		{
			rsp->jingwen_attr[i].attr_type = ntohl(rsp->jingwen_attr[i].attr_type);
			rsp->jingwen_attr[i].attr_value = ntohl(rsp->jingwen_attr[i].attr_value);
			rsp->jingwen_attr[i].cuiqu_suc_ratio = ntohl(rsp->jingwen_attr[i].cuiqu_suc_ratio);
			CMD_LOG("jingwen_attr[%d] --> {type = %d,attr_value = %d,cuiqu_suc_ratio = %d}",
				i,
				rsp->jingwen_attr[i].attr_type,
				rsp->jingwen_attr[i].attr_value,
				rsp->jingwen_attr[i].cuiqu_suc_ratio
				);
		}
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_shenbingZhuankeJingwenRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithQueryArenaTeamBasicInfoRsp(char* head,char* body)
{
	SPCmd_QueryArenaTeamBasicInfoRsp* rsp = (SPCmd_QueryArenaTeamBasicInfoRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->team_info.jifen = ntohl(rsp->team_info.jifen);
		rsp->team_info.free_fight_remainder_time = ntohl(rsp->team_info.free_fight_remainder_time);
		rsp->team_info.challenge_remainder_time = ntohl(rsp->team_info.challenge_remainder_time);
		rsp->team_info.buy_challenge_remainder_time = ntohl(rsp->team_info.buy_challenge_remainder_time);

		CMD_LOG("jifen = %d,free_fight_remainder_time = %d,challenge_remainder_time = %d,buy_challenge_remainder_time = %d",
			rsp->team_info.jifen,
			rsp->team_info.free_fight_remainder_time,
			rsp->team_info.challenge_remainder_time,
			rsp->team_info.buy_challenge_remainder_time
			);
	}

	if (!reloadCheck(rsp->err_code,(SimpleProtocolCmd)((SPHead*)head)->cmd,MSG_queryArenaTeamBasicInfoRsp,MSG_Rsp::createMsg(rsp)))
	{
		return;
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryArenaTeamBasicInfoRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithQueryArenaMarketRsp(char* head,char* body)
{
	SPCmd_QueryArenaMarketRsp* rsp = (SPCmd_QueryArenaMarketRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryArenaMarketRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->item_count = ntohl(rsp->item_count);
		CMD_LOG("item_count = %d",rsp->item_count);

		for (int i=0;i<rsp->item_count;i++)
		{
			rsp->item_arr[i].item_type = ntohl(rsp->item_arr[i].item_type);
			rsp->item_arr[i].item_count = ntohl(rsp->item_arr[i].item_count);
			rsp->item_arr[i].jifen_count = ntohl(rsp->item_arr[i].jifen_count);
			flat_data.item_list.push_back(rsp->item_arr[i]);

			CMD_LOG("item_arr[%d]---> {item_type = %u,item_count = %u,jifen_count = %u}",
				i,
				rsp->item_arr[i].item_type,
				rsp->item_arr[i].item_count,
				rsp->item_arr[i].jifen_count
				);
		}
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryArenaMarketRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithRefreshArenaChlgListRsp(char* head,char* body)
{
	SPCmd_RefreshArenaChallengeListRsp* rsp = (SPCmd_RefreshArenaChallengeListRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_RefreshArenaChallengeListRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		flat_data.gold_balance = ntohl(rsp->gold_balance);
		flat_data.silver_balance = ntohl(rsp->silver_balance);
		flat_data.item_need.type = ntohl(rsp->item_need.type);
		flat_data.item_need.count = ntohl(rsp->item_need.count);
		int count = ntohl(rsp->hero_count);
		CMD_LOG("gold_balance = %d,silver_balance = %d,refreshItem--->{type = %d,count=%d},hero_count = %d",
			flat_data.gold_balance,
			flat_data.silver_balance,
			flat_data.item_need.type,
			flat_data.item_need.count,
			count
			);

		CMD_LOG("---------------------Heros-------------------------");
		for (int i=0;i<count;i++)
		{
			rsp->hero_arr[i].jifen = ntohl(rsp->hero_arr[i].jifen);
			hton_HeroInfo(rsp->hero_arr[i].hero_info);
			flat_data.hero_list.push_back(rsp->hero_arr[i]);

			log(flat_data.hero_list[i].hero_info);
			CMD_LOG("jifeng = %d, has_win = %s",flat_data.hero_list[i].jifen,flat_data.hero_list[i].has_win?"true":"false");

		}
		CMD_LOG("---------------------Heros-------------------------");

	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_refreshArenaChlgList_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithQueryArenaChlgListRsp(char* head,char* body)
{
	SPCmd_QueryArenaChallengeListRsp* rsp = (SPCmd_QueryArenaChallengeListRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryArenaChallengeListRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		flat_data.item_need.type= ntohl(rsp->item_need.type);
		flat_data.item_need.count= ntohl(rsp->item_need.count);
		int count = ntohl(rsp->hero_count);
		CMD_LOG("refreshItem-->{type = %d,count = %d} = %d,hero_count = %d",
			flat_data.item_need.type,flat_data.item_need.count,
			count
			);

		CMD_LOG("---------------------Heros-------------------------");
		for (int i=0;i<count;i++)
		{
			rsp->hero_arr[i].jifen = ntohl(rsp->hero_arr[i].jifen);
			hton_HeroInfo(rsp->hero_arr[i].hero_info);
			flat_data.hero_list.push_back(rsp->hero_arr[i]);

			log(flat_data.hero_list[i].hero_info);
			CMD_LOG("jifeng = %d,has_win = %s",flat_data.hero_list[i].jifen,flat_data.hero_list[i].has_win?"true":"false");

		}
		CMD_LOG("---------------------Heros-------------------------");

	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryArenaChlgListRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}

void CmdHelper::dealWithArenaChallengeRsp(char* head,char* body)
{
	SPCmd_ArenaChallengeRsp* rsp = (SPCmd_ArenaChallengeRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_ArenaChallengeRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		flat_data.jifen_after_fight = ntohl(rsp->jifen_after_fight);
		CMD_LOG("jifen_after_fight = %d",flat_data.jifen_after_fight);
		char* pointerToScript = ((char*)rsp) + sizeof(int) + sizeof(unsigned int);
		// 剧本
		FightScriptBuilder builder;
		vector<FightScript> scripts = builder.CharArr_2_FightScriptVec(pointerToScript,1);
		flat_data.fightScript = scripts[0];

	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_arenaChallengeRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithArenaMarketBuyRsp(char* head,char* body)
{
	SPCmd_ArenaMarketBuyRsp* rsp = (SPCmd_ArenaMarketBuyRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->jifen_balance = ntohl(rsp->jifen_balance);
		CMD_LOG("jifen_balance = %d",rsp->jifen_balance);
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_arenaMarketBuyRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithArenaBuyChallengeTimeRsp(char* head,char* body)
{
	SPCmd_ArenaBuyChallengeTimeRsp* rsp = (SPCmd_ArenaBuyChallengeTimeRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->gold_balance = ntohl(rsp->gold_balance);
		rsp->challenge_remainder_time = ntohl(rsp->challenge_remainder_time);
		rsp->buy_challenge_remainder_time = ntohl(rsp->buy_challenge_remainder_time);
		CMD_LOG("gold_balance = %d,challenge_remainder_time = %d,buy_challenge_remainder_time = %d",
			rsp->gold_balance,
			rsp->challenge_remainder_time,
			rsp->buy_challenge_remainder_time
			);
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_arenaBuyChlgTimeRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithQueryZahuopuRsp(char* head,char* body)
{
	SPCmd_QueryZahuopuRsp* rsp = (SPCmd_QueryZahuopuRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->silver_need_for_refresh = ntohl(rsp->silver_need_for_refresh);
		rsp->draw_need_coupons = ntohl(rsp->draw_need_coupons);
		rsp->coupons_balance = ntohl(rsp->coupons_balance);

		CMD_LOG("silver_need_for_refresh = %u,draw_need_coupons = %u,coupons_balance = %u",
			rsp->silver_need_for_refresh,
			rsp->draw_need_coupons,
			rsp->coupons_balance);

		int count = 6;
		for (int i=0;i<count;i++)
		{
			rsp->item_arr[i].item_type = ntohl(rsp->item_arr[i].item_type);
			rsp->item_arr[i].suipian_id = ntohl(rsp->item_arr[i].suipian_id);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);

			if (rsp->item_arr[i].suipian_id > 0)
			{
				CMD_LOG("[%d] item suipian --> {type = %d,suipian_id = %d,count = %d,is_getted = %s}",
					i,
					rsp->item_arr[i].item_type,rsp->item_arr[i].suipian_id,rsp->item_arr[i].count,
					rsp->item_arr[i].already_got == true?"true":"false");
			}
			else
			{
				CMD_LOG("[%d] item --> {type = %d,count = %d,is_getted = %s}",
					i,
					rsp->item_arr[i].item_type,rsp->item_arr[i].count,
					rsp->item_arr[i].already_got == true?"true":"false");
			}
		}

	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryZahuopuRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithRefreshZahuopuRsp(char* head,char* body)
{
	SPCmd_RefreshZahuopuRsp* rsp = (SPCmd_RefreshZahuopuRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->silver_need_for_refresh = ntohl(rsp->silver_need_for_refresh);
		rsp->silver_balance = ntohl(rsp->silver_balance);
		rsp->draw_need_coupons = ntohl(rsp->draw_need_coupons);
		rsp->coupons_balance = ntohl(rsp->coupons_balance);

		CMD_LOG("silver_need_for_refresh = %u,silver_balance = %d,draw_need_coupons = %d,coupons_balance = %d",
			rsp->silver_need_for_refresh,rsp->silver_balance,rsp->draw_need_coupons,rsp->coupons_balance);
		int count = 6;
		for (int i=0;i<count;i++)
		{
			rsp->item_arr[i].item_type = ntohl(rsp->item_arr[i].item_type);
			rsp->item_arr[i].suipian_id = ntohl(rsp->item_arr[i].suipian_id);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);

			if (rsp->item_arr[i].suipian_id > 0)
			{
				CMD_LOG("[%d] item suipian --> {type = %d,suipian_id = %d,count = %d,is_getted = %s}",
					i,
					rsp->item_arr[i].item_type,rsp->item_arr[i].suipian_id,rsp->item_arr[i].count,
					rsp->item_arr[i].already_got == true?"true":"false");
			}
			else
			{
				CMD_LOG("[%d] item --> {type = %d,count = %d,is_getted = %s}",
					i,
					rsp->item_arr[i].item_type,rsp->item_arr[i].count,
					rsp->item_arr[i].already_got == true?"true":"false");
			}
		}

	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_refreshZahuopuRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithZahuopuGetOneItemRsp(char* head,char* body)
{
	SPCmd_ZahuopuGetOneItemRsp* rsp = (SPCmd_ZahuopuGetOneItemRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_ZahuopuGetOneItemRsp flatData;
	flatData.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->get_index = ntohl(rsp->get_index);
		rsp->gold_balance = ntohl(rsp->gold_balance);
		rsp->coupons_balance = ntohl(rsp->coupons_balance);

		flatData.get_index = rsp->get_index;
		flatData.gold_balance = rsp->gold_balance;
		flatData.coupons_balance = rsp->coupons_balance;

		CMD_LOG("get_index = %d,gold_balance = %d,coupons_balance = %d",flatData.get_index,flatData.gold_balance,flatData.coupons_balance);

	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_zahuopuGetOneItemRsp_F,Flat_MSG_Rsp::createMsg(&flatData));
}


void CmdHelper::dealWithQueryHellTaskHeroInfoRsp(char* head,char* body)
{
	SPCmd_QueryHellTaskHeroInfoRsp* rsp = (SPCmd_QueryHellTaskHeroInfoRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->battle_time_remainder = ntohl(rsp->battle_time_remainder);
		rsp->last_active_task_id = ntohl(rsp->last_active_task_id);

		CMD_LOG("battle_time_remainder = %d,last_active_task_id = %d",
			rsp->battle_time_remainder,rsp->last_active_task_id);

	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryHellTaskHeroInfoRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithHellTaskBattleRspRsp(char* head,char* body)
{
	SPCmd_HellTaskBattleRsp* rsp = (SPCmd_HellTaskBattleRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_HellTaskBattleRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->fightNum = ntohl(rsp->fightNum);
		// 剧本
		FightScriptBuilder builder;
		flat_data.fightScriptVec = builder.CharArr_2_FightScriptVec(rsp->fightScript_array,rsp->fightNum);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_hellTaskBattleRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}

void CmdHelper::dealWithQueryYuanshenOfHeroRsp(char* head,char* body)
{
	SPCmd_QueryYuanshenOfHeroRsp* rsp = (SPCmd_QueryYuanshenOfHeroRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryYuanshenOfHeroRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		hton_HeroDetail(rsp->hero_detail);
		flat_data.hero_detail = rsp->hero_detail;
		log(rsp->hero_detail);
		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);

		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,rsp->zhuangbei_count,flat_data.equips);

#if (DATAPOOL_LOG == 1)
		for (int zb_index = 0;zb_index < rsp->zhuangbei_count;zb_index++)
		{
			CMD_LOG("\n--------------------ZhuangbeiDetail[%d]---------------------------------",zb_index);
			log(flat_data.equips[zb_index]);
			CMD_LOG("--------------------ZhuangbeiDetail[%d]---------------------------------",zb_index);
		}
#endif

	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryYuanshenOfHeroRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithChurchLevelUpRsp(char* head,char* body)
{
	SPCmd_ChurchLevelUpRsp* rsp = (SPCmd_ChurchLevelUpRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		rsp->church_level = ntohl(rsp->church_level);
		rsp->dalishi_balance = ntohl(rsp->dalishi_balance);
		rsp->huagangyan_balance = ntohl(rsp->huagangyan_balance);
		rsp->mucai_balance = ntohl(rsp->mucai_balance);
		rsp->shuijinshi_balance = ntohl(rsp->shuijinshi_balance);
		rsp->silver_balance = ntohl(rsp->silver_balance);

		CMD_LOG("church_level = %u,dalishi_balance = %u,huagangyan_balance = %u,mucai_balance = %u,shuijinshi_balance = %u,silver_balance = %u",
			rsp->church_level,rsp->dalishi_balance,rsp->huagangyan_balance,rsp->mucai_balance,rsp->shuijinshi_balance,rsp->silver_balance);
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_churchLevelUpRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithSkillLevelUpRsp(char* head,char* body)
{
	SPCmd_SkillLevelUpRsp* rsp = (SPCmd_SkillLevelUpRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	if (rsp->err_code == Success)
	{
		rsp->new_skill_level = ntohl(rsp->new_skill_level);
		rsp->silver_balance = ntohl(rsp->silver_balance);
		rsp->skill_book_end.type = ntohl(rsp->skill_book_end.type);
		rsp->skill_book_end.count = ntohl(rsp->skill_book_end.count);
		CMD_LOG("new_skill_level = %u,silver_balance = %u,skill_book_end = {type = %u,count = %u}",
			rsp->new_skill_level,rsp->silver_balance,rsp->skill_book_end.type,rsp->skill_book_end.count);
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_skillLevelUpRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithGetZhuangbeiAlbumRsp(char* head,char* body)
{
	SPCmd_GetZhuangbeiAlbumRsp* rsp = (SPCmd_GetZhuangbeiAlbumRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_GetZhuangbeiAlbumRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->legend_count = ntohl(rsp->legend_count);
		rsp->xiyou_count = ntohl(rsp->xiyou_count);
		CMD_LOG("legend_count = %u,xiyou_count = %u",
			rsp->legend_count,rsp->xiyou_count);

		vector<Flat_DuanzaoGroupZBInfo> tempList;
		CMD_LOG("----------------------------Zhuangbei-------------------------");
		ntoh_DuanzaoGroupZBInfoArrayToFlat_DuanzaoGroupZBInfoVec(
			(DuanzaoGroupZBInfo*)rsp->zb_arr,
			rsp->legend_count+rsp->xiyou_count,
			tempList);
		CMD_LOG("----------------------------Zhuangbei-------------------------");

		CCAssert(rsp->legend_count > 0,"必须有传奇套装传过来");
		OneSuitInfo chuanqi;
		// 根据传奇的groupId查询她的套装信息
		//bool found = SuitPool::getPool()->getSuitInforBySuitId(tempList[0].zb_group_id,chuanqi);
		bool found = SuitPool::getPool()->getSuitInforByZBGroupId(tempList[0].zb_group_id,chuanqi);
		CCAssert(found,"必须找到");
		CCAssert(chuanqi.parts_of_suit.size() == rsp->legend_count,"个数也应该一样");

		int tempListReadIndex = 0;

		// 传奇套装
		for (int i=0,count=rsp->legend_count; i<count; i++)
		{
			flat_data.chuanqiSuitData.push_back(tempList[tempListReadIndex]);
			tempListReadIndex ++;
		}
		// 稀有
		for (int i=0; i<rsp->xiyou_count; i++)
		{
			flat_data.xiYouVec.push_back(tempList[tempListReadIndex]);
			tempListReadIndex ++;
		}

	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getZhuangbeiAlbumRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}

void CmdHelper::dealWithCityGuardProcessNotifyRsp(char* head,char* body)
{
	SPCmd_CityGuardProcessNotify* rsp = (SPCmd_CityGuardProcessNotify*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_CityGuardProcessNotify flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->play_time = ntohl(rsp->play_time);
		rsp->boss_hp_start = ntohl(rsp->boss_hp_start);
		rsp->boss_hp_end = ntohl(rsp->boss_hp_end);
		rsp->hero_count = ntohl(rsp->hero_count);

		flat_data.play_time = rsp->play_time;
		flat_data.boss_hp_start = rsp->boss_hp_start;
		flat_data.boss_hp_end = rsp->boss_hp_end;

		CMD_LOG("need play time = %u,boss_hp_start = %u,boss_hp_end = %u,hero_count = %u",
			flat_data.play_time,flat_data.boss_hp_start,flat_data.boss_hp_end,rsp->hero_count
			);

		for (int i=0;i<rsp->hero_count;i++)
		{
			rsp->another_hero[i].profession_id = ntohl(rsp->another_hero[i].profession_id);
			rsp->another_hero[i].shanghai = ntohl(rsp->another_hero[i].shanghai);
			flat_data.shanghaiVec.push_back(rsp->another_hero[i]);

			CMD_LOG("hero shanghai[%d] = {name = %s,profession_id = %u,shanghai = %u}",
				i,rsp->another_hero[i].name,rsp->another_hero[i].profession_id,rsp->another_hero[i].shanghai);
		}
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_cityGuardProcessNotifyRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithGetCityGuardCurrentStatRsp(char* head,char* body)
{
	SPCmd_getCityGuardCurrentStatRsp* rsp = (SPCmd_getCityGuardCurrentStatRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->stat.boss_blood_total = ntohl(rsp->stat.boss_blood_total);
		rsp->stat.is_opening = rsp->stat.is_opening;
		rsp->stat.time_before_open = ntohl(rsp->stat.time_before_open);
		rsp->have_reward = rsp->have_reward;

		CMD_LOG("is_opening = %s,time_before_open = %u,boss_blood_total = %u,have_reward = %s",
			rsp->stat.is_opening?"true":"false",rsp->stat.time_before_open,rsp->stat.boss_blood_total,
			rsp->have_reward?"true":"false");
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getCityGuardCurrentStatRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithCityGuardBattleRsp(char* head,char* body)
{
	SPCmd_cityGuardBattleRsp* rsp = (SPCmd_cityGuardBattleRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_cityGuardBattleRsp flat_data;
	flat_data.err_code = rsp->err_code;

	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		flat_data.gold_balance = ntohl(rsp->gold_balance);
		flat_data.battle_data.is_boss_dead = rsp->battle_data.is_boss_dead;
		if (!flat_data.battle_data.is_boss_dead)
		{
			flat_data.battle_data.total_shanghai = ntohl(rsp->battle_data.total_shanghai);
			flat_data.battle_data.shanghai_order = ntohl(rsp->battle_data.shanghai_order);

			CMD_LOG("%s:is_boss_dead = false,total_shanghai = %d,shanghai_order = %d,gold_balance = %d",
				__FUNCTION__,flat_data.battle_data.total_shanghai,
				flat_data.battle_data.shanghai_order,flat_data.gold_balance);

			FightScriptBuilder builder;
			flat_data.battle_data.fightScript = builder.CharArr_2_FightScriptVec(rsp->battle_data.fightScript,1)[0];
		}else
		{
			flat_data.battle_data.total_shanghai = 0;
			flat_data.battle_data.shanghai_order = 0;

			CMD_LOG("%s:is_boss_dead = true!!",__FUNCTION__);
		}
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_cityGuardBattleRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
//void CmdHelper::dealWithCityGuardBattleSkipCDRsp(char* head,char* body)
//{
//	SPCmd_cityGuardBattleSkipCDRsp* rsp = (SPCmd_cityGuardBattleSkipCDRsp*)body;
//	rsp->err_code = ntohl(rsp->err_code);
//
//	Flat_cityGuardBattleSkipCDRsp flat_data;
//	flat_data.err_code = rsp->err_code;
//
//	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
//
//	if (rsp->err_code == Success)
//	{
//		flat_data.battle_data.is_boss_dead = rsp->battle_data.is_boss_dead;
//		flat_data.battle_data.total_shanghai = ntohl(rsp->battle_data.total_shanghai);
//		flat_data.battle_data.shanghai_order = ntohl(rsp->battle_data.shanghai_order);
//		flat_data.gold_balance = ntohl(rsp->gold_balance);
//		// 剧本
//		FightScriptBuilder builder;
//		flat_data.battle_data.fightScript = builder.CharArr_2_FightScriptVec(rsp->battle_data.fightScript,1)[0];
//	}
//	// 发送消息
//	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_cityGuardBattleSkipCDRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
//
//}
void CmdHelper::dealWithGetCityGuardLastDayReportRsp(char* head,char* body)
{
	SPCmd_getCityGuardLastDayReportRsp* rsp = (SPCmd_getCityGuardLastDayReportRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_getCityGuardLastDayReportRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->have_reward = rsp->have_reward;
		rsp->lastday_report.cityguard_total_time = ntohl(rsp->lastday_report.cityguard_total_time);

		flat_data.have_reward = rsp->have_reward;
		flat_data.lastday_report.cityguard_total_time = rsp->lastday_report.cityguard_total_time;

		CMD_LOG("have_reward = %s,cityguard_total_time = %u",
			rsp->have_reward?"true":"false",rsp->lastday_report.cityguard_total_time);
		hton_HeroInfo(rsp->lastday_report.hero_kill_boss);
		rsp->lastday_report.hero_kill_reward.type = ntohl(rsp->lastday_report.hero_kill_reward.type);
		rsp->lastday_report.hero_kill_reward.count = ntohl(rsp->lastday_report.hero_kill_reward.count);

		flat_data.lastday_report.hero_kill_boss = rsp->lastday_report.hero_kill_boss;
		flat_data.lastday_report.hero_kill_reward = rsp->lastday_report.hero_kill_reward;

		CMD_LOG("hero_kill_boss:");
		log(rsp->lastday_report.hero_kill_boss);
		CMD_LOG("hero_kill_reward = {type = %d,count = %d}",rsp->lastday_report.hero_kill_reward.type,rsp->lastday_report.hero_kill_reward.count);

		int num_20 = sizeof(rsp->lastday_report.hero_order)/sizeof(HeroInfo);
		for (int i=0;i<num_20;i++)
		{
			hton_HeroInfo(rsp->lastday_report.hero_order[i]);

			if (rsp->lastday_report.hero_order[i].hero_id == 0)
				continue;

			rsp->lastday_report.hero_order_reward[i].type = ntohl(rsp->lastday_report.hero_order_reward[i].type);
			rsp->lastday_report.hero_order_reward[i].count = ntohl(rsp->lastday_report.hero_order_reward[i].count);

			flat_data.lastday_report.hero_order_reward.push_back(rsp->lastday_report.hero_order_reward[i]);
			flat_data.lastday_report.hero_order.push_back(rsp->lastday_report.hero_order[i]);
			CMD_LOG("hero_order[%d]:",i);
			log(rsp->lastday_report.hero_order[i]);
			CMD_LOG("hero_order_reward = {type = %d,count = %d}",rsp->lastday_report.hero_order_reward[i].type,rsp->lastday_report.hero_order_reward[i].count);
		}

		int luckNum = sizeof(rsp->lastday_report.hero_lucky)/sizeof(HeroInfo);
		for (int i=0;i<luckNum;i++)
		{
			hton_HeroInfo(rsp->lastday_report.hero_lucky[i]);
			if (rsp->lastday_report.hero_lucky[i].hero_id == 0)
				continue;

			flat_data.lastday_report.hero_lucky.push_back(rsp->lastday_report.hero_lucky[i]);

			CMD_LOG("hero_lucky[%d]:",i);
			log(rsp->lastday_report.hero_lucky[i]);
		}
		rsp->lastday_report.hero_lucky_reward.type = ntohl(rsp->lastday_report.hero_lucky_reward.type);
		rsp->lastday_report.hero_lucky_reward.count = ntohl(rsp->lastday_report.hero_lucky_reward.count);
		flat_data.lastday_report.hero_lucky_reward = rsp->lastday_report.hero_lucky_reward;

		CMD_LOG("hero_lucky_reward = {type = %d,count = %d}",rsp->lastday_report.hero_lucky_reward.type,rsp->lastday_report.hero_lucky_reward.count);

	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getCityGuardLastDayReportRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}

void CmdHelper::dealWithGetMyCityGuardRewardRsp(char* head,char* body)
{
	SPCmd_GetMyCityGuardRewardRsp* rsp = (SPCmd_GetMyCityGuardRewardRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("Item Count = %d",rsp->count);
		for (int i = 0; i<rsp->count; i++)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			CMD_LOG("item_reward[%d] = {type = %d,count = %d}",i,rsp->item_arr[i].type,rsp->item_arr[i].count);
		}
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getMyCityGuardRewardRsp,MSG_Rsp::createMsg(rsp));

}

inline vector<Flat_BonusInfo> BonusInfoArrayToFlat_BonusInfoVec(BonusInfo* infor_arr,int count)
{
	vector<Flat_BonusInfo> ret;

	for (int i=0;i<count;i++)
	{
		Flat_BonusInfo flatInfor;

		infor_arr->type = ntohl(infor_arr->type);
		infor_arr->is_get = infor_arr->is_get;
		infor_arr->enable = infor_arr->enable;
		infor_arr->bonus_id = ntohl(infor_arr->bonus_id);

		flatInfor.type = (BonusType)infor_arr->type;
		flatInfor.is_get = infor_arr->is_get;
		flatInfor.enable = infor_arr->enable;
		flatInfor.bonus_id = infor_arr->bonus_id;

		CMD_LOG("Bonus[%d]-->type = %u, bonus_id = %u,is_get = %s,enable = %s",i,
			infor_arr->type, infor_arr->bonus_id,infor_arr->is_get?"true":"false",infor_arr->enable?"true":"false");

		const char* pData = (char*)&infor_arr->bonus_id;
		pData += sizeof(unsigned int);
		//体力
		flatInfor.vitality = ntohl((unsigned int)(*pData));
		CCLog("vitality = %u", flatInfor.vitality);
		//item
		pData += sizeof(unsigned int);
		unsigned int itemCount = ntohl(*(unsigned int*)pData);
		CCLog("itemCount [%u]", itemCount);
		pData += sizeof(unsigned int);
		ItemInfo *pItems = (ItemInfo*)pData;
		for (int i=0; i<itemCount; ++i)
		{
			pItems[i].type = ntohl(pItems[i].type);
			pItems[i].count = ntohl(pItems[i].count);
			flatInfor.itemVec.push_back(pItems[i]);
			CCLog("item[%d] type=%u, count=%u", i, pItems[i].type, pItems[i].count);
//			pItems += sizeof(ItemInfo);
		}
		//道具碎片
		pData = (const char*)&pItems[itemCount];
		unsigned int itemDebrisCount = ntohl(*(unsigned int*)pData);
		CCLog("itemDebrisCount [%u]", itemDebrisCount);
		pData += sizeof(unsigned int);
		ItemSuipianInfo *pItemDebrises = (ItemSuipianInfo*)pData;
		for (int i=0; i<itemDebrisCount; ++i)
		{
			pItemDebrises[i].item_id = ntohl(pItemDebrises[i].item_id);
			pItemDebrises[i].suipian_count = ntohl(pItemDebrises[i].suipian_count);
			pItemDebrises[i].suipian_id = ntohl(pItemDebrises[i].suipian_id);
			flatInfor.itemDebrisVec.push_back(pItemDebrises[i]);
			CCLog("itemSuiPian[%d] item_id=%u, suipian_count=%u, suipian_id=%u", i, pItemDebrises[i].item_id, pItemDebrises[i].suipian_count, pItemDebrises[i].suipian_id);
//			pItemDebrises += sizeof(ItemSuipianInfo);
		}
		//装备碎片
		pData = (const char*)&pItemDebrises[itemDebrisCount];
		unsigned int zbDebrisCount = ntohl(*(unsigned int*)pData);
		CCLog("zbDebrisCount [%u]", zbDebrisCount);
		pData += sizeof(unsigned int);
		ShenbingSuipianInfo *pShenbingDebrises = (ShenbingSuipianInfo*)pData;
		for (int i=0; i<zbDebrisCount; ++i)
		{
			pShenbingDebrises[i].suipian_count = ntohl(pShenbingDebrises[i].suipian_count);
			pShenbingDebrises[i].suipian_id = ntohl(pShenbingDebrises[i].suipian_id);
			pShenbingDebrises[i].zb_group_id = ntohl(pShenbingDebrises[i].zb_group_id);
			flatInfor.equipDebrisVec.push_back(pShenbingDebrises[i]);
			CCLog("shenbingSuiPian[%d] zb_group_id=%u, suipian_id=%u, suipian_count=%u", i, pShenbingDebrises[i].zb_group_id, pShenbingDebrises[i].suipian_id, pShenbingDebrises[i].suipian_count);
//			pShenbingDebrises += sizeof(ShenbingSuipianInfo);
		}
		//礼包
		pData = (const char*)&pShenbingDebrises[zbDebrisCount];
		unsigned int giftCount = ntohl(*(unsigned int*)pData);
		CCLog("giftCount [%u]", giftCount);
		pData += sizeof(unsigned int);
		GiftBag *pGiftBags = (GiftBag*)pData;
		for (int i=0; i<giftCount; ++i)
		{
			pGiftBags[i].id = ntohl(pGiftBags[i].id);
			pGiftBags[i].count = ntohl(pGiftBags[i].count);
			flatInfor.giftBagVec.push_back(pGiftBags[i]);
			CCLog("giftBag[%d] id=%u, count=%u", i, pGiftBags[i].id, pGiftBags[i].count);
//			pGiftBags += sizeof(GiftBag);
		}
		//随机装备数量
		pData = (const char*)&pGiftBags[giftCount];
		flatInfor.ran_zb_count = ntohl(*(unsigned int*)pData);
		CCLog("random_zb_count [%u]", flatInfor.ran_zb_count);
		//固定装备
		pData += sizeof(unsigned int);
		unsigned int eqCount = ntohl(*(unsigned int*)pData);
		CCLog("eqCount [%u]", eqCount);
		pData += sizeof(unsigned int);
		DuanzaoGroupZBInfo *pEquip = (DuanzaoGroupZBInfo*)pData;
		pData += ntoh_DuanzaoGroupZBInfoArrayToFlat_DuanzaoGroupZBInfoVec(pEquip,eqCount,flatInfor.equipVec);
		//
		/* type为BonusType_DayWelfare或BonusType_Reward 有以下类容*/
		if (flatInfor.type == BonusType_DayWelfare || flatInfor.type == BonusType_Reward)
		{
			//>0:领取多倍奖励的vip等级分界线 0:只有1倍奖励 
			flatInfor.vip_level = ntohl(*(unsigned int *)pData); 			
			CCLog("vip_level = %u", flatInfor.vip_level);
			//倍数
			pData += sizeof(unsigned int);
			flatInfor.multiple = ntohl(*(unsigned int *)pData);		
			CCLog("multiple = %u", flatInfor.multiple);
			//描述内容为变长, 以'\0'结束
			pData += sizeof(unsigned int);
			flatInfor.description = pData;
			//
			pData += flatInfor.description.length()+1;
		}
				
		ret.push_back(flatInfor);

		infor_arr = (BonusInfo*)pData;
	}

	return ret;
}
/*
void CmdHelper::dealWithQueryDailyLoginBonusRsp(char* head,char* body)
{
	SPCmd_QueryDailyLoginBonusRsp* rsp = (SPCmd_QueryDailyLoginBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryDailyLoginBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("BonusNum = %d",rsp->count);
		flat_data.bonus_arr = BonusInfoArrayToFlat_BonusInfoVec((BonusInfo*)rsp->bonus_arr,rsp->count);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryDailyLoginBonusRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithQueryLevelBonusRsp(char* head,char* body)
{
	SPCmd_QueryLevelBonusRsp* rsp = (SPCmd_QueryLevelBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryLevelBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("BonusNum = %d",rsp->count);
		flat_data.bonus_arr = BonusInfoArrayToFlat_BonusInfoVec((BonusInfo*)rsp->bonus_arr,rsp->count);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryLevelBonusRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithQuerySystemBonusRsp(char* head,char* body)
{
	SPCmd_QuerySystemBonusRsp* rsp = (SPCmd_QuerySystemBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryLevelBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("BonusNum = %d",rsp->count);
		flat_data.bonus_arr = BonusInfoArrayToFlat_BonusInfoVec((BonusInfo*)rsp->bonus_arr,rsp->count);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_querySystemBonusRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithGetDailyLoginBonusRsp(char* head,char* body)
{
	SPCmd_GetDailyLoginBonusRsp* rsp = (SPCmd_GetDailyLoginBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_GetDailyLoginBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CMD_LOG("zhuangbei_count = %d",rsp->zhuangbei_count);
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->detail,rsp->zhuangbei_count,flat_data.externGettedEquip);
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getDailyLoginBonusRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithGetLevelBonusRsp(char* head,char* body)
{
	SPCmd_GetLevelBonusRsp* rsp = (SPCmd_GetLevelBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_GetLevelBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CMD_LOG("zhuangbei_count = %d",rsp->zhuangbei_count);
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->detail,rsp->zhuangbei_count,flat_data.externGettedEquip);
	}

	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getLevelBonusRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithGetSystemBonusRsp(char* head,char* body)
{
	SPCmd_GetSystemBonusRsp* rsp = (SPCmd_GetSystemBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_GetSystemBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CMD_LOG("zhuangbei_count = %d",rsp->zhuangbei_count);
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->detail,rsp->zhuangbei_count,flat_data.externGettedEquip);
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getSystemBonusRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}*/

/*void CmdHelper::dealWithGetChessRandomBoxRsp(char* head,char* body)
{
	SPCmd_GetChessRandomBoxRsp* rsp = (SPCmd_GetChessRandomBoxRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("itemCount = %d",rsp->count);
		for (int i = 0; i < rsp->count; i++)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			CMD_LOG("item[%d] --> {type = %d,count = %d}",i,rsp->item_arr[i].type,rsp->item_arr[i].count);
		}
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getChessRandomBoxRsp,MSG_Rsp::createMsg(rsp));
	
}*/

void CmdHelper::dealWithQueryTaskRateRsp(char* head,char* body)
{
	SPCmd_QueryRateTaskRsp* rsp = (SPCmd_QueryRateTaskRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryRateTaskRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->task_max = ntohs(rsp->task_max);
		CMD_LOG("task_max = %d",rsp->task_max);
		for (int i=0;i<rsp->task_max;i++)
		{
			rsp->task_info[i].star = ntohs(rsp->task_info[i].star);
			flat_data.taskRate.push_back(rsp->task_info[i]);

			CMD_LOG("task_star[%d] = %d, cuprumGeted=%s, silverGeted=%s,  goldGeted=%s",
				i,rsp->task_info[i].star, 
				rsp->task_info[i].is_get_cuprum?"true":"false", 
				rsp->task_info[i].is_get_silver?"true":"false",
				rsp->task_info[i].is_get_gold?"true":"false");
		}

		rsp->chap_max = ntohs(rsp->chap_max);
		CMD_LOG("chap_max = %d",rsp->chap_max);
		for (int i=0;i<rsp->chap_max;i++)
		{
			rsp->chap_info[i].star = ntohs(rsp->chap_info[i].star);
			flat_data.chapterRate.push_back(rsp->chap_info[i]);
			CMD_LOG("chap_star[%d] = %d, cuprumGeted=%s, goldGeted=%s",i,rsp->chap_info[i].star,
				rsp->chap_info[i].is_get_cuprum?"true":"false", 
				rsp->chap_info[i].is_get_gold?"true":"false");
		}

	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryRateTaskRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithGetTaskRateBonusRsp(char* head,char* body)
{
	SPCmd_GetRateTaskBonusRsp* rsp = (SPCmd_GetRateTaskBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getRateTaskBonusRsp,MSG_Rsp::createMsg(rsp));

}

void CmdHelper::dealWithQueryRateTaskBonusRsp(char* head,char* body)
{
	SPCmd_QueryRateTaskBonusRsp* rsp = (SPCmd_QueryRateTaskBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryRateTaskBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;

	flat_data.type = lastQueryTaskRateType;

	if (rsp->err_code == Success)
	{
		rsp->bonus_count = ntohs(rsp->bonus_count);
		CCLOG("bonus count = %d", rsp->bonus_count);
		const char* pData = (char*)rsp->bonus_arr;
		for (int i=0; i<rsp->bonus_count; ++i)
		{
			unsigned short star = *(unsigned short*)pData;
			star =  ntohs(star);
			pData += sizeof(unsigned short);
			unsigned int exp = *(unsigned int*)pData;
			exp = ntohl(exp);
			pData += sizeof(unsigned int);
			unsigned short itemCount = *(unsigned short*)pData;
			itemCount = ntohs(itemCount);
			pData += sizeof(unsigned short);
			CCLOG("BonusArr[%d] : starType=%d, exp=%d, itemcount=%d", i, star, exp, itemCount);
			ItemInfo *pItems = (ItemInfo*)pData;
			vector<ItemInfo> tempVec;
			int idx=0;
			for (; idx<itemCount; ++idx)
			{
				pItems[idx].type = ntohl(pItems[idx].type);
				pItems[idx].count = ntohl(pItems[idx].count);
				CCLOG("item[%d]: type=%d, count=%d", idx, pItems[idx].type, pItems[idx].count);
				tempVec.push_back(pItems[idx]);
			}
			pData = (char*)&pItems[idx];

			//
			if (star == RateStarLevel_Cuprum){
				flat_data.exp1 = exp;
				flat_data.items1 = tempVec;
			}
			else if(star == RateStarLevel_Silver){
				flat_data.exp2 = exp;
				flat_data.items2 = tempVec;
			}
			else if (star == RateStarLevel_Gold){
				flat_data.exp3 = exp;
				flat_data.items3 = tempVec;
			}
		}

	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryRateTaskBonus_F,Flat_MSG_Rsp::createMsg(&flat_data));

}

void CmdHelper::dealWithQueryVipPrivilegeRsp(char* head,char* body)
{
	SPCmd_QueryVipPrivilegeRsp* rsp = (SPCmd_QueryVipPrivilegeRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->priv_info.sweep_num = ntohl(rsp->priv_info.sweep_num);
		rsp->priv_info.buy_arena_num = ntohl(rsp->priv_info.buy_arena_num);
		rsp->priv_info.buy_vitality_num = ntohl(rsp->priv_info.buy_vitality_num);
		rsp->priv_info.over_creep_num = ntohl(rsp->priv_info.over_creep_num);
		rsp->priv_info.refresh_reward_num = ntohl(rsp->priv_info.refresh_reward_num);
		rsp->priv_info.cast_silver_num = ntohl(rsp->priv_info.cast_silver_num);

		rsp->deft_info.sweep_num = ntohl(rsp->deft_info.sweep_num);
		rsp->deft_info.buy_arena_num = ntohl(rsp->deft_info.buy_arena_num);
		rsp->deft_info.buy_vitality_num = ntohl(rsp->deft_info.buy_vitality_num);
		rsp->deft_info.over_creep_num = ntohl(rsp->deft_info.over_creep_num);
		rsp->deft_info.refresh_reward_num = ntohl(rsp->deft_info.refresh_reward_num);
		rsp->deft_info.cast_silver_num = ntohl(rsp->deft_info.cast_silver_num);


		CMD_LOG("buy_vitality_num = %d,sweep_num = %d,buy_arena_num = %d,over_creep_num = %d,honor_exchange = %s,refresh_reward_num = %d,cast_silver_num = %d",
			rsp->priv_info.buy_vitality_num,rsp->priv_info.sweep_num,rsp->priv_info.buy_arena_num,rsp->priv_info.over_creep_num,
			rsp->priv_info.honor_exchange?"true":"false",rsp->priv_info.refresh_reward_num,rsp->priv_info.cast_silver_num);
		CMD_LOG("\n-------default-------");
		CMD_LOG("buy_vitality_num = %d,sweep_num = %d,buy_arena_num = %d,over_creep_num = %d,honor_exchange = %s,refresh_reward_num = %d,cast_silver_num = %d",
			rsp->deft_info.buy_vitality_num,rsp->deft_info.sweep_num,rsp->deft_info.buy_arena_num,rsp->deft_info.over_creep_num,
			rsp->deft_info.honor_exchange?"true":"false",rsp->deft_info.refresh_reward_num,rsp->deft_info.cast_silver_num);
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryVipPrivilegeRsp,MSG_Rsp::createMsg(rsp));

}

void CmdHelper::dealWithUpgradeShenbingRsp(char* head,char* body)
{
	SPCmd_UpgradeShenbingRsp* rsp = (SPCmd_UpgradeShenbingRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_UpgradeShenbingRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		for (int i=0;i<sizeof(rsp->item_arr)/sizeof(ItemInfo);i++)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);

			flat_data.item_arr.push_back(rsp->item_arr[i]);

			CMD_LOG("item_arr[%d]-->{type = %d,count = %d}",i,rsp->item_arr[i].type,rsp->item_arr[i].count);
		}

		vector<EquipData> temp;
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->detail,1,temp);
		flat_data.equipNewData = temp[0];
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_upgradeShenbingRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}

void CmdHelper::dealWithQueryChargePacketRsp(char* head,char* body)
{
	SPCmd_QueryChargePacketRsp* rsp = (SPCmd_QueryChargePacketRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryChargePacketRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->next_vip_level = ntohl(rsp->next_vip_level);
		rsp->next_need_charge = ntohl(rsp->next_need_charge);

		flat_data.next_need_charge = rsp->next_need_charge;
		flat_data.next_vip_level = rsp->next_vip_level;
		flat_data.b_first_charged = rsp->b_first_charged;

		CMD_LOG("b_first_charged = %s,next_vip_level = %d,next_need_charge = %d",
			flat_data.b_first_charged?"true":"false",
			flat_data.next_vip_level,
			flat_data.next_need_charge
			);

		for (int i=0; i<sizeof(rsp->first_present_item)/sizeof(ItemInfo); i++)
		{
			rsp->first_present_item[i].type = ntohl(rsp->first_present_item[i].type);
			rsp->first_present_item[i].count = ntohl(rsp->first_present_item[i].count);
			flat_data.first_present_items.push_back(rsp->first_present_item[i]);

			CMD_LOG("first_present_item[%d] --> {type = %d,count = %d}",
				i,rsp->first_present_item[i].type,rsp->first_present_item[i].count
				);
		}

		rsp->count = ntohl(rsp->count);
		CMD_LOG("chargePaket count = %d",rsp->count);
		for (int i = 0; i < rsp->count; i++)
		{
			rsp->info[i].id = ntohl(rsp->info[i].id);
			rsp->info[i].charge = ntohl(rsp->info[i].charge);
			rsp->info[i].gold_count = ntohl(rsp->info[i].gold_count);
			rsp->info[i].present_count = ntohl(rsp->info[i].present_count);

			flat_data.chargePakets.push_back(rsp->info[i]);

			CMD_LOG("chargePaket[%d] --> {id = %d,charge = %d,gold_count = %d,present_count = %d}",
				i,rsp->info[i].id,rsp->info[i].charge,rsp->info[i].gold_count,rsp->info[i].present_count
				);
		}

	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryChargePacketRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));

}

void CmdHelper::dealWithUnlockAssistantRsp(char* head,char* body)
{
	SPCmd_UnlockAssistantRsp* rsp = (SPCmd_UnlockAssistantRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->honor = ntohl(rsp->honor);
		CMD_LOG("honor = %d",rsp->honor);
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_unlockAssistantRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithQueryGiftActivityRsp(char* head,char* body)
{
	SPCmd_QueryGiftActivityRsp* rsp = (SPCmd_QueryGiftActivityRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryGiftActivityRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("count = %d",rsp->count);
		for (int i=0; i<rsp->count; i++)
		{
			rsp->act_arr[i].id = ntohl(rsp->act_arr[i].id);
			rsp->act_arr[i].open_time = ntohl(rsp->act_arr[i].open_time);
			rsp->act_arr[i].close_time = ntohl(rsp->act_arr[i].close_time);

			CMD_LOG("act_arr[%d]-->{type = %d,is_geted = %s,open_time = %u,close_time = %u}",
				i,rsp->act_arr[i].id,rsp->act_arr[i].is_get?"true":"false" ,
				rsp->act_arr[i].open_time,rsp->act_arr[i].close_time
				);

			flat_data.activities.push_back(rsp->act_arr[i]);
		}
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryGiftActivityRsp_F,MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithGetGiftActivityRsp(char* head,char* body)
{
	SPCmd_GetGiftActivityRsp* rsp = (SPCmd_GetGiftActivityRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->vit_current = ntohl(rsp->vit_current);
		CMD_LOG("vit_current = %d",rsp->vit_current);
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getGiftActivityRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithQueryKilledMonsterRsp(char* head,char* body)
{
	SPCmd_QueryKilledMonsterRsp* rsp = (SPCmd_QueryKilledMonsterRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->elite_monster_count = ntohl(rsp->elite_monster_count);
		rsp->common_monster_count = ntohl(rsp->common_monster_count);
		CMD_LOG("common_monster_count = %d,elite_monster_count = %d",rsp->common_monster_count,rsp->elite_monster_count);
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryKilledMonsterRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithQueryKilledMonsterBonusRsp(char* head,char* body)
{
	SPCmd_QueryKilledMonsterBonusRsp* rsp = (SPCmd_QueryKilledMonsterBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryKilledMonsterBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->common_bonus_count = ntohl(rsp->common_bonus_count);
		rsp->elite_bonus_count = ntohl(rsp->elite_bonus_count);

		CMD_LOG("common_bonus_count = %d,elite_bonus_count = %d",rsp->common_bonus_count,rsp->elite_bonus_count);
		int common_bonus_count = rsp->common_bonus_count;
		int elite_bonus_count = rsp->elite_bonus_count;

		for (int i = 0; i < common_bonus_count; i++)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			CMD_LOG("common_bonus[%d] --> {type = %d,count = %d}",i,rsp->item_arr[i].type,rsp->item_arr[i].count);
			flat_data.commonBonus.push_back(rsp->item_arr[i]);
		}
		for (int i = 0; i < elite_bonus_count; i++)
		{
			int index = common_bonus_count+i;
			rsp->item_arr[index].type = ntohl(rsp->item_arr[index].type);
			rsp->item_arr[index].count = ntohl(rsp->item_arr[index].count);
			CMD_LOG("elite_bonus[%d] --> {type = %d,count = %d}",i,rsp->item_arr[index].type,rsp->item_arr[index].count);
			flat_data.eliteBonus.push_back(rsp->item_arr[index]);
		}

	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryKilledMonsterBonusRsp_F,Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithGetKilledMonsterBonusRsp(char* head,char* body)
{
	SPCmd_GetKilledMonsterBonusRsp* rsp = (SPCmd_GetKilledMonsterBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getKilledMonsterBonusRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithTopArenaQueryGuessRsp(char* head,char* body)
{
	SPCmd_TopArenaQueryGuessRsp* rsp = (SPCmd_TopArenaQueryGuessRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->hero_value_a = ntohl(rsp->hero_value_a);
		rsp->hero_value_b = ntohl(rsp->hero_value_b);

		CMD_LOG("hero_value_a = %u,hero_value_b = %u",rsp->hero_value_a,rsp->hero_value_b);
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_TopArenaQueryGuessRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithTopArenaQueryStatusRsp(char* head,char* body)
{
	SPCmd_TopArenaQueryStatusRsp* rsp = (SPCmd_TopArenaQueryStatusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->next_top_arena_time = ntohl(rsp->next_top_arena_time);
		CMD_LOG("openTime = %u",rsp->next_top_arena_time);
		for (int i=0; i<sizeof(rsp->stat)/sizeof(unsigned int); i++)
		{
			rsp->stat[i] = ntohl(rsp->stat[i]);
			CMD_LOG("stat[%d] = %d",i,rsp->stat[i]);
		}

		for (int i=0; i<sizeof(rsp->guess)/sizeof(TopArenaGuessInfo); i++)
		{
			rsp->guess[i].is_joined = rsp->guess[i].is_joined;
			rsp->guess[i].player.winer = rsp->guess[i].player.winer;
			hton_HeroInfo(rsp->guess[i].player.hero_a);
			hton_HeroInfo(rsp->guess[i].player.hero_b);
			rsp->guess[i].guess_winer = ntohs(rsp->guess[i].guess_winer);
			rsp->guess[i].result = ntohs(rsp->guess[i].result);
			rsp->guess[i].wager = ntohl(rsp->guess[i].wager);


			CMD_LOG("-----guess[%d]-----",i);
			CMD_LOG("is_joined = %s",rsp->guess[i].is_joined?"true":"false");
			CMD_LOG("player--> winner = %s",rsp->guess[i].player.winer?"A":"B");
			CMD_LOG("player--> hero_a:");
			log(rsp->guess[i].player.hero_a);
			CMD_LOG("player--> hero_b:");
			log(rsp->guess[i].player.hero_b);
			CMD_LOG("-----guess[%d]----- \n",i);
		}



		for (int i=0; i<sizeof(rsp->player_arr_8_4)/sizeof(TopArenaInfo); i++)
		{
			hton_HeroInfo(rsp->player_arr_8_4[i].hero_a);
			hton_HeroInfo(rsp->player_arr_8_4[i].hero_b);

			CMD_LOG("----player_arr_8_4[%d],---- winer = %s",
				i,rsp->player_arr_8_4[i].winer?"A":"B");
			CMD_LOG("hero_a:-->");
			log(rsp->player_arr_8_4[i].hero_a);
			CMD_LOG("hero_b:-->");
			log(rsp->player_arr_8_4[i].hero_b);
			CMD_LOG("----player_arr_8_4[%d]---- \n",i);
		}

		for (int i=0; i<sizeof(rsp->player_arr_4_2)/sizeof(TopArenaInfo); i++)
		{
			hton_HeroInfo(rsp->player_arr_4_2[i].hero_a);
			hton_HeroInfo(rsp->player_arr_4_2[i].hero_b);

			CMD_LOG("----player_arr_4_2[%d],---- winer = %s",
				i,rsp->player_arr_4_2[i].winer?"A":"B");
			CMD_LOG("hero_a:-->");
			log(rsp->player_arr_4_2[i].hero_a);
			CMD_LOG("hero_b:-->");
			log(rsp->player_arr_4_2[i].hero_b);
			CMD_LOG("----player_arr_4_2[%d]---- \n",i);
		}

		for (int i=0; i<sizeof(rsp->player_arr_2_1)/sizeof(TopArenaInfo); i++)
		{
			hton_HeroInfo(rsp->player_arr_2_1[i].hero_a);
			hton_HeroInfo(rsp->player_arr_2_1[i].hero_b);

			CMD_LOG("----player_arr_2_1[%d],---- winer = %s",
				i,rsp->player_arr_2_1[i].winer?"A":"B");
			CMD_LOG("hero_a:-->");
			log(rsp->player_arr_2_1[i].hero_a);
			CMD_LOG("hero_b:-->");
			log(rsp->player_arr_2_1[i].hero_b);
			CMD_LOG("----player_arr_2_1[%d]---- \n",i);
		}
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_topArenaQueryStatusRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithTopArenaGuessRsp(char* head,char* body)
{
	SPCmd_TopArenaGuessRsp* rsp = (SPCmd_TopArenaGuessRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->silver_balance = ntohl(rsp->silver_balance);
		CMD_LOG("silver_balance = %d",rsp->silver_balance);
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_topArenaGuessRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithTopArenaReplayRsp(char* head,char* body)
{
	SPCmd_TopArenaReplayRsp* rsp = (SPCmd_TopArenaReplayRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_topArenaReplayRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		FightScriptBuilder builder;
		flat_data.fightScript = builder.CharArr_2_FightScriptVec(rsp->fight_script,1)[0];
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_topArenaReplay_F,MSG_Rsp::createMsg(&flat_data));
}

void CmdHelper::dealWithCastSilverRsp(char* head,char* body)
{
	SPCmd_CastSilverRsp* rsp = (SPCmd_CastSilverRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->gold_balance = ntohl(rsp->gold_balance);
		rsp->silver_balance = ntohl(rsp->silver_balance);

		CMD_LOG("gold_balance = %d,silver_balance = %d",rsp->gold_balance,rsp->silver_balance);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_CastSilverRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithQueryLegendNoteRsp(char* head,char* body)
{
	SPCmd_QueryLegendNoteRsp* rsp = (SPCmd_QueryLegendNoteRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryLegendNoteRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->times = ntohl(rsp->times);
		CMD_LOG("times = %d",rsp->times);
		flat_data.times = rsp->times;


		// 准备装备数据
		int professions[] = {
			Profession_Yemanren,
			Profession_Chike,
			Profession_Fashi,
			Profession_Qishi,
			Profession_ShenJianShou,
			Profession_MiShu,
			Profession_WuSeng,
			Profession_YouXia,
			Profession_LieShou,
			Profession_WuShi,
		};
		vector<Flat_DuanzaoGroupZBInfo> null_vec;
		flat_data.all_profession_zhuangbeis.push_back(null_vec);

		vector<ChuanQi> allChuanqi = ChuanqiPool::getPool()->getAllChuanqi();
		const int chuanqi_count = allChuanqi.size();
		const int level = CS::getShenBingLevels()[0];
		for (int i=0; i<sizeof(professions)/sizeof(int); i++)
		{
			vector<Flat_DuanzaoGroupZBInfo> zb_vec;

			for (int zbIndex=0; zbIndex<chuanqi_count; zbIndex++)
			{
				if (allChuanqi[zbIndex].allChuanqiFenShenInLifeSpan[0].profession_id == professions[i])
				{
					Flat_DuanzaoGroupZBInfo data;
					bool mustFound = allChuanqi[zbIndex].getChuanqiDataByLevel(level,data);
					CCAssert(mustFound,"");
					if (mustFound)
					{
						zb_vec.push_back(data);
					}
				}
			}
			flat_data.all_profession_zhuangbeis.push_back(zb_vec);
		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryLegendNoteRsp_F,MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithChallengeLegendNoteRsp(char* head,char* body)
{
	SPCmd_ChallengeLegendNoteRsp* rsp = (SPCmd_ChallengeLegendNoteRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_ChallengeLegendNoteRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		FightScriptBuilder builder;
		flat_data.fightScript = builder.CharArr_2_FightScriptVec(rsp->fight_script,1)[0];
	}
	// 发送消息
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_challengeLegendNoteRsp_f,MSG_Rsp::createMsg(&flat_data));
}


void CmdHelper::dealWithQuerySelfChallengeInfoRsp(char* head,char* body)
{
	SPCmd_QuerySelfChallengeInfoRsp* rsp = (SPCmd_QuerySelfChallengeInfoRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->open_time = htonl(rsp->open_time);
		rsp->close_time = htonl(rsp->close_time);

		rsp->info.times = htonl(rsp->info.times);
		rsp->info.energy = htonl(rsp->info.energy);
		rsp->info.hp_gain = htonl(rsp->info.hp_gain);
		rsp->info.attack_gain = htonl(rsp->info.attack_gain);
		rsp->info.defense_gain = htonl(rsp->info.defense_gain);
		rsp->info.difficulty = htonl(rsp->info.difficulty);
		rsp->info.checkpoints = htonl(rsp->info.checkpoints);

		CMD_LOG("open_time = %u,close_time = %d, curentTime = %u",rsp->open_time,rsp->close_time,ServerTime::getTime());
		CMD_LOG("currentState = %s,info.times = %d,info.energy = %d,info.hp_gain = %d,info.attack_gain = %d,info.defense_gain = %d,info.difficulty = %d,info.checkpoints = %d",
			rsp->info.b_on_the_way?"In":"Out",rsp->info.times,rsp->info.energy,rsp->info.hp_gain,rsp->info.attack_gain,rsp->info.defense_gain,rsp->info.difficulty,rsp->info.checkpoints
			);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_querySelfChallengeInfoRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithQuerySelfChallengeOrderRsp(char* head,char* body)
{
	SPCmd_QuerySelfChallengeOrderRsp* rsp = (SPCmd_QuerySelfChallengeOrderRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QuerySelfChallengeOrderRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->count = htonl(rsp->count);
		CMD_LOG("count = %d",rsp->count);
		for (int i = 0; i < rsp->count; i++)
		{
			hton_HeroInfo(rsp->order_arr[i].hero_info);
			rsp->order_arr[i].order = htonl(rsp->order_arr[i].order);
			rsp->order_arr[i].checkpoints = htonl(rsp->order_arr[i].checkpoints);
			rsp->order_arr[i].item.type = htonl(rsp->order_arr[i].item.type);
			rsp->order_arr[i].item.count = htonl(rsp->order_arr[i].item.count);

			flat_data.order_arr.push_back(rsp->order_arr[i]);

			CMD_LOG("--------\n");
			log(rsp->order_arr[i].hero_info);
			CMD_LOG("order_arr[%d] --> {order = %d,checkpoints = %d}",i,rsp->order_arr[i].order,rsp->order_arr[i].checkpoints);
		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_querySelfChallengeOrderRsp_F,MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithSelfChallengeRsp(char* head,char* body)
{
	SPCmd_SelfChallengeRsp* rsp = (SPCmd_SelfChallengeRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_SelfChallengeRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->energy = ntohl(rsp->energy);
		rsp->checkpoints = ntohl(rsp->checkpoints);
		rsp->silver = ntohl(rsp->silver);
		rsp->gift_bag.id = ntohl(rsp->gift_bag.id);
		rsp->gift_bag.count = ntohl(rsp->gift_bag.count);

		flat_data.energy = rsp->energy;
		flat_data.checkpoints = rsp->checkpoints;
		flat_data.silver = rsp->silver;
		flat_data.is_have_box = rsp->is_have_box;
		flat_data.giftBag = rsp->gift_bag;


		FightScriptBuilder fb;
		flat_data.fightScript = fb.CharArr_2_FightScriptVec(rsp->fight_script,1)[0];

		CMD_LOG("energy = %d,checkpoints=%d,silver = %d",rsp->energy,rsp->checkpoints,rsp->silver);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_selfChallengeRsp_F,MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithSelfChallengeGainRsp(char* head,char* body)
{
	SPCmd_SelfChallengeGainRsp* rsp = (SPCmd_SelfChallengeGainRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->energy = ntohl(rsp->energy);
		rsp->hp_gain = ntohl(rsp->hp_gain);
		rsp->attack_gain = ntohl(rsp->attack_gain);
		rsp->defense_gain = ntohl(rsp->defense_gain);

		CMD_LOG("energy = %d,hp_gain=%d,attack_gain = %d,defense_gain = %d",
			rsp->energy,rsp->hp_gain,rsp->attack_gain,rsp->defense_gain);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_selfChallengeGainRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithGetSelfChallengeBoxRsp(char* head,char* body)
{
	SPCmd_GetSelfChallengeBoxRsp* rsp = (SPCmd_GetSelfChallengeBoxRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->item.type = ntohl(rsp->item.type);
		rsp->item.count = ntohl(rsp->item.count);

		rsp->item_arr[0].count = ntohl(rsp->item_arr[0].count);
		rsp->item_arr[0].type = ntohl(rsp->item_arr[0].type);
		rsp->item_arr[1].count = ntohl(rsp->item_arr[1].count);
		rsp->item_arr[1].type = ntohl(rsp->item_arr[1].type);

		CMD_LOG("getted --> {type = %d,count = %d}",rsp->item.type ,rsp->item.count);
		CMD_LOG("miss1 --> {type = %d,count = %d}",rsp->item_arr[0].type ,rsp->item_arr[0].count);
		CMD_LOG("miss2 --> {type = %d,count = %d}",rsp->item_arr[1].type ,rsp->item_arr[1].count);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getSelfChallengeBoxRsp,MSG_Rsp::createMsg(rsp));
} 


void CmdHelper::dealWithQueryProtectSpiritRsp(char* head,char* body)
{
	SPCmd_QueryProtectSpiritRsp* rsp = (SPCmd_QueryProtectSpiritRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		hton_HeroInfo(rsp->first_spirit);
		CMD_LOG("first_spirit : ");
		log(rsp->first_spirit);

		rsp->current_spirit.spirit_id = ntohl(rsp->current_spirit.spirit_id);
		rsp->current_spirit.profession_id = ntohl(rsp->current_spirit.profession_id);
		rsp->current_spirit.defend_time = ntohl(rsp->current_spirit.defend_time);
		rsp->current_spirit.level = ntohl(rsp->current_spirit.level);
		CMD_LOG("current_spirit : ");
		CMD_LOG("current_spirit.spirit_id = %u,current_spirit.profession_id = %u,current_spirit.defend_time = %u,current_spirit.level = %u,name = %s,is_get = %s",
			rsp->current_spirit.spirit_id,rsp->current_spirit.profession_id,rsp->current_spirit.defend_time,rsp->current_spirit.level,rsp->current_spirit.name,rsp->current_spirit.is_get?"true":"false");
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryProtectSpiritRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithChallengeProtectSpiritRsp(char* head,char* body)
{
	SPCmd_ChallengeProtectSpiritRsp* rsp = (SPCmd_ChallengeProtectSpiritRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_ChallengeProtectSpiritRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		FightScriptBuilder fb;
		flat_data.fightScript = fb.CharArr_2_FightScriptVec(rsp->fight_script,1)[0];
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_challengeProtectSpiritRsp_F,MSG_Rsp::createMsg(&flat_data));

}
void CmdHelper::dealWithGetProtectSpiritBonusRsp(char* head,char* body)
{
	SPCmd_GetProtectSpiritBonusRsp* rsp = (SPCmd_GetProtectSpiritBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->item_count = ntohs(rsp->item_count);
		CCLOG("item count = %d", rsp->item_count);
		for (int i=0; i<rsp->item_count; ++i)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			CCLOG("item[%d], type=%d, count=%d", i, rsp->item_arr[i].type, rsp->item_arr[i].count);
		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getProtectSpiritBonusRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithWorshiProtectSpiritRsp(char* head,char* body)
{
	SPCmd_WorshiProtectSpiritRsp* rsp = (SPCmd_WorshiProtectSpiritRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->gift_bag.id = ntohl(rsp->gift_bag.id);
		rsp->gift_bag.count = ntohl(rsp->gift_bag.count);
		CCLOG("giftbag id=%d, count = %d", rsp->gift_bag.id, rsp->gift_bag.count);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_worshiProtectSpiritRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithQueryDiscoverInfoRsp(char* head,char* body)
{
	SPCmd_QueryDiscoverInfoRsp* rsp = (SPCmd_QueryDiscoverInfoRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->remainder_times = htonl(rsp->remainder_times);
		rsp->discovered_times = htonl(rsp->discovered_times);
		rsp->destroy_seal = htonl(rsp->destroy_seal);
		rsp->unlocked_seal = htonl(rsp->unlocked_seal);	
		rsp->last_modify_timestamp = htonl(rsp->last_modify_timestamp);

		CMD_LOG("remainder_times = %d,discovered_times = %d,destroy_seal = %d,unlocked_seal = %d,last_modify_timestamp = %d",
			rsp->remainder_times,rsp->discovered_times,rsp->destroy_seal,rsp->unlocked_seal,rsp->last_modify_timestamp);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryDiscoverInfoRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithDiscoverRsp(char* head,char* body)
{
	SPCmd_DiscoverRsp* rsp = (SPCmd_DiscoverRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->remainder_times = ntohl(rsp->remainder_times);
		rsp->discovered_times = ntohl(rsp->discovered_times);
		CMD_LOG("is_destroyed = %s,timesLeft = %d,discovered_times = %d",
			rsp->is_destroyed?"true":"false",rsp->remainder_times,rsp->discovered_times);

		rsp->count = ntohl(rsp->count);
		for (int i=0;i<rsp->count; i++)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			CMD_LOG("item_arr[%d]-->{type = %u,count= %u}",i,rsp->item_arr[i].type,rsp->item_arr[i].count);
		}

		rsp->last_modify_timestamp = ntohl(rsp->last_modify_timestamp);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_discoverRsp,MSG_Rsp::createMsg(rsp));

}
void CmdHelper::dealWithUnlockDiscoverSealRsp(char* head,char* body)
{
	SPCmd_UnlockDiscoverSealRsp* rsp = (SPCmd_UnlockDiscoverSealRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_UnlockDiscoverSealRsp flat_data;
	flat_data.err_code = rsp->err_code;
	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("count = %d",rsp->count);
		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->data,rsp->count,flat_data.equips);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_unlockDiscoverSealRsp_F,MSG_Rsp::createMsg(&flat_data));
}

void CmdHelper::dealWithQueryPurifyRsp(char* head,char* body)
{
	SPCmd_QueryPurifyRsp* rsp = (SPCmd_QueryPurifyRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->purified_times = ntohl(rsp->purified_times);
		rsp->purify_free_times = ntohl(rsp->purify_free_times);
		CMD_LOG("purified_times = %u, purify_free_times = %u",rsp->purified_times,rsp->purify_free_times);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryPurifyRsp,MSG_Rsp::createMsg(rsp));
}
void CmdHelper::dealWithPurifyRsp(char* head,char* body)
{
	SPCmd_PurifyRsp* rsp = (SPCmd_PurifyRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_PurifyRsp flat_data;
	flat_data.err_code = rsp->err_code;
	if (rsp->err_code == Success)
	{
		rsp->purify_hole = ntohl(rsp->purify_hole);
		CMD_LOG("count = %u",rsp->purify_hole);

		for (int i=0; i<rsp->purify_hole; ++i)
		{
			rsp->attr_arr[i].attr_type = ntohl(rsp->attr_arr[i].attr_type);
			rsp->attr_arr[i].attr_value = ntohl(rsp->attr_arr[i].attr_value);
			rsp->attr_arr[i].cuiqu_suc_ratio = ntohl(rsp->attr_arr[i].cuiqu_suc_ratio);
			CMD_LOG("attr_type = %u,attr_value = %u,cuiqu_suc_ratio = %u",
				rsp->attr_arr[i].attr_type,rsp->attr_arr[i].attr_value,rsp->attr_arr[i].cuiqu_suc_ratio);
			flat_data.zbAttrVec.push_back(rsp->attr_arr[i]);
		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_PurifyRsp,MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithConfirmPurifyRsp(char* head,char* body)
{
	SPCmd_ConfirmPurifyRsp* rsp = (SPCmd_ConfirmPurifyRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

// 	if (rsp->err_code == Success)
// 	{
// 	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ConfirmPurifyRsp,MSG_Rsp::createMsg(rsp));
}

void CmdHelper::dealWithQueryAssessWeaponRsp(char* head, char*body)
{
	SPCmd_QueryAssessWeaponRsp* rsp = (SPCmd_QueryAssessWeaponRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);

	Flat_QueryAssessWeaponRsp flat_data;
	flat_data.err_code = rsp->err_code;
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	if (rsp->err_code == Success)
	{
		ntoh_DuanzaoGroupZBInfoArrayToFlat_DuanzaoGroupZBInfoVec(
			(DuanzaoGroupZBInfo*)rsp->material_zb, 1, flat_data.eqVec);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryAssessWeaponRsp, Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::dealWithAssessWeaponRsp(char* head, char*body)
{
	SPCmd_AssessWeaponRsp* rsp = (SPCmd_AssessWeaponRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_AssessWeaponRsp flat_data;
	flat_data.err_code = rsp->err_code;
	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CCLog("Item count = %u", rsp->count);
		for (int i=0; i<rsp->count; ++i)
		{
			rsp->item_arr[i].type = ntohl(rsp->item_arr[i].type);
			rsp->item_arr[i].count = ntohl(rsp->item_arr[i].count);
			CCLog("type=%u, count=%u", rsp->item_arr[i].type, rsp->item_arr[i].count);
			flat_data.itemsVec.push_back(rsp->item_arr[i]);
		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_AssessWeaponRsp, Flat_MSG_Rsp::createMsg(&flat_data));
}


//邮件
void CmdHelper::cmdQueryEmail()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s", __FUNCTION__);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryEmail);

	SimpleProtocolCmd cmd = queryEmail;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryEmail* _data = (SPCmd_QueryEmail*)bodyChar;

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithQueryEmailRsp(char* head, char*body)
{
	SPCmd_QueryEmailRsp* rsp = (SPCmd_QueryEmailRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryEmailRsp flat_data;
	flat_data.err_code = rsp->err_code;
	Email *emailArray = NULL;
	if (rsp->err_code == Success)
	{
		int count = ntohl(rsp->count);
		emailArray = new Email[count];
		CMD_LOG("email count = %d", count);
		flat_data.emailVec.clear();
		char* nextPos = rsp->email_arr;
		unsigned int lengh = 0;
		for (int i=0; i<count; ++i)
		{
			CMD_LOG("----------------------------email_idx[%d] begin-------------------------", i);
			lengh = Email::email_ntoh(&emailArray[i], (EmailInfo*)nextPos);
			nextPos += lengh;
			flat_data.emailVec.push_back(&emailArray[i]);
			CMD_LOG("----------------------------email_idx[%d] end-------------------------", i);
		}
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryEmailRsp, Flat_MSG_Rsp::createMsg(&flat_data));
	if (emailArray) delete [] emailArray;
}
void CmdHelper::cmdQueryEmailRsp(CCObject* msg_rsp)
{
	Flat_QueryEmailRsp *data = (Flat_QueryEmailRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code != Success){
		return;
	}

	EmailManager::getManager()->resetEmailVector(&data->emailVec);
}

static unsigned int s_currReadEmailId = 0;
void CmdHelper::cmdReadEmail(unsigned int email_id)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s ---> email_id=[%u]", __FUNCTION__, email_id);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_ReadEmail);

	SimpleProtocolCmd cmd = readEmail;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	s_currReadEmailId = email_id;
	SPCmd_ReadEmail* _data = (SPCmd_ReadEmail*)bodyChar;
	_data->id = htonl(email_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithReadEmailRsp(char* head, char*body)
{
	SPCmd_ReadEmailRsp* rsp = (SPCmd_ReadEmailRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_ReadEmailRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->zhuangbei_count = ntohl(rsp->zhuangbei_count);
		CMD_LOG("zhuangbei_count = %u", rsp->zhuangbei_count);

		ZhuangbeiDetail* detailArray = (ZhuangbeiDetail*)rsp->detail;
		CS::ntoh_zhuangbei_detail_2_equipdata(detailArray,rsp->zhuangbei_count,flat_data.eqVec);
	}
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ReadEmailRsp, Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::cmdReadEmailRsp(CCObject* msg_rsp)
{
	Flat_ReadEmailRsp* flat_data = (Flat_ReadEmailRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (flat_data->err_code != Success){
		return;
	}

	Email* mail = EmailManager::getManager()->getOneEmailById(s_currReadEmailId);
	//标记
	EmailManager::getManager()->setOneEmailHasRead(s_currReadEmailId);

	//添加体力
	BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	actor->setVitality(actor->getCurrentProperty().vit_current + mail->vitality);
	// 添加装备
	for (int i=0,equipNum=flat_data->eqVec.size(); i<equipNum; i++){
		EquipmentManager::getManager()->addOneEquipment(flat_data->eqVec[i]);
	}
	//装备碎片
	for (int i=0,count=mail->shenBingSuiPianVec.size(); i<count; i++){
		SuiPianManager::getManager()->addOneShenBingSuiPian(mail->shenBingSuiPianVec.at(i));
	}
	//添加道具
	for (int i=0,count=mail->itemVec.size(); i<count; ++i){
		WuPinManager::getManager()->addWuPin((ItemsType)mail->itemVec.at(i).type, mail->itemVec.at(i).count);
	}
	//添加道具碎片
	for (int i=0,count=mail->itemSuiPianVec.size(); i<count; ++i){
		SuiPianManager::getManager()->addOneItemSuiPian(mail->itemSuiPianVec.at(i));
	}
	//礼包
	for (int i=0,count=mail->giftVec.size(); i<count; ++i){
		SystemGiftBagManager::getManager()->addOneGiftBag(mail->giftVec[i]);
	}
}

void CmdHelper::cmdWriteEmail(unsigned int receiver_id, string content)
{
	//检测 文本长度
	if (content.length() > CONTENT_MAX_LEN){
		CCLog("Error: %s ---> email content length = %d, max=%d", content.length(), CONTENT_MAX_LEN);
		POST_ERROR(CEC_EmailContentOutSize,MSG_WriteEmailRsp);
		return;
	}

	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s ---> receiver_id=[%u], countent=[%s]", __FUNCTION__, receiver_id, content.c_str());
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_WriteEmail);

	SimpleProtocolCmd cmd = writeEmail;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_WriteEmail* _data = (SPCmd_WriteEmail*)bodyChar;
	_data->receiver_id = htonl(receiver_id);
	strcpy(_data->content, content.c_str());//已检测长度。

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithWriteEmailRsp(char* head, char*body)
{
	SPCmd_WriteEmailRsp* rsp = (SPCmd_WriteEmailRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_WriteEmailRsp, MSG_Rsp::createMsg(rsp));
}
void CmdHelper::cmdWriteEmailRsp(CCObject* msg_rsp)
{
	//no thing to do
	//SPCmd_WriteEmailRsp* data = (SPCmd_WriteEmailRsp*)((MSG_Rsp*)msg_rsp)->getData();
	//if (data->err_code == Success)
	//{

	//}
}

static unsigned int s_deleteEmailId = 0;
void CmdHelper::cmdDeleteEmail(unsigned int email_id)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s ---> email_id=[%u]", __FUNCTION__, email_id);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_DeleteEmail);

	SimpleProtocolCmd cmd = deleteEmail;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	s_deleteEmailId = email_id;
	SPCmd_DeleteEmail* _data = (SPCmd_DeleteEmail*)bodyChar;
	_data->id = htonl(email_id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithDeleteEmailRsp(char* head, char*body)
{
	SPCmd_DeleteEmailRsp* rsp = (SPCmd_DeleteEmailRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_DeleteEmailRsp, MSG_Rsp::createMsg(rsp));
}
void CmdHelper::cmdDeleteEmailRsp(CCObject* msg_rsp)
{
	SPCmd_DeleteEmailRsp* rsp = (SPCmd_DeleteEmailRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success){
		return;
	}

	//本地数据删除
	EmailManager::getManager()->deleteOneEmail(s_deleteEmailId);
}

void CmdHelper::dealWithPushEmail(char* head, char*body)
{
	SPCmd_PushEmail* rsp = (SPCmd_PushEmail*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_PushEmailRsp flat_data;
	flat_data.err_code = rsp->err_code;
	if (flat_data.err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("email count = %d", rsp->count);
		flat_data.emailVec.clear();
		char* nextPos = rsp->email_arr;
		unsigned int lengh = 0;
		for (int i=0; i<rsp->count; ++i)
		{
			CMD_LOG("----------------------------email_idx[%d] begin-------------------------", i);
			Email email;
			lengh = Email::email_ntoh(&email, (EmailInfo*)nextPos);
			nextPos += lengh;
			flat_data.emailVec.push_back(&email);
			CMD_LOG("----------------------------email_idx[%d] end-------------------------", i);
		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_SystemEmailPushRsp, Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::pushEmailRsp(CCObject *msg_rsp)
{
	Flat_PushEmailRsp *data = (Flat_PushEmailRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code == Success)
	{
        int count = data->emailVec.size();
        CCLog("%d", count);
		for (int i=0; i<count; ++i)
		{
            //IOS上直接addOneEmail(data->emailVec.at(i))会崩溃，所以先取出来，然后再传参数
            Email* email = data->emailVec.at(i);
			EmailManager::getManager()->addOneEmail(email);
		}
	}
}

//系统礼包
void CmdHelper::cmdQuerySystemGiftBag()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s", __FUNCTION__);
	removeLastBuff();


	//缓存
	if (! SystemGiftBagManager::getManager()->getAllGiftBag()->empty()){
		Flat_GetSysTemGiftBagRsp flat_data;
		flat_data.err_code = Success;
		flat_data.vitality = 0;
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_GetSystemGiftBagRsp, Flat_MSG_Rsp::createMsg(&flat_data));
	}


	unsigned int dataLength = sizeof(SPCmd_QueryGiftBag);

	SimpleProtocolCmd cmd = queryGiftBag;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryGiftBag* _data = (SPCmd_QueryGiftBag*)bodyChar;
	_data->stub = htonl(0);//无用参数

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithQuerySystemGiftBagRsp(char* head, char*body)
{
	SPCmd_QueryGiftBagRsp* rsp = (SPCmd_QueryGiftBagRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_QueryGiftBagRsp flat_data;
	flat_data.err_code = rsp->err_code;
	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("gift bag count = %d", rsp->count);
		GiftBag* pGift = (GiftBag*)rsp->gift_bag_arr;
		for(int i=0; i<rsp->count; ++i)
		{
			pGift[i].id = ntohl(pGift[i].id);
			pGift[i].count = ntohl(pGift[i].count);
			flat_data.giftBagVec.push_back(pGift[i]);
			CMD_LOG("giftId[%u], count[%u]", pGift[i].id, pGift[i].count);
		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QuerySystemGiftBagRsp, Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::cmdQuerySystemGiftBagRsp(CCObject* msg_rsp)
{
	Flat_QueryGiftBagRsp* data = (Flat_QueryGiftBagRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code == Success)
	{
		SystemGiftBagManager::getManager()->resetGiftBagList(data->giftBagVec);
	}
}
static unsigned int s_getSystemGiftBagId = 0;
void CmdHelper::cmdGetSystemGiftBag(unsigned int id)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s ---> giftId=[%u]", __FUNCTION__, id);

	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_GetGiftBag);

	SimpleProtocolCmd cmd = getGiftBag;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	s_getSystemGiftBagId = id;
	SPCmd_GetGiftBag* _data = (SPCmd_GetGiftBag*)bodyChar;
	_data->id = htonl(id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}

void CmdHelper::ntoh_GiftBagInfo(GiftBagInfo& info, Flat_GetSysTemGiftBagRsp &flat_data)
{
	//体力
	flat_data.vitality = ntohl(info.vitality);
	CMD_LOG("vitality[%u]", flat_data.vitality);
	//道具
	unsigned int itemCount = ntohl( info.item_count);
	CMD_LOG("itemCount[%u]", itemCount);
	for (int i=0; i<itemCount; ++i)
	{
		 info.item_arr[i].type = ntohl( info.item_arr[i].type);
		 info.item_arr[i].count = ntohl( info.item_arr[i].count);
		CMD_LOG("item[%d] type[%u], count[%u]", i,  info.item_arr[i].type,  info.item_arr[i].count);
		flat_data.itemVec.push_back( info.item_arr[i]);
	}
	const char* pData = (char*)& info.item_arr[itemCount];
	//道具碎片
	unsigned int itemDebrisCount = ntohl(*(unsigned int*)pData);
	pData += sizeof(unsigned int);
	CMD_LOG("item debris count = %d", itemDebrisCount);
	ItemSuipianInfo* pItemSuipian = (ItemSuipianInfo*)pData;
	for (int i=0; i<itemDebrisCount; ++i)
	{
		pItemSuipian[i].item_id = ntohl(pItemSuipian[i].item_id);
		pItemSuipian[i].suipian_count = ntohl(pItemSuipian[i].suipian_count);
		pItemSuipian[i].suipian_id = ntohl(pItemSuipian[i].suipian_id);
		CMD_LOG("itemDebris[%d] itemId[%u], suipianId[%u], count[%u]", 
			i, pItemSuipian[i].item_id, pItemSuipian[i].suipian_id, pItemSuipian[i].suipian_count);
		flat_data.itemDebrisVec.push_back(pItemSuipian[i]);
	}
	pData = (char*)&pItemSuipian[itemDebrisCount];
	//装备碎片
	unsigned int eqSuipianCount = ntohl(*(unsigned int*)pData);
	pData += sizeof(unsigned int);
	CMD_LOG("equip debris count = %d", eqSuipianCount);
	ShenbingSuipianInfo* pEquipSuipian = (ShenbingSuipianInfo*)pData;
	for (int i=0; i<eqSuipianCount; ++i)
	{
		pEquipSuipian[i].zb_group_id = ntohl(pEquipSuipian[i].zb_group_id);
		pEquipSuipian[i].suipian_id = ntohl(pEquipSuipian[i].suipian_id);
		pEquipSuipian[i].suipian_count = ntohl(pEquipSuipian[i].suipian_count);
		CMD_LOG("equipmentDebris[%d] groupId[%u], suipianId[%u], count[%u]", 
			i, pEquipSuipian[i].zb_group_id, pEquipSuipian[i].suipian_id, pEquipSuipian[i].suipian_count);
		flat_data.shenbingDebrisVec.push_back(pEquipSuipian[i]);
	}
	pData = (char*)&pEquipSuipian[eqSuipianCount];
	//礼包
	unsigned int giftBagCount = ntohl(*(unsigned int*)pData);
	pData += sizeof(unsigned int);
	CMD_LOG("gift Bag Count = %d", giftBagCount);
	GiftBag* pGiftBag = (GiftBag*)pData;
	for (int i=0; i<giftBagCount; ++i)
	{
		pGiftBag[i].id = ntohl(pGiftBag[i].id);
		pGiftBag[i].count = ntohl(pGiftBag[i].count);
		CMD_LOG("giftBag[%d] id[%u], count[%u]", i, pGiftBag[i].id, pGiftBag[i].count);
		flat_data.giftBagVec.push_back(pGiftBag[i]);
	}
	pData = (char*)&pGiftBag[giftBagCount];
	//装备
	unsigned int equipCount = ntohl(*(unsigned int*)pData);
	pData += sizeof(unsigned int);
	CMD_LOG("equip Count = %d", equipCount);
	ZhuangbeiDetail* pEquips = (ZhuangbeiDetail*)pData;
	CS::ntoh_zhuangbei_detail_2_equipdata(pEquips,equipCount,flat_data.equipVec);
	for (int i=0;i<equipCount;i++) log(flat_data.equipVec[i]);

	CMD_LOG("------------------------gift bag end-----------------------");
}
void CmdHelper::dealWithGetSystemGiftBagRsp(char* head, char*body)
{
	SPCmd_GetGiftBagRsp* rsp = (SPCmd_GetGiftBagRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s---> erroCode = %d",__FUNCTION__,rsp->err_code);

	Flat_GetSysTemGiftBagRsp flat_data;
	flat_data.err_code = rsp->err_code;
	if (rsp->err_code == Success)
	{
		CMD_LOG("------------------------gift bag begin-----------------------");
		ntoh_GiftBagInfo(rsp->info, flat_data);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_GetSystemGiftBagRsp, Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::cmdGetSystemGiftBagRsp(CCObject* msg_rsp)
{
	Flat_GetSysTemGiftBagRsp * data = (Flat_GetSysTemGiftBagRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code != Success){
		return;
	}

	SystemGiftBagManager::getManager()->subOneGiftBag(s_getSystemGiftBagId);
	//体力
	BaseActor* zhujue = MyselfManager::getManager()->getMyZhuJueData();
	zhujue->setVitality(zhujue->getCurrentProperty().vit_current + data->vitality);
	//礼包
	for (int i=0, count=data->giftBagVec.size(); i<count; ++i){
		SystemGiftBagManager::getManager()->addOneGiftBag(data->giftBagVec[i]);
	}
	//道具
	for (int i=0, count=data->itemVec.size(); i<count; ++i){
		WuPinManager::getManager()->addWuPin((ItemsType)data->itemVec[i].type, data->itemVec[i].count);
	}
	//道具碎片
	for (int i=0, count=data->itemDebrisVec.size(); i<count; ++i){
		SuiPianManager::getManager()->addOneItemSuiPian(data->itemDebrisVec[i]);
	}
	//装备碎片
	for (int i=0, count=data->shenbingDebrisVec.size(); i<count; ++i){
		SuiPianManager::getManager()->addOneShenBingSuiPian(data->shenbingDebrisVec[i]);
	}
	//装备
	for (int i=0, count=data->equipVec.size(); i<count; ++i){
		EquipmentManager::getManager()->addOneEquipment(data->equipVec[i]);
	}
}

//系统奖励、等级奖励、登录奖励、任务  bonus
void CmdHelper::cmdQueryBonus(BonusType type)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s ---> query type=[%d]", __FUNCTION__, type);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryBonus);

	SimpleProtocolCmd cmd = queryBonus;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryBonus* _data = (SPCmd_QueryBonus*)bodyChar;
	_data->type = htonl(type);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithQueryBonusRsp(char* head,char* body)
{
	SPCmd_QueryBonusRsp *rsp = (SPCmd_QueryBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s ---> error code=[%u]", __FUNCTION__, rsp->err_code);

	Flat_QueryBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;
	if (rsp->err_code == Success)
	{
		CMD_LOG("-----------------------------bonus begin-------------------------------------");
		int allcount = 0;
		for (int i=0; i<4; ++i)
		{
			rsp->type[i] = ntohl(rsp->type[i]);
			rsp->count[i] = ntohl(rsp->count[i]);
			CMD_LOG("type=[%u], cont=[%d]", rsp->type[i], rsp->count[i]);
			allcount += rsp->count[i];
		}
		flat_data.bonus_arr = BonusInfoArrayToFlat_BonusInfoVec((BonusInfo*)rsp->bonus_arr, allcount);
		CMD_LOG("-----------------------------bonus end-------------------------------------");
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_queryBonusRsp, Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::cmdQueryBonusRsp(CCObject* msg_rsp)
{
	Flat_QueryBonusRsp *data = (Flat_QueryBonusRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code == Success){
		BonusManager::getManager()->setBonus(data->bonus_arr);
	}
}
void CmdHelper::cmdGetBonus(BonusType type, unsigned int id)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s ---> type=[%d], id=[%u]", __FUNCTION__, type, id);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_GetBonus);

	SimpleProtocolCmd cmd = getBonus;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_GetBonus* _data = (SPCmd_GetBonus*)bodyChar;
	_data->type = htonl(type);
	_data->id = htonl(id);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithGetBonusRsp(char* head,char* body)
{
	SPCmd_GetBonusRsp *rsp = (SPCmd_GetBonusRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s ---> error code=[%u]", __FUNCTION__, rsp->err_code);

	Flat_GetBonusRsp flat_data;
	flat_data.err_code = rsp->err_code;
	if (rsp->err_code == Success)
	{
		flat_data.type = (BonusType)ntohl(rsp->type);
		flat_data.id = ntohl(rsp->id);
		unsigned int count = ntohl(rsp->zb_count);
		CMD_LOG("bonus type = %u, id = %u, zb_count = %u", flat_data.type, flat_data.id, count);

		CS::ntoh_zhuangbei_detail_2_equipdata((ZhuangbeiDetail*)rsp->detail,count,flat_data.gettedEquip);
#if DATAPOOL_LOG
		for (int i=0;i<count;i++){
			log(flat_data.gettedEquip[i]);
		}
#endif
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_getBonusRsp, Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::cmdGetBonusRsp(CCObject* msg_rsp)
{
	Flat_GetBonusRsp* data = (Flat_GetBonusRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code == Success){
		Flat_BonusInfo bonusInfo;
		bool hasFind = BonusManager::getManager()->getBonus(data->type, data->id, bonusInfo);
		if (hasFind && !bonusInfo.is_get && bonusInfo.enable){
			BonusManager::getManager()->setIsGet(data->type, data->id);

			int multiple = 1; //奖励翻倍
			//type为BonusType_DayWelfare或BonusType_Reward 有以下类容
			if (data->type == BonusType_DayWelfare || data->type == BonusType_Reward){
				if (bonusInfo.vip_level > 0){
					multiple = bonusInfo.multiple;
					//vip 
					if (MyselfManager::getManager()->getMyZhuJueData()->getVipLevel() < bonusInfo.vip_level){
						MyselfManager::getManager()->getMyZhuJueData()->setVipLevel(bonusInfo.vip_level);
					}
					
				}
			}

			//体力 * multiple
			BaseActor *zhujue = MyselfManager::getManager()->getMyZhuJueData();
			zhujue->setVitality(zhujue->getCurrentProperty().vit_current + bonusInfo.vitality*multiple);
			//item * multiple
			WuPinManager* pWupin = WuPinManager::getManager();
			for (int i=0,count=bonusInfo.itemVec.size(); i<count; ++i){
				pWupin->addWuPin((ItemsType)bonusInfo.itemVec[i].type, bonusInfo.itemVec[i].count*multiple);
			}
			//item debris  *  multiple
			SuiPianManager* pSuiPian = SuiPianManager::getManager();
			ItemSuipianInfo itemDebrisTemp;
			for (int i=0,count=bonusInfo.itemDebrisVec.size(); i<count; ++i){
				memcpy(&itemDebrisTemp, &bonusInfo.itemDebrisVec[i], sizeof(ItemSuipianInfo));
				itemDebrisTemp.suipian_count *= multiple;
				pSuiPian->addOneItemSuiPian(itemDebrisTemp);
			}
			//equip debris *  multiple
			ShenbingSuipianInfo eqDebrisTemp;
			for (int i=0,count=bonusInfo.equipDebrisVec.size(); i<count; ++i){
				memcpy(&eqDebrisTemp, &bonusInfo.equipDebrisVec[i], sizeof(ShenbingSuipianInfo));
				eqDebrisTemp.suipian_count *= multiple;
				pSuiPian->addOneShenBingSuiPian(eqDebrisTemp);
			}
			//equip //服务器已处理multiple
			EquipmentManager* pEquip = EquipmentManager::getManager();
			for (int i=0,count=data->gettedEquip.size(); i<count; ++i){
				pEquip->addOneEquipment(data->gettedEquip[i]);
			}
			//gift * multiple
			SystemGiftBagManager* pGiftBag = SystemGiftBagManager::getManager();
			GiftBag giftTemp;
			for (int i=0,count=bonusInfo.giftBagVec.size(); i<count; ++i){
				memcpy(&giftTemp, &bonusInfo.giftBagVec[i], sizeof(GiftBag));
				giftTemp.count *= multiple;
				pGiftBag->addOneGiftBag(giftTemp);
			}
		}
		else {
			//领奖返回消息 ：在pool里必须找到该id、该奖励必须未领取、必须能领取
			CCAssert(false,"");
		}
	}
}
//系统推送
void CmdHelper::delWithSystemPushBonusRsp(char* head,char* body)
{
	SPCmd_SendBonusNotify* rsp = (SPCmd_SendBonusNotify*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s ---> error code = %u", __FUNCTION__, rsp->err_code);
	if (rsp->err_code == Success){
		rsp->id = ntohl(rsp->id);
		rsp->type = ntohl(rsp->type);
		CMD_LOG("new bonus notify , id=%u, type=%u", rsp->id, rsp->type);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_SystemPushBonusRsp, MSG_Rsp::createMsg(rsp));
}
void CmdHelper::cmdSystemPushBonusRsp(CCObject* msg_rsp)
{
	SPCmd_SendBonusNotify *data = (SPCmd_SendBonusNotify*)((MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code == Success)
	{
		BonusManager::getManager()->setOneBonusEnable((BonusType)data->type, data->id);
	}
}

//图鉴掉落查询
void CmdHelper::cmdQueryEquipFall()
{
	//已经查回来了就不用查了
	if (EquipFallManager::getManager()->getAlbumEquipFallVec().size() > 0){
		Flat_QueryEquipFallRsp flat_data;
		flat_data.err_code = Success;
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryEquipFallRsp, Flat_MSG_Rsp::createMsg(&flat_data));
		return;
	}


	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s", __FUNCTION__);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryOrigin);

	SimpleProtocolCmd cmd = queryOrigin;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryOrigin* _data = (SPCmd_QueryOrigin*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithQueryEquipFallRsp(char* head, char*body)
{
	// dealwith package
	SPCmd_QueryOriginRsp *rsp = (SPCmd_QueryOriginRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s ---> error code = %u", __FUNCTION__, rsp->err_code);

	Flat_QueryEquipFallRsp flat_data;
	flat_data.err_code = rsp->err_code;

	if (rsp->err_code == Success)
	{
		rsp->count = ntohl(rsp->count);
		CMD_LOG("count = %u", rsp->count);
		for (int i=0; i<rsp->count; ++i)
		{
			rsp->origin_arr[i].group_id = ntohl(rsp->origin_arr[i].group_id);
			rsp->origin_arr[i].task_id = ntohl(rsp->origin_arr[i].task_id);
			CMD_LOG("equip fall [%d] : groupId=%u, taskId=%u", i, rsp->origin_arr[i].group_id, rsp->origin_arr[i].task_id);
			flat_data.fallVec.push_back(rsp->origin_arr[i]);
		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryEquipFallRsp, Flat_MSG_Rsp::createMsg(&flat_data));
}
void CmdHelper::cmdQueryEquipFallRsp(CCObject* msg_rsp)
{
	Flat_QueryEquipFallRsp *data = (Flat_QueryEquipFallRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code == Success && EquipFallManager::getManager()->getAlbumEquipFallVec().size() == 0) //查过了就不用查了
	{
		EquipFallManager::getManager()->resetAlbumEquipFall(data->fallVec);
	}
}


//20140617 查询主角其他信息
void CmdHelper::cmdQueryHeroOtherInfo()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s", __FUNCTION__);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryHeroOtherInfo);

	SimpleProtocolCmd cmd = queryHeroOtherInfo;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryHeroOtherInfo* _data = (SPCmd_QueryHeroOtherInfo*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithQueryHeroOtherInfoRsp(char* head, char*body)
{
	SPCmd_QueryHeroOtherInfoRsp* rsp = (SPCmd_QueryHeroOtherInfoRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s ---> error code = %u", __FUNCTION__, rsp->err_code);

	if (rsp->err_code == Success)
	{
		rsp->toplist_order = ntohl(rsp->toplist_order);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryHeroOtherInfoRsp, MSG_Rsp::createMsg(rsp));
}
void CmdHelper::cmdQueryHeroOtherInfoRsp(CCObject* msg_rsp)
{
	//no thing 
}

//20140618 查询公告
void CmdHelper::cmdQuerySystemNotifyInfo()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s", __FUNCTION__);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryNotice);

	SimpleProtocolCmd cmd = queryNotice;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryNotice* _data = (SPCmd_QueryNotice*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithSystemNotifyInfoRsp(char* head, char*body)
{
	SPCmd_QueryNoticeRsp* rsp = (SPCmd_QueryNoticeRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s ---> err_code = %u", __FUNCTION__, rsp->err_code);

	Flat_SystempNotifyInfoRsp flatData;
	flatData.err_code = rsp->err_code;
	if (rsp->err_code == Success)
	{
		//更新
// 		for ( int i=0; i<updateCount; ++i)
// 		{
// 			flatData.updateInfoVec.push_back(pData);
// 			CMD_LOG("update[%d] = %d", i, pData);
// 			pData += flatData.updateInfoVec[i].length() + 1;
// 		}
		//活动
		unsigned short activity_count = ntohs(rsp->info.activity_count);
		CMD_LOG("activity_count = %d", activity_count);
		const char* pData = (char*)rsp->info.activity_arr;
		for (int i=0; i<activity_count; ++i)
		{
			Flat_SystempNotifyInfoRsp::strACTIVITY temp;
			temp.type = ntohs(*(unsigned short*)pData);
			pData += sizeof(unsigned short);
			temp.title = pData;
			pData += temp.title.length() + 1;
			temp.time = pData;
			pData += temp.time.length() + 1;
			temp.content = pData;
			pData += temp.content.length() + 1;
			CMD_LOG("type = %d, tilte = %s, time = %s, content = %s", temp.type, temp.title.c_str(), temp.time.c_str(), temp.content.c_str());
			flatData.activityVec.push_back(temp);
		}
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_QueryNotifyInfoRsp, MSG_Rsp::createMsg(&flatData));
}
void CmdHelper::cmdQuerySystemNotifyInfoRsp(CCObject* msg_rsp)
{
	//no thing
}

//教学状态位
void CmdHelper::cmdGetGuideState()
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s", __FUNCTION__);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_QueryInstruction);

	SimpleProtocolCmd cmd = queryInstruction;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_QueryInstruction* _data = (SPCmd_QueryInstruction*)bodyChar;
	_data->stub = htonl(0);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithGetGuideStateRsp(char* head, char*body)
{
	SPCmd_QueryInstructionRsp * rsp = (SPCmd_QueryInstructionRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s ---> err_code = %u", __FUNCTION__, rsp->err_code);

	if (rsp->err_code == Success){
		rsp->progress = LF::lf_htonll(rsp->progress);
		CMD_LOG("process = %llu", rsp->progress);
	}

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_GetGuideStateRsp, MSG_Rsp::createMsg(rsp));
}
void CmdHelper::cmdGetGuideStateRsp(CCObject* msg_rsp)
{
	SPCmd_QueryInstructionRsp* data = (SPCmd_QueryInstructionRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code == Success){
		 GameGuideManager::getManager()->setGuideProcess(data->progress);
	}
}
void CmdHelper::cmdSetGuideState(unsigned long long process)
{
	GameDirector::getDirector()->showWaiting();
	CMD_LOG("%s", __FUNCTION__);
	removeLastBuff();

	unsigned int dataLength = sizeof(SPCmd_CompleteInstruction);

	SimpleProtocolCmd cmd = completeInstruction;
	char* head = newHead(cmd,dataLength);
	char* bodyChar = new char[dataLength];

	SPCmd_CompleteInstruction* _data = (SPCmd_CompleteInstruction*)bodyChar;
	_data->progress = LF::lf_htonll(process);

	sendByteDataToServer(head,sizeof(SPHead),bodyChar,dataLength);

	// 先缓冲，不要删除
	lastCmd.head = head;
	lastCmd.body = bodyChar;
	lastCmd.cmd = cmd;
}
void CmdHelper::dealWithSetGuideStateRsp(char* head, char*body)
{
	GameDirector::getDirector()->hideWaiting();
	SPCmd_CompleteInstructionRsp* rsp = (SPCmd_CompleteInstructionRsp*)body;
	rsp->err_code = ntohl(rsp->err_code);
	CMD_LOG("%s ---> err_code = %d", __FUNCTION__, rsp->err_code);

	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_SetGuideStateRsp, MSG_Rsp::createMsg(rsp));
}
void CmdHelper::cmdSetGuideStateRsp(CCObject* msg_rsp)
{
	SPCmd_CompleteInstructionRsp* data = (SPCmd_CompleteInstructionRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code == Success)
	{
	}
}