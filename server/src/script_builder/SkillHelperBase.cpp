#include "SkillHelperBase.h"
#include "SkillHelperBaseAdapter.h"
#include "CalculateData.h"

extern float lfRandom0_1();
#if (USE_FIGHT_LOG == 1)
#define S_LOG(format, ...)     CCLog(format, ##__VA_ARGS__)
#else
#define S_LOG(format, ...)     
#endif

SkillHelperBase::SkillHelperBase(SkillHelperBaseAdapter* adpater)
{
	mProvider = adpater;
}

void SkillHelperBase::setSkillHelperBaseProvider(SkillHelperBaseAdapter* provider)
{
	this->mProvider = provider;
}

SkillFight SkillHelperBase::fight(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight data;

	switch (skill.skillType)
	{
	// �츳����
	case SkillType_BaoLieZhan:
		S_LOG("SkillType_BaoLieZhan");
		data = fight_BaoLieZhan(fightor,skill);
		break;
	case SkillType_ShaLu:
		S_LOG("SkillType_ShaLu");
		data = fight_ShaLu(fightor,skill);
		break;
	case SkillType_ShanDian:
		S_LOG("SkillType_ShanDian");
		data = fight_ShanDian(fightor,skill);
		break;
	case SkillType_JianShou:
		S_LOG("SkillType_JianShou ---> ShengCai");
		//data = fight_JianShou(fightor,skill);
		data = fight_ShengCai(fightor,skill);
		break;
	case SkillType_ShenZhiXingNian:
		S_LOG("SkillType_ShenZhiXingNian");
		//data = fight_ShenZhiXingNian(fightor,skill);
		data = fight_ShenZhiXingNian2(fightor,skill);
		break;
	case SkillType_ShunYinTuXi:
		S_LOG("SkillType_ShunYinTuXi");
		data = fight_ShunYinTuXi(fightor,skill);
		break;
	case SkillType_LieGong:
		S_LOG("fight_LieGong");
		data = fight_LieGong(fightor,skill);
		break;
	case SkillType_JianYu:
		S_LOG("SkillType_JianYu");
		data = fight_JianYu(fightor,skill);
		break;
	case SkillType_HuoQiuShu:
		S_LOG("SkillType_HuoQiuShu");
		//data = fight_HuoQiuShu(fightor,skill);
		data = fight_HuoQiuShu2(fightor,skill);
		break;
	case SkillType_WenYi:
		S_LOG("SkillType_WenYi");
		//data = fight_WenYi(fightor,skill);
		data = fight_WenYi2(fightor,skill);
		break;

	// ������1
	case SkillType_BanYueZhan:
		S_LOG("SkillType_BanYueZhan");
		data = fight_BanYueZhan(fightor,skill);
		break;
	case SkillType_CiSha:
		S_LOG("SkillType_CiSha");
		data = fight_CiSha(fightor,skill);
		break;
	case SkillType_LeiBao:
		S_LOG("SkillType_LeiBao");
		data = fight_LeiBao(fightor,skill);
		break;
	case SkillType_MangCi:
		S_LOG("SkillType_MangCi");
		data = fight_MangCi(fightor,skill);
		break;
	case SkillType_ShenZhiZhuFu:
		S_LOG("SkillType_ShenZhiZhuFu");
		data = fight_ShenZhiZhuFu(fightor,skill);
		break;
	case SkillType_LiRenHuiXuan:
		S_LOG("SkillType_LiRenHuiXuan");
		data = fight_LiRenHuiXuan(fightor,skill);
		break;
	case SkillType_JingZhunZhiCan:
		S_LOG("SkillType_JingZhunZhiCan");
		data = fight_JingZhunZhiCan(fightor,skill);
		break;
	case SkillType_JiSuDuJian:
		S_LOG("SkillType_JiSuDuJian");
		data = fight_JiSuDuJian(fightor,skill);
		break;
	case SkillType_BaoFengXue:
		S_LOG("SkillType_BaoFengXue");
		data = fight_BaoFengXue(fightor,skill);
		break;
	case SkillType_KongJu:
		S_LOG("SkillType_KongJu");
		data = fight_KongJu(fightor,skill);
		break;

	// ������2
	case SkillType_KuangBao:
		S_LOG("SkillType_KuangBao");
		data = fight_KuangBao(fightor,skill);
		break;
	case SkillType_ShiXueZhiRen:
		S_LOG("SkillType_ShiXueZhiRen");
		data = fight_ShiXueZhiRen(fightor,skill);
		break;
	case SkillType_YunShi:
		S_LOG("SkillType_YunShi");
		data = fight_YunShi(fightor,skill);
		break;
	case SkillType_ShouHu:
		S_LOG("SkillType_ShouHu");
		data = fight_ShouHu(fightor,skill);
		break;
	case SkillType_ZhongShenZhiLi:
		S_LOG("SkillType_ZhongShenZhiLi");
		data = fight_ZhongShenZhiLi(fightor,skill);
		break;
	case SkillType_XuanFengZhan:
		S_LOG("SkillType_XuanFengZhan");
		data = fight_XuanFengZhan(fightor,skill);
		break;
	case SkillType_ZhuiHunDuJian:
		S_LOG("SkillType_ZhuiHunDuJian");
		data = fight_ZhuiHunDuJian(fightor,skill);
		break;
	case SkillType_SiWangSaoShe:
		S_LOG("SkillType_SiWangSaoShe");
		data = fight_SiWangSaoShe(fightor,skill);
		break;
	case SkillType_HanBingCiGu:
		S_LOG("SkillType_HanBingCiGu");
		data = fight_HanBingCiGu(fightor,skill);
		break;
	case SkillType_ShiHun:
		S_LOG("SkillType_ShiHun");
		data = fight_ShiHun(fightor,skill);
		break;

	// ������3
	case SkillType_BaoNu:
		S_LOG("SkillType_BaoNu");
		data = fight_BaoNu(fightor,skill);
		break;
	case SkillType_YinZheHun:
		S_LOG("SkillType_YinZheHun");
		data = fight_YinZheHun(fightor,skill);
		break;
	case SkillType_TianTangZhiGuang:
		S_LOG("SkillType_TianTangZhiGuang");
		data = fight_TianTangZhiGuang(fightor,skill);
		break;
        
    // ����
	case SkillType_Monster_XieE:
		S_LOG("SkillType_Monster_XieE");
		data = fight_Monster_XieE(fightor,skill);
		break;
        
    // ����
	case SkillType_BOSS_Sweep:
		S_LOG("SkillType_BOSS_Sweep");
		data = fight_BOSS_Sweep(fightor, skill);
		break;
	case SkillType_BOSS_Flog:
		S_LOG("SkillType_BOSS_Flog");
		data = fight_BOSS_Flog(fightor, skill);
		break;
	default:
		S_LOG("%s line:%d This Skill [type = %d] is not implement!",__FUNCTION__,__LINE__,skill.skillType);
		CCAssert(false,"û��ʵ��");
		break;
	}

	// Ŀǰͳһ����Ϊ����ʾ����
	data.skillPatten = Skill_Noraml;
	// ���飺
	{
		CCAssert(data.canMissVec.size() == data.targetVec.size(),"����Ӧ��һ��");
		CCAssert(data.targetBloodChange.size() == data.targetVec.size(),"����Ӧ��һ��");
		if (!data.firedBuff.empty())
		{
			int buffNum = data.firedBuff.size();
			for (int i=0;i<buffNum;i++)
			{
				bool found = false;
				int targetsNum = data.targetVec.size();
				for (int j=0;j<targetsNum;j++)
				{
					if (data.targetVec[j] == data.firedBuff[i].attachedToUId)
					{
						found = true;
						break;
					}
				}
				CCAssert(found,"�����buff����Ҫ���ӵ����˱�����target��");
			}
		}
	}


	return data;
}


