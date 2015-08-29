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
// ������������,����vector��push
// �ڶ����Ƴ�ʼ��ʱ�����õĽ��ǿ������캯����������ȱʡ���캯��
PropertyCalculator::PropertyCalculator(const PropertyCalculator& cal)
{
	// ����new
	euipCal = new EquipCalculator();
	buffCal = new BuffCalculator();
}
// ��ֵ�������
PropertyCalculator& PropertyCalculator::operator = (const PropertyCalculator& rightData)
{
	if(this == &rightData) // ����Լ����Լ���ֵ��ֱ�ӷ���
	{
		return *this;
	}

	// ����new
	euipCal = new EquipCalculator();
	buffCal = new BuffCalculator();
	return *this;
}

float PropertyCalculator::calculateSkillShangHai(float actorPutongGongJiLi,const SkillDataForFight& skill)const
{
	//�����˺�=��ǰ���ܵ�Ч������*��ɫ��ǰ������*����Ч��
	float ret = actorPutongGongJiLi;

	float percent = ((float)CS::getSkillEffectRatioOfNormalAttack(skill.skillType))/100.0f;
	percent += ((float)CS::getSkillEffectRatioAddBySkillLevel(skill.skillLevel))/100.0f;

	ret *= percent;

	return ret;
}

// ���㹥���������ǵ�ǰ��ɫ�Ĺ�����������һ�㹥��������,�õ��ľ�����ֵ
float PropertyCalculator::calculateGongJiLi(const CalculateData& attack,ValueRange range /* = Range_Ramdom */) const
{
	// include����ӵ�
	float gongji_include_add = 0.0f;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		gongji_include_add += calculateGongJiLi(attack.includeCalDatas[i],range);
	}
	// ����һ��û��include������
	CalculateData flat_data = attack;
	flat_data.includeCalDatas.clear();


	float gongjili = 0;
	{
		// ������=װ��������*��0.02*������+1���� ��ע�����ֿ�ȭ=������1��
		float gongji_min = 0;
		float gongji_max = 0;
		float gongji_min_add = 0;
		float gongji_max_add = 0;
		// װ��������
		euipCal->getEquipGongJiLi(flat_data.allEquips,gongji_min,gongji_max,gongji_min_add,gongji_max_add);
		gongji_min += gongji_min_add;
		gongji_max += gongji_max_add;


		float mainProperty = flat_data.mainPro;

		float gongjiMin = ((float)gongji_min) * (0.02f*mainProperty + 1.0f);
		float gongjiMax = ((float)gongji_max) * (0.02f*mainProperty + 1.0f);

		// �����ӳ�
		float per_add = euipCal->getEquipGongJiAdd_ratio(flat_data.allEquips);
		per_add /= 100.0f;
		gongjiMin += gongjiMin * per_add;
		gongjiMax += gongjiMax * per_add;

		// ְҵ�Ĺ����ӳ�
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

		// �����ɫ�Ľ׵�Ѫ���ӳɣ�Ԫ��Ľײ��ܼӳɣ�ֻ���ڵȼ��������Ŀ���
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float gongjiPer = ((float)CS::getHeroStageEffectRatio(flat_data.base_pro.stage))/100.0f;
				gongjiMin += gongjiMin * gongjiPer;
				gongjiMax += gongjiMax * gongjiPer;
			}
		}

		// �����ǿ�������ӳ�
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
			gongjili = max(randomWithin_f(gongjiMin,gongjiMax),1.0f);// ��СΪ1
			break;
		case Range_Max:
			gongjili = max(max(gongjiMax,gongjiMin),1.0f);// ��СΪ1
			break;
		case Range_Average:
			gongjili = max((gongjiMin + gongjiMax)/2.0f,1.0f);// ��СΪ1
			break;
		case Range_Min:
			gongjili = max(min(gongjiMax,gongjiMin),1.0f);// ��СΪ1
			break;
		default:
			CCAssert(false,"��֧�ֵ�ֵ�����");
			break;
		}

		float gongjili_add_lv = buffCal->getGongJiLiAddLv(flat_data.allBuffs);
		gongjili_add_lv /= 100.0f;

		gongjili += gongjili*gongjili_add_lv;

		float percent = flat_data.base_pro.effect_convert_per / 100.0f;
		gongjili *= percent; // ת����
	}

	gongjili += gongji_include_add;

	return gongjili;
}

