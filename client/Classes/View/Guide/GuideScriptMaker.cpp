#include "GuideScriptMaker.h"
#include "GameDirector.h"
#include "CS_Common.h"
#include "BaseActorPropertyParser.h"
#include "FightDirector.h"
#include "ClientFightLayer.h"
#include "LFFileUtiles.h"
#include "FightScriptUserData.h"

#define YEMANREN_FIGHT_FILE				"config/yemanren_fight1.bin"
#define YEMANREN_YOUXIA_FIGHT_FILE		"config/yemanren_fight2.bin"
#define CIKE_FIGHT_FILE					"config/cike_fight1.bin"
#define CIKE_YOUXIA_FIGHT_FILE			"config/cike_fight2.bin"
#define FASHI_FIGHT_FILE				"config/fashi_fight1.bin"
#define FASHI_QISHI_FIGHT_FILE			"config/fashi_fight2.bin"

GuideScriptMaker::GuideScriptMaker(void)
{
}

class BaseDataProvider : public ScriptDataProvider
{
public:

	// 下面的都可以不用管
	virtual void getTeamBloodNum(short& currentNum,short& defaultNum,int& bloodAddPercent,ScriptBuilderUserData data)
	{
		currentNum = 0;
		defaultNum = 0;
		bloodAddPercent = 0;
	}
	virtual void getHeroCDTime(short& cdStepNum,unsigned heroId,ScriptBuilderUserData data)
	{
		cdStepNum = 0;
	}
	virtual void getHeroDetail(HeroDetail& outData,unsigned int _heroId,ScriptBuilderUserData data)
	{
	}
	virtual unsigned int fightEndCallback(ScriptBuilderUserData data,bool win)
	{
		return 0;
	}
	virtual vector<ItemSuipianInfo> getFightEndItemSuiPians(ScriptBuilderUserData data,bool win)
	{
		vector<ItemSuipianInfo> ret;
		return ret;
	}
	virtual vector<ShenbingSuipianInfo> getFightEndShenBingSuiPians(ScriptBuilderUserData data,bool win)
	{
		vector<ShenbingSuipianInfo> ret;
		return ret;
	}
	virtual vector<EquipData> getFightEndEquips(ScriptBuilderUserData data,bool win)
	{
		vector<EquipData> ret;
		return ret;
	}
	virtual vector<ItemInfo> getFightEndItems(ScriptBuilderUserData data,bool win)
	{
		vector<ItemInfo> ret;
		return ret;
	}
	virtual void teamFightEnd(bool win,TeamEndParam parm,TeamEndOutParam& gettedItems)
	{
	}
};

