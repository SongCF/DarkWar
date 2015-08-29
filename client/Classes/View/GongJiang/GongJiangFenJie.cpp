#include "GongJiangFenJie.h"
#include "GongJiangFenJie_EquipSelect.h"
#include "LFStrings.h"
#include "font.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "LFAlert.h"
#include "CS_Common.h"
#include "GuideLayerBase.h"
#include "FXToast.h"
#include "UnblockManager.h"
extern bool g_gongjiang_FenJie_Guide;

#define TAG_LFSLERT_DIALOG 1000

#define LABEL_SIZE 20
#define LABEL_COLOR ccc3(127,127,127)

enum Tag
{
	Tag_MenuItem_TianJiaZhuangBei = 1,
	Tag_MenuItem_Auto_Bai,
	Tag_MenuItem_Auto_Lan,
	Tag_MenuItem_QueRen,
	Tag_MenuItem_FanHui,

	Tag_Img_EquipAndDiWen,
	Tag_Label_TiShiZhuangBeiName,

	Tag_Board_ZhuTieFen,
	Tag_Board_DuanJinFen,
	Tag_Board_EMoGu,
	Tag_Label_Num,
	Tag_Label_Baoshi,
	Tag_Board_XiLianJingHua,
	Tag_Board_CaiLiaoFanHui,

	Tag_Label_Name_CaiLiaoFanHui,
	Tag_Label_Name_ZhuTieFen,
	Tag_Label_Name_XiLianJingHua,

	Tag_OK,
	Tag_Cancel,
};

GongJiangFenJie* GongJiangFenJie::create(Equipment *equip)
{
	GongJiangFenJie* layer = new GongJiangFenJie;
	if(layer && layer->init(equip))
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

bool GongJiangFenJie::init(Equipment *equip)
{
	m_bIsHadXiLianJingHua = false;
	if(equip != NULL)
	{
		mComeFromEquipDetail = true;
		mComFromGJ_equip = equip;
	}
	else
	{
		mComeFromEquipDetail = false;
		mComFromGJ_equip = NULL;
	}

	mBg = BaseSprite::create("gongjiang_fenjie_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

	mMenu = CCMenu::create();
	mBg->addChild(mMenu);
	mMenu->setPosition(CCPointZero);

	// 
	CCMenuItemImage *tianjia = CCMenuItemImage::create(
		"gongjiang_duanzao_xuanze_zhuangbei.png"
		, "gongjiang_duanzao_xuanze_zhuangbei_select.png"
		, this, menu_selector(GongJiangFenJie::menuItemClicked));
	mMenu->addChild(tianjia, 1, Tag_MenuItem_TianJiaZhuangBei);
	tianjia->setPosition(ccp(320, 838-286));

	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(GongJiangFenJie::menuItemClicked));
	mMenu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
	fanhui->setPosition(ccp(mBg->getContentSize().width/2, 838-784));
	//
	CCMenuItemImage *queren = CCMenuItemImage::create(
		"zhuangbei_xiangxi_queding.png"
		, "zhuangbei_xiangxi_queding_select.png"
		, this, menu_selector(GongJiangFenJie::menuItemClicked));
	mMenu->addChild(queren, 1, Tag_MenuItem_QueRen);
	queren->setPosition(ccp(mBg->getContentSize().width/3*2, 838-784));
	queren->setVisible(false);


	//
	CCMenuItemImage *autoBai = CCMenuItemImage::create(
		"gongjiang_fenjie_anniu_zidongfenjie_putong.png",
		"gongjiang_fenjie_anniu_zidongfenjie_putong_select.png",
		this, menu_selector(GongJiangFenJie::menuItemClicked_Auto));
	mMenu->addChild(autoBai, 1, Tag_MenuItem_Auto_Bai);
	autoBai->setPosition(ccp(120, 838-380-56));
	CCMenuItemImage *autoLan = CCMenuItemImage::create(
		"gongjiang_fenjie_anniu_zidongfenjie_fumo.png",
		"gongjiang_fenjie_anniu_zidongfenjie_fumo_select.png",
		this, menu_selector(GongJiangFenJie::menuItemClicked_Auto));
	mMenu->addChild(autoLan, 1, Tag_MenuItem_Auto_Lan);
	autoLan->setPosition(ccp(520, 838-380-56));


	clearnBackGround(1);

	if(mComeFromEquipDetail)
	{
		mComeFromEquipDetail = false;
		map<unsigned long long,Equipment*> ids;
		ids.insert(pair<unsigned long long,Equipment*>(mComFromGJ_equip->getEquipId(),mComFromGJ_equip));
		setSelectedEquipmentId(ids);
	}
	
	return true;
}

void GongJiangFenJie::menuItemClicked(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if(tag == Tag_MenuItem_FanHui)
	{
		if(mComFromGJ_equip != NULL)
			GameDirector::getDirector()->mainScene(mainlayer_ZhuangBei);
		else if (! mFooterLayer->getTopLayer())
			GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
		else
			mFooterLayer->dismmisPopUp();
	}
	else if(tag == Tag_MenuItem_TianJiaZhuangBei)
	{
		GongJiangFenJie_EquipSelect *layer = GongJiangFenJie_EquipSelect::create(this);
		mFooterLayer->popUpContaintLayer(layer);
	}
	else if(tag == Tag_MenuItem_QueRen)
	{
		{
			//弹框提示
			//弹框背景
			BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
			dialog->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
			dialog->setTag(TAG_LFSLERT_DIALOG);
			//label
			CCString temp_;
			temp_.initWithFormat(LFStrings::getValue("querenzhuangbeifenjie").c_str(), mCurrentNeedFenjieEquipFromSelListVec.size());
			CCLabelTTF *label = CCLabelTTF::create(temp_.getCString(), 
				fontStr_kaiti, 30, CCSize(400, 0), kCCTextAlignmentLeft);
			dialog->addChild(label);
			label->setAnchorPoint(ccp(0.5, 1));
			label->setPosition(ccp(dialog->getContentSize().width/2, 278));
			label->setColor(fonColor_JingGao);

			//按键
			CCMenu* menu;
			{
				CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
					"shangpin_goumai_queren.png",
					"shangpin_goumai_queren_select.png",
					this,SEL_MenuHandler(&GongJiangFenJie::menuItemClicked_fenjie_OkCancel));


				CCMenuItemImage* menuItemCacel = CCMenuItemImage::create(
					"shangpin_goumai_quxiao.png",
					"shangpin_goumai_quxiao_select.png",
					this,SEL_MenuHandler(&GongJiangFenJie::menuItemClicked_fenjie_OkCancel));

				menu = CCMenu::create(menuItemMakeSure, menuItemCacel, NULL);
				menu->setPosition(CCPointZero);

				menuItemMakeSure->setTag(Tag_OK);
				menuItemCacel->setTag(Tag_Cancel);
				menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/3*2,70));
				menuItemCacel->setPosition(ccp(dialog->getContentSize().width/3, 70));
			}

			//弹框
			{
				LFAlert* lfAlert = new LFAlert();
				lfAlert->setTag(TAG_LFSLERT_DIALOG);
				lfAlert->setBaseContainer(dialog,menu);
				CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert, 128, TAG_LFSLERT_DIALOG);
				lfAlert->release();
			}
		}
		
	}
}

