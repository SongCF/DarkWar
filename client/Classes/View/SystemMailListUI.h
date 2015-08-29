#ifndef __NetFight__SystemMailListUI__
#define __NetFight__SystemMailListUI__
#include <iostream>
#include "cocos2d.h"
#include <string.h>
#include "ListMenu.h"
#include "MailListTile.h"
USING_NS_CC;
using namespace std;
class SystemMailListUI:public CCLayer
{
public:
	CREATE_FUNC(SystemMailListUI);
	SystemMailListUI();
	~SystemMailListUI();
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void addSystemMail(Email mail);
	void setEnveroment(int height,int side);

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
	inline void checkInfoOK(unsigned int id)
	{
		map<unsigned int,MailListTile *>::iterator iter = m_lMailListTileList.find(id);
		if (iter != m_lMailListTileList.end())
		{
			iter->second->checkInfoOK();
		}
	}
	inline void clickDeleteOK(unsigned int id)
	{
		map<unsigned int,MailListTile *>::iterator iter = m_lMailListTileList.find(id);
		if (iter != m_lMailListTileList.end())
		{
			//iter->second->clickDeleteOK();
		}
	}
	inline void clickOKOK(unsigned int id)
	{
		map<unsigned int,MailListTile *>::iterator iter = m_lMailListTileList.find(id);
		if (iter != m_lMailListTileList.end())
		{
			//iter->second->clickOKOK();
		}
	}
	inline void clickCancelOK(unsigned int id)
	{
		map<unsigned int,MailListTile *>::iterator iter = m_lMailListTileList.find(id);
		if (iter != m_lMailListTileList.end())
		{
			//iter->second->clickCancelOK();
		}
	}
private:
	ListMenu		*m_mMenu;
	int				m_nHeight;
	int				m_nSide;
	int				m_nCount;

	ccColor3B		m_cTTFColor;

	map<unsigned int,MailListTile *> m_lMailListTileList;
	CCObject		*m_oTarger;

	SEL_MenuHandler m_hCheckInfo;
	SEL_MenuHandler m_hClickDelete;
	SEL_MenuHandler m_hClickOK;
	SEL_MenuHandler m_hClickCancel;
};
#endif