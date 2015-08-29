#include "LightMaskFactory.h"

LightMaskFactory* LightMaskFactory::instance;

LightMaskFactory::LightMaskFactory(void)
{
}

LightMaskFactory* LightMaskFactory::getCurrentFactory()
{
	if (instance == NULL)
	{
		instance = new LightMaskFactory();
	}
	return instance;
}

CCSprite* LightMaskFactory::buildSprite(CCSprite* base,CCSprite* lightNode)
{
	vector<CCSprite*> vec;
	vec.push_back(lightNode);
	return buildSprite(base,vec);
}

// 构建一个Sprite,是以base为底
// lightNodes为亮点盖在base上面，扣掉Base对应的地方的像素
// 外界需要设置好lightNode跟base的相对位置
CCSprite* LightMaskFactory::buildSprite(CCSprite* base,vector<CCSprite*> lightNodes)
{
	int baseWidth = base->getContentSize().width;
	int baseHeight = base->getContentSize().height;

	CCRenderTexture* rt = CCRenderTexture::create(baseWidth, baseHeight);//创建纹理渲染范围
	ccBlendFunc blendFunc;
	blendFunc.src = GL_DST_ALPHA;	//表示使用目标颜色的alpha值来作为因子
	blendFunc.dst = GL_ZERO;		//不使用目标颜色

	rt->begin();
	base->setPosition(ccp(baseWidth/2.0f,baseHeight/2.0f));
	base->visit();	//注意，这里需要先绘制A，遮罩图片
	int nodeNum = lightNodes.size();
	for (int i=0;i<nodeNum;i++)
	{
		// 位置从上面计数的
		lightNodes[i]->setPositionY(baseHeight-lightNodes[i]->getPositionY());
		lightNodes[i]->setBlendFunc(blendFunc);
		//注意渲染顺序，先渲染的为的精灵的颜色为目标颜色，后渲染的精灵的颜色为源颜色。
		lightNodes[i]->visit();
	}
	rt->end();

	return CCSprite::createWithTexture(rt->getSprite()->getTexture());
}

LightMaskFactory::~LightMaskFactory(void)
{
}
