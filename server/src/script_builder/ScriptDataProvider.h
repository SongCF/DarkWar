#pragma once
#include "protocol.h"
#include <vector>
#include "CommonDefine.h"
#include "GameBuff.h"
#include "PropertyCalculator.h"

using namespace std;



#ifdef  _CLIENT_
#define USE_FIGHT_LOG  1 // Ҫ����־����1����1���ǲ���
#endif

#if USE_FIGHT_LOG == 0
#define LFLog(...)       do {} while (0)

#elif USE_FIGHT_LOG == 1
#define LFLog(format, ...)      CCLog(format, ##__VA_ARGS__)
#endif


#define TEAM_TAG_A 1
#define TEAM_TAG_B -1

// ������ؿ��У�boss������Ĭ��boss��Tag������B����
#define DEFAULT_SPECIAL_CONTROL_BOSS_TAG	TEAM_TAG_B

// ��ɫ������
enum HeroBaseType
{
	Type_Guaiwu_putong = 1,
	Type_Guaiwu_jingying = 2,
	//Type_Guaiwu_boss = 3, // boss���Ǿ�Ӣ�����Բ���Ҫ���壡
	Type_Hero_Normal = 4,
	Type_Hero_Team = 5,		// �Ǵ���һ���ӵ�Item
};

// �ű�����ֵ
struct HeroState
{
	unsigned int A_teamHeroId[FormationHeroAcountMax];
	unsigned int B_teamHeroId[FormationHeroAcountMax];
	unsigned int currentHp_A[FormationHeroAcountMax];	// A�Ե�4���˵�Ѫ����û���˾���0
	unsigned int currentHp_B[FormationHeroAcountMax];	// B�Ե�4���˵�Ѫ����û���˾���0
	
	short A_teamCurrentBloodNum;						// ��ʣ�µ�Ѫƿ
	short B_teamCurrentBloodNum;

	// A��,B�ӵ����Ļغ���(>=1)
	// ����֮����п��ܲ�1
	// ���Ҫ��ȡ�����ϵģ����ԵĻغ�������ô��ֱ��MAX(A,B)����
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

// �Ŷ���ս���֮�������
struct TeamEndParam
{
	// ÿ���˶�Ӧ�Ŀ�Ѫ����
	int fightedBlood[FormationHeroAcountMax];
};

struct TeamEndOutParam
{
	// ÿ���˵õ��ĵ��ߺ͸���
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

// ��ʼ����������->������,��һ��Ѫ
enum FirstBloodBoard
{
	FBB_BoardA = 0,	// A ��������
	FBB_BoardB,		// B ��������
	FBB_Random,		// ���
};

// �û�����
struct ScriptBuilderUserData
{
	//
	// ��������粻��Ҫ�޸ã����Ϳ����ˣ�TEAM_TAG_A ����A�ԣ�TEAM_TAG_B ����B�ԣ���Builder��д
	int teamTag;						
	void* data;
	ScriptBuilderUserData()
	{
		data = NULL;
	}
};

// ���ظ��ͻ��˿��Ľ�ɫ��������
struct HeroBase
{
	unsigned int heroId;			// 
	Profession profession_id;		// ְҵid
	unsigned int level;
	unsigned int stage;				// �ף���ʼ����1
	unsigned int hp;				// Ѫ������ǰ��Ѫֵ
	unsigned int defaultHp;			// ��Ѫֵ,���ڷ�������˵��û���õ�
	HeroBaseType hero_type;			// HeroBaseType
	short defaultCdTime;					// cdʱ�䣬�ǲ�����������wait��useBlood
	short x_size;	// ����x�����ϵ�ռ�õĸ���������Ĭ����1�����е�ȡֵ��2
	short resonance_star;			//װ��ǿ�������Ǽ�

	// װ���ĸ�����Ϣ
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
	unsigned int mDefaultFullHp;		// ��Ѫֵ
	int mFightTurn;						// ��ǰFIght������
	short mCdTime;
	list<GameBuff> mAttachedBuffVec;
public:
	// ���ڹ����ļ��ܣ���Ŀǰֻ֧��һ����Ҳֻ��ѡ���0��ʹ��
	vector<SkillDataForFight> skilVec;
	// �Ƿ�װ����ĳ������
	bool isEquipedSkill(SkillType type);
	// ����һ��buff���ɹ��ͷ���true
	// ʧ����ӦΪ��buff�����ٸ����ˣ���Ϊ�Ѿ�����
	// ��ô��Ӧ�õ���updateBuff
	bool attachBuff(GameBuff buff);
	// ����һ�����ڵ�buff,�ɹ�����true
	// ʧ������Ϊ��״̬���͵�buff�����ڣ����в���update
	// ��ô��Ӧ�õ���attachBuff
	bool updateBuff(GameBuff buff);

