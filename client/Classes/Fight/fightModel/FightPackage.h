#pragma once
#include "WuPinManager.h"
#include "EquipmentManager.h"
/**
 *	主线任务的背包
 *	里面存放战斗中掉落的任何东西.
 *	存放的都是掉落出来的状态的数据。
 *	这里是单独的一份拷贝。
 */
class FightPackage
{
private:
	FightPackage(void);

	static FightPackage* package;

	// 不能存放WuPin* ,因为这里的数据仅仅只是缓存的一般的数据，不能公用，不能被其他地方修改
	vector<WuPin> gettedWuPin;
	// 存放的就是得到时的数据
	vector<Equipment> gettedEquip;
public:
	static FightPackage* getPakage();

	void put(EquipData equip);
	void put(WuPin item);

	vector<Equipment> getAllEquipments();
	vector<WuPin> getAllWuping();
	vector<WuPin> getAllWupingExceptSilverAndGold();

	void clearPackage();
	~FightPackage(void);
};

