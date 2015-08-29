#include "MainLayerHerosBaseBody.h"
#include "LFUtile.h"
#include "BaseActorPropertyParser.h"
#include "font.h"
#include "ZhuangBeiMianBanBase.h"
#include <algorithm>
#include "MyselfManager.h"
#include "MessageDisplay.h"
#include "CS_Common.h"
#include "GameDirector.h"
#include "TaoZhuangInfoUI.h"
#include "AddFriendUI.h"
#include "BaseSprite.h"
#include "AddFriendUI.h"

#define TAG_Menu    1121
#define TAG_Unlock  1122
#define Tag_Label_HeroDirection 10


#define TAG_BaseBody_TABLE_ITEM         11112
#define TAG_BaseBody_TABLE_HERO_IMG   11113
#define TAG_BaseBody_LABLE_NAME			11114
#define TAG_BaseBody_LABLE_LEVEL			11115	
#define TAG_BaseBody_LABLE_XUEZHI			11116	// 
#define TAG_BaseBody_LABLE_GONGJI			11117	// 
#define TAG_BaseBody_LABLE_HUJIA			11118	// 
#define TAG_BaseBody_LABLE_MOKANG			11119	// 
#define TAG_BaseBody_MENU					11120

enum 
{
	Tag_Window_JieSuo = 666,
};


// 06-15 11:43:50.179: D/cocos2d-x debug info(1716): Get data from file(tabulation_armor_helmet_1.png) failed!
MainLayerHerosBaseBody::MainLayerHerosBaseBody(int flag):mFlag(flag)
{
	mTableView = CCTableView::create(this, CCSizeMake(610,671));
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setPosition(ccp(15,100));
	mTableView->setDelegate(this);
	addChild(mTableView);
}


MainLayerHerosBaseBody::~MainLayerHerosBaseBody(void)
{
}


void MainLayerHerosBaseBody::scrollViewDidScroll(CCScrollView* view) 
{
}
void MainLayerHerosBaseBody::scrollViewDidZoom(CCScrollView* view) 
{
}

void MainLayerHerosBaseBody::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
// 	int idx = cell->getIdx();
// 	int heroId = mHerosVec[idx]->getId();
// 
// 
// 	ZhuangBeiMianBanBody* mianBan = new ZhuangBeiMianBanBody(heroId);
// 	mFooterForPop->popUpContaintLayer(mianBan);
// 	mianBan->release();

}
void MainLayerHerosBaseBody::tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell)
{
	//CCLog("tableCellWillRecycle");
}
void MainLayerHerosBaseBody::tableCellHighlight(CCTableView* table, CCTableViewCell* cell)
{
}

void MainLayerHerosBaseBody::tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell)
{
}

CCSize MainLayerHerosBaseBody::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSizeMake(610, 136);
}


void MainLayerHerosBaseBody::disableAllTouchBegin()
{
	mTableView->setTouchEnabled(false);
	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
			if (bg)
			{
				CCMenu* menu = (CCMenu*)bg->getChildByTag(TAG_BaseBody_TABLE_HERO_IMG);
				if (menu)
				{
					menu->setEnabled(false);
				}
				menu = (CCMenu*)bg->getChildByTag(TAG_Menu);
				if(menu)
				{
					menu->setEnabled(false);
				}
			}
		}
	}

}
void MainLayerHerosBaseBody::disableAllTouchEnd()
{
	mTableView->setTouchEnabled(true);

	int cellNum = numberOfCellsInTableView(mTableView);
	for (int i=0;i<cellNum;i++)
	{
		CCTableViewCell* cell = mTableView->cellAtIndex(i);
		if (cell)
		{
			CCNode* bg = cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
			if (bg)
			{
				CCMenu* menu = (CCMenu*)bg->getChildByTag(TAG_BaseBody_TABLE_HERO_IMG);
				if (menu)
				{
					menu->setEnabled(true);
				}
				menu = (CCMenu*)bg->getChildByTag(TAG_Menu);
				if(menu)
				{
					menu->setEnabled(true);
				}
			}
		}
	}
}

CCTableViewCell* MainLayerHerosBaseBody::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCString *string = CCString::createWithFormat("%d", idx);
	CCTableViewCell *cell = table->dequeueCell();
	if (cell == NULL) 
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}
	else 
	{
		cell->removeAllChildren();
	}

	cell->addChild(getItemContentByIndex(idx),0,TAG_BaseBody_TABLE_ITEM);
		//CCLog("CCTableViewCell one");
