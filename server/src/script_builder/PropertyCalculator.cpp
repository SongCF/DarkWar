#include "PropertyCalculator.h"
#include "CalculateData.h"
#include "CS_Common.h"
#include "ScriptDataProvider.h"
#include "SuitPool.h"
#include "ZhuangbeiGongMingManager.h"
#include <cmath>
#ifdef _CLIENT_
#include "Debug.h"
#endif // _CLIENT_


extern int randomWithin(int start,int end);
extern float randomWithin_f(float start,float end);
extern float lfRandom0_1();

PropertyCalculator::PropertyCalculator(void)
{
	euipCal = new EquipCalculator();
	buffCal = new BuffCalculator();
}
// 拷贝构造会调用,比如vector的push
// 在对象复制初始化时，调用的将是拷贝构造函数，而不是缺省构造函数
PropertyCalculator::PropertyCalculator(const PropertyCalculator& cal)
{
	// 重新new
	euipCal = new EquipCalculator();
	buffCal = new BuffCalculator();
}
// 赋值语句会调用
PropertyCalculator& PropertyCalculator::operator = (const PropertyCalculator& rightData)
{
	if(this == &rightData) // 如果自己给自己赋值则直接返回
	{
		return *this;
	}

	// 重新new
	euipCal = new EquipCalculator();
	buffCal = new BuffCalculator();
	return *this;
}

float PropertyCalculator::calculateSkillShangHai(float actorPutongGongJiLi,const SkillDataForFight& skill)const
{
	//技能伤害=当前技能的效果倍数*角色当前攻击力*升级效果
	float ret = actorPutongGongJiLi;

	float percent = ((float)CS::getSkillEffectRatioOfNormalAttack(skill.skillType))/100.0f;
	percent += ((float)CS::getSkillEffectRatioAddBySkillLevel(skill.skillLevel))/100.0f;

	ret *= percent;

	return ret;
}

// 计算攻击力：就是当前角色的攻击力，就是一般攻击的能力,得到的就是数值
float PropertyCalculator::calculateGongJiLi(const CalculateData& attack,ValueRange range /* = Range_Ramdom */) const
{
	// include额外加的
	float gongji_include_add = 0.0f;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		gongji_include_add += calculateGongJiLi(attack.includeCalDatas[i],range);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = attack;
	flat_data.includeCalDatas.clear();


	float gongjili = 0;
	{
		// 攻击力=装备攻击力*（0.02*主属性+1）。 （注：赤手空拳=攻击力1）
		float gongji_min = 0;
		float gongji_max = 0;
		float gongji_min_add = 0;
		float gongji_max_add = 0;
		// 装备攻击力
		euipCal->getEquipGongJiLi(flat_data.allEquips,gongji_min,gongji_max,gongji_min_add,gongji_max_add);
		gongji_min += gongji_min_add;
		gongji_max += gongji_max_add;


		float mainProperty = flat_data.mainPro;

		float gongjiMin = ((float)gongji_min) * (0.02f*mainProperty + 1.0f);
		float gongjiMax = ((float)gongji_max) * (0.02f*mainProperty + 1.0f);

		// 攻击加成
		float per_add = euipCal->getEquipGongJiAdd_ratio(flat_data.allEquips);
		per_add /= 100.0f;
		gongjiMin += gongjiMin * per_add;
		gongjiMax += gongjiMax * per_add;

		// 职业的攻击加成
		{
			if (flat_data.base_pro.profession < Profession_GW_Start)
			{
				int bloodAdd = 0;
				int gongjiAdd = 0;
				CS::getProfesionBloodAndGongJiAddPercent(flat_data.base_pro.profession,bloodAdd,gongjiAdd);
				float gongjiPer = ((float)gongjiAdd)/100.0f;
				gongjiMin += gongjiMin * gongjiPer;
				gongjiMax += gongjiMax * gongjiPer;
			}
		}

		// 真身角色的阶的血量加成（元神的阶不能加成，只用于等级的升级的砍）
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float gongjiPer = ((float)CS::getHeroStageEffectRatio(flat_data.base_pro.stage))/100.0f;
				gongjiMin += gongjiMin * gongjiPer;
				gongjiMax += gongjiMax * gongjiPer;
			}
		}

		// 真身的强化共鸣加成
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float addPer = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(flat_data.allEquips,GM_GongJiAddPer);
				addPer = addPer/100.0f;
				gongjiMin += gongjiMin * addPer;
				gongjiMax += gongjiMax * addPer;
			}
		}

		// 
		{

		}


		switch (range)
		{
		case Range_Ramdom:
			gongjili = max(randomWithin_f(gongjiMin,gongjiMax),1.0f);// 最小为1
			break;
		case Range_Max:
			gongjili = max(max(gongjiMax,gongjiMin),1.0f);// 最小为1
			break;
		case Range_Average:
			gongjili = max((gongjiMin + gongjiMax)/2.0f,1.0f);// 最小为1
			break;
		case Range_Min:
			gongjili = max(min(gongjiMax,gongjiMin),1.0f);// 最小为1
			break;
		default:
			CCAssert(false,"不支持的值域计算");
			break;
		}

		float gongjili_add_lv = buffCal->getGongJiLiAddLv(flat_data.allBuffs);
		gongjili_add_lv /= 100.0f;

		gongjili += gongjili*gongjili_add_lv;

		float percent = flat_data.base_pro.effect_convert_per / 100.0f;
		gongjili *= percent; // 转换率
	}

	gongjili += gongji_include_add;

	return gongjili;
}

float PropertyCalculator::calculateGongJiLi_MacroValue(const CalculateData& attack,bool skillOK, const SkillDataForFight& skill, ValueRange range) const
{
	//角色宏观攻击力=角色当前攻击力*(1-暴击率)+（暴击几率*暴击伤害因子*攻击力）+（1/技能触发周期*技能伤害）

	// include额外加的
	float gongji_include_add = 0.0f;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		// 额外的都不算skill
		gongji_include_add += calculateGongJiLi_MacroValue(attack.includeCalDatas[i],false,skill,range);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = attack;
	flat_data.includeCalDatas.clear();


	float ret = 0.0f;
	{
		// 已经是有转换率的了
		// 所以后面就不需要再处理转换率了
		const float base_gongji = calculateGongJiLi(flat_data,range);
		const float bao_shang = ((float)calculateBaoJiShangHaiYinzi(flat_data))/100.0f;
		const float baoji_lv = ((float)calculateBaoJiLv(flat_data))/100.0f;

		ret = base_gongji*(1-baoji_lv) + base_gongji*bao_shang*baoji_lv;

		// 技能
		if (skillOK)
		{
			float skillZhouQi = calculateJiNengZhouQi(flat_data/*,skill.skillType*/);
			float jinengShangHai = calculateSkillShangHai(base_gongji,skill);
			ret += 1.0f/skillZhouQi*jinengShangHai;
		}

		// 转换率 ： 上面的已经算了转换率了。这里就不用转换率了
		//float percent = flat_data.base_pro.effect_convert_per /100.0f;
		float percent = 1;
		ret *= percent;
	}


	// 额外的include的数据
	ret += gongji_include_add;

	return ret;
}

