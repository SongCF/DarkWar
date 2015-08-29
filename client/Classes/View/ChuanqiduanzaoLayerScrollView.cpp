#include "ChuanqiduanzaoLayerScrollView.h"
#include "MyselfManager.h"
#include "font.h"
#include "SuitPool.h"
#include "GameDirector.h"
#include "CmdHelper.h"
#include "flat.h"
#include "MessageDisplay.h"
#include "ClientErrorCode.h"
#include "WuPinManager.h"
#include "ShenBingShangDianDuanZao.h"
#include "MainLayerStoreFrame.h"
#include "ShenBingPool.h"
#include "ShenBingShangDian.h"
#include "TaoZhuangInfoUI.h"
USING_NS_CC;

ChuanqiduanzaoLayerScrollView::ChuanqiduanzaoLayerScrollView(CCSize viewPortSize,int gridNum,int gridNumInX,int gridNumInY,float gridPadding /* = 10.0f */,CCPoint scrollViewPortOffset /* = CCPointZero */,float pageInval /* = 40.0f */)
	:GridScrollView(viewPortSize,gridNum,gridNumInX,gridNumInY,gridPadding,scrollViewPortOffset,pageInval)
{
	m_iCurPageIndex = 0;
}

ChuanqiduanzaoLayerScrollView::~ChuanqiduanzaoLayerScrollView(void)
{
}

ChuanqiduanzaoLayerScrollView* ChuanqiduanzaoLayerScrollView::createView(CCSize viewPortSize,int gridNum,int gridNumInX,int gridNumInY,float gridPadding /* = 10.0f */,CCPoint scrollViewPortOffset /* = CCPointZero */,float pageInval /* = 40.0f */)
{
	ChuanqiduanzaoLayerScrollView* pView = new ChuanqiduanzaoLayerScrollView(viewPortSize, gridNum, gridNumInX, gridNumInY);
	pView->autorelease();
	return pView;
}

bool sortActorInfoByProfId(const BaseActor *a, const BaseActor *b)
{
	return a->getCurrentProperty().profession < b->getCurrentProperty().profession;
}

ClickableItem* ChuanqiduanzaoLayerScrollView::createGridItem(int pageIndex,int xIndex,int yIndex)
{
	int index = pageIndex*gridNumInX*gridNumInY+xIndex+yIndex*gridNumInX;
	vector<BaseActor*> myteamInfo = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
	sort(myteamInfo.begin(), myteamInfo.end(), sortActorInfoByProfId);
	ChuanqiduanzaoItem* pItem = NULL;
	if (index >= 0 && index < myteamInfo.size())
	{
		pItem = ChuanqiduanzaoItem::createChuanqiduanzaoItem(myteamInfo.at(index));
	}

	return pItem;
}

void ChuanqiduanzaoLayerScrollView::pageSelectedEnd(int pageIndex)
{
	CCLOG("pageindex is %d", pageIndex);
	m_iCurPageIndex = pageIndex;
	ShenBingShangDian* pParent = dynamic_cast<ShenBingShangDian*>(this->getParent());
	if (pParent)
	{
		pParent->changePageIndexEnd(m_iCurPageIndex);
	}
}

ChuanqiduanzaoItem::ChuanqiduanzaoItem()
{

}

ChuanqiduanzaoItem::~ChuanqiduanzaoItem()
{

}

ChuanqiduanzaoItem* ChuanqiduanzaoItem::createChuanqiduanzaoItem(BaseActor* info)
{
	ChuanqiduanzaoItem* pItem = new ChuanqiduanzaoItem();
	if (pItem->initItem(info))
	{
		pItem->autorelease();
		pItem->setClickable(false);
		return pItem;
	}
	CC_SAFE_DELETE(pItem);
	return NULL;
}

