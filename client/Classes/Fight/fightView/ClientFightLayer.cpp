#include "ClientFightLayer.h"
#include "GameDirector.h"
#include "Music.h"
#include "LFToast.h"
#include "FightSettlementLayer.h"
#include "MyselfManager.h"
#include "font.h"
#include "LFAlert.h"
#include "SpriteHelper.h"
#include "ImageManager.h"
#include "Bundle.h"
//#include "ShouChengLayer.h"
#include "TextureOpSprite.h"
#include "LFDelayToMusic.h"
#include "ChatContentManager.h"
#include "History.h"
#include "ChessBoardStateManager.h"
#include "ShenBingPool.h"
#include "SuitPool.h"
#include "TaskManager.h"
#include "SkillSprite.h"
#include "Debug.h"
#include "FightGuideFactory.h"
#include "SkillTree.h"
#include "JumpHolder.h"
#include "FightTrigleDriver.h"
#include "WorldBossCityGuardManager.h"
#include "CS_Common.h"
#include "HeroRankChallengeEndLayer.h"
#include "XueSeJiaoTangEndLayer.h"
#include "FX_CommonFunc.h"
#include "TaskManager.h"
#include "FightEndBaseLayer.h"
#include "UnblockManager.h"
#include "SkillConfigManager.h"
#include "GameStoryLayer.h"

//// 定义在MapsLayer.cpp
//extern int g_taskId;
//extern int g_gridIndex;

static float fight_gap = 0.7f;				// 攻击间隔
static float pulled_wait = 0.0f;			// 掉东西的等待时间，每死亡一个怪物，这个数据就清零，每掉一个物品，这个就被赋值
static float last_fight_wait = 0.0f;		// 最后一次攻击需要的延时

static ClientFightLayer* g_current_instance = NULL;

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#if SHOW_FIGHT_DEBUG == 1
#define DEBUG_ENABLE
#endif
#endif

#define ChessBoard_Buttom			"config/chessboard_buttom.txt"
#define ChessBoard_Top				"config/chessboard_top.txt"
#define MAX_STEP					30		// 最多打30步
#define TAG_ITEM_RESTART			22122
#define TAG_ITEM_RETURN				22123
#define TAG_ITEM_FAILD_YES			43212	// 步数太长的失败弹框里面的Yes按钮
#define TAG_ALERT					43213	// 对话框
#define TAG_CARD_IN_BAOXIANG		43214	// 宝箱里面的卡片
#define TAG_BALCK_MASK				43215		// 用于把暴击角色分开用的
//#define TAG_TRIGLE_PAUSE_NODE		44000	// 用于计时暂停攻击trigle的Node
#define BLACK_MASK_Z_ORDER			4
#define TAG_FIGHT_BG				22125
#define STATE_CHANGE_STEP			0
#define STATE_CHANGE_FIGHT			1
#define TAG_BLOOD_VIAL_BAR_BUTTOM	1232124 // 下面队伍的血瓶条
#define TAG_BloodHeroImgCD          1232200 // 喝血后，显示头像cd时间
#define TAG_TEST_MENU				212228
#define MSG_JUMP					"jump_to_end"

#define JumpTo							"JumpToWhenEnd"
#define JumpToDefault					"Default"
#define JumpToMainPageFromInitGuide		"FirstInitGuideToMainPage"
#define JumpToFightEndLayer				"FightEndLayer"
#define JumpToJingJiSuiJi				"JingJi_2"
#define JumpToJingJiFree				"JingJi_Free"
#define JumpToJingJiKaiBaoXiang			"JingJi_KaiBaoXiang"
#define JumpToShouCheng					"ShouCheng"
#define JumpToPaiHangBang				"PaiHangBang"
#define JumpToDianFengDuiJueList		"DianFengDuiJueList"
#define JumpToJiXianShouCheng			"JiXianShouCheng"
#define JumpToChuanQiLu					"ChuanQiLu"
#define JumpToXueSheJiaoTang			"XueSeJiangTang"
#define JumpToMapsLayer					"MapsLayer"





static void getGridPosByIndex(int index,int& out_x_index,int& out_y_index)
{
	out_x_index = index % ChessBoard_X_Num;
	out_y_index = index / ChessBoard_X_Num;
}
static int getIndexPos(int x_index,int y_index)
{
	return x_index + y_index * ChessBoard_X_Num;
}
static float chessBoardOffsetY = 0;

// 
// 
class ZhuangTaiBar : public BaseSprite
{
private:
	int mDefaultBloodNum;
	int mCurrentBloodNum;
	CCArray* mBloodItemArray;

	int wuPinItemNum;
	int baoshiNum;
	int cailiaoNum;
	int equipNum;
	int currentFightSection;
	int totalFight;
	CCLabelTTF* lableItemNum;
	CCLabelTTF* lableEquipNum;
//	CCLabelTTF* lableFightNum;
	CCLabelTTF* mLabelHuiHe;

	BaseSprite* baoShiSection;
	BaseSprite* caiLiaoSection;
	BaseSprite* zhuangBeiSection;

	CCMenuItemImage* jump_item;
	CCLabelAtlas *m_JumpCDTimeLabel;
	int m_JumpNeedWaitTime;

	void jump_clicked(CCObject* sender)
	{
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_JUMP,NULL);
		//jump_item->setEnabled(false);
	}
public:
#define		TAG_GETTED_IMAGE	112233
	ZhuangTaiBar(int defaultBloodNum,int currentBloodNum,int fightTotalNum)
	{
		initWithFile("zhandou_zhuangtai.png");
		// 
		{
			string font_name = fontStr_kaiti;
			float font_size = 25;
			lableItemNum = LF::lable(this,"0",ccp(68,30),font_size,font_name);
			lableEquipNum = LF::lable(this,"0",ccp(176,30),font_size,font_name);
//			lableFightNum = LF::lable(this,"0",ccp(getContentSize().width/2,140),font_size,font_name);
//			lableFightNum->setAnchorPoint(ccp(0.5f,0.5f));

			lableItemNum->setVisible(false);
			lableEquipNum->setVisible(false);

			this->totalFight = fightTotalNum;
			this->wuPinItemNum = 0;
			this->baoshiNum = 0;
			this->cailiaoNum = 0;
			this->equipNum = 0;
			this->currentFightSection = 0;


			baoShiSection = BaseSprite::create("zhandou_weihuode_baoshi.png");
			caiLiaoSection = BaseSprite::create("zhandou_weihuode_cailiao.png");
			zhuangBeiSection = BaseSprite::create("zhandou_weihuode_zhuangbei.png");
			BaseSprite* baoshiGetted = BaseSprite::create("zhandou_huode_baoshi.png");
			BaseSprite* caiLiaoGetted = BaseSprite::create("zhandou_huode_cailiao.png");
			BaseSprite* zhuangBeiGetted = BaseSprite::create("zhandou_huode_zhuangbei.png");
			baoShiSection->addChild(baoshiGetted,0,TAG_GETTED_IMAGE);
			caiLiaoSection->addChild(caiLiaoGetted,0,TAG_GETTED_IMAGE);
			zhuangBeiSection->addChild(zhuangBeiGetted,0,TAG_GETTED_IMAGE);
			baoshiGetted->setVisible(false);
			caiLiaoGetted->setVisible(false);
			zhuangBeiGetted->setVisible(false);
			baoshiGetted->setPosition(ccp(baoShiSection->getContentSize().width/2,baoShiSection->getContentSize().height/2));
			caiLiaoGetted->setPosition(ccp(caiLiaoSection->getContentSize().width/2,caiLiaoSection->getContentSize().height/2));
			zhuangBeiGetted->setPosition(ccp(zhuangBeiSection->getContentSize().width/2,zhuangBeiSection->getContentSize().height/2));

			BaseSprite* sectionMask = BaseSprite::create("zhandou_fenge.png");
			addChild(baoShiSection);
			addChild(caiLiaoSection);
			addChild(zhuangBeiSection);
			addChild(sectionMask);

			CCPoint posLeft = ccp(71,65);
			baoShiSection->setPosition(posLeft);
			caiLiaoSection->setPosition(posLeft);
			zhuangBeiSection->setPosition(posLeft);
			sectionMask->setPosition(posLeft);

			jump_item = CCMenuItemImage::create(
				"zhandou_tiaoguo_buke.png",
				"zhandou_tiaoguo_select.png",
				"zhandou_tiaoguo_buke.png",
				this,SEL_MenuHandler(&ZhuangTaiBar::jump_clicked));
			jump_item->setEnabled(false);

			CCMenu* menu = CCMenu::create(jump_item,NULL);
			addChild(menu);
			menu->setPosition(ccp(568,65));
			{
				m_JumpCDTimeLabel = CCLabelAtlas::create(
					"0123456789",
					"bloodNum_shoushang_baoji.png",
					60, 94,//71
					'0');
				m_JumpCDTimeLabel->setString("0");
				m_JumpCDTimeLabel->setAnchorPoint(ccp(0.5,0.5));
				addChild(m_JumpCDTimeLabel,1);
				m_JumpCDTimeLabel->setPosition(menu->getPosition());
				m_JumpCDTimeLabel->setVisible(false);
				m_JumpCDTimeLabel->setScale(0.65f);
			}
		}

		//回合
		{
			BaseSprite *board = BaseSprite::create("zhandou_huiheshu_bg.png");
			addChild(board, 100);
			board->setPosition(ccp(320, 127));
			mLabelHuiHe = CCLabelTTF::create(
				CCString::createWithFormat("%s%d/%d",LFStrings::getValue("HuiHe").c_str(), 1, MAX_STEP)->getCString(),
				fontStr_kaiti, 24);
			board->addChild(mLabelHuiHe);
			mLabelHuiHe->setPosition(ccp(75, 26));
		}


		// 血瓶bar
		{
			mDefaultBloodNum = defaultBloodNum;
			mCurrentBloodNum = currentBloodNum;
			CCAssert(mDefaultBloodNum >= mCurrentBloodNum,"");

			mBloodItemArray = CCArray::createWithCapacity(defaultBloodNum);
			mBloodItemArray->retain();

			float posx = 173;
			float posy = 60;
			const float _distance = 27.5f;

			for (int i=0;i<defaultBloodNum;i++)
			{
				BaseSprite* item = BaseSprite::create("zhandou_yaopin.png");
				posx += item->getContentSize().width/2;
				item->setPosition(ccp(posx,posy));
				posx += item->getContentSize().width/2;
				posx += _distance;

				BaseSprite* itemBg = BaseSprite::create("zhandou_yaopin_bg.png");
				addChild(itemBg);
				itemBg->setPosition(item->getPosition());
				addChild(item);

				mBloodItemArray->addObject(item);
			}
			//{
			//	// 用shader的blood，盖在现在的血瓶上，做一个流光效果，然后消失
			//	vector<CCTexture2D*> opTexVec;
			//	vector<string> opTexSamlpeNameVec;
			//	string v_shader_file = "xueping.vsh";
			//	string f_shader_file = "xueping.fsh";
			//	opTexSamlpeNameVec.push_back("CC_Texture0");
			//	opTexSamlpeNameVec.push_back("flow_texture");
			//	opTexVec.push_back(CCTextureCache::sharedTextureCache()->addImage("zhandou_yaopin.png"));
			//	opTexVec.push_back(CCTextureCache::sharedTextureCache()->addImage("zhandou_yaopin_shuaxin.png"));
			//	TextureOpSprite* bloodCopy = new TextureOpSprite(opTexVec,opTexSamlpeNameVec,v_shader_file,f_shader_file);
			//	addChild(bloodCopy);
			//	bloodCopy->setPosition(ccp(200,100));
			//	bloodCopy->release();
			//}
		}
		

		refreshBlood(false);
	}
	CCLabelTTF *getHuiHeLabel(){return mLabelHuiHe;}
	void setCurrentBloodNum(int num)
	{
		CCAssert(mDefaultBloodNum >= num,"");
		CCAssert(num >= 0,"");
		mCurrentBloodNum = num;
	}
	int getCurrentBloodNum(){return mCurrentBloodNum;};
	BaseSprite* getBloodByIndex(int index)
	{
		if (index < 0 || index > mDefaultBloodNum-1)
		{
			return NULL;
		}
		return (BaseSprite*)mBloodItemArray->objectAtIndex(index);
	}

	void refreshBlood(bool annimation = true)
	{
		for (int i=0;i<mDefaultBloodNum;i++)
		{
			if (i >= mCurrentBloodNum)
			{
				((BaseSprite*)mBloodItemArray->objectAtIndex(i))->setColor(ccGRAY);
				if (annimation)
				{
					((BaseSprite*)mBloodItemArray->objectAtIndex(i))->runAction(CCFadeTo::create(0.2f,50));//setOpacity(50);
				}else
				{
					((BaseSprite*)mBloodItemArray->objectAtIndex(i))->setOpacity(50);//setOpacity(50);
				}
			}
		}
	}
	// 设置是否可点击
	void setJumpItemEnable(bool enbale)
	{
		jump_item->setEnabled(enbale);
	}
	void addJumpDirectionMenuItem()
	{
		jump_item->initWithNormalImage(
			"zhandou_tiaoguo_buke.png",
			"zhandou_tiaoguo_select.png",
			"zhandou_tiaoguo_buke.png",
			this,SEL_MenuHandler(&ZhuangTaiBar::jumpDirectionClicked));
		jump_item->setEnabled(true);
// 		CCMenuItemImage *jumpMaskItem = CCMenuItemImage::create(
// 			"zhandou_tiaoguo_buke.png",
// 			"zhandou_tiaoguo_select.png",
// 			"zhandou_tiaoguo_buke.png",
// 			this,SEL_MenuHandler(&ZhuangTaiBar::jumpDirectionClicked));
// 		jumpMaskItem->setOpacity(0);
// 		jump_item->getParent()->addChild(jumpMaskItem);
// 		jumpMaskItem->setPosition(jump_item->getPosition());
	}
	void jumpDirectionClicked(CCObject *pSender)
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("TiaoGuo_ZhanKuangBuMing"), false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1000);
	}
	void setJumpCDTime(int time)
	{
		if (time == 0){
			jump_item->initWithNormalImage(
				"zhandou_tiaoguo.png",
				"zhandou_tiaoguo_select.png",
				NULL,
				this,SEL_MenuHandler(&ZhuangTaiBar::jump_clicked));
			jump_item->setEnabled(false);
			return;
		}
		else {
			m_JumpNeedWaitTime = time;
			m_JumpCDTimeLabel->setString(CCString::createWithFormat("%d", m_JumpNeedWaitTime)->getCString());
			m_JumpCDTimeLabel->setVisible(true);

			jump_item->initWithNormalImage(
				"zhandou_tiaoguo_wuwenzi.png",
				"zhandou_tiaoguo_select.png",
				NULL,
				this,SEL_MenuHandler(&ZhuangTaiBar::jump_clicked));
			jump_item->setEnabled(false);

			schedule(schedule_selector(ZhuangTaiBar::UpdateJumpCDTime), 1);
		}
	}
	void UpdateJumpCDTime(float dt)
	{
		m_JumpCDTimeLabel->setString(CCString::createWithFormat("%d", m_JumpNeedWaitTime)->getCString());
		m_JumpNeedWaitTime -= 1;
		if (m_JumpNeedWaitTime <= 0){
			unschedule(schedule_selector(ZhuangTaiBar::UpdateJumpCDTime));
		}
	}
	void setJumpCDTimeOk()
	{
		m_JumpNeedWaitTime = 0;
		m_JumpCDTimeLabel->setString(CCString::createWithFormat("0", m_JumpNeedWaitTime)->getCString());
		m_JumpCDTimeLabel->runAction(CCSequence::create(CCFadeOut::create(0.25f), CCHide::create(), NULL));
		GLubyte op = jump_item->getOpacity();
		jump_item->initWithNormalImage(
			"zhandou_tiaoguo.png",
			"zhandou_tiaoguo_select.png",
			"zhandou_tiaoguo_buke.png",
			this,SEL_MenuHandler(&ZhuangTaiBar::jump_clicked));
		jump_item->setOpacity(op);
		setJumpItemEnable(true);
	}

	void addOneItem(ItemsType itemType)
	{
		wuPinItemNum ++;
		if (itemType > ItemsType_Baoshi_min && itemType < ItemsType_Baoshi_max)
		{
			baoshiNum++;
			baoShiSection->getChildByTag(TAG_GETTED_IMAGE)->setVisible(true);
		}
		else if (
			itemType == ItemsType_QianghuaNormal || 
			itemType == ItemsType_QianghuaXiyou ||
			itemType == ItemsType_QianghuaChuangqi ||
			itemType == ItemsType_QianghuaGailvTianjia)
		{
			cailiaoNum++;
			caiLiaoSection->getChildByTag(TAG_GETTED_IMAGE)->setVisible(true);
		}
		lableItemNum->setString(CCString::createWithFormat("%d",wuPinItemNum)->getCString());
	}
	// 
	void addOneEquip()
	{
		equipNum ++;
		lableEquipNum->setString(CCString::createWithFormat("%d",equipNum)->getCString());
		zhuangBeiSection->getChildByTag(TAG_GETTED_IMAGE)->setVisible(true);
	}

	void setFightIndex(int index)
	{
		currentFightSection = index + 1;
// 		lableFightNum->setString(CCString::createWithFormat("%s:%d/%d",
// 			LF::getString("huihe").c_str(),
// 			currentFightSection,
// 			totalFight)->getCString());
		//if (totalFight <= 1) lableFightNum->setVisible(false);
	}


	~ZhuangTaiBar()
	{
		mBloodItemArray->release();
	}
};

ClientFightLayer::ClientFightLayer(vector<FightDirector> dir_vec,CCSprite* bg,Bundle bundle /* = Bundle */)
{
	mChatTag = Chat_NO;
	mTrigle = NULL;
//	huiHeLable = NULL;
	//isInGuide = false;
	this->mBundle = bundle;
	initWithDirctorVecAndBg(dir_vec,bg);
	g_current_instance = this;
	addObserver(SEL_CallFuncO(&ClientFightLayer::jumpItemClicked),MSG_JUMP);
	addObserver(SEL_CallFuncO(&ClientFightLayer::useSkillEnd),MSGFight_Skill_UseEnd);
}
ClientFightLayer::~ClientFightLayer(void)
{
	delete mSkillProc;
	removeObserver(MSG_ItemDead);
	removeObserver(MSGFight_Skill_UseEnd);
}


void ClientFightLayer::setBeforeFightPro(HeroBaseProperty data[])
{
	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		oldArmy[i] = data[i];
	}
}

struct Temp_Id_Index
{
	int uniqueId;
	int dirIndex;
};

