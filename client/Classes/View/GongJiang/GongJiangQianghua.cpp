#include "GongJiangQianghua.h"
#include "LFStrings.h"
#include "font.h"
#include "EquipmentManager.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "MessageDisplay.h"
#include "GameDirector.h"
#include "LFAlert.h"
#include "GuideLayerBase.h"
#include "History.h"
#include "GongJiangBase.h"
#include "FX_CommonFunc.h"
#include "UnblockManager.h"

using namespace FX_CommonFunc;


#define TAG_HuiTui_DIALOG 111
#define TAG_HuiTui_DIALOG_OK 112
#define TAG_HuiTui_DIALOG_Cancel 113

#define LABEL_SIZE 20
#define LABEL_COLOR ccc3(127,127,127)

#define STR_MenuFile_QiangHua_Normal "gongjiang_anniu_qianghua_dengdai.png"
#define STR_MenuFile_QiangHua_Select "gongjiang_anniu_qianghua_dengdai.png"
#define STR_MenuFile_QiangHua_Disable "gongjiang_anniu_qianghua_select.png"

#define STR_MenuFile_HuiTui_Normal "gongjiang_anniu_huitui_dengdai.png"
#define STR_MenuFile_HuiTui_Select "gongjiang_anniu_huitui_dengdai.png"
#define STR_MenuFile_HuiTui_Disable "gongjiang_anniu_huitui_select.png"

enum menuTag
{
	Tag_Menu_CaiLiao = 1 //强化粉材料说明按钮

	, Tag_MenuItem_Qianghua
	, Tag_MenuItem_HuiTui
	, Tag_MenuItem_FanHui
	, Tag_MenuItem_QueRen
	, Tag_MenuItem_TianJiaZhuangBei
	, Tag_XingXing
	, Tag_Label_Prop

	, Tag_Label_TiShiZhuangBeiName

	, Tag_Img_EquipAndDiWen

	, Tag_Label_JiaCheng_Percent

	, Tag_Menu_QiangHuaLiShiJiLu
	, Tag_Img_QiangHuaLiShiJiLu

	, Tag_CautionDialog = 200
	, Tag_Board_RongLian
	, Tag_Label_RongShiNeedNum
	, Tag_MenuItem_RongShiAdd
	, Tag_MenuItem_RongShiSub
};

GongJiangQianghua* GongJiangQianghua::create(int flag, Equipment *equip)
{
	
	GongJiangQianghua *layer = new GongJiangQianghua;
	if(layer &&layer->init(flag, equip))
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

bool GongJiangQianghua::init(int flag, Equipment *equip)
{
	ishuidian=0;
	ishuidui=0;
	mUI_Guide_FenJieOrHuiTui = 0;

	if(equip != NULL)
	{
		mComeFromEquipDetail = true;
		mComFromGJ_equip = equip;
	}
	else
	{
		mComeFromEquipDetail = false;
	}


	bool bRet = false;
	mCurEquip = NULL;

	do 
	{
		mBg = BaseSprite::create("gongjiang_qianghua_bg.png");
		addChild(mBg);
		mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

		mMainMenu = CCMenu::create();
		mBg->addChild(mMainMenu);
		mMainMenu->setPosition(CCPointZero);

		CCMenuItemImage* qianghua = CCMenuItemImage::create(
			STR_MenuFile_QiangHua_Normal
			, STR_MenuFile_QiangHua_Select
			, this, menu_selector(GongJiangQianghua::menuItemHeadClicked));
		mMainMenu->addChild(qianghua, 1, Tag_MenuItem_Qianghua);
		qianghua->setPosition(ccp(170, 838-39));

		CCMenuItemImage *huitui = CCMenuItemImage::create(
			STR_MenuFile_HuiTui_Normal
			, STR_MenuFile_HuiTui_Select
			, this, menu_selector(GongJiangQianghua::menuItemHeadClicked));
		mMainMenu->addChild(huitui, 1, Tag_MenuItem_HuiTui);
		huitui->setPosition(ccp(469, 838-39));

		CCMenuItemImage *fanhui = CCMenuItemImage::create(
			"shuxing_renwu_fanhui.png"
			, "shuxing_renwu_fanhui_select.png"
			, this, menu_selector(GongJiangQianghua::menuItemClicked));
		mMainMenu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
		fanhui->setPosition(ccp(mBg->getContentSize().width/2, 838-784));

		CCMenuItemImage *qianghuaItem = CCMenuItemImage::create(
			"gongjiang_qianghua_anniu_qianghua.png"
			, "gongjiang_qianghua_anniu_qianghua_select.png"
			, this, menu_selector(GongJiangQianghua::menuItemClicked));
		mMainMenu->addChild(qianghuaItem, 1, Tag_MenuItem_QueRen);
		qianghuaItem->setPosition(ccp(mBg->getContentSize().width/3*2, 838-784));
		m_iQianghuaItem = qianghuaItem;

		CCMenuItemImage *huituiItem = CCMenuItemImage::create(
			"gongjiang_qianghua_anniu_huitui.png"
			, "gongjiang_qianghua_anniu_huitui_select.png"
			, this, menu_selector(GongJiangQianghua::menuItemClicked));
		mMainMenu->addChild(huituiItem, 1, Tag_MenuItem_QueRen);
		huituiItem->setPosition(ccp(mBg->getContentSize().width/3*2, 838-784));
		m_iHuituiItem = huituiItem;
		
		CCMenuItemImage *tianjia = CCMenuItemImage::create(
			"gongjiang_duanzao_xuanze_zhuangbei.png"
			, "gongjiang_duanzao_xuanze_zhuangbei_select.png"
			, this, menu_selector(GongJiangQianghua::menuItemClicked));
		mMainMenu->addChild(tianjia, 1, Tag_MenuItem_TianJiaZhuangBei);
		tianjia->setPosition(ccp(164, 838-274));


		bRet = true;
	} while (0);
	//
	mCurStatus = flag;
	if(mCurStatus == gongjiang_qianghua)
	{
		menuItemHeadClicked(mMainMenu->getChildByTag(Tag_MenuItem_Qianghua));
		
	}
	else if(mCurStatus == gongjiang_huitui)
	{
		menuItemHeadClicked(mMainMenu->getChildByTag(Tag_MenuItem_HuiTui));
	}

	return bRet;
}

void GongJiangQianghua::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
	CCMenu *caiLiaoMenu = (CCMenu*)mBg->getChildByTag(Tag_Menu_CaiLiao);
	if (caiLiaoMenu) caiLiaoMenu->setEnabled(false);
}

void GongJiangQianghua::disableAllTouchEnd()
{
	mMainMenu->setEnabled(true);
	CCMenu *caiLiaoMenu = (CCMenu*)mBg->getChildByTag(Tag_Menu_CaiLiao);
	if (caiLiaoMenu) caiLiaoMenu->setEnabled(true);
}

