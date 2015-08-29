#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "BaseSprite.h"
#include "BaseActor.h"
#include "ImageManager.h"
#include "scrollView/GridScrollView.h"
#include "RenWuXiangXiLayer.h"
#include "MainLayerBase.h"
#include "MyselfManager.h"
//#include "MyMenu.h"
#include "LevelUpLayer.h"
#include "DegreeUpLayer.h"
#include "cocos-ext.h"
#include "ZhuangbeiOptionLevel.h"
USING_NS_CC_EXT;

#define TAG_HuShenFu 4643+1105
#define TAG_Kuijia 4643+1101
#define TAG_JieZhi 4643+1102
#define TAG_WuQi 4643+1103
#define TAG_DunPai 4643+1104

enum yuanshen_comefrom
{
	comefrom_base = 1,
	comefrom_other = 2,
};



class ZhuangBeiMianBan_YuanShenBack_Interface
{
public:
	virtual void yuanshen_backFunc(unsigned int yuanshenId) = 0;
};


class ZhuangBeiMianBanYuanShen : public BaseLayer, public IMainLayerBody
	, public ClickableItemCreater/*, public MyMenuCallBackInterface*/, public LevelUpEndCallBackInterface, public DegreeUpEndCallBackInterface
{
public:
	static ZhuangBeiMianBanYuanShen* create(CCArray *arr_YuanShen, int yuanshen_id, yuanshen_comefrom from, ZhuangBeiMianBan_YuanShenBack_Interface *callfunc);
	bool init(CCArray *arr_YuanShen, int yuanshen_id, yuanshen_comefrom from, ZhuangBeiMianBan_YuanShenBack_Interface *callfunc);
	ZhuangBeiMianBanYuanShen();
	~ZhuangBeiMianBanYuanShen();

	virtual void initFoot(MainLayerBase* footerLayer)
	{
		mFooterLayer = footerLayer;
		addHeroScrollView();
	}

	vector<EquipData> getEquipsDataByActorId(unsigned int actorId);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// BaseLayer 
	virtual void refreshLayer();
	// LevelUpEndCallBackInterface
	virtual void LevelUpEndCallBackFunc(){refreshLayer();}
	virtual void DegreeUpEndCallBackFunc(){refreshLayer();}
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);}

	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();


	// 创建一个Item，根据传入的页面号，x(从左到右的号)，y(从上到下的号)
	virtual ClickableItem* createGridItem(int pageIndex,int xIndex,int yIndex);
	// 页面选择了
	virtual void pageSelectedEnd(int pageIndex);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//MyMenuCallBackInterface
	//3秒长按 卸下装备
	virtual void menuItemLongTimeTouched(CCObject* pSender);
	//发送跟换装备请求
	void setChangeZhuangBei(TempEquipZhuangbei data, Equipment* equip);
	void changeZhuangBeiRsp(CCObject* msg_rsp);
	void removeZhuangBeiRsp(CCObject* msg_rsp);

protected:
	void menuItemHeroListClicked(CCObject* pSender);
	void menuItemEquipClicked(CCObject* pSender);
	void menuItemAddEquipClicked(CCObject* pSender);
	void menuItemClicked_MainMenu(CCObject *pSender);
//	void menuItemClicked_BackFromGongMing(CCObject *pSender);

	void addHeroScrollView();

	void refreshEquipmentUI();
	void refreshPrepertyLabel();
	void refreshHerosImgScrollView();

protected:
	BaseSprite* mBg;
	CCMenu* mEquipMenu;
	CCMenu* mHeroMenu;
	CCMenu *mMainMenu;

	CCArray *mArray_YuanShen;
	int mCurYuanShenIdx;
	vector<Equipment*> mCurYuanShenEquips;

	TempEquipZhuangbei reqData;
	//换装
	Equipment* mChangeEquip;

	GridScrollView* mHeroView;

	yuanshen_comefrom mComeFrom;
	ZhuangBeiMianBan_YuanShenBack_Interface *mCallFunc;

	vector<ZhuangbeiOptionLevel*>		m_lOptionList;
};



class ScrollYuanShenNode : public ClickableItem
{
public:
	ScrollYuanShenNode(CCNode* node, BaseActor* actor, MainLayerBase* footer, yuanshen_comefrom from)
	{
		init();
		mFooterLayer = footer;
		mFrom = from;
		actor->retain();
		mActor = actor;

		addChild(node);
		this->setContentSize(CCSizeMake(288, 531));
		node->setPosition(ccp(this->getContentSize().width/2, this->getContentSize().height/2));
	}

	~ScrollYuanShenNode()
	{
		mActor->release();
	}

	virtual void clickedMe()
	{
		RenWuXiangXiLayer* layer = NULL;
		if (mFrom == comefrom_base)
			layer = new RenWuXiangXiLayer(mActor, comefrom_renxingMianBan, true);
		else 
			layer = new RenWuXiangXiLayer(mActor, comefrom_otherMianBan, true);
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
	yuanshen_comefrom mFrom;
};