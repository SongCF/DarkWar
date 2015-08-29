#include "FightScriptBuilder.h"
#include "FightControler.h"
#include "GameBuffProc.h"
#include "ChessBoardHelper_TeamFight.h"
#include "CS_Common.h"
#include "CalculateData.h"
#include "ZhuangbeiGongMingManager.h"


double random(double start, double end)
{    
	return start+(end-start)*rand()/(RAND_MAX + 1.0);
}

// 随机一个0到1直接的随机数
// 包括0和1
float lfRandom0_1()
{
	//srand(unsigned(time(0)));
	return (float)rand()/RAND_MAX;
}
int randomWithin(int start,int end)
{
	return (int)((float)start + ((float)(end-start))*lfRandom0_1());
}
float randomWithin_f(float start,float end)
{
	return start + (end-start)*lfRandom0_1();
}


//
FightScriptBuilder::FightScriptBuilder(void)
{
}

void FightScriptBuilder::fillHeroBaseDataByChessBoardItem(unsigned heroId,HeroBase& outData,const ChessBoardItem* item)
{
	if (item == NULL || heroId == 0)
	{
		// heroId或item一个为0或空，对方也应该为0或空
		CCAssert(heroId == 0,"");	// 
		CCAssert(item == NULL,"");	// 

		outData.heroId = 0;
		outData.profession_id = Profession_Yemanren;
		outData.hp = 0;
		outData.defaultCdTime = 0;
		outData.defaultHp = 0;
		outData.level = 0;
		outData.stage = 1;
		outData.x_size = 1;
		outData.resonance_star = 0;
		outData.hero_type = Type_Hero_Normal;
		outData.summary_equips.clear();	// 无装备
		//outData.handNum = 1;
		return;
	}
	fillHeroBaseDataByAbstractItem(heroId,outData,item->hero);
}

void FightScriptBuilder::fillHeroBaseDataByAbstractItem(unsigned heroId,HeroBase& outData,const AbstractItem* item)
{
	if (item == NULL || heroId == 0)
	{
		// heroId或item一个为0或空，对方也应该为0或空
		CCAssert(heroId == 0,"");	// 
		CCAssert(item == NULL,"");	// 

		outData.profession_id = Profession_Yemanren;
		outData.hp = 0;
		outData.defaultCdTime = 0;
		outData.defaultHp = 0;
		outData.level = 0;
		outData.stage = 1;
		outData.x_size = 1;
		outData.resonance_star = 0;
		outData.hero_type = Type_Hero_Normal;
		return;
	}
	outData.heroId = heroId;
	outData.profession_id = item->getProfession();
	outData.hp = item->getHp();
	outData.defaultCdTime = item->getDefaultCdTime();
	outData.defaultHp = item->getDefaultHp();
	outData.level = item->getLevel();
	outData.stage = item->getStage();
	outData.hero_type = item->getHeroType();
	outData.x_size = item->getXSize();

	// 是玩家的角色
	AbstractItem* temp = const_cast<AbstractItem*>(item);
	const Hero* hero = dynamic_cast<Hero*>(temp);
	const TeamHero* teamHero = dynamic_cast<TeamHero*>(temp);
	if (hero)
	{
		outData.resonance_star = hero->get_resonance_star();
        
		vector<EquipData> equips = hero->equipVec;
		int equipNum = equips.size();
		for (int i=0;i<equipNum;i++)
		{
			ZhuangbeiSummary summary;
			summary.zhuangbei_colour = equips[i].zhuangbei_colour;
			summary.zhuangbei_type = equips[i].zhuangbei_type;
			strcpy(summary.zhuangbei_pic,equips[i].equipPic.c_str());

			outData.summary_equips.push_back(summary);
		}
	}
	else if(teamHero)
	{
		vector<EquipData> equips = teamHero->equipVec;
		int equipNum = equips.size();
		for (int i=0;i<equipNum;i++)
		{
			ZhuangbeiSummary summary;
			summary.zhuangbei_colour = equips[i].zhuangbei_colour;
			summary.zhuangbei_type = equips[i].zhuangbei_type;
			strcpy(summary.zhuangbei_pic,equips[i].equipPic.c_str());

			outData.summary_equips.push_back(summary);
		}
	}
	
}

