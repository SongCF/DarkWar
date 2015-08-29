#pragma once
#include <string>
#include <vector>
#include <string.h>

struct ZhuangbeiDetail;
struct EquipData;
struct ItemInfo;

#define TEN_TIME_DISCOUNT 0.8f	// 打折

//角色初始带装模板id
#define ZhuangBeiInitNameId_Jian   1
#define ZhuangBeiInitGroupId_Jian   1

#define ZhuangBeiInitNameId_DunPai 5
#define ZhuangBeiInitGroupId_DunPai 5

#define ZhuangBeiInitNameId_FazhangShangshou 7
#define ZhuangBeiInitGroupId_FazhangShangshou 7

#define ZhuangBeiInitNameId_Gong 8
#define ZhuangBeiInitGroupId_Gong 8

//特定groupID装备////////////////////////////////////////////
//初始掉装轻盔
#define ZhuangBei_FirstDiaoluo_NameId_qingkuijia 10
#define ZhuangBei_FirstDiaoluo_GroupId_qingkuijia 10

//初始掉装重盔
#define ZhuangBei_FirstDiaoluo_NameId_zhongkuijia 11
#define ZhuangBei_FirstDiaoluo_GroupId_zhongkuijia 11

//初始带孔轻盔
#define ZhuangBei_FirstDaikong_NameId_qingkuijia 12
#define ZhuangBei_FirstDaikong_GroupId_qingkuijia 12

//初始带孔重盔
#define ZhuangBei_FirstDaikong_NameId_zhongkuijia 13
#define ZhuangBei_FirstDaikong_GroupId_zhongkuijia 13

//初始萃取单手剑
#define ZhuangBei_FirstCuiqu_NameId_jian 14
#define ZhuangBei_FirstCuiqu_GroupId_jian 14

//初始附魔单手剑
#define ZhuangBei_FirstFumo_NameId_jian 15
#define ZhuangBei_FirstFumo_GroupId_jian 15

//第三关boss制定掉落稀有重盔甲
#define ZhuangBei_Task3_NameId_zhongkuijia 16
#define ZhuangBei_Task3_GroupId_zhongkuijia 16

//第四关boss制定掉落稀有剑
#define ZhuangBei_Task4_NameId_jian 17
#define ZhuangBei_Task4_GroupId_jian 17

//特定装的最大ID
//关卡随机掉落时，小于等于此最大ID的装备都不可能掉落
#define ZhuangBeiInitGroupId_Max 1000

//特殊处理装备ID////////////////////////////////////////////
#define SUIT_fashi_level15_faqi_group_id 1411
#define SUIT_cike_level15_kuijia_group_id 1405
#define SUIT_yemanren_level15_kuijia_group_id 1400

#define SUIT_fashi_level30_faqi_group_id 1413
#define SUIT_fashi_level30_kuijia_group_id 1414

#define SUIT_cike_level30_kuijia_group_id 1408
#define SUIT_cike_level30_jiezhi_group_id 1409

#define SUIT_yemanren_level30_kuijia_group_id 1402
#define SUIT_yemanren_level30_jiezhi_group_id 1403

//特殊处理装备ID end////////////////////////////////////////////


//精炼次数
#define jinglian_time_chuanqi_max 3
#define jinglian_time_shenbing_max 9

//精炼几阶之后可以篆刻经文
#define JingwenZhuanke_need_JinglianLevel 5

//开放进度
#define Level_limit_arena 8
//守城开放等级限制
#define Level_limit_cityguard 20
//宝石合成与镶嵌等级限制
#define Level_limit_baoshi 15
//英雄榜开放等级限制
#define Level_limit_topherolist 11
//巅峰对决活动开放等级
#define Level_limit_toparena 25

//巅峰对决参与选手最小身价
#define MIN_HERO_VALUE 160000

//巅峰对决参与选手最大身价
#define MAX_HERO_VALUE 90000000

//每天挑战传奇录最大次数
#define MAX_CHALLENGE_LEGEND_NOTE_TIMES 5
//传奇录开放等级
#define LEVEL_LIMIT_LEGEND_NOTE 5

//血色教堂自我挑战 每闯过一关获得能量数
#define PASS_SELFCHLG_CHECKPOINT_ENERGY	15

//背包最大的未装备数
#define MAX_COUNTS_IN_KNAPSACK 100

//内部邮件最大id数
#define MAX_INTERNAL_EMAIL_ID 10000




// VIP特权
struct VipPrivilege
{
	//unsigned int vipLevel;					//VIP等级
	unsigned int MaxBuyTiLiTime;			//体力购买次数
	unsigned int MaxSaoDangTime;			//副本扫荡次数
	unsigned int MaxBuyArenaChallengeTime;		//可购买竞技场挑战次数
	unsigned int JumpGridNum;				//关卡可跳过小怪数
	// 下面的先不管
	bool enableFastRongYuDuiHuan;			// 开启荣誉快速兑换功能
	unsigned int freeRefreshTime;			// 免费刷新悬赏任务次数

