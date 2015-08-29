#include "ChessBoardHelper.h"
#include "GameBuffProc.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void ChessBoard::setName(string name)
{
	idName = name;
}
string ChessBoard::getName() const
{
	return idName;
}
ChessBoard::ChessBoard()
{
	//callback = NULL;
	defaultBloodNum = 0;
	currentBloodNum = 0;
	bloodAddPercent = 0;
}

// ����delete��
void ChessBoard::addItem(ChessBoardItem* item,int pos_x,int pos_y)
{
	CCAssert(item->posx == -1 && item->posy == -1,"");

	if (isItemExist(item))
	{
		// ������Ѿ������
		CCAssert(false,"");
		return;
	}

	if (!isPosOK(pos_x,pos_y))
	{
		CCAssert(false,"");
		return;
	}

	if (item->currentBoard != NULL)
	{
		CCAssert(false,"");
	}

	if (getActorItem(pos_x,pos_y,true) != NULL)
	{
		// ���λ��������
		CCAssert(false,"");
		return;
	}

	// �����޸�Origin
	item->currentBoard = this;
	item->posx = pos_x;
	item->posy = pos_y;
	//if (callback)
	//	callback->itemAdd(item);

	currentItems.push_back(item);
}
void ChessBoard::removeItem(ChessBoardItem* item)
{
	int num = currentItems.size();
	vector<ChessBoardItem*>::iterator it;

	for (it = currentItems.begin();it!=currentItems.end();it++)
	{
		if ( (*it) == item)
		{
			//if (callback)
			//	callback->itemRemoved(item);

			item->currentBoard = NULL;
			item->posx = -1;
			item->posy = -1;
			currentItems.erase(it);
			break;
		}
	}
}

void ChessBoard::removeItem(int x_index,int y_index)
{
	ChessBoardItem* item = getActorItem(x_index,y_index,true);

	if (item != NULL)
	{
		removeItem(item);
	}
}

bool ChessBoard::isItemExist(ChessBoardItem* item)
{
	int num = currentItems.size();
	for (int i=0;i<num;i++)
	{
		if (currentItems[i] == item)
		{
			return true;
		}
	}
	return false;
}

bool ChessBoard::isPosOK(int x_index,int y_index)
{
	return x_index >=0 && y_index >=0 && x_index < GRID_NUM_X && y_index < GRID_NUM_Y;
}
int ChessBoard::getItemNumInY(int y_index,bool justAlive)
{
	int num = 0;

	ChessBoardItem* frontLeftItem = getActorItem(0,y_index);
	ChessBoardItem* frontRightItem = getActorItem(1,y_index);

	if (frontLeftItem != NULL)
	{
		if (justAlive && frontLeftItem->hero->getHp() > 0)
		{
			num ++;
		}
		else if (!justAlive)
		{
			num ++;
		}
	}
	// ����frontRightItem��frontLeftItem��ͬһ��
	if (frontRightItem != NULL && frontRightItem != frontLeftItem)
	{
		if (justAlive && frontRightItem->hero->getHp() > 0)
		{
			num ++;
		}
		else if (!justAlive)
		{
			num ++;
		}
	}

	return num;
}
// �õ�ĳ��λ���ϵ�Item,ռ��һ�ŵ�Ҳ�᷵��
ChessBoardItem* ChessBoard::getActorItem(int x_index,int y_index,bool includeWideItem /* = true */)
{
	if (!isPosOK(x_index,y_index))
	{
		return NULL;
	}

	int num = currentItems.size();
	for (int i=0;i<num;i++)
	{
		if (currentItems[i]->posx == x_index && currentItems[i]->posy == y_index)
		{
			return currentItems[i];
		}
		else if (currentItems[i]->posy == y_index && currentItems[i]->hero->getXSize() > 1 && includeWideItem)
		{
			return currentItems[i];
		}
	}
	return NULL;
}

ChessBoardItem* ChessBoard::getActorItem(unsigned int uniqueId)
{
	int num = currentItems.size();
	for (int i=0;i<num;i++)
	{
		if (currentItems[i]->getUniqueId() == uniqueId)
		{
			return currentItems[i];
		}
	}
	return NULL;
};

