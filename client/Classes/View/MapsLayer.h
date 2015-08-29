//@ maps layer
//@ used to show maps to player
//@ by _Richard



#pragma once
#include "BaseLayer.h"
#include "cocos-ext.h"
#include "IMainLayerBody.h"
#include "FightPrepare.h"
#include "ChatLayer.h"
/*#include "AwardDisplay.h"*/
#include "FightBagLayer.h"
#include "flat.h"
#include "GetTiLiLayer.h"
#include "EquipmentTaoZhuangXiangQing.h"
#include "LevelUpLayer.h"
#include "ShouHuZhiLingManager.h"
#include "FXScrollMenu.h"
#include "ZhuShouChatLayer.h"
#include "AddFriendUI.h"
#include "AddFriendWarnLayer.h"
#define Task_NUM 10

enum TaskDifficult
{
	difficult_normal = 0,
	difficult_hard   = 1,
};

enum GoTo_UI_Type
{
	Default_UI_Type = 0,					//默认界面
	GoTo_Shouhuzhiling_UI_Type,				//直接跳转到守护之灵界面
	GoTo_LevelTiaozhen_UI_Type,				//直接跳转到关卡挑战开始界面
	GoTo_BossTiaozhen_UI_Type				//直接跳转到关卡挑战Boss开始界面
};

/*
**此类用于  点击冒险后 选择地图 或进入任务 锁看到的地图界面
*/
class MapsLayer : public BaseLayer,public IMainLayerBody,ZhuShouChatLayerCallBack
	, public FightPrepareCallback
	, public ChatLayerCallBack
	//, public AwardDisplayCallBack_Interface
	, public BagCallBack
	, public GetTiLiCallBack
	, public TaoZhuangLayerInterface
	, public LevelUpEndCallBackInterface
	, public CCTableViewDataSource
{
public:
	MapsLayer(GoTo_UI_Type ui_type = Default_UI_Type);
	~MapsLayer(void);
	void schedule_initLayer(float dt);
	int  getMainTaskId(){return (int)mMainId;}

	//bag call back     //扫荡完了之后看背包
	virtual void disableAllTouch();
	virtual void enableAllTouch();

public:
	void setWillGoToLevel(int iWillGoToLevel){m_iWillGoToLevel = iWillGoToLevel;};

protected:
	//刷新地图UI
	void drawRenWu();
	void CenterMainTask(TaskId id, TaskStatus state);
	void resetZhangJie_AddRefreshUI(int zj); // zj == 0 世界地图
	void initCacheMapByIndx(int zj);
	CCPoint getTaskMenuPos(int zj, int idx);

	bool unblockNewZone_TaskEnd();  //有新解锁的 则返回true

	void menuItem_Clicked_HeadMenu(CCObject *pSender);

	//npc board
	void menuItemClicked_NewTask(CCObject* pSender); //最新任务  （需要先get）
	void menuItemClicked_Task(CCObject* pSender);
	void menuItemClicked_NpcDetailBack(CCObject* psender);
	void menuItemClicked_BeginFight(CCObject* pSender);
	void menuItemClicked_GetAward(CCObject *pSender);
	void menuItemClicked_ShouHuZhiLing(CCObject *pSender);
	void menuItemClicked_JiangJieLingJiangFanHui(CCObject *pSender);
	void menuItemClicked_SaoDang(CCObject *pSender);
	//扫荡board
	void menuItemSaoDang_item(CCObject *pSender);
	void menuItemClicked_SaoDangEnd(CCObject *pSender);
	void rsp_SaoDangEnd(CCObject *msg_f);
	//章节
	void unblock_diaoluoChaXun();
	void menuItemClicked_ZhangJie_DiaoLuo(CCObject *pSender);
	void menuItemClicked_zhangjie(CCObject *pSender);
	void schedule_resetChapter();
	void menuItemClicked_MainMenu(CCObject *pSender);

	void menuItemClicked_NpcEquipLoot(CCObject *pSender);
	void menuItemDiaoLuo_Item(CCObject *pSender);
	void menuItemClicked_equipDetailMenu(CCObject *pSender);
//	void menuItemClicked_taoZhuangFanhui(CCObject *pSender);

	void rsp_QueyNormal_HardTask(CCObject *msg);
	void rsp_ChangeTaskState(CCObject* obj);
	void rsp_FlushTaskList(CCObject* obj);  //重新 getTaskList

	void rsp_QueryEquipLoot(CCObject *msg_f);
	void rsp_QueryEquipLoot_HardDificult(CCObject *msg_f);
	void rsp_QueryTaskRate(CCObject *msg_f); //查询关卡通关星级 每一次normal getTaskList都应该调用
	void rsp_QueryRateTaskBonus(CCObject *msg_f); 
	void rsp_QueryRateChapterBonus(CCObject *msg_f); 
	void rsp_GetedTaskBonus(CCObject *msg);
	void rsp_GetedChapterBonus(CCObject *msg);

	void rsp_FightBeginNormal(CCObject* obj);
	void rsp_FightBeginHard(CCObject *msg_f);


	void section5EndAnimation(CCNode *node);
	void section5EndAnimationEnd_ToFlushTaskState();


	void rsp_QueryShouHuZhiLing(CCObject *msg);
	void rsp_ShouHuZhiLing_TiaoZhan(CCObject *msg_f);
	void rsp_ShouHuZhiLing_MoBai(CCObject *msg);
	void rsp_ShouHuZhiLing_LingJiang(CCObject *msg);
	void rsp_SHZLHero(CCObject *msg);
	void rsp_queryYuanShen(CCObject *msg);

	void callfuncND_ChangeMenuPriority(CCNode *node, void *priority);
		
	void showTongguanJiangliUI();

	inline void teachEnd()
	{
		m_bIsTeachIng = false;
	}

	virtual void ZhuShouchatEndCallBack();
	void addFriendCallBack(CCNode *node);
	inline string getNewFirendName(unsigned int id)
	{
		vector<BaseActor *> list = MyselfManager::getManager()->getTeamActors_ZhenShen();
		for (vector<BaseActor *>::iterator iter = list.begin(); iter != list.end(); iter ++)
		{
			if ((*iter)->getCurrentProperty().profession == id)
			{
				return (*iter)->getCurrentProperty().nameInChinese;
			}
		}
		return "";
	}


	/////////////////////////////////////////////////////////////////////////////
	// CCScrollViewDelegate
	virtual void scrollViewDidScroll(CCScrollView* view){};
	virtual void scrollViewDidZoom(CCScrollView* view){};
	// CCTableViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell){};
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell){};
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell){};
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell){};
	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);
	///////////////////////////////////////////////////////////////////////////////

