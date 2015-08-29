#ifndef __MsgDefine__
#define __MsgDefine__
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d::extension;

class Skill;
class BaseActor;

// 攻击消息
#define		MSG_AttackedByNormal						"runAttackedNormalAction"
#define		MSG_AttackedByNormalBaoJi					"runAttackedNormalBaoJiAction"
#define		MSG_AttackedBySkill							"runAttackedSkillAction"
#define		MSG_AttackedBySkillBaoJi					"runAttackedSkillBaoJiAction"
#define		MSG_SKILL_MISS								"SkillMiss"
#define		MSG_SKILL_OK								"SkillOK"
#define		MSG_ItemDead								"ClientChessBoardItemDead"	// item 死亡了就发送消息

// 网络消息
#define		MSG_NetworkConnError						"NetworkConnError"		// 链接失败
#define		MSG_NetworkConnSuc							"NetworkConnSuc"		// 链接成功
#define		MSG_sendDataError							"sendDataError"			// 发送数据失败
#define		MSG_recvDataError							"recvDataError"			// 接受数据失败

// 命令消息
#define		MSG_clientLoginRsp							"clientLoginRsp"
#define		MSG_reconnectRsp							"reconnectRsp"
#define		MSG_clientRegistRsp							"clientRegistRsp"
#define		MSG_createHeroRsp							"createHeroRsp"
#define		MSG_createAssitRsp							"createAssitRsp"
#define		MSG_getHerosInTeamRsp						"getHerosInTeamRsp"
#define		MSG_getZhuangbeiInTeamRsp_F					"getZhuangbeiInTeamRsp_F"
#define		MSG_equipZhuangbeiRsp						"equipZhuangbeiRsp"
#define		MSG_takeOffZhuangbeiRsp						"takeOffZhuangbeiRsp"
#define		MSG_adjustFormationRsp						"adjustFormationRsp"
#define		MSG_getTaskListRsp							"getTaskListRsp"
#define		MSG_getItemsInTeamRsp						"getItemsInTeamRsp"
#define		MSG_ZBQianghuaRsp							"ZBQianghuaRsp"
#define		MSG_ZBZhuankongRsp							"ZBZhuankongRsp"
#define		MSG_ZBQianghuaHuituiRsp						"ZBQianghuaHuituiRsp"
#define		MSG_ZBChaijieRsp							"ZBChaijieRsp"
#define		MSG_ZBXiangBaoshiRsp						"ZBXiangBaoshiRsp"
#define		MSG_ZBChaiBaoshiRsp							"ZBChaiBaoshiRsp"
#define		MSG_BaoshiHechengRsp						"BaoshiHechengRsp"
#define		MSG_getMarketInfoRsp						"getMarketInfoRsp"
#define		MSG_buyItemInMarketRsp						"buyItemInMarketRsp"
#define		MSG_BattleRsp								"BattleRsp"
#define		MSG_GetTaskChessBoardRsp					"GetTaskChessBoardRsp"
#define		MSG_ChangeTaskStatRsp_F						"ChangeTaskStatRsp"
#define		MSG_restoreVitalityRsp						"restoreVitalityRsp"
#define		MSG_equipTeamItemRsp						"equipTeamItemRsp"	
#define		MSG_beKickOutRsp							"beKickOutRsp"	
#define		MSG_heartBeatRsp							"heartBeatRsp"	
#define		MSG_arenaEnterRsp_F							"arenaEnterRsp_F"	
#define		MSG_arenaUpdateRoomRsp_F					"arenaUpdateRoomRsp_F"	
#define		MSG_arenaGetBattleScriptAndGiftRsp_F		"arenaGetBattleScriptAndGiftRsp_F"	
#define		MSG_arenaGetGiftRsp							"arenaGetGiftRsp"	
#define		MSG_teamBossEnterRsp_F						"teamBossEnterRsp_F"	
#define		MSG_teamBossUpdateRoomRsp_F					"teamBossUpdateRoomRsp_F"	
#define		MSG_teamBossGetBattleScriptAndGiftRsp_F		"teamBossGetBattleScriptAndGiftRsp_F"	
#define		MSG_teamBossGetGiftRsp						"teamBossGetGiftRsp"	
#define		MSG_cityGuardEnterRsp						"cityGuardEnterRsp"	
#define		MSG_cityGuardGetNextMonsterRsp				"cityGuardGetNextMonsterRsp"	
#define		MSG_cityGuardFightRsp_F						"cityGuardFightRsp_F"	
#define		MSG_queryAnotherTeamInfoRsp_F				"queryAnotherTeamInfoRsp_F"	
#define		MSG_topHeroGetListRsp_F						"topHeroGetListRsp_F"	
#define		MSG_topHeroChallengeRsp_F					"topHeroChallengeRsp_F"
#define		MSG_systemGiftQueryRsp_F					"systemGiftQueryRsp_F"	// 查询系统奖励
#define		MSG_systemGiftGetGiftRsp					"systemGiftGetGiftRsp"
#define		MSG_chargeVitalityRsp						"chargeVitalityRsp"
#define		MSG_DuanzaoListQueryRsp						"DuanzaoListQueryRsp"
#define		MSG_DuanzaoRsp_F							"DuanzaoRsp_F"
#define		MSG_DuanzaoGroupZBQueryRsp_F				"DuanzaoGroupZBQueryRsp_F"
#define		MSG_QueryAllSuitNameRsp						"QueryAllSuitNameRsp"
#define		MSG_ZBCuiquRsp								"ZBCuiquRsp"
#define		MSG_ZBFumoRsp								"ZBFumoRsp"
#define		MSG_QueryAllMofaJinghuaRsp					"QueryAllMofaJinghuaRsp"
#define		MSG_QueryFubenDiaoluoRsp_F					"QueryFubenDiaoluoRsp_F"
#define		MSG_NewAssistantJoinRsp_F					"NewAssistantJoinRsp_F"
#define		MSG_QuerySubTaskListRsp_F					"querySubTaskListRsp_F"
#define		MSG_newSubTaskGotRsp						"newSubTaskGotRsp"
#define		MSG_subTaskSubmitedRsp						"subTaskSubmitedRsp"
#define		MSG_QueryMainTaskRewardRsp_F				"QueryMainTaskRewardRsp_F"
#define		MSG_ChargeGoldRsp							"ChargeGoldRsp"
#define		MSG_QueryTeamSkillListRsp					"QueryTeamSkillListRsp"
#define		MSG_EquipSkillRsp							"EquipSkillRsp"
#define		MSG_ActorLearnSkillRsp						"ActorLearnSkillRsp"
#define		MSG_QueryAllShenbingListRsp_F				"QueryAllShenbingListRsp"
#define		MSG_QueryTeamShenbingSuipianRsp				"QueryTeamShenbingSuipianRsp"
#define		MSG_ShenbingHechengRsp_F					"ShenbingHechengRsp"
#define		MSG_QueryTeamItemSuipianRsp					"QueryTeamItemSuipianRsp"
#define		MSG_ItemHechengRsp							"ItemHechengRsp"
#define		MSG_saodangFubenRsp_F						"saodangFubenRsp"
#define		MSG_heroLevelUpRsp							"heroLevelUpRsp"
#define		MSG_heroUpgradeRsp							"heroUpgradeRsp"
#define		MSG_shenbingJieFengyinRsp					"shenbingJieFengyinRsp"
#define		MSG_shenbingJiandingRsp						"shenbingJiandingRsp"
#define		MSG_ZBJinglianRsp							"zbJinglianRsp"
#define		MSG_shenbingZhuankeJingwenRsp				"shenbingZhuankeJingwenRsp"
#define		MSG_queryArenaTeamBasicInfoRsp				"queryArenaTeamBasicInfoRsp"
#define		MSG_queryArenaMarketRsp_F					"queryArenaMarketRsp_F"
#define		MSG_refreshArenaChlgList_F					"refreshArenaChlgList"
#define		MSG_queryArenaChlgListRsp_F					"queryArenaChlgListRsp"
#define		MSG_arenaChallengeRsp_F						"arenaChallengeRsp_F"
#define		MSG_arenaMarketBuyRsp						"arenaMarketBuyRsp"
#define		MSG_arenaBuyChlgTimeRsp						"arenaBuyChlgTimeRsp"
#define		MSG_queryZahuopuRsp							"queryZahuopuRsp"
#define		MSG_refreshZahuopuRsp						"refreshZahuopuRsp"
#define		MSG_zahuopuGetOneItemRsp_F					"zahuopuGetOneItemRsp"
#define		MSG_queryHellTaskHeroInfoRsp				"queryHellTaskHeroInfoRsp"
#define		MSG_hellTaskBattleRsp_F						"hellTaskBattleRsp_F"
#define		MSG_queryYuanshenOfHeroRsp_F				"queryYuanshenOfHeroRsp_F"
#define		MSG_churchLevelUpRsp						"churchLevelUpRsp"
#define		MSG_skillLevelUpRsp							"skillLevelUpRsp"
#define		MSG_getZhuangbeiAlbumRsp_F					"getZhuangbeiAlbumRsp_F"
#define		MSG_cityGuardProcessNotifyRsp_F				"cityGuardProcessNotify"
#define		MSG_getCityGuardCurrentStatRsp				"getCityGuardCurrentStatRsp"
#define		MSG_cityGuardBattleRsp_F					"cityGuardBattleRsp_F"
#define		MSG_getCityGuardLastDayReportRsp_F			"getCityGuardLastDayReportRsp_F"
#define		MSG_getMyCityGuardRewardRsp					"getMyCityGuardRewardRsp"
//20140613 奖励系统变更（奖励与悬赏任务合并）
// #define		MSG_queryDailyLoginBonusRsp_F				"queryDailyLoginBonusRsp_F"
// #define		MSG_queryLevelBonusRsp_F					"queryLevelBonusRsp_F"
// #define		MSG_querySystemBonusRsp_F					"querySystemBonusRsp_F"
// #define		MSG_getDailyLoginBonusRsp_F					"getDailyLoginBonusRsp_F"
// #define		MSG_getLevelBonusRsp_F						"getLevelBonusRsp_F"
// #define		MSG_getSystemBonusRsp_F						"getSystemBonusRsp_F"
#define		MSG_queryBonusRsp							"queryBonusRsp"
#define		MSG_getBonusRsp								"getBonusRsp"
#define		MSG_SystemPushBonusRsp						"SystemPushBonusRsp"

