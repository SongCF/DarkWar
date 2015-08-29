#include "XueZhanManager.h"
#include "MyselfManager.h"
#include "LFStrings.h"
#include "ServerTime.h"
#include "CS_Common.h"
#include "TaskManager.h"
XueZhanManager* XueZhanManager::mInstance = NULL;

XueZhanManager::XueZhanManager(void)
{
	lastGettedSilverNum = 0;
	_giftBag.id = 0;
	_giftBag.count = 0;
	mCanOpenBox = false;
	isDiffFixed = false;
}

XueZhanManager* XueZhanManager::getManager()
{
	if (mInstance == NULL)
	{
		mInstance = new XueZhanManager();
	}
	return mInstance;
}

SelfChlgDiffType XueZhanManager::convertToSelfChlgDiffType(Difficult diff)
{
	SelfChlgDiffType ret = SelfChlgDiffType_NULL;
	switch (diff)
	{
	case Difficult_PuTong:
		ret = SelfChlgDiffType_Common;
		break;
	case Difficult_KunNan:
		ret = SelfChlgDiffType_Difficult;
		break;
	case Difficult_DiYu:
		ret = SelfChlgDiffType_Hell;
		break;
	case Difficult_EMeng:
		ret = SelfChlgDiffType_Nightmare;
		break;
	default:
		CCAssert(false,"");
		break;
	}
	return ret;
}
Difficult XueZhanManager::convertToDifficult(SelfChlgDiffType diff)
{
	Difficult ret = Difficult_PuTong;
	switch (diff)
	{
	case SelfChlgDiffType_Common:
		ret = Difficult_PuTong;
		break;
	case SelfChlgDiffType_Difficult:
		ret = Difficult_KunNan;
		break;
	case SelfChlgDiffType_Hell:
		ret = Difficult_DiYu;
		break;
	case SelfChlgDiffType_Nightmare:
		ret = Difficult_EMeng;
		break;
	default:
		CCAssert(false,"");
		break;
	}
	return ret;
}

void XueZhanManager::refreshData(SPCmd_QuerySelfChallengeInfoRsp data)
{
	xuezhanData.open_time = data.open_time;
	xuezhanData.close_time = data.close_time;
	xuezhanData.currentChlgNumLeft = data.info.times;
	xuezhanData.currentPowerNum = data.info.energy;
	xuezhanData.currentPassedNum = data.info.checkpoints;
	xuezhanData.currentState = data.info.b_on_the_way?State_In:State_Out;
	if (data.info.difficulty == SelfChlgDiffType_NULL)
	{
		isDiffFixed = false;
	}
	else
	{
		xuezhanData.difficult = convertToDifficult((SelfChlgDiffType)data.info.difficulty);
		isDiffFixed = true;
	}

	xuezhanData.currentHpAddPercent = data.info.hp_gain;
	xuezhanData.currentGongJiAddPercent = data.info.attack_gain;
	xuezhanData.currentHuJiaAddPercent = data.info.defense_gain;
}

void XueZhanManager::addPassedNum(unsigned int toAdd)
{
	xuezhanData.currentPassedNum += toAdd;
}
void XueZhanManager::addPowerNum(unsigned int toAdd)
{
	xuezhanData.currentPowerNum += toAdd;
}
void XueZhanManager::setPowerNum(unsigned int num)
{
	xuezhanData.currentPowerNum = num;
}
void XueZhanManager::subPowerNum(unsigned int toSub)
{
	if (toSub > xuezhanData.currentPowerNum)
	{
		CCAssert(false,"");
		xuezhanData.currentPowerNum = 0;
	}
	else
	{
		xuezhanData.currentPowerNum -= toSub;
	}
}

void XueZhanManager::subCanChlgNum(unsigned int toSub)
{
	if (toSub > xuezhanData.currentChlgNumLeft)
	{
		CCAssert(false,"");
		toSub = xuezhanData.currentChlgNumLeft;
	}
	xuezhanData.currentChlgNumLeft -= toSub;
}
void XueZhanManager::setCanChlgNum(unsigned int num)
{
	xuezhanData.currentChlgNumLeft = num;
}

void XueZhanManager::setCurrentHpAddPercent(unsigned int percent)
{
	xuezhanData.currentHpAddPercent = percent;
}
void XueZhanManager::setCurrentGongJiAddPercent(unsigned int percent)
{
	xuezhanData.currentGongJiAddPercent = percent;
}
void XueZhanManager::setCurrentHuJiaAddPercent(unsigned int percent)
{
	xuezhanData.currentHuJiaAddPercent = percent;
}
void XueZhanManager::addCurrentHpAddPercent(int percentToAdd)
{
	xuezhanData.currentHuJiaAddPercent += percentToAdd;
}
void XueZhanManager::addCurrentGongJiAddPercent(int percentToAdd)
{
	xuezhanData.currentHuJiaAddPercent += percentToAdd;
}
void XueZhanManager::addCurrentHuJiaAddPercent(int percentToAdd)
{
	xuezhanData.currentHuJiaAddPercent += percentToAdd;
}

