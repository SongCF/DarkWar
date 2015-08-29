#pragma once
#include "SkillHelperBaseAdapter.h"

class ChessBoardHelper;
class FightControler;
class ChessBoard_TeamFight;
class TeamFightControler;
/**
 * 棋盘战斗的适配器实现类
 */
class SkillHelperAdapterImpl : public SkillHelperBaseAdapter
{
private:
	FightControler* mFightControler;
	ChessBoardHelper* mChessHelper;
public:
	SkillHelperAdapterImpl(ChessBoardHelper* chessHelper,FightControler* controler);

	virtual ChessBoardItemBase* getDefaultTargetBeFight(ChessBoardItemBase*fightor,float distance = -1);
	virtual int genOneBuffEffectId();
	virtual vector<ChessBoardItemBase*> getAlivedTeamItems(ChessBoardItemBase* item);
	virtual vector<ChessBoardItemBase*> getAlivedEnemyItems(ChessBoardItemBase* item);
	virtual ChessBoardItemBase* getWeaklessAndDyingItem(vector<ChessBoardItemBase*> itemSet,ChessBoardItemBase* fightor);
	virtual vector<ChessBoardItemBase*> getFrontRowAlivedEnemyItem(ChessBoardItemBase* fightor);
	virtual vector<ChessBoardItemBase*> getBackRowAlivedEnemyItem(ChessBoardItemBase* fightor);
	virtual float distance(ChessBoardItemBase* item1,ChessBoardItemBase* item2);
	virtual bool isInCol(ChessBoardItemBase* item,ChessBoardItemBase* test);
	virtual bool isInRow(ChessBoardItemBase* item,ChessBoardItemBase* test);
};


/**
 *	团队战斗的棋盘适配实现类
 */
class SkillHelperTFAdapterImpl : public SkillHelperBaseAdapter
{
private:
	ChessBoard_TeamFight* mChessHelper;
	TeamFightControler* mFightControler;
public:
	SkillHelperTFAdapterImpl(ChessBoard_TeamFight* chessHelper,TeamFightControler* controler);

	virtual ChessBoardItemBase* getDefaultTargetBeFight(ChessBoardItemBase*fightor,float distance = -1);
	virtual int genOneBuffEffectId();
	virtual vector<ChessBoardItemBase*> getAlivedTeamItems(ChessBoardItemBase* item);
	virtual vector<ChessBoardItemBase*> getAlivedEnemyItems(ChessBoardItemBase* item);
	virtual ChessBoardItemBase* getWeaklessAndDyingItem(vector<ChessBoardItemBase*> itemSet,ChessBoardItemBase* fightor);
	virtual vector<ChessBoardItemBase*> getFrontRowAlivedEnemyItem(ChessBoardItemBase* fightor);
	virtual vector<ChessBoardItemBase*> getBackRowAlivedEnemyItem(ChessBoardItemBase* fightor);
	virtual float distance(ChessBoardItemBase* item1,ChessBoardItemBase* item2);
	virtual bool isInCol(ChessBoardItemBase* item,ChessBoardItemBase* test);
	virtual bool isInRow(ChessBoardItemBase* item,ChessBoardItemBase* test);
};

