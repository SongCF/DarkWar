#pragma once
#include "ScriptDataProvider.h"


// FightScriptUserData
struct FightSUD
{
	unsigned int fightIndex;
	bool boss_freezy;
	HeroState heroHpAfterLastFight;		// 上次打完的血量数据

	FightSUD()
	{
		boss_freezy = false;
	}
};

