
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
#include <list>
#include <map>
#include <string.h>
#include <math.h>

#include "utility/logs.h"
#include "common/SDConfiguration.h"
#include "cmysql.h"
#include "protocol.h"


#include "os.h"
#include "PacketQueue.h"


//1级单手武器攻击力
#define DanShouWuqi_Gongji_Base 9.5
//1级双手武器攻击力
#define ShuangShouWuqi_Gongji_Base 12.4
//武器攻击力每升高一级的提升比率
#define Wuqi_Gongji_Multi_PerLevel 1.05
//稀有武器是普通武器攻击的多少倍
#define XiyouWuqi_Gongji_Multi 1.3
//传奇武器是普通武器攻击的多少倍
#define ChuanqiWuqi_Gongji_Multi 1.7


//1级重盔甲护甲值
#define Zhongkuijia_Hujia_Base 6.5
//1级轻盔甲护甲值
#define Qingkuijia_Hujia_Base 5.53
//1级盾护甲值
#define Dunpai_Hujia_Base 2.28
//稀有防具是普通防具护甲值的多少倍
#define Xiyou_Fangju_Hujia_Multi 1.5
//传奇防具是普通防具护甲值的多少倍
#define Chuanqi_Fangju_Hujia_Multi 2.0


//1级戒指主属性
#define Jiezhi_Zhushuxing_Base 12
//1级护身符主属性
#define Hushenfu_Zhushuxing_Base 18
//1级法器主属性
#define Faqi_Zhushuxing_Base 12
//首饰每升一级主属性提升比率
#define Shoushi_Zhushuxing_Multi_PerLevel 1.05
//稀有首饰是普通防具主属性的多少倍
#define Xiyou_Shoushi_Zhushuxing_Multi 1.3
//传奇首饰是普通防具主属性的多少倍
#define Chuanqi_Shoushi_Zhushuxing_Multi 1.7


//武器主属性相当于戒指主属性的多少
#define Wuqi_Zhushuxing_Multi_Jiezhi 0.8


//装备属性配置key字符串
#define ZB_Key_gailv       "gailv"
#define ZB_Key_count       "count"
#define ZB_Key_kangxing    "kx"
#define ZB_Key_mingjie     "mj"
#define ZB_Key_zhili       "zl"
#define ZB_Key_liliang     "ll"
#define ZB_Key_tineng      "tn"
#define ZB_Key_baoji       "bj"
#define ZB_Key_baoshang    "bs"
#define ZB_Key_hp_ration   "hp"
#define ZB_Key_gedanglv    "gdlv"
#define ZB_Key_gedangzhi   "gdzi"
#define ZB_Key_gongji_min_add  "gjmin"
#define ZB_Key_gongji_max_add  "gjmax"
//#define ZB_Key_skill_ratio_add  "sklrt"
#define ZB_Key_hujia_add  "hujia"
#define ZB_Key_skill_zhouqi "sklzq"
#define ZB_Key_skill_turn "skltn"
#define ZB_Key_skill_result "sklst"





static std::string g_db_ip;
static std::string g_db_username;
static std::string g_db_psw;

static CMysql* g_mysql;