void ClientFightLayer::initWithDirctorVecAndBg(vector<FightDirector> dir_vec,CCSprite* bg)
{
	CCAssert(dir_vec.size() > 0,"必须要有一个导演");
	for (int i = 0;i<dir_vec.size();i++)
	{
		if (!dir_vec[i].isInited())
		{
			CCAssert(false,"必须是一个准备好脚本的导演");
			return;
		}
	}

	mPauseFightTrigle = false;
	mBg = bg;
	mBg->setPosition(ccp(mVisibleSize.width/2,mVisibleSize.height - mBg->getContentSize().height/2));
	addChild(mBg,-1,TAG_FIGHT_BG);

	mCurrentStepIndex = 0;
	currentDirIndex = 0;
	boardTop = NULL;
	boardButom = NULL;
	mSkillProc = NULL;
	itemNext = NULL;

	if (mBundle.getString(JumpTo,"") == JumpToFightEndLayer)
	{
		for (int i=0;i<FormationHeroAcountMax;i++)
		{
			int _heroId = dir_vec[0].getScript().init.myTeam[i].heroId;
			if (_heroId == INVALID_ID)
			{
				HeroBaseProperty pro_null;
				pro_null.hero_id = INVALID_ID;
				nowArmy[i] = pro_null;
				continue;
			}
			nowArmy[i] = MyselfManager::getManager()->getMyTeamActor(_heroId)->getCurrentProperty();
		}
	}

	mDirvec = dir_vec;

	rewardVec.clear();
	chessBoardOffsetY = 0;

	//20140603 1-4铁匠的锤子
	{
		mHammerIdx = -1;
		mSurvivalSpriteNum = 0;
		for (int i=0; i<FormationHeroAcountMax; ++i){
			if (mDirvec.at(0).getScript().init.oppositeTeam[i].heroId != 0)
				++mSurvivalSpriteNum;//someOneDead 调用一次就 --mSurvivalSpriteNum
		}
	}
	addObserver(SEL_CallFuncO(&ClientFightLayer::someOneDead),MSG_ItemDead);
	addObserver(SEL_CallFuncO(&ClientFightLayer::fightGuideEnd),MSG_FG_END);

	// 分布获得的物品和装备的位置
	{
		// 分离出需要显示掉落的装备，物品
		vector<EquipData> gettedEquips;
		vector<ItemInfo> gettedItems;
		vector<ItemSuipianInfo> gettedItemSuiPians;
		vector<ShenbingSuipianInfo> gettedShenBingSuiPians;
		vector<PowerPoint> gettedPPT;
		for (int i = 0;i<dir_vec.size();i++)
		{
			FightEnd end = dir_vec[i].getScript().endResult;
			int item_num = end.gettedItem.size();
			for (int j=0;j<item_num;j++)
			{
				// 银币不算
				if (end.gettedItem[j].type == ItemsType_Silver)
					continue;

				gettedItems.push_back(end.gettedItem[j]);
			}
			if (end.gettedZhuangBei.size() > 0)
			{
				gettedEquips.insert(gettedEquips.end(),end.gettedZhuangBei.begin(),end.gettedZhuangBei.end());
			}
			if (end.gettedItemSuiPian.size() > 0)
			{
				gettedItemSuiPians.insert(gettedItemSuiPians.end(),end.gettedItemSuiPian.begin(),end.gettedItemSuiPian.end());
			}
			if (end.gettedShenBingSuiPian.size() > 0)
			{
				gettedShenBingSuiPians.insert(gettedShenBingSuiPians.end(),end.gettedShenBingSuiPian.begin(),end.gettedShenBingSuiPian.end());
			}
			if (end.gettedPowerPoints.size() > 0)
			{
				gettedPPT.insert(gettedPPT.end(),end.gettedPowerPoints.begin(),end.gettedPowerPoints.end());
			}
		}

		// 抽取出所有的怪物uniqueId,改成了最后一波掉装
		//int dirNum = dir_vec.size();
		//list<Temp_Id_Index> allTopUniqueIdArray;					// 所有敌人的的id，是3场的Id
		//for (int i = 0; i< dirNum;i++)
		//{
		//	for (int j=0;j<FormationHeroAcountMax;j++)
		//	{
		//		if (dir_vec[i].getChessBoardInitData().oppositeTeam[j].heroId > 0)
		//		{
		//			Temp_Id_Index one;
		//			one.uniqueId = dir_vec[i].getChessBoardInitData().oppositeUniqueId[j];
		//			one.dirIndex = i;
		//			allTopUniqueIdArray.push_back(one);
		//		}
		//	}
		//}


		// 抽离出最后一场战斗的怪物的id
		list<Temp_Id_Index> allTopUniqueIdArray;					// 所有敌人的的id，是3场的Id
		for (int j=0;j<FormationHeroAcountMax;j++)
		{
			int index = dir_vec.size()-1;
			if (dir_vec[index].getChessBoardInitData().oppositeTeam[j].heroId > 0)
			{
				Temp_Id_Index one;
				one.uniqueId = dir_vec[index].getChessBoardInitData().oppositeUniqueId[j];
				one.dirIndex = index;
				allTopUniqueIdArray.push_back(one);
			}
		}


		// 用于可以领取掉落的上面阵型的actor
		list<Temp_Id_Index> allTopUniqueIdArray_ToPull;

		// 分布Item
		for (int i=0;i<gettedItems.size();i++)
		{
			if (allTopUniqueIdArray_ToPull.empty())
			{// 没有了就从全部集合里面重新开始找
				allTopUniqueIdArray_ToPull = allTopUniqueIdArray;
			}

			int index = CCRANDOM_0_1() * allTopUniqueIdArray_ToPull.size();
			list<Temp_Id_Index>::iterator it = allTopUniqueIdArray_ToPull.begin();
			for (;it != allTopUniqueIdArray_ToPull.end();it++)
			{
				if (index == 0)
				{
					FightReward reward;
					reward.rewardType = TYPE_ITEM;
					reward.deadUniqueId = (*it).uniqueId;
					reward.dirIndex =  (*it).dirIndex;
					reward.item = gettedItems[i];
					//
					rewardVec.push_back(reward);

					allTopUniqueIdArray_ToPull.erase(it);

					break;
				}
				index--;
			}
		}

		// 分布equip
		for (int i=0;i<gettedEquips.size();i++)
		{
			if (allTopUniqueIdArray_ToPull.empty())
			{// 没有了就从全部集合里面重新开始找
				allTopUniqueIdArray_ToPull = allTopUniqueIdArray;
			}

			int index = CCRANDOM_0_1() * allTopUniqueIdArray_ToPull.size();
			list<Temp_Id_Index>::iterator it = allTopUniqueIdArray_ToPull.begin();
			for (;it != allTopUniqueIdArray_ToPull.end();it++)
			{
				if (index == 0)
				{
					FightReward reward;
					reward.rewardType = TYPE_EQUIP;
					reward.deadUniqueId = (*it).uniqueId;
					reward.dirIndex =  (*it).dirIndex;
					reward.equip = gettedEquips[i];
					//
					rewardVec.push_back(reward);

					allTopUniqueIdArray_ToPull.erase(it);

					break;
				}
				index--;
			}
		}

		// 分布Item碎片
		for (int i=0;i<gettedItemSuiPians.size();i++)
		{
			if (allTopUniqueIdArray_ToPull.empty())
			{// 没有了就从全部集合里面重新开始找
				allTopUniqueIdArray_ToPull = allTopUniqueIdArray;
			}

			int index = CCRANDOM_0_1() * allTopUniqueIdArray_ToPull.size();
			list<Temp_Id_Index>::iterator it = allTopUniqueIdArray_ToPull.begin();
			for (;it != allTopUniqueIdArray_ToPull.end();it++)
			{
				if (index == 0)
				{
					FightReward reward;
					reward.rewardType = TYPE_ITEM_SUIPIAN;
					reward.deadUniqueId = (*it).uniqueId;
					reward.dirIndex =  (*it).dirIndex;
					reward.itemSuiPian = gettedItemSuiPians[i];
					//
					rewardVec.push_back(reward);

					allTopUniqueIdArray_ToPull.erase(it);

					break;
				}
				index--;
			}
		}

		// 分布神兵碎片
		for (int i=0;i<gettedShenBingSuiPians.size();i++)
		{
			if (allTopUniqueIdArray_ToPull.empty())
			{// 没有了就从全部集合里面重新开始找
				allTopUniqueIdArray_ToPull = allTopUniqueIdArray;
			}

			int index = CCRANDOM_0_1() * allTopUniqueIdArray_ToPull.size();
			list<Temp_Id_Index>::iterator it = allTopUniqueIdArray_ToPull.begin();
			for (;it != allTopUniqueIdArray_ToPull.end();it++)
			{
				if (index == 0)
				{
					FightReward reward;
					reward.rewardType = TYPE_SHENBING_SUIPIAN;
					reward.deadUniqueId = (*it).uniqueId;
					reward.dirIndex =  (*it).dirIndex;
					reward.shenBingSuiPian = gettedShenBingSuiPians[i];
					//
					rewardVec.push_back(reward);

					allTopUniqueIdArray_ToPull.erase(it);

					break;
				}
				index--;
			}
		}

		// 分布PPT
		for (int i=0;i<gettedPPT.size();i++)
		{
			if (allTopUniqueIdArray_ToPull.empty())
			{// 没有了就从全部集合里面重新开始找
				allTopUniqueIdArray_ToPull = allTopUniqueIdArray;
			}

			int index = CCRANDOM_0_1() * allTopUniqueIdArray_ToPull.size();
			list<Temp_Id_Index>::iterator it = allTopUniqueIdArray_ToPull.begin();
			for (;it != allTopUniqueIdArray_ToPull.end();it++)
			{
				if (index == 0)
				{
					FightReward reward;
					reward.rewardType = TYPE_PPT;
					reward.deadUniqueId = (*it).uniqueId;
					reward.dirIndex =  (*it).dirIndex;
					reward.pptData = gettedPPT[i];
					//
					rewardVec.push_back(reward);

					allTopUniqueIdArray_ToPull.erase(it);

					break;
				}
				index--;
			}
		}

	}

	//hammer test
// 	FightReward reward;
// 	reward.rewardType = TYPE_ITEM;
// 	reward.deadUniqueId = 5;
// 	reward.dirIndex =  0;
// 	reward.item.type = ItemsType_Hammer;
// 	rewardVec.push_back(reward);


	playWithDirctor(0);
}

void ClientFightLayer::fu_huo_action(BaseSprite* container,string pic)
{
	const float fuwenFrameDelayTime = 0.0625f;
	//   fuwen
	CCArray *aniArray = new CCArray(25);
	for(int i=0; i!=25; ++i)
	{
		char buf[60] = "";
		sprintf(buf, "zhandou_fuhuo_fuzhou_%d.png", i+1);
		CCSpriteFrame *fuzhou = CCSpriteFrame::create(buf, CCRect(0,0,424,460));
		aniArray->addObject(fuzhou);
	}
	CCAnimation* animation = CCAnimation::createWithSpriteFrames(aniArray, fuwenFrameDelayTime);

	CCSprite *fuwen = CCSprite::createWithSpriteFrame((CCSpriteFrame*)aniArray->objectAtIndex(0));
	container->addChild(fuwen, 1);
	fuwen->setPosition(ccp(mWinSize.width/3.0, mWinSize.height/5.0));
	fuwen->runAction(CCSequence::create(
		CCAnimate::create(animation)
		, CCCallFuncN::create(this, SEL_CallFuncN(&BaseSprite::removeNode))
		, NULL));

	CC_SAFE_DELETE(aniArray);


	//    renwu
	//人物类型   test! 野蛮人 双手
	CCSprite *hero = CCSprite::create(pic.c_str());
	container->addChild(hero, fuwen->getZOrder()+2);
	hero->setPosition(fuwen->getPosition());
	hero->setOpacity(0);
	hero->runAction(CCSequence::create(
		CCDelayTime::create(fuwenFrameDelayTime*20)
		, CCSpawn::create(CCFadeTo::create(fuwenFrameDelayTime*5, 255), NULL)
		, CCSpawn::create(CCScaleTo::create(fuwenFrameDelayTime*4, 1.06), NULL)
		, CCDelayTime::create(fuwenFrameDelayTime*6)
		, CCSpawn::create(CCScaleTo::create(fuwenFrameDelayTime, 1.0), NULL)
		, CCCallFuncN::create(this, SEL_CallFuncN(&BaseSprite::removeNode))
		, NULL));


	//  guangxiao   
	CCSprite* guangxiao[4];
	for(int i=0; i!=4; ++i)
	{
		char buf[60] = "";
		sprintf(buf, "fuhuo_guangxiao_%d.png", i+1);
		guangxiao[i] = CCSprite::create(buf);
		guangxiao[i]->setPosition(hero->getPosition());
		guangxiao[i]->setOpacity(0);
		if(i!=0)
			container->addChild(guangxiao[i], hero->getZOrder()-1);
		else
			container->addChild(guangxiao[i], hero->getZOrder()+1);
	}

	guangxiao[0]->runAction(CCSequence::create(
		CCDelayTime::create(fuwenFrameDelayTime*20)
		, CCSpawn::create(CCFadeTo::create(fuwenFrameDelayTime*5, 255), NULL)
		, CCScaleTo::create(fuwenFrameDelayTime*4, 1.1)
		, CCDelayTime::create(fuwenFrameDelayTime*3)
		, CCFadeTo::create(fuwenFrameDelayTime, 0)
		, CCCallFuncN::create(this, SEL_CallFuncN(&BaseSprite::removeNode))
		, NULL));

	guangxiao[1]->setPosition(ccp(guangxiao[1]->getPositionX()-6, guangxiao[1]->getPositionY()-4));
	guangxiao[1]->runAction(CCSequence::create(
		CCDelayTime::create(fuwenFrameDelayTime*20)
		, CCSpawn::create(CCFadeTo::create(fuwenFrameDelayTime*5, 255), NULL)
		, CCScaleTo::create(fuwenFrameDelayTime*4, 1.1)
		, CCDelayTime::create(fuwenFrameDelayTime*3)
		, CCFadeTo::create(fuwenFrameDelayTime, 0)
		, CCCallFuncN::create(this, SEL_CallFuncN(&BaseSprite::removeNode))
		, NULL));

	guangxiao[2]->setScale(0);
	guangxiao[2]->runAction(CCSequence::create(
		CCDelayTime::create(fuwenFrameDelayTime*20)
		, CCSpawn::create(CCFadeTo::create(fuwenFrameDelayTime*5, 255), CCScaleTo::create(fuwenFrameDelayTime*5, 1), NULL)
		, CCRotateBy::create(fuwenFrameDelayTime*7, 3)
		, CCFadeTo::create(fuwenFrameDelayTime, 0)
		, CCCallFuncN::create(this, SEL_CallFuncN(&BaseSprite::removeNode))
		, NULL));

	guangxiao[3]->setScale(0);
	guangxiao[3]->runAction(CCSequence::create(
		CCDelayTime::create(fuwenFrameDelayTime*20)
		, CCSpawn::create(CCFadeTo::create(fuwenFrameDelayTime*5, 255), CCScaleTo::create(fuwenFrameDelayTime*5, 1), NULL)
		, CCRotateBy::create(fuwenFrameDelayTime*7, -3)
		, CCFadeTo::create(fuwenFrameDelayTime, 0)
		, CCCallFuncN::create(this, SEL_CallFuncN(&BaseSprite::removeNode))
		, NULL));
}

