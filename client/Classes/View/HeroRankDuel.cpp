
#include "HeroRankDuel.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "flat.h"
#include "LFStrings.h"
#include "font.h"
#include "FightDirector.h"
#include "ClientFightLayer.h"
#include "BaseActorPropertyParser.h"
#include "ZhuangBeiMianBanOther.h"
#include "FX_CommonFunc.h"
#include "MyselfManager.h"
#include "CS_Common.h"
#include "ImageManager.h"
#include "ServerTime.h"
#include "DianFengDuiJueManager.h"
#include "FXToast.h"
#include "MapsManager.h"

using namespace FX_CommonFunc;

#define Label_Color_DanHuang ccc3(195, 145, 96)
#define Label_Size_Min 20
#define Label_Size_Max 24

enum 
{
	Tag_MainMenuItem_Rule = 1,
	Tag_MainMenuItem_MyGuess,
	Tag_MainMenuItem_Exit,

	Tag_Img_Top1,  //正中间

	Tag_GuessMenuItem_ = 11,	   // 11 - 17  mGuess_PlayBack
	Tag_PlayBackMenuItem_ = 31,    // 31 - 38  mGuess_PlayBack

	Tag_Label_Title = 50,
	Tag_Label_Time  ,
	Tag_Label_GuessHeroName,
	Tag_Label_GuessMoneyNum,


	Tag_Board_ = 100,
	Tag_Board_Menu,
	Tag_BoardItem_Close,
};


