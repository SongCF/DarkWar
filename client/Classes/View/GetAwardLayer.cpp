#include "GetAwardLayer.h"
#include "ImageManager.h"
#include "font.h"
#include "CS_Common.h"
#include "flat.h"
#include "protocol.h"
#include "CmdHelper.h"
#include "GameDirector.h"

GetAwardLayer* GetAwardLayer::create(CCString info, vector<ItemInfo> items, vector<Flat_DuanzaoGroupZBInfo> equips)
{
	GetAwardLayer* layer = new GetAwardLayer(info, items, equips);
	if(layer != NULL)
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

GetAwardLayer::GetAwardLayer(CCString info, vector<ItemInfo> items, vector<Flat_DuanzaoGroupZBInfo> equips)
{
	CCLayerColor::initWithColor(ccc4(0,0,0,255));


	CCSize visSize = CCDirector::sharedDirector()->getWinSize();
	mBoard = BaseSprite::create("shangpin_goumai_tishikuang.png");
	addChild(mBoard);
	mBoard->setPosition(ccp(visSize.width/2, visSize.height/2));

	//label 提示
	{
		CCLabelTTF* label = CCLabelTTF::create(info.getCString(), 
			fontStr_kaiti, 20, CCSize(425, 0), kCCTextAlignmentLeft);
		label->setColor(ccWHITE);
		mBoard->addChild(label, 1);
		if (items.size() == 0 && equips.size() == 0)
		{
			label->setAnchorPoint(ccp(0.5, 0.5));
			label->setPosition(ccp(mBoard->getContentSize().width/2, 222));
		}
		else
		{
			label->setAnchorPoint(ccp(0.5, 1));
			label->setPosition(ccp(mBoard->getContentSize().width/2, 326));
		}
	}

	//材料
	int label_size = 20;
	ccColor3B label_color = ccWHITE;

	int allNum = items.size() + equips.size();
	int posIdx = 0;
	for (int i=0; i < items.size(); ++i, ++posIdx)
	{
		BaseSprite* cailiaoImg = BaseSprite::create("daoju_kuang_bg.png");
		mBoard->addChild(cailiaoImg);

		int pos_x = mBoard->getContentSize().width/2;
		int delta = posIdx - allNum/2;
		if(allNum % 2 == 0) 
		{
			float del = delta + 0.5;
			pos_x += del * (cailiaoImg->getContentSize().width + 10);
		}
		else
		{
			pos_x += delta * (cailiaoImg->getContentSize().width + 10);
		}
		cailiaoImg->setPosition(ccp(pos_x, mBoard->getContentSize().height/2+24));
		//item img
		BaseSprite * itemImg = BaseSprite::create(
			ImageManager::getManager()->getItemFilename(items.at(i).type)); // .count .type
		cailiaoImg->addChild(itemImg);
		itemImg->setPosition(ccp(cailiaoImg->getContentSize().width/2, cailiaoImg->getContentSize().height/2));

		//数量
		char buf[20];
		sprintf(buf, "\nx%u", items.at(i).count);
		string temp = CS::getItemName(items.at(i).type);
		temp += buf;
		CCLabelTTF *labelShuliang = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, label_size
			, CCSize(0,0), kCCTextAlignmentCenter);
		cailiaoImg->addChild(labelShuliang);
		labelShuliang->setPosition(ccp(cailiaoImg->getContentSize().width/2, -labelShuliang->getContentSize().height/2));
		labelShuliang->setColor(label_color);
	}	
	for (int i=0; i<equips.size(); ++i, ++posIdx)
	{
		BaseSprite* diwenImg = BaseSprite::create(
			ImageManager::getManager()->getImageData_DiWen_WithBianKuang(equips.at(i).zhuangbei_colour)); // .count .type
		mBoard->addChild(diwenImg);

		int pos_x = mBoard->getContentSize().width/2;
		int delta = posIdx - allNum/2;
		if(allNum % 2 == 0) 
		{
			float del = delta + 0.5;
			pos_x += del * (diwenImg->getContentSize().width + 10);
		}
		else
		{
			pos_x += delta * (diwenImg->getContentSize().width + 10);
		}
		diwenImg->setPosition(ccp(pos_x, mBoard->getContentSize().height/2+24));
		//equip pic
		string equipfile = equips.at(i).zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		BaseSprite *s = BaseSprite::create(equipfile);
		diwenImg->addChild(s);
		s->setScale(0.23f);
		s->setPosition(ccp(diwenImg->getContentSize().width/2,diwenImg->getContentSize().height/2));

		//数量
		char buf[20];
		sprintf(buf, "\nx%d", 1);
		string temp = equips.at(i).zhuangbei_name;
		temp += buf;
		CCLabelTTF *labelShuliang = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, label_size
			, CCSize(0,0), kCCTextAlignmentCenter);
		diwenImg->addChild(labelShuliang);
		labelShuliang->setPosition(ccp(diwenImg->getContentSize().width/2, -labelShuliang->getContentSize().height/2));
		labelShuliang->setColor(label_color);
	}


	//
	this->setTag(TAG_GetAward_Layer);
	this->setZOrder(1025);
}

void GetAwardLayer::setMenuContainer(CCMenuItem* item)
{
	if (item == NULL) return;

	CCAssert(item->getParent() == NULL, ""); // 我来添加

	mMenu = CCMenu::create(item, NULL);
	mBoard->addChild(mMenu, 10);
	mMenu->setPosition(ccp(0,0));
	mMenu->setTouchPriority(-131);

	//
	item->setPosition(ccp(mBoard->getContentSize().width/2, 70));
}


void GetAwardLayer::onEnter()
{
	CCLayerColor::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -130, true);
}

void GetAwardLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayerColor::onExit();
}

bool GetAwardLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint touchLocation = pTouch->getLocation();

	CCPoint pos = mBoard->convertToNodeSpace(touchLocation);
	CCRect rect = mBoard->getTextureRect();
	rect.origin = CCPointZero;
	if (rect.containsPoint(pos))
	{
		mBoard->runAction(CCSequence::create(
			CCScaleTo::create(0.1f,1.1f),
			CCScaleTo::create(0.1f,0.9f),
			CCScaleTo::create(0.05f,1.0f),
			NULL
			));
		return true;
	}
	

	this->runAction(CCSequence::create(
		CCScaleTo::create(0.1f, 0.1f),
		CCRemoveSelf::create(),
		NULL));

	return true;
}