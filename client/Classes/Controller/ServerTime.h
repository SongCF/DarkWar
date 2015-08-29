#pragma once

#include <time.h>
#include <string>
// 服务器时间
#include <ctime>

class ServerTime
{
private:
	static int time0_Offset;
public:

	static void setCurrentServerTime(int _s_time);
	// 获取时间的字符串表示
	static char* getTimeInString(int _s_time);
	// 获取大致的服务器时间
	static unsigned int getTime();
	// 
	static tm* getTime_tm();
	//
	static tm* getTime_tm(unsigned int _time);

	static std::string getTimeString(unsigned int _time);
	// format里面要包括6项数据，年月日，时分秒
	static std::string getTimeString(unsigned int _time,std::string format);

	ServerTime(void);
	~ServerTime(void);
};

