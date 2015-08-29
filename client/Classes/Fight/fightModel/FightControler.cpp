#include "FightControler.h"
#include "ChessBoardHelper_TeamFight.h"
#include "ChessBoardHelper.h"
#include "FightScriptBuilder.h"
//#include "SkillHelper.h"
#include "PropertyCalculator.h"
#include "GameBuffProc.h"
//#include "SkillHelper_TF.h"
#include "SkillHelperAdapterImpl.h"
#include "SkillHelperBase.h"


#if _CLIENT_
#include "cocos2d.h"
using namespace cocos2d;
#endif

#if (USE_FIGHT_LOG == 1)
#define F_LOG(format, ...)     CCLog(format, ##__VA_ARGS__)
#else
#define F_LOG(format, ...)     
#endif

extern float lfRandom0_1();

FightControler::FightControler(ChessBoardHelper* helper,GameBuffProc* proc)
{
	this->turnIndexA = 0;
	this->turnIndexB = 0;
	this->A_fightedHp = 0;
	this->B_fightedHp = 0;
	mHelper = helper;
	mBuffProc = proc;
	//mSkillHelper = new SkillHelper(mHelper,this);
	calculator = new PropertyCalculator();

	mSkillHelperAdatper = new SkillHelperAdapterImpl(helper,this);
	mSkillHelper = new SkillHelperBase(mSkillHelperAdatper);
}

ChessBoardItem* FightControler::getOneItemFromBoard(ChessBoard* board,int uniqueId)
{
	vector<ChessBoardItem*> items = board->getInBoardItems();
	int num = items.size();
	for (int i=0;i<num;i++)
	{
		if (items[i]->getUniqueId() == uniqueId && items[i]->hero->getHp() > 0)
		{
			return items[i];
		}
	}
	return NULL;
}
ChessBoardItem* FightControler::getOneItemFrom2Board(int uniqueId)
{
	ChessBoardItem* itemA = getOneItemFromBoard(mHelper->getBoardA(),uniqueId);
	ChessBoardItem* itemB = getOneItemFromBoard(mHelper->getBoardB(),uniqueId);
	CCAssert(itemA == NULL || itemB == NULL,"不可能同时找到");
	return itemA == NULL?itemB : itemA;
}

void FightControler::initFirstFightBoard(FirstBloodBoard firstBoard)
{
	firstBloodBoard = NULL;
	// 攻击方的棋盘
	ChessBoard* board[2] = {NULL};//board[0]是先手，board[1]是后手
	mFightQueue.clear();
	mCurrentFightIndex = 0;
	this->turnIndexA = 0;
	this->turnIndexB = 0;
	this->A_fightedHp = 0;
	this->B_fightedHp = 0;

	A_LastTurnFightHistory.clear();
	B_LastTurnFightHistory.clear();
	boardAInitUiqueId.clear();
	boardBInitUiqueId.clear();

	// 初始化各个棋盘初始的成员Id，用于统计砍血量时
	// 根据u_id知道这个元素师哪个board的,因为我们不能通过Id，找ChessBoardItem，再得到原始棋盘
	// 原因是ChessBoardItem可能就是NULL了，因为已经过了N步，那么Buff的原因可能这个Id的元素
	// 在N步之后就挂了，所以根据ChessBoardItem是不能操作的。
	{
		vector<ChessBoardItem*> itemsInA = mHelper->getAlivedItemsByOriginBoard(mHelper->getBoardA());
		int num_a = itemsInA.size();
		for (int i=0;i<num_a;i++)
		{
			boardAInitUiqueId.push_back(itemsInA[i]->getUniqueId());
		}
		

		vector<ChessBoardItem*> itemsInB = mHelper->getAlivedItemsByOriginBoard(mHelper->getBoardB());
		int num_b = itemsInB.size();
		for (int i=0;i<num_b;i++)
		{
			boardBInitUiqueId.push_back(itemsInB[i]->getUniqueId());
		}
	}

	if (firstBoard == FBB_BoardA)
	{
		board[0] = mHelper->getBoardA();
		board[1] = mHelper->getBoardB();
	}
	else if(firstBoard == FBB_BoardB)
	{
		board[0] = mHelper->getBoardB();
		board[1] = mHelper->getBoardA();
	}
	else if(firstBoard == FBB_Random)
	{
		if (lfRandom0_1() > 0.5f)
		{
			board[0] = mHelper->getBoardA();
			board[1] = mHelper->getBoardB();
		}else
		{
			board[0] = mHelper->getBoardB();
			board[1] = mHelper->getBoardA();
		}
	}
	else 
	{
		F_LOG("Error : %s --> unsported FirstBloodBoard = %d",__FUNCTION__,firstBoard);
		CCAssert(false,"不支持的先手配置");
	}
	//// 确定先手
	//{
	//	int mingJieA = mHelper->getBoardA()->getMingJie();
	//	int mingJieB = mHelper->getBoardB()->getMingJie();

	//	if (mingJieA > mingJieB)
	//	{
	//		board[0] = mHelper->getBoardA();
	//		board[1] = mHelper->getBoardB();
	//	}
	//	else if (mingJieB > mingJieA)
	//	{
	//		board[0] = mHelper->getBoardB();
	//		board[1] = mHelper->getBoardA();
	//	}
	//	else
	//	{
	//		if (lfRandom0_1() > 0.5f)
	//		{
	//			board[0] = mHelper->getBoardA();
	//			board[1] = mHelper->getBoardB();
	//		}else
	//		{
	//			board[0] = mHelper->getBoardB();
	//			board[1] = mHelper->getBoardA();
	//		}
	//	}
	//}
	
	// 初始化 攻击顺序
	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		{
			ChessBoardItem* item = board[0]->getActorItem(i%2,i/2,false);
			if (item != NULL)// 有角色
			{
				mFightQueue.push_back(item->getUniqueId());
			}
		}
		{
			ChessBoardItem* item = board[1]->getActorItem(i%2,i/2,false);
			if (item != NULL)// 有角色
			{
				mFightQueue.push_back(item->getUniqueId());
			}
		}
	}

	firstBloodBoard = board[0];

	// log
	{
		F_LOG("initFirstFightBoard --> Board Is %s",board[0] == mHelper->getBoardA()?"A":"B");
	}
}
//
void FightControler::fight_normal(OneStep& outStep,ChessBoardItem* fightor,vector<ChessBoardItem*> attackedItems)
{
	CCAssert(attackedItems.size() > 0,"要有攻击对象");

	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	outStep.stepType = StepType_Fight;
	outStep.fightData.fightorUniqueId = fightor->getUniqueId();
	outStep.fightData.fightPattern = lfRandom0_1() <= baoJilv?Fight_Normal_BaoJi:Fight_Normal;

#if (USE_FIGHT_LOG == 1)
	if (attackedItems.size() > 1)
	{
		CCLog("-----------------------normal fight multiple targets-------------------");
	}
#endif

	int num = attackedItems.size();
	ChessBoardItem* attackedItem = NULL;
	for (int i=0;i<num;i++)
	{
		attackedItem = attackedItems[i];
		outStep.fightData.targetUniqueVec.push_back(attackedItem->getUniqueId());

		int bloodChange = 0;
		{
			if (outStep.fightData.fightPattern == Fight_Normal_BaoJi)
			{
				// 打出最大值
				float normalShangHaiValue = fightor->hero->getShangHaiValue(attackedItem->hero,Range_Max);
				float baoShangYinZi = ((float)fightor->hero->getBaoJiShangHaiYinZi())/100.0f;
				bloodChange = -normalShangHaiValue * baoShangYinZi;
			}else
			{
				float normalShangHaiValue = fightor->hero->getShangHaiValue(attackedItem->hero);
				bloodChange = -normalShangHaiValue;
			}
		}
		
		outStep.fightData.targetBloodChange.push_back(bloodChange);

		float shanbilv = ((float)attackedItem->hero->getShanBiLv())/100.0f;
		bool canMiss = (lfRandom0_1() <= shanbilv);
		outStep.fightData.canMissVec.push_back(canMiss);

		// 没有Miss 就要减血
		if (!canMiss)
		{
			// 实际也要减血
			attackedItem->hero->addHp(bloodChange);

			mBuffProc->beattacked(fightor->getUniqueId(),attackedItem->getUniqueId(),bloodChange,outStep.buff);

			F_LOG("fight : beattackedItem[%d,%d] mHpChange = %d ,target hp = %d",attackedItem->posx,attackedItem->posy,bloodChange,attackedItem->hero->getHp());
		}
		else
		{
			F_LOG("fight  --> Miss : beattackedItem[%d,%d] mHpChange = %d ,target hp = %d",attackedItem->posx,attackedItem->posy,bloodChange,attackedItem->hero->getHp());
		}
	}

	if (num > 0)
	{
		mBuffProc->fight_end(fightor->getUniqueId(),outStep.buff);
	}

#if (USE_FIGHT_LOG == 1)
	if (attackedItems.size() > 1)
	{
		CCLog("-----------------------normal fight multiple targets-------------------\n");
	}
#endif
}
//
void FightControler::fight_skill(OneStep& outStep,SkillDataForFight skill,unsigned int f_uniqueId,SkillFight skillFight)
{
	CCAssert(skillFight.targetVec.size() > 0,"要有攻击对象");
	outStep.stepType = StepType_Skill;
	outStep.skillData.skillType = skill.skillType;
	outStep.skillData.fightorUniqueId = f_uniqueId;
	outStep.skillData.canMissVec = skillFight.canMissVec;
	outStep.skillData.skillPatten = skillFight.skillPatten;
	outStep.skillData.targetBloodChange = skillFight.targetBloodChange;
	outStep.skillData.targetUniqueVec = skillFight.targetVec;
	//转换成client的
	int buff_size = skillFight.firedBuff.size();
	for (int buffIndex =0;buffIndex < buff_size;buffIndex++)
	{
		ClientGameBuff buff(skillFight.firedBuff[buffIndex]);
		outStep.skillData.firedBuff.push_back(buff);
	}

	int targetNum = outStep.skillData.targetUniqueVec.size();

	// 先调用fightEnd
	if (targetNum > 0)
	{
		mBuffProc->fight_end(f_uniqueId,outStep.buff);
	}

	for (int i = 0;i<targetNum;i++)
	{
		ChessBoardItem* oneTarget = mHelper->getOneItemByUniqueId(outStep.skillData.targetUniqueVec[i]);
		CCAssert(oneTarget!=NULL,"必须存在");
		CCAssert(oneTarget->hero->getHp() > 0,"必须活的");

		// 没有Miss 就要减血,附加buff
		if (!outStep.skillData.canMissVec[i])
		{
			// 实际也要减血
			oneTarget->hero->addHp(outStep.skillData.targetBloodChange[i]);

			mBuffProc->beattacked(f_uniqueId,oneTarget->getUniqueId(),outStep.skillData.targetBloodChange[i],outStep.buff);
			// 还有血,则加buff
			if (oneTarget->hero->getHp() > 0 && skillFight.firedBuff.size() > 0)
			{

				// buff 附加
				int bufNum = skillFight.firedBuff.size();
				for (int buffIndex = 0;buffIndex < bufNum;buffIndex++)
				{
					if (skillFight.firedBuff[buffIndex].attachedToUId == oneTarget->getUniqueId())
					{
						// 附加buff
						// 是否有这个buff，有就更新
						if (!skillFight.firedBuff[buffIndex].cumulative && oneTarget->hero->haveBuffWithStateIs(skillFight.firedBuff[buffIndex].state))
						{
							bool suc = oneTarget->hero->updateBuff(skillFight.firedBuff[buffIndex]);
							CCAssert(suc,"这必须成功，不然就是bug");
						}else
						{
							bool suc = oneTarget->hero->attachBuff(skillFight.firedBuff[buffIndex]);
							CCAssert(suc,"这必须成功，不然就是bug");
						}

						F_LOG("Skill [%s] -->Buff: beattackedItem[%d,%d],\n####{%s}####",
							skillFight.nameForLog.c_str(),
							oneTarget->posx,oneTarget->posy,
							outStep.skillData.firedBuff[buffIndex].getLog().c_str()
							);
					}
				}
			}// oneTarget->hero->getHp() > 0

			F_LOG("Skill [%s] : beattackedItem[%d,%d] mHpChange = %d ,target hp = %d \n",
				skillFight.nameForLog.c_str(),
				oneTarget->posx,oneTarget->posy,
				outStep.skillData.targetBloodChange[i],
				oneTarget->hero->getHp());
		}// !outStep.skillData.canMissVec[i]
		else
		{
			F_LOG("Skill [%s] --> Miss : beattackedItem[%d,%d] mHpChange = %d ,target hp = %d \n",
				skillFight.nameForLog.c_str(),
				oneTarget->posx,oneTarget->posy,
				outStep.skillData.targetBloodChange[i],
				oneTarget->hero->getHp());
		}
	}// int i = 0;i<targetNum;i++


	/*if (targetNum > 0)
	{
		mBuffProc->fight_end(f_uniqueId,outStep.buff);
	}*/
}