public:
///////////////////////////////////////////////////////////////
	//fight prepare call back
	// 失败，返回一个Code，就是协议定义的SimpleProtocolErrorCode
	virtual void prepareError(int code);
	// 提供背景
	virtual CCSprite* getBg(int taskId,int gridIndex);
	virtual void prepareSuccess();
////////////////////////////////////////////////////////////////
	// chat end callback interface
	virtual void chatEndCallBack();
	//
	virtual void getTiliCallBackFunc();
	//
	virtual void LevelUpEndCallBackFunc();
/////////////////////////////////////////////////////////////////
	virtual void disableAllTouchBegin();
	virtual void disableAllTouchEnd();
	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return mBg->getPosition();}
	////////////////////////////////////////////////////////////////
	//套装返回 回调
	virtual void callbackInterface_TaoZhuangLayer();
	/////////////////////////////////////////////////////////////////////////////////////////////////////

private:
	bool m_bDisplayWindow_CannotTouch;

	bool m_bMultTouch;
	CCPoint mBeginPos;
	CCPoint mMoveEndPos;

	bool m_canotMultTouch;

	bool m_bMultTouchBeginHasInit;
	CCPoint mMultTouchBeginPos[2];
	CCPoint mMultTouchEndPos[2];

	//需要跳转的关卡数(1-40)
	int m_iWillGoToLevel;
	//保存通关奖励的返回数据
//	Flat_QueryRateTaskBonusRsp *m_data;
	bool m_bClickTongguanBtn;
	CCMenuItem *m_fightItem[Task_NUM];
public:
	//move 的时候是多个点 则 是放大缩小
	//move 的时候是单个点 且 是左右滑动 且 显示的章节地图 ze 左右切换章节地图
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);

	void callfunc_mapsScrollEnd();

	virtual void onEnter();
	virtual void onExit();

	void teachGuide();
	void updateTongGuanJiangliInfo();
	void updateLevelJiangliInfo(CCMenuItem* pItem);
	void setShouhuzhilingMenuEnable(bool e);
private:
	bool		m_bIsTeachIng;
	//UI bg
	BaseSprite *mBg;
	CCMenu *mHeadMenu;
	BaseSprite *mTitleBar;
	CCMenu *mMainMenu;
	//maps
	CCSprite* mMap;
	CCSprite *mMapCache;
	bool m_mapCacheHasInit;

	FXScrollMenu * mRenwuMenu; //1-10
	CCArray *mNameArray;


	//扫荡
	bool mAuto_FenJie[3];
	int mSaoDangNum;
	Flat_SaodangRsp mSaoDangData;

	//npc对话框 掉落。
	Flat_QueryFubenDiaoluoRsp mDiaoLuoRsp;
	Flat_FubenDiaoluoZBInfo mCurTaoZhuang_Info;  //方便 套装查看， 当改装备是套装时才用， 在显示装备详情时赋值、 套装查看时使用

	//npc 对话
	int mChatSection; //接新任务1、刚开始一个任务2、完成一个任务5，都需要显示npc对话
	bool mSection5TranslationMap;
	bool mTongGuanAwardWindowEnd;


	CCObject* m_task_menuItem;// 记录点击地图上的那个按钮
	FightPrepare *mFight;     // 战斗脚本


////////////////////////////////////////////////////////////////////////////////
	//重要数据
	//////////////////////////////////////////////////////////////////////////
	static int mCurZhangjie;

	static int mCurDifficulty;  //0 normal   1 hard
	// 记录当前开启的最高任务
	TaskId mMainId;
	TaskStatus mMainState;

	//关卡评级 //章节评级   (刷新地图的时候，就要获取) 1.query    2. flush
	vector<RateStarInfo> mTaskRate;	
	vector<RateStarInfo> mChapterRate;
//////////////////////////////////////////////////////////////////////////////////

protected:
	SHZHL mCurSHZLData;

	//查询角色详细面板时使用。
	int mNum_YuanShen;
	CCArray *mQueryHeroActors;
	CCArray *mQueryYuanShen; //actor在setYuanshen时没有retain，所以要自己保存并释放（和equipment一样）
	vector<Equipment*> mQueryActorsEquip; //
	GoTo_UI_Type	m_GoTo_UI_Type;
};