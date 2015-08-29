#include "FightPrepare.h"
#include "protocol.h"
#include "CmdHelper.h"
#include "FightScriptDefine.h"
#include "GameDirector.h"
#include "MyselfManager.h"
#include "LFMessageBox.h"
#include "ClientFightLayer.h"
#include "FightResourceLoader.h"

FightPrepare::FightPrepare(FightPrepareCallback* callback)
{
	//mState = Prepare_State_NULL;
	mCallback = callback;
	mTestMode = false;
}
void FightPrepare::setTest(bool testMode)
{
	mTestMode = testMode;
}

void FightPrepare::prepareData(int taskId,int gridIndex)
{
	mState = Prepare_State_Loading;

	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,SEL_CallFuncO(&FightPrepare::battleCallBack),MSG_BattleRsp,NULL);
	mTaskId = taskId;
	mGridIndex = gridIndex;

	BaseActorProperty pro = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty();
	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		BaseActor* actor = MyselfManager::getManager()->getMyTeamZhenShenActorByPosIndex(i+1);
		if (actor == NULL)
		{
			HeroBaseProperty pro_null;
			pro_null.hero_id = INVALID_ID;
			beforeFightPro[i] = pro_null;
			continue;
		}
		beforeFightPro[i] = actor->getCurrentProperty();
	}

	// boss 才是打的时候扣除体力
	if (gridIndex == 0 && pro.vit_current < Vitality_GuanQia_Boss)
	{
		mCallback->prepareError(TiliNotEnought);
		return;
	}
// 	if (gridIndex == 8 && pro.vit_current < Vitality_GuanQia_Boss)
// 	{
// 		mCallback->prepareError(TiliNotEnought);
// 		return;
// 	}
// 	else if (pro.vit_current < Vitality_GuanQia_Normal_Jingying)
// 	{
// 		mCallback->prepareError(TiliNotEnought);
// 		return;
// 	}

	SPCmd_Battle req;
	req.grid_id = mGridIndex;
	req.task_id = mTaskId;
	CmdHelper::getHelper()->cmdBattle(req);
}
void FightPrepare::battleCallBack(CCObject* msg_rsp)
{
	// 先设置为可以用
	mState = Prepare_State_NULL;

	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,MSG_BattleRsp);

	OneBattle* battle = (OneBattle*) ((MSG_Rsp*)msg_rsp)->getData();
	if (battle->err_code != Success)
	{
		GameDirector::getDirector()->hideWaiting();
		mCallback->prepareError(battle->err_code);
		//LFMessageBox::alert("",LF::getString("tilibugou"));
		return;
	}
	if (battle->scriptVec.size() <= 0)
	{
		GameDirector::getDirector()->hideWaiting();
		CCAssert(false,"");
		return;
	}

	mState = Prepare_State_Loaded;

	vector<FightDirector> dir_vec;
	int num = battle->scriptVec.size();
	for (int i = 0;i<num;i++)
	{
		FightDirector dir;
		dir.initScript(battle->scriptVec[i]);
		dir_vec.push_back(dir);
	}

	// 准备好的数据
	mPreparedDir_vec = dir_vec;
	// 完毕
	mCallback->prepareSuccess();

	// 不载入
	//FightResourceLoader loader;
	//loader.loadResource(battle->scriptVec);
}


void FightPrepare::fight()
{
	if (mState != Prepare_State_Loaded)
	{
		CCAssert(false,"数据还没准备好，请确认调用了prepareData函数载入数据，并且是在prepareSuccess回调接口中调用的这个函数");
		return;
	}

	Bundle bundle;
	bundle.putBool("testMode",mTestMode);
	if (!mTestMode)
	{
		bundle.putString("JumpToWhenEnd","FightEndLayer");
	}
	
	// 直接换scene
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(mPreparedDir_vec,mCallback->getBg(mTaskId,mGridIndex),bundle);
	GameDirector::getDirector()->hideWaiting();
	layer->setBeforeFightPro(beforeFightPro);
}

FightPrepare::~FightPrepare(void)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,MSG_BattleRsp);
}
