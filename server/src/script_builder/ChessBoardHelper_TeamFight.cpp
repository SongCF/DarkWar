#include "ChessBoardHelper_TeamFight.h"
#include "FightControler.h"

void ChessBoard_TeamFight::removeItem(ChessBoardItem_TeamFight* item)
{
	int num = currentItems.size();
	vector<ChessBoardItem_TeamFight*>::iterator it;

	for (it = currentItems.begin();it!=currentItems.end();it++)
	{
		if ( (*it) == item)
		{
			item->currentIndex = -1;
			currentItems.erase(it);
			break;
		}
	}
}
void ChessBoard_TeamFight::removeItem(int index)
{
	ChessBoardItem_TeamFight* item = getActorItemByIndex(index);

	if (item != NULL)
	{
		removeItem(item);
	}
}
ChessBoard_TeamFight::ChessBoard_TeamFight()
{
}
// 检测是是否成功了
// 返回 >0 --> 则团队战胜了Boss
// 返回 <0 --> 则Boss战胜了团队
// 返回 =0 --> 则还没分出胜负
int ChessBoard_TeamFight::checkSuccess()
{
#if (USE_FIGHT_LOG == 1)
	CCLog("---------------------------ChessSuccess----------------------------------");
	ChessBoardItem_TeamFight * item = NULL;
	for (int i=0;i<4;i++)
	{
		item = getActorItemByUId(i+1);
		CCLog("item[u_id = %d],hp = [%d/%d],frezyNum = [%u/%u]",
			item->getUniqueId(),item->hero->getHp(),item->hero->getDefaultHp(),
			item->hero->getCdTime(),((TeamHero*)item->hero)->getTeamProperty().mDefaultFreezyNum);
	}
	item = getActorItemByUId(TEAM_BOSS_U_ID);
	CCLog("item[u_id = %d],hp = [%d/%d]",
		item->getUniqueId(),item->hero->getHp(),item->hero->getDefaultHp());
	CCLog("------------------------------------------------------------------");
#endif
	if (getActorItemByUId(TEAM_BOSS_U_ID)->hero->getHp() <= 0)
	{
		return 1;
	}
	return 0; // boss战只有步数用完了才会输
}

void ChessBoard_TeamFight::one_visible_step(vector<CdDiscr>& cd_time_arr,unsigned int action_item_Uid)
{
	// 只针对action的对象，更新时间
	int num = currentItems.size();
	for (int i=0;i<num;i++)
	{
		if (currentItems[i]->getUniqueId() != action_item_Uid)
		{
			continue;
		}

		int num = currentItems[i]->hero->getCdTime() - 1;
		currentItems[i]->hero->setCdTime(num>0?num:0);

		CdDiscr oneDisc;
		oneDisc.cdTime = currentItems[i]->hero->getCdTime();
		oneDisc.uniqueId = currentItems[i]->getUniqueId();
		cd_time_arr.push_back(oneDisc);
	}

	/*int num = currentItems.size();
	for (int i=0;i<num;i++)
	{
	int num = currentItems[i]->hero->getCdTime() - 1;
	currentItems[i]->hero->setCdTime(num>0?num:0);

	CdDiscr oneDisc;
	oneDisc.cdTime = currentItems[i]->hero->getCdTime();
	oneDisc.uniqueId = currentItems[i]->getUniqueId();
	cd_time_arr.push_back(oneDisc);
	}*/
}

void ChessBoard_TeamFight::setName(string name)
{
	idName = name;
}
string ChessBoard_TeamFight::getName() const
{
	return idName;
}
// 
void ChessBoard_TeamFight::addItem(ChessBoardItem_TeamFight* item,int index)
{
	CCAssert(item->currentIndex == -1,"");
	CCAssert(item->hero->getXSize() == 1,"目前只支持占一格的");

	if (isItemExist(item))
	{
		// 这个人已经添加了
		CCAssert(false,"");
		return;
	}

	if (!isPosOK(index))
	{
		CCAssert(false,"");
		return;
	}

	if (getActorItemByIndex(index) != NULL)
	{
		// 这个位置有人了
		CCAssert(false,"");
		return;
	}

	item->currentIndex = index;
	currentItems.push_back(item);
}
// 
bool ChessBoard_TeamFight::isItemExist(ChessBoardItem_TeamFight* item)
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