class YemanRenProvider : public BaseDataProvider
{
private:
	bool onePerson;
public:
	YemanRenProvider(bool onePerson)
	{
		this->onePerson = onePerson;
	}
	virtual BaseHero* getBaseHero(unsigned int _heroId,ScriptBuilderUserData user_data)
	{
		float gongJiJuLi_A[] = {1,1,2,2};
		float gongJiJuLi_B[] = {1,1,2,2};

		int *zhiye_a = NULL;
		{
			int zhiye_a1[] = {Profession_Yemanren,Profession_YouXia,Profession_Yemanren,Profession_Yemanren};
			int zhiye_a2[] = {Profession_YouXia,Profession_Yemanren,Profession_Yemanren,Profession_Yemanren};
			if (onePerson)
			{
				zhiye_a = zhiye_a1;
			}
			else
			{
				zhiye_a = zhiye_a2;
			}
		}
		int zhiye_b[] = {69,69,68,68};

		// A 队
		if (user_data.teamTag > 0)
		{
			//
			Hero* ret = new Hero();
			// test
			{
				// 不用装备
				ret->equipVec.clear();
				ret->skilVec.clear();

				HeroBaseProperty data;
				data.hero_id= _heroId;
				data.profession = (Profession)zhiye_a[(_heroId-1)%4];
				data.level = 10;

				data.exp_current = 20;
				data.exp_need = 2000;
				data.parent_hero_id = 0;
				data.postion = 1;
				data.vit_current = 100;

				data.mLiLiang = 4;
				data.mMinJie = 2;
				data.mZhiLi = 6;
				data.mTiNeng = 0;
				data.vit_current = 200;
				data.mHP = 60;
				data.mGongJiJuLi = CS::getHeroGongjiJuLiByProfession(data.profession);
				data.mBaoJiLv = 0;
				data.mBaoJiShangHaiYinZi = 150;
				data.mJiNengFaChuLv = 0;
				data.mShangHaiLeiXing = BaseActorPropertyParser::getParser()->getExtraProerty(data.profession).mShangHaiLeiXing;
				data.currentStage = 1;
				data.currentEffectPercent = 100;
				data.isYuanShen = false;
				ret->init(data);
				ret->setDefaultCdTime(0);
				if (data.profession == Profession_YouXia)
					ret->skilVec.push_back(SkillType_LiRenHuiXuan);

				{
					// 加装备,测试
					EquipData zhuangbei;
					zhuangbei.gongji_max = 110;
					zhuangbei.gongji_min = 100;
					zhuangbei.hole = 3;
					zhuangbei.hujia = 2;
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipDicrib = "equipDicrib";
					zhuangbei.zhuangbei_hero_id = 1;
					zhuangbei.zhuangbei_id = 2;
					zhuangbei.zhuangbei_level = 3;
					zhuangbei.equipName = "equipName";
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_fu_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Hushenfu;

					if (data.profession == Profession_YouXia)
					{
						ZBAttr att;
						att.attr_type = AttrType_skillzhouqi;
						att.attr_value = 8; // 
						zhuangbei.attr_vec.push_back(att);

						att.attr_type = AttrType_skillturn;
						att.attr_value = 8; // 
						zhuangbei.attr_vec.push_back(att);
					}

					ret->equipVec.push_back(zhuangbei);
				}
				ret->zhuangbeiOpEnd();
			}

			return ret;
		}
		else
		{

			//
			Guaiwu* ret = new Guaiwu();
			GuaiWuData g_Data;
			g_Data.level = 1;
			g_Data.gongJiLi = 100;
			g_Data.xueLiang = 150;
			g_Data.baoJiLv = 0;
			g_Data.baoShangYinZi = 120;
			g_Data.jiNengChuFaLv = 0;
			g_Data.huJiaJianMianLv = 30;
			g_Data.kangXingJianMianLv = 10;
			g_Data.profession = (Profession)zhiye_b[(_heroId-1)%4];
			g_Data.gongJiJuLi = BaseActorPropertyParser::getParser()->getExtraProerty(g_Data.profession).gongJiJuLi;
			g_Data.shangHaiLeiXing = BaseActorPropertyParser::getParser()->getExtraProerty(g_Data.profession).mShangHaiLeiXing;
			g_Data.x_size = 1;
			g_Data.type = ((_heroId-1) % 4)==1?Type_Guaiwu_jingying:Type_Guaiwu_putong;
			ret->initWithGuaiwuData(g_Data);
			ret->setDefaultCdTime(0);

			return ret;
		}
	}
};

