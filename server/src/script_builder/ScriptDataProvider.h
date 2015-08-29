#pragma once
#include "protocol.h"
#include <vector>
#include "CommonDefine.h"
#include "GameBuff.h"
#include "PropertyCalculator.h"

using namespace std;



#ifdef  _CLIENT_
#define USE_FIGHT_LOG  1 // 要打日志就是1，非1就是不打
#endif

#if USE_FIGHT_LOG == 0
#define LFLog(...)       do {} while (0)

#elif USE_FIGHT_LOG == 1
#define LFLog(format, ...)      CCLog(format, ##__VA_ARGS__)
#endif


#define TEAM_TAG_A 1
#define TEAM_TAG_B -1

// 在特殊关卡中，boss不动，默认boss的Tag，就是B棋盘
#define DEFAULT_SPECIAL_CONTROL_BOSS_TAG	TEAM_TAG_B

// 角色的类型
enum HeroBaseType
{
	Type_Guaiwu_putong = 1,
	Type_Guaiwu_jingying = 2,
	//Type_Guaiwu_boss = 3, // boss就是精英，所以不需要定义！
	Type_Hero_Normal = 4,
	Type_Hero_Team = 5,		// 是代表一个队的Item
};

// 脚本返回值
struct HeroState
{
	unsigned int A_teamHeroId[FormationHeroAcountMax];
	unsigned int B_teamHeroId[FormationHeroAcountMax];
	unsigned int currentHp_A[FormationHeroAcountMax];	// A对的4个人的血量，没有人就是0
	unsigned int currentHp_B[FormationHeroAcountMax];	// B对的4个人的血量，没有人就是0
	
	short A_teamCurrentBloodNum;						// 还剩下的血瓶
	short B_teamCurrentBloodNum;

	// A队,B队的最后的回合数(>=1)
	// 他们之间很有可能差1
	// 如果要获取主观上的，绝对的回合数，那么就直接MAX(A,B)即可
	unsigned int roundNumForA;
	unsigned int roundNumForB;

	HeroState()
	{
		A_teamCurrentBloodNum = 0;
		B_teamCurrentBloodNum = 0;
		roundNumForA = 0;
		roundNumForB = 0;
		memset(currentHp_A,0,sizeof(unsigned int)*FormationHeroAcountMax);
		memset(currentHp_B,0,sizeof(unsigned int)*FormationHeroAcountMax);
		memset(A_teamHeroId,0,sizeof(unsigned int)*FormationHeroAcountMax);
		memset(B_teamHeroId,0,sizeof(unsigned int)*FormationHeroAcountMax);
	}
};

// 团队作战完毕之后的数据
struct TeamEndParam
{
	// 每个人对应的砍血数量
	int fightedBlood[FormationHeroAcountMax];
};

struct TeamEndOutParam
{
	// 每个人得到的道具和个数
	int tuanDuiDaoJuType[FormationHeroAcountMax];
	int tuanDuiDaoJuNum[FormationHeroAcountMax];
};

enum BattleType
{
	BattleType_Arena		= 1,
	BattleType_MainTask		= 2,
	BattleType_CityGuard	= 3,
	BattleType_TopHeroChallenge	= 4,
	BattleType_TopArena	= 5,
	BattleType_LegendNote	= 6,
	BattleType_SelfChallenge = 7,
	BattleType_ProtectSpirit = 8,
};

// 初始攻击的棋盘->即先手,第一滴血
enum FirstBloodBoard
{
	FBB_BoardA = 0,	// A 棋盘先手
	FBB_BoardB,		// B 棋盘先手
	FBB_Random,		// 随机
};

// 用户数据
struct ScriptBuilderUserData
{
	//
	// 这个标记外界不需要修该，读就可以了，TEAM_TAG_A 就是A对，TEAM_TAG_B 就是B对，有Builder填写
	int teamTag;						
	void* data;
	ScriptBuilderUserData()
	{
		data = NULL;
	}
};

// 返回给客户端看的角色基本数据
struct HeroBase
{
	unsigned int heroId;			// 
	Profession profession_id;		// 职业id
	unsigned int level;
	unsigned int stage;				// 阶，初始就是1
	unsigned int hp;				// 血量，当前的血值
	unsigned int defaultHp;			// 满血值,对于服务器来说是没有用的
	HeroBaseType hero_type;			// HeroBaseType
	short defaultCdTime;					// cd时间，是步数，不包括wait和useBlood
	short x_size;	// 描述x方向上的占用的格子数量，默认是1，还有的取值是2
	short resonance_star;			//装备强化共鸣星级

