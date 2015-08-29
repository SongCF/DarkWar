
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

//�ͻ���ƽ̨
#define PLATFORM_IOS 1
#define PLATFORM_ANDROID 2


//������ʾ��Сʱ����
#define iOSUpdatePromptTimeGap 86400
#define iOSCommentPromptTimeGap 86400


//ǿ���������
#define QianghuaRatio_tie  37
#define QianghuaRatio_tong 50
#define QianghuaRatio_yin  10
#define QianghuaRatio_jin  3
//ǿ������Ч��
#define QianghuaPercent_tie 3
#define QianghuaPercent_tong 6
#define QianghuaPercent_yin 12
#define QianghuaPercent_jin 24

//ϵͳ�������һ��װ��ǿ�����ٴ�
#define Qianghua_time_max 9

//��ѯ����װ���ؿ���
#define MAX_POLL_DROP_EQUIP_TASK 40

//boss�ص���װ������
#define DROP_ZHUANGBEI_RATIO 30

//����ƶ���������
#define MAX_MARQUEE_MESSAGE_COUNT 10


//��������ڱ���ID
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
    
    SMID_CityGuard  = 1207,		//�س�boss
};


//Ӣ�����
enum HeroType
{
	HeroType_Invalid	= 0, 	//��Ч
    HeroType_Hero  		= 1,	//Ӣ��
    HeroType_Soul 		= 2,	//Ԫ��
};


//������
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
    //��һ���˽����ʱ�伴Ϊ���䴴��ʱ��
    time_t create_time;

    std::vector<ArenaHero> hero_vec;
};
struct Arena
{
    //����arena�ṹ�е����ݵĶ�д���������Ȼ�ȡ����
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


//ս��userdata �ű��������
struct FightSUD
{ 
	bool boss_freezy;
	bool b_check_count;					//�Ƿ���Ҫ��鱳����δװ���ĸ����������� Ĭ��Ϊtrue
	BattleType battle_type;
	unsigned int battle_kind;			//BattleType_ProtectSpirit: ս��B������ 0:hero,  1:monster 
										//BattleType_SelfChallenge: ��Ӧ��Ӧ�Ѷ�SelfChlgDiffType
										//BattleType_CityGuard: ��Ӧ�Ƿ�Ǯ���� 0:û�� 1:��Ǯ����
	unsigned int heroId;
	unsigned int taskId;
	unsigned int gridId;
	unsigned int fightIndex;
	// ����pvp���εȼ����Ƶ�
	// �򶷵�ʱ��˫���ĵȼ���ȡ��ͬ�ĵȼ�������ƽ��ֵ
	unsigned int hero_a_level;
	unsigned int hero_b_level;			//BattleType_SelfChallenge: ��Ӧ����ؿ�
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


//�Ŷ�BOSS
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
    //��һ���˽����ʱ�伴Ϊ���䴴��ʱ��
    time_t create_time;

    std::vector<TeamBossHero> hero_vec;
};
struct TeamBossHall
{
    //����arena�ṹ�е����ݵĶ�д���������Ȼ�ȡ����
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



//�۷�Ծ�
/********************�Ǻ�***************************
*
*�ǺŰ�Χ�Ĵ����Ǵ�ɾ�Ĵ���
*
********************�Ǻ�***************************/
#if 0
struct TopArenaHeroInfo
{
    HeroInfo hero_info;

    //Ӣ�۰�����
    unsigned int top_hero_order;
};
struct SPCmd_topArenaQueryStatusRsp
{
    int err_code;

    //stat ��Ϊ����Ч(0)����������(1)������������(2)���������(3)
    unsigned int stat;

    //����ѡ��0~2Ϊa������սb��3~5, ������
    TopArenaHeroInfo hero_info_arr[30];

    //��Ҿ���
    bool hasGuess;
    bool guess_arr[5];

    //����Ǳ������״̬��������Ϊʤ����ϵ
    //Ϊtrue��ʾa��ʤ����֮b��ʤ
    bool game_result_arr[15];

    //����Ǳ������״̬�������ʾ�Ƿ��н�
    unsigned int gold_count;
};

struct SPCmd_topArenaGuess
{
    //Ϊtrue��ʾa��ʤ����֮b��ʤ
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
    //ռλ
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

struct TopArenaHeroValue { //С���Ӣ�����
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
    //����topArena�ṹ�е����ݵĶ�д���������Ȼ�ȡ����
    pthread_mutex_t mutex;

	TopArenaRoundType round;    		//��������, 0:δ��ʼ, 1:8��4, 2:4��2, 3:2��1
	//bool last_guess_clear;				//���2��1���½����������־λ false: ����� true:δ���
	unsigned int next_top_arena_time;	//�´��۷�Ծ�����ʱ�� 0:�������, >0:�δ��ʼ
	
	TopArenaStat stat[3];

	//bool round_info_record[3]; 	//�Ƿ��б�����Ϣ, true:��, false:û��; �±�0:8��4, 1:4��2, 2:2��1  
	
	TopArenaInfo player_arr_8_4[4];  	//8��4ѡ��
	TopArenaInfo player_arr_4_2[2];  	//4��2ѡ��
	TopArenaInfo player_arr_2_1[1];  	//2��1ѡ��

	TopArenaHeroValue value_arr_8_4[4];		//8��4��С��Ӣ�����
	TopArenaHeroValue value_arr_4_2[2];		//4��2��С��Ӣ�����
	TopArenaHeroValue value_arr_2_1[1];		//2��1��С��Ӣ�����

	TopArenaScriptInfo script_arr_8_4[4]; 	//8��4ս���ű�
	TopArenaScriptInfo script_arr_4_2[2];	//4��2ս���ű�
	TopArenaScriptInfo script_arr_2_1[1];	//2��1ս���ű�

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
    //�ڸ���ʱ�俪ʼʱ��boss��Ѫ��
    unsigned int boss_hp;

    //�ڸ���ʱ���ڣ����з�����ս���˺���¼
    std::list<CityGuardShanghai> shanghai_list;
};
struct CityGuardHero
{
    unsigned int hero_id;
};

//����ս��֪ͨ��������ѯ���������
#define CityGuardHeroNotifyPeriodSec 10

//����ս���У��ͻ���ÿ���Ų��ż����˺�����
#define CityGuardHeroNotifyShowShanghaiPerSec 4

//�س�
struct CityGuard
{
    //���нṹ�е����ݵĶ�д���������Ȼ�ȡ����
    pthread_mutex_t mutex;

    //��ǰ�سǻ�Ƿ���
    bool is_opening;
    //��¼boss��ʼ��Ѫ������ʹ��
    unsigned int boss_total_hp;
    //��¼����ʱ���
    time_t opening_timestamp;

    //�سǽ���ʱ��Ҫ����֪ͨ��֪boss��������Ҷ���
    std::list<CityGuardHero> hero_wait_for_boss_dead_notify_table[CityGuardHeroNotifyPeriodSec];


    //����ս���˺��ļ�¼
    std::map<time_t, CityGuardBattleRecord> battle_record_map;

    //���в����سǻ����ҵ��б�
    //��������ʱ��ʹ��bos_hash�㷨���������±�
    //����������ѯ���͡�ÿ����һ����������������ҷ���һ��֪ͨ��
    std::list<CityGuardHero> hero_notify_table[CityGuardHeroNotifyPeriodSec];
    unsigned int next_index_to_send_notify; //��һ��Ҫ����֪ͨ�������±ꡣ

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
    //�ͻ�����Ҫ�ö������������˺����̡�
    unsigned int play_time; //Ϊ0��ʾ����Ч��

    //��play_timeʱ�����boss��Ѫ����Ҫ������ô��
    unsigned int boss_hp_start; //Ϊ0��ʾboss�ѱ���ɱ
    unsigned int boss_hp_end;

