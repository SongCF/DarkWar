#ifndef __EmailSystemUI_H__
#define __EmailSystemUI_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "SystemMailListUI.h"
#include "BaseLayer.h"
#include "IMainLayerBody.h"
using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_CC;

class EmailSystemUI : public BaseLayer,public IMainLayerBody
{
public:
	EmailSystemUI();
	virtual ~EmailSystemUI();

	static CCScene* scene();
	virtual bool init();
	CREATE_FUNC(EmailSystemUI);
	virtual void onEnter();

	// 得到这个Body的Size
	virtual CCSize getBodySize(){return CCSize(mWinSize.width, mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height);} // 106是底的高度
	// 获得body的中点位置
	virtual CCPoint getBodyCenterPos() {return ccp(mWinSize.width/2, (mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height)/2);}
private:


private:
	void selectDelete(CCObject* obj);//选择删除按钮

	void selectCheckInfo(CCObject *obj);//查看详情

	void selectOK(CCObject* obj);//选择接受按钮

	void selectCancel(CCObject *obj);//选择拒绝

	void drawUI(CCObject *obj);
private:

	vector<Email>	m_lSystemMailRead;//所有邮件
	//vector<SystemMail>	m_lSystemMailWillRead;//未读信件

	SystemMailListUI	*m_cAuctionList;

	CCScrollView		*m_cScroll;
	int					m_nHeight;
	CCMenu				*m_mMenu;
};

#endif // __EmailSystemUI_H__