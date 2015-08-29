
#ifndef SIMPLEPROTOCOLCMD_H
#define SIMPLEPROTOCOLCMD_H

// 取消对什么数组长度为0的警告
#pragma warning (disable:4200)

#include "cocos2d.h"
using namespace cocos2d;

#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <windows.h>
#pragma comment(lib, "wsock32.lib")
#else
#include <arpa/inet.h>
#endif
#include "LFUtile.h"

#define Current_ProtocolVersion 1


#define SimpleProtocol_Tag_Server 21335
#define SimpleProtocol_Tag_Client 21336

//整个阵型的最大人数 
#define FormationHeroAcountMax 4
// 无效Id
#define INVALID_ID 0

//iOS客户端token长度
#define iOS_ClientPushTokenLen 32

//客户端登录
#define ClientAccountMaxLen 32
#define ClientPswMaxLen     16

//一份概率材料强化成功率加成
#define QIANGHUA_GAILV_CAILIAO_ADD_RATIO 20

//体力消耗
#define Vitality_Total                   120
//翻牌消耗体力
#define Vitality_GuanQia_Floped 		 2
//打boss关消耗体力
#define Vitality_GuanQia_Boss            12

//体力价格 金币
#define Vitality_Price                   50

//英雄等级限定
#define HERO_MAX_LEVEL 30
#define HERO_MAX_LEVEL_JINGYING 30

//英雄阶数限定
#define HERO_MAX_STAGE 10

//技能最多学习到几重
#define SKILL_MAX_LEVEL 8


//元神等级限定
#define YUANSHEN_MAX_LEVEL 25

//元神等级限定
#define YUANSHEN_MAX_STAGE 5


//可以开启地狱副本的等级 
#define HERO_LEVEL_HELL_TASK_OPEN 5

//竞技场与团队boss副本中满级后的难度选择
#define ARENA_HARD_LEVEL_COUNT 5

//竞技场一个房间的人数
#define ARENA_ROOM_HERO_COUNT 4

//团队BOSS一个房间的人数
#define TEAMBOSS_ROOM_HERO_COUNT ARENA_ROOM_HERO_COUNT

//锻造装备名称
#define DUANZAO_GROUP_NAME_MAX_LEN 48

////锻造装备价格(金币)
//#define DUANZAO_ONCE_PRICE 200
//#define DUANZAO_TENTIMES_PRICE 1800

//强化与回退所要求的装备最小等级
#define Qianghua_zhuangbei_min_level 1
//萃取附魔所要求的装备最小等级
#define Cuiqu_zhuangbei_min_level 8

//回退最多keep多少次强化
#define Huitui_zhuangbei_keep_max 4



//装备精炼属性加成率
#define REFINE_EQUIP_GAIN_PER 	0.12f

//抽取一次神兵所需金币
#define DROW_SHENBING_NEED_GOLD  400


typedef unsigned int TaskId;

//主线任务id
#define MainTaskId_min 1
#define MainTaskId_max 50

//地狱副本id初始值
#define HellTaskId_Min 101
#define HellTaskId_Max 131

#define CONTENT_MAX_LEN	120		// 描述内容最大长度
#define SUMMARY_MAX_LEN	60		// 邮件提要最大长度


//装备强化级别
enum QianghuaLevel
{
    QianghuaLevel_tie    = 1,
    QianghuaLevel_tong   = 2,
    QianghuaLevel_yin    = 3,
    QianghuaLevel_jin    = 4,
    QianghuaLevel_jin_special    = 5,
};


enum GuaiwuTypeId
{
    Type_JinZhan = 1,
    Type_YuanCheng = 2,
    Type_FaShi = 3,
};


enum Profession
{
    Profession_Yemanren = 1,
    Profession_Chike = 2,
    Profession_Fashi = 3,
    Profession_Qishi = 4,
    Profession_ShenJianShou = 5,
	Profession_MiShu = 6,
	Profession_WuSeng = 7,
	Profession_YouXia = 8,
	Profession_LieShou = 9,
	Profession_WuShi = 10,

	// 下面是怪物
	Profession_GW_Start = 19, // 怪物的开始，对于配置工具这个些怪物的枚举是没有用的,所以注释掉
	// 近战
	Profession_JZ_JiangShi = 20,
	Profession_JZ_ShiRenMo = 21,
	Profession_JZ_YeRen = 22,
	Profession_JZ_KuLouBin = 23,
	Profession_JZ_KuLouZhanShi = 24,
	Profession_JZ_ShuGuai = 25,
	// 远程
	Profession_YC_KuLouGongJianShou = 40,
	Profession_YC_ShuYao = 41,
	// 法师
	Profession_FS_KuLouWuShi = 60,
	Profession_FS_YeRenSaMan = 61,
	// 后面还会有n多的职业....直到Profession_GW_End
	Profession_GW_End = 1700, // 结束的地方

	Profession_Big_Boss_1 = 1800,	// 团队boss的
};

enum ZhuangbeiType
{
    ZhuangbeiType_DanshouJian = 1,
	ZhuangbeiType_ShuangshouBishou = 2,	// ZhuangbeiType_ShuangshouMao -> 改成双手匕首
    ZhuangbeiType_ShuangshouFu = 3,
    ZhuangbeiType_DanshouFazhang = 4,
    ZhuangbeiType_Gong = 5,
    ZhuangbeiType_Faqi = 6,
    ZhuangbeiType_Dunpai = 7,
    //ZhuangbeiType_Toukui = 8,
    ZhuangbeiType_Hushenfu = 9,
    ZhuangbeiType_KuijiaZhong = 10,
    ZhuangbeiType_KuijiaQin = 11,
    //ZhuangbeiType_KuziZhong = 12,
    //ZhuangbeiType_KuziQin = 13,
    //ZhuangbeiType_Xuezi = 14,
    ZhuangbeiType_Jiezhi = 15,
    ZhuangbeiType_ShuangshouFazhang = 16,
};

enum ZhuangbeiColour
{
    ZhuangbeiColour_Baizhuang = 1,
    ZhuangbeiColour_Fumo = 2,
    ZhuangbeiColour_Xiyou = 3,
    ZhuangbeiColour_Chuanqi = 4,

    ZhuangbeiColour_BaiZhuang_Fumo = 10, //数据库中装备模板用
};

enum AttrType
{
    AttrType_mingjie     = 1,
    AttrType_liliang     = 2,
    AttrType_zhili       = 3,
    AttrType_tineng      = 4,
    AttrType_gongji_min  = 5,
    AttrType_gongji_max  = 6,
    AttrType_baoji       = 7,
    AttrType_baoshang    = 8,
    //AttrType_skillratio  = 9,
    AttrType_hujia_add   = 10,
    AttrType_hp_percent  = 11,
    AttrType_kangxing    = 12,
    AttrType_gedanglv    = 13,
    AttrType_gedangzhi   = 14,
    AttrType_skillzhouqi = 15,	// 技能周期
	AttrType_skillturn   = 16,	// 技能触发点
	AttrType_skillresult = 17,	// 技能效果加成
	AttrType_actor_gongji_addratio = 18,	// 角色整体攻击加成
	AttrType_actor_hp_addratio = 19,	// 角色整体血量加成
};

//0标示还未激活，1表示已激活但还未领取，2表示已领取，3标示已完成但未提交，4表示已提交。
enum TaskStatus
{
    TaskStatus_NotActived = 0,
    TaskStatus_NotGot = 1,
    TaskStatus_Got = 2,
    TaskStatus_Finished = 3,
    TaskStatus_Submitted = 4,
};


enum GuanQiaType
{
    GuanQiaType_Normal   = 0,
    GuanQiaType_JingYing = 1,
    GuanQiaType_BOSS     = 2,
};

enum SkillType
{
	SkillType_BaoLieZhan         = 1,   // 爆裂斩
	SkillType_ShaLu              = 2,			// 杀戮
	SkillType_ShanDian           = 3,			// 闪电
	SkillType_JianShou           = 4,			// 坚守
	SkillType_ShenZhiXingNian    = 5,	// 神之信念
	SkillType_ShunYinTuXi        = 6,		// 瞬影突袭
	SkillType_LieGong            = 7,			// 烈弓
	SkillType_JianYu             = 8,			// 剑雨
	SkillType_HuoQiuShu          = 9,		// 火球术
	SkillType_WenYi              = 10,			// 瘟疫

	SkillType_BanYueZhan         = 11,
	SkillType_CiSha              = 12,
	SkillType_LeiBao             = 13,
	SkillType_MangCi             = 14,
	SkillType_ShenZhiZhuFu       = 15,
	SkillType_LiRenHuiXuan       = 16,
	SkillType_JingZhunZhiCan     = 17,
	SkillType_JiSuDuJian         = 18,
	SkillType_BaoFengXue         = 19,
	SkillType_KongJu             = 20,

	SkillType_KuangBao           = 21,
	SkillType_ShiXueZhiRen       = 22,
	SkillType_YunShi             = 23,
	SkillType_ShouHu             = 24,
	SkillType_ZhongShenZhiLi     = 25,
	SkillType_XuanFengZhan       = 26,
	SkillType_ZhuiHunDuJian      = 27,
	SkillType_SiWangSaoShe       = 28,
	SkillType_HanBingCiGu        = 29,
	SkillType_ShiHun             = 30,

	SkillType_BaoNu              = 31,
	SkillType_YinZheHun          = 32,
	SkillType_TianTangZhiGuang   = 33,

    //以下是怪物技能
    SkillType_Monster_XieE       = 101,
    
    //BOSS技能
    SkillType_BOSS_Sweep       	 = 201,		//横扫 
    SkillType_BOSS_Flog       	 = 202,		//鞭笞 
};


//除装备外的物品类型
enum ItemsType
{
    ItemsType_Gold = 1,
    ItemsType_Silver = 2,
    
    ItemsType_QianghuaNormal = 5,
    ItemsType_QianghuaXiyou = 6,
    ItemsType_QianghuaChuangqi = 7,
    ItemsType_QianghuaGailvTianjia = 8,

//10~100为宝石类型区间
    ItemsType_Baoshi_min            = 10,
    
    ItemsType_Baoshi_lv_1 = 11,
    ItemsType_Baoshi_lv_2 = 12,
    ItemsType_Baoshi_lv_3 = 13,
    ItemsType_Baoshi_lv_4 = 14,
    ItemsType_Baoshi_lv_5 = 15,
    ItemsType_Baoshi_lv_6 = 16,
    ItemsType_Baoshi_lv_7 = 17,
    //ItemsType_Baoshi_lv_8 = 18,
    //ItemsType_Baoshi_lv_9 = 19,
    //ItemsType_Baoshi_lv_10 = 20,

    ItemsType_Baoshi_hong_1 = 31,
    ItemsType_Baoshi_hong_2 = 32,
    ItemsType_Baoshi_hong_3 = 33,
    ItemsType_Baoshi_hong_4 = 34,
    ItemsType_Baoshi_hong_5 = 35,
    ItemsType_Baoshi_hong_6 = 36,
    ItemsType_Baoshi_hong_7 = 37,
    //ItemsType_Baoshi_hong_8 = 38,
    //ItemsType_Baoshi_hong_9 = 39,
    //ItemsType_Baoshi_hong_10 = 40,

    //ItemsType_Baoshi_zhi_1 = 51,
    //ItemsType_Baoshi_zhi_2 = 52,
    //ItemsType_Baoshi_zhi_3 = 53,
    //ItemsType_Baoshi_zhi_4 = 54,
    //ItemsType_Baoshi_zhi_5 = 55,
    //ItemsType_Baoshi_zhi_6 = 56,
    //ItemsType_Baoshi_zhi_7 = 57,
    //ItemsType_Baoshi_zhi_8 = 58,
    //ItemsType_Baoshi_zhi_9 = 59,
    //ItemsType_Baoshi_zhi_10 = 60,

    ItemsType_Baoshi_huang_1 = 71,
    ItemsType_Baoshi_huang_2 = 72,
    ItemsType_Baoshi_huang_3 = 73,
    ItemsType_Baoshi_huang_4 = 74,
    ItemsType_Baoshi_huang_5 = 75,
    ItemsType_Baoshi_huang_6 = 76,
    ItemsType_Baoshi_huang_7 = 77,
    //ItemsType_Baoshi_huang_8 = 78,
    //ItemsType_Baoshi_huang_9 = 79,
    //ItemsType_Baoshi_huang_10 = 80,

    ItemsType_Baoshi_max            = 100,
//10~100为宝石类型区间

    ItemsType_Blood        = 202,  //血瓶
    ItemsType_HellStone     = 203,  //武器锻造材料，地狱熔岩

    //指定获得一颗金星的强化材料
    ItemsType_GoldStar_Qianghua   = 204,
    //神兵解封印道具
    ItemsType_Shenbing_Jiefengyin = 205,
    //神兵鉴定
    ItemsType_Shenbing_Jianding   = 206,
    //大理石，花岗岩，木材，水晶石
    ItemsType_Dalishi             = 207,
    ItemsType_Huagangyan          = 208,
    ItemsType_Mucai               = 209,
    ItemsType_Shuijingshi         = 210,
    //角色升级所需道具
    ItemsType_Hero_LevelUp        = 211,
    ItemsType_Hero_Upgrade        = 212,
    //经文篆刻材料
    ItemsType_JingwenZhuanke      = 213,

//寻魔率道具
    ItemsType_XunMoLv_1       = 301,
    ItemsType_XunMoLv_2       = 302,
    ItemsType_XunMoLv_3       = 303,
    ItemsType_XunMoLv_4       = 304,
    ItemsType_XunMoLv_5       = 305,
    ItemsType_XunMoLv_6       = 306,
    ItemsType_XunMoLv_7       = 307,
    ItemsType_XunMoLv_8       = 308,
    ItemsType_XunMoLv_9       = 309,
//

    //技能书
    ItemsType_SkillBook_MinId              = 400,
    
    ItemsType_SkillBook_BaoLieZhan         = ItemsType_SkillBook_MinId+SkillType_BaoLieZhan      ,   // 爆裂斩
	ItemsType_SkillBook_ShaLu              = ItemsType_SkillBook_MinId+SkillType_ShaLu           ,			// 杀戮
	ItemsType_SkillBook_ShanDian           = ItemsType_SkillBook_MinId+SkillType_ShanDian        ,			// 闪电
	ItemsType_SkillBook_JianShou           = ItemsType_SkillBook_MinId+SkillType_JianShou        ,			// 坚守
	ItemsType_SkillBook_ShenZhiXingNian    = ItemsType_SkillBook_MinId+SkillType_ShenZhiXingNian ,	// 神之信念
	ItemsType_SkillBook_ShunYinTuXi        = ItemsType_SkillBook_MinId+SkillType_ShunYinTuXi     ,		// 瞬影突袭
	ItemsType_SkillBook_LieGong            = ItemsType_SkillBook_MinId+SkillType_LieGong         ,			// 烈弓
	ItemsType_SkillBook_JianYu             = ItemsType_SkillBook_MinId+SkillType_JianYu          ,			// 剑雨
	ItemsType_SkillBook_HuoQiuShu          = ItemsType_SkillBook_MinId+SkillType_HuoQiuShu       ,		// 火球术
	ItemsType_SkillBook_WenYi              = ItemsType_SkillBook_MinId+SkillType_WenYi           ,			// 瘟疫

	ItemsType_SkillBook_BanYueZhan         = ItemsType_SkillBook_MinId+SkillType_BanYueZhan      ,
	ItemsType_SkillBook_CiSha              = ItemsType_SkillBook_MinId+SkillType_CiSha           ,
	ItemsType_SkillBook_LeiBao             = ItemsType_SkillBook_MinId+SkillType_LeiBao          ,
	ItemsType_SkillBook_MangCi             = ItemsType_SkillBook_MinId+SkillType_MangCi          ,
	ItemsType_SkillBook_ShenZhiZhuFu       = ItemsType_SkillBook_MinId+SkillType_ShenZhiZhuFu    ,
	ItemsType_SkillBook_LiRenHuiXuan       = ItemsType_SkillBook_MinId+SkillType_LiRenHuiXuan    ,
	ItemsType_SkillBook_JingZhunZhiCan     = ItemsType_SkillBook_MinId+SkillType_JingZhunZhiCan  ,
	ItemsType_SkillBook_JiSuDuJian         = ItemsType_SkillBook_MinId+SkillType_JiSuDuJian      ,
	ItemsType_SkillBook_BaoFengXue         = ItemsType_SkillBook_MinId+SkillType_BaoFengXue      ,
	ItemsType_SkillBook_KongJu             = ItemsType_SkillBook_MinId+SkillType_KongJu          ,

	ItemsType_SkillBook_KuangBao           = ItemsType_SkillBook_MinId+SkillType_KuangBao        ,
	ItemsType_SkillBook_ShiXueZhiRen       = ItemsType_SkillBook_MinId+SkillType_ShiXueZhiRen    ,
	ItemsType_SkillBook_YunShi             = ItemsType_SkillBook_MinId+SkillType_YunShi          ,
	ItemsType_SkillBook_ShouHu             = ItemsType_SkillBook_MinId+SkillType_ShouHu          ,
	ItemsType_SkillBook_ZhongShenZhiLi     = ItemsType_SkillBook_MinId+SkillType_ZhongShenZhiLi  ,
	ItemsType_SkillBook_XuanFengZhan       = ItemsType_SkillBook_MinId+SkillType_XuanFengZhan    ,
	ItemsType_SkillBook_ZhuiHunDuJian      = ItemsType_SkillBook_MinId+SkillType_ZhuiHunDuJian   ,
	ItemsType_SkillBook_SiWangSaoShe       = ItemsType_SkillBook_MinId+SkillType_SiWangSaoShe    ,
	ItemsType_SkillBook_HanBingCiGu        = ItemsType_SkillBook_MinId+SkillType_HanBingCiGu     ,
	ItemsType_SkillBook_ShiHun             = ItemsType_SkillBook_MinId+SkillType_ShiHun          ,

	ItemsType_SkillBook_BaoNu              = ItemsType_SkillBook_MinId+SkillType_BaoNu           ,
	ItemsType_SkillBook_YinZheHun          = ItemsType_SkillBook_MinId+SkillType_YinZheHun       ,
	ItemsType_SkillBook_TianTangZhiGuang   = ItemsType_SkillBook_MinId+SkillType_TianTangZhiGuang,

    ItemsType_SkillBook_MaxId              = 434,

