#include "ClientSkillProcessorBase.h"
#include "SpriteHelper.h"
#include "GameDirector.h"
#include "Music.h"
#include "BaseActorPropertyParser.h"
#include "LFDelayToMusic.h"
#include "SkillSprite.h"
#include "SkillTree.h"
#include "IChessboardProperty.h"
#include "SkillConfigManager.h"
#include "ClientChessBoard.h"


#define SKILL_HENGFU_FILE "skill/jinenghengfu/jinenghengfu.ExportJson"
#define SKILL_HENGFU "jinenghengfu"

#define SKILL_FILE_FORMAT "skill/%s/%s.ExportJson"
#define SKILL_ShiFa "shifa"
#define SKILL_ShouShang "shoushang"
#define SKILL_FeiXing "feixing"

// 注意：
// 我们应该把技能做一个缩放，把一个技能的效果最好缩小在一个图片里面，加载也方便....
// 
//
//


void loadPlist(string file)
{
	string _path = file;
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(_path.c_str());
}

ClientSkillProcessorBase::ClientSkillProcessorBase()
{
	mWinSize = CCDirector::sharedDirector()->getWinSize();
	mVisibleSize = CCDirector::sharedDirector()->getVisibleSize();


	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(SKILL_HENGFU_FILE);
}

StepSkill ClientSkillProcessorBase::getOneFightUniteByIndex(StepSkill step,int index)
{
	StepSkill ret;
	ret.fightorUniqueId = step.fightorUniqueId;
	ret.skillPatten = step.skillPatten;
	ret.skillType = step.skillType;
	ret.targetUniqueVec.push_back(step.targetUniqueVec[index]);
	ret.canMissVec.push_back(step.canMissVec[index]);
	ret.targetBloodChange.push_back(step.targetBloodChange[index]);

	// 把打向这个角色的Buff全部取出来
	int buffNum = step.firedBuff.size();
	for (int i=0;i<buffNum;i++)
	{
		if (step.firedBuff[i].attachedToUId == ret.targetUniqueVec[0])
		{
			ret.firedBuff.push_back(step.firedBuff[i]);
		}
	}
	return ret;
}

void ClientSkillProcessorBase::runDefaultFightorAction(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* target,float delay /* = -1 */)
{
	float angle = 45;
	if (target != NULL)
	{
		CCPoint vec = LF::getVectorFromAToB(fightor,target);
		angle = vec.y > 0 ? -45.0f:45.0f;
	}
	CCAction* action = NULL;

	if (delay > 0)
	{
		action = CCSequence::create(
			CCDelayTime::create(delay),
			CCScaleTo::create(0.2f,1.15f),
			CCRotateTo::create(0.05f,angle),
			CCSpawn::create(
			CCRotateTo::create(0.1f,0.0f),
			CCScaleTo::create(0.1f,1.0f),
			NULL
			),
			NULL
			);
		
	}else
	{
		action = CCSequence::create(
			CCScaleTo::create(0.2f,1.15f),
			CCRotateTo::create(0.05f,angle),
			CCSpawn::create(
			CCRotateTo::create(0.1f,0.0f),
			CCScaleTo::create(0.1f,1.0f),
			NULL
			),
			NULL
			);
	}

	fightor->runAction(action);
}

CCActionInterval* ClientSkillProcessorBase::createAttackMovementAction(ClientChessBoardItemBase* fightor,ClientChessBoardItemBase* target,float delay,float& time_cost,MSG_Skill* callSkillHitData)
{
	CCActionInterval* action = NULL;
	// 攻击动画
	{
		float delay_before_move = delay;
		float move_time = 0;

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
		CCPoint vec = LF::getVectorFromAToB(fightor,target);
		CCPoint moveByPoint = ccpSub(endPoint,statPoint);
		moveByPoint = ccpSub(moveByPoint,ccp(target->getContentSize().width/2 * vec.x,target->getContentSize().height/2 * vec.y));
		float moveTime = ccpDistance(moveByPoint,CCPointZero)/380.0f * 0.2f;
		moveTime = MAX(moveTime,0.08f);
		action =  CCSequence::create(
			CCDelayTime::create(delay_before_move),
			CCEaseIn::create(CCMoveTo::create(moveTime*0.5f,ccpAdd(fightor->getPosition(),ccpMult(moveByPoint,1.0f))),2.0f),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),callSkillHitData),
			CCEaseOut::create(CCMoveTo::create(moveTime,fightor->getPosition()),2.0f),
			CCDelayTime::create(0.1f),
			NULL
			);
		//fightor->runAction(action);
		time_cost = moveTime;
	}

	return action;
}

void ClientSkillProcessorBase::skill_fight(StepSkill step)
{
	CCLOG("skill type = %d", step.skillType);

	// 先归位
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor != NULL,"");
	fightor->setPosition(fightor->getNormalPos());


	switch (step.skillType)
	{
	// 天赋技能
	case SkillType_BaoLieZhan:
		skill_fight_prepare(step);
		//skill_fight_baoLieZhan(step);
		break;
	case SkillType_ShaLu:
		skill_fight_prepare(step);
		//skill_fight_shaLu(step);
		break;
	case SkillType_ShanDian:
		skill_fight_prepare(step);
		//skill_fight_shanDian(step);
		break;
	case SkillType_JianShou:
		skill_fight_prepare(step);
		//skill_fight_shengCai(step);
		//skill_fight_jianShou(step);
		break;
	case SkillType_ShenZhiXingNian:
		//skill_fight_shengZhiXinNian(step);
		skill_fight_prepare(step);
		break;
	case SkillType_ShunYinTuXi:
		skill_fight_shunYinTuXi(step);
		break;
	case SkillType_LieGong:
		skill_fight_prepare(step);
		//skill_fight_lieGong(step);
		break;
	case SkillType_JianYu:
		skill_fight_prepare(step);
		//skill_fight_jianYu(step);
		break;
	case SkillType_HuoQiuShu:
		skill_fight_prepare(step);
		//skill_fight_huoQiuShu(step);
		break;
	case SkillType_WenYi:
		skill_fight_prepare(step);
		//skill_fight_wenYi(step);
		break;


		// 技能书1
	case SkillType_BanYueZhan:
		skill_fight_BanYueZhan(step);
		break;
	case SkillType_CiSha:
		skill_fight_CiSha(step);
		break;
	case SkillType_LeiBao:
		skill_fight_LeiBao(step);
		break;
	case SkillType_MangCi:
		skill_fight_MangCi(step);
		break;
	case SkillType_ShenZhiZhuFu:
		skill_fight_ShenZhiZhuFu(step);
		break;
	case SkillType_LiRenHuiXuan:
		skill_fight_prepare(step);
		//skill_fight_LiRenHuiXuan(step);
		break;
	case SkillType_JingZhunZhiCan:
		skill_fight_JingZhunZhiCan(step);
		break;
	case SkillType_JiSuDuJian:
		skill_fight_JiSuDuJian(step);
		break;
	case SkillType_BaoFengXue:
		skill_fight_BaoFengXue(step);
		break;
	case SkillType_KongJu:
		skill_fight_KongJu(step);
		break;

		// 技能书2
	case SkillType_KuangBao:
		skill_fight_KuangBao(step);
		break;
	case SkillType_ShiXueZhiRen:
		skill_fight_ShiXueZhiRen(step);
		break;
	case SkillType_YunShi:
		skill_fight_YunShi(step);
		break;
	case SkillType_ShouHu:
		skill_fight_ShouHu(step);
		break;
	case SkillType_ZhongShenZhiLi:
		skill_fight_ZhongShenZhiLi(step);
		break;
	case SkillType_XuanFengZhan:
		skill_fight_XuanFengZhan(step);
		break;
	case SkillType_ZhuiHunDuJian:
		skill_fight_ZhuiHunDuJian(step);
		break;
	case SkillType_SiWangSaoShe:
		skill_fight_SiWangSaoShe(step);
		break;
	case SkillType_HanBingCiGu:
		skill_fight_HanBingCiGu(step);
		break;
	case SkillType_ShiHun:
		skill_fight_ShiHun(step);
		break;

		// 技能书3
	case SkillType_BaoNu:
		skill_fight_BaoNu(step);
		break;
	case SkillType_YinZheHun:
		skill_fight_YinZheHun(step);
		break;
	case SkillType_TianTangZhiGuang:
		skill_fight_TianTangZhiGuang(step);
		break;

		//小怪技能
	case SkillType_Monster_XieE:
		skill_fight_Monster_XieE(step);
		break;
		//BOSS技能
	case SkillType_BOSS_Sweep:
		skill_fight_prepare(step);
		//skill_fight_Monster_XieE(step);
		break;
	case SkillType_BOSS_Flog:
		skill_fight_prepare(step);
		//skill_fight_Monster_XieE(step);
		break;
	default:
		CCLog("Error:%s line:%d skill type is not surported!",__FUNCTION__,__LINE__);
		CCAssert(false,"没有实现");
		break;
	}

}

//void ClientSkillProcessorBase::skill_fight_baoLieZhan(StepSkill step)
//{
//	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
//
//	// 前面这一部分是做检验
//	// 每个的实现都一样
//	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
//	CCAssert(fightor!=NULL,"fightor必须存在");
//	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
//	CCAssert(target!=NULL,"target必须存在");
//	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
//	CCAssert(step.firedBuff.size() == 0,"不支持buff");
//
//	Music::sharedMusicPlayer()->playEffectSound("baoLieZhan_1.wav");
//
//	MSG_Skill* msg_data = new MSG_Skill();
//	msg_data->step = step;
//	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
//	msg_data->playDefaultHithedSound = false;
//	msg_data->hited_sound = "baoLieZhan_2.wav";
//
//
//	float move_time = 0;
//	float delay_before_move = 0.4f;
//	// 攻击动画
//	{
//		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
//		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
//		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
//		CCPoint vec = LF::getVectorFromAToB(fightor,target);
//		CCPoint moveByPoint = ccpSub(endPoint,statPoint);
//		moveByPoint = ccpSub(moveByPoint,ccp(target->getContentSize().width/2 * vec.x,target->getContentSize().height/2 * vec.y));
//		float moveTime = ccpDistance(moveByPoint,CCPointZero)/380.0f * 0.2f;
//		moveTime = MAX(moveTime,0.08f);
//		CCAction* action =  CCSequence::create(
//			CCDelayTime::create(delay_before_move),
//			CCEaseIn::create(CCMoveTo::create(moveTime*0.5f,ccpAdd(fightor->getPosition(),ccpMult(moveByPoint,1.0f))),2.0f),
//			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
//			CCEaseOut::create(CCMoveTo::create(moveTime,fightor->getPosition()),2.0f),
//			CCDelayTime::create(0.1f),
//			NULL
//			);
//		fightor->runAction(action);
//
//		move_time = moveTime;
//	}
//
//	// 攻击效果
//	{
//		// 技能动画
//		// 得到A到B的向量，如果y<0则B在下面
//		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
//
//		SkillSprite* fightEff = SkillSprite::create("jineng_yemanren_baoliezhan_qianyao_1.png");
//		fightor->addChild(fightEff,10);
//		fightEff->setPosition(ccpAdd(fightor->getSkillFirePos(),ccp(0,-40)));
//
//		CCParticleSystemQuad* bomb = CCParticleSystemQuad::create("baoliezhanbaozhan.plist");
//		bomb->setAutoRemoveOnFinish(true);
//		target->addChild(bomb);
//		bomb->setPosition(target->getSkillFirePos());
//		bomb->setVisible(false);
//		bomb->setScale(0.8f);
//
//		SkillSprite* zhuaHeng = SkillSprite::create("jineng_yemanren_baoliezhan_1.png");
//		target->addChild(zhuaHeng,1);
//		zhuaHeng->setPosition(target->getSkillFirePos());
//
//
//		CCFiniteTimeAction* fightAction = SpriteHelper::easyCreateAction_pList("jineng_yemanren_baoliezhan_qianyao_%d.png",1,9,0.035f,false);
//		fightEff->runAction(CCSequence::create(
//			fightAction,
//			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
//			NULL
//			));
//
//		bomb->runAction(CCSequence::create(
//			//CCDelayTime::create(move_time + 0.2f+delay_before_move),
//			CCDelayTime::create(move_time+delay_before_move),
//			CCShow::create(),
//			NULL
//			));
//
//		CCFiniteTimeAction* zhuaHengAction = SpriteHelper::easyCreateAction_pList("jineng_yemanren_baoliezhan_%d.png",1,2,0.005f,false);
//		zhuaHeng->setVisible(false);
//		zhuaHeng->runAction(CCSequence::create(
//			CCDelayTime::create(move_time+delay_before_move),
//			CCShow::create(),
//			zhuaHengAction,
//			CCDelayTime::create(0.2f),
//			CCCallFuncN::create(zhuaHeng->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
//			NULL
//			));
//	}
//}


