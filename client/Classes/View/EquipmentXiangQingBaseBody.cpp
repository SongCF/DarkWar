#include "EquipmentXiangQingBaseBody.h"
#include "EquipmentManager.h"
#include "ImageManager.h"
#include "LFUtile.h"
#include "ChangeZhuangBeiBody.h"
#include "MyselfManager.h"
#include "BaseActorPropertyParser.h"
#include "BaseActor.h"
#include "RenWuXiangXiLayer.h"
#include "font.h"
#include "LFUtile.h"
#include "CS_Common.h"
#include "GameDirector.h"
#include "LFMessageBox.h"
#include "LFToast.h"
#include "LFStrings.h"
#include "SuitPool.h"
#include "GuideLayerBase.h"
#include "History.h"
#include "ShenBingPool.h"
#include "FX_CommonFunc.h"
#include "ZhuangBeiMianBanOther.h"
#include "ZhuangBeiMianBanYuanShen.h"

using namespace FX_CommonFunc;

extern bool g_firstTaskEnd_to_changeEquip;
extern Equipment *g_YuanShiBiJiao_Equip;

extern string GetMoFaJingHua_PrepertyName(MofaJinghua jinghua, bool shuzhiLeft);

using namespace std;

#define LABEL_COLOR ccc3(79, 110, 217)

//#define LABEL_SIZE 26
//#define Y_OFFSET		122

int Y_OFFSET = 122;
int LABEL_SIZE = 26;

#define TAG_ITEM_CLOSE           1111  //关闭
#define TAG_ITEM_QUEDING         1112 //确定
#define TAG_ITEM_CHANGE      	 1113  //更换
#define TAG_ITEM_XiangXi_BIJIAO	 1114	// 详细对比
#define TAG_ITEM_YuanShi_BIJIAO	 1115	// 原始对比
#define TAG_ITEM_TAOZHUANG       1116 //套装
#define TAG_ITEM_JIEFENG         1117 //解封

//工匠系统
#define Tag_Item_Qianghua  1121 //强化
#define Tag_Item_JiangLian 1122 //精炼
#define Tag_Item_ZhuanKe   1123 //篆刻
#define Tag_Item_BaoShi    1124 //宝石
#define Tag_Item_JianDing  1125 //鉴定
#define Tag_Item_ShenBingLevUp 1126 //神兵升级

#define Tag_Item_Xilian 1127 //洗炼

#define TAG_XiangXiLeft 1211	

#define POS_ITEM_1             ccp(320, 838-792)
#define POS_ITEM_YuanShiBiJiao ccp(100, 838-792)
#define POS_ActorName          ccp(612, 838+Y_OFFSET-90 )

#define POS_Item_QiangHua      ccp(128, 838+Y_OFFSET-436)
#define POS_Item_JingLian      ccp(128, 838+Y_OFFSET-530)
#define POS_Item_ZhuanKe       ccp(128, 838+Y_OFFSET-622)

#define POS_Item_XiLian       ccp(128, 838+Y_OFFSET - 530 - 118)


EquipmentXiangQingBaseBody::EquipmentXiangQingBaseBody(Equipment* equip, unsigned int actorId,ZhuangbeiXiangQingFrom from)
{
	init(equip, actorId, from);
}

void EquipmentXiangQingBaseBody::refreshLayer()
{
	this->removeAllChildren();
	init(mEquip, mActorId, mFrom);
}