void ClientFightLayer::playWithDirctor(int dirIndex)
{
	CCAssert(dirIndex >= 0 && dirIndex < mDirvec.size(),"");
	currentDirIndex = dirIndex;
	mCurrentStepIndex = 0;
	fightEnded = false;
	//pulled = false;
	mChatTag = Chat_NO;
	pulled_wait = 0.0f;
	//unschedule(schedule_selector(ClientFightLayer::fightTrgile));
	{
		if (mTrigle)
			mTrigle->removeAllChildrenWithCleanup(true);
		mTrigle = NULL;
		if (!getChildByTag(TAG_FIGHT_BG))
		{
			addChild(mBg,-1,TAG_FIGHT_BG);
			mBg->setPosition(ccp(mVisibleSize.width/2,mVisibleSize.height - mBg->getContentSize().height/2));
		}

		// 移除所有的child，除了背景
		if ( m_pChildren && m_pChildren->count() > 0 )
		{
			// 拷贝一份，不然直接循环m_pChildren移除的话有问题
			// 因为又在循环，定好索引，又在移除，会遗漏或崩溃
			CCArray* childrenCopy = CCArray::createWithCapacity(m_pChildren->count());
			childrenCopy->addObjectsFromArray(m_pChildren);

			// 循环移除
			CCObject* child;
			CCARRAY_FOREACH(childrenCopy, child)
			{
				CCNode* pNode = (CCNode*) child;
				if (pNode == mBg)
				{
					continue;
				}
				
				if (pNode)
				{
					pNode->removeFromParentAndCleanup(true);
				}
			}
		}
	}

	// 计时器，触发器
	mTrigle = new FightTrigleDriver();
	mTrigle->setTrigleGapTime(fight_gap);
	addChild(mTrigle);
	addObserver(SEL_CallFuncO(&ClientFightLayer::fightTrigle),MSG_TRIGLE_TIME);
	addObserver(SEL_CallFuncO(&ClientFightLayer::waitTimeEnd),MSG_TRIGLE_PAUSE_END);
	addObserver(SEL_CallFuncO(&ClientFightLayer::canJumpNotified),MSG_JUMP_CAN_JUMP);

// 	huiHeLable = CCLabelTTF::create("",fontStr_kaiti,30);
// 	huiHeLable->setHorizontalAlignment(kCCTextAlignmentCenter);
// 	huiHeLable->setString(CCString::createWithFormat(LF::getString("di_x_huihe").c_str(),1, MAX_STEP)->getCString());
// 	huiHeLable->setPosition(ccp(mWinSize.width/2,mWinSize.height - huiHeLable->getContentSize().height));
// 	addChild(huiHeLable,ITEM_Z_OERDER_IN_STAGE + 1);

	// 初始化棋盘
	boardButom = ClientChessBoard::create(ChessBoard_Buttom,this);
	boardTop = ClientChessBoard::create(ChessBoard_Top,this);
	// 根据当前游戏设计的透视
	// 我们下方的棋盘应该是压住上面的棋盘的
	// 所以，后添加下面的棋盘
	addChild(boardTop,1);
	addChild(boardButom,1);
	boardButom->setBoardName(BOARD_NAME_BUTTOM);
	boardTop->setBoardName(BOARD_NAME_TOP);
	boardButom->setPosition(ccp(mWinSize.width/2,252 + chessBoardOffsetY));
	boardTop->setPosition(ccp(mWinSize.width/2,mWinSize.height-252 + chessBoardOffsetY));
	boardButom->setOpacity(0);
	boardTop->setOpacity(0);
	mSkillProc = new ClientSkillProcessor(boardButom,boardTop);
	// 初始化棋盘Item
	InitChessBoard initData = mDirvec[currentDirIndex].getChessBoardInitData();
	boardButom->setCanUsedBloodNum(initData.myCurBloodNum);
	boardButom->setDefaultBloodNum(initData.myDefaultBloodNum);
	boardTop->setCanUsedBloodNum(initData.oppositeCurBloodNum);
	boardTop->setDefaultBloodNum(initData.oppositeDefaultBloodNum);

	
	string intent = mBundle.getString(JumpTo,JumpToDefault);
	bool isInMainTask = (intent == JumpToFightEndLayer);
	bool isFirstInit = (intent == JumpToMainPageFromInitGuide);

	// 下面的状态条
	TaskManager* mapManger = TaskManager::getManager();
//	int fightNum = isInMainTask?ChessBoardStateManager::getManager()->getSectionTotalFightNum(g_taskId,g_gridIndex):1;
	int fightNum = isInMainTask?ChessBoardStateManager::getManager()->getSectionTotalFightNum(
		mapManger->getCurrentFightTaskId(), mapManger->getCurrentFightGridIdx()):1;
	if (isFirstInit)
		fightNum = 2; // 2场

	ZhuangTaiBar* buttomBloodBar = new ZhuangTaiBar(boardButom->getDefaultBloodNum(),boardButom->getCurrentBloodNum(),fightNum);
	{
		addChild(buttomBloodBar,20,TAG_BLOOD_VIAL_BAR_BUTTOM);
		buttomBloodBar->setPosition(ccp(mVisibleSize.width/2,buttomBloodBar->getContentSize().height/2));
		buttomBloodBar->release();
		buttomBloodBar->setFightIndex(dirIndex);
		buttomBloodBar->setJumpItemEnable(false);


		FightContext context;
		context.waitTime = 5;
		/*1.PVE战斗：（9宫格小怪）首次战斗不可跳过，之后战斗10秒后可跳过
					 （BOSS怪）恒定不可跳过           
					 （极限守城）战斗5秒后可跳过       
					 （血色教堂）战斗5秒后可跳过                                    
		  2.PVP战斗： 恒定为5秒可跳过
		*/
		if (mBundle.getBool("testMode",false))
		{
			context.battleType = BattleType2_TestMode;
		}
		else if (isInMainTask || intent == JumpToDefault || intent == JumpToFightEndLayer)
		{
// 			context.gridId = g_gridIndex;
// 			context.taskId = g_taskId;
			context.waitTime = MIN_WAIT_BEFOER_JUMP;
			context.gridId = TaskManager::getManager()->getCurrentFightGridIdx();
			context.taskId = TaskManager::getManager()->getCurrentFightTaskId();
			context.battleType = BattleType2_MainTask;
		}
		else if(isFirstInit)
		{
			context.battleType = BattleType2_InitGuide;
		}
		else if(intent == JumpToJingJiSuiJi || intent == JumpToJingJiFree || intent == JumpToJingJiKaiBaoXiang)
		{
			context.battleType = BattleType2_Arena;
		}
		else if(intent == JumpToShouCheng || intent == JumpToJiXianShouCheng)
		{
			context.battleType = BattleType2_WorldBoss;
		}
		else if(intent == JumpToPaiHangBang)
		{
			context.battleType = BattleType2_TopHeroChallenge;
		}
		else if(intent == JumpToDianFengDuiJueList)
		{
			context.battleType = BattleType2_TopArenaPlayBack;
		}
		else if(intent == JumpToChuanQiLu)
		{
			context.battleType = BattleType2_ChuanQiLu;
		}
		else if(intent == JumpToXueSheJiaoTang)
		{
			context.battleType = BattleType2_XueSeJiaoTang;
		}
		else if(intent == JumpToMapsLayer)
		{
			context.battleType = BattleType2_SHZL;
		}
		else
		{
			CCAssert(false,"未知的战斗类型");
		}

		JumpHolder::getHolder()->setFightContext(context);
		if (JumpHolder::getHolder()->canJump(true)){
			buttomBloodBar->setJumpCDTime(context.waitTime);
		}
		else if (TaskManager::getManager()->getLastActivedMainTaskId() == TaskManager::getManager()->getCurrentFightTaskId()
			&& !ChessBoardStateManager::getManager()->isBossSection(TaskManager::getManager()->getCurrentFightTaskId(),TaskManager::getManager()->getCurrentFightGridIdx())){
			//（9宫格小怪）首次战斗不可跳过
			buttomBloodBar->addJumpDirectionMenuItem();	
		}
	}
	

	if (mBundle.getBool("setFightBarVisible",true))
	{
		chessBoardOffsetY = buttomBloodBar->getContentSize().height * 0.5f;
	}else
	{
		chessBoardOffsetY = 0;
		buttomBloodBar->setVisible(false);
	}
	// 重新设置下位置，top的始终在这个位置
	boardButom->setPosition(ccp(mWinSize.width/2,252 + chessBoardOffsetY));
	boardTop->setPosition(ccp(mWinSize.width/2,mWinSize.height-252));


	// 敌方/上方，根据透视情况，需要1,2压住3,4
	for (int i=FormationHeroAcountMax-1;i>=0;i--)
	{
		int posX = i % boardTop->getChessBoard_X_Num();
		int posY = i / boardTop->getChessBoard_X_Num();

		if (initData.oppositeUniqueId[i] > 0)
		{
			ClientChessBoardItem* item = new ClientChessBoardItem(
				boardTop,
				initData.oppositeTeam[i],
				initData.oppositeUniqueId[i]);
			boardTop->addActor(item,posX,posY);
			item->release();

			item->setFightEnviroment(isInMainTask?FIGHT_EVN_MAIN_LINE:FIGHT_EVN_OTHER);
		}
	}

	// 我方/下方,根据透视情况，需要3,4压住1,2
	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		int posX = i % boardButom->getChessBoard_X_Num();
		int posY = i / boardButom->getChessBoard_X_Num();
		if (initData.myTeamUniqueId[i] > 0)
		{
			ClientChessBoardItem* item = new ClientChessBoardItem(
				boardButom,initData.myTeam[i],
				initData.myTeamUniqueId[i]);

			boardButom->addActor(item,posX,posY);
			item->release();
			item->setFightEnviroment(isInMainTask?FIGHT_EVN_MAIN_LINE:FIGHT_EVN_OTHER);
		}
	}

	
	Music::sharedMusicPlayer()->playPlayMusic();

	if (isFirstInit)
	{// 第一次的初始战斗，这里不弹出对话，而是怪物掉下来才开始对话
		// 敌人从天而降
		actorFallDownFromSky(boardTop);
	}
	else if (isInMainTask && TaskManager::getManager()->getLastActivedMainTaskId() == TaskManager::getManager()->getCurrentFightTaskId())
	{// 主线任务
		vector<ChatItem> chat;
		{
			// 刚开始进入任务时
			char tempJustInTask[64];
			sprintf(tempJustInTask,"duihua_%d_%d_showed",TaskManager::getManager()->getCurrentFightTaskId(),2); 
			bool showedJustInTask = false;
			History::getHistory()->getBoolFromCurrentHero(tempJustInTask,showedJustInTask);

			// BOSS关卡开始战斗前
//			bool isBossSection = ChessBoardStateManager::getManager()->isBossSection(g_taskId,g_gridIndex);
			TaskManager* mapManager = TaskManager::getManager();
			bool isBossSection = ChessBoardStateManager::getManager()->isBossSection(mapManager->getCurrentFightTaskId(),mapManager->getCurrentFightGridIdx());
			char tempBossStart[64];
			sprintf(tempBossStart,"duihua_%d_%d_showed",TaskManager::getManager()->getCurrentFightTaskId(),3); 
			bool showedBossStart = false;
			if (isBossSection)
			{
				History::getHistory()->getBoolFromCurrentHero(tempBossStart,showedBossStart);
			}

			if (!showedJustInTask)
			{// 把刚进任务的对话拿出来
				vector<ChatItem> temp = ChatContentManager::sharedChatContentManager()->getMainTaskChatContentById(TaskManager::getManager()->getCurrentFightTaskId(),2);
				if (temp.size() > 0)
				{
					chat.insert(chat.end(),temp.begin(),temp.end());
					// 设置显示了
					History::getHistory()->putInCurrentHero(tempJustInTask,true);
				}
			}
			if (isBossSection && !showedBossStart)
			{// 把Boss刚开始的对话拿出来
				vector<ChatItem> temp = ChatContentManager::sharedChatContentManager()->getMainTaskChatContentById(TaskManager::getManager()->getCurrentFightTaskId(),3);
				if (temp.size() > 0)
				{
					chat.insert(chat.end(),temp.begin(),temp.end());
					// 设置显示了
					History::getHistory()->putInCurrentHero(tempBossStart,true);
				}
				
			}
		}

		if(chat.size() > 0)
		{
			mChatTag = Chat_WhenHeroOK;
			ChatLayer* chatLayer = ChatLayer::create(chat,this);
			addChild(chatLayer,1024);
			// 先隐藏，在callback后再显示，并从天上掉落
			boardTop->setAllActorVisible(false);
		}else
		{
			// 敌人从天而降
			actorFallDownFromSky(boardTop);
		}
	}else
	{
		// 敌人从天而降
		actorFallDownFromSky(boardTop);
	}
}

void ClientFightLayer::chatEndCallBack()
{
	CCScene* p_scene = CCDirector::sharedDirector()->getRunningScene();
	CCLayer* p_layer = 0;
	unsigned int taskId = TaskManager::getManager()->getCurrentFightTaskId();
	switch (mChatTag)
	{
	case Chat_WhenHeroOK:
		if (taskId == 19)
		{
			p_layer = ChapterIllustrationLayer::create(TaskManager::getManager()->getCurrentFightTaskId(), this, 
				callfunc_selector(ClientFightLayer::ChapterIllustrationEndCallBack));
			p_scene->addChild(p_layer, this->getZOrder() + 1000);
			this->setVisible(false);
		}
		else
		{
			boardTop->setAllActorVisible(true);
			// 敌人从天而降
			actorFallDownFromSky(boardTop);
		}
		break;
	case Chat_WhenGuaiWuOK:
		// 如果是第一次引导的话，还需要播放介绍的guide
		if (mBundle.getString(JumpTo,"") == JumpToMainPageFromInitGuide && currentDirIndex == 0) 
		{
			FightGuideFactory::InitGuideConfig config;
			config.toAdd = this;

			ClientChessBoardItem* mainItem = boardButom->getActorItem(0,0); // 获取主角的位置
			CCAssert(mainItem!=NULL,"Main Hero is Not In Pos 0-0");
			ClientChessBoardItem* guaiwuItem = boardTop->getActorItem(0,0);	// 获取怪物位置
			CCAssert(guaiwuItem!=NULL,"Guaiwu is Not In Pos 0-0");

			CCPoint heroPos = LF::getWorldPostion(mainItem->getJiaZi());//->convertToWorldSpace(CCPointZero);
			CCSize heroSize = mainItem->getJiaZi()->getContentSize();

			//CCPoint guaiwuPos = guaiwuItem->getJiaZi()->convertToWorldSpace(CCPointZero);
			CCPoint guaiwuPos = LF::getWorldPostion(guaiwuItem->getJiaZi());
			CCSize guaiwuSize = guaiwuItem->getJiaZi()->getContentSize();

			config.rectGlPosForHero = CCRectMake(heroPos.x - heroSize.width/2,heroPos.y - heroSize.height/2,heroSize.width,heroSize.height);
			config.rectGlPosForGuaiwu = CCRectMake(guaiwuPos.x - guaiwuSize.width/2,guaiwuPos.y - heroSize.height/2,guaiwuSize.width,guaiwuSize.height);
			
			FightGuideFactory::getFactory()->showInitGuideLayer(config);
		}
		else
		{
			beginFightTrigle(); // 开始战斗
		}
		
		break;
	case Chat_OneFightEnd:
		CCAssert(currentDirIndex < mDirvec.size()-1,"警告：必须还有下一场");
		// 下一场
//		dispLayerNextFightLabel(currentDirIndex+1);
		schedule(schedule_selector(ClientFightLayer::timeToNextFight),1.5f);
		break;
	case Chat_WhenFightEnd:
		if (taskId == 10)
		{
			p_layer = ChapterIllustrationLayer::create(TaskManager::getManager()->getCurrentFightTaskId(), this, 
				callfunc_selector(ClientFightLayer::ChapterIllustrationEndCallBack));
			p_scene->addChild(p_layer, this->getZOrder() + 1000);
			this->setVisible(false);
		}
		else 
		{
			if (mBundle.getString(JumpTo,"") == JumpToMainPageFromInitGuide)
			{
				// 跳主页
				GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
			}
			else
			{
				// 直接fightend
				timeToJumpToEndLayer(0);
			}
		}
		break;
	default:
		CCAssert(false,"不支持");
		break;
	}
}
void ClientFightLayer::ChapterIllustrationEndCallBack()
{
	this->setVisible(true);
	switch (mChatTag)
	{
	case Chat_WhenHeroOK:
		boardTop->setAllActorVisible(true);
		// 敌人从天而降
		actorFallDownFromSky(boardTop);
		break;
	case Chat_WhenFightEnd:
		if (mBundle.getString(JumpTo,"") == JumpToMainPageFromInitGuide)
		{
			// 跳主页
			GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		}
		else
		{
			// 直接fightend
			timeToJumpToEndLayer(0);
		}
		break;
	default:
		CCAssert(false,"不支持");
		break;
	}
}

void ClientFightLayer::actorFallDownFromSky(ClientChessBoard* board)
{
	//CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.5f);
	// 敌人从天而降
	CCArray* top_items = board->getInBoardItems();
	float width = 0;
	float height = 0;
	// 找最大的宽，高
	for (int i=0;i<top_items->count(); i++)
	{
		float tempWidth = ((CCNode*)top_items->objectAtIndex(i))->getContentSize().width;
		float tempHeight = ((CCNode*)top_items->objectAtIndex(i))->getContentSize().height;
		if (tempWidth > width)
			width = tempWidth;
		if (tempHeight > height)
			height = tempHeight;
	}
	float scale = 5.0f;
	width *= scale;
	height *= scale*1.2f;

	float posx = mVisibleSize.width/2;
	float posy = mVisibleSize.height + height/2.0f;
	float dist = 0;
	for (int i=0;i<top_items->count();i++)
	{
		ClientChessBoardItem* item = (ClientChessBoardItem*)top_items->objectAtIndex(i);
		item->setOpacity(0);
		CCPoint orginPos = item->getPosition();
		CCPoint layerPos = ccp(posx + dist * getIndexPos(item->getGridX(),item->getGridY()),posy);
		CCPoint startPos = item->getContainer()->convertToNodeSpace(convertToWorldSpace(layerPos));
		item->setPosition(startPos);
		item->setScale(scale);

		item->runAction(CCSequence::create(
			CCDelayTime::create(0.2f+0.2f*i),
			CCSpawn::create(
			CCFadeTo::create(0.2f,255),
			CCMoveTo::create(0.2f,orginPos),
			CCScaleTo::create(0.2f,1.0f),
			NULL
			),
			CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::nodeHitGround)),
			NULL
			));
	}

	dispLayerNextFightLabel(currentDirIndex+1);
	scheduleOnce(schedule_selector(ClientFightLayer::actionEnd),0.4 + 0.2*top_items->count());
}

void ClientFightLayer::nodeHitGround(CCNode* node)
{
	Music::sharedMusicPlayer()->playEffectSound("guaiWuHitedGroud.wav");
	screenShake();
}
void ClientFightLayer::screenShake()
{
	runAction(
		CCSpawn::create(
		CCSequence::create(
		CCRotateTo::create(0.08f,1.0f),
		CCRotateTo::create(0.08f,-1.0f),
		CCRotateTo::create(0.08f,0.5f),
		CCRotateTo::create(0.04f,0),
		NULL
		),
		
		CCSequence::create(
		CCMoveBy::create(0.08f,ccp(0,4)),
		CCMoveBy::create(0.08f,ccp(0,-2)),
		CCMoveBy::create(0.08f,ccp(0,1)),
		CCMoveTo::create(0.0f,ccp(0,0)),
		NULL),
		NULL));
}

void ClientFightLayer::actionEnd(float t)
{
	if (mBundle.getString(JumpTo,"") == JumpToMainPageFromInitGuide)
	{
		int id1 = currentDirIndex+1; // 到时再定值，根据dirIndex来确定
		int id2 = 2;
		vector<ChatItem> chat = ChatContentManager::sharedChatContentManager()->getGuideTaskChatContent(id1,id2);
		if(chat.size() > 0)
		{
			if (mChatTag != Chat_NO)
			{
				CCAssert(false,"");
			}
			mChatTag = Chat_WhenGuaiWuOK;
			ChatLayer* chatLayer = ChatLayer::create(chat,this);
			addChild(chatLayer,1024);
		}
		else
		{
			beginFightTrigle();
		}
	}
	else if (!FightGuideFactory::getFactory()->isFightGuideShowed(GID_BloodGuide))
	{// 第一次，就教学血瓶
		ZhuangTaiBar* bar = ((ZhuangTaiBar*)getChildByTag(TAG_BLOOD_VIAL_BAR_BUTTOM));
		BaseSprite* blood = bar->getBloodByIndex(bar->getCurrentBloodNum()-1);

		if (blood == NULL)
		{
			beginFightTrigle();
		}
		else
		{
			FightGuideFactory::getFactory()->showBloodGuideLayer(this,blood->getContentSize(),LF::getWorldPostion(blood));
		}
	}
	else
	{
		beginFightTrigle();
	}
}

void ClientFightLayer::fightGuideEnd(CCObject* guideLayer)
{
	FightGuideLayerBase* layer = (FightGuideLayerBase*)guideLayer;
	if (layer->getId() == GID_BloodGuide)
	{
		beginFightTrigle();	// 开始战斗
	}
	else if(layer->getId() == GID_INIT)
	{
		beginFightTrigle();
	}
	else if (layer->getId() == GID_FirstGetEquip)
	{
		CCNode* eff_node = (CCNode*)layer->getUserData();
		eff_node->runAction(CCSequence::create(
			CCDelayTime::create(0.2f),
			CCScaleTo::create(0.2f,0),
			CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::equipActionEnd)),
			CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::removeNode)),
			NULL
			));
		layer->setUserData(NULL);
		mTrigle->resumeTrigle();
		//setFightTrigleWaitSomeTime(0.4f); // 0.4s之后恢复
	}
	else if (layer->getId() == GID_FirstGetSkillBook)
	{
		CCNode* eff_node = (CCNode*)layer->getUserData();
		eff_node->runAction(CCSequence::create(
			CCDelayTime::create(0.2f),
			CCScaleTo::create(0.2f,0),
			CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::itemActionEnd)),
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
		layer->setUserData(NULL);
		mTrigle->resumeTrigle();
		//setFightTrigleWaitSomeTime(0.4f); // 0.4s之后恢复
	}
	else
	{
		CCAssert(false,"不支持");
	}
}

void ClientFightLayer::jumpItemClicked(CCObject* null_obj)
{
	if (!JumpHolder::getHolder()->canJump())
	{
		LFToast::makeToast(LFStrings::getValue("can_not_jump"));
		return;
	}

	fightEnd(true);
}
void ClientFightLayer::canJumpNotified(CCObject* null_obj)
{
	ZhuangTaiBar* bar = ((ZhuangTaiBar*)getChildByTag(TAG_BLOOD_VIAL_BAR_BUTTOM));
	bar->setJumpCDTimeOk();
	CCLog("%s --> jump button is enable!", __FUNCTION__);
}

void ClientFightLayer::beginFightTrigle()
{
	if (mDirvec[currentDirIndex].getStepNum() <= 0)
	{
		fightEnd(true);
		return;
	}
	mTrigle->trigle();
#ifdef DEBUG_ENABLE
	mTrigle->setAutoTrigleFalse();
	CCMenuItemFont::setFontSize(40);
	itemNext = CCMenuItemFont::create(
		getDebugStr(),
		this,SEL_MenuHandler(&ClientFightLayer::debugNextStepClicked));

	CCArray* items = CCArray::createWithCapacity(mDirvec.size()+1);
	float pos_x = mWinSize.width/2;
	float pos_y = mWinSize.height/2 - 150;
	float dis = 50;
	for (int i=0,count = mDirvec.size(); i<count; i++)
	{
		CCMenuItemFont* itemJumpDir = CCMenuItemFont::create(
			CCString::createWithFormat("Director[%d]",i)->getCString(),
			this,SEL_MenuHandler(&ClientFightLayer::debugGotoDirctor));
		itemJumpDir->setTag(i);
		items->addObject(itemJumpDir);
		itemJumpDir->setPosition(ccp(pos_x,pos_y));pos_y -= dis;

		if (currentDirIndex == i)
		{
			//ccColor3B c(12,62,106);
			itemJumpDir->setColor(ccc3(24,124,212));
		}
	}

	items->addObject(itemNext);
	CCMenu* menu = CCMenu::createWithArray(items);
	menu->setPosition(CCPointZero);
	addChild(menu,5,TAG_TEST_MENU);
	itemNext->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));


	{
		// 返回按钮
		{
			CCMenuItemImage* itemRestart = CCMenuItemImage::create(
				"restart.png",
				"restart.png",
				this,SEL_MenuHandler(&ClientFightLayer::menuItemClicked)
				);

			CCMenuItemImage* itemReturn = CCMenuItemImage::create(
				"zhuangbei_xiangxi_genghuan.png",
				"zhuangbei_xiangxi_genghuan_select.png",
				this,SEL_MenuHandler(&ClientFightLayer::menuItemClicked)
				);

			CCMenu* menu = CCMenu::create(itemRestart,itemReturn,NULL);
			addChild(menu,2);
			menu->setPosition(CCPointZero);
			itemRestart->setPosition(ccp(640 - itemRestart->getContentSize().width/2,itemRestart->getContentSize().height));
			itemReturn->setPosition(ccp(itemReturn->getContentSize().width,itemReturn->getContentSize().height));

			//
			itemRestart->setTag(TAG_ITEM_RESTART);
			itemReturn->setTag(TAG_ITEM_RETURN);
			itemReturn->setVisible(false);
			itemRestart->setVisible(false);
		}
	}
