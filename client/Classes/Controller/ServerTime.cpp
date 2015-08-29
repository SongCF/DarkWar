#include "ServerTime.h"
#include "cocos2d.h"

using namespace cocos2d;

int ServerTime::time0_Offset = 0;

ServerTime::ServerTime(void)
{
}

ServerTime::~ServerTime(void)
{
}

void ServerTime::setCurrentServerTime(int _s_time)
{
	int clientTime = time(0);
	time0_Offset = _s_time - clientTime;
}

char* ServerTime::getTimeInString(int _s_time)
{
	time_t timep = _s_time; 
	/*用gmtime将time_t类型的时间转换为struct tm类型的时间按， 
	 然后再用asctime转换为我们常见的格式 Fri Jan 11 17:25:24 2008 
	*/ 
	return asctime(gmtime(&timep));
}

// 获取大致的服务器时间
unsigned int ServerTime::getTime()
{
	int clientTime = time(0);

	return clientTime + time0_Offset;
}

tm* ServerTime::getTime_tm()
{
	int clientTime = time(0);
	time_t timep = clientTime + time0_Offset; 

	return gmtime(&timep);
}

tm* ServerTime::getTime_tm(unsigned int _time)
{
	time_t timep = _time; 
	return gmtime(&timep);
}

std::string ServerTime::getTimeString(unsigned int _time)
{
	return getTimeString(_time,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d");
	//char timeString[128];
	//time_t timep = time(0); 
	//tm* _tm = gmtime(&timep);
	//sprintf(timeString,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
	//	_tm->tm_year+1900,_tm->tm_mon+1,_tm->tm_mday,
	//	_tm->tm_hour+8,_tm->tm_min,_tm->tm_sec);
	//return timeString;
};

std::string ServerTime::getTimeString(unsigned int _time,std::string format)
{
	char timeString[128];
	time_t timep = _time; 
	tm* _tm = gmtime(&timep);
	//sprintf(timeString,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
	//	_tm->tm_year+1900,_tm->tm_mon+1,_tm->tm_mday,
	//	_tm->tm_hour+8,_tm->tm_min,_tm->tm_sec);
	sprintf(timeString,format.c_str(),
		_tm->tm_year+1900,_tm->tm_mon+1,_tm->tm_mday,
		_tm->tm_hour+8,_tm->tm_min,_tm->tm_sec);
	return timeString;
}