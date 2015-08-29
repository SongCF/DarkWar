#include "BaseActorPropertyParser.h"
#include "LFFileUtiles.h"
#include "BaseActor.h"
#include "ResourcePathManager.h"
#include "LFStrings.h"
#include "include_json.h"
#include "LFLog.h"
#include "CS_Common.h"

#define FILE_PATH "config/profession_config.txt"

BaseActorPropertyParser* BaseActorPropertyParser::parser = new BaseActorPropertyParser();


BaseActorPropertyParser::BaseActorPropertyParser(void)
{
}

BaseActorPropertyParser* BaseActorPropertyParser::getParser()
{
	return parser;
}
/**
 * 这个数据只是本地用的，也睡跟服务器配置一样的。主要是面板显示需要的信息而已
 */
HeroFixedPro BaseActorPropertyParser::getExtraProerty(Profession type,bool assertIfNotFound /* = true */)
{
	map<Profession, HeroFixedPro >::iterator l_it;; 
	l_it=mTemplateExtraPool.find(type);
	if(l_it==mTemplateExtraPool.end())
	{
		CCLog("Error : %s ---> type = %d",__FUNCTION__,type);
		if(assertIfNotFound)
			CCAssert(false,"没找到");
	}
	return l_it->second;
}

bool BaseActorPropertyParser::init()
{
	mTemplateExtraPool.clear();

	bool success;
	string doc = LFFileUtiles::getLFFileUtiles()->getResourceFile(FILE_PATH,success);

	if (!success)
	{
		LFLogError("load file[%s] error!",FILE_PATH);
		abort();
		return false;
	}


	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(doc,root))
	{
		LFLogError("reader.parse failed!");
		LFLog(doc.c_str());
		CCAssert(false,"");
		abort();
		return false;
	}

	// 英雄部分的解析
	{
		Json::Value _professions= root["hero_professions"];
		if (!_professions.isNull() && _professions.isArray())
		{
			int _professNum = _professions.size();
			for (int i=0;i<_professNum;i++)
			{
				Json::Value onePair = _professions[i];

				HeroFixedPro pro;
				Profession professionId = (Profession)onePair["profession_id"].asInt();

				pro.baseName = onePair["keyName"].asString();
				pro.nameInChinese = onePair["nameInChinese"].asString();
				//pro.gongJiJuLi = onePair["gongJiJuLi"].asDouble(); // 不在使用配置的
				pro.gongJiJuLi = CS::getHeroGongjiJuLiByProfession(professionId); 
				pro.wugong_baoji = (WuGong)onePair["wugong_baoji"].asInt();
				pro.wugong_putong = (WuGong)onePair["wugong_putong"].asInt();
				pro.hotPosXPercentOfOneHand = onePair["oneHandHotPosX"].asDouble();
				pro.hotPosYPercentOfOneHand = onePair["oneHandHotPosY"].asDouble();
				pro.hotPosXPercentOfTwoHand = onePair["twoHandHotPosX"].asDouble();
				pro.hotPosYPercentOfTwoHand = onePair["twoHandHotPosY"].asDouble();
				pro.hero_or_guaiwu = true;

				if (onePair["shangHaiLeiXing"].asString() == "SHANG_HAI_WU_LI")
				{
					pro.mShangHaiLeiXing = SHANG_HAI_WU_LI;
				}else
				{
					pro.mShangHaiLeiXing = SHANG_HAI_MO_FA;
				}

				mTemplateExtraPool.insert(pair<Profession,HeroFixedPro>(professionId,pro));
			}
		}else
		{
			LFLogError("find professions root error,may it is not an array!");
			LFLog(doc.c_str());
			CCAssert(false,"");
			abort();
		}
	}

	// 怪物部分的解析
	{
		Json::Value _professions= root["guaiwu_professions"];
		if (!_professions.isNull() && _professions.isArray())
		{
			int _professNum = _professions.size();
			for (int i=0;i<_professNum;i++)
			{
				Json::Value onePair = _professions[i];

				HeroFixedPro pro;
				pro.baseName = onePair["keyName"].asString();
				pro.nameInChinese = onePair["nameInChinese"].asString();
				pro.gongJiJuLi = onePair["gongJiJuLi"].asDouble();
				pro.wugong_baoji = (WuGong)onePair["wugong_baoji"].asInt();
				pro.wugong_putong = (WuGong)onePair["wugong_putong"].asInt();
				pro.hotPosXPercentForGuaiWu = onePair["hot_pos_x"].asDouble();
				pro.hotPosYPercentForGuaiWu = onePair["hot_pos_y"].asDouble();
				pro.hero_or_guaiwu = false;

				if (onePair["shangHaiLeiXing"].asString() == "SHANG_HAI_WU_LI")
				{
					pro.mShangHaiLeiXing = SHANG_HAI_WU_LI;
				}else
				{
					pro.mShangHaiLeiXing = SHANG_HAI_MO_FA;
				}

				Profession professionId = (Profession)onePair["profession_id"].asInt();
				mTemplateExtraPool.insert(pair<Profession,HeroFixedPro>(professionId,pro));
			}
		}else
		{
			LFLogError("find professions root error,may it is not an array!");
			LFLog(doc.c_str());
			CCAssert(false,"");
			abort();
		}
	}

	// boss
	{
		HeroFixedPro pro;
		pro.nameInChinese = LF::getString("guaiwu");
		pro.gongJiJuLi = 2;
		pro.mShangHaiLeiXing = SHANG_HAI_WU_LI;
		pro.baseName = "boss_mohuajuren";
		pro.hotPosXPercentForGuaiWu = 0.5f;
		pro.hotPosYPercentForGuaiWu = 0.5f;
		pro.hero_or_guaiwu = false;
		mTemplateExtraPool.insert(pair<Profession,HeroFixedPro>(Profession_Big_Boss_1,pro));
	}
	return true;
}


BaseActorPropertyParser::~BaseActorPropertyParser(void)
{
}
