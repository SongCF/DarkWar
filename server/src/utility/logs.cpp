#include "logs.h"

static char g_szLog[512];


int LogInit(const char* szLogPath)
{
	snprintf(g_szLog, sizeof(g_szLog), "%s.log", szLogPath);

	return 0;
}


int LogMsg(const char *szFormat, ...)
{
	va_list ap;
	FILE *fp;
	int iSize;
	char szTime[32];
	time_t iCurrTime;
	struct tm stCurrTime;

	if((fp = fopen(g_szLog, "a+")) == NULL)
		return -1;	

	iSize = ftell(fp);
	if(iSize > 524288000)  //log文件不能超过500M
	{
		fclose(fp);
		return 0;
	}	

	time(&iCurrTime);
	localtime_r(&iCurrTime, &stCurrTime);
	sprintf(szTime, "%d%02d%02d %02d:%02d:%02d",
		stCurrTime.tm_year+1900, stCurrTime.tm_mon+1, stCurrTime.tm_mday,
		stCurrTime.tm_hour, stCurrTime.tm_min, stCurrTime.tm_sec);
	fprintf(fp, "[%s] ", szTime);
	
	va_start(ap, szFormat);
	vfprintf(fp, szFormat, ap);
	va_end(ap);

	fprintf(fp, "\n");
	fclose(fp);
	
	return 0;
}

