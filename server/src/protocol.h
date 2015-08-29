
#ifndef SIMPLEPROTOCOLCMD_H
#define SIMPLEPROTOCOLCMD_H

#include <arpa/inet.h>
#include "utils.h"
#include <assert.h>

#define Current_ProtocolVersion 1


#define SimpleProtocol_Tag_Server 21335
#define SimpleProtocol_Tag_Client 21336

//�������͵�������� 
#define FormationHeroAcountMax 4
// ��ЧId
#define INVALID_ID 0

//iOS�ͻ���token����
#define iOS_ClientPushTokenLen 32

//�ͻ��˵�¼
#define ClientAccountMaxLen 32
#define ClientPswMaxLen     16

//һ�ݸ��ʲ���ǿ���ɹ��ʼӳ�
#define QIANGHUA_GAILV_CAILIAO_ADD_RATIO 20

//��������
#define Vitality_Total                   120
//������������
#define Vitality_GuanQia_Floped 		 2
//��boss����������
#define Vitality_GuanQia_Boss            12

//�����۸� ���
#define Vitality_Price                   50

//Ӣ�۵ȼ��޶�
#define HERO_MAX_LEVEL 30
#define HERO_MAX_LEVEL_JINGYING 30

//Ӣ�۽����޶�
#define HERO_MAX_STAGE 10

//�������ѧϰ������
#define SKILL_MAX_LEVEL 8


//Ԫ��ȼ��޶�
#define YUANSHEN_MAX_LEVEL 25

//Ԫ��ȼ��޶�
#define YUANSHEN_MAX_STAGE 5


//���Կ������������ĵȼ� 
#define HERO_LEVEL_HELL_TASK_OPEN 5

//���������Ŷ�boss��������������Ѷ�ѡ��
#define ARENA_HARD_LEVEL_COUNT 5

//������һ�����������
#define ARENA_ROOM_HERO_COUNT 4

//�Ŷ�BOSSһ�����������
#define TEAMBOSS_ROOM_HERO_COUNT ARENA_ROOM_HERO_COUNT

//����װ������
#define DUANZAO_GROUP_NAME_MAX_LEN 48

////����װ���۸�(���)
//#define DUANZAO_ONCE_PRICE 200
//#define DUANZAO_TENTIMES_PRICE 1800

//ǿ���������Ҫ���װ����С�ȼ�
#define Qianghua_zhuangbei_min_level 1
//��ȡ��ħ��Ҫ���װ����С�ȼ�
#define Cuiqu_zhuangbei_min_level 8

//�������keep���ٴ�ǿ��
#define Huitui_zhuangbei_keep_max 4



//װ���������Լӳ���
#define REFINE_EQUIP_GAIN_PER 	0.12f

//��ȡһ�����������
#define DROW_SHENBING_NEED_GOLD  400


typedef unsigned int TaskId;

//��������id
#define MainTaskId_min 1
#define MainTaskId_max 50

//��������id��ʼֵ
#define HellTaskId_Min 101
#define HellTaskId_Max 131

#define CONTENT_MAX_LEN	120		// ����������󳤶�
#define SUMMARY_MAX_LEN	60		// �ʼ���Ҫ��󳤶�


//װ��ǿ������
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

	// �����ǹ���
	Profession_GW_Start = 19, // ����Ŀ�ʼ���������ù������Щ�����ö����û���õ�,����ע�͵�
	// ��ս
	Profession_JZ_JiangShi = 20,
	Profession_JZ_ShiRenMo = 21,
	Profession_JZ_YeRen = 22,
	Profession_JZ_KuLouBin = 23,
	Profession_JZ_KuLouZhanShi = 24,
	Profession_JZ_ShuGuai = 25,
	// Զ��
	Profession_YC_KuLouGongJianShou = 40,
	Profession_YC_ShuYao = 41,
	// ��ʦ
	Profession_FS_KuLouWuShi = 60,
	Profession_FS_YeRenSaMan = 61,
	// ���滹����n���ְҵ....ֱ��Profession_GW_End
	Profession_GW_End = 1700, // �����ĵط�

	Profession_Big_Boss_1 = 1800,	// �Ŷ�boss��
};

enum ZhuangbeiType
{
    ZhuangbeiType_DanshouJian = 1,
	ZhuangbeiType_ShuangshouBishou = 2,	// ZhuangbeiType_ShuangshouMao -> �ĳ�˫��ذ��
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

    ZhuangbeiColour_BaiZhuang_Fumo = 10, //���ݿ���װ��ģ����
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
    AttrType_skillzhouqi = 15,	// ��������
	AttrType_skillturn   = 16,	// ���ܴ�����
	AttrType_skillresult = 17,	// ����Ч���ӳ�
	AttrType_actor_gongji_addratio = 18,	// ��ɫ���幥���ӳ�
	AttrType_actor_hp_addratio = 19,	// ��ɫ����Ѫ���ӳ�
};

//0��ʾ��δ���1��ʾ�Ѽ����δ��ȡ��2��ʾ����ȡ��3��ʾ����ɵ�δ�ύ��4��ʾ���ύ��
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
	SkillType_BaoLieZhan         = 1,   // ����ն
	SkillType_ShaLu              = 2,			// ɱ¾
	SkillType_ShanDian           = 3,			// ����
	SkillType_JianShou           = 4,			// ����
	SkillType_ShenZhiXingNian    = 5,	// ��֮����
	SkillType_ShunYinTuXi        = 6,		// ˲ӰͻϮ
	SkillType_LieGong            = 7,			// �ҹ�
	SkillType_JianYu             = 8,			// ����
	SkillType_HuoQiuShu          = 9,		// ������
	SkillType_WenYi              = 10,			// ����

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

    //�����ǹ��＼��
    SkillType_Monster_XieE       = 101,
    
    //BOSS����
    SkillType_BOSS_Sweep       	 = 201,		//��ɨ 
    SkillType_BOSS_Flog       	 = 202,		//���� 
};


//��װ�������Ʒ����
enum ItemsType
{
    ItemsType_Gold = 1,
    ItemsType_Silver = 2,
    
    ItemsType_QianghuaNormal = 5,
    ItemsType_QianghuaXiyou = 6,
    ItemsType_QianghuaChuangqi = 7,
    ItemsType_QianghuaGailvTianjia = 8,

//10~100Ϊ��ʯ��������
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
//10~100Ϊ��ʯ��������

    ItemsType_Blood        = 202,  //Ѫƿ
    ItemsType_HellStone     = 203,  //����������ϣ���������

    //ָ�����һ�Ž��ǵ�ǿ������
    ItemsType_GoldStar_Qianghua   = 204,
    //������ӡ����
    ItemsType_Shenbing_Jiefengyin = 205,
    //�������
    ItemsType_Shenbing_Jianding   = 206,
    //����ʯ�������ң�ľ�ģ�ˮ��ʯ
    ItemsType_Dalishi             = 207,
    ItemsType_Huagangyan          = 208,
    ItemsType_Mucai               = 209,
    ItemsType_Shuijingshi         = 210,
    //��ɫ�����������
    ItemsType_Hero_LevelUp        = 211,
    ItemsType_Hero_Upgrade        = 212,
    //����׭�̲���
    ItemsType_JingwenZhuanke      = 213,

//Ѱħ�ʵ���
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

    //������
    ItemsType_SkillBook_MinId              = 400,
    
    ItemsType_SkillBook_BaoLieZhan         = ItemsType_SkillBook_MinId+SkillType_BaoLieZhan      ,   // ����ն
	ItemsType_SkillBook_ShaLu              = ItemsType_SkillBook_MinId+SkillType_ShaLu           ,			// ɱ¾
	ItemsType_SkillBook_ShanDian           = ItemsType_SkillBook_MinId+SkillType_ShanDian        ,			// ����
	ItemsType_SkillBook_JianShou           = ItemsType_SkillBook_MinId+SkillType_JianShou        ,			// ����
	ItemsType_SkillBook_ShenZhiXingNian    = ItemsType_SkillBook_MinId+SkillType_ShenZhiXingNian ,	// ��֮����
	ItemsType_SkillBook_ShunYinTuXi        = ItemsType_SkillBook_MinId+SkillType_ShunYinTuXi     ,		// ˲ӰͻϮ
	ItemsType_SkillBook_LieGong            = ItemsType_SkillBook_MinId+SkillType_LieGong         ,			// �ҹ�
	ItemsType_SkillBook_JianYu             = ItemsType_SkillBook_MinId+SkillType_JianYu          ,			// ����
	ItemsType_SkillBook_HuoQiuShu          = ItemsType_SkillBook_MinId+SkillType_HuoQiuShu       ,		// ������
	ItemsType_SkillBook_WenYi              = ItemsType_SkillBook_MinId+SkillType_WenYi           ,			// ����

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

	//�ӻ��̴�����
	ItemsType_DaiGouJuan				   = 501,
	//������������
	ItemsType_PresentVitality			   = 502,
	//����VIP1����
	ItemsType_PresentVIP1			   	   = 503,
	//����ϡ��װ������
	ItemsType_PresentEquip_Xiyou	       = 504,
	//���ʹ���װ������
	ItemsType_PresentEquip_Legend	       = 505,
    
	//ϴ������
	ItemsType_Purify_Scroll	   			   = 511,
	
	//��������
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


    // ȫ��
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

    //�鿴���˵��Ŷ���Ϣ
    queryAnotherTeamInfo     = 53,
    queryAnotherTeamInfoRsp  = 54,

    //��������
    chargeVitality           = 55,
    chargeVitalityRsp        = 56,

    //��ȡ������װ��Ϣ
    queryAllSuitName         = 57,
    queryAllSuitNameRsp      = 58,

    //��ȡ��ħ
    ZBCuiqu                  = 59,
    ZBCuiquRsp               = 60,
    ZBFumo                   = 61,
    ZBFumoRsp                = 62,

    //��ѯ�Լ�ӵ�е�ħ������
    queryAllMofaJinghua      = 63,
    queryAllMofaJinghuaRsp   = 64,

    //��ѯ����boss����
    queryFubenDiaoluo        = 65,
    queryFubenDiaoluoRsp     = 66,

    //�����ּ��룬����������֪ͨ
    newAssistantJoinRsp      = 68,

    //��ѯӢ�۵�ǰ��֧�����б�
    querySubTaskList         = 69,
    querySubTaskListRsp      = 70,

    //���������
    newSubTaskGotRsp         = 72,
    //������֪ͨ�������ύ
    subTaskSubmitedRsp       = 74,

    //��ѯ����������
    queryMainTaskReward      = 75,
    queryMainTaskRewardRsp   = 76,

    //��ֵ���ӣ�������
    chargeGold               = 77,
    chargeGoldRsp            = 78,

    //��ѯ�Ŷ���ӵ�еļ���
    queryTeamSkillList       = 79,
    queryTeamSkillListRsp    = 80,
    //��ĳ����ɫװ�似��
    equipSkill               = 81,
    equipSkillRsp            = 82,
    //ʹ�õõ��ļ������ĳ����ɫѧϰ
    actorLearnSkill          = 83,
    actorLearnSkillRsp       = 84,

    //�����Ƭ
    queryTeamShenbingSuipian = 85,
    queryTeamShenbingSuipianRsp = 86,
    shenbingHecheng          = 87,
    shenbingHechengRsp          = 88,

    //������Ƭ
    queryTeamItemSuipian     = 89,
    queryTeamItemSuipianRsp  = 90,
    itemHecheng              = 91,
    itemHechengRsp           = 92,

    //��ѯ��������б�
    queryAllShenbingList     = 93,
    queryAllShenbingListRsp  = 94,

    //ɨ������
    saodangFuben             = 95,
    saodangFubenRsp          = 96,

    //����
    heroLevelUp              = 97,
    heroLevelUpRsp           = 98,

    //����
    heroUpgrade              = 99,
    heroUpgradeRsp           = 100,

    //������ӡ
    shenbingJieFengyin       = 101,
    shenbingJieFengyinRsp    = 102,

    //�������
    shenbingJianding         = 103,
    shenbingJiandingRsp      = 104,

    //����
    zbJinglian               = 105,
    zbJinglianRsp            = 106,

    //���׭�̾���
    shenbingZhuankeJingwen   = 107,
    shenbingZhuankeJingwenRsp = 108,

    //��ѯĳ����ɫ��Ԫ��
    queryYuanshenOfHero      = 109,
    queryYuanshenOfHeroRsp   = 110,

    //����
    churchLevelUp            = 111,
    churchLevelUpRsp         = 112,

    //��������
    skillLevelUp             = 113,
    skillLevelUpRsp          = 114,

    //��ȡװ��ͼ��
    getZhuangbeiAlbum        = 115,
    getZhuangbeiAlbumRsp     = 116,

