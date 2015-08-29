#include "ClientFightLayer_TF.h"
#include "GameDirector.h"
#include "Music.h"
#include "LFToast.h"
//#include "FightEndLayer.h"
#include "MyselfManager.h"
#include "font.h"
#include "LFAlert.h"
#include "SpriteHelper.h"
#include "ImageManager.h"
#include "ClientChessBoard.h"
#include "ClientSkillProcessor.h"
#include "TuanDuiEndLayer.h"
#include "Debug.h"
#include "JumpHolder.h"
#include "FightTrigleDriver.h"

static float fight_gap = 0.7f;				// 攻击间隔
static float last_fight_wait = 0.0f;
static bool pulled = false;			// 是否有东西掉出 
static ClientFightLayer_TF* g_current_instance = NULL;

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#if SHOW_FIGHT_DEBUG == 1
#define DEBUG_ENABLE
#endif
#endif

#define TAG_ITEM_RESTART	22122
#define TAG_ITEM_RETURN		22123
#define TAG_ITEM_FAILD_YES	43212	// 步数太长的失败弹框里面的Yes按钮
#define TAG_ALERT			43213	// 对话框
#define TAG_CARD_IN_BAOXIANG	43214	// 宝箱里面的卡片
#define TAG_BALCK_MASK		43215		// 用于把暴击角色分开用的
#define BLACK_MASK_Z_ORDER	4

#define TAG_FIGHT_BG		22125
#define STATE_CHANGE_STEP	0
#define STATE_CHANGE_FIGHT	1

#define TAG_BLOOD_VIAL_BAR_TOP		1232123	// 上面队伍的血瓶条
#define TAG_BLOOD_VIAL_BAR_BUTTOM	1232124 // 下面队伍的血瓶条

#define TAG_TEST_MENU 523321


static float chessBoardOffsetY = 0;


ClientFightLayer_TF::ClientFightLayer_TF(vector<FightDirector> dir_vec,CCSprite* bg,Bundle bundle /* = Bundle */)
{
	mBundle = bundle;
	mTrigle = NULL;
	initWithDirctorVecAndBg(dir_vec,bg);
	g_current_instance = this;
}

struct Temp_Id_Index
{
	int uniqueId;
	int dirIndex;
};

void ClientFightLayer_TF::initWithDirctorVecAndBg(vector<FightDirector> dir_vec,CCSprite* bg)
{
	CCAssert(dir_vec.size() > 0,"必须要有一个导演");
	for (int i = 0;i<dir_vec.size();i++)
	{
		if (!dir_vec[i].isInited())
		{
			CCAssert(false,"必须是一个准备好脚本的导演");
			return;
		}
	}

	mBg = bg;
	mCurrentStepIndex = 0;
	currentDirIndex = 0;
	mSkillProc = NULL;
	itemNext = NULL;

	mDirvec = dir_vec;
	addObserver(SEL_CallFuncO(&ClientFightLayer_TF::someOneDead),MSG_ItemDead);
	playWithDirctor(0);
}

void ClientFightLayer_TF::setData(Flat_TeamBossGetBattleScriptGiftRsp script, vector<HeroInfo> heros)
{
	this->script = script;
	this->heros = heros;
}
 
