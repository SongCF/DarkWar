#include "MainLayerBeibaoItemList.h"
#include "CS_Common.h"
#include "WuPinManager.h"
//#include "MofaJinghuaManager.h"
#include "SuiPianManager.h"
#include "font.h"
#include "SkillTree.h"
#include "SuitPool.h"
#include "FX_CommonFunc.h"
#include "ImageManager.h"
#include "UnblockManager.h"
#include "SystemGiftBagManager.h"
#include "font.h"
#include "FXScrollMenu.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "MainLayerStoreFrame.h"
#include "AwardDisplay.h"

using namespace FX_CommonFunc;

#define TAG_BG_MASK_PIC 22221

#define TAG_HeadMenu       1120
#define TAG_HeadMenu_DaoJu 1121
#define TAG_HeadMenu_BaoShi 1122
#define TAG_HeadMenu_CaiLiao 1123
#define TAG_HeadMenu_ZaXiang 1124
#define Tag_Menu             1125
#define Tag_MenuItem_Da_KAi   1126
#define LabelSize 20 


MainLayerBeibaoItemList::MainLayerBeibaoItemList(int from)
{
	id=0;
	selectedIndex = -1;

	mFrom = from;
	mBg = BaseSprite::create("beibao_bg.jpg");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2,mBg->getContentSize().height/2));

	mTableView->initWithViewSize(CCSize(640, 721));
	mTableView->retain();
	mTableView->removeFromParent();
	mBg->addChild(mTableView);
	mTableView->setPosition(ccp(13,28));
	mTableView->release();

	//head menu
	{
		//道具 礼包
		CCMenuItemImage* dj = CCMenuItemImage::create(
			"daoju_dinglan_daoju_dengdai.png"
			, "daoju_dinglan_daoju_dengdai.png"
			, "daoju_dinglan_daoju_select.png"
			, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));
		//宝石
		CCMenuItemImage* bs = CCMenuItemImage::create(
			"daoju_dinglan_baoshi_dengdai.png"
			, "daoju_dinglan_baoshi_dengdai.png"
			, "daoju_dinglan_baoshi_select.png"
			, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));
		//材料
		CCMenuItemImage* cl = CCMenuItemImage::create(
			"daoju_dinglan_cailiao_dengdai.png"
			, "daoju_dinglan_cailiao_dengdai.png"
			, "daoju_dinglan_cailiao_select.png"
			, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));
		//杂项  碎片
		CCMenuItemImage* zx = CCMenuItemImage::create(
			"daoju_dinglan_zaxiang_dengdai.png"
			, "daoju_dinglan_zaxiang_dengdai.png"
			, "daoju_dinglan_zaxiang_select.png"
			, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));

		//points
		float sameY = mBg->getContentSize().height-47;
		CCPoint item1 = ccp(98, sameY);
		CCPoint item2 = ccp(246, sameY);
		CCPoint item3 = ccp(394, sameY);
		CCPoint item4 = ccp(542, sameY);

		cl->setPosition(item1);
		cl->setTag(TAG_HeadMenu_CaiLiao);
		bs->setPosition(item2);
		bs->setTag(TAG_HeadMenu_BaoShi);
		dj->setPosition(item3);
		dj->setTag(TAG_HeadMenu_DaoJu);
		zx->setPosition(item4);
		zx->setTag(TAG_HeadMenu_ZaXiang);

		CCMenu* menu = CCMenu::create(dj, bs, cl, zx, NULL);
		mBg->addChild(menu, 0, TAG_HeadMenu);
		menu->setPosition(CCPointZero);

		headItemClicked(cl);
	}

	//menu ok  cancel   if came from BuZhen
	{
		CCMenuItemImage* makeSure = CCMenuItemImage::create(
			"buzhen_queren.png",
			"buzhen_queren_select.png",
			this,SEL_MenuHandler(&MainLayerBeibaoItemList::makeSureClicked));
		CCMenuItemImage* cacel = CCMenuItemImage::create(
			"shangpin_goumai_quxiao.png",
			"shangpin_goumai_quxiao_select.png",
			this,SEL_MenuHandler(&MainLayerBeibaoItemList::cacelClicked));
		makeSure->setPosition(mWinSize.width/3, 50);
		cacel->setPosition(mWinSize.width/3*2, 50);

		CCMenu* menu = CCMenu::create(makeSure, cacel, NULL);
		addChild(menu);
		menu->setPosition(ccp(0,0));

		if(from == NULL)
		{
			makeSure->setVisible(false);
			cacel->setVisible(false);
		}
		else
		{
			headItemClicked(this->getChildByTag(TAG_HeadMenu)->getChildByTag(TAG_HeadMenu_DaoJu));
		}
	}

	{

			addObserver(callfuncO_selector(MainLayerBeibaoItemList::rsp_getLibao),MSG_QuerySystemGiftBagRsp);
			CmdHelper::getHelper()->cmdQuerySystemGiftBag();

	}
}


