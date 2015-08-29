#pragma once

#include "cocos2d.h"
#include "FightScriptDefine.h"
#include "ClientChessBoardItem.h"
#include "cocos-ext.h"
using namespace cocos2d;
using namespace cocos2d::extension;


#define MSGFight_Skill_Prepare	  "msgFight_UseSkillPrepare"
#define MSGFight_Skill_PrepareEnd "msgFight_UseSkillPrepareEnd"
#define MSGFight_Skill_UseEnd     "msgFight_UseSkillEnd"



// 对应MSG_Fight
class MSG_SOUND : public CCObject
{
public:
	string sound_file;
	MSG_SOUND()
	{
		sound_file = "";
	}
	MSG_SOUND(string sound_file)
	{
		this->sound_file = sound_file;
	}
};
class MSG_Skill : public CCObject
{
public:
	StepSkill step;
	ClientChessBoardItemBase* fightor;
	bool playDefaultHithedSound;
	string hited_sound;		// 指定新的hited声音,默认没有，有的话就会播放这个声音，default该播放还是会播放

	MSG_Skill()
	{
		playDefaultHithedSound = true;
		hited_sound = "";
	}
	// 看item是否是攻击目标
	// 是的话就把他对应的数据返回出来
	bool testTarget(const ClientChessBoardItemBase* item,SkillType& skill,int& bloodChange,bool& canMiss)
	{
		int num = step.targetUniqueVec.size();

		for (int i=0;i<num;i++)
		{
			//if (step.targetVec[i] == item->getHero()->heroId)
			if (step.targetUniqueVec[i] == item->getUniqueId())
			{
				skill = step.skillType;
				bloodChange = step.targetBloodChange[i];
				canMiss = step.canMissVec[i];
				return true;
			}
		}
		return false;
	}
};


// 客户端的技能处理器 （效果 + 动画 + 减血）
class ClientSkillProcessorBase : public CCObject 
{
protected:

	CCSize mWinSize;
	CCSize mVisibleSize;

	// 棋盘的访问方法
	virtual ClientChessBoardItemBase* getItemFromBoard(unsigned int _heroId) = 0;
	// 创建一个角色A攻击角色B时，A冲向B的一个动画
	// 当装到B时，会回调skill_Hit方法，进行减血等操作，但是需要MSG_Skill
	// 所以这里得传入.
	// time_cost:A 攻击到B时用时多少秒
	CCActionInterval* createAttackMovementAction(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* target,float delay,float& time_cost,MSG_Skill* callSkillHitData);
protected:
	// 得到StepSkill中一连串攻击目标里面的一个单元
	StepSkill getOneFightUniteByIndex(StepSkill step,int index);

	// 天赋技能
	void skill_fight_baoLieZhan(StepSkill step);
	void skill_fight_shaLu(StepSkill step);
	void skill_fight_shanDian(StepSkill step);
	void skill_fight_jianShou(StepSkill step);
	// 代替坚守技能的
	void skill_fight_shengCai(StepSkill step);
	void skill_fight_shengZhiXinNian(StepSkill step);
	void skill_fight_LiRenHuiXuan(StepSkill step);
	void skill_fight_lieGong(StepSkill step);
	void skill_fight_jianYu(StepSkill step);
	void skill_fight_huoQiuShu(StepSkill step);
	void skill_fight_wenYi(StepSkill step);

	// 技能书1
	void skill_fight_BanYueZhan(StepSkill step);
	void skill_fight_CiSha(StepSkill step);
	void skill_fight_LeiBao(StepSkill step);
	void skill_fight_MangCi(StepSkill step);
	void skill_fight_ShenZhiZhuFu(StepSkill step);
	void skill_fight_shunYinTuXi(StepSkill step);
	void skill_fight_JingZhunZhiCan(StepSkill step);
	void skill_fight_JiSuDuJian(StepSkill step);
	void skill_fight_BaoFengXue(StepSkill step);
	void skill_fight_KongJu(StepSkill step);

	// 技能书2
	void skill_fight_KuangBao(StepSkill step);
	void skill_fight_ShiXueZhiRen(StepSkill step);
	void skill_fight_YunShi(StepSkill step);
	void skill_fight_ShouHu(StepSkill step);
	void skill_fight_ZhongShenZhiLi(StepSkill step);
	void skill_fight_XuanFengZhan(StepSkill step);
	void skill_fight_ZhuiHunDuJian(StepSkill step);
	void skill_fight_SiWangSaoShe(StepSkill step);
	void skill_fight_HanBingCiGu(StepSkill step);
	void skill_fight_ShiHun(StepSkill step);

	// 技能书3
	void skill_fight_BaoNu(StepSkill step);
	void skill_fight_YinZheHun(StepSkill step);
	void skill_fight_TianTangZhiGuang(StepSkill step);


	// 怪物的
	void skill_fight_Monster_XieE(StepSkill step);

	// 击中，转掉，是攻击就通知Item
	// 是一般的buff或者不需要Item做表演的就不通知Item处理
	// 也就是选择性的通知
	void skill_Hit(CCNode* node,CCObject* msg_skill);
	//
	void playEffectSound(CCNode* node,CCObject* sound_data);

	// 技能发出者的默认的攻击动画
	// 这里就是快速的甩一下
	void runDefaultFightorAction(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* target,float delay = -1);



	//新技能展示
	void skill_fight_prepare(StepSkill step);
	//1. normal skill animate 
	void skill_fightArmatureBegin(CCNode *node, void* data);
	//2. special skill animate
	void skill_fightArmatureBegin_LiRenHuiXuan(CCNode *node, void* data);
	void skill_fightArmatureBegin_HuoQiuShu(CCNode *node, void* data);

	CCPoint getSkillFightorPosOffset(SkillType type, bool fightOrTarget);// true fightor

	//群攻每一个受伤延时播放
	void callback_playAnimation(CCNode* armature, void * name);
	//横幅播放完后的回调（用于移除横幅CCArmature）
	void callfunc_RemoveWhenAnimateEnd(CCArmature *, MovementEventType, const char *);
	//技能播放完后的回调（用于移除技能CCArmature）
	void callfunc_postSkillEnd(float dt);

public:
	// 
	ClientSkillProcessorBase();

	// 技能攻击
	void skill_fight(StepSkill step);

	// 处理反弹
	void dealWithFanTan(vector<Buff_Doing> fantan,float actionDelay);

	~ClientSkillProcessorBase(void);
};

