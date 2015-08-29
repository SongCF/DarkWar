#pragma once

#include "cocos2d.h"
#include "BaseSprite.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;

using namespace cocos2d;

//1. 开场剧情
class GameStoryLayer : public CCLayer
{
public:
	~GameStoryLayer();
	CREATE_FUNC(GameStoryLayer);
	bool init();
	virtual void onEnter();
	virtual void onExit();
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

protected:
	void callback_OneStoryAnimateEnd(CCArmature *, MovementEventType, const char *);
	void callback_allAnimatePlayEnd();

	void onMenu_next(CCObject *pSender);
private:
	unsigned int mCurStoryIdx;
	CCArmature *mCurArmature;

	CCSize mWinSize;

	CCMenuItemImage *mNextItem;
	bool mTouchAble;
	CCLabelTTF *mNextLabel;
	BaseSprite *mBg;
};


//2. 章节插画
class ChapterIllustrationLayer : public CCLayer
{
public:
	~ChapterIllustrationLayer();
	static ChapterIllustrationLayer* create(unsigned int taskId, CCObject* callback_target, SEL_CallFunc pCallbackFunc);
	bool init(unsigned int taskId, CCObject* callback_target, SEL_CallFunc pCallbackFunc);
	virtual void onEnter();
	virtual void onExit();
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

protected:
	void callback_OneStoryAnimateEnd(CCArmature *, MovementEventType, const char *);
	void callback_allAnimatePlayEnd();

	void onMenu_next(CCObject *pSender);
private:
	CCArmature *mCurArmature;

	CCSize mWinSize;

	CCMenuItemImage *mNextItem;
	CCLabelTTF *mNextLabel;
	bool mTouchAble;

	string m_strAnimateName;

	CCObject* mCallbackTarget;
	SEL_CallFunc m_pCallbackFunc;
};