void MainLayerBeibaoItemList::disableAllTouchBegin()
{

}
void MainLayerBeibaoItemList::disableAllTouchEnd()
{

}


MainLayerBeibaoItemList::~MainLayerBeibaoItemList(void)
{
}

void MainLayerBeibaoItemList::headItemClicked(CCObject* pSender)
{
	refreshAllHeadMenu();
	id=0;
	vector<ItemInfo> allItems = WuPinManager::getManager()->getAllItems();
	//去掉工匠铁锤 20140530
	for (vector<ItemInfo>::iterator it=allItems.begin(); it!=allItems.end(); ++it){
		if (it->type == ItemsType_Hammer){
			allItems.erase(it);
			break;
		}
	}
	
	
	vector<ItemInfo> curItems;
//	vector<MofaJinghua> allJinghua;
	vector<ItemSuipian> suipianItemVec;
	vector<ShenbingSuipian> suipianEquipVec;
	CCMenuItemImage* item = ((CCMenuItemImage*)pSender);
	switch (item->getTag())
	{
	case TAG_HeadMenu_DaoJu:
		item->initWithNormalImage("daoju_dinglan_daoju_select.png"
			, "daoju_dinglan_daoju_dengdai.png"
			, "daoju_dinglan_daoju_dengdai.png"
			, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));
	    id=1;
		break;
	case TAG_HeadMenu_BaoShi:
		item->initWithNormalImage("daoju_dinglan_baoshi_select.png"
			, "daoju_dinglan_baoshi_dengdai.png"
			, "daoju_dinglan_baoshi_dengdai.png"
			, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));

		
		for (int i=0; i!=allItems.size(); ++i)
		{
			if(allItems.at(i).type >= ItemsType_Baoshi_min && allItems.at(i).type <= ItemsType_Baoshi_max)
				curItems.push_back(allItems.at(i));
		}
		break;
	case TAG_HeadMenu_CaiLiao:
		item->initWithNormalImage("daoju_dinglan_cailiao_select.png"
			, "daoju_dinglan_cailiao_dengdai.png"
			, "daoju_dinglan_cailiao_dengdai.png"
			, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));
		
		for (int i=0; i!=allItems.size(); ++i)
		{
			if((allItems.at(i).type >= ItemsType_QianghuaNormal && allItems.at(i).type <= ItemsType_QianghuaGailvTianjia)
				|| (allItems.at(i).type >= ItemsType_HellStone && allItems.at(i).type <= ItemsType_JingwenZhuanke)
				|| allItems.at(i).type >= ItemsType_DaiGouJuan)
				curItems.push_back(allItems.at(i));
		}
		break;
	case TAG_HeadMenu_ZaXiang:
		item->initWithNormalImage("daoju_dinglan_zaxiang_select.png"
			, "daoju_dinglan_zaxiang_dengdai.png"
			, "daoju_dinglan_zaxiang_dengdai.png"
			, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));
		
		//杂项放的精华
// 		for (int i=0; i!=allItems.size(); ++i)
// 		{
// 			if(allItems.at(i).type == ItemsType_Gold || allItems.at(i).type == ItemsType_Silver)
// 				curItems.push_back(allItems.at(i));
// 		}
//		allJinghua = MofaJinghuaManager::getManager()->getAllMoFaJingHua();
		suipianItemVec = SuiPianManager::getManager()->getAllItemSuiPian();
		suipianEquipVec = SuiPianManager::getManager()->getAllShenBingSuiPian();
		break;
	default:
		break;
	}

	bool bShowTip = false;
	if (item->getTag() == TAG_HeadMenu_ZaXiang)
	{
		if (suipianEquipVec.size() + suipianItemVec.size() <= 0)
		{
			bShowTip = true;
		}
	}
	else
	{
		if (curItems.size() <= 0)
		{
			bShowTip = true;
		}

		if(id==1&&giftbags.size()>0){
			bShowTip = false;
		}
	}

	if (bShowTip)
	{
		if(id==1&&giftbags.size()>0){
			
		}else{
			string infoStr = LFStrings::getValue("Beibao_Empty");
			UnblockWarnDialog* pDlg =UnblockWarnDialog::create(infoStr);
			CCDirector::sharedDirector()->getRunningScene()->addChild(pDlg, 300);
		}
	}

	setItemsData(curItems, suipianItemVec, suipianEquipVec);
}

