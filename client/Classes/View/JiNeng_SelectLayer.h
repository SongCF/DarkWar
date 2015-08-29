
#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "protocol.h"
#include "flat.h"
#include "SkillManager.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;

enum JiNengLayer_comeFrom
{
	comefrom_JiuGuan,            //来自酒馆， 可查看技能列表 但只能看，不能操作
	comefrom_renxingMianBan,     //自己的、想怎么操作就这么操作
	comefrom_otherMianBan,       // 和详细对比一样。
	comefrom_xiangxiDuiBi,       //详细对比时， 查看技能列表的按钮不可点击
};

class JiNeng_SelectLayer : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	static JiNeng_SelectLayer* create(BaseActor* actor, JiNengLayer_comeFrom from);
	bool init(BaseActor* actor, JiNengLayer_comeFrom from);

	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_learnSkill(CCObject *pSender);
	void menuItemClicked_skillLevelUp(CCObject *pSender);
	void menuItemClicked_LevelUpWindow(CCObject *pSender);

	void rsp_LearnSkill(CCObject *msg);
	void rsp_LevelUpSkill(CCObject *msg);

	SkillType getSkillType(Profession prof, int idxCell);
	void equipSkill_Rsp(CCObject *msg);

	virtual void onEnter();

public:
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return mBg->getPosition();}
public:
	// CCScrollViewDelegate
	virtual void scrollViewDidScroll(CCScrollView* view){}
	virtual void scrollViewDidZoom(CCScrollView* view){}
	// CCTableViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell){}
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell){}
	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx){return CCSize(622, 206);}
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);

protected:
	CCTableView* mTableView;
	BaseSprite *mBg;
	CCMenu *mMainMenu;

	BaseActor *mActor;
	vector<SkillData*> mAllSkill;

	int mSelectIndex;
	JiNengLayer_comeFrom mFrom;
};