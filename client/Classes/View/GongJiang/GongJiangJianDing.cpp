#include "GongJiangJianDing.h"
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

enum 
{
	Tag_Label_Title1,
	Tag_Label_Title2,

	Tag_Label_EquipName,

	Tag_MenuItem_JianDing,
	Tag_MenuItem_FanHui,
	Tag_MenuItem_Close,
	Tag_Img_EquipBg,

	Tag_Dlg_JianDing,
	Tag_Dlg_BG,
	Tag_Dlg_OK,
	Tag_Dlg_Cancel,

	Tag_Board_NeedEquip = 30,

	Tag_Img_EquipDiWen = 50,
	Tag_Img_NeedBoard,
	Tag_CaiLiaoIcon,
	Tag_CaiLiaoName,
	Tag_CaiLiaoCostLabel,
	Tag_CaiLiaoCountLabel,
};


GongJiangXiYouZhuangBeiJianDing* GongJiangXiYouZhuangBeiJianDing::create(int flag, Equipment *equip)
{
	GongJiangXiYouZhuangBeiJianDing *layer = new GongJiangXiYouZhuangBeiJianDing;
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

bool GongJiangXiYouZhuangBeiJianDing::init(int flag, Equipment *equip)
{

	mCurStatus = flag;
	mEquip = equip;
	mCailiaoEquip = NULL;

	mBg = BaseSprite::create("gongjiang_jinglianzhuanke_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 10);
	mMainMenu->setPosition(CCPointZero);

	//返回按钮
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(GongJiangXiYouZhuangBeiJianDing::menuItemClicked_MainMenu));
	mMainMenu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
	fanhui->setPosition(ccp(mBg->getContentSize().width/2, 838-784));

	//鉴定按钮
	CCMenuItemImage *queren = CCMenuItemImage::create(
		"zhuangbei_xiangxi_jianding.png"
		, "zhuangbei_xiangxi_jianding_select.png"
		, this, menu_selector(GongJiangXiYouZhuangBeiJianDing::menuItemClicked_MainMenu));
	mMainMenu->addChild(queren, 1, Tag_MenuItem_JianDing);
	queren->setPosition(ccp(mBg->getContentSize().width/2, 838-784));
	queren->setVisible(false);

	//关闭按钮
	CCMenuItemImage* pClose = CCMenuItemImage::create(
		"zhuangbei_xiangxi_guanbi.png"
		, "zhuangbei_xiangxi_guanbi_select.png"
		, this, menu_selector(GongJiangXiYouZhuangBeiJianDing::menuItemClicked_MainMenu));
	mMainMenu->addChild(pClose, 1, Tag_MenuItem_Close);
	pClose->setPosition(ccp(mBg->getContentSize().width - pClose->getContentSize().width/2 + 10, mBg->getContentSize().height-pClose->getContentSize().height/2-60));

	initZhuangBei();

	CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GongJiangXiYouZhuangBeiJianDing::QueryAssessWeaponRsp), MSG_QueryAssessWeaponRsp, NULL);
	mCmdHlp->cmdQueryAssessWeapon(equip->getEquipId());

	return true;
}

