#include "MyScrollView.h"
#include <cmath>

float minMoveDistance = 768.0f/2.0f;//移动多少个像素才可以翻页;
float maxMoveForClick = 10.0f;//移动多少像素之内属于点击;
float minEffectiveDisForClick = 10.0f;
float minVForPage = 3.0f;//翻页的最小速度;
bool clickEfective = true;//是否认为点击有效：因为点击之后可能移动，超过minEffectiveDisForClick，但是呢，在抬起之前，有滑动回来，那么就认为点击无效;

CCPoint lastMovePoint = CCPointMake(0,0);

MyScrollView::MyScrollView(CCSize viewPortSize)
{
	this->scrollViewPortSize = viewPortSize;
	this->winSize = CCDirector::sharedDirector()->getWinSize();
	this->zeroY = (winSize.height-scrollViewPortSize.height)/2;
	this->zeroX = (winSize.width-scrollViewPortSize.width)/2;
	this->canScroll = true;
	this->mSelectedIndex = 0;

	minMoveDistance = MIN(viewPortSize.width,winSize.width)/2.0f;
}

MyScrollView::~MyScrollView()
{
	//CC_SAFE_DELETE(manager);//不能删是全局的！
}

void MyScrollView::setEnableScroll(bool enbale)
{
	canScroll = enbale;
}
void MyScrollView::addRecord(CCPoint record)
{
	const int RECORD_NUM = 2;
	movePointRecord.insert(movePointRecord.end(),record);
	if (movePointRecord.size() > RECORD_NUM)
		movePointRecord.erase(movePointRecord.begin());
}

float MyScrollView::getInstantaneousVelocity()
{
	const int RECORD_NUM = 2;
	//数据不为RECORD_NUM个，就直接放弃
	if (movePointRecord.size() != RECORD_NUM)
		return 0;
	return movePointRecord[1].x - movePointRecord[0].x;
}

bool MyScrollView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	m_pTouch = pTouch;
	m_pEvent = pEvent;
	moving = false;
	clickEfective = true;//恢复有效;
	m_tBeginPos = pTouch->getLocation();// ->locationInView();	
	//m_tBeginPos = CCDirector::sharedDirector()->convertToGL( m_tBeginPos );
	lastMovePoint = m_tBeginPos;
	bool ret = CCScrollView::ccTouchBegan(pTouch, pEvent);
	if (!ret)
	{
		onTouchFalse(pTouch,pEvent,m_tBeginPos);
	}
    onTouchBegan(pTouch, pEvent, m_tBeginPos);
	return ret;
}

void MyScrollView::onTouchFalse(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint)
{

}

void MyScrollView::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	pEvent = pEvent;pTouch = pTouch;
	moving = true;
	
	lastMovePoint = pTouch->getLocation();	
	addRecord(lastMovePoint);

	//如果途中有大距离移动，那么最终是不可以实现点击的;
	if ( abs(lastMovePoint.x - m_tBeginPos.x) >= minEffectiveDisForClick ||
		 abs(lastMovePoint.y - m_tBeginPos.y) >= minEffectiveDisForClick )
	{
		clickEfective = false;
	}
	onTouchMove(pTouch,pEvent,lastMovePoint);
	if (!canScroll)
	{
		return ;
	}
	CCScrollView::ccTouchMoved(pTouch, pEvent);


	/*CCPoint moveDistance = ccpSub(lastMovePoint, m_tBeginPos);
	if (getContentOffset().x < -(m_pContainer->getContentSize().width - 1024) )
	{
	moveDistance.x /= 2.5;
	}
	if (getContentOffset().x > 0)
	{
	moveDistance.x /= 2.5;
	}
	m_pContainer->setPosition(ccpAdd(m_pContainer->getPosition(), moveDistance));*/
}

void MyScrollView::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCScrollView::ccTouchEnded(pTouch, pEvent);
	// 不让他自己滑动
	unscheduleAllSelectors();

	CCPoint endPoint = pTouch->getLocation();	
	float v = getInstantaneousVelocity();

#ifdef DEBUG_ENABLE_MYSCORLLVIEW_LOG
	CCLog("InstantaneousVelocity = %f",v);
#endif
	

	onTouchEnd(pTouch, pEvent, endPoint);

	//属于点击??;
	if ( abs(endPoint.x - m_tBeginPos.x) < maxMoveForClick && clickEfective)
	{
		intentToScrollToCurrentPage();//恢复到原页;
		onTouch(m_pTouch,m_pEvent,endPoint);
		return;
	}

	clickEfective = true;//恢复有效;

	float offset = endPoint.x - m_tBeginPos.x;	//记录点击和抬起手指的偏移量
	if ( offset > minMoveDistance || v > minVForPage)
	{//向左滑动;

		if (v < 0 || offset <= 0)
		{
			intentToScrollToCurrentPage();
		}
		else
		{
			intentToScrollToLeftPage();
		}
	}
	else if (offset < -minMoveDistance || v < -minVForPage)
	{//向右滑动;

		if ( v > 0 || offset >= 0 )
		{
			intentToScrollToCurrentPage();
		}else
		{
			intentToScrollToRightPage();
		}
	}
	else if ( abs(offset) < minMoveDistance && abs(v) < minVForPage)
	{//回滑
		intentToScrollToCurrentPage();//恢复到原页;
	}
}

void MyScrollView::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
	CCScrollView::ccTouchCancelled(pTouch, pEvent);
	intentToScrollToCurrentPage();//恢复到原页;
	clickEfective = true;//恢复有效;
    
    CCPoint endPoint = pTouch->getLocation();
    onTouchCancle(pTouch, pEvent, endPoint);
}

