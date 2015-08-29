#include "ListViewLayer.h"

#define TAG_LIST_CELL_ITEM_WRAP 11111
//#define TAG_LIST_CELL_ITEM		11112
#define TAG_LIST_CONTAINER		11113


ListViewLayer::ListViewLayer()
{
	mTableView = NULL;
}
void ListViewLayer::reloadData(ListViewDataSource source)
{
	// 容器不能为空
	CCAssert(source.listViewContainer != NULL,"");

	// 目前已经有的容器
	CCNode* container = getChildByTag(TAG_LIST_CONTAINER);


	if (source.listViewContainer != container)
	{
		// 把旧的移除
		if (container)
			container->removeFromParentAndCleanup(true);

		// 添加新的
		mSource = source;
		addChild(mSource.listViewContainer,0,TAG_LIST_CONTAINER);
		mTableView = CCTableView::create(this, mSource.listViewContainer->getContentSize());
		mTableView->setDirection(mSource.dir);
		mTableView->setPosition(ccp(0,0));
		mTableView->setDelegate(this);
		mSource.listViewContainer->addChild(mTableView);
	}

	mSource = source;
	mTableView->reloadData();
}
ListViewLayer::~ListViewLayer(void)
{
}

void ListViewLayer::scrollViewDidScroll(CCScrollView* view) 
{
}
void ListViewLayer::scrollViewDidZoom(CCScrollView* view) 
{
}

CCNode* ListViewLayer::getListItemByIndex(int index)
{
	CCTableViewCell* lastCell = mTableView->cellAtIndex(index);
	if (lastCell)
	{
		CCNode* wrap = lastCell->getChildByTag(TAG_LIST_CELL_ITEM_WRAP);
		return (CCNode*)wrap->getChildren()->objectAtIndex(0);
	}
	// 已经释放了
	return NULL;
}

void ListViewLayer::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	listItemTouched(getListItemByIndex(cell->getIdx()),cell->getIdx());
}
void ListViewLayer::tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell)
{
}
void ListViewLayer::tableCellHighlight(CCTableView* table, CCTableViewCell* cell)
{
}

void ListViewLayer::tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell)
{
}

// 禁用触摸
void ListViewLayer::disableAllTouchBegin()
{
	mTableView->setTouchEnabled(false);
}
// 启用触摸
void ListViewLayer::disableAllTouchEnd()
{
	mTableView->setTouchEnabled(true);
}


CCTableViewCell* ListViewLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();
	if (cell == NULL) {
		cell = new CCTableViewCell();
		cell->autorelease();

		if (mSource.creator)
		{
			CCNode* item = mSource.creator->getItemContentByIndex(idx);

			// 包裹
			CCNode* wraped = CCNode::create();
			wraped->setContentSize(item->getContentSize());
			wraped->addChild(item,0);

			// 有个0.5,0.5的偏移
			wraped->setPosition(ccp(wraped->getContentSize().width/2,wraped->getContentSize().height/2));

			cell->addChild(wraped,0,TAG_LIST_CELL_ITEM_WRAP);
		}else
		{
			CCAssert(false,"");
		}

		CCLog("ListViewLayer::tableCellAtIndex ---> create!");
	}
	else
	{
		CCLog("ListViewLayer::tableCellAtIndex ---> refesh!");

		CCNode* warped = cell->getChildByTag(TAG_LIST_CELL_ITEM_WRAP);
		CCNode* item = (CCNode*)warped->getChildren()->objectAtIndex(0);

		if (mSource.creator)
		{
			mSource.creator->refreshItem(item,idx);
		}else
		{
			CCAssert(false,"");
		}
	}

	return cell;
}

CCSize ListViewLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return mSource.itemDefaultSize;
}
unsigned int ListViewLayer::numberOfCellsInTableView(CCTableView *table)
{
	return mSource.listItemNum;
}