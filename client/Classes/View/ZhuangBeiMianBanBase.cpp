#include "ZhuangBeiMianBanBase.h"
#include "EquipmentXiangQingBody.h"
#include "font.h"
#include "ChangeZhuangBeiBody.h"
#include "CommonDefine.h"
#include "GameDirector.h"
#include "LFMessageBox.h"
#include "CS_Common.h"
#include "SkillTree.h"
#include "SkillManager.h"
#include "History.h"
#include "GuideLayerBase.h"
#include "LevelUpLayer.h"
#include "ZhuangBeiMianBanYuanShen.h"
#include "ZhuangbeiGongMingManager.h"
#include "ZhuangbeiScanner.h"
#include "FX_CommonFunc.h"
#include "MessageDisplay.h"
#include "UnblockManager.h"
#include "FXToast.h"
#include "ZhuangbeiOptionLevel.h"
using namespace FX_CommonFunc;

#define  Tag_GongMingLayer  555

#define Label_Size 24

//第一次通关，主页配装教程
extern bool g_firstTaskEnd_to_changeEquip;
extern bool g_firstLearnSkillBook;

extern string getJianYingFileName(Profession prof, int pos, bool isHaveWuqi, bool isHaveDunpai, bool isSS_Wuq);

enum TAG{
	  Tag_MenuItem_HeroList

	  , Tag_MenuItem_Wuqi
	  , Tag_MenuItem_Dunpai
	  , Tag_MenuItem_Kuijia
	  , Tag_MenuItem_Hushengfu
	  , Tag_MenuItem_Jiezhi1
	  , Tag_MenuItem_Jiezhi2

	  , Tag_MenuItem_YuanShen
	  , Tag_MenuItem_ShengJi
	  , Tag_MenuItem_GongMing

	, Tag_DiWen_Wuqi
	, Tag_DiWen_Dunpai
	, Tag_DiWen_Kuijia
	, Tag_DiWen_Hushengfu
	, Tag_DiWen_Jiezhi1
	, Tag_DiWen_Jiezhi2
	, Tag_Diwen_Qianghua				//强化提示图标
// 	, Tag_JingLianBar_Wuqi
// 	, Tag_JingLianBar_Dunpai
// 	, Tag_JingLianBar_Kuijia
// 	, Tag_JingLianBar_Hushengfu
// 	, Tag_JingLianBar_Jiezhi1
// 	, Tag_JingLianBar_Jiezhi2
	, Tag_JingLianNum_Wuqi
	, Tag_JingLianNum_Dunpai
	, Tag_JingLianNum_Kuijia
	, Tag_JingLianNum_Hushengfu
	, Tag_JingLianNum_Jiezhi1
	, Tag_JingLianNum_Jiezhi2

	, Tag_Label_Xueliang
	, Tag_Label_Gongji
	, Tag_Label_Hujia
	, Tag_Label_Mokang

	, Tag_Label_Level
	, Tag_ProgressTimerBoarder
	, Tag_ProgressTimer
	, Tag_ExpNum


	, Tag_Img_HeroBg
	, Tag_Board_JiNengBianKuang
	, Tag_Img_FullHero

	, Tag_Board_ZhanDouLi

	, Tag_Img_Xilian
};

#define posWuqi ccp(95, 838-293)
#define posDunpai ccp(95, 838-549)
#define posHushengfu ccp(547, 838-436)
#define posJiezhi1 ccp(547, 838-526)
#define posJiezhi2 ccp(547, 838-615)
#define posKuijia ccp(547, 838-288)

//如果是 EquipClicked就有装备
//若是 AddEquipClicked就为NULL
Equipment *g_YuanShiBiJiao_Equip = NULL;

extern bool sortByActorPosition(BaseActor *const aa, BaseActor *const bb);

ZhuangBeiMianBanBase::ZhuangBeiMianBanBase()
	:m_pMinEquipemnt(NULL)
	,m_CurShowQianghuaDiwen(NULL)
{
	g_YuanShiBiJiao_Equip = NULL;
}

ZhuangBeiMianBanBase::~ZhuangBeiMianBanBase()
{
	g_YuanShiBiJiao_Equip = NULL;
}

