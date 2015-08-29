#include "ResourcePathManager.h"

/**
 *	版本的控制数据，描述信息
 */
#define Tag_Version_1_1		1.1f			//版本1.1 这个版本不再区分目录了，不能再调用setPrefixPath
#define Current_Version Tag_Version_1_1		//当前版本


string g_localeSuffix[] = 
{
	"",				// 忽略Locale_Ignore
	"_zh",			// 中文Locale_zh
	"_en",			// 英文Locale_en
	"_en"			// 英文Locale_default
};


ResourcePathManager* ResourcePathManager::manager;
ResourcePathManager::ResourcePathManager()
{
	currentLocale = getDiviceLocale();

	//// 测试
	//currentLocale = Locale_en;

	prefix = "";					//就是season1/之类的
	modelDir = "";					//iOS下就是"",其他的是retina或normal
	picName_prefix = "";			//加上图片名前缀比如season1_
	modelSuffix = "";				//加上视网膜的后缀：testPic@2x,现在都是"",因为是按照coco2dx默认的去找
	setEnableRetina(false);			//没找到 查看是否开启了retina的函数
}

ResourcePathManager* ResourcePathManager::getManager()
{
	if (manager == NULL)
	{
		manager = new ResourcePathManager();
	}
	return manager;
}

/** 
 *	结果等于：
 *	前缀+模式的目录(比如retina)+文件名前缀(season1_)+文件名(比如test)+地域标记(_zh)+模式的后缀(比如@2x,ipadhd)+后缀名(.png)
 *	season/retina/season1_test_zh@2x.png
 *	我们这里处理过程如下：
 *
 *  这里实际获得的是：
 *	test.png	(本身数据)	---->	
 *	season/retina/season1_test_zh.png	(Manager返回数据)	---->	  
 *	season/retina/season1_test_zh-ipadhd.png	(cocos2dx真正的数据)
 *
 *	版本1.1：
 *	处理情况(由于实现统一目录了，所以就不在套很多目录了)：
 *	这里使用默认的语系：Locale_default，就没有后缀了
 *	test.png	(本身数据)	---->	
 *	retina/test_zh.png	(Manager返回数据)	---->	  
 *	retina/test_zh-ipadhd.png	(cocos2dx真正的数据)
 */
string ResourcePathManager::getPath(string fileName)
{
	return getPath(fileName,Locale_Ignore);
}
string ResourcePathManager::getPath_locale(string fileName)
{
	return getPath(fileName,currentLocale);
}

void ResourcePathManager::setLocale(Locale loc)
{
	currentLocale = loc;
	CCLog("ResourcePathManager::setLocale = [%s]",g_localeSuffix[loc].c_str());
}
Locale ResourcePathManager::getLocale()
{
	return currentLocale;
}
void ResourcePathManager::changeLocale()
{
	currentLocale = (Locale)(((int)currentLocale) + 1);

	if (currentLocale >= Locale_default)
	{
		currentLocale = (Locale)(((int)Locale_Ignore) + 1);
	}
}
Locale ResourcePathManager::getDiviceLocale()
{
	Locale locale = Locale_default;
	//ccLanguageType currentLanguageType = CCApplication::sharedApplication().getCurrentLanguage();
	ccLanguageType currentLanguageType = CCApplication::sharedApplication()->getCurrentLanguage();
	switch (currentLanguageType)
	{
	case kLanguageEnglish:
		locale = Locale_en;
		CCLog("getDiviceLocale--->current language is English.");
		break;
	case kLanguageChinese:
		locale = Locale_zh;
		CCLog("getDiviceLocale--->current language is Chinese.");
		break;
	case kLanguageFrench:
		CCLog("getDiviceLocale--->current language is French.");
		break;
	case kLanguageGerman:
		CCLog("getDiviceLocale--->current language is German.");
		break;
	case kLanguageItalian:
		CCLog("getDiviceLocale--->current language is Italian.");
		break;
	case kLanguageRussian:
		CCLog("getDiviceLocale--->current language is Russian.");
		break;
	case kLanguageSpanish:
		CCLog("getDiviceLocale--->current language is Spanish.");
		break;
	}
	return locale;
}
string ResourcePathManager::getPath(string fileName,Locale locale)
{
	int dotPos = fileName.find_last_of(".",-1);
	int stringLength = fileName.length();
	string extensionName = fileName.substr(dotPos,stringLength);	//扩展名的名字：.png
	string no_extensionName = fileName.substr(0,dotPos);			//没有扩展名的名字：testPic

	string newFilePath = prefix;					//就是season1/之类的
	newFilePath += modelDir;						//iOS下就是"",其他的是retina或normal
	newFilePath += picName_prefix;					//加上图片名前缀比如season1_
	newFilePath += no_extensionName;				//原始的fileName
	newFilePath += g_localeSuffix[locale];	//加上地域标记的后缀
	newFilePath += modelSuffix;						//加上视网膜的后缀：testPic@2x,现在都是"",因为是按照coco2dx默认的去找
	newFilePath += extensionName;					//还原为原来的，如.png

	return newFilePath;
}

/**
 *	获取阴影的路径,不可消除和可以消除的用同一个阴影
 *	拼接方法是在最后面加_shadow.
 *	例如：test.png
 *	变成：test_shadow.png
 *	实际上(cocos2dx方面)：test_shadow-ipadhd.png
 */
string ResourcePathManager::getShadowPath(string path)
{
    string p = path;
    //replace_all_distinct(p,"_no.png",".png"); // 不在让不可消除的也用这个图片了，每一个都用不同的图片
    replace_all_distinct(p,".png","_shadow.png");
    return p;
}

void ResourcePathManager::setEnableRetina(bool enableRetina)
{
	this->enableRetina = enableRetina;
	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{//如果是IOS,那么不需要前缀
		if (enableRetina)
		{
			this->modelDir = "";
			//this->modelSuffix = "@2x";
			this->modelSuffix = "";//使用ccos2dx默认的，所以这里不需要modelSuffix了
		}else
		{
			this->modelDir = "";
			this->modelSuffix = "";
		}
	}else
	{
		if (enableRetina)
		{
			//非IOS下面不准开视网膜
			CCAssert(true,"you can not enable retina display on no-iOS platform");
			//this->modelDir = "retina/";
			//this->modelSuffix = "@2x";

			//this->modelDir = "normal/";
			this->modelDir = "";
			this->modelSuffix = "";	//后缀
		}else
		{
			//this->modelDir = "normal/";
			this->modelDir = "";
			this->modelSuffix = "";
		}
	}
}



/**
 *	版本1.1：
 *	这个版本由于不再区分目录了，所以就不需要在用这个函数改变目录了
 */
void ResourcePathManager::setPrefixPath(string prefix)
{
	if (Current_Version == Tag_Version_1_1)
		CCAssert(false,"Tag_Version_1_1 不能调用这个函数!");

	this->picName_prefix = prefix;
	replace_all(picName_prefix,"/","_");

	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{//如果是IOS,那么不需要前缀
		this->prefix = "";
	}else
	{
		this->prefix = prefix;
	}
}
string ResourcePathManager::getFilePrefixPath()
{
	string newFilePath = prefix;				//就是season1/之类的
	newFilePath += modelDir;					//iOS下就是"",其他的是retina或normal
	newFilePath += picName_prefix;				//加上图片名前缀比如season1_
	return newFilePath;
}

string ResourcePathManager::getPicNamePrefixPath()
{
    return picName_prefix;
}

