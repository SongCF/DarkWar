#include "GongJiangBaoShiHeCheng_BaoShiSelect.h"
#include "CS_Common.h"
#include "WuPinManager.h"

#define TAG_BG_MASK_PIC 22221
#define LABEL_SIZE 20

#include "font.h"

GongJiangBaoShiHeCheng_BaoShiSelect *GongJiangBaoShiHeCheng_BaoShiSelect::create(GongJiangBaoShiHeCheng* hechengLayer)
{
	GongJiangBaoShiHeCheng_BaoShiSelect *layer = new GongJiangBaoShiHeCheng_BaoShiSelect(hechengLayer);
	if(layer)
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


GongJiangBaoShiHeCheng_BaoShiSelect::GongJiangBaoShiHeCheng_BaoShiSelect(GongJiangBaoShiHeCheng* hechengLayer)
{
	mTableView = CCTableView::create(this, CCSizeMake(587,681));
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setPosition(ccp(34,40));
	mTableView->setDelegate(this);
	addChild(mTableView);
	mFooterForPop = NULL;


	//初始化
	selectedIndex = -1;

	mHechengLayer = hechengLayer;

	tabBar = BaseSprite::create("mianban_zhuangbei.png");
	addChild(tabBar);
	tabBar->setPosition(ccp(mWinSize.width/2,tabBar->getContentSize().height/2));

	mTableView->retain();
	mTableView->removeFromParent();
	tabBar->addChild(mTableView);
	mTableView->initWithViewSize(CCSize(613, 738));
	mTableView->setPosition(ccp(8,14));
	mTableView->release();

	

	//
	{
//		setItemsData(WuPinManager::getManager()->getAllBaoshi());
		setItemsData(getBaoShi());
		
	}
	////选择您要合成的宝石
	//CCLabelTTF* xuanzheniyao = CCLabelTTF::create(LFStrings::getValue("hecheng_xuan_zhe_ni_yao").c_str(), fontStr_kaiti, LABEL_SIZE);
	//addChild(xuanzheniyao);
	//xuanzheniyao->setColor(fonColor_PuTong);
	//xuanzheniyao->setPosition(ccp(280, 838));

	//创建按钮
	{
		mMenuItemCancel = CCMenuItemImage::create(
			"xuanze_zhuang_bei_quxiao.png",
			"xuanze_zhuang_bei_quxiao_select.png",
			"xuanze_zhuang_bei_quxiao_select.png",
			this,SEL_MenuHandler(&GongJiangBaoShiHeCheng_BaoShiSelect::menuItemCancelClicked));
		mMenuItemMakeSure = CCMenuItemImage::create(
			"xuanze_zhuang_bei_queding.png",
			"xuanze_zhuang_bei_queding_select.png",
			"xuanze_zhuang_bei_queding_select.png",
			this,SEL_MenuHandler(&GongJiangBaoShiHeCheng_BaoShiSelect::menuItemMakeSureClicked));


		mMenu = CCMenu::create(mMenuItemCancel, mMenuItemMakeSure, NULL);
		addChild(mMenu,1);
		mMenu->setPosition(CCPointZero);


		mMenuItemCancel->setPosition(ccp(86,802));

		mMenuItemMakeSure->setPosition(ccp(554,802));
		mMenuItemMakeSure->setEnabled(false);
	}

	//title
	CCLabelTTF *title = CCLabelTTF::create(LFStrings::getValue("XuanZeNiXuyao_deBaoShi").c_str(), fontStr_kaiti, 20);
	addChild(title, 1);
	title->setColor(fonColor_PuTong);
	title->setPosition(ccp(320, 804));
	CCLabelTTF *la = CCLabelTTF::create(LFStrings::getValue("hecheng").c_str(), fontStr_kaiti, 20);
	addChild(la, 1);
	la->setPosition(ccp(340, 804));
}

void GongJiangBaoShiHeCheng_BaoShiSelect::scrollViewDidScroll(CCScrollView* view) 
{
}
void GongJiangBaoShiHeCheng_BaoShiSelect::scrollViewDidZoom(CCScrollView* view) 
{
}


void GongJiangBaoShiHeCheng_BaoShiSelect::tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell)
{
	//CCLog("tableCellWillRecycle");
}
void GongJiangBaoShiHeCheng_BaoShiSelect::tableCellHighlight(CCTableView* table, CCTableViewCell* cell)
{
}

