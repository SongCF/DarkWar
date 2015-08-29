#include "AddFriendWarnLayer.h"
#include "LFStrings.h"
#include "font.h"
AddFriendWarnLayer::AddFriendWarnLayer()
	:CCLayer()
{
}
AddFriendWarnLayer::~AddFriendWarnLayer()
{
}
bool AddFriendWarnLayer::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
	return true;
}
void AddFriendWarnLayer::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-2014,true);
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	
	//m_lNodeList.push_back(spr);
	ccColor4B color = {0,0,0,100};
	CCLayerColor *colorLayer1 = CCLayerColor::create(color);
	colorLayer1->setPosition(CCPointZero);
	colorLayer1->setContentSize(size);
	addChild(colorLayer1);

	CCSprite *spr = CCSprite::createWithTexture(CCSprite::create("jiaoxue_tanchukuang_jiesuo_zhushou.png")->getTexture());
	spr->setPosition(ccp(size.width / 2,size.height / 2));
	addChild(spr);


	CCLabelTTF *nameTTF = CCLabelTTF::create(CCString::createWithFormat("%s%s",m_sFriendName.c_str(),LFStrings::getValue("AddFriendTiShi").c_str())->getCString(),fontStr_kaiti,25.0f);
	nameTTF->setPosition(ccp(spr->getContentSize().width / 2,spr->getContentSize().height / 2));
	spr->addChild(nameTTF);
	nameTTF->setColor(ccWHITE);
}

void AddFriendWarnLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}
bool AddFriendWarnLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void AddFriendWarnLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	this->removeFromParent();
}