// 	}
// 	else
// 	{
// 		//CCLog("refreshItem one");
// 		CCNode* temp = cell->getChildByTag(TAG_BaseBody_TABLE_ITEM);
// 		refreshItem(temp,idx);
// 	}

	return cell;
}

BaseSprite* MainLayerHerosBaseBody::getItemContentByIndex(unsigned int index)
{

	BaseSprite* bg = BaseSprite::create("duiwu_yingxiong_bg.png");
	bg->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));

	//图片
	{
		ImageData data = ImageManager::getManager()->getImageData_ProfessionHead((Profession)mHerosVec[index]->getActorType());

		CCLOG("----------------->%s",data.getCurrentImageFile().c_str());
		BaseSprite *sprite = BaseSprite::create( data.getCurrentImageFile());
		BaseSprite *sprite2 = BaseSprite::create( data.getCurrentImageFile());
		BaseSprite *sprite3 = BaseSprite::create( data.getCurrentImageFile());
		CCMenuItemSprite* heroItem = CCMenuItemSprite::create(sprite, sprite2, sprite3, this, menu_selector(MainLayerHerosBaseBody::menuItemClicked_heroImg));
		CCMenu* menu = CCMenu::create(heroItem, NULL);
		bg->addChild(menu, 0, TAG_BaseBody_TABLE_HERO_IMG);
		heroItem->setPosition(ccp(74, bg->getContentSize().height/2));
		menu->setPosition(CCPointZero);

		BaseSprite *biankuang = BaseSprite::create("renxing_renwu_touxiang_select.png");
		bg->addChild(biankuang, 1);
		biankuang->setPosition(heroItem->getPosition());
		BaseSprite *item = NULL;
		Profession iType = (Profession)mHerosVec[index]->getActorType();
		if (iType == Profession_Yemanren)
		{
			item = BaseSprite::create("touxiang_jiaobiao_3.png");
		}
		else if (iType == Profession_Chike)
		{
			item = BaseSprite::create("touxiang_jiaobiao_2.png");
		}
		else if (iType == Profession_Fashi)
		{
			item = BaseSprite::create("touxiang_jiaobiao_1.png");
		}
		else if (iType == Profession_Qishi)
		{
			item = BaseSprite::create("touxiang_jiaobiao_3.png");
		}
		else if (iType == Profession_ShenJianShou)
		{
			item = BaseSprite::create("touxiang_jiaobiao_4.png");
		}
		else if (iType == Profession_MiShu)
		{
			item = BaseSprite::create("touxiang_jiaobiao_1.png");
		}
		else if (iType == Profession_WuSeng)
		{
			item = BaseSprite::create("touxiang_jiaobiao_5.png");
		}
		else if (iType == Profession_YouXia)
		{
			item = BaseSprite::create("touxiang_jiaobiao_2.png");
		}
		else if (iType == Profession_LieShou)
		{
			item = BaseSprite::create("touxiang_jiaobiao_4.png");
		}
		else if (iType == Profession_WuShi)
		{
			item = BaseSprite::create("touxiang_jiaobiao_6.png");
		}

		if (item)
		{
			item->setAnchorPoint(ccp(1, 0));
			biankuang->addChild(item);
			item->setPosition(ccp(biankuang->getContentSize().width, 0));
		}		
	}

	//name
	HeroFixedPro info = BaseActorPropertyParser::getParser()->getExtraProerty((Profession)mHerosVec[index]->getActorType());
	LF::lable(bg, info.nameInChinese,ccp(140, 106),22,fontStr_kaiti)->setTag(TAG_BaseBody_LABLE_NAME);

	// 英雄介绍
	{
		Profession profId = mHerosVec[index]->getActorType();
		CCLabelTTF *directionLabel = CCLabelTTF::create(getHeroDirectionByProfessionId(profId).c_str(), fontStr_kaiti, 20, CCSize(380, 0), kCCTextAlignmentLeft);
		bg->addChild(directionLabel, 1, Tag_Label_HeroDirection);
		directionLabel->setColor(ccc3(165, 125, 76));
		directionLabel->setAnchorPoint(ccp(0, 1));
		directionLabel->setPosition(ccp(140, 94));
	}

	if (mFlag == 1)
	{
		CCMenuItemImage* chose = CCMenuItemImage::create(
			"duiwu_yingxiong_weixuanzhong.png"
			, "duiwu_yingxiong_weixuanzhong.png"
			, this, menu_selector(MainLayerHerosBaseBody::menuItemClicked_choseHero));
		chose->setPosition(558, 83);
		CCMenu* menu = CCMenu::create(chose, NULL);
		menu->setPosition(CCPointZero);
		bg->addChild(menu, 0, TAG_Menu);

		CCSprite* unlock = CCSprite::create("duiwu_yingxiong_unlock.png", CCRect(0, 0, 610, 129));
		bg->addChild(unlock, 0, TAG_Unlock);
		unlock->setOpacity(200);
		unlock->setPosition(bg->getPosition());

		int state = mChoseHeroState.at(index);
		if(state>=1 && state<=4) //已选中的
		{
			chose->initWithNormalImage(
				"duiwu_yingxiong_xuanzhong.png"
				, "duiwu_yingxiong_xuanzhong.png"
				, "duiwu_yingxiong_xuanzhong.png"
				, this, menu_selector(MainLayerHerosBaseBody::menuItemClicked_choseHero));
			unlock->setVisible(false);
		}
		else if(state == 0 && mChoseNum<3) //未选中 可选的
		{
			chose->initWithNormalImage(
				"duiwu_yingxiong_daixuan.png"
				, "duiwu_yingxiong_daixuan.png"
				, "duiwu_yingxiong_daixuan.png"
				, this, menu_selector(MainLayerHerosBaseBody::menuItemClicked_choseHero));
			unlock->setVisible(false);
		}

		else						//选满了、不能选的
		{
			bg->initWithFile("duiwu_yingxiong_bg_suoding.png");
		}

	}
	else if (mFlag == 2)
	{
		if (mHerosVec.at(index)->canUnlock())
		{
			//可解锁
			CCMenuItemImage* kjsItem = CCMenuItemImage::create(
				"duiwu_yingxiong_jiesuo.png"
				, "duiwu_yingxiong_jiesuo.png"
				, this, menu_selector(MainLayerHerosBaseBody::menuItemClicked_jiesuoHero));
			kjsItem->setPosition(558, 83);
			kjsItem->setTag(index);
			CCMenu* menu = CCMenu::create(kjsItem, NULL);
			menu->setPosition(CCPointZero);
			bg->addChild(menu, 0, TAG_Menu);
		}
		else //还不能解锁
		{
			//不能解锁
			CCMenuItemImage* kjsItem = CCMenuItemImage::create(
				"duiwu_yingxiong_suoding.png"
				, "duiwu_yingxiong_suoding.png"
				, this, menu_selector(MainLayerHerosBaseBody::menuItemClicked_jiesuoHero));
			kjsItem->setPosition(558, 83);
			kjsItem->setTag(index);
			CCMenu* menu = CCMenu::create(kjsItem, NULL);
			menu->setPosition(CCPointZero);
			bg->addChild(menu, 0, TAG_Menu);

			CCSprite* unlock = CCSprite::create("duiwu_yingxiong_unlock.png", CCRect(0, 0, 610, 129));
			bg->addChild(unlock, 0, TAG_Unlock);
			unlock->setOpacity(200);
			unlock->setPosition(bg->getPosition());
		}

	}
	

	return bg;
}
/*
void MainLayerHerosBaseBody::refreshItem(CCNode* item,int index)
{
	//img
	{
		ImageData data = ImageManager::getManager()->getImageData_ProfessionHead((Profession)mHerosVec[index]->getActorType());
		CCMenu* menu = (CCMenu*)item->getChildByTag(TAG_BaseBody_TABLE_HERO_IMG);
		CCMenuItemSprite* menuItem = (CCMenuItemSprite*)menu->getChildren()->objectAtIndex(0);
		BaseSprite *sprite = BaseSprite::create(data.getCurrentImageFile());
		BaseSprite *sprite2 = BaseSprite::create(data.getCurrentImageFile());
		BaseSprite *sprite3 = BaseSprite::create(data.getCurrentImageFile());
		menuItem->initWithNormalSprite(sprite, sprite2, sprite3, this, menu_selector(MainLayerHerosBaseBody::heroImgClicked));
		menuItem->setPosition(ccp(74, menu->getParent()->getContentSize().height/2));
	}

	//name
	{
		HeroFixedPro info = BaseActorPropertyParser::getParser()->getExtraProerty((Profession)mHerosVec[index]->getActorType());
		CCLabelTTF* lable = (CCLabelTTF*)item->getChildByTag(TAG_BaseBody_LABLE_NAME);
		lable->setString(info.nameInChinese.c_str());
	}

	//hero direction
	{
		CCLabelTTF* lable = (CCLabelTTF*)item->getChildByTag(Tag_Label_HeroDirection);
		lable->setString(getHeroDirectionByProfessionId(mHerosVec[index]->getActorType()).c_str());
	}

	{
		CCMenuItemImage* chose = (CCMenuItemImage*)item->getChildByTag(TAG_Menu)->getChildren()->objectAtIndex(0);
		 chose->initWithNormalImage(
			"duiwu_yingxiong_suoding.png"
			, "duiwu_yingxiong_suoding.png"
			, "duiwu_yingxiong_suoding.png"
			, this, menu_selector(MainLayerHerosBaseBody::choseHero));
		chose->setPosition(558, 83);

		CCSprite* unlock = (CCSprite*)item->getChildByTag(TAG_Unlock);
		unlock->setPosition(item->getPosition());
		unlock->setVisible(true);

		int state = mChoseHeroState.at(index);
		if(state>=1 && state<=4)
		{
			chose->initWithNormalImage("duiwu_yingxiong_xuanzhong.png"
				, "duiwu_yingxiong_xuanzhong.png", "duiwu_yingxiong_xuanzhong.png"
				, this, menu_selector(MainLayerHerosBaseBody::choseHero));
			unlock->setVisible(false);


			((BaseSprite*)item)->initWithFile("duiwu_yingxiong_bg_suoding.png");
		}
		else if(state == 0 && mChoseNum<3)
		{
			chose->initWithNormalImage("duiwu_yingxiong_daixuan.png"
				, "duiwu_yingxiong_daixuan.png", "duiwu_yingxiong_daixuan.png"
				, this, menu_selector(MainLayerHerosBaseBody::choseHero));
			unlock->setVisible(false);


			((BaseSprite*)item)->initWithFile("duiwu_yingxiong_bg.png");
		}

		else
		{
			((BaseSprite*)item)->initWithFile("duiwu_yingxiong_bg.png");
		}
	}
}*/

