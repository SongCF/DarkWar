#pragma once

#include "BaseLayer.h"
#include "cocos-ext.h"
using namespace std;
using namespace cocos2d::extension;


#define TAG_MENU_HOME 1000
#define TAG_MENU_MAOXIAN 1001
#define TAG_MENU_HUODONG 1002
#define TAG_MENU_JINGJI 1003
#define TAG_MENU_STORE 1004
#define TAG_Jingji_lizi_Ani					1005
#define TAG_Jixianshoucheng_lizi_Ani			1006

typedef enum enum_MainLayerIndex
{
	mainlayer_ZhuYe = 0,         // 主页
	mainlayer_MaoXian = 1,       // 冒险
	mainlayer_ZhuangBei = 2,     // 装备
	mainlayer_YingXiongBang = 3,   // 排行榜
	mainlayer_ChongZhi = 4,     //充值
	mainlayer_ShangDian = 5,     // 商店
	mainlayer_JingJiChang_challenge,         //竞技场 (默认用该项)
	mainlayer_JingJiChang_random,       //竞技场
	mainlayer_HuoDong,           //活动副本

}enum_MainLayerIndex;

class FooterCallback
{
public:
	virtual void footerItemSelected(enum_MainLayerIndex index){};
};

// 头部数据
class MainFooterLayer : public BaseLayer
{
	enum _iTag
	{
		Tag_MessageInfoBar = 101,
		Tag_MessageTipStr,
	};


private:
	FooterCallback* mCallback;
	BaseSprite* mFootContainer;

	CCMenu* mMenu;
//	CCMenuItemImage* mMenuItemHome;
//	CCMenuItemImage* mMenuItemZhuangbei;
//	CCMenuItemImage* mMenuItemMaoxian;
// 	CCMenuItemImage* mMenuItemJinji;
// 	CCMenuItemImage* mMenuItemMengyou;
//	CCMenuItemImage* mMenuItemStore;

	void initAllMenuItemPic();

	void showMessageBar(MarqueeMessage info);
	void removeMessageBar(CCNode* pNode);
	void getNewMessageInfo(CCNode* pInfoStr);

	enum_MainLayerIndex selectedIndex;
protected:

public:
	MainFooterLayer(void);

	CCSize getFooterSize();
	void selectAtIndex(enum_MainLayerIndex index,bool reSelect = false);
	void setCallBack(FooterCallback* callback);

	void menuItemClicked(CCObject* item);
	~MainFooterLayer(void);

	CCMenuItem* getMaoXianMenu(){return (CCMenuItem*)mMenu->getChildByTag(TAG_MENU_MAOXIAN);}
	CCMenuItem* getJingJiMenu(){return (CCMenuItem*)mMenu->getChildByTag(TAG_MENU_JINGJI);}
	CCMenuItem* getHuoDongMenu(){return (CCMenuItem*)mMenu->getChildByTag(TAG_MENU_HUODONG);}
	void updateMessage(float delta);
	void messageMoveDone(CCNode* pInfoStr);
	void checkIsShowLiziAni(CCObject* pObj);
	void checkJixianshouchengIsOpen(float dt);
	void showLiziXiaoguo(CCNode* pParent, int iTag);
	void removeLiziXiaoguo(CCNode* pParent, int iTag);
};



//
//class FooterCallback
//{
//public:
//	virtual void footerItemSelected(int index){};
//};
//
//// 头部数据
//class MainFooterLayer : public BaseLayer , public CCTableViewDataSource, public CCTableViewDelegate
//{
//private:
//	FooterCallback* mCallback;
//	BaseSprite* mFootContainer;
//	CCTableView* tableView;
//	int selectedIndex;
//protected:
//	// CCTableView
//	virtual void scrollViewDidScroll(CCScrollView* view);
//	virtual void scrollViewDidZoom(CCScrollView* view);
//
//	// CCScrollViewDelegate
//	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
//	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell);
//	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell);
//
//
//	// CCTableViewDataSource
//	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
//	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
//	virtual unsigned int numberOfCellsInTableView(CCTableView *table);
//
//public:
//	MainFooterLayer(void);
//
//	void selectAtIndex(unsigned int index,bool reSelect = false);
//
//	CCSize getFooterSize();
//	void setCallBack(FooterCallback* callback);
//	~MainFooterLayer(void);
//};

