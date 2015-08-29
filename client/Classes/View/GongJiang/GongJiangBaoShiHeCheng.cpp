#include "GongJiangBaoShiHeCheng.h"
#include "GongJiangBaoShiHeCheng_BaoShiSelect.h"
#include "font.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "GuideLayerBase.h"
#include "History.h"

extern bool g_gongjiang_BsHc_Opend;

#define LABEL_SIZE 20
#define LABEL_COLOR ccc3(195, 145, 96)

enum 
{
	Tag_MenuItem_FanHui,
	Tag_MenuItem_QueRen,
	Tag_MenuItem_TianJiaBaoShi,
	Tag_Label_TishiBaoshi,

	Tag_MenuItem_Jia,
	Tag_MenuItem_Jian,
	Tag_DiZuo_BaoShiNum,
	Tag_Label_DizuoNum,

	Tag_Board_SuoXuBaoShi,
	Tag_Board_SuoXuJinBi,
	Tag_Label_JinBi,
	Tag_Label_JinBiAll,
	Tag_Label_BaoShi_Need,
	Tag_Label_BaoShi_All,
};

GongJiangBaoShiHeCheng* GongJiangBaoShiHeCheng::create()
{
	GongJiangBaoShiHeCheng *layer = new GongJiangBaoShiHeCheng;
	if(layer &&layer->init())
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

bool GongJiangBaoShiHeCheng::init()
{
	//bg
	mBg = BaseSprite::create("gongjiang_hecheng_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
	//
	BaseSprite *biankuang = BaseSprite::create("gongjiang_baoshi_waikuang.png");
	mBg->addChild(biankuang);
	biankuang->setPosition(ccp(320, 838-236));

	//menu
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 1);
	mMainMenu->setPosition(CCPointZero);

	//menu item
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(GongJiangBaoShiHeCheng::menuItemClicked_MainMenu));
	mMainMenu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
	fanhui->setPosition(ccp(mBg->getContentSize().width/2, 838-784));

	CCMenuItemImage *queren = CCMenuItemImage::create(
		"zhuangbei_xiangxi_queding.png"
		, "zhuangbei_xiangxi_queding_select.png"
		, this, menu_selector(GongJiangBaoShiHeCheng::menuItemClicked_MainMenu));
	mMainMenu->addChild(queren, 1, Tag_MenuItem_QueRen);
	queren->setPosition(ccp(mBg->getContentSize().width/3*2, 838-784));
	queren->setVisible(false);

	CCMenuItemImage *tianjia = CCMenuItemImage::create(
		"gongjiang_duanzao_xuanze_zhuangbei.png"
		, "gongjiang_duanzao_xuanze_zhuangbei_select.png"
		, this, menu_selector(GongJiangBaoShiHeCheng::menuItemClicked_MainMenu));
	mMainMenu->addChild(tianjia, 1, Tag_MenuItem_TianJiaBaoShi);
	tianjia->setPosition(ccp(320, 838-230));

	CCLabelTTF *label = CCLabelTTF::create(LFStrings::getValue("xuanzebaoshi").c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(label, 1, Tag_Label_TishiBaoshi);
	label->setColor(ccc3(127,127,127));
	label->setPosition(ccp(320, 838-105));


	return true;
}

void GongJiangBaoShiHeCheng::clearnBackGround()
{

	mMainMenu->retain();
	mBg->removeAllChildren();
	mBg->addChild(mMainMenu, 1);
	mMainMenu->setPosition(CCPointZero);
	mMainMenu->release();
	mMainMenu->setVisible(true);
	mErrorInfo.clear();	
	//
	BaseSprite *biankuang = BaseSprite::create("gongjiang_baoshi_waikuang.png");
	mBg->addChild(biankuang);
	biankuang->setPosition(ccp(320, 838-236));
	//
	CCLabelTTF *label = CCLabelTTF::create(LFStrings::getValue("xuanzebaoshi").c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(label, 1, Tag_Label_TishiBaoshi);
	label->setColor(ccc3(127,127,127));
	label->setPosition(ccp(320, 838-105));

	((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaBaoShi))->setOpacity(255);
	mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(false);
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mBg->getContentSize().width/2, 838-784));
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setVisible(true);

}