void ClientSkillProcessorBase::skill_fight_baoLieZhan(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	1

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif

	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
	CCAssert(step.firedBuff.size() == 0,"不支持buff");
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");

	Music::sharedMusicPlayer()->playEffectSound("baoLieZhan_1.wav");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;
	msg_data->hited_sound = "baoLieZhan_2.wav";

	//CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.5f);

	// 攻击效果
	{
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();

		CCPoint vec = LF::getVectorFromAToB(target,fightor);
	
		const float frame_time = 0.08f;
		SPRITE* fightEff = SPRITE::create("jineng_yemanren_baolie_shifa_1.png");
		fightor->addChild(fightEff,10);
		fightEff->setPosition(ccpAdd(fightor->getSkillFirePos(),ccp(0,-40)));
		CCFiniteTimeAction* fightAction = EASY_ACTION("jineng_yemanren_baolie_shifa_%d.png",1,5,frame_time,false);
		fightEff->runAction(CCSequence::create(
			fightAction,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));

		float move_time = 0;
		float delay_before_move = fightAction->getDuration();
		// 跑过去
		CCActionInterval* action = createAttackMovementAction(fightor,target,delay_before_move,move_time,msg_data);
		fightor->runAction(action);

		CCPoint offset = ccp(0,41*2);
		SPRITE* bomb = SPRITE::create("jineng_yemanren_baolie_shoushang_1.png");
		target->addChild(bomb,vec.y > 0?2:1); // 上面打下面的情况要盖住刀光
		bomb->setPosition(ccpAdd(target->getSkillFirePos(),offset));
		CCFiniteTimeAction* zhuaHengAction = EASY_ACTION("jineng_yemanren_baolie_shoushang_%d.png",1,9,frame_time,false);
		bomb->setVisible(false);
		bomb->runAction(CCSequence::create(
			CCDelayTime::create(move_time+delay_before_move),
			CCShow::create(),
			zhuaHengAction,
			CCDelayTime::create(0.2f),
			CCCallFuncN::create(bomb->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));


		SPRITE* daoguang = SPRITE::create("jineng_yemanren_baolie_feixing_1.png");
		target->addChild(daoguang,1);
		daoguang->setPosition(bomb->getPosition());
		daoguang->setPositionY(daoguang->getPositionY() - 80 - 60 + (vec.y>0?60:0) ); // 上面打下面的时候要把刀光上移一点
		CCFiniteTimeAction* daoguangAction = EASY_ACTION("jineng_yemanren_baolie_feixing_%d.png",1,4,frame_time,false);
		daoguang->setVisible(false);
		daoguang->runAction(CCSequence::create(
			CCDelayTime::create(move_time+delay_before_move),
			CCShow::create(),
			daoguangAction,
			CCDelayTime::create(0.2f),
			CCCallFuncN::create(daoguang->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));

		if (vec.y > 0)
		{
			daoguang->setFlipX(true);
			daoguang->setFlipY(true);
		}
	}
}






void ClientSkillProcessorBase::skill_fight_shaLu(StepSkill step)
{
#undef USE_PLIST
#define USE_PLIST	1
	
#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif

	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
	CCAssert(step.firedBuff.size() == 0,"不支持buff");

	Music::sharedMusicPlayer()->playEffectSound("shalu_1.wav");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;
	msg_data->hited_sound = "shalu_2.wav";


	float move_time = 0;
	float delay_before_move = 0.6f;
	// 攻击动画
	{
		fightor->runAction(createAttackMovementAction(fightor,target,delay_before_move,move_time,msg_data));
	}
	// 攻击效果
	{
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();

		SPRITE* fightEff = SPRITE::create("jineng_cike_shalu_shifa_1.png");
		fightor->addChild(fightEff,10);
		fightEff->setPosition(ccpAdd(fightor->getSkillFirePos(),ccp(0,30)));


		SPRITE* zhuaHeng = SPRITE::create("jineng_cike_shalu_shoushang_1.png");
		target->addChild(zhuaHeng,1);
		zhuaHeng->setPosition(target->getSkillFirePos());


		CCFiniteTimeAction* fightAction = EASY_ACTION("jineng_cike_shalu_shifa_%d.png",1,13,0.06f,false);
		fightEff->runAction(CCSequence::create(
			fightAction,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));

		CCFiniteTimeAction* zhuaHengAction = EASY_ACTION("jineng_cike_shalu_shoushang_%d.png",1,23,0.04f,false);
		zhuaHeng->setVisible(false);
		zhuaHeng->runAction(CCSequence::create(
			CCDelayTime::create(move_time+delay_before_move),
			CCShow::create(),
			zhuaHengAction,
			CCDelayTime::create(0.2f),
			CCCallFuncN::create(zhuaHeng->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
	}




// 2014-04-16 日注释
////控制用plist还是图片集合 
//#undef USE_PLIST
//#define USE_PLIST	0
//
//#if USE_PLIST == 1
//	typedef SkillSprite SPRITE;
//#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
//	// 载入
//	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
//#else
//	typedef BaseSprite	SPRITE;
//#define EASY_ACTION	SpriteHelper::easyCreateAction
//#endif
//
//	// 前面这一部分是做检验
//	// 每个的实现都一样
//	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
//	CCAssert(fightor!=NULL,"fightor必须存在");
//	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
//	CCAssert(target!=NULL,"target必须存在");
//	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
//	CCAssert(step.firedBuff.size() == 0,"不支持buff");
//
//	Music::sharedMusicPlayer()->playEffectSound("shalu_1.wav");
//
//	MSG_Skill* msg_data = new MSG_Skill();
//	msg_data->step = step;
//	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
//	msg_data->playDefaultHithedSound = false;
//	msg_data->hited_sound = "shalu_2.wav";
//
//
//	float move_time = 0;
//	float delay_before_move = 0.0f;
//	// 攻击动画
//	{
//		//fightor->runAction(createAttackMovementAction(fightor,target,delay_before_move,move_time,msg_data));
//
//	}
//	// 攻击效果
//	{
//		const float frame_gap = 0.1f;
//		// 技能动画
//		// 得到A到B的向量，如果y<0则B在下面
//		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
//
//		// 技能动画
//		// 得到A到B的向量，如果y<0则B在下面
//		CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),target->getFireNode());
//		float angle = LF::getRotateAngle(aToB,ccp(0,1));
//
//
//		SPRITE* fightEff = SPRITE::create("jineng_cike_shalu_shifa_1.png");
//		fightor->addChild(fightEff,10);
//		fightEff->setPosition(ccpAdd(fightor->getSkillFirePos(),ccp(0,-40)));
//		fightEff->setVisible(false);
//		CCFiniteTimeAction* fightAction = EASY_ACTION("jineng_cike_shalu_shifa_%d.png",1,8,frame_gap,false);
//		fightEff->runAction(CCSequence::create(
//			CCDelayTime::create(move_time+delay_before_move),
//			CCShow::create(),
//			fightAction,
//			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
//			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
//			NULL
//			));
//
//		SPRITE* daoguang = SPRITE::create("jineng_cike_shalu_feixing_1.png");
//		target->addChild(daoguang,1);
//		daoguang->setPosition(target->getSkillFirePos());
//		daoguang->setRotation(angle);
//		daoguang->setVisible(false);
//		CCFiniteTimeAction* daoguangAction = EASY_ACTION("jineng_cike_shalu_feixing_%d.png",1,3,frame_gap,false);
//		daoguang->runAction(CCSequence::create(
//			CCDelayTime::create(move_time+delay_before_move+6*frame_gap),
//			CCShow::create(),
//			daoguangAction,
//			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
//			NULL
//			));
//
//		fightor->runAction(CCSequence::create(
//			CCDelayTime::create(move_time+delay_before_move+6*frame_gap),
//			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
//			CCRotateTo::create(0.0f,0),
//			CCScaleTo::create(0.1f,1.15f),
//			CCScaleTo::create(0.01f,1.0f),
//			NULL
//			));
//
//
//		SPRITE* shoushang = SPRITE::create("jineng_cike_shalu_shoushang_1.png");
//		target->addChild(shoushang,1);
//		shoushang->setPosition(target->getSkillFirePos());
//		CCFiniteTimeAction* zhuaHengAction = EASY_ACTION("jineng_cike_shalu_shoushang_%d.png",1,5,frame_gap,false);
//		shoushang->setVisible(false);
//		shoushang->runAction(CCSequence::create(
//			CCDelayTime::create(move_time+delay_before_move+7*frame_gap),
//			CCShow::create(),
//			zhuaHengAction,
//			//CCDelayTime::create(0.2f),
//			CCCallFuncN::create(shoushang->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
//			NULL
//			));
//	}


	//2014-04-15日修改之前的代码
	//// 前面这一部分是做检验
	//// 每个的实现都一样
	//// 载入
	//loadPlist(SkillTree::getSkillPlistFile(step.skillType));
	//ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	//CCAssert(fightor!=NULL,"fightor必须存在");
	//ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	//CCAssert(target!=NULL,"target必须存在");
	//CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
	//CCAssert(step.firedBuff.size() == 0,"不支持buff");

	//Music::sharedMusicPlayer()->playEffectSound("baoLieZhan_1.wav");

	//MSG_Skill* msg_data = new MSG_Skill();
	//msg_data->step = step;
	//msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	//msg_data->playDefaultHithedSound = false;
	//msg_data->hited_sound = "baoLieZhan_2.wav";


	//float move_time = 0;
	//float delay_before_move = 0.4f;
	//// 攻击动画
	//{
	//	fightor->runAction(createAttackMovementAction(fightor,target,delay_before_move,move_time,msg_data));
	//}
	//// 攻击效果
	//{
	//	// 技能动画
	//	// 得到A到B的向量，如果y<0则B在下面
	//	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();

	//	SkillSprite* fightEff = SkillSprite::create("jineng_hunter_shalu_qianyao_1.png");
	//	fightor->addChild(fightEff,10);
	//	fightEff->setPosition(ccpAdd(fightor->getSkillFirePos(),ccp(0,-40)));


	//	SkillSprite* zhuaHeng = SkillSprite::create("jineng_hunter_shalu_1.png");
	//	target->addChild(zhuaHeng,1);
	//	zhuaHeng->setPosition(target->getSkillFirePos());


	//	CCFiniteTimeAction* fightAction = SpriteHelper::easyCreateAction_pList("jineng_hunter_shalu_qianyao_%d.png",1,16,0.045f,false);
	//	fightEff->runAction(CCSequence::create(
	//		fightAction,
	//		CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
	//		NULL
	//		));

	//	CCFiniteTimeAction* zhuaHengAction = SpriteHelper::easyCreateAction_pList("jineng_hunter_shalu_%d.png",1,16,0.02f,false);
	//	zhuaHeng->setVisible(false);
	//	zhuaHeng->runAction(CCSequence::create(
	//		CCDelayTime::create(move_time+delay_before_move),
	//		CCShow::create(),
	//		zhuaHengAction,
	//		CCDelayTime::create(0.2f),
	//		CCCallFuncN::create(zhuaHeng->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
	//		NULL
	//		));
	//}
}
void ClientSkillProcessorBase::skill_fight_shanDian(StepSkill step)
{
		// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	1

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif

	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
	CCAssert(step.firedBuff.size() == 0,"不支持buff");


	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;

	// 攻击者的动画,需要跟技能动画协调时间
	{
		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
	}

	{
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		SPRITE* shi_fa = SPRITE::create("jineng_jisi_shandian_shifa_1.png");
		fightor->addChild(shi_fa,10);
		shi_fa->setPosition(ccpAdd(fightor->getNormalPos(),ccp(0,80)));

		//
		Music::sharedMusicPlayer()->playEffectSound("shandian_1.wav");

		const float frameTime = 0.03f;
		CCFiniteTimeAction* shi_fa_action = EASY_ACTION("jineng_jisi_shandian_shifa_%d.png",1,23,frameTime,false);
		shi_fa->runAction(CCSequence::create(
			shi_fa_action,
			CCDelayTime::create(frameTime),
			CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));

		{
			SPRITE* shan_dian = SPRITE::create("jineng_jisi_shandian_shoushang_1.png");
			target->addChild(shan_dian,10);
			const float shoushang_delay = shi_fa_action->getDuration() - frameTime;

			CCPoint pos = ccpAdd(
				ccp(target->getContentSize().width/2,target->getContentSize().height/2),
				ccp(0,(shan_dian->getContentSize().height-target->getContentSize().height)/2));
			shan_dian->setPosition(ccpAdd(pos,ccp(15,140)));
			CCFiniteTimeAction* shan_dian_action = EASY_ACTION("jineng_jisi_shandian_shoushang_%d.png",1,18,frameTime,false);
			shan_dian->setVisible(false);
			shan_dian->runAction(CCSequence::create(
				CCDelayTime::create(shoushang_delay),
				CCShow::create(),
				LFDelayToMusic::create(0,"shandian_2.wav",true),
				shan_dian_action,
				CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				));

			CCNode* hit = CCNode::create();
			hit->runAction(CCSequence::create(
				CCDelayTime::create(shoushang_delay + frameTime * 4),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				));
			target->addChild(hit);
			

			SPRITE* shan_dian_end = SPRITE::create("jineng_jisi_leibao_shoushang_jieshu_1.png");
			target->addChild(shan_dian_end,10);
			shan_dian_end->setPosition(ccpAdd(target->getNormalPos(),ccp(15,50)));
			CCFiniteTimeAction* jieSu_action = EASY_ACTION("jineng_jisi_leibao_shoushang_jieshu_%d.png",1,14,frameTime,false);
			shan_dian_end->setVisible(false);
			shan_dian_end->runAction(CCSequence::create(
				CCDelayTime::create(shoushang_delay),
				CCDelayTime::create(shan_dian_action->getDuration()),
				CCShow::create(),
				jieSu_action,
				CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				));
			target->setOpacity(255);
			target->runAction(CCSequence::create(
				CCDelayTime::create(shoushang_delay),
				CCDelayTime::create(shan_dian_action->getDuration()),
				LFDelayToMusic::create(0,"shandian_3.wav",true),
				CCRepeat::create(
				CCSequence::create(
				CCFadeTo::create(0.03f,100),
				CCFadeTo::create(0.03f,255),
				NULL),
				11),
				CCFadeTo::create(0.01f,255),
				NULL));
		}
	}






// 2014-04-16版本
//	// 控制用plist还是图片集合
//#undef USE_PLIST
//#define USE_PLIST	0
//
//#if USE_PLIST == 1
//	typedef SkillSprite SPRITE;
//#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
//	// 载入
//	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
//#else
//	typedef BaseSprite	SPRITE;
//#define EASY_ACTION	SpriteHelper::easyCreateAction
//#endif
//
//	// 前面这一部分是做检验
//	// 每个的实现都一样
//	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
//	CCAssert(fightor!=NULL,"fightor必须存在");
//	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
//	CCAssert(target!=NULL,"target必须存在");
//	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
//	CCAssert(step.firedBuff.size() == 0,"不支持buff");
//
//
//	MSG_Skill* msg_data = new MSG_Skill();
//	msg_data->step = step;
//	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
//	msg_data->playDefaultHithedSound = false;
//
//	// 攻击者的动画,需要跟技能动画协调时间
//	{
//		fightor->runAction(CCSequence::create(
//			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
//			CCRotateTo::create(0.0f,0),
//			CCScaleTo::create(0.1f,1.15f),
//			CCScaleTo::create(0.01f,1.0f),
//			NULL
//			));
//	}
//
//	{
//		// 技能动画
//		// 得到A到B的向量，如果y<0则B在下面
//		SPRITE* shi_fa = SPRITE::create("jineng_fashi_shandian_shifa_1.png");
//		fightor->addChild(shi_fa,10);
//		shi_fa->setPosition(ccpAdd(fightor->getNormalPos(),ccp(0,80)));
//
//		//
//		Music::sharedMusicPlayer()->playEffectSound("shandian_1.wav");
//
//		const float frameTime = 0.1f;
//		CCFiniteTimeAction* shi_fa_action = EASY_ACTION("jineng_fashi_shandian_shifa_%d.png",1,8,frameTime,false);
//		shi_fa->runAction(CCSequence::create(
//			shi_fa_action,
//			CCDelayTime::create(frameTime),
//			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
//			CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
//			NULL
//			));
//
//		for (int i=0;i<1;i++)
//		{
//			SPRITE* shan_dian = SPRITE::create("jineng_fashi_shandian_shoushang_1.png");
//			target->addChild(shan_dian,10);
//			const float delay_arr[3] = {0};
//			const float shoushang_delay = frameTime * 7;
//
//			CCPoint pos = ccpAdd(
//				ccp(target->getContentSize().width/2,target->getContentSize().height/2),
//				ccp(0,(shan_dian->getContentSize().height-target->getContentSize().height)/2));
//			shan_dian->setPosition(ccpAdd(pos,ccp(15,-44-100)));
//			CCFiniteTimeAction* shan_dian_action = EASY_ACTION("jineng_fashi_shandian_shoushang_%d.png",1,13,frameTime,false);
//			shan_dian->setVisible(false);
//			shan_dian->runAction(CCSequence::create(
//				CCDelayTime::create(shoushang_delay),
//				CCDelayTime::create(delay_arr[i]),
//				CCShow::create(),
//				LFDelayToMusic::create(0,"shandian_2.wav",true),
//				shan_dian_action,
//				CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
//				NULL
//				));
//			/*SPRITE* shan_dian_end = SPRITE::create("jineng_jisi_leibao_shoushang_jieshu_1.png");
//			target->addChild(shan_dian_end,10);
//			shan_dian_end->setPosition(ccpAdd(target->getNormalPos(),ccp(15,50)));
//			CCFiniteTimeAction* jieSu_action = EASY_ACTION("jineng_jisi_leibao_shoushang_jieshu_%d.png",1,14,0.03f,false);
//			shan_dian_end->setVisible(false);
//			shan_dian_end->runAction(CCSequence::create(
//				CCDelayTime::create(shoushang_delay),
//				CCDelayTime::create(delay_arr[i] + shan_dian_action->getDuration()),
//				CCShow::create(),
//				jieSu_action,
//				CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
//				NULL
//				));
//			target->setOpacity(255);
//			target->runAction(CCSequence::create(
//				CCDelayTime::create(shoushang_delay),
//				CCDelayTime::create(shan_dian_action->getDuration()),
//				CCDelayTime::create(delay_arr[i]),
//				LFDelayToMusic::create(0,"shandian_3.wav",true),
//				CCRepeat::create(
//				CCSequence::create(
//				CCFadeTo::create(0.03f,100),
//				CCFadeTo::create(0.03f,255),
//				NULL),
//				11),
//				CCFadeTo::create(0.01f,255),
//				NULL));*/
//		}
//	}
}

void ClientSkillProcessorBase::skill_fight_shengCai(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	1

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif

	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}


	const float frame_time = 0.07f;
	float shifa_need_time = 0;
	// 攻击者的动画
	{
		SPRITE* fire = SPRITE::create("jineng_qishi_shengcai_shifa_1.png");
		CCFiniteTimeAction* fire_action = EASY_ACTION("jineng_qishi_shengcai_shifa_%d.png",1,12,frame_time,false);
		fire->runAction(CCSequence::create(
			LFDelayToMusic::create(0.0f,"shengcai_1.wav"),
			fire_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
		fire->setPosition(ccp(fightor->getContentSize().width/2,fightor->getContentSize().height/2));
		fightor->getContainer()->addChild(fire,3);
		fire->setPosition(ccpAdd(fightor->getSkillFirePos(),ccp(8*2,-8*2)));

		shifa_need_time = 12*frame_time;
		fightor->runAction(CCSequence::create(
			CCDelayTime::create(shifa_need_time),
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
	}

	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	CCPoint targetPosVecSum = CCPointZero;
	for (int i=0;i<targetNum;i++)
	{
		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;

		targetPosVecSum = ccpAdd(targetPosVecSum,LF::getNativePostion(targetVec[i]->getFireNode(),layer));
		// 受伤
		{
			SPRITE* fangYu = SPRITE::create("jineng_qishi_shengcai_shoushang_1.png");
			targetVec[i]->getContainer()->addChild(fangYu,targetVec[i]->getZOrder()+4);
			fangYu->setPosition(ccpAdd(targetVec[i]->getSkillFirePos(),ccp(0,0)));
			fangYu->setVisible(false);

			CCFiniteTimeAction* dun_action = EASY_ACTION("jineng_qishi_shengcai_shoushang_%d.png",1,6,frame_time,false);

			fangYu->runAction(
				CCSequence::create(
				CCDelayTime::create(shifa_need_time),
				CCShow::create(),
				dun_action,
				CCHide::create(),
				NULL
				)
				);

			// 一定要在skill_Hit之后移除
			// 而且只能是当前的这个动作，不能根据时间来，会有bug
			// 比如一卡顿，其中的一个动作有移除Node，那么动画就没了，skill_Hit就没调用
			fangYu->runAction(CCSequence::create(
				CCDelayTime::create(shifa_need_time),//0.9f
				LFDelayToMusic::create(0.0f,"shengcai_2.wav"),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCDelayTime::create(dun_action->getDuration()),
				CCCallFuncN::create(targetVec[i]->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				));
		}
	}

	// 回旋
	{
		SPRITE* fangYu = SPRITE::create("jineng_qishi_shengcai_feixing_1.png");
		layer->addChild(fangYu,3);
		CCPoint pos = ccpMult(ccp(0,targetPosVecSum.y),1.0f/(float)targetNum);
		pos.x = mVisibleSize.width/2;
		fangYu->setPosition(ccpAdd(pos,ccp(0,-fangYu->getContentSize().height/2)));
		fangYu->setVisible(false);

		CCPoint vec = LF::getVectorFromAToB(fangYu,fightor);
		if (vec.y > 0)
		{
			fangYu->setFlipX(true);
			fangYu->setFlipY(true);
			fangYu->setPositionY(fangYu->getPositionY() + fangYu->getContentSize().height/2);
		}

		CCFiniteTimeAction* dun_action = EASY_ACTION("jineng_qishi_shengcai_feixing_%d.png",1,4,frame_time,false);

		fangYu->runAction(
			CCSequence::create(
			CCDelayTime::create(shifa_need_time),//0.9f
			CCShow::create(),
			dun_action,
			CCHide::create(),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			)
			);
	}
}

void ClientSkillProcessorBase::skill_fight_jianShou(StepSkill step)
{
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));

	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}
	// 该技能只有Buff
	CCAssert(step.firedBuff.size() > 0,"buff必须要有");

	// 攻击者的动画
	{
		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
		SkillSprite* fire = SkillSprite::create("jineng_qishi_shenzhixinnian1.png");
		//loadPlist("jineng_qishi_shenzhixinnian.plist");
		CCFiniteTimeAction* fire_action = SpriteHelper::easyCreateAction_pList("jineng_qishi_shenzhixinnian%d.png",1,21,0.04f,false);
		fire->runAction(CCSequence::create(
			fire_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		fire->setPosition(ccp(fightor->getContentSize().width/2,fightor->getContentSize().height/2));
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(fire,10);
		fire->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
	}



	for (int i=0;i<targetNum;i++)
	{
		Music::sharedMusicPlayer()->playEffectSound("jiaHuJia_1.wav");

		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;
		msg_data->hited_sound = "jiaHuJia_2.wav";


		// 防御的顿
		{
			SkillSprite* fangYu = SkillSprite::create("jineng_qishi_shenzhixinnian_dun1.png");
			targetVec[i]->getContainer()->addChild(fangYu,targetVec[i]->getZOrder()+4);
			//fangYu->setPosition(ccpAdd(targetVec[i]->getPosition(),ccp(0,-5)));
			fangYu->setPosition(ccpAdd(targetVec[i]->getSkillFirePos(),ccp(0,-5)));
			fangYu->setVisible(false);

			CCFiniteTimeAction* dun_action = SpriteHelper::easyCreateAction_pList("jineng_qishi_shenzhixinnian_dun%d.png",1,12,0.05f,false);

			fangYu->runAction(
				CCSequence::create(
				CCDelayTime::create(0.8f),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCShow::create(),
				dun_action,
				CCCallFuncN::create(targetVec[i]->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
				NULL
				)
				);
		}
	}
}
void ClientSkillProcessorBase::skill_fight_shengZhiXinNian(StepSkill step)
{
	step.skillType = SkillType_ShenZhiZhuFu;
	skill_fight_ShenZhiZhuFu(step);
	return;


	//loadPlist("jineng_magebane_shenzhixinnian.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));


	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}
	// 该技能只有Buff
	CCAssert(step.firedBuff.size() > 0,"buff必须要有");

	// 攻击者的动画
	{
		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
		SkillSprite* fire = SkillSprite::create("jineng_magebane_shenzhixinnian_qianyao_1.png");
		CCFiniteTimeAction* fire_action = SpriteHelper::easyCreateAction_pList("jineng_magebane_shenzhixinnian_qianyao_%d.png",1,23,0.03f,false);
		fire->runAction(CCSequence::create(
			fire_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		fire->setPosition(ccp(fightor->getContentSize().width/2,fightor->getContentSize().height/2));
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(fire,10);
		fire->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));

	}



	for (int i=0;i<targetNum;i++)
	{
		Music::sharedMusicPlayer()->playEffectSound("jiaHuJia_1.wav");

		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;
		msg_data->hited_sound = "jiaHuJia_2.wav";


		// 防御的万字符
		{
			SkillSprite* fangYu = SkillSprite::create("jineng_magebane_shenzhixinnian_zhishi_1.png");
			targetVec[i]->getContainer()->addChild(fangYu,targetVec[i]->getZOrder()+4);
			fangYu->setPosition(ccpAdd(targetVec[i]->getSkillFirePos(),ccp(0,-5)));
			fangYu->setVisible(false);

			CCFiniteTimeAction* dun_action = SpriteHelper::easyCreateAction_pList("jineng_magebane_shenzhixinnian_zhishi_%d.png",1,12,0.06f,false);

			fangYu->runAction(
				CCSequence::create(
				CCDelayTime::create(0.6f+CCRANDOM_0_1()*0.3f),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCShow::create(),
				dun_action,
				CCCallFuncN::create(targetVec[i]->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
				NULL
				)
				);
		}
	}
}

void ClientSkillProcessorBase::skill_fight_shunYinTuXi(StepSkill step)
{
	//loadPlist("jineng_youxia_shunyingtuxi.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));


	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
	CCAssert(step.firedBuff.size() == 0,"不支持buff");

	Music::sharedMusicPlayer()->playEffectSound("shunyintuxi_01.wav");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;
	msg_data->hited_sound = "baoLieZhan_2.wav";


	{
		// 技能动画
		SkillSprite* yinXing = SkillSprite::create("jineng_youxia_shunyingtuxi_shifa_1.png");
		fightor->addChild(yinXing,10);
		yinXing->setPosition(ccpAdd(fightor->getNormalPos(),ccp(0,70)));

		SkillSprite* zhuahen = SkillSprite::create("jineng_youxia_shunyingtuxi_shoushang_1.png");
		target->addChild(zhuahen,10);
		zhuahen->setPosition(target->getNormalPos());
		zhuahen->setVisible(false);

		float frame_time = 0.02f;

		CCFiniteTimeAction* hideAndShow = SpriteHelper::easyCreateAction_pList("jineng_youxia_shunyingtuxi_shifa_%d.png",1,38,frame_time,false);
		yinXing->runAction(CCSequence::create(
			LFDelayToMusic::create(0.0f,"shunyintuxi_01.wav"),
			hideAndShow,
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			CCCallFuncN::create(fightor,SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));

		
		CCFiniteTimeAction* zhuahenAction = SpriteHelper::easyCreateAction_pList("jineng_youxia_shunyingtuxi_shoushang_%d.png",1,11,0.02f,false);
		zhuahen->runAction(CCSequence::create(
			CCDelayTime::create(hideAndShow->getDuration()),
			CCShow::create(),
			(CCFiniteTimeAction*)zhuahenAction->copy()->autorelease(),
			CCCallFuncN::create(target,SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));

		vector<CCSprite*> figterBodyParts = fightor->getItemParts(Part_Body);
		int partNum = figterBodyParts.size();
		for (int i=0;i<partNum;i++)
		{
			figterBodyParts[i]->runAction(CCSequence::create(
				CCFadeTo::create(frame_time*10,0),// 第9帧就看不到了
				CCDelayTime::create(hideAndShow->getDuration()+zhuahenAction->getDuration()),
				CCFadeTo::create(0.1f,255),
				NULL
				));
		}
	}
}
void ClientSkillProcessorBase::skill_fight_lieGong(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	1

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif
	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");

	Music::sharedMusicPlayer()->playEffectSound("huijinzhiyu_1.wav");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;
	msg_data->hited_sound = "huijinzhiyu_2.wav";

	const float frame_gap = 0.08f;
	{
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),target->getFireNode());
		float angle = LF::getRotateAngle(aToB,ccp(0,1));

		SPRITE* yun_qi = SPRITE::create();
		SPRITE* jian = SPRITE::create("jineng_gongjianshou_huijinzhiyu_feixing_1.png");
		jian->setVisible(false);

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
		layer->addChild(yun_qi,10);
		layer->addChild(jian,10);

		jian->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
		yun_qi->setPosition(ccpAdd(LF::getNativePostion(fightor->getFireNode(),layer),ccp(14,20)));
		jian->setRotation(angle);
		yun_qi->setRotation(angle);
		yun_qi->setScale(1.25f);

		const float action_delay = 0;
		// 运气
		CCFiniteTimeAction* yun_qi_action = EASY_ACTION("jineng_shenjianshou_huijinzhiyu_shifa_%d.png",1,13,frame_gap,false);
		yun_qi->setVisible(false);
		yun_qi->runAction(CCSequence::create(
			CCDelayTime::create(action_delay),
			CCShow::create(),
			yun_qi_action,
			CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));

		const float fly_delay = 12 * frame_gap + action_delay;

		// 攻击者的动画,需要跟技能动画协调时间
		{
			CCFiniteTimeAction* back = fightor->get_fire_shake_Action(fightor,target);
			CCAction* action =  CCSequence::create(
				CCDelayTime::create(fly_delay),
				back,
				NULL
				);
			fightor->runAction(action);
		}

		// 箭飞行
		float dis = ccpDistance(endPoint,statPoint);
		float time = dis/456.0f * 0.18f;
		CCPoint offset = ccp(jian->getContentSize().height/2,jian->getContentSize().height/2);
		CCAction* fly = CCSequence::create(
			CCDelayTime::create(fly_delay),
			CCShow::create(),
			CCSpawn::create(
			CCMoveTo::create(time,ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
			CCFadeTo::create(time,0),NULL),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			CCCallFuncN::create(target,SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			);
		jian->runAction(fly);


		CCFiniteTimeAction* bomb2Action = EASY_ACTION("jineng_gongjianshou_huijinzhiyu_shoushang_%d.png",1,8,frame_gap,false);
		SPRITE* bomb2 = SPRITE::create();
		bomb2->setVisible(false);
		bomb2->runAction(CCSequence::create(
			CCDelayTime::create(fly_delay+time),
			CCShow::create(),
			bomb2Action,
			CCCallFuncN::create(target,SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
		bomb2->setPosition(ccpAdd(LF::getNativePostion(target,layer),ccp(0,80)));
		layer->addChild(bomb2,10);
	}

	

	// 一个箭，烧团火的烈弓
	{
		//loadPlist(SkillTree::getSkillPlistFile(step.skillType));
		//// 前面这一部分是做检验
		//// 每个的实现都一样
		//ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
		//CCAssert(fightor!=NULL,"fightor必须存在");
		//ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
		//CCAssert(target!=NULL,"target必须存在");
		//CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
		//CCAssert(step.firedBuff.size() == 0,"不支持buff");

		//Music::sharedMusicPlayer()->playEffectSound("lieGong_1.wav");

		//MSG_Skill* msg_data = new MSG_Skill();
		//msg_data->step = step;
		//msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		//msg_data->playDefaultHithedSound = false;
		//msg_data->hited_sound = "lieGong_2.wav";


		//{
		//	// 技能动画
		//	// 得到A到B的向量，如果y<0则B在下面
		//	CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),target->getFireNode());
		//	float angle = LF::getRotateAngle(aToB,ccp(0,1));

		//	SkillSprite* yun_qi = SkillSprite::create();
		//	SkillSprite* jian = SkillSprite::create("jineng_shenjianshou_liegong_feixing.png");
		//	jian->setVisible(false);

		//	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		//	CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		//	CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
		//	layer->addChild(yun_qi,10);
		//	layer->addChild(jian,10);

		//	jian->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
		//	yun_qi->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
		//	jian->setRotation(angle);
		//	yun_qi->setRotation(angle);
		//	yun_qi->setScale(1.25f);

		//	float action_delay = 0;
		//	// 运气
		//	CCFiniteTimeAction* yun_qi_action = SpriteHelper::easyCreateAction_pList("jineng_shenjianshou_liegong_%d.png",1,33,0.04f,false);
		//	yun_qi->setVisible(false);
		//	yun_qi->runAction(CCSequence::create(
		//		CCDelayTime::create(action_delay),
		//		CCShow::create(),
		//		yun_qi_action,
		//		CCCallFuncN::create(fightor,SEL_CallFuncN(&SkillSprite::removeNode)),
		//		NULL
		//		));

		//	float fly_delay = yun_qi_action->getDuration()*(24.0f/33.0f) + action_delay;


		//	// 攻击者的动画,需要跟技能动画协调时间
		//	{
		//		CCFiniteTimeAction* back = fightor->get_fire_shake_Action(fightor,target);
		//		CCAction* action =  CCSequence::create(
		//			CCDelayTime::create(fly_delay),
		//			back,
		//			NULL
		//			);
		//		fightor->runAction(action);
		//	}

		//	// 箭飞行
		//	float dis = ccpDistance(endPoint,statPoint);
		//	float time = dis/456.0f * 0.18f;
		//	CCPoint offset = ccp(jian->getContentSize().height/2,jian->getContentSize().height/2);
		//	CCAction* fly = CCSequence::create(
		//		CCDelayTime::create(fly_delay),
		//		CCShow::create(),
		//		CCSpawn::create(
		//		CCMoveTo::create(time,ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
		//		CCFadeTo::create(time,0),NULL),
		//		CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
		//		CCCallFuncN::create(target,SEL_CallFuncN(&SkillSprite::removeNode)),
		//		NULL
		//		);
		//	jian->runAction(fly);


		//	CCFiniteTimeAction* bomb2Action = SpriteHelper::easyCreateAction_pList("jineng_shenjianshou_shoushang_2_%d.png",1,21,0.02f,false);
		//	SkillSprite* bomb2 = SkillSprite::create();
		//	bomb2->setVisible(false);
		//	bomb2->runAction(CCSequence::create(
		//		CCDelayTime::create(fly_delay+time),
		//		CCShow::create(),
		//		bomb2Action,
		//		CCCallFuncN::create(target,SEL_CallFuncN(&SkillSprite::removeNode)),
		//		NULL
		//		));
		//	bomb2->setPosition(ccpAdd(LF::getNativePostion(target,layer),ccp(0,30)));
		//	layer->addChild(bomb2,10);
		//}
	}
	
}
void ClientSkillProcessorBase::skill_fight_jianYu(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	1

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif

	//《箭雨》
	/*攻击距离2.5
	攻击所有在射程范围内的敌方单位。
	伤害值为普攻3倍。*/
	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}

	// 攻击者的动画
	{
		SPRITE* fire = SPRITE::create("jineng_huntsman_jianyu_qianyao_1.png");
		CCFiniteTimeAction* fire_action = EASY_ACTION("jineng_huntsman_jianyu_qianyao_%d.png",1,19,0.03f,false);
		fire->runAction(CCSequence::create(
			LFDelayToMusic::create(0.0f,"jianyu_01.wav"),
			fire_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
		fire->setPosition(ccp(fightor->getContentSize().width/2,fightor->getContentSize().height/2));
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(fire,10);
		fire->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));

		fightor->runAction(CCSequence::create(
			CCDelayTime::create(14*0.03f),	// 第22帧
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
	}



	for (int i=0;i<targetNum;i++)
	{
		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;
		/*msg_data->hited_sound = "jianyu_02.wav";*/

		// 箭
		{
			SPRITE* fangYu = SPRITE::create("jineng_huntsman_jianyu_zhishi_1.png");
			targetVec[i]->getContainer()->addChild(fangYu,targetVec[i]->getZOrder()+4);
			fangYu->setPosition(ccpAdd(targetVec[i]->getSkillFirePos(),ccp(0,80)));
			fangYu->setVisible(false);

			CCFiniteTimeAction* dun_action = EASY_ACTION("jineng_huntsman_jianyu_zhishi_%d.png",1,19,0.04f,false);

			float randomDelay = CCRANDOM_0_1()*0.3f;
			fangYu->runAction(
				CCSequence::create(
				CCDelayTime::create(0.55f + randomDelay),//0.9f
				CCShow::create(),
				LFDelayToMusic::create(0.0f,"jianyu_02.wav"),
				dun_action,
				CCHide::create(),
				NULL
				)
				);

			fangYu->runAction(CCSequence::create(
				CCDelayTime::create(0.55f + randomDelay + 0.18f),//0.9f
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCDelayTime::create(dun_action->getDuration()),
				CCCallFuncN::create(targetVec[i]->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				));
		}
	}

}
void ClientSkillProcessorBase::skill_fight_huoQiuShu(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	1

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif

	//CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.4f);

	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");

	// 攻击者的动画,需要跟技能动画协调时间
	{
		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
	}

	const float frame_gap = 0.08f;
	for (int i=0,count=step.targetUniqueVec.size(); i<count; i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");

		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;


		{
			// 技能动画
			// 得到A到B的向量，如果y<0则B在下面
			CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),target->getFireNode());
			float angle = LF::getRotateAngle(aToB,ccp(0,1));

			SPRITE* shi_fa = SPRITE::create("jineng_mishushi_ronghuachongji_shifa_1.png");
			fightor->addChild(shi_fa,10);
			shi_fa->setPosition(ccpAdd(fightor->getSkillFirePos(),ccp(0,0)));

			//
			Music::sharedMusicPlayer()->playEffectSound("shandian_1.wav");

			CCFiniteTimeAction* shi_fa_action = EASY_ACTION(
				"jineng_mishushi_ronghuachongji_shifa_%d.png",1,7,frame_gap,false);
			shi_fa->runAction(CCSequence::create(
				shi_fa_action,
				CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				));
			shi_fa->setRotation(angle);


			SPRITE* feixing = SPRITE::create("jineng_mishushi_ronghuachongji_feixing.png");
			BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
			CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
			CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
			layer->addChild(feixing,10);
			feixing->setPosition(ccpAdd(LF::getNativePostion(fightor->getFireNode(),layer),ccp(0,0)));
			feixing->setRotation(angle);
			feixing->setVisible(false);
			// 箭飞行
			float dis = ccpDistance(endPoint,statPoint);
			float time = dis/456.0f * 0.22f;
			CCPoint offset = ccp(feixing->getContentSize().height/2,feixing->getContentSize().height/2);
			float extraWait = 0.0f;
			//CCFiniteTimeAction* flyFrame = EASY_ACTION("jineng_mishushi_ronghuachongji_feixing_%d.png",1,2,time/3.0f,false);;
			//CCAction* fly = CCSequence::create(
			//	CCDelayTime::create(shi_fa_action->getDuration()+extraWait),
			//	CCShow::create(),
			//	CCMoveTo::create(time,ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
			//	CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			//	CCCallFuncN::create(target,SEL_CallFuncN(&SPRITE::removeNode)),
			//	NULL
			//	);
			//feixing->runAction(fly);
			feixing->runAction(CCSequence::create(
				CCDelayTime::create(shi_fa_action->getDuration()+extraWait),
				CCShow::create(),
				//flyFrame,
				CCMoveTo::create(time,ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCCallFuncN::create(target,SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				));


			SPRITE* shoushang = SPRITE::create("jineng_mishushi_ronghuachongji_shoushang_1.png");
			target->addChild(shoushang,10);
			shoushang->setPosition(ccpAdd(target->getFireNode()->getPosition(),ccp(15,50)));
			CCFiniteTimeAction* shoushang_action = EASY_ACTION("jineng_mishushi_ronghuachongji_shoushang_%d.png",1,6,frame_gap,false);
			shoushang->setVisible(false);
			shoushang->runAction(CCSequence::create(
				CCDelayTime::create(shi_fa_action->getDuration()+extraWait+time),
				CCShow::create(),
				LFDelayToMusic::create(0,"shandian_2.wav",true),
				shoushang_action,
				CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				));

		}
	}

}
void ClientSkillProcessorBase::skill_fight_wenYi(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	1

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif

	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}
	// 该技能只有Buff
	CCAssert(step.firedBuff.size() > 0,"buff必须要有");

	Music::sharedMusicPlayer()->playEffectSound("linghundiaoling_1.wav");

	const float frame_gap = 0.07f;
	// 攻击者的动画
	{
		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));

		SPRITE* shi_fa = SPRITE::create("jineng_wushi_linghundiaoling_shifa_1.png");
		fightor->addChild(shi_fa,10);
		CCPoint pos = ccpAdd(
			ccp(fightor->getContentSize().width/2,fightor->getContentSize().height/2),
			ccp(0,(shi_fa->getContentSize().height-fightor->getContentSize().height)/2));
		shi_fa->setPosition(ccpAdd(pos,ccp(0,80))); 

		CCFiniteTimeAction* shi_fa_action = EASY_ACTION(
			"jineng_wushi_linghundiaoling_shifa_%d.png",1,13,frame_gap,false);
		shi_fa->runAction(CCSequence::create(
			shi_fa_action,
			CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
	}

	//// 中心爆炸
	//CCPoint boardCenter = targetVec[0]->getCurrentBoardProperty()->getCenterWorldPoint();
	//CCLayer* layer = GameDirector::getDirector()->getFightLayer();
	//SPRITE* center_bomb = SPRITE::create("jineng_wushi_linghundiaoling_feixing_1.png");
	//layer->addChild(center_bomb,10);
	//center_bomb->setPosition(layer->convertToNodeSpace(boardCenter));
	//center_bomb->setVisible(false);
	//CCFiniteTimeAction* center_bomb_action = EASY_ACTION(
	//	"jineng_wushi_linghundiaoling_feixing_%d.png",1,15,frame_gap,false);
	//center_bomb->runAction(CCSequence::create(
	//	CCDelayTime::create(frame_gap * 13),
	//	CCShow::create(),
	//	LFDelayToMusic::create(0.0f,"linghundiaoling_2.wav"),
	//	center_bomb_action,
	//	CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
	//	NULL
	//	));
	//CCPoint vec = LF::getVectorFromAToB(center_bomb,fightor);
	//if (vec.y > 0)
	//{
	//	center_bomb->setPositionY(center_bomb->getPositionY() + center_bomb->getContentSize().height/2 - 160);
	//}



	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = targetVec[i];

		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;

		// 瘟疫的图片效果
		{
			SPRITE* shoushang = SPRITE::create("jineng_wushi_linghundiaoling_shoushang_1.png");
			target->addChild(shoushang,10);
			shoushang->setPosition(ccpAdd(target->getSkillFirePos(),ccp(0,0)));

			CCFiniteTimeAction* shan_dian_action = EASY_ACTION(
				"jineng_wushi_linghundiaoling_shoushang_%d.png",1,10,0.1f,false);
			shoushang->setVisible(false);
			shoushang->runAction(CCSequence::create(
				CCDelayTime::create(frame_gap*(12)),
				LFDelayToMusic::create(0.0f,"linghundiaoling_2.wav"),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCShow::create(),
				shan_dian_action,
				CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				));

			//target->runAction(CCSequence::create(
			//	CCDelayTime::create(fightTime*0.8f),
			//	CCDelayTime::create(shan_dian_action->getDuration()),
			//	LFDelayToMusic::create(0,"shandian_3.wav",true),
			//	CCFadeTo::create(0.01f,255),
			//	NULL));

			/*target->runAction(CCSequence::create(
				CCDelayTime::create(fightTime*0.4f+randomDelay),
				CCTintTo::create(0.3f,53,112,18),
				CCRepeat::create(CCSequence::create(
					CCTintTo::create(0.43f,77,142,40),
					CCTintTo::create(0.43f,53,112,18),
					NULL
				),3),
				CCTintTo::create(0.5f,255,255,255),
				NULL));*/
		}
	}
}

// 技能书1
void ClientSkillProcessorBase::skill_fight_BanYueZhan(StepSkill step)
{
	CCAssert(false,"还没实现");
}
void ClientSkillProcessorBase::skill_fight_CiSha(StepSkill step)
{
	CCAssert(false,"还没实现");
}
void ClientSkillProcessorBase::skill_fight_LeiBao(StepSkill step)
{
	//loadPlist("jineng_jisi_leibao.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));


	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}

	float shifa_time = 0.0f;

	Music::sharedMusicPlayer()->playEffectSound("shandian_1.wav");

	// 攻击者的动画
	{
		SkillSprite* shifa = SkillSprite::create("jineng_jisi_leibao_shifa_1.png");
		CCFiniteTimeAction* shifa_action = SpriteHelper::easyCreateAction_pList("jineng_jisi_leibao_shifa_%d.png",1,25,0.035f,false);
		shifa->runAction(CCSequence::create(
			shifa_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		shifa->setPosition(ccp(fightor->getContentSize().width/2,fightor->getContentSize().height/2));
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(shifa,10);
		shifa->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));

		shifa_time = shifa_action->getDuration();

		fightor->runAction(CCSequence::create(
			CCDelayTime::create(shifa_time),
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
	}



	for (int i=0;i<targetNum;i++)
	{
		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;
		msg_data->hited_sound = "shandian_2.wav";


		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),targetVec[i]->getFireNode());
		float angle = LF::getRotateAngle(aToB,ccp(0,1));

		SkillSprite* jian = SkillSprite::create("jineng_jisi_leibao_feixing.png");
		jian->setVisible(false);

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(targetVec[i]->getFireNode(),layer);
		layer->addChild(jian,10);
		jian->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));

		jian->setRotation(angle);
		// 箭飞行
		float dis = ccpDistance(endPoint,statPoint);
		float time = dis/456.0f * 0.18f;
		CCPoint offset = ccp(jian->getContentSize().height/2,jian->getContentSize().height/2);
		CCAction* fly = CCSequence::create(
			CCDelayTime::create(shifa_time),
			CCShow::create(),
			CCSpawn::create(
			CCMoveTo::create(time,ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
			CCFadeTo::create(time,0),NULL),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		jian->runAction(fly);



		SkillSprite* shoushang = SkillSprite::create("jineng_jisi_leibao_shoushuang_1.png");
		CCFiniteTimeAction* shifa_action = SpriteHelper::easyCreateAction_pList("jineng_jisi_leibao_shoushuang_%d.png",1,11,0.03f,false);
		shoushang->runAction(CCSequence::create(
			CCDelayTime::create(shifa_time+time),
			CCShow::create(),
			shifa_action,
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
		shoushang->setPosition(ccp(fightor->getContentSize().width/2,fightor->getContentSize().height/2));
		layer->addChild(shoushang,10);
		shoushang->setPosition(LF::getNativePostion(targetVec[i]->getFireNode(),layer));
		shoushang->setVisible(false);


		/*for (int shandianIndex = 0;shandianIndex < 3;shandianIndex++)
		{
			SkillSprite* shan_dian = SkillSprite::create("jineng_jisi_shandian_shoushang_1.png");
			targetVec[i]->addChild(shan_dian,10);
			CCPoint pos_offset[3] = {ccp(15,50)};
			float delay_arr[3] = {0};

			shan_dian->setPosition(ccpAdd(targetVec[i]->getNormalPos(),pos_offset[shandianIndex]));

			CCFiniteTimeAction* jieSu_action = SpriteHelper::easyCreateAction_pList("jineng_jisi_leibao_shoushang_jieshu_%d.png",1,14,0.03f,false);
			shan_dian->setVisible(false);
			shan_dian->runAction(CCSequence::create(
				CCDelayTime::create(shifa_time+time + shifa_action->getDuration()),
				CCDelayTime::create(delay_arr[shandianIndex]),
				CCShow::create(),
				LFDelayToMusic::create(0,"shandian_2.wav",true),
				jieSu_action,
				CCCallFuncN::create(fightor,SEL_CallFuncN(&SkillSprite::removeNode)),
				NULL
				));

			targetVec[i]->runAction(CCSequence::create(
				CCDelayTime::create(shifa_time+time + shifa_action->getDuration()),
				CCDelayTime::create(delay_arr[shandianIndex]),
				LFDelayToMusic::create(0,"shandian_3.wav",true),
				CCRepeat::create(
				CCSequence::create(
				CCFadeTo::create(0.03f,100),
				CCFadeTo::create(0.03f,255),
				NULL),
				11),
				CCFadeTo::create(0.01f,255),
				NULL));
		}*/
	}

}
void ClientSkillProcessorBase::skill_fight_MangCi(StepSkill step)
{
	//loadPlist("jineng_qishi_mangci.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));


	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
	// 该技能只有Buff
	CCAssert(step.firedBuff.size() > 0,"buff必须要有");


	Music::sharedMusicPlayer()->playEffectSound("jiaHuJia_1.wav");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;
	msg_data->hited_sound = "jiaHuJia_2.wav";

	// 攻击者的动画
	{
		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
		SkillSprite* fire = SkillSprite::create("jineng_qishi_mangci_shifa_1.png");
		CCFiniteTimeAction* fire_action = SpriteHelper::easyCreateAction_pList("jineng_qishi_mangci_shifa_%d.png",1,15,0.04f,false);
		fire->runAction(CCSequence::create(
			fire_action,
			CCFadeTo::create(0.2f,50),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		fire->setPosition(ccp(fightor->getContentSize().width/2,fightor->getContentSize().height/2));
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(fire,10);
		fire->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
	}

}
void ClientSkillProcessorBase::skill_fight_ShenZhiZhuFu(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	1

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
	#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
	#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif

	

	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}
	// 该技能只有Buff
	CCAssert(step.firedBuff.size() > 0,"buff必须要有");

	const float fire_frame_gap = 0.045f;
	// 攻击者的动画
	{
		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
		//jineng_magebane_shenzhizhufu_shifa_27.png
		SPRITE* fire = SPRITE::create("jineng_magebane_shenzhizhufu_shifa_1.png");
		CCFiniteTimeAction* fire_action = EASY_ACTION("jineng_magebane_shenzhizhufu_shifa_%d.png",1,17,fire_frame_gap,false);
		fire->runAction(CCSequence::create(
			LFDelayToMusic::create(0.0f,"shenzhizhufu_1.wav"),
			fire_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(fire,10);
		fire->setPosition(ccpAdd(LF::getNativePostion(fightor->getFireNode(),layer),ccp(-30,160)));

	}



	for (int i=0;i<targetNum;i++)
	{
		//Music::sharedMusicPlayer()->playEffectSound("jiaHuJia_1.wav");

		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;
		//msg_data->hited_sound = "jiaHuJia_2.wav";


		// 防御的万字符
		{
			SPRITE* fangYu = SPRITE::create("jineng_magebane_shenzhizhufu_xiaoguo_1.png");
			targetVec[i]->getContainer()->addChild(fangYu,targetVec[i]->getZOrder()+4);
			fangYu->setPosition(ccpAdd(targetVec[i]->getSkillFirePos(),ccp(0,-5)));
			fangYu->setVisible(false);

			CCFiniteTimeAction* dun_action = EASY_ACTION("jineng_magebane_shenzhizhufu_xiaoguo_%d.png",1,17,fire_frame_gap,false);

			fangYu->runAction(
				CCSequence::create(
				CCDelayTime::create((17*fire_frame_gap + fire_frame_gap)+CCRANDOM_0_1()*0.2f),//
				LFDelayToMusic::create(0.0f,"shenzhizhufu_2.wav"),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCShow::create(),
				dun_action,
				CCCallFuncN::create(targetVec[i]->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				)
				);
		}
	}
}
void ClientSkillProcessorBase::skill_fight_LiRenHuiXuan(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	1

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif


	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}

	float jianFlyDelay = 0;
	// 攻击者的动画
	{
		SPRITE* shiFa1 = SPRITE::create("jineng_youxia_lirenhuixuan_shifa_1.png");
		CCFiniteTimeAction* shifa1_action = EASY_ACTION("jineng_youxia_lirenhuixuan_shifa_%d.png",1,21,0.03f,false);
		shiFa1->runAction(CCSequence::create(
			LFDelayToMusic::create(0.0f,"lirenhuixuan_01.wav"),
			shifa1_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
		fightor->getContainer()->addChild(shiFa1,10);
		shiFa1->setPosition(fightor->getPosition());

		jianFlyDelay = shifa1_action->getDuration();

		fightor->runAction(CCSequence::create(
			CCDelayTime::create(jianFlyDelay),
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));

	}


	if (targetNum == 1)
	{
		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,0);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;
		msg_data->hited_sound = "jiaHuJia_2.wav";

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(targetVec[0]->getFireNode(),layer);
		endPoint = ccpAdd(endPoint,ccp(0,0));

		SPRITE* feixing = SPRITE::create("jineng_youxia_lirenhuixuan_feixing.png");
		CCPoint offset = ccp(feixing->getContentSize().height/2,0);
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		CCPoint aToB = LF::getVectorFromAToB(fightor,targetVec[0]);
		// 箭飞行
		float dis = ccpDistance(endPoint,statPoint);
		float time = dis/456.0f * 0.3f;
		// 延长飞行的距离
		CCPoint endPointExtern = ccpAdd(endPoint,ccp(aToB.x * 180.0f,aToB.y*180.0f));

		layer->addChild(feixing,10);
		feixing->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
		feixing->setVisible(false);

		CCPoint moveEnd = ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y));
		CCPoint moveEnd2 = ccpSub(endPointExtern,ccp(offset.x * aToB.x,offset.y * aToB.y));
		float flyDelay = jianFlyDelay+0*0.0f;
		CCAction* fly = CCSequence::create(
			CCDelayTime::create(flyDelay),
			CCShow::create(),
			LFDelayToMusic::create(0.0f,"lirenhuixuan_02.wav"),
			CCMoveTo::create(time,moveEnd),
			LFDelayToMusic::create(0.0f,"lirenhuixuan_03.wav"),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			CCMoveTo::create(time,moveEnd2),
			CCSpawn::create(
			CCMoveTo::create(time*2.3f,ccpSub(statPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
			CCFadeTo::create(time*2.3f,50),NULL),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		feixing->runAction(fly);
		feixing->runAction(CCRepeat::create(CCRotateBy::create(0.1f,720),-1));
		//feixing->runAction(CCRepeat::create(CCRotateBy::create(1.0f,720),-1));

		SPRITE* shoushang = SPRITE::create("jineng_youxia_lirenhuixuan_feixing_bg.png");
		layer->addChild(shoushang,10);
		shoushang->setVisible(false);
		shoushang->setPosition(moveEnd);
		shoushang->runAction(CCSequence::create(
			CCDelayTime::create(flyDelay + time),
			CCShow::create(),
			CCFadeTo::create(0.1f,50),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
	}
	else if (targetNum == 2)
	{
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint0 = LF::getNativePostion(targetVec[0]->getFireNode(),layer);
		CCPoint endPoint1 = LF::getNativePostion(targetVec[1]->getFireNode(),layer);
		CCPoint centerPoint = ccpMidpoint(endPoint0,endPoint1);

		float dis0 = ccpDistance(endPoint0,statPoint);
		float dis1 = ccpDistance(endPoint1,statPoint);
		float disFromCenter = ccpDistance(centerPoint,statPoint);
		float dis_max = dis0 >= dis1 ? dis0 : dis1;

		// 近的就是0
		MSG_Skill* msg_data0 = new MSG_Skill();
		msg_data0->step = getOneFightUniteByIndex(step,dis0 < dis1?0:1);
		msg_data0->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data0->playDefaultHithedSound = false;
		msg_data0->hited_sound = "jiaHuJia_2.wav";

		// 远的就是1
		MSG_Skill* msg_data2 = new MSG_Skill();
		msg_data2->step = getOneFightUniteByIndex(step,dis0 >= dis1?0:1);
		msg_data2->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data2->playDefaultHithedSound = false;
		msg_data2->hited_sound = "jiaHuJia_2.wav";

		// 需要延长的距离
		float dis_need_add = dis_max>disFromCenter?dis_max-disFromCenter:0;
		// 实际该打到的点
		CCPoint endPos = ccpAdd(centerPoint, ccpMult(ccp(centerPoint.x - statPoint.x,centerPoint.y - statPoint.y),dis_need_add/dis_max) );


		SPRITE* feixing = SPRITE::create("jineng_youxia_lirenhuixuan_feixing.png");
		CCPoint offset = ccp(feixing->getContentSize().height/2,0);
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		CCPoint aToB = ccpNormalize(ccpSub(endPos,statPoint));
		// 箭飞行
		float dis = ccpDistance(endPos,statPoint);
		float time = dis/456.0f * 0.4f;

		layer->addChild(feixing,10);
		feixing->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
		feixing->setVisible(false);

		CCPoint moveEnd = ccpSub(endPos,ccp(offset.x * aToB.x,offset.y * aToB.y));
		float flyDelay = jianFlyDelay+0*0.1f;
		CCAction* fly = CCSequence::create(
			CCDelayTime::create(flyDelay),
			CCShow::create(),
			LFDelayToMusic::create(0.0f,"lirenhuixuan_02.wav"),
			CCMoveTo::create(time/2,ccpMidpoint(statPoint,moveEnd)),
			LFDelayToMusic::create(0.0f,"lirenhuixuan_03.wav"),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data0),
			CCMoveTo::create(time/2,moveEnd),
			LFDelayToMusic::create(0.0f,"lirenhuixuan_03.wav"),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data2),
			CCSpawn::create(
			CCMoveTo::create(time*1.3f,ccpSub(statPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
			CCFadeTo::create(time*1.3f,50),NULL),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		feixing->runAction(fly);
		feixing->runAction(CCRepeat::create(CCRotateBy::create(0.1f,720),-1));

		SPRITE* shoushang = SPRITE::create("jineng_youxia_lirenhuixuan_feixing_bg.png");
		layer->addChild(shoushang,10);
		shoushang->setVisible(false);
		shoushang->setPosition(moveEnd);
		shoushang->runAction(CCSequence::create(
			CCDelayTime::create(flyDelay + time),
			CCShow::create(),
			CCFadeTo::create(0.1f,50),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));

		SPRITE* shoushang0 = SPRITE::create("jineng_youxia_lirenhuixuan_feixing_bg.png");
		layer->addChild(shoushang0,10);
		shoushang0->setVisible(false);
		shoushang0->setPosition(ccpMidpoint(statPoint,moveEnd));
		shoushang0->runAction(CCSequence::create(
			CCDelayTime::create(flyDelay + time/2),
			CCShow::create(),
			CCFadeTo::create(0.1f,50),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));

	}
	else
	{
		CCAssert(false,"不支持");
	}


	//for (int i=0;i<targetNum;i++)
	//{

	//	MSG_Skill* msg_data = new MSG_Skill();
	//	msg_data->step = getOneFightUniteByIndex(step,i);
	//	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	//	msg_data->playDefaultHithedSound = false;
	//	msg_data->hited_sound = "jiaHuJia_2.wav";

	//	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	//	CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
	//	CCPoint endPoint = LF::getNativePostion(targetVec[i]->getFireNode(),layer);
	//	endPoint = ccpAdd(endPoint,ccp(0,0));

	//	SkillSprite* feixing = SkillSprite::create("jineng_youxia_lirenhuixuan_feixing.png");
	//	CCPoint offset = ccp(feixing->getContentSize().height/2,0);
	//	// 技能动画
	//	// 得到A到B的向量，如果y<0则B在下面
	//	CCPoint aToB = LF::getVectorFromAToB(fightor,targetVec[i]);
	//	// 箭飞行
	//	float dis = ccpDistance(endPoint,statPoint);
	//	float time = dis/456.0f * 0.18f;

	//	layer->addChild(feixing,10);
	//	feixing->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
	//	feixing->setVisible(false);

	//	CCPoint moveEnd = ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y));
	//	float flyDelay = jianFlyDelay+i*0.1f;
	//	CCAction* fly = CCSequence::create(
	//		CCDelayTime::create(flyDelay),
	//		CCShow::create(),
	//		CCMoveTo::create(time,moveEnd),
	//		CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
	//		//CCSpawn::create(
	//		//	CCMoveTo::create(time*1.3f,ccpSub(statPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
	//		//	CCFadeTo::create(time*1.3f,50),NULL),
	//		CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
	//		NULL
	//		);
	//	feixing->runAction(fly);
	//	feixing->runAction(CCRepeat::create(CCRotateBy::create(0.1f,360),-1));


	//	/*SkillSprite* feixing_chuantou = SkillSprite::create("jineng_youxia_lirenhuixuan_feixing.png");
	//	feixing->getParent()->addChild(feixing_chuantou,10);
	//	feixing_chuantou->setPosition(feixing->getPosition());
	//	feixing_chuantou->setVisible(false);
	//	feixing_chuantou->runAction(CCSequence::create(
	//		CCDelayTime::create(flyDelay),
	//		CCShow::create(),
	//		CCMoveTo::create(time,moveEnd),
	//		CCSpawn::create(
	//		CCMoveBy::create(time,ccp(0,dis * aToB.y)),
	//		CCFadeTo::create(time,0),NULL),
	//		CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
	//		NULL
	//		));
	//	feixing_chuantou->runAction(CCRepeat::create(CCRotateBy::create(0.1f,360),-1));*/


	//	//SkillSprite* attack = SkillSprite::create("jineng_youxia_lirenhuixuan_shoushang_1.png");
	//	//attack->setVisible(false);
	//	//attack->runAction(CCSequence::create(
	//	//	CCDelayTime::create(flyDelay+time+0.2f),
	//	//	CCShow::create(),
	//	//	(CCFiniteTimeAction*)attackAction->copy()->autorelease(),
	//	//	CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
	//	//	NULL
	//	//	));
	//	//layer->addChild(attack,10);
	//	//attack->setPosition(moveEnd);
	//	//attack->setOpacity(50);


	//	/*SkillSprite* feixingWeiBa = SkillSprite::create("jineng_youxia_lirenhuixuan_feixing2.png");
	//	feixing->addChild(feixingWeiBa);
	//	feixingWeiBa->setPosition(ccp(feixing->getContentSize().width/2,feixing->getContentSize().height/2));
	//	feixingWeiBa->runAction(CCSequence::create(
	//	CCDelayTime::create(flyDelay),
	//	CCShow::create(),
	//	CCDelayTime::create(time),
	//	CCHide::create(),
	//	CCCallFuncN::create(feixing,SEL_CallFuncN(&SkillSprite::removeNode)),
	//	NULL
	//	));*/

	//	SkillSprite* shoushang = SkillSprite::create("jineng_youxia_lirenhuixuan_feixing_bg.png");
	//	layer->addChild(shoushang,10);
	//	shoushang->setVisible(false);
	//	shoushang->setPosition(moveEnd);
	//	shoushang->runAction(CCSequence::create(
	//		CCDelayTime::create(flyDelay + time),
	//		CCShow::create(),
	//		//CCDelayTime::create(0.4f),
	//		CCFadeTo::create(0.1f,50),
	//		CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
	//		NULL
	//		));

	//	//SkillSprite* shouShang = SkillSprite::create("jineng_youxia_lirenhuixuan_shoushang_1.png");
	//	//feixing->addChild(shouShang,10);
	//	//shouShang->setPosition(ccp(feixing->getContentSize().width/2,feixing->getContentSize().height/2));
	//	//shouShang->setVisible(false);
	//	//shouShang->runAction(CCSequence::create(
	//	//	CCDelayTime::create(jianFlyDelay+time),
	//	//	CCShow::create(),
	//	//	(CCFiniteTimeAction*)shouShangAction->copy()->autorelease(),
	//	//	CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
	//	//	NULL
	//	//	));
	//}

}
void ClientSkillProcessorBase::skill_fight_JingZhunZhiCan(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	0

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif

	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");

	Music::sharedMusicPlayer()->playEffectSound("lieGong_1.wav");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;
	msg_data->hited_sound = "lieGong_2.wav";


	{
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),target->getFireNode());
		float angle = LF::getRotateAngle(aToB,ccp(0,1));

		SPRITE* yun_qi = SPRITE::create();
		SPRITE* jian_begin = SPRITE::create("jineng_shenjianshou_jingzhunzhican_feixing_1.png");
		jian_begin->setVisible(false);
		SPRITE* jian_end = SPRITE::create("jineng_shenjianshou_jingzhunzhican_feixing_2.png");
		jian_end->setVisible(false);

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
		layer->addChild(yun_qi,10);
		layer->addChild(jian_begin,10);
		layer->addChild(jian_end,10);

		jian_begin->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
		jian_begin->setRotation(angle);
		jian_end->setRotation(angle);
		jian_end->setPosition(endPoint);
		yun_qi->setRotation(angle);
		yun_qi->setScale(1.25f);
		yun_qi->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));

		float action_delay = 0;
		// 运气
		CCFiniteTimeAction* yun_qi_action = EASY_ACTION("jineng_shenjianshou_jingzhunzhican_shifa_%d.png",1,12,0.04f,false);
		yun_qi->setVisible(false);
		yun_qi->runAction(CCSequence::create(
			CCDelayTime::create(action_delay),
			CCShow::create(),
			yun_qi_action,
			CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));

		// 显现一下
		jian_begin->runAction(CCSequence::create(
			CCDelayTime::create(action_delay + yun_qi_action->getDuration() - 0.04f),
			CCShow::create(),
			CCDelayTime::create(0.04f),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));

		float fly_delay = yun_qi_action->getDuration()+ action_delay;


		// 攻击者的动画,需要跟技能动画协调时间
		{
			CCFiniteTimeAction* back = fightor->get_fire_shake_Action(fightor,target);
			CCAction* action =  CCSequence::create(
				CCDelayTime::create(fly_delay),
				back,
				NULL
				);
			fightor->runAction(action);
		}

		CCFiniteTimeAction* bomb2Action = EASY_ACTION("jineng_shenjianshou_jingzhunzhican_shoushang_%d.png",1,4,0.04f,false);
		SPRITE* bomb2 = SPRITE::create();
		bomb2->setVisible(false);
		bomb2->runAction(CCSequence::create(
			CCDelayTime::create(fly_delay),
			CCShow::create(),
			bomb2Action,
			CCCallFuncN::create(target,SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
		bomb2->setPosition(ccpAdd(LF::getNativePostion(target,layer),ccp(0,30)));
		layer->addChild(bomb2,10);

		// 显现一下
		jian_end->runAction(CCSequence::create(
			CCDelayTime::create(fly_delay - 0.04f),
			CCShow::create(),
			CCDelayTime::create(0.04f),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
	}
}
void ClientSkillProcessorBase::skill_fight_JiSuDuJian(StepSkill step)
{
	//loadPlist("jineng_lieshou_jishudujian.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));



	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}

	float jianFlyDelay = 0;
	// 攻击者的动画
	{
		SkillSprite* shiFa1 = SkillSprite::create("jineng_lieshou_jishudujian_shifa_1_1.png");
		CCFiniteTimeAction* shifa1_action = SpriteHelper::easyCreateAction_pList("jineng_lieshou_jishudujian_shifa_1_%d.png",1,22,0.03f,false);
		shiFa1->runAction(CCSequence::create(
			LFDelayToMusic::create(0.0f,"jianyu_01.wav"),
			shifa1_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		fightor->getContainer()->addChild(shiFa1,10);
		shiFa1->setPosition(fightor->getPosition());

		float shifa2Delay = shifa1_action->getDuration() / 22.0f * 13.0f; // 13帧开始
		SkillSprite* shiFa2 = SkillSprite::create("jineng_lieshou_jishudujian_shifa_2_1.png");
		shiFa2->setVisible(false);
		fightor->getContainer()->addChild(shiFa2,10);
		shiFa2->setPosition(fightor->getPosition());

		CCFiniteTimeAction* shifa2_action = SpriteHelper::easyCreateAction_pList("jineng_lieshou_jishudujian_shifa_2_%d.png",1,13,0.03f,false);
		shiFa2->runAction(CCSequence::create(
			CCDelayTime::create(shifa2Delay),
			CCShow::create(),
			shifa2_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));

		jianFlyDelay = shifa2Delay + shifa2_action->getDuration();

		fightor->runAction(CCSequence::create(
			CCDelayTime::create(jianFlyDelay),
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
		
	}

	CCFiniteTimeAction* bombAction = SpriteHelper::easyCreateAction_pList("jineng_lieshou_jishudujian_shoushang_%d.png",1,31,0.03f,false);

	for (int i=0;i<targetNum;i++)
	{
		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		//msg_data->playDefaultHithedSound = false;
		//msg_data->hited_sound = "jiaHuJia_2.wav";

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(targetVec[i]->getFireNode(),layer);
		
		SkillSprite* jian = SkillSprite::create("jineng_lieshou_jishudujian_feixing.png");
		CCPoint offset = ccp(jian->getContentSize().height/2,jian->getContentSize().height/2);
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),targetVec[i]->getFireNode());
		float angle = LF::getRotateAngle(aToB,ccp(0,1));
		// 箭飞行
		float dis = ccpDistance(endPoint,statPoint);
		float time = dis/456.0f * 0.18f;

		layer->addChild(jian,10);
		jian->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
		jian->setRotation(angle);
		jian->setVisible(false);

		CCAction* fly = CCSequence::create(
			CCDelayTime::create(jianFlyDelay),
			CCShow::create(),
			LFDelayToMusic::create(0.0f,"huoJian.wav"),
			CCSpawn::create(
			CCMoveTo::create(time,ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
			CCFadeTo::create(time,0),NULL),
			LFDelayToMusic::create(0,"jishudujian_03.wav"),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		jian->runAction(fly);


		SkillSprite* shouShang = SkillSprite::create("jineng_lieshou_jishudujian_shoushang_1.png");
		targetVec[i]->addChild(shouShang,10);
		shouShang->setPosition(targetVec[i]->getNormalPos());
		shouShang->setVisible(false);
		shouShang->runAction(CCSequence::create(
			CCDelayTime::create(jianFlyDelay+time),
			CCShow::create(),
			(CCFiniteTimeAction*)bombAction->copy()->autorelease(),
			CCSpawn::create(CCScaleTo::create(0.3f,2.0f),CCFadeTo::create(0.3f,0),NULL),
			//CCRepeat::create(CCSequence::create(
			//	CCMoveBy::create(0.3f,ccp(0,20)),
			//	CCMoveBy::create(0.3f,ccp(0,-20)),NULL),1),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
	}

}
void ClientSkillProcessorBase::skill_fight_BaoFengXue(StepSkill step)
{
	CCAssert(false,"还没实现");
}
void ClientSkillProcessorBase::skill_fight_KongJu(StepSkill step)
{
	CCAssert(false,"还没实现");
}

// 技能书2
void ClientSkillProcessorBase::skill_fight_KuangBao(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	0

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif


	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target != NULL,"");

	Music::sharedMusicPlayer()->playEffectSound("baoLieZhan_1.wav");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;
	msg_data->hited_sound = "baoLieZhan_2.wav";


	float move_time = 0;
	float delay_before_move = 0.4f;
	// 攻击动画
	{
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
		CCPoint vec = LF::getVectorFromAToB(fightor,target);
		CCPoint moveByPoint = ccpSub(endPoint,statPoint);
		moveByPoint = ccpSub(moveByPoint,ccp(target->getContentSize().width/2 * vec.x,target->getContentSize().height/2 * vec.y));
		float moveTime = ccpDistance(moveByPoint,CCPointZero)/380.0f * 0.2f;
		moveTime = MAX(moveTime,0.08f);
		CCAction* action =  CCSequence::create(
			CCDelayTime::create(delay_before_move),
			CCEaseIn::create(CCMoveTo::create(moveTime*0.5f,ccpAdd(fightor->getPosition(),ccpMult(moveByPoint,1.0f))),2.0f),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			CCEaseOut::create(CCMoveTo::create(moveTime,fightor->getPosition()),2.0f),
			CCDelayTime::create(0.1f),
			NULL
			);
		fightor->runAction(action);

		move_time = moveTime;
	}

	// 攻击效果
	{
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();

		SPRITE* fightEff = SPRITE::create("jineng_yemanren_kuangbao_shifa_1.png");
		fightor->addChild(fightEff,10);
		fightEff->setPosition(ccpAdd(fightor->getSkillFirePos(),ccp(0,-40)));

		{
			//CCParticleSystemQuad* bomb = CCParticleSystemQuad::create("baoliezhanbaozhan.plist");
			//bomb->setAutoRemoveOnFinish(true);
			//target->addChild(bomb);
			//bomb->setPosition(target->getSkillFirePos());
			//bomb->setVisible(false);
			//bomb->setScale(0.8f);
			//bomb->runAction(CCSequence::create(
			//	CCDelayTime::create(move_time+delay_before_move),
			//	CCShow::create(),
			//	NULL
			//	));
		}
		

		SPRITE* zhuaHeng = SPRITE::create("jineng_yemanren_kuangbao_shoushang_1.png");
		target->addChild(zhuaHeng,1);
		zhuaHeng->setPosition(target->getSkillFirePos());


		CCFiniteTimeAction* fightAction = EASY_ACTION("jineng_yemanren_kuangbao_shifa_%d.png",1,8,0.035f,false);
		fightEff->runAction(CCSequence::create(
			fightAction,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));

		

		CCFiniteTimeAction* zhuaHengAction = EASY_ACTION("jineng_yemanren_kuangbao_shoushang_%d.png",1,6,0.005f,false);
		zhuaHeng->setVisible(false);
		zhuaHeng->runAction(CCSequence::create(
			CCDelayTime::create(move_time+delay_before_move),
			CCShow::create(),
			zhuaHengAction,
			CCDelayTime::create(0.2f),
			CCCallFuncN::create(zhuaHeng->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
	}

}
void ClientSkillProcessorBase::skill_fight_ShiXueZhiRen(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	0

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif



	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");

	// 冲出去的耗时和延时
	float move_time = 0;
	float delay_before_move = 0.0f;
	// 攻击者的动画
	{
		//fightor->runAction(CCSequence::create(
		//	CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
		//	CCRotateTo::create(0.0f,0),
		//	CCScaleTo::create(0.1f,1.15f),
		//	CCScaleTo::create(0.01f,1.0f),
		//	NULL
		//	));

		//jineng_hunter_youmingzhinu_shifa_16.png
		SPRITE* fire = SPRITE::create("jineng_hunter_youmingzhinu_shifa_1.png");
		CCFiniteTimeAction* fire_action = EASY_ACTION("jineng_hunter_youmingzhinu_shifa_%d.png",1,16,0.04f,false);
		fire->runAction(CCSequence::create(
			fire_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(fire,10);
		fire->setPosition(ccpAdd(LF::getNativePostion(fightor->getFireNode(),layer),ccp(8,0)));

		delay_before_move = 12*0.04f; // 

		Music::sharedMusicPlayer()->playEffectSound("baoLieZhan_1.wav");

		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = step;
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;
		msg_data->hited_sound = "baoLieZhan_2.wav";

		// 冲出去
		fightor->runAction(createAttackMovementAction(fightor,target,delay_before_move,move_time,msg_data));
		// 变透明
		{
			vector<CCSprite*> figterBodyParts = fightor->getItemParts(Part_Body);
			int partNum = figterBodyParts.size();
			for (int i=0;i<partNum;i++)
			{
				figterBodyParts[i]->runAction(CCSequence::create(
					CCDelayTime::create(delay_before_move),
					CCFadeTo::create(0.2f,50),// 第9帧就看不到了
					CCDelayTime::create(12*0.06f),
					CCFadeTo::create(0.1f,255),
					NULL
					));
			}
		}
	}

	{
		//jineng_hunter_youmingzhinu_shoushang_12
		SPRITE* fangYu = SPRITE::create("jineng_hunter_youmingzhinu_shoushang_1.png");
		target->getContainer()->addChild(fangYu,target->getZOrder()+4);
		fangYu->setPosition(ccpAdd(target->getSkillFirePos(),ccp(0,60)));
		fangYu->setVisible(false);

		CCFiniteTimeAction* shoushang_action = EASY_ACTION("jineng_hunter_youmingzhinu_shoushang_%d.png",1,12,0.06f,false);

		fangYu->runAction(
			CCSequence::create(
			CCDelayTime::create(move_time+delay_before_move),
			CCShow::create(),
			shoushang_action,
			CCCallFuncN::create(target->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			)
			);
	}

}
void ClientSkillProcessorBase::skill_fight_YunShi(StepSkill step)
{
	//loadPlist("jineng_jisi_yunshi.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));


	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
	//CCAssert(step.firedBuff.size() == 0,"不支持buff");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;


	// 攻击者的动画,需要跟技能动画协调时间
	{
		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
	}

	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();

	SkillSprite* shifa1 = SkillSprite::create("jineng_jisi_yunshi_shifa_1_1.png");
	CCFiniteTimeAction* shi_fa1_action = SpriteHelper::easyCreateAction_pList("jineng_jisi_yunshi_shifa_1_%d.png",1,23,0.03f,false);
	shifa1->runAction(CCSequence::create(
		shi_fa1_action,
		CCCallFuncN::create(fightor,SEL_CallFuncN(&SkillSprite::removeNode)),
		NULL
		));
	layer->addChild(shifa1,10);
	shifa1->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));

	// 17帧的时候再施法的第二拨
	float shifa2_delay = shi_fa1_action->getDuration()*(17.0f/23.0f);
	SkillSprite* shifa2 = SkillSprite::create("jineng_jisi_yunshi_shifa_2_1.png");
	layer->addChild(shifa2,10);
	shifa2->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
	shifa2->setVisible(false);
	CCFiniteTimeAction* shi_fa2_action = SpriteHelper::easyCreateAction_pList("jineng_jisi_yunshi_shifa_2_%d.png",1,21,0.02f,false);
	shifa2->runAction(CCSequence::create(
		CCDelayTime::create(shifa2_delay),
		CCShow::create(),
		shi_fa2_action,
		CCCallFuncN::create(fightor,SEL_CallFuncN(&SkillSprite::removeNode)),
		NULL
		));

	float shoushangQianYaoDelay = shifa2_delay + shi_fa2_action->getDuration()*0.8f;
	SkillSprite* shoushangQianYao = SkillSprite::create("jineng_jisi_yunshi_shoushuang_1_1.png");
	shoushangQianYao->setVisible(false);
	shoushangQianYao->setOpacity(0);
	target->addChild(shoushangQianYao,10);
	shoushangQianYao->setPosition(target->getDizuo()->getPosition());// 放到底座
	shoushangQianYao->setOpacity(0);
	shoushangQianYao->runAction(CCSequence::create(
		CCDelayTime::create(shoushangQianYaoDelay),
		CCShow::create(),
		/*CCRepeat::create(CCSequence::create(CCFadeTo::create(0.2f,255),CCFadeTo::create(0.2f,100),NULL),2),*/
		CCSequence::create(CCFadeTo::create(0.3f,255),CCFadeTo::create(0.3f,100),NULL),
		CCCallFuncN::create(target,SEL_CallFuncN(&SkillSprite::removeNode)),
		NULL
		));

	float dizuoTouYinDelay = shoushangQianYaoDelay+0.8f;
	float touYinTime = 0.1f;
	SkillSprite* dizuoTouYin = SkillSprite::create("jineng_jisi_yunshi_baozha_yinying.png");
	dizuoTouYin->setVisible(false);
	dizuoTouYin->setScale(0.1f);
	dizuoTouYin->setPosition(target->getDizuo()->getPosition());// 放到底座
	target->addChild(dizuoTouYin,10);
	dizuoTouYin->runAction(CCSequence::create(
		CCDelayTime::create(dizuoTouYinDelay),
		CCShow::create(),
		CCScaleTo::create(touYinTime,1.0f),
		CCCallFuncN::create(target,SEL_CallFuncN(&SkillSprite::removeNode)),
		NULL
		));


	SkillSprite* shitouDiaoLuo = SkillSprite::create("jineng_jisi_yunshi_feixing.png");
	float visibleSizeHight = CCDirector::sharedDirector()->getVisibleSize().height;
	CCPoint startPos = ccp(LF::getNativePostion(target,layer).x,visibleSizeHight+shitouDiaoLuo->getContentSize().height/2.0f);
	CCPoint endPos = LF::getNativePostion(target,layer);
	float move_time = touYinTime;
	shitouDiaoLuo->setPosition(startPos);
	shitouDiaoLuo->setVisible(false);
	layer->addChild(shitouDiaoLuo,10);
	shitouDiaoLuo->runAction(CCSequence::create(
		CCDelayTime::create(dizuoTouYinDelay),
		CCShow::create(),
		CCMoveTo::create(move_time,endPos),
		CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
		CCCallFuncN::create(target,SEL_CallFuncN(&SkillSprite::removeNode)),
		NULL
		));


	//
	SkillSprite* baoZao = SkillSprite::create("jineng_jisi_yunshi_baozha_1.png");
	CCFiniteTimeAction* baozaoAction = SpriteHelper::easyCreateAction_pList("jineng_jisi_yunshi_baozha_%d.png",1,10,0.04f,false);
	baoZao->setPosition(endPos);
	layer->addChild(baoZao,10);
	baoZao->setVisible(false);
	baoZao->runAction(CCSequence::create(
		CCDelayTime::create(dizuoTouYinDelay+move_time),
		CCShow::create(),
		baozaoAction,
		CCFadeTo::create(0.3f,0),
		CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
		NULL
		));
}
void ClientSkillProcessorBase::skill_fight_ShouHu(StepSkill step)
{
	//loadPlist("jineng_qishi_shouhu.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));



	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
	// 该技能只有Buff
	CCAssert(step.firedBuff.size() > 0,"buff必须要有");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;
	//msg_data->hited_sound = "jiaHuJia_2.wav";

	fightor->runAction(CCSequence::create(
		CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
		CCRotateTo::create(0.0f,0),
		CCScaleTo::create(0.1f,1.15f),
		CCScaleTo::create(0.01f,1.0f),
		NULL
		));


	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	SkillSprite* shifa = SkillSprite::create("jineng_qishi_shouhu_shifa_1.png");
	CCFiniteTimeAction* shifaAction = SpriteHelper::easyCreateAction_pList("jineng_qishi_shouhu_shifa_%d.png",1,56,0.02f,false);
	shifa->setPosition(fightor->getNormalPos());
	shifa->runAction(CCSequence::create(
		shifaAction,
		CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
		NULL
		));
	layer->addChild(shifa,10);
	shifa->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));

	CCNode* node = CCNode::create();
	fightor->addChild(node);
	node->runAction(CCSequence::create(
		CCDelayTime::create(shifaAction->getDuration()*0.9f),
		CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
		CCCallFuncN::create(fightor,SEL_CallFuncN(&SkillSprite::removeNode)),
		NULL
		));
}
void ClientSkillProcessorBase::skill_fight_ZhongShenZhiLi(StepSkill step)
{
	CCAssert(false,"还没实现");
}
void ClientSkillProcessorBase::skill_fight_XuanFengZhan(StepSkill step)
{
	//loadPlist("jineng_youxia_xuanfengzhan.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));


	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}

	float jianFlyDelay = 0;
	float rollingTime = 0.8f; // 持续旋转0.5f秒
	float rotate = -150.0f;
	// 攻击者的动画
	{
		SkillSprite* shiFa1 = SkillSprite::create("jineng_youxia_xuanfengzhan_shifa_1_1.png");
		CCFiniteTimeAction* shifa1_action = SpriteHelper::easyCreateAction_pList("jineng_youxia_xuanfengzhan_shifa_1_%d.png",1,13,0.03f,false);
		shiFa1->runAction(CCSequence::create(
			shifa1_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		fightor->getContainer()->addChild(shiFa1,10);
		shiFa1->setPosition(fightor->getPosition());

		float xuanzhuan1Delay = shifa1_action->getDuration() * (10.0f/13.0f);
		float xuanzhuanShowTime = 0.2f;
		float xuanzhuanFadeTime = 0.2f;

		SkillSprite* xuanzhuan1 = SkillSprite::create("jineng_youxia_xuanfengzhan_shifa_2.png");
		SkillSprite* xuanzhuan2 = SkillSprite::create("jineng_youxia_xuanfengzhan_shifa_3.png");
		xuanzhuan1->addChild(xuanzhuan2);
		xuanzhuan2->setPosition(ccp(xuanzhuan1->getContentSize().width/2,xuanzhuan1->getContentSize().height/2));
		xuanzhuan1->setVisible(false);
		xuanzhuan2->setVisible(false);
		xuanzhuan2->setOpacity(0);
		xuanzhuan2->setScale(1.0f/xuanzhuan2->getInitScale()); 
		// 一直旋转
		xuanzhuan1->runAction(CCRepeat::create(CCRotateBy::create(0.1f,rotate),-1));
		xuanzhuan1->runAction(CCSequence::create(
			CCDelayTime::create(xuanzhuan1Delay),
			CCShow::create(),
			CCDelayTime::create(xuanzhuanShowTime), // 这之后，就可以慢慢看到xuanzhuan2渐渐可见
			CCFadeTo::create(xuanzhuanFadeTime,0),
			CCDelayTime::create(rollingTime),
			CCCallFuncN::create(fightor,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
		//xuanzhuan1->setScale(1.3f);

		xuanzhuan2->runAction(CCSequence::create(
			CCDelayTime::create(xuanzhuan1Delay),
			CCDelayTime::create(xuanzhuanShowTime),
			CCShow::create(),
			CCFadeTo::create(xuanzhuanFadeTime,255),
			NULL
			));

		fightor->addChild(xuanzhuan1,10);
		xuanzhuan1->setPosition(fightor->getFireNode()->getPosition());



		jianFlyDelay =  xuanzhuan1Delay+xuanzhuanShowTime+xuanzhuanFadeTime;

		fightor->runAction(CCSequence::create(
			CCDelayTime::create(xuanzhuan1Delay),
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
	}

	CCFiniteTimeAction* hertAction = SpriteHelper::easyCreateAction_pList("jineng_youxia_xuanfengzhan_shoushang_%d.png",1,20,0.02f,false);

	float distance_max = 0;	// 最远距离
	float angle_min = 360;
	float angle_max = -360;
	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
	for (int i=0;i<targetNum;i++)
	{
		CCPoint aToB = LF::getVectorFromAToB(fightor,targetVec[i]);
		float angle = LF::getRotateAngle(aToB,ccp(0,1));
		CCPoint endPoint = LF::getNativePostion(targetVec[i]->getFireNode(),layer);
		float dis = ccpDistance(endPoint,statPoint);

		if (distance_max < dis)
			distance_max = dis;

		if (angle < angle_min)
			angle_min = angle;
		if (angle > angle_max)
			angle_max = angle;
	}

	// 对于正负180度的特殊情况，我们应该选择夹角距离最小的取值
	{
		if (abs(angle_max) >= 178.0f && abs(angle_max) <= 182.0f)
		{
			if (angle_min < 0) // 如果angle_min是负的
			{
				angle_max = -180;
			}
			else
			{
				angle_max = 180;
			}
		}
		if (abs(angle_min) >= 178.0f && abs(angle_min) <= 182.0f)
		{
			if (angle_max < 0) // 如果angle_max是负的
			{
				angle_min = -180;
			}
			else
			{
				angle_min = 180;
			}
		}
		// 
		float tempMin = angle_min;
		angle_min = min(angle_min,angle_max);
		angle_max = max(tempMin,angle_max);
	}

	float angle_gap = 10.0f;
	for (int fireIndex=0;fireIndex<4;fireIndex++) // 连续3次
	{
		// 扫出一片区域
		for (float angle = angle_min - angle_gap;angle < angle_max + angle_gap;angle += angle_gap)
		{
			SkillSprite* fly = SkillSprite::create("jineng_youxia_xuanfengzhan_feixing.png");
			//fly->runAction(CCRepeat::create(CCRotateBy::create(0.1f,200.0f),-1));
			fly->setRotation(angle);
			// 扫射区域
			//float dis = distance_max/2.5f;
			float dis = distance_max * 0.8f;
			float time = dis/456.0f * 0.35f;

			CCPoint endPoint = ccpRotateByAngle(ccp(0,1),ccp(0,0),CC_DEGREES_TO_RADIANS(angle));
			endPoint.x = abs(endPoint.x) * ((angle < 0 || angle > 180) ? -1 : 1);
			endPoint.y = abs(endPoint.y) * ((angle < -90 || angle > 90) ? -1 : 1);
			endPoint.x *= dis;
			endPoint.y *= dis;
			endPoint = ccpAdd(statPoint,endPoint);

			layer->addChild(fly,10);
			fly->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
			fly->setVisible(false);

			//CCLog("endPoint = [%f,%f],angle = %f",endPoint.x,endPoint.y,angle);

			CCAction* flyAction = CCSequence::create(
				CCDelayTime::create(jianFlyDelay + fireIndex*0.15f + 0.2f*CCRANDOM_0_1()),
				CCShow::create(),
				//LFDelayToMusic::create(0.0f,"huoJian.wav"),
				CCSpawn::create(CCMoveTo::create(time,endPoint),CCFadeTo::create(time,50),NULL),
				//LFDelayToMusic::create(0,"jishudujian_03.wav"),
				CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
				NULL
				);
			fly->runAction(flyAction);
			//fly->setScale(1.3f);
		}
	}
	
	for (int i=0;i<targetNum;i++)
	{
		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;

		SkillSprite* shouShang = SkillSprite::create();
		targetVec[i]->addChild(shouShang,10);
		shouShang->setPosition(targetVec[i]->getNormalPos());
		shouShang->setVisible(false);
		shouShang->runAction(CCSequence::create(
			CCDelayTime::create(jianFlyDelay + 0.1f + 0.3f*CCRANDOM_0_1()),
			CCShow::create(),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			(CCFiniteTimeAction*)hertAction->copy()->autorelease(),
			CCFadeTo::create(0.3f,0),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));

		//shouShang->setScale(1.3f);
	}

}
void ClientSkillProcessorBase::skill_fight_ZhuiHunDuJian(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	0

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif
	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");

	Music::sharedMusicPlayer()->playEffectSound("huijinzhiyu_1.wav");

	MSG_Skill* msg_data = new MSG_Skill();
	msg_data->step = step;
	msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
	msg_data->playDefaultHithedSound = false;
	msg_data->hited_sound = "lieGong_2.wav";


	{
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),target->getFireNode());
		float angle = LF::getRotateAngle(aToB,ccp(0,1));

		SPRITE* yun_qi = SPRITE::create();
		SPRITE* jian_begin = SPRITE::create("jineng_shenjianshou_zhuihundujian_feixing_1.png");
		jian_begin->setVisible(false);
		SPRITE* jian_end = SPRITE::create("jineng_shenjianshou_zhuihundujian_feixing_2.png");
		jian_end->setVisible(false);

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(target->getFireNode(),layer);
		layer->addChild(yun_qi,10);
		layer->addChild(jian_begin,10);
		layer->addChild(jian_end,10);

		jian_begin->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
		jian_begin->setRotation(angle);
		jian_end->setRotation(angle);
		jian_end->setPosition(endPoint);
		yun_qi->setRotation(angle);
		//yun_qi->setScale(1.25f);
		yun_qi->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));

		float action_delay = 0;
		const float frame_delay = 0.1f;
		// 运气
		CCFiniteTimeAction* yun_qi_action = EASY_ACTION("jineng_shenjianshou_zhuihundujian_shifa_%d.png",1,14,frame_delay,false);
		yun_qi->setVisible(false);
		yun_qi->runAction(CCSequence::create(
			CCDelayTime::create(action_delay),
			CCShow::create(),
			yun_qi_action,
			/*CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),*/
			CCCallFuncN::create(fightor,SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));

		CCNode* hitNode = CCNode::create();
		hitNode->runAction(CCSequence::create(
			CCDelayTime::create(action_delay + 13 * frame_delay),
			LFDelayToMusic::create(0.0f,"huijinzhiyu_2.wav"),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			CCCallFuncN::create(fightor,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
		target->addChild(hitNode);


		// 显现一下
		jian_begin->runAction(CCSequence::create(
			CCDelayTime::create(action_delay + 13 * frame_delay),
			CCShow::create(),
			CCDelayTime::create(frame_delay),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));

		float fly_delay = yun_qi_action->getDuration()+ action_delay;
		// 攻击者的动画,需要跟技能动画协调时间
		{
			CCFiniteTimeAction* back = fightor->get_fire_shake_Action(fightor,target);
			CCAction* action =  CCSequence::create(
				CCDelayTime::create(fly_delay),
				back,
				NULL
				);
			fightor->runAction(action);
		}

		CCFiniteTimeAction* bomb2Action = EASY_ACTION("jineng_shenjianshou_zhuihundujian_shoushang_%d.png",1,8,frame_delay*1.1f,false);
		SPRITE* bomb2 = SPRITE::create();
		bomb2->setVisible(false);
		bomb2->runAction(CCSequence::create(
			CCDelayTime::create(fly_delay),
			CCShow::create(),
			bomb2Action,
			CCCallFuncN::create(target,SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
		bomb2->setPosition(ccpAdd(LF::getNativePostion(target,layer),ccp(0,30)));
		layer->addChild(bomb2,10);

		// 显现一下
		jian_end->runAction(CCSequence::create(
			CCDelayTime::create(fly_delay - frame_delay),
			CCShow::create(),
			CCDelayTime::create(frame_delay),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
	}

}
void ClientSkillProcessorBase::skill_fight_SiWangSaoShe(StepSkill step)
{
	//loadPlist("jineng_lieshou_siwangsaoshe.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));

	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}

	CCFiniteTimeAction* shifa2Action = SpriteHelper::easyCreateAction_pList("jineng_lieshou_siwangsaoshe_shifa_2_%d.png",1,14,0.03f,false);
	float jianFlyDelay = 0;
	float shifa2Delay = 0;
	// 攻击者的动画
	{
		SkillSprite* shiFa1 = SkillSprite::create("jineng_lieshou_siwangsaoshe_shifa_1_1.png");
		CCFiniteTimeAction* shifa1_action = SpriteHelper::easyCreateAction_pList("jineng_lieshou_siwangsaoshe_shifa_1_%d.png",1,39,0.03f,false);
		shiFa1->runAction(CCSequence::create(
			shifa1_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		fightor->getContainer()->addChild(shiFa1,10);
		shiFa1->setPosition(fightor->getPosition());

		shifa2Delay = shifa1_action->getDuration();
		jianFlyDelay =  (shifa1_action->getDuration()+shifa2Action->getDuration()) * (41.0f/49.0f);

		fightor->runAction(CCSequence::create(
			CCDelayTime::create(jianFlyDelay),
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
	}

	CCFiniteTimeAction* bombAction = SpriteHelper::easyCreateAction_pList("jineng_lieshou_siwangsaoshe_shoushang_%d.png",1,13,0.03f,false);
	
	for (int i=0;i<targetNum;i++)
	{
		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;

		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
		CCPoint endPoint = LF::getNativePostion(targetVec[i]->getFireNode(),layer);

		SkillSprite* jian = SkillSprite::create("jineng_lieshou_siwangsaoshe_feixing_1.png");
		CCPoint offset = ccp(jian->getContentSize().height/2,jian->getContentSize().height/2);
		// 技能动画
		// 得到A到B的向量，如果y<0则B在下面
		CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),targetVec[i]->getFireNode());
		float angle = LF::getRotateAngle(aToB,ccp(0,1));

		// 
		SkillSprite* shiFa2 = SkillSprite::create("jineng_lieshou_siwangsaoshe_shifa_1_1.png");
		shiFa2->setVisible(false);
		shiFa2->runAction(CCSequence::create(
			CCDelayTime::create(shifa2Delay),
			CCShow::create(),
			(CCFiniteTimeAction*)shifa2Action->copy()->autorelease(),
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		fightor->getContainer()->addChild(shiFa2,10);
		shiFa2->setPosition(fightor->getPosition());
		shiFa2->setRotation(angle);


		// 箭飞行
		float dis = ccpDistance(endPoint,statPoint);
		float time = dis/456.0f * 0.18f;

		layer->addChild(jian,10);
		jian->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
		jian->setRotation(angle);
		jian->setVisible(false);

		CCAction* fly = CCSequence::create(
			CCDelayTime::create(jianFlyDelay),
			CCShow::create(),
			//LFDelayToMusic::create(0.0f,"huoJian.wav"),
			CCSpawn::create(
			CCMoveTo::create(time,ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
			CCFadeTo::create(time,0),NULL),
			//LFDelayToMusic::create(0,"jishudujian_03.wav"),
			CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			);
		jian->runAction(fly);


		SkillSprite* shouShang = SkillSprite::create("jineng_lieshou_siwangsaoshe_shoushang_1.png");
		targetVec[i]->addChild(shouShang,10);
		shouShang->setPosition(targetVec[i]->getNormalPos());
		shouShang->setVisible(false);
		shouShang->runAction(CCSequence::create(
			CCDelayTime::create(jianFlyDelay+time),
			CCShow::create(),
			(CCFiniteTimeAction*)bombAction->copy()->autorelease(),
			CCSpawn::create(CCScaleTo::create(0.3f,2.0f),CCFadeTo::create(0.3f,0),NULL),
			CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
	}

}
void ClientSkillProcessorBase::skill_fight_HanBingCiGu(StepSkill step)
{
	CCAssert(false,"还没实现");
}
void ClientSkillProcessorBase::skill_fight_ShiHun(StepSkill step)
{
	// 控制用plist还是图片集合
#undef USE_PLIST
#define USE_PLIST	0

#if USE_PLIST == 1
	typedef SkillSprite SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction_pList
	// 载入
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));
#else
	typedef BaseSprite	SPRITE;
#define EASY_ACTION	SpriteHelper::easyCreateAction
#endif


	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}
	// 该技能只有Buff
	CCAssert(step.firedBuff.size() > 0,"buff必须要有");

	// 攻击者的动画
	{
		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));
		//jineng_magebane_shenzhizhufu_shifa_27.png
		SPRITE* fire = SPRITE::create("jineng_sorcerer_shihun_shifa_1.png");
		CCFiniteTimeAction* fire_action = EASY_ACTION("jineng_sorcerer_shihun_shifa_%d.png",1,21,0.05f,false);
		fire->runAction(CCSequence::create(
			fire_action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
			NULL
			));
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		layer->addChild(fire,10);
		fire->setPosition(ccpAdd(LF::getNativePostion(fightor->getFireNode(),layer),ccp(12,40)));
	}



	for (int i=0;i<targetNum;i++)
	{
		Music::sharedMusicPlayer()->playEffectSound("jiaHuJia_1.wav");

		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;
		msg_data->hited_sound = "jiaHuJia_2.wav";


		{
			SPRITE* shoushang = SPRITE::create("jineng_sorcerer_shihun_shoushang_1.png");
			targetVec[i]->getContainer()->addChild(shoushang,targetVec[i]->getZOrder()+4);
			shoushang->setPosition(ccpAdd(targetVec[i]->getSkillFirePos(),ccp(10,60)));
			shoushang->setVisible(false);

			CCFiniteTimeAction* dun_action = EASY_ACTION("jineng_sorcerer_shihun_shoushang_%d.png",1,24,0.04f,false);

			shoushang->runAction(
				CCSequence::create(
				CCDelayTime::create(21*0.05f + CCRANDOM_0_1() * 0.2f),//
				CCShow::create(),
				// 第19帧再hit
				CCSpawn::create(
					dun_action,
					CCSequence::create(
					CCDelayTime::create(13*0.05f),
					CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
					NULL),
					NULL),
				CCSequence::create(CCMoveBy::create(0.2f,ccp(0,20)),CCMoveBy::create(0.3f,ccp(0,-20)),NULL),
				CCSpawn::create(CCFadeTo::create(0.1f,0),CCMoveBy::create(0.1f,ccp(0,10)),NULL),
				CCCallFuncN::create(targetVec[i]->getParent(),SEL_CallFuncN(&SPRITE::removeNode)),
				NULL
				)
				);
		}
	}


}

// 技能书3
void ClientSkillProcessorBase::skill_fight_BaoNu(StepSkill step)
{
	CCAssert(false,"还没实现");
}
void ClientSkillProcessorBase::skill_fight_YinZheHun(StepSkill step)
{
	CCAssert(false,"还没实现");
}
void ClientSkillProcessorBase::skill_fight_TianTangZhiGuang(StepSkill step)
{
	//loadPlist("jineng_fashi_tiantangzhiguang.plist");
	loadPlist(SkillTree::getSkillPlistFile(step.skillType));

	// 前面这一部分是做检验
	// 每个的实现都一样
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = step.targetUniqueVec.size();
	for (int i=0;i<targetNum;i++)
	{
		ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[i]);
		CCAssert(target!=NULL,"target必须存在");
		targetVec.push_back(target);
	}


	float frame_gap = 0.025f;
	CCFiniteTimeAction* guang_action = SpriteHelper::easyCreateAction_pList("jineng_fashi_tiantangzhiguang_shoushang_%d.png",1,30,0.02f,false);
	float guang_delay = 0;
	// 攻击者的动画
	{
		SkillSprite* shifa1 = SkillSprite::create();
		CCFiniteTimeAction* shifa1_part1Action = SpriteHelper::easyCreateAction_pList("jineng_fashi_tiantangzhiguang_shifa_1_%d.png",1,36,0.05f,false);
		
		SkillSprite* photo = SkillSprite::create(
			fightor->isShuangshou()?
			"jineng_fashi_tiantangzhiguang_shifa_2_shuangshou.png":
		"jineng_fashi_tiantangzhiguang_shifa_2_danshou.png");

		SkillSprite* shifa2 = SkillSprite::create();
		CCFiniteTimeAction* shifa2Action = 
			fightor->isShuangshou()?
			SpriteHelper::easyCreateAction_pList("jineng_fashi_tiantangzhiguang_shifa_2_shuangshou_%d.png",1,16,frame_gap,false):
			SpriteHelper::easyCreateAction_pList("jineng_fashi_tiantangzhiguang_shifa_2_danshou_%d.png",1,16,frame_gap,false);
		CCFiniteTimeAction* shifa3Action = SpriteHelper::easyCreateAction_pList("jineng_fashi_tiantangzhiguang_shifa_3_%d.png",1,7,frame_gap,false);

		shifa2->setVisible(false);
		photo->setVisible(false);
		photo->setOpacity(0);

		fightor->runAction(CCSequence::create(
			CCRepeat::create(CCSequence::create(CCRotateTo::create(0.01f,-20.0f),CCRotateTo::create(0.01f,20.0f),NULL),3),
			CCRotateTo::create(0.0f,0),
			CCScaleTo::create(0.1f,1.15f),
			CCScaleTo::create(0.01f,1.0f),
			NULL
			));

		float shifa1_wait_for_shifa2 = shifa2Action->getDuration()*(7.0f/16.0f);
		BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
		shifa1->runAction(CCSequence::create(
			shifa1_part1Action,
			CCDelayTime::create(shifa1_wait_for_shifa2),// 等待图片涨到施法2的圆圈处,就可以播放施法3的了
			shifa3Action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		layer->addChild(shifa1,10);
		shifa1->setPosition(ccpAdd(LF::getNativePostion(fightor->getFireNode(),layer),ccp(0,-20)));

		shifa2->runAction(CCSequence::create(
			CCDelayTime::create(shifa1_part1Action->getDuration()),
			CCShow::create(),
			shifa2Action,
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
		shifa2->setPosition(shifa1->getPosition());
		layer->addChild(shifa2,10);

		photo->setPosition(shifa1->getPosition());
		layer->addChild(photo,9);
		photo->runAction(CCSequence::create(
			CCDelayTime::create(shifa1_part1Action->getDuration()),
			CCShow::create(),
			CCFadeTo::create(shifa2Action->getDuration(),255),
			CCDelayTime::create(0.5f+guang_action->getDuration()),
			CCFadeTo::create(0.5f,0),
			CCCallFuncN::create(fightor->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));

		guang_delay = shifa1_part1Action->getDuration()+shifa2Action->getDuration() + shifa3Action->getDuration();

		vector<CCSprite*> figterBodyParts = fightor->getItemParts(Part_Body);
		int partNum = figterBodyParts.size();
		for (int i=0;i<partNum;i++)
		{
			figterBodyParts[i]->runAction(CCSequence::create(
				CCDelayTime::create(shifa1_part1Action->getDuration()),
				CCFadeTo::create(shifa2Action->getDuration(),0), // 跟 photo同步逆向
				CCDelayTime::create(0.5f+guang_action->getDuration()),
				CCFadeTo::create(0.5f,255),
				NULL
				));
		}

	}

	for (int i=0;i<targetNum;i++)
	{
		//Music::sharedMusicPlayer()->playEffectSound("jiaHuJia_1.wav");

		MSG_Skill* msg_data = new MSG_Skill();
		msg_data->step = getOneFightUniteByIndex(step,i);
		msg_data->fightor = getItemFromBoard(step.fightorUniqueId);
		msg_data->playDefaultHithedSound = false;
		//msg_data->hited_sound = "jiaHuJia_2.wav";

		// 是攻击者自身，那么就是加buff
		if (targetVec[i] == fightor)
		{
			CCNode* temp = CCNode::create();
			fightor->addChild(temp);
			temp->runAction(
				CCSequence::create(
				CCDelayTime::create(guang_delay),
				CCShow::create(),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCCallFuncN::create(targetVec[i]->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
				NULL
				));
			continue;
		}


		SkillSprite* guang = SkillSprite::create("jineng_fashi_tiantangzhiguang_shoushang_1.png");
		targetVec[i]->getContainer()->addChild(guang,targetVec[i]->getZOrder()+4);
		guang->setPosition(ccpAdd(targetVec[i]->getSkillFirePos(),ccp(0,-5)));
		guang->setVisible(false);

		guang->setScale(1.2f);
		guang->runAction(
			CCSequence::create(
			CCDelayTime::create(guang_delay),
			CCShow::create(),
			CCSpawn::create(
				(CCFiniteTimeAction*)guang_action->copy()->autorelease(),
				CCSequence::create(
					CCDelayTime::create(guang_action->getDuration()*(20.0f/30.0f)), // 第20帧开始
					CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
					NULL),
				NULL),
			CCCallFuncN::create(targetVec[i]->getParent(),SEL_CallFuncN(&SkillSprite::removeNode)),
			NULL
			));
	}

}


void ClientSkillProcessorBase::skill_fight_Monster_XieE(StepSkill step)
{
	step.skillType = SkillType_JiSuDuJian;
	skill_fight_JiSuDuJian(step); // 暂时用其他效果
}

void ClientSkillProcessorBase::playEffectSound(CCNode* node,CCObject* sound_data)
{
	MSG_SOUND* msg_data = (MSG_SOUND*)sound_data;
	if (msg_data->sound_file != "")
		Music::sharedMusicPlayer()->playEffectSound(msg_data->sound_file);

	msg_data->release();
}

void ClientSkillProcessorBase::skill_Hit(CCNode* node,CCObject* msg_skill)
{
	MSG_Skill* msg_data = (MSG_Skill*)msg_skill;

	HeroFixedPro pro = BaseActorPropertyParser::getParser()->getExtraProerty(msg_data->fightor->getProfession());
	if (msg_data->playDefaultHithedSound)
	{
		if (pro.mShangHaiLeiXing == SHANG_HAI_WU_LI)
		{
			Music::sharedMusicPlayer()->playEffectSound("wuLiGongJi.wav");
		}else
		{
			Music::sharedMusicPlayer()->playEffectSound("moFaGongJi.wav");
		}
	}

	if (msg_data->hited_sound != "")
	{
		Music::sharedMusicPlayer()->playEffectSound(msg_data->hited_sound);
	}

	// 减血
	int num = msg_data->step.targetUniqueVec.size();
	if (num > 1)
	{
		CCLog("waring ： this skill hit contains more than one target,make sure this is your want!");
		CCAssert(false,"预防错误，因为msg_data为release多次， 调用：getOneFightUniteByIndex进行分解");
	}

	ClientChessBoardItemBase* target = getItemFromBoard(msg_data->step.targetUniqueVec[0]);
	//CCAssert(target!=NULL,"");
	// 由于快速的攻击，target在这次技能到达的时候，已经在之前被移除了
	// 所有这里不Assert target必须存在
	if (target)
	{
		if (msg_data->step.canMissVec[0])
		{
			target->missAction(target);
		}else
		{
			target->hpAdd(msg_data->step.targetBloodChange[0]);
			// 击退
			target->shake_attacked(msg_data->fightor,target);
			// 附加buff
			int buffNum = msg_data->step.firedBuff.size();
			for (int buffIndex = 0; buffIndex < buffNum; buffIndex++)
			{
				CCAssert(msg_data->step.firedBuff[buffIndex].attachedToUId == target->getUniqueId(),"buff一定只能打向Target");
				target->attachBuff(msg_data->step.firedBuff[buffIndex]);
			}
		}
	}
	msg_data->release();
}

ClientSkillProcessorBase::~ClientSkillProcessorBase(void)
{

	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(SKILL_HENGFU_FILE);
	//所有的技能和横幅都在这里释放
	//一场战斗会有很多技能、 所以全部在这里释放。
	const vector<SkillConfigInfor>* pVec = SkillConfigManager::getManager()->getAllSkillInfo();
	for (int i=0,count=pVec->size(); i<count; ++i)
	{
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(
			CCString::createWithFormat(SKILL_FILE_FORMAT, pVec->at(i).armatureName.c_str(), pVec->at(i).armatureName.c_str())->getCString());
	}
}


class AutoSkillData : public CCObject
{
public:
	StepSkill mSkill;
	static AutoSkillData* create(const StepSkill& ski){
		AutoSkillData * p = new AutoSkillData(ski);
		p->autorelease();
		return p;
	}
	AutoSkillData(const StepSkill& ski){
		mSkill = ski;
	}
};

void ClientSkillProcessorBase::skill_fight_prepare(StepSkill step)
{
	//0. 检验
	ClientChessBoardItemBase* target = getItemFromBoard(step.targetUniqueVec[0]);
	CCAssert(target!=NULL,"target必须存在");
	ClientChessBoardItemBase* fightor = getItemFromBoard(step.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	//CCAssert(step.targetUniqueVec.size() == 1,"只支持单体攻击");
	//CCAssert(step.firedBuff.size() == 0,"不支持buff");

	//1. 播放音效

	//2. 横幅动画
	//todo switch
	const SkillConfigInfor* skill_info = SkillConfigManager::getManager()->getOneSkillInfo(step.skillType);
	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	if (layer)
	{
		CCArmature *armature = CCArmature::create(SKILL_HENGFU);
		armature->getAnimation()->play(skill_info->armatureName.c_str());
		armature->getAnimation()->setMovementEventCallFunc(this, 
			SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
		armature->setPosition(mWinSize.width/2, mWinSize.height/2);
		layer->getParent()->addChild(armature,layer->getZOrder()+10);

		//3. skill动画准备
		AutoSkillData *data = AutoSkillData::create(step);
		data->retain();//有target动画完成后删除

		CCCallFuncND *pFunc = NULL;
		if (step.skillType == SkillType_LiRenHuiXuan)
		{
			pFunc = CCCallFuncND::create(this, callfuncND_selector(ClientSkillProcessorBase::skill_fightArmatureBegin_LiRenHuiXuan), (void*)data);
		}
		else if (step.skillType == SkillType_HuoQiuShu)
		{
			pFunc = CCCallFuncND::create(this, callfuncND_selector(ClientSkillProcessorBase::skill_fightArmatureBegin_HuoQiuShu), (void*)data);
		}
		else 
		{
			pFunc = CCCallFuncND::create(this, callfuncND_selector(ClientSkillProcessorBase::skill_fightArmatureBegin), (void*)data);
		}

		layer->runAction(CCSequence::create(CCDelayTime::create(0.75f + 0.4f), pFunc, NULL));
	}
}
void ClientSkillProcessorBase::skill_fightArmatureBegin(CCNode *node, void* data)
{
	AutoSkillData *skillData = (AutoSkillData *)data;


	const SkillConfigInfor* skill_info = SkillConfigManager::getManager()->getOneSkillInfo(skillData->mSkill.skillType);
	//skill
	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	ClientChessBoardItemBase* fightor = getItemFromBoard(skillData->mSkill.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = skillData->mSkill.targetUniqueVec.size();
	for (int i=0; i<targetNum; ++i){
		targetVec.push_back(getItemFromBoard(skillData->mSkill.targetUniqueVec[i]));
	}
	CCAssert(targetVec.size()>0,"target必须存在");
	if (layer)
	{
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(
			CCString::createWithFormat(SKILL_FILE_FORMAT, skill_info->armatureName.c_str(), skill_info->armatureName.c_str())->getCString());


		CCPoint offset = getSkillFightorPosOffset((SkillType)skill_info->ID, true);
		//1. shi fa
		Music::sharedMusicPlayer()->playEffectSound(SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 1));
		CCArmature *armature_shifa = CCArmature::create(skill_info->armatureName.c_str());
		CCLOG(" ------ %s ------", skill_info->armatureName.c_str());
		armature_shifa->setScale(1 / 0.4f);
		//boss skill
		if (skillData->mSkill.skillType == SkillType_BOSS_Flog ||
			skillData->mSkill.skillType == SkillType_BOSS_Sweep)
		{
			ClientChessBoard *board = ((ClientChessBoardItem*)targetVec[0])->getOriginBoard();
			layer->addChild(armature_shifa, 20);
			if (skillData->mSkill.skillType == SkillType_BOSS_Sweep)
				armature_shifa->setPosition(ccp(board->getCenterWorldPoint().x, board->getGridPosInWorld(0, ((ClientChessBoardItem*)targetVec[0])->getGridY()).y));
			else if (skillData->mSkill.skillType == SkillType_BOSS_Flog)
				armature_shifa->setPosition(ccpAdd(board->getCenterWorldPoint(), ccp(0, 0)));
			armature_shifa->getAnimation()->setMovementEventCallFunc(this, 
				SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
			armature_shifa->getAnimation()->play(SKILL_ShiFa);
		}
		else //hero skill
		{
			armature_shifa->setPosition(ccpAdd(fightor->getSkillFirePos(),offset));
			fightor->addChild(armature_shifa,10);
			armature_shifa->getAnimation()->setMovementEventCallFunc(this, 
				SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
			armature_shifa->getAnimation()->play(SKILL_ShiFa);
		}


		unsigned int maxFrameCounts = armature_shifa->getAnimation()->getRawDuration();
		//
		float everyOneDelayTime = 0.15f;
		int targetNum = skillData->mSkill.targetUniqueVec.size();
		for (int i=0;i<targetNum;i++)
		{
			CCPoint vec = LF::getVectorFromAToB(targetVec.at(i),fightor);
			//2. shou shang
			if (armature_shifa->getAnimation()->getAnimationData()->getMovement(SKILL_ShouShang))
			{
				CCArmature *armature_shoushang = CCArmature::create(skill_info->armatureName.c_str());
				offset = getSkillFightorPosOffset((SkillType)skill_info->ID, false);
				armature_shoushang->setPosition(ccpAdd(targetVec.at(i)->getNormalPos(),offset));
				targetVec.at(i)->addChild(armature_shoushang,vec.y > 0?2:1); // 上面打下面的情况要盖住刀光
				armature_shoushang->setScale(1 / 0.4f);
				armature_shoushang->getAnimation()->setMovementEventCallFunc(this, 
					SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
				//armature_shoushang->getAnimation()->play(SKILL_ShouShang);
				armature_shoushang->setVisible(false);
				CCString *animateName = CCString::create(SKILL_ShouShang);
				animateName->retain();
				armature_shoushang->runAction(CCSequence::create(
					CCDelayTime::create(everyOneDelayTime * i),
					CCCallFuncND::create(this, callfuncND_selector(ClientSkillProcessorBase::callback_playAnimation), (void*)animateName),
					NULL));

			}

			//3. fei xing (刀光)   //特殊不同的技能
			if (armature_shifa->getAnimation()->getAnimationData()->getMovement(SKILL_FeiXing))
			{
				CCArmature *armature_feixing = CCArmature::create(skill_info->armatureName.c_str());
				offset = getSkillFightorPosOffset((SkillType)skill_info->ID, false);
				armature_feixing->setPosition(ccpAdd(targetVec.at(i)->getNormalPos(),offset));
				targetVec.at(i)->addChild(armature_feixing, 1); // 上面打下面的情况要盖住刀光
				armature_feixing->setScale(1 / 0.4f);
				armature_feixing->getAnimation()->setMovementEventCallFunc(this, 
					SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
				//armature_feixing->getAnimation()->play(SKILL_FeiXing);
				armature_feixing->setVisible(false);
				CCString *animateName = CCString::create(SKILL_FeiXing);
				animateName->retain();
				armature_feixing->runAction(CCSequence::create(
					CCDelayTime::create(everyOneDelayTime * i),
					CCCallFuncND::create(this, callfuncND_selector(ClientSkillProcessorBase::callback_playAnimation), (void*)animateName),
					NULL));
				if (vec.y > 0){
					armature_feixing->setRotation(180);
				}

			}


			{
				float shifa_time = armature_shifa->getAnimation()->getRawDuration() / float(60);
				MSG_Skill* msg_data = new MSG_Skill();
				msg_data->step = getOneFightUniteByIndex(skillData->mSkill,i);
				msg_data->fightor = getItemFromBoard(skillData->mSkill.fightorUniqueId);
				msg_data->playDefaultHithedSound = false;
				CCNode* hit = CCNode::create();
				hit->runAction(CCSequence::create(
					//CCDelayTime::create(/*shifa_time + */maxFrameCounts/float(60)*0.5f + everyOneDelayTime*i),
					CCDelayTime::create(skill_info->animationHitFrameNum / 60.0f + everyOneDelayTime*i),
					LFDelayToMusic::create(0.0f,SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 2)),
					CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
					CCRemoveSelf::create(),
					NULL
					));
				targetVec.at(i)->addChild(hit);
			}
		}

		float postmsg_delay_time = maxFrameCounts / float(60) + everyOneDelayTime*targetNum + 0.15f;
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(ClientSkillProcessorBase::callfunc_postSkillEnd), this,
			0, 0, postmsg_delay_time, false);
	}


	skillData->release();
}

void ClientSkillProcessorBase::skill_fightArmatureBegin_LiRenHuiXuan(CCNode *node, void* data)
{
	AutoSkillData *skillData = (AutoSkillData *)data;


	const SkillConfigInfor* skill_info = SkillConfigManager::getManager()->getOneSkillInfo(skillData->mSkill.skillType);
	//skill
	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	ClientChessBoardItemBase* fightor = getItemFromBoard(skillData->mSkill.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = skillData->mSkill.targetUniqueVec.size();
	for (int i=0; i<targetNum; ++i){
		targetVec.push_back(getItemFromBoard(skillData->mSkill.targetUniqueVec[i]));
	}
	CCAssert(targetVec.size()>0,"target必须存在");
	if (layer)
	{
		unsigned int maxFrameCounts = 0;

		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(
			CCString::createWithFormat(SKILL_FILE_FORMAT, skill_info->armatureName.c_str(), skill_info->armatureName.c_str())->getCString());


		CCPoint offset = getSkillFightorPosOffset((SkillType)skill_info->ID, true);
		//1. shi fa
		Music::sharedMusicPlayer()->playEffectSound(SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 1));
		CCArmature *armature_shifa = CCArmature::create(skill_info->armatureName.c_str());
		CCLOG(" ------ %s ------", skill_info->armatureName.c_str());
		armature_shifa->setScale(1 / 0.4f);
		{
			armature_shifa->setPosition(ccpAdd(fightor->getSkillFirePos(),offset));
			fightor->addChild(armature_shifa,10);
			armature_shifa->getAnimation()->setMovementEventCallFunc(this, 
				SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
			armature_shifa->getAnimation()->play(SKILL_ShiFa);
		}

		//3. fei xing
		CCArmature *armature_feixing = CCArmature::create(skill_info->armatureName.c_str());
		armature_feixing->setScale(1 / 0.4f);
		armature_feixing->getAnimation()->setMovementEventCallFunc(this, 
			SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
		armature_feixing->setVisible(false);

		int targetNum = skillData->mSkill.targetUniqueVec.size();
		if (targetNum == 1)
		{
			MSG_Skill* msg_data = new MSG_Skill();
			msg_data->step = getOneFightUniteByIndex(skillData->mSkill,0);
			msg_data->fightor = getItemFromBoard(skillData->mSkill.fightorUniqueId);
			msg_data->playDefaultHithedSound = false;
//			msg_data->hited_sound = "jiaHuJia_2.wav";

			BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
			CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
			CCPoint endPoint = LF::getNativePostion(targetVec[0]->getFireNode(),layer);
			endPoint = ccpAdd(endPoint,ccp(0,0));

			CCPoint offset = ccp(armature_feixing->getContentSize().height/2,0);
			// 技能动画
			// 得到A到B的向量，如果y<0则B在下面
			CCPoint aToB = LF::getVectorFromAToB(fightor,targetVec[0]);
			// 箭飞行
			float dis = ccpDistance(endPoint,statPoint);
			float time = dis/456.0f * 0.3f;
			// 延长飞行的距离
			CCPoint endPointExtern = ccpAdd(endPoint,ccp(aToB.x * 180.0f,aToB.y*180.0f));

			layer->addChild(armature_feixing,10);
			armature_feixing->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
			CCString *animateName = CCString::create(SKILL_FeiXing);
			animateName->retain();

			CCPoint moveEnd = ccpSub(endPoint,ccp(offset.x * aToB.x,offset.y * aToB.y));
			CCPoint moveEnd2 = ccpSub(endPointExtern,ccp(offset.x * aToB.x,offset.y * aToB.y));
			CCAction* fly = CCSequence::create(
				CCDelayTime::create(17.0f/60),
				CCShow::create(),
				CCCallFuncND::create(this, callfuncND_selector(ClientSkillProcessorBase::callback_playAnimation), (void*)animateName),
				LFDelayToMusic::create(0.0f,SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 2)),
				CCMoveTo::create(time,moveEnd),
				LFDelayToMusic::create(0.0f,SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 3)),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),
				CCMoveTo::create(time,moveEnd2),
				CCSpawn::create(
				CCMoveTo::create(time*2.3f,ccpSub(statPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
				CCFadeTo::create(time*2.3f,50),NULL),
				CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
				NULL
				);
			armature_feixing->runAction(fly);
			//feixing->runAction(CCRepeat::create(CCRotateBy::create(0.1f,720),-1));
		}
		else if (targetNum == 2)
		{
			BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
			CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
			CCPoint endPoint0 = LF::getNativePostion(targetVec[0]->getFireNode(),layer);
			CCPoint endPoint1 = LF::getNativePostion(targetVec[1]->getFireNode(),layer);
			CCPoint centerPoint = ccpMidpoint(endPoint0,endPoint1);

			float dis0 = ccpDistance(endPoint0,statPoint);
			float dis1 = ccpDistance(endPoint1,statPoint);
			float disFromCenter = ccpDistance(centerPoint,statPoint);
			float dis_max = dis0 >= dis1 ? dis0 : dis1;

			// 近的就是0
			MSG_Skill* msg_data0 = new MSG_Skill();
			msg_data0->step = getOneFightUniteByIndex(skillData->mSkill,dis0 < dis1?0:1);
			msg_data0->fightor = getItemFromBoard(skillData->mSkill.fightorUniqueId);
			msg_data0->playDefaultHithedSound = false;
//			msg_data0->hited_sound = "jiaHuJia_2.wav";

			// 远的就是1
			MSG_Skill* msg_data2 = new MSG_Skill();
			msg_data2->step = getOneFightUniteByIndex(skillData->mSkill,dis0 >= dis1?0:1);
			msg_data2->fightor = getItemFromBoard(skillData->mSkill.fightorUniqueId);
			msg_data2->playDefaultHithedSound = false;
//			msg_data2->hited_sound = "jiaHuJia_2.wav";

			// 需要延长的距离
			float dis_need_add = dis_max>disFromCenter?dis_max-disFromCenter:0;
			// 实际该打到的点
			CCPoint endPos = ccpAdd(centerPoint, ccpMult(ccp(centerPoint.x - statPoint.x,centerPoint.y - statPoint.y),dis_need_add/dis_max) );


			CCPoint offset = ccp(armature_feixing->getContentSize().height/2,0);
			// 技能动画
			// 得到A到B的向量，如果y<0则B在下面
			CCPoint aToB = ccpNormalize(ccpSub(endPos,statPoint));
			// 箭飞行
			float dis = ccpDistance(endPos,statPoint);
			float time = dis/456.0f * 0.4f;

			layer->addChild(armature_feixing,10);
			armature_feixing->setPosition(LF::getNativePostion(fightor->getFireNode(),layer));
			CCString *animateName = CCString::create(SKILL_FeiXing);
			animateName->retain();

			CCPoint moveEnd = ccpSub(endPos,ccp(offset.x * aToB.x,offset.y * aToB.y));
			CCAction* fly = CCSequence::create(
				CCDelayTime::create(17.0f/60),
				CCShow::create(),
				CCCallFuncND::create(this, callfuncND_selector(ClientSkillProcessorBase::callback_playAnimation), (void*)animateName),
				LFDelayToMusic::create(0.0f,SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 2)),
				CCMoveTo::create(time/2,ccpMidpoint(statPoint,moveEnd)),
				LFDelayToMusic::create(0.0f,SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 3)),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data0),
				CCMoveTo::create(time/2,moveEnd),
				LFDelayToMusic::create(0.0f,SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 3)),
				CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data2),
				CCSpawn::create(
				CCMoveTo::create(time*1.3f,ccpSub(statPoint,ccp(offset.x * aToB.x,offset.y * aToB.y))),
				CCFadeTo::create(time*1.3f,50),NULL),
				CCCallFuncN::create(layer,SEL_CallFuncN(&BaseLayer::removeNode)),
				NULL
				);
			armature_feixing->runAction(fly);
//			feixing->runAction(CCRepeat::create(CCRotateBy::create(0.1f,720),-1));
		}
		else
		{
			CCAssert(false,"不支持");
		}

		//
		if (maxFrameCounts < armature_shifa->getAnimation()->getRawDuration())
			maxFrameCounts = armature_shifa->getAnimation()->getRawDuration();
		float postmsg_delay_time = 75.0f / float(60);
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(ClientSkillProcessorBase::callfunc_postSkillEnd), this,
			0, 0, postmsg_delay_time, false);
	}


	skillData->release();
}
void ClientSkillProcessorBase::skill_fightArmatureBegin_HuoQiuShu(CCNode *node, void* data)
{
	AutoSkillData *skillData = (AutoSkillData *)data;


	const SkillConfigInfor* skill_info = SkillConfigManager::getManager()->getOneSkillInfo(skillData->mSkill.skillType);
	//skill
	BaseLayer* layer = GameDirector::getDirector()->getFightLayer();
	ClientChessBoardItemBase* fightor = getItemFromBoard(skillData->mSkill.fightorUniqueId);
	CCAssert(fightor!=NULL,"fightor必须存在");
	vector<ClientChessBoardItemBase*> targetVec;
	int targetNum = skillData->mSkill.targetUniqueVec.size();
	for (int i=0; i<targetNum; ++i){
		targetVec.push_back(getItemFromBoard(skillData->mSkill.targetUniqueVec[i]));
	}
	CCAssert(targetVec.size()>0,"target必须存在");
	if (layer)
	{
		unsigned int maxFrameCounts = 0;

		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(
			CCString::createWithFormat(SKILL_FILE_FORMAT, skill_info->armatureName.c_str(), skill_info->armatureName.c_str())->getCString());


		CCPoint offset = getSkillFightorPosOffset((SkillType)skill_info->ID, true);
		//1. shi fa
		Music::sharedMusicPlayer()->playEffectSound(SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 1));
		CCArmature *armature_shifa = CCArmature::create(skill_info->armatureName.c_str());
		CCLOG(" ------ %s ------", skill_info->armatureName.c_str());
		armature_shifa->setScale(1 / 0.4f);
		{
			armature_shifa->setPosition(ccpAdd(fightor->getSkillFirePos(),offset));
			fightor->addChild(armature_shifa,10);
			armature_shifa->getAnimation()->setMovementEventCallFunc(this, 
				SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
			armature_shifa->getAnimation()->play(SKILL_ShiFa);
		}


		//
		float everyOneDelayTime = 0.15f;
		int targetNum = skillData->mSkill.targetUniqueVec.size();
		for (int i=0;i<targetNum;i++)
		{
			CCPoint vec = LF::getVectorFromAToB(targetVec.at(i),fightor);
			//2. shou shang
			if (armature_shifa->getAnimation()->getAnimationData()->getMovement(SKILL_ShouShang))
			{
				CCArmature *armature_shoushang = CCArmature::create(skill_info->armatureName.c_str());
				offset = getSkillFightorPosOffset((SkillType)skill_info->ID, false);
				//targetVec.at(i)->addChild(armature_shoushang,vec.y > 0?2:1); // 上面打下面的情况要盖住刀光
				layer->addChild(armature_shoushang, 11); 
				//armature_shoushang->setPosition(ccpAdd(targetVec.at(i)->getNormalPos(),offset));
				armature_shoushang->setPosition(LF::getNativePostion(targetVec.at(i)->getFireNode(),layer));
				armature_shoushang->setScale(1 / 0.4f);
				armature_shoushang->getAnimation()->setMovementEventCallFunc(this, 
					SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
				//armature_shoushang->getAnimation()->play(SKILL_ShouShang);
				armature_shoushang->setVisible(false);
				CCString *animateName = CCString::create(SKILL_ShouShang);
				animateName->retain();
				armature_shoushang->runAction(CCSequence::create(
					CCDelayTime::create(everyOneDelayTime * i),
					CCCallFuncND::create(this, callfuncND_selector(ClientSkillProcessorBase::callback_playAnimation), (void*)animateName),
					NULL));

				maxFrameCounts = armature_shoushang->getAnimation()->getRawDuration();
			}

			//3. fei xing (刀光) 
			if (armature_shifa->getAnimation()->getAnimationData()->getMovement(SKILL_FeiXing))
			{
				CCArmature *armature_feixing = CCArmature::create(skill_info->armatureName.c_str());

				CCPoint aToB = LF::getVectorFromAToB(fightor->getFireNode(),targetVec.at(i)->getFireNode());
				float angle = LF::getRotateAngle(aToB,ccp(0,1));
				CCPoint statPoint = LF::getNativePostion(fightor->getFireNode(),layer);
				CCPoint endPoint = LF::getNativePostion(targetVec.at(i)->getFireNode(),layer);
				layer->addChild(armature_feixing,10);
				armature_feixing->setPosition(ccpAdd(LF::getNativePostion(fightor->getFireNode(),layer),ccp(0,0)));
				armature_feixing->setRotation(angle);
				armature_feixing->setVisible(false);
				// 箭飞行
				float dis = ccpDistance(endPoint,statPoint);
				float time = dis/456.0f * 0.2f;
				armature_feixing->getAnimation()->setSpeedScale(time >= 0.2f ? 1 : 2);
				CCPoint off = ccp(armature_feixing->getContentSize().height/2,armature_feixing->getContentSize().height/2);
				armature_feixing->setScale(1 / 0.4f);
				armature_feixing->getAnimation()->setMovementEventCallFunc(this, 
					SEL_MovementEventCallFunc(&ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd));
				//armature_feixing->getAnimation()->play(SKILL_FeiXing);
				armature_feixing->setVisible(false);
				CCString *animateName = CCString::create(SKILL_FeiXing);
				animateName->retain();

					MSG_Skill* msg_data = new MSG_Skill();
					msg_data->step = getOneFightUniteByIndex(skillData->mSkill,i);
					msg_data->fightor = getItemFromBoard(skillData->mSkill.fightorUniqueId);
					msg_data->playDefaultHithedSound = false;

				armature_feixing->runAction(CCSequence::create(
					CCDelayTime::create(everyOneDelayTime * i + 40 / 60.0f),

					CCSpawn::create(
					CCCallFuncND::create(this, callfuncND_selector(ClientSkillProcessorBase::callback_playAnimation), (void*)animateName),
					CCSequence::create(CCMoveTo::create(time,ccpSub(endPoint,ccp(off.x * aToB.x,off.y * aToB.y))),
						LFDelayToMusic::create(0.0f,SkillConfigManager::getManager()->getMusicFile(skill_info->ID, 2)),
						CCCallFuncND::create(this,SEL_CallFuncND(&ClientSkillProcessorBase::skill_Hit),msg_data),NULL), 
					NULL),
					
					NULL));

				if (maxFrameCounts < armature_feixing->getAnimation()->getRawDuration())
					maxFrameCounts = armature_feixing->getAnimation()->getRawDuration();
			}
		}

		//
		if (maxFrameCounts < armature_shifa->getAnimation()->getRawDuration())
			maxFrameCounts = armature_shifa->getAnimation()->getRawDuration();
		float postmsg_delay_time = maxFrameCounts / float(60) + everyOneDelayTime*targetNum + 0.15f;
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(ClientSkillProcessorBase::callfunc_postSkillEnd), this,
			0, 0, postmsg_delay_time, false);
	}


	skillData->release();
}

void ClientSkillProcessorBase::callback_playAnimation(CCNode* armature, void * name)
{
	CCArmature* ar = dynamic_cast<CCArmature*>(armature);
	CCString *str = (CCString*)(name);
	if (ar && name){
		ar->setVisible(true);
		ar->getAnimation()->play(str->getCString());
		str->release();
	}
}

void ClientSkillProcessorBase::callfunc_RemoveWhenAnimateEnd(CCArmature *ar, MovementEventType type, const char *ch)
{
	if (type == COMPLETE || type == LOOP_COMPLETE) {
		ar->removeFromParent();
	}
}
void ClientSkillProcessorBase::callfunc_postSkillEnd(float dt)
{
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSGFight_Skill_UseEnd);
}

//fightor true
//target false
CCPoint ClientSkillProcessorBase::getSkillFightorPosOffset(SkillType type, bool fightOrTarget)
{
	return CCPointZero;

	CCPoint offset = CCPointZero;
	switch (type)
	{
// 	case SkillType_BaoLieZhan:
// 	case SkillType_LieGong:
// 	case SkillType_JianYu:
// 	case SkillType_WenYi:
// 	case SkillType_LiRenHuiXuan:
// 	case SkillType_HuoQiuShu:
// 	case SkillType_ShaLu:
// 	case SkillType_ShanDian:
// 	case SkillType_JianShou:
// 	case SkillType_ShenZhiZhuFu:
	case SkillType_BaoLieZhan:
		offset = ccp(0,0);
		break;
	case SkillType_ShaLu:
		offset = ccp(0,0);
		break;
	case SkillType_ShanDian:
		if (fightOrTarget) offset = ccp(0,150);
		else offset = ccp(0,100);
		break;
	case SkillType_JianShou:
		offset = ccp(0,0);
		break;
	case SkillType_ShenZhiXingNian:
		break;
	case SkillType_ShunYinTuXi:
		break;
	case SkillType_LieGong:
		offset = ccp(0,0);
		break;
	case SkillType_JianYu:
		if (fightOrTarget) offset = ccp(0,180);
		else offset = ccp(0,120);
		break;
	case SkillType_HuoQiuShu:
		offset = ccp(0,0);
		break;
	case SkillType_WenYi:
		offset = ccp(0,0);
		break;
	case SkillType_BanYueZhan:
		break;
	case SkillType_CiSha:
		break;
	case SkillType_LeiBao:
		break;
	case SkillType_MangCi:
		break;
	case SkillType_ShenZhiZhuFu:
		offset = ccp(0,0);
		break;
	case SkillType_LiRenHuiXuan:
		offset = ccp(0,0);
		break;
	case SkillType_JingZhunZhiCan:
		break;
	case SkillType_JiSuDuJian:
		break;
	case SkillType_BaoFengXue:
		break;
	case SkillType_KongJu:
		break;
	case SkillType_KuangBao:
		break;
	case SkillType_ShiXueZhiRen:
		break;
	case SkillType_YunShi:
		break;
	case SkillType_ShouHu:
		break;
	case SkillType_ZhongShenZhiLi:
		break;
	case SkillType_XuanFengZhan:
		break;
	case SkillType_ZhuiHunDuJian:
		break;
	case SkillType_SiWangSaoShe:
		break;
	case SkillType_HanBingCiGu:
		break;
	case SkillType_ShiHun:
		break;
	case SkillType_BaoNu:
		break;
	case SkillType_YinZheHun:
		break;
	case SkillType_TianTangZhiGuang:
		break;
	case SkillType_Monster_XieE:
		break;
	case SkillType_BOSS_Sweep:
		break;
	case SkillType_BOSS_Flog:
		break;
	default:
		break;
	}

	return offset;
}