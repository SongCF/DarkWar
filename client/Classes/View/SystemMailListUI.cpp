#include "SystemMailListUI.h"
#include "EmailManager.h"
SystemMailListUI::SystemMailListUI()
	:CCLayer(),
	m_nCount(0),
	m_nSide(0),
	m_nHeight(0),
	m_mMenu(NULL)
{
}

SystemMailListUI::~SystemMailListUI() {
}
bool SystemMailListUI::init() {
	if (!CCLayer::init()) {
		return false;
	}
	m_lMailListTileList.clear();
	return true;
}

void SystemMailListUI::onEnter() {
	CCLayer::onEnter();
	
	m_mMenu = ListMenu::create();
	m_mMenu->setAnchorPoint(CCPointZero);
	m_mMenu->setPosition(CCPointZero);
	addChild(m_mMenu);
	m_cTTFColor.r = 195;
	m_cTTFColor.g = 145;
	m_cTTFColor.b = 95;

}
void SystemMailListUI::onExit() {
	CCLayer::onExit();
	m_lMailListTileList.clear();
}
void SystemMailListUI::addSystemMail(Email mail)
{
	MailListTile *tile = MailListTile::create();
	addChild(tile);
	tile->setSystemMail(mail);
	tile->ignoreAnchorPointForPosition(false);
	tile->setAnchorPoint(CCPointZero);
	CCPoint pos;
	pos.x = 5;
	pos.y = m_nHeight - ( m_nCount + 1 ) * (tile->getContentSize().height) - m_nCount * m_nSide;
	tile->setPosition(pos);
	m_nCount ++;
	m_lMailListTileList.insert(map<unsigned int,MailListTile *>::value_type(mail.email_id,tile));
	tile->setMenuHandler(this,SEL_MenuHandler(&SystemMailListUI::selectCheckInfo),SEL_MenuHandler(&SystemMailListUI::selectDelete),SEL_MenuHandler(&SystemMailListUI::selectOK),SEL_MenuHandler(&SystemMailListUI::selectCancel));
}
void SystemMailListUI::setEnveroment(int height,int side)
{
	m_nHeight = height;
	m_nSide = side;
}
void SystemMailListUI::selectCheckInfo(CCObject *obj)
{
	(m_oTarger->*m_hCheckInfo)((CCMenuItem *)obj);
}
void SystemMailListUI::selectDelete(CCObject* obj)
{
	
	(m_oTarger->*m_hClickDelete)((CCMenuItem *)obj);
}
void SystemMailListUI::selectCancel(CCObject *obj)
{
	(m_oTarger->*m_hCheckInfo)((CCMenuItem *)obj);
	//(m_oTarger->*m_hClickCancel)((CCMenuItem *)obj);
}
void SystemMailListUI::selectOK(CCObject* obj)
{
	(m_oTarger->*m_hClickOK)((CCMenuItem *)obj);
}