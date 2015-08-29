#include "DebrisSynthesiseLayer.h"
#include "font.h"
#include "GameDirector.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "SkillTree.h"
#include "SuitPool.h"
#include "ShenBingPool.h"
#include "flat.h"
#include "MessageDisplay.h"
#include "GuideLayerBase.h"
#include "FXScrollMenu.h"
#include "FX_CommonFunc.h"
#include "EquipmentManager.h"

using namespace FX_CommonFunc;

#define LABEL_SIZE 23
#define LABLE_COLOR ccc3(255, 228, 145)

#define OneContentPage_MaxNum 16
#define EQUIP_SCALE 0.3f

#define POS_1 ccp(104, 365-302)
#define POS_2 ccp(45, 365-144)
#define POS_3 ccp(182, 365-41)
#define POS_4 ccp(316, 365-143)
#define POS_5 ccp(258, 365-302)

enum 
{
	Tag_Item_Books = 1,
	Tag_Item_Material,
	Tag_Item_Equip,
	Tag_Item_FanHui,

	Tag_Label_Title,

	Tag_Label_DebrisName_ = 11,  //11 - 15
	Tag_Board_DebrisNum_  = 21,  //21 - 25
	Tag_Sprite_Debris     = 31,  //31 - 35

	Tag_Layer_Content = 100,
	Tag_Menu_NextLast,
	Tag_ContentLayer_CellMenu,
	Tag_Layer_Synthesise,
	Tag_SynthesiseMenu,
	Tag_SynthesiseMenuItem_Synthesise,
	Tag_SynthesiseMenuItem_Back,

	Tag_Img_SynBoard,
	Tag_Img_SynEquipOrItem,
};

DebrisSynthesiseLayer *DebrisSynthesiseLayer::create()
{
	DebrisSynthesiseLayer *layer = new DebrisSynthesiseLayer;
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

DebrisSynthesiseLayer::DebrisSynthesiseLayer()
{
	mBg = NULL;
	mMainMenu = NULL;
}

bool DebrisSynthesiseLayer::init()
{
	BaseLayer::init();
	//

	CC_SAFE_RELEASE(mBg);
	CC_SAFE_RELEASE(mMainMenu);
	mBg = BaseSprite::create("sphc_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(CCPointZero);

	CCMenuItemImage *itemMaterial = CCMenuItemImage::create(
		"sphc_anniu_cailiao.png"
		, "sphc_anniu_cailiao_select.png"
		, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(itemMaterial, 1, Tag_Item_Material);
	itemMaterial->setPosition(ccp(570, 838-291));

	CCMenuItemImage *itemBooks = CCMenuItemImage::create(
		"sphc_anniu_shuye.png"
		, "sphc_anniu_shuye_select.png"
		, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(itemBooks, 1, Tag_Item_Books);
	itemBooks->setPosition(ccp(569, 838-431));

	CCMenuItemImage *itemEquip = CCMenuItemImage::create(
		"sphc_anniu_shenbing.png"
		, "sphc_anniu_shenbing_select.png"
		, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(itemEquip, 1, Tag_Item_Equip);
	itemEquip->setPosition(ccp(570, 838-571));

	CCMenuItemImage *item = CCMenuItemImage::create(
		"sphc_anniu_fanhui.png"
		, "sphc_anniu_fanhui_select.png"
		, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainBack));
	mMainMenu->addChild(item, 1, Tag_Item_FanHui);
	item->setPosition(ccp(262, 838-770));

	CCLabelTTF *labelTitle = CCLabelTTF::create("", fontStr_kaiti, LABEL_SIZE);
	labelTitle->setColor(ccc3(90, 28, 23));
	mBg->addChild(labelTitle, 1, Tag_Label_Title);
	labelTitle->setPosition(ccp(257, 838-132));

	BaseSprite *contentLayer = BaseSprite::create();
	mBg->addChild(contentLayer, 0, Tag_Layer_Content);
	contentLayer->setPosition(ccp(0,0));

	return true;
}

void DebrisSynthesiseLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	CCLabelTTF *labelTile = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Title);
	CCMenuItemImage *itemBooks = (CCMenuItemImage *)mMainMenu->getChildByTag(Tag_Item_Books);
	CCMenuItemImage *itemMaterail = (CCMenuItemImage *)mMainMenu->getChildByTag(Tag_Item_Material);
	CCMenuItemImage *itemEquip = (CCMenuItemImage *)mMainMenu->getChildByTag(Tag_Item_Equip);

	BaseSprite *contentLayer = (BaseSprite*)mBg->getChildByTag(Tag_Layer_Content);
	contentLayer->removeAllChildren();
	contentLayer->setVisible(true);
	contentLayer->setPosition(ccp(0,0));
	mMainMenu->getChildByTag(Tag_Item_FanHui)->setVisible(true);
	mBg->removeChildByTag(Tag_Layer_Synthesise);

	{
		itemBooks->initWithNormalImage(
			"sphc_anniu_shuye.png"
			, "sphc_anniu_shuye_select.png"
			, NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainMenu));
		itemMaterail->initWithNormalImage(
			"sphc_anniu_cailiao.png"
			, "sphc_anniu_cailiao_select.png"
			, NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainMenu));
		itemEquip->initWithNormalImage(
			"sphc_anniu_shenbing.png"
			, "sphc_anniu_shenbing_select.png"
			, NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainMenu));
	}
	if (tag == Tag_Item_Material)
	{
		mCurFlag = flag_material;
		labelTile->setString((LFStrings::getValue("cailiao")+LFStrings::getValue("hecheng")).c_str());
		itemMaterail->initWithNormalImage(
			"sphc_anniu_cailiao_xuanzhong.png"
			, "sphc_anniu_cailiao_select.png"
			, NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainMenu));
	}
	else if (tag == Tag_Item_Books)
	{
		mCurFlag = flag_book;
		labelTile->setString((LFStrings::getValue("jinengshu")+LFStrings::getValue("hecheng")).c_str());
		itemBooks->initWithNormalImage(
			"sphc_anniu_shuye_xuanzhong.png"
			, "sphc_anniu_shuye_select.png"
			, NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainMenu));
	}
	else if (tag == Tag_Item_Equip)
	{
		mCurFlag = flag_equip;
		labelTile->setString((LFStrings::getValue("shenbing")+LFStrings::getValue("hecheng")).c_str());
		itemEquip->initWithNormalImage(
			"sphc_anniu_shenbing_xuanzhong.png"
			, "sphc_anniu_shenbing_select.png"
			, NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_MainMenu));
	}
	refreshContentDebris();

}

