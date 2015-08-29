#include "FightBagLayer.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "font.h"
#include "MyselfManager.h"
#include "EquipmentXiangQingBody.h"
#include "MainLayerBase.h"
#include "FX_CommonFunc.h"
#include "FXScrollMenu.h"

using namespace FX_CommonFunc;

enum  
{
	Tag_MenuItem_ZhuangBei = 1,
	Tag_MenuItem_CaiLiao = 2,
	Tag_MenuItem_Fanhui = 3,

	Tag_Board,
	Tag_Table_EquipDiwen,
	Tag_Table_ItemImage,
	Tag_Table_FengYinImg,	//封印
	Tag_Cell_Menu,
	Tag_Cell_LabelName,
	Tag_Cell_LabelType,
	Tag_Cell_LabelBaiZhi,
	Tag_Cell_JunShangLabel,
	Tag_Cell_ShangHaiXiangXiLabel,
	Tag_Cell_ShangHaiXiangXiLabelNum,
	Tag_Cell_LevelLabel,
	Tag_Cell_BiaoJiImg,

	Tag_Cell_ShuXingLabel,
	Tag_Cell_CountLabel,
	Tag_Cell_CountLabelNum,

	Tag_EquipXiangQing_Layer,
};


FightBagLayer* FightBagLayer::create(vector<Equipment> equips, vector<Bag_WuPinInfo> wupin, BagCallBack *callback, bool isFooterPop)
{
	FightBagLayer *layer = new FightBagLayer;
	if(layer && layer->init(equips, wupin, callback, isFooterPop))
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
bool FightBagLayer::init(vector<Equipment> equips, vector<Bag_WuPinInfo> cailiao, BagCallBack *callback, bool isFooterPop)
{
	mEquips = equips;
	mWupins = cailiao;
	mCallback = callback;
	mCurStatus = Tag_MenuItem_ZhuangBei;
	m_isFooterPop = isFooterPop;

	mCallback->disableAllTouch();///////////////////////

	mBg = BaseSprite::create("beibao_bg.jpg");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));

	//menu
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(CCPointZero);
	//
	CCMenuItemImage *zhuangbeiItem = CCMenuItemImage::create(
		"zhandou_beibao_anniu_zhuangbei_select.png"
		, "zhandou_beibao_anniu_zhuangbei_anxia.png"
		, "zhandou_beibao_anniu_zhuangbei_dengdai.png"
		, this, menu_selector(FightBagLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(zhuangbeiItem, 1, Tag_MenuItem_ZhuangBei);
	zhuangbeiItem->setPosition(ccp(178, mBg->getContentSize().height-50));
	CCMenuItemImage *cailiaoItem = CCMenuItemImage::create(
		"zhandou_beibao_anniu_cailiao_select.png"
		, "zhandou_beibao_anniu_cailiao_anxia.png"
		, "zhandou_beibao_anniu_cailiao_dengdai.png"
		, this, menu_selector(FightBagLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(cailiaoItem, 1, Tag_MenuItem_CaiLiao);
	cailiaoItem->setPosition(ccp(462, zhuangbeiItem->getPositionY()));
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(FightBagLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(fanhui, 1, Tag_MenuItem_Fanhui);
	fanhui->setPosition(ccp(mBg->getContentSize().width/2, 50));

	//
	mTableView = CCTableView::create(this, CCSizeMake(614,635));
	mBg->addChild(mTableView);
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setPosition(ccp(14,100));
	mTableView->setDelegate(this);

//	menuItemClicked_MainMenu(zhuangbeiItem);

	return true;
}

void FightBagLayer::onEnter()
{
	BaseLayer::onEnter();

	menuItemClicked_MainMenu(mMainMenu->getChildByTag(Tag_MenuItem_ZhuangBei));
}

void FightBagLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	if (this->getChildByTag(Tag_EquipXiangQing_Layer) != NULL)
	{
		return;
	}

	int tag = ((CCMenuItemImage*)pSender)->getTag();

	CCMenuItemImage *zhuangbeiItem = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_ZhuangBei);
	CCMenuItemImage *cailiaoItem = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_CaiLiao);

	if(tag == Tag_MenuItem_ZhuangBei)
	{
		zhuangbeiItem->initWithNormalImage(
			"zhandou_beibao_anniu_zhuangbei_select.png"
			, "zhandou_beibao_anniu_zhuangbei_anxia.png"
			, "zhandou_beibao_anniu_zhuangbei_dengdai.png"
			, this, menu_selector(FightBagLayer::menuItemClicked_MainMenu));
		cailiaoItem->initWithNormalImage(
			"zhandou_beibao_anniu_cailiao_dengdai.png"
			, "zhandou_beibao_anniu_cailiao_anxia.png"
			, "zhandou_beibao_anniu_cailiao_dengdai.png"
			, this, menu_selector(FightBagLayer::menuItemClicked_MainMenu));
		mCurStatus = Tag_MenuItem_ZhuangBei;
	}
	else if(tag == Tag_MenuItem_CaiLiao)
	{
		zhuangbeiItem->initWithNormalImage(
			"zhandou_beibao_anniu_zhuangbei_dengdai.png"
			, "zhandou_beibao_anniu_zhuangbei_anxia.png"
			, "zhandou_beibao_anniu_zhuangbei_dengdai.png"
			, this, menu_selector(FightBagLayer::menuItemClicked_MainMenu));
		cailiaoItem->initWithNormalImage(
			"zhandou_beibao_anniu_cailiao_select.png"
			, "zhandou_beibao_anniu_cailiao_anxia.png"
			, "zhandou_beibao_anniu_cailiao_dengdai.png"
			, this, menu_selector(FightBagLayer::menuItemClicked_MainMenu));
		mCurStatus = Tag_MenuItem_CaiLiao;
	}
	else if(tag == Tag_MenuItem_Fanhui)
	{
		/////////////
		mCallback->enableAllTouch();
		if(m_isFooterPop)
			mFooterLayer->dismmisPopUp();
		else 
			this->removeFromParent();
		return;
	}

	mTableView->reloadData();
}

CCSize FightBagLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	if(mCurStatus == Tag_MenuItem_CaiLiao)
		return CCSize(613, 145);
	else 
		return CCSize(606, 219);
}

