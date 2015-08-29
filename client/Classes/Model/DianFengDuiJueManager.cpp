#include "DianFengDuiJueManager.h"
#include "CS_Common.h"

DianFengDuiJueManager* DianFengDuiJueManager::mInstance = NULL;

DianFengDuiJueManager::DianFengDuiJueManager(void)
{
	inited = false;
}

DianFengDuiJueManager* DianFengDuiJueManager::getManager()
{
	if (mInstance == NULL)
	{
		mInstance = new DianFengDuiJueManager();
	}
	return mInstance;
}

void DianFengDuiJueManager::setCurrentStatus(SPCmd_TopArenaQueryStatusRsp status)
{
	memcpy(stat,status.stat,sizeof(stat));
	next_top_arena_time = status.next_top_arena_time;

	for (int i=0;i<3;i++)
	{
		my_guess[i].gussed = status.guess[i].is_joined;
		if (!my_guess[i].gussed) // 没猜，下面的就不用看了
			continue;

		my_guess[i].hero_a = status.guess[i].player.hero_a;
		my_guess[i].hero_b = status.guess[i].player.hero_b;
		my_guess[i].i_guess_winner_is = status.guess[i].guess_winer;
		my_guess[i].my_wager = status.guess[i].wager;
		my_guess[i].am_i_right = status.guess[i].result;
	}

	memcpy(player_arr_8_4,status.player_arr_8_4,sizeof(player_arr_8_4));
	memcpy(player_arr_4_2,status.player_arr_4_2,sizeof(player_arr_4_2));
	memcpy(player_arr_2_1,status.player_arr_2_1,sizeof(player_arr_2_1));

	inited = true;
}

void DianFengDuiJueManager::updateGuessData(unsigned short groupIndex,bool guess_winer,unsigned int wager)
{
	int round = getCurrentRound();
	if (!is_group_index_valide_in_round(groupIndex,(DianFengDuiJueManager::Round)round))
	{
		CCAssert(false,"groupIndex Error");
		return;
	}
	if (wager != CS::getTopArenaWager(1) && wager != CS::getTopArenaWager(2))
	{
		CCAssert(false,"wager Error");
		return;
	}

	if (round == Round_2_1)
	{
		my_guess[round].hero_a = player_arr_2_1[groupIndex].hero_a;
		my_guess[round].hero_b = player_arr_2_1[groupIndex].hero_b;
	}
	else if (round == Round_4_2)
	{
		my_guess[round].hero_a = player_arr_4_2[groupIndex].hero_a;
		my_guess[round].hero_b = player_arr_4_2[groupIndex].hero_b;
	}
	else if (round == Round_8_4)
	{
		my_guess[round].hero_a = player_arr_8_4[groupIndex].hero_a;
		my_guess[round].hero_b = player_arr_8_4[groupIndex].hero_b;
	}

	my_guess[round].gussed = true;
	my_guess[round].i_guess_winner_is = guess_winer?0:1;
	my_guess[round].my_wager = wager;
	my_guess[round].am_i_right = 0;
}

void DianFengDuiJueManager::setHeroValueInCurentRound(unsigned int moneyA,unsigned int moneyB,int groupIndex)
{
	Round round = getCurrentRound();
	if (!is_group_index_valide_in_round(groupIndex,round))
	{
		return;
	}

	unsigned int heroIdA = getOneGroup(groupIndex,round).first.hero_id;
	unsigned int heroIdB = getOneGroup(groupIndex,round).second.hero_id;

	// 有就更新
	bool heroAFounded = false;
	bool heroBFounded = false;
	for (int i=0,count=heroValues.size(); i<count; i++)
	{
		if (heroValues[i].round == round && heroValues[i].heroId == heroIdA)
		{
			CCAssert(heroAFounded==false,"");
			heroValues[i].value = moneyA;
			heroAFounded = true;
		}
		else if (heroValues[i].round == round && heroValues[i].heroId == heroIdB)
		{
			CCAssert(heroBFounded==false,"");
			heroValues[i].value = moneyB;
			heroBFounded = true;
		}
	}

	// 按照正确逻辑，要不都找到，要不都没找到
	CCAssert( (heroAFounded && heroBFounded) || (!heroBFounded && !heroAFounded),"");
	if (heroAFounded)
	{
		return;
	}

	// 新增
	HeroValue hvA;
	hvA.heroId = heroIdA;
	hvA.round = round;
	hvA.value = moneyA;

	HeroValue hvB;
	hvB.heroId = heroIdB;
	hvB.round = round;
	hvB.value = moneyB;

	heroValues.push_back(hvA);
	heroValues.push_back(hvB);
}

