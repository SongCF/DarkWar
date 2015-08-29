#include "GongJiangJingLianZhuanKe.h"
#include "font.h"
#include "EquipmentManager.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "MessageDisplay.h"
#include "GameDirector.h"
#include "ImageManager.h"
#include "FX_CommonFunc.h"
#include "UnblockManager.h"
#include "History.h"
#include "GuideLayerBase.h"
using namespace FX_CommonFunc;


#define LABEL_SIZE 20
#define LABEL_COLOR ccc3(127,127,127)
#define Color_Huang ccc3(220, 157, 36)

#define POS_Title1				ccp(320, 838-105)
#define POS_Title2				ccp(320, 838-483)
#define POS_EqName				ccp(320, 838-156)
#define POS_EqDiwen				ccp(320, 838-307)
#define POS_NeedBg				ccp(320, 838-596)

#define STR_MenuFile_JingLian_Normal "gongjiang_anniu_jinglian_select.png"
#define STR_MenuFile_JingLian_Select "gongjiang_anniu_jinglian_anxia.png"
#define STR_MenuFile_JingLian_Disable "gongjiang_anniu_jinglian_dengdai.png"

#define STR_MenuFile_ZhuanKe_Normal "gongjiang_anniu_zhuanke_select.png"
#define STR_MenuFile_ZhuanKe_Select "gongjiang_anniu_zhuanke_anxia.png"
#define STR_MenuFile_ZhuanKe_Disable "gongjiang_anniu_zhuanke_dengdai.png"

enum 
{
	Tag_Label_Title1,
	Tag_Label_Title2,

	Tag_Label_EquipName,

	Tag_MenuItem_JingLian,
	Tag_MenuItem_ZhuanKe,
	Tag_MenuItem_QueRen,
	Tag_MenuItem_FanHui,
	Tag_MenuItem_TianJiaZB,
	Tag_Img_EquipBg,

	Tag_Board_NeedEquip = 30,

	Tag_Img_EquipDiWen = 50,
	Tag_Img_NeedBoard,

};


GongJiangJingLianZhuanKe* GongJiangJingLianZhuanKe::create(int flag, Equipment *equip)
{
	GongJiangJingLianZhuanKe *layer = new GongJiangJingLianZhuanKe;
	if(layer && layer->init(flag, equip))
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
void GongJiangJingLianZhuanKe::onEnter()
{
	BaseLayer::onEnter();
	{//第一次进入教学
		bool isMaoxianEnded = false;
		if(!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_JingLianZhuanKe))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_JingLianZhuanKe);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_JingLianZhuanKe);
			CCMenuItem *tianjiaZB = (CCMenuItem *)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaZB);
			//History::getHistory()->putInCurrentHero(Key_FirstJingLian,false);
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(CCSize(tianjiaZB->getContentSize().width * tianjiaZB->getScaleX(), tianjiaZB->getContentSize().height * tianjiaZB->getScaleY()), tianjiaZB->getParent()->convertToWorldSpace(tianjiaZB->getPosition())
				, LFStrings::getValue("CaiLiaoFanHui"), NULL, false);
			mFooterLayer->getFooter()->addChild(guideLayer, 1024);
		}
	}
}
bool GongJiangJingLianZhuanKe::init(int flag, Equipment *equip)
{
	if (!BaseLayer::init())
	{
		return false;
	}
	mComFromGJ_equip = equip;
	if(mComFromGJ_equip != NULL) 
		mComeFromEquipDetail = true;
	else 
		mComeFromEquipDetail = false;

	mCurStatus = flag;
	mEquip = NULL;
	mCailiaoEquip = NULL;

	mBg = BaseSprite::create("gongjiang_jinglianzhuanke_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
	currentsp=mBg;

	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 10);
	mMainMenu->setPosition(CCPointZero);

	CCMenuItemImage* jinglian = CCMenuItemImage::create(
		STR_MenuFile_JingLian_Normal
		, STR_MenuFile_JingLian_Select
		, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_HeadMenu));
	mMainMenu->addChild(jinglian, 1, Tag_MenuItem_JingLian);
	jinglian->setPosition(ccp(170, 838-39));

	CCMenuItemImage *zhuanke = CCMenuItemImage::create(
		STR_MenuFile_ZhuanKe_Normal
		, STR_MenuFile_ZhuanKe_Select
		, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_HeadMenu));
	mMainMenu->addChild(zhuanke, 1, Tag_MenuItem_ZhuanKe);
	zhuanke->setPosition(ccp(469, 838-39));

	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_MainMenu));
	mMainMenu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
	fanhui->setPosition(ccp(mBg->getContentSize().width/2, 838-784));

	CCMenuItemImage *queren = CCMenuItemImage::create(
		"zhuangbei_xiangxi_queding.png"
		, "zhuangbei_xiangxi_queding_select.png"
		, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_MainMenu));
	mMainMenu->addChild(queren, 1, Tag_MenuItem_QueRen);
	queren->setPosition(ccp(mBg->getContentSize().width/3*2, 838-784));
	queren->setVisible(false);

	CCMenuItemImage *tianjiaZB = CCMenuItemImage::create(
		"gongjiang_duanzao_xuanze_zhuangbei.png"
		, "gongjiang_duanzao_xuanze_zhuangbei_select.png"
		, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_MainMenu));
	mMainMenu->addChild(tianjiaZB, 1, Tag_MenuItem_TianJiaZB);
	tianjiaZB->setPosition(POS_EqDiwen);

	

	if(flag == gongjiang_jinglian)
		menuItemClicked_HeadMenu(jinglian);
	else 
		menuItemClicked_HeadMenu(zhuanke);

	
	return true;
}