void FightControler::tryFightWithSkill(ChessBoardItem* fightor,OneStep& out_step)
{
	out_step.skillData.targetUniqueVec.clear();
	SkillDataForFight skill = fightor->hero->skilVec[0];
	SkillFight skillFight = mSkillHelper->fight(fightor,skill);
	if (skillFight.targetVec.size() > 0)
	{
		fightor->hero->fightTurnInc(); // 更新攻击轮次

		// 通知棋盘，这是一个可看到的步奏
		mHelper->one_visible_step(out_step.cdTime_arr,fightor->getUniqueId());
		mBuffProc->one_visible_step(out_step.buff);

		// 后攻击，不然刚攻击打出的buff就在one_visible_step被移除了
		fight_skill(out_step,skill,fightor->getUniqueId(),skillFight);

		F_LOG("skill fight step buffData = **[%s]**",out_step.getBuffString().c_str());
	}
}

bool FightControler::tryMove(ChessBoardItem* fightor,OneStep& out_step)
{
	ChessBoard* move_to_board;
	int posX;
	int posY;
	bool moved = false;
	if (!chessBoardItemTryGoStep(fightor,move_to_board,posX,posY))
	{
		return false;
	}

	F_LOG("Move %s[%d,%d] ---> %s[%d,%d]",
		fightor->currentBoard->getName().c_str(),
		fightor->posx,fightor->posy,
		move_to_board->getName().c_str(),
		posX,posY);

	moved = true;
	out_step.stepType = StepType_Move;
	setItemMoveTo(out_step.moveData,fightor,move_to_board,posX,posY);


	// 通知棋盘，这是一个可看到的步奏，这里先移动还是先one_visible_step无所谓
	mHelper->one_visible_step(out_step.cdTime_arr,fightor->getUniqueId());
	mBuffProc->one_visible_step(out_step.buff);

	F_LOG("move step buffData = **[%s]**",out_step.getBuffString().c_str());
	// 已经死了，就返回
	if (fightor->hero->getHp() <= 0)
	{
		F_LOG("fightor dead after move...");
	}

	return true;
}