	//杂货铺代购劵
	ItemsType_DaiGouJuan				   = 501,
	//赠送体力道具
	ItemsType_PresentVitality			   = 502,
	//赠送VIP1道具
	ItemsType_PresentVIP1			   	   = 503,
	//赠送稀有装备道具
	ItemsType_PresentEquip_Xiyou	       = 504,
	//赠送传奇装备道具
	ItemsType_PresentEquip_Legend	       = 505,
    
	//洗炼卷轴
	ItemsType_Purify_Scroll	   			   = 511,
	
	//工匠铁锤
	ItemsType_Hammer					   = 520,
};





enum ZhuangbeiType_Filter
{
    ZBT_Filter_DanshouJian          = 0x00001,
	ZBT_Filter_ShuangShouBiShou     = 0x00002,
    ZBT_Filter_ShuangshouFu         = 0x00004,
    ZBT_Filter_DanshouFazhang       = 0x00008,
    ZBT_Filter_Gong                 = 0x00010,
    ZBT_Filter_Faqi                 = 0x00020,
    ZBT_Filter_Toukui               = 0x00040,
    ZBT_Filter_Hushenfu             = 0x00080,
    ZBT_Filter_KuijiaZhong          = 0x00100,
    ZBT_Filter_KuijiaQin            = 0x00200,
    ZBT_Filter_KuziZhong            = 0x00400,
    ZBT_Filter_KuziQin              = 0x00800,
    ZBT_Filter_Xuezi                = 0x01000,
    ZBT_Filter_Jiezhi               = 0x02000,
    ZBT_Filter_ShuangshouFazhang    = 0x04000,
    ZBT_Filter_Dunpai               = 0x08000,


    // 全部
    ZBT_Filter_ALL                  = 0x0ffff,
    ZBT_Filter_NONE                 = 0x00000,

};



//
#define HeroNameMaxLen 18
#define ZhuangbeiNameMaxLen 24
#define ZhuangbeiPicMaxLen 12
//#define ZhuangbeiDescMaxLen 96
#define SystemGiftDesc_MaxLen 48


enum SimpleProtocolErrorCode
{
    Success             = 0,
    Error               = 1,
    NotLogin            = 2,
    AccountOrPswErr     = 3,
    AccountAlreadyExist = 4,
    TiliNotEnought      = 5,
};

enum SimpleProtocolCmd
{
    clientLogin              = 1,
    clientLoginRsp           = 2,
    createHero               = 3,
    createHeroRsp            = 4,
    createAssit              = 5,
    createAssitRsp           = 6,
    getHerosInTeam           = 7,
    getHerosInTeamRsp        = 8,
    getZhuangbeiInTeam       = 9,
    getZhuangbeiInTeamRsp    = 10,
    equipZhuangbei           = 11,
    equipZhuangbeiRsp        = 12,
    adjustFormation          = 13,
    adjustFormationRsp       = 14,
    getTaskList              = 15,
    getTaskListRsp           = 16,
    getItemsInTeam           = 17,
    getItemsInTeamRsp        = 18,
    ZBQianghua               = 19,
    ZBQianghuaRsp            = 20,
    ZBQianghuaHuitui         = 21,
    ZBQianghuaHuituiRsp      = 22,
    ZBChaijie                = 23,
    ZBChaijieRsp             = 24,
    ZBXiangBaoshi            = 25,
    ZBXiangBaoshiRsp         = 26,
    ZBChaiBaoshi             = 27,
    ZBChaiBaoshiRsp          = 28,
    BaoshiHecheng            = 29,
    BaoshiHechengRsp         = 30,
    Battle                   = 31,
    BattleRsp                = 32,
    Register                 = 37,
    RegisterRsp              = 38,
    Reconnect                = 39,
    ReconnectRsp             = 40,
    GetTaskChessboard        = 41,
    GetTaskChessboardRsp     = 42,
    changeTaskStat           = 43,
    changeTaskStatRsp        = 44,
    flopTaskGrid             = 45,
    flopTaskGridRsp          = 46,
    equipTeamItem            = 47,
    equipTeamItemRsp         = 48,
    beKickOutRsp             = 50,
    heartBeat                = 51,
    heartBeatRsp             = 52,

    //查看别人的团队信息
    queryAnotherTeamInfo     = 53,
    queryAnotherTeamInfoRsp  = 54,

    //充满体力
    chargeVitality           = 55,
    chargeVitalityRsp        = 56,

    //获取所有套装信息
    queryAllSuitName         = 57,
    queryAllSuitNameRsp      = 58,

    //萃取附魔
    ZBCuiqu                  = 59,
    ZBCuiquRsp               = 60,
    ZBFumo                   = 61,
    ZBFumoRsp                = 62,

    //查询自己拥有的魔法精华
    queryAllMofaJinghua      = 63,
    queryAllMofaJinghuaRsp   = 64,

    //查询副本boss掉落
    queryFubenDiaoluo        = 65,
    queryFubenDiaoluoRsp     = 66,

    //新助手加入，服务器主动通知
    newAssistantJoinRsp      = 68,

    //查询英雄当前分支任务列表
    querySubTaskList         = 69,
    querySubTaskListRsp      = 70,

    //获得新任务
    newSubTaskGotRsp         = 72,
    //服务器通知任务已提交
    subTaskSubmitedRsp       = 74,

    //查询主线任务奖励
    queryMainTaskReward      = 75,
    queryMainTaskRewardRsp   = 76,

    //充值金子，测试用
    chargeGold               = 77,
    chargeGoldRsp            = 78,

    //查询团队所拥有的技能
    queryTeamSkillList       = 79,
    queryTeamSkillListRsp    = 80,
    //跟某个角色装配技能
    equipSkill               = 81,
    equipSkillRsp            = 82,
    //使用得到的技能书给某个角色学习
    actorLearnSkill          = 83,
    actorLearnSkillRsp       = 84,

    //神兵碎片
    queryTeamShenbingSuipian = 85,
    queryTeamShenbingSuipianRsp = 86,
    shenbingHecheng          = 87,
    shenbingHechengRsp          = 88,

    //道具碎片
    queryTeamItemSuipian     = 89,
    queryTeamItemSuipianRsp  = 90,
    itemHecheng              = 91,
    itemHechengRsp           = 92,

    //查询所有神兵列表
    queryAllShenbingList     = 93,
    queryAllShenbingListRsp  = 94,

    //扫荡副本
    saodangFuben             = 95,
    saodangFubenRsp          = 96,

    //升级
    heroLevelUp              = 97,
    heroLevelUpRsp           = 98,

    //升阶
    heroUpgrade              = 99,
    heroUpgradeRsp           = 100,

    //神兵解封印
    shenbingJieFengyin       = 101,
    shenbingJieFengyinRsp    = 102,

    //神兵鉴定
    shenbingJianding         = 103,
    shenbingJiandingRsp      = 104,

    //精炼
    zbJinglian               = 105,
    zbJinglianRsp            = 106,

    //神兵篆刻经文
    shenbingZhuankeJingwen   = 107,
    shenbingZhuankeJingwenRsp = 108,

    //查询某个角色的元神
    queryYuanshenOfHero      = 109,
    queryYuanshenOfHeroRsp   = 110,

    //教堂
    churchLevelUp            = 111,
    churchLevelUpRsp         = 112,

    //技能升级
    skillLevelUp             = 113,
    skillLevelUpRsp          = 114,

    //获取装备图鉴
    getZhuangbeiAlbum        = 115,
    getZhuangbeiAlbumRsp     = 116,

    //获取技能图鉴
    getSkillAlbum            = 117,
    getSkillAlbumRsp         = 118,

    //竞技场
    arenaEnter               = 161,
    arenaEnterRsp            = 162,
    arenaUpdateRoomRsp       = 164,
    arenaGetBattleScriptAndGiftRsp  = 166,
    arenaGetGift             = 167,
    arenaGetGiftRsp          = 168,

    //团队副本
    teamBossEnter            = 181,
    teamBossEnterRsp         = 182,
    teamBossUpdateRoomRsp    = 184,
    teamBossGetBattleScriptAndGiftRsp  = 186,
    teamBossGetGift          = 187,
    teamBossGetGiftRsp       = 188,

    //守城(废弃)
    cityGuardEnter           = 201,
    cityGuardEnterRsp        = 202,
    cityGuardGetNextMonster  = 203,
    cityGuardGetNextMonsterRsp  = 204,
    cityGuardFight           = 205,
    cityGuardFightRsp        = 206,

    //英雄榜
    topHeroGetList           = 221,
    topHeroGetListRsp        = 222,
    topHeroChallenge         = 223,
    topHeroChallengeRsp      = 224,
    topHeroClearCDTime       = 225,
    topHeroClearCDTimeRsp    = 226,

    //巅峰对决
    topArenaQueryStatus      = 241,
    topArenaQueryStatusRsp   = 242,
    topArenaGuess            = 243,
    topArenaGuessRsp         = 244,
    topArenaReplay         	 = 245,
    topArenaReplayRsp        = 246,
    topArenaQueryGuess     	 = 247,
    topArenaQueryGuessRsp    = 248,

    //系统奖励
    systemGiftQuery          = 261,
    systemGiftQueryRsp       = 262,
    systemGiftGetGift        = 263,
    systemGiftGetGiftRsp     = 264,

    //租赁场
    rentGetMyZB              = 281,
    rentGetMyZBRsp           = 282,
    rentAddMyZB              = 283,
    rentAddMyZBRsp           = 284,
    rentCancelMyZB           = 285,
    rentCancelMyZBRsp        = 286,
    rentRecommend            = 287,
    rentRecommendRsp         = 288,
    rentQuery                = 289,
    rentQueryRsp             = 290,
    rentOwnerChangePrice     = 293,
    rentOwnerChangePriceRsp  = 294,

    //装备锻造
    duanzaoListQuery         = 311,
    duanzaoListQueryRsp      = 312,
    duanzao                  = 313,
    duanzaoRsp               = 314,
    duanzaoGroupZBQuery      = 315,
    duanzaoGroupZBQueryRsp   = 316,

    //竞技场挑战
    queryArenaTeamBasicInfo      = 341,
    queryArenaTeamBasicInfoRsp   = 342,
    queryArenaMarket         = 343,
    queryArenaMarketRsp      = 344,
    refreshArenaChlgList     = 345,
    refreshArenaChlgListRsp  = 346,
    queryArenaChlgList       = 347,
    queryArenaChlgListRsp    = 348,
    arenaChallenge           = 349,
    arenaChallengeRsp        = 350,
    arenaMarketBuy           = 351,
    arenaMarketBuyRsp        = 352,
    arenaBuyChlgTime         = 353,
    arenaBuyChlgTimeRsp      = 354,

    //杂货铺
    queryZahuopu             = 371,
    queryZahuopuRsp          = 372,
    refreshZahuopu           = 373,
    refreshZahuopuRsp        = 374,
    zahuopuGetOneItem        = 375,
    zahuopuGetOneItemRsp     = 376,

    //地狱副本
    queryHellTaskHeroInfo    = 391,
    queryHellTaskHeroInfoRsp = 392,
    hellTaskBattle           = 393,
    hellTaskBattleRsp        = 394,

    //守城(世界boss)
    cityGuardProcessNotify      = 396,
    getCityGuardCurrentStat  	= 397,
    getCityGuardCurrentStatRsp 	= 398,
    cityGuardBattle          	= 399,
    cityGuardBattleRsp       	= 400,
    quitCityGuard   		 	= 401,
    quitCityGuardRsp 			= 402,
    getCityGuardLastDayReport 	= 403,
    getCityGuardLastDayReportRsp	= 404,
    getMyCityGuardReward     	= 405,
    getMyCityGuardRewardRsp  	= 406,
		
	//奖励
	queryBonus 				= 411,
	queryBonusRsp 			= 412,
	getBonus	 			= 413,
	getBonusRsp				= 414,
	sendBonusNotify 		= 415,

	//获取棋盘宝箱
	//getChessBox  			= 431,
	//getChessBoxRsp			= 432,
	
	//关卡章节评级
	queryRateTask  			= 441,
	queryRateTaskRsp		= 442,
	queryRateTaskBonus		= 443,
	queryRateTaskBonusRsp	= 444,
	getRateTaskBonus		= 445,
	getRateTaskBonusRsp		= 446,

    //查询vip特权
	queryVipPrivilege       = 451,
	queryVipPrivilegeRsp    = 452,
	
    //升级神兵
	upgradeShenbing       	= 461,
	upgradeShenbingRsp    	= 462,

	//查询充值信息
    queryChargePacket       = 471,
    queryChargePacketRsp    = 482,
	
	//解锁助手
    unlockAssistant       	= 491,
    unlockAssistantRsp    	= 492,

	//体力赠送活动
	queryGiftActivity		= 501,
	queryGiftActivityRsp	= 502,
	getGiftActivity			= 503,
	getGiftActivityRsp		= 504,
	
	//击杀怪物累计
	queryKilledMonster		= 511,
	queryKilledMonsterRsp	= 512,
	getKilledMonsterBonus	= 513,
	getKilledMonsterBonusRsp	= 514,
	queryKilledMonsterBonus	= 515,
	queryKilledMonsterBonusRsp	= 516,

	//铸银
	castSilver				= 521,
	castSilverRsp			= 522,

	//传奇录
	queryLegendNote			= 531,
	queryLegendNoteRsp		= 532,
	challengeLegendNote 	= 533,
	challengeLegendNoteRsp 	= 534,

	//血色教堂
	querySelfChallengeInfo		= 541,
	querySelfChallengeInfoRsp	= 542,
	querySelfChallengeOrder 	= 543,
	querySelfChallengeOrderRsp 	= 544,
	selfChallenge				= 545,
	selfChallengeRsp			= 546,
	selfChallengeGain			= 547,
	selfChallengeGainRsp		= 548,
	getSelfChallengeBox 		= 549,
	getSelfChallengeBoxRsp 		= 550,

	//守护之灵
	queryProtectSpirit			= 561,
	queryProtectSpiritRsp		= 562,
	challengeProtectSpirit		= 563,
	challengeProtectSpiritRsp	= 564,
	getProtectSpiritBonus		= 565,
	getProtectSpiritBonusRsp	= 566,
	worshiProtectSpirit			= 567,
	worshiProtectSpiritRsp		= 568,
	
	//探索
	queryDiscoverInfo		= 581,
	queryDiscoverInfoRsp	= 582,
	discover				= 583,
	discoverRsp				= 584,
	unlockDiscoverSeal		= 585,
	unlockDiscoverSealRsp	= 586,
	
	//邮件
	queryEmail				= 591,
	queryEmailRsp			= 592,
	readEmail				= 593,
	readEmailRsp			= 594,
	writeEmail				= 595,
	writeEmailRsp			= 596,
	deleteEmail				= 597,
	deleteEmailRsp			= 598,
	pushEmail				= 599,
	
	//礼包
    queryGiftBag			= 611,
    queryGiftBagRsp			= 612,
    getGiftBag				= 613,
    getGiftBagRsp			= 614,

    //鉴定装备
    queryAssessWeapon    	= 621,
    queryAssessWeaponRsp	= 622,
    assessWeapon       		= 623,
    assessWeaponRsp    		= 624,

	//洗炼
    queryPurify				= 631,
    queryPurifyRsp			= 632,
    purify					= 633,
    purifyRsp				= 634,
    confirmPurify			= 635,
    confirmPurifyRsp		= 636,
	
	//装备产出
    queryOrigin				= 641,
    queryOriginRsp			= 642,
    
	//玩家其他信息
    queryHeroOtherInfo		= 651,
    queryHeroOtherInfoRsp	= 652,
    
	//公告
    queryNotice				= 661,
    queryNoticeRsp			= 662,

	//商城
    queryMarket            	= 671,
    queryMarketRsp        	= 672,
    buyItemInMarket         = 673,
    buyItemInMarketRsp      = 674,

	//教学
    queryInstruction        = 681,
    queryInstructionRsp     = 682,
    completeInstruction     = 683,
    completeInstructionRsp  = 684,
};


inline bool _isWuqi(ZhuangbeiType type)
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

inline bool _isFangju(ZhuangbeiType type)
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

