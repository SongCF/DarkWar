
#ifndef _PROCSERVERTHREAD_H_
#define _PROCSERVERTHREAD_H_

#include <map>
#include <string>
#include <list>
#include "cmysql.h"

#include "SDThreadPool.h"
#include "PacketQueue.h"
#include "protocol.h"
#include "AES.h"
#include "ScriptDataProvider.h"
#include "FightScriptBuilder.h"
#include "CS_Common.h"
#include "CalculateData.h"

//客户端平台
#define PLATFORM_IOS 1
#define PLATFORM_ANDROID 2


//升级提示最小时间间隔
#define iOSUpdatePromptTimeGap 86400
#define iOSCommentPromptTimeGap 86400


//强化级别概率
#define QianghuaRatio_tie  37
#define QianghuaRatio_tong 50
#define QianghuaRatio_yin  10
#define QianghuaRatio_jin  3
//强化级别效果
#define QianghuaPercent_tie 3
#define QianghuaPercent_tong 6
#define QianghuaPercent_yin 12
#define QianghuaPercent_jin 24

//系统最多允许一个装备强化多少次
#define Qianghua_time_max 9

//轮询掉落装备关卡数
#define MAX_POLL_DROP_EQUIP_TASK 40

//boss关掉落装备概率
#define DROP_ZHUANGBEI_RATIO 30

//跑马灯队列最大个数
#define MAX_MARQUEE_MESSAGE_COUNT 10


//特殊怪物在表中ID
enum SpecialMonsterID
{
    SMID_LegendNote_YouXia = 1197,
    SMID_LegendNote_YeManRen = 1198,
    SMID_LegendNote_CiKe = 1199,
    SMID_LegendNote_WuSeng = 1200,
    SMID_LegendNote_ShenJianShou = 1201,
    SMID_LegendNote_LieShou = 1202,
    SMID_LegendNote_MiShu = 1203,
    SMID_LegendNote_FaShi = 1204,
    SMID_LegendNote_WuShi = 1205,
    SMID_LegendNote_QiShi = 1206,
    
    SMID_CityGuard  = 1207,		//守城boss
};


//英雄类别
enum HeroType
{
	HeroType_Invalid	= 0, 	//无效
    HeroType_Hero  		= 1,	//英雄
    HeroType_Soul 		= 2,	//元神
};


//竞技场
enum ArenaHeroType
{
    ArenaHeroType_OnlinePlayer  = 1,
    ArenaHeroType_StubPlayer    = 2,
    ArenaHeroType_Robot         = 3,
};


struct ArenaHero
{
    ArenaHeroType type;

    unsigned int hero_id;
};
struct ArenaRoom
{
    //第一个人进入的时间即为房间创建时间
    time_t create_time;

    std::vector<ArenaHero> hero_vec;
};
struct Arena
{
    //所有arena结构中的数据的读写都必须首先获取该锁
    pthread_mutex_t mutex;

    ArenaRoom room_arr[HERO_MAX_LEVEL+ARENA_HARD_LEVEL_COUNT-1];

    Arena()
    {
        pthread_mutex_init(&mutex, NULL);

    };

    ~Arena()
    {
        pthread_mutex_destroy(&mutex);
    };
};


//战斗userdata 脚本传入参数
struct FightSUD
{ 
	bool boss_freezy;
	bool b_check_count;					//是否需要检查背包里未装备的个数超过限制 默认为true
	BattleType battle_type;
	unsigned int battle_kind;			//BattleType_ProtectSpirit: 战斗B方种类 0:hero,  1:monster 
										//BattleType_SelfChallenge: 对应相应难度SelfChlgDiffType
										//BattleType_CityGuard: 对应是否花钱跳过 0:没有 1:花钱跳过
	unsigned int heroId;
	unsigned int taskId;
	unsigned int gridId;
	unsigned int fightIndex;
	// 用于pvp屏蔽等级限制的
	// 打斗的时候双方的等级会取相同的等级，比如平均值
	unsigned int hero_a_level;
	unsigned int hero_b_level;			//BattleType_SelfChallenge: 对应怪物关卡
	HeroState heroHpAfterLastFight;	
	

	FightSUD()
	{
		boss_freezy = false;
		b_check_count = true;
		hero_a_level = 0;
		hero_b_level = 0;
		battle_type = BattleType_MainTask;
	}
};


//团队BOSS
enum TeamBossHeroType
{
    TeamBossHeroType_OnlinePlayer  = ArenaHeroType_OnlinePlayer,
    TeamBossHeroType_StubPlayer    = ArenaHeroType_StubPlayer,
    TeamBossHeroType_Robot         = ArenaHeroType_Robot,
};
struct TeamBossHero
{
    TeamBossHeroType type;

    unsigned int hero_id;
};
struct TeamBossRoom
{
    //第一个人进入的时间即为房间创建时间
    time_t create_time;

    std::vector<TeamBossHero> hero_vec;
};
struct TeamBossHall
{
    //所有arena结构中的数据的读写都必须首先获取该锁
    pthread_mutex_t mutex;

    TeamBossRoom room_arr[HERO_MAX_LEVEL+ARENA_HARD_LEVEL_COUNT-1];

    TeamBossHall()
    {
        pthread_mutex_init(&mutex, NULL);

    };

    ~TeamBossHall()
    {
        pthread_mutex_destroy(&mutex);
    };
};



//巅峰对决
/********************星号***************************
*
*星号包围的代码是待删的代码
*
********************星号***************************/
#if 0
struct TopArenaHeroInfo
{
    HeroInfo hero_info;

    //英雄榜排名
    unsigned int top_hero_order;
};
struct SPCmd_topArenaQueryStatusRsp
{
    int err_code;

    //stat 分为：无效(0)、开启竞猜(1)、比赛进行中(2)、比赛完成(3)
    unsigned int stat;

    //参赛选手0~2为a方，对战b方3~5, 、、、
    TopArenaHeroInfo hero_info_arr[30];

    //玩家竞猜
    bool hasGuess;
    bool guess_arr[5];

    //如果是比赛完成状态，则下面为胜负关系
    //为true表示a方胜，反之b方胜
    bool game_result_arr[15];

    //如果是比赛完成状态，这里标示是否中奖
    unsigned int gold_count;
};

struct SPCmd_topArenaGuess
{
    //为true表示a方胜，反之b方胜
    bool guess_arr[5];
};

struct SPCmd_topArenaGuessRsp
{
    int err_code;
};

struct SPCmd_topArenaWatchTV
{
    //1~15
    unsigned int match_id;
};

struct SPCmd_topArenaWatchTVRsp
{
    int err_code;

    char fight_script[1];
};

struct SPCmd_topArenaGetGift
{
    //占位
    int stub;
};

struct SPCmd_topArenaGetGiftRsp
{
    int err_code;

    unsigned int gold_count;
};
/***************************************************/
#endif

struct TopArenaScriptInfo {
	int len[3];
	char *script[3];

	TopArenaScriptInfo() {
		for(int i=0; i<3; i++) {
			len[i] = 0;
			script[i] = NULL;
		}
	};
	
	void clearAllInfo(){
		for(int i=0; i<3; i++) {
			if(script[i] != NULL) {
				len[i] = 0;
				delete script[i];
				script[i] = NULL;
			}
		}
	}
	
	~TopArenaScriptInfo() {
		clearAllInfo();
	};

};

struct TopArenaHeroValue { //小组各英雄身价
	unsigned int value_a;
	unsigned int value_b;

	void initValue()
	{
		value_a = MIN_HERO_VALUE;
		value_b = MIN_HERO_VALUE;
	}
};

struct TopArena
{
    //所有topArena结构中的数据的读写都必须首先获取该锁
    pthread_mutex_t mutex;

	TopArenaRoundType round;    		//比赛场次, 0:未开始, 1:8进4, 2:4进2, 3:2进1
	//bool last_guess_clear;				//最后2进1竞猜结果表的清除标志位 false: 已清除 true:未清除
	unsigned int next_top_arena_time;	//下次巅峰对决开启时间 0:活动进行中, >0:活动未开始
	
	TopArenaStat stat[3];

	//bool round_info_record[3]; 	//是否有比赛信息, true:有, false:没有; 下标0:8进4, 1:4进2, 2:2进1  
	
	TopArenaInfo player_arr_8_4[4];  	//8进4选手
	TopArenaInfo player_arr_4_2[2];  	//4进2选手
	TopArenaInfo player_arr_2_1[1];  	//2进1选手

	TopArenaHeroValue value_arr_8_4[4];		//8进4各小组英雄身价
	TopArenaHeroValue value_arr_4_2[2];		//4进2各小组英雄身价
	TopArenaHeroValue value_arr_2_1[1];		//2进1各小组英雄身价

	TopArenaScriptInfo script_arr_8_4[4]; 	//8进4战斗脚本
	TopArenaScriptInfo script_arr_4_2[2];	//4进2战斗脚本
	TopArenaScriptInfo script_arr_2_1[1];	//2进1战斗脚本

    TopArena()
    {
        pthread_mutex_init(&mutex, NULL);
    	round = TopArenaRoundType_NUL;
		next_top_arena_time = 0;
		for(int i=0; i<3; i++) {
			stat[i] = TopArenaStat_Invalide;
			//round_info_record[i] = false; 
		}
    };

    ~TopArena()
    {
        pthread_mutex_destroy(&mutex);
    };

	void clearAllInfo()
	{
		for(int i=0; i<3; i++) {
			stat[i] = TopArenaStat_Invalide;
			//round_info_record[i] = false;
		}
		round = TopArenaRoundType_NUL;

		for(int i=0; i<4; i++){
			bzero(&player_arr_8_4[i], sizeof(TopArenaInfo));
			//player_arr_8_4[i].round = TopArenaRoundType_8_4;
			value_arr_8_4[i].initValue();
			script_arr_8_4[i].clearAllInfo();
		}
		for(int i=0; i<2; i++){
			bzero(&player_arr_4_2[i], sizeof(TopArenaInfo));
			//player_arr_4_2[i].round = TopArenaRoundType_4_2;
			value_arr_4_2[i].initValue();
			script_arr_4_2[i].clearAllInfo();
		}
		bzero(&player_arr_2_1[0], sizeof(TopArenaInfo));
		//player_arr_2_1[0].round = TopArenaRoundType_2_1;
		value_arr_2_1[0].initValue();
		script_arr_2_1[0].clearAllInfo();
	}
};

struct CityGuardShanghai
{
    unsigned int hero_id;
    unsigned int profession;
    unsigned int shanghai;
    std::string hero_name;
};
struct CityGuardBattleRecord
{
    //在该秒时间开始时，boss的血量
    unsigned int boss_hp;

    //在该秒时间内，所有发生的战斗伤害记录
    std::list<CityGuardShanghai> shanghai_list;
};
struct CityGuardHero
{
    unsigned int hero_id;
};

//发送战况通知多少秒轮询完所有玩家
#define CityGuardHeroNotifyPeriodSec 10

//发送战报中，客户端每秒大概播放几条伤害数据
#define CityGuardHeroNotifyShowShanghaiPerSec 4

//守城
struct CityGuard
{
    //所有结构中的数据的读写都必须首先获取该锁
    pthread_mutex_t mutex;

    //当前守城活动是否开启
    bool is_opening;
    //记录boss初始总血量方便使用
    unsigned int boss_total_hp;
    //记录开启时间点
    time_t opening_timestamp;

    //守城结束时需要发送通知告知boss已死的玩家队列
    std::list<CityGuardHero> hero_wait_for_boss_dead_notify_table[CityGuardHeroNotifyPeriodSec];


    //所有战斗伤害的记录
    std::map<time_t, CityGuardBattleRecord> battle_record_map;

    //所有参与守城活动的玩家的列表。
    //新玩家入表时，使用bos_hash算法决定数组下标
    //在数组中轮询发送。每秒向一个数组项中所有玩家发送一次通知。
    std::list<CityGuardHero> hero_notify_table[CityGuardHeroNotifyPeriodSec];
    unsigned int next_index_to_send_notify; //下一次要发送通知的数组下标。

    CityGuard()
    {
        pthread_mutex_init(&mutex, NULL);
    };

    ~CityGuard()
    {
        pthread_mutex_destroy(&mutex);
    };
};
struct CityGuardProcessNotifyData
{
    //客户端需要用多少秒来播放伤害过程。
    unsigned int play_time; //为0表示都无效了

    //在play_time时间段内boss的血量需要减少这么多
    unsigned int boss_hp_start; //为0表示boss已被击杀
    unsigned int boss_hp_end;

    //需要播放的伤害
    std::vector<CityGuardAnotherHeroShanghai> another_hero_shanghai;
};

//VIP功能信息
enum VIP_FIELD_TYPE {
	VIP_FIELD_TYPE_VITALITY = 1,
	VIP_FIELD_TYPE_SWEEP = 2,
	VIP_FIELD_TYPE_ARENA = 3,
	VIP_FIELD_TYPE_REFRESH_REWAED = 4,
	VIP_FIELD_TYPE_CAST_SILVER = 5,
};

#pragma pack(1) //按照单字节对齐

struct __PurifyAttr{
    unsigned int star_level;
    unsigned int weight;
    unsigned int min_value;
    unsigned int max_value;
};

struct __ZhuangbeiDetail
{
    unsigned long long zhuangbei_id;
    unsigned int group_id; //装备模板组
    unsigned int profession_id;

    unsigned int zhuangbei_type;
    char zhuangbei_name[ZhuangbeiNameMaxLen+1];
    unsigned int zhuangbei_colour;
    char zhuangbei_pic[ZhuangbeiPicMaxLen+1];

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

    //附魔信息
    bool has_fumo;
    ZBAttr fumo_attr;

    //
    std::vector<ZBAttr> attr_vec;
    

    std::string desc;

    //如果是传奇装，则会有精炼信息
    unsigned char jinglian_level;
    std::vector<ZBAttr> jinglian_attr_vec;
    //std::vector<ZBAttr> next_jinglian_attr_vec;