HeroState FightScriptBuilder::genScript( 
	unsigned int A_team_id[],unsigned int B_team_id[], 
	FightScript& A_script,FightScript& B_script, 
	ScriptDataProvider* provider, 
	ScriptBuilderUserData userData, 
	FirstBloodBoard firstBloodBoard, 
	int huiHeLimite /* = -1 */ )
{

	if (huiHeLimite >= 0)
	{
		LFLog("genScript with huihe limite [%d]",huiHeLimite);
	}
	else
	{
		LFLog("genScript with no huihe limite");
	}
	


	unsigned int uniqueIdArray[FormationHeroAcountMax*2] = {
		1,2,3,4,	// A的
		5,6,7,8};	// B的

	HeroState ret;

	// 帮助类
	ChessBoardHelper helper;
	helper.initHelper(provider,
		A_team_id,uniqueIdArray,
		B_team_id,uniqueIdArray+FormationHeroAcountMax,
		userData);
	GameBuffProc buff_pro(&helper);
	// 作战过程的控制，生成器
	FightControler mFControler(&helper,&buff_pro);
	mFControler.initFirstFightBoard(firstBloodBoard);

	// 初始化信息
	{
		for (int i=0;i<FormationHeroAcountMax;i++)
		{
			// a队
			{
				A_script.init.myTeamUniqueId[i] = A_team_id[i]>0?uniqueIdArray[i]:0;
				A_script.init.oppositeUniqueId[i] = B_team_id[i]>0?uniqueIdArray[i+FormationHeroAcountMax]:0;
				A_script.init.myCurBloodNum = helper.getBoardA()->getCurrentBloodNum();
				A_script.init.myDefaultBloodNum = helper.getBoardA()->getDefaultBloodNum();
				A_script.init.oppositeCurBloodNum = helper.getBoardB()->getCurrentBloodNum();
				A_script.init.oppositeDefaultBloodNum = helper.getBoardB()->getDefaultBloodNum();
				fillHeroBaseDataByChessBoardItem(A_team_id[i],A_script.init.myTeam[i],helper.getBoardA()->getActorItem(A_script.init.myTeamUniqueId[i]));
				fillHeroBaseDataByChessBoardItem(B_team_id[i],A_script.init.oppositeTeam[i],helper.getBoardB()->getActorItem(A_script.init.oppositeUniqueId[i]));
			}

			{
				// b队
				B_script.init.myTeamUniqueId[i] = B_team_id[i]>0?uniqueIdArray[i+FormationHeroAcountMax]:0;
				B_script.init.oppositeUniqueId[i] = A_team_id[i]>0?uniqueIdArray[i]:0;
				B_script.init.myCurBloodNum = helper.getBoardB()->getCurrentBloodNum();
				B_script.init.myDefaultBloodNum = helper.getBoardB()->getDefaultBloodNum();
				B_script.init.oppositeCurBloodNum = helper.getBoardA()->getCurrentBloodNum();
				B_script.init.oppositeDefaultBloodNum = helper.getBoardA()->getDefaultBloodNum();
				fillHeroBaseDataByChessBoardItem(B_team_id[i],B_script.init.myTeam[i],helper.getBoardB()->getActorItem(B_script.init.myTeamUniqueId[i]));
				fillHeroBaseDataByChessBoardItem(A_team_id[i],B_script.init.oppositeTeam[i],helper.getBoardA()->getActorItem(B_script.init.oppositeUniqueId[i]));
			}
		}
	}

	int state_code = 0;
//	if (specail_control == SC_BOSS_FREEZE)
//	{// Boss不动的作战过程信息
//#define FIGHT_TRUN_NUM	10
//		vector<OneStep> stepVec;
//		int round = 0;
//		bool break_lose = false;
//		// 动态的模拟作战，并且生成数据
//		while ( (state_code = helper.checkSuccess()) == 0 )
//		{
//			LFLog("round [%d],turn [%d]",round,mFControler.getBoardATurnIndex() );
//			if ( (DEFAULT_SPECIAL_CONTROL_BOSS_TAG == TEAM_TAG_B?mFControler.getBoardATurnIndex():mFControler.getBoardBTurnIndex()) > FIGHT_TRUN_NUM )
//			{//10轮内没打死Boss就输
//				break_lose = true;
//				OneStep step;
//				step.stepType = StepType_FailByTakeTooMuchTime;
//				stepVec.push_back(step);
//				LFLog("step --> FIGHT_TRUN_NUM,break!");
//				break;
//			}
//
//			vector<OneStep> step = mFControler.oneChessBoardFight();
//			stepVec.insert(stepVec.end(),step.begin(),step.end());
//
//			round += step.size();
//		}
//		if (break_lose)
//			state_code = -1;	// A输
//
//		LFLog("=====================================================");
//		LFLog("====================%s board Win!====================",state_code>0 ? "A":"B");
//		LFLog("=====================================================");
//		A_script.stepVec = stepVec;
//		B_script.stepVec = stepVec;
//
//	}
//	else
	{// 普通作战过程信息
		
		vector<OneStep> stepVec;
		int fightStep = 0;
		int valide_step_index = 0;

		bool break_lose = false;
		// 动态的模拟作战，并且生成数据
		while ( (state_code = helper.checkSuccess()) == 0)
		{
			LFLog("step [%d], turn [%d]",fightStep,mFControler.getCurrentTurnIndex());

			if (huiHeLimite >= 0 && mFControler.getCurrentTurnIndex() > huiHeLimite)
			{
				break_lose = true;
				OneStep step;
				step.stepType = StepType_FailByTakeTooMuchTime;
				stepVec.push_back(step);
				LFLog("step --> PVE_MAX_STEP_NUM,break!");
				break;
			}
			/*if (valide_step_index > PVE_MAX_STEP_NUM && (userData.battle_type == BattleType_MainTask || userData.battle_type == BattleType_CityGuard) )
			{
				break_lose = true;
				OneStep step;
				step.stepType = StepType_FailByTakeTooMuchTime;
				stepVec.push_back(step);
				LFLog("step --> PVE_MAX_STEP_NUM,break!");
				break;
			}*/

			vector<OneStep> step = mFControler.oneChessBoardFight();
			stepVec.insert(stepVec.end(),step.begin(),step.end());
			LFLog("=====>Turn after this round : BoardA-Turn [%d],BoardB-Turn [%d] huihe = [%d] \n\n",
				mFControler.getBoardATurnIndex(),mFControler.getBoardBTurnIndex(),mFControler.getCurrentTurnIndex() );

			fightStep += step.size();
			valide_step_index ++;
		}
		if (break_lose)
			state_code = -1;	// A输

		LFLog("=====================================================");
		LFLog("====================%s board Win!====================",state_code>0 ? "A":"B");
		LFLog("=====================================================");
		A_script.stepVec = stepVec;
		B_script.stepVec = stepVec;

	}




	// 战后的角色数据
	{
		LFLog("---------------------User hp after fight---------------------");
		for (int i=0;i<FormationHeroAcountMax;i++)
		{
			ChessBoardItem* a = helper.getOneItemByUniqueId(uniqueIdArray[i]);
			ChessBoardItem* b = helper.getOneItemByUniqueId(uniqueIdArray[i+FormationHeroAcountMax]);
			ret.currentHp_A[i] = (a != NULL)?a->hero->getHp():0;
			ret.currentHp_B[i] = (b != NULL)?b->hero->getHp():0;
			ret.A_teamHeroId[i] = A_script.init.myTeam[i].heroId;
			ret.B_teamHeroId[i] = B_script.init.myTeam[i].heroId;
			ret.A_teamCurrentBloodNum = helper.getBoardA()->getCurrentBloodNum();
			ret.B_teamCurrentBloodNum = helper.getBoardB()->getCurrentBloodNum();
			ret.roundNumForA = mFControler.getBoardATurnIndex()+1;
			ret.roundNumForB = mFControler.getBoardBTurnIndex()+1;

			LFLog("A team index=[%d],heroId = [%d] ,hp = [%d]",i,ret.A_teamHeroId[i],ret.currentHp_A[i]);
			LFLog("B team index=[%d],heroId = [%d] ,hp = [%d]",i,ret.B_teamHeroId[i],ret.currentHp_B[i]);
		}
#if (USE_FIGHT_LOG == 1)
		CCLog("---------------------User hp after fight---------------------\n\n");
#endif
	}


	// 战后数据
	{
		A_script.endResult.myResult = state_code > 0 ? Result_Win:Result_Lose;
		B_script.endResult.myResult = state_code < 0 ? Result_Win:Result_Lose;

		// 回调，用于数据持久化
		userData.teamTag = TEAM_TAG_A;
		A_script.endResult.gettedExp = provider->fightEndCallback(userData,A_script.endResult.myResult == Result_Win);
		userData.teamTag = TEAM_TAG_B;
		B_script.endResult.gettedExp = provider->fightEndCallback(userData,B_script.endResult.myResult == Result_Win);

		//获得物品和装备
		{
			userData.teamTag = TEAM_TAG_A;
			A_script.endResult.gettedItemSuiPian = provider->getFightEndItemSuiPians(userData,A_script.endResult.myResult == Result_Win);
			A_script.endResult.gettedShenBingSuiPian = provider->getFightEndShenBingSuiPians(userData,A_script.endResult.myResult == Result_Win);
			A_script.endResult.gettedZhuangBei = provider->getFightEndEquips(userData,A_script.endResult.myResult == Result_Win);
			A_script.endResult.gettedItem = provider->getFightEndItems(userData,A_script.endResult.myResult == Result_Win);
			A_script.endResult.gettedPowerPoints = provider->getFightEndPowerPoints(userData,A_script.endResult.myResult == Result_Win);
			
			userData.teamTag = TEAM_TAG_B;
			B_script.endResult.gettedItemSuiPian = provider->getFightEndItemSuiPians(userData,B_script.endResult.myResult == Result_Win);
			B_script.endResult.gettedShenBingSuiPian = provider->getFightEndShenBingSuiPians(userData,B_script.endResult.myResult == Result_Win);
			B_script.endResult.gettedZhuangBei = provider->getFightEndEquips(userData,B_script.endResult.myResult == Result_Win);
			B_script.endResult.gettedItem = provider->getFightEndItems(userData,B_script.endResult.myResult == Result_Win);
			B_script.endResult.gettedPowerPoints = provider->getFightEndPowerPoints(userData,B_script.endResult.myResult == Result_Win);
		}

		//// 数据，经验等变化，最后结果
		//for (int i=0;i<FormationHeroAcountMax;i++)
		//{
		//	userData.teamTag = TEAM_TAG_A;
		//	provider->getHeroDetail(A_script.endResult.heroDetailAttrAfterFight[i],A_team_id[i],userData);
		//	userData.teamTag = TEAM_TAG_B;
		//	provider->getHeroDetail(B_script.endResult.heroDetailAttrAfterFight[i],B_team_id[i],userData);
		//}
	}


	return ret;
}


void FightScriptBuilder::genTeamFightScript(
	TeamHero* hero[FormationHeroAcountMax], 
	Guaiwu* boss, 
	vector<FightScript>& script_vec_4, 
	TeamEndParam& endData)
{
	CCAssert(boss!=NULL,"");

	unsigned int bossId = 1000;

	unsigned int uniqueIdArray[FormationHeroAcountMax+1] = {
		1,2,3,4,			// team的
		TEAM_BOSS_U_ID};	// Boos的

	ChessBoard_TeamFight teamBoard;
	vector<ChessBoardItem_TeamFight*> items;
	for (int i=0;i<4;i++)
	{
		if (hero[i] == NULL)
		{
			items.push_back(NULL);
			continue;
		}

		ChessBoardItem_TeamFight* item = new ChessBoardItem_TeamFight(uniqueIdArray[i]);
		item->hero = hero[i];
		teamBoard.addItem(item,i);
		items.push_back(item);
	}

	ChessBoardItem_TeamFight* bossItem = new ChessBoardItem_TeamFight(uniqueIdArray[4]);
	bossItem->hero = boss;
	teamBoard.addItem(bossItem,4);
	items.push_back(bossItem);


	

	GameBuffProc buff_pro(&teamBoard);
	// 作战过程的控制，生成器
	TeamFightControler mFControler(&teamBoard,&buff_pro);
	mFControler.initFirstFightBoard();

	FightScript script;

	// 初始化信息
	{
		// boss数据
		script.init.oppositeUniqueId[0] = bossItem->getUniqueId();
		fillHeroBaseDataByAbstractItem(bossId,script.init.oppositeTeam[0],boss);

		// 团队数据
		for (int i=0;i<FormationHeroAcountMax;i++)
		{
			if (hero[i] == NULL)
			{
				script.init.myTeamUniqueId[i] = 0;
				script.init.myCurBloodNum = 0;
				script.init.myDefaultBloodNum = 0;
				script.init.oppositeCurBloodNum = 0;
				script.init.oppositeDefaultBloodNum = 0;
				fillHeroBaseDataByAbstractItem(0,script.init.myTeam[i],NULL);
				continue;
			}

			script.init.myTeamUniqueId[i] = items[i]->getUniqueId();
			script.init.myCurBloodNum = 0;
			script.init.myDefaultBloodNum = 0;
			script.init.oppositeCurBloodNum = 0;
			script.init.oppositeDefaultBloodNum = 0;
			fillHeroBaseDataByAbstractItem(hero[i]->getTeamProperty().mLeaderId,script.init.myTeam[i],hero[i]);
		}
	}

	int state_code = 0;
	// 作战过程信息
	{
		vector<OneStep> stepVec;
		int round = 0;
		int valide_step_index = 0;

		bool break_lose = false;
		// 动态的模拟作战，并且生成数据
		while ( (state_code = teamBoard.checkSuccess()) == 0)
		{
			LFLog("round [%d]",round);
			if (valide_step_index > PVE_MAX_STEP_NUM)
			{
				break_lose = true;
				OneStep step;
				step.stepType = StepType_FailByTakeTooMuchTime;
				stepVec.push_back(step);
				LFLog("step --> PVE_MAX_STEP_NUM,break!");
				break;
			}

			vector<OneStep> step = mFControler.oneChessBoardFight();
			stepVec.insert(stepVec.end(),step.begin(),step.end());

			round += step.size();
			valide_step_index ++;
		}

		if (break_lose)
			state_code = -1;	// A输

		LFLog("=====================================================");
		LFLog("====================%s board Win!====================",state_code>0 ? "A":"B");
		LFLog("=====================================================");
		script.stepVec = stepVec;
	}

	// 战后数据
	{
		script.endResult.myResult = state_code > 0 ? Result_Win:Result_Lose;
		
		// 初始化砍的血量...  ----> 这其实有个隐晦的bug
		// 
		// 隐晦的bug : 就是虽然角色A时砍血了X滴，但是不代表他就对于团队来说伤害了Boss 有X滴
		// 因为如果角色A被迷幻了，那么其实在这里X滴血中，可能有Y滴是砍的队友的血量......
		// 也就是可能叛徒却是最大的砍血量者.....
		//
		// 解决方案：
		// 换一个思路，我们不再看攻击者是谁就加他的攻击点数，我们应该看该攻击者攻击的是不是团队的目标
		// 是的话再加上，不是的话就不能加，就解决了！ （目前还没有按照这个改，目前还是bug版本）
		// 
		{
			int blood[5] = {0};
			int num = script.stepVec.size();
			for (int stepIndex = 0;stepIndex<num;stepIndex++)
			{
				if (script.stepVec[stepIndex].stepType == StepType_Fight)
				{
					int targetNum = script.stepVec[stepIndex].fightData.targetUniqueVec.size();
					for (int tarIndex = 0;tarIndex < targetNum ; tarIndex ++)
					{
						if ( script.stepVec[stepIndex].fightData.canMissVec[tarIndex] )
							continue;
						blood[script.stepVec[stepIndex].fightData.fightorUniqueId-1] += script.stepVec[stepIndex].fightData.targetBloodChange[tarIndex];
					}
				}
				else if (
					script.stepVec[stepIndex].stepType == StepType_FailByTakeTooMuchTime ||
					script.stepVec[stepIndex].stepType == StepType_ReLoadBlood ||
					script.stepVec[stepIndex].stepType == StepType_UseBlood ||
					script.stepVec[stepIndex].stepType == StepType_Move ||
					script.stepVec[stepIndex].stepType == StepType_Wait
					)
				{
					continue;
				}
				else if (script.stepVec[stepIndex].stepType == StepType_Skill)
				{
					int targetNum =  script.stepVec[stepIndex].skillData.targetUniqueVec.size();
					for (int tarIndex = 0;tarIndex < targetNum;tarIndex++)
					{
						if ( script.stepVec[stepIndex].skillData.canMissVec[tarIndex] )
							continue;
						blood[script.stepVec[stepIndex].skillData.fightorUniqueId-1] += script.stepVec[stepIndex].skillData.targetBloodChange[tarIndex];
					}

					// buff的减血，这个不计算，因为可能会不公平，比如A的buff是减血，B的buff是减防
					// 那么结果就是B是为他人做了嫁衣
					// 所有buff的影响不考虑比较好
				}
				else
				{
					CCAssert(false,"未知的Step类型");
				}
			}

			endData.fightedBlood[0] = abs(blood[0]);
			endData.fightedBlood[1] = abs(blood[1]);
			endData.fightedBlood[2] = abs(blood[2]);
			endData.fightedBlood[3] = abs(blood[3]);
			LFLog("--------------------------------------------------------");
			LFLog("blood data [%d,%d,%d,%d]",endData.fightedBlood[0],endData.fightedBlood[1],endData.fightedBlood[2],endData.fightedBlood[3]);
			LFLog("--------------------------------------------------------\n");
		}

		// 保存得到的道具数据到脚本
		for (int i=0;i<FormationHeroAcountMax;i++)
		{
			FightScript oneScript = script;
			if (hero[i] == NULL)
			{
				script_vec_4.push_back(oneScript);
				continue;
			}
			//// 不会长经验什么的，所有这里的数据不用管
			//HeroDetail outData;
			//memset(&outData,0,sizeof(HeroDetail));
			//oneScript.endResult.heroDetailAttrAfterFight[0] = outData;
			//oneScript.endResult.heroDetailAttrAfterFight[1] = outData;
			//oneScript.endResult.heroDetailAttrAfterFight[2] = outData;
			//oneScript.endResult.heroDetailAttrAfterFight[3] = outData;
			// 目前不提供了，因为在GetBattleScriptGiftRsp的Giftdata里面已经有了
			oneScript.endResult.gettedItem.clear();
			oneScript.endResult.gettedZhuangBei.clear();
			oneScript.endResult.gettedPowerPoints.clear();
			script_vec_4.push_back(oneScript);
		}
	}


	// 删除new出来的Item
	int size = items.size();
	for (int i=0;i<size;i++)
	{
		if (items[i] == NULL)
		{
			continue;
		}

		delete items[i];
	}
	items.clear();
}


