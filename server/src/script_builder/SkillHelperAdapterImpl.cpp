#include "SkillHelperAdapterImpl.h"
#include "FightControler.h"
#include "GameBuffProc.h"

SkillHelperAdapterImpl::SkillHelperAdapterImpl(ChessBoardHelper* chessHelper,FightControler* controler)
{
	this->mChessHelper = chessHelper;
	this->mFightControler = controler;
}

ChessBoardItemBase* SkillHelperAdapterImpl::getDefaultTargetBeFight(ChessBoardItemBase*fightor,float distance /*= -1*/)
{
	ChessBoardItem* _fightor = dynamic_cast<ChessBoardItem*>(fightor);
	CCAssert(_fightor!=NULL,"���Ŷ�ս��������ChessBoardItem");
	return mFightControler->prepareChessBoardBeFightedItem(_fightor,distance);
}
int SkillHelperAdapterImpl::genOneBuffEffectId()
{
	return mFightControler->mBuffProc->genOneBuffId();
}
vector<ChessBoardItemBase*> SkillHelperAdapterImpl::getAlivedTeamItems(ChessBoardItemBase* item)
{
	ChessBoardItem* _item = dynamic_cast<ChessBoardItem*>(item);
	CCAssert(_item!=NULL,"���Ŷ�ս��������ChessBoardItem");
	vector<ChessBoardItem*> allItems = mChessHelper->getAlivedItemsByOriginBoard(_item->getOriginBoard());

	vector<ChessBoardItemBase*> ret;
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		ret.push_back(allItems[i]);
	}

	return ret;
}
vector<ChessBoardItemBase*> SkillHelperAdapterImpl::getAlivedEnemyItems(ChessBoardItemBase* item)
{
	ChessBoardItem* _item = dynamic_cast<ChessBoardItem*>(item);
	CCAssert(_item!=NULL,"���Ŷ�ս��������ChessBoardItem");
	
	ChessBoard* opBoard = mChessHelper->getBoardA() == _item->getOriginBoard() ? mChessHelper->getBoardB() : mChessHelper->getBoardA();
	vector<ChessBoardItem*> allItems = mChessHelper->getAlivedItemsByOriginBoard(opBoard);

	vector<ChessBoardItemBase*> ret;
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		ret.push_back(allItems[i]);
	}

	return ret;
}
ChessBoardItemBase* SkillHelperAdapterImpl::getWeaklessAndDyingItem(vector<ChessBoardItemBase*> itemSet,ChessBoardItemBase* fightor)
{
	ChessBoardItem* _fightor = dynamic_cast<ChessBoardItem*>(fightor);
	CCAssert(_fightor!=NULL,"���Ŷ�ս��������ChessBoardItem");

	vector<ChessBoardItem*> tempVec;
	int num = itemSet.size();
	for (int i = 0;i<num;i++)
	{
		CCAssert(dynamic_cast<ChessBoardItem*>(itemSet[i]),"���Ŷ�ս��������ChessBoardItem");
		tempVec.push_back((ChessBoardItem*)itemSet[i]);
	}
	return mFightControler->getWeaklessAndDyingItem(tempVec,_fightor);
}
vector<ChessBoardItemBase*> SkillHelperAdapterImpl::getFrontRowAlivedEnemyItem(ChessBoardItemBase* fightor)
{
	vector<ChessBoardItemBase*> ret;

	ChessBoardItem* _fightor = dynamic_cast<ChessBoardItem*>(fightor);
	CCAssert(_fightor!=NULL,"���Ŷ�ս��������ChessBoardItem");

	// �Ź������ȿ���0�ţ��ٿ��������
	ChessBoard* targetBoard = mChessHelper->getBoardA() == _fightor->getOriginBoard() ? mChessHelper->getBoardB() : mChessHelper->getBoardA();
	vector<ChessBoardItem*> allItems = mChessHelper->getAlivedXItems(targetBoard,0);
	for (int i=0,count=allItems.size(); i<count; i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		ret.push_back(allItems[i]);
	}

	return ret;
}
vector<ChessBoardItemBase*> SkillHelperAdapterImpl::getBackRowAlivedEnemyItem(ChessBoardItemBase* fightor)
{
	vector<ChessBoardItemBase*> ret;

	ChessBoardItem* _fightor = dynamic_cast<ChessBoardItem*>(fightor);
	CCAssert(_fightor!=NULL,"���Ŷ�ս��������ChessBoardItem");

	// �Ź������ȿ���0�ţ��ٿ��������
	ChessBoard* targetBoard = mChessHelper->getBoardA() == _fightor->getOriginBoard() ? mChessHelper->getBoardB() : mChessHelper->getBoardA();
	vector<ChessBoardItem*> allItems = mChessHelper->getAlivedXItems(targetBoard,1);
	for (int i=0,count=allItems.size(); i<count; i++)
	{
		CCAssert(allItems[i]->hero->getHp() > 0,"�쳣");
		ret.push_back(allItems[i]);
	}

	return ret;
}


