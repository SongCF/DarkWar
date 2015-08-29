#include "TaoZhuangInfoUI.h"
#include "BaseActorPropertyParser.h"
#include "ImageManager.h"
#include "SkillTree.h"
#include "SkillManager.h"
#include "CS_Common.h"
#include "ShenBingPool.h"
#include "SuitPool.h"
#include "MyselfManager.h"
#include "FX_CommonFunc.h"
using namespace FX_CommonFunc;
TaoZhuangInfoUI::TaoZhuangInfoUI()
	:CCLayer()
	,m_sProfessionid(Profession_LieShou)
{
}
TaoZhuangInfoUI::~TaoZhuangInfoUI()
{
}
bool TaoZhuangInfoUI::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
	return true;
}
void TaoZhuangInfoUI::onEnter()
{
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-2014,true);
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	//m_lNodeList.push_back(spr);
	ccColor4B color = {0,0,0,100};
	CCLayerColor *colorLayer1 = CCLayerColor::create(color);
	colorLayer1->setPosition(CCPointZero);
	colorLayer1->setContentSize(size);
	addChild(colorLayer1);

	UILayer* uiLayer = UILayer::create();  
	///auto myLayout = dynamic_cast<Layout*>(CCUIHELPER->createWidgetFromJsonFile("testUI.ExportJson")); //alpha0中使用  
	Widget* myLayout = GUIReader::shareReader()->widgetFromJsonFile("NewFriendAddUI_2.ExportJson"); //alpha1中使用  
	uiLayer->addWidget(myLayout);  
	this->addChild(uiLayer);
	//CCSize size = CCDirector::sharedDirector()->getWinSize();
	myLayout->ignoreAnchorPointForPosition(false);
	myLayout->setAnchorPoint(ccp(0.5,0.5));
	myLayout->setPosition(ccp(size.width / 2,size.height / 2));

	//BaseSprite* getFullPhotoOfActor()
	unsigned int curGroupId = getZhuangbeiGroupID(m_sProfessionid);
	OneSuitInfo suitInfo;
	if(SuitPool::getPool()->getSuitInforByZBGroupId(curGroupId, suitInfo))
	{
		//ImageManager::getManager()->
		Label *name = dynamic_cast<Label*>(myLayout->getChildByName("Name"));
		if (name)
		{
			name->setText(suitInfo.suit_name.c_str());
		}
		int index = 0;
		for (vector<SuitPart>::iterator iter = suitInfo.parts_of_suit.begin() ; iter != suitInfo.parts_of_suit.end() ; iter++)
		{
			index ++;
			Label *name1 = dynamic_cast<Label*>(myLayout->getChildByName(CCString::createWithFormat("Name%d",index)->getCString()));

			string nameTitle = iter->zhuangbei_name.c_str();

			if (iter->zhuanbei_type == ZhuangbeiType_DanshouJian || iter->zhuanbei_type == ZhuangbeiType_ShuangshouBishou || iter->zhuanbei_type == ZhuangbeiType_ShuangshouFu || iter->zhuanbei_type == ZhuangbeiType_DanshouFazhang || iter->zhuanbei_type == ZhuangbeiType_Gong || iter->zhuanbei_type == ZhuangbeiType_ShuangshouFazhang)
			{
				nameTitle += LFStrings::getValue("ZhuangBeiWuQi");
			}
			else if (iter->zhuanbei_type == ZhuangbeiType_Faqi || iter->zhuanbei_type == ZhuangbeiType_Dunpai)
			{
				nameTitle += LFStrings::getValue("ZhuangBeiFuShou");
			}
			else if (iter->zhuanbei_type == ZhuangbeiType_Hushenfu)
			{
				nameTitle += LFStrings::getValue("ZhuangBeiHuShenFu");
			}
			else if (iter->zhuanbei_type == ZhuangbeiType_KuijiaZhong || iter->zhuanbei_type == ZhuangbeiType_KuijiaQin)
			{
				nameTitle += LFStrings::getValue("ZhuangBeiXiongJia");
			}
			else
			{
				nameTitle += LFStrings::getValue("ZhuangBeiJieZhi");
			}
			name1->setText(nameTitle.c_str());
			name1->setFontName(fontStr_kaiti);
		}
		for (int i = index + 1; i <= 6; i++)
		{
			Label *name1 = dynamic_cast<Label*>(myLayout->getChildByName(CCString::createWithFormat("Name%d",i)->getCString()));
			name1->setVisible(false);
		}
		
		for (int i = 0 ; i < 5 ; i ++)
		{
			Label *name2 = dynamic_cast<Label*>(myLayout->getChildByName(CCString::createWithFormat("Label%d",i + 1)->getCString()));
			name2->setFontName(fontStr_kaiti);
			Label *name1 = dynamic_cast<Label*>(myLayout->getChildByName(CCString::createWithFormat("ZB%dNumber",i + 1)->getCString()));
			if (suitInfo.suit_attr[i][0].attr_type == 0)
			{
				name2->setVisible(false);
				name1->setVisible(false);
				continue;
			}
			name1->setText(GetMoFaShuXing_ByZBAttr(suitInfo.suit_attr[i][0]));
			name1->setFontName(fontStr_kaiti);

			
			//string nameTitle = iter->zhuangbei_name.c_str();
		}
	}
	{
		Label *miaoShuText = dynamic_cast<Label*>(myLayout->getChildByName("Label_50"));
		if (miaoShuText)
		{
			miaoShuText->setFontName(fontStr_kaiti);
			miaoShuText->setText(getNpcMiaoShu(m_sProfessionid).c_str());
		}
	}

	CCNode *node = ImageManager::getManager()->getFullPhotoOfAllShenBing(m_sProfessionid);
	{
		UIImageView *taoZhuang = dynamic_cast<UIImageView*>(myLayout->getChildByName("Image_49"));
		if (taoZhuang)
		{
			myLayout->addNode(node,2);
			node->ignoreAnchorPointForPosition(false);
			node->setAnchorPoint(taoZhuang->getAnchorPoint());
			node->setPosition(ccp(taoZhuang->getPositionX(),taoZhuang->getPositionY() + node->getContentSize().height / 2 + 100));
			taoZhuang->setVisible(false);
			taoZhuang->setEnabled(false);
		}
	}
	{
		Button *closeItem = dynamic_cast<Button*>(myLayout->getChildByName("CloseBtn"));
		if (closeItem)
		{
			//m_iCloseItem = closeItem;
			m_lButtonList.push_back(closeItem);
			closeItem->addTouchEventListener(this, toucheventselector(TaoZhuangInfoUI::clickOk));
		}
	}
}
unsigned int TaoZhuangInfoUI::getZhuangbeiGroupID(Profession id)
{
	ChuanqiPool *pool = ChuanqiPool::getPool();
	vector<ChuanQi> list = pool->getAllChuanqi();
	
	for (vector<ChuanQi>::iterator iter = list.begin() ; iter != list.end() ; iter++)
	{
		if (iter->allChuanqiFenShenInLifeSpan.at(0).profession_id == id){
			return iter->allChuanqiFenShenInLifeSpan.at(0).zb_group_id;
		}
	}
	return 0;
}
void TaoZhuangInfoUI::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}
bool TaoZhuangInfoUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	for (vector<Button *>::iterator iter = m_lButtonList.begin(); iter != m_lButtonList.end(); iter++)
	{
		(*iter)->onTouchBegan(pTouch,pEvent);
	}
	return true;
}
void TaoZhuangInfoUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	for (vector<Button *>::iterator iter = m_lButtonList.begin(); iter != m_lButtonList.end(); iter++)
	{
		(*iter)->onTouchMoved(pTouch,pEvent);
	}
}
void TaoZhuangInfoUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	for (vector<Button *>::iterator iter = m_lButtonList.begin(); iter != m_lButtonList.end(); iter++)
	{
		(*iter)->onTouchEnded(pTouch,pEvent);
	}
}
void TaoZhuangInfoUI::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
	for (vector<Button *>::iterator iter = m_lButtonList.begin(); iter != m_lButtonList.end(); iter++)
	{
		(*iter)->onTouchCancelled(pTouch,pEvent);
	}
}
Profession TaoZhuangInfoUI::getZhuShouId(int index)
{
	Profession id = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;

	switch (id)
	{
	case Profession_Yemanren:
		{
			if (index == 1)
			{
				return Profession_MiShu;
			}
			else if (index == 2)
			{
				return Profession_YouXia;
			}
			else if (index == 3)
			{
				return Profession_LieShou;
			}
		}
		break;
	case Profession_Chike:
		{
			if (index == 1)
			{
				return Profession_LieShou;
			}
			else if (index == 2)
			{
				return Profession_MiShu;
			}
			else if (index == 3)
			{
				return Profession_Qishi;
			}
		}
		break;
	case Profession_Fashi:
		{
			if (index == 1)
			{
				return Profession_YouXia;
			}
			else if (index == 2)
			{
				return Profession_Qishi;
			}
			else if (index == 3)
			{
				return Profession_LieShou;
			}
		}
		break;
	default:
		break;
	}
	return Profession_Yemanren;
}
string TaoZhuangInfoUI::getNpcName(Profession id)
{
	string actorName;

	switch (id)
	{
	case Profession_Yemanren:
		break;
	case Profession_Chike:
		break;
	case Profession_Fashi:
		break;
	case Profession_Qishi:
		{
			actorName = LFStrings::getValue("qiShi");
		}
		break;
	case Profession_ShenJianShou:
		{
			actorName = LFStrings::getValue("shenjianshou");
		}
		break;
	case Profession_MiShu:
		{
			actorName = LFStrings::getValue("mishu");
		}
		break;
	case Profession_WuSeng:
		{
			actorName = LFStrings::getValue("wuseng");
		}
		break;
	case Profession_YouXia:
		{
			actorName = LFStrings::getValue("youxia");
		}
		break;
	case Profession_LieShou:
		{
			actorName = LFStrings::getValue("lieshou");
		}
		break;
	case Profession_WuShi:
		{
			actorName = LFStrings::getValue("wushi");
		}
		break;
	case Profession_GW_Start:
		break;
	case Profession_JZ_JiangShi:
		break;
	case Profession_JZ_ShiRenMo:
		break;
	case Profession_JZ_YeRen:
		break;
	case Profession_JZ_KuLouBin:
		break;
	case Profession_JZ_KuLouZhanShi:
		break;
	case Profession_JZ_ShuGuai:
		break;
	case Profession_YC_KuLouGongJianShou:
		break;
	case Profession_YC_ShuYao:
		break;
	case Profession_FS_KuLouWuShi:
		break;
	case Profession_FS_YeRenSaMan:
		break;
	case Profession_GW_End:
		break;
	case Profession_Big_Boss_1:
		break;
	default:
		break;
	}
	return actorName;
}
void TaoZhuangInfoUI::clickTaoZhuang(CCObject *obj,TouchEventType type)
{

}
void TaoZhuangInfoUI::clickOk(CCObject *obj,TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		this->removeFromParent();
	}
}