bool EquipmentXiangQingBaseBody::init(Equipment* equip, unsigned int actorId,ZhuangbeiXiangQingFrom from)
{
	//初始化成员变量
	mEquip = equip;
	mActorId = actorId;
	mFrom = from;
	//
	
	if (mFrom == From_ZhuangBeiDuanZao)
	{
		mBoard = BaseSprite::create("gongjiang_duanzao_zhuangbei_bg.png");
		Y_OFFSET = -235;
		LABEL_SIZE = 20;
	}
	else
	{
		mBoard = BaseSprite::create("zhuangbei_xiangxi_mianban.png");
		Y_OFFSET = 122;
		LABEL_SIZE = 26;
	}
	addChild(mBoard);
	mBoard->setPosition(ccp(mWinSize.width/2,mWinSize.height/2));
	//
	mMenu = CCMenu::create();
//	mBoard->addChild(mMenu);                //在属性对比 之前， 在装备底纹之后
	mMenu->setPosition(CCPointZero);
	//
	mGongJiangMenu= CCMenu::create();
//加在 属性层上面
	mGongJiangMenu->setPosition(CCPointZero);

	//标题，装备名称 
	{
		
		CCLabelTTF* lableName = NULL;
		if (mFrom == From_ZhuangBeiDuanZao)
		{
			lableName = LF::lable(mBoard,mEquip->getName(),CCPointZero,24,fontStr_kaiti);
			lableName->setPosition(ccp(mBoard->getContentSize().width/2,mBoard->getContentSize().height-lableName->getContentSize().height-13));
		}
		else
		{
			lableName = LF::lable(mBoard,mEquip->getName(),CCPointZero,30,fontStr_kaiti);
			lableName->setPosition(ccp(mBoard->getContentSize().width/2,mBoard->getContentSize().height-lableName->getContentSize().height));
		}
		
		ccColor3B color = ImageManager::getManager()->getShowColor(equip->getEquipColor(),false,equip->isPartOfSuit());
		lableName->setColor(color);

		//套装绑定
		if (equip->getEquipData().profession_id != 0)
		{
			CCString temp;
			temp.initWithFormat(("("+LFStrings::getValue("jing_s_KeZhuangBei")+")").c_str(),
				BaseActorPropertyParser::getParser()->getExtraProerty((Profession)equip->getEquipData().profession_id).nameInChinese.c_str());
			
			CCLabelTTF *profLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, LABEL_SIZE-2);
			profLabel->setColor(ccRED);
			mBoard->addChild(profLabel);
			profLabel->setPosition(ccp(lableName->getPositionX()+lableName->getContentSize().width/2+profLabel->getContentSize().width/2,
				lableName->getPositionY()));

			if (mFrom == From_ZhuangBeiDuanZao)
			{
				lableName->setFontSize(LABEL_SIZE-2);
			}
		}
	}

	//底纹
	{
		ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			equip->getEquipColor(),
			false,
			equip->isPartOfSuit(),
			equip->haveZhuanke());
		BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
		diWen->setScale(0.7f);
		diWen->setPosition(ccp(123, 838+Y_OFFSET-240));
		if (mFrom == From_ZhuangBeiDuanZao)
		{
			diWen->setScale(0.47f);
			diWen->setPosition(ccp(94, 838+Y_OFFSET-240+50));
		}
		
		mBoard->addChild(diWen);
		
		// 装备图片
		ImageData data = ImageManager::getManager()->getImageData_ZhuangBei_XiangQing(equip);
		BaseSprite* s= ImageManager::getManager()->getSpriteOfZhuangbei(equip);
		s->setScale(data.scale);
		diWen->addChild(s);
		s->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));

		//等级不足提示图片
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(mActorId);
//		CCAssert(actor!=NULL,"");
		if (actor && !actor->isYuanShen() && mEquip->getLevel() > actor->getCurrentProperty().level)
		{
			BaseSprite* lock = BaseSprite::create("zhuangbei_xiangxi_diwen_dengjibuzu_new.png");
			diWen->addChild(lock);
			lock->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));
		}

		//套装按钮
		if(SuitPool::getPool()->isSuit(equip->getEquipData().group_id) && false)
		{
			CCMenuItemImage *taozhuangItem = CCMenuItemImage::create(
				"zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing.png"
				, "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing_select.png"
				, this ,menu_selector(EquipmentXiangQingBaseBody::menuItemClicked));
			mMenu->addChild(taozhuangItem, 0, TAG_ITEM_TAOZHUANG);
			//taozhuangItem->setAnchorPoint(ccp(1, 0));
			taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()-taozhuangItem->getContentSize().width/2
				, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()+taozhuangItem->getContentSize().height/2));
			taozhuangItem->setEnabled(false);
		}
		else if(SuitPool::getPool()->isSuit(equip->getEquipData().group_id))
		{
			CCMenuItemImage *taozhuangItem = CCMenuItemImage::create(
				"zhuangbei_xiangxi_mianban_anniu_taozhuang.png"
				, "zhuangbei_xiangxi_mianban_anniu_taozhuang_select.png"
				, this ,menu_selector(EquipmentXiangQingBaseBody::menuItemClicked));
			mMenu->addChild(taozhuangItem, 0, TAG_ITEM_TAOZHUANG);
//			taozhuangItem->setAnchorPoint(ccp(1, 0));


			if (mFrom == From_ZhuangBeiDuanZao)
			{
				taozhuangItem->setScale(0.65f);
			}

			taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()-taozhuangItem->getContentSize().width/2*taozhuangItem->getScaleX()
				, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()+taozhuangItem->getContentSize().height/2*taozhuangItem->getScaleY()));
			taozhuangItem->setEnabled(false);

			//传奇套装
			BaseSprite* pTaozhuang = NULL;
			if (mEquip->getEquipData().zhuangbei_colour == ZhuangbeiColour_Chuanqi)
			{
				pTaozhuang = BaseSprite::create("zhuangbei_xiangxi_chuanqitaozhuang.png");
			}
			else if (mEquip->getEquipData().zhuangbei_colour == ZhuangbeiColour_Xiyou)
			{
				pTaozhuang = BaseSprite::create("zhuangbei_xiangxi_chuanqitaozhuang.png");
				pTaozhuang->setVisible(false);
			}
			if (pTaozhuang)
			{
				mBoard->addChild(pTaozhuang, 1);

				if (mFrom == From_ZhuangBeiDuanZao)
				{
					pTaozhuang->setScale(0.7f);
				}
				
				pTaozhuang->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()-pTaozhuang->getContentSize().width/2*pTaozhuang->getScaleX(), 
					diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()-pTaozhuang->getContentSize().height/2*pTaozhuang->getScaleY()));

				if (!pTaozhuang->isVisible())
				{
					pTaozhuang->setPositionY(diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY());
				}

				//穿在身上的能触发套装属性的套装件数、 groupId
				vector<unsigned int> _vec;
				if (mEquip->getActorId() != INVALID_ID)
				{
					//自己英雄的
					if (MyselfManager::getManager()->getMyTeamActor(mEquip->getActorId()) != NULL)
					{
						vector<Equipment*> vecEquip = MyselfManager::getManager()->getMyTeamActor(mEquip->getActorId())->getEquipments();
						vector<unsigned int> equipedOnHero_groupId;
						for (int curI=0; curI<vecEquip.size(); ++curI)
							equipedOnHero_groupId.push_back(vecEquip.at(curI)->getGroupId());
						_vec = SuitPool::getPool()->getInSuitedGroupIdsFromTheGroupId(equipedOnHero_groupId, mEquip->getGroupId());
					}
					//查询其他英雄时出现的
					else
					{
						if (mFooterLayer)
						{
							ZhuangBeiMianBanOther *layer_other = dynamic_cast<ZhuangBeiMianBanOther*>(mFooterLayer->getTopLayer(-1));
							ZhuangBeiMianBanYuanShen *layer_yuanshen = dynamic_cast<ZhuangBeiMianBanYuanShen*>(mFooterLayer->getTopLayer(-1));
							if (layer_other || layer_yuanshen)
							{ 
								vector<EquipData> vecData;
								if (layer_other) vecData = layer_other->getEquipsDataByActorId(mEquip->getActorId());
								else             vecData = layer_yuanshen->getEquipsDataByActorId(mEquip->getActorId());

								vector<unsigned int> equipedOnHero_groupId;
								for (int curI=0; curI<vecData.size(); ++curI)
									equipedOnHero_groupId.push_back(vecData.at(curI).group_id);
								_vec = SuitPool::getPool()->getInSuitedGroupIdsFromTheGroupId(equipedOnHero_groupId, mEquip->getGroupId());
							}
							else
							{
								CCLog("Error: %s --> 查询其他英雄的套装时 没找到ZhuangBeiMianBanOther", __FUNCTION__);
							}
						}
					}
				}

				bool hasEquiped = false;   
				if (_vec.size() > 0)
					hasEquiped = true;
				else 
					hasEquiped = false;

				int start_y = pTaozhuang->getPositionY()-pTaozhuang->getContentSize().height/2 - 20;
			
				ccColor3B showColor = ImageManager::getManager()->getShowColor(ZhuangbeiColour_Chuanqi, false, true);

				OneSuitInfo suitInfo;
				if(SuitPool::getPool()->getSuitInforByZBGroupId(mEquip->getGroupId(), suitInfo))
				{

					//套装名字
					string strName = suitInfo.suit_name;
					CCLabelTTF *labelName = CCLabelTTF::create(strName.c_str(), fontStr_kaiti, LABEL_SIZE, CCSizeMake(203, 0), kCCTextAlignmentCenter);
					mBoard->addChild(labelName);
					//labelName->setAnchorPoint(ccp(0.5f, 0.5));
					labelName->setHorizontalAlignment(kCCTextAlignmentCenter);
					labelName->setPosition(ccp(123, start_y));
					
					start_y -= (labelName->getContentSize().height + 2);
					
					if (mFrom == From_ZhuangBeiDuanZao)
					{
						labelName->setFontSize(LABEL_SIZE-2);
						labelName->setPosition(ccp(94, start_y+35));
						start_y -= (labelName->getContentSize().height + 2) - 35;
					}

					// 3 every one name
					int taozhuangNum = suitInfo.parts_of_suit.size(); //记录有多少件
					for (int i=0; i!=suitInfo.parts_of_suit.size(); ++i)
					{
						string strOneName = suitInfo.parts_of_suit.at(i).zhuangbei_name;
						string strTypeName = getPosName_ByZhuangBeiType(suitInfo.parts_of_suit.at(i).zhuanbei_type);
						CCLabelTTF *labelOneName = CCLabelTTF::create((strOneName + " (" + strTypeName + ")").c_str(), fontStr_kaiti, LABEL_SIZE-2);
						mBoard->addChild(labelOneName);
						labelOneName->setAnchorPoint(ccp(0, 0.5));
						labelOneName->setPosition(ccp(pTaozhuang->getPositionX()-pTaozhuang->getContentSize().width/2*pTaozhuang->getScaleX(), start_y));
						labelOneName->setColor(showColor);

						//如果没有穿上、 且不是当前详情展示的这件装备  则变灰
						if (! hasEquiped && strOneName != mEquip->getName())
						{
							labelOneName->setColor(fonColor_CiYao);
							--taozhuangNum;
						}
						//如果穿上了、则要显示套装属性哦
						else if (hasEquiped)
						{
							bool curHas = false;
							for (int jNum=0,c=_vec.size(); jNum<c; ++jNum)
							{
								if (suitInfo.parts_of_suit.at(i).containsGroupId(_vec[jNum]))
								{
									curHas = true;
									break;
								}
							}
							if (! curHas)
							{
								labelOneName->setColor(fonColor_CiYao);
								--taozhuangNum;
							}
						}

						start_y -= (labelOneName->getContentSize().height + 2);
					}

					labelName->setString(CCString::createWithFormat("%s(%d/%d)", labelName->getString(), taozhuangNum, suitInfo.parts_of_suit.size())->getCString());
				}
			}
		}

		//封印标记
		if (equip->isInFengYin())
		{
			CCSize diwenSize = diWen->getContentSize();
			float sca = diWen->getScale();
			BaseSprite *fyImg = BaseSprite::create("shenbing_fengyin_suo.png");
			diWen->addChild(fyImg, 10);
			fyImg->setPosition(ccp(diwenSize.width/2, diwenSize.height/2));
			fyImg->setScaleX(diwenSize.width / fyImg->getContentSize().width);
			fyImg->setScaleY(diwenSize.height / fyImg->getContentSize().height);

			if (mFrom == From_RenXingMianBan || mFrom == From_ZhuangBeiList)
			{
				CCMenuItemImage *jiefengItem = CCMenuItemImage::create(
					"zhuangbei_xiangxi_jianding.png"
					, "zhuangbei_xiangxi_jianding_select.png"
					, this, menu_selector(EquipmentXiangQingBaseBody::menuItemClicked));
				mMenu->addChild(jiefengItem, 0, TAG_ITEM_JIEFENG);
				jiefengItem->setPosition(POS_ITEM_1);
			}
		}
	}
