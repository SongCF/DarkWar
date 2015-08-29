
#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "BonusManager.h"
#include "EquipmentTaoZhuangXiangQing.h"
#include "cocos-ext.h"
using namespace cocos2d::extension;


class LoginGiftLayer : public BaseLayer, public IMainLayerBody, public CCTableViewDataSource, public CCTableViewDelegate
	, public TaoZhuangLayerInterface
{
public:
	static LoginGiftLayer* create();
	virtual bool init();

	virtual void menuItemClicked_MainMenu(CCObject *pSender);
	virtual void menuItemClicked_GetGift(CCObject *pSender);
	virtual void menuItemClicked_Equip(CCObject *pSender);
	virtual void rsp_getGift(CCObject *msg_f);

	void menuItemClicked_equipDetailMenu(CCObject *pSender);

	inline void setCallBakc(CCObject *obj,SEL_CallFunc resetDengLu)
	{
		m_oTarger = obj;
		m_hResetDengLu = resetDengLu;
		//m_hResetDengJi = resetDengJi;
	}
public:
	virtual void onEnter();
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return CCSize(mWinSize.width, mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height);} // 106是底的高度
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return ccp(mWinSize.width/2, (mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height)/2);}
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
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);
	
	//套装详情回调接口
	virtual void callbackInterface_TaoZhuangLayer();


protected:
	CCTableView* mTableView;
	int mClickedItemIdx;   //用于刷新单个 cell
//	BaseSprite *mBg;

	CCMenu *mMainMenu;

	vector<Flat_BonusInfo> mGiftsVec;
	Flat_DuanzaoGroupZBInfo mCurTaoZhuang_Info;

	CCObject				*m_oTarger;
	SEL_CallFunc			m_hResetDengLu;
	//SEL_CallFunc			m_hResetDengJi;
};