void GongJiangQianghua::menuItemHeadClicked(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();
	CCMenuItemImage *qianghua = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_Qianghua);
	CCMenuItemImage *huitui = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_HuiTui);

	if(tag == Tag_MenuItem_Qianghua)
	{
	   

		m_iHuituiItem->setVisible(false);
		m_iHuituiItem->setEnabled(false);
		m_iQianghuaItem->setVisible(true);
		m_iQianghuaItem->setEnabled(true);

		qianghua->initWithNormalImage(
			STR_MenuFile_QiangHua_Disable
			, STR_MenuFile_QiangHua_Select
			, NULL, this, menu_selector(GongJiangQianghua::menuItemHeadClicked));
		huitui->initWithNormalImage(
			STR_MenuFile_HuiTui_Normal
			, STR_MenuFile_HuiTui_Select
			, NULL, this, menu_selector(GongJiangQianghua::menuItemHeadClicked));

		mCurStatus = gongjiang_qianghua;
		clearnBackGround(mCurStatus);

		string temp;
		if(mCurEquip!=NULL && mCurEquip->canBeQianghua(temp)){
			//setSelectedEquipmentId(mCurEquip->getEquipId());
			selectEquipmentCallback(mCurEquip->getEquipId(), selectTo_QiangHua);
		}

	}else
	{

		ishuidui=1;
		m_iHuituiItem->setVisible(true);
		m_iHuituiItem->setEnabled(true);
		m_iQianghuaItem->setVisible(false);
		m_iQianghuaItem->setEnabled(false);

		qianghua->initWithNormalImage(
			STR_MenuFile_QiangHua_Normal
			, STR_MenuFile_QiangHua_Select
			, NULL, this, menu_selector(GongJiangQianghua::menuItemHeadClicked));
		huitui->initWithNormalImage(
			STR_MenuFile_HuiTui_Disable
			, STR_MenuFile_HuiTui_Select
			, NULL, this, menu_selector(GongJiangQianghua::menuItemHeadClicked));

		mCurStatus = gongjiang_huitui;
		clearnBackGround(mCurStatus);

		string temp;
		if(mCurEquip!=NULL && mCurEquip->canBeHuiTui(temp))
			//setSelectedEquipmentId(mCurEquip->getEquipId());
			selectEquipmentCallback(mCurEquip->getEquipId(), selectTo_QiangHua);
	}

	if(mComeFromEquipDetail)
	{
		
		mComeFromEquipDetail = false;
		//setSelectedEquipmentId(mComFromGJ_equip->getEquipId());
		selectEquipmentCallback(mComFromGJ_equip->getEquipId(), selectTo_QiangHua);
	}
}

void GongJiangQianghua::menuItemClicked(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();
	if(tag == Tag_MenuItem_FanHui)
	{
		//分解教学
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_FenJie))
		{
			GongJiangBase *layer = GongJiangBase::create(0);
			mFooterLayer->setContaintLayer(layer);
			return;
		}

		if (mFooterLayer->getContentLayer() == this)
			GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		else 
			mFooterLayer->dismmisPopUp();
	}
	else if(tag == Tag_MenuItem_TianJiaZhuangBei)
	{
		//
//		GongjiangQiangHua_EquipSelect *equipSelectLayer = GongjiangQiangHua_EquipSelect::create(this, (Gongjiang_Qianghua_Status)mCurStatus);
		GongJiang_EquipSelect *layer = NULL;
		if (mCurStatus == gongjiang_qianghua)
			layer = GongJiang_EquipSelect::create(this, selectTo_QiangHua);
		else if(mCurStatus == gongjiang_huitui)
			layer = GongJiang_EquipSelect::create(this, selectTo_HuiTui);

		if (layer) mFooterLayer->popUpContaintLayer(layer);
	}
	else if(tag == Tag_MenuItem_QueRen)
	{
		//银币不足就不会发送， 但是要弹出对话框
		//
		
		if(mCurStatus == gongjiang_qianghua)
		{
			ishuidui=0;
			if(mErrorInfomation == LFStrings::getValue("qianghuasuoxucailiaobuzu"))
			{
				//第一次出现材料不足，进行分解教学
				/*
				if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_FenJie))
				{
					GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_FenJie);
					mUI_Guide_FenJieOrHuiTui = 10;
					g_gongjiang_FenJie_Guide = true;
					MessageDisplay* layer = MessageDisplay::create(mErrorInfomation, this);
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
				}
				else
				{
					MessageDisplay* layer = MessageDisplay::create(mErrorInfomation);
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
				}		
				*/

				string temp = LFStrings::getValue("Sxclbz");
	            FXToast* layer = FXToast::create(temp);
	            CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
				return;
			}
			if(mErrorInfomation == LFStrings::getValue("suoxuyinbibuzu"))
			{
				MessageDisplay* layer = MessageDisplay::create(mErrorInfomation);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
				return;
			}
			if(mErrorInfomation == LFStrings::getValue("yisizuidaqianghuacishu"))
			{
				FXToast* layer = FXToast::create(mErrorInfomation, ccWHITE, this);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
				return;
			}
			
			addObserver(callfuncO_selector(GongJiangQianghua::ZB_QiangHua_Rsp), MSG_ZBQianghuaRsp);
			SPCmd_ZBQianghua spc;
			spc.zhuangbei_id = mCurEquip->getEquipId();
			//if (mRongShiNum > 0)	
			//	spc.b_gold_star = true;
			spc.b_gold_star = false;
			mCmdHlp->cmdZBQianghua(spc);
		}
		else //回退
		{
			ishuidui=1;
			if(mErrorInfomation == LFStrings::getValue("suoxujinbibuzu"))
			{
				MessageDisplay* layer = MessageDisplay::create(mErrorInfomation);
				CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
				return;
			}

			//有银星要回退才显示提示
			bool direction = false;
			QianghuaData qianghuaData = mCurEquip->getEquipData().qianghua_data;
// 			//	//如果是回退则要排序
// 			//给 QianghuaData排序  由好到差
// 			for(int i=0; i!=qianghuaData.qianghua_count; ++i)
// 			{
// 				for(int j=i; j!=qianghuaData.qianghua_count; ++j)
// 				{
// 					if(  (qianghuaData.info_arr[i].level < qianghuaData.info_arr[j].level)   
// 						|| (qianghuaData.info_arr[i].level == qianghuaData.info_arr[j].level && 
// 						qianghuaData.info_arr[i].percent < qianghuaData.info_arr[j].percent)  )
// 					{
// 						swap(qianghuaData.info_arr[i], qianghuaData.info_arr[j]);
// 					}
// 				}
// 			}
			sortQianghuaDataAtHuiTui(qianghuaData);

			for(int i=0; i!=qianghuaData.qianghua_count; ++i)
			{
				if(mQianghuaLevel[i] != QianghuaLevel_jin_special && ! mHuiTui_BaoLiu_Select[i]
					&& qianghuaData.info_arr[i].level >= 3)
				{
					direction = true;
					break;
				}
			}

			if (! direction)
			{
				addObserver(callfuncO_selector(GongJiangQianghua::ZB_HuiTui_Rsp), MSG_ZBQianghuaHuituiRsp);

				vector<unsigned char> keep;
				for(int i=0; i!=qianghuaData.qianghua_count; ++i)
				{
					if(mHuiTui_BaoLiu_Select[i])
						keep.push_back(qianghuaData.info_arr[i].id);
				}

				mCmdHlp->cmdZBQianghuaHuitui(mCurEquip->getEquipId(),keep);	
			}
			else
			{
				//弹框提示
				BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
				dialog->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
				dialog->setTag(TAG_HuiTui_DIALOG);
				//label
				CCLabelTTF *label = CCLabelTTF::create(LFStrings::getValue("huituiqueding").c_str(), 
					fontStr_kaiti, 30, CCSize(400, 0), kCCTextAlignmentLeft);
				dialog->addChild(label);
				label->setAnchorPoint(ccp(0.5, 1));
				label->setPosition(ccp(dialog->getContentSize().width/2, 278));
				label->setColor(ccRED);

				//按键
				CCMenu* menu;
				{
					CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
						"shangpin_goumai_queren.png",
						"shangpin_goumai_queren_select.png",
						this,SEL_MenuHandler(&GongJiangQianghua::menuItemClicked_Huitui_OkCancel));


					CCMenuItemImage* menuItemCacel = CCMenuItemImage::create(
						"shangpin_goumai_quxiao.png",
						"shangpin_goumai_quxiao_select.png",
						this,SEL_MenuHandler(&GongJiangQianghua::menuItemClicked_Huitui_OkCancel));

					menu = CCMenu::create(menuItemMakeSure, menuItemCacel, NULL);
					menu->setPosition(CCPointZero);

					menuItemMakeSure->setTag(TAG_HuiTui_DIALOG_OK);
					menuItemCacel->setTag(TAG_HuiTui_DIALOG_Cancel);
					menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/3*2,70));
					menuItemCacel->setPosition(ccp(dialog->getContentSize().width/3, 70));
				}

				//弹框
				{
					LFAlert* lfAlert = new LFAlert();
					lfAlert->setTag(TAG_HuiTui_DIALOG);
					lfAlert->setBaseContainer(dialog,menu);
					CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert, 128, TAG_HuiTui_DIALOG);
					lfAlert->release();
				}
			}

		}
	}
}