float PropertyCalculator::calculateGongJiLi_MacroValue(const CalculateData& attack,bool skillOK, const SkillDataForFight& skill, ValueRange range) const
{
	//��ɫ��۹�����=��ɫ��ǰ������*(1-������)+����������*�����˺�����*��������+��1/���ܴ�������*�����˺���

	// include����ӵ�
	float gongji_include_add = 0.0f;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		// ����Ķ�����skill
		gongji_include_add += calculateGongJiLi_MacroValue(attack.includeCalDatas[i],false,skill,range);
	}
	// ����һ��û��include������
	CalculateData flat_data = attack;
	flat_data.includeCalDatas.clear();


	float ret = 0.0f;
	{
		// �Ѿ�����ת���ʵ���
		// ���Ժ���Ͳ���Ҫ�ٴ���ת������
		const float base_gongji = calculateGongJiLi(flat_data,range);
		const float bao_shang = ((float)calculateBaoJiShangHaiYinzi(flat_data))/100.0f;
		const float baoji_lv = ((float)calculateBaoJiLv(flat_data))/100.0f;

		ret = base_gongji*(1-baoji_lv) + base_gongji*bao_shang*baoji_lv;

		// ����
		if (skillOK)
		{
			float skillZhouQi = calculateJiNengZhouQi(flat_data/*,skill.skillType*/);
			float jinengShangHai = calculateSkillShangHai(base_gongji,skill);
			ret += 1.0f/skillZhouQi*jinengShangHai;
		}

		// ת���� �� ������Ѿ�����ת�����ˡ�����Ͳ���ת������
		//float percent = flat_data.base_pro.effect_convert_per /100.0f;
		float percent = 1;
		ret *= percent;
	}


	// �����include������
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
	// include����ӵ�
	float include_add = 0.0f;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateZhanDouLi(attack.includeCalDatas[i],false,skill,range);
	}

	// ����һ��û��include������
	CalculateData flat_data = attack;
	flat_data.includeCalDatas.clear();

	//��ɫս����=��ɫ������+4*��ɫ��۹�����
	float ret = 0;
	{
		float chengshouli = calculateChengShouLi(flat_data);
		float gongjili_macro = calculateGongJiLi_MacroValue(flat_data,skillOK,skill,range);
		ret = chengshouli+gongjili_macro*4;

		// ת����,����ģ�calculateChengShouLi��calculateGongJiLi_MacroValue���Ѿ������ת���ʵ���
		// ���в���������
		//float percent = flat_data.base_pro.effect_convert_per/100.0f;
		float percent = 1;
		ret *= percent;

	}


	// include����
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


/*2014.07.03 �µļ���ս�����ķ���*/
float PropertyCalculator::calculateZhanDouLi(const CalculateData& attack,ValueRange range /* = Range_Ramdom */) const
{
    float hp_cap = calculateHp(attack);     //Ѫ��
    float attack_cap = calculateGongJiLi(attack, range);     //������
    float defend_cap = calculateHuJiaZhi(attack);     //����
    float crit_damage_cap = calculateBaoJiShangHaiYinzi(attack); //����
    float crit_ratio = calculateBaoJiLv(attack); //������
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
	// include����ӵ�
	float include_add = 0.0f;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateChengShouLi(attack.includeCalDatas[i]);
	}

	// ����һ��û��include������
	CalculateData flat_data = attack;
	flat_data.includeCalDatas.clear();

	//��ɫ������=��ɫ��ǰѪ��/(1-��ɫ��ǰ���׼�����(�ý�ɫ�Լ��ĵȼ�����))
	float ret = 0;
	{
		float currentHp = calculateHp(flat_data);
		float hujiaJianMianLv = calculateHuJiaJianMianLv(flat_data.base_pro.level,flat_data);
		hujiaJianMianLv /= 100.0f;
		ret = currentHp/(1-hujiaJianMianLv);

		// ת����
		//float percent = flat_data.base_pro.effect_convert_per/100.0f;
		float percent = 1;
		ret *= percent;
	}


	// include����
	ret += include_add;
	return ret;
}

