#include "JiaoTangLayer.h"
#include "protocol.h"
#include "MyselfManager.h"
#include "LFStrings.h"
#include "font.h"
#include "CS_Common.h"
#include "ImageManager.h"
#include "WuPinManager.h"
#include "FX_CommonFunc.h"
#include "SkillManager.h"
#include "GameDirector.h"
#include "MessageDisplay.h"

using namespace FX_CommonFunc;

#define Label_Size_Min 20
#define Label_Size_Max 24
#define COLOR_Hui ccc3(127,127,127)
#define COLOR_DanHuang ccc3(195, 145, 96)
#define COLOR_Huang ccc3(253, 180, 40)
#define COLOR_Lv ccc3(123, 200, 28)

enum 
{
	Tag_HeadMenuItem_JiaoTangShengJi,
	Tag_HeadMenuItem_JueSeXiLi,

	Tag_MenuItem_ShengJi,
	Tag_MenuItem_XiLi,
	Tag_MenuItem_QuXiao,
	Tag_MenuItem_Ok,

	Tag_XiLiBoard_Cur,    //洗礼人物 的 当前属性
	Tag_XiLiBoard_Next,   //洗礼人物 的 下一阶属性
	Tag_XiLiBoard_Skill,  //洗礼 下一阶的技能
	Tag_XiLi_Hero,        //洗礼人物 的 全身像
	Tag_XiLiBoard_NeedMaterial,   //洗礼人物 所需材料
	Tag_XiLiBoard_NeedSilver,       //所需银币
	Tag_XiLiBoard_ManJie,  //满阶 属性框
	//洗礼成功后添加的
	Tag_XiLiLabel_ChengGong,
	Tag_XiLiLabel_JieSuoJiNeng,
	Tag_XiLiImg_JiNeng,

	Tag_BoardMenu = 99,
	Tag_Board_XiLiKuang = 100, // 100-110
};


JiaoTangLayer* JiaoTangLayer::create(int flag)
{
	JiaoTangLayer *layer = new JiaoTangLayer(flag);
	if (layer)
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

JiaoTangLayer::~JiaoTangLayer()
{

}

JiaoTangLayer::JiaoTangLayer(int flag)
{
	BaseLayer::init();

	mActorsVec = MyselfManager::getManager()->getAllTeamActors(Actor_All,LF_All, ACTOR_TYPE_ZHEN_SHEN, false);

	mCurFlag = flag;

	mBg = BaseSprite::create("jiaotang_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));
	//
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 10);
	mMainMenu->setPosition(ccp(0,0));
	//
	mHeadMenu = CCMenu::create();
	mBg->addChild(mHeadMenu);
	mHeadMenu->setPosition(ccp(0,0));
	CCMenuItemImage *sj = CCMenuItemImage::create(
		"jiaotang_anniu_jiaotangshengji.png",
		"jiaotang_anniu_jiaotangshengji_select.png",
		this, menu_selector(JiaoTangLayer::menuItemClicked_HeadMenu));
	mHeadMenu->addChild(sj, 0, Tag_HeadMenuItem_JiaoTangShengJi);
	sj->setPosition(ccp(129, 838-44));
	CCMenuItemImage *xl = CCMenuItemImage::create(
		"jiaotang_anniu_renwuxili.png",
		"jiaotang_anniu_renwuxili_select.png",
		this, menu_selector(JiaoTangLayer::menuItemClicked_HeadMenu));
	mHeadMenu->addChild(xl, 0, Tag_HeadMenuItem_JueSeXiLi);
	xl->setPosition(ccp(501, 838-44));

	if (mCurFlag == 0)
		menuItemClicked_HeadMenu(sj);
	else if (mCurFlag == 1)
		menuItemClicked_HeadMenu(xl);
}

void JiaoTangLayer::disableAllTouchBegin()
{
	mHeadMenu->setVisible(false);
	mMainMenu->setVisible(false);

	CCMenu *xiliBoardMenu = (CCMenu*)mBg->getChildByTag(Tag_BoardMenu);
	if (xiliBoardMenu) xiliBoardMenu->setVisible(false);
}

void JiaoTangLayer::disableAllTouchEnd()
{
	mHeadMenu->setVisible(true);
	mMainMenu->setVisible(false);

	CCMenu *xiliBoardMenu = (CCMenu*)mBg->getChildByTag(Tag_BoardMenu);
	if (xiliBoardMenu) xiliBoardMenu->setVisible(true);
}

//
CCSize JiaoTangLayer::getBodySize()
{
	return mBg->getContentSize();
}

CCPoint JiaoTangLayer::getBodyCenterPos()
{
	return ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2);
}