void GongJiangQianghua::menuItemClicked_Huitui_OkCancel(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->getChildByTag(TAG_HuiTui_DIALOG)->removeFromParent();

	int tag = ((CCMenuItemImage*)pSender)->getTag();
	if(tag == TAG_HuiTui_DIALOG_OK)
	{
		addObserver(callfuncO_selector(GongJiangQianghua::ZB_HuiTui_Rsp), MSG_ZBQianghuaHuituiRsp);
		vector<unsigned char> keep;
		QianghuaData qianghuaData = mCurEquip->getEquipData().qianghua_data;
// 		//	//如果是回退则要排序
// 		//给 QianghuaData排序  由好到差
// 		for(int i=0; i!=qianghuaData.qianghua_count; ++i)
// 		{
// 			for(int j=i; j!=qianghuaData.qianghua_count; ++j)
// 			{
// 				if(  (qianghuaData.info_arr[i].level < qianghuaData.info_arr[j].level)   
// 					|| (qianghuaData.info_arr[i].level == qianghuaData.info_arr[j].level && 
// 					qianghuaData.info_arr[i].percent < qianghuaData.info_arr[j].percent)  )
// 				{
// 					swap(qianghuaData.info_arr[i], qianghuaData.info_arr[j]);
// 				}
// 			}
// 		}
		sortQianghuaDataAtHuiTui(qianghuaData);
		for(int i=0; i!=qianghuaData.qianghua_count; ++i)
		{
			if(mHuiTui_BaoLiu_Select[i])
				keep.push_back(qianghuaData.info_arr[i].id);
		}
		mCmdHlp->cmdZBQianghuaHuitui(mCurEquip->getEquipId(),keep);	
	}
}

void GongJiangQianghua::ZB_QiangHua_Rsp(CCObject *msg)
{
	removeObserver(MSG_ZBQianghuaRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_ZBQianghuaRsp * data = (SPCmd_ZBQianghuaRsp*)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		mErrorInfomation.clear();
		return;
	}
	CCLOG("---------------------->");
	if(ishuidui!=1){
	  ishuidian=2;
	  ishuidui=0;
	}
	//成功了就刷新界面   或者  来一个动画
	refreshBK_WhenRsp(mCurStatus);
}

void GongJiangQianghua::ZB_HuiTui_Rsp(CCObject *msg)
{
	removeObserver(MSG_ZBQianghuaRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_ZBQianghuaHuituiRsp * data = (SPCmd_ZBQianghuaHuituiRsp*)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		mErrorInfomation.clear();
		return;
	}

	float fadeOutTime = 0.15f, fadetime = 0.75f;
	//回退效果
	for(int i=0; i!=9; ++i)
	{
		if (mHuiTui_BaoLiu_Select[i] || mQianghuaLevel[i] >= 4) continue;  //保留了的 

		BaseSprite *infoBoard = (BaseSprite *)mBg->getChildByTag(Tag_Img_QiangHuaLiShiJiLu + i);
		if (infoBoard == NULL) continue;
		else
		{
			infoBoard->runAction(CCFadeOut::create(fadeOutTime));
		}

		BaseSprite *xingxing = (BaseSprite*)infoBoard->getChildByTag(Tag_XingXing);
		if (xingxing != NULL)
		{
			xingxing->runAction(CCFadeOut::create(fadeOutTime));
		}

		CCLabelTTF *shuxing = (CCLabelTTF*)infoBoard->getChildByTag(Tag_Label_Prop);
		if (shuxing != NULL)
		{
			shuxing->runAction(CCFadeOut::create(fadeOutTime));
		}

		CCMenu *menu = (CCMenu*)infoBoard->getChildByTag(Tag_Menu_QiangHuaLiShiJiLu);
		if(menu == NULL) continue;
		
		CCMenuItemImage *item = (CCMenuItemImage*)menu->getChildByTag(i);
		if (item != NULL)
		{
			item->runAction(CCFadeOut::create(fadeOutTime));
		}

		//
		BaseSprite *b1 = BaseSprite::create("gongjiang_huitui_xiaoguo_1_10_35.png");
		infoBoard->addChild(b1);
		b1->setPosition(ccp(10, 35));
		b1->setOpacity(0);
		b1->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(-60, -80)), CCRotateBy::create(fadetime, -60), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b2 = BaseSprite::create("gongjiang_huitui_xiaoguo_2_55_34.png");
		infoBoard->addChild(b2);
		b2->setPosition(ccp(55, 34));
		b2->setOpacity(0);
		b2->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(-20, -110)), CCRotateBy::create(fadetime, -15), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b3 = BaseSprite::create("gongjiang_huitui_xiaoguo_3_91_59.png");
		infoBoard->addChild(b3);
		b3->setPosition(ccp(91, 59));
		b3->setOpacity(0);
		b3->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(-10, -60)), CCRotateBy::create(fadetime, -60), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b4 = BaseSprite::create("gongjiang_huitui_xiaoguo_4_121_33.png");
		infoBoard->addChild(b4);
		b4->setPosition(ccp(121, 33));
		b4->setOpacity(0);
		b4->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(-30, -110)), CCRotateBy::create(fadetime, -15), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b5 = BaseSprite::create("gongjiang_huitui_xiaoguo_5_146_62.png");
		infoBoard->addChild(b5);
		b5->setPosition(ccp(146, 62));
		b5->setOpacity(0);
		b5->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(-30, -60)), CCRotateBy::create(fadetime, -30), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b6 = BaseSprite::create("gongjiang_huitui_xiaoguo_6_180_31.png");
		infoBoard->addChild(b6);
		b6->setPosition(ccp(180, 31));
		b6->setOpacity(0);
		b6->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(0, -110)), CCRotateBy::create(fadetime, 10), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b7 = BaseSprite::create("gongjiang_huitui_xiaoguo_7_187_63.png");
		infoBoard->addChild(b7);
		b7->setPosition(ccp(187, 63));
		b7->setOpacity(0);
		b7->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(30, -60)), CCRotateBy::create(fadetime, 60), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b8 = BaseSprite::create("gongjiang_huitui_xiaoguo_8_226_18.png");
		infoBoard->addChild(b8);
		b8->setPosition(ccp(226, 18));
		b8->setOpacity(0);
		b8->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(30, -60)), CCRotateBy::create(fadetime, 30), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b9 = BaseSprite::create("gongjiang_huitui_xiaoguo_9_238_35.png");
		infoBoard->addChild(b9);
		b9->setPosition(ccp(238, 35));
		b9->setOpacity(0);
		b9->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(0, -110)), CCRotateBy::create(fadetime, 20), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b10 = BaseSprite::create("gongjiang_huitui_xiaoguo_10_270_62.png");
		infoBoard->addChild(b10);
		b10->setPosition(ccp(270, 62));
		b10->setOpacity(0);
		b10->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(30, -60)), CCRotateBy::create(fadetime, 40), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b11 = BaseSprite::create("gongjiang_huitui_xiaoguo_11_280_25.png");
		infoBoard->addChild(b11);
		b11->setPosition(ccp(280, 25));
		b11->setOpacity(0);
		b11->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(30, -60)), CCRotateBy::create(fadetime, 20), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b12 = BaseSprite::create("gongjiang_huitui_xiaoguo_12_277_52.png");
		infoBoard->addChild(b12);
		b12->setPosition(ccp(277, 52));
		b12->setOpacity(0);
		b12->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(30, -60)), CCRotateBy::create(fadetime, 30), CCFadeOut::create(fadetime), NULL),
			NULL));
		BaseSprite *b13 = BaseSprite::create("gongjiang_huitui_xiaoguo_13_294_30.png");
		infoBoard->addChild(b13);
		b13->setPosition(ccp(294, 30));
		b13->setOpacity(0);
		b13->runAction(CCSequence::create(
			CCFadeTo::create(0.1f, 255),
			CCSpawn::create(CCMoveBy::create(fadetime, ccp(30, -60)), CCRotateBy::create(fadetime, 10), CCFadeOut::create(fadetime), NULL),
			CCCallFuncN::create(this, callfuncN_selector(GongJiangQianghua::HuiTuiEffectEnd)),
			NULL));
	}

	/*if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_HuiTui) && (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_HuiTuiCancel)))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_HuiTuiCancel);
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_HuiTuiCancel);

		CCMenuItem *fanhui = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_FanHui);
		if (fanhui)
		{
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(fanhui->getContentSize()
				, fanhui->getParent()->convertToWorldSpace(fanhui->getPosition())
				, "", NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
	}*/

	//动画玩了再刷新
