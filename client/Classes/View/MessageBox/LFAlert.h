#ifndef __LFAlert_H__
#define __LFAlert_H__
#include "cocos2d.h"
#include <string>

using namespace std;
using namespace cocos2d;

/***
//弹框背景
BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
dialog->setPosition(ccp(320, mTableView->getViewSize().height/2 + 50));
//按键
CCMenu* menu;
{
CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
"shangpin_goumai_queren.png",
"shangpin_goumai_queren_select.png",
this,SEL_MenuHandler(&MainLayerStoreDaoju::menuItemOKAfterBuyClicked));
menu = CCMenu::create(menuItemMakeSure, NULL);
menu->setPosition(CCPointZero);
menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/2,94));
}

// 需要没有添加到某个Node的Menu和没有添加到某个Node的base
LFAlert* lfAlert = new LFAlert();
lfAlert->setBaseContainer(dialog,menu);
lfAlert->show();
lfAlert->release();
}
*/
class LFAlert : public CCLayer
{
private:
	bool mModal;
	bool mAutoDismiss;
	string mMessage;

	CCNode* mBaseNode;
	CCMenu* mMenu;

	bool m_bDialogCanTouch;

	// 判断当前的点是否可以释放出去作为点击点
	bool canTouch(CCPoint glPoint);

	// 下一帧修改meue的优先级。主要是等待menu的onEnter调用之后。
	// 这样才设置了触摸，才可以修改优先级
	void lazeyChangeMenuPriority(float t);
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
public:
	/**
	 *	构造一个模态的，什么都没有的对话框
	 *	需要显示东西，就添加就是了
	 */
	LFAlert(bool modal = true);

	// 设置自动关闭，关闭时候会发出消息dismissMsg
	// CCNotificationCenter::sharedNotificationCenter()->postNotification(mMessage.c_str(),this);
	void setAutoDismiss(const char* dismissMsg,bool autoDismiss = true);

	/**
	 *	添加一个基地，这个用于承载显示的类容
	 *	以及以后做动画的根基，默认会把base放在屏幕中间
	 *	base : 显示的基地
	 *	menu ：可点击的按钮
	 */
	LFAlert* setBaseContainer(CCNode* base,CCMenu* menu,bool useDefaultPos = true);


	//设置一个区域 canTouch在该区域返回false
	//dialog的区域
	void setZoneCanotTouch();


	// 显示
	void show(int z_order = 1024);
};
#endif