    //��ȡ����ͼ��
    getSkillAlbum            = 117,
    getSkillAlbumRsp         = 118,

    //������
    arenaEnter               = 161,
    arenaEnterRsp            = 162,
    arenaUpdateRoomRsp       = 164,
    arenaGetBattleScriptAndGiftRsp  = 166,
    arenaGetGift             = 167,
    arenaGetGiftRsp          = 168,

    //�ŶӸ���
    teamBossEnter            = 181,
    teamBossEnterRsp         = 182,
    teamBossUpdateRoomRsp    = 184,
    teamBossGetBattleScriptAndGiftRsp  = 186,
    teamBossGetGift          = 187,
    teamBossGetGiftRsp       = 188,

    //�س�(����)
    cityGuardEnter           = 201,
    cityGuardEnterRsp        = 202,
    cityGuardGetNextMonster  = 203,
    cityGuardGetNextMonsterRsp  = 204,
    cityGuardFight           = 205,
    cityGuardFightRsp        = 206,

    //Ӣ�۰�
    topHeroGetList           = 221,
    topHeroGetListRsp        = 222,
    topHeroChallenge         = 223,
    topHeroChallengeRsp      = 224,
    topHeroClearCDTime       = 225,
    topHeroClearCDTimeRsp    = 226,

    //�۷�Ծ�
    topArenaQueryStatus      = 241,
    topArenaQueryStatusRsp   = 242,
    topArenaGuess            = 243,
    topArenaGuessRsp         = 244,
    topArenaReplay         	 = 245,
    topArenaReplayRsp        = 246,
    topArenaQueryGuess     	 = 247,
    topArenaQueryGuessRsp    = 248,

    //ϵͳ����
    systemGiftQuery          = 261,
    systemGiftQueryRsp       = 262,
    systemGiftGetGift        = 263,
    systemGiftGetGiftRsp     = 264,

    //���޳�
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

    //װ������
    duanzaoListQuery         = 311,
    duanzaoListQueryRsp      = 312,
    duanzao                  = 313,
    duanzaoRsp               = 314,
    duanzaoGroupZBQuery      = 315,
    duanzaoGroupZBQueryRsp   = 316,

    //��������ս
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

    //�ӻ���
    queryZahuopu             = 371,
    queryZahuopuRsp          = 372,
    refreshZahuopu           = 373,
    refreshZahuopuRsp        = 374,
    zahuopuGetOneItem        = 375,
    zahuopuGetOneItemRsp     = 376,

    //��������
    queryHellTaskHeroInfo    = 391,
    queryHellTaskHeroInfoRsp = 392,
    hellTaskBattle           = 393,
    hellTaskBattleRsp        = 394,

    //�س�(����boss)
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
		
	//����
	queryBonus 				= 411,
	queryBonusRsp 			= 412,
	getBonus	 			= 413,
	getBonusRsp				= 414,
	sendBonusNotify 		= 415,

	//��ȡ���̱���
	//getChessBox  			= 431,
	//getChessBoxRsp			= 432,
	
	//�ؿ��½�����
	queryRateTask  			= 441,
	queryRateTaskRsp		= 442,
	queryRateTaskBonus		= 443,
	queryRateTaskBonusRsp	= 444,
	getRateTaskBonus		= 445,
	getRateTaskBonusRsp		= 446,

    //��ѯvip��Ȩ
	queryVipPrivilege       = 451,
	queryVipPrivilegeRsp    = 452,
	
    //�������
	upgradeShenbing       	= 461,
	upgradeShenbingRsp    	= 462,

	//��ѯ��ֵ��Ϣ
    queryChargePacket       = 471,
    queryChargePacketRsp    = 482,
	
	//��������
    unlockAssistant       	= 491,
    unlockAssistantRsp    	= 492,

	//�������ͻ
	queryGiftActivity		= 501,
	queryGiftActivityRsp	= 502,
	getGiftActivity			= 503,
	getGiftActivityRsp		= 504,
	
	//��ɱ�����ۼ�
	queryKilledMonster		= 511,
	queryKilledMonsterRsp	= 512,
	getKilledMonsterBonus	= 513,
	getKilledMonsterBonusRsp	= 514,
	queryKilledMonsterBonus	= 515,
	queryKilledMonsterBonusRsp	= 516,

	//����
	castSilver				= 521,
	castSilverRsp			= 522,

	//����¼
	queryLegendNote			= 531,
	queryLegendNoteRsp		= 532,
	challengeLegendNote 	= 533,
	challengeLegendNoteRsp 	= 534,

	//Ѫɫ����
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

	//�ػ�֮��
	queryProtectSpirit			= 561,
	queryProtectSpiritRsp		= 562,
	challengeProtectSpirit		= 563,
	challengeProtectSpiritRsp	= 564,
	getProtectSpiritBonus		= 565,
	getProtectSpiritBonusRsp	= 566,
	worshiProtectSpirit			= 567,
	worshiProtectSpiritRsp		= 568,
	
	//̽��
	queryDiscoverInfo		= 581,
	queryDiscoverInfoRsp	= 582,
	discover				= 583,
	discoverRsp				= 584,
	unlockDiscoverSeal		= 585,
	unlockDiscoverSealRsp	= 586,
	
	//�ʼ�
	queryEmail				= 591,
	queryEmailRsp			= 592,
	readEmail				= 593,
	readEmailRsp			= 594,
	writeEmail				= 595,
	writeEmailRsp			= 596,
	deleteEmail				= 597,
	deleteEmailRsp			= 598,
	pushEmail				= 599,
	
	//���
    queryGiftBag			= 611,
    queryGiftBagRsp			= 612,
    getGiftBag				= 613,
    getGiftBagRsp			= 614,

    //����װ��
    queryAssessWeapon    	= 621,
    queryAssessWeaponRsp	= 622,
    assessWeapon       		= 623,
    assessWeaponRsp    		= 624,

	//ϴ��
    queryPurify				= 631,
    queryPurifyRsp			= 632,
    purify					= 633,
    purifyRsp				= 634,
    confirmPurify			= 635,
    confirmPurifyRsp		= 636,
	
	//װ������
    queryOrigin				= 641,
    queryOriginRsp			= 642,
    
	//���������Ϣ
    queryHeroOtherInfo		= 651,
    queryHeroOtherInfoRsp	= 652,
    
	//����
    queryNotice				= 661,
    queryNoticeRsp			= 662,

	//�̳�
    queryMarket            	= 671,
    queryMarketRsp        	= 672,
    buyItemInMarket         = 673,
    buyItemInMarketRsp      = 674,

	//��ѧ
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

#pragma pack(1) //���յ��ֽڶ���

//��ͷ
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
    
    //��������ǰʱ��
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
    unsigned int zhenshen_id; //�����Ԫ������ֶ�Ϊ�������hero_id������Ϊ��Ч�ֶΡ�
	
    unsigned int vip_level;
    unsigned int profession_id;
    unsigned int level; //Ӣ�۵ȼ�
	unsigned int stage; //Ӣ�۽�
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

    //����վλ��0��ʾδ����
    unsigned int postion;	
	bool is_unlocked;	//Ӣ���Ƿ����
	bool unlockAble;	//�Ƿ���Խ�����

    //
    unsigned int exp_current;
    unsigned int exp_need;

    //����
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

    //��������ǰʱ��
    unsigned int server_time;
    
	//Ӣ���б�
    unsigned int hero_count;
    char hero_data[0]; //HeroInfo����
};


//������Ӣ��
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

//��������
struct SPCmd_CreateAssit
{
    unsigned int profession_id;
};

struct SPCmd_CreateAssitRsp
{
    int err_code;

    HeroDetail hero_detail;
};

//��ȡ�Ŷ�����Ӣ��
struct SPCmd_GetHerosInTeam
{
    //��Ӣ��id
    unsigned hero_id;
};

struct SPCmd_GetHerosInTeamRsp
{
    int err_code;

    //���õȼ�
    unsigned int church_level;

    unsigned int hero_count;
    //variable
    HeroDetail hero_detail_arr[0];
};

// װ���ĸ�Ҫ����4+4+13 = 21B
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
    //�䳤
	//char attr_arr[1];// ZBAttr������ ZBAttr attr_arr[0];
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
    unsigned char max_qianghua_time; //��װ�����ǿ�����ٴ�
    
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
    unsigned int group_id; //װ��ģ����

    unsigned int zhuangbei_type;
    char zhuangbei_name[ZhuangbeiNameMaxLen+1];
    unsigned int zhuangbei_colour;
    char zhuangbei_pic[ZhuangbeiPicMaxLen+1];

    //װ����˭������
    unsigned int zhuangbei_hero_id;

    unsigned int profession_id;    		//�󶨵�ְҵID, Ϊ0���κ�ְҵ
    unsigned int zhuangbei_level;
    unsigned int hole;
    unsigned int diamond_type_1;
    unsigned int diamond_type_2;
    unsigned int diamond_type_3;
    
    unsigned int gongji_min;
    unsigned int gongji_max;
    unsigned int hujia;

    //ǿ����Ϣ
    //����
    QianghuaData qianghua_data;

    //��ħ��Ϣ
    //����
    bool has_fumo;
    ZBAttr fumo_attr;

    //�䳤
    ZhuangbeiCommonAttr common_attr;

    //common_attr�����󣬽���
    //�䳤����'\0'����
    //char zhuangbei_desc[1];

    //����װ��������Ϣ���Ǵ���װ����(20140521��һЩϡ��װҲ������Ϣ)
    //JinglianData jinglian_data;

	//���β���ϴ����Ϣ
	//unsigned char purify_hole;		//ϴ������
	//ZBAttr purify_attr_arr[0]; 		//���5��ϴ������ 

	////2014.05.26���is_unlocked�����Ƶ�ϡ��װ�� ����is_shenbing������
    //bool is_unlocked; //�Ƿ���ӡ

    //bool is_shenbing; //Ϊtrue��Ϊ��������Ϣ
    
    //�����������Ϣ�����������//////////////////////
    //unsigned char identified_arr_count; //������������Լ����˼���
    //ZBAttr hide_attr_arr[0]; //��������
    //׭�̾���
    //bool has_jingwen;
    //ZBAttr jingwen_attr_arr[2];//׭�̾�����������
    ////////////////////////////////////////

    ZhuangbeiDetail()
    {
        memset(this, 0, sizeof(ZhuangbeiDetail));
    };

    static unsigned int max_length()
    {
        //���ݿ���������װ���������96�ֽ�
        
        return sizeof(ZhuangbeiDetail) + 10*sizeof(ZBAttr) + 97 + sizeof(JinglianData) 
            + sizeof(bool)*2 + sizeof(unsigned char) + sizeof(ZBAttr)*5
            + sizeof(bool) + sizeof(ZBAttr)*2 + sizeof(unsigned char) + sizeof(ZBAttr)*5;
    }
};
inline void hton_zhuangbei_detail(ZhuangbeiDetail& detail_in, ZhuangbeiDetail& detail_out)
{

    detail_out.zhuangbei_id = Utl::htonll(detail_in.zhuangbei_id);
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

    //ǿ����Ϣȫ����char������ת��
    detail_out.qianghua_data = detail_in.qianghua_data;

    //��ħ��Ϣ
    detail_out.has_fumo = detail_in.has_fumo;
    detail_out.fumo_attr.attr_type = htonl(detail_in.fumo_attr.attr_type);
    detail_out.fumo_attr.attr_value = htonl(detail_in.fumo_attr.attr_value);
    detail_out.fumo_attr.cuiqu_suc_ratio = htonl(detail_in.fumo_attr.cuiqu_suc_ratio);

    //����ħ������
    detail_out.common_attr.attr_count = htonl(detail_in.common_attr.attr_count);
    for(int i=0; i<detail_in.common_attr.attr_count; ++i)
    {
        detail_out.common_attr.attr_arr[i].attr_type = htonl( detail_in.common_attr.attr_arr[i].attr_type);
        detail_out.common_attr.attr_arr[i].attr_value = htonl(detail_in.common_attr.attr_arr[i].attr_value);
        detail_out.common_attr.attr_arr[i].cuiqu_suc_ratio = htonl(detail_in.common_attr.attr_arr[i].cuiqu_suc_ratio);
    }

    //����
    char* desc_in = ((char*)detail_in.common_attr.attr_arr) + sizeof(ZBAttr)*detail_in.common_attr.attr_count;
    char* desc_out = ((char*)detail_out.common_attr.attr_arr) + sizeof(ZBAttr)*detail_in.common_attr.attr_count;
    strcpy(desc_out, desc_in);

    //����
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
    
    //���
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
    detail_out.zhuangbei_id = Utl::ntohll(detail_in.zhuangbei_id);
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

    //ǿ����Ϣȫ����char������ת��
    detail_out.qianghua_data = detail_in.qianghua_data;

    //��ħ��Ϣ
    detail_out.has_fumo = detail_in.has_fumo;
    detail_out.fumo_attr.attr_type = ntohl(detail_in.fumo_attr.attr_type);
    detail_out.fumo_attr.attr_value = ntohl(detail_in.fumo_attr.attr_value);
    detail_out.fumo_attr.cuiqu_suc_ratio = ntohl(detail_in.fumo_attr.cuiqu_suc_ratio);

    //����ħ������
    detail_out.common_attr.attr_count = ntohl(detail_in.common_attr.attr_count);
    for(unsigned int i=0; i<detail_out.common_attr.attr_count; ++i)
    {
        detail_out.common_attr.attr_arr[i].attr_type = ntohl( detail_in.common_attr.attr_arr[i].attr_type);
        detail_out.common_attr.attr_arr[i].attr_value = ntohl(detail_in.common_attr.attr_arr[i].attr_value);
        detail_out.common_attr.attr_arr[i].cuiqu_suc_ratio = ntohl(detail_in.common_attr.attr_arr[i].cuiqu_suc_ratio);
    }

    //����
    char* desc_in = ((char*)detail_in.common_attr.attr_arr) + sizeof(ZBAttr)*detail_out.common_attr.attr_count;
    char* desc_out = ((char*)detail_out.common_attr.attr_arr) + sizeof(ZBAttr)*detail_out.common_attr.attr_count;
    strcpy(desc_out, desc_in);

    //����
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

    //���
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
    //վλ������
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
    
    //��1��4��λ��Ӣ��id�����Ϊ0���ʾ��λ��ȱ��
    unsigned int hero_id_arr[FormationHeroAcountMax];
};