void DebrisSynthesiseLayer::menuItemClicked_MainBack(CCObject *pSender)
{
	mBg->removeAllChildren();
	GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
}


unsigned int DebrisSynthesiseLayer::numberOfCellsInTableView(CCTableView *table)
{
	//一个tableCell一屏幕
	if (mCurFlag == flag_equip)
	{
		return (mCurShenBingVec.size() + 5) / 6;
	}
	else 
	{
		return (mCurItemVec.size() + 15) / 16;
	}
}

CCSize DebrisSynthesiseLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSize(498, 566);
}

CCTableViewCell* DebrisSynthesiseLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();
	if (cell == NULL)
	{
		cell = new CCTableViewCell;
		cell->autorelease();
	}
	else
	{
		cell->removeAllChildren();
	}

	if (mTableView != table) return cell;

	if (mCurFlag == flag_equip)
	{
		FXScrollMenu *menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		cell->addChild(menu, 1, Tag_ContentLayer_CellMenu);
		menu->setPosition(ccp(0,0));

		//int start_x = 97, start_y = 838-297, delta_x = 157, delta_y = 243;
		int start_x = 97 - 12, start_y = 838-297-100, delta_x = 157, delta_y = 243;

		for (int i=idx*6, num=0; i<mCurShenBingVec.size() && num<6; ++i, ++num)
		{
			CCSprite *norItem = BaseSprite::create("zhandou_jiesuan_zhuangbei_select.png");
			norItem->setOpacity(0);
			CCSprite *selItem = BaseSprite::create("zhandou_jiesuan_zhuangbei_select.png");
			CCMenuItemSprite *boardItem = CCMenuItemSprite::create(
				norItem,
				selItem,
				NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_SelectEquip));
			menu->addChild(boardItem, 0, i);
			boardItem->setPosition(ccp(start_x + i%3*delta_x, start_y - i%6/3*delta_y)); 
			//diwen
			string cardFile = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(
				mCurShenBingVec.at(i).suiPianDetail.zhuangbei_colour
				, false//, ChuanqiPool::getPool()->isShenBing(mCurShenBingVec.at(i).suiPianDetail.zb_group_id)
				, SuitPool::getPool()->isSuit(mCurShenBingVec.at(i).suiPianDetail.zb_group_id));
			BaseSprite *board = BaseSprite::create(cardFile);
			cell->addChild(board);
			board->setPosition(boardItem->getPosition());
			//方便一些、、、
			if (i == 0 && g_ChuanQiSuiPianHeCheng_Guide)
			{
				mGuide_Sprite = board;
			}
			//equip
			string equipfile = mCurShenBingVec.at(i).suiPianDetail.zhuangbei_pic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *equipImg = BaseSprite::create(equipfile);
			equipImg->setScale(0.23f);
			board->addChild(equipImg);
			equipImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
			//
			int hechengNum = 0x7fffffff;
			for (int id=1; id!=MAX_SUIPIAN_ID_NUM+1; ++id)  // 1 - 5
			{
				if (hechengNum > mCurShenBingVec.at(i).count_array[id])
					hechengNum = mCurShenBingVec.at(i).count_array[id];
			}
			if (hechengNum > 0)
			{
				BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
				board->addChild(derectionImg);
				derectionImg->setPosition(ccp(board->getContentSize().width, board->getContentSize().height-6));
				char buf[10];
				sprintf(buf, "%d", hechengNum);
				CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
				labelNum->setColor(LABLE_COLOR);
				derectionImg->addChild(labelNum);
				labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
			}
			//name
			string name = mCurShenBingVec.at(i).suiPianDetail.zhuangbei_name;
			CCLabelTTF *labName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, LABEL_SIZE-3, CCSize(130,0), kCCTextAlignmentCenter);
			board->addChild(labName);
			labName->setColor(ccBLACK);
			labName->setAnchorPoint(ccp(0.5, 1));
			labName->setPosition(ccp(board->getContentSize().width/2, -6));
		}
		string tempStr = LFStrings::getValue("di")+getStrByInt(idx+1)+LFStrings::getValue("ye");
		CCLabelTTF *page1Label = CCLabelTTF::create(tempStr.c_str(), fontStr_kaiti, LABEL_SIZE);
		cell->addChild(page1Label);
		page1Label->setColor(ccBLACK);
		page1Label->setPosition(ccp(tableCellSizeForIndex(mTableView, idx).width/2, 
			tableCellSizeForIndex(mTableView, idx).height-page1Label->getContentSize().height/2));
	}
	else 
	{
		FXScrollMenu *menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		cell->addChild(menu, 1, Tag_ContentLayer_CellMenu);
		menu->setPosition(ccp(0,0));

//		int start_x = 71, start_y = 838-260, delta_x = 119, delta_y = 130;
		int start_x = 71 - 12, start_y = 838-260-100, delta_x = 119, delta_y = 130;

		for (int i=idx*16, num=0; i!=mCurItemVec.size() && num<16; ++i, ++num)
		{
			CCSprite *norItem = BaseSprite::create("sphc_wupin_bg_select.png");
			norItem->setOpacity(0);
			CCSprite *selItem = BaseSprite::create("sphc_wupin_bg_select.png");
			CCMenuItemSprite *boardItem = CCMenuItemSprite::create(
				norItem,
				selItem,
				NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_SelectItem));
			menu->addChild(boardItem, 0, mCurItemVec.at(i).item_type);
			boardItem->setPosition(ccp(start_x + i%4*delta_x, start_y - i%16/4*delta_y)); 
			//
			BaseSprite *board = BaseSprite::create("sphc_wupin_bg.png");
			cell->addChild(board);
			board->setPosition(boardItem->getPosition());
			//
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mCurItemVec.at(i).item_type));
			itemImg->setScale(0.65f);
			board->addChild(itemImg);
			itemImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
			//
			int hechengNum = 0x7fffffff;
			for (int id=1; id!=6; ++id)  // 1 - 5
			{
				if (hechengNum > mCurItemVec.at(i).count_array[id])
					hechengNum = mCurItemVec.at(i).count_array[id];
			}
			if (hechengNum > 0)
			{
				BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
				board->addChild(derectionImg);
				derectionImg->setPosition(ccp(board->getContentSize().width, board->getContentSize().height-10));
				char buf[10];
				sprintf(buf, "%d", hechengNum);
				CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
				labelNum->setColor(LABLE_COLOR);
				derectionImg->addChild(labelNum);
				labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
			}
			//name
			string name;
			if (mCurItemVec.at(i).item_type >= ItemsType_SkillBook_MinId && mCurItemVec.at(i).item_type <= ItemsType_SkillBook_MaxId)
				name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(mCurItemVec.at(i).item_type - ItemsType_SkillBook_MinId), true);
			else 
				name = CS::getItemName(mCurItemVec.at(i).item_type);
			CCLabelTTF *labName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, LABEL_SIZE-3, CCSize(100,0), kCCTextAlignmentCenter);
			board->addChild(labName);
			labName->setColor(ccBLACK);
			labName->setAnchorPoint(ccp(0.5, 1));
			labName->setPosition(ccp(board->getContentSize().width/2, 0));
		}
		string tempStr = LFStrings::getValue("di")+getStrByInt(idx+1)+LFStrings::getValue("ye");
		CCLabelTTF *page1Label = CCLabelTTF::create(tempStr.c_str(), fontStr_kaiti, LABEL_SIZE);
		cell->addChild(page1Label);
		page1Label->setColor(ccBLACK);
		page1Label->setPosition(ccp(tableCellSizeForIndex(mTableView, idx).width/2, 
			tableCellSizeForIndex(mTableView, idx).height-page1Label->getContentSize().height/2));
	}
	
	return cell;
}


