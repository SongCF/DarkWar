#include "LFMessageBox.h"
#include "LFAlert.h"
#include "BaseSprite.h"
#include "font.h"
#include "LFUtile.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "iconv/iconv.h"
#pragma comment(lib,"libiconv.lib") 
#include <Windows.h>
#include "tchar.h"
#endif

//	CCMessageBox 只显示UTF-8的文字，但是唯一跟一起不一样的是，win32平台显示的不是UTF-8的文字
//	所有得分开对待
LFMessageBox::LFMessageBox(string title,string msg)
{
	//BaseSprite* base = BaseSprite::create("shangpin_goumai_tishikuang.png");
	//CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
	//	"shangpin_goumai_queren.png",
	//	"shangpin_goumai_queren_select.png",
	//	this,SEL_MenuHandler(&LFMessageBox::itemClicked));
	//CCMenu* menu = CCMenu::create(menuItemMakeSure,NULL);
	//menu->setPosition(CCPointZero);
	//menuItemMakeSure->setPosition(ccp(base->getContentSize().width/2,90));

	//float contentTopPadding = 40;	// 是内容留给上面的空白，也是标题可用的区域
	//float contentButtomPadding = 100;
	//float contentLeftPadding = 10;
	//float contentRightPadding = 10;

	//float titleTopPadding = 5;
	//float titleButtomPadding = 5;
	//float titleTopLeftPadding = 10;
	//float titleTopRightPadding = 10;

	//// 内容
	//{
	//	CCLabelTTF* lable = CCLabelTTF::create(
	//		msg.c_str(),
	//		fontStr_kaiti,
	//		30,
	//		CCSizeMake(	base->getContentSize().width - contentLeftPadding - contentRightPadding,
	//					base->getContentSize().height - contentTopPadding - contentButtomPadding),
	//		kCCTextAlignmentLeft);
	//	lable->setAnchorPoint(ccp(0.0f,0.0f));
	//	lable->setPosition(ccp(contentLeftPadding, base->getContentSize().height - contentTopPadding));
	//	ccColor3B color;
	//	color.r = 201;
	//	color.g = 156;
	//	color.b = 18;
	//	lable->setColor(color);

	//	base->addChild(lable);
	//}

	//// 标题
	//{
	//	CCLabelTTF* lable = CCLabelTTF::create(
	//		msg.c_str(),
	//		fontStr_kaiti,
	//		35,
	//		CCSizeMake(	base->getContentSize().width - titleTopLeftPadding - titleTopRightPadding,
	//					contentTopPadding - titleTopPadding - titleButtomPadding),
	//		kCCTextAlignmentLeft);
	//	lable->setAnchorPoint(ccp(0,0.0f));
	//	lable->setPosition(ccp(titleTopLeftPadding, base->getContentSize().height - titleTopPadding));
	//	ccColor3B color;
	//	color.r = 201;
	//	color.g = 156;
	//	color.b = 18;
	//	lable->setColor(color);

	//	base->addChild(lable);
	//}

	//// 需要没有添加到某个Node的Menu和没有添加到某个Node的base
	//LFAlert* lfAlert = new LFAlert();
	//lfAlert->setBaseContainer(base,menu);
	//addChild(lfAlert);
	//lfAlert->release();

}

void LFMessageBox::itemClicked(CCObject* sender)
{
	//removeFromParent();
	//CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_ALERT_CLOSE);
}

//int utf2gbk()
//{
//	size_t ret;
//	char in_utf8[] = {0xE6,0xAD,0xA3, 0xE5, 0x9C, 0xA8, 0xE5, 0xAE , 0x89 ,0xE8 ,0xA3 ,0x85, 00};
//	char in_gb2312[] = "正在安装";
//	char *in;
//	size_t src_len = strlen(in_utf8);
//	char *szDest;
//	char *sout;
//	size_t dest_len = 64;
//	iconv_t conv;
//	conv = iconv_open("gbk","utf-8");
//	if (conv == (iconv_t)-1)
//	{
//		return -1;
//	}
//	szDest=(char *)malloc(64);
//	if ( szDest == NULL) return -1;
//	memset(szDest,0,64);
//	sout = szDest;
//	in=in_utf8;
//	ret = iconv(conv,(const char**)&in,(size_t *)&src_len,(char **)&szDest,(size_t *)&dest_len);
//	if (ret == -1)
//	{
//		return -1;
//	}
//	printf("%s\r\n",sout);
//	iconv_close(conv);
//	if ( sout ) free( sout);
//	return 0;
//}

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
int utf8_to_gbk(string utf8,string& out_gbk)
{
	size_t ret;
	const char *in;
	size_t src_len = utf8.length();
	char *szDest;
	char *sout;
	size_t dest_len = src_len * 4 + 1;
	iconv_t conv;
	conv = iconv_open("gbk","utf-8");
	if (conv == (iconv_t)-1)
	{
		return -1;
	}
	szDest=(char *)malloc(dest_len);
	if ( szDest == NULL) return -1;
	memset(szDest,0,dest_len);
	sout = szDest;
	in = utf8.c_str();
	ret = iconv(conv,(const char**)&in,(size_t *)&src_len,(char **)&szDest,(size_t *)&dest_len);
	if (ret == -1)
	{
		return -1;
	}
	out_gbk = string(sout);
	iconv_close(conv);
	if ( sout ) free( sout);
	return 0;
}
#endif
void LFMessageBox::alert(string title,string msg)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	string out1;
	string out2;
	utf8_to_gbk(title,out1);
	utf8_to_gbk(msg,out2);
	CCMessageBox(out2.c_str(),out1.c_str());
#else
	CCMessageBox(msg.c_str(),title.c_str());
#endif
}


LFMessageBox::~LFMessageBox(void)
{
}
