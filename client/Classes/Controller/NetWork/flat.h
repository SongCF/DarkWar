/**
 *	说明，这文件定义的是协议文件的另一个表述形式，这里的数据比协议的数据好用，方便，包括一些发送的命令和接收的命令。
 *	所有的数据已经是展开的，不用在用的时候去移动指针来实现，这里都处理好的。
 *	当使用的地方，监听的消息是*****_F的形式，那么这个命令的返回的数据就是Flat_Msg_Rsp,里面的data就是这里面定义的结构体的指针
 */

#pragma once
#include "protocol.h"
#include <vector>
#include <algorithm> 
#include "FightScriptDefine.h"
#include "LFUtile.h"

using namespace std;


//装备属性配置key字符串
#define		ZB_Key_gailv				"gailv"
#define		ZB_Key_count				"count"
#define		ZB_Key_kangxing				"kx"
#define		ZB_Key_mingjie				"mj"
#define		ZB_Key_zhili				"zl"
#define		ZB_Key_liliang				"ll"
#define		ZB_Key_tineng				"tn"
#define		ZB_Key_baoji				"bj"
#define		ZB_Key_baoshang				"bs"
#define		ZB_Key_hp_ration			"hp"
#define		ZB_Key_gedanglv				"gdlv"
#define		ZB_Key_gedangzhi			"gdzi"
#define		ZB_Key_gongji_min_add		"gjmin"
#define		ZB_Key_gongji_max_add		"gjmax"
#define		ZB_Key_skill_ratio_add		"sklrt"
#define		ZB_Key_hujia_add			"hujia"
#define		ZB_Key_skill_zhouqi			"sklzq"
#define		ZB_Key_skill_turn			"skltn"
#define		ZB_Key_skill_result			"sklst"
#define		ZB_Key_gongji_add_ration	"gjadd"		// 角色整体的攻击加成，基础攻击，装备攻击都算完毕之后再算
#define		ZB_Key_hp_add_ratio			"hpadd"		// 角色整体的血量加成，基础血量，装备血量都算完毕之后再算


#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
#pragma pack(1) //按照单字节对齐,跟协议要一致
#endif

//gailv=50,count=1,bj=5~10,bs=20~30,gjmin=3~5,gjmax=3~5
struct AttrbuteItem
{
	int gailv;
	int count;

	int kangxing_min;
	int mingjie_min;
	int zhili_min;
	int liliang_min;
	int tineng_min;
	int baoji_min;
	int baoshang_min;
	int hp_tation_min;
	int gedanglv_min;
	int gedangzhi_min;
	int gongji_min_add_min;
	int gongji_max_add_min;
	int hujia_add_min;
	int skill_zhouqi_sub_min;                // 减少几个技能周期
	int skill_turn_sub_min;                  // 提前几个触发点
	int skill_result_add_percent_min;
	int gongji_add_ratio_min;
	int hp_add_ratio_min;


	int kangxing_max;
	int mingjie_max;
	int zhili_max;
	int liliang_max;
	int tineng_max;
	int baoji_max;
	int baoshang_max;
	int hp_tation_max;
	int gedanglv_max;
	int gedangzhi_max;
	int gongji_min_add_max;
	int gongji_max_add_max;
	int hujia_add_max;
	int skill_zhouqi_sub_max;                // 减少多少周期
	int skill_turn_sub_max;                  // 提前几个触发点
	int skill_result_add_percent_max;
	int gongji_add_ratio_max;
	int hp_add_ratio_max;