float SkillHelperAdapterImpl::distance(ChessBoardItemBase* item1,ChessBoardItemBase* item2)
{
	ChessBoardItem* _item1 = dynamic_cast<ChessBoardItem*>(item1);
	ChessBoardItem* _item2 = dynamic_cast<ChessBoardItem*>(item2);
	CCAssert(_item1!=NULL,"���Ŷ�ս��������ChessBoardItem");
	CCAssert(_item2!=NULL,"���Ŷ�ս��������ChessBoardItem");

	return mChessHelper->distance(_item1,_item2);
}
bool SkillHelperAdapterImpl::isInCol(ChessBoardItemBase* item,ChessBoardItemBase* test)
{
	ChessBoardItem* _item = dynamic_cast<ChessBoardItem*>(item);
	ChessBoardItem* _test = dynamic_cast<ChessBoardItem*>(test);
	CCAssert(_item!=NULL,"���Ŷ�ս��������ChessBoardItem");
	CCAssert(_test!=NULL,"���Ŷ�ս��������ChessBoardItem");

	// ���Boss��˭����һ��
	if (item->hero->getXSize() > 1 || test->hero->getXSize() > 1)
	{
		return true;
	}

	return _item->posx == _test->posx;
}

bool SkillHelperAdapterImpl::isInRow(ChessBoardItemBase* item,ChessBoardItemBase* test)
{
	ChessBoardItem* _item = dynamic_cast<ChessBoardItem*>(item);
	ChessBoardItem* _test = dynamic_cast<ChessBoardItem*>(test);
	CCAssert(_item!=NULL,"���Ŷ�ս��������ChessBoardItem");
	CCAssert(_test!=NULL,"���Ŷ�ս��������ChessBoardItem");


	if (_item->currentBoard == _test->currentBoard && _item->posy == _test->posy)
	{
		return true;
	}
	return false;
}





//////////////////////////////////////////////////////////////////////////
// �������Ŷӵ�������
//////////////////////////////////////////////////////////////////////////
SkillHelperTFAdapterImpl::SkillHelperTFAdapterImpl(ChessBoard_TeamFight* chessHelper,TeamFightControler* controler)
{
	this->mChessHelper = chessHelper;
	this->mFightControler = controler;
}

