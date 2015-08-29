#include "JiNeng_SelectLayer.h"
#include "font.h"
#include "ImageManager.h"
#include "BaseActorPropertyParser.h"
#include "CS_Common.h"
#include "SkillTree.h"
#include "SkillManager.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "WuPinManager.h"
#include "GuideLayerBase.h"
#include "History.h"
#include "FX_CommonFunc.h"

using namespace FX_CommonFunc;

#define LABEL_SIZE 20
#define LABEL_COLOR ccc3(195, 145, 96)
#define SkillDiscribeLabel_Color ccc3(243, 184, 101)

extern bool g_firstLearnSkillBook;

enum 
{
	Tag_Board,
	Tag_Board_Select,
	Tag_MenuItem_QueRen,
	Tag_MenuItem_FanHui,

	Tag_Dialog_LearnSkill = 100,
	Tag_Ok,
	Tag_Cancel,

	Tag_SkillBook,

	Tag_MenuItem_SkillLvUp_Ok,
	Tag_MenuItem_SkillLvUp_Cancel,
	Tag_Window_SkillLevelUp = 210,
};

JiNeng_SelectLayer* JiNeng_SelectLayer::create(BaseActor* actor, JiNengLayer_comeFrom from)
{
	JiNeng_SelectLayer *layer = new JiNeng_SelectLayer;
	if(layer && layer->init(actor, from))
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

bool JiNeng_SelectLayer::init(BaseActor* actor, JiNengLayer_comeFrom from)
{
	mActor = actor;
	mFrom = from;
	mAllSkill = mActor->getAllSkills();

	mBg = BaseSprite::create("jineng_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

	//人像
	ImageData data = ImageManager::getManager()->getImageData_ProfessionHead(actor->getActorType());
	BaseSprite* heroImg = BaseSprite::create(data.getCurrentImageFile());
	mBg->addChild(heroImg);
	heroImg->setPosition(ccp(60, 838-60));
	BaseSprite* heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
	heroImg->addChild(heroImgBg);
	heroImgBg->setPosition(ccp(heroImg->getContentSize().width/2, heroImg->getContentSize().height/2));
	heroImg->setScale(0.7f);
	//name
	string zhiYeName = BaseActorPropertyParser::getParser()->getExtraProerty(actor->getActorType()).nameInChinese;
	LF::lable(mBg,zhiYeName,ccp(110,838-90),23,fontStr_kaiti);

	//////////////////////////////////////利刃回旋 和 瞬移突袭交换位置了
	unsigned int curSkill = actor->getCurrentUseSkill()->getInfor()->skill_type;
	if (curSkill == SkillType_ShunYinTuXi)
		mSelectIndex = 1;
	else if(curSkill == SkillType_LiRenHuiXuan)
		mSelectIndex = 0;
	else 
		mSelectIndex = curSkill / 10;

	mTableView = CCTableView::create(this, CCSizeMake(622, 618));
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	mTableView->setPosition(ccp(14, 106));
	mTableView->setDelegate(this);
	mBg->addChild(mTableView);
	if(mFrom == comefrom_JiuGuan)
		mTableView->setTouchEnabled(false);

	//
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu);
	mMainMenu->setPosition(CCPointZero);
	if(mFrom == comefrom_renxingMianBan)
	{
		CCMenuItemImage *queren = CCMenuItemImage::create(
			"zhuangbei_xiangxi_queding.png"
			, "zhuangbei_xiangxi_queding_select.png"
			, this, menu_selector(JiNeng_SelectLayer::menuItemClicked_MainMenu));
		mMainMenu->addChild(queren, 1, Tag_MenuItem_QueRen);
		queren->setPosition(ccp(mBg->getContentSize().width/2, queren->getContentSize().height));
	}
	else if(mFrom == comefrom_JiuGuan)
	{
		CCMenuItemImage *fanhui = CCMenuItemImage::create(
			"shuxing_renwu_fanhui.png"
			, "shuxing_renwu_fanhui_select.png"
			, this, menu_selector(JiNeng_SelectLayer::menuItemClicked_MainMenu));
		mMainMenu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
		fanhui->setPosition(ccp(mBg->getContentSize().width/2, fanhui->getContentSize().height));
	}


	return true;
}

unsigned int JiNeng_SelectLayer::numberOfCellsInTableView(CCTableView *table)
{
	Profession prof = mActor->getCurrentProperty().profession;
	if (prof == Profession_Chike
		|| prof == Profession_Fashi
		|| prof == Profession_Yemanren)
		return 4;
	else 
		return 3;
	
}

void JiNeng_SelectLayer::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	if (mFrom == comefrom_JiuGuan)
		return;

	SkillType type = getSkillType(mActor->getCurrentProperty().profession, cell->getIdx());

	bool bHas = false; //是否已经学习
	for (int i=0; i!=mAllSkill.size(); ++i)
	{
		if (type == mAllSkill.at(i)->getInfor()->skill_type)
			bHas = true;
	}
	if( !bHas)
	{
		//没有学、又没有技能书  则返回
		bool hasBook = false;
		vector<ItemInfo> allItem = WuPinManager::getManager()->getAllItems();
		for (int i=0; i!=allItem.size(); ++i)
		{
			if(allItem.at(i).type - ItemsType_SkillBook_MinId == type)
			{
				hasBook = true;
				break;
			}
		}

		if (! hasBook)
		{
			return;
		}
		else // 点击可学习
		{
			{
				//弹框提示
				//弹框背景
				BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
				dialog->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));
				dialog->setTag(Tag_Dialog_LearnSkill);
				//label
				CCLabelTTF *label = CCLabelTTF::create(LFStrings::getValue("shifouxuexigaijineng").c_str(), 
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
						this,SEL_MenuHandler(&JiNeng_SelectLayer::menuItemClicked_learnSkill));


					CCMenuItemImage* menuItemCacel = CCMenuItemImage::create(
						"shangpin_goumai_quxiao.png",
						"shangpin_goumai_quxiao_select.png",
						this,SEL_MenuHandler(&JiNeng_SelectLayer::menuItemClicked_learnSkill));

					menu = CCMenu::create(menuItemMakeSure, menuItemCacel, NULL);
					menu->setPosition(CCPointZero);

					menuItemMakeSure->setTag(Tag_Ok);
					menuItemCacel->setTag(Tag_Cancel);
					menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/3*2,70));
					menuItemCacel->setPosition(ccp(dialog->getContentSize().width/3, 70));
					menuItemMakeSure->setUserData((void*)(ItemsType_SkillBook_MinId + type));
				}

				//弹框
				{
					LFAlert* lfAlert = new LFAlert();
					lfAlert->setTag(Tag_Dialog_LearnSkill);
					lfAlert->setBaseContainer(dialog,menu);
					CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert, 128, Tag_Dialog_LearnSkill);
					lfAlert->release();
				}
			}
		}
		return;
	}

	//删除以前的
	CCTableViewCell* oldSelectCell = table->cellAtIndex(mSelectIndex);
	if(oldSelectCell != NULL)
		oldSelectCell->removeChildByTag(Tag_Board_Select);

	mSelectIndex = cell->getIdx();
	BaseSprite* selectImg = BaseSprite::create("jineng_jiesuo_select.png");
	cell->addChild(selectImg, 10, Tag_Board_Select);
	CCSize size = tableCellSizeForIndex(table, cell->getIdx());
	selectImg->setPosition(ccp(size.width/2, size.height/2));
}