int ChessBoard::getMingJie()
{
	int ret = 0;
	int num = currentItems.size();
	for (int i=0;i<num;i++)
	{
		ret += currentItems[i]->hero->getMinJie();
	}
	return ret;
}
// �õ�xIndex�ϵ�����Ԫ�أ�������һ��������,����ռ��һ�����ŵ�Ҳ�᷵�أ���ΪҲ��������
vector<ChessBoardItem*> ChessBoard::getYItems(int xIndex,bool includeWideItem /* = true */)
{
	vector<ChessBoardItem*> items;

	int num = currentItems.size();
	for (int i=0;i<num;i++)
	{
		// ռ��2��λ�ÿ�Ҳ��
		if (currentItems[i]->posx == xIndex)
		{
			items.push_back(currentItems[i]);
		}
		else if (currentItems[i]->hero->getXSize() > 1 && includeWideItem)
		{
			items.push_back(currentItems[i]);
		}
	}

	return items;
}
// �õ�xIndex�ϵ�����Ԫ�أ�������һ��������
vector<ChessBoardItem*> ChessBoard::getXItems(int yIndex)
{
	vector<ChessBoardItem*> items;

	int num = currentItems.size();
	for (int i=0;i<num;i++)
	{
		if (currentItems[i]->posy == yIndex)
		{
			items.push_back(currentItems[i]);
		}
	}

	return items;
}

vector<ChessBoardItem*> ChessBoard::getInBoardItems(){return currentItems;};



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ChessBoardHelper::ChessBoardHelper()
{
	boardA = NULL;
	boardB = NULL;
	mDataProvider = NULL;
}
ChessBoardHelper::~ChessBoardHelper()
{
	int num = itemPool.size();
	for (int i=0;i<num;i++)
	{
		delete itemPool[i];
	}
	itemPool.clear();

	delete boardA;
	delete boardB;
}
void ChessBoardHelper::initChessBoard(ChessBoard* board, 
									  unsigned int team_hero_ids_4[], 
									  unsigned int team_unique_ids_4[], 
									  ScriptBuilderUserData userData)
{
	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		// ��ЧId
		if (team_hero_ids_4[i] == INVALID_ID)
			continue;

		int posX;
		int posY;
		getGridPos(i,posX,posY);

		ChessBoardItem* item = new ChessBoardItem(board,i,team_unique_ids_4[i]);
		//mDataProvider->getHero(item->hero,team_hero_ids_4[i],userData);
		item->hero = mDataProvider->getBaseHero(team_hero_ids_4[i],userData);

		if (item == NULL)
			CCAssert(false,"�ⲻ������NULL���������ݿ�Ĺ���ɾ���ˣ�");

		itemPool.push_back(item);
		board->addItem(item,posX,posY);
	}
}
void ChessBoardHelper::initHelper(ScriptDataProvider* dataProvider, 
								  unsigned int teamA_hero_ids_4[], 
								  unsigned int uniqueId_4_A[], 
								  unsigned int teamB_hero_ids_4[], 
								  unsigned int uniqueId_4_B[], 
								  ScriptBuilderUserData userData)
{
	this->boardA = new ChessBoard();
	this->boardB = new ChessBoard();

	this->boardA->setName("A");
	this->boardB->setName("B");

	this->mDataProvider = dataProvider;

	userData.teamTag = TEAM_TAG_A;
	this->initChessBoard(boardA,teamA_hero_ids_4,uniqueId_4_A,userData);
	short defaultBloodNum_A;
	short currentBloodNum_A;
	int percent_A;
	mDataProvider->getTeamBloodNum(currentBloodNum_A,defaultBloodNum_A,percent_A,userData);
	CCAssert(defaultBloodNum_A >= currentBloodNum_A,"");
	boardA->setCanUsedBloodNum(currentBloodNum_A);
	boardA->setDefaultBloodNum(defaultBloodNum_A);
	boardA->setBloodAddPercent(percent_A);

	userData.teamTag = TEAM_TAG_B;
	this->initChessBoard(boardB,teamB_hero_ids_4,uniqueId_4_B,userData);
	short defaultBloodNum_B;
	short currentBloodNum_B;
	int percent_B;
	mDataProvider->getTeamBloodNum(currentBloodNum_B,defaultBloodNum_B,percent_B,userData);
	CCAssert(defaultBloodNum_B >= currentBloodNum_B,"");
	boardB->setCanUsedBloodNum(currentBloodNum_B);
	boardB->setDefaultBloodNum(defaultBloodNum_B);
	boardB->setBloodAddPercent(percent_B);
}
//
int ChessBoardHelper::checkSuccess()
{
#if (USE_FIGHT_LOG == 1)
	CCLog("\n---------Team Actor Infor-----------");
	CCLog("A");
#endif

	int numA = getAlivedItemNumByOriginBoard(boardA);

#if (USE_FIGHT_LOG == 1)
	CCLog("B");
#endif

	int numB = getAlivedItemNumByOriginBoard(boardB);

#if (USE_FIGHT_LOG == 1)
	CCLog("------------------------------------");
#endif

	// B Ӯ��
	if ( numA <= 0)
	{
		return -1;
	}
	// A Ӯ��
	else if (numB <= 0)
	{
		return 1;
	}

	return 0;
}

