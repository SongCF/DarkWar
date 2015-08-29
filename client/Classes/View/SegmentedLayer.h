#pragma once

#include "BaseLayer.h"


/**
 * 分段的layer，把一个layer分成了3段
 * 注意和MainLayerBase的区别，MainLayerBase是把一个界面分成了3个layer
 * 头部和尾部都是固话好的 分别置顶和置底。而且开发期间是直接给了3段的素材的。
 *
 * 这里的这个layer和MainLayerBase基本一样，但是没有自动排版，做规定等。
 * 只是简单的把东西分为3类：头，中，尾。我们可以分别控制头中尾的个性偏移
 *
 */
class SegmentedLayer : public BaseLayer
{
private:
	// 头部的基准
	CCNode* headBase;
	// 脚部的基准
	CCNode* footBase;
	// 内容区的基准
	CCNode* bodyBase;
public:
	SegmentedLayer(void);
	// 设置头部的偏移
	void setHeadOffset(CCPoint offset){headBase->setPosition(offset);};
	// 设置尾部的偏移
	void setFootOffset(CCPoint offset){footBase->setPosition(offset);};
	// 设置中部的偏移
	void setBodyOffset(CCPoint offset){bodyBase->setPosition(offset);};

	// 添加到头部区域
	void addChildToHead(CCNode* node,int z_order = 0,int tag = -1){headBase->addChild(node,z_order,tag);};
	// 添加到尾部区域
	void addChildToFoot(CCNode* node,int z_order = 0,int tag = -1){footBase->addChild(node,z_order,tag);};
	// 添加到中部区域
	void addChildToBody(CCNode* node,int z_order = 0,int tag = -1){bodyBase->addChild(node,z_order,tag);};

	// 设置头部偏移到最上面：上移 [***-winSize.height]
	void setHeadTopMost();
	// 设置尾部偏移到最下面：setPostion(0,0);
	void setFootButtomMost();
	// 设置中部偏移到最中间：上移 [(***-winSize.height)/2]
	void setBodyCenterMost();

	// 从分段中得到Item
	CCNode* getChildByTagFromFoot(int tag){return footBase->getChildByTag(tag);};
	CCNode* getChildByTagFromBody(int tag){return bodyBase->getChildByTag(tag);};
	CCNode* getChildByTagFromHead(int tag){return headBase->getChildByTag(tag);};

	~SegmentedLayer(void);
};