// 
// 	//右边详细属性
	BaseSprite* temp  = (BaseSprite*)mBoard->getChildByTag(TAG_XiangXiLeft);
	if(temp)
		temp->removeFromParentAndCleanup(true);
	temp = BaseSprite::create();
	mBoard->addChild(temp,0,TAG_XiangXiLeft);

	// 写属性
	createEquipPropertyUI(0);
	//因为套装信息会用到 footer 所以延迟一帧刷新ui

	temp->addChild(mGongJiangMenu, 1);

	if (mEquip->isInFengYin())
	{

		CCArray *chidren = mGongJiangMenu->getChildren();
		CCObject *obj = NULL;
		CCARRAY_FOREACH(chidren, obj)
			dynamic_cast<CCMenuItem*>(obj)->setEnabled(false);
	}


	//关闭按钮 
	if (mFrom != From_ZhuangBeiDuanZao)
	{
		CCMenuItemImage* closeItem = CCMenuItemImage::create(
			"close.png",
			"close_select.png",
			this,menu_selector(EquipmentXiangQingBaseBody::menuItemClicked)    
			);
		closeItem->setScale(1.2f);
		closeItem->setPosition(ccp(mBoard->getContentSize().width - closeItem->getContentSize().width + 13,
			mBoard->getContentSize().height - closeItem->getContentSize().height + 15));
		closeItem->setTag(TAG_ITEM_CLOSE);
		mMenu->addChild(closeItem);
	}

	mBoard->addChild(mMenu);//在属性对比 之前， 在装备底纹之后

	//
	CCMenuItemImage *item1, *item2;
	//
	ImageData data;
	BaseActor* actor = NULL;
	CalculateData calculatedata;
	PropertyCalculator calculator;
	string nameStr;
	CCLabelTTF *nameLabel = NULL;
	switch (mFrom)
	{
	case From_OtherHeroDetail://需要把按钮禁止掉（更换 工匠）
	case From_SpecialScene:
		mGongJiangMenu->setEnabled(false);
		break;
	case From_RenXingMianBan:
		//“有持有人”状态，则提供“更换”和“工匠”按钮。点击更换按钮切换到装备选择列表。
		//在选择列表上点击装备则进入目标装备的详情，并且为“目的装备”状态。
		//按钮
		if (equip->getActorId() != INVALID_ID)
		{
			item1 = CCMenuItemImage::create(
				"zhuangbei_xiangxi_genghuan.png"
				, "zhuangbei_xiangxi_genghuan_select.png"
				, this, menu_selector(EquipmentXiangQingBaseBody::menuItemClicked));
			mMenu->addChild(item1, 0, TAG_ITEM_CHANGE);
			item1->setPosition(POS_ITEM_1);
			//人像
			/*
			data = ImageManager::getManager()->getImageData_ProfessionHead(MyselfManager::getManager()->getMyTeamActor(mActorId)->getActorType());
			heroImg = BaseSprite::create(data.getCurrentImageFile());
			mBoard->addChild(heroImg);
			heroImg->setPosition(POS_TOUXIANG);
			heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
			heroImg->addChild(heroImgBg);
			heroImgBg->setPosition(ccp(heroImg->getContentSize().width/2, heroImg->getContentSize().height/2));
			*/
			//名字
			nameStr = LFStrings::getValue("zhuangBeiYu");
			nameStr += MyselfManager::getManager()->getMyTeamActor(equip->getActorId())->getCurrentProperty().nameInChinese;
			nameLabel = CCLabelTTF::create(nameStr.c_str(), fontStr_kaiti, LABEL_SIZE);
			mBoard->addChild(nameLabel);
			nameLabel->setAnchorPoint(ccp(1, 0.5));
			nameLabel->setPosition(POS_ActorName);
			nameLabel->setColor(ImageManager::getManager()->getShowColor(equip->getEquipColor(),false,equip->isPartOfSuit()));
		}
		break;
	case From_ZhuangBeiList:
		//“无持有人”状态，则提供“工匠”按钮。
		//按钮
		if(mActorId != INVALID_ID)//有持有人
		{
			//人像
			/*
			data = ImageManager::getManager()->getImageData_ProfessionHead(MyselfManager::getManager()->getMyTeamActor(mActorId)->getActorType());
			heroImg = BaseSprite::create(data.getCurrentImageFile());
			mBoard->addChild(heroImg);
			heroImg->setPosition(POS_TOUXIANG);
			heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
			heroImg->addChild(heroImgBg);
			heroImgBg->setPosition(ccp(heroImg->getContentSize().width/2, heroImg->getContentSize().height/2));
			*/
			//名字
			nameStr = LFStrings::getValue("zhuangBeiYu");
			nameStr += MyselfManager::getManager()->getMyTeamActor(equip->getActorId())->getCurrentProperty().nameInChinese;
			nameLabel = CCLabelTTF::create(nameStr.c_str(), fontStr_kaiti, LABEL_SIZE-2);
			mBoard->addChild(nameLabel);
			nameLabel->setAnchorPoint(ccp(1, 0.5));
			nameLabel->setPosition(POS_ActorName);
			ccColor3B color = ImageManager::getManager()->getShowColor(equip->getEquipColor(),false,equip->isPartOfSuit());
			nameLabel->setColor(color);
		}
		break;
	case From_GongJiang:
		//“工匠请求”状态，只显示装配人即可。
		if(mActorId != INVALID_ID)
		{//人像
			/*
			data = ImageManager::getManager()->getImageData_ProfessionHead(MyselfManager::getManager()->getMyTeamActor(mActorId)->getActorType());
			heroImg = BaseSprite::create(data.getCurrentImageFile());
			mBoard->addChild(heroImg);
			heroImg->setPosition(POS_TOUXIANG);
			heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
			heroImg->addChild(heroImgBg);
			heroImgBg->setPosition(ccp(heroImg->getContentSize().width/2, heroImg->getContentSize().height/2));
			*/
			//名字
			nameStr = LFStrings::getValue("zhuangBeiYu");
			nameStr += MyselfManager::getManager()->getMyTeamActor(equip->getActorId())->getCurrentProperty().nameInChinese;
			nameLabel = CCLabelTTF::create(nameStr.c_str(), fontStr_kaiti, LABEL_SIZE);
			mBoard->addChild(nameLabel);
			nameLabel->setAnchorPoint(ccp(1, 0.5));
			nameLabel->setPosition(POS_ActorName);
			ccColor3B color = ImageManager::getManager()->getShowColor(equip->getEquipColor(),false,equip->isPartOfSuit());
			nameLabel->setColor(color);
		}
		mGongJiangMenu->setEnabled(false);
		break;
	case From_ChangeZhuangBei:
		//“目的装备”状态，则提供“确定”和“原始对比”按钮，以及简单对比信息。
		//点击确定则直接换装。立即切换到人形界面。人形界面表演从原装切换为新装备的过程。
		//按钮
		mGongJiangMenu->setEnabled(false);
		item1 = CCMenuItemImage::create(
			"zhuangbei_xiangxi_mianban_queding.png"
			, "zhuangbei_xiangxi_mianban_queding_select.png"
			, this, menu_selector(EquipmentXiangQingBaseBody::menuItemClicked));
		mMenu->addChild(item1, 0, TAG_ITEM_QUEDING);
		item1->setPosition(POS_ITEM_1);
		item2 = CCMenuItemImage::create(
			"zhuangbei_xiangxi_yuanshiduibi.png"
			, "zhuangbei_xiangxi_yuanshiduibi_select.png"
			, this, menu_selector(EquipmentXiangQingBaseBody::menuItemClicked));
		mMenu->addChild(item2, 0, TAG_ITEM_YuanShi_BIJIAO);
		item2->setPosition(POS_ITEM_YuanShiBiJiao);
		if( ! isDisplay_YuanShiDuiBi_Item())
			item2->setVisible(false);
		//人像
		/*
		heroImgBg = BaseSprite::create("renxing_renwu_touxiang_select.png");
		mBoard->addChild(heroImgBg);
		heroImgBg->setPosition(POS_TOUXIANG);
		data = ImageManager::getManager()->getImageData_ProfessionHead(MyselfManager::getManager()->getMyTeamActor(mActorId)->getActorType());
		CCMenuItemImage *herobgItem = CCMenuItemImage::create(
			data.getCurrentImageFile().c_str()
			, data.getCurrentImageFile().c_str()
			, this, menu_selector(EquipmentXiangQingBaseBody::menuItemClicked));
		mMenu->addChild(herobgItem, 0, TAG_ITEM_BIJIAO_);
		herobgItem->setPosition(heroImgBg->getPosition());
		//名字
		nameStr = LFStrings::getValue("zhuangBeiYu");
		nameStr += MyselfManager::getManager()->getMyTeamActor(equip->getActorId())->getCurrentProperty().nameInChinese;
		nameLabel = CCLabelTTF::create(nameStr.c_str(), fontStr_kaiti, LABEL_SIZE);
		mBoard->addChild(nameLabel);
		nameLabel->setAnchorPoint(ccp(1, 0.5));
		nameLabel->setPosition(POS_ActorName);
		ccColor3B color = ImageManager::getManager()->getShowColor(equip->getEquipColor(),equip->isShenBing(),equip->isPartOfSuit());
		nameLabel->setColor(color);
		*/

		// 简单对比信息
		//board  是一个按钮，点击后进入详细对比
		CCMenuItemImage *barItem = CCMenuItemImage::create(
			"zhuangbei_xiangxi_mianban_shuxingt.png"
			, "zhuangbei_xiangxi_mianban_shuxingt.png"
            , "zhuangbei_xiangxi_mianban_shuxingt.png"
			, this, menu_selector(EquipmentXiangQingBaseBody::menuItemClicked));
		mMenu->addChild(barItem, 0, TAG_ITEM_XiangXi_BIJIAO);
		barItem->setPosition(ccp(mBoard->getContentSize().width/2, 838-720));
        barItem->setEnabled(false);
		// 换上当前装备的数据
		actor = MyselfManager::getManager()->getMyTeamActor(mActorId);
		calculatedata = actor->getCalculateDataWhenEquipOne(mEquip,g_YuanShiBiJiao_Equip);
		//
		int newHp = calculator.calculateHp(calculatedata);
		int newGongjili = calculator.calculateGongJiLi_MacroValue(calculatedata,Range_Average);
		int newHujia = calculator.calculateHuJiaZhi(calculatedata);
		int newKangxin = calculator.calculateKuangXingZhi(calculatedata);
		//
		int curHp = actor->getXueliang();
		int curGongjili = actor->getGongji_MacroValue();
		int curHujia = actor->getHujia();
		int curKangxin = actor->getMokang(); 
		addCompareInfoLabel(barItem, curHp, newHp, ccp(112,34), 25, fontStr_kaiti);
		addCompareInfoLabel(barItem, curGongjili, newGongjili, ccp(284,34), 25, fontStr_kaiti);
		addCompareInfoLabel(barItem, curHujia, newHujia, ccp(456,34), 25, fontStr_kaiti);
//		addCompareInfoLabel(barItem, curKangxin, newKangxin, ccp(504,34), 25, fontStr_kaiti);
		break;
	}

	return true;
}