struct SPCmd_AdjustFormationRsp
{
    int err_code;
};


struct SPCmd_GetTaskList
{
    //��ʱ
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
    
    //������������״̬��
    unsigned int task_status;

    //��һ��������Щ�쿪��
    //bit0~6�ֱ��Ӧ�����ա�����1~6
    unsigned char active_day_bitmap;

    //�Ƿ��п���ʱ������
    bool is_hour_limited;
    //hour��0~23�� min��0~59
    //����ʱ��
    unsigned char start_hour;
    unsigned char start_min;
    //����ʱ��
    unsigned char end_hour;
    unsigned char end_min;

    //�Ƿ���ÿ�մ�������
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
    
    //��ǰ��������һ��������
    unsigned int maintask_last_actived;
    //������������״̬��
    unsigned int maintask_status;

    //��ǰ��Ҫ��ʾ�ĸ����б�
    //int subtask_count;
    //�ɱ�����
    //SubTaskInfo task_arr[0];
};


/* ��������  */
//�������̸�������
enum TaskGridType {
    TaskGridType_Empty 		= 0, 	//����
    TaskGridType_Monster 	= 1, 	//������
    TaskGridType_Box 		= 2, 	//������
};

//�������̸���״̬
enum TaskGridStat {
    TaskGridStat_PreFlop 	= 0, 	//δ����
    TaskGridStat_Floped 	= 1, 	//�ѷ���
    TaskGridStat_Folded		= 2, 	//����
};

//���̸�����Ϣ
struct TaskGridInfo
{
    unsigned int type;				//TaskGridType
	
	//���ƺ�������ǹ����� ֱ���ó�״̬TaskGridStat_Folded ��Ȼ��ΪTaskGridStat_Floped
    unsigned int stat; 				//TaskGridStat
    unsigned int monster_type;		//��������ǹ��� �ؿ����� GuanQiaType
};

//��ȡ�������������
struct SPCmd_GetTaskChessBoard
{
    unsigned int task_id;

    bool need_refresh_chessboard; 	//�Ƿ���Ҫ����ˢ������ ��Ŀǰ�����ÿ�δӵ�ͼ����ʱ��ˢ�¡�
};

//��ȡ�������̷��ذ�
struct SPCmd_GetTaskChessBoardRsp
{
    unsigned int err_code;

    unsigned int horizontal_count;	//���ŵĸ�������
    unsigned int vertical_count;	//���ŵĸ�������

	TaskGridInfo grid_arr[1];		//�±�0��boss ����ľ���horizontal_count*vertical_count
};

//���������
struct SPCmd_FlopTaskGrid
{
	unsigned int task_id;
	unsigned int grid_id;	//ֻ����1-16
};

//���Ʒ��ذ�
struct SPCmd_FlopTaskGridRsp
{
    unsigned int err_code;

	unsigned int boss_stat;			//TaskGridStat
	unsigned int grid_stat;			//TaskGridStat
	
    //item����
    int count;
    ItemInfo item_arr[0];
};


//��ȡ���̱��������
struct SPCmd_GetChessBox {
	unsigned int task_id;
    unsigned int grid_id; //1~16
};

//��ȡ���̱��䷵�ذ�
struct SPCmd_GetChessBoxRsp {
    unsigned int err_code;
	
    //item����
    int count;
    ItemInfo item_arr[0];
};


struct SPCmd_GetItemsInTeam
{
    //��ʱ
    unsigned int stub;
};

struct SPCmd_GetItemsInTeamRsp
{
    unsigned int err_code;

    //item����
    int count;
    ItemInfo item_arr[0];
};
/*
struct SPCmd_ZBQianghua
{
    
    unsigned long long zhuangbei_id;

    //�����˼��ݸ�����������
    unsigned int gailv_cailiao_count;
};

struct SPCmd_ZBQianghuaRsp
{
    int err_code;

    //qianghua_result��ʾǿ���ɹ����
    //����ɹ��������ļ�������Ϊǿ����Ľ��
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
	bool b_gold_star;			//  �Ƿ�ֱ��ǿ��Ϊ����
};

struct SPCmd_ZBQianghuaRsp
{
    int err_code;

    //ǿ��Ч��
    QianghuaInfo qianghua_info;
};

struct SPCmd_ZBQianghuaHuitui
{
    unsigned long long zhuangbei_id;

    //Ҫ������ǿ��id
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

    //�����item����
    int count;
    ItemInfo item_arr[0];

    //����õĲ���Ϊ�䳤 ������������Ĳ���
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
    //ռλ
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
    MofaJinghua jinghua; //sucess=true ����Ч
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
    char attr_str[0]; //attr_str_count�������ַ�����ÿ���ַ�����'\0'����

    //char desc[0]; //������attr_str�����һ��'\0'֮����'\0'���� 
};
struct SPCmd_QueryFubenDiaoluoRsp
{
    int err_code;

    unsigned int item_count;
    unsigned int item_type_arr[0]; //�䳤

    //unsigned int zb_count;
    //char data[0]; //�䳤�ṹFubenDiaoluoZBInfo ����
};



//
struct SPCmd_Battle
{
    //
    unsigned int task_id;
    unsigned int grid_id; //0~16
};

// ÿһ����ɫ���ϵ�װ������
#define	MAX_EQUIP_NUM_EACH_ACTOR	8

// ��HeroBase
struct Battel_HeroBase
{
	unsigned int heroId;
	unsigned profession_id;		// ְҵid
	unsigned int level;
	unsigned int stage;			// �ף���ʼ����1
	unsigned int hp;			// Ѫ������ǰ��Ѫֵ
	unsigned int defaultHp;		// ��Ѫֵ
	short hero_type;			// HeroBaseType
	short defaultCdTime;		// cdʱ�䣬�ǲ�����������wait��useBlood
	short x_size;				// ����x�����ϵ�ռ�õĸ���������Ĭ����1�����е�ȡֵ��2,�ͻ��˾�Ӧ�ð�xsize��2�ķ��ں�����м������gridpos����
	short resonance_star;		//װ��ǿ�������Ǽ�

	// װ����Ϣ
	short zb_summary_Num;
	ZhuangbeiSummary zhuangbei_summary_arr[MAX_EQUIP_NUM_EACH_ACTOR]; //ZhuangbeiSummary
};
// ��InitChessBoard
struct Battel_InitChessBoard
{
	// �ҷ���ս������,[0]������index0��λ�õĽ�ɫ����
	Battel_HeroBase myTeam[FormationHeroAcountMax];
	unsigned int myTeamUniqueId[FormationHeroAcountMax];// ȫս��һ��
	short myCurBloodNum;		// �ҷ���ǰ����Ѫƿ���� -----> class ChessBoard -->currentBloodNum
	short myDefaultBloodNum;	// �ҷ���ǰȫ��Ѫƿ����

	// �Է��Ĳ���
	Battel_HeroBase oppositeTeam[FormationHeroAcountMax];
	unsigned int oppositeUniqueId[FormationHeroAcountMax];
	short oppositeCurBloodNum;		// �Է���ǰ����Ѫƿ����
	short oppositeDefaultBloodNum;	// �Է���ǰȫ��Ѫƿ����
};
// �μ�ClientGameBuffEffect
struct Battel_GameBuffEffect		// buff��һ��Ч��,Ҳ��һ�ε����ã����Ƴ���ʱ������á�
{
	int effect_id;					// id��һ��ս����Ψһ��ʶ����GameBuffPro����
	bool xuanyun;
	bool fantan;					// ����
	// change_state��������Ҫ��*����������Ҫ��*
	char hp_change_state;			
	char baoji_change_state;		
	char gongjili_change_state;	
	char shanbi_change_state;	
	char fangyu_change_state;	
};
// �μ�ClientGameBuff
struct Battel_GameBuff
{
	short buffStateId;					//BuffStateId״̬���ͣ���ͬ״̬��Ӧ���໥����
	bool cumulative;					// �Ƿ�����ۻ�
	unsigned int attachedToUId;	// �������ϵĽ�ɫ��uniqueId
	unsigned int fireUId;		// �ͷŸ�buff�Ľ�ɫuniqueId

	int effectNum;
	char gameBuffEff_arr[1];
};


// ��StepMove
struct Battel_StepMove
{
	unsigned int uniqueId;	// ˭�ƶ�
	int destIndex;			// Ŀ�� ������λ�ã����<0�ʹ����ƶ�
	bool inSelfChessBoard;	// �Ƿ����������̵�����
};
// ��StepChangeRound
struct Battel_StepChangeRound
{
	unsigned int round;	// �غ���
};
// ��StepFight
struct Battel_StepFight
{
	// �����ߺͱ������ߵ�Id
	unsigned int fightorUniqueId;
	short fightPattern;

	short targetNum;
	unsigned int targetUniqueId_arr[1];		// ����������,�䳤
	bool canMiss[1];						// �ܷ��ܣ��䳤
	int targetBloodChange[1];				// Ѫ���仯���������Ǽ�Ѫ���������Ǽ�Ѫ
};
//��StepUseBlood
struct Battel_StepUseBlood
{
	unsigned int uniqueId;	// ˭ʹ�ã�cdʱ���ǹ̶��ģ��ڳ�ʼ����Ϣ���档
	int bloodChangeForUser;	// Ѫ���仯
};
// ��StepSkill
struct Battel_StepSkill
{
	unsigned int fightorUniqueId;
	short skillPatten;
	short skillType;						// ��������


	short targetNum;
	short buffNum;							// �����buff����
	char gameBuff_arr[1];					// Battel_GameBuff�����Buff��buff�����˸����ߣ���ô���buff������Ӹ������ߵ�
	unsigned int targetUniqueId_arr[1];		// ����������,�䳤
	bool canMiss[1];						// �ܷ��ܣ��䳤
	int targetBloodChange[1];				// Ѫ���仯���������Ǽ�Ѫ���������Ǽ�Ѫ
};


// ��Buff_Doing
struct Battel_Buff_Doing
{
	unsigned int target;
	int hp_add;		// Ѫ�������ӣ��������Ǽ���
	short relativedBuffStateId;	// ��������buff

	bool isFantan;				// ����Ƿ�������������ݲ�����
	unsigned int fightor_u_id;	// ������Ŀ��
	
};

// ��StepBuff
struct Battel_BuffInStep
{
	int removeBuffNum;
	int remove_buff_arr[1];		// �Ƴ���buff������

	// ��������ж��target����ͬһ���ˣ��ͻ�����Ҫ����ĳ��target��buffdoingȫ���ŵ��Ǹ�������
	int effect_num;
	char effects[1]; //vector<Buff_Doing> effects 
};


// ����
struct Battel_OneStep
{
	// ������ʽ���ƶ���������ֱ�ӹ���(������Ժ����ƶ�);
	// ������ʽ�Ĳ�ͬ����ȡ��ͬ������
	short stepType;	
	Battel_BuffInStep buff;

	int cdTimeNum;
	char cdDiscr_arr[1];

	char stepData[1];			// �������ݣ�������ƶ�����ô����Battel_StepMove�������fight����Battel_StepFight
};

// һ��������
struct PowerPoint
{
	unsigned int power; // ��������������
};

// ս������
struct Battel_FightEnd
{
	// �ҷ���ս�����4B
	short myResult;		//FightResult

	// �õ��ľ���
	unsigned int gettedExp;
	// �õ���ItemSuiPian
	unsigned int gettedItemSuiPianNum;
	char gettedItemSuiPian_arr[1];
	// �õ��������Ƭ
	unsigned int gettedShenBingSuiPianNum;
	char gettedShenBingSuiPian_arr[1];

	unsigned int powerPointNum;
	char powerPoint_arr[1]; // PowerPoint

	unsigned int zhuangbeiNum;			//��õ�װ������
	unsigned int itemNum;				// ��õ���Ʒ

	char zhuangbei_arr[1];//ZhuangbeiDetail
	char item_arr[1];//ItemInfo
};

// ��ĳһ���ͻ��˵ľ籾
struct Battel_FightScript
{
	// ��ʼ�����ݣ��ͻ��˲�һ��
	Battel_InitChessBoard init;

	unsigned int stepCount;		// ��������
	Battel_OneStep step_arr[1];

	// ������ݣ��ͻ��˲�һ��
	Battel_FightEnd endResult;
};
struct SPCmd_BattleRsp
{
    int err_code;

	int fightNum;	// ս���Ĵ���
	// �ű����� ����
	char fightScript_array[1];

	//Battel_FightScript fightScript;
};


//���������
struct SPCmd_Reconnect
{
    char client_account[ClientAccountMaxLen+1];
    char client_psw[ClientPswMaxLen+1];

    unsigned int main_hero_id;
};

//�������ذ�
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

    //������ύ�������������������������Ϊ0
    unsigned int exp;
    unsigned int silver;

    //
    unsigned int hero_count;
    HeroDetail hero_arr[0];
};

struct SPCmd_RestoreVitality
{
    //ռλ��
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


/* ������ */
//�������Ϣ����
enum MarqueeType {
    MarqueeType_Invalid				= 0,	//��Ч����
    MarqueeType_ForgeLegend			= 1,	//���촫��		
    MarqueeType_VarietyStore		= 2,	//�ӻ���
    MarqueeType_Compound			= 3,	//����ϳ�
    MarqueeType_TopArena			= 4,	//�۷�Ծ�
    MarqueeType_TopList				= 5,	//Ӣ�۰�
    MarqueeType_ProtectSpirit		= 6,	//�ػ�֮��
    MarqueeType_Discover_Destroy	= 7,	//Ѱ�� �����ӡ
    MarqueeType_Discover_Unlock		= 8,	//Ѱ�� ���װ��
};

