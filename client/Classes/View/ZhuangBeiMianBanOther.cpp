
#include "ZhuangBeiMianBanOther.h"
#include "EquipmentXiangQingBody.h"
#include "font.h"
#include "ChangeZhuangBeiBody.h"
#include "CommonDefine.h"
#include "GameDirector.h"
#include "LFMessageBox.h"
#include "CS_Common.h"
#include "PropertyCalculator.h"
#include "SuitPool.h"
#include "ShenBingPool.h"
#include "ZhuangBeiMianBanYuanShen.h"
#include "MessageDisplay.h"
#include "flat.h"
#include "ZhuangbeiGongMingManager.h"
#include "FX_CommonFunc.h"
#include "ZhuangbeiOptionLevel.h"
using namespace FX_CommonFunc;


#define  Tag_GongMingLayer  555

#define Label_Size 24

enum TAG{
	  Tag_MenuItem_HeroList

	  , Tag_MenuItem_Wuqi
	  , Tag_MenuItem_Dunpai
	  , Tag_MenuItem_Kuijia
	  , Tag_MenuItem_Hushengfu
	  , Tag_MenuItem_Jiezhi1
	  , Tag_MenuItem_Jiezhi2

	  , Tag_MenuItem_YuanShen
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

	, Tag_Label_Level


	, Tag_Img_HeroBg
	, Tag_Img_FullHero

	, Tag_FanhuiMenu
	, Tag_Img_Xilian

};

#define posWuqi ccp(95, 838-293)
#define posDunpai ccp(95, 838-549)
#define posHushengfu ccp(547, 838-436)
#define posJiezhi1 ccp(547, 838-526)
#define posJiezhi2 ccp(547, 838-615)
#define posKuijia ccp(547, 838-288)