void EquipmentXiangQingBaseBody::addCompareInfoLabel(CCNode* node, int curData,int newData,CCPoint fontLeftPos,float fontSize,string fontName)
{
// 	if (curData != newData)
// 	{
// 		CCLabelTTF* lable = LF::lable(mBoard,CCString::createWithFormat("%d",curData),fontLeftPos,fontSize,fontName);
// 		float width = lable->getContentSize().width;
// 		if (newData > curData)
// 		{
// 			LF::lable(
// 				mBoard,
// 				CCString::createWithFormat("+%d",newData-curData),
// 				ccp(fontLeftPos.x + width,fontLeftPos.y - lable->getContentSize().height*0.1f),
// 				fontSize*0.8f,fontName)->setColor(ccGREEN);
// 		}else
// 		{
// 			LF::lable(
// 				mBoard,
// 				CCString::createWithFormat("-%d",curData-newData),
// 				ccp(fontLeftPos.x + width,fontLeftPos.y - lable->getContentSize().height*0.1f),
// 				fontSize*0.8f,fontName)->setColor(ccRED);
// 		}
// 	}else
// 	{
// 		LF::lable(mBoard,CCString::createWithFormat("%d",curData),fontLeftPos,fontSize,fontName);
// 	}
	if (newData > curData)
	{
		LF::lable(node, CCString::createWithFormat("+%d",newData-curData), fontLeftPos, fontSize, fontName)->setColor(ccGREEN);
	}
	else if (newData == curData)
	{
		LF::lable(node, CCString::createWithFormat("+%d",newData-curData), fontLeftPos, fontSize, fontName)->setColor(ccWHITE);
	}
	else
	{
		LF::lable(node, CCString::createWithFormat("-%d",curData-newData), fontLeftPos, fontSize, fontName)->setColor(ccRED);
	}
}

void EquipmentXiangQingBaseBody::disableAllTouchBegin()
{
//	mMenu->setEnabled(false);
	mMenu->setVisible(false);
	mGongJiangMenu->setVisible(false);
}
void EquipmentXiangQingBaseBody::disableAllTouchEnd()
{
	refreshLayer();
//	mMenu->setEnabled(true);
	mMenu->setVisible(true);
	mGongJiangMenu->setVisible(true);
}

void EquipmentXiangQingBaseBody::popOneChildLayer(RenWuXiangXiBaseLayer* layer)
{
	addChild(layer);
	//layer->setPosition(getContentSize().width/2, getContentSize().height/2);
	layer->setPosition(0, 0);
}
void EquipmentXiangQingBaseBody::dismissMyselfLayer(bool someThingChanged)
{
	removeFromParent();
}

void EquipmentXiangQingBaseBody::replaceMyselfLayer(BaseLayer* layer)
{
	//把自己删除
}

void EquipmentXiangQingBaseBody::menuItemClicked(CCObject* sender)
{
	CCMenuItemImage* item = (CCMenuItemImage*)sender;
	int tag = item->getTag();

	if (tag == TAG_ITEM_CLOSE)
	{
		dismissMyselfLayer();
	}
}

void EquipmentXiangQingBaseBody::reportChangeZhuangBeiRsp(CCObject* msg_rsp)
{

}

EquipmentXiangQingBaseBody::~EquipmentXiangQingBaseBody(void)
{
}

