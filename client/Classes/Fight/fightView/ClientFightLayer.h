#pragma once

#include "BaseLayer.h"
#include "FightDirector.h"
#include "ClientChessBoard.h"
#include "ClientSkillProcessor.h"
#include "Bundle.h"
#include "ChatLayer.h"

#define TYPE_ITEM				12334	// 得到的东西的类型，这是物品
#define TYPE_EQUIP				12335	// 得到的东西的类型，这是装备
#define TYPE_ITEM_SUIPIAN		12336	// 得到的东西的类型，这是item碎片
#define TYPE_SHENBING_SUIPIAN	12337	// 得到的东西类型，这是神兵碎片
#define TYPE_PPT				12338	// 得到的东西类型，这是能量点
// 战斗的奖励
struct FightReward
{
	int deadUniqueId;						// 哪一个角色dead的时候
	int dirIndex;							// 第几个director
	int rewardType;							// TYPE_ITEM/TYPE_EQUIP/TYPE_ITEM_SUIPIAN/TYPE_SHENBING_SUIPIAN
	ItemInfo item;							// 会掉的物品
	EquipData equip;
	ItemSuipianInfo itemSuiPian;
	ShenbingSuipianInfo shenBingSuiPian;
	PowerPoint pptData;
};

class FightTrigleDriver;
class ZhuangTaiBar;

enum FightChatTag
{
	Chat_NO				= -1,	// 没有
	Chat_WhenHeroOK		= 0,	// 英雄站好了，怪物还没出来
	Chat_WhenGuaiWuOK,			// 怪物刚出来
	Chat_OneFightEnd,			// 打完一场了，还有下一场的情况
	Chat_WhenFightEnd,			// 已经打完了
};
// 战斗界面
class ClientFightLayer : public BaseLayer,ChatLayerCallBack
{
private:
	int currentDirIndex;
	int mCurrentStepIndex;
	Bundle mBundle;
	vector<FightDirector> mDirvec;
	FightTrigleDriver* mTrigle;
//	CCLabelTTF* huiHeLable;
	CCSprite* mBg;
	bool mPauseFightTrigle;
	FightChatTag mChatTag;
	bool fightEnded;
	vector<FightReward> rewardVec;
	int mHammerIdx;//工匠铁锤在rewardVec中的位置 初始化-1
	int mSurvivalSpriteNum;//初始化该关的怪物数
	//HeroBaseProperty mBeForedata;
	HeroBaseProperty oldArmy[4];
	HeroBaseProperty nowArmy[4];
	//HeroBaseProperty mNowData;

	ClientChessBoard* boardTop;
	ClientChessBoard* boardButom;
	//CCSprite* mFightBar;
	ClientSkillProcessor* mSkillProc;

	//boss单独血条
	BaseSprite *mboosBloodBar;
	CCProgressTimer *mBossBloodProgress;
	CCLabelTTF *mBossBloodName;
	int mBossHP;				// 血量，当前的血值
	int mDefaultHP;			// 满血值,对于服务器来说是没有用的
	void addBossHP(int toAdd, float delayTime=0.0f);

	// 测试用的
	CCMenuItemFont* itemNext;

	// 从A/B棋盘里面寻找Id为这个的
	ClientChessBoardItem* getChessBoardItem(int unique_Id);
	int getAlivedItemNumByOriginBoard(ClientChessBoard* board);
	void removeSomeBuff(vector<int> buff_id_arr);
	void doBuff(BuffInStep buff);

	// 下一步
	void debugNextStepClicked(CCObject* sender);
	void debugGotoDirctor(CCObject* sender);


	const char* getDebugStr();

	void menuItemClicked(CCObject* obj);

	// 初始化
	void initWithDirctorVecAndBg(vector<FightDirector> dir_vec,CCSprite* bg);
	// 用某个导演来表演
	void playWithDirctor(int dirIndex);
	// 对话完毕的回调
	void chatEndCallBack();
	void ChapterIllustrationEndCallBack();
	// 敌人从天而降
	void actorFallDownFromSky(ClientChessBoard* board);

	void fu_huo_action(BaseSprite* container,string pic);
	void nodeHitGround(CCNode* node);
	void screenShake();
	void skill_screenShake();
	void actionEnd(float t); // 角色从天而降的动做完毕
	// 开始
	void beginFightTrigle();
	// 战斗的教学完毕
	void fightGuideEnd(CCObject* guideLayer);