ChessBoardItemBase* SkillHelperTFAdapterImpl::getDefaultTargetBeFight(ChessBoardItemBase*fightor,float distance /*= -1*/)
{
	return  fightor->getUniqueId() == TEAM_BOSS_U_ID?
		mFightControler->prepareBeFightedByBoss():
		mChessHelper->getActorItemByUId(TEAM_BOSS_U_ID);
}
int SkillHelperTFAdapterImpl::genOneBuffEffectId()
{
	return mFightControler->mBuffProc->genOneBuffId();
}
vector<ChessBoardItemBase*> SkillHelperTFAdapterImpl::getAlivedTeamItems(ChessBoardItemBase* item)
{
	ChessBoardItem_TeamFight* _item = dynamic_cast<ChessBoardItem_TeamFight*>(item);
	CCAssert(_item!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");

	vector<ChessBoardItemBase*> ret;

	vector<ChessBoardItem_TeamFight*> allItems = mChessHelper->getAllAlivedTeamItems(_item);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		ret.push_back(allItems[i]);
	}

	return ret;

}
vector<ChessBoardItemBase*> SkillHelperTFAdapterImpl::getAlivedEnemyItems(ChessBoardItemBase* item)
{
	ChessBoardItem_TeamFight* _item = dynamic_cast<ChessBoardItem_TeamFight*>(item);
	CCAssert(_item!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");

	vector<ChessBoardItemBase*> ret;

	vector<ChessBoardItem_TeamFight*> allItems = mChessHelper->getAllAlivedEnemyItems(_item);
	int num = allItems.size();
	for (int i=0;i<num;i++)
	{
		ret.push_back(allItems[i]);
	}

	return ret;
}
ChessBoardItemBase* SkillHelperTFAdapterImpl::getWeaklessAndDyingItem(vector<ChessBoardItemBase*> itemSet,ChessBoardItemBase* fightor)
{
	ChessBoardItem_TeamFight* _fightor = dynamic_cast<ChessBoardItem_TeamFight*>(fightor);
	CCAssert(_fightor!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");

	vector<ChessBoardItem_TeamFight*> tempVec;
	int num = itemSet.size();
	for (int i = 0;i<num;i++)
	{
		CCAssert(dynamic_cast<ChessBoardItem_TeamFight*>(itemSet[i]),"�Ŷ�ս��������ChessBoardItem_TeamFight");
		tempVec.push_back((ChessBoardItem_TeamFight*)itemSet[i]);
	}

	return mFightControler->getWeaklessAndDyingItem(tempVec,_fightor);
}

vector<ChessBoardItemBase*> SkillHelperTFAdapterImpl::getFrontRowAlivedEnemyItem(ChessBoardItemBase* fightor)
{
	ChessBoardItem_TeamFight* _fightor = dynamic_cast<ChessBoardItem_TeamFight*>(fightor);
	CCAssert(_fightor!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");

	CCAssert(false,"Boss���ֻ��ǰ�ţ���ô�ͱ����ˣ�ÿ�ζ���ǰ���������Ź��������Ŷ�boss�Ͳ����ṩ����ӿ�");


	vector<ChessBoardItemBase*> ret;
	return ret;
}
vector<ChessBoardItemBase*> SkillHelperTFAdapterImpl::getBackRowAlivedEnemyItem(ChessBoardItemBase* fightor)
{
	ChessBoardItem_TeamFight* _fightor = dynamic_cast<ChessBoardItem_TeamFight*>(fightor);
	CCAssert(_fightor!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");

	CCAssert(false,"Boss���ֻ��ǰ�ţ���ô�ͱ����ˣ�ÿ�ζ���ǰ���������Ź��������Ŷ�boss�Ͳ����ṩ����ӿ�");

	vector<ChessBoardItemBase*> ret;
	return ret;
}

float SkillHelperTFAdapterImpl::distance(ChessBoardItemBase* item1,ChessBoardItemBase* item2)
{
	ChessBoardItem_TeamFight* _item1 = dynamic_cast<ChessBoardItem_TeamFight*>(item1);
	ChessBoardItem_TeamFight* _item2 = dynamic_cast<ChessBoardItem_TeamFight*>(item2);
	CCAssert(_item1!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");
	CCAssert(_item2!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");

	return mChessHelper->distance(_item1,_item2);
}
bool SkillHelperTFAdapterImpl::isInCol(ChessBoardItemBase* item,ChessBoardItemBase* test)
{
	ChessBoardItem_TeamFight* _item = dynamic_cast<ChessBoardItem_TeamFight*>(item);
	ChessBoardItem_TeamFight* _test = dynamic_cast<ChessBoardItem_TeamFight*>(test);
	CCAssert(_item!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");
	CCAssert(_test!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");

	if ( (_item->currentIndex == 0 && _test->currentIndex == 2) || (_item->currentIndex == 2 && _test->currentIndex == 0))
	{
		return true;
	}
	if ( (_item->currentIndex == 1 && _test->currentIndex == 3) || (_item->currentIndex == 3 && _test->currentIndex == 1) )
	{
		return true;
	}

	return false;
}
bool SkillHelperTFAdapterImpl::isInRow(ChessBoardItemBase* item,ChessBoardItemBase* test)
{
	ChessBoardItem_TeamFight* _item = dynamic_cast<ChessBoardItem_TeamFight*>(item);
	ChessBoardItem_TeamFight* _test = dynamic_cast<ChessBoardItem_TeamFight*>(test);
	CCAssert(_item!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");
	CCAssert(_test!=NULL,"�Ŷ�ս��������ChessBoardItem_TeamFight");

	if ( (_item->currentIndex == 0 && _test->currentIndex == 1) || (_item->currentIndex == 1 && _test->currentIndex == 0))
	{
		return true;
	}
	if ( (_item->currentIndex == 2 && _test->currentIndex == 3) || (_item->currentIndex == 3 && _test->currentIndex == 2) )
	{
		return true;
	}

	return false;
}