class CikeProvider : public BaseDataProvider
{
private:
	bool onePerson;
public:
	CikeProvider(bool onePerson)
	{
		this->onePerson = onePerson;
	}
	virtual BaseHero* getBaseHero(unsigned int _heroId,ScriptBuilderUserData user_data)
	{
		float gongJiJuLi_A[] = {1,1,2,2};
		float gongJiJuLi_B[] = {1,1,2,2};
		int *zhiye_a = NULL;
		{
			int zhiye_a1[] = {Profession_Chike,Profession_YouXia,Profession_Chike,Profession_Chike};
			int zhiye_a2[] = {Profession_YouXia,Profession_Chike,Profession_Chike,Profession_Chike};
			if (onePerson)
			{
				zhiye_a = zhiye_a1;
			}
			else
			{
				zhiye_a = zhiye_a2;
			}
		}
		int zhiye_b[] = {69,69,68,68};

		// A 队
		if (user_data.teamTag > 0)
		{
			//
			Hero* ret = new Hero();
			// test
			{
				// 不用装备
				ret->equipVec.clear();
				ret->skilVec.clear();

				HeroBaseProperty data;
				data.hero_id= _heroId;
				data.profession = (Profession)zhiye_a[(_heroId-1)%4];
				data.level = 10;

				data.exp_current = 20;
				data.exp_need = 2000;
				data.parent_hero_id = 0;
				data.postion = 1;
				data.vit_current = 100;

				data.mLiLiang = 4;
				data.mMinJie = 2;
				data.mZhiLi = 6;
				data.mTiNeng = 0;
				data.vit_current = 200;
				data.mHP = 60;
				data.mGongJiJuLi = CS::getHeroGongjiJuLiByProfession(data.profession);
				data.mBaoJiLv = 0;
				data.mBaoJiShangHaiYinZi = 150;
				data.mJiNengFaChuLv = 0;
				data.mShangHaiLeiXing = BaseActorPropertyParser::getParser()->getExtraProerty(data.profession).mShangHaiLeiXing;
				data.currentStage = 1;
				data.currentEffectPercent = 100;
				data.isYuanShen = false;
				ret->init(data);
				ret->setDefaultCdTime(0);
				if (data.profession == Profession_YouXia)
					ret->skilVec.push_back(SkillType_LiRenHuiXuan);

				{
					// 加装备,测试
					EquipData zhuangbei;
					zhuangbei.gongji_max = 110;
					zhuangbei.gongji_min = 100;
					zhuangbei.hole = 3;
					zhuangbei.hujia = 2;
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipDicrib = "equipDicrib";
					zhuangbei.zhuangbei_hero_id = 1;
					zhuangbei.zhuangbei_id = 2;
					zhuangbei.zhuangbei_level = 3;
					zhuangbei.equipName = "equipName";
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_fu_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Hushenfu;

					if (data.profession == Profession_YouXia)
					{
						ZBAttr att;
						att.attr_type = AttrType_skillzhouqi;
						att.attr_value = 8; // 
						zhuangbei.attr_vec.push_back(att);

						att.attr_type = AttrType_skillturn;
						att.attr_value = 8; // 
						zhuangbei.attr_vec.push_back(att);
					}

					ret->equipVec.push_back(zhuangbei);
				}
				ret->zhuangbeiOpEnd();
			}

			return ret;
		}
		else
		{

			//
			Guaiwu* ret = new Guaiwu();
			GuaiWuData g_Data;
			g_Data.level = 1;
			g_Data.gongJiLi = 100;
			g_Data.xueLiang = 150;
			g_Data.baoJiLv = 0;
			g_Data.baoShangYinZi = 120;
			g_Data.jiNengChuFaLv = 0;
			g_Data.huJiaJianMianLv = 30;
			g_Data.kangXingJianMianLv = 10;
			g_Data.profession = (Profession)zhiye_b[(_heroId-1)%4];
			g_Data.gongJiJuLi = BaseActorPropertyParser::getParser()->getExtraProerty(g_Data.profession).gongJiJuLi;
			g_Data.shangHaiLeiXing = BaseActorPropertyParser::getParser()->getExtraProerty(g_Data.profession).mShangHaiLeiXing;
			g_Data.x_size = 1;
			g_Data.type = ((_heroId-1) % 4)==1?Type_Guaiwu_jingying:Type_Guaiwu_putong;
			ret->initWithGuaiwuData(g_Data);
			ret->setDefaultCdTime(0);

			return ret;
		}
	}
};