float PropertyCalculator::calculateHuJiaZhi_MacroValue(const CalculateData& actor) const
{
	// include����ӵ�
	float hujia_include_add = 0.0f;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		hujia_include_add += calculateHuJiaZhi_MacroValue(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	float ret = 0;
	{
		ret = calculateHuJiaZhi(flat_data);
		//// ת����
		//float percent = flat_data.base_pro.effect_convert_per/100.0f;
		//ret *= percent;
	}

	// include����
	ret += hujia_include_add;

	return ret;
}
float PropertyCalculator::calculateKuangXingZhi_MacroValue(const CalculateData& beAttacked) const
{
	// include����ӵ�
	float kangxing_include_add = 0.0f;
	for (int i=0, include_count = beAttacked.includeCalDatas.size(); i<include_count; i++)
	{
		kangxing_include_add += calculateKuangXingZhi_MacroValue(beAttacked.includeCalDatas[i]);
	}
	// ����һ��û��include������
	CalculateData flat_data = beAttacked;
	flat_data.includeCalDatas.clear();

	float ret = 0.0f;
	{
		ret = calculateKuangXingZhi(flat_data);
		//float percent = flat_data.base_pro.effect_convert_per/100.0f;
		//ret *= percent; // ת������
	}

	ret += kangxing_include_add;

	return ret;
}
// ����actor�Ļ���ֵ
float PropertyCalculator::calculateHuJiaZhi(const CalculateData& actor) const
{
	// include����ӵ�
	float hujia_include_add = 0.0f;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		hujia_include_add += calculateHuJiaZhi(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	float ret = 0.0f;
	{
		ret = calculateHuJiaZhi(flat_data.base_pro.liliang,flat_data.allEquips,flat_data.allBuffs);

		// �����ɫ�Ľ׵�Ѫ���ӳɣ�Ԫ��Ľײ��ܼӳɣ�ֻ���ڵȼ��������Ŀ���
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float hujiaPer = ((float)CS::getHeroStageEffectRatio(flat_data.base_pro.stage))/100.0f;
				ret += ret * hujiaPer;
			}
		}

		// �����ǿ������
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				// ǿ�����������������˶����֮�������������û����
				float addPer = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(flat_data.allEquips,GM_HuJiaAddPer);
				addPer = addPer/100.0f;
				ret += ret * addPer;
			}
		}

		ret *= (flat_data.base_pro.effect_convert_per/100.0f); // ת������
	}


	ret += hujia_include_add;

	return ret;
}
float PropertyCalculator::calculateHuJiaZhi(unsigned int baseHeroLiliang,vector<EquipData> allEquips,list<GameBuff> allBuffs) const
{
	//2014-02-24ȥ�����׵�������ת��
	//// 1����=0.3����
	//float huJiaZhi = baseHeroLiliang*0.3f;

	float huJiaZhi = 0;

	huJiaZhi += euipCal->getEquipHuJiaZhi(allEquips);
	float buff_hujia_add = buffCal->getBuffHuJiaZhi(allBuffs);

	float ret = buff_hujia_add + huJiaZhi;

	return ret>1?ret:1; // ����ֵ��СΪ1
}

unsigned int PropertyCalculator::calculateHuJiaJianMianLv(int fightorLevel,const CalculateData& beAttacked) const
{
	
	// ������Ҫ���ǻ���ֵ�������ʾͲ����ٰ�һ���ˣ�
	// �������Ҫ��һ�㣬��ô�����������ô���أ�������ֱ�ӼӾ����ˣ���
	// ֱ�ӾͲ���һ������

	// ת����Ҳ�������ˣ���Ϊ�͸�����ֵ�йأ�����ֵ�Ѿ�ת�����ˣ���ô����Ͳ�ת��



	// ���׼���=����ֵ/������ֵ+5*���ֵȼ����� ��ע���ಲ������=����ֵ1��
	return calculateHuJiaJianMianLv(fightorLevel,calculateHuJiaZhi(beAttacked));
}
unsigned int PropertyCalculator::calculateHuJiaJianMianLv(int fightorLevel,float hujiazhi) const
{
	// ���׼���=����ֵ/������ֵ+5*���ֵȼ����� ��ע���ಲ������=����ֵ1��
	unsigned int huJiaJianMian = hujiazhi/(hujiazhi + 5.0f * fightorLevel) * 100.0f;
	if (huJiaJianMian >= 100)
		huJiaJianMian = 99;
	return huJiaJianMian;
}