float PropertyCalculator::calculateGongJiLi_MacroValue(const CalculateData& attack,ValueRange range /* = Range_Ramdom */) const
{
	SkillDataForFight _skill(SkillType_BaoLieZhan);
	bool use_skill = false;
	if (attack.base_pro.skill_type > 0)
	{
		_skill.skillType = (SkillType)attack.base_pro.skill_type;
		_skill.skillLevel = attack.base_pro.skill_level;
		use_skill = true;
	}
	return calculateGongJiLi_MacroValue(attack,use_skill,_skill,range);
}

//float PropertyCalculator::calculateGongJiLi_MacroValue(const CalculateData& attack,const SkillDataForFight& skill,ValueRange range /*= Range_Ramdom*/) const
//{
//	return calculateGongJiLi_MacroValue(attack,true,skill,range);
//}

float PropertyCalculator::calculateZhanDouLi(const CalculateData& attack,bool skillOK, const SkillDataForFight& skill,ValueRange range) const
{
	// include额外加的
	float include_add = 0.0f;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateZhanDouLi(attack.includeCalDatas[i],false,skill,range);
	}

	// 拷贝一份没有include的数据
	CalculateData flat_data = attack;
	flat_data.includeCalDatas.clear();

	//角色战斗力=角色承受力+4*角色宏观攻击力
	float ret = 0;
	{
		float chengshouli = calculateChengShouLi(flat_data);
		float gongjili_macro = calculateGongJiLi_MacroValue(flat_data,skillOK,skill,range);
		ret = chengshouli+gongjili_macro*4;

		// 转换率,上面的（calculateChengShouLi，calculateGongJiLi_MacroValue）已经是算过转换率的了
		// 所有不用再算了
		//float percent = flat_data.base_pro.effect_convert_per/100.0f;
		float percent = 1;
		ret *= percent;

	}


	// include新增
	ret += include_add;
	return ret;

}

float PropertyCalculator::calculateZhanDouLi_2(const CalculateData& attack,ValueRange range /* = Range_Ramdom */) const
{
	if (attack.base_pro.skill_type > 0)
	{
		SkillDataForFight skill((SkillType)attack.base_pro.skill_type,attack.base_pro.skill_level);
		return calculateZhanDouLi(attack,true,skill,range);
	}

	SkillDataForFight skill_null(SkillType_BaoLieZhan);
	return calculateZhanDouLi(attack,false,skill_null,range);
}


/*2014.07.03 新的计算战斗力的方法*/
float PropertyCalculator::calculateZhanDouLi(const CalculateData& attack,ValueRange range /* = Range_Ramdom */) const
{
    float hp_cap = calculateHp(attack);     //血量
    float attack_cap = calculateGongJiLi(attack, range);     //攻击力
    float defend_cap = calculateHuJiaZhi(attack);     //护甲
    float crit_damage_cap = calculateBaoJiShangHaiYinzi(attack); //暴伤
    float crit_ratio = calculateBaoJiLv(attack); //暴击率
    float skill_damage_cap = 0;

    
	if (attack.base_pro.skill_type > 0) {
		SkillDataForFight skill((SkillType)attack.base_pro.skill_type,attack.base_pro.skill_level);
        skill_damage_cap = calculateSkillShangHai(attack_cap, skill);
	}

    float capacity = 0;
    capacity = hp_cap *0.25f + attack_cap *1.0f + defend_cap *0.5f 
        + crit_damage_cap *10.0f + crit_ratio *10.0f + skill_damage_cap *1.0f;
    
	return capacity;
}


float PropertyCalculator::calculateChengShouLi(const CalculateData& attack)const
{
	// include额外加的
	float include_add = 0.0f;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateChengShouLi(attack.includeCalDatas[i]);
	}

	// 拷贝一份没有include的数据
	CalculateData flat_data = attack;
	flat_data.includeCalDatas.clear();

	//角色承受力=角色当前血量/(1-角色当前护甲减免率(用角色自己的等级折算))
	float ret = 0;
	{
		float currentHp = calculateHp(flat_data);
		float hujiaJianMianLv = calculateHuJiaJianMianLv(flat_data.base_pro.level,flat_data);
		hujiaJianMianLv /= 100.0f;
		ret = currentHp/(1-hujiaJianMianLv);

		// 转换率
		//float percent = flat_data.base_pro.effect_convert_per/100.0f;
		float percent = 1;
		ret *= percent;
	}


	// include新增
	ret += include_add;
	return ret;
}

float PropertyCalculator::calculateHuJiaZhi_MacroValue(const CalculateData& actor) const
{
	// include额外加的
	float hujia_include_add = 0.0f;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		hujia_include_add += calculateHuJiaZhi_MacroValue(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	float ret = 0;
	{
		ret = calculateHuJiaZhi(flat_data);
		//// 转换率
		//float percent = flat_data.base_pro.effect_convert_per/100.0f;
		//ret *= percent;
	}

	// include新增
	ret += hujia_include_add;

	return ret;
}
float PropertyCalculator::calculateKuangXingZhi_MacroValue(const CalculateData& beAttacked) const
{
	// include额外加的
	float kangxing_include_add = 0.0f;
	for (int i=0, include_count = beAttacked.includeCalDatas.size(); i<include_count; i++)
	{
		kangxing_include_add += calculateKuangXingZhi_MacroValue(beAttacked.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = beAttacked;
	flat_data.includeCalDatas.clear();

	float ret = 0.0f;
	{
		ret = calculateKuangXingZhi(flat_data);
		//float percent = flat_data.base_pro.effect_convert_per/100.0f;
		//ret *= percent; // 转换因子
	}

	ret += kangxing_include_add;

	return ret;
}
// 计算actor的护甲值
float PropertyCalculator::calculateHuJiaZhi(const CalculateData& actor) const
{
	// include额外加的
	float hujia_include_add = 0.0f;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		hujia_include_add += calculateHuJiaZhi(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	float ret = 0.0f;
	{
		ret = calculateHuJiaZhi(flat_data.base_pro.liliang,flat_data.allEquips,flat_data.allBuffs);

		// 真身角色的阶的血量加成（元神的阶不能加成，只用于等级的升级的砍）
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float hujiaPer = ((float)CS::getHeroStageEffectRatio(flat_data.base_pro.stage))/100.0f;
				ret += ret * hujiaPer;
			}
		}

		// 真身的强化共鸣
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				// 强化共鸣，是在算完了额外的之后再算这里，所有没问题
				float addPer = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(flat_data.allEquips,GM_HuJiaAddPer);
				addPer = addPer/100.0f;
				ret += ret * addPer;
			}
		}

		ret *= (flat_data.base_pro.effect_convert_per/100.0f); // 转换因子
	}


	ret += hujia_include_add;

	return ret;
}
float PropertyCalculator::calculateHuJiaZhi(unsigned int baseHeroLiliang,vector<EquipData> allEquips,list<GameBuff> allBuffs) const
{
	//2014-02-24去掉护甲到力量的转换
	//// 1力量=0.3护甲
	//float huJiaZhi = baseHeroLiliang*0.3f;

	float huJiaZhi = 0;

	huJiaZhi += euipCal->getEquipHuJiaZhi(allEquips);
	float buff_hujia_add = buffCal->getBuffHuJiaZhi(allBuffs);

	float ret = buff_hujia_add + huJiaZhi;

	return ret>1?ret:1; // 护甲值最小为1
}