#define		MSG_getChessRandomBoxRsp					"getChessRandomBoxRsp"
#define		MSG_queryRateTaskBonus_F					"queryRateTaskBonus_F"
#define		MSG_queryRateTaskRsp_F						"queryRateTaskRsp_F"
#define		MSG_getRateTaskBonusRsp						"getRateTaskBonusRsp"
#define		MSG_queryVipPrivilegeRsp					"queryVipPrivilegeRsp"
#define		MSG_upgradeShenbingRsp_F					"upgradeShenbingRsp"
#define		MSG_queryChargePacketRsp_F					"queryChargePacketRsp"
#define		MSG_unlockAssistantRsp						"unlockAssistantRsp"
#define		MSG_queryGiftActivityRsp_F					"queryGiftActivityRsp"
#define		MSG_getGiftActivityRsp						"getGiftActivityRsp"
#define		MSG_queryKilledMonsterRsp					"queryKilledMonsterRsp"
#define		MSG_QueryKilledMonsterBonusRsp_F			"QueryKilledMonsterBonusRsp"
#define		MSG_getKilledMonsterBonusRsp				"getKilledMonsterBonusRsp"
#define		MSG_TopArenaQueryGuessRsp					"TopArenaQueryGuessRsp"	
#define		MSG_topArenaQueryStatusRsp					"topArenaQueryStatusRsp"	
#define		MSG_topArenaGuessRsp						"topArenaGuessRsp"	
#define		MSG_topArenaReplay_F						"topArenaReplay"	
#define		MSG_CastSilverRsp							"CastSilverRsp"
#define		MSG_queryLegendNoteRsp_F					"queryLegendNoteRsp"
#define		MSG_challengeLegendNoteRsp_f				"challengeLegendNoteRsp"
#define		MSG_querySelfChallengeInfoRsp				"querySelfChallengeInfoRsp"
#define		MSG_querySelfChallengeOrderRsp_F			"querySelfChallengeOrderRsp_F"
#define		MSG_selfChallengeRsp_F						"selfChallengeRsp_F"
#define		MSG_selfChallengeGainRsp					"selfChallengeGainRsp"
#define		MSG_getSelfChallengeBoxRsp					"getSelfChallengeBoxRsp"
#define		MSG_queryProtectSpiritRsp					"queryProtectSpiritRsp"
#define		MSG_challengeProtectSpiritRsp_F				"challengeProtectSpiritRsp_F"
#define		MSG_getProtectSpiritBonusRsp				"getProtectSpiritBonusRsp"
#define		MSG_worshiProtectSpiritRsp					"worshiProtectSpiritRsp"
#define		MSG_queryDiscoverInfoRsp					"queryDiscoverInfoRsp"
#define		MSG_discoverRsp								"discoverRsp"
#define		MSG_unlockDiscoverSealRsp_F					"unlockDiscoverSealRsp_F"
#define		MSG_TopHeroClearCDTimeRsp					"TopHeroClearCDTimeRsp"
#define		MSG_quitCityGuardRsp						"quitCityGuardRsp"