ZhuangBeiMianBanOther* ZhuangBeiMianBanOther::create(CCArray *actors, CCArray *willDelYuanShen, vector<Equipment*> willDelEquip, int idx)
{
	ZhuangBeiMianBanOther* layer = new ZhuangBeiMianBanOther;
	if(layer && layer->init(actors, willDelYuanShen, willDelEquip, idx))
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

bool ZhuangBeiMianBanOther::init(CCArray *actors, CCArray *willDelYuanShen, vector<Equipment*> willDelEquip, int idx)
{
	bool bRet = false;

	do {
		/////0  data
		mCurHeroIdx = idx;
		mHeroActors = actors;
		mHeroActors->retain();
		mWillDeleteActors_YuanShen = willDelYuanShen;
		mWillDeleteActors_YuanShen->retain();
		mWillDeleteEquip = willDelEquip;

		BaseActor *actor = dynamic_cast<BaseActor*>(mHeroActors->objectAtIndex(mCurHeroIdx));
		mCurEquips = actor->getEquipments();
		//////

		// 1. bg
		mBg = BaseSprite::create("renxing_bg.png");
		addChild(mBg);
		mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

		// 2.  hero menu
		mHeroMenu = CCMenu::create();
		mBg->addChild(mHeroMenu, 10);
		mHeroMenu->setPosition(CCPointZero);

		// 3. equip menu
		mEquipMenu = CCMenu::create();
		mBg->addChild(mEquipMenu, 10);
		mEquipMenu->setPosition(CCPointZero);

		//
		mHeroView = NULL;

		//
		mMainMenu = CCMenu::create();
		mBg->addChild(mMainMenu, 11);
		mMainMenu->setPosition(CCPointZero);
		{
			CCMenuItemImage *yuanShen = CCMenuItemImage::create(
				"yuanshen_anniu.png",
				"yuanshen_anniu_select.png",
				this, menu_selector(ZhuangBeiMianBanOther::menuItemClicked_MainMenu));
			mMainMenu->addChild(yuanShen, 1, Tag_MenuItem_YuanShen);
			yuanShen->setPosition(ccp(550, 838-699));
			if (((BaseActor*)mHeroActors->objectAtIndex(mCurHeroIdx))->getCurrentProperty().level < 30)
			{
				yuanShen->setVisible(false);
			}

			//共鸣
			ZhuangbeiGongMingManager *manager = ZhuangbeiGongMingManager::getManager();
			//当前共鸣星级
			vector<EquipData> vec;
			for (int i=0; i!=mCurEquips.size(); ++i)
				vec.push_back(mCurEquips.at(i)->getEquipData());
			int curStarNum = manager->getGongMingStarNum(vec);
			int curLevel = manager->getGongMingLevel(curStarNum);
			int imgIdx = 1;
			if (curLevel == 0 || curLevel == 1) imgIdx = 1;
			else if (curLevel == 2 || curLevel == 3) imgIdx = 2;
			else if (curLevel == 4 || curLevel == 5 || curLevel == 6) imgIdx = 3;
			else if (curLevel == 7) imgIdx = 4;
			else if (curLevel == 8) imgIdx = 5;
			else if (curLevel == 9) imgIdx = 6;
			else if (curLevel == 10) imgIdx = 7;
			CCString temp;
			temp.initWithFormat("gongming_anniu_%d.png", imgIdx);
			CCMenuItemImage *gmItem = CCMenuItemImage::create(
				temp.getCString(),
				temp.getCString(),
				this, menu_selector(ZhuangBeiMianBanOther::menuItemClicked_MainMenu));
			mMainMenu->addChild(gmItem, 1, Tag_MenuItem_GongMing);
			gmItem->setPosition(ccp(222, 838-200));
		}

		// 4. hero menu item
		for(int i=0; i!=mHeroActors->count(); ++i)
		{
			BaseSprite* touxiangBg = BaseSprite::create("renxing_renwu_touxiang_bg.png");
			mBg->addChild(touxiangBg);
			touxiangBg->setPosition(ccp(109+140*i, 838-83));

			
			ImageData data = ImageManager::getManager()->getImageData_ProfessionHead(
				((BaseActor*)mHeroActors->objectAtIndex(i))->getActorType());
			BaseSprite* pic = BaseSprite::create(data.getCurrentImageFile());
			touxiangBg->addChild(pic);
			pic->setPosition(ccp(touxiangBg->getContentSize().width/2, touxiangBg->getContentSize().height/2));


			CCMenuItemImage* item;
			if(i == mCurHeroIdx)
			{
				item = CCMenuItemImage::create(
					"renxing_renwu_touxiang_select.png"
					, "renxing_renwu_touxiang_select.png"
					, this, menu_selector(ZhuangBeiMianBanOther::menuItemHeroListClicked));
			}else
			{
				item = CCMenuItemImage::create(
					"renxing_renwu_touxiang.png"
					, "renxing_renwu_touxiang.png"
					, this, menu_selector(ZhuangBeiMianBanOther::menuItemHeroListClicked));
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

		// 6. hero scroll
//		addHeroScrollView();
		//由 initFooter调用
		
		//7. 返回 按钮
		CCMenu *menu = CCMenu::create();
		mBg->addChild(menu, 1, Tag_FanhuiMenu);
		menu->setPosition(CCPointZero);
		CCMenuItemImage *item = CCMenuItemImage::create(
			"shuxing_renwu_fanhui.png"
			, "shuxing_renwu_fanhui_select.png"
			, this, menu_selector(ZhuangBeiMianBanOther::menuItemClicked_Back));
		menu->addChild(item);
		item->setPosition(ccp(320, 838-698));


		bRet = true;
	} while (0);


	return bRet;
}

void ZhuangBeiMianBanOther::refreshEquipmentUI()
{
	BaseActor *actor = dynamic_cast<BaseActor*>(mHeroActors->objectAtIndex(mCurHeroIdx));
	if (actor == NULL) return;

	mCurEquips = actor->getEquipments();

	// 5. hero equip menu item
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

	for(int i=0; i!=mCurEquips.size(); ++i)
	{
		//底纹
		ImageData diwenData;
		BaseSprite* diwen;
		int menuItemTag = -1;
		int TAG_Place;
		switch (mCurEquips.at(i)->getType())
		{
		case ZhuangbeiType_Dunpai:
		case ZhuangbeiType_Faqi:
			diwenData = ImageManager::getManager()->getImageData_DiWen_ZhuangBeiMianBan(
				TAG_DunPai,
				mCurEquips.at(i)->getEquipColor(),
				false,//mCurEquips.at(i)->isShenBing(),
				mCurEquips.at(i)->isPartOfSuit(),
				mCurEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Dunpai);
			diwen->setPosition(posDunpai);
			diwen->setScale(diwenData.scale);
			menuItemTag = Tag_MenuItem_Dunpai;
			TAG_Place = TAG_DunPai;
			//强化和精炼
			if (mCurEquips.at(i)->canBeJingLian(xxx) && mCurEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
				char temp[16];
				sprintf(temp,"+%d",mCurEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Dunpai);
				lableNum->setPosition(ccp(155,387));
			}
			if (mCurEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurEquips.at(i));
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
				mCurEquips.at(i)->getEquipColor(),
				false,//mCurEquips.at(i)->isShenBing(),
				mCurEquips.at(i)->isPartOfSuit(),
				mCurEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Hushengfu);
			diwen->setScale(diwenData.scale);
			diwen->setPosition(posHushengfu);
			menuItemTag = Tag_MenuItem_Hushengfu;
			TAG_Place = TAG_HuShenFu;
			//强化和精炼
			if (mCurEquips.at(i)->canBeJingLian(xxx) && mCurEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
				//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Hushengfu);
				//				mBg->addChild(icon, 20, Tag_JingLianBar_Hushengfu);
				//				icon->setPositionY(432);
				lableNum->setPosition(ccp(583,432));
				//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}
			if (mCurEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurEquips.at(i));
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
				mCurEquips.at(i)->getEquipColor(),
				false,//mCurEquips.at(i)->isShenBing(),
				mCurEquips.at(i)->isPartOfSuit(),
				mCurEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Jiezhi1+jiezhi-1);
			diwen->setScale(diwenData.scale);
			TAG_Place = TAG_JieZhi;
			if(jiezhi == 1)
			{
				diwen->setPosition(posJiezhi1);
				menuItemTag = Tag_MenuItem_Jiezhi1;
				//强化和精炼
				if (mCurEquips.at(i)->canBeJingLian(xxx) && mCurEquips.at(i)->getCurrentJingLianJieShu() >= 1)
				{
					//					BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
					char temp[16];
					sprintf(temp,"+%d",mCurEquips.at(i)->getCurrentJingLianJieShu());
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
				//强化和精炼
				if (mCurEquips.at(i)->canBeJingLian(xxx) && mCurEquips.at(i)->getCurrentJingLianJieShu() >= 1)
				{
					//					BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
					char temp[16];
					sprintf(temp,"+%d",mCurEquips.at(i)->getCurrentJingLianJieShu());
					CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
					lableNum->setAnchorPoint(ccp(1.0f,0.5f));
					mBg->addChild(lableNum, 20, Tag_JingLianNum_Jiezhi2);
					//					mBg->addChild(icon, 20, Tag_JingLianBar_Jiezhi2);
					//					icon->setPositionY(250);
					lableNum->setPosition(ccp(583,250));
					//					icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
				}
			}
			if (mCurEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurEquips.at(i));
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
				mCurEquips.at(i)->getEquipColor(),
				false,//mCurEquips.at(i)->isShenBing(),
				mCurEquips.at(i)->isPartOfSuit(),
				mCurEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Kuijia);
			diwen->setScale(diwenData.scale);
			diwen->setPosition(posKuijia);
			menuItemTag = Tag_MenuItem_Kuijia;
			TAG_Place = TAG_Kuijia;
			//强化和精炼
			if (mCurEquips.at(i)->canBeJingLian(xxx) && mCurEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
				//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Kuijia);
				//				mBg->addChild(icon, 20, Tag_JingLianBar_Kuijia);
				//				icon->setPositionY(626);
				lableNum->setPosition(ccp(605,625));
				//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}
			if (mCurEquips.at(i)->canPurify(xxx,1))
			{
				ZhuangbeiOptionLevel *layer = ZhuangbeiOptionLevel::createWithEquipment(mCurEquips.at(i));
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
				mCurEquips.at(i)->getEquipColor(),
				false,//mCurEquips.at(i)->isShenBing(),
				mCurEquips.at(i)->isPartOfSuit(),
				mCurEquips.at(i)->haveZhuanke());
			diwen = BaseSprite::create(diwenData.getCurrentImageFile());
			mBg->addChild(diwen, 1, Tag_DiWen_Wuqi);
			diwen->setScale(diwenData.scale);
			diwen->setPosition(posWuqi);
			menuItemTag = Tag_MenuItem_Wuqi;
			TAG_Place = TAG_WuQi;
			//强化和精炼
			if (mCurEquips.at(i)->canBeJingLian(xxx) && mCurEquips.at(i)->getCurrentJingLianJieShu() >= 1)
			{
				//				BaseSprite* icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
				char temp[16];
				sprintf(temp,"+%d",mCurEquips.at(i)->getCurrentJingLianJieShu());
				CCLabelTTF* lableNum = CCLabelTTF::create(temp,fontStr_BookAntiqua,20,CCSizeMake(0,0),kCCTextAlignmentRight);
				lableNum->setAnchorPoint(ccp(1.0f,0.5f));
				mBg->addChild(lableNum, 20, Tag_JingLianNum_Wuqi);
				//				mBg->addChild(icon, 20, Tag_JingLianBar_Wuqi);
				//				icon->setPositionY(643);
				lableNum->setPosition(ccp(154,643));
				//				icon->setPositionX(lableNum->getPositionX() - lableNum->getContentSize().width/2 - icon->getContentSize().width/2);
			}

			//fushou 
			if(CS::isShuangShouWuQi(mCurEquips.at(i)->getType()))
			{
				//底纹
				BaseSprite* fushouDiwen = BaseSprite::create("mianban_renwu_zhuangbei_fushou_diban.png");
				mBg->addChild(fushouDiwen, 1, Tag_DiWen_Dunpai);
				fushouDiwen->setScale(diwenData.scale);
				fushouDiwen->setPosition(posDunpai);
				//装备
				BaseSprite* fushouZhuangBei = ImageManager::getManager()->getSpriteOfZhuangbei(mCurEquips.at(i)->getEquipData());
				fushouDiwen->addChild(fushouZhuangBei);
				fushouZhuangBei->setColor(ccc3(185,117,117));
				fushouZhuangBei->setPosition(ccp(fushouDiwen->getContentSize().width/2, fushouDiwen->getContentSize().height/2));
				fushouZhuangBei->setScale(ImageManager::getManager()->getImageData_ZhuangBei_MianBan(mCurEquips.at(i)).scale);
				//按钮
				CCSprite* imgNormal = CCSprite::create(
					ImageManager::getManager()->getImageData_DiWen_Selected(
					mCurEquips.at(i)->getEquipColor(),
					false,//mCurEquips.at(i)->isShenBing(),
					mCurEquips.at(i)->isPartOfSuit()
					).getCurrentImageFile().c_str());
				imgNormal->setOpacity(0);
				CCSprite* imgSelected = CCSprite::create(
					ImageManager::getManager()->getImageData_DiWen_Selected(
					mCurEquips.at(i)->getEquipColor(),
					false,//mCurEquips.at(i)->isShenBing(),
					mCurEquips.at(i)->isPartOfSuit()
					).getCurrentImageFile().c_str());
				imgSelected->setOpacity(30);
				CCMenuItemSprite* menuItem = CCMenuItemSprite::create(
					imgNormal
					, imgSelected
					, this, menu_selector(ZhuangBeiMianBanOther::menuItemEquipClicked));
				mEquipMenu->addChild(menuItem, 1, Tag_MenuItem_Dunpai);
				menuItem->setPosition(fushouDiwen->getPosition());
				menuItem->setScaleX(fushouDiwen->getContentSize().width / menuItem->getContentSize().width);
				menuItem->setScaleY(fushouDiwen->getContentSize().height / menuItem->getContentSize().height);
				menuItem->setUserData((void *)i);
			}

			break;
		}

		//装备
		ImageData equipData = ImageManager::getManager()->getImageData_ZhuangBei_MianBan(mCurEquips.at(i));
		BaseSprite* zhuangbei = ImageManager::getManager()->getSpriteOfZhuangbei(mCurEquips.at(i)->getEquipData(),false,true);
		diwen->addChild(zhuangbei);
		zhuangbei->setPosition(ccp(diwen->getContentSize().width/2, diwen->getContentSize().height/2));
		zhuangbei->setScale(equipData.scale);

		//按钮
		CCSprite* imgNormal = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
			TAG_Place,
			mCurEquips.at(i)->getEquipColor(),
			false,//mCurEquips.at(i)->isShenBing(),
			mCurEquips.at(i)->isPartOfSuit()
			).getCurrentImageFile().c_str());
		imgNormal->setOpacity(0);
		CCSprite* imgSelected = CCSprite::create(
			ImageManager::getManager()->getImageData_DiWen_Selected_ZhuangBeiMianBan(
			TAG_Place,
			mCurEquips.at(i)->getEquipColor(),
			false,//mCurEquips.at(i)->isShenBing(),
			mCurEquips.at(i)->isPartOfSuit()
			).getCurrentImageFile().c_str());
		CCMenuItemSprite* menuItem = CCMenuItemSprite::create(
			imgNormal
			, imgSelected
			, this, menu_selector(ZhuangBeiMianBanOther::menuItemEquipClicked));
		mEquipMenu->addChild(menuItem, 1, menuItemTag);
		menuItem->setPosition(diwen->getPosition());
		menuItem->setScaleX(diwen->getContentSize().width / menuItem->getContentSize().width);
		menuItem->setScaleY(diwen->getContentSize().height / menuItem->getContentSize().height);
		menuItem->setUserData((void *)i);
	}

	//没有装备的地方

}

void ZhuangBeiMianBanOther::refreshPrepertyLabel()
{
	BaseActor *actor = dynamic_cast<BaseActor*>(mHeroActors->objectAtIndex(mCurHeroIdx));
	if (actor == NULL) return;

	char buf[20];
	//xueliang
	CCLabelTTF* xueliang = ((CCLabelTTF*)mBg->getChildByTag(Tag_Label_Xueliang));
	if (xueliang == NULL)
	{
		xueliang = CCLabelTTF::create(buf, fontStr_kaiti, Label_Size);
		mBg->addChild(xueliang, 1, Tag_Label_Xueliang);
		xueliang->setAnchorPoint(ccp(0,0.5));
		xueliang->setPosition(ccp(116, 838-783));
	}
	sprintf(buf, "%.1f", actor->getXueliang());
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
		lv->setPosition(ccp(202, 838-630));
	}
	lv->setString(buf);
	
	if (actor->getCurrentProperty().level < 30)
		mMainMenu->getChildByTag(Tag_MenuItem_YuanShen)->setVisible(false);
	else 
		mMainMenu->getChildByTag(Tag_MenuItem_YuanShen)->setVisible(true);


	//共鸣
	CCMenuItemImage *gongmingItem = (CCMenuItemImage*)mMainMenu->getChildByTag(Tag_MenuItem_GongMing);
	ZhuangbeiGongMingManager *manager = ZhuangbeiGongMingManager::getManager();
	//当前共鸣星级
	vector<EquipData> vec;
	for (int i=0; i!=mCurEquips.size(); ++i)
		vec.push_back(mCurEquips.at(i)->getEquipData());
	int curStarNum = manager->getGongMingStarNum(vec);
	int curLevel = manager->getGongMingLevel(curStarNum);
	int imgIdx = 1;
	if (curLevel == 0 || curLevel == 1) imgIdx = 1;
	else if (curLevel == 2 || curLevel == 3) imgIdx = 2;
	else if (curLevel == 4 || curLevel == 5 || curLevel == 6) imgIdx = 3;
	else if (curLevel == 7) imgIdx = 4;
	else if (curLevel == 8) imgIdx = 5;
	else if (curLevel == 9) imgIdx = 6;
	else if (curLevel == 10) imgIdx = 7;
	CCString temp;
	temp.initWithFormat("gongming_anniu_%d.png", imgIdx);
	gongmingItem->initWithNormalImage(temp.getCString(),temp.getCString(),
		NULL, this, menu_selector(ZhuangBeiMianBanOther::menuItemClicked_MainMenu));
	if (curLevel == 0)
		gongmingItem->setVisible(false);
	else 
		gongmingItem->setVisible(true);

}