void MainLayerBeibaoItemList::setItemsData(vector<ItemInfo> itemsVec, vector<ItemSuipian> suipianItemVec, vector<ShenbingSuipian> suipianEquipVec)
{
	this->mItemsVec = itemsVec; 
	this->mSuipianItemVec = suipianItemVec;
	this->mSuipianEquipVec = suipianEquipVec;
	giftbags.clear();
	giftbags =*SystemGiftBagManager::getManager()->getAllGiftBag();
	
	mTableView->reloadData();
}

CCTableViewCell* MainLayerBeibaoItemList::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCString *string = CCString::createWithFormat("%d", idx);
	CCTableViewCell *cell = table->dequeueCell();
	if (cell == NULL) 
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}
	else
	{
		cell->removeAllChildren();
	}

	cell->addChild(getItemContentByIndex(idx),0,TAG_BaseBody_TABLE_ITEM);


	return cell;
}

BaseSprite* MainLayerBeibaoItemList::getItemContentByIndex(unsigned int index)
{
	BaseSprite* bg = BaseSprite::create("shangdian_shangpin_bg.png");
	BaseSprite *sprite = NULL;
	//材料

   
	
	 if(id==0){

			if(index < mItemsVec.size())
			{


				unsigned int item_type = mItemsVec[index].type;
				//item图片
				{
					sprite = BaseSprite::create(ImageManager::getManager()->getItemFilename(item_type));
			
					sprite->setPosition(ccp(68, bg->getContentSize().height/2));

					bg->addChild(sprite,1,TAG_BaseBody_TABLE_ITEM_Image);
				}
				// 名字
				{
					string name ;
					bool useColor = false;
					ccColor3B colorName;
					if(item_type > ItemsType_SkillBook_MinId && item_type < ItemsType_SkillBook_MaxId){
						name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(item_type-ItemsType_SkillBook_MinId));
					}else {
						name = CS::getItemName(item_type);
						useColor = true;
						colorName = FX_CommonFunc::getCaiLiaoColor(item_type);//材料颜色分类 道具和宝石
					}

					CCLabelTTF* lable = LF::lable(bg,name,ccp(142,116),23,fontStr_kaiti); 
					if(useColor)
						lable->setColor(colorName);
					lable->setTag(TAG_BaseBody_LABLE_NAME);
				}
				//属性
				{
					string shuxingStr ;
					if(item_type > ItemsType_SkillBook_MinId && item_type < ItemsType_SkillBook_MaxId)
						shuxingStr = SkillTree::sharedSkillTree()->getSkillDescribeByType((SkillType)(item_type-ItemsType_SkillBook_MinId));
					else 
						shuxingStr = CS::getItemShuxing(item_type);
					CCLabelTTF* lable = CCLabelTTF::create(shuxingStr.c_str(), fontStr_kaiti,20,CCSize(450, 0), kCCTextAlignmentLeft);
					bg->addChild(lable);
					lable->setAnchorPoint(ccp(0, 0.5));
					lable->setPosition(ccp(142, 76));
					lable->setTag(TAG_BaseBody_LABLE_SHUXING);
					lable->setColor(ccc3(201, 156, 18));
				}
				//数量
				{
					CCLabelTTF* lable = LF::lable(bg, (LFStrings::getValue("shuliang")+": ").c_str(),ccp(142,32),22,fontStr_kaiti);
					lable->setTag(TAG_BaseBody_LABLE_COUNT);
					ccColor3B color;
					color.r = 89;
					color.g = 111;
					color.b = 183;
					lable->setColor(color);
				}
				//数量数值
				{
					char str[32];
					sprintf(str, "%d",mItemsVec[index].count);
					CCLabelTTF* lable = LF::lable(bg, str,ccp(202,32),20,fontStr);
					lable->setTag(TAG_BaseBody_LABLE_COUNT_NUM);
				}
			}
			//材料碎片
			else if(mSuipianItemVec.size() > 0 && index < mItemsVec.size()+mSuipianItemVec.size())
			{
				ItemSuipian spData = mSuipianItemVec.at(index - mItemsVec.size());
				//item图片
				{
					sprite = BaseSprite::create(ImageManager::getManager()->getItemDebrisFileName(spData.item_type));
					sprite->setPosition(ccp(68, bg->getContentSize().height/2));
					bg->addChild(sprite,1,TAG_BaseBody_TABLE_ITEM_Image);
				}
				// 名字
				{
					//name
					string name;
					if (spData.item_type >= ItemsType_SkillBook_MinId && spData.item_type <= ItemsType_SkillBook_MaxId)
						name = SkillTree::sharedSkillTree()->getSkillNameAddInfo((SkillType)(spData.item_type - ItemsType_SkillBook_MinId), true);
					else 
						name = CS::getItemName(spData.item_type);
					string nameTemp = name + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(spData.suipian_id);
					CCLabelTTF* lable = LF::lable(bg,nameTemp,ccp(142,116),23,fontStr_kaiti);
					lable->setTag(TAG_BaseBody_LABLE_NAME);
				}
				//属性
				{
					CCLabelTTF* lable = CCLabelTTF::create("", fontStr_kaiti,20,CCSize(450, 0), kCCTextAlignmentLeft);
					bg->addChild(lable);
					lable->setAnchorPoint(ccp(0, 0.5));
					lable->setPosition(ccp(142, 70));
					lable->setTag(TAG_BaseBody_LABLE_SHUXING);
					//CCSize size = lable->getContentSize();

					ccColor3B color;
					color.r = 201;
					color.g = 156;
					color.b = 18;
					lable->setColor(color);
				}
				//数量
				{
					CCLabelTTF* lable = LF::lable(bg, (LFStrings::getValue("shuliang")+": ").c_str(),ccp(142,24),22,fontStr_kaiti);
					lable->setTag(TAG_BaseBody_LABLE_COUNT);
					ccColor3B color;
					color.r = 89;
					color.g = 111;
					color.b = 183;
					lable->setColor(color);
				}
				//数量数值
				{
					char str[32];
					sprintf(str, "%d",spData.suipian_count);
					CCLabelTTF* lable = LF::lable(bg, str,ccp(202,24),20,fontStr);
					lable->setTag(TAG_BaseBody_LABLE_COUNT_NUM);
				}
			}
			//神兵碎片
			else if (mSuipianEquipVec.size() > 0 && index >= mItemsVec.size()+mSuipianItemVec.size())
			{
				ShenbingSuipian spData = mSuipianEquipVec.at(index - mItemsVec.size() - mSuipianItemVec.size());
				//item图片
				{
					string diwenFile = "wuqi_diwen_jiezhi1_chuanqi.png";
		// 			if (SuitPool::getPool()->isSuit(spData.suiPianDetail.zb_group_id))
		// 				diwenFile = "wuqi_diwen_jiezhi1_shenbing_taozhuang.png";
		// 			else
		// 				diwenFile = "wuqi_diwen_jiezhi1_shenbing.png";
					BaseSprite *diwenImg = BaseSprite::create(diwenFile);
					bg->addChild(diwenImg,0,TAG_BaseBody_TABLE_ITEM_Image);
					diwenImg->setPosition(ccp(68, bg->getContentSize().height/2+15));
					//
					string equipfile = spData.suiPianDetail.zhuangbei_pic;
					equipfile = "minute_" + equipfile + ".png";
					BaseSprite *sprite = BaseSprite::create(equipfile);
					diwenImg->addChild(sprite);
					sprite->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
					sprite->setScale(0.2f);
					//
					BaseSprite *maskImg = BaseSprite::create(ImageManager::getManager()->getShenbingDebrisMaskFileName());
					diwenImg->addChild(maskImg, 1);
					maskImg->setPosition(sprite->getPosition());
					maskImg->setScaleX(diwenImg->getContentSize().width / maskImg->getContentSize().width);
					maskImg->setScaleY(diwenImg->getContentSize().height / maskImg->getContentSize().height);
				}
				// 名字
				{
					//name
					string name = spData.suiPianDetail.zhuangbei_name + LFStrings::getValue("zhi")
						+ LFStrings::getValue("suipian") + getStrByInt(spData.suipian_id);
					CCLabelTTF* lable = LF::lable(bg,name,ccp(142,116),23,fontStr_kaiti);
					lable->setTag(TAG_BaseBody_LABLE_NAME);
				}
				//属性
				{
					CCLabelTTF* lable = CCLabelTTF::create("", fontStr_kaiti,20,CCSize(450, 0), kCCTextAlignmentLeft);
					bg->addChild(lable);
					lable->setAnchorPoint(ccp(0, 0.5));
					lable->setPosition(ccp(142, 70));
					lable->setTag(TAG_BaseBody_LABLE_SHUXING);
					//CCSize size = lable->getContentSize();

					ccColor3B color;
					color.r = 201;
					color.g = 156;
					color.b = 18;
					lable->setColor(color);
				}
				//数量
				{
					CCLabelTTF* lable = LF::lable(bg, (LFStrings::getValue("shuliang")+": ").c_str(),ccp(142,34),22,fontStr_kaiti);
					lable->setTag(TAG_BaseBody_LABLE_COUNT);
					ccColor3B color;
					color.r = 89;
					color.g = 111;
					color.b = 183;
					lable->setColor(color);
				}
				//数量数值
				{
					char str[32];
					sprintf(str, "%d",spData.suipian_count);
					CCLabelTTF* lable = LF::lable(bg, str,ccp(202,34),20,fontStr);
					lable->setTag(TAG_BaseBody_LABLE_COUNT_NUM);
				}
			}

	}else{

		
		//libao img
		BaseSprite *LbImg = BaseSprite::create(ImageManager::getManager()->getGiftBagFileName(giftbags[index].id));
		bg->addChild(LbImg,2);
		LbImg->setPosition(ccp(72, bg->getContentSize().height/2));

		//礼包背景
		BaseSprite *LbImgBg = BaseSprite::create(ImageManager::getManager()->getGiftBagBoardName(giftbags[index].id));
		bg->addChild(LbImgBg,1);
		LbImgBg->setPosition(ccp(72, bg->getContentSize().height/2));


		
		//获取礼包名
		std::string lbm=SystemGiftBagManager::getManager()->getGiftBagName(giftbags[index].id);
		CCLabelTTF *LbmLabel = CCLabelTTF::create(lbm.c_str(), fontStr_kaiti, LabelSize, CCSize(330, 0), kCCTextAlignmentLeft);
		bg->addChild(LbmLabel);
		LbmLabel->setAnchorPoint(ccp(0, 1));
		LbmLabel->setPosition(ccp(142, 148-32));

		//礼包数量
		string  Lbnumstr=LF::getString("LBSL");
		string  lbnum=CCString::createWithFormat("%d",giftbags[index].count)->getCString();
		Lbnumstr+=":"+lbnum;
		CCLabelTTF *Libaonumlabel = CCLabelTTF::create(Lbnumstr.c_str(), fontStr_kaiti, LabelSize);
		bg->addChild(Libaonumlabel);
		Libaonumlabel->setColor(ccc3(89,111,183));
		Libaonumlabel->setAnchorPoint(ccp(0, 0.5f));
		Libaonumlabel->setPosition(ccp(142,32));


		//获取礼包描述
		std::string lbms=SystemGiftBagManager::getManager()->getGiftBagDescribe(giftbags[index].id);
		CCLabelTTF *LbmsLabel = CCLabelTTF::create(lbms.c_str(), fontStr_kaiti, LabelSize, CCSize(330, 0), kCCTextAlignmentLeft);
		bg->addChild(LbmsLabel);
		LbmsLabel->setAnchorPoint(ccp(0, 0.5));
		LbmsLabel->setPosition(ccp(LbmLabel->getPositionX(), 
			LbmLabel->getPositionY()-LbmLabel->getContentSize().height-LbmsLabel->getContentSize().height/2-5));
		LbmsLabel->setColor(ccc3(195,145,96));


		//打开礼包
		FXScrollMenu *menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		bg->addChild(menu, 0, Tag_Menu);
		menu->setPosition(CCPointZero);

		CCMenuItemImage *item= CCMenuItemImage::create(
				"libao_dakai.png"
				, "libao_dakai_select.png"
				, this, menu_selector(MainLayerBeibaoItemList::menuItemClicked_Da_Kai));
		item->setPosition(ccp(534, 148-70));
		item->setUserData((void*)index);
		menu->addChild(item, 0, Tag_MenuItem_Da_KAi);


		
	}



	bg->setPosition(ccp(bg->getContentSize().width/2,bg->getContentSize().height/2));

	if (sprite)
	{
		std::string biankuangName = ImageManager::getManager()->getItemBoardFileName(mItemsVec.at(index).type);
		CCSprite* pSmallBg = CCSprite::create(biankuangName.c_str());
		bg->addChild(pSmallBg);
		pSmallBg->setPosition(sprite->getPosition());
	}
	return bg;
}

