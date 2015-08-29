#include "openBoxLayer.h"
#include "GameDirector.h"
#include "CmdHelper.h"
#include "font.h"
#include "ImageManager.h"
#include "ShenBingPool.h"
#include "SuitPool.h"

enum TAG{
	Tag_BoxBg = 1
	, Tag_Label_Num = 2
	, Tag_Box_1 = 11
};


OpenBoxLayer::OpenBoxLayer(OpenBoxFrom from, int openBonNum, vector<GiftData>& gettedData, vector<GiftData>& missedData)
	:mNum(openBonNum)
	,mNowOpenedNum(0)
	,mOpenning(false)
{
	for(int i=0; i!=9; ++i)
		mOpened[i] = false;

	mGettedData = gettedData;
	mMissedData = missedData;


	mFrom = from;
}

OpenBoxLayer::~OpenBoxLayer()
{
	//避免开宝箱时断开网路  重联后跳到主界面 而没有删除该监听
	if(mFrom == fromJingJiChang)
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_arenaGetGiftRsp);
	else if(mFrom = fromTuanDui)
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_teamBossGetGiftRsp);
}

OpenBoxLayer* OpenBoxLayer::create(OpenBoxFrom from, int openBonNum, vector<GiftData> gettedData, vector<GiftData> missedData)
{
	OpenBoxLayer* layer = new OpenBoxLayer(from, openBonNum, gettedData, missedData);
	if(layer && layer->init())
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

bool OpenBoxLayer::init()
{
	if( !CCLayerColor::init() )
		return false;

	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	CCSprite* boxBg = BaseSprite::create("PVP_jiangli_baoxiang_bg.png");
	addChild(boxBg, 0, Tag_BoxBg);
	boxBg->setAnchorPoint(ccp(0.5, 0));
	boxBg->setPosition(ccp(winSize.width/2, 0));

	//box
	for(int i=0; i!=9; ++i)
	{
		//y 216 
		CCSprite* box = BaseSprite::create("zhandou_shengli_baoxiang_2_1.png");
		boxBg->addChild(box, 1, Tag_Box_1+i);
		
		box->setPositionX(320 + (i%3-1)*173);
		box->setPositionY(510 + (i/3-1)*180);
	}

	//次数
	CCString str;
	str.initWithFormat("%s %d", LFStrings::getValue("shengyucishu").c_str(), mNum-mNowOpenedNum);
	CCLabelTTF *label = CCLabelTTF::create(str.getCString(), fontStr_kaiti, 26);
	boxBg->addChild(label, 1, Tag_Label_Num);
	label->setPosition(ccp(320, 140));

	return true;
}


void OpenBoxLayer::onEnter()
{
	CCLayerColor::onEnter();
	//优先级必须比WaitLayer(-1000) 高
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1024, true);
}

void OpenBoxLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayerColor::onExit();
}

bool OpenBoxLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//开完宝箱了
	if(mNowOpenedNum >= mNum)
	{
//		GameDirector::getDirector()->mainSceneFuben();
		return true;
	}

	//正在开其中一个
	if(mOpenning)
		return true;


	CCPoint p =  pTouch->getLocation();

	for(int i=0; i!=9; ++i)
	{	
		CCSprite* box = (CCSprite*)((CCSprite*)getChildByTag(Tag_BoxBg))->getChildByTag(Tag_Box_1+i);
		CCPoint nativePos = box->getParent()->convertToNodeSpace(p);

		if (box->boundingBox().containsPoint(nativePos) && !mOpened[i])
		{
			//xiang服务器发请求
			if(mFrom == fromJingJiChang)
			{
				CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(OpenBoxLayer::getGiftRsp), MSG_arenaGetGiftRsp, NULL);
				CmdHelper::getHelper()->cmdArenaGetGift(mGettedData.at(mNowOpenedNum).gift_id);
			}
			else if(mFrom == fromTuanDui)
			{
				CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(OpenBoxLayer::getGiftRsp), MSG_teamBossGetGiftRsp, NULL);
				CmdHelper::getHelper()->cmdTeamBossGetGift(mGettedData.at(mNowOpenedNum).gift_id);
			}
			mNowOpenedBoxIdx = i;

			break;
		}
	}


	return true;
}

void OpenBoxLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
}