// 	//成功了就刷新界面   
// 	refreshBK_WhenRsp(mCurStatus);
}

void GongJiangQianghua::HuiTuiEffectEnd(CCNode *node)
{
	//成功了就刷新界面   
	ishuidian=1;
	refreshBK_WhenRsp(mCurStatus);
}

void GongJiangQianghua::refreshBK_WhenRsp(int status)
{
	//if(mCurEquip->getEquipData().qianghua_data.qianghua_count >= mCurEquip->getEquipData().qianghua_data.max_qianghua_time)
	{
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_HuiTui))
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_HuiTui);
			mUI_Guide_FenJieOrHuiTui = 20;
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_HuiTui);
			guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
				, LFStrings::getValue("gongjiang_huituiTishi"), NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}

		//FXToast *layer = FXToast::create(LFStrings::getValue("yisizuidaqianghuacishu"), ccWHITE, this);
		//CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	}

	menuItemHeadClicked(mMainMenu->getChildByTag(Tag_MenuItem_Qianghua));
//	setSelectedEquipmentId(mCurEquip->getEquipId());
}

void GongJiangQianghua::clearnBackGround(int status)
{
	mMainMenu->retain();
	mBg->removeAllChildren();
	mBg->addChild(mMainMenu);
	mMainMenu->release();

	//CCMenuItemImage *querenItem = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_QueRen);
	//querenItem->setVisible(false);

	m_iHuituiItem->setVisible(false);
	m_iHuituiItem->setEnabled(false);
	m_iQianghuaItem->setVisible(false);
	m_iQianghuaItem->setEnabled(false);
	CCMenuItemImage *fanhui = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_FanHui);
	fanhui->setPositionX(mBg->getContentSize().width/2);

	if(status == gongjiang_huitui)
	{
		//label
		CCLabelTTF *tishiZhuangBeiName = CCLabelTTF::create(LFStrings::getValue("qiangxuanzezhuangbei").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(tishiZhuangBeiName, 1, Tag_Label_TiShiZhuangBeiName);
		tishiZhuangBeiName->setPosition(ccp(165, 838-105));
		tishiZhuangBeiName->setColor(LABEL_COLOR);

//		CCLabelTTF *huituiSuoXu = CCLabelTTF::create(LFStrings::getValue("huituisuoxu").c_str(), fontStr_kaiti, LABEL_SIZE);
		/*CCLabelTTF *huituiSuoXu = CCLabelTTF::create("", fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(huituiSuoXu, 1);
		huituiSuoXu->setPosition(ccp(163, 838-485));
		huituiSuoXu->setColor(LABEL_COLOR);
		*/
		CCLabelTTF *baoliuShuXing = CCLabelTTF::create(LFStrings::getValue("gouxuanyibaoliushuxing").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(baoliuShuXing, 1);
		baoliuShuXing->setPosition(ccp(478, 838-105));
		baoliuShuXing->setColor(LABEL_COLOR);


		CCLabelTTF *huituiSuoXu = CCLabelTTF::create(LFStrings::getValue("huiTuiTeachName").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(huituiSuoXu, 1);
		huituiSuoXu->setPosition(ccp(163, 838-485));
		huituiSuoXu->setColor(LABEL_COLOR);

		/*CCLabelTTF *huituiMingXi = CCLabelTTF::create(LFStrings::getValue("huiTuiInfo").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(huituiMingXi, 1);
		huituiMingXi->setPosition(ccp(478, 838-105));
		huituiMingXi->setColor(LABEL_COLOR);*/
	}
	else if(status == gongjiang_qianghua)
	{
		//label
		CCLabelTTF *tishiZhuangBeiName = CCLabelTTF::create(LFStrings::getValue("qiangxuanzezhuangbei").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(tishiZhuangBeiName, 1, Tag_Label_TiShiZhuangBeiName);
		tishiZhuangBeiName->setPosition(ccp(165, 838-105));
		tishiZhuangBeiName->setColor(LABEL_COLOR);

		CCLabelTTF *qianghuaSuoXu = CCLabelTTF::create(LFStrings::getValue("qianghuasuoxu").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(qianghuaSuoXu, 1);
		qianghuaSuoXu->setPosition(ccp(163, 838-485));
		qianghuaSuoXu->setColor(LABEL_COLOR);

		CCLabelTTF *qianghuaMingXi = CCLabelTTF::create(LFStrings::getValue("qianghuamingxi").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(qianghuaMingXi, 1);
		qianghuaMingXi->setPosition(ccp(478, 838-105));
		qianghuaMingXi->setColor(LABEL_COLOR);
	}

	mErrorInfomation.clear();
}

void GongJiangQianghua::selectEquipmentCallback(unsigned long long eqId, Flag_SelectTo_GongJiang flag)
//void GongJiangQianghua::setSelectedEquipmentId(unsigned long long eqID)
{

	
	clearnBackGround(mCurStatus);

	Equipment *equip = EquipmentManager::getManager()->getOneEquipment(eqId);
	mCurEquip = equip;
	CCMenuItemImage *tianjiaIten = (CCMenuItemImage *)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaZhuangBei);

	//1.diwen
	{
		ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			equip->getEquipColor(),
			false,//equip->isShenBing(),
			equip->isPartOfSuit(),
			equip->haveZhuanke());
		
		BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
		mBg->addChild(diwenImg, 1, Tag_Img_EquipAndDiWen);
		diwenImg->setPosition(tianjiaIten->getPosition());
		//zhuangbei
		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(equip);
		diwenImg->addChild(eqImg);
		eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
		diwenImg->setScale(0.50);
		CCLOG("----->%s",eqImg->getFileName().c_str());
	
		  currentzb=eqImg;
		//name
		CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_TiShiZhuangBeiName);
		eqName->setString(equip->getName().c_str());
	}

	//
	//一定要给强化信息排序
	QianghuaData qianghuaData = equip->getEquipData().qianghua_data;
//
	//2.基础攻击   强化加成百分比
	string leixingStr;
	{
		float baizi;
		switch (equip->getType())
		{
		case ZhuangbeiType_DanshouJian:
		case ZhuangbeiType_ShuangshouBishou:
		case ZhuangbeiType_ShuangshouFu:
		case ZhuangbeiType_DanshouFazhang:
		case ZhuangbeiType_ShuangshouFazhang:
		case ZhuangbeiType_Gong:
			leixingStr = LFStrings::getValue("gongJi");
			baizi = (equip->getEquipData().gongji_min + equip->getEquipData().gongji_max) / 2.0;
			break;
		case ZhuangbeiType_Dunpai:
			//case ZhuangbeiType_Toukui:
		case ZhuangbeiType_KuijiaZhong:
		case ZhuangbeiType_KuijiaQin:
			//case ZhuangbeiType_KuziZhong:
			//case ZhuangbeiType_KuziQin:
			//case ZhuangbeiType_Xuezi:
			leixingStr = LFStrings::getValue("huJia");
			baizi = equip->getEquipData().hujia;
			break;
		case ZhuangbeiType_Jiezhi:
		case ZhuangbeiType_Hushenfu:
		case ZhuangbeiType_Faqi:
		default:
			CCLog("Error: GongJiangQianghua::setSelectedEquipmentId ---> error type");
			break;
		}
		char buf[20];
		sprintf(buf, "%.1f", baizi);
		string tempBuf = leixingStr;
		CCLabelTTF *leixingLabel = CCLabelTTF::create((tempBuf+""+buf).c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(leixingLabel);
		leixingLabel->setAnchorPoint(ccp(0, 0.5));
		leixingLabel->setPosition(ccp(82, 838-436));
		//
		int shuxingAddPercent = 0;
		for(int i=0; i!=qianghuaData.qianghua_count; ++i)
		{
			shuxingAddPercent += qianghuaData.info_arr[i].percent;
		}
		sprintf(buf, "(+%d%%)", shuxingAddPercent);
		CCLabelTTF *jiachengLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		mBg->addChild(jiachengLabel, 1, Tag_Label_JiaCheng_Percent);
		jiachengLabel->setColor(ccc3(0, 255, 0));
		jiachengLabel->setAnchorPoint(ccp(0, 0.5));
		jiachengLabel->setPosition(ccp(leixingLabel->getPositionX()+leixingLabel->getContentSize().width+6, 838-436));

	}
	
	//3.历史强化记录
	//如果是回退则要排序
	//给 QianghuaData排序  由好到差
	if(mCurStatus == gongjiang_huitui)
	{
// 		for(int i=0; i!=qianghuaData.qianghua_count; ++i)
// 		{
// 			for(int j=i; j!=qianghuaData.qianghua_count; ++j)
// 			{
// 				if(  (qianghuaData.info_arr[i].level < qianghuaData.info_arr[j].level)   
// 					|| (qianghuaData.info_arr[i].level == qianghuaData.info_arr[j].level && 
// 					qianghuaData.info_arr[i].percent < qianghuaData.info_arr[j].percent)  )
// 				{
// 					swap(qianghuaData.info_arr[i], qianghuaData.info_arr[j]);
// 				}
// 			}
// 		}
		sortQianghuaDataAtHuiTui(qianghuaData);
		for (int ix=0; ix<9; ++ix) mQianghuaLevel[ix] = 0;
		for (int ix=0; ix<qianghuaData.qianghua_count; ++ix) mQianghuaLevel[ix]=qianghuaData.info_arr[ix].level;
	}

	for(int i=0; i!=qianghuaData.qianghua_count; ++i)
	{
		QianghuaInfo info = qianghuaData.info_arr[i];
		//
		BaseSprite *infoBoard = BaseSprite::create("gongjiang_qianghua_jieguo_bg.png");
		mBg->addChild(infoBoard, 1, Tag_Img_QiangHuaLiShiJiLu + i);
		infoBoard->setPosition(ccp(476, 838-166-i*66));

		//星星
		BaseSprite *xingxing;
		switch (info.level)
		{
		case 1:
			xingxing = BaseSprite::create("gongjiang_qianghua_jieguo_tie.png");
			xingxing->setPosition(ccp(40, 69-31));
			break;
		case 2:
			xingxing = BaseSprite::create("gongjiang_qianghua_jieguo_tong.png");
			xingxing->setPosition(ccp(40, 69-31));
			break;
		case 3:
			xingxing = BaseSprite::create("gongjiang_qianghua_jieguo_yin.png");
			xingxing->setPosition(ccp(40, 69-31));
				if(i==qianghuaData.qianghua_count-1&&flag==selectTo_QiangHua&&ishuidian==2){
					   Qianhuadh(xingxing,3);
					   ishuidian=0;
		         }
			break;
		case 4:
		case QianghuaLevel_jin_special:
			xingxing = BaseSprite::create("gongjiang_qianghua_jieguo_jin.png");
			xingxing->setPosition(ccp(40, 69-31));
			if(i==qianghuaData.qianghua_count-1&&flag==selectTo_QiangHua&&ishuidian==2){
				   Qianhuadh(xingxing,4);
				   ishuidian=0;
		    }
			break;
		default:
			break;
		}
		infoBoard->addChild(xingxing, 10, Tag_XingXing);

		

		//加成属性
		string temp = leixingStr;
		char buf[20];
		sprintf(buf, "+%d%%", info.percent);
		temp += buf;
		CCLabelTTF *leixingJiaCheng = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LABEL_SIZE);
		infoBoard->addChild(leixingJiaCheng, 0, Tag_Label_Prop);
		leixingJiaCheng->setColor(ccc3(195, 145, 96));
		leixingJiaCheng->setAnchorPoint(ccp(0, 0.5));
		leixingJiaCheng->setPosition(ccp(86, 69-31));
		
		
	}

	//4.强化所需
	if(mCurStatus == gongjiang_qianghua)
	{
		qianghua_SuoXu(equip, mCurStatus);
	}
	else 
	{
		for(int i=0; i!=9; ++i)
			mHuiTui_BaoLiu_Select[i] = false;
		mHuiTui_SelectCount = 0;
		//除特殊金星外 前4个 金星显示为锁、不可回退；银星默认勾选；铜铁默认不勾选
		for (int i=0; i!=qianghuaData.qianghua_count; ++i)
		{
			if (i < 4)
			{
				if (qianghuaData.info_arr[i].level >= 3 && qianghuaData.info_arr[i].level != QianghuaLevel_jin_special)
				{
					mHuiTui_BaoLiu_Select[i] = true;
					++mHuiTui_SelectCount;
				}
			}
			if (mHuiTui_SelectCount >= 8) break;
		}

		refreshHuiTuiItem(equip, mCurStatus);
		huitui_SuoXu(equip, mCurStatus);
	}

	/*CCMenuItemImage *querenItem = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_QueRen);*/

	if (mCurStatus == gongjiang_qianghua)
	{
		m_iQianghuaItem->setVisible(true);
		m_iQianghuaItem->setEnabled(true);
	}
	else if (mCurStatus == gongjiang_huitui)
	{
		m_iHuituiItem->setVisible(true);
		m_iHuituiItem->setEnabled(true);
	}
	
	
	CCMenuItemImage *fanhui = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_FanHui);
	//querenItem->setVisible(true);
	fanhui->setPositionX(mBg->getContentSize().width/3);



	//第一次强化教学
	if (mCurStatus == gongjiang_qianghua)
	{
		if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GongJiang_QiangHua))
		{
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_QiangHua);
			g_gongjiang_QhHt_Opend = false;   //强化教学完毕。clear

			CCSize size(304,72); 
			CCPoint pos(476, 838-169);
			size.height = 72*qianghuaData.qianghua_count;
			pos.y = 838 - 167 + 72/2 - size.height/2;

			m_rHuituiSuoxuRect = CCRectMake(156,230,312,224);

			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCMenuItem* menuItem = (CCMenuItem*)m_iQianghuaItem;
			guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
				, LFStrings::getValue(""), NULL, false);

			GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;
			guideLayer2->init(m_rHuituiSuoxuRect.size, mBg->convertToWorldSpace(m_rHuituiSuoxuRect.origin), LFStrings::getValue("QianghuaSuoxuTeach"), guideLayer, true);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer2, 1024);
			return;
		}
	}
	//第一次回退教学
	if(mCurStatus == gongjiang_huitui && mUI_Guide_FenJieOrHuiTui == 20)
	{
		if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GongJiang_HuiTui))
		{
			//GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_HuiTui);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_HuiTui);
			mUI_Guide_FenJieOrHuiTui = 0;

			CCSize size(304,72); 
			CCPoint pos(476, 838-169);
			size.height = 72*qianghuaData.qianghua_count;
			pos.y = 838 - 167 + 72/2 - size.height/2;

			m_rHuituiSuoxuRect = CCRectMake(156,230,312,224);

			GuideLayerBase *guideLayer3 = new GuideLayer_SimpleLayer;
			CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_FanHui);
			guideLayer3->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
				, LFStrings::getValue("gongjiang_fenjieTishi"), NULL, false);

			//CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);

			GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;
			guideLayer2->init(m_rHuituiSuoxuRect.size, mBg->convertToWorldSpace(m_rHuituiSuoxuRect.origin), LFStrings::getValue("huituiSuoxuShuoming"), guideLayer3, true);

			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(size, mBg->convertToWorldSpace(pos), LFStrings::getValue("gongjiang_huituiShuoming"), guideLayer2, true);
			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
	}
}