vector<FightScript> temp;
//
char* FightScriptBuilder::FightScript_2_Package(vector<FightScript> A_scriptVec)
{
	int dataLength = sizeof(SPCmd_BattleRsp);
	const int scriptNum = A_scriptVec.size();

	int script_lenght;
	char* script_data = FightScriptVec_2_CharArr(A_scriptVec,script_lenght);
	dataLength += script_lenght;

	// 一次性new够
	char* msg = new char[sizeof(SPHead) + dataLength];

	SPHead* rspHead;
	(rspHead) = (SPHead*)msg;
	(rspHead)->protocol_tag = SimpleProtocol_Tag_Server;
	(rspHead)->protocol_ver = 1;
	(rspHead)->cmd = BattleRsp;
	(rspHead)->data_len = dataLength;
	//将包头转换为网络字节序
	hton_SPHead((rspHead));

	SPCmd_BattleRsp* rspBody = (SPCmd_BattleRsp*)((rspHead)+1);
	rspBody->err_code = htonl(Success);
	rspBody->fightNum = htonl(scriptNum);

	// 没有脚本，直接就返回了
	if (scriptNum <= 0)
		return msg;
	memcpy(rspBody->fightScript_array,script_data,script_lenght); 
	delete[] script_data;
	return msg;
}
vector<FightScript> FightScriptBuilder::SPCmd_BattleRsp_2_FightScript(const char* data,int& rspCode)
{
	SPCmd_BattleRsp* rspBody = (SPCmd_BattleRsp*)(data);
	rspBody->err_code = ntohl(rspBody->err_code);
	rspCode = rspBody->err_code;

	vector<FightScript> ret_vec;

	if (rspBody->err_code != Success)
		return ret_vec;

	int num = ntohl(rspBody->fightNum);

	ret_vec = CharArr_2_FightScriptVec(rspBody->fightScript_array,num);

	/*int offset = 0;
	for (int i=0;i<num;i++)
	{
	FightScript one = Battel_FightScript_2_FightScript(rspBody->fightScript_array + offset);
	ret_vec.push_back(one);
	offset += getBattel_FightScriptLength(&one);
	}*/

	return ret_vec;
}

//
char* FightScriptBuilder::FightScriptVec_2_CharArr(vector<FightScript> A_scriptVec,int & length)
{
	length = 0;
	if (A_scriptVec.size() <= 0)
		return NULL;

	temp = A_scriptVec;

	int dataLength = 0;
	int scriptNum = A_scriptVec.size();
	// 每一段的长度
	unsigned long* script_length = new unsigned long[scriptNum];
	for (int i=0;i<scriptNum;i++)
	{
		script_length[i] = getBattel_FightScriptLength(&A_scriptVec[i]);
		dataLength += script_length[i];
	}
	// 一次性new够
	char* msg = new char[dataLength];
	length = dataLength;

	CCLog("FightScriptVec_2_CharArr : scriptNum = [%d],totalDataLength = [%.2f KB]",scriptNum,(float)dataLength/1024.0f);

	int offset = 0;// 在SPCmd_BattleRsp的fightScript_array中的偏移
	for (int i=0;i<scriptNum;i++)
	{
		unsigned long char_length;
		char* one_Script = FightScript_2_Battel_FightScript(A_scriptVec[i],char_length);
		CCAssert(char_length == script_length[i],"");
		memcpy(msg + offset,one_Script,char_length); 
		offset += char_length;
		delete [] one_Script;
	}

	return msg;
}
// 将char数组的网络字节序的脚本，转化成本地字节序，并存放在FightScript里面
// 需要告诉内部，这里面存放了有一个脚本，才可以解析
vector<FightScript> FightScriptBuilder::CharArr_2_FightScriptVec(const char* scirpt_char_arr,int fightScriptNum)
{
	vector<FightScript> ret_vec;
	int num = fightScriptNum;
	CCLog("CharArr_2_FightScriptVec : scriptNum = [%d]",num);
	int offset = 0;
	for (int i=0;i<num;i++)
	{
		FightScript one = Battel_FightScript_2_FightScript(scirpt_char_arr + offset);
		//// ceshi 
		//bool isequ = one.isEqual(temp[i]);
		//int length = getBattel_FightScriptLength(&one);
		//int lengthold = getBattel_FightScriptLength(&temp[i]);
		//CCAssert(length == lengthold,"");
		ret_vec.push_back(one);
		offset += getBattel_FightScriptLength(&one);
	}

	return ret_vec;
}

