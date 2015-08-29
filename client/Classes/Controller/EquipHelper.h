#pragma once

#include "flat.h"
#include <map>
using std::pair;

class EquipHelper
{
private:
	static EquipHelper* _equipHelper;
	EquipHelper();
public:
	static EquipHelper* getHelper();

	//获取一件当前没有， 且在前5关副本内能刷到的黄装
	//return : unsigned int --> task id
	pair<unsigned int, Flat_FubenDiaoluoZBInfo> getOneBetterEquip(unsigned int myHeroId, unsigned int taskId);
};