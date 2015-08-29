#include "VarietyStoreLayer.h"
#include "CommonDefine.h"
#include "MyselfManager.h"
#include "WuPinManager.h"
#include "font.h"
#include "ZahuopuHelper.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "SkillTree.h"
#include "flat.h"
#include "FX_CommonFunc.h"

using namespace FX_CommonFunc;

#define LABEL_SIZE 23
#define LABEL_COLOR ccc3(195,145,96)

#define OPEN_CARD_TIME 1.0f

enum
{
	//head
	Tag_Label_PlayerName = 10,
	Tag_Label_Gold,
	Tag_Label_Silver,

	//bg
	Tag_Label_DaiGouJuan, //刚进入的时候，提示当前有多少代沟卷
	Tag_Label_DaiGouJuanNum,
	Tag_Img_DaiGouJuan,

	Tag_Label_Need_DaiGouJuanNum, //抽取的时候提示本次消耗多少代购卷
	Tag_Img_Need_DaiGouJuan,

	Tag_Label_Direction,
	Tag_Label_RefreshSilver,
	Tag_Img_SilverIcon,
	Tag_Label_NeedGoldNum,

	Tag_MenuItem_Refresh,
	Tag_MenuItem_Start,
	Tag_MenuItem_GiveUp,

	Tag_CardMenuItem_  = 50,
	Tag_CardOpened = 100,
	Tag_CardClosed,
	Tag_Img_Item,
	Tag_Label_CardName,
	Tag_Label_CardNum,
};


bool VarietyStoreLayer::init()
{
	mCurStatus = false;
	//1. bg
	mBg = BaseSprite::create("zahuopu_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));
	//2. head
	mHead = BaseSprite::create("zhuangtailan_jinqian.png");
	mBg->addChild(mHead, 0);
	mHead->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height + mHead->getContentSize().height/2));
	//金币银币图标
	CCSprite* pJinbi = CCSprite::create("jinbi_tubiao.png");
	mHead->addChild(pJinbi, 1);
	pJinbi->setPosition(ccp(253, 36));

	CCSprite* pYinbi = CCSprite::create("yinbi_tubiao.png");
	mHead->addChild(pYinbi, 1);
	pYinbi->setPosition(ccp(450, pJinbi->getPositionY()));
	{
		BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
		BaseActorProperty pro = actor->getCurrentProperty();
		CCLabelTTF* name = LF::lable(mHead, pro.customName, ccp(33, mHead->getContentSize().height/2), 25, fontStr_BookAntiqua);
		name->setTag(Tag_Label_PlayerName);

		WuPin* gold = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		WuPin* yingBi = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
		int goldNum = gold!=NULL?gold->getCount():0;
		int yinBiNum = yingBi!=NULL?yingBi->getCount():0;

		LF::lable(mHead,goldNum,ccp(282,mHead->getContentSize().height/2),24,fontStr_BookAntiqua)->setTag(Tag_Label_Gold);
		LF::lable(mHead,yinBiNum,ccp(485,mHead->getContentSize().height/2),24,fontStr_BookAntiqua)->setTag(Tag_Label_Silver);
	}
	//3. direction
	CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("zaHuoPu_ShuaXinTishi").c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(tishiLabel, 1, Tag_Label_Direction);
	tishiLabel->setPosition(ccp(320, 761-626));
	tishiLabel->setColor(LABEL_COLOR);
	//4. menu
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 1);
	mMainMenu->setPosition(ccp(0,0));
	{
		CCMenuItemImage *itemSx = CCMenuItemImage::create(
			"zahuopu_anniu_shuaxin.png",
			"zahuopu_anniu_shuaxin_select.png",
			this, menu_selector(VarietyStoreLayer::menuItemClicked_MainMenu));
		mMainMenu->addChild(itemSx, 0, Tag_MenuItem_Refresh);
		itemSx->setPosition(ccp(186, 761-709));
		CCMenuItemImage *itemKs = CCMenuItemImage::create(
			"zahuopu_anniu_kaishi.png",
			"zahuopu_anniu_kaishi_select.png",
			this, menu_selector(VarietyStoreLayer::menuItemClicked_MainMenu));
		mMainMenu->addChild(itemKs, 0, Tag_MenuItem_Start);
		itemKs->setPosition(ccp(453, 761-709));
		CCMenuItemImage *giveUp = CCMenuItemImage::create(
			"zahuopu_anniu_shuaxin.png",
			"zahuopu_anniu_shuaxin_select.png",
			this, menu_selector(VarietyStoreLayer::menuItemClicked_MainMenu));
		mMainMenu->addChild(giveUp, 0, Tag_MenuItem_GiveUp);
		giveUp->setPosition(ccp(mWinSize.width/2, 761-709));
		giveUp->setVisible(false);
	}
	//5. refresh need silver
	BaseSprite *icon = BaseSprite::create("yinbi_tubiao.png");
	mBg->addChild(icon, 1, Tag_Img_SilverIcon);
	icon->setScale(0.7f);
	icon->setPosition(ccp(160, 761-661));
	//
	CCLabelTTF *silLabel = CCLabelTTF::create("", fontStr_BookAntiqua, LABEL_SIZE);
	mBg->addChild(silLabel, 1, Tag_Label_RefreshSilver);
	silLabel->setAnchorPoint(ccp(0, 0.5));
	silLabel->setPosition(ccp(icon->getPositionX()+icon->getContentSize().width/2, icon->getPositionY()));

	//6. card menu
	mCardMenu = CCMenu::create();
	mBg->addChild(mCardMenu, 1);
	mCardMenu->setPosition(ccp(0,0));
	for (int i=0; i!=6; ++i)
	{
		CCMenuItemImage *cardItem = CCMenuItemImage::create(
			"zahuopu_pai_zhengmian.png",
			"zahuopu_pai_zhengmian.png",
			this, menu_selector(VarietyStoreLayer::menuItemClicked_ChoseCard));
		mCardMenu->addChild(cardItem, 0, Tag_CardMenuItem_ + i);
		cardItem->setPosition(ccp(160 + i%3*160, 761-210-i/3*235));
		cardItem->setOpacity(0);
		cardItem->setEnabled(false);
	}
	{
		addObserver(callfuncO_selector(VarietyStoreLayer::rsp_QueryZaHuoPu), MSG_queryZahuopuRsp);
		mCmdHlp->cmdQueryZahuopu();
	}

	return true;
}