void MainLayerBeibaoItemList::refreshAllHeadMenu()
{
	CCMenu* menu = (CCMenu*)mBg->getChildByTag(TAG_HeadMenu);
	CCMenuItemImage* dj = (CCMenuItemImage*)menu->getChildByTag(TAG_HeadMenu_DaoJu);
	CCMenuItemImage* bs = (CCMenuItemImage*)menu->getChildByTag(TAG_HeadMenu_BaoShi);
	CCMenuItemImage* cl = (CCMenuItemImage*)menu->getChildByTag(TAG_HeadMenu_CaiLiao);
	CCMenuItemImage* zx = (CCMenuItemImage*)menu->getChildByTag(TAG_HeadMenu_ZaXiang);

	dj->initWithNormalImage(
		"daoju_dinglan_daoju_dengdai.png"
		, "daoju_dinglan_daoju_select.png"
		, "daoju_dinglan_daoju_select.png"
		, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));
	bs->initWithNormalImage(
		"daoju_dinglan_baoshi_dengdai.png"
		, "daoju_dinglan_baoshi_select.png"
		, "daoju_dinglan_baoshi_select.png"
		, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));
	cl->initWithNormalImage(
		"daoju_dinglan_cailiao_dengdai.png"
		, "daoju_dinglan_cailiao_select.png"
		, "daoju_dinglan_cailiao_select.png"
		, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));
	zx->initWithNormalImage(
		"daoju_dinglan_zaxiang_dengdai.png"
		, "daoju_dinglan_zaxiang_select.png"
		, "daoju_dinglan_zaxiang_select.png"
		, this, menu_selector(MainLayerBeibaoItemList::headItemClicked));

}

