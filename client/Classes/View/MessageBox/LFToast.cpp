#include "LFToast.h"
#include "cocos-ext.h"
#include "font.h"

using namespace cocos2d::extension;

LFToast::LFToast(string msg,float during)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCScale9Sprite* base = CCScale9Sprite::create("toast.png");
	CCLabelTTF* lable = CCLabelTTF::create(
		msg.c_str(),
		fontStr_kaiti,
		30,
		CCSize(base->getContentSize().width - 10,0), 
		kCCTextAlignmentLeft);
	lable->setAnchorPoint(ccp(0,0.5f));
	addChild(base);
	base->setContentSize(CCSizeMake(base->getContentSize().width,lable->getContentSize().height + 20));
	base->setOpacity(0);
	base->addChild(lable);
	lable->setPosition(ccp(10,base->getContentSize().height/2));
	base->setPosition(ccp(visibleSize.width/2,base->getContentSize().height + 20));
	base->runAction(CCSequence::create(
		CCFadeTo::create(0.2f,255),
		CCDelayTime::create(during),
		CCFadeTo::create(0.1f,0),
		CCCallFuncN::create(this,SEL_CallFuncN(&LFToast::removeToast)),
		NULL
		));
}
LFToast::~LFToast(void)
{
	//CCLog("LFToast delete");
}

// 创建，并显示一个Toast
void LFToast::makeToast(string msg,float during /*= 1.0f*/)
{
	makeToast(msg,CCPointZero,during);
}
// 在指定的位置显示一个
void LFToast::makeToast(string msg,CCPoint offset,float during /*= 1.0f*/)
{
	LFToast* temp = new LFToast(msg,during);
	CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
	scene->addChild(temp,2048);
	temp->release();
	temp->setPosition(offset);

	temp->runAction(CCSequence::create(
		CCDelayTime::create(1.0f + during),
		CCCallFuncN::create(temp,SEL_CallFuncN(&LFToast::removeNode)),NULL));
}

void LFToast::removeToast(CCNode* node)
{
	if (node)
	{
		node->removeFromParent();

		//CCLog("Toast disapear");
	}
}

void LFToast::removeNode(CCNode* node)
{
	BaseSprite::removeNode(node);

	//CCLog("Toast removed");
}