ZhuangBeiMianBanBase* ZhuangBeiMianBanBase::create(int id, bool bShowQianghuaTipSpr /*= false*/)
{
	ZhuangBeiMianBanBase* layer = new ZhuangBeiMianBanBase;
	layer->m_bShowQianghuaTipSpr = bShowQianghuaTipSpr;
	if(layer && layer->init(id))
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

bool ZhuangBeiMianBanBase::init(int id)
{
	bool bRet = false;

	do {

		// hero
		mHeros = MyselfManager::getManager()->getMyZhuShous_ZhenShen(Actor_ShangZheng);
		BaseActor* zhujue = MyselfManager::getManager()->getMyZhuJueData();
		mHeros.insert(mHeros.begin(), zhujue);
		sort(mHeros.begin(), mHeros.end(), sortByActorPosition);
		for(int i=0; i!=mHeros.size(); ++i)
		{
			if(mHeros.at(i)->getId() == id)
			{
				// cur idx
				mCurHeroIdx = i;
				break;
			}
		}

		// 30级 还没元神的 获取元神
		for (int i=0; i!=mHeros.size(); ++i)
		{
			if (mHeros.at(i)->getCurrentProperty().level >= 30 && ! mHeros.at(i)->isYuanShen() && mHeros.at(i)->getMyYuanShen() == NULL)
			{
				CmdHelper::getHelper()->cmdQueryYuanshenOfHero(mHeros.at(i)->getId());
				GameDirector::getDirector()->hideWaiting();
			}
		}

		// cur hero equip
		mCurHeroEquips = MyselfManager::getManager()->getAllEquipmentsOnActor(mHeros.at(mCurHeroIdx)->getId(), Filter_Equipped);


		// 1. bg
		mBg = BaseSprite::create("renxing_bg.png");
		addChild(mBg);
		mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

		// 2.  hero menu
		mHeroMenu = CCMenu::create();
		mBg->addChild(mHeroMenu, 10);
		mHeroMenu->setPosition(CCPointZero);
		//
		mMainMenu = CCMenu::create();
		mBg->addChild(mMainMenu, 10);
		mMainMenu->setPosition(CCPointZero);
		{
			//升级
			CCMenuItemImage *shengJi = CCMenuItemImage::create(
				"shengji_anniu_1.png",
				"shengji_anniu_1_select.png",
				this, menu_selector(ZhuangBeiMianBanBase::menuItemClicked_MainMenu));
			mMainMenu->addChild(shengJi, 1, Tag_MenuItem_ShengJi);
			shengJi->setPosition(ccp(550, 838-699));
			if (mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_current < mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_need 
				|| mHeros.at(mCurHeroIdx)->getCurrentProperty().level >= 30)
			{
				shengJi->setVisible(false);
			}
			//元神
			CCMenuItemImage *yuanShen = CCMenuItemImage::create(
				"yuanshen_anniu.png",
				"yuanshen_anniu_select.png",
				this, menu_selector(ZhuangBeiMianBanBase::menuItemClicked_MainMenu));
			mMainMenu->addChild(yuanShen, 1, Tag_MenuItem_YuanShen);
			yuanShen->setPosition(ccp(550, 838-699));
			if (mHeros.at(mCurHeroIdx)->getMyYuanShen() == NULL)
			{
				yuanShen->setVisible(false);
			}
			//共鸣
			ZhuangbeiGongMingManager *manager = ZhuangbeiGongMingManager::getManager();
			//当前共鸣星级
			vector<EquipData> vec;
			for (int i=0; i!=mCurHeroEquips.size(); ++i)
				vec.push_back(mCurHeroEquips.at(i)->getEquipData());
			int curStarNum = manager->getGongMingStarNum(vec);
			int curLevel = manager->getGongMingLevel(curStarNum);
			int imgIdx = 1;
			if (curLevel == 0 || curLevel == 1) imgIdx = 1;
			else if (curLevel == 2 || curLevel == 3) imgIdx = 2;
			else if (curLevel == 4 || curLevel == 5 || curLevel == 6) imgIdx = 3;
			else if (curLevel == 7) imgIdx = 4;
			else if (curLevel == 8) imgIdx = 5;
			else if (curLevel == 9) imgIdx = 6;
			else if (curLevel == 10) imgIdx = 7;
			CCString temp;
			temp.initWithFormat("gongming_anniu_%d.png", imgIdx);
			CCMenuItemImage *gmItem = CCMenuItemImage::create(
				temp.getCString(),
				temp.getCString(),
				this, menu_selector(ZhuangBeiMianBanBase::menuItemClicked_MainMenu));
			mMainMenu->addChild(gmItem, 1, Tag_MenuItem_GongMing);
			gmItem->setPosition(ccp(222, 838-200));
			
			gmItem->runAction(
				CCRepeatForever::create(
					CCSequence::create(
						CCScaleTo::create(0.5f, 1.2f), 
						CCScaleTo::create(0.5f, 1.0f), 
						CCDelayTime::create(1.0f),
					NULL))); 
		}

		// 3. equip menu
// 		this->setMenuTouchedDelayTime(2);
// 		mEquipMenu = MyMenu::create(this);
		mEquipMenu = CCMenu::create();
		mBg->addChild(mEquipMenu, 10);
		mEquipMenu->setPosition(CCPointZero);

		// 
		mHeroView = NULL;

		// 4. hero menu item
		for(int i=0; i!=mHeros.size(); ++i)
		{
			BaseSprite* touxiangBg = BaseSprite::create("renxing_renwu_touxiang_bg.png");
			mBg->addChild(touxiangBg);
			touxiangBg->setPosition(ccp(109+140*i, 838-83));


			ImageData data = ImageManager::getManager()->getImageData_ProfessionHead(
				(Profession)mHeros[i]->getActorType());
			BaseSprite* pic = BaseSprite::create(data.getCurrentImageFile());
			touxiangBg->addChild(pic);
			pic->setPosition(ccp(touxiangBg->getContentSize().width/2, touxiangBg->getContentSize().height/2));


			CCMenuItemImage* item;
			if(i == mCurHeroIdx)
			{
				item = CCMenuItemImage::create(
					"renxing_renwu_touxiang_select.png"
					, "renxing_renwu_touxiang_select.png"
					, this, menu_selector(ZhuangBeiMianBanBase::menuItemHeroListClicked));
			}else
			{
				item = CCMenuItemImage::create(
					"renxing_renwu_touxiang.png"
					, "renxing_renwu_touxiang.png"
					, this, menu_selector(ZhuangBeiMianBanBase::menuItemHeroListClicked));
			}
			mHeroMenu->addChild(item, 1, Tag_MenuItem_HeroList+i);
			item->setPosition(touxiangBg->getPosition());

		}

		// 5. equip
		refreshEquipmentUI();

// 		char buf[20];
// 		//xueliang
// 		sprintf(buf, "%.1f", mHeros.at(mCurHeroIdx)->getXueliang());
// 		CCLabelTTF* xueliang = CCLabelTTF::create(buf, fontStr_kaiti, 26);
// 		mBg->addChild(xueliang, 1, Tag_Label_Xueliang);
// 		xueliang->setAnchorPoint(ccp(0,0.5));
// 		xueliang->setPosition(ccp(84, 838-783));
// 		//gongji
// 		PropertyCalculator prop;
// 		sprintf(buf, "%.1f",prop.calculateGongJiLi_MacroValue(mHeros.at(mCurHeroIdx)->getCalculateData(), Range_Average));
// 		CCLabelTTF* gongji = CCLabelTTF::create(buf, fontStr_kaiti, 26);
// 		mBg->addChild(gongji, 1, Tag_Label_Gongji);
// 		gongji->setAnchorPoint(ccp(0,0.5));
// 		gongji->setPosition(ccp(236, 838-783));
// 		//hujia
// 		sprintf(buf, "%.1f", prop.calculateHuJiaZhi_MacroValue(mHeros.at(mCurHeroIdx)->getCalculateData()));
// 		CCLabelTTF* hujia = CCLabelTTF::create(buf, fontStr_kaiti, 26);
// 		mBg->addChild(hujia, 1, Tag_Label_Hujia);
// 		hujia->setAnchorPoint(ccp(0,0.5));
// 		hujia->setPosition(ccp(386, 838-783));
// 		//mokang
// 		sprintf(buf, "%.1f", mHeros.at(mCurHeroIdx)->getMokang());
// 		CCLabelTTF* mokang = CCLabelTTF::create(buf, fontStr_kaiti, 26);
// 		mBg->addChild(mokang, 1, Tag_Label_Mokang);
// 		mokang->setAnchorPoint(ccp(0,0.5));
// 		mokang->setPosition(ccp(538, 838-783));
// 		//lv
// 		sprintf(buf, "Lv%d", mHeros.at(mCurHeroIdx)->getCurrentProperty().level);
// 		CCLabelTTF* lv = CCLabelTTF::create(buf, fontStr_kaiti, 28);
// 		mBg->addChild(lv, 11, Tag_Label_Level);
// 		lv->setAnchorPoint(ccp(0,0.5));
// 		lv->setPosition(ccp(192, 838-580));
// 
// 		{
// 			float curExp = mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_current;
// 			float needExp = mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_need;
// 			BaseSprite* jinYanBarBoarder = BaseSprite::create("zhuye_jinyan_bg.png");
// 			mBg->addChild(jinYanBarBoarder, 1, Tag_ProgressTimerBoarder);
// 			jinYanBarBoarder->setPosition(ccp(319, lv->getPositionY()));
// 
// 			CCProgressTimer* jinYanBar = LF::createProgressTimer(BaseSprite::create("zhuye_jinyan.png"));
// 			mBg->addChild(jinYanBar, 1, Tag_ProgressTimer);
// 			jinYanBar->setPosition(jinYanBarBoarder->getPosition());
// 			jinYanBar->setPercentage(curExp/needExp * 100.0f);
// 
// 			if (mHeros.at(mCurHeroIdx)->getCurrentProperty().level >= 30)
// 			{
// 				jinYanBarBoarder->setVisible(false);
// 				jinYanBar->setVisible(false);
// 			}
// 			else
// 			{
// 				jinYanBarBoarder->setVisible(true);
// 				jinYanBar->setVisible(true);
// 			}
// 		}

// 		BaseSprite *menuBar = BaseSprite::create("renxing_anniu_bg.png");
// 		mBg->addChild(menuBar, 9);
// 		menuBar->setPosition(ccp(320, 838-209));
		// 盖住 hero scroll 
		BaseSprite* img = BaseSprite::create("renxing_renwu_biankuang.png");
		mBg->addChild(img, 10, Tag_Img_HeroBg);
		img->setPosition(ccp(319, 838-410));

		//技能
		BaseSprite *jinengBoard = BaseSprite::create("renxing_jineng_bg.png");
		mBg->addChild(jinengBoard, 11, Tag_Board_JiNengBianKuang);
		jinengBoard->setPosition(ccp(321, 838-624));

		SkillType type;
		if (mHeros.at(mCurHeroIdx)->getCurrentUseSkill() != NULL)
			type = (SkillType)mHeros.at(mCurHeroIdx)->getCurrentUseSkill()->getInfor()->skill_type;
		else
		{
			type = (SkillType)1;
			CCLog("Error: hero has not [current use skill] !");
		}
		//pic
		BaseSprite *pic = BaseSprite::create(ImageManager::getManager()->getSkillPicFileName(type));
		pic->setScale(0.4f);
		jinengBoard->addChild(pic);
		pic->setPosition(ccp(34, 33));
		//jineng
		CCString jinengStr;
		jinengStr.initWithFormat("%s%s+%d", LFStrings::getValue("jineng").c_str()
			, SkillTree::sharedSkillTree()->getSkillNameByType(type).c_str(), mHeros.at(mCurHeroIdx)->getCurrentUseSkill()->getInfor()->skill_level);
		CCLabelTTF *jinengLabel = CCLabelTTF::create(jinengStr.getCString(), fontStr_kaiti, 20);
		jinengLabel->setColor(ccc3(195, 145, 96));
		jinengBoard->addChild(jinengLabel);
		jinengLabel->setPosition(ccp(162, 42));
		//周期
		int chuFaDian = mHeros.at(mCurHeroIdx)->getCurrentSkillTurn();
		int chuFaZhouQi = mHeros.at(mCurHeroIdx)->getCurrentSkillZhouQi();
		for (int i=0; i!=chuFaZhouQi; ++i)
		{
			BaseSprite *img = NULL;
			if(i == chuFaDian-1)
				img = BaseSprite::create("jineng_chufalunci_icon_chufa.png");
			else 
				img = BaseSprite::create("jineng_chufalunci_icon.png");
			jinengBoard->addChild(img);
			img->setPosition(ccp(162-img->getContentSize().width*chuFaZhouQi + i*img->getContentSize().width*2, 20));
		}

		// 6. preperty
		refreshPrepertyLabel();
		// 6. hero scroll
//		addHeroScrollView();
		//由 initFooter调用



		bRet = true;
	} while (0);


	return bRet;
}
void ZhuangBeiMianBanBase::IsChuanqi(CCNode *item)
{
	GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstChuanQiShiPin);
	CCPoint pos = item->getParent()->convertToWorldSpace(item->getPosition());
	GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
	guideLayer->init(CCSizeMake(item->getContentSize().width * item->getScaleX(),item->getContentSize().height * item->getScaleY()), pos
		, LFStrings::getValue("first_chuanqi_shuoming"), NULL, false);
	CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	//}
}
void ZhuangBeiMianBanBase::refreshEquipmentUI()
{
	mCurHeroEquips = MyselfManager::getManager()->getAllEquipmentsOnActor(mHeros.at(mCurHeroIdx)->getId(), Filter_Equipped);
	//好装提示
	TestReport actReport = ZhuangbeiScanner::getScanner()->getTestReportByActor(mHeros.at(mCurHeroIdx));

	// 0wuqi  1dunpai  2kaijia  3hushengfu  4jiezhi1  5jiezhi2
	bool isHaveEquip[6];
	for(int i=0; i!=6; ++i)  isHaveEquip[i] = false;

	bool bShuangShou_Wuqi = false;

	// . hero equip menu item
	int jiezhi = 0;
	string xxx;
	mBg->removeChildByTag(Tag_JingLianNum_Dunpai);
//	mBg->removeChildByTag(Tag_JingLianBar_Dunpai);
	mBg->removeChildByTag(Tag_JingLianNum_Hushengfu);
//	mBg->removeChildByTag(Tag_JingLianBar_Hushengfu);
	mBg->removeChildByTag(Tag_JingLianNum_Jiezhi1);
//	mBg->removeChildByTag(Tag_JingLianBar_Jiezhi1);
	mBg->removeChildByTag(Tag_JingLianNum_Jiezhi2);
//	mBg->removeChildByTag(Tag_JingLianBar_Jiezhi2);
	mBg->removeChildByTag(Tag_JingLianNum_Kuijia);
//	mBg->removeChildByTag(Tag_JingLianBar_Kuijia);
	mBg->removeChildByTag(Tag_JingLianNum_Wuqi);
//	mBg->removeChildByTag(Tag_JingLianBar_Wuqi);

	for (vector<ZhuangbeiOptionLevel *>::iterator iter = m_lOptionList.begin() ; iter != m_lOptionList.end() ; iter ++)
	{
		(*iter)->removeFromParent();
	}
	m_lOptionList.clear();

	if (m_bShowQianghuaTipSpr)
	{
		m_pMinEquipemnt = getMinEquipment();
	}

	for(int i=0; i!=mCurHeroEquips.size(); ++i)
	{
		
		// 1.底纹
		ImageData diwenData;
		BaseSprite* diwen;
		int menuItemTag = -1;
		int TAG_Place; //use to menu select image
		switch (mCurHeroEquips.at(i)->getType())
		{
		case ZhuangbeiType_Dunpai:
		case ZhuangbeiType_Faqi:
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_DunPai,
				mCurHeroEquips.at(i)->getEquipColor(), 
				false,//mCurHeroEquips.at(i)->isShenBing(),
				mCurHeroEquips.at(i)->isPartOfSuit(),
				mCurHeroEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Dunpai);
			diwen->setPosition(posDunpai);
			diwen->setScale(diwenData.scale);
			menuItemTag = Tag_MenuItem_Dunpai;
			TAG_Place = TAG_DunPai;
			isHaveEquip[1] = true;
			//强化和精炼
			if (mCurHeroEquips.at(i)->canBeJingLian(xxx) && mCurHeroEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurHeroEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Dunpai);
//				mBg->addChild(icon, 20, Tag_JingLianBar_Dunpai);
//				icon->setPositionY(387);
				lableNum->setPosition(ccp(155,387));
//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}

			if (mCurHeroEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurHeroEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		case ZhuangbeiType_Hushenfu:
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_HuShenFu,
				mCurHeroEquips.at(i)->getEquipColor(),
				false,//mCurHeroEquips.at(i)->isShenBing(),
				mCurHeroEquips.at(i)->isPartOfSuit(),
				mCurHeroEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Hushengfu);
			diwen->setScale(diwenData.scale);
			diwen->setPosition(posHushengfu);
			menuItemTag = Tag_MenuItem_Hushengfu;
			TAG_Place = TAG_HuShenFu;
			isHaveEquip[3] = true;
			//强化和精炼
			if (mCurHeroEquips.at(i)->canBeJingLian(xxx) && mCurHeroEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurHeroEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Hushengfu);
//				mBg->addChild(icon, 20, Tag_JingLianBar_Hushengfu);
//				icon->setPositionY(432);
				lableNum->setPosition(ccp(583,432));
//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}
			if (mCurHeroEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurHeroEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		case ZhuangbeiType_Jiezhi:
			++jiezhi;
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_JieZhi,
				mCurHeroEquips.at(i)->getEquipColor(), 
				false,//mCurHeroEquips.at(i)->isShenBing(),
				mCurHeroEquips.at(i)->isPartOfSuit(),
				mCurHeroEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Jiezhi1+jiezhi-1);
			diwen->setScale(diwenData.scale);
			TAG_Place = TAG_JieZhi;
			if(jiezhi == 1)
			{
				diwen->setPosition(posJiezhi1);
				menuItemTag = Tag_MenuItem_Jiezhi1;
				isHaveEquip[4] = true;
				//强化和精炼
				if (mCurHeroEquips.at(i)->canBeJingLian(xxx) && mCurHeroEquips.at(i)->getCurrentJingLianJieShu() >= 1)
				{
//					BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
					char temp[16];
					sprintf(temp,"+%d",mCurHeroEquips.at(i)->getCurrentJingLianJieShu());
					CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
					lableNum->setAnchorPoint(ccp(1.0f,0.5f));
					mBg->addChild(lableNum, 20, Tag_JingLianNum_Jiezhi1);
//					mBg->addChild(icon, 20, Tag_JingLianBar_Jiezhi1);
//					icon->setPositionY(340);
					lableNum->setPosition(ccp(583,340));
//					icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
				}
			}
			else 
			{
				diwen->setPosition(posJiezhi2);
				menuItemTag = Tag_MenuItem_Jiezhi2;
				isHaveEquip[5] = true;
				//强化和精炼
				if (mCurHeroEquips.at(i)->canBeJingLian(xxx) && mCurHeroEquips.at(i)->getCurrentJingLianJieShu() >= 1)
				{
//					BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
					char temp[16];
					sprintf(temp,"+%d",mCurHeroEquips.at(i)->getCurrentJingLianJieShu());
					CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
					lableNum->setAnchorPoint(ccp(1.0f,0.5f));
					mBg->addChild(lableNum, 20, Tag_JingLianNum_Jiezhi2);
//					mBg->addChild(icon, 20, Tag_JingLianBar_Jiezhi2);
//					icon->setPositionY(250);
					lableNum->setPosition(ccp(583,250));
//					icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
				}
			}
			if (mCurHeroEquips.at(i)->canPurify(xxx,1))
			{
				
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurHeroEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		case ZhuangbeiType_KuijiaQin:
		case ZhuangbeiType_KuijiaZhong:
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_Kuijia,
				mCurHeroEquips.at(i)->getEquipColor(), 
				false,//mCurHeroEquips.at(i)->isShenBing(),
				mCurHeroEquips.at(i)->isPartOfSuit(),
				mCurHeroEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Kuijia);
			diwen->setScale(diwenData.scale);
			diwen->setPosition(posKuijia);
			menuItemTag = Tag_MenuItem_Kuijia;
			TAG_Place = TAG_Kuijia;
			isHaveEquip[2] = true;
			//强化和精炼
			if (mCurHeroEquips.at(i)->canBeJingLian(xxx) && mCurHeroEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurHeroEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Kuijia);
//				mBg->addChild(icon, 20, Tag_JingLianBar_Kuijia);
//				icon->setPositionY(626);
				lableNum->setPosition(ccp(605,625));
//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}
			if (mCurHeroEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurHeroEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		case ZhuangbeiType_DanshouJian:
		case ZhuangbeiType_ShuangshouBishou:
		case ZhuangbeiType_ShuangshouFu:
		case ZhuangbeiType_DanshouFazhang:
		case ZhuangbeiType_Gong:
		case ZhuangbeiType_ShuangshouFazhang:
			//wuqi
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_WuQi,
				mCurHeroEquips.at(i)->getEquipColor(), 
				false,//mCurHeroEquips.at(i)->isShenBing(),
				mCurHeroEquips.at(i)->isPartOfSuit(),
				mCurHeroEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Wuqi);
			diwen->setScale(diwenData.scale);
			diwen->setPosition(posWuqi);
			menuItemTag = Tag_MenuItem_Wuqi;
			TAG_Place = TAG_WuQi;
			isHaveEquip[0] = true;
			//强化和精炼
			if (mCurHeroEquips.at(i)->canBeJingLian(xxx) && mCurHeroEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurHeroEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Wuqi);
//				mBg->addChild(icon, 20, Tag_JingLianBar_Wuqi);
//				icon->setPositionY(643);
				lableNum->setPosition(ccp(154,643));
//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}

			//fushou 
			if(CS::isShuangShouWuQi(mCurHeroEquips.at(i)->getType()))
			{
				bShuangShou_Wuqi = true;
				//底纹
				BaseSprite* fushouDiwen = BaseSprite::create("mianban_renwu_zhuangbei_fushou_diban.png");
				mBg->addChild(fushouDiwen, 1, Tag_DiWen_Dunpai);
				fushouDiwen->setScale(diwenData.scale);
				fushouDiwen->setPosition(posDunpai);
				//装备
				BaseSprite* fushouZhuangBei = ImageManager::getManager()->getSpriteOfZhuangbei(mCurHeroEquips.at(i));
				fushouDiwen->addChild(fushouZhuangBei);
				fushouZhuangBei->setColor(ccc3(185,117,117));
				fushouZhuangBei->setPosition(ccp(fushouDiwen->getContentSize().width/2, fushouDiwen->getContentSize().height/2));
				fushouZhuangBei->setScale(ImageManager::getManager()->getImageData_ZhuangBei_MianBan(mCurHeroEquips.at(i)).scale);
				//按钮
				CCSprite* imgNormal = CCSprite::create(
					ImageManager::getManager()->getImageData_DiWen_Selected(
					mCurHeroEquips.at(i)->getEquipColor(),
					false,//mCurHeroEquips.at(i)->isShenBing(),
					mCurHeroEquips.at(i)->isPartOfSuit()).getCurrentImageFile().c_str());
				imgNormal->setOpacity(0);
				CCSprite* imgSelected = CCSprite::create(
					ImageManager::getManager()->getImageData_DiWen_Selected(
					mCurHeroEquips.at(i)->getEquipColor(),
					false,//mCurHeroEquips.at(i)->isShenBing(),
					mCurHeroEquips.at(i)->isPartOfSuit()
					).getCurrentImageFile().c_str());
				imgSelected->setOpacity(30);
				CCMenuItemSprite* menuItem = NULL;
				switch (mHeros.at(mCurHeroIdx)->getActorType())
				{
				case Profession_Yemanren:
				case Profession_Chike:
				case Profession_Fashi:
					menuItem = CCMenuItemSprite::create(
						imgNormal
						, imgSelected
						, this, menu_selector(ZhuangBeiMianBanBase::menuItemAddEquipClicked));
					break;
				default:
					menuItem = CCMenuItemSprite::create(
						imgNormal
						, imgSelected
						, this, menu_selector(ZhuangBeiMianBanBase::menuItemEquipClicked));
					break;

				}
				mEquipMenu->addChild(menuItem, 1, Tag_MenuItem_Dunpai);
				menuItem->setPosition(fushouDiwen->getPosition());
				menuItem->setScaleX(fushouDiwen->getContentSize().width / menuItem->getContentSize().width);
				menuItem->setScaleY(fushouDiwen->getContentSize().height / menuItem->getContentSize().height);
				menuItem->setUserData(mCurHeroEquips.at(i));

				isHaveEquip[1] = true;  

			}
			if (mCurHeroEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurHeroEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		default:
			CCLog("Error: ZhuangBeiMianBanBase::refreshEquipmentUI ---> default zhuangbei type " );
			break;
		}

		// 2.装备
		ImageData equipData = ImageManager::getManager()->getImageData_ZhuangBei_MianBan(mCurHeroEquips.at(i));
		BaseSprite* zhuangbei = ImageManager::getManager()->getSpriteOfZhuangbei(mCurHeroEquips.at(i),false,true);
		zhuangbei->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
		zhuangbei->setScale(equipData.scale);
		diwen->addChild(zhuangbei);

		// 3.按钮
		CCSprite* imgNormal = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
			TAG_Place,
			mCurHeroEquips.at(i)->getEquipColor(),
			false,//mCurHeroEquips.at(i)->isShenBing(),
			mCurHeroEquips.at(i)->isPartOfSuit()
			).getCurrentImageFile().c_str());
		imgNormal->setOpacity(0);
		CCSprite* imgSelected = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
			TAG_Place,
			mCurHeroEquips.at(i)->getEquipColor(),
			false,//mCurHeroEquips.at(i)->isShenBing(),
			mCurHeroEquips.at(i)->isPartOfSuit()
			).getCurrentImageFile().c_str());
		CCMenuItemSprite* menuItem = CCMenuItemSprite::create(
			imgNormal
			, imgSelected
			, this, menu_selector(ZhuangBeiMianBanBase::menuItemEquipClicked));
		mEquipMenu->addChild(menuItem, 1, menuItemTag);
		menuItem->setPosition(diwen->getPosition());
		menuItem->setScaleX(diwen->getContentSize().width / menuItem->getContentSize().width);
		menuItem->setScaleY(diwen->getContentSize().height / menuItem->getContentSize().height);
		menuItem->setUserData(mCurHeroEquips.at(i));


		//好装备提  枚举所有旧装备
		menuItem->getNormalImage()->setUserData((void*)0);
		//有当前英雄的report
		if (actReport.reportList.size() > 0 && actReport.reportList.at(0).actor == mHeros.at(mCurHeroIdx))
		{
			bool bMarked = false;
			vector<Equipment*> oldEqVec = actReport.reportList.at(0).oldEquipVec;
			for (int iidx=0; iidx<oldEqVec.size(); ++iidx)
			{
				if (bMarked) break;
				if (oldEqVec.at(iidx) == NULL) continue;
				EquipRequirePlace place = oldEqVec.at(iidx)->getCurrentPlace();
				//如果没有忽略
				if (place == mCurHeroEquips.at(i)->getCurrentPlace()
					&& ! ZhuangbeiScanner::getScanner()->isIgnoreHint(actReport.reportList.at(0).actor, place))
				{
					//当前这个装备 ---》 已经有更高属性的装备了     在底纹上加提示
					BaseSprite *tishiImg = BaseSprite::create("renxing_zhuangbeitisheng.png");
					menuItem->addChild(tishiImg, 10);
					if (mCurHeroEquips.at(i)->getType() == ZhuangbeiType_Hushenfu || mCurHeroEquips.at(i)->getType() == ZhuangbeiType_Jiezhi)
						tishiImg->setScale(0.7f);
					tishiImg->setPosition(ccp(diwen->getContentSize().width-tishiImg->getContentSize().width/2*0.7f+4,
						tishiImg->getContentSize().height/2*0.7f-4));
					menuItem->getNormalImage()->setUserData((void*)1);
					break;
					//
					bMarked = true;
					break;
				}
			}
		}
		//if (mCurHeroEquips.at(i)->getEquipColor() == ZhuangbeiColour_Chuanqi && mCurHeroEquips.at(i)->canPurify(xxx,1))
		//{
		//	//IsChuanqi(diwen);
		//}

		if (m_pMinEquipemnt && m_pMinEquipemnt == mCurHeroEquips.at(i))
		{
			if (diwen)
			{
				m_CurShowQianghuaDiwen = diwen;
				BaseSprite* pQianghuaTipSpr = BaseSprite::create("zhandou_tishi_qianghua.png");
				diwen->addChild(pQianghuaTipSpr);
				pQianghuaTipSpr->setTag(Tag_Diwen_Qianghua);
				pQianghuaTipSpr->setPosition(ccp(diwen->getContentSize().width*diwen->getScaleX()-pQianghuaTipSpr->getContentSize().width/2, pQianghuaTipSpr->getContentSize().height/2));
			}
		}
	}

	// . 没有装备的地方
	for(int i=0; i!=6; ++i)
	{// 0wuqi  1dunpai  2kaijia  3hushengfu  4jiezhi1  5jiezhi2
		if(isHaveEquip[i])
			continue;

		BaseSprite *jianyingImg = NULL;
		int menuItemTag = -1;
		int diwen_tag = -1;
		CCPoint pos;
		CCSize size;
		EquipRequirePlace zbPlace;//装备类型

		int TAG_Place;
		switch (i)
		{
		case 0:
			menuItemTag = Tag_MenuItem_Wuqi;
			pos = posWuqi;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
					TAG_WuQi,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(mHeros.at(mCurHeroIdx)->getCurrentProperty().profession
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Wuqi;
			TAG_Place = TAG_WuQi;
			zbPlace = POS_ZhuWuQi;
			break;
		case 1:
			menuItemTag = Tag_MenuItem_Dunpai;
			pos = posDunpai;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_DunPai,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(mHeros.at(mCurHeroIdx)->getCurrentProperty().profession
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Dunpai;
			TAG_Place = TAG_DunPai;
			zbPlace = POS_FuShou;
			break;
		case 2:
			menuItemTag = Tag_MenuItem_Kuijia;
			pos = posKuijia;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_Kuijia,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(mHeros.at(mCurHeroIdx)->getCurrentProperty().profession
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Kuijia;
			TAG_Place = TAG_Kuijia;
			zbPlace = POS_XiongJia;
			break;
		case 3:
			menuItemTag = Tag_MenuItem_Hushengfu;
			pos = posHushengfu;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_HuShenFu,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(mHeros.at(mCurHeroIdx)->getCurrentProperty().profession
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Hushengfu;
			TAG_Place = TAG_HuShenFu;
			zbPlace = POS_Hushenfu;
			break;
		case 4:
			menuItemTag = Tag_MenuItem_Jiezhi1;
			pos = posJiezhi1;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_JieZhi,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(mHeros.at(mCurHeroIdx)->getCurrentProperty().profession
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Jiezhi1;
			TAG_Place = TAG_JieZhi;
			zbPlace = POS_JieZhi1;
			break;
		case 5:
			menuItemTag = Tag_MenuItem_Jiezhi2;
			pos = posJiezhi2;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_JieZhi,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(mHeros.at(mCurHeroIdx)->getCurrentProperty().profession
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Jiezhi2;
			TAG_Place = TAG_JieZhi;
			zbPlace = POS_JieZhi2;
			break;
		default:
			break;
		}

		//装备剪影
		mBg->addChild(jianyingImg, 1, diwen_tag);
		jianyingImg->setPosition(pos);

		//按钮
		CCSprite* imgNormal = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
				TAG_Place,ZhuangbeiColour_Fumo,false,false).getCurrentImageFile().c_str());
		imgNormal->setOpacity(0);
		CCSprite* imgSelected = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
				TAG_Place,ZhuangbeiColour_Fumo,false,false).getCurrentImageFile().c_str());
		imgSelected->setOpacity(0);
		CCMenuItemSprite* menuItem = CCMenuItemSprite::create(
			imgNormal
			, imgSelected
			, this, menu_selector(ZhuangBeiMianBanBase::menuItemAddEquipClicked));
		mEquipMenu->addChild(menuItem, 1, menuItemTag);
		menuItem->setPosition(pos);
		menuItem->setScaleX(size.width / menuItem->getContentSize().width);
		menuItem->setScaleY(size.height / menuItem->getContentSize().height);

		//好装备提  没装备的地方也要提示
		menuItem->getNormalImage()->setUserData((void*)0);
		if (actReport.reportList.size() > 0 && actReport.reportList.at(0).actor == mHeros.at(mCurHeroIdx)
			&& ! ZhuangbeiScanner::getScanner()->isIgnoreHint(actReport.reportList.at(0).actor, zbPlace))
		{
			bool has = false;
			for (int idx=0; idx!=actReport.reportList.at(0).betterEquipVec.size(); ++idx) 
			{
				for (int xx=0; xx!=actReport.reportList.at(0).betterEquipVec.at(idx)->getRequiredPlaces().size(); ++xx)
				{
					if (actReport.reportList.at(0).betterEquipVec.at(idx)->getRequiredPlaces().at(xx) == zbPlace)
					{//当前这个装备 ---》 已经有更高属性的装备了     在底纹上加提示
						BaseSprite *tishiImg = BaseSprite::create("renxing_zhuangbeitisheng.png");
						menuItem->addChild(tishiImg, 10);
						if (actReport.reportList.at(0).betterEquipVec.at(idx)->getType() == ZhuangbeiType_Hushenfu 
							|| actReport.reportList.at(0).betterEquipVec.at(idx)->getType() == ZhuangbeiType_Jiezhi)
							tishiImg->setScale(0.7f);
						tishiImg->setPosition(ccp(menuItem->getContentSize().width-tishiImg->getContentSize().width/2*0.7f+4,
							tishiImg->getContentSize().height/2*0.7f-4));
						menuItem->getNormalImage()->setUserData((void*)1);
						has = true;
						break;
					}
				}
				if (has) break;
			}
		}
	}

}

Equipment* ZhuangBeiMianBanBase::getMinEquipment()
{
	Equipment* pEquipment = NULL;
	
	std::map<int, Equipment*> ds;
	std::map <int, Equipment*>::iterator m1_Iter;

	int iMin = 0;
	for (int i = 0; i < mCurHeroEquips.size(); ++i)
	{
		int total = 0;
		pEquipment = mCurHeroEquips.at(i);
		EquipType type = pEquipment->getEquipType();
		if (type == EquipType_WuQi)
		{
			for(int j=0; j!= pEquipment->getEquipData().qianghua_data.qianghua_count; ++j)
			{
				total += pEquipment->getEquipData().qianghua_data.info_arr[i].percent;
			}
			if (i == 0)
			{
				iMin = total;
				ds.insert(std::map<int,Equipment*>::value_type(iMin, pEquipment));
			}
			else
			{
				if (total <= iMin)
				{
					iMin = total;
					ds.insert(std::map<int,Equipment*>::value_type(iMin, pEquipment));
				}
			}
		}
		else if (type == EquipType_FangJu)
		{
			ZhuangbeiType type = mCurHeroEquips.at(i)->getType();
			if (type == ZhuangbeiType_KuijiaQin || type == ZhuangbeiType_KuijiaZhong)
			{
				for(int j=0; j!= pEquipment->getEquipData().qianghua_data.qianghua_count; ++j)
				{
					total += pEquipment->getEquipData().qianghua_data.info_arr[i].percent;
				}

				if (i == 0)
				{
					iMin = total;
					ds.insert(std::map<int,Equipment*>::value_type(iMin, pEquipment));
				}
				else
				{
					if (total <= iMin)
					{
						iMin = total;
						if (ds.size() >= 2)
						{
							m1_Iter = ++ds.begin();
							if (ds.begin()->first == m1_Iter->first && ds.begin()->second->getEquipType() != EquipType_WuQi)
							{
								ds.insert(std::map<int,Equipment*>::value_type(iMin, pEquipment));
							}
						}
					}
				}
			}
		}
	}

	if (ds.size() > 0)
	{
		pEquipment = ds.begin()->second;
	}
	else
	{
		pEquipment = NULL;
	}
	
	return pEquipment;
}

void ZhuangBeiMianBanBase::refreshPrepertyLabel()
{
	BaseActor *actor = mHeros.at(mCurHeroIdx);

	char buf[20];
	//xueliang
	sprintf(buf, "%.1f", actor->getXueliang());
	CCLabelTTF* xueliang = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Xueliang));
	if (xueliang == NULL)
	{
		xueliang = CCLabelTTF::create("", fontStr_kaiti, Label_Size);
		mBg->addChild(xueliang, 1, Tag_Label_Xueliang);
		xueliang->setAnchorPoint(ccp(0,0.5));
		xueliang->setPosition(ccp(116, 838-783));
	}
	xueliang->setString(buf);
	//gongji
	PropertyCalculator prop;
	sprintf(buf, "%.1f", actor->getGongji_MacroValue());
	CCLabelTTF* gongji = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Gongji));
	if (gongji == NULL)
	{
		gongji = CCLabelTTF::create("", fontStr_kaiti, Label_Size);
		mBg->addChild(gongji, 1, Tag_Label_Gongji);
		gongji->setAnchorPoint(ccp(0,0.5));
		gongji->setPosition(ccp(306, 838-783));
	}
	gongji->setString(buf);
	//hujia
	sprintf(buf, "%.1f", prop.calculateHuJiaZhi_MacroValue(actor->getCalculateData()));
	CCLabelTTF* hujia = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Hujia));
	if (hujia == NULL)
	{
		hujia = CCLabelTTF::create("", fontStr_kaiti, Label_Size);
		mBg->addChild(hujia, 1, Tag_Label_Hujia);
		hujia->setAnchorPoint(ccp(0,0.5));
		hujia->setPosition(ccp(496, 838-783));
	}
	hujia->setString(buf);
