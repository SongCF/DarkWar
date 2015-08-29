#include "GongJiangBaoShiXiangQian.h"
#include "font.h"
#include "GongJiangBaoShiXiangQian_BaoShiSelect.h"
#include "EquipmentManager.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "MessageDisplay.h"
#include "GameDirector.h"
#include "GuideLayerBase.h"
#include "History.h"


#define LABEL_SIZE 20
#define LABEL_COLOR ccc3(127,127,127)

#define POS_Equip ccp(320, 838-290)
#define POS_BaoShi ccp(320, 838-580)

#define STR_MenuFile_XiangQian_Normal "gongjiang_anniu_xiangqian_pingshi.png"
#define STR_MenuFile_XiangQian_Select "gongjiang_anniu_xiangqian_pingshi.png"
#define STR_MenuFile_XiangQian_Disable "gongjiang_anniu_xiangqian_select.png"

#define STR_MenuFile_CaiXie_Normal "gongjiang_anniu_chaixie_pingshi.png"
#define STR_MenuFile_CaiXie_Select "gongjiang_anniu_chaixie_pingshi.png"
#define STR_MenuFile_CaiXie_Disable "gongjiang_anniu_chaixie_select.png"

enum 
{
	Tag_Label_ZhuangBeiName,
	Tag_Label_BaoShiName,

	Tag_MenuItem_XiangQian,
	Tag_MenuItem_CaiXie,
	Tag_MenuItem_QueRen,
	Tag_MenuItem_FanHui,
	Tag_MenuItem_TianJiaZB,
	Tag_MenuItem_TianJiaBS,

	Tag_Board_BaoshiBiankuang,
	Tag_Img_EquipAndDiWen,
	Tag_Img_BaoShi,

	Tag_Board_SuoXuYinbi,
	Tag_Label_Yinbi,
	Tag_Label_CxBaoShiTs,
	Tag_Label_CxBaoShiCg,
};

GongJiangBaoShiXiangQian* GongJiangBaoShiXiangQian::create(int flag, Equipment *equip)
{
	GongJiangBaoShiXiangQian *layer = new GongJiangBaoShiXiangQian;
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

bool GongJiangBaoShiXiangQian::init(int flag, Equipment *equip)
{    
	equipid=-1;
	mComFromGJ_equip = equip;
	if(mComFromGJ_equip != NULL) 
		mComeFromEquipDetail = true;
	else 
		mComeFromEquipDetail = false;

	mCurStatus = flag;

	mBg = BaseSprite::create("gongjiang_xiangqian_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 10);
	mMainMenu->setPosition(CCPointZero);

	CCMenuItemImage* xiangqian = CCMenuItemImage::create(
		STR_MenuFile_XiangQian_Normal
		, STR_MenuFile_XiangQian_Select
		, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_HeadMenu));
	mMainMenu->addChild(xiangqian, 1, Tag_MenuItem_XiangQian);
	xiangqian->setPosition(ccp(170, 838-39));

	CCMenuItemImage *caixie = CCMenuItemImage::create(
		STR_MenuFile_CaiXie_Normal
		, STR_MenuFile_CaiXie_Select
		, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_HeadMenu));
	mMainMenu->addChild(caixie, 1, Tag_MenuItem_CaiXie);
	caixie->setPosition(ccp(469, 838-39));

	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_MainMenu));
	mMainMenu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
	fanhui->setPosition(ccp(mBg->getContentSize().width/2, 838-784));

	CCMenuItemImage *queren = CCMenuItemImage::create(
		"zhuangbei_xiangxi_queding.png"
		, "zhuangbei_xiangxi_queding_select.png"
		, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_MainMenu));
	mMainMenu->addChild(queren, 1, Tag_MenuItem_QueRen);
	queren->setPosition(ccp(mBg->getContentSize().width/3*2, 838-784));
	queren->setVisible(false);

	CCMenuItemImage *tianjiaZB = CCMenuItemImage::create(
		"gongjiang_duanzao_xuanze_zhuangbei.png"
		, "gongjiang_duanzao_xuanze_zhuangbei_select.png"
		, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_MainMenu));
	mMainMenu->addChild(tianjiaZB, 1, Tag_MenuItem_TianJiaZB);
	tianjiaZB->setPosition(ccp(mBg->getContentSize().width/2, 838-294));

	CCMenuItemImage *tianjiaBS = CCMenuItemImage::create(
		"gongjiang_duanzao_xuanze_zhuangbei.png"
		, "gongjiang_duanzao_xuanze_zhuangbei_select.png"
		, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_MainMenu));
	mMainMenu->addChild(tianjiaBS, 1, Tag_MenuItem_TianJiaBS);
	tianjiaBS->setPosition(ccp(mBg->getContentSize().width/2, 838-585));

	if(flag == gongjiang_xiangqian)
		menuItemClicked_HeadMenu(xiangqian);
	else 
		menuItemClicked_HeadMenu(caixie);
	return true;
}

