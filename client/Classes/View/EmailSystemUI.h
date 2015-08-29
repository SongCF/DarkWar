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

	// �õ����Body��Size
	virtual CCSize getBodySize(){return CCSize(mWinSize.width, mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height);} // 106�ǵ׵ĸ߶�
	// ���body���е�λ��
	virtual CCPoint getBodyCenterPos() {return ccp(mWinSize.width/2, (mWinSize.height-MainLayerBase::getCurrentMainBase()->getFooter()->getFooterSize().height)/2);}
private:


private:
	void selectDelete(CCObject* obj);//ѡ��ɾ����ť

	void selectCheckInfo(CCObject *obj);//�鿴����

	void selectOK(CCObject* obj);//ѡ����ܰ�ť

	void selectCancel(CCObject *obj);//ѡ��ܾ�

	void drawUI(CCObject *obj);
private:

	vector<Email>	m_lSystemMailRead;//�����ʼ�
	//vector<SystemMail>	m_lSystemMailWillRead;//δ���ż�

	SystemMailListUI	*m_cAuctionList;

	CCScrollView		*m_cScroll;
	int					m_nHeight;
	CCMenu				*m_mMenu;
};

#endif // __EmailSystemUI_H__