	bool haveBuff(int _id);
	// �Ƿ���ĳ�����͵�buff
	bool haveBuffWithStateIs(BuffStateId state);
	// �Ƴ�ָ��Id��Buff
	void removeBuff(int _id);

	list<GameBuff> getCurrentAttachedBuff() const {return mAttachedBuffVec;};
public:
	AbstractItem();
	virtual ~AbstractItem(){}; // ���������һ�������麯������Ȼ��������������޷�����

	// ���úͻ�ȡ��ǰ�Ĺ�����������û�е��ù�fightTurnInc֮ǰ�����ݶ���1
	// ÿ�ι������֮��һ��Ҫ����fightTurnInc��������
	virtual void fightTurnInc(){mFightTurn++;};
	virtual int getCurrentFightTurn(){return mFightTurn;};
	// ���Ե�ǰ�Ƿ�����ü��ܹ������
	virtual bool canUseThisInCurrentFightTurn();

	virtual void subHp(int toSub);				// ��Ѫ
	virtual void addHp(int toAdd);				// ��Ѫ
	virtual int getDefaultHp() const;			// ��ȡĬ�ϵ�Ѫֵ��������Ѫ��Ѫֵ
	virtual unsigned int getMinJie() const;
	virtual unsigned int getZhiLi() const ;	

	//virtual void setDefaultCdTime(short time){};
	virtual short getDefaultCdTime()const{return 0;};
	virtual void setCdTime(short time){mCdTime = time;};
	virtual short getCdTime()const{return mCdTime;};
public:

	virtual Profession getProfession() const = 0;
	virtual HeroBaseType getHeroType() const = 0;
	virtual int getLevel() const = 0;				// ��ȡ�ȼ�
	virtual int getStage() const = 0;
	virtual float getGongJiJuLi()const= 0;			// ��������
	virtual float getGongJiLi(ValueRange range) const = 0;			// ������
	virtual int getHp() const = 0;
	virtual void setCurrentHp(unsigned int hp) = 0;
	virtual int getXSize() const = 0;	// ��ȡX�����ϵĴ�С,����һ������ռ3���м�����
	virtual unsigned int getBaoJiLv() const = 0;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getHuJiaJianMian() const = 0;		// ���׼��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getKangXingJianMian() const = 0;	// ���Լ��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getBaoJiShangHaiYinZi() const = 0;	// �����˺����ӣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaLv() const = 0;		// ���ܴ����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaZhouQi() const = 0;	// ���ܴ������ڣ���ÿ����ô���һ�μ���
	virtual unsigned int getJiNengChuFaTurn() const = 0;	// ���ܴ����㣬��һ�δ������ܵĵ�
	virtual unsigned int getJiNengResult() const = 0;		// ���ܵ�Ч�������ܵ��˺���Ҫ�������Ч���ӳɡ� �õ�ʱ����Ҫ�Լ�����100
	virtual CalculateData getCalculateData() const = 0;		// ��ȡ������Ҫ������Դ
	// vs bug,������structһ��
	virtual struct HeroPropertySummery getSummeryProperty() const = 0;	 
	virtual unsigned int getShanBiLv() const = 0;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual int getShangHaiLeiXing()const = 0;		
	// ��ȡ����targetʱ�������˺�ֵ
	virtual float getShangHaiValue(AbstractItem* target,ValueRange range = Range_Ramdom) const = 0;

	//�������ⴥ����¼
	virtual bool getSpecialTrigger() const = 0;
	virtual void setSpecialTrigger() = 0;
};

// �Ŷӵ��ۺ�ʵ�������ݣ�������Ӵ�Boos
// �Ŷӵ�װ����������ʲô�Ķ�ֱ��ӳ�䵽��������
struct TeamBaseProperty
{
	Profession leaderProfression;	
	unsigned int mLeaderId;				//
	unsigned int mHP;					// ����ֵ,
	unsigned int mLevel;				// �Ŷӵĵȼ����������ǵȼ�
	unsigned int mStage;				// �ŶӵĽ�
	unsigned int mGongJiLi_MIN;			// ������
	unsigned int mGongJiLi_MAX;			// ������
	float mGongJiJuLi;					// ������Ǻ㶨��1����Ϊ�������̵Ĳ��֣���Ҷ���1
	unsigned int mBaoJiLv;				// ������
	unsigned int mBaoJiShangHaiYinZi;	// �������˺����ӣ�����150%�����ǹ�����150
	unsigned int mJiNengFaChuLv;		// ���ܷ�������
	unsigned int mJiNengZhouQi;			// ��������
	unsigned int mJiNengTurn;			// ���ܴ�����
	unsigned int mJiNengResultPercent;	// �����˺�ֵ��Ĭ���Ǿ���100����1.0