unsigned int MainLayerHerosBaseBody::numberOfCellsInTableView(CCTableView *table)
{
	// 第一步
	return mHerosVec.size();
}

void MainLayerHerosBaseBody::sortTableList()
{
	for(int i=0; i!=mHerosVec.size(); ++i)
	{
		for(int j=i+1; j!=mHerosVec.size(); ++j)
		{
			int iLev = mHerosVec[i]->getCurrentProperty().level;
			int jLev = mHerosVec[j]->getCurrentProperty().level;
			if(iLev < jLev)
				swap(mHerosVec[i], mHerosVec[j]);
		}
	}

	reverse(mHerosVec.begin(), mHerosVec.end());
}

void MainLayerHerosBaseBody::menuItemClicked_choseHero(CCObject* pSender)
{
	BaseSprite* bg = (BaseSprite*)((CCMenuItemImage*)pSender)->getParent()->getParent();
	CCTableViewCell* cell = (CCTableViewCell*)bg->getParent();
	int idx = cell->getIdx();

	if(mChoseHeroState[idx]>=1 && mChoseHeroState[idx]<=4)
	{
		mChoseHeroState[idx] = 0;
		--mChoseNum;

//		mTableView->reloadData();
		for(int i=0; i!=mHerosVec.size(); ++i)
			mTableView->updateCellAtIndex(i);
	}
	else if(mChoseHeroState[idx] == 0 && mChoseNum<3)
	{
		int temp[4] = {false};
		int chessboard;
		for(int i=0; i!=mChoseHeroState.size(); ++i)
			if(mChoseHeroState[i]>=1 && mChoseHeroState[i]<=4)
				temp[mChoseHeroState[i]-1] = true;
		for(int i=0; i!=4; ++i)
			if(! temp[i])		
			{
				if(MyselfManager::getManager()->getMyZhuJueData()->getChessBoardPos() == i+1)
					continue;
				chessboard = i+1;
				break;
			}
			
		mChoseHeroState[idx] = chessboard;
		++mChoseNum;

//		mTableView->reloadData();

		for(int i=0; i!=mHerosVec.size(); ++i)
			mTableView->updateCellAtIndex(i);
	}
	
}

