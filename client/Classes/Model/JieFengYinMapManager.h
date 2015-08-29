#pragma once

#include <map>
using std::map;
using std::pair;

struct Flat_DuanzaoGroupZBInfo;

//装备 解封印 需要的材料装备映射表、 查询过的会保存、 没查询过的需要查询服务器  cmdQueryAssessWeapon 
class JieFengYinMapManager
{
private:
	JieFengYinMapManager(){}
	map<unsigned int, Flat_DuanzaoGroupZBInfo> mJieFenMap;
	static JieFengYinMapManager* _jieFengYin_Instance;

public:
	static JieFengYinMapManager *getManager();

	//获取解封印需要的 材料装备groupId   ==0 时就是未找到
	const Flat_DuanzaoGroupZBInfo* getNeedEqMaterialInfo(unsigned int jieFengYinEqGroupId);

	void addOneJieFengMap(unsigned int equip_groupId, Flat_DuanzaoGroupZBInfo& info);
};