	int mShangHaiLeiXing;				// �˺�����,Ĭ����SHANG_HAI_MO_FA
	int mShanBiLv;						// ������
	int mGeDangLv;						// ����
	int mGeDangZhi;						// ��ֵ
	int mHuJiaZhi;						// ����ֵ
	int mKangXingZhi;					// ����ֵ
	short mDefaultFreezyNum;						// Ĭ����ȴʱ��

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

	//virtual void setDefaultCdTime(short time){CCAssert(false,"TeamHero���Ѿ�����mPro��ģ�����Ҫ����");};
	virtual short getDefaultCdTime()const{return mPro.mDefaultFreezyNum;};

	// ֻ��������ʾ��װ����û���κ�ʵ������ͼ���
	// ��Ϊװ���ĺ��Ӱ��ֵ�Ѿ���TeamBaseProperty����
	// ������Ҫ���ڸ��߿ͻ�����ʾ��Щװ��
	vector<EquipData> equipVec;

public:

	// getCalculateDataֻ�ǿռ��ӣ����ܵ���
	virtual CalculateData getCalculateData() const;
	// �ռ��ӣ����ܵ���
	virtual struct HeroPropertySummery getSummeryProperty() const;	

	TeamBaseProperty getTeamProperty() const;
	virtual void setCurrentHp(unsigned int hp);
	virtual Profession getProfession() const;
	virtual HeroBaseType getHeroType() const;
	virtual int getLevel() const;				// ��ȡ�ȼ�
	virtual int getStage() const;
	virtual float getGongJiJuLi()const;			// ��������
	virtual float getGongJiLi(ValueRange range) const;			// ������
	virtual int getHp() const;
	virtual int getXSize() const;	// ��ȡX�����ϵĴ�С,����һ������ռ3���м�����
	virtual unsigned int getBaoJiLv() const;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getHuJiaJianMian() const;		// ���׼��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getKangXingJianMian() const;	// ���Լ��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getBaoJiShangHaiYinZi() const;	// �����˺����ӣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaLv() const;		// ���ܴ����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaZhouQi() const;	// ���ܴ������ڣ���ÿ����ô���һ�μ���
	virtual unsigned int getJiNengChuFaTurn() const;	// ���ܴ����㣬��һ�δ������ܵĵ�
	virtual unsigned int getJiNengResult() const;		// ���ܵ�Ч�������ܵ��˺���Ҫ�������Ч���ӳɡ� �õ�ʱ����Ҫ�Լ�����100

	virtual unsigned int getShanBiLv() const;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual int getShangHaiLeiXing()const;		
	// ��ȡ����targetʱ�������˺�ֵ
	virtual float getShangHaiValue(AbstractItem* target,ValueRange range = Range_Ramdom) const;
	
	//�������ⴥ����¼
	virtual bool getSpecialTrigger() const;
	virtual void setSpecialTrigger();
};

// ս��ʱ�õĽ�ɫ�����ݣ������û�����κξ���ʵ�֣�ȫ�ǿպ���������һ��Adapter
// ��������̳�
class BaseHero : public AbstractItem 
{
private:
	//BaseHero(const BaseHero &a);						// ˽�еĿ������캯��
	//BaseHero& operator = (const BaseHero& rightData);	// ˽�и�ֵ������
protected:
	PropertyCalculator mCalulator;
	short mDefaultBloodCdTime;				// cdʱ�䣬����Ѫ֮�󣬾Ͳ����ٺ�Ѫ
	virtual void setCurrentHp(unsigned int hp);
public:
	BaseHero();
	virtual ~BaseHero();

	virtual void setDefaultCdTime(short time){mDefaultBloodCdTime = time;};
	virtual short getDefaultCdTime()const{return mDefaultBloodCdTime;};