unsigned int FightBagLayer::numberOfCellsInTableView(CCTableView *table)
{
	if(mCurStatus == Tag_MenuItem_ZhuangBei)
		return mEquips.size();
	else 
		return mWupins.size();
}

CCTableViewCell* FightBagLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
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

	if (table != mTableView) return cell;

	if(mCurStatus == Tag_MenuItem_ZhuangBei)
		cell->addChild(getEquipCellBody(idx), 1, Tag_Board);
	else 
		cell->addChild(getWupinCellBody(idx), 1, Tag_Board);

	CCSize size = tableCellSizeForIndex(table, idx);
	cell->getChildByTag(Tag_Board)->setPosition(ccp(size.width/2, size.height/2));


	return cell;
}

BaseSprite * FightBagLayer::getEquipCellBody(int idx)
{
	BaseSprite *bg = BaseSprite::create("mianban_zhuangbei_xiangxi.png");
	CCPoint picPos = ccp(75.0f, bg->getContentSize().height/2);

	{
		ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_List(
			mEquips.at(idx).getEquipColor(),
			false,
			mEquips.at(idx).isPartOfSuit(),
			mEquips.at(idx).haveZhuanke());
		BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
		diWen->setScale(diWenData.scale);
		diWen->setPosition(picPos);
		bg->addChild(diWen,0,Tag_Table_EquipDiwen);
	
		{
			BaseSprite *sprite = ImageManager::getManager()->getSpriteOfZhuangbei(&mEquips.at(idx));
			//sprite->setScale(data.scale);
			sprite->setScale(0.43f);
			sprite->setPosition(picPos);
			bg->addChild(sprite,0,Tag_Table_ItemImage);
		}
		if (mEquips.at(idx).isInFengYin())
		{
			BaseSprite *fyImg = BaseSprite::create("shenbing_fengyin_suo.png");
			bg->addChild(fyImg, 0, Tag_Table_FengYinImg);
			fyImg->setPosition(diWen->getPosition());
			fyImg->setScale(diWen->getContentSize().width*diWen->getScale() / fyImg->getContentSize().width);
		}
	}



	// 添加图片识别区
	{
		ImageData selecedData = ImageManager::getManager()->getImageData_DiWen_Selected(
			mEquips.at(idx).getEquipColor(),
			false,//mEquips.at(idx).isShenBing(),
			mEquips.at(idx).isPartOfSuit());
		BaseSprite* menuItemSel = BaseSprite::create(selecedData.getCurrentImageFile());
		menuItemSel->setScale(selecedData.scale);
		CCSprite* menuRect = CCSprite::createWithTexture(menuItemSel->getTexture());
		menuRect->setScale(0.0f);

		FXScrollMenu * menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		bg->addChild(menu,0,Tag_Cell_Menu);
		menu->setPosition(ccp(0,0));
		CCMenuItemSprite* t = CCMenuItemSprite::create(menuRect,menuItemSel,this,SEL_MenuHandler(&FightBagLayer::menuItemClicked_ItemImg));
		menu->addChild(t, 0, idx);
		t->setPosition(picPos);

		//降低menu优先级
//		changeMenuHandlerPriority(menu, false);
	}

	float left_x_start = 160;
	//名称
	{
		CCLabelTTF* label;
		char str[64];
		if (mEquips.at(idx).getEquipData().qianghua_data.qianghua_count != 0)
		{
			//sprintf(str, "%s+%d", mEquipmentVec[index]->getName().c_str(), mEquipmentVec[index]->getEquipData().qianghua_time);
			sprintf(str, "%s", mEquips.at(idx).getName().c_str());
		}
		else
		{
			sprintf(str, "%s", mEquips.at(idx).getName().c_str());
		}

		label = LF::lable(bg,str,ccp(left_x_start,190),28,fontStr_kaiti);
		label->setTag(Tag_Cell_LabelName);

		ccColor3B color = ImageManager::getManager()->getShowColor(mEquips.at(idx).getEquipColor(),false,mEquips.at(idx).isPartOfSuit());
		label->setColor(color);
	}

	//类型
	{
		std::string typeStr = 
			CS::getZhuangbeiTypeName(mEquips.at(idx).getType(), mEquips.at(idx).getEquipColor());

		CCLabelTTF* label;
		label = LF::lable(bg,typeStr,ccp(left_x_start,165),16,fontStr_kaiti);
		label->setTag(Tag_Cell_LabelType);

		ccColor3B color = ImageManager::getManager()->getShowColor(mEquips.at(idx).getEquipColor(),false,false);
	}

	//白字
	{
		CCLabelTTF* label_baizhi;
		bool hasBaiZhi = false;
		float baizi = 0;
		{
			switch(mEquips.at(idx).getType())
			{
			case ZhuangbeiType_DanshouJian:
			case ZhuangbeiType_ShuangshouBishou:
			case ZhuangbeiType_ShuangshouFu:
			case ZhuangbeiType_DanshouFazhang:
			case ZhuangbeiType_ShuangshouFazhang:
			case ZhuangbeiType_Gong:
				hasBaiZhi = true;
				baizi = mEquips.at(idx).getGongjiLiAfterQianghua(false);
				break;
			case ZhuangbeiType_Dunpai:
			case ZhuangbeiType_KuijiaZhong:
			case ZhuangbeiType_KuijiaQin:
				hasBaiZhi = true;
				baizi = mEquips.at(idx).getHuJiaZhiAfterQianghua();
				break;
			default:
				break;
			}
		}
		if (hasBaiZhi)
		{
			char str[64];
			sprintf(str, "%.1f", baizi);

			label_baizhi = LF::lable(bg,str,ccp(left_x_start,120),60,fontStr_BookAntiqua);
			label_baizhi->setTag(Tag_Cell_LabelBaiZhi);
		}
		else
		{
			label_baizhi = LF::lable(bg,"",ccp(left_x_start,120),60,fontStr_BookAntiqua);
			label_baizhi->setTag(Tag_Cell_LabelBaiZhi);
		}

		//junshang
		CCLabelTTF* label_junshang = LF::lable(bg,LFStrings::getValue("junshang"),ccp(0,0),20,fontStr_kaiti);
		ccColor3B color;
		color.r = 155;
		color.g = 155;
		color.b = 155;
		label_junshang->setColor(color);
		label_junshang->setAnchorPoint(ccp(0, 0.5));
		label_junshang->setPosition(
			ccp(label_baizhi->getPositionX()+label_baizhi->getContentSize().width+10, 110));
		label_junshang->setTag(Tag_Cell_JunShangLabel);
		if(hasBaiZhi)
		{
			label_junshang->setVisible(true);
			if(mEquips.at(idx).getEquipType() == EquipType_FangJu)
				label_junshang->setString(LFStrings::getValue("huJia").c_str());
		}
		else
		{
			label_junshang->setVisible(false);
		}
	}


	//伤害详细 
	if (mEquips.at(idx).getEquipType() == EquipType_WuQi)
	{
		CCLabelTTF* label;
		char str[64];
		sprintf(str, " %u-%u ",mEquips.at(idx).getEquipData().gongji_min
			, mEquips.at(idx).getEquipData().gongji_max);

		label = LF::lable(bg,str,ccp(left_x_start,75),18,fontStr_kaiti);
		label->setTag(Tag_Cell_ShangHaiXiangXiLabel);

		CCLabelTTF* label2;
		label2 = LF::lable(bg,LFStrings::getValue("dianShangHai")
			,ccp(label->getPosition().x + label->getContentSize().width, label->getPositionY()),20,fontStr_kaiti);
		label2->setTag(Tag_Cell_ShangHaiXiangXiLabelNum);
	}
	else
	{
		CCLabelTTF* label;

		label = LF::lable(bg,"",ccp(left_x_start,75),18,fontStr_kaiti);
		label->setTag(Tag_Cell_ShangHaiXiangXiLabel);

		CCLabelTTF* label2;
		label2 = LF::lable(bg,""
			,ccp(label->getPosition().x + label->getContentSize().width, label->getPositionY()),20,fontStr_kaiti);
		label2->setTag(Tag_Cell_ShangHaiXiangXiLabelNum);
	}

	//等级
	{
		CCLabelTTF* label;
		char str[64];
		sprintf(str, "%s %u ", LFStrings::getValue("dengji").c_str(), mEquips.at(idx).getEquipData().zhuangbei_level);
		label = LF::lable(bg,str,ccp(left_x_start,28),18,fontStr_kaiti);
		ccColor3B color;
		color.r = 247;
		color.g = 234;
		color.b = 190;
		label->setColor(color);
		label->setTag(Tag_Cell_LevelLabel);
	}
	//魔法属性
	{
		Equipment* equip = &mEquips.at(idx);
		std::vector<std::string> vec = equip->getAllMofaShuxing();

		//大于5个  第五个就是 ......
		for(int i=0; i<vec.size(); ++i)
		{
			std::string txt;

			if(i==4 && vec.size()>5)
			{
				txt = "....";
			}
			else
			{
				txt = vec[i];
			}

			BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
			bg->addChild(biaoji, 0, Tag_Cell_BiaoJiImg+i);
			biaoji->setPosition(ccp(430, 170-i*28));
			biaoji->setOpacity(80);

			CCLabelTTF* label;
			label = LF::lable(bg,txt,ccp(0,0),20,fontStr_kaiti);
			ccColor3B color;
			color.r = 79;
			color.g = 110;
			color.b = 217;
			label->setColor(color);
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(430+15, 170-i*28));
			label->setTag(i);

			if(i==4 && vec.size()>5)
			{
				break;
			}
		}
	}

	return bg;
}