unsigned int PropertyCalculator::calculateHuJiaJianMianLv(int fightorLevel,const CalculateData& beAttacked) const
{
	
	// 这里需要的是护甲值，减免率就不用再包一层了，
	// 如果还是要包一层，那么多个减免率怎么合呢？？还是直接加就是了？？
	// 直接就不包一层算了

	// 转换率也不考虑了，因为就跟护甲值有关，护甲值已经转换率了，那么这里就不转了



	// 护甲减免=护甲值/（护甲值+5*对手等级）。 （注：赤膊无铠甲=护甲值1）
	return calculateHuJiaJianMianLv(fightorLevel,calculateHuJiaZhi(beAttacked));
}
unsigned int PropertyCalculator::calculateHuJiaJianMianLv(int fightorLevel,float hujiazhi) const
{
	// 护甲减免=护甲值/（护甲值+5*对手等级）。 （注：赤膊无铠甲=护甲值1）
	unsigned int huJiaJianMian = hujiazhi/(hujiazhi + 5.0f * fightorLevel) * 100.0f;
	if (huJiaJianMian >= 100)
		huJiaJianMian = 99;
	return huJiaJianMian;
}



// 计算物理的格挡值
float PropertyCalculator::calculateWuLiGeDangZhi(const CalculateData& beAttacked) const
{
	// include额外加的
	float gedangzhi_include_add = 0.0f;
	for (int i=0, include_count = beAttacked.includeCalDatas.size(); i<include_count; i++)
	{
		gedangzhi_include_add += calculateWuLiGeDangZhi(beAttacked.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = beAttacked;
	flat_data.includeCalDatas.clear();



	float currentGeDangZhi = 0.0f;
	{
		currentGeDangZhi = euipCal->getEquipGeDangZhi(flat_data.allEquips);

		// 真身的强化共鸣加成
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				unsigned int addValue = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(flat_data.allEquips,GM_GeDangZhiAdd);
				currentGeDangZhi += addValue;
			}
		}


		currentGeDangZhi *= (flat_data.base_pro.effect_convert_per/100.0f); // 转换率
	}


	currentGeDangZhi += gedangzhi_include_add;

	
	return currentGeDangZhi;
}
// 计算格挡率
unsigned int PropertyCalculator::calculateGeDangLv(const CalculateData& beAttacked) const
{
	// include额外加的
	unsigned int gedanglv_include_add = 0;
	for (int i=0, include_count = beAttacked.includeCalDatas.size(); i<include_count; i++)
	{
		gedanglv_include_add += calculateGeDangLv(beAttacked.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = beAttacked;
	flat_data.includeCalDatas.clear();


	unsigned int currentGeDangLv = 0;
	{
		currentGeDangLv = euipCal->getEquipGeDangLv(flat_data.allEquips);
		currentGeDangLv *= (flat_data.base_pro.effect_convert_per/100.0f); // 效果因子
	}

	currentGeDangLv += gedanglv_include_add;

	return currentGeDangLv;
}
// 计算抗性值
float PropertyCalculator::calculateKuangXingZhi(const CalculateData& beAttacked) const
{
	// include额外加的
	float kangxinzhi_include_add = 0;
	for (int i=0, include_count = beAttacked.includeCalDatas.size(); i<include_count; i++)
	{
		kangxinzhi_include_add += calculateKuangXingZhi(beAttacked.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = beAttacked;
	flat_data.includeCalDatas.clear();



	float ret = 0.0f;
	{
		// 1智力=0.5抗性
		float kuangXingZhi = ((float)flat_data.base_pro.zhili) * 0.5f;
		kuangXingZhi += euipCal->getEquipKangXingZhi(flat_data.allEquips);
		ret = kuangXingZhi;

		// 真身角色的阶的血量加成（元神的阶不能加成，只用于等级的升级的砍）
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float hujiaPer = ((float)CS::getHeroStageEffectRatio(flat_data.base_pro.stage))/100.0f;
				ret += ret * hujiaPer;
			}
		}

		ret *= (flat_data.base_pro.effect_convert_per/100.0f);
	}

	ret += kangxinzhi_include_add;

	return ret;
}
// 计算抗性减免率,减免魔法攻击
unsigned int PropertyCalculator::calculateKangXingMianLv(int fightLevel,const CalculateData& beAttacked) const
{
	// 这个跟护甲减免一样，只需要一个抗性值，这里就不合成了,不包裹了

	return calculateKangXingMianLv(fightLevel,calculateKuangXingZhi(beAttacked));
}
unsigned int PropertyCalculator::calculateKangXingMianLv(int fightLevel,float kangxingzhi) const
{
	// 抗性减免=抗性值/（抗性值+10*对手等级）
	unsigned int kuangXingJianMian = kangxingzhi / (float)(kangxingzhi + 10.0f * fightLevel) * 100;
	return kuangXingJianMian;
}


// 计算一个角色攻击另一个角色，产生的伤害值,得到的就是数值
// 根据角色类型，自动确定是物理还是，魔法伤害
// 只是攻击的伤害值，没有技能，要计算暴击，就在这个基础上计算即可
// 综合了攻击力，护甲减免，抗性减免，格挡值，就是直接需要减去的值
float PropertyCalculator::calculateAttackShangHaiValue(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	// 警告：
	// 这里不能包裹
	// 简单点的原因就是出现了2个CalculateData，没法包裹，需要把数据推后到只有一个CalculateData的地方去处理，
	// 这里就不包裹了
	// 其实包不包裹无所谓，因为底层都是包裹处理了的。
	// 上层包裹于下层包裹的最大的区别就是数据量误差的区别。
	// 在顶层看来，最上面包裹一下误差最小。

	//int shanghai_leixing = f_pro->mShangHaiLeiXing;
	int shanghai_leixing = SHANG_HAI_WU_LI;

	float ret = 0;
	{
		if (shanghai_leixing == SHANG_HAI_WU_LI)
		{
			ret = calculateWuLiShangHai(attack,beAttacked,range);
		}
		else if (shanghai_leixing == SHANG_HAI_MO_FA)
		{
			ret = calculateMoFaShangHai(attack,beAttacked,range);
		}
		// 报错
		else
		{
			CCLog("ValueCalculator::calculateAttackValue --> mShangHaiLeiXing != SHANG_HAI_MO_FA && mShangHaiLeiXing != SHANG_HAI_WU_LI");
			CCAssert(false,"");
		}
	}


	return ret;
}
float PropertyCalculator::calculateAttackShangHaiValue(const CalculateData& attack,const Guaiwu* beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	// include额外加的
	float shanghai_include_add = 0;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		shanghai_include_add += calculateAttackShangHaiValue(attack.includeCalDatas[i],beAttacked,range);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = attack;
	flat_data.includeCalDatas.clear();

	//int shanghai_leixing = f_pro->mShangHaiLeiXing;
	int shanghai_leixing = SHANG_HAI_WU_LI;

	float ret = 0;
	{
		if (shanghai_leixing == SHANG_HAI_WU_LI)
		{
			ret = calculateWuLiShangHai(
				calculateGongJiLi(flat_data,range),
				beAttacked->getHuJiaJianMian(), // 怪物的护甲减免率是指定的
				0,0,
				buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
		}
		else if (shanghai_leixing == SHANG_HAI_MO_FA)
		{
			ret = calculateMoFaShangHai(
				calculateGongJiLi(flat_data,range),
				beAttacked->getHuJiaJianMian(),// 护甲减免是指定的
				beAttacked->getKangXingJianMian(),// 抗性减免是指定的
				buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
		}
		// 报错
		else
		{
			CCLog("ValueCalculator::calculateAttackValue --> mShangHaiLeiXing != SHANG_HAI_MO_FA && mShangHaiLeiXing != SHANG_HAI_WU_LI");
			CCAssert(false,"");
		}

		//// 转换率
		//ret *= (flat_data.base_pro.effect_convert_per/100.0f);

		if (ret <= 1)
			ret = 1;
	}

	ret += shanghai_include_add;

	return ret;
}
float PropertyCalculator::calculateAttackShangHaiValue(const Guaiwu* attack,const CalculateData& beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	//int shanghai_leixing = attack->getShangHaiLeiXing();
	int shanghai_leixing = SHANG_HAI_WU_LI;

	// 注意：
	// 怪物攻击不需要值域的指定，这里指定了就没有用

	// 这里不能简单的包裹了
	// 如果就直接包裹相加肯定是不对的，因为不可能beAttacked越多，伤害越大。
	// 
	// 这里就直接放弃包裹，对于buff计算里面没有算到beAttacked中include的buff的情况：
	// 这不可能有！！因为include的数据不会在作战中动态变化！也就是buff一直是空的！！

	float ret = 0;
	if (shanghai_leixing == SHANG_HAI_WU_LI)
	{
		ret = calculateWuLiShangHai(
			attack->getGongJiLi(range),
			calculateHuJiaJianMianLv(attack->getLevel(),beAttacked),
			calculateGeDangLv(beAttacked),
			calculateWuLiGeDangZhi(beAttacked),
			buffCal->getBuffJianShangLv(beAttacked.allBuffs));
	}
	else if (shanghai_leixing == SHANG_HAI_MO_FA)
	{
		ret = calculateMoFaShangHai(
			attack->getGongJiLi(range),
			calculateHuJiaJianMianLv(attack->getLevel(),beAttacked),
			calculateKangXingMianLv(attack->getLevel(),beAttacked),
			buffCal->getBuffJianShangLv(beAttacked.allBuffs));
	}
	// 报错
	else
	{
		CCLog("ValueCalculator::calculateAttackValue --> mShangHaiLeiXing != SHANG_HAI_MO_FA && mShangHaiLeiXing != SHANG_HAI_WU_LI");
		CCAssert(false,"");
	}


	if (ret <= 0)
		ret = 1;

	return ret;
}
float PropertyCalculator::calculateAttackShangHaiValue(const Guaiwu* attack,const Guaiwu* beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	//int shanghai_leixing = attack->getShangHaiLeiXing();
	int shanghai_leixing = SHANG_HAI_WU_LI;

	// 注意：
	// 怪物攻击不需要值域的指定，这里指定了就没有用
	// 
	// 怪物之间，不用包裹： 没有CalculatData不用包裹

	float ret = 0;

	if (shanghai_leixing == SHANG_HAI_WU_LI)
	{
		ret = calculateWuLiShangHai(
			attack->getGongJiLi(range),
			beAttacked->getHuJiaJianMian(),
			0,0, // 怪物没有格挡
			buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
	}
	else if (shanghai_leixing == SHANG_HAI_MO_FA)
	{
		ret = calculateMoFaShangHai(
			attack->getGongJiLi(range),
			beAttacked->getHuJiaJianMian(),
			beAttacked->getKangXingJianMian(),
			buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
	}
	// 报错
	else
	{
		CCLog("ValueCalculator::calculateAttackValue --> mShangHaiLeiXing != SHANG_HAI_MO_FA && mShangHaiLeiXing != SHANG_HAI_WU_LI");
		CCAssert(false,"");
	}

	if (ret <= 0)
		ret = 1;

	return ret;
}
float PropertyCalculator::calculateAttackShangHaiValue(const Guaiwu* attack,const TeamHero* beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	//int shanghai_leixing = attack->getShangHaiLeiXing();
	int shanghai_leixing = SHANG_HAI_WU_LI;

	// 注意：
	// 怪物攻击不需要值域的指定，这里指定了就没有用

	// 没有CalculatData不用包裹

	float ret = 0;

	if (shanghai_leixing == SHANG_HAI_WU_LI)
	{
		float teamHuJiaZhi = beAttacked->getTeamProperty().mHuJiaZhi + buffCal->getBuffHuJiaZhi(beAttacked->getCurrentAttachedBuff());
		ret = calculateWuLiShangHai(
			attack->getGongJiLi(range),
			calculateHuJiaJianMianLv(attack->getProperty().level,teamHuJiaZhi),
			0,0,
			buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
	}
	else if (shanghai_leixing == SHANG_HAI_MO_FA)
	{
		float teamKangXingZhi = beAttacked->getTeamProperty().mKangXingZhi /*+ buffCal->getBu*/;
		float teamHuJiaZhi = beAttacked->getTeamProperty().mHuJiaZhi + buffCal->getBuffHuJiaZhi(beAttacked->getCurrentAttachedBuff());
		ret = calculateMoFaShangHai(
			attack->getGongJiLi(range),
			calculateHuJiaJianMianLv(attack->getProperty().level,teamHuJiaZhi),
			calculateKangXingMianLv(attack->getProperty().level,teamKangXingZhi),
			buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
	}
	// 报错
	else
	{
		CCLog("ValueCalculator::calculateAttackValue --> mShangHaiLeiXing != SHANG_HAI_MO_FA && mShangHaiLeiXing != SHANG_HAI_WU_LI");
		CCAssert(false,"");
	}

	if (ret <= 0)
		ret = 1;

	return ret;
}
float PropertyCalculator::calculateAttackShangHaiValue(const TeamHero* attack,const Guaiwu* beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	float ret = 0;

	float gongJiLi =  attack->getGongJiLi(range);
	/*switch (range)
	{
	case Range_Ramdom:
		gongJiLi = randomWithin(attack->getTeamProperty().mGongJiLi_MIN,attack->getTeamProperty().mGongJiLi_MAX);
		break;
	case Range_Max:
		gongJiLi = max(attack->getTeamProperty().mGongJiLi_MIN,attack->getTeamProperty().mGongJiLi_MAX);
		break;
	case Range_Average:
		gongJiLi = (attack->getTeamProperty().mGongJiLi_MIN+attack->getTeamProperty().mGongJiLi_MAX)/2.0f;
		break;
	case Range_Min:
		gongJiLi = min(attack->getTeamProperty().mGongJiLi_MIN,attack->getTeamProperty().mGongJiLi_MAX);
		break;
	default:
		CCAssert(false,"不支持的值域类型");
		break;
	}*/

	//int shanghai_leixing = attack->getShangHaiLeiXing();
	int shanghai_leixing = SHANG_HAI_WU_LI;

	if (shanghai_leixing == SHANG_HAI_WU_LI)
	{
		ret = calculateWuLiShangHai(
			gongJiLi,
			beAttacked->getHuJiaJianMian(),
			0,0,
			buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
	}
	else if (shanghai_leixing == SHANG_HAI_MO_FA)
	{
		ret = calculateMoFaShangHai(
			gongJiLi,
			beAttacked->getHuJiaJianMian(),
			beAttacked->getKangXingJianMian(),
			buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
	}
	// 报错
	else
	{
		CCLog("ValueCalculator::calculateAttackValue --> mShangHaiLeiXing != SHANG_HAI_MO_FA && mShangHaiLeiXing != SHANG_HAI_WU_LI");
		CCAssert(false,"");
	}

	if (ret <= 0)
		ret = 1;

	return ret;
}



// 计算actor的闪避率
unsigned int  PropertyCalculator::calculateShanBiLv(const CalculateData& actor) const
{
	// include额外加的
	unsigned int shanbilv_include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		shanbilv_include_add += calculateShanBiLv(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	int ret = 0;
	{

		//2014-02-24去掉敏捷到闪避的转换
		float shanbilv = 0;

		//2014-02-24给职业设定初始的闪避率
		// 刺客、游戏、猎手、神箭手的闪避改为天生10%，其他所有职业的闪避全部为0
		switch (actor.base_pro.profession)
		{
		case Profession_Chike:
		case Profession_YouXia:
		case Profession_LieShou:
		case Profession_ShenJianShou:
			shanbilv = 0.1f;
		default:
			break;
		}

		//2014-02-24去掉敏捷到闪避的转换
		////0~100的敏捷点，每敏捷点转换为0.1%闪避； 
		////101~250的敏捷点，每敏捷点转换为0.05%闪避，
		////251~500的敏捷点，每敏捷点转换为0.03%闪避；
		////501以上的敏捷点，每敏捷点转换为0.01%闪避。
		//float mingjie = calculateMinJie(flat_data);
		//float shanbilv = 0;
		//if (mingjie > 500)
		//{
		//	shanbilv += (mingjie-500.0f)*0.0001f;//501以上的敏捷点，每敏捷点转换为0.01%闪避。
		//}
		//if (mingjie > 250)
		//{
		//	shanbilv += ( min(mingjie,500.0f) - 250)*0.0003f;//251~500的敏捷点，每敏捷点转换为0.03%闪避；
		//}
		//if (mingjie > 100)
		//{
		//	shanbilv += ( min(mingjie,250.0f) - 100)*0.0005f;//101~250的敏捷点，每敏捷点转换为0.05%闪避，
		//}
		//if (mingjie > 0)
		//{
		//	shanbilv += ( min(mingjie,100.0f) - 0)*0.003f;//0~100的敏捷点，每敏捷点转换为0.1%闪避；
		//}

		shanbilv *= 100;

		int buff_shanbi_add = buffCal->getBuffShanBiLv(flat_data.allBuffs);

		ret = buff_shanbi_add + (int)shanbilv;
		ret = ret > 0 ? ret : 0;

		ret *= (flat_data.base_pro.effect_convert_per / 100.0f);
	}

	ret += shanbilv_include_add;

	return ret;
}
// 计算暴击率
unsigned int PropertyCalculator::calculateBaoJiLv(const CalculateData& actor) const
{
	// include额外加的
	unsigned int baojilv_include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		baojilv_include_add += calculateBaoJiLv(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	unsigned int ret_baoji = 0;
	{
		unsigned int ret = flat_data.base_pro.baojilv;
		ret += euipCal->getEquipBaojiLv(flat_data.allEquips);
		int buff_baoji_add = buffCal->getBuffBaoJiLv(flat_data.allBuffs);

		ret_baoji = buff_baoji_add + ret;
		ret_baoji = ret_baoji>0?ret_baoji:0;

		ret_baoji *= (flat_data.base_pro.effect_convert_per/100.0f);
	}

	ret_baoji += baojilv_include_add;

	return ret_baoji;
}
//
unsigned int PropertyCalculator::calculateJiNengChuFaLv(const CalculateData& actor) const
{

	return 0;

	//// include额外加的
	//unsigned int include_add = 0;
	//for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	//{
	//	include_add += calculateJiNengChuFaLv(actor.includeCalDatas[i]);
	//}
	//// 拷贝一份没有include的数据
	//CalculateData flat_data = actor;
	//flat_data.includeCalDatas.clear();


	//float ret = 0.0f;
	//{
	//	ret = flat_data.basePro->mJiNengFaChuLv;
	//	ret += euipCal->getSkillRatioAdd(flat_data.allEquips);

	//	ret *= (flat_data.base_pro.effect_convert_per/100.0f);
	//}

	//ret += include_add;
	//
	//return ret;
}

unsigned int PropertyCalculator::calculateJiNengZhouQi(const CalculateData& actor) const
{
	CCAssert(actor.base_pro.skill_type != 0,"有技能才能调用");
#if ALWAYS_USE_SKILL == 1
	return 1;
#endif

	// 这里比较特殊
	// 不能套一下就行了
	// 需要单独计算装备减少的技能数据

	unsigned int equip_sub_jineng_zhouqi = 0;
	{
		for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
		{
			equip_sub_jineng_zhouqi += euipCal->getSkillZhouQiSub(actor.includeCalDatas[i].allEquips);

			// 不支持include的data里面还include其他的
			CCAssert(actor.includeCalDatas[i].includeCalDatas.empty(),"这个地方没法用递归，所有就简单点只支持一层的嵌套！");
		}

		equip_sub_jineng_zhouqi += euipCal->getSkillZhouQiSub(actor.allEquips);
	}

	int ret = CS::getSkillZhouQi(actor.base_pro.skill_type);
	ret -= equip_sub_jineng_zhouqi;
	if (ret < 2) // 周期至少是2
		ret = 2;
	return ret;
}
unsigned int PropertyCalculator::calculateJiNengTurn(const CalculateData& actor) const
{
	CCAssert(actor.base_pro.skill_type != 0,"有技能才能调用");
#if ALWAYS_USE_SKILL == 1
	return 1;
#endif

	// 这里跟calculateJiNengZhouQi一样，我们需要单独处理
	// 需要自己包一层
	unsigned int skill_turn_sub = 0;
	{
		skill_turn_sub += euipCal->getSkillTurnSub(actor.allEquips);
		for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
		{
			skill_turn_sub += euipCal->getSkillTurnSub(actor.includeCalDatas[i].allEquips);

			// 不支持include的data里面还include其他的
			CCAssert(actor.includeCalDatas[i].includeCalDatas.empty(),"这个地方没法用递归，所有就简单点只支持一层的嵌套！");
		}
	}


	int ret = CS::getSkillTurn(actor.base_pro.skill_type);
	// 计算当前情况的周期
	int zhouqi = calculateJiNengZhouQi(actor);
	if (ret > zhouqi) // 先压制好触发点，再进行触发点的修正
		ret = zhouqi;

	ret -= skill_turn_sub;
	if (ret < 1)	// 触发点最多只能是第一次就触发
		ret = 1;
	return ret;
}
unsigned int PropertyCalculator::calculateJiNengResult(const CalculateData& actor) const
{
	CCAssert(actor.base_pro.skill_type != 0,"有技能才能调用");
	// 这里也不能递归简单的包裹
	// 因为int ret = 100;定义了，那么每次都会加成100%
	// 所有还是得单独写
	// 这里跟calculateJiNengZhouQi一样，我们需要单独处理
	// 需要自己包一层
	/*
	float skill_ration_add = 0;
	{
		skill_ration_add += euipCal->getSkillRatioAdd(actor.allEquips);
		for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
		{
			skill_ration_add += euipCal->getSkillRatioAdd(actor.includeCalDatas[i].allEquips);

			// 不支持include的data里面还include其他的
			CCAssert(actor.includeCalDatas[i].includeCalDatas.empty(),"这个地方没法用递归，所有就简单点只支持一层的嵌套！");
		}
	}
	*/

    /* 2014.07.03 增加技能伤害加成 */
	float skill_ration_add = 0;
	{
		skill_ration_add += euipCal->getSkillRatioAdd(actor.allEquips);
		for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
		{
			skill_ration_add += euipCal->getSkillResultAddPercent(actor.includeCalDatas[i].allEquips);

			// 不支持include的data里面还include其他的
			CCAssert(actor.includeCalDatas[i].includeCalDatas.empty(),"这个地方没法用递归，所有就简单点只支持一层的嵌套！");
		}
	}


	int ret = 100; // 从100%开始嘛，默认就是1.0f
	ret += skill_ration_add;
	// 等级带来的加成
	ret += CS::getSkillEffectRatioAddBySkillLevel(actor.base_pro.skill_level);
	return ret;
}

// 计算attack攻击beAttacked照成的物理伤害
float PropertyCalculator::calculateWuLiShangHai(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	// 警告
	// 这里有2个CalculateData，没法包裹，直接交给下一层
	// buff的话不用管，因为includeData的buff不会有

	// 物理伤害
	// 物理伤害=攻击力×（1-护甲减免）-格挡值
	float ret = calculateWuLiShangHai(
		calculateGongJiLi(attack,range),
		calculateHuJiaJianMianLv(attack.base_pro.level,beAttacked),
		calculateGeDangLv(beAttacked),
		calculateWuLiGeDangZhi(beAttacked),
		buffCal->getBuffJianShangLv(beAttacked.allBuffs));
	return ret;
}
float PropertyCalculator::calculateWuLiShangHai(float gongjiLi,int huJiaJianMianLv,int gedanglv,float gedangzhi,int jiansahnglv) const
{
	// 物理伤害
	// 物理伤害=攻击力×（1-护甲减免）-格挡值
	float gongJiLi = gongjiLi;
	float huJiaJianMian = ((float)huJiaJianMianLv)/100.0f;
	float geDangZhi = gedangzhi;
	float geDangLv = ((float)gedanglv)/100.0f;

	if (lfRandom0_1() > geDangLv)
		geDangZhi = 0;

	float ret = gongJiLi*(1-huJiaJianMian) - geDangZhi;
	// 减伤
	float jianshang_lv = ((float)jiansahnglv)/100.0f;
	ret = ret - ret*jianshang_lv;

	if (ret <= 1)
	{
		ret = 1;
	}

	return ret;
}

// 计算attack攻击beAttacked照成的魔法伤害
float PropertyCalculator::calculateMoFaShangHai(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	// 警告
	// 这里有2个CalculateData，没法包裹，直接交给下一层
	// buff的话不用管，因为includeData的buff不会有

	float ret = calculateMoFaShangHai(
		calculateGongJiLi(attack,range),
		calculateHuJiaJianMianLv(attack.base_pro.level,beAttacked),
		calculateKangXingMianLv(attack.base_pro.level,beAttacked),
		buffCal->getBuffJianShangLv(beAttacked.allBuffs));

	return ret;
}
// 计算attack攻击beAttacked照成的魔法伤害
float PropertyCalculator::calculateMoFaShangHai(float gongjiLi,int huJiaJianMianLv,int kangxingJianMianLv,int jiansahnglv) const
{
	// 魔法伤害
	// 魔法伤害=攻击力*（1-护甲减免）*（1-抗性减免）
	float gongJiLi = gongjiLi;
	float kuangXingJianMian = ((float)kangxingJianMianLv) / 100.0f;
	float hujiaJianMian = huJiaJianMianLv / 100.0f;

	float ret = gongJiLi * (1- hujiaJianMian) * (1 - kuangXingJianMian);
	// 减伤
	float jianshang_lv = ((float)jiansahnglv)/100.0f;
	ret = ret - ret*jianshang_lv;

	if (ret <= 0)
	{
		ret = 1;
	}
	return ret;
}

// 暴击因子
unsigned int PropertyCalculator::calculateBaoJiShangHaiYinzi(const CalculateData& actor) const
{
	// include额外加的
	unsigned int include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateBaoJiShangHaiYinzi(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	unsigned int ret = 0;
	{
		ret = flat_data.base_pro.baoshang_yinzi;
		ret += euipCal->getEquipBaoshangYinZi(flat_data.allEquips);

		// 真身的强化共鸣加成
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				unsigned int addValue = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(flat_data.allEquips,GM_BaoShangYinZiAdd);
				ret += addValue;
			}
		}

		ret *= (flat_data.base_pro.effect_convert_per/100.0f);
	}
	
	ret += include_add;

	return ret;
}
float PropertyCalculator::calculateLiliang(const CalculateData& actor) const
{
	// include额外加的
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateLiliang(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();

	float ret = 0;
	{
		ret = flat_data.base_pro.liliang;
		ret += euipCal->getEquipLiliang(flat_data.allEquips);

		ret *= (flat_data.base_pro.effect_convert_per/100.0f);
	}

	ret += include_add;

	return ret;
}
float PropertyCalculator::calculateMinJie(const CalculateData& actor) const
{
	// include额外加的
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateMinJie(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();

	float ret = 0;
	{
		ret = flat_data.base_pro.mingjie;
		ret += euipCal->getEquipMingJie(flat_data.allEquips);

		ret *= (flat_data.base_pro.effect_convert_per/100.0f);
	}

	ret += include_add;

	return ret;
}
float PropertyCalculator::calculateZhili(const CalculateData& actor) const
{
	// include额外加的
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateZhili(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	float ret = 0;
	{
		ret = flat_data.base_pro.zhili;
		ret += euipCal->getEquipZhili(flat_data.allEquips);

		ret *= (flat_data.base_pro.effect_convert_per/100.0f);
	}
	
	ret += include_add;

	return ret;
}
float PropertyCalculator::calculateTineng(const CalculateData& actor) const
{
	// include额外加的
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateTineng(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	float ret = 0;
	{
		ret = flat_data.base_pro.tineng;
		ret += euipCal->getEquipTineng(flat_data.allEquips);

		ret *= (flat_data.base_pro.effect_convert_per/100.0f);
	}

	ret += include_add;

	return ret;
}
float PropertyCalculator::calculateHp(const CalculateData& actor) const
{
	// include额外加的
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateHp(actor.includeCalDatas[i]);
	}
	// 拷贝一份没有include的数据
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	float ret = 0.0f;
	{
		// ---> 不要了--->血量与体能的关系：1体能=1.1*(等级/5)  点血。（注：结果不足2，按2计）
		// 血量与体能的关系：1体能=1 点血。
		//血量= （初始血量 + (本身体能+装备附加体能)* (体能转换血量公式)） * (1+装备附加百分比血值)

		float zb_fuJia_tiNeng = euipCal->getEquipTineng(flat_data.allEquips);
		float benShenTiNeng = flat_data.base_pro.tineng;
		float chuShiXueZhi = flat_data.base_pro.base_hp;
		float zhuangbeiFuJiaPercent = euipCal->getEquipHp_ratio(flat_data.allEquips)/100.0f;

		// 血量与体能的关系：1体能=1.1*(等级/5)  点血。（注：结果不足2，按2计）
		//float tineng_hp = (benShenTiNeng + zb_fuJia_tiNeng) * 1.1f * (flat_data.base_pro.level/5.0f);
		//if (tineng_hp < 2)
		//	tineng_hp = 2;

		// 血量与体能的关系：1体能=1 点血。
		//stub 
		float tineng_hp = 7*(benShenTiNeng + zb_fuJia_tiNeng);

		ret  = (chuShiXueZhi + tineng_hp) * (1 + zhuangbeiFuJiaPercent);


		// 职业血量加成
		{
			if (flat_data.base_pro.profession < Profession_GW_Start)
			{
				int bloodAdd = 0;
				int gongjiAdd = 0;
				CS::getProfesionBloodAndGongJiAddPercent(flat_data.base_pro.profession,bloodAdd,gongjiAdd);
				float bloodPer = ((float)bloodAdd)/100.0f;
				ret += ret * bloodPer;
			}
		}

		// 真身角色的阶的血量加成（元神的阶不能加成，只用于等级的升级的砍）
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float bloodPer = ((float)CS::getHeroStageEffectRatio(flat_data.base_pro.stage))/100.0f;
				ret += ret * bloodPer;
			}
		}

		// 真身的强化共鸣加成
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float addPer = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(flat_data.allEquips,GM_HPAddPer);
				addPer = addPer/100.0f;
				ret += ret * addPer;
			}
		}

		
		// 这里就可以有percent了
		// 因为前面用到每一个属性都是没有计算percent的
		float percent = ((float)flat_data.base_pro.effect_convert_per)/100.0f;
		ret *= percent;

	}
	
	ret += include_add;

	return ret;
}

PropertyCalculator::~PropertyCalculator(void)
{
	delete euipCal;
	delete buffCal;
}






// 装备的计算
EquipCalculator::EquipCalculator()
{

}
// 计算这些值都是各管个的，装备管装备的护甲值，需要加上力量也是自己管
// 而整体属性计算的也是，自己计算自己的护甲以及自己力量的影响，不用管其他地方的力量
float EquipCalculator::getEquipHuJiaZhi(vector<EquipData> equipVec) const
{
	float ret = 0;
	int num = equipVec.size();

	bool wareKuiJia = false;

	for (int i=0;i<num;i++)
	{
		float percent = 0.0f;
		if ( ((EquipType)CS::getEquipType(equipVec[i].zhuangbei_type)) == EquipType_FangJu)
		{
			for (int qianghua_index = 0;qianghua_index < equipVec[i].qianghua_data.qianghua_count;qianghua_index++)
			{
				percent += equipVec[i].qianghua_data.info_arr[qianghua_index].percent;
			}
			percent *= 0.01f;	// 换算成100分比
		}

		// 看有没有穿铠甲
		if (equipVec[i].zhuangbei_type == ZhuangbeiType_KuijiaQin || equipVec[i].zhuangbei_type == ZhuangbeiType_KuijiaZhong)
		{
			wareKuiJia = true;
		}

		ret += equipVec[i].hujia;			// 本身护甲
		ret += equipVec[i].hujia*percent;	// 强化提升
		ret += equipVec[i].getAtrriValueByType(AttrType_hujia_add);
	}

	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_hujia_add).attr_value; // 套装加成
	
	// 2014-02-24去掉力量到护甲的转换
	////1力量=0.3护甲
	//ret += getEquipLiliang(equipVec) * 0.3f;

	//// 强化共鸣，需要放到外面，因为要判断是否是元神
	//float addPer = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(equipVec,GM_HuJiaAddPer);
	//addPer = addPer/100.0f;
	//ret += ret * addPer;

	if (!wareKuiJia)
	{// 没穿铠甲，护甲需要额外加上4
		ret += 4.0f;
	}

	return ret;
}
float EquipCalculator::getEquipGeDangZhi(vector<EquipData> equipVec) const
{
	// 暂时不要了
	return 0;

	float ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_gedangzhi);
	}

	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_gedangzhi).attr_value; // 套装加成
	return ret;
}
unsigned int EquipCalculator::getEquipGeDangLv(vector<EquipData> equipVec) const
{
	// 暂时不要了
	return 0;

	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_gedanglv);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_gedanglv).attr_value; // 套装加成
	return ret;
}
float EquipCalculator::getEquipKangXingZhi(vector<EquipData> equipVec) const
{
	// 暂时不要了
	return 0;

	float ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_kangxing);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_kangxing).attr_value; // 套装加成
	//1智力=0.5抗性
	float zhiLi = getEquipZhili(equipVec);
	return ret + zhiLi * 0.5f;
}
void EquipCalculator::getEquipGongJiLi(vector<EquipData> equipVec,float& gongji_min,float& gongji_max,float& gongji_min_add,float& gongji_max_add) const
{
	gongji_min = 0;
	gongji_max = 0;
	gongji_min_add = 0;
	gongji_max_add = 0;


	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		float percent = 0.0f;
		if ( ((EquipType)CS::getEquipType(equipVec[i].zhuangbei_type)) == EquipType_WuQi)
		{
			for (int qianghua_index = 0;qianghua_index < equipVec[i].qianghua_data.qianghua_count;qianghua_index++)
			{
				percent += equipVec[i].qianghua_data.info_arr[qianghua_index].percent;
			}
			percent *= 0.01f; // 换算成100分比
		}
		gongji_min += equipVec[i].gongji_min;
		gongji_max += equipVec[i].gongji_max;
		gongji_min += equipVec[i].gongji_min * percent;
		gongji_max += equipVec[i].gongji_max * percent;

		gongji_min_add += equipVec[i].getAtrriValueByType(AttrType_gongji_min);
		gongji_max_add += equipVec[i].getAtrriValueByType(AttrType_gongji_max);
	}

	gongji_min_add += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_gongji_min).attr_value; // 套装加成
	gongji_max_add += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_gongji_max).attr_value; // 套装加成

}

