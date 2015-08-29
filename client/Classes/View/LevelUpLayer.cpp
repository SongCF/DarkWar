#include "LevelUpLayer.h"
#include "ImageManager.h"
#include "BaseSprite.h"
#include "MyselfManager.h"
#include "font.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "CmdHelper.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "FightSettlementLayer.h"
#include "History.h"
#include "GuideLayerBase.h"
#include "UnblockManager.h"

#define Label_Size 25

enum
{
	LiLiang = 0,
	MinJie  = 1,
	ZhiLi   = 2,
	TiNeng  = 3,

	Tag_MenuItem_Ok,
	Tag_MenuItem_Cancel,
	Tag_Label_CaiLiaoBuZu,
};


LevelUpLayer *LevelUpLayer::create(unsigned int hero_id, LevelUpEndCallBackInterface *func, bool fightEndDisplay)
{
	if (MyselfManager::getManager()->getMyTeamActor(hero_id)->getCurrentProperty().level >= 30)
	{
		return NULL;
	}


	LevelUpLayer *layer = new LevelUpLayer;
	if(layer && layer->init(hero_id, func, fightEndDisplay))
	{
		layer->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(layer);
		layer = NULL;
	}

	return layer;
}

bool LevelUpLayer::init(unsigned int hero_id, LevelUpEndCallBackInterface *func, bool fightEndDisplay)
{
	mAllCaiLiaoNumLabel = NULL; //刷新材料个数
	mNextLayer = NULL;
	mDisplayerEnd = false;
	mHeroId = hero_id;
	mCallbackFun = func;
	if(! CCLayerColor::initWithColor(ccc4(0,0,0,255)))
		return false;

	mWinSize = CCDirector::sharedDirector()->getWinSize();


	BaseActorProperty heroProperty = MyselfManager::getManager()->getMyTeamActor(mHeroId)->getCurrentProperty();
	mLiliang = heroProperty.mLiLiang;
	mMinJie  = heroProperty.mMinJie;
	mZhili   = heroProperty.mZhiLi;
	mTineng  = heroProperty.mTiNeng;
	mOldLv   = heroProperty.level;

	//menu
	mMainMenu = CCMenu::create();
	this->addChild(mMainMenu, 10);
	mMainMenu->setPosition(ccp(0,0));
	mMainMenu->setTouchPriority(-301);
	//
	CCMenuItemImage *itemOk = CCMenuItemImage::create(
		"shangpin_goumai_queren.png",
		"shangpin_goumai_queren_select.png",
		"shangpin_goumai_queren_suoding.png",
		this, menu_selector(LevelUpLayer::menuItemClicked));
	itemOk->setPosition(ccp(mWinSize.width/3, 170));
	mMainMenu->addChild(itemOk, 1, Tag_MenuItem_Ok);
	CCMenuItemImage *itemCancel = CCMenuItemImage::create(
		"shangpin_goumai_quxiao.png",
		"shangpin_goumai_quxiao_select.png",
		this, menu_selector(LevelUpLayer::menuItemClicked));
	itemCancel->setPosition(ccp(mWinSize.width/3*2, 170));
	mMainMenu->addChild(itemCancel, 1, Tag_MenuItem_Cancel);

	//前几级不需要道具  直接升级、
	if (! MyselfManager::getManager()->getMyTeamActor(mHeroId)->isYuanShen()
		&& CS::heroLevelUpNeedItemCount(heroProperty.level + 1) == 0
		&& heroProperty.exp_current >= heroProperty.exp_need)
	{
		itemOk->setOpacity(0);
		itemCancel->setOpacity(0); 
//		menuItemClicked(itemOk); //由OnEnter调用，如果现在调用会导致没有添加进scene的layer开始监听消息， 7级一下会有问题
		return true;
	}
	if (MyselfManager::getManager()->getMyTeamActor(mHeroId)->isYuanShen()
		&& heroProperty.exp_current >= heroProperty.exp_need)
	{
		itemOk->setOpacity(0);
		itemCancel->setOpacity(0);
//		menuItemClicked(itemOk);
		return true;
	}

	//人像
	ImageData data = ImageManager::getManager()->getImageData_ProfessionHead(heroProperty.profession);
	BaseSprite* heroImg = BaseSprite::create(data.getCurrentImageFile());
	this->addChild(heroImg, 1);
	heroImg->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 + heroImg->getContentSize().height/2));
	BaseSprite* heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
	heroImg->addChild(heroImgBg);
	heroImgBg->setPosition(ccp(heroImg->getContentSize().width/2, heroImg->getContentSize().height/2));

	//天使、、、、、、、、
	BaseSprite *titleImg = BaseSprite::create("shengji_biaoti.png");
	this->addChild(titleImg);
	titleImg->setPosition(ccp(mWinSize.width/2, heroImg->getPositionY()+heroImg->getContentSize().height/2+titleImg->getContentSize().height/2));

	//lv old -> new
	char buf[20];
	sprintf(buf, "Lv %d", heroProperty.level);
	CCLabelTTF *oldLvLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, Label_Size);
	this->addChild(oldLvLabel);
	oldLvLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	oldLvLabel->setPosition(ccp(heroImg->getPositionX(), heroImg->getPositionY()-heroImg->getContentSize().height/2-oldLvLabel->getContentSize().height));

	string tishiStr;
	//所需
	CCString temp;
	int labelsize = 22;
	bool isHave = true;
	//经验
	temp = LFStrings::getValue("suoxu") + LFStrings::getValue("jingyan");
	CCLabelTTF *expNeedLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, labelsize);
	expNeedLabel->setColor(ccc3(195,165,96));
	expNeedLabel->setPosition(ccp(239, heroImg->getPositionY()-190));
	this->addChild(expNeedLabel);
	{
		//exp need
		unsigned int exp_need = heroProperty.exp_need;
		temp.initWithFormat("%u/", exp_need);
		CCLabelTTF *needNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, labelsize);
		this->addChild(needNumLabel);
		needNumLabel->setAnchorPoint(ccp(1, 0.5f));
		needNumLabel->setPosition(ccp(403, expNeedLabel->getPositionY()));
		//exp all
		unsigned int exp_all = heroProperty.exp_current;
		temp.initWithFormat("%u", exp_all);
		CCLabelTTF *allNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, labelsize);
		allNumLabel->setAnchorPoint(ccp(0, 0.5f));
		allNumLabel->setPosition(ccp(needNumLabel->getPositionX(), needNumLabel->getPositionY()));
		if (exp_all < exp_need)
		{
			allNumLabel->setColor(ccRED);
			itemOk->setEnabled(false);
			isHave = false;
			tishiStr = LFStrings::getValue("jinyanbuzu");
		}
		this->addChild(allNumLabel);
	}
	//材料 ---> 升级也要材料 、//
	temp = CS::getItemName(ItemsType_Hero_LevelUp);
	CCLabelTTF *cailiaoLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, labelsize);
	cailiaoLabel->setColor(ccc3(195,165,96));
	cailiaoLabel->setPosition(ccp(239, heroImg->getPositionY()-190-72));
	this->addChild(cailiaoLabel);
	{
		//img
		BaseSprite *imgBoard = BaseSprite::create("daoju_kuang_bg.png");
		this->addChild(imgBoard);
		imgBoard->setPosition(ccp(403, cailiaoLabel->getPositionY()));
		imgBoard->setScale(0.6f);
		BaseSprite *img = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_Hero_LevelUp));
		imgBoard->addChild(img);
		img->setPosition(ccp(imgBoard->getContentSize().width/2, imgBoard->getContentSize().height/2));
		//need num
		int need_num = CS::heroLevelUpNeedItemCount(heroProperty.level + 1);
		temp.initWithFormat("%d/", need_num);
		CCLabelTTF *needLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, labelsize);
		this->addChild(needLabel);
		needLabel->setAnchorPoint(ccp(0, 0.5));
		needLabel->setPosition(ccp(imgBoard->getPositionX()+imgBoard->getContentSize().width/2, imgBoard->getPositionY()));
		//all num
		int all_num = WuPinManager::getManager()->getWuPin(ItemsType_Hero_LevelUp)->getCount();
		temp.initWithFormat("%d", all_num);
		mAllCaiLiaoNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, labelsize);
		this->addChild(mAllCaiLiaoNumLabel);
		mAllCaiLiaoNumLabel->setAnchorPoint(ccp(0, 0.5f));
		mAllCaiLiaoNumLabel->setPosition(ccp(needLabel->getPositionX()+needLabel->getContentSize().width, needLabel->getPositionY()));
		if (need_num > all_num)
		{
			mAllCaiLiaoNumLabel->setColor(ccRED);
			itemOk->setEnabled(false);
			isHave = false;
			tishiStr = LFStrings::getValue("xunZhangBuZu_QianWangJingJi");
		}
	}
	if (! isHave)
	{
		CCLabelTTF *label = CCLabelTTF::create(tishiStr.c_str(), fontStr_kaiti, labelsize);
		label->setColor(ccRED);
		label->setPosition(ccp(mWinSize.width/2, (itemOk->getPositionY()+cailiaoLabel->getPositionY())/2));
		this->addChild(label, 0, Tag_Label_CaiLiaoBuZu);

		//如果是战斗结算界面  则不要一直提示。
//		if (CCDirector::sharedDirector()->getRunningScene()->getChildByTag(TAG_FIGHT_END_LAYER) && ! fightEndDisplay)
//			return false;
		BaseActor *actor = MyselfManager::getManager()->getMyTeamActor(mHeroId);
		bool hasFirstLevelUpDisplay = true;  //第一次到8级材料不足时升级是否提示过
		if (! History::getHistory()->getBoolFromCurrentHero("HasFirstLevelUpDisplay", hasFirstLevelUpDisplay))
		{
			hasFirstLevelUpDisplay = false;  //没有显示过
			History::getHistory()->putInCurrentHero("HasFirstLevelUpDisplay", true);
		}

		if (actor && actor->getCurrentProperty().level < 9 && ! hasFirstLevelUpDisplay
			|| CCDirector::sharedDirector()->getRunningScene()->getChildByTag(TAG_FIGHT_END_LAYER) == NULL)
			return true;
		else 
			return false;
	}


	return true;
}

