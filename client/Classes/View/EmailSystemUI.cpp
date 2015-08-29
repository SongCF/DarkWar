#include "EmailSystemUI.h"
#include "MenuPopUpLayer.h"
#include "MailTanChuLayer.h"
#include "EmailManager.h"
#include "GameDirector.h"
#include "UnblockManager.h"
EmailSystemUI::EmailSystemUI()
	:BaseLayer(),
	m_cScroll(NULL),
	m_mMenu(NULL),
	m_nHeight(0),
	m_cAuctionList(NULL)
{
}
EmailSystemUI::~EmailSystemUI()
{
}
CCScene* EmailSystemUI::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	EmailSystemUI *layer = EmailSystemUI::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
bool EmailSystemUI::init()
{
	if (!BaseLayer::init())
	{
		return false;
	}
	return true;
}
void EmailSystemUI::onEnter()
{
	BaseLayer::onEnter();

	CCMenu *menu = CCMenu::create();
	menu->setAnchorPoint(CCPointZero);
	menu->setPosition(CCPointZero);
	addChild(menu);

	CCSprite *bgSpr = CCSprite::create("renwuliebiao_down_bg.png");
	bgSpr->setPosition(ccp(getBodySize().width / 2,getBodySize().height / 2));
	addChild(bgSpr,-1);

	CCSprite *headSpr = CCSprite::create("denglujiangli_up_bg.png");
	headSpr->setPosition(ccp(bgSpr->getPositionX(),getBodySize().height - headSpr->getContentSize().height / 2));
	addChild(headSpr);

	bgSpr->setScaleX(getBodySize().width / bgSpr->getContentSize().width);
	bgSpr->setScaleY(getBodySize().height / bgSpr->getContentSize().height);

	CCSprite *titleSpr = CCSprite::create("youxiangziti.png");
	titleSpr->setPosition(headSpr->getPosition());
	addChild(titleSpr);

	int _height = getBodySize().height - headSpr->getContentSize().height;

	m_mMenu = menu;
	m_nHeight = _height;

	addObserver(SEL_CallFuncO(&EmailSystemUI::drawUI),MSG_QueryEmailRsp);
	CmdHelper::getHelper()->cmdQueryEmail();
}
void EmailSystemUI::selectCheckInfo(CCObject *obj)
{
	m_cAuctionList->checkInfoOK(((CCMenuItem *)obj)->getTag());
}
void EmailSystemUI::selectDelete(CCObject* obj)
{
	drawUI(obj);
	//addObserver(SEL_CallFuncO(&EmailSystemUI::drawUI),MSG_QueryEmailRsp);
	//CmdHelper::getHelper()->cmdQueryEmail();
	//m_cAuctionList->clickDeleteOK(((CCMenuItem *)obj)->getTag());
}
void EmailSystemUI::selectCancel(CCObject *obj)
{
	m_cAuctionList->clickCancelOK(((CCMenuItem *)obj)->getTag());
}
void EmailSystemUI::selectOK(CCObject* obj)
{
	m_cAuctionList->clickOKOK(((CCMenuItem *)obj)->getTag());
}
void EmailSystemUI::drawUI(CCObject *obj)
{
	removeObserver(MSG_QueryEmailRsp);
	GameDirector::getDirector()->hideWaiting();
	m_lSystemMailRead.clear();
	const vector<Email>* list = EmailManager::getManager()->getAllEmail();

	
	for (vector<Email>::const_iterator iter = list->begin() ; iter != list->end() ; iter ++)
	{
		m_lSystemMailRead.push_back(*iter);
	}
	if (m_cAuctionList)
	{
		m_cAuctionList->removeFromParent();
	}
	
	if (m_cScroll)
	{
		
		m_cScroll->removeFromParent();
		m_cScroll = NULL;
	}
	
	if (list->size() == 0)
	{
		string infoStr = LFStrings::getValue("NOTEMAIL");
		UnblockWarnDialog* pDlg = UnblockWarnDialog::create(infoStr);
		CCDirector::sharedDirector()->getRunningScene()->addChild(pDlg, 300);
		return;
	}

	int side = 5;

	int height = CCSprite::create("denglujiangli_jinri_bg.png")->getContentSize().height * m_lSystemMailRead.size() + side * ( m_lSystemMailRead.size() - 1) ;
	
	if(height < m_nHeight)
	{
		height = m_nHeight;
	}
	m_cAuctionList = SystemMailListUI::create();
	m_cAuctionList->ignoreAnchorPointForPosition(false);
	m_cAuctionList->setAnchorPoint(CCPointZero);
	m_cAuctionList->setContentSize(CCSizeMake(this->getContentSize().width,height));

	m_cScroll = CCScrollView::create(CCSizeMake(this->getContentSize().width,m_nHeight),m_cAuctionList);
	m_cScroll->setDirection(kCCScrollViewDirectionVertical);

	CCPoint pos = m_cScroll->getContentOffset();

	m_cScroll->setContentOffset(ccp(0,-(height - m_nHeight)));
	m_cAuctionList->setEnveroment(height,side);
	m_cScroll->setAnchorPoint(CCPointZero);
	m_cScroll->setPosition(CCPointZero);
	//m_cAuctionList->setMenuHandler(this,menu_selector(CardAuctionUI::onRandSelectCheck),menu_selector(CardAuctionUI::onRandSelectBuy));
	this->addChild(m_cScroll);

	for (vector<Email>::iterator iter = m_lSystemMailRead.begin(); iter != m_lSystemMailRead.end() ; iter ++)
	{
		m_cAuctionList->addSystemMail(*iter);
	}

	m_cAuctionList->setMenuHandler(this,SEL_MenuHandler(&EmailSystemUI::selectCheckInfo),SEL_MenuHandler(&EmailSystemUI::selectDelete),SEL_MenuHandler(&EmailSystemUI::selectOK),SEL_MenuHandler(&EmailSystemUI::selectCancel));
}