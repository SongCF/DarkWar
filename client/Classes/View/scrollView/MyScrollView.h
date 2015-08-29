#ifndef __MyScrollView_H__
#define __MyScrollView_H__

#include "cocos2d.h"
#include <vector>
#include "cocos-ext.h" 
USING_NS_CC; 
USING_NS_CC_EXT; 
using namespace std;
//
//一个自扩展的CCScrollView类。用于实现对只触摸和滑动的分开
//完成全局GL坐标转换成ScrollView身上的本身局部相对坐标（原点在左下角）
//
class MyScrollView: public CCScrollView
{
protected:
	CCPoint convertToScrollViewRelativePoint(CCPoint& glPoint);//把屏幕的GL左边转换成相对于当前ScrollView的坐标
	bool clicked(CCSprite* sprite,CCPoint clickPos);//用于判断clickPos是否在sprite里面（注意，只对ScrollView的孩子Sprite有用）
	float getZeroY(){return zeroY;}//得到可视区域左下角的Y坐标
	float getZeroX(){return zeroX;}//得到可视区域左下角的X坐标

	/**
	 *	获得某一页对应的Offset
	 */
	float getPageOffsetX(int pageIndex,float pageSizeX,float pageInval_X);
	
	/**
	 *	拦截页面的滚动，true就拦截，就不会滚动
	 *	默认不拦截.
	 */
	virtual bool pageScrollIntercept(int pageIndex);
	/**
	 * 这里讲整个ScrollView的container容量抽象成页与页的组合，页面之间有一个间隔pageInval。
	 * 所以只需要告诉我你的界面大小，页面间的间隔，那么此函数就会把算出来的页面位置移动到屏幕中间
	 * 当然如果在往中间移动的意图中，发现已经把容器内容移动完毕了，那么就会只移动这里，也就是整体的移动范围就是
	 * 容器的sizeX。
	 * 
	 * 注意这里的pageInval_X是指两页之间的距离，如ABC，即为AB之间BC之间的距离，而A左边是没有距离的，要实现A左边或者C右边也有距离
	 * 那么也很简单，直接把页面设置大点就是了。或者你在抽象的页面里面排版的时候留下空间就是了。
	 */
	void srollToPage(int pageIndex,float pageSizeX,float pageInval_X,float duration);			//滑动到某页面,从0开始。;
	//滑动到，每一页，speed：每一秒多少像素
	//如果 speed<=0 那么就是立即到达这个页面
	void srollToPageBySpeed(int pageIndex,float pageSizeX,float pageInval_X,float speed);		//滑动到某页面,从0开始。;
	
	void scrollToChild(CCNode* child);


	/**
	 *	滑动到了某个页面，就会调用，是一个回调接口
	 *	有到这个页面的意图，也会发送的，然后就开始动画移动过去
	 *	或者直接到那里去
	 */
	virtual void pageSelected(int pageIndex){};
	/**
	 *	滑动完毕了
	 */
	virtual void pageSelectedEnd(int pageIndex){};
public:
	~MyScrollView();
	MyScrollView(CCSize viewPortSize);
	//触摸的处理，一般不用再重写了
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);//触摸的处理，一般不用再重写了
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);//触摸的处理，一般不用再重写了
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);//触摸的处理，一般不用再重写了

	virtual void intentToScrollToRightPage(){}//意图滑到右边页
	virtual void intentToScrollToLeftPage(){}//意图滑到左边页
	virtual void intentToScrollToCurrentPage(){}

	
	//如果发生了只是点击的事件，就会调用这个方法,是抬起了之后调用的
	virtual void onTouch(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint);
	//点击失败：比如刚点击时，点击点在自己可以识别的外面了就会调用。
	virtual void onTouchFalse(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint);
    
    //下面4个方法对应ccTouch的4个方法；
	//如果发生了移动事件就会调用这个方法
	virtual void onTouchMove(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchEnd(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchCancle(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchBegan(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);

	//设置是否可以滚动，true则可以滚动
	void setEnableScroll(bool enbale);

private:
	CCSize winSize;
	CCSize scrollViewPortSize;
	float zeroX;
	float zeroY;
	CCTouch *m_pTouch;
	CCEvent *m_pEvent;
	CCPoint m_tBeginPos;
	bool moving;//是否有moving发生
	bool canScroll;

	float  mInstantaneousVelocity;	// 瞬时速度，在抬起手指之前的瞬时速度
	
	vector<CCPoint> movePointRecord;
	void addRecord(CCPoint record);
	float getInstantaneousVelocity();

	int mSelectedIndex;
	//
	void scrollActionEnd(CCNode* container);
};

#endif