bool FightControler::canUseSkillInThisTurnAndCurrentSituation(ChessBoardItem* fightor)
{
	bool try_use_skill = true;
	do // 技能的触发决策
	{
		// 不能用技能，那么直接返回
		if (!fightor->hero->canUseThisInCurrentFightTurn())
		{
			try_use_skill = false;
			break;
		}
		//// 是神之信念(加防)，就需要策略
		//if (fightor->hero->skilVec[0].skillType == SkillType_ShenZhiXingNian || fightor->hero->skilVec[0].skillType == SkillType_JianShou)
		//{
		//	//看是否可以直接攻击
		//	ChessBoardItem* attackedItem= prepareChessBoardBeFightedItem(fightor);
		//	if (attackedItem != NULL && attackedItem->hero->getHp() > 0)
		//	{
		//		// 没有暴击的考虑
		//		int hp_sub = fightor->hero->getShangHaiValue(attackedItem->hero,Range_Min);
		//		if (hp_sub >= attackedItem->hero->getHp())	// 弄得死对方，就不用加防
		//		{
		//			try_use_skill = false;
		//			break;
		//		}
		//	}

		//	// 可能需要提前走一步才能试探攻击
		//	if (attackedItem == NULL || attackedItem->hero->getHp() <= 0)
		//	{
		//		ChessBoard* oldBoard = fightor->currentBoard;
		//		int old_pos_x = fightor->posx;
		//		int old_pos_y = fightor->posy;

		//		ChessBoard* move_to_board;
		//		int posX;
		//		int posY;
		//		// 能移动
		//		if (chessBoardItemTryGoStep(fightor,move_to_board,posX,posY))
		//		{
		//			int bloodSubByBuff = 0;	// buff的减血量
		//			list<GameBuff> buffs = fightor->hero->getCurrentAttachedBuff();
		//			// 看是否有减血的buff，如果移动之后就挂了，那么还不如加防再挂
		//			for (list<GameBuff>::iterator it_buf = buffs.begin();
		//				it_buf != buffs.end();it_buf++)
		//			{
		//				if (it_buf->effect_vec.empty())
		//					continue;

		//				bloodSubByBuff += -(it_buf->effect_vec.begin())->persistent_hp_add;
		//			}
		//			// buff就会减血让我死，那么就只能尝试用加防
		//			if (bloodSubByBuff >= fightor->hero->getHp())
		//			{
		//				try_use_skill = true;
		//				break;
		//			}


		//			// 临时改位置
		//			fightor->currentBoard = move_to_board;
		//			fightor->posx = posX;
		//			fightor->posy = posY;

		//			//再次看是否可以攻击
		//			ChessBoardItem* attackedItem= prepareChessBoardBeFightedItem(fightor);
		//			if (attackedItem != NULL && attackedItem->hero->getHp() > 0)
		//			{
		//				// 没有暴击的考虑
		//				int hp_sub = fightor->hero->getShangHaiValue(attackedItem->hero,Range_Min);
		//				if (hp_sub >= attackedItem->hero->getHp())	// 弄得死对方，就不用加防
		//				{
		//					try_use_skill = false;
		//				}
		//			}

		//			// 恢复位置
		//			fightor->currentBoard = oldBoard;
		//			fightor->posx = old_pos_x;
		//			fightor->posy = old_pos_y;
		//		}
		//	}

		//	break;
		//}

	} while (false);



	bool useSkill = fightor->hero->canUseThisInCurrentFightTurn();
	do 
	{
		if (fightor->hero->skilVec.size() == 0)
			break;

		//// 加防，没有攻击的技能就不触发了
		//SkillType skill = fightor->hero->skilVec[0].skillType;
		//if( skill == SkillType_ShenZhiXingNian || 
		//	skill == SkillType_JianShou)
		//{
		//	//chufaLv = -1;
		//	useSkill = false;
		//	break;
		//}

	} while (false);


	return useSkill && try_use_skill;
}
// 
vector<OneStep> FightControler::fight(ChessBoardItem* fightor)
{
	CCAssert(fightor!=NULL,"");


    // BOSS怪 特殊表现处理
    //bool isBOSSUseBlood = false;    //boss加血
    bool isSkillSpecialTrigger = false; //技能特殊处理
    if(!fightor->hero->skilVec.empty()) {
        if(fightor->hero->skilVec[0].skillType == SkillType_BOSS_Sweep) {
            //血量小于15%且没有触发过
            if((float)fightor->hero->getHp()/(float)fightor->hero->getDefaultHp() <= 0.15f && 
                !fightor->hero->getSpecialTrigger()) {
                isSkillSpecialTrigger = true;
            }
        }
        /*
        else if(fightor->hero->skilVec[0].skillType == SkillType_BOSS_Flog) {
            //血量小于30%且没有触发过
            if((float)fightor->hero->getHp()/(float)fightor->hero->getDefaultHp() <= 0.30f) {
                isBOSSUseBlood = true;
            }
        }
        */
    }

    
	bool useBlood = false;
	OneStep stepUseBlood;
	// 看是否需要加血，需要的话，就尝试加血
	// 喝血的条件：需要喝血(失血过半)+不在cd时间+有血瓶
	if ( (float)fightor->hero->getHp()/(float)fightor->hero->getDefaultHp() <= 0.5f &&
		 //fightor->getCurrentBloodCdTime() <= 0 &&
		 fightor->hero->getCdTime() <= 0 &&
		 fightor->getOriginBoard()->getCurrentBloodNum() > 0 
		 //&& fightor->hero->getXSize() < 2 /* 这句是排除boss 免得boss也会加血 因为boss需要特殊处理 */
        )
	{
		useBlood = true;
		stepUseBlood.stepType = StepType_UseBlood;
		stepUseBlood.useBlood.uniqueId = fightor->getUniqueId();

		// 真实的加血
		stepUseBlood.useBlood.bloodChangeForUser = fightor->hero->getDefaultHp() * fightor->getOriginBoard()->getBloodAddPercent() / 100.0f;
		fightor->hero->addHp(stepUseBlood.useBlood.bloodChangeForUser);
		short bloodNum = fightor->getOriginBoard()->getCurrentBloodNum()-1;
		fightor->setOriginBoardCanUsedBlooldNum(bloodNum>0?bloodNum:0);
		fightor->hero->setCdTime(fightor->hero->getDefaultCdTime());

		F_LOG("Use one Blood -> bloodAdd = %d, fightor[u_id = %u,originBoard = %s,posx = %d,posy = %d]",
			stepUseBlood.useBlood.bloodChangeForUser,
			fightor->getUniqueId(),fightor->getOriginBoard()->getName().c_str(),fightor->posx,fightor->posy);
	}

    /*
    // boss触发加血
    if(isBOSSUseBlood) {
        stepUseBlood.stepType = StepType_UseBlood;
		stepUseBlood.useBlood.uniqueId = fightor->getUniqueId();

		// 真实的加血
		stepUseBlood.useBlood.bloodChangeForUser = fightor->hero->getDefaultHp() * fightor->getOriginBoard()->getBloodAddPercent() /100.0f;
		fightor->hero->addHp(stepUseBlood.useBlood.bloodChangeForUser);
        short bloodNum = fightor->getOriginBoard()->getCurrentBloodNum()-1;
		fightor->setOriginBoardCanUsedBlooldNum(bloodNum>0?bloodNum:0);
		fightor->hero->setCdTime(fightor->hero->getDefaultCdTime());

		F_LOG("Use one Blood -> bloodAdd = %d, fightor[u_id = %u,originBoard = %s,posx = %d,posy = %d]",
			stepUseBlood.useBlood.bloodChangeForUser,
			fightor->getUniqueId(),fightor->getOriginBoard()->getName().c_str(),fightor->posx,fightor->posy);
    }
    */


	bool isSkillGiveUp = false; // 技能放弃攻击，转交给普攻
	// 先使用技能
	{
		// 技能
		if (isSkillSpecialTrigger || 
            canUseSkillInThisTurnAndCurrentSituation(fightor))
		{
			F_LOG("Use Skill .......");
            if(isSkillSpecialTrigger) { //把boss特殊处理标志置成已经触发
                fightor->hero->setSpecialTrigger();
            }

			const float float_accuracy = 0.0001f;
			if (fightor->hero->getGongJiJuLi() > (1.0f+float_accuracy) )
			{// 远程，那么就先看是否可以攻击，有就不移动，没有就尝试移动，移动成功了再看是否可以攻击

				F_LOG("Use Skill --> gongjijuli > 1.0f !");

				vector<OneStep> ret;

				//1. 加血 or boss加血
				if (useBlood /*|| isBOSSUseBlood*/)	
					ret.push_back(stepUseBlood);

				//2. 攻击
				OneStep step_skill;
				tryFightWithSkill(fightor,step_skill);
				if (!step_skill.skillData.targetUniqueVec.empty())
				{
					ret.push_back(step_skill);
					return ret;
				}
				 
				//3. 尝试移动   ---> 移动中死了，算倒霉
				OneStep stepMove;
				bool moved = tryMove(fightor,stepMove);
				if (moved)
				{
					ret.push_back(stepMove);
					if (fightor->hero->getHp() <= 0)
					{
						F_LOG("SKill : fightor dead after move...");
						return ret;
					}else
					{
						//4. 再次尝试攻击
						OneStep step_skill2;
						tryFightWithSkill(fightor,step_skill2);
						if (!step_skill2.skillData.targetUniqueVec.empty())
						{
							ret.push_back(step_skill2);
							return ret;
						}else
						{
							return ret; // 只返回移动+【血】
						}
					}
				}
				else
				{
					isSkillGiveUp = true;
					F_LOG("SKill : can not move and no target for skill,GiveUp!");
				}
			}
			else
			{// 近战，那么就先看是否可以移动，移动了之后再攻击

				F_LOG("Use Skill --> getGongJiJuLi() <= 1.0f!");

				vector<OneStep> ret;

				// 1. 加血
				if (useBlood)	
					ret.push_back(stepUseBlood);

				// 2.尝试移动 --> 移动中死了算倒霉
				OneStep stepMove;
				bool moved = tryMove(fightor,stepMove);
				if (moved)
				{
					ret.push_back(stepMove);
					if (fightor->hero->getHp() <= 0)
					{
						F_LOG("SKill : fightor dead after move...");
						return ret;
					}
				}
				// 3.尝试攻击
				F_LOG("SKill : fight use skill now after try move");
				OneStep step_skill;
				tryFightWithSkill(fightor,step_skill);
				if (!step_skill.skillData.targetUniqueVec.empty())
				{
					ret.push_back(step_skill);
					return ret;
				}else
				{
					if (moved)
					{
						F_LOG("SKill : no target after moved,return.");
						return ret;
					}
					isSkillGiveUp = true;
					F_LOG("SKill : can not move and no target for skill,GiveUp!");
				}
			}
		}
	}


	// 普通攻击
	{
		// 2014-03-26
		// 对于当前的战斗模式：不移动，到处都能打的要求，这里attackedItems肯定不为空的
		// 所有只会进入分支1
		vector<ChessBoardItem*> attackedItems = getBeFightedItem(fightor);
		if (!attackedItems.empty())
		{// 有目标，直接攻击

			fightor->hero->fightTurnInc(); // 更新攻击轮次

			vector<OneStep> ret;
			OneStep step_fight;

			// 通知棋盘，这是一个可看到的步奏
			mHelper->one_visible_step(step_fight.cdTime_arr,fightor->getUniqueId());
			mBuffProc->one_visible_step(step_fight.buff);

			// 后攻击，不然先攻击打出的就在one_visible_step中移除了，不过普通攻击不打buff，无所谓
			fight_normal(step_fight,fightor,attackedItems);

			if (useBlood)	ret.push_back(stepUseBlood);
			ret.push_back(step_fight);

			F_LOG("fight step buffData = **[%s]**",step_fight.getBuffString().c_str());
			return ret;
		}
		else
		{// 没有目标，先移动一步试试
			ChessBoard* move_to_board;
			int posX;
			int posY;
			// 能移动
			if (chessBoardItemTryGoStep(fightor,move_to_board,posX,posY))
			{
				F_LOG("Fight : Move %s[%d,%d] ---> %s[%d,%d]",
					fightor->currentBoard->getName().c_str(),
					fightor->posx,fightor->posy,
					move_to_board->getName().c_str(),
					posX,posY);


				vector<OneStep> ret;
				OneStep step_move;
				step_move.stepType = StepType_Move;
				setItemMoveTo(step_move.moveData,fightor,move_to_board,posX,posY);
				

				// 通知棋盘，这是一个可看到的步奏，这个无所谓先后
				mHelper->one_visible_step(step_move.cdTime_arr,fightor->getUniqueId());
				mBuffProc->one_visible_step(step_move.buff);

				if (useBlood)	ret.push_back(stepUseBlood);
				ret.push_back(step_move);

				F_LOG("fight move step buffData = **[%s]**",step_move.getBuffString().c_str());
				// 已经死了，就返回
				if (fightor->hero->getHp() <= 0)
				{
					F_LOG("Fight : target dead after moved.....");
					return ret;
				}

				attackedItems = getBeFightedItem(fightor);
				if (!attackedItems.empty())
				{
					fightor->hero->fightTurnInc(); // 更新攻击轮次

					OneStep step_fight;

					mHelper->one_visible_step(step_fight.cdTime_arr,fightor->getUniqueId());
					mBuffProc->one_visible_step(step_fight.buff);

					// 后攻击，不然先攻击打出的就在one_visible_step中移除了，不过这个没有buff，无所谓
					fight_normal(step_fight,fightor,attackedItems);

					ret.push_back(step_fight);
					F_LOG("fight fight(after move) step buffData = **[%s]**",step_fight.getBuffString().c_str());
					return ret;
				}
				else
				{
					F_LOG("fight : no target after moved ,return.");
					return ret;
				}
			}
			else
			{// 不能移动,也不能攻击
				vector<OneStep> ret;
				OneStep step_wait;
				step_wait.stepType = StepType_Wait;	// 不算Buff的步数

				if (useBlood)	ret.push_back(stepUseBlood);
				ret.push_back(step_wait);
				F_LOG("fight : wait...");
				return ret;
			}
		}
	}

	vector<OneStep> null_step;
	CCAssert(false,"");
	return null_step;
}
//
void FightControler::fileWithNoMove(StepMove& out_move,const ChessBoardItem* item)
{
	out_move.inSelfChessBoard = true;
	out_move.destIndex = mHelper->getIndex(item->posx,item->posy);
	//out_move.heroId = item->hero.getHeroId();
	out_move.uniqueId = item->getUniqueId();
}
//
void FightControler::setItemMoveTo(StepMove& out_move,ChessBoardItem* item,ChessBoard* desBoard,int posX,int posY)
{
	out_move.inSelfChessBoard = (desBoard == item->getOriginBoard());
	out_move.destIndex = mHelper->getIndex(posX,posY);
	out_move.uniqueId = item->getUniqueId();

	// 会移动到别人的棋盘,并且之前没有在别人棋盘，那么就需要换棋盘
	if (!out_move.inSelfChessBoard && item->currentBoard == item->getOriginBoard())
	{
		item->currentBoard->removeItem(item);
		desBoard->removeItem(posX,posY);
		desBoard->addItem(item,posX,posY);
	}
	// 不用换棋盘
	else
	{
		item->currentBoard->removeItem(posX,posY);
		item->posx = posX;
		item->posy = posY;
	}
}
//
vector<OneStep> FightControler::oneChessBoardFight()
{
	int index;
	ChessBoardItem* fightor = prepareFightItemFromIndex(mCurrentFightIndex,index);

	bool newRoundFlag = false;
	unsigned int roundIndex = 0;
	// 更新一轮的攻击历史，更新轮数
	{
		if ( fightor->getGeneBoard() == mHelper->getBoardA() )
		{
			int aHistoryNum = A_LastTurnFightHistory.size();
			bool isNewRound = false;

			for (int i=0;i<aHistoryNum;i++)
			{
				if (A_LastTurnFightHistory[i] == fightor)
				{// 证明fightor这一轮已经打过了，那么代表是新的一轮了
					A_LastTurnFightHistory.clear();
					A_LastTurnFightHistory.push_back(fightor);
					isNewRound = true;

					turnIndexA ++;
					break;
				}
			}

			if (!isNewRound)
			{
				A_LastTurnFightHistory.push_back(fightor);
			}

			// 如果fightor所生棋盘是新的一回合了
			if (isNewRound && turnIndexA > turnIndexB)
			{
				newRoundFlag = true;
				roundIndex = turnIndexA;
			}
		}
		else if ( fightor->getGeneBoard() == mHelper->getBoardB() )
		{
			int bHistoryNum = B_LastTurnFightHistory.size();
			bool isNewRound = false;

			for (int i=0;i<bHistoryNum;i++)
			{
				if (B_LastTurnFightHistory[i] == fightor)
				{// 证明fightor这一轮已经打过了，那么代表是新的一轮了
					B_LastTurnFightHistory.clear();
					B_LastTurnFightHistory.push_back(fightor);
					isNewRound = true;

					turnIndexB ++;
					break;
				}
			}

			if (!isNewRound)
			{
				B_LastTurnFightHistory.push_back(fightor);
			}

			// 注意：
			// 应该是一旦有一方发现是新的一轮开始，那么就是新的一轮了
			// 不应该找先手的判断。因为可能先手的[0,0]位置挂了
			//
			if (isNewRound && turnIndexB > turnIndexA)
			{
				newRoundFlag = true;
				roundIndex = turnIndexB;
			}
		}
		else
		{
			CCAssert(false,"BUG");
		}
		
	}


	OneStep roundChangeStep; //newRoundFlag为true，就生效
	// 回合切换
	{
		if (newRoundFlag)
		{
			roundChangeStep.stepType = StepType_ChangeRound;
			mBuffProc->fightRoundChange(roundChangeStep.buff);
			roundChangeStep.changeRound.round = roundIndex;

			F_LOG("%s new round[roundIndex = %d] begin!",__FUNCTION__,roundIndex); 
			F_LOG("stepRound buffData = **[%s]**",roundChangeStep.getBuffString().c_str());

			// 修改2014-03-08 在轮次切换的时候修改cd
			// 更新cdTime
			vector<ChessBoardItem*> allItems = mHelper->getAlivedItemsByOriginBoard(NULL);
			int itemNum = allItems.size();
			for (int i=0;i<itemNum;i++)
			{
				int num = allItems[i]->hero->getCdTime() - 1;
				allItems[i]->hero->setCdTime(num>0?num:0);
				CdDiscr oneDisc;
				oneDisc.cdTime = allItems[i]->hero->getCdTime();
				oneDisc.uniqueId = allItems[i]->getUniqueId();
				roundChangeStep.cdTime_arr.push_back(oneDisc);
				F_LOG("Update cdTime --> {uniqueId = %u,cdTime=%u}",oneDisc.cdTime,oneDisc.uniqueId);
			}
		}
	}


	// 处理眩晕的buff,就是一般的等待，只是身上挂了一个Buff图标，所以不会触发buff_step
	{
		OneStep step;
		step.stepType = StepType_Wait;
		bool canFight = mBuffProc->beforeFight(fightor->getUniqueId(),step.buff);
		if (!canFight)
		{
			F_LOG("FightControler::oneChessBoardFight fightor is [xuanyun],waiting.");
			F_LOG("xuanyun step buffData = **[%s]**",step.getBuffString().c_str());

			// bug:修复
			// 2014-04-01
			// 这里添加了这句，不然眩晕时mCurrentFightIndex没有更新
			// 那么每次都是他攻击，直到眩晕完毕。
			// 应该是眩晕的话，就放弃
			mCurrentFightIndex = (index+1)%(mFightQueue.size());

			vector<OneStep> ret;
			if (newRoundFlag)
			{
				ret.push_back(roundChangeStep);
			}
			ret.push_back(step);
			return ret;
		}
	}

	// 异常
	if(fightor == NULL)
	{
		F_LOG("Error : FightControler::oneChessBoardFight --> prepareChessBoardFightItem is NULL");
		CCAssert(false,"");
		vector<OneStep> ret;
		if (newRoundFlag)
		{
			ret.push_back(roundChangeStep);
		}
		return ret;
	}

	string name = fightor->getOriginBoard()->getName().c_str();
	F_LOG("------ %s item at [%d,%d]",name.c_str(),fightor->posx,fightor->posy);
	vector<OneStep> fight_step = fight(fightor);
	mCurrentFightIndex = (index+1)%(mFightQueue.size());
	F_LOG("------------------------------------%s attack  end------------------------------------\n\n",name.c_str());



	// 统计砍血的值
	// 这里就按照解决隐晦bug的方式来统计，就是根据攻击者的对象来统计血量
	{
		int stepNum = fight_step.size();
		for (int i=0;i<stepNum;i++)
		{
			if (fight_step[i].stepType == StepType_Fight)
			{
				int targetNum = fight_step[i].fightData.targetUniqueVec.size();
				for (int tarIndex = 0;tarIndex < targetNum ; tarIndex ++)
				{
					if (fight_step[i].fightData.canMissVec[tarIndex])
						continue;

					// 根据攻击的对象，来统计血量
					if (isInitedInBoardA(fight_step[i].fightData.targetUniqueVec[tarIndex]))
					{
						B_fightedHp += -(fight_step[i].fightData.targetBloodChange[tarIndex]);
					}
					else if (isInitedInBoardB(fight_step[i].fightData.targetUniqueVec[tarIndex]))
					{
						A_fightedHp += -(fight_step[i].fightData.targetBloodChange[tarIndex]);
					}
					else
					{
						CCAssert(false,"未知来源");
					}
				}
			}
			else if (
				fight_step[i].stepType == StepType_FailByTakeTooMuchTime ||
				fight_step[i].stepType == StepType_ReLoadBlood ||
				fight_step[i].stepType == StepType_UseBlood ||
				fight_step[i].stepType == StepType_Move ||
				fight_step[i].stepType == StepType_ChangeRound ||
				fight_step[i].stepType == StepType_Wait
				)
			{
				continue;
			}
			else if (fight_step[i].stepType == StepType_Skill)
			{
				int targetNum = fight_step[i].skillData.targetUniqueVec.size();
				for (int tarIndex = 0;tarIndex < targetNum ; tarIndex ++)
				{
					if (fight_step[i].skillData.canMissVec[tarIndex])
						continue;

					// 根据攻击的对象，来统计血量
					if (isInitedInBoardA(fight_step[i].skillData.targetUniqueVec[tarIndex]))
					{
						B_fightedHp += -(fight_step[i].skillData.targetBloodChange[tarIndex]);
					}
					else if (isInitedInBoardB(fight_step[i].skillData.targetUniqueVec[tarIndex]))
					{
						A_fightedHp += -(fight_step[i].skillData.targetBloodChange[tarIndex]);
					}
					else
					{
						CCAssert(false,"未知来源");
					}
				}

				// buff的不统计

			}else
			{
				CCAssert(false,"未知的step");
			}
		}
	}
	vector<OneStep> ret;
	if (newRoundFlag) // 加入round
	{
		ret.push_back(roundChangeStep);
	}
	ret.insert(ret.end(),fight_step.begin(),fight_step.end());

	return ret;
}