class FashiProvider : public BaseDataProvider
{
private:
	bool onePerson;
public:
	FashiProvider(bool onePerson)
	{
		this->onePerson = onePerson;
	}
	virtual BaseHero* getBaseHero(unsigned int _heroId,ScriptBuilderUserData user_data)
	{
		float gongJiJuLi_A[] = {1,1,2,2};
		float gongJiJuLi_B[] = {1,1,2,2};
		int *zhiye_a = NULL;
		{
			int zhiye_a1[] = {Profession_Fashi,Profession_YouXia,Profession_Chike,Profession_Chike};
			int zhiye_a2[] = {Profession_Qishi,Profession_Fashi,Profession_Chike,Profession_Chike};
			if (onePerson)
			{
				zhiye_a = zhiye_a1;
			}
			else
			{
				zhiye_a = zhiye_a2;
			}
		}
		
		int zhiye_b[] = {69,69,68,68};

		// A 队
		if (user_data.teamTag > 0)
		{
			//
			Hero* ret = new Hero();
			// test
			{
				// 不用装备
				ret->equipVec.clear();
				ret->skilVec.clear();

				HeroBaseProperty data;
				data.hero_id= _heroId;
				data.profession = (Profession)zhiye_a[(_heroId-1)%4];
				data.level = 10;

				data.exp_current = 20;
				data.exp_need = 2000;
				data.parent_hero_id = 0;
				data.postion = 1;
				data.vit_current = 100;

				data.mLiLiang = 4;
				data.mMinJie = 2;
				data.mZhiLi = 6;
				data.mTiNeng = 0;
				data.vit_current = 200;
				data.mHP = 60;
				data.mGongJiJuLi = CS::getHeroGongjiJuLiByProfession(data.profession);
				data.mBaoJiLv = 0;
				data.mBaoJiShangHaiYinZi = 150;
				data.mJiNengFaChuLv = 0;
				data.mShangHaiLeiXing = BaseActorPropertyParser::getParser()->getExtraProerty(data.profession).mShangHaiLeiXing;
				data.currentStage = 1;
				data.currentEffectPercent = 100;
				data.isYuanShen = false;
				ret->init(data);
				ret->setDefaultCdTime(0);
				if (data.profession == Profession_Qishi)
					ret->skilVec.push_back(SkillType_ShouHu);

				{
					// 加装备,测试
					EquipData zhuangbei;
					zhuangbei.gongji_max = 110;
					zhuangbei.gongji_min = 100;
					zhuangbei.hole = 3;
					zhuangbei.hujia = 2;
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipDicrib = "equipDicrib";
					zhuangbei.zhuangbei_hero_id = 1;
					zhuangbei.zhuangbei_id = 2;
					zhuangbei.zhuangbei_level = 3;
					zhuangbei.equipName = "equipName";
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_fu_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Hushenfu;

					if (data.profession == Profession_Qishi)
					{
						ZBAttr att;
						att.attr_type = AttrType_skillzhouqi;
						att.attr_value = 8; // 
						zhuangbei.attr_vec.push_back(att);

						att.attr_type = AttrType_skillturn;
						att.attr_value = 8; // 
						zhuangbei.attr_vec.push_back(att);
					}

					ret->equipVec.push_back(zhuangbei);
				}
				ret->zhuangbeiOpEnd();
			}

			return ret;
		}
		else
		{

			//
			Guaiwu* ret = new Guaiwu();
			GuaiWuData g_Data;
			g_Data.level = 1;
			g_Data.gongJiLi = 100;
			g_Data.xueLiang = 150;
			g_Data.baoJiLv = 0;
			g_Data.baoShangYinZi = 120;
			g_Data.jiNengChuFaLv = 0;
			g_Data.huJiaJianMianLv = 30;
			g_Data.kangXingJianMianLv = 10;
			g_Data.profession = (Profession)zhiye_b[(_heroId-1)%4];
			g_Data.gongJiJuLi = BaseActorPropertyParser::getParser()->getExtraProerty(g_Data.profession).gongJiJuLi;
			g_Data.shangHaiLeiXing = BaseActorPropertyParser::getParser()->getExtraProerty(g_Data.profession).mShangHaiLeiXing;
			g_Data.x_size = 1;
			g_Data.type = ((_heroId-1) % 4)==1?Type_Guaiwu_jingying:Type_Guaiwu_putong;
			ret->initWithGuaiwuData(g_Data);
			ret->setDefaultCdTime(0);

			return ret;
		}
	}
};