void VarietyStoreLayer::rsp_QueryZaHuoPu(CCObject *msg)
{
	removeObserver(MSG_queryZahuopuRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_QueryZahuopuRsp* data = (SPCmd_QueryZahuopuRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue(mCmdHlp->getMessage(data->err_code)));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	//刷新所需银币
	char buf[20];
	sprintf(buf, "%u", ZahuopuHelper::getHelper()->getRefreshNeedSilver());
	CCLabelTTF *refreshSilLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_RefreshSilver);
	refreshSilLabel->setString(buf);
	//

	vector<ZahuopuItem> items = ZahuopuHelper::getHelper()->getCurrentZaHuiList();
	for (int i=0; i!=items.size(); ++i)
		if (items.at(i).already_got)
		{
			mCurStatus = true;
			//
			mMainMenu->getChildByTag(Tag_MenuItem_GiveUp)->setVisible(true);
			mMainMenu->getChildByTag(Tag_MenuItem_Refresh)->setVisible(false);
			mMainMenu->getChildByTag(Tag_MenuItem_Start)->setVisible(false);
			//
			BaseSprite *icon = (BaseSprite* )mBg->getChildByTag(Tag_Img_SilverIcon);
			icon->setPositionX(mBg->getContentSize().width/2 - 20);
			mBg->getChildByTag(Tag_Label_RefreshSilver)->setPosition(
				ccp(icon->getPositionX()+icon->getContentSize().width/2, icon->getPositionY()));
			//
			CCLabelTTF *direcLabel = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_Direction);
			direcLabel->setString(LFStrings::getValue("zaHuoPu_chouQuSuoXu").c_str());
			direcLabel->setPositionX(mBg->getContentSize().width/2 - 20);
			//
			//代购卷
			int dgjAll = WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount();
			int dgjNeed = ZahuopuHelper::getHelper()->getPickItemNeedDaiGouJuan();
			if (dgjAll > 0)
			{
				CCSprite *dgjImg = CCSprite::create("daoju_daigouoquan.png");
				mBg->addChild(dgjImg, 1, Tag_Img_Need_DaiGouJuan);
				dgjImg->setScale(0.5f);
				dgjImg->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+dgjImg->getContentSize().width/2*dgjImg->getScaleX(),
					direcLabel->getPositionY()));
				char buf[32];
				if (dgjNeed <= dgjAll) sprintf(buf, " x%d", dgjNeed);
				else sprintf(buf, " x%d", dgjAll);
				CCLabelTTF *dgjNumLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
				mBg->addChild(dgjNumLabel, 1, Tag_Label_Need_DaiGouJuanNum);
				dgjNumLabel->setPosition(ccp(dgjImg->getPositionX()+dgjImg->getContentSize().width/2*dgjImg->getScaleX()+dgjNumLabel->getContentSize().width/2-16,
					dgjImg->getPositionY()));
			}
			//金币
			CCLabelTTF *needGoldNumLabel = CCLabelTTF::create(
				CCString::createWithFormat("%u", CS::zahuopuGetOneItemNeedGold(WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount()))->getCString(),
				fontStr_BookAntiqua, 
				LABEL_SIZE);
			mBg->addChild(needGoldNumLabel, 1, Tag_Label_NeedGoldNum);
			needGoldNumLabel->setAnchorPoint(ccp(0, 0.5f));
			if (dgjAll > 0)
				needGoldNumLabel->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+126, direcLabel->getPositionY()));
			else
				needGoldNumLabel->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+40, direcLabel->getPositionY()));
			BaseSprite *goldIcon = BaseSprite::create("jinbi_tubiao.png");
			needGoldNumLabel->addChild(goldIcon);
			goldIcon->setPosition(ccp(-goldIcon->getContentSize().width/2, needGoldNumLabel->getContentSize().height/2));
			goldIcon->setScale(0.7f);
			break;
		}
	for (int i=0; i!=items.size() && i != 6; ++i)
	{
		CCMenuItemImage *cardItem = (CCMenuItemImage*)mCardMenu->getChildByTag(Tag_CardMenuItem_ + i);
		CCSize size = cardItem->getContentSize();
		//opened board
		BaseSprite *board = BaseSprite::create("zahuopu_pai_zhengmian.png");
		cardItem->addChild(board, 1, Tag_CardOpened);
		board->setPosition(ccp(size.width/2, size.height/2));
		//mask( closed board)
		BaseSprite *maskImg = BaseSprite::create("zahuopu_pai_zbeimian.png");
		cardItem->addChild(maskImg, board->getZOrder()+1, Tag_CardClosed);
		maskImg->setPosition(board->getPosition());
		maskImg->setVisible(false);
		//item img
		BaseSprite *itemImg = NULL;
		if (items.at(i).suipian_id == 0)
			itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(items.at(i).item_type));
		else 
			itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(items.at(i).item_type));
		board->addChild(itemImg, 0, Tag_Img_Item);
		itemImg->setPosition(ccp(70, 205-87));
		// item name label
		string name;
		if (items.at(i).item_type >= ItemsType_SkillBook_MinId && items.at(i).item_type <= ItemsType_SkillBook_MaxId)
			name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(items.at(i).item_type - ItemsType_SkillBook_MinId), true);
		else 
			name = CS::getItemName(items.at(i).item_type);
		if (items.at(i).suipian_id != 0)
			name = name + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(items.at(i).suipian_id);
		char buf[20];
		sprintf(buf, " x%u", items.at(i).count);
		name += buf;

		CCLabelTTF *nameLabel = CCLabelTTF::create(name.c_str(), fontStr_kaiti, 20, CCSize(142,0), kCCTextAlignmentCenter);
		cardItem->addChild(nameLabel, 1, Tag_Label_CardName);
		nameLabel->setAnchorPoint(ccp(0, 1));
		nameLabel->setPosition(ccp((cardItem->getContentSize().width-nameLabel->getContentSize().width)/2,50));
		//num
