#pragma once

#include "protocol.h"
#include <vector>
#include "GameBuffItemGetter.h"
#include "FightScriptDefine.h"
#include "ChessboardItemBase.h"

using namespace std;


class ChessBoardItem_TeamFight : public ChessBoardItemBase
{
private:
	unsigned int freezyNum;	// ��ȴʱ��
public:
	ChessBoardItem_TeamFight(unsigned int uniqueId):ChessBoardItemBase(uniqueId)
	{
		this->hero = NULL;
		this->currentIndex = -1;
		this->freezyNum = 0;
	}
	~ChessBoardItem_TeamFight()
	{
	}
	int currentIndex;
};
//
class ChessBoard_TeamFight : public GameBuffItemGetter
{
private:
	string idName;
	void removeItem(ChessBoardItem_TeamFight* item);
	void removeItem(int index);
public:
	ChessBoard_TeamFight();
	// ������Ƿ�ɹ���
	// ���� >0 --> ��Aսʤ��B
	// ���� <0 --> ��Bսʤ��A
	// ���� =0 --> ��û�ֳ�ʤ��
	int checkSuccess();

	// ����ÿ����ɫ��cdʱ�䣬������cd_time_arr����
	void one_visible_step(vector<CdDiscr>& cd_time_arr,unsigned int action_item_Uid);

	void setName(string name);
	string getName() const;

	// ����delete��
	// ���̵Ĳ�����
	//		0		1
	//    
	//			4
	//
	//		2		3
	// 0~3���Ŷӣ�4��Boss
	void addItem(ChessBoardItem_TeamFight* item,int index);
	

	bool isItemExist(ChessBoardItem_TeamFight* item);

	bool isPosOK(int index);

	// �õ�ĳ��λ���ϵ�Item
	ChessBoardItem_TeamFight* getActorItemByIndex(int index);
	ChessBoardItem_TeamFight* getActorItemByUId(unsigned int uniqueId);
	//vector<ChessBoardItem_TeamFight*> getInBoardItems(){return currentItems;};


	// ����Item�����������uniqueId���õ�����AbstractItem����
	virtual AbstractItem* getAbstractItemByUniqueId(unsigned int uniqueId);
	// ��ȡȫ���Ļ��ŵ�Item����
	virtual vector<AbstractItem*> getAllAlivedItems();

	virtual vector<ChessBoardItem_TeamFight*> getAllAlivedTeamItems(ChessBoardItem_TeamFight* item);
	virtual vector<ChessBoardItem_TeamFight*> getAllAlivedEnemyItems(ChessBoardItem_TeamFight* item);

	float distance(ChessBoardItem_TeamFight* a,ChessBoardItem_TeamFight* b);
	// �Ƿ��Ƕ���
	virtual bool isTeammate(unsigned int uniqueId1,unsigned int uniqueId2);

	virtual unsigned int getItemUniqueId(AbstractItem* item);

private:
	// ������������е�Ԫ��
	vector<ChessBoardItem_TeamFight*> currentItems;
};