void GongJiangBaoShiXiangQian::cleanBackGround()
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

	CCLabelTTF *tishiZhuangbei = CCLabelTTF::create(LFStrings::getValue("qiangxuanzezhuangbei").c_str(), fontStr_kaiti, LABEL_SIZE);
	tishiZhuangbei->setColor(LABEL_COLOR);
	mBg->addChild(tishiZhuangbei, 1, Tag_Label_ZhuangBeiName);
	tishiZhuangbei->setPosition(ccp(320, 838-105));

	if(mCurStatus == gongjiang_xiangqian)
	{
		CCLabelTTF *tishiBaoshi = CCLabelTTF::create(LFStrings::getValue("xuanzebaoshi").c_str(), fontStr_kaiti, LABEL_SIZE);
		tishiBaoshi->setColor(LABEL_COLOR);
		mBg->addChild(tishiBaoshi, 1, Tag_Label_BaoShiName);
		tishiBaoshi->setPosition(ccp(320, 838-486));

		mMainMenu->getChildByTag(Tag_MenuItem_TianJiaBS)->setVisible(true);
		BaseSprite *board = BaseSprite::create("gongjiang_baoshi_waikuang.png");
		mBg->addChild(board, 1, Tag_Board_BaoshiBiankuang);
		board->setPosition(ccp(320, 838-585));

		((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaBS))->setOpacity(255);

		

	}
	else
	{
		mMainMenu->getChildByTag(Tag_MenuItem_TianJiaBS)->setVisible(false);
	}
	((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaZB))->setOpacity(255);

	mHasBaoshi = false;
	mHasEquip = false;
	mErrorInfo.clear();
}

void GongJiangBaoShiXiangQian::selectEquipmentCallback(unsigned long long eqId, Flag_SelectTo_GongJiang flag)
//void GongJiangBaoShiXiangQian::setSelectedEquipmentId(unsigned long long eqID)
{	
	mErrorInfo.clear();
	mCurEquip = EquipmentManager::getManager()->getOneEquipment(eqId);
	mHasEquip = true;
	//1.diwen
	{
		((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaZB))->setOpacity(0);

		BaseSprite* diwen = (BaseSprite*)mBg->getChildByTag(Tag_Img_EquipAndDiWen);
		if(diwen != NULL)
			diwen->removeFromParent();

		ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			mCurEquip->getEquipColor(),
			false,
			mCurEquip->isPartOfSuit(),
			mCurEquip->haveZhuanke());
		BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
		mBg->addChild(diwenImg, 1, Tag_Img_EquipAndDiWen);
		diwenImg->setPosition(POS_Equip);
		//zhuangbei
		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurEquip);
		diwenImg->addChild(eqImg);
		eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
		diwenImg->setScale(0.50);

		//name
		CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_ZhuangBeiName);
		eqName->setString(mCurEquip->getName().c_str());

		if(mCurStatus != gongjiang_xiangqian)
		{
		// 拆卸宝石提示
				CCLabelTTF *cqlable= CCLabelTTF::create(LFStrings::getValue("cxbswxhjws").c_str(), fontStr_kaiti,22);
				mBg->addChild(cqlable, 1, Tag_Label_CxBaoShiTs);
				cqlable->setPosition(ccp(320, 838-486));
				cqlable->setColor(ccc3(235,166,12));
		}
	}

	if(mCurStatus == gongjiang_xiangqian && mHasBaoshi)
	{
		refreshXiangQian_UI();
	}
	else if(mCurStatus == gongjiang_caixie)
	{
		refreshCaiXie_UI();
	}
}

