#pragma once
#include <vector>
#include <string.h>
using namespace std;

struct EquipData;
class Hero;

// 加成护甲	加成生命	加成攻击	加成格挡值	加成伤害暴击值
enum GM_PropertyIndex
{
	GM_HuJiaAddPer = 0,
	GM_HPAddPer,
	GM_GongJiAddPer,
	GM_GeDangZhiAdd,
	GM_BaoShangYinZiAdd,

	GM_PropertyNum,
};
#define		GongMingLevelMAX	10 // 共鸣分10级，不同的级数要求的starNum不一样
struct GongMingConfig
{
	// 各个等级需要的星星数，1级就是levelNeedStarNumConfig[1]需要的
	unsigned int levelNeedStarNumConfig[GongMingLevelMAX+1];
	// 等级1就是取properyAdd[1]的数据
	unsigned int properyAdd[GongMingLevelMAX+1][GM_PropertyNum];

	GongMingConfig()
	{
		// 清空
		memset(properyAdd,0,sizeof(unsigned int)*(GongMingLevelMAX+1)*(GM_PropertyNum));
		memset(levelNeedStarNumConfig,0,sizeof(unsigned int)*(GongMingLevelMAX+1));
	}
};

/**
 * 装备共鸣的配置管理器
 */
class ZhuangbeiGongMingManager
{
private:
	GongMingConfig mConfig;
	static ZhuangbeiGongMingManager* mInstance;
	ZhuangbeiGongMingManager(void);
public:
	static ZhuangbeiGongMingManager* getManager();

	// 没有初始化就用默认值
	void initGongMingConfig(GongMingConfig config);

	// 获取当前装备穿着带来的额共鸣星个数
	// 强化一颗银星，增加1点共鸣星级
	// 强化一颗金星，增加3点共鸣星级
	// 目前不考虑元神的数据
	unsigned int getGongMingStarNum(const vector<EquipData>& euqipsOnHero);
	// starNum获取当前的level
	unsigned int getGongMingLevel(unsigned int starNum);

	// 根据装备获取当前的某个属性加成
	unsigned int getPropertyAdd(const vector<EquipData>& euqipsOnHero,GM_PropertyIndex index);

	// 获取完整的共鸣配置数据
	GongMingConfig getGongMingConfig();

	~ZhuangbeiGongMingManager(void);
};

