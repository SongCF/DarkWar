#pragma once

#include "cocos2d.h"
#include "protocol.h"
#include "flat.h"
#include "MsgDefine.h"
#include <vector>
//#include "CommonDefine.h"

using namespace std;
using namespace cocos2d;

struct EquipData;
struct CmdData
{
	char* head;
	char* body;
	SimpleProtocolCmd cmd;

	CmdData()
	{
		head = NULL;
		body = NULL;
	}
	//// 拷贝一份
	//CmdData copyOne()
	//{
	//	CmdData coped;
	//	coped.cmd = cmd;
	//}
};
struct FightScript;

struct TempHeroAndZhuangbei
{
	unsigned int hero_id;
	vector<long long> zb_id_arr;
};
struct TempEquipZhuangbei
{
	////临时 主英雄id
	//unsigned int main_hero_id;
	vector<TempHeroAndZhuangbei> hero_zb_arr;
};

// 临时用的，避免到处报错
struct SPCmd_ZBZhuankong
{

};

/**
 *	命令的帮助类
 *	用于快速直接调用拼装好的命令
 *	操作之后就会显示等待框，然后失败或者获得数据返回之后就发送消息，并关闭等待框
 */
class CmdHelper : public CCObject
{
private:
	static CmdHelper* helper;

	int m_SystemAlertNum;	// 是否需要cmd层自己弹框，默认是0

	CmdHelper(void);

	// 根据错误代码弹框
	void try_alert(int errCode);

	//// 传入cmd，需要的时候把请求包的cmd或者接收包的cmd传进来
	//bool StateRolling(int msg_type,int cmd = -1);
	char* newHead(SimpleProtocolCmd cmd,unsigned int dataLength);
	void removeLastBuff();
	void addObserver(SEL_CallFuncO selector,const char *name);// 添加消息监听
	void removeObserver(const char *name);// 移除消息监听

	void hton_HeroInfo(HeroInfo& infor);
	void hton_HeroDetail(HeroDetail& detail);
	void hton_HeroAttrWithoutZhuangbei(HeroAttrWithoutZhuangbei& data);
	void hton_GiftData(GiftData& data);

	void ntoh_GiftBagInfo(GiftBagInfo& info, Flat_GetSysTemGiftBagRsp &outData);

	void log(const HeroInfo& data);
	void log(const HeroDetail& data);
	void log(const HeroAttrWithoutZhuangbei& data);
	void log(const ZhuangbeiDetail& data);
	void log(const EquipData& data);

	void reload();// 把3个命令getHerosInTeamRsp-getZhuangbeiInTeamRsp-getItemsInTeamRsp连发
	void reconnect();// 直接发reconnect，不管什么状态

	void changeState(int state);

	bool reloadCheck(int err_code,SimpleProtocolCmd cmd,const char* post_msg, CCObject * postData);

	void jumpToIndex_error();
	void jumpToMain_reload();
	void jumpToIndex_kickOut();

	// 发送字节流
	// 需要调用者自己delete
	void sendByteDataToServer(char* netHead,int headLength,char* netBody,int bodyLength);

