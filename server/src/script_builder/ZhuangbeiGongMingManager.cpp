#include "ZhuangbeiGongMingManager.h"
#include "CommonDefine.h"

ZhuangbeiGongMingManager* ZhuangbeiGongMingManager::mInstance = NULL;

ZhuangbeiGongMingManager::ZhuangbeiGongMingManager(void)
{
	// 等级需要的星数
	mConfig.levelNeedStarNumConfig[0] = 0;
	mConfig.levelNeedStarNumConfig[1] = 2;
	mConfig.levelNeedStarNumConfig[2] = 4;
	mConfig.levelNeedStarNumConfig[3] = 7;
	mConfig.levelNeedStarNumConfig[4] = 10;
	mConfig.levelNeedStarNumConfig[5] = 14;
	mConfig.levelNeedStarNumConfig[6] = 18;
	mConfig.levelNeedStarNumConfig[7] = 24;
	mConfig.levelNeedStarNumConfig[8] = 30;
	mConfig.levelNeedStarNumConfig[9] = 38;
	mConfig.levelNeedStarNumConfig[10] = 48;

	// 个等级的加成数据
	mConfig.properyAdd[1][GM_HuJiaAddPer] = 1;
	mConfig.properyAdd[1][GM_HPAddPer] = 0;
	mConfig.properyAdd[1][GM_GongJiAddPer] = 0;
	mConfig.properyAdd[1][GM_GeDangZhiAdd] = 0;
	mConfig.properyAdd[1][GM_BaoShangYinZiAdd] = 0;


	mConfig.properyAdd[2][GM_HuJiaAddPer] = 3;
	mConfig.properyAdd[2][GM_HPAddPer] = 0;
	mConfig.properyAdd[2][GM_GongJiAddPer] = 0;
	mConfig.properyAdd[2][GM_GeDangZhiAdd] = 0;
	mConfig.properyAdd[2][GM_BaoShangYinZiAdd] = 0;


	mConfig.properyAdd[3][GM_HuJiaAddPer] = 5;
	mConfig.properyAdd[3][GM_HPAddPer] = 3;
	mConfig.properyAdd[3][GM_GongJiAddPer] = 0;
	mConfig.properyAdd[3][GM_GeDangZhiAdd] = 0;
	mConfig.properyAdd[3][GM_BaoShangYinZiAdd] = 0;



	mConfig.properyAdd[4][GM_HuJiaAddPer] = 9;
	mConfig.properyAdd[4][GM_HPAddPer] = 6;
	mConfig.properyAdd[4][GM_GongJiAddPer] = 0;
	mConfig.properyAdd[4][GM_GeDangZhiAdd] = 0;
	mConfig.properyAdd[4][GM_BaoShangYinZiAdd] = 0;


	mConfig.properyAdd[5][GM_HuJiaAddPer] = 13;
	mConfig.properyAdd[5][GM_HPAddPer] = 9;
	mConfig.properyAdd[5][GM_GongJiAddPer] = 0;
	mConfig.properyAdd[5][GM_GeDangZhiAdd] = 0;
	mConfig.properyAdd[5][GM_BaoShangYinZiAdd] = 0;


	mConfig.properyAdd[6][GM_HuJiaAddPer] = 18;
	mConfig.properyAdd[6][GM_HPAddPer] = 15;
	mConfig.properyAdd[6][GM_GongJiAddPer] = 7;
	mConfig.properyAdd[6][GM_GeDangZhiAdd] = 0;
	mConfig.properyAdd[6][GM_BaoShangYinZiAdd] = 0;


	mConfig.properyAdd[7][GM_HuJiaAddPer] = 24;
	mConfig.properyAdd[7][GM_HPAddPer] = 20;
	mConfig.properyAdd[7][GM_GongJiAddPer] = 12;
	mConfig.properyAdd[7][GM_GeDangZhiAdd] = 0;
	mConfig.properyAdd[7][GM_BaoShangYinZiAdd] = 0;


	mConfig.properyAdd[8][GM_HuJiaAddPer] = 30;
	mConfig.properyAdd[8][GM_HPAddPer] = 25;
	mConfig.properyAdd[8][GM_GongJiAddPer] = 18;
	mConfig.properyAdd[8][GM_GeDangZhiAdd] = 0;
	mConfig.properyAdd[8][GM_BaoShangYinZiAdd] = 0;

	mConfig.properyAdd[9][GM_HuJiaAddPer] = 40;
	mConfig.properyAdd[9][GM_HPAddPer] = 30;
	mConfig.properyAdd[9][GM_GongJiAddPer] = 25;
	mConfig.properyAdd[9][GM_GeDangZhiAdd] = 100;
	mConfig.properyAdd[9][GM_BaoShangYinZiAdd] = 0;


	mConfig.properyAdd[10][GM_HuJiaAddPer] = 50;
	mConfig.properyAdd[10][GM_HPAddPer] = 40;
	mConfig.properyAdd[10][GM_GongJiAddPer] = 35;
	mConfig.properyAdd[10][GM_GeDangZhiAdd] = 0;
	mConfig.properyAdd[10][GM_BaoShangYinZiAdd] = 50;
}

ZhuangbeiGongMingManager* ZhuangbeiGongMingManager::getManager()
{
	if (mInstance == NULL)
	{
		mInstance = new ZhuangbeiGongMingManager();
	}
	return mInstance;
}

void ZhuangbeiGongMingManager::initGongMingConfig(GongMingConfig config)
{
	mConfig = config;
}
unsigned int ZhuangbeiGongMingManager::getGongMingStarNum(const vector<EquipData>& euqipsOnHero)
{
	int starNum = 0;

	for (int i = 0,count = euqipsOnHero.size(); i < count; i++)
	{
		for (int qianghuaIndex = 0; qianghuaIndex < euqipsOnHero[i].qianghua_data.qianghua_count; qianghuaIndex++)
		{
			if (euqipsOnHero[i].qianghua_data.info_arr[qianghuaIndex].level == 3) // 银星
			{
				starNum +=1;
			}
			else if (euqipsOnHero[i].qianghua_data.info_arr[qianghuaIndex].level == 4) // 金星
			{
				starNum +=3;
			}
			else if (euqipsOnHero[i].qianghua_data.info_arr[qianghuaIndex].level == QianghuaLevel_jin_special) // 金星
			{
				starNum +=3;
			}
		}
	}

	return starNum;
}
unsigned int ZhuangbeiGongMingManager::getGongMingLevel(unsigned int starNum)
{
	unsigned int levelRet = 0;
	for (int i=0;i<=GongMingLevelMAX;i++)
	{
		if (starNum >= mConfig.levelNeedStarNumConfig[i])
		{
			levelRet = i;
		}
		else
		{
			break; // 直接退出，后面的会更大，不需要再比较了
		}
	}
	return levelRet;

}
unsigned int ZhuangbeiGongMingManager::getPropertyAdd(const vector<EquipData>& euqipsOnHero,GM_PropertyIndex index)
{
	CCAssert(index >= 0 && index < GM_PropertyNum,"");
	int level = getGongMingLevel(getGongMingStarNum(euqipsOnHero));
	CCAssert(level <= GongMingLevelMAX,"");
	return mConfig.properyAdd[level][index];
}

GongMingConfig ZhuangbeiGongMingManager::getGongMingConfig()
{
	return mConfig;
}

ZhuangbeiGongMingManager::~ZhuangbeiGongMingManager(void)
{
}
