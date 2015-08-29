#include "MailTanChuLayer.h"
#include "EmailManager.h"
#include "font.h"
#include "CmdHelper.h"
#include "GameDirector.h"
MailTanChuLayer::MailTanChuLayer()
	:CCLayer(),
	m_nHeight(0),
	m_mMenu(NULL),
	m_mBackSpr(NULL)
{
}

MailTanChuLayer::~MailTanChuLayer() {
}
bool MailTanChuLayer::init() {
	if (!CCLayer::init()) {
		return false;
	}
	return true;
}

void MailTanChuLayer::onEnter() {
	CCLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-129,true);
	m_mMenu = CCMenu::create();
	m_mMenu->setAnchorPoint(CCPointZero);
	m_mMenu->setPosition(CCPointZero);
	addChild(m_mMenu,2);
	m_mMenu->runAction(CCCallFuncND::create(this, callfuncND_selector(MailTanChuLayer::changeMenuPriority_callfuncND), (void*)(-130)));
	CCLayerColor *colorLayer = CCLayerColor::create(ccc4(0,0,0,125));
	colorLayer->setPosition(CCPointZero);
	colorLayer->setContentSize(CCDirector::sharedDirector()->getWinSize());
	addChild(colorLayer,-1);
}
void MailTanChuLayer::onExit() {
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	
	CCLayer::onExit();
}
void MailTanChuLayer::setSystemMail(Email mail)
{
	m_sSystemMail = mail;

	CCSprite *bgSpr = CCSprite::create("ditu_renwu_tanchukuang_bg.png");
	bgSpr->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 2 , CCDirector::sharedDirector()->getWinSize().height / 2 ));
	addChild(bgSpr,0);

	CCSprite *titleSpr = NULL;
	
	CCLabelTTF *nameTTF = NULL;

	CCMenuItemImage *imageItem = NULL;

	if (mail.type == 1)
	{
		nameTTF = CCLabelTTF::create(LFStrings::getValue("XiTongBuChang").c_str(),fontStr_kaiti,25.0f);
		titleSpr = CCSprite::create("denglujiangli_langtiao.png");
		imageItem = CCMenuItemImage::create("youxiang_anniu_lingqu.png","youxiang_anniu_lingqu_select.png","youxiang_anniu_lingqu_select.png",this,SEL_MenuHandler(&MailTanChuLayer::selectCancel));
	}
	else if (mail.type == 0)
	{
		nameTTF = CCLabelTTF::create(LFStrings::getValue("XiTongJiangli").c_str(),fontStr_kaiti,25.0f);
		titleSpr = CCSprite::create("denglujiangli_langtiao.png");
		imageItem = CCMenuItemImage::create("youxiang_anniu_lingqu.png","youxiang_anniu_lingqu_select.png","youxiang_anniu_lingqu_select.png",this,SEL_MenuHandler(&MailTanChuLayer::selectCancel));
	}
	else if (mail.type == 2)
	{
		nameTTF = CCLabelTTF::create(LFStrings::getValue("XiTongTongZhi").c_str(),fontStr_kaiti,25.0f);
		titleSpr = CCSprite::create("denglujiangli_hongtiao.png");
		imageItem = CCMenuItemImage::create("youxiang_anniu_guanbi.png","youxiang_anniu_guanbi_select.png","youxiang_anniu_guanbi_select.png",this,SEL_MenuHandler(&MailTanChuLayer::selectCancel));
	}

	m_mMenuItemImage = imageItem;
	titleSpr->setPosition(ccp(bgSpr->getContentSize().width / 2,bgSpr->getContentSize().height - titleSpr->getContentSize().height / 2 - 40.0f));
	bgSpr->addChild(titleSpr);
	titleSpr->setScaleX(0.93f);

	nameTTF->setPosition(titleSpr->getPosition());
	bgSpr->addChild(nameTTF);
	nameTTF->setColor(ccWHITE);

	imageItem->setPosition(ccp(this->getContentSize().width / 2,bgSpr->getPositionY() - bgSpr->getContentSize().height / 2 + imageItem->getContentSize().height / 2 + 30));
	m_mMenu->addChild(imageItem);
	m_mBackSpr = bgSpr;


	CCLabelTTF *infoTTF = CCLabelTTF::create(mail.content.c_str(),fontStr_kaiti,20.0f);
	infoTTF->setAnchorPoint(ccp(0,1.0f));
	infoTTF->setPosition(ccp(45,bgSpr->getContentSize().height - 110));
	infoTTF->setDimensions(CCSizeMake(540,410));
	infoTTF->setHorizontalAlignment(kCCTextAlignmentLeft);
	bgSpr->addChild(infoTTF,10);
	infoTTF->setColor(ccc3(83,33,0));

	if (mail.isHaveGift)
	{
		
		CCSprite *thingSpr = NULL;
		thingSpr = CCSprite::create("denglujiangli_heidi.png");
		//thingSpr->setContentSize(CCSizeMake(width,height));
		thingSpr->setPosition(ccp(bgSpr->getContentSize().width / 2,thingSpr->getContentSize().height * thingSpr->getScaleY() / 2 + 108));
		bgSpr->addChild(thingSpr);
		CCPoint beginPos = CCPointMake(10,0);
		if (mail.equipVec.size() > 0)
		{
			thingSpr->setScaleY(1.5f);
			//beginPos = CCPointMake(10,20);
			thingSpr->setPosition(ccp(bgSpr->getContentSize().width / 2,thingSpr->getContentSize().height * thingSpr->getScaleY() / 2 + 110));
		}
		beginPos = ccp(thingSpr->getPosition().x - thingSpr->getContentSize().width / 2,thingSpr->getPositionY() - thingSpr->getContentSize().height * thingSpr->getScaleY() / 2 );
		beginPos.x += 5;
		int _width = thingSpr->getContentSize().width - 10;
		m_lMailInfoLayer = MailInfoListLayer::create();
		//m_lMailInfoLayer->ignoreAnchorPointForPosition(false);
		//m_lMailInfoLayer->setAnchorPoint(CCPointZero);
		m_lMailInfoLayer->setMailTanChuLayer(this);
		m_lMailInfoLayer->setHeight(thingSpr->getContentSize().height * thingSpr->getScaleY());
		m_lMailInfoLayer->setSystemMail(m_sSystemMail);

		int height = thingSpr->getContentSize().height * thingSpr->getScaleY();
		int width = 0;

		
		if (m_lMailInfoLayer->getWidth() > _width)
		{
			width = m_lMailInfoLayer->getWidth();
		}
		else
		{
			width = _width;
		}
		m_lMailInfoLayer->setContentSize(CCSizeMake(width,height));
		m_cScroll = CCScrollView::create(CCSizeMake(_width,height),m_lMailInfoLayer);
		m_cScroll->setDirection(kCCScrollViewDirectionHorizontal);

		//CCPoint pos = m_cScroll->getContentOffset();

		
		m_cScroll->setAnchorPoint(CCPointZero);
		m_cScroll->setPosition(beginPos);
		bgSpr->addChild(m_cScroll);
		m_cScroll->setTouchPriority(-132);
	}
}
void MailTanChuLayer::selectCancel(CCObject *obj)
{
	addObserver(callfuncO_selector(MailTanChuLayer::getMessage), MSG_ReadEmailRsp);
	CmdHelper::getHelper()->cmdReadEmail(m_sSystemMail.email_id);
	this->removeFromParent();
	(m_oTarger->*m_hClickCancel)();
}
bool MailTanChuLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void MailTanChuLayer::getMessage(CCObject *obj)
{
	removeObserver(MSG_ReadEmailRsp);
	GameDirector::getDirector()->hideWaiting();
}
void MailTanChuLayer::addObserver(SEL_CallFuncO selector,const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,selector,name,NULL);
}
// 移除消息监听
void MailTanChuLayer::removeObserver(const char *name)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,name);
}