//�������Ϣ
struct MarqueeMessage {
	//��ϢΨһ��ʾ��
    unsigned int uid;					
	
	//��Ϣ���� 
    unsigned short type;				//MarqueeType
    
    //��Ϣ����
    char subject[HeroNameMaxLen+1];		//�����

	//��Ϣ����
    union {
		//MarqueeType_ForgeLegend, MarqueeType_Compound, MarqueeType_Discover_Unlock
		unsigned int group_id;		//����group 
		
		//MarqueeType_VarietyStore
		unsigned int item_type;		//��������
		
		//MarqueeType_TopArena
		unsigned int round;			//���� 0:4ǿ 1:���� 2: �ھ�
		
		//MarqueeType_TopList
		unsigned int top_order;		//Ӣ�۰�����

		//MarqueeType_ProtectSpirit
		unsigned int chapter_id;	//�½�id
    } object ;


	unsigned short count;			//��Ϣ��������

	MarqueeMessage () {
		memset(this, 0, sizeof(MarqueeMessage));
	}
};


struct SPCmd_HeartBeat
{
    //ռλ��
    unsigned int stub;
};

struct SPCmd_HeartBeatRsp
{
    int err_code;

    unsigned int current_vit; 	//��ǰ����ֵ
    
    unsigned char msg_count;	//�������Ϣ���� ���10��
    MarqueeMessage msg_arr[0];	//�������Ϣ����
};


struct SkillInfo
{
    unsigned int skill_id; //����ʵ��id
    unsigned int skill_type; //enum SkillType
    unsigned int skill_level; 
    
    unsigned int owner_hero_id; //��˭ѧ��ļ���
    bool is_equiped; //�Ƿ�װ��
};


struct SPCmd_QueryAnotherTeamInfo
{
    //another team main hero_id
    unsigned int hero_id;
};

struct SPCmd_QueryAnotherTeamInfoRsp
{
    int err_code;

    //����Ӣ����Ϣ
    HeroDetail hero_arr[4];

    //����Ӣ����װ���ļ���
    SkillInfo skill_arr[4];

    //װ����Ϣ
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct SPCmd_ChargeVitality
{
    //ռλ
    int stub;
};

struct SPCmd_ChargeVitalityRsp
{
    int err_code;

    unsigned int gold_balance;
};

struct SPCmd_QueryAllSuitName
{
    //ռλ
    int stub;
};

struct SuitNameInfo
{
    unsigned int suit_id;
    char suit_name[ZhuangbeiNameMaxLen+1];

