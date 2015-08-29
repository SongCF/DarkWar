#pragma once

#include "BaseSprite.h"
#include <vector>
#include "FightScriptDefine.h"
#include "cocos-ext.h"

using namespace cocos2d::extension;

using namespace std;

class ClientChessBoard;
class ClientChessBoard_TeamFight;
class ClientChessBoardItem;
class ClientChessBoardItemBase;
class IChessboardProperty;

#define TAG_CD_TIME			1122312
#define TAG_FIRE_POINT		2123981		// item上面发动攻击的点，就是比如技能，烈弓等的发射点
#define TAG_CENTER_POINT	2123982		// item上面中心点的坐标

// 攻击消息
class MSG_Fight : public CCObject
{
private:
	MSG_Fight()
	{
		fightor = NULL;
	}
public:
	// 一打一的状态
	static MSG_Fight* create(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* beAttacked,int hpToSub,bool canMiss)
	{
		MSG_Fight* msg = new MSG_Fight();
		msg->autorelease();
		msg->fightor = fightor;
		msg->beAttackedVec.push_back(beAttacked);
		msg->hpToSubVec.push_back(hpToSub);
		msg->canMissVec.push_back(canMiss);

		return msg;
	}
	// 一打N的状态
	static MSG_Fight* create(ClientChessBoardItemBase* fightor,vector<ClientChessBoardItemBase*> beAttacked,vector<int> hpToSub,vector<bool> canMiss)
	{
		MSG_Fight* msg = new MSG_Fight();
		msg->autorelease();
		msg->fightor = fightor;
		msg->beAttackedVec = beAttacked;
		msg->hpToSubVec = hpToSub;
		msg->canMissVec = canMiss;

		return msg;
	}

	// 只保留第一个target数据
	void keepSingle()
	{
		ClientChessBoardItemBase* target = beAttackedVec[0];
		int bloodSub = hpToSubVec[0];
		bool canMiss = canMissVec[0];

		beAttackedVec.clear();
		hpToSubVec.clear();
		canMissVec.clear();

		beAttackedVec.push_back(target);
		hpToSubVec.push_back(bloodSub);
		canMissVec.push_back(canMiss);
	}

	// 从MSG_Fight中的多个攻击单元里面得到第index个攻击单元
	// 因为不能把多个攻击单元的MSG_Fight直接拿去用，必须的拆分出来用
	MSG_Fight* createOneFightUniteByIndex(int index)
	{
		MSG_Fight* msg = new MSG_Fight();
		msg->fightor = fightor;
		msg->beAttackedVec.push_back(beAttackedVec[index]);
		msg->hpToSubVec.push_back(hpToSubVec[index]);
		msg->canMissVec.push_back(canMissVec[index]);
		return msg;
	}

	// 是否item是被攻击的对象，是的话就是true，并把要减血的值放在out_bloodSub中
	bool isBeAttacked(ClientChessBoardItemBase* item,int& out_bloodSub,bool& out_canMiss)
	{
		int num = beAttackedVec.size();
		for (int i=0;i<num;i++)
		{
			if (item == beAttackedVec[i])
			{
				out_bloodSub = hpToSubVec[i];
				out_canMiss = canMissVec[i];
				return true;
			}
		}
		return false;
	}

	ClientChessBoardItemBase* fightor;
	vector<ClientChessBoardItemBase*> beAttackedVec;
	vector<int> hpToSubVec;
	vector<bool> canMissVec;
};

// 反弹的消息
class MSG_FanTan : public CCObject
{
public:
	Buff_Doing fantan;
	ClientChessBoardItemBase* target;
	ClientChessBoardItemBase* fightor;
	bool playDefaultHithedSound;
	string hited_sound;		// 指定新的hited声音,默认没有，有的话就会播放这个声音，default该播放还是会播放

	MSG_FanTan()
	{
		playDefaultHithedSound = true;
		hited_sound = "";
	}
};

enum ClientChessBoardItemPart
{
	Part_All = 0,
	Part_Body = 1,	// 身体，除了底座部分，其他的全算
	Part_Dizuo = 2,	// 底座的部分，包括座子，阴影，血条等，Part_Body+Part_Dizuo就是全部数据
};

