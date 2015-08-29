#pragma once
#include "ChessBoardHelper.h"
#include "FightScriptDefine.h"
#include "ChessboardItemBase.h"

class SkillHelperBaseAdapter;

struct SkillFight
{
	string nameForLog;						// ���֣�����־�õ�
	SkillPattern skillPatten;				// ������ʽ������or��ͨ����

	vector<GameBuff> firedBuff;				// �����Buff,�����buff����targetֱ�Ӹ��ӵ���������
	vector<unsigned int> targetVec;			// �������Ķ���
	vector<bool> canMissVec;				// �ܷ���
	vector<int> targetBloodChange;			// Ŀ���Ѫ���仯
};

// ���ܵİ�����
// ����ģ�⼼�ܵĹ�����
// ֻ��ģ�⣬û������ļ�Ѫ����Ҫ����Լ���������ָʾ��Ѫ��
class SkillHelperBase
{
protected:
	// �츳����
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

	// ������1
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

	// ������2
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

	// ������3
	SkillFight fight_BaoNu(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_YinZheHun(ChessBoardItemBase* fightor,SkillDataForFight skill);
	SkillFight fight_TianTangZhiGuang(ChessBoardItemBase* fightor,SkillDataForFight skill);

	// ����
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
	// ���ܹ���
	SkillFight fight(ChessBoardItemBase* fightor,SkillDataForFight skill);

	~SkillHelperBase(void);
};

