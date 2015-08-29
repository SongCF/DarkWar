#ifndef __NetFight__NetBaseTouchLayer__
#define __NetFight__NetBaseTouchLayer__
#include <iostream>
#include "cocos2d.h"
#include <string.h>
#include "cocos-ext.h"
USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;
class NetBaseTouchLayer:public CCLayer
{
public:
	CREATE_FUNC(NetBaseTouchLayer);
	NetBaseTouchLayer();
	~NetBaseTouchLayer();
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
private:
};
#endif