void JiaoTangLayer::menuItemClicked_HeadMenu(CCObject *pSender)
{
	clearBg();
	int tag = ((CCMenuItemImage*)pSender)->getTag();
	CCMenuItemImage *jt = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenuItem_JiaoTangShengJi);
	CCMenuItemImage *js = (CCMenuItemImage*)mHeadMenu->getChildByTag(Tag_HeadMenuItem_JueSeXiLi);
	CCString temp;

	if (tag == Tag_HeadMenuItem_JiaoTangShengJi)
	{
		mCurFlag = 0;
		jt->initWithNormalImage(
			"jiaotang_anniu_jiaotangshengji_select.png",
			"jiaotang_anniu_jiaotangshengji.png",
			NULL, this, menu_selector(JiaoTangLayer::menuItemClicked_HeadMenu));
		js->initWithNormalImage(
			"jiaotang_anniu_renwuxili.png",
			"jiaotang_anniu_renwuxili_select.png",
			NULL, this, menu_selector(JiaoTangLayer::menuItemClicked_HeadMenu));

		//
		CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("ShuoMing_ShengJIJiaoTang").c_str(), fontStr_kaiti, Label_Size_Min);
		mBg->addChild(tishiLabel);
		tishiLabel->setColor(COLOR_Hui);
		tishiLabel->setPosition(ccp(320, 838-122));
		//
		unsigned int level_jiaotang = MyselfManager::getManager()->getJiaoTangLevel();
		CCString jiaotangStr;
		jiaotangStr.initWithFormat("zhuye_anniu_jiaotang_%u_select.png", getJiaoTangFileIdxByLevel(level_jiaotang));
		//test
		//level_jiaotang = HERO_MAX_STAGE; 
		if (level_jiaotang >= HERO_MAX_STAGE) //人物和教堂一样的。
		{
			BaseSprite *jtBoard = BaseSprite::create("jiaotang_shengji_bg.png");
			mBg->addChild(jtBoard);
			jtBoard->setPosition(ccp(320, 838-303));
			//
			BaseSprite *jtImg = BaseSprite::create(jiaotangStr.getCString());
			jtBoard->addChild(jtImg);
			jtImg->setPosition(ccp(jtBoard->getContentSize().width/2, jtBoard->getContentSize().height/2));
			jtImg->setScale(0.6f);
			//
			temp.initWithFormat("%s:", LFStrings::getValue("DangQian").c_str());
			CCLabelTTF *dangQianLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			mBg->addChild(dangQianLabel);
			dangQianLabel->setColor(COLOR_Huang);
			dangQianLabel->setPosition(ccp(320, 838-168));
			//
			temp.initWithFormat("%s%s%s", getStrByInt(level_jiaotang).c_str()
				, LFStrings::getValue("ji").c_str(), LFStrings::getValue("JiaoTang").c_str());
			CCLabelTTF *jtLvLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			mBg->addChild(jtLvLabel);
			jtLvLabel->setColor(COLOR_Huang);
			jtLvLabel->setPosition(ccp(320, 838-444));
			//
			temp.initWithFormat("%s", LFStrings::getValue("YiShiManJi").c_str());
			CCLabelTTF *tsLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Max);
			mBg->addChild(tsLabel);
			tsLabel->setColor(COLOR_Hui);
			tsLabel->setPosition(ccp(320, 838-573));
		}
		else
		{
			/////////////////////////
			//current
			/////////////////////////
			BaseSprite *jtCurBoard = BaseSprite::create("jiaotang_shengji_bg.png");
			mBg->addChild(jtCurBoard);
			jtCurBoard->setPosition(ccp(169, 838-303));
			//
			BaseSprite *jtCurImg = BaseSprite::create(jiaotangStr.getCString());
			jtCurBoard->addChild(jtCurImg);
			jtCurImg->setPosition(ccp(jtCurBoard->getContentSize().width/2, jtCurBoard->getContentSize().height/2));
			jtCurImg->setScale(0.6f);
			//
			temp.initWithFormat("%s:", LFStrings::getValue("DangQian").c_str());
			CCLabelTTF *dangQianLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			mBg->addChild(dangQianLabel);
			dangQianLabel->setColor(COLOR_Huang);
			dangQianLabel->setPosition(ccp(169, 838-168));
			//
			temp.initWithFormat("%s%s%s", getStrByInt(level_jiaotang).c_str()
				, LFStrings::getValue("ji").c_str(), LFStrings::getValue("JiaoTang").c_str());
			CCLabelTTF *jtCurLvLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			mBg->addChild(jtCurLvLabel);
			jtCurLvLabel->setColor(COLOR_Huang);
			jtCurLvLabel->setPosition(ccp(169, 838-444));
			/////////////////////////
			//next stage
			/////////////////////////
			BaseSprite *jtNextBoard = BaseSprite::create("jiaotang_shengji_bg.png");
			mBg->addChild(jtNextBoard);
			jtNextBoard->setPosition(ccp(468, 838-303));
			//
			jiaotangStr.initWithFormat("zhuye_anniu_jiaotang_%u_select.png", getJiaoTangFileIdxByLevel(level_jiaotang + 1));
			BaseSprite *jtNextImg = BaseSprite::create(jiaotangStr.getCString());
			jtNextBoard->addChild(jtNextImg);
			jtNextImg->setPosition(ccp(jtNextBoard->getContentSize().width/2, jtNextBoard->getContentSize().height/2));
			jtNextImg->setScale(0.6f);
			//
			temp.initWithFormat("%s:", LFStrings::getValue("XiaYiJie").c_str());
			CCLabelTTF *dangQianNextLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			mBg->addChild(dangQianNextLabel);
			dangQianNextLabel->setColor(ccWHITE);
			dangQianNextLabel->setPosition(ccp(468, 838-168));
			//
			temp.initWithFormat("%s%s%s", getStrByInt(level_jiaotang + 1).c_str()
				, LFStrings::getValue("ji").c_str(), LFStrings::getValue("JiaoTang").c_str());
			CCLabelTTF *jtNextLvLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			mBg->addChild(jtNextLvLabel);
			jtNextLvLabel->setColor(ccWHITE);
			jtNextLvLabel->setPosition(ccp(468, 838-444));
			///////////////////升级所需
			BaseSprite *nextImg = BaseSprite::create("jiaotang_shengji_jiantou.png");
			mBg->addChild(nextImg);
			nextImg->setPosition(ccp(320, 838-303));
			//
			BaseSprite *hengTiaoImg = BaseSprite::create("jiaotang_shengji_tiao.png");
			mBg->addChild(hengTiaoImg);
			hengTiaoImg->setPosition(ccp(320, 838-504));
			//
			temp.initWithFormat("%s%s", LFStrings::getValue("suoxu").c_str(), LFStrings::getValue("cailiao").c_str());
			CCLabelTTF *suoxuLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			mBg->addChild(suoxuLabel);
			suoxuLabel->setPosition(hengTiaoImg->getPosition());
			suoxuLabel->setColor(COLOR_DanHuang);

			//need
			bool isHave = true;
			unsigned int dalishi_need=0, huagangyan_need=0, mucai_need=0, shuijinshi_need=0;
			CS::churchLevelUpNeedItemCount(level_jiaotang+1, dalishi_need, huagangyan_need, mucai_need, shuijinshi_need);
			int count = 0;
			BaseSprite *board[4];
			if (dalishi_need > 0)
			{
				board[count] = getNeedMaterialBoard(ItemsType_Dalishi, dalishi_need, isHave);
				mBg->addChild(board[count]);
				++count;
			}
			if (huagangyan_need > 0)
			{
				board[count] = getNeedMaterialBoard(ItemsType_Huagangyan, huagangyan_need, isHave);
				mBg->addChild(board[count]);
				++count;
			}
			if (mucai_need > 0)	
			{
				board[count] = getNeedMaterialBoard(ItemsType_Mucai, mucai_need, isHave);
				mBg->addChild(board[count]);
				++count;
			}
			if (shuijinshi_need > 0) 
			{
				board[count] = getNeedMaterialBoard(ItemsType_Shuijingshi, shuijinshi_need, isHave);
				mBg->addChild(board[count]);
				++count;
			}
			int start_x = 320-(count-1)*65;
			for (int idx=0; idx!=count; ++idx)
				board[idx]->setPosition(ccp(start_x + 130*idx, 838-598));	
			//silver
			unsigned int needSil = CS::churchLevelUpNeedSilver(level_jiaotang + 1);
			if (needSil > 0)
			{
				BaseSprite *silIcon = BaseSprite::create("yinbi_tubiao.png");
				mBg->addChild(silIcon);
				silIcon->setPosition(ccp(254, 838-714));
				//
				temp.initWithFormat("%u / ", needSil);
				CCLabelTTF *needLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
				silIcon->addChild(needLabel);
				needLabel->setAnchorPoint(ccp(0, 0.5f));
				needLabel->setPosition(ccp(silIcon->getContentSize().width, silIcon->getContentSize().height/2));
				//
				int allSil = WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount();
				temp.initWithFormat("%d", allSil);
				CCLabelTTF *allLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
				needLabel->addChild(allLabel);
				allLabel->setAnchorPoint(ccp(0, 0.5f));
				allLabel->setPosition(ccp(needLabel->getContentSize().width, needLabel->getContentSize().height/2));
				if (allSil < needSil)
				{
					allLabel->setColor(ccRED);
					isHave = false;
				}
			}

			//menu
			CCMenuItemImage *sjItem = CCMenuItemImage::create(
				"jiaotang_anniu_shengji.png",
				"jiaotang_anniu_shengji_select.png",
				"jiaotang_anniu_shengji_suoding.png",
				this, menu_selector(JiaoTangLayer::menuItemClicked_JiaoTangMainMenu));
			mMainMenu->addChild(sjItem, 1, Tag_MenuItem_ShengJi);
			sjItem->setPosition(ccp(195, 838-780));
			CCMenuItemImage *qxItem = CCMenuItemImage::create(
				"shangpin_goumai_quxiao.png",
				"shangpin_goumai_quxiao_select.png",
				this, menu_selector(JiaoTangLayer::menuItemClicked_JiaoTangMainMenu));
			mMainMenu->addChild(qxItem, 1, Tag_MenuItem_QuXiao);
			qxItem->setPosition(ccp(483, 838-780));
			if (! isHave)
				sjItem->setEnabled(false);
		}		
	}
	else if (tag == Tag_HeadMenuItem_JueSeXiLi)
	{
		mCurFlag = 1;
		jt->initWithNormalImage(
			"jiaotang_anniu_jiaotangshengji.png",
			"jiaotang_anniu_jiaotangshengji_select.png",
			NULL, this, menu_selector(JiaoTangLayer::menuItemClicked_HeadMenu));
		js->initWithNormalImage(
			"jiaotang_anniu_renwuxili_select.png",
			"jiaotang_anniu_renwuxili.png",
			NULL, this, menu_selector(JiaoTangLayer::menuItemClicked_HeadMenu));

		//列出所有人物
		CCMenu *boardMenu = CCMenu::create();
		mBg->addChild(boardMenu, 0, Tag_BoardMenu);
		boardMenu->setPosition(ccp(0,0));
		for (int i=0; i!=mActorsVec.size(); ++i)
		{
			BaseSprite *board = BaseSprite::create("jiaotang_xili_renwukuang.png");
			mBg->addChild(board, 0, Tag_Board_XiLiKuang + i);
			board->setPosition(ccp(68+110*(i%2), 838-193-153*(i/2)));
			//
			BaseSprite *headImg = BaseSprite::create(
				ImageManager::getManager()->getImageData_ProfessionHead(mActorsVec.at(i)->getActorType()).getCurrentImageFile());
			board->addChild(headImg);
			headImg->setScale(0.7f);
			headImg->setPosition(ccp(52, 106));
			BaseSprite *kuang = BaseSprite::create("renxing_renwu_touxiang_select.png");
			headImg->addChild(kuang);
			kuang->setPosition(ccp(headImg->getContentSize().width/2, headImg->getContentSize().height/2));
			//level
			temp.initWithFormat("Lv%u", mActorsVec.at(i)->getCurrentProperty().level);
			CCLabelTTF *lvLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
			board->addChild(lvLabel);
			lvLabel->setColor(COLOR_Hui);
			lvLabel->setPosition(ccp(30, 46));
			//stage
			temp.initWithFormat("%s%s", getStrByInt(mActorsVec.at(i)->getCurrentStage()).c_str(), LFStrings::getValue("jie").c_str());
			CCLabelTTF *stageLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
			board->addChild(stageLabel);
			if (mActorsVec.at(i)->getCurrentStage() >= HERO_MAX_STAGE)
				stageLabel->setColor(COLOR_Lv);
			else 
				stageLabel->setColor(COLOR_Huang);
			stageLabel->setPosition(ccp(74, 46));
			//shang zheng 
			if (mActorsVec.at(i)->getChessBoardPos() != 0)
			{
				CCLabelTTF *label = CCLabelTTF::create(LFStrings::getValue("YiShangZhen").c_str(), fontStr_kaiti, Label_Size_Min);
				board->addChild(label);
				label->setPosition(ccp(52, 22));
			}
			//
			CCMenuItemImage *item = CCMenuItemImage::create(
				"jiaotang_xili_renwukuang.png",
				"jiaotang_xili_renwukuang.png",
				this, menu_selector(JiaoTangLayer::menuItemClicked_XiLiHeroBoard));
			boardMenu->addChild(item, 1, Tag_Board_XiLiKuang + i);
			item->setPosition(board->getPosition());
			item->setUserData(mActorsVec.at(i));
			item->setOpacity(0);
			if (mActorsVec.at(i)->getId() == MyselfManager::getManager()->getMyZhuJueData()->getId()) mXiLi_CurHeroItem = item;
		}
		///////////////////////////////////////////
		//hero img board
		BaseSprite *heroBoard = BaseSprite::create("jiaotang_shengji_bg.png");
		mBg->addChild(heroBoard);
		heroBoard->setPosition(ccp(430, 838-298));
		heroBoard->setScale(1.7f);
		//hero img
		CCNode *heroImg = CCNode::create();
		mBg->addChild(heroImg, 2, Tag_XiLi_Hero);
		heroImg->setPosition(heroBoard->getPosition());
		//cur stage prop board
		BaseSprite *curPropBoard = BaseSprite::create("jiaotang_shengji_bg.png");
		mBg->addChild(curPropBoard, 1, Tag_XiLiBoard_Cur);
		curPropBoard->setScale(0.7f);
		curPropBoard->setPosition(ccp(300, 838-308));
		//next stage prop board
		BaseSprite *nextPropBoard = BaseSprite::create("jiaotang_shengji_bg.png");
		mBg->addChild(nextPropBoard, 1, Tag_XiLiBoard_Next);
		nextPropBoard->setScale(0.7f);
		nextPropBoard->setPosition(ccp(560, 838-308));
		//next skill
		BaseSprite *nextSkillBoard = BaseSprite::create("jineng_icon_bg.png");
		mBg->addChild(nextSkillBoard, 1, Tag_XiLiBoard_Skill);
		nextSkillBoard->setPosition(ccp(560, 838-440));
		nextSkillBoard->setScale(0.8f);
		//need material board
		BaseSprite *matBoard = BaseSprite::create();
		mBg->addChild(matBoard, 0, Tag_XiLiBoard_NeedMaterial);
		matBoard->setPosition(ccp(429, 838-565));
		//need silver board
		BaseSprite *silIcon = BaseSprite::create("yinbi_tubiao.png");
		mBg->addChild(silIcon, 0, Tag_XiLiBoard_NeedSilver);
		silIcon->setPosition(ccp(378, 838-698));
		//满阶board
		BaseSprite *mjBoard = BaseSprite::create("jiaotang_shengji_bg.png");
		mBg->addChild(mjBoard, 0, Tag_XiLiBoard_ManJie);
		mjBoard->setPosition(ccp(429, 838-633));
		mjBoard->setScale(0.7f);
		//menu
		CCMenuItemImage *xiliItem = CCMenuItemImage::create(
			"jiaotang_anniu_xili.png",
			"jiaotang_anniu_xili_select.png",
			"jiaotang_anniu_xili_suoding.png",
			this, menu_selector(JiaoTangLayer::menuItemClicked_XiLiMainMenu));
		mMainMenu->addChild(xiliItem, 0, Tag_MenuItem_XiLi);
		xiliItem->setPosition(ccp(429, 838-780));
		CCMenuItemImage *okItem = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this, menu_selector(JiaoTangLayer::menuItemClicked_XiLiMainMenu));
		mMainMenu->addChild(okItem, 0, Tag_MenuItem_Ok);
		okItem->setPosition(ccp(429, 838-780));
		//
		menuItemClicked_XiLiHeroBoard(mXiLi_CurHeroItem);
	}
}

