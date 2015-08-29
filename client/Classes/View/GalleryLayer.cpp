#include "GalleryLayer.h" 

using namespace cocos2d; 
using namespace cocos2d::extension; 

CCPoint distance = ccp(10,0);

GalleryLayer::GalleryLayer()
{
	mCallBack = NULL;
	mDataSet = NULL;
	itemArray = CCArray::createWithCapacity(5);
	itemArray->retain();
	mCurrentPageIndex = 0;
	mPageNum = 0;
	pageWidth = 0;
	touchEffective = false;
}
GalleryLayer* GalleryLayer::create(CCSize size/*, CCNode* container*/)
{
	CCLayer* container  = CCLayer::create();
	GalleryLayer* lay = new GalleryLayer();
	lay->initWithViewSize(size,container);
	lay->container = container;
	lay->autorelease();
	lay->setDirection(kCCScrollViewDirectionHorizontal);
	lay->pageWidth = size.width + distance.x;

	container->setContentSize(size);
	container->setPosition(0,size.height/2);


	return lay;
}
void GalleryLayer::setDataSetAndCallBack(GalleryDataSet* dataSet, GalleryCallBack* callBack)
{
	mCallBack = callBack;
	mDataSet = dataSet;
}

void GalleryLayer::reloadData()
{
	CCPoint offset = ccp(0,0);
	

	if (mDataSet)
	{
		mPageNum = mDataSet->getItemNum();

		for (int i=0;i<mPageNum;i++)
		{
			CCNode* item = mDataSet->createItem(i);
			container->addChild(item);
			itemArray->addObject(item);

			offset.x += item->getContentSize().width/2;
			//offset.y += item->getContentSize().height/2;
			item->setPosition(offset);
			offset.x += item->getContentSize().width/2;
			//offset.y += item->getContentSize().height/2;
			offset.x += distance.x;
			offset.y += distance.y;
		}
	}
}

bool GalleryLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) 
{ 
	touchEffective = CCScrollView::ccTouchBegan(pTouch,pEvent);
	m_touchPoint = CCDirector::sharedDirector()->convertToGL(pTouch->getLocationInView()); 
	return true; 
} 

void GalleryLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) 
{ 
	CCScrollView::ccTouchMoved(pTouch,pEvent);
} 

void GalleryLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) 
{ 
	CCScrollView::ccTouchEnded(pTouch,pEvent);
	// 不让他自己滑动
	unscheduleAllSelectors();

	if (touchEffective)
		tryAutoScroll(m_touchPoint,pTouch->getLocation());
	return;
} 

void GalleryLayer::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) 
{ 
	CCScrollView::ccTouchCancelled(pTouch,pEvent);
	// 不让他自己滑动
	unscheduleAllSelectors();

	if (touchEffective)
		tryAutoScroll(m_touchPoint,pTouch->getLocation());
	return;

} 
void GalleryLayer::scrollToPage(int pageIndex,bool animation /* = true */)
{
	if (pageIndex < mPageNum && pageIndex >= 0)
	{
		mCurrentPageIndex = pageIndex;
		scrollToCurrentPage(animation);
	}
}
void GalleryLayer::scrollToCurrentPage(bool animation /* = true */)
{
	CCPoint offset = ccp( -mCurrentPageIndex*pageWidth,container->getPositionY());
	if (animation)
	{
		CCAction* action = CCSequence::create(
			CCMoveTo::create(0.2f,offset),
			CCCallFuncN::create(this,SEL_CallFuncN(&GalleryLayer::scrollEnd)),
			NULL
			);
		container->stopAllActions();
		container->runAction(action);
	}else
	{
		container->setPosition(offset);
		scrollEnd(NULL);
	}

}
void GalleryLayer::scrollEnd(CCNode* container)
{
	if (mCallBack)
	{
		mCallBack->onGallerySelected(mCurrentPageIndex,(CCNode*)itemArray->objectAtIndex(mCurrentPageIndex));
	}
}
void GalleryLayer::tryAutoScroll(CCPoint startPoint,CCPoint endPoint)
{
	float dis = ccpDistance(startPoint,endPoint);
	bool goToRight = endPoint.x - startPoint.x < 0;
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize(); 
	if (dis > visibleSize.width/3)
	{
		if (goToRight && mCurrentPageIndex + 1 < mPageNum)
		{
			mCurrentPageIndex++;
		}
		else if(!goToRight && mCurrentPageIndex - 1 >= 0)
		{
			mCurrentPageIndex--;
		}
	}
	scrollToCurrentPage();

}
GalleryLayer::~GalleryLayer()
{
	itemArray->release();
}