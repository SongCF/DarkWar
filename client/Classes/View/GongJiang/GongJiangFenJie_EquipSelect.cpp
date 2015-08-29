#include "GongJiangFenJie_EquipSelect.h"
#include "GongJiangFenJie.h"
#include "CS_Common.h"
#include "font.h"

#define TAG_BG_MASK_PIC 22221


//06-14 20:50:24.949: D/cocos2d-x debug info(6365): MainLayerGongjiangFrame::tableCellAtIndex new --> index = [4],file = [function_knapsack.png]

GongJiangFenJie_EquipSelect* GongJiangFenJie_EquipSelect::create(GongJiangFenJie* chaijieLayer)
{
	GongJiangFenJie_EquipSelect *layer = new GongJiangFenJie_EquipSelect(chaijieLayer);
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

GongJiangFenJie_EquipSelect::GongJiangFenJie_EquipSelect(GongJiangFenJie* chaijieLayer)
	:MainLayerZhuangBeiBaseBody(From_GongJiang)
{
	//初始化
	selectedEqList.clear();

	mChaijieLayer = chaijieLayer;

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
	{
		vector<Equipment*> allEpVec = MyselfManager::getManager()->getAllEquipments(Filter_All);
		vector<Equipment*> epVec;
		for(int i=0; i<allEpVec.size(); ++i)
		{
			//传奇装不能分解了、、、、
			if(allEpVec[i]->getActorId() == INVALID_ID && allEpVec[i]->getEquipColor() != ZhuangbeiColour_Chuanqi)
			{
				epVec.insert(epVec.end(), allEpVec[i]);
			}
		}

		setEquipmentData(epVec, SORT_IN_FenJie);
	}

	//创建按钮
	{
		mMenuItemCancel = CCMenuItemImage::create(
			"xuanze_zhuang_bei_quxiao.png",
			"xuanze_zhuang_bei_quxiao_select.png",
			"xuanze_zhuang_bei_quxiao_select.png",
			this,SEL_MenuHandler(&GongJiangFenJie_EquipSelect::menuItemCancelClicked));
		mMenuItemMakeSure = CCMenuItemImage::create(
			"xuanze_zhuang_bei_queding.png",
			"xuanze_zhuang_bei_queding_select.png",
			"xuanze_zhuang_bei_queding_select.png",
			this,SEL_MenuHandler(&GongJiangFenJie_EquipSelect::menuItemMakeSureClicked));


		mMenu = CCMenu::create(mMenuItemCancel, mMenuItemMakeSure, NULL);
		addChild(mMenu,1);
		mMenu->setPosition(CCPointZero);


		mMenuItemCancel->setPosition(ccp(86,804));

		mMenuItemMakeSure->setPosition(ccp(554,804));
		mMenuItemMakeSure->setEnabled(false);
	}

	//title
	CCLabelTTF *title = CCLabelTTF::create((
		LFStrings::getValue("XuanZeNiXuyao_deZhuangBei")+"("+LFStrings::getValue("KeDuoXuan")+")").c_str(), fontStr_kaiti, 20);
	addChild(title, 1);
	title->setColor(fonColor_PuTong);
	title->setPosition(ccp(320, 804));
	CCLabelTTF *la = CCLabelTTF::create(LFStrings::getValue("FenJie").c_str(), fontStr_kaiti, 20);
	addChild(la, 1);
	la->setPosition(ccp(300, 804));

}

BaseSprite* GongJiangFenJie_EquipSelect::getItemContentByIndex(unsigned int index)
{
	BaseSprite* ret = MainLayerZhuangBeiBaseBody::getItemContentByIndex(index);

	map<unsigned long long,Equipment*>::iterator it = selectedEqList.find(mEquipmentVec[index]->getEquipId());
	if ( it != selectedEqList.end())
	{
		BaseSprite* addEff = BaseSprite::create("mianban_zhuangbei_xiangxi_select.png");
		ret->addChild(addEff,10,TAG_BG_MASK_PIC);
		addEff->setPosition(ccp(ret->getContentSize().width/2,ret->getContentSize().height/2));
	}

	return ret;
}
void GongJiangFenJie_EquipSelect::menuItemCancelClicked(CCObject* obj)
{
	mFooterLayer->dismmisPopUp();
}

void GongJiangFenJie_EquipSelect::menuItemMakeSureClicked(CCObject* obj)
{
	if (!selectedEqList.empty())
	{
		mChaijieLayer->setSelectedEquipmentId(selectedEqList);
		//
		mFooterLayer->dismmisPopUp();
	}

	//unsigned long long eqID = 0;
	//if(selectedIndex >= 0)
	//{
	//	eqID = mEquipmentVec[selectedIndex]->getEquipId();
	//}
	//mChaijieLayer->setSelectedEquipmentId(eqID);

	////
	//mFooterLayer->dismmisPopUp();
}

void GongJiangFenJie_EquipSelect::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	BaseSprite* bg = (BaseSprite*)cell->getChildByTag(TAG_EquipBaseBody_TABLE_ITEM);
	// 先不管是否点击了图片
	
	// 再次选自己，代表取消
	map<unsigned long long,Equipment*>::iterator it = selectedEqList.find(mEquipmentVec[cell->getIdx()]->getEquipId());
	if ( it != selectedEqList.end())
	{
		// 移除自己的选择框
		cell->getChildByTag(TAG_EquipBaseBody_TABLE_ITEM)->removeChildByTag(TAG_BG_MASK_PIC);
		selectedEqList.erase(it);
	}else
	{
		selectedEqList.insert(pair<unsigned long long,Equipment*>(mEquipmentVec[cell->getIdx()]->getEquipId(),mEquipmentVec[cell->getIdx()]));
		BaseSprite* addEff = BaseSprite::create("mianban_zhuangbei_xiangxi_select.png");
		bg->addChild(addEff,10,TAG_BG_MASK_PIC);
		addEff->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));
	}


	if(selectedEqList.empty())
	{
		mMenuItemMakeSure->setEnabled(false);
	}
	else
	{
		mMenuItemMakeSure->setEnabled(true);
	}

	//bar
	{
		CCString temp;
		if (m_zbFenJieTiShi)
		{
			m_zbFenJieTiShi->setVisible(true);
			m_zbFenJieTiShi->setColor(fonColor_PuTong);
			temp.initWithFormat(LFStrings::getValue("YiXuanFenJie_d_ZhuangBei").c_str(), selectedEqList.size());
			m_zbFenJieTiShi->setString(temp.getCString());
		}

		temp.initWithFormat("%s: %d/%u", LFStrings::getValue("zhuangbei").c_str()
			, MyselfManager::getManager()->getAllEquipmentsWithOutEquipedOrChuanQi().size() - selectedEqList.size(), MAX_COUNTS_IN_KNAPSACK);
		if (m_zbTotalLabel) 
		{
			m_zbTotalLabel->setString(temp.getCString());
		}
	}
}

void GongJiangFenJie_EquipSelect::disableAllTouchBegin()
{
	mMenu->setEnabled(false);
	MainLayerZhuangBeiBaseBody::disableAllTouchBegin();
}
void GongJiangFenJie_EquipSelect::disableAllTouchEnd()
{
	mMenu->setEnabled(true);
	MainLayerZhuangBeiBaseBody::disableAllTouchEnd();
}


GongJiangFenJie_EquipSelect::~GongJiangFenJie_EquipSelect(void)
{
}