SkillFight SkillHelperBase::fight_BaoLieZhan(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ������ͨ�����ķ�ʽ
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 2.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}

SkillFight SkillHelperBase::fight_ShaLu(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = "ShaLu";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ������ͨ�����ķ�ʽ
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 2.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}

SkillFight SkillHelperBase::fight_ShanDian(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f; 
	f.nameForLog = "ShanDian";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ������ͨ�����ķ�ʽ
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 2.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}
	return f;
}
SkillFight SkillHelperBase::fight_ShengCai(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	vector<ChessBoardItemBase*> targets = mProvider->getFrontRowAlivedEnemyItem(fightor);
	if (targets.empty())
	{
		targets = mProvider->getBackRowAlivedEnemyItem(fightor);
		CCAssert(!targets.empty(),"");
	}
	for (int i=0,count=targets.size(); i<count; i++)
	{
		// ����
		int skillHp = fightor->hero->getShangHaiValue(targets[i]->hero) * 1.5f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)targets[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(targets[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}
SkillFight SkillHelperBase::fight_JianShou(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	CCAssert(false,"��ʱ�����ˣ�����ShengCai");

	CCAssert(dynamic_cast<Hero*>(fightor->hero)!=NULL || dynamic_cast<TeamHero*>(fightor->hero)!=NULL,"ֻ��Hero�ſ������������");

	SkillFight f;
	f.nameForLog = "JianShou";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	//�������״̬�������������40%���������֡�
	//����״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	{
		CCAssert(fightor->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff�������buff����Ӻͺ����û�����⣬�Ƽ��Ǻ���ӣ���Ϊ�ǹ����˲��ܼ�buff��
		// ����ֻ�ܸ��Ӽ���
		// �������ĸ��ӵ�ʱ�򣬻��ж�Ѫֵ��
		// �������buff������һ����ӵ���ɫ����ȥ
		// ��Ϊ���ﲻ�жϵ�Ѫ���Ƿ����0��û���ã���Ϊ����û�и�����ȥ�������ں���żӵģ����������ӵ�ʱ��Ҳ������buff��Ӱ�죬
		// ʹ�������ɫ�Ѿ�����
		//GameBuffEffect effect(mProvider->genOneBuffEffectId());
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.jianshang_lv = 40 * skillResultPercent;
		//effect.remove_pos = Pos_Self_BeAttacked;
		//
		// 2014-03-03�޸ģ����е��Ƴ��㶼�ƶ������ִ��滻�ĵط���
		// 
		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_JianShou,fightor->getUniqueId(),fightor->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3��Ч��

		f.firedBuff.push_back(buff);
	}
	

	return f;
}

SkillFight SkillHelperBase::fight_ShenZhiXingNian(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	//CCAssert(dynamic_cast<Hero*>(fightor->hero)!=NULL,"ֻ��Hero�ſ������������");
	CCAssert(dynamic_cast<Hero*>(fightor->hero)!=NULL || dynamic_cast<TeamHero*>(fightor->hero)!=NULL,"ֻ��Hero�ſ������������");


	SkillFight f;
	f.nameForLog = "ShenZhiXingNian";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	PropertyCalculator calculate;
	// ��������20%������3�֡�
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedTeamItems(fightor);;
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff�������buff����Ӻͺ����û�����⣬�Ƽ��Ǻ���ӣ���Ϊ�ǹ����˲��ܼ�buff��
		// ����ֻ�ܸ��Ӽ���
		// �������ĸ��ӵ�ʱ�򣬻��ж�Ѫֵ��
		// �������buff������һ����ӵ���ɫ����ȥ
		// ��Ϊ���ﲻ�жϵ�Ѫ���Ƿ����0��û���ã���Ϊ����û�и�����ȥ�������ں���żӵģ����������ӵ�ʱ��Ҳ������buff��Ӱ�죬
		// ʹ�������ɫ�Ѿ�����
		int itemHuJiaZhi = calculate.calculateHuJiaZhi(((Hero*)allItems[i]->hero)->getCalculateData());
		
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.jianshang_lv = (20.0f * skillResultPercent);
		//effect.remove_pos = Pos_Self_BeAttacked;
		//
		// 2014-03-03�޸ģ����е��Ƴ��㶼�ƶ������ִ��滻�ĵط���
		// 
		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_ShenZhiPiHu,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3��Ч��

		// ���
		f.firedBuff.push_back(buff);
	}

	return f;
}

SkillFight SkillHelperBase::fight_ShenZhiXingNian2(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	PropertyCalculator calculate;
	// 2014-03-25�����������滻fight_ShenZhiXingNian
	// ʹ�������е�λ������֮����״̬�����Ŀ���൱��������20%�Ĺ�����������2�غϡ���״̬���ɵ��ӣ�

	float fightor_gongjili = fightor->hero->getGongJiLi(Range_Average);

	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedTeamItems(fightor);;
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		float self_gongjili = allItems[i]->hero->getGongJiLi(Range_Average);
		float addPer = fightor_gongjili*0.2f / self_gongjili; // ����20%������ռ���ҵĶ���
		addPer *= 100;

		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		float lv = addPer * skillResultPercent;
		// ������1���ϣ�����0�Ĳ���
		if (lv < 0 && lv > -1)
			lv = -1;
		if (lv > 0 && lv < 1) 
			lv = 1;
		effect.gongjili_add_lv = lv;

		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_ShenZhiXinNian,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		// ���
		f.firedBuff.push_back(buff);
	}

	return f;
}

SkillFight SkillHelperBase::fight_ShunYinTuXi(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = "ShunYinTuXi";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ������ͨ�����ķ�ʽ
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 3.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}

SkillFight SkillHelperBase::fight_LieGong(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = "LieGong";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// �������������ơ�
	// ������Χ���壬���ȹ����������ߺͱ����ߡ�
	// �˺�ֵΪ�չ�2����
	ChessBoardItemBase* target = NULL;
	{
		// ����Χ���е�
		vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
		target = mProvider->getWeaklessAndDyingItem(allItems,fightor);
	}
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 2.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}


	return f;
}