void OpenBoxLayer::getGiftRsp(CCObject* msg)
{
	GameDirector::getDirector()->hideWaiting();
	if(mFrom == fromJingJiChang)
	{
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_arenaGetGiftRsp);
		if(((SPCmd_ArenaGetGiftRsp*)((MSG_Rsp*)msg)->getData())->err_code != Success)
		{
			//
		}
		else
		{
			++mNowOpenedNum;
			mOpened[mNowOpenedBoxIdx] = true;
			mOpenning = true;
			openBox(mNowOpenedBoxIdx);
		}
	}
	else if(fromTuanDui == mFrom)
	{
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_teamBossGetGiftRsp);
		if(((SPCmd_TeamBossGetGiftRsp*)((MSG_Rsp*)msg)->getData())->err_code != Success)
		{
			//
		}
		else
		{
			++mNowOpenedNum;
			mOpened[mNowOpenedBoxIdx] = true;
			mOpenning = true;
			openBox(mNowOpenedBoxIdx);
		}
	}
	
}

void OpenBoxLayer::openBox(int idx)
{
	CCSprite* box = (CCSprite*)((CCSprite*)getChildByTag(Tag_BoxBg))->getChildByTag(Tag_Box_1+idx);

	CCSequence* rotate = CCSequence::create(CCRotateBy::create(0.05f, 2), CCRotateBy::create(0.05f, -3), NULL);
	CCSequence* scale = CCSequence::create(CCScaleTo::create(0.05f, 0.95f), CCScaleTo::create(0.05f, 1.05f), NULL);
	CCCallFuncN* setFrame = CCCallFuncN::create(this, callfuncN_selector(OpenBoxLayer::displayAward));
	box->runAction(CCSequence::create(
		CCSpawn::create(rotate, scale, NULL)
		, setFrame
		, NULL));

	refreshLabel();
}

void OpenBoxLayer::refreshLabel()
{
	int num = mNum - mNowOpenedNum;

	if(num > 0)
	{
		CCLabelTTF *labelNum = (CCLabelTTF*)this->getChildByTag(Tag_BoxBg)->getChildByTag(Tag_Label_Num);
		CCString str;
		str.initWithFormat("%s %d", LFStrings::getValue("shengyucishu").c_str(), num);
		labelNum->setString(str.getCString());
	}
	else if(num == 0)
	{
		CCLabelTTF *labelNum = (CCLabelTTF*)this->getChildByTag(Tag_BoxBg)->getChildByTag(Tag_Label_Num);
		CCPoint pos = labelNum->getPosition();
		labelNum->removeFromParent();

		CCMenuItemImage *item = CCMenuItemImage::create(
			"zhuangbei_xiangxi_queding.png"
			, "zhuangbei_xiangxi_queding_select.png"
			, this, menu_selector(OpenBoxLayer::menuItemOkClicked));
		item->setPosition(pos);
		CCMenu *menu = CCMenu::create(item, NULL);
		this->getChildByTag(Tag_BoxBg)->addChild(menu);
		menu->setPosition(CCPointZero);

		menu->setHandlerPriority(-1025);
	}
	else
	{
		CCLog("Error: OpenBoxLayer::refreshLabel ---> current label num error [num = %d]", num);
	}
}