void GongJiangQianghua::qianghua_SuoXu(Equipment *equip, int status)
{
	if(status != gongjiang_qianghua)
		return;

	mRongShiNum = 0; // init

	unsigned int cailiao_normal_need = 0;
// 	unsigned int cailiao_xiyou_need = 0;
// 	unsigned int cailiao_chuanqi_need = 0;
	cailiao_normal_need = CS::ItemsNeedForQianghuaZhuangbei(equip->getLevel(), equip->getEquipColor());
	unsigned int cailiao_normal_total = 0;
// 	unsigned int cailiao_xiyou_total = 0;
// 	unsigned int cailiao_chuanqi_total = 0;
	{
		WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_QianghuaNormal);
		if(wp != NULL)
		{
			cailiao_normal_total = wp->getCount();
		}
// 		wp = WuPinManager::getManager()->getWuPin(ItemsType_QianghuaXiyou);
// 		if(wp != NULL)
// 		{
// 			cailiao_xiyou_total = wp->getCount();
// 		}
// 		wp = WuPinManager::getManager()->getWuPin(ItemsType_QianghuaChuangqi);
// 		if(wp != NULL)
// 		{
// 			cailiao_chuanqi_total = wp->getCount();
// 		}
	}

// 	int num_cailiao_leixing = 0;
// 	if(cailiao_normal_need > 0)		++num_cailiao_leixing;
// 	if(cailiao_xiyou_need > 0)		++num_cailiao_leixing;
// 	if(cailiao_chuanqi_need > 0)	++num_cailiao_leixing;

	//材料是一个按钮、点击后告诉玩家哪里可以得到。
	CCMenu *cailiaoMenu = CCMenu::create();
	mBg->addChild(cailiaoMenu, 1, Tag_Menu_CaiLiao);
	cailiaoMenu->setPosition(CCPointZero);
	//65,528  163,528  261,528      x_98     w,h(93,128)
	if (cailiao_normal_need > 0)
	{
		//board
		BaseSprite *board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
		mBg->addChild(board);
		board->setPosition(ccp(165, 838-606));
		//name
		CCLabelTTF *name = CCLabelTTF::create(LFStrings::getValue("QianghuaShuoming").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(name);
		name->setColor(ccc3(40,40,40));
		name->setPosition(ccp(board->getPositionX(), 838-527));
		//image
		string naStr = ImageManager::getManager()->getItemFilename(ItemsType_QianghuaNormal);
		CCMenuItemImage *cailiaoItem = CCMenuItemImage::create(naStr.c_str(), naStr.c_str(), 
			this, menu_selector(GongJiangQianghua::menuItemClicked_CailiaoClicked));
		cailiaoMenu->addChild(cailiaoItem, 1, ItemsType_QianghuaNormal);
		cailiaoItem->setPosition(mBg->convertToNodeSpace(board->convertToWorldSpace(ccp(46, 128-46))) );
		//label need
		char buf[20];
		sprintf(buf, "%d/", cailiao_normal_need);
		CCLabelTTF *needLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		board->addChild(needLabel);
		needLabel->setAnchorPoint(ccp(1, 0.5));
		needLabel->setPosition(ccp(47, 15));
		//all cailiao
		sprintf(buf, "%d", cailiao_normal_total);
		CCLabelTTF *allLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
		board->addChild(allLabel);
		allLabel->setAnchorPoint(ccp(0, 0.5));
		allLabel->setPosition(needLabel->getPosition());
		if(cailiao_normal_total < cailiao_normal_need) 
		{
			allLabel->setColor(ccc3(255, 0, 0));
			mErrorInfomation = LFStrings::getValue("qianghuasuoxucailiaobuzu");
		}
	}
	//所需银币
	BaseSprite *yinbiImg = BaseSprite::create("gongjiang_qianghua_yinbi_bg.png");
	mBg->addChild(yinbiImg);
	yinbiImg->setPosition(ccp(163, 838-695));
	//yinbi icon
	BaseSprite *yinbiIcon = BaseSprite::create("yinbi_tubiao.png");
	yinbiImg->addChild(yinbiIcon);
	yinbiIcon->setScale(0.8f);
	yinbiIcon->setPosition(ccp(yinbiImg->getContentSize().width/3, yinbiImg->getContentSize().height/2));
	//need yinbi
	unsigned int silver_need = CS::QianghuaZhuangbeiNeedSilver(equip->getLevel(), equip->getEquipColor());
	unsigned int silver_total = 0;
	char buf[20];
	sprintf(buf, " %d/", silver_need);
	CCLabelTTF *yinbiLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
	yinbiImg->addChild(yinbiLabel);
	yinbiLabel->setAnchorPoint(ccp(0, 0.5));
	yinbiLabel->setPosition(ccp(yinbiIcon->getPositionX()+10, yinbiIcon->getPositionY()));
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
	yinbiImg->addChild(yinbiAllLabel);
	yinbiAllLabel->setAnchorPoint(ccp(0, 0.5));
	yinbiAllLabel->setPosition(ccp(yinbiLabel->getPositionX()+yinbiLabel->getContentSize().width, yinbiIcon->getPositionY()));
	if(silver_total < silver_need)
	{
		yinbiAllLabel->setColor(ccRED);
		mErrorInfomation = LFStrings::getValue("suoxuyinbibuzu");
	}

	//
	if(equip->getEquipData().qianghua_data.qianghua_count >= equip->getEquipData().qianghua_data.max_qianghua_time)
		mErrorInfomation = LFStrings::getValue("yisizuidaqianghuacishu");
}

