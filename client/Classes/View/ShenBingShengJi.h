#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include <string>

using std::string;

class Equipment;

class ShenBingShengJi : public BaseLayer, public IMainLayerBody
{
public:
	ShenBingShengJi(Equipment *eq);

protected:
	void menuItemClicked_LevUp(CCObject *pSender);
	void menuItemClicked_GoBack(CCObject *pSender);
	void menuItemClicked_UpEndDialog(CCObject *pSender);

	void clearBg();
	void showPropertyBoard(bool isCur = true);
	void showNeedMaterial();
	void showLevUpEndDialog();

	void rsp_levUp(CCObject *msg_f);

public:
	virtual void onEnter();
/////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin(){mMainMenu->setEnabled(false);}
	virtual void disableAllTouchEnd(){mMainMenu->setEnabled(true);}
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	virtual CCPoint getBodyCenterPos(){return mBg->getPosition();}
/////////////////////////////////////////////////////////////////////////////////////////////////
	~ShenBingShengJi(void);
	int isAnimation;
	int ishuidian;
	BaseSprite *currentzb;
	CCArmature *ar;
	CCArmature *ar1;
	void Qianhuadh(BaseSprite *xingxing,int level);
	void callfunc_AnimateEnd(CCArmature *, MovementEventType, const char *);


private:
	BaseSprite *mBg;
	CCMenu *mMainMenu;

	Equipment *mCurEquip;

	unsigned int mEqOldLevel;


	string mErrorInfo;
};