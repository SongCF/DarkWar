#include "NetSysUpMsgTile.h"
#include "MailTanChuLayer.h"
#include "NetTextListShowUI.h"
#include "NetLabelTTF.h"
#include "font.h"
NetSysUpMsgTile::NetSysUpMsgTile()
	:CCLayer(),
	m_nCount(0),
	m_nSide(0),
	m_nHeight(0)
{
}

NetSysUpMsgTile::~NetSysUpMsgTile() {
}
bool NetSysUpMsgTile::init() {
	if (!CCLayer::init()) {
		return false;
	}

	return true;
}

void NetSysUpMsgTile::onEnter() {
	CCLayer::onEnter();

	m_cTTFColor.r = 195;
	m_cTTFColor.g = 145;
	m_cTTFColor.b = 96;

	m_nSide = 5;
}
void NetSysUpMsgTile::onExit() {
	CCLayer::onExit();
}
void NetSysUpMsgTile::setSysMsg(HuoDongMsg msg)
{
	m_cTTFColor.r = 195;
	m_cTTFColor.g = 145;
	m_cTTFColor.b = 96;

	m_sSysmsg = msg;

	int side = 10;
	string inputStr = "";
	
	CCSprite *infoBgSpr = CCSprite::create("gonggao_neirong_kuang.png");

	

	CCSprite *infoBgSpr2 = CCSprite::create("gonggao_neirong_kuang_2.png");

	/*infoBgSpr2->setPosition(ccp(bgSpr->getContentSize().width / 2,infoBgSpr->getPositionY() - infoBgSpr->getContentSize().height / 2 - infoBgSpr2->getContentSize().height * infoBgSpr2->getScaleY() / 2 - side));
	bgSpr->addChild(infoBgSpr2);*/


	CCSprite *infoBgSpr3 = CCSprite::createWithTexture(infoBgSpr->getTexture());

	infoBgSpr3->setRotationX(180);
	infoBgSpr3->setPosition(ccp(infoBgSpr3->getContentSize().width / 2,infoBgSpr3->getContentSize().height / 2));
	addChild(infoBgSpr3);

	NetTextListShowUI *listShow = NetTextListShowUI::create();
	listShow->setTextSize(20);
	listShow->setTextWidth(m_nWidth);
	listShow->addMessage(listShow->getConversionMsgStr(msg.info,m_cTTFColor));
	NetLabelTTF *ttf2 = NetLabelTTF::create(listShow->getMsgString().c_str(),fontStr_kaiti,20.0f);
	ttf2->setHorizontalAlignment(kCCTextAlignmentLeft);
	ttf2->setHeight(listShow->getHeight());
	ttf2->setMsgColorTexCoords(listShow->getMsgColorTexCoordsList());
	ttf2->setPosition(ccp(ttf2->getContentSize().width / 2 + side,ttf2->getContentSize().height / 2 + infoBgSpr3->getContentSize().height));
	addChild(ttf2,2);

	CCLabelTTF *timeTTf1 = CCLabelTTF::create(LFStrings::getValue("HuoDongShiJian").c_str(),fontStr_kaiti,24.0f);
	timeTTf1->setColor(ccWHITE);
	timeTTf1->setPosition(ccp(timeTTf1->getContentSize().width / 2 + side,ttf2->getPositionY() + ttf2->getContentSize().height / 2 + timeTTf1->getContentSize().height / 2));
	addChild(timeTTf1,2);

	CCLabelTTF *timeTTF2 = CCLabelTTF::create(msg.timeStr.c_str(),fontStr_kaiti,24.0f);
	timeTTF2->setColor(ccc3(79,110,217));
	timeTTF2->setPosition(ccp(timeTTF2->getContentSize().width / 2 + side + timeTTf1->getPositionX() + timeTTf1->getContentSize().width / 2 ,timeTTf1->getPositionY()));
	addChild(timeTTF2,2);
	
	CCLabelTTF *LevelTTf = CCLabelTTF::create(LFStrings::getValue("TuiJianXingJi").c_str(),fontStr_kaiti,24.0f);
	LevelTTf->setColor(ccWHITE);
	LevelTTf->setPosition(ccp(LevelTTf->getContentSize().width / 2 + side,timeTTf1->getPositionY() + timeTTf1->getContentSize().height / 2 + LevelTTf->getContentSize().height / 2));
	addChild(LevelTTf,2);

	CCPoint pos = LevelTTf->getPosition();
	pos.x += LevelTTf->getContentSize().width / 2;
	for (int i = 0 ; i < msg.level ; i ++)
	{
		CCSprite *starSpr = CCSprite::create("gonggao_tuijianxingji.png");
		starSpr->setPosition(ccp(pos.x + i * (starSpr->getContentSize().width),pos.y));
		addChild(starSpr,2);
	}

	infoBgSpr2->setScaleY((LevelTTf->getPositionY() + LevelTTf->getContentSize().height - ttf2->getPositionY() + ttf2->getContentSize().height / 2 ) / infoBgSpr2->getContentSize().height);
	infoBgSpr2->setPosition(ccp(infoBgSpr2->getContentSize().width / 2,infoBgSpr3->getPositionY() + infoBgSpr2->getContentSize().height * infoBgSpr2->getScaleY() / 2));
	addChild(infoBgSpr2);

	infoBgSpr->setPosition(ccp(infoBgSpr->getContentSize().width / 2,infoBgSpr2->getPositionY() + infoBgSpr->getContentSize().height / 2 + infoBgSpr2->getContentSize().height * infoBgSpr2->getScaleY() / 2));
	addChild(infoBgSpr);

	CCSprite *upBgTitleSpr = CCSprite::create("gonggao_biaoti_huodong.png");
	upBgTitleSpr->setPosition(ccp(infoBgSpr->getPositionX(),infoBgSpr->getPositionY() + infoBgSpr->getContentSize().height / 2 + upBgTitleSpr->getContentSize().height / 2 + 10));
	addChild(upBgTitleSpr);

	CCLabelTTF *ttf = CCLabelTTF::create(msg.updataMsgTitle.c_str(),fontStr_kaiti,24.0f);
	ttf->setColor(ccWHITE);
	ttf->setPosition(ccp(upBgTitleSpr->getContentSize().width / 2, upBgTitleSpr->getContentSize().height / 2));
	upBgTitleSpr->addChild(ttf);

	m_nHeight = upBgTitleSpr->getPositionY() + upBgTitleSpr->getContentSize().height / 2;
}