	// 装备的概述信息
	vector<ZhuangbeiSummary> summary_equips;

	HeroBase()
	{
		defaultHp = 0;  
		hp = 0;
		x_size = 1;
		stage = 1;
		defaultCdTime = 0;
		resonance_star = 0;
		hero_type = Type_Hero_Normal;
	}
};

struct SkillDataForFight
{
	SkillType skillType;
	unsigned int skillLevel;

	SkillDataForFight(SkillType _skillType)
	{
		skillType = _skillType;
		skillLevel = 1;
	}
	SkillDataForFight(SkillType _skillType,unsigned int _skillLevel)
	{
		skillType = _skillType;
		skillLevel = _skillLevel;
	}
};

class AbstractItem
{
protected:
	unsigned int mDefaultFullHp;		// 满血值
	int mFightTurn;						// 当前FIght的轮数
	short mCdTime;
	list<GameBuff> mAttachedBuffVec;
public:
	// 用于攻击的技能，但目前只支持一个，也只会选择第0个使用
	vector<SkillDataForFight> skilVec;
	// 是否装配了某个技能
	bool isEquipedSkill(SkillType type);
	// 附加一个buff，成功就返回true
	// 失败是应为该buff不能再附加了，因为已经有了
	// 那么就应该调用updateBuff
	bool attachBuff(GameBuff buff);
	// 更新一个存在的buff,成功返回true
	// 失败是因为该状态类型的buff不存在，所有不能update
	// 那么就应该调用attachBuff
	bool updateBuff(GameBuff buff);

	bool haveBuff(int _id);
	// 是否有某个类型的buff
	bool haveBuffWithStateIs(BuffStateId state);
	// 移除指定Id的Buff
	void removeBuff(int _id);

	list<GameBuff> getCurrentAttachedBuff() const {return mAttachedBuffVec;};
public:
	AbstractItem();
	virtual ~AbstractItem(){}; // 父类的析构一定得是虚函数，不然子类的析构函数无法调用

	// 设置和获取当前的攻击轮数，在没有调用过fightTurnInc之前，数据都是1
	// 每次攻击完毕之后，一定要调用fightTurnInc更新数据
	virtual void fightTurnInc(){mFightTurn++;};
	virtual int getCurrentFightTurn(){return mFightTurn;};
	// 测试当前是否可以用技能攻击与否
	virtual bool canUseThisInCurrentFightTurn();

	virtual void subHp(int toSub);				// 加血
	virtual void addHp(int toAdd);				// 减血
	virtual int getDefaultHp() const;			// 获取默认的血值，就是满血的血值
	virtual unsigned int getMinJie() const;
	virtual unsigned int getZhiLi() const ;	

	//virtual void setDefaultCdTime(short time){};
	virtual short getDefaultCdTime()const{return 0;};
	virtual void setCdTime(short time){mCdTime = time;};
	virtual short getCdTime()const{return mCdTime;};
public:

	virtual Profession getProfession() const = 0;
	virtual HeroBaseType getHeroType() const = 0;
	virtual int getLevel() const = 0;				// 获取等级
	virtual int getStage() const = 0;
	virtual float getGongJiJuLi()const= 0;			// 攻击距离
	virtual float getGongJiLi(ValueRange range) const = 0;			// 攻击力
	virtual int getHp() const = 0;
	virtual void setCurrentHp(unsigned int hp) = 0;
	virtual int getXSize() const = 0;	// 获取X方向上的大小,用于一个怪物占3格中间的情况
	virtual unsigned int getBaoJiLv() const = 0;			// 暴击率，用的时候需要自己除以100
	virtual unsigned int getHuJiaJianMian() const = 0;		// 护甲减免，用的时候需要自己除以100
	virtual unsigned int getKangXingJianMian() const = 0;	// 抗性减免，用的时候需要自己除以100
	virtual unsigned int getBaoJiShangHaiYinZi() const = 0;	// 暴击伤害因子，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaLv() const = 0;		// 技能触发率，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaZhouQi() const = 0;	// 技能触发周期，即每隔多久触发一次技能
	virtual unsigned int getJiNengChuFaTurn() const = 0;	// 技能触发点，第一次触发技能的点
	virtual unsigned int getJiNengResult() const = 0;		// 技能的效果，技能的伤害需要根据这个效果加成。 用的时候需要自己除以100
	virtual CalculateData getCalculateData() const = 0;		// 获取计算需要的数据源
	// vs bug,必须用struct一下
	virtual struct HeroPropertySummery getSummeryProperty() const = 0;	 
	virtual unsigned int getShanBiLv() const = 0;			// 闪避率，用的时候需要自己除以100
	virtual int getShangHaiLeiXing()const = 0;		
	// 获取攻击target时产生的伤害值
	virtual float getShangHaiValue(AbstractItem* target,ValueRange range = Range_Ramdom) const = 0;

