#pragma once

#include "BaseLayer.h"
#include "cocos-ext.h"

using namespace cocos2d::extension;
using namespace cocos2d;


class ListViewCreator
{
public:
	// 创建第i个元素
	virtual BaseSprite* getItemContentByIndex(unsigned int index){return BaseSprite::create();};
	// 刷新第i个元素，也可以简单的用getItemContentByIndex，再创建一个就是了
	virtual void refreshItem(CCNode* item,int index){};
};


// listView的数据
// 	kCCScrollViewDirectionNone = -1,
//	kCCScrollViewDirectionHorizontal = 0,
//	kCCScrollViewDirectionVertical,
//	kCCScrollViewDirectionBoth
struct ListViewDataSource
{
	int listItemNum;
	CCSize itemDefaultSize;
	CCScrollViewDirection dir;
	// listView的容器，很重要！他就决定了ListView的可视范围。listView也刚好在container中铺满
	// 这个会被加入到ListViewLayer里面，所以外界不要在addChild
	// 这里默认会有一个透明的,全屏的大小的
	// 我们可以setContentSize去改变他的大小
	BaseSprite* listViewContainer;	
	ListViewCreator* creator;

	ListViewDataSource()
	{
		listItemNum = 0;
		itemDefaultSize = CCSizeMake(0,0);
		dir = kCCScrollViewDirectionVertical;
		listViewContainer = BaseSprite::create();
		listViewContainer->setContentSize(CCDirector::sharedDirector()->getVisibleSize());
		listViewContainer->setOpacity(0);
		creator = NULL;
	}
};

/**
 *	一个ListView的layer
 */
class ListViewLayer : public BaseLayer , public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	ListViewLayer();
	void reloadData(ListViewDataSource source);

	// 获得某个元素，NULL就是已经释放了
	CCNode* getListItemByIndex(int index);

	~ListViewLayer(void);
private:

	// 这些都不能重写了，因为这一层已经封装好了

	// CCTableViewDataSource
	CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	unsigned int numberOfCellsInTableView(CCTableView *table);

	// CCScrollViewDelegate
	void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
	void tableCellHighlight(CCTableView* table, CCTableViewCell* cell);
	void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell);
	void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell);

protected:

	CCTableView* mTableView;
	ListViewDataSource mSource;

	// CCTableView
	virtual void scrollViewDidScroll(CCScrollView* view);
	virtual void scrollViewDidZoom(CCScrollView* view);

	// 元素被点击
	virtual void listItemTouched(CCNode* listItem,int index){};

	// 禁用触摸
	virtual void disableAllTouchBegin();
	// 启用触摸
	virtual void disableAllTouchEnd();
};

