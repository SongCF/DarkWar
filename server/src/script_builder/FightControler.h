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


// ����Ѫ���ķ�ֵ
#define WAIT_BLOOD 0.75f
// ��ȡ��ǰѪ���İٷֱ�
#define GET_CUR_HP_PERC(CHESSBOARDITEM) \
	( ((float)(CHESSBOARDITEM)->hero->getHp()) / (float)(CHESSBOARDITEM)->hero->getDefaultHp() )


enum ItemFilter
{
	ItemFilter_teammates	=	0,		// ����
	ItemFilter_enemy		=	1,		// ����
};

// ս�����̵Ŀ�����������ս����ʽ��ս�����󣬼�Ѫ
// �൱��CLient�˵�FightSystem
class FightControler
{
	//
	//friend class SkillHelper;
	friend class SkillHelperAdapterImpl;
private:
	ChessBoardHelper* mHelper;
	//SkillHelper* mSkillHelper;
	GameBuffProc* mBuffProc;

	// ��������
	ChessBoard* firstBloodBoard;

	SkillHelperBase* mSkillHelper;
	SkillHelperAdapterImpl* mSkillHelperAdatper;

	vector<int> mFightQueue;	// ����˳��Ķ��壬�����л��������̵�����
	int mCurrentFightIndex;
	// ��ʼ��ʱ���̵ĳ�ԱuniqueId
	// ֻ����ͳ�ƿ�Ѫ��ͳ�Ƶ�ʱ��
	vector<unsigned int> boardAInitUiqueId;
	vector<unsigned int> boardBInitUiqueId;

	// A��B�Ե���һ�ֵĹ�������ʷ��ֻ����һ�ֵ�
	vector<ChessBoardItem*> A_LastTurnFightHistory;
	vector<ChessBoardItem*> B_LastTurnFightHistory;

	int A_fightedHp;	// A�ӿ���B�ӵ�Ѫ��
	int B_fightedHp;	// B�ӿ���A�ӵ�Ѫ��

	// A�ӵĵ�ǰ������һ�ּ����Լ���Աloopһ�ξ���һ�֣�
	// ���a����1,2,3,4����ô1,2,3,4��һ�ξ���һ�֣���1,2,3,4��Щ��ΪĳЩԭ��û�д���ôwaitҲ��ģ�
	// ���Թ�һ�ξ���һ��
	int turnIndexA;	
	int turnIndexB;

	// �ж�u_id��item�Ƿ����ڳ�ʼ����ʱ����A/B����
	// ����boardAInitUiqueId�������ж�
	bool isInitedInBoardA(unsigned int u_id);
	bool isInitedInBoardB(unsigned int u_id);

	// ��fightIndex��ʼѭ���ң��ҵ�����
	ChessBoardItem* prepareFightItemFromIndex(int fightIndex,int& outIndex);

	// ׼���������Ľ�ɫ
	// lastItem = NULL�����صľ���index������ĵ�һ��
	// û���˾ͷ���false
	ChessBoardItem* prepareChessBoardFightItem(ChessBoardItem* lastItem,ChessBoard* chessBoard);

	// ��ȡfightorItem��ǰ����ʱ���������Ķ���
	// dis : -1����Ĭ�ϵĵ�ǰ�������룬����0 ������ָ���ľ���
	vector<ChessBoardItem*> getBeFightedItem(ChessBoardItem* fightorItem,float dis = -1);

	// ׼���ܹ��Ľ�ɫ��
	// fightorItem ָ����������Item
	// ���ر������Ľ�ɫ
	// û�оͷ���NULL
	// dis>=0������dis��Ϊָ���ľ��룬���disС��0����ô��ʹ��fightorItemĬ�ϵ�
	ChessBoardItem* prepareChessBoardBeFightedItem(ChessBoardItem* fightorItem,float dis = -1);

