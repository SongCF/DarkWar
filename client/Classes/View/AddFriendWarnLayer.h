#ifndef __AddFriendWarnLayer_H__
#define __AddFriendWarnLayer_H__

#include "cocos2d.h"
#include "cocos-ext.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;
class AddFriendWarnLayer : public CCLayer
{
public:
	AddFriendWarnLayer();
	virtual ~AddFriendWarnLayer();


	virtual bool init();
	CREATE_FUNC(AddFriendWarnLayer);
	virtual void onEnter();
	virtual void onExit();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	inline void setFriendName(string name)
	{
		m_sFriendName = name;
	}
private:

private:
	string			m_sFriendName;
};

#endif // __MaskLayerUI_H__