void EquipmentXiangQingBaseBody::createEquipPropertyUI(float dt)
{
	Equipment *equip = mEquip;
	BaseSprite* base  = (BaseSprite*)mBoard->getChildByTag(TAG_XiangXiLeft);

	CCSize containerSize = CCSizeMake(640,838+Y_OFFSET);
	//类型
	float label_gap = 2;
	m_Label_y = containerSize.height - 78 ;
	float start_x = 250;
	if (mFrom == From_ZhuangBeiDuanZao)
	{
		start_x = 180;
	}
	{
		CCLabelTTF* label;
		//string str = CS::getZhuangbeiTypeName(equip->getType(),equip->getEquipColor());
		string str = CCString::createWithFormat("%d", equip->getEquipData().zhuangbei_level)->getCString() + LFStrings::getValue("ZhuangbeiXiangqing_ZhuangbeiLeixing");
		label = LF::lable(base,str,ccp(0,0),24,fontStr_kaiti);
		label->setAnchorPoint(ccp(0, 0.5));
		label->setPosition(ccp(start_x, m_Label_y - label_gap - label->getContentSize().height/2));

		ccColor3B color = ImageManager::getManager()->getShowColor(equip->getEquipColor(),false, false);
		label->setColor(color);
		m_Label_y = m_Label_y - label_gap - label->getContentSize().height -10;
	}
	CCLabelTTF* label;
	//白字
	{
		
		float baizi = 0;
		bool hasBaiZhi = false;
		switch(equip->getType())
		{
		case ZhuangbeiType_DanshouJian:
		case ZhuangbeiType_ShuangshouBishou:
		case ZhuangbeiType_ShuangshouFu:
		case ZhuangbeiType_DanshouFazhang:
		case ZhuangbeiType_ShuangshouFazhang:
		case ZhuangbeiType_Gong:
			hasBaiZhi = true;
			baizi = equip->getGongjiLiAfterQianghua(false);
			break;
		case ZhuangbeiType_Dunpai:
		case ZhuangbeiType_KuijiaZhong:
		case ZhuangbeiType_KuijiaQin:
			hasBaiZhi = true;
			baizi = equip->getHuJiaZhiAfterQianghua();
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
			label = LF::lable(base,str,ccp(0,0),80,fontStr_BookAntiqua);
			label->setColor(ccWHITE);
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(start_x, m_Label_y - label_gap - label->getContentSize().height/2));

			if (mFrom == From_ZhuangBeiDuanZao)
			{
				label->setFontSize(50);
				label->setPosition(ccp(start_x, m_Label_y - label_gap - label->getContentSize().height/2+10));
			}

			m_Label_y = m_Label_y - label_gap - label->getContentSize().height;
		}
	}
	//伤害详细
	if (equip->getEquipType() == EquipType_WuQi)
	{
// 		CCLabelTTF* label;
// 		char str[64];
// 		sprintf(str, "%u-%u ", equip->getEquipData().gongji_min, equip->getEquipData().gongji_max);
// 		label = LF::lable(base,str,ccp(0,0),LABEL_SIZE,fontStr);
// 		label->setColor(ccWHITE);
// 		label->setAnchorPoint(ccp(0, 0.5));
// 		label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));

		CCLabelTTF* label2;
		label2 = LF::lable(base,LFStrings::getValue("dianShangHai"),ccp(0,0),LABEL_SIZE,fontStr_kaiti);
// 		ccColor3B color;
// 		color.r = 100;
// 		color.g = 98;
// 		color.b = 97;
// 		label2->setColor(color);
		label2->setAnchorPoint(ccp(0, 0.5));
		label2->setPosition(ccp(label->getPosition().x + label->getContentSize().width, 
			label->getPositionY()-label->getContentSize().height/2+label2->getContentSize().height));
	}
	
	//等级
	{
// 		CCLabelTTF* label;
// 		char str[64];
// 		sprintf(str, "%s %u ", LFStrings::getValue("dengji").c_str(), equip->getEquipData().zhuangbei_level);
// 		label = LF::lable(base,str,ccp(0,0),LABEL_SIZE,fontStr_kaiti);
// 		ccColor3B color;
// 		color.r = 100;
// 		color.g = 98;
// 		color.b = 97;
// 		label->setColor(color);
// 		label->setAnchorPoint(ccp(0, 0.5));
// 		label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height));
// 
// 		label_y -= label_gap + label->getContentSize().height;
	}
	//魔法属性
	{
		m_Label_y = m_Label_y - label_gap - 10;

		std::vector<std::string> vec = equip->getAllMofaShuxing(false);

		for(int i=0; i<vec.size(); ++i)
		{
			BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
			base->addChild(biaoji);

			CCLabelTTF* label;
			label = LF::lable(base,vec[i],ccp(0,0),LABEL_SIZE,fontStr_kaiti);
			label->setColor(LABEL_COLOR);
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(start_x+biaoji->getContentSize().width + 10, m_Label_y));
			//
			biaoji->setPosition(ccp(start_x+biaoji->getContentSize().width/2, label->getPositionY()));

			m_Label_y = m_Label_y - label_gap - label->getContentSize().height;
		}
	}

	string info;
	//强化属性
	if(equip->canBeQianghua(info) && mFrom != From_ZhuangBeiDuanZao)
	{
		QianghuaData data = equip->getEquipData().qianghua_data;
		//sort
		for(int i=0; i!=data.qianghua_count; ++i)
		{
			for(int j=i; j!=data.qianghua_count; ++j)
			{
				if(  (data.info_arr[i].level < data.info_arr[j].level)   
					|| (data.info_arr[i].level == data.info_arr[j].level && 
					data.info_arr[i].percent < data.info_arr[j].percent)  )
				{
					swap(data.info_arr[i], data.info_arr[j]);
				}
			}
		}

		int qh_x = start_x+33;
		int qh_y = m_Label_y-70;
		int delta_x = 25;
		int total = 0;
		//已强化部分
		for(int i=0; i!=data.qianghua_count; ++i)
		{
			BaseSprite *xingxingImg = NULL;
			switch (data.info_arr[i].level)
			{
			case 4:
			case QianghuaLevel_jin_special:
				xingxingImg = BaseSprite::create("gongjiang_qianghua_jieguo_jin.png");
				break;
			case 3:
				xingxingImg = BaseSprite::create("gongjiang_qianghua_jieguo_yin.png");
				break;
			case 2:
				xingxingImg = BaseSprite::create("gongjiang_qianghua_jieguo_tong.png");
				break;
			case 1:
				xingxingImg = BaseSprite::create("gongjiang_qianghua_jieguo_tie.png");
				break;
			default:
				break;
			}
			base->addChild(xingxingImg);
			xingxingImg->setPosition(ccp(qh_x + i*delta_x, qh_y));
			xingxingImg->setScale(0.58f);
			total += data.info_arr[i].percent;
		}
		//未强化部分
		for(int i=data.qianghua_count; i!=data.max_qianghua_time; ++i)
		{
			BaseSprite *xingxingImg = BaseSprite::create("gongjiang_qianghua_jieguo_kong.png");
			base->addChild(xingxingImg);
			xingxingImg->setPosition(ccp(qh_x + i*delta_x, qh_y));
			xingxingImg->setScale(0.58f);
		}

		//分割线
		BaseSprite* pFengexian = BaseSprite::create("zhuangbei_xiangxi_mianban_fenge.png");
		base->addChild(pFengexian);
		pFengexian->setPosition(ccp(start_x+pFengexian->getContentSize().width/2-13, m_Label_y - label_gap - pFengexian->getContentSize().height/2));

		//小图标
		BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
		base->addChild(biaoji);
		biaoji->setPosition(ccp(start_x+biaoji->getContentSize().width/2, pFengexian->getPositionY() - pFengexian->getContentSize().height/2 - biaoji->getContentSize().height/2 - 5));

		//强化:
		CCLabelTTF* pQianghua = CCLabelTTF::create(LFStrings::getValue("ZhuangbeiXiangqing_Qianghua").c_str(), fontStr_kaiti, LABEL_SIZE);
		pQianghua->setColor(LABEL_COLOR);
		base->addChild(pQianghua);
		pQianghua->setPosition(ccp(biaoji->getPositionX()+biaoji->getContentSize().width/2 +pQianghua->getContentSize().width/2, 
			biaoji->getPositionY()));

		//百分比
		char buf[20];
		sprintf(buf, "+%d%%", total);
		CCLabelTTF *percentLabel = CCLabelTTF::create(buf, fontStr_kaiti, LABEL_SIZE);
		percentLabel->setColor(fonColor_ChengGong);
		base->addChild(percentLabel);
		percentLabel->setAnchorPoint(ccp(0.0f, 0.5));
		percentLabel->setPosition(ccp(pQianghua->getPositionX()+pQianghua->getContentSize().width/2, pQianghua->getPositionY()));

		//强化按钮
		CCMenuItemImage *qianghuaItem = CCMenuItemImage::create(
			"zhuangbei_xiangxi_gongjiang_qianghua.png"
			, "zhuangbei_xiangxi_gongjiang_qianghua_select.png"
			, "zhuangbei_xiangxi_gongjiang_qianghua_select.png"
			, this, menu_selector(EquipmentXiangQingBaseBody::menuItemGongJiang));
		mGongJiangMenu->addChild(qianghuaItem, 1, Tag_Item_Qianghua);
		//qianghuaItem->setPosition(POS_Item_QiangHua);
		qianghuaItem->setPosition(ccp(mWinSize.width-qianghuaItem->getContentSize().width/2 -10, 
			pQianghua->getPositionY() - qianghuaItem->getContentSize().height/2+10));

		m_Label_y = qianghuaItem->getPositionY() - 3*pQianghua->getContentSize().height/2;
	}

	int side_y = 0;
	//洗炼
	string xilian;
	if (equip->canPurify(xilian,1) && mFrom != From_ZhuangBeiDuanZao)
	{
		//分割线
		BaseSprite* pFengexian = BaseSprite::create("zhuangbei_xiangxi_mianban_fenge.png");
		base->addChild(pFengexian);
		pFengexian->setPosition(ccp(start_x+pFengexian->getContentSize().width/2-13, m_Label_y - label_gap - pFengexian->getContentSize().height/2));

		//小图标
		BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
		base->addChild(biaoji);
		biaoji->setPosition(ccp(start_x+biaoji->getContentSize().width/2, pFengexian->getPositionY() - pFengexian->getContentSize().height/2 - biaoji->getContentSize().height/2 - 5));

		//洗练:
		CCLabelTTF* pXilian = CCLabelTTF::create(LFStrings::getValue("ZhuangbeiXiangqing_Xilian").c_str(), fontStr_kaiti, LABEL_SIZE);
		pXilian->setColor(LABEL_COLOR);
		base->addChild(pXilian);
		pXilian->setPosition(ccp(biaoji->getPositionX()+biaoji->getContentSize().width/2 +pXilian->getContentSize().width/2, 
			biaoji->getPositionY()));

		vector<ZBAttr> data_list = equip->getEquipData().purify_attr_vec;

		int max_count = CS::getPurifyMaxHoles(equip->getEquipData().zhuangbei_type,equip->getEquipColor(),equip->getLevel());
		int count = data_list.size();

		int qh_x = start_x+33;
		int qh_y = m_Label_y-70;
		int delta_y = 25;

		int atter_type_index[5] = {AttrType_baoshang,AttrType_hp_percent,AttrType_skillresult,equip->getMainProType(),0};
		string name_str[4] = {"gongjiang_qianghua_jieguo_tie.png","gongjiang_qianghua_jieguo_tong.png","gongjiang_qianghua_jieguo_yin.png","gongjiang_qianghua_jieguo_jin.png"};
		for(int i=0; i != max_count ; ++i)
		{
			BaseSprite *xingxingImg = NULL;
			if (i < count)
			{
				xingxingImg = BaseSprite::create(name_str[ ( CS::getPurifyLevel(data_list[i].attr_value) + 1 ) / 2 - 1].c_str());
				base->addChild(xingxingImg);
				xingxingImg->setPosition(ccp(qh_x, qh_y - i * delta_y));
				xingxingImg->setScale(0.58f);

				ZBAttr zb_attr;
				zb_attr.attr_type = atter_type_index[i];
				zb_attr.attr_value = data_list[i].attr_value;
				string attrStr = GetMoFaShuXing_ByZBAttr(zb_attr, true);
				CCLabelTTF *label = CCLabelTTF::create(attrStr.c_str(), fontStr_kaiti, LABEL_SIZE);
				base->addChild(label);
				label->setColor(LABEL_COLOR);
				label->setPosition(ccp(qh_x + delta_y + label->getContentSize().width / 2, qh_y - i * delta_y));
			}
			else
			{
				xingxingImg = BaseSprite::create("zhuangbei_xiangxi_xilian_xilianqian_suo.png");
				base->addChild(xingxingImg);
				xingxingImg->setPosition(ccp(qh_x, qh_y - i * delta_y));
				xingxingImg->setScale(0.58f);
				CCLabelTTF *label = CCLabelTTF::create(LFStrings::getValue("weijihuo").c_str(), fontStr_kaiti, LABEL_SIZE);
				base->addChild(label);
				label->setColor(ccc3(127,127,127));
				label->setPosition(ccp(qh_x + delta_y + label->getContentSize().width / 2, qh_y - i * delta_y));
			}
			side_y += delta_y;

		}
		side_y += ( delta_y + 10 );
				
		m_Label_y = qh_y - side_y + 40;

// 		//百分比
// 		char buf[20];
// 		sprintf(buf, "+%d%%", total);
// 		CCLabelTTF *percentLabel = CCLabelTTF::create(buf, fontStr_kaiti, LABEL_SIZE);
// 		percentLabel->setColor(fonColor_ChengGong);
// 		base->addChild(percentLabel);
// 		percentLabel->setAnchorPoint(ccp(0.0f, 0.5));
// 		percentLabel->setPosition(ccp(pXilian->getPositionX()+pXilian->getContentSize().width/2, pXilian->getPositionY()));

		//洗练按钮
		CCMenuItemImage *pXilianItem = CCMenuItemImage::create(
			"zhuangbei_xiangxi_gongjiang_xilian.png"
			, "zhuangbei_xiangxi_gongjiang_xilian_select.png"
			, "zhuangbei_xiangxi_gongjiang_xilian_select.png"
			, this, menu_selector(EquipmentXiangQingBaseBody::menuItemGongJiang));
		mGongJiangMenu->addChild(pXilianItem, 1, Tag_Item_Xilian);
		//if(equip->canBeQianghua(info) || equip->getEquipData().qianghua_data.qianghua_count>0)
		{
			//jinglianItem->setPosition(POS_Item_JingLian);
		}
		//else 
		{
			pXilianItem->setPosition(ccp(mWinSize.width-pXilianItem->getContentSize().width/2 -10, 
				pFengexian->getPositionY() - pXilianItem->getContentSize().height/2 + 20 - side_y/2));
		}

		/*bool isFirstXilian = false;
		if (!History::getHistory()->getBoolFromCurrentHero(Key_First_LookXilian,isFirstXilian))
		{
			History::getHistory()->putInCurrentHero(Key_First_LookXilian,false);
			CCPoint pos = jinglianItem->getParent()->convertToWorldSpace(jinglianItem->getPosition());
			pos.y += 115;
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(jinglianItem->getContentSize(),pos
				, LFStrings::getValue("first_look_chuanqi_shuoming"), NULL, false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}*/
			
	}

	//精炼
	string temp;
	if (equip->canBeJingLian(temp) && mFrom != From_ZhuangBeiDuanZao)
	{

		//分割线
		BaseSprite* pFengexian = BaseSprite::create("zhuangbei_xiangxi_mianban_fenge.png");
		base->addChild(pFengexian);
		pFengexian->setPosition(ccp(start_x+pFengexian->getContentSize().width/2-13, m_Label_y - label_gap - pFengexian->getContentSize().height/2));

		//小图标
		BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
		base->addChild(biaoji);
		biaoji->setPosition(ccp(start_x+biaoji->getContentSize().width/2, pFengexian->getPositionY() - pFengexian->getContentSize().height/2 - biaoji->getContentSize().height/2 - 5));

		//精炼:
		CCLabelTTF* pJinglian = CCLabelTTF::create(LFStrings::getValue("ZhuangbeiXiangqing_Jinglian").c_str(), fontStr_kaiti, LABEL_SIZE);
		pJinglian->setColor(LABEL_COLOR);
		base->addChild(pJinglian);
		pJinglian->setPosition(ccp(biaoji->getPositionX()+biaoji->getContentSize().width/2 +pJinglian->getContentSize().width/2, 
			biaoji->getPositionY()));

		int time = equip->getEquipData().jinglian_times;
		
		//精炼次数图标
		BaseSprite *icon = BaseSprite::create("gongjiang_jinglian_tubiao.png");
		base->addChild(icon, 1);
		icon->setPosition(ccp(start_x + icon->getContentSize().width/2+18, pJinglian->getPositionY() - pJinglian->getContentSize().height - label_gap - icon->getContentSize().height/2));

		CCLabelTTF* pJinglianCishu = CCLabelTTF::create(CCString::createWithFormat("+%d", time)->getCString(), fontStr_kaiti, LABEL_SIZE);
		pJinglianCishu->setColor(LABEL_COLOR);
		pJinglianCishu->setAnchorPoint(ccp(0.0f, 0.5f));
		base->addChild(pJinglianCishu);
		pJinglianCishu->setPosition(ccp(icon->getPositionX()+icon->getContentSize().width/2 + 10, icon->getPositionY()));

		CCLabelTTF *pLabel = NULL;
		if (time > 0)
		{
			//新接口
			ZBAttr zb_attr;
			zb_attr.attr_type = equip->getMainProType();// 如果是攻击，统一返回最小攻击
			zb_attr.attr_value = equip->getPropertyAddRatioByEachJingLian();
			zb_attr.attr_value *= (time);
			string attrStr = GetMoFaShuXing_ByZBAttr(zb_attr, true);
			pLabel = CCLabelTTF::create(attrStr.c_str(), fontStr_kaiti, LABEL_SIZE);
		}
// 		else
// 		{
// 			pLabel = CCLabelTTF::create(CCString::createWithFormat("+%d", time)->getCString(), fontStr_kaiti, LABEL_SIZE);
//		}
		if (pLabel)
		{
			base->addChild(pLabel);
			pLabel->setAnchorPoint(ccp(0.0f, 0.5f));
			pLabel->setColor(fonColor_ChengGong);
			pLabel->setPosition(ccp(pJinglian->getPositionX()+pJinglian->getContentSize().width/2, pJinglian->getPositionY()));
		}

		CCMenuItemImage *jinglianItem = CCMenuItemImage::create(
			"zhuangbei_xiangxi_gongjiang_jinglian.png"
			, "zhuangbei_xiangxi_gongjiang_jinglian_select.png"
			, "zhuangbei_xiangxi_gongjiang_jinglian_select.png"
			, this, menu_selector(EquipmentXiangQingBaseBody::menuItemGongJiang));
		mGongJiangMenu->addChild(jinglianItem, 1, Tag_Item_JiangLian);
		jinglianItem->setPosition(ccp(mWinSize.width-jinglianItem->getContentSize().width/2 -10, 
			pJinglian->getPositionY() - jinglianItem->getContentSize().height/2+10));

		m_Label_y = m_Label_y - jinglianItem->getContentSize().height - label_gap;
	}

	//宝石
	if (equip->getEquipData().hole > 0 && mFrom != From_ZhuangBeiDuanZao) //有孔
	{
		CCMenuItemImage *baoshiItem = NULL;
// 		if (! equip->canBeJianDing(info))
// 		{
// 			baoshiItem= CCMenuItemImage::create(
// 				"zhuangbei_xiangxi_gongjiang_baoshi1.png",
// 				"zhuangbei_xiangxi_gongjiang_baoshi1_select.png",
// 				"zhuangbei_xiangxi_gongjiang_baoshi1_suoding.png",
// 				this, menu_selector(EquipmentXiangQingBaseBody::menuItemGongJiang));
// 			baoshiItem->setPosition(ccp(430, containerSize.height-484));
// 		}
// 		else
		{
			//

		}
		m_Label_y = m_Label_y - 20;
		//分割线
		BaseSprite* pFengexian = BaseSprite::create("zhuangbei_xiangxi_mianban_fenge.png");
		base->addChild(pFengexian);
		pFengexian->setPosition(ccp(start_x+pFengexian->getContentSize().width/2-13, m_Label_y - label_gap - pFengexian->getContentSize().height/2));

		//标记
		BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
		base->addChild(biaoji);
		biaoji->setPosition(ccp(start_x+biaoji->getContentSize().width/2, 
			pFengexian->getPositionY()-pFengexian->getContentSize().height/2-biaoji->getContentSize().height/2));

		//宝石
		CCLabelTTF* pBaoshi = CCLabelTTF::create(LFStrings::getValue("ZhuangbeiXiangqing_Baoshi").c_str(), fontStr_kaiti, LABEL_SIZE);
		pBaoshi->setColor(LABEL_COLOR);
		base->addChild(pBaoshi);
		pBaoshi->setPosition(ccp(biaoji->getPositionX()+biaoji->getContentSize().width/2 +pBaoshi->getContentSize().width/2, 
			biaoji->getPositionY()));

		m_Label_y = m_Label_y - pBaoshi->getContentSize().height;
	
		//宝石属性
		std::vector<Equipment::BaoshiShuxing> vec = equip->getAllBaoshiShuxing();
		for(int i=0; i<vec.size(); ++i)
		{
			BaseSprite *hole = BaseSprite::create("zhuangbei_kong.png");
			base->addChild(hole);
			hole->setScale(0.35f);
			hole->setPosition(ccp(pBaoshi->getPositionX()-pBaoshi->getContentSize().width/2+hole->getContentSize().width/2, 
				pBaoshi->getPositionY()-pBaoshi->getContentSize().height/2-hole->getContentSize().height/2*hole->getScale() - 10 - i*30));

			BaseSprite* baoshi = BaseSprite::create(ImageManager::getManager()->getBaoshiFilename(vec[i].type));
			base->addChild(baoshi);
			baoshi->setScale(0.35f);
			baoshi->setPosition(hole->getPosition());

			CCLabelTTF* label;
			label = LF::lable(base,vec[i].shuxing,ccp(0,0),LABEL_SIZE,fontStr_kaiti);
			label->setColor(LABEL_COLOR);
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(hole->getPositionX() + hole->getContentSize().width*hole->getScale() + 5, hole->getPositionY()));
		}

		int iYOffset = vec.size()*30;

		baoshiItem= CCMenuItemImage::create(
			"zhuangbei_xiangxi_gongjiang_baoshi1.png",
			"zhuangbei_xiangxi_gongjiang_baoshi1_select.png",
			"zhuangbei_xiangxi_gongjiang_baoshi1_select.png",
			this, menu_selector(EquipmentXiangQingBaseBody::menuItemGongJiang));
		mGongJiangMenu->addChild(baoshiItem, 1, Tag_Item_BaoShi);
