#include "GameInforEditer.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "cocos2d.h"
#include "Music.h"

using namespace cocos2d;
//配置文件的key;
string readGameInforPath = "infor/infor.txt";
string writeGameInforPath = "infor/infor.txt";

// datapool里面回调
void g_SetNeedComment(string url)
{
	GameInforEditer::getGameInfor()->setNeedComment(true,url);
	GameInforEditer::getGameInfor()->saveSessonInforList();
	CCLog("g_SetNeedComment ---> url = [%s]",url.c_str());
}

using namespace std;

static string isFirstRunApp = "isFirstRunApp";
static string clientId = "clientId";
static string userName = "userName";
static string password = "password";
static string hero_id = "hero_id";
static string initLocale = "initLocale";
static string lastSettedLocale = "lastSettedLocale";
static string needCommentGame = "needCommentGame";
static string commentUrl = "commentUrl";
static string backgroundSoundOpen = "backgroundSoundOpen";
static string effctiveSoundOpen = "effctiveSoundOpen";
static string lockScreen = "lockScreen";
static string ServerMessage = "ServerMessage";

GameInforEditer* GameInforEditer::gameInforEditer;

GameInforEditer::GameInforEditer()
{
	mIsFirstRunApp = true;
	mInitLocale = Locale_default;

	mNeedCommentGame = false;
	mCommentUrl = "";
	lastUsedName = "";
	lastUsedPsw = "";
	lastLoginedHeroId = 0;

	fileUtiles = LFFileUtiles::getLFFileUtiles();
	fileUtiles->copyFileFromResToDocIfNotExist(writeGameInforPath);

	bool sec = initGameInfor(readGameInforPath);
	if (!sec)
	{
		CCLog("\n======GameInforEditer construct failed !");
	}
}

GameInforEditer::~GameInforEditer()
{
}
Locale GameInforEditer::getInitLocale()
{
	return mInitLocale;
}
int GameInforEditer::getClientId()
{
	return mClientId;
}
void GameInforEditer::setClientId(int _id)
{
	CCAssert(_id != 0,"This Method can just called by first Time get the ClientId");

	// 如果已经是分配了Id的了,那么 就应该 服务器返回的数据也是这个Id,不用修改
	if (mClientId != 0)
	{
		CCAssert(mClientId == _id ,"We have already getted the clientId,you can not change it!");

		return;
	}

	mClientId = _id;

	saveSessonInforList();
}

string GameInforEditer::getLastUsername()
{
	return lastUsedName;
}
string GameInforEditer::getLastPasseord()
{
	return lastUsedPsw;
}
unsigned GameInforEditer::getLastLoginedHeroId()
{
	return lastLoginedHeroId;
}
void GameInforEditer::setLastUsedNameAndPsw_save(string name,string psw)
{
	lastUsedName = name;
	lastUsedPsw = psw;

	// 保存
	saveSessonInforList();
}

void GameInforEditer::setLastLoginedHeroId(unsigned int _id)
{
	lastLoginedHeroId = _id;

	saveSessonInforList();
}

GameInforEditer* GameInforEditer::getGameInfor()
{
	if (gameInforEditer == NULL)
	{
		gameInforEditer = new GameInforEditer();
	}
	return gameInforEditer;
}
//在iPad上调用coco2dx的读写时不需要相对前缀的。
//但是直接使用fileOpen等是需要路径的;
//在windows上，都是需要路径的;
bool GameInforEditer::initGameInfor(string fileFullPath)
{
	Json::Reader reader;
	Json::Value root;
	unsigned long size = 0;
	CCLog("%s%s","Loading gameInfor->",fileFullPath.c_str());

	const char* doc = fileUtiles->readDocFile(fileFullPath,&size);
	//文件读取失败就返回NULL;
	if (size<=0)
	{
		CCLog("%s%s%s","\n=======readInforFile---->Load gameInfor->",fileFullPath.c_str()," fail !");
		return false;
	}

	bool parseOK = parseInforString(doc);
	if (!parseOK)
	{
		CCLog(doc);
		CCAssert(false,"解析游戏保存的数据信息出错,详见infor.txt");
	}
	
	delete[] doc;

	return true;
}

