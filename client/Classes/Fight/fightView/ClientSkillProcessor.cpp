#include "ClientSkillProcessor.h"
#include "ClientChessBoard.h"

ClientSkillProcessor::ClientSkillProcessor(ClientChessBoard* boardA,ClientChessBoard* boardB)
{
	this->boardA = boardA;
	this->boardB = boardB;
}

ClientChessBoardItemBase* ClientSkillProcessor::getItemFromBoard(unsigned int _heroId)
{
	ClientChessBoardItem* itemA = boardA->getActorItemByUniqueId(_heroId);
	ClientChessBoardItem* itemB = boardB->getActorItemByUniqueId(_heroId);

	CCAssert(itemA == NULL || itemB == NULL,"不能同时能找到");

	return itemA == NULL?itemB:itemA;
}

ClientSkillProcessor::~ClientSkillProcessor(void)
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 
ClientSkillProcessor_TF::ClientSkillProcessor_TF(ClientChessBoard_TeamFight* board)
{
	this->board = board;
}

// 棋盘的访问方法
ClientChessBoardItemBase* ClientSkillProcessor_TF::getItemFromBoard(unsigned int _heroId)
{
	return board->getOneByUId(_heroId);
}
