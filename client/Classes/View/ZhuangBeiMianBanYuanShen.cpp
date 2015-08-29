#include "ZhuangBeiMianBanYuanShen.h"
#include "EquipmentXiangQingBody.h"
#include "font.h"
#include "ChangeZhuangBeiBody.h"
#include "CommonDefine.h"
#include "GameDirector.h"
#include "LFMessageBox.h"
#include "CS_Common.h"
#include "DegreeUpLayer.h"
#include "MessageDisplay.h"
#include "LevelUpLayer.h"


#define Label_Size 24

enum TAG{
	Tag_MenuItem_HeroList

	, Tag_MenuItem_Wuqi
	, Tag_MenuItem_Dunpai
	, Tag_MenuItem_Kuijia
	, Tag_MenuItem_Hushengfu
	, Tag_MenuItem_Jiezhi1
	, Tag_MenuItem_Jiezhi2

	, Tag_MenuItem_ZhenShen
	, Tag_MenuItem_ShengJie
	, Tag_MenuItem_ShengJi
	, Tag_MenuItem_GongMing

	, Tag_DiWen_Wuqi
	, Tag_DiWen_Dunpai
	, Tag_DiWen_Kuijia
	, Tag_DiWen_Hushengfu
	, Tag_DiWen_Jiezhi1
	, Tag_DiWen_Jiezhi2
	, Tag_JingLianNum_Wuqi
	, Tag_JingLianNum_Dunpai
	, Tag_JingLianNum_Kuijia
	, Tag_JingLianNum_Hushengfu
	, Tag_JingLianNum_Jiezhi1
	, Tag_JingLianNum_Jiezhi2

	, Tag_Label_Xueliang
	, Tag_Label_Gongji
	, Tag_Label_Hujia
	, Tag_Label_Mokang
	, Tag_Label_ZBZhuanHuaLv

	, Tag_Label_Level
	, Tag_ProgressTimer
	, Tag_ProgressTimerBoarder
	, Tag_ExpNum


	, Tag_Img_HeroBg
	, Tag_Board_JiNengBianKuang
	, Tag_Img_FullHero
	, Tag_Img_Xilian
};

#define posWuqi ccp(95, 838-293)
#define posDunpai ccp(95, 838-549)
#define posHushengfu ccp(547, 838-436)
#define posJiezhi1 ccp(547, 838-526)
#define posJiezhi2 ccp(547, 838-615)
#define posKuijia ccp(547, 838-288)

//如果是 EquipClicked就有装备
//若是 AddEquipClicked就为NULL
extern Equipment *g_YuanShiBiJiao_Equip;

extern string getJianYingFileName(Profession prof, int pos, bool isHaveWuqi, bool isHaveDunpai, bool isSS_Wuq);

ZhuangBeiMianBanYuanShen* ZhuangBeiMianBanYuanShen::create(CCArray *arr_YuanShen, int yuanshen_id, yuanshen_comefrom from, ZhuangBeiMianBan_YuanShenBack_Interface *callfunc)
{
	ZhuangBeiMianBanYuanShen* layer = new ZhuangBeiMianBanYuanShen;
	if(layer && layer->init(arr_YuanShen, yuanshen_id, from, callfunc))
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

ZhuangBeiMianBanYuanShen::ZhuangBeiMianBanYuanShen()
{
	g_YuanShiBiJiao_Equip = NULL;
}

ZhuangBeiMianBanYuanShen::~ZhuangBeiMianBanYuanShen()
{
	mArray_YuanShen->release();
	g_YuanShiBiJiao_Equip = NULL;
}

bool ZhuangBeiMianBanYuanShen::init(CCArray *arr_YuanShen, int yuanshen_id, yuanshen_comefrom from, ZhuangBeiMianBan_YuanShenBack_Interface *callfunc)
{
	bool bRet = false;

	do {
		mComeFrom = from;
		arr_YuanShen->retain();
		mArray_YuanShen = arr_YuanShen;
		mCallFunc = callfunc;

		mCurYuanShenIdx = -1;
		for (int i=0; i!=mArray_YuanShen->count(); ++i)
		{
			BaseActor *yuanshen_actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(i));
			CCAssert(yuanshen_actor != NULL, "");
			if (yuanshen_actor->getId() == yuanshen_id)
			{
				mCurYuanShenIdx = i;
				break;
			}
		}
		if (mCurYuanShenIdx == -1)
		{
			CCLog("Error: %s ---> yuanshen id is 0", __FUNCTION__);
			mCurYuanShenIdx = 0;
		}

		// 1. bg
		mBg = BaseSprite::create("renxing_bg.png");
		addChild(mBg);
		mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));

		// 2.  hero menu
		mHeroMenu = CCMenu::create();
		mBg->addChild(mHeroMenu, 10);
		mHeroMenu->setPosition(CCPointZero);
		//
		mMainMenu = CCMenu::create();
		mBg->addChild(mMainMenu, 10);
		mMainMenu->setPosition(CCPointZero);
		{
			//返回真身
			CCMenuItemImage *zhenShen = CCMenuItemImage::create(
				"zhenshen_anniu.png",
				"zhenshen_anniu_select.png",
				this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemClicked_MainMenu));
			mMainMenu->addChild(zhenShen, 1, Tag_MenuItem_ZhenShen);
			zhenShen->setPosition(ccp(550, 838-699));

			//升阶
			CCMenuItemImage *shengJie = CCMenuItemImage::create(
				"shengjie_anniu.png",
				"shengjie_anniu_select.png",
				this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemClicked_MainMenu));
			mMainMenu->addChild(shengJie, 1, Tag_MenuItem_ShengJie);
			shengJie->setPosition(ccp(91, 838-699));
			if (mComeFrom == comefrom_other) shengJie->setVisible(false);

			//升级
			CCMenuItemImage *shengJi = CCMenuItemImage::create(
				"shengji_anniu_1.png",
				"shengji_anniu_1_select.png",
				this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemClicked_MainMenu));
			mMainMenu->addChild(shengJi, 1, Tag_MenuItem_ShengJi);
			shengJi->setPosition(shengJie->getPosition());
			if (mComeFrom == comefrom_other) shengJi->setVisible(false);

			int errorcode;
			BaseActor *actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(mCurYuanShenIdx));
			if (actor == NULL || ! actor->canUpdateStage(errorcode)) shengJie->setVisible(false);
			if (actor == NULL || ! actor->canUpdateLevel(errorcode)) shengJi->setVisible(false);

			//共鸣  元神没有共鸣
