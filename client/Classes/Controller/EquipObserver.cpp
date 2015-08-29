#include "EquipObserver.h"
#include "GlobalTickTrigle.h"
#include "ZhuangbeiScanner.h"
#include "EquipmentManager.h"

#define MSG_EQUIP_ADDED_OR_JIEFENGYIN_OP_END	"MSG_EQUIP_ADDED_OP_END"


EquipObserver* EquipObserver::mInstance = NULL;

EquipObserver::EquipObserver(void)
{
	trigleId = 0;
}

EquipObserver* EquipObserver::getObserver()
{
	if (mInstance == NULL)
	{
		mInstance = new EquipObserver();
	}
	return mInstance;
}
void EquipObserver::inittedEquipManager()
{
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_EquipObserver_EquipInited,NULL);
}

void EquipObserver::tryNotifyZhuangbeiScannerToGenReportNextFrame()
{
	if (trigleId == 0)
	{
		OneTrigle item = OneTrigle::makeTrigleDelay(MSG_EQUIP_ADDED_OR_JIEFENGYIN_OP_END,0,false);
		//item.type = Trigle_Delay;
		//item.loop = false;
		//item.time = 0;
		//item.notifyMsg = MSG_EQUIP_ADDED_OR_JIEFENGYIN_OP_END;
		trigleId = GlobalTickTrigle::getTrigle()->addOneTrigle(item);
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this,SEL_CallFuncO(&EquipObserver::trigleAfterEquipAdded),MSG_EQUIP_ADDED_OR_JIEFENGYIN_OP_END,NULL);
	}
}

void EquipObserver::addOneToEquipeManager(Equipment* equip)
{
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_EquipObserver_EquipAdded,NULL);
	// 添加到scanner
	ZhuangbeiScanner::getScanner()->addOneEquipment(equip);

	// 推一个通知，下一帧触发
	tryNotifyZhuangbeiScannerToGenReportNextFrame();
}

// 解封一个
void EquipObserver::jieFengOneEquip(Equipment* equip)
{
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_EquipObserver_EquipJieFengEd,NULL);
	// 添加到scanner
	ZhuangbeiScanner::getScanner()->addOneEquipment(equip);

	// 推一个通知，下一帧触发
	tryNotifyZhuangbeiScannerToGenReportNextFrame();
}

void EquipObserver::trigleAfterEquipAdded(CCObject* tickObjectData)
{
	GlobalTickTrigle::getTrigle()->removeTrigle(trigleId);
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,MSG_EQUIP_ADDED_OR_JIEFENGYIN_OP_END);

	ZhuangbeiScanner::getScanner()->setIgnoreHint(false);
}

void EquipObserver::removeOneFromEquipManager(const Equipment* equip)
{
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_EquipObserver_EquipRemoved,NULL);
	// 添加到scanner
	ZhuangbeiScanner::getScanner()->removeOneEquipment(equip);
}

void EquipObserver::equipmentModified(Equipment* equip)
{
	// 不再提示了
	//ZhuangbeiScanner::getScanner()->equipmentChanged(equip);
}


EquipObserver::~EquipObserver(void)
{
}
