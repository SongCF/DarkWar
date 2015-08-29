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
	CCAssert(itemA == NULL || itemB == NULL,"������ͬʱ�ҵ�");
	return itemA == NULL?itemB : itemA;
}

void FightControler::initFirstFightBoard(FirstBloodBoard firstBoard)
{
	firstBloodBoard = NULL;
	// ������������
	ChessBoard* board[2] = {NULL};//board[0]�����֣�board[1]�Ǻ���
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

	// ��ʼ���������̳�ʼ�ĳ�ԱId������ͳ�ƿ�Ѫ��ʱ
	// ����u_id֪�����Ԫ��ʦ�ĸ�board��,��Ϊ���ǲ���ͨ��Id����ChessBoardItem���ٵõ�ԭʼ����
	// ԭ����ChessBoardItem���ܾ���NULL�ˣ���Ϊ�Ѿ�����N������ôBuff��ԭ��������Id��Ԫ��
	// ��N��֮��͹��ˣ����Ը���ChessBoardItem�ǲ��ܲ����ġ�
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
		CCAssert(false,"��֧�ֵ���������");
	}
	//// ȷ������
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
	
	// ��ʼ�� ����˳��
	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		{
			ChessBoardItem* item = board[0]->getActorItem(i%2,i/2,false);
			if (item != NULL)// �н�ɫ
			{
				mFightQueue.push_back(item->getUniqueId());
			}
		}
		{
			ChessBoardItem* item = board[1]->getActorItem(i%2,i/2,false);
			if (item != NULL)// �н�ɫ
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
	CCAssert(attackedItems.size() > 0,"Ҫ�й�������");

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
				// ������ֵ
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

		// û��Miss ��Ҫ��Ѫ
		if (!canMiss)
		{
			// ʵ��ҲҪ��Ѫ
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
	CCAssert(skillFight.targetVec.size() > 0,"Ҫ�й�������");
	outStep.stepType = StepType_Skill;
	outStep.skillData.skillType = skill.skillType;
	outStep.skillData.fightorUniqueId = f_uniqueId;
	outStep.skillData.canMissVec = skillFight.canMissVec;
	outStep.skillData.skillPatten = skillFight.skillPatten;
	outStep.skillData.targetBloodChange = skillFight.targetBloodChange;
	outStep.skillData.targetUniqueVec = skillFight.targetVec;
	//ת����client��
	int buff_size = skillFight.firedBuff.size();
	for (int buffIndex =0;buffIndex < buff_size;buffIndex++)
	{
		ClientGameBuff buff(skillFight.firedBuff[buffIndex]);
		outStep.skillData.firedBuff.push_back(buff);
	}

	int targetNum = outStep.skillData.targetUniqueVec.size();

	// �ȵ���fightEnd
	if (targetNum > 0)
	{
		mBuffProc->fight_end(f_uniqueId,outStep.buff);
	}

	for (int i = 0;i<targetNum;i++)
	{
		ChessBoardItem* oneTarget = mHelper->getOneItemByUniqueId(outStep.skillData.targetUniqueVec[i]);
		CCAssert(oneTarget!=NULL,"�������");
		CCAssert(oneTarget->hero->getHp() > 0,"������");

		// û��Miss ��Ҫ��Ѫ,����buff
		if (!outStep.skillData.canMissVec[i])
		{
			// ʵ��ҲҪ��Ѫ
			oneTarget->hero->addHp(outStep.skillData.targetBloodChange[i]);

			mBuffProc->beattacked(f_uniqueId,oneTarget->getUniqueId(),outStep.skillData.targetBloodChange[i],outStep.buff);
			// ����Ѫ,���buff
			if (oneTarget->hero->getHp() > 0 && skillFight.firedBuff.size() > 0)
			{

				// buff ����
				int bufNum = skillFight.firedBuff.size();
				for (int buffIndex = 0;buffIndex < bufNum;buffIndex++)
				{
					if (skillFight.firedBuff[buffIndex].attachedToUId == oneTarget->getUniqueId())
					{
						// ����buff
						// �Ƿ������buff���о͸���
						if (!skillFight.firedBuff[buffIndex].cumulative && oneTarget->hero->haveBuffWithStateIs(skillFight.firedBuff[buffIndex].state))
						{
							bool suc = oneTarget->hero->updateBuff(skillFight.firedBuff[buffIndex]);
							CCAssert(suc,"�����ɹ�����Ȼ����bug");
						}else
						{
							bool suc = oneTarget->hero->attachBuff(skillFight.firedBuff[buffIndex]);
							CCAssert(suc,"�����ɹ�����Ȼ����bug");
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
		fightor->hero->fightTurnInc(); // ���¹����ִ�

		// ֪ͨ���̣�����һ���ɿ����Ĳ���
		mHelper->one_visible_step(out_step.cdTime_arr,fightor->getUniqueId());
		mBuffProc->one_visible_step(out_step.buff);

		// �󹥻�����Ȼ�չ��������buff����one_visible_step���Ƴ���
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


	// ֪ͨ���̣�����һ���ɿ����Ĳ��࣬�������ƶ�������one_visible_step����ν
	mHelper->one_visible_step(out_step.cdTime_arr,fightor->getUniqueId());
	mBuffProc->one_visible_step(out_step.buff);

	F_LOG("move step buffData = **[%s]**",out_step.getBuffString().c_str());
	// �Ѿ����ˣ��ͷ���
	if (fightor->hero->getHp() <= 0)
	{
		F_LOG("fightor dead after move...");
	}

	return true;
}

bool FightControler::canUseSkillInThisTurnAndCurrentSituation(ChessBoardItem* fightor)
{
	bool try_use_skill = true;
	do // ���ܵĴ�������
	{
		// �����ü��ܣ���ôֱ�ӷ���
		if (!fightor->hero->canUseThisInCurrentFightTurn())
		{
			try_use_skill = false;
			break;
		}
		//// ����֮����(�ӷ�)������Ҫ����
		//if (fightor->hero->skilVec[0].skillType == SkillType_ShenZhiXingNian || fightor->hero->skilVec[0].skillType == SkillType_JianShou)
		//{
		//	//���Ƿ����ֱ�ӹ���
		//	ChessBoardItem* attackedItem= prepareChessBoardBeFightedItem(fightor);
		//	if (attackedItem != NULL && attackedItem->hero->getHp() > 0)
		//	{
		//		// û�б����Ŀ���
		//		int hp_sub = fightor->hero->getShangHaiValue(attackedItem->hero,Range_Min);
		//		if (hp_sub >= attackedItem->hero->getHp())	// Ū�����Է����Ͳ��üӷ�
		//		{
		//			try_use_skill = false;
		//			break;
		//		}
		//	}

		//	// ������Ҫ��ǰ��һ��������̽����
		//	if (attackedItem == NULL || attackedItem->hero->getHp() <= 0)
		//	{
		//		ChessBoard* oldBoard = fightor->currentBoard;
		//		int old_pos_x = fightor->posx;
		//		int old_pos_y = fightor->posy;

		//		ChessBoard* move_to_board;
		//		int posX;
		//		int posY;
		//		// ���ƶ�
		//		if (chessBoardItemTryGoStep(fightor,move_to_board,posX,posY))
		//		{
		//			int bloodSubByBuff = 0;	// buff�ļ�Ѫ��
		//			list<GameBuff> buffs = fightor->hero->getCurrentAttachedBuff();
		//			// ���Ƿ��м�Ѫ��buff������ƶ�֮��͹��ˣ���ô������ӷ��ٹ�
		//			for (list<GameBuff>::iterator it_buf = buffs.begin();
		//				it_buf != buffs.end();it_buf++)
		//			{
		//				if (it_buf->effect_vec.empty())
		//					continue;

		//				bloodSubByBuff += -(it_buf->effect_vec.begin())->persistent_hp_add;
		//			}
		//			// buff�ͻ��Ѫ����������ô��ֻ�ܳ����üӷ�
		//			if (bloodSubByBuff >= fightor->hero->getHp())
		//			{
		//				try_use_skill = true;
		//				break;
		//			}


		//			// ��ʱ��λ��
		//			fightor->currentBoard = move_to_board;
		//			fightor->posx = posX;
		//			fightor->posy = posY;

		//			//�ٴο��Ƿ���Թ���
		//			ChessBoardItem* attackedItem= prepareChessBoardBeFightedItem(fightor);
		//			if (attackedItem != NULL && attackedItem->hero->getHp() > 0)
		//			{
		//				// û�б����Ŀ���
		//				int hp_sub = fightor->hero->getShangHaiValue(attackedItem->hero,Range_Min);
		//				if (hp_sub >= attackedItem->hero->getHp())	// Ū�����Է����Ͳ��üӷ�
		//				{
		//					try_use_skill = false;
		//				}
		//			}

		//			// �ָ�λ��
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

		//// �ӷ���û�й����ļ��ܾͲ�������
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


    // BOSS�� ������ִ���
    //bool isBOSSUseBlood = false;    //boss��Ѫ
    bool isSkillSpecialTrigger = false; //�������⴦��
    if(!fightor->hero->skilVec.empty()) {
        if(fightor->hero->skilVec[0].skillType == SkillType_BOSS_Sweep) {
            //Ѫ��С��15%��û�д�����
            if((float)fightor->hero->getHp()/(float)fightor->hero->getDefaultHp() <= 0.15f && 
                !fightor->hero->getSpecialTrigger()) {
                isSkillSpecialTrigger = true;
            }
        }
        /*
        else if(fightor->hero->skilVec[0].skillType == SkillType_BOSS_Flog) {
            //Ѫ��С��30%��û�д�����
            if((float)fightor->hero->getHp()/(float)fightor->hero->getDefaultHp() <= 0.30f) {
                isBOSSUseBlood = true;
            }
        }
        */
    }

    
	bool useBlood = false;
	OneStep stepUseBlood;
	// ���Ƿ���Ҫ��Ѫ����Ҫ�Ļ����ͳ��Լ�Ѫ
	// ��Ѫ����������Ҫ��Ѫ(ʧѪ����)+����cdʱ��+��Ѫƿ
	if ( (float)fightor->hero->getHp()/(float)fightor->hero->getDefaultHp() <= 0.5f &&
		 //fightor->getCurrentBloodCdTime() <= 0 &&
		 fightor->hero->getCdTime() <= 0 &&
		 fightor->getOriginBoard()->getCurrentBloodNum() > 0 
		 //&& fightor->hero->getXSize() < 2 /* ������ų�boss ���bossҲ���Ѫ ��Ϊboss��Ҫ���⴦�� */
        )
	{
		useBlood = true;
		stepUseBlood.stepType = StepType_UseBlood;
		stepUseBlood.useBlood.uniqueId = fightor->getUniqueId();

		// ��ʵ�ļ�Ѫ
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
    // boss������Ѫ
    if(isBOSSUseBlood) {
        stepUseBlood.stepType = StepType_UseBlood;
		stepUseBlood.useBlood.uniqueId = fightor->getUniqueId();

		// ��ʵ�ļ�Ѫ
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


	bool isSkillGiveUp = false; // ���ܷ���������ת�����չ�
	// ��ʹ�ü���
	{
		// ����
		if (isSkillSpecialTrigger || 
            canUseSkillInThisTurnAndCurrentSituation(fightor))
		{
			F_LOG("Use Skill .......");
            if(isSkillSpecialTrigger) { //��boss���⴦���־�ó��Ѿ�����
                fightor->hero->setSpecialTrigger();
            }

			const float float_accuracy = 0.0001f;
			if (fightor->hero->getGongJiJuLi() > (1.0f+float_accuracy) )
			{// Զ�̣���ô���ȿ��Ƿ���Թ������оͲ��ƶ���û�оͳ����ƶ����ƶ��ɹ����ٿ��Ƿ���Թ���

				F_LOG("Use Skill --> gongjijuli > 1.0f !");

				vector<OneStep> ret;

				//1. ��Ѫ or boss��Ѫ
				if (useBlood /*|| isBOSSUseBlood*/)	
					ret.push_back(stepUseBlood);

				//2. ����
				OneStep step_skill;
				tryFightWithSkill(fightor,step_skill);
				if (!step_skill.skillData.targetUniqueVec.empty())
				{
					ret.push_back(step_skill);
					return ret;
				}
				 
				//3. �����ƶ�   ---> �ƶ������ˣ��㵹ù
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
						//4. �ٴγ��Թ���
						OneStep step_skill2;
						tryFightWithSkill(fightor,step_skill2);
						if (!step_skill2.skillData.targetUniqueVec.empty())
						{
							ret.push_back(step_skill2);
							return ret;
						}else
						{
							return ret; // ֻ�����ƶ�+��Ѫ��
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
			{// ��ս����ô���ȿ��Ƿ�����ƶ����ƶ���֮���ٹ���

				F_LOG("Use Skill --> getGongJiJuLi() <= 1.0f!");

				vector<OneStep> ret;

				// 1. ��Ѫ
				if (useBlood)	
					ret.push_back(stepUseBlood);

				// 2.�����ƶ� --> �ƶ��������㵹ù
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
				// 3.���Թ���
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


	// ��ͨ����
	{
		// 2014-03-26
		// ���ڵ�ǰ��ս��ģʽ�����ƶ����������ܴ��Ҫ������attackedItems�϶���Ϊ�յ�
		// ����ֻ������֧1
		vector<ChessBoardItem*> attackedItems = getBeFightedItem(fightor);
		if (!attackedItems.empty())
		{// ��Ŀ�ֱ꣬�ӹ���

			fightor->hero->fightTurnInc(); // ���¹����ִ�

			vector<OneStep> ret;
			OneStep step_fight;

			// ֪ͨ���̣�����һ���ɿ����Ĳ���
			mHelper->one_visible_step(step_fight.cdTime_arr,fightor->getUniqueId());
			mBuffProc->one_visible_step(step_fight.buff);

			// �󹥻�����Ȼ�ȹ�������ľ���one_visible_step���Ƴ��ˣ�������ͨ��������buff������ν
			fight_normal(step_fight,fightor,attackedItems);

			if (useBlood)	ret.push_back(stepUseBlood);
			ret.push_back(step_fight);

			F_LOG("fight step buffData = **[%s]**",step_fight.getBuffString().c_str());
			return ret;
		}
		else
		{// û��Ŀ�꣬���ƶ�һ������
			ChessBoard* move_to_board;
			int posX;
			int posY;
			// ���ƶ�
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
				

				// ֪ͨ���̣�����һ���ɿ����Ĳ��࣬�������ν�Ⱥ�
				mHelper->one_visible_step(step_move.cdTime_arr,fightor->getUniqueId());
				mBuffProc->one_visible_step(step_move.buff);

				if (useBlood)	ret.push_back(stepUseBlood);
				ret.push_back(step_move);

				F_LOG("fight move step buffData = **[%s]**",step_move.getBuffString().c_str());
				// �Ѿ����ˣ��ͷ���
				if (fightor->hero->getHp() <= 0)
				{
					F_LOG("Fight : target dead after moved.....");
					return ret;
				}

				attackedItems = getBeFightedItem(fightor);
				if (!attackedItems.empty())
				{
					fightor->hero->fightTurnInc(); // ���¹����ִ�

					OneStep step_fight;

					mHelper->one_visible_step(step_fight.cdTime_arr,fightor->getUniqueId());
					mBuffProc->one_visible_step(step_fight.buff);

					// �󹥻�����Ȼ�ȹ�������ľ���one_visible_step���Ƴ��ˣ��������û��buff������ν
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
			{// �����ƶ�,Ҳ���ܹ���
				vector<OneStep> ret;
				OneStep step_wait;
				step_wait.stepType = StepType_Wait;	// ����Buff�Ĳ���

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

	// ���ƶ������˵�����,����֮ǰû���ڱ������̣���ô����Ҫ������
	if (!out_move.inSelfChessBoard && item->currentBoard == item->getOriginBoard())
	{
		item->currentBoard->removeItem(item);
		desBoard->removeItem(posX,posY);
		desBoard->addItem(item,posX,posY);
	}
	// ���û�����
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
	// ����һ�ֵĹ�����ʷ����������
	{
		if ( fightor->getGeneBoard() == mHelper->getBoardA() )
		{
			int aHistoryNum = A_LastTurnFightHistory.size();
			bool isNewRound = false;

			for (int i=0;i<aHistoryNum;i++)
			{
				if (A_LastTurnFightHistory[i] == fightor)
				{// ֤��fightor��һ���Ѿ�����ˣ���ô�������µ�һ����
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

			// ���fightor�����������µ�һ�غ���
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
				{// ֤��fightor��һ���Ѿ�����ˣ���ô�������µ�һ����
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

			// ע�⣺
			// Ӧ����һ����һ���������µ�һ�ֿ�ʼ����ô�����µ�һ����
			// ��Ӧ�������ֵ��жϡ���Ϊ�������ֵ�[0,0]λ�ù���
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


	OneStep roundChangeStep; //newRoundFlagΪtrue������Ч
	// �غ��л�
	{
		if (newRoundFlag)
		{
			roundChangeStep.stepType = StepType_ChangeRound;
			mBuffProc->fightRoundChange(roundChangeStep.buff);
			roundChangeStep.changeRound.round = roundIndex;

			F_LOG("%s new round[roundIndex = %d] begin!",__FUNCTION__,roundIndex); 
			F_LOG("stepRound buffData = **[%s]**",roundChangeStep.getBuffString().c_str());

			// �޸�2014-03-08 ���ִ��л���ʱ���޸�cd
			// ����cdTime
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


	// ����ѣ�ε�buff,����һ��ĵȴ���ֻ�����Ϲ���һ��Buffͼ�꣬���Բ��ᴥ��buff_step
	{
		OneStep step;
		step.stepType = StepType_Wait;
		bool canFight = mBuffProc->beforeFight(fightor->getUniqueId(),step.buff);
		if (!canFight)
		{
			F_LOG("FightControler::oneChessBoardFight fightor is [xuanyun],waiting.");
			F_LOG("xuanyun step buffData = **[%s]**",step.getBuffString().c_str());

			// bug:�޸�
			// 2014-04-01
			// �����������䣬��Ȼѣ��ʱmCurrentFightIndexû�и���
			// ��ôÿ�ζ�����������ֱ��ѣ����ϡ�
			// Ӧ����ѣ�εĻ����ͷ���
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

	// �쳣
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



	// ͳ�ƿ�Ѫ��ֵ
	// ����Ͱ��ս������bug�ķ�ʽ��ͳ�ƣ����Ǹ��ݹ����ߵĶ�����ͳ��Ѫ��
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

					// ���ݹ����Ķ�����ͳ��Ѫ��
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
						CCAssert(false,"δ֪��Դ");
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

					// ���ݹ����Ķ�����ͳ��Ѫ��
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
						CCAssert(false,"δ֪��Դ");
					}
				}

				// buff�Ĳ�ͳ��

			}else
			{
				CCAssert(false,"δ֪��step");
			}
		}
	}
	vector<OneStep> ret;
	if (newRoundFlag) // ����round
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
		CCAssert(false,"������");
		fightIndex = num;
	}
	// ������
	for (int i=fightIndex;i<num;i++)
	{
		fightor = getOneItemFrom2Board(mFightQueue[i]);
		if (fightor != NULL)
		{
			outIndex = i;
			return fightor;
		}
	}
	// ��ͷ��
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
#define WORLD_BOSS_PROFESSION	95 // ����boss��ְҵ

	vector<ChessBoardItem*> attackedItems;
	{// Ѱ��Ŀ��
		if (fightorItem->hero->getProfession() == Profession_LieShou ||
			fightorItem->hero->getProfession() == WORLD_BOSS_PROFESSION)
		{
			// �Ź������ȿ���0�ţ��ٿ��������
			ChessBoard* targetBoard = mHelper->getBoardA() == fightorItem->getOriginBoard() ? mHelper->getBoardB() : mHelper->getBoardA();
			vector<ChessBoardItem*> allItems = mHelper->getAlivedXItems(targetBoard,0);
			if (allItems.empty())
				allItems = mHelper->getAlivedXItems(targetBoard,1);
			int num = allItems.size();
			for (int itemIndex=0;itemIndex<num;itemIndex++)
			{
				CCAssert(allItems[itemIndex]->hero->getHp() > 0,"�쳣");
				attackedItems.push_back(allItems[itemIndex]);
			}
		}
		else if(fightorItem->hero->getProfession() == Profession_ShenJianShou)
		{
			if (dis > 0)
			{
				CCLog("=============>Waring : %s --> Profession_ShenJianShou , use the specified dis > 0,we will ingore this");
			}
			// ����Χ���е�
			ChessBoard* targetBoard = mHelper->getBoardA() == fightorItem->getOriginBoard() ? mHelper->getBoardB() : mHelper->getBoardA();
			vector<ChessBoardItem*> allItems = mHelper->getAlivedItemsByOriginBoard(targetBoard);;
			ChessBoardItem* weakItem = getWeaklessAndDyingItem(allItems,fightorItem);
			CCAssert(weakItem!=NULL,"");
			attackedItems.push_back(weakItem);
		}
		else
		{
			// ���幥��
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
	// �޸ģ�2014-03-03
	// �����þ�����������
	//
	if (fightorItem->hero->getProfession() == Profession_ShenJianShou)
	{
		CCLog("=============>Waring : %s ---> ShenJianShou should not call this funciton(Are you gived him a wrong skill type?)!",__FUNCTION__);
	}
	return getAroundICanFightItemInNearestByY(fightorItem);








	// ���ܵ�,��ȫ����
	if (fightorItem->hero->getProfession() == Profession_ShenJianShou)
	{
		CCLog("=============>Waring : %s ---> Please make sure that ShenJianShou will use distance limit when fight(Are you gived him a wrong skill type >)!",__FUNCTION__);
		// ����Χ���е�
		ChessBoard* opBoard = mHelper->getBoardA() == fightorItem->getOriginBoard() ? mHelper->getBoardB() : mHelper->getBoardA();
		vector<ChessBoardItem*> allItems = mHelper->getAlivedItemsByOriginBoard(opBoard);
		vector<ChessBoardItem*> targets;
		// �о���ɸѡ,��ɸһ�ξ���
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

	// Ѱ�������ģ�Ҳ���ǹ������������˼�Ѫ����
	ChessBoardItem* weakless = itemSet[0];
	unsigned int maxShangHaiValue = fightor->hero->getShangHaiValue(itemSet[0]->hero,Range_Min);

	ChessBoardItem* dying = itemSet[0];
	unsigned int minHpLeft = weakless->hero->getHp() > maxShangHaiValue?weakless->hero->getHp() - maxShangHaiValue:0;

	// ���ٷ���
	if (minHpLeft == 0)
	{
		return dying;
	}

	for (int i=1;i<num;i++)
	{
		// �ҷ�������
		unsigned int tempValue = fightor->hero->getShangHaiValue(itemSet[i]->hero,Range_Min);
		if ( tempValue > maxShangHaiValue )
		{
			maxShangHaiValue = tempValue;
			weakless = itemSet[i];
		}

		// �ҹ������֮��Ѫ��ʣ�����ٵ�
		unsigned int tempLeftHp = itemSet[i]->hero->getHp() > tempValue?itemSet[i]->hero->getHp() - tempValue:0;
		if (tempLeftHp < minHpLeft)
		{
			minHpLeft = tempLeftHp;
			dying = itemSet[i];
		}
	}

	// ��ֱ�Ӵ������ˣ���ô��ѡֱ�Ӵ�����,����ͷ���ɱ��������
	if (minHpLeft == 0)
		return dying;

	return weakless;
}

// �����Y�����ϣ��ҹ�����Χ���棬����������Ǹ�����Item
ChessBoardItem* FightControler::getYICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis /* = -1 */)
{
	vector<ChessBoardItem*> allItems = mHelper->getYItems(fightor->posx);
	return getICanFightItemInGongJiJuLi(allItems,fightor,dis);
}
// �����X�����ϣ��ҹ�����Χ���棬����������Ǹ�����Item
ChessBoardItem* FightControler::getXICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis /* = -1 */)
{
	vector<ChessBoardItem*> allItems = mHelper->getXItems(fightor->currentBoard,fightor->posy);
	return getICanFightItemInGongJiJuLi(allItems,fightor,dis);
}
//
ChessBoardItem* FightControler::getAroundICanFightItemInGongJiJuLi(ChessBoardItem* fightor,float dis /* = -1 */)
{
	// �ȿ�Y������
	ChessBoardItem* yItem = getYICanFightItemInGongJiJuLi(fightor,dis);
	if (yItem)
		return yItem;

	// �ٿ�X������
	ChessBoardItem* xItem = getXICanFightItemInGongJiJuLi(fightor,dis);
	if (xItem)
		return xItem;

	// �ٿ���Χ���е�
	ChessBoard* opBoard = mHelper->getBoardA() == fightor->getOriginBoard() ? mHelper->getBoardB() : mHelper->getBoardA();
	vector<ChessBoardItem*> allItems = mHelper->getAlivedItemsByOriginBoard(opBoard);
	return getICanFightItemInGongJiJuLi(allItems,fightor,dis);
}

ChessBoardItem* FightControler::getAroundICanFightItemInNearestByY(ChessBoardItem* fightor)
{
	CCAssert(fightor->getGeneBoard() == fightor->currentBoard,"��֧���ƶ�����");
	ChessBoardItem* ret = NULL;

	ChessBoard* opBoard = (fightor->getOriginBoard() == mHelper->getBoardA()?mHelper->getBoardB():mHelper->getBoardA());
	
	vector<ChessBoardItem*> itemsInMyRow = mHelper->getXItems(fightor->currentBoard,fightor->posy);

	F_LOG("%s--> find from my side,this must not found!",__FUNCTION__);
	float MAX_DIS = GRID_NUM_X*2+GRID_NUM_Y*2 + 2;
	ret = getNearestItemFromSet(itemsInMyRow,fightor,MAX_DIS,ItemFilter_enemy);
	if (ret != NULL)
	{
		CCLog("Error : %s ---> there are enemys at my side!",__FUNCTION__);
		CCAssert(false,"������������ܴ���");
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


// �õ���itemSet�У����ҵĹ�����Χ���棬�ҿ��Թ����Ľ�ɫ(һ���ǻ��ŵ�);
ChessBoardItem* FightControler::getICanFightItemInGongJiJuLi(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor,float dis /* = -1 */)
{
	return getNearestItemFromSet(itemSet,fightor,dis>=0?dis:fightor->hero->getGongJiJuLi(),ItemFilter_enemy);
}

ChessBoardItem* FightControler::getNearestItemFromSet(vector<ChessBoardItem*> itemSet,ChessBoardItem* fightor,float juLi,ItemFilter filter /* = ItemFilter_enemy */)
{
	// û���˾�ֱ�ӷ���
	if(itemSet.size() <= 0)
	{
		F_LOG("FightControler::getItemFromSet ---> No one in this set");
		// ���ܹ���
		return NULL;
	}

	// ��������Լ�����ôֱ�ӷ���
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
		// Ҫ�ҵ��˵���������Լ��˾ͺ���
		if (filter == ItemFilter_enemy && itemSet[i]->getOriginBoard() == fightor->getOriginBoard())
			continue;
		// Ҫ�Ҷ��ѵ�������ǵ��˾ͺ���
		else if (filter == ItemFilter_teammates && itemSet[i]->getOriginBoard() != fightor->getOriginBoard())
			continue;
		else if(filter != ItemFilter_enemy && filter != ItemFilter_teammates)
			CCAssert(false,"��֧�ֵ�Filter");

		float tempDistance = mHelper->distance(fightor,itemSet[i]);
        /* ��xSize >=2 ʱ�� ��Ϊ������x���� ����ͬһ�еľ������ͬ �ұߵľͻḲ����ߵ�item */
		//if (tempDistance <= minDistance && itemSet[i]->hero->getHp() > 0) 
		// Ŀǰ������С�����һ��ǻ��ŵ�item
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
	//ȥ���ƶ�������
	return false;


	// �õ��Աߵ�Item,�Աߵ��е��ˣ��ͼ��������ǰ���ƶ�����������εģ�ϸ������ôӦ�õ���
	// �жϻ���������ʵ�֣�������ʱ��isTeammate���ж�
	ChessBoardItem* sideItem = item->currentBoard->getActorItem(item->posx == 0?1:0,item->posy);
	if (sideItem != NULL && sideItem->hero->getHp() > 0 && !mHelper->isTeammate(item->getUniqueId(),sideItem->getUniqueId()))
	{
		return false;
	}

	mHelper->getNextYPosOfItem(item,out_moveToBoard,out_posy);
	out_posx = item->posx;

	// ��Item����ôǰ��һ��ֻҪ��һ������
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
// �Զ���ʼ������
// ��Ҫ�����̶���ʼ�����֮�����
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
		CCAssert(false,"������");
		fightIndex = num;
	}
	// ������
	for (int i=fightIndex;i<num;i++)
	{
		fightor = teamBoard->getActorItemByUId(mFightQueue[i]);// getOneItemFrom2Board(mFightQueue[i]);
		if (fightor != NULL)
		{
			outIndex = i;
			return fightor;
		}
	}
	// ��ͷ��
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

// ���ܻ᷵�ضಽ����fight��skill��premove �������
vector<OneStep> TeamFightControler::oneChessBoardFight()
{
	int index;
	ChessBoardItem_TeamFight* fightor = prepareFightItemFromIndex(mCurrentFightIndex,index);

	// ����ѣ�ε�buff,����һ��ĵȴ���ֻ�����Ϲ���һ��Buffͼ�꣬���Բ��ᴥ��buff_step
	{
		OneStep step;
		step.stepType = StepType_Wait;
		bool canFight = mBuffProc->beforeFight(fightor->getUniqueId(),step.buff);
		if (!canFight)
		{
			F_LOG("FightControler::oneChessBoardFight fightor is [xuanyun],waiting.");
			F_LOG("xuanyun step buffData = **[%s]**",step.getBuffString().c_str());

			// bug:�޸�
			// 2014-04-01
			// �����������䣬��Ȼѣ��ʱmCurrentFightIndexû�и���
			// ��ôÿ�ζ�����������ֱ��ѣ����ϡ�
			// Ӧ����ѣ�εĻ����ͷ���
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
		int index = mLastBossFightedTeamUId -1;					// ��Ӧ��index
		int u_id_next = (index+1)%FormationHeroAcountMax + 1;	// ��һ������index+1��Ȼ���indexת��u_id
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

	// Ѱ�������ģ�Ҳ���ǹ������������˼�Ѫ����
	ChessBoardItem_TeamFight* weakless = itemSet[0];
	unsigned int maxShangHaiValue = fightor->hero->getShangHaiValue(itemSet[0]->hero);

	ChessBoardItem_TeamFight* dying = itemSet[0];
	unsigned int minHpLeft = weakless->hero->getHp() > maxShangHaiValue?weakless->hero->getHp() > maxShangHaiValue:0;

	// ���ٷ���
	if (minHpLeft == 0)
	{
		return dying;
	}

	for (int i=1;i<num;i++)
	{
		// �ҷ�������
		unsigned int tempValue = fightor->hero->getShangHaiValue(itemSet[i]->hero);
		if ( tempValue > maxShangHaiValue )
		{
			maxShangHaiValue = tempValue;
			weakless = itemSet[i];
		}

		// �ҹ������֮��Ѫ��ʣ�����ٵ�
		unsigned int tempLeftHp = itemSet[i]->hero->getHp() > tempValue?itemSet[i]->hero->getHp() - tempValue:0;
		if (tempLeftHp < minHpLeft)
		{
			minHpLeft = tempLeftHp;
			dying = itemSet[i];
		}
	}

	// ��ֱ�Ӵ������ˣ���ô��ѡֱ�Ӵ�����,����ͷ���ɱ��������
	if (minHpLeft == 0)
		return dying;

	return weakless;
}

// ս������Ѫ
void TeamFightControler::fight_normal(OneStep& outStep,ChessBoardItem_TeamFight* fightor,vector<ChessBoardItem_TeamFight*> attackedItems)
{
	CCAssert(attackedItems.size() > 0,"Ҫ�й�������");

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
				// ������ֵ
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

		// û��Miss ��Ҫ��Ѫ
		if (!canMiss)
		{
			// ʵ��ҲҪ��Ѫ
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
// ս������Ѫ����buff
void TeamFightControler::fight_skill(OneStep& outStep,SkillDataForFight skill,unsigned int f_uniqueId,SkillFight skillFight)
{
	CCAssert(skillFight.targetVec.size() > 0,"Ҫ�й�������");

	outStep.stepType = StepType_Skill;
	outStep.skillData.skillType = skill.skillType;
	outStep.skillData.fightorUniqueId = f_uniqueId;
	outStep.skillData.canMissVec = skillFight.canMissVec;
	outStep.skillData.skillPatten = skillFight.skillPatten;
	outStep.skillData.targetBloodChange = skillFight.targetBloodChange;
	outStep.skillData.targetUniqueVec = skillFight.targetVec;
	//ת����client��
	int buff_size = skillFight.firedBuff.size();
	for (int buffIndex =0;buffIndex < buff_size;buffIndex++)
	{
		ClientGameBuff buff(skillFight.firedBuff[buffIndex]);
		outStep.skillData.firedBuff.push_back(buff);
	}

	int targetNum = outStep.skillData.targetUniqueVec.size();
	// �ȵ���fightEnd
	if (targetNum > 0)
	{
		mBuffProc->fight_end(f_uniqueId,outStep.buff);
	}
	for (int i = 0;i<targetNum;i++)
	{
		ChessBoardItem_TeamFight* oneTarget = teamBoard->getActorItemByUId(outStep.skillData.targetUniqueVec[i]);
		CCAssert(oneTarget!=NULL,"�������");
		CCAssert(oneTarget->hero->getHp() > 0,"������");

		// û��Miss ��Ҫ��Ѫ,����buff
		if (!outStep.skillData.canMissVec[i])
		{
			// ʵ��ҲҪ��Ѫ
			oneTarget->hero->addHp(outStep.skillData.targetBloodChange[i]);

			mBuffProc->beattacked(f_uniqueId,oneTarget->getUniqueId(),outStep.skillData.targetBloodChange[i],outStep.buff);

			// ����Ѫ,���buff
			if (oneTarget->hero->getHp() > 0 && skillFight.firedBuff.size() > 0)
			{
				// buff ����
				int bufNum = skillFight.firedBuff.size();
				for (int buffIndex = 0;buffIndex < bufNum;buffIndex++)
				{
					if (skillFight.firedBuff[buffIndex].attachedToUId == oneTarget->getUniqueId())
					{
						// ����buff
						// �Ƿ������buff���о͸���
						if (!skillFight.firedBuff[buffIndex].cumulative && oneTarget->hero->haveBuffWithStateIs(skillFight.firedBuff[buffIndex].state))
						{
							bool suc = oneTarget->hero->updateBuff(skillFight.firedBuff[buffIndex]);
							CCAssert(suc,"�����ɹ�����Ȼ����bug");
						}else
						{
							bool suc = oneTarget->hero->attachBuff(skillFight.firedBuff[buffIndex]);
							CCAssert(suc,"�����ɹ�����Ȼ����bug");
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

	// ����
	if ( fightor->getUniqueId()!=TEAM_BOSS_U_ID && (float)fightor->hero->getHp() <=0)
	{
		if (fightor->hero->getCdTime() > 0 ) // cdʱ�仹û�������ܲ�����ֻ��wait
		{
			vector<OneStep> ret;
			OneStep step_wait;
			step_wait.stepType = StepType_Wait;	// ����Buff�Ĳ���
			// cdʱ��,�Ŷ�����������Ҫ������ʱ�򣬵����ִ���cdʱ���ʱ��Ÿ���cdʱ��
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

		// ��ʵ�ļ�Ѫ
		stepUseBlood.useBlood.bloodChangeForUser = fightor->hero->getDefaultHp();	// �򵥵ģ�ֱ�Ӽ�Ĭ��ֵ������
		fightor->hero->addHp(stepUseBlood.useBlood.bloodChangeForUser);
		// ����֮����freze
		//fightor->setCurrentFreezyNum(fightor->hero->getTeamProperty().mDefaultFreezyNum);

		F_LOG("ReLoadBlood -> bloodAdd = %d, fightor[u_id = %u,index = %d]",
			stepUseBlood.useBlood.bloodChangeForUser,
			fightor->getUniqueId(),fightor->currentIndex);
	}



	bool try_use_skill = true;
	do // ���ܵĴ�������
	{
		// �����ã��Ͳ���Ҫ����
		if (!fightor->hero->canUseThisInCurrentFightTurn())
		{
			try_use_skill = false;
			break;
		}
		// ����֮����(�ӷ�)������Ҫ����
		if (fightor->hero->skilVec[0].skillType == SkillType_ShenZhiXingNian)
		{
			//���Ƿ����ֱ�ӹ���
			//ChessBoardItem* attackedItem= prepareChessBoardBeFightedItem(fightor);
			ChessBoardItem_TeamFight* attackedItem = fightor->getUniqueId() == TEAM_BOSS_U_ID?
				prepareBeFightedByBoss():
				teamBoard->getActorItemByUId(TEAM_BOSS_U_ID);

			if (attackedItem != NULL && attackedItem->hero->getHp() > 0)
			{
				// û�б����Ŀ���
				int hp_sub = fightor->hero->getShangHaiValue(attackedItem->hero);
				if (hp_sub >= attackedItem->hero->getHp())	// Ū�����Է����Ͳ��üӷ�
				{
					try_use_skill = false;
					break;
				}
			}
		}

	} while (false);



	bool isSkillGiveUp = false; // ���ܷ���������ת�����չ�
	// ��ʹ�ü���
	{
		//float chufaLv = ((float)fightor->hero->getJiNengChuFaLv())/100.0f;
		bool useSkill = fightor->hero->canUseThisInCurrentFightTurn();
		// ����
		if (try_use_skill && useSkill)
		{
			F_LOG("Use Skill .......");
			SkillDataForFight skill = fightor->hero->skilVec[0];
			SkillFight skillFight = mSkillHelper->fight(fightor,skill);

			if (skillFight.targetVec.size() > 0)
			{// ��Ŀ�ֱ꣬�ӹ���

				fightor->hero->fightTurnInc(); // ���¹����ִ�

				OneStep step;
				fight_skill(step,skill,fightor->getUniqueId(),skillFight);

				// ֪ͨ���̣�����һ���ɿ����Ĳ���
				teamBoard->one_visible_step(step.cdTime_arr,fightor->getUniqueId());
				// һ����Ͼͼ��
				mBuffProc->one_visible_step(step.buff);

				vector<OneStep> ret;
				if (useBlood)	ret.push_back(stepUseBlood);
				ret.push_back(step);
				F_LOG("skill fight step buffData = **[%s]**",step.getBuffString().c_str());

				return ret;
			}
			else
			{// û��Ŀ�꣬��wait
				F_LOG("Skill : No target, we will use normal fight try again!");
				isSkillGiveUp = true;

				// ���ﲻ���أ���Ӧ�����չ�����
				//OneStep step_wait;
				//step_wait.stepType = StepType_Wait;// ����buff�Ĳ���
				//F_LOG("Skill : wait...");
				//vector<OneStep> ret;
				//if (useBlood)	ret.push_back(stepUseBlood);
				//ret.push_back(step_wait);
				//return ret;
			}
		}
	}


	// ��ͨ����
	{
		// ׼���������Ķ���
		vector<ChessBoardItem_TeamFight*> attackedItems;
		{
			if (fightor->getUniqueId() == TEAM_BOSS_U_ID)//��boss
			{
				ChessBoardItem_TeamFight* attackedItem = prepareBeFightedByBoss();
				if (attackedItem && attackedItem->hero->getHp() > 0)
				{
					mLastBossFightedTeamUId = attackedItem->getUniqueId();
					attackedItems.push_back(attackedItem);
				}
			}
			else// ���Ŷ�
			{
				ChessBoardItem_TeamFight* attackedItem = teamBoard->getActorItemByUId(TEAM_BOSS_U_ID);
				CCAssert(attackedItem != NULL && attackedItem->hero->getHp() > 0,"");
				attackedItems.push_back(attackedItem);
			}
		}
		

		if (!attackedItems.empty())
		{// ��Ŀ�ֱ꣬�ӹ���

			fightor->hero->fightTurnInc(); // ���¹����ִ�

			vector<OneStep> ret;
			OneStep step_fight;
			fight_normal(step_fight,fightor,attackedItems);
			// �����ˣ��ͽ���cdʱ��
			int targetNum = attackedItems.size();
			for (int tarIndex=0;tarIndex<targetNum;tarIndex++)
			{
				if (attackedItems[tarIndex]->hero->getHp() <=0 )
				{
					attackedItems[tarIndex]->hero->setCdTime(attackedItems[tarIndex]->hero->getDefaultCdTime());
				}
			}

			// ֪ͨ���̣�����һ���ɿ����Ĳ���
			teamBoard->one_visible_step(step_fight.cdTime_arr,fightor->getUniqueId());
			mBuffProc->one_visible_step(step_fight.buff);

			if (useBlood)	ret.push_back(stepUseBlood);
			ret.push_back(step_fight);

			F_LOG("fight step buffData = **[%s]**",step_fight.getBuffString().c_str());
			return ret;
		}
		else
		{// û��Ŀ�ֱ꣬��wait

			vector<OneStep> ret;
			OneStep step_wait;
			step_wait.stepType = StepType_Wait;	// ����Buff�Ĳ���

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