// 			CCMenuItemImage *gmItem = CCMenuItemImage::create(
// 				"gongming_anniu.png",
// 				"gongming_anniu_select.png",
// 				this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemClicked_MainMenu));
// 			mMainMenu->addChild(gmItem, 1, Tag_MenuItem_GongMing);
// 			gmItem->setPosition(ccp(242, 838-210));
		}

		// 3. equip menu
// 		this->setMenuTouchedDelayTime(2);
// 		mEquipMenu = MyMenu::create(this);
		mEquipMenu = CCMenu::create();
		mBg->addChild(mEquipMenu, 10);
		mEquipMenu->setPosition(CCPointZero);

		// 
		mHeroView = NULL;

		// 4. hero menu item
		for(int i=0; i!=mArray_YuanShen->count(); ++i)
		{
			BaseActor *actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(i));
			if (actor == NULL) continue;

			BaseSprite* touxiangBg = BaseSprite::create("renxing_renwu_touxiang_bg.png");
			mBg->addChild(touxiangBg);
			touxiangBg->setPosition(ccp(109+140*i, 838-83));

			ImageData data = ImageManager::getManager()->getImageData_ProfessionHead(actor->getActorType());
			BaseSprite* pic = BaseSprite::create(data.getCurrentImageFile());
			touxiangBg->addChild(pic);
			pic->setPosition(ccp(touxiangBg->getContentSize().width/2, touxiangBg->getContentSize().height/2));


			CCMenuItemImage* item;
			if(i == mCurYuanShenIdx)
			{
				item = CCMenuItemImage::create(
					"renxing_renwu_touxiang_select.png"
					, "renxing_renwu_touxiang_select.png"
					, this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemHeroListClicked));
			}else
			{
				item = CCMenuItemImage::create(
					"renxing_renwu_touxiang.png"
					, "renxing_renwu_touxiang.png"
					, this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemHeroListClicked));
			}
			mHeroMenu->addChild(item, 1, Tag_MenuItem_HeroList+i);
			item->setPosition(touxiangBg->getPosition());

		}

		// 5. equip
		refreshEquipmentUI();

		// 6. preperty
		refreshPrepertyLabel();

// 		BaseSprite *menuBar = BaseSprite::create("renxing_anniu_bg.png");
// 		mBg->addChild(menuBar, 9);
// 		menuBar->setPosition(ccp(320, 838-209));
		// 盖住 hero scroll 
		BaseSprite* img = BaseSprite::create("renxing_renwu_biankuang.png");
		mBg->addChild(img, 10, Tag_Img_HeroBg);
		img->setPosition(ccp(319, 838-410));
		//技能
		BaseSprite *jinengBoard = BaseSprite::create("renxing_jineng_bg.png");
		mBg->addChild(jinengBoard, 11, Tag_Board_JiNengBianKuang);
		jinengBoard->setPosition(ccp(321, 838-624));


		bRet = true;
	} while (0);


	return bRet;
}

