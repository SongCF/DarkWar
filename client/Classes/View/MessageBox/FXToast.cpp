#include "FXToast.h"
#include "font.h"

#define LabelSize 24

#define DisplayTime 1.75f

FXToast * FXToast::create(string infoStr, ccColor3B labelColor, FXToast_CallbackInterface *callfunc, bool delayTimeRemove)
{
	FXToast *layer = new FXToast(infoStr, labelColor, callfunc);
	if (layer)
	{
		layer->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(layer);
		layer = NULL;
	}

	return layer;
}

FXToast::FXToast(string infoStr, ccColor3B labelColor, FXToast_CallbackInterface *callfunc, bool delayTimeRemove)
	:mClicked(false)
	, mCallfunc(callfunc)
	, m_delayAutoRemove(delayTimeRemove)
{
	CCLayerColor::initWithColor(ccc4(0,0,0,110));

	mWinSize = CCDirector::sharedDirector()->getWinSize();

	mBg = CCSprite::create("jiaoxue_tanchukuang_weijiesuo.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));


	mLabel = CCLabelTTF::create(infoStr.c_str(), fontStr_kaiti, LabelSize, CCSize(340, 0), kCCTextAlignmentCenter);
	mLabel->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
	mBg->addChild(mLabel);
	mLabel->setColor(labelColor);


	this->setTag(Tag_ToastLayer);
	this->setZOrder(200);
}


//
void FXToast::onEnter()
{
	CCLayerColor::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -140, true);

	if (m_delayAutoRemove)
		scheduleOnce(schedule_selector(FXToast::updateRemove), DisplayTime);
}

void FXToast::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayerColor::onExit();
}

bool FXToast::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (mClicked) return true;

// 	mBg->runAction(CCSequence::create(
// 		CCScaleTo::create(0.1f, 1.1f),
// 		CCScaleTo::create(0.1f, 0.9f),
// 		CCScaleTo::create(0.05f, 1),
// 		NULL));

	updateRemove(0);
	mClicked = true;

	return true;
}

void FXToast::updateRemove(float dt)
{
	mLabel->runAction(CCSpawn::create(
		CCFadeTo::create(0.15f, 0),
//		CCMoveBy::create(0.5f, ccp(0, 20)),  //它会跟着bg移动
		NULL));

	mBg->runAction(CCSequence::create(
		CCSpawn::create(CCFadeTo::create(0.15f, 0),CCMoveBy::create(0.15f, ccp(0, 20)),NULL),
		CCCallFunc::create(this, callfunc_selector(FXToast::callback_remove)),
		NULL));
}

void FXToast::callback_remove()
{
	this->removeFromParent();
	if (mCallfunc)
		mCallfunc->fxToast_callback();
}