//		baoshiItem->setPosition(ccp(start_x , label_y));
		baoshiItem->setPosition(ccp(mWinSize.width-baoshiItem->getContentSize().width/2 -10, 
			pFengexian->getPositionY() - baoshiItem->getContentSize().height/2 - iYOffset/2));

		m_Label_y = m_Label_y - iYOffset - 30;
	}
	//传奇升级
	if (equip->getEquipColor() == ZhuangbeiColour_Chuanqi && mFrom != From_ZhuangBeiDuanZao)
	{
		int nextLv = CS::getNextShenBingLevel(equip->getLevel());
		CCMenuItemImage *sbLevUp = NULL;
		if (nextLv < 0)
		{
			sbLevUp = CCMenuItemImage::create(
				"zhuangbei_xiangxi_shengji_max.png",
				"zhuangbei_xiangxi_shengji_max.png",
				NULL, this , NULL);
			sbLevUp->setPosition(ccp(75+sbLevUp->getContentSize().width/2, 838-652));
		}
		else 
		{
			sbLevUp = CCMenuItemImage::create(
				"zhuangbei_xiangxi_shenbinshengji.png",
				"zhuangbei_xiangxi_shenbinshengji_select.png",
				this , menu_selector(EquipmentXiangQingBaseBody::menuItemGongJiang));
			sbLevUp->setPosition(ccp(15+sbLevUp->getContentSize().width/2, 838-642));
		}
		mGongJiangMenu->addChild(sbLevUp, 1, Tag_Item_ShenBingLevUp);
		
	}
	//鉴定
	/*if (equip->canBeJianDing(info)) 
	{
		CCMenuItemImage *jiandingItem = NULL;
		if (equip->getEquipData().hole > 0)
		{
			jiandingItem = CCMenuItemImage::create(
				"zhuangbei_xiangxi_jianding.png"
				, "zhuangbei_xiangxi_jianding_select.png"
				, "zhuangbei_xiangxi_jianding_suoding.png"
				, this, menu_selector(EquipmentXiangQingBaseBody::menuItemGongJiang));
			jiandingItem->setPosition(ccp(523, containerSize.height-484));
		}
		else
		{
			jiandingItem = CCMenuItemImage::create(
				"zhuangbei_xiangxi_gongjiang_jianding1.png"
				, "zhuangbei_xiangxi_gongjiang_jianding1_select.png"
				, "zhuangbei_xiangxi_gongjiang_jianding1_suoding.png"
				, this, menu_selector(EquipmentXiangQingBaseBody::menuItemGongJiang));
			jiandingItem->setPosition(ccp(430, containerSize.height-484));
		}
		mGongJiangMenu->addChild(jiandingItem, 1, Tag_Item_JianDing);
		//鉴定属性
		int bj_X = 455; 
		if (equip->getEquipData().hole == 0)   bj_X = start_x+11;
		int row_gap = 25;
		int jdNum = 0;
		vector<string> knowShuxing = equip->getKnownHidenMoFaShuxing();
		for (jdNum=0; jdNum!=knowShuxing.size(); ++jdNum)
		{
			BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
			base->addChild(biaoji);
			biaoji->setPosition(ccp(bj_X, containerSize.height-529 - jdNum * row_gap));

			CCLabelTTF* label = CCLabelTTF::create(knowShuxing.at(jdNum).c_str(), fontStr_kaiti, LABEL_SIZE);
			base->addChild(label);
			label->setColor(LABEL_COLOR);
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(bj_X+16, containerSize.height-529 - jdNum * row_gap));
		}
		for (; jdNum!=equip->getKnownHidenMoFaShuxing().size()+equip->getCurrentUnkownHideAttriNum(); ++jdNum)
		{
			CCLabelTTF* label = CCLabelTTF::create("?????", fontStr_kaiti, LABEL_SIZE);
			base->addChild(label);
			label->setColor(LABEL_COLOR);
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(bj_X+16, containerSize.height-529 - jdNum * row_gap));
		}
	}*/
	

	//篆刻
	//暂时关闭此功能
