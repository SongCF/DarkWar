#include "SystemGiftManager.h"
#include "cocos2d.h"

using namespace cocos2d;

SystemGiftManager* SystemGiftManager::manager = new SystemGiftManager();

SystemGiftManager::SystemGiftManager(void)
{
}
SystemGiftManager::~SystemGiftManager(void)
{
}
SystemGiftManager* SystemGiftManager::getManager()
{
	return manager;
}

void SystemGiftManager::init(vector<systemGift> gift_arr)
{
	CCLog("SystemGiftManager::init ok!");
	gift_pool = gift_arr;
}

void SystemGiftManager::removeOneGiftById(int gift_id)
{
	for (vector<systemGift>::iterator it = gift_pool.begin();it != gift_pool.end();it++)
	{
		if (it->gift_id == gift_id)
		{

			gift_pool.erase(it);
			break;
		}
	}
}
bool SystemGiftManager::getOneGiftById(int gift_id,systemGift& outData)
{
	int num = gift_pool.size();

	for (int i=0;i<num;i++)
	{
		if (gift_pool[i].gift_id == gift_id)
		{
			outData = gift_pool[i];
			return true;
		}
	}

	return false;
}

vector<systemGift> SystemGiftManager::getAllGift()
{
	return gift_pool;
}