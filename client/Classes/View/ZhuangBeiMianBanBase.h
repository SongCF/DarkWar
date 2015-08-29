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
#include "ZhuangBeiMianBanYuanShen.h"
#include "cocos-ext.h"
#include "ZhuangbeiOptionLevel.h"
#include "GuideLayerBase.h"
USING_NS_CC_EXT;

#define TAG_HuShenFu 4643+1105
#define TAG_Kuijia 4643+1101
#define TAG_JieZhi 4643+1102
#define TAG_WuQi 4643+1103
#define TAG_DunPai 4643+1104


class ZhuangBeiMianBanBase : public BaseLayer, public IMainLayerBody
	, public ClickableItemCreater/*, public MyMenuCallBackInterface*/, public LevelUpEndCallBackInterface
	, public ZhuangBeiMianBan_YuanShenBack_Interface
{
public:
	static ZhuangBeiMianBanBase* create(int id, bool bShowQianghuaTipSpr = false);
	bool init(int id);
	ZhuangBeiMianBanBase();
	~ZhuangBeiMianBanBase();

	virtual void initFoot(MainLayerBase* footerLayer)
	{
		mFooterLayer = footerLayer;
		addHeroScrollView();
	}
///////////////////////////////////////////////////////////////////////////////////////////
	// BaseLayer 
	virtual void refreshLayer();
	// LevelUpEndCallBackInterface
	virtual void LevelUpEndCallBackFunc();

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
	// 元神返回
	virtual void yuanshen_backFunc(unsigned int yuanshenId);

	virtual void onEnter();
////////////////////////////////////////////////////////////////////////////////////////////////

	void teachGuide();

	//MyMenuCallBackInterface
	//3秒长按 卸下装备
	virtual void menuItemLongTimeTouched(CCObject* pSender);
	//发送跟换装备请求
	void setChangeZhuangBei(TempEquipZhuangbei data, Equipment* equip);
	void changeZhuangBeiRsp(CCObject* msg_rsp);
	void removeZhuangBeiRsp(CCObject* msg_rsp);
 

	inline void HuanZhuangTeach()
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstJinXingTongGuan);
	}
protected:
	void menuItemHeroListClicked(CCObject* pSender);
	void menuItemEquipClicked(CCObject* pSender);
	void menuItemAddEquipClicked(CCObject* pSender);
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_BackFromGongMing(CCObject *pSender);

	void addHeroScrollView();

//	string getJianYingFileName(Profession prof, int pos, bool isHaveWuqi=false, bool isHaveDunpai = false, bool isSS_Wuqi=false);
	void refreshEquipmentUI();
	void refreshPrepertyLabel();
	void refreshHerosImgScrollView();
private:
	void IsChuanqi(CCNode *item);
	Equipment* getMinEquipment();
protected:
	BaseSprite* mBg;
	CCMenu* mEquipMenu;
	CCMenu* mHeroMenu;
	CCMenu *mMainMenu;

	vector<BaseActor*> mHeros;
	int mCurHeroIdx;
	vector<Equipment*> mCurHeroEquips;

	bool m_bShowQianghuaTipSpr;
	//换装
	Equipment* mChangeEquip;

	Equipment* m_pMinEquipemnt;
	BaseSprite* m_CurShowQianghuaDiwen;

	GridScrollView* mHeroView;

	vector<ZhuangbeiOptionLevel*>		m_lOptionList;
};



class ScrollHeroNode : public ClickableItem
{
public:
	ScrollHeroNode(CCNode* node, int id, MainLayerBase* footer)
	{
		init();
		mFooterLayer = footer;
		mActorId = id;

		addChild(node);
		this->setContentSize(CCSizeMake(288, 531));
		node->setPosition(ccp(this->getContentSize().width/2, this->getContentSize().height/2));
	}

	virtual void clickedMe()
	{
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(mActorId);
		RenWuXiangXiLayer* layer = new RenWuXiangXiLayer(actor, comefrom_renxingMianBan);
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
	int mActorId;
};