    //��Ҫ���ŵ��˺�
    std::vector<CityGuardAnotherHeroShanghai> another_hero_shanghai;
};

//VIP������Ϣ
enum VIP_FIELD_TYPE {
	VIP_FIELD_TYPE_VITALITY = 1,
	VIP_FIELD_TYPE_SWEEP = 2,
	VIP_FIELD_TYPE_ARENA = 3,
	VIP_FIELD_TYPE_REFRESH_REWAED = 4,
	VIP_FIELD_TYPE_CAST_SILVER = 5,
};

#pragma pack(1) //���յ��ֽڶ���

struct __PurifyAttr{
    unsigned int star_level;
    unsigned int weight;
    unsigned int min_value;
    unsigned int max_value;
};

struct __ZhuangbeiDetail
{
    unsigned long long zhuangbei_id;
    unsigned int group_id; //װ��ģ����
    unsigned int profession_id;

    unsigned int zhuangbei_type;
    char zhuangbei_name[ZhuangbeiNameMaxLen+1];
    unsigned int zhuangbei_colour;
    char zhuangbei_pic[ZhuangbeiPicMaxLen+1];

    //װ����˭������
    unsigned int zhuangbei_hero_id;

    unsigned int zhuangbei_level;
    unsigned int hole;
    unsigned int diamond_type_1;
    unsigned int diamond_type_2;
    unsigned int diamond_type_3;
    
    unsigned int gongji_min;
    unsigned int gongji_max;
    unsigned int hujia;

    //ǿ����Ϣ
    QianghuaData qianghua_data;

    //��ħ��Ϣ
    bool has_fumo;
    ZBAttr fumo_attr;

    //
    std::vector<ZBAttr> attr_vec;
    

    std::string desc;

    //����Ǵ���װ������о�����Ϣ
    unsigned char jinglian_level;
    std::vector<ZBAttr> jinglian_attr_vec;
    //std::vector<ZBAttr> next_jinglian_attr_vec;

    //��������������н��ӡ�;���׭����Ϣ
    bool is_shenbing;
    bool is_unlocked; //�Ƿ���ӡ
    std::vector<ZBAttr> identified_attr; //�Ѽ�������������
    bool has_jingwen;
    ZBAttr jingwen_attr_arr[2];

	std::vector<ZBAttr> purify_attr; 	 //����ϴ������ 

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

        //�������ϡ��װ�����򵽴˽���
        if(zhuangbei_colour < ZhuangbeiColour_Xiyou) {
            detail_out_len = desc_p+desc.length()+1 - (char*)detail_out;
            return;
        }

        //����
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

		//������Ǵ���װ�����򵽴˽���
        if(zhuangbei_colour != ZhuangbeiColour_Chuanqi) {
            detail_out_len = (char*)(is_unlocked_p +1) - (char*)detail_out;
            return;
        }
		
		
        //
        bool* is_shenbing_p = (bool*)(is_unlocked_p +1);

		//����װ��Ϊϡ�У������Ϊ����Ĵ���ʽ
		//*is_shenbing_p = is_shenbing;
        *is_shenbing_p = is_shenbing = true;

        //�����������򵽴˽���
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
    unsigned int profession_id;    		// �󶨵�ְҵID
	unsigned int next_group_id;			//��������һ�ȼ���װ��id
	unsigned int need_expend_group_id; 	//��������������ɵ�װ��id
    unsigned int ratio; 				//��������

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


//��ȡװ��ʱ�����ýṹ��
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

//������Ϣ
struct __BonusInfo {
	BonusType type;
	
    bool is_get;		   		//�Ƿ���ȡ�ν��� false:δ��ȡ, true:����ȡ
    bool enable;		   		//�Ƿ������ȡ���� false:������ȡ true:������ȡ
    unsigned int bonus_id; 		//ÿ�ֽ�����ID

	/**************** �Ǻ�֮���Ϊ�ճ��������������  ****************/
    unsigned int w_day; 		//ˢ������
    unsigned int vip_level; 	//>0:��ȡ�౶������vip�ȼ��ֽ��� 0:ֻ��1������ 
    unsigned int multiple; 		//����
    std::string description; 	//�Խ���������
    /*****************************************************************/
	
    unsigned int ran_zb_count;	// ���װ������

	//ÿ��ȡ���ݵ�ʱ��zb_vec��zb_ranflg_vec ��3��ְҵ��ȡһ��
	unsigned int prof_zb_ran[Profession_Fashi];	//����3��ְҵ���������
	std::vector<unsigned int> prof_zb_group_vec[Profession_Fashi];	//����3��ְҵ��group_id Ұ���� �̿� ��ʦ

	/* ����Ϊ������Ϣ */
	unsigned int vitality;								//0:û��������Ϣ >0: ����������ֵ
    std::vector<ItemInfo> item_vec;						//���߲���
    std::vector<ItemSuipianInfo> item_suipian_vec;		//������Ƭ
    std::vector<ShenbingSuipianInfo> zb_suipian_vec;	//װ����Ƭ
    std::vector<GiftBag> gift_bag_vec;					//���
	std::vector<__DuanzaoGroupZBInfo> zb_vec;			//ָ��װ��

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
		
		char *pos = NULL; 	//�䳤��λ����ÿ���ֶε�λ��
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



//�ڲ��ʼ�ID ��Ӧemail_internal_temp���ű��ID
enum EmailID
{	
    EmailID_Invalid = 0,
		
	//�۷�Ծ�����	
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

	//�۷�Ծ�ѡ�ְ佱
    EmailID_TopArena_Top8 = 213,
    EmailID_TopArena_Top4 = 214,
    EmailID_TopArena_Top2 = 215,
    EmailID_TopArena_Top1 = 216,
    
	//Ӣ�۰���������
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

	//������ս��������
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
		
	//װ������
	//EmailID_Present_Weapon = 601,
	//�״γ�ֵ����
	EmailID_First_Charge = 602,
	//��������
	EmailID_PresentVitality = 603,
		

};


//�ʼ���Ϣ
struct __EmailInfo{
	EmailType type;
	
	bool is_read;				//�Ƿ��Ķ� false:δ��, true:�Ѷ�
	unsigned int email_id;		//�ʼ�ģ��id
	unsigned int assign_id;		//ÿ����Ҷ�Ӧ���ʼ�id
	unsigned int addresser_id;	//������id 0:Ϊϵͳ >0:���

	std::string addresser;		//����������
	std::string content;	
	std::string summary;	
	

    unsigned int ran_zb_count;	// ���װ������

	//ÿ��ȡ���ݵ�ʱ��zb_vec��zb_ranflg_vec ��3��ְҵ��ȡһ��
	unsigned int prof_zb_ran[Profession_Fashi];	//����3��ְҵ���������
	std::vector<unsigned int> prof_zb_group_vec[Profession_Fashi];	//����3��ְҵ��group_id Ұ���� �̿� ��ʦ

	/* ����Ϊ������Ϣ */
	unsigned int vitality;								//0:û��������Ϣ >0: ����������ֵ
    std::vector<ItemInfo> item_vec;						//���߲���
    std::vector<ItemSuipianInfo> item_suipian_vec;		//������Ƭ
    std::vector<ShenbingSuipianInfo> zb_suipian_vec;	//װ����Ƭ
    std::vector<GiftBag> gift_bag_vec;					//���
	std::vector<__DuanzaoGroupZBInfo> zb_vec;			//ָ��װ��

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

		char *pos = NULL; 	//�䳤��λ����ÿ���ֶε�λ��
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


//�������
enum GiftBagType {
	GiftBagType_All 		= 0,	//�����������
	GiftBagType_Random 		= 1,	//������
	GiftBagType_Invalid 	= 0xFF,	//��Ч
};

//�����Ϣ
struct __GiftBagInfo{

    unsigned int ran_zb_count;	// ���װ������

	//ÿ��ȡ���ݵ�ʱ��zb_vec��zb_ranflg_vec ��3��ְҵ��ȡһ��
	unsigned int prof_zb_ran[Profession_Fashi];	//����3��ְҵ���������
	std::vector<unsigned int> prof_zb_group_vec[Profession_Fashi];	//����3��ְҵ��group_id Ұ���� �̿� ��ʦ
	