void GongJiangXiYouZhuangBeiJianDing::initZhuangBei()
{
	mMainMenu->retain();
	mBg->removeAllChildren();
	mBg->addChild(mMainMenu, 10);
	mMainMenu->setPosition(CCPointZero);
	mMainMenu->release();
	mMainMenu->setVisible(true);

	//上面背景
	BaseSprite *needBg = BaseSprite::create("gongjiang_jinglianzhuanke_jinglian_bg.png");
	mBg->addChild(needBg, 0, Tag_Img_NeedBoard);
	needBg->setPosition(POS_NeedBg);

	//最上面提示
	CCLabelTTF *tishiZhuangbei = CCLabelTTF::create(LFStrings::getValue("JianDing_Top_String").c_str(), fontStr_kaiti, LABEL_SIZE);
	tishiZhuangbei->setColor(LABEL_COLOR);
	mBg->addChild(tishiZhuangbei, 1, Tag_Label_Title1);
	tishiZhuangbei->setPosition(POS_Title1);

	mErrorInfo.clear();

	//下边中间文字
	CCLabelTTF *title2 = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Title2);
	if (title2)
	{
		title2->removeFromParent();
	}
	title2 = CCLabelTTF::create(LFStrings::getValue("JianDing_Center_String").c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(title2, 1, Tag_Label_Title2);
	title2->setPosition(POS_Title2);
	title2->setColor(LABEL_COLOR);

	//装备底纹
	BaseSprite* diwen = (BaseSprite*)mBg->getChildByTag(Tag_Img_EquipDiWen);
	if(diwen != NULL)
		diwen->removeFromParent();

	BaseSprite *diwenImg = BaseSprite::create("zhuangbei_xiangxi_diwen_xiyou.png");
	mBg->addChild(diwenImg, 1, Tag_Img_EquipDiWen);
	diwenImg->setPosition(POS_EqDiwen);
	//zhuangbei
	BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mEquip);
	diwenImg->addChild(eqImg);
	eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
	diwenImg->setScale(0.50f);

	//装备名字
	CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_EquipName);
	if (eqName != NULL) eqName->setString(mEquip->getName().c_str());
	else {
		eqName = CCLabelTTF::create(mEquip->getName().c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(eqName, 1, Tag_Label_EquipName);
		eqName->setPosition(POS_EqName);
		eqName->setColor(ImageManager::getManager()->getShowColor(mEquip->getEquipColor(), false, mEquip->isPartOfSuit()));
	}
}

void GongJiangXiYouZhuangBeiJianDing::QueryAssessWeaponRsp(CCObject* pObj)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_QueryAssessWeaponRsp);
	GameDirector::getDirector()->hideWaiting();

	Flat_QueryAssessWeaponRsp* rsp = (Flat_QueryAssessWeaponRsp*)((MSG_Rsp*)pObj)->getData();

	if (rsp->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
		return;
	}

	vector<Equipment*> equips;
	if (mEquip->getEquipmentsCanUsedToJieFeng(equips) == STATE_Ok)
	{
		mCailiaoEquip = equips.at(0);
		this->initCailiao(mCailiaoEquip);
		CCNode* pNode = mMainMenu->getChildByTag(Tag_MenuItem_FanHui);
		if (pNode)
		{
			pNode->setVisible(false);
		}
		else
		{
			pNode->setVisible(true);
		}

		pNode = mMainMenu->getChildByTag(Tag_MenuItem_JianDing);
		if (pNode)
		{
			pNode->setVisible(true);
		}
		else
		{
			pNode->setVisible(false);
		}
	}
	else
	{
		const Flat_DuanzaoGroupZBInfo* info = JieFengYinMapManager::getManager()->getNeedEqMaterialInfo(mEquip->getGroupId());
		this->initCailiao(info);
	}
}

void GongJiangXiYouZhuangBeiJianDing::setJianDingBtnVisible(bool e)
{
	CCNode* pNode = mMainMenu->getChildByTag(Tag_MenuItem_FanHui);
	if (pNode)
	{
		pNode->setVisible(!e);
	}

	pNode = mMainMenu->getChildByTag(Tag_MenuItem_JianDing);
	if (pNode)
	{
		pNode->setVisible(e);
	}
}

void GongJiangXiYouZhuangBeiJianDing::selectEquipmentCallback(unsigned long long eqId, Flag_SelectTo_GongJiang flag)
{
	mCailiaoEquip = EquipmentManager::getManager()->getOneEquipment(eqId);
	this->initCailiao(mCailiaoEquip);
	this->setJianDingBtnVisible(true);
}

