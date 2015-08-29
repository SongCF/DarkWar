#include "TanSuoResultLayer.h"
#include "font.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "SystemGiftBagManager.h"
#include "FX_CommonFunc.h"
#include "MyselfManager.h"
#include "ChatContentManager.h"
using namespace FX_CommonFunc;

TanSuoResultLayer* TanSuoResultLayer::create(CCString info,vector<ItemInfo> bouns,bool isJieFeng)
{
	TanSuoResultLayer* layer = new TanSuoResultLayer(info, bouns,isJieFeng);
	if(layer != NULL)
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

TanSuoResultLayer::TanSuoResultLayer(CCString info, vector<ItemInfo> itemInfo,bool isJieFeng)
{
	//CCAssert(bouns.size() > 0 , "gift data size less than 0");

	//int allNum = items.size() + equips.size();
	m_sInfo = info;
	m_iItems = itemInfo;
	m_bIsJieFeng = isJieFeng;
	
	CCSize visSize = CCDirector::sharedDirector()->getWinSize();
	BaseSprite* dialog = BaseSprite::create("juqing_duihuakuang.png");
	dialog->setPosition(ccp(visSize.width/2, visSize.height/2  - dialog->getContentSize().height / 2 + 50));

	CCLabelTTF *title = CCLabelTTF::create(m_sInfo.getCString(),fontStr_kaiti,24.0f);
	title->setColor(ccWHITE);
	title->setPosition(ccp(dialog->getContentSize().width / 2 , dialog->getContentSize().height - title->getContentSize().height - 20));
	dialog->addChild(title);

	string fileName;
	Profession proId = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;
	if(proId == Profession_Yemanren)
		fileName = "yemanren";
	else if(proId == Profession_Chike)
		fileName = "cike";
	else if(proId == Profession_Fashi)
		fileName = "fashi";
	fileName = ChatContentManager::sharedChatContentManager()->getNpcFileNameByNpcName(fileName);

	CCSprite *mySpr = CCSprite::create(fileName.c_str());
	mySpr->setPosition(ccp(dialog->getContentSize().width - mySpr->getContentSize().width / 2 - 10, dialog->getContentSize().height + mySpr->getContentSize().height / 2 - 15));
	dialog->addChild(mySpr);

	CCPoint pos = ccp(20,dialog->getContentSize().height / 2 - 15);

	int allItemNum = m_iItems.size();

	if (m_bIsJieFeng)
	{
		allItemNum ++;
	}

	pos.x = dialog->getContentSize().width / 2 - ( allItemNum - 1 ) * 130 / 2;

	//if (bouns.is_get || bouns.enable)  //已领取过的、 和今日的
	{

		int itemNum = m_iItems.size();
		for (int i=0; i!=itemNum; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(m_iItems.at(i).type));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(m_iItems.at(i).type));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", CS::getItemName(m_iItems.at(i).type).c_str(), m_iItems.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			dialog->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(pos);

			pos.x += 130;
		}
		if (m_bIsJieFeng)
		{
			CCSprite *jieFengSpr = CCSprite::create("xunbao_fengyinshi_pohuai.png");
			jieFengSpr->setPosition(pos);
			jieFengSpr->setScale(0.75f);
			dialog->addChild(jieFengSpr);
			jieFengSpr->setPositionY(jieFengSpr->getPositionY() - 10);
			pos.x += 130;
		}
	}

	//确认 取消按键  
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	{
		CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&TanSuoResultLayer::menuItemOkClicked));

		menu->addChild(menuItemOk, 1);
		menuItemOk->setPosition(ccp(dialog->getContentSize().width/2,70));
		menuItemOk->setOpacity(0);

		CCSize winsize = CCDirector::sharedDirector()->getWinSize();
		menuItemOk->setScaleX(winsize.width / menuItemOk->getContentSize().width);
		menuItemOk->setScaleY(winsize.height / menuItemOk->getContentSize().height);
	}

	//
	{
		this->setTag(TAG_TanSuoResultLayerer_Layer);
		this->setZOrder(1025);
		this->setBaseContainer(dialog,menu,false);
	}
}

void TanSuoResultLayer::menuItemOkClicked(CCObject* pSender)
{
	this->removeFromParent();
	//if (mCallback) mCallback->TanSuoResultLayerCallback_func();
}