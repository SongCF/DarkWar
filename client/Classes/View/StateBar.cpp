#include "StateBar.h"
#include "LFUtile.h"
#include "MyselfManager.h"
#include "WuPinManager.h"
#include "font.h"
#include "Debug.h"
#include "NetGameSetUI.h"
//#include "JingyanBar.h"
// 测试
#include "TestCmdLayer.h"

#define LABEL_SIZE 24

enum MyEnum
{
	TAG_YINBI   = 4643 + 210,
	TAG_JINBI,
	TAG_DEGNJI  ,
	TAG_JINGYAN  ,
	TAG_VIP,
};

StateBar::StateBar(void)
{
//	initWithFile("zhuangtailan_mianban.png");
	init();
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	this->setContentSize(CCSize(winSize.width, 110));

	WuPin* gold = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
	WuPin* yingBi = WuPinManager::getManager()->getWuPin(ItemsType_Silver);

	const int Total_TiLi = 120;

	BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	BaseActorProperty pro = actor->getCurrentProperty();
	string name = pro.customName;

	int goldNum = gold!=NULL?gold->getCount():0;
	int yinBiNum = yingBi!=NULL?yingBi->getCount():0;
	int dengJi = pro.level;
	int defaultJinYanZhi = pro.exp_need;
	int currentJinYanZhi = pro.exp_current;
	int totalTiLi = Total_TiLi;
	int currentTiLi = pro.vit_current;
	CCString *imgName = CCString::createWithFormat("touxiang_%s1.png",pro.baseName.c_str());
	int type = 0;
	if (pro.baseName == "barbarian")
	{
		type = 3;
	}
	else if (pro.baseName == "hunter")
	{
		type = 1;
	}
	else
	{
		type = 2;
	}
	char buf[20];
	int start_y = 90;
	int start_x = 6;
	
	
	
	

	CCSprite *heroBgSpr = CCSprite::create();
	CCMenu *menu = CCMenu::create();
	heroBgSpr->addChild(menu,2);
	menu->setPosition(ccp(0,0));
	addChild(heroBgSpr,1);
	{
		CCSprite *bgSpr = CCSprite::create("zhuye_dinglan.png");
		heroBgSpr->setContentSize(bgSpr->getContentSize());
		heroBgSpr->setPosition(ccp(heroBgSpr->getContentSize().width / 2,this->getContentSize().height - heroBgSpr->getContentSize().height / 2));
		bgSpr->setPosition(ccp(bgSpr->getContentSize().width / 2 , bgSpr->getContentSize().height / 2));
		heroBgSpr->addChild(bgSpr,1);
	}
	m_sBgSpr = heroBgSpr;

	//头像
	{
		CCMenuItemImage *headSpr = CCMenuItemImage::create(
			imgName->getCString(),
			imgName->getCString(),
			this, menu_selector(StateBar::clickHero));
		menu->addChild(headSpr);
		headSpr->setPosition(ccp(65,77));
		//heroBgSpr->addChild(headSpr);
	}
	//标记
	{
		CCSprite *spr = CCSprite::create(CCString::createWithFormat("zhuye_jiaobiao_%d.png",type)->getCString());
		if (spr)
		{
			spr->setPosition(ccp(143,116));
			heroBgSpr->addChild(spr,1);
		}
	}
	{
		//name

		CCLabelTTF *labelName = CCLabelTTF::create(name.c_str(), fontStr_kaiti, LABEL_SIZE);
		heroBgSpr->addChild(labelName, 1);
		labelName->setColor(fonColor_FaGuang);
		labelName->setAnchorPoint(ccp(0, 0.5));
		labelName->setPosition(ccp(171, 116));

		//vip
		BaseSprite *imgVip = getVipImg();
		if(imgVip != NULL)
		{
			heroBgSpr->addChild(imgVip,1,TAG_VIP);
			imgVip->setPosition(ccp(338, labelName->getPositionY()));
		}
	}
	int spr_y = 116;
	{//金币银币显示
		//gold
		BaseSprite *imgJinbi = BaseSprite::create("jinbi_tubiao.png");
		heroBgSpr->addChild(imgJinbi, 1);
		imgJinbi->setScale(0.6f);
		imgJinbi->setPosition(ccp(410, spr_y));
		//
		sprintf(buf, "%d", goldNum);
		CCLabelTTF *labelJinbi = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
		labelJinbi->setColor(ccWHITE);
		heroBgSpr->addChild(labelJinbi, 1, TAG_JINBI);
		labelJinbi->setAnchorPoint(ccp(0, 0.5));
		labelJinbi->setPosition(ccp(425, spr_y));
		//silver
		BaseSprite *imgYinbi = BaseSprite::create("yinbi_tubiao.png");
		heroBgSpr->addChild(imgYinbi, 1);
		imgYinbi->setScale(0.6f);
		imgYinbi->setPosition(ccp(520, spr_y));
		//
		sprintf(buf, "%d", yinBiNum);
		CCLabelTTF *labelYinbi = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
		heroBgSpr->addChild(labelYinbi, 1, TAG_YINBI);
		labelYinbi->setAnchorPoint(ccp(0, 0.5));
		labelYinbi->setPosition(ccp(545, spr_y));

	}
	{//经验条

		BaseSprite *lvImg = BaseSprite::create("zhuye_dengji.png");
		heroBgSpr->addChild(lvImg, 2);
		lvImg->setAnchorPoint(ccp(0, 0.5));
		lvImg->setPosition(ccp(0,28));
		//level
		sprintf(buf, "%2d", dengJi);
		CCLabelTTF *labelLev = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		labelLev->setColor(fonColor_PuTong);
		heroBgSpr->addChild(labelLev, 2, TAG_DEGNJI);
		labelLev->setAnchorPoint(ccp(0, 0.5));
		labelLev->setPosition(ccp(lvImg->getPositionX()+lvImg->getContentSize().width, lvImg->getPositionY()));

		CCSprite *jingyanSpr = CCSprite::create("zhuye_yuanxing_jinyan3.png");
		jingyanSpr->setPosition(ccp(94,46));
		heroBgSpr->addChild(jingyanSpr);

		CCSprite *jingyanBgSpr = CCSprite::create("zhuye_yuanxing_jinyan2.png");
		jingyanBgSpr->setAnchorPoint(ccp(0.0f,1.0f));
		jingyanBgSpr->setPosition(ccp(65,77));
		heroBgSpr->addChild(jingyanBgSpr,0,101);
		jingyanBgSpr->setRotation(((float)currentJinYanZhi)/((float)defaultJinYanZhi) * (-90.0f));
		/*
		CCControlPotentiometer *jinYanBar = CCControlPotentiometer::create("zhuye_yuanxing_jinyan3.png","zhuye_yuanxing_jinyan2.png","jiangyanthum.png");
		//CCProgressTimer* jinYanBar = LF::createProgressTimer(BaseSprite::create("zhuye_yuanxing_jinyan2.png"));
		heroBgSpr->addChild(jinYanBar, 1, TAG_JINGYAN);
		//jinYanBar->setAnchorPoint(CCPointZero);
		jinYanBar->setPosition(ccp(94,46));
		jinYanBar->getProgressTimer()->setType(kCCProgressTimerTypeRadial);
		//jinYanBar->setProgressTimer(LF::createProgressTimer(BaseSprite::create("zhuye_yuanxing_jinyan2.png")));
		//jinYanBar->setMinimumValue(0);
		//jinYanBar->setMaximumValue(100);
		jinYanBar->setValue(((float)currentJinYanZhi)/(float)defaultJinYanZhi);
		//jinYanBar->setPercentage(((float)currentJinYanZhi)/(float)defaultJinYanZhi * 100.0f);
		*/

	}
	{//体力
		BaseSprite *labelT = BaseSprite::create("zhuye_tili_wenzi.png");
		//labelT->setAnchorPoint(ccp(0, 0.5));
		heroBgSpr->addChild(labelT, 1);
		labelT->setPosition(ccp(150, 76));
		{

			BaseSprite* tiLiBarBoarder = BaseSprite::create("zhuye_tili_bg.png");
			heroBgSpr->addChild(tiLiBarBoarder,1, 10000);
			tiLiBarBoarder->setPosition(ccp(247, labelT->getPositionY()));

			CCProgressTimer* tiLiBar = LF::createProgressTimer(BaseSprite::create("zhuye_tili.png"));
			tiLiBarBoarder->addChild(tiLiBar, 2, 101);
			tiLiBar->setPosition(ccp(tiLiBarBoarder->getContentSize().width/2, tiLiBarBoarder->getContentSize().height/2));
			tiLiBar->setPercentage(((float)currentTiLi)/(float)totalTiLi * 100.0f);

			

			//num
			CCString temp;
			temp.initWithFormat("%u/%d", MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current, Vitality_Total);
			CCLabelTTF *tiliLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 15);
			tiliLabel->setPosition(ccp(tiLiBarBoarder->getContentSize().width/2, tiLiBarBoarder->getContentSize().height/2));
			tiLiBarBoarder->addChild(tiliLabel, 2, 102);


			//体力购买按钮
			{
				
				
				CCMenuItemImage *buyItem = CCMenuItemImage::create(
					"tiligoumai.png",
					"tiligoumai_select.png",
					this, menu_selector(StateBar::menuItemClicked_TiLiBuy));
				menu->addChild(buyItem);
				buyItem->setPosition(ccp(340, tiLiBarBoarder->getPositionY()));
			}
		}
	}
	//
	/*
	
	{
		CCProgressTimer* jinYanBar = LF::createProgressTimer(BaseSprite::create("zhuye_jinyan.png"));
		addChild(jinYanBar, 1, TAG_JINGYAN);
		jinYanBar->setPosition(ccp(140, labelLev->getPositionY()));
		jinYanBar->setPercentage(((float)currentJinYanZhi)/(float)defaultJinYanZhi * 100.0f);

		BaseSprite* jinYanBarBoarder = BaseSprite::create("zhuye_jinyan_bg.png");
		addChild(jinYanBarBoarder);
		jinYanBarBoarder->setPosition(jinYanBar->getPosition());

		//num
		if (MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level < 30)
		{
			CCString temp;
			temp.initWithFormat("%u/%u", MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().exp_current
				, MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().exp_need);
			CCLabelTTF *tiliLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 15);
			tiliLabel->setPosition(this->convertToNodeSpace(
				jinYanBarBoarder->convertToWorldSpace(ccp(jinYanBarBoarder->getContentSize().width/2, jinYanBarBoarder->getContentSize().height/2))));
			addChild(tiliLabel, 2);
		}
	}

	//体力
	
	*/