	// 存放战斗中获得的物品
	void storeAllGettedInFight(vector<FightScript> scripts,bool copyToFightPackage);

#pragma region 处理不同的包，日志和字节序,level1
	void dealWithClientLoginRsp(char* head,char* body);
	void dealWithReconnectRsp(char* head,char* body);
	void dealWithClientRegistRsp(char* head,char* body);
	void dealWithEquipZhuangbeiRsp(char* head,char* body);
	void dealWithCreateHeroRsp(char* head,char* body);
	void dealWithCreateAssitRsp(char* head,char* body);
	void dealWithGetHerosInTeamRsp(char* head,char* body);
	void dealWithGetZhuangbeiInTeamRsp(char* head,char* body);
	void dealWithAdjustFormationRsp(char* head,char* body);
	void dealWithGetTaskListRsp(char* head,char* body);
	void dealWithGetItemsInTeamRsp(char* head,char* body);
	void dealWithZBQianghuaRsp(char* head,char* body);
	void dealWithZBQianghuaHuituiRsp(char* head,char* body);
	void dealWithZBZhuankongRsp(char* head,char* body);
	void dealWithZBChaijieRsp(char* head,char* body);
	void dealWithZBXiangBaoshiRsp(char* head,char* body);
	void dealWithZBChaiBaoshiRsp(char* head,char* body);
	void dealWithBaoshiHechengRsp(char* head,char* body);
	void dealWithGetMarketInfoRsp(char* head,char* body);
	void dealWithBuyItemInMarketRsp (char* head,char* body);
	void dealWithBattleRsp(char* head,char* body);
	void dealWithGetTaskChessBoardRsp(char* head,char* body);
	void dealWithChangeTaskStateRsp(char* head,char* body);
	void dealWithRestoreVitalityRsp(char* head,char* body);
	void dealWithBeKickOutRsp(char* head,char* body);// 被服务器踢出了，客户端没有发送方式，收到就跳向login
	void dealWithHeartBeatRsp(char* head,char* body);// 心跳包，只管接受即可
	void dealWithArenaEnterRsp(char* head,char* body);
	void dealWithArenaUpdateRoomRsp(char* head,char* body);
	void dealWithArenaGetBattleScriptAndGiftRsp(char* head,char* body);
	void dealWithArenaGetGiftRsp(char* head,char* body);
	void dealWithTeamBossEnterRsp(char* head,char* body);
	void dealWithTeamBossUpdateRoomRsp(char* head,char* body);
	void dealWithTeamBossGetBattleScriptAndGiftRsp(char* head,char* body);
	void dealWithTeamBossGetGiftRsp(char* head,char* body);
	void dealWithCityGuardEnterRsp(char* head,char* body);
	void dealWithCityGuardGetNextMonsterRsp(char* head,char* body);
	void dealWithCityGuardFightRsp(char* head,char* body);
	void dealWithQueryAnotherTeamInfoRsp(char* head,char* body);    
	void dealWithTopHeroGetListRsp(char* head,char* body);
	void dealWithTopHeroChallengeRsp(char* head,char* body);
	void dealWithSystemGiftQueryRsp(char* head,char* body);
	void dealWithSystemGiftGetGiftRsp(char* head,char* body);
	void dealWithchargeVitalityRsp(char* head,char* body);
	void dealWithDuanzaoListQueryRsp(char* head,char* body);
	void dealWithDuanzaoRsp(char* head,char* body);
	void dealWithDuanzaoGroupZBQueryRsp(char* head,char* body);
	void dealWithQueryAllSuitNameRsp(char* head,char* body);
	void dealWithZBCuiquRsp(char* head,char* body);
	void dealWithZBFumoRsp(char* head,char* body);
	void dealWithQueryAllMofaJinghuaRsp(char* head,char* body);
	void dealWithQueryFubenDiaoluoRsp(char* head,char* body);
	void dealWithNewAssistantJoinRsp(char* head,char* body);
	void dealWithQuerySubTaskListRsp(char* head,char* body);
	void dealWithNewSubTaskGotRsp(char* head,char* body);
	void dealWithSubTaskSubmitedRsp(char* head,char* body);
	void dealWithQueryMainTaskRewardRsp(char* head,char* body);
	void dealWithChargeGoldRsp(char* head,char* body);
	void dealWithQueryTeamSkillListRsp(char* head,char* body);
	void dealWithEquipSkillRsp(char* head,char* body);
	void dealWithActorLearnSkillRsp(char* head,char* body);
	void dealWithQueryAllShenbingListRsp(char* head,char* body);
	void dealWithQueryTeamShenbingSuipianRsp(char* head,char* body);
	void dealWithShenbingHechengRsp(char* head,char* body);
	void dealWithQueryTeamItemSuipianRsp(char* head,char* body);
	void dealWithItemHechengRsp(char* head,char* body);
	void dealWithSaodangFubenRsp(char* head,char* body);
	void dealWithHeroLevelUpRsp(char* head,char* body);
	void dealWithHeroUpgradeRsp(char* head,char* body);
	void dealWithShenbingJieFengyinRsp(char* head,char* body);
	void dealWithShenbingJiandingRsp(char* head,char* body);
	void dealWithZBJinglianRsp(char* head,char* body);
	void dealWithShenbingZhuankeJingwenRsp(char* head,char* body);
	void dealWithQueryArenaTeamBasicInfoRsp(char* head,char* body);
	void dealWithQueryArenaMarketRsp(char* head,char* body);
	void dealWithRefreshArenaChlgListRsp(char* head,char* body);
	void dealWithQueryArenaChlgListRsp(char* head,char* body);
	void dealWithArenaChallengeRsp(char* head,char* body);
	void dealWithArenaMarketBuyRsp(char* head,char* body);
	void dealWithArenaBuyChallengeTimeRsp(char* head,char* body);
	void dealWithQueryZahuopuRsp(char* head,char* body);
	void dealWithRefreshZahuopuRsp(char* head,char* body);
	void dealWithZahuopuGetOneItemRsp(char* head,char* body);
	void dealWithQueryHellTaskHeroInfoRsp(char* head,char* body);
	void dealWithHellTaskBattleRspRsp(char* head,char* body);
	void dealWithQueryYuanshenOfHeroRsp(char* head,char* body);
	void dealWithChurchLevelUpRsp(char* head,char* body);
	void dealWithSkillLevelUpRsp(char* head,char* body);
	void dealWithGetZhuangbeiAlbumRsp(char* head,char* body);
	void dealWithCityGuardProcessNotifyRsp(char* head,char* body);
	void dealWithGetCityGuardCurrentStatRsp(char* head,char* body);
	void dealWithCityGuardBattleRsp(char* head,char* body);
	void dealWithGetCityGuardLastDayReportRsp(char* head,char* body);
	void dealWithGetMyCityGuardRewardRsp(char* head,char* body);
	//20140613 奖励系统变更（奖励与悬赏任务合并）
// 	void dealWithQueryDailyLoginBonusRsp(char* head,char* body);
// 	void dealWithQueryLevelBonusRsp(char* head,char* body);
// 	void dealWithQuerySystemBonusRsp(char* head,char* body); // 服务器推送的系统奖励
// 	void dealWithGetDailyLoginBonusRsp(char* head,char* body);
// 	void dealWithGetLevelBonusRsp(char* head,char* body);
// 	void dealWithGetSystemBonusRsp(char* head,char* body);
	void dealWithQueryBonusRsp(char* head,char* body);
	void dealWithGetBonusRsp(char* head,char* body);
	void delWithSystemPushBonusRsp(char* head,char* body);

//	void dealWithGetChessRandomBoxRsp(char* head,char* body);
	void dealWithQueryTaskRateRsp(char* head,char* body);
	void dealWithGetTaskRateBonusRsp(char* head,char* body);
	void dealWithQueryRateTaskBonusRsp(char* head,char* body);
	void dealWithQueryVipPrivilegeRsp(char* head,char* body);
	void dealWithUpgradeShenbingRsp(char* head,char* body);
	void dealWithQueryChargePacketRsp(char* head,char* body);
	void dealWithUnlockAssistantRsp(char* head,char* body);
	void dealWithQueryGiftActivityRsp(char* head,char* body);
	void dealWithGetGiftActivityRsp(char* head,char* body);
	void dealWithQueryKilledMonsterRsp(char* head,char* body);
	void dealWithQueryKilledMonsterBonusRsp(char* head,char* body);
	void dealWithGetKilledMonsterBonusRsp(char* head,char* body);
	void dealWithTopArenaQueryGuessRsp(char* head,char* body);
	void dealWithTopArenaQueryStatusRsp(char* head,char* body);
	void dealWithTopArenaGuessRsp(char* head,char* body); 
	void dealWithTopArenaReplayRsp(char* head,char* body);
	void dealWithCastSilverRsp(char* head,char* body);
	void dealWithQueryLegendNoteRsp(char* head,char* body);
	void dealWithChallengeLegendNoteRsp(char* head,char* body);
	void dealWithQuerySelfChallengeInfoRsp(char* head,char* body);
	void dealWithQuerySelfChallengeOrderRsp(char* head,char* body);
	void dealWithSelfChallengeRsp(char* head,char* body);
	void dealWithSelfChallengeGainRsp(char* head,char* body);
	void dealWithGetSelfChallengeBoxRsp(char* head,char* body); 
	void dealWithQueryProtectSpiritRsp(char* head,char* body);
	void dealWithChallengeProtectSpiritRsp(char* head,char* body);
	void dealWithGetProtectSpiritBonusRsp(char* head,char* body);
	void dealWithWorshiProtectSpiritRsp(char* head,char* body);
	void dealWithQueryDiscoverInfoRsp(char* head,char* body);
	void dealWithDiscoverRsp(char* head,char* body);
	void dealWithUnlockDiscoverSealRsp(char* head,char* body);
	void dealWithTopHeroClearCDTimeRsp(char* head,char* body);