SkillFight SkillHelperBase::fight_JianYu(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = "JianYu";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ��������2.5
	// ������������̷�Χ�ڵĵз���λ��
	// �˺�ֵΪ�չ�2����
	// 
	// --> ����
	// �����Ϊ������2.5���ڵ��˺�ֵΪ�չ�1.5�����������2.5���˺�ֵΪ�չ�1����
	//
	// --> ����
	// --> �������ез��ܵ����˺�ȫ����Ϊ1���չ���������޹ء�
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		//if (mProvider->distance(fightor,allItems[i]) > 2.5f)
		//	continue;

		float beishu = 1.0f;
		//if (mProvider->distance(fightor,allItems[i]) <= 2.5f)
		//{
		//	beishu = 1.5f;
		//}

		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * beishu * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}
SkillFight SkillHelperBase::fight_HuoQiuShu2(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// �����������Ŀ�꣬���150%���˺�������20%���ʻ���Ŀ��(����״̬,���ɵ���)��ʹ��ֹͣ�ж�������2�غ�

	// ���ѡ2�����ظ��ģ���������һ��
	vector<ChessBoardItemBase*> targets;
	{
		vector<ChessBoardItemBase*> items = mProvider->getAlivedEnemyItems(fightor);
		CCAssert(!items.empty(),"");
		int index = (items.size()-1)*lfRandom0_1();
		targets.push_back(items[index]);

		if (items.size() >= 2)
		{
			// ȥ��ѡ����
			vector<ChessBoardItemBase*>::iterator it = items.begin();
			for (;it != items.end(); it++)
			{
				if (index == 0)
				{
					items.erase(it);
					break;
				}
				index --;
			}

			// ���ѡ�ڶ���
			int index = (items.size()-1)*lfRandom0_1();
			targets.push_back(items[index]);
		}
	}
	
	for (int i=0,count=targets.size(); i<count; i++)
	{
		CCAssert(targets[i]->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(targets[i]->hero) * 1.5f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)targets[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(targets[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);

        
        /* 2014.06.03 �޸Ĳ��ܴ˼��ܵ�ѣ��Ч��*/  
        if((targets[i]->hero->getHeroType()== Type_Guaiwu_jingying)
            && (targets[i]->hero->getXSize() > 1)) {  
            continue;
        }
        
		if (lfRandom0_1() < 0.2f)
		{
			// buffЧ��
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.xuanyun = true;
			effect.remove_pos = Pos_ChangeRound; // ���е�buffĿǰ����������ط��Ƴ�

			GameBuff buff(BuffState_JiYun,fightor->getUniqueId(),targets[i]->getUniqueId(),false); 
			buff.effect_vec.push_back(effect);

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 2��Ч��

			f.firedBuff.push_back(buff);
		}
	}

	return f;
}
SkillFight SkillHelperBase::fight_HuoQiuShu(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = "HuoQiuShu";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ��������2.
	// ������Χ���壬���ȹ�����ǰ����λ��
	// �˺�ֵΪ�չ���2����
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 2.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}
SkillFight SkillHelperBase::fight_WenYi2(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	float fightor_gongjili = fightor->hero->getGongJiLi(Range_Average);

	// 2014-03-25����ӣ������滻fight_WenYi
	//ʹ�з����е�λ��������״̬������Ŀ���൱����ʦ������20%�Ĺ�����������2��غϡ���״̬���ɵ��ӣ�
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		float self_gongjili = allItems[i]->hero->getGongJiLi(Range_Average);
		float addPer = fightor_gongjili* (-0.2f) / self_gongjili; // ����20%������ռ���ҵĶ���
		addPer *= 100;

		// buff�������buff����Ӻͺ����û�����⣬�Ƽ��Ǻ���ӣ���Ϊ�ǹ����˲��ܼ�buff��
		// ����ֻ�ܸ��Ӽ���
		// �������ĸ��ӵ�ʱ�򣬻��ж�Ѫֵ��
		// �������buff������һ����ӵ���ɫ����ȥ
		// ��Ϊ���ﲻ�жϵ�Ѫ���Ƿ����0��û���ã���Ϊ����û�и�����ȥ�������ں���żӵģ����������ӵ�ʱ��Ҳ������buff��Ӱ�죬
		// ʹ�������ɫ�Ѿ�����
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		float lv = addPer * skillResultPercent;
		// ������1���ϣ�����0�Ĳ���
		if (lv < 0 && lv > -1)
			lv = -1;
		if (lv > 0 && lv < 1) 
			lv = 1;
		effect.gongjili_add_lv = lv;

		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_WenYi,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		// ���
		f.firedBuff.push_back(buff);
	}

	return f;
}
SkillFight SkillHelperBase::fight_WenYi(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = "WenYi";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// �Է����е�λ��������״̬�����ͷ���20%������3�֡�
	// ����״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff�������buff����Ӻͺ����û�����⣬�Ƽ��Ǻ���ӣ���Ϊ�ǹ����˲��ܼ�buff��
		// ����ֻ�ܸ��Ӽ���
		// �������ĸ��ӵ�ʱ�򣬻��ж�Ѫֵ��
		// �������buff������һ����ӵ���ɫ����ȥ
		// ��Ϊ���ﲻ�жϵ�Ѫ���Ƿ����0��û���ã���Ϊ����û�и�����ȥ�������ں���żӵģ����������ӵ�ʱ��Ҳ������buff��Ӱ�죬
		// ʹ�������ɫ�Ѿ�����
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.jianshang_lv = -20.0f * skillResultPercent;
		//effect.remove_pos = Pos_Self_BeAttacked;
		//
		// 2014-03-03�޸ģ����е��Ƴ��㶼�ƶ������ִ��滻�ĵط���
		// 
		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_WenYi,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3��Ч��

		// ���
		f.firedBuff.push_back(buff);
	}

	return f;
}