void MainLayerHerosBaseBody::menuItemClicked_heroImg(CCObject* pSender)
{
	CCTableViewCell *cell = (CCTableViewCell*)((CCMenuItemSprite*)pSender)->getParent()->getParent()->getParent();
	int idx = cell->getIdx();
	AddFriendUI *layer = AddFriendUI::create();
	layer->ignoreAnchorPointForPosition(false);
	layer->setAnchorPoint(CCPointZero);
	layer->setPosition(CCPointZero);
	layer->setProfessionId(mHerosVec[idx]->getActorType());
	layer->setMenuShow(true,false);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);
	

	
}

string MainLayerHerosBaseBody::getHeroDirectionByProfessionId(Profession profId)
{
	string direction;
	switch (profId)
	{
	case Profession_Qishi:
		direction = LFStrings::getValue("renwuJieShao_QiShi");
		break;
	case Profession_ShenJianShou:
		direction = LFStrings::getValue("renwuJieShao_ShenJianShou");
		break;
	case Profession_MiShu:
		direction = LFStrings::getValue("renwuJieShao_MiShuShi");
		break;
	case Profession_WuSeng:
		direction = LFStrings::getValue("renwuJieShao_WuSeng");
		break;
	case Profession_YouXia:
		direction = LFStrings::getValue("renwuJieShao_YouXia");
		break;
	case Profession_LieShou:
		direction = LFStrings::getValue("renwuJieShao_LieShou");
		break;
	case Profession_WuShi:
		direction = LFStrings::getValue("renwuJieShao_WuShi");
		break;
	default:
		break;
	}

	return direction;
}

