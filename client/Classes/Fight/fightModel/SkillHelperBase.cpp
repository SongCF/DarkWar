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
	// 天赋技能
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

	// 技能书1
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

	// 技能书2
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

	// 技能书3
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
        
    // 怪物
	case SkillType_Monster_XieE:
		S_LOG("SkillType_Monster_XieE");
		data = fight_Monster_XieE(fightor,skill);
		break;
        
    // 怪物
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
		CCAssert(false,"没有实现");
		break;
	}

	// 目前统一设置为不显示暴击
	data.skillPatten = Skill_Noraml;
	// 检验：
	{
		CCAssert(data.canMissVec.size() == data.targetVec.size(),"个数应该一样");
		CCAssert(data.targetBloodChange.size() == data.targetVec.size(),"个数应该一样");
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
				CCAssert(found,"打出的buff，需要附加到的人必须在target中");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 就是普通攻击的方式
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 就是普通攻击的方式
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 就是普通攻击的方式
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	vector<ChessBoardItemBase*> targets = mProvider->getFrontRowAlivedEnemyItem(fightor);
	if (targets.empty())
	{
		targets = mProvider->getBackRowAlivedEnemyItem(fightor);
		CCAssert(!targets.empty(),"");
	}
	for (int i=0,count=targets.size(); i<count; i++)
	{
		// 攻击
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
	CCAssert(false,"暂时不用了，改用ShengCai");

	CCAssert(dynamic_cast<Hero*>(fightor->hero)!=NULL || dynamic_cast<TeamHero*>(fightor->hero)!=NULL,"只有Hero才可以用这个技能");

	SkillFight f;
	f.nameForLog = "JianShou";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	//进入坚守状态，提升自身防御40%，持续三轮。
	//坚守状态只可覆盖不可叠加。
	{
		CCAssert(fightor->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff，这里的buff先添加和后添加没有问题，推荐是后添加，因为是攻击了才能加buff嘛
		// 这里只管附加即可
		// 在真正的附加的时候，会判断血值的
		// 所以这个buff不见得一定会加到角色上面去
		// 因为这里不判断的血量是否大于0，没有用，因为这里没有附加上去，而是在后面才加的，而且真正加的时候也许其他buff的影响，
		// 使得这个角色已经挂了
		//GameBuffEffect effect(mProvider->genOneBuffEffectId());
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.jianshang_lv = 40 * skillResultPercent;
		//effect.remove_pos = Pos_Self_BeAttacked;
		//
		// 2014-03-03修改，所有的移除点都移动到了轮次替换的地方了
		// 
		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_JianShou,fightor->getUniqueId(),fightor->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3次效果

		f.firedBuff.push_back(buff);
	}
	

	return f;
}

SkillFight SkillHelperBase::fight_ShenZhiXingNian(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	//CCAssert(dynamic_cast<Hero*>(fightor->hero)!=NULL,"只有Hero才可以用这个技能");
	CCAssert(dynamic_cast<Hero*>(fightor->hero)!=NULL || dynamic_cast<TeamHero*>(fightor->hero)!=NULL,"只有Hero才可以用这个技能");


	SkillFight f;
	f.nameForLog = "ShenZhiXingNian";
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	PropertyCalculator calculate;
	// 提升防御20%，持续3轮。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedTeamItems(fightor);;
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff，这里的buff先添加和后添加没有问题，推荐是后添加，因为是攻击了才能加buff嘛
		// 这里只管附加即可
		// 在真正的附加的时候，会判断血值的
		// 所以这个buff不见得一定会加到角色上面去
		// 因为这里不判断的血量是否大于0，没有用，因为这里没有附加上去，而是在后面才加的，而且真正加的时候也许其他buff的影响，
		// 使得这个角色已经挂了
		int itemHuJiaZhi = calculate.calculateHuJiaZhi(((Hero*)allItems[i]->hero)->getCalculateData());
		
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.jianshang_lv = (20.0f * skillResultPercent);
		//effect.remove_pos = Pos_Self_BeAttacked;
		//
		// 2014-03-03修改，所有的移除点都移动到了轮次替换的地方了
		// 
		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_ShenZhiPiHu,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3次效果

		// 添加
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	PropertyCalculator calculate;
	// 2014-03-25新增，用于替换fight_ShenZhiXingNian
	// 使本方所有单位进入神之信念状态，提高目标相当于自身攻击20%的攻击力，持续2回合。（状态不可叠加）

	float fightor_gongjili = fightor->hero->getGongJiLi(Range_Average);

	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedTeamItems(fightor);;
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		float self_gongjili = allItems[i]->hero->getGongJiLi(Range_Average);
		float addPer = fightor_gongjili*0.2f / self_gongjili; // 计算20%攻击，占用我的多少
		addPer *= 100;

		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		float lv = addPer * skillResultPercent;
		// 至少在1以上，等于0的不管
		if (lv < 0 && lv > -1)
			lv = -1;
		if (lv > 0 && lv < 1) 
			lv = 1;
		effect.gongjili_add_lv = lv;

		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_ShenZhiXinNian,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		// 添加
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 就是普通攻击的方式
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击距离无限制。
	// 攻击范围单体，优先攻击防御弱者和濒死者。
	// 伤害值为普攻2倍。
	ChessBoardItemBase* target = NULL;
	{
		// 看周围所有的
		vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
		target = mProvider->getWeaklessAndDyingItem(allItems,fightor);
	}
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击距离2.5
	// 攻击所有在射程范围内的敌方单位。
	// 伤害值为普攻2倍。
	// 
	// --> 修正
	// 剑雨改为：距离2.5以内的伤害值为普攻1.5倍，距离大于2.5的伤害值为普攻1倍。
	//
	// --> 修正
	// --> 技能所有敌方受到的伤害全部改为1倍普攻，与距离无关。
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

		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击随机两个目标，造成150%的伤害，并有20%几率击晕目标(击晕状态,不可叠加)，使其停止行动，持续2回合

	// 随机选2个不重复的，不够就用一个
	vector<ChessBoardItemBase*> targets;
	{
		vector<ChessBoardItemBase*> items = mProvider->getAlivedEnemyItems(fightor);
		CCAssert(!items.empty(),"");
		int index = (items.size()-1)*lfRandom0_1();
		targets.push_back(items[index]);

		if (items.size() >= 2)
		{
			// 去掉选过的
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

			// 随机选第二个
			int index = (items.size()-1)*lfRandom0_1();
			targets.push_back(items[index]);
		}
	}
	
	for (int i=0,count=targets.size(); i<count; i++)
	{
		CCAssert(targets[i]->hero->getHp() > 0,"异常");
		int skillHp = fightor->hero->getShangHaiValue(targets[i]->hero) * 1.5f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)targets[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(targets[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);

        
        /* 2014.06.03 修改不受此技能的眩晕效果*/  
        if((targets[i]->hero->getHeroType()== Type_Guaiwu_jingying)
            && (targets[i]->hero->getXSize() > 1)) {  
            continue;
        }
        
		if (lfRandom0_1() < 0.2f)
		{
			// buff效果
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.xuanyun = true;
			effect.remove_pos = Pos_ChangeRound; // 所有的buff目前都放在这个地方移除

			GameBuff buff(BuffState_JiYun,fightor->getUniqueId(),targets[i]->getUniqueId(),false); 
			buff.effect_vec.push_back(effect);

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 2次效果

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击距离2.
	// 攻击范围单体，优先攻击正前方单位。
	// 伤害值为普攻的2倍。
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	float fightor_gongjili = fightor->hero->getGongJiLi(Range_Average);

	// 2014-03-25日添加，用于替换fight_WenYi
	//使敌方所有单位进入瘟疫状态，降低目标相当于巫师自身攻击20%的攻击力，持续2会回合。（状态不可叠加）
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		float self_gongjili = allItems[i]->hero->getGongJiLi(Range_Average);
		float addPer = fightor_gongjili* (-0.2f) / self_gongjili; // 计算20%攻击，占用我的多少
		addPer *= 100;

		// buff，这里的buff先添加和后添加没有问题，推荐是后添加，因为是攻击了才能加buff嘛
		// 这里只管附加即可
		// 在真正的附加的时候，会判断血值的
		// 所以这个buff不见得一定会加到角色上面去
		// 因为这里不判断的血量是否大于0，没有用，因为这里没有附加上去，而是在后面才加的，而且真正加的时候也许其他buff的影响，
		// 使得这个角色已经挂了
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		float lv = addPer * skillResultPercent;
		// 至少在1以上，等于0的不管
		if (lv < 0 && lv > -1)
			lv = -1;
		if (lv > 0 && lv < 1) 
			lv = 1;
		effect.gongjili_add_lv = lv;

		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_WenYi,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		// 添加
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 对方所有单位进入瘟疫状态。降低防御20%，持续3轮。
	// 瘟疫状态只可覆盖不可叠加。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff，这里的buff先添加和后添加没有问题，推荐是后添加，因为是攻击了才能加buff嘛
		// 这里只管附加即可
		// 在真正的附加的时候，会判断血值的
		// 所以这个buff不见得一定会加到角色上面去
		// 因为这里不判断的血量是否大于0，没有用，因为这里没有附加上去，而是在后面才加的，而且真正加的时候也许其他buff的影响，
		// 使得这个角色已经挂了
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.jianshang_lv = -20.0f * skillResultPercent;
		//effect.remove_pos = Pos_Self_BeAttacked;
		//
		// 2014-03-03修改，所有的移除点都移动到了轮次替换的地方了
		// 
		effect.remove_pos = Pos_ChangeRound;

		GameBuff buff(BuffState_WenYi,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3次效果

		// 添加
		f.firedBuff.push_back(buff);
	}

	return f;
}



// 技能书1
SkillFight SkillHelperBase::fight_BanYueZhan(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击所有与自己距离为1和1.5的敌方单位。
	// 距离为1的所有单位均摊2倍普攻伤害，
	// 距离为1.5的所有单位均摊1倍普攻伤害。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 1.5f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击正前方距离1和2的所有敌方单位。正前方！！
	// 距离为1的单位承受普攻2倍伤害，
	// 距离为2的单位承受普攻1倍伤害。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();

	bool chooseCol = false;	// 是否选择一列，如果一列都没有再尝试选择一排
	for (int i=0;i<num;i++)
	{
		if (!mProvider->isInCol(fightor,allItems[i]))
			continue;
		chooseCol = true; // 有人在一列上，就选择一列
	}

	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		// 需要攻击的是一列，那么不是正前方的，不管
		if (chooseCol && !mProvider->isInCol(fightor,allItems[i]))
			continue;
		// 需要攻击的是以排，那么不是一排的不管
		if (!chooseCol && !mProvider->isInRow(fightor,allItems[i]))
			continue;

		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 2.0f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 攻击距离2.5以内的所有敌方单位。
	// 距离2及以内单位均摊2倍普攻伤害，
	// 距离2.5的单位均摊1倍普攻伤害。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 2.5f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 进入芒刺状态，提升自身防御30%，并可反弹所有物理攻击伤害值的20%。持续3轮。
	// 芒刺状态只可覆盖不可叠加。
	{
		CCAssert(fightor->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff，这里的buff先添加和后添加没有问题，推荐是后添加，因为是攻击了才能加buff嘛
		// 这里只管附加即可
		// 在真正的附加的时候，会判断血值的
		// 所以这个buff不见得一定会加到角色上面去
		// 因为这里不判断的血量是否大于0，没有用，因为这里没有附加上去，而是在后面才加的，而且真正加的时候也许其他buff的影响，
		// 使得这个角色已经挂了
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.fantan_lv = 20*skillResultPercent;
		effect.jianshang_lv = 30*skillResultPercent;
		effect.remove_pos = Pos_Self_BeAttacked;

		GameBuff buff(BuffState_MangCi,fightor->getUniqueId(),fightor->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3次效果

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 本方全体进入神之祝福状态，处于神之祝福状态的角色每轮可恢复自身7%血量，持续3轮。
	// 神之祝福状态只可覆盖不可叠加。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedTeamItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
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
		buff.effect_vec.push_back(effect);	// 2次效果

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3次效果

		// 添加
		f.firedBuff.push_back(buff);
	}






	return f;
}
// 备份，以前的打横排，打纵排
// 要改成打纵排，打斜排
//SkillFight SkillHelperBase::fight_LiRenHuiXuan(ChessBoardItemBase* fightor,SkillDataForFight skill)
//{
//	SkillFight f;
//	f.nameForLog = string(__FUNCTION__);
//	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
//	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
//	// 效果因子
//	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;
//
//	// 攻击正前方距离1和2的所有敌方单位。
//	// 距离为1的单位承受普攻2倍伤害，
//	// 距离为2的单位承受普攻1倍伤害。
//	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
//	int num = allItems.size();
//
//
//	bool chooseCol = false;	// 是否选择一列，如果一列都没有再尝试选择一排
//	for (int i=0;i<num;i++)
//	{
//		if (!mProvider->isInCol(fightor,allItems[i]))
//			continue;
//		chooseCol = true; // 有人在一列上，就选择一列
//	}
//
//
//	float distance_temp = 0;
//	for (int i=0;i<num;i++)
//	{
//		// 需要攻击的是一列，那么不是正前方的，不管
//		if (chooseCol && !mProvider->isInCol(fightor,allItems[i]))
//			continue;
//		// 需要攻击的是以排，那么不是一排的不管
//		if (!chooseCol && !mProvider->isInRow(fightor,allItems[i]))
//			continue;
//
//		distance_temp = mProvider->distance(fightor,allItems[i]);
//		if (distance_temp > 2.0f)
//			continue;
//		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
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
	CCAssert(dynamic_cast<ChessBoardItem*>(fightor)!=NULL,"目前为了方便，只有4X4棋盘才可以用这个技能");

	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击正前方距离1和2的所有敌方单位。
	// 距离为1的单位承受普攻2倍伤害，
	// 距离为2的单位承受普攻1倍伤害。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();


	bool chooseCol = false;	// 是否选择一列，如果一列都没有再尝试选择一排
	for (int i=0;i<num;i++)
	{
		if (!mProvider->isInCol(fightor,allItems[i]))
			continue;
		chooseCol = true; // 有人在一列上，就选择一列
	}
	
	for (int i=0;i<num;i++)
	{
		// 如果是打一列，那么就判断是否是一列
		// 如果不是打一列，那么其实要攻击的对象在另一列，于是可以简单点，都是被攻击对象
		//
		// 需要攻击的是一列，那么不是正前方的，不管
		if (chooseCol && !mProvider->isInCol(fightor,allItems[i]))
			continue;
		
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");

		float effectForRow = 1.0f;
		// 前排
		if ( ((ChessBoardItem*)allItems[i])->posy == 0 )
		{
			effectForRow = 1.5f;
		}
		else
		{// 后排
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 全屏的范围
	// 攻击范围单体，优先攻击防御弱者和濒死者。
	// 伤害值为普攻3倍。并有50%几率使对方致残，停止行动一轮。
	ChessBoardItemBase* target = NULL;
	{
		// 看周围所有的
		vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
		target = mProvider->getWeaklessAndDyingItem(allItems,fightor);
	}
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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

			// 添加
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 攻击距离2.5
	// 攻击所有在射程范围内的敌方单位。
	// 伤害值为普攻2倍，并是对方进入着火状态，（持续掉血两轮，伤害相当于普攻1倍）。
	// 着火状态只可覆盖不可叠加
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		if (mProvider->distance(fightor,allItems[i]) > 2.5f)
			continue;

		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
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


		// 添加
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 攻击距离2.5以内的所有敌方单位。
	// 距离2及以内单位均摊2倍普攻伤害，
	// 距离2.5的单位均摊1倍普攻伤害。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 2.5f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 对方所有单位70%几率进入恐惧状态。
	// 降低攻击30%。持续3轮。
	// 恐惧状态只可覆盖不可叠加。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		// 70%的几率进入恐惧
		if (lfRandom0_1() > 0.7f)
			continue;

		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);


		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.gongjili_add_lv = -30 * skillResultPercent;
		effect.remove_pos = Pos_Self_Fight_End;

		GameBuff buff(BuffState_KongJu,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3次效果

		// 添加
		f.firedBuff.push_back(buff);
	}



	return f;
}

// 技能书2
SkillFight SkillHelperBase::fight_KuangBao(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击距离1。
	// 攻击范围单体，优先攻击正前方单位。
	// 伤害值为普攻的2倍，有50%几率将对方击晕一轮。
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
		int skillHp = fightor->hero->getShangHaiValue(target->hero) * 2.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)target->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(target->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);

		// 因为是单体攻击，所以可以只管添加与否就是了
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 攻击距离1.
	// 攻击范围单体，优先攻击正前方单位。
	// 伤害值为普攻1.5倍，使对方进入流血状态（持续两轮持续掉血，伤害相当于普攻的1倍）。
	// 流血状态只能覆盖不可叠加。
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,1.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
		buff.effect_vec.push_back(effect);	// 2次效果

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 攻击距离2。
	// 攻击攻击范围单体，优先攻击正前方单位。
	// 伤害值为普攻3倍，有50%几率将对方击晕一轮。
	// 就是普通攻击的方式
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	//进入守护者状态，对一切攻击免疫一轮。并且提升自身攻击力50%，
	{
		CCAssert(fightor->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// 攻击免疫
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.jianshang_lv = 100; // 这个就不能提升了
			effect.remove_pos = Pos_Self_BeAttacked;

			GameBuff buff(BuffState_ShouHuZhe1,fightor->getUniqueId(),fightor->getUniqueId()); 
			buff.effect_vec.push_back(effect);

			f.firedBuff.push_back(buff);
		}

		// 加攻击
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 本方全体进入众神之力状态，提升攻击20%，持续3轮。
	// 众神之力只可覆盖不可叠加。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedTeamItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff，这里的buff先添加和后添加没有问题，推荐是后添加，因为是攻击了才能加buff嘛
		// 这里只管附加即可
		// 在真正的附加的时候，会判断血值的
		// 所以这个buff不见得一定会加到角色上面去
		// 因为这里不判断的血量是否大于0，没有用，因为这里没有附加上去，而是在后面才加的，而且真正加的时候也许其他buff的影响，
		// 使得这个角色已经挂了
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.gongjili_add_lv = 20 * skillResultPercent;
		effect.remove_pos = Pos_Self_BeAttacked;

		GameBuff buff(BuffState_ZhongShenZhiLi,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3次效果

		// 添加
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击所有与自己距离为1和1.5的敌方单位。
	// 距离为1的所有单位均摊2倍普攻伤害，距离为1.5的所有单位均摊1倍普攻伤害。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	float distance_temp = 0;
	for (int i=0;i<num;i++)
	{
		distance_temp = mProvider->distance(fightor,allItems[i]);
		if (distance_temp > 1.5f)
			continue;
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击距离无限制。
	// 攻击范围单体，优先攻击防御弱者和濒死者。
	// 伤害值为普攻4倍，使对方进入中毒状态（持续两轮掉血，伤害相当于普攻的1倍）。中毒状态只可覆盖不可叠加。
	ChessBoardItemBase* target = NULL;
	{
		// 看周围所有的
		vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
		target = mProvider->getWeaklessAndDyingItem(allItems,fightor);
	}
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
		buff.effect_vec.push_back(effect); // 第2次


		// 添加
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 攻击距离2.5
	// 攻击所有在射程范围内的敌方单位。
	// 伤害值为普攻3.0倍，并使对方进入恐惧状态。降低攻击30%。持续3轮。
	// 恐惧状态只可覆盖不可叠加。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		if (mProvider->distance(fightor,allItems[i]) > 2.5f)
			continue;

		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
		float hp_sub = fightor->hero->getShangHaiValue(allItems[i]->hero);
		int skillHp = hp_sub * 3.0f * skillResultPercent;
		bool canMiss = lfRandom0_1() <= ((float)allItems[i]->hero->getShanBiLv())/100.0f;
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(-skillHp);
		f.canMissVec.push_back(canMiss);


		//// 并使对方35%几率进入恐惧状态
		//if (lfRandom0_1()<0.35f)
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.gongjili_add_lv = -30 * skillResultPercent;
			effect.remove_pos = Pos_Self_Fight_End;

			GameBuff buff(BuffState_KongJu,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
			buff.effect_vec.push_back(effect);

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 2次效果

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 3次效果

			// 添加
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 攻击距离2。
	// 攻击攻击范围单体，优先攻击正前方单位。
	// 伤害值为普攻2倍，有50%几率将对方冰冻一轮。
	ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
	if (target!=NULL)
	{
		CCAssert(target->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;
	CCLog("Waring : %s ---> skill effect add not use!",__FUNCTION__);

	//对方所有单位70%几率进入恐惧状态。降低攻击30%。持续3轮。
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		// 70%的几率进入恐惧
		if (lfRandom0_1() > 0.5f)
			continue;

		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(allItems[i]->getUniqueId());
		f.targetBloodChange.push_back(0);


		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.xuanyun = true;
		effect.remove_pos = Pos_Self_Fight_End;

		GameBuff buff(BuffState_ShiHun,fightor->getUniqueId(),allItems[i]->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		// 添加
		f.firedBuff.push_back(buff);
	}



	return f;
}

// 技能书3
SkillFight SkillHelperBase::fight_BaoNu(ChessBoardItemBase* fightor,SkillDataForFight skill)
{
	SkillFight f;
	f.nameForLog = string(__FUNCTION__);
	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	f.skillPatten = lfRandom0_1() <= baoJilv?Skill_Noraml_BaoJI:Skill_Noraml;
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 进入暴怒状态，提高自身攻击力30%，提升暴击率30%，持续3轮。
	// 触发此技能后本轮立即发动攻击（普攻或暴击）。
	// 暴怒状态只可覆盖不可叠加。
	{
		CCAssert(fightor->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff，这里的buff先添加和后添加没有问题，推荐是后添加，因为是攻击了才能加buff嘛
		// 这里只管附加即可
		// 在真正的附加的时候，会判断血值的
		// 所以这个buff不见得一定会加到角色上面去
		// 因为这里不判断的血量是否大于0，没有用，因为这里没有附加上去，而是在后面才加的，而且真正加的时候也许其他buff的影响，
		// 使得这个角色已经挂了
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.gongjili_add_lv = 30 * skillResultPercent;
		effect.baoji_add = 30 * skillResultPercent;
		effect.remove_pos = Pos_Self_Fight_End;

		GameBuff buff(BuffState_BaoNu,fightor->getUniqueId(),fightor->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 3次效果

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 因为暴击率和闪避率的移除点不一样
	// 
	// 进入隐者状态，提升自身暴击率30%，提升闪避率50%，持续3轮。触发此技能后本轮立即发动攻击（普攻或暴击）。
	// 隐者状态只可覆盖不可叠加。
	{
		CCAssert(fightor->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// 加暴击
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.baoji_add = 30 * skillResultPercent;
			effect.remove_pos = Pos_Self_Fight_End;

			GameBuff buff(BuffState_BaoNu,fightor->getUniqueId(),fightor->getUniqueId()); 
			buff.effect_vec.push_back(effect);

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 2次效果

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 3次效果

			f.firedBuff.push_back(buff);
		}

		// 加闪避
		{
			GameBuffEffect effect(mProvider->genOneBuffEffectId());
			effect.shanbi_add = 30 * skillResultPercent;
			effect.remove_pos = Pos_Self_BeAttacked;

			GameBuff buff(BuffState_BaoNu,fightor->getUniqueId(),fightor->getUniqueId()); 
			buff.effect_vec.push_back(effect);

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 2次效果

			effect.effect_id = mProvider->genOneBuffEffectId();
			buff.effect_vec.push_back(effect);	// 3次效果

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	// 进入天堂使者状态，
	// 
	// 提升自身攻击力50%持续3轮。 （那也就是有一次就拿到本次用了，那么后面还剩2次）
	// 然后发动攻击，攻击距离为2，伤害值为普攻3倍。
	// 天堂使者状态只可覆盖不可叠加。
	// 攻击攻击范围单体，优先攻击正前方单位
	{
		ChessBoardItemBase* target = mProvider->getDefaultTargetBeFight(fightor,2.0f);
		if (target!=NULL)
		{ 
			CCAssert(target->hero->getHp() > 0,"异常");

			// 内部的提升一次buff,然后马上移除,用于本次的buff效果
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

			// 移除
			fightor->hero->removeBuff(effect.effect_id);
		}

		// 自家加buff
		CCAssert(fightor->hero->getHp() > 0,"异常");
		f.canMissVec.push_back(false);
		f.targetVec.push_back(fightor->getUniqueId());
		f.targetBloodChange.push_back(0);

		// buff，这里的buff先添加和后添加没有问题，推荐是后添加，因为是攻击了才能加buff嘛
		// 这里只管附加即可
		// 在真正的附加的时候，会判断血值的
		// 所以这个buff不见得一定会加到角色上面去
		// 因为这里不判断的血量是否大于0，没有用，因为这里没有附加上去，而是在后面才加的，而且真正加的时候也许其他buff的影响，
		// 使得这个角色已经挂了
		GameBuffEffect effect(mProvider->genOneBuffEffectId());
		effect.gongjili_add_lv = 50 * skillResultPercent;
		effect.remove_pos = Pos_Self_Fight_End;

		GameBuff buff(BuffState_TianTangShiZhe,fightor->getUniqueId(),fightor->getUniqueId()); 
		buff.effect_vec.push_back(effect);

		effect.effect_id = mProvider->genOneBuffEffectId();
		buff.effect_vec.push_back(effect);	// 2次效果

		//effect.effect_id = mProvider->genOneBuffEffectId();
		//buff.effect_vec.push_back(effect);	// 3次效果

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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;


	// 攻击距离全体
	// 攻击所有在射程范围内的敌方单位。
	// 伤害值为普攻1倍
	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"异常");
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	vector<ChessBoardItemBase*> targets = mProvider->getFrontRowAlivedEnemyItem(fightor);
	if (targets.empty()) {
		targets = mProvider->getBackRowAlivedEnemyItem(fightor);
		CCAssert(!targets.empty(),"");
	}
    
	for (int i=0,count=targets.size(); i<count; i++) {
		// 攻击
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
	// 效果因子
	float skillResultPercent = ((float)fightor->hero->getJiNengResult())/100.0f;

	vector<ChessBoardItemBase*> allItems = mProvider->getAlivedEnemyItems(fightor);
    
	for (int i=0,count=allItems.size(); i<count; i++) {
		// 攻击
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