// 
int FightScriptBuilder::getBattel_FightScriptLength(const FightScript* A_script)
{
	int dataLength = 0;
	// Battel_FightScript 的长度
	{
		// 开始
		dataLength += sizeof(Battel_InitChessBoard);//1608

		dataLength += sizeof(unsigned int);//unsigned int stepCount  1612
		// 步奏
		int stepNum = A_script->stepVec.size();
		for (int i=0;i<stepNum;i++)
		{
			// 可多不可少,131,149,
			dataLength += sizeof(Battel_OneStep);
			dataLength += A_script->stepVec[i].buff.effects.size() * sizeof(Buff_Doing);
			dataLength += A_script->stepVec[i].buff.remove_buff_array.size() * sizeof(int);
			dataLength += A_script->stepVec[i].cdTime_arr.size()*sizeof(CdDiscr);	


			if (A_script->stepVec[i].stepType == StepType_Fight)
			{
				dataLength += sizeof(Battel_StepFight);//146
				dataLength += (sizeof(unsigned int) + sizeof(bool) + sizeof(int)) * A_script->stepVec[i].fightData.targetUniqueVec.size();

			}
			else if (A_script->stepVec[i].stepType == StepType_Move)
			{
				dataLength += sizeof(Battel_StepMove);
			}
			else if (A_script->stepVec[i].stepType == StepType_ChangeRound)
			{
				dataLength += sizeof(Battel_StepChangeRound);
			}
			else if (A_script->stepVec[i].stepType == StepType_Wait)
			{
				// 无操作
			}
			else if (A_script->stepVec[i].stepType == StepType_FailByTakeTooMuchTime)
			{
				// 无操作
			}
			else if (A_script->stepVec[i].stepType == StepType_UseBlood || A_script->stepVec[i].stepType == StepType_ReLoadBlood)
			{
				dataLength += sizeof(Battel_StepUseBlood);
			}
			else if (A_script->stepVec[i].stepType == StepType_Skill)
			{
				dataLength += sizeof(Battel_StepSkill);	// 可多，不能少
				short buffNum = A_script->stepVec[i].skillData.firedBuff.size();
				if (buffNum > 0)   
				{
					dataLength += sizeof(Battel_GameBuff) * buffNum;
					for (int j = 0;j<buffNum;j++)
					{
						int buff_eff_num = A_script->stepVec[i].skillData.firedBuff[j].effect_vec.size();
						dataLength += buff_eff_num * sizeof(Battel_GameBuffEffect);
					}
				}

				// 每一个target对应这3个sizeof
				dataLength += (sizeof(unsigned int) + sizeof(bool) + sizeof(int)) * A_script->stepVec[i].skillData.targetUniqueVec.size();
			}
			else
			{
				CCAssert(false,"Unkown-StepType");
			}
		}

		// 结束
		dataLength += sizeof(Battel_FightEnd);// 可多不可少1972

		dataLength += sizeof(ItemSuipianInfo) *  A_script->endResult.gettedItemSuiPian.size();
		dataLength += sizeof(ShenbingSuipianInfo) *  A_script->endResult.gettedShenBingSuiPian.size();

		int zhuangBeiNum = A_script->endResult.gettedZhuangBei.size();
		for (int i=0;i<zhuangBeiNum;i++)
		{
			string dsrc =  A_script->endResult.gettedZhuangBei[i].equipDicrib;
			dataLength += (sizeof(ZhuangbeiDetail) + dsrc.length()+1);//1968
			dataLength += A_script->endResult.gettedZhuangBei[i].attr_vec.size() * sizeof(ZBAttr);//2141 - 1986 = 173

			// 传奇就带精炼数据    2014.05.22稀有装备也带精炼数据
			if (A_script->endResult.gettedZhuangBei[i].zhuangbei_colour == ZhuangbeiColour_Chuanqi
                || A_script->endResult.gettedZhuangBei[i].zhuangbei_colour == ZhuangbeiColour_Xiyou)
			{
				dataLength += sizeof(JinglianData);
                dataLength += 1; //purify_hole
                dataLength +=  A_script->endResult.gettedZhuangBei[i].purify_attr_vec.size() * sizeof(ZBAttr);

                dataLength += 1; //is_unlocked
                
                if(A_script->endResult.gettedZhuangBei[i].zhuangbei_colour == ZhuangbeiColour_Chuanqi)
                {
    				dataLength += 1; //is_shenbing
    				if(A_script->endResult.gettedZhuangBei[i].isShenBing)
    				{
    					dataLength += 1; // identified_arr_count
    					dataLength += A_script->endResult.gettedZhuangBei[i].known_hide_attr_vec.size() * sizeof(ZBAttr);
    					dataLength += 1; // has_jingwen
    					dataLength += 2 * sizeof(ZBAttr); // 2个固定
    				}
                }
			}
		}
		int wupingNum = A_script->endResult.gettedItem.size();
		dataLength += sizeof(ItemInfo) * wupingNum;//1683

		int powerNum = A_script->endResult.gettedPowerPoints.size();
		dataLength += sizeof(PowerPoint) * powerNum;

	}

	return dataLength;// 1968
}

