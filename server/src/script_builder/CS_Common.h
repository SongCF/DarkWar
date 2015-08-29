#pragma once
#include <string>
#include <vector>
#include <string.h>

struct ZhuangbeiDetail;
struct EquipData;
struct ItemInfo;

#define TEN_TIME_DISCOUNT 0.8f	// ����

//��ɫ��ʼ��װģ��id
#define ZhuangBeiInitNameId_Jian   1
#define ZhuangBeiInitGroupId_Jian   1

#define ZhuangBeiInitNameId_DunPai 5
#define ZhuangBeiInitGroupId_DunPai 5

#define ZhuangBeiInitNameId_FazhangShangshou 7
#define ZhuangBeiInitGroupId_FazhangShangshou 7

#define ZhuangBeiInitNameId_Gong 8
#define ZhuangBeiInitGroupId_Gong 8

//�ض�groupIDװ��////////////////////////////////////////////
//��ʼ��װ���
#define ZhuangBei_FirstDiaoluo_NameId_qingkuijia 10
#define ZhuangBei_FirstDiaoluo_GroupId_qingkuijia 10

//��ʼ��װ�ؿ�
#define ZhuangBei_FirstDiaoluo_NameId_zhongkuijia 11
#define ZhuangBei_FirstDiaoluo_GroupId_zhongkuijia 11

//��ʼ�������
#define ZhuangBei_FirstDaikong_NameId_qingkuijia 12
#define ZhuangBei_FirstDaikong_GroupId_qingkuijia 12

//��ʼ�����ؿ�
#define ZhuangBei_FirstDaikong_NameId_zhongkuijia 13
#define ZhuangBei_FirstDaikong_GroupId_zhongkuijia 13

//��ʼ��ȡ���ֽ�
#define ZhuangBei_FirstCuiqu_NameId_jian 14
#define ZhuangBei_FirstCuiqu_GroupId_jian 14

//��ʼ��ħ���ֽ�
#define ZhuangBei_FirstFumo_NameId_jian 15
#define ZhuangBei_FirstFumo_GroupId_jian 15

//������boss�ƶ�����ϡ���ؿ���
#define ZhuangBei_Task3_NameId_zhongkuijia 16
#define ZhuangBei_Task3_GroupId_zhongkuijia 16

//���Ĺ�boss�ƶ�����ϡ�н�
#define ZhuangBei_Task4_NameId_jian 17
#define ZhuangBei_Task4_GroupId_jian 17

//�ض�װ�����ID
//�ؿ��������ʱ��С�ڵ��ڴ����ID��װ���������ܵ���
#define ZhuangBeiInitGroupId_Max 1000

//���⴦��װ��ID////////////////////////////////////////////
#define SUIT_fashi_level15_faqi_group_id 1411
#define SUIT_cike_level15_kuijia_group_id 1405
#define SUIT_yemanren_level15_kuijia_group_id 1400

#define SUIT_fashi_level30_faqi_group_id 1413
#define SUIT_fashi_level30_kuijia_group_id 1414

#define SUIT_cike_level30_kuijia_group_id 1408
#define SUIT_cike_level30_jiezhi_group_id 1409

#define SUIT_yemanren_level30_kuijia_group_id 1402
#define SUIT_yemanren_level30_jiezhi_group_id 1403

//���⴦��װ��ID end////////////////////////////////////////////


//��������
#define jinglian_time_chuanqi_max 3
#define jinglian_time_shenbing_max 9

//��������֮�����׭�̾���
#define JingwenZhuanke_need_JinglianLevel 5

//���Ž���
#define Level_limit_arena 8
//�سǿ��ŵȼ�����
#define Level_limit_cityguard 20
//��ʯ�ϳ�����Ƕ�ȼ�����
#define Level_limit_baoshi 15
//Ӣ�۰񿪷ŵȼ�����
#define Level_limit_topherolist 11
//�۷�Ծ�����ŵȼ�
#define Level_limit_toparena 25