void ZhuangBeiMianBanYuanShen::refreshEquipmentUI()
{
	BaseActor *actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(mCurYuanShenIdx));
	if (actor == NULL) return;

	mCurYuanShenEquips = actor->getEquipments();

	// 0wuqi  1dunpai  2kaijia  3hushengfu  4jiezhi1  5jiezhi2
	bool isHaveEquip[6];
	for(int i=0; i!=6; ++i)  isHaveEquip[i] = false;

	bool bShuangShou_Wuqi = false;

	// . hero equip menu item
	int jiezhi = 0;
	string xxx;
	mBg->removeChildByTag(Tag_JingLianNum_Dunpai);
	mBg->removeChildByTag(Tag_JingLianNum_Hushengfu);
	mBg->removeChildByTag(Tag_JingLianNum_Jiezhi1);
	mBg->removeChildByTag(Tag_JingLianNum_Jiezhi2);
	mBg->removeChildByTag(Tag_JingLianNum_Kuijia);
	mBg->removeChildByTag(Tag_JingLianNum_Wuqi);

	for (vector<ZhuangbeiOptionLevel *>::iterator iter = m_lOptionList.begin() ; iter != m_lOptionList.end() ; iter ++)
	{
		(*iter)->removeFromParent();
	}
	m_lOptionList.clear();

	for(int i=0; i!=mCurYuanShenEquips.size(); ++i)
	{
		// 1.底纹
		ImageData diwenData;
		BaseSprite* diwen;
		int menuItemTag = -1;
		int TAG_Place; //use to menu select image
		switch (mCurYuanShenEquips.at(i)->getType())
		{
		case ZhuangbeiType_Dunpai:
		case ZhuangbeiType_Faqi:
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_DunPai,
				mCurYuanShenEquips.at(i)->getEquipColor(), 
				false,//mCurYuanShenEquips.at(i)->isShenBing(),
				mCurYuanShenEquips.at(i)->isPartOfSuit(),
				mCurYuanShenEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Dunpai);
			diwen->setPosition(posDunpai);
			diwen->setScale(diwenData.scale);
			menuItemTag = Tag_MenuItem_Dunpai;
			TAG_Place = TAG_DunPai;
			isHaveEquip[1] = true;
			//强化和精炼
			if (mCurYuanShenEquips.at(i)->canBeJingLian(xxx) && mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
				char temp[16];
				sprintf(temp,"+%d",mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Dunpai);
				lableNum->setPosition(ccp(155,387));
			}
			if (mCurYuanShenEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurYuanShenEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		case ZhuangbeiType_Hushenfu:
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_HuShenFu,
				mCurYuanShenEquips.at(i)->getEquipColor(),
				false,//mCurYuanShenEquips.at(i)->isShenBing(),
				mCurYuanShenEquips.at(i)->isPartOfSuit(),
				mCurYuanShenEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Hushengfu);
			diwen->setScale(diwenData.scale);
			diwen->setPosition(posHushengfu);
			menuItemTag = Tag_MenuItem_Hushengfu;
			TAG_Place = TAG_HuShenFu;
			isHaveEquip[3] = true;
			//强化和精炼
			if (mCurYuanShenEquips.at(i)->canBeJingLian(xxx) && mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
				//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Hushengfu);
				//				mBg->addChild(icon, 20, Tag_JingLianBar_Hushengfu);
				//				icon->setPositionY(432);
				lableNum->setPosition(ccp(583,432));
				//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}
			if (mCurYuanShenEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurYuanShenEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		case ZhuangbeiType_Jiezhi:
			++jiezhi;
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_JieZhi,
				mCurYuanShenEquips.at(i)->getEquipColor(), 
				false,//mCurYuanShenEquips.at(i)->isShenBing(),
				mCurYuanShenEquips.at(i)->isPartOfSuit(),
				mCurYuanShenEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Jiezhi1+jiezhi-1);
			diwen->setScale(diwenData.scale);
			TAG_Place = TAG_JieZhi;
			if(jiezhi == 1)
			{
				diwen->setPosition(posJiezhi1);
				menuItemTag = Tag_MenuItem_Jiezhi1;
				isHaveEquip[4] = true;
				//强化和精炼
				if (mCurYuanShenEquips.at(i)->canBeJingLian(xxx) && mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu() >= 1)
				{
					//					BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
					char temp[16];
					sprintf(temp,"+%d",mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu());
					CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
					lableNum->setAnchorPoint(ccp(1.0f,0.5f));
					mBg->addChild(lableNum, 20, Tag_JingLianNum_Jiezhi1);
					//					mBg->addChild(icon, 20, Tag_JingLianBar_Jiezhi1);
					//					icon->setPositionY(340);
					lableNum->setPosition(ccp(583,340));
					//					icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
				}
			}
			else 
			{
				diwen->setPosition(posJiezhi2);
				menuItemTag = Tag_MenuItem_Jiezhi2;
				isHaveEquip[5] = true;
				//强化和精炼
				if (mCurYuanShenEquips.at(i)->canBeJingLian(xxx) && mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu() >= 1)
				{
					//					BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
					char temp[16];
					sprintf(temp,"+%d",mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu());
					CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
					lableNum->setAnchorPoint(ccp(1.0f,0.5f));
					mBg->addChild(lableNum, 20, Tag_JingLianNum_Jiezhi2);
					//					mBg->addChild(icon, 20, Tag_JingLianBar_Jiezhi2);
					//					icon->setPositionY(250);
					lableNum->setPosition(ccp(583,250));
					//					icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
				}
			}
			if (mCurYuanShenEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurYuanShenEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		case ZhuangbeiType_KuijiaQin:
		case ZhuangbeiType_KuijiaZhong:
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_Kuijia,
				mCurYuanShenEquips.at(i)->getEquipColor(), 
				false,//mCurYuanShenEquips.at(i)->isShenBing(),
				mCurYuanShenEquips.at(i)->isPartOfSuit(),
				mCurYuanShenEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Kuijia);
			diwen->setScale(diwenData.scale);
			diwen->setPosition(posKuijia);
			menuItemTag = Tag_MenuItem_Kuijia;
			TAG_Place = TAG_Kuijia;
			isHaveEquip[2] = true;
			//强化和精炼
			if (mCurYuanShenEquips.at(i)->canBeJingLian(xxx) && mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
				//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Kuijia);
				//				mBg->addChild(icon, 20, Tag_JingLianBar_Kuijia);
				//				icon->setPositionY(626);
				lableNum->setPosition(ccp(605,625));
				//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}
			if (mCurYuanShenEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurYuanShenEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		case ZhuangbeiType_DanshouJian:
		case ZhuangbeiType_ShuangshouBishou:
		case ZhuangbeiType_ShuangshouFu:
		case ZhuangbeiType_DanshouFazhang:
		case ZhuangbeiType_Gong:
		case ZhuangbeiType_ShuangshouFazhang:
			//wuqi
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_WuQi,
				mCurYuanShenEquips.at(i)->getEquipColor(), 
				false,//mCurYuanShenEquips.at(i)->isShenBing(),
				mCurYuanShenEquips.at(i)->isPartOfSuit(),
				mCurYuanShenEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Wuqi);
			diwen->setScale(diwenData.scale);
			diwen->setPosition(posWuqi);
			menuItemTag = Tag_MenuItem_Wuqi;
			TAG_Place = TAG_WuQi;
			isHaveEquip[0] = true;
			//强化和精炼
			if (mCurYuanShenEquips.at(i)->canBeJingLian(xxx) && mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
				//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurYuanShenEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Wuqi);
				//				mBg->addChild(icon, 20, Tag_JingLianBar_Wuqi);
				//				icon->setPositionY(643);
				lableNum->setPosition(ccp(154,643));
				//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}

			//fushou 
			if(CS::isShuangShouWuQi(mCurYuanShenEquips.at(i)->getType()))
			{
				bShuangShou_Wuqi = true;
				//底纹
				BaseSprite* fushouDiwen = BaseSprite::create("mianban_renwu_zhuangbei_fushou_diban.png");
				mBg->addChild(fushouDiwen, 1, Tag_DiWen_Dunpai);
				fushouDiwen->setScale(diwenData.scale);
				fushouDiwen->setPosition(posDunpai);
				//装备
				BaseSprite* fushouZhuangBei = ImageManager::getManager()->getSpriteOfZhuangbei(mCurYuanShenEquips.at(i));
				fushouDiwen->addChild(fushouZhuangBei);
				fushouZhuangBei->setColor(ccc3(185,117,117));
				fushouZhuangBei->setPosition(ccp(fushouDiwen->getContentSize().width/2, fushouDiwen->getContentSize().height/2));
				fushouZhuangBei->setScale(ImageManager::getManager()->getImageData_ZhuangBei_MianBan(mCurYuanShenEquips.at(i)).scale);
				//按钮
				CCSprite* imgNormal = CCSprite::create(
					ImageManager::getManager()->getImageData_DiWen_Selected(
					mCurYuanShenEquips.at(i)->getEquipColor(),
					false,//mCurYuanShenEquips.at(i)->isShenBing(),
					mCurYuanShenEquips.at(i)->isPartOfSuit()).getCurrentImageFile().c_str());
				imgNormal->setOpacity(0);
				CCSprite* imgSelected = CCSprite::create(
					ImageManager::getManager()->getImageData_DiWen_Selected(
					mCurYuanShenEquips.at(i)->getEquipColor(),
					false,//mCurYuanShenEquips.at(i)->isShenBing(),
					mCurYuanShenEquips.at(i)->isPartOfSuit()
					).getCurrentImageFile().c_str());
				imgSelected->setOpacity(30);
				CCMenuItemSprite* menuItem = CCMenuItemSprite::create(
					imgNormal
					, imgSelected
					, this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemEquipClicked));
				mEquipMenu->addChild(menuItem, 1, Tag_MenuItem_Dunpai);
				menuItem->setPosition(fushouDiwen->getPosition());
				menuItem->setScaleX(fushouDiwen->getContentSize().width / menuItem->getContentSize().width);
				menuItem->setScaleY(fushouDiwen->getContentSize().height / menuItem->getContentSize().height);
				menuItem->setUserData(mCurYuanShenEquips.at(i));

				isHaveEquip[1] = true;  

			}
			if (mCurYuanShenEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurYuanShenEquips.at(i));
				mBg->addChild(layer,20,Tag_Img_Xilian);
				CCPoint pos = diwen->getPosition();
				pos.x -= diwen->getContentSize().width * diwen->getScaleX() / 2;
				pos.y -= diwen->getContentSize().height * diwen->getScaleY() / 2;

				int width = pos.x + layer->getContentSize().width / 2 + 10;
				int height = pos.y + layer->getContentSize().height / 2;
				layer->setPosition(ccp(width,height));
				m_lOptionList.push_back(layer);
			}
			break;
		default:
			CCLog("Error: ZhuangBeiMianBanBase::refreshEquipmentUI ---> default zhuangbei type " );
			break;
		}

		// 2.装备
		ImageData equipData = ImageManager::getManager()->getImageData_ZhuangBei_MianBan(mCurYuanShenEquips.at(i));
		BaseSprite* zhuangbei = ImageManager::getManager()->getSpriteOfZhuangbei(mCurYuanShenEquips.at(i),false,true);
		zhuangbei->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
		zhuangbei->setScale(equipData.scale);
		diwen->addChild(zhuangbei);

		// 3.按钮
		CCSprite* imgNormal = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
			TAG_Place,
			mCurYuanShenEquips.at(i)->getEquipColor(),
			false,//mCurYuanShenEquips.at(i)->isShenBing(),
			mCurYuanShenEquips.at(i)->isPartOfSuit()
			).getCurrentImageFile().c_str());
		imgNormal->setOpacity(0);
		CCSprite* imgSelected = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
			TAG_Place,
			mCurYuanShenEquips.at(i)->getEquipColor(),
			false,//mCurYuanShenEquips.at(i)->isShenBing(),
			mCurYuanShenEquips.at(i)->isPartOfSuit()
			).getCurrentImageFile().c_str());
		CCMenuItemSprite* menuItem = CCMenuItemSprite::create(
			imgNormal
			, imgSelected
			, this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemEquipClicked));
		mEquipMenu->addChild(menuItem, 1, menuItemTag);
		menuItem->setPosition(diwen->getPosition());
		menuItem->setScaleX(diwen->getContentSize().width / menuItem->getContentSize().width);
		menuItem->setScaleY(diwen->getContentSize().height / menuItem->getContentSize().height);
		menuItem->setUserData(mCurYuanShenEquips.at(i));
	}

	bool isOpend[5];
	CS::openZhuangbeiOfYuanshenLevel(actor->getCurrentProperty().level, isOpend[0], isOpend[1], isOpend[2], isOpend[3], isOpend[4]);
	// . 没有装备的地方
	for(int i=0; i!=6; ++i)
	{// 0wuqi  1dunpai  2kaijia  3hushengfu  4jiezhi1  5jiezhi2
		if(isHaveEquip[i])
			continue;

		BaseSprite *jianyingImg = NULL;
		int menuItemTag = -1;
		int diwen_tag = -1;
		CCPoint pos;
		CCSize size;

		bool isOpen = false;
		int TAG_Place;
		switch (i)
		{
		case 0:
			menuItemTag = Tag_MenuItem_Wuqi;
			pos = posWuqi;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_WuQi,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(actor->getActorType()
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Wuqi;
			TAG_Place = TAG_WuQi;
			if (isOpend[0]) isOpen = true;
			break;
		case 1:
			menuItemTag = Tag_MenuItem_Dunpai;
			pos = posDunpai;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_DunPai,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(actor->getActorType()
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Dunpai;
			TAG_Place = TAG_DunPai;
			if (isOpend[1]) isOpen = true;
			break;
		case 2:
			menuItemTag = Tag_MenuItem_Kuijia;
			pos = posKuijia;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_Kuijia,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(actor->getActorType()
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Kuijia;
			TAG_Place = TAG_Kuijia;
			if (isOpend[2]) isOpen = true;
			break;
		case 3:
			menuItemTag = Tag_MenuItem_Hushengfu;
			pos = posHushengfu;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_HuShenFu,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(actor->getActorType()
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Hushengfu;
			TAG_Place = TAG_HuShenFu;
			if (isOpend[3]) isOpen = true;
			break;
		case 4:
			menuItemTag = Tag_MenuItem_Jiezhi1;
			pos = posJiezhi1;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_JieZhi,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(actor->getActorType()
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Jiezhi1;
			TAG_Place = TAG_JieZhi;
			if (isOpend[4]) isOpen = true;
			break;
		case 5:
			menuItemTag = Tag_MenuItem_Jiezhi2;
			pos = posJiezhi2;
			size = BaseSprite::create(ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_JieZhi,ZhuangbeiColour_Fumo,false,false,false).getCurrentImageFile())->getContentSize();
			jianyingImg = BaseSprite::create(getJianYingFileName(actor->getActorType()
				, menuItemTag, isHaveEquip[0], isHaveEquip[1], bShuangShou_Wuqi));
			diwen_tag = Tag_DiWen_Jiezhi2;
			TAG_Place = TAG_JieZhi;
			if (isOpend[4]) isOpen = true;
			break;
		default:
			break;
		}

		//装备剪影
		mBg->addChild(jianyingImg, 1, diwen_tag);
		jianyingImg->setPosition(pos);

		//按钮
		CCSprite* imgNormal = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
			TAG_Place,ZhuangbeiColour_Fumo,false,false).getCurrentImageFile().c_str());
		imgNormal->setOpacity(0);
		CCSprite* imgSelected = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
			TAG_Place,ZhuangbeiColour_Fumo,false,false).getCurrentImageFile().c_str());
		imgSelected->setOpacity(0);
		CCMenuItemSprite* menuItem = CCMenuItemSprite::create(
			imgNormal
			, imgSelected
			, this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemAddEquipClicked));
		mEquipMenu->addChild(menuItem, 1, menuItemTag);
		menuItem->setPosition(pos);
		menuItem->setScaleX(size.width / menuItem->getContentSize().width);
		menuItem->setScaleY(size.height / menuItem->getContentSize().height);

		//mianban  other
		if (mComeFrom == comefrom_other)
			menuItem->setEnabled(false);
		//槽位锁
		if (! isOpen)
		{
			menuItem->setEnabled(false);
			BaseSprite *suoImg = BaseSprite::create("mianban_caowei_suoding.png");
			jianyingImg->addChild(suoImg);
			suoImg->setPosition(ccp(jianyingImg->getContentSize().width/2, jianyingImg->getContentSize().height/2));
		}
	}

}