void ZhuangBeiMianBanOther::refreshHerosImgScrollView()
{
	mHeroView->prepareToDisplay();
	mHeroView->scrollToPageSimple(mCurHeroIdx, 0);
}

void ZhuangBeiMianBanOther::addHeroScrollView()
{
	mHeroView = new GridScrollView(
		CCSizeMake(288,531),
		mHeroActors->count(),			// 元素的个数
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
	mHeroView->scrollToPageSimple(mCurHeroIdx, 0);

}

ClickableItem* ZhuangBeiMianBanOther::createGridItem(int pageIndex,int xIndex,int yIndex)
{
	BaseActor *actor = dynamic_cast<BaseActor*>(mHeroActors->objectAtIndex(pageIndex));

	ScrollOhterHeroNode* node = new ScrollOhterHeroNode(
		ImageManager::getManager()->getFullPhotoOfActor(actor), actor, mFooterLayer);
	node->autorelease();
	node->setScale(1.2f);
	return node;
}
// 页面选择了
void ZhuangBeiMianBanOther::pageSelectedEnd(int pageIndex)
{
	menuItemHeroListClicked( mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+pageIndex) );
}

void ZhuangBeiMianBanOther::menuItemHeroListClicked(CCObject* pSender)
{
	CCMenuItemImage* oldMenuItem = (CCMenuItemImage*)mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+mCurHeroIdx);
	oldMenuItem->initWithNormalImage(
		"renxing_renwu_touxiang.png"
		, "renxing_renwu_touxiang.png"
		, NULL, this, menu_selector(ZhuangBeiMianBanOther::menuItemHeroListClicked));

	mCurHeroIdx = ((CCMenuItemImage*)pSender)->getTag() - Tag_MenuItem_HeroList;
	CCMenuItemImage* curMenuItem = (CCMenuItemImage*)mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+mCurHeroIdx);
	curMenuItem->initWithNormalImage(
		"renxing_renwu_touxiang_select.png"
		, "renxing_renwu_touxiang_select.png"
		, NULL, this, menu_selector(ZhuangBeiMianBanOther::menuItemHeroListClicked));

	mBg->removeChildByTag(Tag_DiWen_Wuqi);
	mBg->removeChildByTag(Tag_DiWen_Hushengfu);
	mBg->removeChildByTag(Tag_DiWen_Jiezhi1);
	mBg->removeChildByTag(Tag_DiWen_Jiezhi2);
	mBg->removeChildByTag(Tag_DiWen_Kuijia);
	mBg->removeChildByTag(Tag_DiWen_Dunpai);
	mEquipMenu->removeAllChildren();
	refreshEquipmentUI();
	refreshPrepertyLabel();
	mHeroView->scrollToPageSimple(mCurHeroIdx, 0);
}

