#pragma once

#include "cocos2d.h"
#include <string>

using std::string;
using namespace cocos2d;

#define Tag_ToastLayer 4643 + 3000




class FXToast_CallbackInterface
{
public:
	virtual void fxToast_callback() = 0;
};




class FXToast : public CCLayerColor
{
public:
	static FXToast * create(string infoStr, ccColor3B labelColor=ccWHITE, FXToast_CallbackInterface *callfunc = NULL, bool delayTimeRemove=true);
	FXToast(string infoStr, ccColor3B labelColor=ccWHITE, FXToast_CallbackInterface *callfunc = NULL, bool delayTimeRemove=true);

	//
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

protected:

	void updateRemove(float dt);
	void callback_remove();

private:
	CCSprite *mBg;
	CCLabelTTF *mLabel;

	CCSize mWinSize;
	bool mClicked;

	FXToast_CallbackInterface *mCallfunc;

	bool m_delayAutoRemove;
};