void MyScrollView::onTouch(CCTouch *pTouch, CCEvent *pEvent,CCPoint glClickPoint)
{
	pTouch = pTouch;
	pEvent = pEvent;
	glClickPoint = glClickPoint;
}
void MyScrollView::onTouchMove(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint)
{
	pTouch = pTouch;
	pEvent = pEvent;
    glPoint = glPoint;
}
void MyScrollView::onTouchBegan(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint)
{
	pTouch = pTouch;
	pEvent = pEvent;
    glPoint = glPoint;
}
void MyScrollView::onTouchEnd(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint)
{
    pTouch = pTouch;
	pEvent = pEvent;
    glPoint = glPoint;
}
void MyScrollView::onTouchCancle(CCTouch *pTouch, CCEvent *pEvent,CCPoint glPoint)
{
    pTouch = pTouch;
	pEvent = pEvent;
    glPoint = glPoint;
}

float MyScrollView::getPageOffsetX(int pageIndex,float pageSizeX,float pageInval_X)
{
	float offsetX = -pageIndex*(pageSizeX+pageInval_X);//刚移动到整个页面处于视图左边;
	offsetX += ((getViewSize().width-pageSizeX)/2);//移回去到中间;
	CCSize containnerSize = getContainer()->getContentSize();
	float minSroll = -(containnerSize.width-getViewSize().width);
	float maxSroll = 0.0f;

	if (offsetX > maxSroll)
		offsetX = maxSroll;
	if (offsetX < minSroll)
		offsetX = minSroll;

	return offsetX;
}
bool MyScrollView::pageScrollIntercept(int pageIndex)
{
	return false;
}
void MyScrollView::srollToPage(int pageIndex,float pageSizeX,float pageInval_X,float duration)
{
	if (pageScrollIntercept(pageIndex))
	{
		CCLog("MyScrollView::srollToPage index = %d,is tntercepted.",pageIndex);
		return;
	}

	mSelectedIndex = pageIndex;

	float offsetY = 0.0f;
	float offsetX = getPageOffsetX(pageIndex,pageSizeX,pageInval_X);
	if (duration<=0)
	{
		setContentOffset(ccp(offsetX,offsetY));
	}else
	{
		//// 直接用框架的API
		//setContentOffsetInDuration(ccp(offsetX,offsetY),duration);

		// 用动画的形式
		CCAction* action = CCSequence::create(
			CCMoveTo::create(duration,ccp(offsetX,offsetY)),
			CCCallFuncN::create(this,SEL_CallFuncN(&MyScrollView::scrollActionEnd)),
			NULL
			);
		getContainer()->stopAllActions();
		getContainer()->runAction(action);
	}
	pageSelected(mSelectedIndex);
}
void MyScrollView::scrollActionEnd(CCNode* container)
{
	pageSelectedEnd(mSelectedIndex);
}
void MyScrollView::srollToPageBySpeed(int pageIndex,float pageSizeX,float pageInval_X,float speed)
{
	float oldOffsetX = getContentOffset().x;
	float newOffsetX = getPageOffsetX(pageIndex,pageSizeX,pageInval_X);
	float distanceX = abs(newOffsetX-oldOffsetX);
	float duration = 0;
	if (speed <= 0)
	{
		duration = -1;
	}else
	{
		duration = distanceX/speed;
	}
	srollToPage(pageIndex,pageSizeX,pageInval_X,duration);
}

//
//此方法还有bug，不要用，因为child->getPosition()的值是固定的，但是一旦移动了
//ScrollView，那么还是用child->getPosition()的值取实现移动就不对了.
//
void MyScrollView::scrollToChild(cocos2d::CCNode *child)
{
	CCSize containnerSize = getContainer()->getContentSize();
	CCSize childSize = child->getContentSize();
	CCPoint childPos = child->getPosition();

	CCPoint childCenterPos = CCPointMake(childPos.x+childSize.width/2,childPos.y+childSize.height/2);
	CCPoint centerPoint = CCPointMake(CCDirector::sharedDirector()->getWinSize().width/2,CCDirector::sharedDirector()->getWinSize().height/2);
	
	CCPoint destPos = CCPointMake(centerPoint.x-childCenterPos.x,0);
	
	float minSroll = -(containnerSize.width-getViewSize().width);
	float maxSroll = 0.0f;

	if (destPos.x > maxSroll)
	{
		destPos.x = maxSroll;
	}
	if (destPos.y < minSroll)
	{
		destPos.y = minSroll;
	}
#ifdef DEBUG_ENABLE_MYSCORLLVIEW_LOG
	CCLog("%s%f%s%f","Scroll to--> x=",destPos.x," y=",destPos.y);
#endif
	setContentOffsetInDuration(destPos,0.1f);
}

CCPoint MyScrollView::convertToScrollViewRelativePoint(cocos2d::CCPoint &glPoint)
{
	float gl_x = glPoint.x;
	float gl_y = glPoint.y;

	float currentScrollViewOffsetX = getContentOffset().x;
	float currentScrollViewOffsetY = getContentOffset().y;

	float x_distance = zeroX;
	float y_distance = zeroY;


	float scroll_x = gl_x - x_distance - currentScrollViewOffsetX;
	float scroll_y = gl_y - y_distance - currentScrollViewOffsetY;


	return CCPointMake(scroll_x,scroll_y);

}

//是否点中了，是就返回true;
bool MyScrollView::clicked(CCSprite* sprite,CCPoint clickPos)
{
	if (sprite->boundingBox().containsPoint(clickPos))
	{
		return true;
	}
	return false;
}