void ZhuangBeiMianBanOther::menuItemEquipClicked(CCObject* pSender)
{
	int idx = (int)((CCMenuItemImage*)pSender)->getUserData();

	EquipmentXiangQingBody* q = new EquipmentXiangQingBody(mCurEquips.at(idx), mCurEquips.at(idx)->getActorId(), From_OtherHeroDetail);
	mFooterLayer->popUpContaintLayer(q, Pop_Pos_WondowDown);
	q->release();
}

void ZhuangBeiMianBanOther::disableAllTouchBegin()
{
	mHeroMenu->setEnabled(false);
	mEquipMenu->setEnabled(false);
	mMainMenu->setEnabled(false);
	mHeroView->setTouchEnabled(false);
	mBg->getChildByTag(Tag_FanhuiMenu)->setVisible(false);
}

void ZhuangBeiMianBanOther::disableAllTouchEnd()
{
	mHeroMenu->setEnabled(true);
	mEquipMenu->setEnabled(true);
	mMainMenu->setEnabled(true);
	mBg->getChildByTag(Tag_FanhuiMenu)->setVisible(true);
	if (mHeroView != NULL)
	{
		mHeroView->setTouchEnabled(true);
	}
}


void ZhuangBeiMianBanOther::refreshLayer()
{
	menuItemHeroListClicked( mHeroMenu->getChildByTag(Tag_MenuItem_HeroList+mCurHeroIdx) );
// 	this->refreshEquipmentUI();
// 	this->refreshPrepertyLabel();
}

