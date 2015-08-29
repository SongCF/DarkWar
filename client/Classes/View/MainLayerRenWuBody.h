#pragma once

#include "BaseLayer.h"
#include "IMainLayerBody.h"
#include "ChatLayer.h"
#include "MenuPopUpLayer.h"
#include <vector>
#include "ZhuShouChatLayer.h"
using namespace std;

// 主界面里面选中人物选项的body
class MainLayerRenWuBody : public BaseLayer,public IMainLayerBody, public ChatLayerCallBack, public ZhuShouChatLayerCallBack
{
private:
	BaseSprite* imageBoard;
	vector<BaseSprite*> actors;

	void menuItemClicked(CCObject* sender);

	//体力赠送
	void display_TiLiZengSong();
	void menuItemClicked_GetTiLiGift(CCObject *pSender);
	void rsp_GetTiLiGift(CCObject *msg);

	//铸银系统
	void display_ZhuYinXiTong();
	void menuItemClicked_ZhuYin(CCObject *pSender);
	void rsp_ZhuYin(CCObject *msg);

	void buzhenCallBack(CCObject* msg);

	void huodongChaXun_Rsp(CCObject *f_msg);

	inline void removeWarnDilog()
	{
		m_dMessageDlg = NULL;
	}

	virtual void ZhuShouchatEndCallBack();
public:
	MainLayerRenWuBody(void);
	~MainLayerRenWuBody(void);

	CCMenuItemSprite *m_mItemEmail ;
	void addEmailInfo(CCObject *obj);
	void teachGuide();

	void Show7DayShuoMing();
	void play7DayLogin();
	void playJiuGuan();
	/////////////////////////////////////////////////////////
	virtual void chatEndCallBack();
	///////////////////////////////////////////////////////////////////
	// 得到body的大小
	virtual CCSize getBodySize(){return mBg->getContentSize();}
	//
	virtual CCPoint getBodyCenterPos(){return mBg->getPosition();}
    
    // 禁用触摸
	virtual void disableAllTouchBegin();
	// 启用触摸
	virtual void disableAllTouchEnd();

	///////////////////////////////////////////////////////
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	//////////////////////////////////////////////////////////////

private:
	void OpeningStory(int index);
	void addTieJiang();
	void firstOverTieJiang(CCNode *node);
	void pauseTieJiang();
	void refreshTieJiang();

	void addShangren();
	void firstOverShangren(CCNode *node);
	void pauseShangren();
	void refreshShangren();

	void addCloud();
	void refreshCloud(CCNode *node);

	void addRain();

	void playLei();
	inline void guangyunColor(CCSprite *guangyunSpr,CCNode *decNode)
	{
		CCRenderTexture* rt = CCRenderTexture::create(guangyunSpr->getContentSize().width, guangyunSpr->getContentSize().height);//创建纹理渲染范围</pre>
		ccBlendFunc blendFunc;

		blendFunc.src = GL_DST_ALPHA; //表示使用源颜色的alpha值来作为因子
		blendFunc.dst = GL_DST_ALPHA; //不使用目标颜色

		guangyunSpr->setBlendFunc(blendFunc);
		rt->begin();
		guangyunSpr->visit();//注意，这里需要先绘制B，被遮罩图片
		decNode->visit();//注意渲染顺序，先渲染的为的精灵的颜色为目标颜色，后渲染的精灵的颜色为源颜色。
		rt->end();
	}
	void updateZhuyinCishuNum();
	void rsp_query(CCObject *msg);
	void updateCircleLiziXiaoguo(CCNode* pParent, int iTag);
	void removeCircleLiziXiaoguo(CCNode* pParent, int iTag);

	void UnLockXunBao();
	void unLockXunBaoOver();

	void UnLockYingXiongBang();
	void unLockYingXiongBangOver();
private:
	MenuPopUpLayer				*m_lMenuLayer;

	CCLayerColor				*m_dMessageDlg;
protected:
	BaseSprite *mBg;

	CCMenu *mMainMenu;
	CCMenu *mMenu;

	CCPoint mActorPos[4];
	int mTag_chessboardPos;
	CCPoint m_deltaPos;

	CCPoint mBeginTouchPos; //避免 精准屏幕  不能点击人物,
//	bool mHasTouchMoved;

	int mHeroClickedId;
	bool mFullPhotoHasMoved;


	//比较特殊啊、、、
	bool m_Guyide;
	CCNode *m_node;

	bool				m_bIsFirstGame;
	bool				m_bIsFirstMaoxian;

	vector<BaseSprite *>m_lNameLabelList;

	CCSprite			*m_sTieJiangSpr1;
	CCSprite			*m_sTieJiangSpr2;
	CCSprite			*m_sTieJiangSpr3;

	int					m_nFirstTieJiangCount;

	int					m_nFirstShangrenCount;

	CCSprite			*m_sShangrenSpr1;
	CCSprite			*m_sShangrenSpr2;
	CCSprite			*m_sShangrenSpr3;

	bool				m_bChuanqiJiaoXue;
	bool				m_bChuanqiTuJian;

	bool				m_bIsGongJiangJiaoXueIng;

	bool				m_bIs7DayLogin;//是否为7日登录

	bool				m_bIsFirstBaoShi;
};