#else

	itemNext = NULL;
	//unschedule(schedule_selector(ClientFightLayer::fightTrgile));
	//schedule(schedule_selector(ClientFightLayer::fightTrgile),fight_gap);
#endif


	//20140708 新加boss血条
	mBossHP = 0;
	mDefaultHP = 0;
	int x_size = 1, x_idx;
	for (int i=0; i<FormationHeroAcountMax; ++i)
	{
		if (mDirvec.at(mDirvec.size()-1).getScript().init.oppositeTeam[i].heroId != INVALID_ID){
			x_size = mDirvec.at(mDirvec.size()-1).getScript().init.oppositeTeam[i].x_size;
			x_idx = i;
			break;
		}
	}
	if (x_size > 1)
	{
		mDefaultHP = mDirvec.at(mDirvec.size()-1).getScript().init.oppositeTeam[x_idx].defaultHp;
		mBossHP = mDirvec.at(mDirvec.size()-1).getScript().init.oppositeTeam[x_idx].hp;

		mboosBloodBar = BaseSprite::create("zhandou_guai_boss_xuetiao_bg.png");
		addChild(mboosBloodBar, 10);
		mboosBloodBar->setPosition(ccp(mWinSize.width/2, mWinSize.height-mboosBloodBar->getContentSize().height/2));
		//
		mBossBloodProgress = LF::createProgressTimer(BaseSprite::create("zhandou_guai_boss_xuetiao.png"));
		mboosBloodBar->addChild(mBossBloodProgress);
		mBossBloodProgress->setPosition(ccp(mboosBloodBar->getContentSize().width/2, mboosBloodBar->getContentSize().height/2));
		mBossBloodProgress->setPercentage(mBossHP/float(mDefaultHP) * 100.0f);
		//
		string name = TaskManager::getManager()->getBossName(mDirvec.at(mDirvec.size()-1).getScript().init.oppositeTeam[x_idx].profession_id);
		
		mBossBloodName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, 26);
		mBossBloodName->setColor(fonColor_ShangDian);
		mboosBloodBar->addChild(mBossBloodName, 1);
		mBossBloodName->setPosition(mBossBloodProgress->getPosition());

		mboosBloodBar->setScaleX(0);
		mboosBloodBar->runAction(CCSequence::create(
			CCScaleTo::create(0.15f, 1.2f, 1.2f),
			CCScaleTo::create(0.05f, 1.0f),
			NULL));
	}
}
//
const char* ClientFightLayer::getDebugStr()
{
	CCString* s = CCString::createWithFormat("Next Step[index->%d/num->%d]",mCurrentStepIndex,mDirvec[currentDirIndex].getStepNum());
	return s->getCString();
}
//
void ClientFightLayer::debugNextStepClicked(CCObject* sender)
{
	mTrigle->postTrigleTime();
	//fightTrgile(0);
	itemNext->initWithString(getDebugStr(),this,SEL_MenuHandler(&ClientFightLayer::debugNextStepClicked));
	if (mCurrentStepIndex > mDirvec[currentDirIndex].getStepNum() - 1)
	{
		//itemNext->setVisible(false);
	}
}

void ClientFightLayer::debugGotoDirctor(CCObject* sender)
{
	CCMenuItem* item = (CCMenuItem*)sender;
	if (item->getTag() > mDirvec.size() - 1)
	{
		CCLog("Error : %s --> tag = %d,size = %d",item->getTag(),mDirvec.size());
		return;
	}
	playWithDirctor(item->getTag());
}

void ClientFightLayer::menuItemClicked(CCObject* obj)
{
	CCMenuItem* item = (CCMenuItem*)obj;
	if (item->getTag() == TAG_ITEM_RESTART)
	{
		GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	}
	else if (item->getTag() == TAG_ITEM_RETURN)
	{
		GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	}
	else if (item->getTag() == TAG_ITEM_FAILD_YES)
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_ALERT);
		timeToJumpToEndLayer(0);
	}
}

ClientChessBoardItem* ClientFightLayer::getChessBoardItem(int unique_Id)
{
	ClientChessBoardItem* itemA = boardButom->getActorItemByUniqueId(unique_Id);
	ClientChessBoardItem* itemB = boardTop->getActorItemByUniqueId(unique_Id);
	CCAssert(itemA==NULL || itemB == NULL,"");

	if (itemA != NULL)
	{
		return itemA;
	}
	else if ( itemB != NULL)
	{
		return itemB;
	}
	return NULL;
}

void ClientFightLayer::removeSomeBuff(vector<int> buff_id_arr)
{
	// Top 里面寻找
	{
		CCArray* items = boardTop->getInBoardItems();
		int itemsNumInBoardA = items->count();
		for (int i=0;i<itemsNumInBoardA;i++)
		{
			ClientChessBoardItem* item = ((ClientChessBoardItem*)items->objectAtIndex(i));
			int buffNum = buff_id_arr.size();
			for (int j=0;j<buffNum;j++)
			{
				item->removeBuffById(buff_id_arr[j]);
			}
		}
	}

	// Buttom 里面寻找
	{
		CCArray* items = boardButom->getInBoardItems();
		int itemsNumInBoardA = items->count();
		for (int i=0;i<itemsNumInBoardA;i++)
		{
			ClientChessBoardItem* item = ((ClientChessBoardItem*)items->objectAtIndex(i));
			int buffNum = buff_id_arr.size();
			for (int j=0;j<buffNum;j++)
			{
				item->removeBuffById(buff_id_arr[j]);
			}
		}
	}
}

void ClientFightLayer::doBuff(BuffInStep buff)
{
	// 以后可能就不需要指定来移除buff了
	// 可以改成自动移除，ClientBuff上面也加上移除点
	// 那么他只是一个表现形式，到点就自动移除就是了
	// 至于buff的减血等特殊效果是脚本在doBuff里面指定的，所以没有问题，没有同步的bug
	removeSomeBuff(buff.remove_buff_array);

	//用于根据targetId来分组
	map<unsigned int,vector<Buff_Doing> > buffsOnTargetMap;
	map<unsigned int,vector<Buff_Doing> >::iterator it;

	// buff作用
	int targetNum = buff.effects.size();
	for (int i=0;i<targetNum;i++)
	{
		// 不处理反弹
		if (buff.effects[i].isFantan)
		{
			CCAssert(false,"为了不引起bug，这里assert一下，因为这里不处理反弹，那么调用者就应该先把反弹的数据移掉");
			continue;
		}

		// 看是否已经存在
		it = buffsOnTargetMap.find(buff.effects[i].target);
		if (it != buffsOnTargetMap.end()) // 已经有这个target的数据了。那么就附加
		{
			it->second.push_back(buff.effects[i]);
		}else
		{
			vector<Buff_Doing> doingVec;
			doingVec.push_back(buff.effects[i]);
			buffsOnTargetMap.insert(pair<unsigned int,vector<Buff_Doing> >(buff.effects[i].target,doingVec));
		}
		//ClientChessBoardItem* item = getChessBoardItem(buff.effects[i].target);
		//CCAssert(item!=NULL,"一定能找到");
		//item->hpAdd(buff.effects[i].hp_add);
	}

	// 让item表演
	for (it = buffsOnTargetMap.begin();it!=buffsOnTargetMap.end();it++)
	{
		ClientChessBoardItem* item = getChessBoardItem(it->first);
		CCAssert(item!=NULL,"一定能找到");
		item->runBuffDoingEffect(it->second);
	}
}

int ClientFightLayer::getAlivedItemNumByOriginBoard(ClientChessBoard* board)
{
	vector<ClientChessBoardItem*> retVec;
	// A 里面寻找
	{
		CCArray* items = boardTop->getInBoardItems();
		int itemsNumInBoardA = items->count();
		for (int i=0;i<itemsNumInBoardA;i++)
		{
			ClientChessBoardItem* item = ((ClientChessBoardItem*)items->objectAtIndex(i));
			if ( item->getOriginBoard() == board && item->getCurrentHp() > 0)
			{
				retVec.push_back(item);
			}
		}
	}
	// B 里面寻找
	{
		CCArray* items = boardButom->getInBoardItems();
		int itemsNumInBoardA = items->count();
		for (int i=0;i<itemsNumInBoardA;i++)
		{
			ClientChessBoardItem* item = ((ClientChessBoardItem*)items->objectAtIndex(i));
			if ( item->getOriginBoard() == board && item->getCurrentHp() > 0)
			{
				retVec.push_back(item);
			}
		}
	}

	return retVec.size();
}

vector<Buff_Doing> ClientFightLayer::separationFanTanDataFromStep(OneStep& step)
{
	vector<Buff_Doing> ret;
	for (vector<Buff_Doing>::iterator it = step.buff.effects.begin();it!=step.buff.effects.end();)
	{
		if (it->isFantan)
		{
			ret.push_back(*it);
			it = step.buff.effects.erase(it);
		}else
		{
			it++;
		}
	}

	return ret;
}

void ClientFightLayer::waitTimeEnd(CCObject* null_obj)
{
#ifdef DEBUG_ENABLE
	getChildByTag(TAG_TEST_MENU)->setVisible(true);
#endif 
}

static StepSkill s_skillstep; //skill用。
void ClientFightLayer::fightTrigle(CCObject* null_obj)
{
	if (fightEnded)
		return;

	OneStep step;
	// 只用于StepType_Fight
	MSG_Fight* fightData = NULL;
	// 只用于StepType_Fight
	vector<ClientChessBoardItemBase*> targetVec;
	vector<int> bloodSub;
	vector<bool> canMiss;

	if (mDirvec[currentDirIndex].getStep(mCurrentStepIndex,step))
	{
		// 分离出反弹的数据,放到要反弹的人身上
		{
			ClientChessBoardItem* temp;
			vector<Buff_Doing> fantanData = separationFanTanDataFromStep(step);
			int num = fantanData.size();
			for (int i=0;i<num;i++)
			{
				temp = getChessBoardItem(fantanData[i].fightor_u_id);
				CCAssert(temp!=NULL,"一定存在");
				temp->pushFanTanData(fantanData[i]);
			}
		}

		float bossBloodSubWaitTime = 0.0f;//boss减血等待时间
		ClientChessBoardItem* itemFightor = NULL;
		switch (step.stepType)
		{
		case StepType_Fight:
			last_fight_wait = 0.0f;
			itemFightor = getChessBoardItem(step.fightData.fightorUniqueId);
			CCLOG("StepType_Fight (%s)", itemFightor->getOriginBoard()->getBoardName().c_str());

			for (int i=0; i < (int)step.fightData.targetUniqueVec.size() ; i++)
			{
				ClientChessBoardItem* itemAttacked = getChessBoardItem(step.fightData.targetUniqueVec[i]);
				CCAssert(itemAttacked != NULL,"脚本指定的，不可能找不到");
				if (itemAttacked == NULL)
					continue;

				targetVec.push_back(itemAttacked);
				bloodSub.push_back(-step.fightData.targetBloodChange[i]);
				canMiss.push_back(step.fightData.canMissVec[i]);
			}

			fightData = MSG_Fight::create(itemFightor,targetVec,bloodSub,canMiss);

			if (step.fightData.fightPattern == Fight_Normal)
			{
				itemFightor->attack_UseNormal(fightData);
			}
			else if(step.fightData.fightPattern == Fight_Normal_BaoJi)
			{
				itemFightor->attack_UseNormalBaoJi(fightData);
			}
			else
			{
				CCAssert(false,"异常");
			}

			//boss 减血
			if (mDefaultHP != 0 && itemFightor->getOriginBoard()->getBoardName() == BOARD_NAME_BUTTOM) 
			{
				if (step.fightData.targetBloodChange.size() > 1) 
					CCLOG("%s : BUG ---> boos chess board has %d item", __FUNCTION__, step.fightData.targetBloodChange.size());
				addBossHP(step.fightData.targetBloodChange[0], 0.5f);
			}

			one_visible_step(step.cdTime_arr);
			// 
			doBuff(step.buff);
			break;
		case StepType_Skill:
			CCLOG("StepType_Skill (%s)", getChessBoardItem(step.skillData.fightorUniqueId)->getOriginBoard()->getBoardName().c_str());

			last_fight_wait = 1.5f;
			bossBloodSubWaitTime = 2.5f;
			switch (step.skillData.skillType)
			{
				//已改技能
			case SkillType_BaoLieZhan:
			case SkillType_LieGong:
			case SkillType_JianYu:
			case SkillType_LiRenHuiXuan:
			case SkillType_HuoQiuShu:
			case SkillType_ShaLu:
			case SkillType_ShanDian:
			case SkillType_JianShou:
//			case SkillType_ShenZhiZhuFu:
			case SkillType_ShenZhiXingNian:
			case SkillType_BOSS_Flog:
			case SkillType_BOSS_Sweep:
				setFightTrigleWaitSomeTime(2.5f);
				break;
			case SkillType_WenYi:
				setFightTrigleWaitSomeTime(3.3f);
				break;



//			case SkillType_ShenZhiXingNian:
			case SkillType_ShenZhiZhuFu:
				setFightTrigleWaitSomeTime(1.5f);	
				bossBloodSubWaitTime = 1.5f;
				break;
			case SkillType_LeiBao:
				setFightTrigleWaitSomeTime(1.5f);	
				bossBloodSubWaitTime = 1.5f;
				break;
			case SkillType_ShouHu:
				setFightTrigleWaitSomeTime(0.6f);	
				bossBloodSubWaitTime = 0.6f;
				break;
			case SkillType_TianTangZhiGuang:
				setFightTrigleWaitSomeTime(2.5f);
				bossBloodSubWaitTime = 2.5f;
				break;
			case SkillType_MangCi:
			case SkillType_JingZhunZhiCan:
			case SkillType_JiSuDuJian:
			case SkillType_BaoFengXue:
			case SkillType_KongJu:
			case SkillType_KuangBao:
			case SkillType_ShiXueZhiRen:
			case SkillType_YunShi:
			case SkillType_ZhongShenZhiLi:
			case SkillType_XuanFengZhan:
			case SkillType_ZhuiHunDuJian:
			case SkillType_SiWangSaoShe:
			case SkillType_HanBingCiGu:
			case SkillType_ShiHun:
			case SkillType_BaoNu:
			case SkillType_YinZheHun:
			case SkillType_ShunYinTuXi:
			case SkillType_BanYueZhan:
			case SkillType_CiSha:
			case SkillType_Monster_XieE:
			default:
				setFightTrigleWaitSomeTime(1.0f);	
				bossBloodSubWaitTime = 1.0f;
				break;
			}

			mSkillProc->skill_fight(step.skillData);
			//0.
			if (step.skillData.skillType == SkillType_BaoLieZhan ||
				step.skillData.skillType == SkillType_LieGong ||
				step.skillData.skillType == SkillType_JianYu ||
				step.skillData.skillType == SkillType_WenYi ||
				step.skillData.skillType == SkillType_LiRenHuiXuan ||
				step.skillData.skillType == SkillType_HuoQiuShu ||
				step.skillData.skillType == SkillType_ShaLu ||
				step.skillData.skillType == SkillType_ShanDian ||
				step.skillData.skillType == SkillType_JianShou ||
				step.skillData.skillType == SkillType_ShenZhiXingNian || //己方加buf的不动 ---》 prepare里面设定

				step.skillData.skillType == SkillType_BOSS_Flog ||
				step.skillData.skillType == SkillType_BOSS_Sweep)
			{
				skill_screenShake();
				s_skillstep = step.skillData;
				this->runAction(CCSequence::create(
					CCDelayTime::create(0.75f),
					CCCallFuncND::create(this, callfuncND_selector(ClientFightLayer::userSkillPrepare), 0),
					NULL));
			}


			//boss 减血
			if (mDefaultHP != 0 && getChessBoardItem(step.skillData.fightorUniqueId)->getOriginBoard()->getBoardName() == BOARD_NAME_BUTTOM) 
			{
				if (step.skillData.targetBloodChange.size() > 1)
					CCLOG("%s : BUG ---> boos chess board has %d item", __FUNCTION__, step.skillData.targetBloodChange.size());
				addBossHP(step.skillData.targetBloodChange[0], bossBloodSubWaitTime);
			}

			doBuff(step.buff);
			one_visible_step(step.cdTime_arr);
			break;
		case StepType_UseBlood:
			CCLog("StepType_UseBlood");
			last_fight_wait = 0.0f;
			itemFightor = getChessBoardItem(step.useBlood.uniqueId);
			CCAssert(itemFightor!=NULL,"");

			useBlood(itemFightor,step.useBlood);
			// 
			doBuff(step.buff);
			break;
		case StepType_Move:
			CCLog("StepType_Move");
			last_fight_wait = 0.0f;
			itemFightor = getChessBoardItem(step.moveData.uniqueId);
			CCAssert(itemFightor!=NULL,"");
			int x_index;
			int y_index;
			getGridPosByIndex(step.moveData.destIndex,x_index,y_index);

			// 需要从自己的棋盘移动到别人的棋盘
			if (itemFightor->getCurrentBoard() == itemFightor->getOriginBoard() && !step.moveData.inSelfChessBoard)
			{
				// 第一步移除该移除的
				if (itemFightor->getCurrentBoard() == boardButom)
				{
					if (itemFightor->getHero()->x_size > 1) // 宽的
					{
						boardTop->removeActor(y_index);
					}else
					{
						boardTop->removeActor(x_index,y_index);
					}

				}else
				{
					if (itemFightor->getHero()->x_size > 1) // 宽的
					{
						boardButom->removeActor(y_index);
					}else
					{
						boardButom->removeActor(x_index,y_index);
					}
				}

				// 第二步，移动
				itemFightor->getCurrentBoard()->crossMove(
					itemFightor->getCurrentBoard() == boardButom?boardTop:boardButom,itemFightor,x_index,y_index);
			}else
			{
				if (itemFightor->getHero()->x_size > 1)
				{
					itemFightor->getCurrentBoard()->removeActor(y_index);
				}
				else
				{
					itemFightor->getCurrentBoard()->removeActor(x_index,y_index);
				}
				// 移动
				itemFightor->getCurrentBoard()->moveActor(itemFightor,x_index,y_index);
			}

			one_visible_step(step.cdTime_arr);
			// 
			doBuff(step.buff);
			break;
		case StepType_ChangeRound:
			CCLog("StepType_ChangeRound");

			mCurrentStepIndex ++; // 因为直接就return了，所有这里加一个  
			// 刷新回合的lable
			getZhuangTaiBar()->getHuiHeLabel()->setString(
				CCString::createWithFormat("%s%d/%d",LFStrings::getValue("HuiHe").c_str(), step.changeRound.round+1, MAX_STEP)->getCString());
			//huiHeLable->setString(CCString::createWithFormat(LF::getString("di_x_huihe").c_str(),step.changeRound.round+1, MAX_STEP)->getCString());
			// dobuff
			last_fight_wait = 0.0f;
			one_visible_step(step.cdTime_arr);
			doBuff(step.buff);
			mTrigle->postTrigleTime(); // 调用下一个    
			return;

			break;
		case StepType_Wait:
			CCLog("StepType_Wait");
			mCurrentStepIndex ++; // 因为直接就return了，所有这里加一个
			// 直接跳过，只看得到眩晕的buff移除
			doBuff(step.buff);
			// 没有了
			if (mCurrentStepIndex > mDirvec[currentDirIndex].getStepNum() - 1 )
			{
				CCLog("Step Over...");
				mTrigle->stop();
				fightEnd();
				return;
			}
			mTrigle->postTrigleTime();
			return;
			break;
		case StepType_FailByTakeTooMuchTime:
			CCLog("StepType_FailByTakeTooMuchTime");

			// 
			doBuff(step.buff);

			mTrigle->stop();
			fightEnd();
			return;
			break;
		default:
			CCAssert(false,"");
			break;
		}
	}
	else
	{
		fightEnd(true);
	}
	mCurrentStepIndex ++;
}