#define     MSG_FlopTaskGridRsp							"FlopTaskGridRsp"
#define     MSG_QueryPurifyRsp							"QueryPurifyRsp"
#define     MSG_PurifyRsp								"PurifyRsp"
#define     MSG_ConfirmPurifyRsp						"ConfirmPurifyRsp"
#define     MSG_QueryAssessWeaponRsp					"QueryAssessWeaponRsp"
#define     MSG_AssessWeaponRsp							"AssessWeaponRsp"

#define		MSG_QueryEmailRsp							"QueryEmailRsp"
#define		MSG_ReadEmailRsp							"ReadEmailRsp"
#define     MSG_WriteEmailRsp							"WriteEmailRsp"
#define		MSG_DeleteEmailRsp							"DeleteEmailRsp"
#define		MSG_SystemEmailPushRsp						"SystemEmailPushRsp"

#define		MSG_QuerySystemGiftBagRsp					"QuerySystemGiftBagRsp"
#define		MSG_GetSystemGiftBagRsp						"GetSystemGiftBagRsp"
#define     MSG_QueryEquipFallRsp						"QueryEquipFallRsp"
#define     MSG_QueryHeroOtherInfoRsp					"QueryHeroOtherInfoRsp"
#define     MSG_QueryNotifyInfoRsp						"QueryNotifyInfoRsp"
#define     MSG_GetGuideStateRsp						"GetGuideStateRsp"
#define     MSG_SetGuideStateRsp						"SetGuideStateRsp"

