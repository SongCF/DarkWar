#pragma once

#include "cocos2d.h"
#include "LightMaskFactory.h"
#include "font.h"
#include "BaseLayer.h"
#include "MainLayerBase.h"
#include "MyselfManager.h"
#include "FX_CommonFunc.h"

#define LABLE_GAP_WITH_SCREEN 10

using namespace std;
using namespace FX_CommonFunc;
USING_NS_CC;

#define LightMask_Opacity 180


enum 
{
	Tag_LightBg = 1,
};

struct LightMaskPos
{
	CCSize size;
	CCPoint pos;
	string info;
};


extern bool isUI_Guide_MainLine;
extern bool isUI_Guide_LearnSkill;
extern bool isUI_Guide_GongJiang;


//第一次通关，主页配装教程
extern bool g_firstTaskEnd_to_changeEquip;
//教学的时候 第四关刚结束 就要直接跳到主页 工匠强化   此时任务完成还未提交。
extern bool g_gongjiang_QhHt_Opend;
extern bool g_gongjiang_FenJie_Guide;
extern bool g_gongjiang_DuanZao_Opend;
extern bool g_gongjiang_BsXq_Opend;
extern bool g_gongjiang_BsHc_Opend;
//
extern bool g_ChuanQiFuBen_Opend;  extern bool g_ChuanQiFuBen_Guide;
extern bool g_ChuanQiLu_Opend;   extern bool g_ChuanQiLu_Guide;
extern bool g_ChuanQiSuiPianHeCheng_Guide;  extern bool g_ChuanQiSuiPianHeCheng_GuideEnd;
extern bool g_FirstChuanQiEquipLook;
extern bool g_FirstTaoZhuangLook;
extern bool g_YingXiongBang_Opend;
extern bool g_JingJiChang_Opend;
extern bool g_XueSeJiaoTang_Opend;
extern bool g_JiuGuan_Opend;
extern bool g_JiXianShouCheng_Opend;
extern bool g_DianFengDuiJue_Opend;
//学技能书教学
extern bool g_firstLearnSkillBook;




class GameGuideManager
{
public:
	//GuideProcess 对应_guideProcess的位置
	//如果GID_OpeningStory(值为0)教学已经完成   那么_guideProcess的第0(GID_OpeningStory)个bit就为1。
	//详见方法：void setOneGuideEnd(GuideProcess idx)
	enum GuideProcess
	{
		GID_OpeningStory = 0,
		GID_FirstTaskEnd_ChangeEquip,//1 第一次通关，主页配装教程

		GID_FirstComeInGame,//2 第一次进入游戏
		GID_FirstMaoXian,//3 第一次进行冒险
		GID_FirstJinXingTongGuan,//4 第一次金星通关
		GID_RenWuShuXing,//5 人物详细属性
		GID_GongMingLevel,//6 共鸣教学
		GID_FirstChuanQi,//7 传奇装教学
		GID_FirstChuanQiShiPin,//8 传奇装教学饰品
		GID_ChangeZhuangBei,//9 换装教学

		GID_NewFriendAddOne,// 10 第一次添加好友教学
		GID_NewFriendAddTwo,//11 第二次好友教学教学

		GID_FirstLearnSkillBook,// 12 第一次学习技能教学

		GID_FenJieUnLock,//13 分解解锁动画
		GID_First7DayLogin,//14 第一次领取七日奖励

		GID_FirstDisplayChuanQiEquip,//15 第一次展现传奇装备
		GID_FirstLockXiLian,//16 第一次看见洗炼
		GID_FirstDisplayChuanQiEquipLevel,//17 第一次展现传奇装备升级

		GID_FirstXunBaoTeach,//18 寻宝功能解锁

		GID_FirstYingXiongBang,//19 英雄榜功能解锁

		GID_FirstXunBaoTeachPlay,//20 寻宝功能解锁动画

		GID_FirstYingXiongBangPlay,//21 英雄榜功能解锁

		GID_FirstJingJiChang,//22 竞技场功能解锁
		GID_FirstJingJiChangPlay,//23 竞技场功能解锁

		GID_FirstJiuGuan,//24 酒馆功能解锁
		GID_FirstJiuGuanPlay,//25 酒馆功能解锁动画
		GID_FirstJiuGuanIn,//26 酒馆内部动画
		GID_FirstBuZhen,//27 布阵教学

		GID_FirstXueSeJiaoTang,//28 血色教堂

		GID_FirstXueSeJiaoTangPlay,//29 血色教堂

		GID_GetFirstBaoShi,//30 获得了第一颗宝石

		GID_GongJiang_QhHt,//31 强化事件发生

		GID_GongJiang_QiangHua,//32 工匠强化教学
		GID_GongJiang_FenJie,

		GID_GongJiang_HuiTui,//回退教学
		GID_GongJiang_HuiTuiCancel,//回退教学的返回

		GID_GongJiang_SelectZhuangBei,//选择装备
		GID_GongJiang_SelectZhuangBeiOK,//选择装备确定
		GID_GongJiang_ZhuangBeiJianDing,//装备鉴定
		GID_GongJiang_JingLianZhuanKe,//装备鉴定

		GID_GongJiang_DuanZao,
		GID_GongJiang_BsXq,
		GID_GongJiang_BsHc,

		GID_ChuanQiFuBen,
		GID_ChuanQiLu,
		GID_SuiPianHeCheng,
		GID_FirstChuanQiEquipLooked,
		GID_FirstTaoZhuangLooked,
		//GID_FirstLearnSkillBook,