ZhuangTaiBar* ClientFightLayer::getZhuangTaiBar()
{
	return ((ZhuangTaiBar*)getChildByTag(TAG_BLOOD_VIAL_BAR_BUTTOM));
}
void ClientFightLayer::useBlood(ClientChessBoardItem* user,StepUseBlood step)
{
	// 延迟通知加血,那么战斗驱动也得延迟
	// 不然血还没有加上，就被砍死了
	setFightTrigleWaitSomeTime(1.5f);
	CCNode* node_hug_user = CCNode::create();
	addChild(node_hug_user);
	// 保存user和其血量的变化
	node_hug_user->setUserData(user);
	node_hug_user->setTag(step.bloodChangeForUser);
	node_hug_user->runAction(CCSequence::create(
		CCDelayTime::create(1.5f),
		CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::useBloodOk)),
		CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
		NULL
		));

	short bloodNum = user->getOriginBoard()->getCurrentBloodNum()-1;
	user->getOriginBoard()->setCanUsedBloodNum(bloodNum);
	ZhuangTaiBar* bar = ((ZhuangTaiBar*)getChildByTag(TAG_BLOOD_VIAL_BAR_BUTTOM));
	BaseSprite* blood = bar->getBloodByIndex(bloodNum);
	bar->setCurrentBloodNum(bloodNum);
	bar->refreshBlood();

	// 用shader的blood，盖在现在的血瓶上，做一个流光效果，然后消失
	vector<CCTexture2D*> opTexVec;
	vector<string> opTexSamlpeNameVec;
	string v_shader_file = "xueping.vsh";
	string f_shader_file = "xueping.fsh";
	opTexSamlpeNameVec.push_back("CC_Texture0");
	opTexSamlpeNameVec.push_back("flow_texture");
	opTexVec.push_back(CCTextureCache::sharedTextureCache()->addImage("zhandou_yaopin.png"));
	opTexVec.push_back(CCTextureCache::sharedTextureCache()->addImage("zhandou_yaopin_shuaxin.png"));
	TextureOpSprite* bloodCopy = new TextureOpSprite(opTexVec,opTexSamlpeNameVec,v_shader_file,f_shader_file);
	addChild(bloodCopy,bar->getZOrder());
	bloodCopy->setPosition(LF::getNativePostion(blood,this));
	bloodCopy->release();
	bloodCopy->runAction(CCSequence::create(
		CCDelayTime::create(0.3f),
		CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
		NULL
		));

	// 消失过程
	/*BaseSprite* xuepingxiaoshi = BaseSprite::create("zhandou_yaopin_xiaohao_1.png");
	addChild(xuepingxiaoshi,bar->getZOrder());
	xuepingxiaoshi->setPosition(LF::getNativePostion(blood,this));
	xuepingxiaoshi->setVisible(false);
	CCFiniteTimeAction* xuepingxiaoshi_action = SpriteHelper::easyCreateAction("zhandou_yaopin_xiaohao_%d.png",1,21,0.03f,false);
	xuepingxiaoshi->runAction(CCSequence::create(
	CCDelayTime::create(0.2f),
	CCShow::create(),
	LFDelayToMusic::create(0,"jiaXue_2.wav"),
	xuepingxiaoshi_action,
	CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
	NULL
	));*/

	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("zhandou_yaopin_plist.plist");
	SkillSprite* xuepingxiaoshi = SkillSprite::create("zhandou_yaopin_xiaohao_1.png");
	addChild(xuepingxiaoshi,bar->getZOrder());
	xuepingxiaoshi->setPosition(LF::getNativePostion(blood,this));
	xuepingxiaoshi->setVisible(false);
	CCFiniteTimeAction* xuepingxiaoshi_action = SpriteHelper::easyCreateAction_pList("zhandou_yaopin_xiaohao_%d.png",1,21,0.03f,false);
	xuepingxiaoshi->runAction(CCSequence::create(
		CCDelayTime::create(0.2f),
		CCShow::create(),
		LFDelayToMusic::create(0,"jiaXue_2.wav"),
		xuepingxiaoshi_action,
		CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
		NULL
		));


	// 吸收过程
	/*BaseSprite* xuepingxishou = BaseSprite::create("zhandou_yaopin_xishou_1.png");
	user->addChild(xuepingxishou);
	xuepingxishou->setPosition(user->getSkillFirePos());
	xuepingxishou->setScale(1.5f);

	xuepingxishou->setVisible(false);
	CCFiniteTimeAction* xuepingxishou_action = SpriteHelper::easyCreateAction("zhandou_yaopin_xishou_%d.png",1,33,0.03f,false);
	xuepingxishou->runAction(CCSequence::create(
		CCDelayTime::create(0.8f),
		CCShow::create(),
		xuepingxishou_action,
		CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
		NULL
		));*/

	SkillSprite* xuepingxishou = SkillSprite::create("zhandou_yaopin_xishou_1.png");
	user->addChild(xuepingxishou);
	xuepingxishou->setPosition(user->getSkillFirePos());
	xuepingxishou->setScale(1.5f);

	xuepingxishou->setVisible(false);
	CCFiniteTimeAction* xuepingxishou_action = SpriteHelper::easyCreateAction_pList("zhandou_yaopin_xishou_%d.png",1,33,0.03f,false);
	xuepingxishou->runAction(CCSequence::create(
		CCDelayTime::create(0.8f),
		CCShow::create(),
		xuepingxishou_action,
		CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
		NULL
		));

	// 清空血瓶
	user->clearBlood();

	//cd 头像
	string headfile = ImageManager::getManager()->getImageData_ProfessionHead(user->getProfession()).getCurrentImageFile();
	BaseSprite *headImg = BaseSprite::create(headfile);
	headImg->setPosition(LF::getNativePostion(blood,this));
	addChild(headImg,bar->getZOrder()+1, TAG_BloodHeroImgCD+user->getUniqueId());
	headImg->setScale(0.5f);
	BaseSprite *headMask = BaseSprite::create("renxing_renwu_touxiang_select.png");
	headImg->addChild(headMask);
	headMask->setPosition(ccp(headImg->getContentSize().width/2, headImg->getContentSize().height/2));
	CCLabelTTF *cdLabel = CCLabelTTF::create(CCString::createWithFormat("%d",3)->getCString(), fontStr_BookAntiqua, 24);
	headImg->addChild(cdLabel, 1, 10000);
	cdLabel->setScale(2);
	cdLabel->setPosition(ccp(headImg->getContentSize().width, headImg->getContentSize().height));
	//
	FX_CommonFunc::setAllChildrenColorOrOpacity(headImg, ccWHITE, 0, 2);
	FX_CommonFunc::nodeAndAllChildRunAction(headImg, CCSequence::create(CCDelayTime::create(0.75f), CCFadeTo::create(0.25f, 255), NULL));
}

void ClientFightLayer::useBloodOk(CCNode* node_hag_user)
{
	ClientChessBoardItem* user = (ClientChessBoardItem*)node_hag_user->getUserData();
	user->hpAdd(node_hag_user->getTag());
}
void ClientFightLayer::addBossHP(int toAdd, float delayTime)
{
	if (toAdd == 0)
		return;

	int oldHP = mBossHP;
	mBossHP += toAdd;
	if (mBossHP < 0) mBossHP = 0;
	else if (mBossHP > mDefaultHP) mBossHP = mDefaultHP;

	mBossBloodProgress->runAction(CCSequence::create(
		CCDelayTime::create(delayTime),
		CCProgressFromTo::create(0.1f, oldHP/float(mDefaultHP) * 100.0f, mBossHP/float(mDefaultHP) * 100.0f),
		NULL));
}

void ClientFightLayer::one_visible_step(vector<CdDiscr> cdDisc)
{
	// 更新CD时间
	int num = cdDisc.size();
	for (int i=0;i<num;i++)
	{
		ClientChessBoardItem* item = getChessBoardItem(cdDisc[i].uniqueId);
		if (item)
		{
			item->setCurrentBloodCdTime(cdDisc[i].cdTime);

			CCSprite *headImg = (CCSprite*)this->getChildByTag(TAG_BloodHeroImgCD+cdDisc[i].uniqueId);
			if (headImg){
				CCLabelTTF *cdLabel = (CCLabelTTF*)headImg->getChildByTag(10000);
				cdLabel->setString(CCString::createWithFormat("%d", cdDisc[i].cdTime)->getCString());
				if (cdDisc[i].cdTime == 0){
					FX_CommonFunc::nodeAndAllChildRunAction(headImg, CCSequence::create(
						CCDelayTime::create(0.5f), CCFadeTo::create(0.25f, 0), CCRemoveSelf::create(), NULL));
				}
			}
		}
	}
}

void ClientFightLayer::setItemOrderBack(CCNode* node)
{
	this->reorderChild(node->getParent(),ITEM_Z_OERDER_IN_STAGE);
}

void ClientFightLayer::showFailByTakeTooMuchTimeDailog()
{
	//弹框背景
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	//提示
	{
		CCLabelTTF* lable = CCLabelTTF::create(
			LFStrings::getValue("StepType_FailByTakeTooMuchTime").c_str(), 
			fontStr_kaiti,30,CCSize(431, 0), kCCTextAlignmentLeft);
		dialog->addChild(lable);
		lable->setPosition(ccp(0,0));
		lable->setPosition(ccp(dialog->getContentSize().width/2, 228));

		ccColor3B color;
		color.r = 201;
		color.g = 156;
		color.b = 18;
		lable->setColor(color);
	}

	//按键
	CCMenu* menu;
	{
		CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&ClientFightLayer::menuItemClicked));
		menu = CCMenu::create(menuItemMakeSure, NULL);
		menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/2,70));
		menuItemMakeSure->setTag(TAG_ITEM_FAILD_YES);
	}

	//弹框
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128,TAG_ALERT);
		lfAlert->release();
	}

	stopAllActions();
	unscheduleAllSelectors();
}

void ClientFightLayer::someOneDead(CCObject* clientChessBoardItem)
{
	--mSurvivalSpriteNum;

	pulled_wait = 0.0f;
	ClientChessBoardItem* item = (ClientChessBoardItem*)clientChessBoardItem;

	vector<FightReward> willPull;
	// 掉东西装备
	int rewardNum = rewardVec.size(); 
	for (int i=0;i<rewardNum;i++)
	{
		if (rewardVec[i].deadUniqueId == item->getUniqueId() && currentDirIndex == rewardVec[i].dirIndex)
		{
			if (rewardVec[i].rewardType == TYPE_ITEM && rewardVec[i].item.type == ItemsType_Hammer){
				mHammerIdx = i;
				continue;
			}
			willPull.push_back(rewardVec[i]);
		}
	}

	if (mSurvivalSpriteNum == 0 && mHammerIdx != -1){
		willPull.push_back(rewardVec[mHammerIdx]);
	}
	// 掉落
	{
		vector<CCPoint> offsets;
		int num = willPull.size();
		switch (num)
		{
		case 0:
			break;
		case 1:
			offsets.push_back(ccp(0,0));
			break;
		case 2:
			offsets.push_back(ccp(-80,0));
			offsets.push_back(ccp(80,0));
			break;
		case 3:
			offsets.push_back(ccp(-60,-60));
			offsets.push_back(ccp(60,-60));
			offsets.push_back(ccp(0,60));
			break;
		case 4:
			offsets.push_back(ccp(-60,-70));
			offsets.push_back(ccp(60,-70));
			offsets.push_back(ccp(-60,70));
			offsets.push_back(ccp(60,70));
			break;
		case 5:
			offsets.push_back(ccp(-60,-90));
			offsets.push_back(ccp(60,-90));
			offsets.push_back(ccp(-90,40));
			offsets.push_back(ccp(90,40));
			offsets.push_back(ccp(0,100));
			break;
		default:
			for (int i=0;i<num;i++)
			{// 横着一排
				offsets.push_back(ccp(-80*(num/2)+80*i,0));
			}
			break;
		}

		for (int i=0;i<num;i++)
		{
			if (willPull[i].rewardType == TYPE_EQUIP)
			{
				pullOneEquip(item,willPull[i],offsets[i]);
			}
			else if (willPull[i].rewardType == TYPE_ITEM)
			{
				if (willPull[i].item.type == ItemsType_Hammer){
					showHammer(item, offsets[i]);
				}else {
					pullOneItem(item,willPull[i],offsets[i]);
				}
			}
			else if (willPull[i].rewardType == TYPE_ITEM_SUIPIAN)
			{
				pullOneItemSuiPian(item,willPull[i],offsets[i]);
			}
			else if (willPull[i].rewardType == TYPE_SHENBING_SUIPIAN)
			{
				pullOneShenBinSuiPian(item,willPull[i],offsets[i]);
			}
			else if(willPull[i].rewardType == TYPE_PPT)
			{
				pullOnePPT(item,willPull[i],offsets[i]);
			}
			else
			{
				CCAssert(false,"BUG");
			}
		}
	}
}
void ClientFightLayer::setFightTrigleWaitSomeTime(float time_to_wait)
{
#ifdef DEBUG_ENABLE
	getChildByTag(TAG_TEST_MENU)->runAction(CCSequence::create(
		CCHide::create(),
		CCDelayTime::create(time_to_wait*1.5f),
		CCShow::create(),
		NULL
		));
#endif 
	mTrigle->pauseTrigleSomeTime(time_to_wait);
	//CCNode* node = CCNode::create();
	//addChild(node,0,TAG_TRIGLE_PAUSE_NODE);
	//node->runAction(CCSequence::create(
	//	CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::pauseFightTrigle)),
	//	CCDelayTime::create(time_to_wait),
	//	CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::resumeFightTrigle)),
	//	CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::removeNode)),
	//	NULL
	//	));
}
void ClientFightLayer::canleFightTrigleWait()
{
	mTrigle->clearPauseTime();
	//CCNode* node = getChildByTag(TAG_TRIGLE_PAUSE_NODE);
	//if (node)
	//{
	//	node->stopAllActions();
	//	node->removeFromParentAndCleanup(true);
	//}
}