unsigned int DianFengDuiJueManager::getHeroValueInCurrentRoundByGroupIndex(unsigned int heroId)
{
	Round round = getCurrentRound();
	for (int i=0,count=heroValues.size(); i<count; i++)
	{
		if (heroValues[i].heroId == heroId && heroValues[i].round == round)
		{
			return heroValues[i].value;
		}
	}
	return 0;
}

DianFengDuiJueManager::Round DianFengDuiJueManager::getCurrentRound()
{
	CCAssert(inited,"");

	Round ret = Round_Not_inited;
	if(stat[0] == TopArenaStat_Invalide)
	{
		CCAssert(stat[1] == TopArenaStat_Invalide && stat[2] == TopArenaStat_Invalide,"");
		ret = Round_Not_inited;
	}
	else if (stat[0] == TopArenaStat_GuessOpen)
	{
		CCAssert(stat[1] == TopArenaStat_Invalide && stat[2] == TopArenaStat_Invalide,"");
		ret = Round_8_4;
	}
	else if (stat[1] == TopArenaStat_GuessOpen)
	{
		CCAssert(stat[0] == TopArenaStat_Finished && stat[2] == TopArenaStat_Invalide,"");
		ret = Round_4_2;
	}
	else if (stat[2] == TopArenaStat_GuessOpen)
	{
		CCAssert(stat[0] == TopArenaStat_Finished && stat[1] == TopArenaStat_Finished,"");
		ret = Round_2_1;
	}
	else if (stat[2] == TopArenaStat_Finished)
	{
		CCAssert(stat[0] == TopArenaStat_Finished && stat[1] == TopArenaStat_Finished,"");
		ret = Round_AllFinished;
	}

	return ret;
}

unsigned int DianFengDuiJueManager::getOpenTime()
{
	CCAssert(inited,"");

	return next_top_arena_time;
}

bool DianFengDuiJueManager::getGuess(Guess& outData,Round round)
{
	//CCAssert(round == Round_2_1 || round == Round_4_2 || round == Round_8_4,"");
	if (round != Round_2_1 && round != Round_4_2 && round != Round_8_4)
	{
		CCLog("Warring : %s --> round = %d",__FUNCTION__,round);
	}
	

	if (my_guess[round].gussed)
	{
		outData = my_guess[round];
		return true;
	}
	return false;
}
// 返回我的竞猜
vector<Guess> DianFengDuiJueManager::myGuessData()
{
	vector<Guess> ret;

	for (int i=0;i<sizeof(my_guess)/sizeof(Guess); i++)
	{
		if (my_guess[i].gussed)
		{
			ret.push_back(my_guess[i]);
		}
	}

	return ret;
}

pair<HeroInfo,HeroInfo> DianFengDuiJueManager::getOneGroup(int groupIndex,Round round)
{
	CCAssert(inited,"");

	HeroInfo hero1;
	HeroInfo hero2;

	switch (round)
	{
	case DianFengDuiJueManager::Round_8_4:
		CCAssert(groupIndex >= 0 && groupIndex < 4,"");
		hero1 = player_arr_8_4[groupIndex].hero_a;
		hero2 = player_arr_8_4[groupIndex].hero_b;
		break;
	case DianFengDuiJueManager::Round_4_2:
		CCAssert(groupIndex >= 0 && groupIndex < 2,"");
		hero1 = player_arr_4_2[groupIndex].hero_a;
		hero2 = player_arr_4_2[groupIndex].hero_b;
		break;
	case DianFengDuiJueManager::Round_2_1:
		CCAssert(groupIndex == 0,"");
		hero1 = player_arr_2_1[groupIndex].hero_a;
		hero2 = player_arr_2_1[groupIndex].hero_b;
		break;
	default:
		CCAssert(false,"");
		break;
	}

	return make_pair(hero1,hero2);
}
pair<HeroInfo,HeroInfo> DianFengDuiJueManager::getOneGroupByCurrentRound(int groupIndex)
{
	return getOneGroup(groupIndex,getCurrentRound());
}