	AttrbuteItem(string attriStr)
	{
		memset(this,0,sizeof(AttrbuteItem));
		//gailv=50,count=1,bj=5~10,bs=20~30,gjmin=3~5,gjmax=3~5
		vector<string> tokens = LF::string_split(attriStr,",");
		for (int i = 0; i < (int)tokens.size();i++ )
		{
			// bs=20~30
			vector<string> key_value = LF::string_split(tokens[i],"=");
			if (key_value.size() != 2)
				continue;

			string key = key_value[0];
			key =  LF::trim(key);

			string _value = key_value[1];

			vector<string> temp = LF::string_split(_value,"~");
			if (key == ZB_Key_gailv)
			{
				gailv = atoi(_value.c_str());
			}
			else if (key == ZB_Key_count)
			{
				count = atoi(_value.c_str());
			}
			else if (key == ZB_Key_kangxing)
			{
				kangxing_min = atoi(temp[0].c_str());
				kangxing_max = kangxing_min;
				if (temp.size() >= 2)
					kangxing_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_mingjie)
			{
				mingjie_min = atoi(temp[0].c_str());
				mingjie_max = mingjie_min;
				if (temp.size() >= 2)
					mingjie_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_zhili)
			{
				zhili_min = atoi(temp[0].c_str());
				zhili_max = zhili_min;
				if (temp.size() >= 2)
					zhili_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_liliang)
			{
				liliang_min = atoi(temp[0].c_str());
				liliang_max = liliang_min;
				if (temp.size() >= 2)
					liliang_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_tineng)
			{
				tineng_min = atoi(temp[0].c_str());
				tineng_max = tineng_min;
				if (temp.size() >= 2)
					tineng_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_baoji)
			{
				baoji_min = atoi(temp[0].c_str());
				baoji_max = baoji_min;
				if (temp.size() >= 2)
					baoji_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_baoshang)
			{
				baoshang_min = atoi(temp[0].c_str());
				baoshang_max = baoshang_min;
				if (temp.size() >= 2)
					baoshang_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_hp_ration)
			{
				hp_tation_min = atoi(temp[0].c_str());
				hp_tation_max = hp_tation_min;
				if (temp.size() >= 2)
					hp_tation_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_gedanglv)
			{
				gedanglv_min = atoi(temp[0].c_str());
				gedanglv_max = gedanglv_min;
				if (temp.size() >= 2)
					gedanglv_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_gedangzhi)
			{
				gedangzhi_min = atoi(temp[0].c_str());
				gedangzhi_max = gedangzhi_min;
				if (temp.size() >= 2)
					gedangzhi_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_gongji_min_add)
			{
				gongji_min_add_min = atoi(temp[0].c_str());
				gongji_min_add_max = gongji_min_add_min;
				if (temp.size() >= 2)
					gongji_min_add_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_gongji_max_add)
			{
				gongji_max_add_min = atoi(temp[0].c_str());
				gongji_max_add_max = gongji_max_add_min;
				if (temp.size() >= 2)
					gongji_max_add_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_skill_ratio_add)
			{
				// 不再支持了
			}
			else if(key == ZB_Key_hujia_add)
			{
				hujia_add_min = atoi(temp[0].c_str());
				hujia_add_max = hujia_add_min;
				if (temp.size() >= 2)
					hujia_add_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_skill_zhouqi)
			{
				skill_zhouqi_sub_min = atoi(temp[0].c_str());
				skill_zhouqi_sub_max = skill_zhouqi_sub_min;
				if (temp.size() >= 2)
					skill_zhouqi_sub_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_skill_turn)
			{
				skill_turn_sub_min = atoi(temp[0].c_str());
				skill_turn_sub_max = skill_turn_sub_min;
				if (temp.size() >= 2)
					skill_turn_sub_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_skill_result)
			{
				skill_result_add_percent_min = atoi(temp[0].c_str());
				skill_result_add_percent_max = skill_result_add_percent_min;
				if (temp.size() >= 2)
					skill_result_add_percent_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_gongji_add_ration)
			{
				gongji_add_ratio_min = atoi(temp[0].c_str());
				gongji_add_ratio_max = gongji_add_ratio_min;
				if (temp.size() >= 2)
					gongji_add_ratio_max = atoi(temp[1].c_str());
			}
			else if(key == ZB_Key_hp_add_ratio)
			{
				hp_add_ratio_min = atoi(temp[0].c_str());
				hp_add_ratio_max = hp_add_ratio_min;
				if (temp.size() >= 2)
					hp_add_ratio_max = atoi(temp[1].c_str());
			}
			else
			{
				CCAssert(false,"未知的key");
			}
		}
	}