bool FightControler::isInitedInBoardA(unsigned int u_id)
{
	int a_num = boardAInitUiqueId.size();
	for (int i=0;i<a_num;i++)
	{
		if (boardAInitUiqueId[i] == u_id)
		{
			return true;
		}
	}

	return false;
}

bool FightControler::isInitedInBoardB(unsigned int u_id)
{
	int b_num = boardBInitUiqueId.size();
	for (int i=0;i<b_num;i++)
	{
		if (boardBInitUiqueId[i] == u_id)
		{
			return true;
		}
	}

	return false;
}

//
ChessBoardItem* FightControler::prepareFightItemFromIndex(int fightIndex,int& outIndex)
{
	ChessBoardItem* fightor = NULL;
	int num = mFightQueue.size();
	if (fightIndex >= num)
	{
		CCAssert(false,"不可能");
		fightIndex = num;
	}
	// 往后找
	for (int i=fightIndex;i<num;i++)
	{
		fightor = getOneItemFrom2Board(mFightQueue[i]);
		if (fightor != NULL)
		{
			outIndex = i;
			return fightor;
		}
	}
	// 重头找
	for (int i=0;i<fightIndex;i++)
	{
		fightor = getOneItemFrom2Board(mFightQueue[i]);
		if (fightor != NULL)
		{
			outIndex = i;
			return fightor;
		}
	}
	return fightor;
}
//
ChessBoardItem* FightControler::prepareChessBoardFightItem(ChessBoardItem* lastItem,ChessBoard* chessBoard)
{
	return mHelper->getTeamNextItem(chessBoard,lastItem);
}