void GongJiangFenJie::menuItemClicked_Auto(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();
	clearnBackGround(1);

	ZhuangbeiColour zbcolor;
	if (tag == Tag_MenuItem_Auto_Bai)
	{
		zbcolor = ZhuangbeiColour_Baizhuang;
	}
	else if (tag == Tag_MenuItem_Auto_Lan)
	{
		zbcolor = ZhuangbeiColour_Fumo;
	}

	mCurrentNeedFenjieEquipFromSelListVec.clear();

	vector<Equipment*> allEpVec = MyselfManager::getManager()->getAllEquipments(Filter_All);
	for(int i=0; i<allEpVec.size(); ++i)
	{
		if(allEpVec[i]->getActorId() == INVALID_ID && allEpVec[i]->getEquipColor() == zbcolor)
		{
			mCurrentNeedFenjieEquipFromSelListVec.push_back(allEpVec[i]);
		}
	}

	if (mCurrentNeedFenjieEquipFromSelListVec.size() > 0)
		menuItemClicked(mMenu->getChildByTag(Tag_MenuItem_QueRen));
	else
	{
		FXToast *layer = FXToast::create(LFStrings::getValue("ZiDongFenJie_nohave"));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
}

void GongJiangFenJie::menuItemClicked_fenjie_OkCancel(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_LFSLERT_DIALOG);

	if(tag == Tag_OK)
	{
		vector<long long> equips;
		for (int index = 0,equipNum = mCurrentNeedFenjieEquipFromSelListVec.size();index < equipNum;index++)
		{
			equips.push_back(mCurrentNeedFenjieEquipFromSelListVec[index]->getEquipId());
		}

		if (equips.size() <= 0) return;

		addObserver(callfuncO_selector(GongJiangFenJie::rsp_zhuangbeiFenJie), MSG_ZBChaijieRsp);
		mCmdHlp->cmdZBChaijie(equips);	
	}
}

void GongJiangFenJie::rsp_zhuangbeiFenJie(CCObject *msg)
{
	removeObserver(MSG_ZBChaijieRsp);
	GameDirector::getDirector()->hideWaiting();

	Flat_ZBChaijieRsp *data = (Flat_ZBChaijieRsp *)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	// 删除装备 img  name
	if (mBg->getChildByTag(Tag_Img_EquipAndDiWen)) mBg->getChildByTag(Tag_Img_EquipAndDiWen)->removeFromParent();
	((CCLabelTTF*)mBg->getChildByTag(Tag_Label_TiShiZhuangBeiName))->setString(
		LFStrings::getValue("qiangxuanzezhuangbei").c_str());
	//隐藏 确认 按钮
	mMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(false);
	mMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mBg->getContentSize().width/2, 838-784));

	//所得材料
	//只有一种了  ---> 铸铁粉
	int cailiaoNum = 1;