	vector<ZBAttr> getZbAttrsIgnoreGaiLvAndCount()
	{
		vector<ZBAttr> ret;

#define ADD_IF_POSSIBLE(MIN_DATA,MAX_DATA,ATTR_TYPE)		\
		if (MIN_DATA > 0)									\
		{													\
			ZBAttr newOne;									\
			newOne.attr_type = ATTR_TYPE;					\
			newOne.attr_value = (MIN_DATA+MAX_DATA)/2.0f;	\
			ret.push_back(newOne);							\
		}

		ADD_IF_POSSIBLE(kangxing_min,kangxing_max,AttrType_kangxing);
		ADD_IF_POSSIBLE(mingjie_min,mingjie_max,AttrType_mingjie);
		ADD_IF_POSSIBLE(zhili_min,zhili_max,AttrType_zhili);
		ADD_IF_POSSIBLE(liliang_min,liliang_max,AttrType_liliang);
		ADD_IF_POSSIBLE(tineng_min,tineng_max,AttrType_tineng);
		ADD_IF_POSSIBLE(baoji_min,baoji_max,AttrType_baoji);
		ADD_IF_POSSIBLE(baoshang_min,baoshang_max,AttrType_baoshang);
		ADD_IF_POSSIBLE(hp_tation_min,hp_tation_max,AttrType_hp_percent);
		ADD_IF_POSSIBLE(gedanglv_min,gedanglv_max,AttrType_gedanglv);
		ADD_IF_POSSIBLE(gedangzhi_min,gedangzhi_max,AttrType_gedangzhi);
		ADD_IF_POSSIBLE(gongji_min_add_min,gongji_min_add_max,AttrType_gongji_min);
		ADD_IF_POSSIBLE(gongji_max_add_min,gongji_max_add_max,AttrType_gongji_max);
		ADD_IF_POSSIBLE(hujia_add_min,hujia_add_max,AttrType_hujia_add);
		ADD_IF_POSSIBLE(skill_zhouqi_sub_min,skill_zhouqi_sub_max,AttrType_skillzhouqi);
		ADD_IF_POSSIBLE(skill_turn_sub_min,skill_turn_sub_max,AttrType_skillturn);
		ADD_IF_POSSIBLE(skill_result_add_percent_min,skill_result_add_percent_max,AttrType_skillresult);
		ADD_IF_POSSIBLE(gongji_add_ratio_min,gongji_add_ratio_max,AttrType_actor_gongji_addratio);
		ADD_IF_POSSIBLE(hp_add_ratio_min,hp_add_ratio_max,AttrType_actor_hp_addratio);

		return ret;
	}
};

struct Flat_GetZhuangbeiInTeamRsp
{
	int err_code;
	vector<EquipData> equips;
};


struct Flat_ChangeTaskStatRsp
{
	int err_code;

	//如果是提交主线任务，则会有任务奖励。否则为0
	unsigned int exp;
	unsigned int silver;
	//
	vector<HeroBaseProperty> hero_new_attri_vec;
	vector<HeroBaseProperty> hero_old_attri_vec;
};


// SPCmd_ArenaEnterRsp
struct Flat_ArenaEnterRsp
{
	int err_code;
	//进入竞技场时，房间已有的人
	vector<HeroInfo> already_waited;
	vector<EquipData> allEquips;

	vector<EquipData> getEquipsByHeroId(int heroId)
	{
		vector<EquipData> ret;
		int num = allEquips.size();
		for (int i=0;i<num;i++)
		{
			if (allEquips[i].zhuangbei_hero_id == heroId)
			{
				ret.push_back(allEquips[i]);
			}
		}
		return ret;
	}
};