void ZhuangBeiMianBanYuanShen::refreshPrepertyLabel()
{
	BaseActor *actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(mCurYuanShenIdx));
	if (actor == NULL) return;

	//转化率
	CCString temp;
	temp.initWithFormat("%s%u%%", LFStrings::getValue("ZB_ZhuanHuaLv").c_str()
		, CS::transferRatioOfYuanshenLevel(actor->getCurrentProperty().level));
	CCLabelTTF* zhl = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_ZBZhuanHuaLv));
	if (zhl == NULL)
	{
		zhl = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, Label_Size);
		mBg->addChild(zhl, 12, Tag_Label_ZBZhuanHuaLv);
		zhl->setAnchorPoint(ccp(0, 0.5f));
		zhl->setPosition(ccp(202, 838-630));
	}
	zhl->setString(temp.getCString());

	char buf[20];
	//xueliang
	sprintf(buf, "%.1f", actor->getXueliang());
	CCLabelTTF* xueliang = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Xueliang));
	if (xueliang == NULL)
	{
		xueliang = CCLabelTTF::create("", fontStr_kaiti, Label_Size);
		mBg->addChild(xueliang, 1, Tag_Label_Xueliang);
		xueliang->setAnchorPoint(ccp(0,0.5));
		xueliang->setPosition(ccp(116, 838-783));
	}
	xueliang->setString(buf);
	//gongji
	PropertyCalculator prop;
	sprintf(buf, "%.1f", actor->getGongji_MacroValue());
	CCLabelTTF* gongji = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Gongji));
	if (gongji == NULL)
	{
		gongji = CCLabelTTF::create("", fontStr_kaiti, Label_Size);
		mBg->addChild(gongji, 1, Tag_Label_Gongji);
		gongji->setAnchorPoint(ccp(0,0.5));
		gongji->setPosition(ccp(306, 838-783));
	}
	gongji->setString(buf);
	//hujia
	sprintf(buf, "%.1f", prop.calculateHuJiaZhi_MacroValue(actor->getCalculateData()));
	CCLabelTTF* hujia = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Hujia));
	if (hujia == NULL)
	{
		hujia = CCLabelTTF::create("", fontStr_kaiti, Label_Size);
		mBg->addChild(hujia, 1, Tag_Label_Hujia);
		hujia->setAnchorPoint(ccp(0,0.5));
		hujia->setPosition(ccp(496, 838-783));
	}
	hujia->setString(buf);
	//mokang