void GongJiangQianghua::huitui_SuoXu(Equipment *equip, int status)
{
	if(status != gongjiang_huitui)
		return;
	unsigned int cailiao_normal_need = 0;
	// 	unsigned int cailiao_xiyou_need = 0;
	// 	unsigned int cailiao_chuanqi_need = 0;

		//name
	CCLabelTTF *name = CCLabelTTF::create(LFStrings::getValue("huiTuiInfo").c_str(), fontStr_kaiti, LABEL_SIZE - 2.0f,CCSizeMake(310,400),kCCTextAlignmentLeft);
	mBg->addChild(name);
	name->setColor(ccc3(40,40,40));
	name->setPosition(ccp(170, 838 - 527 - 210));
		
	
	//所需银币
	BaseSprite *yinbiImg = BaseSprite::create("gongjiang_qianghua_yinbi_bg.png");
	mBg->addChild(yinbiImg);
	yinbiImg->setPosition(ccp(163, 838-695));
	//yinbi icon
	BaseSprite *yinbiIcon = BaseSprite::create("jinbi_tubiao.png");
	yinbiImg->addChild(yinbiIcon);
	yinbiIcon->setScale(0.8f);
	yinbiIcon->setPosition(ccp(yinbiImg->getContentSize().width/3, yinbiImg->getContentSize().height/2));
	//need yinbi
	unsigned int silver_need = CS::HuituiZhuangbeiNeedGold(mHuiTui_SelectCount);
	unsigned int silver_total = 0;
	char buf[20];
	sprintf(buf, " %d/", silver_need);
	CCLabelTTF *yinbiLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
	yinbiImg->addChild(yinbiLabel);
	yinbiLabel->setAnchorPoint(ccp(0, 0.5));
	yinbiLabel->setPosition(ccp(yinbiIcon->getPositionX()+10, yinbiIcon->getPositionY()));

	m_tSuoxuGlobeTTF = yinbiLabel;
	// all yinbi
	{
		WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		if (wp != NULL)
		{
			silver_total = wp->getCount();
		}
	}
	m_sglobbiIcon = yinbiIcon;
	sprintf(buf, "%d", silver_total);
	CCLabelTTF *yinbiAllLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
	yinbiImg->addChild(yinbiAllLabel);
	yinbiAllLabel->setAnchorPoint(ccp(0, 0.5));
	yinbiAllLabel->setPosition(ccp(yinbiLabel->getPositionX()+yinbiLabel->getContentSize().width, yinbiIcon->getPositionY()));
	if(silver_total < silver_need)
	{
		yinbiAllLabel->setColor(ccRED);
		mErrorInfomation = LFStrings::getValue("suoxujinbibuzu");
	}
	m_tSuoyouGlobeTTF = yinbiAllLabel;

	
}
void GongJiangQianghua::refreshHuitui_SuoXu(Equipment *equip, int status)
{
	if(status != gongjiang_huitui)
		return;

	//need yinbi
	unsigned int silver_need = CS::HuituiZhuangbeiNeedGold(mHuiTui_SelectCount);
	unsigned int silver_total = 0;
	char buf[20];
	sprintf(buf, " %d/", silver_need);
	//CCLabelTTF *yinbiLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
	m_tSuoxuGlobeTTF->setString(buf);
	// all yinbi
	{
		WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		if (wp != NULL)
		{
			silver_total = wp->getCount();
		}
	}
	sprintf(buf, "%d", silver_total);
	//CCLabelTTF *yinbiAllLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
	m_tSuoyouGlobeTTF->setString(buf);
	m_tSuoyouGlobeTTF->setPosition(ccp(m_tSuoxuGlobeTTF->getPositionX()+m_tSuoxuGlobeTTF->getContentSize().width, m_sglobbiIcon->getPositionY()));
	if(silver_total < silver_need)
	{
		m_tSuoyouGlobeTTF->setColor(ccRED);
		mErrorInfomation = LFStrings::getValue("suoxujinbibuzu");
	}
}
void GongJiangQianghua::refreshHuiTuiItem(Equipment *equip, int status)
{
	if(status != gongjiang_huitui)
		return;

	QianghuaData qianghuaData = equip->getEquipData().qianghua_data;


// 	//如果是回退则要排序
// 	//给 QianghuaData排序  由好到差
// 	for(int i=0; i!=qianghuaData.qianghua_count; ++i)
// 	{
// 		for(int j=i; j!=qianghuaData.qianghua_count; ++j)
// 		{
// 			if(  (qianghuaData.info_arr[i].level < qianghuaData.info_arr[j].level)   
// 				|| (qianghuaData.info_arr[i].level == qianghuaData.info_arr[j].level && 
// 				qianghuaData.info_arr[i].percent < qianghuaData.info_arr[j].percent)  )
// 			{
// 				swap(qianghuaData.info_arr[i], qianghuaData.info_arr[j]);
// 			}
// 		}
// 	}
	sortQianghuaDataAtHuiTui(qianghuaData);
	for(int i=0; i!=qianghuaData.qianghua_count; ++i)
	{
		//mBg->addChild(infoBoard, 1, Tag_Img_QiangHuaLiShiJiLu + i);   setEquipData中已加入
		BaseSprite *infoBoard = (BaseSprite *)mBg->getChildByTag(Tag_Img_QiangHuaLiShiJiLu + i);

		CCMenu *menu = (CCMenu*)infoBoard->getChildByTag(Tag_Menu_QiangHuaLiShiJiLu);
		if(menu == NULL)
		{
			menu = CCMenu::create();
			infoBoard->addChild(menu, 1, Tag_Menu_QiangHuaLiShiJiLu);
			menu->setPosition(CCPointZero);
		}

		CCMenuItemImage *item = (CCMenuItemImage*)menu->getChildByTag(i);
		if(mHuiTui_BaoLiu_Select[i])
		{
			if(item == NULL)
			{
				item = CCMenuItemImage::create(
				"gongjiang_qianghua_huitui_select.png"
				, "gongjiang_qianghua_huitui_dengdai.png"
				, "duiwu_yingxiong_jiesuo.png"                       
				, this, menu_selector(GongJiangQianghua::menuClicked_HuiTuiBaoLiu_Select));

				menu->addChild(item, 1, i);
			}
			else 
			{
				item->initWithNormalImage(
				"gongjiang_qianghua_huitui_select.png"
				, "gongjiang_qianghua_huitui_dengdai.png"
				, "duiwu_yingxiong_jiesuo.png"
				, this, menu_selector(GongJiangQianghua::menuClicked_HuiTuiBaoLiu_Select));
			}
			//金默认不可回退  熔金石段出来的 直接锁定。
			if (qianghuaData.info_arr[i].level == QianghuaLevel_jin_special)
				item->initWithNormalImage(
				"gongjiang_qianghua_huitui_select.png"
				, "gongjiang_qianghua_huitui_dengdai.png"
				, "duiwu_yingxiong_suoding.png"
				, this, menu_selector(GongJiangQianghua::menuClicked_HuiTuiBaoLiu_Select));
			if (qianghuaData.info_arr[i].level >= QianghuaLevel_jin) item->setEnabled(false);
		}			
		else 
		{
			if(item ==NULL)
			{
				item = CCMenuItemImage::create(
				"gongjiang_qianghua_huitui_dengdai.png"
				, "gongjiang_qianghua_huitui_select.png"
				, "duiwu_yingxiong_weixuanzhong.png"
				, this, menu_selector(GongJiangQianghua::menuClicked_HuiTuiBaoLiu_Select));

				menu->addChild(item, 1, i);
			}
			else
			{
				item->initWithNormalImage(
				"gongjiang_qianghua_huitui_dengdai.png"
				, "gongjiang_qianghua_huitui_select.png"
				, "duiwu_yingxiong_weixuanzhong.png"
				, this, menu_selector(GongJiangQianghua::menuClicked_HuiTuiBaoLiu_Select));
			}
			//金默认不可回退  熔金石段出来的 直接锁定。
			if (qianghuaData.info_arr[i].level == QianghuaLevel_jin_special)
			{
				item->initWithNormalImage(
				"gongjiang_qianghua_huitui_dengdai.png"
				, "gongjiang_qianghua_huitui_select.png"
				, "duiwu_yingxiong_suoding.png"
				, this, menu_selector(GongJiangQianghua::menuClicked_HuiTuiBaoLiu_Select));
				item->setEnabled(false);
			}

			if(mHuiTui_SelectCount >= 8) item->setEnabled(false);
		}

		item->setPosition(ccp(268, infoBoard->getContentSize().height/2));
	}
}
void GongJiangQianghua::huitui_SuoxuGlob()
{
	refreshHuitui_SuoXu(mCurEquip,mCurStatus);
}
//pSender 的tag为对应mHuiTui_BaoLiu_Select的下标
void GongJiangQianghua::menuClicked_HuiTuiBaoLiu_Select(CCObject *pSender)
{
	int idx = ((CCMenuItemImage*)pSender)->getTag();
	if(mHuiTui_BaoLiu_Select[idx])
	{
		mHuiTui_BaoLiu_Select[idx] = false;
		--mHuiTui_SelectCount;
	}
	else
	{
		//最多保留4个回退
		if(mHuiTui_SelectCount >= 8)
			return;
		mHuiTui_BaoLiu_Select[idx] = true;
		++mHuiTui_SelectCount;
	}

	huitui_SuoxuGlob();
	refreshHuiTuiItem(mCurEquip, mCurStatus);
}

