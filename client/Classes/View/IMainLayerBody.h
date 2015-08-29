#pragma once

#include "cocos2d.h"
#include "MainLayerBase.h"


using namespace cocos2d;

// MainLayerFooter的每一个Body的接口
// 每一个具体的Body都自己居中写就是了，foot会自动移动他们到合适的位置
class IMainLayerBody
{

protected:
	MainLayerBase* mFooterLayer;
	CC_SYNTHESIZE(PopUpPos, mPopPos, PopPos);
public:
	IMainLayerBody()
	{
		mFooterLayer = NULL;
	}
	virtual void initFoot(MainLayerBase* footerLayer)
	{
		mFooterLayer = footerLayer;
	}
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return CCSizeMake(0,0);};
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return CCPointZero;};
};


class TestLayer : public IMainLayerBody,public BaseLayer
{
public:
	TestLayer()
	{

	}
	~TestLayer()
	{

	}
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return CCSizeMake(0,0);};
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos(){return CCPoint(mWinSize.width/2,mWinSize.height/2);};
private:

};