void JiaoTangLayer::clearBg()
{
	mHeadMenu->retain();
	mBg->removeAllChildren();
	mBg->addChild(mHeadMenu);
	mHeadMenu->setPosition(ccp(0,0));
	mHeadMenu->release();

	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 10);
	mMainMenu->setPosition(ccp(0,0));
}

void JiaoTangLayer::setPropBoardData(BaseSprite *board, int stage, bool cur)
{
	board->removeAllChildren();
	CCSize size = board->getContentSize();
	ccColor3B color;
	if (! cur)						  color = COLOR_Hui;
	else if (stage >= HERO_MAX_STAGE) color = COLOR_Lv;
	else							  color = COLOR_Huang;
	float scale = 1 / board->getScale();
	CCString temp;
	//
	if (cur)
		temp.initWithFormat("%s:", LFStrings::getValue("DangQian").c_str());
	else 
		temp.initWithFormat("%s:", LFStrings::getValue("XiaYiJie").c_str());
	CCLabelTTF *dqLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	board->addChild(dqLabel);
	dqLabel->setPosition(ccp(size.width/2, size.height-dqLabel->getContentSize().height - 30));
	dqLabel->setColor(color);
	dqLabel->setScale(scale);
	//stage num
	if (stage >= HERO_MAX_STAGE)
		temp.initWithFormat("%s", LFStrings::getValue("ManJie").c_str());
	else
		temp.initWithFormat("%s%s", getStrByInt(stage).c_str(), LFStrings::getValue("jie").c_str());
	CCLabelTTF *stageLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	board->addChild(stageLabel);
	stageLabel->setPosition(ccp(size.width/2, dqLabel->getPositionY()-42));
	stageLabel->setColor(color);
	stageLabel->setScale(scale);
	//prop
	unsigned int ratio = CS::getHeroStageEffectRatio(stage);
	temp.initWithFormat("%s+%u%%", LFStrings::getValue("gongJi").c_str(), ratio);
	CCLabelTTF *gjLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	board->addChild(gjLabel);
	gjLabel->setColor(color);
	gjLabel->setPosition(ccp(size.width/2, stageLabel->getPositionY() - 42));
	gjLabel->setScale(scale);
	//
	temp.initWithFormat("%s+%u%%", LFStrings::getValue("huJia").c_str(), ratio);
	CCLabelTTF *hjLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	board->addChild(hjLabel);
	hjLabel->setColor(color);
	hjLabel->setPosition(ccp(size.width/2, gjLabel->getPositionY() - 34));
	hjLabel->setScale(scale);
	//
	temp.initWithFormat("%s+%u%%", LFStrings::getValue("shengMin").c_str(), ratio);
	CCLabelTTF *xlLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	board->addChild(xlLabel);
	xlLabel->setColor(color);
	xlLabel->setPosition(ccp(size.width/2, hjLabel->getPositionY() - 34));
	xlLabel->setScale(scale);
}