//�۷�Ծ�����ѡ����С���
#define MIN_HERO_VALUE 160000

//�۷�Ծ�����ѡ��������
#define MAX_HERO_VALUE 90000000

//ÿ����ս����¼������
#define MAX_CHALLENGE_LEGEND_NOTE_TIMES 5
//����¼���ŵȼ�
#define LEVEL_LIMIT_LEGEND_NOTE 5

//Ѫɫ����������ս ÿ����һ�ػ��������
#define PASS_SELFCHLG_CHECKPOINT_ENERGY	15

//��������δװ����
#define MAX_COUNTS_IN_KNAPSACK 100

//�ڲ��ʼ����id��
#define MAX_INTERNAL_EMAIL_ID 10000




// VIP��Ȩ
struct VipPrivilege
{
	//unsigned int vipLevel;					//VIP�ȼ�
	unsigned int MaxBuyTiLiTime;			//�����������
	unsigned int MaxSaoDangTime;			//����ɨ������
	unsigned int MaxBuyArenaChallengeTime;		//�ɹ��򾺼�����ս����
	unsigned int JumpGridNum;				//�ؿ�������С����
	// ������Ȳ���
	bool enableFastRongYuDuiHuan;			// �����������ٶһ�����
	unsigned int freeRefreshTime;			// ���ˢ�������������

	VipPrivilege()
	{
		memset(this,0,sizeof(VipPrivilege));
	}
};


//ϴ���ȼ�
enum PurifyLevel {
	PurifyLevel_Error			= 0,	//����	
	PurifyLevel_IronStar		= 1,	//����	
	PurifyLevel_CuprumStar    	= 2,	//ͭ��	
	PurifyLevel_SilverStar		= 3,	//����
	PurifyLevel_GoldStar		= 4,	//����
	PurifyLevel_YellowDiamond	= 5,	//����	
	PurifyLevel_GreenDiamond	= 6,	//����	
	PurifyLevel_PurpleDiamond	= 7,	//����	
};


//�ȼ�����ID ��Ӧlevel_bonus_temp���ű�Ľ���ID
enum LevelBonusID
{		
	LevelBonusID_5 = 5, 		//5��
	LevelBonusID_10 = 10, 		//10��
	LevelBonusID_15 = 15, 		//15��
	LevelBonusID_20 = 20, 		//20��
	LevelBonusID_25 = 25, 		//25��
	LevelBonusID_30 = 30, 		//30��
};

//ϵͳ����ID ��Ӧsystem_bonus_temp���ű�Ľ���ID
enum SystemBonusID
{		
	/* �ճ����� */	
	BonusID_Killed_Boss = 1001, 		//��ɱ����һ���ؿ�BOSS
	BonusID_Random_Arena = 1002,		//����һ�����������ս
	BonusID_VarietyStore = 1003,		//�ӻ��̷���
	BonusID_Intensify_GoldStar = 1004,	//ǿ����һ�Ž���

	/* �ܳ����� */
	BonusID_Discover = 1005,			//ǰ��Ѱ�����ҵ���һ����ӡʯ��
	BonusID_TopHeroList = 1006,			//����һ��Ӣ�۰���ս
	BonusID_SelfChallenge = 1007,		//Ѫɫ����ͨ��50��
};


namespace CS
{
#ifdef  _CLIENT_
	//�Ƿ�����Ƕ��ʯ
	bool canZBXiangqianBaoshi(unsigned long long eqId);
#endif

	// �������ֽ����zhuangbei_detail����ת����equipdata����
	// ���ص���zhuangbe_detail��ĩβ�������µĿ�ʼ�����ݵĵ�ַ
	char* ntoh_zhuangbei_detail_2_equipdata(ZhuangbeiDetail* detail_arr,int count,std::vector<EquipData>& out);
	char* hton_equipdata_2_zhuangbei_detail(const std::vector<EquipData>& equips,ZhuangbeiDetail* detail_arr_out);

