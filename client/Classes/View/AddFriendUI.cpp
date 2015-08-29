#include "AddFriendUI.h"
#include "BaseActorPropertyParser.h"
#include "ImageManager.h"
#include "SkillTree.h"
#include "SkillManager.h"
#include "CS_Common.h"
#include "MyselfManager.h"
#include "TaoZhuangInfoUI.h"
AddFriendUI::AddFriendUI()
	:CCLayer()
	,m_sFriendIndex(0)
	,m_oTarger(NULL)
	,m_pProfessionId(Profession_Yemanren)
	,m_bAddTeamItem(true)
	,m_bCloseItem(false)
{
}
AddFriendUI::~AddFriendUI()
{
}
bool AddFriendUI::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
	return true;
}
void AddFriendUI::onEnter()
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
	Widget* myLayout = dynamic_cast<Widget*>(GUIReader::shareReader()->widgetFromJsonFile("NewFriendAddUI_1.ExportJson")); //alpha1中使用  
	uiLayer->addWidget(myLayout);  
	this->addChild(uiLayer);
	//CCSize size = CCDirector::sharedDirector()->getWinSize();
	myLayout->ignoreAnchorPointForPosition(false);
	myLayout->setAnchorPoint(ccp(0.5,0.5));
	myLayout->setPosition(ccp(size.width / 2,size.height / 2));

	CCSprite *headImg = CCSprite::create(ImageManager::getManager()->getImageData_ProfessionHead(getZhuShouId(m_sFriendIndex)).getCurrentImageFile().c_str());

	if (headImg)
	{
		CCSprite *bianKuangImg = CCSprite::create("renxing_renwu_touxiang_select.png");
		bianKuangImg->setPosition(ccp(headImg->getContentSize().width / 2, headImg->getContentSize().height / 2));
		headImg->addChild(bianKuangImg);
	}
	Widget* bar = myLayout->getChildByName("ImageBar");

	if (bar)
	{
		//bar->get
		headImg->setPosition(bar->getPosition());
		CCSize loyoutSize = bar->getLayoutSize();
		myLayout->addNode(headImg);
	}

	{
		Button *taoZhuang = dynamic_cast<Button*>(myLayout->getChildByName("TaoZhuangBtn"));
		if (taoZhuang)
		{
			taoZhuang->addTouchEventListener(this, toucheventselector(AddFriendUI::clickTaoZhuang));
			m_lButtonList.push_back(taoZhuang);
		}
		
		//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(taoZhuang,-2014,true);
		//taoZhuang->set
	}
	{
		if (m_bAddTeamItem)
		{
			Button *addTeam = dynamic_cast<Button*>(myLayout->getChildByName("AddTeamBtn"));
			if (addTeam)
			{
				addTeam->addTouchEventListener(this, toucheventselector(AddFriendUI::clickOk));
				m_lButtonList.push_back(addTeam);
			}
		}
		else
		{
			Button *addTeam = dynamic_cast<Button*>(myLayout->getChildByName("AddTeamBtn"));
			if (addTeam)
			{
				addTeam->setVisible(false);
				addTeam->setEnabled(false);
			}
		}
		if(m_bCloseItem)
		{
			Button *addTeam = dynamic_cast<Button*>(myLayout->getChildByName("CloseItem"));
			if (addTeam)
			{
				addTeam->addTouchEventListener(this, toucheventselector(AddFriendUI::clickOk));
				m_lButtonList.push_back(addTeam);
			}
		}
		else
		{
			Button *addTeam = dynamic_cast<Button*>(myLayout->getChildByName("CloseItem"));
			if (addTeam)
			{
				addTeam->setVisible(false);
				addTeam->setEnabled(false);
			}
		}
		
	}
	{
		Label *titleName = dynamic_cast<Label*>(myLayout->getChildByName("TitleName"));
		if (titleName)
		{
			titleName->setFontName(fontStr_kaiti);
			titleName->setText(getNpcName(getZhuShouId(m_sFriendIndex)).c_str());
		}
	}
	{
		Label *npcInfo = dynamic_cast<Label*>(myLayout->getChildByName("Info"));
		if (npcInfo)
		{
			npcInfo->setFontName(fontStr_kaiti);
			npcInfo->setText(getNpcInfo(getZhuShouId(m_sFriendIndex)).c_str());
		}
		
	}

	SkillTree *tree = SkillTree::sharedSkillTree();
	BaseActor *actor = getZhuShouActor(getZhuShouId(m_sFriendIndex));
	if (!actor)
	{
		return;
	}
	Widget *jiNengImg = dynamic_cast<Widget*>(myLayout->getChildByName("JiNengImg"));
	{
		Label *jiNengName = dynamic_cast<Label*>(jiNengImg->getChildByName("JiNengName"));
		if (jiNengName)
		{
			
			
			CCString jinengStr;
			SkillInfo *info = actor->getCurrentUseSkill()->getInfor();
			if (info->skill_level >= 2)
			jinengStr.initWithFormat("%s+%d", tree->getSkillNameByType((SkillType)info->skill_type).c_str(), info->skill_level - 1);
			else 
			jinengStr = tree->getSkillNameByType((SkillType)info->skill_type);

			jiNengName->setText(jinengStr.getCString());
			jiNengName->setFontName(fontStr_kaiti);
		}
		
	}
	{
		int chuFaDian = actor->getCurrentSkillTurn();
		int chuFaZhouQi = actor->getCurrentSkillZhouQi();

		Widget *jiNengName = dynamic_cast<Widget*>(jiNengImg->getChildByName("ChuFaLunCi"));
		for (int i=0; i!=chuFaZhouQi; ++i)
		{
			CCSprite *img = NULL;
			if(i == chuFaDian-1)
				img = BaseSprite::create("jineng_chufalunci_icon_chufa.png");
			else 
				img = BaseSprite::create("jineng_chufalunci_icon.png");
			jiNengImg->addNode(img,1);
			CCPoint pos = jiNengName->getPosition();
			img->setPosition(ccp(pos.x + jiNengName->getCustomSize().width / 2 + 30 + i*img->getContentSize().width,pos.y));
		}
	}
	{
		SkillInfo *info = actor->getCurrentUseSkill()->getInfor();
		CCSprite *pic = CCSprite::create(ImageManager::getManager()->getSkillPicFileName((SkillType)info->skill_type).c_str());

		UIImageView *jiNengIcon = dynamic_cast<UIImageView*>(myLayout->getChildByName("JiNengIcon"));
		jiNengIcon->setEnabled(false);
		jiNengIcon->setVisible(false);


		//jiNengIcon->loadTexture(ImageManager::getManager()->getSkillPicFileName((SkillType)info->skill_type).c_str());
		pic->setPosition(jiNengIcon->getPosition());
		pic->setScale(0.6f);
		myLayout->addNode(pic);
	}
	{
		Label *jiNengInfo = dynamic_cast<Label*>(myLayout->getChildByName("JiNengInfo"));
		if (jiNengInfo)
		{


			CCString jinengStr;
			SkillInfo *info = actor->getCurrentUseSkill()->getInfor();
			string infoStr = tree->getSkillDescribeByType((SkillType)info->skill_type);

			jiNengInfo->setText(infoStr.c_str());
			jiNengInfo->setFontName(fontStr_kaiti);
			jiNengInfo->setFontSize(20.0f);
		}
	}

}