void JiaoTangLayer::menuItemClicked_XiLiHeroBoard(CCObject *pSender)
{
	BaseSprite *infoBoar = NULL;
	CCMenuItemImage *newHeroItem = (CCMenuItemImage*)pSender;
	BaseActor *actor = (BaseActor*)newHeroItem->getUserData();
	//上一个选中的 board 设置为 未选中
	infoBoar = (BaseSprite*)mBg->getChildByTag(mXiLi_CurHeroItem->getTag());
	infoBoar->initWithFile("jiaotang_xili_renwukuang.png");
	//当前的设置为选中
	infoBoar = (BaseSprite*)mBg->getChildByTag(newHeroItem->getTag());
	infoBoar->initWithFile("jiaotang_xili_renwukuang_select.png");
	//保存
	mXiLi_CurHeroItem = newHeroItem;
	//

	CCString temp;
	CCPoint pos;
	//全身像
	CCNode *heroImg = mBg->getChildByTag(Tag_XiLi_Hero);
	pos = heroImg->getPosition();
	heroImg->removeFromParent();
	heroImg = ImageManager::getManager()->getFullPhotoOfActor(actor);
	mBg->addChild(heroImg, 2, Tag_XiLi_Hero);
	heroImg->setPosition(pos);
	//
	if (actor->getCurrentStage() >= HERO_MAX_STAGE) // 满级
	{
		BaseSprite *board = NULL;
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_Cur);
		if (board) board->setVisible(false);
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_Next);
		if (board) board->setVisible(false);
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_Skill);
		if (board) board->setVisible(false);
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_NeedMaterial);
		if (board) board->setVisible(false);
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_NeedSilver);
		if (board) board->setVisible(false);
		//
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_ManJie);
		if (board)
		{
			board->setVisible(true);
			board->removeAllChildren();
			setPropBoardData(board, actor->getCurrentStage(), true);
		}
		mMainMenu->getChildByTag(Tag_MenuItem_XiLi)->setVisible(false);
		mMainMenu->getChildByTag(Tag_MenuItem_Ok)->setVisible(false);
	}
	else
	{
		bool isHave = true;
		//
		BaseSprite *board = NULL;
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_ManJie);
		if (board) board->setVisible(false);
		//
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_Cur);
		if (board)
		{
			board->setVisible(true);
			board->removeAllChildren();
			setPropBoardData(board, actor->getCurrentStage(), true);
		}
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_Next);
		if (board)
		{
			board->setVisible(true);
			board->removeAllChildren();
			setPropBoardData(board, actor->getCurrentStage()+1, false);
		}
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_Skill);
		if (board) 
		{
			board->setVisible(true);
			board->removeAllChildren();
			BaseSprite *skillImg = BaseSprite::create(ImageManager::getManager()->getSkillPicFileName(
				(SkillType)actor->getCurrentUseSkill()->getInfor()->skill_type));
			board->addChild(skillImg);
			skillImg->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));
		}
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_NeedMaterial);
		if (board)
		{
			board->setVisible(true);
			pos = board->getPosition();
			board->removeFromParent();
			board = getNeedMaterialBoard(ItemsType_Hero_Upgrade, CS::heroUpgradeNeedItemCount(actor->getCurrentStage()+1), isHave);
			mBg->addChild(board, 1, Tag_XiLiBoard_NeedMaterial);
			board->setPosition(pos);
		}
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_NeedSilver);
		if (board)
		{
			board->setVisible(true);
			board->removeAllChildren();
			//
			unsigned int needSil = CS::heroUpgradeNeedSilver(actor->getCurrentStage() + 1);
			temp.initWithFormat("%u / ", needSil);
			CCLabelTTF *needLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
			board->addChild(needLabel);
			needLabel->setAnchorPoint(ccp(0, 0.5f));
			needLabel->setPosition(ccp(board->getContentSize().width, board->getContentSize().height/2));
			//
			int allSil = WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount();
			temp.initWithFormat("%d", allSil);
			CCLabelTTF *allLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, Label_Size_Min);
			needLabel->addChild(allLabel);
			allLabel->setAnchorPoint(ccp(0, 0.5f));
			allLabel->setPosition(ccp(needLabel->getContentSize().width, needLabel->getContentSize().height/2));
			if (allSil < needSil)
			{
				allLabel->setColor(ccRED);
				isHave = false;
			}
		}
		//menu
		mMainMenu->getChildByTag(Tag_MenuItem_XiLi)->setVisible(true);
		mMainMenu->getChildByTag(Tag_MenuItem_Ok)->setVisible(false);
		if (! isHave) ((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_XiLi))->setEnabled(false);
		else          ((CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_XiLi))->setEnabled(true);
	}
}

