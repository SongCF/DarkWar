#include "GetTiLiLayer.h"
#include "BaseLayer.h"
#include "GameDirector.h"
#include "font.h"
#include "WuPinManager.h"
#include "MyselfManager.h"
#include "protocol.h"
enum TAG{
	Tag_Buy = 333,
	Tag_Close, 

	Tag_UpVip,
	Tag_ChongZhi,
	Tag_Background
};


GetTiLiLayer * GetTiLiLayer::create(GetTiLiCallBack *callback, bool notEnough)
{
	GetTiLiLayer* layer = new GetTiLiLayer(callback, notEnough);
	if (layer)
	{
		layer->autorelease();
	}
	else 
	{
		CC_SAFE_DELETE(layer);
		layer = NULL;
	}

	return layer;
}

GetTiLiLayer::GetTiLiLayer(GetTiLiCallBack *callback, bool notEnough)
	:mLabel(NULL)
	, mMenu(NULL)
	, mCallBack(callback)
{
	//体力不足的窗口
	CCSize visSize = CCDirector::sharedDirector()->getVisibleSize();
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->setPosition(ccp(visSize.width/2, visSize.height/2));

	//确认 取消按键  
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	{
		CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
			"shangdian_goumai.png",
			"shangdian_goumai_select.png",
			this,SEL_MenuHandler(&GetTiLiLayer::menuItemClicked_BuyOrClose));
		CCMenuItemImage* menuItemCancel = CCMenuItemImage::create(
			"shangpin_goumai_guanbi.png",
			"shangpin_goumai_guanbi_select.png",
			this,SEL_MenuHandler(&GetTiLiLayer::menuItemClicked_BuyOrClose));

		menu->addChild(menuItemOk, 1, Tag_Buy);
		menu->addChild(menuItemCancel, 1, Tag_Close);

		menuItemOk->setPosition(ccp(dialog->getContentSize().width/3,70));
		menuItemCancel->setPosition(ccp(dialog->getContentSize().width/3*2, 70));

		mMenu = menu;
		mMenu->retain();
	}


	//label
	{
		char buf[120];
		//当前体力已不足！\n剩余购买次数：%d\n花费%d金币购买体力？
		int tiliCiShu = MyselfManager::getManager()->getRuntionData()->buyTiLiTimeLeft;
		int needNum = Vitality_Price;
		if (notEnough)
			sprintf(buf, (LFStrings::getValue("TiLiBuZu")+"\n"+LFStrings::getValue("tilibuzhutishi")).c_str(), tiliCiShu, needNum);
		else 
			sprintf(buf, LFStrings::getValue("tilibuzhutishi").c_str(), tiliCiShu, needNum);
		CCLabelTTF* label = CCLabelTTF::create(buf, fontStr_kaiti,30,CCSize(430, 0), kCCTextAlignmentCenter);
		label->setAnchorPoint(ccp(0.5f, 0.5f));
		label->setPosition(ccp(dialog->getContentSize().width/2, 226));
		label->setColor(ccc3(0xff, 0xd7, 0x00));
		dialog->addChild(label, 1);
		mLabel = label;
		mLabel->retain();
	}


	//
	{
		this->setTag(Tag_Background);
		this->setZOrder(1024);
		this->setBaseContainer(dialog,menu);
	}


}

GetTiLiLayer::~GetTiLiLayer()
{
	CC_SAFE_RELEASE(mLabel);
	CC_SAFE_RELEASE(mMenu);
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_restoreVitalityRsp);
}

void GetTiLiLayer::menuItemClicked_BuyOrClose(CCObject* pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();
	if(tag == Tag_Buy)
	{
		//次数不足
		if (MyselfManager::getManager()->getRuntionData()->buyTiLiTimeLeft <= 0)
		{
			CCMenuItemImage *tsVip = CCMenuItemImage::create(
				"shangpin_goumai_tishengvip.png",
				"shangpin_goumai_tishengvip_select.png",
				this, menu_selector(GetTiLiLayer::menuItemClicked_UpVip));
			mMenu->addChild(tsVip, 1, Tag_UpVip);
			tsVip->setPosition(mMenu->getChildByTag(Tag_Buy)->getPosition());
			mMenu->getChildByTag(Tag_Buy)->setVisible(false);
			mLabel->setString(LFStrings::getValue("tilihuifu_TiShengVip").c_str());
			return;
		}
		//金币不足
		else if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() < Vitality_Price)
		{
			CCMenuItemImage *czItem = CCMenuItemImage::create(
				"shangpin_goumai_chongzhi.png",
				"shangpin_goumai_chongzhi_select.png",
				this, menu_selector(GetTiLiLayer::menuItemClicked_UpVip));
			mMenu->addChild(czItem, 1, Tag_ChongZhi);
			czItem->setPosition(mMenu->getChildByTag(Tag_Buy)->getPosition());
			mMenu->getChildByTag(Tag_Buy)->setVisible(false);
			mLabel->setString(LFStrings::getValue("tilihuifujinbibuzu").c_str());
			return;
		}


		//金币/次数 足够
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this,SEL_CallFuncO(&GetTiLiLayer::huifuTiLi),MSG_chargeVitalityRsp,NULL);
		this->setVisible(false);
		CmdHelper::getHelper()->cmdchargeVitality();
	}
	else if (tag == Tag_Close)
	{
		this->removeFromParent();
	}
}

void GetTiLiLayer::menuItemClicked_UpVip(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	GameDirector::getDirector()->mainScene(mainlayer_ChongZhi);
}

void GetTiLiLayer::huifuTiLi(CCObject* msg)
{
	GameDirector::getDirector()->hideWaiting();
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_chargeVitalityRsp);
	this->setVisible(true);


	mMenu->getChildByTag(Tag_Close)->setPositionX(mMenu->getParent()->getContentSize().width/2);
	mMenu->getChildByTag(Tag_Buy)->setVisible(false);


	SPCmd_ChargeVitalityRsp* rsp = (SPCmd_ChargeVitalityRsp*)((MSG_Rsp*)msg)->getData();
	if (rsp->err_code != Success)
	{
		string msg = CmdHelper::getHelper()->getMessage(rsp->err_code);
		mLabel->setString(msg.c_str());    
	}
	else
	{
		mLabel->setString(LFStrings::getValue("tilihuifu").c_str());
	}

	if (mCallBack != NULL)
		mCallBack->getTiliCallBackFunc();
}