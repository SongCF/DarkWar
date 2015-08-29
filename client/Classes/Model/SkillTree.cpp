
#include "SkillTree.h"
#include "SkillConfigManager.h"
#include "protocol.h"
#include "LFStrings.h"
#include "BaseActorPropertyParser.h"

SkillTree* SkillTree::s_skillTree = NULL;

SkillTree * SkillTree::sharedSkillTree()
{
	if (s_skillTree == NULL)
	{
		s_skillTree = new SkillTree;
	}

	return s_skillTree;
}

string SkillTree::getSkillNameByType(SkillType type)
{
	for (int i=0; i!=mAllSkillVec.size(); ++i)
	{
		if(type == mAllSkillVec.at(i).skill_type)
			return mAllSkillVec.at(i).name;
	}

	return "";
}

string SkillTree::getSkillNameAddInfo(SkillType type, bool simple)
{
	string str;

	for (int i=0; i!=mAllSkillVec.size(); ++i)
	{
		if(type == mAllSkillVec.at(i).skill_type)
		{
			if (simple)
			{
				str = BaseActorPropertyParser::getParser()->getExtraProerty(mAllSkillVec.at(i).actor_profession).nameInChinese + "-";
				str += mAllSkillVec.at(i).name;
			}
			else 
			{
				str = BaseActorPropertyParser::getParser()->getExtraProerty(mAllSkillVec.at(i).actor_profession).nameInChinese + " ";
				str += LFStrings::getValue("jinengshu") + ":";
				str += mAllSkillVec.at(i).name;
			}
		}
	}

	return str;
}

string SkillTree::getSkillDescribeByType(SkillType type)
{
	for (int i=0; i!=mAllSkillVec.size(); ++i)
	{
		if(type == mAllSkillVec.at(i).skill_type)
			return mAllSkillVec.at(i).describe;
	}

	return "";
}

int SkillTree::getSkillLevelByType(SkillType type)
{
	int level = 0;
	switch (type)
	{
	case SkillType_BaoLieZhan:
	case SkillType_ShaLu:
	case SkillType_ShanDian:
	case SkillType_JianShou:
	case SkillType_ShenZhiXingNian:
	case SkillType_LiRenHuiXuan:
	case SkillType_LieGong:
	case SkillType_JianYu:
	case SkillType_HuoQiuShu:
	case SkillType_WenYi:
		level = 1;
		break;
	case SkillType_BanYueZhan:
	case SkillType_CiSha:
	case SkillType_LeiBao:
		level = 10;
		break;
	case SkillType_MangCi:
	case SkillType_ShenZhiZhuFu:
	case SkillType_ShunYinTuXi:
	case SkillType_JingZhunZhiCan:
	case SkillType_JiSuDuJian:
	case SkillType_BaoFengXue:
	case SkillType_KongJu:
		level = 15;
		break;
	case SkillType_KuangBao:
	case SkillType_ShiXueZhiRen:
	case SkillType_YunShi:
		level = 20;
		break;
	case SkillType_ShouHu:
	case SkillType_ZhongShenZhiLi:
	case SkillType_XuanFengZhan:
	case SkillType_ZhuiHunDuJian:
	case SkillType_SiWangSaoShe:
	case SkillType_HanBingCiGu:
	case SkillType_ShiHun:
		level = 30;
		break;
	case SkillType_BaoNu:
	case SkillType_YinZheHun:
	case SkillType_TianTangZhiGuang:
		level = 30;
		break;

	case SkillType_Monster_XieE:
		level = 1;
		break;
	default:
		CCAssert(false, "");
		break;
	}

	return level;
}

SkillItem SkillTree::getSkillInfor(SkillType type)
{
	for (int i=0; i!=mAllSkillVec.size(); ++i)
	{
		if(type == mAllSkillVec.at(i).skill_type)
			return mAllSkillVec[i];
	}
	CCAssert(false,"没找到");
	SkillItem null_item;
	return null_item;
}

SkillType SkillTree::itemsType2SkillType(int itemType)
{
	return (SkillType) (itemType - ItemsType_SkillBook_MinId);
}
ItemsType SkillTree::skillType2ItemsType(int skillType)
{
	return(ItemsType) (skillType + ItemsType_SkillBook_MinId);
}