HeroRankDuel* HeroRankDuel::create()
{
	HeroRankDuel *layer = new HeroRankDuel();
	if (layer)
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

HeroRankDuel::~HeroRankDuel()
{

}

HeroRankDuel::HeroRankDuel()
{
	initLayer();
}

void HeroRankDuel::initLayer()
{
	mBg = BaseSprite::create("dianfengduijue.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));
	BaseSprite *img = BaseSprite::create("dfdj_shengli_bg.png");
	mBg->addChild(img, 2, Tag_Img_Top1);
	img->setPosition(ccp(320, 860-420));
	//
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 10);
	mMainMenu->setPosition(ccp(0, 0));
	{
		CCMenuItemImage *ruleItem = CCMenuItemImage::create(
			"dfdj_anniu_guize.png",
			"dfdj_anniu_guize_select.png",
			this, menu_selector(HeroRankDuel::menuItemClicked_MainMenu));
		mMainMenu->addChild(ruleItem, 0, Tag_MainMenuItem_Rule);
		ruleItem->setPosition(ccp(200, 860-777));
		CCMenuItemImage *myGuessItem = CCMenuItemImage::create(
			"dfdj_anniu_wodejingcai.png",
			"dfdj_anniu_wodejingcai_select.png",
			this, menu_selector(HeroRankDuel::menuItemClicked_MainMenu));
		mMainMenu->addChild(myGuessItem, 0, Tag_MainMenuItem_MyGuess);
		myGuessItem->setPosition(ccp(320, 860-777));
		CCMenuItemImage *exitItem = CCMenuItemImage::create(
			"dfdj_anniu_tuichu.png",
			"dfdj_anniu_tuichu_select.png",
			this, menu_selector(HeroRankDuel::menuItemClicked_MainMenu));
		mMainMenu->addChild(exitItem, 0, Tag_MainMenuItem_Exit);
		exitItem->setPosition(ccp(440, 860-777));
	}
	//
	mHeroImgMenu = CCMenu::create();
	mBg->addChild(mHeroImgMenu, 10);
	mHeroImgMenu->setPosition(ccp(0,0));
	// 
	mGuessMenu = CCMenu::create();
	mBg->addChild(mGuessMenu, 10);
	mGuessMenu->setPosition(ccp(0,0));
	//
	mPlayBackMenu = CCMenu::create();
	mBg->addChild(mPlayBackMenu, 10);
	mPlayBackMenu->setPosition(ccp(0,0));


	if (DianFengDuiJueManager::getManager()->getCurrentRound() != DianFengDuiJueManager::Round_Not_inited)
		scheduleOnce(schedule_selector(HeroRankDuel::getHeroRankDuleState), 0);
}

void HeroRankDuel::getHeroRankDuleState(float dt)
{
	DianFengDuiJueManager::Round curState = DianFengDuiJueManager::getManager()->getCurrentRound();

	CCString temp;
	string str;
	//title
	if (curState == DianFengDuiJueManager::Round_8_4) str = getStrByInt(8) + LFStrings::getValue("qiang").c_str() + LFStrings::getValue("ZhengDuo");
	else if (curState == DianFengDuiJueManager::Round_4_2) str = getStrByInt(8) + LFStrings::getValue("qiang").c_str() + LFStrings::getValue("ZhengDuo");
	else if (curState == DianFengDuiJueManager::Round_2_1) str = LFStrings::getValue("WangZhe") + LFStrings::getValue("ZhengDuo");
	else str = "";
	CCLabelTTF *tilte = CCLabelTTF::create(str.c_str(), fontStr_kaiti, Label_Size_Max);
	tilte->setPosition(ccp(320, 860-164));
	tilte->setColor(Label_Color_DanHuang);
	mBg->addChild(tilte, 1, Tag_Label_Title);
	//time
	unsigned int time_ = DianFengDuiJueManager::getManager()->getOpenTime() - ServerTime::getTime();
	if (DianFengDuiJueManager::getManager()->getOpenTime() < ServerTime::getTime())
	{
		temp.initWithFormat("%s", LFStrings::getValue("JingCaiZhong").c_str());
		mbOpen = true;
	}
	else 
	{
		temp.initWithFormat("%s:%02d:%02d:%02d", LFStrings::getValue("daojishi").c_str(),
			time_/60/60, time_/60%60, time_%60);
		mbOpen = false;
	}
	CCLabelTTF *titleTime = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	titleTime->setPosition(ccp(320, 860-195));
	mBg->addChild(titleTime, 1, Tag_Label_Time);
	schedule(schedule_selector(HeroRankDuel::updateBeginTime), 1.0f);
	//guess hero name
	Guess myGuess;
	if (DianFengDuiJueManager::getManager()->getGuess(myGuess,curState))
	{
		if (myGuess.i_guess_winner_is == 0) str = myGuess.hero_a.name;
		else str = myGuess.hero_b.name;
		temp.initWithFormat("%s%s:%s", LFStrings::getValue("JingCai").c_str(), LFStrings::getValue("YingXiong").c_str(), str.c_str());
		char buf[32];
		sprintf (buf, "%u", myGuess.my_wager);
		str = buf;
		mGuessMenu->setVisible(false);
	}
	else 
	{
		temp.initWithFormat("%s%s:", LFStrings::getValue("JingCai").c_str(), LFStrings::getValue("YingXiong").c_str());
		str.clear(); //竞猜金额
	}
	CCLabelTTF *guessName = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	mBg->addChild(guessName, 1, Tag_Label_GuessHeroName);
	guessName->setPosition(ccp(320, 860-658));
	//guess hero money
	temp.initWithFormat("%s%s:%s", LFStrings::getValue("JingCai").c_str(), LFStrings::getValue("JinE").c_str(), str.c_str());
	CCLabelTTF *guessMoney = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	mBg->addChild(guessMoney, 1, Tag_Label_GuessMoneyNum);
	guessMoney->setPosition(ccp(320, 860-687));


	//top 8
	{
		pair<HeroInfo, HeroInfo> pair0 = DianFengDuiJueManager::getManager()->getOneGroup(0, DianFengDuiJueManager::Round_8_4);
		mTop8HeroInfoVec.push_back(pair0.first);
		mTop8HeroInfoVec.push_back(pair0.second);
		pair<HeroInfo, HeroInfo> pair1 = DianFengDuiJueManager::getManager()->getOneGroup(1, DianFengDuiJueManager::Round_8_4);
		mTop8HeroInfoVec.push_back(pair1.first);
		mTop8HeroInfoVec.push_back(pair1.second);
		pair<HeroInfo, HeroInfo> pair2 = DianFengDuiJueManager::getManager()->getOneGroup(2, DianFengDuiJueManager::Round_8_4);
		mTop8HeroInfoVec.push_back(pair2.first);
		mTop8HeroInfoVec.push_back(pair2.second);
		pair<HeroInfo, HeroInfo> pair3 = DianFengDuiJueManager::getManager()->getOneGroup(3, DianFengDuiJueManager::Round_8_4);
		mTop8HeroInfoVec.push_back(pair3.first);
		mTop8HeroInfoVec.push_back(pair3.second);
	}
	for (int i=0; i!=8; ++i)
	{
		HeroInfo oneHeroInfo = mTop8HeroInfoVec.at(i);

		str = ImageManager::getManager()->getImageData_ProfessionHead((Profession)oneHeroInfo.profession_id).getCurrentImageFile();
		CCMenuItemImage *item = CCMenuItemImage::create(
			str.c_str(), str.c_str(), this, menu_selector(HeroRankDuel::menuItemClicked_HeroImg));
		mHeroImgMenu->addChild(item, 0);

		//先左列  后右列
		float x = 69 + i / 4 * (567-69);
		float y = (860-214) - i % 4 * 148;
		if (i == 2 || i == 3 || i == 6 || i == 7) y += 24;
		item->setPosition(ccp(x, y));
		item->setUserData((void *)oneHeroInfo.hero_id);
		item->setScale(0.7f);

		BaseSprite *kuang = BaseSprite::create("renxing_renwu_touxiang_select.png");
		item->addChild(kuang);
		kuang->setPosition(ccp(item->getContentSize().width/2, item->getContentSize().height/2));
		ccColor3B nameColor = ccWHITE;
		//当8-4打完了，且i 英雄输了   //1.ko字样 + 2.灰色名字 + 3.回放按钮
		if (curState > DianFengDuiJueManager::Round_8_4 
			&& ! DianFengDuiJueManager::getManager()->is_hero_win_in_round(oneHeroInfo, DianFengDuiJueManager::Round_8_4))
		{     //输了
			nameColor = fonColor_CiYao;
			BaseSprite *koImg = BaseSprite::create("dfdj_ko.png");
			kuang->addChild(koImg);
			koImg->setPosition(ccp(kuang->getContentSize().width/2, kuang->getContentSize().height/2));
			koImg->setScale(1 / item->getScale());
			//
			CCMenuItemImage *playBackItem = CCMenuItemImage::create(
				"dfdj_anniu_huifang.png",
				"dfdj_anniu_huifang_select.png",
				this, menu_selector(HeroRankDuel::menuItemClicked_PlayBack));
			mPlayBackMenu->addChild(playBackItem, 0, Tag_PlayBackMenuItem_ + i);
			playBackItem->setUserData((void*)DianFengDuiJueManager::Round_8_4);
			playBackItem->setPosition(getPlayBackOrGuessItemPositionByIdx(DianFengDuiJueManager::Round_8_4, i));
		}
		else if (curState > DianFengDuiJueManager::Round_8_4)//赢了    4.加绿线  //5. 嵌套 判断下一场4-2
		{
			//绿线
			BaseSprite *top8WinImg = getLuXianByIdx(DianFengDuiJueManager::Round_8_4, i); //已设置位置信息
			mBg->addChild(top8WinImg);
			//嵌套 4-2
			//当4-2打完了，且i 英雄输了   //1.ko字样 + 2.灰色名字 + 3.回放按钮
			if (curState > DianFengDuiJueManager::Round_4_2 
				&& ! DianFengDuiJueManager::getManager()->is_hero_win_in_round(oneHeroInfo, DianFengDuiJueManager::Round_4_2))
			{     //输了
				nameColor = fonColor_CiYao;
				BaseSprite *koImg = BaseSprite::create("dfdj_ko.png");
				kuang->addChild(koImg);
				koImg->setPosition(ccp(kuang->getContentSize().width/2, kuang->getContentSize().height/2));
				koImg->setScale(1 / item->getScale());
				//
				CCMenuItemImage *playBackItem = CCMenuItemImage::create(
					"dfdj_anniu_huifang.png",
					"dfdj_anniu_huifang_select.png",
					this, menu_selector(HeroRankDuel::menuItemClicked_PlayBack));
				mPlayBackMenu->addChild(playBackItem, 0, Tag_PlayBackMenuItem_ + i);
				playBackItem->setUserData((void*)DianFengDuiJueManager::Round_4_2);
				playBackItem->setPosition(getPlayBackOrGuessItemPositionByIdx(DianFengDuiJueManager::Round_4_2, i));
			}
			else if (curState > DianFengDuiJueManager::Round_4_2) //赢了    4.加绿线  //5. 嵌套 判断下一场 2-1
			{
				//绿线
				BaseSprite *top8WinImg = getLuXianByIdx(DianFengDuiJueManager::Round_4_2, i); //已设置位置信息
				mBg->addChild(top8WinImg);
				//嵌套 2-1
				//当2-1打完了，且i 英雄输了   //1.ko字样 + 2.灰色名字 + 3.回放按钮
				if (curState > DianFengDuiJueManager::Round_2_1 
					&& ! DianFengDuiJueManager::getManager()->is_hero_win_in_round(oneHeroInfo, DianFengDuiJueManager::Round_2_1))
				{     //输了
					nameColor = fonColor_CiYao;
					BaseSprite *koImg = BaseSprite::create("dfdj_ko.png");
					kuang->addChild(koImg);
					koImg->setPosition(ccp(kuang->getContentSize().width/2, kuang->getContentSize().height/2));
					koImg->setScale(1 / item->getScale());
					//
					CCMenuItemImage *playBackItem = CCMenuItemImage::create(
						"dfdj_anniu_huifang.png",
						"dfdj_anniu_huifang_select.png",
						this, menu_selector(HeroRankDuel::menuItemClicked_PlayBack));
					playBackItem->setUserData((void*)DianFengDuiJueManager::Round_2_1);
					mPlayBackMenu->addChild(playBackItem, 0, Tag_PlayBackMenuItem_ + i);
					playBackItem->setPosition(getPlayBackOrGuessItemPositionByIdx(DianFengDuiJueManager::Round_2_1, i));
				}
				else if (curState > DianFengDuiJueManager::Round_2_1) //赢了    4.加绿线  
				{
					//绿线
					BaseSprite *top8WinImg = getLuXianByIdx(DianFengDuiJueManager::Round_2_1, i); //已设置位置信息
					mBg->addChild(top8WinImg);
					//2-1赢了  加 1头像 2边框 3名字 4胜利
					CCSprite *top1Img = (CCSprite*)mBg->getChildByTag(Tag_Img_Top1);/////////////////
					CCMenuItemImage *top1Item = CCMenuItemImage::create(
						str.c_str(), str.c_str(), this, menu_selector(HeroRankDuel::menuItemClicked_HeroImg));
					mHeroImgMenu->addChild(top1Item, 0);
					top1Item->setPosition(top1Img->getPosition());
					top1Item->setUserData((void *)oneHeroInfo.hero_id);
					top1Item->setScale(0.7f);
					top1Img->removeFromParent();///////////////////////////////////
					BaseSprite *kuang2 = BaseSprite::create("renxing_renwu_touxiang_select.png");
					top1Item->addChild(kuang2);
					kuang2->setPosition(ccp(top1Item->getContentSize().width/2, top1Item->getContentSize().height/2));
					CCLabelTTF *slLabel = CCLabelTTF::create(LFStrings::getValue("shengli").c_str(), fontStr_kaiti, 24);
					mBg->addChild(slLabel);
					slLabel->setPosition(ccp(320, 860-478));
					slLabel->setColor(fonColor_ChengGong);
					CCLabelTTF *nameLa = CCLabelTTF::create(oneHeroInfo.name, fontStr_kaiti, 20);
					mBg->addChild(nameLa);
					nameLa->setPosition(ccp(320, 860-511));
				}
			}
		}

		str = oneHeroInfo.name;
		CCLabelTTF *nameLabel = CCLabelTTF::create(str.c_str(), fontStr_kaiti, Label_Size_Min);
		nameLabel->setScale(1 / item->getScale());
		kuang->addChild(nameLabel);
		nameLabel->setPosition(ccp(kuang->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
		nameLabel->setColor(nameColor);
	}


	//竞猜按钮
	if (curState == DianFengDuiJueManager::Round_8_4)
	{
		for (int i=0; i!=4; ++i)
		{
			CCMenuItemImage *item = CCMenuItemImage::create(
				"dfdj_anniu_jingcai.png",
				"dfdj_anniu_jingcai_select.png",
				this, menu_selector(HeroRankDuel::menuItemClicked_GuessMenu));
			mGuessMenu->addChild(item, 0, Tag_GuessMenuItem_ + DianFengDuiJueManager::Round_8_4); //8进4
			item->setUserData((void*)i);
			item->setPosition(getPlayBackOrGuessItemPositionByIdx(DianFengDuiJueManager::Round_8_4, i*2));
		}
	}
	else if (curState == DianFengDuiJueManager::Round_4_2)
	{
		for (int i=0; i!=2; ++i)
		{
			CCMenuItemImage *item = CCMenuItemImage::create(
				"dfdj_anniu_jingcai.png",
				"dfdj_anniu_jingcai_select.png",
				this, menu_selector(HeroRankDuel::menuItemClicked_GuessMenu));
			mGuessMenu->addChild(item, 0, Tag_GuessMenuItem_ + DianFengDuiJueManager::Round_4_2);
			item->setUserData((void*)i);
			item->setPosition(getPlayBackOrGuessItemPositionByIdx(DianFengDuiJueManager::Round_4_2, i*4));
		}
	}
	else if (curState == DianFengDuiJueManager::Round_2_1)
	{
		CCMenuItemImage *item = CCMenuItemImage::create(
			"dfdj_anniu_jingcai.png",
			"dfdj_anniu_jingcai_select.png",
			this, menu_selector(HeroRankDuel::menuItemClicked_GuessMenu));
		mGuessMenu->addChild(item, 0, Tag_GuessMenuItem_ + DianFengDuiJueManager::Round_2_1);
		item->setUserData((void *)0);
		item->setPosition(getPlayBackOrGuessItemPositionByIdx(DianFengDuiJueManager::Round_2_1, 0));
	}
}

void HeroRankDuel::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
	mHeroImgMenu->setEnabled(false);
	mGuessMenu->setEnabled(false);
	mPlayBackMenu->setEnabled(false);
}

void HeroRankDuel::disableAllTouchEnd()
{
	mMainMenu->setEnabled(true);
	mHeroImgMenu->setEnabled(true);
	mGuessMenu->setEnabled(true);
	mPlayBackMenu->setEnabled(true);
}

void HeroRankDuel::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if (tag == Tag_MainMenuItem_Exit)
	{
		GameDirector::getDirector()->mainScene(mainlayer_HuoDong);
	}
	else if (tag == Tag_MainMenuItem_Rule)
	{
		disableAllTouchBegin();
		//
		BaseSprite *board = BaseSprite::create("dfdj_shuoming.png");
		mBg->addChild(board, 100, Tag_Board_);
		board->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-480));
		BaseSprite *bar = BaseSprite::create("guizeshuoming.png");
		board->addChild(bar);
		bar->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height-52));
		//
		CCMenu *menu = CCMenu::create();
		board->addChild(menu);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *closeItem = CCMenuItemImage::create(
			"close.png",
			"close_select.png",
			this,menu_selector(HeroRankDuel::menuItemClicked_CloseBoard));
		menu->addChild(closeItem, 0, Tag_BoardItem_Close);
		closeItem->setPosition(ccp(board->getContentSize().width - closeItem->getContentSize().width, bar->getPositionY()));