// 	//mokang
// 	sprintf(buf, "%.1f", actor->getMokang());
// 	CCLabelTTF* mokang = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Mokang));
// 	if (mokang == NULL)
// 	{
// 		mokang = CCLabelTTF::create("", fontStr_kaiti, 26);
// 		mBg->addChild(mokang, 1, Tag_Label_Mokang);
// 		mokang->setAnchorPoint(ccp(0,0.5));
// 		mokang->setPosition(ccp(538, 838-783));
// 	}
// 	mokang->setString(buf);
	//lv
	sprintf(buf, "Lv%d", actor->getCurrentProperty().level);
	CCLabelTTF* lv = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Level));
	if (lv == NULL)
	{
		lv = CCLabelTTF::create(buf, fontStr_kaiti, Label_Size);
		mBg->addChild(lv, 11, Tag_Label_Level);
		lv->setAnchorPoint(ccp(0,0.5));
		lv->setPosition(ccp(192, 838-560));
	}
	lv->setString(buf);

	CCProgressTimer* jinYanBar = (CCProgressTimer*)mBg->getChildByTag(Tag_ProgressTimer);
	BaseSprite* jinYanBarBoarder = (BaseSprite*)mBg->getChildByTag(Tag_ProgressTimerBoarder);
	if (jinYanBar == NULL && jinYanBarBoarder == NULL)
	{
		jinYanBarBoarder = BaseSprite::create("zhuye_jinyan_bg.png");
		mBg->addChild(jinYanBarBoarder, 10, Tag_ProgressTimerBoarder);
		jinYanBarBoarder->setPosition(ccp(lv->getPositionX()+jinYanBarBoarder->getContentSize().width/2-4, 838-582));

		jinYanBar = LF::createProgressTimer(BaseSprite::create("zhuye_jinyan.png"));
		mBg->addChild(jinYanBar, 11, Tag_ProgressTimer);
		jinYanBar->setPosition(jinYanBarBoarder->getPosition());
	}
	{
		float curExp = mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_current;
		float needExp = mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_need;
		jinYanBar->setPercentage(curExp/needExp * 100.0f);
		if (mHeros.at(mCurHeroIdx)->getCurrentProperty().level >= 30)
		{
			jinYanBarBoarder->setVisible(false);
			jinYanBar->setVisible(false);
			lv->setPositionY(838-576);
			mBg->removeChildByTag(Tag_ExpNum);
		}
		else
		{
			jinYanBarBoarder->setVisible(true);
			jinYanBar->setVisible(true);
			lv->setPositionY(838-560);
			//num
			{
				mBg->removeChildByTag(Tag_ExpNum);
				CCString temp;
				temp.initWithFormat("%u/%u", mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_current
					, mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_need);
				CCLabelTTF *tiliLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 15);
				//tiliLabel->setOpacity(255/2);
				//tiliLabel->setAnchorPoint(ccp(0, 0.5));
				tiliLabel->setPosition(this->convertToNodeSpace(
					//jinYanBarBoarder->convertToWorldSpace(ccp(8, 11))));
					jinYanBarBoarder->convertToWorldSpace(ccp(jinYanBarBoarder->getContentSize().width/2, jinYanBarBoarder->getContentSize().height/2))));
				mBg->addChild(tiliLabel, 12, Tag_ExpNum);
			}
		}
	}
	//升级
	if (mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_current < mHeros.at(mCurHeroIdx)->getCurrentProperty().exp_need
		|| mHeros.at(mCurHeroIdx)->getCurrentProperty().level >= 30)
	{
		mMainMenu->getChildByTag(Tag_MenuItem_ShengJi)->setVisible(false);
	}
	else
	{
		mMainMenu->getChildByTag(Tag_MenuItem_ShengJi)->setVisible(true);
	}
	//元神
	if (mHeros.at(mCurHeroIdx)->getMyYuanShen() == NULL)
	{
		mMainMenu->getChildByTag(Tag_MenuItem_YuanShen)->setVisible(false);
	}
	else
	{
		mMainMenu->getChildByTag(Tag_MenuItem_YuanShen)->setVisible(true);
	}
	//共鸣
	CCMenuItemImage *gongmingItem = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_GongMing);
	ZhuangbeiGongMingManager *manager = ZhuangbeiGongMingManager::getManager();
	//当前共鸣星级
	vector<EquipData> vec;
	for (int i=0; i!=mCurHeroEquips.size(); ++i)
		vec.push_back(mCurHeroEquips.at(i)->getEquipData());
	int curStarNum = manager->getGongMingStarNum(vec);
	int curLevel = manager->getGongMingLevel(curStarNum);
	int imgIdx = 1;
	if (curLevel == 0 || curLevel == 1) imgIdx = 1;
	else if (curLevel == 2 || curLevel == 3) imgIdx = 2;
	else if (curLevel == 4 || curLevel == 5 || curLevel == 6) imgIdx = 3;
	else if (curLevel == 7) imgIdx = 4;
	else if (curLevel == 8) imgIdx = 5;
	else if (curLevel == 9) imgIdx = 6;
	else if (curLevel == 10) imgIdx = 7;
	CCString temp;
	temp.initWithFormat("gongming_anniu_%d.png", imgIdx);
	gongmingItem->initWithNormalImage(temp.getCString(),temp.getCString(),
		NULL, this, menu_selector(ZhuangBeiMianBanBase::menuItemClicked_MainMenu));
	if (curLevel == 0)
		gongmingItem->setVisible(false);
	else 
		gongmingItem->setVisible(true);

	//技能
	BaseSprite *jinengBoard = (BaseSprite*)mBg->getChildByTag(Tag_Board_JiNengBianKuang);
	jinengBoard->removeAllChildren();
	SkillInfo *pSkillInfo = mHeros.at(mCurHeroIdx)->getCurrentUseSkill()->getInfor();
	SkillType type = (SkillType)pSkillInfo->skill_type;
	//pic
	BaseSprite *pic = BaseSprite::create(ImageManager::getManager()->getSkillPicFileName(type));
	pic->setScale(0.4f);
	jinengBoard->addChild(pic);
	pic->setPosition(ccp(34, 33));
	//jineng
	CCString jinengStr;
	if (pSkillInfo->skill_level >= 2)
		jinengStr.initWithFormat("%s+%d", SkillTree::sharedSkillTree()->getSkillNameByType(type).c_str(), int(pSkillInfo->skill_level - 1));
	else 
		jinengStr = SkillTree::sharedSkillTree()->getSkillNameByType(type);
	CCLabelTTF *jinengLabel = CCLabelTTF::create(jinengStr.getCString(), fontStr_kaiti, 20);
	jinengLabel->setColor(ccc3(195, 145, 96));
	jinengBoard->addChild(jinengLabel);
	jinengLabel->setPosition(ccp(162, 42));
	//周期
	int chuFaDian = mHeros.at(mCurHeroIdx)->getCurrentSkillTurn();
	int chuFaZhouQi = mHeros.at(mCurHeroIdx)->getCurrentSkillZhouQi();
	for (int i=0; i!=chuFaZhouQi; ++i)
	{
		BaseSprite *img = NULL;
		if(i == chuFaDian-1)
			img = BaseSprite::create("jineng_chufalunci_icon_chufa.png");
		else 
			img = BaseSprite::create("jineng_chufalunci_icon.png");
		jinengBoard->addChild(img);
		img->setPosition(ccp(162-img->getContentSize().width*chuFaZhouQi + i*img->getContentSize().width*2, 20));
	}

	//战斗力
	BaseSprite *zdlBoard = (BaseSprite*)mBg->getChildByTag(Tag_Board_ZhanDouLi);
	if (zdlBoard == NULL)
	{
		zdlBoard = BaseSprite::create("renxing_zhandouli_bg.png");
		mBg->addChild(zdlBoard, 20, Tag_Board_ZhanDouLi);
		zdlBoard->setPosition(ccp(320, mBg->getContentSize().height - 702));
	}
	zdlBoard->removeAllChildren();
	BaseSprite *zdlLabel = BaseSprite::create("renxing_zhandouli.png");
	zdlBoard->addChild(zdlLabel);
	zdlLabel->setPosition(ccp(57, 29));
	sprintf(buf, "%d", (int)actor->getMyZhanDouLi(Range_Average));
	CCLabelTTF *zdlL = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
	zdlL->setAnchorPoint(ccp(0, 0.5));
	zdlBoard->addChild(zdlL);
	zdlL->setPosition(ccp(94, zdlLabel->getPositionY()));
}