	//技能特殊触发记录
	virtual bool getSpecialTrigger() const = 0;
	virtual void setSpecialTrigger() = 0;
};

// 团队的综合实力的数据，用于组队打Boos
// 团队的装备啊，道具什么的都直接映射到数据里面
struct TeamBaseProperty
{
	Profession leaderProfression;	
	unsigned int mLeaderId;				//
	unsigned int mHP;					// 生命值,
	unsigned int mLevel;				// 团队的等级，就用主角等级
	unsigned int mStage;				// 团队的阶
	unsigned int mGongJiLi_MIN;			// 攻击力
	unsigned int mGongJiLi_MAX;			// 攻击力
	float mGongJiJuLi;					// 这里就是恒定的1，因为根据棋盘的布局，大家都是1
	unsigned int mBaoJiLv;				// 暴击率
	unsigned int mBaoJiShangHaiYinZi;	// 暴击的伤害因子，比如150%，就是攻击的150
	unsigned int mJiNengFaChuLv;		// 技能发出概率
	unsigned int mJiNengZhouQi;			// 技能周期
	unsigned int mJiNengTurn;			// 技能触发点
	unsigned int mJiNengResultPercent;	// 技能伤害值，默认是就是100，即1.0


	int mShangHaiLeiXing;				// 伤害类型,默认是SHANG_HAI_MO_FA
	int mShanBiLv;						// 闪避率
	int mGeDangLv;						// 格挡率
	int mGeDangZhi;						// 格挡值
	int mHuJiaZhi;						// 护甲值
	int mKangXingZhi;					// 抗性值
	short mDefaultFreezyNum;						// 默认冷却时间

	TeamBaseProperty()
	{
		mLeaderId = 0;
		mDefaultFreezyNum = 0;
		mJiNengResultPercent = 100;
		mStage = 1;
	}
};

class TeamHero : public AbstractItem
{
private:
	TeamBaseProperty mPro;
public:
	TeamHero();
	~TeamHero();
	void initWithTeamBaseProperty(TeamBaseProperty pro);

	//virtual void setDefaultCdTime(short time){CCAssert(false,"TeamHero的已经存在mPro里的，不需要调用");};
	virtual short getDefaultCdTime()const{return mPro.mDefaultFreezyNum;};

	// 只是用于显示的装备，没有任何实际意义和计算
	// 因为装备的宏观影响值已经在TeamBaseProperty中了
	// 这里主要用于告诉客户端显示哪些装备
	vector<EquipData> equipVec;

public:

	// getCalculateData只是空架子，不能调用
	virtual CalculateData getCalculateData() const;
	// 空架子，不能调用
	virtual struct HeroPropertySummery getSummeryProperty() const;	

	TeamBaseProperty getTeamProperty() const;
	virtual void setCurrentHp(unsigned int hp);
	virtual Profession getProfession() const;
	virtual HeroBaseType getHeroType() const;
	virtual int getLevel() const;				// 获取等级
	virtual int getStage() const;
	virtual float getGongJiJuLi()const;			// 攻击距离
	virtual float getGongJiLi(ValueRange range) const;			// 攻击力
	virtual int getHp() const;
	virtual int getXSize() const;	// 获取X方向上的大小,用于一个怪物占3格中间的情况
	virtual unsigned int getBaoJiLv() const;			// 暴击率，用的时候需要自己除以100
	virtual unsigned int getHuJiaJianMian() const;		// 护甲减免，用的时候需要自己除以100
	virtual unsigned int getKangXingJianMian() const;	// 抗性减免，用的时候需要自己除以100
	virtual unsigned int getBaoJiShangHaiYinZi() const;	// 暴击伤害因子，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaLv() const;		// 技能触发率，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaZhouQi() const;	// 技能触发周期，即每隔多久触发一次技能
	virtual unsigned int getJiNengChuFaTurn() const;	// 技能触发点，第一次触发技能的点
	virtual unsigned int getJiNengResult() const;		// 技能的效果，技能的伤害需要根据这个效果加成。 用的时候需要自己除以100