void ClientFightLayer_TF::playWithDirctor(int dirIndex)
{
	//CCDirector::sharedDirector()->getScheduler()->setTimeScale(5.0f);

	CCAssert(dirIndex >= 0 && dirIndex < mDirvec.size(),"");
	currentDirIndex = dirIndex;
	mCurrentStepIndex = 0;
	fightEnded = false;
	pulled = false;

	{
		if (mTrigle)
			mTrigle->removeAllChildrenWithCleanup(true);
		mTrigle = NULL;
		
		CC_SAFE_RETAIN(mBg);
		removeAllChildren();
		addChild(mBg,-1,TAG_FIGHT_BG);
		mBg->release();
		mBg->setPosition(ccp(mVisibleSize.width/2,mVisibleSize.height/2));
	}

	// 计时器，触发器
	mTrigle = new FightTrigleDriver();
	mTrigle->setTrigleGapTime(fight_gap);
	addChild(mTrigle);
	addObserver(SEL_CallFuncO(&ClientFightLayer_TF::fightTrigle),MSG_TRIGLE_TIME);
	addObserver(SEL_CallFuncO(&ClientFightLayer_TF::waitTimeEnd),MSG_TRIGLE_PAUSE_END);

	//BaseSprite* black_mask = BaseSprite::create("zhandou _baoji_bg.png");
	//addChild(black_mask,BLACK_MASK_Z_ORDER,TAG_BALCK_MASK);
	//black_mask->setPosition(ccp(mVisibleSize.width/2,mVisibleSize.height/2));
	//black_mask->setVisible(false);

	// 初始化棋盘
	{
		InitChessBoard initData = mDirvec[currentDirIndex].getChessBoardInitData();
		board = new ClientChessBoard_TeamFight();
		addChild(board);
		board->release();

		for (int index=0;index<4;index++)
		{
			if (initData.myTeamUniqueId[index] > 0)
			{
				ClientChessBoardItemBase* item = new ClientChessBoardItem_TF(initData.myTeam[index],initData.myTeamUniqueId[index]);
				board->addOneItem(item);
				item->release();
			}
		}

		ClientChessBoardItemBase* item = new ClientChessBoardItem_TF(initData.oppositeTeam[0],initData.oppositeUniqueId[0]);
		board->addOneItem(item);
		item->release();
	}

	mSkillProc = new ClientSkillProcessor_TF(board);

	Music::sharedMusicPlayer()->playPlayMusic();

	// 跳过
	{
		CCMenuItemImage* jump_item = CCMenuItemImage::create(
			"zhandou_tiaoguo.png",
			"zhandou_tiaoguo_select.png",
			"zhandou_tiaoguo_buke.png",
			this,SEL_MenuHandler(&ClientFightLayer_TF::jump_clicked));
		CCMenu* menu = CCMenu::create(jump_item,NULL);
		addChild(menu);
		menu->setPosition(CCPointZero);
		jump_item->setPosition(ccp(mVisibleSize.width - jump_item->getContentSize().width/2-10,jump_item->getContentSize().height/2 + 10));
	}

	{
		CCArray* top_items = CCArray::createWithCapacity(1);
		ClientChessBoardItemBase* item = board->getOneByUId(5);
		CCAssert(item!=NULL,"");
		top_items->addObject(item);
		float width = 0;
		float height = 0;
		if (top_items->count() > 0)
		{
			width = ((CCNode*)top_items->objectAtIndex(0))->getContentSize().width;
			height = ((CCNode*)top_items->objectAtIndex(0))->getContentSize().height;
		}
		float scale = 5.0f;
		width *= scale;
		height *= scale*1.2f;

		float posx = mVisibleSize.width/2;
		float posy = mVisibleSize.height + height/2.0f;
		float dist = 0;
		for (int i=0;i<top_items->count();i++)
		{
			ClientChessBoardItem* item = (ClientChessBoardItem*)top_items->objectAtIndex(i);
			item->setOpacity(0);
			CCPoint orginPos = item->getPosition();
			CCPoint layerPos = ccp(posx,posy);
			CCPoint startPos = item->getContainer()->convertToNodeSpace(convertToWorldSpace(layerPos));
			item->setPosition(startPos);
			item->setScale(scale);

			item->runAction(CCSequence::create(
				CCDelayTime::create(0.2f+0.2f*i),
				CCSpawn::create(
					CCFadeTo::create(0.2f,255),
					CCMoveTo::create(0.2f,orginPos),
					CCScaleTo::create(0.2f,1.0f),
					NULL
				),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer_TF::nodeHitGround)),
				NULL
				));
		}

		scheduleOnce(schedule_selector(ClientFightLayer_TF::actionEnd),0.4 + 0.2*top_items->count());
	}
}