void ZhuangBeiMianBanBase::refreshHerosImgScrollView()
{
	mHeroView->prepareToDisplay();
	mHeroView->scrollToPageSimple(mCurHeroIdx, 0);
}

void ZhuangBeiMianBanBase::addHeroScrollView()
{
	mHeroView = new GridScrollView(
		CCSizeMake(288,531),
		mHeros.size(),	// 元素的个数
		1,								// 每页横向显示几个
		1,								// 每页纵向显示几个
		10.0f,
		CCPointZero,
		20.0f);


	mBg->addChild(mHeroView, 5);
	mHeroView->release();
	mHeroView->setClickableCreater(this);
	// 可以显示了
	mHeroView->prepareToDisplay();
	mHeroView->setPosition(ccp(178, 838-660));
	mHeroView->scrollToPageSimple(mCurHeroIdx, 0);

}

ClickableItem* ZhuangBeiMianBanBase::createGridItem(int pageIndex,int xIndex,int yIndex)
{
	ScrollHeroNode* node = new ScrollHeroNode(
		ImageManager::getManager()->getFullPhotoOfActor(mHeros.at(pageIndex)), mHeros.at(pageIndex)->getId(), mFooterLayer);
	node->autorelease();
	node->setScale(1.2f);
	return node;
}
// 页面选择了
void ZhuangBeiMianBanBase::pageSelectedEnd(int pageIndex)
{
	menuItemHeroListClicked( mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+pageIndex) );
}

