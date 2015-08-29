#pragma once
#include <stdlib.h>
#include <string>
#include "cocos2d.h"

//看一个方法耗时多少

using namespace cocos2d;
using namespace std;

// http://blog.csdn.net/loseleo/article/details/9985737
class TimeLog
{
private:
	string functionName;
	struct cc_timeval tvBegin;
	struct cc_timeval lastLog;
public:
	TimeLog(string fucntionName)
	{
		CCLog("TimeLog>>%s:--> begin",fucntionName.c_str());
		this->functionName = fucntionName;
		CCTime::gettimeofdayCocos2d(&tvBegin, NULL);
		lastLog = tvBegin;
	}

	void log(string msg)
	{
		struct cc_timeval tv;
		CCTime::gettimeofdayCocos2d(&tv, NULL);
		CCLog("TimeLog>>%s:-->from last log timecost = %.3f ms",msg.c_str(),CCTime::timersubCocos2d(&lastLog,&tv));
		lastLog = tv;
	}

	~TimeLog()
	{
		struct cc_timeval tv;
		CCTime::gettimeofdayCocos2d(&tv, NULL);
		CCLog("TimeLog>>%s:--> end, timecost = %.3f ms",functionName.c_str(),CCTime::timersubCocos2d(&tvBegin,&tv));
	}
};