bool is_wuqi(ZhuangbeiType type)
{
    if(type == ZhuangbeiType_DanshouJian
        || type == ZhuangbeiType_ShuangshouBishou
        || type == ZhuangbeiType_ShuangshouFu
        || type == ZhuangbeiType_DanshouFazhang
        || type == ZhuangbeiType_Gong
        || type == ZhuangbeiType_ShuangshouFazhang)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool is_fangju(ZhuangbeiType type)
{
    if(type == ZhuangbeiType_Dunpai
        || type == ZhuangbeiType_KuijiaQin
        || type == ZhuangbeiType_KuijiaZhong)
    {
        return true;
    }
    else
    {
        return false;
    }
}


unsigned int del4add5(float num)
{
    unsigned int v = (unsigned int)(num*10);
    v %= 10;
    if(v >=5)
    {
        return (unsigned int)(num+1);
    }
    else
    {
        return (unsigned int)num;
    }
}


struct zb_template_basic_info
{
    unsigned int zb_name_id;
    unsigned int zb_group_id;

    unsigned int bind_profession_id;

    unsigned int zb_type;
    unsigned int zb_colour;
    unsigned int zb_level;

    std::string zb_name;
};
struct zb_template_detail
{
    zb_template_basic_info basic_info;

    bool is_boss_zhuangbei;

    unsigned int gongji_min_min;
    unsigned int gongji_min_max;
    unsigned int gongji_max_min;
    unsigned int gongji_max_max;
    
    unsigned int wufang_min;
    unsigned int wufang_max;
    
    std::vector<std::string> attr_vec;
};

//计算武器的攻击
void calc_wuqi_gongji(unsigned int zb_color, unsigned int zb_level, unsigned int zb_type
    , unsigned int& gongji_min_min, unsigned int& gongji_min_max
    , unsigned int& gongji_max_min, unsigned int& gongji_max_max)
{
    //白装平均攻击
    float basic_gongji = 0;
    if(zb_type == ZhuangbeiType_DanshouJian || zb_type == ZhuangbeiType_DanshouFazhang)
    {
        basic_gongji = DanShouWuqi_Gongji_Base*powf(Wuqi_Gongji_Multi_PerLevel, zb_level-1);
    }
    else
    {
        basic_gongji = ShuangShouWuqi_Gongji_Base*powf(Wuqi_Gongji_Multi_PerLevel, zb_level-1);
    }

    //本装备平均攻击
    float pingjun_gongji = basic_gongji;
    if(zb_color == ZhuangbeiColour_Xiyou)
    {
        pingjun_gongji *= XiyouWuqi_Gongji_Multi;
    }
    else if(zb_color == ZhuangbeiColour_Chuanqi)
    {
        pingjun_gongji *= ChuanqiWuqi_Gongji_Multi;
    }

    //计算最大和最小攻击
    float gongji_min = pingjun_gongji*0.9;
    float gongji_max = pingjun_gongji*1.1;
    if(zb_color == ZhuangbeiColour_Xiyou || zb_color == ZhuangbeiColour_Chuanqi)
    {
        gongji_min_min = del4add5(gongji_min);
        gongji_min_max = del4add5(gongji_min);
        gongji_max_min = del4add5(gongji_max);
        gongji_max_max = del4add5(gongji_max);
    }
    else
    {
        gongji_min_min = del4add5(gongji_min*0.95);
        gongji_min_max = del4add5(gongji_min*1.05);
        gongji_max_min = del4add5(gongji_max*0.95);
        gongji_max_max = del4add5(gongji_max*1.05);
    }
}

//计算防具的护甲
void calc_fangju_hujia(unsigned int zb_color, unsigned int zb_level, unsigned int zb_type
    , unsigned int& hujia_min, unsigned int& hujia_max)
{
    //白装平均护甲
    float basic_hujia = 0;
    if(zb_type == ZhuangbeiType_KuijiaZhong)
    {
        basic_hujia = Zhongkuijia_Hujia_Base*zb_level;
    }
    else if(zb_type == ZhuangbeiType_KuijiaQin)
    {
        basic_hujia = Qingkuijia_Hujia_Base*zb_level;
    }
    else
    {
        basic_hujia = Dunpai_Hujia_Base*zb_level;
    }

    //本装备平均护甲
    float pingjun_hujia = basic_hujia;
    if(zb_color == ZhuangbeiColour_Xiyou)
    {
        pingjun_hujia *= Xiyou_Fangju_Hujia_Multi;
    }
    else if(zb_color == ZhuangbeiColour_Chuanqi)
    {
        pingjun_hujia *= Chuanqi_Fangju_Hujia_Multi;
    }

    //计算最大和最小护甲
    if(zb_color == ZhuangbeiColour_Xiyou || zb_color == ZhuangbeiColour_Chuanqi)
    {
        hujia_min = del4add5(pingjun_hujia);
        hujia_max = del4add5(pingjun_hujia);
    }
    else
    {
        hujia_min = del4add5(pingjun_hujia*0.95);
        hujia_max = del4add5(pingjun_hujia*1.05);
    }
}

void calc_wuqi_shuxing(unsigned int zb_color, unsigned int zb_level, unsigned int zb_type
    , unsigned int bind_profession_id
    , std::vector<std::string>& attr_vec)
{
    attr_vec.clear();

    //白装不带属性
    if(zb_color == ZhuangbeiColour_Baizhuang)
    {
        return;
    }

    //主属性
    {
        std::string attr("gailv=100");
        if(zb_type == ZhuangbeiType_DanshouJian)
        {
            float attr_val 
                = Wuqi_Zhushuxing_Multi_Jiezhi*Jiezhi_Zhushuxing_Base*powf(Shoushi_Zhushuxing_Multi_PerLevel, zb_level-1);
            
            if(zb_color == ZhuangbeiColour_Chuanqi)
            {
                char buf[64];
                
                if(bind_profession_id == Profession_Qishi
                    || bind_profession_id == Profession_WuSeng)
                {
                    //力量
                    sprintf(buf, ",count=1,ll=%u", del4add5(attr_val));
                }
                else if(bind_profession_id == 0)
                {
                    //力量
                    sprintf(buf, ",count=2,ll=%u,mj=%u", del4add5(attr_val), del4add5(attr_val));
                }
                else
                {
                    //敏捷
                    sprintf(buf, ",count=1,mj=%u", del4add5(attr_val));
                }
                
                attr = attr + buf;
            }
            else if(zb_color == ZhuangbeiColour_Xiyou)
            {
                //力量和敏捷只带一个
                char buf[64];
                sprintf(buf, ",count=1,mj=%u,ll=%u", del4add5(attr_val), del4add5(attr_val));

                attr = attr + buf;
            }
            else
            {
                //力量和敏捷只带一个
                char buf[64];
                sprintf(buf, ",count=1,mj=%u~%u,ll=%u~%u"
                    , del4add5(attr_val*0.6), del4add5(attr_val)
                    , del4add5(attr_val*0.6), del4add5(attr_val));

                attr = attr + buf;
            }
        }
        else if(zb_type == ZhuangbeiType_DanshouFazhang)
        {
            float attr_val 
                = Wuqi_Zhushuxing_Multi_Jiezhi*Jiezhi_Zhushuxing_Base*powf(Shoushi_Zhushuxing_Multi_PerLevel, zb_level-1);
            
            if(zb_color == ZhuangbeiColour_Chuanqi)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,zl=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else if(zb_color == ZhuangbeiColour_Xiyou)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,zl=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,zl=%u~%u", del4add5(attr_val*0.6), del4add5(attr_val));

                attr = attr + buf;
            }
        }
        else if(zb_type == ZhuangbeiType_ShuangshouFazhang)
        {
            float attr_val 
                = Wuqi_Zhushuxing_Multi_Jiezhi*Jiezhi_Zhushuxing_Base*powf(Shoushi_Zhushuxing_Multi_PerLevel, zb_level-1);

            if(zb_color == ZhuangbeiColour_Chuanqi)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,zl=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else if(zb_color == ZhuangbeiColour_Xiyou)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,zl=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,zl=%u~%u", del4add5(attr_val*0.6), del4add5(attr_val));

                attr = attr + buf;
            }
        }
        else if(zb_type == ZhuangbeiType_ShuangshouBishou)
        {
            float attr_val 
                = Wuqi_Zhushuxing_Multi_Jiezhi*Jiezhi_Zhushuxing_Base*powf(Shoushi_Zhushuxing_Multi_PerLevel, zb_level-1);

            if(zb_color == ZhuangbeiColour_Chuanqi)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,mj=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else if(zb_color == ZhuangbeiColour_Xiyou)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,mj=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,mj=%u~%u", del4add5(attr_val*0.6), del4add5(attr_val));

                attr = attr + buf;
            }
        }
        else if(zb_type == ZhuangbeiType_ShuangshouFu)
        {
            float attr_val 
                = Wuqi_Zhushuxing_Multi_Jiezhi*Jiezhi_Zhushuxing_Base*powf(Shoushi_Zhushuxing_Multi_PerLevel, zb_level-1);

            if(zb_color == ZhuangbeiColour_Chuanqi)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,ll=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else if(zb_color == ZhuangbeiColour_Xiyou)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,ll=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,ll=%u~%u", del4add5(attr_val*0.6), del4add5(attr_val));

                attr = attr + buf;
            }
        }
        else if(zb_type == ZhuangbeiType_Gong)
        {
            float attr_val 
                = Wuqi_Zhushuxing_Multi_Jiezhi*Jiezhi_Zhushuxing_Base*powf(Shoushi_Zhushuxing_Multi_PerLevel, zb_level-1);

            if(zb_color == ZhuangbeiColour_Chuanqi)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,mj=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else if(zb_color == ZhuangbeiColour_Xiyou)
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,mj=%u", del4add5(attr_val));

                attr = attr + buf;
            }
            else
            {
                //
                char buf[64];
                sprintf(buf, ",count=1,mj=%u~%u", del4add5(attr_val*0.6), del4add5(attr_val));

                attr = attr + buf;
            }
        }
        else
        {
            assert(false);
        }

        attr_vec.push_back(attr);
    }
    

    //暴击爆伤
    {
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            unsigned int baoshang = 0;
            if(zb_level <= 5)
            {
                baoshang = 20;
            }
            else if(zb_level <= 15)
            {
                baoshang = 25;
            }
            else if(zb_level <= 25)
            {
                baoshang = 30;
            }
            else
            {
                baoshang = 40;
            }

            
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=2,bj=%u,bs=%u", 10, baoshang);

            attr = attr + buf;
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            unsigned int baoshang = 0;
            if(zb_level <= 10)
            {
                baoshang = 15;
            }
            else if(zb_level <= 20)
            {
                baoshang = 20;
            }
            else if(zb_level <= 25)
            {
                baoshang = 25;
            }
            else
            {
                baoshang = 30;
            }

            
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=2,bj=%u,bs=%u", 5, baoshang);

            attr = attr + buf;
        }
        else
        {
            unsigned int baoshang = 0;
            if(zb_level <= 10)
            {
                baoshang = 10;
            }
            else if(zb_level <= 20)
            {
                baoshang = 15;
            }
            else if(zb_level <= 25)
            {
                baoshang = 20;
            }
            else
            {
                baoshang = 25;
            }

            
            //
            char buf[64];
            sprintf(buf, "gailv=50,count=1,bj=%u,bs=%u", 5, baoshang);

            attr = attr + buf;
        }

        attr_vec.push_back(attr);
    }
}

