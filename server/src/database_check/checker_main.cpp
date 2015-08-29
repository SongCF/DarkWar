
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <string.h>
#include "utility/logs.h"
#include "common/SDConfiguration.h"
#include "cmysql.h"
#include "protocol.h"


#include "os.h"
#include "PacketQueue.h"


#define JingyingGuanqiaCountMin 3


static std::string g_db_ip;
static std::string g_db_username;
static std::string g_db_psw;

static CMysql* g_mysql;

//create table client_id (client_id int unsigned, account varchar(32) charset utf8 default '' not null, psw varchar(16) not null);
struct struct_client_id
{
    unsigned int client_id;
    std::string account;
    std::string psw;
};
std::map<unsigned int, struct_client_id> g_client_id_map;
void check_client_id()
{
    
}
void load_client_id()
{
    char sql[1024];
    sprintf(sql, "select client_id, account, psw "
        " from hellgate.client_id"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_client_id id;
            
            id.client_id = atoi(g_mysql->GetField("client_id"));
            id.account = (g_mysql->GetField("account"));
            id.psw = (g_mysql->GetField("psw"));

            g_client_id_map.insert(make_pair(id.client_id, id));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


//create table profession_list (profession_id int unsigned, liliang int unsigned, 
//mingjie int unsigned, zhili int unsigned, tineng int unsigned, xueliang int unsigned, 
//gongji_min int unsigned, gongji_max int unsigned, baoji int unsigned, baoshang int unsigned, jinengchufa int unsigned);
struct struct_profession_list
{
    unsigned int profession_id;
    unsigned int liliang;
    unsigned int mingjie;
    unsigned int zhili;
    unsigned int tineng;
    unsigned int xueliang;
    unsigned int gongji_min;
    unsigned int gongji_max;
    unsigned int baoji;
    unsigned int baoshang;
    unsigned int jinengchufa;
};
std::map<unsigned int, struct_profession_list> g_profession_list_map;
void check_profession_list()
{
}
void load_profession_list()
{
    char sql[1024];
    sprintf(sql, "select profession_id, liliang, mingjie, zhili, tineng, xueliang "
        ", gongji_min, gongji_max, baoji, baoshang, jinengchufa "
        " from hellgate.profession_list"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_profession_list data;
            
            data.profession_id = atoi(g_mysql->GetField("profession_id"));
            data.liliang = atoi(g_mysql->GetField("liliang"));
            data.mingjie = atoi(g_mysql->GetField("mingjie"));
            data.zhili = atoi(g_mysql->GetField("zhili"));
            data.tineng = atoi(g_mysql->GetField("tineng"));
            data.xueliang = atoi(g_mysql->GetField("xueliang"));
            data.gongji_min = atoi(g_mysql->GetField("gongji_min"));
            data.gongji_max = atoi(g_mysql->GetField("gongji_max"));
            data.baoji = atoi(g_mysql->GetField("baoji"));
            data.baoshang = atoi(g_mysql->GetField("baoshang"));
            data.jinengchufa = atoi(g_mysql->GetField("jinengchufa"));

            g_profession_list_map.insert(make_pair(data.profession_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}

//create table profession_level_attr (profession_id int unsigned, level int unsigned, 
//exp_need int unsigned, liliang_add int unsigned, mingjie_add int unsigned, 
//zhili_add int unsigned, tineng_add int unsigned);
struct struct_profession_level_attr
{
    unsigned int profession_id;
    unsigned int level;
    unsigned int exp_need;
    unsigned int liliang_add;
    unsigned int mingjie_add;
    unsigned int zhili_add;
    unsigned int tineng_add;
};
std::vector<struct_profession_level_attr> g_profession_level_attr_vec;
void check_profession_level_attr()
{
}
void load_profession_level_attr()
{
    char sql[1024];
    sprintf(sql, "select profession_id, level, exp_need, liliang_add, mingjie_add, zhili_add, tineng_add "
        " from hellgate.profession_level_attr"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_profession_level_attr data;
            
            data.profession_id = atoi(g_mysql->GetField("profession_id"));
            data.level = atoi(g_mysql->GetField("level"));
            data.exp_need = atoi(g_mysql->GetField("exp_need"));
            data.liliang_add = atoi(g_mysql->GetField("liliang_add"));
            data.mingjie_add = atoi(g_mysql->GetField("mingjie_add"));
            data.zhili_add = atoi(g_mysql->GetField("zhili_add"));
            data.tineng_add = atoi(g_mysql->GetField("tineng_add"));

            g_profession_level_attr_vec.push_back(data);
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


//create table hero_list (client_id int unsigned, hero_id int unsigned);
struct struct_hero_list
{
    unsigned int client_id;
    unsigned int hero_id;
};
std::map<unsigned int, struct_hero_list> g_hero_list_map;
void check_hero_list();//在后面
void load_hero_list()
{
    char sql[1024];
    sprintf(sql, "select client_id, hero_id "
        " from hellgate.hero_list"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_hero_list data;
            
            data.client_id = atoi(g_mysql->GetField("client_id"));
            data.hero_id = atoi(g_mysql->GetField("hero_id"));

            g_hero_list_map.insert(make_pair(data.hero_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


//create table hero_infor (hero_id int unsigned, parent_hero_id int unsigned default 0, 
//profession_id int unsigned, level int unsigned default 1, hero_name varchar(18) charset utf8 default '', 
//vit_current int unsigned default 120, exp_current int unsigned default 0, position int unsigned default 0, vit_last_modify_time int unsigned default 0);
struct struct_hero_infor
{
    unsigned int hero_id;
    unsigned int parent_hero_id;
    unsigned int profession_id;
    unsigned int level;
    std::string hero_name;
    unsigned int vit_current;
    unsigned int exp_current;
    unsigned int position;
    unsigned int vit_last_modify_time;
};
std::map<unsigned int, struct_hero_infor> g_hero_infor_map;
void check_hero_list()
{
    LogMsg("%s, line %d, #################check_hero_list. g_hero_list_map.size=[%u]."
                    , __FILE__, __LINE__, g_hero_list_map.size());

    std::map<unsigned int, struct_hero_list>::iterator pos = g_hero_list_map.begin();
    for(; pos!=g_hero_list_map.end(); ++pos)
    {
        if(g_client_id_map.find(pos->second.client_id) == g_client_id_map.end())
        {
            //
            LogMsg("%s, line %d, hero_list. client_id[%u] not exist."
                    , __FILE__, __LINE__, pos->second.client_id);
        }

        if(g_hero_infor_map.find(pos->second.hero_id) == g_hero_infor_map.end())
        {
            //
            LogMsg("%s, line %d, hero_list. hero_id[%u] not exist."
                    , __FILE__, __LINE__, pos->second.hero_id);
        }
    }
}
void check_hero_infor()
{
    LogMsg("%s, line %d, #################check_hero_infor. g_hero_infor_map.size=[%u]."
                    , __FILE__, __LINE__, g_hero_infor_map.size());

    std::map<unsigned int, struct_hero_infor>::iterator pos = g_hero_infor_map.begin();
    for(; pos != g_hero_infor_map.end(); ++pos)
    {
        //判断parent
        if(pos->second.parent_hero_id > 0 
            && g_hero_list_map.find(pos->second.parent_hero_id) == g_hero_list_map.end())
        {
            //
            LogMsg("%s, line %d, hero_infor. parent_hero_id[%u] not exist."
                    , __FILE__, __LINE__, pos->second.parent_hero_id);
        }
        //判断profession_id
        if(g_profession_list_map.find(pos->second.profession_id) == g_profession_list_map.end())
        {
            //
            LogMsg("%s, line %d, hero_infor. profession_id[%u] not exist. hero_id[%u]"
                    , __FILE__, __LINE__, pos->second.profession_id, pos->second.hero_id);
        }
        //判断level
        if(pos->second.level > 60)
        {
            //
            LogMsg("%s, line %d, hero_infor. level[%u] invalide. hero_id[%u]"
                    , __FILE__, __LINE__, pos->second.level, pos->second.hero_id);
        }
        //判断vit_current
        if(pos->second.vit_current > Vitality_Total)
        {
            //
            LogMsg("%s, line %d, hero_infor. vit_current[%u] invalide. hero_id[%u]"
                    , __FILE__, __LINE__, pos->second.vit_current, pos->second.hero_id);
        }
        //判断position
        if(pos->second.position > 4)
        {
            //
            LogMsg("%s, line %d, hero_infor. position[%u] invalide.hero_id[%u]"
                    , __FILE__, __LINE__, pos->second.position, pos->second.hero_id);
        }
        //vit_last_modify_time
        if(pos->second.vit_last_modify_time > time(0))
        {
            //
            LogMsg("%s, line %d, hero_infor. vit_last_modify_time[%u] invalide.hero_id[%u]"
                    , __FILE__, __LINE__, pos->second.vit_last_modify_time, pos->second.hero_id);
        }

        //如果是主角
        if(g_hero_list_map.find(pos->second.hero_id) != g_hero_list_map.end())
        {
            if(pos->second.parent_hero_id != 0)
            {
                //
                LogMsg("%s, line %d, hero_infor. parent_hero_id[%u] invalide. hero_id[%u] is main_hero"
                    , __FILE__, __LINE__, pos->second.parent_hero_id, pos->second.hero_id);
            }
            if(pos->second.profession_id != Profession_Yemanren
                && pos->second.profession_id != Profession_Chike
                && pos->second.profession_id != Profession_Fashi)
            {
                //
                LogMsg("%s, line %d, hero_infor. profession_id[%u] invalide. hero_id[%u] is main_hero"
                    , __FILE__, __LINE__, pos->second.profession_id, pos->second.hero_id);
            }
            if(pos->second.hero_name.length() == 0)
            {
                //
                LogMsg("%s, line %d, hero_infor. hero_name null . hero_id[%u] is main_hero"
                    , __FILE__, __LINE__, pos->second.hero_id);
            }
            if(pos->second.position == 0)
            {
                //
                LogMsg("%s, line %d, hero_infor. position[%u] invalide. hero_id[%u] is main_hero"
                    , __FILE__, __LINE__, pos->second.position, pos->second.hero_id);
            }
        }
        else //助手
        {
            if(pos->second.parent_hero_id == 0)
            {
                //
                LogMsg("%s, line %d, hero_infor. parent_hero_id[%u] invalide. hero_id[%u] is not main_hero"
                    , __FILE__, __LINE__, pos->second.parent_hero_id, pos->second.hero_id);
            }
            if(pos->second.profession_id != Profession_LieShou
                && pos->second.profession_id != Profession_ShenJianShou
                && pos->second.profession_id != Profession_WuSeng
                && pos->second.profession_id != Profession_YouXia
                && pos->second.profession_id != Profession_Qishi
                && pos->second.profession_id != Profession_WuShi
                && pos->second.profession_id != Profession_MiShu)
            {
                //
                LogMsg("%s, line %d, hero_infor. profession_id[%u] invalide. hero_id[%u] is not main_hero"
                    , __FILE__, __LINE__, pos->second.profession_id, pos->second.hero_id);
            }
        }
    }
}
void load_hero_infor()
{
    char sql[1024];
    sprintf(sql, "select hero_id, parent_hero_id, profession_id, level, hero_name, vit_current, exp_current, position, vit_last_modify_time "
        " from hellgate.hero_infor"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_hero_infor data;
            
            data.hero_id = atoi(g_mysql->GetField("hero_id"));
            data.parent_hero_id = atoi(g_mysql->GetField("parent_hero_id"));
            data.profession_id = atoi(g_mysql->GetField("profession_id"));
            data.level = atoi(g_mysql->GetField("level"));
            data.hero_name = (g_mysql->GetField("hero_name"));
            data.vit_current = atoi(g_mysql->GetField("vit_current"));
            data.exp_current = atoi(g_mysql->GetField("exp_current"));
            data.position = atoi(g_mysql->GetField("position"));
            data.vit_last_modify_time = atoi(g_mysql->GetField("vit_last_modify_time"));

            g_hero_infor_map.insert(make_pair(data.hero_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}



struct struct_zhuangbei_name_list
{
    unsigned int zhuangbei_name_id;
    unsigned int group_weight;
    unsigned int group_id;
    unsigned int zhuangbei_type;
    std::string zhuangbei_name;
    unsigned int zhuangbei_colour;
    std::string zhuangbei_pic;
    std::string zhuangbei_desc;
    unsigned int zhuangbei_level;
    unsigned int gongji_min_min;
    unsigned int gongji_min_max;
    unsigned int gongji_max_min;
    unsigned int gongji_max_max;
    unsigned int wufang_min;
    unsigned int wufang_max;
    std::string attr_1;
    std::string attr_2;
    std::string attr_3;
    std::string attr_4;
    std::string attr_5;
    std::string attr_6;
    std::string attr_7;
    std::string attr_8;
    std::string attr_9;
    std::string attr_10;
};
std::map<unsigned int, struct_zhuangbei_name_list> g_zhuangbei_name_list_map;
void check_zhuangbei_name_list_copy()
{
    LogMsg("%s, line %d, #################check_zhuangbei_name_list_copy. g_zhuangbei_name_list_map.size=[%u]."
                    , __FILE__, __LINE__, g_zhuangbei_name_list_map.size());

    
}
void load_zhuangbei_name_list_copy()
{
    char sql[1024];
    sprintf(sql, "select zhuangbei_name_id, group_weight, group_id, zhuangbei_type, zhuangbei_name, zhuangbei_colour, zhuangbei_pic,  "
        " zhuangbei_desc, zhuangbei_level, gongji_min_min, gongji_min_max, gongji_max_min, gongji_max_max, wufang_min, wufang_max, "
        " attr_1, attr_2, attr_3, attr_4, attr_5, attr_6, attr_7, attr_8, attr_9, attr_10 "
        " from hellgate.zhuangbei_name_list_copy"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_zhuangbei_name_list data;
            
            data.zhuangbei_name_id = atoi(g_mysql->GetField("zhuangbei_name_id"));
            data.group_weight = atoi(g_mysql->GetField("group_weight"));
            data.group_id = atoi(g_mysql->GetField("group_id"));
            data.zhuangbei_type = atoi(g_mysql->GetField("zhuangbei_type"));
            data.zhuangbei_name = atoi(g_mysql->GetField("zhuangbei_name"));
            data.zhuangbei_colour = atoi(g_mysql->GetField("zhuangbei_colour"));
            data.zhuangbei_pic = atoi(g_mysql->GetField("zhuangbei_pic"));
            data.zhuangbei_desc = atoi(g_mysql->GetField("zhuangbei_desc"));
            data.zhuangbei_level = atoi(g_mysql->GetField("zhuangbei_level"));
            data.gongji_min_min = atoi(g_mysql->GetField("gongji_min_min"));
            data.gongji_min_max = atoi(g_mysql->GetField("gongji_min_max"));
            data.gongji_max_min = atoi(g_mysql->GetField("gongji_max_min"));
            data.gongji_max_max = atoi(g_mysql->GetField("gongji_max_max"));
            data.wufang_min = atoi(g_mysql->GetField("wufang_min"));
            data.wufang_max = atoi(g_mysql->GetField("wufang_max"));
            data.attr_1 = atoi(g_mysql->GetField("attr_1"));
            data.attr_2 = atoi(g_mysql->GetField("attr_2"));
            data.attr_3 = atoi(g_mysql->GetField("attr_3"));
            data.attr_4 = atoi(g_mysql->GetField("attr_4"));
            data.attr_5 = atoi(g_mysql->GetField("attr_5"));
            data.attr_6 = atoi(g_mysql->GetField("attr_6"));
            data.attr_7 = atoi(g_mysql->GetField("attr_7"));
            data.attr_8 = atoi(g_mysql->GetField("attr_8"));
            data.attr_9 = atoi(g_mysql->GetField("attr_9"));
            data.attr_10 = atoi(g_mysql->GetField("attr_10"));

            g_zhuangbei_name_list_map.insert(make_pair(data.zhuangbei_name_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


struct struct_zhuangbei_list
{
    unsigned long long zhuangbei_id;
    unsigned int zhuangbei_name_id;
    unsigned int owner_hero_id;
    unsigned int zhuangbei_hero_id;
    unsigned int hole;
    unsigned int diamond_type_1;
    unsigned int diamond_type_2;
    unsigned int diamond_type_3;
    unsigned int qianghua_time;
    unsigned int gongji_min;
    unsigned int gongji_max;
    unsigned int hujia;
    unsigned int kangxing;
    unsigned int mingjie;
    unsigned int zhili;
    unsigned int liliang;
    unsigned int tineng;
    unsigned int baoji;
    unsigned int baoshang;
    unsigned int hp_ratio;
    unsigned int gongji_min_add;
    unsigned int gongji_max_add;
    unsigned int gedanglv;
    unsigned int gedangzhi;
};
std::map<unsigned long long, struct_zhuangbei_list> g_zhuangbei_list_map;
void check_zhuangbei_list()
{
    LogMsg("%s, line %d, #################check_zhuangbei_list. g_zhuangbei_list_map.size=[%u]."
                    , __FILE__, __LINE__, g_zhuangbei_list_map.size());

    std::map<unsigned long long, struct_zhuangbei_list>::iterator pos = g_zhuangbei_list_map.begin();
    for(; pos!=g_zhuangbei_list_map.end(); ++pos)
    {
        unsigned int zb_type = g_zhuangbei_name_list_map.find(pos->second.zhuangbei_name_id)->second.zhuangbei_type;
        
        //zhuangbei_name_id
        if(g_zhuangbei_name_list_map.find(pos->second.zhuangbei_name_id) == g_zhuangbei_name_list_map.end())
        {
            //
            LogMsg("%s, line %d, zhuangbei_list. zhuangbei_name_id[%u] invalide. zhuangbei_id[%llu]"
                , __FILE__, __LINE__, pos->second.zhuangbei_name_id, pos->second.zhuangbei_id);
        }
        //owner_hero_id
        if(g_hero_list_map.find(pos->second.owner_hero_id) == g_hero_list_map.end())
        {
            //
            LogMsg("%s, line %d, zhuangbei_list. owner_hero_id[%u] invalide. zhuangbei_id[%llu]"
                , __FILE__, __LINE__, pos->second.owner_hero_id, pos->second.zhuangbei_id);
        }
        //zhuangbei_hero_id
        if(pos->second.zhuangbei_hero_id != 0
            && g_hero_infor_map.find(pos->second.zhuangbei_hero_id) == g_hero_infor_map.end())
        {
            //
            LogMsg("%s, line %d, zhuangbei_list. zhuangbei_hero_id[%u] invalide. zhuangbei_id[%llu]"
                , __FILE__, __LINE__, pos->second.zhuangbei_hero_id, pos->second.zhuangbei_id);
        }
        //hole
        if(pos->second.hole == 2)
        {
            if(zb_type != ZhuangbeiType_KuijiaZhong
                && zb_type != ZhuangbeiType_KuijiaQin)
            {
                //
                LogMsg("%s, line %d, zhuangbei_list. hole[%u] invalide. zhuangbei_id[%llu] type[%u]"
                    , __FILE__, __LINE__, pos->second.hole, pos->second.zhuangbei_id, zb_type);
            }
        }
        if(pos->second.hole == 3)
        {
            if(zb_type != ZhuangbeiType_KuijiaZhong
                && zb_type != ZhuangbeiType_KuijiaQin)
            {
                //
                LogMsg("%s, line %d, zhuangbei_list. hole[%u] invalide. zhuangbei_id[%llu] type[%u]"
                    , __FILE__, __LINE__, pos->second.hole, pos->second.zhuangbei_id, zb_type);
            }
        }
        if(pos->second.hole > 3)
        {
            //
            LogMsg("%s, line %d, zhuangbei_list. hole[%u] invalide. zhuangbei_id[%llu]"
                , __FILE__, __LINE__, pos->second.hole, pos->second.zhuangbei_id);
        }
        //diamond
        int diamond_count = 0;
        if(pos->second.diamond_type_1 > 0)
        {
            diamond_count++;

            if(pos->second.diamond_type_1 <= ItemsType_Baoshi_min 
                || pos->second.diamond_type_1 >= ItemsType_Baoshi_max)
            {
                //
                LogMsg("%s, line %d, zhuangbei_list. diamond_type_1[%u] invalide. zhuangbei_id[%llu]"
                    , __FILE__, __LINE__, pos->second.diamond_type_1, pos->second.zhuangbei_id);
            }
        }
        if(pos->second.diamond_type_2 > 0)
        {
            diamond_count++;

            if(pos->second.diamond_type_2 <= ItemsType_Baoshi_min 
                || pos->second.diamond_type_2 >= ItemsType_Baoshi_max)
            {
                //
                LogMsg("%s, line %d, zhuangbei_list. diamond_type_2[%u] invalide. zhuangbei_id[%llu]"
                    , __FILE__, __LINE__, pos->second.diamond_type_2, pos->second.zhuangbei_id);
            }
        }
        if(pos->second.diamond_type_3 > 0)
        {
            diamond_count++;

            if(pos->second.diamond_type_3 <= ItemsType_Baoshi_min 
                || pos->second.diamond_type_3 >= ItemsType_Baoshi_max)
            {
                //
                LogMsg("%s, line %d, zhuangbei_list. diamond_type_3[%u] invalide. zhuangbei_id[%llu]"
                    , __FILE__, __LINE__, pos->second.diamond_type_3, pos->second.zhuangbei_id);
            }
        }
        if(diamond_count > pos->second.hole)
        {
            //
                LogMsg("%s, line %d, zhuangbei_list. diamond_count[%u]>hole[%u] invalide. zhuangbei_id[%llu]"
                    , __FILE__, __LINE__, diamond_count, pos->second.hole, pos->second.zhuangbei_id);
        }

        //qianghua_time
        if(pos->second.qianghua_time > 20)
        {
            //
            LogMsg("%s, line %d, zhuangbei_list. qianghua_time[%u] invalide. zhuangbei_id[%llu]"
                , __FILE__, __LINE__, pos->second.qianghua_time, pos->second.zhuangbei_id);
        }

        //gongji_min, gongji_max
        if(!_isWuqi((ZhuangbeiType)zb_type)
            && (pos->second.gongji_min > 0 || pos->second.gongji_max > 0))
        {
            //
            LogMsg("%s, line %d, zhuangbei_list. gongji_min[%u] gongji_max[%u] invalide. zhuangbei_id[%llu] type[%u] not wuqi"
                , __FILE__, __LINE__, pos->second.gongji_min, pos->second.gongji_max, pos->second.zhuangbei_id, zb_type);
        }
        //hujia
        if(!_isFangju((ZhuangbeiType)zb_type)
            && (pos->second.hujia > 0))
        {
            //
            LogMsg("%s, line %d, zhuangbei_list. hujia[%u] invalide. zhuangbei_id[%llu] type[%u] not fangju"
                , __FILE__, __LINE__, pos->second.hujia, pos->second.zhuangbei_id, zb_type);
        }
        //gedanglv, gedangzhi
        if(zb_type != ZhuangbeiType_Dunpai
            && (pos->second.gedanglv > 0 || pos->second.gedangzhi > 0))
        {
            //
            LogMsg("%s, line %d, zhuangbei_list. gedanglv[%u] gedangzhi[%u] invalide. zhuangbei_id[%llu] type[%u] not dunpai"
                , __FILE__, __LINE__, pos->second.gedanglv, pos->second.gedangzhi, pos->second.zhuangbei_id, zb_type);
        }
        //检查数值设定范围
        {
            //.........................;
        }
    }
}
void load_zhuangbei_list()
{
    char sql[1024];
    sprintf(sql, "select zhuangbei_id, zhuangbei_name_id, owner_hero_id, zhuangbei_hero_id,  "
        " hole, diamond_type_1, diamond_type_2, diamond_type_3, "
        " qianghua_time, gongji_min, gongji_max, hujia, kangxing, mingjie, zhili, "
        " liliang, tineng, baoji, baoshang, hp_ratio, gongji_min_add, gongji_max_add, gedanglv, gedangzhi "
        " from hellgate.zhuangbei_list"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_zhuangbei_list data;
            
            data.zhuangbei_id = atoll(g_mysql->GetField("zhuangbei_id"));
            data.zhuangbei_name_id = atoi(g_mysql->GetField("zhuangbei_name_id"));
            data.owner_hero_id = atoi(g_mysql->GetField("owner_hero_id"));
            data.zhuangbei_hero_id = atoi(g_mysql->GetField("zhuangbei_hero_id"));
            data.hole = atoi(g_mysql->GetField("hole"));
            data.diamond_type_1 = atoi(g_mysql->GetField("diamond_type_1"));
            data.diamond_type_2 = atoi(g_mysql->GetField("diamond_type_2"));
            data.diamond_type_3 = atoi(g_mysql->GetField("diamond_type_3"));
            data.qianghua_time = atoi(g_mysql->GetField("qianghua_time"));
            data.gongji_min = atoi(g_mysql->GetField("gongji_min"));
            data.gongji_max = atoi(g_mysql->GetField("gongji_max"));
            data.hujia = atoi(g_mysql->GetField("hujia"));
            data.kangxing = atoi(g_mysql->GetField("kangxing"));
            data.mingjie = atoi(g_mysql->GetField("mingjie"));
            data.zhili = atoi(g_mysql->GetField("zhili"));
            data.liliang = atoi(g_mysql->GetField("liliang"));
            data.tineng = atoi(g_mysql->GetField("tineng"));
            data.baoji = atoi(g_mysql->GetField("baoji"));
            data.baoshang = atoi(g_mysql->GetField("baoshang"));
            data.baoshang = atoi(g_mysql->GetField("baoshang"));
            data.gongji_min_add = atoi(g_mysql->GetField("gongji_min_add"));
            data.gongji_max_add = atoi(g_mysql->GetField("gongji_max_add"));
            data.gedanglv = atoi(g_mysql->GetField("gedanglv"));
            data.gedangzhi = atoi(g_mysql->GetField("gedangzhi"));

            g_zhuangbei_list_map.insert(make_pair(data.zhuangbei_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


//create table items_in_team (hero_id int unsigned, item_type int unsigned, item_count int unsigned default 0);
struct struct_items_in_team
{
    unsigned int hero_id;
    unsigned int item_type;
    unsigned int item_count;
};
std::vector<struct_items_in_team> g_items_in_team_vec;
void check_items_in_team()
{
}
void load_items_in_team()
{
    char sql[1024];
    sprintf(sql, "select hero_id, item_type, item_count  "
        " from hellgate.items_in_team"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_items_in_team data;
            
            data.hero_id = atoi(g_mysql->GetField("hero_id"));
            data.item_type = atoi(g_mysql->GetField("item_type"));
            data.item_count = atoi(g_mysql->GetField("item_count"));

            g_items_in_team_vec.push_back(data);
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


struct struct_zb_qianghua_chenggonglv
{
    unsigned int ratio_1;
    unsigned int ratio_2;
    unsigned int ratio_3;
    unsigned int ratio_4;
    unsigned int ratio_5;
    unsigned int ratio_6;
    unsigned int ratio_7;
    unsigned int ratio_8;
    unsigned int ratio_9;
    unsigned int ratio_10;
};
struct_zb_qianghua_chenggonglv g_zb_qianghua_chenggonglv;
void check_zb_qianghua_chenggonglv()
{
}
void load_zb_qianghua_chenggonglv()
{
    char sql[1024];
    sprintf(sql, "select ratio_1, ratio_2, ratio_3, ratio_4, ratio_5, ratio_6, ratio_7, ratio_8, ratio_9, ratio_10  "
        " from hellgate.zb_qianghua_chenggonglv"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            g_zb_qianghua_chenggonglv.ratio_1 = atoi(g_mysql->GetField("ratio_1"));
            g_zb_qianghua_chenggonglv.ratio_2 = atoi(g_mysql->GetField("ratio_2"));
            g_zb_qianghua_chenggonglv.ratio_3 = atoi(g_mysql->GetField("ratio_3"));
            g_zb_qianghua_chenggonglv.ratio_4 = atoi(g_mysql->GetField("ratio_4"));
            g_zb_qianghua_chenggonglv.ratio_5 = atoi(g_mysql->GetField("ratio_5"));
            g_zb_qianghua_chenggonglv.ratio_6 = atoi(g_mysql->GetField("ratio_6"));
            g_zb_qianghua_chenggonglv.ratio_7 = atoi(g_mysql->GetField("ratio_7"));
            g_zb_qianghua_chenggonglv.ratio_8 = atoi(g_mysql->GetField("ratio_8"));
            g_zb_qianghua_chenggonglv.ratio_9 = atoi(g_mysql->GetField("ratio_9"));
            g_zb_qianghua_chenggonglv.ratio_10 = atoi(g_mysql->GetField("ratio_10"));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


//create table hero_maintask_progress (hero_id int unsigned, maintask_last_actived int unsigned default 1, status int unsigned default 1);
struct struct_hero_maintask_progress
{
    unsigned int hero_id;
    unsigned int maintask_last_actived;
    unsigned int status;
};
std::map<unsigned int, struct_hero_maintask_progress> g_hero_maintask_progress_map;
void check_hero_maintask_progress()
{
}
void load_hero_maintask_progress()
{
    char sql[1024];
    sprintf(sql, "select hero_id, maintask_last_actived, status  "
        " from hellgate.hero_maintask_progress"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_hero_maintask_progress data;
            
            data.hero_id = atoi(g_mysql->GetField("hero_id"));
            data.maintask_last_actived = atoi(g_mysql->GetField("maintask_last_actived"));
            data.status = atoi(g_mysql->GetField("status"));
            

            g_hero_maintask_progress_map.insert(make_pair(data.hero_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


//create table subtask_daily_zhuangbei (active_day_bitmap tinyint unsigned, play_times_limit_oneday int unsigned, level_limit int unsigned);
struct struct_subtask_daily_zhuangbei
{
    unsigned char active_day_bitmap;
    unsigned int play_times_limit_oneday;
    unsigned int level_limit;
};
struct_subtask_daily_zhuangbei g_subtask_daily_zhuangbei;
void check_subtask_daily_zhuangbei()
{
}
void load_subtask_daily_zhuangbei()
{
    char sql[1024];
    sprintf(sql, "select active_day_bitmap, play_times_limit_oneday, level_limit  "
        " from hellgate.subtask_daily_zhuangbei"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            g_subtask_daily_zhuangbei.active_day_bitmap = atoi(g_mysql->GetField("active_day_bitmap"));
            g_subtask_daily_zhuangbei.play_times_limit_oneday = atoi(g_mysql->GetField("play_times_limit_oneday"));
            g_subtask_daily_zhuangbei.level_limit = atoi(g_mysql->GetField("level_limit"));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


//create table hero_progress_subtask_daily_zhuangbei (hero_id int unsigned, status int unsigned default 0, play_times_today int unsigned default 0);
struct struct_hero_progress_subtask_daily_zhuangbei
{
    unsigned int hero_id;
    unsigned int status;
    unsigned int play_times_today;
};
std::map<unsigned int, struct_hero_progress_subtask_daily_zhuangbei> g_hero_progress_subtask_daily_zhuangbei_map;
void check_hero_progress_subtask_daily_zhuangbei()
{
}
void load_hero_progress_subtask_daily_zhuangbei()
{
    char sql[1024];
    sprintf(sql, "select hero_id, play_times_today, status  "
        " from hellgate.hero_progress_subtask_daily_zhuangbei"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_hero_progress_subtask_daily_zhuangbei data;
            
            data.hero_id = atoi(g_mysql->GetField("hero_id"));
            data.play_times_today = atoi(g_mysql->GetField("play_times_today"));
            data.status = atoi(g_mysql->GetField("status"));
            
            g_hero_progress_subtask_daily_zhuangbei_map.insert(make_pair(data.hero_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


//create table subtask_daily_baoshi (active_day_bitmap tinyint unsigned, play_times_limit_oneday int unsigned, level_limit int unsigned);
struct struct_subtask_daily_baoshi
{
    unsigned char active_day_bitmap;
    unsigned int play_times_limit_oneday;
    unsigned int level_limit;
};
struct_subtask_daily_baoshi g_subtask_daily_baoshi;
void check_subtask_daily_baoshi()
{
}
void load_subtask_daily_baoshi()
{
    char sql[1024];
    sprintf(sql, "select active_day_bitmap, play_times_limit_oneday, level_limit  "
        " from hellgate.subtask_daily_baoshi"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            g_subtask_daily_baoshi.active_day_bitmap = atoi(g_mysql->GetField("active_day_bitmap"));
            g_subtask_daily_baoshi.play_times_limit_oneday = atoi(g_mysql->GetField("play_times_limit_oneday"));
            g_subtask_daily_baoshi.level_limit = atoi(g_mysql->GetField("level_limit"));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


//create table hero_progress_subtask_daily_baoshi (hero_id int unsigned, status int unsigned default 0, play_times_today int unsigned default 0);
struct struct_hero_progress_subtask_daily_baoshi
{
    unsigned int hero_id;
    unsigned int status;
    unsigned int play_times_today;
};
std::map<unsigned int, struct_hero_progress_subtask_daily_baoshi> g_hero_progress_subtask_daily_baoshi_map;
void check_hero_progress_subtask_daily_baoshi()
{
}
void load_hero_progress_subtask_daily_baoshi()
{
    char sql[1024];
    sprintf(sql, "select hero_id, play_times_today, status  "
        " from hellgate.hero_progress_subtask_daily_baoshi"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_hero_progress_subtask_daily_baoshi data;
            
            data.hero_id = atoi(g_mysql->GetField("hero_id"));
            data.play_times_today = atoi(g_mysql->GetField("play_times_today"));
            data.status = atoi(g_mysql->GetField("status"));
            
            g_hero_progress_subtask_daily_baoshi_map.insert(make_pair(data.hero_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


struct struct_task_chessboard
{
    unsigned int hero_id;
    unsigned int task_id;
    
	unsigned int grid_1_guanqia_id;
	unsigned int grid_2_guanqia_id;
	unsigned int grid_3_guanqia_id;
    unsigned int grid_4_guanqia_id;
    unsigned int grid_5_guanqia_id;
    unsigned int grid_6_guanqia_id;
    unsigned int grid_7_guanqia_id;
    unsigned int grid_8_guanqia_id;
    unsigned int grid_9_guanqia_id;
    bool grid_1_stat;
    bool grid_2_stat;
    bool grid_3_stat;
    bool grid_4_stat;
    bool grid_5_stat;
    bool grid_6_stat;
    bool grid_7_stat;
    bool grid_8_stat;
    bool grid_9_stat;
};
std::vector<struct_task_chessboard> g_task_chessboard_vec;
void check_task_chessboard()
{
    
}
void load_task_chessboard()
{
    char sql[1024];
    sprintf(sql, "select hero_id, task_id, grid_1_guanqia_id, grid_1_stat, grid_2_guanqia_id, grid_2_stat, "
        " grid_3_guanqia_id, grid_3_stat, grid_4_guanqia_id, grid_4_stat, grid_5_guanqia_id, grid_5_stat, "
        " grid_6_guanqia_id, grid_6_stat, grid_7_guanqia_id, grid_7_stat, grid_8_guanqia_id, grid_8_stat, grid_9_guanqia_id, grid_9_stat "
        " from hellgate.task_chessboard"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_task_chessboard data;
            
            data.hero_id = atoi(g_mysql->GetField("hero_id"));
            data.task_id = atoi(g_mysql->GetField("task_id"));
            data.grid_1_guanqia_id = atoi(g_mysql->GetField("grid_1_guanqia_id"));
            data.grid_1_stat = atoi(g_mysql->GetField("grid_1_stat"));
            data.grid_2_guanqia_id = atoi(g_mysql->GetField("grid_2_guanqia_id"));
            data.grid_2_stat = atoi(g_mysql->GetField("grid_2_stat"));
            data.grid_3_guanqia_id = atoi(g_mysql->GetField("grid_3_guanqia_id"));
            data.grid_3_stat = atoi(g_mysql->GetField("grid_3_stat"));
            data.grid_4_guanqia_id = atoi(g_mysql->GetField("grid_4_guanqia_id"));
            data.grid_4_stat = atoi(g_mysql->GetField("grid_4_stat"));
            data.grid_5_guanqia_id = atoi(g_mysql->GetField("grid_5_guanqia_id"));
            data.grid_5_stat = atoi(g_mysql->GetField("grid_5_stat"));
            data.grid_6_guanqia_id = atoi(g_mysql->GetField("grid_6_guanqia_id"));
            data.grid_6_stat = atoi(g_mysql->GetField("grid_6_stat"));
            data.grid_7_guanqia_id = atoi(g_mysql->GetField("grid_7_guanqia_id"));
            data.grid_7_stat = atoi(g_mysql->GetField("grid_7_stat"));
            data.grid_8_guanqia_id = atoi(g_mysql->GetField("grid_8_guanqia_id"));
            data.grid_8_stat = atoi(g_mysql->GetField("grid_8_stat"));
            data.grid_9_guanqia_id = atoi(g_mysql->GetField("grid_9_guanqia_id"));
            data.grid_9_stat = atoi(g_mysql->GetField("grid_9_stat"));
            
            g_task_chessboard_vec.push_back(data);
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}



struct struct_guaiwu_profession_type_list
{
    unsigned int profession_type_id;
    unsigned int gongji;
    unsigned int xueliang;
    unsigned int baoji;
    unsigned int baoshang;
    unsigned int jinengchufa;
    unsigned int hujia_jianmian;
    unsigned int kangxing_jianmian;
};
std::map<unsigned int, struct_guaiwu_profession_type_list> g_guaiwu_profession_type_list_map;
void check_guaiwu_profession_type_list()
{
}
void load_guaiwu_profession_type_list()
{
    char sql[1024];
    sprintf(sql, "select profession_type_id, gongji, xueliang, baoji,  "
        " baoshang, jinengchufa, hujia_jianmian, kangxing_jianmian "
        " from hellgate.guaiwu_profession_type_list"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_guaiwu_profession_type_list data;
            
            data.profession_type_id = atoi(g_mysql->GetField("profession_type_id"));
            data.gongji = atoi(g_mysql->GetField("gongji"));
            data.xueliang = atoi(g_mysql->GetField("xueliang"));
            data.baoji = atoi(g_mysql->GetField("baoji"));
            data.baoshang = atoi(g_mysql->GetField("baoshang"));
            data.jinengchufa = atoi(g_mysql->GetField("jinengchufa"));
            data.hujia_jianmian = atoi(g_mysql->GetField("hujia_jianmian"));
            data.kangxing_jianmian = atoi(g_mysql->GetField("kangxing_jianmian"));
            
            
            g_guaiwu_profession_type_list_map.insert(make_pair(data.profession_type_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


struct struct_guaiwu_profession_list
{
    unsigned int profession_id;
    unsigned int profession_type_id;
    std::string profession_name;
    std::string pic;
};
std::map<unsigned int, struct_guaiwu_profession_list> g_guaiwu_profession_list_map;
void check_guaiwu_profession_list()
{
    LogMsg("%s, line %d, #################check_guaiwu_profession_list. g_guaiwu_profession_list_map.size=[%u]."
                    , __FILE__, __LINE__, g_guaiwu_profession_list_map.size());

    std::map<unsigned int, struct_guaiwu_profession_list>::iterator pos = g_guaiwu_profession_list_map.begin();
    for(; pos!=g_guaiwu_profession_list_map.end(); ++pos)
    {
        //profession_type_id
        if(g_guaiwu_profession_type_list_map.find(pos->second.profession_type_id) == g_guaiwu_profession_type_list_map.end())
        {
            LogMsg("%s, line %d, guaiwu_profession_list. profession_type_id[%u] invalide. profession_id[%u]"
                    , __FILE__, __LINE__, pos->second.profession_type_id, pos->second.profession_id);
        }
        //profession_name
        if(pos->second.profession_name.length() == 0)
        {
            LogMsg("%s, line %d, guaiwu_profession_list. profession_name null. profession_id[%u]"
                    , __FILE__, __LINE__, pos->second.profession_id);
        }
        //pic
        //....................;
    }
}
void load_guaiwu_profession_list()
{
    char sql[1024];
    sprintf(sql, "select profession_id, profession_type_id, profession_name, pic  "
        " from hellgate.guaiwu_profession_list"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_guaiwu_profession_list data;
            
            data.profession_id = atoi(g_mysql->GetField("profession_id"));
            data.profession_type_id = atoi(g_mysql->GetField("profession_type_id"));
            data.profession_name = (g_mysql->GetField("profession_name"));
            data.pic = (g_mysql->GetField("pic"));
            
            
            g_guaiwu_profession_list_map.insert(make_pair(data.profession_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}



struct struct_guaiwu_list_new
{
    unsigned int guaiwu_id;
    unsigned int guaiwu_level;
    unsigned int guaiwu_zhiye;
    bool is_jingying;
    unsigned int hp_add;
    unsigned int gongji_add;
    unsigned int jineng_chufa_lv_add;
    unsigned int baojishanghai_add;
    unsigned int baoji_lv_add;
    unsigned int hujia_jianmian_add;
    unsigned int kangxing_jianmian_add;
};
std::map<unsigned int, struct_guaiwu_list_new> g_guaiwu_list_new_map;
void check_guaiwu_list_new()
{
    LogMsg("%s, line %d, #################check_guaiwu_list_new. g_guaiwu_list_new_map.size=[%u]."
                    , __FILE__, __LINE__, g_guaiwu_list_new_map.size());

    std::map<unsigned int, struct_guaiwu_list_new>::iterator pos = g_guaiwu_list_new_map.begin();
    for(; pos!=g_guaiwu_list_new_map.end(); ++pos)
    {
        //guaiwu_zhiye
        if(g_guaiwu_profession_list_map.find(pos->second.guaiwu_zhiye) == g_guaiwu_profession_list_map.end())
        {
            LogMsg("%s, line %d, guaiwu_list_new. guaiwu_zhiye[%u] invalide. guaiwu_id[%u]"
                    , __FILE__, __LINE__, pos->second.guaiwu_zhiye, pos->second.guaiwu_id);
        }
    }
}
void load_guaiwu_list_new()
{
    char sql[1024];
    sprintf(sql, "select guaiwu_id, guaiwu_level, guaiwu_zhiye, is_jingying,  "
        " hp_add, gongji_add, jineng_chufa_lv_add, baojishanghai_add, baoji_lv_add,  "
        " hujia_jianmian_add, kangxing_jianmian_add "
        " from hellgate.guaiwu_list_new"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_guaiwu_list_new data;
            
            data.guaiwu_id = atoi(g_mysql->GetField("guaiwu_id"));
            data.guaiwu_level = atoi(g_mysql->GetField("guaiwu_level"));
            data.guaiwu_zhiye = atoi(g_mysql->GetField("guaiwu_zhiye"));
            data.is_jingying = atoi(g_mysql->GetField("is_jingying"));
            data.hp_add = atoi(g_mysql->GetField("hp_add"));
            data.gongji_add = atoi(g_mysql->GetField("gongji_add"));
            data.jineng_chufa_lv_add = atoi(g_mysql->GetField("jineng_chufa_lv_add"));
            data.baojishanghai_add = atoi(g_mysql->GetField("baojishanghai_add"));
            data.baoji_lv_add = atoi(g_mysql->GetField("baoji_lv_add"));
            data.hujia_jianmian_add = atoi(g_mysql->GetField("hujia_jianmian_add"));
            data.kangxing_jianmian_add = atoi(g_mysql->GetField("kangxing_jianmian_add"));
            
            
            g_guaiwu_list_new_map.insert(make_pair(data.guaiwu_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}




struct struct_task_template
{
    unsigned int task_id;
    unsigned int guanqia_id;
    unsigned int weight;
    unsigned int task_type;
    unsigned int experience;
    unsigned int silver;
    
    unsigned int zhuangbei_group_id_1;
    unsigned int ratio_1;
    unsigned int zhuangbei_group_id_2;
    unsigned int ratio_2;
    unsigned int zhuangbei_group_id_3;
    unsigned int ratio_3;
    unsigned int zhuangbei_group_id_4;
    unsigned int ratio_4;
    unsigned int zhuangbei_group_id_5;
    unsigned int ratio_5;
    unsigned int zhuangbei_group_id_6;
    unsigned int ratio_6;
    unsigned int zhuangbei_group_id_7;
    unsigned int ratio_7;
    unsigned int zhuangbei_group_id_8;
    unsigned int ratio_8;
    unsigned int zhuangbei_group_id_9;
    unsigned int ratio_9;
    unsigned int zhuangbei_group_id_10;
    unsigned int ratio_10;
    
    unsigned int monster_id_1_1;
    unsigned int monster_id_1_2;
    unsigned int monster_id_1_3;
    unsigned int monster_id_1_4;
    unsigned int monster_id_2_1;
    unsigned int monster_id_2_2;
    unsigned int monster_id_2_3;
    unsigned int monster_id_2_4;
    unsigned int monster_id_3_1;
    unsigned int monster_id_3_2;
    unsigned int monster_id_3_3;
    unsigned int monster_id_3_4;

};
std::map<unsigned int, struct_task_template> g_task_template_map;
struct task_basic
{
    unsigned int task_id;
    unsigned int normal_guanqia_count;
    unsigned int jingying_guanqia_count;
    unsigned int boss_guanqia_count;
};

bool isZhuangbeiGroupIdExist(unsigned int id)
{
    std::map<unsigned int, struct_zhuangbei_name_list>::iterator pos = g_zhuangbei_name_list_map.begin();
    for(; pos!=g_zhuangbei_name_list_map.end(); ++pos)
    {
        if(pos->second.group_id == id)
        {
            return true;
        }
    }
    return false;
}
void check_task_template()
{
    LogMsg("%s, line %d, #################check_task_template. g_task_template_map.size=[%u]."
                    , __FILE__, __LINE__, g_task_template_map.size());

    std::map<unsigned int, task_basic> task_id_map;

    std::map<unsigned int, struct_task_template>::iterator pos = g_task_template_map.begin();
    for(; pos != g_task_template_map.end(); ++pos)
    {
        //
        std::map<unsigned int, task_basic>::iterator task_pos = task_id_map.find(pos->second.task_id);
        if(task_pos == task_id_map.end())
        {
            task_basic task;
            task.task_id = pos->second.task_id;
            task.normal_guanqia_count = 0;
            task.jingying_guanqia_count = 0;
            task.boss_guanqia_count = 0;

            task_pos = task_id_map.insert(make_pair(task.task_id, task)).first;
        }
        if(pos->second.task_type == GuanQiaType_Normal)
        {
            task_pos->second.normal_guanqia_count++;
        }
        else if(pos->second.task_type == GuanQiaType_JingYing)
        {
            task_pos->second.jingying_guanqia_count++;
        }
        else if(pos->second.task_type == GuanQiaType_BOSS)
        {
            task_pos->second.boss_guanqia_count++;
        }
        else
        {
            LogMsg("%s, line %d, task_template. task_type[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.task_type, pos->second.guanqia_id);
        }
        
        //zhuangbei_group_id_1
        if(pos->second.zhuangbei_group_id_1 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_1))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_1[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_1, pos->second.guanqia_id);
        }
        //zhuangbei_group_id_2
        if(pos->second.zhuangbei_group_id_2 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_2))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_2[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_2, pos->second.guanqia_id);
        }
        //zhuangbei_group_id_3
        if(pos->second.zhuangbei_group_id_3 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_3))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_3[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_3, pos->second.guanqia_id);
        }
        //zhuangbei_group_id_4
        if(pos->second.zhuangbei_group_id_4 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_4))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_4[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_4, pos->second.guanqia_id);
        }
        //zhuangbei_group_id_5
        if(pos->second.zhuangbei_group_id_5 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_5))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_5[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_5, pos->second.guanqia_id);
        }
        //zhuangbei_group_id_6
        if(pos->second.zhuangbei_group_id_6 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_6))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_6[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_6, pos->second.guanqia_id);
        }
        //zhuangbei_group_id_7
        if(pos->second.zhuangbei_group_id_7 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_7))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_7[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_7, pos->second.guanqia_id);
        }
        //zhuangbei_group_id_8
        if(pos->second.zhuangbei_group_id_8 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_8))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_8[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_8, pos->second.guanqia_id);
        }
        //zhuangbei_group_id_9
        if(pos->second.zhuangbei_group_id_9 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_9))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_9[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_9, pos->second.guanqia_id);
        }
        //zhuangbei_group_id_10
        if(pos->second.zhuangbei_group_id_10 > 0
            && !isZhuangbeiGroupIdExist(pos->second.zhuangbei_group_id_10))
        {
            LogMsg("%s, line %d, task_template. zhuangbei_group_id_10[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.zhuangbei_group_id_10, pos->second.guanqia_id);
        }

        //monster_id_1_1
        if(pos->second.monster_id_1_1 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_1_1) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_1_1[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_1_1, pos->second.guanqia_id);
        }
        //monster_id_1_2
        if(pos->second.monster_id_1_2 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_1_2) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_1_2[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_1_2, pos->second.guanqia_id);
        }
        //monster_id_1_3
        if(pos->second.monster_id_1_3 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_1_3) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_1_3[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_1_3, pos->second.guanqia_id);
        }
        //monster_id_1_4
        if(pos->second.monster_id_1_4 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_1_4) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_1_4[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_1_4, pos->second.guanqia_id);
        }
        //monster_id_2_1
        if(pos->second.monster_id_2_1 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_2_1) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_2_1[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_2_1, pos->second.guanqia_id);
        }
        //monster_id_2_2
        if(pos->second.monster_id_2_2 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_2_2) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_2_2[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_2_2, pos->second.guanqia_id);
        }
        //monster_id_2_3
        if(pos->second.monster_id_2_3 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_2_3) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_2_3[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_2_3, pos->second.guanqia_id);
        }
        //monster_id_2_4
        if(pos->second.monster_id_2_4 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_2_4) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_2_4[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_2_4, pos->second.guanqia_id);
        }
        //monster_id_3_1
        if(pos->second.monster_id_3_1 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_3_1) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_3_1[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_3_1, pos->second.guanqia_id);
        }
        //monster_id_3_2
        if(pos->second.monster_id_3_2 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_3_2) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_3_2[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_3_2, pos->second.guanqia_id);
        }
        //monster_id_3_3
        if(pos->second.monster_id_3_3 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_3_3) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_3_3[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_3_3, pos->second.guanqia_id);
        }
        //monster_id_3_4
        if(pos->second.monster_id_3_4 > 0
            && g_guaiwu_list_new_map.find(pos->second.monster_id_3_4) == g_guaiwu_list_new_map.end())
        {
            LogMsg("%s, line %d, task_template. monster_id_3_4[%u] invalide. guanqia_id[%u]"
                    , __FILE__, __LINE__, pos->second.monster_id_3_4, pos->second.guanqia_id);
        }
    }

    std::map<unsigned int, task_basic>::iterator task_pos = task_id_map.begin();
    for(; task_pos!=task_id_map.end(); ++task_pos)
    {
        if(task_pos->second.normal_guanqia_count != 8)
        {
            LogMsg("%s, line %d, task_template. normal_guanqia_count[%u] invalide. task_id[%u]"
                    , __FILE__, __LINE__, task_pos->second.normal_guanqia_count, task_pos->second.task_id);
        }
        if(task_pos->second.jingying_guanqia_count < JingyingGuanqiaCountMin)
        {
            LogMsg("%s, line %d, task_template. jingying_guanqia_count[%u] invalide. task_id[%u]"
                    , __FILE__, __LINE__, task_pos->second.jingying_guanqia_count, task_pos->second.task_id);
        }
        if(task_pos->second.boss_guanqia_count != 1)
        {
            LogMsg("%s, line %d, task_template. boss_guanqia_count[%u] invalide. task_id[%u]"
                    , __FILE__, __LINE__, task_pos->second.boss_guanqia_count, task_pos->second.task_id);
        }
    }
}
void load_task_template()
{
    char sql[1024];
    sprintf(sql, "select task_id, guanqia_id, weight, task_type, experience, silver, "
        " zhuangbei_group_id_1, zhuangbei_group_id_2, zhuangbei_group_id_3, zhuangbei_group_id_4, zhuangbei_group_id_5, "
        " zhuangbei_group_id_6, zhuangbei_group_id_7, zhuangbei_group_id_8, zhuangbei_group_id_9, zhuangbei_group_id_10, "
        " ratio_1, ratio_2, ratio_3, ratio_4, ratio_5, ratio_6, ratio_7, ratio_8, ratio_9, ratio_10, "
        " monster_id_1_1, monster_id_1_2, monster_id_1_3, monster_id_1_4, "
        " monster_id_2_1, monster_id_2_2, monster_id_2_3, monster_id_2_4, "
        " monster_id_3_1, monster_id_3_2, monster_id_3_3, monster_id_3_4 "
        " from hellgate.task_template"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_task_template data;
            
            data.task_id = atoi(g_mysql->GetField("task_id"));
            data.guanqia_id = atoi(g_mysql->GetField("guanqia_id"));
            data.weight = atoi(g_mysql->GetField("weight"));
            data.task_type = atoi(g_mysql->GetField("task_type"));
            data.experience = atoi(g_mysql->GetField("experience"));
            data.silver = atoi(g_mysql->GetField("silver"));
            
            data.zhuangbei_group_id_1 = atoi(g_mysql->GetField("zhuangbei_group_id_1"));
            data.zhuangbei_group_id_2 = atoi(g_mysql->GetField("zhuangbei_group_id_2"));
            data.zhuangbei_group_id_3 = atoi(g_mysql->GetField("zhuangbei_group_id_3"));
            data.zhuangbei_group_id_4 = atoi(g_mysql->GetField("zhuangbei_group_id_4"));
            data.zhuangbei_group_id_5 = atoi(g_mysql->GetField("zhuangbei_group_id_5"));
            data.zhuangbei_group_id_6 = atoi(g_mysql->GetField("zhuangbei_group_id_6"));
            data.zhuangbei_group_id_7 = atoi(g_mysql->GetField("zhuangbei_group_id_7"));
            data.zhuangbei_group_id_8 = atoi(g_mysql->GetField("zhuangbei_group_id_8"));
            data.zhuangbei_group_id_9 = atoi(g_mysql->GetField("zhuangbei_group_id_9"));
            data.zhuangbei_group_id_10 = atoi(g_mysql->GetField("zhuangbei_group_id_10"));
            
            data.ratio_1 = atoi(g_mysql->GetField("ratio_1"));
            data.ratio_2= atoi(g_mysql->GetField("ratio_2"));
            data.ratio_3= atoi(g_mysql->GetField("ratio_3"));
            data.ratio_4= atoi(g_mysql->GetField("ratio_4"));
            data.ratio_5= atoi(g_mysql->GetField("ratio_5"));
            data.ratio_6= atoi(g_mysql->GetField("ratio_6"));
            data.ratio_7= atoi(g_mysql->GetField("ratio_7"));
            data.ratio_8= atoi(g_mysql->GetField("ratio_8"));
            data.ratio_9= atoi(g_mysql->GetField("ratio_9"));
            data.ratio_10= atoi(g_mysql->GetField("ratio_10"));
            
            data.monster_id_1_1 = atoi(g_mysql->GetField("monster_id_1_1"));
            data.monster_id_1_2 = atoi(g_mysql->GetField("monster_id_1_2"));
            data.monster_id_1_3 = atoi(g_mysql->GetField("monster_id_1_3"));
            data.monster_id_1_4 = atoi(g_mysql->GetField("monster_id_1_4"));
            data.monster_id_2_1 = atoi(g_mysql->GetField("monster_id_2_1"));
            data.monster_id_2_2 = atoi(g_mysql->GetField("monster_id_2_2"));
            data.monster_id_2_3 = atoi(g_mysql->GetField("monster_id_2_3"));
            data.monster_id_2_4 = atoi(g_mysql->GetField("monster_id_2_4"));
            data.monster_id_3_1 = atoi(g_mysql->GetField("monster_id_3_1"));
            data.monster_id_3_2 = atoi(g_mysql->GetField("monster_id_3_2"));
            data.monster_id_3_3 = atoi(g_mysql->GetField("monster_id_3_3"));
            data.monster_id_3_4 = atoi(g_mysql->GetField("monster_id_3_4"));
            
            g_task_template_map.insert(make_pair(data.guanqia_id, data));
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}



//create table market (item_type int unsigned, item_count int unsigned default 1, gold_count int unsigned not null);
struct struct_market
{
    unsigned int item_type;
    unsigned int item_count;
    unsigned int gold_count;
};
std::vector<struct_market> g_market;
void check_market()
{
}
void load_market()
{
    char sql[1024];
    sprintf(sql, "select item_type, item_count, gold_count "
        " from hellgate.market"
        );
    try
    {
        g_mysql->Query(sql);
        while(g_mysql->FetchRow() != NULL)
        {
            struct_market data;
            
            data.item_type = atoi(g_mysql->GetField("item_type"));
            data.item_count = atoi(g_mysql->GetField("item_count"));
            data.gold_count = atoi(g_mysql->GetField("gold_count"));
            
            
            g_market.push_back(data);
        }
        
        g_mysql->FreeResult();
    }
    catch(CMysqlException& e)
    {
        LogMsg("%s, line %d, mysql exception. [%s]"
                    , __FILE__, __LINE__, e.GetErrMsg());
        //
        exit(1);
    }
}


 

 
void loadAllData()
{
    load_client_id();
    load_profession_list();
    load_profession_level_attr();
    load_hero_list();
    load_hero_infor();
    load_zhuangbei_name_list_copy();
    load_zhuangbei_list();
    load_items_in_team();
    load_zb_qianghua_chenggonglv();
    load_hero_maintask_progress();
    load_subtask_daily_zhuangbei();
    load_hero_progress_subtask_daily_zhuangbei();
    load_subtask_daily_baoshi();
    load_hero_progress_subtask_daily_baoshi();
    load_task_chessboard();
    load_task_template();
    load_guaiwu_profession_type_list();
    load_guaiwu_profession_list();
    load_guaiwu_list_new();
    load_market();
}

void checkAllData()
{
    check_client_id();
    check_profession_list();
    check_profession_level_attr();
    check_hero_list();
    check_hero_infor();
    check_zhuangbei_name_list_copy();
    check_zhuangbei_list();
    check_items_in_team();
    check_zb_qianghua_chenggonglv();
    check_hero_maintask_progress();
    check_subtask_daily_zhuangbei();
    check_hero_progress_subtask_daily_zhuangbei();
    check_subtask_daily_baoshi();
    check_hero_progress_subtask_daily_baoshi();
    check_task_chessboard();
    check_task_template();
    check_guaiwu_profession_type_list();
    check_guaiwu_profession_list();
    check_guaiwu_list_new();
    check_market();

}



int main(int argc, char* argv[])
{
	char property_file[256];

    LogInit(argv[0]);

	if(argc >= 2)
	{
		strcpy(property_file, argv[1]);
	}
	else
	{
		LogMsg("start server failed. argc=[%d]", argc);
        return 1;
	}

	//ignore some signal
	signal(SIGPIPE, SIG_IGN);
    

    //load config
    SDConfiguration conf;
    if(!conf.load(argv[1]))
    {
        LogMsg("load confige file failed. file path=[%s]", argv[1]);
        return 1;
    }
    
    g_db_ip = conf.getString("db_ip");
    if(g_db_ip == "")
    {
        LogMsg("db_ip is null");
        return 1;
    }
    g_db_username = conf.getString("db_username");
    if(g_db_username == "")
    {
        LogMsg("db_username is null");
        return 1;
    }
    g_db_psw = conf.getString("db_psw");
    if(g_db_psw == "")
    {
        LogMsg("db_psw is null");
        return 1;
    }

    //连接数据库
    {
        try
        {
            g_mysql = new CMysql(g_db_ip.c_str(), g_db_username.c_str()
                , g_db_psw.c_str(), true);
            g_mysql->Connect();

        }
        catch(CMysqlException& e)
        {
             LogMsg("%s, line %d, mysql connect failed. abort. [%s]"
                        , __FILE__, __LINE__, e.GetErrMsg());

            exit(1);
            return 0;
        }
    }

    //加载数据库数据
    loadAllData();
    //检查
    checkAllData();

    return 0;
}