void GongJiangBaoShiXiangQian::setSelectedBaoshiId(unsigned int baoshiID)
{

	if(mBg->getChildByTag(Tag_Label_CxBaoShiTs)!=NULL){
			mBg->removeChildByTag(Tag_Label_CxBaoShiTs);
	}
	mErrorInfo.clear();
	mCurBaoShiType = baoshiID;
	mHasBaoshi = true;
	{

		BaseSprite *baoshi = (BaseSprite*)mBg->getChildByTag(Tag_Img_BaoShi);
		if(baoshi != NULL)
			baoshi->removeFromParent();
		
		BaseSprite *baoshiImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(mCurBaoShiType));
		mBg->addChild(baoshiImg, 1, Tag_Img_BaoShi);
		baoshiImg->setPosition(POS_BaoShi);
		


		((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaBS))->setOpacity(0);

		//name
		CCLabelTTF *name = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_BaoShiName);
		name->setString(CS::getBaoshiName(mCurBaoShiType).c_str());
	}

	if(mHasEquip)
	{
		refreshXiangQian_UI();
	}
}

void GongJiangBaoShiXiangQian::menuItemClicked_HeadMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();
	CCMenuItemImage *xiangqian = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_XiangQian);
	CCMenuItemImage *caixie = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_CaiXie);

	if(tag == Tag_MenuItem_XiangQian)
	{
		
		if(mBg->getChildByTag(Tag_Label_CxBaoShiTs)!=NULL){
			mBg->removeChildByTag(Tag_Label_CxBaoShiTs);
		}
		mCurStatus = gongjiang_xiangqian;
		xiangqian->initWithNormalImage(
			STR_MenuFile_XiangQian_Disable
			, STR_MenuFile_XiangQian_Select
			, NULL, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_HeadMenu));
		caixie->initWithNormalImage(
			STR_MenuFile_CaiXie_Normal
			, STR_MenuFile_CaiXie_Select
			, NULL, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_HeadMenu));
	}
	else
	{
		mCurStatus = gongjiang_caixie;
		xiangqian->initWithNormalImage(
			STR_MenuFile_XiangQian_Normal
			, STR_MenuFile_XiangQian_Select
			, NULL, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_HeadMenu));
		caixie->initWithNormalImage(
			STR_MenuFile_CaiXie_Disable
			, STR_MenuFile_CaiXie_Select
			, NULL, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_HeadMenu));
	}
	
	cleanBackGround();

	if(mComeFromEquipDetail)
	{
		mComeFromEquipDetail = false;
		//setSelectedEquipmentId(mComFromGJ_equip->getEquipId());
		selectEquipmentCallback(mComFromGJ_equip->getEquipId(), selectTo_BaoShiXiangQian);
	}
}

