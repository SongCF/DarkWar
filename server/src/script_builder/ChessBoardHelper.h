#pragma once

#include "protocol.h"
#include <vector>
#include "ScriptDataProvider.h"
#include "GameBuffItemGetter.h"
#include "FightScriptDefine.h"
#include "ChessboardItemBase.h"
using namespace std;

// 棋盘的格子数
#define GRID_NUM_X		2
#define GRID_NUM_Y		2

class ChessBoard;
class ChessBoardItem;


class ChessBoardItem : public ChessBoardItemBase
{
private:
	const ChessBoard* geneBoard;	// 唯一标示出生份的，基因一样，不能修改

	ChessBoard* originBoard;	// 初始棋盘,用于判断是否是队友的唯一标记
	int originIndex;

public:
	ChessBoardItem(ChessBoard* originBoard,int originIndex,unsigned int uniqueId):ChessBoardItemBase(uniqueId)
	{
		this->geneBoard = originBoard;
		this->originBoard = originBoard;
		this->originIndex = originIndex;
		this->currentBoard = NULL;
		this->hero = NULL;
		this->posx = -1;
		this->posy = -1;
	}
	~ChessBoardItem()
	{
		if (hero != NULL)
		{
			delete hero;
		}
	}
	int getOriginIndex(){return originIndex;};
	// 唯一的途径判断是否两个人是队友就看他们的原始棋盘
	const ChessBoard* getOriginBoard()const {return originBoard;};
	// 修改出生棋盘，主要用于混淆视听，也就是奸细的实现
	void changeOriginBoard(ChessBoard* board){originBoard = board;};

	const ChessBoard* getGeneBoard(){return geneBoard;};

	void setOriginBoardCanUsedBlooldNum(short num);

	// 看test跟我是否是队友
	bool isTeammate(ChessBoardItem* test){return test->getOriginBoard() == originBoard;};

	// 棋盘中的posx,posy
	int posx;
	int posy;	
	ChessBoard* currentBoard;	// 当前棋盘
};
class ChessBoard
{
private:
	//ChessBoardCallback* callback;
	string idName;

	short currentBloodNum;		// 该棋盘当前可用的血瓶数量
	short defaultBloodNum;		// 默认的个数
	int bloodAddPercent;		// 血瓶的加血百分比
public:
	void setName(string name);
	string getName() const;
	ChessBoard();
	void setCanUsedBloodNum(short bloodNum){currentBloodNum = bloodNum;};
	short getCurrentBloodNum() const {return currentBloodNum;};
	void setDefaultBloodNum(short bloodNum){defaultBloodNum = bloodNum;};
	short getDefaultBloodNum() const {return defaultBloodNum;};
	void setBloodAddPercent(int per){bloodAddPercent = per;};
	int getBloodAddPercent() const{return bloodAddPercent;};
	// 不会delete的
	void addItem(ChessBoardItem* item,int pos_x,int pos_y);
	void removeItem(ChessBoardItem* item);
	void removeItem(int x_index,int y_index);
	bool isItemExist(ChessBoardItem* item);
	bool isPosOK(int x_index,int y_index);
	// 获得棋盘一个横向的的Item的数量
	int getItemNumInY(int y_index,bool justAlive);
	// 得到某个位置上的Item
	ChessBoardItem* getActorItem(int x_index,int y_index,bool includeWideItem = true);
	ChessBoardItem* getActorItem(unsigned int uniqueId);
	int getMingJie();
	// 得到xIndex上的所有元素，他们在一个竖线上
	// 默认会把宽的Item也包含进来
	vector<ChessBoardItem*> getYItems(int xIndex,bool includeWideItem = true);
	// 得到xIndex上的所有元素，他们在一个竖线上
	vector<ChessBoardItem*> getXItems(int yIndex);
	vector<ChessBoardItem*> getInBoardItems();
private:
	// 处在这个棋盘中的元素
	vector<ChessBoardItem*> currentItems;
};


// 管理棋盘的
// 这里主要用于在生成战斗过程时，起到帮助的作用
class ChessBoardHelper : public GameBuffItemGetter
{
public:
	ChessBoardHelper();
	~ChessBoardHelper();
	void initHelper(ScriptDataProvider* dataProvider,
		unsigned int teamA_hero_ids_4[],		// teamA的角色的Id，对应数据库全局唯一
		unsigned int uniqueId_4_A[],			// teamA的角色的唯一id，是外界指定的，对于一场战斗，全局唯一
		unsigned int teamB_hero_ids_4[],
		unsigned int uniqueId_4_B[],
		ScriptBuilderUserData userData);
	// 检测是是否成功了
	// 返回 >0 --> 则A战胜了B
	// 返回 <0 --> 则B战胜了A
	// 返回 =0 --> 则还没分出胜负
	int checkSuccess();