string TaoZhuangInfoUI::getNpcInfo(Profession id)
{
	string info;

	switch (id)
	{
	case Profession_Yemanren:
		break;
	case Profession_Chike:
		break;
	case Profession_Fashi:
		break;
	case Profession_Qishi:
		{
			info = LFStrings::getValue("renwuJieShao_QiShi");
		}
		break;
	case Profession_ShenJianShou:
		{
			info = LFStrings::getValue("renwuJieShao_ShenJianShou");
		}
		break;
	case Profession_MiShu:
		{
			info = LFStrings::getValue("renwuJieShao_MiShuShi");
		}
		break;
	case Profession_WuSeng:
		{
			info = LFStrings::getValue("renwuJieShao_WuSeng");
		}
		break;
	case Profession_YouXia:
		{
			info = LFStrings::getValue("renwuJieShao_YouXia");
		}
		break;
	case Profession_LieShou:
		{
			info = LFStrings::getValue("renwuJieShao_LieShou");
		}
		break;
	case Profession_WuShi:
		{
			info = LFStrings::getValue("renwuJieShao_WuShi");
		}
		break;
	case Profession_GW_Start:
		break;
	case Profession_JZ_JiangShi:
		break;
	case Profession_JZ_ShiRenMo:
		break;
	case Profession_JZ_YeRen:
		break;
	case Profession_JZ_KuLouBin:
		break;
	case Profession_JZ_KuLouZhanShi:
		break;
	case Profession_JZ_ShuGuai:
		break;
	case Profession_YC_KuLouGongJianShou:
		break;
	case Profession_YC_ShuYao:
		break;
	case Profession_FS_KuLouWuShi:
		break;
	case Profession_FS_YeRenSaMan:
		break;
	case Profession_GW_End:
		break;
	case Profession_Big_Boss_1:
		break;
	default:
		break;
	}
	return info;
}
BaseActor *TaoZhuangInfoUI::getZhuShouActor(Profession id)
{
	vector<BaseActor *> list = MyselfManager::getManager()->getTeamActors_ZhenShen();
	for (vector<BaseActor *>::iterator iter = list.begin(); iter != list.end() ;iter++)
	{
		if ((*iter)->getCurrentProperty().profession == id)
		{
			return (*iter);
		}
	}
	return NULL;
}
string TaoZhuangInfoUI::getNpcMiaoShu(Profession id)
{
	string info;

	switch (id)
	{
	case Profession_Yemanren:
		{
			info = LFStrings::getValue("YeManRenMiaoShu");
		}
		break;
	case Profession_Chike:
		{
			info = LFStrings::getValue("CiKeMiaoShu");
		}
		break;
	case Profession_Fashi:
		{
			info = LFStrings::getValue("FaShiMiaoShu");
		}
		break;
	case Profession_Qishi:
		{
			info = LFStrings::getValue("QiShiMiaoShu");
		}
		break;
	case Profession_ShenJianShou:
		{
			info = LFStrings::getValue("ShenJianMiaoShu");
		}
		break;
	case Profession_MiShu:
		{
			info = LFStrings::getValue("MiShuMiaoShu");
		}
		break;
	case Profession_WuSeng:
		{
			info = LFStrings::getValue("WuShengMiaoShu");
		}
		break;
	case Profession_YouXia:
		{
			info = LFStrings::getValue("YouXiaMiaoShu");
		}
		break;
	case Profession_LieShou:
		{
			info = LFStrings::getValue("LieRenMiaoShu");
		}
		break;
	case Profession_WuShi:
		{
			info = LFStrings::getValue("WuShiMiaoShu");
		}
		break;
	case Profession_GW_Start:
		break;
	case Profession_JZ_JiangShi:
		break;
	case Profession_JZ_ShiRenMo:
		break;
	case Profession_JZ_YeRen:
		break;
	case Profession_JZ_KuLouBin:
		break;
	case Profession_JZ_KuLouZhanShi:
		break;
	case Profession_JZ_ShuGuai:
		break;
	case Profession_YC_KuLouGongJianShou:
		break;
	case Profession_YC_ShuYao:
		break;
	case Profession_FS_KuLouWuShi:
		break;
	case Profession_FS_YeRenSaMan:
		break;
	case Profession_GW_End:
		break;
	case Profession_Big_Boss_1:
		break;
	default:
		break;
	}
	return info;
}