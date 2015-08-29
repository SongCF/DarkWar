#ifndef __ZhuangbeiXilianOption_H__
#define __ZhuangbeiXilianOption_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "BaseSprite.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;

class ZhuangbeiXilianOption : public CCSprite
{
public:
	ZhuangbeiXilianOption();
	virtual ~ZhuangbeiXilianOption();

	static CCScene* scene();
	virtual bool init();
	CREATE_FUNC(ZhuangbeiXilianOption);
	virtual void onEnter();
public:
	inline void setCallBack(CCObject *obj,SEL_CallFuncN handler)
	{
		m_oTarger = obj;
		m_hCallBackHandler = handler;
	}
	inline void setLevelSheng()
	{
		m_bIsLevelSheng = true;
	}
	inline void setValueSheng()
	{
		m_bIsValueSheng = true;
	}
	inline void runActionOver(CCNode *node)
	{
		node->removeFromParent();
	}
	inline void setValue(int index,int level,int percent,int atter_type_index)
	{
		m_nIndex = index;
		m_nLevel = level;
		m_nPercent = percent;
		m_nAtter_type_index = atter_type_index;
	}
	inline bool getIsSelect()
	{
		return m_bIsSelect;
	}
	void selectOther(int index);
	//void unSelectOther(int index);

	inline void setMenuEnabled(bool isCan)
	{
		m_mMenu->setEnabled(isCan);
	}

	void selectSelf();

	void addNewShuxing();

	int isAnimation;
	void Qianhuadh(BaseSprite *xingxing,int level);
	void callfunc_AnimateEnd(CCArmature *, MovementEventType, const char *);
	
private:
	void clickSelect(CCObject *obj);
	void Blink(CCNode *node,float dur);
private:
	CCObject				*m_oTarger;
	SEL_CallFuncN			m_hCallBackHandler;

	int						m_nIndex;
	int						m_nLevel;
	int						m_nPercent;
	int						m_nAtter_type_index;

	CCMenu					*m_mMenu;
	bool					m_bIsSelect;
	CCMenuItemImage			*m_iSelectMenuItem;
	//CCMenuItemImage			*m_iUnSelectMenuItem;
	CCSprite				*m_sSelectedSpr;

	bool					m_bIsLevelSheng;

	bool					m_bIsValueSheng;
};

#endif // __ZhuangbeiXilianOption_H__