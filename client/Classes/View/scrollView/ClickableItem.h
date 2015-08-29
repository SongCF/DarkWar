#ifndef __ClickableItem_H__
#define __ClickableItem_H__

#include "cocos2d.h"
#include "IBorder.h"
using namespace cocos2d;
//一个可以拾取点击的元素的抽象类
class ClickableItem  : public CCSprite,IBorder
{
public:
	static const int ENABLE = 0x0001;//末位是1，那么有效，末位是0，那么无效
	static const int CLICKABLE = 0x0010;//倒数第二位是1有效，0无效

	ClickableItem()
	{
		//初始化为有效，可点击;
		myState = ENABLE | CLICKABLE;
		initScale = -1;
	}

	//如果点击的位置在我的位置里面，就应该（会）回调这个方法;
	//至于如何处理，就看用户需要，用户根据当前的状态去更改Item的显示，动作等;
	virtual void clickedMe()
	{
        leaveMe();
	}
    //触摸到了我，那么默认是放大
    virtual void touchMe()
    {
        anchorPointBk = getAnchorPoint();
        isIgnoreAnchorBk = isIgnoreAnchorPointForPosition();
		initScale = getScale();
        setScale(1.2f*initScale);
    }
    //手指离开了我，默认是变回原大小
    virtual void leaveMe()
    {
		if (initScale != -1)
		{
			setScale(1.0f*initScale);
		}
    }
    
	//获得我自己的可以拾取用户点击的范围,默认就返回我的边框
	virtual std::list<CCRect> getBounds()
	{
		std::list<CCRect> rects;
		CCRect r = boundingBox();
		rects.insert(rects.end(),r);
		return rects; 
	}

	void setEnable(bool enbale)
	{
		if (enbale)
		{
			myState = myState | ENABLE;
		}else
		{
			myState = myState & ~ENABLE;
		}
	}
	void setClickable(bool clickable)
	{
		if (clickable)
		{
			myState = myState | CLICKABLE;
		}else
		{
			myState = myState & ~CLICKABLE;
		}
	}

	//该元素是否为有效，无效的话就是对于灰色的图片
	//元素无效的话那么肯定不可点击;有效的话，还区分是否可以点击；
	//有就是4个状态：没有锁的有效，没有锁的无效，有锁的有效，有锁的无效
	bool isEnable()
	{
		int temp = myState & ~ENABLE;
		if ( temp == ENABLE)
		{
			return true;
		}else
		{
			return false;
		}
	}

	//元素是否可以点击，一般是是否有加锁的数据，也就是元素有效，但不一定可以点击;
	//元素可以点击，一定是有效的;
	bool isClickable()
	{
		int temp = myState & ~CLICKABLE;
		if ( temp == CLICKABLE )
		{
			return true;
		}else
		{
			return false;
		}
	}
	//得到状态:
	//
	int getState()
	{
		return myState;
	}
private:
	int myState;
	float initScale;
protected:
    CCPoint anchorPointBk;
    bool isIgnoreAnchorBk;
};




#endif