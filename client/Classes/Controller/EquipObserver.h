#pragma once
#include <stdlib.h>
#include "cocos2d.h"
using namespace cocos2d;

class Equipment;

// EquipmentManager一旦初始化就触发
#define MSG_EquipObserver_EquipInited		"MSG_EquipObserver_EquipInited"
// EquipmentManager一旦有新加装备就触发
#define MSG_EquipObserver_EquipAdded		"MSG_EquipObserver_EquipAdded"
// EquipmentManager一旦有移除装备就触发
#define MSG_EquipObserver_EquipRemoved		"MSG_EquipObserver_EquipRemoved"
// 一旦解封一个就触发
#define MSG_EquipObserver_EquipJieFengEd	"MSG_EquipObserver_EquipJieFengEd"

// EquipmentManager一旦有新加装备，然后添加完毕之后就触发
// 也就是不是每一次add都触发，而只是最后一次add之后才触发
#define MSG_EquipObserver_EquipsAdded	"MSG_EquipObserver_EquipsAdded"

// 装备观察器
// 当装备库有改动，就会在下一帧通知
// 或者立即发出通知
class EquipObserver : public CCObject
{
	friend class EquipmentManager;
	friend class Equipment;
private:
	static EquipObserver* mInstance;

	unsigned int trigleId;

	//vector<EquipData> tempAddedEquips;
	//vector<EquipData> tempRemovedEquips;

	EquipObserver(void);

	void inittedEquipManager();
	// 添加之后调用
	void addOneToEquipeManager(Equipment* equip); 
	// 移除之前调用
	void removeOneFromEquipManager(const Equipment* equip);
	// 解封一个
	void jieFengOneEquip(Equipment* equip); 
	// 装备有改动了
	// 比如，强化之后，精炼之后，升级之后等
	void equipmentModified(Equipment* equip);

	void tryNotifyZhuangbeiScannerToGenReportNextFrame();

	void trigleAfterEquipAdded(CCObject* tickObjectData);

public:
	static EquipObserver* getObserver();
	~EquipObserver(void);
};

