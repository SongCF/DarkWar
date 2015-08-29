#ifndef __LFLayer_H__
#define __LFLayer_H__
#include "cocos2d.h"
#include <cmath>
#include <sys/timeb.h>
#include <time.h> 
using namespace cocos2d;

#define MOVE_CLICK_DIS 20
#define DOUBLE_CLICK_DIS 5
#define CLICK_Trigger 0.5f
class LFLayer : public CCLayer
{
private:
	CCPoint mPreBeginPos;
	timeb lastClickDownTime;
	void ccTouchesBegan (CCSet *pTouches, CCEvent *pEvent)
	{
		//获得刚点击的位置
		mBeginPos = getFirstTouchGLPoint(pTouches);
		lfTouchesBegan(mBeginPos);

		bool doubleClicked = false;

		if (lastClickDownTime.time > 0)
		{//代表之前已经点过一次了
			timeb temp = currentTime();
			if ( (temp.time - lastClickDownTime.time)*1000 + temp.millitm - lastClickDownTime.millitm <= CLICK_Trigger*1000)
			{
				if ( abs(mPreBeginPos.x - mBeginPos.x) <= DOUBLE_CLICK_DIS && abs(mPreBeginPos.y - mBeginPos.y) <= DOUBLE_CLICK_DIS)
				{
					doubleClicked = true;
					lfDoubleClickDown(mBeginPos);
					lastClickDownTime.time = 0;
				}
			}
		}
		mPreBeginPos = mBeginPos;

		if (!doubleClicked)
		{
			lastClickDownTime = currentTime();
		}
	}
	void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
	{
		//获得刚点击的位置
		CCPoint	point = getFirstTouchGLPoint(pTouches);
		lfTouchesMoved(point);
	}
	void ccTouchesEnded (CCSet *pTouches, CCEvent *pEvent)
	{
		//获得刚点击的位置
		CCPoint temp = getFirstTouchGLPoint(pTouches);

		if (abs(temp.x - mBeginPos.x)<MOVE_CLICK_DIS && abs(temp.y - mBeginPos.y)<MOVE_CLICK_DIS)
		{
			lfClick(temp);
		}
		lfTouchesEnded(temp);
	}
	//双击事件的触发器
	void doubleClickDownTrigger(float time)
	{
	}

	void now(char* buff)
	{
		struct timeb tp;
		struct tm *tm;
		ftime(&tp);
		tm = localtime(&tp.time);
		sprintf(buff, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d",
			1900+ tm->tm_year,
			1 + tm->tm_mon,
			tm->tm_yday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec,
			tp.millitm);
	}
	
	timeb currentTime()
	{
		struct timeb tp;
		ftime(&tp);
		return tp;
	}

	time_t currentTimeSec()
	{
		struct timeb tp;
		ftime(&tp);
		return tp.time;
	}
	unsigned short currentTimeMill()
	{
		struct timeb tp;
		ftime(&tp);
		return tp.millitm;
	}

protected:

	CCPoint mBeginPos;

	CCPoint getFirstTouchGLPoint(CCSet *pTouches)
	{
		CCSetIterator it = pTouches->begin();
		CCTouch* touch = (CCTouch*)(*it);//取得第一个Touch
		//获得刚点击的位置
		//CCPoint temp = touch->locationInView();	
		//temp = CCDirector::sharedDirector()->convertToGL( temp );
		//return temp;

		return touch->getLocation();
	}

	virtual void lfClick(CCPoint glPoint){}
	virtual void lfDoubleClickDown(CCPoint glPoint){}//双击，按下时调用
	virtual void lfTouchesBegan(CCPoint glPoint){}
	virtual void lfTouchesMoved(CCPoint glPoint){}
	virtual void lfTouchesEnded(CCPoint glPoint){}
public:
	LFLayer(void)
	{
		init();
		setTouchEnabled(true);
		lastClickDownTime.time = 0;
		char temp[512];
		now(temp);
		CCLog(temp);
	}
	~LFLayer(void)
	{

	}
};

#endif