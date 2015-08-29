#include "GridScrollView.h"
#include "ResourcePathManager.h"

using namespace extension;

GridScrollView::GridScrollView(CCSize viewPortSize,int gridNum,int gridNumInX,int gridNumInY,float gridPadding /* = 10.0f */,CCPoint scrollViewPortOffset /* = CCPointZero */,float pageInval /* = 40.0f */)
:MyScrollView(viewPortSize)
{
	this->itemCreator = NULL;
	this->winSize = CCDirector::sharedDirector()->getWinSize();
	this->scrollViewPortOffset = scrollViewPortOffset;
	this->gridNum = gridNum;
	this->gridNumInX = gridNumInX;
	this->gridNumInY = gridNumInY;
	this->mScrollViewPortSize = viewPortSize;

	this->cal_gridNumInOnePage = gridNumInX * gridNumInY;
	this->cal_PageNum = gridNum/cal_gridNumInOnePage;if (gridNum % cal_gridNumInOnePage != 0)cal_PageNum++;
	this->gridItemArray = CCArray::createWithCapacity(gridNum);
	this->gridItemArray->retain();
	this->pageInval = pageInval;
	this->cal_currentShowedPageIndex = 0;
	this->pageScrollTime = 0.4f;
	this->container = createContainner();
	this->cal_containerSize = CCSizeMake((mScrollViewPortSize.width+pageInval)*cal_PageNum-pageInval,mScrollViewPortSize.height);//viewPortSize
	this->container->setTextureRect(CCRectMake(0,0,cal_containerSize.width,cal_containerSize.height));
	this->initWithViewSize(mScrollViewPortSize,container);

	this->setPosition(ccp(winSize.width/2-mScrollViewPortSize.width/2,winSize.height/2-mScrollViewPortSize.height/2) );
	this->container->setPosition( ccp(winSize.width/2-cal_containerSize.width/2,winSize.height/2-cal_containerSize.height/2) );
	
	this->setDirection(kCCScrollViewDirectionHorizontal);
	//this->setDirection(CCScrollViewDirectionHorizontal);

	this->setPadding(gridPadding,false);
}
GridScrollView::~GridScrollView()
{
	CC_SAFE_RELEASE(gridItemArray);
}

void GridScrollView::setClickableCreater(ClickableItemCreater* creater)
{
	this->itemCreator = creater;
}

void GridScrollView::prepareToDisplay()
{
	initGridItem(gridItemArray,gridNumInX,gridNumInY);
}

CCSprite* GridScrollView::createContainner()
{
	// 一个纯透明的背景，作为真正的容器。
	//return CCSprite::create(ResourcePathManager::getManager()->getPath("select_bg.png").c_str());
	CCSprite* s = CCSprite::create();
	s->setOpacity(0);
	return s;
}

void GridScrollView::initGridItem(CCArray* itemToStore,int gridNumInX,int gridNumInY)
{
	// 
	if (itemCreator == NULL)
	{
		CCAssert(false,"需要先调用setClickableCreater");
		return;
	}

	{//清空
		container->removeAllChildrenWithCleanup(true);//先清空容器的东西;
		gridItemArray->removeAllObjects();
	}

	int itemNum = itemToStore->capacity();
	int pageNum = itemNum/(gridNumInX*gridNumInY);
	if (itemNum % (gridNumInX*gridNumInY) != 0)
	{
		pageNum++;
	}
	//每一列的间隔，就是X间隔
	float invalInX = (mScrollViewPortSize.width - paddingLeft - paddingRight)/gridNumInX;
	//每一行的间隔，就是Y间隔
	float invalInY = (mScrollViewPortSize.height - paddingTop - paddingButtom)/gridNumInY;

	float startPosX = scrollViewPortOffset.x + paddingLeft;
	float startPosY = mScrollViewPortSize.height  -scrollViewPortOffset.y - paddingTop;
	
	float posX = startPosX;
	float posY = startPosY;

	int addedNum = 1;
	for (int i=0;i<pageNum;i++)
	{
		//一页;
		for(int j=0;j<gridNumInY;j++)
		{
			posY -= (invalInY)/2;
			//一行;
			for (int m = 0;m<gridNumInX;m++)
			{
				if (addedNum > itemNum)
					break;
				ClickableItem* item = NULL;
				if (itemCreator != NULL)
				{
					item = itemCreator->createGridItem(i,m,j);
				}
				itemToStore->addObject(item);
				posX += (invalInX)/2;
				if (item!=NULL)
					item->setPosition(ccp(posX,posY));//把Item放在网格的中心
				posX += (invalInX)/2;
				addedNum++;
				if (item!=NULL)
				{
					addChild(item);
					item->setAnchorPoint(ccp(0.5f,0.5f));
				}
				//CCLog("addChild---->posX=%f,posY=%f",item->getPositionX(),item->getPositionY());
			}
			posX = startPosX;
			posY -= (invalInY)/2;

			if (addedNum > itemNum)
				break;
		}
		posY = startPosY;
		startPosX += (mScrollViewPortSize.width+pageInval);
		posX = startPosX;

		if (addedNum > itemNum)
			break;
	}
	setContentOffset(CCPointMake(0,0));
}