float kuijia_tineng_arr[] 
    = {4.0,8.0,12.0,16.0,20.0,25.0,31.0,38.0,46.0,55.0,59.0
    ,63.0,67.0,71.0,75.0,79.0,83.0,87.0,92.0,97.0,124.0,141.0
    ,158.0,175.0,183.0,199.0,215.0,231.0,257.0,263.0};
void calc_kuijia_shuxing(unsigned int zb_color, unsigned int zb_level, unsigned int zb_type
    , std::vector<std::string>& attr_vec)
{
    attr_vec.clear();

    //白装不带属性
    if(zb_color == ZhuangbeiColour_Baizhuang)
    {
        return;
    }

    //体能
    {
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,tn=%u", del4add5(kuijia_tineng_arr[zb_level-1]));

            attr = attr + buf;
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,tn=%u", del4add5(kuijia_tineng_arr[zb_level-1]));

            attr = attr + buf;
        }
        else
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,tn=%u~%u"
                , del4add5(kuijia_tineng_arr[zb_level-1]*0.7), del4add5(kuijia_tineng_arr[zb_level-1]));

            attr = attr + buf;
        }

        attr_vec.push_back(attr);
    }
}

void calc_dunpai_shuxing(unsigned int zb_color, unsigned int zb_level, unsigned int zb_type
    , std::vector<std::string>& attr_vec)
{
    attr_vec.clear();

    //白装不带属性
    if(zb_color == ZhuangbeiColour_Baizhuang)
    {
        return;
    }

    //体能
    {
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,tn=%u", del4add5(kuijia_tineng_arr[zb_level-1]*0.4));

            attr = attr + buf;
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,tn=%u", del4add5(kuijia_tineng_arr[zb_level-1]*0.4));

            attr = attr + buf;
        }
        else
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,tn=%u~%u"
                , del4add5(kuijia_tineng_arr[zb_level-1]*0.4*0.8), del4add5(kuijia_tineng_arr[zb_level-1]*0.4));

            attr = attr + buf;
        }

        attr_vec.push_back(attr);
    }
}

