#pragma once

#include <vector>
using namespace std;

class AbstractItem;
class GameBuffItemGetter
{
public:
	// ����Item�����������uniqueId���õ�����AbstractItem����
	virtual AbstractItem* getAbstractItemByUniqueId(unsigned int uniqueId) = 0;
	// ��ȡȫ���Ļ��ŵ�Item����
	virtual vector<AbstractItem*> getAllAlivedItems() = 0;

	virtual unsigned int getItemUniqueId(AbstractItem* item) = 0;
	// �Ƿ��Ƕ���
	virtual bool isTeammate(unsigned int uniqueId1,unsigned int uniqueId2) = 0;
};