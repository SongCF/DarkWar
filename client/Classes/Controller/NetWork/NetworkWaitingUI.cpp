#include "NetworkWaitingUI.h"

#define Default_Priority -256

#define TAG_Waite	2313

NetworkWaitingUI::NetworkWaitingUI(void)
{
	BaseSprite* waite = BaseSprite::create("wait.png");
	addChild(waite,0,TAG_Waite);
	waite->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));

	waite->runAction(CCRepeat::create(
		CCRotateBy::create(1.0f,180),
		-1
		));
}

NetworkWaitingUI* NetworkWaitingUI::create()
{
	NetworkWaitingUI* w = new NetworkWaitingUI();
	w->autorelease();
	return w;
}

bool NetworkWaitingUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//CCNode* waite = getChildByTag(TAG_Waite);
	//waite->setPosition(pTouch->getLocation());
	return true;
}

void NetworkWaitingUI::onExit()
{
	BaseLayer::onExit();

	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}
void NetworkWaitingUI::onEnter()
{
	BaseLayer::onEnter();

	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,Default_Priority,true);
}


NetworkWaitingUI::~NetworkWaitingUI(void)
{
}
