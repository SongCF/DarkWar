#include "GameStoryLayer.h"
#include "CreateHeroLayer.h"
#include "LFStrings.h"
#include "font.h"
#include <string>

using std::string;





//1. 开场动画
#define MAX_STORY 6
#define FILE_FORMAT "TheOpeningStory/kaichangjuqing%d/kaichangjuqing%d.ExportJson"
#define Armature_Format "kaichangjuqing%d"
#define Animate_Format "kaichangjuqing%d"

bool GameStoryLayer::init()
{
	//data
	mCurStoryIdx = 0;
	mWinSize = CCDirector::sharedDirector()->getWinSize();
	mTouchAble = false;


	//ui
	mBg = BaseSprite::create("TheOpeningStory/juqing_bg.png");
	addChild(mBg, 0);
	mBg->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	mBg->setOpacity(0);

	CCMenu *menu = CCMenu::create();
	this->addChild(menu, 100);
	menu->setPosition(CCPointZero);
	mNextItem = CCMenuItemImage::create(
		"gongjiang_cuiqu_fumo_chenggong_jiantou.png",
		"gongjiang_cuiqu_fumo_chenggong_jiantou.png",
		"gongjiang_cuiqu_fumo_chenggong_jiantou.png",
		this, menu_selector(GameStoryLayer::onMenu_next));
	menu->addChild(mNextItem);
	mNextItem->setVisible(false);
	mNextItem->setPosition(ccp(mWinSize.width-100, 60));
	mNextItem->setEnabled(false);//layer来吞噬整个触摸
	//
	mNextLabel = CCLabelTTF::create(LFStrings::getValue("JiXu").c_str(), fontStr_kaiti, 32);
	mNextLabel->setColor(ccc3(181,93,6));
	this->addChild(mNextLabel, 100);
	mNextLabel->setPosition(ccp(mNextItem->getPositionX()-mNextItem->getContentSize().width, mNextItem->getPositionY()));
	mNextLabel->setVisible(false);

	return true;
}
GameStoryLayer::~GameStoryLayer()
{
}
void GameStoryLayer::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1, true);

	//kaichangjuqing%d   0-5
	mCurStoryIdx = 0;
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(
		CCString::createWithFormat(FILE_FORMAT, mCurStoryIdx, mCurStoryIdx)->getCString());
	mCurArmature = CCArmature::create(CCString::createWithFormat(Armature_Format, mCurStoryIdx)->getCString());
	mCurArmature->getAnimation()->play(CCString::createWithFormat(Animate_Format, mCurStoryIdx)->getCString());
	mCurArmature->getAnimation()->setMovementEventCallFunc(this, 
		SEL_MovementEventCallFunc(&GameStoryLayer::callback_OneStoryAnimateEnd));
	mCurArmature->setPosition(mWinSize.width/2, mWinSize.height/2);
	this->addChild(mCurArmature);
}
void GameStoryLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}


bool GameStoryLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint pos = pTouch->getLocation();
	pos = mNextItem->getParent()->convertToNodeSpace(pos);
	CCPoint itemPos = mNextItem->getPosition();

	if (pos.y < itemPos.y * 2 && mTouchAble)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void GameStoryLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}
void GameStoryLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint pos = pTouch->getLocation();
	pos = mNextItem->getParent()->convertToNodeSpace(pos);
	CCPoint itemPos = mNextItem->getPosition();

	if (pos.y < itemPos.y * 2)
	{
		onMenu_next(0);
	}
}

void GameStoryLayer::callback_OneStoryAnimateEnd(CCArmature *ar, MovementEventType type, const char *p)
{
	if (type == COMPLETE || type == LOOP_COMPLETE) {
		mNextItem->setOpacity(0);
		mNextItem->setVisible(true);
		mNextItem->runAction(CCRepeatForever::create(CCSequence::create(
			CCFadeTo::create(1, 255),
			CCFadeTo::create(1, 0),
			NULL)));

		mNextLabel->setOpacity(0);
		mNextLabel->setVisible(true);
		mNextLabel->runAction(CCRepeatForever::create(CCSequence::create(
			CCFadeTo::create(1, 255),
			CCFadeTo::create(1, 0),
			NULL)));


		mTouchAble = true;
	}
}