char* FightScriptBuilder::FightScript_2_Battel_FightScript(FightScript f_script,unsigned long& length)
{
	FightScript* fight_script = &f_script;

	int dataLength = getBattel_FightScriptLength(fight_script);
	length = dataLength;
	char* msg = new char[dataLength];
	memset(msg,0,dataLength); // 清空，压缩率才高

	Battel_FightScript* script = (Battel_FightScript*)msg;

	// InitChessBoard
	script->init.myCurBloodNum = htons(fight_script->init.myCurBloodNum);
	script->init.myDefaultBloodNum = htons(fight_script->init.myDefaultBloodNum);
	script->init.oppositeCurBloodNum = htons(fight_script->init.oppositeCurBloodNum);
	script->init.oppositeDefaultBloodNum = htons(fight_script->init.oppositeDefaultBloodNum);
	for (int i=0;i<FormationHeroAcountMax;i++)   
	{
		script->init.myTeam[i].heroId = htonl(fight_script->init.myTeam[i].heroId);
		script->init.myTeam[i].hp = htonl(fight_script->init.myTeam[i].hp);
		script->init.myTeam[i].defaultHp = htonl(fight_script->init.myTeam[i].defaultHp);
		script->init.myTeam[i].level = htonl(fight_script->init.myTeam[i].level);
		script->init.myTeam[i].stage = htonl(fight_script->init.myTeam[i].stage);
		script->init.myTeam[i].profession_id = htonl(fight_script->init.myTeam[i].profession_id);
		script->init.myTeam[i].x_size = htons(fight_script->init.myTeam[i].x_size);
		script->init.myTeam[i].resonance_star= htons(fight_script->init.myTeam[i].resonance_star);
		script->init.myTeam[i].hero_type = htons(fight_script->init.myTeam[i].hero_type);
		script->init.myTeam[i].defaultCdTime = htons(fight_script->init.myTeam[i].defaultCdTime);
		// 装备信息
		{
			short equipNum = fight_script->init.myTeam[i].summary_equips.size();
			if (equipNum > MAX_EQUIP_NUM_EACH_ACTOR)
			{
				CCAssert(false,"");
				return NULL;
			}
			script->init.myTeam[i].zb_summary_Num = htons(equipNum);
			ZhuangbeiSummary* mySummary = (ZhuangbeiSummary*)script->init.myTeam[i].zhuangbei_summary_arr;
			for (int equipIndex=0;equipIndex<equipNum;equipIndex++)
			{
				mySummary[equipIndex].zhuangbei_type = htonl(fight_script->init.myTeam[i].summary_equips[equipIndex].zhuangbei_type);
				mySummary[equipIndex].zhuangbei_colour = htonl(fight_script->init.myTeam[i].summary_equips[equipIndex].zhuangbei_colour);
				strncpy(mySummary[equipIndex].zhuangbei_pic,fight_script->init.myTeam[i].summary_equips[equipIndex].zhuangbei_pic,ZhuangbeiPicMaxLen);
				mySummary[equipIndex].zhuangbei_pic[ZhuangbeiPicMaxLen] = 0;
			}
		}
		script->init.myTeamUniqueId[i] = htonl(fight_script->init.myTeamUniqueId[i]);


		script->init.oppositeTeam[i].heroId = htonl(fight_script->init.oppositeTeam[i].heroId);
		script->init.oppositeTeam[i].hp = htonl(fight_script->init.oppositeTeam[i].hp);
		script->init.oppositeTeam[i].defaultHp = htonl(fight_script->init.oppositeTeam[i].defaultHp);
		script->init.oppositeTeam[i].level = htonl(fight_script->init.oppositeTeam[i].level);
		script->init.oppositeTeam[i].stage = htonl(fight_script->init.oppositeTeam[i].stage);
		script->init.oppositeTeam[i].profession_id = htonl(fight_script->init.oppositeTeam[i].profession_id);
		script->init.oppositeTeam[i].x_size = htons(fight_script->init.oppositeTeam[i].x_size);
		script->init.oppositeTeam[i].resonance_star = htons(fight_script->init.oppositeTeam[i].resonance_star);
		script->init.oppositeTeam[i].hero_type = htons(fight_script->init.oppositeTeam[i].hero_type);
		script->init.oppositeTeam[i].defaultCdTime = htons(fight_script->init.oppositeTeam[i].defaultCdTime);
		// 装备信息
		{
			short equipNum = fight_script->init.oppositeTeam[i].summary_equips.size();
			if (equipNum > MAX_EQUIP_NUM_EACH_ACTOR)
			{
				CCAssert(false,"");
				return NULL;
			}
			script->init.oppositeTeam[i].zb_summary_Num = htons(equipNum);
			ZhuangbeiSummary* opSummary = (ZhuangbeiSummary*)script->init.oppositeTeam[i].zhuangbei_summary_arr;
			for (int equipIndex=0;equipIndex<equipNum;equipIndex++)
			{
				opSummary[equipIndex].zhuangbei_type = htonl(fight_script->init.oppositeTeam[i].summary_equips[equipIndex].zhuangbei_type);
				opSummary[equipIndex].zhuangbei_colour = htonl(fight_script->init.oppositeTeam[i].summary_equips[equipIndex].zhuangbei_colour);
				strncpy(opSummary[equipIndex].zhuangbei_pic,fight_script->init.oppositeTeam[i].summary_equips[equipIndex].zhuangbei_pic,ZhuangbeiPicMaxLen);
				opSummary[equipIndex].zhuangbei_pic[ZhuangbeiPicMaxLen] = 0;
			}
		}
		script->init.oppositeUniqueId[i] = htonl(fight_script->init.oppositeUniqueId[i]);

	}

	int stepNum = fight_script->stepVec.size();
	script->stepCount = htonl(stepNum);

	// Battel_OneStep 数组
	Battel_OneStep* step = script->step_arr;
	for (int i=0;i<stepNum;i++)
	{
		step->stepType = htons(fight_script->stepVec[i].stepType);

		char* next = NULL;
		// buff
		{
			int removeBuffNum = fight_script->stepVec[i].buff.remove_buff_array.size();
			step->buff.removeBuffNum = htonl(removeBuffNum);
			for (int removeIndex = 0;removeIndex < removeBuffNum;removeIndex++)
			{
				step->buff.remove_buff_arr[removeIndex] = htonl(fight_script->stepVec[i].buff.remove_buff_array[removeIndex]);
			}
			next = (char*)(step->buff.remove_buff_arr + removeBuffNum); // 指向effect_num

			int doingNum = fight_script->stepVec[i].buff.effects.size();
			int* effect_num = (int*)next;
			(*effect_num) = htonl(doingNum);
			next = (char*)(effect_num + 1); // 指向effects

			Battel_Buff_Doing* doing_arr = (Battel_Buff_Doing*)next;
			for (int doingInex = 0;doingInex < doingNum;doingInex++)
			{
				doing_arr[doingInex].target = htonl(fight_script->stepVec[i].buff.effects[doingInex].target);
				doing_arr[doingInex].hp_add = htonl(fight_script->stepVec[i].buff.effects[doingInex].hp_add);

				doing_arr[doingInex].isFantan = fight_script->stepVec[i].buff.effects[doingInex].isFantan;
				doing_arr[doingInex].fightor_u_id = htonl(fight_script->stepVec[i].buff.effects[doingInex].fightor_u_id);
				doing_arr[doingInex].relativedBuffStateId = htons(fight_script->stepVec[i].buff.effects[doingInex].relativedBuffStateId);
			}
			next = (char*)(doing_arr + doingNum); // 指向cdTimeNum
		}

		// cdtime
		int cd_num = fight_script->stepVec[i].cdTime_arr.size();
		int* cdTime = (int*)next;
		(*cdTime) = htonl(cd_num);
		next = (char*)(cdTime+1); // 指向cdDiscr_arr
		CdDiscr* cdDiscr_arr = (CdDiscr*)next;
		for (int j=0;j<cd_num;j++)
		{
			cdDiscr_arr[j].cdTime = htons(fight_script->stepVec[i].cdTime_arr[j].cdTime);
			cdDiscr_arr[j].uniqueId = htonl(fight_script->stepVec[i].cdTime_arr[j].uniqueId);
		}
		next = (char*)(cdDiscr_arr + cd_num); // 指向stepData

		if (fight_script->stepVec[i].stepType == StepType_Fight)
		{
			Battel_StepFight* fight =  (Battel_StepFight*)next;
			short num = fight_script->stepVec[i].fightData.targetUniqueVec.size();

			fight->fightorUniqueId = htonl(fight_script->stepVec[i].fightData.fightorUniqueId);
			fight->fightPattern = htons(fight_script->stepVec[i].fightData.fightPattern);
			fight->targetNum = htons(num);

			int shift = num > 1 ? num -1 : 0;
			// targetUniqueId_arr
			for (int targetIndex = 0;targetIndex < num;targetIndex++)
			{
				fight->targetUniqueId_arr[targetIndex] = htonl(fight_script->stepVec[i].fightData.targetUniqueVec[targetIndex]);
			}
			fight = (Battel_StepFight*) (((char*)fight) + sizeof(unsigned int) * shift);

			// canMiss
			for (int targetIndex = 0;targetIndex < num;targetIndex++)
			{
				fight->canMiss[targetIndex] = fight_script->stepVec[i].fightData.canMissVec[targetIndex];
			}
			fight = (Battel_StepFight*) (((char*)fight) + sizeof(bool) * shift);

			//targetBloodChange
			for (int targetIndex = 0;targetIndex < num;targetIndex++)
			{
				fight->targetBloodChange[targetIndex] = htonl(fight_script->stepVec[i].fightData.targetBloodChange[targetIndex]);
			}
			fight = (Battel_StepFight*) (((char*)fight) + sizeof(int) * shift);

			++fight;
			step = (Battel_OneStep*)fight;

		}
		else if (fight_script->stepVec[i].stepType == StepType_Move)
		{
			Battel_StepMove* move =  (Battel_StepMove*)next;

			move->destIndex = htonl(fight_script->stepVec[i].moveData.destIndex);
			move->uniqueId = htonl(fight_script->stepVec[i].moveData.uniqueId);
			move->inSelfChessBoard = fight_script->stepVec[i].moveData.inSelfChessBoard;


			++move;
			step = (Battel_OneStep*)move;
		}
		else if (fight_script->stepVec[i].stepType == StepType_ChangeRound)
		{
			Battel_StepChangeRound* change =  (Battel_StepChangeRound*)next;
			change->round = htonl(fight_script->stepVec[i].changeRound.round);

			++change;
			step = (Battel_OneStep*)change;
		}
		else if (fight_script->stepVec[i].stepType == StepType_UseBlood || fight_script->stepVec[i].stepType == StepType_ReLoadBlood)
		{
			Battel_StepUseBlood* useBlood = (Battel_StepUseBlood*)next;
			useBlood->uniqueId = htonl(fight_script->stepVec[i].useBlood.uniqueId);
			useBlood->bloodChangeForUser = htonl(fight_script->stepVec[i].useBlood.bloodChangeForUser);

			++useBlood;
			step = (Battel_OneStep*)useBlood;
		}
		else if (fight_script->stepVec[i].stepType == StepType_Skill)
		{
			Battel_StepSkill* skill = (Battel_StepSkill*)next;

			short num = fight_script->stepVec[i].skillData.targetUniqueVec.size();
			short buffNum = fight_script->stepVec[i].skillData.firedBuff.size();
			skill->fightorUniqueId = htonl(fight_script->stepVec[i].skillData.fightorUniqueId);
			skill->skillPatten = htons(fight_script->stepVec[i].skillData.skillPatten);
			skill->skillType = htons(fight_script->stepVec[i].skillData.skillType);
			skill->targetNum = htons(num);
			skill->buffNum = htons(buffNum);


			int offset_for_buf = 0;		// buff后面的数据的偏移：如果没有buff，就不偏移
			{//
				Battel_GameBuff* buff_arr = (Battel_GameBuff*)skill->gameBuff_arr;
				for (int j=0;j<buffNum;j++)
				{
					int eff_num = fight_script->stepVec[i].skillData.firedBuff[j].effect_vec.size();
					short state_id = fight_script->stepVec[i].skillData.firedBuff[j].state;
					bool cumulative = fight_script->stepVec[i].skillData.firedBuff[j].cumulative;
					unsigned int fire_uId = fight_script->stepVec[i].skillData.firedBuff[j].fireUId;
					unsigned int attachto_uId = fight_script->stepVec[i].skillData.firedBuff[j].attachedToUId;

					buff_arr->effectNum = htonl(eff_num);
					buff_arr->fireUId = htonl(fire_uId);
					buff_arr->attachedToUId = htonl(attachto_uId);
					buff_arr->buffStateId = htons(state_id);
					buff_arr->cumulative = cumulative;

					Battel_GameBuffEffect* buff_eff_arr = (Battel_GameBuffEffect*)buff_arr->gameBuffEff_arr;

					int temp_index = 0;
					for (list<ClientGameBuffEffect>::iterator it = fight_script->stepVec[i].skillData.firedBuff[j].effect_vec.begin();
						it != fight_script->stepVec[i].skillData.firedBuff[j].effect_vec.end();it++)
					{
						buff_eff_arr[temp_index].effect_id = htonl(it->effect_id);
						// 其他的不用转换
						buff_eff_arr[temp_index].xuanyun = it->xuanyun;
						buff_eff_arr[temp_index].fantan = it->fantan;
						buff_eff_arr[temp_index].hp_change_state = it->hp_change_state;
						buff_eff_arr[temp_index].baoji_change_state = it->baoji_change_state;
						buff_eff_arr[temp_index].gongjili_change_state = it->gongjili_change_state;
						buff_eff_arr[temp_index].shanbi_change_state = it->shanbi_change_state;
						buff_eff_arr[temp_index].fangyu_change_state = it->fangyu_change_state;

						temp_index ++;
					}

					++buff_arr;
					buff_arr = (Battel_GameBuff*) (((char*)buff_arr) + eff_num * sizeof(Battel_GameBuffEffect));

					// 记录偏移
					offset_for_buf += sizeof(Battel_GameBuff) + eff_num * sizeof(Battel_GameBuffEffect);
				}
			}
			
			skill = (Battel_StepSkill*) (((char*)skill) + offset_for_buf);

			int shift = num > 1 ? num -1 : 0;

			// targetId
			for (int j=0;j<num;j++)
			{
				skill->targetUniqueId_arr[j] = htonl(fight_script->stepVec[i].skillData.targetUniqueVec[j]);
			}
			skill = (Battel_StepSkill*) (((char*)skill) + sizeof(unsigned int) * shift);

			// canMiss
			for (int j=0;j<num;j++)
			{
				skill->canMiss[j] = fight_script->stepVec[i].skillData.canMissVec[j];
			}
			skill = (Battel_StepSkill*) (((char*)skill) + sizeof(bool) * shift);

			//targetBloodChange
			for (int j=0;j<num;j++)
			{
				skill->targetBloodChange[j] = htonl(fight_script->stepVec[i].skillData.targetBloodChange[j]);
			}
			skill = (Battel_StepSkill*) (((char*)skill) + sizeof(int) * shift);

			++skill;

			step = (Battel_OneStep*)skill;
		}
		else if (fight_script->stepVec[i].stepType == StepType_Wait)
		{
			step = (Battel_OneStep*)next;
		}
		else if (fight_script->stepVec[i].stepType == StepType_FailByTakeTooMuchTime)
		{
			step = (Battel_OneStep*)next;
		}
		else
		{
			CCLog("FightScriptBuilder::FightScript_2_Package invalide stepType[%d]",fight_script->stepVec[i].stepType);
			CCAssert(false,"");
		}
	}

	// 战后数据
	Battel_FightEnd* end = (Battel_FightEnd*)step;
	end->myResult = htons(fight_script->endResult.myResult);

	//for (int i=0;i<FormationHeroAcountMax;i++)
	//{
	//	hton_hero_detail(fight_script->endResult.heroDetailAttrAfterFight[i],end->heroDetailAttrAfterFight[i]);
	//}
	//char* next = (char*)(end->heroDetailAttrAfterFight + FormationHeroAcountMax); // 指向gettedExp

	char* next = (char*)(&(end->gettedExp)); // 指向gettedExp
	unsigned int* gettedExp = (unsigned int*)next;
	*gettedExp = htonl(fight_script->endResult.gettedExp);
	next = (char*)(gettedExp + 1); // 指向gettedItemSuiPianNum

	unsigned int* gettedItemSuiPianNum = (unsigned int*)next;
	*gettedItemSuiPianNum = htonl(fight_script->endResult.gettedItemSuiPian.size());
	next = (char*)(gettedItemSuiPianNum + 1); // 指向gettedItemSuiPian_arr
	ItemSuipianInfo* itemSuiPianInfor = (ItemSuipianInfo*)next;
	for (int i=0; i<(int)fight_script->endResult.gettedItemSuiPian.size();i++)
	{
		itemSuiPianInfor[i].item_id = htonl(fight_script->endResult.gettedItemSuiPian[i].item_id);
		itemSuiPianInfor[i].suipian_id = htonl(fight_script->endResult.gettedItemSuiPian[i].suipian_id);
		itemSuiPianInfor[i].suipian_count = htonl(fight_script->endResult.gettedItemSuiPian[i].suipian_count);
	}
	next = (char*)(itemSuiPianInfor + fight_script->endResult.gettedItemSuiPian.size()); // 指向gettedShenBingSuiPianNum

	unsigned int* gettedShenBingSuiPianNum = (unsigned int*)next;
	*gettedShenBingSuiPianNum = htonl(fight_script->endResult.gettedShenBingSuiPian.size());
	next = (char*)(gettedShenBingSuiPianNum + 1); // 指向gettedShenBingSuiPian_arr
	ShenbingSuipianInfo* shenbingSuiPian_arry = (ShenbingSuipianInfo*)next;
	for (int i=0; i<(int)fight_script->endResult.gettedShenBingSuiPian.size();i++)
	{
		shenbingSuiPian_arry[i].zb_group_id = htonl(fight_script->endResult.gettedShenBingSuiPian[i].zb_group_id);
		shenbingSuiPian_arry[i].suipian_id = htonl(fight_script->endResult.gettedShenBingSuiPian[i].suipian_id);
		shenbingSuiPian_arry[i].suipian_count = htonl(fight_script->endResult.gettedShenBingSuiPian[i].suipian_count);
	}
	next = (char*)(shenbingSuiPian_arry + fight_script->endResult.gettedShenBingSuiPian.size()); // 指向powerPointNum

	// 能量点
	int powerPointNum = fight_script->endResult.gettedPowerPoints.size();
	unsigned int* pPowerPointNum = (unsigned int*)next;
	*pPowerPointNum = htonl(powerPointNum);
	pPowerPointNum ++; // 指向powerPoint_arr

	PowerPoint* powerPointArray = (PowerPoint*)pPowerPointNum;
	for (int i=0;i<powerPointNum;i++)
	{
		powerPointArray[i].power = htonl(fight_script->endResult.gettedPowerPoints[i].power);
	}
	next = (char*)(powerPointArray + powerPointNum); // 指向zhuangbeiNum


	//
	int zhuangbeiNum = fight_script->endResult.gettedZhuangBei.size();
	int itemNum = fight_script->endResult.gettedItem.size();

	unsigned int* pZhuangbeiNum = (unsigned int*)next;
	*pZhuangbeiNum = htonl(zhuangbeiNum);
	pZhuangbeiNum ++; // 指向itemNum
	*pZhuangbeiNum = htonl(itemNum);
	pZhuangbeiNum ++;// 指向zhuangbei_arr

	ZhuangbeiDetail* zhuangbeiArray = (ZhuangbeiDetail*)pZhuangbeiNum;
	char* endPos = CS::hton_equipdata_2_zhuangbei_detail(fight_script->endResult.gettedZhuangBei,zhuangbeiArray);
	zhuangbeiArray = (ZhuangbeiDetail*)endPos;

	int test = ((char*)zhuangbeiArray) - msg;

	ItemInfo* item_array = (ItemInfo*)zhuangbeiArray;
	for (int i=0;i<itemNum;i++)
	{
		//CCLog("item[%d]",i);
		item_array[i].count = fight_script->endResult.gettedItem[i].count;
		item_array[i].type = fight_script->endResult.gettedItem[i].type;

		item_array[i].count = htonl(item_array[i].count);
		item_array[i].type = htonl(item_array[i].type);
	}

	return msg;
}

