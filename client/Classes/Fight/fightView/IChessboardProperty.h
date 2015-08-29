#pragma once

#include "cocos2d.h"
using namespace cocos2d;

class IChessboardProperty
{
public:
	// 获取棋盘的中心点坐标，是世界GL坐标
	virtual CCPoint getCenterWorldPoint() = 0;
};