// 数据初始化，网络操作消息
#define		MSG_ChessBoardInitSuc						"ChessBoardInitSuc"
#define		MSG_ChessBoardInitFaild						"ChessBoardInitFaild"



class MSG_Data : public CCObject
{
public:
	Skill* mSkill;			// 当前的攻击技能
	BaseActor* mTarget;		// 被攻击者
	BaseActor* mFightor;	// 当前的攻击者
	float mHp;				// 当前的血量减少量
	MSG_Data(BaseActor* fightor,BaseActor* target,Skill* skill,float hp)
	{
		mSkill = skill;
		mFightor = fightor;
		mTarget = target;
		mHp = hp;
	}

	MSG_Data(BaseActor* fightor,BaseActor* target,float hp)
	{
		mSkill = NULL;
		mFightor = fightor;
		mTarget = target;
		mHp = hp;
	}
};

/**
 *	服务器返回数据的封装，直接对应的就是协议的Rsp结构体的数据
 *	注意，有些结构体是变长的，不能直接访问结构的的值，需要移动指针
 *	那么就用 Flat_MSG_Rsp 代替，这结构是线性的数据，可以直接访问
 */
class MSG_Rsp : public CCObject
{
	// 这个Data就死对应的协议里面的rsp数据结构的指针
	// 接受者自己强转就是了，已经是本地字节序了！
	// Global会自己清理这个数据，所以不用管这个data
	void* data;
public:
	static MSG_Rsp* createMsg(void* data)
	{
		MSG_Rsp* r = new MSG_Rsp(data);
		r->autorelease();
		return r;
	}
	MSG_Rsp(void* data)
	{
		CCLog("MSG_Rsp----> new");
		this->data = data;
	}
	~MSG_Rsp()
	{
		CCLog("MSG_Rsp----> free");
		/*if (data)
		{
		delete [] data;
		}*/
	}
	// 获取数据data
	void* getData()
	{
		return data;
	};
};

///**服务器返回数据的封装，没有直接对应协议的Rsp结构体，而是直接对应的另外一个结构体 Flat_*****
// *	比如SPCmd_ArenaGetBattleScriptGiftRsp ---> Flat_ArenaGetBattleScriptGiftRsp
// *	而postNotification的msg也对应的有一个_F结尾
// *	见头文件:flat.h 里面定义了需要展开的协议的Rsp结构体
// */
typedef MSG_Rsp Flat_MSG_Rsp;

///**
// *	服务器返回数据的封装，没有直接对应协议的Rsp结构体，而是直接对应的另外一个结构体 Flat_*****
// *	比如SPCmd_ArenaGetBattleScriptGiftRsp ---> Flat_ArenaGetBattleScriptGiftRsp
// *	而postNotification的msg也对应的有一个_F结尾
// *	见头文件:flat.h 里面定义了需要展开的协议的Rsp结构体
// */
//class Flat_MSG_Rsp : public CCObject
//{
//	// 这个Data就死对应的协议里面的rsp数据结构的指针
//	// 接受者自己强转就是了，已经是本地字节序了！
//	// Global会自己清理这个数据，所以不用管这个data
//	void* flat_data;
//public:
//	static Flat_MSG_Rsp* createMsg(void* flat_data)
//	{
//		Flat_MSG_Rsp* r = new Flat_MSG_Rsp(flat_data);
//		r->autorelease();
//		return r;
//	}
//	Flat_MSG_Rsp(void* flat_data)
//	{
//		CCLog("Flat_MSG_Rsp----> new");
//		this->flat_data = flat_data;
//	}
//	~Flat_MSG_Rsp()
//	{
//		CCLog("Flat_MSG_Rsp----> free");
//	}
//	// 获取数据data
//	void* getData()
//	{
//		return flat_data;
//	};
//};

#endif