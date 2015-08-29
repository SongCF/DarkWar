#pragma once

#include "protocol.h"
#include <vector>
#include "ScriptDataProvider.h"
#include "GameBuffItemGetter.h"
#include "FightScriptDefine.h"
#include "ChessboardItemBase.h"
using namespace std;

// ���̵ĸ�����
#define GRID_NUM_X		2
#define GRID_NUM_Y		2

class ChessBoard;
class ChessBoardItem;


class ChessBoardItem : public ChessBoardItemBase
{
private:
	const ChessBoard* geneBoard;	// Ψһ��ʾ�����ݵģ�����һ���������޸�

	ChessBoard* originBoard;	// ��ʼ����,�����ж��Ƿ��Ƕ��ѵ�Ψһ���
	int originIndex;

public:
	ChessBoardItem(ChessBoard* originBoard,int originIndex,unsigned int uniqueId):ChessBoardItemBase(uniqueId)
	{
		this->geneBoard = originBoard;
		this->originBoard = originBoard;
		this->originIndex = originIndex;
		this->currentBoard = NULL;
		this->hero = NULL;
		this->posx = -1;
		this->posy = -1;
	}
	~ChessBoardItem()
	{
		if (hero != NULL)
		{
			delete hero;
		}
	}
	int getOriginIndex(){return originIndex;};
	// Ψһ��;���ж��Ƿ��������Ƕ��ѾͿ����ǵ�ԭʼ����
	const ChessBoard* getOriginBoard()const {return originBoard;};
	// �޸ĳ������̣���Ҫ���ڻ���������Ҳ���Ǽ�ϸ��ʵ��
	void changeOriginBoard(ChessBoard* board){originBoard = board;};

	const ChessBoard* getGeneBoard(){return geneBoard;};

	void setOriginBoardCanUsedBlooldNum(short num);

	// ��test�����Ƿ��Ƕ���
	bool isTeammate(ChessBoardItem* test){return test->getOriginBoard() == originBoard;};

	// �����е�posx,posy
	int posx;
	int posy;	
	ChessBoard* currentBoard;	// ��ǰ����
};
class ChessBoard
{
private:
	//ChessBoardCallback* callback;
	string idName;

	short currentBloodNum;		// �����̵�ǰ���õ�Ѫƿ����
	short defaultBloodNum;		// Ĭ�ϵĸ���
	int bloodAddPercent;		// Ѫƿ�ļ�Ѫ�ٷֱ�
public:
	void setName(string name);
	string getName() const;
	ChessBoard();
	void setCanUsedBloodNum(short bloodNum){currentBloodNum = bloodNum;};
	short getCurrentBloodNum() const {return currentBloodNum;};
	void setDefaultBloodNum(short bloodNum){defaultBloodNum = bloodNum;};
	short getDefaultBloodNum() const {return defaultBloodNum;};
	void setBloodAddPercent(int per){bloodAddPercent = per;};
	int getBloodAddPercent() const{return bloodAddPercent;};
	// ����delete��
	void addItem(ChessBoardItem* item,int pos_x,int pos_y);
	void removeItem(ChessBoardItem* item);
	void removeItem(int x_index,int y_index);
	bool isItemExist(ChessBoardItem* item);
	bool isPosOK(int x_index,int y_index);
	// �������һ������ĵ�Item������
	int getItemNumInY(int y_index,bool justAlive);
	// �õ�ĳ��λ���ϵ�Item
	ChessBoardItem* getActorItem(int x_index,int y_index,bool includeWideItem = true);
	ChessBoardItem* getActorItem(unsigned int uniqueId);
	int getMingJie();
	// �õ�xIndex�ϵ�����Ԫ�أ�������һ��������
	// Ĭ�ϻ�ѿ��ItemҲ��������
	vector<ChessBoardItem*> getYItems(int xIndex,bool includeWideItem = true);
	// �õ�xIndex�ϵ�����Ԫ�أ�������һ��������
	vector<ChessBoardItem*> getXItems(int yIndex);
	vector<ChessBoardItem*> getInBoardItems();
private:
	// ������������е�Ԫ��
	vector<ChessBoardItem*> currentItems;
};


// �������̵�
// ������Ҫ����������ս������ʱ���𵽰���������
class ChessBoardHelper : public GameBuffItemGetter
{
public:
	ChessBoardHelper();
	~ChessBoardHelper();
	void initHelper(ScriptDataProvider* dataProvider,
		unsigned int teamA_hero_ids_4[],		// teamA�Ľ�ɫ��Id����Ӧ���ݿ�ȫ��Ψһ
		unsigned int uniqueId_4_A[],			// teamA�Ľ�ɫ��Ψһid�������ָ���ģ�����һ��ս����ȫ��Ψһ
		unsigned int teamB_hero_ids_4[],
		unsigned int uniqueId_4_B[],
		ScriptBuilderUserData userData);
	// ������Ƿ�ɹ���
	// ���� >0 --> ��Aսʤ��B
	// ���� <0 --> ��Bսʤ��A
	// ���� =0 --> ��û�ֳ�ʤ��
	int checkSuccess();