bool GameInforEditer::parseInforString(string str)
{
	Json::Reader reader;
	Json::Value root;
	//解析不成功就返回空;
	if (!reader.parse(str, root))
	{
		CCLog("\n=======parseInforString---->Parse seasonInfo failed !");
		return false;
	}
	//解析isFirstRunApp
	{
		Json::Value _isFirstRunApp = root[isFirstRunApp];
		mIsFirstRunApp = _isFirstRunApp.asBool();
	}
	// 评论
	{
		if (!root[needCommentGame].isNull())
		{
			mNeedCommentGame = root[needCommentGame].asBool();
		}else
		{
			mNeedCommentGame = false;
		}

		if (mNeedCommentGame && !root[commentUrl].isNull())
		{
			mCommentUrl = root[commentUrl].asString();
		}else
		{
			mCommentUrl = "";
		}
	}
	{
		Json::Value _id = root[clientId];
		if (!_id.isNull())
		{
			mClientId = _id.asInt();
		}else
		{
			mClientId = 0;
		}

		if (root[userName].isNull() || root[password].isNull())
		{
			lastUsedName = "";
			lastUsedPsw = "";
		}
		else
		{
			lastUsedName = root[userName].asString();
			lastUsedPsw = root[password].asString();
			if (!root[hero_id].isNull())
			{
				lastLoginedHeroId = root[hero_id].asUInt();
			}
		}
	}
	// 解析地区和语言设置
	{
		// 有存值，只存支持的版本，不存放其他值
		if ( !root[initLocale].isNull() )
		{
			mInitLocale = (Locale)root[initLocale].asInt();
		}else
		{
			mInitLocale = ResourcePathManager::getManager()->getDiviceLocale();
		}
		// 是默认值，也就是 不是可以直接支持的版本，那么统统设置为英文
		if (mInitLocale == Locale_default || mInitLocale == Locale_Ignore)
		{
			mInitLocale = Locale_en;
		}

		// 如果有设置的保存，那么就用上次的设置
		if ( !root[lastSettedLocale].isNull())
		{
			Locale l = (Locale)root[lastSettedLocale].asInt();
			ResourcePathManager::getManager()->setLocale(l);
		}
	}
	//声音开关
	{
		if (!root[backgroundSoundOpen].isNull())
		{
			bool backOn = root[backgroundSoundOpen].asBool();
			Music::sharedMusicPlayer()->setBackgroundMute(!backOn);
		}
		if (!root[effctiveSoundOpen].isNull())
		{
			bool effectOn = root[effctiveSoundOpen].asBool();
			Music::sharedMusicPlayer()->setEffectiveMute(!effectOn);
		}

		if (!root[lockScreen].isNull())
		{
			m_beffctiveSoundOpen = root[lockScreen].asBool();
			
		}
		if (!root[ServerMessage].isNull())
		{
			m_bServerMessageOpen = root[ServerMessage].asBool();
		}
	}
	return true;
}

bool GameInforEditer::saveSessonInforList()
{
	string seasonInforString = getSeasonInfoListString();
	bool sec = false;
	sec = fileUtiles->writeToDocFile(writeGameInforPath,seasonInforString);
	return sec;
}

string GameInforEditer::getSeasonInfoListString()
{
	Json::Value root;
	
	//生成isFirstRunApp的信息
	{
		root[isFirstRunApp] = Json::Value(mIsFirstRunApp);
	}
	{
		if (mNeedCommentGame && mCommentUrl != "")
		{
			root[needCommentGame] = Json::Value(mNeedCommentGame);
			root[commentUrl] = Json::Value(mCommentUrl);
		}else
		{
			root[needCommentGame] = Json::Value(false);
		}
	}

	{
		root[clientId] = Json::Value(mClientId);
	}

	{
		root[userName] = Json::Value(lastUsedName);
		root[password] = Json::Value(lastUsedPsw);
		root[hero_id] = Json::Value(lastLoginedHeroId);
	}

	// 解析地区和语言设置
	{
		root[initLocale] = Json::Value(mInitLocale);
		Locale settedLocale = ResourcePathManager::getManager()->getLocale();
		root[lastSettedLocale] = Json::Value(settedLocale);
	}

	//声音开关
	{
		root[backgroundSoundOpen] = Json::Value( !(Music::sharedMusicPlayer()->isBackgroundMute()));
		root[effctiveSoundOpen] = Json::Value( !(Music::sharedMusicPlayer()->isEffectiveMute()));
	}
	//锁屏
	{
		root[lockScreen] = Json::Value(m_bLockScreenOpen);
		root[ServerMessage] = Json::Value(m_bServerMessageOpen);
	}
	//得到配置文本
	Json::StyledWriter styled_writer;

	return styled_writer.write(root);
}

bool GameInforEditer::isFirstRunMe()
{
	return mIsFirstRunApp;
}
void GameInforEditer::firstRunOverApp()
{
	mIsFirstRunApp = false;
	saveSessonInforList();
}

void GameInforEditer::makeNew()
{
	// 可以直接读取infor_defalut.txt
	mIsFirstRunApp = true;
	saveSessonInforList();
}

bool GameInforEditer::needComments()
{
	return mNeedCommentGame;
}
string GameInforEditer::getCommentUrl()
{
	return mCommentUrl;
}
void GameInforEditer::setNeedComment(bool need,string url)
{
	mNeedCommentGame = need;
	mCommentUrl = url;

	CCLog("GameInforEditer::setNeedComment need = %s,url = %s",need?"true":"false",url.c_str());
}
void GameInforEditer::setBackgroundSoundOpen(bool isOpen)
{
	m_bBackgroundSoundOpen = isOpen;
	Music::sharedMusicPlayer()->setBackgroundMute(!isOpen);
}
void GameInforEditer::setEffctiveSoundOpen(bool isOpen)
{
	m_beffctiveSoundOpen = isOpen;
	Music::sharedMusicPlayer()->setEffectiveMute(!isOpen);
}
void GameInforEditer::setLockScreenOpen(bool isOpen)
{
	m_bLockScreenOpen = isOpen;
}
void GameInforEditer::setServerMessageOpen(bool isOpen)
{
	m_bServerMessageOpen = isOpen;
}