	// ���ְҵ��Ѫ���͹����ӳ�
	// ���Ƕ���ļӳɣ�����һ�ж����������֮���ټ��ϵİٷֱ�
	void getProfesionBloodAndGongJiAddPercent(unsigned int profession, int& bloodAdd,int& gongjiAdd);

	// ������ǵĹ�������
	float getHeroGongjiJuLiByProfession(unsigned int profession);

	// ����ǿ��װ����Ҫ����Ʒ
	unsigned int ItemsNeedForQianghuaZhuangbei(unsigned int zhuangbei_level, unsigned int zhuangbei_colour);

	// ǿ��װ����Ҫ������
	unsigned int QianghuaZhuangbeiNeedSilver(unsigned int zhuangbei_level, unsigned int zhuangbei_colour);
    unsigned int HuituiZhuangbeiNeedSilver(unsigned int zhuangbei_level, unsigned int zhuangbei_colour);

	// ����װ�������� 	retain_count:��������
    unsigned int HuituiZhuangbeiNeedGold(unsigned int retain_count);


	// �ܷ�ɱ�ǿ��
	bool canZBTypeBeQianghua(unsigned int zb_type);

	//���ĳ��װ����Ҫ������
	unsigned int ChaijieZhuangbeiNeedSilver(unsigned int zhuangbei_level, unsigned int zhuangbei_colour);
	
	//���ĳ��װ����õ���Ʒ
	void getItemsForChaijieZhuangbei(unsigned int zhuangbei_level, unsigned int zhuangbei_colour,
		std::vector<ItemInfo> &item_vec);
	
	// �ϳɱ�ʯ��Ҫ�Ľ��
	unsigned int HechengBaoshiNeedGold(unsigned int baoshi_type);
	// �ϳɱ�ʯ��Ҫ�ı�ʯ����
	unsigned int HechengBaoshiNeedBaoshiType(unsigned int baoshi_type);
	// �ϳɱ�ʯ��Ҫ�ı�ʯ�ĸ�������HechengBaoshiNeedBaoshiType�ĸ���
	unsigned int HechengBaoshiNeedBaoshiCount(unsigned int baoshi_type);
	// ĳ�����͵ı�ʯ�Ƿ���Ա��ϳ�
	bool canBaoShiBeHeCheng(unsigned int baoshi_type);

	// ��һ��ĳ�ֱ�ʯ��Ҫ������
	unsigned int XiangbaoshiNeedSilver(unsigned int zb_level, unsigned int baoshi_type);

	// �����Ҫ�Ľ��
	unsigned int ZhuangkongNeedGold(
		unsigned int zhuangbei_level, unsigned int zhuangbei_colour);
	// �Ƿ񻹿������
	bool canZBTypeBeZhuangkong(unsigned int zhuangbei_type,unsigned int hole_count);

	// �Ƿ�
	bool canEquipToHero(unsigned int zhuangbei_type,unsigned int profession_id);

	// ItemsType_Baoshi_lv_1 �� ItemsType_Baoshi_huang_10
	// �����������ݣ����������ܵļӳ�ֵ
	void getBaoShiJiaCheng(unsigned int baoShiType,unsigned int& liLiang,unsigned int& minJie,unsigned int& zhiLi,unsigned int& tiNeng);


    //ÿ������ħ��������������ֵ��Χ
    void getMofaJinghuaAttrValue(int zb_type, int zttr_type, unsigned int mfjh_level
        , unsigned int& value_min, unsigned int& value_max);
    //װ�������Ƿ�����ȡĳ������
    bool isAttrTypeZBTypeCanCuiqu(unsigned int zb_type, unsigned int attr_type);
    