// 		规则说明：
// 			1.每周三凌晨4点，英雄榜排名前8的英雄，获得参加资格。
// 			2.所有英雄，每天可以选择一场比赛进行竞猜。竞猜时间为周三（八强赛），周四（四强赛），周五（巅峰对决）的当日凌晨4点到次日凌晨4点。
// 			3.对决将会在每周四、周五、周六的凌晨4点开始。
// 			4.对决采用三局二胜制，率先获得二局胜利者将晋级到下一轮，最后胜出者将成为巅峰王者。
// 
// 			王者奖励：银币100万
// 			亚军奖励：银币60万
// 			四强奖励：银币30万
// 			参与奖励：银币10万
		CCLabelTTF *ruleLabel = CCLabelTTF::create(LFStrings::getValue("DFDJ_GuiZeShuoMing").c_str(), fontStr_kaiti, Label_Size_Min
			, CCSize(540,0), kCCTextAlignmentLeft);
		board->addChild(ruleLabel);
		ruleLabel->setAnchorPoint(ccp(0, 1));
		ruleLabel->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Fumo, false, false));
		ruleLabel->setPosition(ccp(36, 730-122));
		CCLabelTTF *awardLabel = CCLabelTTF::create(LFStrings::getValue("DFDJ_GuiZeJiangLi").c_str(), fontStr_kaiti, Label_Size_Min
			, CCSize(540,0), kCCTextAlignmentLeft);
		board->addChild(awardLabel);
		awardLabel->setAnchorPoint(ccp(0, 1));
		awardLabel->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Xiyou, false, false));
		awardLabel->setPosition(ccp(ruleLabel->getPositionX(), ruleLabel->getPositionY()-ruleLabel->getContentSize().height - 30));
	}
	else if (tag == Tag_MainMenuItem_MyGuess)
	{
		disableAllTouchBegin();
		//
		BaseSprite *board = BaseSprite::create("dfdj_shuoming.png");
		mBg->addChild(board, 100, Tag_Board_);
		board->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-480));
		BaseSprite *bar = BaseSprite::create("dfdj_tiao_wodejingcai.png");
		board->addChild(bar);
		bar->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height-52));
		//
		CCMenu *menu = CCMenu::create();
		board->addChild(menu);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *closeItem = CCMenuItemImage::create(
			"close.png",
			"close_select.png",
			this,menu_selector(HeroRankDuel::menuItemClicked_CloseBoard));
		menu->addChild(closeItem, 0, Tag_BoardItem_Close);
		closeItem->setPosition(ccp(board->getContentSize().width - closeItem->getContentSize().width, bar->getPositionY()));

		int guessedNum = DianFengDuiJueManager::getManager()->myGuessData().size();
		if (guessedNum == 0)
			return;

		for (int i=0; i<guessedNum; ++i)
		{
			Guess oneGuess = DianFengDuiJueManager::getManager()->myGuessData().at(i);
			//竞猜列表
			BaseSprite *bar = BaseSprite::create("dfdj_tiao_wodejingcai_bg.png");
			board->addChild(bar);
			bar->setPosition(ccp(305, 730-148 - i*116));
			//
			string str;
			{//竞猜场次
				str = LFStrings::getValue("JingCai") + LFStrings::getValue("huihe") + ":"; 
				CCLabelTTF *title1 = CCLabelTTF::create(str.c_str(), fontStr_kaiti, Label_Size_Min);
				bar->addChild(title1);
				title1->setPosition(ccp(61, 116-24));
				title1->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Fumo, false, false));
				CCLabelTTF *vsLabel = CCLabelTTF::create("VS", fontStr_BookAntiqua, Label_Size_Min);
				bar->addChild(vsLabel);
				vsLabel->setPosition(ccp(566/2, title1->getPositionY()));
				CCLabelTTF *nameA = CCLabelTTF::create(oneGuess.hero_a.name, fontStr_kaiti, Label_Size_Min);
				nameA->setColor(fonColor_CiYao);
				bar->addChild(nameA);
				nameA->setAnchorPoint(ccp(1, 0.5));
				nameA->setPosition(ccp(vsLabel->getPositionX()-20, vsLabel->getPositionY()));
				CCLabelTTF *nameB = CCLabelTTF::create(oneGuess.hero_b.name, fontStr_kaiti, Label_Size_Min);
				nameB->setColor(fonColor_CiYao);
				bar->addChild(nameB);
				nameB->setAnchorPoint(ccp(0, 0.5));
				nameB->setPosition(ccp(vsLabel->getPositionX()+20, vsLabel->getPositionY()));
			}
			string myGuessHeroName;
			{//竞猜英雄
				str = LFStrings::getValue("JingCai") + LFStrings::getValue("YingXiong") + ":"; 
				CCLabelTTF *title2 = CCLabelTTF::create(str.c_str(), fontStr_kaiti, Label_Size_Min);
				bar->addChild(title2);
				title2->setPosition(ccp(61, 116-56));
				title2->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Fumo, false, false));
				if (oneGuess.i_guess_winner_is == 0) str = oneGuess.hero_a.name;
				else str = oneGuess.hero_b.name;
				myGuessHeroName = str;
				CCLabelTTF *guessNameLabel = CCLabelTTF::create(str.c_str(), fontStr_kaiti, Label_Size_Min);
				bar->addChild(guessNameLabel);
				guessNameLabel->setPosition(ccp(566/2, title2->getPositionY()));
			}
			{//竞猜结果
				str = LFStrings::getValue("JingCai") + LFStrings::getValue("JieGuo") + ":"; 
				CCLabelTTF *title3 = CCLabelTTF::create(str.c_str(), fontStr_kaiti, Label_Size_Min);
				bar->addChild(title3);
				title3->setPosition(ccp(61, 116-90));
				title3->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Fumo, false, false));
				if (oneGuess.am_i_right == 1) str = myGuessHeroName;
				else if (oneGuess.am_i_right == 2) str = (myGuessHeroName!=oneGuess.hero_a.name ? oneGuess.hero_a.name : oneGuess.hero_b.name);
				else str.clear();
				CCLabelTTF *jieguoLabel = CCLabelTTF::create(str.c_str(), fontStr_kaiti, Label_Size_Min);
				bar->addChild(jieguoLabel);
				jieguoLabel->setPosition(ccp(566/2, title3->getPositionY()));
				jieguoLabel->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Xiyou, false, false));
			}
		}
	}
}

