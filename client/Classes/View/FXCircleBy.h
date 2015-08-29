#pragma once
#include "cocos2d.h"
using namespace cocos2d;

//很多不支持
//反转  reverse
//复制  copyWithZone
//...

class FXCircleBy : public CCActionInterval
{
public:
	//动作时间、  圆心、  半径、   转多少度
	static FXCircleBy * create(float duration, CCPoint centerPos, float radius, float angle);
	bool init(float duration, CCPoint centerPos, float radius, float angle);

	virtual void update(float t);  //动作完成的百分比
	virtual void startWithTarget(CCNode *pTarget);

protected:
	CCPoint mBeginPos;     //开始运行动作时，node的位置
	CCPoint mCenterPos;    //圆心
	float mRadius;         //半径
	float mAngle;          //一共旋转多少角度
	float mOriginAngle;    //开始运行动作时， node的位置与圆心所形成的角度。
};