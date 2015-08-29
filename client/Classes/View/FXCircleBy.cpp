#include "FXCircleBy.h"

#define PI 3.14159f

FXCircleBy * FXCircleBy::create(float duration, CCPoint centerPos, float radius, float angle)
{
	FXCircleBy *action = new FXCircleBy();
	if (action && action->init(duration, centerPos, radius, angle))
	{
		action->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(action);
		action = NULL;
	}

	return action;
}

bool FXCircleBy::init(float duration, CCPoint centerPos, float radius, float angle)
{
	if (CCActionInterval::initWithDuration(duration))
	{
		mCenterPos = centerPos;
		mRadius = radius;
		mAngle = angle;

		return true;
	}

	return false;
}

void FXCircleBy::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
	mBeginPos  = m_pTarget->getPosition();

	//计算出其实角度    y = sin@ * R;
	mOriginAngle = asin((mBeginPos.y - mCenterPos.y) / mRadius) * 180 / PI;
}

//角度需要转换为弧度

void FXCircleBy::update(float t)
{
	float deltaAngle = mAngle * t;
	float curAngle = mOriginAngle + deltaAngle;
	// y = sin@ * R;
	// x = cos@ * R;
	float dx = cos(curAngle * PI / 180) * mRadius;
	float dy = sin(curAngle * PI / 180) * mRadius;
	m_pTarget->setPosition(ccp(mCenterPos.x + dx, mCenterPos.y + dy));


//	CCLog("deltaAngle = %5.2f, curAngle = %5.2f,\n dx,dy = (%3.1f, %3.1f)", deltaAngle, curAngle, dx, dy);
}