void GongJiangXiYouZhuangBeiJianDing::initCailiao(const Flat_DuanzaoGroupZBInfo* info)
{
	//board
	BaseSprite *board = (BaseSprite*)mBg->getChildByTag(Tag_Board_NeedEquip);
	if (board)
	{
		board->removeFromParent();
	}
	board = BaseSprite::create("gongjiang_jinglian_suoxu_bg.png");
	mBg->addChild(board, 1, Tag_Board_NeedEquip + 0);
	board->setPosition(ccp(320, 838-584-10));

	//menu
	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 0, 1);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *addItem = CCMenuItemImage::create(
		"gongjiang_duanzao_xuanze_zhuangbei.png"
		, "gongjiang_duanzao_xuanze_zhuangbei_select.png"
		, this, menu_selector(GongJiangXiYouZhuangBeiJianDing::menuItemClicked_addNeedEquip));
	menu->addChild(addItem);
	addItem->setScale(0.5f);
	addItem->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));

	{//第一次进入教学
		bool isMaoxianEnded = false;
		if(!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_ZhuangBeiJianDing))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_ZhuangBeiJianDing);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_ZhuangBeiJianDing);
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(CCSize(addItem->getContentSize().width * addItem->getScaleX(), addItem->getContentSize().height * addItem->getScaleY()), addItem->getParent()->convertToWorldSpace(addItem->getPosition())
				, LFStrings::getValue("CaiLiaoFanHui"), NULL, false);
			mFooterLayer->getFooter()->addChild(guideLayer, 1024);
		}
	}
	if (!info)
	{
		return;
	}

	//材料底纹
	ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
		info->zhuangbei_colour,
		false,
		false,
		false);
	BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
	board->addChild(diwenImg);
	diwenImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
	diwenImg->setScale(0.25f);

	//材料的图片
	string equipfile = info->zhuangbei_pic;
	equipfile = "minute_" + equipfile + ".png";
	BaseSprite *pCailiaoSpr = BaseSprite::create(equipfile);
	board->addChild(pCailiaoSpr);
	pCailiaoSpr->setScale(0.25f);
	pCailiaoSpr->setPosition(diwenImg->getPosition());
	pCailiaoSpr->setTag(Tag_CaiLiaoIcon);

	//材料名字
	CCLabelTTF* pName = CCLabelTTF::create(info->zhuangbei_name.c_str(), fontStr_kaiti, LABEL_SIZE);
	board->addChild(pName);
	pName->setPosition(ccp(board->getContentSize().width/2, -15));
	pName->setTag(Tag_CaiLiaoName);
	pName->setColor(ImageManager::getManager()->getShowColor(info->zhuangbei_colour, false, false));

	int iCost = 1;
	//材料花费的数量
	CCLabelTTF* costLabel = CCLabelTTF::create(CCString::createWithFormat("%d/", iCost)->getCString(), fontStr_kaiti, LABEL_SIZE);
	board->addChild(costLabel);
	costLabel->setPosition(ccp(board->getContentSize().width/2 - 10, pName->getPositionY() - 30));
	costLabel->setTag(Tag_CaiLiaoCostLabel);
	

	//材料的数量
	CCLabelTTF* countLabel = CCLabelTTF::create(CCString::createWithFormat("%d", 0)->getCString(), fontStr_kaiti, LABEL_SIZE);
	countLabel->setAnchorPoint(ccp(0.0f, 0.5f));
	countLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
	board->addChild(countLabel);
	countLabel->setPosition(ccp(costLabel->getPositionX() + 10, costLabel->getPositionY()));
	countLabel->setTag(Tag_CaiLiaoCountLabel);
	countLabel->setColor(ccRED);

}