void ClientFightLayer_TF::nodeHitGround(CCNode* node)
{
	Music::sharedMusicPlayer()->playEffectSound("guaiWuHitedGroud.wav");
	screenShake();
}
void ClientFightLayer_TF::screenShake()
{
	//runAction(CCSequence::create(
	//	CCRotateTo::create(0.1f,8.0f),
	//	CCRotateTo::create(0.1f,-8.0f),
	//	CCRotateTo::create(0.08f,4.0f),
	//	CCRotateTo::create(0.04f,0),
	//	NULL
	//	));

	runAction(
		
		CCSpawn::create(

		CCSequence::create(
		CCRotateTo::create(0.08f,1.0f),
		CCRotateTo::create(0.08f,-1.0f),
		CCRotateTo::create(0.08f,0.5f),
		CCRotateTo::create(0.04f,0),
		NULL
		),
		
		CCSequence::create(
		CCMoveBy::create(0.08f,ccp(0,4)),
		CCMoveBy::create(0.08f,ccp(0,-2)),
		CCMoveBy::create(0.08f,ccp(0,1)),
		CCMoveTo::create(0.0f,ccp(0,0)),
		NULL),
		NULL));
}

void ClientFightLayer_TF::actionEnd(float t)
{
	begingFightTrigle();
}

void ClientFightLayer_TF::begingFightTrigle()
{
	if (mDirvec[currentDirIndex].getStepNum() <= 0)
	{
		fightEnd(true);
		return;
	}
	mTrigle->trigle();
#ifdef DEBUG_ENABLE
	mTrigle->setAutoTrigleFalse();
	CCMenuItemFont::setFontSize(40);
	itemNext = CCMenuItemFont::create(
		getDebugStr(),
		this,SEL_MenuHandler(&ClientFightLayer_TF::debugNextStepClicked));

	CCMenuItemFont* itemFirstDir = CCMenuItemFont::create(
		"First Dir",
		this,SEL_MenuHandler(&ClientFightLayer_TF::debugGotoFirstDirctor));
	CCArray* items = CCArray::createWithCapacity(3);
	items->addObject(itemNext);
	if (mDirvec.size() > 0)
		items->addObject(itemFirstDir);

	CCMenu* menu = CCMenu::createWithArray(items);
	menu->setPosition(CCPointZero);
	addChild(menu,5,TAG_TEST_MENU);
	itemNext->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));

	float pos_x = mWinSize.width/2;
	float pos_y = mWinSize.height/2 - 150;
	float dis = 50;
	itemFirstDir->setPosition(ccp(pos_x,pos_y));pos_y -= dis;

	{
		// 返回按钮
		{
			CCMenuItemImage* itemRestart = CCMenuItemImage::create(
				"restart.png",
				"restart.png",
				this,SEL_MenuHandler(&ClientFightLayer_TF::menuItemClicked)
				);

			CCMenuItemImage* itemReturn = CCMenuItemImage::create(
				"zhuangbei_xiangxi_genghuan.png",
				"zhuangbei_xiangxi_genghuan_select.png",
				this,SEL_MenuHandler(&ClientFightLayer_TF::menuItemClicked)
				);

			CCMenu* menu = CCMenu::create(itemRestart,itemReturn,NULL);
			addChild(menu,2);
			menu->setPosition(CCPointZero);
			itemRestart->setPosition(ccp(640 - itemRestart->getContentSize().width/2,itemRestart->getContentSize().height));
			itemReturn->setPosition(ccp(itemReturn->getContentSize().width,itemReturn->getContentSize().height));

			//
			itemRestart->setTag(TAG_ITEM_RESTART);
			itemReturn->setTag(TAG_ITEM_RETURN);
			itemReturn->setVisible(false);
			itemRestart->setVisible(false);

		}
	}
#else
	itemNext = NULL;
	//unschedule(schedule_selector(ClientFightLayer::fightTrgile));
	//schedule(schedule_selector(ClientFightLayer::fightTrgile),fight_gap);
#endif
}
//
const char* ClientFightLayer_TF::getDebugStr()
{
	CCString* s = CCString::createWithFormat("Next Step[index->%d/num->%d]",mCurrentStepIndex,mDirvec[currentDirIndex].getStepNum());
	return s->getCString();
}
//
void ClientFightLayer_TF::debugNextStepClicked(CCObject* sender)
{
	mTrigle->postTrigleTime();
	//fightTrgile(0);
	itemNext->initWithString(getDebugStr(),this,SEL_MenuHandler(&ClientFightLayer_TF::debugNextStepClicked));
	if (mCurrentStepIndex > mDirvec[currentDirIndex].getStepNum() - 1)
	{
		//itemNext->setVisible(false);
	}
}