void ClientFightLayer::pullOneItemSuiPian(ClientChessBoardItem* item, FightReward itemSuiPian,CCPoint offset /*= CCPointZero*/)
{
	pullOneItemOrSuiPian(item,itemSuiPian,offset);
}
void ClientFightLayer::pullOneShenBinSuiPian(ClientChessBoardItem* item, FightReward shenBingSuiPian,CCPoint offset /*= CCPointZero*/)
{

	Flat_DuanzaoGroupZBInfo shenbingData;
	int useMinLevel = CS::getShenBingLevels()[0];
	bool mustFound = ChuanqiPool::getPool()->getChuanQiData(shenBingSuiPian.shenBingSuiPian.zb_group_id,shenbingData,useMinLevel);
	CCAssert(mustFound,"");
	if (!mustFound)
		return;

	// 攻击暂停一会
	{
		setFightTrigleWaitSomeTime(2.5f);
		pulled_wait = 2.5f;
	}

	// 角色卡牌消失
	{
		CCAction* disappear = CCSequence::create(
			CCDelayTime::create(0.3f),
			CCSpawn::create(
			CCMoveBy::create(0.1f,ccp(0,20.0f)),
			CCFadeTo::create(0.1f,50),
			NULL
			),
			CCFadeTo::create(0.1f,0),
			NULL
			);

		item->runAction((CCAction*)disappear->copy()->autorelease());
	}


	string cardFile = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(ZhuangbeiColour_Chuanqi
		, false
		, SuitPool::getPool()->isSuit(shenBingSuiPian.shenBingSuiPian.zb_group_id));

	CCNode* eff_node = CCNode::create();
	float total_delay = 0.5f;
	BaseSprite* card = BaseSprite::create(cardFile);
	vector<ItemsType> null_baoshi;
	QianghuaData nullQiangHua;
	nullQiangHua.qianghua_count = 0;
	nullQiangHua.max_qianghua_time = 0;
	BaseSprite* zhuangbei = ImageManager::getManager()->getSpriteOfZhuangbei(
		shenbingData.zhuangbei_pic,
		ImageManager::ZhuangbeiColourExt_ShengBing,
		0,null_baoshi,nullQiangHua,0);
	{
		BaseSprite* mask = BaseSprite::create("zhandou_chuanqi_suipian_xiaoguo.png");
		zhuangbei->addChild(mask);
		mask->setPosition(ccp(zhuangbei->getContentSize().width/2,zhuangbei->getContentSize().height/2));
		zhuangbei->setScale(0.24f);
		card->addChild(zhuangbei);
		zhuangbei->setPosition(ccp(card->getContentSize().width/2,card->getContentSize().height/2));
	}

	BaseSprite* pic5 = NULL;
	BaseSprite* pic4 = NULL;
	BaseSprite* pic3 = NULL;
	BaseSprite* pic2 = NULL;
	BaseSprite* pic1 = NULL;

	if (shenbingData.zhuangbei_colour == ZhuangbeiColour_Chuanqi)
	{
		//bool isSuit = SuitPool::getPool()->isSuit(shenBingSuiPian.shenBingSuiPian.zb_group_id);
		//pic5 = BaseSprite::create(isSuit?"zhandou_shenbingtaozhuang_5.png":"zhandou_shenbing_5.png");
		//pic4 = BaseSprite::create(isSuit?"zhandou_shenbingtaozhuang_4.png":"zhandou_shenbing_4.png");
		//pic3 = BaseSprite::create(isSuit?"zhandou_shenbingtaozhuang_3.png":"zhandou_shenbing_3.png");
		//pic2 = BaseSprite::create(isSuit?"zhandou_shenbingtaozhuang_2.png":"zhandou_shenbing_2.png");
		//pic1 = BaseSprite::create(isSuit?"zhandou_shenbingtaozhuang_1.png":"zhandou_shenbing_1.png");
		
		pic5 = BaseSprite::create("zhandou_chuanqi_5.png");
		pic4 = BaseSprite::create("zhandou_chuanqi_4.png");
		pic3 = BaseSprite::create("zhandou_chuanqi_3.png");
		pic2 = BaseSprite::create("zhandou_chuanqi_2.png");
		pic1 = BaseSprite::create("zhandou_chuanqi_1.png");
		// 效果
		{
			eff_node->addChild(card,0);
			card->addChild(pic1);
			pic1->setPosition(ccp(card->getContentSize().width/2,card->getContentSize().height/2));

			eff_node->addChild(pic2,-1);
			eff_node->addChild(pic3,-1);
			eff_node->addChild(pic4,-1);
			eff_node->addChild(pic5,-1);

			pic2->setVisible(false);
			pic3->setVisible(false);
			pic4->setVisible(false);
			pic5->setVisible(false);
			card->setVisible(false);

			float initScale = 3.0f;

			float scale_back_time = 0.5f;
			float show_time = 1.8f;
			float fade_out_time = 0.5f;
			float totale_scale_over_time = 0.8f;

			pic2->setScale(initScale);
			pic3->setScale(initScale);
			pic4->setScale(initScale);
			pic5->setScale(initScale);

			CCAction* topAction = CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCSpawn::create(
				CCEaseIn::create(CCScaleTo::create(scale_back_time,1.2f),2),
				CCRepeat::create(
				CCSequence::create(
				CCSpawn::create(
				CCScaleTo::create(1.0f,1.0f),
				CCFadeTo::create(1.0f,200),
				NULL),
				CCSpawn::create(
				CCScaleTo::create(1.0f,1.2f),
				CCFadeTo::create(1.0f,255),
				NULL),
				NULL),
				-1),
				NULL),
				NULL);

			pic2->runAction((CCAction*)topAction->copy()->autorelease());
			pic3->runAction((CCAction*)topAction->copy()->autorelease());

			pic4->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				CCRepeat::create(CCRotateBy::create(1.0f,5.0f),-1),
				NULL));
			pic5->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				CCRepeat::create(CCRotateBy::create(1.0f,-5.0f),-1),
				NULL));

			//card->setScale(0.8f);
			card->setScaleY(0);
			card->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time),
				CCShow::create(),
				CCScaleTo::create(0.05f,card->getScaleX()),
				NULL
				));
			//// 偏移
			//eff_node->runAction(CCSequence::create(
			//	CCDelayTime::create(total_delay),
			//	CCMoveBy::create(scale_back_time,offset),
			//	NULL));

			pic1->runAction(
				CCSequence::create(
				CCDelayTime::create(total_delay + scale_back_time),
				CCRepeat::create(
				CCSequence::create(
				CCFadeTo::create(1.0f,200),
				CCFadeTo::create(1.0f,255),
				NULL),
				-1),NULL)
				);


			addChild(eff_node,10);
			eff_node->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));

			eff_node->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCScaleTo::create(totale_scale_over_time,0),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::equipActionEnd)),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::removeNode)),
				NULL
				));
			// 变淡
			pic1->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic2->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic3->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic4->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic5->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			card->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			zhuangbei->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
		}
	}
	else
	{
		CCAssert(false,"");
	}
}
void ClientFightLayer::pullOneEquip(ClientChessBoardItem* item, FightReward equip,CCPoint offset /* = CCPointZero */)
{
	if (equip.equip.isShenBing)
	{
		CCLog("Waring : %s the equip is shenbing, the pull effect is not surpport!",__FUNCTION__);
	}

	// 攻击暂停一会
	{
		if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Baizhuang)
		{
			if (mTrigle->getCurrentPauseTimeLeft() < 0.2f)
			{
				setFightTrigleWaitSomeTime(1.0f);
			}
			pulled_wait = 0.5f;
		}else
		{
			if (mTrigle->getCurrentPauseTimeLeft() < 0.2f)
			{
				setFightTrigleWaitSomeTime(2.5f);
			}
			pulled_wait = 2.5f;
		}
		
	}

	// 角色卡牌消失
	{
		CCAction* disappear = CCSequence::create(
			CCDelayTime::create(0.3f),
			CCSpawn::create(
			CCMoveBy::create(0.1f,ccp(0,20.0f)),
			CCFadeTo::create(0.1f,50),
			NULL
			),
			CCFadeTo::create(0.1f,0),
			NULL
			);

		item->runAction((CCAction*)disappear->copy()->autorelease());

		//if (item->getDizuo() != NULL)
		//	item->getDizuo()->runAction((CCAction*)disappear->copy()->autorelease());
		//if (item->getBloodBarBg() != NULL)
		//	item->getBloodBarBg()->runAction((CCAction*)disappear->copy()->autorelease());
	}


	string cardFile = ImageManager::getManager()->getImageData_DiWen_WithBianKuang((ZhuangbeiColour)equip.equip.zhuangbei_colour
		,false//, ChuanqiPool::getPool()->isShenBing(equip.equip.group_id)
		, SuitPool::getPool()->isSuit(equip.equip.group_id));

	// 播放音效
	switch (equip.equip.zhuangbei_colour)
	{
	case ZhuangbeiColour_Baizhuang:
		Music::sharedMusicPlayer()->playEffectSound("diaoZB_pt.wav");
		break;
	case ZhuangbeiColour_Fumo:
		Music::sharedMusicPlayer()->playEffectSound("diaoZB_mf.wav");
		break;
	case ZhuangbeiColour_Xiyou:
		Music::sharedMusicPlayer()->playEffectSound("diaoZB_xy.wav");
		break;
	case ZhuangbeiColour_Chuanqi:
		Music::sharedMusicPlayer()->playEffectSound("diaoZB_cq.wav");
		break;
	default:
		CCAssert(false,"");
		break;
	}

	CCNode* eff_node = CCNode::create();
	float total_delay = 0.5f;
	BaseSprite* card = BaseSprite::create(cardFile);
	BaseSprite* zhuangbei = ImageManager::getManager()->getSpriteOfZhuangbei(equip.equip);
	{
		zhuangbei->setScale(0.24f);
		card->addChild(zhuangbei);
		zhuangbei->setPosition(ccp(card->getContentSize().width/2,card->getContentSize().height/2));
	}

	BaseSprite* pic5 = NULL;
	BaseSprite* pic4 = NULL;
	BaseSprite* pic3 = NULL;
	BaseSprite* pic2 = NULL;
	BaseSprite* pic1 = NULL;

	float particle_delayTime = 0;
	if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Baizhuang)
	{
		particle_delayTime = 1.5f;
	}
	else if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Fumo)
	{
		pic5 = BaseSprite::create("zhandou_mofa_5.png");
		pic4 = BaseSprite::create("zhandou_mofa_4.png");
		pic3 = BaseSprite::create("zhandou_mofa_3.png");
		pic2 = BaseSprite::create("zhandou_mofa_2.png");
		pic1 = BaseSprite::create("zhandou_mofa_1.png");
		particle_delayTime = 2.75f;
	}
	else if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Xiyou)
	{
		pic5 = BaseSprite::create("zhandou_xiyou_5.png");
		pic4 = BaseSprite::create("zhandou_xiyou_4.png");
		pic3 = BaseSprite::create("zhandou_xiyou_3.png");
		pic2 = BaseSprite::create("zhandou_xiyou_2.png");
		pic1 = BaseSprite::create("zhandou_xiyou_1.png");
		particle_delayTime = 3.0f;
	}
	else if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Chuanqi)
	{
		pic5 = BaseSprite::create("zhandou_chuanqi_5.png");
		pic4 = BaseSprite::create("zhandou_chuanqi_4.png");
		pic3 = BaseSprite::create("zhandou_chuanqi_3.png");
		pic2 = BaseSprite::create("zhandou_chuanqi_2.png");
		pic1 = BaseSprite::create("zhandou_chuanqi_1.png");
		particle_delayTime = 3.35f;
	}
	else
	{
		CCAssert(false,"");
	}

	if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Baizhuang)
	{
		// 前奏的圆点
		BaseSprite* circleNode = BaseSprite::create();
		{
			CCFiniteTimeAction* circleNodeFrameAction = NULL;
			circleNodeFrameAction = SpriteHelper::easyCreateAction("zhandou_putong_fujia_%d.png",1,4,0.1f,false);
			circleNode->runAction(CCSequence::create(
				CCDelayTime::create(total_delay-0.1f*3),
				circleNodeFrameAction,
				CCFadeTo::create(0.3f,0),
				CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),NULL));
			addChild(circleNode,10);
			circleNode->setScale(1.5f);
			circleNode->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));
		}

		// 效果
		{
			eff_node->addChild(card,0);
			card->setVisible(false);
			float initScale = 0.6f;
			float scale_back_time = 0.1f;
			float show_time = 0.8f;
			float fade_out_time = 0.5f;
			float totale_scale_over_time = 0.2f;

			card->setScale(initScale);
			card->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				NULL));

			addChild(eff_node,10);
			eff_node->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));

			if (!FightGuideFactory::getFactory()->isFightGuideShowed(GID_FirstGetEquip))
			{// 还没显示过
				//canleFightTrigleWait(); // 取消外面之前的定时，因为如果不取消，那么这下面的暂停就会在将来恢复的
				//pauseFightTrigle(NULL); // 暂停攻击的任务

				// 直接暂停，教学完毕之后再开始
				mTrigle->pauseTrigle();
				eff_node->runAction(CCSequence::create(
					CCDelayTime::create(total_delay+scale_back_time),
					CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::showFirstGetEquipGuide)),
					NULL
					));
			}
			else
			{// 已经显示过了，那么就正常的显示
				eff_node->runAction(CCSequence::create(
					CCDelayTime::create(total_delay+scale_back_time+show_time),
					CCScaleTo::create(totale_scale_over_time,0),
					CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::equipActionEnd)),
					CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::removeNode)),
					NULL
					));
				// 变淡
				card->runAction(CCSequence::create(
					CCDelayTime::create(total_delay+scale_back_time+show_time),
					CCFadeTo::create(fade_out_time,0),
					NULL
					));
				// 变淡
				zhuangbei->runAction(CCSequence::create(
					CCDelayTime::create(total_delay+scale_back_time+show_time),
					CCFadeTo::create(fade_out_time,0),
					NULL
					));
			}

		}

	}
	else if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Fumo || equip.equip.zhuangbei_colour == ZhuangbeiColour_Xiyou)
	{
		// 前奏的圆点
		BaseSprite* circleNode = BaseSprite::create();
		{
			CCFiniteTimeAction* circleNodeFrameAction = NULL;
			if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Fumo)
			{
				circleNodeFrameAction = SpriteHelper::easyCreateAction("zhandou_mofa_fujia_%d.png",1,5,0.1f,false);
			}else
			{
				circleNodeFrameAction = SpriteHelper::easyCreateAction("zhandou_xiyou_fujia_%d.png",1,5,0.1f,false);
			}
			circleNode->runAction(CCSequence::create(
				CCDelayTime::create(total_delay-0.1f*4),
				circleNodeFrameAction,
				CCFadeTo::create(0.3f,0),
				CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),NULL));
			addChild(circleNode,10);
			circleNode->setScale(1.5f);
			circleNode->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));
		}
		
		// 效果
		{
			eff_node->addChild(card,0);
			card->addChild(pic1);
			pic1->setPosition(ccp(card->getContentSize().width/2,card->getContentSize().height/2));

			eff_node->addChild(pic2,-1);
			eff_node->addChild(pic3,-1);
			eff_node->addChild(pic4,-1);
			eff_node->addChild(pic5,-1);

			pic2->setVisible(false);
			pic3->setVisible(false);
			pic4->setVisible(false);
			pic5->setVisible(false);
			card->setVisible(false);

			float initScale = 0.6f;

			float scale_back_time = 0.1f;
			float show_time = 1.8f;
			float fade_out_time = 0.5f;
			float totale_scale_over_time = 0.8f;

			pic2->setScale(initScale);
			pic3->setScale(initScale);
			pic4->setScale(initScale);
			pic5->setScale(initScale);

			CCAction* topAction = CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCSpawn::create(
				CCEaseIn::create(CCScaleTo::create(scale_back_time,1.2f),2),
				CCRepeat::create(
				CCSequence::create(
				CCSpawn::create(
				CCScaleTo::create(1.0f,1.0f),
				CCFadeTo::create(1.0f,200),
				NULL),
				CCSpawn::create(
				CCScaleTo::create(1.0f,1.2f),
				CCFadeTo::create(1.0f,255),
				NULL),
				NULL),
				-1),
				NULL),
				NULL);

			pic2->runAction((CCAction*)topAction->copy()->autorelease());
			pic3->runAction((CCAction*)topAction->copy()->autorelease());

			pic4->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				CCRepeat::create(CCRotateBy::create(1.0f,5.0f),-1),
				NULL));
			pic5->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				CCRepeat::create(CCRotateBy::create(1.0f,-5.0f),-1),
				NULL));

			card->setScale(initScale);
			card->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				NULL));

			//// 偏移
			//eff_node->runAction(CCSequence::create(
			//	CCDelayTime::create(total_delay),
			//	CCMoveBy::create(scale_back_time,offset),
			//	NULL));

			pic1->runAction(
				CCSequence::create(
				CCDelayTime::create(total_delay + scale_back_time),
				CCRepeat::create(
				CCSequence::create(
				CCFadeTo::create(1.0f,200),
				CCFadeTo::create(1.0f,255),
				NULL),
				-1),NULL)
				);


			addChild(eff_node,10);
			eff_node->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));

			eff_node->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCScaleTo::create(totale_scale_over_time,0),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::equipActionEnd)),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::removeNode)),
				NULL
				));
			// 变淡
			pic1->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic2->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic3->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic4->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic5->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			card->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			zhuangbei->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
		}
	}else
	{// 传奇

		// 效果
		{
			eff_node->addChild(card,0);
			card->addChild(pic1);
			pic1->setPosition(ccp(card->getContentSize().width/2,card->getContentSize().height/2));

			eff_node->addChild(pic2,-1);
			eff_node->addChild(pic3,-1);
			eff_node->addChild(pic4,-1);
			eff_node->addChild(pic5,-1);

			pic2->setVisible(false);
			pic3->setVisible(false);
			pic4->setVisible(false);
			pic5->setVisible(false);
			card->setVisible(false);

			float initScale = 3.0f;

			float scale_back_time = 0.5f;
			float show_time = 1.8f;
			float fade_out_time = 0.5f;
			float totale_scale_over_time = 0.8f;

			pic2->setScale(initScale);
			pic3->setScale(initScale);
			pic4->setScale(initScale);
			pic5->setScale(initScale);

			CCAction* topAction = CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCSpawn::create(
				CCEaseIn::create(CCScaleTo::create(scale_back_time,1.2f),2),
				CCRepeat::create(
				CCSequence::create(
				CCSpawn::create(
				CCScaleTo::create(1.0f,1.0f),
				CCFadeTo::create(1.0f,200),
				NULL),
				CCSpawn::create(
				CCScaleTo::create(1.0f,1.2f),
				CCFadeTo::create(1.0f,255),
				NULL),
				NULL),
				-1),
				NULL),
				NULL);

			pic2->runAction((CCAction*)topAction->copy()->autorelease());
			pic3->runAction((CCAction*)topAction->copy()->autorelease());

			pic4->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				CCRepeat::create(CCRotateBy::create(1.0f,5.0f),-1),
				NULL));
			pic5->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				CCRepeat::create(CCRotateBy::create(1.0f,-5.0f),-1),
				NULL));

			//card->setScale(0.8f);
			card->setScaleY(0);
			card->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time),
				CCShow::create(),
				CCScaleTo::create(0.05f,card->getScaleX()),
				NULL
				));
			//// 偏移
			//eff_node->runAction(CCSequence::create(
			//	CCDelayTime::create(total_delay),
			//	CCMoveBy::create(scale_back_time,offset),
			//	NULL));

			pic1->runAction(
				CCSequence::create(
				CCDelayTime::create(total_delay + scale_back_time),
				CCRepeat::create(
				CCSequence::create(
				CCFadeTo::create(1.0f,200),
				CCFadeTo::create(1.0f,255),
				NULL),
				-1),NULL)
				);


			addChild(eff_node,10);
			eff_node->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));

			eff_node->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCScaleTo::create(totale_scale_over_time,0),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::equipActionEnd)),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::removeNode)),
				NULL
				));
			// 变淡
			pic1->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic2->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic3->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic4->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic5->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			card->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			zhuangbei->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
		}
	}



	{
		CCParticleSystemQuad* par = NULL;
		if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Baizhuang)
			par = CCParticleSystemQuad::create("zhuangbei_xiaoguo_putong.plist");
		else if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Fumo)
			par = CCParticleSystemQuad::create("zhuangbei_donghua_mofa.plist");
		else// if (equip.equip.zhuangbei_colour == ZhuangbeiColour_Xiyou)
			par = CCParticleSystemQuad::create("zhuangbei_donghua_xiyou.plist");
		addChild(par, eff_node->getZOrder()-1); // 效果node的下面
		par->setPosition(eff_node->getPosition());
		par->runAction(CCSequence::create(
			CCDelayTime::create(particle_delayTime),
			CCMoveTo::create(0.5f, ccp(50,50)),
			CCFadeOut::create(0.05f),
			CCRemoveSelf::create(),
			NULL));
	}
}
void ClientFightLayer::pullOneItem(ClientChessBoardItem* item, FightReward reward,CCPoint offset /* = CCPointZero */)
{
	pullOneItemOrSuiPian(item,reward,offset);
}
void ClientFightLayer::pullOneItemOrSuiPian(ClientChessBoardItem* item, FightReward itemOrSuiPian,CCPoint offset /*= CCPointZero*/)
{
	CCAssert(itemOrSuiPian.rewardType == TYPE_ITEM_SUIPIAN || itemOrSuiPian.rewardType == TYPE_ITEM,"只能是Item或者碎片");


	// 攻击暂停一会
	{
		setFightTrigleWaitSomeTime(0.5f);
		pulled_wait = 0.5f;
	}
	// 角色卡牌消失
	{
		item->runAction(CCSequence::create(
			CCDelayTime::create(0.3f),
			CCSpawn::create(
			CCMoveBy::create(0.1f,ccp(0,20.0f)),
			CCFadeTo::create(0.1f,50),
			NULL
			),
			CCFadeTo::create(0.1f,0),
			NULL
			));
	}


	// 掉宝箱
	{
		float first_delay = 0.1f;
		float frame_time = 0.06f;
		float card_show_time = 0.8f;

		BaseSprite* flashLight = BaseSprite::create("zhandou_daoju_1.png");
		CCFiniteTimeAction* lightAction = SpriteHelper::easyCreateAction(
			"zhandou_daoju_%d.png",1,15,frame_time,false);

		flashLight->setVisible(false);
		flashLight->runAction(CCSequence::create(
			CCDelayTime::create(first_delay),
			CCShow::create(),
			lightAction,
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
		addChild(flashLight,11);
		flashLight->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));

		BaseSprite* card = BaseSprite::create("zhandou_huode_daoju.png");
		BaseSprite* itemPic = NULL;
		{
			string itemFile = itemOrSuiPian.rewardType == TYPE_ITEM_SUIPIAN?
			ImageManager::getManager()->getItemDebrisFileName(itemOrSuiPian.itemSuiPian.item_id):
			ImageManager::getManager()->getItemFilename(itemOrSuiPian.item.type);

			itemPic = BaseSprite::create(itemFile);
			card->addChild(itemPic);
			itemPic->setPosition(ccp(card->getContentSize().width/2,card->getContentSize().height/2));
		}
		addChild(card,10);
		card->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));

		if ( itemOrSuiPian.rewardType == TYPE_ITEM &&  !FightGuideFactory::getFactory()->isFightGuideShowed(GID_FirstGetSkillBook) && 
			itemOrSuiPian.item.type > ItemsType_SkillBook_MinId && itemOrSuiPian.item.type < ItemsType_SkillBook_MaxId)
		{// 还没教学技能书

			mTrigle->pauseTrigle();
			CCAction* cardAction = CCSequence::create(
				CCDelayTime::create(first_delay),
				CCDelayTime::create(frame_time*5),
				CCShow::create(),
				CCFadeTo::create(frame_time*8,255),
				NULL
				);

			card->setOpacity(0);
			itemPic->setOpacity(0);
			card->runAction(CCSequence::create(
				(CCSequence*)cardAction->copy()->autorelease(),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::showFirstGetSkillBookGuide)),
				NULL
				));
			card->setTag(itemOrSuiPian.item.type);

			itemPic->runAction(CCSequence::create(
				(CCSequence*)cardAction->copy()->autorelease(),
				NULL
				));
		}
		else
		{
			CCAction* cardAction = CCSequence::create(
				CCDelayTime::create(first_delay),
				CCDelayTime::create(frame_time*5),
				CCShow::create(),
				CCFadeTo::create(frame_time*8,255),
				CCDelayTime::create(card_show_time),
				CCScaleTo::create(0.2f,0),
				NULL
				);

			card->setOpacity(0);
			itemPic->setOpacity(0);
			card->runAction(CCSequence::create(
				(CCSequence*)cardAction->copy()->autorelease(),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::itemActionEnd)),
				CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
				NULL
				));
			card->setTag(itemOrSuiPian.item.type);

			itemPic->runAction(CCSequence::create(
				(CCSequence*)cardAction->copy()->autorelease(),
				NULL
				));
		}


		if (itemOrSuiPian.rewardType == TYPE_ITEM)
		{
			float particle_delayTime = 1.75f;
			CCParticleSystemQuad* par = CCParticleSystemQuad::create("zhuangbei_xiaoguo_putong.plist");
			addChild(par, card->getZOrder()-1); // 效果node的下面
			par->setPosition(card->getPosition());
			par->runAction(CCSequence::create(
				CCDelayTime::create(particle_delayTime),
				CCMoveTo::create(0.5f, ccp(50,50)),
				CCFadeOut::create(0.05f),
				CCRemoveSelf::create(),
				NULL));
		}
	}
}
void ClientFightLayer::pullOnePPT(ClientChessBoardItem* item, FightReward ppt,CCPoint offset /*= CCPointZero*/)
{
	// 攻击暂停一会
	{
		setFightTrigleWaitSomeTime(0.5f);
		pulled_wait = 0.5f;
	}
	// 角色卡牌消失
	{
		item->runAction(CCSequence::create(
			CCDelayTime::create(0.3f),
			CCSpawn::create(
			CCMoveBy::create(0.1f,ccp(0,20.0f)),
			CCFadeTo::create(0.1f,50),
			NULL
			),
			CCFadeTo::create(0.1f,0),
			CCHide::create(),
			NULL
			));
	}


	// 掉能量点
	{
		float first_delay = 0.1f;
		float frame_time = 0.08f;

		BaseSprite* pptItem = BaseSprite::create("zhandou_daoju_diaoluo_nengliang_1.png");
		pptItem->setVisible(false);
		CCFiniteTimeAction* pptAction = SpriteHelper::easyCreateAction(
			"zhandou_daoju_diaoluo_nengliang_%d.png",1,16,frame_time,false);

		pptItem->runAction(CCSequence::create(
			CCDelayTime::create(first_delay),
			CCShow::create(),
			pptAction,
			CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
			NULL
			));
		addChild(pptItem,11);
		offset = ccpAdd(offset,ccp(0,60));
		pptItem->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));
	}
}