void HeroRankDuel::menuItemClicked_GuessMenu(CCObject *pSender)
{
	disableAllTouchBegin();


	int round = ((CCMenuItem*)pSender)->getTag() - Tag_GuessMenuItem_;
	int iii = (int)((CCMenuItem*)pSender)->getUserData();

	HeroInfo hero[2];
	if (round == DianFengDuiJueManager::Round_8_4)
	{
		hero[0] = mTop8HeroInfoVec.at(iii*2);
		hero[1] = mTop8HeroInfoVec.at(iii*2+1);
	}
	else if (round == DianFengDuiJueManager::Round_4_2)
	{
		int num = 0;
		for (int i=iii*4; i<iii*4+4; ++i)
		{
			if (DianFengDuiJueManager::getManager()->is_hero_win_in_round(mTop8HeroInfoVec.at(i), DianFengDuiJueManager::Round_8_4))
			{
				hero[num++] = mTop8HeroInfoVec.at(i);
				if (num == 2) break;
			}
		}
	}
	else 
	{
		int num = 0;
		for (int i=0; i<8; ++i)
		{
			if (DianFengDuiJueManager::getManager()->is_hero_win_in_round(mTop8HeroInfoVec.at(i), DianFengDuiJueManager::Round_4_2))
			{
				hero[num++] = mTop8HeroInfoVec.at(i);
				if (num == 2) break;
			}
		}
	}

	pair<HeroInfo, HeroInfo> guessHero;
	guessHero.first = hero[0];
	guessHero.second = hero[1];
	mCurGuessHero = guessHero;

	addObserver(callfuncO_selector(HeroRankDuel::rsp_QueryShenJia), MSG_TopArenaQueryGuessRsp);
	mCmdHlp->cmdTopArenaQueryGuess(DianFengDuiJueManager::getManager()->getGroupIndexInRound(hero[0].hero_id,(DianFengDuiJueManager::Round)round));
}