bool ChuanqiduanzaoItem::initItem(BaseActor* info)
{
	bool bret = false;

	//初始化背景
	if (CCSprite::initWithFile("shangdian_chuanqi_bg.png"))
	{
		bret = true;
	}

	if (bret)
	{
		m_Profession_id = info->getCurrentProperty().profession;
		//顶部文字显示
		CCLabelTTF* pTopTip = CCLabelTTF::create(LFStrings::getValue("ShenBingDuanZao_ShuoMingTiShi").c_str(), fontStr_kaiti, 18);
		pTopTip->setPosition(ccp(this->getContentSize().width/2, this->getContentSize().height-70));
		pTopTip->setColor(fonColor_FaGuang);
		this->addChild(pTopTip);

		//头像
		CCSprite* pIcon = CCSprite::create(this->getIconFileNameByType(info->getActorType()).c_str());
		this->addChild(pIcon);
		pIcon->setPosition(ccp(this->getContentSize().width/2, 400));

		//美术字(英雄类型)
		CCSprite* pHeroType = CCSprite::create(this->getHeroTypeFileNameByType(info->getActorType()).c_str());
		this->addChild(pHeroType);
		pHeroType->setPosition(ccp(this->getContentSize().width/2, 
			pIcon->getPositionY()-pIcon->getContentSize().height/2-pHeroType->getContentSize().height+2));


		//套装名字
		string zhuangBeiStr = ChuanqiPool::getPool()->getChuanQiSuitName(info->getActorType());
		CCLabelTTF* pName = CCLabelTTF::create(zhuangBeiStr.c_str(), fontStr_kaiti, 20);
		pName->setPosition(ccp(this->getContentSize().width/2, 280));
		this->addChild(pName);

		CCMenu* pMenu = CCMenu::create();
		this->addChild(pMenu);
		pMenu->setPosition(CCPointZero);
		//锻造一次
		CCMenuItemImage *yici = CCMenuItemImage::create(
			"shangdian_shenbin_anniu_duanzaoyici.png",
			"shangdian_shenbin_anniu_duanzaoyici_select.png",
			this, menu_selector(ChuanqiduanzaoItem::menuItemClicked_DuanZao));
		pMenu->addChild(yici, 0, Tag_MenuItem_Yici);
		yici->setPosition(ccp(this->getContentSize().width/2-yici->getContentSize().width/2, 185));
		yici->setUserData((void*)info->getActorType());

		//锻造十次
		CCMenuItemImage *shici = CCMenuItemImage::create(
			"shangdian_shenbin_anniu_duanzaoshici.png",
			"shangdian_shenbin_anniu_duanzaoshici_select.png",
			this, menu_selector(ChuanqiduanzaoItem::menuItemClicked_DuanZao));
		pMenu->addChild(shici, 0, Tag_MenuItem_Shici);
		shici->setPosition(ccp(this->getContentSize().width/2+yici->getContentSize().width/2, yici->getPositionY()));
		shici->setUserData((void*)info->getActorType());

		char buf[20];
		//锻造一次需要金币
		BaseSprite *goldIcon1 = BaseSprite::create("jinbi_tubiao.png");
		this->addChild(goldIcon1);
		goldIcon1->setScale(0.5f);
		goldIcon1->setPosition(ccp(yici->getPositionX()-goldIcon1->getContentSize().width/2, yici->getPositionY()+yici->getContentSize().height/2+goldIcon1->getContentSize().height/2));
		sprintf(buf, "%d", 400);
		CCLabelTTF *yiciLabel = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
		this->addChild(yiciLabel);
		yiciLabel->setPosition(ccp(goldIcon1->getPositionX()+goldIcon1->getContentSize().width/2*goldIcon1->getScaleX()+yiciLabel->getContentSize().width/2
			, goldIcon1->getPositionY()));
		//锻造十次需要金币
		BaseSprite *goldIcon10 = BaseSprite::create("jinbi_tubiao.png");
		this->addChild(goldIcon10);
		goldIcon10->setScale(0.5f);
		goldIcon10->setPosition(ccp(shici->getPositionX()-goldIcon10->getContentSize().width/2, shici->getPositionY()+shici->getContentSize().height/2+goldIcon10->getContentSize().height/2));
		sprintf(buf, "%d", 400*10);
		CCLabelTTF *yiciLabel0 = CCLabelTTF::create(buf, fontStr_BookAntiqua, 20);
		this->addChild(yiciLabel0);
		yiciLabel0->setPosition(ccp(goldIcon10->getPositionX()+goldIcon10->getContentSize().width/2*goldIcon10->getScaleX()+yiciLabel0->getContentSize().width/2
			, goldIcon10->getPositionY()));

		CCMenuItemImage* pXiaoguo = CCMenuItemImage::create(
			"shandian_anniu_taozhuangxiaoguo.png",
			"shandian_anniu_taozhuangxiaoguo_select.png",
			this, menu_selector(ChuanqiduanzaoItem::menuItemClicked_Xiaoguo)
			);
		pMenu->addChild(pXiaoguo);
		pXiaoguo->setPosition(ccp(this->getContentSize().width/2, pXiaoguo->getContentSize().height/2));
	}

	return bret;
}


void ChuanqiduanzaoItem::menuItemClicked_Xiaoguo(CCObject* pObj)
{

	CCMenuItemImage* pItem = (CCMenuItemImage*)pObj;

	TaoZhuangInfoUI *layer = TaoZhuangInfoUI::create();
	layer->ignoreAnchorPointForPosition(false);
	layer->setAnchorPoint(CCPointZero);
	layer->setPosition(CCPointZero);
	layer->setProfessionId(m_Profession_id);
	//layer->setFriendName(getNewFirendName(8));

	CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);

}

