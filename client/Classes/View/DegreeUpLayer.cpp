#include "DegreeUpLayer.h"
#include "ImageManager.h"
#include "BaseSprite.h"
#include "MyselfManager.h"
#include "font.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "CmdHelper.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "FX_CommonFunc.h"

using namespace FX_CommonFunc;

#define Label_Size 25

enum
{
	Tag_MenuItem_Ok,
	Tag_MenuItem_Cancel,

	Tag_Label_DegreeNum,
};


DegreeUpLayer *DegreeUpLayer::create(unsigned int hero_id, DegreeUpEndCallBackInterface *func)
{
	DegreeUpLayer *layer = new DegreeUpLayer;
	if(layer && layer->init(hero_id, func))
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

bool DegreeUpLayer::init(unsigned int hero_id, DegreeUpEndCallBackInterface *func)
{
	mDisplayerEnd = false;
	mHeroId = hero_id;
	mCallbackFun = func;
	if(! CCLayerColor::initWithColor(ccc4(0,0,0,255)))
		return false;

	mWinSize = CCDirector::sharedDirector()->getWinSize();

	//menu
	CCMenu *menu = CCMenu::create();
	this->addChild(menu, 10);
	menu->setPosition(ccp(0,0));
	menu->setTouchPriority(-131);
	//
	CCMenuItemImage *itemOk = CCMenuItemImage::create(
		"shangpin_goumai_queren.png",
		"shangpin_goumai_queren_select.png",
		"shangpin_goumai_queren_suoding.png",
		this, menu_selector(DegreeUpLayer::menuItemClicked));
	itemOk->setPosition(ccp(mWinSize.width/3, 170));
	menu->addChild(itemOk, 1, Tag_MenuItem_Ok);
	CCMenuItemImage *itemCancel = CCMenuItemImage::create(
		"shangpin_goumai_quxiao.png",
		"shangpin_goumai_quxiao_select.png",
		this, menu_selector(DegreeUpLayer::menuItemClicked));
	itemCancel->setPosition(ccp(mWinSize.width/3*2, 170));
	menu->addChild(itemCancel, 1, Tag_MenuItem_Cancel);

	//人像
	ImageData data = ImageManager::getManager()->getImageData_ProfessionHead(MyselfManager::getManager()->getMyTeamActor(mHeroId)->getCurrentProperty().profession);
	BaseSprite* heroImg = BaseSprite::create(data.getCurrentImageFile());
	this->addChild(heroImg, 1);
	heroImg->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 + heroImg->getContentSize().height/2));
	BaseSprite* heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
	heroImg->addChild(heroImgBg);
	heroImgBg->setPosition(ccp(heroImg->getContentSize().width/2, heroImg->getContentSize().height/2));

	//天使、、、、、、、、
	BaseSprite *titleImg = BaseSprite::create("shengjie_biaoti.png");
	this->addChild(titleImg);
	titleImg->setPosition(ccp(mWinSize.width/2, heroImg->getPositionY()+heroImg->getContentSize().height/2+titleImg->getContentSize().height/2));

	CCString temp;
	//degree num
	unsigned int iStage = MyselfManager::getManager()->getMyTeamActor(mHeroId)->getCurrentStage();
	temp.initWithFormat("%s%s%s%s", LFStrings::getValue("DangQian").c_str(), LFStrings::getValue("di").c_str()
		, getStrByInt(iStage).c_str(), LFStrings::getValue("jie").c_str());
	CCLabelTTF *oldLvLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size);
	this->addChild(oldLvLabel, 0, Tag_Label_DegreeNum);
	oldLvLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	oldLvLabel->setPosition(ccp(heroImg->getPositionX(), heroImg->getPositionY()-heroImg->getContentSize().height/2-oldLvLabel->getContentSize().height));

	//所需
	int labelsize = 22;
	bool isHave = true;
	//银币
	temp = LFStrings::getValue("suoxu") + LFStrings::getValue("yinbi");
	CCLabelTTF *silNeedLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, labelsize);
	silNeedLabel->setColor(ccc3(195,165,96));
	silNeedLabel->setPosition(ccp(239, heroImg->getPositionY()-190));
	this->addChild(silNeedLabel);
	{
		//silver need
		unsigned int sil_need = CS::yuanshenUpgradeNeedSilver(iStage + 1);
		temp.initWithFormat("%u/", sil_need);
		CCLabelTTF *needNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, labelsize);
		this->addChild(needNumLabel);
		needNumLabel->setAnchorPoint(ccp(1, 0.5f));
		needNumLabel->setPosition(ccp(403, silNeedLabel->getPositionY()));
		//exp all
		unsigned int sil_all = WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount();
		temp.initWithFormat("%u", sil_all);
		CCLabelTTF *allNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, labelsize);
		allNumLabel->setAnchorPoint(ccp(0, 0.5f));
		allNumLabel->setPosition(ccp(needNumLabel->getPositionX(), needNumLabel->getPositionY()));
		if (sil_all < sil_need)
		{
			allNumLabel->setColor(ccRED);
			itemOk->setEnabled(false);
			isHave = false;
		}
		this->addChild(allNumLabel);
	}
	//材料   //元神升阶不要材料
	if (! MyselfManager::getManager()->getMyTeamActor(hero_id)->isYuanShen())
	{
		temp = CS::getItemName(ItemsType_Hero_Upgrade);
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
			BaseSprite *img = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_Hero_Upgrade));
			imgBoard->addChild(img);
			img->setPosition(ccp(imgBoard->getContentSize().width/2, imgBoard->getContentSize().height/2));
			//need num
			int need_num = CS::heroUpgradeNeedItemCount(iStage + 1);
			temp.initWithFormat("%d/", need_num);
			CCLabelTTF *needLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, labelsize);
			this->addChild(needLabel);
			needLabel->setAnchorPoint(ccp(0, 0.5));
			needLabel->setPosition(ccp(imgBoard->getPositionX()+imgBoard->getContentSize().width/2, imgBoard->getPositionY()));
			//all num
			int all_num = WuPinManager::getManager()->getWuPin(ItemsType_Hero_Upgrade)->getCount();
			temp.initWithFormat("%d", all_num);
			CCLabelTTF *allLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, labelsize);
			this->addChild(allLabel);
			allLabel->setAnchorPoint(ccp(0, 0.5f));
			allLabel->setPosition(ccp(needLabel->getPositionX()+needLabel->getContentSize().width, needLabel->getPositionY()));
			if (need_num > all_num)
			{
				allLabel->setColor(ccRED);
				itemOk->setEnabled(false);
				isHave = false;
			}
		}
	}

	if (! isHave)
	{
		CCLabelTTF *label = CCLabelTTF::create(LFStrings::getValue("suoxuyinbibuzu").c_str(), fontStr_kaiti, labelsize);
		label->setColor(ccRED);
		label->setPosition(ccp(mWinSize.width/2, (itemOk->getPositionY()+silNeedLabel->getPositionY())/2));
		this->addChild(label);
	}


	return true;
}