// 	int cailiaoNum = 0;  // 1 2 3 种类
// 	for (int i=0; i!=data->count; ++i)
// 	{
// 		if (data->item_arr[i].type == ItemsType_QianghuaNormal)
// 		{
// 			cailiaoNum++;
// 			break;
// 		}
// 	}
// 	for (int i=0; i!=data->count; ++i)
// 	{
// 		if (data->item_arr[i].type == ItemsType_QianghuaXiyou)
// 		{
// 			cailiaoNum++;
// 			break;
// 		}
// 	}
// 	for (int i=0; i!=data->count; ++i)
// 	{
// 		if (data->item_arr[i].type == ItemsType_QianghuaChuangqi)
// 		{
// 			cailiaoNum++;
// 			break;
// 		}
// 	}
	bool bYiDe[3] = {false, false, false}; //已经得到了的, 方便后面设置没有得到的 label 为0
	int baoshiNum = 0;
	int size = data->item_arr.size();
	bool hadXilianJinghua = false;


	bool hadQiangHua = false;
	for (vector<ItemInfo>::iterator iter = data->item_arr.begin();iter != data->item_arr.end();iter++)
	{
		if (iter->type == ItemsType_Purify_Scroll)
		{
			hadXilianJinghua = true;
			break;
		}
	}

	if (data->re_item_arr.size() > 0)
	{
		hadQiangHua = true;
	}
	/*for (vector<>)
	{
	}*/
	int cailiao_Zhonglei = 1;  //白蓝装只有 铸铁粉、 黄装增加锻金粉、传奇装增加恶魔骨
	for (int i=0; i!= size; ++i)
	{
		ItemInfo oneItem = data->item_arr[i];
		CCLabelTTF *label = NULL, *name = NULL;
		BaseSprite *board;
		if (oneItem.type == ItemsType_QianghuaNormal) 
		{
			bYiDe[0] = true;
			//board
			board = (BaseSprite*)mBg->getChildByTag(Tag_Board_ZhuTieFen);
			if (! board)
			{
				board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
				mBg->addChild(board, 1, Tag_Board_ZhuTieFen);
				//image
				BaseSprite *cailiao = BaseSprite::create(
					ImageManager::getManager()->getItemFilename(ItemsType_QianghuaNormal));
				board->addChild(cailiao);
				cailiao->setPosition(ccp(46, 128-46));

				board->setPosition(ccp(320-(cailiao_Zhonglei-1)*60, 838-620));

				if (hadXilianJinghua && hadQiangHua)
				{
					board->setPositionX(320-(cailiao_Zhonglei + 1.5)*60);
				}
				else if (hadXilianJinghua || hadQiangHua)
				{
					board->setPositionX(320-(cailiao_Zhonglei + 0.5)*60);
				}
			}

			label = (CCLabelTTF*)board->getChildByTag(Tag_Label_Num);
			if (! label)
			{
				label = CCLabelTTF::create("???", fontStr_BookAntiqua, LABEL_SIZE);
				board->addChild(label, 1, Tag_Label_Num);
				label->setPosition(ccp(board->getContentSize().width/2, 15));
			}

			//name
			name = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Name_ZhuTieFen);
			if (! name)
			{
				name = CCLabelTTF::create(LFStrings::getValue("FenJieCaiLiao").c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(name, 0, Tag_Label_Name_ZhuTieFen);
				name->setColor(ccc3(40,40,40));
				name->setPosition(ccp(board->getPositionX(), board->getPositionY()+board->getContentSize().height/2+name->getContentSize().height));
			}
			/*name->setPosition(ccp(board->getPositionX(), 
				board->getPositionY()+board->getContentSize().height/2+name->getContentSize().height));*/
			label->setString(CCString::createWithFormat("%d",oneItem.count)->getCString());

		}
		else if (oneItem.type == ItemsType_Purify_Scroll)
		{
			board = (BaseSprite*)mBg->getChildByTag(Tag_Board_XiLianJingHua);
			if (! board)
			{
				board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
				mBg->addChild(board, 1, Tag_Board_XiLianJingHua);
				//image
				BaseSprite *cailiao = BaseSprite::create("daoju_youlanzhiye.png");
				board->addChild(cailiao);
				cailiao->setPosition(ccp(46, 128-46));
			}
			//board->setPosition(ccp(320-60*(cailiaoNum-2), 838-620));
			//num
			label = (CCLabelTTF*)board->getChildByTag(Tag_Label_Num);
			if (! label)
			{
				label = CCLabelTTF::create("???", fontStr_BookAntiqua, LABEL_SIZE);
				board->addChild(label, 1, Tag_Label_Num);
				label->setPosition(ccp(board->getContentSize().width/2, 15));
			}
			//name
			name = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Name_XiLianJingHua);
			if (! name)
			{
				name = CCLabelTTF::create(LFStrings::getValue("XilianJinghua").c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(name, 0, Tag_Label_Name_ZhuTieFen);
				name->setColor(ccc3(40,40,40));
			}
			/*name->setPosition(ccp(board->getPositionX(), 
				board->getPositionY()+board->getContentSize().height/2+name->getContentSize().height));*/
			label->setString(CCString::createWithFormat("%d",oneItem.count)->getCString());
		}
		else if (oneItem.type >= ItemsType_Baoshi_min && oneItem.type <= ItemsType_Baoshi_max)
		{
			UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("FenjieBaoshi").c_str(), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			/*
			BaseSprite *baoshiImg = BaseSprite::create(ImageManager::getManager()->getBaoshiFilename(oneItem.type));
			mBg->addChild(baoshiImg);
			CCNode *node = mBg->getChildByTag(Tag_Board_EMoGu);
			if (node == NULL) node = mBg->getChildByTag(Tag_Board_DuanJinFen);
			if (node == NULL) node = mBg->getChildByTag(Tag_Board_ZhuTieFen);
			baoshiImg->setPosition(ccp(node->getPositionX()+node->getContentSize().width+20*(baoshiNum++), node->getPositionY()));

			CCLabelTTF *label = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Baoshi);
			if (label == NULL)
			{
				label = CCLabelTTF::create(LFStrings::getValue("baoshi").c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(label, 1, Tag_Label_Baoshi);
				label->setPosition(ccp(baoshiImg->getPositionX(), baoshiImg->getPositionY() + baoshiImg->getContentSize().height));
				label->setColor(ccc3(40,40,40));
			}*/
		}
		else if (oneItem.type == ItemsType_GoldStar_Qianghua) // 强化金星的材料
		{
			/*
			BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_GoldStar_Qianghua));
			mBg->addChild(itemImg);
			itemImg->setPosition(ccp(320-100, 838-620));
			//
			char buf[10];
			sprintf(buf, "%d", oneItem.count);
			CCLabelTTF *numLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, LABEL_SIZE);
			itemImg->addChild(numLabel);
			numLabel->setPosition(ccp(itemImg->getContentSize().width/2, - numLabel->getContentSize().height/2));*/
		}
		else
		{
			CCLog("Error: fen jie error type [%d]", oneItem.type);
		}

	}


	size = data->re_item_arr.size();
	for (int i=0; i!= size; ++i)
	{
		ItemInfo oneItem = data->re_item_arr[i];
		CCLabelTTF *label = NULL, *name = NULL;
		BaseSprite *board;
		if (oneItem.type == ItemsType_QianghuaNormal) 
		{
			bYiDe[0] = true;
			//board
			board = (BaseSprite*)mBg->getChildByTag(Tag_Board_CaiLiaoFanHui);
			if (! board)
			{
				board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
				mBg->addChild(board, 1, Tag_Board_CaiLiaoFanHui);
				//image
				BaseSprite *cailiao = BaseSprite::create(
					ImageManager::getManager()->getItemFilename(ItemsType_QianghuaNormal));
				board->addChild(cailiao);
				cailiao->setPosition(ccp(46, 128-46));
				board->setPosition(ccp(320-(cailiao_Zhonglei-2)*60, 838-620));
				if (hadXilianJinghua)
				{
					board->setPosition(ccp(320-(cailiao_Zhonglei - 1)*60,838-620));
				}
			}
	
	
			label = (CCLabelTTF*)board->getChildByTag(Tag_Label_Num);
			if (! label)
			{
				label = CCLabelTTF::create("???", fontStr_BookAntiqua, LABEL_SIZE);
				board->addChild(label, 1, Tag_Label_Num);
				label->setPosition(ccp(board->getContentSize().width/2, 15));
			}

			
			//name
			name = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Name_CaiLiaoFanHui);
			if (! name)
			{
				name = CCLabelTTF::create(LFStrings::getValue("FanHuiCaiLiao").c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(name, 0, Tag_Board_CaiLiaoFanHui);
				name->setColor(ccc3(40,40,40));

				name->setPosition(ccp(board->getPositionX(), board->getPositionY()+board->getContentSize().height/2+name->getContentSize().height));
			}
			label->setString(CCString::createWithFormat("%d",oneItem.count)->getCString());

		}
	}

	//未得的删除
	for (int i=0; i!=3; ++i)
	{
		if(! bYiDe[i])
		{
			BaseSprite *img = (BaseSprite*)mBg->getChildByTag(Tag_Board_ZhuTieFen+i);
			if(img != NULL)
				img->removeFromParent();

			CCLabelTTF *label = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Name_ZhuTieFen+i);
			if(label != NULL)
				label->removeFromParent();
		}
	}

	mBg->initWithFile("gongjiang_fenjie_bg.png");
}

