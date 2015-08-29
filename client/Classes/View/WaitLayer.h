
#pragma once
#include "cocos2d.h"
#include "font.h"
#include <string>
using std::string;

USING_NS_CC;

#define TAG_Waiting_Layer 4643+90

class WaitLayer : public CCLayerColor
{
public:
	WaitLayer(string infomation){
		mInfo = infomation;
	}

	~WaitLayer(){};

	static WaitLayer* create(string infomation=""){
		WaitLayer* layer = new WaitLayer(infomation);
		if(layer && layer->init())
			layer->autorelease();
		else
		{
			CC_SAFE_DELETE(layer);
			layer = NULL;
		}
		return layer;
	}

	bool init(){
		if( !CCLayerColor::initWithColor(ccc4(0,0,0,10)) ) 
			return false;

		CCSize winSize = CCDirector::sharedDirector()->getWinSize();


		if (mInfo.size() > 0)
		{
			//bg
			CCSprite* waitImg = CCSprite::create("PVP_tishi.png");
			addChild(waitImg);
			waitImg->setPosition(ccp(winSize.width/2, winSize.height/2));
			//infomation
			CCLabelTTF* info = CCLabelTTF::create(mInfo.c_str(), fontStr_kaiti, 24, CCSize(228, 0), kCCTextAlignmentCenter);
			info->setAnchorPoint(ccp(0.5, 0.5));
			waitImg->addChild(info);
			info->setColor(ccc3(0x8B, 0x45, 0x13));
			info->setPosition(ccp(waitImg->getContentSize().width/2, waitImg->getContentSize().height/2));
		}

		this->setTag(TAG_Waiting_Layer);
		this->setZOrder(1000);

		return true;
	}
	

	virtual void onEnter() {
		CCLayerColor::onEnter();
		CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1000, true);
	}

	virtual void onExit() {
		CCLayerColor::onExit();
		CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	}

	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){ 

		this->runAction(CCSequence::create(
			CCScaleTo::create(0.05f, 1.1f)
			, CCScaleTo::create(0.05f, 1.0f)
			, NULL));
		return true; 
	}

protected:
	string mInfo;
};