	// ��ս�����е��õ�
	// �ɼ��Ĳ��඼���
	void one_visible_step(vector<CdDiscr>& cd_time_arr,unsigned int action_item_uId);

	// ��ȡ����ӵ���һ��Item,��ѭ��ȡ��-->���һ������һ�����ǵ�0��
	// ���currentItem = NULL���ͷ��ض���index����ĵ�һ��
	// ������˾ͷ���NULL
	ChessBoardItem* getTeamNextItem(ChessBoard* originBoard,ChessBoardItem* currentItem);

	// ��BordA��BoardB��Ѱ��ԭʼ������orginBoard��Ԫ��
	int getAlivedItemNumByOriginBoard(ChessBoard* orginBoard);

	// ���ԭʼ������orginBoard�����л��ŵ�Item,orginBoard=NULL,��ôÿ�����̶�����
	vector<ChessBoardItem*> getAlivedItemsByOriginBoard(const ChessBoard* orginBoard);
	// ���item�����߷����ϵ�����Item������2�����̵�
	// �е��ˣ��ж���
	vector<ChessBoardItem*> getYItems(int xIndex);
	// ��ú����ϵ�Items
	vector<ChessBoardItem*> getXItems(ChessBoard* board, int yIndex);
	// ��ú����ϵ�Items,���ŵ�
	vector<ChessBoardItem*> getAlivedXItems(ChessBoard* board, int yIndex);

	// ��item1��item2�ľ��룬֧�ֿ����̼���
	float distance(ChessBoardItem* item1,ChessBoardItem* item2);

	// ��ȡitem��Y������Ϊdis������
	// Ѱ��item��ǰ������Ϊdis��һ��Item����νǰ��������Լ�ԭʼ���̵�Y��ݼ��ķ���
	// ֻ���Y��ֵ�����Կ��Դ����Item
	void getNextYPosOfItem(const ChessBoardItem* item,ChessBoard*& out_pBoard,int& out_posy,int dis = 1 );

	//// ���item��NextY��Item��dis = 1
	//// ���ܴ����Item��ǰ��,����ǰ�������Wide��Item����ô�㲻��Ļ�����Ҫ�ò���ָ��
	//ChessBoardItem* getOneNextFrontOfItem(const ChessBoardItem* item,bool includeWideItem = true);
	// ��2������������ң��ҵ�uniqueId��Item���оͷ��أ�û�оͷ���NULL
	ChessBoardItem* getOneItemByUniqueId(unsigned int uniqueId);


	// ����Item�����������uniqueId���õ�����AbstractItem����
	virtual AbstractItem* getAbstractItemByUniqueId(unsigned int uniqueId);
	// ��ȡȫ���Ļ��ŵ�Item����
	virtual vector<AbstractItem*> getAllAlivedItems();
	// �Ƿ��Ƕ���
	virtual bool isTeammate(unsigned int uniqueId1,unsigned int uniqueId2);
	// 
	virtual unsigned int getItemUniqueId(AbstractItem* item);

	// ��x,y��grid����ת������������
	int getIndex(int x_index,int y_index);
	void getGridPos(int index,int& out_x_index,int& out_y_index);
	ChessBoard* getBoardA(){return boardA;};
	ChessBoard* getBoardB(){return boardB;};
	// ��Item�ĵ�ǰposy,ת����ָ����board��posy
	int convertToBoardGridY(const ChessBoard* board,const ChessBoardItem* item);
	int convertToOriginBoardGridY(const ChessBoardItem* item);
private:
	ChessBoard* boardA;
	ChessBoard* boardB;
	ScriptDataProvider* mDataProvider;
	// �������new������Item����ʱdelete
	vector<ChessBoardItem*> itemPool;

	void removeItemFromVec(vector<ChessBoardItem*>& items,ChessBoardItem* toRemove);
	//
	void sortChessBoardItemsByIndexInc(vector<ChessBoardItem*> &toSort);
	void initChessBoard(ChessBoard* board,
		unsigned int team_hero_ids_4[],		// ��Ӧ���ݿ⣬ȫ��Ψһ
		unsigned int team_unique_ids_4[],	// ��Ӧһ��ս����ȫ��Ψһ
		ScriptBuilderUserData userData);
};