FightScript FightScriptBuilder::Battel_FightScript_2_FightScript(const char* data)
{
	FightScript out_script;
	Battel_FightScript* script = (Battel_FightScript*)data;

	//InitChessBoard
	out_script.init.myCurBloodNum = htons(script->init.myCurBloodNum);
	out_script.init.myDefaultBloodNum = htons(script->init.myDefaultBloodNum);
	out_script.init.oppositeCurBloodNum = htons(script->init.oppositeCurBloodNum);
	out_script.init.oppositeDefaultBloodNum = htons(script->init.oppositeDefaultBloodNum);
	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		out_script.init.myTeam[i].heroId = ntohl(script->init.myTeam[i].heroId);
		out_script.init.myTeam[i].hp = ntohl(script->init.myTeam[i].hp);
		out_script.init.myTeam[i].defaultHp = ntohl(script->init.myTeam[i].defaultHp);
		out_script.init.myTeam[i].level = ntohl(script->init.myTeam[i].level);
		out_script.init.myTeam[i].stage = ntohl(script->init.myTeam[i].stage);
		out_script.init.myTeam[i].profession_id = (Profession)ntohl(script->init.myTeam[i].profession_id);
		out_script.init.myTeam[i].x_size = htons(script->init.myTeam[i].x_size);
		out_script.init.myTeam[i].resonance_star= htons(script->init.myTeam[i].resonance_star);
		out_script.init.myTeam[i].hero_type = (HeroBaseType)htons(script->init.myTeam[i].hero_type);
		out_script.init.myTeam[i].defaultCdTime = htons(script->init.myTeam[i].defaultCdTime);
		// 装备信息
		{
			short equipNum = htons(script->init.myTeam[i].zb_summary_Num);
			if (equipNum > MAX_EQUIP_NUM_EACH_ACTOR)
			{
				CCAssert(false,"");
				equipNum = MAX_EQUIP_NUM_EACH_ACTOR;
			}
			ZhuangbeiSummary* summary_arr = (ZhuangbeiSummary*)script->init.myTeam[i].zhuangbei_summary_arr;
			for (int equipIndex=0;equipIndex<equipNum;equipIndex++)
			{
				ZhuangbeiSummary one;
				one.zhuangbei_type = htonl(summary_arr[equipIndex].zhuangbei_type);
				one.zhuangbei_colour = htonl(summary_arr[equipIndex].zhuangbei_colour);
				strncpy(one.zhuangbei_pic, summary_arr[equipIndex].zhuangbei_pic, ZhuangbeiPicMaxLen);
				one.zhuangbei_pic[ZhuangbeiPicMaxLen] = 0;

				out_script.init.myTeam[i].summary_equips.push_back(one);
			}
		}
		out_script.init.myTeamUniqueId[i] = ntohl(script->init.myTeamUniqueId[i]);


		out_script.init.oppositeTeam[i].heroId = ntohl(script->init.oppositeTeam[i].heroId);
		out_script.init.oppositeTeam[i].hp = ntohl(script->init.oppositeTeam[i].hp);
		out_script.init.oppositeTeam[i].defaultHp = ntohl(script->init.oppositeTeam[i].defaultHp);
		out_script.init.oppositeTeam[i].level = ntohl(script->init.oppositeTeam[i].level);
		out_script.init.oppositeTeam[i].stage = ntohl(script->init.oppositeTeam[i].stage);
		out_script.init.oppositeTeam[i].profession_id = (Profession)ntohl(script->init.oppositeTeam[i].profession_id);
		out_script.init.oppositeTeam[i].x_size = htons(script->init.oppositeTeam[i].x_size);
		out_script.init.oppositeTeam[i].resonance_star = htons(script->init.oppositeTeam[i].resonance_star);
		out_script.init.oppositeTeam[i].hero_type = (HeroBaseType)htons(script->init.oppositeTeam[i].hero_type);
		out_script.init.oppositeTeam[i].defaultCdTime = htons(script->init.oppositeTeam[i].defaultCdTime);
		// 装备信息
		{
			short equipNum = htons(script->init.oppositeTeam[i].zb_summary_Num);
			if (equipNum > MAX_EQUIP_NUM_EACH_ACTOR)
			{
				CCAssert(false,"");
				equipNum = MAX_EQUIP_NUM_EACH_ACTOR;
			}
			ZhuangbeiSummary* summary_arr = (ZhuangbeiSummary*)script->init.oppositeTeam[i].zhuangbei_summary_arr;
			for (int equipIndex=0;equipIndex<equipNum;equipIndex++)
			{
				ZhuangbeiSummary one;
				one.zhuangbei_type = htonl(summary_arr[equipIndex].zhuangbei_type);
				one.zhuangbei_colour = htonl(summary_arr[equipIndex].zhuangbei_colour);
				// 会堆栈溢出
				//strcpy(one.zhuangbei_pic,summary_arr[equipIndex].zhuangbei_pic);
				strncpy(one.zhuangbei_pic, summary_arr[equipIndex].zhuangbei_pic, ZhuangbeiPicMaxLen);
				one.zhuangbei_pic[ZhuangbeiPicMaxLen] = 0;

				out_script.init.oppositeTeam[i].summary_equips.push_back(one);
			}
		}
		out_script.init.oppositeUniqueId[i] = ntohl(script->init.oppositeUniqueId[i]);

	}
	Battel_OneStep* step = script->step_arr;
	int num = ntohl(script->stepCount);

	for (int i=0;i<num;i++)
	{
		OneStep one_step;
		one_step.stepType = (StepType)ntohs(step->stepType);

		char* next = NULL;
		// buff
		{
			int removeBuffNum = htonl(step->buff.removeBuffNum);
			for (int removeIndex = 0;removeIndex < removeBuffNum;removeIndex++)
			{
				one_step.buff.remove_buff_array.push_back(htonl(step->buff.remove_buff_arr[removeIndex]));
			}
			next = (char*)(step->buff.remove_buff_arr + removeBuffNum); // 指向effect_num
			int* effect_num = (int*)next;
			int doingNum = htonl(*effect_num);
			next = (char*)(effect_num + 1); // 指向effects

			// 他妈的bug！！！这里千万要用Battel_Buff_Doing而不是BBuff_Doing
			// 虽然他们内存模型一样，但是还是不能用，难道有方法的结构体数据结构不一样？？
			Battel_Buff_Doing* doing_arr = (Battel_Buff_Doing*)next; 

			for (int doingInex = 0;doingInex < doingNum;doingInex++)
			{
				Buff_Doing doing;
				doing.target = htonl(doing_arr[doingInex].target);
				doing.hp_add = htonl(doing_arr[doingInex].hp_add);
				doing.isFantan = doing_arr[doingInex].isFantan;
				doing.fightor_u_id = htonl(doing_arr[doingInex].fightor_u_id);
				doing.relativedBuffStateId = htons(doing_arr[doingInex].relativedBuffStateId);
				one_step.buff.effects.push_back(doing);
			}
			next = (char*)(doing_arr + doingNum); // 指向cdTimeNum
		}

		//
		int* cdTimeNum = (int*)next;
		int cd_num = ntohl(*cdTimeNum);
		next += sizeof(int); // 指向cdDiscr_arr
		CdDiscr* cd_discr_arr = (CdDiscr*)next;
		for (int j=0;j<cd_num;j++)
		{
			CdDiscr oneDisc;
			oneDisc.cdTime = ntohs(cd_discr_arr[j].cdTime);
			oneDisc.uniqueId = ntohl(cd_discr_arr[j].uniqueId);
			one_step.cdTime_arr.push_back(oneDisc);
		}
		next = (char*)(cd_discr_arr + cd_num); // 指向stepData

		if (one_step.stepType == StepType_Fight)
		{
			Battel_StepFight* fight = (Battel_StepFight*)next;
			int num = ntohs(fight->targetNum);

			one_step.fightData.fightorUniqueId = ntohl(fight->fightorUniqueId);
			one_step.fightData.fightPattern = (FightPattern)ntohs(fight->fightPattern);

			int shift = num > 1 ? num -1 : 0;
			for (int j=0;j<num;j++)
			{
				one_step.fightData.targetUniqueVec.push_back(ntohl(fight->targetUniqueId_arr[j]));
			}
			fight = (Battel_StepFight*) (((char*)fight) + sizeof(unsigned int) * shift);

			// canMiss
			for (int j=0;j<num;j++)
			{
				//one_step.skillData.canMissVec.push_back(ntohl(skill->canMiss[j]));
				one_step.fightData.canMissVec.push_back(fight->canMiss[j]);
			}
			fight = (Battel_StepFight*) (((char*)fight) + sizeof(bool) * shift);

			//targetBloodChange
			for (int j=0;j<num;j++)
			{
				one_step.fightData.targetBloodChange.push_back(ntohl(fight->targetBloodChange[j]));
			}
			fight = (Battel_StepFight*) (((char*)fight) + sizeof(int) * shift);


			//// 移位
			++fight;
			step = (Battel_OneStep*)fight;
		}
		else if (one_step.stepType == StepType_Move)
		{
			Battel_StepMove* move =  (Battel_StepMove*)next;

			one_step.moveData.destIndex = htonl(move->destIndex);
			one_step.moveData.uniqueId = htonl(move->uniqueId);
			one_step.moveData.inSelfChessBoard = move->inSelfChessBoard;

			// 移位
			++move;
			step = (Battel_OneStep*)move;
		}
		else if (one_step.stepType == StepType_ChangeRound)
		{
			Battel_StepChangeRound* change =  (Battel_StepChangeRound*)next;
			one_step.changeRound.round = htonl(change->round);

			// 移位
			++change;
			step = (Battel_OneStep*)change;
		}
		else if (one_step.stepType == StepType_UseBlood || one_step.stepType == StepType_ReLoadBlood)
		{
			Battel_StepUseBlood* useBlood = (Battel_StepUseBlood*)next;
			one_step.useBlood.uniqueId = htonl(useBlood->uniqueId);
			one_step.useBlood.bloodChangeForUser = htonl(useBlood->bloodChangeForUser);

			// 移位
			++useBlood;
			step = (Battel_OneStep*)useBlood;
		}
		else if (one_step.stepType == StepType_Skill)
		{
			Battel_StepSkill* skill =  (Battel_StepSkill*)next;

			one_step.skillData.fightorUniqueId = ntohl(skill->fightorUniqueId);
			one_step.skillData.skillPatten = (SkillPattern)ntohs(skill->skillPatten);
			one_step.skillData.skillType = (SkillType)ntohs(skill->skillType);
			int num = ntohs(skill->targetNum);
			int buffNum = ntohs(skill->buffNum);
			int offset_for_buf = 0;		// buff后面的数据的偏移：如果没有buff，就不偏移
			{//
				Battel_GameBuff* buff_arr = (Battel_GameBuff*)skill->gameBuff_arr;
				for (int j=0;j<buffNum;j++)
				{
					int eff_num = ntohl(buff_arr->effectNum);
					BuffStateId state_id = (BuffStateId)ntohs(buff_arr->buffStateId);
					Battel_GameBuffEffect* buff_eff_arr = (Battel_GameBuffEffect*)buff_arr->gameBuffEff_arr;
					ClientGameBuff buff;
					buff.state = state_id;
					buff.fireUId = ntohl(buff_arr->fireUId);
					buff.attachedToUId = ntohl(buff_arr->attachedToUId);

					buff.cumulative = buff_arr->cumulative;
					for (int h=0;h<eff_num;h++)
					{
						ClientGameBuffEffect eff(ntohl(buff_eff_arr[h].effect_id));
						// 下面的不用转换
						eff.xuanyun = buff_eff_arr[h].xuanyun;
						eff.fantan = buff_eff_arr[h].fantan;
						eff.hp_change_state = buff_eff_arr[h].hp_change_state;
						eff.baoji_change_state = buff_eff_arr[h].baoji_change_state;
						eff.gongjili_change_state = buff_eff_arr[h].gongjili_change_state;
						eff.shanbi_change_state = buff_eff_arr[h].shanbi_change_state;
						eff.fangyu_change_state = buff_eff_arr[h].fangyu_change_state;

						buff.effect_vec.push_back(eff);
					}
					one_step.skillData.firedBuff.push_back(buff);

					++buff_arr;
					buff_arr = (Battel_GameBuff*) (((char*)buff_arr) + eff_num * sizeof(Battel_GameBuffEffect));

					// 记录偏移
					offset_for_buf += sizeof(Battel_GameBuff) + eff_num * sizeof(Battel_GameBuffEffect);
				}
			}
			skill = (Battel_StepSkill*) (((char*)skill) + offset_for_buf);


			int shift = num > 1 ? num -1 : 0;

			for (int j=0;j<num;j++)
			{
				one_step.skillData.targetUniqueVec.push_back(ntohl(skill->targetUniqueId_arr[j]));
			}
			skill = (Battel_StepSkill*) (((char*)skill) + sizeof(unsigned int) * shift);

			// canMiss
			for (int j=0;j<num;j++)
			{
				//one_step.skillData.canMissVec.push_back(ntohl(skill->canMiss[j]));
				one_step.skillData.canMissVec.push_back(skill->canMiss[j]);
			}
			skill = (Battel_StepSkill*) (((char*)skill) + sizeof(bool) * shift);

			//targetBloodChange
			for (int j=0;j<num;j++)
			{
				one_step.skillData.targetBloodChange.push_back(ntohl(skill->targetBloodChange[j]));
			}
			skill = (Battel_StepSkill*) (((char*)skill) + sizeof(int) * shift);

			++skill;

			step = (Battel_OneStep*)skill;
		}
		else if (one_step.stepType == StepType_Wait)
		{
			step = (Battel_OneStep*)next;
		}
		else if (one_step.stepType == StepType_FailByTakeTooMuchTime)
		{
			step = (Battel_OneStep*)next;
		}
		else
		{
			CCAssert(false,"Unkown-StepType");
		}

		out_script.stepVec.push_back(one_step);
	}

	// 战后数据

	Battel_FightEnd* end = (Battel_FightEnd*)step;
	out_script.endResult.myResult = (FightResult)ntohs(end->myResult);

	//for (int i=0;i<FormationHeroAcountMax;i++)
	//{
	//	ntoh_hero_detail(end->heroDetailAttrAfterFight[i],out_script.endResult.heroDetailAttrAfterFight[i]);
	//}
	//char* next = (char*)(end->heroDetailAttrAfterFight + FormationHeroAcountMax); // 指向gettedExp

	char* next = (char*)(&(end->gettedExp)); // 指向gettedExp
	unsigned int* gettedExp = (unsigned int*)next;
	out_script.endResult.gettedExp = ntohl(*gettedExp);
	next = (char*)(gettedExp + 1); // 指向gettedItemSuiPianNum

	unsigned int* gettedItemSuiPianNum = (unsigned int*)next;
	int item_suipian_num = ntohl(*gettedItemSuiPianNum);
	next = (char*)(gettedItemSuiPianNum + 1); // 指向gettedItemSuiPian_arr
	ItemSuipianInfo* itemSuiPianInfor = (ItemSuipianInfo*)next;
	for (int i=0; i<item_suipian_num;i++)
	{
		itemSuiPianInfor[i].item_id = htonl(itemSuiPianInfor[i].item_id);
		itemSuiPianInfor[i].suipian_id = htonl(itemSuiPianInfor[i].suipian_id);
		itemSuiPianInfor[i].suipian_count = htonl(itemSuiPianInfor[i].suipian_count);

		out_script.endResult.gettedItemSuiPian.push_back(itemSuiPianInfor[i]);
	}
	next = (char*)(itemSuiPianInfor + item_suipian_num); // 指向gettedShenBingSuiPianNum

	unsigned int* gettedShenBingSuiPianNum = (unsigned int*)next;
	int shenbing_suipian_num = ntohl(*gettedShenBingSuiPianNum);
	next = (char*)(gettedShenBingSuiPianNum + 1); // 指向gettedShenBingSuiPian_arr
	ShenbingSuipianInfo* shenbingSuiPian_arry = (ShenbingSuipianInfo*)next;
	for (int i=0; i<shenbing_suipian_num;i++)
	{
		shenbingSuiPian_arry[i].zb_group_id = htonl(shenbingSuiPian_arry[i].zb_group_id);
		shenbingSuiPian_arry[i].suipian_id = htonl(shenbingSuiPian_arry[i].suipian_id);
		shenbingSuiPian_arry[i].suipian_count = htonl(shenbingSuiPian_arry[i].suipian_count);

		out_script.endResult.gettedShenBingSuiPian.push_back(shenbingSuiPian_arry[i]);
	}
	next = (char*)(shenbingSuiPian_arry + shenbing_suipian_num); // 指向powerPointNum

	// 能量点
	int powerPointNum = 0;
	unsigned int* pPowerPointNum = (unsigned int*)next;
	powerPointNum = htonl(*pPowerPointNum);
	pPowerPointNum ++; // 指向powerPoint_arr
	PowerPoint* pp_array = (PowerPoint*)pPowerPointNum;
	for (int i=0;i<powerPointNum ;i++)
	{
		pp_array[i].power = ntohl(pp_array[i].power);
		out_script.endResult.gettedPowerPoints.push_back(pp_array[i]);
	}
	next = (char*)(pp_array+powerPointNum); // 指向zhuangbeiNum


	int zhuangbeiNum = 0;
	int itemNum = 0;
	unsigned int* pZhuangbeiNum = (unsigned int*)next;
	zhuangbeiNum = htonl(*pZhuangbeiNum);
	pZhuangbeiNum ++; // 指向itemNum
	itemNum = htonl(*pZhuangbeiNum);
	pZhuangbeiNum ++;// 指向zhuangbei_arr


	ZhuangbeiDetail* zhuangbeiArray = (ZhuangbeiDetail*)pZhuangbeiNum;
	char* endPos = CS::ntoh_zhuangbei_detail_2_equipdata(zhuangbeiArray,zhuangbeiNum,out_script.endResult.gettedZhuangBei);
	zhuangbeiArray = (ZhuangbeiDetail*)endPos;

	ItemInfo* item_array = (ItemInfo*)zhuangbeiArray;
	for (int i=0;i<itemNum;i++)
	{
		ItemInfo item;

		item.count = ntohl(item_array[i].count);
		item.type = ntohl(item_array[i].type);
		out_script.endResult.gettedItem.push_back(item);
	}

	return out_script;
}