vector<FightScript> GuideScriptMaker::genYemanrenInGuideFight()
{
	FightScriptBuilder buider;
	unsigned int A_team[FormationHeroAcountMax] = {1,0,0,0};
	unsigned int B_team[FormationHeroAcountMax] = {1,0,3,0};
	vector<FightScript> scrips;
	ScriptBuilderUserData usrData;
	YemanRenProvider pro(true);
	{
		FightSUD data;
		FightScript A_Script;
		FightScript B_Script;
		data.fightIndex = 0;
		usrData.data = &data;
		buider.genScript(A_team,B_team,A_Script,B_Script,&pro,usrData,FBB_BoardA);
		scrips.push_back(A_Script);
	}


	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		bool suc = saveScript(YEMANREN_FIGHT_FILE,scrips);
		CCAssert(suc,"");
	}



	return scrips;
}
vector<FightScript> GuideScriptMaker::genYemanrenWithYouxiaInGuideFight()
{
	FightScriptBuilder buider;
	unsigned int A_team[FormationHeroAcountMax] = {1,2,0,0};
	unsigned int B_team[FormationHeroAcountMax] = {1,0,3,0};
	vector<FightScript> scrips;
	ScriptBuilderUserData usrData;
	YemanRenProvider pro(false);
	{
		FightSUD data;
		FightScript A_Script;
		FightScript B_Script;
		data.fightIndex = 0;
		usrData.data = &data;
		buider.genScript(A_team,B_team,A_Script,B_Script,&pro,usrData,FBB_BoardA);
		scrips.push_back(A_Script);
	}

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		bool suc = saveScript(YEMANREN_YOUXIA_FIGHT_FILE,scrips);
		CCAssert(suc,"");
	}

	return scrips;
}

vector<FightScript> GuideScriptMaker::genFashiInGuideFight()
{
	FightScriptBuilder buider;
	unsigned int A_team[FormationHeroAcountMax] = {1,0,0,0};
	unsigned int B_team[FormationHeroAcountMax] = {1,0,3,0};
	vector<FightScript> scrips;
	ScriptBuilderUserData usrData;
	FashiProvider pro(true);
	{
		FightSUD data;
		FightScript A_Script;
		FightScript B_Script;
		data.fightIndex = 0;
		usrData.data = &data;
		buider.genScript(A_team,B_team,A_Script,B_Script,&pro,usrData,FBB_BoardA);
		scrips.push_back(A_Script);
	}

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		bool suc = saveScript(FASHI_FIGHT_FILE,scrips);
		CCAssert(suc,"");
	}

	return scrips;
}
vector<FightScript> GuideScriptMaker::genFashiWithQishiInGuideFight()
{
	FightScriptBuilder buider;
	unsigned int A_team[FormationHeroAcountMax] = {1,2,0,0};
	unsigned int B_team[FormationHeroAcountMax] = {1,0,3,0};
	vector<FightScript> scrips;
	ScriptBuilderUserData usrData;
	FashiProvider pro(false);
	{
		FightSUD data;
		FightScript A_Script;
		FightScript B_Script;
		data.fightIndex = 0;
		usrData.data = &data;
		buider.genScript(A_team,B_team,A_Script,B_Script,&pro,usrData,FBB_BoardA);
		scrips.push_back(A_Script);
	}

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		bool suc = saveScript(FASHI_QISHI_FIGHT_FILE,scrips);
		CCAssert(suc,"");
	}

	return scrips;
}

