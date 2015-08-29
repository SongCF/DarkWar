#pragma once
#include <vector>
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

/**
 CCSprite* A = CCSprite::create("zhandou_jiugongge_touming.png");
 CCSprite* B = CCSprite::create("light.png");
 CCSprite* C = CCSprite::create("light.png");
 CCSprite* D = CCSprite::create("light.png");
 vector<CCSprite*> vec;
 vec.push_back(B);
 vec.push_back(C);
 vec.push_back(D);
 B->setPosition(ccp(A->getContentSize().width/2,A->getContentSize().height/2));
 C->setPosition(ccp(A->getContentSize().width/5,A->getContentSize().height/2));
 D->setPosition(ccp(A->getContentSize().width/2,A->getContentSize().height * 0.8f));
 D->setScaleX(1.8f);
 C->setScaleY(1.8f);
 CCSprite* build = LightMaskFactory::getCurrentFactory()->buildSprite(A,vec);
 this->addChild(build);
 */
class LightMaskFactory
{
protected:
	static LightMaskFactory* instance;

	LightMaskFactory(void);
public:
	static LightMaskFactory* getCurrentFactory();

	// 构建一个Sprite,是以base为底
	// lightNodes为亮点盖在base上面，扣掉Base对应的地方的像素
	// 外界需要设置好lightNode跟base的相对位置
	CCSprite* buildSprite(CCSprite* base,vector<CCSprite*> lightNodes);
	CCSprite* buildSprite(CCSprite* base,CCSprite* lightNode);

	~LightMaskFactory(void);
};