void MainLayerBeibaoItemList::makeSureClicked(CCObject* pSender)
{
	mFooterLayer->dismmisPopUp();
}

void MainLayerBeibaoItemList::cacelClicked(CCObject* pSender)
{
	mFooterLayer->dismmisPopUp();
}


void MainLayerBeibaoItemList::menuItemClicked_Da_Kai(CCObject *pSender){


	unsigned int DkId = (unsigned int)((CCMenuItemImage*)pSender)->getUserData();
    
	{
		addObserver(callfuncO_selector(MainLayerBeibaoItemList::rsp_DaikLibao),MSG_GetSystemGiftBagRsp);
		CmdHelper::getHelper()->cmdGetSystemGiftBag(giftbags[DkId].id);
	}

}


void MainLayerBeibaoItemList::rsp_DaikLibao(CCObject *msg_rsp){


	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_GetSystemGiftBagRsp);

	Flat_GetSysTemGiftBagRsp * data = (Flat_GetSysTemGiftBagRsp*)((Flat_MSG_Rsp*)msg_rsp)->getData();
	if (data->err_code != Success){

		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
    }

	//弹出所领取的东西
    AwardDisplay *layer = AwardDisplay::create(data);
    CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	giftbags.clear();
	giftbags =*SystemGiftBagManager::getManager()->getAllGiftBag();
	mTableView->reloadData();


	

}

void MainLayerBeibaoItemList::rsp_getLibao(CCObject *msg){

	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_QuerySystemGiftBagRsp);

	Flat_QueryGiftBagRsp *flat_data=(Flat_QueryGiftBagRsp*)((MSG_Rsp*)msg)->getData();

	if(flat_data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(flat_data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	giftbags =*SystemGiftBagManager::getManager()->getAllGiftBag();
	

}



