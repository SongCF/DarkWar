#pragma once

#include <vector>
#include "protocol.h"
using namespace std;

// 管理市场的
class MarketManager
{
private:
	// 市场上的东西
	vector<MarketItem> itemVec;
	// 竞技场卖的东西
	vector<ArenaMarketItem> itemVecInArena;

	static MarketManager* manager;

	MarketManager(void);

	void clearAll();
public:
	static MarketManager* getManager();

	// 市场上的
	void init(vector<MarketItem> itemVec);
	const MarketItem* getItemById(unsigned int id);
	MarketItem getItemAtIndex(int index);
	int itemCount();
	vector<MarketItem> getAllItems();
	
	// 竞技场专用的
	void initInArena(vector<ArenaMarketItem> itemVecInArena);
	vector<ArenaMarketItem> getAllItemsInArena();
	int itemCountInArena();
	ArenaMarketItem getItemInArenaAtIndex(int index);

	~MarketManager(void);
};

