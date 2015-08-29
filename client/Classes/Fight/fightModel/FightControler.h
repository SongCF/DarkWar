#pragma once

#include "ChessBoardHelper.h"
#include "FightScriptDefine.h"
#include "ChessBoardHelper_TeamFight.h"
class GameBuffProc;
class SkillHelper;
class SkillHelper_TF;
class SkillHelperBase;
class SkillHelperAdapterImpl;
class SkillHelperTFAdapterImpl;
class ChessBoardHelper;
struct SkillFight;


// 渴望血量的伐值
#define WAIT_BLOOD 0.75f
// 获取当前血量的百分比
#define GET_CUR_HP_PERC(CHESSBOARDITEM) \
	( ((float)(CHESSBOARDITEM)->hero->getHp()) / (float)(CHESSBOARDITEM)->hero->getDefaultHp() )


enum ItemFilter
{
	ItemFilter_teammates	=	0,		// 队友
	ItemFilter_enemy		=	1,		// 敌人
};

// 战斗过程的控制器，控制战斗方式，战斗对象，减血
// 相当于CLient端的FightSystem
class FightControler
{
	//
	//friend class SkillHelper;
	friend class SkillHelperAdapterImpl;
private:
	ChessBoardHelper* mHelper;
	//SkillHelper* mSkillHelper;
	GameBuffProc* mBuffProc;

	// 先手棋盘
	ChessBoard* firstBloodBoard;

	SkillHelperBase* mSkillHelper;
	SkillHelperAdapterImpl* mSkillHelperAdatper;

	vector<int> mFightQueue;	// 攻击顺序的定义，不用切换攻击棋盘的那种
	int mCurrentFightIndex;
	// 初始化时棋盘的成员uniqueId
	// 只用于统计砍血量统计的时候
	vector<unsigned int> boardAInitUiqueId;
	vector<unsigned int> boardBInitUiqueId;

	// A对B对的上一轮的攻击的历史，只保存一轮的
	vector<ChessBoardItem*> A_LastTurnFightHistory;
	vector<ChessBoardItem*> B_LastTurnFightHistory;

	int A_fightedHp;	// A队砍掉B队的血量
	int B_fightedHp;	// B队砍掉A队的血量

	// A队的当前轮数，一轮即：自己队员loop一次就算一轮，
	// 如果a队是1,2,3,4，那么1,2,3,4过一次就是一轮，当1,2,3,4有些因为某些原因没有打，那么wait也算的，
	// 所以过一次就算一轮
	int turnIndexA;	
	int turnIndexB;

	// 判断u_id的item是否是在初始化的时候再A/B棋盘
	// 根据boardAInitUiqueId来做的判断
	bool isInitedInBoardA(unsigned int u_id);
	bool isInitedInBoardB(unsigned int u_id);

	// 从fightIndex开始循环找，找到存活的
	ChessBoardItem* prepareFightItemFromIndex(int fightIndex,int& outIndex);

	// 准备攻击方的角色
	// lastItem = NULL，返回的就是index的升序的第一个
	// 没有了就返回false
	ChessBoardItem* prepareChessBoardFightItem(ChessBoardItem* lastItem,ChessBoard* chessBoard);

	// 获取fightorItem当前攻击时，被攻击的对象
	// dis : -1就用默认的当前攻击距离，大于0 ，就用指定的距离
	vector<ChessBoardItem*> getBeFightedItem(ChessBoardItem* fightorItem,float dis = -1);

	// 准备受功的角色的
	// fightorItem 指定攻击方的Item
	// 返回被攻击的角色
	// 没有就返回NULL
	// dis>=0就是用dis作为指定的距离，如果dis小于0，那么就使用fightorItem默认的
	ChessBoardItem* prepareChessBoardBeFightedItem(ChessBoardItem* fightorItem,float dis = -1);