	// 作战过程中调用的
	// 可见的步奏都会调
	void one_visible_step(vector<CdDiscr>& cd_time_arr,unsigned int action_item_uId);

	// 获取队伍队的下一个Item,是循环取的-->最后一个的下一个就是第0个
	// 如果currentItem = NULL，就返回队伍index升序的第一个
	// 队伍空了就返回NULL
	ChessBoardItem* getTeamNextItem(ChessBoard* originBoard,ChessBoardItem* currentItem);

	// 在BordA和BoardB中寻找原始棋盘是orginBoard的元素
	int getAlivedItemNumByOriginBoard(ChessBoard* orginBoard);

	// 获得原始棋盘是orginBoard的所有活着的Item,orginBoard=NULL,那么每个棋盘都会算
	vector<ChessBoardItem*> getAlivedItemsByOriginBoard(const ChessBoard* orginBoard);
	// 获得item的竖线方向上的所有Item，包括2个棋盘的
	// 有敌人，有队友
	vector<ChessBoardItem*> getYItems(int xIndex);
	// 获得横向上的Items
	vector<ChessBoardItem*> getXItems(ChessBoard* board, int yIndex);
	// 获得横向上的Items,活着的
	vector<ChessBoardItem*> getAlivedXItems(ChessBoard* board, int yIndex);

	// 求item1和item2的距离，支持跨棋盘计算
	float distance(ChessBoardItem* item1,ChessBoardItem* item2);

	// 获取item的Y方距离为dis的坐标
	// 寻找item的前方距离为dis的一个Item，所谓前方是相对自己原始棋盘的Y轴递减的方向
	// 只求出Y的值，所以可以处理宽Item
	void getNextYPosOfItem(const ChessBoardItem* item,ChessBoard*& out_pBoard,int& out_posy,int dis = 1 );

	//// 获得item的NextY的Item，dis = 1
	//// 不能处理宽Item的前面,但是前面如果是Wide的Item，那么算不算的话就需要用参数指定
	//ChessBoardItem* getOneNextFrontOfItem(const ChessBoardItem* item,bool includeWideItem = true);
	// 从2个棋盘里面查找，找到uniqueId的Item，有就返回，没有就返回NULL
	ChessBoardItem* getOneItemByUniqueId(unsigned int uniqueId);


	// 根据Item在棋盘里面的uniqueId，得到他的AbstractItem数据
	virtual AbstractItem* getAbstractItemByUniqueId(unsigned int uniqueId);
	// 获取全部的活着的Item数据
	virtual vector<AbstractItem*> getAllAlivedItems();
	// 是否是队友
	virtual bool isTeammate(unsigned int uniqueId1,unsigned int uniqueId2);
	// 
	virtual unsigned int getItemUniqueId(AbstractItem* item);

	// 把x,y的grid坐标转化成线性坐标
	int getIndex(int x_index,int y_index);
	void getGridPos(int index,int& out_x_index,int& out_y_index);
	ChessBoard* getBoardA(){return boardA;};
	ChessBoard* getBoardB(){return boardB;};
	// 把Item的当前posy,转换成指定的board的posy
	int convertToBoardGridY(const ChessBoard* board,const ChessBoardItem* item);
	int convertToOriginBoardGridY(const ChessBoardItem* item);
private:
	ChessBoard* boardA;
	ChessBoard* boardB;
	ScriptDataProvider* mDataProvider;
	// 存放所有new出来的Item，到时delete
	vector<ChessBoardItem*> itemPool;

	void removeItemFromVec(vector<ChessBoardItem*>& items,ChessBoardItem* toRemove);
	//
	void sortChessBoardItemsByIndexInc(vector<ChessBoardItem*> &toSort);
	void initChessBoard(ChessBoard* board,
		unsigned int team_hero_ids_4[],		// 对应数据库，全局唯一
		unsigned int team_unique_ids_4[],	// 对应一场战斗，全局唯一
		ScriptBuilderUserData userData);
};