bool ChessBoard_TeamFight::isPosOK(int index)
{
	return index >=0 && index <= 4;
}

// 得到某个位置上的Item
ChessBoardItem_TeamFight* ChessBoard_TeamFight::getActorItemByIndex(int index)
{
	if (!isPosOK(index))
	{
		return NULL;
	}

	int num = currentItems.size();
	for (int i=0;i<num;i++)
	{
		if (currentItems[i]->currentIndex == index)
		{
			return currentItems[i];
		}
	}
	return NULL;
}

ChessBoardItem_TeamFight* ChessBoard_TeamFight::getActorItemByUId(unsigned int uniqueId)
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

// 根据Item在棋盘里面的uniqueId，得到他的AbstractItem数据
AbstractItem* ChessBoard_TeamFight::getAbstractItemByUniqueId(unsigned int uniqueId)
{
	ChessBoardItem_TeamFight* item = getActorItemByUId(uniqueId);
	if(item) return item->hero;

	return NULL;
}

// 获取全部的活着的Item数据
vector<AbstractItem*> ChessBoard_TeamFight::getAllAlivedItems()
{
	vector<AbstractItem*> ret;

	int size = currentItems.size();
	for (int i=0;i<size;i++)
	{
		if (currentItems[i]->hero->getHp() > 0)
		{
			ret.push_back(currentItems[i]->hero);
		}
	}

	return ret;
}

vector<ChessBoardItem_TeamFight*> ChessBoard_TeamFight::getAllAlivedEnemyItems(ChessBoardItem_TeamFight* item)
{
	vector<ChessBoardItem_TeamFight*> ret;

	int size = currentItems.size();
	for (int i=0;i<size;i++)
	{
		if (isTeammate(item->getUniqueId(),currentItems[i]->getUniqueId()))
		{
			continue;
		}
		if (currentItems[i]->hero->getHp() > 0)
		{
			ret.push_back(currentItems[i]);
		}
	}

	return ret;
}

vector<ChessBoardItem_TeamFight*> ChessBoard_TeamFight::getAllAlivedTeamItems(ChessBoardItem_TeamFight* item)
{
	vector<ChessBoardItem_TeamFight*> ret;

	int size = currentItems.size();
	for (int i=0;i<size;i++)
	{
		if (!isTeammate(item->getUniqueId(),currentItems[i]->getUniqueId()))
		{
			continue;
		}
		if (currentItems[i]->hero->getHp() > 0)
		{
			ret.push_back(currentItems[i]);
		}
	}

	return ret;
}

// 是否是队友
bool ChessBoard_TeamFight::isTeammate(unsigned int uniqueId1,unsigned int uniqueId2)
{
	// 只有5号是敌人，而且只有这一个敌人
	if (uniqueId1 == TEAM_BOSS_U_ID && uniqueId2 != TEAM_BOSS_U_ID)
	{
		return false;
	}
	if (uniqueId2 == TEAM_BOSS_U_ID && uniqueId1 != TEAM_BOSS_U_ID)
	{
		return false;
	}
	return true;
}

unsigned int ChessBoard_TeamFight::getItemUniqueId(AbstractItem* item)
{
	int size = currentItems.size();
	for (int i=0;i<size;i++)
	{
		if (currentItems[i]->hero == item)
		{
			return currentItems[i]->getUniqueId();
		}
	}

	CCAssert(false,"没有找到，是个BUG");
	return 0;
}


float ChessBoard_TeamFight::distance(ChessBoardItem_TeamFight* a,ChessBoardItem_TeamFight* b)
{
	// 对角线的距离是2.0f
	if ( (a->currentIndex == 0 && b->currentIndex == 3) || (a->currentIndex == 3 && b->currentIndex == 1))
	{
		return 2.0f;
	}
	if ( (a->currentIndex == 1 && b->currentIndex == 2) || (a->currentIndex == 2 && b->currentIndex == 1) )
	{
		return 2.0f;
	}

	return 1.0f;
}