	void dealWithQuitCityGuardRsp(char* head,char* body);

	//20140521 scf 
	void dealWithFlopTaskGridRsp(char* head,char* body);
	void dealWithQueryPurifyRsp(char* head,char* body);
	void dealWithPurifyRsp(char* head,char* body);
	void dealWithConfirmPurifyRsp(char* head,char* body);
	void dealWithQueryAssessWeaponRsp(char* head, char*body);
	void dealWithAssessWeaponRsp(char* head, char*body);
	//20140609 邮件
	void dealWithQueryEmailRsp(char* head, char*body);
	void dealWithReadEmailRsp(char* head, char*body);
	void dealWithWriteEmailRsp(char* head, char*body);
	void dealWithDeleteEmailRsp(char* head, char*body);
	void dealWithPushEmail(char* head, char*body); //推送来的实时邮件
	//20140611 系统礼包
	void dealWithQuerySystemGiftBagRsp(char* head, char*body);
	void dealWithGetSystemGiftBagRsp(char* head, char*body);
	//20140613 装备图鉴 掉落查询
	void dealWithQueryEquipFallRsp(char* head, char*body);
	//20140617 查询主角其他信息
	void dealWithQueryHeroOtherInfoRsp(char* head, char*body);
	//20140618 查询公告
	void dealWithSystemNotifyInfoRsp(char* head, char*body);
	//20140625 教学状态
	void dealWithGetGuideStateRsp(char* head, char*body);
	void dealWithSetGuideStateRsp(char* head, char*body);


#pragma endregion

#pragma region 网络的不同包的业务逻辑处理,level2
	void loginCallBack(CCObject* msg_rsp);// 登陆回调
	void reconnectedCallback(CCObject* msg_rsp);// 重连
	void registerCallback(CCObject* msg_rsp);
	void equipCallBack(CCObject* msg_rsp);// 装备/卸装 完毕
	void createAHeroCallback(CCObject* msg_rsp);// 创建英雄回调
	void createAAssitCallback(CCObject* msg_rsp);// 创建助手回调
	void getHerosInTeamCallBack(CCObject* msg_rsp);// 得到队伍里面的成员回调
	void getZhuangbeiInTeamCallback(CCObject* f_msg_rsp);
	void adjustFormationCallback(CCObject* msg_rsp);
	void getTaskListCallback(CCObject* msg_rsp);
	void getItemsInTeamCallback(CCObject* msg_rsp);
	void cmdZBQianghuaCallback(CCObject* msg_rsp);// 强化装备
	//void cmdZBZhuankongCallback(CCObject* msg_rsp);// 钻孔
	void cmdZBQianghuaHuituiCallback(CCObject* msg_rsp);// 强化回退
	void cmdZBChaijieCallback(CCObject* msg_rsp);// 拆解
	void cmdZBXiangBaoshiCallback(CCObject* msg_rsp);// 镶宝石
	void cmdZBChaiBaoshiCallback(CCObject* msg_rsp);// 拆宝石
	void cmdBaoshiHechengCallback(CCObject* msg_rsp);// 宝石合成
	void cmdGetMarketInfoCallBack(CCObject* msg_rsp);// 获取市场信息
	void cmdBuyItemInMarketCallBack(CCObject* msg_rsp);// 购买的回调
	void cmdBattleCallBack(CCObject* msg_rsp);// 战斗回调
	void cmdGetTaskChessBoardCallback(CCObject* msg_rsp);// 获得任务棋盘的回调
	void cmdChangeTaskStateCallback(CCObject* msg_rsp);// 改变状态
	void cmdRestoreVitalityCallback(CCObject* msg_rsp);// 恢复体力回调
	void cmdBeKickOutRsp(CCObject* msg_rsp);// 被服务器踢出了，客户端没有发送方式，收到就跳向login
	void cmdHeartBeatRsp(CCObject* msg_rsp);// 心跳包，只管接受即可
	void cmdArenaEnterRsp(CCObject* f_msg_rsp);// 进入竞技场
	void cmdArenaUpdateRoomRsp(CCObject* f_msg_rsp);// 竞技场人员有新增，有变动
	void cmdArenaGetBattleScriptAndGiftRsp(CCObject* f_msg_rsp);// 竞技场战斗完毕
	void cmdArenaGetGiftRsp(CCObject* msg_rsp);// 领取竞技场物品
	void cmdTeamBossEnterRsp(CCObject* f_msg_rsp);
	void cmdTeamBossUpdateRoomRsp(CCObject* f_msg_rsp);
	void cmdTeamBossGetBattleScriptAndGiftRsp(CCObject* f_msg_rsp);
	void cmdTeamBossGetGiftRsp(CCObject* msg_rsp);
	void cmdCityGuardEnterRsp(CCObject* msg_rsp);
	void cmdCityGuardGetNextMonsterRsp(CCObject* msg_rsp);
	void cmdCityGuardFightRsp(CCObject* f_msg_rsp);
	void cmdQueryAnotherTeamInfoRsp(CCObject* f_msg_rsp);    
	void cmdTopHeroGetListRsp(CCObject* f_msg_rsp);
	void cmdTopHeroChallengeRsp(CCObject* f_msg_rsp);
	void cmdSystemGiftQueryRsp(CCObject* f_msg_rsp);
	void cmdSystemGiftGetGiftRsp(CCObject* msg_rsp);
	void cmdchargeVitalityRsp(CCObject* msg_rsp);
	void cmdDuanzaoListQueryRsp(CCObject* msg_rsp);
	void cmdDuanzaoRsp(CCObject* f_msg_rsp);
	void cmdDuanzaoGroupZBQueryRsp(CCObject* f_msg_rsp);
	void cmdQueryAllSuitNameRsp(CCObject* msg_rsp);
	void cmdZBCuiquRsp(CCObject* msg_rsp);
	void cmdZBFumoRsp(CCObject* msg_rsp);
	void cmdQueryAllMofaJinghuaRsp(CCObject* msg_rsp);
	void cmdQueryFubenDiaoluoRsp(CCObject* f_msg_rsp);
	void cmdNewAssistantJoinRsp(CCObject* f_msg_rsp);
	void cmdQuerySubTaskListRsp(CCObject* f_msg_rsp);
	void cmdNewSubTaskGotRsp(CCObject* msg_rsp);
	void cmdSubTaskSubmitedRsp(CCObject* msg_rsp);
	void cmdQueryMainTaskRewardRsp(CCObject* f_msg_rsp);
	void cmdChargeGoldRsp(CCObject* msg_rsp);
	void cmdQueryTeamSkillListRsp(CCObject* msg_rsp);
	void cmdEquipSkillRsp(CCObject* msg_rsp);
	void cmdActorLearnSkillRsp(CCObject* msg_rsp);
	void cmdQueryAllShenbingListRsp(CCObject* f_msg_rsp);
	void cmdQueryTeamShenbingSuipianRsp(CCObject* msg_rsp);
	void cmdShenbingHechengRsp(CCObject* f_msg_rsp);
	void cmdQueryTeamItemSuipianRsp(CCObject* msg_rsp);
	void cmdItemHechengRsp(CCObject* msg_rsp);
	void cmdSaodangFubenRsp(CCObject* f_msg_rsp);
	void cmdHeroLevelUpRsp(CCObject* msg_rsp);
	void cmdHeroUpgradeRsp(CCObject* msg_rsp);
	void cmdShenbingJieFengyinRsp(CCObject* msg_rsp);
	void cmdShenbingJiandingRsp(CCObject* msg_rsp);
	void cmdZBJinglianRsp(CCObject* msg_rsp);
	void cmdShenbingZhuankeJingwenRsp(CCObject* msg_rsp);
	void cmdQueryArenaTeamBasicInfoRsp(CCObject* msg_rsp);
	void cmdQueryArenaMarketRsp(CCObject* f_msg_rsp);
	void cmdRefreshArenaChlgListRsp(CCObject* f_msg_rsp);
	void cmdQueryArenaChlgListRsp(CCObject* f_msg_rsp);
	void cmdArenaChallengeRsp(CCObject* f_msg_rsp);
	void cmdArenaMarketBuyRsp(CCObject* msg_rsp);
	void cmdArenaBuyChallengeTimeRsp(CCObject* msg_rsp);
	void cmdQueryZahuopuRsp(CCObject* msg_rsp);
	void cmdRefreshZahuopuRsp(CCObject* msg_rsp);
	void cmdZahuopuGetOneItemRsp(CCObject* f_msg_rsp);
	void cmdQueryHellTaskHeroInfoRsp(CCObject* msg_rsp);
	void cmdHellTaskBattleRsp(CCObject* f_msg_rsp);
	void cmdQueryYuanshenOfHeroRsp(CCObject* f_msg_rsp);
	void cmdChurchLevelUpRsp(CCObject* msg_rsp);
	void cmdSkillLevelUpRsp(CCObject* msg_rsp);
	void cmdGetZhuangbeiAlbumRsp(CCObject* f_msg_rsp);
	void cmdCityGuardProcessNotifyRsp(CCObject* f_msg_rsp);
	void cmdGetCityGuardCurrentStatRsp(CCObject* msg_rsp);
	void cmdCityGuardBattleRsp(CCObject* f_msg_rsp);
	void cmdGetCityGuardLastDayReportRsp(CCObject* f_msg_rsp);
	void cmdGetMyCityGuardRewardRsp(CCObject* msg_rsp);
	//20140613 奖励系统变更（奖励与悬赏任务合并）
// 	void cmdQueryDailyLoginBonusRsp(CCObject* f_msg_rsp);
// 	void cmdQueryLevelBonusRsp(CCObject* f_msg_rsp);
// 	void cmdQuerySystemBonusRsp(CCObject* f_msg_rsp); // 服务器推送的系统奖励
// 	void cmdGetDailyLoginBonusRsp(CCObject* msg_rsp);
// 	void cmdGetLevelBonusRsp(CCObject* msg_rsp);
// 	void cmdGetSystemBonusRsp(CCObject* msg_rsp);
	void cmdQueryBonusRsp(CCObject* msg_rsp);
	void cmdGetBonusRsp(CCObject* msg_rsp);
	void cmdSystemPushBonusRsp(CCObject* msg_rsp);

//	void cmdGetChessRandomBoxRsp(CCObject* msg_rsp);
	void cmdQueryTaskRateRsp(CCObject* f_msg_rsp);
	void cmdGetTaskRateBonusRsp(CCObject* msg_rsp);
	void cmdQueryRateTaskBonusRsp(CCObject* f_msg_rsp);
	void cmdQueryVipPrivilegeRsp(CCObject* msg_rsp);
	void cmdUpgradeShenbingRsp(CCObject* f_msg_rsp);
	void cmdQueryChargePacketRsp(CCObject* f_msg_rsp);
	void cmdUnlockAssistantRsp(CCObject* msg_rsp);
	void cmdQueryGiftActivityRsp(CCObject* f_msg_rsp);
	void cmdGetGiftActivityRsp(CCObject* msg_rsp);
	void cmdQueryKilledMonsterRsp(CCObject* msg_rsp);
	void cmdQueryKilledMonsterBonusRsp(CCObject* f_msg_rsp);
	void cmdGetKilledMonsterBonusRsp(CCObject* msg_rsp);
	void cmdTopArenaQueryGuessRsp(CCObject* msg_rsp);
	void cmdTopArenaQueryStatusRsp(CCObject* f_msg_rsp);
	void cmdTopArenaGuessRsp(CCObject* msg_rsp); 
	void cmdTopArenaReplayRsp(CCObject* f_msg_rsp);
	void cmdCastSilverRsp(CCObject* msg_rsp);
	void cmdQueryLegendNoteRsp(CCObject* msg_rsp);
	void cmdChallengeLegendNoteRsp(CCObject* f_msg_rsp);
	void cmdQuerySelfChallengeInfoRsp(CCObject* msg_rsp);
	void cmdQuerySelfChallengeOrderRsp(CCObject* f_msg_rsp);
	void cmdSelfChallengeRsp(CCObject* f_msg_rsp);
	void cmdSelfChallengeGainRsp(CCObject* msg_rsp);
	void cmdGetSelfChallengeBoxRsp(CCObject* msg_rsp); 
	void cmdQueryProtectSpiritRsp(CCObject* msg_rsp);
	void cmdChallengeProtectSpiritRsp(CCObject* f_msg_rsp);
	void cmdGetProtectSpiritBonusRsp(CCObject* msg_rsp);
	void cmdWorshiProtectSpiritRsp(CCObject* msg_rsp);
	void cmdQueryDiscoverInfoRsp(CCObject* msg_rsp);
	void cmdDiscoverRsp(CCObject* msg_rsp);
	void cmdUnlockDiscoverSealRsp(CCObject* f_msg_rsp);
	void cmdTopHeroClearCDTimeRsp(CCObject* msg_rsp);

