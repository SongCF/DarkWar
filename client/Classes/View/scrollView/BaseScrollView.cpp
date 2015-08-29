#include "BaseScrollView.h"

using namespace extension;

BaseScrollView::BaseScrollView(BaseAdapter* adapter,CCSize viewPortSize):MyScrollView(viewPortSize)
{
	this->adapter = adapter;
	cal_currentShowedPageIndex = 0;
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	itemNum = adapter->getItemCount();
	CCSize containerSize = adapter->getContainerSize();
	CCSprite* container = adapter->getContainer();

	cal_PageNum = adapter->getPageNum();
	pageInval = adapter->getPageInval();
	pageScrollTime = adapter->getPageScrollTime();

	initWithViewSize(viewPortSize,container);

	setPosition(ccp(winSize.width/2-viewPortSize.width/2,winSize.height/2-viewPortSize.height/2) );
	container->setPosition( ccp(winSize.width/2-containerSize.width/2,winSize.height/2-containerSize.height/2) );

	this->setDirection(kCCScrollViewDirectionHorizontal);

	this->setEnableScroll(adapter->canScroll());
	for (int i=0;i<itemNum;i++)
	{
		addChild(adapter->getItem(i));
	}
}

BaseScrollView::~BaseScrollView()
{
}
void BaseScrollView::intentToScrollToLeftPage(){
	cal_currentShowedPageIndex--;
	if (cal_currentShowedPageIndex<0)
		cal_currentShowedPageIndex = 0;
	srollToPage(cal_currentShowedPageIndex,getViewSize().width,pageInval,pageScrollTime);
}

void BaseScrollView::intentToScrollToCurrentPage()
{
	srollToPage(cal_currentShowedPageIndex,getViewSize().width,pageInval,pageScrollTime);
}

void BaseScrollView::intentToScrollToRightPage(){
	cal_currentShowedPageIndex++;
	if (cal_currentShowedPageIndex >= cal_PageNum)
		cal_currentShowedPageIndex = cal_PageNum-1;
	srollToPage(cal_currentShowedPageIndex,getViewSize().width,pageInval,pageScrollTime);
}

void BaseScrollView::onTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent, cocos2d::CCPoint glPoint)
{
    for (int i=0;i<itemNum;i++)
	{
		CCPoint scrollPoint = convertToScrollViewRelativePoint(glPoint);
		ClickableItem* temp = adapter->getItem(i);
		if (clicked(temp,scrollPoint))
		{
			temp->touchMe();
			break;
		}
	}
}
void BaseScrollView::onTouchMove(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent, cocos2d::CCPoint glPoint)
{
    for (int i=0;i<itemNum;i++)
	{
		CCPoint scrollPoint = convertToScrollViewRelativePoint(glPoint);
		ClickableItem* temp = adapter->getItem(i);
		if (!clicked(temp,scrollPoint))
		{
			temp->leaveMe();
		}
	}
}

void BaseScrollView::onTouch(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint)
{
	pTouch = pTouch;pEvent = pEvent;
	//m_tBeginPos
	for (int i=0;i<itemNum;i++)
	{
		CCPoint scrollPoint = convertToScrollViewRelativePoint(glClickPoint);
		ClickableItem* temp = adapter->getItem(i);
		if (clicked(temp,scrollPoint))
		{
			itemClicked(temp,i);
			break;
		}
	}
}

void BaseScrollView::itemClicked(ClickableItem* clickedObj,int index)
{
	clickedObj->clickedMe();
}