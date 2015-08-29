#include "FightPackage.h"

FightPackage* FightPackage::package = new FightPackage();

FightPackage::FightPackage(void)
{
	clearPackage();
}

FightPackage::~FightPackage(void)
{
}

FightPackage* FightPackage::getPakage()
{
	return package;
}

void FightPackage::put(EquipData equip)
{
	gettedEquip.push_back(equip);
}
void FightPackage::put(WuPin item)
{
	int wuPinNum = gettedWuPin.size();
	for (int i=0;i<wuPinNum;i++)
	{
		if (gettedWuPin[i].getType() == item.getType())
		{
			int count = gettedWuPin[i].getCount() + item.getCount();
			gettedWuPin[i].setCount(count);

			return;
		}
	}

	gettedWuPin.push_back(item);
}

vector<Equipment> FightPackage::getAllEquipments()
{
	return gettedEquip;
}
vector<WuPin> FightPackage::getAllWuping()
{
	return gettedWuPin;
}

vector<WuPin> FightPackage::getAllWupingExceptSilverAndGold()
{
	vector<WuPin> ret;
	int num = gettedWuPin.size();
	for (int i=0;i<num;i++)
	{
		if (gettedWuPin[i].getType() == ItemsType_Silver || gettedWuPin[i].getType() == ItemsType_Gold)
		{
			continue;
		}

		ret.push_back(gettedWuPin[i]);
	}

	return ret;
}

void FightPackage::clearPackage()
{
	gettedEquip.clear();
	gettedWuPin.clear();
}