void GongJiangBaoShiHeCheng::menuItemClicked_MainMenu(CCObject *pSender)
{
	CCMenuItemImage *menuItem = (CCMenuItemImage*)pSender;
	int tag = menuItem->getTag();

	if(tag == Tag_MenuItem_FanHui)
	{
		mFooterLayer->dismmisPopUp();
	}
	else if(tag == Tag_MenuItem_TianJiaBaoShi)
	{
		GongJiangBaoShiHeCheng_BaoShiSelect *layer = GongJiangBaoShiHeCheng_BaoShiSelect::create(this);
		mFooterLayer->popUpContaintLayer(layer);
	}
	else if(tag == Tag_MenuItem_QueRen)
	{
		if(! mErrorInfo.empty())
		{
			MessageDisplay* layer = MessageDisplay::create(mErrorInfo);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		addObserver(callfuncO_selector(GongJiangBaoShiHeCheng::BaoShiHeCheng_Rsp), MSG_BaoshiHechengRsp);
		SPCmd_BaoshiHecheng rep;
		rep.baoshi_type = mCurHeChengBaoShiType;
		rep.count = mCurHeChengBaoShiNum;
		mCmdHlp->cmdBaoshiHecheng(rep);
	}
}

void GongJiangBaoShiHeCheng::setSelectedBaoshiTypeId(unsigned int baoshiType, int baoshiCount)
{
	clearnBackGround();
	mCurHeChengBaoShiType = baoshiType;

	CCLabelTTF *label = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_TishiBaoshi);
	label->setString(CS::getBaoshiName(mCurHeChengBaoShiType).c_str());

	CCMenuItemImage *tianjiaItem = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaBaoShi);
	tianjiaItem->setOpacity(0);//保证已选有宝石的情况下 还能重新选择
	
	//宝石 图片
	BaseSprite *baoshiImg = BaseSprite::create(
		ImageManager::getManager()->getBaoshiFilename(mCurHeChengBaoShiType));
	mBg->addChild(baoshiImg);
	baoshiImg->setPosition(tianjiaItem->getPosition());

	//增减 宝石
	BaseSprite *dizuo = BaseSprite::create("gongjiang_hecheng_jindutiao_bg.png");
	mBg->addChild(dizuo, 1, Tag_DiZuo_BaoShiNum);
	dizuo->setPosition(ccp(mBg->getContentSize().width/2, baoshiImg->getPositionY()-120));
	//
	CCMenu *menu = CCMenu::create();
	mBg->addChild(menu, 1);
	menu->setPosition(CCPointZero);
	//item
	CCMenuItemImage *jia = CCMenuItemImage::create(
		"gongjiang_duanzao_zengjia_baohu_jia.png"
		, "gongjiang_duanzao_zengjia_baohu_jia_select.png"
		, "gongjiang_duanzao_zengjia_baohu_jia_select.png"
		, this, menu_selector(GongJiangBaoShiHeCheng::menuItemClicked_JiaJianBaoShi));
	menu->addChild(jia, 1, Tag_MenuItem_Jia);
	jia->setPosition(ccp(dizuo->getPositionX()+dizuo->getContentSize().width/2+jia->getContentSize().width/2, dizuo->getPositionY()));
	CCMenuItemImage *jian = CCMenuItemImage::create(
		"gongjiang_duanzao_zengjia_baohu_jian.png"
		, "gongjiang_duanzao_zengjia_baohu_jian_select.png"
		, "gongjiang_duanzao_zengjia_baohu_jian_select.png"
		, this, menu_selector(GongJiangBaoShiHeCheng::menuItemClicked_JiaJianBaoShi));
	menu->addChild(jian, 1, Tag_MenuItem_Jian);
	jian->setPosition(ccp(dizuo->getPositionX()-dizuo->getContentSize().width/2-jian->getContentSize().width/2, dizuo->getPositionY()));

	// 数量
	mCurHeChengBaoShiNum = 1;
	CCLabelTTF *numLabel = CCLabelTTF::create("1", fontStr_BookAntiqua, LABEL_SIZE);
	dizuo->addChild(numLabel, 1, Tag_Label_DizuoNum);
	numLabel->setPosition(ccp(dizuo->getContentSize().width/2, dizuo->getContentSize().height/2));

	//“数量” 
	CCLabelTTF *shuliang = CCLabelTTF::create(LFStrings::getValue("shuliang").c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(shuliang);
	shuliang->setColor(LABEL_COLOR);
	shuliang->setPosition(ccp(mBg->getContentSize().width/2, dizuo->getPositionY()-40));

	//所需 宝石
	{
		mCurNeedBaoShiType = CS::HechengBaoshiNeedBaoshiType(mCurHeChengBaoShiType);
		//宝石

		//name
		CCLabelTTF *name = CCLabelTTF::create((LFStrings::getValue("suoxu") + CS::getBaoshiName(mCurNeedBaoShiType)).c_str(), fontStr_kaiti, LABEL_SIZE);
		name->setColor(ccc3(127, 127, 127));
		mBg->addChild(name);
		name->setPosition(ccp(320, 838-486));

		//board
		BaseSprite *board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
		mBg->addChild(board, 1, Tag_Board_SuoXuBaoShi);
		board->setPosition(ccp(mBg->getContentSize().width/2, 838-588));
		//baoshi Img
		BaseSprite *baoshiImg = BaseSprite::create(
			ImageManager::getManager()->getBaoshiFilename(mCurNeedBaoShiType));
		board->addChild(baoshiImg);
		baoshiImg->setPosition(ccp(46, 128-46));
		//baoshi NUM
		unsigned int baoshi_need = mCurHeChengBaoShiNum *  CS::HechengBaoshiNeedBaoshiCount(mCurHeChengBaoShiType);
		unsigned int baoshi_all = 0;
		WuPin* ww = WuPinManager::getManager()->getWuPin((ItemsType) mCurNeedBaoShiType);
		if(ww != NULL)
		{
			baoshi_all = ww->getCount();
		}
		//label need
		char buf[20];
		sprintf(buf, "%d/", baoshi_need);
		CCLabelTTF *needLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		board->addChild(needLabel, 1, Tag_Label_BaoShi_Need);
		needLabel->setAnchorPoint(ccp(1, 0.5));
		needLabel->setPosition(ccp(47, 15));
		//all cailiao
		sprintf(buf, "%d", baoshi_all);
		CCLabelTTF *allLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		board->addChild(allLabel, 1, Tag_Label_BaoShi_All);
		allLabel->setAnchorPoint(ccp(0, 0.5));
		allLabel->setPosition(needLabel->getPosition());
		if(baoshi_all < baoshi_need) 
		{
			allLabel->setColor(ccc3(255, 0, 0));
			mErrorInfo = LFStrings::getValue("hechengsuoxubaoshibuzu");
		}
	}
	//所需金币
	{
		//board
		BaseSprite *jinbiBoard = BaseSprite::create("gongjiang_qianghua_yinbi_bg.png");
		mBg->addChild(jinbiBoard, 1, Tag_Board_SuoXuJinBi);
		jinbiBoard->setPosition(ccp(mBg->getContentSize().width/2, 838-680));
		//"金币"
		BaseSprite *jinbiIcon = BaseSprite::create("jinbi_tubiao.png");
		jinbiBoard->addChild(jinbiIcon);
		jinbiIcon->setScale(0.8);
		jinbiIcon->setPosition(ccp(jinbiBoard->getContentSize().width/3, jinbiBoard->getContentSize().height/2));
		//金币
		unsigned int gold_need = CS::HechengBaoshiNeedGold(mCurHeChengBaoShiType) * mCurHeChengBaoShiNum;
		unsigned int gold_total = 0;
		char buf[20];
		sprintf(buf, " %d/", gold_need);
		CCLabelTTF *numLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		jinbiBoard->addChild(numLabel, 1, Tag_Label_JinBi);
		numLabel->setAnchorPoint(ccp(0, 0.5));
		numLabel->setPosition(ccp(jinbiIcon->getPositionX()+10, jinbiIcon->getPositionY()));
		//all gold
		{
			WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
			if (wp != NULL)
			{
				gold_total = wp->getCount();
			}
		}
		sprintf(buf, "%d", gold_total);
		CCLabelTTF *jinbiAllLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		jinbiBoard->addChild(jinbiAllLabel, 1, Tag_Label_JinBiAll);
		jinbiAllLabel->setAnchorPoint(ccp(0, 0.5));
		jinbiAllLabel->setPosition(ccp(numLabel->getPositionX()+numLabel->getContentSize().width, jinbiIcon->getPositionY()));
		//
		if(gold_total < gold_need) 
		{
			jinbiAllLabel->setColor(ccRED);
			mErrorInfo = LFStrings::getValue("suoxuyinbibuzu");
		}
	}

	//menu
	{
		mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(true);
		mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mBg->getContentSize().width/3, 838-784));
	}
}

