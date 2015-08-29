#pragma once
#include "FightScriptDefine.h"
class GameBuffItemGetter;

// GameBuff的处理器
// 处理棋盘的角色的Buff运作
class GameBuffProc
{
private:
	int mCurrentUsedBuffId;
	//ChessBoardHelper* mHelper;
	GameBuffItemGetter* mGetter;
public:
	GameBuffProc(GameBuffItemGetter* getter);
	~GameBuffProc(void);

	// 返回false，就不能fight，即带有眩晕的buff
	// 移除的buff(buffeffect)的id会放在removedBuff数组里面
	bool beforeFight(int fightorUniqueId,BuffInStep& buff);

	// 战斗的回合变换
	// 双方都打了一圈了，就触发。
	// 在beforeFight之前触发
	// 移除的buff(buffeffect)的id会放在removedBuff数组里面
	void fightRoundChange(BuffInStep& buff);

	// 一个步奏：只处理血量的buff,move fight skill 3种。
	// 移除的buff(buffeffect)的id会放在removedBuff数组里面
	void one_visible_step(BuffInStep& buff);
	//// 角色攻击或被攻击了，也许是敌人攻击，也许是队友的附加帮助
	//// 只处理攻击和被攻击之后的buff移除，没法处理眩晕，因为这个函数是都攻击了之后调用的
	//// 如果有眩晕的话，这个函数是不会调用的
	//// 移除的buff(buffeffect)的id会放在removedBuff数组里面
	//void fight_end_or_beattacked(int fightorUniqueId,int targetUniqueId,vector<int>& removedBuff);


	// 角色攻击或被攻击了，也许是敌人攻击，也许是队友的附加帮助
	// 只处理攻击和被攻击之后的buff移除，没法处理眩晕，因为这个函数是都攻击了之后调用的
	// 如果有眩晕的话，这个函数是不会调用的
	// 移除的buff(buffeffect)的id会放在removedBuff数组里面
	void fight_end(int fightorUniqueId,BuffInStep& buff);
	void beattacked(int fightorUniqueId,int targetUniqueId,int targetBloodChange,BuffInStep& buff);

	// 生成一个Buff的Id
	int genOneBuffId(){return ++mCurrentUsedBuffId;};
};

