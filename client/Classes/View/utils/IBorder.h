#ifndef __IBorder_H__
#define __IBorder_H__

#include <list>
#include "cocos2d.h"

using namespace cocos2d;

//一个边框的定义接口;
class IBorder
{
public:
	//获得边框,一个物体可能是不规则的，需要多个边框来诠释，所以返回list
	//CCRect的origin是物体的position,size就是物体的contentSize
	virtual std::list<CCRect> getBounds() = 0;
};



#endif