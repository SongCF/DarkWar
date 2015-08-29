#include "ShenBingShengJi.h"
#include "EquipmentManager.h"
#include "ImageManager.h"
#include "font.h"
#include "CS_Common.h"
#include "WuPinManager.h"
#include "flat.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "ShenBingPool.h"
#include "FX_CommonFunc.h"
#include "GuideLayerBase.h"
#include "History.h"

using namespace FX_CommonFunc;

enum 
{
	Tag_MenuItem_LevUp = 1,
	Tag_MenuItem_GoBack,

	Tag_Img_EquipAndDiWen = 10,
	Tag_Label_ZhuangBeiName,
	Tag_Img_ShengJiShuoMing,

	Tag_Board_CurProperty,
	Tag_Board_NextPproperty,


	Tag_Dialog_UpEndWindow = 109,
};

ShenBingShengJi::ShenBingShengJi(Equipment *eq)
{
	mCurEquip = eq;

	mBg = BaseSprite::create("zhuangbei_xiangxi_shengji_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));
	CCSize bgSize = mBg->getContentSize();
	//
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(ccp(0,0));
	{
		CCMenuItemImage *lvupItem = CCMenuItemImage::create(
			"zhuangbei_xiangxi_shengji.png",
			"zhuangbei_xiangxi_shengji_select.png",
			this, menu_selector(ShenBingShengJi::menuItemClicked_LevUp));
		mMainMenu->addChild(lvupItem, 0, Tag_MenuItem_LevUp);
		lvupItem->setPosition(ccp(320, bgSize.height-793));
		CCMenuItemImage *backItem = CCMenuItemImage::create(
			"close.png",
			"close_select.png",
			this, menu_selector(ShenBingShengJi::menuItemClicked_GoBack));
		mMainMenu->addChild(backItem, 0, Tag_MenuItem_GoBack);
		backItem->setPosition(ccp(mBg->getPositionX()+mBg->getContentSize().width/2-backItem->getContentSize().width/2 - 25,800));
	}

	//equip
	{
		ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			mCurEquip->getEquipColor(),
			false,//mCurEquip->isShenBing(),
			mCurEquip->isPartOfSuit(),
			mCurEquip->haveZhuanke());
		BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
		mBg->addChild(diwenImg, 1, Tag_Img_EquipAndDiWen);
		diwenImg->setPosition(ccp(320, bgSize.height-241));
		
		//zhuangbei
		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurEquip);
		diwenImg->addChild(eqImg);
		eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
		diwenImg->setScale(0.50);
		//name
		currentzb=mBg;
		

		CCLabelTTF *eqName = CCLabelTTF::create(mCurEquip->getName().c_str(), fontStr_kaiti, 30);
		mBg->addChild(eqName, 0, Tag_Label_ZhuangBeiName);
		eqName->setColor(ImageManager::getManager()->getShowColor(mCurEquip->getEquipColor(), false, mCurEquip->isPartOfSuit()));
		eqName->setPosition(ccp(bgSize.width/2, 810));
	}

	//todo
	int nextLv = CS::getNextShenBingLevel(mCurEquip->getLevel()); // -1则为满级
	//如果是顶级
	if (false)
	{
		showPropertyBoard();
		mMainMenu->getChildByTag(Tag_MenuItem_LevUp)->setVisible(false);
		mMainMenu->getChildByTag(Tag_MenuItem_GoBack)->setPositionX(mBg->getContentSize().width/2);
	}
	//如果不是顶级
	else 
	{
		showPropertyBoard();
		showPropertyBoard(false);
		showNeedMaterial();
	}
	//Qianhuadh(currentzb,1);
}

void ShenBingShengJi::menuItemClicked_GoBack(CCObject *pSender)
{
	mFooterLayer->dismmisPopUp();
}