void GongJiangFenJie::setSelectedEquipmentId(map<unsigned long long,Equipment*> eqVec)
{
	clearnBackGround(MAX(eqVec.size(),1));
	mMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(true);
	mMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mBg->getContentSize().width/3, 838-784));

	mCurrentNeedFenjieEquipFromSelListVec.clear();

	map<unsigned long long,Equipment*>::iterator it = eqVec.begin();
	for (;it != eqVec.end();it++)
	{
		mCurrentNeedFenjieEquipFromSelListVec.push_back(it->second);
	}
	if (mCurrentNeedFenjieEquipFromSelListVec.empty())
	{
		CCAssert(false,"必须要有装备");
		return;
	}
	

	CCMenuItemImage *tianjiaIten = (CCMenuItemImage *)mMenu->getChildByTag(Tag_MenuItem_TianJiaZhuangBei);
	//1.排列
	{
		BaseSprite* equipsRootNode = BaseSprite::create();
		mBg->addChild(equipsRootNode, 100, Tag_Img_EquipAndDiWen);
		equipsRootNode->setPosition(tianjiaIten->getPosition());

		const float disGapX = 50.0f;
		const float maxDis = 300.0f;

		int equipNum = mCurrentNeedFenjieEquipFromSelListVec.size();
		if (equipNum == 1) 
		{
			const float disGapX = 50.0f;
			const float maxDis = 300.0f;

			int equipNum = mCurrentNeedFenjieEquipFromSelListVec.size();
			float posRight = -equipNum * disGapX / 2;

			posRight += disGapX / 2;

			for (int equipIndex = 0;equipIndex < equipNum;equipIndex++)
			{
				ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
					mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->getEquipColor(),
					false,//mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->isShenBing(),
					mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->isPartOfSuit(),
					mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->haveZhuanke());

				BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
				equipsRootNode->addChild(diwenImg, 1);
				float x_offset = posRight + equipIndex * disGapX;
				if (x_offset < -maxDis)
				{
					x_offset = -maxDis;
				}
				if (x_offset > maxDis)
				{
					x_offset = maxDis;
				}
				diwenImg->setPosition(ccpAdd(
					ccp(equipsRootNode->getContentSize().width/2,equipsRootNode->getContentSize().height/2),
					ccp(x_offset,0)));
				//zhuangbei
				BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurrentNeedFenjieEquipFromSelListVec[equipIndex]);
				diwenImg->addChild(eqImg);
				eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
				diwenImg->setScale(0.50);
			}
			string moreEquipName = equipNum > 1 ? 
				CCString::createWithFormat(LFStrings::getValue("fenjie_list").c_str(),equipNum)->getCString() :
				"";
			//name
			CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_TiShiZhuangBeiName);
			eqName->setString( (mCurrentNeedFenjieEquipFromSelListVec[0]->getName() + moreEquipName).c_str() ) ;
		}
		else if(equipNum >= 2 && equipNum <= 5)
		{
			const float eachWidth = 64.0f;
			const float eachGap = 25.0f;
			int equipNum = mCurrentNeedFenjieEquipFromSelListVec.size();

			float posx = -( eachWidth * equipNum + eachGap * (equipNum - 1) )/2 + eachWidth/2;
			for (int equipIndex = 0;equipIndex < equipNum;equipIndex++)
			{
				ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
					mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->getEquipColor(),
					false,//mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->isShenBing(),
					mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->isPartOfSuit(),
					mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->haveZhuanke());

				BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
				equipsRootNode->addChild(diwenImg, 1);
				diwenImg->setPosition(ccp(posx,equipsRootNode->getContentSize().height/2));
				//zhuangbei
				BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurrentNeedFenjieEquipFromSelListVec[equipIndex]);
				diwenImg->addChild(eqImg);
				eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
				diwenImg->setScale(eachWidth/diwenImg->getContentSize().width);

				posx += eachWidth + eachGap;
			}

			string moreEquipName = equipNum > 1 ? 
				CCString::createWithFormat(LFStrings::getValue("fenjie_list").c_str(),equipNum)->getCString() :
				"";
			//name
			CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_TiShiZhuangBeiName);
			eqName->setString( (mCurrentNeedFenjieEquipFromSelListVec[0]->getName() + moreEquipName).c_str() ) ;

		}
		else if(equipNum > 5)
		{
			const float eachWidth = 64.0f;
			const float eachGap = 25.0f;
			const float eachGapHeight = 11.0f;
			int equipNum = mCurrentNeedFenjieEquipFromSelListVec.size();
			equipNum = MIN(equipNum,9);

			const float posxInit = -( eachWidth * 5 + eachGap * (5 - 1) )/2 + eachWidth/2;
			float posx = posxInit;
			float posy = 183.0f - 131;
			for (int equipIndex = 0;equipIndex < equipNum;equipIndex++)
			{
				ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
					mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->getEquipColor(),
					false,//mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->isShenBing(),
					mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->isPartOfSuit(),
					mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->haveZhuanke());

				BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
				equipsRootNode->addChild(diwenImg, 1);
				diwenImg->setPosition(ccp(posx,posy));
				//zhuangbei
				BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurrentNeedFenjieEquipFromSelListVec[equipIndex]);
				diwenImg->addChild(eqImg);
				eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
				diwenImg->setScale(eachWidth/diwenImg->getContentSize().width);

				posx += eachWidth + eachGap;

				if (equipIndex == 4) // 换行
				{
					posy -= eachGapHeight;
					posy -= diwenImg->getContentSize().height * diwenImg->getScaleY();

					posx = posxInit;
				}
			}

			string moreEquipName = equipNum > 1 ? 
				CCString::createWithFormat(LFStrings::getValue("fenjie_list").c_str(),equipNum)->getCString() :
				"";
			//name
			CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_TiShiZhuangBeiName);
			eqName->setString( (mCurrentNeedFenjieEquipFromSelListVec[0]->getName() + moreEquipName).c_str() ) ;
		}


		string moreEquipName = equipNum > 1 ? 
			CCString::createWithFormat(LFStrings::getValue("fenjie_list").c_str(),equipNum)->getCString() :
			"";
		//name
		CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_TiShiZhuangBeiName);
		eqName->setString( (mCurrentNeedFenjieEquipFromSelListVec[0]->getName() + moreEquipName).c_str() ) ;
	}


	////1.diwen
	//{
	//	BaseSprite* equipsRootNode = BaseSprite::create();
	//	mBg->addChild(equipsRootNode, 100, Tag_Img_EquipAndDiWen);
	//	equipsRootNode->setPosition(tianjiaIten->getPosition());

	//	const float disGapX = 50.0f;
	//	const float maxDis = 300.0f;

	//	int equipNum = mCurrentNeedFenjieEquipFromSelListVec.size();

	//	float posRight = -equipNum * disGapX / 2;

	//	//posRight += (equipNum % 2 == 0) ? (disGapX / 2) : disGapX / 2;
	//	posRight += disGapX / 2;

	//	for (int equipIndex = 0;equipIndex < equipNum;equipIndex++)
	//	{
	//		ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
	//			mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->getEquipColor(),
	//			mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->isShenBing(),
	//			mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->isPartOfSuit(),
	//			mCurrentNeedFenjieEquipFromSelListVec[equipIndex]->haveZhuanke());

	//		BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
	//		equipsRootNode->addChild(diwenImg, 1);
	//		float x_offset = posRight + equipIndex * disGapX;
	//		if (x_offset < -maxDis)
	//		{
	//			x_offset = -maxDis;
	//		}
	//		if (x_offset > maxDis)
	//		{
	//			x_offset = maxDis;
	//		}
	//		diwenImg->setPosition(ccpAdd(
	//			ccp(equipsRootNode->getContentSize().width/2,equipsRootNode->getContentSize().height/2),
	//			ccp(x_offset,0)));
	//		//zhuangbei
	//		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurrentNeedFenjieEquipFromSelListVec[equipIndex]);
	//		diwenImg->addChild(eqImg);
	//		eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
	//		diwenImg->setScale(0.50);
	//	}
	//	string moreEquipName = equipNum > 1 ? 
	//		CCString::createWithFormat(LFStrings::getValue("fenjie_list").c_str(),equipNum)->getCString() :
	//		"";
	//	//name
	//	CCLabelTTF *eqName = (CCLabelTTF *)mBg->getChildByTag(Tag_Label_TiShiZhuangBeiName);
	//	eqName->setString( (mCurrentNeedFenjieEquipFromSelListVec[0]->getName() + moreEquipName).c_str() ) ;
	//}