void ZhuangBeiMianBanBase::menuItemHeroListClicked(CCObject* pSender)
{
	CCMenuItemImage* oldMenuItem = (CCMenuItemImage*)mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+mCurHeroIdx);
	oldMenuItem->initWithNormalImage(
		"renxing_renwu_touxiang.png"
		, "renxing_renwu_touxiang.png"
		, NULL, this, menu_selector(ZhuangBeiMianBanBase::menuItemHeroListClicked));

	mCurHeroIdx = ((CCMenuItemImage*)pSender)->getTag() - Tag_MenuItem_HeroList;
	CCMenuItemImage* curMenuItem = (CCMenuItemImage*)mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+mCurHeroIdx);
	curMenuItem->initWithNormalImage(
		"renxing_renwu_touxiang_select.png"
		, "renxing_renwu_touxiang_select.png"
		, NULL, this, menu_selector(ZhuangBeiMianBanBase::menuItemHeroListClicked));

	mBg->removeChildByTag(Tag_DiWen_Wuqi);
	mBg->removeChildByTag(Tag_DiWen_Hushengfu);
	mBg->removeChildByTag(Tag_DiWen_Jiezhi1);
	mBg->removeChildByTag(Tag_DiWen_Jiezhi2);
	mBg->removeChildByTag(Tag_DiWen_Kuijia);
	mBg->removeChildByTag(Tag_DiWen_Dunpai);
	mEquipMenu->removeAllChildren();
	refreshEquipmentUI();
	refreshPrepertyLabel();
	mHeroView->scrollToPageSimple(mCurHeroIdx, 0);


	//共鸣
	teachGuide();
}