    //如果是神兵，则会有解封印和经文篆刻信息
    bool is_shenbing;
    bool is_unlocked; //是否解封印
    std::vector<ZBAttr> identified_attr; //已鉴定的隐藏属性
    bool has_jingwen;
    ZBAttr jingwen_attr_arr[2];

	std::vector<ZBAttr> purify_attr; 	 //现有洗炼属性 

    void init()
    {
        zhuangbei_id = 0;
        group_id = 0;

        zhuangbei_type = 0;
        zhuangbei_name[0] = 0;
        zhuangbei_colour = 0;
        zhuangbei_pic[0] = 0;

        zhuangbei_hero_id = 0;

        zhuangbei_level = 0;
        hole = 0;
        diamond_type_1 = 0;
        diamond_type_2 = 0;
        diamond_type_3 = 0;
        
        gongji_min = 0;
        gongji_max = 0;
        hujia = 0;

        qianghua_data.max_qianghua_time = Qianghua_time_max;
        qianghua_data.qianghua_count = 0;

        has_fumo = false;

        attr_vec.clear();

        desc = "";

        jinglian_level = 0;

        is_shenbing = false;
        is_unlocked = false;
        has_jingwen = false;
    };
    __ZhuangbeiDetail()
    {
        init();
    };

    //
    void convert2ZhuangbeiDetailAndHton(ZhuangbeiDetail* detail_out, unsigned int& detail_out_len)
    {
        detail_out->zhuangbei_id = Utl::htonll(zhuangbei_id);
        detail_out->group_id = htonl(group_id);
		detail_out->profession_id = htonl(profession_id);

        detail_out->zhuangbei_type = htonl(zhuangbei_type);
        strcpy(detail_out->zhuangbei_name, zhuangbei_name);
        detail_out->zhuangbei_colour = htonl(zhuangbei_colour);
        strcpy(detail_out->zhuangbei_pic, zhuangbei_pic);

        detail_out->zhuangbei_hero_id = htonl(zhuangbei_hero_id);

        detail_out->zhuangbei_level = htonl(zhuangbei_level);
        detail_out->hole = htonl(hole);
        detail_out->diamond_type_1 = htonl(diamond_type_1);
        detail_out->diamond_type_2 = htonl(diamond_type_2);
        detail_out->diamond_type_3 = htonl(diamond_type_3);
        
        detail_out->gongji_min = htonl(gongji_min);
        detail_out->gongji_max = htonl(gongji_max);
        detail_out->hujia = htonl(hujia);

        detail_out->qianghua_data = qianghua_data;
        
        detail_out->has_fumo = has_fumo;
        detail_out->fumo_attr.attr_type = htonl(fumo_attr.attr_type);
        detail_out->fumo_attr.attr_value = htonl(fumo_attr.attr_value);
        detail_out->fumo_attr.cuiqu_suc_ratio = htonl(fumo_attr.cuiqu_suc_ratio);

        detail_out->common_attr.attr_count = htonl(attr_vec.size());
        for(int i=0; i<attr_vec.size(); ++i)
        {
            ((ZBAttr*)detail_out->common_attr.attr_arr)[i].attr_type = htonl(attr_vec[i].attr_type);
            ((ZBAttr*)detail_out->common_attr.attr_arr)[i].attr_value = htonl(attr_vec[i].attr_value);
            ((ZBAttr*)detail_out->common_attr.attr_arr)[i].cuiqu_suc_ratio = htonl(attr_vec[i].cuiqu_suc_ratio);
        }

        ZBAttr* attr_p = (ZBAttr*)detail_out->common_attr.attr_arr;
        attr_p += attr_vec.size();
        char* desc_p = (char*)attr_p;
        strcpy(desc_p, desc.c_str());

        //如果不是稀有装以上则到此结束
        if(zhuangbei_colour < ZhuangbeiColour_Xiyou) {
            detail_out_len = desc_p+desc.length()+1 - (char*)detail_out;
            return;
        }

        //精炼
        JinglianData* jinglian_data_p = (JinglianData*)(desc_p+desc.length()+1);
        jinglian_data_p->jinglian_times = htonl(jinglian_level);
        jinglian_data_p->jinglian_attr_arr[0].attr_type = 0;
        jinglian_data_p->jinglian_attr_arr[1].attr_type = 0;
        for(int i=0; i<jinglian_attr_vec.size(); ++i)
        {
            jinglian_data_p->jinglian_attr_arr[i].attr_type = htonl(jinglian_attr_vec[i].attr_type);
            jinglian_data_p->jinglian_attr_arr[i].attr_value = htonl(jinglian_attr_vec[i].attr_value);
            jinglian_data_p->jinglian_attr_arr[i].cuiqu_suc_ratio = htonl(jinglian_attr_vec[i].cuiqu_suc_ratio);
        }

		unsigned char* purify_hole_p = (unsigned char*)((char*)(jinglian_data_p+1));

		*purify_hole_p	= (unsigned char)purify_attr.size();

		ZBAttr* purify_arr_p = (ZBAttr*)(purify_hole_p+1);
		for(int i=0; i<purify_attr.size(); ++i) {
            purify_arr_p[i].attr_type = htonl(purify_attr[i].attr_type);
            purify_arr_p[i].attr_value = htonl(purify_attr[i].attr_value);
            purify_arr_p[i].cuiqu_suc_ratio = htonl(purify_attr[i].cuiqu_suc_ratio);
		}

        //
        bool* is_unlocked_p = (bool*)(purify_arr_p + purify_attr.size()); //is_shenbing_p+1;
        *is_unlocked_p = is_unlocked;

		//如果不是传奇装以上则到此结束
        if(zhuangbei_colour != ZhuangbeiColour_Chuanqi) {
            detail_out_len = (char*)(is_unlocked_p +1) - (char*)detail_out;
            return;
        }
		
		
        //
        bool* is_shenbing_p = (bool*)(is_unlocked_p +1);

		//传奇装降为稀有，神兵设为传奇的处理方式
		//*is_shenbing_p = is_shenbing;
        *is_shenbing_p = is_shenbing = true;

        //如果不是神兵则到此结束
        if(!is_shenbing)
        {
            detail_out_len = (char*)(is_shenbing_p+1) - (char*)detail_out;
            return;
        }


        //
        unsigned char* identified_arr_count_p = (unsigned char*)is_shenbing_p+1;
        *identified_arr_count_p = identified_attr.size();
        //
        ZBAttr* identified_arr = (ZBAttr*)(identified_arr_count_p+1);
        for(int i=0; i<identified_attr.size(); ++i)
        {
            identified_arr[i].attr_type = htonl(identified_attr[i].attr_type);
            identified_arr[i].attr_value = htonl(identified_attr[i].attr_value);
            identified_arr[i].cuiqu_suc_ratio = htonl(identified_attr[i].cuiqu_suc_ratio);
        }

        //
        bool* has_jingwen_p = (bool*)((char*)(identified_arr+identified_attr.size()));
        *has_jingwen_p = has_jingwen;
        ZBAttr* jingwen_attr_arr_out = (ZBAttr*)(has_jingwen_p+1);
        jingwen_attr_arr_out[0].attr_type = htonl(jingwen_attr_arr[0].attr_type);
        jingwen_attr_arr_out[0].attr_value = htonl(jingwen_attr_arr[0].attr_value);
        jingwen_attr_arr_out[0].cuiqu_suc_ratio = htonl(jingwen_attr_arr[0].cuiqu_suc_ratio);
        jingwen_attr_arr_out[1].attr_type = htonl(jingwen_attr_arr[1].attr_type);
        jingwen_attr_arr_out[1].attr_value = htonl(jingwen_attr_arr[1].attr_value);
        jingwen_attr_arr_out[1].cuiqu_suc_ratio = htonl(jingwen_attr_arr[1].cuiqu_suc_ratio);

        detail_out_len = (char*)(jingwen_attr_arr_out+2) - (char*)detail_out;

        return;
    };
};

struct __DuanzaoGroupZBInfo
{
    unsigned int zb_group_id;
    unsigned int zhuangbei_type;
    char zhuangbei_name[ZhuangbeiNameMaxLen+1];
    unsigned int zhuangbei_colour;
    char zhuangbei_pic[ZhuangbeiPicMaxLen+1];
    unsigned int level;
    unsigned int profession_id;    		// 绑定的职业ID
	unsigned int next_group_id;			//升级到下一等级的装备id
	unsigned int need_expend_group_id; 	//神兵升级所需吞噬的装备id
    unsigned int ratio; 				//产出概率

    unsigned int gongji_min_min;
    unsigned int gongji_min_max;
    unsigned int gongji_max_min;
    unsigned int gongji_max_max;
    unsigned int wufang_min;
    unsigned int wufang_max;
    unsigned int hole1_percent;
    unsigned int hole2_percent;
    unsigned int hole3_percent;

    std::vector<std::string> attr_vec;

    std::string desc;

	void convert2ZBInfoAndHton(DuanzaoGroupZBInfo* zb_out, unsigned int& zb_out_len)
	{
		zb_out_len = sizeof(DuanzaoGroupZBInfo);
		DuanzaoGroupZBInfo* p = zb_out;
		p->ratio = htonl(ratio);
        
        p->zb_group_id = htonl(zb_group_id);
        p->next_group_id = htonl(next_group_id);
        p->need_expend_group_id = htonl(need_expend_group_id);
        p->profession_id = htonl(profession_id);
        p->zhuangbei_type = htonl(zhuangbei_type);
        p->zhuangbei_colour = htonl(zhuangbei_colour);
        p->level = htonl(level);
        strncpy(p->zhuangbei_name, zhuangbei_name, ZhuangbeiNameMaxLen);
        p->zhuangbei_name[ZhuangbeiNameMaxLen] = 0;
        strncpy(p->zhuangbei_pic, zhuangbei_pic, ZhuangbeiPicMaxLen);
        p->zhuangbei_pic[ZhuangbeiPicMaxLen] = 0;

        p->gongji_min_min = htonl(gongji_min_min);
        p->gongji_min_max = htonl(gongji_min_max);
        p->gongji_max_min = htonl(gongji_max_min);
        p->gongji_max_max = htonl(gongji_max_max);
        p->wufang_min = htonl(wufang_min);
        p->wufang_max = htonl(wufang_max);
        p->hole1_percent = htonl(hole1_percent);
        p->hole2_percent = htonl(hole2_percent);
        p->hole3_percent = htonl(hole3_percent);

        p->attr_str_count = htonl(attr_vec.size());

        char* str = p->attr_str;
        for(int j=0; j<attr_vec.size(); ++j)
        {
            strcpy(str, attr_vec[j].c_str());
            str += attr_vec[j].length() +1;
			zb_out_len += attr_vec[j].length() +1;
        }

        //desc
        strcpy(str, desc.c_str());
		
        zb_out_len += desc.length() + 1;
       
	}
};


//抽取装备时候所用结构体
struct __zb_type_group_id{
    unsigned int type;
    unsigned int group_id;
    unsigned int zb_level;
};

struct BonuStatus {
	bool is_get;
	bool enable;
	unsigned int id;
	unsigned int refresh_time;
};

//奖励信息
struct __BonusInfo {
	BonusType type;
	
    bool is_get;		   		//是否领取次奖励 false:未领取, true:已领取
    bool enable;		   		//是否可以领取奖励 false:不能领取 true:可以领取
    unsigned int bonus_id; 		//每种奖励的ID

	/**************** 星号之间的为日常任务和悬赏任务  ****************/
    unsigned int w_day; 		//刷新天数
    unsigned int vip_level; 	//>0:领取多倍奖励的vip等级分界线 0:只有1倍奖励 
    unsigned int multiple; 		//倍数
    std::string description; 	//对奖励的描述
    /*****************************************************************/
	
    unsigned int ran_zb_count;	// 随机装备数量

	//每次取数据的时候zb_vec和zb_ranflg_vec 从3个职业中取一个
	unsigned int prof_zb_ran[Profession_Fashi];	//保存3个职业的随机数量
	std::vector<unsigned int> prof_zb_group_vec[Profession_Fashi];	//保存3个职业的group_id 野蛮人 刺客 法师

	/* 以下为附件信息 */
	unsigned int vitality;								//0:没有体力信息 >0: 补偿的体力值
    std::vector<ItemInfo> item_vec;						//道具材料
    std::vector<ItemSuipianInfo> item_suipian_vec;		//道具碎片
    std::vector<ShenbingSuipianInfo> zb_suipian_vec;	//装备碎片
    std::vector<GiftBag> gift_bag_vec;					//礼包
	std::vector<__DuanzaoGroupZBInfo> zb_vec;			//指定装备

	__BonusInfo () {
		type = BonusType_Invalid;
		is_get = false;
		enable = false;
		bonus_id = 0;
		w_day = 0;
		vip_level = 0;
		multiple = 0;
		vitality = 0;
		ran_zb_count = 0;

		for(int i=0; i<Profession_Fashi; ++i) {
			prof_zb_ran[i] = 0;
		}
	};

	const __BonusInfo& operator=(const __BonusInfo& info) {
		type = info.type;
		is_get = info.is_get;
		enable = info.enable;
		bonus_id = info.bonus_id;
		w_day = info.w_day;
		vip_level = info.vip_level;
		multiple = info.multiple;
				
		ran_zb_count = info.ran_zb_count;
		for(int i=0; i<Profession_Fashi; ++i) {
			prof_zb_ran[i] = info.prof_zb_ran[i];
			prof_zb_group_vec[i] = info.prof_zb_group_vec[i];
		}
		
		vitality = info.vitality;
		item_vec = info.item_vec;
		item_suipian_vec = info.item_suipian_vec;
		zb_suipian_vec = info.zb_suipian_vec;
		gift_bag_vec = info.gift_bag_vec;
		zb_vec = info.zb_vec;
		
		return *this;
	}