void ClientFightLayer_TF::debugGotoFirstDirctor(CCObject* sender)
{
	playWithDirctor(0);
}

void ClientFightLayer_TF::menuItemClicked(CCObject* obj)
{
	CCMenuItem* item = (CCMenuItem*)obj;
	if (item->getTag() == TAG_ITEM_RESTART)
	{
		GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	}
	else if (item->getTag() == TAG_ITEM_RETURN)
	{
		GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	}
	else if (item->getTag() == TAG_ITEM_FAILD_YES)
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_ALERT);
		timeToJumpToEndLayer(0);
	}
}

ClientChessBoardItemBase* ClientFightLayer_TF::getChessBoardItem(int heroUId)
{
	return board->getOneByUId(heroUId);
}

void ClientFightLayer_TF::removeSomeBuff(vector<int> buff_id_arr)
{
	// Top 里面寻找
	CCArray* items = board->getAllItems_CCArray();
	int itemsNumInBoardA = items->count();
	for (int i=0;i<itemsNumInBoardA;i++)
	{
		ClientChessBoardItemBase* item = ((ClientChessBoardItemBase*)items->objectAtIndex(i));
		int buffNum = buff_id_arr.size();
		for (int j=0;j<buffNum;j++)
		{
			item->removeBuffById(buff_id_arr[j]);
		}
	}
}
void ClientFightLayer_TF::doBuff(BuffInStep buff)
{
	// 以后可能就不需要指定来移除buff了
	// 可以改成自动移除，ClientBuff上面也加上移除点
	// 那么他只是一个表现形式，到点就自动移除就是了
	// 至于buff的减血等特殊效果是脚本在doBuff里面指定的，所以没有问题，没有同步的bug
	removeSomeBuff(buff.remove_buff_array);

	//用于根据targetId来分组
	map<unsigned int,vector<Buff_Doing> > buffsOnTargetMap;
	map<unsigned int,vector<Buff_Doing> >::iterator it;

	// buff作用
	int targetNum = buff.effects.size();
	for (int i=0;i<targetNum;i++)
	{
		// 不处理反弹
		if (buff.effects[i].isFantan)
		{
			CCAssert(false,"为了不引起bug，这里assert一下，因为这里不处理反弹，那么调用者就应该先把反弹的数据移掉");
			continue;
		}

		// 看是否已经存在
		it = buffsOnTargetMap.find(buff.effects[i].target);
		if (it != buffsOnTargetMap.end()) // 已经有这个target的数据了。那么就附加
		{
			it->second.push_back(buff.effects[i]);
		}else
		{
			vector<Buff_Doing> doingVec;
			doingVec.push_back(buff.effects[i]);
			buffsOnTargetMap.insert(pair<unsigned int,vector<Buff_Doing> >(buff.effects[i].target,doingVec));
		}
		//ClientChessBoardItem* item = getChessBoardItem(buff.effects[i].target);
		//CCAssert(item!=NULL,"一定能找到");
		//item->hpAdd(buff.effects[i].hp_add);
	}

	// 让item表演
	for (it = buffsOnTargetMap.begin();it!=buffsOnTargetMap.end();it++)
	{
		ClientChessBoardItemBase* item = getChessBoardItem(it->first);
		CCAssert(item!=NULL,"一定能找到");
		item->runBuffDoingEffect(it->second);
	}
}
void ClientFightLayer_TF::waitTimeEnd(CCObject* null_obj)
{
#ifdef DEBUG_ENABLE
	getChildByTag(TAG_TEST_MENU)->setVisible(true);
#endif 
}
void ClientFightLayer_TF::fightTrigle(CCObject* null_obj)
{
	if (fightEnded)
		return;

	OneStep step;
	// 只用于StepType_Fight
	MSG_Fight* fightData = NULL;
	// 只用于StepType_Fight
	vector<ClientChessBoardItemBase*> targetVec;
	vector<int> bloodSub;
	vector<bool> canMiss;

	if (mDirvec[currentDirIndex].getStep(mCurrentStepIndex,step))
	{
		// 分离出反弹的数据,放到要反弹的人身上
		{
			ClientChessBoardItemBase* temp;
			vector<Buff_Doing> fantanData = separationFanTanDataFromStep(step);
			int num = fantanData.size();
			for (int i=0;i<num;i++)
			{
				temp = getChessBoardItem(fantanData[i].fightor_u_id);
				CCAssert(temp!=NULL,"一定存在");
				temp->pushFanTanData(fantanData[i]);
			}
		}
	
	
		ClientChessBoardItemBase* itemFightor = NULL;
		switch (step.stepType)
		{
		case StepType_Fight:
			last_fight_wait = 0.0f;
			itemFightor = getChessBoardItem(step.fightData.fightorUniqueId);
			for (int i=0; i < (int)step.fightData.targetUniqueVec.size() ; i++)
			{
				ClientChessBoardItemBase* itemAttacked = getChessBoardItem(step.fightData.targetUniqueVec[i]);
				CCAssert(itemAttacked != NULL,"脚本指定的，不可能找不到");
				if (itemAttacked == NULL)
					continue;

				targetVec.push_back(itemAttacked);
				bloodSub.push_back(-step.fightData.targetBloodChange[i]);
				canMiss.push_back(step.fightData.canMissVec[i]);
			}

			fightData = MSG_Fight::create(itemFightor,targetVec,bloodSub,canMiss);

			if (step.fightData.fightPattern == Fight_Normal)
			{
				itemFightor->attack_UseNormal(fightData);
			}
			else if(step.fightData.fightPattern == Fight_Normal_BaoJi)
			{
				itemFightor->attack_UseNormalBaoJi(fightData);
			}
			else
			{
				CCAssert(false,"异常");
			}

			one_visible_step(step.cdTime_arr);
			// 
			//removeSomeBuff(step.remove_buff_array);
			doBuff(step.buff);
			break;
		case StepType_Skill:
			last_fight_wait = 1.5f;
			if (step.skillData.skillType == SkillType_ShenZhiXingNian || step.skillData.skillType == SkillType_ShouHu)
			{
				setFightTrigleWaitSomeTime(0.6f);	// 技能延迟一会,等待buff加上去
			}
			else if (step.skillData.skillType == SkillType_LeiBao)
			{
				setFightTrigleWaitSomeTime(1.5f);	// 技能延迟一会
			}
			else if (step.skillData.skillType == SkillType_ShanDian)
			{
				setFightTrigleWaitSomeTime(1.5f);	// 技能延迟一会
			}
			else if (step.skillData.skillType == SkillType_WenYi)
			{
				setFightTrigleWaitSomeTime(1.8f);
			}
			else if (step.skillData.skillType == SkillType_TianTangZhiGuang)
			{
				setFightTrigleWaitSomeTime(2.5f);
			}
			else if (step.skillData.skillType == SkillType_BaoLieZhan || (step.skillData.skillType == SkillType_ShaLu) )
			{
				setFightTrigleWaitSomeTime(0.5f);	// 技能延迟一会
			}
			else
			{
				setFightTrigleWaitSomeTime(1.0f);	// 技能延迟一会
			}

			mSkillProc->skill_fight(step.skillData);
			// 
			doBuff(step.buff);

			one_visible_step(step.cdTime_arr);

			break;
		case StepType_UseBlood:
			CCAssert(false,"不会用");
			break;
		case StepType_Move:
			CCAssert(false,"不会用");
			break;
		case StepType_ReLoadBlood: 
			// 可能的Bug
			// 当boss打一个team的时候，还没有打中减血，就来到了reload的step
			// 那么这里加血就是加满
			// 但是刚加满，刚才的fight就打到了
			// 于是加血早于了fight
			// 于是加血失败，成了bug
			// 但是只要有cd时间，那么到加血的时间就至少过了一个周期了，应该就不会了
			// 如果点击的够快也有可能出bug
			itemFightor = getChessBoardItem(step.useBlood.uniqueId);
			CCAssert(itemFightor!=NULL,"");
			itemFightor->hpAdd(step.useBlood.bloodChangeForUser);


			break;
		case StepType_Wait:
			mCurrentStepIndex ++;
			// 直接跳过，只看得到眩晕的buff移除
			doBuff(step.buff);
			//removeSomeBuff(step.remove_buff_array);
			// 没有了
			if (mCurrentStepIndex > mDirvec[currentDirIndex].getStepNum() - 1 )
			{
				CCLog("Step Over...");
				mTrigle->stop();
				fightEnd();
				return;
			}
			//fightTrgile(0);
			mTrigle->postTrigleTime();
			return;
			break;
		case StepType_FailByTakeTooMuchTime:

			// 
			doBuff(step.buff);

			mTrigle->stop();
			fightEnd();
			return;
			break;
		default:
			CCAssert(false,"");
			break;
		}
	}
	else
	{
		fightEnd(true);
	}
	mCurrentStepIndex ++;

	//// 没有了
	//if (mCurrentStepIndex > mDirvec[currentDirIndex].getStepNum() - 1 )
	//{
	//	CCLog("Step Over...");
	//	fightEnd();
	//}
}
vector<Buff_Doing> ClientFightLayer_TF::separationFanTanDataFromStep(OneStep& step)
{
	vector<Buff_Doing> ret;
	for (vector<Buff_Doing>::iterator it = step.buff.effects.begin();it!=step.buff.effects.end();)
	{
		if (it->isFantan)
		{
			ret.push_back(*it);
			it = step.buff.effects.erase(it);
		}else
		{
			it++;
		}
	}

	return ret;
}