void GongJiangQianghua::menuItemClicked_CailiaoClicked(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	string tishiStr, cailiaoFileStr;

	if(tag == ItemsType_QianghuaChuangqi)
	{
		tishiStr = LFStrings::getValue("chuanqiduanzaocailiaochuchutishi");
		cailiaoFileStr = ImageManager::getManager()->getItemFilename(ItemsType_QianghuaChuangqi);
	}
	else if (tag == ItemsType_QianghuaXiyou)
	{
		tishiStr = LFStrings::getValue("xiyouduanzaocailiaochuchutishi");
		cailiaoFileStr = ImageManager::getManager()->getItemFilename(ItemsType_QianghuaXiyou);
	}
	else if (tag == ItemsType_QianghuaNormal)
	{
		tishiStr = LFStrings::getValue("putongduanzaocailiaochuchutishi");
		cailiaoFileStr = ImageManager::getManager()->getItemFilename(ItemsType_QianghuaNormal);
	}
	else if (tag == ItemsType_QianghuaChuangqi+Tag_CautionDialog)
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_CautionDialog);
		return;
	}



	//弹框背景
	BaseSprite* dialog = BaseSprite::create("gongjiang_duanzao_cailiaobuzu_tanchukuang.png");
	dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 + 50));
	dialog->setTag(Tag_CautionDialog);
	//提示
	{
		BaseSprite *cailiaoBg = BaseSprite::create("daoju_kuang_bg.png");
		dialog->addChild(cailiaoBg);
		cailiaoBg->setPosition(ccp(cailiaoBg->getContentSize().width/2 + 20, dialog->getContentSize().height/2));
		BaseSprite *cailiaoImg = BaseSprite::create(cailiaoFileStr);
		cailiaoBg->addChild(cailiaoImg);
		cailiaoImg->setPosition(ccp(cailiaoBg->getContentSize().width/2, cailiaoBg->getContentSize().height/2));

		CCLabelTTF* lable = CCLabelTTF::create(tishiStr.c_str(), fontStr_kaiti,20,CCSize(220, 0), kCCTextAlignmentLeft);
		dialog->addChild(lable);
		lable->setAnchorPoint(ccp(0,0.5));
		lable->setPosition(ccp(cailiaoBg->getPositionX()+cailiaoBg->getContentSize().width/2+10, dialog->getContentSize().height/2));
	}
	//按键
	CCMenu* menu = CCMenu::create();
	{
		CCMenuItemImage* item = CCMenuItemImage::create(
			"gongjiang_duanzao_cailiaobuzu_tanchukuang.png",
			"gongjiang_duanzao_cailiaobuzu_tanchukuang.png",
			this, menu_selector(GongJiangQianghua::menuItemClicked_CailiaoClicked));
		menu->addChild(item, 0, ItemsType_QianghuaChuangqi+Tag_CautionDialog);
		item->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
		item->setOpacity(0);
		item->setScaleX(mWinSize.width / dialog->getContentSize().width);
		item->setScaleY(mWinSize.height / dialog->getContentSize().height);
// 		item->setScaleX(mWinSize.width / item->getContentSize().width);
// 		item->setScaleY(mWinSize.height / item->getContentSize().height);
	}
	//弹框
	{

		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_CautionDialog);
		lfAlert->release();
	}
}