void ZhuangBeiMianBanBase::menuItemEquipClicked(CCObject* pSender)
{
	Equipment* equip = (Equipment*)((CCMenuItemImage*)pSender)->getUserData();
	g_YuanShiBiJiao_Equip = equip;

	if ((int)((CCMenuItemSprite*)pSender)->getNormalImage()->getUserData() == 1) //点击了有装备提示的槽位
	{
		int tag = ((CCMenuItemSprite*)pSender)->getTag();
		EquipRequirePlace place;
		switch (tag)
		{
		case Tag_MenuItem_Wuqi:
			place = POS_ZhuWuQi;
			break;
		case Tag_MenuItem_Dunpai:
			place = POS_FuShou;
			break;
		case Tag_MenuItem_Kuijia:
			place = POS_XiongJia;
			break;
		case Tag_MenuItem_Hushengfu:
			place = POS_Hushenfu;
			break;
		case Tag_MenuItem_Jiezhi1:
			place = POS_JieZhi1;
			break;
		case Tag_MenuItem_Jiezhi2:
			place = POS_JieZhi2;
			break;
		default:
			break;
		}
		ChangeZhuangBeiBody* change = new ChangeZhuangBeiBody(
			mHeros.at(mCurHeroIdx)->getId(), place);
		mFooterLayer->popUpContaintLayer(change);
		change->setFooter(mFooterLayer);

		change->release();
	}
	else
	{
		EquipmentXiangQingBody* q = new EquipmentXiangQingBody(equip, equip->getActorId(), From_RenXingMianBan);
		mFooterLayer->popUpContaintLayer(q, Pop_Pos_WondowDown);
		q->release();
	}

	if (m_pMinEquipemnt && m_CurShowQianghuaDiwen && m_pMinEquipemnt == equip)
	{
		CCNode* pDiwen = m_CurShowQianghuaDiwen->getChildByTag(Tag_Diwen_Qianghua);
		if (pDiwen)
		{
			m_bShowQianghuaTipSpr = false;
			pDiwen->removeFromParent();
		}
	}
}

void ZhuangBeiMianBanBase::menuItemAddEquipClicked(CCObject* pSender)
{

	g_YuanShiBiJiao_Equip = NULL;
	int tag = ((CCMenuItemSprite*)pSender)->getTag();
	EquipRequirePlace place;
	switch (tag)
	{
	case Tag_MenuItem_Wuqi:
		place = POS_ZhuWuQi;
		break;
	case Tag_MenuItem_Dunpai:
		place = POS_FuShou;
		break;
	case Tag_MenuItem_Kuijia:
		place = POS_XiongJia;
		break;
	case Tag_MenuItem_Hushengfu:
		place = POS_Hushenfu;
		break;
	case Tag_MenuItem_Jiezhi1:
		place = POS_JieZhi1;
		break;
	case Tag_MenuItem_Jiezhi2:
		place = POS_JieZhi2;
		break;
	default:
		break;
	}
	ChangeZhuangBeiBody* change = new ChangeZhuangBeiBody(
		mHeros.at(mCurHeroIdx)->getId(), place);
	mFooterLayer->popUpContaintLayer(change);
	change->setFooter(mFooterLayer);

	change->release();

}

void ZhuangBeiMianBanBase::disableAllTouchBegin()
{
	mHeroMenu->setEnabled(false);
	mEquipMenu->setEnabled(false);
	mHeroView->setTouchEnabled(false);
	mMainMenu->setEnabled(false);
}

void ZhuangBeiMianBanBase::disableAllTouchEnd()
{
	refreshLayer();

	mHeroMenu->setEnabled(true);
	mEquipMenu->setEnabled(true);
	mMainMenu->setEnabled(true);
	if (mHeroView != NULL)
	{
		mHeroView->setTouchEnabled(true);
	}


	//////////////////////////////////////////////////////////////////
	if(isUI_Guide_MainLine && g_firstTaskEnd_to_changeEquip)
	{
		//
		g_firstTaskEnd_to_changeEquip = false;
		//
		// 4. 人物详细属性
// 		GuideLayerBase *guideLayer4 = new GuideLayer_SimpleLayer;
// 		CCNode *hero = mBg->getChildByTag(Tag_Img_HeroBg);
// 		guideLayer4->init(CCSize(hero->getContentSize().width, hero->getContentSize().height-69)
// 			, hero->getParent()->convertToWorldSpace(ccp(hero->getPositionX(), hero->getPositionY()+69/2))
// 			, LFStrings::getValue("renxingmianban_xingxishuxing"), NULL, false);
// 		// 3. 人物当前技能
// 		GuideLayerBase *guideLayer3 = new GuideLayer_SimpleLayer;
// 		CCNode *jineng = mBg->getChildByTag(Tag_Board_JiNengBianKuang);
// 		guideLayer3->init(jineng->getContentSize(), jineng->getParent()->convertToWorldSpace(jineng->getPosition())
// 			, LFStrings::getValue("renxingmianban_jineng"), guideLayer4, true);
// 		// 2. 4个基本属性
// 		GuideLayer_RenXingMianBan_Special *guideLayer2 = new GuideLayer_RenXingMianBan_Special;
// 		vector<string> strVec;
// 		strVec.push_back(LFStrings::getValue("shengMin"));
// 		strVec.push_back(LFStrings::getValue("gongJi"));
// 		strVec.push_back(LFStrings::getValue("huJia"));
// 		strVec.push_back(LFStrings::getValue("moKang"));
// 		guideLayer2->init(CCSize(640-30,58), mBg->convertToWorldSpace(ccp(mBg->getContentSize().width/2, 838-783)), strVec, guideLayer3, true);
// 		// 1. 基本属性
// 		GuideLayerBase *guideLayer1 = new GuideLayer_SimpleLayer;
// 		guideLayer1->init(CCSize(640-30,58), mBg->convertToWorldSpace(ccp(mBg->getContentSize().width/2, 838-783))
// 			, LFStrings::getValue("renxingmianban_jichushuxing"), guideLayer2, true);

		
	}

}

void ZhuangBeiMianBanBase::LevelUpEndCallBackFunc()
{
	refreshLayer();

	///////////////升级触发的 新玩法提示   
	if (g_ChuanQiFuBen_Opend) //传奇副本
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_ChuanQiFuBenKaiQi"), true,false,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	}
	else if (g_YingXiongBang_Opend)  //英雄榜
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_YingXiongBangKaiQi"), true,false,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	}
	else if (g_JingJiChang_Opend) // 竞技场
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("xinWanFa_JingJiChangKaiQi"), true,false,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	}
	else if (g_JiuGuan_Opend) //酒馆
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("JiuGuanKaiqi"), true,false,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	}
	else if (g_XueSeJiaoTang_Opend) //血色教堂
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("TiShi_XueSeJiaoTang"), true, true,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	}
	else if (g_JiXianShouCheng_Opend)//极限守城
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("TiShi_JiXianShouCheng"), true, true,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	}
	else if (g_DianFengDuiJue_Opend)//巅峰对决
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("TiShi_DianFengDuiJue"), true, true,true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, this->getZOrder()+1);
	}
}


void ZhuangBeiMianBanBase::refreshLayer()
{
	menuItemHeroListClicked( mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+mCurHeroIdx) );
// 	this->refreshEquipmentUI();
// 	this->refreshPrepertyLabel();
}


void ZhuangBeiMianBanBase::setChangeZhuangBei(TempEquipZhuangbei data, Equipment* equip)
{
	mChangeEquip = equip;
	addObserver(SEL_CallFuncO(&ZhuangBeiMianBanBase::changeZhuangBeiRsp),MSG_equipZhuangbeiRsp);
	// 网络请求
	mCmdHlp->cmdEquipZhuangBei(data);
}

void ZhuangBeiMianBanBase::changeZhuangBeiRsp(CCObject* msg_rsp)
{
	removeObserver(MSG_equipZhuangbeiRsp);
	SPCmd_EquipZhuangbeiRsp* rsp = (SPCmd_EquipZhuangbeiRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		GameDirector::getDirector()->hideWaiting();

		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		//LFMessageBox::alert(LF::getString("cuowu"),LF::getString("huanzhuangshibai"));
		return;
	}

	// 数据层会做装备的脱+装
	//// 脱装备的数据
	//{
	//	if (mChangeEquip->getActorId() != INVALID_ID)
	//	{
	//		MyselfManager::getManager()->removeEquipementFromOwner(mChangeEquip);
	//	}
	//}
	//// 移除需要替换的，并装上新的
	//{
	//	BaseActor* actor = mHeros.at(mCurHeroIdx);
	//	CCAssert(actor!=NULL,"");
	//	// 移除需要被替换掉的
	//	vector<Equipment*> needTakeOff = actor->getNeedTakeOffIfWareOnEqui(mChangeEquip->getType());
	//	int takeOffNum = needTakeOff.size();
	//	for (int i=0;i<takeOffNum;i++)
	//	{
	//		MyselfManager::getManager()->removeEquipementFromOwner(needTakeOff[i]);
	//	}
	//	// 装上
	//	MyselfManager::getManager()->attachEquipment(mChangeEquip,actor->getId());
	//}

	refreshHerosImgScrollView();
	refreshLayer();
	GameDirector::getDirector()->hideWaiting();
	CCLog("reportChangeZhuangBeiOK ----> Success");
}

void ZhuangBeiMianBanBase::removeZhuangBeiRsp(CCObject* msg_rsp)
{
	removeObserver(MSG_equipZhuangbeiRsp);
	SPCmd_EquipZhuangbeiRsp* rsp = (SPCmd_EquipZhuangbeiRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		GameDirector::getDirector()->hideWaiting();
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		//reportError(NULL);
		return;
	}

	// 脱装备的数据
	{
		if (mChangeEquip->getActorId() != INVALID_ID)
		{
			MyselfManager::getManager()->removeEquipementFromOwner(mChangeEquip);
		}
	}

	refreshHerosImgScrollView();
	refreshLayer();
	GameDirector::getDirector()->hideWaiting();
}