// ������1
SkillFight SkillHelperBase::fight_BanYueZhan(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// �����������Լ�����Ϊ1��1.5�ĵз���λ��
	// ����Ϊ1�����е�λ��̯2���չ��˺���
	// ����Ϊ1.5�����е�λ��̯1���չ��˺���
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 1.5f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");

		if (distance_temp <= 1.0f)
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 2.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
		else
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 1.5f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
	}



	return f;
}
SkillFight SkillHelperBase::fight_CiSha(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ������ǰ������1��2�����ез���λ����ǰ������
	// ����Ϊ1�ĵ�λ�����չ�2���˺���
	// ����Ϊ2�ĵ�λ�����չ�1���˺���
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();

	bool chooseCol = false;	// �Ƿ�ѡ��һ�У����һ�ж�û���ٳ���ѡ��һ��
	for (int i=0;i<num;i++)
	{
		if (!mProvider->isInCol(fightor,allItems[i]))
			continue;
		chooseCol = true; // ������һ���ϣ���ѡ��һ��
	}

	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		// ��Ҫ��������һ�У���ô������ǰ���ģ�����
		if (chooseCol && !mProvider->isInCol(fightor,allItems[i]))
			continue;
		// ��Ҫ�����������ţ���ô����һ�ŵĲ���
		if (!chooseCol && !mProvider->isInRow(fightor,allItems[i]))
			continue;

		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 2.0f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");

		if (distance_temp <= 1.0f)
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 2.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
		else
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 1.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
	}


	return f;
}
SkillFight SkillHelperBase::fight_LeiBao(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ��������2.5���ڵ����ез���λ��
	// ����2�����ڵ�λ��̯2���չ��˺���
	// ����2.5�ĵ�λ��̯1���չ��˺���
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 2.5f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");

		if (distance_temp <= 2.0f)
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 2.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
		else
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 1.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
	}



	return f;
}
SkillFight SkillHelperBase::fight_MangCi(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ����â��״̬�������������30%�����ɷ��������������˺�ֵ��20%������3�֡�
	// â��״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	{
		CCAssert(fightor->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff�������buff����Ӻͺ����û�����⣬�Ƽ��Ǻ���ӣ���Ϊ�ǹ����˲��ܼ�buff��
		// ����ֻ�ܸ��Ӽ���
		// �������ĸ��ӵ�ʱ�򣬻��ж�Ѫֵ��
		// �������buff������һ����ӵ���ɫ����ȥ
		// ��Ϊ���ﲻ�жϵ�Ѫ���Ƿ����0��û���ã���Ϊ����û�и�����ȥ�������ں���żӵģ����������ӵ�ʱ��Ҳ������buff��Ӱ�죬
		// ʹ�������ɫ�Ѿ�����
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.fantan_lv = 20*skillResultPercent;
		effect.jianshang_lv = 30*skillResultPercent;
		effect.remove_pos = Pos_Self_BeAttacked;

		GameBuff buff(BuffState_MangCi,fightor->getUniqueId(),fightor->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3��Ч��

		f.firedBuff.push_back(buff);
	}

	return f;
}
SkillFight SkillHelperBase::fight_ShenZhiZhuFu(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ����ȫ�������֮ף��״̬��������֮ף��״̬�Ľ�ɫÿ�ֿɻָ�����7%Ѫ��������3�֡�
	// ��֮ף��״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedTeamItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		float hp_add = allItems[i]->hero->getHp() * 0.07f * skillResultPercent;

		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.persistent_hp_add = hp_add;
		effect.remove_pos = Pos_Step;

		GameBuff buff(BuffState_ShenZhiZhuFu,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3��Ч��

		// ���
		f.firedBuff.push_back(buff);
	}






	return f;
}
// ���ݣ���ǰ�Ĵ���ţ�������
// Ҫ�ĳɴ����ţ���б��
//SkillFight SkillHelperBase::fight_LiRenHuiXuan(ChessBoardItemBase* fightor,SkillDataForFight skill)
//{
//	SkillFight f;
//	f.nameForLog = string(__FUNCTION__);
//	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
//	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
//	// Ч������
//	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;
//
//	// ������ǰ������1��2�����ез���λ��
//	// ����Ϊ1�ĵ�λ�����չ�2���˺���
//	// ����Ϊ2�ĵ�λ�����չ�1���˺���
//	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
//	int num = allItems.size();
//
//
//	bool chooseCol = false;	// �Ƿ�ѡ��һ�У����һ�ж�û���ٳ���ѡ��һ��
//	for (int i=0;i<num;i++)
//	{
//		if (!mProvider->isInCol(fightor,allItems[i]))
//			continue;
//		chooseCol = true; // ������һ���ϣ���ѡ��һ��
//	}
//
//
//	float distance_temp = 0;
//	for (int i=0;i<num;i++)
//	{
//		// ��Ҫ��������һ�У���ô������ǰ���ģ�����
//		if (chooseCol && !mProvider->isInCol(fightor,allItems[i]))
//			continue;
//		// ��Ҫ�����������ţ���ô����һ�ŵĲ���
//		if (!chooseCol && !mProvider->isInRow(fightor,allItems[i]))
//			continue;
//
//		distance_temp = mProvider->distance(fightor,allItems[i]);
//		if (distance_temp > 2.0f)
//			continue;
//		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
//
//		if (distance_temp <= 1.0f)
//		{
//			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 1.5f * skillResultPercent;
//			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
//			f.targetVec.push_back(allItems[i]->getUniqueId());
//			f.targetBloodChange.push_back(-skillHp);
//			f.canMissVec.push_back(canMiss);
//		}
//		else
//		{
//			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 1.0f * skillResultPercent;
//			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
//			f.targetVec.push_back(allItems[i]->getUniqueId());
//			f.targetBloodChange.push_back(-skillHp);
//			f.canMissVec.push_back(canMiss);
//		}
//	}
//	return f;
//}

SkillFight SkillHelperBase::fight_LiRenHuiXuan(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	CCAssert(dynamic_cast<ChessBoardItem*>(fightor)!=NULL,"ĿǰΪ�˷��㣬ֻ��4X4���̲ſ������������");

	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ������ǰ������1��2�����ез���λ��
	// ����Ϊ1�ĵ�λ�����չ�2���˺���
	// ����Ϊ2�ĵ�λ�����չ�1���˺���
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();


	bool chooseCol = false;	// �Ƿ�ѡ��һ�У����һ�ж�û���ٳ���ѡ��һ��
	for (int i=0;i<num;i++)
	{
		if (!mProvider->isInCol(fightor,allItems[i]))
			continue;
		chooseCol = true; // ������һ���ϣ���ѡ��һ��
	}
	
	for (int i=0;i<num;i++)
	{
		// ����Ǵ�һ�У���ô���ж��Ƿ���һ��
		// ������Ǵ�һ�У���ô��ʵҪ�����Ķ�������һ�У����ǿ��Լ򵥵㣬���Ǳ���������
		//
		// ��Ҫ��������һ�У���ô������ǰ���ģ�����
		if (chooseCol && !mProvider->isInCol(fightor,allItems[i]))
			continue;
		
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");

		float effectForRow = 1.0f;
		// ǰ��
		if ( ((ChessBoardItem*)allItems[i])->posy == 0 )
		{
			effectForRow = 1.5f;
		}
		else
		{// ����
			effectForRow = 1.0f;
		}

		int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * effectForRow * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}

SkillFight SkillHelperBase::fight_JingZhunZhiCan(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ȫ���ķ�Χ
	// ������Χ���壬���ȹ����������ߺͱ����ߡ�
	// �˺�ֵΪ�չ�3��������50%����ʹ�Է��²У�ֹͣ�ж�һ�֡�
	ChessBoardItemBase* target = NULL;
	{
		// ����Χ���е�
		vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
		target = mProvider->getWeaklessAndDyingItem(allItems,fightor);
	}
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 3.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);

		if (lfRandom0_1() < 0.5f)
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.xuanyun = true;
			effect.remove_pos = Pos_ChangeRound;

			GameBuff buff(BuffState_WhatEver,fightor->getUniqueId(),target->getUniqueId(),true); 
			buff.effect_vec.push_back(effect);

			// ���
			f.firedBuff.push_back(buff);
		}
	}




	return f;
}
SkillFight SkillHelperBase::fight_JiSuDuJian(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ��������2.5
	// ������������̷�Χ�ڵĵз���λ��
	// �˺�ֵΪ�չ�2�������ǶԷ������Ż�״̬����������Ѫ���֣��˺��൱���չ�1������
	// �Ż�״ֻ̬�ɸ��ǲ��ɵ���
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		if (mProvider->distance(fightor,allItems[i]) > 2.5f)
			continue;

		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		float hp_sub = fightor->hero->getShangHaiValue(allItems[i]->hero);
		int skillHp = hp_sub * 2.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);

		
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.persistent_hp_add = -hp_sub * skillResultPercent;
		effect.remove_pos = Pos_Step;

		GameBuff buff(BuffState_ZhongDu,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);


		// ���
		f.firedBuff.push_back(buff);
	}


	return f;
}
SkillFight SkillHelperBase::fight_BaoFengXue(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ��������2.5���ڵ����ез���λ��
	// ����2�����ڵ�λ��̯2���չ��˺���
	// ����2.5�ĵ�λ��̯1���չ��˺���
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 2.5f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");

		if (distance_temp <= 2.0f)
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 2.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
		else
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 1.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
	}





	return f;
}
SkillFight SkillHelperBase::fight_KongJu(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// �Է����е�λ70%���ʽ���־�״̬��
	// ���͹���30%������3�֡�
	// �־�״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		// 70%�ļ��ʽ���־�
		if (lfRandom0_1() > 0.7f)
			continue;

		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);


		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.gongjili_add_lv = -30 * skillResultPercent;
		effect.remove_pos = Pos_Self_Fight_End;

		GameBuff buff(BuffState_KongJu,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3��Ч��

		// ���
		f.firedBuff.push_back(buff);
	}



	return f;
}