// 	sprintf(buf, "%.1f", actor->getMokang());
// 	CCLabelTTF* mokang = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Mokang));
// 	if (mokang == NULL)
// 	{
// 		mokang = CCLabelTTF::create("", fontStr_kaiti, 26);
// 		mBg->addChild(mokang, 1, Tag_Label_Mokang);
// 		mokang->setAnchorPoint(ccp(0,0.5));
// 		mokang->setPosition(ccp(538, 838-783));
// 	}
// 	mokang->setString(buf);
	//lv
	sprintf(buf, "Lv%d", actor->getCurrentProperty().level);
	CCLabelTTF* lv = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Level));
	if (lv == NULL)
	{
		lv = CCLabelTTF::create(buf, fontStr_kaiti, Label_Size);
		mBg->addChild(lv, 11, Tag_Label_Level);
		lv->setAnchorPoint(ccp(0,0.5));
		if (mComeFrom == comefrom_base)
			lv->setPosition(ccp(192, 838-560));
		else 
			lv->setPosition(ccp(192, 838-576));
	}
	lv->setString(buf);

	//经验条
	if (mComeFrom == comefrom_base)
	{
		CCProgressTimer* jinYanBar = (CCProgressTimer*)mBg->getChildByTag(Tag_ProgressTimer);
		BaseSprite* jinYanBarBoarder = (BaseSprite*)mBg->getChildByTag(Tag_ProgressTimerBoarder);
		if (jinYanBarBoarder == NULL && jinYanBar == NULL)
		{
			jinYanBarBoarder = BaseSprite::create("zhuye_jinyan_bg.png");
			mBg->addChild(jinYanBarBoarder, 1, Tag_ProgressTimerBoarder);
			jinYanBarBoarder->setPosition(ccp(lv->getPositionX()+jinYanBarBoarder->getContentSize().width/2-4, 838-582));

			jinYanBar = LF::createProgressTimer(BaseSprite::create("zhuye_jinyan.png"));
			mBg->addChild(jinYanBar, 1, Tag_ProgressTimer);
			jinYanBar->setPosition(jinYanBarBoarder->getPosition());
		}
		{
			float curExp = actor->getCurrentProperty().exp_current;
			float needExp = actor->getCurrentProperty().exp_need;
			jinYanBar->setPercentage(curExp/needExp * 100.0f);
			//

			//num
			{
				mBg->removeChildByTag(Tag_ExpNum);
				CCString temp;
				temp.initWithFormat("%u/%u", actor->getCurrentProperty().exp_current
					, actor->getCurrentProperty().exp_need);
				CCLabelTTF *tiliLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 15);
				//tiliLabel->setOpacity(255/2);
				//tiliLabel->setAnchorPoint(ccp(0, 0.5));
				tiliLabel->setPosition(this->convertToNodeSpace(
					//jinYanBarBoarder->convertToWorldSpace(ccp(8, 11))));
					jinYanBarBoarder->convertToWorldSpace(ccp(jinYanBarBoarder->getContentSize().width/2, jinYanBarBoarder->getContentSize().height/2))));
				mBg->addChild(tiliLabel, 12, Tag_ExpNum);
			}
		}
	}

