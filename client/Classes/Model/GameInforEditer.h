#ifndef __GAME_INFOR_EDITER_H__
#define __GAME_INFOR_EDITER_H__

#include "cocos2d.h"
#include "LFFileUtiles.h"
#include "ResourcePathManager.h"

using namespace cocos2d;

#include "include_json.h"

#include <string>
#include <list>

using namespace std;

//一个用于读写(编辑)游戏信息的类，里面存放了关于游戏的信息,比如：
//1，游戏是否第一次打开
//2，某一章（season）是否已经玩过，得分是多少
//3，某一章的哪些小节（section）是否已经玩过，得分是多少
class GameInforEditer
{
public:
	static GameInforEditer* getGameInfor();
	bool saveSessonInforList();										// 把当前的SessonInfor（包括season和season下面的section数据写回文本文件），成功返回true
	bool isFirstRunMe();											// 是否第一次启动我这个程序
	void firstRunOverApp();											// 第一次运行了程序，也就是置标志位，并保存
	void makeNew();
	bool needComments();
	string getCommentUrl();
	void setNeedComment(bool need,string url);
	int getClientId();
	void setClientId(int _id);
	string getLastUsername();
	string getLastPasseord();
	unsigned int getLastLoginedHeroId();
	void setLastUsedNameAndPsw_save(string name,string psw);
	void setLastLoginedHeroId(unsigned int _id);
	Locale getInitLocale();

	inline bool getBackgroundSoundOpen()
	{
		return m_bBackgroundSoundOpen;
	}
	inline bool getEffctiveSoundOpen()
	{
		return m_beffctiveSoundOpen;
	}
	inline bool getLockScreenOpen()
	{
		return m_bLockScreenOpen;
	}
	inline bool getServerMessageOpen()
	{
		return m_bServerMessageOpen;
	}

	void setBackgroundSoundOpen(bool isOpen);
	void setEffctiveSoundOpen(bool isOpen);
	void setLockScreenOpen(bool isOpen);
	void setServerMessageOpen(bool isOpen);
private:
	GameInforEditer();
	~GameInforEditer();
	bool initGameInfor(string fileFullPath);						// 初始化GameInfor的信息
	string getSeasonInfoListString();								//获得seasonInfor数据的字符串描述
	bool parseInforString(string str);								//解析，成功则返回true
	static GameInforEditer* gameInforEditer;
	//是不是第一次run我们只个APP，只有显示完毕Guide之后，我们才认为是第一次运行(则即使中途崩溃，下次也还是会引导我们)
	bool mIsFirstRunApp;
	bool mNeedCommentGame;			// 是否需要评论
	string mCommentUrl;				// 评论的链接
	string inforFilePath_Read;
	string inforFilePath_Write;
	LFFileUtiles* fileUtiles;
	int mClientId;

	// 上次使用的用户名
	string lastUsedName;
	// 上次使用的密码
	string lastUsedPsw;
	unsigned int lastLoginedHeroId;

	Locale mInitLocale;

	bool					m_bBackgroundSoundOpen;
	bool					m_beffctiveSoundOpen;
	bool					m_bLockScreenOpen;
	bool					m_bServerMessageOpen;
};
#endif