void AddFriendUI::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}
bool AddFriendUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	for (vector<Button *>::iterator iter = m_lButtonList.begin(); iter != m_lButtonList.end(); iter++)
	{
		(*iter)->onTouchBegan(pTouch,pEvent);
	}
	return true;
}
void AddFriendUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	for (vector<Button *>::iterator iter = m_lButtonList.begin(); iter != m_lButtonList.end(); iter++)
	{
		(*iter)->onTouchMoved(pTouch,pEvent);
	}
}
void AddFriendUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	for (vector<Button *>::iterator iter = m_lButtonList.begin(); iter != m_lButtonList.end(); iter++)
	{
		(*iter)->onTouchEnded(pTouch,pEvent);
	}
}
void AddFriendUI::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
	for (vector<Button *>::iterator iter = m_lButtonList.begin(); iter != m_lButtonList.end(); iter++)
	{
		(*iter)->onTouchCancelled(pTouch,pEvent);
	}
}
Profession AddFriendUI::getZhuShouId(int index)
{
	if (index == 0)
	{
		return m_pProfessionId;
	}
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
string AddFriendUI::getNpcName(Profession id)
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
void AddFriendUI::clickTaoZhuang(CCObject *obj,TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		TaoZhuangInfoUI *layer = TaoZhuangInfoUI::create();
		layer->ignoreAnchorPointForPosition(false);
		layer->setAnchorPoint(CCPointZero);
		layer->setPosition(CCPointZero);
		layer->setProfessionId(getZhuShouId(m_sFriendIndex));
		//layer->setProfessionId(Profession_Chike);

		CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);
	}
	
}
void AddFriendUI::clickOk(CCObject *obj,TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		if (m_oTarger)
		{
			CCNode *node = CCNode::create();
			node->setTag(getZhuShouId(m_sFriendIndex));
			(m_oTarger->*m_nHandler)(node);
		}
		this->removeFromParent();
	}
	
}

string AddFriendUI::getNpcInfo(Profession id)
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
BaseActor *AddFriendUI::getZhuShouActor(Profession id)
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