CCTableViewCell* JiNeng_SelectLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
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

	{
		BaseSprite *board = BaseSprite::create("jineng_jiesuo_bg.png");
		cell->addChild(board, 1, Tag_Board);
		CCSize size = tableCellSizeForIndex(table, idx);
		board->setPosition(ccp(size.width/2, size.height/2));

		//
		SkillTree *tree = SkillTree::sharedSkillTree();

		SkillType type = getSkillType(mActor->getCurrentProperty().profession, idx);
		//pic
		BaseSprite *pic = BaseSprite::create(ImageManager::getManager()->getSkillPicFileName(type));
		board->addChild(pic);
		pic->setPosition(ccp(72, 214-89));
		//name
		CCString jinengStr;
		bool isLearn = false;//学习否
		SkillInfo *info = NULL;
		for (int i=0; i!=mAllSkill.size(); ++i)  
			if(type == mAllSkill.at(i)->getInfor()->skill_type)
			{
				isLearn = true;
				info = mAllSkill.at(i)->getInfor();
				break;
			}
		if (isLearn && info->skill_level >= 2)
			jinengStr.initWithFormat("%s+%d", tree->getSkillNameByType(type).c_str(), int(info->skill_level - 1));
		else 
			jinengStr = tree->getSkillNameByType(type);
		CCLabelTTF *nameLabel = CCLabelTTF::create(jinengStr.getCString(), fontStr_kaiti, LABEL_SIZE + 4);
		board->addChild(nameLabel);
		nameLabel->setPosition(ccp(72, 214-176));
		//describe
		CCLabelTTF *dscLabel = CCLabelTTF::create(tree->getSkillDescribeByType(type).c_str(), fontStr_kaiti, LABEL_SIZE
			, CCSize(600-146, 0), kCCTextAlignmentLeft);
		board->addChild(dscLabel);
		dscLabel->setColor(SkillDiscribeLabel_Color);
		dscLabel->setAnchorPoint(ccp(0, 1));
		dscLabel->setPosition(ccp(146, 214-25));
		//触发
		CCLabelTTF *chufaLabel = CCLabelTTF::create(LFStrings::getValue("chufalunci").c_str(), fontStr_kaiti, LABEL_SIZE);
		board->addChild(chufaLabel);
		chufaLabel->setColor(ccc3(127, 127, 127));
		chufaLabel->setAnchorPoint(ccp(0, 0.5));
		chufaLabel->setPosition(ccp(148, 214-182));
		//
		int chuFaDian = CS::getSkillTurn(type);
		int chuFaZhouQi = CS::getSkillZhouQi(type);
		for (int i=0; i!=chuFaZhouQi; ++i)
		{
			BaseSprite *img = NULL;
			if(i == chuFaDian-1)
				img = BaseSprite::create("jineng_chufalunci_icon_chufa.png");
			else 
				img = BaseSprite::create("jineng_chufalunci_icon.png");
			board->addChild(img);
			img->setPosition(ccp(148+chufaLabel->getContentSize().width+i*img->getContentSize().width*2, 214-182));
		}

		//当前技能状态
		CCLabelTTF *stateLable = NULL;
		if (mFrom == comefrom_renxingMianBan)
		{
			// 1. 当前装配的技能  
			if(type == mActor->getCurrentUseSkill()->getInfor()->skill_type)
			{
				stateLable = CCLabelTTF::create(LFStrings::getValue("yizhuangbei").c_str(), fontStr_kaiti, LABEL_SIZE);
				stateLable->setColor(ccGREEN);
				 //技能书的位置放置 升级   2处 --->已装配
				if (mActor->getCurrentUseSkill()->getInfor()->skill_level < SKILL_MAX_LEVEL)
				{
					CCMenu *menu = CCMenu::create();
					board->addChild(menu);
					menu->setPosition(ccp(0,0));
					CCMenuItemImage *sjItem = CCMenuItemImage::create(
						"shengji_anniu_1.png",
						"shengji_anniu_1_select.png",
						this, menu_selector(JiNeng_SelectLayer::menuItemClicked_skillLevelUp));
					menu->addChild(sjItem);
					sjItem->setPosition(ccp(board->getContentSize().width-sjItem->getContentSize().width/3*2
						, sjItem->getContentSize().height*1.5f));
					sjItem->setUserData(mActor->getCurrentUseSkill()->getInfor());
				}				
			}
			else 
			{
				stateLable = CCLabelTTF::create("", fontStr_kaiti, LABEL_SIZE);

				// 2. 已学习    
				bool b = false;
				for (int i=0; i!=mAllSkill.size(); ++i)   
				{
					if(type == mAllSkill.at(i)->getInfor()->skill_type)
					{
						stateLable->setString(LFStrings::getValue("kezhuangbei").c_str());
						stateLable->setColor(ccGREEN);
						b = true;
						//技能书的位置放置 升级    2处--->已学习
						if (mAllSkill.at(i)->getInfor()->skill_level < SKILL_MAX_LEVEL)
						{
							CCMenu *menu = CCMenu::create();
							board->addChild(menu);
							menu->setPosition(ccp(0,0));
							CCMenuItemImage *sjItem = CCMenuItemImage::create(
								"shengji_anniu_1.png",
								"shengji_anniu_1_select.png",
								this, menu_selector(JiNeng_SelectLayer::menuItemClicked_skillLevelUp));
							menu->addChild(sjItem);
							sjItem->setPosition(ccp(board->getContentSize().width-sjItem->getContentSize().width/3*2
								, sjItem->getContentSize().height*1.5f));
							sjItem->setUserData(mAllSkill.at(i)->getInfor());
						}

						break;
					}
				}

				if( !b )      
				{
					BaseSprite *jinengshuImg = BaseSprite::create(ImageManager::getManager()->getSkillBookPicFileName());
					board->addChild(jinengshuImg, 1, Tag_SkillBook);
					jinengshuImg->setPosition(ccp(board->getContentSize().width-jinengshuImg->getContentSize().width/3*2
						, jinengshuImg->getContentSize().height));

					int levelNeed = tree->getSkillLevelByType(type);
					// 3. 等级不够 未解锁
					if(levelNeed > mActor->getCurrentProperty().level)
					{
						char buf[20];
						sprintf(buf, "%d", levelNeed);
						string temp = buf + LFStrings::getValue("ji") + LFStrings::getValue("jiesuo");
						stateLable->setString(temp.c_str());
						stateLable->setColor(ccc3(127, 127, 127));
						nameLabel->setColor(ccc3(127, 127, 127));
						board->initWithFile("jineng_weijiesuo_bg.png");
						jinengshuImg->initWithFile("jinengshu_disable.png");
					}
					else
					{
						bool hasJiNengShu = false;
						vector<ItemInfo> allItems = WuPinManager::getManager()->getAllItems();
						for (int i=0; i!=allItems.size(); ++i)
						{
							if(type == allItems.at(i).type - ItemsType_SkillBook_MinId)
							{
								hasJiNengShu = true;
								break;
							}
						}
						// 4. 有技能书 还未学习
						if (hasJiNengShu)
						{
							stateLable->setString(LFStrings::getValue("kexuexi").c_str());
							stateLable->setColor(ccGREEN);
						}					
						else
						{// 5. 缺少技能书
							stateLable->setString(LFStrings::getValue("queshaojinengshu").c_str());
							stateLable->setColor(ccRED);
							jinengshuImg->initWithFile("jinengshu_disable.png");
						}					
					}

				}
			}
		}
		else
		{
			//显示 多少级解锁
			char buf[20];
			sprintf(buf, "%d", tree->getSkillLevelByType(type));
			string temp = buf + LFStrings::getValue("ji") + LFStrings::getValue("jiesuo");
			stateLable = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, LABEL_SIZE);
			stateLable->setColor(ccc3(127, 127, 127));

			board->initWithFile("jineng_weijiesuo_bg.png");
		}
		board->addChild(stateLable);
		stateLable->setAnchorPoint(ccp(1, 0.5));
		stateLable->setPosition(ccp(596, 214-182));
		

		if(idx == mSelectIndex && mFrom == comefrom_renxingMianBan)
		{
			BaseSprite* selectImg = BaseSprite::create("jineng_jiesuo_select.png");
			cell->addChild(selectImg, 10, Tag_Board_Select);
			selectImg->setPosition(ccp(size.width/2, size.height/2));
		}

	}

	return cell;
}