void OpenBoxLayer::displayAward(CCNode* node)
{
	CCSprite* box = ((CCSprite*)node);
	box->runAction(CCFadeOut::create(0.1f));

	//当前打开的箱子
	if (mGettedData.at(mNowOpenedNum-1).type == GiftType_Jifen)
	{
		CCLabelTTF *label = CCLabelTTF::create(LFStrings::getValue("rongYuDian").c_str(), fontStr_kaiti, 23);
		label->setColor(ccGREEN);
		box->getParent()->addChild(label);
		label->setOpacity(0);
		label->setPosition(ccp(box->getPositionX()-10, box->getPositionY()));
		label->runAction(CCSequence::create(
			CCDelayTime::create(0.1f),
			CCFadeIn::create(0.5f),
			NULL));

		//数量
		char buf[20];
		sprintf(buf, "x%d", mGettedData.at(mNowOpenedNum-1).data_union.item.count);
		CCLabelTTF *labelShuliang = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
		label->addChild(labelShuliang);
		labelShuliang->setAnchorPoint(ccp(0, 0.5f));
		labelShuliang->setPosition(ccp(label->getContentSize().width, label->getContentSize().height/2));
		labelShuliang->setOpacity(0);
		labelShuliang->runAction(CCSequence::create(
			CCDelayTime::create(0.1f)
			, CCFadeIn::create(0.4f)
			, NULL));
	}
	else if(mGettedData.at(mNowOpenedNum-1).type == GiftType_Item)
	{
		CCSprite* display = BaseSprite::create(
			ImageManager::getManager()->getItemFilename(mGettedData.at(mNowOpenedNum-1).data_union.item.type).c_str()); // .count .type
		box->getParent()->addChild(display);
		display->setOpacity(0);
		display->setPosition(box->getPosition());
		display->runAction(CCSequence::create(
			CCDelayTime::create(0.1f)
			, CCFadeIn::create(0.5f)
			, NULL));

		//数量
		char buf[20];
		sprintf(buf, "x%d", mGettedData.at(mNowOpenedNum-1).data_union.item.count);
		CCLabelTTF *labelShuliang = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
		display->addChild(labelShuliang);
		labelShuliang->setAnchorPoint(ccp(0, 0));
		labelShuliang->setPosition(ccp(display->getContentSize().width-18, 10));
		labelShuliang->setOpacity(0);
		labelShuliang->runAction(CCSequence::create(
			CCDelayTime::create(0.1f)
			, CCFadeIn::create(0.4f)
			, NULL));

// 		//partical
// 		{
// 			CCParticleSystemQuad* par = CCParticleSystemQuad::create("partical_settlement_equip.plist");
// 			par->setAutoRemoveOnFinish(true);
// 			par->setScale(0.6);
// 			display->getParent()->addChild(par, 2);
// 			par->setPosition(ccp(display->getPositionX(), display->getPositionY()-30));
// 
// 			ccColor4F c4 = {0x43/255.f, 0x6e/255.f, 0xee/255.f, 0.4};
// 			par->setStartColor(c4);
// 			par->setEndColor(c4);
// 			CCMoveTo *move = CCMoveTo::create(0.2, ccp(display->getPositionX(), display->getPositionY()-65));
// 			par->runAction(CCSequence::create(move, NULL));
// 		}
	}
	else if(mGettedData.at(mNowOpenedNum-1).type == GiftType_Zhuangbei)
	{
		ImageData data = ImageManager::getManager()->getImageData_DiWen_List(
			(ZhuangbeiColour)mGettedData.at(mNowOpenedNum-1).data_union.zhuangbei.zhuangbei_colour
			,false,false);
		BaseSprite *display = BaseSprite::create(data.getCurrentImageFile());
		display->setScale(data.scale / 2);

//此处的zhuangbei_pic好像不是装备图片的全文件名、（目前任务奖励暂无装备、等有装备了再来判定）
		BaseSprite *equipImg = BaseSprite::create(mGettedData.at(mNowOpenedNum-1).data_union.zhuangbei.zhuangbei_pic);
		display->addChild(equipImg);
		equipImg->setPosition(ccp(display->getContentSize().width/2, display->getContentSize().height/2));
		equipImg->setOpacity(0);
		box->getParent()->addChild(display);
		display->setOpacity(0);
		display->setPosition(box->getPosition());
		display->runAction(CCSequence::create(
			CCDelayTime::create(0.1f)
			, CCFadeIn::create(0.5f)
			, NULL));
		equipImg->runAction(CCSequence::create(
			CCDelayTime::create(0.1f)
			, CCFadeIn::create(0.5f)
			, NULL));
		//partical
		{
			CCParticleSystemQuad* par = CCParticleSystemQuad::create("partical_settlement_equip.plist");
			par->setAutoRemoveOnFinish(true);
			par->setScale(0.6f);
			display->getParent()->addChild(par, 2);
			par->setPosition(ccp(display->getPositionX(), display->getPositionY()-30));

			ccColor4F c4 = {0x43/255.f, 0x6e/255.f, 0xee/255.f, 0.4f};
			switch (mGettedData.at(mNowOpenedNum-1).data_union.zhuangbei.zhuangbei_colour)
			{
			case ZhuangbeiColour_Baizhuang:
				c4.r = 0x8b/255.f;
				c4.g = 0x7d/255.f;
				c4.b = 0x6b/255.f;
				break;
			case ZhuangbeiColour_Fumo://
				c4.r = 0x18/255.f;
				c4.g = 0x74/255.f;
				c4.b = 0xcd/255.f;
				break;
			case ZhuangbeiColour_Xiyou:
				c4.r = 0xee/255.f;
				c4.g = 0xc9/255.f;
				c4.b = 0x00/255.f;
				break;
			case ZhuangbeiColour_Chuanqi:
				c4.r = 0xcd/255.f;
				c4.g = 0x66/255.f;
				c4.b = 0x1d/255.f;
				break;
			default:
				break;
			}
			par->setStartColor(c4);
			par->setEndColor(c4);
			CCMoveTo *move = CCMoveTo::create(0.2f, ccp(display->getPositionX(), display->getPositionY()-65));
			par->runAction(CCSequence::create(move, NULL));
		}
	}
	else
	{
		//CCLog("open Jingjichang box [zhuangbei]");
	}


	mOpenning = false;
}

void OpenBoxLayer::menuItemOkClicked(CCObject* pSender)
{
	GameDirector::getDirector()->mainScene(mainlayer_JingJiChang_random);
}