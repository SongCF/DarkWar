#pragma once
#include <list>
#include <vector>
#include <memory.h>
#include "GameBuff.h"

using namespace std;

struct HeroBaseProperty;
struct EquipData;
struct GameBuff;
struct HeroPropertySummery;

// 英雄的属性缩略信息
// 全部是本身的属性，不带任何影响的因素
// 与之完全对应的是HeroBaseProperty，HeroDetail
struct HeroPropertySummery
{
	bool is_yuanshen;					// 是否是元神

	unsigned int profession;			// 职业
	unsigned int stage;					// 阶
	unsigned int level;					// 等级
	unsigned int base_hp;				// 基础血量，纯自身的血量，不包括装备等附加的
	unsigned int liliang;				// 基础力量
	unsigned int mingjie;				// 基础敏捷
	unsigned int zhili;					// 基础智力
	unsigned int tineng;				// 基础体能
	unsigned int baojilv;				// 基础暴击率
	unsigned int baoshang_yinzi;		// 基础暴击伤害因子，比如200%，即填写200
	unsigned int effect_convert_per;	// 基础效果转换率，默认100即100%

	// 技能数据
	unsigned int skill_type;			// 技能类型，没有就是0
	unsigned int skill_level;			// 技能等级

	HeroPropertySummery()
	{
		memset(this,0,sizeof(HeroPropertySummery));
	};
};


// 角色计算数据的结构体，PVP和PVE用
struct CalculateData
{
	HeroPropertySummery base_pro;
	vector<EquipData> allEquips;
	list<GameBuff> allBuffs;
	int mainPro;	// 主属性值，是有装备影响的

	// 额外包含的data数据，比如真身的话，就可能包含了他元神的数据
	// 这里面的CalculateData，里面allBuffs始终是空
	// 没有buff的数据。有也不会管！！
	vector<CalculateData> includeCalDatas;

	CalculateData()
	{
		mainPro = 0;
	}
};
