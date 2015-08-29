#ifndef __LFUtile_H__
#define __LFUtile_H__

#include "cocos2d.h"
#include "LFStrings.h"

using namespace cocos2d;

#define LF_SPACE_BEGIN namespace LF {
#define LF_SPACE_END  }

LF_SPACE_BEGIN

//extern void setLablePos(CCLabelTTF* lable,CCPoint leftPos);
// 创建一个Lable
extern CCLabelTTF* lable(CCNode* toAdd, string lableStr,CCPoint leftPos,float size,string frontStr);
extern CCLabelTTF* lable(CCNode* toAdd, CCString* lableStr,CCPoint leftPos,float size,string frontStr);
extern CCLabelTTF* lable(CCNode* toAdd, int num,CCPoint leftPos,float size,string frontStr);

// 得到A-->B的一个向量。B-A的位置 的单位向量
extern CCPoint getVectorFromAToB(CCNode* A,CCNode* B);

// 得到item相对于nativeNode中的坐标值
extern CCPoint getNativePostion(CCNode* item,CCNode* nativeNode);

// 获得node的世界坐标
extern CCPoint getWorldPostion(CCNode* node);
/**
 *	获得点击事件的坐标
 */
extern CCPoint lfConvertToLocalPos(CCTouch* pTouch);

/**
 *	创建一个水平的，从左向右的进度条
 */
extern CCProgressTimer* createProgressTimer(CCSprite* sprite);

/**
 *	获取LFStrings配置的数据
 */
extern string getString(string key);

extern unsigned long long lf_htonll(unsigned long long val);

 /*return 1 : little-endian, return 0:big-endian*/
extern int checkCPUendian();

// 使屏幕是否常亮，true就常亮
extern void setScreenWake(bool awake);

extern float randomWithin(float start,float end);

// 计算baseVec旋转到vec.需要的角度
float getRotateAngle(CCPoint vec,CCPoint baseVec = ccp(0,1));

//编码转换只对win32有用
//因为android和iOS都可以直接从配置文件载入数据
extern int GBKToUTF8(std::string & gbkStr, const char* toCode, const char* fromCode);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
extern string UTF8ToGBK(const std::string strUTF8);
extern string GBKToUTF8(const std::string strGBK);
extern WCHAR* charToWChAR(string data);
#endif

// 把source 根据 token进行拆分
extern vector<string> string_split(string source,string token);

extern string ltrim(string str);
extern string rtrim(string str);
extern string trim(string str);
extern bool is_pws_valide(string str);	// 是否作为密码是可行的

//每次字符串被替换后，再在替换过的字符串中进行替换，是第一种，否则是第二种。
extern string& replace_all(string& str,const string& old_value,const string& new_value);  
extern string& replace_all_distinct(string&  str,const string& old_value,const   string&   new_value);  

LF_SPACE_END

#endif