//	int errorcode;
	BaseActorProperty propf = actor->getCurrentProperty();
	if (actor->getLevel() % 5 == 0 && propf.exp_current >= propf.exp_need && mComeFrom == comefrom_base)
		mMainMenu->getChildByTag(Tag_MenuItem_ShengJie)->setVisible(true);
	else 
		mMainMenu->getChildByTag(Tag_MenuItem_ShengJie)->setVisible(false);

	if (actor->getLevel() % 5 != 0 && propf.exp_current >= propf.exp_need && mComeFrom == comefrom_base) 
		mMainMenu->getChildByTag(Tag_MenuItem_ShengJi)->setVisible(true);
	else
		mMainMenu->getChildByTag(Tag_MenuItem_ShengJi)->setVisible(false);
}

void ZhuangBeiMianBanYuanShen::refreshHerosImgScrollView()
{
	mHeroView->prepareToDisplay();
	mHeroView->scrollToPageSimple(mCurYuanShenIdx, 0);
}

void ZhuangBeiMianBanYuanShen::addHeroScrollView()
{
	mHeroView = new GridScrollView(
		CCSizeMake(288,531),
		mArray_YuanShen->count(),		// 元素的个数
		1,								// 每页横向显示几个
		1,								// 每页纵向显示几个
		10.0f,
		CCPointZero,
		20.0f);


	mBg->addChild(mHeroView, 5);
	mHeroView->release();
	mHeroView->setClickableCreater(this);
	// 可以显示了
	mHeroView->prepareToDisplay();
	mHeroView->setPosition(ccp(178, 838-660));
	mHeroView->scrollToPageSimple(mCurYuanShenIdx, 0);

}

ClickableItem* ZhuangBeiMianBanYuanShen::createGridItem(int pageIndex,int xIndex,int yIndex)
{
	BaseActor *actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(pageIndex));
	if (actor == NULL) return NULL;
	//元神不是用底座
	ScrollYuanShenNode* node = new ScrollYuanShenNode(
		ImageManager::getManager()->getFullPhotoOfActor(actor,false), actor, mFooterLayer, mComeFrom);
	node->autorelease();
	node->setScale(1.2f);
	return node;
}
// 页面选择了
void ZhuangBeiMianBanYuanShen::pageSelectedEnd(int pageIndex)
{
	menuItemHeroListClicked( mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+pageIndex) );
}