void LevelUpLayer::setDisplayerEnd()
{
	mDisplayerEnd = true;
}

void LevelUpLayer::refreshLayer()
{
	if (mAllCaiLiaoNumLabel != NULL)
	{
		CCString temp;
		BaseActorProperty heroProperty = MyselfManager::getManager()->getMyTeamActor(mHeroId)->getCurrentProperty();
		int need_num = CS::heroLevelUpNeedItemCount(heroProperty.level + 1);
		int all_num = WuPinManager::getManager()->getWuPin(ItemsType_Hero_LevelUp)->getCount();
		temp.initWithFormat("%d", all_num);
		mAllCaiLiaoNumLabel->setString(temp.getCString());
		if (need_num > all_num)
		{
			mAllCaiLiaoNumLabel->setColor(ccRED);
			CCMenuItem* itemOk = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_Ok);
			itemOk->setEnabled(false);
			
			//材料不足
			CCLabelTTF *label = (CCLabelTTF*)this->getChildByTag(Tag_Label_CaiLiaoBuZu);
			if (label == NULL)
			{
				label = CCLabelTTF::create(LFStrings::getValue("cailiaobuzu").c_str(), fontStr_kaiti, 22);
				label->setColor(ccRED);
				label->setPosition(ccp(mWinSize.width/2, (itemOk->getPositionY()+mAllCaiLiaoNumLabel->getPositionY())/2));
				this->addChild(label);
			}
		}
	}
}