// ��������ĸ�ֵ
float PropertyCalculator::calculateWuLiGeDangZhi(const CalculateData& beAttacked) const
{
	// include����ӵ�
	float gedangzhi_include_add = 0.0f;
	for (int i=0, include_count = beAttacked.includeCalDatas.size(); i<include_count; i++)
	{
		gedangzhi_include_add += calculateWuLiGeDangZhi(beAttacked.includeCalDatas[i]);
	}
	// ����һ��û��include������
	CalculateData flat_data = beAttacked;
	flat_data.includeCalDatas.clear();



	float currentGeDangZhi = 0.0f;
	{
		currentGeDangZhi = euipCal->getEquipGeDangZhi(flat_data.allEquips);

		// �����ǿ�������ӳ�
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				unsigned int addValue = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(flat_data.allEquips,GM_GeDangZhiAdd);
				currentGeDangZhi += addValue;
			}
		}


		currentGeDangZhi *= (flat_data.base_pro.effect_convert_per/100.0f); // ת����
	}


	currentGeDangZhi += gedangzhi_include_add;

	
	return currentGeDangZhi;
}
// �������
unsigned int PropertyCalculator::calculateGeDangLv(const CalculateData& beAttacked) const
{
	// include����ӵ�
	unsigned int gedanglv_include_add = 0;
	for (int i=0, include_count = beAttacked.includeCalDatas.size(); i<include_count; i++)
	{
		gedanglv_include_add += calculateGeDangLv(beAttacked.includeCalDatas[i]);
	}
	// ����һ��û��include������
	CalculateData flat_data = beAttacked;
	flat_data.includeCalDatas.clear();


	unsigned int currentGeDangLv = 0;
	{
		currentGeDangLv = euipCal->getEquipGeDangLv(flat_data.allEquips);
		currentGeDangLv *= (flat_data.base_pro.effect_convert_per/100.0f); // Ч������
	}

	currentGeDangLv += gedanglv_include_add;

	return currentGeDangLv;
}
// ���㿹��ֵ
float PropertyCalculator::calculateKuangXingZhi(const CalculateData& beAttacked) const
{
	// include����ӵ�
	float kangxinzhi_include_add = 0;
	for (int i=0, include_count = beAttacked.includeCalDatas.size(); i<include_count; i++)
	{
		kangxinzhi_include_add += calculateKuangXingZhi(beAttacked.includeCalDatas[i]);
	}
	// ����һ��û��include������
	CalculateData flat_data = beAttacked;
	flat_data.includeCalDatas.clear();



	float ret = 0.0f;
	{
		// 1����=0.5����
		float kuangXingZhi = ((float)flat_data.base_pro.zhili) * 0.5f;
		kuangXingZhi += euipCal->getEquipKangXingZhi(flat_data.allEquips);
		ret = kuangXingZhi;

		// �����ɫ�Ľ׵�Ѫ���ӳɣ�Ԫ��Ľײ��ܼӳɣ�ֻ���ڵȼ��������Ŀ���
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
// ���㿹�Լ�����,����ħ������
unsigned int PropertyCalculator::calculateKangXingMianLv(int fightLevel,const CalculateData& beAttacked) const
{
	// ��������׼���һ����ֻ��Ҫһ������ֵ������Ͳ��ϳ���,��������

	return calculateKangXingMianLv(fightLevel,calculateKuangXingZhi(beAttacked));
}
unsigned int PropertyCalculator::calculateKangXingMianLv(int fightLevel,float kangxingzhi) const
{
	// ���Լ���=����ֵ/������ֵ+10*���ֵȼ���
	unsigned int kuangXingJianMian = kangxingzhi / (float)(kangxingzhi + 10.0f * fightLevel) * 100;
	return kuangXingJianMian;
}


// ����һ����ɫ������һ����ɫ���������˺�ֵ,�õ��ľ�����ֵ
// ���ݽ�ɫ���ͣ��Զ�ȷ���������ǣ�ħ���˺�
// ֻ�ǹ������˺�ֵ��û�м��ܣ�Ҫ���㱩����������������ϼ��㼴��
// �ۺ��˹����������׼��⣬���Լ��⣬��ֵ������ֱ����Ҫ��ȥ��ֵ
float PropertyCalculator::calculateAttackShangHaiValue(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	// ���棺
	// ���ﲻ�ܰ���
	// �򵥵��ԭ����ǳ�����2��CalculateData��û����������Ҫ�������ƺ�ֻ��һ��CalculateData�ĵط�ȥ����
	// ����Ͳ�������
	// ��ʵ������������ν����Ϊ�ײ㶼�ǰ��������˵ġ�
	// �ϲ�������²�������������������������������
	// �ڶ��㿴�������������һ�������С��

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
		// ����
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
	// include����ӵ�
	float shanghai_include_add = 0;
	for (int i=0, include_count = attack.includeCalDatas.size(); i<include_count; i++)
	{
		shanghai_include_add += calculateAttackShangHaiValue(attack.includeCalDatas[i],beAttacked,range);
	}
	// ����һ��û��include������
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
				beAttacked->getHuJiaJianMian(), // ����Ļ��׼�������ָ����
				0,0,
				buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
		}
		else if (shanghai_leixing == SHANG_HAI_MO_FA)
		{
			ret = calculateMoFaShangHai(
				calculateGongJiLi(flat_data,range),
				beAttacked->getHuJiaJianMian(),// ���׼�����ָ����
				beAttacked->getKangXingJianMian(),// ���Լ�����ָ����
				buffCal->getBuffJianShangLv(beAttacked->getCurrentAttachedBuff()));
		}
		// ����
		else
		{
			CCLog("ValueCalculator::calculateAttackValue --> mShangHaiLeiXing != SHANG_HAI_MO_FA && mShangHaiLeiXing != SHANG_HAI_WU_LI");
			CCAssert(false,"");
		}

		//// ת����
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

	// ע�⣺
	// ���﹥������Ҫֵ���ָ��������ָ���˾�û����

	// ���ﲻ�ܼ򵥵İ�����
	// �����ֱ�Ӱ�����ӿ϶��ǲ��Եģ���Ϊ������beAttackedԽ�࣬�˺�Խ��
	// 
	// �����ֱ�ӷ�������������buff��������û���㵽beAttacked��include��buff�������
	// �ⲻ�����У�����Ϊinclude�����ݲ�������ս�ж�̬�仯��Ҳ����buffһֱ�ǿյģ���

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
	// ����
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

	// ע�⣺
	// ���﹥������Ҫֵ���ָ��������ָ���˾�û����
	// 
	// ����֮�䣬���ð����� û��CalculatData���ð���

	float ret = 0;

	if (shanghai_leixing == SHANG_HAI_WU_LI)
	{
		ret = calculateWuLiShangHai(
			attack->getGongJiLi(range),
			beAttacked->getHuJiaJianMian(),
			0,0, // ����û�и�
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
	// ����
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

	// ע�⣺
	// ���﹥������Ҫֵ���ָ��������ָ���˾�û����

	// û��CalculatData���ð���

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
	// ����
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
		CCAssert(false,"��֧�ֵ�ֵ������");
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
	// ����
	else
	{
		CCLog("ValueCalculator::calculateAttackValue --> mShangHaiLeiXing != SHANG_HAI_MO_FA && mShangHaiLeiXing != SHANG_HAI_WU_LI");
		CCAssert(false,"");
	}

	if (ret <= 0)
		ret = 1;

	return ret;
}



// ����actor��������
unsigned int  PropertyCalculator::calculateShanBiLv(const CalculateData& actor) const
{
	// include����ӵ�
	unsigned int shanbilv_include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		shanbilv_include_add += calculateShanBiLv(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	int ret = 0;
	{

		//2014-02-24ȥ�����ݵ����ܵ�ת��
		float shanbilv = 0;

		//2014-02-24��ְҵ�趨��ʼ��������
		// �̿͡���Ϸ�����֡�����ֵ����ܸ�Ϊ����10%����������ְҵ������ȫ��Ϊ0
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

		//2014-02-24ȥ�����ݵ����ܵ�ת��
		////0~100�����ݵ㣬ÿ���ݵ�ת��Ϊ0.1%���ܣ� 
		////101~250�����ݵ㣬ÿ���ݵ�ת��Ϊ0.05%���ܣ�
		////251~500�����ݵ㣬ÿ���ݵ�ת��Ϊ0.03%���ܣ�
		////501���ϵ����ݵ㣬ÿ���ݵ�ת��Ϊ0.01%���ܡ�
		//float mingjie = calculateMinJie(flat_data);
		//float shanbilv = 0;
		//if (mingjie > 500)
		//{
		//	shanbilv += (mingjie-500.0f)*0.0001f;//501���ϵ����ݵ㣬ÿ���ݵ�ת��Ϊ0.01%���ܡ�
		//}
		//if (mingjie > 250)
		//{
		//	shanbilv += ( min(mingjie,500.0f) - 250)*0.0003f;//251~500�����ݵ㣬ÿ���ݵ�ת��Ϊ0.03%���ܣ�
		//}
		//if (mingjie > 100)
		//{
		//	shanbilv += ( min(mingjie,250.0f) - 100)*0.0005f;//101~250�����ݵ㣬ÿ���ݵ�ת��Ϊ0.05%���ܣ�
		//}
		//if (mingjie > 0)
		//{
		//	shanbilv += ( min(mingjie,100.0f) - 0)*0.003f;//0~100�����ݵ㣬ÿ���ݵ�ת��Ϊ0.1%���ܣ�
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
// ���㱩����
unsigned int PropertyCalculator::calculateBaoJiLv(const CalculateData& actor) const
{
	// include����ӵ�
	unsigned int baojilv_include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		baojilv_include_add += calculateBaoJiLv(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
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

	//// include����ӵ�
	//unsigned int include_add = 0;
	//for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	//{
	//	include_add += calculateJiNengChuFaLv(actor.includeCalDatas[i]);
	//}
	//// ����һ��û��include������
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
	CCAssert(actor.base_pro.skill_type != 0,"�м��ܲ��ܵ���");
#if ALWAYS_USE_SKILL == 1
	return 1;
#endif

	// ����Ƚ�����
	// ������һ�¾�����
	// ��Ҫ��������װ�����ٵļ�������

	unsigned int equip_sub_jineng_zhouqi = 0;
	{
		for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
		{
			equip_sub_jineng_zhouqi += euipCal->getSkillZhouQiSub(actor.includeCalDatas[i].allEquips);

			// ��֧��include��data���滹include������
			CCAssert(actor.includeCalDatas[i].includeCalDatas.empty(),"����ط�û���õݹ飬���оͼ򵥵�ֻ֧��һ���Ƕ�ף�");
		}

		equip_sub_jineng_zhouqi += euipCal->getSkillZhouQiSub(actor.allEquips);
	}

	int ret = CS::getSkillZhouQi(actor.base_pro.skill_type);
	ret -= equip_sub_jineng_zhouqi;
	if (ret < 2) // ����������2
		ret = 2;
	return ret;
}
unsigned int PropertyCalculator::calculateJiNengTurn(const CalculateData& actor) const
{
	CCAssert(actor.base_pro.skill_type != 0,"�м��ܲ��ܵ���");
#if ALWAYS_USE_SKILL == 1
	return 1;
#endif

	// �����calculateJiNengZhouQiһ����������Ҫ��������
	// ��Ҫ�Լ���һ��
	unsigned int skill_turn_sub = 0;
	{
		skill_turn_sub += euipCal->getSkillTurnSub(actor.allEquips);
		for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
		{
			skill_turn_sub += euipCal->getSkillTurnSub(actor.includeCalDatas[i].allEquips);

			// ��֧��include��data���滹include������
			CCAssert(actor.includeCalDatas[i].includeCalDatas.empty(),"����ط�û���õݹ飬���оͼ򵥵�ֻ֧��һ���Ƕ�ף�");
		}
	}


	int ret = CS::getSkillTurn(actor.base_pro.skill_type);
	// ���㵱ǰ���������
	int zhouqi = calculateJiNengZhouQi(actor);
	if (ret > zhouqi) // ��ѹ�ƺô����㣬�ٽ��д����������
		ret = zhouqi;

	ret -= skill_turn_sub;
	if (ret < 1)	// ���������ֻ���ǵ�һ�ξʹ���
		ret = 1;
	return ret;
}
unsigned int PropertyCalculator::calculateJiNengResult(const CalculateData& actor) const
{
	CCAssert(actor.base_pro.skill_type != 0,"�м��ܲ��ܵ���");
	// ����Ҳ���ܵݹ�򵥵İ���
	// ��Ϊint ret = 100;�����ˣ���ôÿ�ζ���ӳ�100%
	// ���л��ǵõ���д
	// �����calculateJiNengZhouQiһ����������Ҫ��������
	// ��Ҫ�Լ���һ��
	/*
	float skill_ration_add = 0;
	{
		skill_ration_add += euipCal->getSkillRatioAdd(actor.allEquips);
		for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
		{
			skill_ration_add += euipCal->getSkillRatioAdd(actor.includeCalDatas[i].allEquips);

			// ��֧��include��data���滹include������
			CCAssert(actor.includeCalDatas[i].includeCalDatas.empty(),"����ط�û���õݹ飬���оͼ򵥵�ֻ֧��һ���Ƕ�ף�");
		}
	}
	*/

    /* 2014.07.03 ���Ӽ����˺��ӳ� */
	float skill_ration_add = 0;
	{
		skill_ration_add += euipCal->getSkillRatioAdd(actor.allEquips);
		for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
		{
			skill_ration_add += euipCal->getSkillResultAddPercent(actor.includeCalDatas[i].allEquips);

			// ��֧��include��data���滹include������
			CCAssert(actor.includeCalDatas[i].includeCalDatas.empty(),"����ط�û���õݹ飬���оͼ򵥵�ֻ֧��һ���Ƕ�ף�");
		}
	}


	int ret = 100; // ��100%��ʼ�Ĭ�Ͼ���1.0f
	ret += skill_ration_add;
	// �ȼ������ļӳ�
	ret += CS::getSkillEffectRatioAddBySkillLevel(actor.base_pro.skill_level);
	return ret;
}

// ����attack����beAttacked�ճɵ������˺�
float PropertyCalculator::calculateWuLiShangHai(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	// ����
	// ������2��CalculateData��û��������ֱ�ӽ�����һ��
	// buff�Ļ����ùܣ���ΪincludeData��buff������

	// �����˺�
	// �����˺�=����������1-���׼��⣩-��ֵ
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
	// �����˺�
	// �����˺�=����������1-���׼��⣩-��ֵ
	float gongJiLi = gongjiLi;
	float huJiaJianMian = ((float)huJiaJianMianLv)/100.0f;
	float geDangZhi = gedangzhi;
	float geDangLv = ((float)gedanglv)/100.0f;

	if (lfRandom0_1() > geDangLv)
		geDangZhi = 0;

	float ret = gongJiLi*(1-huJiaJianMian) - geDangZhi;
	// ����
	float jianshang_lv = ((float)jiansahnglv)/100.0f;
	ret = ret - ret*jianshang_lv;

	if (ret <= 1)
	{
		ret = 1;
	}

	return ret;
}

// ����attack����beAttacked�ճɵ�ħ���˺�
float PropertyCalculator::calculateMoFaShangHai(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range /*= Range_Ramdom*/) const
{
	// ����
	// ������2��CalculateData��û��������ֱ�ӽ�����һ��
	// buff�Ļ����ùܣ���ΪincludeData��buff������

	float ret = calculateMoFaShangHai(
		calculateGongJiLi(attack,range),
		calculateHuJiaJianMianLv(attack.base_pro.level,beAttacked),
		calculateKangXingMianLv(attack.base_pro.level,beAttacked),
		buffCal->getBuffJianShangLv(beAttacked.allBuffs));

	return ret;
}
// ����attack����beAttacked�ճɵ�ħ���˺�
float PropertyCalculator::calculateMoFaShangHai(float gongjiLi,int huJiaJianMianLv,int kangxingJianMianLv,int jiansahnglv) const
{
	// ħ���˺�
	// ħ���˺�=������*��1-���׼��⣩*��1-���Լ��⣩
	float gongJiLi = gongjiLi;
	float kuangXingJianMian = ((float)kangxingJianMianLv) / 100.0f;
	float hujiaJianMian = huJiaJianMianLv / 100.0f;

	float ret = gongJiLi * (1- hujiaJianMian) * (1 - kuangXingJianMian);
	// ����
	float jianshang_lv = ((float)jiansahnglv)/100.0f;
	ret = ret - ret*jianshang_lv;

	if (ret <= 0)
	{
		ret = 1;
	}
	return ret;
}

// ��������
unsigned int PropertyCalculator::calculateBaoJiShangHaiYinzi(const CalculateData& actor) const
{
	// include����ӵ�
	unsigned int include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateBaoJiShangHaiYinzi(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	unsigned int ret = 0;
	{
		ret = flat_data.base_pro.baoshang_yinzi;
		ret += euipCal->getEquipBaoshangYinZi(flat_data.allEquips);

		// �����ǿ�������ӳ�
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
	// include����ӵ�
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateLiliang(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
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
	// include����ӵ�
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateMinJie(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
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
	// include����ӵ�
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateZhili(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
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
	// include����ӵ�
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateTineng(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
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
	// include����ӵ�
	float include_add = 0;
	for (int i=0, include_count = actor.includeCalDatas.size(); i<include_count; i++)
	{
		include_add += calculateHp(actor.includeCalDatas[i]);
	}
	// ����һ��û��include������
	CalculateData flat_data = actor;
	flat_data.includeCalDatas.clear();


	float ret = 0.0f;
	{
		// ---> ��Ҫ��--->Ѫ�������ܵĹ�ϵ��1����=1.1*(�ȼ�/5)  ��Ѫ����ע���������2����2�ƣ�
		// Ѫ�������ܵĹ�ϵ��1����=1 ��Ѫ��
		//Ѫ��= ����ʼѪ�� + (��������+װ����������)* (����ת��Ѫ����ʽ)�� * (1+װ�����Ӱٷֱ�Ѫֵ)

		float zb_fuJia_tiNeng = euipCal->getEquipTineng(flat_data.allEquips);
		float benShenTiNeng = flat_data.base_pro.tineng;
		float chuShiXueZhi = flat_data.base_pro.base_hp;
		float zhuangbeiFuJiaPercent = euipCal->getEquipHp_ratio(flat_data.allEquips)/100.0f;

		// Ѫ�������ܵĹ�ϵ��1����=1.1*(�ȼ�/5)  ��Ѫ����ע���������2����2�ƣ�
		//float tineng_hp = (benShenTiNeng + zb_fuJia_tiNeng) * 1.1f * (flat_data.base_pro.level/5.0f);
		//if (tineng_hp < 2)
		//	tineng_hp = 2;

		// Ѫ�������ܵĹ�ϵ��1����=1 ��Ѫ��
		//stub 
		float tineng_hp = 7*(benShenTiNeng + zb_fuJia_tiNeng);

		ret  = (chuShiXueZhi + tineng_hp) * (1 + zhuangbeiFuJiaPercent);


		// ְҵѪ���ӳ�
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

		// �����ɫ�Ľ׵�Ѫ���ӳɣ�Ԫ��Ľײ��ܼӳɣ�ֻ���ڵȼ��������Ŀ���
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float bloodPer = ((float)CS::getHeroStageEffectRatio(flat_data.base_pro.stage))/100.0f;
				ret += ret * bloodPer;
			}
		}

		// �����ǿ�������ӳ�
		{
			if (!flat_data.base_pro.is_yuanshen)
			{
				float addPer = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(flat_data.allEquips,GM_HPAddPer);
				addPer = addPer/100.0f;
				ret += ret * addPer;
			}
		}

		
		// ����Ϳ�����percent��
		// ��Ϊǰ���õ�ÿһ�����Զ���û�м���percent��
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






// װ���ļ���
EquipCalculator::EquipCalculator()
{

}
// ������Щֵ���Ǹ��ܸ��ģ�װ����װ���Ļ���ֵ����Ҫ��������Ҳ���Լ���
// ���������Լ����Ҳ�ǣ��Լ������Լ��Ļ����Լ��Լ�������Ӱ�죬���ù������ط�������
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
			percent *= 0.01f;	// �����100�ֱ�
		}

		// ����û�д�����
		if (equipVec[i].zhuangbei_type == ZhuangbeiType_KuijiaQin || equipVec[i].zhuangbei_type == ZhuangbeiType_KuijiaZhong)
		{
			wareKuiJia = true;
		}

		ret += equipVec[i].hujia;			// ������
		ret += equipVec[i].hujia*percent;	// ǿ������
		ret += equipVec[i].getAtrriValueByType(AttrType_hujia_add);
	}

	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_hujia_add).attr_value; // ��װ�ӳ�
	
	// 2014-02-24ȥ�����������׵�ת��
	////1����=0.3����
	//ret += getEquipLiliang(equipVec) * 0.3f;

	//// ǿ����������Ҫ�ŵ����棬��ΪҪ�ж��Ƿ���Ԫ��
	//float addPer = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(equipVec,GM_HuJiaAddPer);
	//addPer = addPer/100.0f;
	//ret += ret * addPer;

	if (!wareKuiJia)
	{// û�����ף�������Ҫ�������4
		ret += 4.0f;
	}

	return ret;
}
float EquipCalculator::getEquipGeDangZhi(vector<EquipData> equipVec) const
{
	// ��ʱ��Ҫ��
	return 0;

	float ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_gedangzhi);
	}

	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_gedangzhi).attr_value; // ��װ�ӳ�
	return ret;
}
unsigned int EquipCalculator::getEquipGeDangLv(vector<EquipData> equipVec) const
{
	// ��ʱ��Ҫ��
	return 0;

	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_gedanglv);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_gedanglv).attr_value; // ��װ�ӳ�
	return ret;
}
float EquipCalculator::getEquipKangXingZhi(vector<EquipData> equipVec) const
{
	// ��ʱ��Ҫ��
	return 0;

	float ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_kangxing);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_kangxing).attr_value; // ��װ�ӳ�
	//1����=0.5����
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
			percent *= 0.01f; // �����100�ֱ�
		}
		gongji_min += equipVec[i].gongji_min;
		gongji_max += equipVec[i].gongji_max;
		gongji_min += equipVec[i].gongji_min * percent;
		gongji_max += equipVec[i].gongji_max * percent;

		gongji_min_add += equipVec[i].getAtrriValueByType(AttrType_gongji_min);
		gongji_max_add += equipVec[i].getAtrriValueByType(AttrType_gongji_max);
	}

	gongji_min_add += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_gongji_min).attr_value; // ��װ�ӳ�
	gongji_max_add += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_gongji_max).attr_value; // ��װ�ӳ�

}

