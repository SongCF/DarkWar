#include "JGG_FootprintManager.h"
#include "cocos2d.h"
using namespace cocos2d;

JGG_FootprintManager *JGG_FootprintManager::_manager = NULL;

JGG_FootprintManager::JGG_FootprintManager()
{

}

JGG_FootprintManager* JGG_FootprintManager::getManager()
{
	if (_manager == NULL)
	{
		_manager = new JGG_FootprintManager;
	}

	return _manager;
}

vector<bool> JGG_FootprintManager::getFootprintById(int id)
{
	if (id == mTaskId)
	{
		return mFootprint;
	}
	else
	{
		if (mTaskId != 0)
			CCLog("error: %s ---> [get ID != old id]", __FUNCTION__);
		return vector<bool>(9, false);
	}
}

void JGG_FootprintManager::setFootprint(int id, int jggIndex)//走过就设置为true
{
	if (mTaskId == id)
	{
		mFootprint.at(jggIndex) = true;
	}
	else       //已经是另外一个任务了。。。
	{
		CCLog("Warrior: %s ---> [set ID != old task id]", __FUNCTION__);
		clearJGGFootprint();
		mTaskId = id;
		mFootprint = vector<bool> (9, false);
		mFootprint.at(jggIndex) = true;
	}
}

void JGG_FootprintManager::clearJGGFootprint()
{
	mTaskId = 0;
	mHasDisplayBoss = false;
	mFootprint.clear();
}