	void convert2_bonus_info_hton(BonusInfo* bonus_out, unsigned int& bonus_out_len)
	{
		bonus_out->type = htonl(type);
		bonus_out->is_get = is_get;
		bonus_out->enable = enable;
		bonus_out->bonus_id = htonl(bonus_id);
		
		char *pos = NULL; 	//变长定位后面每个字段的位置
		pos = (char*)&(bonus_out->bonus_id); 
		
		//vitality
		pos += sizeof(unsigned int);
		unsigned int *vitality_p = (unsigned int *)pos; 	
		*vitality_p = htonl(vitality);

		//item_count
		unsigned int *item_count_p = vitality_p +1;		
		*item_count_p = htonl(item_vec.size());
		
		//item_arr
	    ItemInfo* item_arr_p = (ItemInfo*)(item_count_p +1); 
	    for(int i=0; i<item_vec.size(); ++i) {
	        item_arr_p[i].type = htonl(item_vec[i].type);
	        item_arr_p[i].count = htonl(item_vec[i].count);
	    }

		//item_suipian_count
		unsigned int *item_suipian_count_p = (unsigned int *)(item_arr_p + item_vec.size());
		*item_suipian_count_p = htonl(item_suipian_vec.size());
		//item_suipian_arr
	    ItemSuipianInfo* item_suipian_arr_p = (ItemSuipianInfo*)(item_suipian_count_p +1); 
	    for(int i=0; i<item_suipian_vec.size(); ++i) {
	        item_suipian_arr_p[i].item_id = htonl(item_suipian_vec[i].item_id);
	        item_suipian_arr_p[i].suipian_id = htonl(item_suipian_vec[i].suipian_id);
	        item_suipian_arr_p[i].suipian_count = htonl(item_suipian_vec[i].suipian_count);
	    }

		//zb_suipian_count
		unsigned int *zb_suipian_count_p = (unsigned int *)(item_suipian_arr_p + item_suipian_vec.size());
		*zb_suipian_count_p = htonl(zb_suipian_vec.size());
		//zb_suipian_arr
	    ShenbingSuipianInfo* zb_suipian_arr_p = (ShenbingSuipianInfo*)(zb_suipian_count_p +1); 
	    for(int i=0; i<zb_suipian_vec.size(); ++i) {
	        zb_suipian_arr_p[i].zb_group_id = htonl(zb_suipian_vec[i].zb_group_id);
	        zb_suipian_arr_p[i].suipian_id = htonl(zb_suipian_vec[i].suipian_id);
	        zb_suipian_arr_p[i].suipian_count = htonl(zb_suipian_vec[i].suipian_count);
	    }

		//gift_bag_count
		unsigned int *gift_bag_count_p = (unsigned int *)(zb_suipian_arr_p + zb_suipian_vec.size());
		*gift_bag_count_p = htonl(gift_bag_vec.size());
		//gift_bag_arr
	    GiftBag* gift_bag_arr_p = (GiftBag*)(gift_bag_count_p +1); 
	    for(int i=0; i<gift_bag_vec.size(); ++i) {
	        gift_bag_arr_p[i].id = htonl(gift_bag_vec[i].id);
	        gift_bag_arr_p[i].count = htonl(gift_bag_vec[i].count);
	    }

		//ran_zb_count
		unsigned int *ran_zb_count_p = (unsigned int *)(gift_bag_arr_p + gift_bag_vec.size());
		*ran_zb_count_p =  htonl(ran_zb_count);

		//zb_count
		unsigned int *zb_count_p = ran_zb_count_p +1;
		*zb_count_p =  htonl(zb_vec.size());

		//zb_arr
		pos = (char *)(zb_count_p +1);
	    DuanzaoGroupZBInfo* zb_p = (DuanzaoGroupZBInfo*)pos;
	    for(int i=0; i<zb_vec.size(); ++i) {
			unsigned int zb_len =0;
	        zb_vec[i].convert2ZBInfoAndHton(zb_p, zb_len);
	        pos += zb_len;
	        zb_p = (DuanzaoGroupZBInfo*)(pos);
	    }

		if(type == BonusType_DailyLogin || type == BonusType_Level) {
			bonus_out_len = pos - (char*)bonus_out;
			return ;
		}
		
		//vip_level
		unsigned int *vip_level_p = (unsigned int *)pos;
		*vip_level_p =  htonl(vip_level);

		//multiple
		unsigned int *multiple_p = vip_level_p +1;
		*multiple_p =  htonl(multiple);

		//description
		pos = (char*)(multiple_p +1);
		strcpy(pos, description.c_str());
		pos +=  description.length() +1;

		bonus_out_len = pos - (char*)bonus_out;
		return ;
	}

	unsigned int get_length(void) 
	{	
		unsigned int length = sizeof(BonusInfo);

		length += sizeof(unsigned int) *2; //vitality, item_count
		length += sizeof(ItemInfo) * item_vec.size(); // item_arr;
		
		length += sizeof(unsigned int); //item_suipian_count
		length += sizeof(ItemSuipianInfo) * item_suipian_vec.size(); // item_suipian_arr;
		
		length += sizeof(unsigned int); //zb_suipian_count
		length += sizeof(ShenbingSuipianInfo) * zb_suipian_vec.size(); // zb_suipian_arr;
		
		length += sizeof(unsigned int); //gift_bag_count
		length += sizeof(GiftBag) * gift_bag_vec.size(); // gift_bag_arr;
		
		length += sizeof(unsigned int) *2;	//ran_zb_count, zb_count

		//zb_arr
		length += sizeof(DuanzaoGroupZBInfo) * zb_vec.size();
		for(int i=0; i<zb_vec.size(); ++i) {
            for(int j=0; j<zb_vec[i].attr_vec.size(); ++j) {
                length += zb_vec[i].attr_vec[j].length() +1;
            }
            length += zb_vec[i].desc.length() +1;
        }
		
		if(BonusType_DayWelfare==type || BonusType_Reward==type) {
			length += sizeof(unsigned int) *2; //vip_level, multiple
			//description
			length += description.length() +1;
		}
		
		return length;
	}
};



//内部邮件ID 对应email_internal_temp这张表的ID
enum EmailID
{	
    EmailID_Invalid = 0,
		
	//巅峰对决竞猜	
    EmailID_GuessRight_Low_8_4 = 201,
    EmailID_GuessWrong_Low_8_4 = 202,
    EmailID_GuessRight_High_8_4 = 203,
    EmailID_GuessWrong_High_8_4 = 204,
    EmailID_GuessRight_Low_4_2 = 205,
    EmailID_GuessWrong_Low_4_2 = 206,
    EmailID_GuessRight_High_4_2 = 207,
    EmailID_GuessWrong_High_4_2 = 208,
    EmailID_GuessRight_Low_2_1 = 209,
    EmailID_GuessWrong_Low_2_1 = 210,
    EmailID_GuessRight_High_2_1 = 211,
    EmailID_GuessWrong_High_2_1 = 212,

	//巅峰对决选手颁奖
    EmailID_TopArena_Top8 = 213,
    EmailID_TopArena_Top4 = 214,
    EmailID_TopArena_Top2 = 215,
    EmailID_TopArena_Top1 = 216,
    
	//英雄榜排名奖励
	EmailID_TopList_5001_1w 	= 301,
	EmailID_TopList_2001_5k 	= 302,
	EmailID_TopList_1001_2k 	= 303,
	EmailID_TopList_201_1k  	= 304,
	EmailID_TopList_101_200 	= 305,
	EmailID_TopList_31_100 		= 306,
	EmailID_TopList_11_30 		= 307,
	EmailID_TopList_4_10 		= 308,
	EmailID_TopList_3 			= 309,
	EmailID_TopList_2 			= 310,
	EmailID_TopList_1 			= 311,

	//自我挑战排名奖励
	EmailID_SelfChlgOrderCommon_1 = 401,
	EmailID_SelfChlgOrderCommon_2 = 402,
	EmailID_SelfChlgOrderCommon_3 = 403,
	EmailID_SelfChlgOrderCommon_4_10 = 404,
	EmailID_SelfChlgOrderDifficult_1 = 405,
	EmailID_SelfChlgOrderDifficult_2 = 406,
	EmailID_SelfChlgOrderDifficult_3 = 407,
	EmailID_SelfChlgOrderDifficult_4_10 = 408,
	EmailID_SelfChlgOrderHell_1 = 409,
	EmailID_SelfChlgOrderHell_2 = 410,
	EmailID_SelfChlgOrderHell_3 = 411,
	EmailID_SelfChlgOrderHell_4_10 =412,
	EmailID_SelfChlgOrderNightmare_1 = 413,
	EmailID_SelfChlgOrderNightmare_2 = 414,
	EmailID_SelfChlgOrderNightmare_3 = 415,
	EmailID_SelfChlgOrderNightmare_4_10 = 416,
		
	//装备奖励
	//EmailID_Present_Weapon = 601,
	//首次充值赠送
	EmailID_First_Charge = 602,
	//赠送体力
	EmailID_PresentVitality = 603,
		

};


//邮件信息
struct __EmailInfo{
	EmailType type;
	
	bool is_read;				//是否阅读 false:未读, true:已读
	unsigned int email_id;		//邮件模板id
	unsigned int assign_id;		//每个玩家对应的邮件id
	unsigned int addresser_id;	//发件人id 0:为系统 >0:玩家

	std::string addresser;		//发件人姓名
	std::string content;	
	std::string summary;	
	

    unsigned int ran_zb_count;	// 随机装备数量

	//每次取数据的时候zb_vec和zb_ranflg_vec 从3个职业中取一个
	unsigned int prof_zb_ran[Profession_Fashi];	//保存3个职业的随机数量
	std::vector<unsigned int> prof_zb_group_vec[Profession_Fashi];	//保存3个职业的group_id 野蛮人 刺客 法师

	/* 以下为附件信息 */
	unsigned int vitality;								//0:没有体力信息 >0: 补偿的体力值
    std::vector<ItemInfo> item_vec;						//道具材料
    std::vector<ItemSuipianInfo> item_suipian_vec;		//道具碎片
    std::vector<ShenbingSuipianInfo> zb_suipian_vec;	//装备碎片
    std::vector<GiftBag> gift_bag_vec;					//礼包
	std::vector<__DuanzaoGroupZBInfo> zb_vec;			//指定装备

	__EmailInfo () {
		type = EmailType_Bonus;
		is_read = false;
		email_id = 0;
		assign_id = 0;
		addresser_id = 0;
		vitality = 0;
		ran_zb_count = 0;

		for(int i=0; i<Profession_Fashi; ++i) {
			prof_zb_ran[i] = 0;
		}
	};

	const __EmailInfo& operator=(const __EmailInfo& info) {
		type = info.type;
		is_read = info.is_read;
		email_id = info.email_id;
		assign_id = info.assign_id;
		addresser_id = info.addresser_id;
		
		addresser = info.addresser;
		content = info.content;
		summary = info.summary;
		
		ran_zb_count = info.ran_zb_count;
		for(int i=0; i<Profession_Fashi; ++i) {
			prof_zb_ran[i] = info.prof_zb_ran[i];
			prof_zb_group_vec[i] = info.prof_zb_group_vec[i];
		}
		
		vitality = info.vitality;
		item_vec = info.item_vec;
		item_suipian_vec = info.item_suipian_vec;
		zb_suipian_vec = info.zb_suipian_vec;
		gift_bag_vec = info.gift_bag_vec;
		zb_vec = info.zb_vec;
		
		return *this;
	}

	void convert2_email_info_hton(EmailInfo* email_out, unsigned int& email_out_len)
	{
		email_out->type = htonl(type);
		email_out->is_read = is_read;
		email_out->id = htonl(assign_id);
		email_out->addresser_id = htonl(addresser_id);
		
		strncpy(email_out->addresser, addresser.c_str(), HeroNameMaxLen);
		strcpy(email_out->content, content.c_str());

		char *pos = NULL; 	//变长定位后面每个字段的位置
		pos = email_out->content;
		pos += content.length();

		// '\0'
		pos++; 	//summary
		strcpy(pos, summary.c_str());
		pos += summary.length();

		// '\0'
		pos++;	//is_have_attach
		bool *is_have_attach_p = (bool *)pos;
		*is_have_attach_p = (type==EmailType_Bonus || type==EmailType_Compensate);

		if(!is_have_attach_p) {
			email_out_len = pos +1 - (char*)email_out;
			return ;
		}

		//vitality
		unsigned int *vitality_p = (unsigned int *)(pos +1); 	
		*vitality_p = htonl(vitality);

		//item_count
		unsigned int *item_count_p = vitality_p +1;		
		*item_count_p = htonl(item_vec.size());
		
		//item_arr
	    ItemInfo* item_arr_p = (ItemInfo*)(item_count_p +1); 
	    for(int i=0; i<item_vec.size(); ++i) {
	        item_arr_p[i].type = htonl(item_vec[i].type);
	        item_arr_p[i].count = htonl(item_vec[i].count);
	    }

		//item_suipian_count
		unsigned int *item_suipian_count_p = (unsigned int *)(item_arr_p + item_vec.size());
		*item_suipian_count_p = htonl(item_suipian_vec.size());
		//item_suipian_arr
	    ItemSuipianInfo* item_suipian_arr_p = (ItemSuipianInfo*)(item_suipian_count_p +1); 
	    for(int i=0; i<item_suipian_vec.size(); ++i) {
	        item_suipian_arr_p[i].item_id = htonl(item_suipian_vec[i].item_id);
	        item_suipian_arr_p[i].suipian_id = htonl(item_suipian_vec[i].suipian_id);
	        item_suipian_arr_p[i].suipian_count = htonl(item_suipian_vec[i].suipian_count);
	    }

		//zb_suipian_count
		unsigned int *zb_suipian_count_p = (unsigned int *)(item_suipian_arr_p + item_suipian_vec.size());
		*zb_suipian_count_p = htonl(zb_suipian_vec.size());
		//zb_suipian_arr
	    ShenbingSuipianInfo* zb_suipian_arr_p = (ShenbingSuipianInfo*)(zb_suipian_count_p +1); 
	    for(int i=0; i<zb_suipian_vec.size(); ++i) {
	        zb_suipian_arr_p[i].zb_group_id = htonl(zb_suipian_vec[i].zb_group_id);
	        zb_suipian_arr_p[i].suipian_id = htonl(zb_suipian_vec[i].suipian_id);
	        zb_suipian_arr_p[i].suipian_count = htonl(zb_suipian_vec[i].suipian_count);
	    }

		//gift_bag_count
		unsigned int *gift_bag_count_p = (unsigned int *)(zb_suipian_arr_p + zb_suipian_vec.size());
		*gift_bag_count_p = htonl(gift_bag_vec.size());
		//gift_bag_arr
	    GiftBag* gift_bag_arr_p = (GiftBag*)(gift_bag_count_p +1); 
	    for(int i=0; i<gift_bag_vec.size(); ++i) {
	        gift_bag_arr_p[i].id = htonl(gift_bag_vec[i].id);
	        gift_bag_arr_p[i].count = htonl(gift_bag_vec[i].count);
	    }

		//ran_zb_count
		unsigned int *ran_zb_count_p = (unsigned int *)(gift_bag_arr_p + gift_bag_vec.size());
		*ran_zb_count_p =  htonl(ran_zb_count);

		//zb_count
		unsigned int *zb_count_p = ran_zb_count_p +1;
		*zb_count_p =  htonl(zb_vec.size());

		//zb_arr
		pos = (char *)(zb_count_p +1);
	    DuanzaoGroupZBInfo* zb_p = (DuanzaoGroupZBInfo*)pos;
	    for(int i=0; i<zb_vec.size(); ++i) {
			unsigned int zb_len =0;
	        zb_vec[i].convert2ZBInfoAndHton(zb_p, zb_len);
	        pos += zb_len;
	        zb_p = (DuanzaoGroupZBInfo*)(pos);
	    }

		email_out_len = pos - (char*)email_out;
		return ;
	}