void ZhuangBeiMianBanYuanShen::menuItemHeroListClicked(CCObject* pSender)
{
	CCMenuItemImage* oldMenuItem = (CCMenuItemImage*)mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+mCurYuanShenIdx);
	oldMenuItem->initWithNormalImage(
		"renxing_renwu_touxiang.png"
		, "renxing_renwu_touxiang.png"
		, NULL, this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemHeroListClicked));

	mCurYuanShenIdx = ((CCMenuItemImage*)pSender)->getTag() - Tag_MenuItem_HeroList;
	CCMenuItemImage* curMenuItem = (CCMenuItemImage*)mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+mCurYuanShenIdx);
	curMenuItem->initWithNormalImage(
		"renxing_renwu_touxiang_select.png"
		, "renxing_renwu_touxiang_select.png"
		, NULL, this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemHeroListClicked));

	mBg->removeChildByTag(Tag_DiWen_Wuqi);
	mBg->removeChildByTag(Tag_DiWen_Hushengfu);
	mBg->removeChildByTag(Tag_DiWen_Jiezhi1);
	mBg->removeChildByTag(Tag_DiWen_Jiezhi2);
	mBg->removeChildByTag(Tag_DiWen_Kuijia);
	mBg->removeChildByTag(Tag_DiWen_Dunpai);
	mEquipMenu->removeAllChildren();
	refreshEquipmentUI();
	refreshPrepertyLabel();
	mHeroView->scrollToPageSimple(mCurYuanShenIdx, 0);
}

void ZhuangBeiMianBanYuanShen::menuItemEquipClicked(CCObject* pSender)
{
	Equipment* equip = (Equipment*)((CCMenuItemImage*)pSender)->getUserData();

	EquipmentXiangQingBody* q = NULL;
	if (mComeFrom == comefrom_base)
		q = new EquipmentXiangQingBody(equip, equip->getActorId(), From_RenXingMianBan);
	else 
		q = new EquipmentXiangQingBody(equip, equip->getActorId(), From_OtherHeroDetail);
	mFooterLayer->popUpContaintLayer(q, Pop_Pos_WondowDown);
	q->release();

	g_YuanShiBiJiao_Equip = equip;
}

void ZhuangBeiMianBanYuanShen::menuItemAddEquipClicked(CCObject* pSender)
{
	g_YuanShiBiJiao_Equip = NULL;
	if (mComeFrom == comefrom_other) return;

	BaseActor *actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(mCurYuanShenIdx));
	if (actor == NULL) return ;

	int tag = ((CCMenuItemSprite*)pSender)->getTag();
	EquipRequirePlace place;
	switch (tag)
	{
	case Tag_MenuItem_Wuqi:
		place = POS_ZhuWuQi;
		break;
	case Tag_MenuItem_Dunpai:
		place = POS_FuShou;
		break;
	case Tag_MenuItem_Kuijia:
		place = POS_XiongJia;
		break;
	case Tag_MenuItem_Hushengfu:
		place = POS_Hushenfu;
		break;
	case Tag_MenuItem_Jiezhi1:
		place = POS_JieZhi1;
		break;
	case Tag_MenuItem_Jiezhi2:
		place = POS_JieZhi2;
		break;
	default:
		break;
	}
	ChangeZhuangBeiBody* change = new ChangeZhuangBeiBody(
		actor->getId(), place);
	mFooterLayer->popUpContaintLayer(change);
	change->setFooter(mFooterLayer);

	change->release();
}

void ZhuangBeiMianBanYuanShen::disableAllTouchBegin()
{
	mHeroMenu->setEnabled(false);
	mEquipMenu->setEnabled(false);
	mHeroView->setTouchEnabled(false);
	mMainMenu->setEnabled(false);
}

void ZhuangBeiMianBanYuanShen::disableAllTouchEnd()
{
	refreshLayer();

	mHeroMenu->setEnabled(true);
	mEquipMenu->setEnabled(true);
	mMainMenu->setEnabled(true);
	if (mHeroView != NULL)
	{
		mHeroView->setTouchEnabled(true);
	}

}

vector<EquipData> ZhuangBeiMianBanYuanShen::getEquipsDataByActorId(unsigned int actorId)
{
	vector<EquipData> vec;

	for (int i=0; i!=mCurYuanShenEquips.size(); ++i)
	{
		vec.push_back(mCurYuanShenEquips.at(i)->getEquipData());
	}

	return vec;
}

void ZhuangBeiMianBanYuanShen::refreshLayer()
{
	menuItemHeroListClicked( mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+mCurYuanShenIdx) );
}


void ZhuangBeiMianBanYuanShen::setChangeZhuangBei(TempEquipZhuangbei data, Equipment* equip)
{
	reqData = data;
	mChangeEquip = equip;
	addObserver(SEL_CallFuncO(&ZhuangBeiMianBanYuanShen::changeZhuangBeiRsp),MSG_equipZhuangbeiRsp);
	// 网络请求
	mCmdHlp->cmdEquipZhuangBei(data);
}

