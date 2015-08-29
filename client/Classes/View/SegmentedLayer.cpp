#include "SegmentedLayer.h"

#define DEFAULT_HEIGHT 960		// 理想，默认高度960

#define HEAD_Z 128
#define FOOT_Z 128
#define BODY_Z 100

SegmentedLayer::SegmentedLayer(void)
{
	// 头部，默认不操作
	headBase = CCNode::create();
	addChild(headBase,HEAD_Z);
	//headBase->setPosition(ccp(0,(mVisibleSize.height - DEFAULT_HEIGHT)));
	headBase->setPosition(ccp(0,0));

	// 尾部，默认置底,其实就是没有操作
	footBase = CCNode::create();
	addChild(footBase,FOOT_Z);
	footBase->setPosition(CCPointZero);

	// 内容部分，默认不操作
	bodyBase = CCNode::create();
	addChild(bodyBase,BODY_Z);
	//bodyBase->setPosition(ccp(0,(mVisibleSize.height - DEFAULT_HEIGHT)/2.0f));
	bodyBase->setPosition(ccp(0,0));
}

// 设置头部偏移到最上面：上移 [***-winSize.height]
void SegmentedLayer::setHeadTopMost()
{
	headBase->setPosition(ccp(0,mVisibleSize.height - DEFAULT_HEIGHT));
}
// 设置尾部偏移到最下面：setPostion(0,0);
void SegmentedLayer::setFootButtomMost()
{
	footBase->setPosition(ccp(0,0));
}
// 设置中部偏移到最中间：上移 [(***-winSize.height)/2]
void SegmentedLayer::setBodyCenterMost()
{
	bodyBase->setPosition(ccp(0,(mVisibleSize.height - DEFAULT_HEIGHT)/2));
}

SegmentedLayer::~SegmentedLayer(void)
{
}
