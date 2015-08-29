#ifndef __NetFight__MailTanChuLayer__
#define __NetFight__MailTanChuLayer__
#include <iostream>
#include "cocos2d.h"
#include <string.h>
#include "cocos-ext.h"
#include "MailInfoListLayer.h"
#include "EmailManager.h"
USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;
class MailTanChuLayer:public CCLayer
{
public:
	CREATE_FUNC(MailTanChuLayer);
	MailTanChuLayer();
	~MailTanChuLayer();
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void setSystemMail(Email mail);
	inline void setEnveroment(int height)
	{
		m_nHeight = height;
	}
	void getMessage(CCObject *obj);
	void selectCancel(CCObject *obj);//选择拒绝

	inline void setMenuHandler(CCObject *obj,SEL_CallFunc clickCancel)
	{
		m_oTarger = obj;
		m_hClickCancel = clickCancel;
	}
	bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	inline void changeMenuPriority_callfuncND(CCNode* node, void *priority)
	{
		CCMenu *menu = dynamic_cast<CCMenu*>(node);
		if (menu)
		{
			menu->setHandlerPriority((int)priority);
		}
	}

	// 添加消息监听
	void addObserver(SEL_CallFuncO selector,const char *name);
	// 移除消息监听
	void removeObserver(const char *name);

	inline CCMenu *getTanChuMenu()
	{
		return m_mMenu;
	}
private:
	CCMenu			*m_mMenu;
	CCMenuItem		*m_mMenuItemImage;
	CCSprite		*m_mBackSpr;

	CCObject		*m_oTarger;

	SEL_CallFunc m_hClickCancel;

	Email			m_sSystemMail;
	int				m_nHeight;
	MailInfoListLayer	*m_lMailInfoLayer;
	CCScrollView	*m_cScroll;
};
#endif