SkillType JiNeng_SelectLayer::getSkillType(Profession prof, int idxCell)
{
	SkillType type;
	switch (prof)
	{
	case Profession_Yemanren:
		if (idxCell == 0)
			type = SkillType_BaoLieZhan;
		else if(idxCell == 1)
			type = SkillType_BanYueZhan;
		else if(idxCell == 2)
			type = SkillType_KuangBao;
		else if(idxCell == 3)
			type = SkillType_BaoNu;
		break;
	case Profession_Chike:
		if (idxCell == 0)
			type = SkillType_ShaLu;
		else if(idxCell == 1)
			type = SkillType_CiSha;
		else if(idxCell == 2)
			type = SkillType_ShiXueZhiRen;
		else if(idxCell == 3)
			type = SkillType_YinZheHun;
		break;
	case Profession_Fashi:
		if (idxCell == 0)
			type = SkillType_ShanDian;
		else if(idxCell == 1)
			type = SkillType_LeiBao;
		else if(idxCell == 2)
			type = SkillType_YunShi;
		else if(idxCell == 3)
			type = SkillType_TianTangZhiGuang;
		break;
	case Profession_Qishi:
		if (idxCell == 0)
			type = SkillType_JianShou;
		else if(idxCell == 1)
			type = SkillType_MangCi;
		else if(idxCell == 2)
			type = SkillType_ShouHu;
		break;
	case Profession_ShenJianShou:
		if (idxCell == 0)
			type = SkillType_LieGong;
		else if(idxCell == 1)
			type = SkillType_JingZhunZhiCan;
		else if(idxCell == 2)
			type = SkillType_ZhuiHunDuJian;
		break;
	case Profession_MiShu:
		if (idxCell == 0)
			type = SkillType_HuoQiuShu;
		else if(idxCell == 1)
			type = SkillType_BaoFengXue;
		else if(idxCell == 2)
			type = SkillType_HanBingCiGu;
		break;
	case Profession_WuSeng:
		if (idxCell == 0)
			type = SkillType_ShenZhiXingNian;
		else if(idxCell == 1)
			type = SkillType_ShenZhiZhuFu;
		else if(idxCell == 2)
			type = SkillType_ZhongShenZhiLi;
		break;
	case Profession_YouXia:
		if (idxCell == 0)
			type = SkillType_LiRenHuiXuan;
		else if(idxCell == 1)
			type = SkillType_ShunYinTuXi;
		else if(idxCell == 2)
			type = SkillType_XuanFengZhan;
		break;
	case Profession_LieShou:
		if (idxCell == 0)
			type = SkillType_JianYu;
		else if(idxCell == 1)
			type = SkillType_JiSuDuJian;
		else if(idxCell == 2)
			type = SkillType_SiWangSaoShe;
		break;
	case Profession_WuShi:
		if (idxCell == 0)
			type = SkillType_WenYi;
		else if(idxCell == 1)
			type = SkillType_KongJu;
		else if(idxCell == 2)
			type = SkillType_ShiHun;
		break;
	default:
		CCAssert(false, "jineng error");
		break;
	}

	return type;
}

