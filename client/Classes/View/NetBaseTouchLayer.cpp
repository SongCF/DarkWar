#include "NetBaseTouchLayer.h"

NetBaseTouchLayer::NetBaseTouchLayer()
	:CCLayer()
{
}

NetBaseTouchLayer::~NetBaseTouchLayer() {
}
bool NetBaseTouchLayer::init() {
	if (!CCLayer::init()) {
		return false;
	}
	return true;
}

void NetBaseTouchLayer::onEnter() {
	CCLayer::onEnter();
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-129,true);
}
void NetBaseTouchLayer::onExit() {
	//CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);

	CCLayer::onExit();
}
bool NetBaseTouchLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}