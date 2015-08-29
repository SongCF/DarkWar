#ifndef __BaseAdapter_H__
#define __BaseAdapter_H__

#include "cocos2d.h"
#include "ClickableItem.h"
using namespace cocos2d;

class BaseAdapter
{
public:
	BaseAdapter(CCArray* itemList)
	{
		mItemList = itemList;
		mItemList->retain();
	}
	~BaseAdapter()
	{
		CC_SAFE_RELEASE(mItemList);
	}
	
	virtual ClickableItem* getItem(int index) = 0;

	virtual int getItemCount() = 0;

	virtual CCSprite* getContainer() = 0;

	virtual CCSize getContainerSize() = 0;

	virtual int getPageNum() = 0;

	virtual float getPageInval() = 0;

	virtual float getPageScrollTime() = 0;

	virtual bool canScroll()
	{
		return true;
	}
private:
	CCArray* mItemList;
};


#endif