void ShenBingShengJi::menuItemClicked_LevUp(CCObject *pSender)
{
	mEqOldLevel = mCurEquip->getLevel();
	
	if (mErrorInfo.empty())
	{
		vector<Equipment*> canUsedEqVec;
		int needEquip = mCurEquip->getEquipmentsCanUsedToLevelUpShengBing(canUsedEqVec);
		if (needEquip != STATE_Not_Need_Equip && canUsedEqVec.size() >= 1)
		{
			
			addObserver(callfuncO_selector(ShenBingShengJi::rsp_levUp), MSG_upgradeShenbingRsp_F);
			mCmdHlp->cmdUpgradeShenbing(mCurEquip->getEquipId(), canUsedEqVec.at(0)->getEquipId());
		}
		else if (needEquip == STATE_Not_Need_Equip)
		{
			
			addObserver(callfuncO_selector(ShenBingShengJi::rsp_levUp), MSG_upgradeShenbingRsp_F);
			mCmdHlp->cmdUpgradeShenbing(mCurEquip->getEquipId(), 0);
		}
	}
	else 
	{
		
		MessageDisplay *layer = MessageDisplay::create(mErrorInfo);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
}

void ShenBingShengJi::menuItemClicked_UpEndDialog(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Dialog_UpEndWindow);

	clearBg();
	showPropertyBoard();
	showPropertyBoard(false);
	showNeedMaterial();

	mMainMenu->setVisible(true);
}

void ShenBingShengJi::clearBg()
{
	mMainMenu->retain();
	mBg->removeAllChildren();
	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(ccp(0,0));
	mMainMenu->release();

	mErrorInfo.clear();

	CCSize bgSize = mBg->getContentSize();
	//equip
	{
		ImageData data = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			mCurEquip->getEquipColor(),
			false,//mCurEquip->isShenBing(),
			mCurEquip->isPartOfSuit(),
			mCurEquip->haveZhuanke());
		BaseSprite *diwenImg = BaseSprite::create(data.getCurrentImageFile());
		mBg->addChild(diwenImg, 1, Tag_Img_EquipAndDiWen);
		diwenImg->setPosition(ccp(320, bgSize.height-241));


		//zhuangbei
		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurEquip);
		diwenImg->addChild(eqImg);
		eqImg->setPosition(ccp(diwenImg->getContentSize().width/2, diwenImg->getContentSize().height/2));
		diwenImg->setScale(0.50);
		//name
		CCLabelTTF *eqName = CCLabelTTF::create(mCurEquip->getName().c_str(), fontStr_kaiti, 30);
		mBg->addChild(eqName, 0, Tag_Label_ZhuangBeiName);
		eqName->setColor(ImageManager::getManager()->getShowColor(mCurEquip->getEquipColor(), false, mCurEquip->isPartOfSuit()));
		eqName->setPosition(ccp(bgSize.width/2, 810));
	}
}

