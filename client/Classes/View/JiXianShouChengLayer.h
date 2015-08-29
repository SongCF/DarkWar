
#pragma once

#include "BaseLayer.h"
#include "BaseSprite.h"
#include "IMainLayerBody.h"
#include "MessageDisplay.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;


class JiXianShouChengLayer : public BaseLayer, public IMainLayerBody
	, public CCTableViewDataSource, public CCTableViewDelegate, public MessageDisplayCallBackInterface
{
public:
	//传2 就表示打完一场战斗 跳过来
	JiXianShouChengLayer();
	~JiXianShouChengLayer();
	static JiXianShouChengLayer* create();


////////////////////////////////////////////////////////////////////////////////////////
	//messagedisplay
	virtual void messageDialogEndCallback();
	//footlayer
	virtual void disableAllTouchBegin(){mMainMenu->setEnabled(false);}
	virtual void disableAllTouchEnd(){mMainMenu->setEnabled(true);}
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return mBg->getPosition();}
////////////////////////////////////////////////////////////////////////////////////////////
	// CCScrollViewDelegate
	virtual void scrollViewDidScroll(CCScrollView* view){}
	virtual void scrollViewDidZoom(CCScrollView* view){}
	// CCTableViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell){}
	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);
////////////////////////////////////////////////////////////////////////////////////////////

protected:
	void refreshBackGround();
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_resportMenu(CCObject *pSender);
	void displayReport();
	void refreReportBoard();
	int getRandomRotation();
	CCPoint getRandomPosition(int i);
	void removeChildFromParent_WhenActionEnd(CCNode *node);

	//展示其他玩家的伤害
	void msg_displayOtherHero(CCObject *msg);
	void update_BossBlood(float dt);
	void update_WaitOpenTime(float dt);
	void update_WaitCDTime(float dt);
	//
	void rsp_getCurrentState(CCObject *msg);
	void rsp_beginFight(CCObject *msg_f);
	void rsp_getReport(CCObject *msg);
	void rsp_getReward(CCObject *msg);

	virtual void onExit();

private:
	BaseSprite* mBg;
	CCMenu *mMainMenu;
	BaseSprite *mBossInfoBar;

	int mCurState;   //未开启0  开启1  死亡2

	CCTableView *mTableView;
};