vector<ChessBoardItem*> FightControler::getBeFightedItem(ChessBoardItem* fightorItem,float dis /*= -1*/)
{
#define WORLD_BOSS_PROFESSION	95 // 世界boss的职业

	vector<ChessBoardItem*> attackedItems;
	{// 寻找目标
		if (fightorItem->hero->getProfession() == Profession_LieShou ||
			fightorItem->hero->getProfession() == WORLD_BOSS_PROFESSION)
		{
			// 排攻击，先看第0排，再看后面的排
			ChessBoard* targetBoard = mHelper->getBoardA() == fightorItem->getOriginBoard() ? mHelper->getBoardB() : mHelper->getBoardA();
			vector<ChessBoardItem*> allItems = mHelper->getAlivedXItems(targetBoard,0);
			if (allItems.empty())
				allItems = mHelper->getAlivedXItems(targetBoard,1);
			int num = allItems.size();
			for (int itemIndex=0;itemIndex<num;itemIndex++)
			{
				CCAssert(allItems[itemIndex]->hero->getHp() > 0,"异常");
				attackedItems.push_back(allItems[itemIndex]);
			}
		}
		else if(fightorItem->hero->getProfession() == Profession_ShenJianShou)
		{
			if (dis > 0)
			{
				CCLog("=============>Waring : %s --> Profession_ShenJianShou , use the specified dis > 0,we will ingore this");
			}
			// 看周围所有的
			ChessBoard* targetBoard = mHelper->getBoardA() == fightorItem->getOriginBoard() ? mHelper->getBoardB() : mHelper->getBoardA();
			vector<ChessBoardItem*> allItems = mHelper->getAlivedItemsByOriginBoard(targetBoard);;
			ChessBoardItem* weakItem = getWeaklessAndDyingItem(allItems,fightorItem);
			CCAssert(weakItem!=NULL,"");
			attackedItems.push_back(weakItem);
		}
		else
		{
			// 单体攻击
			ChessBoardItem* attackedItemTemp = prepareChessBoardBeFightedItem(fightorItem);
			if (attackedItemTemp != NULL && attackedItemTemp->hero->getHp() > 0)
			{
				attackedItems.push_back(attackedItemTemp);
			}
		}
	}

	return attackedItems;
}

//
ChessBoardItem* FightControler::prepareChessBoardBeFightedItem(ChessBoardItem* fightorItem,float dis /* = -1 */)
{
	// 修改：2014-03-03
	// 不在用距离做限制了
	//
	if (fightorItem->hero->getProfession() == Profession_ShenJianShou)
	{
		CCLog("=============>Waring : %s ---> ShenJianShou should not call this funciton(Are you gived him a wrong skill type?)!",__FUNCTION__);
	}
	return getAroundICanFightItemInNearestByY(fightorItem);








	// 智能的,打全屏的
	if (fightorItem->hero->getProfession() == Profession_ShenJianShou)
	{
		CCLog("=============>Waring : %s ---> Please make sure that ShenJianShou will use distance limit when fight(Are you gived him a wrong skill type >)!",__FUNCTION__);
		// 看周围所有的
		ChessBoard* opBoard = mHelper->getBoardA() == fightorItem->getOriginBoard() ? mHelper->getBoardB() : mHelper->getBoardA();
		vector<ChessBoardItem*> allItems = mHelper->getAlivedItemsByOriginBoard(opBoard);
		vector<ChessBoardItem*> targets;
		// 有距离筛选,就筛一次距离
		if (dis >= 0)
		{
			int num = allItems.size();
			for (int i=0;i<num;i++)
			{
				if (mHelper->distance(fightorItem,allItems[i]) <= dis)
				{
					targets.push_back(allItems[i]);
				}
			}
		}else
		{
			targets = allItems;
		}
		
		return getWeaklessAndDyingItem(targets,fightorItem);
	}
	return getAroundICanFightItemInGongJiJuLi(fightorItem,dis);
}

ChessBoardItem* FightControler::getWeaklessAndDyingItem(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor)
{
	int num = itemSet.size();
	if (num == 0)
		return NULL;

	// 寻找最弱的，也就是攻击方攻击别人减血最多的
	ChessBoardItem* weakless = itemSet[0];
	unsigned int maxShangHaiValue = fightor->hero->getShangHaiValue(itemSet[0]->hero,Range_Min);

	ChessBoardItem* dying = itemSet[0];
	unsigned int minHpLeft = weakless->hero->getHp() > maxShangHaiValue?weakless->hero->getHp() - maxShangHaiValue:0;

	// 快速返回
	if (minHpLeft == 0)
	{
		return dying;
	}

	for (int i=1;i<num;i++)
	{
		// 找防御最差的
		unsigned int tempValue = fightor->hero->getShangHaiValue(itemSet[i]->hero,Range_Min);
		if ( tempValue > maxShangHaiValue )
		{
			maxShangHaiValue = tempValue;
			weakless = itemSet[i];
		}

		// 找攻击完毕之后血量剩余最少的
		unsigned int tempLeftHp = itemSet[i]->hero->getHp() > tempValue?itemSet[i]->hero->getHp() - tempValue:0;
		if (tempLeftHp < minHpLeft)
		{
			minHpLeft = tempLeftHp;
			dying = itemSet[i];
		}
	}

	// 有直接打死的人，那么就选直接打死的,否则就返回杀伤力最大的
	if (minHpLeft == 0)
		return dying;

	return weakless;
}