	virtual unsigned int getShanBiLv() const;			// 闪避率，用的时候需要自己除以100
	virtual int getShangHaiLeiXing()const;		
	// 获取攻击target时产生的伤害值
	virtual float getShangHaiValue(AbstractItem* target,ValueRange range = Range_Ramdom) const;
	
	//技能特殊触发记录
	virtual bool getSpecialTrigger() const;
	virtual void setSpecialTrigger();
};

// 战斗时用的角色的数据，这个类没有做任何具体实现，全是空函数，类似一个Adapter
// 方便子类继承
class BaseHero : public AbstractItem 
{
private:
	//BaseHero(const BaseHero &a);						// 私有的拷贝构造函数
	//BaseHero& operator = (const BaseHero& rightData);	// 私有赋值符重载
protected:
	PropertyCalculator mCalulator;
	short mDefaultBloodCdTime;				// cd时间，喝了血之后，就不能再喝血
	virtual void setCurrentHp(unsigned int hp);
public:
	BaseHero();
	virtual ~BaseHero();

	virtual void setDefaultCdTime(short time){mDefaultBloodCdTime = time;};
	virtual short getDefaultCdTime()const{return mDefaultBloodCdTime;};

	// getCalculateData只是空架子，不能调用
	virtual CalculateData getCalculateData() const;
	// 只是空架子，不能调用
	virtual struct HeroPropertySummery getSummeryProperty() const;	
	//vi
	virtual Profession getProfession() const;
	virtual HeroBaseType getHeroType() const;
	virtual unsigned int getMinJie() const;
	virtual int getLevel() const;				// 获取等级
	virtual int getStage() const;
	virtual float getGongJiJuLi()const;			// 攻击距离
	virtual float getGongJiLi(ValueRange range) const;			// 攻击力
	virtual int getHp() const;
	virtual int getXSize() const;	// 获取X方向上的大小,用于一个怪物占3格中间的情况
	virtual unsigned int getZhiLi() const ;	
	virtual unsigned int getLiLiang() const ;	
	virtual unsigned int getBaoJiLv() const;			// 暴击率，用的时候需要自己除以100
	virtual unsigned int getHuJiaJianMian() const;		// 护甲减免，用的时候需要自己除以100
	virtual unsigned int getKangXingJianMian() const;	// 抗性减免，用的时候需要自己除以100
	virtual unsigned int getBaoJiShangHaiYinZi() const;	// 暴击伤害因子，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaLv() const;		// 技能触发率，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaZhouQi() const;	// 技能触发周期，即每隔多久触发一次技能
	virtual unsigned int getJiNengChuFaTurn() const;	// 技能触发点，第一次触发技能的点
	virtual unsigned int getJiNengResult() const;		// 技能的效果，技能的伤害需要根据这个效果加成。 用的时候需要自己除以100
	virtual unsigned int getShanBiLv() const;			// 闪避率，用的时候需要自己除以100
	virtual int getShangHaiLeiXing()const;				// 获取伤害类型
	
	//技能特殊触发记录
	virtual bool getSpecialTrigger() const;
	virtual void setSpecialTrigger();
};

// 怪物的最终数据
struct GuaiWuData
{
	int level;
	unsigned int gongJiLi;
	unsigned int xueLiang;
	unsigned int baoJiLv;
	unsigned int baoShangYinZi;
	unsigned int jiNengChuFaLv;
	unsigned int huJiaJianMianLv;
	unsigned int kangXingJianMianLv;
	float gongJiJuLi;
	int shangHaiLeiXing;//SHANG_HAI_MO_FA,SHANG_HAI_WU_LI
	Profession profession;
	HeroBaseType type;
	int x_size;

	GuaiWuData()
	{
		type = Type_Guaiwu_putong;
		x_size = 1;
	}

};

// 怪物
class Guaiwu : public BaseHero
{
private:
	GuaiWuData mData;
	bool mSpecialTriggerFlag;
protected:
	virtual void setCurrentHp(unsigned int hp);
	// 只用于技能的，其他的没用！
	// 计算攻击，伤害等是在PropertyCalculator里面直接用Guaiwu* 计算的
	virtual CalculateData getCalculateDataForSkill() const;
public:
	void initWithGuaiwuData(GuaiWuData data);