// 		CCLabelTTF *numLabel = CCLabelTTF::create(buf, fontStr_kaiti, 20);
// 		cardItem->addChild(numLabel, 1, Tag_Label_CardNum);
// 		numLabel->setAnchorPoint(ccp(0, 1));
// 		nameLabel->setPositionX((cardItem->getContentSize().width-nameLabel->getContentSize().width
// 			-numLabel->getContentSize().width)/2);
// 		numLabel->setPosition(ccp(nameLabel->getPositionX()+nameLabel->getContentSize().width, nameLabel->getPositionY()));
		//
		if (mCurStatus && ! items.at(i).already_got)
		{
			cardItem->setEnabled(true);
			board->setVisible(false);
			maskImg->setVisible(true);
//			numLabel->setOpacity(0);
			nameLabel->setOpacity(0);
		}
	}

	if (! mCurStatus) //还没有抽取过、 显示代购卷总数
	{
		//3.0 代购卷个数
		CCLabelTTF *dgjLabel = CCLabelTTF::create(LFStrings::getValue("YongYouDaiGouJuan_").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(dgjLabel, 1, Tag_Label_DaiGouJuan);
		dgjLabel->setAnchorPoint(ccp(1, 0.5f));
		dgjLabel->setPosition(ccp(340, 761-626+30));
		CCSprite *dgjImg = CCSprite::create("daoju_daigouoquan.png");
		mBg->addChild(dgjImg, 1, Tag_Img_DaiGouJuan);
		dgjImg->setScale(0.5f);
		dgjImg->setPosition(ccp(dgjLabel->getPositionX()+dgjImg->getContentSize().width/2*dgjImg->getScaleX(), dgjLabel->getPositionY()));
		char buf[32];
		sprintf(buf, " x%d", WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount());
		CCLabelTTF *dgjNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		mBg->addChild(dgjNum, 1, Tag_Label_DaiGouJuanNum);
		dgjNum->setPosition(ccp(dgjImg->getPositionX()+dgjImg->getContentSize().width/2*dgjImg->getScaleX()+dgjNum->getContentSize().width/2
			, dgjImg->getPositionY()));
	}
}

