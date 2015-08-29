#pragma once
#include <stdlib.h>
#include "protocol.h"

struct Guess
{
	bool gussed;

	HeroInfo hero_a;
	HeroInfo hero_b;
	unsigned int i_guess_winner_is; // 0:就是A，1就是B
	unsigned int my_wager;			// 我的赌注，银币数量

	unsigned int am_i_right;		// 我猜对没-->0:结果还未出来 1: 竞猜正确 2: 竞猜错误

	Guess()
	{
		memset(this,0,sizeof(Guess));
	}

};

// 英雄的身价
struct HeroValue
{
	unsigned int heroId;
	unsigned int round;
	unsigned int value;

	HeroValue()
	{
		heroId = 0;
		round = 0;
		value = 0;
	}
};

class DianFengDuiJueManager
{
public:
	enum Round
	{
		Round_8_4 = 0,				// 8进4，下标值明确为0
		Round_4_2 = 1,				// 4进2，下标值明确为1
		Round_2_1 = 2,				// 2进1，下标值明确为2
		Round_AllFinished = 3,		// 全部完毕了，可以看上次的数据,下标值明确为3
		Round_Not_inited,			// 完全没开启活动,什么数据都是空的
	};

private:
	static DianFengDuiJueManager* mInstance;

	bool inited;

	// 如果还没开始，那么着就是开始的时间，若已经开始，那么这就是0
	unsigned int next_top_arena_time;
	// 我的竞猜，gussed标记当前有没有猜
	//下标0为8进4， 下标1为4进2， 下标2为2进1
	// 跟Round的枚举直接对应
	Guess my_guess[3];

	/* 最多3组竞猜消息  TopArenaStat*/
	// 如果都是Invalid，那么就是开服还没打过
	// 跟Round的枚举直接对应
	// 下标0为8进4， 下标1为4进2， 下标2为2进1
	TopArenaStat stat[3];			
	TopArenaInfo player_arr_8_4[4];	//8进4选手
	TopArenaInfo player_arr_4_2[2]; //4进2选手
	TopArenaInfo player_arr_2_1[1]; //2进1选手

	vector<HeroValue> heroValues;

	DianFengDuiJueManager(void);

	// 在all_group中看是否有a和b战斗，并且a战胜了的
	bool is_a_overcome_b(const HeroInfo& hero_a,const HeroInfo& hero_b,const TopArenaInfo* all_group,int count);
	// 在这里面是否赢过
	bool is_hero_win_in_group(const HeroInfo& hero,const TopArenaInfo* all_group,int count);

	// 获取heroId的组号，没有就是-1
	int getGroupIndexInRound(int heroId,const TopArenaInfo* all_group,int count);

public:
	static DianFengDuiJueManager* getManager();
	void setCurrentStatus(SPCmd_TopArenaQueryStatusRsp status);
	void updateGuessData(unsigned short groupIndex,bool guess_winer,unsigned int wager);
	// 设置某组英雄的价值
	// 如果当前round不正确，就会asert
	void setHeroValueInCurentRound(unsigned int moneyA,unsigned int moneyB,int groupIndex);

	// 返回我的竞猜
	vector<Guess> myGuessData();

	// 获取某场的竞猜
	// 就猜就返回true，并把数据填在outData里面
	// 没有就返回false
	bool getGuess(Guess& outData,Round round);

	// 获取英雄在当场竞猜中的的身价
	unsigned int getHeroValueInCurrentRoundByGroupIndex(unsigned int heroId);
	//
	Round getCurrentRound();
	// 获取开启时间
	unsigned int getOpenTime();

	// 获取某场比赛某组的对打的两个人
	// 在8->4中，index可以有4个取值：0,1,2,3
	// 在4->2中，index可以有2个取值：0,1
	// 在2->1中，index可以有1个取值：0
	// 
	// 界面上布局时，就可以调用这个方法，挨个调用即可：
	// getOneGroup(0,Round_8_4);getOneGroup(1,Round_8_4);
	// getOneGroup(2,Round_8_4);getOneGroup(3,Round_8_4);
	// 就可以把人物布局出来
	// 至于谁赢谁胜，就调用is_a_overcome_b判断
	// 还有更简单的方式实现"绿线"的点亮，直接调用：is_hero_win_in_round
	pair<HeroInfo,HeroInfo> getOneGroup(int groupIndex,Round round);
	// 获取当前场次的某组对手
	pair<HeroInfo,HeroInfo> getOneGroupByCurrentRound(int groupIndex);

	// 获取heroId在round中的组号
	// 没有就是-1
	int getGroupIndexInRound(int heroId,Round round);

	// 在所有的组里面找，是否a打赢了b
	// 打赢了就返回true
	// 其他情况，全部返回false
	// 
	// 该函数可能用处不大，is_hero_win_in_round就可以解决界面问题
	// 
	// 例如：
	// 如果a和b根本就没打过，那么返回都是false,如下：
	//  is_a_overcome_b(a,b) == is_a_overcome_b(b,a) == false
	bool is_a_overcome_b(HeroInfo hero_a,HeroInfo hero_b);

	// 判断某个hero是否在某场赢了
	// 这个可以很方便的实现拓扑图的走线
	bool is_hero_win_in_round(HeroInfo hero_a,Round round);

	// 我是否猜过当前场次了
	bool am_i_guessed();

	// 是否groupIndex有效
	// 也会校验round是否填错
	bool is_group_index_valide_in_round(unsigned short groupIndex,Round round);

	~DianFengDuiJueManager(void);
};

