#ifndef __MailListTile__
#define __MailListTile__
#include <iostream>
#include "cocos2d.h"
#include <string.h>
#include "ListMenu.h"
#include "MailTanChuLayer.h"
#include "EmailManager.h"
USING_NS_CC;
using namespace std;

class MailListTile:public CCLayer
{
public:
	CREATE_FUNC(MailListTile);
	MailListTile();
	~MailListTile();
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void setSystemMail(Email mail);

	void selectDelete(CCObject* obj);//选择删除按钮

	void selectCheckInfo(CCObject *obj);//查看详情

	void selectOK(CCObject* obj);//选择接受按钮

	void selectCancel(CCObject *obj);//选择拒绝

	inline void setMenuHandler(CCObject *obj,SEL_MenuHandler checkInfo,SEL_MenuHandler clickDelete,SEL_MenuHandler clickOK,SEL_MenuHandler clickCancel)
	{
		m_oTarger = obj;
		m_hCheckInfo = checkInfo;
		m_hClickDelete = clickDelete;
		m_hClickOK = clickOK;
		m_hClickCancel = clickCancel;
	}
	inline void setMenuTanChuCallBack()
	{
		m_lMailTanChuLayer = NULL;
		CCMenuItem *obj = CCMenuItem::create();
		obj->setTag(m_sSystemMail.email_id);
		(m_oTarger->*m_hCheckInfo)(obj);
	}
	void checkInfoOK();
	void clickDeleteOK(CCObject *obj);
	void clickOKOK(CCObject *obj);
	void clickCancelOK(CCObject *obj);
	void refresh();


	// 添加消息监听
	void addObserver(SEL_CallFuncO selector,const char *name);
	// 移除消息监听
	void removeObserver(const char *name);

	void getMessage(CCObject *obj);
private:
	ListMenu		*m_mMenu;
	int				m_nHeight;
	int				m_nSide;
	int				m_nCount;
	ccColor3B		m_cTTFColor;
	Email			m_sSystemMail;

	CCSprite		*m_sBgSpr;

	CCObject		*m_oTarger;

	SEL_MenuHandler m_hCheckInfo;
	SEL_MenuHandler m_hClickDelete;
	SEL_MenuHandler m_hClickOK;
	SEL_MenuHandler m_hClickCancel;

	MailTanChuLayer	*m_lMailTanChuLayer;
};
#endif