#define	FIGHT_EVN_MAIN_LINE		1
#define	FIGHT_EVN_OTHER			-1

enum HPChangePattern
{
	Sub_Normal = 0,
	Sub_BaoJi,
	Add_Normal,
};

// 客户端棋盘元素的基类
// 封装了所有的动作动画
// 血量和buff的处理
class ClientChessBoardItemBase : public BaseSprite
{
public:
	ClientChessBoardItemBase(HeroBase hero,unsigned int uniqueId);
	~ClientChessBoardItemBase();
	//
	virtual BaseSprite* getContainer(){return container;};
	// 获取架子的Sprite
	// 因为一个Item分为：底座，架子，和架子上面的装备叠加
	BaseSprite* getJiaZi();
	BaseSprite* getDizuo();
	BaseSprite* getBloodBarBg();
	virtual const HeroBase* getHero()const{return &hero;};
	virtual unsigned int getUniqueId() const {return uniqueId;};

	virtual vector<CCSprite*> getItemParts(ClientChessBoardItemPart partFilter = Part_All);

	// 是否是双手，是的话就返回true，不是就返回false
	bool isShuangshou();

	// 重写，让这个Item的透明度可以传到全身相的Item上
	virtual void setOpacity(GLubyte opacity);
	virtual void setColor(const ccColor3B& color3);

	virtual void setFightEnviroment(int env){fight_env = env;};
public:// 作战接口
	// 放入反弹的数据，那么在被攻击的时候，就会将这个反弹数据反弹回去
	// 每次反弹只会反弹一个
	virtual void pushFanTanData(Buff_Doing fantan);
	// 添加一次可以显示守护盾的次数，是skill处理的时候在buff
	void addShowShouHuDuanNum(int toAdd);

	// 做buff生效的效果
	void runBuffDoingEffect(vector<Buff_Doing> doingVec);

	// 获取当前所在Board的属性数据
	virtual IChessboardProperty* getCurrentBoardProperty() = 0;

	virtual void hpTo(int hp);
	virtual void hpAdd(int toAdd);
	virtual void hpSub(int toSub,HPChangePattern pattern = Sub_Normal);
	virtual int getCurrentHp(){return hero.hp;};
	virtual Profession getProfession(){return hero.profession_id;};
	// 攻击
	virtual void attack_UseNormal(MSG_Fight* data);
	virtual void attack_UseNormalBaoJi(MSG_Fight* data);
	// buff
	// 注意，加buff的时间一定要比移除buff的时间早
	// 不然就有buff
	virtual void removeBuffById(int _id);
	virtual void attachBuff(ClientGameBuff buff);
	// 获得正常情况下的位置
	// 也就是container的中心
	virtual CCPoint getNormalPos();
	// 获取发技能的地点
	// 默认就是卡牌的中心点
	// 但是由于现在的卡牌需要发射点在上半身，于是需要移动一下即可
	virtual CCPoint getSkillFirePos();
	// 获得发射技能等的发射点的Node，就是放在SkillFirePos的一个Node
	CCNode* getFireNode();
	// 获得中心的Node
	CCNode* getCenterNode();
protected:
	virtual void runBloodChangeAction(int num,HPChangePattern pattern);
	virtual void refreshBloodBar(float delay);
	virtual void refreshBuffIcon();
	virtual void runShouShangAction();
	virtual void showDeadImage();
	virtual void showAliveImage();

	virtual void light_on();
	virtual void light_off();

	// 攻击动画
	virtual void attackAction_Normal(MSG_Fight* data,bool playFightorAction = true);
	virtual void attackeAction_NormalBaoJi(MSG_Fight* data,bool playFightorAction = true);
	void attackeAction_Normal_boss(MSG_Fight* data,bool playFightorAction = true);
	void callfunc_RemoveWhenAnimateEnd(CCArmature *, MovementEventType, const char *);

	// 攻击击中 --> 攻击动画完毕，用于告示外界，被攻击对象可以减血，可以显示受攻击动画了
	// 转调 被攻击代码
	virtual void attackUseNormal_Hit(CCNode* node,void* msg_fight);
	virtual void attackUseNormalBaoJi_Hit(CCNode* node,void* msg_fight);