	VipPrivilege()
	{
		memset(this,0,sizeof(VipPrivilege));
	}
};


//洗炼等级
enum PurifyLevel {
	PurifyLevel_Error			= 0,	//错误	
	PurifyLevel_IronStar		= 1,	//铁星	
	PurifyLevel_CuprumStar    	= 2,	//铜星	
	PurifyLevel_SilverStar		= 3,	//银星
	PurifyLevel_GoldStar		= 4,	//金星
	PurifyLevel_YellowDiamond	= 5,	//黄钻	
	PurifyLevel_GreenDiamond	= 6,	//绿钻	
	PurifyLevel_PurpleDiamond	= 7,	//紫钻	
};


//等级奖励ID 对应level_bonus_temp这张表的奖励ID
enum LevelBonusID
{		
	LevelBonusID_5 = 5, 		//5级
	LevelBonusID_10 = 10, 		//10级
	LevelBonusID_15 = 15, 		//15级
	LevelBonusID_20 = 20, 		//20级
	LevelBonusID_25 = 25, 		//25级
	LevelBonusID_30 = 30, 		//30级
};

//系统奖励ID 对应system_bonus_temp这张表的奖励ID
enum SystemBonusID
{		
	/* 日常任务 */	
	BonusID_Killed_Boss = 1001, 		//击杀任意一个关卡BOSS
	BonusID_Random_Arena = 1002,		//参与一次随机竞技挑战
	BonusID_VarietyStore = 1003,		//杂货铺翻牌
	BonusID_Intensify_GoldStar = 1004,	//强化出一颗金星

	/* 周常任务 */
	BonusID_Discover = 1005,			//前往寻宝并且点亮一个封印石碑
	BonusID_TopHeroList = 1006,			//参与一次英雄榜挑战
	BonusID_SelfChallenge = 1007,		//血色教堂通过50关
};


namespace CS
{
#ifdef  _CLIENT_
	//是否还能镶嵌宝石
	bool canZBXiangqianBaoshi(unsigned long long eqId);
#endif

	// 将网络字节序的zhuangbei_detail数组转换成equipdata数组
	// 返回的是zhuangbe_detail的末尾，就是新的开始的数据的地址
	char* ntoh_zhuangbei_detail_2_equipdata(ZhuangbeiDetail* detail_arr,int count,std::vector<EquipData>& out);
	char* hton_equipdata_2_zhuangbei_detail(const std::vector<EquipData>& equips,ZhuangbeiDetail* detail_arr_out);

	// 获得职业的血量和攻击加成
	// 这是额外的加成，是在一切都计算完毕了之后再加上的百分比
	void getProfesionBloodAndGongJiAddPercent(unsigned int profession, int& bloodAdd,int& gongjiAdd);

	// 获得主角的攻击距离
	float getHeroGongjiJuLiByProfession(unsigned int profession);

	// 计算强化装备需要的物品
	unsigned int ItemsNeedForQianghuaZhuangbei(unsigned int zhuangbei_level, unsigned int zhuangbei_colour);

	// 强化装备需要的银币
	unsigned int QianghuaZhuangbeiNeedSilver(unsigned int zhuangbei_level, unsigned int zhuangbei_colour);
    unsigned int HuituiZhuangbeiNeedSilver(unsigned int zhuangbei_level, unsigned int zhuangbei_colour);

	// 回退装备所需金币 	retain_count:保留个数
    unsigned int HuituiZhuangbeiNeedGold(unsigned int retain_count);


	// 能否可被强化
	bool canZBTypeBeQianghua(unsigned int zb_type);

	//拆解某种装备需要的银币
	unsigned int ChaijieZhuangbeiNeedSilver(unsigned int zhuangbei_level, unsigned int zhuangbei_colour);
	
	//拆解某种装备获得的物品
	void getItemsForChaijieZhuangbei(unsigned int zhuangbei_level, unsigned int zhuangbei_colour,
		std::vector<ItemInfo> &item_vec);
	
	// 合成宝石需要的金币
	unsigned int HechengBaoshiNeedGold(unsigned int baoshi_type);
	// 合成宝石需要的宝石类型
	unsigned int HechengBaoshiNeedBaoshiType(unsigned int baoshi_type);
	// 合成宝石需要的宝石的个数，即HechengBaoshiNeedBaoshiType的个数
	unsigned int HechengBaoshiNeedBaoshiCount(unsigned int baoshi_type);
	// 某种类型的宝石是否可以被合成
	bool canBaoShiBeHeCheng(unsigned int baoshi_type);