void GongJiangXiYouZhuangBeiJianDing::initCailiao(Equipment* info)
{
	//board
	BaseSprite *board = (BaseSprite*)mBg->getChildByTag(Tag_Board_NeedEquip);
	if (board)
	{
		board->removeFromParent();
	}
	board = BaseSprite::create("gongjiang_jinglian_suoxu_bg.png");
	mBg->addChild(board, 1, Tag_Board_NeedEquip + 0);
	board->setPosition(ccp(320, 838-584-10));

	//menu
	CCMenu *menu = CCMenu::create();
	board->addChild(menu, 0, 1);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *addItem = CCMenuItemImage::create(
		"gongjiang_duanzao_xuanze_zhuangbei.png"
		, "gongjiang_duanzao_xuanze_zhuangbei_select.png"
		, this, menu_selector(GongJiangXiYouZhuangBeiJianDing::menuItemClicked_addNeedEquip));
	menu->addChild(addItem);
	addItem->setScale(0.5f);
	addItem->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));

	if (!info)
	{
		return;
	}

	//材料底纹
	ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
		info->getEquipColor(),
		info->isShenBing(),
		info->isPartOfSuit(),
		info->haveZhuanke());
	BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
	board->addChild(diwenImg);
	diwenImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
	diwenImg->setScale(0.25f);

	//材料的图片
	BaseSprite *pCailiaoSpr = ImageManager::getManager()->getSpriteOfZhuangbei(info);
	board->addChild(pCailiaoSpr);
	pCailiaoSpr->setScale(0.25f);
	pCailiaoSpr->setPosition(diwenImg->getPosition());
	pCailiaoSpr->setTag(Tag_CaiLiaoIcon);

	//材料名字
	CCLabelTTF* pName = CCLabelTTF::create(info->getName().c_str(), fontStr_kaiti, LABEL_SIZE);
	board->addChild(pName);
	pName->setPosition(ccp(board->getContentSize().width/2, -15));
	pName->setTag(Tag_CaiLiaoName);
	pName->setColor(ImageManager::getManager()->getShowColor(mCailiaoEquip->getEquipColor(), false, mCailiaoEquip->isPartOfSuit()));

 	vector<Equipment*> equips;
	if (mEquip->getEquipmentsCanUsedToJieFeng(equips) != STATE_Ok)
	{
		CCLOG("state   !=   STATE_Ok");
	}
//	equips = EquipmentManager::getManager()->getAllEquipment(Filter_No_Equipped);
	if (equips.size() > 0)
	{
		int iCost = 1;
		//材料花费的数量
		CCLabelTTF* costLabel = CCLabelTTF::create(CCString::createWithFormat("%d/", iCost)->getCString(), fontStr_kaiti, LABEL_SIZE);
		board->addChild(costLabel);
		costLabel->setPosition(ccp(board->getContentSize().width/2 - 10, pName->getPositionY() - 30));
		costLabel->setTag(Tag_CaiLiaoCostLabel);


		//材料的数量
		CCLabelTTF* countLabel = CCLabelTTF::create(CCString::createWithFormat("%d", equips.size())->getCString(), fontStr_kaiti, LABEL_SIZE);
		countLabel->setAnchorPoint(ccp(0.0f, 0.5f));
		countLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
		board->addChild(countLabel);
		countLabel->setPosition(ccp(costLabel->getPositionX() + 10, costLabel->getPositionY()));
		countLabel->setTag(Tag_CaiLiaoCountLabel);

		if (equips.size() < iCost)
		{
			countLabel->setColor(ccRED);
		}
		else
		{
			countLabel->setColor(ccWHITE);
		}
	}
}

void GongJiangXiYouZhuangBeiJianDing::menuItemClicked_addNeedEquip(CCObject *pSender)
{
	GongJiang_EquipSelect *layer = GongJiang_EquipSelect::create(this, selectTo_JianDingNeed, mEquip);
	mFooterLayer->popUpContaintLayer(layer);
}

