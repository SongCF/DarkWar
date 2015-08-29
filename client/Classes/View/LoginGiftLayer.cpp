#include "LoginGiftLayer.h"
#include "font.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "GameDirector.h"
#include "FX_CommonFunc.h"
#include "SuitPool.h"
#include "MessageDisplay.h"
#include "FXScrollMenu.h"
#include "AwardDisplay.h"
#include "ClientErrorCode.h"
#include "SystemGiftBagManager.h"
#include "GiftSysDisplay.h"
using namespace FX_CommonFunc;


#define LABEL_SIZE_min 20
#define LABEL_SIZE_max 24
#define LABEL_COLOR ccc3(195, 145, 96)

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

	Tag_Board_TaoZhuangShuxing,

};

bool compareByBonusId(const Flat_BonusInfo &a, const Flat_BonusInfo &b) {return a.bonus_id < b.bonus_id;}
bool compareByItemId(const ItemInfo& a, const ItemInfo& b){return a.type < b.type;}
bool compareByItemSuiPianId(const ItemSuipianInfo& a, const ItemSuipianInfo& b){return a.item_id < b.item_id;}

LoginGiftLayer* LoginGiftLayer::create()
{
	LoginGiftLayer *layer = new LoginGiftLayer;
	if(layer && layer->init())
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

bool LoginGiftLayer::init()
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

	CCSprite *titleSpr = CCSprite::create("denglujiangli_wenzi_meiridenglu.png");
	titleSpr->setPosition(ccp(tou->getContentSize().width / 2, tou->getContentSize().height / 2));
	tou->addChild(titleSpr);
	//body
	float delta_height = bodySize.height - di->getContentSize().height - tou->getContentSize().height;
	BaseSprite *body = BaseSprite::create("renwuliebiao_middle_bg.png");
	this->addChild(body);
	body->setPosition(ccp(bodySize.width/2, di->getContentSize().height + delta_height/2));
	body->setScaleY(delta_height / body->getContentSize().height);

	mGiftsVec = BonusManager::getManager()->getBonus(BonusType_DailyLogin);
	//排序
	{
		sort(mGiftsVec.begin(), mGiftsVec.end(), compareByBonusId);
	}

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
		, this, menu_selector(LoginGiftLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(fanhui, 1, Tag_MenuItem_Fanhui);
	fanhui->setPosition(ccp(di->getContentSize().width/2, di->getContentSize().height/2));

	return true;
}

void LoginGiftLayer::onEnter()
{
	BaseLayer::onEnter();

	mTableView->reloadData();
}

unsigned int LoginGiftLayer::numberOfCellsInTableView(CCTableView *table)
{
	return mGiftsVec.size();
}

void LoginGiftLayer::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{

}

CCSize LoginGiftLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	if (mGiftsVec.at(idx).is_get || mGiftsVec.at(idx).enable)
		return CCSize(630, 189);
	else 
		return CCSize(630, 155);
}

