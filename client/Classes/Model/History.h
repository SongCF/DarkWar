#pragma once

#include <string>
#include <map>
#include <vector>
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

#define OWNER_APP		"APP"		// 默认的唯一的全局数据

#define	SCOPE_APP		0		// 应用程序级别		---> 对应的组就是SCOPE_0
#define	SCOPE_USERID	1		// 用户账号级别		---> 对应的组就是SCOPE_1
#define	SCOPE_HEROID	2		// 用户的主英雄的级别	---> 对应的组就是SCOPE_2



//用户一些共用的控制性key
#define Key_FirstJiuGongGe "FirstJiuGongGe"
#define Key_FirstFeicuiShenlin "FirstFeicuiShenlin"
#define Key_FirstMaoXianRenwu "FirstMaoXianRenwu"
#define Key_HeroIsMaoxianEnded     "HeroIsMaoxianEnded"
#define Key_HeroFirstComeIn     "HeroFirstComeIn"
#define Key_FirstDisplayJiuGongGe "FirstDisplayJGG"
#define Key_FirstBossJiuGongGe "FirstBossJiuGongGe"
#define Key_FirstStartBossJiuGongGe "FirstStartBossJiuGongGe"
#define Key_FirstComeToRenXingMianBan "FirstCoomToRenXingMianBan"
#define Key_DiaoLuoChaXunOpend  "MapsLayer_DiaoLuoChaXunOpened"
#define Key_FirstQIanghuaSelect "FirstQIanghuaSelect"
#define Key_FirstQIanghuaSelectOK "FirstQIanghuaSelectOK"
//技能书教学
#define Key_IsFirstLearnSkillBook "FirstLearnSkillBook"
//工匠
#define Key_FirstGongJiang_FenJie "First_GongJiang_FenJie"
#define Key_FirstGongJiang_HuiTui "First_GongJiang_HuiTui"
#define Key_FirstGongJiang_ChaiXie "First_GongJiang_ChaiXie"
#define Key_FirstGongJiang_DuanZao "First_GongJiang_DuanZao"
//
#define Key_FirstGongJiang_QiangHua "First_GongJiang_QiangHua"
#define Key_FirstGongJiang_BsXq "First_GongJiang_BaoShiXiangQian"
#define Key_FirstGongJiang_BsHc "First_GongJiang_BaoShiHeCheng"
//
//酒馆 6、10级新助手提示    //int 类型    =6 6级提示过了   =10 10级提示过了
#define Key_JiuGuanZhuShou "JiuGuan_ZhuShou_TiShi"
#define Key_First_TuJian_TiShi "First_TuJian_TiShi"
#define Key_First_JiuGuan "First_JiuGuan"
//英雄榜提示
#define Key_FirstOpenHeroRankList "First_OpenHeroRankList"
//第一次查看装备，看到有套装
#define Key_First_TaoZhuang "FirstLook_TaoZhuang"
#define Key_First_Chuanqi "First_Chuanqi"
#define Key_First_LookXilian "First_LookXilian"
#define Key_First_LookXilianShuoming "First_LookXilianShuoming"
#define Key_ZhuyeMenuTanchu "ZhuyeMenuTanchu"
#define Key_First_ChuanChuanqi "First_ChuanChuanqi"

#define Key_FirstJianDing     "FirstJianDing"
#define Key_FirstJingLian     "FirstJingLian"
#define Key_First_Zhuashou1    "First_Zhuashou1"
#define Key_First_Zhuashou2     "First_Zhuashou2"
enum ValueType
{
	VType_INT = 0,
	VType_BOOL,
	VType_STRING,
};

// history的一个元素
struct HistoryValueItem
{
	ValueType valueType;	// 值
	bool bValue;
	int iValue;
	string sValue;

	void putInt(int value)
	{
		valueType = VType_INT;
		iValue = value;
	}
	void putString(string value)
	{
		valueType = VType_STRING;
		sValue = value;
	}
	void putBool(bool value)
	{
		valueType = VType_BOOL;
		bValue = value;
	}

	int getInt()
	{
		CCAssert(valueType == VType_INT,"");
		return iValue;
	}
	bool getBool()
	{
		CCAssert(valueType == VType_BOOL,"");
		return bValue;
	}
	string getString()
	{
		CCAssert(valueType == VType_STRING,"");
		return sValue;
	}
};

