#include "GiftSysDisplay.h"
#include "font.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "SystemGiftBagManager.h"
#include "FX_CommonFunc.h"
#include "MyselfManager.h"
using namespace FX_CommonFunc;

GiftDisplay* GiftDisplay::create(CCString info,Flat_BonusInfo bouns, GiftSysDisplayCallBack_Interface* callbackfunc)
{
	GiftDisplay* layer = new GiftDisplay(info, bouns, callbackfunc);
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

GiftDisplay::GiftDisplay(CCString info, Flat_BonusInfo bouns, GiftSysDisplayCallBack_Interface* callbackfunc)
{
	//CCAssert(bouns.size() > 0 , "gift data size less than 0");

	mCallback = callbackfunc;
	//int allNum = items.size() + equips.size();

	CCSize visSize = CCDirector::sharedDirector()->getWinSize();
	BaseSprite* dialog = BaseSprite::create("gongming_bg.png");
	dialog->setPosition(ccp(visSize.width/2, visSize.height/2));
	CCPoint pos = ccp(20,dialog->getContentSize().height / 2);
	{
		CCLabelTTF *ttf = CCLabelTTF::create(info.getCString(),fontStr_kaiti,20.0f);
		ttf->setColor(ccWHITE);
		ttf->setPosition(ccp(dialog->getContentSize().width / 2 , dialog->getContentSize().height - ttf->getContentSize().height - 50));
		dialog->addChild(ttf,10);
	}
	
	int allItemNum = bouns.itemDebrisVec.size() + bouns.equipDebrisVec.size() + bouns.equipVec.size() + bouns.giftBagVec.size() + bouns.itemVec.size();
	if (bouns.ran_zb_count > 0)
	{
		allItemNum ++;
	}
	pos.x = dialog->getContentSize().width / 2 - ( allItemNum - 1 ) * 130 / 2;
	
	//if (bouns.is_get || bouns.enable)  //已领取过的、 和今日的
	{
		//addChild(dialog, 1);
		//dialog->setPosition(ccp(pos.x,pos.y - dialog->getContentSize().height / 2));

		if (bouns.vitality > 0)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(ItemsType_PresentVitality));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_PresentVitality));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u",CS::getItemName(ItemsType_PresentVitality).c_str(), bouns.vitality);
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

		int itemNum = bouns.itemVec.size();
		for (int i=0; i!=itemNum; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(bouns.itemVec.at(i).type));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(bouns.itemVec.at(i).type));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", CS::getItemName(bouns.itemVec.at(i).type).c_str(), bouns.itemVec.at(i).count);
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

		int itemNum2 = bouns.itemDebrisVec.size();
		//sort(bouns.itemDebrisVec.begin(), bouns.itemDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum2; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(bouns.itemDebrisVec.at(i).item_id));
			string nameTemp = CS::getItemName(bouns.itemDebrisVec.at(i).item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(bouns.itemDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(bouns.itemDebrisVec.at(i).item_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), bouns.itemDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			dialog->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(pos);

			pos.x += 130;;
		}

		int itemNum3 = bouns.equipDebrisVec.size();
		//sort(bouns.equipDebrisVec.begin(), bouns.equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum3; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(bouns.equipDebrisVec.at(i).zb_group_id));
			string nameTemp = CS::getItemName(bouns.equipDebrisVec.at(i).zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(bouns.equipDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(bouns.equipDebrisVec.at(i).zb_group_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), bouns.equipDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			dialog->addChild(bg);
			bg->setPosition(pos);

			pos.x += 130;
		}
		int itemNum4 = bouns.giftBagVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum4; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(bouns.giftBagVec.at(i).id));

			BaseSprite *itemImg = BaseSprite::create(SystemGiftBagManager::getManager()->getGiftBagFileName(bouns.giftBagVec.at(i).id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			unsigned int count = bouns.giftBagVec.at(i).count;
			if (bouns.vip_level > MyselfManager::getManager()->getCurrentVipLevel())
			{
				count *= bouns.multiple;
			}
			
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(bouns.giftBagVec.at(i).id).c_str(),count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			dialog->addChild(bg);
			bg->setPosition(pos);

			pos.x += 130;
		}
		int itemNum5 = 0;
		if (bouns.ran_zb_count > 0)
		{
			itemNum5 = 1;
		}


		for (int i=0; i!=itemNum5; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getGiftBagBoardName(3));

			BaseSprite *itemImg = BaseSprite::create("daoju_libao_3.png");
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", LFStrings::getValue("RandChuanQiZhuangBei").c_str(),bouns.ran_zb_count);
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
		//装备

		for (int i=0, count=bouns.equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = bouns.equipVec.at(i);

			string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(oneEq.zhuangbei_colour);
			BaseSprite *eqDiwenImg = BaseSprite::create(diwenStr.c_str());
			dialog->addChild(eqDiwenImg);
			eqDiwenImg->setUserData(&bouns.equipVec.at(i));
			eqDiwenImg->setPosition(pos);
			eqDiwenImg->setScale(0.8f);

			string equipfile = oneEq.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			eqDiwenImg->addChild(equipImg);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(eqDiwenImg->getContentSize().width/2, eqDiwenImg->getContentSize().height/2));

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
			this,SEL_MenuHandler(&GiftDisplay::menuItemOkClicked));

		menu->addChild(menuItemOk, 1);
		menuItemOk->setPosition(ccp(dialog->getContentSize().width/2,70));
		menuItemOk->setOpacity(0);

		CCSize winsize = CCDirector::sharedDirector()->getWinSize();
		menuItemOk->setScaleX(winsize.width / menuItemOk->getContentSize().width);
		menuItemOk->setScaleY(winsize.height / menuItemOk->getContentSize().height);
	}

	//
	{
		this->setTag(TAG_GiftDisplayer_Layer);
		this->setZOrder(1025);
		this->setBaseContainer(dialog,menu);
	}
}

void GiftDisplay::menuItemOkClicked(CCObject* pSender)
{
	this->removeFromParent();
	if (mCallback) mCallback->giftDisplayCallback_func();
}