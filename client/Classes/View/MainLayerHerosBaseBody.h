#pragma once

#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
#include "Equipment.h"
#include "MyselfManager.h"
#include "ImageManager.h"
#include "EquipmentXiangQingBody.h"
#include <map>

using namespace cocos2d::extension;
using std::map;


// 物品界面的全部选项内容
class MainLayerHerosBaseBody : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
{
protected:

	CCTableView* mTableView;
	vector<BaseActor*> mHerosVec;
	int mFlag;  // 1已解锁、 2未解锁

	unsigned int mJieSuo_HeroId;
	// CCTableView
	virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);

	// CCScrollViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell);
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell);

	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);

protected:
	void menuItemClicked_jiesuoHero(CCObject *pSender);
	void menuItemClicked_LockHero_OkCancel(CCObject *pSender);
	void menuItemClicked_choseHero(CCObject* pSender);
	void menuItemClicked_heroImg(CCObject* pSender);

	virtual void rsp_JieSuoYingXiong(CCObject *msg); //子类实现

	vector<int> mChoseHeroState;
	int         mChoseNum;

	// 刷新数据
	void setHerosData(vector<BaseActor*> herosVec)
	{
		for(int i=0; i!=herosVec.size(); ++i)
			if(MyselfManager::getManager()->getMyZhuJueData()->getId() == herosVec.at(i)->getId())
			{
				herosVec.erase(herosVec.begin()+i); //删除主角，不让他在酒馆中显示
				break;
			}
		this->mHerosVec = herosVec; 
		sortTableList();

		mChoseHeroState.resize(mHerosVec.size());
		mChoseNum = 0;
		for(int i=0; i!=mHerosVec.size(); ++i)
		{
			mChoseHeroState.at(i) = mHerosVec.at(i)->getChessBoardPos();  //0就是没上阵
			if(mChoseHeroState.at(i)>0)		++mChoseNum;
		}

		mTableView->reloadData();	
	};
	void sortTableList();

	virtual BaseSprite* getItemContentByIndex(unsigned int index);
	//
//	virtual void refreshItem(CCNode* item,int index);

	string getHeroDirectionByProfessionId(Profession profId);

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
public:
	MainLayerHerosBaseBody(int flag=1);
	~MainLayerHerosBaseBody(void);
};


