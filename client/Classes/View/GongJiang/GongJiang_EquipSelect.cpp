#include "GongJiang_EquipSelect.h"
#include "CS_Common.h"
#include "font.h"
#include "GuideLayerBase.h"
#include "History.h"
#define TAG_BG_MASK_PIC 22221

GongJiang_EquipSelect* GongJiang_EquipSelect::create(GongJiangEquipSelectCallbackInterface* callbackLayer, 
													 Flag_SelectTo_GongJiang flag, Equipment *equip)
{
	GongJiang_EquipSelect *layer = new GongJiang_EquipSelect(callbackLayer, flag, equip);
	if(layer)
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

GongJiang_EquipSelect::GongJiang_EquipSelect(GongJiangEquipSelectCallbackInterface* callbackLayer, 
											 Flag_SelectTo_GongJiang flag, Equipment *equip)
	:MainLayerZhuangBeiBaseBody(From_GongJiang)
{
	//初始化
	selectedIndex = -1;
	mSetEquipCallback = callbackLayer;
	mSelectTo = flag;


	tabBar = BaseSprite::create("mianban_zhuangbei.png");
	addChild(tabBar);
	tabBar->setPosition(ccp(mWinSize.width/2,tabBar->getContentSize().height/2));

	mTableView->retain();
	mTableView->removeFromParent();
	tabBar->addChild(mTableView);
//	mTableView->initWithViewSize(CCSize(613, 687));
	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	mTableView->setPosition(ccp(8,74));
	mTableView->release();

	
	//
	string usedStr; //选择你需要 usedStr 的装备
	{
		string info;
		vector<Equipment*> allEpVec = MyselfManager::getManager()->getAllEquipments(Filter_All);
		vector<Equipment*> epVec;

		switch (mSelectTo)
		{
		case selectTo_QiangHua:
			for(int i=0; i<allEpVec.size(); ++i)
			{
				if(allEpVec[i]->canBeQianghua(info))/////////////////////////////////////强化
				{
					epVec.insert(epVec.end(), allEpVec[i]);
				}
			}
			usedStr = LFStrings::getValue("qianghua");
			break;
		case selectTo_HuiTui:
			for(int i=0; i<allEpVec.size(); ++i)
			{
				if(allEpVec[i]->canBeHuiTui(info))/////////////////////////////////////回退
				{
					epVec.insert(epVec.end(), allEpVec[i]);
				}
			}
			usedStr = LFStrings::getValue("HuiTui");
			break;
		case selectTo_JingLian: //用于被精炼
			for(int i=0; i<allEpVec.size(); ++i)
			{
				if(allEpVec[i]->canBeJingLian(info) && ! allEpVec[i]->isInFengYin())
				{
					epVec.insert(epVec.end(), allEpVec[i]);
				}
			}
			usedStr = LFStrings::getValue("jingLian");
			break;
		case selectTo_JingLianNeed: //用于精炼消耗
			for(int i=0; i<allEpVec.size(); ++i)
			{
				if(equip != NULL && equip->canJingLianUseTheEquip(info,allEpVec[i]))// && ! allEpVec[i]->isInFengYin())
				{
					epVec.insert(epVec.end(), allEpVec[i]);
				}
			}
			usedStr = LFStrings::getValue("XiaoHao");
			break;
		case selectTo_ZhuanKe://用于篆刻
			for(int i=0; i<allEpVec.size(); ++i)
			{
				if(allEpVec[i]->canBeZhuanKe(info) && ! allEpVec[i]->isInFengYin())
				{
					epVec.insert(epVec.end(), allEpVec[i]);
				}
			}
			usedStr = LFStrings::getValue("ZhuanKe");
			break;
		case selectTo_FenJie:  //不会来...分解有专门的selectLayer
			for(int i=0; i<allEpVec.size(); ++i)
			{
				//传奇装不能分解了、、、、
				if(allEpVec[i]->getActorId() == INVALID_ID && allEpVec[i]->getEquipColor() != ZhuangbeiColour_Chuanqi)
				{
					epVec.insert(epVec.end(), allEpVec[i]);
				}
			}
			usedStr = LFStrings::getValue("FenJie");
			break;
		case selectTo_BaoShiXiangQian:
			for(int i=0; i<allEpVec.size(); ++i)
			{
				if(CS::canZBXiangqianBaoshi(allEpVec[i]->getEquipId()))
				{
					epVec.insert(epVec.end(), allEpVec[i]);
				}
			}
			usedStr = LFStrings::getValue("XiangQian");
			break;
		case selectTo_BaoShiCaiXie:
			for(int i=0; i<allEpVec.size(); ++i)
			{
				unsigned int diamond_1 = allEpVec[i]->getEquipData().diamond_type_1;
				unsigned int diamond_2 = allEpVec[i]->getEquipData().diamond_type_2;
				unsigned int diamond_3 = allEpVec[i]->getEquipData().diamond_type_3;
				if(diamond_1 > 0 || diamond_2 > 0 || diamond_3 > 0)
				{
					epVec.push_back(allEpVec[i]);
				}
			}
			usedStr = LFStrings::getValue("ChaiXie");
			break;

		case selectTo_JianDingNeed:
			{
				vector<Equipment*> equips;
			 	if (equip->getEquipmentsCanUsedToJieFeng(equips) != STATE_Ok)
			 	{
			 		CCLOG("no JianDingNeed..............");
			 	}
				usedStr = LFStrings::getValue("XiaoHao");
				epVec = equips;
			}
			break;
		default:
			CCAssert(false, "Error enum value");
			break;
		}

		if (mSelectTo == selectTo_FenJie)
			setEquipmentData(epVec, SORT_IN_FenJie);
		else
			setEquipmentData(epVec, SORT_IN_GongJiang);
	}

	//创建按钮
	{
		mMenuItemCancel = CCMenuItemImage::create(
			"xuanze_zhuang_bei_quxiao.png",
			"xuanze_zhuang_bei_quxiao_select.png",
			"xuanze_zhuang_bei_quxiao_select.png",
			this,SEL_MenuHandler(&GongJiang_EquipSelect::menuItemCancelClicked));
		mMenuItemMakeSure = CCMenuItemImage::create(
			"xuanze_zhuang_bei_queding.png",
			"xuanze_zhuang_bei_queding_select.png",
			"xuanze_zhuang_bei_queding_select.png",
			this,SEL_MenuHandler(&GongJiang_EquipSelect::menuItemMakeSureClicked));


		mMenu = CCMenu::create(mMenuItemCancel, mMenuItemMakeSure, NULL);
		addChild(mMenu,1);
		mMenu->setPosition(CCPointZero);


		mMenuItemCancel->setPosition(ccp(86,804));

		mMenuItemMakeSure->setPosition(ccp(554,804));
		mMenuItemMakeSure->setEnabled(false);
	}


	//title
	CCLabelTTF *title = CCLabelTTF::create(LFStrings::getValue("XuanZeNiXuyao_deZhuangBei").c_str(), fontStr_kaiti, 20);
	addChild(title, 1);
	title->setColor(fonColor_PuTong);
	title->setPosition(ccp(320, 804));
	CCLabelTTF *la = CCLabelTTF::create(usedStr.c_str(), fontStr_kaiti, 20);
	addChild(la, 1);
	la->setPosition(ccp(340, 804));

	//mTableView->get
	select();
}

// void GongJiang_EquipSelect::refreshItem(CCNode* item,int index)
// {
// 	MainLayerZhuangBeiBaseBody::refreshItem(item,index);
// 
// 	if (index != selectedIndex)
// 	{
// 		item->removeChildByTag(TAG_BG_MASK_PIC);
// 	}else
// 	{
// 		if (!item->getChildByTag(TAG_BG_MASK_PIC))
// 		{
// 			BaseSprite* addEff = BaseSprite::create("mianban_zhuangbei_xiangxi_select.png");
// 			item->addChild(addEff,-1,TAG_BG_MASK_PIC);
// 			addEff->setPosition(ccp(item->getContentSize().width/2,item->getContentSize().height/2));
// 		}
// 	}
// }

void GongJiang_EquipSelect::menuItemCancelClicked(CCObject* obj)
{
	mFooterLayer->dismmisPopUp();
}

void GongJiang_EquipSelect::menuItemMakeSureClicked(CCObject* obj)
{
	unsigned long long eqID = 0;
	if(selectedIndex >= 0)
	{
		eqID = mEquipmentVec[selectedIndex]->getEquipId();
	}

	mSetEquipCallback->selectEquipmentCallback(eqID, mSelectTo);

// 	if (mFlag == selectTo_JingLianNeed)
// 		mJingLianLayer->setSelectedNeedEquipId(eqID);
// 	
// 	else 
// 		mJingLianLayer->setSelectedEquipmentId(eqID);

	//
	mFooterLayer->dismmisPopUp();
}

BaseSprite* GongJiang_EquipSelect::getItemContentByIndex(unsigned int index)
{
	BaseSprite* ret = MainLayerZhuangBeiBaseBody::getItemContentByIndex(index);

	if (selectedIndex == index)
	{
		BaseSprite* addEff = BaseSprite::create("mianban_zhuangbei_xiangxi_select.png");
		ret->addChild(addEff,10,TAG_BG_MASK_PIC);
		addEff->setPosition(ccp(ret->getContentSize().width/2,ret->getContentSize().height/2));
	}

	return ret;
}

void GongJiang_EquipSelect::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	BaseSprite* bg = (BaseSprite*)cell->getChildByTag(TAG_EquipBaseBody_TABLE_ITEM);
	// 先不管是否点击了图片

	// 先移除上一个点击的
	if (selectedIndex >= 0)
	{
		CCTableViewCell* lastCell = table->cellAtIndex(selectedIndex);
		if (lastCell)
		{
			lastCell->getChildByTag(TAG_EquipBaseBody_TABLE_ITEM)->removeChildByTag(TAG_BG_MASK_PIC);
		}
	}
	// 再次选自己，代表取消
	if (selectedIndex == cell->getIdx())
	{
		selectedIndex = -1;
	}
	else
	{
		selectedIndex = cell->getIdx();
		BaseSprite* addEff = BaseSprite::create("mianban_zhuangbei_xiangxi_select.png");
		bg->addChild(addEff,10,TAG_BG_MASK_PIC);
		addEff->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
	}

	if(selectedIndex < 0)
	{
		mMenuItemMakeSure->setEnabled(false);
	}
	else
	{
		mMenuItemMakeSure->setEnabled(true);
	}
	bool isFirstIn = false;
	// 先不管是否点击了图片
	if(GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GongJiang_QiangHua))
	{
		//GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_QhHt);
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_QiangHua);

		if (mMenuItemMakeSure)
		{
			CCPoint pos =  mMenuItemMakeSure->getParent()->convertToWorldSpace(mMenuItemMakeSure->getPosition());
			GuideLayerBase *guideLayer1 = new GuideLayer_SimpleLayer;
			guideLayer1->init(mMenuItemMakeSure->getContentSize(),pos
				, LFStrings::getValue("DingjiQueding"), NULL, false);
			//((GuideLayer_SimpleLayer*)guideLayer1)->setCallBack(this,SEL_CallFunc(&JiuGongGeLayer::GuiDeLayerCallBack));
			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer1, 128);
		}
	}
}

