// a singleton 
// manager JiuGongGe translation maps
// by _Richard

#pragma once

#include "cocos2d.h"
#include "TaskManager.h"
#include <map>
#include <vector>
#include <string>
using std::vector;
using std::map;
using std::string;

USING_NS_CC;

typedef struct JGGMapsStruct
{
	string background;
	string str0;  //左上
	string str1;  //左下
	string str2;  //右下
	string str3;  //右上

}JGGMapsStruct;

class MapsManager : public CCObject
{
public:
	//返回的sprite已经匹配屏幕
	CCSprite* getBackGroundMapImage(int taskId /*,int gridIndex = 0*/);   // 新new一个战斗背景图
	CCSprite* getXueSeJiaoTangMapImage(int grid);
	CCSprite* getJingJiChangMapImage(int idx = 0); //无效参数
	CCSprite* getTuanDuiFuBenMapImage(int idx = 0); //无用参数
	CCSprite* getShouChengFuBenMapImage(int idx = 0); //无用参数

	static MapsManager* getManager();

	~MapsManager();
protected:
	MapsManager();


	map<int, CCArray*> mMap;
	map<int, vector<JGGMapsStruct> > mJGGMaps;

	static MapsManager* s_mManager;
};