string SkillTree::getSkillPlistFile(SkillType type,string extrenName /* = ".plist" */)
{
	string file = "";
	switch (type)
	{
	case SkillType_BaoLieZhan:
		file = "jineng_yemanren_baolie"+extrenName;
		break;
	case SkillType_ShaLu:
		file = "jineng_cike_shalu"+extrenName;
		break;
	case SkillType_ShanDian:
		file = "jineng_jisi_shandian"+extrenName;
		break;
	case SkillType_JianShou:
		file = "jineng_qishi_shengcai"+extrenName;
		break;
	case SkillType_ShenZhiXingNian:
		// 暂时用神之祝福的效果
		file = "jineng_magebane_shenzhizhufu"+extrenName;
		break;
	case SkillType_ShunYinTuXi:
		file = "jineng_youxia_shunyingtuxi"+extrenName;
		break;
	case SkillType_LieGong:
		file = "jineng_gongjianshou_huijinzhiyu"+extrenName;
		break;
	case SkillType_JianYu:
		file = "jineng_huntsman_jianyu"+extrenName;
		break;
	case SkillType_HuoQiuShu:
		file = "jineng_mishushi_ronghuachongji"+extrenName;
		break;
	case SkillType_WenYi:
		file = "jineng_wushi_linghundiaoling"+extrenName;
		break;
	case SkillType_BanYueZhan:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_CiSha:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_LeiBao:
		file = "jineng_jisi_leibao"+extrenName;
		break;
	case SkillType_MangCi:
		file = "jineng_qishi_mangci"+extrenName;
		break;
	case SkillType_ShenZhiZhuFu:
		file = "jineng_magebane_shenzhizhufu"+extrenName;
		break;
	case SkillType_LiRenHuiXuan:
		file = "jineng_youxia_lirenhuixuan"+extrenName;
		break;
	case SkillType_JingZhunZhiCan:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_JiSuDuJian:
		file = "jineng_lieshou_jishudujian"+extrenName;
		break;
	case SkillType_BaoFengXue:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_KongJu:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_KuangBao:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_ShiXueZhiRen:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_YunShi:
		file = "jineng_jisi_yunshi"+extrenName;
		break;
	case SkillType_ShouHu:
		file = "jineng_qishi_shouhu"+extrenName;
		break;
	case SkillType_ZhongShenZhiLi:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_XuanFengZhan:
		file = "jineng_youxia_xuanfengzhan"+extrenName;
		break;
	case SkillType_ZhuiHunDuJian:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_SiWangSaoShe:
		file = "jineng_lieshou_siwangsaoshe"+extrenName;
		break;
	case SkillType_HanBingCiGu:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_ShiHun:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_BaoNu:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_YinZheHun:
		CCAssert(false,"还没有");
		file = "jineng_yemanren_baoliezhan"+extrenName;
		break;
	case SkillType_TianTangZhiGuang:
		file = "jineng_fashi_tiantangzhiguang"+extrenName;
		break;
	case SkillType_Monster_XieE:
		CCAssert(false,"还没有");
		file = "jineng_monster_xie_e"+extrenName;
		break;
	default:
		CCAssert(false,"未知技能");
		break;
	}

	return file;
}