void ZhuangBeiMianBanBase::menuItemLongTimeTouched(CCObject* pSender)
{
	CCLog("============================long touch===========================");


	Equipment* equip = (Equipment*)((CCMenuItemImage*)pSender)->getUserData();
	BaseActor* actor = mHeros.at(mCurHeroIdx);

	if(equip == NULL) //没有装备的地方
		return;
	
	// 不在自己身上？！！
	if (equip->getActorId() != actor->getId())
	{
		CCLog("Error: ZhuangBeiMianBanBase::menuItemLongTimeTouched ---> current hero is not the equip owner");
		refreshLayer();
		return;
	}


	TempEquipZhuangbei requestData;
	// 脱装备的数据
	{
		if (equip->getActorId() != INVALID_ID)
		{
			TempHeroAndZhuangbei heroData;
			heroData.hero_id = actor->getId();
			int equipNum = actor->getEquipments().size();
			for (int i=0;i<equipNum;i++)
			{
				// 要脱掉的就不要加入
				if (actor->getEquipments()[i]->getEquipId() == equip->getEquipId())
				{
					continue;
				}

				heroData.zb_id_arr.push_back(actor->getEquipments()[i]->getEquipId());
			}
			requestData.hero_zb_arr.push_back(heroData);
		}
		else
		{
			CCLog("Error: ZhuangBeiMianBanBase::menuItemLongTimeTouched ---> equip id error!");
			refreshLayer();
			return;
		}
	}
	
	mChangeEquip = equip;
	// 监听 和发送
	addObserver(SEL_CallFuncO(&ZhuangBeiMianBanBase::removeZhuangBeiRsp),MSG_equipZhuangbeiRsp);
	// 网络请求
	mCmdHlp->cmdEquipZhuangBei(requestData);
}

void ZhuangBeiMianBanBase::onEnter()
{
	BaseLayer::onEnter();
	teachGuide();
}

void ZhuangBeiMianBanBase::teachGuide()
{
	
	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstJinXingTongGuan))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstJinXingTongGuan);
		//GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstJinXingTongGuan);
		CCMenuItem* item = NULL;
		CCMenuItem* item2= NULL;
		CCMenuItem* item3= NULL;
		//5 换衣服  
		item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Kuijia);
		GuideLayerBase *guideLayer5 = new GuideLayer_MultLightAndLabel;
		guideLayer5->setChatEndBack(this,SEL_CallFunc(&ZhuangBeiMianBanBase::HuanZhuangTeach));

		guideLayer5->init(CCSize(item->getContentSize().width*item->getScaleX(), item->getContentSize().height*item->getScaleY())
			, item->getParent()->convertToWorldSpace(item->getPosition())
			, LFStrings::getValue("renxingmianban_huanYiFu"), NULL, false);
		//4 护身符
		/*
		GuideLayerBase *guideLayer4 = new GuideLayer_MultLightAndLabel;
		guideLayer4->init(CCSize(item->getContentSize().width*item->getScaleX(), item->getContentSize().height*item->getScaleY())
			, item->getParent()->convertToWorldSpace(item->getPosition())
			, LFStrings::getValue("renxingmianban_huShenFu"), guideLayer5, true);

			*/
		//3 戒指
		item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Jiezhi1);
		item2 = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Jiezhi2);
		item3 = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Hushengfu);


		GuideLayerBase *guideLayer3 = new GuideLayer_MultLightAndLabel;
		LightMaskPos temp1, temp2,temp3;
		temp1.info = LFStrings::getValue("renxingmianban_jieZhi");
		temp1.pos  = item->getParent()->convertToWorldSpace(item->getPosition());
		temp1.size = CCSize(item->getContentSize().width*(item->getScaleX()), item->getContentSize().height*(item->getScaleY()));
		temp2.info = LFStrings::getValue("renxingmianban_jieZhi");
		temp2.pos  = item2->getParent()->convertToWorldSpace(item2->getPosition());
		temp2.size = CCSize(item2->getContentSize().width*(item2->getScaleX()), item2->getContentSize().height*(item2->getScaleY()));

		temp3.info = LFStrings::getValue("renxingmianban_huShenFu");
		temp3.pos  = item3->getParent()->convertToWorldSpace(item3->getPosition());
		temp3.size = CCSize(item3->getContentSize().width*(item3->getScaleX()), item3->getContentSize().height*(item3->getScaleY()));
		vector<LightMaskPos> vec;
		vec.push_back(temp1);
		vec.push_back(temp2);
		vec.push_back(temp3);
		guideLayer3->init(vec, guideLayer5, true);
		//2 副手
		item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Dunpai);
		item2 = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Wuqi);

		//LightMaskPos temp1, temp2;
		temp1.info = LFStrings::getValue("renxingmianban_fuShou");
		temp1.pos  = item->getParent()->convertToWorldSpace(item->getPosition());
		temp1.size = CCSize(item->getContentSize().width*(item->getScaleX()), item->getContentSize().height*(item->getScaleY()));
		temp2.info = LFStrings::getValue("renxingmianban_wuqi");
		temp2.pos  = item2->getParent()->convertToWorldSpace(item2->getPosition());
		temp2.size = CCSize(item2->getContentSize().width*(item2->getScaleX()), item2->getContentSize().height*(item2->getScaleY()));
		//vector<LightMaskPos> vec;
		vec.clear();
		vec.push_back(temp1);
		vec.push_back(temp2);
		GuideLayerBase *guideLayer1 = new GuideLayer_MultLightAndLabel;
		guideLayer1->init(vec, guideLayer3, true);
		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer1, 1024);
		return;
	}
	

	//技能教学
	
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_RenWuShuXing))
	{
		//GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_RenWuShuXing);
		if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJinXingTongGuan))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_RenWuShuXing);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_RenWuShuXing);
			// 4. 人物详细属性
			GuideLayerBase *guideLayer4 = new GuideLayer_SimpleLayer;
			CCNode *hero = mBg->getChildByTag(Tag_Img_HeroBg);
			guideLayer4->init(CCSize(hero->getContentSize().width, hero->getContentSize().height-69)
				, hero->getParent()->convertToWorldSpace(ccp(hero->getPositionX(), hero->getPositionY()+69/2))
				, LFStrings::getValue("renxingmianban_xingxishuxing"), NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer4, 1024);
			return;
		}
	}


	//共鸣教学说明
	//当前共鸣星级
	vector<EquipData> vec;
	for (int i=0; i!=mCurHeroEquips.size(); ++i)
		vec.push_back(mCurHeroEquips.at(i)->getEquipData());
	int curStarNum = ZhuangbeiGongMingManager::getManager()->getGongMingStarNum(vec);
	int curLevel = ZhuangbeiGongMingManager::getManager()->getGongMingLevel(curStarNum);
	if (curLevel >= 1)
	{
		CCMenuItem *item = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_GongMing);
	
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongMingLevel))
		{
			if (item)
			{
				GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongMingLevel);
				GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongMingLevel);
				GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
				guideLayer->init(
					item->getContentSize()
					, item->getParent()->convertToWorldSpace(item->getPosition())
					, LFStrings::getValue("renXing_GongMingXiaoGuo"), NULL, false);

				CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
				return;
			}
		}
	}


	//第一次在面板看到传奇装备
	
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstChuanQi))
	{
		//当前是否有传奇装备
		for (int i = 0; i < mCurHeroEquips.size(); ++i)
		{
			if (mCurHeroEquips.at(i)->getEquipColor() == ZhuangbeiColour_Chuanqi)
			{
				
				int nextLv = CS::getNextShenBingLevel(mCurHeroEquips.at(i)->getLevel()); //-1就是满级了
				if (nextLv == -1)//顶级神兵会出现死循环 如果出现顶级神兵就不要出该教学了
				{
					GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstChuanQi);
					break;
				}

				CCMenuItem *item = NULL;
				EquipRequirePlace place = mCurHeroEquips.at(i)->getCurrentPlace();
				switch (place)
				{
				case POS_Hushenfu:
					item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Hushengfu);
					break;
				case POS_XiongJia:
					item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Kuijia);
					break;
				case POS_JieZhi1:
					item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Jiezhi1);
					break;
				case POS_JieZhi2:
					item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Jiezhi2);
					break;
				case POS_ZhuWuQi:
					item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Wuqi);
					break;
				case POS_FuShou:
					item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Dunpai);
					break;
				default:
					break;
				}

				if (item)
				{
					GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstChuanQi);
					GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstChuanQi);
					GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
					guideLayer->init(
						item->getContentSize(), 
						item->getParent()->convertToWorldSpace(item->getPosition()), 
						LFStrings::getValue("chuanqi_chuanQiZhuangKeChengZhang"), NULL, false);
					CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
					return;
				}
			}
		}
	}
	else 
	{
		
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstChuanQiShiPin))
		{
			for (int i = 0; i < mCurHeroEquips.size(); ++i)
			{
				if (mCurHeroEquips.at(i)->getEquipColor() == ZhuangbeiColour_Chuanqi)
				{

					string xxx;
					if (mCurHeroEquips.at(i)->getEquipColor() == ZhuangbeiColour_Chuanqi && mCurHeroEquips.at(i)->canPurify(xxx,1))
					{
						
						CCMenuItem *item = NULL;
						EquipRequirePlace place = mCurHeroEquips.at(i)->getCurrentPlace();
						switch (place)
						{
						case POS_Hushenfu:
							item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Hushengfu);
							break;

						case POS_JieZhi1:
							item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Jiezhi1);
							break;
						case POS_JieZhi2:
							item = (CCMenuItem*)mEquipMenu->getChildByTag(Tag_MenuItem_Jiezhi2);
							break;

						default:
							break;
						}
						if (item)
						{
							GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstChuanQiShiPin);
							IsChuanqi(item);
							break;
						}
					}
				}
			}
		}
	}
}