vector<FightScript> GuideScriptMaker::genCikeInGuideFight()
{
	FightScriptBuilder buider;
	unsigned int A_team[FormationHeroAcountMax] = {1,0,0,0};
	unsigned int B_team[FormationHeroAcountMax] = {1,0,3,0};
	vector<FightScript> scrips;
	ScriptBuilderUserData usrData;
	CikeProvider pro(true);
	{
		FightSUD data;
		FightScript A_Script;
		FightScript B_Script;
		data.fightIndex = 0;
		usrData.data = &data;
		buider.genScript(A_team,B_team,A_Script,B_Script,&pro,usrData,FBB_BoardA);
		scrips.push_back(A_Script);
	}

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		bool suc = saveScript(CIKE_FIGHT_FILE,scrips);
		CCAssert(suc,"");
	}

	return scrips;
}
vector<FightScript> GuideScriptMaker::genCikeWithYouXiaInGuideFight()
{
	FightScriptBuilder buider;
	unsigned int A_team[FormationHeroAcountMax] = {1,2,0,0};
	unsigned int B_team[FormationHeroAcountMax] = {1,0,3,0};
	vector<FightScript> scrips;
	ScriptBuilderUserData usrData;
	CikeProvider pro(false);
	{
		FightSUD data;
		FightScript A_Script;
		FightScript B_Script;
		data.fightIndex = 0;
		usrData.data = &data;
		buider.genScript(A_team,B_team,A_Script,B_Script,&pro,usrData,FBB_BoardA);
		scrips.push_back(A_Script);
	}

	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		bool suc = saveScript(CIKE_YOUXIA_FIGHT_FILE,scrips);
		CCAssert(suc,"");
	}

	return scrips;
}


vector<FightScript> GuideScriptMaker::getYemanrenInGuideFight()
{
	vector<FightScript> ret;
	loadScript(YEMANREN_FIGHT_FILE,ret);
	return ret;
}
vector<FightScript> GuideScriptMaker::getYemanrenWithYouxiaInGuideFight()
{
	vector<FightScript> ret;
	loadScript(YEMANREN_YOUXIA_FIGHT_FILE,ret);
	return ret;
}

vector<FightScript> GuideScriptMaker::getFashiInGuideFight()
{
	vector<FightScript> ret;
	loadScript(FASHI_FIGHT_FILE,ret);
	return ret;
}
vector<FightScript> GuideScriptMaker::getFashiWithQishiInGuideFight()
{
	vector<FightScript> ret;
	loadScript(FASHI_QISHI_FIGHT_FILE,ret);
	return ret;
}

vector<FightScript> GuideScriptMaker::getCikeInGuideFight()
{
	vector<FightScript> ret;
	loadScript(CIKE_FIGHT_FILE,ret);
	return ret;
}
vector<FightScript> GuideScriptMaker::getCikeWithYouXiaInGuideFight()
{
	vector<FightScript> ret;
	loadScript(CIKE_YOUXIA_FIGHT_FILE,ret);
	return ret;
}

void GuideScriptMaker::testLoadAll()
{
	vector<FightScript> scrips;
	vector<FightScript> temp;
	{
		temp = getYemanrenInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		temp = getYemanrenWithYouxiaInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		CCLog("getScript yemanren over");


		/*temp = getCikeInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		temp = getCikeWithYouXiaInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		CCLog("getScript cike over");


		temp = getFashiInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		temp = getFashiWithQishiInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		CCLog("getScript fashi over");*/

	}
	CCLog("getScript over");

	FightDirector dir;
	vector<FightDirector> dir_vec;
	for (int i=0;i<scrips.size();i++)
	{
		dir.initScript(scrips[i]);
		dir_vec.push_back(dir);
	}
	// 直接换scene
	Bundle bundle;
	bundle.putString("JumpToWhenEnd","FirstInitGuideToMainPage");
	BaseSprite* beiJing = BaseSprite::create("zhandou_beijing_chengzhenzhoubian.png");
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec,beiJing,bundle);
}