SkillTree::SkillTree()
{
	mAllSkillVec.clear();

	SkillConfigManager *skillConfigManager = SkillConfigManager::getManager();

	SkillItem item;

	{// 爆裂斩  1
		item.skill_type = SkillType_BaoLieZhan;
		item.actor_profession = Profession_Yemanren;
		item.level = getSkillLevelByType(item.skill_type);
		//item.name = LFStrings::getValue("skill_YeManRen_TianFu");
		//item.describe = LFStrings::getValue("skill_YeManRen_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//              = 2,			// 杀戮
		item.skill_type = SkillType_ShaLu;
		item.actor_profession = Profession_Chike;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_CiKe_TianFu");
// 		item.describe = LFStrings::getValue("skill_CiKe_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	}
	{//            = 3,			// 闪电	
		item.skill_type = SkillType_ShanDian;
		item.actor_profession = Profession_Fashi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_FaShi_TianFu");
// 		item.describe = LFStrings::getValue("skill_FaShi_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//            = 4,			// 坚守
		item.skill_type = SkillType_JianShou;
		item.actor_profession = Profession_Qishi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_QiShi_TianFu");
// 		item.describe = LFStrings::getValue("skill_QiShi_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//     = 5,	// 神之信念
		item.skill_type = SkillType_ShenZhiXingNian;
		item.actor_profession = Profession_WuSeng;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_WuSeng_TianFu");
// 		item.describe = LFStrings::getValue("skill_WuSeng_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//         = 6,		// 瞬影突袭
		item.skill_type = SkillType_ShunYinTuXi;
		item.actor_profession = Profession_YouXia;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_YouXia_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_YouXia_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//             = 7,			// 烈弓
		item.skill_type = SkillType_LieGong;
		item.actor_profession = Profession_ShenJianShou;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_ShenJianShou_TianFu");
// 		item.describe = LFStrings::getValue("skill_ShenJianShou_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//              = 8,			// 剑雨
		item.skill_type = SkillType_JianYu;
		item.actor_profession = Profession_LieShou;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_LieShou_TianFu");
// 		item.describe = LFStrings::getValue("skill_LieShou_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//           = 9,		// 火球术
		item.skill_type = SkillType_HuoQiuShu;
		item.actor_profession = Profession_MiShu;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_MiShuShi_TianFu");
// 		item.describe = LFStrings::getValue("skill_MiShuShi_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//               = 10,			// 瘟疫	
		item.skill_type = SkillType_WenYi;
		item.actor_profession = Profession_WuShi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_WuShi_TianFu");
// 		item.describe = LFStrings::getValue("skill_WuShi_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//          = 11,
		item.skill_type = SkillType_BanYueZhan;
		item.actor_profession = Profession_Yemanren;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_YeManRen_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_YeManRen_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//               = 12,
		item.skill_type = SkillType_CiSha;
		item.actor_profession = Profession_Chike;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_CiKe_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_CiKe_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//              = 13,
		item.skill_type = SkillType_LeiBao;
		item.actor_profession = Profession_Fashi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_FaShi_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_FaShi_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//              = 14,
		item.skill_type = SkillType_MangCi;
		item.actor_profession = Profession_Qishi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_QiShi_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_QiShi_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//        = 15,
		item.skill_type = SkillType_ShenZhiZhuFu;
		item.actor_profession = Profession_WuSeng;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_WuSeng_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_WuSeng_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//        = 16,
		item.skill_type = SkillType_LiRenHuiXuan;
		item.actor_profession = Profession_YouXia;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_YouXia_TianFu");
// 		item.describe = LFStrings::getValue("skill_YouXia_TianFu_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//      = 17,
		item.skill_type = SkillType_JingZhunZhiCan;
		item.actor_profession = Profession_ShenJianShou;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_ShenJianShou_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_ShenJianShou_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//          = 18,
		item.skill_type = SkillType_JiSuDuJian;
		item.actor_profession = Profession_LieShou;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_LieShou_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_LieShou_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//          = 19,
		item.skill_type = SkillType_BaoFengXue;
		item.actor_profession = Profession_MiShu;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_MiShuShi_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_MiShuShi_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//              = 20,
		item.skill_type = SkillType_KongJu;
		item.actor_profession = Profession_WuShi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_WuShi_JiNeng1");
// 		item.describe = LFStrings::getValue("skill_WuShi_JiNeng1_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//            = 21,
		item.skill_type = SkillType_KuangBao;
		item.actor_profession = Profession_Yemanren;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_YeManRen_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_YeManRen_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 	
	{//        = 22,
		item.skill_type = SkillType_ShiXueZhiRen;
		item.actor_profession = Profession_Chike;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_CiKe_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_CiKe_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//              = 23,
		item.skill_type = SkillType_YunShi;
		item.actor_profession = Profession_Fashi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_FaShi_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_FaShi_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//              = 24,
		item.skill_type = SkillType_ShouHu;
		item.actor_profession = Profession_Qishi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_QiShi_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_QiShi_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//      = 25,
		item.skill_type = SkillType_ZhongShenZhiLi;
		item.actor_profession = Profession_WuSeng;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_WuSeng_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_WuSeng_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//        = 26,
		item.skill_type = SkillType_XuanFengZhan;
		item.actor_profession = Profession_YouXia;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_YouXia_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_YouXia_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//       = 27,
		item.skill_type = SkillType_ZhuiHunDuJian;
		item.actor_profession = Profession_ShenJianShou;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_ShenJianShou_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_ShenJianShou_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//        = 28,
		item.skill_type = SkillType_SiWangSaoShe;
		item.actor_profession = Profession_LieShou;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_LieShou_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_LieShou_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//        = 29,
		item.skill_type = SkillType_HanBingCiGu;
		item.actor_profession = Profession_MiShu;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_MiShuShi_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_MiShuShi_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//              = 30,
		item.skill_type = SkillType_ShiHun;
		item.actor_profession = Profession_WuShi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_WuShi_JiNeng2");
// 		item.describe = LFStrings::getValue("skill_WuShi_JiNeng2_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//               = 31,
		item.skill_type = SkillType_BaoNu;
		item.actor_profession = Profession_Yemanren;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_YeManRen_JiNeng3");
// 		item.describe = LFStrings::getValue("skill_YeManRen_JiNeng3_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//           = 32,
		item.skill_type = SkillType_YinZheHun;
		item.actor_profession = Profession_Chike;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_CiKe_JiNeng3");
// 		item.describe = LFStrings::getValue("skill_CiKe_JiNeng3_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
	{//    = 33,
		item.skill_type = SkillType_TianTangZhiGuang;
		item.actor_profession = Profession_Fashi;
		item.level = getSkillLevelByType(item.skill_type);
// 		item.name = LFStrings::getValue("skill_FaShi_JiNeng3");
// 		item.describe = LFStrings::getValue("skill_FaShi_JiNeng3_dsc");
		item.name = skillConfigManager->getSkillName(item.skill_type);
		item.describe = skillConfigManager->getDescribe(item.skill_type);
		mAllSkillVec.push_back(item);
	} 
}