void ZhuangBeiMianBanBase::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_ShengJi)
	{
		LevelUpLayer *layer = LevelUpLayer::create(mHeros.at(mCurHeroIdx)->getId(), this);
		if (layer)
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 150);
	}
	else if (tag == Tag_MenuItem_YuanShen)
	{
		CCArray *yuanshenActor = CCArray::create();
		for (int i=0; i!=mHeros.size(); ++i)
		{
			if (mHeros.at(i)->getMyYuanShen() != NULL)
				yuanshenActor->addObject(mHeros.at(i)->getMyYuanShen());
		}
		ZhuangBeiMianBanYuanShen *layer = ZhuangBeiMianBanYuanShen::create(yuanshenActor, mHeros.at(mCurHeroIdx)->getMyYuanShen()->getId()
			, comefrom_base, this);
		mFooterLayer->popUpContaintLayer(layer);
	}
	else if (tag == Tag_MenuItem_GongMing)
	{
		//
		this->disableAllTouchBegin();
		//
		BaseSprite *layer = BaseSprite::create("zhandou_jiugongge_touming.png");
		layer->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 128, Tag_GongMingLayer);
		//
		CCMenu *menu = CCMenu::create();
		layer->addChild(menu);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *item = CCMenuItemImage::create(
			"zhandou_jiugongge_touming.png",
			"zhandou_jiugongge_touming.png",
			this, menu_selector(ZhuangBeiMianBanBase::menuItemClicked_BackFromGongMing));
		menu->addChild(item);
		item->setOpacity(0);
		item->setPosition(ccp(layer->getContentSize().width/2, layer->getContentSize().height/2));


		CCString temp;
		ZhuangbeiGongMingManager *manager = ZhuangbeiGongMingManager::getManager();
		//
		BaseSprite *board = BaseSprite::create("gongming_bg.png");
		board->setPosition(ccp(layer->getContentSize().width/2, layer->getContentSize().height/2));
		layer->addChild(board, 10);
		//当前共鸣星级
		vector<EquipData> vec;
		for (int i=0; i!=mCurHeroEquips.size(); ++i)
			vec.push_back(mCurHeroEquips.at(i)->getEquipData());
		int curStarNum = manager->getGongMingStarNum(vec);
		int curLevel = manager->getGongMingLevel(curStarNum);
		temp.initWithFormat("%s: %s%s(%d%s)", LFStrings::getValue("GongMing_CurStar").c_str(), getStrByInt(curLevel).c_str()
			, LFStrings::getValue("ji").c_str(), curStarNum, LFStrings::getValue("xing").c_str());
		CCLabelTTF *curStarLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 24);
		curStarLabel->setAnchorPoint(ccp(0, 0.5f));
		curStarLabel->setPosition(ccp(48, 304-53));
		curStarLabel->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Xiyou, false, false));
		board->addChild(curStarLabel);
		//下一阶共鸣需求星级
		temp.initWithFormat("%s: %u%s", LFStrings::getValue("GongMing_NextStar").c_str()
			, manager->getGongMingConfig().levelNeedStarNumConfig[curLevel + 1], LFStrings::getValue("xing").c_str());
		CCLabelTTF *nextStarLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 21);
		nextStarLabel->setAnchorPoint(ccp(0, 0.5f));
		nextStarLabel->setPosition(ccp(curStarLabel->getPositionX(), 304-85));
		board->addChild(nextStarLabel);
		//共鸣效果
		CCLabelTTF *gmxgLabel = CCLabelTTF::create(LFStrings::getValue("GongMing_Function").c_str(), fontStr_kaiti, 21);
		gmxgLabel->setAnchorPoint(ccp(0, 0.5f));
		gmxgLabel->setPosition(ccp(curStarLabel->getPositionX(), 304-134));
		gmxgLabel->setColor(ccc3(255, 108, 0));
		board->addChild(gmxgLabel);
		//共鸣效果列表
// 			GM_HuJiaAddPer = 0,
// 			GM_HPAddPer,
// 			GM_GongJiAddPer,
// 			GM_GeDangZhiAdd,
// 			GM_BaoShangYinZiAdd,
		string str;
		if (manager->getGongMingConfig().properyAdd[curLevel][GM_HuJiaAddPer] > 0)
		{
			temp.initWithFormat("%s+%u%%  ", LFStrings::getValue("huJia").c_str(), manager->getGongMingConfig().properyAdd[curLevel][0]);
			str += temp.getCString();
		}
		if (manager->getGongMingConfig().properyAdd[curLevel][GM_HPAddPer] > 0)
		{
			temp.initWithFormat("%s+%u%%  ", LFStrings::getValue("shengMin").c_str(), manager->getGongMingConfig().properyAdd[curLevel][1]);
			str += temp.getCString();
		}
		if (manager->getGongMingConfig().properyAdd[curLevel][GM_GongJiAddPer] > 0)
		{
			temp.initWithFormat("%s+%u%%  ", LFStrings::getValue("gongJi").c_str(), manager->getGongMingConfig().properyAdd[curLevel][2]);
			str += temp.getCString();
		}
		if (manager->getGongMingConfig().properyAdd[curLevel][GM_GeDangZhiAdd] > 0)
		{
			temp.initWithFormat("%s+%u  ", LFStrings::getValue("geDangZhi").c_str(), manager->getGongMingConfig().properyAdd[curLevel][3]);
			str += temp.getCString();
		}
		if (manager->getGongMingConfig().properyAdd[curLevel][GM_BaoShangYinZiAdd] > 0)
		{
			temp.initWithFormat("%s+%u  ", LFStrings::getValue("baojishanghai").c_str(), manager->getGongMingConfig().properyAdd[curLevel][4]);
			str += temp.getCString();
		}
		CCLabelTTF *propLabel = CCLabelTTF::create(str.c_str(), fontStr_kaiti, 21, CCSize(410,0), kCCTextAlignmentLeft);
		propLabel->setAnchorPoint(ccp(0, 1));
		propLabel->setPosition(ccp(68, 304-153));
		propLabel->setColor(gmxgLabel->getColor());
		board->addChild(propLabel);
		//根据全身装备强化效果计算共鸣星级
		str = LFStrings::getValue("GongMing_Calculate") + ":";
		CCLabelTTF *calcLable = CCLabelTTF::create(str.c_str(), fontStr_kaiti, 21);
		calcLable->setAnchorPoint(ccp(0, 0.5f));
		calcLable->setPosition(ccp(curStarLabel->getPositionX(), 304-222));
		board->addChild(calcLable);
		//
		temp.initWithFormat("%s%s=1%s, %s%s=3%s", 
			LFStrings::getValue("yin").c_str(), LFStrings::getValue("xing").c_str(), LFStrings::getValue("xing").c_str(),
			LFStrings::getValue("jin").c_str(), LFStrings::getValue("xing").c_str(), LFStrings::getValue("xing").c_str());
		CCLabelTTF *caLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 21);
		caLabel->setAnchorPoint(ccp(0, 0.5f));
		caLabel->setPosition(ccp(curStarLabel->getPositionX(), 304-246));
		board->addChild(caLabel);
	}
}

void ZhuangBeiMianBanBase::menuItemClicked_BackFromGongMing(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_GongMingLayer);
	this->disableAllTouchEnd();
}

void ZhuangBeiMianBanBase::yuanshen_backFunc(unsigned int yuanshenId)
{
	int idx = -1;
	for (int i=0, count=mHeros.size(); i<count; ++i)
	{
		BaseActor *actor = mHeros.at(i)->getMyYuanShen();
		if (actor)
		{
			if (actor->getId() == yuanshenId)
			{
				idx = i;
				break;
			}
		}
	}
	//
	if (idx > -1)
	{
		pageSelectedEnd(idx);
	}
	else
	{
		CCLog("Error: %s : can not found this yuanshen's parent [yuanshen id = %u]", __FUNCTION__, yuanshenId);
	}
}



string getJianYingFileName(Profession prof, int pos, bool isHaveWuqi, bool isHaveDunpai, bool isSS_Wuq)
{
	string fileName;

	if(pos == Tag_MenuItem_Jiezhi1 || pos == Tag_MenuItem_Jiezhi2)
	{
		fileName = "jianying_zb_jz.png";
	}
	else if(pos == Tag_MenuItem_Hushengfu)
	{
		fileName = "jianying_zb_hsf.png";
	}
	else if(pos == Tag_MenuItem_Kuijia)
	{
		if(prof == Profession_Fashi
			|| prof == Profession_MiShu
			|| prof == Profession_WuShi
			|| prof == Profession_ShenJianShou
			|| prof == Profession_LieShou)
			fileName = "jianying_zb_kj_q.png";
		else 
			fileName = "jianying_zb_kj_z.png"; 
	}
	else
	{
		string wuqi, dun;
		switch (prof)
		{
		case Profession_Yemanren:
			if(isHaveWuqi)
			{
				if(isSS_Wuq) 
					CCLog("Error : ZhuangBeiMianBanBase::getJianYingFileName ---> you shuang shou wuqi"); // 有双手武器时 剪影就用改武器的。不应该再掉该函数
				else
					dun = "jianying_zb_shld.png";  //单手武器  则是小圆盾剪影
			}
			else
			{
				if(isHaveDunpai)
					wuqi = "jianying_zb_swd.png";   //有盾则用 单手剑剪影
				else  //无剑 无盾 则用 双手斧
				{
					wuqi = "jianying_zb_fu_ss.png";
					dun = "jianying_zb_fu_ss_wuxiao.png";
				}
			}
			break;
		case Profession_Chike:
			if(isHaveWuqi)
			{
				if(isSS_Wuq) 
					CCLog("Error : ZhuangBeiMianBanBase::getJianYingFileName ---> you shuang shou wuqi"); // 有双手武器时 剪影就用改武器的。不应该再掉该函数
				else
					dun = "jianying_zb_shld.png";  //单手武器  则是小圆盾剪影
			}
			else
			{
				if(isHaveDunpai)
					wuqi = "jianying_zb_swd.png";   //有盾则用 单手剑剪影
				else  //无剑 无盾 则用 匕首
				{
					wuqi = "jianying_zb_bishou_ss.png";
					dun = "jianying_zb_bishou_ss_wuxiao.png";
				}
			}
			break;
		case Profession_Fashi:
			if(isHaveWuqi)
			{
				if(isSS_Wuq) 
					CCLog("Error : ZhuangBeiMianBanBase::getJianYingFileName ---> you shuang shou wuqi"); // 有双手武器时 剪影就用改武器的。不应该再掉该函数
				else
					dun = "jianying_zb_fq.png";  //单手武器  则是法器剪影
			}
			else
			{//有无盾都用 单手法杖
				if(isHaveDunpai)
					wuqi = "jianying_zb_fz.png";   //
				else  //无剑 无盾 则用 匕首
				{
					wuqi = "jianying_zb_fz.png";
					dun = "jianying_zb_fq.png";
				}
			}
			break;
		case Profession_Qishi:
		case Profession_WuSeng:
		case Profession_YouXia:
			wuqi = "jianying_zb_swd.png";
			dun = "jianying_zb_shld.png";
			break;
		case Profession_ShenJianShou:
		case Profession_LieShou:
			if(isHaveWuqi)
			{//神箭手、猎手 只有双手武器
				CCLog("Error : ZhuangBeiMianBanBase::getJianYingFileName ---> you shuang shou wuqi"); // 有双手武器时 剪影就用改武器的。不应该再掉该函数
			}
			else
			{
				wuqi = "jianying_zb_gong_ss.png";
				dun = "jianying_zb_gong_ss_wuxiao.png";
			}
			break;
		case Profession_MiShu:
		case Profession_WuShi:
			if(isHaveWuqi)
			{//巫医、秘术师  只有双手武器
				CCLog("Error : ZhuangBeiMianBanBase::getJianYingFileName ---> you shuang shou wuqi"); // 有双手武器时 剪影就用改武器的。不应该再掉该函数
			}
			else
			{
				wuqi = "jianying_zb_fz_ss.png";
				dun = "jianying_zb_fz_ss_wuxiao.png";
			}
			break;
		default:
			break;
		}	

		if(pos == Tag_MenuItem_Wuqi)
			fileName = wuqi;
		else 
			fileName = dun;
	}

	return fileName;
}

bool sortByActorPosition(BaseActor *const aa, BaseActor *const bb)
{
	return aa->getChessBoardPos() < bb->getChessBoardPos();
}