void LevelUpLayer::onEnter()
{
	CCLayerColor::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -300, true);  //高于guidelayer ->200

	refreshLayer();

	BaseActorProperty heroProperty = MyselfManager::getManager()->getMyTeamActor(mHeroId)->getCurrentProperty();
	if (! MyselfManager::getManager()->getMyTeamActor(mHeroId)->isYuanShen()
		&& CS::heroLevelUpNeedItemCount(heroProperty.level + 1) == 0
		&& heroProperty.exp_current >= heroProperty.exp_need)
	{
		menuItemClicked(mMainMenu->getChildByTag(Tag_MenuItem_Ok));
	}
	if (MyselfManager::getManager()->getMyTeamActor(mHeroId)->isYuanShen()
		&& heroProperty.exp_current >= heroProperty.exp_need)
	{
		menuItemClicked(mMainMenu->getChildByTag(Tag_MenuItem_Ok));
	}
}

void LevelUpLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayerColor::onExit();
}

bool LevelUpLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}

void LevelUpLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if (! mDisplayerEnd)
		return;

	this->setVisible(false);
	if (mCallbackFun != NULL && mNextLayer == NULL)
		mCallbackFun->LevelUpEndCallBackFunc();
	if (mNextLayer != NULL)
	{
		CCDirector::sharedDirector()->getRunningScene()->addChild(mNextLayer, 150);
		mNextLayer->release();  //set的时候 retain；
	}
	this->removeFromParent();
}