#ifdef SHOW_DEBUG_LABLE
	CCMenuItemFont* itemTest = CCMenuItemFont::create(
		"GoTo Test",
		this,SEL_MenuHandler(&StateBar::testClicked));//

	CCMenu* TESTmenu = CCMenu::createWithItem(itemTest);
	TESTmenu->setPosition(CCPointZero);
	itemTest->setPosition(ccp(getContentSize().width - 80,-320));
	addChild(TESTmenu);
#endif 
}

void StateBar::testClicked(CCObject* sender)
{
	TestCmdLayer* layer = new TestCmdLayer();
	CCScene* scene = CCScene::create();
	scene->addChild(layer);
	layer->release();
	CCDirector::sharedDirector()->pushScene(scene);
}

StateBar* StateBar::createBar()
{
	StateBar* bar = new StateBar();
	bar->autorelease();
	return bar;
}


StateBar::~StateBar(void)
{
}

CCLabelTTF* StateBar::getSilverLabel()
{
	return (CCLabelTTF*) m_sBgSpr->getChildByTag(TAG_YINBI);
}

CCLabelTTF* StateBar::getLevelLabel()
{
	return (CCLabelTTF*) m_sBgSpr->getChildByTag(TAG_DEGNJI);
}

CCProgressTimer* StateBar::getExpBar()
{
	return (CCProgressTimer*)m_sBgSpr-> getChildByTag(TAG_JINGYAN);
}