void DebrisSynthesiseLayer::refreshContentDebris()
{
	BaseSprite *contentLayer = dynamic_cast<BaseSprite*>(mBg->getChildByTag(Tag_Layer_Content));
	mTableView = CCTableView::create(this, CCSize(498, 566));
	contentLayer->addChild(mTableView);
	mTableView->setPosition(ccp(8, 100));
	mTableView->setDelegate(this);
	mTableView->setDirection(kCCScrollViewDirectionHorizontal);

	if (mCurFlag == flag_equip)
	{
		mCurShenBingVec = SuiPianManager::getManager()->getAllShenbingSuiPianWithGrouped();
	}
	else 
	{
		mCurItemVec = SuiPianManager::getManager()->getAllItemSuiPianWithGrouped();
		vector<ItemSuipianGroupData> vec;
		if (mCurFlag == flag_material)
		{
			for (int i=0; i!=mCurItemVec.size(); ++i)
				if (! (mCurItemVec.at(i).item_type >= ItemsType_SkillBook_MinId && mCurItemVec.at(i).item_type <= ItemsType_SkillBook_MaxId))
					vec.push_back(mCurItemVec.at(i));
		}
		else if (mCurFlag == flag_book)
		{
			for (int i=0; i!=mCurItemVec.size(); ++i)
				if (mCurItemVec.at(i).item_type >= ItemsType_SkillBook_MinId && mCurItemVec.at(i).item_type <= ItemsType_SkillBook_MaxId)
					vec.push_back(mCurItemVec.at(i));
		}
		mCurItemVec = vec;
	}
	
	mTableView->reloadData();

	//next  last
	CCMenu *menu = CCMenu::create();
	contentLayer->addChild(menu, 0, Tag_Menu_NextLast);
	menu->setPosition(ccp(0,0));
	//
	CCMenuItemImage *next = CCMenuItemImage::create(
		"sphc_anniu_fanye.png",
		"sphc_anniu_fanye_select.png",
		NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_PageChange));
	menu->addChild(next, 1, 1);
	next->setPosition(ccp(454, 838-771));
	CCMenuItemImage *last = CCMenuItemImage::create(
		"sphc_anniu_fanye.png",
		"sphc_anniu_fanye_select.png",
		NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_PageChange));
	last->setRotation(180);
	menu->addChild(last, 1, 2);
	last->setPosition(ccp(75, 838-771));
// 	if (mCurFlag == flag_equip)
// 	{
// 		last->setEnabled(false);
// 		if (mCurShenBingVec.size() <= 6) next->setEnabled(false);
// 	}
// 	else
// 	{
// 		last->setEnabled(false);
// 		if (mCurItemVec.size() <= 16) next->setEnabled(false);
// 	}
}

void DebrisSynthesiseLayer::menuItemClicked_PageChange(CCObject *pSender)
{
	int tag = dynamic_cast<CCMenuItem*>(pSender)->getTag();

	BaseSprite* contentLayer = dynamic_cast<BaseSprite*>(mBg->getChildByTag(Tag_Layer_Content));
	CCMenu *menu = dynamic_cast<CCMenu*>(contentLayer->getChildByTag(Tag_Menu_NextLast));
	if (menu == NULL || contentLayer == NULL) return;

// 	CCMenuItem * next = (CCMenuItem*)menu->getChildByTag(1);
// 	CCMenuItem * last = (CCMenuItem*)menu->getChildByTag(2);

	CCPoint offset = mTableView->getContentOffset();
	CCSize size = tableCellSizeForIndex(mTableView, 0);
	if (tag == 1)  //next
	{
		if (offset.x >= -size.width*(numberOfCellsInTableView(mTableView)-1-1))
			mTableView->setContentOffsetInDuration(ccp(offset.x-size.width, offset.y), 0.25f);
		else 
			mTableView->setContentOffsetInDuration(
				ccp(-size.width*(numberOfCellsInTableView(mTableView)-1), offset.y), 0.15f);
	}
	else if (tag == 2)//before
	{
		if (offset.x <= -size.width)
			mTableView->setContentOffsetInDuration(ccp(offset.x+size.width, offset.y), 0.25f);
		else 
			mTableView->setContentOffsetInDuration(ccp(0, offset.y), 0.15f);
	}
}

