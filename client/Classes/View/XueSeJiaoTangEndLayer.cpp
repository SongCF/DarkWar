#include "XueSeJiaoTangEndLayer.h"
#include "Music.h"
#include "BaseSprite.h"
#include "LFStrings.h"
#include "font.h"
#include "GameDirector.h"
#include "XueSeJiaoTangFightPrepareLayer.h"
#include "XueZhanManager.h"
#include "XueSeJiaoTangLayer.h"
#include "BackgroudLayer.h"
#include "GameScene.h"

#define Label_Size 24


enum 
{
	Tag_MenuItem_Next = 10,
};

XueSeJiaoTangEndLayer* XueSeJiaoTangEndLayer::create(bool bWin)
{
	XueSeJiaoTangEndLayer* layer = new XueSeJiaoTangEndLayer(bWin);
	if(layer)
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


XueSeJiaoTangEndLayer::XueSeJiaoTangEndLayer(bool bWin)
	:mWin(bWin)
{
	// init layer;
	CCLayer::init();
	mWinSize = CCDirector::sharedDirector()->getWinSize();
	this->setTag(Tag_XueSeJiaoTangEnd_Layer);
	this->setZOrder(1025);

	// 半透明背景
	CCSprite* btm = BaseSprite::create("zhandou_jiugongge_touming.png");
	addChild(btm, -1);
	btm->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	btm->setScaleX(mWinSize.width / btm->getContentSize().width);
	btm->setScaleY(mWinSize.height / btm->getContentSize().height);
	btm->setOpacity(0);
	btm->runAction(CCFadeTo::create(0.5,220));


	//1. 成功失败！！？
	std::string music, tishiStr = LFStrings::getValue("zhanDou");
	CCSprite *imageBiaoTi;

	if(bWin)
	{//成功
		imageBiaoTi = BaseSprite::create("zhandou_shengli.png");
		music = "zhandou_jiesuan_shengli.wav";
		tishiStr += LFStrings::getValue("shengli");
	}else
	{//失败
		imageBiaoTi = BaseSprite::create("zhandou_shibai.png");
		music = "zhandou_jiesuan_shibai.wav";
		tishiStr += LFStrings::getValue("shibai");
	}


	{
		imageBiaoTi->setPosition(ccp(mWinSize.width/2, mWinSize.height-260));
		imageBiaoTi->setOpacity(100);
		addChild(imageBiaoTi, 1);

		//闪烁效果
		imageBiaoTi->setScale(5);
		CCScaleTo* scale = CCScaleTo::create(0.25f, 0.8f, 0.8f);
		CCFadeTo * fade  = CCFadeTo::create(0.25f, 255);
		CCDelayTime *dt = CCDelayTime::create(0.25f);
		CCScaleTo *scaleBack = CCScaleTo::create(0.15f, 1, 1);

		CCCallFunc *callfun = CCCallFunc::create(this, callfunc_selector(XueSeJiaoTangEndLayer::displayInfomation));
		imageBiaoTi->runAction(CCSequence::create(CCSpawn::create(scale, fade, NULL), callfun, scaleBack, NULL));
	}

	Music::sharedMusicPlayer()->playEffectSound(music);


}

void XueSeJiaoTangEndLayer::displayInfomation()
{
	CCString temp;
	if (mWin)
	{
		CCMenu *menu = CCMenu::create();
		addChild(menu, 10);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *endItem = CCMenuItemImage::create(
			"zhandou_ditu_renwushuoming_xiayiguan.png",
			"zhandou_ditu_renwushuoming_xiayiguan_select.png",
			this, menu_selector(XueSeJiaoTangEndLayer::menuItemClicked_MainMenu));
		menu->addChild(endItem, 0, Tag_MenuItem_Next);
		endItem->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 - 100));
	}
	else
	{
		CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("ZhanDouShiBai_TiShi").c_str(), fontStr_kaiti, Label_Size);
		addChild(tishiLabel, 1);
		tishiLabel->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 + 70));
	}

}

void XueSeJiaoTangEndLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_Next)
	{
		XueSeJiaoTangFightPrepareLayer *layer = new XueSeJiaoTangFightPrepareLayer();
		CCScene *scene = CCScene::create();
		scene->addChild(layer);
		layer->release();
		CCDirector::sharedDirector()->replaceScene(scene);
		return;
	}
}

void XueSeJiaoTangEndLayer::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -127, true);
}

void XueSeJiaoTangEndLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}

bool XueSeJiaoTangEndLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
} 

void XueSeJiaoTangEndLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if (! mWin)
	{
		MainLayerBase* footer = new MainLayerBase();
		BackgroudLayer* bgLayer = new BackgroudLayer();

		BaseLayer* layer = XueSeJiaoTangLayer::create();
		footer->setContaintLayer(layer);

		GameScene* scene = GameScene::createScene();
		scene->addChild(footer,1);
		scene->addChild(bgLayer,-1);

		footer->release();
		bgLayer->release();

		CCDirector::sharedDirector()->replaceScene(scene);
		return;
	}
}