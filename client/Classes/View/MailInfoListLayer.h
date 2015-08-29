#ifndef __NetFight__MailInfoListLayer__
#define __NetFight__MailInfoListLayer__
#include <iostream>
#include "cocos2d.h"
#include <string.h>
#include "cocos-ext.h"
#include "EmailManager.h"
#include "EquipmentTaoZhuangXiangQing.h"
#include "ListMenu.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;
class MailTanChuLayer;
class MailInfoListLayer:public CCLayer,public TaoZhuangLayerInterface
{
public:
	CREATE_FUNC(MailInfoListLayer);
	MailInfoListLayer();
	~MailInfoListLayer();
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void setSystemMail(Email mail);
	inline int getWidth()
	{
		return m_nWidth;
	}
	inline void setHeight(int height)
	{
		m_nHeight = height;
	}
	inline void setMailTanChuLayer(MailTanChuLayer *layer)
	{
		m_lMailTanChuLayer = layer;
	}
private:
	void sortMail(Email mail);
	
	
	void menuItemClicked_Equip(CCObject *obj);
	void displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo);
	void menuItemClicked_equipDetailMenu(CCObject *obj);
	void menuItemClicked_close(CCObject *obj);

	virtual void callbackInterface_TaoZhuangLayer();

	inline void changeMenuPriority_callfuncND(CCNode* node, void *priority)
	{
		CCMenu *menu = dynamic_cast<CCMenu*>(node);
		if (menu)
		{
			menu->setHandlerPriority((int)priority);
		}
	}
private:
	Email					m_eEmail;//物品ID,物品数量
	int						m_nWidth;
	int						m_nHeight;

	map<string,string>		m_lThingsList;

	ListMenu					*m_mMenu;
	BaseSprite				*m_dEquimentDialog;
	EquipmentTaoZhuangXiangQing		*m_dEquimentInfoLayer;
	ListMenu						*m_mDEquipmentMenu;
	Flat_DuanzaoGroupZBInfo	mCurTaoZhuang_Info;

	//CCMenu					*m_mParentMenu;

	MailTanChuLayer			*m_lMailTanChuLayer;
};
#endif