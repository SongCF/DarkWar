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
	unsigned int freezyNum;	// 冷却时间
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
	// 检测是是否成功了
	// 返回 >0 --> 则A战胜了B
	// 返回 <0 --> 则B战胜了A
	// 返回 =0 --> 则还没分出胜负
	int checkSuccess();

	// 更新每个角色的cd时间，并放在cd_time_arr里面
	void one_visible_step(vector<CdDiscr>& cd_time_arr,unsigned int action_item_Uid);

	void setName(string name);
	string getName() const;

	// 不会delete的
	// 棋盘的布局是
	//		0		1
	//    
	//			4
	//
	//		2		3
	// 0~3是团队，4是Boss
	void addItem(ChessBoardItem_TeamFight* item,int index);
	

	bool isItemExist(ChessBoardItem_TeamFight* item);

	bool isPosOK(int index);

	// 得到某个位置上的Item
	ChessBoardItem_TeamFight* getActorItemByIndex(int index);
	ChessBoardItem_TeamFight* getActorItemByUId(unsigned int uniqueId);
	//vector<ChessBoardItem_TeamFight*> getInBoardItems(){return currentItems;};


	// 根据Item在棋盘里面的uniqueId，得到他的AbstractItem数据
	virtual AbstractItem* getAbstractItemByUniqueId(unsigned int uniqueId);
	// 获取全部的活着的Item数据
	virtual vector<AbstractItem*> getAllAlivedItems();

	virtual vector<ChessBoardItem_TeamFight*> getAllAlivedTeamItems(ChessBoardItem_TeamFight* item);
	virtual vector<ChessBoardItem_TeamFight*> getAllAlivedEnemyItems(ChessBoardItem_TeamFight* item);

	float distance(ChessBoardItem_TeamFight* a,ChessBoardItem_TeamFight* b);
	// 是否是队友
	virtual bool isTeammate(unsigned int uniqueId1,unsigned int uniqueId2);

	virtual unsigned int getItemUniqueId(AbstractItem* item);

private:
	// 处在这个棋盘中的元素
	vector<ChessBoardItem_TeamFight*> currentItems;
};

