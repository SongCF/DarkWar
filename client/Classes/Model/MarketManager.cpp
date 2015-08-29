#include "MarketManager.h"

MarketManager* MarketManager::manager = new MarketManager();

MarketManager::MarketManager(void)
{
	clearAll();
}

void MarketManager::init(vector<MarketItem> itemVec)
{
	this->itemVec = itemVec;
	CCLog("MarketManager::init ---> over");
}
MarketManager* MarketManager::getManager()
{
	return manager;
}

int MarketManager::itemCount()
{
	return itemVec.size();
}
vector<MarketItem> MarketManager::getAllItems()
{
	return itemVec;
}
MarketItem MarketManager::getItemAtIndex(int index)
{
	if (index < 0 || index > itemCount() - 1)
	{
		//MarketItem item;
		CCAssert(false,"");
		//return item;

		index = 0;
	}
	return itemVec[index];
}
const MarketItem* MarketManager::getItemById(unsigned int id)
{
	for (int i=0,count=itemVec.size(); i<count; ++i)
	{
		if (itemVec.at(i).id == id) return &itemVec[i];
	}
	return 0;
}

void MarketManager::clearAll()
{
	itemVec.clear();
	itemVecInArena.clear();
}



void MarketManager::initInArena(vector<ArenaMarketItem> itemVecInArena)
{
	this->itemVecInArena = itemVecInArena;
	CCLog("MarketManager::init ---> over");
}
vector<ArenaMarketItem> MarketManager::getAllItemsInArena()
{
	return itemVecInArena;
}
int MarketManager::itemCountInArena()
{
	return itemVecInArena.size();
}
ArenaMarketItem MarketManager::getItemInArenaAtIndex(int index)
{
	return itemVecInArena[index];
}



MarketManager::~MarketManager(void)
{
}