/*	string infor;
	if (equip->canBeZhuanKe(infor))
	{
		CCMenuItemImage *zhuankeItem = CCMenuItemImage::create(
			"zhuangbei_xiangxi_gongjiang_zhuanke.png"
			, "zhuangbei_xiangxi_gongjiang_zhuanke_select.png"
			, "zhuangbei_xiangxi_gongjiang_zhuanke_suoding.png"
			, this, menu_selector(EquipmentXiangQingBaseBody::menuItemGongJiang));
		mGongJiangMenu->addChild(zhuankeItem, 1, Tag_Item_ZhuanKe);
		if(equip->canBeQianghua(info) || equip->getEquipData().qianghua_data.qianghua_count>0)
			zhuankeItem->setPosition(POS_Item_ZhuanKe);
		else 
			zhuankeItem->setPosition(ccp(POS_Item_ZhuanKe.x, POS_Item_QiangHua.y-(POS_Item_JingLian.y-POS_Item_ZhuanKe.y)));
		//
		if (equip->haveZhuanke())
		{
			vector<string> strs = equip->getZhuanKeMoFaShuxing();
			for (int i=0; i!=strs.size(); ++i)
			{
				CCLabelTTF *label = CCLabelTTF::create(strs.at(i).c_str(), fontStr_kaiti, LABEL_SIZE);
				base->addChild(label);
				label->setColor(LABEL_COLOR);
				label->setPosition(ccp(zhuankeItem->getPositionX(), zhuankeItem->getPositionY()-zhuankeItem->getContentSize().height/2
					- 10 - i*label->getContentSize().height));
			}	
		}	
	}*/

	//套装信息
	scheduleOnce(schedule_selector(EquipmentXiangQingBaseBody::createTaoZhuangShuXing), 0);	

	//描述
	{
		float Y = containerSize.height - 685 - Y_OFFSET;
		CCSize size = CCSizeMake(590, 60);
		if (mFrom == From_ZhuangBeiDuanZao)
		{
			Y = Y-85;
			size = CCSizeMake(340, 60);
		}

		
		CCLabelTTF* label = CCLabelTTF::create(equip->getEquipData().equipDicrib.c_str()
			, fontStr_kaiti, LABEL_SIZE, size, kCCTextAlignmentLeft );
		base->addChild(label);
		label->setAnchorPoint(ccp(0, 1));
		label->setPosition(ccp(25, Y));
		label->setColor(ccc3(188, 179, 138));
		
	}

}

