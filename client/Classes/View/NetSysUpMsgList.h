#ifndef __NetFight__NetSysUpMsgListUI__
#define __NetFight__NetSysUpMsgListUI__
#include <iostream>
#include "cocos2d.h"
#include <string.h>
#include "NetSysUpMsgTile.h"
USING_NS_CC;
using namespace std;


class NetSysUpMsgList:public CCLayer
{
public:
	CREATE_FUNC(NetSysUpMsgList);
	NetSysUpMsgList();
	~NetSysUpMsgList();
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void addSysMsg(HuoDongMsg mail);
	void setEnveroment(int width,int side);

	inline int getHeight()
	{
		return m_nHeight;
	}
private:
	int				m_nWidth;
	int				m_nHeight;
	int				m_nSide;
	int				m_nCount;

	ccColor3B		m_cTTFColor;

	CCPoint			m_pPos;
};
#endif