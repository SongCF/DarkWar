#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "BaseActor.h"
#include "ImageManager.h"
#include "scrollView/GridScrollView.h"
#include "RenWuXiangXiLayer.h"
#include "MainLayerBase.h"
#include "cocos-ext.h"
#include "BaseActorPropertyParser.h"
#include "ZhuangBeiMianBanYuanShen.h"
#include "JiNeng_SelectLayer.h"
#include "ZhuangbeiOptionLevel.h"
USING_NS_CC_EXT;

#define TAG_HuShenFu 4643+1105
#define TAG_Kuijia 4643+1101
#define TAG_JieZhi 4643+1102
#define TAG_WuQi 4643+1103
#define TAG_DunPai 4643+1104

class ZhuangBeiMianBanOther : public BaseLayer, public IMainLayerBody, public ClickableItemCreater
	, public ZhuangBeiMianBan_YuanShenBack_Interface
{
public:
	static ZhuangBeiMianBanOther* create(CCArray *actors, CCArray *willDelYuanShen, vector<Equipment*> willDelEquip, int idx=0);
	bool init(CCArray *actors, CCArray *willDelYuanShen, vector<Equipment*> willDelEquip, int idx=0);
	~ZhuangBeiMianBanOther()
	{
		for(int i=0; i!= mWillDeleteEquip.size(); ++i)
		{
			delete mWillDeleteEquip.at(i);
		}

		mHeroActors->release();
		mWillDeleteActors_YuanShen->release();
	}

	virtual void initFoot(MainLayerBase* footerLayer)
	{
		mFooterLayer = footerLayer;
		addHeroScrollView();
	}

	// BaseLayer 
	virtual void refreshLayer();

	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();

	//元神面板回调
	virtual void yuanshen_backFunc(unsigned int yuanshenId); 
	// 创建一个Item，根据传入的页面号，x(从左到右的号)，y(从上到下的号)
	virtual ClickableItem* createGridItem(int pageIndex,int xIndex,int yIndex);
	// 页面选择了
	virtual void pageSelectedEnd(int pageIndex);

	vector<EquipData> getEquipsDataByActorId(unsigned int actorId);  // 装备详情里面 点了套装信息 必须要到该数据

 
protected:
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemHeroListClicked(CCObject* pSender);
	void menuItemEquipClicked(CCObject* pSender);
	void menuItemClicked_Back(CCObject *pSender);
	void menuItemClicked_BackFromGongMing(CCObject *pSender);

	void addHeroScrollView();

	void refreshEquipmentUI();
	void refreshPrepertyLabel();
	void refreshHerosImgScrollView();

protected:
	BaseSprite* mBg;
	CCMenu* mEquipMenu;
	CCMenu* mHeroMenu;
	CCMenu *mMainMenu;

	int mCurHeroIdx;
	CCArray *mHeroActors;
	CCArray *mWillDeleteActors_YuanShen; //元神放到 heroActor中时  是不会retain的
	vector<Equipment*> mWillDeleteEquip;
	//
	vector<Equipment*> mCurEquips;

	GridScrollView* mHeroView;
	vector<ZhuangbeiOptionLevel*>		m_lOptionList;
};




class ScrollOhterHeroNode : public ClickableItem
{
public:
	ScrollOhterHeroNode(CCNode* node, BaseActor* actor, MainLayerBase* footer)
	{
		init();
		mFooterLayer = footer;
		mActor = actor;
		mActor->retain();

		addChild(node);
		this->setContentSize(CCSizeMake(288, 531));
		node->setPosition(ccp(this->getContentSize().width/2, this->getContentSize().height/2));
	}

	~ScrollOhterHeroNode()
	{
		mActor->release();
	}

	virtual void clickedMe()
	{
		RenWuXiangXiLayer* layer = new RenWuXiangXiLayer(mActor, comefrom_otherMianBan);
		mFooterLayer->popUpContaintLayer(layer);
		layer->release();
	}

	virtual void touchMe()
	{
		anchorPointBk = getAnchorPoint();
		isIgnoreAnchorBk = isIgnoreAnchorPointForPosition();
	}

protected:
	MainLayerBase* mFooterLayer;
	BaseActor *mActor;
};