void GongJiangQianghua::onEnter()
{
	BaseLayer::onEnter();

	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GongJiang_QiangHua))
	{
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_TianJiaZhuangBei);
		guideLayer->init(CCSize(menuItem->getContentSize().width, menuItem->getContentSize().height)
			, menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue("gongjiang_qianghua_xuanze"), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}
	/*else if (g_FirstChuanQiEquipLook)
	{
		CCMenuItem *fanhui = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_FanHui);
		if (fanhui)
		{
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(fanhui->getContentSize()
				, fanhui->getParent()->convertToWorldSpace(fanhui->getPosition())
				, "", NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
	}*/
}

void GongJiangQianghua::fxToast_callback()
{
	messageDialogEndCallback();
}

//开启教学才会触发该函数
void GongJiangQianghua::messageDialogEndCallback()
{
	//分解
	if (g_gongjiang_FenJie_Guide)
	{
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(Tag_MenuItem_FanHui);
		guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue("gongjiang_fenjieTishi"), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		mUI_Guide_FenJieOrHuiTui = 0;
		g_gongjiang_FenJie_Guide = true;
	}
	//回退
	else if (mUI_Guide_FenJieOrHuiTui == 20)
	{

		
		
	}
}


void GongJiangQianghua::Qianhuadh(BaseSprite *bs,int level){



	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("gongjiang_xiaoguo.ExportJson");
	CCArmature *armature = CCArmature::create("gongjiang_xiaoguo");
	ar=armature;
	
	armature->setAnchorPoint(ccp(0.5,0.5));
	string name="qianghua_yinxing";
	if(level==4){
		name="qianghua_jinxing";
		armature->getAnimation()->setFrameEventCallFunc(this,SEL_FrameEventCallFunc(&GongJiangQianghua::onFrameEvent));  

	}
	armature->getAnimation()->play(name.c_str());
	armature->getAnimation()->setMovementEventCallFunc(this, 
		  SEL_MovementEventCallFunc(&GongJiangQianghua::callfunc_AnimateEnd));
	armature->setPosition(ccp(bs->getPositionX()-15,bs->getPositionY()-10));
	bs->addChild(armature);
	//layer->getParent()->addChild(armature,layer->getZOrder()+10);

}

void GongJiangQianghua::callfunc_AnimateEnd(CCArmature *ar2, MovementEventType, const char *){

  // if (type == COMPLETE || type == LOOP_COMPLETE) {
		ar2->removeFromParent();
		isAnimation=1;
		//析构函数中释放资源
	//}
	CCLOG("over--->");
}

void  GongJiangQianghua::onFrameEvent(CCBone *bone, const char *evt, int originFrameIndex, int currentFrameIndex){


	if(strcmp(evt,"FrameEvent_xianshibeihou") == 0)  
    {  

		 CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("gongjiang_xiaoguo.ExportJson");
	     CCArmature *armature = CCArmature::create("gongjiang_xiaoguo");
	     ar1=armature;

		 //armature->setAnchorPoint(currentzb->getAnchorPoint());
		 armature->setScale(2);
	     armature->getAnimation()->play("qianghua_jinxing_2");
	     armature->getAnimation()->setMovementEventCallFunc(this, 
		 SEL_MovementEventCallFunc(&GongJiangQianghua::callfunc_AnimateEnd));
	     armature->setPosition(ccp(currentzb->getPositionX()+290,currentzb->getPositionY()-420));
	     currentzb->addChild(armature);
         
    }  

	
}

GongJiangQianghua::~GongJiangQianghua(void){

	if(isAnimation==1)
	  CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("gongjiang_xiaoguo.ExportJson");

}