void ClientFightLayer::showItemCard(CCNode* baoxiang)
{
	CCNode* card = baoxiang->getChildByTag(TAG_CARD_IN_BAOXIANG);
	card->setScale(0.0f);

	card->runAction(CCSequence::create(
		CCDelayTime::create(0.001f),
		CCShow::create(),
		CCSpawn::create(
			CCScaleTo::create(0.1f,0.6f),
			//CCMoveBy::create(0.1f,ccp(0,150)),
			NULL),
		CCDelayTime::create(0.01f),
		CCRepeat::create(CCSequence::create(
			CCMoveBy::create(0.35f,ccp(0,7)),
			CCMoveBy::create(0.35f,ccp(0,-7)),NULL),
			-1),
		NULL
		));

	card->runAction(CCSequence::create(
		CCDelayTime::create(1.5f),
		CCFadeTo::create(0.3f,0),
		NULL
		));
	baoxiang->runAction(CCSequence::create(
		CCDelayTime::create(1.5f),
		CCFadeTo::create(0.3f,0),
		CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::itemActionEnd)),
		CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),
		NULL
		));

}
void ClientFightLayer::showHammer(ClientChessBoardItem* item, CCPoint offset)
{
	// 攻击暂停一会
	{
		if (mTrigle->getCurrentPauseTimeLeft() < 0.2f)
		{
			setFightTrigleWaitSomeTime(2.5f);
		}
		pulled_wait = 2.5f;
	}

	// 角色卡牌消失
	{
		CCAction* disappear = CCSequence::create(
			CCDelayTime::create(0.3f),
			CCSpawn::create(
			CCMoveBy::create(0.1f,ccp(0,20.0f)),
			CCFadeTo::create(0.1f,50),
			NULL
			),
			CCFadeTo::create(0.1f,0),
			NULL
			);

		item->runAction((CCAction*)disappear->copy()->autorelease());
	}

	// 播放音效
	Music::sharedMusicPlayer()->playEffectSound("diaoZB_cq.wav");

	CCNode* eff_node = CCNode::create();
	float total_delay = 0.5f;
//	BaseSprite* card = BaseSprite::create(cardFile);
	BaseSprite* card = BaseSprite::create();
	BaseSprite* zhuangbei = BaseSprite::create("daoju_tiechui.png");
	{
//		zhuangbei->setScale(0.24f);
		zhuangbei->setScale(0.35f);
		card->addChild(zhuangbei);
		zhuangbei->setPosition(ccp(card->getContentSize().width/2,card->getContentSize().height/2));
	}

	BaseSprite* pic5 = NULL;
	BaseSprite* pic4 = NULL;
	BaseSprite* pic3 = NULL;
	BaseSprite* pic2 = NULL;
	BaseSprite* pic1 = NULL;

	{
		pic5 = BaseSprite::create("zhandou_mofa_5.png");
		pic4 = BaseSprite::create("zhandou_mofa_4.png");
		pic3 = BaseSprite::create("zhandou_mofa_3.png");
		pic2 = BaseSprite::create("zhandou_mofa_2.png");
		pic1 = BaseSprite::create("zhandou_mofa_1.png");
	}

	{
		// 前奏的圆点
		BaseSprite* circleNode = BaseSprite::create();
		{
			CCFiniteTimeAction* circleNodeFrameAction = NULL;
			circleNodeFrameAction = SpriteHelper::easyCreateAction("zhandou_mofa_fujia_%d.png",1,5,0.1f,false);
			circleNode->runAction(CCSequence::create(
				CCDelayTime::create(total_delay-0.1f*4),
				circleNodeFrameAction,
				CCFadeTo::create(0.3f,0),
				CCCallFuncN::create(this,SEL_CallFuncN(&BaseLayer::removeNode)),NULL));
			addChild(circleNode,10);
			circleNode->setScale(1.5f);
			circleNode->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));
		}

		// 效果
		{
			eff_node->addChild(card,0);
			card->addChild(pic1);
			pic1->setPosition(ccp(card->getContentSize().width/2,card->getContentSize().height/2));

			eff_node->addChild(pic2,-1);
			eff_node->addChild(pic3,-1);
			eff_node->addChild(pic4,-1);
			eff_node->addChild(pic5,-1);

			pic2->setVisible(false);
			pic3->setVisible(false);
			pic4->setVisible(false);
			pic5->setVisible(false);
			card->setVisible(false);

			float initScale = 0.6f;

			float scale_back_time = 0.1f;
			float show_time = 1.8f;
			float fade_out_time = 0.5f;
			float totale_scale_over_time = 0.8f;

			pic2->setScale(initScale);
			pic3->setScale(initScale);
			pic4->setScale(initScale);
			pic5->setScale(initScale);

			CCAction* topAction = CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCSpawn::create(
				CCEaseIn::create(CCScaleTo::create(scale_back_time,1.2f),2),
				CCRepeat::create(
				CCSequence::create(
				CCSpawn::create(
				CCScaleTo::create(1.0f,1.0f),
				CCFadeTo::create(1.0f,200),
				NULL),
				CCSpawn::create(
				CCScaleTo::create(1.0f,1.2f),
				CCFadeTo::create(1.0f,255),
				NULL),
				NULL),
				-1),
				NULL),
				NULL);

			pic2->runAction((CCAction*)topAction->copy()->autorelease());
			pic3->runAction((CCAction*)topAction->copy()->autorelease());

			pic4->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				CCRepeat::create(CCRotateBy::create(1.0f,5.0f),-1),
				NULL));
			pic5->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				CCRepeat::create(CCRotateBy::create(1.0f,-5.0f),-1),
				NULL));

			card->setScale(initScale);
			card->runAction(CCSequence::create(
				CCDelayTime::create(total_delay),
				CCShow::create(),
				CCScaleTo::create(scale_back_time,1.0f),
				NULL));

			//// 偏移
			//eff_node->runAction(CCSequence::create(
			//	CCDelayTime::create(total_delay),
			//	CCMoveBy::create(scale_back_time,offset),
			//	NULL));

			pic1->runAction(
				CCSequence::create(
				CCDelayTime::create(total_delay + scale_back_time),
				CCRepeat::create(
				CCSequence::create(
				CCFadeTo::create(1.0f,200),
				CCFadeTo::create(1.0f,255),
				NULL),
				-1),NULL)
				);


			addChild(eff_node,10);
			eff_node->setPosition(ccpAdd(LF::getNativePostion(item,this),offset));

			eff_node->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCScaleTo::create(totale_scale_over_time,0),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::equipActionEnd)),
				CCCallFuncN::create(this,SEL_CallFuncN(&ClientFightLayer::removeNode)),
				NULL
				));
			// 变淡
			pic1->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic2->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic3->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic4->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			pic5->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			card->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
			// 变淡
			zhuangbei->runAction(CCSequence::create(
				CCDelayTime::create(total_delay+scale_back_time+show_time),
				CCFadeTo::create(fade_out_time,0),
				NULL
				));
		}
	}
}
void ClientFightLayer::itemActionEnd(CCNode* node)
{
	ItemsType item_type = (ItemsType)node->getTag();
	ZhuangTaiBar* bar = (ZhuangTaiBar*)getChildByTag(TAG_BLOOD_VIAL_BAR_BUTTOM);
	bar->addOneItem(item_type);
}
void ClientFightLayer::equipActionEnd(CCNode* node)
{
	ZhuangTaiBar* bar = (ZhuangTaiBar*)getChildByTag(TAG_BLOOD_VIAL_BAR_BUTTOM);
	bar->addOneEquip();
	node->removeFromParentAndCleanup(true);
}

void ClientFightLayer::showFirstGetEquipGuide(CCNode* eff_node)
{
	CCLayer* layer = FightGuideFactory::getFactory()->showFirstGetEquipGuideLayer(
		this,CCSizeMake(101,147),LF::getWorldPostion(eff_node));
	layer->setUserData(eff_node); // 保存，回调时移除
}

void ClientFightLayer::showFirstGetSkillBookGuide(CCNode* eff_node)
{
	int itemType = eff_node->getTag();
	int skillType = SkillTree::sharedSkillTree()->itemsType2SkillType(itemType);
	CCLayer* layer = FightGuideFactory::getFactory()->showFirstGetSkillBookGuideLayer(
		this,skillType,CCSizeMake(101,147),LF::getWorldPostion(eff_node));
	layer->setUserData(eff_node); // 保存，回调时移除
}

void ClientFightLayer::fightEnd(bool notWait /* = false */)
{
	if (fightEnded)
	{
		return;
	}
	fightEnded = true;

	// 掉落装备，物品，咱不等待
	if (notWait)
	{
		pulled_wait = 0.0f;
		last_fight_wait = 0.0f; 
	}

	removeObserver(MSG_JUMP_CAN_JUMP);



	// 还有下一场
	if (currentDirIndex < mDirvec.size() - 1)
	{
		if (mBundle.getString(JumpTo,"") == JumpToMainPageFromInitGuide)
		{// 初始引导的结束对话
			vector<ChatItem> chat = ChatContentManager::sharedChatContentManager()->getGuideTaskChatContent(currentDirIndex+1,4);
			if(chat.size() > 0)
			{
				mChatTag = Chat_OneFightEnd;
				ChatLayer* chatLayer = ChatLayer::create(chat,this);
				addChild(chatLayer,1024);
			}
			else
			{
				// 直接下一场
//				dispLayerNextFightLabel(currentDirIndex+1);
				//schedule(schedule_selector(ClientFightLayer::timeToNextFight),1.5f);
				schedule(schedule_selector(ClientFightLayer::timeToNextFight),(notWait?0.1f:1.8f) + last_fight_wait);
			}
		}
		else
		{
//			dispLayerNextFightLabel(currentDirIndex+1);
			//schedule(schedule_selector(ClientFightLayer::timeToNextFight),1.5f);
			schedule(schedule_selector(ClientFightLayer::timeToNextFight),(notWait?0.1f:1.8f) + last_fight_wait);
		}
		
	}else
	{

		string intent = mBundle.getString(JumpTo,JumpToDefault);
		if (intent == JumpToJingJiSuiJi)
		{
			schedule(schedule_selector(ClientFightLayer::timeToJumpToJingJi),(notWait?0.1f:1.8f) + last_fight_wait);
		}
		else if (intent == JumpToJingJiFree)
		{
			schedule(schedule_selector(ClientFightLayer::timeToJumpToZiYouJingji),(notWait?0.1f:1.8f) + last_fight_wait);
		}
		else if (intent == JumpToJingJiKaiBaoXiang)
		{
			schedule(schedule_selector(ClientFightLayer::timeToJumpToKaiBaoXiang),(notWait?0.1f:1.8f) + last_fight_wait);
		}
		else if (intent == JumpToShouCheng)
		{
			schedule(schedule_selector(ClientFightLayer::timeToShouCheng),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
		else if (intent == JumpToPaiHangBang)
		{
			schedule(schedule_selector(ClientFightLayer::timeToPaiHangBang),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
		else if (intent == JumpToDianFengDuiJueList)
		{
			schedule(schedule_selector(ClientFightLayer::timeToDianFengDuiJue),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
		else if (intent == JumpToFightEndLayer)
		{
			// 到对话界面
			schedule(schedule_selector(ClientFightLayer::beginFightEndChat),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
		else if (intent == JumpToMainPageFromInitGuide)
		{
			// 到对话界面
			schedule(schedule_selector(ClientFightLayer::beginFightEndChat),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
		else if (intent == JumpToJiXianShouCheng)
		{
			// 到对话界面
			schedule(schedule_selector(ClientFightLayer::timeToJiXianShouCheng),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
		else if (intent == JumpToChuanQiLu)
		{
			// 到对话界面
			schedule(schedule_selector(ClientFightLayer::timeToChuanQiLu),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
		else if (intent == JumpToXueSheJiaoTang)
		{
			// 到对话界面
			schedule(schedule_selector(ClientFightLayer::timeToXueSeJiaoTangEndLayer),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
		else if (intent == JumpToMapsLayer)
		{
			// 到对话界面
			schedule(schedule_selector(ClientFightLayer::timeToMapsLayer),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
		else 
		{
			CCLog("Waring : %s --> No %s config",__FUNCTION__,JumpTo);
			// scheduleOnce 的回调不能再schedule
			// 于是就直接用schedule  
			schedule(schedule_selector(ClientFightLayer::timeToJumpToEndLayer),(notWait?0.1f:1.8f)+ last_fight_wait);
		}
	}
}
void ClientFightLayer::dispLayerNextFightLabel(int changci_num)
{
	BaseSprite *title = BaseSprite::create(CCString::createWithFormat("zhandou_changci%d.png", changci_num)->getCString());
	this->addChild(title, 1024);
	title->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	title->setOpacity(0);
	title->setScale(3);

	title->runAction(CCSequence::create(
		CCSpawn::create(CCFadeTo::create(0.15f, 255), CCScaleTo::create(0.15f, 1), NULL),
		CCScaleTo::create(0.05f, 1.1f), CCScaleTo::create(0.05f, 1.0f),
		CCDelayTime::create(0.5f),
		CCMoveTo::create(0.15f, ccp(mWinSize.width + title->getContentSize().width, title->getPositionY())),
		CCRemoveSelf::create(), 
		NULL));
}

void ClientFightLayer::beginFightEndChat(float t)
{
	unschedule(schedule_selector(ClientFightLayer::beginFightEndChat));
	if (pulled_wait > 0.0f)
	{
		schedule(schedule_selector(ClientFightLayer::beginFightEndChat),pulled_wait);  
		pulled_wait = 0.0f;
		return;
	}
	// 非初始引导，对话完了就去结算界面
	if (mBundle.getString(JumpTo,"") != JumpToMainPageFromInitGuide && (mDirvec[mDirvec.size()-1].getScript().endResult.myResult == Result_Lose ||
		TaskManager::getManager()->getLastActivedMainTaskId() != TaskManager::getManager()->getCurrentFightTaskId()) )
	{
		// 直接跳向结束界面
		timeToJumpToEndLayer(0);
		return;
	}

	vector< pair<string, string> > chat;
	{
		if (mBundle.getString(JumpTo,"") == JumpToMainPageFromInitGuide)
		{// 初始引导的结束对话
			vector<ChatItem> chat = ChatContentManager::sharedChatContentManager()->getGuideTaskChatContent(currentDirIndex+1,4);
			if(chat.size() > 0)
			{
				mChatTag = Chat_WhenFightEnd;
				ChatLayer* chatLayer = ChatLayer::create(chat,this);
				addChild(chatLayer,1024);
			}else
			{
				// 直接fightend
				timeToJumpToEndLayer(t);
			}
		}
		else
		{// 一般的对话
			char temp[64];
			sprintf(temp,"duihua_%d_%d_showed",TaskManager::getManager()->getCurrentFightTaskId(),4);
			bool showed = false;
//			bool isBossSection = ChessBoardStateManager::getManager()->isBossSection(g_taskId,g_gridIndex);
			TaskManager* mapManager = TaskManager::getManager();
			bool isBossSection = ChessBoardStateManager::getManager()->isBossSection(mapManager->getCurrentFightTaskId(),mapManager->getCurrentFightGridIdx());
			bool needShow = isBossSection;
			if (History::getHistory()->getBoolFromCurrentHero(temp,showed))
			{
				if (showed)
				{
					needShow = false;
				}
			}

			vector<ChatItem> chat = ChatContentManager::sharedChatContentManager()->getMainTaskChatContentById(TaskManager::getManager()->getCurrentFightTaskId(),4);
			if(needShow && chat.size() > 0 && TaskManager::getManager()->getLastActivedMainTaskId() == TaskManager::getManager()->getCurrentFightTaskId())
			{
				mChatTag = Chat_WhenFightEnd;
				ChatLayer* chatLayer = ChatLayer::create(chat,this);
				addChild(chatLayer,1024);
				// 设置显示了
				History::getHistory()->putInCurrentHero(temp,true);
			}else
			{
				// 直接fightend
				timeToJumpToEndLayer(t);
			}
		}
	}



}

void ClientFightLayer::timeToNextFight(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToNextFight));
	if (pulled_wait > 0.0f)
	{
		schedule(schedule_selector(ClientFightLayer::timeToNextFight),pulled_wait);  
		//pulled = false;
		pulled_wait = 0.0f;
		return;
	}
	currentDirIndex++;
	playWithDirctor(currentDirIndex);
}

void ClientFightLayer::timeToJumpToEndLayer(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToJumpToEndLayer));
	if (pulled_wait > 0.0f)
	{
		schedule(schedule_selector(ClientFightLayer::timeToJumpToEndLayer),pulled_wait);  
		//pulled = false;
		pulled_wait = 0.0f;
		return;
	}

	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}


	vector<EquipData> gettedEquipVec;
	vector<ItemInfo> gettedItemVec;

	int state = 1;	// 0：输，1成功，-1步奏太长
	int dirNum = mDirvec.size();
	for (int i=0;i<dirNum;i++)
	{
		FightEnd endData = mDirvec[i].getScript().endResult;

		OneStep endStep;
		bool getted = mDirvec[i].getStep(mDirvec[i].getStepNum() - 1,endStep);
		if (!getted)
		{
			CCLog("Error : %s -->step error!!!",__FUNCTION__);
		}
		//CCAssert(getted,"肯定能得到");
		if (getted && endStep.stepType == StepType_FailByTakeTooMuchTime)
		{
			state = -1;
			break;
		}
		else if (endData.myResult == Result_Lose)
		{
			state = 0;
			break;
		}
		else
		{
			for (int j=0;j<endData.gettedItem.size();j++)
			{
				gettedItemVec.push_back(endData.gettedItem[j]);
			}
			for (int j=0;j<endData.gettedZhuangBei.size();j++)
			{
				gettedEquipVec.push_back(endData.gettedZhuangBei[j]);
			}
		}
	}

	Settlement *taskInfo = new Settlement;
	taskInfo->taskName = "NoName";
	taskInfo->bossName = "NoName";
	taskInfo->win = state;
	//taskInfo->exp = mNowData.exp_current - mBeForedata.exp_current + levelUp*mBeForedata.exp_need;
	taskInfo->exp = 0;
	taskInfo->items = gettedItemVec;
	taskInfo->equips = gettedEquipVec;
	for (int i=0;i<FormationHeroAcountMax;i++)
	{
		taskInfo->oldArmy[i] = oldArmy[i];
		taskInfo->curArmy[i] = nowArmy[i];
	}

	FightSettlementLayer *retLayer = FightSettlementLayer::create(taskInfo);
	CCScene *runS = CCDirector::sharedDirector()->getRunningScene();
	runS->addChild(retLayer, 130, TAG_FIGHT_END_LAYER);

	delete taskInfo;
}


void ClientFightLayer::timeToJumpToJingJi(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToJumpToJingJi));
	if (pulled_wait > 0.0f)
	{
		schedule(schedule_selector(ClientFightLayer::timeToJumpToJingJi),pulled_wait);  
		//pulled = false;
		pulled_wait = 0.0f;
		return;
	}

	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}

//	GameDirector::getDirector()->goToJingJiChangFromFight(1);
	FightEndBaseLayer *layer = FightEndBaseLayer::create(mDirvec.at(mDirvec.size()-1).getScript().endResult.myResult==Result_Win,
		FightEndBaseLayer::ComeFrom_Jingji_Suiji_Type);
	layer->setSuijiJingjiIndex(1);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 130);
}
void ClientFightLayer::timeToJumpToKaiBaoXiang(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToJumpToKaiBaoXiang));
	if (pulled_wait > 0.0f)
	{
		schedule(schedule_selector(ClientFightLayer::timeToJumpToKaiBaoXiang),pulled_wait);  
		//pulled = false;
		pulled_wait = 0.0f;
		return;
	}

	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}

//	GameDirector::getDirector()->goToJingJiChangFromFight(2);
	FightEndBaseLayer *layer = FightEndBaseLayer::create(mDirvec.at(mDirvec.size()-1).getScript().endResult.myResult==Result_Win,
		FightEndBaseLayer::ComeFrom_Jingji_Suiji_Type);
	layer->setSuijiJingjiIndex(2);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 130);
}

void ClientFightLayer::timeToShouCheng(float t)
{
	CCAssert(false,"废弃了");
	//unschedule(schedule_selector(ClientFightLayer::timeToShouCheng));
	//if (pulled_wait)
	//{
	//	schedule(schedule_selector(ClientFightLayer::timeToShouCheng),pulled_wait);  
	//	//pulled = false;
	//	pulled_wait = 0.0f;
	//	return;
	//}

	//if (mBundle.getBool("testMode",false))
	//{
	//	GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	//	return;
	//}

	//if (mDirvec[currentDirIndex].getScript().endResult.myResult == Result_Win)
	//{
	//	GameDirector::getDirector()->goToShouChengFromFight(ShouCheng_Win);
	//}
	//else if (mDirvec[currentDirIndex].getScript().endResult.myResult == Result_Lose)
	//{
	//	GameDirector::getDirector()->goToShouChengFromFight(ShouCheng_Fail);
	//}
	//else
	//{
	//	CCAssert(false,"");
	//	GameDirector::getDirector()->goToShouChengFromFight(ShouCheng_Fail);
	//}
}

void ClientFightLayer::timeToPaiHangBang(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToPaiHangBang));
	if (pulled_wait)
	{
		schedule(schedule_selector(ClientFightLayer::timeToPaiHangBang),pulled_wait);  
		//pulled = false;
		pulled_wait = 0.0f;
		return;
	}
	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}

	bool win = false;
	{
		win = (mDirvec[mDirvec.size()-1].getScript().endResult.myResult == Result_Win);
	}
	HeroRankChallengeEndLayer *retLayer = HeroRankChallengeEndLayer::create(win);
	CCScene *runS = CCDirector::sharedDirector()->getRunningScene();
	runS->addChild(retLayer);
}

void ClientFightLayer::timeToDianFengDuiJue(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToDianFengDuiJue));
	if (pulled_wait)
	{
		schedule(schedule_selector(ClientFightLayer::timeToDianFengDuiJue),pulled_wait);  
		//pulled = false;
		pulled_wait = 0.0f;
		return;
	}
	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}

	GameDirector::getDirector()->goToTopDuelFight();
}