void VarietyStoreLayer::rsp_RefreshItems(CCObject *msg)
{
	removeObserver(MSG_refreshZahuopuRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_RefreshZahuopuRsp* data = (SPCmd_RefreshZahuopuRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue(mCmdHlp->getMessage(data->err_code)));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	CCString temp;
	temp.initWithFormat("%u", data->silver_balance);
	((CCLabelTTF *)mHead->getChildByTag(Tag_Label_Silver))->setString(temp.getCString());

	//初始化几面
	{
		mCurStatus = false;
		//刷新所需银币
		BaseSprite *icon = (BaseSprite*)mBg->getChildByTag(Tag_Img_SilverIcon);
		icon->setPositionX(160);
		char buf[20];
		sprintf(buf, "%u", ZahuopuHelper::getHelper()->getRefreshNeedSilver());
		CCLabelTTF *refreshSilLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_RefreshSilver);
		refreshSilLabel->setString(buf);
		refreshSilLabel->setPosition(ccp(icon->getPositionX()+icon->getContentSize().width/2, icon->getPositionY()));
		//
		mBg->removeChildByTag(Tag_Label_NeedGoldNum);
		//
		((CCLabelTTF *)mBg->getChildByTag(Tag_Label_Direction))->setString(LFStrings::getValue("zaHuoPu_ShuaXinTishi").c_str());
		//
		mMainMenu->getChildByTag(Tag_MenuItem_Start)->setVisible(true);
		mMainMenu->getChildByTag(Tag_MenuItem_Refresh)->setVisible(true);
		mMainMenu->getChildByTag(Tag_MenuItem_GiveUp)->setVisible(false);
	}
	//

	vector<ZahuopuItem> items = ZahuopuHelper::getHelper()->getCurrentZaHuiList();
	for (int i=0; i!=items.size() && i != 6; ++i)
	{
		CCMenuItemImage *cardItem = (CCMenuItemImage*)mCardMenu->getChildByTag(Tag_CardMenuItem_ + i);
		cardItem->setEnabled(false);
		CCSize size = cardItem->getContentSize();
//		cardItem->removeAllChildren();
		cardItem->removeChildByTag(Tag_Label_CardName);
//		cardItem->removeChildByTag(Tag_Label_CardNum);
		cardItem->removeChildByTag(Tag_CardClosed);
		cardItem->removeChildByTag(Tag_CardOpened);
		//
		//opened board
		BaseSprite *board = BaseSprite::create("zahuopu_pai_zhengmian.png");
		cardItem->addChild(board, 0, Tag_CardOpened);
		board->setPosition(ccp(size.width/2, size.height/2));
		//mask( closed board)
		BaseSprite *maskImg = BaseSprite::create("zahuopu_pai_zbeimian.png");
		cardItem->addChild(maskImg, board->getZOrder()+1, Tag_CardClosed);
		maskImg->setPosition(board->getPosition());
		maskImg->setVisible(false);
		//item img
		BaseSprite *itemImg = NULL;
		if (items.at(i).suipian_id == 0)
			itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(items.at(i).item_type));
		else 
			itemImg = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(items.at(i).item_type));
		board->addChild(itemImg, 0, Tag_Img_Item);
		itemImg->setPosition(ccp(70, 205-87));
		// item name label
		string name;
		if (items.at(i).item_type >= ItemsType_SkillBook_MinId && items.at(i).item_type <= ItemsType_SkillBook_MaxId)
			name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(items.at(i).item_type - ItemsType_SkillBook_MinId), true);
		else 
			name = CS::getItemName(items.at(i).item_type);
		if (items.at(i).suipian_id != 0)
			name = name + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(items.at(i).suipian_id);
		char buf[20];
		sprintf(buf, " x%u", items.at(i).count);
		name += buf;
		CCLabelTTF *nameLabel = CCLabelTTF::create(name.c_str(), fontStr_kaiti, 20, CCSize(142,0), kCCTextAlignmentCenter);
		cardItem->addChild(nameLabel, 1, Tag_Label_CardName);
		nameLabel->setAnchorPoint(ccp(0, 1));
		nameLabel->setPosition(ccp((cardItem->getContentSize().width-nameLabel->getContentSize().width)/2, 55));
// 		//num
// 		CCLabelTTF *numLabel = CCLabelTTF::create(buf, fontStr_kaiti, 20);
// 		cardItem->addChild(numLabel, 1, Tag_Label_CardNum);
// 		numLabel->setAnchorPoint(ccp(0, 1));
// 		nameLabel->setPositionX((cardItem->getContentSize().width-nameLabel->getContentSize().width
// 			-numLabel->getContentSize().width)/2);
// 		numLabel->setPosition(ccp(nameLabel->getPositionX()+nameLabel->getContentSize().width, nameLabel->getPositionY()));
	}
}

void VarietyStoreLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	CCMenuItemImage *item = (CCMenuItemImage*)pSender;
	int tag = item->getTag();

	{
		mBg->removeChildByTag(Tag_Label_DaiGouJuan);
		mBg->removeChildByTag(Tag_Label_DaiGouJuanNum);
		mBg->removeChildByTag(Tag_Img_DaiGouJuan);
		mBg->removeChildByTag(Tag_Label_Need_DaiGouJuanNum);
		mBg->removeChildByTag(Tag_Img_Need_DaiGouJuan);
	}

	if (tag == Tag_MenuItem_Refresh || tag == Tag_MenuItem_GiveUp)
	{
		if (WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount() < ZahuopuHelper::getHelper()->getRefreshNeedSilver())
		{
			MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue("suoxuyinbibuzu"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		{
			//3.0 代购卷个数
			CCLabelTTF *dgjLabel = CCLabelTTF::create(LFStrings::getValue("YongYouDaiGouJuan_").c_str(), fontStr_kaiti, LABEL_SIZE);
			mBg->addChild(dgjLabel, 1, Tag_Label_DaiGouJuan);
			dgjLabel->setAnchorPoint(ccp(1, 0.5f));
			dgjLabel->setPosition(ccp(340, 761-626+30));
			CCSprite *dgjImg = CCSprite::create("daoju_daigouoquan.png");
			mBg->addChild(dgjImg, 1, Tag_Img_DaiGouJuan);
			dgjImg->setScale(0.5f);
			dgjImg->setPosition(ccp(dgjLabel->getPositionX()+dgjImg->getContentSize().width/2*dgjImg->getScaleX(), dgjLabel->getPositionY()));
			char buf[32];
			sprintf(buf, " x%d", WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount());
			CCLabelTTF *dgjNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
			mBg->addChild(dgjNum, 1, Tag_Label_DaiGouJuanNum);
			dgjNum->setPosition(ccp(dgjImg->getPositionX()+dgjImg->getContentSize().width/2*dgjImg->getScaleX()+dgjNum->getContentSize().width/2
				, dgjImg->getPositionY()));
		}

		//
		addObserver(callfuncO_selector(VarietyStoreLayer::rsp_RefreshItems), MSG_refreshZahuopuRsp);
		mCmdHlp->cmdRefreshZahuopu();
	}
	else if (tag == Tag_MenuItem_Start)
	{
		disableAllTouchBegin();
		CCLabelTTF *dirLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Direction);
		dirLabel->setString(LFStrings::getValue("zaHuoPu_shuaXinING").c_str());
// 		//代购卷
// 		{
// 			mBg->getChildByTag(Tag_Label_DaiGouJuan)->setVisible(false);
// 			mBg->getChildByTag(Tag_Label_DaiGouJuanNum)->setVisible(false);
// 			mBg->getChildByTag(Tag_Img_DaiGouJuan)->setVisible(false);
// 		}
		//
		for (int i=0; i!=ZahuopuHelper::getHelper()->getCurrentZaHuiList().size() && i!=6; ++i)
		{
			CCMenuItemImage *cardItem = (CCMenuItemImage*)mCardMenu->getChildByTag(Tag_CardMenuItem_ + i);
			//名字 和 数量消失
			CCLabelTTF *nameLabel = (CCLabelTTF*)cardItem->getChildByTag(Tag_Label_CardName);
//			CCLabelTTF *numLabel  = (CCLabelTTF*)cardItem->getChildByTag(Tag_Label_CardNum);
			nameLabel->runAction(CCFadeOut::create(0.5f));
//			numLabel->runAction(CCFadeOut::create(0.5f));
			//正面卡牌开始翻转
			BaseSprite *openedCard = (BaseSprite*)cardItem->getChildByTag(Tag_CardOpened);
			openedCard->runAction(CCSequence::create(
				CCOrbitCamera::create(OPEN_CARD_TIME * .5, 1, 0, 0, 90, 0, 0),  
				CCHide::create(),  
				CCDelayTime::create(OPEN_CARD_TIME * .5),  
				NULL));  
			//背面卡牌开始翻转
			BaseSprite *closedCard = (BaseSprite*)cardItem->getChildByTag(Tag_CardClosed);
			closedCard->runAction(CCSequence::create(
				CCDelayTime::create(OPEN_CARD_TIME * .5),  
				CCShow::create(),  
				CCOrbitCamera::create(OPEN_CARD_TIME * .5, 1, 0, 270, 90, 0, 0),  
				NULL));
		}
		scheduleOnce(schedule_selector(VarietyStoreLayer::schedule_ShuffleCard), OPEN_CARD_TIME + 0.1f);
	}
}

void VarietyStoreLayer::schedule_ShuffleCard(float dt)
{
	mMainMenu->getChildByTag(Tag_MenuItem_Start)->setVisible(false);
	mMainMenu->getChildByTag(Tag_MenuItem_Refresh)->setVisible(false);
	mMainMenu->getChildByTag(Tag_MenuItem_GiveUp)->setVisible(true);

	//
	BaseSprite *icon = (BaseSprite* )mBg->getChildByTag(Tag_Img_SilverIcon);
	icon->setPositionX(mBg->getContentSize().width/2 - 20);
	mBg->getChildByTag(Tag_Label_RefreshSilver)->setPosition(
		ccp(icon->getPositionX()+icon->getContentSize().width/2, icon->getPositionY()));
	//本次消耗
	CCLabelTTF *direcLabel = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_Direction);
	direcLabel->setString(LFStrings::getValue("zaHuoPu_chouQuSuoXu").c_str());
	direcLabel->setPositionX(mBg->getContentSize().width/2 - 20);
	//代购卷
	int dgjAll = WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount();
	int dgjNeed = ZahuopuHelper::getHelper()->getPickItemNeedDaiGouJuan();
	if (dgjAll > 0)
	{
		CCSprite *dgjImg = CCSprite::create("daoju_daigouoquan.png");
		mBg->addChild(dgjImg, 1, Tag_Img_Need_DaiGouJuan);
		dgjImg->setScale(0.5f);
		dgjImg->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+dgjImg->getContentSize().width/2*dgjImg->getScaleX(),
			direcLabel->getPositionY()));
		char buf[32];
		if (dgjNeed <= dgjAll) sprintf(buf, " x%d", dgjNeed);
		else sprintf(buf, " x%d", dgjAll);
		CCLabelTTF *dgjNumLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		mBg->addChild(dgjNumLabel, 1, Tag_Label_Need_DaiGouJuanNum);
		dgjNumLabel->setPosition(ccp(dgjImg->getPositionX()+dgjImg->getContentSize().width/2*dgjImg->getScaleX()+dgjNumLabel->getContentSize().width/2-16,
			dgjImg->getPositionY()));
	}
	//金币
	CCLabelTTF *needGoldNumLabel = CCLabelTTF::create(
		CCString::createWithFormat("%u", CS::zahuopuGetOneItemNeedGold(dgjAll))->getCString(),
		fontStr_BookAntiqua, LABEL_SIZE);
	mBg->addChild(needGoldNumLabel, 1, Tag_Label_NeedGoldNum);
	needGoldNumLabel->setAnchorPoint(ccp(0, 0.5f));
	if (dgjAll > 0)
		needGoldNumLabel->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+126, direcLabel->getPositionY()));
	else
		needGoldNumLabel->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+40, direcLabel->getPositionY()));
	BaseSprite *goldIcon = BaseSprite::create("jinbi_tubiao.png");
	needGoldNumLabel->addChild(goldIcon);
	goldIcon->setPosition(ccp(-goldIcon->getContentSize().width/2, needGoldNumLabel->getContentSize().height/2));
	goldIcon->setScale(0.7f);
	if (dgjNeed <= dgjAll)
	{
		needGoldNumLabel->setVisible(false);
	}


	//洗牌
	CCMenuItemImage *baseCard = dynamic_cast<CCMenuItemImage*>(mCardMenu->getChildByTag(Tag_CardMenuItem_));
	CCPoint pos = baseCard->getPosition();
	float time = 0.5f;
	for (int i=1; i!=6; ++i)
	{
		CCMenuItemImage *cardItem = dynamic_cast<CCMenuItemImage*>(mCardMenu->getChildByTag(Tag_CardMenuItem_ + i));
		if (cardItem != NULL)
		{
			CCCallFunc *func = NULL;
			if (i == 5)
				func = CCCallFunc::create(this, callfunc_selector(VarietyStoreLayer::callback_shuffleEnd));

			CCPoint oriPos = cardItem->getPosition();
			cardItem->runAction(CCSequence::create(
				CCSpawn::create(
					CCMoveTo::create(time, pos),
					CCEaseSineIn::create(CCRotateBy::create(time, -360)),
					NULL),
				CCDelayTime::create(time/2),
				CCSpawn::create(
					CCMoveTo::create(time, oriPos),
					CCEaseSineOut::create(CCRotateBy::create(time, 360)),
					NULL),
				func,
				NULL));
		}
	}

}