void GongJiangBaoShiXiangQian::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if(tag == Tag_MenuItem_FanHui)
	{
		mFooterLayer->dismmisPopUp();
	}
	else if(tag == Tag_MenuItem_TianJiaZB)
	{
		if(mCurStatus == gongjiang_xiangqian)
		{
			if(mBg->getChildByTag(Tag_Label_CxBaoShiTs)!=NULL){
			mBg->removeChildByTag(Tag_Label_CxBaoShiTs);
		    }
			GongJiang_EquipSelect *layer = GongJiang_EquipSelect::create(this, selectTo_BaoShiXiangQian);
			mFooterLayer->popUpContaintLayer(layer);
			
		}
		else
		{
			GongJiang_EquipSelect *layer = new GongJiang_EquipSelect(this, selectTo_BaoShiCaiXie);
			mFooterLayer->popUpContaintLayer(layer);
			layer->release();
			
		}
	}
	else if(tag == Tag_MenuItem_TianJiaBS)
	{
		GongJiangBaoShiXiangQian_BaoShiSelect *layer = GongJiangBaoShiXiangQian_BaoShiSelect::create(this);
		mFooterLayer->popUpContaintLayer(layer);

	}
	else if(tag == Tag_MenuItem_QueRen)
	{
		if(!mErrorInfo.empty())
		{
			MessageDisplay* layer = MessageDisplay::create(mErrorInfo);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		if(mCurStatus == gongjiang_xiangqian)
		{
			addObserver(callfuncO_selector(GongJiangBaoShiXiangQian::XiangQian_Rsp), MSG_ZBXiangBaoshiRsp);
			SPCmd_ZBXiangBaoshi rep;
			rep.baoshi_type = mCurBaoShiType;
			rep.zhuangbei_id = mCurEquip->getEquipId();
			mCmdHlp->cmdZBXiangBaoshi(rep);

		}
		else if(mCurStatus == gongjiang_caixie)
		{
			addObserver(callfuncO_selector(GongJiangBaoShiXiangQian::ChaiXie_Rsp), MSG_ZBChaiBaoshiRsp);
			SPCmd_ZBChaiBaoshi rep;
			rep.zhuangbei_id = mCurEquip->getEquipId();
			mCmdHlp->cmdZBChaiBaoshi(rep);
		}
	}
}

void GongJiangBaoShiXiangQian::refreshXiangQian_UI()
{
	mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(true);
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setVisible(true);
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mBg->getContentSize().width/3, 838-784));

	//镶嵌拆卸不再消耗银币
/*
	//镶嵌所需银币
	{
		//board
		BaseSprite *yinbiBoard = (BaseSprite*)mBg->getChildByTag(Tag_Board_SuoXuYinbi);
		if(yinbiBoard != NULL)
			yinbiBoard->removeFromParent();
		yinbiBoard = BaseSprite::create("gongjiang_qianghua_yinbi_bg.png");
		mBg->addChild(yinbiBoard, 1, Tag_Board_SuoXuYinbi);
		yinbiBoard->setPosition(ccp(mBg->getContentSize().width/2, 838-680));
		//"银币"icon
		BaseSprite *yinbiIcon = BaseSprite::create("yinbi_tubiao.png");
		yinbiBoard->addChild(yinbiIcon);
		yinbiIcon->setScale(0.8f);
		yinbiIcon->setPosition(ccp(yinbiBoard->getContentSize().width/3, yinbiBoard->getContentSize().height/2));
		//银币 need
		unsigned int silver_need = CS::XiangbaoshiNeedSilver(mCurEquip->getLevel(), mCurBaoShiType);
		unsigned int silver_total = 0;
		char buf[20];
		sprintf(buf, " %d/", silver_need);
		CCLabelTTF *numLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		yinbiBoard->addChild(numLabel, 1, Tag_Label_Yinbi);
		numLabel->setAnchorPoint(ccp(0, 0.5f));
		numLabel->setPosition(ccp(yinbiIcon->getPositionX()+10, yinbiIcon->getPositionY()));
		//yinbi  all
		{
			WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
			if (wp != NULL)
			{
				silver_total = wp->getCount();
			}
		}
		sprintf(buf, "%d", silver_total);
		CCLabelTTF *yinbiAllLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		yinbiBoard->addChild(yinbiAllLabel);
		yinbiAllLabel->setAnchorPoint(ccp(0, 0.5));
		yinbiAllLabel->setPosition(ccp(numLabel->getPositionX()+numLabel->getContentSize().width, yinbiIcon->getPositionY()));
		if(silver_total < silver_need) 
		{
			yinbiAllLabel->setColor(ccRED);
			mErrorInfo = LFStrings::getValue("suoxuyinbibuzu");
		}
	}*/

	//menu
	{
		mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(true);
		mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mBg->getContentSize().width/3, 838-784));
	}
}

