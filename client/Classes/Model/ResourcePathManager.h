#ifndef __ResourcePathManager_H__
#define __ResourcePathManager_H__

#include "cocos2d.h"
#include <string>

using namespace std;
using namespace cocos2d;

enum Locale
{
	Locale_Ignore = 0,	// 忽略，就是不区分地区，不加后缀，要放在首位，因为changeLocale方法根据他来确认起始值的
	Locale_zh,			// 中文
	Locale_en,			// 英文
	Locale_default,		// 默认,要放在最后，因为changeLocale方法根据他来确认个数的
};


//一个资源路径的管理器,由于在iPad，iPhone上，游戏打包的资源不区分路径，但是对于：
//CCFileUtils::sharedFileUtils()->getFileData()就需要一个g_BoundleResourcePath全路径

//目前只完成图片资源路径的获取,最终得到的是：前缀+模式目录+文件名+高清标记+文件格式
//根据指定的
class ResourcePathManager
{
public:

	static ResourcePathManager* getManager();//拿到全局的资源管理器
	Locale getDiviceLocale();						//获得当前设备(系统)的位置，语系,不是我们当前设置的ResoureceManager的使用的Locale


	string getPath(string fileName);		// 根据忽略路径的图片名，得到默认语系（Locale_default）下面的真正的图片名：例如getPath("testPic.png");
	string getPath_locale(string fileName);	// 获得当前语系下面的对应的图片，当前的语系，默认是Locale_default

	// 设置图片的语言系，默认不设置就是当前设备的语言
	void setLocale(Locale loc);
	Locale getLocale();

	void changeLocale();
	
	//设置全局的路径前缀,例如setPrefixPath("season1/");
	//只对非iOS有效。
	void setPrefixPath(string prefix);

	string getFilePrefixPath();	// 获得整体的前缀

    //根据getPath得到的路径去找他默认的阴影路径
    string getShadowPath(string path);
	// 只是图片的前缀，比如season1_
    string getPicNamePrefixPath();
    
private:
	static ResourcePathManager* manager;
	ResourcePathManager();
	//设置全局的是否开启视网膜模式,不让外界设置了，按照cocos2dx默认的方式来，他会自动找文件，那么我们所有的操作只需要给名字和前缀即可.
	void setEnableRetina(bool enableRetina);

	Locale currentLocale;		// 位置，用于标记语言

	bool enableRetina;//是否开启了Retina，开启了就找高清图片
	
	string prefix;//全局的前缀

	string picName_prefix;//图片的文件名前缀(就是prefix种的“/”换成“_”)，比如原始为：pic.png,现在为picName_prefix + pic.png

	string modelSuffix;//当前模式下的文件名后缀：视网膜时就是"@2x",其他时候就是""

	string modelDir;//如果是retina模式，那么retina的图片所在的文件名

	//每次字符串被替换后，再在替换过的字符串中进行替换，是第一种，否则是第二种。
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

	string getPath(string fileName,Locale locale);	// 获得指定语系下面的对应的图片

};


#endif