void LevelUpLayer::callbackRemove(CCNode* pSender)
{
	pSender->removeFromParent();
}

CCSprite* LevelUpLayer::getProfLabel(int num, int prof)
{
	CCSprite* bg = CCSprite::create("zhandou_jiesuan_shengji_zitidiguang.png");
	CCSprite* profImg = NULL;
	switch(prof)
	{
	case LiLiang:
		profImg = CCSprite::create("zhandou_jiesuan_shengji_liliang.png");
		break;
	case MinJie:
		profImg = CCSprite::create("zhandou_jiesuan_shengji_minjie.png");
		break;
	case ZhiLi:
		profImg = CCSprite::create("zhandou_jiesuan_shengji_zhili.png");
		break;
	case TiNeng:
		profImg = CCSprite::create("zhandou_jiesuan_shengji_tineng.png");
		break;
	default:
		break;
	}
	bg->addChild(profImg);
	profImg->setPosition(ccp(bg->getContentSize().width/2.0-2.5, bg->getContentSize().height/2.0));
	profImg->setOpacity(0);

	int c = 1;  //位数
	int h = 1;  //最高位
	int temp = num;
	while ( (temp/=10) != 0 )		{++c; h*=10;}

	for(int i=0; i!=c; ++i,h/=10)
	{
		int n = num / h;
		num -= n*h;
		CCSprite* labelNum = labelNum = CCSprite::create("zhandou_jiesuan_shengji_shuzi.png", CCRect(13*n, 0, 13, 20));
		bg->addChild(labelNum);
		labelNum->setPosition(ccp(profImg->getPositionX()+30+13*i, profImg->getPositionY()));
		labelNum->setOpacity(0);
	}

	return bg;
}

void LevelUpLayer::menuItemClicked(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_Ok)
	{
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(LevelUpLayer::rsp_HeroLevelUp), MSG_heroLevelUpRsp, NULL);
		CmdHelper::getHelper()->cmdHeroLevelUp(mHeroId);
	}
	else if (tag == Tag_MenuItem_Cancel)
	{
		this->setVisible(false);
		if (mNextLayer != NULL)
		{
			CCDirector::sharedDirector()->getRunningScene()->addChild(mNextLayer, 150);
			mNextLayer->release();  //set的时候 retain；
		}
		if (mCallbackFun != NULL)
			mCallbackFun->LevelUpEndCallBackFunc();
		this->removeFromParent();
	}
}

