#pragma once

#include <vector>

using std::vector;

//只有正常副本才会用， 地狱副本只有boss关卡
/*
 * MapsLayer 任务开始战斗时
		(直接在npc Board 点击了开始按钮的时候调用)
		1.开始一个新任务  clearJGGFootPrint 、
		2.老任务，前往九宫格 clearJGGFootprint 、 	
 * FightSettlementLayer 战斗结算时win， setFootprint(id, grid)
		再次闯关  ---> clearJGGFootpint
 * JiuGongGeLayer 刷新格子  getFootprintById
*/


class JGG_FootprintManager
{
public:
	static JGG_FootprintManager *getManager();

	//记录玩家哪些格子走过。
	vector<bool> getFootprintById(int id);
	void setFootprint(int id, int jggIndex);//走过就设置为true
	void clearJGGFootprint();//

	//
	void setHasDisplayBoss(bool flag = true){mHasDisplayBoss = flag;}
	bool getHasDisPlayBoss(){return mHasDisplayBoss;}

protected:
	JGG_FootprintManager();

	static JGG_FootprintManager *_manager;

	//只会有一个task正在进行
	int mTaskId;
	vector<bool> mFootprint;

	//刚进入的时候显示boss位置， 下次刷新进来后才再次显示
	bool mHasDisplayBoss;
};