// 	ZhuangbeiColour color = ZhuangbeiColour_Baizhuang;
// 	{
// 		int equipNum = mCurrentNeedFenjieEquipFromSelListVec.size();
// 		for (int i=0;i<equipNum;i++)
// 		{
// 			if (mCurrentNeedFenjieEquipFromSelListVec[i]->getEquipColor() > color)
// 			{
// 				color = mCurrentNeedFenjieEquipFromSelListVec[i]->getEquipColor();
// 			}
// 		}
// 	}

	int cailiao_Zhonglei = 1;  //白蓝装只有 铸铁粉、 黄装增加锻金粉、传奇装增加恶魔骨
	//全部都只有铸铁粉了
// 	switch (color)
// 	{
// 	case ZhuangbeiColour_Baizhuang:
// 	case ZhuangbeiColour_Fumo:
// 		cailiao_Zhonglei = 1;
// 		break;
// 	case ZhuangbeiColour_Xiyou:
// 		cailiao_Zhonglei = 2;
// 		break;
// 	case ZhuangbeiColour_Chuanqi:
// 		cailiao_Zhonglei = 3;
// 		break;
// 	default:
// 		break;
// 	}
	bool hadXilianJinghua = false;
	bool hadQiangHua = false;
	for (vector<Equipment*>::iterator iter = mCurrentNeedFenjieEquipFromSelListVec.begin();iter != mCurrentNeedFenjieEquipFromSelListVec.end();iter++)
	{
		if ((*iter)->getEquipColor() == ZhuangbeiColour_Xiyou)
		{
			hadXilianJinghua = true;
			if (hadQiangHua)
			{
				break;
			}
		}
		if ((*iter)->getEquipData().qianghua_data.qianghua_count > 0)
		{
			hadQiangHua = true;
			if (hadXilianJinghua)
			{
				break;
			}
		}
	}

	//分解所得
	{
		/*CCLabelTTF *suodeLabel = CCLabelTTF::create(LFStrings::getValue("fenjiehousuodecailiao").c_str(), fontStr_kaiti, LABEL_SIZE);
		mBg->addChild(suodeLabel);
		suodeLabel->setColor(LABEL_COLOR);
		suodeLabel->setPosition(ccp(320, 838-486));*/

		// 分解所得 铸铁粉		
		{//board
			BaseSprite *board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
			mBg->addChild(board, 1, Tag_Board_ZhuTieFen);
			board->setPosition(ccp(320-(cailiao_Zhonglei-1)*60, 838-620));

			if (hadXilianJinghua && hadQiangHua)
			{
				board->setPositionX(320-(cailiao_Zhonglei + 1.5)*60);
			}
			else if (hadXilianJinghua || hadQiangHua)
			{
				board->setPositionX(320-(cailiao_Zhonglei + 0.5)*60);
			}
			//name
			CCLabelTTF *name = CCLabelTTF::create(LFStrings::getValue("FenJieCaiLiao").c_str(), fontStr_kaiti, LABEL_SIZE);
			mBg->addChild(name, 0, Tag_Label_Name_ZhuTieFen);
			name->setColor(ccc3(40,40,40));
			name->setPosition(ccp(board->getPositionX(), board->getPositionY()+board->getContentSize().height/2+name->getContentSize().height));
			//image
			BaseSprite *cailiao = BaseSprite::create(
				ImageManager::getManager()->getItemFilename(ItemsType_QianghuaNormal));
			board->addChild(cailiao);
			cailiao->setPosition(ccp(46, 128-46));
			//问号
			CCLabelTTF *wenhao = CCLabelTTF::create("???", fontStr_BookAntiqua, LABEL_SIZE);
			board->addChild(wenhao, 1, Tag_Label_Num);
			wenhao->setPosition(ccp(board->getContentSize().width/2, 15));
		}
		// 材料返回 铸铁粉	
		if (hadQiangHua)
		{
			BaseSprite *board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
			mBg->addChild(board, 1, Tag_Board_CaiLiaoFanHui);
			board->setPosition(ccp(320-(cailiao_Zhonglei-2)*60, 838-620));

			if (hadXilianJinghua)
			{
				board->setPosition(ccp(320-(cailiao_Zhonglei - 1)*60,838-620));
			}
			//name
			CCLabelTTF *name = CCLabelTTF::create(LFStrings::getValue("FanHuiCaiLiao").c_str(), fontStr_kaiti, LABEL_SIZE);
			mBg->addChild(name, 0, Tag_Label_Name_CaiLiaoFanHui);
			name->setColor(ccc3(40,40,40));
			name->setPosition(ccp(board->getPositionX(), board->getPositionY()+board->getContentSize().height/2+name->getContentSize().height));
			//image
			BaseSprite *cailiao = BaseSprite::create(
				ImageManager::getManager()->getItemFilename(ItemsType_QianghuaNormal));
			board->addChild(cailiao);
			cailiao->setPosition(ccp(46, 128-46));
			//问号
			CCLabelTTF *wenhao = CCLabelTTF::create("???", fontStr_BookAntiqua, LABEL_SIZE);
			board->addChild(wenhao, 1, Tag_Label_Num);
			wenhao->setPosition(ccp(board->getContentSize().width/2, 15));
		}
		{//board
			
		}
		if (hadXilianJinghua)
		{
			m_bIsHadXiLianJingHua = true;
			//洗炼精华	
			{//board
				BaseSprite *board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
				mBg->addChild(board, 1, Tag_Board_XiLianJingHua);

				if (hadQiangHua)
				{
					board->setPosition(ccp(320-(cailiao_Zhonglei - 3.5)*60,838-620));
				}
				else
				{
					board->setPosition(ccp(320-(cailiao_Zhonglei - 2)*60,838-620));
				}
				
				//name
				CCLabelTTF *name = CCLabelTTF::create(LFStrings::getValue("XilianJinghua").c_str(), fontStr_kaiti, LABEL_SIZE);
				mBg->addChild(name, 0, Tag_Label_Name_ZhuTieFen);
				name->setColor(ccc3(40,40,40));
				name->setPosition(ccp(board->getPositionX(), board->getPositionY()+board->getContentSize().height/2+name->getContentSize().height));
				//image
				BaseSprite *cailiao = BaseSprite::create("daoju_youlanzhiye.png");
				board->addChild(cailiao);
				cailiao->setPosition(ccp(46, 128-46));
				//问号
				CCLabelTTF *wenhao = CCLabelTTF::create("???", fontStr_BookAntiqua, LABEL_SIZE);
				board->addChild(wenhao, 1, Tag_Label_Num);
				wenhao->setPosition(ccp(board->getContentSize().width/2, 15));
			}
		}
		

// 		if(cailiao_Zhonglei >= 2)// 锻金粉
// 		{//board
// 			BaseSprite *board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
// 			mBg->addChild(board, 1, Tag_Board_DuanJinFen);
// 			board->setPosition(ccp(320+60-60*(cailiao_Zhonglei%2), 838-620));
// 			//name
// 			CCLabelTTF *name = CCLabelTTF::create(CS::getItemName(ItemsType_QianghuaXiyou).c_str(), fontStr_kaiti, LABEL_SIZE);
// 			mBg->addChild(name, 0, Tag_Label_Name_ZhuTieFen + 1);
// 			name->setColor(ccc3(40,40,40));
// 			name->setPosition(ccp(board->getPositionX(), board->getPositionY()+board->getContentSize().height/2+name->getContentSize().height));
// 			//image
// 			BaseSprite *cailiao = BaseSprite::create(
// 				ImageManager::getManager()->getItemFilename(ItemsType_QianghuaXiyou));
// 			board->addChild(cailiao);
// 			cailiao->setPosition(ccp(46, 128-46));
// 			//问号
// 			CCLabelTTF *wenhao = CCLabelTTF::create("???", fontStr_BookAntiqua, LABEL_SIZE);
// 			board->addChild(wenhao, 1, Tag_Label_Num);
// 			wenhao->setPosition(ccp(board->getContentSize().width/2, 15));
// 		}
// 		if(cailiao_Zhonglei >= 3)// 恶魔骨
// 		{//board
// 			BaseSprite *board = BaseSprite::create("gongjiang_qianghua_cailiao_bg.png");
// 			mBg->addChild(board, 1, Tag_Board_EMoGu);
// 			board->setPosition(ccp(320+120, 838-620));
// 			//name
// 			CCLabelTTF *name = CCLabelTTF::create(CS::getItemName(ItemsType_QianghuaChuangqi).c_str(), fontStr_kaiti, LABEL_SIZE);
// 			mBg->addChild(name, 0, Tag_Label_Name_ZhuTieFen + 2);
// 			name->setColor(ccc3(40,40,40));
// 			name->setPosition(ccp(board->getPositionX(), board->getPositionY()+board->getContentSize().height/2+name->getContentSize().height));
// 			//image
// 			BaseSprite *cailiao = BaseSprite::create(
// 				ImageManager::getManager()->getItemFilename(ItemsType_QianghuaChuangqi));
// 			board->addChild(cailiao);
// 			cailiao->setPosition(ccp(46, 128-46));
// 			//问号
// 			CCLabelTTF *wenhao = CCLabelTTF::create("???", fontStr_BookAntiqua, LABEL_SIZE);
// 			board->addChild(wenhao, 1, Tag_Label_Num);
// 			wenhao->setPosition(ccp(board->getContentSize().width/2, 15));
// 		}
	}
}