float EquipCalculator::getEquipLiliang(vector<EquipData> equipVec) const
{
	float ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_liliang);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_liliang).attr_value; // 套装加成
	// 宝石
	{
		float liliang;
		float zhili;
		float minjie;
		float tineng;
		getEquipBaoShiJiaCheng(equipVec,liliang,minjie,zhili,tineng);
		ret += liliang;
	}

	return ret;
}

float EquipCalculator::getEquipMingJie(vector<EquipData> equipVec) const
{
	float ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_mingjie);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_mingjie).attr_value; // 套装加成
	// 宝石
	{
		float liliang;
		float zhili;
		float minjie;
		float tineng;
		getEquipBaoShiJiaCheng(equipVec,liliang,minjie,zhili,tineng);

		ret += minjie;
	}
	return ret;
}
float EquipCalculator::getEquipZhili(vector<EquipData> equipVec) const
{
	float ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_zhili);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_zhili).attr_value; // 套装加成
	// 宝石
	{
		float liliang;
		float zhili;
		float minjie;
		float tineng;
		getEquipBaoShiJiaCheng(equipVec,liliang,minjie,zhili,tineng);

		ret += zhili;
	}

	return ret;
}
float EquipCalculator::getEquipTineng(vector<EquipData> equipVec) const
{
	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_tineng);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_tineng).attr_value; // 套装加成

	// 宝石
	{
		float liliang;
		float zhili;
		float minjie;
		float tineng;
		getEquipBaoShiJiaCheng(equipVec,liliang,minjie,zhili,tineng);

		ret += tineng;
	}

	return ret;
}
unsigned int EquipCalculator::getEquipBaojiLv(vector<EquipData> equipVec) const
{
	float ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_baoji);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_baoji).attr_value; // 套装加成
	return ret;
}
unsigned int EquipCalculator::getEquipBaoshangYinZi(vector<EquipData> equipVec) const
{
	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_baoshang);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_baoshang).attr_value; // 套装加成
	return ret;
}

