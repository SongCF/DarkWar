#pragma once

#include "ClientSkillProcessorBase.h"
class ClientChessBoard;
class ClientChessBoard_TeamFight;

// 客户端的技能处理器 （效果 + 动画 + 减血）
class ClientSkillProcessor : public ClientSkillProcessorBase
{
private:
	ClientChessBoard* boardA;
	ClientChessBoard* boardB;

	// 棋盘的访问方法
	virtual ClientChessBoardItemBase* getItemFromBoard(unsigned int _heroId);
public:
	// 
	ClientSkillProcessor(ClientChessBoard* boardA,ClientChessBoard* boardB);

	~ClientSkillProcessor(void);
};


class ClientSkillProcessor_TF : public ClientSkillProcessorBase
{
private:
	ClientChessBoard_TeamFight * board;

protected:
	// 棋盘的访问方法
	virtual ClientChessBoardItemBase* getItemFromBoard(unsigned int _heroId);
public:
	// 
	ClientSkillProcessor_TF(ClientChessBoard_TeamFight* board);

};
