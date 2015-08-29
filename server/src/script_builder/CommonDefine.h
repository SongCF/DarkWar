#pragma once

#include <string>
#include "protocol.h"

using namespace std;

#ifdef  _CLIENT_
#else
#include "logs.h"
#include <assert.h>
inline void assert_stub(bool conf, const char* str)
{
	assert(conf);
}
#define CCLog LogMsg
#define CCAssert assert_stub
#endif


// 伤害类型
#define SHANG_HAI_MO_FA 0x0001		// 魔法伤害 1
#define SHANG_HAI_WU_LI 0x0010		// 物理伤害 16

#define MAX_EQUIP_HIDE_ARRTIBUTE_NUM		5 // 装备的隐藏属性最大是5
#define MAX_EQUIP_ZHUANKE_ARRTIBUTE_NUM		2 // 装备的篆刻属性最大是2

enum EquipType
{
	EquipType_WuQi = 0,
	EquipType_FangJu,
	EquipType_ShiPin,
};

enum EquipRequirePlace
{
	POS_NONE = -1,
	POS_Hushenfu = 0,
	POS_XiongJia,
	POS_JieZhi1,
	POS_JieZhi2,
	POS_ZhuWuQi,
	POS_FuShou,
};

// 加减血量的就没有了做了！！！！！！
struct EquipData
{
	unsigned long long zhuangbei_id;
	unsigned int group_id; //装备模板组
	ZhuangbeiType zhuangbei_type;
	string equipName;
	ZhuangbeiColour zhuangbei_colour;
	string equipPic;	// 关键字
	unsigned int profession_id;    	//绑定的职业ID, 为0是任何职业

	bool isInFengYin;	// 在封印里面，那么所有的数据都不能用了，除了类型，名字，颜色等
	bool isShenBing;	// 是否是神兵
	//装备在谁的身上
	unsigned int zhuangbei_hero_id;

	unsigned int zhuangbei_level;
	unsigned int hole;
	unsigned int diamond_type_1;
	unsigned int diamond_type_2;
	unsigned int diamond_type_3;

	unsigned int gongji_min;
	unsigned int gongji_max;
	unsigned int hujia;

	//强化信息
	QianghuaData qianghua_data;

	// 装备的魔法属性
	std::vector<ZBAttr> attr_vec;

	unsigned int jinglian_times;// 精炼的次数，开始是0，精炼就是0阶
	// 精炼所提供的属性
	std::vector<ZBAttr> jinglian_attr_vec;
	// 即将拥有的精炼数据（下一阶的不用了！！！）
	std::vector<ZBAttr> jinglian_attr_will_have_vec;
	// 篆刻经文提供的数据
	std::vector<ZBAttr> zhuanke_attr_vec;

	//未知的属性的个数：还没有解的隐藏属性个数
	int unknownHideAttriNum;	
	//已经解开的隐藏属性
	std::vector<ZBAttr> known_hide_attr_vec;

	//现有洗炼属性 
	std::vector<ZBAttr> purify_attr_vec; 

	//附魔信息，一个装备可以额外附魔一次
	bool has_fumo;
	ZBAttr fumo_attr;

	string equipDicrib;

	EquipData();
	// 获取所有的魔法属性
	vector<ZBAttr> getAttributeVec(bool includeAddtional = true) const;
	// 获得某个魔法属性的值，如果includeAddtional，那么把附魔,精炼，篆刻的对应属性也会算在里面
	// 通常是需要计算的，所以默认就是true
	unsigned int getAtrriValueByType(AttrType type,bool includeAddtional = true) const;
	void initWithZhungbeiDetail(const ZhuangbeiDetail* data);
	bool isEqual(EquipData data);
};

// 武功的标记
enum WuGong
{

};

// 固定的属性
struct HeroFixedPro
{
	string baseName;		// 就是基础的名字，比如野蛮人就是yeManRen,用于找跟他相关的图片用的
	string nameInChinese;	// 中文名字，比如野蛮人，就是野蛮人
	//int gongJiJuLi;
	float gongJiJuLi;
	int mShangHaiLeiXing;
	bool hero_or_guaiwu;	// hero则true
	WuGong wugong_putong;
	WuGong wugong_baoji;
	// 只用于hero，区分单手装和双手装
	float hotPosXPercentOfOneHand;
	float hotPosYPercentOfOneHand;
	float hotPosXPercentOfTwoHand;
	float hotPosYPercentOfTwoHand;
	// 只用于怪物
	float hotPosXPercentForGuaiWu;
	float hotPosYPercentForGuaiWu;

	HeroFixedPro();
};

// 没有任何装备和其他数据影响下的人物属性 ---> HeroDetail
// 都是计算好的值
struct HeroBaseProperty
{
	Profession profession;
	string customName;		// 自定义名字

	unsigned int hero_id;
	unsigned int vip_level;
	unsigned int parent_hero_id;
	unsigned int level;

	unsigned int currentStage;	// 阶，当前的阶，真身的话通过教堂可以提升，元神的话满级之后通过金币升阶

	// 当前效果值，就是转换率，本来可以打出100，如果转换率是80，那么只能打出80
	unsigned int currentEffectPercent;	
	// 是否是元神,元神没有技能的配置,也没有站位的要求，不过就直接映射主角的站位即可
	bool isYuanShen;	

	//阵型站位，0表示未上阵。
	unsigned int postion;

	bool isLocked;	// 是否是锁住的
	bool canUnlock;	// 能否解锁

	//
	unsigned int exp_current;
	unsigned int exp_need;

	unsigned int mLiLiang;		// 力量
	unsigned int mMinJie;		// 敏捷
	unsigned int mZhiLi;		// 智力
	unsigned int mTiNeng;		// 体能 
	//体力
	unsigned int vit_current;

	unsigned int mHP;			// 生命值,

	unsigned short resonance_star; //装备强化星级
	
	unsigned int mBaoJiLv;		// 暴击率
	unsigned int mBaoJiShangHaiYinZi;	// 暴击的伤害因子，比如150%，就是攻击的150
	unsigned int mJiNengFaChuLv;	// 技能发出概率

	// 固有属性，职业定了，这个就定了。
	int mShangHaiLeiXing;	// 伤害类型,默认是SHANG_HAI_MO_FA
	float mGongJiJuLi;		// 攻击距离
	string baseName;		// 就是基础的名字，比如野蛮人就是br***,用于找跟他相关的图片用的
	string nameInChinese;	// 中文名字，比如野蛮人，就是野蛮人


	HeroBaseProperty();
	HeroBaseProperty(const HeroDetail* detail);

	//刷新数据，只能刷新非基础信息的数据
	//因为要检验基础信息的一致性，避免乱调用，产生bug
	void refeshPro(const HeroDetail* detail);

	// 初始话固定的属性
	void initFixedProValue(HeroFixedPro data);
};

typedef HeroBaseProperty BaseActorProperty;