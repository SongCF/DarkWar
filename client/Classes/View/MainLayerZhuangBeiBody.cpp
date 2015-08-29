#include "MainLayerZhuangBeiBody.h"
#include "MainLayerZhuangBeiBaseBody.h"

#define TAG_TABLE_ITEM 1112
#define  TAG_MENU      1110


// 元素的图片
static string itemFiles[] = {
	"mianban_zhuangbei_wuqi.png",
	"mianban_zhuangbei_fangju.png",
	"mianban_zhuangbei_shipin.png",
	"mianban_zhuangbei_suoyou.png"
};
static string itemFiles_select[] = {
	"mianban_zhuangbei_wuqi_select.png",
	"mianban_zhuangbei_fangju_select.png",
	"mianban_zhuangbei_shipin_select.png",
	"mianban_zhuangbei_suoyou_select.png"
};
//06-14 20:50:24.949: D/cocos2d-x debug info(6365): MainLayerZhuangBeiBody::tableCellAtIndex new --> index = [4],file = [function_knapsack.png]

MainLayerZhuangBeiBody::MainLayerZhuangBeiBody(void)
{
	container = BaseSprite::create("mianban_zhuangbei.png");
	addChild(container);
	container->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));

	selectedIndex = -1;

	tableView = CCTableView::create(this, CCSizeMake(636,84));
	tableView->setDirection(kCCScrollViewDirectionHorizontal);
	tableView->setPosition(ccp(10,763));
	tableView->setDelegate(this);
	container->addChild(tableView);
	tableView->reloadData();

	// 禁用滑动
	tableView->setBounceable(false);

	selectedContaint = NULL;
	
}

void MainLayerZhuangBeiBody::selectAtIndex(unsigned int index)
{
	// 0 是第一个  所有装备
	CCTableViewCell* currentCell = tableView->cellAtIndex(index);
	CCTableViewCell* lastCell = tableView->cellAtIndex(selectedIndex);

	//if (selectedIndex != index)
	{
		if (lastCell!=NULL)
		{
			CCMenuItemImage* itemImage = (CCMenuItemImage*)lastCell->getChildByTag(TAG_MENU)->getChildByTag(lastCell->getIdx());
			int idx = itemImage->getTag();
			itemImage->initWithNormalImage(
				itemFiles[idx].c_str()
				,itemFiles_select[idx].c_str()
				,NULL, this, menu_selector(MainLayerZhuangBeiBody::menuItemClicked));
		}

		if (currentCell!=NULL)
		{
			CCMenuItemImage* itemImage = (CCMenuItemImage*)currentCell->getChildByTag(TAG_MENU)->getChildByTag(currentCell->getIdx());
			int idx = itemImage->getTag();
			itemImage->initWithNormalImage(
				itemFiles_select[idx].c_str()
				,itemFiles[idx].c_str()
				,NULL, this, menu_selector(MainLayerZhuangBeiBody::menuItemClicked));
		}

		selectedIndex = index;

		BaseLayer* content = createContentAtIndex(index);

		if (selectedContaint)
		{
			selectedContaint->removeFromParent();
		}
		if (content)
		{
			container->addChild(content);
			content->setPosition(ccp(0,0));
		}
		selectedContaint = content;
	}
}

void MainLayerZhuangBeiBody::menuItemClicked(CCObject *pSender)
{
	CCMenuItemImage* itemImage = (CCMenuItemImage*)pSender;
	int idx = itemImage->getTag();

	selectAtIndex(idx);
}

void MainLayerZhuangBeiBody::initFoot(MainLayerBase* footerLayer)
{
	IMainLayerBody::initFoot(footerLayer);
	selectAtIndex(0);
}

