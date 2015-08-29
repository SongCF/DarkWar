
#pragma once

#include "cocos2d.h"
#include "LFAlert.h"
#include "font.h"
#include "LFStrings.h"
#include "BaseSprite.h"
#include "GameDirector.h"
#include <string>
using std::string;
using namespace cocos2d;

#define TAG_MessageDisplayer_Layer 4643+10001

class MessageDisplayCallBackInterface
{
public: virtual void messageDialogEndCallback() = 0;
};

class MessageDisplay : public LFAlert
{
public:

	static MessageDisplay* create(string infoStr = LFStrings::getValue("errordisplay"), MessageDisplayCallBackInterface* callback = NULL)
	{
		MessageDisplay* layer = new MessageDisplay(infoStr, callback);
		if(layer != NULL)
		{
			layer->autorelease();
		}
		else
		{
			CC_SAFE_DELETE(layer);
			layer  = NULL;
		}

		return layer;
	}

	~MessageDisplay()
	{
		CC_SAFE_RELEASE(mLabel);
		CC_SAFE_RELEASE(mMenu);
	}

	MessageDisplay(string infoStr, MessageDisplayCallBackInterface* callback)
	{
		mCallback = callback;

		CCSize visSize = CCDirector::sharedDirector()->getWinSize();
		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
		dialog->setPosition(ccp(visSize.width/2, visSize.height/2));

		//确认 取消按键  
		CCMenu* menu = CCMenu::create();
		menu->setPosition(CCPointZero);
		{
			CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
				"shangpin_goumai_queren.png",
				"shangpin_goumai_queren_select.png",
				this,SEL_MenuHandler(&MessageDisplay::menuItemOkClicked));
			

			menu->addChild(menuItemOk, 1);

			menuItemOk->setPosition(ccp(dialog->getContentSize().width/2,70));

			mMenu = menu;
			mMenu->retain();
			menu->setTouchPriority(-1050);
		}


		//label
		{
			CCLabelTTF* label = CCLabelTTF::create(infoStr.c_str(), fontStr_kaiti,36,CCSize(400, 0), kCCTextAlignmentCenter);
			label->setAnchorPoint(ccp(0.5, 1));
			label->setPosition(ccp(dialog->getContentSize().width/2, 260));
			label->setColor(fonColor_JingGao);
			dialog->addChild(label, 1);
			mLabel = label;
			mLabel->retain();
		}


		//
		{
			this->setTag(TAG_MessageDisplayer_Layer);
			this->setZOrder(1025);
			this->setBaseContainer(dialog,menu);
		}
	}

	void menuItemOkClicked(CCObject* pSender)
	{
		if(mCallback != NULL)
			mCallback->messageDialogEndCallback();

		this->removeFromParent();
	}

protected:

	CCLabelTTF* mLabel;
	CCMenu* mMenu;

	MessageDisplayCallBackInterface *mCallback;
};

class EquipBagNotEnoughDialog : public CCLayer
{
public:
	EquipBagNotEnoughDialog() {CCLayer::init();}
	void showGoToFenJieDialog()
	{
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();

		//弹框背景
		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
		dialog->setPosition(ccp(winSize.width/2, winSize.height/2));
		//提示
		{
			CCLabelTTF* lable = CCLabelTTF::create(LFStrings::getValue("ZbShangXian_GotoFenJie").c_str()
				, fontStr_kaiti,30,CCSize(340, 0), kCCTextAlignmentLeft);
			dialog->addChild(lable);
			lable->setPosition(ccp(0,0));
			lable->setPosition(ccp(dialog->getContentSize().width/2, 228));
		}

		//按键
		CCMenu* menu = CCMenu::create();
		menu->setPosition(ccp(0,0));
		{
			CCMenuItemImage* item = CCMenuItemImage::create(
				"mianban_zhuangbei_qufenjie.png",
				"mianban_zhuangbei_qufenjie_select.png",
				this, menu_selector(EquipBagNotEnoughDialog::menuItemClicked));
			menu->addChild(item);
			item->setPosition(ccp(dialog->getContentSize().width/2, 70));
		}
		//弹框
		{
			LFAlert* lfAlert = new LFAlert();
			lfAlert->setBaseContainer(dialog,menu);
			lfAlert->setAutoDismiss("");
			this->addChild(lfAlert);
			lfAlert->release();
		}

		CCDirector::sharedDirector()->getRunningScene()->addChild(this, 1024);
	}

protected:
	void menuItemClicked(CCObject *pSender)
	{
		GameDirector::getDirector()->goToGongJiangFenJie();
	}
};