unsigned int EquipCalculator::getEquipGongJiAdd_ratio(vector<EquipData> equipVec) const
{
	//// 强化共鸣 应该放在外面，而且还要判断他是否是元神
	//return ZhuangbeiGongMingManager::getManager()->getPropertyAdd(equipVec,GM_GongJiAddPer);

	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_actor_gongji_addratio);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_actor_gongji_addratio).attr_value; // 套装加成
	return ret;
}

unsigned int EquipCalculator::getEquipHp_ratio(vector<EquipData> equipVec) const
{
	//// 强化共鸣 应该放在外面，而且还要判断他是否是元神
	//return ZhuangbeiGongMingManager::getManager()->getPropertyAdd(equipVec,GM_HPAddPer);
	
	// 1.暂时不要血量加成了
	// 2.还是要血量加成了，因为套装加上了这个属性了AttrType_actor_hp_addratio
	// 
	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_hp_percent);
		// AttrType_actor_hp_addratio跟AttrType_hp_percent一样
		// 就放在一起处理了
		ret += equipVec[i].getAtrriValueByType(AttrType_actor_hp_addratio);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_hp_percent).attr_value; // 套装加成
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_actor_hp_addratio).attr_value; // 套装加成
	return ret;
}

unsigned int EquipCalculator::getSkillRatioAdd(vector<EquipData> equipVec) const
{
	return 0;
	//unsigned int ret = 0;
	//int num = equipVec.size();
	//for (int i=0;i<num;i++)
	//{
	//	ret += equipVec[i].getAtrriValueByType(AttrType_skillratio);
	//}
	//ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_skillratio).attr_value; // 套装加成
	//return ret;
}

