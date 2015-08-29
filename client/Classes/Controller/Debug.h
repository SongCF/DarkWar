#ifndef __Debug__
#define __Debug__
// 不要在头文件中包含我
// 到时修改了导致的编译很困难

#include "cocos2d.h"
using namespace cocos2d;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

/**
 *	一个开启和关闭调试功能的头文件，发布的时候，全部禁用Debug即可
 *	规定一下，这里面定义的都是DEBUG_ENABLE_****这种，即都是是否启用某东西
 *	这样编程，就不会有疑问了
 */
//#define ALWAYS_USE_SKILL	1	// 总是使用技能
////
//#define SHOW_DEBUG_LABLE	1	// 显示主界面的goto Test的lable
////
//#define SHOW_FIGHT_DEBUG	1	// 显示战斗的debug

//#define DOT_SHOW_GUIDE 1 

#endif

#endif