void LevelUpLayer::rsp_HeroLevelUp(CCObject *msg)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_heroLevelUpRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_HeroLevelUpRsp* data = (SPCmd_HeroLevelUpRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	//获取元神
	if (data->hero_detial.basic_info.level >= 30)
	{
		CmdHelper::getHelper()->cmdQueryYuanshenOfHero(data->hero_detial.basic_info.hero_id);
		GameDirector::getDirector()->hideWaiting();
	}

	int liLiang = data->hero_detial.hero_attr_without_zhuangbei.liliang - mLiliang;
	int minJie = data->hero_detial.hero_attr_without_zhuangbei.mingjie - mMinJie;
	int zhiLi = data->hero_detial.hero_attr_without_zhuangbei.zhili - mZhili;
	int tiNeng = data->hero_detial.hero_attr_without_zhuangbei.tineng - mTineng;
	levelUpAnimate(liLiang, minJie, zhiLi, tiNeng);

	//教学 --->  由主角升级后触发的新玩法解锁
	if (MyselfManager::getManager()->getMyZhuJueData()->getId() == mHeroId)
	{
		mNewLev = data->hero_detial.basic_info.level;
		teachGuide();
	}
}

void LevelUpLayer::teachGuide()
{
	UnblockManager *_unblockManager = UnblockManager::getManager();

//	if (_unblockManager->canOpen(unblock_DiYuFuBen))// 不是level控制的会蹦
	if (false)
	{
		/*
		**暂不开放传奇副本
		*/
		//g_ChuanQiFuBen_Opend = true;
	}

//	if (mNewLev == 5) //传奇录
	else if (_unblockManager->canOpen(unblock_ChuanQiLu))
	{
		/*
		**该版本不开放传奇录
		*/
		//g_ChuanQiLu_Opend = true;
	}
//	else if (mNewLev == 6)  //英雄榜
	//else if (_unblockManager->canOpen(unblock_YingXiongBang))  //英雄榜
	//{
		//g_YingXiongBang_Opend = true;
	//}
//	else if (mNewLev == 8) // 竞技场
	else if (_unblockManager->canOpen(unblock_JingJiChang)) // 竞技场
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJingJiChang) && (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJingJiChangPlay)))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstJingJiChang);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstJingJiChang);
			g_JingJiChang_Opend = true;
		}
	}
//	else if (mNewLev == 10)//酒馆
	else if (_unblockManager->canOpen(unblock_JiuGuan))//酒馆
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJiuGuan))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstJiuGuan);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstJiuGuan);
			g_JiuGuan_Opend = true;
		}
	}
//	else if (mNewLev == 11)//血色教堂
	else if (_unblockManager->canOpen(unblock_XueSeJiangTang))//血色教堂
	{
		//g_XueSeJiaoTang_Opend = true;
	}
//	else if (mNewLev == 20)//极限守城
	else if (_unblockManager->canOpen(unblock_JiXianShouCheng))//极限守城
	{
		g_JiXianShouCheng_Opend = true;
	}
//	else if (mNewLev == 25)//巅峰对决
	else if (_unblockManager->canOpen(unblock_DianFengDuiJue))//巅峰对决
	{
		g_DianFengDuiJue_Opend = true;
	}
}

