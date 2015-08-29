#ifndef __GRID_SCROLLVIEW_H__
#define __GRID_SCROLLVIEW_H__

#include "cocos2d.h"
#include "MyScrollView.h"
#include "ClickableItem.h"
#include "ClickableItemCreater.h"
using namespace cocos2d;
//一个网格化的ScrollView，网格从左到右，从上到下的存放;
class GridScrollView  : public MyScrollView
{
protected:
	CCArray* gridItemArray;//网格中存放的元素的数组,必须是CCSprite*类型;

	float paddingLeft;//真正属于网格的矩形框，与外面ScrollPort区域的左间距
	float paddingRight;//真正属于网格的矩形框，与外面ScrollPort区域的右间距
	float paddingTop;//真正属于网格的矩形框，与外面ScrollPort区域的上间距
	float paddingButtom;//真正属于网格的矩形框，与外面ScrollPort区域的下间距

	CCSize mScrollViewPortSize;//ScrollPort区域的大小
	//CCSize gridSize;//真正属于网格的矩形框大小，mScrollViewPortSize中套一个gridSize
	CCSize winSize;

	CCPoint scrollViewPortOffset;//ScrollPort区域相对原点的偏移

	int gridNumInX;//网格在X方向上的格子个数
	int gridNumInY;//网格在Y方向上的格子个数
	int gridNum;//网格的格子个数,也就是ScrollView中，一共要存放Item的个数
	float pageInval;//页面之间的间隔是多少
	float pageScrollTime;

	int cal_PageNum;//运行时计算出的页面个数;
	int cal_gridNumInOnePage;//运行时计算出的每一页有多少个网格
	int cal_currentShowedPageIndex;//当前显示的是那哪一页;

	//float pageSizeX;//就是视觉大小mScrollViewPortSize.width
	CCSize cal_containerSize;
	CCSprite* container;//容器的CCSprite

	//如果发生了只是点击的事件，就会调用这个方法
	virtual void onTouch(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint);
    virtual void onTouchBegan(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchMove(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchEnd(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    virtual void onTouchCancle(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint);
    
	virtual void intentToScrollToRightPage();//意图滑到右边页
	virtual void intentToScrollToLeftPage();//意图滑到左边页
	virtual void intentToScrollToCurrentPage();
	//当Grid中的某个Item被点击之后就会调用这个方法，如果用户需要处理，那么必须重写这个方法,默认是交给Clickable
	//一般不用重写
	virtual void itemClicked(ClickableItem* clickedObj); 
	////点击位置在视图的范围外
	//virtual void clickedOutOffViewPort();

	virtual CCSprite* createContainner();//创建容器,内部会把容器自动调整大小
	
	// 获得某一页选中的话，container应该的Offset。
	float getPageOffsetX(int pageIndex);

	/**
	 *	滑动到了某个页面，就会调用，是一个回调接口
	 *	有到这个页面的意图，也会发送的，然后就开始动画移动过去
	 *	或者直接到那里去
	 */
	virtual void pageSelected(int pageIndex);
	/**
	 *	滑动完毕了
	 */
	virtual void pageSelectedEnd(int pageIndex);

	//根据itemToStore的容量，创建物体放入到itemToStore中
	//完成Item在ScrollVIew中的网格布局，一般不需要修改;
	virtual void initGridItem(CCArray* itemToStore,int gridNumInX,int gridNumInY);
public:
	//构造一个网格，可翻页的ScrollView。需要指定承载网格的可视区域大小；创建的网格中，格子的个数；网格的横向和纵向的格子个数
	//默认网格会放在视口的中心，视口也放在屏幕的中心;
	//gridPadding 就是一页中的Grid所占用的页面的padding
	GridScrollView(CCSize viewPortSize,int gridNum,int gridNumInX,int gridNumInY,float gridPadding = 10.0f,CCPoint scrollViewPortOffset = CCPointZero,float pageInval = 40.0f);
	~GridScrollView();

	//准备完毕就调用这个方法去显示
	//这个方法之后，如果再有修改pading之类的，就需要refactorGridSize
	void prepareToDisplay();
	//设置Item的生成器
	void setClickableCreater(ClickableItemCreater* creater);
	void setPadding(float padding,bool refactorGridSize = true);		//设置 真正属于网格的矩形框，与外面ScrollPort区域的四周间距
	void setPaddingRight(float padding,bool refactorGridSize = true);	//设置 真正属于网格的矩形框，与外面ScrollPort区域的右间距
	void setPaddingLeft(float padding,bool refactorGridSize = true);	//设置 真正属于网格的矩形框，与外面ScrollPort区域的左间距
	void setPaddingTop(float padding,bool refactorGridSize = true);		//设置 真正属于网格的矩形框，与外面ScrollPort区域的上间距
	void setPaddingButtom(float padding,bool refactorGridSize = true);	//设置 真正属于网格的矩形框，与外面ScrollPort区域的下间距
	void setGridNumInX(int gridNumInX);									//设置网格的横向格子个数
	void setGridNumInY(int gridNumInY);									//设置网格的纵向格子个数

	void setPageScrollDur(float dur);									//设置翻页耗时

	//简单的翻页，就是根据当前的页面大小，页面间隔翻页，父类的scrollToPage是可以动态的指定页面大小，页面间隔而翻页
	// 
	void scrollToPageSimple(int pageIndex,float time);

	// 翻页，使用默认的速度
	void scrollToPageSimple(int pageIndex);

	//根据gridIndex，找到该按钮处于的页面,注意index这些都是从0开始的，
	int getPageIndex(int gridIndex);
	// 获得该元素在Grid中的页面位置,-1代表没有找到
	int getPageIndex(CCSprite* item);
	//设置某页的元素的透明度(只是GirdItem的透明度)
	void setPageItemOpacity(int pageIndex,GLubyte opacity);
	//设置page的某一个元素的透明度
	void setGridItemOpacity(CCSprite* item,GLubyte opacity);

	CCArray* getPageItems(int pageIndex);			// 获得某页的所有Item

private:
	//重新构建自己，当改了padding，网格属性等都需要调用这个方法，才会有效
	void reBuildSelf();
	ClickableItemCreater* itemCreator;
};

#endif