void ChuanqiduanzaoItem::menuItemClicked_DuanZao(CCObject* pObj)
{
	int tag = ((CCMenuItem*)pObj)->getTag();
	bool  bShici = false;
	if (tag == Tag_MenuItem_Yici)  bShici = false;
	else if(tag == Tag_MenuItem_Shici) bShici = true;
	else return;

	int profId = (int)((CCMenuItem*)pObj)->getUserData();

	CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(ChuanqiduanzaoItem::rsp_DuanZao), MSG_DuanzaoRsp_F, NULL);
	CmdHelper::getHelper()->cmdDuanzao(bShici, profId);
}

void ChuanqiduanzaoItem::rsp_DuanZao(CCObject *msg_f)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, MSG_DuanzaoRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_DuanzaoRsp *data = (Flat_DuanzaoRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if (data->err_code != Success)
	{
		if (data->err_code == CEC_NoMoreFreeSpaceToStoreEquip)
		{
			EquipBagNotEnoughDialog *layer = new EquipBagNotEnoughDialog;
			layer->showGoToFenJieDialog();
		}
		else
		{
			MessageDisplay *layer = MessageDisplay::create(CmdHelper::getHelper()->getMessage(data->err_code));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		}
		return;
	}

	//刷新头部金币
	{
		int gold = 0;
		WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
		if (wp != NULL)
		{
			gold = wp->getCount();
		}

		BaseSprite* header = MainLayerBase::getCurrentMainBase()->getHeader()->getHeaderContent();

		CCLabelTTF* lable = (CCLabelTTF*)header->getChildByTag(STORE_FRAM_TAG_HEAD_JINBI);
		char str[64];
		sprintf(str, "%d", gold);
		lable->setString(str);
	}

	ShenBingShangDianDuanZao *layer = new ShenBingShangDianDuanZao(data->equips);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1000);
	layer->release();
}

string ChuanqiduanzaoItem::getHeroTypeFileNameByType(int iType)
{
	string retStr = "shangdian_chuanqi_archer.png";
	switch (iType)
	{
	case Profession_Yemanren:
		{
			retStr = "shangdian_chuanqi_barbarian.png";
		}
		break;
	case Profession_Chike:
		{
			retStr = "shangdian_chuanqi_hunter.png";
		}
		break;
	case Profession_Fashi:
		{
			retStr = "shangdian_chuanqi_wizard.png";
		}
		break;
	case Profession_Qishi:
		{
			retStr = "shangdian_chuanqi_warrior.png";
		}
		break;
	case Profession_ShenJianShou:
		{
			retStr = "shangdian_chuanqi_archer.png";
		}
		break;
	case Profession_MiShu:
		{
			retStr = "shangdian_chuanqi_witcher.png";
		}
		break;
	case Profession_WuSeng:
		{
			retStr = "shangdian_chuanqi_magebane.png";
		}
		break;
	case Profession_YouXia:
		{
			retStr = "shangdian_chuanqi_ranger.png";
		}
		break;
	case Profession_LieShou:
		{
			retStr = "shangdian_chuanqi_hunteman.png";
		}
		break;
	case Profession_WuShi:
		{
			retStr = "shangdian_chuanqi_sorcerer.png";
		}
		break;
	default:
		break;
	}

	return retStr;
}

string ChuanqiduanzaoItem::getIconFileNameByType(int iType)
{
	string retStr = "touxiang_archer1.png";
	switch (iType)
	{
	case Profession_Yemanren:
		{
			retStr = "touxiang_barbarian1.png";
		}
		break;
	case Profession_Chike:
		{
			retStr = "touxiang_hunter1.png";
		}
		break;
	case Profession_Fashi:
		{
			retStr = "touxiang_wizard1.png";
		}
		break;
	case Profession_Qishi:
		{
			retStr = "touxiang_warrior1.png";
		}
		break;
	case Profession_ShenJianShou:
		{
			retStr = "touxiang_archer1.png";
		}
		break;
	case Profession_MiShu:
		{
			retStr = "touxiang_witcher1.png";
		}
		break;
	case Profession_WuSeng:
		{
			retStr = "touxiang_magebane1.png";
		}
		break;
	case Profession_YouXia:
		{
			retStr = "touxiang_ranger1.png";
		}
		break;
	case Profession_LieShou:
		{
			retStr = "touxiang_huntsman1.png";
		}
		break;
	case Profession_WuShi:
		{
			retStr = "touxiang_sorcerer1.png";
		}
		break;
	default:
		break;
	}

	return retStr;
}