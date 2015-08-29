#include "NetSysUpMsgList.h"
NetSysUpMsgList::NetSysUpMsgList()
	:CCLayer(),
	m_nCount(0),
	m_nSide(0),
	m_nHeight(0),
	m_pPos(CCPointZero)
{
}

NetSysUpMsgList::~NetSysUpMsgList() {
}
bool NetSysUpMsgList::init() {
	if (!CCLayer::init()) {
		return false;
	}
	return true;
}

void NetSysUpMsgList::onEnter() {
	CCLayer::onEnter();

	m_cTTFColor.r = 195;
	m_cTTFColor.g = 145;
	m_cTTFColor.b = 95;

}
void NetSysUpMsgList::onExit() {
	CCLayer::onExit();
}
void NetSysUpMsgList::addSysMsg(HuoDongMsg msg)
{
	NetSysUpMsgTile *tile = NetSysUpMsgTile::create();
	addChild(tile);
	tile->setWidth(m_nWidth);
	tile->setSysMsg(msg);
	tile->ignoreAnchorPointForPosition(false);
	tile->setAnchorPoint(CCPointZero);
	CCPoint pos;
	pos.x = 5;
	pos.y = m_pPos.y;
	tile->setPosition(pos);
	m_nCount ++;
	m_pPos.y = pos.y + tile->getHeight() + m_nSide;
	m_nHeight = m_pPos.y - m_nSide;
}
void NetSysUpMsgList::setEnveroment(int width,int side)
{
	m_nWidth = width;
	m_nSide = side;
}