float EquipCalculator::getEquipLiliang(vector<EquipData> equipVec) const
{
	float ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_liliang);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_liliang).attr_value; // ��װ�ӳ�
	// ��ʯ
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
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_mingjie).attr_value; // ��װ�ӳ�
	// ��ʯ
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
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_zhili).attr_value; // ��װ�ӳ�
	// ��ʯ
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
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_tineng).attr_value; // ��װ�ӳ�

	// ��ʯ
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
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_baoji).attr_value; // ��װ�ӳ�
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
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_baoshang).attr_value; // ��װ�ӳ�
	return ret;
}

unsigned int EquipCalculator::getEquipGongJiAdd_ratio(vector<EquipData> equipVec) const
{
	//// ǿ������ Ӧ�÷������棬���һ�Ҫ�ж����Ƿ���Ԫ��
	//return ZhuangbeiGongMingManager::getManager()->getPropertyAdd(equipVec,GM_GongJiAddPer);

	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_actor_gongji_addratio);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_actor_gongji_addratio).attr_value; // ��װ�ӳ�
	return ret;
}

unsigned int EquipCalculator::getEquipHp_ratio(vector<EquipData> equipVec) const
{
	//// ǿ������ Ӧ�÷������棬���һ�Ҫ�ж����Ƿ���Ԫ��
	//return ZhuangbeiGongMingManager::getManager()->getPropertyAdd(equipVec,GM_HPAddPer);
	
	// 1.��ʱ��ҪѪ���ӳ���
	// 2.����ҪѪ���ӳ��ˣ���Ϊ��װ���������������AttrType_actor_hp_addratio
	// 
	unsigned int ret = 0;
	int num = equipVec.size();
	for (int i=0;i<num;i++)
	{
		ret += equipVec[i].getAtrriValueByType(AttrType_hp_percent);
		// AttrType_actor_hp_addratio��AttrType_hp_percentһ��
		// �ͷ���һ������
		ret += equipVec[i].getAtrriValueByType(AttrType_actor_hp_addratio);
	}
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_hp_percent).attr_value; // ��װ�ӳ�
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_actor_hp_addratio).attr_value; // ��װ�ӳ�
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
	//ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_skillratio).attr_value; // ��װ�ӳ�
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
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_skillzhouqi).attr_value; // ��װ�ӳ�
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
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_skillturn).attr_value; // ��װ�ӳ�
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
	ret += SuitPool::getPool()->getSuitZBAttriByEquipAndAttrType(equipVec,AttrType_skillresult).attr_value; // ��װ�ӳ�
	return ret;
}


// ��ʯ�ӳ�
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





// buff��ʵʱ����
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