unsigned int EquipCalculator::getSkillZhouQiSub(vector<EquipData> equipVec) const
{
	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_skillzhouqi);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_skillzhouqi).attr_value; // 套装加成
	return ret;
}
unsigned int EquipCalculator::getSkillTurnSub(vector<EquipData> equipVec) const
{
	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_skillturn);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_skillturn).attr_value; // 套装加成
	return ret;
}
unsigned int EquipCalculator::getSkillResultAddPercent(vector<EquipData> equipVec) const
{
	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_skillresult);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_skillresult).attr_value; // 套装加成
	return ret;
}


// 宝石加成
void EquipCalculator::getEquipBaoShiJiaCheng(vector<EquipData> equipVec,float& liLiang,float& minJie,float& zhiLi,float& tiNeng) const
{
	liLiang = 0;
	minJie = 0;
	zhiLi = 0;
	tiNeng = 0;

	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		unsigned int temp_liLiang = 0;
		unsigned int temp_minJie = 0;
		unsigned int temp_zhiLi = 0;
		unsigned int temp_tiNeng = 0;
		{
			if (equipVec[i].diamond_type_1 != 0)
			{
				CS::getBaoShiJiaCheng(equipVec[i].diamond_type_1,temp_liLiang,temp_minJie,temp_zhiLi,temp_tiNeng);
			}
			liLiang += temp_liLiang;
			minJie += temp_minJie;
			zhiLi += temp_zhiLi;
			tiNeng += temp_tiNeng;
		}
		{
			temp_liLiang = 0;
			temp_minJie = 0;
			temp_zhiLi = 0;
			temp_tiNeng = 0;
			if (equipVec[i].diamond_type_2 != 0)
			{
				CS::getBaoShiJiaCheng(equipVec[i].diamond_type_2,temp_liLiang,temp_minJie,temp_zhiLi,temp_tiNeng);
			}
			liLiang += temp_liLiang;
			minJie += temp_minJie;
			zhiLi += temp_zhiLi;
			tiNeng += temp_tiNeng;
		}
		{
			temp_liLiang = 0;
			temp_minJie = 0;
			temp_zhiLi = 0;
			temp_tiNeng = 0;
			if (equipVec[i].diamond_type_3 != 0)
			{
				CS::getBaoShiJiaCheng(equipVec[i].diamond_type_3,temp_liLiang,temp_minJie,temp_zhiLi,temp_tiNeng);
			}
			liLiang += temp_liLiang;
			minJie += temp_minJie;
			zhiLi += temp_zhiLi;
			tiNeng += temp_tiNeng;
		}
	}
}