void GameStoryLayer::onMenu_next(CCObject *pSender)
{
	mNextItem->setVisible(false);
	mNextLabel->setVisible(false);

	mTouchAble = false;
//	mNextItem->runAction(CCSequence::create(CCFadeTo::create(0.1f, 0), CCHide::create(), NULL));
//	mNextLabel->runAction(CCSequence::create(CCFadeTo::create(0.1f, 0), CCHide::create(), NULL));

	mCurArmature->removeFromParent();
	mCurArmature = NULL;
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(
		CCString::createWithFormat(FILE_FORMAT, mCurStoryIdx, mCurStoryIdx)->getCString());
	++mCurStoryIdx;

	if (mCurStoryIdx < MAX_STORY){
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(
			CCString::createWithFormat(FILE_FORMAT, mCurStoryIdx, mCurStoryIdx)->getCString());
		mCurArmature = CCArmature::create(CCString::createWithFormat(Armature_Format, mCurStoryIdx)->getCString());
		mCurArmature->getAnimation()->play(CCString::createWithFormat(Animate_Format, mCurStoryIdx)->getCString());
		mCurArmature->getAnimation()->setMovementEventCallFunc(this, 
			SEL_MovementEventCallFunc(&GameStoryLayer::callback_OneStoryAnimateEnd));
		mCurArmature->setPosition(mWinSize.width/2, mWinSize.height/2);
		this->addChild(mCurArmature);
	}
	else{
		callback_allAnimatePlayEnd();
	}

	if (mCurStoryIdx == 0 || 
		mCurStoryIdx == 3 ||
		mCurStoryIdx == 5){
		mBg->runAction(CCFadeTo::create(0.35f, 0));
	}else {
		mBg->runAction(CCFadeTo::create(0.1f, 255));
	}
}

void GameStoryLayer::callback_allAnimatePlayEnd()
{
	CreateHeroLayer* layer = new CreateHeroLayer(CreateHero_Dest_UseThisToMain);
	CCScene* scene = CCScene::create();
	scene->addChild(layer);
	layer->release();
	CCDirector::sharedDirector()->replaceScene(scene);

	CCLog("%s----> CreateHeroLayer", __FUNCTION__);
}




//2.. 章节插画
#define ARMATURE_FILE_ChapterIllustrationLayer "TheOpeningStory/zhangjiedonghua/zhangjiedonghua.ExportJson"

