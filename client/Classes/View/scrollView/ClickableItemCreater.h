#ifndef __ClickableItemCreater_H__
#define __ClickableItemCreater_H__
#include "ClickableItem.h"
//ClickableItem的创建接口
class ClickableItemCreater
{
public:
	// 创建一个Item，根据传入的页面号，x(从左到右的号)，y(从上到下的号)
	virtual ClickableItem* createGridItem(int pageIndex,int xIndex,int yIndex) = 0;
	// 页面选择了
	virtual void pageSelectedEnd(int pageIndex) = 0;
};

#endif