void DegreeUpLayer::setDisplayerEnd()
{
	mDisplayerEnd = true;
}

void DegreeUpLayer::onEnter()
{
	CCLayerColor::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -130, true);
}

void DegreeUpLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayerColor::onExit();
}

bool DegreeUpLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}

void DegreeUpLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if (! mDisplayerEnd)
		return;

	this->setVisible(false);
	if (mCallbackFun != NULL)
		mCallbackFun->DegreeUpEndCallBackFunc();
	this->removeFromParent();
}

void DegreeUpLayer::callbackRemove(CCNode* pSender)
{
	pSender->removeFromParent();
}

void DegreeUpLayer::menuItemClicked(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_Ok)
	{
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this, 
			callfuncO_selector(DegreeUpLayer::rsp_YuanShenDegreeUp), MSG_heroUpgradeRsp, NULL);
		CmdHelper::getHelper()->cmdHeroUpgrade(mHeroId);
//		rsp_YuanShenDegreeUp(0);
	}
	else if (tag == Tag_MenuItem_Cancel)
	{
		this->setVisible(false);
		if (mCallbackFun != NULL)
			mCallbackFun->DegreeUpEndCallBackFunc();
		this->removeFromParent();
	}
}

void DegreeUpLayer::rsp_YuanShenDegreeUp(CCObject *msg)
{
	GameDirector::getDirector()->hideWaiting();
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_heroUpgradeRsp);

	SPCmd_HeroUpgradeRsp * data = (SPCmd_HeroUpgradeRsp *)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	unsigned int iStage = MyselfManager::getManager()->getMyTeamActor(mHeroId)->getCurrentStage();
	degreeUpAnimate(iStage);
}

void DegreeUpLayer::degreeUpAnimate(int stage)
{
	mDisplayerEnd = true;
	this->removeAllChildren();

	BaseActorProperty heroProperty = MyselfManager::getManager()->getMyTeamActor(mHeroId)->getCurrentProperty();

	//人像
	ImageData data = ImageManager::getManager()->getImageData_ProfessionHead(heroProperty.profession);
	BaseSprite* heroImg = BaseSprite::create(data.getCurrentImageFile());
	this->addChild(heroImg, 1);
	heroImg->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 + heroImg->getContentSize().height/2));
	BaseSprite* heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
	heroImg->addChild(heroImgBg);
	heroImgBg->setPosition(ccp(heroImg->getContentSize().width/2, heroImg->getContentSize().height/2));
	//天使、、、、、、、、
	BaseSprite *titleImg = BaseSprite::create("shengjie_biaoti.png");
	this->addChild(titleImg);
	titleImg->setPosition(ccp(mWinSize.width/2, heroImg->getPositionY()+heroImg->getContentSize().height/2+titleImg->getContentSize().height/2));
	
	//阶
	CCString temp;
	temp.initWithFormat("%s%s%s", LFStrings::getValue("di").c_str(), getStrByInt(stage).c_str(), LFStrings::getValue("jie").c_str());
	CCLabelTTF *stageLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size);
	this->addChild(stageLabel, 0, Tag_Label_DegreeNum);
	stageLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	stageLabel->setPosition(ccp(heroImg->getPositionX(), 
		heroImg->getPositionY()-heroImg->getContentSize().height/2-stageLabel->getContentSize().height));
	stageLabel->setScale(0.2f);
	stageLabel->setOpacity(0);
	stageLabel->runAction(CCSequence::create(
		CCSpawn::create(CCFadeTo::create(0.25f, 255), CCScaleTo::create(0.25f, 1.2f), NULL),
		CCScaleTo::create(0.15f, 1),
		NULL));

	// level up img
	CCSprite *upImage = CCSprite::create("yuanshen_shengjie_wenzi.png");
	this->addChild(upImage, 10);
	upImage->setPosition(heroImg->getPosition());
	upImage->setScale(0);
	CCScaleTo *scale = CCScaleTo::create(0.2, 0.8);
	CCMoveBy *move = CCMoveBy::create(2, ccp(0, 40));
	CCFadeTo *fade = CCFadeTo::create(1.5, 0);
	upImage->runAction(CCSequence::create(
		CCDelayTime::create(0.4)
		, CCSpawn::create(scale, NULL)
		, CCDelayTime::create(0.1)
		, CCSpawn::create(move, fade, NULL)
		, CCCallFuncN::create(this, SEL_CallFuncN(&DegreeUpLayer::callbackRemove))
		, NULL));
}