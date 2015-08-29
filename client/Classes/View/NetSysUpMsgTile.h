#ifndef __NetSysUpMsgTile__
#define __NetSysUpMsgTile__
#include <iostream>
#include "cocos2d.h"
#include <string.h>
USING_NS_CC;
using namespace std;
struct HuoDongMsg
{
	string updataMsgTitle;//�����
	unsigned int level;//�Ƽ��Ǽ�
	string timeStr;//ʱ��
	string info;//����

};
class NetSysUpMsgTile:public CCLayer
{
public:
	CREATE_FUNC(NetSysUpMsgTile);
	NetSysUpMsgTile();
	~NetSysUpMsgTile();
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void setSysMsg(HuoDongMsg msg);

	inline int getHeight()
	{
		return m_nHeight;
	}
	inline void setWidth(int width)
	{
		m_nWidth = width;
	}
private:
	int				m_nHeight;
	int				m_nWidth;
	int				m_nSide;
	int				m_nCount;
	ccColor3B		m_cTTFColor;
	HuoDongMsg		m_sSysmsg;

	CCSprite		*m_sBgSpr;
};
#endif