void ShenBingShengJi::showPropertyBoard(bool isCur)
{
	int Label_Size_small = 20;

	int nextLv = CS::getNextShenBingLevel(mCurEquip->getLevel()); //-1就是满级了

	CCString temp;
	Flat_DuanzaoGroupZBInfo shenBingData;
	BaseSprite *board = NULL;
	Equipment* tEq = NULL;
	if (! isCur)//下一级
	{
		mBg->removeChildByTag(Tag_Board_NextPproperty);

		if (nextLv < 0) return;   //显示下一等级的属性， 如果当前是满级了就不能再显示了
		board = BaseSprite::create("zhuangbei_xiangxi_shengji_shuxing_bg.png");
		mBg->addChild(board, 0, Tag_Board_NextPproperty);
		board->setPosition(ccp(520, mBg->getContentSize().height-244));

		temp.initWithFormat("%s%s%s(%d%s)", LFStrings::getValue("xia").c_str(), LFStrings::getValue("yi").c_str(), 
			LFStrings::getValue("dengji").c_str(), nextLv, LFStrings::getValue("ji").c_str());

		if (! ChuanqiPool::getPool()->getChuanQiData(mCurEquip->getGroupId(), shenBingData, nextLv))
			CCLog("Error:%s --->[have no shenbing data]", __FUNCTION__);

		EquipData tempEqData = mCurEquip->getEquipData();
		tempEqData.gongji_min = (shenBingData.gongji_min_max+shenBingData.gongji_min_min)/2;
		tempEqData.gongji_max = (shenBingData.gongji_max_max+shenBingData.gongji_max_min)/2;
		tempEqData.hujia = (shenBingData.wufang_min+shenBingData.wufang_max)/2;

		tempEqData.attr_vec.clear();
		for (int i=0,count=shenBingData.itemVec.size(); i<count; i++)
		{
			vector<ZBAttr> temp = shenBingData.itemVec[i].getZbAttrsIgnoreGaiLvAndCount();
			tempEqData.attr_vec.insert(tempEqData.attr_vec.end(),temp.begin(),temp.end());
		}
		tEq = new Equipment(tempEqData);
	}
	else//当前
	{
		mBg->removeChildByTag(Tag_Board_CurProperty);

		board = BaseSprite::create("zhuangbei_xiangxi_shengji_shuxing_bg.png");
		mBg->addChild(board, 0, Tag_Board_CurProperty);
		//如果满级 位置不同
		if (nextLv > 0)
		{
			board->setPosition(ccp(120, mBg->getContentSize().height-244));
		}
		else
		{
			board->setPosition(ccp(320, mBg->getContentSize().height-396-board->getContentSize().height/2));
		}

		temp.initWithFormat("%s(%u%s)", LFStrings::getValue("DangQian").c_str(), mCurEquip->getLevel(), LFStrings::getValue("ji").c_str());

		if (! ChuanqiPool::getPool()->getChuanQiData(mCurEquip->getGroupId(), shenBingData, mCurEquip->getLevel()))
			CCLog("Error:%s --->[have no shenbing data]", __FUNCTION__);

		EquipData tempEqData = mCurEquip->getEquipData();
		tempEqData.gongji_min = (shenBingData.gongji_min_max+shenBingData.gongji_min_min)/2;
		tempEqData.gongji_max = (shenBingData.gongji_max_max+shenBingData.gongji_max_min)/2;
		tempEqData.hujia = (shenBingData.wufang_min+shenBingData.wufang_max)/2;

		tempEqData.attr_vec.clear();
		for (int i=0,count=shenBingData.itemVec.size(); i<count; i++)
		{
			vector<ZBAttr> temp = shenBingData.itemVec[i].getZbAttrsIgnoreGaiLvAndCount();
			tempEqData.attr_vec.insert(tempEqData.attr_vec.end(),temp.begin(),temp.end());
		}
		tEq = new Equipment(tempEqData);
	}

	int start_x = 10, pos_y = board->getContentSize().height-10, gap = 6;
	//当前（x级）    下一等级（x级）
	{
		CCLabelTTF *curLev = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 24);
		board->addChild(curLev);
		curLev->setAnchorPoint(ccp(0, 1));
		curLev->setPosition(ccp(start_x, pos_y));
		curLev->setColor(ImageManager::getManager()->getShowColor(mCurEquip->getEquipColor(), false, mCurEquip->isPartOfSuit()));
		pos_y -= curLev->getContentSize().height + gap;
	}
	//白字
	{
		CCLabelTTF* label;
		float baizi = 0;
		bool hasBaiZhi = false;
		switch(mCurEquip->getType())
		{
		case ZhuangbeiType_DanshouJian:
		case ZhuangbeiType_ShuangshouBishou:
		case ZhuangbeiType_ShuangshouFu:
		case ZhuangbeiType_DanshouFazhang:
		case ZhuangbeiType_ShuangshouFazhang:
		case ZhuangbeiType_Gong:
			hasBaiZhi = true;
			baizi = tEq->getGongjiLiAfterQianghua(false);
			break;
		case ZhuangbeiType_Dunpai:
		case ZhuangbeiType_KuijiaZhong:
		case ZhuangbeiType_KuijiaQin:
			hasBaiZhi = true;
			baizi = tEq->getHuJiaZhiAfterQianghua();
			break;
		case ZhuangbeiType_Jiezhi:
		case ZhuangbeiType_Hushenfu:
		case ZhuangbeiType_Faqi:
		default:
			break;
		}
		if (hasBaiZhi)
		{
			char str[64];
			sprintf(str, "%.1f", baizi);
			label = CCLabelTTF::create(str, fontStr_BookAntiqua, 70);
			label->setAnchorPoint(ccp(0, 1));
			label->setPosition(ccp(start_x, pos_y));
			board->addChild(label);

			pos_y -= label->getContentSize().height + gap;
		}
	}
	//伤害详细
	if (mCurEquip->getEquipType() == EquipType_WuQi)
	{
		char str[64];
		sprintf(str, "%u-%u ", tEq->getEquipData().gongji_min, tEq->getEquipData().gongji_max);
		CCLabelTTF* label = CCLabelTTF::create(str, fontStr_BookAntiqua, Label_Size_small);
		board->addChild(label);
		label->setAnchorPoint(ccp(0, 1));
		label->setPosition(ccp(start_x, pos_y));

		CCLabelTTF* label2 = CCLabelTTF::create(LFStrings::getValue("dianShangHai").c_str(), fontStr_kaiti, Label_Size_small);
		board->addChild(label2);
		label2->setColor(ccc3(100,98,97));
		label2->setAnchorPoint(ccp(0, 1));
		label2->setPosition(ccp(label->getPosition().x + label->getContentSize().width, label->getPositionY()));

		pos_y -= label->getContentSize().height + gap;
	}
	//魔法属性
	{
		vector<string> infoVec = tEq->getAllMofaShuxing(false);
		for(int i=0; i<infoVec.size(); ++i)
		{
			CCLabelTTF* label = CCLabelTTF::create(infoVec.at(i).c_str(), fontStr_kaiti, Label_Size_small);
			board->addChild(label);
			label->setColor(ccc3(79, 110, 217));
			label->setAnchorPoint(ccp(0, 1));
			label->setPosition(ccp(start_x, pos_y));

			pos_y -= label->getContentSize().height + gap;
		}
	}

	delete tEq;
}