void ChessBoardHelper::one_visible_step(vector<CdDiscr>& cd_time_arr,unsigned int action_item_uId)
{
	// �޸ģ�2014-03-08
	// ������step�������ĳ���˵�cdʱ���ˣ����Ƶ�ÿһ��Turn�л��ĵ�ȥ����ʱ����
	return;


	// ֻ����action_item_uId��cdʱ��
	// A ����
	{
		vector<ChessBoardItem*> itemInA = boardA->getInBoardItems();
		int itemNumInA = itemInA.size();
		for (int i=0;i<itemNumInA;i++)
		{
			if (itemInA[i]->getUniqueId() != action_item_uId)
				continue;

			int num = itemInA[i]->hero->getCdTime() - 1;
			itemInA[i]->hero->setCdTime(num>0?num:0);
			CdDiscr oneDisc;
			oneDisc.cdTime = itemInA[i]->hero->getCdTime();
			oneDisc.uniqueId = itemInA[i]->getUniqueId();
			cd_time_arr.push_back(oneDisc);
		}
	}

	// B ����
	{
		vector<ChessBoardItem*> itemInB = boardB->getInBoardItems();
		int itemNumInB = itemInB.size();
		for (int i=0;i<itemNumInB;i++)
		{
			if (itemInB[i]->getUniqueId() != action_item_uId)
				continue;

			int num = itemInB[i]->hero->getCdTime() - 1;
			itemInB[i]->hero->setCdTime(num>0?num:0);

			CdDiscr oneDisc;
			oneDisc.cdTime = itemInB[i]->hero->getCdTime();
			oneDisc.uniqueId = itemInB[i]->getUniqueId();
			cd_time_arr.push_back(oneDisc);
		}
	}



	//// A ����
	//{
	//	vector<ChessBoardItem*> itemInA = boardA->getInBoardItems();
	//	int itemNumInA = itemInA.size();
	//	for (int i=0;i<itemNumInA;i++)
	//	{
	//		int num = itemInA[i]->hero->getCdTime() - 1;
	//		itemInA[i]->hero->setCdTime(num>0?num:0);
	//		CdDiscr oneDisc;
	//		oneDisc.cdTime = itemInA[i]->hero->getCdTime();
	//		oneDisc.uniqueId = itemInA[i]->getUniqueId();
	//		cd_time_arr.push_back(oneDisc);
	//	}
	//}

	//// B ����
	//{
	//	vector<ChessBoardItem*> itemInB = boardB->getInBoardItems();
	//	int itemNumInB = itemInB.size();
	//	for (int i=0;i<itemNumInB;i++)
	//	{
	//		int num = itemInB[i]->hero->getCdTime() - 1;
	//		itemInB[i]->hero->setCdTime(num>0?num:0);

	//		CdDiscr oneDisc;
	//		oneDisc.cdTime = itemInB[i]->hero->getCdTime();
	//		oneDisc.uniqueId = itemInB[i]->getUniqueId();
	//		cd_time_arr.push_back(oneDisc);
	//	}
	//}
}
//
ChessBoardItem* ChessBoardHelper::getTeamNextItem(ChessBoard* originBoard,ChessBoardItem* currentItem)
{
	vector<ChessBoardItem*> items = getAlivedItemsByOriginBoard(originBoard);
	int num = items.size();
	if (num <=0)
	{
		return NULL;
	}

	//��������
	sortChessBoardItemsByIndexInc(items);

	int currentIndex = currentItem!=NULL?currentItem->getOriginIndex():-1;
	int nextIndex = 0;

	if (currentIndex >= 0)
	{
		for (int i=0;i<num;i++)
		{
			// �ҵ��������index�ģ�Ҳ������һ��Ԫ��
			// �����ǰ�����һ������ô�Ͳ����ҵõ�����ôҲ�պþ��ǵ�һ����nextIndex = 0
			if (items[i]->getOriginIndex() > currentIndex)
			{
				nextIndex = i;
				break;
			}
		}
	}
	return items[nextIndex];
}
//
void ChessBoardHelper::sortChessBoardItemsByIndexInc(vector<ChessBoardItem*> &toSort)
{
	int num = toSort.size();
	ChessBoardItem* temp = NULL;

	for (int i=0;i<num;i++)
	{
		for (int j=i+1;j<num;j++)
		{
			// ǰ��ĸ��󣬽���
			if (toSort[i]->getOriginIndex() > toSort[j]->getOriginIndex())
			{
				temp = toSort[i];
				toSort[i] = toSort[j];
				toSort[j] = temp;
			}
		}
	}
}
//
vector<ChessBoardItem*> ChessBoardHelper::getAlivedItemsByOriginBoard(const ChessBoard* orginBoard)
{
	vector<ChessBoardItem*> retVec;

	// A ����Ѱ��
	{
		vector<ChessBoardItem*> items = boardA->getInBoardItems();
		int itemsNumInBoardA = items.size();
		for (int i=0;i<itemsNumInBoardA;i++)
		{
			if ( (items[i]->getOriginBoard() == orginBoard || orginBoard == NULL) && items[i]->hero->getHp() > 0)
			{
				retVec.push_back(items[i]);
			}
		}
	}
	// B ����Ѱ��
	{
		vector<ChessBoardItem*> items = boardB->getInBoardItems();
		int itemsNumInBoardB = items.size();
		for (int i=0;i<itemsNumInBoardB;i++)
		{
			if ( (items[i]->getOriginBoard() == orginBoard || orginBoard == NULL) && items[i]->hero->getHp() > 0)
			{
				retVec.push_back(items[i]);
			}
		}
	}

	return retVec;
}