void GongJiangBaoShiXiangQian::refreshCaiXie_UI()
{
	//拆卸在前、记录有些什么宝石， 拆卸后展示给用户看
	mCaiXieBaoShi_Num = mCurEquip->getBaoShiNum();
	mCaiXieBaoShi_Type[0] = mCurEquip->getEquipData().diamond_type_1;
	mCaiXieBaoShi_Type[1] = mCurEquip->getEquipData().diamond_type_2;
	mCaiXieBaoShi_Type[2] = mCurEquip->getEquipData().diamond_type_3;
	//

	mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(true);
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setVisible(true);
	mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mBg->getContentSize().width/3, 838-784));


	//镶嵌拆卸不再消耗银币
/*	//拆卸所需银币
	{
		//board
		BaseSprite *yinbiBoard = (BaseSprite*)mBg->getChildByTag(Tag_Board_SuoXuYinbi);
		if(yinbiBoard != NULL)
			yinbiBoard->removeFromParent();
		yinbiBoard = BaseSprite::create("gongjiang_qianghua_yinbi_bg.png");
		mBg->addChild(yinbiBoard, 1, Tag_Board_SuoXuYinbi);
		yinbiBoard->setPosition(ccp(mBg->getContentSize().width/2, 838-600));
		//"银币"icon
		BaseSprite *yinbiIcon = BaseSprite::create("yinbi_tubiao.png");
		yinbiBoard->addChild(yinbiIcon);
		yinbiIcon->setScale(0.8f);
		yinbiIcon->setPosition(ccp(yinbiBoard->getContentSize().width/3, yinbiBoard->getContentSize().height/2));
		//银币 need
		unsigned int silver_need = 0;
		vector<Equipment::BaoshiShuxing> shuxingVec = mCurEquip->getAllBaoshiShuxing();
		for (int i=0; i!=mCurEquip->getBaoShiNum(); ++i)
		{
			silver_need += CS::XiangbaoshiNeedSilver(mCurEquip->getLevel(), shuxingVec.at(i).type);
		}
		char buf[20];
		sprintf(buf, " %d/", silver_need);
		CCLabelTTF *numLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		yinbiBoard->addChild(numLabel, 1, Tag_Label_Yinbi);
		numLabel->setAnchorPoint(ccp(0, 0.5));
		numLabel->setPosition(ccp(yinbiIcon->getPositionX()+10, yinbiIcon->getPositionY()));
		//yinbi all
		unsigned int silver_total = 0;
		{
			WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
			if (wp != NULL)
			{
				silver_total = wp->getCount();
			}
		}
		sprintf(buf, "%d", silver_total);
		CCLabelTTF *yinbiAllLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		yinbiBoard->addChild(yinbiAllLabel);
		yinbiAllLabel->setAnchorPoint(ccp(0, 0.5));
		yinbiAllLabel->setPosition(ccp(numLabel->getPositionX()+numLabel->getContentSize().width, yinbiIcon->getPositionY()));
		//
		if(silver_total < silver_need) 
		{
			yinbiAllLabel->setColor(ccRED);
			mErrorInfo = LFStrings::getValue("suoxuyinbibuzu");
		}
	}*/

	//menu
	{
		mMainMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(true);
		mMainMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mBg->getContentSize().width/3, 838-784));
	}
}