void JiaoTangLayer::menuItemClicked_JiaoTangMainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if (tag == Tag_MenuItem_ShengJi)
	{
		addObserver(callfuncO_selector(JiaoTangLayer::rsp_JiaoTangLevelUp), MSG_churchLevelUpRsp);
		mCmdHlp->cmdChurchLevelUp();
	}
	else if (tag == Tag_MenuItem_QuXiao)
	{
		GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
	}
	else if (tag == Tag_MenuItem_Ok)
	{
		menuItemClicked_HeadMenu(mHeadMenu->getChildByTag(Tag_HeadMenuItem_JiaoTangShengJi));
	}
}

void JiaoTangLayer::rsp_JiaoTangLevelUp(CCObject *msg)
{
	removeObserver(MSG_churchLevelUpRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_ChurchLevelUpRsp* data = (SPCmd_ChurchLevelUpRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	clearBg();
	CCString temp;
	//
	temp.initWithFormat("zhuye_anniu_jiaotang_%u_select.png", getJiaoTangFileIdxByLevel(data->church_level));
	BaseSprite *jtImg = BaseSprite::create(temp.getCString());
	mBg->addChild(jtImg);
	jtImg->setPosition(ccp(320, 838-303));
	jtImg->setScale(0.6f);
	//
	temp.initWithFormat("%s:", LFStrings::getValue("DangQian").c_str());
	CCLabelTTF *dangQianLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	mBg->addChild(dangQianLabel);
	dangQianLabel->setColor(COLOR_Huang);
	dangQianLabel->setPosition(ccp(320, 838-168));
	//
	temp.initWithFormat("%s%s%s", getStrByInt(data->church_level).c_str()
		, LFStrings::getValue("ji").c_str(), LFStrings::getValue("JiaoTang").c_str());
	CCLabelTTF *jtLvLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Min);
	mBg->addChild(jtLvLabel);
	jtLvLabel->setColor(COLOR_Huang);
	jtLvLabel->setPosition(ccp(320, 838-444));
	//
	temp.initWithFormat("%s!", LFStrings::getValue("ShengJiChengGong").c_str());
	CCLabelTTF *tsLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size_Max);
	mBg->addChild(tsLabel);
	tsLabel->setPosition(ccp(320, 838-573));
	//
	CCMenuItemImage *itemOk = CCMenuItemImage::create(
		"shangpin_goumai_queren.png",
		"shangpin_goumai_queren_select.png",
		this, menu_selector(JiaoTangLayer::menuItemClicked_JiaoTangMainMenu));
	mMainMenu->addChild(itemOk, 0, Tag_MenuItem_Ok);
	itemOk->setPosition(ccp(320, 838-780));
}