void calc_hushenfu_shuxing(unsigned int zb_color, unsigned int zb_level, unsigned int zb_type
    , unsigned int bind_profession_id
    , std::vector<std::string>& attr_vec)
{
    attr_vec.clear();

    //主属性
    {
        float attr_val = Hushenfu_Zhushuxing_Base*powf(Shoushi_Zhushuxing_Multi_PerLevel, zb_level-1);
        
        
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            char buf[64];
            
            if(bind_profession_id == Profession_Qishi
                || bind_profession_id == Profession_WuSeng
                || bind_profession_id == Profession_Yemanren
                )
            {
                //力量
                sprintf(buf, "gailv=100,count=1,ll=%u", del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi));
            }
            else if(bind_profession_id == Profession_YouXia
                || bind_profession_id == Profession_Chike
                || bind_profession_id == Profession_LieShou
                || bind_profession_id == Profession_ShenJianShou
                )
            {
                //敏捷
                sprintf(buf, "gailv=100,count=1,mj=%u", del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi));
            }
            else if(bind_profession_id == 0)
            {
                sprintf(buf, "gailv=100,count=3,mj=%u,ll=%u,zl=%u"
                    , del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi)
                    , del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi)
                    , del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi));
            }
            else
            {
                //智力
                sprintf(buf, "gailv=100,count=1,zl=%u", del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi));
            }
            

            attr = attr + buf;
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,mj=%u,ll=%u,zl=%u"
                , del4add5(attr_val*Xiyou_Shoushi_Zhushuxing_Multi)
                , del4add5(attr_val*Xiyou_Shoushi_Zhushuxing_Multi)
                , del4add5(attr_val*Xiyou_Shoushi_Zhushuxing_Multi));

            attr = attr + buf;
        }
        else
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,mj=%u~%u,ll=%u~%u,zl=%u~%u"
                , del4add5(attr_val*0.8), del4add5(attr_val)
                , del4add5(attr_val*0.8), del4add5(attr_val)
                , del4add5(attr_val*0.8), del4add5(attr_val));

            attr = attr + buf;
        }

        attr_vec.push_back(attr);
    }

    //暴击爆伤
    {
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            unsigned int baoshang = 0;
            if(zb_level <= 10)
            {
                baoshang = 20;
            }
            else if(zb_level <= 20)
            {
                baoshang = 25;
            }
            else if(zb_level <= 25)
            {
                baoshang = 30;
            }
            else
            {
                baoshang = 40;
            }

            
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=2,bj=%u,bs=%u", 10, baoshang);

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            unsigned int baoshang = 0;
            if(zb_level <= 10)
            {
                baoshang = 15;
            }
            else if(zb_level <= 20)
            {
                baoshang = 20;
            }
            else if(zb_level <= 25)
            {
                baoshang = 25;
            }
            else
            {
                baoshang = 30;
            }

            
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=2,bj=%u,bs=%u", 5, baoshang);

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }


    }

    //技能触发
    {
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            //
            char buf[64];
            
            if(zb_level < 20)
            {
                sprintf(buf, "gailv=100,count=2,sklzq=%u,skltn=%u", 1, 1);
            }
            else if(zb_level < 30)
            {
                sprintf(buf, "gailv=100,count=2,sklzq=%u,skltn=%u", 2, 2);
            }
            else
            {
                sprintf(buf, "gailv=100,count=2,sklzq=%u,skltn=%u", 4, 2);
            }

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            //
            char buf[64];
            
            if(zb_level < 20)
            {
                sprintf(buf, "gailv=100,count=1,sklzq=%u,skltn=%u", 1, 1);
            }
            else if(zb_level < 30)
            {
                sprintf(buf, "gailv=100,count=1,sklzq=%u,skltn=%u", 2, 2);
            }
            else
            {
                sprintf(buf, "gailv=100,count=1,sklzq=%u,skltn=%u", 4, 4);
            }

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }
    }
    
}