struct Flat_ArenaUpdateRoomRsp
{
	int err_code;
	HeroInfo new_hero_info;
	//主角所带装备
	vector<EquipData> equipsOnHero;
};


// 对应SPCmd_ArenaGetBattleScriptGiftRsp
struct Flat_ArenaGetBattleScriptGiftRsp
{
	int err_code;

	unsigned int battle_time_remainder;
	//战局信息
	unsigned int first_turn_hero_id_arr[4]; //0与1一组，2与3一组
	unsigned int second_turn_hero_id_arr[2];

	vector<GiftJifen> gettedData;	// 得到的宝箱
	vector<GiftJifen> missedData;	// 没有得到的宝箱

	vector<FightScript> scriptVec;	// 战斗的剧本，一场一个剧本
};

//struct Flat_TeamBossEnterRsp
//{
//	int err_code;
//
//	//进入团队副本时，房间已有的人
//	vector<HeroInfo> already_waited;
//};

// Flat_TeamBossEnterRsp 跟 Flat_ArenaEnterRsp一样的
typedef Flat_ArenaEnterRsp Flat_TeamBossEnterRsp;

// Flat_TeamBossUpdateRoomRsp 跟 Flat_ArenaUpdateRoomRsp一样的
typedef Flat_ArenaUpdateRoomRsp Flat_TeamBossUpdateRoomRsp;

struct Flat_TeamBossGetBattleScriptGiftRsp
{
	int err_code;

	//结算
	TeamBossScoreData score_arr[4];

	vector<GiftData> gettedData;	// 得到的宝箱
	vector<GiftData> missedData;	// 没有得到的宝箱

	// 脚本数据
	FightScript fightScript;
};



struct Flat_CityGuardFightRsp
{
	int err_code;
	//如果选择退却或者战败，则这里是结算数据，否则这里的数据无效。
	//实际得到的银币。
	unsigned int silver;

	//标示玩家是否选择了迎战
	bool isFighting;
	//如果isFighting为true则这里存放战斗脚本
	FightScript fightScript;
};


struct Flat_QueryAnotherTeamInfoRsp
{
	int err_code;
	//上阵英雄信息,布阵就是按顺序来的，1,2,3,4
	//如果id为0，就是没有人
	HeroDetail hero_arr[4];
	//上阵英雄所装备的技能
	SkillInfo skill_arr[4];
	vector<EquipData> zhuangbei_vec;
};

inline bool sortByOrderAsc(const HeroAndOrder& a,const HeroAndOrder& b)
{
	return a.order < b.order;
}

inline bool sortByOrderDec(const HeroAndOrder& a,const HeroAndOrder& b)
{
	return a.order > b.order;
}

struct Flat_TopHeroGetListRsp
{
	int err_code;

	//当前排名
	unsigned int my_order;

	unsigned int next_chlg_time; //下一场挑战的时间戳(PS:当前时间>=此时间戳时皆可挑战 反之不能挑战)

	//top30
	vector<HeroAndOrder> top30_hero;
	//前后各若干名
	vector<HeroAndOrder> hero_list_surrounding;
	// 战报
	vector<TopHeroReport> reports;
	
	// 把hero_list_surrounding 排序
	// 默认是按照order降序排列
	void sort_hero_list_surround(bool _dec = true)
	{
		if (_dec)
		{
			std::sort(hero_list_surrounding.begin(),hero_list_surrounding.end(),sortByOrderDec);
		}
		else
		{
			std::sort(hero_list_surrounding.begin(),hero_list_surrounding.end(),sortByOrderAsc);
		}
		
	}
};

struct Flat_TopHeroChallengeRsp
{
	int err_code;

	//战斗之后的排名
	unsigned int order_after_fight;
	unsigned int nextChallengeTime; //挑战失败才有效

	FightScript fightScript;
};