	// 获得在Y方向上，我攻击范围里面，离我最近的那个敌人Item
	// dis>=0就是用dis作为指定的距离，如果dis小于0，那么就使用fightorItem默认的
	ChessBoardItem* getYICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis = -1);
	// 获得在X方向上，我攻击范围里面，离我最近的那个敌人Item
	// dis>=0就是用dis作为指定的距离，如果dis小于0，那么就使用fightorItem默认的
	ChessBoardItem* getXICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis = -1);
	// 得到在itemSet中，在我的攻击范围里面，我可以攻击的角色(一定是活着的);
	// dis>=0就是用dis作为指定的距离，如果dis小于0，那么就使用fightorItem默认的
	ChessBoardItem* getICanFightItemInGongJiJuLi(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor,float dis = -1);
	// 得到在itemSet中，相对于fightor，根据filter，得到相应距离里面的item
	// ItemFilter_enemy：则代表得到的Item是敌人
	ChessBoardItem* getNearestItemFromSet(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor,float juLi,ItemFilter filter = ItemFilter_enemy);
	// 得到周围，我可以攻击的，离我最近的item，优先顺序是：Y，X，旁边
	// dis>=0就是用dis作为指定的距离，如果dis小于0，那么就使用fightorItem默认的
	ChessBoardItem* getAroundICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis = -1);
	//
	// 是getAroundICanFightItemInGongJiJuLi的变种
	// 不根据距离限制计算
	// Y方向上距离最近的那一排
	// 只看横排有没可以攻击的，有就攻击，没有就下一个横排
	// 都没有就返回NULL
	ChessBoardItem* getAroundICanFightItemInNearestByY(ChessBoardItem* fightor);

	// 获得itemSet最弱的Item，防御最弱，快要死的
	ChessBoardItem* getWeaklessAndDyingItem(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor);

	// 视图前进一步
	// 成功行走就返回true
	bool chessBoardItemTryGoStep(const ChessBoardItem* item,ChessBoard*& out_moveToBoard,int& out_posx,int& out_posy);


	// fightor 攻击一次
	// 也许该次攻击时一次移动
	// 也或许被挡住了，连移动都没有
	vector<OneStep> fight(ChessBoardItem* fightor);

	// fightor使用skill 实施 攻击，返回out_step
	void tryFightWithSkill(ChessBoardItem* fightor,OneStep& out_step);
	// fightor如果可以移动就实施移动
	// 返回是否移动了，true就是移动了
	bool tryMove(ChessBoardItem* fightor,OneStep& out_step);
	// 看当前轮次和当前的情况，figtor能发技能吗
	bool canUseSkillInThisTurnAndCurrentSituation(ChessBoardItem* fightor);

	// 直接fightor攻击attackedItems，并生成outStep
	void fight_normal(OneStep& outStep,ChessBoardItem* fightor,vector<ChessBoardItem*> attackedItems);
	// 根据skillFight的指示，进行技能的攻击实施，并生成outStep
	void fight_skill(OneStep& outStep,SkillDataForFight skill,unsigned int f_uniqueId,SkillFight skillFight);

	PropertyCalculator* calculator;

	// 根据Item的数据，填充out_move
	void fileWithNoMove(StepMove& out_move,const ChessBoardItem* item);

	// 把item移动到棋盘的某个位置
	void setItemMoveTo(StepMove& out_move,ChessBoardItem* item,ChessBoard* desBoard,int posX,int posY);

	// 从2个棋盘里面得到这个id的角色，没有或者死亡了，返回NULL
	ChessBoardItem* getOneItemFrom2Board(int uniqueId);
	ChessBoardItem* getOneItemFromBoard(ChessBoard* board,int uniqueId);

public:
	FightControler(ChessBoardHelper* helper,GameBuffProc* proc);

	// 自动初始化先手
	// 需要在棋盘都初始化完毕之后调用
	void initFirstFightBoard(FirstBloodBoard firstBoard);

	// 获得棋盘的当前的打斗轮数,就是回合数
	// 客户端界面看到的就是这2个的最大值
	int getBoardATurnIndex(){return turnIndexA;};
	int getBoardBTurnIndex(){return turnIndexB;};
	int getCurrentTurnIndex(){return (turnIndexA>turnIndexB?turnIndexA:turnIndexB);};

	// 获得棋盘砍对方棋盘的血量
	int getBoardAFightedHp(){return A_fightedHp;};
	int getBoardBFightedHp(){return B_fightedHp;};

	// 一次作战
	//OneStep oneChessBoardFight();
	// 可能会返回多步：把fight和skill的premove 提出来了
	vector<OneStep> oneChessBoardFight();

	~FightControler(void);
};

#define TEAM_BOSS_U_ID	5	// 团队Boss的Id

class TeamFightControler
{
	friend class SkillHelperTFAdapterImpl;
private:
	ChessBoard_TeamFight* teamBoard;
	GameBuffProc* mBuffProc;
	SkillHelperBase* mSkillHelper;
	SkillHelperTFAdapterImpl* mSkillHelperAdatper;
	//SkillHelper_TF* mSkillHelper;

	vector<int> mFightQueue;	// 攻击顺序的定义，不用切换攻击棋盘的那种
	int mCurrentFightIndex;

	int mLastBossFightedTeamUId;	// boss最近一次单体攻击的团队

	// 从fightIndex开始循环找，找到的不见得是存活的
	ChessBoardItem_TeamFight* prepareFightItemFromIndex(int fightIndex,int& outIndex);

	ChessBoardItem_TeamFight* prepareBeFightedByBoss();

	// 获得itemSet最弱的Item，防御最弱，快要死的
	ChessBoardItem_TeamFight* getWeaklessAndDyingItem(vector<ChessBoardItem_TeamFight*> itemSet,ChessBoardItem_TeamFight* fightor);

	void fight_skill(OneStep& outStep,SkillDataForFight skill,unsigned int f_uniqueId,SkillFight skillFight);
	void fight_normal(OneStep& outStep,ChessBoardItem_TeamFight* fightor,vector<ChessBoardItem_TeamFight*> attackedItems);
	vector<OneStep> fight(ChessBoardItem_TeamFight* fightor);
public:
	TeamFightControler(ChessBoard_TeamFight* board,GameBuffProc* proc);

	//TeamFightControler(const TeamFightControler& cal);
	//TeamFightControler& operator = (const TeamFightControler& rightData); //赋值符重载

	// 自动初始化先手
	// 需要在棋盘都初始化完毕之后调用
	void initFirstFightBoard();
	// 可能会返回多步：把fight和skill的premove 提出来了
	vector<OneStep> oneChessBoardFight();
	~TeamFightControler(void);
};
