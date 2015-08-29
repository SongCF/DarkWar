#include "ListMenu.h"
ListMenu::ListMenu()
	:CCMenu()
	,m_bIsMove(false)
	,m_pStartPoint(CCPointZero)
{

}
ListMenu::~ListMenu()
{

}
bool ListMenu::init()
{
	if (!CCMenu::init())
	{
		return false;
	}
	setTouchEnabled(true);
	setTouchPriority(kCCMenuHandlerPriority);
	setTouchMode(kCCTouchesOneByOne);
	return true;
}
void ListMenu::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -129,false);
}
bool ListMenu::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){  
	CCPoint	pos = this->convertTouchToNodeSpace(pTouch);
	if(m_rVisableRect.size.width&&m_rVisableRect.size.height){
		if(!m_rVisableRect.containsPoint(pos))
			return false;
	}
	m_bIsMove = false;
	m_pStartPoint = pTouch->getLocation(); 
	return CCMenu::ccTouchBegan(pTouch,pEvent);
}

void ListMenu::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
	CCPoint pos = pTouch->getLocation(); 
	if (fabs(pos.x - m_pStartPoint.x) > 10.0f || fabs(pos.y - m_pStartPoint.y) > 10.0f)
	{
		m_bIsMove = true;
	}
	CCMenu::ccTouchMoved(pTouch,pEvent);
}

void ListMenu::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){
	if (m_bIsMove)
	{
		m_eState = kCCMenuStateWaiting;
		if (m_pSelectedItem)
		{
			m_pSelectedItem->unselected();
		}
	}
	else
	{
		CCMenu::ccTouchEnded(pTouch,pEvent);
	}	
}
void ListMenu::ccTouchCancelled(CCTouch *pTouch, CCEvent* pEvent)
{
	if (m_bIsMove)
	{
		m_eState = kCCMenuStateWaiting;
		if (m_pSelectedItem)
		{
			m_pSelectedItem->unselected();
		}
	}
	else
	{
		CCMenu::ccTouchCancelled(pTouch,pEvent);
	}	
}