void XueZhanManager::setLastGettedSilverNum(unsigned int silverNum)
{
	lastGettedSilverNum = silverNum;
}
void XueZhanManager::setLastGettedGiftBag(GiftBag& gift)
{
	_giftBag = gift;
}
GiftBag XueZhanManager::getLastGettedGiftBag()
{
	return _giftBag;
}
void XueZhanManager::setCanOpenBox(bool canOpen)
{
	mCanOpenBox = canOpen;
	if (canOpen)
	{
		CCAssert(xuezhanData.currentState == State_In,"");
	}
}

// 我是否能参加血战
// 等级大于11就可以
XueZhanManager::ErrorCode XueZhanManager::canIJoinXueZhan()
{
	//改为通关第一章
	bool isLevelOk = (MyselfManager::getManager()->getMyZhuJueData()->getLevel() >= getAllDiff_Level_Map()[Difficult_PuTong].min_level);
	if (!isLevelOk)
	{
		return EC_LevelNotEnough;
	}
	//
	int lastTaskId = TaskManager::getManager()->getLastActivedMainTaskId();
	int taskIdState = TaskManager::getManager()->getLastActivedMainTaskStatus();
	bool isTaskOk =  lastTaskId > 10;
	if (!isTaskOk)
	{
		return EC_TaskNotEnough;
	}

	unsigned int curTime = ServerTime::getTime();
	bool opened = curTime>=xuezhanData.open_time && curTime <=xuezhanData.close_time;
	if (!opened)
	{
		return EC_NotOpenThisTime;
	}

	if (getCanChlgNum() <= 0)
	{
		return EC_NoChlgTime;
	}

	return EC_OK;
}

unsigned int XueZhanManager::getPassedNum()
{
	return xuezhanData.currentPassedNum;
}

unsigned int XueZhanManager::powerWillAddIfChlg()
{
	if (getDefaultChlgNum() == getCanChlgNum())
	{
		return 0;
	}

	return 60;
}
unsigned int XueZhanManager::getWillGetPowerAfterWin()
{
	return PASS_SELFCHLG_CHECKPOINT_ENERGY;
}

unsigned int XueZhanManager::getCurrentPowerNum()
{
	return xuezhanData.currentPowerNum;
}

unsigned int XueZhanManager::getCanChlgNum()
{
	return xuezhanData.currentChlgNumLeft;
}
// 获取默认的挑战次数，就是每天默认几次。3次
unsigned int XueZhanManager::getDefaultChlgNum()
{
	return 3;
}

void XueZhanManager::enter()
{
	// 没用了
	// 不能在enter的时候固定，因为enter之后不见得有打！所以需要在打赢了之后固定

	//// 更新
	//if (isDiffFixed)
	//	return;

	//int heroLv = MyselfManager::getManager()->getMyZhuJueData()->getLevel();
	//xuezhanData.difficult = convertToDifficult((SelfChlgDiffType)CS::getSelfChlgDifficulty(heroLv));
	//isDiffFixed = true;
}

void XueZhanManager::setCurrentDiffFixed(bool fixed)
{
	// 本来已经是固定的，就不能再算了
	if (!isDiffFixed && fixed)
	{
		isDiffFixed = true;
		// 计算难度
		int heroLv = MyselfManager::getManager()->getMyZhuJueData()->getLevel();
		xuezhanData.difficult = convertToDifficult((SelfChlgDiffType)CS::getSelfChlgDifficulty(heroLv));
	}
	
	isDiffFixed = fixed;
}
Difficult XueZhanManager::getCurrentDifficult()
{
	// 没固定就临时计算
	if (!isDiffFixed)
	{
		int heroLv = MyselfManager::getManager()->getMyZhuJueData()->getLevel();
		xuezhanData.difficult = convertToDifficult((SelfChlgDiffType)CS::getSelfChlgDifficulty(heroLv));
	}

	return xuezhanData.difficult;
}

XueZhanState XueZhanManager::getCurrentState()
{
	return xuezhanData.currentState;
}

void XueZhanManager::setCurrentState(XueZhanState state)
{
	xuezhanData.currentState = state; 

// 	// 踢出来了，那么isStateOk就要为false
// 	// 这样下次进去的时候就需要重新计算难度
// 	if (state == State_Out)
// 	{
// 		isDiffFixed = false;
// 	}
}
void XueZhanManager::setCurrentPassedNum(unsigned int num)
{
	xuezhanData.currentPassedNum = num;
}