void ClientFightLayer::timeToJumpToZiYouJingji(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToJumpToZiYouJingji));
	if (pulled_wait)
	{
		schedule(schedule_selector(ClientFightLayer::timeToJumpToZiYouJingji),pulled_wait);  
		pulled_wait = 0.0f;
		return;
	}
	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}

//	GameDirector::getDirector()->mainScene(mainlayer_JingJiChang_challenge);
	FightEndBaseLayer *layer = FightEndBaseLayer::create(mDirvec.at(mDirvec.size()-1).getScript().endResult.myResult==Result_Win,
		FightEndBaseLayer::ComeFrom_Jingji_Type);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 130);
}
void ClientFightLayer::timeToJiXianShouCheng(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToJiXianShouCheng));
	if (pulled_wait)
	{
		schedule(schedule_selector(ClientFightLayer::timeToJiXianShouCheng),pulled_wait);  
		pulled_wait = 0.0f;
		return;
	}

	WorldBossCityGuardManager::getManager()->startCDTimeCounter();

	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}

//	GameDirector::getDirector()->goToJiXianShouChengFromFight();

	FightEndBaseLayer *layer = FightEndBaseLayer::create(mDirvec.at(mDirvec.size()-1).getScript().endResult.myResult==Result_Win,
		FightEndBaseLayer::ComeFrom_Jixianshoucheng_Type);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 130);
}

void ClientFightLayer::timeToChuanQiLu(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToChuanQiLu));
	if (pulled_wait)
	{
		schedule(schedule_selector(ClientFightLayer::timeToChuanQiLu),pulled_wait);  
		pulled_wait = 0.0f;
		return;
	}
	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}
	GameDirector::getDirector()->goToChuanQiLu(mDirvec[mDirvec.size()-1].getScript().endResult.gettedShenBingSuiPian);
}

void ClientFightLayer::timeToXueSeJiaoTangEndLayer(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToXueSeJiaoTangEndLayer));
	if (pulled_wait)
	{
		schedule(schedule_selector(ClientFightLayer::timeToXueSeJiaoTangEndLayer),pulled_wait);  
		pulled_wait = 0.0f;
		return;
	}
	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}


// 	XueSeJiaoTangEndLayer *retLayer = XueSeJiaoTangEndLayer::create(mDirvec[mDirvec.size()-1].getScript().endResult.myResult == Result_Win);
// 	CCScene *runS = CCDirector::sharedDirector()->getRunningScene();
// 	runS->addChild(retLayer, 130);
	FightEndBaseLayer *layer = FightEndBaseLayer::create(mDirvec.at(mDirvec.size()-1).getScript().endResult.myResult==Result_Win,
		FightEndBaseLayer::ComeFrom_Xuesejiaotang_Type);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 130);
}

void ClientFightLayer::timeToMapsLayer(float t)
{
	unschedule(schedule_selector(ClientFightLayer::timeToMapsLayer));
	if (pulled_wait)
	{
		schedule(schedule_selector(ClientFightLayer::timeToMapsLayer),pulled_wait);  
		pulled_wait = 0.0f;
		return;
	}
	if (mBundle.getBool("testMode",false))
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		return;
	}

//	GameDirector::getDirector()->mainScene(mainlayer_MaoXian);
	FightEndBaseLayer *layer = FightEndBaseLayer::create(mDirvec.at(mDirvec.size()-1).getScript().endResult.myResult==Result_Win,
		FightEndBaseLayer::ComeFrom_Shouhuzhiling_Type);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 130);
}

void ClientFightLayer::skill_screenShake()
{
	CCDirector::sharedDirector()->getRunningScene()->runAction(
		CCSpawn::create(
	CCSequence::create(
		CCDelayTime::create(0.1f),
		CCMoveBy::create(0.015f,ccp(1,-1)),
		CCMoveBy::create(0.015f,ccp(-1,-1)),
		CCMoveBy::create(0.015f,ccp(1,1)),
		CCMoveBy::create(0.015f,ccp(1,-1)),
		CCMoveBy::create(0.015f,ccp(-0.75,0.75)),
		CCMoveTo::create(0.01f,ccp(0,0)),
		NULL),
		NULL));
}

static CCPoint s_fightor_OrigPos, s_target_OrigPos;
void ClientFightLayer::userSkillPrepare(CCNode *node, void *data)
{
	StepSkill step = s_skillstep;
//	s_skillstep = step;
	float time = 0.1f;
	//1. 状态栏
	CCAction *action = CCSequence::create(CCFadeTo::create(time, 0), NULL);
	FX_CommonFunc::nodeAndAllChildRunAction((CCSprite*)getZhuangTaiBar(), (CCAction*)action->copy()->autorelease());
	for (int i=0; i<8; ++i)
	{
		CCSprite *headImg = (CCSprite*)this->getChildByTag(TAG_BloodHeroImgCD+i);
		if (headImg){
			FX_CommonFunc::nodeAndAllChildRunAction(headImg, (CCAction*)action->copy()->autorelease());
		}
	}
	//2. 与技能无关的英雄
	vector<ClientChessBoardItemBase*> heroVec;
	//target
	for (int i=0,count=step.targetUniqueVec.size(); i<count; ++i){
		heroVec.push_back(getChessBoardItem(step.targetUniqueVec[i]));
	}
	//fightor
	heroVec.push_back(getChessBoardItem(step.fightorUniqueId));
	//
	CCArray *arrA = boardButom->getInBoardItems();
	CCArray *arrB = boardTop->getInBoardItems();
	for (int i=0,count=arrA->count(); i<count; ++i)
	{
		ClientChessBoardItemBase* temp = dynamic_cast<ClientChessBoardItemBase*>(arrA->objectAtIndex(i));
		bool has = false;
		for (int j=0,jc=heroVec.size(); j<jc; ++j)
		{
			if (heroVec.at(j) == temp){
				has = true; break;
			}
		}
		if (!has) temp->runAction((CCAction*)action->copy()->autorelease());
	}
	for (int i=0,count=arrB->count(); i<count; ++i)
	{
		ClientChessBoardItemBase* temp = dynamic_cast<ClientChessBoardItemBase*>(arrB->objectAtIndex(i));
		bool has = false;
		for (int j=0,jc=heroVec.size(); j<jc; ++j)
		{
			if (heroVec.at(j) == temp){
				has = true; break;
			}
		}
		if (!has) temp->runAction((CCAction*)action->copy()->autorelease());
	}
	//3. mask layer
	CCLayerColor *mask = CCLayerColor::create(ccc4(0,0,0,0));
	mBg->addChild(mask, 1, 10);
	mask->setPosition(ccp(0, mBg->getContentSize().height-mask->getContentSize().height));
	mask->runAction(CCFadeTo::create(time, 140));

	//4. hero move
	CCPoint fightorPos, tagetPos;
	ClientChessBoardItemBase* fightor = heroVec.at(heroVec.size()-1);
	if (arrA->containsObject(fightor)){ //buttom
		fightorPos.y = boardButom->getGridPosInWorld(0,0).y;
		fightorPos.x = mWinSize.width/2;
		fightorPos = fightor->getParent()->convertToNodeSpace(fightorPos);
		tagetPos.y = boardTop->getGridPosInWorld(0,0).y;
		tagetPos.x = mWinSize.width/2;
		tagetPos = heroVec[0]->getParent()->convertToNodeSpace(tagetPos);
	}else{
		fightorPos.y = boardTop->getGridPosInWorld(0,0).y;
		fightorPos.x = mWinSize.width/2;
		fightorPos = fightor->getParent()->convertToNodeSpace(fightorPos);
		tagetPos.y = boardButom->getGridPosInWorld(0,0).y;
		tagetPos.x = mWinSize.width/2;
		tagetPos = heroVec[0]->getParent()->convertToNodeSpace(tagetPos);
	}

	CCPoint fightorOriPos = fightor->getPosition();
	CCPoint targetOriPos = heroVec[0]->getPosition();
	{
		fightorPos = fightor->getContainer()->getParent()->convertToNodeSpace(fightor->getParent()->convertToWorldSpace(fightorPos));
		s_fightor_OrigPos = fightorOriPos = fightor->getContainer()->getParent()->convertToNodeSpace(fightor->getParent()->convertToWorldSpace(fightorOriPos));
		tagetPos = heroVec[0]->getContainer()->getParent()->convertToNodeSpace(heroVec[0]->getParent()->convertToWorldSpace(tagetPos));
		s_target_OrigPos = targetOriPos = heroVec[0]->getContainer()->getParent()->convertToNodeSpace(heroVec[0]->getParent()->convertToWorldSpace(targetOriPos));
	}

	//己方加buf的不动 ---》 prepare里面设定
	if (step.skillType == SkillType_ShenZhiXingNian)
	{
		return;
	}

	float shakeNum = 0.15f;
	time = 0.2f;
	fightor->getContainer()->runAction(CCSequence::create(
		CCMoveTo::create(time*0.8f, ccp(fightorPos.x+(fightorPos.x-fightorOriPos.x)*shakeNum, fightorPos.y+(fightorPos.y-fightorOriPos.y)*shakeNum)),
//		CCMoveTo::create(time*0.1f, ccp(fightorPos.x-(fightorPos.x-fightorOriPos.x)*shakeNum, fightorPos.y-(fightorPos.y-fightorOriPos.y)*shakeNum)),
		CCMoveTo::create(time*0.1f, ccp(fightorPos.x, fightorPos.y)),
		NULL));
	if (heroVec.size() == 2 &&
		SkillConfigManager::getManager()->getSkillType(step.skillType) == 1) 
		heroVec[0]->getContainer()->runAction(CCSequence::create(
		CCMoveTo::create(time*0.8f, ccp(tagetPos.x+(tagetPos.x-targetOriPos.x)*shakeNum, tagetPos.y+(tagetPos.y-targetOriPos.y)*shakeNum)),
//		CCMoveTo::create(time*0.1f, ccp(tagetPos.x-(tagetPos.x-targetOriPos.x)*shakeNum, tagetPos.y-(tagetPos.y-targetOriPos.y)*shakeNum)),
		CCMoveTo::create(time*0.1f, ccp(tagetPos.x, tagetPos.y)),
		NULL));

}
void ClientFightLayer::useSkillEnd(CCObject *obj)
{
	StepSkill &step = s_skillstep;
	float delay = 0.2f;
	float time = 0.1f;
	//1. 状态栏
	CCAction *action = CCSequence::create(CCDelayTime::create(delay), CCFadeTo::create(time, 255), NULL);
	FX_CommonFunc::nodeAndAllChildRunAction((CCSprite*)getZhuangTaiBar(), (CCAction*)action->copy()->autorelease());
	for (int i=0; i<8; ++i)
	{
		CCSprite *headImg = (CCSprite*)this->getChildByTag(TAG_BloodHeroImgCD+i);
		if (headImg){
			FX_CommonFunc::nodeAndAllChildRunAction(headImg, (CCAction*)action->copy()->autorelease());
		}
	}
	//2. 与技能无关的英雄
	vector<ClientChessBoardItemBase*> heroVec;
	//target
	for (int i=0,count=step.targetUniqueVec.size(); i<count; ++i){
		heroVec.push_back(getChessBoardItem(step.targetUniqueVec[i]));
	}
	//fightor
	heroVec.push_back(getChessBoardItem(step.fightorUniqueId));
	//
	CCArray *arrA = boardButom->getInBoardItems();
	CCArray *arrB = boardTop->getInBoardItems();
	for (int i=0,count=arrA->count(); i<count; ++i)
	{
		ClientChessBoardItemBase* temp = dynamic_cast<ClientChessBoardItemBase*>(arrA->objectAtIndex(i));
		bool has = false;
		for (int j=0,jc=heroVec.size(); j<jc; ++j)
		{
			if (heroVec.at(j) == temp){
				has = true; break;
			}
		}
		if (!has) temp->runAction((CCAction*)action->copy()->autorelease());
	}
	for (int i=0,count=arrB->count(); i<count; ++i)
	{
		ClientChessBoardItemBase* temp = dynamic_cast<ClientChessBoardItemBase*>(arrB->objectAtIndex(i));
		bool has = false;
		for (int j=0,jc=heroVec.size(); j<jc; ++j)
		{
			if (heroVec.at(j) == temp){
				has = true; break;
			}
		}
		if (!has) temp->runAction((CCAction*)action->copy()->autorelease());
	}
	//3. mask layer
	CCLayerColor *mask = dynamic_cast<CCLayerColor*>(mBg->getChildByTag(10));
	if (mask) mask->runAction(CCSequence::create(CCFadeTo::create(time, 0), CCRemoveSelf::create(), NULL));

	//4. hero move
	ClientChessBoardItemBase* fightor = heroVec.at(heroVec.size()-1);
	CCPoint fightorPos, tagetPos;
	fightorPos = fightor->getNormalPos();
	tagetPos = heroVec[0]->getNormalPos();
	{
		fightorPos = s_fightor_OrigPos;//fightor->getContainer()->getParent()->convertToNodeSpace(fightor->getParent()->convertToWorldSpace(fightorPos));
		tagetPos = s_target_OrigPos;//heroVec[0]->getContainer()->getParent()->convertToNodeSpace(heroVec[0]->getParent()->convertToWorldSpace(tagetPos));
	}
	fightor->getContainer()->runAction(CCSequence::create(
		CCDelayTime::create(delay),
		CCMoveTo::create(time,fightorPos), NULL));
	if (heroVec.size() == 2 &&
		SkillConfigManager::getManager()->getSkillType(step.skillType) == 1) 
		heroVec[0]->getContainer()->runAction(CCSequence::create(
		CCDelayTime::create(delay),
		CCMoveTo::create(time,tagetPos), NULL));

}