void GongJiangJingLianZhuanKe::menuItemClicked_HeadMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();
	CCMenuItemImage *jinglian = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_JingLian);
	CCMenuItemImage *zhuanke = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_ZhuanKe);

	string info;
	if(tag == Tag_MenuItem_JingLian)
	{
		mCurStatus = gongjiang_jinglian;
		jinglian->initWithNormalImage(
			STR_MenuFile_JingLian_Normal
			, STR_MenuFile_JingLian_Select
			, NULL, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_HeadMenu));
		zhuanke->initWithNormalImage(
			STR_MenuFile_ZhuanKe_Disable
			, STR_MenuFile_ZhuanKe_Select
			, NULL, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_HeadMenu));

		cleanBackGround();
		if (mEquip != NULL && mEquip->canBeJingLian(info))
			selectEquipmentCallback(mEquip->getEquipId(), selectTo_JingLian);
	}
	else
	{
		//暂不开放
		{
			UnblockWarnDialog *dialog = UnblockWarnDialog::create(LFStrings::getValue("BanBen_ZhanWeiKaiQi"), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(dialog);
			return;
		}

		mCurStatus = gongjiang_zhuanke;
		jinglian->initWithNormalImage(
			STR_MenuFile_JingLian_Disable
			, STR_MenuFile_JingLian_Select
			, NULL, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_HeadMenu));
		zhuanke->initWithNormalImage(
			STR_MenuFile_ZhuanKe_Normal
			, STR_MenuFile_ZhuanKe_Select
			, NULL, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_HeadMenu));

		cleanBackGround();
		if (mEquip != NULL && mEquip->canBeZhuanKe(info))
			selectEquipmentCallback(mEquip->getEquipId(), selectTo_ZhuanKe);
	}

	if(mComeFromEquipDetail)
	{
		mComeFromEquipDetail = false;
		selectEquipmentCallback(mComFromGJ_equip->getEquipId(), selectTo_JingLian);
	}
}

void GongJiangJingLianZhuanKe::cleanBackGround()
{
	mMainMenu->retain();
	mBg->removeAllChildren();
	mBg->addChild(mMainMenu, 10);
	mMainMenu->setPosition(CCPointZero);
	mMainMenu->release();
	mMainMenu->setVisible(true);

	mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(false);
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setVisible(true);
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(320, 838-784));
	((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaZB))->setOpacity(255);
	((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaZB))->setVisible(true);

	BaseSprite *needBg;
	if (mCurStatus == gongjiang_jinglian)
		needBg = BaseSprite::create("gongjiang_jinglianzhuanke_jinglian_bg.png");
	else 
		needBg = BaseSprite::create("gongjiang_jinglianzhuanke_zhuanke_bg.png");
	mBg->addChild(needBg, 0, Tag_Img_NeedBoard);
	needBg->setPosition(POS_NeedBg);

	CCLabelTTF *tishiZhuangbei = CCLabelTTF::create(LFStrings::getValue("qiangxuanzezhuangbei").c_str(), fontStr_kaiti, LABEL_SIZE);
	tishiZhuangbei->setColor(LABEL_COLOR);
	mBg->addChild(tishiZhuangbei, 1, Tag_Label_Title1);
	tishiZhuangbei->setPosition(POS_Title1);

	mErrorInfo.clear();
}

