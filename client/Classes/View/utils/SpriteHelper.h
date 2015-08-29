/**
*	一个精灵的帮助类
*
*
**/
#ifndef __SPRITE_HELPER_H__
#define __SPRITE_HELPER_H__

#include <string>
#include "cocos2d.h"
#include <cmath>
#include "IBorder.h"

//#define FRONT "arial"
//#define FRONT "AdobeHebrew-Italic"
//#define FRONT "KozGoPr6N-ExtraLight"
#define FRONT "AdobeGothicStd-Bold"

using namespace cocos2d;
using namespace std;
class SpriteHelper
{
public :
	//只能拼接1个数据和1个字符串
	static string splicingNum_String(const char* format,int num);

	static void pauseNodeAndChildren(CCNode* node);		//暂停Node和他的孩子
	static void pauseSubNodeAndChildrenInParentNodeByTag(CCNode* parent,int tag);
	static void resumeNodeAndChildren(CCNode* node);		//暂停Node和他的孩子
	static void resumeSubNodeAndChildrenInParentNodeByTag(CCNode* parent,int tag);

	//让sprite重复播放fileNames指定的序列动画
	static CCAction* playFrameAction(CCSprite* sprite,std::string fileNames[],int length,float delay,bool isRepeat = true);
	static void playFrameAction(CCSprite* sprite,const char* pList,const char* imageFileName,std::string fileNames[],int length,float delay);
	
	static CCAction* createAction(std::string fileNames[],int length,float delay,bool isRepeat = true);

	//通过纹理图册的方式创建动画，调用这个函数之前需要确保fileNames需要的图片所对应的pList文件已经加入到内存了
	//加入方法：CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(pList);
	//如果有多个相同图片的动画需要显示，那么可以用BachNode封装处理：
	//方法：CCSpriteBatchNode* topBatch = CCSpriteBatchNode::create("动画图册的一个大图片");
	//		layer->addChild(topBatch);
	//要做动画的物体必须要用pList的图片初始化一下才可以用：
	//		initWithSpriteFrameName("airplane_morion_behind_1.png");;
	//
	static CCFiniteTimeAction* createAction_pList(std::string fileNames[],int length,float delay,bool isRepeat = true);


	//创建一个可以回调的Action：
	//createAction(	CCBlink::create(2.0f,3),
	//				SEL_CallFuncO(&SectionGuide::bombClickPromptFinishedShow),
	//				this);
	//这种SEL_CallFuncO的回调线程应该是没有在调用线程里面，所以不能直接访问东西，而需要通过param来访问
	static CCAction* createAction(CCFiniteTimeAction* action,SEL_CallFuncO finishCallBack, CCObject* param);
    
	//回调的应该就在原始线程里面，可以直接使用
	static CCFiniteTimeAction* createAction(CCFiniteTimeAction* action,CCObject* target,SEL_CallFuncN finishCallBack);

	//根据format生成动画 ,例如"shark_%d.png",2那么就是从shark_2.png开始,一共totalNum多个
	static CCFiniteTimeAction* easyCreateAction(const char * format,int startNum,int totalNum,
		float delay,bool repeat = false);

	//通过纹理图册的方式创建动画，调用这个函数之前需要确保fileNames需要的图片所对应的pList文件已经加入到内存了
	//加入方法：CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(pList);
	//如果有多个相同图片的动画需要显示，那么可以用BachNode封装处理：
	//方法：CCSpriteBatchNode* topBatch = CCSpriteBatchNode::create("动画图册的一个大图片");
	//		layer->addChild(topBatch);
	//
	static CCFiniteTimeAction* easyCreateAction_pList(const char * format,int startNum,int totalNum,
		float delay,bool repeat = false);

	//根据format生成动画 ,例如"shark_%d.png",2那么就是从shark_2.png开始,一共totalNum多个
	static CCFiniteTimeAction* easyCreateAction(const char * format,int startNum,int totalNum,
		float delay,CCObject* param,SEL_CallFuncO callBack);

	//推荐使用这种
	static CCFiniteTimeAction* easyCreateAction(const char * format,int startNum,int totalNum,
		float delay,CCObject* target,SEL_CallFuncN callBack);

	//推荐使用这种
	static CCFiniteTimeAction* easyCreateAction_pList(const char * format,int startNum,int totalNum,
		float delay,CCObject* target,SEL_CallFuncN callBack);
    

	static CCFiniteTimeAction* createFadeBetweenAction(int start,int end,float timeCost,int repeatTimes);
	static CCFiniteTimeAction* createFadeBetweenAction(int start,int end,float timeCost,float delay ,int repeatTimes);
    //边移动，边缩放,可以设置动作前的等待时间，如果小于等于0,那么就不等待
    static void moveWithScale(CCNode* target,CCPoint endPos,float endScale,float time,float delayBefore = 0.0f);
    
	//看指定的两个框，是否相交;rect 的中心就是角色的位置，size就是角色的大小;
	static bool isCross(CCRect rect1,CCRect rect2);

	//看两个实现IBorder的物体是否相交;
	static bool isCross(IBorder* border1,CCNode* parent1,IBorder* border2,CCNode* parent2);
    
    //判断与border在同一个坐标系的坐标点是否在border里
    static bool pointInBorder(IBorder* border,CCPoint borderPoint);
    
	//判断2个Sprite是否相交(即碰撞)，前提是锚点在中心
	//碰撞了就返回true
	static bool isCross(CCSprite* sprite1,CCSprite* sprite2);

	//获得target如果要放到屏幕的左下角，那么该放置的位置
	static CCPoint getLeftBottomPos(CCNode* target,CCSize screenSize,float padingLeft,float padingBottom);


	static CCLabelTTF* createLabelTTF(const char* lable,CCNode* toAdd = NULL,float size = 40);
	static CCLabelTTF* createLabelTTF(int lable,CCNode* toAdd = NULL,float size = 40);

	//每次字符串被替换后，再在替换过的字符串中进行替换，是第一种，否则是第二种。
	static string& replace_all(string& str,const string& old_value,const string& new_value)   
	{   
		while(true)   {   
			string::size_type   pos(0);   
			if(   (pos=str.find(old_value))!=string::npos   )   
				str.replace(pos,old_value.length(),new_value);   
			else   break;   
		}   
		return   str;   
	}   
	static string& replace_all_distinct(string&  str,const string& old_value,const   string&   new_value)   
	{   
		for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {   
			if(   (pos=str.find(old_value,pos))!=string::npos   )   
				str.replace(pos,old_value.length(),new_value);   
			else   break;   
		}   
		return   str;   
	}   

	static long millisecondNow();

};

#endif