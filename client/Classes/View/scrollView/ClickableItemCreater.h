#ifndef __ClickableItemCreater_H__
#define __ClickableItemCreater_H__
#include "ClickableItem.h"
//ClickableItem�Ĵ����ӿ�
class ClickableItemCreater
{
public:
	// ����һ��Item�����ݴ����ҳ��ţ�x(�����ҵĺ�)��y(���ϵ��µĺ�)
	virtual ClickableItem* createGridItem(int pageIndex,int xIndex,int yIndex) = 0;
	// ҳ��ѡ����
	virtual void pageSelectedEnd(int pageIndex) = 0;
};

#endif