	// 镶一个某种宝石需要的银币
	unsigned int XiangbaoshiNeedSilver(unsigned int zb_level, unsigned int baoshi_type);

	// 钻孔需要的金币
	unsigned int ZhuangkongNeedGold(
		unsigned int zhuangbei_level, unsigned int zhuangbei_colour);
	// 是否还可以钻孔
	bool canZBTypeBeZhuangkong(unsigned int zhuangbei_type,unsigned int hole_count);

	// 是否
	bool canEquipToHero(unsigned int zhuangbei_type,unsigned int profession_id);

	// ItemsType_Baoshi_lv_1 到 ItemsType_Baoshi_huang_10
	// 对力量，敏捷，智力，体能的加成值
	void getBaoShiJiaCheng(unsigned int baoShiType,unsigned int& liLiang,unsigned int& minJie,unsigned int& zhiLi,unsigned int& tiNeng);


    //每个类型魔法精华的属性数值范围
    void getMofaJinghuaAttrValue(int zb_type, int zttr_type, unsigned int mfjh_level
        , unsigned int& value_min, unsigned int& value_max);
    //装备类型是否能萃取某个属性
    bool isAttrTypeZBTypeCanCuiqu(unsigned int zb_type, unsigned int attr_type);
    
	// 获取技能的固有数据：周期和触发点
	int getSkillZhouQi(unsigned int skill_type);
	int getSkillTurn(unsigned int skill_type);
	//int getSkillResult(unsigned int skill_type);

#if _CLIENT_ 
	//获得宝石名称
	std::string getBaoshiName(unsigned int baoshiType);
	std::string getBaoshiShuxing(unsigned int baoshi_type);
	std::string getItemName(unsigned int item_type);
	std::string getItemShuxing(unsigned int item_type);
	//获取装备类型名称
	std::string getZhuangbeiTypeName(unsigned int zb_type, unsigned int zb_color);
	std::string getZhuangbeiTypeName(unsigned int zb_type);
	std::string getZhuangbeiChengSeName(unsigned int zb_color);
	//获取魔法精华的名字   四级剑之魔法精华
	std::string getMofajinghuaName(unsigned int zb_type, unsigned int level);
#endif

	// 获得装备的类型
	//	EquipType_WuQi = 0,
	//  EquipType_FangJu,
	//  EquipType_ShiPin,
	unsigned int getEquipType(unsigned int type);

	// 是否是双手武器
	bool isShuangShouWuQi(unsigned int type);

    unsigned int ShenbingJiefengyinNeedSilver();
    unsigned int ShenbingJiefengyinNeedItemCount();
    unsigned int ShenbingJiandingNeedSilver(unsigned int identifiy_count);
    unsigned int ShenbingJiandingNeedItemCount(unsigned int identifiy_count);
    unsigned int ShenbingUpgradeNeedSilver(unsigned int shenbbing_level);
    unsigned int ShenbingUpgradeNeedItemCount(unsigned int shenbbing_level);
    unsigned int ZBJinglianNeedSilver(unsigned int color, unsigned int jinglian_level);
    unsigned int ShenbingZhuankeJingwenNeedSilver();
    unsigned int ShenbingZhuankeJingwenNeedItem();
    unsigned int AssistantUnLockNeedHonor(unsigned int locked_count);

	// 神兵的等级，按升序排列
	std::vector<int> getShenBingLevels();
	// 获取神兵的下一个等级
	// 没有/满级了 就返回-1
	int getNextShenBingLevel(unsigned int curLevel);

	//获取可以解锁助手的英雄等级
	int getUnlockAbleHeroLevel();

    //返回副本中棋盘的格子布局
    void getTaskChessboardFormat(unsigned int task_id
        , unsigned int& grid_count_heng, unsigned int& grid_count_shu);

 //   //竞技场挑战各VIP等级每日最多可购买次数
 //   unsigned int getVIPArenaChallengeMaxBuyTime(unsigned int vip_level);
	//// 获取某个等级的Vip数据
	//VipPrivilege getVipPrivilege(unsigned int vip_level);

    //竞技场购买一次挑战所需要的金币
    unsigned int arenaBuyOneChlgTimeNeedGold();

    //杂货铺购买一次多少金币
    unsigned int zahuopuGetOneItemNeedGold(unsigned int coupons_balance);

    //升级需要多少升级道具
    unsigned int heroLevelUpNeedItemCount(unsigned int level);
    unsigned int heroLevelUpNeedSilver(unsigned int level);

    //英雄升阶需要多少东西
    unsigned int heroUpgradeNeedItemCount(unsigned int stage);
    unsigned int heroUpgradeNeedSilver(unsigned int stage);

    //元神升阶需要多少银币
    unsigned int yuanshenUpgradeNeedSilver(unsigned int stage);