void JiNeng_SelectLayer::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
	mTableView->setTouchEnabled(false);
}

void JiNeng_SelectLayer::disableAllTouchEnd()
{
	mTableView->setTouchEnabled(true);
	mMainMenu->setEnabled(true);
}

void JiNeng_SelectLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	if(mFrom == comefrom_JiuGuan)
		mFooterLayer->dismmisPopUp();

	else
	{
		SkillType type = getSkillType(mActor->getCurrentProperty().profession, mSelectIndex);
		int id = -1;
		for (int i=0; i!=mAllSkill.size(); ++i)
		{
			if(type == mAllSkill.at(i)->getInfor()->skill_type)
			{
				id = mAllSkill.at(i)->getInfor()->skill_id;
				break;
			}
		}

		if(id == mActor->getCurrentUseSkill()->getInfor()->skill_id)//当前已装配的就不用发消息了
		{
			mFooterLayer->dismmisPopUp();
			return;
		}

		if (id != -1)
		{
			addObserver(callfuncO_selector(JiNeng_SelectLayer::equipSkill_Rsp), MSG_EquipSkillRsp);
			mCmdHlp->cmdEquipSkill(mActor->getId(), id);
		}
			
	}
}

void JiNeng_SelectLayer::equipSkill_Rsp(CCObject *msg)
{
	removeObserver(MSG_EquipSkillRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_EquipSkillRsp *data = (SPCmd_EquipSkillRsp*) ((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	mFooterLayer->dismmisPopUp();

}

void JiNeng_SelectLayer::menuItemClicked_learnSkill(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Dialog_LearnSkill);

	if(tag == Tag_Ok)
	{
		unsigned int item_id = (unsigned int)((CCMenuItemImage*)pSender)->getUserData();
		addObserver(callfuncO_selector(JiNeng_SelectLayer::rsp_LearnSkill), MSG_ActorLearnSkillRsp);
		mCmdHlp->cmdActorLearnSkill(mActor->getId(), item_id);
	}
}

void JiNeng_SelectLayer::rsp_LearnSkill(CCObject *msg)
{
	removeObserver(MSG_ActorLearnSkillRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_ActorLearnSkillRsp* data = (SPCmd_ActorLearnSkillRsp*)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	for (int i=0; i!=numberOfCellsInTableView(mTableView); ++i)
	{
		mAllSkill = mActor->getAllSkills();
		mTableView->updateCellAtIndex(i);
	}
}

void JiNeng_SelectLayer::onEnter()
{
	BaseLayer::onEnter();

	//1. 是否是教学时刻
	if (isUI_Guide_LearnSkill && g_firstLearnSkillBook)
	{
		g_firstLearnSkillBook = false; // clear

		bool isFirstLearnSkillBook = false;
		if(History::getHistory()->getBoolFromCurrentHero(Key_IsFirstLearnSkillBook, isFirstLearnSkillBook))
		{
			if(isFirstLearnSkillBook)
				isFirstLearnSkillBook = true;
		}
		else
		{
			isFirstLearnSkillBook = true;
		}
		//2. 是否教学过了
		if(isFirstLearnSkillBook)
		{
			History::getHistory()->putInCurrentHero(Key_IsFirstLearnSkillBook, false);

			// 3. 判断一下 没有该技能书、、、
			bool hasJiNengShu = false;
			vector<ItemInfo> allItems = WuPinManager::getManager()->getAllItems();
			Profession proId = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;
			for (int i=0; i!=allItems.size(); ++i)
			{
				if((ItemsType_SkillBook_LeiBao == allItems.at(i).type && proId == Profession_Fashi)
					|| ((ItemsType_SkillBook_CiSha == allItems.at(i).type && proId == Profession_Chike))
					|| ((ItemsType_SkillBook_BanYueZhan == allItems.at(i).type && proId == Profession_Yemanren)))
				{
					hasJiNengShu = true;
					break;
				}
			}
			if (hasJiNengShu)
			{
				GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
				CCSprite* book = (CCSprite*)mTableView->cellAtIndex(1)->getChildByTag(Tag_Board)->getChildByTag(Tag_SkillBook);
				if(book != NULL)
				{
					guideLayer->init(book->getContentSize(), book->getParent()->convertToWorldSpace(book->getPosition())
						, LFStrings::getValue(""), NULL, false);
					CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
				}
			}
		}
	}
}

void JiNeng_SelectLayer::menuItemClicked_skillLevelUp(CCObject *pSender)
{
	int label_size = 23;

	SkillInfo *pSkillInfo = (SkillInfo*)((CCMenuItemImage*)pSender)->getUserData();

	//所需
	bool bHas = true;
	// 1. 技能书 x1
	int iBooks = WuPinManager::getManager()->getWuPin((ItemsType)(ItemsType_SkillBook_MinId + pSkillInfo->skill_type))->getCount();
	if (iBooks < 1) bHas = false;
	// 2. 银币 
	int iSilver = WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount();
	unsigned int silNeed = CS::skillLevelUpNeedSilver(pSkillInfo->skill_level + 1);
	if (iSilver < silNeed) bHas = false;

	//弹框
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	//确认 取消按键  
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	{
		CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&JiNeng_SelectLayer::menuItemClicked_LevelUpWindow));
		CCMenuItemImage* menuItemCancel = CCMenuItemImage::create(
			"shangpin_goumai_quxiao.png",
			"shangpin_goumai_quxiao_select.png",
			this,SEL_MenuHandler(&JiNeng_SelectLayer::menuItemClicked_LevelUpWindow));
		if (bHas)
		{
			menu->addChild(menuItemOk, 1, Tag_MenuItem_SkillLvUp_Ok);
			menu->addChild(menuItemCancel, 1, Tag_MenuItem_SkillLvUp_Cancel);
			menuItemOk->setPosition(ccp(dialog->getContentSize().width/3,74));
			menuItemCancel->setPosition(ccp(dialog->getContentSize().width/3*2, 70));
			menuItemOk->setUserData(pSkillInfo);
		}
		else 
		{
			menu->addChild(menuItemCancel, 1, Tag_MenuItem_SkillLvUp_Cancel);
			menuItemCancel->setPosition(ccp(dialog->getContentSize().width/2,70));
		}
	}
	//content
	{
		CCSize size = dialog->getContentSize();
		CCString temp;
		//title
		temp = LFStrings::getValue("ShiFou") + LFStrings::getValue("jiang") 
			+ SkillTree::sharedSkillTree()->getSkillNameByType((SkillType)pSkillInfo->skill_type)
			+ LFStrings::getValue("cong") + getStrByInt(pSkillInfo->skill_level) + LFStrings::getValue("ji") 
			+ LFStrings::getValue("ShengJi") + LFStrings::getValue("dao")
			+ getStrByInt(pSkillInfo->skill_level + 1) + LFStrings::getValue("ji")+ "?";
		CCLabelTTF *titleLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, label_size);
		dialog->addChild(titleLabel);
		titleLabel->setAnchorPoint(ccp(0.5,0.5));
		titleLabel->setPosition(ccp(size.width/2, size.height-92));
		if (! bHas)
		{
			titleLabel->setString(LFStrings::getValue("cailiaobuzu").c_str());
			titleLabel->setColor(ccRED);
		}
		// 1.材料
		BaseSprite *materialImg = BaseSprite::create(
			ImageManager::getManager()->getItemFilename(ItemsType_SkillBook_MinId + pSkillInfo->skill_type));
		materialImg->setScale(0.75f);
		dialog->addChild(materialImg);
		materialImg->setPosition(ccp(180, 221));
		//
		temp.initWithFormat("%s x1", CS::getItemName(ItemsType_SkillBook_MinId + pSkillInfo->skill_type).c_str());
		CCLabelTTF *costLabel1 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, label_size);
		dialog->addChild(costLabel1);
		costLabel1->setAnchorPoint(ccp(0, 0.5f));
		costLabel1->setPosition(ccp(size.width/2, materialImg->getPositionY()));
		if (! bHas && iBooks < 1)
			costLabel1->setColor(ccRED);
		// 2.银币
		BaseSprite *silImg = BaseSprite::create("yinbi_tubiao.png");
		dialog->addChild(silImg);
		silImg->setPosition(ccp(180, 221-70));
		//
		temp.initWithFormat("%u", silNeed);
		CCLabelTTF *costLabel2 = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, label_size);
		dialog->addChild(costLabel2);
		costLabel2->setAnchorPoint(ccp(0, 0.5f));
		costLabel2->setPosition(ccp(size.width/2, silImg->getPositionY()));
		if (! bHas && iSilver < silNeed)
			costLabel2->setColor(ccRED);
	}
	//
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setTag(Tag_Window_SkillLevelUp);
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_Window_SkillLevelUp);
		lfAlert->release();
	}
}

void JiNeng_SelectLayer::menuItemClicked_LevelUpWindow(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if (tag == Tag_MenuItem_SkillLvUp_Ok)
	{
		SkillInfo *pSkillInfo = (SkillInfo*)((CCMenuItemImage*)pSender)->getUserData();
		addObserver(callfuncO_selector(JiNeng_SelectLayer::rsp_LevelUpSkill), MSG_skillLevelUpRsp);
		mCmdHlp->cmdSkillLevelUp(pSkillInfo->owner_hero_id, pSkillInfo->skill_id);
	}
	else if (tag == Tag_MenuItem_SkillLvUp_Cancel)
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_SkillLevelUp);
	}
}

void JiNeng_SelectLayer::rsp_LevelUpSkill(CCObject *msg)
{
	removeObserver(MSG_skillLevelUpRsp);
	GameDirector::getDirector()->hideWaiting();
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_SkillLevelUp);

	SPCmd_SkillLevelUpRsp* data = (SPCmd_SkillLevelUpRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}
}