	// ��ȡ���ܵĹ������ݣ����ںʹ�����
	int getSkillZhouQi(unsigned int skill_type);
	int getSkillTurn(unsigned int skill_type);
	//int getSkillResult(unsigned int skill_type);

#if _CLIENT_ 
	//��ñ�ʯ����
	std::string getBaoshiName(unsigned int baoshiType);
	std::string getBaoshiShuxing(unsigned int baoshi_type);
	std::string getItemName(unsigned int item_type);
	std::string getItemShuxing(unsigned int item_type);
	//��ȡװ����������
	std::string getZhuangbeiTypeName(unsigned int zb_type, unsigned int zb_color);
	std::string getZhuangbeiTypeName(unsigned int zb_type);
	std::string getZhuangbeiChengSeName(unsigned int zb_color);
	//��ȡħ������������   �ļ���֮ħ������
	std::string getMofajinghuaName(unsigned int zb_type, unsigned int level);
#endif

	// ���װ��������
	//	EquipType_WuQi = 0,
	//  EquipType_FangJu,
	//  EquipType_ShiPin,
	unsigned int getEquipType(unsigned int type);

	// �Ƿ���˫������
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

	// ����ĵȼ�������������
	std::vector<int> getShenBingLevels();
	// ��ȡ�������һ���ȼ�
	// û��/������ �ͷ���-1
	int getNextShenBingLevel(unsigned int curLevel);

	//��ȡ���Խ������ֵ�Ӣ�۵ȼ�
	int getUnlockAbleHeroLevel();

    //���ظ��������̵ĸ��Ӳ���
    void getTaskChessboardFormat(unsigned int task_id
        , unsigned int& grid_count_heng, unsigned int& grid_count_shu);

 //   //��������ս��VIP�ȼ�ÿ�����ɹ������
 //   unsigned int getVIPArenaChallengeMaxBuyTime(unsigned int vip_level);
	//// ��ȡĳ���ȼ���Vip����
	//VipPrivilege getVipPrivilege(unsigned int vip_level);

    //����������һ����ս����Ҫ�Ľ��
    unsigned int arenaBuyOneChlgTimeNeedGold();

    //�ӻ��̹���һ�ζ��ٽ��
    unsigned int zahuopuGetOneItemNeedGold(unsigned int coupons_balance);

    //������Ҫ������������
    unsigned int heroLevelUpNeedItemCount(unsigned int level);
    unsigned int heroLevelUpNeedSilver(unsigned int level);

    //Ӣ��������Ҫ���ٶ���
    unsigned int heroUpgradeNeedItemCount(unsigned int stage);
    unsigned int heroUpgradeNeedSilver(unsigned int stage);

    //Ԫ��������Ҫ��������
    unsigned int yuanshenUpgradeNeedSilver(unsigned int stage);

	//���ȼ���Ԫ�񶼿�������Щװ����λ
    void openZhuangbeiOfYuanshenLevel(unsigned int yuanshen_level
        , bool& wuqi_open, bool& fushou_open, bool& kuijia_open
        , bool& hushenfu_open, bool& jiezhi_open);
    //���ȼ�Ԫ���Ӧ��ת����
    unsigned int transferRatioOfYuanshenLevel(unsigned int yuanshen_level);

	// ��ȡ��ͬ�ȼ��ļ��ܵ�Ч���ӳ�
	unsigned int getSkillEffectRatioAddBySkillLevel(unsigned int skillLevel);

	// ��ȡĳ�ּ��ܵĹ���Ч������ͨ�����İٷ�֮����
	// ��һ�����ʽ����
	// ���緵��200�����Ǽ��ܹ���ʱ�չ���2��
	unsigned int getSkillEffectRatioOfNormalAttack(unsigned int skillType);


    //��ɫ���׶Խ�ɫ���幥��Ѫ�ӳ�Ч��
    unsigned int getHeroStageEffectRatio(unsigned int stage);

    //����������Ҫ�۶�������
    unsigned int skillLevelUpNeedSilver(unsigned int new_skill_level);

