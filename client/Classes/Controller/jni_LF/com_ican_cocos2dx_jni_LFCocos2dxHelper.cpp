#include "com_ican_cocos2dx_jni_LFCocos2dxHelper.h"
#include <stdio.h>
#include "cocos2d.h"
#include "NetWorkThreadProc.h"
#include "GlobalNetworkMsgDispacher.h"
// 使用cocos2dx 的JNI方法，安全。
#include "platform/android/jni/JniHelper.h"
using namespace cocos2d;

extern void appDelegate_applicationDidEnterBackground();


void lfLog(const char* text)
{
	CCLog(text);
	//printf("\n%s\n",text);
}

JNIEXPORT void JNICALL Java_com_ican_cocos2dx_jni_LFCocos2dxHelper_gameActivityOnPause
(JNIEnv * jvm, jclass LFCocos2dxHelper)
{
	appDelegate_applicationDidEnterBackground();
}

bool createJavaDocFile(string dir,string fileName)
{
	CCLog("com_ican_cocos2dx_jni_LFCocos2dxHelper ---> createJavaDocFile dir = %s,fileName = %s",dir.c_str(),fileName.c_str());

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t,
		"com/ican/cocos2dx/jni/LFCocos2dxHelper",
		"createDocFile",
		"(Ljava/lang/String;Ljava/lang/String;)Z"))
	{
		jstring str_dir = t.env->NewStringUTF(dir.c_str());
		jstring str_name = t.env->NewStringUTF(fileName.c_str());
		jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID,str_dir,str_name);
		t.env->DeleteLocalRef(t.classID);
		t.env->DeleteLocalRef(str_dir);
		t.env->DeleteLocalRef(str_name);

		return ret == JNI_TRUE;
	}
#endif

	return false;
}



bool createJavaDocFile(string filePath)
{
	CCLog("com_ican_cocos2dx_jni_LFCocos2dxHelper ---> createJavaDocFile filePath = %s",filePath.c_str());
	string dir = filePath.substr(0,filePath.find_last_of("/"));
	string fileName = filePath.substr(filePath.find_last_of("/")+1);
	return createJavaDocFile(dir,fileName);
}

/**
 *	调用Java端，打开一个链接
 */
void openURLByJava(const char* url,int urlLength)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t,
		"com/ican/cocos2dx/jni/LFCocos2dxHelper",
		"openUrl",
		"(Ljava/lang/String;)V"))
	{
		jstring str = t.env->NewStringUTF(url);
		t.env->CallStaticVoidMethod(t.classID, t.methodID,str);
		t.env->DeleteLocalRef(t.classID);
		t.env->DeleteLocalRef(str);
	}
#endif
}
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
void g_SetScreenWake(bool awake)
{
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t,
		"com/ican/cocos2dx/jni/LFCocos2dxHelper",
		"setScreenWake",
		"(Z)V"))
	{
		jboolean parm = awake?JNI_TRUE:JNI_FALSE;
		t.env->CallStaticVoidMethod(t.classID, t.methodID,parm);
		t.env->DeleteLocalRef(t.classID);
	}
}
#endif