void ClientFightLayer_TF::one_visible_step(vector<CdDiscr> cdDisc)
{
	//更新CD时间
	int num = cdDisc.size();
	for (int i=0;i<num;i++)
	{
		ClientChessBoardItem_TF* item = (ClientChessBoardItem_TF*)board->getOneByUId(cdDisc[i].uniqueId);
		if (item)
		{
			item->setCurrentFreesyTime(cdDisc[i].cdTime);
		}
	}

}

void ClientFightLayer_TF::setItemOrderBack(CCNode* node)
{
	this->reorderChild(node->getParent(),ITEM_Z_OERDER_IN_STAGE);
}

void ClientFightLayer_TF::showFailByTakeTooMuchTimeDailog()
{
	//弹框背景
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	//提示
	{
		CCLabelTTF* lable = CCLabelTTF::create(
			LFStrings::getValue("StepType_FailByTakeTooMuchTime").c_str(), 
			fontStr_kaiti,30,CCSize(431, 0), kCCTextAlignmentLeft);
		dialog->addChild(lable);
		lable->setPosition(ccp(0,0));
		lable->setPosition(ccp(dialog->getContentSize().width/2, 228));

		ccColor3B color;
		color.r = 201;
		color.g = 156;
		color.b = 18;
		lable->setColor(color);
	}

	//按键
	CCMenu* menu;
	{
		CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&ClientFightLayer_TF::menuItemClicked));
		menu = CCMenu::create(menuItemMakeSure, NULL);
		menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/2,70));
		menuItemMakeSure->setTag(TAG_ITEM_FAILD_YES);
	}

	//弹框
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128,TAG_ALERT);
		lfAlert->release();
	}

	stopAllActions();
	unscheduleAllSelectors();
}

