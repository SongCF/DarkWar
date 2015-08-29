#pragma once

#include <vector>
#include "include_json.h"
#include "Equipment.h"

using namespace std;


// 筛选器
enum Filter_Equip
{
	Filter_All = 0,		// 所有的
	Filter_Equipped,	// 已装备的
	Filter_No_Equipped,	// 没有装备的
};

enum FilterEquipType
{
	Filter_Type_All = 0,		// 所有的
	Filter_Type_WuQi,
	Filter_Type_FangJu,
	Filter_Type_ShiPin,
};

// 装备的管理器，管理整个team里面的所有装备
// 获取SPCmd_GetZhuangbeiInTeam命令即可！
// 管理武器，防具.
class EquipmentManager
{
private:

	static EquipmentManager* manger;
	//static unsigned int initedTime;

	vector<Equipment*> equipmentVec;

	EquipmentManager(void);

	// 获得装在某个角色上面的装备
	// 外界需要知道的话就直接找那个角色，去获得
	// 这里不提供全局查找，在返回
	// 这个方法主要用于游戏初始化的
	vector<Equipment*> getEquipOnActor(unsigned int actorId);

	// 从items中扎到第一个 要求 已经装备在place的item
	Equipment* getEquipmentWhichEquipedAtPos(EquipRequirePlace place,vector<Equipment*> items);
public:
	static EquipmentManager* getManager();
	//static unsigned int getInitedTime(){return initedTime;};

	// 必须在MyselfManager 初始化之后才能初始化
	bool initManager(vector<EquipData> configDatas);
	// 刷新数据，必须在MyselfManager 初始化之后才能初始化
	//void refreshData(vector<EquipData> configDatas);

	// 获得装备需要装备的位置
	// 武器都放在主武器那里，双手的也是
	vector<EquipRequirePlace> getRequirdPlace(ZhuangbeiType type);
	//EquipRequirePlace getRequirdPlace(ZhuangbeiType type);
	bool canEquipToPlace(ZhuangbeiType type,EquipRequirePlace place);
	EquipType getEquipType(ZhuangbeiType type);

	// 某个类型的装备是否可以装在某个人身上
	bool canUseEquip(ZhuangbeiType zhuangbei_type,Profession profession_id);

	//获取所有魔法属性的格式化字符串
	//比如"+30 敏捷"
	// 如果里面有多个同类型的，会自动归并,默认不归并
	std::vector<std::string> getZBShuXingString(const vector<ZBAttr>& attr_vec,bool group_common_attr_type = false);

	// 从已装备的装备中，寻找需要被替换下来的装备
	vector<Equipment*> getNeedTakeOffIfWareOnEqui(ZhuangbeiType type,EquipRequirePlace needPlace,vector<Equipment*> currentEquiped);

	//// 获得需要的手的个数，只针对防具，武器，其他的直接返回0
	//int getNeedHandNum(ZhuangbeiType type);

	// 从pool里面获取Equip
	Equipment* getOneEquipment(unsigned long long eId);

	vector<Equipment*> getEquipmentsByGroupId(unsigned int group_id);


	// 获得团队下面的所有的装备
	vector<Equipment*> getAllEquipment(){return equipmentVec;};
	// 筛选
	vector<Equipment*> equipmentFilter(vector<Equipment*> data,Filter_Equip filter,FilterEquipType filterType);
	// 得到团队所有的装备,主角+助手+剩余的
	vector<Equipment*> getAllEquipment(Filter_Equip filter,FilterEquipType filterType = Filter_Type_All);
	// 获得某个角色上的装备
	vector<Equipment*> getAllEquipmentsOnActor(unsigned int actorId, Filter_Equip filter,FilterEquipType filterType = Filter_Type_All);

	// 看空间是否够保存num件装备
	bool isSpareEnoughToSaveEquip(unsigned int num);


	// 是否有某个suit的部分
	bool haveSuitPart(unsigned int group_id);

	// 销毁一个装备
	void destoryOneEquip(unsigned long long zhuangBeiId);

	void removeAll();

	// 添加一个装备
	// 如果装备是装备到人生上的，那么那个人的数据需要先加到MySelfManager
	// 因为这里要负责把装备attach到BaseActor上面
	void addOneEquipment(EquipData data);

	bool isEquipmentExist(unsigned long long zhuangbei_id);

	~EquipmentManager(void);
};

