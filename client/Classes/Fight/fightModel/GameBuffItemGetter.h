#pragma once

#include <vector>
using namespace std;

class AbstractItem;
class GameBuffItemGetter
{
public:
	// 根据Item在棋盘里面的uniqueId，得到他的AbstractItem数据
	virtual AbstractItem* getAbstractItemByUniqueId(unsigned int uniqueId) = 0;
	// 获取全部的活着的Item数据
	virtual vector<AbstractItem*> getAllAlivedItems() = 0;

	virtual unsigned int getItemUniqueId(AbstractItem* item) = 0;
	// 是否是队友
	virtual bool isTeammate(unsigned int uniqueId1,unsigned int uniqueId2) = 0;
};