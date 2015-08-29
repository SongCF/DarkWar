#ifndef __BaseScrollView_H__
#define __BaseScrollView_H__

#include "cocos2d.h"
#include "MyScrollView.h"
#include "BaseAdapter.h"
#include "ClickableItem.h"

class BaseScrollView  : public MyScrollView
{
public:
	BaseScrollView(BaseAdapter* adapter,CCSize viewPortSize);
	~BaseScrollView();
	void setAdapter(BaseAdapter* adapter);
private:
	BaseAdapter* adapter;
	int cal_currentShowedPageIndex;
	int cal_PageNum;
	float pageInval;
	float pageScrollTime;
	int itemNum;
	//如果发生了只是点击的事件，就会调用这个方法
	virtual void onTouch(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint);
    virtual void onTouchBegan(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchMove(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    
	virtual void intentToScrollToRightPage();//意图滑到右边页
	virtual void intentToScrollToLeftPage();//意图滑到左边页
	virtual void intentToScrollToCurrentPage();
	//当Grid中的某个Item被点击之后就会调用这个方法，这方法直接调用Item的ClickMe去实现
	virtual void itemClicked(ClickableItem* clickedObj,int index); 
};


#endif