void GongJiangJingLianZhuanKe::selectEquipmentCallback(unsigned long long eqId, Flag_SelectTo_GongJiang flag)
{
	if (selectTo_JingLian == flag || selectTo_ZhuanKe == flag)
	{	
		cleanBackGround();
		mEquip = EquipmentManager::getManager()->getOneEquipment(eqId);
		mCailiaoEquip = NULL;
		//1.diwen
		{
			((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaZB))->setOpacity(0);

			BaseSprite* diwen = (BaseSprite*)mBg->getChildByTag(Tag_Img_EquipDiWen);
			if(diwen != NULL)
				diwen->removeFromParent();

			ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
				mEquip->getEquipColor(),
				false, //mEquip->isShenBing(),
				mEquip->isPartOfSuit(),
				mEquip->haveZhuanke());
			BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
			mBg->addChild(diwenImg, 1, Tag_Img_EquipDiWen);
			diwenImg->setPosition(POS_EqDiwen);
			//zhuangbei
			BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mEquip);
			diwenImg->addChild(eqImg);
			eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
			diwenImg->setScale(0.50f);

			//name
			CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_EquipName);
			if (eqName != NULL) eqName->setString(mEquip->getName().c_str());
			else {
				eqName = CCLabelTTF::create(mEquip->getName().c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(eqName, 1, Tag_Label_EquipName);
				eqName->setPosition(POS_EqName);
				eqName->setColor(ImageManager::getManager()->getShowColor(mEquip->getEquipColor(), false, mEquip->isPartOfSuit()));
			}
		}

		string info;
		if(mCurStatus == gongjiang_jinglian)
		{
			//title
			((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Title1))->setString(LFStrings::getValue("jingLian_InfoTitle").c_str());

			if(mEquip->canBeJingLian(info, false))
				refresh_jingLianUI();
			else  //最大次数了
			{
				// 2.
				char buf[20];
				string temp = LFStrings::getValue("jingLian") + LFStrings::getValue("di")
					+ getStrByInt(mEquip->getEquipData().jinglian_times) + LFStrings::getValue("jie");
				CCLabelTTF *title2 = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(title2, 1, Tag_Label_Title2);
				title2->setPosition(POS_Title2);
				title2->setColor(Color_Huang);
				//
				BaseSprite *icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				mBg->addChild(icon, 1);
				icon->setPosition(ccp(300, 838-566));
				int allTime = jinglian_time_chuanqi_max;
				if (mEquip->isShenBing()) allTime = jinglian_time_shenbing_max;
				sprintf(buf, " %d/%d", mEquip->getEquipData().jinglian_times, allTime);
				CCLabelTTF *numLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
				numLabel->setAnchorPoint(ccp(0, 0.5));
				icon->addChild(numLabel);
				numLabel->setPosition(ccp(icon->getContentSize().width, icon->getContentSize().height/2));
				//
				ZBAttr zb_attr;
				zb_attr.attr_type = mEquip->getMainProType();// 如果是攻击，统一返回最小攻击
				zb_attr.attr_value = mEquip->getPropertyAddRatioByEachJingLian();
				zb_attr.attr_value *= mEquip->getEquipData().jinglian_times;;
				string attrStr = GetMoFaShuXing_ByZBAttr(zb_attr, true);
				CCLabelTTF *proLabel = CCLabelTTF::create(attrStr.c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(proLabel, 1);
				proLabel->setColor(Color_Huang);
				proLabel->setPosition(ccp(320, 838-630));

				//3.可篆刻 提示， 最大精炼提示
				string info;
				if (mEquip->canBeZhuanKe(info, false))
				{
					CCLabelTTF *zkTishiLabel = CCLabelTTF::create(LFStrings::getValue("KezhuankeTishi").c_str(), fontStr_kaiti, LABEL_SIZE);
					mBg->addChild(zkTishiLabel, 1);
					zkTishiLabel->setColor(Color_Huang);
					zkTishiLabel->setPosition(ccp(320, 838-688));
					zkTishiLabel->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(1.0f), CCFadeIn::create(1.0f), NULL)));
				}
				//
				if (! mEquip->canBeJingLian(info, false))
				{
					CCLabelTTF *maxNumLabel = CCLabelTTF::create(LFStrings::getValue("zuiDaJingLian_Tishi").c_str(), fontStr_kaiti, LABEL_SIZE);
					mBg->addChild(maxNumLabel, 1);
					maxNumLabel->setColor(LABEL_COLOR);
					maxNumLabel->setPosition(ccp(320, 838-529));
				}
			}
		}
		else if(mCurStatus == gongjiang_zhuanke)
		{
			//title
			((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Title1))->setString(LFStrings::getValue("zhuanKe_InfoTitle").c_str());
			if (mEquip->canBeZhuanKe(info, false))
				refresh_zhuanKeUI();
			else if (! mEquip->haveZhuanke())//还不能篆刻
			{
				CCLabelTTF *title2 = CCLabelTTF::create(LFStrings::getValue("zhuanke_buNengTishi").c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(title2, 1);
				title2->setPosition(ccp(320, 838-617));//old 526
			}
			else if(mEquip->haveZhuanke())//篆刻过了
			{
				((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Title1))->setString(LFStrings::getValue("zhuanke_wancheng").c_str());
				mBg->removeChildByTag(Tag_Img_NeedBoard);
				//
				CCLabelTTF *title2 = CCLabelTTF::create(LFStrings::getValue("moWenZhuanKe").c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(title2, 1);
				title2->setPosition(ccp(320, 838-526));
				title2->setColor(Color_Huang);
				//
				vector<string> proStr = mEquip->getZhuanKeMoFaShuxing();
				for (int i=0; i!=proStr.size(); ++i)
				{
					CCLabelTTF *proLabel = CCLabelTTF::create(proStr.at(i).c_str(), fontStr_kaiti, LABEL_SIZE);
					mBg->addChild(proLabel, 1);
					proLabel->setPosition(ccp(320, 838-557-i*25));
					proLabel->setColor(Color_Huang);
				}
			}
		}
	}

	else if (selectTo_JingLianNeed == flag)
	{
		BaseSprite *board = (BaseSprite*)mBg->getChildByTag(Tag_Board_NeedEquip + 0);
		CCMenu *menu = (CCMenu *)board->getChildByTag(1);
		menu->retain();
		board->removeAllChildren();
		board->addChild(menu, 0, 1);
		menu->setPosition(ccp(0,0));
		menu->release();
		//
		Equipment *eq = EquipmentManager::getManager()->getOneEquipment(eqId);
		mCailiaoEquip = eq;
		ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			eq->getEquipColor(),
			false,//eq->isShenBing(),
			eq->isPartOfSuit(),
			eq->haveZhuanke());
		BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
		board->addChild(diwenImg, 1);
		diwenImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
		//zhuangbei
		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(eq);
		diwenImg->addChild(eqImg);
		eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
		diwenImg->setScale(0.25f);
		//name
		CCLabelTTF *eqName = CCLabelTTF::create(eq->getName().c_str(), fontStr_kaiti, LABEL_SIZE);
		board->addChild(eqName, 1);
		eqName->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height+eqName->getContentSize().height/2));
		eqName->setColor(ImageManager::getManager()->getShowColor(eq->getEquipColor(), false, eq->isPartOfSuit()));

		{
			mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(true);
			mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setPosition(ccp(mWinSize.width/3*2, 838-784));
			mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mWinSize.width/3, 838-784));
		}
	}
}