// history_list的一个元素
struct OwnerHistory
{
	string owner;			// 所有者
	// key到value的映射
	map<string,HistoryValueItem> ownerHistory;
};

struct OneScope
{
	int scope;
	vector<OwnerHistory> owners;
};

/**
 * 历史数据的保存文件
 * 文件可分为N个级别，通常有{SCOPE_APP,SCOPE_USERID,SCOPE_HEROID}，级别越小那么适配度就越广，级别越大，适配度就越小。
 *
 * 例如，存放如下键值对:success1_2:true
 * 作为用户[camellia],在SCOPE_APP中存放这个值，那么这个APP上的其他用户取success1_2也会取到
 * 如果scope是SCOPE_USERID，那么只能是这个用户可以取到，如果scope是SCOPE_HEROID只能是指定的英雄登陆才取的到
 *
 * 存放步奏，直接addHistory即可
 * 取值的步奏，从适配度越小的地方开始找，找到就返回，没找到就找大一点的适配度，直到找到或者不存在就返回。
 * 当然也可以直接指定只找某个scope的，或者某个适配度及其以上的
 *
 * <<<目前还不支持，如果在大的scope没找到，就去小的scope中寻找。>>>
 */
class History
{
private:
	History(void);

	// 存放的历史数据
	// 由N多scope组成
	vector<OneScope> histotys;

	static History* instance;

	// 通过一个json的字符串初始化
	void initFromJsonString(string jsonDoc);
	// 保存到本地
	void save(string jsonDoc);

	string genJsonString();
public:
	static History* getHistory();

	void init();
	void save();

	// 存放一个key-value对，指定一个scope和所有者
	// 通常用的scope有SCOPE_APP，SCOPE_USERID，SCOPE_HEROID
	// 当然也可以自己定义其他的scope，scope越小，适配度就越大，搜寻顺序就越靠后，目前scope都没有用
	// owner：所有者，scope中应该全局唯一
	void put(string key,HistoryValueItem value,string owner,int scope);
	void put(string key,int value,string owner,int scope);
	void put(string key,bool value,string owner,int scope);
	void put(string key,string value,string owner,int scope);

	// 放到APP级别,ower就是默认的OWNER_APP
	void putInApp(string key,int value);
	void putInApp(string key,bool value);
	void putInApp(string key,string value);
	// 放到用户级别，需要传入用户的id
	void putInUser(string key,int value,string userId);
	void putInUser(string key,bool value,string userId);
	void putInUser(string key,string value,string userId);

	// 存放到当前的用户里面
	void putInCurrentUser(string key,int value);
	void putInCurrentUser(string key,bool value);
	void putInCurrentUser(string key,string value);


	// 放到主英雄级别，需要传入英雄的id
	void putInHero(string key,int value,int mainHeroId);
	void putInHero(string key,bool value,int mainHeroId);
	void putInHero(string key,string value,int mainHeroId);

	// 存放到当前的英雄里面
	void putInCurrentHero(string key,int value);
	void putInCurrentHero(string key,bool value);
	void putInCurrentHero(string key,string value);

	// 获取历史数据，只寻找指定的scope里面的数据
	bool getValue(string key,string owner,int scope,HistoryValueItem& value);
	bool getInt(string key,string owner,int scope,int& value);
	bool getBool(string key,string owner,int scope,bool& value);
	bool getString(string key,string owner,int scope,string& value);

	// 从app中获取
	bool getIntFromApp(string key,int& value);
	bool getBoolFromApp(string key,bool& value);
	bool getStringFromApp(string key,string& value);

	// 从User中获取
	bool getIntFromUser(string key,string userId,int& value);
	bool getBoolFromUser(string key,string userId,bool& value);
	bool getStringFromUser(string key,string userId,string& value);

	// 根据当前登录的用户获取数据
	bool getIntFromCurrentUser(string key,int& value);
	bool getBoolFromCurrentUser(string key,bool& value);
	bool getStringFromCurrentUser(string key,string& value);

	// 从HeroId中获取
	bool getIntFromHero(string key,int heroId,int& value);
	bool getBoolFromHero(string key,int heroId,bool& value);
	bool getStringFromHero(string key,int heroId,string& value);

	// 根据当前登录的英雄获取数据
	bool getIntFromCurrentHero(string key,int& value);
	bool getBoolFromCurrentHero(string key,bool& value);
	bool getStringFromCurrentHero(string key,string& value);

	~History(void);
};