BaseSprite * FightBagLayer::getWupinCellBody(int idx)
{
	BaseSprite *bg = BaseSprite::create("shangdian_shangpin_bg.png");

	unsigned int item_type = mWupins.at(idx).type;

	//item图片
	{
		BaseSprite *sprite = BaseSprite::create(ImageManager::getManager()->getItemFilename(item_type));
		sprite->setPosition(ccp(72, bg->getContentSize().height/2));
		bg->addChild(sprite,0,Tag_Table_ItemImage);
		if (mWupins.at(idx).isSuiPian)
		{
			BaseSprite *suipian = BaseSprite::create("zhuangbei_sphc_juxing.png");
			sprite->addChild(suipian);
			suipian->setPosition(ccp(sprite->getContentSize().width/2, sprite->getContentSize().height/2));
		}
	}
	// 名字
	{
		string name ;
		if(item_type > ItemsType_SkillBook_MinId && item_type < ItemsType_SkillBook_MaxId)
			name = LFStrings::getValue("jinengshu")
			+ SkillTree::sharedSkillTree()->getSkillNameByType((SkillType)(item_type-ItemsType_SkillBook_MinId));
		else 
			name = CS::getItemName(item_type);
		if (mWupins.at(idx).isSuiPian)
			name += LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(mWupins.at(idx).suipian_id);
		CCLabelTTF* lable = LF::lable(bg,name,ccp(142,116),23,fontStr_kaiti); 
		lable->setTag(Tag_Cell_LabelName);
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
		lable->setPosition(ccp(142, 70));
		lable->setTag(Tag_Cell_ShuXingLabel);
		lable->setColor(ccc3(201, 156, 18));
	}
	//数量
	{
		CCLabelTTF* lable = LF::lable(bg, (LFStrings::getValue("shuliang")+": ").c_str(),ccp(142,32),22,fontStr_kaiti);
		lable->setTag(Tag_Cell_CountLabel);
		ccColor3B color;
		color.r = 89;
		color.g = 111;
		color.b = 183;
		lable->setColor(color);
	}
	//数量数值
	{
		char str[32];
		sprintf(str, "%d",mWupins.at(idx).count);
		CCLabelTTF* lable = LF::lable(bg, str,ccp(202,32),20,fontStr);
		lable->setTag(Tag_Cell_CountLabelNum);
	}

	return bg;
}

void FightBagLayer::menuItemClicked_ItemImg(CCObject *pSender)
{
	if (this->getChildByTag(Tag_EquipXiangQing_Layer) != NULL)
	{
		return;
	}

	int tag = ((CCMenuItem*)pSender)->getTag();
	if(mCurStatus == Tag_MenuItem_ZhuangBei)
	{
		EquipmentXiangQingBody* q = new EquipmentXiangQingBody(&mEquips.at(tag), mEquips.at(tag).getActorId(), From_SpecialScene);
		this->addChild(q, 100, Tag_EquipXiangQing_Layer);
		q->setPosition(ccp(this->getBodyCenterPos().x-q->getBodyCenterPos().x, this->getBodyCenterPos().y-q->getBodyCenterPos().y));	
		q->release();
	}
}