	void cmdQuitCityGuardRsp(CCObject* msg_rsp);

	//20140521 scf 
	void cmdFlopTaskGridRsp(CCObject* msg_rsp);
	void cmdQueryPurifyRsp(CCObject* msg_rsp);
	void cmdPurifyRsp(CCObject* msg_rsp);
	void cmdConfirmPurifyRsp(CCObject* msg_rsp);
	void cmdQueryAssessWeaponRsp(CCObject* msg_rsp);
	void cmdAssessWeaponRsp(CCObject* msg_rsp);
	//20140609 邮件
	void cmdQueryEmailRsp(CCObject* msg_rsp);
	void cmdReadEmailRsp(CCObject* msg_rsp);
	void cmdWriteEmailRsp(CCObject* msg_rsp);
	void cmdDeleteEmailRsp(CCObject* msg_rsp);
	void pushEmailRsp(CCObject *msg_rsp); //系统推送来的实时邮件
	//20140611 系统礼包
	void cmdQuerySystemGiftBagRsp(CCObject* msg_rsp);
	void cmdGetSystemGiftBagRsp(CCObject* msg_rsp);
	//20140613 图鉴装备 掉落查询
	void cmdQueryEquipFallRsp(CCObject* msg_rsp);
	//20140617 查询主角其他信息
	void cmdQueryHeroOtherInfoRsp(CCObject* msg_rsp);
	//20140618 查询公告
	void cmdQuerySystemNotifyInfoRsp(CCObject* msg_rsp);
	//20140625 教学状态
	void cmdGetGuideStateRsp(CCObject* msg_rsp);
	void cmdSetGuideStateRsp(CCObject* msg_rsp);


#pragma endregion

public:
	static CmdHelper* getHelper();