ChapterIllustrationLayer* ChapterIllustrationLayer::create(unsigned int taskId, CCObject* callback_target, SEL_CallFunc pCallbackFunc)
{
	ChapterIllustrationLayer* layer = new ChapterIllustrationLayer;
	if (layer && layer->init(taskId, callback_target, pCallbackFunc))
	{
		layer->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(layer);
		layer = 0;
	}

	return layer;
}
bool ChapterIllustrationLayer::init(unsigned int taskId, CCObject* callback_target, SEL_CallFunc pCallbackFunc)
{
	mCallbackTarget = callback_target;
	m_pCallbackFunc = pCallbackFunc;
	//data
	mWinSize = CCDirector::sharedDirector()->getWinSize();
	mTouchAble = false;


	//ui
	CCMenu *menu = CCMenu::create();
	this->addChild(menu, 100);
	menu->setPosition(CCPointZero);
	mNextItem = CCMenuItemImage::create(
		"gongjiang_cuiqu_fumo_chenggong_jiantou.png",
		"gongjiang_cuiqu_fumo_chenggong_jiantou.png",
		"gongjiang_cuiqu_fumo_chenggong_jiantou.png",
		this, menu_selector(ChapterIllustrationLayer::onMenu_next));
	menu->addChild(mNextItem);
	mNextItem->setVisible(false);
	mNextItem->setPosition(ccp(mWinSize.width-100, 60));
	mNextItem->setEnabled(false);//layer来吞噬整个触摸
	//
	mNextLabel = CCLabelTTF::create(LFStrings::getValue("JiXu").c_str(), fontStr_kaiti, 32);
	mNextLabel->setColor(ccc3(181,93,6));
	this->addChild(mNextLabel, 100);
	mNextLabel->setPosition(ccp(mNextItem->getPositionX()-mNextItem->getContentSize().width, mNextItem->getPositionY()));
	mNextLabel->setVisible(false);


	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(ARMATURE_FILE_ChapterIllustrationLayer);
	if (taskId == 10){
		m_strAnimateName = "zhangjiedonghua1";
	}
	else if(taskId == 19){
		m_strAnimateName = "zhangjiedonghua2";
	}
	else {
		CCLog("%s taskid = %d", __FUNCTION__, taskId);
		CCAssert(false, "");
	}

	return true;
}
ChapterIllustrationLayer::~ChapterIllustrationLayer()
{
}
void ChapterIllustrationLayer::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1, true);

	if (! m_strAnimateName.empty())
	{
		mCurArmature = CCArmature::create("zhangjiedonghua");
		mCurArmature->getAnimation()->play(m_strAnimateName.c_str());
		mCurArmature->getAnimation()->setMovementEventCallFunc(this, 
			SEL_MovementEventCallFunc(&ChapterIllustrationLayer::callback_OneStoryAnimateEnd));
		mCurArmature->setPosition(mWinSize.width/2, mWinSize.height/2);
		this->addChild(mCurArmature);
	}
}
void ChapterIllustrationLayer::onExit()
{
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(ARMATURE_FILE_ChapterIllustrationLayer);
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}


bool ChapterIllustrationLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint pos = pTouch->getLocation();
	pos = mNextItem->getParent()->convertToNodeSpace(pos);
	CCPoint itemPos = mNextItem->getPosition();

	if (pos.y < itemPos.y * 2 && mTouchAble)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void ChapterIllustrationLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint pos = pTouch->getLocation();
	pos = mNextItem->getParent()->convertToNodeSpace(pos);
	CCPoint itemPos = mNextItem->getPosition();

	if (pos.y < itemPos.y * 2)
	{
		onMenu_next(0);
	}
}

void ChapterIllustrationLayer::callback_OneStoryAnimateEnd(CCArmature *ar, MovementEventType type, const char *p)
{
	if (type == COMPLETE || type == LOOP_COMPLETE) {
		mNextItem->setOpacity(0);
		mNextItem->setVisible(true);
		mNextItem->runAction(CCRepeatForever::create(CCSequence::create(
			CCFadeTo::create(1, 255),
			CCFadeTo::create(1, 0),
			NULL)));

		mNextLabel->setOpacity(0);
		mNextLabel->setVisible(true);
		mNextLabel->runAction(CCRepeatForever::create(CCSequence::create(
			CCFadeTo::create(1, 255),
			CCFadeTo::create(1, 0),
			NULL)));


		mTouchAble = true;
	}
}

void ChapterIllustrationLayer::onMenu_next(CCObject *pSender)
{
	mNextItem->setVisible(false);
	mNextLabel->setVisible(false);

	mTouchAble = false;

	mCurArmature->removeFromParent();
	mCurArmature = NULL;


	callback_allAnimatePlayEnd();

}

void ChapterIllustrationLayer::callback_allAnimatePlayEnd()
{
//	CCDirector::sharedDirector()->popScene();
	if (mCallbackTarget && m_pCallbackFunc)
		(mCallbackTarget->*m_pCallbackFunc)();

	this->removeFromParent();
	CCLog("%s----> end", __FUNCTION__);
}