	void jumpItemClicked(CCObject* null_obj);
	// 可以跳的通知
	void canJumpNotified(CCObject* null_obj);

	// 下一场
	void timeToNextFight(float t);

	// 战斗结束的对话
	void beginFightEndChat(float t);

	// 跳向结算界面
	void timeToJumpToEndLayer(float t);
	void timeToJumpToJingJi(float t);
	void timeToJumpToKaiBaoXiang(float t);
	void timeToShouCheng(float t);
	void timeToPaiHangBang(float t);
	void timeToDianFengDuiJue(float t);
	void timeToJumpToZiYouJingji(float t);
	void timeToJiXianShouCheng(float t);
	void timeToChuanQiLu(float t);
	void timeToXueSeJiaoTangEndLayer(float t);
	void timeToMapsLayer(float t);



	void fightTrigle(CCObject* null_obj);
	void waitTimeEnd(CCObject* null_obj);
	// oneStep的触发
	//void fightTrgile(float time);
	// 把step中的反弹数据分离出来
	vector<Buff_Doing> separationFanTanDataFromStep(OneStep& step);
	void setItemOrderBack(CCNode* node);

	// 用血
	void useBlood(ClientChessBoardItem* user,StepUseBlood step);
	ZhuangTaiBar* getZhuangTaiBar();
	// 血用上了
	void useBloodOk(CCNode* node_hag_user);

	// 完成了一个可视化的一步，见ChessBoardHelper的one_visible_step
	void one_visible_step(vector<CdDiscr> cdDisc);

	// 完毕
	void fightEnd(bool notWait = false);
	void dispLayerNextFightLabel(int changci_num);

	void someOneDead(CCObject* clientChessBoardItem);
	void pullOneEquip(ClientChessBoardItem* item, FightReward equip,CCPoint offset = CCPointZero);
	void pullOneItem(ClientChessBoardItem* item, FightReward reward,CCPoint offset = CCPointZero);
	void pullOneItemOrSuiPian(ClientChessBoardItem* item, FightReward itemOrSuiPian,CCPoint offset = CCPointZero);
	void pullOneItemSuiPian(ClientChessBoardItem* item, FightReward itemSuiPian,CCPoint offset = CCPointZero);
	void pullOneShenBinSuiPian(ClientChessBoardItem* item, FightReward shenBingSuiPian,CCPoint offset = CCPointZero);
	void pullOnePPT(ClientChessBoardItem* item, FightReward ppt,CCPoint offset = CCPointZero);
	void showItemCard(CCNode* baoxiang);
	void showHammer(ClientChessBoardItem* item, CCPoint offset);//1-4达林的铁锤
	void itemActionEnd(CCNode* node);
	void equipActionEnd(CCNode* node);
	// 显示第一次的得到装备的guide
	void showFirstGetEquipGuide(CCNode* eff_node);
	// 显示第一次得到技能书的教学
	void showFirstGetSkillBookGuide(CCNode* eff_node);
	
	void setFightTrigleWaitSomeTime(float time_to_wait);
	void canleFightTrigleWait();

	//void pauseFightTrigle(CCNode* node_not_use);
	//void resumeFightTrigle(CCNode* node_not_use);

	void showFailByTakeTooMuchTimeDailog();




	//20140619 scf
	void userSkillPrepare(CCNode *node, void *data); //move hero, hide hero, add mask layer
	void useSkillEnd(CCObject *obj);

public:
	// 多场表演.不支持重载构造函数互调
	// 如果直接调用，相当于建立了一个临时的，而不是自己。
	//ClientFightLayer(vector<FightDirector> dir_vec,CCSprite* bg,bool testMode);

	// 设置数据
	//   key                 value					描述
	// "JumpToWhenEnd" ---> 
	//						"Default"				--> 正常打Boss用的，完毕就到结算界面
	//						"JingJi_2"				--> 竞技场用的，完毕之后就跳向竞技场，进行下一场战斗
	//						"JingJi_KaiBaoXiang"	--> 竞技场用的，完毕之后就跳向竞技场开宝箱
	//
	// "setFightBarVisible"   ----> true/false		
	ClientFightLayer(vector<FightDirector> dir_vec,CCSprite* bg,Bundle bundle = Bundle());

	//static ClientFightLayer* getCurrentInstance();

	// 设置在战斗前一刻的团队数据
	void setBeforeFightPro(HeroBaseProperty data_4[]);

	//void setTest(bool test);

	~ClientFightLayer(void);
};

