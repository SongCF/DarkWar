#pragma once

#include "BaseActor.h"
#include "EquipmentManager.h"
#include <map>

using namespace std;

enum ShangZhenFilter_Actor
{
	Actor_All = 0,			// 全部
	Actor_ShangZheng,		// 上阵的
	Actor_WeiShangZheng,	// 未上阵的
};

enum LockFilter_Actor
{
	LF_All = 0,		// 全部
	LF_UnLock,		// 解锁的，没有锁住的
	LF_Locked,		// 未解锁的，锁住的
};

struct JiaoTang
{
	unsigned int level;	// 教堂的级别
};

struct RuntimeData
{
	unsigned int buyTiLiTimeLeft;			//体力购买次数
	unsigned int saoDangTimeLeft;			//副本扫荡次数
	unsigned int buyArenaChallengeTimeLeft;	//可购买竞技场挑战次数
	unsigned int CanJumpGridNum;			//关卡可跳过小怪数
	unsigned int cast_silver_num;			//铸银次数

	//add by scf 装备剩余的免费洗点
	unsigned int purify_free_times;

	// 下面的先不管
	bool enableFastRongYuDuiHuan;			// 开启荣誉快速兑换功能
	unsigned int freeRefreshTimeLeft;			// 免费刷新悬赏任务次数
	RuntimeData()
	{
		buyTiLiTimeLeft = 0;
		saoDangTimeLeft = 0;
		buyArenaChallengeTimeLeft = 0;
		CanJumpGridNum = 0;
		cast_silver_num = 0;

		//add by scf 装备剩余的免费洗点
		purify_free_times = 0;

		// 下面的先不管
		enableFastRongYuDuiHuan = false;
		freeRefreshTimeLeft = 0;
	}
};

// 自己的管理器
// 管理自己的一些数据
// 比如自己所带的弟子，自己的BaseActor
// 将一切的操作就限制在了我的队伍里面
class MyselfManager
{
private:
	static MyselfManager* manager;

	map<unsigned int,BaseActor*> allTeamActors; // 也包括元神

	JiaoTang jiaoTang;

	unsigned int mMyId;	// 主角Id，也就是teamId

	bool isTeamDataInArenaOK;
	ArenaTeamBasicInfo teamDataInArena; // 竞技场中的数据

	bool isRuntimeDataOk;
	RuntimeData runtimeData;

	bool isDftRuntimeDataOk;
	RuntimeData mDftRuntimeData;

	// vip充值信息
	unsigned int next_vip_level;	//下一VIP等级 
	unsigned int next_need_charge;	//到达下一个等级需要的充值金额 (已是最高级为0)
	bool b_first_charged; 			//首次充值与否 false:未充值过 true:已充值过
	vector<ItemInfo> first_present_items; // 首次回送的Item
	vector<ChargePacketInfo> chargePakets; // 各种充值包


	void freeAllActor();
	// 外界请用getMyTeamActor
	BaseActor* getActorById(unsigned int _id);

	MyselfManager();

public:
	static MyselfManager* getManager();
	// 重新delete，再new 一个，只有换角色登陆才需要这样！
	static void reCreate();

	struct ActorMapping
	{
		unsigned int actorId;
		unsigned int yuanShenId;
	};
	// 初始化，应该只能初始化一次，除非换角色了。
	bool initManager(vector<BaseActorProperty> dataConfig,vector<ActorMapping> mappings);

	// 设置教堂的数据
	void setCurrentJiaoTangData(JiaoTang jiaotang);

	bool isRunTimeOK(){return isRuntimeDataOk;}
	void setRuntionData(RuntimeData data);
	RuntimeData* getRuntionData();

	void setDefaultRuntimeData(RuntimeData data);
	RuntimeData* getDefaultRuntimeData();

	unsigned int getCurrentVipLevel();
	//获取下一个Vip等级，如果已经是最高的，就是当前的值
	unsigned int getNextVipLevel();
	//到达下一个等级需要的充值金额 (已是最高级为0)
	unsigned int getNeedChargeToGetNextVip();
	//获取首充的礼品
	vector<ItemInfo> getFirstChargePresent();
	//获取充值包列表数据
	vector<ChargePacketInfo> getChargePacketInfo();
	//是否充值过
	bool haveChargedBefore();