// 获得在Y方向上，我攻击范围里面，离我最近的那个敌人Item
ChessBoardItem* FightControler::getYICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis /* = -1 */)
{
	vector<ChessBoardItem*> allItems = mHelper->getYItems(fightor->posx);
	return getICanFightItemInGongJiJuLi(allItems,fightor,dis);
}
// 获得在X方向上，我攻击范围里面，离我最近的那个敌人Item
ChessBoardItem* FightControler::getXICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis /* = -1 */)
{
	vector<ChessBoardItem*> allItems = mHelper->getXItems(fightor->currentBoard,fightor->posy);
	return getICanFightItemInGongJiJuLi(allItems,fightor,dis);
}
//
ChessBoardItem* FightControler::getAroundICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis /* = -1 */)
{
	// 先看Y方向上
	ChessBoardItem* yItem = getYICanFightItemInGongJiJuLi(fightor,dis);
	if (yItem)
		return yItem;

	// 再看X方向上
	ChessBoardItem* xItem = getXICanFightItemInGongJiJuLi(fightor,dis);
	if (xItem)
		return xItem;

	// 再看周围所有的
	ChessBoard* opBoard = mHelper->getBoardA() == fightor->getOriginBoard() ? mHelper->getBoardB() : mHelper->getBoardA();
	vector<ChessBoardItem*> allItems = mHelper->getAlivedItemsByOriginBoard(opBoard);
	return getICanFightItemInGongJiJuLi(allItems,fightor,dis);
}

ChessBoardItem* FightControler::getAroundICanFightItemInNearestByY(ChessBoardItem* fightor)
{
	CCAssert(fightor->getGeneBoard() == fightor->currentBoard,"不支持移动棋盘");
	ChessBoardItem* ret = NULL;

	ChessBoard* opBoard = (fightor->getOriginBoard() == mHelper->getBoardA()?mHelper->getBoardB():mHelper->getBoardA());
	
	vector<ChessBoardItem*> itemsInMyRow = mHelper->getXItems(fightor->currentBoard,fightor->posy);

	F_LOG("%s--> find from my side,this must not found!",__FUNCTION__);
	float MAX_DIS = GRID_NUM_X*2+GRID_NUM_Y*2 + 2;
	ret = getNearestItemFromSet(itemsInMyRow,fightor,MAX_DIS,ItemFilter_enemy);
	if (ret != NULL)
	{
		CCLog("Error : %s ---> there are enemys at my side!",__FUNCTION__);
		CCAssert(false,"正常情况不可能存在");
		return ret;
	}

	for (int i=0;i<GRID_NUM_Y;i++)
	{
		F_LOG("%s--> find op board row[%d]",__FUNCTION__,i);
		vector<ChessBoardItem*> itemsInOpBoardRow = mHelper->getXItems(opBoard,i);
		ret = getNearestItemFromSet(itemsInOpBoardRow,fightor,MAX_DIS,ItemFilter_enemy);
		if (ret != NULL)
			break;
	}
	
	return ret;

}


// 得到在itemSet中，在我的攻击范围里面，我可以攻击的角色(一定是活着的);
ChessBoardItem* FightControler::getICanFightItemInGongJiJuLi(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor,float dis /* = -1 */)
{
	return getNearestItemFromSet(itemSet,fightor,dis>=0?dis:fightor->hero->getGongJiJuLi(),ItemFilter_enemy);
}

ChessBoardItem* FightControler::getNearestItemFromSet(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor,float juLi,ItemFilter filter /* = ItemFilter_enemy */)
{
	// 没有人就直接返回
	if(itemSet.size() <= 0)
	{
		F_LOG("FightControler::getItemFromSet ---> No one in this set");
		// 不能攻击
		return NULL;
	}

	// 里面就是自己，那么直接返回
	if (itemSet.size() == 1 && itemSet[0] == fightor)
	{
		F_LOG("FightControler::getItemFromSet ---> Only myself in this set,return!");
		return NULL;
	}

	float minDistance = juLi;
	ChessBoardItem* retItem = NULL;

	int num = itemSet.size();
	for (int i=0;i<num;i++)
	{
		// 要找敌人的情况，是自己人就忽略
		if (filter == ItemFilter_enemy && itemSet[i]->getOriginBoard() == fightor->getOriginBoard())
			continue;
		// 要找队友的情况，是敌人就忽略
		else if (filter == ItemFilter_teammates && itemSet[i]->getOriginBoard() != fightor->getOriginBoard())
			continue;
		else if(filter != ItemFilter_enemy && filter != ItemFilter_teammates)
			CCAssert(false,"不支持的Filter");

		float tempDistance = mHelper->distance(fightor,itemSet[i]);
        /* 当xSize >=2 时候 因为不计算x坐标 这样同一行的距离就相同 右边的就会覆盖左边的item */
		//if (tempDistance <= minDistance && itemSet[i]->hero->getHp() > 0) 
		// 目前距离最小，而且还是活着的item
		if (tempDistance < minDistance && itemSet[i]->hero->getHp() > 0) 
		{
			retItem = itemSet[i];

			minDistance = tempDistance;
		}
	}
	if (retItem == NULL)
	{
		F_LOG("FightControler::getItemFromSet ---> No Item found ,because [too far] or [all of them is not in required board]!");
	}
	return retItem;
}

bool FightControler::chessBoardItemTryGoStep(const ChessBoardItem* item,ChessBoard*& out_moveToBoard,int& out_posx,int& out_posy)
{
	//去掉移动功能了
	return false;


	// 得到旁边的Item,旁边的有敌人，就坚决不能向前面移动，如果是隐蔽的，细作，那么应该调用
	// 判断基因棋盘来实现，这里暂时用isTeammate来判断
	ChessBoardItem* sideItem = item->currentBoard->getActorItem(item->posx == 0?1:0,item->posy);
	if (sideItem != NULL && sideItem->hero->getHp() > 0 && !mHelper->isTeammate(item->getUniqueId(),sideItem->getUniqueId()))
	{
		return false;
	}

	mHelper->getNextYPosOfItem(item,out_moveToBoard,out_posy);
	out_posx = item->posx;

	// 宽Item，那么前面一排只要有一个就算
	if (item->hero->getXSize() > 1)
	{
		return out_moveToBoard->getItemNumInY(out_posy,true) <= 0;
	}
	else
	{
		ChessBoardItem* frontItem = out_moveToBoard->getActorItem(out_posx,out_posy);
		if (frontItem ==NULL || frontItem->hero->getHp() <=0 )
		{
			return true;
		}
		return false;
	}
}


FightControler::~FightControler(void)
{
	delete mSkillHelper;
	delete calculator;
}


//////////////////////////////////////////////////////////////////////////
// TeamFightControler
//////////////////////////////////////////////////////////////////////////
//


TeamFightControler::TeamFightControler(ChessBoard_TeamFight* board,GameBuffProc* proc)
{ 
	teamBoard = board;
	mBuffProc = proc;

	mSkillHelperAdatper = new SkillHelperTFAdapterImpl(board,this);
	mSkillHelper = new SkillHelperBase(mSkillHelperAdatper);
}
// 自动初始化先手
// 需要在棋盘都初始化完毕之后调用
void TeamFightControler::initFirstFightBoard()
{
	mFightQueue.clear();
	mCurrentFightIndex = 0;
	mLastBossFightedTeamUId = 0;

	mFightQueue.push_back(1);
	mFightQueue.push_back(2);
	mFightQueue.push_back(3);
	mFightQueue.push_back(4);
	mFightQueue.push_back(TEAM_BOSS_U_ID);	// boss
}

//
ChessBoardItem_TeamFight* TeamFightControler::prepareFightItemFromIndex(int fightIndex,int& outIndex)
{
	ChessBoardItem_TeamFight* fightor = NULL;
	int num = mFightQueue.size();
	if (fightIndex >= num)
	{
		CCAssert(false,"不可能");
		fightIndex = num;
	}
	// 往后找
	for (int i=fightIndex;i<num;i++)
	{
		fightor = teamBoard->getActorItemByUId(mFightQueue[i]);// getOneItemFrom2Board(mFightQueue[i]);
		if (fightor != NULL)
		{
			outIndex = i;
			return fightor;
		}
	}
	// 重头找
	for (int i=0;i<fightIndex;i++)
	{
		//fightor = getOneItemFrom2Board(mFightQueue[i]);
		fightor = teamBoard->getActorItemByUId(mFightQueue[i]);
		if (fightor != NULL)
		{
			outIndex = i;
			return fightor;
		}
	}
	return fightor;
}
//