	// getCalculateDataֻ�ǿռ��ӣ����ܵ���
	virtual CalculateData getCalculateData() const;
	// ֻ�ǿռ��ӣ����ܵ���
	virtual struct HeroPropertySummery getSummeryProperty() const;	
	//vi
	virtual Profession getProfession() const;
	virtual HeroBaseType getHeroType() const;
	virtual unsigned int getMinJie() const;
	virtual int getLevel() const;				// ��ȡ�ȼ�
	virtual int getStage() const;
	virtual float getGongJiJuLi()const;			// ��������
	virtual float getGongJiLi(ValueRange range) const;			// ������
	virtual int getHp() const;
	virtual int getXSize() const;	// ��ȡX�����ϵĴ�С,����һ������ռ3���м�����
	virtual unsigned int getZhiLi() const ;	
	virtual unsigned int getLiLiang() const ;	
	virtual unsigned int getBaoJiLv() const;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getHuJiaJianMian() const;		// ���׼��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getKangXingJianMian() const;	// ���Լ��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getBaoJiShangHaiYinZi() const;	// �����˺����ӣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaLv() const;		// ���ܴ����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaZhouQi() const;	// ���ܴ������ڣ���ÿ����ô���һ�μ���
	virtual unsigned int getJiNengChuFaTurn() const;	// ���ܴ����㣬��һ�δ������ܵĵ�
	virtual unsigned int getJiNengResult() const;		// ���ܵ�Ч�������ܵ��˺���Ҫ�������Ч���ӳɡ� �õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getShanBiLv() const;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual int getShangHaiLeiXing()const;				// ��ȡ�˺�����
	
	//�������ⴥ����¼
	virtual bool getSpecialTrigger() const;
	virtual void setSpecialTrigger();
};

// �������������
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

// ����
class Guaiwu : public BaseHero
{
private:
	GuaiWuData mData;
	bool mSpecialTriggerFlag;
protected:
	virtual void setCurrentHp(unsigned int hp);
	// ֻ���ڼ��ܵģ�������û�ã�
	// ���㹥�����˺�������PropertyCalculator����ֱ����Guaiwu* �����
	virtual CalculateData getCalculateDataForSkill() const;
public:
	void initWithGuaiwuData(GuaiWuData data);

	GuaiWuData getProperty()const{return mData;};

	virtual Profession getProfession() const;
	virtual HeroBaseType getHeroType() const;
	virtual int getXSize() const;	// ��ȡX�����ϵĴ�С,����һ������ռ3���м�����
	virtual float getGongJiJuLi()const;			// ��������
	virtual int getGongJiLi() const;			// ������
	virtual float getGongJiLi(ValueRange range) const;			// ������
	virtual int getLevel() const;						// ��ȡ�ȼ�
	virtual int getStage() const;
	virtual int getHp() const;
	virtual unsigned int getMinJie() const;
	virtual unsigned int getBaoJiLv() const;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getHuJiaJianMian() const;		// ���׼��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getKangXingJianMian() const;	// ���Լ��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getBaoJiShangHaiYinZi() const;	// �����˺����ӣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaLv() const;		// ���ܴ����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaZhouQi() const;	// ���ܴ������ڣ���ÿ����ô���һ�μ���
	virtual unsigned int getJiNengChuFaTurn() const;	// ���ܴ����㣬��һ�δ������ܵĵ�
	virtual unsigned int getJiNengResult() const;		// ���ܵ�Ч�������ܵ��˺���Ҫ�������Ч���ӳɡ� �õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getShanBiLv() const;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	// ��ȡ����targetʱ�������˺�ֵ
	virtual float getShangHaiValue(AbstractItem* target,ValueRange range = Range_Ramdom) const;
	virtual int getShangHaiLeiXing()const;				// ��ȡ�˺�����
	
	//�������ⴥ����¼
	virtual bool getSpecialTrigger() const;
	virtual void setSpecialTrigger();
};

// Ӣ�ۣ�����Client�˵�BaseActorһ��
class Hero : public BaseHero
{
	// ����
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

	// ����Ԫ����Ҫ��zhuangbeiOpEnd֮ǰ����
	// ��Ȼ������zhuangbeiOpEnd�����ݾ�׼������..
	// ���Զ�delete
	void setYuanShen_AutoDeleted(Hero* yuanshen);

	// װ���Ĳ�������ˣ���Ҫ�����������������ˢ��Ѫ��
	// ���ָ����currentHp����ô�ͻ�ѵ�ǰѪ�����ó�ָ����Ѫֵ
	void zhuangbeiOpEnd(int curHp = -1);
	bool isOpEnd(){return equipOpEnd;};



