#pragma once
#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
//#include "MofaJinghuaManager.h"
#include "SuiPianManager.h"
#include "MainLayerItemsBaseBody.h"

using namespace cocos2d::extension;


// 主界面点击装备选项看到的界面
class MainLayerBeibaoItemList : public MainLayerItemsBaseBody,public IMainLayerBody
{
private:
	BaseSprite* mBg;
	int selectedIndex;
//	vector<MofaJinghua> mMoFaJingHua;
	vector<ItemSuipian> mSuipianItemVec;
	vector<ShenbingSuipian> mSuipianEquipVec;
	int mFrom;
protected:

public:
	
	void menuItemCancelClicked(CCObject* obj);
	void menuItemMakeSureClicked(CCObject* obj);
	void headItemClicked(CCObject* pSender);
	void cacelClicked(CCObject* pSender);
	void makeSureClicked(CCObject* pSender);
	void menuItemClicked_Da_Kai(CCObject *pSender);
	void rsp_DaikLibao(CCObject *object);
	void refreshAllHeadMenu();

	virtual unsigned int numberOfCellsInTableView(CCTableView *table)
	{
		if(id==0){
		   return (mItemsVec.size() + mSuipianItemVec.size() + mSuipianEquipVec.size());
		}else{
		  return  giftbags.size();
		}
	}

	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);

	// 刷新数据
	virtual void setItemsData(vector<ItemInfo> itemsVec, vector<ItemSuipian> suipianItemVec, vector<ShenbingSuipian> suipianEquipVec);
	virtual BaseSprite* getItemContentByIndex(unsigned int index);
	

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();


	// 得到body的大小
	virtual CCSize getBodySize(){return CCSize(640, 842);};
	//
	virtual CCPoint getBodyCenterPos(){return ccp(320,mBg->getContentSize().height/2);};

	MainLayerBeibaoItemList(int from=0);

	void rsp_getLibao(CCObject *msg);
	~MainLayerBeibaoItemList(void);
};