void calc_jiezhi_shuxing(unsigned int zb_color, unsigned int zb_level, unsigned int zb_type
    , unsigned int bind_profession_id
    , std::vector<std::string>& attr_vec)
{
    attr_vec.clear();

    //暂时只处理传奇和稀有戒指
    //蓝装戒指手动配
    if(zb_color == ZhuangbeiColour_Baizhuang || zb_color == ZhuangbeiColour_Fumo)
    {
        return;
    }

    //主属性
    {
        float attr_val = Jiezhi_Zhushuxing_Base*powf(Shoushi_Zhushuxing_Multi_PerLevel, zb_level-1);
        
        
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            char buf[64];
            
            if(bind_profession_id == Profession_Qishi
                || bind_profession_id == Profession_WuSeng
                || bind_profession_id == Profession_Yemanren
                )
            {
                //力量
                sprintf(buf, "gailv=100,count=1,ll=%u", del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi));
            }
            else if(bind_profession_id == Profession_YouXia
                || bind_profession_id == Profession_Chike
                || bind_profession_id == Profession_LieShou
                || bind_profession_id == Profession_ShenJianShou
                )
            {
                //敏捷
                sprintf(buf, "gailv=100,count=1,mj=%u", del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi));
            }
            else if(bind_profession_id == 0)
            {
                sprintf(buf, "gailv=100,count=3,mj=%u,ll=%u,zl=%u"
                    , del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi)
                    , del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi)
                    , del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi));
            }
            else
            {
                //智力
                sprintf(buf, "gailv=100,count=1,zl=%u", del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi));
            }
            

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,mj=%u,ll=%u,zl=%u,tn=%u"
                , del4add5(attr_val*Xiyou_Shoushi_Zhushuxing_Multi)
                , del4add5(attr_val*Xiyou_Shoushi_Zhushuxing_Multi)
                , del4add5(attr_val*Xiyou_Shoushi_Zhushuxing_Multi)
                , del4add5(attr_val*Xiyou_Shoushi_Zhushuxing_Multi));

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }

        
    }

    //暴击爆伤
    {
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            unsigned int baoshang = 0;
            if(zb_level <= 10)
            {
                baoshang = 15;
            }
            else if(zb_level <= 20)
            {
                baoshang = 20;
            }
            else if(zb_level <= 25)
            {
                baoshang = 25;
            }
            else
            {
                baoshang = 30;
            }

            
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=2,bj=%u,bs=%u", 10, baoshang);

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            unsigned int baoshang = 0;
            if(zb_level <= 10)
            {
                baoshang = 10;
            }
            else if(zb_level <= 20)
            {
                baoshang = 15;
            }
            else if(zb_level <= 25)
            {
                baoshang = 20;
            }
            else
            {
                baoshang = 25;
            }

            
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=2,bj=%u,bs=%u", 5, baoshang);

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }


    }

    //技能触发
    {
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            //
            char buf[64];
            
            if(zb_level < 20)
            {
                sprintf(buf, "gailv=100,count=2,sklzq=%u,skltn=%u", 1, 1);
            }
            else if(zb_level < 30)
            {
                sprintf(buf, "gailv=100,count=2,sklzq=%u,skltn=%u", 2, 1);
            }
            else
            {
                sprintf(buf, "gailv=100,count=2,sklzq=%u,skltn=%u", 3, 2);
            }

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            //
            char buf[64];
            
            if(zb_level < 20)
            {
                sprintf(buf, "gailv=100,count=1,sklzq=%u,skltn=%u", 1, 1);
            }
            else if(zb_level < 30)
            {
                sprintf(buf, "gailv=100,count=1,sklzq=%u,skltn=%u", 2, 2);
            }
            else
            {
                sprintf(buf, "gailv=100,count=1,sklzq=%u,skltn=%u", 3, 3);
            }

            attr = attr + buf;

            //
            attr_vec.push_back(attr);
        }
    }
}