	// ֻ��
	const HeroBaseProperty* getHeroBaseProperty_R() const {return &pro;};

	// ֻ��Hero�ſ��������getCalculateData�����������Ķ���Assert��
	virtual CalculateData getCalculateData() const;

	// װ�������ϵ�װ��
	vector<EquipData> equipVec;

	// ��ֵ̬�Ļ�ȡ����Ҫͨ��getHeroBaseProperty_*������ȡ
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
	virtual float getGongJiJuLi()const;			// ��������
	virtual float getGongJiLi(ValueRange range) const;	// ������
	virtual int getLevel() const;						// ��ȡ�ȼ�
	virtual int getStage() const;
	virtual unsigned int getBaoJiLv() const;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getHuJiaJianMian() const;		// ���׼��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getKangXingJianMian() const;	// ���Լ��⣬�õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getBaoJiShangHaiYinZi() const;	// �����˺����ӣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaLv() const;		// ���ܴ����ʣ��õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getJiNengChuFaZhouQi() const;	// ���ܴ������ڣ���ÿ����ô���һ�μ���
	virtual unsigned int getJiNengChuFaTurn() const;	// ���ܴ����㣬��һ�δ������ܵĵ�
	virtual unsigned int getJiNengResult() const;		// ���ܵ�Ч�������ܵ��˺���Ҫ�������Ч���ӳɡ� �õ�ʱ����Ҫ�Լ�����100
	virtual unsigned int getShanBiLv() const;			// �����ʣ��õ�ʱ����Ҫ�Լ�����100
	// ��ȡ����targetʱ�������˺�ֵ
	virtual float getShangHaiValue(AbstractItem* target,ValueRange range = Range_Ramdom) const;
	virtual int getShangHaiLeiXing()const;				// ��ȡ�˺�����
};

// �ű��������ṩ��
class ScriptDataProvider
{
public:
	ScriptDataProvider(void);

	// ��ѯ���ݿ⣬������ݵ�load������ģ��ս������
	// ��������Ҫ���ĵģ�����,����Ҫ��,
	// ����ǹ����new GuaiWu��Ȼ���ʼ�����ؼ���
	// �����Ӣ�ۣ���ô��new Hero Ȼ���ʼ�����ؼ��ɣ����û���ˣ���ô�ͷ���NULL
	virtual BaseHero* getBaseHero(unsigned int _heroId,ScriptBuilderUserData data) = 0;

	// ��ȡ_heroId��Ԫ������ݣ�û�о�ֱ�ӷ���NULL����
	// Ĭ�Ͼ��Ƿ���NULL
	virtual BaseHero* getBaseHeroYuanShen(unsigned int _heroId,ScriptBuilderUserData data){return NULL;};

	// ����Ŷӵ�Ѫƿ����
	virtual void getTeamBloodNum(short& currentNum,short& defaultNum,int& bloodAddPercent,ScriptBuilderUserData data) = 0;
	// ��ȡ����Ѫƿ֮���cdʱ��
	virtual void getHeroCDTime(short& cdStepNum,unsigned heroId,ScriptBuilderUserData data) = 0;

	// ��ѯ���ݿ⣬������ݵ�load�����ڿͻ������ݵ�ˢ��
	// ���ظ��ͻ���
	// ��������Ҫ���ĵģ�����
	virtual void getHeroDetail(HeroDetail& outData,unsigned int _heroId,ScriptBuilderUserData data) = 0;

	// ս����ϣ����������ݳ־û�,���صõ��ľ���ֵ
	virtual unsigned int fightEndCallback(ScriptBuilderUserData data,bool win) = 0;


	// ��ȡ�����Item��Ƭ
	virtual vector<ItemSuipianInfo> getFightEndItemSuiPians(ScriptBuilderUserData data,bool win) = 0;
	// ��ȡ����������Ƭ
	virtual vector<ShenbingSuipianInfo> getFightEndShenBingSuiPians(ScriptBuilderUserData data,bool win) = 0;
	// ��ȡ����װ��
	// �������������
	virtual vector<EquipData> getFightEndEquips(ScriptBuilderUserData data,bool win) = 0;
	// ��ȡ������Ʒ
	// �������������
	virtual vector<ItemInfo> getFightEndItems(ScriptBuilderUserData data,bool win)= 0;
	// ��ȡ�õ���������
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
