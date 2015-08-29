#pragma once

#include "cocos2d.h"

using namespace cocos2d;

class FXScrollMenu : public cocos2d::CCMenu
{
public:
	//scrollVeiw/tabelView左下角世界坐标，view大小，menu超过view边界的部分就不可点击
	//menu不会吞噬触摸消息，滑动时不响应消息
	static FXScrollMenu* create(cocos2d::CCPoint viewLeftDownPos_worldCoordinate, cocos2d::CCSize viewAreaSize);
	bool init(cocos2d::CCPoint viewLeftDownPos_worldCoordinate, cocos2d::CCSize viewAreaSize);


//	virtual void registerWithTouchDispatcher();
	 /**
    @brief For phone event handle functions
    */
    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent* event);
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event); 


protected:
	CCSize mViewSize;
	CCPoint mViewLeftDownPos;
	CCRect mViewRect;

	CCPoint mTouchStartPos;
	bool mTouchMoved;

	CCTouch *mpTouch; //重新派发的touch事件
};