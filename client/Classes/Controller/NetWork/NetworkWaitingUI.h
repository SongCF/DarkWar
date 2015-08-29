#pragma once

#include "BaseLayer.h"

// 网络链接，等待UI，会屏蔽所有的点击，和操作。
class NetworkWaitingUI : public BaseLayer
{
protected:
	virtual void onEnter();
	virtual void onExit();

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

public:
	static NetworkWaitingUI* create();

	NetworkWaitingUI(void);
	~NetworkWaitingUI(void);
};