void GongJiangXiYouZhuangBeiJianDing::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if(tag == Tag_MenuItem_FanHui)
	{
		mFooterLayer->dismmisPopUp();
	}
	else if(tag == Tag_MenuItem_Close)
	{
		mFooterLayer->dismmisPopUp();
	}
	else if(tag == Tag_MenuItem_JianDing)
	{
		if(!mErrorInfo.empty())
		{
			MessageDisplay* layer = MessageDisplay::create(mErrorInfo);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		bool bShowTip = false;
		string infostr = "";
		if (mCailiaoEquip->getActorId() != INVALID_ID)
		{
			bShowTip = true;
			infostr = LFStrings::getValue("JianDing_TunShi_YiZhuangBei");
		}
		//装备精炼过或者强化过，弹出框提示
		else if (mCailiaoEquip->getEquipData().qianghua_data.qianghua_count > 0 ||
			mCailiaoEquip->getEquipData().jinglian_times > 0 )
		{
			bShowTip = true;
			infostr = LFStrings::getValue("JianDing_dlg_String");
		}

		//要鉴定的装备的等级比当前角色的等级高的话，弹框提示
		//Equipment* pEquipInfo = (Equipment*)((CCMenuItemImage*)pSender)->getUserData();
		bool bOffset = false;
		unsigned int id =  mCailiaoEquip->getActorId();
		if (id != INVALID_ID)
		{
			if (mCailiaoEquip->getEquipData().zhuangbei_level > MyselfManager::getManager()->getMyTeamActor(id)->getCurrentProperty().level)
			{
				bShowTip = true;
				bOffset = true;
				infostr = LFStrings::getValue("JianDing_dlg_String");
			}
		}

		if (bShowTip)
		{
			//弹框
			BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
			dialog->setTag(Tag_Dlg_BG);
			dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
			//确认 取消按键  
			CCMenu* menu = CCMenu::create();
			menu->setPosition(CCPointZero);
			{
				CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
					"shangpin_goumai_queren.png",
					"shangpin_goumai_queren_select.png",
					this,SEL_MenuHandler(&GongJiangXiYouZhuangBeiJianDing::dlgBtnCallBack));
				CCMenuItemImage* menuItemCancel = CCMenuItemImage::create(
					"shangpin_goumai_quxiao.png",
					"shangpin_goumai_quxiao_select.png",
					this,SEL_MenuHandler(&GongJiangXiYouZhuangBeiJianDing::dlgBtnCallBack));
	
					menu->addChild(menuItemOk, 1, 10);
					menu->addChild(menuItemCancel, 1, 20);
					menuItemOk->setPosition(ccp(dialog->getContentSize().width/3,70));
					menuItemCancel->setPosition(ccp(dialog->getContentSize().width/3*2, 70));
					menuItemOk->setTag(Tag_Dlg_OK);
					menuItemCancel->setTag(Tag_Dlg_Cancel);

					if (bOffset)
					{
						menuItemCancel->setPosition(ccp(dialog->getContentSize().width/2, 70));
						menuItemOk->setVisible(false);
					}
			}
			//dlg文字
			{
				CCSize size = dialog->getContentSize();
				int label_size = 32;
				string name = CCString::createWithFormat(infostr.c_str(), mEquip->getName().c_str())->getCString();
				CCLabelTTF *titleLabel = CCLabelTTF::create(name.c_str(), fontStr_kaiti, label_size, CCSizeMake(400, 0), kCCTextAlignmentCenter);
				dialog->addChild(titleLabel);
				titleLabel->setAnchorPoint(ccp(0.5,0.5));
				titleLabel->setPosition(ccp(size.width/2, size.height/2+40));
			}

			LFAlert* lfAlert = new LFAlert();
			lfAlert->setTag(Tag_Dlg_JianDing);
			lfAlert->setBaseContainer(dialog,menu);
			mBg->addChild(lfAlert,128);
			lfAlert->release();
		}
		else
		{
			CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GongJiangXiYouZhuangBeiJianDing::AssessWeaponRsp), MSG_AssessWeaponRsp, NULL);
			mCmdHlp->cmdAssessWeapon(mEquip->getEquipId(), mCailiaoEquip->getEquipId());
		}
	}
}

void GongJiangXiYouZhuangBeiJianDing::AssessWeaponRsp(CCObject* pObj)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_AssessWeaponRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_AssessWeaponRsp* rsp = (SPCmd_AssessWeaponRsp*)((MSG_Rsp*)pObj)->getData();
	if (rsp->err_code != Success)
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("JianDing_shibai_String"), false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
		return;
	}

	mFooterLayer->dismmisPopUp();

	UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("JianDing_OK_String"), false);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
}

void GongJiangXiYouZhuangBeiJianDing::dlgBtnCallBack(CCObject* pObj)
{
	CCMenuItemImage* pItem = (CCMenuItemImage*)pObj;
	int iTag = pItem->getTag();

	CCNode* pNode = mBg->getChildByTag(Tag_Dlg_JianDing);
	if (pNode)
	{
		pNode->removeFromParent();
	}

	if (iTag == Tag_Dlg_OK)
	{
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GongJiangXiYouZhuangBeiJianDing::AssessWeaponRsp), MSG_AssessWeaponRsp, NULL);
		mCmdHlp->cmdAssessWeapon(mEquip->getEquipId(), mCailiaoEquip->getEquipId());
	}
}