// ������2
SkillFight SkillHelperBase::fight_KuangBao(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ��������1��
	// ������Χ���壬���ȹ�����ǰ����λ��
	// �˺�ֵΪ�չ���2������50%���ʽ��Է�����һ�֡�
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 2.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);

		// ��Ϊ�ǵ��幥�������Կ���ֻ�������������
		if (lfRandom0_1() < 0.5f)
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.xuanyun = true;
			effect.remove_pos = Pos_Self_Will_fight;

			GameBuff buff(BuffState_WhatEver,fightor->getUniqueId(),target->getUniqueId(),false); 
			buff.effect_vec.push_back(effect);

			f.firedBuff.push_back(buff);
		}
	}


	return f;
}
SkillFight SkillHelperBase::fight_ShiXueZhiRen(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ��������1.
	// ������Χ���壬���ȹ�����ǰ����λ��
	// �˺�ֵΪ�չ�1.5����ʹ�Է�������Ѫ״̬���������ֳ�����Ѫ���˺��൱���չ���1������
	// ��Ѫ״ֻ̬�ܸ��ǲ��ɵ��ӡ�
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		float hp_sub = fightor->hero->getShangHaiValue(target->hero) * skillResultPercent;
		int skillHp = hp_sub * 1.5f;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);

		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.persistent_hp_add = -hp_sub;
		effect.remove_pos = Pos_Step;

		GameBuff buff(BuffState_LiuXue,fightor->getUniqueId(),target->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		f.firedBuff.push_back(buff);
	}




	return f;
}
SkillFight SkillHelperBase::fight_YunShi(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ��������2��
	// ����������Χ���壬���ȹ�����ǰ����λ��
	// �˺�ֵΪ�չ�3������50%���ʽ��Է�����һ�֡�
	// ������ͨ�����ķ�ʽ
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 3.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);

		if (lfRandom0_1() < (0.5f * skillResultPercent) )
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.xuanyun = true;
			effect.remove_pos = Pos_Self_Will_fight;

			GameBuff buff(BuffState_WhatEver,fightor->getUniqueId(),target->getUniqueId(),false); 
			buff.effect_vec.push_back(effect);

			f.firedBuff.push_back(buff);
		}
	}




	return f;
}
SkillFight SkillHelperBase::fight_ShouHu(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	//�����ػ���״̬����һ�й�������һ�֡�����������������50%��
	{
		CCAssert(fightor->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// ��������
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.jianshang_lv = 100; // ����Ͳ���������
			effect.remove_pos = Pos_Self_BeAttacked;

			GameBuff buff(BuffState_ShouHuZhe1,fightor->getUniqueId(),fightor->getUniqueId()); 
			buff.effect_vec.push_back(effect);

			f.firedBuff.push_back(buff);
		}

		// �ӹ���
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.gongjili_add_lv = 50 * skillResultPercent;
			effect.remove_pos = Pos_Self_Fight_End;

			GameBuff buff(BuffState_ShouHuZhe2,fightor->getUniqueId(),fightor->getUniqueId()); 
			buff.effect_vec.push_back(effect);

			f.firedBuff.push_back(buff);
		}
		
	}


	return f;
}
SkillFight SkillHelperBase::fight_ZhongShenZhiLi(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ����ȫ���������֮��״̬����������20%������3�֡�
	// ����֮��ֻ�ɸ��ǲ��ɵ��ӡ�
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedTeamItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff�������buff����Ӻͺ����û�����⣬�Ƽ��Ǻ���ӣ���Ϊ�ǹ����˲��ܼ�buff��
		// ����ֻ�ܸ��Ӽ���
		// �������ĸ��ӵ�ʱ�򣬻��ж�Ѫֵ��
		// �������buff������һ����ӵ���ɫ����ȥ
		// ��Ϊ���ﲻ�жϵ�Ѫ���Ƿ����0��û���ã���Ϊ����û�и�����ȥ�������ں���żӵģ����������ӵ�ʱ��Ҳ������buff��Ӱ�죬
		// ʹ�������ɫ�Ѿ�����
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.gongjili_add_lv = 20 * skillResultPercent;
		effect.remove_pos = Pos_Self_BeAttacked;

		GameBuff buff(BuffState_ZhongShenZhiLi,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3��Ч��

		// ���
		f.firedBuff.push_back(buff);
	}


	return f;
}
SkillFight SkillHelperBase::fight_XuanFengZhan(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// �����������Լ�����Ϊ1��1.5�ĵз���λ��
	// ����Ϊ1�����е�λ��̯2���չ��˺�������Ϊ1.5�����е�λ��̯1���չ��˺���
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 1.5f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");

		if (distance_temp <= 1.0f)
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 2.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
		else
		{
			int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 1.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(allItems[i]->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);
		}
	}



	return f;
}
SkillFight SkillHelperBase::fight_ZhuiHunDuJian(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// �������������ơ�
	// ������Χ���壬���ȹ����������ߺͱ����ߡ�
	// �˺�ֵΪ�չ�4����ʹ�Է������ж�״̬���������ֵ�Ѫ���˺��൱���չ���1�������ж�״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	ChessBoardItemBase* target = NULL;
	{
		// ����Χ���е�
		vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
		target = mProvider->getWeaklessAndDyingItem(allItems,fightor);
	}
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		float hp_sub = fightor->hero->getShangHaiValue(target->hero) * skillResultPercent;
		int skillHp = hp_sub * 4;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);


		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.persistent_hp_add = -hp_sub;
		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_ZhongDu,fightor->getUniqueId(),target->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect); // ��2��


		// ���
		f.firedBuff.push_back(buff);
	}


	return f;
}
SkillFight SkillHelperBase::fight_SiWangSaoShe(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ��������2.5
	// ������������̷�Χ�ڵĵз���λ��
	// �˺�ֵΪ�չ�3.0������ʹ�Է�����־�״̬�����͹���30%������3�֡�
	// �־�״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		if (mProvider->distance(fightor,allItems[i]) > 2.5f)
			continue;

		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		float hp_sub = fightor->hero->getShangHaiValue(allItems[i]->hero);
		int skillHp = hp_sub * 3.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);


		//// ��ʹ�Է�35%���ʽ���־�״̬
		//if (lfRandom0_1()<0.35f)
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.gongjili_add_lv = -30 * skillResultPercent;
			effect.remove_pos = Pos_Self_Fight_End;

			GameBuff buff(BuffState_KongJu,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
			buff.effect_vec.push_back(effect);

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 2��Ч��

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 3��Ч��

			// ���
			f.firedBuff.push_back(buff);

		}
	}



	return f;
}
SkillFight SkillHelperBase::fight_HanBingCiGu(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ��������2��
	// ����������Χ���壬���ȹ�����ǰ����λ��
	// �˺�ֵΪ�չ�2������50%���ʽ��Է�����һ�֡�
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"�쳣");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 2.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);


		if (lfRandom0_1() < 0.5f)
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.xuanyun = true;
			effect.remove_pos = Pos_Self_Will_fight;

			GameBuff buff(BuffState_WhatEver,fightor->getUniqueId(),target->getUniqueId(),false); 
			buff.effect_vec.push_back(effect);

			f.firedBuff.push_back(buff);
		}
		
	}



	return f;
}
SkillFight SkillHelperBase::fight_ShiHun(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;
	CCLog("Waring : %s ---> skill effect add not use!",__FUNCTION__);

	//�Է����е�λ70%���ʽ���־�״̬�����͹���30%������3�֡�
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		// 70%�ļ��ʽ���־�
		if (lfRandom0_1() > 0.5f)
			continue;

		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);


		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.xuanyun = true;
		effect.remove_pos = Pos_Self_Fight_End;

		GameBuff buff(BuffState_ShiHun,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		// ���
		f.firedBuff.push_back(buff);
	}



	return f;
}