void ShenBingShengJi::showNeedMaterial()
{
	int nextLv = CS::getNextShenBingLevel(mCurEquip->getLevel());
	if (nextLv < 0) return;

	char buf[60];

	mErrorInfo.clear();

	BaseSprite *title = BaseSprite::create("zhuangbei_xiangxi_shengji_wenzi.png");
	mBg->addChild(title, 0, Tag_Img_ShengJiShuoMing);
	title->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-452));
	//y546   x240 614
// #define STATE_Ok				0
// #define STATE_Not_Need_Equip	1
// #define STATE_Not_Found_Equip	2
	vector<Equipment*> canUsedEqVec;
	int needEquip = mCurEquip->getEquipmentsCanUsedToLevelUpShengBing(canUsedEqVec);
	//need equip
	if (needEquip != STATE_Not_Need_Equip)
	{
		BaseSprite *eqKuang = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_WithBianKuang(
			mCurEquip->getEquipColor(), 
			false,//mCurEquip->isShenBing(),
			mCurEquip->isPartOfSuit()));
		mBg->addChild(eqKuang);
		eqKuang->setPosition(ccp(240, mBg->getContentSize().height-546));
		//equip img
		BaseSprite *eqImg = ImageManager::getManager()->getSpriteOfZhuangbei(mCurEquip);
		eqKuang->addChild(eqImg);
		eqImg->setScale(0.23f);
		eqImg->setPosition(ccp(eqKuang->getContentSize().width/2, eqKuang->getContentSize().height/2));
		//equi name
		CCLabelTTF *eqNameLabel = CCLabelTTF::create(mCurEquip->getName().c_str(), fontStr_kaiti, 20);
		mBg->addChild(eqNameLabel);
		eqNameLabel->setPosition(ccp(eqKuang->getPositionX(), mBg->getContentSize().height-632));
		eqNameLabel->setColor(ImageManager::getManager()->getShowColor(mCurEquip->getEquipColor(), false, mCurEquip->isPartOfSuit()));
		//num
		CCLabelTTF *eqNeedLabel = CCLabelTTF::create("1/", fontStr_BookAntiqua, 20);
		eqNameLabel->addChild(eqNeedLabel);
		eqNeedLabel->setAnchorPoint(ccp(1, 0.5f));
		eqNeedLabel->setPosition(ccp(eqNameLabel->getContentSize().width/2, -eqNeedLabel->getContentSize().height));
		int allEqNeed = canUsedEqVec.size();
		sprintf(buf, "%d", allEqNeed);
		CCLabelTTF *eqNeedAllLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
		eqNeedAllLabel->setAnchorPoint(ccp(0, 0.5f));
		eqNameLabel->addChild(eqNeedAllLabel);
		eqNeedAllLabel->setPosition(eqNeedLabel->getPosition());
		if (allEqNeed < 1)
		{
			eqNeedAllLabel->setColor(ccRED);
			mErrorInfo = LFStrings::getValue("suoxu") + LFStrings::getValue("cailiaobuzu");
		}
	}

	//need material
	BaseSprite *itemKuang = BaseSprite::create("daoju_kuang_bg.png");
	mBg->addChild(itemKuang);
	if (needEquip != STATE_Not_Need_Equip)
		itemKuang->setPosition(ccp(396, mBg->getContentSize().height-546));
	else 
		itemKuang->setPosition(ccp(320, mBg->getContentSize().height-546));
	BaseSprite *itemImg = BaseSprite::create(ImageManager::getManager()->getItemFilename(ItemsType_Shenbing_Jianding));
	itemKuang->addChild(itemImg);
	itemImg->setPosition(ccp(itemKuang->getContentSize().width/2, itemKuang->getContentSize().height/2));
	//name
	CCLabelTTF *clNameLabel = CCLabelTTF::create(CS::getItemName(ItemsType_Shenbing_Jianding).c_str(), fontStr_kaiti, 20);
	mBg->addChild(clNameLabel);
	clNameLabel->setPosition(ccp(itemKuang->getPositionX(), mBg->getContentSize().height-632));
	//num
	int needItemNum = CS::ShenbingUpgradeNeedItemCount(mCurEquip->getLevel());//此处参数传当前等级
	sprintf(buf, "%u/", needItemNum);  
	CCLabelTTF *clNeedLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
	clNameLabel->addChild(clNeedLabel);
	clNeedLabel->setAnchorPoint(ccp(1, 0.5f));
	clNeedLabel->setPosition(ccp(clNameLabel->getContentSize().width/2, -clNeedLabel->getContentSize().height));
	int allItemNeed = WuPinManager::getManager()->getWuPin(ItemsType_Shenbing_Jianding)->getCount();
	sprintf(buf, "%d", allItemNeed);
	CCLabelTTF *allItemLabelNum = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
	clNameLabel->addChild(allItemLabelNum);
	allItemLabelNum->setAnchorPoint(ccp(0, 0.5f));
	allItemLabelNum->setPosition(clNeedLabel->getPosition());
	if (allItemNeed < needItemNum)
	{
		allItemLabelNum->setColor(ccRED);
		mErrorInfo = LFStrings::getValue("suoxu") + LFStrings::getValue("cailiaobuzu");
	}

	//need silver
	BaseSprite *silverIcon = BaseSprite::create("yinbi_tubiao.png");
	mBg->addChild(silverIcon);
	unsigned int silver_need = CS::ShenbingUpgradeNeedSilver(mCurEquip->getLevel());//此处要传当前等级
	unsigned int silver_total = WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount();
	sprintf(buf, " %d/", silver_need);
	CCLabelTTF *yinbiLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
	silverIcon->addChild(yinbiLabel);
	yinbiLabel->setAnchorPoint(ccp(1, 0.5f));
	yinbiLabel->setPosition(ccp(silverIcon->getContentSize().width+yinbiLabel->getContentSize().width, silverIcon->getContentSize().height/2));
	sprintf(buf, "%d", silver_total);
	CCLabelTTF *yinbiAllLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
	silverIcon->addChild(yinbiAllLabel);
	yinbiAllLabel->setAnchorPoint(ccp(0, 0.5f));
	yinbiAllLabel->setPosition(yinbiLabel->getPosition());
	silverIcon->setPosition(ccp(mBg->getContentSize().width/2-(yinbiLabel->getContentSize().width+yinbiAllLabel->getContentSize().width)/2
		, mBg->getContentSize().height-716));
	if(silver_total < silver_need)
	{
		yinbiAllLabel->setColor(ccRED);
		mErrorInfo = LFStrings::getValue("suoxu") + LFStrings::getValue("yinbibuzu");
	}
}