struct Flat_SystemGiftQueryRsp
{
	int err_code;
	vector<systemGift> gift_arr;
};

struct Flat_DuanzaoRsp
{
	int err_code;
	vector<EquipData> equips;
}; 

struct Flat_ZBChaijieRsp
{
	int err_code;

	//拆解获得item数量
	vector<ItemInfo> item_arr;
	//还的材料
	vector<ItemInfo> re_item_arr;
};

struct Flat_DuanzaoGroupZBInfo
{
	unsigned int zb_group_id;
	ZhuangbeiType zhuangbei_type;
	string zhuangbei_name;

	ZhuangbeiColour zhuangbei_colour;
	string zhuangbei_pic;
	unsigned int level;
	unsigned int ratio;
	unsigned int next_group_id;	//下一等级的group_id, 为0是最高等级
	unsigned int need_expend_group_id;	//升级到需要的传奇装备group_id, 为0是最高等级

	unsigned int profession_id;    		//绑定的职业ID, 为0是任何职业

	unsigned int gongji_min_min;
	unsigned int gongji_min_max;
	unsigned int gongji_max_min;
	unsigned int gongji_max_max;
	unsigned int wufang_min;
	unsigned int wufang_max;
	unsigned int hole1_percent;
	unsigned int hole2_percent;
	unsigned int hole3_percent;

	vector<AttrbuteItem> itemVec;// 属性的数据
	string zhuangbei_desc;
};
struct Flat_DuanzaoGroupZBQueryRsp
{
	int err_code;
	vector<Flat_DuanzaoGroupZBInfo> zbArray;
};

struct Flat_FubenDiaoluoZBInfo
{
	unsigned int group_id;
	bool is_suipian;
	unsigned int zhuangbei_type;
	string zhuangbei_name;
	unsigned int zhuangbei_colour;
	string zhuangbei_pic;
	unsigned int level;

	unsigned int gongji_min_min;
	unsigned int gongji_min_max;
	unsigned int gongji_max_min;
	unsigned int gongji_max_max;
	unsigned int wufang_min;
	unsigned int wufang_max;
	unsigned int hole1_percent;
	unsigned int hole2_percent;
	unsigned int hole3_percent;

	vector<AttrbuteItem> itemVec;// 属性的数据
	string desc;	// 装备描述
};

struct Flat_QueryFubenDiaoluoRsp
{ 
	int err_code;
	vector<ItemsType> items; // 掉落的Item，主要是技能书
	vector<Flat_FubenDiaoluoZBInfo> diaoluo_vec;
};


struct Flat_NewAssistantJoinRsp
{
	int err_code;

	//
	HeroDetail hero_detail;

	SkillInfo skillInfor;

	vector<EquipData> equips;
};


struct Flat_SubTaskInfor
{
	unsigned int task_id;
	SubTaskType task_type;
	ItemsType reward_item_id;
	unsigned int reward_item_count;
	string des;
};
struct Flat_QuerySubTaskListRsp
{
	int err_code;
	vector<Flat_SubTaskInfor> taskes;
};

struct Flat_QueryMainTaskRewardRsp
{
	int err_code;
	vector<main_task_reward> taskRewardVec;
};

typedef Flat_DuanzaoGroupZBQueryRsp Flat_QueryAllShenbingListRsp;


struct Flat_ShenbingHechengRsp
{
	int err_code;
	vector<EquipData> zhuangbei_vec;
};


struct Flat_SaodangRsp
{
	int err_code;

	unsigned int exp;				//经验
	unsigned int decomposed_count; 	//已分解的装备数量
	//道具
	vector<ItemInfo> items;
	//装备掉落
	vector<EquipData> equips;
};


struct Flat_QueryArenaMarketRsp
{
	int err_code;
	vector<ArenaMarketItem> item_list;
};