	// �����Y�����ϣ��ҹ�����Χ���棬����������Ǹ�����Item
	// dis>=0������dis��Ϊָ���ľ��룬���disС��0����ô��ʹ��fightorItemĬ�ϵ�
	ChessBoardItem* getYICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis = -1);
	// �����X�����ϣ��ҹ�����Χ���棬����������Ǹ�����Item
	// dis>=0������dis��Ϊָ���ľ��룬���disС��0����ô��ʹ��fightorItemĬ�ϵ�
	ChessBoardItem* getXICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis = -1);
	// �õ���itemSet�У����ҵĹ�����Χ���棬�ҿ��Թ����Ľ�ɫ(һ���ǻ��ŵ�);
	// dis>=0������dis��Ϊָ���ľ��룬���disС��0����ô��ʹ��fightorItemĬ�ϵ�
	ChessBoardItem* getICanFightItemInGongJiJuLi(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor,float dis = -1);
	// �õ���itemSet�У������fightor������filter���õ���Ӧ���������item
	// ItemFilter_enemy�������õ���Item�ǵ���
	ChessBoardItem* getNearestItemFromSet(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor,float juLi,ItemFilter filter = ItemFilter_enemy);
	// �õ���Χ���ҿ��Թ����ģ����������item������˳���ǣ�Y��X���Ա�
	// dis>=0������dis��Ϊָ���ľ��룬���disС��0����ô��ʹ��fightorItemĬ�ϵ�
	ChessBoardItem* getAroundICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis = -1);
	//
	// ��getAroundICanFightItemInGongJiJuLi�ı���
	// �����ݾ������Ƽ���
	// Y�����Ͼ����������һ��
	// ֻ��������û���Թ����ģ��о͹�����û�о���һ������
	// ��û�оͷ���NULL
	ChessBoardItem* getAroundICanFightItemInNearestByY(ChessBoardItem* fightor);

	// ���itemSet������Item��������������Ҫ����
	ChessBoardItem* getWeaklessAndDyingItem(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor);

	// ��ͼǰ��һ��
	// �ɹ����߾ͷ���true
	bool chessBoardItemTryGoStep(const ChessBoardItem* item,ChessBoard*& out_moveToBoard,int& out_posx,int& out_posy);


	// fightor ����һ��
	// Ҳ��ôι���ʱһ���ƶ�
	// Ҳ������ס�ˣ����ƶ���û��
	vector<OneStep> fight(ChessBoardItem* fightor);

	// fightorʹ��skill ʵʩ ����������out_step
	void tryFightWithSkill(ChessBoardItem* fightor,OneStep& out_step);
	// fightor��������ƶ���ʵʩ�ƶ�
	// �����Ƿ��ƶ��ˣ�true�����ƶ���
	bool tryMove(ChessBoardItem* fightor,OneStep& out_step);
	// ����ǰ�ִκ͵�ǰ�������figtor�ܷ�������
	bool canUseSkillInThisTurnAndCurrentSituation(ChessBoardItem* fightor);

	// ֱ��fightor����attackedItems��������outStep
	void fight_normal(OneStep& outStep,ChessBoardItem* fightor,vector<ChessBoardItem*> attackedItems);
	// ����skillFight��ָʾ�����м��ܵĹ���ʵʩ��������outStep
	void fight_skill(OneStep& outStep,SkillDataForFight skill,unsigned int f_uniqueId,SkillFight skillFight);

	PropertyCalculator* calculator;

	// ����Item�����ݣ����out_move
	void fileWithNoMove(StepMove& out_move,const ChessBoardItem* item);

	// ��item�ƶ������̵�ĳ��λ��
	void setItemMoveTo(StepMove& out_move,ChessBoardItem* item,ChessBoard* desBoard,int posX,int posY);

	// ��2����������õ����id�Ľ�ɫ��û�л��������ˣ�����NULL
	ChessBoardItem* getOneItemFrom2Board(int uniqueId);
	ChessBoardItem* getOneItemFromBoard(ChessBoard* board,int uniqueId);

public:
	FightControler(ChessBoardHelper* helper,GameBuffProc* proc);

	// �Զ���ʼ������
	// ��Ҫ�����̶���ʼ�����֮�����
	void initFirstFightBoard(FirstBloodBoard firstBoard);

	// ������̵ĵ�ǰ�Ĵ�����,���ǻغ���
	// �ͻ��˽��濴���ľ�����2�������ֵ
	int getBoardATurnIndex(){return turnIndexA;};
	int getBoardBTurnIndex(){return turnIndexB;};
	int getCurrentTurnIndex(){return (turnIndexA>turnIndexB?turnIndexA:turnIndexB);};

	// ������̿��Է����̵�Ѫ��
	int getBoardAFightedHp(){return A_fightedHp;};
	int getBoardBFightedHp(){return B_fightedHp;};

	// һ����ս
	//OneStep oneChessBoardFight();
	// ���ܻ᷵�ضಽ����fight��skill��premove �������
	vector<OneStep> oneChessBoardFight();

	~FightControler(void);
};

#define TEAM_BOSS_U_ID	5	// �Ŷ�Boss��Id

class TeamFightControler
{
	friend class SkillHelperTFAdapterImpl;
private:
	ChessBoard_TeamFight* teamBoard;
	GameBuffProc* mBuffProc;
	SkillHelperBase* mSkillHelper;
	SkillHelperTFAdapterImpl* mSkillHelperAdatper;
	//SkillHelper_TF* mSkillHelper;

	vector<int> mFightQueue;	// ����˳��Ķ��壬�����л��������̵�����
	int mCurrentFightIndex;

	int mLastBossFightedTeamUId;	// boss���һ�ε��幥�����Ŷ�

	// ��fightIndex��ʼѭ���ң��ҵ��Ĳ������Ǵ���
	ChessBoardItem_TeamFight* prepareFightItemFromIndex(int fightIndex,int& outIndex);

	ChessBoardItem_TeamFight* prepareBeFightedByBoss();

	// ���itemSet������Item��������������Ҫ����
	ChessBoardItem_TeamFight* getWeaklessAndDyingItem(vector<ChessBoardItem_TeamFight*> itemSet,ChessBoardItem_TeamFight* fightor);

	void fight_skill(OneStep& outStep,SkillDataForFight skill,unsigned int f_uniqueId,SkillFight skillFight);
	void fight_normal(OneStep& outStep,ChessBoardItem_TeamFight* fightor,vector<ChessBoardItem_TeamFight*> attackedItems);
	vector<OneStep> fight(ChessBoardItem_TeamFight* fightor);
public:
	TeamFightControler(ChessBoard_TeamFight* board,GameBuffProc* proc);

	//TeamFightControler(const TeamFightControler& cal);
	//TeamFightControler& operator = (const TeamFightControler& rightData); //��ֵ������

	// �Զ���ʼ������
	// ��Ҫ�����̶���ʼ�����֮�����
	void initFirstFightBoard();
	// ���ܻ᷵�ضಽ����fight��skill��premove �������
	vector<OneStep> oneChessBoardFight();
	~TeamFightControler(void);
};