void GongJiangBaoShiHeCheng::menuItemClicked_JiaJianBaoShi(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	unsigned int baoshi_need = mCurHeChengBaoShiNum *  CS::HechengBaoshiNeedBaoshiCount(mCurHeChengBaoShiType);
	unsigned int baoshi_all = 0;
	WuPin* ww = WuPinManager::getManager()->getWuPin((ItemsType) mCurNeedBaoShiType);
	if(ww != NULL)
	{
		baoshi_all = ww->getCount();
	}

	//宝石不够了 则不能继续添加
	if(baoshi_all <= baoshi_need && tag == Tag_MenuItem_Jia)
	{
		return;
	}

	mErrorInfo.clear();
	//合成宝石 +
	CCLabelTTF *hechengNum = (CCLabelTTF*)mBg->getChildByTag(Tag_DiZuo_BaoShiNum)->getChildByTag(Tag_Label_DizuoNum);
	//所需宝石 +
	CCLabelTTF *needNum = (CCLabelTTF*)mBg->getChildByTag(Tag_Board_SuoXuBaoShi)->getChildByTag(Tag_Label_BaoShi_Need);
	//所有宝石 +
	CCLabelTTF *allNum = (CCLabelTTF*)mBg->getChildByTag(Tag_Board_SuoXuBaoShi)->getChildByTag(Tag_Label_BaoShi_All);
	//所需金币
	CCLabelTTF *yinbiNum = (CCLabelTTF*)mBg->getChildByTag(Tag_Board_SuoXuJinBi)->getChildByTag(Tag_Label_JinBi);
	//所有金币
	CCLabelTTF *jinbiAll = (CCLabelTTF*)mBg->getChildByTag(Tag_Board_SuoXuJinBi)->getChildByTag(Tag_Label_JinBiAll);

	char buf[20];
	if(tag == Tag_MenuItem_Jia)
	{
		++mCurHeChengBaoShiNum;
	}
	else
	{
		if(mCurHeChengBaoShiNum <= 1)
			return;
		--mCurHeChengBaoShiNum;
	}

	sprintf(buf, "%d", mCurHeChengBaoShiNum);
	hechengNum->setString(buf);
	//suoxu baoshi
	baoshi_need = mCurHeChengBaoShiNum *  CS::HechengBaoshiNeedBaoshiCount(mCurHeChengBaoShiType);
	if(baoshi_all < baoshi_need)
	{
		allNum->setColor(ccRED);
		mErrorInfo = LFStrings::getValue("hechengsuoxubaoshibuzu");
	}
	else
	{
		allNum->setColor(ccc3(255,255,255));
	}
	sprintf(buf, "%d/", baoshi_need);
	needNum->setString(buf);
	//jinbi
	unsigned int gold_need = CS::HechengBaoshiNeedGold(mCurHeChengBaoShiType) * mCurHeChengBaoShiNum;
	unsigned int gold_total = 0;
	sprintf(buf, " %d/", gold_need);
	yinbiNum->setString(buf);
	//all
	{
		WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		if (wp != NULL)
		{
			gold_total = wp->getCount();
		}
	}
	sprintf(buf, "%d", gold_total);
	jinbiAll->setString(buf);
	jinbiAll->setPositionX(yinbiNum->getPositionX()+yinbiNum->getContentSize().width);
	if(gold_need > gold_total)
	{
		jinbiAll->setColor(ccRED);
		mErrorInfo = LFStrings::getValue("suoxuyinbibuzu");
	}
	else
	{
		jinbiAll->setColor(ccc3(255, 255, 255));
	}
}

