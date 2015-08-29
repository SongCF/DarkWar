#pragma once
#include "ChessBoardHelper.h"
#include "FightScriptDefine.h"
#include "ChessboardItemBase.h"

class SkillHelperBaseAdapter;

struct SkillFight
{
	string nameForLog;						// 名字，打日志用的
	SkillPattern skillPatten;				// 攻击方式，暴击or普通攻击

	vector<GameBuff> firedBuff;				// 打出的Buff,里面的buff根据target直接附加到对象上面
	vector<unsigned int> targetVec;			// 被攻击的对象
	vector<bool> canMissVec;				// 能否躲避
	vector<int> targetBloodChange;			// 目标的血量变化
};

// 技能的帮助类
// 帮助模拟技能的攻击等
// 只是模拟，没有正真的减血，需要外界自己根据他的指示减血。
class SkillHelperBase
{
protected:
	// 天赋技能
	SkillFight fight_BaoLieZhan(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShaLu(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShanDian(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_JianShou(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShengCai(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShenZhiXingNian(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShenZhiXingNian2(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShunYinTuXi(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_LieGong(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_JianYu(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_HuoQiuShu(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_HuoQiuShu2(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_WenYi(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_WenYi2(ChessBoardItemBase* fightor,SkillDataForFight skill);

	// 技能书1
	SkillFight fight_BanYueZhan(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_CiSha(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_LeiBao(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_MangCi(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShenZhiZhuFu(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_LiRenHuiXuan(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_JingZhunZhiCan(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_JiSuDuJian(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_BaoFengXue(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_KongJu(ChessBoardItemBase* fightor,SkillDataForFight skill);

	// 技能书2
	SkillFight fight_KuangBao(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShiXueZhiRen(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_YunShi(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShouHu(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ZhongShenZhiLi(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_XuanFengZhan(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ZhuiHunDuJian(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_SiWangSaoShe(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_HanBingCiGu(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_ShiHun(ChessBoardItemBase* fightor,SkillDataForFight skill);

	// 技能书3
	SkillFight fight_BaoNu(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_YinZheHun(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_TianTangZhiGuang(ChessBoardItemBase* fightor,SkillDataForFight skill);

	// 怪物
	//SkillType_Monster_XieE
	SkillFight fight_Monster_XieE(ChessBoardItemBase* fightor,SkillDataForFight skill);

	// BOSS
	//SkillType_BOSS_Sweep
	SkillFight fight_BOSS_Sweep(ChessBoardItemBase* fightor,SkillDataForFight skill);
	//SkillType_BOSS_Flog
	SkillFight fight_BOSS_Flog(ChessBoardItemBase* fightor,SkillDataForFight skill);


	void setSkillHelperBaseProvider(SkillHelperBaseAdapter* provider);

	SkillHelperBaseAdapter* mProvider;
public:
	SkillHelperBase(SkillHelperBaseAdapter* adpater);
	// 技能攻击
	SkillFight fight(ChessBoardItemBase* fightor,SkillDataForFight skill);

	~SkillHelperBase(void);
};