TeamBaseProperty FightScriptBuilder::combineActorProperty(Hero* actor)
{
	CCAssert(actor->isOpEnd(),"必须是OpEnd之后合并数据才是正确的");
	//这里的Hero必定是调用了zhuangbeiOpEnd了的
	TeamBaseProperty ret;

	PropertyCalculator cal;

	ret.leaderProfression = actor->getProfession();
	ret.mLeaderId = 0;
	ret.mHP = actor->getHp(); // 已经有血量加成了，因为actor必定是调用了zhuangbeiOpEnd了的
	ret.mLevel = actor->getLevel();

	CalculateData calData = actor->getCalculateData();

	float gongji_min;
	float gongji_max;
	float gongji_min_add;
	float gongji_max_add;
	EquipCalculator equipCal;
	equipCal.getEquipGongJiLi(actor->equipVec,gongji_min,gongji_max,gongji_min_add,gongji_max_add);
	ret.mGongJiLi_MIN = gongji_min + gongji_min_add;
	ret.mGongJiLi_MAX = gongji_max + gongji_max_add;

	// 血量和攻击加成，血量已经在Hero的时候算过了，
	// 这里之所有要单独计算攻击，而不是根据PropertyCalculator来获取攻击
	// 是因为我们要的是独立的最小攻击和最大攻击，所有只能这里分开算装备的，然后再加成
	{
		float gongji_min = ret.mGongJiLi_MIN;
		float gongji_max = ret.mGongJiLi_MAX;
		int bloodAdd = 0;
		int gongjiAdd = 0;
		CS::getProfesionBloodAndGongJiAddPercent(actor->getProfession(),bloodAdd,gongjiAdd);
		float gongjiPer = ((float)gongjiAdd)/100.0f;
		gongji_min += gongji_min * gongjiPer;
		gongji_max += gongji_max * gongjiPer;

		// 真身的强化共鸣加成,只有攻击是单独算的，所有这里需要加上攻击的加成
		{
			if (!actor->getHeroBaseProperty_R()->isYuanShen)
			{
				float addPer = ZhuangbeiGongMingManager::getManager()->getPropertyAdd(actor->equipVec,GM_GongJiAddPer);
				addPer = addPer/100.0f;

				gongji_min += gongji_min * addPer;
				gongji_max += gongji_max * addPer;
			}
		}

		// 更新攻击值
		ret.mGongJiLi_MIN = gongji_min;
		ret.mGongJiLi_MAX = gongji_max;
	}

	ret.mGongJiJuLi = actor->getGongJiJuLi();
	ret.mBaoJiLv = cal.calculateBaoJiLv(calData);
	ret.mBaoJiShangHaiYinZi = cal.calculateBaoJiShangHaiYinzi(calData);
	ret.mJiNengFaChuLv = cal.calculateJiNengChuFaLv(calData);
	if (actor->skilVec.size() > 0)
	{
		ret.mJiNengZhouQi = cal.calculateJiNengZhouQi(calData);
		ret.mJiNengTurn = cal.calculateJiNengTurn(calData);
		ret.mJiNengResultPercent = cal.calculateJiNengResult(calData);
	}
	else
	{
		ret.mJiNengZhouQi = 0;
		ret.mJiNengTurn = 0;
		ret.mJiNengResultPercent = 0;
	}
	

	ret.mShangHaiLeiXing = actor->getShangHaiLeiXing();
	ret.mShanBiLv = cal.calculateShanBiLv(calData);
	ret.mGeDangZhi = cal.calculateWuLiGeDangZhi(calData);
	ret.mGeDangLv = cal.calculateGeDangLv(calData);
	ret.mHuJiaZhi = cal.calculateHuJiaZhi(calData);
	ret.mKangXingZhi = cal.calculateKuangXingZhi(calData);

	// 不是元神，就给攻防血加成，但是血量已经加成过了（zhuangbeiOpEnd的时候计算血量就加成过了），就不再加成了
	//if (!calData.basePro->isYuanShen)
	if (!calData.base_pro.is_yuanshen)
	{
		//float addtionPer = ((float)CS::getHeroStageEffectRatio(calData.basePro->currentStage))/100.0f;
		float addtionPer = ((float)CS::getHeroStageEffectRatio(calData.base_pro.stage))/100.0f;
		ret.mHuJiaZhi += ((float)ret.mHuJiaZhi) * addtionPer;
		ret.mKangXingZhi += ((float)ret.mKangXingZhi) * addtionPer;
		ret.mGongJiLi_MIN += ((float)ret.mGongJiLi_MIN) * addtionPer;
		ret.mGongJiLi_MAX += ((float)ret.mGongJiLi_MAX) * addtionPer;
		// 血量已经是加成过了，不用再加成了
	}

	ret.mDefaultFreezyNum = 0;
	return ret;
}


FightScriptBuilder::~FightScriptBuilder(void)
{
}