void calc_faqi_shuxing(unsigned int zb_color, unsigned int zb_level, unsigned int zb_type
    , std::vector<std::string>& attr_vec)
{
    attr_vec.clear();
    
    //主属性
    {
        float attr_val = Faqi_Zhushuxing_Base*powf(Shoushi_Zhushuxing_Multi_PerLevel, zb_level-1);
        
        
        std::string attr("");

        if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,zl=%u"
                , del4add5(attr_val*Chuanqi_Shoushi_Zhushuxing_Multi));

            attr = attr + buf;
        }
        else if(zb_color == ZhuangbeiColour_Xiyou)
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,zl=%u", del4add5(attr_val*Xiyou_Shoushi_Zhushuxing_Multi));

            attr = attr + buf;
        }
        else
        {
            //
            char buf[64];
            sprintf(buf, "gailv=100,count=1,zl=%u~%u"
                , del4add5(attr_val*0.6), del4add5(attr_val));

            attr = attr + buf;
        }

        attr_vec.push_back(attr);
    }

    //攻击
    {
        //法器攻击力补偿=双手法杖攻击-单手法杖攻击
        float danshou = 
            DanShouWuqi_Gongji_Base*powf(Wuqi_Gongji_Multi_PerLevel, zb_level-1);
        float shuangshou = 
            ShuangShouWuqi_Gongji_Base*powf(Wuqi_Gongji_Multi_PerLevel, zb_level-1);
        float gongji_attr = shuangshou - danshou;
        if(zb_color == ZhuangbeiColour_Xiyou)
        {
            gongji_attr *= XiyouWuqi_Gongji_Multi;
        }
        else if(zb_color == ZhuangbeiColour_Chuanqi)
        {
            gongji_attr *= ChuanqiWuqi_Gongji_Multi;
        }

        //
        char attr[64];
        sprintf(attr, "gailv=100,count=2,gjmin=%u,gjmax=%u", del4add5(gongji_attr), del4add5(gongji_attr));


        attr_vec.push_back(attr);
    }
}

//填写传奇装备升级信息表
struct ZBChuangqiInfo
{
    unsigned int zb_name_id;
    unsigned int zb_group_id;