// ������3
SkillFight SkillHelperBase::fight_BaoNu(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ���뱩ŭ״̬�������������30%������������30%������3�֡�
	// �����˼��ܺ������������������չ��򱩻�����
	// ��ŭ״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	{
		CCAssert(fightor->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff�������buff����Ӻͺ����û�����⣬�Ƽ��Ǻ���ӣ���Ϊ�ǹ����˲��ܼ�buff��
		// ����ֻ�ܸ��Ӽ���
		// �������ĸ��ӵ�ʱ�򣬻��ж�Ѫֵ��
		// �������buff������һ����ӵ���ɫ����ȥ
		// ��Ϊ���ﲻ�жϵ�Ѫ���Ƿ����0��û���ã���Ϊ����û�и�����ȥ�������ں���żӵģ����������ӵ�ʱ��Ҳ������buff��Ӱ�죬
		// ʹ�������ɫ�Ѿ�����
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.gongjili_add_lv = 30 * skillResultPercent;
		effect.baoji_add = 30 * skillResultPercent;
		effect.remove_pos = Pos_Self_Fight_End;

		GameBuff buff(BuffState_BaoNu,fightor->getUniqueId(),fightor->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3��Ч��

		f.firedBuff.push_back(buff);
	}


	return f;
}
SkillFight SkillHelperBase::fight_YinZheHun(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ��Ϊ�����ʺ������ʵ��Ƴ��㲻һ��
	// 
	// ��������״̬��������������30%������������50%������3�֡������˼��ܺ������������������չ��򱩻�����
	// ����״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	{
		CCAssert(fightor->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// �ӱ���
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.baoji_add = 30 * skillResultPercent;
			effect.remove_pos = Pos_Self_Fight_End;

			GameBuff buff(BuffState_BaoNu,fightor->getUniqueId(),fightor->getUniqueId()); 
			buff.effect_vec.push_back(effect);

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 2��Ч��

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 3��Ч��

			f.firedBuff.push_back(buff);
		}

		// ������
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.shanbi_add = 30 * skillResultPercent;
			effect.remove_pos = Pos_Self_BeAttacked;

			GameBuff buff(BuffState_BaoNu,fightor->getUniqueId(),fightor->getUniqueId()); 
			buff.effect_vec.push_back(effect);

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 2��Ч��

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 3��Ч��

			f.firedBuff.push_back(buff);
		}
		
	}



	return f;
}
SkillFight SkillHelperBase::fight_TianTangZhiGuang(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// ��������ʹ��״̬��
	// 
	// ������������50%����3�֡� ����Ҳ������һ�ξ��õ��������ˣ���ô���滹ʣ2�Σ�
	// Ȼ�󷢶���������������Ϊ2���˺�ֵΪ�չ�3����
	// ����ʹ��״ֻ̬�ɸ��ǲ��ɵ��ӡ�
	// ����������Χ���壬���ȹ�����ǰ����λ
	{
		ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
		if (target!=NULL)
		{ 
			CCAssert(target->hero->getHp() > 0,"�쳣");

			// �ڲ�������һ��buff,Ȼ�������Ƴ�,���ڱ��ε�buffЧ��
			GameBuff buff(BuffState_WhatEver,fightor->getUniqueId(),fightor->getUniqueId()); 
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.gongjili_add_lv = 50 * skillResultPercent;
			effect.remove_pos = Pos_Self_Fight_End;
			buff.effect_vec.push_back(effect);
			fightor->hero->attachBuff(buff);

			int skillHp = fightor->hero->getShangHaiValue(target->hero) * 3.0f * skillResultPercent;
			bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
			f.targetVec.push_back(target->getUniqueId());
			f.targetBloodChange.push_back(-skillHp);
			f.canMissVec.push_back(canMiss);

			// �Ƴ�
			fightor->hero->removeBuff(effect.effect_id);
		}

		// �ԼҼ�buff
		CCAssert(fightor->hero->getHp() > 0,"�쳣");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff�������buff����Ӻͺ����û�����⣬�Ƽ��Ǻ���ӣ���Ϊ�ǹ����˲��ܼ�buff��
		// ����ֻ�ܸ��Ӽ���
		// �������ĸ��ӵ�ʱ�򣬻��ж�Ѫֵ��
		// �������buff������һ����ӵ���ɫ����ȥ
		// ��Ϊ���ﲻ�жϵ�Ѫ���Ƿ����0��û���ã���Ϊ����û�и�����ȥ�������ں���żӵģ����������ӵ�ʱ��Ҳ������buff��Ӱ�죬
		// ʹ�������ɫ�Ѿ�����
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.gongjili_add_lv = 50 * skillResultPercent;
		effect.remove_pos = Pos_Self_Fight_End;

		GameBuff buff(BuffState_TianTangShiZhe,fightor->getUniqueId(),fightor->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2��Ч��

		//effect.effect_id = mProvider->genOneBuffEffectId();
		//buff.effect_vec.push_back(effect);	// 3��Ч��

		f.firedBuff.push_back(buff);
	}



	return f;
}


SkillFight SkillHelperBase::fight_Monster_XieE(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// ��������ȫ��
	// ������������̷�Χ�ڵĵз���λ��
	// �˺�ֵΪ�չ�1��
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		float hp_sub = fightor->hero->getShangHaiValue(allItems[i]->hero);
		int skillHp = hp_sub * skillResultPercent * 0.7;
		bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}


SkillFight SkillHelperBase::fight_BOSS_Sweep(ChessBoardItemBase* fightor, SkillDataForFight skill)
{
    SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	vector<ChessBoardItemBase*> targets = mProvider->getFrontRowAlivedEnemyItem(fightor);
	if (targets.empty()) {
		targets = mProvider->getBackRowAlivedEnemyItem(fightor);
		CCAssert(!targets.empty(),"");
	}
    
	for (int i=0,count=targets.size(); i<count; i++) {
		// ����
		int skillHp = fightor->hero->getShangHaiValue(targets[i]->hero) * 4.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)targets[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(targets[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}


SkillFight SkillHelperBase::fight_BOSS_Flog(ChessBoardItemBase* fightor, SkillDataForFight skill)
{
    SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// Ч������
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
    
	for (int i=0,count=allItems.size(); i<count; i++) {
		// ����
		int skillHp = fightor->hero->getShangHaiValue(allItems[i]->hero) * 4.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);
	}

	return f;
}


SkillHelperBase::~SkillHelperBase(void)
{
}