	unsigned int get_length(void) 
	{	
		unsigned int length = sizeof(EmailInfo);
		length += content.length();
		length += summary.length() +1;
		length += 1; //is_have_attach

		if(type==EmailType_Bonus || type==EmailType_Compensate) {
			length += sizeof(unsigned int) *2; //vitality, item_count
			length += sizeof(ItemInfo) * item_vec.size(); // item_arr;
			
			length += sizeof(unsigned int); //item_suipian_count
			length += sizeof(ItemSuipianInfo) * item_suipian_vec.size(); // item_suipian_arr;
			
			length += sizeof(unsigned int); //zb_suipian_count
			length += sizeof(ShenbingSuipianInfo) * zb_suipian_vec.size(); // zb_suipian_arr;
			
			length += sizeof(unsigned int); //gift_bag_count
			length += sizeof(GiftBag) * gift_bag_vec.size(); // gift_bag_arr;
			
			length += sizeof(unsigned int) *2;	//ran_zb_count, zb_count

			//zb_arr
			length += sizeof(DuanzaoGroupZBInfo) * zb_vec.size();
			for(int i=0; i<zb_vec.size(); ++i) {
	            for(int j=0; j<zb_vec[i].attr_vec.size(); ++j) {
	                length += zb_vec[i].attr_vec[j].length() +1;
	            }
	            length += zb_vec[i].desc.length() +1;
	        }
		}

		return length;
	}
};


//礼包类型
enum GiftBagType {
	GiftBagType_All 		= 0,	//所有礼包内容
	GiftBagType_Random 		= 1,	//随机礼包
	GiftBagType_Invalid 	= 0xFF,	//无效
};

//礼包信息
struct __GiftBagInfo{

    unsigned int ran_zb_count;	// 随机装备数量

	//每次取数据的时候zb_vec和zb_ranflg_vec 从3个职业中取一个
	unsigned int prof_zb_ran[Profession_Fashi];	//保存3个职业的随机数量
	std::vector<unsigned int> prof_zb_group_vec[Profession_Fashi];	//保存3个职业的group_id 野蛮人 刺客 法师
	
	unsigned int vitality;								//0:没有体力信息 >0: 补偿的体力值
    std::vector<ItemInfo> item_vec;						//道具材料
    std::vector<ItemSuipianInfo> item_suipian_vec;		//道具碎片
    std::vector<ShenbingSuipianInfo> zb_suipian_vec;	//装备碎片
    std::vector<GiftBag> gift_bag_vec;					//礼包
	std::vector<__ZhuangbeiDetail> zb_vec;			//指定装备

	__GiftBagInfo () {
		ran_zb_count = 0;
		vitality = 0;

		for(int i=0; i<Profession_Fashi; ++i) {
			prof_zb_ran[i] = 0;
		}
	};

	const __GiftBagInfo& operator=(const __GiftBagInfo& info) {
		ran_zb_count = info.ran_zb_count;
		for(int i=0; i<Profession_Fashi; ++i) {
			prof_zb_ran[i] = info.prof_zb_ran[i];
			prof_zb_group_vec[i] = info.prof_zb_group_vec[i];
		}
		
		vitality = info.vitality;
		item_vec = info.item_vec;
		item_suipian_vec = info.item_suipian_vec;
		zb_suipian_vec = info.zb_suipian_vec;
		gift_bag_vec = info.gift_bag_vec;
		zb_vec = info.zb_vec;
		
		return *this;
	}

	void convert2_gift_bag_info_hton(GiftBagInfo* gift_bag_out, unsigned int& gift_bag_out_len)
	{
		gift_bag_out->vitality = htonl(vitality);
		gift_bag_out->item_count = htonl(item_vec.size());
		
		char *pos = NULL; 	//变长定位后面每个字段的位置
		pos = (char*)gift_bag_out->item_arr;
		
		//item_arr
	    for(int i=0; i<item_vec.size(); ++i) {
	        gift_bag_out->item_arr[i].type = htonl(item_vec[i].type);
	        gift_bag_out->item_arr[i].count = htonl(item_vec[i].count);
	    }

		//item_suipian_count
		unsigned int *item_suipian_count_p = (unsigned int *)(gift_bag_out->item_arr + item_vec.size());
		*item_suipian_count_p = htonl(item_suipian_vec.size());
		//item_suipian_arr
	    ItemSuipianInfo* item_suipian_arr_p = (ItemSuipianInfo*)(item_suipian_count_p +1); 
	    for(int i=0; i<item_suipian_vec.size(); ++i) {
	        item_suipian_arr_p[i].item_id = htonl(item_suipian_vec[i].item_id);
	        item_suipian_arr_p[i].suipian_id = htonl(item_suipian_vec[i].suipian_id);
	        item_suipian_arr_p[i].suipian_count = htonl(item_suipian_vec[i].suipian_count);
	    }

		//zb_suipian_count
		unsigned int *zb_suipian_count_p = (unsigned int *)(item_suipian_arr_p + item_suipian_vec.size());
		*zb_suipian_count_p = htonl(zb_suipian_vec.size());
		//zb_suipian_arr
	    ShenbingSuipianInfo* zb_suipian_arr_p = (ShenbingSuipianInfo*)(zb_suipian_count_p +1); 
	    for(int i=0; i<zb_suipian_vec.size(); ++i) {
	        zb_suipian_arr_p[i].zb_group_id = htonl(zb_suipian_vec[i].zb_group_id);
	        zb_suipian_arr_p[i].suipian_id = htonl(zb_suipian_vec[i].suipian_id);
	        zb_suipian_arr_p[i].suipian_count = htonl(zb_suipian_vec[i].suipian_count);
	    }

		//gift_bag_count
		unsigned int *gift_bag_count_p = (unsigned int *)(zb_suipian_arr_p + zb_suipian_vec.size());
		*gift_bag_count_p = htonl(gift_bag_vec.size());
		//gift_bag_arr
	    GiftBag* gift_bag_arr_p = (GiftBag*)(gift_bag_count_p +1); 
	    for(int i=0; i<gift_bag_vec.size(); ++i) {
	        gift_bag_arr_p[i].id = htonl(gift_bag_vec[i].id);
	        gift_bag_arr_p[i].count = htonl(gift_bag_vec[i].count);
	    }
		
		//zb_count
		unsigned int *zb_count_p = (unsigned int *)(gift_bag_arr_p + gift_bag_vec.size());;
		*zb_count_p =  htonl(zb_vec.size());

		//zb_arr
		pos = (char *)(zb_count_p +1);

		//先将zhuangbeidetail转换出来
		char* zhuangbei_detail_data = new char[ZhuangbeiDetail::max_length()*zb_vec.size()];
		unsigned int zhuangbei_detail_data_len = 0;
		char* p = zhuangbei_detail_data;
		for(int i=0; i<zb_vec.size(); ++i) {
			unsigned int len;
			zb_vec[i].convert2ZhuangbeiDetailAndHton((ZhuangbeiDetail*)p, len);
		
			p += len;
			zhuangbei_detail_data_len += len;
		}
		
		memcpy(pos, zhuangbei_detail_data, zhuangbei_detail_data_len);
		pos += zhuangbei_detail_data_len;

		if(zhuangbei_detail_data != NULL) {
			delete zhuangbei_detail_data;
			zhuangbei_detail_data = NULL;
		}
		gift_bag_out_len = pos - (char*)gift_bag_out;
		return ;
	}


	unsigned int get_length(void) 
	{
		unsigned int length = sizeof(GiftBagInfo);
		length += sizeof(ItemInfo) * item_vec.size(); // item_arr;
		
		length += sizeof(unsigned int); //item_suipian_count
		length += sizeof(ItemSuipianInfo) * item_suipian_vec.size(); // item_suipian_arr;
		
		length += sizeof(unsigned int); //zb_suipian_count
		length += sizeof(ShenbingSuipianInfo) * zb_suipian_vec.size(); // zb_suipian_arr;
		
		length += sizeof(unsigned int); //gift_bag_count
		length += sizeof(GiftBag) * gift_bag_vec.size(); // gift_bag_arr;
		
		length += sizeof(unsigned int);	//zb_count

		length += ZhuangbeiDetail::max_length() * zb_vec.size(); // max zb length

		return length;
	}
};


struct VIP_Info { 
	unsigned int level;					//vip等级
	unsigned int total_charge;			//充值
	VipPrivilegeInfo priv;					//特权

	
	VIP_Info(){	
		bzero(this, sizeof(VIP_Info));
	};

	void convert2VipInfoAndHton(VipPrivilegeInfo* vip_out)
	{
		vip_out->buy_vitality_num = ntohl(priv.buy_vitality_num);
		vip_out->sweep_num = ntohl(priv.sweep_num);
		vip_out->buy_arena_num = ntohl(priv.buy_arena_num);
		vip_out->over_creep_num = ntohl(priv.over_creep_num );
		vip_out->cast_silver_num = ntohl(priv.cast_silver_num );
		vip_out->refresh_reward_num = ntohl(priv.refresh_reward_num);
		vip_out->honor_exchange = priv.honor_exchange;
	}
};

/* 公告 */
//活动公告信息
struct __NoticeActivityInfo {
	NoticeActivityType type;	//类型
	std::string title;			//标题
	std::string time;			//时间
	std::string content;		//内容
};

//公告信息
struct __NoticeInfo{
	std::vector<__NoticeActivityInfo> activity_vec;	//活动公告

	const __NoticeInfo& operator=(const __NoticeInfo& info) {
		activity_vec = info.activity_vec;
		return *this;
	}

	void convert2_notice_info_hton(NoticeInfo* notice_out, unsigned int& notice_out_len)
	{
		notice_out->activity_count = htons(activity_vec.size());

		char *pos = NULL; 	//变长定位后面每个字段的位置
		//NoticeActivityInfo
		pos = (char*)notice_out->activity_arr;
		for(int i=0; i<activity_vec.size(); ++i) {
			//type
			unsigned short *type_p = (unsigned short *)pos;
			*type_p = htons(activity_vec[i].type);

			//title
			pos = (char*)(type_p +1);
			strcpy(pos, activity_vec[i].title.c_str());

			//time
			pos += activity_vec[i].title.length() +1;
			strcpy(pos, activity_vec[i].time.c_str());

			//content
			pos += activity_vec[i].time.length() +1;
			strcpy(pos, activity_vec[i].content.c_str());
			
			pos += activity_vec[i].content.length() +1;
		}
		
		notice_out_len = pos - (char*)notice_out;
		return ;
	}


	unsigned int get_length(void) 
	{
		unsigned int length = sizeof(NoticeInfo);

		//NoticeActivityInfo
		for(int i=0; i<activity_vec.size(); ++i) {
			length += sizeof(unsigned short);	//type
			length += activity_vec[i].title.length() +1;	//time
			length += activity_vec[i].time.length() +1;	//time
			length += activity_vec[i].content.length() +1;	//content
		}
		
		return length;
	}
};


//统计类型 对应statis_info这张表的type
enum StatisType {
	StatisType_Invalid 		= 0,	//无效
	StatisType_Market 		= 1,	//商城
};

#pragma pack() //结束


enum robot_power
{
    robot_power_easy = 1,
    robot_power_mid  = 2,
    robot_power_hard = 3,
};

class ProcServerThread: public SDTwinThreadPool, public ScriptDataProvider
{
public:
	ProcServerThread(PacketQueue* import_q
        , PacketQueue* outport_q
        , std::string db_ip
		, std::string db_username
		, std::string db_psw);
	~ProcServerThread();

    //创建一个机器人，返回hero_id
    void create_robots(int count=10000);
    void change_robot_zb_qianghua();
	
    int create_top_hero(Profession prof, char *account, char *passwd, char *heroname);

protected:	
    unsigned int create_robot(Profession prof, unsigned int level, robot_power power, 
		unsigned int &fight_capacity);
    unsigned int create_robot_assist(unsigned int main_hero_id
        , Profession prof, unsigned int level, robot_power power);
    
    void init_robot(unsigned int hero_id,
        Profession prof, unsigned int level, robot_power power, unsigned int &fight_capacity);
    
