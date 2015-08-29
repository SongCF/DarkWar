#ifndef __XWorld__ListMenu__
#define __XWorld__ListMenu__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;
class ListMenu : public CCMenu{
public:
	ListMenu();
	CREATE_FUNC(ListMenu);
	virtual ~ListMenu();
	bool init();
public:
	 virtual void registerWithTouchDispatcher();
	 virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
	 virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
	 virtual void ccTouchCancelled(CCTouch *touch, CCEvent* event);
	 virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
	 inline void setVisableRect(const CCRect &rect)
	 {
		 m_rVisableRect = rect;
	 }
private:
	bool				m_bIsMove;//判断是否移动了
	CCRect				m_rVisableRect;
	CCPoint				m_pStartPoint;
};
#endif