void GongJiangFenJie::clearnBackGround(int equipNum)
{
	mMenu->retain();
	mBg->removeAllChildren();
	mBg->initWithFile(equipNum == 1?"gongjiang_fenjie_bg.png":"gongjiang_fenjie_bg_duoge.png");
	mBg->addChild(mMenu);
	mMenu->setPosition(CCPointZero);
	mMenu->release();

	mMenu->getChildByTag(Tag_MenuItem_QueRen)->setVisible(false);
	mMenu->getChildByTag(Tag_MenuItem_FanHui)->setPosition(ccp(mBg->getContentSize().width/2, 838-784));
	

	CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("qiangxuanzezhuangbei").c_str(), fontStr_kaiti, LABEL_SIZE);
	mBg->addChild(tishiLabel, 1, Tag_Label_TiShiZhuangBeiName);
	tishiLabel->setPosition(ccp(320, 838-105));
	tishiLabel->setColor(LABEL_COLOR);

	CCLabelTTF *fenjieTTF = CCLabelTTF::create(LFStrings::getValue("FenJieTuiHui").c_str(), fontStr_kaiti, 25.0f);
	mBg->addChild(fenjieTTF, 1);
	fenjieTTF->setPosition(ccp(320, 354));
	fenjieTTF->setColor(ccc3(221,171,20));
}

void GongJiangFenJie::disableAllTouchBegin()
{
	mMenu->setEnabled(false);
}

void GongJiangFenJie::disableAllTouchEnd()
{
	mMenu->setEnabled(true);
}

void GongJiangFenJie::onEnter()
{
	BaseLayer::onEnter();

	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GongJiang_FenJie))
	{
		vector<ShowMsg> list;
		ShowMsg msg1;
		msg1.msg = LFStrings::getValue("FenJieFanHuanShuoMing");
		msg1.color = ccWHITE;
		list.push_back(msg1);
		ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
			list, 2,this);
		if(layer != NULL)
		{

			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
		}
		return;
	}
	/*{
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = (CCMenuItem*)mMenu->getChildByTag(Tag_MenuItem_TianJiaZhuangBei);
		guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue("FenjieShuoMing"), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}*/
}
void GongJiangFenJie::ZhuShouchatEndCallBack()
{
	GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_FenJie);
}