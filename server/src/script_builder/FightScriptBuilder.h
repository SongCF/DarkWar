#pragma once

#include "protocol.h"
#include "FightScriptDefine.h"
#include "ScriptDataProvider.h"
#include "ChessBoardHelper.h"


// 战斗脚本的生成器 
// 用于生成两方的战斗的过程，和结果
// 包括：
// 第n步，谁攻击谁，攻击方式是什么(技能，普攻，暴击,这些方式主要是用于确定攻击和受攻动画用的),
//		 攻击时，谁的血量减少多少(或者重置为多少),谁在这一步死亡，谁在这一步战斗成功或失败。
//		 需要怎么移位(谁移动到哪个位置，这个有了就可以不需要另一个可能的攻击/移位)
// 第n步，可能不是攻击，而是移位(但是要考虑到时把移位和攻击和在一块)
//// 第n步，bufftime
//
//
class FightScriptBuilder
{
private:   
	int getBattel_FightScriptLength(const FightScript* A_script);

	// 把一个Battel_FightScript的数据结构转化成一个FightScript
	FightScript Battel_FightScript_2_FightScript(const char* data);

	// 把一个FightScript转化成一个Battel_FightScript的char*
	char* FightScript_2_Battel_FightScript(FightScript f_script,unsigned long& length);

	// 根据填充outData数据
	// item 就是该Id的棋盘角色，如果为NULL，那么outData就会初始化为无效值
	// outData的数据都是直接来源于item，只有id是直接把参数Id赋值给他，还有就是hero_type需要查询数据库
	void fillHeroBaseDataByChessBoardItem(unsigned heroId,HeroBase& outData,const ChessBoardItem* item);

	void fillHeroBaseDataByAbstractItem(unsigned heroId,HeroBase& outData,const AbstractItem* item);
public:
	FightScriptBuilder(void);

	// 生成 A_team 和 B_team的作战剧本
	// A 和 B 的脚本的初始化和结果是不同的，但过程是公用一个过程
	// A_team : A 队的布阵数据A_team[0]就是站在0号位的A对人员的Id,0就是没有人。要完整的4个人
	// B_team : B 队的...
	// A_script : 生成的A队的脚本数据
	// B_script : 生成的B队的脚本数据
	HeroState genScript(
		unsigned int A_team_id[],unsigned int B_team_id[],
		FightScript& A_script,FightScript& B_script,
		ScriptDataProvider* provider,
		ScriptBuilderUserData userData,
		FirstBloodBoard firstBloodBoard,
		int huiHeLimite = -1 //huiHeLimite 控制回合的次数，如果回合次数大于这数据，A方就失败，-1就是不限制
		);


	// 生成团队打Boss的剧本
	// TeamHero* 需要外界自己delete
	void genTeamFightScript(
		TeamHero* hero[FormationHeroAcountMax],
		Guaiwu* boss,
		vector<FightScript>& script_vec_4,
		TeamEndParam& endData);


	// 将A_scriptVec的数据转换好字节序，存放在char的数组中
	// 数组的长度放在length中,A_scriptVec没有就返回NULL，length就为0
	char* FightScriptVec_2_CharArr(vector<FightScript> A_scriptVec,int & length);
	// 将char数组的网络字节序的脚本，转化成本地字节序，并存放在FightScript里面
	// 需要告诉内部，这里面存放了有一个脚本，才可以解析
	vector<FightScript> CharArr_2_FightScriptVec(const char* scirpt_char_arr,int fightScriptNum);


	// 只针对SPCmd_BattleRsp包
	// 把FightScript转化网络封包，里面的data段就是转化成网络字节序的SPCmd_BattleRsp指针
	//char* FightScript_2_Package(FightScript* A_script);
	// 没有紧紧的爱着 放置 FightScript，而是有间隔的：getSPCmd_BattleRspLength可以计算
	char* FightScript_2_Package(vector<FightScript> A_scriptVec);
	// 只针对SPCmd_BattleRsp包
	// 把SPCmd_BattleRsp的数据结构(网络字节序)转换成FightScript本地字节序
	// rspCode就是SPCmd_BattleRsp的code
	// 如果不是Success，那么vector是空值
	vector<FightScript> SPCmd_BattleRsp_2_FightScript(const char* data,int& rspCode);

	// 把actor的综合数据转化成TeamBaseProperty
	TeamBaseProperty combineActorProperty(Hero* actor);

	~FightScriptBuilder(void);
};

