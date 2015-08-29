#pragma once

#include "BaseSprite.h"
#include <vector>
#include "FightScriptDefine.h"
#include "ClientChessBoardItem.h"
#include "BaseLayer.h"
#include "IChessboardProperty.h"

using namespace std;

#define ChessBoard_X_Num 2
#define ChessBoard_Y_Num 2
#define BOARD_NAME_BUTTOM	"boardButom"
#define BOARD_NAME_TOP		"boardTop"
#define	ITEM_Z_OERDER_IN_STAGE		3

// 客户端的棋盘
class ClientChessBoard : public BaseSprite,public IChessboardProperty
{
private:
	string name;

	short currentBloodNum;		// 该棋盘当前可用的血瓶数量
	short defaultBloodNum;		// 默认的个数

	CCArray* itemArray;

	//// item的container不会移除，于是到时item不能移除
	//// 于是就都不能移除
	//// 所以这里帮他移除
	//CCArray* autoRomovePoolForContainer;

	CCNode* mStage;
	// 是棋盘的局部坐标
	vector<CCPoint> posVec;

	ClientChessBoard(void);

	// 初始化棋盘格子划分数据
	bool initWithJsonFile(string jsonFile);

public:
	// stage 指定棋盘的Item最终会放到的地方
	static ClientChessBoard* create(string configFile,CCNode* stage);
	//
	void setBoardName(string name){this->name = name;};
	string getBoardName(){return name;};

	void setCanUsedBloodNum(short bloodNum){currentBloodNum = bloodNum;};
	short getCurrentBloodNum() const {return currentBloodNum;};
	void setDefaultBloodNum(short bloodNum){defaultBloodNum = bloodNum;};
	short getDefaultBloodNum() const {return defaultBloodNum;};

	//
	CCArray* getInBoardItems(){return itemArray;};
	//
	int getChessBoard_X_Num(){return ChessBoard_X_Num;};
	int getChessBoard_Y_Num(){return ChessBoard_Y_Num;};
	// 得到grid的位置,是相对stage下的坐标
	CCPoint getGridPos(int xIndex,int yIndex);
	CCPoint getGridPosInWorld(int xIndex,int yIndex);
	// 判断位置是否正确
	bool isPosCorrect(int xIndex,int yIndex);

	//IChessboardProperty 需求
	virtual CCPoint getCenterWorldPoint();

public:
	// 判断是否角色存在
	bool isActorExist(ClientChessBoardItem* item);

	/**
	 *	获得xIndex和yIndex上的那个角色Id
	 *	没有就是NULL
	 */
	ClientChessBoardItem* getActorItem(int xIndex,int yIndex);

	// 通过Id查找
	ClientChessBoardItem* getActorItemByUniqueId(unsigned int uniqueId);

	// 添加一个角色到某个位置
	bool addActor(ClientChessBoardItem* actor,int xIndex,int yIndex);

	// 把角色移动到某个位置
	void moveActor(ClientChessBoardItem* actor,int toX,int toY);

	// 跨棋盘移动
	void crossMove(ClientChessBoard* desBoard,ClientChessBoardItem* actor,int toX,int toY);

	// 移除一个角色,角色必须存在
	void removeActor(ClientChessBoardItem* actor);
	// 角色可以不存在
	void removeActor(int x_index,int y_index);
	// 移除某一排的角色
	void removeActor(int y_index);

	// 设置在棋盘内的角色的可视化，只要在这个棋盘内都会修改
	void setAllActorVisible(bool visible);

	~ClientChessBoard(void);
};


// 团队打boss的棋盘
// uId为5 那么就是boss，1到4是团队
class ClientChessBoard_TeamFight : public BaseLayer,public IChessboardProperty
{
private:
	CCArray* items;
public:
	ClientChessBoard_TeamFight();
	~ClientChessBoard_TeamFight();
	// 外界new的就需要外界release
	void addOneItem(ClientChessBoardItemBase* item);
	ClientChessBoardItemBase* getOneByUId(unsigned int u_id);
	vector<ClientChessBoardItemBase*> getAllItems();
	CCArray* getAllItems_CCArray(){return items;};

	//IChessboardProperty 需求
	virtual CCPoint getCenterWorldPoint();
};