void DebrisSynthesiseLayer::menuItemClicked_SelectItem(CCObject *pSender)
{
	mMainMenu->getChildByTag(Tag_Item_FanHui)->setVisible(false);
	mBg->getChildByTag(Tag_Layer_Content)->setVisible(false);

	int tag = dynamic_cast<CCMenuItem*>(pSender)->getTag();
	ItemSuipianGroupData itemData;
	for (int i=0; i!=mCurItemVec.size(); ++i)
	{
		if (tag == mCurItemVec.at(i).item_type)
		{
			mSelItemIdx = i;
			itemData.item_type = mCurItemVec.at(i).item_type;
			for (int j=0; j!=6; ++j) itemData.count_array[j] = mCurItemVec.at(i).count_array[j];
			break;
		}
	}

	//1. name
	CCLabelTTF *labelTitle = dynamic_cast<CCLabelTTF*>(mBg->getChildByTag(Tag_Label_Title));

	//name
	string name;
	if (tag >= ItemsType_SkillBook_MinId && tag <= ItemsType_SkillBook_MaxId)
		name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(tag - ItemsType_SkillBook_MinId), true);
	else 
		name = CS::getItemName(tag);
	labelTitle->setString(name.c_str());
	//2. layer
	BaseSprite *layer = BaseSprite::create();
	mBg->addChild(layer, 1, Tag_Layer_Synthesise);
	layer->setPosition(ccp(0, 0));
	// 
	BaseSprite *board = BaseSprite::create("sphc_hecheng_bg.png");
	layer->addChild(board, 0, Tag_Img_SynBoard);
	board->setPosition(ccp(253, 838-412));
	BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(itemData.item_type));
	layer->addChild(itemImg, 1, Tag_Img_SynEquipOrItem);
	itemImg->setPosition(board->getPosition());
	//
	char buf[10];
	string nameSuiPian = ImageManager::getManager()->getItemDebrisFileName(itemData.item_type);
	//
	string nameTemp = name + LFStrings::getValue("zhi") + LFStrings::getValue("suipian");
	CCLabelTTF *labelDeb1 = CCLabelTTF::create((nameTemp+LFStrings::getValue("yi")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb1, 1, Tag_Label_DebrisName_ + 0);
	labelDeb1->setAnchorPoint(ccp(0.5,1));
	labelDeb1->setPosition(ccp(169, 838-566));
	labelDeb1->setColor(ccBLACK);
#define SCALE_SuiPian 0.75f
	if (itemData.count_array[1] > 0)
	{
		BaseSprite *spImg1 = BaseSprite::create(nameSuiPian);
		board->addChild(spImg1, 0, Tag_Sprite_Debris + 0);
		spImg1->setPosition(POS_1);
		spImg1->setScale(SCALE_SuiPian);
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 0);
		derectionImg->setPosition(ccp(210, 838-553));
		sprintf(buf, "%d", itemData.count_array[1]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	//
	CCLabelTTF *labelDeb2 = CCLabelTTF::create((nameTemp+LFStrings::getValue("er")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb2, 1, Tag_Label_DebrisName_ + 1);
	labelDeb2->setAnchorPoint(ccp(0.5, 1));
	labelDeb2->setPosition(ccp(75, 838-410));
	labelDeb2->setColor(ccBLACK);
	if (itemData.count_array[2] > 0)
	{
		BaseSprite *spImg2 = BaseSprite::create(nameSuiPian);
		board->addChild(spImg2, 0, Tag_Sprite_Debris + 1);
		spImg2->setPosition(POS_2);
		spImg2->setScale(SCALE_SuiPian);
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 1);
		derectionImg->setPosition(ccp(148, 838-325));
		sprintf(buf, "%d", itemData.count_array[2]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	//
	CCLabelTTF *labelDeb3 = CCLabelTTF::create((nameTemp+LFStrings::getValue("san")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb3, 1, Tag_Label_DebrisName_ + 2);
	labelDeb3->setAnchorPoint(ccp(0.5, 0));
	labelDeb3->setPosition(ccp(252, 838-231));
	labelDeb3->setColor(ccBLACK);
	if (itemData.count_array[3] > 0)
	{
		BaseSprite *spImg3 = BaseSprite::create(nameSuiPian);
		board->addChild(spImg3, 0, Tag_Sprite_Debris + 2);
		spImg3->setPosition(POS_3);
		spImg3->setScale(SCALE_SuiPian);
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 2);
		derectionImg->setPosition(ccp(300, 838-288));
		sprintf(buf, "%d", itemData.count_array[3]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	//
	CCLabelTTF *labelDeb4 = CCLabelTTF::create((nameTemp+LFStrings::getValue("si")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb4, 1, Tag_Label_DebrisName_ + 3);
	labelDeb4->setAnchorPoint(ccp(0.5, 1));
	labelDeb4->setPosition(ccp(430, 838-411));
	labelDeb4->setColor(ccBLACK);
	if (itemData.count_array[4] > 0)
	{
		BaseSprite *spImg2 = BaseSprite::create(nameSuiPian);
		board->addChild(spImg2, 0, Tag_Sprite_Debris + 3);
		spImg2->setPosition(POS_4);
		spImg2->setScale(SCALE_SuiPian);
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 3);
		derectionImg->setPosition(ccp(430, 838-392));
		sprintf(buf, "%d", itemData.count_array[4]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	//
	CCLabelTTF *labelDeb5 = CCLabelTTF::create((nameTemp+LFStrings::getValue("wu")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb5, 1, Tag_Label_DebrisName_ + 4);
	labelDeb5->setAnchorPoint(ccp(0.5, 1));
	labelDeb5->setPosition(ccp(344, 838-568));
	labelDeb5->setColor(ccBLACK);
	if (itemData.count_array[5] > 0)
	{
		BaseSprite *spImg2 = BaseSprite::create(nameSuiPian);
		board->addChild(spImg2, 0, Tag_Sprite_Debris + 4);
		spImg2->setPosition(POS_5);
		spImg2->setScale(SCALE_SuiPian);
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 4);
		derectionImg->setPosition(ccp(379, 838-553));
		sprintf(buf, "%d", itemData.count_array[5]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	//3. menu
	CCMenu *menu = CCMenu::create();
	layer->addChild(menu, 1, Tag_SynthesiseMenu);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *itemSy = CCMenuItemImage::create(
		"sphc_anniu_hecheng.png",
		"sphc_anniu_hecheng_select.png",
		"sphc_anniu_hecheng_suoding.png",
		this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_SynthesiseOrBack));
	menu->addChild(itemSy, 0, Tag_SynthesiseMenuItem_Synthesise);
	itemSy->setPosition(ccp(148, 838-770));
	CCMenuItemImage *itemBack = CCMenuItemImage::create(
		"sphc_anniu_fanhui.png",
		"sphc_anniu_fanhui_select.png",
		NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_SynthesiseOrBack));
	menu->addChild(itemBack, 0, Tag_SynthesiseMenuItem_Back);
	itemBack->setPosition(ccp(380, 838-770));
	//4. synthesise item is enable
	for (int id=1; id!=6; ++id)
	{
		if (itemData.count_array[id] <= 0)
		{
			itemSy->setEnabled(false);
			break;
		}
	}
}

void DebrisSynthesiseLayer::menuItemClicked_SelectEquip(CCObject *pSender)
{
	mMainMenu->getChildByTag(Tag_Item_FanHui)->setVisible(false);
	mBg->getChildByTag(Tag_Layer_Content)->setVisible(false);

	int idx = dynamic_cast<CCMenuItem*>(pSender)->getTag();
	mSelEquipIdx = idx;
	ShenbingSuipianGroupData sbData = mCurShenBingVec.at(idx);

	//1. name
	CCLabelTTF *labelTitle = dynamic_cast<CCLabelTTF*>(mBg->getChildByTag(Tag_Label_Title));

	//name
	labelTitle->setString(sbData.suiPianDetail.zhuangbei_name.c_str());
	//2. layer
	BaseSprite *layer = BaseSprite::create();
	mBg->addChild(layer, 1, Tag_Layer_Synthesise);
	layer->setPosition(ccp(0, 0));
	// 
	BaseSprite *board = BaseSprite::create("sphc_hecheng_bg.png");
	layer->addChild(board, 0, Tag_Img_SynBoard);
	board->setPosition(ccp(253, 838-412));
	//diwen
	string diwenFile = "zhuangbei_sphc_diwen_chuanqi_taozhuang.png";
// 	if (SuitPool::getPool()->isSuit(sbData.suiPianDetail.zb_group_id))
// 		diwenFile = "zhuangbei_sphc_diwen_shenbing_taozhuang.png";
// 	else
// 		diwenFile = "zhuangbei_sphc_diwen_shenbing.png";
	BaseSprite *diwen = BaseSprite::create(diwenFile);
	board->addChild(diwen);
	diwen->setPosition(ccp(180, 365-180));
	//
	string equipfile = sbData.suiPianDetail.zhuangbei_pic;
	equipfile = "minute_" + equipfile + ".png";
	BaseSprite *equipImg = BaseSprite::create(equipfile);
	equipImg->setScale(EQUIP_SCALE);
	layer->addChild(equipImg, 1, Tag_Img_SynEquipOrItem);
	equipImg->setPosition(board->getPosition());
	//
	char buf[10];
	string maskfile = "zhuangbei_sphc_yuan.png";
	string nameTemp = sbData.suiPianDetail.zhuangbei_name + LFStrings::getValue("zhi") + LFStrings::getValue("suipian");
	//
	CCLabelTTF *labelDeb1 = CCLabelTTF::create((nameTemp+LFStrings::getValue("yi")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb1, 1, Tag_Label_DebrisName_ + 0);
	labelDeb1->setAnchorPoint(ccp(0.5,1));
	labelDeb1->setPosition(ccp(165, 838-568));
	labelDeb1->setColor(ccBLACK);
#define DIWEN_Scale 0.55f
	if (sbData.count_array[1] > 0)
	{
		BaseSprite *spImg1 = BaseSprite::create(diwenFile);
		board->addChild(spImg1, 0, Tag_Sprite_Debris + 0);
		spImg1->setPosition(POS_1);
		spImg1->setScale(DIWEN_Scale);
		BaseSprite *suipianImg = BaseSprite::create(equipfile);
		spImg1->addChild(suipianImg);
		suipianImg->setScale(0.25f);
		suipianImg->setPosition(ccp(spImg1->getContentSize().width/2, spImg1->getContentSize().height/2));
		BaseSprite *maskImg = BaseSprite::create(maskfile);
		spImg1->addChild(maskImg);
		maskImg->setPosition(suipianImg->getPosition());
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 0);
		derectionImg->setPosition(ccp(210, 838-553));
		sprintf(buf, "%d", sbData.count_array[1]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	CCLabelTTF *labelDeb2 = CCLabelTTF::create((nameTemp+LFStrings::getValue("er")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb2, 1, Tag_Label_DebrisName_ + 1);
	labelDeb2->setAnchorPoint(ccp(0.5, 1));
	labelDeb2->setPosition(ccp(75, 838-410));
	labelDeb2->setColor(ccBLACK);
	if (sbData.count_array[2] > 0)
	{
		BaseSprite *spImg1 = BaseSprite::create(diwenFile);
		board->addChild(spImg1, 0, Tag_Sprite_Debris + 1);
		spImg1->setPosition(POS_2);
		spImg1->setScale(DIWEN_Scale);
		BaseSprite *suipianImg = BaseSprite::create(equipfile);
		spImg1->addChild(suipianImg);
		suipianImg->setScale(0.25f);
		suipianImg->setPosition(ccp(spImg1->getContentSize().width/2, spImg1->getContentSize().height/2));
		BaseSprite *maskImg = BaseSprite::create(maskfile);
		spImg1->addChild(maskImg);
		maskImg->setPosition(suipianImg->getPosition());
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 1);
		derectionImg->setPosition(ccp(148, 838-325));
		sprintf(buf, "%d", sbData.count_array[2]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	CCLabelTTF *labelDeb3 = CCLabelTTF::create((nameTemp+LFStrings::getValue("san")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb3, 1, Tag_Label_DebrisName_ + 2);
	labelDeb3->setAnchorPoint(ccp(0.5, 0));
	labelDeb3->setPosition(ccp(252, 838-231));
	labelDeb3->setColor(ccBLACK);
	if (sbData.count_array[3] > 0)
	{
		BaseSprite *spImg1 = BaseSprite::create(diwenFile);
		board->addChild(spImg1, 0, Tag_Sprite_Debris + 2);
		spImg1->setPosition(POS_3);
		spImg1->setScale(DIWEN_Scale);
		BaseSprite *suipianImg = BaseSprite::create(equipfile);
		spImg1->addChild(suipianImg);
		suipianImg->setScale(0.25f);
		suipianImg->setPosition(ccp(spImg1->getContentSize().width/2, spImg1->getContentSize().height/2));
		BaseSprite *maskImg = BaseSprite::create(maskfile);
		spImg1->addChild(maskImg);
		maskImg->setPosition(suipianImg->getPosition());
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 2);
		derectionImg->setPosition(ccp(300, 838-288));
		sprintf(buf, "%d", sbData.count_array[3]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	CCLabelTTF *labelDeb4 = CCLabelTTF::create((nameTemp+LFStrings::getValue("si")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb4, 1, Tag_Label_DebrisName_ + 3);
	labelDeb4->setAnchorPoint(ccp(0.5, 1));
	labelDeb4->setPosition(ccp(430, 838-411));
	labelDeb4->setColor(ccBLACK);
	if (sbData.count_array[4] > 0)
	{
		BaseSprite *spImg1 = BaseSprite::create(diwenFile);
		board->addChild(spImg1, 0, Tag_Sprite_Debris + 3);
		spImg1->setPosition(POS_4);
		spImg1->setScale(DIWEN_Scale);
		BaseSprite *suipianImg = BaseSprite::create(equipfile);
		spImg1->addChild(suipianImg);
		suipianImg->setScale(0.25f);
		suipianImg->setPosition(ccp(spImg1->getContentSize().width/2, spImg1->getContentSize().height/2));
		BaseSprite *maskImg = BaseSprite::create(maskfile);
		spImg1->addChild(maskImg);
		maskImg->setPosition(suipianImg->getPosition());
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 3);
		derectionImg->setPosition(ccp(430, 838-392));
		sprintf(buf, "%d", sbData.count_array[4]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	CCLabelTTF *labelDeb5 = CCLabelTTF::create((nameTemp+LFStrings::getValue("wu")).c_str(), fontStr_kaiti, LABEL_SIZE-2, CCSize(114,0),kCCTextAlignmentCenter);
	layer->addChild(labelDeb5, 1, Tag_Label_DebrisName_ + 4);
	labelDeb5->setAnchorPoint(ccp(0.5, 1));
	labelDeb5->setPosition(ccp(344, 838-568));
	labelDeb5->setColor(ccBLACK);
	if (sbData.count_array[5] > 0)
	{
		BaseSprite *spImg1 = BaseSprite::create(diwenFile);
		board->addChild(spImg1, 0, Tag_Sprite_Debris + 4);
		spImg1->setPosition(POS_5);
		spImg1->setScale(DIWEN_Scale);
		BaseSprite *suipianImg = BaseSprite::create(equipfile);
		spImg1->addChild(suipianImg);
		suipianImg->setScale(0.25f);
		suipianImg->setPosition(ccp(spImg1->getContentSize().width/2, spImg1->getContentSize().height/2));
		BaseSprite *maskImg = BaseSprite::create(maskfile);
		spImg1->addChild(maskImg);
		maskImg->setPosition(suipianImg->getPosition());
		//
		BaseSprite *derectionImg = BaseSprite::create("sphc_shuliang_bg.png");
		layer->addChild(derectionImg, 1, Tag_Board_DebrisNum_ + 4);
		derectionImg->setPosition(ccp(379, 838-553));
		sprintf(buf, "%d", sbData.count_array[5]);
		CCLabelTTF *labelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 21);
		labelNum->setColor(LABLE_COLOR);
		derectionImg->addChild(labelNum);
		labelNum->setPosition(ccp(derectionImg->getContentSize().width/2, derectionImg->getContentSize().height/2));
	}
	//3. menu
	CCMenu *menu = CCMenu::create();
	layer->addChild(menu, 1, Tag_SynthesiseMenu);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *itemSy = CCMenuItemImage::create(
		"sphc_anniu_hecheng.png",
		"sphc_anniu_hecheng_select.png",
		"sphc_anniu_hecheng_suoding.png",
		this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_SynthesiseOrBack));
	menu->addChild(itemSy, 0, Tag_SynthesiseMenuItem_Synthesise);
	itemSy->setPosition(ccp(148, 838-770));
	CCMenuItemImage *itemBack = CCMenuItemImage::create(
		"sphc_anniu_fanhui.png",
		"sphc_anniu_fanhui_select.png",
		NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_SynthesiseOrBack));
	menu->addChild(itemBack, 0, Tag_SynthesiseMenuItem_Back);
	itemBack->setPosition(ccp(380, 838-770));
	//4. synthesise item is enable
	for (int id=1; id!=6; ++id)
	{
		if (sbData.count_array[id] <= 0)
		{
			itemSy->setEnabled(false);
			break;
		}
	}



	//教学
	if (g_ChuanQiSuiPianHeCheng_Guide)
	{
		g_ChuanQiSuiPianHeCheng_Guide = false;
		g_ChuanQiSuiPianHeCheng_GuideEnd = true;
		GuideLayerBase *guideLayer = new GuideLayer_SimpleJumpLayer;
		guideLayer->init(
			board->getContentSize(),
			board->getParent()->convertToWorldSpace(board->getPosition()), 
			LFStrings::getValue("chuanqiSuiPian_HechengShuoMing"),
			NULL,
			true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}
}

void DebrisSynthesiseLayer::menuItemClicked_SynthesiseOrBack(CCObject *pSender)
{
	int tag = dynamic_cast<CCMenuItem*>(pSender)->getTag();

	if (tag == Tag_SynthesiseMenuItem_Synthesise)
	{
		if (mCurFlag == flag_material || mCurFlag == flag_book)
		{
			addObserver(callfuncO_selector(DebrisSynthesiseLayer::rsp_heCheng), MSG_ItemHechengRsp);
			mCmdHlp->cmdItemHecheng(mCurItemVec.at(mSelItemIdx).item_type);
		}
		else if (mCurFlag == flag_equip)
		{
			if (! EquipmentManager::getManager()->isSpareEnoughToSaveEquip(1))
			{
				EquipBagNotEnoughDialog *layer = new EquipBagNotEnoughDialog;
				layer->showGoToFenJieDialog();
			}
			else
			{
				addObserver(callfuncO_selector(DebrisSynthesiseLayer::rsp_heCheng), MSG_ShenbingHechengRsp_F);
				mCmdHlp->cmdShenbingHecheng(mCurShenBingVec.at(mSelEquipIdx).suiPianDetail.zb_group_id);
			}
		}
	}
	else if (tag == Tag_SynthesiseMenuItem_Back)
	{
		mMainMenu->getChildByTag(Tag_Item_FanHui)->setVisible(true);
		mBg->getChildByTag(Tag_Layer_Content)->setVisible(true);
		mBg->removeChildByTag(Tag_Layer_Synthesise);

		CCLabelTTF *labelTitle = dynamic_cast<CCLabelTTF*>(mBg->getChildByTag(Tag_Label_Title));
		if (mCurFlag == flag_material)
			labelTitle->setString((LFStrings::getValue("cailiao")+LFStrings::getValue("hecheng")).c_str());
		else if (mCurFlag == flag_book)
			labelTitle->setString((LFStrings::getValue("jinengshu")+LFStrings::getValue("hecheng")).c_str());
		else if (mCurFlag == flag_equip)
			labelTitle->setString((LFStrings::getValue("shenbing")+LFStrings::getValue("hecheng")).c_str());
	}
}

void DebrisSynthesiseLayer::rsp_heCheng(CCObject *msg)
{
	GameDirector::getDirector()->hideWaiting();

	int num = 0;
	if (mCurFlag == flag_material || mCurFlag == flag_book)
	{
		removeObserver(MSG_ItemHechengRsp);
		SPCmd_ItemHechengRsp *data = (SPCmd_ItemHechengRsp*)dynamic_cast<MSG_Rsp*>(msg)->getData();
		if(data->err_code != Success)
		{
			MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		num = data->item_info.count;
	}
	else if (mCurFlag == flag_equip)
	{
		removeObserver(MSG_ShenbingHechengRsp_F);
		Flat_ShenbingHechengRsp *data = (Flat_ShenbingHechengRsp*)dynamic_cast<Flat_MSG_Rsp*>(msg)->getData();
		if(data->err_code != Success)
		{
			MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		num = data->zhuangbei_vec.size();
	}

	BaseSprite *layer = (BaseSprite*)mBg->getChildByTag(Tag_Layer_Synthesise);
	dynamic_cast<CCMenu*>(layer->getChildByTag(Tag_SynthesiseMenu))->setEnabled(false);

	//
	//effect
	//
	CCString temp;
	BaseSprite* board = (BaseSprite*)layer->getChildByTag(Tag_Img_SynBoard);
	if (board)
	{
		CCCallFuncN *funcN = CCCallFuncN::create(this, callfuncN_selector(DebrisSynthesiseLayer::action_removeFromParent));
		for (int i=0; i!=5; ++i)
		{
			layer->removeChildByTag(Tag_Label_DebrisName_ + i);
			layer->removeChildByTag(Tag_Board_DebrisNum_  + i);
		}
		//board
		board->runAction(CCSequence::create(
			CCEaseSineIn::create(CCRotateBy::create(2.25f, 1800)),
			CCSpawn::create(CCFadeTo::create(0.75f, 0), CCRotateBy::create(0.75f, 600), NULL),
// 			CCDelayTime::create(2.25f),
// 			CCFadeTo::create(0.75f, 0),

			CCDelayTime::create(0.5f),
			CCCallFuncND::create(this, callfuncND_selector(DebrisSynthesiseLayer::heChengEnd_display), (void*)num),
			NULL));
		for (int i=0; i!=5; ++i)
		{
			board->getChildByTag(Tag_Sprite_Debris + i)->runAction(CCSequence::create(
				CCDelayTime::create(2.25f),
				CCFadeTo::create(0.25f, 0),
				funcN->copy()->autorelease(),
				NULL));
		}
		//光影、
		BaseSprite *sunShine1 = BaseSprite::create("sphc_xiaoguo_1_1.png");
		layer->addChild(sunShine1, 11);
		sunShine1->setPosition(board->getPosition());
		sunShine1->setOpacity(0);
		sunShine1->setScale(0.5f);
		sunShine1->runAction(CCSequence::create(
			CCSpawn::create(CCScaleTo::create(0.1f, 1), CCFadeTo::create(0.5f, 255), CCRotateBy::create(1.5f, 60), NULL),
			CCSpawn::create(CCRotateBy::create(0.75f, 30), CCFadeTo::create(0.75f, 0), NULL),
			funcN->copy()->autorelease(),
			NULL));
		BaseSprite *sunShine2 = BaseSprite::create("sphc_xiaoguo_1_2.png");
		layer->addChild(sunShine2, 12);
		sunShine2->setPosition(board->getPosition());
		sunShine2->runAction(CCSequence::create(
			CCSpawn::create(CCScaleTo::create(0.1f, 1), CCFadeTo::create(0.5f, 255), CCRotateBy::create(1.5f, 120), NULL),
			CCSpawn::create(CCRotateBy::create(0.75f, 60), CCFadeTo::create(0.75f, 0), NULL),
			funcN->copy()->autorelease(),
			NULL));
		BaseSprite *sunShine3 = BaseSprite::create("sphc_xiaoguo_1_3.png");
		layer->addChild(sunShine3, 13);
		sunShine3->setPosition(board->getPosition());
		sunShine3->runAction(CCSequence::create(
			CCSpawn::create(CCScaleTo::create(0.1f, 1), CCFadeTo::create(0.5f, 255), CCRotateBy::create(1.5f, 240), NULL),
			CCSpawn::create(CCRotateBy::create(0.75f, 120), CCFadeTo::create(0.75f, 0), NULL),
			funcN->copy()->autorelease(),
			NULL));
		//动画
// 		CCAnimation *animation = CCAnimation::create();
// 		for (int i=1; i!=7; ++i)
// 		{
// 			temp.initWithFormat("sphc_xiaoguo_2_%d.png", i);
// 			animation->addSpriteFrameWithFileName(temp.getCString());
// 		}
// 		animation->setDelayPerUnit(0.05f);
// 		CCAnimate *animate = CCAnimate::create(animation);
// 		BaseSprite *firstFrame = BaseSprite::create("sphc_xiaoguo_2_1.png");
// 		firstFrame->setOpacity(0);
// 		layer->addChild(firstFrame, 14);
// 		firstFrame->setPosition(board->getPosition());
// 		firstFrame->runAction(CCSequence::create(
// 			CCDelayTime::create(2.25f),
// 			CCFadeTo::create(0.15f, 255),
// 			animate,
// 			funcN->copy()->autorelease(),
// 			NULL));
		// 第二套光影转动
		BaseSprite *sunS1 = BaseSprite::create("sphc_xiaoguo_3_1.png");
		layer->addChild(sunS1, 21);
		sunS1->setPosition(board->getPosition());
		sunS1->setOpacity(0);
		sunS1->runAction(CCSequence::create(
			CCDelayTime::create(1.75f),
			CCSpawn::create(CCFadeTo::create(1.0f, 255), CCRotateBy::create(1.25f, 112), NULL),
			CCSpawn::create(CCFadeTo::create(0.75f, 0), CCRotateBy::create(1.0f, 90), NULL),
			funcN->copy()->autorelease(),
			NULL));
		BaseSprite *sunS2 = BaseSprite::create("sphc_xiaoguo_3_2.png");
		layer->addChild(sunS2, 21);
		sunS2->setPosition(board->getPosition());
		sunS2->setOpacity(0);
		sunS2->runAction(CCSequence::create(
			CCDelayTime::create(1.5f),
			CCSpawn::create(CCFadeTo::create(0.75f, 255), CCRotateBy::create(1.25f, 225), NULL),
			CCSpawn::create(CCFadeTo::create(0.75f, 0), CCRotateBy::create(1.0, 180), NULL),
			funcN->copy()->autorelease(),
			NULL));
	}
}

void DebrisSynthesiseLayer::heChengEnd_display(CCNode *node, void *data)
{
	int num = (int)data;
	//delete
	BaseSprite *layer = (BaseSprite*)mBg->getChildByTag(Tag_Layer_Synthesise);
	layer->removeAllChildren();
	//img
	if (mCurFlag == flag_equip)
	{
		//diwen
		string cardFile = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(
			mCurShenBingVec.at(mSelEquipIdx).suiPianDetail.zhuangbei_colour
			, false//, ChuanqiPool::getPool()->isShenBing(mCurShenBingVec.at(mSelEquipIdx).suiPianDetail.zb_group_id)
			, SuitPool::getPool()->isSuit(mCurShenBingVec.at(mSelEquipIdx).suiPianDetail.zb_group_id));
		BaseSprite *board = BaseSprite::create(cardFile);
		layer->addChild(board);
		board->setPosition(ccp(253, 838-412));
		//equip
		string equipfile = mCurShenBingVec.at(mSelEquipIdx).suiPianDetail.zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		BaseSprite *equipImg = BaseSprite::create(equipfile);
		equipImg->setScale(0.23f);
		layer->addChild(equipImg, 1, Tag_Img_SynEquipOrItem);
		equipImg->setPosition(board->getPosition());
	}
	else
	{
// 		//board
// 		BaseSprite *board = BaseSprite::create("daoju_kuang_bg.png");
// 		layer->addChild(board, 1, Tag_Img_SynEquipOrItem);
// 		board->setPosition(ccp(253, 838-412));
		//item
		BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mCurItemVec.at(mSelItemIdx).item_type));
		layer->addChild(itemImg, 1, Tag_Img_SynEquipOrItem);
		itemImg->setPosition(ccp(253, 838-412));
	}
	//back menu
	CCMenu *menu = CCMenu::create();
	layer->addChild(menu);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *itemBack = CCMenuItemImage::create(
		"sphc_anniu_fanhui.png",
		"sphc_anniu_fanhui_select.png",
		NULL, this, menu_selector(DebrisSynthesiseLayer::menuItemClicked_rspBack));
	menu->addChild(itemBack);
	itemBack->setPosition(ccp(263, 838-771));
	//label 
	string str = LFStrings::getValue("chenggong") + LFStrings::getValue("hecheng") +" ";
	if (mCurFlag == flag_equip)
		str += mCurShenBingVec.at(mSelEquipIdx).suiPianDetail.zhuangbei_name;
	else if (mCurFlag == flag_book)
		str += SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(mCurItemVec.at(mSelItemIdx).item_type-ItemsType_SkillBook_MinId), true);
	else if (mCurFlag == flag_material)
		str += CS::getItemName(mCurItemVec.at(mSelItemIdx).item_type);
	str += "  x";
	char buf[10];
	sprintf(buf, "%d", num);
	str += buf;
	CCLabelTTF *labelInfo = CCLabelTTF::create(str.c_str(), fontStr_kaiti, LABEL_SIZE);
	layer->addChild(labelInfo);
	labelInfo->setColor(ccBLACK);
	labelInfo->setPosition(ccp(259, 838-600));
	//
}

void DebrisSynthesiseLayer::menuItemClicked_rspBack(CCObject *pSender)
{
	if (mCurFlag == flag_material)
		menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_Item_Material));
	else if (mCurFlag == flag_book)
		menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_Item_Books));
	else if (mCurFlag == flag_equip)
		menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_Item_Equip));
}

void DebrisSynthesiseLayer::action_removeFromParent(CCNode *node)
{
	node->removeFromParent();
}

void DebrisSynthesiseLayer::onEnter()
{
	BaseLayer::onEnter();

	menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_Item_Equip));


	//教学
	if (g_ChuanQiSuiPianHeCheng_Guide)
	{
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		guideLayer->init(
			mGuide_Sprite->getContentSize(),
			mGuide_Sprite->getParent()->convertToWorldSpace(mGuide_Sprite->getPosition()), 
			LFStrings::getValue("chuanqiSuiPian_HeChengTiShi"), NULL, false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}
}