void GongJiangBaoShiHeCheng::BaoShiHeCheng_Rsp(CCObject *pSender)
{
	removeObserver(MSG_BaoshiHechengRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_BaoshiHechengRsp* data = (SPCmd_BaoshiHechengRsp*)((MSG_Rsp*)pSender)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	//成功
	clearnBackGround();

	mMainMenu->setVisible(false);


	CCMenuItemImage *tianjiaItem = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaBaoShi);
	tianjiaItem->setOpacity(0);

	//名字
	CCLabelTTF *label = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_TishiBaoshi);
	label->setString(CS::getBaoshiName(mCurHeChengBaoShiType).c_str());
	//宝石 图片
	BaseSprite *baoshiImg = BaseSprite::create(
		ImageManager::getManager()->getBaoshiFilename(mCurHeChengBaoShiType));
	mBg->addChild(baoshiImg);
	baoshiImg->setPosition(tianjiaItem->getPosition());
	//
	BaseSprite *dizuo = BaseSprite::create("gongjiang_hecheng_jindutiao_bg.png");
	mBg->addChild(dizuo, 1, Tag_DiZuo_BaoShiNum);
	dizuo->setPosition(ccp(mBg->getContentSize().width/2, baoshiImg->getPositionY()-120));
	// 数量
	char buf[10];
	sprintf(buf, "%d", mCurHeChengBaoShiNum);
	CCLabelTTF *numLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
	dizuo->addChild(numLabel, 1, Tag_Label_DizuoNum);
	numLabel->setPosition(ccp(dizuo->getContentSize().width/2, dizuo->getContentSize().height/2));
	//“数量” 
	CCLabelTTF *shuliang = CCLabelTTF::create(LFStrings::getValue("shuliang").c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(shuliang);
	shuliang->setColor(LABEL_COLOR);
	shuliang->setPosition(ccp(mBg->getContentSize().width/2, dizuo->getPositionY()-40));

	CCLabelTTF *tishi = CCLabelTTF::create(LFStrings::getValue("hechengchenggong").c_str(), fontStr_kaiti, 30);
	mBg->addChild(tishi);
	tishi->setPosition(ccp(mBg->getContentSize().width/2, 200));

	//menu
	((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_FanHui))->setVisible(false);
	CCMenu *menu = CCMenu::create();
	mBg->addChild(menu);
	menu->setPosition(CCPointZero);
	CCMenuItemImage *queren = CCMenuItemImage::create(
		"zhuangbei_xiangxi_queding.png"
		, "zhuangbei_xiangxi_queding_select.png"
		, this, menu_selector(GongJiangBaoShiHeCheng::menuItemClicked_HeChengChengGong));
	menu->addChild(queren, 1, Tag_MenuItem_QueRen);
	queren->setPosition(ccp(mBg->getContentSize().width/2, 838-784));
}

void GongJiangBaoShiHeCheng::menuItemClicked_HeChengChengGong(CCObject *pSender)
{
	clearnBackGround();
}

void GongJiangBaoShiHeCheng::onEnter()
{
	BaseLayer::onEnter();

	if (isUI_Guide_GongJiang && g_gongjiang_BsHc_Opend)
	{
		g_gongjiang_BsHc_Opend = false; //clear
		History::getHistory()->putInCurrentHero(Key_FirstGongJiang_BsHc, false);

		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaBaoShi);
		guideLayer->init(CCSize(menuItem->getContentSize().width/4*3, menuItem->getContentSize().height/2)
			, menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue(""), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}
}