//
int ChessBoardHelper::getAlivedItemNumByOriginBoard(ChessBoard* orginBoard)
{
	//return getAlivedItemsByOriginBoard(orginBoard).size();

	vector<ChessBoardItem*> items = getAlivedItemsByOriginBoard(orginBoard);
	int num = items.size();
	// log
	{
#if (USE_FIGHT_LOG == 1)
		for (int i=0;i<num;i++)
		{
			string buffArray = "";//{buff--->[2,3,5] | buff-->[0]}

			list<GameBuff> buffs = items[i]->hero->getCurrentAttachedBuff();

			for (list<GameBuff>::iterator it = buffs.begin();
				it != buffs.end();it++)
			{
				string eff_id_array = "";
				for (list<GameBuffEffect>::iterator it_eff = it->effect_vec.begin();it_eff != it->effect_vec.end();it_eff++)
				{
					char num[8];
					sprintf(num,"%d",it_eff->effect_id);
					eff_id_array += string(num);
					eff_id_array += ",";
				}
				if (eff_id_array.length() > 0)
				{
					eff_id_array = eff_id_array.substr(0,eff_id_array.length()-1);
				}
				char temp[128];
				sprintf(temp,"buff--->[%s]",eff_id_array.c_str());
				buffArray += string(temp);
				buffArray += " | ";
			}
			if (buffArray.length() > 0)
			{
				buffArray = buffArray.substr(0,buffArray.length() - 3);
			}
			CCLog("item[index=%d/%d] u_id = %u, hp = [%d/%d],board = %s,boardBloodNum=[%d/%d], pos = [%d,%d],cdTime = [%d/%d],buff-->{%s}",
				i,num-1,items[i]->getUniqueId(),items[i]->hero->getHp(),items[i]->hero->getDefaultHp(),items[i]->currentBoard->getName().c_str(),
				items[i]->currentBoard->getCurrentBloodNum(),items[i]->currentBoard->getDefaultBloodNum(),
				items[i]->posx,items[i]->posy,
				items[i]->hero->getCdTime(),items[i]->hero->getDefaultCdTime(),
				buffArray.c_str());
		}
#endif
	}
	return num;
}
//
float ChessBoardHelper::distance(ChessBoardItem* item1,ChessBoardItem* item2)
{
	int x_dis = 0;
	int y_dis = 0;
	// ͬ��һ������
	if (item1->currentBoard == item2->currentBoard )
	{
		x_dis = item1->posx - item2->posx;
		y_dis = item1->posy - item2->posy;
	}
	// ����ͬһ������
	else
	{
		x_dis = item1->posx - item2->posx;
		y_dis = item1->posy + item2->posy + 1;
	}

	// �п�Item����ôX�������0
	if (item1->hero->getXSize() >= 2 || item2->hero->getXSize() >= 2)
		x_dis = 0;

	float dis = pow(x_dis*x_dis+y_dis*y_dis,0.5f);

	// Ҫôȡ������Ҫôȡ0.5
	return (floor(dis)+ceil(dis))/2.0f;
}
//
int ChessBoardHelper::getIndex(int x_index,int y_index)
{
	return x_index + y_index * GRID_NUM_X;
}
//
void ChessBoardHelper::getGridPos(int index,int& out_x_index,int& out_y_index)
{
	out_x_index = index % GRID_NUM_X;
	out_y_index = index / GRID_NUM_X;
}
//
int ChessBoardHelper::convertToBoardGridY(const ChessBoard* board,const ChessBoardItem* item)
{
	CCAssert(board!=NULL,"");
	CCAssert(item!=NULL,"");
	// �ڱ������������棬��ô��ֱ�Ӿ���λ��
	// ����ڶԷ����������棬��ô���ǵ�ǰλ��ȡ����Ȼ���1
	return item->currentBoard == board ? item->posy : -item->posy-1;
}
int ChessBoardHelper::convertToOriginBoardGridY(const ChessBoardItem* item)
{
	return convertToBoardGridY(item->getOriginBoard(),item);
}
//
ChessBoardItem* ChessBoardHelper::getOneItemByUniqueId(unsigned int uniqueId)
{
	ChessBoardItem* itemA = boardA->getActorItem(uniqueId);
	ChessBoardItem* itemB = boardB->getActorItem(uniqueId);
	CCAssert(itemB == NULL || itemA == NULL,"������2���ط��ҵ�");
	return itemA == NULL?itemB:itemA;
}
// ����Item�����������uniqueId���õ�����AbstractItem����
AbstractItem* ChessBoardHelper::getAbstractItemByUniqueId(unsigned int uniqueId)
{
	ChessBoardItem* item = getOneItemByUniqueId(uniqueId);
	if (item)
		return item->hero;

	return NULL;
}
// ��ȡȫ���Ļ��ŵ�Item����
vector<AbstractItem*> ChessBoardHelper::getAllAlivedItems()
{
	vector<ChessBoardItem*> items = getAlivedItemsByOriginBoard(NULL);

	int size = items.size();
	vector<AbstractItem*> ret;
	for (int i=0;i<size;i++)
	{
		ret.push_back(items[i]->hero);
	}
	return ret;
}
// �Ƿ��Ƕ���
bool ChessBoardHelper::isTeammate(unsigned int uniqueId1,unsigned int uniqueId2)
{
	return getOneItemByUniqueId(uniqueId1)->isTeammate(getOneItemByUniqueId(uniqueId2));
	//return getOneItemByUniqueId(uniqueId1)->getOriginBoard() == getOneItemByUniqueId(uniqueId2)->getOriginBoard();
}

