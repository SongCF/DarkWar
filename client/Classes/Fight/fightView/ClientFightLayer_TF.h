#pragma once

#include "BaseLayer.h"
#include "FightDirector.h"
#include "ClientSkillProcessorBase.h"
#include "Bundle.h"
#include "flat.h"

class ClientChessBoard_TeamFight;
class FightTrigleDriver;

// 战斗界面
class ClientFightLayer_TF : public BaseLayer
{
private:
	int currentDirIndex;
	int mCurrentStepIndex;
	Bundle mBundle;
	vector<FightDirector> mDirvec;
	FightTrigleDriver* mTrigle;
	CCSprite* mBg;
	bool fightEnded;

	// endlayer 使用的
	Flat_TeamBossGetBattleScriptGiftRsp script;
	vector<HeroInfo> heros;

	ClientChessBoard_TeamFight* board;

	ClientSkillProcessorBase* mSkillProc;

	// 测试用的
	CCMenuItemFont* itemNext;

	// 从A/B棋盘里面寻找Id为这个的
	ClientChessBoardItemBase* getChessBoardItem(int heroUId);
	void removeSomeBuff(vector<int> buff_id_arr);
	void doBuff(BuffInStep buff);


	// 下一步
	void debugNextStepClicked(CCObject* sender);
	void debugGotoFirstDirctor(CCObject* sender);

	const char* getDebugStr();

	void menuItemClicked(CCObject* obj);

	// 用某个导演来表演
	void playWithDirctor(int dirIndex);
	void nodeHitGround(CCNode* node);
	void screenShake();
	void actionEnd(float t);
	// 开始
	void begingFightTrigle();

	// 初始化
	void initWithDirctorVecAndBg(vector<FightDirector> dir_vec,CCSprite* bg);

	// 下一场
	void timeToNextFight(float t);
	// 跳向结算界面
	void timeToJumpToEndLayer(float t);

	void fightTrigle(CCObject* null_obj);
	void waitTimeEnd(CCObject* null_obj);
	// oneStep的触发
	//void fightTrgile(float time);
	// 把step中的反弹数据分离出来
	vector<Buff_Doing> separationFanTanDataFromStep(OneStep& step);
	void setItemOrderBack(CCNode* node);

	// 完成了一个可视化的一步，见ChessBoardHelper的one_visible_step
	void one_visible_step(vector<CdDiscr> cdDisc);

	void jump_clicked(CCObject* sender);

	// 完毕
	void fightEnd(bool notWait = false);

	void someOneDead(CCObject* clientChessBoardItemBase);
	void showItemCard(CCNode* baoxiang);
	void itemActionEnd(CCNode* node);
	void equipActionEnd(CCNode* node);
	
	void setFightTrigleWaitSomeTime(float time_to_wait);
	//void pauseFightTrigle(CCNode* node_not_use);
	//void resumeFightTrigle(CCNode* node_not_use);

	void showFailByTakeTooMuchTimeDailog();

public:
	// 多场表演.不支持重载构造函数互调
	// 如果直接调用，相当于建立了一个临时的，而不是自己。
	ClientFightLayer_TF(vector<FightDirector> dir_vec,CCSprite* bg,Bundle bundle = Bundle());

	void setData(Flat_TeamBossGetBattleScriptGiftRsp script, vector<HeroInfo> heros);

	~ClientFightLayer_TF(void);
};