void VarietyStoreLayer::callback_shuffleEnd()
{
	//
	disableAllTouchEnd();
	for (int i=0; i!=6; ++i)
	{
		CCMenuItemImage *cardItem = dynamic_cast<CCMenuItemImage*>(mCardMenu->getChildByTag(Tag_CardMenuItem_ + i));
		if (cardItem != NULL) cardItem->setEnabled(true);
	}
}

void VarietyStoreLayer::menuItemClicked_ChoseCard(CCObject *pSender)
{
	mChoseItem = (CCMenuItemImage*)pSender;

	//金币够不够
	{
		if (WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount() 
			< CS::zahuopuGetOneItemNeedGold(WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount()))
		{
			MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue("suoxujinbibuzu"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
	}

	mChoseItem->setEnabled(false);
	addObserver(callfuncO_selector(VarietyStoreLayer::rsp_GetOneItem), MSG_zahuopuGetOneItemRsp_F);
	mCmdHlp->cmdZahuopuGetOneItem();
}

void VarietyStoreLayer::rsp_GetOneItem(CCObject *msg_f)
{
	removeObserver(MSG_zahuopuGetOneItemRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_ZahuopuGetOneItemRsp *data  = (Flat_ZahuopuGetOneItemRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue(mCmdHlp->getMessage(data->err_code)));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	//gold --  //还有可能抽到银币 金币
	CCString temp;
	temp.initWithFormat("%d", WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount());
	((CCLabelTTF *)mHead->getChildByTag(Tag_Label_Gold))->setString(temp.getCString());
	//silver
	temp.initWithFormat("%d", WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount());
	((CCLabelTTF *)mHead->getChildByTag(Tag_Label_Silver))->setString(temp.getCString());
	//item data
	ZahuopuItem item = ZahuopuHelper::getHelper()->getItemByIndex(data->get_index);
	

	CCMenuItemImage *cardItem = mChoseItem;
	int idx = cardItem->getTag() - Tag_CardMenuItem_;
	
	//item img
	BaseSprite *itemImg = (BaseSprite*)cardItem->getChildByTag(Tag_CardOpened)->getChildByTag(Tag_Img_Item);
	if (item.suipian_id == 0)
		itemImg->initWithFile(ImageManager::getManager()->getItemFilename(item.item_type));
	else 
		itemImg->initWithFile(ImageManager::getManager()->getItemDebrisFileName(item.item_type));
	//名字 和 数量出现
	CCLabelTTF *nameLabel = (CCLabelTTF*)cardItem->getChildByTag(Tag_Label_CardName);
//	CCLabelTTF *numLabel  = (CCLabelTTF*)cardItem->getChildByTag(Tag_Label_CardNum);
	string name;
	if (item.item_type >= ItemsType_SkillBook_MinId && item.item_type <= ItemsType_SkillBook_MaxId)
		name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(item.item_type - ItemsType_SkillBook_MinId), true);
	else 
		name = CS::getItemName(item.item_type);
	if (item.suipian_id != 0)
		name = name + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(item.suipian_id);
	temp.initWithFormat("%s x%u", name.c_str(), item.count);
	nameLabel->setString(temp.getCString());
/*	nameLabel->setPosition(ccp(cardItem->getContentSize().width/2, 6));*/
//	numLabel->setString(temp.getCString());
// 	numLabel->setPosition(ccp(nameLabel->getPositionX()+nameLabel->getContentSize().width/2
// 		, nameLabel->getPositionY()-nameLabel->getContentSize().height/2));
// 	nameLabel->setPositionX((cardItem->getContentSize().width-nameLabel->getContentSize().width
// 		-numLabel->getContentSize().width)/2);
// 	numLabel->setPosition(ccp(nameLabel->getPositionX()+nameLabel->getContentSize().width, nameLabel->getPositionY()));
	nameLabel->setPositionX((cardItem->getContentSize().width-nameLabel->getContentSize().width)/2);
	nameLabel->runAction(CCFadeIn::create(0.5f));
//	numLabel->runAction(CCFadeIn::create(0.5f));
	//正面卡牌开始翻转
	BaseSprite *openedCard = (BaseSprite*)cardItem->getChildByTag(Tag_CardOpened);
	openedCard->runAction(CCSequence::create(
		CCDelayTime::create(OPEN_CARD_TIME * .5),  
		CCShow::create(),  
		CCOrbitCamera::create(OPEN_CARD_TIME * .5, 1, 0, 270, 90, 0, 0), 
		CCCallFunc::create(this, callfunc_selector(VarietyStoreLayer::calback_refreshNeedDaiGouJuan)),
		NULL));
	//背面卡牌开始翻转
	BaseSprite *closedCard = (BaseSprite*)cardItem->getChildByTag(Tag_CardClosed);
	closedCard->runAction(CCSequence::create(
		CCOrbitCamera::create(OPEN_CARD_TIME * .5, 1, 0, 0, 90, 0, 0),  
		CCHide::create(),  
		CCDelayTime::create(OPEN_CARD_TIME * .5),  
		NULL));  

	//6个都抽完了
	{
		bool over = true;
		vector<ZahuopuItem> vec = ZahuopuHelper::getHelper()->getCurrentZaHuiList();
		for (int i=0; i!=vec.size(); ++i)
		{
			if (! vec.at(i).already_got)
			{
				over = false;
				break;
			}
		}
		if (over)
		{
			mBg->getChildByTag(Tag_Label_RefreshSilver)->runAction(CCFadeOut::create(0.15f));
			mBg->getChildByTag(Tag_Img_SilverIcon)->runAction(CCFadeOut::create(0.15f));
			//代购卷
			{
				CCLabelTTF* dgjLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_DaiGouJuan);
				if (dgjLabel) dgjLabel->setVisible(true);
				CCSprite *dgjImg = (CCSprite*)mBg->getChildByTag(Tag_Img_DaiGouJuan);
				if (dgjImg) dgjImg->setVisible(true);
				CCLabelTTF *dgjNum = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_DaiGouJuanNum);
				if (dgjNum)
				{
					char buf[32];
					sprintf(buf, " x%d", WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount());
					dgjNum->setString(buf);
					dgjNum->setVisible(true);
				}
			}

			CCMenuItemImage *item = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_GiveUp);
			item->setVisible(false);
			CCMenuItemImage *okItem = CCMenuItemImage::create(
				"shangpin_goumai_queren.png",
				"shangpin_goumai_queren_select.png",
				this, menu_selector(VarietyStoreLayer::menuItemClicked_ok));
			mMainMenu->addChild(okItem);
			okItem->setPosition(item->getPosition());
		}
	}
}