void ClientFightLayer_TF::someOneDead(CCObject* clientChessBoardItemBase)
{
}
void ClientFightLayer_TF::setFightTrigleWaitSomeTime(float time_to_wait)
{
#ifdef DEBUG_ENABLE
	getChildByTag(TAG_TEST_MENU)->runAction(CCSequence::create(
		CCHide::create(),
		CCDelayTime::create(time_to_wait*1.5f),
		CCShow::create(),
		NULL
		));
#endif 
	mTrigle->pauseTrigleSomeTime(time_to_wait);
}
//void ClientFightLayer_TF::pauseFightTrigle(CCNode* node_not_use)
//{
//	unschedule(schedule_selector(ClientFightLayer_TF::fightTrgile));
//}
//void ClientFightLayer_TF::resumeFightTrigle(CCNode* node_not_use)
//{
//	schedule(schedule_selector(ClientFightLayer_TF::fightTrgile),fight_gap);
//}

void ClientFightLayer_TF::jump_clicked(CCObject* sender)
{
	if (!JumpHolder::getHolder()->canJump())
	{
		LFToast::makeToast(LFStrings::getValue("can_not_jump"));
		return;
	}

	fightEnd(true);
}

void ClientFightLayer_TF::fightEnd(bool notWait /* = false */)
{
	if (fightEnded)
	{
		return;
	}
	fightEnded = true;

	// 还有下一场
	if (currentDirIndex < mDirvec.size() - 1)
	{
		schedule(schedule_selector(ClientFightLayer_TF::timeToNextFight),notWait?0.1f:1.8f);
	}else
	{
		// scheduleOnce 的回调不能再schedule
		// 于是就直接用schedule
		schedule(schedule_selector(ClientFightLayer_TF::timeToJumpToEndLayer),notWait?0.1f:1.8f);
	}
}

