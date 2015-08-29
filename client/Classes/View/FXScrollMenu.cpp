#include "FXScrollMenu.h"

//(点击校验范围)
#define ViewTouchMove_Delta 16    

FXScrollMenu* FXScrollMenu::create(cocos2d::CCPoint viewLeftDownPos_worldCoordinate, cocos2d::CCSize viewAreaSize)
{
	FXScrollMenu *menu = new FXScrollMenu;
	if (menu && menu->init(viewLeftDownPos_worldCoordinate, viewAreaSize))
	{
		menu->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(menu);
		menu = NULL;
	}

	return menu;
}

bool FXScrollMenu::init(cocos2d::CCPoint viewLeftDownPos_worldCoordinate, cocos2d::CCSize viewAreaSize)
{
	if ( ! CCMenu::init())
		return false;

	mpTouch = NULL;  //重新派发时  使用
	mViewLeftDownPos = viewLeftDownPos_worldCoordinate;
	mViewSize = viewAreaSize;
	mViewRect.setRect(mViewLeftDownPos.x, mViewLeftDownPos.y, mViewSize.width, mViewSize.height);

	return true;
}

// void FXScrollMenu::registerWithTouchDispatcher()
// {
// 	CCDirector* pDirector = CCDirector::sharedDirector();
// 	pDirector->getTouchDispatcher()->addTargetedDelegate(this, this->getTouchPriority(), true);
// }


bool FXScrollMenu::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
	//是否是重新派发的
	if (touch == mpTouch)
	{
		mpTouch = NULL;
		return false;
	}

	//	CCLog("FXScrollMenu : %s ", __FUNCTION__);
	CC_UNUSED_PARAM(event);
	//	if (m_eState != kCCMenuStateWaiting || ! m_bVisible || !m_bEnabled)   //by fx
	if (m_eState != kCCMenuStateWaiting || ! m_bVisible || ! isEnabled())
	{
		return false;
	}

	for (CCNode *c = this->m_pParent; c != NULL; c = c->getParent())
	{
		if (c->isVisible() == false)
		{
			return false;
		}
	}

	mTouchStartPos = touch->getLocation();        // by fx add
	if (mViewRect.containsPoint(mTouchStartPos))     // by fx add
	{
		mTouchMoved = false;                           // by fx add

		m_pSelectedItem = this->itemForTouch(touch);
		if (m_pSelectedItem)
		{
			m_eState = kCCMenuStateTrackingTouch;
			m_pSelectedItem->selected();
			return true;
		}
	}

	return false;
}

void FXScrollMenu::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
	//
	if (mTouchMoved) return;
	//

	CC_UNUSED_PARAM(event);
//	CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchMoved] -- invalid state");
	CCMenuItem *currentItem = this->itemForTouch(touch);

	//add
	//移动了、那么该按钮不再响应点击消息了
	CCPoint movePos = touch->getLocation();
	if (fabs(movePos.x - mTouchStartPos.x) > ViewTouchMove_Delta ||
		fabs(movePos.y - mTouchStartPos.y) > ViewTouchMove_Delta)
	{
		if (m_pSelectedItem)
		{
			m_pSelectedItem->unselected();
		}
		mTouchMoved = true;


		CCTargetedTouchHandler* pHandler = dynamic_cast<CCTargetedTouchHandler*>(
			CCDirector::sharedDirector()->getTouchDispatcher()->findHandler(this));
		if (pHandler)
		{
			//把自己的touch移除、避免后面响应touchMove
			CCSet* mySet = pHandler->getClaimedTouches();
			mySet->removeObject(touch);
			m_eState = kCCMenuStateWaiting;


			//然后重新派发出一个 触摸消息给低优先级的（该touch已被吞噬）
			mpTouch = touch;
			CCSet* _set = CCSet::create();
			_set->addObject(mpTouch);
			CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(_set, NULL);
		}


		return;
	}
	//


	if (currentItem != m_pSelectedItem) 
	{
		if (m_pSelectedItem)
		{
			m_pSelectedItem->unselected();
		}
		m_pSelectedItem = currentItem;
		if (m_pSelectedItem)
		{
			m_pSelectedItem->selected();
		}
	}
}

void FXScrollMenu::ccTouchEnded(CCTouch *touch, CCEvent* event)
{
	CC_UNUSED_PARAM(touch);
	CC_UNUSED_PARAM(event);

	//多点触摸的时候依然会传到这里，所以去掉这个assert
//	CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchEnded] -- invalid state");
	//	if (m_pSelectedItem)
	if (m_pSelectedItem && ! mTouchMoved)
	{
		m_pSelectedItem->unselected();
		m_pSelectedItem->activate();
	}
	m_eState = kCCMenuStateWaiting;
}

void FXScrollMenu::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{
	CC_UNUSED_PARAM(touch);
	CC_UNUSED_PARAM(event);
//	CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchCancelled] -- invalid state");
	//	if (m_pSelectedItem)
	if (m_pSelectedItem && ! mTouchMoved)
	{
		m_pSelectedItem->unselected();
	}
	m_eState = kCCMenuStateWaiting;
}