void HeroRankDuel::rsp_QueryShenJia(CCObject *msg)
{
	removeObserver(MSG_TopArenaQueryGuessRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_TopArenaQueryGuessRsp *data  = (SPCmd_TopArenaQueryGuessRsp*)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	
	//
	BaseSprite *board = BaseSprite::create("dfdj_shuoming.png");
	mBg->addChild(board, 100, Tag_Board_);
	board->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-480));
	BaseSprite *bar = BaseSprite::create("dfdj_tiao_bisaijingcai.png");
	board->addChild(bar);
	bar->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height-52));
	//
	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 0, Tag_Board_Menu);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *closeItem = CCMenuItemImage::create(
		"close.png",
		"close_select.png",
		this,menu_selector(HeroRankDuel::menuItemClicked_CloseBoard));
	menu->addChild(closeItem, 0, Tag_BoardItem_Close);
	closeItem->setPosition(ccp(board->getContentSize().width - closeItem->getContentSize().width, bar->getPositionY()));
	CCMenuItemImage *makesureItem = CCMenuItemImage::create(
		"zhuangbei_xiangxi_queding.png",
		"zhuangbei_xiangxi_queding_select.png",
		this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
	menu->addChild(makesureItem, 0, 10000);
	makesureItem->setPosition(ccp(305, 730-667));

	//board  init
	mGuessHero = 0;
	mGuessSilverIdx = 0;
	//标题：选择任意一方进行竞猜
	CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("SFDJ_JingCaiTiShi").c_str(), fontStr_kaiti, 20);
	board->addChild(tishiLabel);
	tishiLabel->setAnchorPoint(ccp(0, 0.5));
	tishiLabel->setPosition(ccp(30, 730-112));
	tishiLabel->setColor(fonColor_CiYao);
	//头像kuang name  vs  头像 "选择"  item  、、身价
	CCString tempStr;
	//first
	{
		BaseSprite *headImg = BaseSprite::create(ImageManager::getManager()->getImageData_ProfessionHead(
			(Profession)mCurGuessHero.first.profession_id).getCurrentImageFile());
		board->addChild(headImg);
		headImg->setPosition(ccp(163, 730-209));
		BaseSprite *kuang = BaseSprite::create("renxing_renwu_touxiang_select.png");
		headImg->addChild(kuang);
		kuang->setPosition(ccp(headImg->getContentSize().width/2, headImg->getContentSize().height/2));
		CCLabelTTF *nameLabel = CCLabelTTF::create(mCurGuessHero.first.name, fontStr_kaiti, Label_Size_Min);
		kuang->addChild(nameLabel);
		nameLabel->setPosition(ccp(kuang->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
		CCLabelTTF *xuanZeItem = CCLabelTTF::create(LFStrings::getValue("XuanZe").c_str(), fontStr_kaiti, Label_Size_Min);
		board->addChild(xuanZeItem);
		xuanZeItem->setColor(fonColor_CiYao);
		xuanZeItem->setPosition(ccp(65, 730-192));
		CCMenuItemImage *selectItem = CCMenuItemImage::create(
			"duiwu_yingxiong_daixuan.png",
			"duiwu_yingxiong_xuanzhong.png",
			this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
		menu->addChild(selectItem, 0, 1);
		selectItem->setPosition(ccp(64, 730-238));
		//
		tempStr = LFStrings::getValue("ShengJia") + ":";
		CCLabelTTF *sjLabel = CCLabelTTF::create(tempStr.getCString(), fontStr_kaiti, Label_Size_Min);
		board->addChild(sjLabel);
		sjLabel->setColor(fonColor_PuTong);
		BaseSprite *silIcon = BaseSprite::create("yinbi_tubiao.png");
		sjLabel->addChild(silIcon);
		silIcon->setPosition(ccp(sjLabel->getContentSize().width+silIcon->getContentSize().width/2, sjLabel->getContentSize().height/2));
		tempStr.initWithFormat("%u", DianFengDuiJueManager::getManager()->getHeroValueInCurrentRoundByGroupIndex(mCurGuessHero.first.hero_id));
		CCLabelTTF *numLabel = CCLabelTTF::create(tempStr.getCString(), fontStr_BookAntiqua, Label_Size_Min);
		silIcon->addChild(numLabel);
		numLabel->setColor(fonColor_PuTong);
		numLabel->setPosition(ccp(silIcon->getContentSize().width+numLabel->getContentSize().width/2, silIcon->getContentSize().height/2));

		sjLabel->setAnchorPoint(ccp(0, 0.5));
		sjLabel->setPosition(ccp(163-(sjLabel->getContentSize().width+silIcon->getContentSize().width+numLabel->getContentSize().width)/2, 730-324));
	}
	//second
	{
		BaseSprite *headImg = BaseSprite::create(ImageManager::getManager()->getImageData_ProfessionHead(
			(Profession)mCurGuessHero.second.profession_id).getCurrentImageFile());
		board->addChild(headImg);
		headImg->setPosition(ccp(451, 730-209));
		BaseSprite *kuang = BaseSprite::create("renxing_renwu_touxiang_select.png");
		headImg->addChild(kuang);
		kuang->setPosition(ccp(headImg->getContentSize().width/2, headImg->getContentSize().height/2));
		CCLabelTTF *nameLabel = CCLabelTTF::create(mCurGuessHero.second.name, fontStr_kaiti, Label_Size_Min);
		kuang->addChild(nameLabel);
		nameLabel->setPosition(ccp(kuang->getContentSize().width/2, -nameLabel->getContentSize().height/2 - 6));
		CCLabelTTF *xuanZeItem = CCLabelTTF::create(LFStrings::getValue("XuanZe").c_str(), fontStr_kaiti, Label_Size_Min);
		board->addChild(xuanZeItem);
		xuanZeItem->setColor(fonColor_CiYao);
		xuanZeItem->setPosition(ccp(550, 730-192));
		CCMenuItemImage *selectItem = CCMenuItemImage::create(
			"duiwu_yingxiong_daixuan.png",
			"duiwu_yingxiong_xuanzhong.png",
			this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
		menu->addChild(selectItem, 0, 2);
		selectItem->setPosition(ccp(549, 730-238));
		//
		tempStr = LFStrings::getValue("ShengJia") + ":";
		CCLabelTTF *sjLabel = CCLabelTTF::create(tempStr.getCString(), fontStr_kaiti, Label_Size_Min);
		board->addChild(sjLabel);
		sjLabel->setColor(fonColor_PuTong);
		BaseSprite *silIcon = BaseSprite::create("yinbi_tubiao.png");
		sjLabel->addChild(silIcon);
		silIcon->setPosition(ccp(sjLabel->getContentSize().width+silIcon->getContentSize().width/2, sjLabel->getContentSize().height/2));
		tempStr.initWithFormat("%u", DianFengDuiJueManager::getManager()->getHeroValueInCurrentRoundByGroupIndex(mCurGuessHero.second.hero_id));
		CCLabelTTF *numLabel = CCLabelTTF::create(tempStr.getCString(), fontStr_BookAntiqua, Label_Size_Min);
		silIcon->addChild(numLabel);
		numLabel->setColor(fonColor_PuTong);
		numLabel->setPosition(ccp(silIcon->getContentSize().width+numLabel->getContentSize().width/2, silIcon->getContentSize().height/2));

		sjLabel->setAnchorPoint(ccp(0, 0.5));
		sjLabel->setPosition(ccp(451-(sjLabel->getContentSize().width+silIcon->getContentSize().width+numLabel->getContentSize().width)/2, 730-324));
	}
	BaseSprite *vsImg = BaseSprite::create("dianfengduijue_vs1.png");
	board->addChild(vsImg);
	vsImg->setPosition(ccp(305, 730-213));

	//选择竞猜金娥
	CCLabelTTF *xuanzeJineLabel = CCLabelTTF::create((LFStrings::getValue("XuanZe")+LFStrings::getValue("JingCaiJinE")).c_str(), fontStr_kaiti, Label_Size_Min);
	board->addChild(xuanzeJineLabel);
	xuanzeJineLabel->setAnchorPoint(ccp(0, 0.5));
	xuanzeJineLabel->setColor(fonColor_CiYao);
	xuanzeJineLabel->setPosition(ccp(30, 730-374));
	{
		CCString temp;
		CCMenuItemImage *guessItem = CCMenuItemImage::create(
			"duiwu_yingxiong_daixuan.png",
			"duiwu_yingxiong_xuanzhong.png",
			this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
		menu->addChild(guessItem, 0, 11);
		guessItem->setScale(0.8f);
		guessItem->setPosition(ccp(59+60, 730-440));
		BaseSprite *silIcon = BaseSprite::create("yinbi_tubiao.png");
		board->addChild(silIcon);
		silIcon->setPosition(ccp(guessItem->getPositionX()+guessItem->getContentSize().width/2 + silIcon->getContentSize().width/2,
			guessItem->getPositionY()));
		unsigned int guessNum = CS::getTopArenaWager(1);
		temp.initWithFormat("%u", guessNum);
		CCLabelTTF *guessNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
		board->addChild(guessNumLabel);
		guessNumLabel->setPosition(ccp(silIcon->getPositionX()+silIcon->getContentSize().width/2+guessNumLabel->getContentSize().width/2,
			silIcon->getPositionY()));
		guessNumLabel->setColor(fonColor_PuTong);
	}
	{
		CCString temp;
		CCMenuItemImage *guessItem = CCMenuItemImage::create(
			"duiwu_yingxiong_daixuan.png",
			"duiwu_yingxiong_xuanzhong.png",
			this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
		menu->addChild(guessItem, 0, 12);
		guessItem->setScale(0.8f);
		guessItem->setPosition(ccp(419-60, 730-440));
		BaseSprite *silIcon = BaseSprite::create("yinbi_tubiao.png");
		board->addChild(silIcon);
		silIcon->setPosition(ccp(guessItem->getPositionX()+guessItem->getContentSize().width/2 + silIcon->getContentSize().width/2,
			guessItem->getPositionY()));
		unsigned int guessNum = CS::getTopArenaWager(2);
		temp.initWithFormat("%u", guessNum);
		CCLabelTTF *guessNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
		board->addChild(guessNumLabel);
		guessNumLabel->setPosition(ccp(silIcon->getPositionX()+silIcon->getContentSize().width/2+guessNumLabel->getContentSize().width/2,
			silIcon->getPositionY()));
		guessNumLabel->setColor(fonColor_PuTong);
	}

	//shuoming
	CCLabelTTF *smLabel = CCLabelTTF::create(LFStrings::getValue("ShuoMing").c_str(), fontStr_kaiti, Label_Size_Min);
	board->addChild(smLabel);
	smLabel->setAnchorPoint(ccp(0, 1));
	smLabel->setColor(fonColor_CiYao);
	smLabel->setPosition(ccp(30, 730-490));
	CCLabelTTF *directionLabel = CCLabelTTF::create(LFStrings::getValue("JingCaiShuoMing_").c_str(), fontStr_kaiti, Label_Size_Min
		, CCSize(500,0), kCCTextAlignmentLeft);
	board->addChild(directionLabel);
	directionLabel->setColor(fonColor_CiYao);
	directionLabel->setAnchorPoint(ccp(0, 1));
	directionLabel->setPosition(ccp(smLabel->getPositionX()+smLabel->getContentSize().width + 6, smLabel->getPositionY()));
}

void HeroRankDuel::menuItemClicked_PlayBack(CCObject *pSender)
{
	disableAllTouchBegin();
	//
	BaseSprite *board = BaseSprite::create("dfdj_shuoming.png");
	mBg->addChild(board, 100, Tag_Board_);
	board->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-480));
	BaseSprite *bar = BaseSprite::create("dfdj_tiao_zhandouhuifang.png");
	board->addChild(bar);
	bar->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height-52));
	//
	CCMenu *menu = CCMenu::create();
	board->addChild(menu);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *closeItem = CCMenuItemImage::create(
		"close.png",
		"close_select.png",
		this,menu_selector(HeroRankDuel::menuItemClicked_CloseBoard));
	menu->addChild(closeItem, 0, Tag_BoardItem_Close);
	closeItem->setPosition(ccp(board->getContentSize().width - closeItem->getContentSize().width, bar->getPositionY()));

	//
	int iIdx = ((CCMenuItem*)pSender)->getTag() - Tag_PlayBackMenuItem_;
	int round = (int)((CCMenuItem*)pSender)->getUserData();
	int group = DianFengDuiJueManager::getManager()->getGroupIndexInRound(mTop8HeroInfoVec.at(iIdx).hero_id, (DianFengDuiJueManager::Round)round);
	if (group == -1)
	{
		FXToast *layer = FXToast::create("Error!");
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	pair<HeroInfo, HeroInfo> heros = DianFengDuiJueManager::getManager()->getOneGroup(group, (DianFengDuiJueManager::Round)round);
	for (int i=0; i<3; ++i)
	{
		BaseSprite *bar = BaseSprite::create("dfdj_tiao_zhandouhuifang_bg.png");
		board->addChild(bar, 0, i);
		bar->setPosition(ccp(board->getContentSize().width/2, 730-131-i*bar->getContentSize().height));
		{
			CCLabelTTF *name1 = CCLabelTTF::create(heros.first.name, fontStr_kaiti, Label_Size_Min);
			bar->addChild(name1);
			name1->setPosition(ccp(110, 82-26));
			name1->setColor(fonColor_CiYao);
			CCLabelTTF *vsImg = CCLabelTTF::create("VS", fontStr_BookAntiqua, Label_Size_Min);
			bar->addChild(vsImg);
			vsImg->setPosition(ccp(210, name1->getPositionY()));
			vsImg->setColor(fonColor_CiYao);
			CCLabelTTF *name2 = CCLabelTTF::create(heros.second.name, fontStr_kaiti, Label_Size_Min);
			bar->addChild(name2);
			name2->setPosition(ccp(310, name1->getPositionY()));
			name2->setColor(fonColor_CiYao);
		}
		{
			string winerName = heros.first.name;
			if (DianFengDuiJueManager::getManager()->is_hero_win_in_round(heros.second, (DianFengDuiJueManager::Round)round))
				winerName = heros.second.name;
			CCLabelTTF *name = CCLabelTTF::create(winerName.c_str(), fontStr_kaiti, Label_Size_Min);
			bar->addChild(name);
			name->setPosition(ccp(110, 82-56));
			name->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Xiyou, false, false));
			CCLabelTTF *winLabel = CCLabelTTF::create(LFStrings::getValue("shengli").c_str(), fontStr_kaiti, Label_Size_Min);
			bar->addChild(winLabel);
			winLabel->setPosition(ccp(210, name->getPositionY()));
			winLabel->setColor(name->getColor());
		}
		CCMenu *menu = CCMenu::create();
		bar->addChild(menu);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *item = CCMenuItemImage::create(
			"dfdj_anniu_zhandouhuifang.png",
			"dfdj_anniu_zhandouhuifang_select.png",
			this, menu_selector(HeroRankDuel::menuItemClicked_PlayBackBoard));
		menu->addChild(item, 0, i);
		item->setPosition(ccp(494, bar->getContentSize().height/2));
		item->setUserData((void*)round);
		menu->setUserData((void*)group);
	}
}