int DianFengDuiJueManager::getGroupIndexInRound(int heroId,Round round)
{
	int ret = -1;
	switch (round)
	{
	case DianFengDuiJueManager::Round_8_4:
		ret = getGroupIndexInRound(heroId,player_arr_8_4,sizeof(player_arr_8_4)/sizeof(TopArenaInfo));
		break;
	case DianFengDuiJueManager::Round_4_2:
		ret = getGroupIndexInRound(heroId,player_arr_4_2,sizeof(player_arr_4_2)/sizeof(TopArenaInfo));
		break;
	case DianFengDuiJueManager::Round_2_1:
		ret = getGroupIndexInRound(heroId,player_arr_2_1,sizeof(player_arr_2_1)/sizeof(TopArenaInfo));
		break;
	default:
		CCAssert(false,"");
		break;
	}

	return ret;
}

int DianFengDuiJueManager::getGroupIndexInRound(int heroId,const TopArenaInfo* all_group,int count)
{
	int groupId = -1;

	for (int i=0; i<count; i++)
	{
		if (all_group[i].hero_a.hero_id == heroId || all_group[i].hero_b.hero_id == heroId)
		{
			groupId = i;
			break;
		}
	}

	return groupId;
}

bool DianFengDuiJueManager::is_hero_win_in_group(const HeroInfo& hero,const TopArenaInfo* all_group,int count)
{
	bool win = false;

	for (int i=0; i<count; i++)
	{
		if (all_group[i].hero_a.hero_id == hero.hero_id)
		{
			win = all_group[i].winer;
			break;
		}
		else if (all_group[i].hero_b.hero_id == hero.hero_id)
		{
			win = !all_group[i].winer;
			break;
		}
	}

	return win;
}

bool DianFengDuiJueManager::is_a_overcome_b(const HeroInfo& hero_a,const HeroInfo& hero_b,const TopArenaInfo* all_group,int count)
{
	bool win = false;

	for (int i=0; i<count; i++)
	{
		if (all_group[i].hero_a.hero_id == hero_a.hero_id &&
			all_group[i].hero_b.hero_id == hero_b.hero_id)
		{
			win = all_group[i].winer;
			break;
		}
		else if (all_group[i].hero_b.hero_id == hero_a.hero_id &&
			all_group[i].hero_a.hero_id == hero_b.hero_id)
		{
			win = !all_group[i].winer;
			break;
		}
	}

	return win;
}

bool DianFengDuiJueManager::is_a_overcome_b(HeroInfo hero_a,HeroInfo hero_b)
{
	CCAssert(inited,"");

	if (is_a_overcome_b(hero_a,hero_b,player_arr_8_4,sizeof(player_arr_8_4)/sizeof(TopArenaInfo)) ||
		is_a_overcome_b(hero_a,hero_b,player_arr_4_2,sizeof(player_arr_4_2)/sizeof(TopArenaInfo)) ||
		is_a_overcome_b(hero_a,hero_b,player_arr_2_1,sizeof(player_arr_2_1)/sizeof(TopArenaInfo))
		)
	{
		return true;
	}

	return false;
}

bool DianFengDuiJueManager::is_hero_win_in_round(HeroInfo hero_a,Round round)
{
	CCAssert(inited,"");

	bool win = false;
	switch (round)
	{
	case DianFengDuiJueManager::Round_8_4:
		win = is_hero_win_in_group(hero_a,player_arr_8_4,sizeof(player_arr_8_4)/sizeof(TopArenaInfo));
		break;
	case DianFengDuiJueManager::Round_4_2:
		win = is_hero_win_in_group(hero_a,player_arr_4_2,sizeof(player_arr_4_2)/sizeof(TopArenaInfo));
		break;
	case DianFengDuiJueManager::Round_2_1:
		win = is_hero_win_in_group(hero_a,player_arr_2_1,sizeof(player_arr_2_1)/sizeof(TopArenaInfo));
		break;
	default:
		CCAssert(false,"Error Round");
		break;
	}
	return win;
}

bool DianFengDuiJueManager::is_group_index_valide_in_round(unsigned short groupIndex,Round round)
{
	if (round == DianFengDuiJueManager::Round_2_1)
	{
		return groupIndex < 1;
	}
	else if (round == DianFengDuiJueManager::Round_4_2)
	{
		return groupIndex < 2;
	}
	else if (round == DianFengDuiJueManager::Round_8_4)
	{
		return groupIndex < 4;
	}
	CCAssert(false,"round 错误了");
	return false;
}

bool DianFengDuiJueManager::am_i_guessed()
{
	int round = getCurrentRound();
	if (round != Round_2_1 && round != Round_4_2 && round != Round_8_4 )
	{
		CCAssert(false,"Round Error");
		return false;
	}
	return my_guess[round].gussed;
}

DianFengDuiJueManager::~DianFengDuiJueManager(void)
{
}