		GID_YingXiangBangOpen,
		GID_JiangJiChangOpen,
		GID_XueSeJiangTangOpen,
		GID_JiuGuanOpen,
		GID_JiXianShouChengOpen,
	};
private:
	//教学的总进度，记录哪些地方已经教学过了。
	unsigned long long _guideProcess; 
	//_guideProcess的一份拷贝，当一个新教学开启的时候，_guideProTemp对应位置置1，而_guideProcess还是0
	unsigned long long _guideProTemp; 

	//当前正在教学的进度、进行到哪一步了 ，恒定刚开始为 1
	//比如工匠强化教学分为很多步（开始、选装、保留提示、强化完成、回退提示......），
	//这么多步进行到哪一步了就用_currentGuideState来记录，具体哪一步什么值由UI自己定义。
	//（只是为了方便ui层教学逻辑、数据层不关心。数据层只关心当前这个教学是否完成true or false）
	int _currentGuideState;

	static GameGuideManager * _guideManager;
	GameGuideManager();
public:
	static GameGuideManager * getManager();
	void setGuideProcess(unsigned long long); //仅数据层可调用一次（从服务器获取进度时初始化数据），之后不允许再次调用。
	unsigned long long getGuideProcess();

	void setOneGuideStart(GuideProcess idx);//（升级）触发教学后，调用该接口，让指定的教学开始
	void setOneGuideEnd(GuideProcess idx);//指定教学完成后，调用该接口，通知数据中心更改状态。
	bool isGuideNow(GuideProcess idx); //判断某个教学是否正在进行中
	bool isAlreadyGuide(GuideProcess idx); //判断某个教学是否已经完成了

	//UI专用，方便UI层记录当前这个教学进行到哪一步了。
	int getCurGuideState();
	void setCurGuideState(int state);
};








class GuideClickedCallback
{
public:
	virtual void guide_clickedCallback() = 0;
};

class GuideLayerBase : public CCLayer
{
public:
	GuideLayerBase()
	{
		this->autorelease();      //很多地方都是new出来的  没有release, 干脆直接在这里autorelease算了

		CCLayer::init();

		mMask = new CCSprite;
		mMask->initWithFile("zhandou_jiugongge_touming.png");

		mNextCallback = NULL;

		mVisibleSize = CCDirector::sharedDirector()->getVisibleSize();
		mWinSize = CCDirector::sharedDirector()->getWinSize();

		m_oTarger = NULL;
	}
	virtual ~GuideLayerBase()
	{
		CC_SAFE_RELEASE(mMask);
		CC_SAFE_RELEASE(mNextCallback);
	}

	virtual void init(vector<LightMaskPos> lightPos, GuideLayerBase* nextLayer, bool swallowAllTouches) = 0;
	virtual void init(CCSize size, CCPoint pos, string infoStr, GuideLayerBase* nextLayer, bool swallowAllTouches);

	CCSprite* createOneLightNode(CCSize size,CCPoint pos = CCPointZero);
	CCSprite* createLightBg(vector<LightMaskPos> lightPos);
	CCSprite* createDownArrow();
	// 创建一个亮的边框
	CCNode* createLightFrame(CCSize size,CCPoint worldGlPos);
	CCNode* createLabel(string text, float labelWidth = 0, ccColor3B color = ccWHITE, float fSize = 25.0f);

	void autoVerticalLayoutGuideInfor(CCNode* lable,CCNode* downArrow,CCPoint lightNodePos,CCSize lightNodeSize);


	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){return true;}
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){}
	virtual inline void setChatEndBack(CCObject *obj,SEL_CallFunc handler)
	{
		m_oTarger = obj;
		m_hHandler = handler;
	}
protected:
	bool mSwallowAllTouches;
	GuideLayerBase *mNextCallback;

	CCSprite *mMask;
	CCSize mVisibleSize;
	CCSize mWinSize;

	CCObject *m_oTarger;
	SEL_CallFunc m_hHandler;
};



//////////////////////////////////////////////////////////////////////////
class GuideLayer_SimpleLayer : public GuideLayerBase
{
public:
	virtual void init(vector<LightMaskPos> lightPos, GuideLayerBase* nextLayer, bool swallowAllTouches);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

protected:
	CCPoint mClickedPos;
	CCSize mClickedSize;

	CCTouch *m_pTouch;
	virtual void removeSelfAndDispatchTouchEnd(float dt);//when self touch end
};



//////////////////////////////////////////////////////////////////////////
class GuideLayer_SimpleJumpLayer : public GuideLayer_SimpleLayer
{
public:
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
};



//////////////////////////////////////////////////////////////////////////
class GuideLayer_JiuGongGeSpecial : public GuideLayerBase
{
public:
	virtual void init(vector<LightMaskPos> lightPos, GuideLayerBase* nextLayer, bool swallowAllTouches);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

protected:
	CCPoint mClickedPos[2];
	CCSize mClickedSize[2];
};



//////////////////////////////////////////////////////////////////////////
class GuideLayer_RenXingMianBan_Special : public GuideLayerBase
{
public:
	virtual void init(vector<LightMaskPos> lightPos, GuideLayerBase* nextLayer, bool swallowAllTouches){}
	virtual void init(CCSize size, CCPoint pos, vector<string> infoStrs, GuideLayerBase* nextLayer, bool swallowAllTouches);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

protected:
	CCPoint mClickedPos;
	CCSize mClickedSize;
};




//////////////////////////////////////////////////////////////////////////
class GuideLayer_MultLightAndLabel : public GuideLayerBase
{
public:
	virtual void init(vector<LightMaskPos> lightPos, GuideLayerBase* nextLayer, bool swallowAllTouches);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	void autoCenterLayoutGuideInfor(CCNode* lable,CCNode* downArrow,CCPoint lightNodePos,CCSize lightNodeSize);

protected:
	vector<LightMaskPos> mLightPos;
};

