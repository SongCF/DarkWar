#pragma once
#include <cstdlib>

class AbstractItem;
class ChessBoardItemBase
{
protected:
	// һ��ս������ȫ��Ψһ��Id����ɫ��Ѱ�Һ�ȷ��ȫ������
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