	// 被击中了，包括动画和减血
	virtual void hitedByNormalFight(CCObject* msg_fight);
	virtual void hitedByNormalBaoJiFight(CCObject* msg_fight);

	// 尝试反弹攻击
	// 如果可以反弹就会反弹的
	virtual void tryFanTan(ClientChessBoardItemBase* fightor);
	virtual void fanTan_MangCi(Buff_Doing data,ClientChessBoardItemBase* target);
	virtual void fantan_hit(CCNode* node,void* msg_fantan);
public:
	// 公共动画
	//
	virtual void missAction(ClientChessBoardItemBase* target);
	// 抓痕
	virtual void zhuaHen(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* attacked,int num);
	// 被打退
	virtual void shake_attacked(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* attacked);
	// 发射东西时的后座摇晃
	virtual CCFiniteTimeAction* get_fire_shake_Action(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* attacked,float offset = 15);
	//
	virtual CCFiniteTimeAction* getShake(CCNode* node,CCPoint offset);

	virtual CCProgressTimer* getBloodBar(){return mBloodProgress;};

protected:
	CCProgressTimer* mBloodProgress;
	int defaultHP;
	unsigned int uniqueId;
	BaseSprite* container;
	vector<int> remove_buff_id;		// 记录要求移除的buff的id，主要是为了克服加buff和移除buff的不同步
	list<ClientGameBuff> attachedBuff;	// 身上的buff，这里不做任何计算只用作于buff的标记，因为服务器已经算好了
	// 数据
	HeroBase hero;
	// 存放全身相的各个组件
	// 底座，架子，装备
	CCArray* full_photo_parts;
	vector<Buff_Doing> canFanTanData;	// 可以反弹的数据
	// 可以显示守护盾的次数，但唯一可能引起bug的情况是
	// A，B攻击我，我的显示盾的次数只有1，那么如果本来是该A打我时，我显示盾
	// 但是结果B先打到我，那么就把盾显示给了B
	// 但是这种情况不应该发生，因为我们要做时间，暂停控制等。
	int showShouHuDuanNum;		

	// 主线：1	其他就是其他
	// 默认-1
	int fight_env;	// 主线，其他

	bool mIsShuangshou;
};

// PVE/PVP的item
class ClientChessBoardItem : public ClientChessBoardItemBase
{
public:
	ClientChessBoardItem(ClientChessBoard* originBoard,HeroBase hero,unsigned int uniqueId);
	~ClientChessBoardItem();
	//
	void setCurrentBoard(ClientChessBoard* board);
	void setCurrentPos(int gridX,int gridY);
	int getGridX();
	int getGridY();
	ClientChessBoard* getCurrentBoard();
	ClientChessBoard* getOriginBoard();
	virtual CCPoint getSkillFirePos();
	
public:
	int getCurrentBloodCdTime();
	void setCurrentBloodCdTime(int time);
	// 清空血瓶
	void clearBlood();

protected:
	virtual void showDeadImage();
	void showMuBei(CCNode* node);

	// 获取当前所在Board的属性数据
	virtual IChessboardProperty* getCurrentBoardProperty();

protected:
	int gridX;
	int gridY;
	unsigned int currentCDTime;		// 当前所处的cd时间，0就代表不是cd时间了，就可以用血瓶了
// 	CCProgressTimer* cdBar;
// 	BaseSprite* hightLine;

	ClientChessBoard* originBoard;
	ClientChessBoard* currentBoard;
};

// 
class ClientChessBoardItem_TF : public ClientChessBoardItemBase
{
public:
	ClientChessBoardItem_TF(HeroBase hero,unsigned int uniqueId);
	~ClientChessBoardItem_TF();
	//
public:

	void setCurrentBoard(ClientChessBoard_TeamFight* board);


	int getCurrentFreesyTime();
	virtual CCPoint getSkillFirePos();
	void setCurrentFreesyTime(int time);

	// 重写用于复活
	virtual void hpAdd(int toAdd);

	// 获取当前所在Board的属性数据
	virtual IChessboardProperty* getCurrentBoardProperty();

protected:
	unsigned int freesyNum;		
	ClientChessBoard_TeamFight* currentBoard;
};