	unsigned int vitality;								//0:û��������Ϣ >0: ����������ֵ
    std::vector<ItemInfo> item_vec;						//���߲���
    std::vector<ItemSuipianInfo> item_suipian_vec;		//������Ƭ
    std::vector<ShenbingSuipianInfo> zb_suipian_vec;	//װ����Ƭ
    std::vector<GiftBag> gift_bag_vec;					//���
	std::vector<__ZhuangbeiDetail> zb_vec;			//ָ��װ��

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
		
		char *pos = NULL; 	//�䳤��λ����ÿ���ֶε�λ��
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

		//�Ƚ�zhuangbeidetailת������
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
	unsigned int level;					//vip�ȼ�
	unsigned int total_charge;			//��ֵ
	VipPrivilegeInfo priv;					//��Ȩ

	
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

/* ���� */
//�������Ϣ
struct __NoticeActivityInfo {
	NoticeActivityType type;	//����
	std::string title;			//����
	std::string time;			//ʱ��
	std::string content;		//����
};

//������Ϣ
struct __NoticeInfo{
	std::vector<__NoticeActivityInfo> activity_vec;	//�����

	const __NoticeInfo& operator=(const __NoticeInfo& info) {
		activity_vec = info.activity_vec;
		return *this;
	}

	void convert2_notice_info_hton(NoticeInfo* notice_out, unsigned int& notice_out_len)
	{
		notice_out->activity_count = htons(activity_vec.size());

		char *pos = NULL; 	//�䳤��λ����ÿ���ֶε�λ��
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


//ͳ������ ��Ӧstatis_info���ű��type
enum StatisType {
	StatisType_Invalid 		= 0,	//��Ч
	StatisType_Market 		= 1,	//�̳�
};

#pragma pack() //����


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

    //����һ�������ˣ�����hero_id
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
    
    
    //ʵ�ʴ���ҵ���߼�
	void doIt();
    //����ʱ����
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

	//����
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

    //������
    void DealwithArenaEnter(Packet* pack);
    void DealwithArenaGetGift(Packet* pack);
    void DealwithQueryArenaTeamBasicInfo(Packet* pack);
    void DealwithQueryArenaMarket(Packet* pack);
    void DealwithRefreshArenaChlgList(Packet* pack);
    void DealwithQueryArenaChlgList(Packet* pack);
    void DealwithArenaChallenge(Packet* pack);
    void DealwithArenaMarketBuy(Packet* pack);
    void DealwithArenaBuyChlgTime(Packet* pack);

    //�ӻ���
    void DealwithRefreshZahuopu(Packet* pack);
    void DealwithQueryZahuopu(Packet* pack);
    void DealwithZahuopuGetOneItem(Packet* pack);

    //�Ŷ�boss
    void DealwithTeamBossEnter(Packet* pack);
    void DealwithTeamBossGetGift(Packet* pack);

    //�س�
    void DealwithCityGuardEnter(Packet* pack);
    void DealwithCityGuardGetNextMonster(Packet* pack);
    void DealwithCityGuardFight(Packet* pack);

    //Ӣ�۰�
    void DealwithTopHeroGetList(Packet* pack);
    void DealwithTopHeroChallenge(Packet* pack);
    void DealwithTopHeroClearCDTime(Packet* pack);

    //�۷�Ծ�
    void DealwithTopArenaQueryStatus(Packet* pack);
    void DealwithTopArenaGuess(Packet* pack);
    void DealwithTopArenaReplay(Packet* pack);
    void DealwithTopArenaQueryGuess(Packet* pack);

    //ϵͳ����
    void DealwithSystemGiftQuery(Packet* pack);
    void DealwithSystemGiftGetGift(Packet* pack);

    //װ������
    void DealwithDuanzaoListQuery(Packet* pack);
    void DealwithDuanzao(Packet* pack);
    void DealwithDuanzaoGroupZBQuery(Packet* pack);

    //��������
    void DealwithQueryHellTaskHeroInfo(Packet* pack);
    void DealwithHellTaskBattle(Packet* pack);

    //
    void DealwithQueryYuanshenOfHero(Packet* pack);

    //
    void DealWithSkillLevelUp(Packet* pack);

    //
    void DealWithGetZhuangbeiAlbum(Packet* pack);

	//��ѯÿ�ս���
	void DealwithQueryDailyLoginBonus(Packet* pack);
	
	//��ѯ�ȼ�����
	void DealwithQueryLevelBonus(Packet* pack);
	
	//�����¼� ��ѯϵͳ������Ʒ
	void DealwithQuerySystemBonus(Packet* pack);
	
	//��ȡÿ�յ�½����
	void DealwithGetDailyLoginBonus(Packet* pack);
	
	//��ȡ�ȼ�����
	void DealwithGetLevelBonus(Packet* pack);
	
	//��ȡϵͳ����
	void DealwithGetSystemBonus(Packet* pack);

	//��ѯ����
	void DealwithQueryBonus(Packet* pack);
	
	//��ȡ����
	void DealwithGetBonus(Packet* pack);


	//��ȡС�ֱ���
	void DealwithGetChessBox(Packet* pack);
	
	//��ѯ�ؿ�����״̬
	void DealwithQueryRateTask(Packet* pack);

	//��ѯ��������
	void DealwithQueryRateTaskBonus(Packet* pack);

	//��ȡ��������
	void DealwithGetRateTaskBonus(Packet* pack);
	
	//��ѯVIP��Ȩ��Ϣ
	void DealwithQueryVipPrivilege(Packet* pack);
	
	//�������
	void DealwithUpgradeShenbing(Packet* pack);
	
	//��������
	void DealwithUnlockAssistant(Packet* pack);

	//��ѯ���ͻ
	void DealwithQueryGiftActivity(Packet* pack);

	//��ȡ���ͻ
	void DealwithGetGiftActivity(Packet* pack);

	//��ѯ��ɱС���ۼ�����
	void DealwithQueryKilledMonster(Packet* pack);

	//��ѯ��ɱС�ֽ���
	void DealwithQueryKilledMonsterBonus(Packet* pack);

	//��ȡ��ɱС�ֽ���
	void DealwithGetKilledMonsterBonus(Packet* pack);

	//����
	void DealwithCastSilver(Packet* pack);
	
	//��ѯ����¼
	void DealwithQueryLegendNote(Packet* pack);
	
	//��ս����¼
	void DealwithChallengeLegendNote(Packet* pack);

	//��ѯ������ս��Ϣ
	void DealwithQuerySelfChallengeInfo(Packet* pack);
	
	//��ѯ������ս����
	void DealwithQuerySelfChallengeOrder(Packet* pack);
	
	//������ս����
	void DealwithSelfChallenge(Packet* pack);
	
	//������ս�����ӳ�
	void DealwithSelfChallengeGain(Packet* pack);
	
	//��ȡ������ս���佱��
	void DealwithGetSelfChallengeBox(Packet* pack);
	
	//��ѯ�ػ�֮����Ϣ
	void DealwithQueryProtectSpirit(Packet* pack);
	
	//��ս�ػ�֮��
	void DealwithChallengeProtectSpirit(Packet* pack);
	
	//��ȡ�ػ�֮�齱��
	void DealwithGetProtectSpiritBonus(Packet* pack);
	
	//Ĥ���ػ�֮��
	void DealwithWorshiProtectSpirit(Packet* pack);
	
	//��ѯ̽����Ϣ
	void DealwithQueryDiscoverInfo(Packet* pack);

	//̽��
	void DealwithDiscover(Packet* pack);

	//̽�����ӡ
	void DealwithUnlockDiscoverSeal(Packet* pack);

	void test_function(void);

	//��ѯ�ʼ�
	void DealwithQueryEmail(Packet* pack);
	//�Ķ��ʼ�
	void DealwithReadEmail(Packet* pack);
	//д�ʼ�
	void DealwithWriteEmail(Packet* pack);
	//ɾ�ʼ�
	void DealwithDeleteEmail(Packet* pack);

	//��ѯ���
	void DealwithQueryGiftBag(Packet* pack);
	//�����
	void DealwithGetGiftBag(Packet* pack);

	//��ѯ��ǰ���� ��Ҫ����װ
    void DealwithQueryAssessWeapon(Packet* pack);
	//����װ��
    void DealwithAssessWeapon(Packet* pack);
	
	//��ѯϴ��
    void DealwithQueryPurify(Packet* pack);
	//ϴ��
    void DealwithPurify(Packet* pack);
	//ȷ��ϴ��
    void DealwithConfirmPurify(Packet* pack);

	//����
    void DealwithQueryOrigin(Packet* pack);

	//��ѯ���������Ϣ
    void DealwithQueryHeroOtherInfo(Packet* pack);

	//��ѯ����
    void DealwithQueryNotice(Packet* pack);

	//��ѯ��ѧ����
    void DealwithQueryInstruction(Packet* pack);
	//��ɽ�ѧ
    void DealwithCompleteInstruction(Packet* pack);

    //����
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
	//is_unlocked �����Ƿ����
    void InitAssistant(unsigned int parent_id, Profession hero_prof, unsigned int hero_id, 
		bool is_unlocked=true);

	//��ʼ������װ��
    void init_assistant_arms(unsigned int parent_id, Profession hero_prof, unsigned int hero_id);
	
    //��򵥵�װ�䣬ֻ�ǽ�װ���Ĵ������޸�Ϊhero������ʲô������
    void __equipZhuangbeiToHero(unsigned long long zhuangbei_id, unsigned int hero_id);

    void updateHeroVitality(unsigned int hero_id);

    void GetHeroList(unsigned int client_id, std::vector<HeroInfo>& heroVec);
    bool GetHeroBasicInfo(unsigned int hero_id, HeroInfo& hero_info);
    void GetHeroDetail(unsigned int hero_id, HeroDetail& hero_detail);
	
	//��level>0ʱ����ȼ��������ݿ���ȡ
    void GetMonsterDetail(unsigned int monster_id, GuaiWuData& monster_detail, unsigned int& skill_type, int level=-1);
    //bool EquipZhuangbeiToHero(unsigned int hero_id, unsigned long long zhuangbei_id);
    //���װ���Ƿ�����Ӣ�������Ŷӣ�hero_id������Ӣ�ۻ�����
    bool canZhuangbeiEquipToHero(unsigned long long zhuangbei_id, unsigned int hero_id);
    bool isZhuangbeiConfilct(std::vector<unsigned long long>& zb_vec);

    //
    unsigned int createAssitant(unsigned int main_hero_id, unsigned int profession, 
    	bool is_unlocked=true, bool is_unlock_able=true);
    unsigned int make_assitant_unlock_able(unsigned int main_hero_id, unsigned int profession);
    unsigned int createYuanshen(unsigned int hero_id);

    unsigned int getSuitId(unsigned int zb_group_id);

    //��ȡһ��item������
    unsigned int getItemCountInTeam(unsigned int main_hero_id, unsigned int item_type);
    
    
    //��ȡĳһ��װ������ϸ��Ϣ�����л�ȡװ����ϸ��Ϣ��Ӧ��ͨ���˽ӿ�
    bool getZhuangbeiDetail(unsigned long long zhuangbei_id, unsigned int& owner_id, __ZhuangbeiDetail& detail);
    //
    void unequipZhuangbei(unsigned int hero_id, unsigned long long zhuangbei_id);
    //����װ��
    bool createZhuangbei(unsigned int zhuangbei_name_id, unsigned int suit_id
        , unsigned int owner_hero_id
        , __ZhuangbeiDetail& detail_out, bool b_check_count=true, bool b_battle=false);
    bool keyValue2Attr(std::string key_str,std::string value_str
    , unsigned int zhuangbei_type, unsigned int zhuangbei_colour, unsigned int zhuangbei_level
    ,ZBAttr & attr);
    
    //����װ��
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
    
    //����0��ʾ����
    unsigned int chooseZhuangbeiNameIdFromGroupId(unsigned int zb_group_id);
    
    unsigned int randomFromMinMax(unsigned int min, unsigned int max);
    //����Ȩ�����ѡ�񣬷���ѡ�е�index����������쳣һ����û��ѡ���򷵻�-1
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
	//����װ�� ���� true:�ɹ�; false:ʧ��
    bool CheckAndHuituiZhuangbei(unsigned int hero_id, unsigned long long zhuangbei_id
        , int keep_count, unsigned char id_arr[]);

	//item_vec ���װ�����ò���; re_item_vec �������ڴ�װ���ϵĲ���
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

    //ֻ�ж�Ӣ���Ƿ���Դ�ùؿ������и���
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
    

	//�۳�����
    bool DeductItem(unsigned int hero_id, unsigned int item_type, unsigned int item_count);
	
    void ObtainItem(unsigned int hero_id, unsigned int item_type, unsigned int item_count);
    unsigned long long ObtainZhuangbei(unsigned int hero_id
        , unsigned int zhuangbei_name_id, __ZhuangbeiDetail detail);
    void ObtainItemSuipian(unsigned int hero_id, unsigned int item_type
        , unsigned int suipian_id, unsigned int suipian_count);
    void ObtainZhuangbeiSuipian(unsigned int hero_id
        , unsigned int zb_group_id, unsigned int suipian_id, unsigned int suipian_count);
    void ObtainGiftBag(unsigned int hero_id, unsigned int gift_id, unsigned int gift_count);

    //�������̸�������
     void get_grid_type(unsigned int grid_count, std::vector<unsigned int> &normal_monster_vec, 
     	std::vector<unsigned int> &elite_monster_vec, std::vector<unsigned int> &box_vec);
	//ˢ�����̹ؿ�
    void refreshChessBoard(unsigned int hero_id, unsigned int task_id);
	
    void setChessBoardGridDiscovered(unsigned int hero_id, unsigned int task_id, unsigned int grid_id);
    //��ѯ�ؿ�����
    void getGuanQiaGuaiWu(unsigned int hero_id, unsigned int task_id, unsigned int grid_id
        , unsigned int pos_vec_1[], unsigned int pos_vec_2[], unsigned int pos_vec_3[]);

    //����������������������򷵻�fasle�����򷵻�true
    bool subVitality(unsigned int hero_id, unsigned int vit);
	
    //��������
    bool addVitality(unsigned int hero_id, unsigned int vit);
    //�þ���
    void obtainExp(unsigned int hero_id, unsigned int exp);

    std::vector<float> ajustRatio(std::vector<int> ratio_vec);

    //����״̬Ǩ���Ƿ�Ϸ�
    bool canTaskStat1ChangeToStat2ByClient(unsigned int stat1, unsigned int stat2);

    //�޸�����״̬�����Ұ�������״̬�޸��������������������񡣰���������һ�����񣬼������
    void ChangeTaskStat(unsigned int hero_id, unsigned int task_id, unsigned int stat);


    //������
    void arenaTimer();
    ArenaRoom* getArenaRoom(unsigned int hero_level, unsigned char hard_level);
    void sendArenaUpdateRoomRsp(unsigned int to_hero_id, unsigned int new_enter_hero_id);
    void sendArenaEnterRsp(unsigned int to_client_id, unsigned int to_hero_id
        , std::string fd_account, std::vector<ArenaHero> partner);
    void sendArenaBattleAndGiftRsp(std::vector<ArenaHero> hero_vec);
    void create9ArenaGiftBox(unsigned int hero_id
        , int available_box_count, std::vector<GiftJifen>& gift_vec); //����9���������
    bool getArenaGiftBox(unsigned int hero_id, unsigned int gift_id, GiftJifen& gift_data);

    //�Ŷ�BOSS
    void teamBossTimer();
    TeamBossRoom* getTeamBossRoom(unsigned int hero_level, unsigned char hard_level);
    void sendTeamBossUpdateRoomRsp(unsigned int to_hero_id, unsigned int new_enter_hero_id);
    void sendTeamBossEnterRsp(unsigned int to_client_id, unsigned int to_hero_id
        , std::string fd_account, std::vector<TeamBossHero> partner);
    void sendTeamBossBattleAndGiftRsp(unsigned int room_level, unsigned char room_hard_level
        , std::vector<TeamBossHero> hero_vec);
    void create9TeamBossGiftBox(unsigned int hero_id
        , int available_box_count, std::vector<GiftData>& gift_vec); //����9���������
    bool getTeamBossGiftBox(unsigned int hero_id, unsigned int gift_id, GiftData& gift_data);

    void queryHeroBattleFormation(unsigned int main_hero_id, unsigned int hero_formation[]);

    //�������󷵻ػ����ˡ����п���������ʵ�����
    //hero_id_except��ʾ��Ҫ�ų���id������robot����������Щid
    void getRobot(unsigned int level_min, unsigned int level_max
        , std::vector<unsigned int> hero_id_except
        ,unsigned int robot_need_count, std::vector<unsigned int>& robot_vec);

    //�������󷵻�һ���Ŷ�BOSS
    Guaiwu getTeamBossMonster(unsigned int level, unsigned int hard_level);

    void PvP(BattleType battle_type, FirstBloodBoard fb, unsigned int hero_a, unsigned int hero_b, 
		FightScript& script_a, FightScript& script_b, unsigned int battle_kind=0);

    void getHeroSkill(unsigned int hero_id, SkillType& skill_type, unsigned int& skill_level);

    //�س�
    //���룬����0��ʾ�ɹ�
    int heroEnterCityGuard(unsigned int hero_id
        , unsigned int& last_total_turns, unsigned int& last_total_silver
        , unsigned int& sucess_turn, unsigned int& current_silver);
    void setNextTurnCityGuardMonsterTeamAndSilver(unsigned int hero_id);
    void clearCityGuardMonsterTeamAndSilver(unsigned int hero_id, bool isGiveUp); //
    void getCityGuardCurrentSilverAndNextSilver(unsigned int hero_id
        , unsigned int& current_silver, unsigned int& nextSilver);

    //Ӣ�۰�
    //��ȡ��top30�������һЩӢ��
    void getSomeotherHeroInTopList(unsigned int hero_order, std::vector<HeroAndOrder>& hero_vec);

    //�۷�Ծ�
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


    //ÿ���س�
    void loadCityGuardData();
    void cityGuardTimer();
    //�����سǽ��棬��ѯ��ǰ�سǻ��״̬��Ϣ
    void DealWithGetCityGuardCurrentStat(Packet* pack);
    //����ս��
    //���boss�Ѿ�����ɱ����������ս�����㡣
    void DealWithCityGuardBattle(Packet* pack);
    //�˳������س�
    void DealwithQuitCityGuard(Packet* pack);
    //��ȡ����ս��
    void DealWithGetCityGuardLastDayReport(Packet* pack);
    //��ȡ�Լ��Ľ�Ʒ
    void DealWithGetMyCityGuardReward(Packet* pack);
    //���ʹӶ�����ǰ�����ڵ��سǽ���֪ͨ
    void sendCityGuardProcessNotify(unsigned int hero_id, CityGuardProcessNotifyData notify_data);
    //��װ֪ͨ����
    void packCityGuardProcessNotifyData(unsigned int process_period, CityGuardProcessNotifyData& notify_data);
    void openCityGuard(cityGuardStatInfo stat);
    //boss����ɱʱ�����д���
    void processCityGuardBossBeKilled(unsigned int hero_kill_boss);
    //����ս�����佱
    void cityGuardGenReportAndCalcReward(unsigned int hero_kill_boss);
    //�����´��سǵ���Ϣ
    void setNextCityGuardInfo(unsigned int cityguard_time_period
        , unsigned int& next_boss_total_hp, unsigned int& next_open_time);
    void cityGuardBattleProc(Packet* pack, unsigned int main_hero_id, bool is_skip_cd);

    
    //
    bool getAndDelHeroCityGuardReward(unsigned int hero_id, std::vector<ItemInfo> &reward_vec);

    //
    void getCityGuardCurrentStatInfo(cityGuardStatInfo& stat_info);
    bool heroHasCityGuardReward(unsigned int hero_id, ItemInfo& reward);
    void getLastCityGuardReport(CityGuardReport& report);
    

    //��ȡ
    //����false��ʾ�Ϸ��Դ���cuiqu_suc��ʾ��ȡ�Ƿ�ɹ�
    bool checkAndCuiqu(unsigned int hero_id
        , unsigned long long zb_id, AttrType attr_type, unsigned int attr_value, bool is_fumo_attr
        , bool& cuiqu_suc, unsigned int& zb_type, unsigned int& jinghua_level);
    //����false��ʾ�Ϸ��Դ���cuiqu_suc��ʾ��ȡ�Ƿ�ɹ�
    bool checkAndFumo(unsigned int hero_id
        , unsigned long long zb_id, MofaJinghua jinghua
        , bool& fm_suc, ZBAttr& new_fm_attr);
    
    
    
    //������ȡ�ɹ���
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
    
    //�������������
    void mainTaskDaojuDiaoluo(unsigned int hero_id, unsigned int task_id, unsigned int grid_id
        , std::vector<ItemInfo>& item_vec);

    //��ѯ���ȼ���������ľ���
    unsigned int getProfessionLevelNeedExp(unsigned int level);
    //��ѯԪ����ȼ���������ľ���
    unsigned int getYuanshenLevelNeedExp(unsigned int level);
	
    //��ѯ���ȼ���������ľ���
    //type: HeroType
    unsigned int get_levelup_need_exp(unsigned int type, unsigned int level);
	
    //��ְҵ���ȼ�����Ӧ�Ļ������ԡ�
    void getProfessionLevelAttr(Profession prof, unsigned int level
        , unsigned int& liliang, unsigned int& minjie, unsigned int& zhili, unsigned int& tineng
        , unsigned int& xueliang, unsigned int& baoji, unsigned int& baoshang, unsigned int& jinengchufa);

    //��������ְҵ��װ�����͵��ʽ��к������
    ZhuangbeiType randZhuangbeiTypeBaseProfession(Profession prof, std::vector<ZhuangbeiType> zb_type_vec);

    //֧����������
    void subtaskPointFinished(unsigned int hero_id, unsigned int subtask_id);

    //������װ��
    void loadSuit();
    //���������
    void loadShenbing();

    //ÿ��ȫϵͳˢ�¶�ʱ����
    void SystemRefreshTimer();
    //����ճ�����ˢ��
    void refreshSubtask_Richang();

    //
    void recordClientIDAndFdAccount(unsigned int client_id, std::string fd_account);
    void kickoutFdAcount(std::string fd_account);

    //��ѯӢ���Ƿ��һ��ͨ������ؿ�����Ҫ����ָ��װ��
    //�������true�����ʾ����ָ�������
    bool checkDiaoluoZBSpecialTaskFirstPass(unsigned int hero_id, unsigned int task_id
     , unsigned int& diaoluo_name_id, unsigned int& diaoluo_group_id);
    //��ѯӢ���Ƿ��һ��ͨ������ؿ�����Ҫ����ָ������
    //�������true�����ʾ����ָ�������
    bool checkDiaoluoItemSpecialTaskFirstPass(unsigned int hero_id, unsigned int task_id
     , std::vector<ItemInfo>& item_vec);
    //��ѯӢ���Ƿ��һ��ͨ������ؿ�����Ҫ����ָ������
    //�������true�����ʾ����ָ�������
    bool checkDiaoluoSilverSpecialTaskFirstPass(unsigned int hero_id, unsigned int task_id
     , unsigned int& silver);

    //�Ƿ������ -1:���ǣ� >=0:���
    int ZBGroupIsShenbing(unsigned int zb_group_id);

    //�ⲿ����delete
    Hero* getBattleScriptHero(FightSUD user_data, unsigned int hero_id, unsigned int cdTime);

	//��ǩ�㷨
	int drawbyLot(int *count, const int len);


	/* �ؿ����� */
	//���¹ؿ�����
	void updateRateTaskStarStat(RateStarType type, unsigned int hero_id, 
		unsigned int task_id, unsigned int star);
	//���ؿ�����
	void rateTask(unsigned int hero_id, unsigned int task_id, 
		unsigned int team_pos[], HeroState herostat, 
		unsigned int guaiwu_attach_time, unsigned int attached_round);
	//���½�����
	void rateChapter(unsigned int hero_id, unsigned int chapter_id);

	//��ȡ��������  ����: false: ʧ��, true: �ɹ�
	bool get_ratetask_bonus(unsigned int hero_id, unsigned int type, 
		unsigned int task_id, unsigned int star);
	
	//��ȡӢ��vipĳ����Ȩ��Ϣ
	unsigned int getHeroVipInfo(unsigned int hero_id, VIP_FIELD_TYPE type);
	//����vipĳ����Ȩ��Ϣ
	void updateHeroVipInfo(unsigned int hero_id, VIP_FIELD_TYPE type, unsigned int value);
	//ˢ��Ӣ��vip��Ȩ��Ϣ
	void refreshHeroVipInfo(unsigned int hero_id);

	//��ѯ����װ��
	unsigned int poll_drop_equip(unsigned int hero_id, std::map<unsigned int, std::vector<unsigned int> > *zb_type_map);
	
	//��ѯ����  ����: false: ʧ��, true: �ɹ�
    bool query_bonus(unsigned int main_hero_id, unsigned int type,
    	std::vector<__BonusInfo> *bonus_vec);
	
	//���װ������ 
    void fill_zhuangbei_bonus_old(unsigned int main_hero_id, unsigned int bonus_type,
    	std::vector<__BonusInfo> &bonus_vec);
	
	
	//��ѯ�ճ�����  ����: false: ʧ��, true: �ɹ�
    bool query_daily_bonus(unsigned int main_hero_id, std::vector<__BonusInfo> &bonus_vec);
	
	//��ѯ�ȼ�����  ����: false: ʧ��, true: �ɹ�
    bool query_level_bonus(unsigned int main_hero_id, std::vector<__BonusInfo> &bonus_vec);
		
	//��ѯ�ճ��ܳ�����  ����: false: ʧ��, true: �ɹ�
    bool query_system_bonus(unsigned int main_hero_id, unsigned int type, 
    	std::vector<__BonusInfo> &bonus_vec);
	
	//���ճ������������� ����: false: ʧ��, true: �ɹ�
    bool sort_bonus_order(std::vector<int> &order_vec);
	
	//���װ������ 
    void fill_zhuangbei_bonus(unsigned int main_hero_id, std::vector<__BonusInfo> &bonus_vec);
	
	//����װ��
	bool present_zhuangbei(unsigned int hero_id, __BonusInfo bonus, unsigned int multiple,
		std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//��ȡ���� type:�������� ����: false: ʧ��, true: �ɹ�
    bool check_and_get_bonus(unsigned int hero_id, unsigned int type, unsigned int id, 
    	std::vector<__ZhuangbeiDetail> &zhuangbei_vec);
	
	//����ϵͳ��������֪ͨ
    void send_system_bonus_notify(unsigned int hero_id, unsigned int type, unsigned int id);

	//��������
	bool present_vitality(unsigned int hero_id);
	//����vip
	bool present_vip(unsigned int hero_id, unsigned int vip_level);

	//�������ݿ�����ģ���
	void load_database_template(void);

	//����ÿ�յ�½����ģ��
	void load_daily_bonus_temp(void);
	
	//���صȼ�����ģ��
	void load_level_bonus_temp(void);
	
	//����ϵͳ����ģ��
	void load_system_bonus_temp(void);

	//����ϵͳ����
	void load_system_notice(void);

	//�����ʼ�ģ��
	void load_email_temp(void);

	//�����̳���Ϣ
	void load_market_info(void);

	//���������������ñ�
	void load_levelup_exp(void);

	//��ѯ�ⲿ�ʼ���Ϣ 
    void query_extern_email_info(__EmailInfo &info);

	//��ѯ�ʼ�  ����: false: ʧ��, true: �ɹ�
    bool query_email(unsigned int main_hero_id, std::vector<__EmailInfo> &email_vec);

	//�Ķ��ʼ� type:�������� ����: false: ʧ��, true: �ɹ�
    bool read_email(unsigned int hero_id, unsigned int assign_id, 
    	std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//д�ʼ�  ����: false: ʧ��, true: �ɹ�
    bool write_email(unsigned int main_hero_id, unsigned int receiver_id, char* content);
	
	//ɾ�ʼ�  ����: false: ʧ��, true: �ɹ�
    bool delete_email(unsigned int main_hero_id, unsigned int assign_id);
	
	//�����ʼ� ����: >0:�ʼ�assign_id, 
	//content���� ���˸����ʼ�����������ȫ����NULL
	//id���� �ڸ����ʼ���idΪ0
    unsigned int insert_into_email(unsigned int hero_id, unsigned int addresser_id, 
    	unsigned int type, unsigned int id, char* content=NULL);
	
	//����ϵͳ��������֪ͨ
    void send_email_notify(unsigned int hero_id, std::vector<__EmailInfo> &email_vec);
	
	//�����ʼ������� ʵ�����Ǻϲ�������������(insert_into_email, send_email_notify)
    void insert_and_send_email(unsigned int hero_id, unsigned int addresser_id, 
    	unsigned int type, unsigned int id, char* content=NULL);
	
	//����ʼ�װ������ 
    void fill_zhuangbei_email(unsigned int main_hero_id, std::vector<__EmailInfo> &email_vec);
	
	//�����ʼ�װ��
	bool present_zhuangbei_email(unsigned int hero_id, __EmailInfo email, 
		std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//���ÿ�ղ������͵��ʼ�  ��ʱֻ��Ӣ�۰���������
    void check_email(unsigned int hero_id);
	

	//�����  ����: false: ʧ��, true: �ɹ�
    bool get_gift_bag(unsigned int main_hero_id, unsigned int id, 
    	std::vector<__GiftBagInfo> &gift_bag_vec);

	
	//���������  ����: false: ʧ��, true: �ɹ�
    bool fill_gift_bag(unsigned int main_hero_id, unsigned int id, 
    	std::vector<__GiftBagInfo> &gift_bag_vec);
	
	//�������װ��
	bool present_zhuangbei_gift_bag(unsigned int hero_id, __GiftBagInfo &gift_bag);

	//��ѯ˫������
	void query_both_two_order(
	    unsigned int hero_a, unsigned int hero_b, unsigned int &order_a, unsigned int &order_b);

	//������� ����: false: ʧ��, true: �ɹ�
	bool check_and_upgrade_shenbing(unsigned int hero_id
    , unsigned long long zb_id, unsigned long long cailiao_zb_id
    , std::vector<ItemInfo> &cailiao_vec);

	//����Ӣ�۵ȼ���װ��������ѡ��һ��group_id ����
    unsigned int selected_group_id(unsigned int hero_level, const std::vector<__zb_type_group_id> &zb_vec);

	//��ȡ���  ����: false: ʧ��, true: �ɹ�
    bool check_and_draw_shenbing(unsigned int main_hero_id, unsigned int profession_id
        , bool is_times_10, std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//��ѯװ��ͼ��  ����: false: ʧ��, true: �ɹ�
    bool query_zhuangbei_album(unsigned int main_hero_id, unsigned int profession_id
        , std::vector<__DuanzaoGroupZBInfo> &legend_vec
        , std::vector<__DuanzaoGroupZBInfo> &xiyou_vec);

	
	//��ѯװ��ģ����Ϣ  ����: false: ʧ��, true: �ɹ�
    bool query_duanzao_group_zb_info(unsigned int group_id, __DuanzaoGroupZBInfo &info);

    //����������������Ѷ�
    void adjustMainTaskMonsterDifficulty(unsigned int main_hero_id, unsigned int task_id, GuaiWuData& guaiwu_data);

    //��ȡ��Ӣ����������
    void get_team_formation(unsigned int main_hero_id, unsigned int *team_pos_attr, unsigned int *capacity_attr);

    //�������װ���Ƿ�Ϊͬһ���  ����: false: ����, true: ��
    bool check_same_parts(unsigned int group_id_a, unsigned int group_id_b);

	//��������  ����: false: ʧ��, true: �ɹ�
    bool check_and_unlock_assistant(unsigned int main_hero_id, unsigned int assistant, unsigned int &honor_balance);

	
	//��ȡ����  ����: ������
    int get_zb_hole(unsigned int zb_level, unsigned int zb_colour, unsigned int zb_type);
	//��ȡ���׸���
    void get_zb_hole_percent(unsigned int zb_level, unsigned int zb_colour, unsigned int zb_type,
    	unsigned int &per1, unsigned int &per2, unsigned int &per3);

	//��ѯ���ͻ��Ϣ  ����: false: ʧ��, true: �ɹ�
    bool query_gift_activity(unsigned int hero_id, std::vector<ActivityInfo> &info_vec);
	
	//��ȡ�����  ����: false: ʧ��, true: �ɹ�
    bool check_and_get_activity(unsigned int hero_id, unsigned int id);

	//���»�ɱС������
    void update_killed_monster_counts(unsigned int hero_id, unsigned int common_count, unsigned int elite_count);

	//��ȡ��ɱ���ｱ��  ����: false: ʧ��, true: �ɹ�
    bool check_and_get_killed_monster_bonus(unsigned int hero_id, unsigned int type);

	//ת��top arena info���ֽ���
	void hton_top_arena_info(TopArenaInfo& top_info_in, TopArenaInfo& top_info_out);

	//�۷�Ծ�������Ϣ  ����: false: ʧ��, true: �ɹ�
    bool check_and_guess_top_arena(unsigned int hero_id, unsigned int round, unsigned int group,
    	bool guess_winer, unsigned int wager, unsigned int &silver_balance);
	
	//��ѯѡ�����  ����: false: ʧ��, true: �ɹ�
    bool query_top_arena_value(unsigned int round, unsigned int group, TopArenaHeroValue &value);

	//��鲢����  ����: false: ʧ��, true: �ɹ�
    bool check_and_cast_silver(unsigned int hero_id, unsigned int &gold_balance, unsigned int &silver_balance);

	

	/* ����¼ */
	//��ս����¼  ����: false: ʧ��, true: �ɹ�
    bool check_and_challenge_legend_note(unsigned int main_hero_id, unsigned int group_id, 
    	char **fight_script, int &script_len);
	//��ս����¼ ��ȡ����������Ƭ
    bool get_challenge_legend_suipian(unsigned int main_hero_id, unsigned int group_id, unsigned int times,
    	std::vector<ShenbingSuipianInfo> &sp_vec, std::vector<ItemInfo> &item_vec);
	//����ְҵ��ȡ����¼�Ĺ���id
	unsigned int get_special_monster_id(unsigned int prof);
	

	/* Ӣ�۰� */
	//��ѯӢ�۰� ����: false: ʧ��, true: �ɹ�
	bool query_top_hero_list(unsigned int main_hero_id, unsigned int &hero_order, unsigned int &next_chlg_time,
		std::vector<HeroAndOrder> &order_vec, std::vector<TopHeroReport> &report_vec);
	//Ӣ�۰���ս ����: false: ʧ��, true: �ɹ�
	bool check_and_challenge_top_hero(unsigned int main_hero_id, unsigned int chlg_hero_id,
		unsigned int &order_after_fight, unsigned int &next_chlg_time, 
		char **fight_script, int &script_len);
	//�����ս����ȴʱ�� ����: false: ʧ��, true: �ɹ�
	bool clear_top_hero_cd_time(unsigned int hero_id, unsigned int &gold_balance, unsigned int &next_chlg_time);


	/* Ѫɫ���� ������ս */
	//������սtimer
    void self_challenge_timer();
	//Ѫɫ����������ս ����: false: ʧ��, true: �ɹ�
    bool check_and_self_challenge(unsigned int main_hero_id, unsigned int difficulty, 
    	unsigned int &energy, unsigned int &checkpoints, bool &is_have_box, unsigned int &silver,
    	 GiftBag &gift_bag, char **fight_script, int &script_len);
	//����������ս���佱��
    void update_self_challenge_box(unsigned int hero_id, unsigned int difficulty);
	//����������ս������
    void clear_self_challenge_order(void);
	//��������Ѷ�������
    void update_self_challenge_order(unsigned int hero_id, unsigned int difficulty, unsigned int checkpoints);
	//Ѫɫ���üӳ� ����: false: ʧ��, true: �ɹ�
    bool check_and_gain_self_challenge(unsigned int main_hero_id, unsigned int type, unsigned int degree, 
    	unsigned int &energy, unsigned int &hp_gain, unsigned int &attack_gain, unsigned int &defense_gain);
	//��ȡѪɫ���ñ��� ����: false: ʧ��, true: �ɹ�
    bool check_and_get_self_challenge_box(unsigned int hero_id, ItemInfo &item, std::vector<ItemInfo> &item_vec);
	//��ȡѪɫ���ù���
    void get_self_challenge_monster(unsigned int hero_id, unsigned int checkpoints, unsigned int monster_vec[]);


	/* �ػ�֮�� */
	//��ѯ�ػ�֮����Ϣ ����: false: ʧ��, true: �ɹ�
	bool query_protect_spirit(unsigned int hero_id, unsigned int chapter,
		HeroInfo &first_spirit, ProtectSpiritInfo &current_spirit, 
		bool &is_worship, bool &is_loser);
	//��ս�ػ�֮�� ����: false: ʧ��, true: �ɹ�
    bool challenge_protect_spirit(unsigned int main_hero_id, unsigned int chapter,
    	char **fight_script, int &script_len);
	//��ȡ�ػ�֮�齱�� ����: false: ʧ��, true: �ɹ�
    bool check_and_get_protect_spirit_bonus(unsigned int main_hero_id, unsigned int chapter,
    	std::vector<ItemInfo> &item_vec);
	//Ĥ���ػ�֮�� ����: false: ʧ��, true: �ɹ�
    bool check_and_worship_protect_spirit(unsigned int main_hero_id, unsigned int chapter,
    	GiftBag &gift_bag);
	//�ػ�֮��������
    int debuff_protect_spirit(unsigned int spirit_id);
	

	/* ̽�� */
	//��鲢̽�� ����: false: ʧ��, true: �ɹ�
    bool check_and_discover(unsigned int hero_id, bool akey_all, 
    	bool &is_destroyed, unsigned int &remainder_times, 
    	unsigned int &discovered_times, unsigned int &last_modify_timestamp,
    	std::vector<ItemInfo> &item_vec);
	//̽�����ӡ ����: false: ʧ��, true: �ɹ�
    bool check_and_unlock_discover_seal(unsigned int main_hero_id, unsigned int profession_id,
        unsigned int &destroy_seal, unsigned int &unlocked_seal, std::vector<__ZhuangbeiDetail> &zhuangbei_vec);

	//����Ӣ��ս���� ����� ps:Ԫ���ս���� �Ǹ����������ϵ� ֻ��������ս����
	unsigned int calculate_fight_capacity(unsigned int hero_id);

	//��ȡδװ�������ϵ�װ������
	unsigned int get_unequipped_counts(unsigned int hero_id);

	
	//���� ����: �����ʶ��
	int check_and_flop(unsigned int main_hero_id, unsigned int task, unsigned int grid,
		unsigned int &boss_stat, unsigned int &grid_stat, std::vector<ItemInfo> &item_vec);

	//��ȡ���̱��� ����: false: ʧ��, true: �ɹ�
	bool check_and_get_chess_box(unsigned int main_hero_id, unsigned int task, unsigned int grid,
		std::vector<ItemInfo> &item_vec);


	/* ���� */
	//��ѯ����װ���Ĳ���װ ����: false: ʧ��, true: �ɹ�
	bool check_and_query_assess_weapon(unsigned int group_id, 
		std::vector<__DuanzaoGroupZBInfo> &zb_vec);
	//����װ�� ����: false: ʧ��, true: �ɹ�
	bool check_and_assess_weapon(unsigned int hero_id, unsigned long long zb_id, 
		unsigned long long cailiao_zb_id, std::vector<ItemInfo> &cailiao_vec);


	//���������� 
	void arena_bubble_sort(std::vector<ArenaChlgerHeroInfo> &hero_vec);

	//����װ��������
	void load_assess_weapon(void);

	/* ϴ�� */
	//����ϴ�����Ա�
	void load_purify_attr_temp(void);
	//ϴ��װ�� ����: false: ʧ��, true: �ɹ�
	bool check_and_purify_weapon(unsigned int hero_id, unsigned long long id, 
		unsigned int hole, unsigned int &purify_hole, std::vector<ZBAttr> &attr_vec);
	
	//ȷ��ϴ�� ����: false: ʧ��, true: �ɹ�
	bool check_and_confirm_purify(unsigned int hero_id, unsigned long long id, bool is_confirm);

	
	//����վλ��� ����: false: ʧ��, true: �ɹ�
	bool check_and_adjust_formation(unsigned int main_hero_id, const unsigned int id_arr[]);


	/* ������ */
	//��ѯ��������Ϣ ����: false: ʧ��, true: �ɹ�
	bool query_heart_beat_info(unsigned int hero_id, unsigned int &vitality,
		std::vector<MarqueeMessage> &msg_vec);

	//���뵽�������Ϣ������ ����: false: ʧ��, true: �ɹ�
	//type ��Ϣ����; hero_id ��Ϣ����; other_info ��Ϣ����; count ��Ϣ��������;
	bool insert_marquee_message(unsigned int type, unsigned int hero_id, 
		unsigned int other_info, unsigned int count = 1);
	
	/* �̳� */
	//��ѯ�̳���Ϣ ����: false: ʧ��, true: �ɹ�
	bool query_market_info(unsigned int hero_id, std::vector<MarketItem> &market_vec);
	
	//�����̳���Ʒ ����: false: ʧ��, true: �ɹ�
    bool check_and_buy_item(unsigned int hero_id, unsigned int id, unsigned int buy_times);
	
	//��ѯͳ����Ϣ ����: false: ʧ��, true: �ɹ�
	bool query_statis_info(unsigned int hero_id, unsigned int type, unsigned int id, 
		unsigned int &count);
	
	//ͳ������ ����: false: ʧ��, true: �ɹ�
	bool statis_data(unsigned int hero_id, unsigned int type, unsigned int id, 
		unsigned int count);

public:
    //ʵ��ScriptDataProvider
    
    // ��ѯ���ݿ⣬������ݵ�load������ģ��ս������
	// ��������Ҫ���ĵģ�����
	//virtual void getHero(Hero& outHero,unsigned int _heroId,ScriptBuilderUserData data);

    // ��ѯ���ݿ⣬������ݵ�load������ģ��ս������
	// ��������Ҫ���ĵģ�����,����Ҫ��,
	// ����ǹ����new GuaiWu��Ȼ���ʼ�����ؼ���
	// �����Ӣ�ۣ���ô��new Hero Ȼ���ʼ�����ؼ��ɣ����û���ˣ���ô�ͷ���NULL
	virtual BaseHero* getBaseHero(unsigned int _heroId,ScriptBuilderUserData data);
    
	// ��ѯ���ݿ⣬������ݵ�load�����ڿͻ������ݵ�ˢ��
	// ���ظ��ͻ���
	// ��������Ҫ���ĵģ�����
	virtual void getHeroDetail(HeroDetail& outData,unsigned int _heroId,ScriptBuilderUserData data);

	// ս����ϣ����������ݳ־û�
	virtual unsigned int fightEndCallback(ScriptBuilderUserData data,bool win);

    virtual void getTeamBloodNum(short& currentNum,short& defaultNum,int& bloodAddPercent,ScriptBuilderUserData data);
    virtual void getHeroCDTime(short& cdStepNum,unsigned heroId,ScriptBuilderUserData data);

	// ��ȡ����װ��
	// �������������
	virtual vector<EquipData> getFightEndEquips(ScriptBuilderUserData data,bool win);
	// ��ȡ������Ʒ
	// �������������
	virtual vector<ItemInfo> getFightEndItems(ScriptBuilderUserData data,bool win);

    // ��ȡ�����Item��Ƭ
	virtual vector<ItemSuipianInfo> getFightEndItemSuiPians(ScriptBuilderUserData data,bool win);
	// ��ȡ����������Ƭ
	virtual vector<ShenbingSuipianInfo> getFightEndShenBingSuiPians(ScriptBuilderUserData data,bool win);

    // �Ŷ���ս��ϣ�������Ҫ���ػ�õ�ս��Ʒ
    // Ҳ��Ҫ�־û������ݿ�
    virtual void teamFightEnd(bool win,TeamEndParam parm,TeamEndOutParam& gettedItems);

    int getShangHaiType(Profession prof);

    //�Ƿ���boss�ؿ�
    bool isBOSS(unsigned int task_id, unsigned int grid_id);
    //��������̸�����
    void getTaskChessboardFormat(unsigned int task_id
        , unsigned int& grid_count_heng, unsigned int& grid_count_shu);
    unsigned int getGuaiwuAttackCountOfTask(unsigned int task_id, unsigned int grid_id);
    
    //�޳��������ְҵ��ר��װ��
    std::vector<unsigned int> filterZBGroupCannotUse(
        Profession main_hero_prof, std::vector<unsigned int> zb_group_id_vec);

    //ÿ�ն�ʱˢ�µ������������һ��ˢ��ʱ�������ʱ���ж��Ƿ���Ҫˢ��
    //daily_refresh_hour_pointΪϵͳˢ�µ�����
    bool isNeedSystemDailyRefresh(unsigned int last_refresh_time, unsigned int daily_refresh_hour_point);

    //������һ��ˢ��ʱ�������ʱ���ж��Ѿ���ȥ����
    //daily_refresh_hour_pointΪϵͳˢ�µ�����
    int pastSystemDays(unsigned int last_time, unsigned int daily_refresh_hour_point);

    
private:
	PacketQueue* m_import_q;
    PacketQueue* m_outport_q;

    //
    std::string m_db_ip;
	std::string m_db_username;
	std::string m_db_psw;

    //�߳�ר�����ݣ��洢ÿ���߳��Լ���mysql���Ӿ��
    pthread_key_t m_mysql_key;

    //������
    Arena m_arena;
    pthread_mutex_t m_arena_gift_pool_mutex;

    //�Ŷ�BOSS
    TeamBossHall m_teamBossHall;
    pthread_mutex_t m_teamboss_gift_pool_mutex;

    //�۷�Ծ�
    TopArena m_toparena;

    //�س�
    CityGuard m_cityguard;

	//��������
	__NoticeInfo m_notice;

    //����ģ���
    std::map<unsigned int, __BonusInfo> m_bonus_map[3];
	//�ڲ��ʼ�ģ���
	std::map<unsigned int, __EmailInfo> m_email_internal_map;

    //�������辭��
    std::map<unsigned int, unsigned int> m_exp_map[2];

    //��װ��
    std::vector<SuitNameInfo> m_suit_vec;

    //�����
    std::vector<__DuanzaoGroupZBInfo> m_shenbing_vec;

	//�������Ϣ����
	std::vector<MarqueeMessage> m_msg_vec;
	
	//�̳���Ϣ��
	std::map<unsigned int, MarketItem> m_market_map;

    //��¼�ѵ�¼�����Ϣ�������¼kickout��
    std::map<unsigned int, std::string> m_clientid2fdaccount;
};



#endif //_PROCSERVERTHREAD_H_


