#pragma once
#include <vector>
using namespace std;

class ChessBoardItemBase;
/**
 *	给SkillHelperBase提供数据适配的接口
 */
class SkillHelperBaseAdapter
{
public:
	// 准备受功的角色的
	// fightorItem 指定攻击方的Item
	// 返回被攻击的角色
	// 没有就返回NULL
	// dis>=0就是用dis作为指定的距离，如果dis小于0，那么就使用fightorItem默认的
	virtual ChessBoardItemBase* getDefaultTargetBeFight(ChessBoardItemBase*fightor,float distance = -1) = 0;

	// 生成一个buff效果的Id，这个Id是不能重复的
	// mFightControler->mBuffProc->genOneBuffId()
	virtual int genOneBuffEffectId() = 0;

	// 获取item的全部活着的队友
	// 就根据原始棋盘来判断就是了。不考虑基因棋盘
	virtual vector<ChessBoardItemBase*> getAlivedTeamItems(ChessBoardItemBase* item) = 0;
	// 获取item的全部活着的敌人
	// 就根据原始棋盘来判断就是了。不考虑基因棋盘
	virtual vector<ChessBoardItemBase*> getAlivedEnemyItems(ChessBoardItemBase* item) = 0;

	// 获得itemSet最弱的Item，防御最弱，快要死的
	// 直接转掉FightController即可
	virtual ChessBoardItemBase* getWeaklessAndDyingItem(vector<ChessBoardItemBase*> itemSet,ChessBoardItemBase* fightor) = 0;
	// 获取前排敌人
	virtual vector<ChessBoardItemBase*> getFrontRowAlivedEnemyItem(ChessBoardItemBase* fightor) = 0;
	// 获取后排的敌人
	virtual vector<ChessBoardItemBase*> getBackRowAlivedEnemyItem(ChessBoardItemBase* fightor) = 0;

	// 求item1和item2的距离，支持跨棋盘计算
	virtual float distance(ChessBoardItemBase* item1,ChessBoardItemBase* item2) = 0;

	// 测试test是否是在item的正前方
	// 对于棋盘来说就是是否在一列上面，即2个棋盘的X轴上
	virtual bool isInCol(ChessBoardItemBase* item,ChessBoardItemBase* test) = 0;

	// 测试test是否是在item的正左边或右边
	// 对于棋盘来说就是是否在一个棋盘的一个Y轴上
	virtual bool isInRow(ChessBoardItemBase* item,ChessBoardItemBase* test) = 0;
};

