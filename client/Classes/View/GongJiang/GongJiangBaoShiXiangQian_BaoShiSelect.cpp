
#include "GongJiangBaoShiXiangQian_BaoShiSelect.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "font.h"

#define TAG_BG_MASK_PIC 22221

GongJiangBaoShiXiangQian_BaoShiSelect* GongJiangBaoShiXiangQian_BaoShiSelect::create(GongJiangBaoShiXiangQian* xiangqianLayer)
{
	GongJiangBaoShiXiangQian_BaoShiSelect* layer = new GongJiangBaoShiXiangQian_BaoShiSelect(xiangqianLayer);
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

GongJiangBaoShiXiangQian_BaoShiSelect::GongJiangBaoShiXiangQian_BaoShiSelect(GongJiangBaoShiXiangQian* xiangqianLayer)
{
	//初始化
	selectedIndex = -1;

	mXiangqianLayer = xiangqianLayer;

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

		setItemsData(WuPinManager::getManager()->getAllBaoshi());
	}

	//创建按钮
	{
		mMenuItemCancel = CCMenuItemImage::create(
			"xuanze_zhuang_bei_quxiao.png",
			"xuanze_zhuang_bei_quxiao_select.png",
			"xuanze_zhuang_bei_quxiao_select.png",
			this,SEL_MenuHandler(&GongJiangBaoShiXiangQian_BaoShiSelect::menuItemCancelClicked));
		mMenuItemMakeSure = CCMenuItemImage::create(
			"xuanze_zhuang_bei_queding.png",
			"xuanze_zhuang_bei_queding_select.png",
			"xuanze_zhuang_bei_queding_select.png",
			this,SEL_MenuHandler(&GongJiangBaoShiXiangQian_BaoShiSelect::menuItemMakeSureClicked));


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
	CCLabelTTF *la = CCLabelTTF::create(LFStrings::getValue("XiangQian").c_str(), fontStr_kaiti, 20);
	addChild(la, 1);
	la->setPosition(ccp(340, 804));
}



void GongJiangBaoShiXiangQian_BaoShiSelect::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	// 先移除上一个点击的
	if (selectedIndex >= 0)
	{
		CCTableViewCell* lastCell = table->cellAtIndex(selectedIndex);
		if (lastCell)
		{
			BaseSprite* bg = (BaseSprite*)lastCell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
			//bg->initWithFile("shangdian_shangpin_bg.png");
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
		//cellbg->initWithFile("shangdian_shangpin_bg_select.png");
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


// 
// void GongJiangBaoShiXiangQian_BaoShiSelect::refreshItem(CCNode* item,int index)
// {
// 	MainLayerItemsBaseBody::refreshItem(item,index);
// 
// 	if (index != selectedIndex)
// 	{
// 		((BaseSprite*)item)->initWithFile("shangdian_shangpin_bg.png");
// 	}else
// 	{
// 		((BaseSprite*)item)->initWithFile("shangdian_shangpin_bg_select.png");
// 	}
// }

void GongJiangBaoShiXiangQian_BaoShiSelect::menuItemCancelClicked(CCObject* obj)
{
	mFooterLayer->dismmisPopUp();
}

void GongJiangBaoShiXiangQian_BaoShiSelect::menuItemMakeSureClicked(CCObject* obj)
{
	unsigned int itemType = 0;
	if(selectedIndex >= 0)
	{
		itemType = mItemsVec[selectedIndex].type;
	}
	mXiangqianLayer->setSelectedBaoshiId(itemType);
	

	mFooterLayer->dismmisPopUp();
}

void GongJiangBaoShiXiangQian_BaoShiSelect::disableAllTouchBegin()
{

}
void GongJiangBaoShiXiangQian_BaoShiSelect::disableAllTouchEnd()
{

}


GongJiangBaoShiXiangQian_BaoShiSelect::~GongJiangBaoShiXiangQian_BaoShiSelect(void)
{
}
