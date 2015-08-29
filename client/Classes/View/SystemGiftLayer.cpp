#include "SystemGiftLayer.h"
#include "font.h"
#include "CS_Common.h"
#include "ImageManager.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "FXScrollMenu.h"
#include "AwardDisplay.h"
#include "ClientErrorCode.h"
#include "SystemGiftBagManager.h"
#include "FX_CommonFunc.h"
using namespace FX_CommonFunc;


enum 
{
	Tag_Board,
	Tag_Menu_LingJiang,
	Tag_MenuItem_Fanhui,
	Tag_Menu_Equip,

	Tag_Board_EquipDetail,
	Tag_Menu_EquipDetail,
	Tag_MenuItem_Close,
	Tag_MenuItem_TaoZhuang,

};


#define LABEL_SIZE_min 20
#define LABEL_SIZE_max 24
#define LABEL_COLOR ccc3(195, 145, 96)

extern bool compareByBonusId(const Flat_BonusInfo &a, const Flat_BonusInfo &b);
bool compareByItemId(const ItemInfo& a, const ItemInfo& b);

SystemGiftLayer* SystemGiftLayer::create()
{
	SystemGiftLayer *layer = new SystemGiftLayer;
	if (layer && layer->init())
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

bool SystemGiftLayer::init()
{
	CCSize bodySize = getBodySize();
	m_oTarger = NULL;
	//底
	BaseSprite *di = BaseSprite::create("renwuliebiao_down_bg.png");
	this->addChild(di, 10);
	di->setPosition(ccp(bodySize.width/2, di->getContentSize().height/2));
	//头
	BaseSprite *tou = BaseSprite::create("denglujiangli_up_bg.png");
	this->addChild(tou, 10);
	tou->setPosition(ccp(bodySize.width/2, bodySize.height - tou->getContentSize().height/2));

	CCSprite *titleSpr = CCSprite::create("denglujiangli_wenzi_dengjijiangli.png");
	titleSpr->setPosition(ccp(tou->getContentSize().width / 2, tou->getContentSize().height / 2));
	tou->addChild(titleSpr);
	//body
	float delta_height = bodySize.height - di->getContentSize().height - tou->getContentSize().height;
	BaseSprite *body = BaseSprite::create("renwuliebiao_middle_bg.png");
	this->addChild(body);
	body->setPosition(ccp(bodySize.width/2, di->getContentSize().height + delta_height/2));
	body->setScaleY(delta_height / body->getContentSize().height);

	mGiftsVec = BonusManager::getManager()->getBonus(BonusType_Level);
	{
		sort(mGiftsVec.begin(), mGiftsVec.end(), compareByBonusId);
	}
	sortLevelGif();
	mTableView = CCTableView::create(this, CCSizeMake(630, delta_height));
	this->addChild(mTableView, 1);
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	mTableView->setPosition(ccp((bodySize.width-630)/2, di->getContentSize().height));
	mTableView->setDelegate(this);

	//menu
	mMainMenu = CCMenu::create();
	di->addChild(mMainMenu);
	mMainMenu->setPosition(CCPointZero);
	//menu item
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(SystemGiftLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(fanhui, 1, Tag_MenuItem_Fanhui);
	fanhui->setPosition(ccp(di->getContentSize().width/2, di->getContentSize().height/2));

	return true;
}

CCTableViewCell* SystemGiftLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	
	CCTableViewCell *cell = table->dequeueCell();
	CCSize size = tableCellSizeForIndex(table, idx);
	CCString temp;

	if (cell != NULL)
	{	
		cell->removeAllChildren();
	}
	else 
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}

	if (mTableView != table) return cell;

	if (mGiftsVec.at(idx).is_get || mGiftsVec.at(idx).enable)  //已领取过的、 和今日的
	{
		BaseSprite *board = BaseSprite::create("shengjilibao_jinri_bg.png");
		cell->addChild(board, 1, Tag_Board);
		board->setPosition(ccp(size.width/2, size.height/2));
		//menu
		//title
		CCMenu *boardMenu = CCMenu::create();
		boardMenu->setPosition(CCPointZero);
		board->addChild(boardMenu);

		CCLabelTTF *titleLabel = NULL;
		if (mGiftsVec.at(idx).enable && ! mGiftsVec.at(idx).is_get)
		{
			FXScrollMenu *menu = FXScrollMenu::create(
				mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
			board->addChild(menu, 0, Tag_Menu_LingJiang);
			menu->setPosition(ccp(0,0));
			CCMenuItemImage *item = CCMenuItemImage::create(
				"a1.png",
				"a2.png",
				this, menu_selector(SystemGiftLayer::menuItemClicked_GetGift));
			menu->addChild(item, 0, idx);
			item->setPosition(ccp(550, 189-103));
			item->setUserData((void *)mGiftsVec.at(idx).bonus_id);

			//temp.initWithFormat("%s", mGiftsVec.at(idx).description.c_str());
			temp.initWithFormat("%d%s%s",mGiftsVec.at(idx).bonus_id, LFStrings::getValue("ji").c_str(), LFStrings::getValue("jiangli").c_str());
		}
		else
		{
			temp.initWithFormat("%d%s%s",mGiftsVec.at(idx).bonus_id, LFStrings::getValue("ji").c_str(), LFStrings::getValue("jiangli").c_str());
		}
		titleLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE_min);
		board->addChild(titleLabel);
		titleLabel->setPosition(ccp(board->getContentSize().width/2, 189-30));

		if (mGiftsVec.at(idx).vitality > 0)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(ItemsType_PresentVitality));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_PresentVitality));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u",CS::getItemName(ItemsType_PresentVitality).c_str(), mGiftsVec.at(idx).vitality);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(84, 189-103));
		}
		//
		int itemNum = mGiftsVec.at(idx).itemVec.size();
		sort(mGiftsVec.at(idx).itemVec.begin(), mGiftsVec.at(idx).itemVec.end(), compareByItemId);
		for (int i=0; i!=itemNum; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsVec.at(idx).itemVec.at(i).type));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mGiftsVec.at(idx).itemVec.at(i).type));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));

			temp.initWithFormat("%s x%u", CS::getItemName(mGiftsVec.at(idx).itemVec.at(i).type).c_str(), mGiftsVec.at(idx).itemVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE_min);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);

			board->addChild(bg);
			bg->setScale(0.8f);
			
			bg->setPosition(ccp(84+i*130, 189-103));
		}
		int itemNum2 = mGiftsVec.at(idx).itemDebrisVec.size();
		//sort(mGiftsLoginVec.at(0).itemDebrisVec.begin(), mGiftsLoginVec.at(0).itemDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum2; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsVec.at(idx).itemDebrisVec.at(i).item_id));
			string nameTemp = CS::getItemName(mGiftsVec.at(idx).itemDebrisVec.at(i).item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsVec.at(idx).itemDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsVec.at(idx).itemDebrisVec.at(i).item_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsVec.at(idx).itemDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);
			
			bg->setPosition(ccp(84 + ( i + itemNum ) *130, 189-103));
		}

		int itemNum3 = mGiftsVec.at(idx).equipDebrisVec.size();
		//sort(mGiftsLoginVec.at(0).equipDebrisVec.begin(), mGiftsLoginVec.at(0).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum3; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsVec.at(idx).equipDebrisVec.at(i).zb_group_id));
			string nameTemp = CS::getItemName(mGiftsVec.at(idx).equipDebrisVec.at(i).zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsVec.at(0).equipDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsVec.at(idx).equipDebrisVec.at(i).zb_group_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsVec.at(idx).equipDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);
			
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 ) *130,  189-103));
		}
		int itemNum4 = mGiftsVec.at(idx).giftBagVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum4; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsVec.at(idx).giftBagVec.at(i).id));

			BaseSprite *itemImg = BaseSprite::create(SystemGiftBagManager::getManager()->getGiftBagFileName(mGiftsVec.at(idx).giftBagVec.at(i).id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(mGiftsVec.at(idx).giftBagVec.at(i).id).c_str(),mGiftsVec.at(idx).giftBagVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);
			
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 + itemNum3 ) *130, 189-103));
		}
		int itemNum5 = 0;
		if (mGiftsVec.at(idx).ran_zb_count > 0)
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
			temp.initWithFormat("%s x%u", LFStrings::getValue("RandChuanQiZhuangBei").c_str(),mGiftsVec.at(idx).ran_zb_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);
			
			bg->setPosition(ccp(84 + ( i + itemNum + itemNum2 + itemNum3 + itemNum4) *130, 189-103));
		}
		//装备

		for (int i=0, count=mGiftsVec.at(idx).equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = mGiftsVec.at(idx).equipVec.at(i);

			string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(oneEq.zhuangbei_colour);
			CCMenuItemImage *eqDiwenImg = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str(), this,
				menu_selector(SystemGiftLayer::menuItemClicked_Equip));
			boardMenu->addChild(eqDiwenImg);
			eqDiwenImg->setUserData(&mGiftsVec.at(idx).equipVec.at(i));
			
			eqDiwenImg->setPosition(ccp(84+(itemNum+i + itemNum2 + itemNum3 + itemNum4 + itemNum5)*130, 189-103));
			eqDiwenImg->setScale(0.8f);

			string equipfile = oneEq.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			eqDiwenImg->addChild(equipImg);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(eqDiwenImg->getContentSize().width/2, eqDiwenImg->getContentSize().height/2));
		}
	}
	else 
	{
		CCMenu *boardMenu = CCMenu::create();
		boardMenu->setPosition(CCPointZero);
		
		BaseSprite *board = BaseSprite::create("denglujiangli_ciri_bg.png");
		cell->addChild(board, 1, Tag_Board);
		board->setPosition(ccp(size.width/2, size.height/2));

		board->addChild(boardMenu);
		//title
		temp.initWithFormat("%d%s", mGiftsVec.at(idx).bonus_id,LFStrings::getValue("ji").c_str());
		CCLabelTTF *title1 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE_max);
		board->addChild(title1);
		title1->setPosition(ccp(72, 155-76));
		title1->setColor(LABEL_COLOR);
		temp.initWithFormat("%s%s", LFStrings::getValue("ShengJi").c_str(), LFStrings::getValue("jiangli").c_str());
		CCLabelTTF *title2 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE_min);
		board->addChild(title2);
		title2->setColor(LABEL_COLOR);
		title2->setPosition(ccp(72, 155-116));

		//
		if (mGiftsVec.at(idx).vitality > 0)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(ItemsType_PresentVitality));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_PresentVitality).c_str());
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u",CS::getItemName(ItemsType_PresentVitality).c_str(), mGiftsVec.at(idx).vitality);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);
			bg->setPosition(ccp(253, 155-71));
		}
		//
		int itemNum = mGiftsVec.at(idx).itemVec.size();
		sort(mGiftsVec.at(idx).itemVec.begin(), mGiftsVec.at(idx).itemVec.end(), compareByItemId);
		for (int i=0; i!=itemNum; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsVec.at(idx).itemVec.at(i).type));

			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mGiftsVec.at(idx).itemVec.at(i).type));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));

			temp.initWithFormat("%s x%u", CS::getItemName(mGiftsVec.at(idx).itemVec.at(i).type).c_str(), mGiftsVec.at(idx).itemVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE_min);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);

			board->addChild(bg);
			bg->setScale(0.8f);

			bg->setPosition(ccp(253+i*130, 155-71));
		}
		int itemNum2 = mGiftsVec.at(idx).itemDebrisVec.size();
		//sort(mGiftsLoginVec.at(0).itemDebrisVec.begin(), mGiftsLoginVec.at(0).itemDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum2; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsVec.at(idx).itemDebrisVec.at(i).item_id));
			string nameTemp = CS::getItemName(mGiftsVec.at(idx).itemDebrisVec.at(i).item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsVec.at(idx).itemDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsVec.at(idx).itemDebrisVec.at(i).item_id));
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsVec.at(idx).itemDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);

			bg->setPosition(ccp(253 + ( i + itemNum ) *130, 155-71));
		}

		int itemNum3 = mGiftsVec.at(idx).equipDebrisVec.size();
		//sort(mGiftsLoginVec.at(0).equipDebrisVec.begin(), mGiftsLoginVec.at(0).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum3; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsVec.at(idx).equipDebrisVec.at(i).zb_group_id));
			string nameTemp = CS::getItemName(mGiftsVec.at(idx).equipDebrisVec.at(i).zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mGiftsVec.at(0).equipDebrisVec.at(i).suipian_id);
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(mGiftsVec.at(idx).equipDebrisVec.at(i).zb_group_id).c_str());
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", nameTemp.c_str(), mGiftsVec.at(idx).equipDebrisVec.at(i).suipian_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);

			bg->setPosition(ccp(253 + ( i + itemNum + itemNum2 ) *130,  155-71));
		}
		int itemNum4 = mGiftsVec.at(idx).giftBagVec.size();
		//sort(mGiftsChooseVec.at(index).equipDebrisVec.begin(), mGiftsChooseVec.at(index).equipDebrisVec.end(), compareByItemId);
		for (int i=0; i!=itemNum4; ++i)
		{
			BaseSprite *bg = BaseSprite::create(ImageManager::getManager()->getItemBoardFileName(mGiftsVec.at(idx).giftBagVec.at(i).id));

			BaseSprite *itemImg = BaseSprite::create(SystemGiftBagManager::getManager()->getGiftBagFileName(mGiftsVec.at(idx).giftBagVec.at(i).id).c_str());
			bg->addChild(itemImg);
			itemImg->setPosition(ccp(bg->getContentSize().width/2, bg->getContentSize().height/2));
			CCString temp;
			temp.initWithFormat("%s x%u", SystemGiftBagManager::getManager()->getGiftBagName(mGiftsVec.at(idx).giftBagVec.at(i).id).c_str(),mGiftsVec.at(idx).giftBagVec.at(i).count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);

			bg->setPosition(ccp(253 + ( i + itemNum + itemNum2 + itemNum3 ) *130, 155-71));
		}
		int itemNum5 = 0;
		if (mGiftsVec.at(idx).ran_zb_count > 0)
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
			temp.initWithFormat("%s x%u", LFStrings::getValue("RandChuanQiZhuangBei").c_str(),mGiftsVec.at(idx).ran_zb_count);
			CCLabelTTF *nameLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
			bg->addChild(nameLabel);
			nameLabel->setPosition(ccp(bg->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
			nameLabel->setScale(1 / 0.8f);
			nameLabel->setColor(ccWHITE);
			board->addChild(bg);
			bg->setScale(0.8f);

			bg->setPosition(ccp(253 + ( i + itemNum + itemNum2 + itemNum3 + itemNum4) *130, 155-71));
		}
		//装备

		for (int i=0, count=mGiftsVec.at(idx).equipVec.size(); i<count; ++i)
		{
			Flat_DuanzaoGroupZBInfo oneEq = mGiftsVec.at(idx).equipVec.at(i);

			string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(oneEq.zhuangbei_colour);
			CCMenuItemImage *eqDiwenImg = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str(), this,
				menu_selector(SystemGiftLayer::menuItemClicked_Equip));
			boardMenu->addChild(eqDiwenImg);
			eqDiwenImg->setUserData(&mGiftsVec.at(idx).equipVec.at(i));

			eqDiwenImg->setPosition(ccp(253+(itemNum+i + itemNum2 + itemNum3 + itemNum4 + itemNum5)*130, 155-71));
			eqDiwenImg->setScale(0.8f);

			string equipfile = oneEq.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			eqDiwenImg->addChild(equipImg);
			equipImg->setScale(0.23f);
			equipImg->setPosition(ccp(eqDiwenImg->getContentSize().width/2, eqDiwenImg->getContentSize().height/2));
		}
		
	}
	return cell;
}

