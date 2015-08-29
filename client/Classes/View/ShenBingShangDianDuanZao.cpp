#include "ShenBingShangDianDuanZao.h"
#include "font.h"
#include "ImageManager.h"
#include "SuitPool.h"
#include "CS_Common.h"
#include "GameDirector.h"
#include "flat.h"
#include "MessageDisplay.h"
#include "WuPinManager.h"
#include "EquipmentManager.h"
#include "DuanZaoManager.h"
#include "MyselfManager.h"
#include "EquipmentXiangQingBody.h"
#include "GuideLayerBase.h"
#include "History.h"
#include "ShenBingPool.h"
#include "FX_CommonFunc.h"

using namespace FX_CommonFunc;

extern BaseSprite* getDuanZaoChengGong_EffectSprite(int color, int idx, float actionDelayTime);

ShenBingShangDianDuanZao::ShenBingShangDianDuanZao(vector<EquipData> equips)
{
	mCount = equips.size();
	mEquips = equips;

	mBg = BaseSprite::create("gongjiang_duanzao_zhuangbeizu_neibu_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mWinSize.height - mBg->getContentSize().height/2));

	float EveryOneDelayTime = 1.0f;
	if (mCount == 1) EveryOneDelayTime = 2.0f;
	for(int i=0; i!=equips.size(); ++i)
	{
		//装备
		Equipment *equip = EquipmentManager::getManager()->getOneEquipment(equips.at(i).zhuangbei_id);
		//

		EquipmentXiangQingBody* equipBoard = new EquipmentXiangQingBody(equip, equip->getActorId(), From_ZhuangBeiDuanZao);
		mBg->addChild(equipBoard, 1);
//		equipBoard->setScale(0.67f);
		equipBoard->setPosition(ccp(mBg->getContentSize().width/2 - equipBoard->getContentSize().width/2
			, mBg->getContentSize().height - mWinSize.height/2 - equipBoard->getContentSize().height/2));
		equipBoard->release();

		//光效特效
		int eqColor = equip->getEquipColor();
// 		if (equip->isShenBing() && equip->isPartOfSuit())
// 			eqColor += 3;
// 		else if (equip->isShenBing())
// 			eqColor += 2;
// 		else if(equip->isPartOfSuit())
// 			eqColor += 1;
		// -1 边框光芒
		{
			BaseSprite *lightImg = getDuanZaoChengGong_EffectSprite(eqColor, -1, 0);
			equipBoard->addChild(lightImg, -1);
			lightImg->setPosition(ccp(equipBoard->getContentSize().width/2, equipBoard->getContentSize().height/2));
			lightImg->setScale(1/equipBoard->getScale());
//			lightImg->setScaleY(lightImg->getScaleY()*1.2f);
		}
		// -2 椭圆形不动光
		{
			BaseSprite *lightImg = getDuanZaoChengGong_EffectSprite(eqColor, -2, 0);
			equipBoard->addChild(lightImg, -2);
			lightImg->setPosition(ccp(equipBoard->getContentSize().width/2, equipBoard->getContentSize().height/2));
			lightImg->setScale(1/equipBoard->getScale()*2);
		}

		// -3 圆形缓慢转动光
		{
			BaseSprite *lightImg = getDuanZaoChengGong_EffectSprite(eqColor, -3, 0);
			equipBoard->addChild(lightImg, -3);
			lightImg->setPosition(ccp(equipBoard->getContentSize().width/2, equipBoard->getContentSize().height/2));
			lightImg->setScale(1/equipBoard->getScale()*2);
			lightImg->runAction(CCRepeatForever::create(CCRotateBy::create(5, 30)));
		}
		// -4 忽明忽暗光
		{
			BaseSprite *lightImg = getDuanZaoChengGong_EffectSprite(eqColor, -4, 0);
			equipBoard->addChild(lightImg, -4);
			lightImg->setPosition(ccp(equipBoard->getContentSize().width/2, equipBoard->getContentSize().height/2));
			lightImg->setScale(1/equipBoard->getScale()*2);
			lightImg->runAction(CCRepeatForever::create(CCSequence::create(CCFadeIn::create(1), CCFadeOut::create(1), NULL)));
		}

		int curNum = 0;  //用于结束的回调
		if(equips.size() - 1 == i)                 curNum = 1;
		//
		equipBoard->setPositionX(mBg->getContentSize().width);
		equipBoard->setVisible(false);
		equipBoard->setScale(0.1f);
 		equipBoard->runAction(CCSequence::create(
 			CCDelayTime::create(EveryOneDelayTime * i - i*0.3f),
 			CCShow::create(),
 			CCSpawn::create(CCMoveTo::create(EveryOneDelayTime / 4, ccp(mBg->getContentSize().width/2-equipBoard->getContentSize().width/2, equipBoard->getPositionY())), 
 			CCScaleTo::create(EveryOneDelayTime / 4, 1.0f), NULL),
 			CCDelayTime::create(EveryOneDelayTime / 2),
			CCSpawn::create(CCMoveTo::create(EveryOneDelayTime / 4, ccp(-equipBoard->getContentSize().width/2, equipBoard->getPositionY())), 
			CCScaleTo::create(EveryOneDelayTime / 4, 0.1f), 
			NULL),
			CCHide::create(),
			CCCallFuncND::create(this, callfuncND_selector(ShenBingShangDianDuanZao::effectEndCallback), (void*)curNum),
 			NULL));

		CCLayerColor *node = CCLayerColor::create(ccc4(255,255,255,0));
		this->addChild(node, 100);
		node->runAction(CCSequence::create(
			CCDelayTime::create(EveryOneDelayTime * (mCount-1) - (mCount-1)*0.3f + EveryOneDelayTime / 2),
			CCFadeTo::create(1.0f, 30),
			CCCallFunc::create(this, callfunc_selector(ShenBingShangDianDuanZao::display10Equip)),
			CCFadeTo::create(0.5f, 0),
			CCRemoveSelf::create(),
			NULL));
	}
}

void ShenBingShangDianDuanZao::onEnter()
{
	BaseLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -130, true);
}

