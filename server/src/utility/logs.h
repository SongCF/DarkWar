#if !defined _LOGS_H
#define _LOGS_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <iostream>

using namespace std;



int LogInit(const char* szLogPath);

int LogMsg(const char *szFormat, ...);

#endif