void LevelUpLayer::levelUpAnimate(int liliang, int minjie, int zhili, int tineng)
{
	mDisplayerEnd = true;
	this->removeAllChildren();

	BaseActor *actor = MyselfManager::getManager()->getMyTeamActor(mHeroId);
	BaseActorProperty heroProperty = actor->getCurrentProperty();

	//人像
	ImageData data = ImageManager::getManager()->getImageData_ProfessionHead(heroProperty.profession);
	BaseSprite* heroImg = BaseSprite::create(data.getCurrentImageFile());
	this->addChild(heroImg, 1);
	heroImg->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 + heroImg->getContentSize().height/2));
	BaseSprite* heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
	heroImg->addChild(heroImgBg);
	heroImgBg->setPosition(ccp(heroImg->getContentSize().width/2, heroImg->getContentSize().height/2));
	//天使、、、、、、、、
	BaseSprite *titleImg = BaseSprite::create("shengji_biaoti.png");
	this->addChild(titleImg);
	titleImg->setPosition(ccp(mWinSize.width/2, heroImg->getPositionY()+heroImg->getContentSize().height/2+titleImg->getContentSize().height/2));
	//
	const float levUpInterval = 2.0;
	CCSprite *upBg1 = CCSprite::create("zhandou_jiesuan_shengji_2.png");
	this->addChild(upBg1, heroImg->getZOrder()-1);
	upBg1->setPosition(ccp(heroImg->getPositionX()+4, heroImg->getPositionY()+2));
	upBg1->runAction(CCSequence::create(
		CCDelayTime::create(levUpInterval)
		, CCFadeTo::create(0.2f, 0)
		, CCCallFuncN::create(this, SEL_CallFuncN(&LevelUpLayer::callbackRemove))
		, NULL));

	CCSprite* upBg2 = CCSprite::create("zhandou_jiesuan_shengji_3.png");
	this->addChild(upBg2, heroImg->getZOrder()-1);
	upBg2->setPosition(heroImg->getPosition());
	upBg2->setOpacity(0);
	upBg2->runAction(CCSequence::create(
		CCFadeTo::create(0.1f, 255)
		, CCRotateBy::create(levUpInterval-0.1f, -6)
		, CCSpawn::create(CCFadeTo::create(0.2f, 0), CCScaleTo::create(0.2f, 0.8f), CCRotateBy::create(0.2f, -3), NULL)
		, CCCallFuncN::create(this, SEL_CallFuncN(&LevelUpLayer::callbackRemove))
		, NULL));

	CCSprite *upBg3 = CCSprite::create("zhandou_jiesuan_shengji_4.png");
	this->addChild(upBg3, heroImg->getZOrder()-1);
	upBg3->setPosition(heroImg->getPosition());
	upBg3->setOpacity(0);
	upBg3->runAction(CCSequence::create(
		CCFadeTo::create(0.1f, 255)
		, CCRotateBy::create(levUpInterval-0.1f, 6)
		, CCSpawn::create(CCFadeTo::create(0.2f, 0), CCScaleTo::create(0.2f, 0.8f), CCRotateBy::create(0.2f, 3), NULL)
		, CCCallFuncN::create(this, SEL_CallFuncN(&LevelUpLayer::callbackRemove))
		, NULL));

	CCSprite *upEffect = CCSprite::create("zhandou_jiesuan_shengji_1.png");
	this->addChild(upEffect, heroImg->getZOrder()+1);
	upEffect->setPosition(heroImg->getPosition());
	upEffect->setOpacity(0);
	upEffect->runAction(CCSequence::create(
		CCFadeTo::create(0.2f, 255)
		, CCFadeTo::create(levUpInterval, 0)
		, CCCallFuncN::create(this, SEL_CallFuncN(&LevelUpLayer::callbackRemove))
		, NULL));

	//lv old -> new
	char buf[20];
	sprintf(buf, "Lv %d", mOldLv);
	CCLabelTTF *oldLvLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, Label_Size);
	this->addChild(oldLvLabel);
	oldLvLabel->setAnchorPoint(ccp(0, 0.5));
	oldLvLabel->setPosition(ccp(heroImg->getPositionX() - heroImg->getContentSize().width/2 + 14
		, heroImg->getPositionY()-heroImg->getContentSize().height/2-oldLvLabel->getContentSize().height));
	oldLvLabel->setOpacity(0);
	oldLvLabel->runAction(CCFadeTo::create(0.25, 255));

	//jiantou
	BaseSprite *jiantouImg = BaseSprite::create("buffAdd.png");
	jiantouImg->setRotation(90);
	this->addChild(jiantouImg);
	jiantouImg->setPosition(ccp(oldLvLabel->getPositionX()+oldLvLabel->getContentSize().width+jiantouImg->getContentSize().width/2
		, oldLvLabel->getPositionY()));
	jiantouImg->setOpacity(0);
	jiantouImg->runAction(CCSequence::create(CCDelayTime::create(0.2f), CCFadeTo::create(0.15f, 255), NULL));
	//new lv
	int newLv = heroProperty.level;
	sprintf(buf, "%d", newLv);
	CCLabelTTF *newLvLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, Label_Size);
	newLvLabel->setAnchorPoint(ccp(0, 0.5));
	this->addChild(newLvLabel);
	newLvLabel->setPosition(ccp(jiantouImg->getPositionX()+jiantouImg->getContentSize().width/2, jiantouImg->getPositionY()));
	newLvLabel->setOpacity(0);
	newLvLabel->runAction(CCSequence::create(CCDelayTime::create(0.4f), CCFadeTo::create(0.15f, 255), NULL));

	// level up img
	CCSprite *upImage = CCSprite::create("zhandou_jiesuan_shengji_zi.png");
	this->addChild(upImage, 10);
	upImage->setPosition(heroImg->getPosition());
	upImage->setScale(0);
	CCScaleTo *scale = CCScaleTo::create(0.2f, 0.8f);
	CCMoveBy *move = CCMoveBy::create(2, ccp(0, 40));
	CCFadeTo *fade = CCFadeTo::create(1.5f, 0);
	upImage->runAction(CCSequence::create(
		CCDelayTime::create(0.4f)
		, CCSpawn::create(scale, NULL)
		, CCDelayTime::create(0.1f)
		, CCSpawn::create(move, fade, NULL)
		, CCCallFuncN::create(this, SEL_CallFuncN(&LevelUpLayer::callbackRemove))
		, NULL));

	//shuxing
	if (! actor->isYuanShen())
	{
#define ProfNum 2
//		CCSprite *prof[ProfNum];
		CCSprite *prof[ProfNum];
		Profession proType = actor->getCurrentProperty().profession;
		int levNum = 1;
		switch (proType)
		{
		case Profession_Yemanren:
		case Profession_WuSeng:
		case Profession_Qishi:
			prof[0] = getProfLabel(levNum*2, LiLiang);
// 			prof[1] = getProfLabel(levNum*1, MinJie);
// 			prof[2] = getProfLabel(levNum*1, ZhiLi);
			prof[1] = getProfLabel(levNum*4, TiNeng);
			break;
		case Profession_Chike:
		case Profession_LieShou:
		case Profession_ShenJianShou:
		case Profession_YouXia:
//			prof[0] = getProfLabel(levNum*1, LiLiang);
			prof[0] = getProfLabel(levNum*2, MinJie);
//			prof[2] = getProfLabel(levNum*1, ZhiLi);
			prof[1] = getProfLabel(levNum*4, TiNeng);
			break;
		case Profession_Fashi:
		case Profession_WuShi:
		case Profession_MiShu:
//  			prof[0] = getProfLabel(levNum*1, LiLiang);
//  			prof[1] = getProfLabel(levNum*1, MinJie);
			prof[0] = getProfLabel(levNum*2, ZhiLi);
			prof[1] = getProfLabel(levNum*4, TiNeng);
			break;
		default:
			CCAssert(false, "error Profession");
			break;
		}
		for(int i=0; i<ProfNum; ++i)
		{
			prof[i]->setOpacity(0);
			this->addChild(prof[i]);
			prof[i]->setPosition(ccp(heroImg->getPositionX()
				, oldLvLabel->getPositionY()-oldLvLabel->getContentSize().height/2 - 20 - i*26));
			if(i != 3)
				prof[i]->runAction(CCSequence::create(
				CCDelayTime::create(levUpInterval+0.1f)
				, CCDelayTime::create(i*0.1f)
				, CCFadeTo::create(0.1f, 255)
				, NULL));
			else 
				prof[i]->runAction(CCSequence::create(
				CCDelayTime::create(levUpInterval+0.1f)
				, CCDelayTime::create(i*0.1f)
				, CCFadeTo::create(0.1f, 255)
				, CCCallFunc::create(this, SEL_CallFunc(&LevelUpLayer::setDisplayerEnd))
				, NULL));

			CCObject* it;
			CCArray *array = prof[i]->getChildren();
			CCARRAY_FOREACH(array, it)
			{
				((CCSprite*)it)->runAction(CCSequence::create(
					CCDelayTime::create(levUpInterval+0.1)
					, CCDelayTime::create(i*0.1)
					, CCFadeTo::create(0.2f, 255)
					// 				, CCDelayTime::create(1.1)
					// 				, CCFadeTo::create(0.4, 0)
					// 				, CCCallFuncN::create(this, SEL_CallFuncN(&LevelUpLayer::callbackRemove))
					, NULL));
			}
		}
	}
}