void ZhuangBeiMianBanOther::menuItemClicked_Back(CCObject *pSender)
{
	mFooterLayer->dismmisPopUp();
}

vector<EquipData> ZhuangBeiMianBanOther::getEquipsDataByActorId(unsigned int actorId)
{
	vector<EquipData> vec;

	for (int i=0; i!=mCurEquips.size(); ++i)
	{
		vec.push_back(mCurEquips.at(i)->getEquipData());
	}

	return vec;
}

void ZhuangBeiMianBanOther::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_YuanShen)
	{
		int yuanshen_heroId = 0;
// 		BaseActor *yuanshen_actor = ((BaseActor*)mHeroActors->objectAtIndex(mCurHeroIdx))->getMyYuanShen();
// 		if (yuanshen_actor != NULL) yuanshen_heroId = yuanshen_actor->getId();
		for (int xxx=0, count=mWillDeleteActors_YuanShen->count(); xxx<count; ++xxx)
		{
			unsigned int heroId = ((BaseActor*)mHeroActors->objectAtIndex(mCurHeroIdx))->getId();
			unsigned int yuanshenParId = ((BaseActor*)mWillDeleteActors_YuanShen->objectAtIndex(xxx))->getMyZhenShen()->getId();
			if (heroId == yuanshenParId)
			{
				yuanshen_heroId = ((BaseActor*)mWillDeleteActors_YuanShen->objectAtIndex(xxx))->getId();
			}
		}
		ZhuangBeiMianBanYuanShen *layer = ZhuangBeiMianBanYuanShen::create(
			mWillDeleteActors_YuanShen, yuanshen_heroId, comefrom_other, this);
		mFooterLayer->popUpContaintLayer(layer);
	}
	else if (tag == Tag_MenuItem_GongMing)
	{
		//
		this->disableAllTouchBegin();
		//
		BaseSprite *layer = BaseSprite::create("zhandou_jiugongge_touming.png");
		layer->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 128, Tag_GongMingLayer);
		//
		CCMenu *menu = CCMenu::create();
		layer->addChild(menu);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *item = CCMenuItemImage::create(
			"zhandou_jiugongge_touming.png",
			"zhandou_jiugongge_touming.png",
			this, menu_selector(ZhuangBeiMianBanOther::menuItemClicked_BackFromGongMing));
		menu->addChild(item);
		item->setOpacity(0);
		item->setPosition(ccp(layer->getContentSize().width/2, layer->getContentSize().height/2));
		
		
		CCString temp;
		ZhuangbeiGongMingManager *manager = ZhuangbeiGongMingManager::getManager();
		//
		BaseSprite *board = BaseSprite::create("gongming_bg.png");
		board->setPosition(ccp(layer->getContentSize().width/2, layer->getContentSize().height/2));
		layer->addChild(board, 10);
		//当前共鸣星级
		vector<EquipData> vec;
		for (int i=0; i!=mCurEquips.size(); ++i)
			vec.push_back(mCurEquips.at(i)->getEquipData());
		int curStarNum = manager->getGongMingStarNum(vec);
		int curLevel = manager->getGongMingLevel(curStarNum);
		temp.initWithFormat("%s: %s%s(%d%s)", LFStrings::getValue("GongMing_CurStar").c_str(), getStrByInt(curLevel).c_str()
			, LFStrings::getValue("ji").c_str(), curStarNum, LFStrings::getValue("xing").c_str());
		CCLabelTTF *curStarLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 24);
		curStarLabel->setAnchorPoint(ccp(0, 0.5f));
		curStarLabel->setPosition(ccp(48, 304-53));
		curStarLabel->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Xiyou, false, false));
		board->addChild(curStarLabel);
		//下一阶共鸣需求星级
		temp.initWithFormat("%s: %u%s", LFStrings::getValue("GongMing_NextStar").c_str()
			, manager->getGongMingConfig().levelNeedStarNumConfig[curLevel + 1], LFStrings::getValue("xing").c_str());
		CCLabelTTF *nextStarLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 21);
		nextStarLabel->setAnchorPoint(ccp(0, 0.5f));
		nextStarLabel->setPosition(ccp(curStarLabel->getPositionX(), 304-85));
		board->addChild(nextStarLabel);
		//共鸣效果
		CCLabelTTF *gmxgLabel = CCLabelTTF::create(LFStrings::getValue("GongMing_Function").c_str(), fontStr_kaiti, 21);
		gmxgLabel->setAnchorPoint(ccp(0, 0.5f));
		gmxgLabel->setPosition(ccp(curStarLabel->getPositionX(), 304-134));
		gmxgLabel->setColor(ccc3(255, 108, 0));
		board->addChild(gmxgLabel);
		//共鸣效果列表
		// 			GM_HuJiaAddPer = 0,
		// 			GM_HPAddPer,
		// 			GM_GongJiAddPer,
		// 			GM_GeDangZhiAdd,
		// 			GM_BaoShangYinZiAdd,
		string str;
		if (manager->getGongMingConfig().properyAdd[curLevel][0] > 0)
		{
			temp.initWithFormat("%s+%u%%  ", LFStrings::getValue("huJia").c_str(), manager->getGongMingConfig().properyAdd[curLevel][0]);
			str += temp.getCString();
		}
		if (manager->getGongMingConfig().properyAdd[curLevel][1] > 0)
		{
			temp.initWithFormat("%s+%u%%  ", LFStrings::getValue("shengMin").c_str(), manager->getGongMingConfig().properyAdd[curLevel][1]);
			str += temp.getCString();
		}
		if (manager->getGongMingConfig().properyAdd[curLevel][2] > 0)
		{
			temp.initWithFormat("%s+%u%%  ", LFStrings::getValue("gongJi").c_str(), manager->getGongMingConfig().properyAdd[curLevel][2]);
			str += temp.getCString();
		}
		if (manager->getGongMingConfig().properyAdd[curLevel][3] > 0)
		{
			temp.initWithFormat("%s+%u  ", LFStrings::getValue("geDangZhi").c_str(), manager->getGongMingConfig().properyAdd[curLevel][3]);
			str += temp.getCString();
		}
		if (manager->getGongMingConfig().properyAdd[curLevel][4] > 0)
		{
			temp.initWithFormat("%s+%u  ", LFStrings::getValue("baojishanghai").c_str(), manager->getGongMingConfig().properyAdd[curLevel][4]);
			str += temp.getCString();
		}
		CCLabelTTF *propLabel = CCLabelTTF::create(str.c_str(), fontStr_kaiti, 21, CCSize(410,0), kCCTextAlignmentLeft);
		propLabel->setAnchorPoint(ccp(0, 1));
		propLabel->setPosition(ccp(68, 304-160));
		propLabel->setColor(gmxgLabel->getColor());
		board->addChild(propLabel);
		//根据全身装备强化效果计算共鸣星级
		str = LFStrings::getValue("GongMing_Calculate") + ":";
		CCLabelTTF *calcLable = CCLabelTTF::create(str.c_str(), fontStr_kaiti, 21);
		calcLable->setAnchorPoint(ccp(0, 0.5f));
		calcLable->setPosition(ccp(curStarLabel->getPositionX(), 304-222));
		board->addChild(calcLable);
		//
		temp.initWithFormat("%s%s=1%s, %s%s=3%s", 
			LFStrings::getValue("yin").c_str(), LFStrings::getValue("xing").c_str(), LFStrings::getValue("xing").c_str(),
			LFStrings::getValue("jin").c_str(), LFStrings::getValue("xing").c_str(), LFStrings::getValue("xing").c_str());
		CCLabelTTF *caLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 21);
		caLabel->setAnchorPoint(ccp(0, 0.5f));
		caLabel->setPosition(ccp(curStarLabel->getPositionX(), 304-246));
		board->addChild(caLabel);
	}
}

void ZhuangBeiMianBanOther::menuItemClicked_BackFromGongMing(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_GongMingLayer);
	this->disableAllTouchEnd();
}

void ZhuangBeiMianBanOther::yuanshen_backFunc(unsigned int yuanshenId)
{
	int idx = -1;
	for (int i=0, count=mHeroActors->count(); i<count; ++i)
	{
		BaseActor *actor = dynamic_cast<BaseActor*>(mHeroActors->objectAtIndex(i));
		if (actor && actor->getMyYuanShen())
		{
			if (actor->getMyYuanShen()->getId() == yuanshenId)
			{
				idx = i;
				break;
			}
		}
	}
	//
	if (idx > -1)
	{
		pageSelectedEnd(idx);
	}
	else
	{
		CCLog("Error: %s : can not found this yuanshen's parent [yuanshen id = %u]", __FUNCTION__, yuanshenId);
	}
}