void ShenBingShangDianDuanZao::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	BaseLayer::onExit();
}

bool ShenBingShangDianDuanZao::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}




//zOrder    -1详情边框光效  -2不动光   -3缓慢转动光   -4忽明忽暗光   不是-1都要变大一倍
BaseSprite* getDuanZaoChengGong_EffectSprite(int color, int idx, float actionDelayTime)
{
	string fileName;
	switch (color)
	{
	case ZhuangbeiColour_Baizhuang:
		fileName = "duanzao_chenggong_texiao_putong_";
		break;
	case ZhuangbeiColour_Fumo:
		fileName = "duanzao_chenggong_texiao_mofa_";
		break;
	case ZhuangbeiColour_Xiyou:
		fileName = "duanzao_chenggong_texiao_xiyou_";
		break;
	case ZhuangbeiColour_Chuanqi:
		fileName = "duanzao_chenggong_texiao_chuanqi_";
		break;
	case ZhuangbeiColour_Chuanqi+1:
		fileName = "duanzao_chenggong_texiao_taozhuang_";
		break;
	case ZhuangbeiColour_Chuanqi+2:
		fileName = "duanzao_chenggong_texiao_shenbing_";
		break;
	case ZhuangbeiColour_Chuanqi+3:
		fileName = "duanzao_chenggong_texiao_shenbing_taozhuang_";
		break;
	default:
		break;
	}

	switch (idx)
	{
	case -1:
		fileName += "bg.png";
		break;
	case -2:
		fileName += "1.png";
		break;
	case -3:
		fileName += "2.png";
		break;
	case -4:
		fileName += "3.png";
		break;
	default:
		break;
	}


	return BaseSprite::create(fileName);
}

void ShenBingShangDianDuanZao::effectEndCallback(CCNode *node, void *data)
{
	int num = (int)data;

	node->removeFromParent();

	if (num == 1 && mCount == 1)
	{
		this->removeFromParent();
	}
}

void ShenBingShangDianDuanZao::display10Equip()
{
	CCMenu *menu = CCMenu::create();
	mBg->addChild(menu, 1);
	menu->setPosition(ccp(0,0));
	menu->setTouchPriority(-131);
	CCMenuItemImage *backItem = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(ShenBingShangDianDuanZao::menuItemClicked_EquipImg));
	menu->addChild(backItem, 1, 1);
	backItem->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-873));
	//
	float scale = 0.3f;
	for (int i=0; i<mCount; ++i)
	{
		string cardFile = ImageManager::getManager()->getImageData_DiWen_XiangQing(mEquips.at(i).zhuangbei_colour).getCurrentImageFile();
		string equipfile = mEquips.at(i).equipPic;
		equipfile = "minute_" + equipfile + ".png";

		CCMenuItemImage *item = CCMenuItemImage::create(cardFile.c_str(), cardFile.c_str(), this, 
			menu_selector(ShenBingShangDianDuanZao::menuItemClicked_EquipImg));
		CCPoint pos;
		pos.x = 320 + (i%5 - 2)*120;
		pos.y = mBg->getContentSize().height - 294 - i/5*250;
		item->setPosition(pos);
		menu->addChild(item, 0, 10+i);
		item->setScale(scale);
		BaseSprite *eq = BaseSprite::create(equipfile);
		item->addChild(eq, 0, 1);
		eq->setPosition(ccp(item->getContentSize().width/2, item->getContentSize().height/2));
		//name
		char buf[128];
		sprintf(buf, "%s", mEquips.at(i).equipName.c_str());
		CCLabelTTF *nameLabel = CCLabelTTF::create(buf, fontStr_kaiti, 20, CCSize(106,0), kCCTextAlignmentCenter);
		item->addChild(nameLabel);
		nameLabel->setPosition(ccp(item->getContentSize().width/2, -nameLabel->getContentSize().height/2/item->getScale()));
		nameLabel->setScale(1 / item->getScale());
	}
}

void ShenBingShangDianDuanZao::menuItemClicked_EquipImg(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if (tag == 1)
	{
		this->removeFromParent();
	}
	else
	{
		int idx = tag - 10;
	}
}