    void init_robot_zb(unsigned int main_hero_id
        , unsigned int hero_id,Profession prof, unsigned int level, robot_power power);
    void init_robot_yemanren_zb(unsigned int main_hero_id
        , unsigned int hero_id, unsigned int level, robot_power power);
    void init_robot_cike_zb(unsigned int main_hero_id
        , unsigned int hero_id, unsigned int level, robot_power power);
    void init_robot_fashi_zb(unsigned int main_hero_id
        , unsigned int hero_id, unsigned int level, robot_power power);
    void init_robot_qishi_zb(unsigned int main_hero_id
        , unsigned int hero_id, unsigned int level, robot_power power);
    void init_robot_youxia_zb(unsigned int main_hero_id
        , unsigned int hero_id, unsigned int level, robot_power power);
    void init_robot_mishu_zb(unsigned int main_hero_id
        , unsigned int hero_id, unsigned int level, robot_power power);
    void init_robot_lieshou_zb(unsigned int main_hero_id
        , unsigned int hero_id, unsigned int level, robot_power power);
    void init_robot_shenjianshou_zb(unsigned int main_hero_id
        , unsigned int hero_id, unsigned int level, robot_power power);

    void create_robot_zb(
        unsigned int main_hero_id
        , unsigned int hero_id,unsigned int hero_level, Profession hero_prof
        , ZhuangbeiType zb_type,ZhuangbeiColour zb_color
        ,unsigned int qianghua_goldstar_num,unsigned int qianghua_silverstar_num);
    void get_robot_zb_qianghua_level(robot_power power
        ,unsigned int& qianghua_goldstar_num,unsigned int& qianghua_silverstar_num);

    
    int create_top_assist(Profession prof, unsigned int main_hero_id, 
		unsigned int pos, bool b_assist=true);
	unsigned long long obtain_top_zhuangbei(unsigned int hero_id
    , unsigned int zhuangbei_name_id, __ZhuangbeiDetail detail);
	void create_all_zhuangbei(unsigned int hero_id, Profession prof);
    
    
    //实际处理业务逻辑
	void doIt();
    //处理定时任务
    void doIt2();
    //void StubOptMysql();

    void DealwithPacket(Packet* pack);

    void DealwithRegister(Packet* pack);
    
    void DealwithClientLogin(Packet* pack);

    void DealwithReconnect(Packet* pack);

    void DealwithCreateHero(Packet* pack);

    void DealwithCreateAssit(Packet* pack);

    void DealwithGetHerosInTeam(Packet* pack);

    void DealwithGetZhuangbeiInTeam(Packet* pack);

    //void DealwithGetHeroDetail(Packet* pack);

    void DealwithEquipZhuangbei(Packet* pack);

    void DealwithAdjustFormation(Packet* pack);

    void DealwithGetTaskList(Packet* pack);

    void DealwithGetTaskChessBoard(Packet* pack);

	//翻牌
    void DealwithFlopTaskGrid(Packet* pack);

    void DealwithGetItemsInTeam(Packet* pack);

    void DealwithZhuangbeiQianghua(Packet* pack);
    
    void DealwithZhuangbeiQianghuaHuitui(Packet* pack);
    
    void DealwithZhuangbeiChaijie(Packet* pack);

    void DealwithQueryAllMofaJinghua(Packet* pack);

    void DealwithZBCuiqu(Packet* pack);

    void DealwithZBFumo(Packet* pack);

    //void DealwithZhuangbeiZhuankong(Packet* pack);

    void DealwithXiangBaoshi(Packet* pack);

    void DealwithChaiBaoshi(Packet* pack);

    void DealwithBaoshiHecheng(Packet * pack);

    void DealwithBattle(Packet* pack);
    
    void DealwithChangeTaskStat(Packet* pack);

    void DealwithQueryMarket(Packet* pack);

    void DealwithBuyItemInMarket(Packet* pack);

    void DealwithHeartBeat(Packet* pack);

    void DealwithQueryAnotherTeamInfo(Packet* pack);

    void DealwithChargeVitality(Packet* pack);

    void DealwithQueryAllSuitName(Packet* pack);

    void DealwithQueryAllShenbingList(Packet* pack);

    void DealwithQueryFubenDiaoluo(Packet* pack);

    void DealwithQuerySubtaskList(Packet* pack);

    void DealwithQueryMainTaskReward(Packet* pack);

    void DealwithChargeGold(Packet* pack);
    void DealwithQueryChargePacket(Packet* pack);

    void DealwithQueryTeamSkillList(Packet* pack);
    void DealwithEquipSkill(Packet* pack);
    void DealwithActorLearnSkill(Packet* pack);

    void DealwithQueryTeamShenbingSuipian(Packet* pack);
    void DealwithShenbingHecheng(Packet* pack);

    void DealwithQueryTeamItemSuipian(Packet* pack);
    void DealwithItemHecheng(Packet* pack);

    void DealwithSaodang(Packet* pack);

    void DealwithShenbingJiefengyin(Packet* pack);
    void DealwithShenbingJianding(Packet* pack);
    void DealwithZBJinglian(Packet* pack);
    void DealwithShenbingZhuankeJingwen(Packet* pack);

    void DealwithHeroLevelUp(Packet* pack);
    void DealwithHeroUpgrade(Packet* pack);

    //竞技场
    void DealwithArenaEnter(Packet* pack);
    void DealwithArenaGetGift(Packet* pack);
    void DealwithQueryArenaTeamBasicInfo(Packet* pack);
    void DealwithQueryArenaMarket(Packet* pack);
    void DealwithRefreshArenaChlgList(Packet* pack);
    void DealwithQueryArenaChlgList(Packet* pack);
    void DealwithArenaChallenge(Packet* pack);
    void DealwithArenaMarketBuy(Packet* pack);
    void DealwithArenaBuyChlgTime(Packet* pack);

    //杂货铺
    void DealwithRefreshZahuopu(Packet* pack);
    void DealwithQueryZahuopu(Packet* pack);
    void DealwithZahuopuGetOneItem(Packet* pack);

    //团队boss
    void DealwithTeamBossEnter(Packet* pack);
    void DealwithTeamBossGetGift(Packet* pack);

    //守城
    void DealwithCityGuardEnter(Packet* pack);
    void DealwithCityGuardGetNextMonster(Packet* pack);
    void DealwithCityGuardFight(Packet* pack);

    //英雄榜
    void DealwithTopHeroGetList(Packet* pack);
    void DealwithTopHeroChallenge(Packet* pack);
    void DealwithTopHeroClearCDTime(Packet* pack);

    //巅峰对决
    void DealwithTopArenaQueryStatus(Packet* pack);
    void DealwithTopArenaGuess(Packet* pack);
    void DealwithTopArenaReplay(Packet* pack);
    void DealwithTopArenaQueryGuess(Packet* pack);

    //系统奖励
    void DealwithSystemGiftQuery(Packet* pack);
    void DealwithSystemGiftGetGift(Packet* pack);

    //装备锻造
    void DealwithDuanzaoListQuery(Packet* pack);
    void DealwithDuanzao(Packet* pack);
    void DealwithDuanzaoGroupZBQuery(Packet* pack);

    //地狱副本
    void DealwithQueryHellTaskHeroInfo(Packet* pack);
    void DealwithHellTaskBattle(Packet* pack);

    //
    void DealwithQueryYuanshenOfHero(Packet* pack);

    //
    void DealWithSkillLevelUp(Packet* pack);

    //
    void DealWithGetZhuangbeiAlbum(Packet* pack);

	//查询每日奖励
	void DealwithQueryDailyLoginBonus(Packet* pack);
	
	//查询等级奖励
	void DealwithQueryLevelBonus(Packet* pack);
	
	//触发事件 查询系统奖励礼品
	void DealwithQuerySystemBonus(Packet* pack);
	
	//领取每日登陆奖励
	void DealwithGetDailyLoginBonus(Packet* pack);
	
	//领取等级奖励
	void DealwithGetLevelBonus(Packet* pack);
	
	//领取系统奖励
	void DealwithGetSystemBonus(Packet* pack);

	//查询奖励
	void DealwithQueryBonus(Packet* pack);
	
	//领取奖励
	void DealwithGetBonus(Packet* pack);


	//领取小怪宝箱
	void DealwithGetChessBox(Packet* pack);
	
	//查询关卡评级状态
	void DealwithQueryRateTask(Packet* pack);

	//查询评级奖励
	void DealwithQueryRateTaskBonus(Packet* pack);

	//领取评级奖励
	void DealwithGetRateTaskBonus(Packet* pack);
	
	//查询VIP特权信息
	void DealwithQueryVipPrivilege(Packet* pack);
	
	//升级神兵
	void DealwithUpgradeShenbing(Packet* pack);
	
	//解锁助手
	void DealwithUnlockAssistant(Packet* pack);

	//查询赠送活动
	void DealwithQueryGiftActivity(Packet* pack);

	//领取赠送活动
	void DealwithGetGiftActivity(Packet* pack);

	//查询击杀小怪累计数量
	void DealwithQueryKilledMonster(Packet* pack);

	//查询击杀小怪奖励
	void DealwithQueryKilledMonsterBonus(Packet* pack);

	//领取击杀小怪奖励
	void DealwithGetKilledMonsterBonus(Packet* pack);

	//铸银
	void DealwithCastSilver(Packet* pack);
	
	//查询传奇录
	void DealwithQueryLegendNote(Packet* pack);
	
	//挑战传奇录
	void DealwithChallengeLegendNote(Packet* pack);

	//查询自我挑战信息
	void DealwithQuerySelfChallengeInfo(Packet* pack);
	
	//查询自我挑战排名
	void DealwithQuerySelfChallengeOrder(Packet* pack);
	
	//自我挑战闯关
	void DealwithSelfChallenge(Packet* pack);
	
	//自我挑战能量加成
	void DealwithSelfChallengeGain(Packet* pack);
	
	//领取自我挑战宝箱奖励
	void DealwithGetSelfChallengeBox(Packet* pack);
	
	//查询守护之灵信息
	void DealwithQueryProtectSpirit(Packet* pack);
	
	//挑战守护之灵
	void DealwithChallengeProtectSpirit(Packet* pack);
	
	//领取守护之灵奖励
	void DealwithGetProtectSpiritBonus(Packet* pack);
	
	//膜拜守护之灵
	void DealwithWorshiProtectSpirit(Packet* pack);
	
	//查询探索信息
	void DealwithQueryDiscoverInfo(Packet* pack);

	//探索
	void DealwithDiscover(Packet* pack);

	//探索解封印
	void DealwithUnlockDiscoverSeal(Packet* pack);

	void test_function(void);

	//查询邮件
	void DealwithQueryEmail(Packet* pack);
	//阅读邮件
	void DealwithReadEmail(Packet* pack);
	//写邮件
	void DealwithWriteEmail(Packet* pack);
	//删邮件
	void DealwithDeleteEmail(Packet* pack);

	//查询礼包
	void DealwithQueryGiftBag(Packet* pack);
	//打开礼包
	void DealwithGetGiftBag(Packet* pack);

	//查询当前鉴定 需要材料装
    void DealwithQueryAssessWeapon(Packet* pack);
	//鉴定装备
    void DealwithAssessWeapon(Packet* pack);
	
	//查询洗炼
    void DealwithQueryPurify(Packet* pack);
	//洗炼
    void DealwithPurify(Packet* pack);
	//确认洗炼
    void DealwithConfirmPurify(Packet* pack);

	//产出
    void DealwithQueryOrigin(Packet* pack);

	//查询玩家其他信息
    void DealwithQueryHeroOtherInfo(Packet* pack);

	//查询公告
    void DealwithQueryNotice(Packet* pack);

	//查询教学进度
    void DealwithQueryInstruction(Packet* pack);
	//完成教学
    void DealwithCompleteInstruction(Packet* pack);

    //教堂
    void DealwithChurchLevelUp(Packet* pack);
    bool checkAndChurchLevelUp(unsigned int main_hero_id
        , unsigned int& new_church_level
        , unsigned int& silver_balance
        , unsigned int& dalishi_balance, unsigned int& huagangyan_balance, unsigned int& mocai_balance
        , unsigned int& shuijinshi_balance);

    bool checkAndSkillLevelUp(unsigned int main_hero_id
        , unsigned int owner_hero_id, unsigned int skill_id
        , unsigned int& new_skill_level,unsigned int & silver_balance,ItemInfo& skill_book);

    unsigned int getChurchLevel(unsigned int main_hero_id);

    void getAndRefreshHellTaskHeroInfo(unsigned int hero_id
        ,unsigned int& battle_time_remainder,unsigned int& last_active_task_id);

    bool checkAndHeroLevelUp(unsigned int hero_id
        , unsigned int& silver_balance, unsigned int& levelup_daoju_count, HeroDetail& hero_detail);
    bool checkAndYuanshenLevelUp(unsigned int hero_id
        , unsigned int& silver_balance, unsigned int& levelup_daoju_count, HeroDetail& hero_detail);

    bool checkAndHeroUpgrade(unsigned int hero_id
        , unsigned int& silver_balance, unsigned int& upgrade_daoju_count, HeroDetail& hero_detail);
    bool checkAndYuanshenUpgrade(unsigned int hero_id
        , unsigned int& silver_balance, unsigned int& upgrade_daoju_count, HeroDetail& hero_detail);

    void getAndRefreshHeroArenaBasicInfo(unsigned int hero_id, ArenaTeamBasicInfo& team_info);
        
    void addAssistAtSpecialTaskGrid(
        unsigned int hero_id, unsigned int task_id, unsigned int grid_id
        , unsigned int& assist_id);
    void sendNewAssistJoinRsp(unsigned int main_hero_id, unsigned int assist_id);
    void heroOperatioWhenLevelUpgrade(unsigned int hero_id, unsigned int new_level);

    void getGroupTypeAndName(unsigned int group_id, unsigned int& zb_type, std::string& zb_name);
    void suitDBAttr2CommonAttr( std::string str_attr_vec[],ZBAttr attr_arr[][2]); 

    void InitHero(Profession hero_prof, unsigned int hero_id);
	//is_unlocked 助手是否解锁
    void InitAssistant(unsigned int parent_id, Profession hero_prof, unsigned int hero_id, 
		bool is_unlocked=true);