void MainLayerZhuangBeiBody::disableAllTouchBegin()
{
	if (selectedContaint)
	{
		selectedContaint->disableAllTouchBegin();
	}
	tableView->setTouchEnabled(false);

	for(int i=0; i!=numberOfCellsInTableView(tableView); ++i)
	{
		((CCMenuItemImage*) tableView->cellAtIndex(i)->getChildByTag(TAG_MENU)->getChildByTag(i))->setEnabled(false);
	}
}
void MainLayerZhuangBeiBody::disableAllTouchEnd()
{
	if (selectedContaint)
	{
		selectedContaint->disableAllTouchEnd();
	}
	tableView->setTouchEnabled(true);

	for(int i=0; i!=numberOfCellsInTableView(tableView); ++i)
	{
		((CCMenuItemImage*) tableView->cellAtIndex(i)->getChildByTag(TAG_MENU)->getChildByTag(i))->setEnabled(true);
	}
}
BaseLayer* MainLayerZhuangBeiBody::createContentAtIndex(unsigned int index)
{
	MainLayerZhuangBeiBaseBody* ret = NULL;
	if (index >= sizeof(itemFiles)/sizeof(string))
	{
		//CCLog("MainLayerZhuangBeiBody::createContentAtIndex --> index >= [%d] (sizeof(itemFiles)/sizeof(string))",sizeof(itemFiles)/sizeof(string));
		return ret;
	}
	switch (index)
	{
	case 0://
		ret = new MainLayerZhuangBeiAllBody(Type_WuQi);
		ret->autorelease();
		break;
	case 1://
		ret = new MainLayerZhuangBeiAllBody(Type_FangJu);
		ret->autorelease();
		break;
	case 2://
		ret = new MainLayerZhuangBeiAllBody(Type_ShiPin);
		ret->autorelease();
		break;
	case 3://全部
		ret = new MainLayerZhuangBeiAllBody(Type_All);
		ret->autorelease();
		break;
	default:
		ret = new MainLayerZhuangBeiBaseBody(From_ZhuangBeiList);
		ret->autorelease();

		CCLabelTTF *label = CCLabelTTF::create("Zhuang Bei Not implement!", "Helvetica", 40.0);
		label->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));
		ret->addChild(label);
		break;
	}

	ret->setFooter(mFooterLayer);
	ret->setTableViewPos(ccp(8,74));

	return ret;
}

void MainLayerZhuangBeiBody::refreshLayer()
{
	selectAtIndex(selectedIndex);
}

void MainLayerZhuangBeiBody::scrollViewDidScroll(CCScrollView* view) 
{
}
void MainLayerZhuangBeiBody::scrollViewDidZoom(CCScrollView* view) 
{
}

void MainLayerZhuangBeiBody::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
//	selectAtIndex(cell->getIdx());
}

void MainLayerZhuangBeiBody::tableCellHighlight(CCTableView* table, CCTableViewCell* cell)
{
/*	if (selectedIndex!=cell->getIdx())
	{
		CCMenuItemImage* itemImage = (CCMenuItemImage*)cell->getChildByTag(TAG_MENU)->getChildByTag(cell->getIdx());
		int idx = itemImage->getTag();
		itemImage->initWithNormalImage(
			itemFiles_select[idx].c_str()
			,itemFiles[idx].c_str()
			,NULL, this, menu_selector(MainLayerZhuangBeiBody::menuItemClicked));
	}
*/
}

void MainLayerZhuangBeiBody::tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell)
{
// 	if (selectedIndex!=cell->getIdx())
// 	{
// 		CCMenuItemImage* itemImage = (CCMenuItemImage*)cell->getChildByTag(TAG_MENU)->getChildByTag(cell->getIdx());
// 		int idx = itemImage->getTag();
// 		itemImage->initWithNormalImage(
// 			itemFiles[idx].c_str()
// 			,itemFiles_select[idx].c_str()
// 			,NULL, this, menu_selector(MainLayerZhuangBeiBody::menuItemClicked));
// 	}
}


CCSize MainLayerZhuangBeiBody::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	//CCLog("MainLayerZhuangBeiBody::tableCellSizeForIndex --> index = [%d]",idx);
	return CCSizeMake(159, 87);
}

CCTableViewCell* MainLayerZhuangBeiBody::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCString *string = CCString::createWithFormat("%d", idx);
	CCTableViewCell *cell = NULL;  //table->dequeueCell();
	if (cell == NULL) 
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}
	else
	{
		cell->removeAllChildren();
	}
	//
	CCMenuItemImage *itemImage = CCMenuItemImage::create(
		itemFiles[idx].c_str()
		,itemFiles_select[idx].c_str()
		, this, SEL_MenuHandler(&MainLayerZhuangBeiBody::menuItemClicked));
	CCMenu* menu = CCMenu::create();
	cell->addChild(menu, 0, TAG_MENU);
	menu->setPosition(CCPointZero);
	menu->addChild(itemImage, 0, idx);
	itemImage->setPosition(ccp(itemImage->getContentSize().width/2, itemImage->getContentSize().height/2));


	return cell;
}

unsigned int MainLayerZhuangBeiBody::numberOfCellsInTableView(CCTableView *table)
{
	// 第一步
	return sizeof(itemFiles)/sizeof(string);
}


MainLayerZhuangBeiBody::~MainLayerZhuangBeiBody(void)
{
}