void GongJiangBaoShiHeCheng_BaoShiSelect::tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell)
{
}

CCSize GongJiangBaoShiHeCheng_BaoShiSelect::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSizeMake(622, 148);
}

CCTableViewCell* GongJiangBaoShiHeCheng_BaoShiSelect::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCString *string = CCString::createWithFormat("%d", idx);
	CCTableViewCell *cell = table->dequeueCell();
	if (cell == NULL) 
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}
	else
	{
		cell->removeAllChildren();
	}

	cell->addChild(getItemContentByIndex(idx),0,TAG_BaseBody_TABLE_ITEM);

	return cell;
}

BaseSprite* GongJiangBaoShiHeCheng_BaoShiSelect::getItemContentByIndex(unsigned int index)
{
	BaseSprite* bg = BaseSprite::create("shangdian_shangpin_bg.png");

	CCPoint picPos = ccp(68, bg->getContentSize().height/2);
	unsigned int item_type = mItemsVec[index].type;

	//item图片
	{
		BaseSprite *sprite = BaseSprite::create(ImageManager::getManager()->getItemFilename(item_type));
		sprite->setPosition(picPos);
		bg->addChild(sprite,0,TAG_BaseBody_TABLE_ITEM_Image);
	}

	// 名字
	LF::lable(bg,CS::getItemName(item_type),ccp(142,116),23,fontStr_kaiti)->setTag(TAG_BaseBody_LABLE_NAME);

	//属性
	CCLabelTTF* lable_shuxing = 
		LF::lable(bg,CS::getItemShuxing(item_type),ccp(142,76),25,fontStr_kaiti);
	lable_shuxing->setTag(TAG_BaseBody_LABLE_SHUXING);
	ccColor3B color;
	color.r = 195;
	color.g = 145;
	color.b = 96;
	lable_shuxing->setColor(color);


	//数量
	{
		int baoshi_need = CS::HechengBaoshiNeedBaoshiCount(item_type);
		int baoshi_total = 0;
		WuPin* baoshi= WuPinManager::getManager()->getWuPin((ItemsType)CS::HechengBaoshiNeedBaoshiType(item_type));
		if (baoshi != NULL)
		{
			baoshi_total = baoshi->getCount();
		}
		int gold_need = CS::HechengBaoshiNeedGold(item_type);
		int gold_total = 0;
		WuPin* gold= WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		if (gold != NULL)
		{
			gold_total = gold->getCount();
		}

		if (baoshi_need > baoshi_total)
		{
			CCLabelTTF* lable = LF::lable(bg,LFStrings::getValue("cailiaobuzu"),ccp(142,32),20,fontStr_kaiti);
			lable->setTag(TAG_BaseBody_LABLE_COUNT);
			lable->setColor(ccRED);
		}
		else if(gold_need > gold_total)
		{
			CCLabelTTF* lable = LF::lable(bg,LFStrings::getValue("jinbibuzu"),ccp(142,32),20,fontStr_kaiti);
			lable->setTag(TAG_BaseBody_LABLE_COUNT);
			lable->setColor(ccRED);
		}
		else
		{
			int count 
				= (baoshi_total/baoshi_need) < (gold_total/gold_need) ? (baoshi_total/baoshi_need) : (gold_total/gold_need);
			char str[64];
			sprintf(str, "%s %d", LFStrings::getValue("kehecheng").c_str(), count);

			CCLabelTTF* lable = LF::lable(bg,str,ccp(142,32),20,fontStr_kaiti);
			lable->setTag(TAG_BaseBody_LABLE_COUNT);
			lable->setColor(ccGREEN);
		}

		
	}
	

	bg->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
	return bg;
}