// 可能会返回多步：把fight和skill的premove 提出来了
vector<OneStep> TeamFightControler::oneChessBoardFight()
{
	int index;
	ChessBoardItem_TeamFight* fightor = prepareFightItemFromIndex(mCurrentFightIndex,index);

	// 处理眩晕的buff,就是一般的等待，只是身上挂了一个Buff图标，所以不会触发buff_step
	{
		OneStep step;
		step.stepType = StepType_Wait;
		bool canFight = mBuffProc->beforeFight(fightor->getUniqueId(),step.buff);
		if (!canFight)
		{
			F_LOG("FightControler::oneChessBoardFight fightor is [xuanyun],waiting.");
			F_LOG("xuanyun step buffData = **[%s]**",step.getBuffString().c_str());

			// bug:修复
			// 2014-04-01
			// 这里添加了这句，不然眩晕时mCurrentFightIndex没有更新
			// 那么每次都是他攻击，直到眩晕完毕。
			// 应该是眩晕的话，就放弃
			mCurrentFightIndex = (index+1)%(mFightQueue.size());

			vector<OneStep> ret;
			ret.push_back(step);
			return ret;
		}
	}

	if(fightor == NULL)
	{
		CCAssert(false,"");
		F_LOG("Error : FightControler::oneChessBoardFight --> prepareChessBoardFightItem is NULL");
		vector<OneStep> ret;
		return ret;
	}

	string name = fightor->getUniqueId()==TEAM_BOSS_U_ID?"Boss":"Team";

	F_LOG("------ %s item at [index = %d]",name.c_str(),fightor->currentIndex);
	vector<OneStep> fight_step = fight(fightor);
	mCurrentFightIndex = (index+1)%(mFightQueue.size());
	F_LOG("------------------------------------%s attack  end------------------------------------\n\n",name.c_str());

	return fight_step;
}

ChessBoardItem_TeamFight* TeamFightControler::prepareBeFightedByBoss()
{
	int i = 1;
	while (i <= FormationHeroAcountMax)
	{
		int index = mLastBossFightedTeamUId -1;					// 对应的index
		int u_id_next = (index+1)%FormationHeroAcountMax + 1;	// 下一个就是index+1，然后把index转成u_id
		ChessBoardItem_TeamFight* item = teamBoard->getActorItemByUId(u_id_next);
		if (item && item->hero->getHp() > 0)
		{
			return item;
		}
		i++;
	}

	return NULL;
}

ChessBoardItem_TeamFight* TeamFightControler::getWeaklessAndDyingItem(vector<ChessBoardItem_TeamFight*> itemSet,ChessBoardItem_TeamFight* fightor)
{
	int num = itemSet.size();
	if (num == 0)
		return NULL;

	// 寻找最弱的，也就是攻击方攻击别人减血最多的
	ChessBoardItem_TeamFight* weakless = itemSet[0];
	unsigned int maxShangHaiValue = fightor->hero->getShangHaiValue(itemSet[0]->hero);

	ChessBoardItem_TeamFight* dying = itemSet[0];
	unsigned int minHpLeft = weakless->hero->getHp() > maxShangHaiValue?weakless->hero->getHp() > maxShangHaiValue:0;

	// 快速返回
	if (minHpLeft == 0)
	{
		return dying;
	}

	for (int i=1;i<num;i++)
	{
		// 找防御最差的
		unsigned int tempValue = fightor->hero->getShangHaiValue(itemSet[i]->hero);
		if ( tempValue > maxShangHaiValue )
		{
			maxShangHaiValue = tempValue;
			weakless = itemSet[i];
		}

		// 找攻击完毕之后血量剩余最少的
		unsigned int tempLeftHp = itemSet[i]->hero->getHp() > tempValue?itemSet[i]->hero->getHp() - tempValue:0;
		if (tempLeftHp < minHpLeft)
		{
			minHpLeft = tempLeftHp;
			dying = itemSet[i];
		}
	}

	// 有直接打死的人，那么就选直接打死的,否则就返回杀伤力最大的
	if (minHpLeft == 0)
		return dying;

	return weakless;
}