// 刷新成一个新的挑战
// 会减去一次挑战次数，并置状态为State_Out
void XueZhanManager::makeNewChlgWhenFaild()
{
	lastGettedSilverNum = 0;
	_giftBag.id = 0;
	_giftBag.count = 0;
	xuezhanData.currentState = State_Out;

	//xuezhanData.currentChlgNumLeft = xuezhanData.currentChlgNumLeft;		// 挑战次数不改，而是在外面点击闯关之后才扣除
	//xuezhanData.currentPassedNum = xuezhanData.currentPassedNum;			// 不清空
	//xuezhanData.currentPowerNum = xuezhanData.currentPowerNum;				// 输了能量不清空
	//xuezhanData.currentHpAddPercent = xuezhanData.currentHpAddPercent;		// 不清空
	//xuezhanData.currentGongJiAddPercent = xuezhanData.currentHpAddPercent;	// 不清空
	//xuezhanData.currentGongJiAddPercent = xuezhanData.currentHpAddPercent;	// 不清空
}

// 获取所有的难度等级映射
// 用于难度描述
vector<XueZhanManager::Diff_Level_Map> XueZhanManager::getAllDiff_Level_Map()
{
	Diff_Level_Map putong;
	putong.diff = Difficult_PuTong;
	//通关第一章来判断
//	putong.min_level = 11;
	putong.min_level = 1;
	putong.max_level = 16;

	Diff_Level_Map kunnan;
	kunnan.diff = Difficult_KunNan;
	kunnan.min_level = 17;
	kunnan.max_level = 21;

	Diff_Level_Map diyu;
	diyu.diff = Difficult_DiYu;
	diyu.min_level = 22;
	diyu.max_level = 27;

	Diff_Level_Map emeng;
	emeng.diff = Difficult_EMeng;
	emeng.min_level = 28;
	emeng.max_level = HERO_MAX_LEVEL;


	vector<Diff_Level_Map> ret;
	ret.push_back(putong);
	ret.push_back(kunnan);
	ret.push_back(diyu);
	ret.push_back(emeng);
	return ret;
}

BaseSprite* XueZhanManager::createCurrentBackgroud()
{
	int switch_int = xuezhanData.currentPassedNum%20 + 1;

	string picFileName = "";
	switch (switch_int)
	{
	case 1:
		picFileName = "zhandou_beijing_huangshimuxue.png";
		break;
	case 2:
		picFileName = "zhandou_beijing_muyuan.png";
		break;
	case 3:
		picFileName = "zhandou_beijing_dixiajiucheng.png";
		break;
	case 4:
		picFileName = "zhandou_beijing_xiejiaoneibu.png";
		break;
	case 5:
		picFileName = "zhandou_beijing_zhuorezhidi.png";
		break;
	default:
		CCLog("Error : %s --> xuezhanData.currentPassedNum = %u",__FUNCTION__,xuezhanData.currentPassedNum);
		CCAssert(false,"");
		picFileName = "zhandou_beijing_zhuorezhidi.png";
		break;
	}

	return BaseSprite::create(picFileName);
}
string XueZhanManager::getCurrentBackgroundDescri()
{
	int switch_int = (xuezhanData.currentPassedNum + 20)/20;
	char temp[64];
	sprintf(temp,"xuezhan_scence_discr_%d",switch_int);
	return LFStrings::getValue(temp);
}


// 获取当前的血量/攻击/护甲的百分比加成
unsigned int XueZhanManager::getCurrentHpAddPercent()
{
	return xuezhanData.currentHpAddPercent;
}
unsigned int XueZhanManager::getCurrentGongJiAddPercent()
{
	return xuezhanData.currentGongJiAddPercent;
}
unsigned int XueZhanManager::getCurrentHuJiaAddPercent()
{
	return xuezhanData.currentHuJiaAddPercent;
}

bool XueZhanManager::canOpenBox()
{
	//if (xuezhanData.currentState == State_In && xuezhanData.currentPassedNum >0 && xuezhanData.currentPassedNum%5 == 0 )
	if (xuezhanData.currentState == State_In)
	{
		return mCanOpenBox;
	}
	return false;
}
bool XueZhanManager::canShowRewards()
{
	if (lastGettedSilverNum > 0 ||
		(_giftBag.id >0 && _giftBag.count>0))
	{
		CCAssert(xuezhanData.currentState == State_In,"异常");
		return true;
	}
	return false;
}

void XueZhanManager::setRewardShowed()
{
	lastGettedSilverNum = 0;
	_giftBag.id = 0;
	_giftBag.count = 0;
}

unsigned int XueZhanManager::getGettedSilverNum()
{
	return lastGettedSilverNum;
}
const GiftBag* XueZhanManager::getGettedGiftBag()
{
	return &_giftBag;
}







XueZhanManager::~XueZhanManager(void)
{
}