void VarietyStoreLayer::calback_refreshNeedDaiGouJuan()
{
	int dgjAll = WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount();
	int dgjNeed = ZahuopuHelper::getHelper()->getPickItemNeedDaiGouJuan();

	CCLabelTTF *direcLabel = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_Direction);

	CCSprite *dgjImg = (CCSprite*)mBg->getChildByTag(Tag_Img_Need_DaiGouJuan);
	if (dgjImg) dgjImg->setVisible(false);
	CCLabelTTF *dgjNumLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Need_DaiGouJuanNum);
	if (dgjNumLabel) dgjNumLabel->setVisible(false);
	CCLabelTTF *needGoldNumLabel = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_NeedGoldNum);
	if (needGoldNumLabel) needGoldNumLabel->setVisible(false);

	if (dgjAll > 0)
	{
		if (dgjImg == NULL)
		{
			dgjImg = CCSprite::create("daoju_daigouoquan.png");
			mBg->addChild(dgjImg, 1, Tag_Img_Need_DaiGouJuan);
			dgjImg->setScale(0.5f);
			dgjImg->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+dgjImg->getContentSize().width/2*dgjImg->getScaleX(),
				direcLabel->getPositionY()));
		}
		if (dgjNumLabel == NULL)
		{
			char buf[32];
			if (dgjNeed <= dgjAll) sprintf(buf, " x%d", dgjNeed);
			else sprintf(buf, " x%d", dgjAll);
			dgjNumLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
			mBg->addChild(dgjNumLabel, 1, Tag_Label_Need_DaiGouJuanNum);
			dgjNumLabel->setPosition(ccp(dgjImg->getPositionX()+dgjImg->getContentSize().width/2*dgjImg->getScaleX()+dgjNumLabel->getContentSize().width/2-16,
				dgjImg->getPositionY()));
		}

		dgjImg->setVisible(true);
		dgjNumLabel->setVisible(true);
		char buf[32];
		if (dgjNeed <= dgjAll) sprintf(buf, " x%d", dgjNeed);
		else sprintf(buf, " x%d", dgjAll);
		dgjNumLabel->setString(buf);
		dgjNumLabel->setPosition(ccp(dgjImg->getPositionX()+dgjImg->getContentSize().width/2*dgjImg->getScaleX()+dgjNumLabel->getContentSize().width/2-16,
			dgjImg->getPositionY()));

		needGoldNumLabel->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+110, direcLabel->getPositionY()));
	}
	//金币
	if (dgjNeed > dgjAll && needGoldNumLabel)
	{
		if (needGoldNumLabel) needGoldNumLabel->setVisible(true);
		else CCLog("Error: %s ---> 金币没显示！", __FUNCTION__);
		if (dgjAll > 0)
			needGoldNumLabel->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+126, direcLabel->getPositionY()));
		else
			needGoldNumLabel->setPosition(ccp(direcLabel->getPositionX()+direcLabel->getContentSize().width/2+40, direcLabel->getPositionY()));
	
		needGoldNumLabel->setString(CCString::createWithFormat("%u", CS::zahuopuGetOneItemNeedGold(
			WuPinManager::getManager()->getWuPin(ItemsType_DaiGouJuan)->getCount()))->getCString());
	}
}

