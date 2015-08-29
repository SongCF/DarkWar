#include "TanSuoManager.h"
#include "cocos2d.h"
using namespace cocos2d;

TanSuoManager* TanSuoManager::mInstance = NULL;

TanSuoManager::TanSuoManager(void)
{
}

TanSuoManager* TanSuoManager::getManager()
{
	if (mInstance == NULL)
	{
		mInstance = new TanSuoManager();
	}
	return mInstance;
}

void TanSuoManager::setTanSuoData(TanSuoData data)
{
	mTansuoData = data;
}

void TanSuoManager::subCanTanSuoTime(unsigned int toSub)
{
	if (mTansuoData.currentTimeCanUse < toSub)
	{
		mTansuoData.currentTimeCanUse = 0;
		CCAssert(false,"异常");
		return;
	}
	mTansuoData.currentTimeCanUse -= toSub;
}
//
void TanSuoManager::setCanTanSuoTime(unsigned int time)
{
	mTansuoData.currentTimeCanUse = time;
}
//
void TanSuoManager::addDistoriedNum(unsigned int toAdd)
{
	mTansuoData.destoriedNum += toAdd;
}
//
void TanSuoManager::setDistoriedNum(unsigned int num)
{
	mTansuoData.destoriedNum = num;
}
void TanSuoManager::addUnlockedSealNum(unsigned int num)
{
	mTansuoData.unlocked_seal += num;
	if (mTansuoData.unlocked_seal == 2) mTansuoData.unlocked_seal = 0;
	else if (mTansuoData.unlocked_seal > 2) CCAssert(false, "");
}
void TanSuoManager::setLastModifyTansuoTime(unsigned int time)
{
	mTansuoData.last_modify_timestamp = time;
}



unsigned int TanSuoManager::getCurrentTimeLeft()
{
	return mTansuoData.currentTimeCanUse;
}
unsigned int TanSuoManager::getTotalTanSuoTime()
{
	return mTansuoData.defaultTimeCanUse;
}
unsigned int TanSuoManager::getDistoriedNum()
{
	return mTansuoData.destoriedNum;
}
bool TanSuoManager::canJieFeng()
{
//	return getDistoriedNum() >= 6;
	//20140627 换成每破坏3个封印就可以解封了
	if (getDistoriedNum() == 3 && mTansuoData.unlocked_seal == 0) return true;
	else if (getDistoriedNum() == 6 && mTansuoData.unlocked_seal < 2) return true;
	else return false;
}

unsigned int TanSuoManager::getLastRefreshTime()
{
	return mTansuoData.last_modify_timestamp;
}

TanSuoManager::~TanSuoManager(void)
{
}
