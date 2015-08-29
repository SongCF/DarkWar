#include "FightScriptDefine.h"

//////////////////////////////////////////////////////////////////////////
// OneStep
//////////////////////////////////////////////////////////////////////////
// 1,2,3
string OneStep::getBuffString()
{
	string ret = "remove-->{";// 1,2,3
	int num = buff.remove_buff_array.size();
	for (int i=0;i<num;i++)
	{
		char tem[8];
		sprintf(tem,"%d",buff.remove_buff_array[i]);
		ret += string(tem);
		ret += ",";
	}

	if(ret.length() > 0)
		ret = ret.substr(0,ret.length()-1);
	ret += "}";

	if (buff.effects.size() > 0)
	{
		int num = buff.effects.size();
		ret += "\n";
		ret += "hp_change:{";

		char tem[64];
		for (int i=0;i<num;i++)
		{
			sprintf(tem,"[fantan = [%s], target_uId = %u,hp_add = %d]",
				buff.effects[i].isFantan?"true":"false", 
				buff.effects[i].target,buff.effects[i].hp_add);
			ret += string(tem);
			ret += ",";
		}
		if(ret.length() > 0)
			ret = ret.substr(0,ret.length()-1);
		ret += "}";
	}

	return ret;
}

string OneStep::getCurrentCdTimeString()
{
	string ret = "";// 1,2,3
	int num = cdTime_arr.size();
	for (int i=0;i<num;i++)
	{
		char tem[128];
		sprintf(tem,"(u_id = %d,time = %d)",cdTime_arr[i].uniqueId,cdTime_arr[i].cdTime);
		ret += string(tem);
		ret += ",";
	}

	if(ret.length() > 0)
		ret = ret.substr(0,ret.length()-1);

	return ret;
}


