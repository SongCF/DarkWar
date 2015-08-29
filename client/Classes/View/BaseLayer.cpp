#include "BaseLayer.h"
#include "CmdHelper.h"


BaseLayer::BaseLayer(void)
{
	mWinSize = CCDirector::sharedDirector()->getWinSize();
	mVisibleSize = CCDirector::sharedDirector()->getVisibleSize();
	mVisibleOrigin = CCDirector::sharedDirector()->getVisibleOrigin();
	mCenterPoint = ccp(mVisibleSize.width/2,mVisibleSize.height/2);

	mResMgr = ResourcePathManager::getManager();
	setTouchEnabled(false);
	msgVec.clear();
	mCmdHlp = CmdHelper::getHelper();
}
void BaseLayer::disableAllTouchBegin()
{
}
void BaseLayer::disableAllTouchEnd()
{
}

void BaseLayer::changeMenuHandlerPriority(CCMenu* menu,bool upOrDown)
{
	if (upOrDown)
	{
		menu->runAction(CCSequence::create(
			CCDelayTime::create(0),
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::shengGaoMenuHandlerPriority)),
			NULL
			));
	}
	else
	{
		menu->runAction(CCSequence::create(
			CCDelayTime::create(0),
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::jiangDiMenuHandlerPriority)),
			NULL
			));
	}

}
// 降低menu的优先级
void BaseLayer::jiangDiMenuHandlerPriority(CCNode* menu)
{
	CCMenu* _m = (CCMenu*)menu;
	_m->setHandlerPriority(-127);
}

// 升高menu的优先级
void BaseLayer::shengGaoMenuHandlerPriority(CCNode* menu)
{
	CCMenu* _m = (CCMenu*)menu;

	_m->setHandlerPriority(-129);
}

void BaseLayer::addObserver(SEL_CallFuncO selector,const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,selector,name,NULL);
	msgVec.push_back(name);
}
// 移除消息监听
void BaseLayer::removeObserver(const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,name);
}

// 移除一个Node
void BaseLayer::removeNode(CCNode* toRemove){
	if (toRemove)
	{
		toRemove->removeFromParent();
	}
}

void BaseLayer::onExit()
{
	LFLayer::onExit();
}

BaseLayer::~BaseLayer(void)
{
	// 自动移除所有的监听
	int num = msgVec.size();  
	for (int i=0;i<num;i++)
	{
		removeObserver(msgVec[i].c_str());
	}
	msgVec.clear();
	CCLog("Remove all Observer");
}