	// 设置下次有错就弹框，下么下下次就不会弹了
	// alertNum就代表可以弹框的次数
	void setSystemAlert(int alertNum = 1);
	// 获取code对应的错误消息
	string getMessage(int errorCode);

#pragma region 网络回调
	// 网络端的回调，这里分发给不同的处理函数
	// 不用delete，外界会delete
	void dealWithPackage(char* head,char* body);
	void recvDataError();
	void sendDataError();
	void connectServerError();
	void connectServerSuccess();
#pragma endregion

#pragma region 网络请求接口
	// 用户的login
	void cmdLogin(const char userName[ClientAccountMaxLen+1],const char password[ClientPswMaxLen+1]);
	// 重连
	void cmdReconnect();
	// 注册
	void cmdRegist(const char userName[ClientAccountMaxLen+1],const char password[ClientPswMaxLen+1]);
	// 装装备，报修改过的英雄的装备
	void cmdEquipZhuangBei(TempEquipZhuangbei data);
	// 创建一个英雄
	void cmdCreateHero(unsigned int profession_id,string name);
	// 创建一个助手
	void cmdCreateAssit(unsigned int main_hero_id,unsigned int profession_id);
	// 得到队伍的人
	void cmdGetHerosInTeam(unsigned main_hero_id);
	// 获取装备
	void cmdGetZhuangbeiInTeam(/*unsigned int main_hero_id*/);
	// 调阵型
	void cmdAdjustFormation(unsigned int hero_id_arr[FormationHeroAcountMax]);
	// 获取任务列表
	void cmdGetTaskList(unsigned int main_hero_id);
	// 获取物品
	void cmdGetItemsInTeam();
	// 强化装备
	void cmdZBQianghua(SPCmd_ZBQianghua request);
	// 钻孔
	void cmdZBZhuankong(SPCmd_ZBZhuankong req);
	// 强化回退
	// 指定要回退的装备和回退时哪些需要保留不退
	void cmdZBQianghuaHuitui(unsigned long long zhuangbeiId,vector<unsigned char> keep);
	// 拆解
	void cmdZBChaijie(vector<long long> equips_id_vec);
	// 镶宝石
	void cmdZBXiangBaoshi(SPCmd_ZBXiangBaoshi req);
	// 拆宝石
	void cmdZBChaiBaoshi(SPCmd_ZBChaiBaoshi req);
	// 宝石合成
	void cmdBaoshiHecheng(SPCmd_BaoshiHecheng req);
	// 市场信息
	void cmdGetMarketInfo();
	// 购买
	void cmdBuyItemInMarket(SPCmd_BuyItemInMarket req);
	// 发送战斗,注意这里个函数的回调是不一样的
	// 里面的Data不是协议的结构体指针，而是我们的一个临时变量FightScirpt指针
	void cmdBattle(SPCmd_Battle req);
	// 获得任务的棋盘数据
	void cmdGetTaskChessboard(unsigned int task_id,bool refreshBoard);
	// 改变当前状态：TaskStatus_Got 和 TaskStatus_Submitted 是客户端发的
	void cmdChangeTaskState(unsigned int taskId,TaskStatus state);
	// 恢复体力
	void cmdRestoreVitality();
	// 进入竞技场,这个一直需要持续到cmdArenaGetBattleScriptGiftRsp
	void cmdArenaEnter(SPCmd_ArenaEnter req);	
	// 领取竞技场物品
	//void cmdArenaGetGift(vector<unsigned int> wantGetId_arr);
	void cmdArenaGetGift(unsigned int wantGetId);
	// 进入团队战场,这个一直需要持续到cmdTeamBossGetBattleScriptGiftRsp
	void cmdTeamBossEnter(SPCmd_TeamBossEnter req);	
	// 领取团队战物品
	//void cmdTeamBossGetGift(vector<unsigned int> wantGetId_arr);//unsigned int wantGetId
	void cmdTeamBossGetGift(unsigned int wantGetId);//
	// 进入团队副本
	void cmdCityGuardEnter();
	// 获取对战数据
	void cmdCityGuardGetNextMonster();
	// 迎战 或者 放弃
	void cmdCityGuardFight(bool fight);
	// 查询duiy
	void cmdQueryAnotherTeamInfo(unsigned int hero_id);
	// 获取英雄榜
	void cmdTopHeroGetList();
	// 挑战英雄榜,传入对方主英雄的id
	void cmdTopHeroChallenge(unsigned int hero_id);
	// 查询是否有奖励
	void cmdSystemGiftQuery();
	// 领取系统的奖励
	void cmdSystemGiftGetGift(unsigned int giftId);
	void cmdchargeVitality();
	// 锻造
	// 查询可以锻造的东西
	void cmdDuanzaoListQuery();
	// 选择一项，进行锻造
	//void cmdDuanzao(bool duanzao_10_times,unsigned int duanzao_group_id);
	void cmdDuanzao(bool duanzao_10_times,unsigned int profession);
	// 查询锻造组中的装备信息
	void cmdDuanzaoGroupZBQuery(unsigned int duanzao_group_id);
	// 查询套装，这个应该在初始化的时候就应查获，最好英该在查询装备之前
	void cmdQueryAllSuitName();
	// 萃取
	void cmdZBCuiqu(SPCmd_ZBCuiqu req);
	// 附魔
	void cmdZBFumo(SPCmd_ZBFumo req);
	// 查询所有的魔法精华
	void cmdQueryAllMofaJinghua();
	// 查询副本掉落
	void cmdQueryFubenDiaoluo(unsigned int task_id);
	// 查询支线任务列表
	void cmdQuerySubTaskList();
	// 查询主线任务的奖励
	void cmdQueryMainTaskReward();
	// 充金币充值包// 1,2,3,4
	void cmdChargeGold(unsigned int goldPacketId);
	// 查询技能
	void cmdQueryTeamSkillList();
	// 装备技能
	void cmdEquipSkill(unsigned int hero_id,unsigned int skill_id);
	// 学习技能
	void cmdActorLearnSkill(unsigned int hero_id,unsigned int item_id);
	// 查询所有的神兵列表
	void cmdQueryAllShenbingList();
	// 查询神兵碎片列表
	void cmdQueryTeamShenbingSuipian();
	// 神兵合成
	void cmdShenbingHecheng(unsigned int zb_group_id);
	// Item碎片
	void cmdQueryTeamItemSuipian();
	// Item碎片合成
	void cmdItemHecheng(unsigned int item_type);
	// 副本扫荡
	void cmdSaodangFuben(SPCmd_Saodang req);
	// 英雄升级
	void cmdHeroLevelUp(unsigned int hero_id);
	// 英雄升阶
	void cmdHeroUpgrade(unsigned int hero_id);
	// 解封印
	void cmdShenbingJieFengyin(unsigned long long zb_id);
	// 神兵属性鉴定
	void cmdShenbingJianding(unsigned long long zb_id);
	// 装备精练
	void cmdZBJinglian(unsigned long long zb_id,unsigned long long cailiao_zb_id);
	// 神兵篆刻经文
	void cmdShenbingZhuankeJingwen(unsigned long long zb_id);
	// 查询竞技场的信息
	void cmdQueryArenaTeamBasicInfo();
	// 查询竞技场的市场数据
	void cmdQueryArenaMarket();
	// 点击按钮手动刷新
	void cmdRefreshArenaChlgList();
	// 获取当前的挑战列表
	void cmdQueryArenaChlgList();
	// 竞技场挑战
	void cmdArenaChallenge(unsigned int challenge_hero_id);
	// 竞技场购买商品
	void cmdArenaMarketBuy(unsigned int item_type,unsigned int item_count);
	// 购买挑战的次数
	// is_max_buy = false--> 代表只购买一次
	// is_max_buy = true --> 代表买满
	void cmdArenaBuyChallengeTime(bool is_max_buy);
	// 查询杂货铺数据-->ZahuopuHelper就会保存查询的返回
	void cmdQueryZahuopu();
	// 刷新杂货铺数据-->ZahuopuHelper就会保存查询的返回
	void cmdRefreshZahuopu();
	// 获取杂货铺的一个东西
	void cmdZahuopuGetOneItem();
	// 查询地狱副本的数据
	void cmdQueryHellTaskHeroInfo();
	// 打副本
	void cmdHellTaskBattle(unsigned int task_id);
	// 查询某个Hero的元神数据
	void cmdQueryYuanshenOfHero(unsigned int heroId);
	// 升级教堂
	void cmdChurchLevelUp();
	// 升级技能
	void cmdSkillLevelUp(unsigned int owner_hero_id,unsigned int skill_id);
	// 获取装备图鉴(技能图鉴客户端可以自己拼接，SKillTree可以提供)
	void cmdGetZhuangbeiAlbum(int profession);
	// 世界boss的数据在WorldBossCityGuardManager有实时更新
	// 所有的网络命令都会更新WorldBossCityGuardManager
	// 
	// 获取当前的世界boos守城的状态
	void cmdGetCityGuardCurrentStat();
	// 开始战斗（如果还处于倒计时，那么就不能调用,需要用cmdCityGuardBattleSkipCD代替）
	void cmdCityGuardBattle();
	//// 跳过CD时间，直接开打（getCityGuardBattleSkipCDNeedGold）
	//void cmdCityGuardBattleSkipCD();
	// 获取守城上次的战报数据
	void cmdGetCityGuardLastDayReport();
	// 领取守城的奖励
	void cmdGetMyCityGuardReward();
	//20140613 奖励系统变更（奖励与悬赏任务合并）
// 	// 查询日常登陆奖励
// 	void cmdQueryDailyLoginBonus();
// 	// 查询等级变化的奖励
// 	void cmdQueryLevelBonus();
// 	// 查询系统奖励
// 	void cmdQuerySystemBonus();
// 	// 领取日常登陆奖励
// 	void cmdGetDailyLoginBonus(unsigned int _id);
// 	// 领取升级登陆奖励
// 	void cmdGetLevelBonus(unsigned int _id);
// 	// 领取系统推送的奖励
// 	void cmdGetSystemBonus(unsigned int _id);
	//查询奖励
	void cmdQueryBonus(BonusType queryType=BonusType_All);
	//领取奖励
	void cmdGetBonus(BonusType type, unsigned int id);

//	// 领取棋盘上随机的宝箱
//	void cmdGetChessRandomBox(unsigned int task_id,unsigned int boxId);
	// 查询关卡的星级
	void cmdQueryTaskRate();
	// 查询奖励
	void cmdQueryRateTaskBonus(RateStarType type,unsigned int task_id);
	// 领取关卡评级的奖励，调用一次，领一个
	void cmdGetTaskRateBonus(RateStarType type,RateStarLevel level,unsigned short task_id);
	// 查询当前VIP特权数据
	void cmdQueryVipPrivilege();
	// 神兵升级
	// 指定要升级的装备Id和要被吞噬的传奇材料id
	void cmdUpgradeShenbing(unsigned long long zb_id,unsigned long long cailiao_zb_id);
	// 查询充值信息
	void cmdQueryChargePacket();
	//解锁英雄
	void cmdUnlockAssistant(unsigned int hero_id);
	// 查询活动，目前只有体力赠送活动
	void cmdQueryGiftActivity();
	// 获活动的礼物，传入活动的类型
	void cmdGetGiftActivity(unsigned int activtyType);
	// 查询击杀的怪物数量
	void cmdQueryKilledMonster();
	// 查询击杀boss的奖励
	void cmdQueryKilledMonsterBonus();
	// 领取击杀boss的奖励
	void cmdGetKilledMonsterBonus(AccumulateType type);
	// 巅峰对决
	// 查询猜测数据
	void cmdTopArenaQueryGuess(int groupIndex);
	void cmdTopArenaQueryStatus();
	void cmdTopArenaGuess(unsigned short group,bool guess_winer,unsigned int wager); 
	void cmdTopArenaReplay(unsigned short round,unsigned short group,unsigned short times);
	// 铸银，每次的金额不一样
	void cmdCastSilver();
	// 
	void cmdQueryLegendNote();
	void cmdChallengeLegendNote(unsigned int groupId);
	// 血色教堂（自我挑战/血战）
	void cmdQuerySelfChallengeInfo();
	void cmdQuerySelfChallengeOrder(int difficut);
	void cmdSelfChallenge(); // 闯关
	void cmdSelfChallengeGain(SelfChlgGainType _gType,SelfChlgGainDegree _degree); // 加成
	void cmdGetSelfChallengeBox(); // 领保险