//////////////////////////////////////////////////////////////////////////
// FightScript
//////////////////////////////////////////////////////////////////////////
bool FightScript::isEqual(const FightScript& test)
{
#define BREAK 1
	FightScript testCopy = test;
	// InitChessBoard
	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		if (test.init.myCurBloodNum != init.myCurBloodNum ||
			test.init.myDefaultBloodNum != init.myDefaultBloodNum || 
			test.init.oppositeCurBloodNum != init.oppositeCurBloodNum || 
			test.init.oppositeDefaultBloodNum != init.oppositeDefaultBloodNum 
			)
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}

		if (test.init.myTeam[i].heroId != init.myTeam[i].heroId ||
			test.init.myTeam[i].hp != init.myTeam[i].hp ||
			test.init.myTeam[i].defaultHp != init.myTeam[i].defaultHp ||
			test.init.myTeam[i].defaultCdTime != init.myTeam[i].defaultCdTime ||
			test.init.myTeam[i].level != init.myTeam[i].level ||
			test.init.myTeam[i].stage != init.myTeam[i].stage ||
			test.init.myTeam[i].profession_id != init.myTeam[i].profession_id ||
			test.init.myTeam[i].summary_equips.size() != init.myTeam[i].summary_equips.size() ||
			test.init.myTeam[i].x_size != init.myTeam[i].x_size ||
			test.init.myTeam[i].hero_type != init.myTeam[i].hero_type ||
			test.init.myTeamUniqueId[i] != init.myTeamUniqueId[i]
		)
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}
		// 装备
		{
			int equipNum = test.init.myTeam[i].summary_equips.size();
			for (int e = 0;e<equipNum;e++)
			{
				if (test.init.myTeam[i].summary_equips[e].zhuangbei_colour != init.myTeam[i].summary_equips[e].zhuangbei_colour ||
					test.init.myTeam[i].summary_equips[e].zhuangbei_type != init.myTeam[i].summary_equips[e].zhuangbei_type ||
					strcmp(test.init.myTeam[i].summary_equips[e].zhuangbei_pic,init.myTeam[i].summary_equips[e].zhuangbei_pic) != 0)
				{
#if BREAK == 1
					CCAssert(false,"");
#endif
					return false;
				}
			}
		}


		if (test.init.oppositeTeam[i].heroId != init.oppositeTeam[i].heroId ||
			test.init.oppositeTeam[i].hp != init.oppositeTeam[i].hp ||
			test.init.oppositeTeam[i].defaultHp != init.oppositeTeam[i].defaultHp ||
			test.init.oppositeTeam[i].defaultCdTime != init.oppositeTeam[i].defaultCdTime ||
			test.init.oppositeTeam[i].level != init.oppositeTeam[i].level ||
			test.init.oppositeTeam[i].stage != init.oppositeTeam[i].stage ||
			test.init.oppositeTeam[i].profession_id != init.oppositeTeam[i].profession_id||
			test.init.oppositeTeam[i].summary_equips.size() != init.oppositeTeam[i].summary_equips.size() ||
			test.init.oppositeTeam[i].x_size != init.oppositeTeam[i].x_size||
			test.init.oppositeTeam[i].hero_type != init.oppositeTeam[i].hero_type||
			test.init.oppositeUniqueId[i] != init.oppositeUniqueId[i]
		)
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}
		// 装备
		{
			int equipNum = test.init.oppositeTeam[i].summary_equips.size();
			for (int e = 0;e<equipNum;e++)
			{
				if (test.init.oppositeTeam[i].summary_equips[e].zhuangbei_colour != init.oppositeTeam[i].summary_equips[e].zhuangbei_colour ||
					test.init.oppositeTeam[i].summary_equips[e].zhuangbei_type != init.oppositeTeam[i].summary_equips[e].zhuangbei_type ||
					strcmp(test.init.oppositeTeam[i].summary_equips[e].zhuangbei_pic,init.oppositeTeam[i].summary_equips[e].zhuangbei_pic) != 0)
				{
#if BREAK == 1
					CCAssert(false,"");
#endif
					return false;
				}
			}
		}

	}

	// Step
	int stepNum = stepVec.size();
	if (stepNum != test.stepVec.size())
	{
#if BREAK == 1
		CCAssert(false,"");
#endif
		return false;
	}

	for (int i=0;i<stepNum;i++)
	{
		if (stepVec[i].stepType != test.stepVec[i].stepType)
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}

		if (stepVec[i].getBuffString() != testCopy.stepVec[i].getBuffString())
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}
		if (stepVec[i].getCurrentCdTimeString() != testCopy.stepVec[i].getCurrentCdTimeString())
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}

		if (stepVec[i].stepType == StepType_Fight)
		{
			if (stepVec[i].fightData.fightPattern != test.stepVec[i].fightData.fightPattern ||
				stepVec[i].fightData.fightorUniqueId != test.stepVec[i].fightData.fightorUniqueId ||
				stepVec[i].fightData.targetUniqueVec.size() != test.stepVec[i].fightData.targetUniqueVec.size()
				)
			{
#if BREAK == 1
				CCAssert(false,"");
#endif
				return false;
			}

			int targetNum = stepVec[i].fightData.targetUniqueVec.size();
			for (int j=0;j<targetNum;j++)
			{
				if (stepVec[i].fightData.targetUniqueVec[j] != test.stepVec[i].fightData.targetUniqueVec[j] ||
					stepVec[i].fightData.targetBloodChange[j] != test.stepVec[i].fightData.targetBloodChange[j] ||
					stepVec[i].fightData.canMissVec[j] != test.stepVec[i].fightData.canMissVec[j]
				)
				{
#if BREAK == 1
					CCAssert(false,"");
#endif
					return false;
				}
			}
		}
		else if (stepVec[i].stepType == StepType_Move)
		{
			if (stepVec[i].moveData.destIndex != test.stepVec[i].moveData.destIndex ||
				stepVec[i].moveData.uniqueId != test.stepVec[i].moveData.uniqueId ||
				stepVec[i].moveData.inSelfChessBoard != test.stepVec[i].moveData.inSelfChessBoard
				)
			{
#if BREAK == 1
				CCAssert(false,"");
#endif
				return false;
			}
		}
		else if (stepVec[i].stepType == StepType_Skill)
		{
			if (stepVec[i].skillData.skillType != test.stepVec[i].skillData.skillType ||
				stepVec[i].skillData.fightorUniqueId != test.stepVec[i].skillData.fightorUniqueId ||
				stepVec[i].skillData.skillPatten != test.stepVec[i].skillData.skillPatten ||
				stepVec[i].skillData.firedBuff.size() !=  test.stepVec[i].skillData.firedBuff.size() || 
				stepVec[i].skillData.targetUniqueVec.size() != test.stepVec[i].skillData.targetUniqueVec.size()
				)
			{
#if BREAK == 1
				CCAssert(false,"");
#endif
				return false;
			}

			int targetNum = stepVec[i].skillData.targetUniqueVec.size();
			for (int j=0;j<targetNum;j++)
			{
				if (stepVec[i].skillData.targetUniqueVec[j] != test.stepVec[i].skillData.targetUniqueVec[j] ||
					stepVec[i].skillData.targetBloodChange[j] != test.stepVec[i].skillData.targetBloodChange[j] ||
					stepVec[i].skillData.canMissVec[j] != test.stepVec[i].skillData.canMissVec[j]
				)
				{
#if BREAK == 1
					CCAssert(false,"");
#endif
					return false;
				}
			}

			int buffNum = stepVec[i].skillData.firedBuff.size();

			for (int j=0;j<buffNum;j++)
			{
				if (stepVec[i].skillData.firedBuff[j].getLog() != testCopy.stepVec[i].skillData.firedBuff[j].getLog())
				{
#if BREAK == 1
					CCAssert(false,"");
#endif
					return false;
				}
			}
		}
		else if (stepVec[i].stepType == StepType_Wait)
		{
			// do nothing
		}
		else if(stepVec[i].stepType == StepType_ChangeRound)
		{
			if (stepVec[i].changeRound.round != test.stepVec[i].changeRound.round)
			{
#if BREAK == 1
				CCAssert(false,"");
#endif
				return false;
			}
		}
		else if (stepVec[i].stepType == StepType_UseBlood || stepVec[i].stepType == StepType_ReLoadBlood)
		{
			if (stepVec[i].useBlood.uniqueId != test.stepVec[i].useBlood.uniqueId ||
				stepVec[i].useBlood.bloodChangeForUser != test.stepVec[i].useBlood.bloodChangeForUser
				)
			{
#if BREAK == 1
				CCAssert(false,"");
#endif
				return false;
			}
		}
		else if (stepVec[i].stepType == StepType_FailByTakeTooMuchTime)
		{
			// do nothing
		}
		else
		{
			CCAssert(false,"");
		}
	}

	// Resullt

	if (endResult.myResult != test.endResult.myResult ||
		endResult.gettedExp != test.endResult.gettedExp ||
		endResult.gettedItemSuiPian.size() != test.endResult.gettedItemSuiPian.size() ||
		endResult.gettedShenBingSuiPian.size() != test.endResult.gettedShenBingSuiPian.size() ||
		endResult.gettedItem.size() != test.endResult.gettedItem.size() ||
		endResult.gettedPowerPoints.size() != test.endResult.gettedPowerPoints.size() ||
		endResult.gettedZhuangBei.size() != test.endResult.gettedZhuangBei.size())
	{
#if BREAK == 1
		CCAssert(false,"");
#endif
		return false;
	}

	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		/*if (endResult.heroDetailAttrAfterFight[i].basic_info.hero_id != test.endResult.heroDetailAttrAfterFight[i].basic_info.hero_id ||
		endResult.heroDetailAttrAfterFight[i].basic_info.level != test.endResult.heroDetailAttrAfterFight[i].basic_info.level ||
		endResult.heroDetailAttrAfterFight[i].basic_info.stage != test.endResult.heroDetailAttrAfterFight[i].basic_info.stage ||
		endResult.heroDetailAttrAfterFight[i].basic_info.is_yuanshen != test.endResult.heroDetailAttrAfterFight[i].basic_info.is_yuanshen ||
		endResult.heroDetailAttrAfterFight[i].basic_info.hero_id != test.endResult.heroDetailAttrAfterFight[i].basic_info.hero_id ||
		endResult.heroDetailAttrAfterFight[i].basic_info.vip_level != test.endResult.heroDetailAttrAfterFight[i].basic_info.vip_level ||
		endResult.heroDetailAttrAfterFight[i].basic_info.profession_id != test.endResult.heroDetailAttrAfterFight[i].basic_info.profession_id ||
		strcmp(endResult.heroDetailAttrAfterFight[i].basic_info.name,test.endResult.heroDetailAttrAfterFight[i].basic_info.name) != 0
		)
		{
		#if BREAK == 1
		CCAssert(false,"");
		#endif
		return false;
		}*/

		/*if (endResult.heroDetailAttrAfterFight[i].exp_current != test.endResult.heroDetailAttrAfterFight[i].exp_current ||
			endResult.heroDetailAttrAfterFight[i].exp_need != test.endResult.heroDetailAttrAfterFight[i].exp_need ||
			endResult.heroDetailAttrAfterFight[i].parent_hero_id != test.endResult.heroDetailAttrAfterFight[i].parent_hero_id ||
			endResult.heroDetailAttrAfterFight[i].postion != test.endResult.heroDetailAttrAfterFight[i].postion ||
			endResult.heroDetailAttrAfterFight[i].vit_current != test.endResult.heroDetailAttrAfterFight[i].vit_current
			)
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}

		if (0 != memcmp(&endResult.heroDetailAttrAfterFight[i].hero_attr_without_zhuangbei,
			&test.endResult.heroDetailAttrAfterFight[i].hero_attr_without_zhuangbei,sizeof(HeroAttrWithoutZhuangbei)))
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}*/

	}

	int item_sui_pian_num = endResult.gettedItemSuiPian.size();
	for (int i=0;i<item_sui_pian_num;i++)
	{
		if ( memcmp(&endResult.gettedItemSuiPian[i],&(test.endResult.gettedItemSuiPian[i]),sizeof(ItemSuipianInfo) ) != 0)
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}
	}

	int shengbing_sui_pian_num = endResult.gettedShenBingSuiPian.size();
	for (int i=0;i<shengbing_sui_pian_num;i++)
	{
		if ( memcmp(&endResult.gettedShenBingSuiPian[i],&(test.endResult.gettedShenBingSuiPian[i]),sizeof(ShenbingSuipianInfo) ) != 0)
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}
	}


	int zhuangbeiNum = endResult.gettedZhuangBei.size();
	for (int i=0;i<zhuangbeiNum;i++)
	{
		if (endResult.gettedZhuangBei[i].isEqual(test.endResult.gettedZhuangBei[i]))
		{
		}
		else
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}
	}

	int itemNum = endResult.gettedItem.size();
	for (int i=0;i<itemNum;i++)
	{
		if (endResult.gettedItem[i].count != test.endResult.gettedItem[i].count ||
			endResult.gettedItem[i].type != test.endResult.gettedItem[i].type
			)
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}
	}

	int powerNum = endResult.gettedPowerPoints.size();
	for (int i=0;i<powerNum;i++)
	{
		if (endResult.gettedPowerPoints[i].power != test.endResult.gettedPowerPoints[i].power)
		{
#if BREAK == 1
			CCAssert(false,"");
#endif
			return false;
		}
	}

	return true;
}
