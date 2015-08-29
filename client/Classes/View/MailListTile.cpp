#include "MailListTile.h"
#include "MailTanChuLayer.h"
#include "EmailManager.h"
#include "font.h"
#include "CmdHelper.h"
#include "GameDirector.h"
MailListTile::MailListTile()
	:CCLayer(),
	m_nCount(0),
	m_nSide(0),
	m_nHeight(0),
	m_mMenu(NULL),
	m_lMailTanChuLayer(NULL)
{
}

MailListTile::~MailListTile() {
}
bool MailListTile::init() {
	if (!CCLayer::init()) {
		return false;
	}

	return true;
}

void MailListTile::onEnter() {
	CCLayer::onEnter();

	m_mMenu = ListMenu::create();
	m_mMenu->setAnchorPoint(CCPointZero);
	m_mMenu->setPosition(CCPointZero);
	addChild(m_mMenu);
	m_cTTFColor.r = 195;
	m_cTTFColor.g = 145;
	m_cTTFColor.b = 95;

	m_nSide = 10;
}
void MailListTile::onExit() {
	CCLayer::onExit();
}
void MailListTile::setSystemMail(Email mail)
{
	m_sSystemMail = mail;
	string biaotiImg[5] = {"denglujiangli_langtiao.png","denglujiangli_langtiao.png","denglujiangli_hongtiao.png","denglujiangli_hangtiao.png","denglujiangli_zitiao.png"};

	string titleStr[5] = {LFStrings::getValue("XiTongJiangli"),LFStrings::getValue("XiTongBuChang"),LFStrings::getValue("XiTongTongZhi"),LFStrings::getValue("HaoYouShenQing"),LFStrings::getValue("HaoYouShanChu")};
	CCSprite *bgSpr = CCSprite::create("denglujiangli_jinri_bg.png");
	bgSpr->setAnchorPoint(CCPointZero);
	CCPoint pos;
	pos.x = 5;
	pos.y = 0;
	bgSpr->setPosition(pos);
	addChild(bgSpr,-1);
	m_sBgSpr = bgSpr;
	int side = 10;
	this->setContentSize(bgSpr->getContentSize());
	{//标题
		CCSprite *titleSpr = CCSprite::create(biaotiImg[mail.type].c_str());
		titleSpr->setPosition(ccp(bgSpr->getContentSize().width / 2,bgSpr->getContentSize().height - titleSpr->getContentSize().height / 2));
		bgSpr->addChild(titleSpr);
		
		CCLabelTTF *titleTTF = CCLabelTTF::create(titleStr[mail.type].c_str(),fontStr_kaiti,20.0f);
		titleTTF->setPosition(titleSpr->getPosition());
		bgSpr->addChild(titleTTF);
		titleTTF->setColor(ccWHITE);
		string info;
		if (mail.summary.length() > 1)
		{
			info = mail.summary;
		}
		else
		{
			info = mail.content;
		}
		//内容
		switch (mail.type)
		{
		case 0:
		case 1:
			{
				
				
				CCLabelTTF *ttf = CCLabelTTF::create(info.c_str(),fontStr_kaiti,20.0f);
				ttf->setAnchorPoint(ccp(0,1.0f));
				ttf->setPosition(ccp(ttf->getContentSize().height ,bgSpr->getContentSize().height - titleSpr->getContentSize().height - side));
				ttf->setDimensions(CCSizeMake(bgSpr->getContentSize().width - 2 * ttf->getContentSize().height,bgSpr->getContentSize().height - titleSpr->getContentSize().height));
				ttf->setHorizontalAlignment(kCCTextAlignmentLeft);
				bgSpr->addChild(ttf);
				ttf->setColor(m_cTTFColor);
				if (mail.is_read)
				{
					CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_shanchu.png","youxiang_anniu_shanchu_select.png","youxiang_anniu_shanchu_select.png",this,SEL_MenuHandler(&MailListTile::selectDelete));
					menuItem->setTag(mail.email_id);
					menuItem->setPosition(ccp(bgSpr->getPositionX() + bgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
					m_mMenu->addChild(menuItem);

					CCSprite *koSpr = CCSprite::create("jingjichang_wanjiazhuangtailan_ko.png");
					koSpr->setPosition(ccp(bgSpr->getContentSize().width - koSpr->getContentSize().width / 2 - menuItem->getContentSize().width, 20 + koSpr->getContentSize().height / 2));
					bgSpr->addChild(koSpr);
				}
				else
				{
					CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_xiangqing.png","youxiang_anniu_xiangqing_select.png","youxiang_anniu_xiangqing_select.png",this,SEL_MenuHandler(&MailListTile::selectCheckInfo));
					menuItem->setTag(mail.email_id);
					menuItem->setPosition(ccp(bgSpr->getPositionX() + bgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
					m_mMenu->addChild(menuItem);
				}
			}
			break;
		case 2:
			{
				CCLabelTTF *ttf = CCLabelTTF::create(info.c_str(),fontStr_kaiti,20.0f);
				ttf->setAnchorPoint(ccp(0,1.0f));
				ttf->setPosition(ccp(ttf->getContentSize().height ,bgSpr->getContentSize().height - titleSpr->getContentSize().height - side));
				ttf->setDimensions(CCSizeMake(bgSpr->getContentSize().width - 2 * ttf->getContentSize().height,bgSpr->getContentSize().height - titleSpr->getContentSize().height));
				ttf->setHorizontalAlignment(kCCTextAlignmentLeft);
				bgSpr->addChild(ttf);
				ttf->setColor(m_cTTFColor);
				if (mail.is_read)
				{
					CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_xiangqing.png","youxiang_anniu_xiangqing_select.png","youxiang_anniu_xiangqing_select.png",this,SEL_MenuHandler(&MailListTile::selectCheckInfo));
					menuItem->setTag(mail.email_id);
					menuItem->setPosition(ccp(bgSpr->getPositionX() + bgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide * 2 - menuItem->getContentSize().width,pos.y + m_nSide + menuItem->getContentSize().height / 2));
					m_mMenu->addChild(menuItem);


					CCMenuItemImage *menuItem2 = CCMenuItemImage::create("youxiang_anniu_shanchu.png","youxiang_anniu_shanchu_select.png","youxiang_anniu_shanchu_select.png",this,SEL_MenuHandler(&MailListTile::selectDelete));
					menuItem2->setTag(mail.email_id);
					menuItem2->setPosition(ccp(bgSpr->getPositionX() + bgSpr->getContentSize().width - menuItem2->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem2->getContentSize().height / 2));
					m_mMenu->addChild(menuItem2);
				}
				else
				{
					CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_xiangqing.png","youxiang_anniu_xiangqing_select.png","youxiang_anniu_xiangqing_select.png",this,SEL_MenuHandler(&MailListTile::selectCheckInfo));
					menuItem->setTag(mail.email_id);
					menuItem->setPosition(ccp(bgSpr->getPositionX() + bgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
					m_mMenu->addChild(menuItem);
				}
			}
			break;
		case 3:
			{
				CCLabelTTF *ttf = CCLabelTTF::create(CCString::createWithFormat("%s",mail.addresserName.c_str())->getCString(),fontStr_kaiti,20.0f);
				ttf->setAnchorPoint(ccp(0,1.0f));
				ttf->setPosition(ccp(ttf->getContentSize().height ,bgSpr->getContentSize().height - titleSpr->getContentSize().height - side));
				//ttf->setDimensions(CCSizeMake(bgSpr->getContentSize().width - 2 * ttf->getContentSize().height,bgSpr->getContentSize().height - titleSpr->getContentSize().height));
				ttf->setHorizontalAlignment(kCCTextAlignmentLeft);
				bgSpr->addChild(ttf);

				
				

				CCLabelTTF *ttf2 = CCLabelTTF::create(CCString::createWithFormat("%s",info.c_str())->getCString(),fontStr_kaiti,20.0f);
				ttf2->setAnchorPoint(ccp(0,1.0f));
				ttf2->setPosition(ccp(ttf->getPositionX() + ttf->getContentSize().width ,ttf->getPositionY()));
				ttf2->setDimensions(CCSizeMake(bgSpr->getContentSize().width - ttf->getPositionX() - ttf->getContentSize().width ,ttf->getPositionY()));
				ttf2->setHorizontalAlignment(kCCTextAlignmentLeft);
				bgSpr->addChild(ttf2);
				ttf2->setColor(m_cTTFColor);
				if (ttf->getContentSize().height == 0)
				{
					ttf2->setPosition(ccp(30.0f ,ttf->getPositionY()));
				}

				if (mail.is_read)
				{
					CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_shanchu.png","youxiang_anniu_shanchu_select.png","youxiang_anniu_shanchu_select.png",this,SEL_MenuHandler(&MailListTile::selectDelete));
					menuItem->setTag(mail.email_id);
					menuItem->setPosition(ccp(bgSpr->getPositionX() + bgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
					m_mMenu->addChild(menuItem);
				}
				else
				{
					CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_jieshou.png","youxiang_anniu_jieshou_select.png","youxiang_anniu_jieshou_select.png",this,SEL_MenuHandler(&MailListTile::selectOK));
					menuItem->setTag(mail.email_id);
					menuItem->setPosition(ccp(bgSpr->getPositionX() + bgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide * 2 - menuItem->getContentSize().width,pos.y + m_nSide + menuItem->getContentSize().height / 2));
					m_mMenu->addChild(menuItem);


					CCMenuItemImage *menuItem2 = CCMenuItemImage::create("youxiang_anniu_jujue.png","youxiang_anniu_jujue_select.png","youxiang_anniu_jujue_select.png",this,SEL_MenuHandler(&MailListTile::selectCancel));
					menuItem2->setTag(mail.email_id);
					menuItem2->setPosition(ccp(bgSpr->getPositionX() + bgSpr->getContentSize().width - menuItem2->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem2->getContentSize().height / 2));
					m_mMenu->addChild(menuItem2);
				}

			}
			break;
		case 4:
			{
				CCLabelTTF *ttf = CCLabelTTF::create(CCString::createWithFormat("%s",info.c_str())->getCString(),fontStr_kaiti,20.0f);
				ttf->setAnchorPoint(ccp(0,1.0f));
				ttf->setPosition(ccp(ttf->getContentSize().height ,bgSpr->getContentSize().height - titleSpr->getContentSize().height - side));
				ttf->setDimensions(CCSizeMake(bgSpr->getContentSize().width - 2 * ttf->getContentSize().height,bgSpr->getContentSize().height - titleSpr->getContentSize().height));
				ttf->setHorizontalAlignment(kCCTextAlignmentLeft);
				bgSpr->addChild(ttf);
				ttf->setColor(ccRED);
				CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_shanchu.png","youxiang_anniu_shanchu_select.png","youxiang_anniu_shanchu_select.png",this,SEL_MenuHandler(&MailListTile::selectDelete));
				menuItem->setTag(mail.email_id);
				menuItem->setPosition(ccp(bgSpr->getPositionX() + bgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
				m_mMenu->addChild(menuItem);
			}
			break;
		default:
			break;
		}
	}
}
void MailListTile::selectCheckInfo(CCObject *obj)
{
	//(m_oTarger->*m_hCheckInfo)((CCMenuItem *)obj);
	if (m_lMailTanChuLayer)
	{
		m_lMailTanChuLayer->removeFromParent();
	}
	m_lMailTanChuLayer = MailTanChuLayer::create();
	m_lMailTanChuLayer->ignoreAnchorPointForPosition(false);
	m_lMailTanChuLayer->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 2,CCDirector::sharedDirector()->getWinSize().height / 2) );
	m_lMailTanChuLayer->setAnchorPoint(ccp(0.5,0.5));
	m_lMailTanChuLayer->setMenuHandler(this,SEL_CallFunc(&MailListTile::setMenuTanChuCallBack));
	CCDirector::sharedDirector()->getRunningScene()->addChild(m_lMailTanChuLayer,1030);
	m_lMailTanChuLayer->setSystemMail(m_sSystemMail);
}
void MailListTile::selectDelete(CCObject* obj)
{
	addObserver(callfuncO_selector(MailListTile::clickDeleteOK), MSG_DeleteEmailRsp);
	CmdHelper::getHelper()->cmdDeleteEmail(m_sSystemMail.email_id);
	
}
void MailListTile::selectCancel(CCObject *obj)
{
	//addObserver(callfuncO_selector(MailListTile::clickDeleteOK), MSG_DeleteEmailRsp);
	//CmdHelper::getHelper()->cmdDeleteEmail(m_sSystemMail.email_id);
	(m_oTarger->*m_hClickCancel)((CCMenuItem *)obj);
}
void MailListTile::selectOK(CCObject* obj)
{
	(m_oTarger->*m_hClickOK)((CCMenuItem *)obj);
}
void MailListTile::checkInfoOK()
{
	//m_mMenu->removeAllChildrenWithCleanup(true);
	if (m_sSystemMail.is_read)
	{
		return;
	}
	
	CCArray* menuList = m_mMenu->getChildren();
	for (unsigned int i = 0; i < menuList->count(); ++i)
	{
		CCMenuItem* pObj=(CCMenuItem*)menuList->objectAtIndex(i);
		pObj->setEnabled(false);
		pObj->setVisible(false);
	}
	m_sSystemMail.is_read = true;
	refresh();
}
void MailListTile::clickDeleteOK(CCObject *obj)
{
	removeObserver(MSG_DeleteEmailRsp);
	GameDirector::getDirector()->hideWaiting();
	(m_oTarger->*m_hClickDelete)((CCMenuItem *)obj);
}
void MailListTile::clickOKOK(CCObject *obj)
{

}
void MailListTile::clickCancelOK(CCObject *obj)
{

}
void MailListTile::refresh()
{
	CCPoint pos;
	pos.x = 5;
	pos.y = 0;
	//内容
	switch (m_sSystemMail.type)
	{
	case 0:
	case 1:
		{
			if (m_sSystemMail.is_read)
			{
				CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_shanchu.png","youxiang_anniu_shanchu_select.png","youxiang_anniu_shanchu_select.png",this,SEL_MenuHandler(&MailListTile::selectDelete));
				menuItem->setTag(m_sSystemMail.email_id);
				menuItem->setPosition(ccp(m_sBgSpr->getPositionX() + m_sBgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
				m_mMenu->addChild(menuItem);

				CCSprite *koSpr = CCSprite::create("jingjichang_wanjiazhuangtailan_ko.png");
				koSpr->setPosition(ccp(m_sBgSpr->getContentSize().width - koSpr->getContentSize().width / 2 - menuItem->getContentSize().width, 20 + koSpr->getContentSize().height / 2));
				m_sBgSpr->addChild(koSpr);
			}
			else
			{
				CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_xiangqing.png","youxiang_anniu_xiangqing_select.png","youxiang_anniu_xiangqing_select.png",this,SEL_MenuHandler(&MailListTile::selectCheckInfo));
				menuItem->setTag(m_sSystemMail.email_id);
				menuItem->setPosition(ccp(m_sBgSpr->getPositionX() + m_sBgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
				m_mMenu->addChild(menuItem);
			}
		}
		break;
	case 2:
		{
			if (m_sSystemMail.is_read)
			{
				CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_xiangqing.png","youxiang_anniu_xiangqing_select.png","youxiang_anniu_xiangqing_select.png",this,SEL_MenuHandler(&MailListTile::selectCheckInfo));
				menuItem->setTag(m_sSystemMail.email_id);
				menuItem->setPosition(ccp(m_sBgSpr->getPositionX() + m_sBgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide * 2 - menuItem->getContentSize().width,pos.y + m_nSide + menuItem->getContentSize().height / 2));
				m_mMenu->addChild(menuItem);


				CCMenuItemImage *menuItem2 = CCMenuItemImage::create("youxiang_anniu_shanchu.png","youxiang_anniu_shanchu_select.png","youxiang_anniu_shanchu_select.png",this,SEL_MenuHandler(&MailListTile::selectDelete));
				menuItem2->setTag(m_sSystemMail.email_id);
				menuItem2->setPosition(ccp(m_sBgSpr->getPositionX() + m_sBgSpr->getContentSize().width - menuItem2->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem2->getContentSize().height / 2));
				m_mMenu->addChild(menuItem2);
			}
			else
			{
				CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_xiangqing.png","youxiang_anniu_xiangqing_select.png","youxiang_anniu_xiangqing_select.png",this,SEL_MenuHandler(&MailListTile::selectCheckInfo));
				menuItem->setTag(m_sSystemMail.email_id);
				menuItem->setPosition(ccp(m_sBgSpr->getPositionX() + m_sBgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
				m_mMenu->addChild(menuItem);
			}
		}
		break;
	case 3:
		{
			if (m_sSystemMail.is_read)
			{
				CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_shanchu.png","youxiang_anniu_shanchu_select.png","youxiang_anniu_shanchu_select.png",this,SEL_MenuHandler(&MailListTile::selectDelete));
				menuItem->setTag(m_sSystemMail.email_id);
				menuItem->setPosition(ccp(m_sBgSpr->getPositionX() + m_sBgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
				m_mMenu->addChild(menuItem);
			}
			else
			{
				CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_jieshou.png","youxiang_anniu_jieshou_select.png","youxiang_anniu_jieshou_select.png",this,SEL_MenuHandler(&MailListTile::selectOK));
				menuItem->setTag(m_sSystemMail.email_id);
				menuItem->setPosition(ccp(m_sBgSpr->getPositionX() + m_sBgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide * 2 - menuItem->getContentSize().width,pos.y + m_nSide + menuItem->getContentSize().height / 2));
				m_mMenu->addChild(menuItem);


				CCMenuItemImage *menuItem2 = CCMenuItemImage::create("youxiang_anniu_jujue.png","youxiang_anniu_jujue_select.png","youxiang_anniu_jujue_select.png",this,SEL_MenuHandler(&MailListTile::selectCancel));
				menuItem2->setTag(m_sSystemMail.email_id);
				menuItem2->setPosition(ccp(m_sBgSpr->getPositionX() + m_sBgSpr->getContentSize().width - menuItem2->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem2->getContentSize().height / 2));
				m_mMenu->addChild(menuItem2);
			}

		}
		break;
	case 4:
		{
			CCMenuItemImage *menuItem = CCMenuItemImage::create("youxiang_anniu_shanchu.png","youxiang_anniu_shanchu_select.png","youxiang_anniu_shanchu_select.png",this,SEL_MenuHandler(&MailListTile::selectDelete));
			menuItem->setTag(m_sSystemMail.email_id);
			menuItem->setPosition(ccp(m_sBgSpr->getPositionX() + m_sBgSpr->getContentSize().width - menuItem->getContentSize().width / 2 - m_nSide,pos.y + m_nSide + menuItem->getContentSize().height / 2));
			m_mMenu->addChild(menuItem);
		}
		break;
	default:
		break;
	}
}
void MailListTile::getMessage(CCObject *obj)
{
	removeObserver(MSG_DeleteEmailRsp);
	GameDirector::getDirector()->hideWaiting();
}
void MailListTile::addObserver(SEL_CallFuncO selector,const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,selector,name,NULL);
}
// 移除消息监听
void MailListTile::removeObserver(const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,name);
}