    unsigned int zb_level;
    std::string zb_name;
};
struct ZBList
{
    std::list<ZBChuangqiInfo> zb_list; //按照等级从小到大
};
struct ZBTypeMap
{
    std::map<unsigned int, ZBList> type_map;
};
struct ChuanqiZBPool
{
    std::map<unsigned int, ZBTypeMap> prof_map;
};
void update_chuanqi_info()
{

    //先清空
    {
        char sql[1024];
        sprintf(sql, "delete from hellgate.shenbing_id_list");
        try
        {
            g_mysql->Query(sql);
            
        }
        catch(CMysqlException& e)
        {
            LogMsg("%s, line %d, mysql exception. [%s]"
                        , __FILE__, __LINE__, e.GetErrMsg());
            //
            exit(1);
        }
    }
    
    //
    ChuanqiZBPool zb_pool;
    
    
    //
    std::vector<zb_template_basic_info> zb_temp_vec;
    
    {
        char sql[1024];
        sprintf(sql, "select zhuangbei_name_id, group_id, bind_profession_id "
            ", zhuangbei_type, jiezhi_left_right_flag, zhuangbei_colour, zhuangbei_level, zhuangbei_name "
            " from hellgate.zhuangbei_name_list_copy "
            " where zhuangbei_colour=%u and bind_profession_id<>0 order by bind_profession_id"
            , (unsigned int)ZhuangbeiColour_Chuanqi);
        try
        {
            g_mysql->Query(sql);
            while(g_mysql->FetchRow() != NULL)
            {
                zb_template_basic_info info;
                
                info.zb_name_id = atoi(g_mysql->GetField("zhuangbei_name_id"));
                info.zb_group_id = atoi(g_mysql->GetField("group_id"));
                info.bind_profession_id = atoi(g_mysql->GetField("bind_profession_id"));
                info.zb_type = atoi(g_mysql->GetField("zhuangbei_type"));
                unsigned int jiezhi_flag = atoi(g_mysql->GetField("jiezhi_left_right_flag"));
                if(jiezhi_flag > 0)
                {
                    //用来区分两个戒指
                    info.zb_type += 1000;
                }
                info.zb_colour = atoi(g_mysql->GetField("zhuangbei_colour"));
                info.zb_level = atoi(g_mysql->GetField("zhuangbei_level"));
                info.zb_name = g_mysql->GetField("zhuangbei_name");

                zb_temp_vec.push_back(info);
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

    //把所有装备入pool
    for(int i=0; i<zb_temp_vec.size(); ++i)
    {
        //
        std::map<unsigned int, ZBTypeMap>::iterator prof_map_pos
            = zb_pool.prof_map.find(zb_temp_vec[i].bind_profession_id);
        if(prof_map_pos == zb_pool.prof_map.end())
        {
            ZBTypeMap type_map;
            zb_pool.prof_map.insert(make_pair(zb_temp_vec[i].bind_profession_id, type_map));

            prof_map_pos = zb_pool.prof_map.find(zb_temp_vec[i].bind_profession_id);
        }

        //
        std::map<unsigned int, ZBList>::iterator type_map_pos
            = prof_map_pos->second.type_map.find(zb_temp_vec[i].zb_type);
        if(type_map_pos == prof_map_pos->second.type_map.end())
        {
            ZBList zb_list;
            prof_map_pos->second.type_map.insert(make_pair(zb_temp_vec[i].zb_type, zb_list));

            type_map_pos = prof_map_pos->second.type_map.find(zb_temp_vec[i].zb_type);
        }

        //找到本传奇装所在的vector
        std::list<ZBChuangqiInfo>::iterator zb_list_pos = type_map_pos->second.zb_list.begin();
        //把本装备按照等级从小到大的顺序插入
        for(;zb_list_pos!=type_map_pos->second.zb_list.end(); ++zb_list_pos)
        {
            if(zb_temp_vec[i].zb_level < zb_list_pos->zb_level)
            {
                break;
            }
        }
        {
            ZBChuangqiInfo zb_info;
            zb_info.zb_name_id = zb_temp_vec[i].zb_name_id;
            zb_info.zb_group_id = zb_temp_vec[i].zb_group_id;
            zb_info.zb_level = zb_temp_vec[i].zb_level;
            zb_info.zb_name = zb_temp_vec[i].zb_name;

            type_map_pos->second.zb_list.insert(zb_list_pos, zb_info);
        }
    }

    //入库
    std::map<unsigned int, ZBTypeMap>::iterator prof_map_pos
        = zb_pool.prof_map.begin();
    for(; prof_map_pos!=zb_pool.prof_map.end(); ++prof_map_pos)
    {
        std::map<unsigned int, ZBList>::iterator type_map_pos
            = prof_map_pos->second.type_map.begin();
        for(; type_map_pos!=prof_map_pos->second.type_map.end(); ++type_map_pos)
        {
            std::list<ZBChuangqiInfo>::iterator zb_list_pos = type_map_pos->second.zb_list.begin();
            for(; zb_list_pos!=type_map_pos->second.zb_list.end(); ++zb_list_pos)
            {
                std::list<ZBChuangqiInfo>::iterator next_pos = zb_list_pos;
                next_pos++;

                unsigned int next_group_id = 0;
                if(next_pos != type_map_pos->second.zb_list.end())
                {
                    next_group_id = next_pos->zb_group_id;
                }

                //入库
                {
                    char sql[1024];
                    sprintf(sql, "insert into hellgate.shenbing_id_list "
                        "set zb_group_id=%u, zb_level=%u, next_level_group_id=%u, next_level_need_id=0"
                        , zb_list_pos->zb_group_id, zb_list_pos->zb_level, next_group_id
                        );
                    try
                    {
                        g_mysql->Query(sql);
                    }
                    catch(CMysqlException& e)
                    {
                        LogMsg("%s, line %d, mysql exception. [%s]"
                                    , __FILE__, __LINE__, e.GetErrMsg());
                        //
                        exit(1);
                    }
                }
            }
        }
    }
    
}

void update_zhuangbei_template()
{
    //
    std::vector<zb_template_basic_info> basic_vec;
    
    {
        char sql[1024];
        sprintf(sql, "select zhuangbei_name_id, group_id, bind_profession_id "
            ", zhuangbei_type, zhuangbei_colour, zhuangbei_level, zhuangbei_name "
            " from hellgate.zhuangbei_name_list_copy order by zhuangbei_type"
            );
        try
        {
            g_mysql->Query(sql);
            while(g_mysql->FetchRow() != NULL)
            {
                zb_template_basic_info info;
                
                info.zb_name_id = atoi(g_mysql->GetField("zhuangbei_name_id"));
                info.zb_group_id = atoi(g_mysql->GetField("group_id"));
                info.bind_profession_id = atoi(g_mysql->GetField("bind_profession_id"));
                info.zb_type = atoi(g_mysql->GetField("zhuangbei_type"));
                info.zb_colour = atoi(g_mysql->GetField("zhuangbei_colour"));
                info.zb_level = atoi(g_mysql->GetField("zhuangbei_level"));
                info.zb_name = g_mysql->GetField("zhuangbei_name");

                basic_vec.push_back(info);
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


    //
    std::vector<zb_template_detail> detail_vec;
    for(int i=0; i<basic_vec.size(); ++i)
    {
        zb_template_detail detail;
        detail.basic_info = basic_vec[i];

        //
        if(detail.basic_info.zb_colour == ZhuangbeiColour_Chuanqi
            || detail.basic_info.zb_colour == ZhuangbeiColour_Xiyou)
        {
            detail.is_boss_zhuangbei = true;
        }
        else
        {
            detail.is_boss_zhuangbei = false;
        }

        //攻击
        if(is_wuqi((ZhuangbeiType)detail.basic_info.zb_type))
        {
            calc_wuqi_gongji(detail.basic_info.zb_colour,detail.basic_info.zb_level
                ,detail.basic_info.zb_type
                ,detail.gongji_min_min,detail.gongji_min_max,detail.gongji_max_min,detail.gongji_max_max);
        }
        else
        {
            detail.gongji_min_min = 0;
            detail.gongji_min_max = 0;
            detail.gongji_max_min = 0;
            detail.gongji_max_max = 0;
        }

        //护甲
        if(is_fangju((ZhuangbeiType)detail.basic_info.zb_type))
        {
            calc_fangju_hujia(detail.basic_info.zb_colour,detail.basic_info.zb_level
                ,detail.basic_info.zb_type
                ,detail.wufang_min,detail.wufang_max);
        }
        else
        {
            detail.wufang_min = 0;
            detail.wufang_max = 0;
        }

        //魔法属性
        if(is_wuqi((ZhuangbeiType)detail.basic_info.zb_type))
        {
            calc_wuqi_shuxing(detail.basic_info.zb_colour,detail.basic_info.zb_level,detail.basic_info.zb_type
                , detail.basic_info.bind_profession_id
                , detail.attr_vec);
        }
        else if(detail.basic_info.zb_type==ZhuangbeiType_KuijiaQin
            || detail.basic_info.zb_type==ZhuangbeiType_KuijiaZhong)
        {
            calc_kuijia_shuxing(detail.basic_info.zb_colour,detail.basic_info.zb_level,detail.basic_info.zb_type
                , detail.attr_vec);
        }
        else if(detail.basic_info.zb_type==ZhuangbeiType_Dunpai)
        {
            calc_dunpai_shuxing(detail.basic_info.zb_colour,detail.basic_info.zb_level,detail.basic_info.zb_type
                , detail.attr_vec);
        }
        else if(detail.basic_info.zb_type==ZhuangbeiType_Faqi)
        {
            calc_faqi_shuxing(detail.basic_info.zb_colour,detail.basic_info.zb_level,detail.basic_info.zb_type
                , detail.attr_vec);
        }
        else if(detail.basic_info.zb_type==ZhuangbeiType_Hushenfu)
        {
            calc_hushenfu_shuxing(detail.basic_info.zb_colour,detail.basic_info.zb_level,detail.basic_info.zb_type
                , detail.basic_info.bind_profession_id
                , detail.attr_vec);
        }
        else if(detail.basic_info.zb_type==ZhuangbeiType_Jiezhi)
        {
            calc_jiezhi_shuxing(detail.basic_info.zb_colour,detail.basic_info.zb_level,detail.basic_info.zb_type
                , detail.basic_info.bind_profession_id
                , detail.attr_vec);
        }

        //update
        //戒指的蓝装手动配置
        if(detail.basic_info.zb_type == ZhuangbeiType_Jiezhi 
            && (detail.basic_info.zb_colour == ZhuangbeiColour_Baizhuang
                || detail.basic_info.zb_colour == ZhuangbeiColour_Fumo))
        {
            continue;
        }
        {
            std::string attr_str;
            for(int i=1; i<=10; ++i)
            {
                std::string tmp_attr;
                if(detail.attr_vec.size() >= i)
                {
                    tmp_attr = detail.attr_vec[i-1];
                }
                char buf[128];
                sprintf(buf, ", attr_%u='%s'", i, tmp_attr.c_str());

                attr_str = attr_str + buf;
            }
            char sql[1024];
            sprintf(sql, "update hellgate.zhuangbei_name_list_copy "
                " set is_boss_zhuangbei=%u, "
                " gongji_min_min=%u, gongji_min_max=%u, gongji_max_min=%u, gongji_max_max=%u, "
                " wufang_min=%u, wufang_max=%u "
                " %s "
                " where zhuangbei_name_id=%u and group_id=%u"
                , (int)detail.is_boss_zhuangbei
                , detail.gongji_min_min, detail.gongji_min_max, detail.gongji_max_min, detail.gongji_max_max
                , detail.wufang_min, detail.wufang_max
                , attr_str.c_str()
                , detail.basic_info.zb_name_id, detail.basic_info.zb_group_id);
            try
            {
                g_mysql->Query(sql);
                
            }
            catch(CMysqlException& e)
            {
                LogMsg("%s, line %d, mysql exception. [%s]"
                            , __FILE__, __LINE__, e.GetErrMsg());
                //
                exit(1);
            }

            LogMsg("%s, line %d, [%s]. updated"
                            , __FILE__, __LINE__, detail.basic_info.zb_name.c_str());
        }
    }
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

    //
    update_zhuangbei_template();
    update_chuanqi_info();

    return 0;
}

