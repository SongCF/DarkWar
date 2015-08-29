#include "LFUtile.h"
#if CC_TARGET_PLATFORM != CC_PLATFORM_WIN32
#include <endian.h>
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "iconv/iconv.h"
#pragma comment(lib,"libiconv.lib") 
#include <Windows.h>
#include "tchar.h"
#endif

// iOS用
extern bool ByteOrder_In_Big;

extern void g_SetScreenWake(bool awake);

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
void g_SetScreenWake(bool awake)
{
	CCLog("g_SetScreenWake[%s]--> win32 do nothing!",awake?"true":"false");
}
#endif


using namespace cocos2d;
LF_SPACE_BEGIN


int checkCPUendian(){
	union {
		unsigned int a;
		unsigned char b;            
	}c;
	c.a = 1;
	return (c.b == 1);      
}   /*return 1 : little-endian, return 0:big-endian*/

void setScreenWake(bool awake)
{
	//g_SetScreenWake(awake);
	CCAssert(false,"没有实现");
}

unsigned long long lf_htonll(unsigned long long val)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (ByteOrder_In_Big)
	{
		return val;
	}
	else
	{
		return (((unsigned long long )htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));
	}

#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
	{
		return (((unsigned long long )htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));

	}else if (__BYTE_ORDER == __BIG_ENDIAN)
	{
		return val;
	}
#endif
	CCAssert(false,"");
	return val;
}

float randomWithin(float start,float end)
{
	return start + (end-start)*CCRANDOM_0_1();
}

float getRotateAngle(CCPoint vec,CCPoint baseVec /* = ccp */)
{
	float angle = CC_RADIANS_TO_DEGREES(ccpAngle(vec,baseVec));
	//if (vec.x < -0.0f)
	if (vec.x < baseVec.x)
		angle = -angle;
	return angle;
}


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

//void setLablePos(CCLabelTTF* lable,CCPoint leftPos)
//{
//	//l->setPosition(ccpAdd(leftPos,ccp(l->getContentSize().width/2,l->getContentSize().height/2)));
//	lable->setPosition(ccpAdd(leftPos,ccp(lable->getContentSize().width/2,0)));
//}

CCLabelTTF* lable(CCNode* toAdd, string lableStr,CCPoint leftPos,float size,string frontStr)
{
	CCLabelTTF *l = CCLabelTTF::create(lableStr.c_str(), frontStr.c_str(), size);
	//setLablePos(l,leftPos);
	if(leftPos.x != 0 || leftPos.y != 0)
	{
		l->setAnchorPoint(ccp(0, 0.5));
	}
	
	l->setPosition(leftPos);
	if (toAdd)
	{
		toAdd->addChild(l);
	}
	return l;
}
CCLabelTTF* lable(CCNode* toAdd, CCString* lableStr,CCPoint leftPos,float size,string frontStr)
{
	return lable(toAdd,lableStr->getCString(),leftPos,size,frontStr);
}
CCLabelTTF* lable(CCNode* toAdd, int num,CCPoint leftPos,float size,string frontStr)
{
	return lable(toAdd,CCString::createWithFormat("%d",num),leftPos,size,frontStr);
}

CCPoint lfConvertToLocalPos(CCTouch* pTouch)
{
	//CCPoint temp = pTouch->locationInView();	
	//temp = CCDirector::sharedDirector()->convertToGL( temp );
	//return temp;
	return pTouch->getLocation();
}

CCPoint getVectorFromAToB(CCNode* A,CCNode* B)
{
	if (A == B)
	{
		return CCPointZero;
	}
	
	CCPoint APos = A->getParent()->convertToWorldSpace(A->getPosition());
	CCPoint BPos = B->getParent()->convertToWorldSpace(B->getPosition());
	CCPoint move = ccpSub(BPos,APos);
	move = ccpNormalize(move);
	return move;
}

CCPoint getWorldPostion(CCNode* node)
{
	return node->getParent()->convertToWorldSpace(node->getPosition());
}

CCPoint getNativePostion(CCNode* item,CCNode* nativeNode)
{
	return nativeNode->convertToNodeSpace(getWorldPostion(item));
}

CCProgressTimer* createProgressTimer(CCSprite* sprite)
{
	CCProgressTimer *progress = CCProgressTimer::create(sprite);
	progress->setType( kCCProgressTimerTypeBar );
	progress->setMidpoint(ccp(0,0));
	progress->setBarChangeRate(ccp(1, 0));
	return progress;
}