void HeroRankDuel::menuItemClicked_HeroImg(CCObject *pSender)
{
	int tag = (int)((CCMenuItemImage*)pSender)->getUserData();

	this->addObserver(callfuncO_selector(HeroRankDuel::rsp_queryAnotherTeamInfo), MSG_queryAnotherTeamInfoRsp_F);
	mCmdHlp->cmdQueryAnotherTeamInfo(tag);
}

void HeroRankDuel::menuItemClicked_CloseBoard(CCObject *pSender)
{
	mBg->removeChildByTag(Tag_Board_);
	disableAllTouchEnd();
}

void HeroRankDuel::rsp_queryAnotherTeamInfo(CCObject* msg_f)
{
	removeObserver(MSG_queryAnotherTeamInfoRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_QueryAnotherTeamInfoRsp* data = (Flat_QueryAnotherTeamInfoRsp*) ((Flat_MSG_Rsp*)msg_f)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	vector<HeroDetail> heroDetailVec;
	for(int i=0; i!=4; ++i)
	{
		if(data->hero_arr[i].basic_info.hero_id != 0) 
			heroDetailVec.push_back(data->hero_arr[i]);
		else continue;
	}
	vector<EquipData> heroEquipVec[4];  //按照 heroDetail排序
	for (int i=0; i!=data->zhuangbei_vec.size(); ++i)
	{
		for(int j=0; j!=heroDetailVec.size(); ++j)
		{
			if(data->zhuangbei_vec.at(i).zhuangbei_hero_id == heroDetailVec.at(j).basic_info.hero_id)
			{
				heroEquipVec[j].push_back(data->zhuangbei_vec.at(i));
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//创建actor  并查询元神
	mQueryYuanShen = new CCArray;
	mQueryYuanShen->init();
	mQueryHeroActors = new CCArray;     //----remember delete (获取元神数据后，创建并传递给ZhuangBeiMianBanOther后，再release)
	mQueryHeroActors->initWithCapacity(heroDetailVec.size());
	mQueryActorsEquip.clear();
	mNum_YuanShen = 0;
	for (int i=0; i!=heroDetailVec.size(); ++i)
	{
		BaseActor *heroActor = NULL;
		HeroBaseProperty propData = HeroBaseProperty(&heroDetailVec.at(i));
		propData.initFixedProValue(BaseActorPropertyParser::getParser()->getExtraProerty(propData.profession));
		heroActor = new BaseActor(propData); // 1. remember delete actor
		vector<Equipment*> equipVec;   //2. will delete equips
		for(int eq_idx=0; eq_idx!=heroEquipVec[i].size(); ++eq_idx)
		{
			equipVec.push_back(new Equipment(heroEquipVec[i].at(eq_idx)));
		}
		heroActor->initEquipment(equipVec);
		//skill
		for (int skillIdx=0; skillIdx<4; ++skillIdx)
		{
			if (data->skill_arr[skillIdx].skill_id != 0 && data->skill_arr[skillIdx].owner_hero_id == heroActor->getId()){
				SkillData *skill_data = new SkillData(data->skill_arr[skillIdx]);
				heroActor->setCurrentUseSkill(skill_data);
				delete skill_data;
			}
		}

		mQueryHeroActors->addObject(heroActor);
		heroActor->release();  // delete actor
		for (int eq_idx=0; eq_idx!=equipVec.size(); ++eq_idx) // delete equips 传递给mianbanOther，等它用完之后再删除、 
			mQueryActorsEquip.push_back(equipVec.at(eq_idx));

		if (heroDetailVec.at(i).basic_info.level >= 30) ++mNum_YuanShen; 
	}

	/////////  query 元神info  
	if (mNum_YuanShen > 0)
	{
		addObserver(callfuncO_selector(HeroRankDuel::rsp_queryAnotherTeamYuanshen), MSG_queryYuanshenOfHeroRsp_F);
		for (int i=0; i!=heroDetailVec.size(); ++i)
		{
			if (heroDetailVec.at(i).basic_info.level >= 30)
				mCmdHlp->cmdQueryYuanshenOfHero(heroDetailVec.at(i).basic_info.hero_id);
		}
	}
	else
	{
		rsp_queryAnotherTeamYuanshen(NULL);
	}
}

void HeroRankDuel::rsp_queryAnotherTeamYuanshen(CCObject *msg_f)
{
	if (msg_f != NULL)  //==NULL表示 没有元神
	{
		Flat_QueryYuanshenOfHeroRsp *data = (Flat_QueryYuanshenOfHeroRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
		if (data->err_code != Success)
		{
			removeObserver(MSG_queryYuanshenOfHeroRsp_F);
			GameDirector::getDirector()->hideWaiting();
			MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		BaseActor *yuanshen_actor = NULL;
		HeroBaseProperty propData = HeroBaseProperty(&data->hero_detail);
		propData.initFixedProValue(BaseActorPropertyParser::getParser()->getExtraProerty(propData.profession));
		yuanshen_actor = new BaseActor(propData); // 1. remember delete actor
		vector<Equipment*> equipVec;   //2. will delete equips
		for(int eq_idx=0; eq_idx!=data->equips.size(); ++eq_idx)
		{
			equipVec.push_back(new Equipment(data->equips.at(eq_idx)));
		}
		yuanshen_actor->initEquipment(equipVec);
		for (int i=0; i!=mQueryHeroActors->count(); ++i)
		{
			BaseActor *curhero = (BaseActor*)mQueryHeroActors->objectAtIndex(i);
			if (curhero->getCurrentProperty().hero_id == data->hero_detail.basic_info.zhenshen_id)
			{
				((BaseActor*)mQueryHeroActors->objectAtIndex(i))->setYuanShen(yuanshen_actor);
				break;
			}
		}

		//内存管理
		mQueryYuanShen->addObject(yuanshen_actor);
		yuanshen_actor->release();        // delete actor 1
		for (int i=0; i!=equipVec.size(); ++i)
			mQueryActorsEquip.push_back(equipVec.at(i)); // delete equips 2

		--mNum_YuanShen;
		if (mNum_YuanShen != 0) return;

		removeObserver(MSG_queryYuanshenOfHeroRsp_F);
		GameDirector::getDirector()->hideWaiting();
	}


	///////////////
	ZhuangBeiMianBanOther *equipDetailLayer = ZhuangBeiMianBanOther::create(mQueryHeroActors, mQueryYuanShen, mQueryActorsEquip, 0);
	mFooterLayer->popUpContaintLayer(equipDetailLayer);

	mQueryHeroActors->release();   //查询hero info创建actor的时候 new的、
	mQueryHeroActors = NULL;
	mQueryYuanShen->release();
	mQueryYuanShen = NULL;
}

BaseSprite* HeroRankDuel::getLuXianByIdx(int round, int idx)
{
	BaseSprite *img = NULL;
	CCPoint pos;
	if (round == DianFengDuiJueManager::Round_8_4)
	{
		img = BaseSprite::create("dfdj_tiao_1.png");
		if (idx % 2 == 1)  // 1 3 5 7
			img->setFlipY(true);
		if (idx > 3)  // 4 5 6 7
			img->setFlipX(true);
	}
	else if (round == DianFengDuiJueManager::Round_4_2)
	{
		img = BaseSprite::create("dfdj_tiao_2.png");
		if (idx == 2 || idx == 3 || idx == 6 || idx == 7)
			img->setFlipY(true);
		if (idx > 3)
			img->setFlipX(true);
	}
	else if (round == DianFengDuiJueManager::Round_2_1)
	{
		img = BaseSprite::create("dfdj_tiao_3.png");
	}
	else 
		img = BaseSprite::create();



	//位置
	if (round == DianFengDuiJueManager::Round_8_4)
	{
		if (idx == 0) pos = ccp(133, 860-244);
		else if (idx == 1) pos = ccp(133, 860-329);
		else if (idx == 2) pos = ccp(133, 860-516);
		else if (idx == 3) pos = ccp(133, 860-601);
		else if (idx == 4) pos = ccp(508, 860-244);
		else if (idx == 5) pos = ccp(508, 860-329);
		else if (idx == 6) pos = ccp(508, 860-516);
		else if (idx == 7) pos = ccp(508, 860-601);
	}
	else if (round == DianFengDuiJueManager::Round_4_2)
	{
		if (idx == 0 || idx == 1) pos = ccp(196, 860-348);
		else if (idx == 2 || idx == 3) pos = ccp(196, 860-497);
		else if (idx == 4 || idx == 5) pos = ccp(444, 860-348);
		else if (idx == 6 || idx == 7) pos = ccp(444, 860-497);
	}
	else if (round == DianFengDuiJueManager::Round_2_1)
	{
		if (idx < 4) pos = ccp(257, 860-422);
		else pos = ccp(384, 860-422);
	}

	img->setPosition(pos);

	return img;
}

CCPoint HeroRankDuel::getPlayBackOrGuessItemPositionByIdx(int round, int idx)
{
	CCPoint pos;
	if (round == DianFengDuiJueManager::Round_8_4)
	{
		if (idx == 0 || idx == 1) pos = ccp(162, 860-284);
		else if (idx == 2 || idx == 3) pos = ccp(162, 860-561);
		else if (idx == 4 || idx == 5) pos = ccp(478, 860-284);
		else if (idx == 6 || idx == 7) pos = ccp(478, 860-561);
	}
	else if (round == DianFengDuiJueManager::Round_4_2)
	{
		if (idx < 4) pos = ccp(219, 860-423);
		else pos = ccp(422, 860-423);
	}
	else if (round == DianFengDuiJueManager::Round_2_1)
	{
		pos = ccp(320, 860-338);
	}

	return pos;
}

void HeroRankDuel::updateBeginTime(float dt)
{
	CCLabelTTF *timeLabel = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_Time);

	unsigned int time_ = DianFengDuiJueManager::getManager()->getOpenTime() - ServerTime::getTime();
	CCString temp;
	if (DianFengDuiJueManager::getManager()->getOpenTime() < ServerTime::getTime())
	{
		temp.initWithFormat("%s", LFStrings::getValue("JingCaiZhong").c_str());
		if (! mbOpen)//刚开启的   刷新界面
		{
			this->removeAllChildren();
			initLayer();
			return;
		}
	}
	else 
	{
		temp.initWithFormat("%s:%02d:%02d:%02d", LFStrings::getValue("daojishi").c_str(),
			time_/60/60, time_/60%60, time_%60);
		if (mbOpen)
		{
			this->removeAllChildren();
			initLayer();
			return;
		}
	}

	if (timeLabel) timeLabel->setString(temp.getCString());
}

void HeroRankDuel::menuItemClicked_GuessBoard(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	CCSprite *board = (CCSprite*)mBg->getChildByTag(Tag_Board_);
	if (board == NULL) 
	{
		CCLog("Error: %s ---> can not found board [tag == Tag_Board_]", __FUNCTION__);
		return;
	}
	CCMenu *menu = (CCMenu*)board->getChildByTag(Tag_Board_Menu);
	if (menu == NULL) return;

	CCMenuItemImage *item1 = (CCMenuItemImage*)menu->getChildByTag(1);
	CCMenuItemImage *item2 = (CCMenuItemImage*)menu->getChildByTag(2);
	CCMenuItemImage *item11 = (CCMenuItemImage*)menu->getChildByTag(11);
	CCMenuItemImage *item12 = (CCMenuItemImage*)menu->getChildByTag(12);
	if (! (item1 && item2 && item11 && item12))
		return;

	//选人
	if (tag == 1) // left
	{
		if (mGuessHero == 0)
		{
			item1->initWithNormalImage(
				"duiwu_yingxiong_xuanzhong.png",
				"duiwu_yingxiong_daixuan.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessHero = 1;
		}
		else if (mGuessHero == 1)
		{
			item1->initWithNormalImage(
				"duiwu_yingxiong_daixuan.png",
				"duiwu_yingxiong_xuanzhong.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessHero = 0;
		}
		else if (mGuessHero == 2)
		{
			item1->initWithNormalImage(
				"duiwu_yingxiong_xuanzhong.png",
				"duiwu_yingxiong_daixuan.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			item2->initWithNormalImage(
				"duiwu_yingxiong_daixuan.png",
				"duiwu_yingxiong_xuanzhong.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessHero = 1;
		}
	}
	else if (tag == 2) // right
	{
		if (mGuessHero == 0)
		{
			item2->initWithNormalImage(
				"duiwu_yingxiong_xuanzhong.png",
				"duiwu_yingxiong_daixuan.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessHero = 2;
		}
		else if (mGuessHero == 2)
		{
			item2->initWithNormalImage(
				"duiwu_yingxiong_daixuan.png",
				"duiwu_yingxiong_xuanzhong.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessHero = 0;
		}
		else if (mGuessHero == 1)
		{
			item2->initWithNormalImage(
				"duiwu_yingxiong_xuanzhong.png",
				"duiwu_yingxiong_daixuan.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			item1->initWithNormalImage(
				"duiwu_yingxiong_daixuan.png",
				"duiwu_yingxiong_xuanzhong.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessHero = 2;
		}
	}
	//押注
	else if (tag == 11)
	{
		if (mGuessSilverIdx == 0)
		{
			item11->initWithNormalImage(
				"duiwu_yingxiong_xuanzhong.png",
				"duiwu_yingxiong_daixuan.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessSilverIdx = 1;
		}
		else if (mGuessSilverIdx == 1)
		{
			item11->initWithNormalImage(
				"duiwu_yingxiong_daixuan.png",
				"duiwu_yingxiong_xuanzhong.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessSilverIdx = 0;
		}
		else if (mGuessSilverIdx == 2)
		{
			item11->initWithNormalImage(
				"duiwu_yingxiong_xuanzhong.png",
				"duiwu_yingxiong_daixuan.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			item12->initWithNormalImage(
				"duiwu_yingxiong_daixuan.png",
				"duiwu_yingxiong_xuanzhong.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessSilverIdx = 1;
		}
	}
	else if (tag == 12)
	{
		if (mGuessSilverIdx == 0)
		{
			item12->initWithNormalImage(
				"duiwu_yingxiong_xuanzhong.png",
				"duiwu_yingxiong_daixuan.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessSilverIdx = 2;
		}
		else if (mGuessSilverIdx == 2)
		{
			item12->initWithNormalImage(
				"duiwu_yingxiong_daixuan.png",
				"duiwu_yingxiong_xuanzhong.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessSilverIdx = 0;
		}
		else if (mGuessSilverIdx == 1)
		{
			item12->initWithNormalImage(
				"duiwu_yingxiong_xuanzhong.png",
				"duiwu_yingxiong_daixuan.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			item11->initWithNormalImage(
				"duiwu_yingxiong_daixuan.png",
				"duiwu_yingxiong_xuanzhong.png",
				NULL, this, menu_selector(HeroRankDuel::menuItemClicked_GuessBoard));
			mGuessSilverIdx = 2;
		}
	}

	//确定
	else if (tag == 10000)
	{
		if (mGuessHero == 0 || mGuessSilverIdx == 0)
		{
			FXToast *layer = FXToast::create(LFStrings::getValue("JingCai_Hero_Sil"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		HeroInfo hero;
		bool bbb = true;
		if (mGuessHero == 1) {hero = mCurGuessHero.first; bbb=true;}
		else {hero = mCurGuessHero.second; bbb=false;}
		int ground = DianFengDuiJueManager::getManager()->getGroupIndexInRound(hero.hero_id, DianFengDuiJueManager::getManager()->getCurrentRound());

		addObserver(callfuncO_selector(HeroRankDuel::rsp_guessHero), MSG_topArenaGuessRsp);
		mCmdHlp->cmdTopArenaGuess(ground, bbb, CS::getTopArenaWager(mGuessSilverIdx));
	}
}

void HeroRankDuel::menuItemClicked_PlayBackBoard(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	int round = (int)((CCMenuItem*)pSender)->getUserData();
	int group = (int)((CCMenuItem*)pSender)->getParent()->getUserData();

	addObserver(callfuncO_selector(HeroRankDuel::rsp_playBack), MSG_topArenaReplay_F);
	mCmdHlp->cmdTopArenaReplay(round, group, tag);
}

void HeroRankDuel::rsp_guessHero(CCObject *msg)
{
	removeObserver(MSG_topArenaGuessRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_TopArenaGuessRsp *data = (SPCmd_TopArenaGuessRsp *)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	this->removeAllChildren();
	initLayer();
}

void HeroRankDuel::rsp_playBack(CCObject *msg_f)
{
	removeObserver(MSG_topArenaReplay_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_topArenaReplayRsp *data = (Flat_topArenaReplayRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	FightDirector dir;
	vector<FightDirector> dir_vec;
	dir.initScript(data->fightScript);
	dir_vec.push_back(dir);

	Bundle bundle;
	bundle.putString("JumpToWhenEnd","DianFengDuiJueList");
	ClientFightLayer* layer = (ClientFightLayer*)GameDirector::getDirector()->fight(dir_vec
		,MapsManager::getManager()->getJingJiChangMapImage(),bundle);
}