void EquipmentXiangQingBaseBody::createTaoZhuangShuXing(float dt)
{
	OneSuitInfo suitInfo;
	if(SuitPool::getPool()->getSuitInforByZBGroupId(mEquip->getGroupId(), suitInfo))
	{
		BaseSprite* base  = (BaseSprite*)mBoard->getChildByTag(TAG_XiangXiLeft);

		//CCSize containerSize = CCSizeMake(640,838+Y_OFFSET);
		//类型
		float label_gap = 2;
		//float label_y = containerSize.height - 78;
		float start_x = 250;

		if (mFrom == From_ZhuangBeiDuanZao)
		{
			start_x = 180;
		}
		

		//分割线
		BaseSprite* pFengexian = BaseSprite::create("zhuangbei_xiangxi_mianban_fenge.png");
		base->addChild(pFengexian);
		pFengexian->setPosition(ccp(start_x+pFengexian->getContentSize().width/2-13, m_Label_y - label_gap - pFengexian->getContentSize().height/2 - 20));

		if (mFrom == From_ZhuangBeiDuanZao)
		{
			pFengexian->setScaleX(0.7f);
		}
		pFengexian->setPosition(ccp(start_x+pFengexian->getContentSize().width/2*pFengexian->getScaleX()-13, m_Label_y - label_gap - pFengexian->getContentSize().height/2 - 20));


		//标记
		BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
		base->addChild(biaoji);
		biaoji->setPosition(ccp(start_x+biaoji->getContentSize().width/2, 
			pFengexian->getPositionY()-pFengexian->getContentSize().height/2-biaoji->getContentSize().height/2));

		//套装效果
		CCLabelTTF* pTaozhuangxiaoguo = CCLabelTTF::create(LFStrings::getValue("ZhuangbeiXiangqing_ZhuangbeiXiaoguo").c_str(), fontStr_kaiti, LABEL_SIZE);
		pTaozhuangxiaoguo->setColor(LABEL_COLOR);
		base->addChild(pTaozhuangxiaoguo);
		pTaozhuangxiaoguo->setPosition(ccp(biaoji->getPositionX()+biaoji->getContentSize().width/2 +pTaozhuangxiaoguo->getContentSize().width/2, 
			biaoji->getPositionY()));

		m_Label_y =  pTaozhuangxiaoguo->getPositionY()-pFengexian->getContentSize().height/2 - 25;


		ccColor3B cqColor = fonColor_ChengGong;//ImageManager::getManager()->getShowColor(ZhuangbeiColour_Chuanqi,false,false);

		//穿在身上的能触发套装属性的套装件数、 groupId
		vector<unsigned int> _vec;
		if (mEquip->getActorId() != INVALID_ID)
		{
			//自己英雄的
			if (MyselfManager::getManager()->getMyTeamActor(mEquip->getActorId()) != NULL)
			{
				vector<Equipment*> vecEquip = MyselfManager::getManager()->getMyTeamActor(mEquip->getActorId())->getEquipments();
				vector<unsigned int> equipedOnHero_groupId;
				for (int curI=0; curI<vecEquip.size(); ++curI)
					equipedOnHero_groupId.push_back(vecEquip.at(curI)->getGroupId());
				_vec = SuitPool::getPool()->getInSuitedGroupIdsFromTheGroupId(equipedOnHero_groupId, mEquip->getGroupId());
			}
			//查询其他英雄时出现的
			else
			{
				ZhuangBeiMianBanOther *layer_other = dynamic_cast<ZhuangBeiMianBanOther*>(mFooterLayer->getTopLayer(-1));
				ZhuangBeiMianBanYuanShen *layer_yuanshen = dynamic_cast<ZhuangBeiMianBanYuanShen*>(mFooterLayer->getTopLayer(-1));
				if (layer_other || layer_yuanshen)
				{ 
					vector<EquipData> vecData;
					if (layer_other) vecData = layer_other->getEquipsDataByActorId(mEquip->getActorId());
					else             vecData = layer_yuanshen->getEquipsDataByActorId(mEquip->getActorId());

					vector<unsigned int> equipedOnHero_groupId;
					for (int curI=0; curI<vecData.size(); ++curI)
						equipedOnHero_groupId.push_back(vecData.at(curI).group_id);
					_vec = SuitPool::getPool()->getInSuitedGroupIdsFromTheGroupId(equipedOnHero_groupId, mEquip->getGroupId());
				}
				else
				{
					CCLog("Error: %s --> 查询其他英雄的套装时 没找到ZhuangBeiMianBanOther", __FUNCTION__);
				}
			}
		}
		//当前详情界面的这件套装 有没有穿上、 没穿 则只有它的名字是白色、套装属性一个也没有
		//只穿这一件equipedGroupIdVec.size == 1  和  没有穿它equipedGroupIdVec.size == 0
		int taozhuangNum = _vec.size(); //记录有多少件
//		m_Label_y = 838+Y_OFFSET-706+146;

		// 1 name
// 		CCString tempStr;
// 		tempStr.initWithFormat("%s(%s%d/%d)", equip->getName().c_str(), LFStrings::getValue("TaoZhuang").c_str(), taozhuangNum, suitInfo.parts_of_suit.size());
// 		CCLabelTTF *labelName = CCLabelTTF::create(tempStr.getCString(), fontStr_kaiti, LABEL_SIZE);
// 		base->addChild(labelName);
// 		labelName->setAnchorPoint(ccp(0, 0.5));
// 		labelName->setPosition(ccp(start_x, m_Label_y));
// 		labelName->setColor(cqColor);
// 		m_Label_y -= (labelName->getContentSize().height + label_gap);

		// 2 套装属性显示   (2)件
		char buf[32];
		for(int i=0; i<suitInfo.parts_of_suit.size()-1; ++i)
		{
			ccColor3B color;
			if(i+2 <= taozhuangNum)
				color = cqColor;
			else 
				color = fonColor_CiYao;
			// 2、3、4、5件
			sprintf(buf, "(%d)", i+2);
			string strjian = buf + LFStrings::getValue("jian");
			CCLabelTTF *labelJian = CCLabelTTF::create(strjian.c_str(), fontStr_kaiti, LABEL_SIZE-2);
			base->addChild(labelJian);
			labelJian->setAnchorPoint(ccp(0, 0.5));
			labelJian->setPosition(ccp(pTaozhuangxiaoguo->getPositionX()-pTaozhuangxiaoguo->getContentSize().width/2, m_Label_y));
			labelJian->setColor(color);
			//属性
			for (int j=0; j<=1; ++j)
			{
				if (suitInfo.suit_attr[i][j].attr_type == 0)
					continue;

				CCLog("%s ---> [i=%d][j=%d]", __FUNCTION__, i, j);
				string strShuXing = GetMoFaShuXing_ByZBAttr(suitInfo.suit_attr[i][j]);
				CCLabelTTF *labelShuxing = CCLabelTTF::create(strShuXing.c_str(), fontStr_kaiti, LABEL_SIZE-2);
				base->addChild(labelShuxing);
				labelShuxing->setAnchorPoint(ccp(0, 0.5));
				if (j > 0) m_Label_y -= (labelShuxing->getContentSize().height + label_gap);
				labelShuxing->setPosition(ccp(labelJian->getPositionX() + labelJian->getContentSize().width + 6, m_Label_y));
				labelShuxing->setColor(color);
			}

			m_Label_y -= (labelJian->getContentSize().height + label_gap);
		}
	}
}

bool EquipmentXiangQingBaseBody::isDisplay_YuanShiDuiBi_Item()
{	
	if(g_YuanShiBiJiao_Equip == NULL)
		return false;
	else
		return true;
}