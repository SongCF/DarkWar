#include "FightResourceLoader.h"
#include "TexturePreLoad.h"
#include "ImageManager.h"
#include "PreLoadPareser.h"


FightResourceLoader::FightResourceLoader(void)
{
}

FightResourceLoader::~FightResourceLoader(void)
{
}

string FightResourceLoader::getSkillPreLoadKey(SkillType skillType)
{
	return SkillTree::getSkillPlistFile(skillType,".png"); // 暂时不管 
}

void FightResourceLoader::loadResource(vector<FightScript> scriptVec)
{
	bool isNormalFightLoaded = false;
	bool isUseBloodLoaded = false;
	map<SkillType,int> isSkillLoaded;
	map<ItemsType,int> isItemLoaded;
	map<string,int> isEquipLoaded;

	vector<string> pic_need_load;

	int scriptNum = scriptVec.size();
	for (int scriptIndex = 0;scriptIndex < scriptNum ; scriptIndex ++)
	{
		// 载入得到的物品的图片，以及掉装的底纹
		int itemNum = scriptVec[scriptIndex].endResult.gettedItem.size();
		if (itemNum > 0)
		{
			// 道具的特效，道具的底纹
		}
		
		for (int itemIndex = 0;itemIndex < itemNum;itemIndex ++)
		{
			ItemsType _type = (ItemsType)scriptVec[scriptIndex].endResult.gettedItem[itemIndex].type;
			map<ItemsType,int>::iterator it = isItemLoaded.find(_type);
			if (it == isItemLoaded.end())
			{// 如果没有存放，那么就加入
				isItemLoaded.insert(pair<ItemsType,int>(_type,0));
				pic_need_load.push_back(ImageManager::getManager()->getItemFilename(_type));
			}
		}

		int equipNum = scriptVec[scriptIndex].endResult.gettedZhuangBei.size();
		// 载入装备的图片，以及底纹掉装的底纹
		for (int equipIndex = 0;equipIndex < equipNum;equipIndex ++)
		{
			string key_words = scriptVec[scriptIndex].endResult.gettedZhuangBei[equipIndex].equipPic;
			map<string,int>::iterator it = isEquipLoaded.find(key_words);
			if (it == isEquipLoaded.end())
			{// 如果没有存放，那么就加入
				isEquipLoaded.insert(pair<string,int>(key_words,0));
				pic_need_load.push_back(ImageManager::getManager()->getEquipImageFile(key_words));
			}

			// 底纹先不管，掉落的效果也先不管
		}


		// 载入技能的图片
		// 载入普通攻击的图片
		// 载入喝血的图片
		int stepNum = scriptVec[scriptIndex].stepVec.size();
		for (int stepIndex = 0;stepIndex < stepNum ; stepIndex++)
		{
			if (scriptVec[scriptIndex].stepVec[stepIndex].stepType == StepType_Skill)
			{
				string pic = getSkillPreLoadKey(scriptVec[scriptIndex].stepVec[stepIndex].skillData.skillType);
				pic_need_load.push_back(pic);
			}
			else if (scriptVec[scriptIndex].stepVec[stepIndex].stepType == StepType_Fight)
			{
				if (scriptVec[scriptIndex].stepVec[stepIndex].fightData.fightPattern == Fight_Normal || 
					scriptVec[scriptIndex].stepVec[stepIndex].fightData.fightPattern == Fight_Normal_BaoJi)
				{
					if (isNormalFightLoaded)
						continue;

					pic_need_load.push_back("jineng_lieshou_baoji.png");
					pic_need_load.push_back("jineng_lieshou_pugong.png");

					isNormalFightLoaded = true;
				}
			}
			else if (scriptVec[scriptIndex].stepVec[stepIndex].stepType == StepType_UseBlood)
			{
				if (isUseBloodLoaded)
					continue;
				pic_need_load.push_back("zhandou_yaopin_plist.png");
				isUseBloodLoaded = true;
			}
		}
	}

	CCLog("will load %d pic(s)",pic_need_load.size());

	if (!TexturePreLoad::getInstance()->isBusy())
	{
		TexturePreLoad::getInstance()->addTask(pic_need_load);
		TexturePreLoad::getInstance()->startAsync(NULL);
	}
}