void GongJiangBaoShiXiangQian::XiangQian_Rsp(CCObject *msg)
{
	removeObserver(MSG_ZBXiangBaoshiRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_ZBXiangBaoshiRsp *data = (SPCmd_ZBXiangBaoshiRsp*)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
	cleanBackGround();
	mBg->getChildByTag(Tag_Board_BaoshiBiankuang)->removeFromParent();
	mMainMenu->setVisible(false);


	//1.diwen
	{
		BaseSprite* diwen = (BaseSprite*)mBg->getChildByTag(Tag_Img_EquipAndDiWen);
		if(diwen != NULL)
			diwen->removeFromParent();

		ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			mCurEquip->getEquipColor(),
			false,//mCurEquip->isShenBing(),
			mCurEquip->isPartOfSuit(),
			mCurEquip->haveZhuanke());
		BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
		mBg->addChild(diwenImg, 1, Tag_Img_EquipAndDiWen);
		diwenImg->setPosition(POS_Equip);
		//zhuangbei
		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurEquip);
		diwenImg->addChild(eqImg);
		eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
		diwenImg->setScale(0.50);
		//name
		CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_ZhuangBeiName);
		eqName->setString(mCurEquip->getName().c_str());

		//baoshi name
		mBg->getChildByTag(Tag_Label_BaoShiName)->removeFromParent();
	}

	//menu
	((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_FanHui))->setVisible(false);
	CCMenu *menu = CCMenu::create();
	mBg->addChild(menu);
	menu->setPosition(CCPointZero);
	CCMenuItemImage *queren = CCMenuItemImage::create(
		"zhuangbei_xiangxi_queding.png"
		, "zhuangbei_xiangxi_queding_select.png"
		, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_WanCheng));
	menu->addChild(queren, 1, Tag_MenuItem_QueRen);
	queren->setPosition(ccp(mBg->getContentSize().width/2, 838-784));


	if(equipid!=mCurEquip->getEquipId()){
		if( Labeltfarry.size()>0)
		    Labeltfarry.clear();
		equipid=mCurEquip->getEquipId();
	}
    
	std::vector<Equipment::BaoshiShuxing> shuxing_vec=mCurEquip->getAllBaoshiShuxing();
	for(int i=0;i<shuxing_vec.size();i++){


		CCLabelTTF* lable = CCLabelTTF::create((getBaoshiShuxing(shuxing_vec[i].type)).c_str(), fontStr_kaiti,24);
		float  centerjl=(180+220-shuxing_vec.size()*lable->getContentSize().height)/2;
		Labeltfarry.push_back(lable);
		addChild(lable);
		lable->setAnchorPoint(ccp(0, 0.5));
		lable->setPosition(ccp(mWinSize.width/2-lable->getContentSize().width/2,mBg->getPositionY()-centerjl-i*lable->getContentSize().height));
		lable->setTag(TAG_BaseBody_LABLE_SHUXING);
		lable->setColor(ccc3(255,255,255));
		
	}

	CCLabelTTF *cglable= CCLabelTTF::create(LFStrings::getValue("Xqcg").c_str(), fontStr_kaiti, 22);
	mBg->addChild(cglable, 1, Tag_Label_CxBaoShiCg);
	cglable->setPosition(ccp(320, 838-486));
	cglable->setColor(ccc3(38,229,35));
	
}

