#pragma once

#include "cocos2d.h"
using namespace cocos2d;

/**
 *	游戏场景的基础类，在这一层适配了所有分辨率，下面的比如Layer，SPrite，Node等都不用考虑了
 *	只需要认为开发的绝对操作区是1024*768即可。
 */
class GameScene : public CCScene
{
public:
	static GameScene* createScene();
	GameScene(void);
	~GameScene(void);
};