	//初始化助手装备
    void init_assistant_arms(unsigned int parent_id, Profession hero_prof, unsigned int hero_id);
	
    //最简单的装配，只是将装备的穿着人修改为hero，其他什么都不做
    void __equipZhuangbeiToHero(unsigned long long zhuangbei_id, unsigned int hero_id);

    void updateHeroVitality(unsigned int hero_id);

    void GetHeroList(unsigned int client_id, std::vector<HeroInfo>& heroVec);
    bool GetHeroBasicInfo(unsigned int hero_id, HeroInfo& hero_info);
    void GetHeroDetail(unsigned int hero_id, HeroDetail& hero_detail);
	
	//当level>0时怪物等级不从数据库中取
    void GetMonsterDetail(unsigned int monster_id, GuaiWuData& monster_detail, unsigned int& skill_type, int level=-1);
    //bool EquipZhuangbeiToHero(unsigned int hero_id, unsigned long long zhuangbei_id);
    //检查装备是否属于英雄所在团队，hero_id可以是英雄或助手
    bool canZhuangbeiEquipToHero(unsigned long long zhuangbei_id, unsigned int hero_id);
    bool isZhuangbeiConfilct(std::vector<unsigned long long>& zb_vec);

    //
    unsigned int createAssitant(unsigned int main_hero_id, unsigned int profession, 
    	bool is_unlocked=true, bool is_unlock_able=true);
    unsigned int make_assitant_unlock_able(unsigned int main_hero_id, unsigned int profession);
    unsigned int createYuanshen(unsigned int hero_id);

    unsigned int getSuitId(unsigned int zb_group_id);

    //获取一个item的数量
    unsigned int getItemCountInTeam(unsigned int main_hero_id, unsigned int item_type);
    
    
    //获取某一个装备的详细信息，所有获取装备详细信息都应该通过此接口
    bool getZhuangbeiDetail(unsigned long long zhuangbei_id, unsigned int& owner_id, __ZhuangbeiDetail& detail);
    //
    void unequipZhuangbei(unsigned int hero_id, unsigned long long zhuangbei_id);
    //生成装备
    bool createZhuangbei(unsigned int zhuangbei_name_id, unsigned int suit_id
        , unsigned int owner_hero_id
        , __ZhuangbeiDetail& detail_out, bool b_check_count=true, bool b_battle=false);
    bool keyValue2Attr(std::string key_str,std::string value_str
    , unsigned int zhuangbei_type, unsigned int zhuangbei_colour, unsigned int zhuangbei_level
    ,ZBAttr & attr);
    
    //销毁装备
    void destroyZhuangbei(unsigned long long zhuangbei_id);

    bool checkAndShenbingJiefengyin(unsigned int hero_id, unsigned long long zb_id);
	
    bool checkAndShenbingJianding(unsigned int hero_id, unsigned long long zb_id
        , std::vector<ZBAttr>& identified_attr_vec);
    bool checkAndShenbingZhuankeJingwen(unsigned int hero_id, unsigned long long zb_id
        , std::vector<ZBAttr>& jingwen_attr_vec);

    bool checkAndZBJinglian(unsigned int hero_id, unsigned long long zb_id
        , unsigned long long cailiao_zb_id, unsigned int& jinglian_time
    	, std::vector<ItemInfo> &cailiao_vec
        , std::vector<ZBAttr>& jinglian_attr_vec);
    

    bool isYuanshenZhuangbeiTypeOpen(unsigned int yuanshen_level, unsigned int zhuangbei_type);
    
    //返回0表示出错
    unsigned int chooseZhuangbeiNameIdFromGroupId(unsigned int zb_group_id);
    
    unsigned int randomFromMinMax(unsigned int min, unsigned int max);
    //根据权重随机选择，返回选中的index。如果出现异常一个都没有选中则返回-1
    int RandomByWeight(std::vector<float> weightVec);
    
    void Str2KeyValueList(std::string str
        , std::list<std::string>& key_list, std::list<std::string>& value_list);
    void Str2MinMax(std::string str, unsigned int& min, unsigned int& max);

    bool isMainHeroOfClient(unsigned int client_id, unsigned int main_hero_id);
    bool isHeroBelong2Team(unsigned int hero_id, unsigned int main_hero_id);

    //void getSubTaskList(unsigned int hero_id, std::vector<SubTaskInfo>& sub_task_vec);
    //void getSubTaskDailyZhuangbei(unsigned int hero_id, SubTaskInfo& task);
    //void getSubTaskDailyBaoshi(unsigned int hero_id, SubTaskInfo& task);

    
    int getQianghuaMaxTime(ZhuangbeiType type, ZhuangbeiColour colour, unsigned int level);
    void CheckAndQianghuaZhuangbei(unsigned int hero_id, unsigned long long zhuangbei_id
        , bool b_gold_star, bool& qianghua_sucess, QianghuaInfo& qianghua_info);
    void _QianghuaZhuangbei(unsigned long long zhuangbei_id, unsigned int qianghua_count_now
        , bool b_gold_star, unsigned int cailiao_count
        , QianghuaInfo& qianghua_info);
	//回退装备 返回 true:成功; false:失败
    bool CheckAndHuituiZhuangbei(unsigned int hero_id, unsigned long long zhuangbei_id
        , int keep_count, unsigned char id_arr[]);

	//item_vec 拆解装备所得材料; re_item_vec 返还用在此装备上的材料
    bool CheckAndChaijieZhuangbei(unsigned int hero_id, std::vector<unsigned long long> zb_vec
        , std::vector<ItemInfo>& item_vec, std::vector<ItemInfo>& re_item_vec);

    bool CheckAndZhuangkong(unsigned int hero_id, unsigned long long zhuangbei_id);
    bool canZBTypeBeZhuangkong(unsigned int zhuangbei_type, unsigned int hole_count);
    bool CheckAndXiangbaoshi(unsigned int hero_id, unsigned long long zhuangbei_id
        , unsigned int baoshi_type);
    
    bool CheckAndChaibaoshi(unsigned int hero_id, unsigned long long zhuangbei_id);
    bool CheckAndHechengBaoshi(unsigned int hero_id, unsigned int baoshi_type, int count);

    void ItemsFromChaijieZhuangbei(unsigned int zhuangbei_type, unsigned int zhuangbei_level
        , unsigned int zhuangbei_colour, std::vector<ItemInfo> &item_vec);

    unsigned int ZhuangkongNeedGold(unsigned int zhuangbei_level, unsigned int zhuangbei_colour);

    bool canHeroEnterTaskGrid(unsigned int hero_id, unsigned int task_id, unsigned int grid, bool battle=false);

    //只判断英雄是否可以打该关卡的所有格子
    bool canHeroSaodangTask(unsigned int hero_id, unsigned int task_id);

    bool checkAndBuyItemInArenaMarket(unsigned int hero_id
        ,unsigned int item_type,unsigned int item_count , unsigned int& jifen_balance);
    bool checkAndBuyChlgTime(unsigned int main_hero_id, unsigned int buy_time
        ,unsigned int& chlg_remainder_time, unsigned int& buy_chalg_remainder_time,unsigned int& gold_balance);
    bool checkAndRefreshArenaChlgList(unsigned int main_hero_id, std::vector<ArenaChlgerHeroInfo>& hero_vec
		, unsigned int& gold_balance, unsigned int& silver_balance, ItemInfo& item);
    bool checkArenaChallengeValideAndGetJifen(unsigned int hero_id, unsigned int chlger_id
        , unsigned int& jifen);

    //
    bool checkAndRefreshZahuopu(unsigned int main_hero_id
        , std::vector<ZahuopuItem>& item_vec,unsigned int& silver_balance
        , unsigned int& silver_need_for_manual_refresh, unsigned int &draw_need_coupons);
    void getRandom6ZahuopuItem(std::vector<ZahuopuItem>& item_vec, std::vector<unsigned int>& order_vec);
    bool checkAndZahuopuGetOneItem(unsigned int hero_id
        ,ZahuopuItem& item_zahuopu, unsigned int& item_list_index, unsigned int& gold_balance, unsigned int& coupons_balance);

    bool checkAndRestoreVitality(unsigned int hero_id);
    

	//扣除道具
    bool DeductItem(unsigned int hero_id, unsigned int item_type, unsigned int item_count);
	
    void ObtainItem(unsigned int hero_id, unsigned int item_type, unsigned int item_count);
    unsigned long long ObtainZhuangbei(unsigned int hero_id
        , unsigned int zhuangbei_name_id, __ZhuangbeiDetail detail);
    void ObtainItemSuipian(unsigned int hero_id, unsigned int item_type
        , unsigned int suipian_id, unsigned int suipian_count);
    void ObtainZhuangbeiSuipian(unsigned int hero_id
        , unsigned int zb_group_id, unsigned int suipian_id, unsigned int suipian_count);
    void ObtainGiftBag(unsigned int hero_id, unsigned int gift_id, unsigned int gift_count);

    //计算棋盘格子类型
     void get_grid_type(unsigned int grid_count, std::vector<unsigned int> &normal_monster_vec, 
     	std::vector<unsigned int> &elite_monster_vec, std::vector<unsigned int> &box_vec);
	//刷新棋盘关卡
    void refreshChessBoard(unsigned int hero_id, unsigned int task_id);
	
    void setChessBoardGridDiscovered(unsigned int hero_id, unsigned int task_id, unsigned int grid_id);
    //查询关卡怪物
    void getGuanQiaGuaiWu(unsigned int hero_id, unsigned int task_id, unsigned int grid_id
        , unsigned int pos_vec_1[], unsigned int pos_vec_2[], unsigned int pos_vec_3[]);

    //消耗体力，如果体力不够则返回fasle，否则返回true
    bool subVitality(unsigned int hero_id, unsigned int vit);
	
    //增加体力
    bool addVitality(unsigned int hero_id, unsigned int vit);
    //得经验
    void obtainExp(unsigned int hero_id, unsigned int exp);

    std::vector<float> ajustRatio(std::vector<int> ratio_vec);

    //任务状态迁移是否合法
    bool canTaskStat1ChangeToStat2ByClient(unsigned int stat1, unsigned int stat2);

    //修改任务状态，并且包括任务状态修改所产生的所有其他事务。包括激活下一个任务，激活副本等
    void ChangeTaskStat(unsigned int hero_id, unsigned int task_id, unsigned int stat);


    //竞技场
    void arenaTimer();
    ArenaRoom* getArenaRoom(unsigned int hero_level, unsigned char hard_level);
    void sendArenaUpdateRoomRsp(unsigned int to_hero_id, unsigned int new_enter_hero_id);
    void sendArenaEnterRsp(unsigned int to_client_id, unsigned int to_hero_id
        , std::string fd_account, std::vector<ArenaHero> partner);
    void sendArenaBattleAndGiftRsp(std::vector<ArenaHero> hero_vec);
    void create9ArenaGiftBox(unsigned int hero_id
        , int available_box_count, std::vector<GiftJifen>& gift_vec); //创建9个随机宝箱
    bool getArenaGiftBox(unsigned int hero_id, unsigned int gift_id, GiftJifen& gift_data);

    //团队BOSS
    void teamBossTimer();
    TeamBossRoom* getTeamBossRoom(unsigned int hero_level, unsigned char hard_level);
    void sendTeamBossUpdateRoomRsp(unsigned int to_hero_id, unsigned int new_enter_hero_id);
    void sendTeamBossEnterRsp(unsigned int to_client_id, unsigned int to_hero_id
        , std::string fd_account, std::vector<TeamBossHero> partner);
    void sendTeamBossBattleAndGiftRsp(unsigned int room_level, unsigned char room_hard_level
        , std::vector<TeamBossHero> hero_vec);
    void create9TeamBossGiftBox(unsigned int hero_id
        , int available_box_count, std::vector<GiftData>& gift_vec); //创建9个随机宝箱
    bool getTeamBossGiftBox(unsigned int hero_id, unsigned int gift_id, GiftData& gift_data);

    void queryHeroBattleFormation(unsigned int main_hero_id, unsigned int hero_formation[]);

    //根据需求返回机器人。其中可能是用真实的玩家
    //hero_id_except表示需要排除的id，就是robot不能再用这些id
    void getRobot(unsigned int level_min, unsigned int level_max
        , std::vector<unsigned int> hero_id_except
        ,unsigned int robot_need_count, std::vector<unsigned int>& robot_vec);

    //根据需求返回一个团队BOSS
    Guaiwu getTeamBossMonster(unsigned int level, unsigned int hard_level);

    void PvP(BattleType battle_type, FirstBloodBoard fb, unsigned int hero_a, unsigned int hero_b, 
		FightScript& script_a, FightScript& script_b, unsigned int battle_kind=0);

    void getHeroSkill(unsigned int hero_id, SkillType& skill_type, unsigned int& skill_level);

    //守城
    //进入，返回0表示成功
    int heroEnterCityGuard(unsigned int hero_id
        , unsigned int& last_total_turns, unsigned int& last_total_silver
        , unsigned int& sucess_turn, unsigned int& current_silver);
    void setNextTurnCityGuardMonsterTeamAndSilver(unsigned int hero_id);
    void clearCityGuardMonsterTeamAndSilver(unsigned int hero_id, bool isGiveUp); //
    void getCityGuardCurrentSilverAndNextSilver(unsigned int hero_id
        , unsigned int& current_silver, unsigned int& nextSilver);

    //英雄榜
    //获取除top30外的其他一些英雄
    void getSomeotherHeroInTopList(unsigned int hero_order, std::vector<HeroAndOrder>& hero_vec);

    //巅峰对决
    void topArenaTimer();
    void loadTopArenaData_8_4();
    void loadTopArenaData_4_2();
    void loadTopArenaData_2_1();
	void topArenaReadyPlayer();
    void topArenaFight_8_4();
    void topArenaFight_4_2();
    void topArenaFight_2_1();
    void topArenaAwardGuess(TopArenaRoundType type);
    void topArenaAwardtoSystem(unsigned int hero_id);
    void topArenaAwardTop8();