	//各等级的元神都开启了哪些装备槽位
    void openZhuangbeiOfYuanshenLevel(unsigned int yuanshen_level
        , bool& wuqi_open, bool& fushou_open, bool& kuijia_open
        , bool& hushenfu_open, bool& jiezhi_open);
    //各等级元神对应的转化率
    unsigned int transferRatioOfYuanshenLevel(unsigned int yuanshen_level);

	// 获取不同等级的技能的效果加成
	unsigned int getSkillEffectRatioAddBySkillLevel(unsigned int skillLevel);

	// 获取某种技能的攻击效果是普通攻击的百分之多少
	// 是一个普适结果。
	// 比如返回200，就是技能攻击时普攻的2倍
	unsigned int getSkillEffectRatioOfNormalAttack(unsigned int skillType);


    //角色升阶对角色整体攻防血加成效果
    unsigned int getHeroStageEffectRatio(unsigned int stage);

    //技能升级需要扣多少银币
    unsigned int skillLevelUpNeedSilver(unsigned int new_skill_level);

    //
    void churchLevelUpNeedItemCount(unsigned int new_church_level
        ,unsigned int& dalishi_need,unsigned int& huagangyan_need
        ,unsigned int& mocai_need,unsigned int& shuijinshi_need);
    
    unsigned int churchLevelUpNeedSilver(unsigned int new_church_level);

	//击杀小怪对面奖励需要的击杀数量 type: 1: 普通小怪, 2: 精英怪
    unsigned int cashBonusNeedKilledMonsters(unsigned int type);

	//巅峰对决竞猜赌注 type<TopArenaWagerType>: 1: 低赌注, 2: 高赌注
    unsigned int getTopArenaWager(unsigned int type);

	//铸银所需金币数 times:铸造次数
    unsigned int castSilverNeedGold(unsigned int times);

	//铸银所得银币数 level:主英雄等级
    unsigned int getCastSilverCount(unsigned int level);

	//刷新竞技场所需货币 times:刷新次数
    void refreshArenaChlgListNeedItem(unsigned int times, ItemInfo &item);
 
	//血色教堂自我挑战难度 level: 主英雄等级; 返回: SelfChlgDiffType
    unsigned int getSelfChlgDifficulty(unsigned int level);

	//血色教堂加成 type: SelfChlgGainType; degree: SelfChlgGainDegree
	//expend:消耗能量多少; gain:加成比例
    void getSelfChlgGain(unsigned int type, unsigned int degree, 
    	unsigned int &expend, unsigned int &gain);

	//守护之灵奖励 chapter: 章节id
    void getProtectSpiritBonus(unsigned int chapter, unsigned int defend_days, std::vector<ItemInfo> &item_vec);

	//膜拜守护之灵材料 level: 主英雄等级
    void worshiProtectSpirit(unsigned int level, std::vector<ItemInfo> &item_vec);

	//获取守护之灵的章节 level: 主英雄等级
    unsigned int getProtectSpiritChapter(unsigned int level);
	
	//获取探索次数的上限 vip_level: VIP等级
    unsigned int getDiscoverLimitTimes(unsigned int vip_level);

	//清除英雄榜冷却时间消耗金币数
    unsigned int clearTopHeroCDTimeNeedGold();

	// 获取守城，跳过需要的金币
	unsigned int getCityGuardBattleSkipCDNeedGold();
	// 获取守城战斗完毕之后需要等待的CD时间(秒)
	unsigned int getCityGuardBattleEndCDTime();

    //守城，击杀boss奖励
    void getCityGuardKillBossReward(ItemInfo& item);
    //守城，伤害输出奖励
    void getCityGuardHeroShanghaiOrderReward(
        ItemInfo& item_order_1
        , ItemInfo& item_order_2
        , ItemInfo& item_order_3
        , ItemInfo& item_order_4_10
        , ItemInfo& item_order_11_20);
    //守城，幸运奖励
    void getCityGuardLuckyReward(ItemInfo& item);
	
    //守城，参与奖
    void getCityGuardJoinReward(ItemInfo& item);

	//精炼最大次数 color:装备成色, level:装备等级
    unsigned int getJingLianMaxTimes(unsigned int color, unsigned int level);
	
	/* 洗炼 */
	//洗炼最大孔数 type:装备类型, color:装备成色, level:装备等级
    unsigned int getPurifyMaxHoles(unsigned int type, unsigned int colour, unsigned int level);
	//洗炼星级 	value:数值
    PurifyLevel getPurifyLevel(unsigned int value);
	//洗炼上限星级 	color:装备成色, level:装备等级, upper_limit:上限数值
    PurifyLevel getPurifyUpperLimitLevel(unsigned int colour, unsigned int level, unsigned int &upper_limit);
}