void GongJiangBaoShiXiangQian::ChaiXie_Rsp(CCObject *msg)
{
	removeObserver(MSG_ZBChaiBaoshiRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_ZBChaiBaoshiRsp *data = (SPCmd_ZBChaiBaoshiRsp*)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	cleanBackGround();
	mMainMenu->setVisible(false);

	//1.diwen
	{
		BaseSprite* diwen = (BaseSprite*)mBg->getChildByTag(Tag_Img_EquipAndDiWen);
		if(diwen != NULL)
			diwen->removeFromParent();

		ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			mCurEquip->getEquipColor(),
			false,//mCurEquip->isShenBing(),
			mCurEquip->isPartOfSuit(),
			mCurEquip->haveZhuanke());
		BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
		mBg->addChild(diwenImg, 1, Tag_Img_EquipAndDiWen);
		diwenImg->setPosition(POS_Equip);
		//zhuangbei
		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurEquip);
		diwenImg->addChild(eqImg);
		eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
		diwenImg->setScale(0.50);

		//name
		CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_ZhuangBeiName);
		eqName->setString(mCurEquip->getName().c_str());
	}

	//2. 宝石   //180
	if(mCaiXieBaoShi_Num == 2)
	{//没有宝石 type一定为0
		unsigned int curIdx = 0;
		if(mCaiXieBaoShi_Type[curIdx] == 0) ++curIdx;
		{
			//  1
			//board
			BaseSprite *board = BaseSprite::create("gongjiang_baoshi_waikuang.png");
			mBg->addChild(board, 1, Tag_Board_BaoshiBiankuang);
			board->setPosition(ccp(320-180/2, 838-600));
			//img
			BaseSprite *baoshiImg = BaseSprite::create(ImageManager::getManager()->getBaoshiFilename(mCaiXieBaoShi_Type[curIdx]));
			board->addChild(baoshiImg, 1, Tag_Img_BaoShi);
			baoshiImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2+4));
			//name
			CCLabelTTF *tishiBaoshi = CCLabelTTF::create(CS::getBaoshiName(mCaiXieBaoShi_Type[curIdx]).c_str(), fontStr_kaiti, LABEL_SIZE);
			tishiBaoshi->setColor(LABEL_COLOR);
			mBg->addChild(tishiBaoshi, 1, Tag_Label_BaoShiName);
			tishiBaoshi->setPosition(ccp(board->getPositionX(), 838-486));
		}
		++curIdx;
		if(mCaiXieBaoShi_Type[curIdx] == 0) ++curIdx;
		{
			//   2
			//board
			BaseSprite *board = BaseSprite::create("gongjiang_baoshi_waikuang.png");
			mBg->addChild(board, 1, Tag_Board_BaoshiBiankuang);
			board->setPosition(ccp(320+180/2, 838-600));
			//img
			BaseSprite *baoshiImg = BaseSprite::create(ImageManager::getManager()->getBaoshiFilename(mCaiXieBaoShi_Type[curIdx]));
			board->addChild(baoshiImg, 1, Tag_Img_BaoShi);
			baoshiImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2+4));
			//name
			CCLabelTTF *tishiBaoshi = CCLabelTTF::create(CS::getBaoshiName(mCaiXieBaoShi_Type[curIdx]).c_str(), fontStr_kaiti, LABEL_SIZE);
			tishiBaoshi->setColor(LABEL_COLOR);
			mBg->addChild(tishiBaoshi, 1, Tag_Label_BaoShiName);
			tishiBaoshi->setPosition(ccp(board->getPositionX(), 838-486));
		}
	}
	else if(mCaiXieBaoShi_Num == 1)
	{
		unsigned int curIdx = 0;
		for (int xx=0; xx!=3; ++xx)
			if(mCaiXieBaoShi_Type[xx] != 0)  
			{
				curIdx = xx;
				break;
			}
		//board
		BaseSprite *board = BaseSprite::create("gongjiang_baoshi_waikuang.png");
		mBg->addChild(board, 1, Tag_Board_BaoshiBiankuang);
		board->setPosition(ccp(320, 838-600));
		//img
		BaseSprite *baoshiImg = BaseSprite::create(ImageManager::getManager()->getBaoshiFilename(mCaiXieBaoShi_Type[curIdx]));
		board->addChild(baoshiImg, 1, Tag_Img_BaoShi);
		baoshiImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2+4));
		//name
		CCLabelTTF *tishiBaoshi = CCLabelTTF::create(CS::getBaoshiName(mCaiXieBaoShi_Type[curIdx]).c_str(), fontStr_kaiti, LABEL_SIZE);
		tishiBaoshi->setColor(LABEL_COLOR);
		mBg->addChild(tishiBaoshi, 1, Tag_Label_BaoShiName);
		tishiBaoshi->setPosition(ccp(board->getPositionX(), 838-486));
	}
	else if (mCaiXieBaoShi_Num == 3)
	{
		for (int curIdx=0; curIdx!=3; ++curIdx)
		{
			//board
			BaseSprite *board = BaseSprite::create("gongjiang_baoshi_waikuang.png");
			mBg->addChild(board, 1, Tag_Board_BaoshiBiankuang);
			board->setPosition(ccp(320-180+curIdx*180, 838-600));
			//img
			BaseSprite *baoshiImg = BaseSprite::create(ImageManager::getManager()->getBaoshiFilename(mCaiXieBaoShi_Type[curIdx]));
			board->addChild(baoshiImg, 1, Tag_Img_BaoShi);
			baoshiImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2+4));
			//name
			CCLabelTTF *tishiBaoshi = CCLabelTTF::create(CS::getBaoshiName(mCaiXieBaoShi_Type[curIdx]).c_str(), fontStr_kaiti, LABEL_SIZE);
			tishiBaoshi->setColor(LABEL_COLOR);
			mBg->addChild(tishiBaoshi, 1, Tag_Label_BaoShiName);
			tishiBaoshi->setPosition(ccp(board->getPositionX(), 838-486));
		}
	}

	//menu
	((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_FanHui))->setVisible(false);
	CCMenu *menu = CCMenu::create();
	mBg->addChild(menu);
	menu->setPosition(CCPointZero);
	CCMenuItemImage *queren = CCMenuItemImage::create(
		"zhuangbei_xiangxi_queding.png"
		, "zhuangbei_xiangxi_queding_select.png"
		, this, menu_selector(GongJiangBaoShiXiangQian::menuItemClicked_WanCheng));
	menu->addChild(queren, 1, Tag_MenuItem_QueRen);
	queren->setPosition(ccp(mBg->getContentSize().width/2, 838-784));
}

