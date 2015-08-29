#ifndef __ZhuangbeiXilianUI_H__
#define __ZhuangbeiXilianUI_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Equipment.h"
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "ZhuangbeiXilianOption.h"
#include "LFAlert.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;

class ZhuangbeiXilianUI : public BaseLayer, public IMainLayerBody
{
public:
	ZhuangbeiXilianUI();
	virtual ~ZhuangbeiXilianUI();

	static CCScene* scene();
	virtual bool init(Equipment *equipment);
	static ZhuangbeiXilianUI *createWithEquipment(Equipment *equipment);
	virtual void onEnter();

	virtual CCPoint getBodyCenterPos(){return ccp(m_sBackSpr->getContentSize().width/2, m_sBackSpr->getContentSize().height/2);}

	// 得到这个Body的Size
	virtual CCSize getBodySize(){return m_sBackSpr->getContentSize();};

	// 禁用触摸
	virtual void disableAllTouchBegin()
	{
		m_mMenu->setEnabled(false);

		for (vector<ZhuangbeiXilianOption*>::iterator iter = m_uOptionList.begin(); iter != m_uOptionList.end(); iter ++)
		{
			(*iter)->setMenuEnabled(false);
		}
		
	}
	// 启用触摸
	virtual void disableAllTouchEnd()
	{
		m_mMenu->setEnabled(true);
		for (vector<ZhuangbeiXilianOption*>::iterator iter = m_uOptionList.begin(); iter != m_uOptionList.end(); iter ++)
		{
			(*iter)->setMenuEnabled(true);
		}
	}

	
private:
	void clickXilian(CCObject *obj);
	void clickSelect(CCNode *node);
	void getMessage(CCObject *obj);

	void refreshShowUI(vector<ZBAttr> old_list,vector<ZBAttr> list);
	void addShuxingShow(vector<ZBAttr> old_list,vector<ZBAttr> list);

	void getQueRenMessage(CCObject *obj);

	void clickShuoming(CCObject *obj);

	void menuItemClicked_ShuoMingMenu(CCObject *pSender);

	void cilickClose(CCObject *obj);

	void clickXilianJingHua(CCObject *obj);

	inline void clickCiaoliaoShuoMing(CCObject *obj)
	{
		m_dInfoDialog->removeFromParent();
		m_dInfoDialog = NULL;
	}
private:
	Equipment					*mEquipment;

	CCSprite					*m_sBackSpr;

	unsigned int				m_nTodayFreeCount;
	int							m_nSelectIndex;

	vector<ZhuangbeiXilianOption*> m_uOptionList;

	vector<ZBAttr>					m_lData_list;
	vector<ZBAttr>					m_lXilianData_list;

	CCMenu						*m_mMenu;

	CCSprite					*m_sShuxingShowSpr;

	CCLabelTTF					*m_tFreeCountsTTF;
	CCLabelTTF					*m_tXilianJinghuaCountTTF;
	CCLabelTTF					*m_tXilianJinghuaTTF;
	bool						m_bIsRefreshXilian;

	LFAlert						*m_dInfoDialog;
};

#endif // __ZhuangbeiXilianUI_H__