	// 守护之灵
	// 每次都应该查询
	void cmdQueryProtectSpirit(unsigned int seasonId);
	// 挑战
	void cmdChallengeProtectSpirit(unsigned int seasonId);
	// 领取奖励
	void cmdGetProtectSpiritBonus(unsigned int seasonId);
	// 膜拜
	void cmdWorshiProtectSpirit(unsigned int seasonId);
	// 探索的消息
	// 查询信息
	void cmdQueryDiscoverInfo();
	// 探索
	void cmdDiscover(bool akey_all);
	// 解除封印，得传奇
	void cmdUnlockDiscoverSeal(unsigned int profession);
	//
	void cmdTopHeroClearCDTime();
	
	//
	void cmdQuitCityGuard();

	//20140521 scf 九宫格改为翻拍， 添加翻拍消息
	void cmdFlopTaskGrid(unsigned int taskId, unsigned int gridId);
	//装备洗练
	void cmdQueryPurify();
	void cmdPurify(unsigned long long zbID, unsigned int holeId);
	void cmdConfirmPurify(unsigned long long zbID, bool save);
	//稀有装鉴定
	void cmdQueryAssessWeapon(unsigned long long zbID);
	void cmdAssessWeapon(unsigned long long zbId, unsigned long long usedZbId);
	//20140609 邮件
	void cmdQueryEmail();
	void cmdReadEmail(unsigned int email_id);
	void cmdDeleteEmail(unsigned int email_id);
	void cmdWriteEmail(unsigned int receiver_id, string content);//发送email
	//20140611 系统礼包
	void cmdQuerySystemGiftBag();
	void cmdGetSystemGiftBag(unsigned int id);//打开礼包
	//20140613 装备图鉴 掉落查询
	void cmdQueryEquipFall();
	//20140617 查询主角其他信息
	void cmdQueryHeroOtherInfo();
	//20140618 查询公告
	void cmdQuerySystemNotifyInfo();
	//20140625 教学状态
	void cmdGetGuideState();
	void cmdSetGuideState(unsigned long long);//0-63

#pragma endregion

	//注销
	void logOff();

	// 关闭网络链接，直接关闭
	// 那么就会触发重联，重登陆
	void forceCloseNetwork();

	// 获取缓存的运行时数据
	// 把数据存放在SPCmd_CityGuardEnterRsp结构体里面
	void getCityGruardRuntimeData(SPCmd_CityGuardEnterRsp& out_data);

	~CmdHelper(void);
};