void ZhuangBeiMianBanYuanShen::changeZhuangBeiRsp(CCObject* msg_rsp)
{
	removeObserver(MSG_equipZhuangbeiRsp);
	SPCmd_EquipZhuangbeiRsp* rsp = (SPCmd_EquipZhuangbeiRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		GameDirector::getDirector()->hideWaiting();
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	// 数据层会做装备的脱+装
	//// 脱装备的数据
	//{
	//	if (mChangeEquip->getActorId() != INVALID_ID)
	//	{
	//		MyselfManager::getManager()->removeEquipementFromOwner(mChangeEquip);
	//	}
	//}
	//// 移除需要替换的，并装上新的
	//{
	//	BaseActor* actor = mHeros.at(mCurHeroIdx);
	//	CCAssert(actor!=NULL,"");
	//	// 移除需要被替换掉的
	//	vector<Equipment*> needTakeOff = actor->getNeedTakeOffIfWareOnEqui(mChangeEquip->getType());
	//	int takeOffNum = needTakeOff.size();
	//	for (int i=0;i<takeOffNum;i++)
	//	{
	//		MyselfManager::getManager()->removeEquipementFromOwner(needTakeOff[i]);
	//	}
	//	// 装上
	//	MyselfManager::getManager()->attachEquipment(mChangeEquip,actor->getId());
	//}

	refreshHerosImgScrollView();
	refreshLayer();
	GameDirector::getDirector()->hideWaiting();
	CCLog("reportChangeZhuangBeiOK ----> Success");
}

void ZhuangBeiMianBanYuanShen::removeZhuangBeiRsp(CCObject* msg_rsp)
{
	removeObserver(MSG_equipZhuangbeiRsp);
	SPCmd_EquipZhuangbeiRsp* rsp = (SPCmd_EquipZhuangbeiRsp*)((MSG_Rsp*)msg_rsp)->getData();
	if (rsp->err_code != Success)
	{
		GameDirector::getDirector()->hideWaiting();
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	// 脱装备的数据
	{
		if (mChangeEquip->getActorId() != INVALID_ID)
		{
			MyselfManager::getManager()->removeEquipementFromOwner(mChangeEquip);
		}
	}

	refreshHerosImgScrollView();
	refreshLayer();
	GameDirector::getDirector()->hideWaiting();
}

void ZhuangBeiMianBanYuanShen::menuItemLongTimeTouched(CCObject* pSender)
{
	CCLog("============================long touch===========================");
	if (mComeFrom == comefrom_other) return;

	Equipment* equip = (Equipment*)((CCMenuItemImage*)pSender)->getUserData();
	if(equip == NULL) //没有装备的地方
		return;

	BaseActor* actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(mCurYuanShenIdx));
	if (actor == NULL) return;

	// 不在自己身上？！！
	if (equip->getActorId() != actor->getId())
	{
		CCLog("Error: ZhuangBeiMianBanBase::menuItemLongTimeTouched ---> current hero is not the equip owner");
		refreshLayer();
		return;
	}


	TempEquipZhuangbei requestData;
	// 脱装备的数据
	{
		if (equip->getActorId() != INVALID_ID)
		{
			TempHeroAndZhuangbei heroData;
			heroData.hero_id = actor->getId();
			int equipNum = actor->getEquipments().size();
			for (int i=0;i<equipNum;i++)
			{
				// 要脱掉的就不要加入
				if (actor->getEquipments()[i]->getEquipId() == equip->getEquipId())
				{
					continue;
				}

				heroData.zb_id_arr.push_back(actor->getEquipments()[i]->getEquipId());
			}
			requestData.hero_zb_arr.push_back(heroData);
		}
		else
		{
			CCLog("Error: ZhuangBeiMianBanBase::menuItemLongTimeTouched ---> equip id error!");
			refreshLayer();
			return;
		}
	}

	mChangeEquip = equip;
	// 监听 和发送
	addObserver(SEL_CallFuncO(&ZhuangBeiMianBanYuanShen::removeZhuangBeiRsp),MSG_equipZhuangbeiRsp);
	// 网络请求
	mCmdHlp->cmdEquipZhuangBei(requestData);
}

void ZhuangBeiMianBanYuanShen::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_ShengJie)
	{
	// 		LevelUpLayer *layer = LevelUpLayer::create(mHeros.at(mCurHeroIdx)->getId());
	// 		if (layer)
	// 			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 150);
		if (mComeFrom == comefrom_other) return;

		BaseActor *actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(mCurYuanShenIdx));
		if (actor == NULL) return;

		DegreeUpLayer *layer = DegreeUpLayer::create(actor->getId(), this);
		if (layer)
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 150);
	}
	else if (tag == Tag_MenuItem_ShengJi)
	{
		if (mComeFrom == comefrom_other) return;

		BaseActor *actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(mCurYuanShenIdx));
		if (actor == NULL) return;

		LevelUpLayer *layer = LevelUpLayer::create(actor->getId(), this);
		if (layer)
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 150);
	}
	else if (tag == Tag_MenuItem_ZhenShen)
	{
//		ClickableItemCreater *layer = dynamic_cast<ClickableItemCreater*>(mFooterLayer->getTopLayer(-1));
//		if (layer != NULL) layer->pageSelectedEnd(mCurYuanShenIdx);

		BaseActor *actor = dynamic_cast<BaseActor*>(mArray_YuanShen->objectAtIndex(mCurYuanShenIdx));
		mCallFunc->yuanshen_backFunc(actor->getId());
		mFooterLayer->dismmisPopUp();
	}
	//元神没有共鸣
// 	else if (tag == Tag_MenuItem_GongMing)
// 	{
// 		//
// 		this->disableAllTouchBegin();
// 		//
// 		BaseSprite *layer = BaseSprite::create("zhandou_jiugongge_touming.png");
// 		layer->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
// 		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 128, Tag_GongMingLayer);
// 		//
// 		CCMenu *menu = CCMenu::create();
// 		layer->addChild(menu);
// 		menu->setPosition(ccp(0,0));
// 		CCMenuItemImage *item = CCMenuItemImage::create(
// 			"zhandou_jiugongge_touming.png",
// 			"zhandou_jiugongge_touming.png",
// 			this, menu_selector(ZhuangBeiMianBanYuanShen::menuItemClicked_BackFromGongMing));
// 		menu->addChild(item);
// 		item->setOpacity(0);
// 		item->setPosition(ccp(layer->getContentSize().width/2, layer->getContentSize().height/2));
// 		//
// 		BaseSprite *board = BaseSprite::create("gongming_bg.png");
// 		board->setPosition(ccp(layer->getContentSize().width/2, layer->getContentSize().height/2));
// 		layer->addChild(board, 10);
// 	}
}
// 
// void ZhuangBeiMianBanYuanShen::menuItemClicked_BackFromGongMing(CCObject *pSender)
// {
// 	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_GongMingLayer);
// 	this->disableAllTouchEnd();
// }