    //
    void churchLevelUpNeedItemCount(unsigned int new_church_level
        ,unsigned int& dalishi_need,unsigned int& huagangyan_need
        ,unsigned int& mocai_need,unsigned int& shuijinshi_need);
    
    unsigned int churchLevelUpNeedSilver(unsigned int new_church_level);

	//��ɱС�ֶ��潱����Ҫ�Ļ�ɱ���� type: 1: ��ͨС��, 2: ��Ӣ��
    unsigned int cashBonusNeedKilledMonsters(unsigned int type);

	//�۷�Ծ����¶�ע type<TopArenaWagerType>: 1: �Ͷ�ע, 2: �߶�ע
    unsigned int getTopArenaWager(unsigned int type);

	//������������ times:�������
    unsigned int castSilverNeedGold(unsigned int times);

	//�������������� level:��Ӣ�۵ȼ�
    unsigned int getCastSilverCount(unsigned int level);

	//ˢ�¾������������ times:ˢ�´���
    void refreshArenaChlgListNeedItem(unsigned int times, ItemInfo &item);
 
	//Ѫɫ����������ս�Ѷ� level: ��Ӣ�۵ȼ�; ����: SelfChlgDiffType
    unsigned int getSelfChlgDifficulty(unsigned int level);

	//Ѫɫ���üӳ� type: SelfChlgGainType; degree: SelfChlgGainDegree
	//expend:������������; gain:�ӳɱ���
    void getSelfChlgGain(unsigned int type, unsigned int degree, 
    	unsigned int &expend, unsigned int &gain);

	//�ػ�֮�齱�� chapter: �½�id
    void getProtectSpiritBonus(unsigned int chapter, unsigned int defend_days, std::vector<ItemInfo> &item_vec);

	//Ĥ���ػ�֮����� level: ��Ӣ�۵ȼ�
    void worshiProtectSpirit(unsigned int level, std::vector<ItemInfo> &item_vec);

	//��ȡ�ػ�֮����½� level: ��Ӣ�۵ȼ�
    unsigned int getProtectSpiritChapter(unsigned int level);
	
	//��ȡ̽������������ vip_level: VIP�ȼ�
    unsigned int getDiscoverLimitTimes(unsigned int vip_level);

	//���Ӣ�۰���ȴʱ�����Ľ����
    unsigned int clearTopHeroCDTimeNeedGold();

	// ��ȡ�سǣ�������Ҫ�Ľ��
	unsigned int getCityGuardBattleSkipCDNeedGold();
	// ��ȡ�س�ս�����֮����Ҫ�ȴ���CDʱ��(��)
	unsigned int getCityGuardBattleEndCDTime();

    //�سǣ���ɱboss����
    void getCityGuardKillBossReward(ItemInfo& item);
    //�سǣ��˺��������
    void getCityGuardHeroShanghaiOrderReward(
        ItemInfo& item_order_1
        , ItemInfo& item_order_2
        , ItemInfo& item_order_3
        , ItemInfo& item_order_4_10
        , ItemInfo& item_order_11_20);
    //�سǣ����˽���
    void getCityGuardLuckyReward(ItemInfo& item);
	
    //�سǣ����뽱
    void getCityGuardJoinReward(ItemInfo& item);

	//���������� color:װ����ɫ, level:װ���ȼ�
    unsigned int getJingLianMaxTimes(unsigned int color, unsigned int level);
	
	/* ϴ�� */
	//ϴ�������� type:װ������, color:װ����ɫ, level:װ���ȼ�
    unsigned int getPurifyMaxHoles(unsigned int type, unsigned int colour, unsigned int level);
	//ϴ���Ǽ� 	value:��ֵ
    PurifyLevel getPurifyLevel(unsigned int value);
	//ϴ�������Ǽ� 	color:װ����ɫ, level:װ���ȼ�, upper_limit:������ֵ
    PurifyLevel getPurifyUpperLimitLevel(unsigned int colour, unsigned int level, unsigned int &upper_limit);
}