void GongJiangBaoShiXiangQian::menuItemClicked_WanCheng(CCObject *pSender)
{

	//清除属性显示
	for(int i=0;i<Labeltfarry.size();i++){
		this->removeChild(Labeltfarry[i]);
	}

	if(mBg->getChildByTag(Tag_Label_CxBaoShiCg)!=NULL)
		mBg->removeChildByTag(Tag_Label_CxBaoShiCg);

	cleanBackGround();

	if (mCurStatus == gongjiang_xiangqian && mCurEquip != NULL && CS::canZBXiangqianBaoshi(mCurEquip->getEquipId()))
	{
		mHasEquip = true;
		selectEquipmentCallback(mCurEquip->getEquipId(), selectTo_BaoShiXiangQian);
	}


	//镶嵌完成后、查看是否还未出现过拆卸教程、
	//第一次拆卸 教学 是在第一次镶嵌成功后发出
	if (isUI_Guide_GongJiang && mCurStatus == gongjiang_xiangqian)
	{
		bool isFirstChaiXie = false;
		if(History::getHistory()->getBoolFromCurrentHero(Key_FirstGongJiang_ChaiXie, isFirstChaiXie))
		{
			if(isFirstChaiXie)
				isFirstChaiXie = true;
		}
		else
		{
			isFirstChaiXie = true;
		}
		//
		if(isFirstChaiXie)
		{

			History::getHistory()->putInCurrentHero(Key_FirstGongJiang_ChaiXie, false);
			//
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_CaiXie);
			guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
				, LFStrings::getValue("gongjiang_chaixieTishi"), NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
	}
}


std::string GongJiangBaoShiXiangQian::getBaoshiShuxing(unsigned int baoshi_type)
{
	unsigned int minjie = 0;
	unsigned int zhili = 0;
	unsigned int liliang = 0;
	unsigned int tineng = 0;
	CS::getBaoShiJiaCheng(baoshi_type, liliang, minjie, zhili, tineng);
	char str[64];
	if(minjie > 0)
	{
		sprintf(str, "%s +%u",LFStrings::getValue("minjie").c_str(), minjie);
	}
	else if(liliang > 0)
	{
		sprintf(str, "%s +%u", LFStrings::getValue("liliang").c_str(),liliang);
	}
	else if(zhili > 0)
	{
		sprintf(str, "%s +%u", LFStrings::getValue("zhili").c_str(),zhili);
	}
	else
	{
		sprintf(str, "%s +%u", LFStrings::getValue("tineng").c_str(),tineng);
	}

	return str;
}