void GongJiangJingLianZhuanKe::refresh_jingLianUI()
{
	//label current property
	int time = mEquip->getEquipData().jinglian_times;
	if (time == 0)
	{
		CCLabelTTF *curProLabel = CCLabelTTF::create(LFStrings::getValue("jingLianTishi_weiJingLian").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(curProLabel, 0);
		curProLabel->setColor(Color_Huang);
		curProLabel->setPosition(ccp(142, 838-312));
	}
	else
	{
		string str = LFStrings::getValue("DangQian") + LFStrings::getValue("di") + getStrByInt(time) + LFStrings::getValue("jie");
		CCLabelTTF *curProLabel = CCLabelTTF::create(str.c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(curProLabel, 0);
		curProLabel->setColor(Color_Huang);
		curProLabel->setPosition(ccp(142, 838-312));
		{
			//新接口
			ZBAttr zb_attr;
			zb_attr.attr_type = mEquip->getMainProType();// 如果是攻击，统一返回最小攻击
			zb_attr.attr_value = mEquip->getPropertyAddRatioByEachJingLian();
			zb_attr.attr_value *= time;
			string attrStr = GetMoFaShuXing_ByZBAttr(zb_attr, true);

			CCLabelTTF *curlabel = CCLabelTTF::create(attrStr.c_str(), fontStr_kaiti, LABEL_SIZE);
			mBg->addChild(curlabel, 0);
			curlabel->setColor(Color_Huang);
			curlabel->setPosition(ccp(curProLabel->getPositionX(), 838-343));
		}
	}
	//label next property
	if (mEquip->jingLianTimeLeft() > 0)
	{
		CCLabelTTF *nextProLabel = CCLabelTTF::create(LFStrings::getValue("jingLianTishi_xiaYiJie").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(nextProLabel, 0);
		nextProLabel->setPosition(ccp(500, 838-312));
		{
			//新接口
			ZBAttr zb_attr;
			zb_attr.attr_type = mEquip->getMainProType();// 如果是攻击，统一返回最小攻击
			zb_attr.attr_value = mEquip->getPropertyAddRatioByEachJingLian();
			zb_attr.attr_value *= (time + 1);
			string attrStr = GetMoFaShuXing_ByZBAttr(zb_attr, true);

			CCLabelTTF *nexrLabel = CCLabelTTF::create(attrStr.c_str(), fontStr_kaiti, LABEL_SIZE);
			mBg->addChild(nexrLabel, 0);
			nexrLabel->setAnchorPoint(ccp(0, 0.5f));
			nexrLabel->setPosition(ccp(440, 838-343));
		}
	}

	//title 2
	CCLabelTTF *title2 = CCLabelTTF::create(LFStrings::getValue("jingLian_InfoTitle2").c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(title2, 1, Tag_Label_Title2);
	title2->setPosition(POS_Title2);
	title2->setColor(LABEL_COLOR);
	//board
	BaseSprite *board = BaseSprite::create("gongjiang_jinglian_suoxu_bg.png");
	mBg->addChild(board, 1, Tag_Board_NeedEquip + 0);
	board->setPosition(ccp(320, 838-604));
	//menu
	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 0, 1);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *addItem = CCMenuItemImage::create(
		"gongjiang_duanzao_xuanze_zhuangbei.png"
		, "gongjiang_duanzao_xuanze_zhuangbei_select.png"
		, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_addNeedEquip));
	menu->addChild(addItem);
	addItem->setScale(0.5f);
	addItem->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
	//need silver
	vector<CostData> costVec = mEquip->getZbJingLianCost();
	BaseSprite *icon = NULL;
	if (costVec.at(0).item == ItemsType_Silver) icon = BaseSprite::create("yinbi_tubiao.png");
	else CCLog("Error: --- ");
	mBg->addChild(icon);
	icon->setScale(0.8f);
	icon->setPosition(ccp(260, 838-700));
	//need yinbi
	unsigned int silver_need = costVec.at(0).count;
	unsigned int silver_total = 0;
	char buf[20];
	sprintf(buf, " %d/", silver_need);
	CCLabelTTF *yinbiLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
	mBg->addChild(yinbiLabel);
	yinbiLabel->setAnchorPoint(ccp(0, 0.5));
	yinbiLabel->setPosition(ccp(icon->getPositionX()+10, icon->getPositionY()));
	// all yinbi
	{
		WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
		if (wp != NULL)
		{
			silver_total = wp->getCount();
		}
	}
	sprintf(buf, "%d", silver_total);
	CCLabelTTF *yinbiAllLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
	mBg->addChild(yinbiAllLabel);
	yinbiAllLabel->setAnchorPoint(ccp(0, 0.5f));
	yinbiAllLabel->setPosition(ccp(yinbiLabel->getPositionX()+yinbiLabel->getContentSize().width, icon->getPositionY()));
	if(silver_total < silver_need)
	{
		yinbiAllLabel->setColor(ccRED);
		mErrorInfo = LFStrings::getValue("suoxuyinbibuzu");
	}
}

void GongJiangJingLianZhuanKe::refresh_zhuanKeUI()
{
	//1. tilte 2
	CCLabelTTF *title2 = CCLabelTTF::create(LFStrings::getValue("zhuanke_suoxu").c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(title2, 1, Tag_Label_Title2);
	title2->setPosition(POS_Title2);
	title2->setColor(LABEL_COLOR);
	//2. 材料
	//baord
	BaseSprite *board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
	mBg->addChild(board);
	board->setPosition(ccp(320, 838-609));
	//img
	vector<CostData> needData = mEquip->getShenBingZhuankeCost();
	for (int i=0; i!=needData.size(); ++i) // 排序，让银币排在最后面，显示。
		for (int j=i+1; j!=needData.size(); ++j)
			if (needData.at(i).item < needData.at(j).item)
				swap(needData.at(i), needData.at(j));
	BaseSprite *img = BaseSprite::create(ImageManager::getManager()->getItemFilename(needData.at(0).item));
	board->addChild(img);
	img->setPosition(ccp(46, 128-46));
	//name
	CCLabelTTF *nameLabel = CCLabelTTF::create(CS::getItemName(needData.at(0).item).c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(nameLabel, 1);
	nameLabel->setPosition(ccp(320, 838-531));
	//need num
	CCString buf;
	buf.initWithFormat("%d/", needData.at(0).count);
	CCLabelTTF *needNumLabel = CCLabelTTF::create(buf.getCString(), fontStr_BookAntiqua, LABEL_SIZE);
	board->addChild(needNumLabel);
	needNumLabel->setAnchorPoint(ccp(1, 0.5));
	needNumLabel->setPosition(ccp(47, 15));
	//all num
	int allNum = WuPinManager::getManager()->getWuPin(needData.at(0).item)->getCount();
	buf.initWithFormat("%d", allNum);
	CCLabelTTF *allLabel = CCLabelTTF::create(buf.getCString(), fontStr_BookAntiqua, LABEL_SIZE);
	board->addChild(allLabel);
	allLabel->setAnchorPoint(ccp(0, 0.5));
	allLabel->setPosition(needNumLabel->getPosition());
	if (allNum < needData.at(0).count)
	{
		allLabel->setColor(ccRED);
		mErrorInfo = LFStrings::getValue("zhuanke_suoxu") + LFStrings::getValue("cailiaobuzu");
	}
	//3. silver
	BaseSprite *icon = BaseSprite::create("yinbi_tubiao.png");
	mBg->addChild(icon, 1);
	icon->setScale(0.8f);
	icon->setPosition(ccp(260, 838-700));
	//
	unsigned int silver_need = needData.at(1).count;
	unsigned int silver_total = 0;
	buf.initWithFormat(" %d/", silver_need);
	CCLabelTTF *yinbiLabel = CCLabelTTF::create(buf.getCString(), fontStr_BookAntiqua, LABEL_SIZE);
	mBg->addChild(yinbiLabel);
	yinbiLabel->setAnchorPoint(ccp(0, 0.5));
	yinbiLabel->setPosition(ccp(icon->getPositionX()+10, icon->getPositionY()));
	// all yinbi
	silver_total = WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount();
	buf.initWithFormat("%d", silver_total);
	CCLabelTTF *yinbiAllLabel = CCLabelTTF::create(buf.getCString(), fontStr_BookAntiqua, LABEL_SIZE);
	mBg->addChild(yinbiAllLabel);
	yinbiAllLabel->setAnchorPoint(ccp(0, 0.5f));
	yinbiAllLabel->setPosition(ccp(yinbiLabel->getPositionX()+yinbiLabel->getContentSize().width, icon->getPositionY()));
	if(silver_total < silver_need)
	{
		yinbiAllLabel->setColor(ccRED);
		mErrorInfo = LFStrings::getValue("suoxuyinbibuzu");
	}
	//
	mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(true);
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mWinSize.width/3, 838-784));
}

void GongJiangJingLianZhuanKe::menuItemClicked_addNeedEquip(CCObject *pSender)
{
	GongJiang_EquipSelect *layer = GongJiang_EquipSelect::create(this, selectTo_JingLianNeed, mEquip);
	mFooterLayer->popUpContaintLayer(layer);
}

void GongJiangJingLianZhuanKe::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if(tag == Tag_MenuItem_FanHui)
	{
		mFooterLayer->dismmisPopUp();
	}
	else if(tag == Tag_MenuItem_TianJiaZB)
	{
		if(mCurStatus == gongjiang_jinglian)
		{
			GongJiang_EquipSelect *layer = GongJiang_EquipSelect::create(this, selectTo_JingLian);
			mFooterLayer->popUpContaintLayer(layer);
		}
		else
		{
			GongJiang_EquipSelect *layer = new GongJiang_EquipSelect(this, selectTo_ZhuanKe);
			mFooterLayer->popUpContaintLayer(layer);
			layer->release();
		}
	}
	else if(tag == Tag_MenuItem_QueRen)
	{
		if(!mErrorInfo.empty())
		{
			MessageDisplay* layer = MessageDisplay::create(mErrorInfo);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		if(mCurStatus == gongjiang_jinglian)
		{
			addObserver(callfuncO_selector(GongJiangJingLianZhuanKe::rsp_JingLian), MSG_ZBJinglianRsp);
			mCmdHlp->cmdZBJinglian(mEquip->getEquipId(), mCailiaoEquip->getEquipId());
		}
		else if(mCurStatus == gongjiang_zhuanke)
		{
			addObserver(callfuncO_selector(GongJiangJingLianZhuanKe::rsp_ZhuanKe), MSG_shenbingZhuankeJingwenRsp);
			mCmdHlp->cmdShenbingZhuankeJingwen(mEquip->getEquipId());
		}
	}
}

void GongJiangJingLianZhuanKe::rsp_JingLian(CCObject *msg)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_ZBJinglianRsp);

	SPCmd_ZBJinglianRsp *data = (SPCmd_ZBJinglianRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	//
	cleanBackGround();
	Qianhuadh(currentsp);

	((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaZB))->setVisible(false);
	mCailiaoEquip = NULL;
	//1.diwen
	{
		BaseSprite* diwen = (BaseSprite*)mBg->getChildByTag(Tag_Img_EquipDiWen);
		if(diwen != NULL)
			diwen->removeFromParent();

		ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			mEquip->getEquipColor(),
			false,//mEquip->isShenBing(),
			mEquip->isPartOfSuit(),
			mEquip->haveZhuanke());
		BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
		mBg->addChild(diwenImg, 1, Tag_Img_EquipDiWen);
		diwenImg->setPosition(POS_EqDiwen);
		//zhuangbei
		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mEquip);
		diwenImg->addChild(eqImg);
		eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
		diwenImg->setScale(0.50f);
		//title
		((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Title1))->setString(LFStrings::getValue("jingLianTishi_wanCheng").c_str());
		//name
		CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_EquipName);
		if (eqName != NULL) eqName->setString(mEquip->getName().c_str());
		else {
			eqName = CCLabelTTF::create(mEquip->getName().c_str(), fontStr_kaiti, LABEL_SIZE);
			mBg->addChild(eqName, 1, Tag_Label_EquipName);
			eqName->setPosition(POS_EqName);
			eqName->setColor(ImageManager::getManager()->getShowColor(mEquip->getEquipColor(), false, mEquip->isPartOfSuit()));
		}
	}
	// 2.
	char buf[20];
	string temp = LFStrings::getValue("jingLian") + LFStrings::getValue("di")
		+ getStrByInt(mEquip->getEquipData().jinglian_times) + LFStrings::getValue("jie");
	CCLabelTTF *title2 = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(title2, 1, Tag_Label_Title2);
	title2->setPosition(POS_Title2);
	title2->setColor(Color_Huang);
	// 
	BaseSprite *icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
	mBg->addChild(icon, 1);
	icon->setPosition(ccp(300, 838-566));
	int allTime = CS::getJingLianMaxTimes(mEquip->getEquipColor(), mEquip->getLevel());
	sprintf(buf, " %d/%d", mEquip->getEquipData().jinglian_times, allTime);
	CCLabelTTF *numLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
	numLabel->setAnchorPoint(ccp(0, 0.5));
	icon->addChild(numLabel);
	numLabel->setPosition(ccp(icon->getContentSize().width, icon->getContentSize().height/2));
	//
	//新接口
	ZBAttr zb_attr;
	zb_attr.attr_type = mEquip->getMainProType();// 如果是攻击，统一返回最小攻击
	zb_attr.attr_value = mEquip->getPropertyAddRatioByEachJingLian();
	zb_attr.attr_value *= mEquip->getEquipData().jinglian_times;;
	string attrStr = GetMoFaShuXing_ByZBAttr(zb_attr, true);
	CCLabelTTF *proLabel = CCLabelTTF::create(attrStr.c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(proLabel, 1);
	proLabel->setColor(Color_Huang);
	proLabel->setPosition(ccp(320, 838-618));
	//3.可篆刻 提示， 最大精炼提示
	string info;
	if (mEquip->canBeZhuanKe(info, false))
	{
		CCLabelTTF *zkTishiLabel = CCLabelTTF::create(LFStrings::getValue("KezhuankeTishi").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(zkTishiLabel, 1);
		zkTishiLabel->setColor(Color_Huang);
		zkTishiLabel->setPosition(ccp(320, 838-688));
		zkTishiLabel->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(1.0f), CCFadeIn::create(1.0f), NULL)));
	}
	//
	if (! mEquip->canBeJingLian(info, false))
	{
		CCLabelTTF *maxNumLabel = CCLabelTTF::create(LFStrings::getValue("zuiDaJingLian_Tishi").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(maxNumLabel, 1);
		maxNumLabel->setColor(LABEL_COLOR);
		maxNumLabel->setPosition(ccp(320, 838-529));
	}

	//
	mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(false);
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setVisible(false);
	//
	CCMenu *menu = CCMenu::create();
	mBg->addChild(menu, 1);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"zhuangbei_xiangxi_queding.png"
		, "zhuangbei_xiangxi_queding_select.png"
		, this, menu_selector(GongJiangJingLianZhuanKe::menuItemClicked_WanChengItem));
	menu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
	fanhui->setPosition(ccp(mBg->getContentSize().width/2, 838-784));
}

