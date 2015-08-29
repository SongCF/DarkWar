#pragma once

#include "BaseLayer.h"

// 头部数据
class MainHeaderLayer : public BaseLayer
{
private:
	BaseSprite* mHeader;

	// 创建默认的一个头
	BaseSprite* createDefaultHead();
public:
	MainHeaderLayer(void);
	void setContent(BaseSprite* header);
	BaseSprite* getHeaderContent(){return mHeader;};
	CCSize getHeaderSize();
	bool isEmpty(){return mHeader == NULL;};
	~MainHeaderLayer(void);
};