struct Flat_RefreshArenaChallengeListRsp
{
	int err_code;
	unsigned int gold_balance; 		//当前金币余额
	unsigned int silver_balance; 	//当前银币余额
	//下次手动刷新列表需要消耗金币/银币数量。type为0就免费
	ItemInfo item_need;
	vector<ArenaChlgerHeroInfo> hero_list;
};

struct Flat_QueryArenaChallengeListRsp
{
	int err_code;
	//下次手动刷新列表需要消耗金币/银币数量。type为0就免费
	ItemInfo item_need;
	vector<ArenaChlgerHeroInfo> hero_list;
};


struct Flat_ArenaChallengeRsp
{
	int err_code;
	//战斗之后的积分
	unsigned int jifen_after_fight;
	//战斗脚本
	FightScript fightScript;
};

struct Flat_ZahuopuGetOneItemRsp
{
	int err_code;

	//本次抽取后金币余额
	unsigned int gold_balance;
	unsigned int coupons_balance;			//当前剩余的代购劵数量

	// index就是item的索引,界面只需要ZahuopuHelper::getHelper()->getItem
	unsigned int get_index;
};

struct Flat_HellTaskBattleRsp
{
	int err_code;
	vector<FightScript> fightScriptVec;
};


struct Flat_QueryYuanshenOfHeroRsp
{
	int err_code;

	//
	HeroDetail hero_detail;
	vector<EquipData> equips;
};

//struct Flat_GetZhuangbeiAlbumRsp
//{
//	int err_code;
//	vector<Flat_DuanzaoGroupZBInfo> zbArray;
//};

struct Flat_GetZhuangbeiAlbumRsp
{
	int err_code;

	//传奇套装的数据，几件套就有几个数据
	//都只是5级.
	vector<Flat_DuanzaoGroupZBInfo> chuanqiSuitData;

	// 单件稀有
	vector<Flat_DuanzaoGroupZBInfo> xiYouVec;
};


struct Flat_CityGuardProcessNotify
{
	int err_code;

	//客户端需要用多少秒来播放伤害过程。
	unsigned int play_time; 

	//在play_time时间段内boss的血量需要减少这么多
	unsigned int boss_hp_start; //为0表示boss已被击杀
	unsigned int boss_hp_end;

	//需要播放的伤害
	vector<CityGuardAnotherHeroShanghai> shanghaiVec;
};



struct Flat_CityGuardBattleData
{
	//是否boss已被击杀了，如果已被击杀，则后面的战斗脚本无效。
	bool is_boss_dead;

	//本次战斗后的个人统计数据
	unsigned int total_shanghai; //对boss的总伤害
	unsigned int shanghai_order; //当前伤害排名

	FightScript fightScript;
};

struct Flat_cityGuardBattleRsp
{
	int err_code;

	//跳过冷却时间消耗金币后的剩余金币
	//如果进入战斗时boss已死，则不会扣金币
	unsigned int gold_balance;

	Flat_CityGuardBattleData battle_data;
};

//struct Flat_cityGuardBattleSkipCDRsp
//{
//	int err_code;
//
//	//跳过冷却时间消耗金币后的剩余金币
//	//如果进入战斗时boss已死，则不会扣金币
//	unsigned int gold_balance;
//
//	Flat_CityGuardBattleData battle_data;
//};

struct Flat_CityGuardReport
{
	unsigned int cityguard_total_time; //守城总耗时

	HeroInfo hero_kill_boss; //击杀boss者
	ItemInfo hero_kill_reward; //击杀者奖励
	vector<HeroInfo> hero_order;
	vector<ItemInfo> hero_order_reward;
	vector<HeroInfo> hero_lucky; //10名幸运奖
	ItemInfo hero_lucky_reward; //幸运奖奖品
};

struct Flat_getCityGuardLastDayReportRsp
{
	int err_code;

	//是否有我的奖励。
	bool have_reward;

	Flat_CityGuardReport lastday_report;

};



