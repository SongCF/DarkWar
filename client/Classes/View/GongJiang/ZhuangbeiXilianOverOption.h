#ifndef __ZhuangbeiXilianOverOption_H__
#define __ZhuangbeiXilianOverOption_H__

#include "cocos2d.h"
#include "cocos-ext.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;

class ZhuangbeiXilianOverOption : public CCSprite
{
public:
	ZhuangbeiXilianOverOption();
	virtual ~ZhuangbeiXilianOverOption();

	static CCScene* scene();
	virtual bool init();
	CREATE_FUNC(ZhuangbeiXilianOverOption);
	virtual void onEnter();
public:
	inline void setValue(int level,int percent,int atter_type_index,bool isFirst = false,bool isBiger = false)
	{
		m_bIsFirst = isFirst;
		m_nLevel = level;
		m_nPercent = percent;
		m_nAtter_type_index = atter_type_index;
		m_bIsBiger = isBiger;
	}
private:
	void Blink(CCNode *node,float dur);
private:
	bool					m_bIsBiger;
	bool					m_bIsFirst;
	int						m_nIndex;
	int						m_nLevel;
	int						m_nPercent;
	int						m_nAtter_type_index;
};

#endif // __ZhuangbeiXilianOverOption_H__