string getString(string key)
{
	return LFStrings::getValue(key);
}

// 调用：GBKToUTF8(str对象, “gb2312”, “utf-8”);
// 首先要加入iconv库，然后引入iconv.h
// #include "iconv/iconv.h"
// #pragma comment(lib,"libiconv.lib") 
// 而且 if (-1 == iconv(iconvH, pin, &strLength, &outbuf, &outLength)) 这行代码在mac上报错
int GBKToUTF8(std::string & gbkStr, const char* toCode, const char* fromCode)  
{  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	iconv_t iconvH;  
	iconvH = iconv_open(fromCode, toCode);  
	if (iconvH == 0)  
		return -1;  
	const char* strChar = gbkStr.c_str();  
	const char** pin = &strChar;  
	size_t strLength = gbkStr.length();  
	char* outbuf = (char*) malloc(strLength*4);  
	char* pBuff = outbuf;  

	memset( outbuf, 0, strLength*4);  
	size_t outLength = strLength*4;  
	if (-1 == iconv(iconvH, pin, &strLength, &outbuf, &outLength))  
	{  
		iconv_close(iconvH);  
		return -1;  
	}  
	gbkStr = pBuff;  
	iconv_close(iconvH);  
	return 0;  
#endif
	CCLog("GBKToUTF8 ---> is not win32 do nothing");
	return 0;
} 

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
// 只对win32有用
string UTF8ToGBK(const std::string strUTF8)
{
	WCHAR* wszGBK = charToWChAR(strUTF8);
	int len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP,0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}
// 只对win32有用
string GBKToUTF8(const std::string strGBK)
{
	string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;

}

// 需要自己delete
WCHAR* charToWChAR(string data)
{
	const char* w_data = data.c_str();
	//确定转换为Unicode需要多少缓冲区(返回值也包含了最后一个NULL字符)。
	int nLen = MultiByteToWideChar(CP_ACP, 0, w_data, -1, NULL, 0);
	WCHAR* wBuf = new WCHAR[nLen];
	MultiByteToWideChar(CP_ACP, 0, w_data, -1, wBuf, nLen);
	return wBuf;
}
#endif

string ltrim(string str)
{
	string ret = str;
	ret.erase(0,ret.find_first_not_of("\r\t\n "));
	return ret;
}
string rtrim(string str)
{
	string ret = str;
	ret.erase(ret.find_last_not_of("\r\t\n ")+1);
	return ret;
}
string trim(string str)
{
	return ltrim(rtrim(str));
}

// 把source 根据 token进行拆分
vector<string> string_split(string source,string token)
{
	vector<string> ret;

	// 参数解析
	char* cmd = new char[source.length() + 1];
	memset(cmd,0,source.length() + 1);
	strncpy(cmd,source.c_str(),source.length());
	char *tokenPtr=strtok(cmd,token.c_str());
	while(tokenPtr!=NULL)
	{
		ret.push_back(string(tokenPtr));
		tokenPtr=strtok(NULL,token.c_str());
	}
	delete [] cmd;

	return ret;
}

// 只能是AscII编码的非空白字符都行
bool is_pws_valide(string str)
{
	char c[2] ;
	c[0]= 0;
	c[1]= 0;
	for (string::iterator it = str.begin();it != str.end();it++)
	{
		c[0] = (*it);
		if (c[0] > 128 || c[0] < 0)
		{// 非 asc 码
			return false;
		}
	}

	//if (str.find_last_not_of("\r\t\n") != string::npos)
	//{// 有空白字符,空格可以
	//	return false;
	//}
	
	return true;
}

string& replace_all(string& str,const string& old_value,const string& new_value)   
{   
	while(true)   {   
		string::size_type   pos(0);   
		if(   (pos=str.find(old_value))!=string::npos   )   
			str.replace(pos,old_value.length(),new_value);   
		else   break;   
	}   
	return   str;   
} 
string& replace_all_distinct(string&  str,const string& old_value,const   string&   new_value)   
{   
	for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {   
		if(   (pos=str.find(old_value,pos))!=string::npos   )   
			str.replace(pos,old_value.length(),new_value);   
		else   break;   
	}   
	return   str;   
}   

LF_SPACE_END