void SystemGiftLayer::menuItemClicked_GetGift(CCObject *pSender)
{
	mClickedItemIdx = ((CCMenuItem*)pSender)->getTag();

	addObserver(callfuncO_selector(SystemGiftLayer::rsp_getGift), MSG_getBonusRsp);
	unsigned int bonusId = (unsigned int) ((CCMenuItemImage*)pSender)->getUserData();
	mCmdHlp->cmdGetBonus(BonusType_Level,mGiftsVec.at(mClickedItemIdx).bonus_id);
}

void SystemGiftLayer::rsp_getGift(CCObject *msg_f)
{
	removeObserver(MSG_getBonusRsp);
	GameDirector::getDirector()->hideWaiting();

	Flat_GetBonusRsp* data = (Flat_GetBonusRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code)
	{
		if (data->err_code == CEC_NoMoreFreeSpaceToStoreEquip)
		{
			EquipBagNotEnoughDialog *layer = new EquipBagNotEnoughDialog;
			layer->showGoToFenJieDialog();
		}
		else
		{
			MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		}
		return;
	}


	//弹出奖励的框
	{
		CCString infoStr = LFStrings::getValue("HuoDe") + LFStrings::getValue("jiangli");
		AwardDisplay *layer = AwardDisplay::create(infoStr, mGiftsVec.at(mClickedItemIdx).itemVec, mGiftsVec.at(mClickedItemIdx).equipVec);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	}


	mGiftsVec = BonusManager::getManager()->getBonus(BonusType_Level);
	{
		sort(mGiftsVec.begin(), mGiftsVec.end(), compareByBonusId);
	}
	sortLevelGif();
	mTableView->reloadData();

	if (m_oTarger)
	{
		(m_oTarger->*m_hResetDengJi)();
	}
}
void SystemGiftLayer::sortLevelGif()
{
	vector<Flat_BonusInfo> getList;//所有领取的
	vector<Flat_BonusInfo> notGetList;//未领取的

	for (vector<Flat_BonusInfo>::iterator iter = mGiftsVec.begin() ; iter != mGiftsVec.end() ; iter++)
	{
		if (iter->is_get)
		{
			getList.push_back(*iter);
		}
		else
		{
			notGetList.push_back(*iter);
		}
	}
	mGiftsVec.clear();
	for (vector<Flat_BonusInfo>::reverse_iterator iter = getList.rbegin() ; iter != getList.rend() ; iter++)
	{
		mGiftsVec.push_back(*iter);
		break;
	}
	for (vector<Flat_BonusInfo>::iterator iter = notGetList.begin() ; iter != notGetList.end() ; iter++)
	{
		mGiftsVec.push_back(*iter);
	}
}