	GuaiWuData getProperty()const{return mData;};

	virtual Profession getProfession() const;
	virtual HeroBaseType getHeroType() const;
	virtual int getXSize() const;	// 获取X方向上的大小,用于一个怪物占3格中间的情况
	virtual float getGongJiJuLi()const;			// 攻击距离
	virtual int getGongJiLi() const;			// 攻击力
	virtual float getGongJiLi(ValueRange range) const;			// 攻击力
	virtual int getLevel() const;						// 获取等级
	virtual int getStage() const;
	virtual int getHp() const;
	virtual unsigned int getMinJie() const;
	virtual unsigned int getBaoJiLv() const;			// 暴击率，用的时候需要自己除以100
	virtual unsigned int getHuJiaJianMian() const;		// 护甲减免，用的时候需要自己除以100
	virtual unsigned int getKangXingJianMian() const;	// 抗性减免，用的时候需要自己除以100
	virtual unsigned int getBaoJiShangHaiYinZi() const;	// 暴击伤害因子，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaLv() const;		// 技能触发率，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaZhouQi() const;	// 技能触发周期，即每隔多久触发一次技能
	virtual unsigned int getJiNengChuFaTurn() const;	// 技能触发点，第一次触发技能的点
	virtual unsigned int getJiNengResult() const;		// 技能的效果，技能的伤害需要根据这个效果加成。 用的时候需要自己除以100
	virtual unsigned int getShanBiLv() const;			// 闪避率，用的时候需要自己除以100
	// 获取攻击target时产生的伤害值
	virtual float getShangHaiValue(AbstractItem* target,ValueRange range = Range_Ramdom) const;
	virtual int getShangHaiLeiXing()const;				// 获取伤害类型
	
	//技能特殊触发记录
	virtual bool getSpecialTrigger() const;
	virtual void setSpecialTrigger();
};

// 英雄，就是Client端的BaseActor一样
class Hero : public BaseHero
{
	// 属性
private:
	HeroBaseProperty pro;
	int currentHp;
	bool equipOpEnd;
	Hero* yuanShenBody;
protected:
	virtual void setCurrentHp(unsigned int hp);
	CalculateData getCalculateData(bool needMainPro) const;
	struct HeroPropertySummery getSummeryProperty() const;	
public:
	Hero();
	virtual ~Hero();
	void init(HeroBaseProperty basePro);

	// 设置元神，需要在zhuangbeiOpEnd之前设置
	// 不然调用了zhuangbeiOpEnd，数据就准备好了..
	// 会自动delete
	void setYuanShen_AutoDeleted(Hero* yuanshen);

	// 装备的操作完毕了，需要调用这个方法，用于刷新血量
	// 如果指定了currentHp，那么就会把当前血量设置成指定的血值
	void zhuangbeiOpEnd(int curHp = -1);
	bool isOpEnd(){return equipOpEnd;};



	// 只读
	const HeroBaseProperty* getHeroBaseProperty_R() const {return &pro;};

	// 只用Hero才可以用这个getCalculateData方法，其他的都是Assert的
	virtual CalculateData getCalculateData() const;

	// 装备在身上的装备
	vector<EquipData> equipVec;

	// 动态值的获取，不要通过getHeroBaseProperty_*方法获取
	virtual unsigned int getLiLiang() const ;	
	int getTiNeng() const;
	virtual unsigned int getZhiLi() const;	
	virtual unsigned int getMinJie() const;
	virtual int getHp() const;
	int getMainPro() const;
	unsigned short get_resonance_star()const;
	virtual Profession getProfession() const;
	virtual HeroBaseType getHeroType() const;
	virtual int getXSize() const;
	virtual float getGongJiJuLi()const;			// 攻击距离
	virtual float getGongJiLi(ValueRange range) const;	// 攻击力
	virtual int getLevel() const;						// 获取等级
	virtual int getStage() const;
	virtual unsigned int getBaoJiLv() const;			// 暴击率，用的时候需要自己除以100
	virtual unsigned int getHuJiaJianMian() const;		// 护甲减免，用的时候需要自己除以100
	virtual unsigned int getKangXingJianMian() const;	// 抗性减免，用的时候需要自己除以100
	virtual unsigned int getBaoJiShangHaiYinZi() const;	// 暴击伤害因子，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaLv() const;		// 技能触发率，用的时候需要自己除以100
	virtual unsigned int getJiNengChuFaZhouQi() const;	// 技能触发周期，即每隔多久触发一次技能
	virtual unsigned int getJiNengChuFaTurn() const;	// 技能触发点，第一次触发技能的点
	virtual unsigned int getJiNengResult() const;		// 技能的效果，技能的伤害需要根据这个效果加成。 用的时候需要自己除以100
	virtual unsigned int getShanBiLv() const;			// 闪避率，用的时候需要自己除以100
	// 获取攻击target时产生的伤害值
	virtual float getShangHaiValue(AbstractItem* target,ValueRange range = Range_Ramdom) const;
	virtual int getShangHaiLeiXing()const;				// 获取伤害类型
};

