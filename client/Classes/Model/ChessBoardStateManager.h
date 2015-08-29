#pragma once
#include <vector>
//#include "protocol.h"
#include "flat.h"

using namespace std;

// 一个task的棋盘
// 里面的最后一个都是boss关
struct TaskChessBoard
{
	int taskId;
	int numInRow; //行
	int numInCol; //列

	//格子状态、 下标0固定为boss  例如：16格子，下标0为boss  下标1-16为小怪    
	//只有一个格子，那么numInRow=0，numInCol=0
	vector<TaskGridInfo> gridState;

//	int box_id;			// 宝箱所在的格子，2到8之间，第一个个boss关不会有，0就没有
	//TaskStatus state; // 这里只存在一个情况就是 ,目前不管
//	vector<bool> gridstate;	// 格子的状态

	TaskChessBoard()
	{
		taskId = 0;
		numInRow = 0;
		numInCol = 0;
//		box_id = 0;
		//state = TaskStatus_Got;	// 能拿到棋盘了，那么肯定是got的
	}
};
/**
 *	棋盘的状态数据的管理.
 */
class ChessBoardStateManager
{
	friend class CmdHelper;
private:
	ChessBoardStateManager(void);

	// 所有缓存的任务
	vector<TaskChessBoard> taskChessboards;

	// 击杀怪物的奖励
	bool isKillMonstorBonusOk;
	Flat_QueryKilledMonsterBonusRsp killMonstorBonus;

	// 击杀的怪物数量
	unsigned int killedPuTongMonsterNum;
	unsigned int killedJingYinMonsterNum;

	bool isTaskRateOk;
	Flat_QueryRateTaskRsp taskRate;
	// 每一小节的奖励
	map<unsigned int,Flat_QueryRateTaskBonusRsp> sectionRate;
	// 每一章的奖励
	map<unsigned int,Flat_QueryRateTaskBonusRsp> seasonRate;
 
	static ChessBoardStateManager* manager;

	void initOneChessboard(TaskChessBoard board);

	//void setTaskState(int tashId,TaskStatus state);

	// 保存数据
	void setTaskRateBonusList(unsigned int seasonOrSectionId,Flat_QueryRateTaskBonusRsp data);

	void setCurrentTaskRate(Flat_QueryRateTaskRsp rate);

	//
	void setKilledMonsterBonus(Flat_QueryKilledMonsterBonusRsp bonus);

	//
	void setKilledMonsterNum(unsigned int putongNum,unsigned int jingyinNum);

	void subKilledPutongMonster(unsigned int toSub);
	void subKilledJingyinMonster(unsigned int toSub);

	void addKilledPutongMonster(unsigned int toAdd);
	void addKilledJingyinMonster(unsigned int toAdd);

public:
	static ChessBoardStateManager* getManager();

	// 获得棋盘数据.如果本地没有就返回false
	bool getChessBoard(int taskId,TaskChessBoard& out_data);

	// 是否是boss关卡://gridId:0~8
	bool isBossSection(int taskId,int gridId);
	// 得到某个task的某个格子的最多可以打斗的场数
	int getSectionTotalFightNum(int taskId,int gridId);

	// 获取章节的奖励数据,1-4章.没找到就返回NULL
	Flat_QueryRateTaskBonusRsp* getSeasonRateBonus(int seasonId);
	// 获取任务的奖励.没找到就返回NULL
	Flat_QueryRateTaskBonusRsp* getTaskRateBonus(int taskId);

	// 获取当前的星级
	Flat_QueryRateTaskRsp* getCurrentRate();

	Flat_QueryKilledMonsterBonusRsp getKilledMonsterBonusData();

	unsigned int getKilledPutongMonsterNum();
	unsigned int getKilledJingYinMonsterNum();

	~ChessBoardStateManager(void);
};

