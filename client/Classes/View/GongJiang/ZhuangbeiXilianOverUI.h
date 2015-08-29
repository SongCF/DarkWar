#ifndef __ZhuangbeiXilianOverUI_H__
#define __ZhuangbeiXilianOverUI_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Equipment.h"
#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "ZhuangbeiXilianOption.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;

class ZhuangbeiXilianOverUI : public BaseLayer, public IMainLayerBody
{
public:
	ZhuangbeiXilianOverUI();
	virtual ~ZhuangbeiXilianOverUI();

	static CCScene* scene();
	virtual bool init(Equipment *equipment);
	static ZhuangbeiXilianOverUI *createWithEquipment(Equipment *equipment);
	virtual void onEnter();

	virtual CCPoint getBodyCenterPos(){return ccp(m_sBackSpr->getContentSize().width/2, m_sBackSpr->getContentSize().height/2);}

	inline void setOldData(vector<ZBAttr> old_list,vector<ZBAttr> list)
	{
		m_lOldData_list = old_list;
		m_lData_list = list;
	}
	inline void setZhuangbeiId(unsigned long long id)
	{
		m_nZhuangbeiId = id;
	}
	// 得到这个Body的Size
	virtual CCSize getBodySize(){return m_sBackSpr->getContentSize();};

	// 禁用触摸
	virtual void disableAllTouchBegin()
	{
		m_mMenu->setEnabled(false);
	}
	// 启用触摸
	virtual void disableAllTouchEnd()
	{
		m_mMenu->setEnabled(true);
	}
private:
	void clickSelectOk(CCObject *obj);
	void clickSelectCancel(CCObject *obj);

	void getMessage(CCObject *obj);
private:
	Equipment					*mEquipment;

	CCSprite					*m_sBackSpr;

	vector<ZBAttr>				m_lOldData_list;//洗炼前的属性
	vector<ZBAttr>				m_lData_list;//洗炼后的属性
	CCMenu						*m_mMenu;

	unsigned long long			m_nZhuangbeiId;


};

#endif // __ZhuangbeiXilianUI_H__