inline bool _isShipin(ZhuangbeiType type)
{
    if(type == ZhuangbeiType_Hushenfu
        || type == ZhuangbeiType_Jiezhi
        || type == ZhuangbeiType_Faqi)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#pragma pack(1) //按照单字节对齐

//包头
struct SPHead
{
    unsigned short protocol_tag;
    unsigned short protocol_ver;
    unsigned short cmd;
    unsigned char not_use1;
    unsigned char not_use2;
    unsigned int data_len;
};
inline void hton_SPHead(SPHead* head)
{
    head->protocol_tag = htons(head->protocol_tag);
    head->protocol_ver = htons(head->protocol_ver);
    head->cmd = htons(head->cmd);
    head->data_len = htonl(head->data_len);
}
inline void ntoh_SPHead(SPHead* head)
{
    head->protocol_tag = ntohs(head->protocol_tag);
    head->protocol_ver = ntohs(head->protocol_ver);
    head->cmd = ntohs(head->cmd);
    head->data_len = ntohl(head->data_len);
}

//
struct SPCmd_Register
{
    char client_account[ClientAccountMaxLen+1];
    char client_psw[ClientPswMaxLen+1];
};

struct SPCmd_RegisterRsp
{
    int err_code;
    
    //服务器当前时间
    unsigned int server_time;
};

//
struct SPCmd_ClientLogin
{
    char client_account[ClientAccountMaxLen+1];
    char client_psw[ClientPswMaxLen+1];
};

struct HeroInfo
{
    unsigned int hero_id;

    bool is_yuanshen;
    unsigned int zhenshen_id; //如果是元神，则该字段为其真身的hero_id，否则为无效字段。
	
    unsigned int vip_level;
    unsigned int profession_id;
    unsigned int level; //英雄等级
	unsigned int stage; //英雄阶
    char name[HeroNameMaxLen+1];

    HeroInfo()
    {
        is_yuanshen = false;
        zhenshen_id = 0;
    };
};
inline void hton_hero_info(HeroInfo& hero_info_in, HeroInfo& hero_info_out)
{
    hero_info_out.hero_id = htonl(hero_info_in.hero_id);
    hero_info_out.is_yuanshen = (hero_info_in.is_yuanshen);
    hero_info_out.zhenshen_id = htonl(hero_info_in.zhenshen_id );
    hero_info_out.vip_level = htonl(hero_info_in.vip_level);
    hero_info_out.profession_id = htonl(hero_info_in.profession_id);
    hero_info_out.level = htonl(hero_info_in.level);
    hero_info_out.stage = htonl(hero_info_in.stage);

    char name[HeroNameMaxLen+1];
    strncpy(name, hero_info_in.name, HeroNameMaxLen);
    name[HeroNameMaxLen]= 0;
    strcpy(hero_info_out.name, name);
};

//
struct HeroAttrWithoutZhuangbei
{
    //attr
    unsigned int mingjie;
    unsigned int liliang;
    unsigned int zhili;
    unsigned int tineng;
    
    unsigned int xueliang;
    unsigned int baoji;
    unsigned int baoshang;
    unsigned int jinengchufa;
};

struct HeroDetail
{
    HeroInfo basic_info;

    unsigned int parent_hero_id;

    //阵型站位，0表示未上阵。
    unsigned int postion;	
	bool is_unlocked;	//英雄是否解锁
	bool unlockAble;	//是否可以解锁了

    //
    unsigned int exp_current;
    unsigned int exp_need;

    //体力
    unsigned int vit_current;

    HeroAttrWithoutZhuangbei hero_attr_without_zhuangbei;
};
inline void hton_hero_detail(HeroDetail& hero_detail_in, HeroDetail& hero_detail_out)
{
    hton_hero_info(hero_detail_in.basic_info, hero_detail_out.basic_info);
    
    hero_detail_out.parent_hero_id = htonl(hero_detail_in.parent_hero_id);
    hero_detail_out.postion = htonl(hero_detail_in.postion);

	hero_detail_out.is_unlocked = hero_detail_in.is_unlocked;
	hero_detail_out.unlockAble = hero_detail_in.unlockAble;

    hero_detail_out.exp_current = htonl(hero_detail_in.exp_current);
    hero_detail_out.exp_need = htonl(hero_detail_in.exp_need);

    hero_detail_out.vit_current = htonl(hero_detail_in.vit_current);

    hero_detail_out.hero_attr_without_zhuangbei.mingjie 
        = htonl(hero_detail_in.hero_attr_without_zhuangbei.mingjie);
    hero_detail_out.hero_attr_without_zhuangbei.liliang
        = htonl(hero_detail_in.hero_attr_without_zhuangbei.liliang);
    hero_detail_out.hero_attr_without_zhuangbei.zhili 
        = htonl(hero_detail_in.hero_attr_without_zhuangbei.zhili);
    hero_detail_out.hero_attr_without_zhuangbei.tineng
        = htonl(hero_detail_in.hero_attr_without_zhuangbei.tineng);
    hero_detail_out.hero_attr_without_zhuangbei.xueliang
        = htonl(hero_detail_in.hero_attr_without_zhuangbei.xueliang);
    hero_detail_out.hero_attr_without_zhuangbei.baoji
        = htonl(hero_detail_in.hero_attr_without_zhuangbei.baoji);
    hero_detail_out.hero_attr_without_zhuangbei.baoshang
        = htonl(hero_detail_in.hero_attr_without_zhuangbei.baoshang);
    hero_detail_out.hero_attr_without_zhuangbei.jinengchufa
        = htonl(hero_detail_in.hero_attr_without_zhuangbei.jinengchufa);
}
inline void ntoh_hero_detail(HeroDetail& hero_detail_in, HeroDetail& hero_detail_out)
{
    hton_hero_detail(hero_detail_in, hero_detail_out);
}


struct SPCmd_ClientLoginRsp
{
    int err_code;

    //服务器当前时间
    unsigned int server_time;
    
	//英雄列表
    unsigned int hero_count;
    char hero_data[0]; //HeroInfo数组
};


//创建主英雄
struct SPCmd_CreateHero
{
    unsigned int profession_id;
    char name[HeroNameMaxLen+1];
};

struct SPCmd_CreateHeroRsp
{
    int err_code;

    unsigned int hero_id;
};

//创建助手
struct SPCmd_CreateAssit
{
    unsigned int profession_id;
};

struct SPCmd_CreateAssitRsp
{
    int err_code;

    HeroDetail hero_detail;
};

//获取团队所有英雄
struct SPCmd_GetHerosInTeam
{
    //主英雄id
    unsigned hero_id;
};

struct SPCmd_GetHerosInTeamRsp
{
    int err_code;

    //教堂等级
    unsigned int church_level;

    unsigned int hero_count;
    //variable
    HeroDetail hero_detail_arr[0];
};

// 装备的概要数据4+4+13 = 21B
struct ZhuangbeiSummary
{
	unsigned int zhuangbei_type;
	unsigned int zhuangbei_colour;
	char zhuangbei_pic[ZhuangbeiPicMaxLen+1];
};

struct ZBAttr
{
    unsigned int attr_type;
    unsigned int attr_value;
    unsigned int cuiqu_suc_ratio;
};
struct ZhuangbeiCommonAttr
{
    unsigned int attr_count;
    //变长
	//char attr_arr[1];// ZBAttr的数组 ZBAttr attr_arr[0];
	ZBAttr attr_arr[0];
};

inline void hton_zhuangbei_common_attr(ZhuangbeiCommonAttr& attr_in, ZhuangbeiCommonAttr& attr_out)
{
    attr_out.attr_count = htonl(attr_in.attr_count);

    for(unsigned int i=0; i<attr_in.attr_count; ++i)
    {
        ((ZBAttr*)attr_out.attr_arr)[i].attr_type = htonl( ((ZBAttr*)attr_in.attr_arr)[i].attr_type);
        ((ZBAttr*)attr_out.attr_arr)[i].attr_value = htonl(((ZBAttr*)attr_in.attr_arr)[i].attr_value);
        ((ZBAttr*)attr_out.attr_arr)[i].cuiqu_suc_ratio = htonl(((ZBAttr*)attr_in.attr_arr)[i].cuiqu_suc_ratio);
    }
}
inline void ntoh_zhuangbei_common_attr(ZhuangbeiCommonAttr& attr_in, ZhuangbeiCommonAttr& attr_out)
{
    attr_out.attr_count = ntohl(attr_in.attr_count);

    for(unsigned int i=0; i<attr_out.attr_count; ++i)
    {
        ((ZBAttr*)attr_out.attr_arr)[i].attr_type = ntohl( ((ZBAttr*)attr_in.attr_arr)[i].attr_type);
        ((ZBAttr*)attr_out.attr_arr)[i].attr_value = ntohl(((ZBAttr*)attr_in.attr_arr)[i].attr_value);
        ((ZBAttr*)attr_out.attr_arr)[i].cuiqu_suc_ratio = ntohl(((ZBAttr*)attr_in.attr_arr)[i].cuiqu_suc_ratio);
    }
}
struct QianghuaInfo
{
    unsigned char id;
    unsigned char level;
    unsigned char percent;
};
struct QianghuaData
{
    unsigned char max_qianghua_time; //该装备最多强化多少次
    
    unsigned char qianghua_count;
    QianghuaInfo info_arr[9];
};
struct JinglianData
{
    unsigned int jinglian_times;
    ZBAttr jinglian_attr_arr[2];
};


struct ZhuangbeiDetail
{
    unsigned long long zhuangbei_id;
    unsigned int group_id; //装备模板组

    unsigned int zhuangbei_type;
    char zhuangbei_name[ZhuangbeiNameMaxLen+1];
    unsigned int zhuangbei_colour;
    char zhuangbei_pic[ZhuangbeiPicMaxLen+1];

    //装备在谁的身上
    unsigned int zhuangbei_hero_id;

    unsigned int profession_id;    		//绑定的职业ID, 为0是任何职业
    unsigned int zhuangbei_level;
    unsigned int hole;
    unsigned int diamond_type_1;
    unsigned int diamond_type_2;
    unsigned int diamond_type_3;
    
    unsigned int gongji_min;
    unsigned int gongji_max;
    unsigned int hujia;

    //强化信息
    //定长
    QianghuaData qianghua_data;

    //附魔信息
    //定长
    bool has_fumo;
    ZBAttr fumo_attr;

    //变长
    ZhuangbeiCommonAttr common_attr;

    //common_attr结束后，紧跟
    //变长，以'\0'结束
    //char zhuangbei_desc[1];

    //传奇装带精炼信息，非传奇装不带(20140521后一些稀有装也带此信息)
    //JinglianData jinglian_data;

	//首饰才有洗炼信息
	//unsigned char purify_hole;		//洗炼孔数
	//ZBAttr purify_attr_arr[0]; 		//最多5组洗炼属性 

	////2014.05.26后把is_unlocked属性移到稀有装备 不在is_shenbing后面了
    //bool is_unlocked; //是否解封印

    //bool is_shenbing; //为true则为带以下信息
    
    //神兵带以下信息，非神兵不带//////////////////////
    //unsigned char identified_arr_count; //下面的隐藏属性鉴定了几条
    //ZBAttr hide_attr_arr[0]; //隐藏属性
    //篆刻经文
    //bool has_jingwen;
    //ZBAttr jingwen_attr_arr[2];//篆刻经文所得属性
    ////////////////////////////////////////

    ZhuangbeiDetail()
    {
        memset(this, 0, sizeof(ZhuangbeiDetail));
    };

    static unsigned int max_length()
    {
        //数据库中限制了装备描述最大96字节
        
        return sizeof(ZhuangbeiDetail) + 10*sizeof(ZBAttr) + 97 + sizeof(JinglianData) 
            + sizeof(bool)*2 + sizeof(unsigned char) + sizeof(ZBAttr)*5
            + sizeof(bool) + sizeof(ZBAttr)*2 + sizeof(unsigned char) + sizeof(ZBAttr)*5;
    }
};
inline void hton_zhuangbei_detail(ZhuangbeiDetail& detail_in, ZhuangbeiDetail& detail_out)
{

    detail_out.zhuangbei_id = LF::lf_htonll(detail_in.zhuangbei_id);
    detail_out.group_id = htonl(detail_in.group_id);
    
    detail_out.zhuangbei_type = htonl(detail_in.zhuangbei_type);
    
    strncpy(detail_out.zhuangbei_name, detail_in.zhuangbei_name, ZhuangbeiNameMaxLen);
    detail_out.zhuangbei_name[ZhuangbeiNameMaxLen] = 0;
    
    detail_out.zhuangbei_colour = htonl(detail_in.zhuangbei_colour);
    
    strncpy(detail_out.zhuangbei_pic, detail_in.zhuangbei_pic, ZhuangbeiPicMaxLen);
    detail_out.zhuangbei_pic[ZhuangbeiPicMaxLen] = 0;

    detail_out.zhuangbei_hero_id = htonl(detail_in.zhuangbei_hero_id);
	
	detail_out.profession_id = htonl(detail_in.profession_id);

    detail_out.zhuangbei_level = htonl(detail_in.zhuangbei_level);
    detail_out.hole = htonl(detail_in.hole);
    detail_out.diamond_type_1 = htonl(detail_in.diamond_type_1);
    detail_out.diamond_type_2 = htonl(detail_in.diamond_type_2);
    detail_out.diamond_type_3 = htonl(detail_in.diamond_type_3);
    detail_out.gongji_min = htonl(detail_in.gongji_min);
    detail_out.gongji_max = htonl(detail_in.gongji_max);
    detail_out.hujia = htonl(detail_in.hujia);

    //强化信息全部是char，无需转换
    detail_out.qianghua_data = detail_in.qianghua_data;

    //附魔信息
    detail_out.has_fumo = detail_in.has_fumo;
    detail_out.fumo_attr.attr_type = htonl(detail_in.fumo_attr.attr_type);
    detail_out.fumo_attr.attr_value = htonl(detail_in.fumo_attr.attr_value);
    detail_out.fumo_attr.cuiqu_suc_ratio = htonl(detail_in.fumo_attr.cuiqu_suc_ratio);

    //基本魔法属性
    detail_out.common_attr.attr_count = htonl(detail_in.common_attr.attr_count);
    for(int i=0; i<detail_in.common_attr.attr_count; ++i)
    {
        detail_out.common_attr.attr_arr[i].attr_type = htonl( detail_in.common_attr.attr_arr[i].attr_type);
        detail_out.common_attr.attr_arr[i].attr_value = htonl(detail_in.common_attr.attr_arr[i].attr_value);
        detail_out.common_attr.attr_arr[i].cuiqu_suc_ratio = htonl(detail_in.common_attr.attr_arr[i].cuiqu_suc_ratio);
    }

    //描述
    char* desc_in = ((char*)detail_in.common_attr.attr_arr) + sizeof(ZBAttr)*detail_in.common_attr.attr_count;
    char* desc_out = ((char*)detail_out.common_attr.attr_arr) + sizeof(ZBAttr)*detail_in.common_attr.attr_count;
    strcpy(desc_out, desc_in);

    //精炼
    JinglianData* jinglian_data_p_in = (JinglianData*)(desc_in + strlen(desc_in) +1);
    JinglianData* jinglian_data_p_out = (JinglianData*)(desc_out + strlen(desc_out) +1);
    if(detail_in.zhuangbei_colour == ZhuangbeiColour_Chuanqi
		|| detail_in.zhuangbei_colour == ZhuangbeiColour_Xiyou)
    {
        jinglian_data_p_out->jinglian_times = htonl(jinglian_data_p_in->jinglian_times);

        jinglian_data_p_out->jinglian_attr_arr[0].attr_type = htonl(jinglian_data_p_in->jinglian_attr_arr[0].attr_type);
        jinglian_data_p_out->jinglian_attr_arr[0].attr_value = htonl(jinglian_data_p_in->jinglian_attr_arr[0].attr_value);
        jinglian_data_p_out->jinglian_attr_arr[0].cuiqu_suc_ratio = htonl(jinglian_data_p_in->jinglian_attr_arr[0].cuiqu_suc_ratio);

        jinglian_data_p_out->jinglian_attr_arr[1].attr_type = htonl(jinglian_data_p_in->jinglian_attr_arr[1].attr_type);
        jinglian_data_p_out->jinglian_attr_arr[1].attr_value = htonl(jinglian_data_p_in->jinglian_attr_arr[1].attr_value);
        jinglian_data_p_out->jinglian_attr_arr[1].cuiqu_suc_ratio = htonl(jinglian_data_p_in->jinglian_attr_arr[1].cuiqu_suc_ratio);
	}
    else
    {
        return;
    }

	
    unsigned char* purify_hole_p_in = (unsigned char*)(jinglian_data_p_in+1);
    unsigned char* purify_hole_p_out = (unsigned char*)(jinglian_data_p_out+1);
	*purify_hole_p_out = *purify_hole_p_in;
    
	if(*purify_hole_p_in > 0) {
	    ZBAttr* purify_arr_p_in = (ZBAttr*)(purify_hole_p_in+1);
	    ZBAttr* purify_arr_p_out = (ZBAttr*)(purify_hole_p_out+1);

		for(int i=0; i<*purify_hole_p_in; ++i)
	    {
	        purify_arr_p_out->attr_type = htonl(purify_arr_p_in->attr_type);
	        purify_arr_p_out->attr_value = htonl(purify_arr_p_in->attr_value);
	        purify_arr_p_out->cuiqu_suc_ratio = htonl(purify_arr_p_in->cuiqu_suc_ratio);

	        purify_arr_p_in++;
	        purify_arr_p_out++;
	    }

		purify_hole_p_in = (unsigned char*)purify_arr_p_in;
		purify_hole_p_out = (unsigned char*)purify_arr_p_out;
	}
	else {
		purify_hole_p_in++;
		purify_hole_p_out++;
	}
	
	bool* is_unlocked_p_in = (bool*)(purify_hole_p_in);
	bool* is_unlocked_p_out = (bool*)(purify_hole_p_out);
	*is_unlocked_p_out = *is_unlocked_p_in;

	if(detail_in.zhuangbei_colour != ZhuangbeiColour_Chuanqi)
		return ;

    bool* is_shenbing_p_in = (bool*)(is_unlocked_p_in+1);
    bool* is_shenbing_p_out = (bool*)(is_unlocked_p_out+1);
    *is_shenbing_p_out = *is_shenbing_p_in;
    
    //神兵
    if(*is_shenbing_p_in)
    {
        unsigned char* identified_arr_count_p_in = (unsigned char*)(is_shenbing_p_in+1);
        unsigned char* identified_arr_count_p_out = (unsigned char*)(is_shenbing_p_out+1);
        *identified_arr_count_p_out = *identified_arr_count_p_in;

        ZBAttr* hide_arr_p_in = (ZBAttr*)(identified_arr_count_p_in+1);
        ZBAttr* hide_arr_p_out = (ZBAttr*)(identified_arr_count_p_out+1);
        for(int i=0; i<*identified_arr_count_p_in; ++i)
        {
            hide_arr_p_out->attr_type = htonl(hide_arr_p_in->attr_type);
            hide_arr_p_out->attr_value = htonl(hide_arr_p_in->attr_value);
            hide_arr_p_out->cuiqu_suc_ratio = htonl(hide_arr_p_in->cuiqu_suc_ratio);

            hide_arr_p_in++;
            hide_arr_p_out++;
        }

        bool* has_jingwen_p_in = (bool*)hide_arr_p_in;
        bool* has_jingwen_p_out = (bool*)hide_arr_p_out;
        *has_jingwen_p_out = *has_jingwen_p_in;

        ZBAttr* jingwen_arr_p_in = (ZBAttr*)(has_jingwen_p_in+1);
        ZBAttr* jingwen_arr_p_out = (ZBAttr*)(has_jingwen_p_out+1);
        jingwen_arr_p_out[0].attr_type = htonl(jingwen_arr_p_in[0].attr_type);
        jingwen_arr_p_out[0].attr_value = htonl(jingwen_arr_p_in[0].attr_value);
        jingwen_arr_p_out[0].cuiqu_suc_ratio = htonl(jingwen_arr_p_in[0].cuiqu_suc_ratio);

        jingwen_arr_p_out[1].attr_type = htonl(jingwen_arr_p_in[1].attr_type);
        jingwen_arr_p_out[1].attr_value = htonl(jingwen_arr_p_in[1].attr_value);
        jingwen_arr_p_out[1].cuiqu_suc_ratio = htonl(jingwen_arr_p_in[1].cuiqu_suc_ratio);
    }
}
inline void ntoh_zhuangbei_detail(ZhuangbeiDetail& detail_in, ZhuangbeiDetail& detail_out)
{
    detail_out.zhuangbei_id = LF::lf_htonll(detail_in.zhuangbei_id);
    detail_out.group_id = ntohl(detail_in.group_id);
    
    detail_out.zhuangbei_type = ntohl(detail_in.zhuangbei_type);
    
    strncpy(detail_out.zhuangbei_name, detail_in.zhuangbei_name, ZhuangbeiNameMaxLen);
    detail_out.zhuangbei_name[ZhuangbeiNameMaxLen] = 0;
    
    detail_out.zhuangbei_colour = ntohl(detail_in.zhuangbei_colour);
    
    strncpy(detail_out.zhuangbei_pic, detail_in.zhuangbei_pic, ZhuangbeiPicMaxLen);
    detail_out.zhuangbei_pic[ZhuangbeiPicMaxLen] = 0;

    detail_out.zhuangbei_hero_id = ntohl(detail_in.zhuangbei_hero_id);
	
	detail_out.profession_id = ntohl(detail_in.profession_id);

    detail_out.zhuangbei_level = ntohl(detail_in.zhuangbei_level);
    detail_out.hole = ntohl(detail_in.hole);
    detail_out.diamond_type_1 = ntohl(detail_in.diamond_type_1);
    detail_out.diamond_type_2 = ntohl(detail_in.diamond_type_2);
    detail_out.diamond_type_3 = ntohl(detail_in.diamond_type_3);
    detail_out.gongji_min = ntohl(detail_in.gongji_min);
    detail_out.gongji_max = ntohl(detail_in.gongji_max);
    detail_out.hujia = ntohl(detail_in.hujia);

    //强化信息全部是char，无需转换
    detail_out.qianghua_data = detail_in.qianghua_data;

    //附魔信息
    detail_out.has_fumo = detail_in.has_fumo;
    detail_out.fumo_attr.attr_type = ntohl(detail_in.fumo_attr.attr_type);
    detail_out.fumo_attr.attr_value = ntohl(detail_in.fumo_attr.attr_value);
    detail_out.fumo_attr.cuiqu_suc_ratio = ntohl(detail_in.fumo_attr.cuiqu_suc_ratio);

    //基本魔法属性
    detail_out.common_attr.attr_count = ntohl(detail_in.common_attr.attr_count);
    for(unsigned int i=0; i<detail_out.common_attr.attr_count; ++i)
    {
        detail_out.common_attr.attr_arr[i].attr_type = ntohl( detail_in.common_attr.attr_arr[i].attr_type);
        detail_out.common_attr.attr_arr[i].attr_value = ntohl(detail_in.common_attr.attr_arr[i].attr_value);
        detail_out.common_attr.attr_arr[i].cuiqu_suc_ratio = ntohl(detail_in.common_attr.attr_arr[i].cuiqu_suc_ratio);
    }

    //描述
    char* desc_in = ((char*)detail_in.common_attr.attr_arr) + sizeof(ZBAttr)*detail_out.common_attr.attr_count;
    char* desc_out = ((char*)detail_out.common_attr.attr_arr) + sizeof(ZBAttr)*detail_out.common_attr.attr_count;
    strcpy(desc_out, desc_in);

    //精炼
    JinglianData* jinglian_data_p_in = (JinglianData*)(desc_in + strlen(desc_in) +1);
    JinglianData* jinglian_data_p_out = (JinglianData*)(desc_out + strlen(desc_out) +1);
    if(detail_out.zhuangbei_colour == ZhuangbeiColour_Chuanqi
		|| detail_out.zhuangbei_colour == ZhuangbeiColour_Xiyou)
    {
        jinglian_data_p_out->jinglian_times = ntohl(jinglian_data_p_in->jinglian_times);

        jinglian_data_p_out->jinglian_attr_arr[0].attr_type = ntohl(jinglian_data_p_in->jinglian_attr_arr[0].attr_type);
        jinglian_data_p_out->jinglian_attr_arr[0].attr_value = ntohl(jinglian_data_p_in->jinglian_attr_arr[0].attr_value);
        jinglian_data_p_out->jinglian_attr_arr[0].cuiqu_suc_ratio = ntohl(jinglian_data_p_in->jinglian_attr_arr[0].cuiqu_suc_ratio);

        jinglian_data_p_out->jinglian_attr_arr[1].attr_type = ntohl(jinglian_data_p_in->jinglian_attr_arr[1].attr_type);
        jinglian_data_p_out->jinglian_attr_arr[1].attr_value = ntohl(jinglian_data_p_in->jinglian_attr_arr[1].attr_value);
        jinglian_data_p_out->jinglian_attr_arr[1].cuiqu_suc_ratio = ntohl(jinglian_data_p_in->jinglian_attr_arr[1].cuiqu_suc_ratio);
	}
    else
    {
        return;
    }

    unsigned char* purify_hole_p_in = (unsigned char*)(jinglian_data_p_in+1);
    unsigned char* purify_hole_p_out = (unsigned char*)(jinglian_data_p_out+1);
	*purify_hole_p_out = *purify_hole_p_in;
	
	if(*purify_hole_p_in > 0) {
	    ZBAttr* purify_arr_p_in = (ZBAttr*)(purify_hole_p_in+1);
	    ZBAttr* purify_arr_p_out = (ZBAttr*)(purify_hole_p_out+1);

		for(int i=0; i<*purify_hole_p_in; ++i)
	    {
	        purify_arr_p_out->attr_type = ntohl(purify_arr_p_in->attr_type);
	        purify_arr_p_out->attr_value = ntohl(purify_arr_p_in->attr_value);
	        purify_arr_p_out->cuiqu_suc_ratio = ntohl(purify_arr_p_in->cuiqu_suc_ratio);

	        purify_arr_p_in++;
	        purify_arr_p_out++;
	    }

		purify_hole_p_in = (unsigned char*)purify_arr_p_in;
		purify_hole_p_out = (unsigned char*)purify_arr_p_out;
	}
	else {
		purify_hole_p_in++;
		purify_hole_p_out++;
	}
	
	bool* is_unlocked_p_in = (bool*)(purify_hole_p_in);
	bool* is_unlocked_p_out = (bool*)(purify_hole_p_out);
	*is_unlocked_p_out = *is_unlocked_p_in;

	if(detail_out.zhuangbei_colour != ZhuangbeiColour_Chuanqi) {
		return ;
	}

    bool* is_shenbing_p_in = (bool*)(is_unlocked_p_in +1);
    bool* is_shenbing_p_out = (bool*)(is_unlocked_p_out +1);
    *is_shenbing_p_out = *is_shenbing_p_in;

    //神兵
    if(*is_shenbing_p_in)
    {

        unsigned char* identified_arr_count_p_in = (unsigned char*)(is_shenbing_p_in+1);
        unsigned char* identified_arr_count_p_out = (unsigned char*)(is_shenbing_p_out+1);
        *identified_arr_count_p_out = *identified_arr_count_p_in;

        ZBAttr* hide_arr_p_in = (ZBAttr*)(identified_arr_count_p_in+1);
        ZBAttr* hide_arr_p_out = (ZBAttr*)(identified_arr_count_p_out+1);
        for(int i=0; i<*identified_arr_count_p_out; ++i)
        {
            hide_arr_p_out->attr_type = ntohl(hide_arr_p_in->attr_type);
            hide_arr_p_out->attr_value = ntohl(hide_arr_p_in->attr_value);
            hide_arr_p_out->cuiqu_suc_ratio = ntohl(hide_arr_p_in->cuiqu_suc_ratio);

            hide_arr_p_in++;
            hide_arr_p_out++;
        }

        bool* has_jingwen_p_in = (bool*)hide_arr_p_in;
        bool* has_jingwen_p_out = (bool*)hide_arr_p_out;
        *has_jingwen_p_out = *has_jingwen_p_in;

        ZBAttr* jingwen_arr_p_in = (ZBAttr*)(has_jingwen_p_in+1);
        ZBAttr* jingwen_arr_p_out = (ZBAttr*)(has_jingwen_p_out+1);
        jingwen_arr_p_out[0].attr_type = ntohl(jingwen_arr_p_in[0].attr_type);
        jingwen_arr_p_out[0].attr_value = ntohl(jingwen_arr_p_in[0].attr_value);
        jingwen_arr_p_out[0].cuiqu_suc_ratio = ntohl(jingwen_arr_p_in[0].cuiqu_suc_ratio);

        jingwen_arr_p_out[1].attr_type = ntohl(jingwen_arr_p_in[1].attr_type);
        jingwen_arr_p_out[1].attr_value = ntohl(jingwen_arr_p_in[1].attr_value);
        jingwen_arr_p_out[1].cuiqu_suc_ratio = ntohl(jingwen_arr_p_in[1].cuiqu_suc_ratio);
    }
}


struct SPCmd_GetZhuangbeiInTeam
{
    //站位，无用
    unsigned int stub;
};

struct SPCmd_GetZhuangbeiInTeamRsp
{
    int err_code;

    //
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct HeroAndZhuangbei
{
    unsigned int hero_id;

    //
    unsigned int zb_count;
    unsigned long long zb_id_arr[1];
};
struct SPCmd_EquipZhuangbei
{

    //
    unsigned int hero_count;
    HeroAndZhuangbei hero_zb_arr[0];
};

struct SPCmd_EquipZhuangbeiRsp
{
    int err_code;
};

struct SPCmd_AdjustFormation
{
    
    //从1到4号位的英雄id，如果为0则表示该位空缺。
    unsigned int hero_id_arr[FormationHeroAcountMax];
};

struct SPCmd_AdjustFormationRsp
{
    int err_code;
};


struct SPCmd_GetTaskList
{
    //临时
    unsigned int stub;
};

struct ItemInfo
{
    unsigned int type;
    unsigned int count;
};


/*
struct SubTaskInfo
{
    unsigned int subtask_id;
    
    //该任务所处的状态。
    unsigned int task_status;

    //周一到周七哪些天开启
    //bit0~6分别对应星期日、星期1~6
    unsigned char active_day_bitmap;

    //是否有开启时间限制
    bool is_hour_limited;
    //hour从0~23， min从0~59
    //开启时间
    unsigned char start_hour;
    unsigned char start_min;
    //结束时间
    unsigned char end_hour;
    unsigned char end_min;

    //是否有每日次数限制
    bool is_play_times_limited;
    unsigned char times_limit_oneday;
    unsigned char play_times_today;

    void clear()
    {
        subtask_id = 0;
        task_status = TaskStatus_NotActived;

        active_day_bitmap = 0;
        is_hour_limited = false;
        is_play_times_limited = false;
    };

    SubTaskInfo()
    {
        clear();
    }
};
inline void hton_subtask_info(SubTaskInfo& info_in, SubTaskInfo& info_out)
{
    memcpy(&info_out, &info_in, sizeof(info_in));
    
    info_out.subtask_id = htonl(info_in.subtask_id);
    info_out.task_status = htonl(info_in.task_status);
}
inline void ntoh_subtask_info(SubTaskInfo& info_in, SubTaskInfo& info_out)
{
    hton_subtask_info(info_in, info_out);
}
*/
struct SPCmd_GetTaskListRsp
{
    int err_code;
    
    //当前激活的最后一个主任务
    unsigned int maintask_last_actived;
    //该任务所处的状态。
    unsigned int maintask_status;

    //当前需要显示的副本列表
    //int subtask_count;
    //可变数组
    //SubTaskInfo task_arr[0];
};


/* 任务棋盘  */
//任务棋盘格子类型
enum TaskGridType {
    TaskGridType_Empty 		= 0, 	//空牌
    TaskGridType_Monster 	= 1, 	//怪物牌
    TaskGridType_Box 		= 2, 	//宝箱牌
};

//任务棋盘格子状态
enum TaskGridStat {
    TaskGridStat_PreFlop 	= 0, 	//未翻牌
    TaskGridStat_Floped 	= 1, 	//已翻牌
    TaskGridStat_Folded		= 2, 	//扣牌
};

//棋盘格子信息
struct TaskGridInfo
{
    unsigned int type;				//TaskGridType
	
	//翻牌后如果不是怪物牌 直接置成状态TaskGridStat_Folded 不然则为TaskGridStat_Floped
    unsigned int stat; 				//TaskGridStat
    unsigned int monster_type;		//如果格子是怪物 关卡类型 GuanQiaType
};

//获取任务棋盘请求包
struct SPCmd_GetTaskChessBoard
{
    unsigned int task_id;

    bool need_refresh_chessboard; 	//是否需要重新刷新棋盘 ，目前在玩家每次从地图进入时都刷新。
};

//获取任务棋盘返回包
struct SPCmd_GetTaskChessBoardRsp
{
    unsigned int err_code;

    unsigned int horizontal_count;	//横着的格子数量
    unsigned int vertical_count;	//竖着的格子数量

	TaskGridInfo grid_arr[1];		//下标0是boss 后面的就是horizontal_count*vertical_count
};

//翻牌请求包
struct SPCmd_FlopTaskGrid
{
	unsigned int task_id;
	unsigned int grid_id;	//只能是1-16
};

//翻牌返回包
struct SPCmd_FlopTaskGridRsp
{
    unsigned int err_code;

	unsigned int boss_stat;			//TaskGridStat
	unsigned int grid_stat;			//TaskGridStat
	
    //item数量
    int count;
    ItemInfo item_arr[0];
};


//获取棋盘宝箱请求包
struct SPCmd_GetChessBox {
	unsigned int task_id;
    unsigned int grid_id; //1~16
};

//获取棋盘宝箱返回包
struct SPCmd_GetChessBoxRsp {
    unsigned int err_code;
	
    //item数量
    int count;
    ItemInfo item_arr[0];
};


struct SPCmd_GetItemsInTeam
{
    //临时
    unsigned int stub;
};

struct SPCmd_GetItemsInTeamRsp
{
    unsigned int err_code;

    //item数量
    int count;
    ItemInfo item_arr[0];
};
/*
struct SPCmd_ZBQianghua
{
    
    unsigned long long zhuangbei_id;

    //附加了几份概率提升材料
    unsigned int gailv_cailiao_count;
};

struct SPCmd_ZBQianghuaRsp
{
    int err_code;

    //qianghua_result标示强化成功与否。
    //如果成功，则后面的几个属性为强化后的结果
    bool qianghua_sucess;
    unsigned int qianghua_time;
    unsigned int gongji_min;
    unsigned int gongji_max;
    unsigned int hujia;
};
*/

struct SPCmd_ZBQianghua
{
    unsigned long long zhuangbei_id;
	bool b_gold_star;			//  是否直接强化为金星
};

struct SPCmd_ZBQianghuaRsp
{
    int err_code;

    //强化效果
    QianghuaInfo qianghua_info;
};

struct SPCmd_ZBQianghuaHuitui
{
    unsigned long long zhuangbei_id;

    //要保留的强化id
    int keep_count; //0~8
    unsigned char id_arr[0];
};


struct SPCmd_ZBQianghuaHuituiRsp
{
    int err_code;

    QianghuaData new_qianghua_data;
};

/*
struct SPCmd_ZBZhuankong
{
    
    unsigned long long zhuangbei_id;
};

struct SPCmd_ZBZhuankongRsp
{
    int err_code;
};
*/

struct SPCmd_ZBChaijie
{
    //
    unsigned int zb_count;
    unsigned long long zb_id_arr[1];
};

struct SPCmd_ZBChaijieRsp
{
    int err_code;

    //拆解获得item数量
    int count;
    ItemInfo item_arr[0];

    //拆解获得的材料为变长 后面紧跟返还的材料
    //int return_count;
    //ItemInfo re_item_arr[0];
};

struct SPCmd_ZBXiangBaoshi
{
    //
    unsigned long long zhuangbei_id;
    unsigned int baoshi_type;
};

struct SPCmd_ZBXiangBaoshiRsp
{
    int err_code;
};

struct SPCmd_ZBChaiBaoshi
{
    //
    unsigned long long zhuangbei_id;
};

struct SPCmd_ZBChaiBaoshiRsp
{
    int err_code;
};

struct SPCmd_BaoshiHecheng
{
    //
    unsigned int baoshi_type;
    int count;
};

struct SPCmd_BaoshiHechengRsp
{
    int err_code;
};

struct SPCmd_QueryAllMofaJinghua
{
    //占位
    int stub;
};

struct MofaJinghua
{
    unsigned int zb_type;
    unsigned int attr_type;
    unsigned int level;
    
    unsigned int count;
};
struct SPCmd_QueryAllMofaJinghuaRsp
{
    int err_code;

    int count;
    MofaJinghua jinghua_arr[0];
};

struct SPCmd_ZBCuiqu
{
    unsigned long long zhuangbei_id;

    unsigned int attr_type;
    unsigned int attr_value;
    bool is_fumo_attr;
};

struct SPCmd_ZBCuiquRsp
{
    int err_code;

    bool sucess;
    MofaJinghua jinghua; //sucess=true 则有效
};


struct SPCmd_ZBFumo
{
    unsigned long long zhuangbei_id;

    MofaJinghua jinghua;
};

struct SPCmd_ZBFumoRsp
{
    int err_code;

    bool sucess;
    ZBAttr new_fumo_attr;
};

struct SPCmd_QueryFubenDiaoluo
{
    //
    unsigned int task_id;
};

struct FubenDiaoluoZBInfo
{
    unsigned int group_id;
    bool is_suipian;
    unsigned int zhuangbei_type;
    char zhuangbei_name[ZhuangbeiNameMaxLen+1];
    unsigned int zhuangbei_colour;
    char zhuangbei_pic[ZhuangbeiPicMaxLen+1];
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

    unsigned int attr_str_count;
    char attr_str[0]; //attr_str_count个连续字符串，每个字符串以'\0'结束

    //char desc[0]; //紧跟在attr_str的最后一个'\0'之后，以'\0'结束 
};
struct SPCmd_QueryFubenDiaoluoRsp
{
    int err_code;

    unsigned int item_count;
    unsigned int item_type_arr[0]; //变长

    //unsigned int zb_count;
    //char data[0]; //变长结构FubenDiaoluoZBInfo 数组
};



//
struct SPCmd_Battle
{
    //
    unsigned int task_id;
    unsigned int grid_id; //0~16
};

// 每一个角色身上的装备数量
#define	MAX_EQUIP_NUM_EACH_ACTOR	8

// 见HeroBase
struct Battel_HeroBase
{
	unsigned int heroId;
	unsigned profession_id;		// 职业id
	unsigned int level;
	unsigned int stage;			// 阶，初始就是1
	unsigned int hp;			// 血量，当前的血值
	unsigned int defaultHp;		// 满血值
	short hero_type;			// HeroBaseType
	short defaultCdTime;		// cd时间，是步数，不包括wait和useBlood
	short x_size;				// 描述x方向上的占用的格子数量，默认是1，还有的取值是2,客户端就应该把xsize是2的放在横向的中间而不是gridpos里面
	short resonance_star;		//装备强化共鸣星级

	// 装备信息
	short zb_summary_Num;
	ZhuangbeiSummary zhuangbei_summary_arr[MAX_EQUIP_NUM_EACH_ACTOR]; //ZhuangbeiSummary
};
// 见InitChessBoard
struct Battel_InitChessBoard
{
	// 我方的战斗布局,[0]就是在index0号位置的角色数据
	Battel_HeroBase myTeam[FormationHeroAcountMax];
	unsigned int myTeamUniqueId[FormationHeroAcountMax];// 全战斗一的
	short myCurBloodNum;		// 我方当前可用血瓶数量 -----> class ChessBoard -->currentBloodNum
	short myDefaultBloodNum;	// 我方当前全部血瓶数量

	// 对方的布局
	Battel_HeroBase oppositeTeam[FormationHeroAcountMax];
	unsigned int oppositeUniqueId[FormationHeroAcountMax];
	short oppositeCurBloodNum;		// 对方当前可用血瓶数量
	short oppositeDefaultBloodNum;	// 对方当前全部血瓶数量
};
// 参见ClientGameBuffEffect
struct Battel_GameBuffEffect		// buff的一个效果,也是一次的作用，在移除的时候才作用。
{
	int effect_id;					// id，一场战斗的唯一标识，由GameBuffPro生成
	bool xuanyun;
	bool fantan;					// 反弹
	// change_state负数就是要减*，正数就是要加*
	char hp_change_state;			
	char baoji_change_state;		
	char gongjili_change_state;	
	char shanbi_change_state;	
	char fangyu_change_state;	
};
// 参见ClientGameBuff
struct Battel_GameBuff
{
	short buffStateId;					//BuffStateId状态类型，相同状态不应该相互叠加
	bool cumulative;					// 是否可以累积
	unsigned int attachedToUId;	// 被附加上的角色的uniqueId
	unsigned int fireUId;		// 释放该buff的角色uniqueId

	int effectNum;
	char gameBuffEff_arr[1];
};


// 见StepMove
struct Battel_StepMove
{
	unsigned int uniqueId;	// 谁移动
	int destIndex;			// 目的 的索引位置，如果<0就代表不移动
	bool inSelfChessBoard;	// 是否是自身棋盘的坐标
};
// 见StepChangeRound
struct Battel_StepChangeRound
{
	unsigned int round;	// 回合数
};
// 见StepFight
struct Battel_StepFight
{
	// 攻击者和被攻击者的Id
	unsigned int fightorUniqueId;
	short fightPattern;

	short targetNum;
	unsigned int targetUniqueId_arr[1];		// 被攻击对象,变长
	bool canMiss[1];						// 能否躲避，变长
	int targetBloodChange[1];				// 血量变化，正数就是加血，负数就是减血
};
//见StepUseBlood
struct Battel_StepUseBlood
{
	unsigned int uniqueId;	// 谁使用，cd时间是固定的，在初始化信息里面。
	int bloodChangeForUser;	// 血量变化
};
// 见StepSkill
struct Battel_StepSkill
{
	unsigned int fightorUniqueId;
	short skillPatten;
	short skillType;						// 技能类型


	short targetNum;
	short buffNum;							// 打出的buff个数
	char gameBuff_arr[1];					// Battel_GameBuff打出的Buff，buff定义了附加者，那么这个buff就是添加给附加者的
	unsigned int targetUniqueId_arr[1];		// 被攻击对象,变长
	bool canMiss[1];						// 能否躲避，变长
	int targetBloodChange[1];				// 血量变化，正数就是加血，负数就是减血
};


// 见Buff_Doing
struct Battel_Buff_Doing
{
	unsigned int target;
	int hp_add;		// 血量的增加，负数就是减少
	short relativedBuffStateId;	// 关联到的buff

	bool isFantan;				// 如果是反弹，下面的数据才有用
	unsigned int fightor_u_id;	// 反弹的目标
	
};

// 见StepBuff
struct Battel_BuffInStep
{
	int removeBuffNum;
	int remove_buff_arr[1];		// 移除的buff的数组

	// 里面可以有多个target都是同一个人，客户端需要把是某个target的buffdoing全部放到那个人身上
	int effect_num;
	char effects[1]; //vector<Buff_Doing> effects 
};


// 步伐
struct Battel_OneStep
{
	// 步调形式：移动不攻击，直接攻击(里面可以涵盖移动);
	// 根据形式的不同，就取不同的数据
	short stepType;	
	Battel_BuffInStep buff;

	int cdTimeNum;
	char cdDiscr_arr[1];

	char stepData[1];			// 步伐数据，如果是移动，那么就是Battel_StepMove，如果是fight就是Battel_StepFight
};

// 一个能量点
struct PowerPoint
{
	unsigned int power; // 这个能量点的能量
};

// 战斗结束
struct Battel_FightEnd
{
	// 我方的战斗结果4B
	short myResult;		//FightResult

	// 得到的经验
	unsigned int gettedExp;
	// 得到的ItemSuiPian
	unsigned int gettedItemSuiPianNum;
	char gettedItemSuiPian_arr[1];
	// 得到的神兵碎片
	unsigned int gettedShenBingSuiPianNum;
	char gettedShenBingSuiPian_arr[1];

	unsigned int powerPointNum;
	char powerPoint_arr[1]; // PowerPoint

	unsigned int zhuangbeiNum;			//获得的装备数量
	unsigned int itemNum;				// 获得的物品

	char zhuangbei_arr[1];//ZhuangbeiDetail
	char item_arr[1];//ItemInfo
};

// 给某一个客户端的剧本
struct Battel_FightScript
{
	// 初始化数据，客户端不一样
	Battel_InitChessBoard init;

	unsigned int stepCount;		// 步伐数量
	Battel_OneStep step_arr[1];

	// 结果数据，客户端不一样
	Battel_FightEnd endResult;
};
struct SPCmd_BattleRsp
{
    int err_code;

	int fightNum;	// 战斗的次数
	// 脚本数据 数组
	char fightScript_array[1];

	//Battel_FightScript fightScript;
};


//重连请求包
struct SPCmd_Reconnect
{
    char client_account[ClientAccountMaxLen+1];
    char client_psw[ClientPswMaxLen+1];

    unsigned int main_hero_id;
};

//重连返回包
struct SPCmd_ReconnectRsp
{
    int err_code;

    unsigned int server_time;
};

struct SPCmd_ChangeTaskStat
{
    unsigned int task_id;
    unsigned int stat;
};

struct SPCmd_ChangeTaskStatRsp
{
    int err_code;

    //如果是提交主线任务，则会有任务奖励。否则为0
    unsigned int exp;
    unsigned int silver;

    //
    unsigned int hero_count;
    HeroDetail hero_arr[0];
};

struct SPCmd_RestoreVitality
{
    //占位符
    unsigned int stub;
};

struct SPCmd_RestoreVitalityRsp
{
    int err_code;
};

struct SPCmd_BeKickOutRsp
{
    int err_code;
};


/* 心跳包 */
//跑马灯消息类型
enum MarqueeType {
    MarqueeType_Invalid				= 0,	//无效类型
    MarqueeType_ForgeLegend			= 1,	//锻造传奇		
    MarqueeType_VarietyStore		= 2,	//杂货铺
    MarqueeType_Compound			= 3,	//传奇合成
    MarqueeType_TopArena			= 4,	//巅峰对决
    MarqueeType_TopList				= 5,	//英雄榜
    MarqueeType_ProtectSpirit		= 6,	//守护之灵
    MarqueeType_Discover_Destroy	= 7,	//寻宝 破外封印
    MarqueeType_Discover_Unlock		= 8,	//寻宝 解封装备
};

//跑马灯消息
struct MarqueeMessage {
	//消息唯一标示符
    unsigned int uid;					
	
	//消息类型 
    unsigned short type;				//MarqueeType
    
    //消息主体
    char subject[HeroNameMaxLen+1];		//玩家名

	//消息客体
    union {
		//MarqueeType_ForgeLegend, MarqueeType_Compound, MarqueeType_Discover_Unlock
		unsigned int group_id;		//传奇group 
		
		//MarqueeType_VarietyStore
		unsigned int item_type;		//道具类型
		
		//MarqueeType_TopArena
		unsigned int round;			//场次 0:4强 1:决赛 2: 冠军
		
		//MarqueeType_TopList
		unsigned int top_order;		//英雄榜排名

		//MarqueeType_ProtectSpirit
		unsigned int chapter_id;	//章节id
    } object ;


	unsigned short count;			//消息客体数量

	MarqueeMessage () {
		memset(this, 0, sizeof(MarqueeMessage));
	}
};


struct SPCmd_HeartBeat
{
    //占位符
    unsigned int stub;
};

struct SPCmd_HeartBeatRsp
{
    int err_code;

    unsigned int current_vit; 	//当前体力值
    
    unsigned char msg_count;	//跑马灯消息个数 最多10个
    MarqueeMessage msg_arr[0];	//跑马灯消息队列
};


struct SkillInfo
{
    unsigned int skill_id; //道具实体id
    unsigned int skill_type; //enum SkillType
    unsigned int skill_level; 
    
    unsigned int owner_hero_id; //是谁学会的技能
    bool is_equiped; //是否装配
};


struct SPCmd_QueryAnotherTeamInfo
{
    //another team main hero_id
    unsigned int hero_id;
};

struct SPCmd_QueryAnotherTeamInfoRsp
{
    int err_code;

    //上阵英雄信息
    HeroDetail hero_arr[4];

    //上阵英雄所装备的技能
    SkillInfo skill_arr[4];

    //装备信息
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct SPCmd_ChargeVitality
{
    //占位
    int stub;
};

struct SPCmd_ChargeVitalityRsp
{
    int err_code;

    unsigned int gold_balance;
};

struct SPCmd_QueryAllSuitName
{
    //占位
    int stub;
};

struct SuitNameInfo
{
    unsigned int suit_id;
    char suit_name[ZhuangbeiNameMaxLen+1];

    unsigned int group_id_1; //为0表示无效
    unsigned int zhuangbei_type_1; 
    char zhuangbei_name_1[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_2; //为0表示无效
    unsigned int zhuangbei_type_2;
    char zhuangbei_name_2[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_3; //为0表示无效
    unsigned int zhuangbei_type_3;
    char zhuangbei_name_3[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_4; //为0表示无效
    unsigned int zhuangbei_type_4;
    char zhuangbei_name_4[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_5; //为0表示无效
    unsigned int zhuangbei_type_5;
    char zhuangbei_name_5[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_6; //为0表示无效
    unsigned int zhuangbei_type_6;
    char zhuangbei_name_6[ZhuangbeiNameMaxLen+1];

    ZBAttr suit_attr[5][2]; //5件属性，每件最多2个。attr_type为0表示无效
};

struct SPCmd_QueryAllSuitNameRsp
{
    int err_code;

    int suit_count;
    char data[1]; //SuitNameInfo suit_arr[x];
};



struct SPCmd_NewAssistantJoinRsp
{
    int err_code;

    //
    HeroDetail hero_detail;

    //
    SkillInfo skill; //当前装备的技能
    

    //
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};


struct SPCmd_QuerySubTaskList
{
    //占位
    int stub;
};

enum SubTaskType
{
    SubTaskType_Richang = 1,
    SubTaskType_Fenzhi  = 2,
};

#define SubTaskDescMaxLen 96
struct SubTaskInfor
{
    unsigned int task_id;
    unsigned int task_type;
    unsigned int reward_item_id;
    unsigned int reward_item_count;
    char desc[SubTaskDescMaxLen+1];
};
struct SPCmd_QuerySubTaskListRsp
{
    int err_code;

    //
    unsigned int task_count;
    SubTaskInfor task_arr[0];
};

struct SPCmd_SubTaskSubmitedRsp
{
    int err_code;

    unsigned int task_id;
    unsigned int reward_item_id;
    unsigned int reward_item_count;
};


struct SPCmd_QueryMainTaskReward
{
    //占位
    int stub;
};

struct main_task_reward
{
    unsigned int task_id;
    
    unsigned int exp;
    unsigned int silver;
};
struct SPCmd_QueryMainTaskRewardRsp
{
    int err_code;

    unsigned int task_count;
    main_task_reward task_reward[0];
};

struct SPCmd_ArenaEnter
{
    //
    unsigned char hardLevel; // 1~5
};


/* 充值 */
//充值包
struct ChargePacketInfo{
    unsigned int id; 				//充值id
    unsigned int charge; 			//充值金额
    unsigned int gold_count; 		//获得金币数
    unsigned int present_count; 	//赠送金币数
};

//查询充值包请求包
struct SPCmd_QueryChargePacket
{
    //占位
    int stub;
};

//查询充值包返回包
struct SPCmd_QueryChargePacketRsp
{
    int err_code;
	
	unsigned int next_vip_level;	//下一VIP等级 
	unsigned int next_need_charge;	//到达下一个等级需要的充值金额 (已是最高级为0)
	
    bool b_first_charged; 			//首次充值与否 false:未充值过 true:已充值过
    ItemInfo first_present_item[5];	//首次充值赠送的道具(ps:金币翻倍没有在里面)
	
    unsigned int count; 			// 充值包个数
    ChargePacketInfo info[0];
};

//充值请求包
struct SPCmd_ChargeGold
{
    //充值包id
    unsigned int gold_packet_id; 
};

//充值返回包
struct SPCmd_ChargeGoldRsp
{
    int err_code;
    
    unsigned int gold_balance;		//充值后的金币量
    unsigned int vip_level;			//充值后的VIP等级
    
	unsigned int next_vip_level;	//下一VIP等级 
	unsigned int next_need_charge;	//到达下一个等级需要的充值金额 (已是最高级为0)
};

struct SPCmd_QueryTeamSkillList
{
    //占位
    int stub;
};


struct SPCmd_QueryTeamSkillListRsp
{
    int err_code;

    unsigned int skill_count;
    SkillInfo skill_arr[0];
};

struct SPCmd_EquipSkill
{
    unsigned int hero_id; //要给谁装配
    unsigned int skill_id;
};

struct SPCmd_EquipSkillRsp
{
    int err_code;
};

struct SPCmd_ActorLearnSkill
{
    unsigned int hero_id; //要给谁学
    unsigned int item_id; //技能书道具的id
};

struct SPCmd_ActorLearnSkillRsp
{
    int err_code;

    SkillInfo new_skill;
};

struct SPCmd_ArenaEnterRsp
{
    int err_code;
    
    //进入竞技场时，房间已有的人
    HeroInfo hero_info_arr[3]; //hero_id=0表示无效

    //
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct SPCmd_ArenaUpdateRoomRsp
{
    int err_code;
    
    HeroInfo new_hero_info;

    //主角所带装备
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};


struct GiftZhuangbei
{
    unsigned long long zhuangbei_id;

    unsigned int zhuangbei_type;
    char zhuangbei_name[ZhuangbeiNameMaxLen+1];
    unsigned int zhuangbei_colour;
    char zhuangbei_pic[ZhuangbeiPicMaxLen+1];
};
struct GiftItem
{
    unsigned int type;
    unsigned int count;
};
struct GiftJifen
{
    unsigned int gift_id;
    unsigned int jifen;
};

enum GiftType
{
    GiftType_Zhuangbei = 1,
    GiftType_Item      = 2,
    GiftType_Jifen     = 3,
};

struct GiftData
{
    unsigned int gift_id;

    
    int type; //GiftType
    union
    {
        GiftZhuangbei zhuangbei;
        GiftItem item;
        GiftJifen jifen;
    } data_union;

    GiftData htonl_gift()
    {
        GiftData data;
        data.gift_id = htonl(gift_id);
        data.type = htonl(type);

        switch(type)
        {
            case GiftType_Zhuangbei:
                data.data_union.zhuangbei.zhuangbei_id = LF::lf_htonll(data_union.zhuangbei.zhuangbei_id);
                data.data_union.zhuangbei.zhuangbei_type = htonl(data_union.zhuangbei.zhuangbei_type);
                data.data_union.zhuangbei.zhuangbei_colour = htonl(data_union.zhuangbei.zhuangbei_colour);
                memcpy(data.data_union.zhuangbei.zhuangbei_name, data_union.zhuangbei.zhuangbei_name, ZhuangbeiNameMaxLen+1);
                memcpy(data.data_union.zhuangbei.zhuangbei_pic, data_union.zhuangbei.zhuangbei_pic, ZhuangbeiPicMaxLen+1);
                break;
            case GiftType_Item:
                data.data_union.item.type = htonl(data_union.item.type);
                data.data_union.item.count = htonl(data_union.item.count);
                break;
            case GiftType_Jifen:
                data.data_union.jifen.jifen = htonl(data_union.jifen.jifen);
                break;
            default:
                assert(false);
        }

        return data;
    };
};



struct SPCmd_ArenaGetGift
{
    unsigned int gift_id;
};

struct SPCmd_ArenaGetGiftRsp
{
    int err_code;
};


struct SPCmd_ArenaGetBattleScriptGiftRsp
{
    int err_code;

    //随机模式今日还剩余多少次机会
    unsigned int battle_time_remainder;

    //战局信息
    unsigned int first_turn_hero_id_arr[4]; //0与1一组，2与3一组
    unsigned int second_turn_hero_id_arr[2];

    //得到的所有宝箱
    unsigned char gift_available_count;
    GiftJifen gift_jifen_arr[9];

	int fightNum;	// 战斗的次数
	// 脚本数据 数组
	char fightScript_array[1];
};




struct SPCmd_TeamBossEnter
{
    //
    unsigned char hardLevel; // 
};


struct SPCmd_TeamBossEnterRsp
{
    int err_code;
    
    //进入团队副本时，房间已有的人
    HeroInfo hero_info_arr[3]; //hero_id=0表示无效

    //
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct SPCmd_TeamBossUpdateRoomRsp
{
    int err_code;
    
    HeroInfo new_hero_info;

    //主角所带装备
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct SPCmd_TeamBossGetGift
{
    unsigned int gift_id;
};

struct SPCmd_TeamBossGetGiftRsp
{
    int err_code;
};

struct TeamBossScoreData
{
    unsigned int hero_id;
    unsigned char order; //排名1~n
    unsigned int boss_hp; //砍boss多少血量
};

struct SPCmd_TeamBossGetBattleScriptGiftRsp
{
    int err_code;

    //结算
    TeamBossScoreData score_arr[4];

    //得到的所有宝箱
    unsigned char gift_available_count;
    GiftData gift_data_arr[9];

	// 脚本数据
	char fightScript[1];
};

struct SPCmd_CityGuardEnter
{
    //占位
    unsigned char stub;
    
};

struct SPCmd_CityGuardEnterRsp
{
    int err_code;

    //当前已经坚守轮数 0~x
    unsigned int sucess_turn;
    //当前已经获得银币总量
    unsigned int current_silver;
    
    //最近一次守城胜利轮数0~x
    unsigned int last_total_turns;
    //最近一次守城得银币总量
    unsigned int last_silver;
};

struct SPCmd_CityGuardGetNextMonster
{
    //占位
    unsigned int stub;
};

struct SPCmd_CityGuardGetNextMonsterRsp
{
    int err_code;

    //下一波怪物首领
    unsigned int monster_leader_profession;

    //下一波赏金
    unsigned int silver;
};

struct SPCmd_CityGuardFight
{
    //迎战还是退却
    bool fight;
};

struct SPCmd_CityGuardFightRsp
{
    int err_code;

    //如果选择退却或者战败，则这里是结算数据。
    //否则为本次守城积累的银币。
    unsigned int silver;
    
    //标示玩家是否选择了迎战
    bool isFighting;
    //如果isFighting为true则这里存放战斗脚本
    char fightScript[1];
};

struct SPCmd_TopHeroGetList
{
    //占位
    unsigned int stub;
};

struct HeroAndOrder
{
    HeroInfo hero_info;
    unsigned int order;
	unsigned int zhandouli;
	
};

struct TopHeroReport
{
    HeroAndOrder challenger;		//挑战者	ps: order为战斗后的排名
    HeroAndOrder be_challenger;		//被挑战者
    
    unsigned int other_order;		//战斗另一方的当前排名

	bool challenger_win;			//true: 挑战者胜利; false: 被挑战者胜利
	unsigned int fight_time;		//战斗的时间戳
};

struct SPCmd_TopHeroGetListRsp
{
    int err_code;

    //当前排名
    unsigned int my_order;

    unsigned int next_chlg_time; //下一场挑战的时间戳(PS:当前时间>=此时间戳时皆可挑战 反之不能挑战)

    //top10
    HeroAndOrder top10_hero[10];

    unsigned int hero_count;	//排名在自己前的英雄个数
	unsigned int report_count;	//战报数量

	char data[0];
    //HeroAndOrder hero_arr[1];			//排名在自己前的英雄信息
	//TopHeroReport report_arr[0];		//战报
};

struct SPCmd_TopHeroChallenge
{
    //要挑战谁
    unsigned int hero_id;
};

struct SPCmd_TopHeroChallengeRsp
{
    int err_code;

    //战斗之后的排名
    unsigned int order_after_fight;
	
    unsigned int next_chlg_time; //下一场挑战的时间戳(PS:当前时间>=此时间戳时皆可挑战 反之不能挑战)

    //前后各若干名
    //unsigned int hero_count;
    //HeroAndOrder hero_arr[0];
    
    // 脚本
	char fightScript[1];
};

//清除挑战冷却时间请求包
struct SPCmd_TopHeroClearCDTime
{
    int stub; 			//占位
};

//清除挑战冷却时间返回包
struct SPCmd_TopHeroClearCDTimeRsp
{
    int err_code;

	unsigned int gold_balance; 		//当前金币余额
	unsigned int next_chlg_time; //下一场挑战的时间戳(PS:当前时间>=此时间戳时皆可挑战 反之不能挑战) 
};



struct SPCmd_SystemGiftQuery
{
    //占位
    int stub;
};

struct systemGift
{
    unsigned int gift_id;
    bool is_actived;
    unsigned int hero_id;
    unsigned int item_id;
    unsigned int item_count;
    char gift_desc[SystemGiftDesc_MaxLen+1];
};

struct SPCmd_SystemGiftQueryRsp
{
    int err_code;

    unsigned int gift_count;
    systemGift gift_arr[0];
};

struct SPCmd_SystemGiftGetGift
{
    unsigned int gift_id;
};

struct SPCmd_SystemGiftGetGiftRsp
{
    int err_code;

    unsigned int item_id;
    unsigned int item_count;
};


struct RentZB
{
    //当前是否出租出去
    bool is_rent;
    //距离当前租约到期时长(单位分钟)
    unsigned int time_to_rent_end;

    //当前最高竞标价，如果当前无租约则为出租人标价
    unsigned int highest_price;

	// 变长放在最后
	ZhuangbeiDetail zb_detail;
};

struct SPCmd_RentGetMyZB
{
    //占位
    int stub;
};

struct SPCmd_RentGetMyZBRsp
{
    int err_code;

    int zb_count;
    char zb_arr[0]; //RentZB
};

struct SPCmd_RentAddMyZB
{
};

struct SPCmd_RentAddMyZBRsp
{
};

struct SPCmd_RentCancelMyZB
{
};

struct SPCmd_RentCancelMyZBRsp
{
};

struct SPCmd_RentRecommend
{
};

struct SPCmd_RentRecommendRsp
{
};

struct SPCmd_RentQuery
{
};

struct SPCmd_RentQueryRsp
{
};

struct SPCmd_RentBid
{
};

struct SPCmd_RentBidRsp
{
};

struct SPCmd_DuanzaoListQuery
{
    //占位
    unsigned int stub;
};

struct DuanzaoGroup
{
    unsigned int duanzao_group_id;
    unsigned int open_level; //多少级才能开放
    char group_name[DUANZAO_GROUP_NAME_MAX_LEN+1];

    //所需材料
    unsigned int item_id; //为0表示不需要
    unsigned int item_count;
    //所需金币
    unsigned int gold_need;
};
struct SPCmd_DuanzaoListQueryRsp
{
    int err_code;

    int group_count;
    DuanzaoGroup group_arr[0];
};

struct SPCmd_DuanzaoGroupZBQuery
{
    //锻造group_id
    unsigned int duanzao_group_id;
};

struct DuanzaoGroupZBInfo
{
    unsigned int zb_group_id;
    unsigned int zhuangbei_type;
    char zhuangbei_name[ZhuangbeiNameMaxLen+1];
    unsigned int zhuangbei_colour;
    char zhuangbei_pic[ZhuangbeiPicMaxLen+1];
    unsigned int level;
    unsigned int ratio; 				//产出概率
    unsigned int next_group_id;			//下一等级的group_id, 为0是最高等级
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

    unsigned int attr_str_count;
    char attr_str[0]; //attr_str_count个连续字符串，每个字符串以'\0'结束

    //char desc[0]; //紧跟在attr_str的最后一个'\0'之后，以'\0'结束 
};
struct SPCmd_DuanzaoGroupZBQueryRsp
{
    int err_code;

    unsigned int zb_count;
    char zb_arr[0]; //DuanzaoGroupZBInfo 数组
};

struct SPCmd_Duanzao
{
	/*ENUM Profession*/
    unsigned int profession;	//抽取职业,枚举Profession
    bool is_times_10; 		//是否为抽10次 true:抽10次 false:抽1次
};

struct SPCmd_DuanzaoRsp
{
    int err_code;
    
    int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct SPCmd_QueryAllShenbingList
{
    //占位
    int stub;
};

struct SPCmd_QueryAllShenbingListRsp
{
    int err_code;
    
    unsigned int shenbing_count;
    char zb_arr[1]; //DuanzaoGroupZBInfo arr
};


struct SPCmd_QueryTeamShenbingSuipian
{
    //占位
    unsigned int stub;
};

struct ShenbingSuipianInfo
{
    unsigned int zb_group_id;
    unsigned int suipian_id;
    unsigned int suipian_count;
};
struct SPCmd_QueryTeamShenbingSuipianRsp
{
    int err_code;

    int suipian_info_count;
    ShenbingSuipianInfo suipian_into_arr[0]; 
};

struct SPCmd_ShenbingHecheng
{
    unsigned int zb_group_id;
};



struct SPCmd_ShenbingHechengRsp
{
    int err_code;

    unsigned int zhuangbei_count;
    char data[0]; //ZhuangbeiDetail zb_detail[];
};


struct SPCmd_QueryTeamItemSuipian
{
    //占位
    int stub;
};

struct ItemSuipianInfo
{
    unsigned int item_id;
    unsigned int suipian_id;
    unsigned int suipian_count;
};
struct SPCmd_QueryTeamItemSuipianRsp
{
    int err_code;

    int suipian_info_count;
    ItemSuipianInfo suipian_into_arr[0]; 
};

struct SPCmd_ItemHecheng
{
    unsigned int item_id;
};

struct SPCmd_ItemHechengRsp
{
    int err_code;

    ItemInfo item_info;
};

struct SPCmd_Saodang
{
    //主任务副本id
    unsigned int task_id;

    //扫荡次数
    unsigned int saodang_time;
    //
    bool auto_fenjie_baizhuang;
    bool auto_fenjie_lanzhuang;
    bool auto_fenjie_huangzhuang;
};

struct SPCmd_SaodangRsp
{
    int err_code;

	
    unsigned int exp;				//经验
    unsigned int decomposed_count; 	//已分解的装备数量
    unsigned int item_count;		//道具
    ItemInfo item_arr[0];
	
	//item_arr后紧跟装备掉落
    //unsigned int zb_count;
    //ZhuangbeiDetail zb_detail_arr[0];
};


struct SPCmd_HeroLevelUp
{
    //要升级的英雄id
    unsigned int hero_id;
};

struct SPCmd_HeroLevelUpRsp
{
    int err_code;

    //升级后剩余银币
    unsigned int silver_end;

    //升级后还剩几个升级道具
    ItemInfo item_end;

    //升级后的英雄信息
    HeroDetail hero_detial;
};

struct SPCmd_HeroUpgrade
{
    //要升阶的英雄id
    unsigned int hero_id;
};

struct SPCmd_HeroUpgradeRsp
{
    int err_code;

    //升阶后剩余银币
    unsigned int silver_end;

    //升阶后还剩几个升阶道具
    ItemInfo item_end;

    //升阶后的英雄信息
    HeroDetail hero_detial;
};


struct SPCmd_ShenbingJieFengyin
{
    unsigned long long zb_id;			//需要解锁的装备
};

struct SPCmd_ShenbingJieFengyinRsp
{
    int err_code;
};

struct SPCmd_ShenbingJianding
{
    unsigned long long zb_id;
};

struct SPCmd_ShenbingJiandingRsp
{
    int err_code;

    unsigned int identified_attr_count;
    ZBAttr identified_attr_vec[0];
};

struct SPCmd_ZBJinglian
{
    unsigned long long zb_id;
    unsigned long long cailiao_zb_id;
};

struct SPCmd_ZBJinglianRsp
{
    int err_code;
	
    JinglianData jinglian_data;

	//回收材料
    int count;				//item数量
    ItemInfo item_arr[0];
};

struct SPCmd_ShenbingZhuankeJingwen
{
    unsigned long long zb_id;
};

struct SPCmd_ShenbingZhuankeJingwenRsp
{
    int err_code;

    ZBAttr jingwen_attr[2];
};


struct SPCmd_QueryArenaTeamBasicInfo
{
    //占位
    int stub;
};

struct ArenaTeamBasicInfo
{
    unsigned int free_fight_remainder_time; //自由竞技本日剩余次数
    unsigned int challenge_remainder_time; //挑战本日剩余次数

    unsigned int jifen; //当前竞技场总积分
    
    unsigned int buy_challenge_remainder_time; //当日还可购买多少次挑战
};
struct SPCmd_QueryArenaTeamBasicInfoRsp
{
    int err_code;

    ArenaTeamBasicInfo team_info;
};

struct SPCmd_QueryArenaMarket
{
    //占位
    int stub;
};

struct ArenaMarketItem
{
    //出售的商品类型和一单打包数量
    unsigned int item_type;
    unsigned int item_count;

    //所需积分数量
    unsigned int jifen_count;
};

struct SPCmd_QueryArenaMarketRsp
{
    int err_code;

    //
    unsigned int item_count;
    ArenaMarketItem item_arr[0];
};


struct SPCmd_RefreshArenaChallengeList
{
    //占位
    int stub;
};

struct ArenaChlgerHeroInfo
{
    unsigned int jifen; //战胜可得积分
    bool has_win; //是否已经战胜，战胜了就不能再打了
    
    HeroInfo hero_info;
};

struct SPCmd_RefreshArenaChallengeListRsp
{
    int err_code;

    
    unsigned int gold_balance; 		//当前金币余额
    unsigned int silver_balance; 	//当前银币余额

	//下次手动刷新列表需要消耗货币以及数量
    ItemInfo item_need;		//type:为0免费，或者ItemsType_Gold，ItemsType_Silver

    unsigned int hero_count;
    ArenaChlgerHeroInfo hero_arr[0];
};

struct SPCmd_QueryArenaChallengeList
{
    //占位
    int stub;
};

struct SPCmd_QueryArenaChallengeListRsp
{
    int err_code;

	//下次手动刷新列表需要消耗货币以及数量
    ItemInfo item_need;		//type:为0免费，或者ItemsType_Gold，ItemsType_Silver

    unsigned int hero_count;
    ArenaChlgerHeroInfo hero_arr[0];
};


struct SPCmd_ArenaChallenge
{
    unsigned int challenge_hero_id;
};

struct SPCmd_ArenaChallengeRsp
{
    int err_code;

    //战斗之后的积分
    unsigned int jifen_after_fight;
    
    // 脚本
	//char fightScript[1];
};

struct SPCmd_ArenaMarketBuy
{
    //
    unsigned int item_type;
    unsigned int item_count;
};

struct SPCmd_ArenaMarketBuyRsp
{
    int err_code;

    //购买成功后积分余额。如果购买失败，此字段无效
    unsigned int jifen_balance;
};

struct SPCmd_ArenaBuyChallengeTime
{
    //bool is_max_buy; //是否最大购买。就是一次性把剩余次数全部购买完
    //购买多少次
    unsigned int buy_time;
};

struct SPCmd_ArenaBuyChallengeTimeRsp
{
    int err_code;

    //购买后剩余挑战次数
    unsigned int challenge_remainder_time;
    //当日还可购买多少次
    unsigned int buy_challenge_remainder_time;

    //购买后的金币余额
    unsigned int gold_balance;
};




struct SPCmd_QueryZahuopu
{
    //占位
    int stub;
};

struct ZahuopuItem
{
    unsigned int item_type;
    unsigned int suipian_id; //为0表示是整个item，不是碎片
    unsigned int count; //碎片或item的数量
    bool already_got;
};
struct SPCmd_QueryZahuopuRsp
{
    int err_code;

    unsigned int silver_need_for_refresh; 	//下次手动刷新列表需要消耗银币数量
    unsigned int draw_need_coupons;			//抽取杂货铺道具时所需代购券
    unsigned int coupons_balance;			//当前剩余的代购劵数量

    ZahuopuItem item_arr[6];
};

struct SPCmd_RefreshZahuopu
{
    //占位
    int stub;
};

struct SPCmd_RefreshZahuopuRsp
{
    int err_code;

    
    unsigned int silver_balance;			//本次刷新后剩余银币数量
    unsigned int silver_need_for_refresh; 	//下次手动刷新列表需要消耗银币数量
    unsigned int draw_need_coupons;			//抽取杂货铺道具时所需代购券
    unsigned int coupons_balance;			//当前剩余的代购劵数量

    ZahuopuItem item_arr[6];
};

struct SPCmd_ZahuopuGetOneItem
{
    //占位
    int stub;
};

struct SPCmd_ZahuopuGetOneItemRsp
{
    int err_code;

    unsigned int get_index; 				//0~5
    unsigned int gold_balance;				//本次抽取后金币余额
    unsigned int coupons_balance;			//当前剩余的代购劵数量
};


struct SPCmd_QueryHellTaskHeroInfo
{
    //占位
    int stub;
};
struct SPCmd_QueryHellTaskHeroInfoRsp
{
    int err_code;
    
    unsigned int battle_time_remainder; //本日还剩几次
    unsigned int last_active_task_id; //当前进度到那个副本了
};

struct SPCmd_HellTaskBattle
{
    unsigned int task_id;
};

struct SPCmd_HellTaskBattleRsp
{
    int err_code;

	int fightNum;	// 战斗的次数
	// 脚本数据 数组
	char fightScript_array[1];

	//Battel_FightScript fightScript;
};


struct SPCmd_QueryYuanshenOfHero
{
    unsigned int hero_id; //支持查询别人团队的角色
};

struct SPCmd_QueryYuanshenOfHeroRsp
{
    int err_code;

    //
    HeroDetail hero_detail;
    
    //
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct SPCmd_ChurchLevelUp
{
    //占位
    int stub;
};

struct SPCmd_ChurchLevelUpRsp
{
    int err_code;

    //升级后教堂等级
    unsigned int church_level;
    
    //升级后还剩多少材料
    unsigned int dalishi_balance;
    unsigned int huagangyan_balance;
    unsigned int mucai_balance;
    unsigned int shuijinshi_balance;

    //升级后还剩多少银币
    unsigned int silver_balance;
};

struct SPCmd_SkillLevelUp
{
    unsigned int owner_hero_id;
    unsigned int skill_id;
};

struct SPCmd_SkillLevelUpRsp
{
    int err_code;

    unsigned int new_skill_level;

    ////升级后需要刷新该技能书的数量
    ItemInfo skill_book_end;

    //银币还剩多少
    unsigned int silver_balance;
};

struct SPCmd_GetZhuangbeiAlbum
{
	/*ENUM Profession*/
    unsigned int profession;	//查询职业,枚举Profession
};

struct SPCmd_GetZhuangbeiAlbumRsp
{
    int err_code;

    unsigned int legend_count;		//传奇装备数量
	unsigned int xiyou_count;		//稀有装备数量
	
    char zb_arr[1]; 	//DuanzaoGroupZBInfo arr
};



struct CityGuardAnotherHeroShanghai
{
    unsigned int profession_id;
    char name[HeroNameMaxLen+1];

    unsigned int shanghai; //伤害值
};

struct SPCmd_CityGuardProcessNotify
{
    int err_code;

    //客户端需要用多少秒来播放伤害过程。
    unsigned int play_time; 

    //在play_time时间段内boss的血量需要减少这么多
    unsigned int boss_hp_start; //为0表示boss已被击杀
    unsigned int boss_hp_end;

    //需要播放的伤害
    unsigned int hero_count;
    CityGuardAnotherHeroShanghai another_hero[0]; //变长
};

struct SPCmd_getCityGuardCurrentStat
{
    //占位
    int stub;
};


struct cityGuardStatInfo
{
    bool is_opening;
    
    unsigned int boss_blood_total; //boss原始总血量

    unsigned int time_before_open; //距离下次守城还剩多少秒
};
struct SPCmd_getCityGuardCurrentStatRsp
{
    int err_code;

    cityGuardStatInfo stat;

    //是否有我的奖励还没有领取
    bool have_reward;
};

struct SPCmd_cityGuardBattle
{
    //是否是跳过CD时间
    bool is_skil_cd;
};


struct CityGuardBattleData
{
    //是否boss已被击杀了，如果已被击杀，则后面的战斗脚本无效。
    bool is_boss_dead;
    
    //本次战斗后的个人统计数据
    unsigned int total_shanghai; //对boss的总伤害
    unsigned int shanghai_order; //当前伤害排名

	char fightScript[0];
};

struct SPCmd_cityGuardBattleRsp
{
    int err_code;

    //跳过冷却时间消耗金币后的剩余金币
    //如果进入战斗时boss已死，则不会扣金币
    unsigned int gold_balance;
    
    CityGuardBattleData battle_data;
};

//退出极限守城请求包
struct SPCmd_QuitCityGuard
{
    //占位
    int stub;
};

//退出极限守城返回包
struct SPCmd_QuitCityGuardRsp
{
    int err_code;
};


struct SPCmd_getCityGuardLastDayReport
{
    //占位
    int stub;
};

#define CITYGUARD_HERO_ORDER_COUNT 20
#define CITYGUARD_LUCKY_COUNT 10
struct CityGuardReport
{
    unsigned int cityguard_total_time; //守城总耗时

    HeroInfo hero_kill_boss; //击杀boss者
    ItemInfo hero_kill_reward; //击杀者奖励
    HeroInfo hero_order[CITYGUARD_HERO_ORDER_COUNT]; //输出伤害前20名。
    ItemInfo hero_order_reward[CITYGUARD_HERO_ORDER_COUNT]; //输出伤害前20名分别得到什么奖励
    HeroInfo hero_lucky[CITYGUARD_LUCKY_COUNT]; //10名幸运奖
    ItemInfo hero_lucky_reward; //幸运奖奖品

	ItemInfo join_reward;  	//参与奖
};

struct SPCmd_getCityGuardLastDayReportRsp
{
    int err_code;

    //是否有我的奖励。
    bool have_reward;

    CityGuardReport lastday_report;
    
};

struct SPCmd_GetMyCityGuardReward
{
    //占位
    int stub;
};

struct SPCmd_GetMyCityGuardRewardRsp
{
    int err_code;

	unsigned int count;
    ItemInfo item_arr[0];
};


/*  奖励协议  */
//奖励类型
enum BonusType {
	BonusType_Invalid 		= 0,	//无效类型
	BonusType_DailyLogin 	= 1,	//每日登陆奖励
	BonusType_Level 		= 2,	//等级奖励 
	BonusType_DayWelfare  	= 3,	//今日福利(日常任务)
	BonusType_Reward 		= 4,	//悬赏任务(周常任务)
	BonusType_All 			= 10,	//所有任务(查询时候用)
};

//单个邮件信息
struct BonusInfo {
    unsigned int type;			//BonusType
    bool is_get;		   		//是否领取奖励 false:未领取, true:已领取
    bool enable;		   		//是否可以领取奖励 false:不能领取 true:可以领取

	/*ps: bonus_id 在 type 分别为
		BonusType_DailyLogin: 领取日期
		BonusType_Level: 等级
	*/
    unsigned int bonus_id; 		//每种奖励的ID


	/* 奖励内容为变长 后面紧跟 */
	//unsigned int vitality;		//0:没有体力信息 >0: 补偿的体力值
	//unsigned int item_count;		//道具材料
	//ItemInfo item_arr[0];			//ItemInfo

	//unsigned int item_suipian_count;				//道具碎片包括宝石碎片和技能书碎片
	//ItemSuipianInfo item_suipian_arr[0];			//ItemSuipianInfo

	//unsigned int zb_suipian_count;				//装备碎片
	//ShenbingSuipianInfo zb_suipian_arr[0];		//ShenbingSuipianInfo

	//unsigned int gift_bag_count;		//礼包
	//GiftBag gift_bag_arr[0];			//GiftBag

	//unsigned int ran_zb_count;	//随机装备数量
	//unsigned int zb_count;		//装备数量
	//char zb_arr[0];				//DuanzaoGroupZBInfo

	/* type为BonusType_DayWelfare或BonusType_Reward 有以下类容*/
	//unsigned int vip_level; 			//>0:领取多倍奖励的vip等级分界线 0:只有1倍奖励 
	//unsigned int multiple;			//倍数
	//char description[1]; //描述内容为变长, 以'\0'结束

	BonusInfo() {
		memset(this, 0, sizeof(BonusInfo));
	};
};



//查询奖励请求包
struct SPCmd_QueryBonus {
    unsigned int type;		//BonusType
};

//查询奖励返回包
struct SPCmd_QueryBonusRsp {
    unsigned int err_code;

    unsigned int type[4];		//4种不同奖励的BonusType
    int count[4];				//4种bonus的数量
    char bonus_arr[0]; 			//BonusInfo
};

//获取奖励的请求包
struct SPCmd_GetBonus {
    unsigned int type;		//BonusType
    unsigned int id;		//领取id
};

//获取奖励返回包
struct SPCmd_GetBonusRsp {
    unsigned int err_code;
	
    unsigned int type;		//BonusType
    unsigned int id;		//领取id
    
	unsigned int zb_count;
	char detail[1];			//装备detail
};

//触发奖励通知包
struct SPCmd_SendBonusNotify {
    unsigned int err_code;
	
    unsigned int type;		//BonusType
	unsigned int id;		//客户端收到此消息后，把此id的奖励置成可以领取奖励状态
};


/* 邮件 */
//邮件类型
enum EmailType {
	EmailType_Bonus 		= 0,	//系统奖励  有附件的
	EmailType_Compensate 	= 1,	//系统补偿  有附件的
	EmailType_Notice 		= 2,	//通知公告 
	EmailType_BeFriend 		= 3,	//好友邀请 
	EmailType_UnFriend 		= 4,	//删除好友 
	EmailType_Player 		= 5,	//个人邮件
};

//单个邮件信息
struct EmailInfo {
    unsigned int type;			//EmailType
    bool is_read;		   		//是否阅读 false:未读, true:已读
    unsigned int id; 			//邮件ID
    unsigned int addresser_id;	//发件人id 0:为系统 >0:玩家
    char addresser[HeroNameMaxLen +1];	//发件人姓名
    char content[1]; 	//邮件描述或者内容

	//邮件内容为变长, 以'\0'结束, 后面紧跟内容提要, 也以'\0'结束
    //char summary[1]; 	//邮件提要
		
	//bool is_have_attach;		//是否是附件 false: 通知类邮件(没有物品), true: 奖励/补偿类邮件(有物品)		
	/* 以下为附件信息 */
    
    
	//unsigned int vitality;		//0:没有体力信息 >0: 补偿的体力值
	//unsigned int item_count;		//道具材料
    //ItemInfo item_arr[0];			//ItemInfo

	//unsigned int item_suipian_count;				//道具碎片包括宝石碎片和技能书碎片
    //ItemSuipianInfo item_suipian_arr[0];			//ItemSuipianInfo

	//unsigned int zb_suipian_count;				//装备碎片
    //ShenbingSuipianInfo zb_suipian_arr[0];		//ShenbingSuipianInfo

	//unsigned int gift_bag_count;		//礼包
    //GiftBag gift_bag_arr[0];			//GiftBag

    //unsigned int ran_zb_count;	//随机装备数量
    //unsigned int zb_count;		//装备数量
	//char zb_arr[0];				//DuanzaoGroupZBInfo

	EmailInfo() {
		memset(this, 0, sizeof(EmailInfo));
	};
};

//查询邮件请求包
struct SPCmd_QueryEmail {
    unsigned int stub;		//临时
};

//查询邮件返回包
struct SPCmd_QueryEmailRsp {
    unsigned int err_code;

    int count;			//email数量
    char email_arr[0]; 	//EmailInfo
};

//阅读邮件请求包
struct SPCmd_ReadEmail {
    unsigned int id;
};

//阅读邮件返回包
struct SPCmd_ReadEmailRsp {
    unsigned int err_code;
	
	unsigned int zhuangbei_count;
	char detail[1];		//装备detail
};

//写邮件请求包
struct SPCmd_WriteEmail {
	unsigned int receiver_id;	//发件人id 0:广播 >0:玩家
    char content[CONTENT_MAX_LEN +1]; 	//邮件描述或者内容
};

//写邮件返回包
struct SPCmd_WriteEmailRsp {
    unsigned int err_code;
};

//删邮件请求包
struct SPCmd_DeleteEmail {
    unsigned int id;
};

//删邮件返回包
struct SPCmd_DeleteEmailRsp {
    unsigned int err_code;
};

//推送邮件通知包
struct SPCmd_PushEmail {
    unsigned int err_code;

    int count;			//email数量
    char email_arr[0]; 	//EmailInfo
};



/* 礼包 */
//礼包
struct GiftBag {
    unsigned int id;		//礼包id
    unsigned int count; 	//礼包数量
};

//礼包
struct GiftBagInfo {
    unsigned int vitality;		//0:没有体力信息 >0: 补偿的体力值
    unsigned int item_count;		//道具材料
    ItemInfo item_arr[0];			//ItemInfo

	//礼包道具为变长, 后面跟碎片装备等
	//unsigned int item_suipian_count;				//道具碎片包括宝石碎片和技能书碎片
    //ItemSuipianInfo item_suipian_arr[0];			//ItemSuipianInfo

	//unsigned int zb_suipian_count;				//装备碎片
    //ShenbingSuipianInfo zb_suipian_arr[0];		//ShenbingSuipianInfo

	//unsigned int gift_bag_count;		//礼包
    //GiftBag gift_bag_arr[0];			//GiftBag

    //unsigned int zhuangbei_count;		//装备数量
	//char detail[0];				//ZhuangbeiDetail
};


//查询礼包请求包
struct SPCmd_QueryGiftBag {
    unsigned int stub;		//临时
};

//查询礼包返回包
struct SPCmd_QueryGiftBagRsp {
    unsigned int err_code;

    int count;				//giftbag数量
    char gift_bag_arr[0]; 	//GiftBag
};

//打开礼包请求包
struct SPCmd_GetGiftBag {
    unsigned int id;
};

//打开礼包返回包
struct SPCmd_GetGiftBagRsp {
    unsigned int err_code;
	
    GiftBagInfo info;
};



/*  巅峰对决  */
//巅峰对决比赛状态
enum TopArenaStat
{
    TopArenaStat_Invalide    = 0,
    TopArenaStat_GuessOpen   = 1,
    TopArenaStat_Finished    = 2,
};

//巅峰对决比赛场次
enum TopArenaRoundType
{
	TopArenaRoundType_NUL = 0,
 	TopArenaRoundType_8_4 = 1,
 	TopArenaRoundType_4_2 = 2,
 	TopArenaRoundType_2_1 = 3,
};

//巅峰对决赌注
enum TopArenaWagerType
{
 	TopArenaWagerType_Low 	= 1,	//8000银币
 	TopArenaWagerType_High 	= 2,	//16000银币
};

//巅峰对决小组成员信息
struct TopArenaInfo {
	//unsigned short round;    	//所处比赛场次, 0:未开始, 1:8进4, 2:4进2, 3:2进1
	HeroInfo hero_a;			//英雄信息
	HeroInfo hero_b;	    
	bool winer;					//true: a胜 false: b胜
};

//
struct TopArenaGuessInfo {
	bool is_joined;				//是否参与竞猜, true:已经参加, false:没有参加
	TopArenaInfo player;		//选手比赛结果
	unsigned short guess_winer;	//0: a胜 1: b胜
	unsigned short result;		//0:结果还未出来 1: 竞猜正确 2: 竞猜错误
	unsigned int wager;			//赌注
};


//获取巅峰对决信息请求包
struct SPCmd_TopArenaQueryStatus{
    unsigned int stub;			//临时
};


//获取巅峰对决信息返回包
struct SPCmd_TopArenaQueryStatusRsp {
    unsigned int err_code;

	unsigned int next_top_arena_time;	//下次巅峰对决开启时间 0:活动进行中, >0:活动未开始

	/* 最多3组竞猜消息  TopArenaStat*/
	unsigned int stat[3];			//下标0为8进4， 下标1为4进2， 下标2为2进1
	
	/* 最多3组竞猜消息*/
	TopArenaGuessInfo guess[3];		//下标0为8进4， 下标1为4进2， 下标2为2进1
	
	TopArenaInfo player_arr_8_4[4];	//8进4选手
	TopArenaInfo player_arr_4_2[2]; //4进2选手
	TopArenaInfo player_arr_2_1[1]; //2进1选手
};


//竞猜请求包
struct SPCmd_TopArenaGuess {
	unsigned short round;    	//当前比赛场次, 0:未开始, 1:8进4, 2:4进2, 3:2进1
	unsigned short group;		//比赛小组, 0:第1组 1:第2组 2:第3组 3:第4组
	bool guess_winer;			//true: a胜 false: b胜
	unsigned int wager;			//赌注
};


//竞猜返回包
struct SPCmd_TopArenaGuessRsp {
    unsigned int err_code; 
	unsigned int silver_balance; 	//当前剩余银币余额
};

//回放请求包
struct SPCmd_TopArenaReplay{
	unsigned short round;    	//比赛场次, 0:未开始, 1:8进4, 2:4进2, 3:2进1
	unsigned short group;		//比赛小组, 0:第1组 1:第2组 2:第3组 3:第4组
	unsigned short times;		//比赛次数, 0:第1次 1:第2次 2:第3次
};

//回放返回包
struct SPCmd_TopArenaReplayRsp {
    unsigned int err_code;
	
    char fight_script[1];		//战斗脚本
};

//查询竞猜请求包
struct SPCmd_TopArenaQueryGuess {
	unsigned short round;    	//当前比赛场次, 0:未开始, 1:8进4, 2:4进2, 3:2进1
	unsigned short group;		//比赛小组, 0:第1组 1:第2组 2:第3组 3:第4组
};

//查询竞猜返回包
struct SPCmd_TopArenaQueryGuessRsp {
    unsigned int err_code; 
	
	unsigned int hero_value_a; 	//当前身价 MIN_HERO_VALUE -- MAX_HERO_VALUE
	unsigned int hero_value_b; 	//当前身价            16W -- 9000W
};


/* 关卡评级 */
#define MAX_TASK_NUM	 40
#define MAX_CHAPTER_NUM	 4

//关卡章节类型
enum RateStarType {
    RateStarType_Invalid = 0,	//无效
    RateStarType_Task = 1,		//关卡
    RateStarType_Chapter = 2,	//章节
};

//关卡评级等级
enum RateStarLevel {
    RateStarLevel_Invalid = 0,	//无效
    RateStarLevel_Cuprum = 1,	//铜星
    RateStarLevel_Silver = 2,	//银星  //章节奖励没有银星
    RateStarLevel_Gold = 3,		//金星
};

//关卡评级状态信息
struct RateStarInfo {
    unsigned short star;		//RateStarLevel 星级 
    bool is_get_cuprum;			//铜星领取状态 false: 未领取 true:领取
    bool is_get_silver;			//银星领取状态 章节奖励 此标志位没用
    bool is_get_gold;			//金星领取状态 

	RateStarInfo() {
		memset(this, 0, sizeof(RateStarInfo));
	}
};

//关卡评级状态信息
struct RateBonus {
    unsigned short star;		//RateStarLevel 星级 
    unsigned int exp;			//奖励经验 0:没有此奖励, >0:有经验
    
    unsigned short item_count;
	//变长
    //ItemInfo item_arr[0];
};

//查询关卡评级状态请求包
struct SPCmd_QueryRateTask {
    unsigned int stub;			//临时
};

//查询关卡评级状态返回包
struct SPCmd_QueryRateTaskRsp {
    unsigned int err_code;

    unsigned short task_max;					//关卡评级 1-40
    RateStarInfo task_info[MAX_TASK_NUM]; 		//关卡评级 
    unsigned short chap_max;					//关卡评级 1-4
    RateStarInfo chap_info[MAX_CHAPTER_NUM];	//章节评级
	
};

//查询评级奖励请求包
struct SPCmd_QueryRateTaskBonus {
    unsigned short type;		//RateStarType
    unsigned short task_id;		//关卡:1-40, 章节:1-4
};

//查询评级奖励返回包
struct SPCmd_QueryRateTaskBonusRsp {
    unsigned int err_code;

    unsigned short bonus_count;	//星级对应奖励
    RateBonus bonus_arr[0];		//奖励
};

//领取评级奖励请求包
struct SPCmd_GetRateTaskBonus {
    unsigned short type;		//RateStarType 0:关卡评级, 1:章节评级
    unsigned short task_id;		//关卡:1-40, 章节:1-4
    unsigned short star;		//星级
};

//领取评级奖励返回包
struct SPCmd_GetRateTaskBonusRsp {
    unsigned int err_code;
};



/* VIP特权信息 */
struct VipPrivilegeInfo
{
	unsigned int buy_vitality_num;		//购买体力次数
	unsigned int sweep_num;				//副本扫荡次数
	unsigned int buy_arena_num;			//购买竞技场挑战次数
	unsigned int over_creep_num;		//关卡跳过小怪数
	unsigned int cast_silver_num;		//铸银次数

	//下面暂时未做
	bool honor_exchange;				//是否开启荣誉兑换功能 false:没有 true:开启
	unsigned int refresh_reward_num;	//免费刷新悬赏任务次数
};

//查询VIP特权状态请求包
struct SPCmd_QueryVipPrivilege {
    unsigned int stub;			//临时
};

//查询VIP特权状态返回包
struct SPCmd_QueryVipPrivilegeRsp {
    unsigned int err_code;

	VipPrivilegeInfo deft_info;			//当前vip默认的特权信息（满的）
	VipPrivilegeInfo priv_info;			//当前vip特权信息（剩余的）
};

/* 升级神兵 */
//升级神兵请求包
struct SPCmd_UpgradeShenbing {
    unsigned long long zb_id;
    unsigned long long cailiao_zb_id;
};

//升级神兵返回包
struct SPCmd_UpgradeShenbingRsp {
    unsigned int err_code;

	//回收材料
    ItemInfo item_arr[5];
	
	char detail[1];		//装备detail
};

/* 解锁助手 */
//解锁助手请求包
struct SPCmd_UnlockAssistant {
    unsigned int assistant_id;
};

//解锁助手返回包
struct SPCmd_UnlockAssistantRsp {
    unsigned int err_code;

	unsigned int honor;		//剩余荣誉
};


/* 体力赠送活动 */
//赠送活动类型
enum ActivityType {
	ActivityType_12_14 		= 1,		//体力赠送活动 12:00-14:00
    ActivityType_18_20 		= 2,		//体力赠送活动 18:00-20:00
};

//单个活动信息
struct ActivityInfo {
    unsigned int id; 					/* ActivityType */
    bool is_get;		   				//是否领取活动 false:未领取, true:已领取
    unsigned int open_time;				//活动开启时间
    unsigned int close_time;			//活动结束时间
};

//查询体力赠送活动请求包
struct SPCmd_QueryGiftActivity {
    unsigned int stub;					//临时
};

//查询体力赠送活动返回包
struct SPCmd_QueryGiftActivityRsp {
    unsigned int err_code;

	unsigned int count;
	ActivityInfo act_arr[0];
};

//领取体力赠送活动请求包
struct SPCmd_GetGiftActivity {
    unsigned int id;					/* ActivityType */
};

//领取体力赠送活动返回包
struct SPCmd_GetGiftActivityRsp {
    unsigned int err_code;

	unsigned int vit_current;			//当前剩余体力
};


/* 击杀怪物累计 */
//累计类型
enum AccumulateType {
	Accumulate_Common_Monster 	= 1, 	//普通小怪
	Accumulate_Elite_Monster	= 2,	//精英怪
};

//查询击杀怪物奖励请求包
struct SPCmd_QueryKilledMonsterBonus {
    unsigned int stub;					//临时
};

//查询击杀怪物奖励返回包
struct SPCmd_QueryKilledMonsterBonusRsp {
    unsigned int err_code;

	unsigned int common_bonus_count;
	unsigned int elite_bonus_count;

	ItemInfo item_arr[0];
};

//查询击杀怪物累计请求包
struct SPCmd_QueryKilledMonster {
    unsigned int stub;					//临时
};

//查询击杀怪物累计返回包
struct SPCmd_QueryKilledMonsterRsp {
    unsigned int err_code;

	unsigned int common_monster_count;
	unsigned int elite_monster_count;
};

//领取击杀怪物累计奖励请求包
struct SPCmd_GetKilledMonsterBonus {
    unsigned int type;					/* AccumulateType */
};

//领取击杀怪物累计奖励返回包
struct SPCmd_GetKilledMonsterBonusRsp {
    unsigned int err_code;
};


/* 铸银 */
//铸银请求包
struct SPCmd_CastSilver {
    unsigned int stub;					//临时
};

//铸银返回包
struct SPCmd_CastSilverRsp {
    unsigned int err_code;

    unsigned int gold_balance;
	unsigned int silver_balance;
};


/* 传奇录 */
//查询传奇录请求包
struct SPCmd_QueryLegendNote
{
    unsigned int stub;				//临时
};

//查询传奇录返回包
struct SPCmd_QueryLegendNoteRsp
{
    int err_code;
	
    unsigned int times;				//剩余挑战次数
};

//挑战传奇录请求包
struct SPCmd_ChallengeLegendNote
{
    unsigned int group_id;			//挑战装备id
};

//挑战传奇录返回包
struct SPCmd_ChallengeLegendNoteRsp
{
    int err_code;
	
    char fight_script[1];		//战斗脚本
};


/* 血色教堂 */
//自我挑战难度类型
enum SelfChlgDiffType {
	SelfChlgDiffType_NULL		= 0,		//没有选择难度
	SelfChlgDiffType_Common		= 1,		//普通
	SelfChlgDiffType_Difficult	= 2,		//困难
	SelfChlgDiffType_Hell		= 3,		//地狱
	SelfChlgDiffType_Nightmare	= 4,		//梦魇
};

//自我挑战加成类型
enum SelfChlgGainType {
	SelfChlgGainType_HP			= 1,		//生命
	SelfChlgGainType_Attack		= 2,		//攻击
	SelfChlgGainType_Defense	= 3,		//护甲
};

//自我挑战加成程度
enum SelfChlgGainDegree {
	SelfChlgGainDegree_Low		= 1,		
	SelfChlgGainDegree_High		= 2,		
};

//自我挑战信息
struct SelfChlgInfo
{
	bool b_on_the_way;				//true:正在闯关状态; false: 未进入闯关
    unsigned int times;				//剩余挑战次数

    unsigned int energy;			//剩余能量
    unsigned int hp_gain;			//生命加成
    unsigned int attack_gain;		//攻击加成
    unsigned int defense_gain;		//防御加成

	unsigned int difficulty;		///* SelfChlgDiffType */闯关难度
	unsigned int checkpoints;		//已闯关卡
};

//自我挑战排名信息
struct SelfChlgOrder
{
    HeroInfo hero_info;
    unsigned int order;
	unsigned int checkpoints;
	ItemInfo item;				//排名奖励
};

//查询自我挑战信息请求包
struct SPCmd_QuerySelfChallengeInfo
{
    unsigned int stub;				//临时
};

//查询自我挑战信息返回包
struct SPCmd_QuerySelfChallengeInfoRsp
{
    int err_code;
	
    unsigned int open_time;			//开启时间
    unsigned int close_time;		//关闭时间
    
	SelfChlgInfo info;

};

//查询自我挑战排名请求包
struct SPCmd_QuerySelfChallengeOrder
{
    unsigned int difficulty;		///* SelfChlgDiffType */闯关难度
};

//查询自我挑战排名返回包
struct SPCmd_QuerySelfChallengeOrderRsp
{
    int err_code;

	unsigned int count;
	SelfChlgOrder order_arr[0];
};

//自我挑战闯关请求包
struct SPCmd_SelfChallenge
{
    unsigned int difficulty;		///* SelfChlgDiffType */闯关难度
};

//自我挑战闯关返回包
struct SPCmd_SelfChallengeRsp
{
    int err_code;

	unsigned int energy;			//剩余能量
    unsigned int checkpoints;		//关卡
	bool is_have_box;				//true: 每5关有3个宝箱奖励选择; false: 无宝箱
	unsigned int silver;			//>0: 每20关有相应银币奖励; 0:无银币奖励

	GiftBag gift_bag;				//有些关卡有随机礼包奖励  id,count>0 有礼包
	
    char fight_script[1];			//战斗脚本
};

//自我挑战能量加成请求包
struct SPCmd_SelfChallengeGain
{
    unsigned int type;				///* SelfChlgGainType */加成类型
    unsigned int degree;			///* SelfChlgGainDegree */加成程度
};

//自我挑战能量加成返回包
struct SPCmd_SelfChallengeGainRsp
{
    int err_code;

    unsigned int energy;			//剩余能量
    unsigned int hp_gain;			//生命加成
    unsigned int attack_gain;		//攻击加成
    unsigned int defense_gain;		//防御加成
};

//获取自我挑战宝箱奖励请求包
struct SPCmd_GetSelfChallengeBox
{
    unsigned int stub;				//临时
};

//领取自我挑战宝箱奖励返回包
struct SPCmd_GetSelfChallengeBoxRsp
{
    int err_code;
	
	ItemInfo item;					//得到的奖励
	ItemInfo item_arr[2];			//未得到的奖励
};


/* 守护之灵 */
//守护之灵信息
struct ProtectSpiritInfo
{
	unsigned int spirit_id;			//守护之灵id
    unsigned int profession_id;		//职业
    unsigned int defend_time;		//守护时间
    unsigned int level;				//等级
    char name[HeroNameMaxLen+1];	//名称

	/* PS: 在profession_id 为1-3 且 玩家hero_id==spirit_id时有效 */
	bool is_get;					//领取奖励状态 true:已领取; false: 未领取
};

//查询守护之灵信息请求包
struct SPCmd_QueryProtectSpirit
{
    unsigned int chapter;			//章节id: 1-4
};

//查询守护之灵信息返回包
struct SPCmd_QueryProtectSpiritRsp
{
    int err_code;

	HeroInfo first_spirit;
	ProtectSpiritInfo current_spirit;

	bool is_loser;					//true: 24小时内被击败的守护之灵; false:不是
	
	/* PS: 玩家hero_id!=spirit_id时有效 */
	bool is_worship;				//膜拜状态 true:已膜拜; false: 未膜拜
};

//挑战守护之灵请求包
struct SPCmd_ChallengeProtectSpirit
{
    unsigned int chapter;			//章节id: 1-4
};

//挑战守护之灵返回包
struct SPCmd_ChallengeProtectSpiritRsp
{
    int err_code;
	
    char fight_script[1];			//战斗脚本
};

//领取守护之灵奖励请求包
struct SPCmd_GetProtectSpiritBonus
{
    unsigned int chapter;			//章节id: 1-4
};

//领取守护之灵奖励返回包
struct SPCmd_GetProtectSpiritBonusRsp
{
    int err_code;

	short item_count;
	ItemInfo item_arr[0];
};

//膜拜守护之灵请求包
struct SPCmd_WorshiProtectSpirit
{
    unsigned int chapter;			//章节id: 1-4
};

//膜拜守护之灵返回包
struct SPCmd_WorshiProtectSpiritRsp
{
    int err_code;
	
    GiftBag gift_bag;				//膜拜获取礼包
};


/* 探索 */
//查询探索信息请求包
struct SPCmd_QueryDiscoverInfo
{
    unsigned int stub;				//临时
};

//查询探索信息返回包
struct SPCmd_QueryDiscoverInfoRsp
{
    int err_code;

	unsigned int remainder_times;	//探索剩余次数
	unsigned int discovered_times;	//当日已经探索次数
    unsigned int destroy_seal;		//摧毁封印数
    unsigned int unlocked_seal;		//此轮已解封装备数, 每轮点亮6个石碑后解封重新刷新为0
   	unsigned int last_modify_timestamp;	//最后一次回复探索次数的时间戳 如果次数已经满了 则不管
};

//探索请求包
struct SPCmd_Discover
{
    bool akey_all;					//false: 单次探索, true:一键探索; default:true
};

//探索返回包
struct SPCmd_DiscoverRsp
{
    int err_code;

	bool is_destroyed;				//是否摧毁封印
	unsigned int remainder_times;	//剩余探索次数
	unsigned int discovered_times;	//当日已经探索次数
   	unsigned int last_modify_timestamp;	//最后一次回复探索次数的时间戳 如果次数已经满了 则不管

	unsigned int count;
	ItemInfo item_arr[0];
};

//探索解封印请求包
struct SPCmd_UnlockDiscoverSeal
{
	/*ENUM Profession*/
    unsigned int profession;		//职业,枚举Profession
};

//探索解封印返回包
struct SPCmd_UnlockDiscoverSealRsp
{
    int err_code;

    unsigned int destroy_seal;		//摧毁封印数
    unsigned int unlocked_seal;		//此轮已解封装备数, 每轮点亮6个石碑后解封重新刷新为0

	unsigned int count;
    char data[1];					//variable  array of ZhuangbeiDetail
};

/* 鉴定装备 */
//鉴定查询请求包
struct SPCmd_QueryAssessWeapon
{
    unsigned int group_id;				//需要解锁的装备group id
};

//鉴定查询返回包
struct SPCmd_QueryAssessWeaponRsp
{
    int err_code;
	
    char material_zb[0]; 				//DuanzaoGroupZBInfo
};

//鉴定装备请求包
struct SPCmd_AssessWeapon
{
    unsigned long long zb_id;			//需要解锁的装备
    unsigned long long material_id;		//材料装
};

//鉴定装备返回包
struct SPCmd_AssessWeaponRsp
{
    int err_code;
	
	//回收材料
    unsigned int count;
    ItemInfo item_arr[0];
};


/* 洗炼 */
//查询洗炼请求包
struct SPCmd_QueryPurify
{
    unsigned int stub;				//临时
};

//查询洗炼返回包
struct SPCmd_QueryPurifyRsp
{
    int err_code;

	unsigned int purify_free_times;	//剩余免费次数
	unsigned int purified_times;	//已洗炼的次数
};

//洗炼请求包
struct SPCmd_Purify
{
    unsigned long long id;			//要洗炼的装备id
    unsigned int hole;				//要洗炼属性孔 1~5
};

//洗炼返回包
struct SPCmd_PurifyRsp
{
    int err_code;

	unsigned int purify_hole;		//有多少孔是打开的
	ZBAttr	attr_arr[0];			//属性列 最多5孔
};

//确认洗炼请求包
struct SPCmd_ConfirmPurify
{
    unsigned long long id;			//要洗炼的装备id
    bool is_confirm;				//true: 确认; false: 取消 
};

//确认洗炼返回包
struct SPCmd_ConfirmPurifyRsp
{
    int err_code;
};


/* 产出 */
//产出信息
struct OriginInfo
{
    unsigned int group_id;			//装备group
	unsigned int task_id;			//任务关卡
};

//查询产出请求包
struct SPCmd_QueryOrigin
{
    unsigned int stub;				//临时
};

//查询产出返回包
struct SPCmd_QueryOriginRsp
{
    int err_code;

    unsigned int count;
    OriginInfo origin_arr[0];
};


/* 玩家其他信息 */
//查询英雄其他信息请求包
struct SPCmd_QueryHeroOtherInfo
{
    unsigned int stub;				//临时
};

//查询英雄其他信息返回包
struct SPCmd_QueryHeroOtherInfoRsp
{
    int err_code;

    unsigned int toplist_order;		//英雄榜排名
};


/* 公告 */
//公告活动类型
enum NoticeActivityType{
    NoticeActivityType_Invalid 		= 0,		//无效类型
    NoticeActivityType_SilverStar 	= 1,		//银星
    NoticeActivityType_GoldStar 	= 2,		//金星
};

//公告活动信息
struct NoticeActivityInfo {
	unsigned short type;			//NoticeActivityType
	
	/* 活动时间为变长 以'\0'结束 */
	//char activty_title[0];		//活动标题
	
	/* 后面紧跟活动时间内容 也以'\0'结束 */
	//char activty_time[0];			//活动时间
	//char activity_content[0];		//活动内容
};

//公告信息
struct NoticeInfo {
    unsigned short activity_count;		//活动说明个数
    NoticeActivityInfo activity_arr[0];	//NoticeActivityInfo

};

//查询公告请求包
struct SPCmd_QueryNotice
{
    unsigned int stub;				//临时
};

//查询公告返回包
struct SPCmd_QueryNoticeRsp
{
    int err_code;
    NoticeInfo info;	//公告信息	
};


/* 商城 */
//售卖类型 PS: 邮件、礼包、奖励也使用这个类型
enum AttachType {
	AttachType_Gold 		= 0,	//金币
	AttachType_Silver 		= 1,	//银币
	AttachType_Item 		= 2,	//材料
	AttachType_Diamond 		= 3,	//宝石
	AttachType_Props 		= 4,	//道具 
	AttachType_PropsDebris 	= 5,	//道具碎片
	AttachType_ArmsDebris 	= 6,	//装备碎片
	AttachType_Arms			= 7,	//装备
	AttachType_GiftBag		= 8,	//礼包
	AttachType_Vitality		= 9,	//体力
};

//购买限制类型 
enum LimitType {
	LimitType_VIPLvl 		= 0,	//VIP等级
	LimitType_MainHeroLvl 	= 1,	//主角等级
	LimitType_MainSoulLvl 	= 2,	//主角元神等级
};

// 售卖物品
union AttachItem {
	ItemInfo item;					//道具材料 AttachType <= AttachType_Props
	ItemSuipianInfo item_debris;	//道具碎片 AttachType_PropsDebris
	ShenbingSuipianInfo zb_debris;	//装备碎片 AttachType_ArmsDebris
	GiftBag gift;					//礼包 AttachType_GiftBag
	unsigned int vitality;			//体力 AttachType_Vitality
};	


struct MarketItem {
    unsigned int id;

	/*售卖物品类型 暂定只有材料 道具 碎片 礼包 体力等
	  其他类型全部放入到礼包之中*/
    unsigned short sell_type;		//AttachType
    AttachItem sell_item;			//售卖物品

    bool is_unlimit;				//是否限制购买次数 true: 无限; false: 有限
    unsigned int sell_count;		//购买次数 is_unlimit=false有效

	/* 购买所需货币 暂定只有金币银币即AttachType_Gold或者AttachType_Silver */
    unsigned short need_type;		//AttachType
    AttachItem need_item;			//售卖物品

    unsigned short discount;		//折扣 1~100 default:100
    unsigned int sale_time;			//出售倒计时 0:没有倒计时 >0:结束系统时间秒数
    
    unsigned short level_type;		//LimitType
    unsigned int level;				//等级

    unsigned int bought_times;		//该商品已经购买次数

	MarketItem() {
		memset(this, 0, sizeof(MarketItem));
	}
};

//查询商城请求包
struct SPCmd_QueryMarket
{
    unsigned int stub;			//临时
};

//查询商城返回包
struct SPCmd_QueryMarketRsp
{
    int err_code;

    unsigned short item_count;
    MarketItem item_arr[0];
};

//购买商城物品请求包
struct SPCmd_BuyItemInMarket
{
    unsigned int id;
    unsigned short buy_times;		//购买次数 默认为1
};

//购买商城物品返回包
struct SPCmd_BuyItemInMarketRsp
{
    int err_code;
};


/* 教学 */
//查询教学请求包
struct SPCmd_QueryInstruction
{
    unsigned int stub;			//临时
};

//查询教学返回包
struct SPCmd_QueryInstructionRsp
{
    int err_code;

    unsigned long long progress;	//教学完成进度
};

//完成教学请求包
struct SPCmd_CompleteInstruction
{
    unsigned long long progress;	//教学完成进度
};

//完成教学返回包
struct SPCmd_CompleteInstructionRsp
{
    int err_code;
};

#pragma pack() //结束


#endif //SIMPLEPROTOCOLCMD_H

