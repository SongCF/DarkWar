#pragma once

#include <string>
#include "cocos2d.h"
#include "BaseSprite.h"

using namespace  std;
using namespace cocos2d;

// 就是安卓的Toast的模拟
class LFToast : public BaseSprite
{
private:
	LFToast(string msg,float during);
	~LFToast(void);

	void removeToast(CCNode* node);
	virtual void removeNode(CCNode* node);
public:
	// 创建，并显示一个Toast
	static void makeToast(string msg,float during = 1.0f);
	// 在指定的位置显示一个
	static void makeToast(string msg,CCPoint offset,float during = 1.0f);
};