void GuideScriptMaker::testGenAll()
{

	vector<FightScript> scrips;
	vector<FightScript> temp;
	{
		temp = genYemanrenInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		temp = genYemanrenWithYouxiaInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		CCLog("genScript yemanren over");


		temp = genCikeInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		temp = genCikeWithYouXiaInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		CCLog("genScript cike over");


		temp = genFashiInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		temp = genFashiWithQishiInGuideFight();
		scrips.insert(scrips.end(),temp.begin(),temp.end());
		CCLog("genScript fashi over");

	}
	CCLog("genScript over");

	// 保存，读取校验
	{
#define FILE_TEMP	"config/script.bin"
		if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
		{
			CCAssert(saveScript(FILE_TEMP,scrips),"save error");
			vector<FightScript> loaded;
			CCAssert(loadScript(FILE_TEMP,loaded),"load error");

			// 校验
			int num = scrips.size();
			for (int i=0;i<num;i++)
			{
				if (!scrips[i].isEqual(loaded[i]))
				{
					CCLog("%s line:%d----> !A_Script.isEqual(A_Temp) Assert!",__FUNCTION__,__LINE__);
					CCMessageBox("Data InValide!","Error");
					return;
				}
			}

			CCLog("%s --> saveScript loadScript OK",__FUNCTION__);
		}
	}

	FightScriptBuilder buider;
	int len;
	char* script_char = buider.FightScriptVec_2_CharArr(scrips,len);
	CCLog("msg build over");   
	vector<FightScript> A_tempVec = buider.CharArr_2_FightScriptVec(script_char,scrips.size());
	delete [] script_char;
	CCLog("msg parse over");
	// 校验
	{
		int num = scrips.size();
		for (int i=0;i<num;i++)
		{
			if (!scrips[i].isEqual(A_tempVec[i]))
			{
				CCLog("%s line:%d----> !A_Script.isEqual(A_Temp) Assert!",__FUNCTION__,__LINE__);
				CCMessageBox("Data InValide!","Error");
				return;
			}
		}
	}

	FightDirector dir;
	vector<FightDirector> dir_vec;
	for (int i=0;i<A_tempVec.size();i++)
	{
		dir.initScript(A_tempVec[i]);
		dir_vec.push_back(dir);
	}
	// 直接换scene
	Bundle bundle;
	bundle.putBool("testMode",true);
	BaseSprite* beiJing = BaseSprite::create("zhandou_beijing_chengzhenzhoubian.png");
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec,beiJing,bundle);
}


bool GuideScriptMaker::saveScript(string fileName,vector<FightScript> data)
{
	FightScriptBuilder buildr;
	int length;
	char* bin_data = buildr.FightScriptVec_2_CharArr(data,length); // 转字节序再存
	// 保存个数信息
	char* config_data = new char[length + 1];
	memcpy(config_data,bin_data,length + 1);
	config_data[length] = (char)data.size();

	bool ret = LFFileUtiles::getLFFileUtiles()->writeBinaryDataToDocFile(fileName,config_data,length+1);

	delete [] bin_data;
	delete [] config_data;

	return ret;
}
bool GuideScriptMaker::loadScript(string fileName,vector<FightScript>& data)
{
	unsigned long size;
	const char* bin_data = LFFileUtiles::getLFFileUtiles()->readBinaryDataFromResFile(fileName,size);
	if (bin_data == NULL)
	{
		return false;
	}
	int count = bin_data[size-1];

	// 解包
	FightScriptBuilder buildr;
	data = buildr.CharArr_2_FightScriptVec(bin_data,count);

	delete [] bin_data;

	return true;
}

GuideScriptMaker::~GuideScriptMaker(void)
{
}