void VarietyStoreLayer::menuItemClicked_ok(CCObject *pSender)
{
	VarietyStoreLayer *layer = VarietyStoreLayer::create();
	mFooterLayer->setContaintLayer(layer);
}

void VarietyStoreLayer::onEnter()
{
	BaseLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void VarietyStoreLayer::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	BaseLayer::onExit();
}

bool VarietyStoreLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if (! mCurStatus) // false  一个都没抽 的时候才能看材料说明
	{
		CCPoint pos = pTouch->getLocation();
		for (int i=0; i!=6; ++i)
		{
			//cardItem->setPosition(ccp(160 + i%3*160, 761-210-i/3*235));
			//139  205
			CCRect rect;
			CCPoint leftdown = ccp(160 + i%3*160 - 139/2, 761-210-i/3*235 - 205/2);
			leftdown = mCardMenu->convertToWorldSpace(leftdown);
			rect.setRect(leftdown.x, leftdown.y, 139, 205);
			if (rect.containsPoint(pos))
				return true;
		}
	}

	return false;
}

void VarietyStoreLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if (! mCurStatus) // false  一个都没抽 的时候才能看材料说明
	{
		CCPoint pos = pTouch->getLocation();
		//屏蔽材料的详细信息
// 		vector<ZahuopuItem> items = ZahuopuHelper::getHelper()->getCurrentZaHuiList();
// 		for (int i=0; i<6 && i<items.size(); ++i)
// 		{
// 			//cardItem->setPosition(ccp(160 + i%3*160, 761-210-i/3*235));
// 			//139  205
// 			CCRect rect;
// 			CCPoint leftdown = ccp(160 + i%3*160 - 139/2, 761-210-i/3*235 - 205/2);
// 			leftdown = mCardMenu->convertToWorldSpace(leftdown);
// 			rect.setRect(leftdown.x, leftdown.y, 139, 205);
// 			if (rect.containsPoint(pos))
// 			{
// 				//弹框背景
// 				BaseSprite* dialog = BaseSprite::create("gongjiang_duanzao_cailiaobuzu_tanchukuang.png");
// 				dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 + 50));
// 				//提示
// 				{
// 					BaseSprite *cailiaoBg = BaseSprite::create("daoju_kuang_bg.png");
// 					dialog->addChild(cailiaoBg);
// 					cailiaoBg->setPosition(ccp(cailiaoBg->getContentSize().width/2 + 20, dialog->getContentSize().height/2));
// 					BaseSprite *cailiaoImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(items.at(i).item_type));
// 					cailiaoBg->addChild(cailiaoImg);
// 					cailiaoImg->setPosition(ccp(cailiaoBg->getContentSize().width/2, cailiaoBg->getContentSize().height/2));
// 
// 					CCLabelTTF* lable = CCLabelTTF::create(CS::getItemShuxing(items.at(i).item_type).c_str()
// 						, fontStr_kaiti,20,CCSize(240, 0), kCCTextAlignmentLeft);
// 					dialog->addChild(lable);
// 					lable->setAnchorPoint(ccp(0,0.5));
// 					lable->setPosition(ccp(cailiaoBg->getPositionX()+cailiaoBg->getContentSize().width/2+10, dialog->getContentSize().height/2));
// 				}
// 				//按键
// 				CCMenu* menu = CCMenu::create();
// 				{
// 					CCMenuItemImage* item = CCMenuItemImage::create(
// 						"gongjiang_duanzao_cailiaobuzu_tanchukuang.png",
// 						"gongjiang_duanzao_cailiaobuzu_tanchukuang.png",
// 						this, menu_selector(VarietyStoreLayer::menuItemClicked_CaiLiaoShuoMingEnd));
// 					menu->addChild(item);
// 					item->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
// 					item->setOpacity(0);
// 					item->setScaleX(mWinSize.width / dialog->getContentSize().width);
// 					item->setScaleY(mWinSize.height / dialog->getContentSize().height);
// 				}
// 				//弹框
// 				{
// 					LFAlert* lfAlert = new LFAlert();
// 					lfAlert->setBaseContainer(dialog,menu);
// 					CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128,2000);
// 					lfAlert->release();
// 				}
// 			}
// 		}
 	}
}
void VarietyStoreLayer::menuItemClicked_CaiLiaoShuoMingEnd(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(2000);
}