void ShenBingShengJi::showLevUpEndDialog()
{
	BaseSprite *dialog = BaseSprite::create("gongming_bg.png");
	CCDirector::sharedDirector()->getRunningScene()->addChild(dialog, 1000, Tag_Dialog_UpEndWindow);
	dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));

	CCMenu *menu = CCMenu::create();
	dialog->addChild(menu);
	menu->setPosition(ccp(0,0));
	CCMenuItemImage *item = CCMenuItemImage::create(
		"gongming_bg.png",
		"gongming_bg.png",
		this, menu_selector(ShenBingShengJi::menuItemClicked_UpEndDialog));
	menu->addChild(item);
	item->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
	item->setScaleX(mWinSize.width / item->getContentSize().width);
	item->setScaleY(mWinSize.height / item->getContentSize().height);
	item->setOpacity(0);

	mMainMenu->setVisible(false);

	CCString temp;
	//升级成功label
	BaseSprite *bar = BaseSprite::create("shangpin_goumai_tishikuang_tiao.png");
	dialog->addChild(bar);
	//content
	//如果是满级
	int nextLv = CS::getNextShenBingLevel(mCurEquip->getLevel());
	if (nextLv < 0) //满级
	{
		BaseSprite *title = BaseSprite::create("zhuangbei_xiangxi_shengji_max.png");
		dialog->addChild(title);
		title->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/3*2 + 15));
		bar->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));

		CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("GaiWuQiYiShiDingJi").c_str(), fontStr_kaiti, 20);
		dialog->addChild(tishiLabel);
		tishiLabel->setPosition(ccp(bar->getPositionX(), bar->getPositionY() - bar->getContentSize().height - tishiLabel->getContentSize().height));
	}
	else 
	//如果不是满级
	{
		temp.initWithFormat("%s%s!", LFStrings::getValue("ShengJi").c_str(), LFStrings::getValue("chenggong").c_str());
		CCLabelTTF *titleLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 24);
		dialog->addChild(titleLabel);
		titleLabel->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/3*2));
		bar->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2-bar->getContentSize().height/2));
	}

	//成功将
	temp.initWithFormat("%s%s", LFStrings::getValue("chenggong").c_str(), LFStrings::getValue("jiang").c_str());
	CCLabelTTF *label1 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 22);
	bar->addChild(label1);
	//武器名20级(武器颜色)
	temp.initWithFormat("%s%u%s", mCurEquip->getName().c_str(), mEqOldLevel, LFStrings::getValue("ji").c_str());
	CCLabelTTF *oldName = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
	oldName->setColor(ImageManager::getManager()->getShowColor(mCurEquip->getEquipColor(), false, mCurEquip->isPartOfSuit()));
	label1->addChild(oldName);
	oldName->setPosition(ccp(label1->getContentSize().width+oldName->getContentSize().width/2, label1->getContentSize().height/2));
	//升至
	CCLabelTTF *szLabel = CCLabelTTF::create(LFStrings::getValue("ShengZhi").c_str(), fontStr_kaiti, 22);
	oldName->addChild(szLabel);
	szLabel->setPosition(ccp(oldName->getContentSize().width+szLabel->getContentSize().width/2, oldName->getContentSize().height/2));
	//武器名30级(武器颜色)
	temp.initWithFormat("%s%u%s", mCurEquip->getName().c_str(), mCurEquip->getLevel(), LFStrings::getValue("ji").c_str());
	CCLabelTTF *newName = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
	newName->setColor(ImageManager::getManager()->getShowColor(mCurEquip->getEquipColor(), false, mCurEquip->isPartOfSuit()));
	szLabel->addChild(newName);
	newName->setPosition(ccp(szLabel->getContentSize().width+newName->getContentSize().width/2, szLabel->getContentSize().height/2));
	//offset
	label1->setAnchorPoint(ccp(0, 0.5f));
	label1->setPosition(ccp(bar->getContentSize().width/2
		-(label1->getContentSize().width+oldName->getContentSize().width+szLabel->getContentSize().width+newName->getContentSize().width)/2
		, bar->getContentSize().height/2));
}