void JiaoTangLayer::menuItemClicked_XiLiMainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if (tag == Tag_MenuItem_XiLi)
	{
		addObserver(callfuncO_selector(JiaoTangLayer::rsp_HeroDegreeUp), MSG_heroUpgradeRsp);
		BaseActor *actor = (BaseActor*)(mXiLi_CurHeroItem->getUserData());
		if (actor) mCmdHlp->cmdHeroUpgrade(actor->getId());
	}
	else if (tag == Tag_MenuItem_Ok)
	{
		((CCMenu*)mBg->getChildByTag(Tag_BoardMenu))->setEnabled(true);
		mMainMenu->getChildByTag(Tag_MenuItem_Ok)->setVisible(false);

		mBg->removeChildByTag(Tag_XiLiLabel_ChengGong);
		mBg->removeChildByTag(Tag_XiLiLabel_JieSuoJiNeng);
		mBg->removeChildByTag(Tag_XiLiImg_JiNeng);

		menuItemClicked_XiLiHeroBoard(mXiLi_CurHeroItem);
	}
}

void JiaoTangLayer::rsp_HeroDegreeUp(CCObject *msg)
{
	removeObserver(MSG_heroUpgradeRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_HeroUpgradeRsp* data = (SPCmd_HeroUpgradeRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	BaseActor *actor = (BaseActor*)mXiLi_CurHeroItem->getUserData();

	//全身像
	CCNode *heroImg = mBg->getChildByTag(Tag_XiLi_Hero);
	CCPoint pos = heroImg->getPosition();
	heroImg->removeFromParent();
	heroImg = ImageManager::getManager()->getFullPhotoOfActor(actor);
	mBg->addChild(heroImg, 2, Tag_XiLi_Hero);
	heroImg->setPosition(pos);

	//
	BaseSprite *board = NULL;
	board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_Cur);
	board->removeAllChildren();
	setPropBoardData(board, data->hero_detial.basic_info.stage, true);
	{
		//next
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_Next);
		if (board) board->setVisible(false);
		//next skill
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_Skill);
		if (board) board->setVisible(false);
		//need material
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_NeedMaterial);
		if (board) board->setVisible(false);
		//need silver
		board = (BaseSprite*)mBg->getChildByTag(Tag_XiLiBoard_NeedSilver);
		if (board) board->setVisible(false);
	}
	//add
	CCLabelTTF *tishiLabel = CCLabelTTF::create((LFStrings::getValue("XiLiChengGong")+"!").c_str(), fontStr_kaiti, Label_Size_Max);
	mBg->addChild(tishiLabel, 0, Tag_XiLiLabel_ChengGong);
	tishiLabel->setPosition(ccp(429, 838-530));
	//
	CCLabelTTF *jnLabel = CCLabelTTF::create((LFStrings::getValue("JieSuoJiNeng") + ": " 
		+ SkillTree::sharedSkillTree()->getSkillNameByType((SkillType)actor->getCurrentUseSkill()->getInfor()->skill_type)).c_str()
		, fontStr_kaiti, Label_Size_Min);
	mBg->addChild(jnLabel, 0, Tag_XiLiLabel_JieSuoJiNeng);
	jnLabel->setPosition(ccp(429, 838-559));
	//
	BaseSprite *skillBoard = BaseSprite::create("jineng_icon_bg.png");
	mBg->addChild(skillBoard, 0, Tag_XiLiImg_JiNeng);
	skillBoard->setPosition(ccp(429, 838-643));
	skillBoard->setScale(0.8f);
	BaseSprite *skillImg = BaseSprite::create(
		ImageManager::getManager()->getSkillPicFileName((SkillType)actor->getCurrentUseSkill()->getInfor()->skill_type));
	skillBoard->addChild(skillImg);
	skillImg->setPosition(ccp(skillBoard->getContentSize().width/2, skillBoard->getContentSize().height/2));
	//
	mMainMenu->getChildByTag(Tag_MenuItem_XiLi)->setVisible(false);
	mMainMenu->getChildByTag(Tag_MenuItem_Ok)->setVisible(true);
	((CCMenu*)mBg->getChildByTag(Tag_BoardMenu))->setEnabled(false);
}