void ClientFightLayer_TF::timeToNextFight(float t)
{
	unschedule(schedule_selector(ClientFightLayer_TF::timeToNextFight));
	if (pulled)
	{
		schedule(schedule_selector(ClientFightLayer_TF::timeToNextFight),2.5f);  
		pulled = false;
		return;
	}
	currentDirIndex++;
	playWithDirctor(currentDirIndex);
}

void ClientFightLayer_TF::timeToJumpToEndLayer(float t)
{
	unschedule(schedule_selector(ClientFightLayer_TF::timeToJumpToEndLayer));
	if (pulled)
	{
		schedule(schedule_selector(ClientFightLayer_TF::timeToJumpToEndLayer),2.5f);  
		pulled = false;
		return;
	}

	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}

	TuanDuiEndLayer* endLayer = TuanDuiEndLayer::create(script,heros);
	addChild(endLayer,1024);
	board->removeFromParent();




	//vector<EquipData> gettedEquipVec;
	//vector<ItemInfo> gettedItemVec;

	//int state = 1;	// 0：输，1成功，-1步奏太长
	//int dirNum = mDirvec.size();
	//for (int i=0;i<dirNum;i++)
	//{
	//	FightEnd endData = mDirvec[i].getScript().endResult;

	//	OneStep endStep;
	//	bool getted = mDirvec[i].getStep(mDirvec[i].getStepNum() - 1,endStep);
	//	CCAssert(getted,"肯定能得到");

	//	if (endStep.stepType == StepType_FailByTakeTooMuchTime)
	//	{
	//		state = -1;
	//		break;
	//	}
	//	else if (endData.myResult == Result_Lose)
	//	{
	//		state = 0;
	//		break;
	//	}
	//	else
	//	{
	//		for (int j=0;j<endData.gettedItem.size();j++)
	//		{
	//			gettedItemVec.push_back(endData.gettedItem[j]);
	//		}
	//		for (int j=0;j<endData.gettedZhuangBei.size();j++)
	//		{
	//			gettedEquipVec.push_back(endData.gettedZhuangBei[j]);
	//		}
	//	}
	//}

	//Settlement *taskInfo = new Settlement;
	//taskInfo->taskName = "NoName";
	//taskInfo->bossName = "NoName";
	//taskInfo->win = state;
	////taskInfo->exp = mNowData.exp_current - mBeForedata.exp_current + levelUp*mBeForedata.exp_need;
	//taskInfo->exp = 0;
	//taskInfo->items = gettedItemVec;
	//taskInfo->equips = gettedEquipVec;
	//for (int i=0;i<FormationHeroAcountMax;i++)
	//{
	//	//taskInfo->oldArmy[i] = oldArmy[i];
	//	//taskInfo->curArmy[i] = nowArmy[i];
	//}

	//FightEndLayer *retLayer = FightEndLayer::create(taskInfo);
	//CCScene *runS = CCDirector::sharedDirector()->getRunningScene();
	//runS->addChild(retLayer, 1025, TAG_FIGHT_END_LAYER);

	//delete taskInfo;
}

ClientFightLayer_TF::~ClientFightLayer_TF(void)
{
	delete mSkillProc;
	removeObserver(MSG_ItemDead);
}