    unsigned int group_id_1; //Ϊ0��ʾ��Ч
    unsigned int zhuangbei_type_1; 
    char zhuangbei_name_1[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_2; //Ϊ0��ʾ��Ч
    unsigned int zhuangbei_type_2;
    char zhuangbei_name_2[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_3; //Ϊ0��ʾ��Ч
    unsigned int zhuangbei_type_3;
    char zhuangbei_name_3[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_4; //Ϊ0��ʾ��Ч
    unsigned int zhuangbei_type_4;
    char zhuangbei_name_4[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_5; //Ϊ0��ʾ��Ч
    unsigned int zhuangbei_type_5;
    char zhuangbei_name_5[ZhuangbeiNameMaxLen+1];
    unsigned int group_id_6; //Ϊ0��ʾ��Ч
    unsigned int zhuangbei_type_6;
    char zhuangbei_name_6[ZhuangbeiNameMaxLen+1];

    ZBAttr suit_attr[5][2]; //5�����ԣ�ÿ�����2����attr_typeΪ0��ʾ��Ч
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
    SkillInfo skill; //��ǰװ���ļ���
    

    //
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};


struct SPCmd_QuerySubTaskList
{
    //ռλ
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
    //ռλ
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


/* ��ֵ */
//��ֵ��
struct ChargePacketInfo{
    unsigned int id; 				//��ֵid
    unsigned int charge; 			//��ֵ���
    unsigned int gold_count; 		//��ý����
    unsigned int present_count; 	//���ͽ����
};

//��ѯ��ֵ�������
struct SPCmd_QueryChargePacket
{
    //ռλ
    int stub;
};

//��ѯ��ֵ�����ذ�
struct SPCmd_QueryChargePacketRsp
{
    int err_code;
	
	unsigned int next_vip_level;	//��һVIP�ȼ� 
	unsigned int next_need_charge;	//������һ���ȼ���Ҫ�ĳ�ֵ��� (������߼�Ϊ0)
	
    bool b_first_charged; 			//�״γ�ֵ��� false:δ��ֵ�� true:�ѳ�ֵ��
    ItemInfo first_present_item[5];	//�״γ�ֵ���͵ĵ���(ps:��ҷ���û��������)
	
    unsigned int count; 			// ��ֵ������
    ChargePacketInfo info[0];
};

//��ֵ�����
struct SPCmd_ChargeGold
{
    //��ֵ��id
    unsigned int gold_packet_id; 
};

//��ֵ���ذ�
struct SPCmd_ChargeGoldRsp
{
    int err_code;
    
    unsigned int gold_balance;		//��ֵ��Ľ����
    unsigned int vip_level;			//��ֵ���VIP�ȼ�
    
	unsigned int next_vip_level;	//��һVIP�ȼ� 
	unsigned int next_need_charge;	//������һ���ȼ���Ҫ�ĳ�ֵ��� (������߼�Ϊ0)
};

struct SPCmd_QueryTeamSkillList
{
    //ռλ
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
    unsigned int hero_id; //Ҫ��˭װ��
    unsigned int skill_id;
};

struct SPCmd_EquipSkillRsp
{
    int err_code;
};

struct SPCmd_ActorLearnSkill
{
    unsigned int hero_id; //Ҫ��˭ѧ
    unsigned int item_id; //��������ߵ�id
};

struct SPCmd_ActorLearnSkillRsp
{
    int err_code;

    SkillInfo new_skill;
};

struct SPCmd_ArenaEnterRsp
{
    int err_code;
    
    //���뾺����ʱ���������е���
    HeroInfo hero_info_arr[3]; //hero_id=0��ʾ��Ч

    //
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct SPCmd_ArenaUpdateRoomRsp
{
    int err_code;
    
    HeroInfo new_hero_info;

    //��������װ��
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
                data.data_union.zhuangbei.zhuangbei_id = Utl::htonll(data_union.zhuangbei.zhuangbei_id);
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

    //���ģʽ���ջ�ʣ����ٴλ���
    unsigned int battle_time_remainder;

    //ս����Ϣ
    unsigned int first_turn_hero_id_arr[4]; //0��1һ�飬2��3һ��
    unsigned int second_turn_hero_id_arr[2];

    //�õ������б���
    unsigned char gift_available_count;
    GiftJifen gift_jifen_arr[9];

	int fightNum;	// ս���Ĵ���
	// �ű����� ����
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
    
    //�����ŶӸ���ʱ���������е���
    HeroInfo hero_info_arr[3]; //hero_id=0��ʾ��Ч

    //
    unsigned int zhuangbei_count;
    //variable  array of ZhuangbeiDetail
    char data[1];
};

struct SPCmd_TeamBossUpdateRoomRsp
{
    int err_code;
    
    HeroInfo new_hero_info;

    //��������װ��
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
    unsigned char order; //����1~n
    unsigned int boss_hp; //��boss����Ѫ��
};

struct SPCmd_TeamBossGetBattleScriptGiftRsp
{
    int err_code;

    //����
    TeamBossScoreData score_arr[4];

    //�õ������б���
    unsigned char gift_available_count;
    GiftData gift_data_arr[9];

	// �ű�����
	char fightScript[1];
};

struct SPCmd_CityGuardEnter
{
    //ռλ
    unsigned char stub;
    
};

struct SPCmd_CityGuardEnterRsp
{
    int err_code;

    //��ǰ�Ѿ��������� 0~x
    unsigned int sucess_turn;
    //��ǰ�Ѿ������������
    unsigned int current_silver;
    
    //���һ���س�ʤ������0~x
    unsigned int last_total_turns;
    //���һ���سǵ���������
    unsigned int last_silver;
};

struct SPCmd_CityGuardGetNextMonster
{
    //ռλ
    unsigned int stub;
};

struct SPCmd_CityGuardGetNextMonsterRsp
{
    int err_code;

    //��һ����������
    unsigned int monster_leader_profession;

    //��һ���ͽ�
    unsigned int silver;
};

struct SPCmd_CityGuardFight
{
    //ӭս������ȴ
    bool fight;
};

struct SPCmd_CityGuardFightRsp
{
    int err_code;

    //���ѡ����ȴ����ս�ܣ��������ǽ������ݡ�
    //����Ϊ�����سǻ��۵����ҡ�
    unsigned int silver;
    
    //��ʾ����Ƿ�ѡ����ӭս
    bool isFighting;
    //���isFightingΪtrue��������ս���ű�
    char fightScript[1];
};

struct SPCmd_TopHeroGetList
{
    //ռλ
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
    HeroAndOrder challenger;		//��ս��	ps: orderΪս���������
    HeroAndOrder be_challenger;		//����ս��
    
    unsigned int other_order;		//ս����һ���ĵ�ǰ����

	bool challenger_win;			//true: ��ս��ʤ��; false: ����ս��ʤ��
	unsigned int fight_time;		//ս����ʱ���
};

struct SPCmd_TopHeroGetListRsp
{
    int err_code;

    //��ǰ����
    unsigned int my_order;

    unsigned int next_chlg_time; //��һ����ս��ʱ���(PS:��ǰʱ��>=��ʱ���ʱ�Կ���ս ��֮������ս)

    //top10
    HeroAndOrder top10_hero[10];

    unsigned int hero_count;	//�������Լ�ǰ��Ӣ�۸���
	unsigned int report_count;	//ս������

	char data[0];
    //HeroAndOrder hero_arr[1];			//�������Լ�ǰ��Ӣ����Ϣ
	//TopHeroReport report_arr[0];		//ս��
};

struct SPCmd_TopHeroChallenge
{
    //Ҫ��ս˭
    unsigned int hero_id;
};

struct SPCmd_TopHeroChallengeRsp
{
    int err_code;

    //ս��֮�������
    unsigned int order_after_fight;
	
    unsigned int next_chlg_time; //��һ����ս��ʱ���(PS:��ǰʱ��>=��ʱ���ʱ�Կ���ս ��֮������ս)

    //ǰ���������
    //unsigned int hero_count;
    //HeroAndOrder hero_arr[0];
    
    // �ű�
	char fightScript[1];
};

//�����ս��ȴʱ�������
struct SPCmd_TopHeroClearCDTime
{
    int stub; 			//ռλ
};

//�����ս��ȴʱ�䷵�ذ�
struct SPCmd_TopHeroClearCDTimeRsp
{
    int err_code;

	unsigned int gold_balance; 		//��ǰ������
	unsigned int next_chlg_time; //��һ����ս��ʱ���(PS:��ǰʱ��>=��ʱ���ʱ�Կ���ս ��֮������ս) 
};



struct SPCmd_SystemGiftQuery
{
    //ռλ
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
    //��ǰ�Ƿ�����ȥ
    bool is_rent;
    //���뵱ǰ��Լ����ʱ��(��λ����)
    unsigned int time_to_rent_end;

    //��ǰ��߾���ۣ������ǰ����Լ��Ϊ�����˱��
    unsigned int highest_price;

	// �䳤�������
	ZhuangbeiDetail zb_detail;
};

struct SPCmd_RentGetMyZB
{
    //ռλ
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
    //ռλ
    unsigned int stub;
};

struct DuanzaoGroup
{
    unsigned int duanzao_group_id;
    unsigned int open_level; //���ټ����ܿ���
    char group_name[DUANZAO_GROUP_NAME_MAX_LEN+1];

    //�������
    unsigned int item_id; //Ϊ0��ʾ����Ҫ
    unsigned int item_count;
    //������
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
    //����group_id
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
    unsigned int ratio; 				//��������
    unsigned int next_group_id;			//��һ�ȼ���group_id, Ϊ0����ߵȼ�
    unsigned int need_expend_group_id;	//��������Ҫ�Ĵ���װ��group_id, Ϊ0����ߵȼ�
    unsigned int profession_id;    		//�󶨵�ְҵID, Ϊ0���κ�ְҵ

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
    char attr_str[0]; //attr_str_count�������ַ�����ÿ���ַ�����'\0'����

    //char desc[0]; //������attr_str�����һ��'\0'֮����'\0'���� 
};
struct SPCmd_DuanzaoGroupZBQueryRsp
{
    int err_code;

    unsigned int zb_count;
    char zb_arr[0]; //DuanzaoGroupZBInfo ����
};

struct SPCmd_Duanzao
{
	/*ENUM Profession*/
    unsigned int profession;	//��ȡְҵ,ö��Profession
    bool is_times_10; 		//�Ƿ�Ϊ��10�� true:��10�� false:��1��
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
    //ռλ
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
    //ռλ
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
    //ռλ
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
    //�����񸱱�id
    unsigned int task_id;

    //ɨ������
    unsigned int saodang_time;
    //
    bool auto_fenjie_baizhuang;
    bool auto_fenjie_lanzhuang;
    bool auto_fenjie_huangzhuang;
};

struct SPCmd_SaodangRsp
{
    int err_code;

	
    unsigned int exp;				//����
    unsigned int decomposed_count; 	//�ѷֽ��װ������
    unsigned int item_count;		//����
    ItemInfo item_arr[0];
	
	//item_arr�����װ������
    //unsigned int zb_count;
    //ZhuangbeiDetail zb_detail_arr[0];
};


struct SPCmd_HeroLevelUp
{
    //Ҫ������Ӣ��id
    unsigned int hero_id;
};

struct SPCmd_HeroLevelUpRsp
{
    int err_code;

    //������ʣ������
    unsigned int silver_end;

    //������ʣ������������
    ItemInfo item_end;

    //�������Ӣ����Ϣ
    HeroDetail hero_detial;
};

struct SPCmd_HeroUpgrade
{
    //Ҫ���׵�Ӣ��id
    unsigned int hero_id;
};

struct SPCmd_HeroUpgradeRsp
{
    int err_code;

    //���׺�ʣ������
    unsigned int silver_end;

    //���׺�ʣ�������׵���
    ItemInfo item_end;

    //���׺��Ӣ����Ϣ
    HeroDetail hero_detial;
};


struct SPCmd_ShenbingJieFengyin
{
    unsigned long long zb_id;			//��Ҫ������װ��
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

	//���ղ���
    int count;				//item����
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
    //ռλ
    int stub;
};

struct ArenaTeamBasicInfo
{
    unsigned int free_fight_remainder_time; //���ɾ�������ʣ�����
    unsigned int challenge_remainder_time; //��ս����ʣ�����

    unsigned int jifen; //��ǰ�������ܻ���
    
    unsigned int buy_challenge_remainder_time; //���ջ��ɹ�����ٴ���ս
};
struct SPCmd_QueryArenaTeamBasicInfoRsp
{
    int err_code;

    ArenaTeamBasicInfo team_info;
};

struct SPCmd_QueryArenaMarket
{
    //ռλ
    int stub;
};

struct ArenaMarketItem
{
    //���۵���Ʒ���ͺ�һ���������
    unsigned int item_type;
    unsigned int item_count;

    //�����������
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
    //ռλ
    int stub;
};

struct ArenaChlgerHeroInfo
{
    unsigned int jifen; //սʤ�ɵû���
    bool has_win; //�Ƿ��Ѿ�սʤ��սʤ�˾Ͳ����ٴ���
    
    HeroInfo hero_info;
};

struct SPCmd_RefreshArenaChallengeListRsp
{
    int err_code;

    
    unsigned int gold_balance; 		//��ǰ������
    unsigned int silver_balance; 	//��ǰ�������

	//�´��ֶ�ˢ���б���Ҫ���Ļ����Լ�����
    ItemInfo item_need;		//type:Ϊ0��ѣ�����ItemsType_Gold��ItemsType_Silver

    unsigned int hero_count;
    ArenaChlgerHeroInfo hero_arr[0];
};

struct SPCmd_QueryArenaChallengeList
{
    //ռλ
    int stub;
};

struct SPCmd_QueryArenaChallengeListRsp
{
    int err_code;

	//�´��ֶ�ˢ���б���Ҫ���Ļ����Լ�����
    ItemInfo item_need;		//type:Ϊ0��ѣ�����ItemsType_Gold��ItemsType_Silver

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

    //ս��֮��Ļ���
    unsigned int jifen_after_fight;
    
    // �ű�
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

    //����ɹ���������������ʧ�ܣ����ֶ���Ч
    unsigned int jifen_balance;
};

struct SPCmd_ArenaBuyChallengeTime
{
    //bool is_max_buy; //�Ƿ�����򡣾���һ���԰�ʣ�����ȫ��������
    //������ٴ�
    unsigned int buy_time;
};

struct SPCmd_ArenaBuyChallengeTimeRsp
{
    int err_code;

    //�����ʣ����ս����
    unsigned int challenge_remainder_time;
    //���ջ��ɹ�����ٴ�
    unsigned int buy_challenge_remainder_time;

    //�����Ľ�����
    unsigned int gold_balance;
};




struct SPCmd_QueryZahuopu
{
    //ռλ
    int stub;
};

struct ZahuopuItem
{
    unsigned int item_type;
    unsigned int suipian_id; //Ϊ0��ʾ������item��������Ƭ
    unsigned int count; //��Ƭ��item������
    bool already_got;
};
struct SPCmd_QueryZahuopuRsp
{
    int err_code;

    unsigned int silver_need_for_refresh; 	//�´��ֶ�ˢ���б���Ҫ������������
    unsigned int draw_need_coupons;			//��ȡ�ӻ��̵���ʱ�������ȯ
    unsigned int coupons_balance;			//��ǰʣ��Ĵ���������

    ZahuopuItem item_arr[6];
};

struct SPCmd_RefreshZahuopu
{
    //ռλ
    int stub;
};

struct SPCmd_RefreshZahuopuRsp
{
    int err_code;

    
    unsigned int silver_balance;			//����ˢ�º�ʣ����������
    unsigned int silver_need_for_refresh; 	//�´��ֶ�ˢ���б���Ҫ������������
    unsigned int draw_need_coupons;			//��ȡ�ӻ��̵���ʱ�������ȯ
    unsigned int coupons_balance;			//��ǰʣ��Ĵ���������

    ZahuopuItem item_arr[6];
};

struct SPCmd_ZahuopuGetOneItem
{
    //ռλ
    int stub;
};

struct SPCmd_ZahuopuGetOneItemRsp
{
    int err_code;

    unsigned int get_index; 				//0~5
    unsigned int gold_balance;				//���γ�ȡ�������
    unsigned int coupons_balance;			//��ǰʣ��Ĵ���������
};


struct SPCmd_QueryHellTaskHeroInfo
{
    //ռλ
    int stub;
};
struct SPCmd_QueryHellTaskHeroInfoRsp
{
    int err_code;
    
    unsigned int battle_time_remainder; //���ջ�ʣ����
    unsigned int last_active_task_id; //��ǰ���ȵ��Ǹ�������
};

struct SPCmd_HellTaskBattle
{
    unsigned int task_id;
};

struct SPCmd_HellTaskBattleRsp
{
    int err_code;

	int fightNum;	// ս���Ĵ���
	// �ű����� ����
	char fightScript_array[1];

	//Battel_FightScript fightScript;
};


struct SPCmd_QueryYuanshenOfHero
{
    unsigned int hero_id; //֧�ֲ�ѯ�����ŶӵĽ�ɫ
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
    //ռλ
    int stub;
};

struct SPCmd_ChurchLevelUpRsp
{
    int err_code;

    //��������õȼ�
    unsigned int church_level;
    
    //������ʣ���ٲ���
    unsigned int dalishi_balance;
    unsigned int huagangyan_balance;
    unsigned int mucai_balance;
    unsigned int shuijinshi_balance;

    //������ʣ��������
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

    ////��������Ҫˢ�¸ü����������
    ItemInfo skill_book_end;

    //���һ�ʣ����
    unsigned int silver_balance;
};

struct SPCmd_GetZhuangbeiAlbum
{
	/*ENUM Profession*/
    unsigned int profession;	//��ѯְҵ,ö��Profession
};

struct SPCmd_GetZhuangbeiAlbumRsp
{
    int err_code;

    unsigned int legend_count;		//����װ������
	unsigned int xiyou_count;		//ϡ��װ������
	
    char zb_arr[1]; 	//DuanzaoGroupZBInfo arr
};



struct CityGuardAnotherHeroShanghai
{
    unsigned int profession_id;
    char name[HeroNameMaxLen+1];

    unsigned int shanghai; //�˺�ֵ
};

struct SPCmd_CityGuardProcessNotify
{
    int err_code;

    //�ͻ�����Ҫ�ö������������˺����̡�
    unsigned int play_time; 

    //��play_timeʱ�����boss��Ѫ����Ҫ������ô��
    unsigned int boss_hp_start; //Ϊ0��ʾboss�ѱ���ɱ
    unsigned int boss_hp_end;

    //��Ҫ���ŵ��˺�
    unsigned int hero_count;
    CityGuardAnotherHeroShanghai another_hero[0]; //�䳤
};

struct SPCmd_getCityGuardCurrentStat
{
    //ռλ
    int stub;
};


struct cityGuardStatInfo
{
    bool is_opening;
    
    unsigned int boss_blood_total; //bossԭʼ��Ѫ��

    unsigned int time_before_open; //�����´��سǻ�ʣ������
};
struct SPCmd_getCityGuardCurrentStatRsp
{
    int err_code;

    cityGuardStatInfo stat;

    //�Ƿ����ҵĽ�����û����ȡ
    bool have_reward;
};

struct SPCmd_cityGuardBattle
{
    //�Ƿ�������CDʱ��
    bool is_skil_cd;
};


struct CityGuardBattleData
{
    //�Ƿ�boss�ѱ���ɱ�ˣ�����ѱ���ɱ��������ս���ű���Ч��
    bool is_boss_dead;
    
    //����ս����ĸ���ͳ������
    unsigned int total_shanghai; //��boss�����˺�
    unsigned int shanghai_order; //��ǰ�˺�����

	char fightScript[0];
};

struct SPCmd_cityGuardBattleRsp
{
    int err_code;

    //������ȴʱ�����Ľ�Һ��ʣ����
    //�������ս��ʱboss�������򲻻�۽��
    unsigned int gold_balance;
    
    CityGuardBattleData battle_data;
};

//�˳������س������
struct SPCmd_QuitCityGuard
{
    //ռλ
    int stub;
};

//�˳������سǷ��ذ�
struct SPCmd_QuitCityGuardRsp
{
    int err_code;
};


struct SPCmd_getCityGuardLastDayReport
{
    //ռλ
    int stub;
};

#define CITYGUARD_HERO_ORDER_COUNT 20
#define CITYGUARD_LUCKY_COUNT 10
struct CityGuardReport
{
    unsigned int cityguard_total_time; //�س��ܺ�ʱ

    HeroInfo hero_kill_boss; //��ɱboss��
    ItemInfo hero_kill_reward; //��ɱ�߽���
    HeroInfo hero_order[CITYGUARD_HERO_ORDER_COUNT]; //����˺�ǰ20����
    ItemInfo hero_order_reward[CITYGUARD_HERO_ORDER_COUNT]; //����˺�ǰ20���ֱ�õ�ʲô����
    HeroInfo hero_lucky[CITYGUARD_LUCKY_COUNT]; //10�����˽�
    ItemInfo hero_lucky_reward; //���˽���Ʒ

	ItemInfo join_reward;  	//���뽱
};

struct SPCmd_getCityGuardLastDayReportRsp
{
    int err_code;

    //�Ƿ����ҵĽ�����
    bool have_reward;

    CityGuardReport lastday_report;
    
};

struct SPCmd_GetMyCityGuardReward
{
    //ռλ
    int stub;
};

struct SPCmd_GetMyCityGuardRewardRsp
{
    int err_code;

	unsigned int count;
    ItemInfo item_arr[0];
};


/*  ����Э��  */
//��������
enum BonusType {
	BonusType_Invalid 		= 0,	//��Ч����
	BonusType_DailyLogin 	= 1,	//ÿ�յ�½����
	BonusType_Level 		= 2,	//�ȼ����� 
	BonusType_DayWelfare  	= 3,	//���ո���(�ճ�����)
	BonusType_Reward 		= 4,	//��������(�ܳ�����)
	BonusType_All 			= 10,	//��������(��ѯʱ����)
};

//�����ʼ���Ϣ
struct BonusInfo {
    unsigned int type;			//BonusType
    bool is_get;		   		//�Ƿ���ȡ���� false:δ��ȡ, true:����ȡ
    bool enable;		   		//�Ƿ������ȡ���� false:������ȡ true:������ȡ

	/*ps: bonus_id �� type �ֱ�Ϊ
		BonusType_DailyLogin: ��ȡ����
		BonusType_Level: �ȼ�
	*/
    unsigned int bonus_id; 		//ÿ�ֽ�����ID


	/* ��������Ϊ�䳤 ������� */
	//unsigned int vitality;		//0:û��������Ϣ >0: ����������ֵ
	//unsigned int item_count;		//���߲���
	//ItemInfo item_arr[0];			//ItemInfo

	//unsigned int item_suipian_count;				//������Ƭ������ʯ��Ƭ�ͼ�������Ƭ
	//ItemSuipianInfo item_suipian_arr[0];			//ItemSuipianInfo

	//unsigned int zb_suipian_count;				//װ����Ƭ
	//ShenbingSuipianInfo zb_suipian_arr[0];		//ShenbingSuipianInfo

	//unsigned int gift_bag_count;		//���
	//GiftBag gift_bag_arr[0];			//GiftBag

	//unsigned int ran_zb_count;	//���װ������
	//unsigned int zb_count;		//װ������
	//char zb_arr[0];				//DuanzaoGroupZBInfo

	/* typeΪBonusType_DayWelfare��BonusType_Reward ����������*/
	//unsigned int vip_level; 			//>0:��ȡ�౶������vip�ȼ��ֽ��� 0:ֻ��1������ 
	//unsigned int multiple;			//����
	//char description[1]; //��������Ϊ�䳤, ��'\0'����

	BonusInfo() {
		memset(this, 0, sizeof(BonusInfo));
	};
};



//��ѯ���������
struct SPCmd_QueryBonus {
    unsigned int type;		//BonusType
};

//��ѯ�������ذ�
struct SPCmd_QueryBonusRsp {
    unsigned int err_code;

    unsigned int type[4];		//4�ֲ�ͬ������BonusType
    int count[4];				//4��bonus������
    char bonus_arr[0]; 			//BonusInfo
};

//��ȡ�����������
struct SPCmd_GetBonus {
    unsigned int type;		//BonusType
    unsigned int id;		//��ȡid
};

//��ȡ�������ذ�
struct SPCmd_GetBonusRsp {
    unsigned int err_code;
	
    unsigned int type;		//BonusType
    unsigned int id;		//��ȡid
    
	unsigned int zb_count;
	char detail[1];			//װ��detail
};

//��������֪ͨ��
struct SPCmd_SendBonusNotify {
    unsigned int err_code;
	
    unsigned int type;		//BonusType
	unsigned int id;		//�ͻ����յ�����Ϣ�󣬰Ѵ�id�Ľ����óɿ�����ȡ����״̬
};


/* �ʼ� */
//�ʼ�����
enum EmailType {
	EmailType_Bonus 		= 0,	//ϵͳ����  �и�����
	EmailType_Compensate 	= 1,	//ϵͳ����  �и�����
	EmailType_Notice 		= 2,	//֪ͨ���� 
	EmailType_BeFriend 		= 3,	//�������� 
	EmailType_UnFriend 		= 4,	//ɾ������ 
	EmailType_Player 		= 5,	//�����ʼ�
};

//�����ʼ���Ϣ
struct EmailInfo {
    unsigned int type;			//EmailType
    bool is_read;		   		//�Ƿ��Ķ� false:δ��, true:�Ѷ�
    unsigned int id; 			//�ʼ�ID
    unsigned int addresser_id;	//������id 0:Ϊϵͳ >0:���
    char addresser[HeroNameMaxLen +1];	//����������
    char content[1]; 	//�ʼ�������������

	//�ʼ�����Ϊ�䳤, ��'\0'����, �������������Ҫ, Ҳ��'\0'����
    //char summary[1]; 	//�ʼ���Ҫ
		
	//bool is_have_attach;		//�Ƿ��Ǹ��� false: ֪ͨ���ʼ�(û����Ʒ), true: ����/�������ʼ�(����Ʒ)		
	/* ����Ϊ������Ϣ */
    
    
	//unsigned int vitality;		//0:û��������Ϣ >0: ����������ֵ
	//unsigned int item_count;		//���߲���
    //ItemInfo item_arr[0];			//ItemInfo

	//unsigned int item_suipian_count;				//������Ƭ������ʯ��Ƭ�ͼ�������Ƭ
    //ItemSuipianInfo item_suipian_arr[0];			//ItemSuipianInfo

	//unsigned int zb_suipian_count;				//װ����Ƭ
    //ShenbingSuipianInfo zb_suipian_arr[0];		//ShenbingSuipianInfo

	//unsigned int gift_bag_count;		//���
    //GiftBag gift_bag_arr[0];			//GiftBag

    //unsigned int ran_zb_count;	//���װ������
    //unsigned int zb_count;		//װ������
	//char zb_arr[0];				//DuanzaoGroupZBInfo

	EmailInfo() {
		memset(this, 0, sizeof(EmailInfo));
	};
};

//��ѯ�ʼ������
struct SPCmd_QueryEmail {
    unsigned int stub;		//��ʱ
};

//��ѯ�ʼ����ذ�
struct SPCmd_QueryEmailRsp {
    unsigned int err_code;

    int count;			//email����
    char email_arr[0]; 	//EmailInfo
};

//�Ķ��ʼ������
struct SPCmd_ReadEmail {
    unsigned int id;
};

//�Ķ��ʼ����ذ�
struct SPCmd_ReadEmailRsp {
    unsigned int err_code;
	
	unsigned int zhuangbei_count;
	char detail[1];		//װ��detail
};

//д�ʼ������
struct SPCmd_WriteEmail {
	unsigned int receiver_id;	//������id 0:�㲥 >0:���
    char content[CONTENT_MAX_LEN +1]; 	//�ʼ�������������
};

//д�ʼ����ذ�
struct SPCmd_WriteEmailRsp {
    unsigned int err_code;
};

//ɾ�ʼ������
struct SPCmd_DeleteEmail {
    unsigned int id;
};

//ɾ�ʼ����ذ�
struct SPCmd_DeleteEmailRsp {
    unsigned int err_code;
};

//�����ʼ�֪ͨ��
struct SPCmd_PushEmail {
    unsigned int err_code;

    int count;			//email����
    char email_arr[0]; 	//EmailInfo
};



/* ��� */
//���
struct GiftBag {
    unsigned int id;		//���id
    unsigned int count; 	//�������
};

//���
struct GiftBagInfo {
    unsigned int vitality;		//0:û��������Ϣ >0: ����������ֵ
    unsigned int item_count;		//���߲���
    ItemInfo item_arr[0];			//ItemInfo

	//�������Ϊ�䳤, �������Ƭװ����
	//unsigned int item_suipian_count;				//������Ƭ������ʯ��Ƭ�ͼ�������Ƭ
    //ItemSuipianInfo item_suipian_arr[0];			//ItemSuipianInfo

	//unsigned int zb_suipian_count;				//װ����Ƭ
    //ShenbingSuipianInfo zb_suipian_arr[0];		//ShenbingSuipianInfo

	//unsigned int gift_bag_count;		//���
    //GiftBag gift_bag_arr[0];			//GiftBag

    //unsigned int zhuangbei_count;		//װ������
	//char detail[0];				//ZhuangbeiDetail
};


//��ѯ��������
struct SPCmd_QueryGiftBag {
    unsigned int stub;		//��ʱ
};

//��ѯ������ذ�
struct SPCmd_QueryGiftBagRsp {
    unsigned int err_code;

    int count;				//giftbag����
    char gift_bag_arr[0]; 	//GiftBag
};

//����������
struct SPCmd_GetGiftBag {
    unsigned int id;
};

//��������ذ�
struct SPCmd_GetGiftBagRsp {
    unsigned int err_code;
	
    GiftBagInfo info;
};



/*  �۷�Ծ�  */
//�۷�Ծ�����״̬
enum TopArenaStat
{
    TopArenaStat_Invalide    = 0,
    TopArenaStat_GuessOpen   = 1,
    TopArenaStat_Finished    = 2,
};

//�۷�Ծ���������
enum TopArenaRoundType
{
	TopArenaRoundType_NUL = 0,
 	TopArenaRoundType_8_4 = 1,
 	TopArenaRoundType_4_2 = 2,
 	TopArenaRoundType_2_1 = 3,
};

//�۷�Ծ���ע
enum TopArenaWagerType
{
 	TopArenaWagerType_Low 	= 1,	//8000����
 	TopArenaWagerType_High 	= 2,	//16000����
};

//�۷�Ծ�С���Ա��Ϣ
struct TopArenaInfo {
	//unsigned short round;    	//������������, 0:δ��ʼ, 1:8��4, 2:4��2, 3:2��1
	HeroInfo hero_a;			//Ӣ����Ϣ
	HeroInfo hero_b;	    
	bool winer;					//true: aʤ false: bʤ
};

//
struct TopArenaGuessInfo {
	bool is_joined;				//�Ƿ���뾺��, true:�Ѿ��μ�, false:û�вμ�
	TopArenaInfo player;		//ѡ�ֱ������
	unsigned short guess_winer;	//0: aʤ 1: bʤ
	unsigned short result;		//0:�����δ���� 1: ������ȷ 2: ���´���
	unsigned int wager;			//��ע
};


//��ȡ�۷�Ծ���Ϣ�����
struct SPCmd_TopArenaQueryStatus{
    unsigned int stub;			//��ʱ
};


//��ȡ�۷�Ծ���Ϣ���ذ�
struct SPCmd_TopArenaQueryStatusRsp {
    unsigned int err_code;

	unsigned int next_top_arena_time;	//�´��۷�Ծ�����ʱ�� 0:�������, >0:�δ��ʼ

	/* ���3�龺����Ϣ  TopArenaStat*/
	unsigned int stat[3];			//�±�0Ϊ8��4�� �±�1Ϊ4��2�� �±�2Ϊ2��1
	
	/* ���3�龺����Ϣ*/
	TopArenaGuessInfo guess[3];		//�±�0Ϊ8��4�� �±�1Ϊ4��2�� �±�2Ϊ2��1
	
	TopArenaInfo player_arr_8_4[4];	//8��4ѡ��
	TopArenaInfo player_arr_4_2[2]; //4��2ѡ��
	TopArenaInfo player_arr_2_1[1]; //2��1ѡ��
};


//���������
struct SPCmd_TopArenaGuess {
	unsigned short round;    	//��ǰ��������, 0:δ��ʼ, 1:8��4, 2:4��2, 3:2��1
	unsigned short group;		//����С��, 0:��1�� 1:��2�� 2:��3�� 3:��4��
	bool guess_winer;			//true: aʤ false: bʤ
	unsigned int wager;			//��ע
};


//���·��ذ�
struct SPCmd_TopArenaGuessRsp {
    unsigned int err_code; 
	unsigned int silver_balance; 	//��ǰʣ���������
};

//�ط������
struct SPCmd_TopArenaReplay{
	unsigned short round;    	//��������, 0:δ��ʼ, 1:8��4, 2:4��2, 3:2��1
	unsigned short group;		//����С��, 0:��1�� 1:��2�� 2:��3�� 3:��4��
	unsigned short times;		//��������, 0:��1�� 1:��2�� 2:��3��
};

//�طŷ��ذ�
struct SPCmd_TopArenaReplayRsp {
    unsigned int err_code;
	
    char fight_script[1];		//ս���ű�
};

//��ѯ���������
struct SPCmd_TopArenaQueryGuess {
	unsigned short round;    	//��ǰ��������, 0:δ��ʼ, 1:8��4, 2:4��2, 3:2��1
	unsigned short group;		//����С��, 0:��1�� 1:��2�� 2:��3�� 3:��4��
};

//��ѯ���·��ذ�
struct SPCmd_TopArenaQueryGuessRsp {
    unsigned int err_code; 
	
	unsigned int hero_value_a; 	//��ǰ��� MIN_HERO_VALUE -- MAX_HERO_VALUE
	unsigned int hero_value_b; 	//��ǰ���            16W -- 9000W
};


/* �ؿ����� */
#define MAX_TASK_NUM	 40
#define MAX_CHAPTER_NUM	 4

//�ؿ��½�����
enum RateStarType {
    RateStarType_Invalid = 0,	//��Ч
    RateStarType_Task = 1,		//�ؿ�
    RateStarType_Chapter = 2,	//�½�
};

//�ؿ������ȼ�
enum RateStarLevel {
    RateStarLevel_Invalid = 0,	//��Ч
    RateStarLevel_Cuprum = 1,	//ͭ��
    RateStarLevel_Silver = 2,	//����  //�½ڽ���û������
    RateStarLevel_Gold = 3,		//����
};

//�ؿ�����״̬��Ϣ
struct RateStarInfo {
    unsigned short star;		//RateStarLevel �Ǽ� 
    bool is_get_cuprum;			//ͭ����ȡ״̬ false: δ��ȡ true:��ȡ
    bool is_get_silver;			//������ȡ״̬ �½ڽ��� �˱�־λû��
    bool is_get_gold;			//������ȡ״̬ 

	RateStarInfo() {
		memset(this, 0, sizeof(RateStarInfo));
	}
};

//�ؿ�����״̬��Ϣ
struct RateBonus {
    unsigned short star;		//RateStarLevel �Ǽ� 
    unsigned int exp;			//�������� 0:û�д˽���, >0:�о���
    
    unsigned short item_count;
	//�䳤
    //ItemInfo item_arr[0];
};

//��ѯ�ؿ�����״̬�����
struct SPCmd_QueryRateTask {
    unsigned int stub;			//��ʱ
};

//��ѯ�ؿ�����״̬���ذ�
struct SPCmd_QueryRateTaskRsp {
    unsigned int err_code;

    unsigned short task_max;					//�ؿ����� 1-40
    RateStarInfo task_info[MAX_TASK_NUM]; 		//�ؿ����� 
    unsigned short chap_max;					//�ؿ����� 1-4
    RateStarInfo chap_info[MAX_CHAPTER_NUM];	//�½�����
	
};

//��ѯ�������������
struct SPCmd_QueryRateTaskBonus {
    unsigned short type;		//RateStarType
    unsigned short task_id;		//�ؿ�:1-40, �½�:1-4
};

//��ѯ�����������ذ�
struct SPCmd_QueryRateTaskBonusRsp {
    unsigned int err_code;

    unsigned short bonus_count;	//�Ǽ���Ӧ����
    RateBonus bonus_arr[0];		//����
};

//��ȡ�������������
struct SPCmd_GetRateTaskBonus {
    unsigned short type;		//RateStarType 0:�ؿ�����, 1:�½�����
    unsigned short task_id;		//�ؿ�:1-40, �½�:1-4
    unsigned short star;		//�Ǽ�
};

//��ȡ�����������ذ�
struct SPCmd_GetRateTaskBonusRsp {
    unsigned int err_code;
};



/* VIP��Ȩ��Ϣ */
struct VipPrivilegeInfo
{
	unsigned int buy_vitality_num;		//������������
	unsigned int sweep_num;				//����ɨ������
	unsigned int buy_arena_num;			//���򾺼�����ս����
	unsigned int over_creep_num;		//�ؿ�����С����
	unsigned int cast_silver_num;		//��������

	//������ʱδ��
	bool honor_exchange;				//�Ƿ��������һ����� false:û�� true:����
	unsigned int refresh_reward_num;	//���ˢ�������������
};

//��ѯVIP��Ȩ״̬�����
struct SPCmd_QueryVipPrivilege {
    unsigned int stub;			//��ʱ
};

//��ѯVIP��Ȩ״̬���ذ�
struct SPCmd_QueryVipPrivilegeRsp {
    unsigned int err_code;

	VipPrivilegeInfo deft_info;			//��ǰvipĬ�ϵ���Ȩ��Ϣ�����ģ�
	VipPrivilegeInfo priv_info;			//��ǰvip��Ȩ��Ϣ��ʣ��ģ�
};

/* ������� */
//������������
struct SPCmd_UpgradeShenbing {
    unsigned long long zb_id;
    unsigned long long cailiao_zb_id;
};

//����������ذ�
struct SPCmd_UpgradeShenbingRsp {
    unsigned int err_code;

	//���ղ���
    ItemInfo item_arr[5];
	
	char detail[1];		//װ��detail
};

/* �������� */
//�������������
struct SPCmd_UnlockAssistant {
    unsigned int assistant_id;
};

//�������ַ��ذ�
struct SPCmd_UnlockAssistantRsp {
    unsigned int err_code;

	unsigned int honor;		//ʣ������
};


/* �������ͻ */
//���ͻ����
enum ActivityType {
	ActivityType_12_14 		= 1,		//�������ͻ 12:00-14:00
    ActivityType_18_20 		= 2,		//�������ͻ 18:00-20:00
};

//�������Ϣ
struct ActivityInfo {
    unsigned int id; 					/* ActivityType */
    bool is_get;		   				//�Ƿ���ȡ� false:δ��ȡ, true:����ȡ
    unsigned int open_time;				//�����ʱ��
    unsigned int close_time;			//�����ʱ��
};

//��ѯ�������ͻ�����
struct SPCmd_QueryGiftActivity {
    unsigned int stub;					//��ʱ
};

//��ѯ�������ͻ���ذ�
struct SPCmd_QueryGiftActivityRsp {
    unsigned int err_code;

	unsigned int count;
	ActivityInfo act_arr[0];
};

//��ȡ�������ͻ�����
struct SPCmd_GetGiftActivity {
    unsigned int id;					/* ActivityType */
};

//��ȡ�������ͻ���ذ�
struct SPCmd_GetGiftActivityRsp {
    unsigned int err_code;

	unsigned int vit_current;			//��ǰʣ������
};


/* ��ɱ�����ۼ� */
//�ۼ�����
enum AccumulateType {
	Accumulate_Common_Monster 	= 1, 	//��ͨС��
	Accumulate_Elite_Monster	= 2,	//��Ӣ��
};

//��ѯ��ɱ���ｱ�������
struct SPCmd_QueryKilledMonsterBonus {
    unsigned int stub;					//��ʱ
};

//��ѯ��ɱ���ｱ�����ذ�
struct SPCmd_QueryKilledMonsterBonusRsp {
    unsigned int err_code;

	unsigned int common_bonus_count;
	unsigned int elite_bonus_count;

	ItemInfo item_arr[0];
};

//��ѯ��ɱ�����ۼ������
struct SPCmd_QueryKilledMonster {
    unsigned int stub;					//��ʱ
};

//��ѯ��ɱ�����ۼƷ��ذ�
struct SPCmd_QueryKilledMonsterRsp {
    unsigned int err_code;

	unsigned int common_monster_count;
	unsigned int elite_monster_count;
};

//��ȡ��ɱ�����ۼƽ��������
struct SPCmd_GetKilledMonsterBonus {
    unsigned int type;					/* AccumulateType */
};

//��ȡ��ɱ�����ۼƽ������ذ�
struct SPCmd_GetKilledMonsterBonusRsp {
    unsigned int err_code;
};


/* ���� */
//���������
struct SPCmd_CastSilver {
    unsigned int stub;					//��ʱ
};

//�������ذ�
struct SPCmd_CastSilverRsp {
    unsigned int err_code;

    unsigned int gold_balance;
	unsigned int silver_balance;
};


/* ����¼ */
//��ѯ����¼�����
struct SPCmd_QueryLegendNote
{
    unsigned int stub;				//��ʱ
};

//��ѯ����¼���ذ�
struct SPCmd_QueryLegendNoteRsp
{
    int err_code;
	
    unsigned int times;				//ʣ����ս����
};

//��ս����¼�����
struct SPCmd_ChallengeLegendNote
{
    unsigned int group_id;			//��սװ��id
};

//��ս����¼���ذ�
struct SPCmd_ChallengeLegendNoteRsp
{
    int err_code;
	
    char fight_script[1];		//ս���ű�
};


/* Ѫɫ���� */
//������ս�Ѷ�����
enum SelfChlgDiffType {
	SelfChlgDiffType_NULL		= 0,		//û��ѡ���Ѷ�
	SelfChlgDiffType_Common		= 1,		//��ͨ
	SelfChlgDiffType_Difficult	= 2,		//����
	SelfChlgDiffType_Hell		= 3,		//����
	SelfChlgDiffType_Nightmare	= 4,		//����
};

//������ս�ӳ�����
enum SelfChlgGainType {
	SelfChlgGainType_HP			= 1,		//����
	SelfChlgGainType_Attack		= 2,		//����
	SelfChlgGainType_Defense	= 3,		//����
};

//������ս�ӳɳ̶�
enum SelfChlgGainDegree {
	SelfChlgGainDegree_Low		= 1,		
	SelfChlgGainDegree_High		= 2,		
};

//������ս��Ϣ
struct SelfChlgInfo
{
	bool b_on_the_way;				//true:���ڴ���״̬; false: δ���봳��
    unsigned int times;				//ʣ����ս����

    unsigned int energy;			//ʣ������
    unsigned int hp_gain;			//�����ӳ�
    unsigned int attack_gain;		//�����ӳ�
    unsigned int defense_gain;		//�����ӳ�

	unsigned int difficulty;		///* SelfChlgDiffType */�����Ѷ�
	unsigned int checkpoints;		//�Ѵ��ؿ�
};

//������ս������Ϣ
struct SelfChlgOrder
{
    HeroInfo hero_info;
    unsigned int order;
	unsigned int checkpoints;
	ItemInfo item;				//��������
};

//��ѯ������ս��Ϣ�����
struct SPCmd_QuerySelfChallengeInfo
{
    unsigned int stub;				//��ʱ
};

//��ѯ������ս��Ϣ���ذ�
struct SPCmd_QuerySelfChallengeInfoRsp
{
    int err_code;
	
    unsigned int open_time;			//����ʱ��
    unsigned int close_time;		//�ر�ʱ��
    
	SelfChlgInfo info;

};

//��ѯ������ս���������
struct SPCmd_QuerySelfChallengeOrder
{
    unsigned int difficulty;		///* SelfChlgDiffType */�����Ѷ�
};

//��ѯ������ս�������ذ�
struct SPCmd_QuerySelfChallengeOrderRsp
{
    int err_code;

	unsigned int count;
	SelfChlgOrder order_arr[0];
};

//������ս���������
struct SPCmd_SelfChallenge
{
    unsigned int difficulty;		///* SelfChlgDiffType */�����Ѷ�
};

//������ս���ط��ذ�
struct SPCmd_SelfChallengeRsp
{
    int err_code;

	unsigned int energy;			//ʣ������
    unsigned int checkpoints;		//�ؿ�
	bool is_have_box;				//true: ÿ5����3�����佱��ѡ��; false: �ޱ���
	unsigned int silver;			//>0: ÿ20������Ӧ���ҽ���; 0:�����ҽ���

	GiftBag gift_bag;				//��Щ�ؿ�������������  id,count>0 �����
	
    char fight_script[1];			//ս���ű�
};

//������ս�����ӳ������
struct SPCmd_SelfChallengeGain
{
    unsigned int type;				///* SelfChlgGainType */�ӳ�����
    unsigned int degree;			///* SelfChlgGainDegree */�ӳɳ̶�
};

//������ս�����ӳɷ��ذ�
struct SPCmd_SelfChallengeGainRsp
{
    int err_code;

    unsigned int energy;			//ʣ������
    unsigned int hp_gain;			//�����ӳ�
    unsigned int attack_gain;		//�����ӳ�
    unsigned int defense_gain;		//�����ӳ�
};

//��ȡ������ս���佱�������
struct SPCmd_GetSelfChallengeBox
{
    unsigned int stub;				//��ʱ
};

//��ȡ������ս���佱�����ذ�
struct SPCmd_GetSelfChallengeBoxRsp
{
    int err_code;
	
	ItemInfo item;					//�õ��Ľ���
	ItemInfo item_arr[2];			//δ�õ��Ľ���
};


/* �ػ�֮�� */
//�ػ�֮����Ϣ
struct ProtectSpiritInfo
{
	unsigned int spirit_id;			//�ػ�֮��id
    unsigned int profession_id;		//ְҵ
    unsigned int defend_time;		//�ػ�ʱ��
    unsigned int level;				//�ȼ�
    char name[HeroNameMaxLen+1];	//����

	/* PS: ��profession_id Ϊ1-3 �� ���hero_id==spirit_idʱ��Ч */
	bool is_get;					//��ȡ����״̬ true:����ȡ; false: δ��ȡ
};

//��ѯ�ػ�֮����Ϣ�����
struct SPCmd_QueryProtectSpirit
{
    unsigned int chapter;			//�½�id: 1-4
};

//��ѯ�ػ�֮����Ϣ���ذ�
struct SPCmd_QueryProtectSpiritRsp
{
    int err_code;

	HeroInfo first_spirit;
	ProtectSpiritInfo current_spirit;

	bool is_loser;					//true: 24Сʱ�ڱ����ܵ��ػ�֮��; false:����
	
	/* PS: ���hero_id!=spirit_idʱ��Ч */
	bool is_worship;				//Ĥ��״̬ true:��Ĥ��; false: δĤ��
};

//��ս�ػ�֮�������
struct SPCmd_ChallengeProtectSpirit
{
    unsigned int chapter;			//�½�id: 1-4
};

//��ս�ػ�֮�鷵�ذ�
struct SPCmd_ChallengeProtectSpiritRsp
{
    int err_code;
	
    char fight_script[1];			//ս���ű�
};

//��ȡ�ػ�֮�齱�������
struct SPCmd_GetProtectSpiritBonus
{
    unsigned int chapter;			//�½�id: 1-4
};

//��ȡ�ػ�֮�齱�����ذ�
struct SPCmd_GetProtectSpiritBonusRsp
{
    int err_code;

	short item_count;
	ItemInfo item_arr[0];
};

//Ĥ���ػ�֮�������
struct SPCmd_WorshiProtectSpirit
{
    unsigned int chapter;			//�½�id: 1-4
};

//Ĥ���ػ�֮�鷵�ذ�
struct SPCmd_WorshiProtectSpiritRsp
{
    int err_code;
	
    GiftBag gift_bag;				//Ĥ�ݻ�ȡ���
};


/* ̽�� */
//��ѯ̽����Ϣ�����
struct SPCmd_QueryDiscoverInfo
{
    unsigned int stub;				//��ʱ
};

//��ѯ̽����Ϣ���ذ�
struct SPCmd_QueryDiscoverInfoRsp
{
    int err_code;

	unsigned int remainder_times;	//̽��ʣ�����
	unsigned int discovered_times;	//�����Ѿ�̽������
    unsigned int destroy_seal;		//�ݻٷ�ӡ��
    unsigned int unlocked_seal;		//�����ѽ��װ����, ÿ�ֵ���6��ʯ����������ˢ��Ϊ0
   	unsigned int last_modify_timestamp;	//���һ�λظ�̽��������ʱ��� ��������Ѿ����� �򲻹�
};

//̽�������
struct SPCmd_Discover
{
    bool akey_all;					//false: ����̽��, true:һ��̽��; default:true
};

//̽�����ذ�
struct SPCmd_DiscoverRsp
{
    int err_code;

	bool is_destroyed;				//�Ƿ�ݻٷ�ӡ
	unsigned int remainder_times;	//ʣ��̽������
	unsigned int discovered_times;	//�����Ѿ�̽������
   	unsigned int last_modify_timestamp;	//���һ�λظ�̽��������ʱ��� ��������Ѿ����� �򲻹�

	unsigned int count;
	ItemInfo item_arr[0];
};

//̽�����ӡ�����
struct SPCmd_UnlockDiscoverSeal
{
	/*ENUM Profession*/
    unsigned int profession;		//ְҵ,ö��Profession
};

//̽�����ӡ���ذ�
struct SPCmd_UnlockDiscoverSealRsp
{
    int err_code;

    unsigned int destroy_seal;		//�ݻٷ�ӡ��
    unsigned int unlocked_seal;		//�����ѽ��װ����, ÿ�ֵ���6��ʯ����������ˢ��Ϊ0

	unsigned int count;
    char data[1];					//variable  array of ZhuangbeiDetail
};

/* ����װ�� */
//������ѯ�����
struct SPCmd_QueryAssessWeapon
{
    unsigned int group_id;				//��Ҫ������װ��group id
};

//������ѯ���ذ�
struct SPCmd_QueryAssessWeaponRsp
{
    int err_code;
	
    char material_zb[0]; 				//DuanzaoGroupZBInfo
};

//����װ�������
struct SPCmd_AssessWeapon
{
    unsigned long long zb_id;			//��Ҫ������װ��
    unsigned long long material_id;		//����װ
};

//����װ�����ذ�
struct SPCmd_AssessWeaponRsp
{
    int err_code;
	
	//���ղ���
    unsigned int count;
    ItemInfo item_arr[0];
};


/* ϴ�� */
//��ѯϴ�������
struct SPCmd_QueryPurify
{
    unsigned int stub;				//��ʱ
};

//��ѯϴ�����ذ�
struct SPCmd_QueryPurifyRsp
{
    int err_code;

	unsigned int purify_free_times;	//ʣ����Ѵ���
	unsigned int purified_times;	//��ϴ���Ĵ���
};

//ϴ�������
struct SPCmd_Purify
{
    unsigned long long id;			//Ҫϴ����װ��id
    unsigned int hole;				//Ҫϴ�����Կ� 1~5
};

//ϴ�����ذ�
struct SPCmd_PurifyRsp
{
    int err_code;

	unsigned int purify_hole;		//�ж��ٿ��Ǵ򿪵�
	ZBAttr	attr_arr[0];			//������ ���5��
};

//ȷ��ϴ�������
struct SPCmd_ConfirmPurify
{
    unsigned long long id;			//Ҫϴ����װ��id
    bool is_confirm;				//true: ȷ��; false: ȡ�� 
};

//ȷ��ϴ�����ذ�
struct SPCmd_ConfirmPurifyRsp
{
    int err_code;
};


/* ���� */
//������Ϣ
struct OriginInfo
{
    unsigned int group_id;			//װ��group
	unsigned int task_id;			//����ؿ�
};

//��ѯ���������
struct SPCmd_QueryOrigin
{
    unsigned int stub;				//��ʱ
};

//��ѯ�������ذ�
struct SPCmd_QueryOriginRsp
{
    int err_code;

    unsigned int count;
    OriginInfo origin_arr[0];
};


/* ���������Ϣ */
//��ѯӢ��������Ϣ�����
struct SPCmd_QueryHeroOtherInfo
{
    unsigned int stub;				//��ʱ
};

//��ѯӢ��������Ϣ���ذ�
struct SPCmd_QueryHeroOtherInfoRsp
{
    int err_code;

    unsigned int toplist_order;		//Ӣ�۰�����
};


/* ���� */
//��������
enum NoticeActivityType{
    NoticeActivityType_Invalid 		= 0,		//��Ч����
    NoticeActivityType_SilverStar 	= 1,		//����
    NoticeActivityType_GoldStar 	= 2,		//����
};

//������Ϣ
struct NoticeActivityInfo {
	unsigned short type;			//NoticeActivityType
	
	/* �ʱ��Ϊ�䳤 ��'\0'���� */
	//char activty_title[0];		//�����
	
	/* ��������ʱ������ Ҳ��'\0'���� */
	//char activty_time[0];			//�ʱ��
	//char activity_content[0];		//�����
};

//������Ϣ
struct NoticeInfo {
    unsigned short activity_count;		//�˵������
    NoticeActivityInfo activity_arr[0];	//NoticeActivityInfo

};

//��ѯ���������
struct SPCmd_QueryNotice
{
    unsigned int stub;				//��ʱ
};

//��ѯ���淵�ذ�
struct SPCmd_QueryNoticeRsp
{
    int err_code;
    NoticeInfo info;	//������Ϣ	
};


/* �̳� */
//�������� PS: �ʼ������������Ҳʹ���������
enum AttachType {
	AttachType_Gold 		= 0,	//���
	AttachType_Silver 		= 1,	//����
	AttachType_Item 		= 2,	//����
	AttachType_Diamond 		= 3,	//��ʯ
	AttachType_Props 		= 4,	//���� 
	AttachType_PropsDebris 	= 5,	//������Ƭ
	AttachType_ArmsDebris 	= 6,	//װ����Ƭ
	AttachType_Arms			= 7,	//װ��
	AttachType_GiftBag		= 8,	//���
	AttachType_Vitality		= 9,	//����
};

//������������ 
enum LimitType {
	LimitType_VIPLvl 		= 0,	//VIP�ȼ�
	LimitType_MainHeroLvl 	= 1,	//���ǵȼ�
	LimitType_MainSoulLvl 	= 2,	//����Ԫ��ȼ�
};

// ������Ʒ
union AttachItem {
	ItemInfo item;					//���߲��� AttachType <= AttachType_Props
	ItemSuipianInfo item_debris;	//������Ƭ AttachType_PropsDebris
	ShenbingSuipianInfo zb_debris;	//װ����Ƭ AttachType_ArmsDebris
	GiftBag gift;					//��� AttachType_GiftBag
	unsigned int vitality;			//���� AttachType_Vitality
};	


struct MarketItem {
    unsigned int id;

	/*������Ʒ���� �ݶ�ֻ�в��� ���� ��Ƭ ��� ������
	  ��������ȫ�����뵽���֮��*/
    unsigned short sell_type;		//AttachType
    AttachItem sell_item;			//������Ʒ

    bool is_unlimit;				//�Ƿ����ƹ������ true: ����; false: ����
    unsigned int sell_count;		//������� is_unlimit=false��Ч

	/* ����������� �ݶ�ֻ�н�����Ҽ�AttachType_Gold����AttachType_Silver */
    unsigned short need_type;		//AttachType
    AttachItem need_item;			//������Ʒ

    unsigned short discount;		//�ۿ� 1~100 default:100
    unsigned int sale_time;			//���۵���ʱ 0:û�е���ʱ >0:����ϵͳʱ������
    
    unsigned short level_type;		//LimitType
    unsigned int level;				//�ȼ�

    unsigned int bought_times;		//����Ʒ�Ѿ��������

	MarketItem() {
		memset(this, 0, sizeof(MarketItem));
	}
};

//��ѯ�̳������
struct SPCmd_QueryMarket
{
    unsigned int stub;			//��ʱ
};

//��ѯ�̳Ƿ��ذ�
struct SPCmd_QueryMarketRsp
{
    int err_code;

    unsigned short item_count;
    MarketItem item_arr[0];
};

//�����̳���Ʒ�����
struct SPCmd_BuyItemInMarket
{
    unsigned int id;
    unsigned short buy_times;		//������� Ĭ��Ϊ1
};

//�����̳���Ʒ���ذ�
struct SPCmd_BuyItemInMarketRsp
{
    int err_code;
};


/* ��ѧ */
//��ѯ��ѧ�����
struct SPCmd_QueryInstruction
{
    unsigned int stub;			//��ʱ
};

//��ѯ��ѧ���ذ�
struct SPCmd_QueryInstructionRsp
{
    int err_code;

    unsigned long long progress;	//��ѧ��ɽ���
};

//��ɽ�ѧ�����
struct SPCmd_CompleteInstruction
{
    unsigned long long progress;	//��ѧ��ɽ���
};

//��ɽ�ѧ���ذ�
struct SPCmd_CompleteInstructionRsp
{
    int err_code;
};

#pragma pack() //����


#endif //SIMPLEPROTOCOLCMD_H