    //每日守城
    void loadCityGuardData();
    void cityGuardTimer();
    //进入守城界面，查询当前守城活动的状态信息
    void DealWithGetCityGuardCurrentStat(Packet* pack);
    //进入战斗
    //如果boss已经被击杀，立即返回战斗结算。
    void DealWithCityGuardBattle(Packet* pack);
    //退出极限守城
    void DealwithQuitCityGuard(Packet* pack);
    //获取昨日战报
    void DealWithGetCityGuardLastDayReport(Packet* pack);
    //领取自己的奖品
    void DealWithGetMyCityGuardReward(Packet* pack);
    //发送从多少秒前到现在的守城进程通知
    void sendCityGuardProcessNotify(unsigned int hero_id, CityGuardProcessNotifyData notify_data);
    //组装通知数据
    void packCityGuardProcessNotifyData(unsigned int process_period, CityGuardProcessNotifyData& notify_data);
    void openCityGuard(cityGuardStatInfo stat);
    //boss被击杀时的所有处理
    void processCityGuardBossBeKilled(unsigned int hero_kill_boss);
    //生成战报并颁奖
    void cityGuardGenReportAndCalcReward(unsigned int hero_kill_boss);
    //设置下次守城的信息
    void setNextCityGuardInfo(unsigned int cityguard_time_period
        , unsigned int& next_boss_total_hp, unsigned int& next_open_time);
    void cityGuardBattleProc(Packet* pack, unsigned int main_hero_id, bool is_skip_cd);

    
    //
    bool getAndDelHeroCityGuardReward(unsigned int hero_id, std::vector<ItemInfo> &reward_vec);

    //
    void getCityGuardCurrentStatInfo(cityGuardStatInfo& stat_info);
    bool heroHasCityGuardReward(unsigned int hero_id, ItemInfo& reward);
    void getLastCityGuardReport(CityGuardReport& report);
    

    //萃取
    //返回false表示合法性错误，cuiqu_suc表示萃取是否成功
    bool checkAndCuiqu(unsigned int hero_id
        , unsigned long long zb_id, AttrType attr_type, unsigned int attr_value, bool is_fumo_attr
        , bool& cuiqu_suc, unsigned int& zb_type, unsigned int& jinghua_level);
    //返回false表示合法性错误，cuiqu_suc表示萃取是否成功
    bool checkAndFumo(unsigned int hero_id
        , unsigned long long zb_id, MofaJinghua jinghua
        , bool& fm_suc, ZBAttr& new_fm_attr);
    
    
    
    //生成萃取成功率
    unsigned int genZBAttrCuiquRatio(ZhuangbeiType zb_type, ZhuangbeiColour zb_colour, unsigned int zb_level
        , AttrType attr_type, unsigned int attr_value);
    void getZBAttrCuiquLevelRatio(unsigned int zb_level
        , unsigned int& level_1_ratio, unsigned int& level_2_ratio
        , unsigned int& level_3_ratio, unsigned int& level_4_ratio
        );
    unsigned int genZBAttrCuiquLevel(ZhuangbeiType zb_type, unsigned int zb_level
        , AttrType attr_type);
    unsigned int genZBAttrValueFumo(ZhuangbeiType zb_type, unsigned int zb_level
        , AttrType attr_type, unsigned int jinghua_level);
    
    //主线任务掉道具
    void mainTaskDaojuDiaoluo(unsigned int hero_id, unsigned int task_id, unsigned int grid_id
        , std::vector<ItemInfo>& item_vec);

    //查询各等级升级所需的经验
    unsigned int getProfessionLevelNeedExp(unsigned int level);
    //查询元神各等级升级所需的经验
    unsigned int getYuanshenLevelNeedExp(unsigned int level);
	
    //查询各等级升级所需的经验
    //type: HeroType
    unsigned int get_levelup_need_exp(unsigned int type, unsigned int level);
	
    //各职业各等级所对应的基础属性。
    void getProfessionLevelAttr(Profession prof, unsigned int level
        , unsigned int& liliang, unsigned int& minjie, unsigned int& zhili, unsigned int& tineng
        , unsigned int& xueliang, unsigned int& baoji, unsigned int& baoshang, unsigned int& jinengchufa);

    //根据主角职业对装备类型掉率进行合理分配
    ZhuangbeiType randZhuangbeiTypeBaseProfession(Profession prof, std::vector<ZhuangbeiType> zb_type_vec);

    //支线任务点完成
    void subtaskPointFinished(unsigned int hero_id, unsigned int subtask_id);

    //加载套装库
    void loadSuit();
    //加载神兵库
    void loadShenbing();

    //每日全系统刷新定时处理
    void SystemRefreshTimer();
    //玩家日常任务刷新
    void refreshSubtask_Richang();

    //
    void recordClientIDAndFdAccount(unsigned int client_id, std::string fd_account);
    void kickoutFdAcount(std::string fd_account);

    //查询英雄是否第一次通关特殊关卡，需要掉落指定装备
    //如果返回true，则表示是有指定掉落的
    bool checkDiaoluoZBSpecialTaskFirstPass(unsigned int hero_id, unsigned int task_id
     , unsigned int& diaoluo_name_id, unsigned int& diaoluo_group_id);
    //查询英雄是否第一次通关特殊关卡，需要掉落指定道具
    //如果返回true，则表示是有指定掉落的
    bool checkDiaoluoItemSpecialTaskFirstPass(unsigned int hero_id, unsigned int task_id
     , std::vector<ItemInfo>& item_vec);
    //查询英雄是否第一次通关特殊关卡，需要掉落指定银币
    //如果返回true，则表示是有指定掉落的
    bool checkDiaoluoSilverSpecialTaskFirstPass(unsigned int hero_id, unsigned int task_id
     , unsigned int& silver);

    //是否是神兵 -1:不是， >=0:神兵
    int ZBGroupIsShenbing(unsigned int zb_group_id);

    //外部负责delete
    Hero* getBattleScriptHero(FightSUD user_data, unsigned int hero_id, unsigned int cdTime);

	//抽签算法
	int drawbyLot(int *count, const int len);


	/* 关卡评级 */
	//更新关卡评级
	void updateRateTaskStarStat(RateStarType type, unsigned int hero_id, 
		unsigned int task_id, unsigned int star);
	//给关卡评级
	void rateTask(unsigned int hero_id, unsigned int task_id, 
		unsigned int team_pos[], HeroState herostat, 
		unsigned int guaiwu_attach_time, unsigned int attached_round);
	//给章节评级
	void rateChapter(unsigned int hero_id, unsigned int chapter_id);

	//领取评级奖励  返回: false: 失败, true: 成功
	bool get_ratetask_bonus(unsigned int hero_id, unsigned int type, 
		unsigned int task_id, unsigned int star);
	
	//获取英雄vip某个特权信息
	unsigned int getHeroVipInfo(unsigned int hero_id, VIP_FIELD_TYPE type);
	//更新vip某个特权信息
	void updateHeroVipInfo(unsigned int hero_id, VIP_FIELD_TYPE type, unsigned int value);
	//刷新英雄vip特权信息
	void refreshHeroVipInfo(unsigned int hero_id);

	//轮询掉落装备
	unsigned int poll_drop_equip(unsigned int hero_id, std::map<unsigned int, std::vector<unsigned int> > *zb_type_map);
	
	//查询奖励  返回: false: 失败, true: 成功
    bool query_bonus(unsigned int main_hero_id, unsigned int type,
    	std::vector<__BonusInfo> *bonus_vec);
	
	//填充装备奖励 
    void fill_zhuangbei_bonus_old(unsigned int main_hero_id, unsigned int bonus_type,
    	std::vector<__BonusInfo> &bonus_vec);
	
	
	//查询日常奖励  返回: false: 失败, true: 成功
    bool query_daily_bonus(unsigned int main_hero_id, std::vector<__BonusInfo> &bonus_vec);
	
	//查询等级奖励  返回: false: 失败, true: 成功
    bool query_level_bonus(unsigned int main_hero_id, std::vector<__BonusInfo> &bonus_vec);
		
	//查询日常周常奖励  返回: false: 失败, true: 成功
    bool query_system_bonus(unsigned int main_hero_id, unsigned int type, 
    	std::vector<__BonusInfo> &bonus_vec);
	
	//对日常奖励进行排序 返回: false: 失败, true: 成功
    bool sort_bonus_order(std::vector<int> &order_vec);
	
	//填充装备奖励 
    void fill_zhuangbei_bonus(unsigned int main_hero_id, std::vector<__BonusInfo> &bonus_vec);
	