// 脚本的数据提供器
class ScriptDataProvider
{
public:
	ScriptDataProvider(void);

	// 查询数据库，完成数据的load，用于模拟战都过程
	// 服务器需要做的的！！！,棋盘要用,
	// 如果是怪物，就new GuaiWu，然后初始化返回即可
	// 如果是英雄，那么就new Hero 然后初始化返回即可，如果没有人，那么就返回NULL
	virtual BaseHero* getBaseHero(unsigned int _heroId,ScriptBuilderUserData data) = 0;

	// 获取_heroId的元神的数据，没有就直接返回NULL即可
	// 默认就是返回NULL
	virtual BaseHero* getBaseHeroYuanShen(unsigned int _heroId,ScriptBuilderUserData data){return NULL;};

	// 获得团队的血瓶数量
	virtual void getTeamBloodNum(short& currentNum,short& defaultNum,int& bloodAddPercent,ScriptBuilderUserData data) = 0;
	// 获取用完血瓶之后的cd时间
	virtual void getHeroCDTime(short& cdStepNum,unsigned heroId,ScriptBuilderUserData data) = 0;

	// 查询数据库，完成数据的load，用于客户端数据的刷新
	// 返回给客户端
	// 服务器需要做的的！！！
	virtual void getHeroDetail(HeroDetail& outData,unsigned int _heroId,ScriptBuilderUserData data) = 0;

	// 战斗完毕，这里做数据持久化,返回得到的经验值
	virtual unsigned int fightEndCallback(ScriptBuilderUserData data,bool win) = 0;


	// 获取掉落的Item碎片
	virtual vector<ItemSuipianInfo> getFightEndItemSuiPians(ScriptBuilderUserData data,bool win) = 0;
	// 获取掉落的神兵碎片
	virtual vector<ShenbingSuipianInfo> getFightEndShenBingSuiPians(ScriptBuilderUserData data,bool win) = 0;
	// 获取掉的装备
	// 服务器必须得填
	virtual vector<EquipData> getFightEndEquips(ScriptBuilderUserData data,bool win) = 0;
	// 获取掉的物品
	// 服务器必须得填
	virtual vector<ItemInfo> getFightEndItems(ScriptBuilderUserData data,bool win)= 0;
	// 获取得到的能量点
	virtual vector<PowerPoint> getFightEndPowerPoints(ScriptBuilderUserData data,bool win){
		vector<PowerPoint> null_vec;
		return null_vec;
	};


	~ScriptDataProvider(void);
};

#ifdef _CLIENT_

class ClientTestProvider : public ScriptDataProvider
{
	BaseHero* getBaseHero(unsigned int _heroId,ScriptBuilderUserData user_data);
	void getTeamBloodNum(short& currentNum,short& defaultNum,int& bloodAddPercent,ScriptBuilderUserData data);
	void getHeroCDTime(short& cdStepNum,unsigned heroId,ScriptBuilderUserData data);
	void getHeroDetail(HeroDetail& outData,unsigned int _heroId,ScriptBuilderUserData data);
	unsigned int fightEndCallback(ScriptBuilderUserData data,bool win);

	vector<ItemSuipianInfo> getFightEndItemSuiPians(ScriptBuilderUserData data,bool win);
	vector<ShenbingSuipianInfo> getFightEndShenBingSuiPians(ScriptBuilderUserData data,bool win);
	vector<EquipData> getFightEndEquips(ScriptBuilderUserData data,bool win);
	vector<ItemInfo> getFightEndItems(ScriptBuilderUserData data,bool win);
	virtual vector<PowerPoint> getFightEndPowerPoints(ScriptBuilderUserData data,bool win);

	virtual void teamFightEnd(bool win,TeamEndParam parm,TeamEndOutParam& gettedItems);
};
#endif // _CLIENT_