	void setNextVipLevel(unsigned int level);
	void setNeedChargeToGetNextVip(unsigned int needCharge);
	void setFirstChargePresent(vector<ItemInfo> presents);
	void setChargePacketInfo(vector<ChargePacketInfo> parkets);
	void setCharged(bool charged);

	// 获取教堂的等级
	unsigned int getJiaoTangLevel();
	// 获取教堂的数据
	const JiaoTang& getJiaoTangData();
	// 是否可以洗礼actor
	bool canXiLiActor(const BaseActor* actor);

	// 添加一位
	// 如果是元神，那么zhenshenIdIfIsYuanshen就是非0
	void addOneActor(BaseActorProperty config,unsigned int zhenshenIdIfIsYuanshen = 0);

#define ACTOR_TYPE_ZHEN_SHEN	0
#define ACTOR_TYPE_YUAN_SHEN	1
#define ACTOR_TYPE_ALL			3
	// expectZhuJue: true就不会包括主角，false就包括主角
	vector<BaseActor*> getAllTeamActors(ShangZhenFilter_Actor filter,LockFilter_Actor filterLock,int actorType,bool expectZhuJue);

	// 主角的助手，不包括主角  : 真身
	vector<BaseActor*> getMyZhuShous_ZhenShen(ShangZhenFilter_Actor filter = Actor_All,LockFilter_Actor filterLock = LF_All);
	// 所有的人 : 真身
	vector<BaseActor*> getTeamActors_ZhenShen(ShangZhenFilter_Actor filter = Actor_All,LockFilter_Actor filterLock = LF_All);

	// 主角的助手，不包括主角  : 元神
	vector<BaseActor*> getMyZhuShous_YuanShen(ShangZhenFilter_Actor filter = Actor_All);
	// 所有的人 : 元神
	vector<BaseActor*> getTeamActors_YuanShen(ShangZhenFilter_Actor filter = Actor_All);

	// 解锁了n个就返回N
	int getUnlockedHeroNumByProfession(int profession);


	bool getArenaTeamBasicInfo(ArenaTeamBasicInfo& outData); // 竞技场中的数据
	ArenaTeamBasicInfo* getEdiableArenaTeamBasicInfo();
	void initArenaTeamBasicInfo(ArenaTeamBasicInfo data);

	// 获得我的数据
	BaseActor* getMyZhuJueData();
	// 得到我的队伍的人，主角+助手
	BaseActor* getMyTeamActor(unsigned int actorId);
	// 0 代表没有上阵,真身
	BaseActor* getMyTeamZhenShenActorByPosIndex(unsigned int index);

	// 需要等待服务器的数据,不要擅自使用，要服务器正确了再调用，避免出错
	// 将队伍里面的角色中移除这个装备
	void removeEquipementFromOwner(Equipment* quip);

	// 需要等待服务器的数据
	// 给forActorId添加一个装备
	// quip必须是没有被别人占用
	// 如果该角色已经有了这类装装备，那么就替换（移除旧的，添加新的）
	// 如果该角色可以装备多个这类装备，那么就直接装备上即可
	void attachEquipment(Equipment* quip,int forActorId);

	// 筛选
	vector<Equipment*> equipmentFilter(vector<Equipment*> data,Filter_Equip filter,FilterEquipType filterType);

	// 得到团队所有的装备,主角+助手+剩余的
	vector<Equipment*> getAllEquipments(Filter_Equip filter,FilterEquipType filterType = Filter_Type_All);
	// 得到团队所有 未装备 的装备
	vector<Equipment*> getAllEquipmentsWithOutEquipedOrChuanQi();
	// 获得某个角色上的装备
	vector<Equipment*> getAllEquipmentsOnActor(unsigned int actorId, Filter_Equip filter,FilterEquipType filterType = Filter_Type_All);

	~MyselfManager(void);
};