	//赠送装备
	bool present_zhuangbei(unsigned int hero_id, __BonusInfo bonus, unsigned int multiple,
		std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//领取奖励 type:奖励类型 返回: false: 失败, true: 成功
    bool check_and_get_bonus(unsigned int hero_id, unsigned int type, unsigned int id, 
    	std::vector<__ZhuangbeiDetail> &zhuangbei_vec);
	
	//触发系统奖励推送通知
    void send_system_bonus_notify(unsigned int hero_id, unsigned int type, unsigned int id);

	//赠送体力
	bool present_vitality(unsigned int hero_id);
	//赠送vip
	bool present_vip(unsigned int hero_id, unsigned int vip_level);

	//加载数据库所有模板表
	void load_database_template(void);

	//加载每日登陆奖励模板
	void load_daily_bonus_temp(void);
	
	//加载等级奖励模板
	void load_level_bonus_temp(void);
	
	//加载系统奖励模板
	void load_system_bonus_temp(void);

	//加载系统公告
	void load_system_notice(void);

	//加载邮件模板
	void load_email_temp(void);

	//加载商城信息
	void load_market_info(void);

	//加载升级经验配置表
	void load_levelup_exp(void);

	//查询外部邮件信息 
    void query_extern_email_info(__EmailInfo &info);

	//查询邮件  返回: false: 失败, true: 成功
    bool query_email(unsigned int main_hero_id, std::vector<__EmailInfo> &email_vec);

	//阅读邮件 type:奖励类型 返回: false: 失败, true: 成功
    bool read_email(unsigned int hero_id, unsigned int assign_id, 
    	std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//写邮件  返回: false: 失败, true: 成功
    bool write_email(unsigned int main_hero_id, unsigned int receiver_id, char* content);
	
	//删邮件  返回: false: 失败, true: 成功
    bool delete_email(unsigned int main_hero_id, unsigned int assign_id);
	
	//插入邮件 返回: >0:邮件assign_id, 
	//content参数 除了个人邮件以外其他的全部是NULL
	//id参数 在个人邮件是id为0
    unsigned int insert_into_email(unsigned int hero_id, unsigned int addresser_id, 
    	unsigned int type, unsigned int id, char* content=NULL);
	
	//触发系统奖励推送通知
    void send_email_notify(unsigned int hero_id, std::vector<__EmailInfo> &email_vec);
	
	//插入邮件并推送 实际上是合并上面两个函数(insert_into_email, send_email_notify)
    void insert_and_send_email(unsigned int hero_id, unsigned int addresser_id, 
    	unsigned int type, unsigned int id, char* content=NULL);
	
	//填充邮件装备奖励 
    void fill_zhuangbei_email(unsigned int main_hero_id, std::vector<__EmailInfo> &email_vec);
	
	//奖励邮件装备
	bool present_zhuangbei_email(unsigned int hero_id, __EmailInfo email, 
		std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//检查每日产出发送的邮件  暂时只有英雄榜排名产出
    void check_email(unsigned int hero_id);
	

	//打开礼包  返回: false: 失败, true: 成功
    bool get_gift_bag(unsigned int main_hero_id, unsigned int id, 
    	std::vector<__GiftBagInfo> &gift_bag_vec);

	
	//填充礼包礼包  返回: false: 失败, true: 成功
    bool fill_gift_bag(unsigned int main_hero_id, unsigned int id, 
    	std::vector<__GiftBagInfo> &gift_bag_vec);
	
	//奖励礼包装备
	bool present_zhuangbei_gift_bag(unsigned int hero_id, __GiftBagInfo &gift_bag);

	//查询双方排名
	void query_both_two_order(
	    unsigned int hero_a, unsigned int hero_b, unsigned int &order_a, unsigned int &order_b);

	//神兵升级 返回: false: 失败, true: 成功
	bool check_and_upgrade_shenbing(unsigned int hero_id
    , unsigned long long zb_id, unsigned long long cailiao_zb_id
    , std::vector<ItemInfo> &cailiao_vec);

	//根据英雄等级从装备容器中选中一个group_id 返回
    unsigned int selected_group_id(unsigned int hero_level, const std::vector<__zb_type_group_id> &zb_vec);

	//抽取神兵  返回: false: 失败, true: 成功
    bool check_and_draw_shenbing(unsigned int main_hero_id, unsigned int profession_id
        , bool is_times_10, std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//查询装备图鉴  返回: false: 失败, true: 成功
    bool query_zhuangbei_album(unsigned int main_hero_id, unsigned int profession_id
        , std::vector<__DuanzaoGroupZBInfo> &legend_vec
        , std::vector<__DuanzaoGroupZBInfo> &xiyou_vec);

	
	//查询装备模板信息  返回: false: 失败, true: 成功
    bool query_duanzao_group_zb_info(unsigned int group_id, __DuanzaoGroupZBInfo &info);

    //调整主线任务怪物难度
    void adjustMainTaskMonsterDifficulty(unsigned int main_hero_id, unsigned int task_id, GuaiWuData& guaiwu_data);

    //获取主英雄上阵阵容
    void get_team_formation(unsigned int main_hero_id, unsigned int *team_pos_attr, unsigned int *capacity_attr);

    //检查两件装备是否为同一配件  返回: false: 不是, true: 是
    bool check_same_parts(unsigned int group_id_a, unsigned int group_id_b);

	//解锁助手  返回: false: 失败, true: 成功
    bool check_and_unlock_assistant(unsigned int main_hero_id, unsigned int assistant, unsigned int &honor_balance);

	
	//获取孔数  返回: 带孔数
    int get_zb_hole(unsigned int zb_level, unsigned int zb_colour, unsigned int zb_type);
	//获取带孔概率
    void get_zb_hole_percent(unsigned int zb_level, unsigned int zb_colour, unsigned int zb_type,
    	unsigned int &per1, unsigned int &per2, unsigned int &per3);

	//查询赠送活动信息  返回: false: 失败, true: 成功
    bool query_gift_activity(unsigned int hero_id, std::vector<ActivityInfo> &info_vec);
	
	//领取活动赠送  返回: false: 失败, true: 成功
    bool check_and_get_activity(unsigned int hero_id, unsigned int id);

	//更新击杀小怪数量
    void update_killed_monster_counts(unsigned int hero_id, unsigned int common_count, unsigned int elite_count);

	//领取击杀怪物奖励  返回: false: 失败, true: 成功
    bool check_and_get_killed_monster_bonus(unsigned int hero_id, unsigned int type);

	//转换top arena info的字节序
	void hton_top_arena_info(TopArenaInfo& top_info_in, TopArenaInfo& top_info_out);

	//巅峰对决竞猜信息  返回: false: 失败, true: 成功
    bool check_and_guess_top_arena(unsigned int hero_id, unsigned int round, unsigned int group,
    	bool guess_winer, unsigned int wager, unsigned int &silver_balance);
	
	//查询选手身价  返回: false: 失败, true: 成功
    bool query_top_arena_value(unsigned int round, unsigned int group, TopArenaHeroValue &value);

	//检查并铸银  返回: false: 失败, true: 成功
    bool check_and_cast_silver(unsigned int hero_id, unsigned int &gold_balance, unsigned int &silver_balance);

	

	/* 传奇录 */
	//挑战传奇录  返回: false: 失败, true: 成功
    bool check_and_challenge_legend_note(unsigned int main_hero_id, unsigned int group_id, 
    	char **fight_script, int &script_len);
	//挑战传奇录 获取掉落的神兵碎片
    bool get_challenge_legend_suipian(unsigned int main_hero_id, unsigned int group_id, unsigned int times,
    	std::vector<ShenbingSuipianInfo> &sp_vec, std::vector<ItemInfo> &item_vec);
	//根据职业获取传奇录的怪物id
	unsigned int get_special_monster_id(unsigned int prof);
	

	/* 英雄榜 */
	//查询英雄榜 返回: false: 失败, true: 成功
	bool query_top_hero_list(unsigned int main_hero_id, unsigned int &hero_order, unsigned int &next_chlg_time,
		std::vector<HeroAndOrder> &order_vec, std::vector<TopHeroReport> &report_vec);
	//英雄榜挑战 返回: false: 失败, true: 成功
	bool check_and_challenge_top_hero(unsigned int main_hero_id, unsigned int chlg_hero_id,
		unsigned int &order_after_fight, unsigned int &next_chlg_time, 
		char **fight_script, int &script_len);
	//清除挑战冷冷却时间 返回: false: 失败, true: 成功
	bool clear_top_hero_cd_time(unsigned int hero_id, unsigned int &gold_balance, unsigned int &next_chlg_time);


	/* 血色教堂 自我挑战 */
	//自我挑战timer
    void self_challenge_timer();
	//血色教堂自我挑战 返回: false: 失败, true: 成功
    bool check_and_self_challenge(unsigned int main_hero_id, unsigned int difficulty, 
    	unsigned int &energy, unsigned int &checkpoints, bool &is_have_box, unsigned int &silver,
    	 GiftBag &gift_bag, char **fight_script, int &script_len);
	//更新自我挑战宝箱奖励
    void update_self_challenge_box(unsigned int hero_id, unsigned int difficulty);
	//清理自我挑战排名表
    void clear_self_challenge_order(void);
	//更新相关难度排名表
    void update_self_challenge_order(unsigned int hero_id, unsigned int difficulty, unsigned int checkpoints);
	//血色教堂加成 返回: false: 失败, true: 成功
    bool check_and_gain_self_challenge(unsigned int main_hero_id, unsigned int type, unsigned int degree, 
    	unsigned int &energy, unsigned int &hp_gain, unsigned int &attack_gain, unsigned int &defense_gain);
	//领取血色教堂宝箱 返回: false: 失败, true: 成功
    bool check_and_get_self_challenge_box(unsigned int hero_id, ItemInfo &item, std::vector<ItemInfo> &item_vec);
	//获取血色教堂怪物
    void get_self_challenge_monster(unsigned int hero_id, unsigned int checkpoints, unsigned int monster_vec[]);


	/* 守护之灵 */
	//查询守护之灵信息 返回: false: 失败, true: 成功
	bool query_protect_spirit(unsigned int hero_id, unsigned int chapter,
		HeroInfo &first_spirit, ProtectSpiritInfo &current_spirit, 
		bool &is_worship, bool &is_loser);
	//挑战守护之灵 返回: false: 失败, true: 成功
    bool challenge_protect_spirit(unsigned int main_hero_id, unsigned int chapter,
    	char **fight_script, int &script_len);
	//获取守护之灵奖励 返回: false: 失败, true: 成功
    bool check_and_get_protect_spirit_bonus(unsigned int main_hero_id, unsigned int chapter,
    	std::vector<ItemInfo> &item_vec);
	//膜拜守护之灵 返回: false: 失败, true: 成功
    bool check_and_worship_protect_spirit(unsigned int main_hero_id, unsigned int chapter,
    	GiftBag &gift_bag);
	//守护之灵削减比
    int debuff_protect_spirit(unsigned int spirit_id);
	

	/* 探索 */
	//检查并探索 返回: false: 失败, true: 成功
    bool check_and_discover(unsigned int hero_id, bool akey_all, 
    	bool &is_destroyed, unsigned int &remainder_times, 
    	unsigned int &discovered_times, unsigned int &last_modify_timestamp,
    	std::vector<ItemInfo> &item_vec);
	//探索解封印 返回: false: 失败, true: 成功
    bool check_and_unlock_discover_seal(unsigned int main_hero_id, unsigned int profession_id,
        unsigned int &destroy_seal, unsigned int &unlocked_seal, std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//计算英雄战斗力 并入库 ps:元神的战斗力 是附着在真身上的 只计算真身战斗力
	unsigned int calculate_fight_capacity(unsigned int hero_id);

	//获取未装备在身上的装备总数
	unsigned int get_unequipped_counts(unsigned int hero_id);

	
	//翻牌 返回: 网络标识码
	int check_and_flop(unsigned int main_hero_id, unsigned int task, unsigned int grid,
		unsigned int &boss_stat, unsigned int &grid_stat, std::vector<ItemInfo> &item_vec);

	//获取棋盘宝箱 返回: false: 失败, true: 成功
	bool check_and_get_chess_box(unsigned int main_hero_id, unsigned int task, unsigned int grid,
		std::vector<ItemInfo> &item_vec);


	/* 鉴定 */
	//查询鉴定装备的材料装 返回: false: 失败, true: 成功
	bool check_and_query_assess_weapon(unsigned int group_id, 
		std::vector<__DuanzaoGroupZBInfo> &zb_vec);
	//鉴定装备 返回: false: 失败, true: 成功
	bool check_and_assess_weapon(unsigned int hero_id, unsigned long long zb_id, 
		unsigned long long cailiao_zb_id, std::vector<ItemInfo> &cailiao_vec);


	//竞技场排序 
	void arena_bubble_sort(std::vector<ArenaChlgerHeroInfo> &hero_vec);

	//加载装备鉴定表
	void load_assess_weapon(void);

	/* 洗炼 */
	//加载洗炼属性表
	void load_purify_attr_temp(void);
	//洗炼装备 返回: false: 失败, true: 成功
	bool check_and_purify_weapon(unsigned int hero_id, unsigned long long id, 
		unsigned int hole, unsigned int &purify_hole, std::vector<ZBAttr> &attr_vec);
	
	//确认洗炼 返回: false: 失败, true: 成功
	bool check_and_confirm_purify(unsigned int hero_id, unsigned long long id, bool is_confirm);

	
	//调整站位编队 返回: false: 失败, true: 成功
	bool check_and_adjust_formation(unsigned int main_hero_id, const unsigned int id_arr[]);


	/* 心跳包 */
	//查询心跳包信息 返回: false: 失败, true: 成功
	bool query_heart_beat_info(unsigned int hero_id, unsigned int &vitality,
		std::vector<MarqueeMessage> &msg_vec);

	//插入到跑马灯消息队列中 返回: false: 失败, true: 成功
	//type 消息类型; hero_id 消息主体; other_info 消息客体; count 消息客体数量;
	bool insert_marquee_message(unsigned int type, unsigned int hero_id, 
		unsigned int other_info, unsigned int count = 1);
	
	/* 商城 */
	//查询商城信息 返回: false: 失败, true: 成功
	bool query_market_info(unsigned int hero_id, std::vector<MarketItem> &market_vec);
	
	//购买商城物品 返回: false: 失败, true: 成功
    bool check_and_buy_item(unsigned int hero_id, unsigned int id, unsigned int buy_times);
	
	//查询统计信息 返回: false: 失败, true: 成功
	bool query_statis_info(unsigned int hero_id, unsigned int type, unsigned int id, 
		unsigned int &count);
	
	//统计数据 返回: false: 失败, true: 成功
	bool statis_data(unsigned int hero_id, unsigned int type, unsigned int id, 
		unsigned int count);

public:
    //实现ScriptDataProvider
    
    // 查询数据库，完成数据的load，用于模拟战都过程
	// 服务器需要做的的！！！
	//virtual void getHero(Hero& outHero,unsigned int _heroId,ScriptBuilderUserData data);

    // 查询数据库，完成数据的load，用于模拟战都过程
	// 服务器需要做的的！！！,棋盘要用,
	// 如果是怪物，就new GuaiWu，然后初始化返回即可
	// 如果是英雄，那么就new Hero 然后初始化返回即可，如果没有人，那么就返回NULL
	virtual BaseHero* getBaseHero(unsigned int _heroId,ScriptBuilderUserData data);
    
	// 查询数据库，完成数据的load，用于客户端数据的刷新
	// 返回给客户端
	// 服务器需要做的的！！！
	virtual void getHeroDetail(HeroDetail& outData,unsigned int _heroId,ScriptBuilderUserData data);

	// 战斗完毕，这里做数据持久化
	virtual unsigned int fightEndCallback(ScriptBuilderUserData data,bool win);

    virtual void getTeamBloodNum(short& currentNum,short& defaultNum,int& bloodAddPercent,ScriptBuilderUserData data);
    virtual void getHeroCDTime(short& cdStepNum,unsigned heroId,ScriptBuilderUserData data);

	// 获取掉的装备
	// 服务器必须得填
	virtual vector<EquipData> getFightEndEquips(ScriptBuilderUserData data,bool win);
	// 获取掉的物品
	// 服务器必须得填
	virtual vector<ItemInfo> getFightEndItems(ScriptBuilderUserData data,bool win);

    // 获取掉落的Item碎片
	virtual vector<ItemSuipianInfo> getFightEndItemSuiPians(ScriptBuilderUserData data,bool win);
	// 获取掉落的神兵碎片
	virtual vector<ShenbingSuipianInfo> getFightEndShenBingSuiPians(ScriptBuilderUserData data,bool win);

    // 团队作战完毕，这里需要返回获得的战利品
    // 也需要持久化到数据库
    virtual void teamFightEnd(bool win,TeamEndParam parm,TeamEndOutParam& gettedItems);

    int getShangHaiType(Profession prof);

    //是否是boss关卡
    bool isBOSS(unsigned int task_id, unsigned int grid_id);
    //任务的棋盘格子数
    void getTaskChessboardFormat(unsigned int task_id
        , unsigned int& grid_count_heng, unsigned int& grid_count_shu);
    unsigned int getGuaiwuAttackCountOfTask(unsigned int task_id, unsigned int grid_id);
    
    //剔除别的主角职业的专属装备
    std::vector<unsigned int> filterZBGroupCannotUse(
        Profession main_hero_prof, std::vector<unsigned int> zb_group_id_vec);

    //每日定时刷新的情况，根据上一次刷新时间和现在时间判断是否需要刷新
    //daily_refresh_hour_point为系统刷新的整点
    bool isNeedSystemDailyRefresh(unsigned int last_refresh_time, unsigned int daily_refresh_hour_point);

    //根据上一次刷新时间和现在时间判断已经过去几天
    //daily_refresh_hour_point为系统刷新的整点
    int pastSystemDays(unsigned int last_time, unsigned int daily_refresh_hour_point);

    
private:
	PacketQueue* m_import_q;
    PacketQueue* m_outport_q;

    //
    std::string m_db_ip;
	std::string m_db_username;
	std::string m_db_psw;

    //线程专有数据，存储每个线程自己的mysql连接句柄
    pthread_key_t m_mysql_key;

    //竞技场
    Arena m_arena;
    pthread_mutex_t m_arena_gift_pool_mutex;

    //团队BOSS
    TeamBossHall m_teamBossHall;
    pthread_mutex_t m_teamboss_gift_pool_mutex;

    //巅峰对决
    TopArena m_toparena;

    //守城
    CityGuard m_cityguard;

	//公告内容
	__NoticeInfo m_notice;

    //奖励模板库
    std::map<unsigned int, __BonusInfo> m_bonus_map[3];
	//内部邮件模板库
	std::map<unsigned int, __EmailInfo> m_email_internal_map;

    //升级所需经验
    std::map<unsigned int, unsigned int> m_exp_map[2];

    //套装库
    std::vector<SuitNameInfo> m_suit_vec;

    //神兵库
    std::vector<__DuanzaoGroupZBInfo> m_shenbing_vec;

	//跑马灯消息队列
	std::vector<MarqueeMessage> m_msg_vec;
	
	//商城信息库
	std::map<unsigned int, MarketItem> m_market_map;

    //记录已登录玩家信息，互斥登录kickout用
    std::map<unsigned int, std::string> m_clientid2fdaccount;
};



#endif //_PROCSERVERTHREAD_H_