//ClickableItem* GridScrollView::createGridItem(int pageIndex,int xIndex,int yIndex)
//{
//	ClickableItem* item =(ClickableItem*) new ScrollViewTempItem();
//	item->autorelease();
//	item->initWithFile("blocks.png");
//	return item;
//}

void GridScrollView::setPadding(float padding,bool refactorGridSize /* = true */)
{
	setPaddingLeft(padding,false);
	setPaddingRight(padding,false);
	setPaddingTop(padding,false);
	setPaddingButtom(padding,false);
	if (refactorGridSize)
	{
		reBuildSelf();
	}
	//pageSizeX = mScrollViewPortSize.width - paddingLeft - paddingRight;
}

void GridScrollView::setPaddingLeft(float padding,bool refactorGridSize /* = true */)
{
	paddingLeft = padding;
	if (refactorGridSize)
	{
		reBuildSelf();
	}
}
void GridScrollView::setPaddingRight(float padding,bool refactorGridSize /* = true */)
{
	paddingRight= padding;
	if (refactorGridSize)
	{
		reBuildSelf();
	}
}
void GridScrollView::setPaddingTop(float padding,bool refactorGridSize /* = true */)
{
	paddingTop = padding;
	if (refactorGridSize)
	{
		reBuildSelf();
	}
}
void GridScrollView::setPaddingButtom(float padding,bool refactorGridSize /* = true */)
{
	paddingButtom= padding;
	if (refactorGridSize)
	{
		reBuildSelf();
	}
}

void GridScrollView::setPageScrollDur(float dur)
{
	pageScrollTime = dur;
}

float GridScrollView::getPageOffsetX(int pageIndex)
{
	return MyScrollView::getPageOffsetX(pageIndex,mScrollViewPortSize.width,pageInval);
}

/**
 *	滑动到了某个页面，就会调用，是一个回调接口
 *	有到这个页面的意图，也会发送的，然后就开始动画移动过去
 *	或者直接到那里去
 */
void GridScrollView::pageSelected(int pageIndex)
{
	if (itemCreator != NULL)
	{
		//itemCreator->pageSelectedEnd(pageIndex);
	}
}
/**
 *	滑动完毕了
 */
void GridScrollView::pageSelectedEnd(int pageIndex)
{
	if (itemCreator != NULL)
	{
		itemCreator->pageSelectedEnd(pageIndex);
	}
}

void GridScrollView::intentToScrollToLeftPage(){
	cal_currentShowedPageIndex--;
	if (cal_currentShowedPageIndex<0)
		cal_currentShowedPageIndex = 0;

	srollToPageBySpeed(cal_currentShowedPageIndex,mScrollViewPortSize.width,pageInval,1024.0f/pageScrollTime);
	//srollToPage(cal_currentShowedPageIndex,mScrollViewPortSize.width,pageInval,pageScrollTime);
}

void GridScrollView::intentToScrollToCurrentPage()
{
	srollToPageBySpeed(cal_currentShowedPageIndex,mScrollViewPortSize.width,pageInval,1024.0f/pageScrollTime);
	//srollToPage(cal_currentShowedPageIndex,mScrollViewPortSize.width,pageInval,pageScrollTime);
}

void GridScrollView::intentToScrollToRightPage(){
	cal_currentShowedPageIndex++;
	if (cal_currentShowedPageIndex >= cal_PageNum)
		cal_currentShowedPageIndex = cal_PageNum-1;

	srollToPageBySpeed(cal_currentShowedPageIndex,mScrollViewPortSize.width,pageInval,1024.0f/pageScrollTime);
	//srollToPage(cal_currentShowedPageIndex,mScrollViewPortSize.width,pageInval,pageScrollTime);
}
//
void GridScrollView::scrollToPageSimple(int pageIndex)
{
	scrollToPageSimple(pageIndex,pageScrollTime);
}
void GridScrollView::scrollToPageSimple(int pageIndex,float time)
{
	if (pageIndex < 0 || pageIndex > (cal_PageNum-1) )
	{
		CCLog("\nWaring----->GridScrollView::scrollToPageSimple---->pageIndex ( = %d) must bigger than 0,smaller than %d !",pageIndex,cal_PageNum-1);
		return;
	}
	if (time > 0)
	{
		srollToPageBySpeed(pageIndex,mScrollViewPortSize.width,pageInval,1024.0f/time);
	}else
	{
		srollToPageBySpeed(pageIndex,mScrollViewPortSize.width,pageInval,0);
	}
	//srollToPage(pageIndex,mScrollViewPortSize.width,pageInval,time);
	cal_currentShowedPageIndex = pageIndex;
}