void MainLayerHerosBaseBody::menuItemClicked_jiesuoHero(CCObject *pSender)
{
	int idx = ((CCMenuItemImage*)pSender)->getTag();

	char buf[128];

	if (mHerosVec.at(idx)->canUnlock())
	{
		mJieSuo_HeroId = mHerosVec.at(idx)->getId();
		//弹框背景
		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
		dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
		dialog->setTag(Tag_Window_JieSuo);
		//label
		CCString temp;
		//解锁此助手需要花费\n%d荣誉值\n（当前荣誉：%d）
		temp.initWithFormat(LFStrings::getValue("JieSuoZhuShouXuYao_RongYuZhi").c_str(), 
			CS::AssistantUnLockNeedHonor(MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All, LF_Locked).size()),
			MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->jifen);
		CCLabelTTF *label = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 30, CCSize(400, 0), kCCTextAlignmentCenter);
		dialog->addChild(label);
		label->setAnchorPoint(ccp(0.5, 1));
		label->setPosition(ccp(dialog->getContentSize().width/2, 278));

		//按键
		CCMenu* menu;
		{
			CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
				"shangpin_goumai_queren.png",
				"shangpin_goumai_queren_select.png",
				this,SEL_MenuHandler(&MainLayerHerosBaseBody::menuItemClicked_LockHero_OkCancel));


			CCMenuItemImage* menuItemCacel = CCMenuItemImage::create(
				"shangpin_goumai_quxiao.png",
				"shangpin_goumai_quxiao_select.png",
				this,SEL_MenuHandler(&MainLayerHerosBaseBody::menuItemClicked_LockHero_OkCancel));

			menu = CCMenu::create(menuItemMakeSure, menuItemCacel, NULL);
			menu->setPosition(CCPointZero);

			menuItemMakeSure->setTag(10);
			menuItemCacel->setTag(20);
			menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/3*2,70));
			menuItemCacel->setPosition(ccp(dialog->getContentSize().width/3, 70));
		}

		//弹框
		{
			LFAlert* lfAlert = new LFAlert();
			lfAlert->setTag(Tag_Window_JieSuo);
			lfAlert->setBaseContainer(dialog,menu);
			CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert, 128, Tag_Window_JieSuo);
			lfAlert->release();
		}
	}
	else
	{
		sprintf(buf, LFStrings::getValue("XuYaoZhuJueDengJi_jiesuo").c_str(), 15);
		MessageDisplay *layer = MessageDisplay::create(buf);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
}

void MainLayerHerosBaseBody::menuItemClicked_LockHero_OkCancel(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_JieSuo);

	if (tag == 10) //makesure
	{
		unsigned int needNum = CS::AssistantUnLockNeedHonor(MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All, LF_Locked).size());
		unsigned int allNum  = MyselfManager::getManager()->getEdiableArenaTeamBasicInfo()->jifen;
		if (allNum < needNum)
		{
			MessageDisplay *layer = MessageDisplay::create(LFStrings::getValue("rongyuzhiBuZu_HuoDefangshi"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		else
		{
			addObserver(callfuncO_selector(MainLayerHerosBaseBody::rsp_JieSuoYingXiong), MSG_unlockAssistantRsp);
			mCmdHlp->cmdUnlockAssistant(mJieSuo_HeroId);
			return;
		}
	}
// 	else if (tag == 20)//cacle
// 	{
// 	}
	
}

void MainLayerHerosBaseBody::rsp_JieSuoYingXiong(CCObject *msg)
{
//子类实现
	removeObserver(MSG_unlockAssistantRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_UnlockAssistantRsp* data = (SPCmd_UnlockAssistantRsp*) ((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	
	MessageDisplay *layer = MessageDisplay::create(LFStrings::getValue("jieSuoChengGong"));
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
}





