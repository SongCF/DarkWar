#pragma once
#include <cstdlib>

class AbstractItem;
class ChessBoardItemBase
{
protected:
	// 一个战斗里面全局唯一的Id，角色的寻找和确定全靠它！
	unsigned int uniqueId;
public:
	ChessBoardItemBase(unsigned int uniqueId)
	{
		this->uniqueId = uniqueId;
		this->hero = NULL;
	}
	~ChessBoardItemBase()
	{
	}
	virtual unsigned int getUniqueId() const {return uniqueId;};
	AbstractItem* hero;
};