unsigned int ChessBoardHelper::getItemUniqueId(AbstractItem* item)
{
	vector<ChessBoardItem*> items = getAlivedItemsByOriginBoard(NULL);

	int size = items.size();
	vector<AbstractItem*> ret;
	for (int i=0;i<size;i++)
	{
		if (items[i]->hero == item)
		{
			return items[i]->getUniqueId();
		}
	}
	CCAssert(false,"û���ҵ�����Ӧ���ҵ���");
	return 0;
}

////
//ChessBoardItem* ChessBoardHelper::getOneNextFrontOfItem(const ChessBoardItem* item,bool includeWideItem /*= true*/)
//{
//	CCAssert(item->hero->getXSize() == 1,"ֻ�����ռ�õ����ӵ�Item");
//
//	ChessBoard* board;
//	int posX = item->posx;
//	int posY;
//	getNextYPosOfItem(item,board,posY,1);
//
//	if (posY > GRID_NUM_Y || posY < 0 || posX < 0 || posX > GRID_NUM_X)
//	{
//		return NULL;
//	}
//
//	return board->getActorItem(posX,posY,includeWideItem);
//}
//
void ChessBoardHelper::getNextYPosOfItem(const ChessBoardItem* item,ChessBoard*& out_pBoard,int& out_posy,int dis /* = 1 */ )
{
	// ���Լ�������
	if (item->getOriginBoard() == item->currentBoard)
	{
		int nextY = item->posy - dis;

		// С��0�ˣ��ͽ����˶�����˵�����
		if (nextY < 0)
		{
			ChessBoard* bieRenBoard = NULL;
			if (item->currentBoard == boardA)
			{
				bieRenBoard = boardB;
			}else
			{
				bieRenBoard = boardA;
			}
			int newY = -nextY;

			out_pBoard = bieRenBoard;
			out_posy = newY-1;
			return;
		}else
		{
			out_pBoard = item->currentBoard;
			out_posy = nextY;
			return;
		}
	}
	else
	{
		int nextY = item->posy + dis;

		out_pBoard = item->currentBoard;
		out_posy = nextY;
		return;
	}
}
// �е��ˣ��ж���
vector<ChessBoardItem*> ChessBoardHelper::getYItems(int xIndex)
{
	vector<ChessBoardItem*> board1 = boardA->getYItems(xIndex);
	vector<ChessBoardItem*> board2 = boardB->getYItems(xIndex);

	vector<ChessBoardItem*> ret;
	{
		int num1 = board1.size();
		for (int i=0;i<num1;i++)
		{
			ret.push_back(board1[i]);
		}
	}

	{
		int num2 = board2.size();
		for (int i=0;i<num2;i++)
		{
			ret.push_back(board2[i]);
		}
	}

	return ret;
}
// ��ú����ϵ�Items
vector<ChessBoardItem*> ChessBoardHelper::getXItems(ChessBoard* board, int yIndex)
{
	// �����ֱ����ͬһ�����̵������ˣ���ΪҪX����ģ�������һ�����̵�
	return board->getXItems(yIndex);
}

vector<ChessBoardItem*> ChessBoardHelper::getAlivedXItems(ChessBoard* board, int yIndex)
{
	vector<ChessBoardItem*> ret;
	vector<ChessBoardItem*> temp = getXItems(board,yIndex);

	for (int i=0,count=temp.size(); i<count; i++)
	{
		if (temp[i]->hero->getHp() > 0)
		{
			ret.push_back(temp[i]);
		}
	}

	return ret;
}

void ChessBoardHelper::removeItemFromVec(vector<ChessBoardItem*>& items,ChessBoardItem* toRemove)
{
	vector<ChessBoardItem*>::iterator it = items.begin();

	for (;it != items.end();it ++)
	{
		if ( (*it) == toRemove )
		{
			items.erase(it);
			break;
		}
	}
}








//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void ChessBoardItem::setOriginBoardCanUsedBlooldNum(short num)
{
	originBoard->setCanUsedBloodNum(num);
}