unsigned int GongJiangBaoShiHeCheng_BaoShiSelect::numberOfCellsInTableView(CCTableView *table)
{
	// 第一步
	return mItemsVec.size();
}

void GongJiangBaoShiHeCheng_BaoShiSelect::menuItemCancelClicked(CCObject* obj)
{
	mFooterLayer->dismmisPopUp();
}

void GongJiangBaoShiHeCheng_BaoShiSelect::menuItemMakeSureClicked(CCObject* obj)
{
	unsigned int itemType = 0;
	if(selectedIndex >= 0)
	{
		itemType = mItemsVec[selectedIndex].type;
	}
	mHechengLayer->setSelectedBaoshiTypeId(itemType, 1);

	//
	mFooterLayer->dismmisPopUp();
}

void GongJiangBaoShiHeCheng_BaoShiSelect::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	BaseSprite* bg = (BaseSprite*)cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
	// 先不管是否点击了图片

	// 先移除上一个点击的
	if (selectedIndex >= 0)
	{
		CCTableViewCell* lastCell = table->cellAtIndex(selectedIndex);
		if (lastCell)
		{
			BaseSprite* bg = (BaseSprite*)lastCell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
			bg->setColor(ccWHITE);
		}
	}
	// 再次选自己，代表取消
	if (selectedIndex == cell->getIdx())
	{
		selectedIndex = -1;
	}
	else
	{
		selectedIndex = cell->getIdx();
		BaseSprite* cellbg = (BaseSprite*)cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
		cellbg->setColor(ccc3(160,160,160));
	}

	if(selectedIndex < 0)
	{
		mMenuItemMakeSure->setEnabled(false);
	}
	else
	{
		mMenuItemMakeSure->setEnabled(true);
	}
}

void GongJiangBaoShiHeCheng_BaoShiSelect::disableAllTouchBegin()
{
	mTableView->setTouchEnabled(false);
	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
			if (bg)
			{
				CCMenu* menu = (CCMenu*)bg->getChildByTag(TAG_BaseBody_MENU);
				if (menu)
				{
					menu->setEnabled(false);
				}
			}
		}
	}
}
void GongJiangBaoShiHeCheng_BaoShiSelect::disableAllTouchEnd()
{
	mTableView->setTouchEnabled(true);

	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
			if (bg)
			{
				CCMenu* menu = (CCMenu*)bg->getChildByTag(TAG_BaseBody_MENU);
				if (menu)
				{
					menu->setEnabled(true);
				}
			}
		}
	}
}


GongJiangBaoShiHeCheng_BaoShiSelect::~GongJiangBaoShiHeCheng_BaoShiSelect(void)
{
}

vector<ItemInfo> GongJiangBaoShiHeCheng_BaoShiSelect::getBaoShi()
{
	vector<ItemInfo> baoshi;
//	for(int i=10; i!=1; --i)   //4-10个等级的宝石，每个等级4种颜色
	for(int i=7; i!=1; --i)
	{
		int lv    = ItemsType_Baoshi_lv_1 - 1;
		int hong  = ItemsType_Baoshi_hong_1 - 1;
//		int zhi   = ItemsType_Baoshi_zhi_1 - 1;
		int huang = ItemsType_Baoshi_huang_1 - 1;
		ItemInfo item;
		item.count = 1;

		//lv
		item.type = lv + i;
		baoshi.push_back(item);

		//hong 
		item.type = hong + i;
		baoshi.push_back(item);
// 
// 		//zhi
// 		item.type = zhi + i;
// 		baoshi.push_back(item);

		//huang
		item.type = huang + i;
		baoshi.push_back(item);
	}

	return baoshi;
}