void ShenBingShengJi::rsp_levUp(CCObject *msg_f)
{
	CCLOG("level--->");
	removeObserver(MSG_upgradeShenbingRsp_F);
	GameDirector::getDirector()->hideWaiting();
	Flat_UpgradeShenbingRsp* data = (Flat_UpgradeShenbingRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	
	clearBg();
	Qianhuadh(currentzb,1);
	showPropertyBoard();
	showLevUpEndDialog();
	
}

void ShenBingShengJi::onEnter()
{
	BaseLayer::onEnter();

	//教学
	//第一次在面板看到传奇装备

		bool firstDisplay_ChuanQiEquip = false;
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstDisplayChuanQiEquip))
		{
			firstDisplay_ChuanQiEquip = true;

			// 传奇装的教学完毕
			g_FirstChuanQiEquipLook = false; 
			//升级界面再设置  一共3步    第一步在人形面板
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstDisplayChuanQiEquip);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstDisplayChuanQiEquip);
		}
		if (firstDisplay_ChuanQiEquip)
		{
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCSprite *img = (CCSprite*)mBg->getChildByTag(Tag_Img_ShengJiShuoMing);
			if (img)
			{
				guideLayer->init(
					CCSize(img->getContentSize().width, img->getContentSize().height*2), 
					img->getParent()->convertToWorldSpace(img->getPosition()), 
					LFStrings::getValue("chuanqi_chuanQiZhuangShengJiSuoXu"), NULL, true);
				CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
			}
		}
}


void ShenBingShengJi::Qianhuadh(BaseSprite *bs,int level){

	
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("gongjiang_xiaoguo.ExportJson");
	CCArmature *armature = CCArmature::create("gongjiang_xiaoguo");
	ar=armature;
	armature->setZOrder(10);
	//armature->setAnchorPoint(ccp(0.5,0.5));
	//armature->setScale(2);
	string name="chuanqishengji";
	armature->getAnimation()->play(name.c_str());
	armature->getAnimation()->setMovementEventCallFunc(this, 
		  SEL_MovementEventCallFunc(&ShenBingShengJi::callfunc_AnimateEnd));
	CCSize bgSize = bs->getContentSize();
	armature->setPosition(ccp(mWinSize.width/2, bgSize.height-482));
	bs->addChild(armature);
	

}

void ShenBingShengJi::callfunc_AnimateEnd(CCArmature *ar2, MovementEventType, const char *){

 
    ar2->removeFromParent();
    isAnimation=1;
	
	
}


ShenBingShengJi::~ShenBingShengJi(void){

	if(isAnimation==1)
	   CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("gongjiang_xiaoguo.ExportJson");


}