BaseSprite *StateBar::getVipImg()
{
	int level = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vip_level;
	char buf[40];
	sprintf(buf, "Vip_icon_lv%d.png", level);
	
	return BaseSprite::create(buf);
}

void StateBar::menuItemClicked_TiLiBuy(CCObject *pSender)
{
	GetTiLiLayer* layer = GetTiLiLayer::create(this, false);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024);
}

void StateBar::getTiliCallBackFunc()
{
	refreshTiLiBar();
	refreshGoldAndSilver();
}

void StateBar::refreshTiLiBar()
{
	CCProgressTimer *timer = (CCProgressTimer*)m_sBgSpr->getChildByTag(10000)->getChildByTag(101);
	CCAssert(timer != NULL,"");
	CCLabelTTF *label = (CCLabelTTF*)m_sBgSpr->getChildByTag(10000)->getChildByTag(102);
	CCAssert(label != NULL,"");

	//refresh
	BaseActor* actor = MyselfManager::getManager()->getMyZhuJueData();
	BaseActorProperty pro = actor->getCurrentProperty();
	if (timer) timer->setPercentage(pro.vit_current / 120.0f * 100);
	//timer->setRotation(pro.vit_current / 120.0f * (-90.0f));
	CCString temp;
	temp.initWithFormat("%u/%d", MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().vit_current, Vitality_Total);
	if (label) label->setString(temp.getCString());
}

void StateBar::refreshGoldAndSilver()
{
	int silverNum = WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount();
	int goldNum = WuPinManager::getManager()->getWuPin(ItemsType_Gold)->getCount();
	CCLabelTTF *silLabel = (CCLabelTTF*)m_sBgSpr->getChildByTag(TAG_YINBI);
	CCLabelTTF *goldLabel = (CCLabelTTF*) m_sBgSpr->getChildByTag(TAG_JINBI);
	char buf[32];
	sprintf(buf, "%d", silverNum);
	silLabel->setString(buf);
	sprintf(buf, "%d", goldNum);
	goldLabel->setString(buf);
}
void StateBar::refreshVipMig(int level)
{
	CCSprite *spr = (CCSprite *)m_sBgSpr->getChildByTag(TAG_VIP);
	if (level > MyselfManager::getManager()->getCurrentVipLevel())
	{
		spr->setTexture(getVipImg(level)->getTexture());
	}
}
void StateBar::clickHero(CCObject *obj)
{
	NetGameSetUI *layer = NetGameSetUI::create();
	layer->setPosition(CCPointZero);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1030);
}