#pragma once
#include "cocos2d.h"

// cocos2d debug
#if COCOS2D_DEBUG == 0
#define LFLog(...)       do {} while (0)
#define LFLogError(...)  do {} while (0)
#define LFLogWarn(...)   do {} while (0)

#elif COCOS2D_DEBUG == 1
//cocos2d::CCLog("Error : %s line %d %s : %s",__FILE__,__LINE__,__FUNCTION__,cocos2d::CCString::createWithFormat(format, ##__VA_ARGS__)->getCString())
#define LFLog(format, ...)      cocos2d::CCLog("%s : %s",__FUNCTION__,cocos2d::CCString::createWithFormat(format, ##__VA_ARGS__)->getCString())
#define LFLogError(format,...)  cocos2d::CCLog("Error : %s line %d %s : %s",__FILE__,__LINE__,__FUNCTION__,cocos2d::CCString::createWithFormat(format, ##__VA_ARGS__)->getCString())
#define LFLogWarn(...)			cocos2d::CCLog("Waring : %s line %d %s : %s",__FILE__,__LINE__,__FUNCTION__,cocos2d::CCString::createWithFormat(format, ##__VA_ARGS__)->getCString())
#endif