int GridScrollView::getPageIndex(int gridIndex)
{
	return gridIndex/cal_gridNumInOnePage;
}
int GridScrollView::getPageIndex(CCSprite* item)
{
	for (int i=0;i<gridNum;i++)
	{
		CCSprite* temp = (CCSprite*)gridItemArray->objectAtIndex(i);
		if (temp == item)
		{
			return getPageIndex(i);
		}
	}

	return -1;
}
void GridScrollView::setGridItemOpacity(CCSprite* item,GLubyte opacity)
{
	CCArray* children = item->getChildren();
	int num = children->count();
	for (int i=0;i<num;i++)
	{
		((CCSprite*)children->objectAtIndex(i))->setOpacity(opacity);
	}
	item->setOpacity(opacity);
}
CCArray* GridScrollView::getPageItems(int pageIndex)
{
	if (pageIndex < 0 || pageIndex > (cal_PageNum-1) )
	{
		CCLog("GridScrollView::setPageItemOpacity--->pageIndex must in [0,%d],but this index is %d \n",cal_PageNum-1,pageIndex);
		return NULL;
	}
	int start = pageIndex*cal_gridNumInOnePage;
	int end = start + cal_gridNumInOnePage;

	CCArray* ret_array = CCArray::create();
	for (int i=start;i<end;i++)
	{
		if (i > gridItemArray->capacity()-1)
			break;
		CCObject * obj = gridItemArray->objectAtIndex(i);
		ret_array->addObject(obj);
	}
	return ret_array;
}
void GridScrollView::setPageItemOpacity(int pageIndex,GLubyte opacity)
{
	if (pageIndex < 0 || pageIndex > (cal_PageNum-1) )
	{
		CCLog("GridScrollView::setPageItemOpacity--->pageIndex must in [0,%d],but this index is %d \n",cal_PageNum-1,pageIndex);
		return;
	}
	int start = pageIndex*cal_gridNumInOnePage;
	int end = start + cal_gridNumInOnePage;

	for (int i=start;i<end;i++)
	{
		if (i > gridItemArray->capacity() -1 )
			break;

		CCObject * obj = gridItemArray->objectAtIndex(i);
		if (obj!=NULL)
		{
			((CCSprite*)obj)->setOpacity(opacity);
			//setGridItemOpacity( (CCSprite*)obj,opacity);
		}
	}
}

void GridScrollView::onTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent, cocos2d::CCPoint glPoint)
{
    for (int i=0;i<gridNum;i++)
	{
		CCPoint scrollPoint = convertToScrollViewRelativePoint(glPoint);
		ClickableItem* temp = (ClickableItem*)gridItemArray->objectAtIndex(i);
		if (clicked(temp,scrollPoint))
		{
			temp->touchMe();
			break;
		}else
        {
            temp->leaveMe();
        }
	}
}
void GridScrollView::onTouchMove(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent, cocos2d::CCPoint glPoint)
{
    for (int i=0;i<gridNum;i++)
	{
		CCPoint scrollPoint = convertToScrollViewRelativePoint(glPoint);
		ClickableItem* temp = (ClickableItem*)gridItemArray->objectAtIndex(i);
		if (!clicked(temp,scrollPoint))
		{
			temp->leaveMe();
		}
	}
}

void GridScrollView::onTouchEnd(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint)
{
    for (int i=0;i<gridNum;i++)
	{
        ClickableItem* temp = (ClickableItem*)gridItemArray->objectAtIndex(i);
		temp->leaveMe();
	}
}
void GridScrollView::onTouchCancle(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint)
{
    for (int i=0;i<gridNum;i++)
	{
        ClickableItem* temp = (ClickableItem*)gridItemArray->objectAtIndex(i);
		temp->leaveMe();
	}
}

void GridScrollView::onTouch(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint)
{
	pTouch = pTouch;pEvent = pEvent;
	//m_tBeginPos
	for (int i=0;i<gridNum;i++)
	{
		CCPoint scrollPoint = convertToScrollViewRelativePoint(glClickPoint);
		ClickableItem* temp = (ClickableItem*)gridItemArray->objectAtIndex(i);
		if (clicked(temp,scrollPoint))
		{
			itemClicked(temp);
			break;
		}
	}
}

void GridScrollView::itemClicked(ClickableItem* clickedObj)
{
	clickedObj->clickedMe();
}

void GridScrollView::setGridNumInX(int gridNumInX)
{
	this->gridNumInX = gridNumInX;
	reBuildSelf();
}
void GridScrollView::setGridNumInY(int gridNumInY)
{
	this->gridNumInY = gridNumInY;
	reBuildSelf();
}

void GridScrollView::reBuildSelf()
{
	{//释放
		CC_SAFE_RELEASE(gridItemArray);
		gridItemArray = CCArray::createWithCapacity(gridNum);
	}
	initGridItem(gridItemArray,gridNumInX,gridNumInY);
}