void GongJiangJingLianZhuanKe::rsp_ZhuanKe(CCObject *msg)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_shenbingZhuankeJingwenRsp);

	SPCmd_ShenbingZhuankeJingwenRsp *data = (SPCmd_ShenbingZhuankeJingwenRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	//
	cleanBackGround();
	selectEquipmentCallback(mEquip->getEquipId(), selectTo_ZhuanKe);
}

void GongJiangJingLianZhuanKe::menuItemClicked_WanChengItem(CCObject *pSender)
{
	cleanBackGround();
	selectEquipmentCallback(mEquip->getEquipId(), selectTo_JingLian);
}


void GongJiangJingLianZhuanKe::Qianhuadh(BaseSprite *bg){


	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("gongjiang_xiaoguo.ExportJson");
	CCArmature *armature = CCArmature::create("gongjiang_xiaoguo");
	armature->setZOrder(10);
	string name="jinglian";
	armature->getAnimation()->play(name.c_str());
	armature->getAnimation()->setMovementEventCallFunc(this, 
		  SEL_MovementEventCallFunc(&GongJiangJingLianZhuanKe::callfunc_AnimateEnd));
	CCSize bgSize = bg->getContentSize();
	armature->setPosition(ccp(mWinSize.width/2, bgSize.height-552));
	bg->addChild(armature);

}
void GongJiangJingLianZhuanKe::callfunc_AnimateEnd(CCArmature *ar2, MovementEventType, const char *){


	ar2->removeFromParent();
    isAnimation=1;
	CCLOG("over--->");

}

GongJiangJingLianZhuanKe::~GongJiangJingLianZhuanKe(){


	if(isAnimation==1)
	   CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("gongjiang_xiaoguo.ExportJson");

}