CCTableViewCell* LoginGiftLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
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

	if (table != mTableView) return cell;

	if (mGiftsVec.at(idx).is_get || mGiftsVec.at(idx).enable)  //已领取过的、 和今日的
	{

		CCMenu *boardMenu = CCMenu::create();
		boardMenu->setPosition(CCPointZero);
		
		BaseSprite *board = BaseSprite::create("shengjilibao_jinri_bg.png");
		cell->addChild(board, 1, Tag_Board);
		board->setPosition(ccp(size.width/2, size.height/2));

		board->addChild(boardMenu);
		//menu
		//title
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
				this, menu_selector(LoginGiftLayer::menuItemClicked_GetGift));
			menu->addChild(item, 0, idx);
			item->setPosition(ccp(550, 189-103));
			item->setUserData((void *)mGiftsVec.at(idx).bonus_id);

			temp.initWithFormat("%s", LFStrings::getValue("JinRiJiangLi").c_str());
		}
		else
		{
			temp.initWithFormat("%s%s", LFStrings::getValue("YiLing").c_str(), LFStrings::getValue("jiangli").c_str());
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
				menu_selector(LoginGiftLayer::menuItemClicked_Equip));
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
		/*BaseSprite *board = BaseSprite::create("denglujiangli_ciri_bg.png");
		cell->addChild(board, 1, Tag_Board);
		board->setPosition(ccp(size.width/2, size.height/2));
		//title
		temp.initWithFormat("%s", mGiftsVec.at(idx).description.c_str());
		CCLabelTTF *title1 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE_max);
		board->addChild(title1);
		title1->setPosition(ccp(72, 155-76));
		title1->setColor(LABEL_COLOR);
		temp.initWithFormat("%s%s", LFStrings::getValue("ShengJi").c_str(), LFStrings::getValue("jiangli").c_str());
		CCLabelTTF *title2 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE_min);
		board->addChild(title2);
		title2->setColor(LABEL_COLOR);
		title2->setPosition(ccp(72, 155-116));*/
		CCMenu *boardMenu = CCMenu::create();
		boardMenu->setPosition(CCPointZero);
		BaseSprite *board = BaseSprite::create("denglujiangli_ciri_bg.png");
		cell->addChild(board, 1, Tag_Board);
		board->setPosition(ccp(size.width/2, size.height/2));
		board->addChild(boardMenu);
		//title
		if (idx == 1)
			temp.initWithFormat("%s", LFStrings::getValue("mingri").c_str());
		else 
			temp.initWithFormat("%u%s", idx+1, LFStrings::getValue("ri").c_str());
		CCLabelTTF *title1 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE_max);
		board->addChild(title1);
		title1->setColor(LABEL_COLOR);
		title1->setPosition(ccp(73, 155-76));
		temp.initWithFormat("%s%s", LFStrings::getValue("Leiji").c_str(), LFStrings::getValue("DengLu").c_str());
		CCLabelTTF *title2 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE_min);
		board->addChild(title2);
		title2->setColor(LABEL_COLOR);
		title2->setPosition(ccp(72, 155-116));
		//
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

			bg->setPosition(ccp(253 + ( i + itemNum + itemNum2 ) *130,  155-71));
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
				menu_selector(LoginGiftLayer::menuItemClicked_Equip));
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

void LoginGiftLayer::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
	mTableView->setTouchEnabled(false);
	for (int i = 0; i < numberOfCellsInTableView(mTableView); i++)
	{
		CCTableViewCell *cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCSprite *board = (CCSprite*)cell->getChildByTag(Tag_Board);
			if (board)
			{
				CCMenu *menu_LingJiang = (CCMenu*)board->getChildByTag(Tag_Menu_LingJiang);
				if (menu_LingJiang) menu_LingJiang->setEnabled(false);
				CCMenu *menu_Equip = (CCMenu*)board->getChildByTag(Tag_Menu_Equip);
				if (menu_Equip) menu_Equip->setEnabled(false);
			}
		}
	}
}

void LoginGiftLayer::disableAllTouchEnd()
{
	mMainMenu->setEnabled(true);
	mTableView->setTouchEnabled(true);
	for (int i = 0; i < numberOfCellsInTableView(mTableView); i++)
	{
		CCTableViewCell *cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCSprite *board = (CCSprite*)cell->getChildByTag(Tag_Board);
			if (board)
			{
				CCMenu *menu_LingJiang = (CCMenu*)board->getChildByTag(Tag_Menu_LingJiang);
				if (menu_LingJiang) menu_LingJiang->setEnabled(true);
				CCMenu *menu_Equip = (CCMenu*)board->getChildByTag(Tag_Menu_Equip);
				if (menu_Equip) menu_Equip->setEnabled(true);
			}
		}
	}
}

void LoginGiftLayer::menuItemClicked_GetGift(CCObject *pSender)
{
	mClickedItemIdx = ((CCMenuItem*)pSender)->getTag();

	addObserver(callfuncO_selector(LoginGiftLayer::rsp_getGift), MSG_getBonusRsp);
	unsigned int bonusId = (unsigned int) ((CCMenuItemImage*)pSender)->getUserData();
	mCmdHlp->cmdGetBonus(BonusType_DailyLogin,mGiftsVec.at(0).bonus_id);
}

void LoginGiftLayer::rsp_getGift(CCObject *msg_f)
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
		GiftDisplay *layer = GiftDisplay::create(infoStr, mGiftsVec.at(mClickedItemIdx));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	}

	mGiftsVec = BonusManager::getManager()->getBonus(BonusType_DailyLogin);
	//排序
	{
		sort(mGiftsVec.begin(), mGiftsVec.end(), compareByBonusId);
	}
	mTableView->reloadData();

	if (m_oTarger)
	{
		(m_oTarger->*m_hResetDengLu)();
	}
}

void LoginGiftLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	//this->removeFromParent();
	MainLayerBase* footer = mFooterLayer;
	footer->dismmisPopUp();
	BaseLayer* topLayer = footer->getTopLayer();
	if(topLayer != NULL)
	{
		topLayer->refreshLayer();
	}
	else
	{
		footer->getContentLayer()->refreshLayer();
	}
	//GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
}

void LoginGiftLayer::menuItemClicked_Equip(CCObject *pSender)
{
	mCurTaoZhuang_Info = *(Flat_DuanzaoGroupZBInfo*)((CCMenuItemImage*)pSender)->getUserData();

	this->disableAllTouchBegin();

	BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_mianban.png");
	this->addChild(board, 10, Tag_Board_EquipDetail);
	refreshEquipBoard(board, mCurTaoZhuang_Info);
	board->setPosition(ccp(mWinSize.width/2, board->getContentSize().height/2 + 106));


	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 10, Tag_Menu_EquipDetail);
	menu->setPosition(CCPointZero);
	//	menu->setTouchPriority(-131);
	BaseSprite *norImg = BaseSprite::create("close.png");
	BaseSprite *selImg = BaseSprite::create("close_select.png");
	CCMenuItemSprite *item = CCMenuItemSprite::create(norImg, selImg, NULL, this, 
		menu_selector(LoginGiftLayer::menuItemClicked_equipDetailMenu));
	menu->addChild(item, 0, Tag_MenuItem_Close);
	item->setPosition(ccp(board->getContentSize().width-44+12, board->getContentSize().height-36+2));
	//底纹
	ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_XiangQing(
		mCurTaoZhuang_Info.zhuangbei_colour);
	BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
	diWen->setScale(0.7f);
	board->addChild(diWen);
	diWen->setPosition(ccp(123, 838-240+122));
	// 装备图片
	string equipfile = mCurTaoZhuang_Info.zhuangbei_pic;
	equipfile = "minute_" + equipfile + ".png";
	BaseSprite *s = BaseSprite::create(equipfile);
	diWen->addChild(s);
	s->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));
	//套装按钮
	if(SuitPool::getPool()->isSuit(mCurTaoZhuang_Info.zb_group_id))
	{
		string norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang.png";
		string selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_select.png";
		// 		if (ChuanqiPool::getPool()->isShenBing(mEquipVec.at(idx).zb_group_id))
		// 		{
		// 			norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing.png";
		// 			selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing_select.png";
		// 		}  
		CCMenuItemImage *taozhuangItem = CCMenuItemImage::create(
			norStr.c_str()
			, selStr.c_str()
			, this ,menu_selector(LoginGiftLayer::menuItemClicked_equipDetailMenu));
		menu->addChild(taozhuangItem, 0, Tag_MenuItem_TaoZhuang);
		taozhuangItem->setAnchorPoint(ccp(1, 0));
		taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()
			, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()));
		//taozhuangItem->setEnabled(false);
	}
}

void LoginGiftLayer::menuItemClicked_equipDetailMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	if(tag == Tag_MenuItem_Close)
	{
		this->removeChildByTag(Tag_Board_EquipDetail);
		disableAllTouchEnd();
	}
	else
	{
		((CCMenu*)((CCMenuItem*)pSender)->getParent())->setEnabled(false);

		//穿在身上的能触发套装属性的套装件数、 groupId
		vector<unsigned int> _vec;
		EquipmentTaoZhuangXiangQing *layer = new EquipmentTaoZhuangXiangQing(
			mCurTaoZhuang_Info.zb_group_id, mCurTaoZhuang_Info.zhuangbei_name, _vec, this);
		this->addChild(layer, 20, Tag_Board_TaoZhuangShuxing);
		layer->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2, layer->getContentSize().height/2 + 106));
		layer->release();
	}
}

void LoginGiftLayer::callbackInterface_TaoZhuangLayer()
{
	CCSprite *board = (CCSprite*)this->getChildByTag(Tag_Board_EquipDetail);
	if (board)
	{
		CCMenu *menu = (CCMenu*)board->getChildByTag(Tag_Menu_EquipDetail);
		if (menu) menu->setEnabled(true);


		this->removeChildByTag(Tag_Board_TaoZhuangShuxing);
	}
	else
	{
		CCLog("Error: %s 没有找到详情面板。", __FUNCTION__);
	}
}