// buff的实时计算
BuffCalculator::BuffCalculator()
{

}
BuffCalculator::~BuffCalculator()
{
}

int BuffCalculator::getBuffBaoJiLv(list<GameBuff> buffVec) const
{
	int ret = 0;
	for (list<GameBuff>::iterator it = buffVec.begin();it != buffVec.end();it++)
	{
		if (!it->effect_vec.empty())
		{
			ret += (it->effect_vec.begin())->baoji_add;
		}
	}
	return ret;
}
int BuffCalculator::getBuffShanBiLv(list<GameBuff> buffVec) const
{
	int ret = 0;
	for (list<GameBuff>::iterator it = buffVec.begin();it != buffVec.end();it++)
	{
		if (!it->effect_vec.empty())
		{
			ret += (it->effect_vec.begin())->shanbi_add;
		}
	}
	return ret;
}
float BuffCalculator::getBuffHuJiaZhi(list<GameBuff> buffVec) const
{
	float ret = 0;
	for (list<GameBuff>::iterator it = buffVec.begin();it != buffVec.end();it++)
	{
		if (!it->effect_vec.empty())
		{
			ret += (it->effect_vec.begin())->hujia_add;
		}
	}
	return ret;
}

int BuffCalculator::getBuffJianShangLv(list<GameBuff> buffVec) const
{
	int ret = 0;
	for (list<GameBuff>::iterator it = buffVec.begin();it != buffVec.end();it++)
	{
		if (!it->effect_vec.empty())
		{
			ret += (it->effect_vec.begin())->jianshang_lv;
		}
	}
	return ret;
}

int BuffCalculator::getGongJiLiAddLv(list<GameBuff> buffVec) const
{
	int ret = 0;
	for (list<GameBuff>::iterator it = buffVec.begin();it != buffVec.end();it++)
	{
		if (!it->effect_vec.empty())
		{
			ret += (it->effect_vec.begin())->gongjili_add_lv;
		}
	}
	return ret;
}

