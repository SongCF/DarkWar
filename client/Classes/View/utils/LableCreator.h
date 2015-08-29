#pragma once

#include "cocos2d.h"
#include <string>
using namespace std;
using namespace cocos2d;
/**
 *	创建文本的类
 */
class LableCreator
{
private:
	LableCreator(void);

	/**
	 *	创建一个任意语言的文字，需要指定字体和文字大小。
	 *	
	 *	对传入的文本要求是gbk的，win32和安卓都可以直接是gbk。
	 */
	CCLabelTTF* createByGBK(string gbk_string,string fnt,float size);
	/**
	 *	创建一个任意语言的文字，需要指定字体和文字大小。
	 *	注意这里的文字要求是utf-8编码的，这个是运行时传入的。而不是只要代码文件为utf8编码就行的
	 */
	CCLabelTTF* createByUTF8(string utf8_string,string fnt,float size);
public:
	static LableCreator* getCreator();

	/**
	 *	创建一个任意语言的文字，需要指定字体和文字大小。
	 */
	CCLabelTTF* create(string str,string fnt,float size);

	~LableCreator(void);
};
