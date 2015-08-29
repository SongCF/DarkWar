#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"

class JiaoTangLayer : public BaseLayer, public IMainLayerBody
{
public:
	JiaoTangLayer(int flag = 0);
	~JiaoTangLayer();
	static JiaoTangLayer* create(int flag = 0); // 0升级教堂、  1角色洗礼

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	//
	virtual CCSize getBodySize();
	virtual CCPoint getBodyCenterPos();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	void clearBg();
	//洗礼时，显示属性的board
	void setPropBoardData(BaseSprite *board, int stage, bool cur);

	void menuItemClicked_HeadMenu(CCObject *pSender);
	void menuItemClicked_JiaoTangMainMenu(CCObject *pSender);
	void menuItemClicked_XiLiMainMenu(CCObject *pSender);
	void menuItemClicked_XiLiHeroBoard(CCObject *pSender);

	void rsp_JiaoTangLevelUp(CCObject *msg);
	void rsp_HeroDegreeUp(CCObject *msg);

private:
	BaseSprite *mBg;
	CCMenu *mHeadMenu;
	CCMenu *mMainMenu;
	int mCurFlag;

	vector<BaseActor*> mActorsVec;

	CCMenuItemImage *mXiLi_CurHeroItem;
};