struct Flat_BonusInfo
{
	BonusType type;
	bool is_get;		   //是否领取次奖励 false:未领取, true:已领取
	bool enable;		   //是否可以领取奖励 false:不能领取 true:可以领取
	/*ps: bonus_id 的 type 分别为
		BonusType_DailyLogin: 领取日期
		BonusType_Level: 等级*/
	unsigned int bonus_id; 

	//gift
	unsigned int vitality;		//0:没有体力信息 >0: 补偿的体力值
	vector<ItemInfo> itemVec;
	vector<ItemSuipianInfo> itemDebrisVec;
	vector<ShenbingSuipianInfo> equipDebrisVec;
	unsigned int ran_zb_count;	//随机装备数量
	vector<Flat_DuanzaoGroupZBInfo> equipVec;
	vector<GiftBag> giftBagVec;

	//type为BonusType_DayWelfare或BonusType_Reward 有以下类容
	unsigned int vip_level;
	unsigned int multiple;
	string description;

	Flat_BonusInfo(){
		type = BonusType_Invalid;
		is_get = false;		
		enable = false;		
		bonus_id = 0; 
		vitality = 0;		//0:没有体力信息 >0: 补偿的体力值
		ran_zb_count = 0;	//随机装备数量
		vip_level = 0;
		multiple = 0;
	}
};
struct Flat_QueryBonusRsp
{
	unsigned int err_code;
	vector<Flat_BonusInfo> bonus_arr;
};

// typedef QueryBonusRsp Flat_QueryDailyLoginBonusRsp;
// typedef QueryBonusRsp Flat_QueryLevelBonusRsp;
// typedef QueryBonusRsp Flat_QuerySystemBonusRsp;


struct Flat_GetBonusRsp {
	unsigned int err_code;
	BonusType type;		//BonusType
	unsigned int id;		//领取id
	vector<EquipData> gettedEquip;
};
// typedef Flat_GetBonusRsp Flat_GetDailyLoginBonusRsp;
// typedef Flat_GetBonusRsp Flat_GetLevelBonusRsp;
// typedef Flat_GetBonusRsp Flat_GetSystemBonusRsp;


//查询关卡评级状态返回包
struct Flat_QueryRateTaskRsp {
	unsigned int err_code;

	vector<RateStarInfo> taskRate;	//关卡评级
	vector<RateStarInfo> chapterRate;//章节评级
};

//查询关卡评级状态返回包
struct Flat_QueryRateTaskBonusRsp
{
	unsigned int err_code;

	RateStarType type;

	//item数量 关卡:铜星, 章节:铜星
	unsigned int exp1;		//奖励经验 0:没有此奖励, >0:有经验
	vector<ItemInfo> items1;

	//item数量 关卡:银星, 章节:NULL
	unsigned int exp2;		//奖励经验 0:没有此奖励, >0:有经验
	vector<ItemInfo> items2;

	//item数量 关卡:金星, 章节:金星
	unsigned int exp3;		//奖励经验 0:没有此奖励, >0:有经验
	vector<ItemInfo> items3;
	Flat_QueryRateTaskBonusRsp(){
		type = RateStarType_Invalid;
		exp1 = 0;
		exp2 = 0;
		exp3 = 0;
	}
};


struct Flat_UpgradeShenbingRsp {
	unsigned int err_code;
	//回收材料
	vector<ItemInfo> item_arr;
	// 
	EquipData equipNewData;
};



struct Flat_QueryChargePacketRsp
{
	int err_code;

	unsigned int next_vip_level;	//下一VIP等级 
	unsigned int next_need_charge;	//到达下一个等级需要的充值金额 (已是最高级为0)

	bool b_first_charged; 			//首次充值与否 false:未充值过 true:已充值过

	vector<ItemInfo> first_present_items; // 首次回送的Item
	 
	vector<ChargePacketInfo> chargePakets; // 各种充值包

};

struct Flat_QueryGiftActivityRsp {
	unsigned int err_code;
	vector<ActivityInfo> activities;
};