// 战斗，减血
void TeamFightControler::fight_normal(OneStep& outStep,ChessBoardItem_TeamFight* fightor,vector<ChessBoardItem_TeamFight*> attackedItems)
{
	CCAssert(attackedItems.size() > 0,"要有攻击对象");

	float baoJilv = ((float)fightor->hero->getBaoJiLv()) / 100.0f;
	outStep.stepType = StepType_Fight;
	outStep.fightData.fightorUniqueId = fightor->getUniqueId();
	outStep.fightData.fightPattern = lfRandom0_1() <= baoJilv?Fight_Normal_BaoJi:Fight_Normal;

#if (USE_FIGHT_LOG == 1)
	if (attackedItems.size() > 1)
	{
		CCLog("-----------------------normal fight multiple targets-------------------");
	}
#endif

	int num = attackedItems.size();
	ChessBoardItem_TeamFight* attackedItem = NULL;
	for (int i=0;i<num;i++)
	{
		attackedItem = attackedItems[i];
		outStep.fightData.targetUniqueVec.push_back(attackedItem->getUniqueId());

		int bloodChange = 0;
		{
			if (outStep.fightData.fightPattern == Fight_Normal_BaoJi)
			{
				// 打出最大值
				float normalShangHaiValue = fightor->hero->getShangHaiValue(attackedItem->hero,Range_Max);
				float baoShangYinZi = ((float)fightor->hero->getBaoJiShangHaiYinZi())/100.0f;
				bloodChange = -normalShangHaiValue * baoShangYinZi;
			}else
			{
				float normalShangHaiValue = fightor->hero->getShangHaiValue(attackedItem->hero);
				bloodChange = -normalShangHaiValue;
			}
		}

		outStep.fightData.targetBloodChange.push_back(bloodChange);

		float shanbilv = ((float)attackedItem->hero->getShanBiLv())/100.0f;
		bool canMiss = (lfRandom0_1() <= shanbilv);
		outStep.fightData.canMissVec.push_back(canMiss);

		// 没有Miss 就要减血
		if (!canMiss)
		{
			// 实际也要减血
			attackedItem->hero->addHp(bloodChange);

			mBuffProc->beattacked(fightor->getUniqueId(),attackedItem->getUniqueId(),bloodChange,outStep.buff);
			//mBuffProc->fight_end_or_beattacked(fightor->getUniqueId(),attackedItem->getUniqueId(),outStep.remove_buff_array);

			F_LOG("fight : beattackedItem[%d] mHpChange = %d ,target hp = %d",attackedItem->currentIndex,bloodChange,attackedItem->hero->getHp());
		}
		else
		{
			F_LOG("fight  --> Miss : beattackedItem[%d] mHpChange = %d ,target hp = %d",attackedItem->currentIndex,bloodChange,attackedItem->hero->getHp());
		}
	}


	if (num > 0)
	{
		mBuffProc->fight_end(fightor->getUniqueId(),outStep.buff);
	}


#if (USE_FIGHT_LOG == 1)
	if (attackedItems.size() > 1)
	{
		CCLog("-----------------------normal fight multiple targets-------------------\n");
	}
#endif
}
// 战斗，减血，加buff
void TeamFightControler::fight_skill(OneStep& outStep,SkillDataForFight skill,unsigned int f_uniqueId,SkillFight skillFight)
{
	CCAssert(skillFight.targetVec.size() > 0,"要有攻击对象");

	outStep.stepType = StepType_Skill;
	outStep.skillData.skillType = skill.skillType;
	outStep.skillData.fightorUniqueId = f_uniqueId;
	outStep.skillData.canMissVec = skillFight.canMissVec;
	outStep.skillData.skillPatten = skillFight.skillPatten;
	outStep.skillData.targetBloodChange = skillFight.targetBloodChange;
	outStep.skillData.targetUniqueVec = skillFight.targetVec;
	//转换成client的
	int buff_size = skillFight.firedBuff.size();
	for (int buffIndex =0;buffIndex < buff_size;buffIndex++)
	{
		ClientGameBuff buff(skillFight.firedBuff[buffIndex]);
		outStep.skillData.firedBuff.push_back(buff);
	}

	int targetNum = outStep.skillData.targetUniqueVec.size();
	// 先调用fightEnd
	if (targetNum > 0)
	{
		mBuffProc->fight_end(f_uniqueId,outStep.buff);
	}
	for (int i = 0;i<targetNum;i++)
	{
		ChessBoardItem_TeamFight* oneTarget = teamBoard->getActorItemByUId(outStep.skillData.targetUniqueVec[i]);
		CCAssert(oneTarget!=NULL,"必须存在");
		CCAssert(oneTarget->hero->getHp() > 0,"必须活的");

		// 没有Miss 就要减血,附加buff
		if (!outStep.skillData.canMissVec[i])
		{
			// 实际也要减血
			oneTarget->hero->addHp(outStep.skillData.targetBloodChange[i]);

			mBuffProc->beattacked(f_uniqueId,oneTarget->getUniqueId(),outStep.skillData.targetBloodChange[i],outStep.buff);

			// 还有血,则加buff
			if (oneTarget->hero->getHp() > 0 && skillFight.firedBuff.size() > 0)
			{
				// buff 附加
				int bufNum = skillFight.firedBuff.size();
				for (int buffIndex = 0;buffIndex < bufNum;buffIndex++)
				{
					if (skillFight.firedBuff[buffIndex].attachedToUId == oneTarget->getUniqueId())
					{
						// 附加buff
						// 是否有这个buff，有就更新
						if (!skillFight.firedBuff[buffIndex].cumulative && oneTarget->hero->haveBuffWithStateIs(skillFight.firedBuff[buffIndex].state))
						{
							bool suc = oneTarget->hero->updateBuff(skillFight.firedBuff[buffIndex]);
							CCAssert(suc,"这必须成功，不然就是bug");
						}else
						{
							bool suc = oneTarget->hero->attachBuff(skillFight.firedBuff[buffIndex]);
							CCAssert(suc,"这必须成功，不然就是bug");
						}

						F_LOG("Skill [%s] -->Buff: beattackedItem[%d],\n####{%s}####",
							skillFight.nameForLog.c_str(),
							oneTarget->currentIndex,
							outStep.skillData.firedBuff[i].getLog().c_str()
							);
					}
				}
			}// oneTarget->hero->getHp() > 0

			F_LOG("Skill [%s] : beattackedItem[%d] mHpChange = %d ,target hp = %d \n",
				skillFight.nameForLog.c_str(),
				oneTarget->currentIndex,
				outStep.skillData.targetBloodChange[i],
				oneTarget->hero->getHp());
		}// !outStep.skillData.canMissVec[i]
		else
		{
			F_LOG("Skill [%s] --> Miss : beattackedItem[%d] mHpChange = %d ,target hp = %d \n",
				skillFight.nameForLog.c_str(),
				oneTarget->currentIndex,
				outStep.skillData.targetBloodChange[i],
				oneTarget->hero->getHp());
		}
	}// int i = 0;i<targetNum;i++


	/*if (targetNum > 0)
	{
		mBuffProc->fight_end(f_uniqueId,outStep.buff);
	}*/
}
// 
vector<OneStep> TeamFightControler::fight(ChessBoardItem_TeamFight* fightor)
{
	CCAssert(fightor!=NULL,"");

	bool useBlood = false;
	OneStep stepUseBlood;

	// 复活
	if ( fightor->getUniqueId()!=TEAM_BOSS_U_ID && (float)fightor->hero->getHp() <=0)
	{
		if (fightor->hero->getCdTime() > 0 ) // cd时间还没到，不能操作，只能wait
		{
			vector<OneStep> ret;
			OneStep step_wait;
			step_wait.stepType = StepType_Wait;	// 不算Buff的步数
			// cd时间,团对这种是在需要动作的时候，但是又处在cd时间的时候才更新cd时间
			int num = fightor->hero->getCdTime() - 1;
			fightor->hero->setCdTime(num>0?num:0);
			CdDiscr oneDisc;
			oneDisc.cdTime = fightor->hero->getCdTime();
			oneDisc.uniqueId = fightor->getUniqueId();
			step_wait.cdTime_arr.push_back(oneDisc);
			//
			ret.push_back(step_wait);
			F_LOG("fight in cd time: wait...");
			return ret;
		}

		useBlood = true;
		stepUseBlood.stepType = StepType_ReLoadBlood;
		stepUseBlood.useBlood.uniqueId = fightor->getUniqueId();

		// 真实的加血
		stepUseBlood.useBlood.bloodChangeForUser = fightor->hero->getDefaultHp();	// 简单的，直接加默认值就是了
		fightor->hero->addHp(stepUseBlood.useBlood.bloodChangeForUser);
		// 死了之后再freze
		//fightor->setCurrentFreezyNum(fightor->hero->getTeamProperty().mDefaultFreezyNum);

		F_LOG("ReLoadBlood -> bloodAdd = %d, fightor[u_id = %u,index = %d]",
			stepUseBlood.useBlood.bloodChangeForUser,
			fightor->getUniqueId(),fightor->currentIndex);
	}



	bool try_use_skill = true;
	do // 技能的触发决策
	{
		// 不能用，就不需要用了
		if (!fightor->hero->canUseThisInCurrentFightTurn())
		{
			try_use_skill = false;
			break;
		}
		// 是神之信念(加防)，就需要策略
		if (fightor->hero->skilVec[0].skillType == SkillType_ShenZhiXingNian)
		{
			//看是否可以直接攻击
			//ChessBoardItem* attackedItem= prepareChessBoardBeFightedItem(fightor);
			ChessBoardItem_TeamFight* attackedItem = fightor->getUniqueId() == TEAM_BOSS_U_ID?
				prepareBeFightedByBoss():
				teamBoard->getActorItemByUId(TEAM_BOSS_U_ID);

			if (attackedItem != NULL && attackedItem->hero->getHp() > 0)
			{
				// 没有暴击的考虑
				int hp_sub = fightor->hero->getShangHaiValue(attackedItem->hero);
				if (hp_sub >= attackedItem->hero->getHp())	// 弄得死对方，就不用加防
				{
					try_use_skill = false;
					break;
				}
			}
		}

	} while (false);



	bool isSkillGiveUp = false; // 技能放弃攻击，转交给普攻
	// 先使用技能
	{
		//float chufaLv = ((float)fightor->hero->getJiNengChuFaLv())/100.0f;
		bool useSkill = fightor->hero->canUseThisInCurrentFightTurn();
		// 技能
		if (try_use_skill && useSkill)
		{
			F_LOG("Use Skill .......");
			SkillDataForFight skill = fightor->hero->skilVec[0];
			SkillFight skillFight = mSkillHelper->fight(fightor,skill);

			if (skillFight.targetVec.size() > 0)
			{// 有目标，直接攻击

				fightor->hero->fightTurnInc(); // 更新攻击轮次

				OneStep step;
				fight_skill(step,skill,fightor->getUniqueId(),skillFight);

				// 通知棋盘，这是一个可看到的步奏
				teamBoard->one_visible_step(step.cdTime_arr,fightor->getUniqueId());
				// 一步完毕就检测
				mBuffProc->one_visible_step(step.buff);

				vector<OneStep> ret;
				if (useBlood)	ret.push_back(stepUseBlood);
				ret.push_back(step);
				F_LOG("skill fight step buffData = **[%s]**",step.getBuffString().c_str());

				return ret;
			}
			else
			{// 没有目标，就wait
				F_LOG("Skill : No target, we will use normal fight try again!");
				isSkillGiveUp = true;

				// 这里不返回，还应该用普攻试试
				//OneStep step_wait;
				//step_wait.stepType = StepType_Wait;// 不算buff的步数
				//F_LOG("Skill : wait...");
				//vector<OneStep> ret;
				//if (useBlood)	ret.push_back(stepUseBlood);
				//ret.push_back(step_wait);
				//return ret;
			}
		}
	}


	// 普通攻击
	{
		// 准备被攻击的对象
		vector<ChessBoardItem_TeamFight*> attackedItems;
		{
			if (fightor->getUniqueId() == TEAM_BOSS_U_ID)//是boss
			{
				ChessBoardItem_TeamFight* attackedItem = prepareBeFightedByBoss();
				if (attackedItem && attackedItem->hero->getHp() > 0)
				{
					mLastBossFightedTeamUId = attackedItem->getUniqueId();
					attackedItems.push_back(attackedItem);
				}
			}
			else// 是团队
			{
				ChessBoardItem_TeamFight* attackedItem = teamBoard->getActorItemByUId(TEAM_BOSS_U_ID);
				CCAssert(attackedItem != NULL && attackedItem->hero->getHp() > 0,"");
				attackedItems.push_back(attackedItem);
			}
		}
		

		if (!attackedItems.empty())
		{// 有目标，直接攻击

			fightor->hero->fightTurnInc(); // 更新攻击轮次

			vector<OneStep> ret;
			OneStep step_fight;
			fight_normal(step_fight,fightor,attackedItems);
			// 死亡了，就进入cd时间
			int targetNum = attackedItems.size();
			for (int tarIndex=0;tarIndex<targetNum;tarIndex++)
			{
				if (attackedItems[tarIndex]->hero->getHp() <=0 )
				{
					attackedItems[tarIndex]->hero->setCdTime(attackedItems[tarIndex]->hero->getDefaultCdTime());
				}
			}

			// 通知棋盘，这是一个可看到的步奏
			teamBoard->one_visible_step(step_fight.cdTime_arr,fightor->getUniqueId());
			mBuffProc->one_visible_step(step_fight.buff);

			if (useBlood)	ret.push_back(stepUseBlood);
			ret.push_back(step_fight);

			F_LOG("fight step buffData = **[%s]**",step_fight.getBuffString().c_str());
			return ret;
		}
		else
		{// 没有目标，直接wait

			vector<OneStep> ret;
			OneStep step_wait;
			step_wait.stepType = StepType_Wait;	// 不算Buff的步数

			if (useBlood)	ret.push_back(stepUseBlood);
			ret.push_back(step_wait);
			F_LOG("fight : wait...");
			return ret;
		}
	}

	vector<OneStep> null_step;
	CCAssert(false,"");
	return null_step;
}


TeamFightControler::~TeamFightControler(void)
{
	delete mSkillHelperAdatper;
	delete mSkillHelper;
}