void GongJiang_EquipSelect::disableAllTouchBegin()
{
	mMenu->setEnabled(false);
	MainLayerZhuangBeiBaseBody::disableAllTouchBegin();
}
void GongJiang_EquipSelect::disableAllTouchEnd()
{
	mMenu->setEnabled(true);
	MainLayerZhuangBeiBaseBody::disableAllTouchEnd();
}
void GongJiang_EquipSelect::addTeach()
{
}
void GongJiang_EquipSelect::select()
{
	CCTableViewCell* lastCell = mTableView->cellAtIndex(0);
	if (lastCell)
	{
		BaseSprite* bg = (BaseSprite*)lastCell->getChildByTag(TAG_EquipBaseBody_TABLE_ITEM);

		bool isFirstIn = false;
		// 先不管是否点击了图片
		if(GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GongJiang_QiangHua))
		{
			//GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_SelectZhuangBei);
			//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_GongJiang_SelectZhuangBei);
			if (bg)
			{
				CCPoint pos = bg->getParent()->convertToWorldSpace(bg->getPosition());
				GuideLayerBase *guideLayer1 = new GuideLayer_SimpleLayer;
				guideLayer1->init(bg->getContentSize(), ccp(pos.x,pos.y + bg->getContentSize().height / 2)
					, LFStrings::getValue("qiangxuanzezhuangbei"), NULL, false);
				//((GuideLayer_SimpleLayer*)guideLayer1)->setCallBack(this,SEL_CallFunc(&JiuGongGeLayer::GuiDeLayerCallBack));
				CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer1, 128);
			}
		}
	}

}