struct Flat_QueryKilledMonsterBonusRsp {
	unsigned int err_code;
	vector<ItemInfo> commonBonus;	// 普通怪奖励
	vector<ItemInfo> eliteBonus;	// 精英怪奖励
};

struct Flat_topArenaReplayRsp
{
	int err_code;
	FightScript fightScript;
};

struct Flat_QueryLegendNoteRsp
{
	int err_code;

	// 根据profession为索引的
	vector<vector<Flat_DuanzaoGroupZBInfo> > all_profession_zhuangbeis;
	unsigned int times;				//剩余挑战次数
};

struct Flat_ChallengeLegendNoteRsp
{
	int err_code;
	FightScript fightScript;
};

//查询自我挑战排名返回包
struct Flat_QuerySelfChallengeOrderRsp
{
	int err_code;
	vector<SelfChlgOrder> order_arr;
};

//自我挑战闯关返回包
struct Flat_SelfChallengeRsp
{
	int err_code;

	unsigned int energy;			//剩余能量
	unsigned int checkpoints;		//关卡
	bool is_have_box;				//true: 每5关有3个宝箱奖励选择; false: 无宝箱
	unsigned int silver;			//>0: 每20关有相应银币奖励; 0:无银币奖励
	GiftBag giftBag;

	FightScript fightScript;
};

struct Flat_ChallengeProtectSpiritRsp
{
	int err_code;
	FightScript fightScript;
};

struct Flat_UnlockDiscoverSealRsp
{
	int err_code;
	vector<EquipData> equips;
};

//20140521 翻拍后的数据
struct Flat_FlopTaskGridRsp
{
	int err_code;

	unsigned int boss_stat;			//TaskGridStat
	unsigned int grid_stat;			//TaskGridStat

	//item数量
	int count;
	vector<ItemInfo> items;
};
struct Flat_PurifyRsp
{
	int err_code;

	vector<ZBAttr> zbAttrVec;			//属性列 最多5孔
};

struct Flat_AssessWeaponRsp
{
	int err_code;

	vector<ItemInfo> itemsVec;			//属性列 最多5孔
};

struct Flat_QueryAssessWeaponRsp
{
	int err_code;

	vector<Flat_DuanzaoGroupZBInfo> eqVec;	//最多一个数据
};

struct Flat_ReadEmailRsp
{
	int err_code;
	vector<EquipData> eqVec;
};

struct Email;
struct Flat_QueryEmailRsp
{
	int err_code;
	vector<Email*> emailVec;
};

struct Flat_PushEmailRsp
{
	int err_code;
	vector<Email*> emailVec;
};

struct Flat_QueryGiftBagRsp
{
	int err_code;
	vector<GiftBag> giftBagVec;
};

struct Flat_GetSysTemGiftBagRsp
{
	int err_code;

	unsigned int vitality;		//0:没有体力信息 >0: 补偿的体力值
	vector<ItemInfo> itemVec;
	vector<ItemSuipianInfo> itemDebrisVec;
	vector<EquipData> equipVec;
	vector<ShenbingSuipianInfo> shenbingDebrisVec;
	vector<GiftBag> giftBagVec; //礼包里面还有礼包
};

struct Flat_QueryEquipFallRsp
{
	int err_code;
	vector<OriginInfo> fallVec;
};

//跑马灯
struct Flat_HeartBeatRsp
{
	int err_code;    
	
	unsigned int current_vit; 	//当前体力值

	vector<MarqueeMessage> msgVec ;	//跑马灯消息队列
};

//查询公告
struct Flat_SystempNotifyInfoRsp
{
	struct strACTIVITY
	{
		unsigned short type;
		string title;
		string time;
		string content;
	};

	int err_code;    

	//1. 更新说明
	vector<string> updateInfoVec;
	//2. 活动内容
	vector<strACTIVITY> activityVec;
};


#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
#pragma pack() //结束
#endif