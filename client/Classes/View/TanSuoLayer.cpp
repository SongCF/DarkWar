
#include "TanSuoLayer.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "FXToast.h"
#include "ImageManager.h"
#include "MyselfManager.h"
#include "flat.h"
#include "CS_Common.h"
#include "AwardDisplay.h"
#include "FXScrollMenu.h"
#include "ClientErrorCode.h"
#include "BaseActorPropertyParser.h"
#include "FX_CommonFunc.h"
#include "TanSuoResultLayer.h"
#include "ServerTime.h"
#include "UnblockManager.h"
#include "ShenBingShangDianDuanZao.h"
#include "TanSuoManager.h"
#include "GuideLayerBase.h"

#define MAX_TANSUOTIMES		25

enum 
{
	Tag_MenuItem_YiJianTanSuo = 1,
	Tag_MenuItem_TanSuo,
	Tag_MenuItem_JieFen,

	Tag_Label_CurNum,
	Tag_Label_ShengyuNum,
	Tag_Label_YiDianLiang,
	Tag_Label_Haixu,
	Tag_Label_DianLiang,
	Tag_Img_FenYing = 11,  //11-16


};


TanSuoLayer::TanSuoLayer()
{
	m_nFontSize = 24;
	m_nMaxCountDestroy = 3;
	m_bIsJieFeng = false;
	m_nSelectProId = -1;
	mBg = BaseSprite::create("xunbao_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu, 10);
	mMainMenu->setPosition(ccp(0,0));
	CCMenuItemImage *yijian = CCMenuItemImage::create(
		"xunbao_anniu_yijianxunbao.png",
		"xunbao_anniu_yijianxunbao_select.png",
		this, menu_selector(TanSuoLayer::menuItemClicked_TanSuo));
	mMainMenu->addChild(yijian, 0, Tag_MenuItem_YiJianTanSuo);
	yijian->setPosition(ccp(548,mBg->getContentSize().height-674));

	Button *tansuo = Button::create();
	tansuo->loadTextures("xunbao_anniu_xunbao.png","xunbao_anniu_xunbao_select.png","xunbao_anniu_xunbao_select.png",UI_TEX_TYPE_LOCAL);
	/*CCMenuItemImage *tansuo = CCMenuItemImage::create(
		"xunbao_anniu_xunbao.png",
		"xunbao_anniu_xunbao_select.png",
		this, menu_selector(TanSuoLayer::menuItemClicked_TanSuo));*/
	tansuo->addTouchEventListener(this,SEL_TouchEvent(&TanSuoLayer::clickXunBao));

	


	UILayer* layer = UILayer::create();
	layer->ignoreAnchorPointForPosition(false);
	layer->setContentSize(tansuo->getContentSize());
	layer->setAnchorPoint(ccp(0.5,0.5));
	layer->addWidget(tansuo);
	layer->setPosition(320,mBg->getContentSize().height-674);//以前cpp（x，y） 现在左边 都是Point  比如颜色 ccWHITE 也改成了Color3B::WHITE
	mBg->addChild(layer);
	tansuo->setPosition(ccp(tansuo->getContentSize().width / 2 , tansuo->getContentSize().height / 2));

	m_sXunBaoSpr = CCSprite::create("xunbao_anniu_xunbao_xiaoguo.png");
	m_sXunBaoSpr->setPosition(ccp(320,mBg->getContentSize().height-674));
	mBg->addChild(m_sXunBaoSpr);
	m_sXunBaoSpr->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(1.0f),CCFadeIn::create(1.0f),NULL)));
	//mMainMenu->addChild(tansuo, 0, Tag_MenuItem_TanSuo);
	//tansuo->setPosition(ccp(320,mBg->getContentSize().height-674));
	{
		CCMenuItemImage *jiefen = CCMenuItemImage::create(
			"xunbao_anniu_jiechufengyin.png",
			"xunbao_anniu_jiechufengyin_select.png",
			this, menu_selector(TanSuoLayer::menuItemClicked_TanSuo));
		mMainMenu->addChild(jiefen, 0, Tag_MenuItem_JieFen);
		jiefen->setPosition(ccp(320,mBg->getContentSize().height-437));
		jiefen->setVisible(false);
		CCSprite *img = CCSprite::create("xunbao_anniu_jiechufengyin_xiaoguo.png");
		jiefen->addChild(img);
		img->setPosition(ccp(jiefen->getContentSize().width/2, jiefen->getContentSize().height/2));
		img->runAction(CCRepeatForever::create(CCSequence::create(
			CCFadeTo::create(1, 50), CCFadeTo::create(1, 255), NULL)));
	}
	mMainMenu->setVisible(false);

	//title
	CCLabelTTF *tilte = CCLabelTTF::create(LFStrings::getValue("XunBaoShuoMing").c_str(), fontStr_kaiti, m_nFontSize);
	mBg->addChild(tilte);
	tilte->setColor(fonColor_PuTong);
	tilte->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height-110));
	//2
	CCLabelTTF *la1 = CCLabelTTF::create(LFStrings::getValue("XunBaoJieFeng").c_str(), fontStr_kaiti, m_nFontSize);
	mBg->addChild(la1);
	la1->setAnchorPoint(ccp(0, 0.5));
	la1->setColor(fonColor_CiYao);
	la1->setPosition(ccp(134, mBg->getContentSize().height-140));
	CCLabelTTF *la2 = CCLabelTTF::create((LFStrings::getValue("shenbing")+LFStrings::getValue("zhuangbei")).c_str()
		, fontStr_kaiti, m_nFontSize);
	la1->addChild(la2);
	la2->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Chuanqi, false, false));
	la2->setPosition(ccp(la1->getContentSize().width+la2->getContentSize().width/2, la1->getContentSize().height/2));
	la1->setPositionX((mBg->getContentSize().width-la1->getContentSize().width-la2->getContentSize().width)/2);

}

void TanSuoLayer::onEnter()
{
	BaseLayer::onEnter();
	m_bXunBaoMoved = false;
	addObserver(callfuncO_selector(TanSuoLayer::rsp_query), MSG_queryDiscoverInfoRsp);
	mCmdHlp->cmdQueryDiscoverInfo();

	teachGuide();
}
void TanSuoLayer::teachGuide()
{
	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstXunBaoTeachPlay))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstXunBaoTeachPlay);
		vector<ShowMsg> list;
		ShowMsg msg1;
		msg1.msg = LFStrings::getValue("XunBaoTeach1");
		msg1.color = ccWHITE;

		list.push_back(msg1);

		ShowMsg msg2;
		msg2.msg = LFStrings::getValue("ChuanQiZhuangBei");
		msg2.color = ccc3(235,106,12);

		list.push_back(msg2);

		ShowMsg msg3;
		msg3.msg = LFStrings::getValue("XunBaoTeach2");
		msg3.color = ccWHITE;

		list.push_back(msg3);

		ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
			list, 3,this);
		if(layer != NULL)
		{
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
		}
		return;
	}
	
}
void TanSuoLayer::ZhuShouchatEndCallBack()
{

}
void TanSuoLayer::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
}

void TanSuoLayer::disableAllTouchEnd()
{
	mMainMenu->setEnabled(true);
}

void TanSuoLayer::rsp_query(CCObject *msg)
{
	removeObserver(MSG_queryDiscoverInfoRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_QueryDiscoverInfoRsp* data = (SPCmd_QueryDiscoverInfoRsp*) ((MSG_Rsp*)msg)->getData();

	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	mMainMenu->setVisible(true);

	CCString temp;

	int xunBaoCount = CS::getDiscoverLimitTimes(MyselfManager::getManager()->getMyZhuJueData()->getVipLevel());//寻宝次数上限
	//当前剩余次数
	/*temp.initWithFormat("%u/%u", data->remainder_times, 
		CS::getDiscoverLimitTimes(MyselfManager::getManager()->getMyZhuJueData()->getVipLevel()));*/
	temp.initWithFormat("%u", data->remainder_times);
	CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, m_nFontSize);
	mBg->addChild(numLabel, 1, Tag_Label_CurNum);
	//numLabel->setAnchorPoint(ccp(0, 0.5));
	numLabel->setPosition(ccp(374, mBg->getContentSize().height-762));
	numLabel->setColor(ccGREEN);
	//int shengyuTimes = MAX_TANSUOTIMES - data->discovered_times;
	int shengyuTimes = MAX_TANSUOTIMES - data->discovered_times;

	unsigned int serverTime = ServerTime::getTime();
	int tm = 360 - (serverTime - data->last_modify_timestamp) % 360;
	string timeStr = getTimeForString(tm);
	m_nRefreshTime = tm;
	CCLabelTTF *timeTTF = CCLabelTTF::create(CCString::createWithFormat("(%s)",timeStr.c_str())->getCString(), fontStr_kaiti, m_nFontSize);
	mBg->addChild(timeTTF, 1);
	//haixuNumLabel->setAnchorPoint(ccp(0,0.5));
	timeTTF->setPosition(ccp( numLabel->getPositionX() + numLabel->getContentSize().width / 2 + timeTTF->getContentSize().width / 2 + 10, numLabel->getPositionY()));
	timeTTF->setColor(ccWHITE);
	m_tRefreshTimeTTF = timeTTF;

	if (xunBaoCount == data->remainder_times)
	{
		timeTTF->setVisible(false);
		//haixuNumLabel->setPosition(ccp( mBg->getContentSize().width / 2 - 15, 44));
	}
	else
	{
		schedule(SEL_SCHEDULE(&TanSuoLayer::refreshTime),1.0f);
		//str = CCString::createWithFormat("()")
	}

	if (data->discovered_times >= MAX_TANSUOTIMES)
	{
		temp.initWithFormat(LFStrings::getValue("XunBaoYiJing_DianLiang_String").c_str());
		CCLabelTTF *yidianliangLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, m_nFontSize, CCSize(0,0), kCCTextAlignmentCenter);
		mBg->addChild(yidianliangLabel, 1, Tag_Label_YiDianLiang);
		//yidianliangLabel->setAnchorPoint(ccp(0,0.5));
		//yidianliangLabel->setPosition(ccp(370, 54));
		yidianliangLabel->setPosition(ccp( mBg->getContentSize().width / 2, 44));
		yidianliangLabel->setColor(fonColor_PuTong);
		m_tHaixuTTF = NULL;
		m_nHadCount = data->remainder_times;
	}
	else
	{
		CCString *str = CCString::createWithFormat("%s  %s",LFStrings::getValue("XunBaoHaixu_String").c_str(),LFStrings::getValue("XunBaoDianLiang_String").c_str());
		CCLabelTTF *haixuNumLabel1 = CCLabelTTF::create(str->getCString(), fontStr_kaiti, m_nFontSize, CCSize(0,0), kCCTextAlignmentCenter);
		mBg->addChild(haixuNumLabel1, 1,Tag_Label_ShengyuNum);
		//haixuNumLabel->setAnchorPoint(ccp(0,0.5));
		haixuNumLabel1->setPosition(ccp( mBg->getContentSize().width / 2, 44));
		haixuNumLabel1->setColor(fonColor_PuTong);

		CCLabelTTF *haixuNumLabel = CCLabelTTF::create(CCString::createWithFormat("%d",shengyuTimes)->getCString(), fontStr_kaiti, m_nFontSize, CCSize(0,0), kCCTextAlignmentCenter);
		mBg->addChild(haixuNumLabel, 1,Tag_Label_Haixu);
		//haixuNumLabel->setAnchorPoint(ccp(0,0.5));
		haixuNumLabel->setPosition(ccp(haixuNumLabel1->getPositionX() - haixuNumLabel1->getContentSize().width / 2 + m_nFontSize * 2 + haixuNumLabel->getContentSize().width / 2, haixuNumLabel1->getPositionY()));
		haixuNumLabel->setColor(ccWHITE);

		m_tHaixuTTF = haixuNumLabel;
		m_nHadCount = data->remainder_times;

		
		


		
		/*
		//还需
		temp.initWithFormat(LFStrings::getValue("XunBaoHaixu_String").c_str());
		CCLabelTTF *haixuNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, m_nFontSize, CCSize(0,0), kCCTextAlignmentCenter);
		mBg->addChild(haixuNumLabel, 1, Tag_Label_Haixu);
		//haixuNumLabel->setAnchorPoint(ccp(0,0.5));
		haixuNumLabel->setPosition(ccp( mBg->getContentSize().width / 2, 44));
		haixuNumLabel->setColor(fonColor_PuTong);

		//右边剩余次数
		temp.initWithFormat(" %d ", shengyuTimes);
		CCLabelTTF *rightnumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, m_nFontSize, CCSize(40,0), kCCTextAlignmentCenter);
		mBg->addChild(rightnumLabel, 1, Tag_Label_ShengyuNum);
		rightnumLabel->setColor(ccGREEN);

		if (shengyuTimes < 10)
		{
			rightnumLabel->setPosition(ccp(haixuNumLabel->getPositionX()+ 60, haixuNumLabel->getPositionY()));
		}
		else
		{
			rightnumLabel->setPosition(ccp(haixuNumLabel->getPositionX()+ 55, haixuNumLabel->getPositionY()));
		}
		
		//次点亮石碑
		temp.initWithFormat(LFStrings::getValue("XunBaoDianLiang_String").c_str());
		CCLabelTTF *shibeiLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, m_nFontSize, CCSize(0,0), kCCTextAlignmentLeft);
		mBg->addChild(shibeiLabel, 1, Tag_Label_DianLiang);
		shibeiLabel->setAnchorPoint(ccp(0,0.5));
		shibeiLabel->setPosition(ccp(haixuNumLabel->getPositionX()+ 80, haixuNumLabel->getPositionY()));
		shibeiLabel->setColor(fonColor_PuTong);*/
	}

	//恢复提示
	/*CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("XunBaoHuiFuTiShi").c_str(), fontStr_kaiti, m_nFontSize);
	mBg->addChild(tishiLabel);
	tishiLabel->setAnchorPoint(ccp(0, 0.5));
	tishiLabel->setColor(fonColor_CiYao);
	tishiLabel->setPosition(ccp(20, mBg->getContentSize().height-802));
	*/
	//fengyin
	{
		CCPoint pos[6] = {ccp(319,838-356), ccp(444,838-386), ccp(486,838-464)
		, ccp(319,838-534), ccp(152,838-464), ccp(194,838-386)};
		float scale[6] = {.6f, .75f, .9f, 1.0f, .9f, .75f};
		char buf[128];

		int i=0;
		mDestroyCount = data->destroy_seal;
		for (int count=mDestroyCount; i<count; ++i)
		{
			CCSprite *img = CCSprite::create("xunbao_fengyinshi.png");
			mBg->addChild(img, 10-(i-3)*(i-3), Tag_Img_FenYing + i);
			img->setScale(scale[i]);
			img->setPosition(pos[i]);
			sprintf(buf, "xunbao_fengyinshi_fuwen_%d.png", i+1);
			CCSprite *fwImg = CCSprite::create(buf);
			img->addChild(fwImg);
			fwImg->setPosition(ccp(img->getContentSize().width/2, img->getContentSize().height/2));
		}
		for (; i<6; ++i)
		{
			CCSprite *img = CCSprite::create("xunbao_fengyinshi.png");
			mBg->addChild(img, 10-(i-3)*(i-3), Tag_Img_FenYing + i);
			img->setScale(scale[i]);
			img->setPosition(pos[i]);
		}
		TanSuoManager *manager = TanSuoManager::getManager();
		if (manager->canJieFeng())
		{
			m_bIsJieFeng = true;
			m_nSelectProId = -1;
			mMainMenu->getChildByTag(Tag_MenuItem_JieFen)->setVisible(true);
		}
		else
		{
			mMainMenu->getChildByTag(Tag_MenuItem_JieFen)->setVisible(false);
		}
	}
}

void TanSuoLayer::refreshDisconverTime(unsigned int count, unsigned int tansuoCount)
{
	//unsigned int iTotalCount =  CS::getDiscoverLimitTimes(MyselfManager::getManager()->getMyZhuJueData()->getVipLevel());
	CCLabelTTF *label = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_CurNum);
	if (label)
	{
		CCString temp;
		temp.initWithFormat("%u", count);
		label->setString(temp.getCString());
	}
	int xunBaoCount = CS::getDiscoverLimitTimes(MyselfManager::getManager()->getMyZhuJueData()->getVipLevel());//寻宝次数上限
	if (m_nHadCount == xunBaoCount)
	{
		m_nRefreshTime = 360;
	}
	m_nHadCount = count;
	
	if (count < xunBaoCount)
	{
		schedule(SEL_SCHEDULE(&TanSuoLayer::refreshTime),1.0f);
	}
	if (tansuoCount >= MAX_TANSUOTIMES)
	{
		mBg->removeChildByTag(Tag_Label_Haixu);
		mBg->removeChildByTag(Tag_Label_ShengyuNum);
		mBg->removeChildByTag(Tag_Label_DianLiang);

		m_tHaixuTTF = NULL;
		CCLabelTTF *pTTF = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_YiDianLiang);
		if (pTTF)
		{
			pTTF->removeFromParent();
		}
		CCString* temp = CCString::create(LFStrings::getValue("XunBaoYiJing_DianLiang_String").c_str());
		CCLabelTTF *yidianliangLabel = CCLabelTTF::create(temp->getCString(), fontStr_kaiti, m_nFontSize, CCSize(0,0), kCCTextAlignmentCenter);
		mBg->addChild(yidianliangLabel, 1, Tag_Label_YiDianLiang);
		//yidianliangLabel->setAnchorPoint(ccp(0,0.5));
		yidianliangLabel->setPosition(ccp( mBg->getContentSize().width / 2, 44));
		yidianliangLabel->setColor(fonColor_PuTong);
	}
	else
	{
		int shengyuTimes = MAX_TANSUOTIMES - tansuoCount;
		//CCString *str = CCString::createWithFormat("%s%d%s",LFStrings::getValue("XunBaoHaixu_String").c_str(),shengyuTimes,LFStrings::getValue("XunBaoDianLiang_String").c_str());
		CCLabelTTF *usedNum = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_Haixu);
		if (usedNum)
		{
			usedNum->setString(CCString::createWithFormat("%d",shengyuTimes)->getCString());
		}
	}
	
}

void TanSuoLayer::refreshDestroyFenyin(int allNum)
{
	if (allNum > 6)
	{
		CCLog("Error; %s ---> destroy Num = [%d]", __FUNCTION__, allNum);
		return;
	}

	for (int i=0; i<allNum; ++i)
	{
		CCSprite *img = (CCSprite*)mBg->getChildByTag(Tag_Img_FenYing + i);
		img->removeAllChildren();
		img->initWithFile("xunbao_fengyinshi.png");
		char buf[128];
		sprintf(buf, "xunbao_fengyinshi_fuwen_%d.png", i+1);
		CCSprite *fwImg = CCSprite::create(buf);
		img->addChild(fwImg);
		fwImg->setPosition(ccp(img->getContentSize().width/2, img->getContentSize().height/2));
	}
	for (int i=allNum; i<6; ++i)
	{
		CCSprite *img = (CCSprite*)mBg->getChildByTag(Tag_Img_FenYing + i);
		img->removeAllChildren();
		img->initWithFile("xunbao_fengyinshi.png");
	}

	mDestroyCount = allNum;

	TanSuoManager *manager = TanSuoManager::getManager();
	if (manager->canJieFeng())
	{
		m_bIsJieFeng = true;
		m_nSelectProId = -1;
		mMainMenu->getChildByTag(Tag_MenuItem_JieFen)->setVisible(true);
	}
	else
	{
		mMainMenu->getChildByTag(Tag_MenuItem_JieFen)->setVisible(false);
		m_bIsJieFeng = false;
	}
}

void TanSuoLayer::menuItemClicked_TanSuo(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_YiJianTanSuo)
	{
		if (m_bIsJieFeng)
		{
			string infoStr = LFStrings::getValue("XuYaoJieFeng");
			UnblockWarnDialog* pDlg = UnblockWarnDialog::create(infoStr);
			CCDirector::sharedDirector()->getRunningScene()->addChild(pDlg, 300);
			return;
		}
		if (MyselfManager::getManager()->getMyZhuJueData()->getVipLevel() < 3)
		{
			FXToast *layer = FXToast::create(LFStrings::getValue("XunBao_VipNeed"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		m_bDiscoverAll = true;
		addObserver(callfuncO_selector(TanSuoLayer::rsp_tansuo), MSG_discoverRsp);
		mCmdHlp->cmdDiscover(m_bDiscoverAll);
	}
	else if (tag == Tag_MenuItem_TanSuo)
	{
		m_bDiscoverAll = false;
		addObserver(callfuncO_selector(TanSuoLayer::rsp_tansuo), MSG_discoverRsp);
		mCmdHlp->cmdDiscover(m_bDiscoverAll);
	}
	else if (tag == Tag_MenuItem_JieFen)
	{
		if (mDestroyCount >= m_nMaxCountDestroy)
		{
			m_bIsJieFeng = true;
			m_nSelectProId = -1;
			if (! EquipmentManager::getManager()->isSpareEnoughToSaveEquip(1))
			{
				EquipBagNotEnoughDialog *layer = new EquipBagNotEnoughDialog;
				layer->showGoToFenJieDialog();
			}


			CCSize visSize = CCDirector::sharedDirector()->getWinSize();
			BaseSprite* dialog = BaseSprite::create("juqing_duihuakuang.png");
			dialog->setPosition(ccp(visSize.width/2, visSize.height/2));
			//label 提示
			{
				/*CCLabelTTF* label = CCLabelTTF::create(LFStrings::getValue("XunBao_XuanZeHeroTiShi").c_str(), 
				fontStr_kaiti, m_nFontSize, CCSize(425, 0), kCCTextAlignmentLeft);
				label->setAnchorPoint(ccp(0.5, 1));
				label->setPosition(ccp(dialog->getContentSize().width/2, 250));
				dialog->addChild(label, 1);*/

				CCLabelTTF *title = CCLabelTTF::create(LFStrings::getValue("JieChuFengYin").c_str(),fontStr_kaiti,m_nFontSize);
				title->setAnchorPoint(ccp(0.5,1));
				title->setPosition(ccp(dialog->getContentSize().width/2, 250));
				dialog->addChild(title, 1);


				CCLabelTTF *chuanqi = CCLabelTTF::create(LFStrings::getValue("ChuanQiZhuangBei").c_str(),fontStr_kaiti,m_nFontSize);
				chuanqi->setAnchorPoint(ccp(0.5,1));
				chuanqi->setPosition(ccp(title->getPositionX() - title->getContentSize().width / 2 + m_nFontSize * 7 + chuanqi->getContentSize().width / 2, 250));
				dialog->addChild(chuanqi, 1);
				chuanqi->setColor(ccc3(235,106,12));
			}
			{
				m_lMenuItemList.clear();
				//mHeros = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All, LF_UnLock);
				mHeros = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng, LF_UnLock);
				//tabel view
				CCTableView *tabelView = CCTableView::create(this, CCSizeMake(560, 158));
				tabelView->setDirection(kCCScrollViewDirectionHorizontal);
				tabelView->setPosition(ccp(41, 44));
				tabelView->setDelegate(this);
				dialog->addChild(tabelView);
				tabelView->reloadData();
			}
			//确认 取消按键  
			CCMenu* menu = CCMenu::create();
			menu->setPosition(CCPointZero);
			menu->setTag(0);
			//
			{
				LFAlert* lfAlert = new LFAlert();
				lfAlert->setTag(TAG_AwardDisplayer_Layer);
				lfAlert->setBaseContainer(dialog,menu);
				lfAlert->setZoneCanotTouch();
				CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, TAG_AwardDisplayer_Layer);
				lfAlert->release();
			}
			return;
		}
		else
		{
			MessageDisplay* layer = MessageDisplay::create("ERROR!");
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
	}
}

void TanSuoLayer::rsp_tansuo(CCObject *msg)
{
	removeObserver(MSG_discoverRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_DiscoverRsp* data = (SPCmd_DiscoverRsp*) ((MSG_Rsp*)msg)->getData();

	if(data->err_code != Success)
	{
		if (data->err_code == CEC_NO_TANSUO_TIME)
		{
			FXToast *layer = FXToast::create(LFStrings::getValue("XunBaoCiShuYongJing"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	mItems.clear();
	string info;
	char buf[128];
	vector<ItemInfo> items;	
	if (m_bDiscoverAll)
	{
		info = LFStrings::getValue("XunBaoJieSu");
		for (int i=0; i<data->count; ++i)
		{
			ItemInfo tItem;
			tItem.type = data->item_arr[i].type;
			tItem.count = data->item_arr[i].count;
			items.push_back(tItem);
		}
		TanSuoResultLayer *layer = NULL;
		if (data->is_destroyed)
		{
			layer = TanSuoResultLayer::create(info, items,true);
		}
		else
		{
			layer = TanSuoResultLayer::create(info, items,false);
		}
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		/*unsigned int silNum = 0, goldNum = 0, cailNum = 0;
		for (int i=0; i<data->count; ++i)
		{
			switch (data->item_arr[i].type)
			{
			case ItemsType_Silver:
				silNum += data->item_arr[i].count;
				break;
			case ItemsType_Gold:
				goldNum += data->item_arr[i].count;
				break;
			case ItemsType_QianghuaNormal:
				cailNum += data->item_arr[i].count;
				break;
			default:
				CCLog("Error: %s  ---> item type error!", __FUNCTION__);
				break;
			}

// 			ItemInfo tItem;
// 			tItem.type = data->item_arr[0].type;
// 			tItem.count = data->item_arr[0].count;
// 			mItems.push_back(tItem);
		}

		
// 		if (mItems.size() > 0)
// 		{
// 			mMainMenu->setEnabled(false);
// 			schedule(schedule_selector(TanSuoLayer::showGetItem), 0, mItems.size(), 1);
// 		}

		sprintf(buf, " x%u", silNum);
		info = LFStrings::getValue("huode") + LFStrings::getValue("yinbi") + buf;
		sprintf(buf, " x%u", cailNum);
		info += "\n" + LFStrings::getValue("huode") + LFStrings::getValue("zhutiefen") + buf;
		sprintf(buf, " x%u", goldNum);
		info += "\n" + LFStrings::getValue("huode") + LFStrings::getValue("jinbi") + buf;
		if (data->is_destroyed)
		{
			sprintf(buf, LFStrings::getValue("XunBaoJieFeng_u").c_str(), 1);
			info += "\n";
			info += buf;
		}

		CCSize visSize = CCDirector::sharedDirector()->getWinSize();
		BaseSprite* dialog = BaseSprite::create("gongming_bg.png");
		dialog->setPosition(ccp(visSize.width/2, visSize.height/2));
		//label 提示
		{
			CCLabelTTF* label = CCLabelTTF::create(info.c_str(), 
				fontStr_kaiti, m_nFontSize, CCSize(425, 0), kCCTextAlignmentLeft);
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(80, dialog->getContentSize().height/2));
			dialog->addChild(label, 1);
			label->setColor(fonColor_PuTong);

			CCLabelTTF *title = CCLabelTTF::create(LFStrings::getValue("XunBaoJieSu").c_str(), fontStr_kaiti, m_nFontSize);
			title->setAnchorPoint(ccp(0.5, 1));
			title->setPosition(ccp(dialog->getContentSize().width/2, 250));
			dialog->addChild(title, 1);
		}
		//确认 取消按键  
		CCMenu* menu = CCMenu::create();
		menu->setPosition(CCPointZero);
		{
			CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
				"shangpin_goumai_queren.png",
				"shangpin_goumai_queren_select.png",
				this,SEL_MenuHandler(&TanSuoLayer::menuItemClicked_removeAwardLayer));

			menu->addChild(menuItemOk, 1);
			menuItemOk->setPosition(ccp(dialog->getContentSize().width/2,70));
			menuItemOk->setOpacity(0);

			CCSize winsize = CCDirector::sharedDirector()->getWinSize();
			menuItemOk->setScaleX(winsize.width / menuItemOk->getContentSize().width);
			menuItemOk->setScaleY(winsize.height / menuItemOk->getContentSize().height);
		}

		//
		{
			LFAlert* lfAlert = new LFAlert();
			lfAlert->setBaseContainer(dialog,menu);
			CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, TAG_AwardDisplayer_Layer);
			lfAlert->release();
		}*/
	}
	else
	{
		if (data->count >= 1)
		{
			switch (data->item_arr[0].type)
			{
			case ItemsType_Silver:
				info = LFStrings::getValue("XunBao_YinBi");
				break;
			case ItemsType_Gold:
				info = LFStrings::getValue("XunBao_JinBi");
				break;
			case ItemsType_QianghuaNormal:
				info = LFStrings::getValue("XunBao_QiangHuaFen");
				break;
			default:
				CCLog("Error: %s  ---> item type error!", __FUNCTION__);
				break;
			}
			ItemInfo tItem;
			tItem.type = data->item_arr[0].type;
			tItem.count = data->item_arr[0].count;
			items.push_back(tItem);

			TanSuoResultLayer *layer = TanSuoResultLayer::create(info, items,false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		}
		else if (data->is_destroyed)
		{
			TanSuoResultLayer *layer = TanSuoResultLayer::create(LFStrings::getValue("XunBao_FengYin"), items,true);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		}
	}
	//m_nRefreshTime = (ServerTime::getTime() - data->) % 3600;

	//刷新 次数
	refreshDisconverTime(data->remainder_times, data->discovered_times);

	//刷新 封印
	if (data->is_destroyed) refreshDestroyFenyin(mDestroyCount + 1);
}

void TanSuoLayer::menuItemClicked_JieFenHeroSelect(CCObject *pSender)
{
	CCMenuItem *item = (CCMenuItem*)pSender;
	int tag = item->getTag();

	for (vector<CCMenuItem*>::iterator iter = m_lMenuItemList.begin() ; iter != m_lMenuItemList.end() ; iter ++)
	{
		if ((*iter)->getTag() == tag)
		{
			(*iter)->setEnabled(false);
		}
		else
		{
			(*iter)->setEnabled(true);
		}
	}
	if (m_nSelectProId == -1)
	{
		CCMenu *menu = (CCMenu*)(item->getParent());
		CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&TanSuoLayer::selectJieFengHero));

		CCNode *node = menu->getParent()->getParent()->getParent()->getParent();
		CCNode *diaoLogMenu = node->getChildByTag(0);
		menuItemOk->setPosition(ccp(node->getContentSize().width/2,0));
		diaoLogMenu->addChild(menuItemOk);
	}
	
	m_nSelectProId = tag;
	/*
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_AwardDisplayer_Layer);

	Profession profId = (Profession)tag;

	addObserver(callfuncO_selector(TanSuoLayer::rsp_jiefen), MSG_unlockDiscoverSealRsp_F);
	mCmdHlp->cmdUnlockDiscoverSeal(profId);*/
}
void TanSuoLayer::selectJieFengHero(CCObject *msg)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_AwardDisplayer_Layer);

	Profession profId = (Profession)m_nSelectProId;

	addObserver(callfuncO_selector(TanSuoLayer::rsp_jiefen), MSG_unlockDiscoverSealRsp_F);
	mCmdHlp->cmdUnlockDiscoverSeal(profId);
}
void TanSuoLayer::rsp_jiefen(CCObject *msg)
{
	removeObserver(MSG_unlockDiscoverSealRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_UnlockDiscoverSealRsp* data = (Flat_UnlockDiscoverSealRsp*) ((Flat_MSG_Rsp*)msg)->getData();

	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	ShenBingShangDianDuanZao *layer = new ShenBingShangDianDuanZao(data->equips);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1000);
	layer->release();
/*
	string info = LFStrings::getValue("gongxini") + LFStrings::getValue("huode") + ":";

	CCSize visSize = CCDirector::sharedDirector()->getWinSize();
	BaseSprite* dialog = BaseSprite::create("gongming_bg.png");
	dialog->setPosition(ccp(visSize.width/2, visSize.height/2));
	//label 提示
	{
		CCLabelTTF* label = CCLabelTTF::create(info.c_str(), fontStr_kaiti, m_nFontSize, CCSize(425, 0), kCCTextAlignmentCenter);
		label->setAnchorPoint(ccp(0.5, 1));
		label->setPosition(ccp(dialog->getContentSize().width/2, 250));
		dialog->addChild(label, 1);
	}
	{
		int allNum = data->equips.size();
		for (int i=0, posIdx=0; i<allNum; ++i, ++posIdx)
		{
			BaseSprite* diwenImg = BaseSprite::create(
				ImageManager::getManager()->getImageData_DiWen_WithBianKuang(data->equips.at(i).zhuangbei_colour)); // .count .type
			dialog->addChild(diwenImg);

			int pos_x = dialog->getContentSize().width/2;
			int delta = posIdx - allNum/2;
			if(allNum % 2 == 0) 
			{
				float del = delta + 0.5;
				pos_x += del * (diwenImg->getContentSize().width + 10);
			}
			else
			{
				pos_x += delta * (diwenImg->getContentSize().width + 10);
			}
			diwenImg->setPosition(ccp(pos_x, dialog->getContentSize().height/2));
			//equip pic
			string equipfile = data->equips.at(i).equipPic;
			equipfile = "minute_" + equipfile + ".png";
			BaseSprite *s = BaseSprite::create(equipfile);
			diwenImg->addChild(s);
			s->setScale(0.23f);
			s->setPosition(ccp(diwenImg->getContentSize().width/2,diwenImg->getContentSize().height/2));

			//数量
			char buf[20];
			sprintf(buf, "\nx%d", 1);
			string temp = data->equips.at(i).equipName;
			temp += buf;
			CCLabelTTF *labelShuliang = CCLabelTTF::create(temp.c_str(), fontStr_kaiti, m_nFontSize
				, CCSize(0,0), kCCTextAlignmentCenter);
			diwenImg->addChild(labelShuliang);
			labelShuliang->setPosition(ccp(diwenImg->getContentSize().width/2, -labelShuliang->getContentSize().height/2));
		}
	}
	//确认 取消按键  
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	{
		CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
			"shangpin_goumai_queren.png",
			"shangpin_goumai_queren_select.png",
			this,SEL_MenuHandler(&TanSuoLayer::menuItemClicked_removeAwardLayer));

		menu->addChild(menuItemOk, 1);
		menuItemOk->setPosition(ccp(dialog->getContentSize().width/2,70));
		menuItemOk->setOpacity(0);

		CCSize winsize = CCDirector::sharedDirector()->getWinSize();
		menuItemOk->setScaleX(winsize.width / menuItemOk->getContentSize().width);
		menuItemOk->setScaleY(winsize.height / menuItemOk->getContentSize().height);
	}

	//
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		lfAlert->setZoneCanotTouch();
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, TAG_AwardDisplayer_Layer);
		lfAlert->release();
	}
	*/
	if (TanSuoManager::getManager()->getDistoriedNum() == 3)
	{
		refreshDestroyFenyin(3);
	}
	else
	{
		refreshDestroyFenyin(0);
	}
	
}



CCSize TanSuoLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSize(139, 158);
}

unsigned int TanSuoLayer::numberOfCellsInTableView(CCTableView *table)
{
	return mHeros.size();
}

CCTableViewCell* TanSuoLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();
	if (cell != NULL)
	{
		cell->removeAllChildren();
	}
	else 
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}

	if (table->getParent() != NULL)
	{
		CCSize cellSize = tableCellSizeForIndex(table, idx);
		Profession profId = mHeros.at(idx)->getCurrentProperty().profession;

		//head  Profession 1-10
		BaseSprite *headImg = BaseSprite::create(ImageManager::getManager()->getImageData_ProfessionHead(profId).getCurrentImageFile());
		cell->addChild(headImg);
		headImg->setPosition(ccp(cellSize.width/2, 92));

		//menu
		FXScrollMenu *menu = FXScrollMenu::create(
			table->getParent()->convertToWorldSpace(table->getPosition()), table->getViewSize());
		cell->addChild(menu, 1);
		menu->setPosition(ccp(0,0));
		CCMenuItemImage *item = CCMenuItemImage::create(
			"renxing_renwu_touxiang.png",
			"renxing_renwu_touxiang_select.png", 
			"renxing_renwu_touxiang_select.png", 
			this, menu_selector(TanSuoLayer::menuItemClicked_JieFenHeroSelect));
		menu->addChild(item, 0, profId);

		m_lMenuItemList.push_back(item);

		item->setPosition(headImg->getPosition());
		//name
		CCLabelTTF *nameLabel = CCLabelTTF::create(BaseActorPropertyParser::getParser()->getExtraProerty(profId).nameInChinese.c_str()
			, fontStr_kaiti, m_nFontSize);
		cell->addChild(nameLabel);
		nameLabel->setPosition(ccp(headImg->getPositionX()
			, headImg->getPositionY()-headImg->getContentSize().height/2-nameLabel->getContentSize().height/2));
		nameLabel->setColor(ccc3(195, 145, 96));
	}

	return cell;
}

void TanSuoLayer::menuItemClicked_removeAwardLayer(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(TAG_AwardDisplayer_Layer);
}

void TanSuoLayer::showGetItem(float dt)
{
// 	if (mItems.size() <= 0)
// 	{
// 		mMainMenu->setEnabled(true);
// 		return;
// 	}
//	AwardDisplay *layer = AwardDisplay::create(mItems.end(), mItems);
//	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
}
void TanSuoLayer::clickXunBao(CCObject *obj,TouchEventType type)
{
	if (type == TOUCH_EVENT_BEGAN)
	{
		m_sXunBaoSpr->setVisible(false);
	}
	else if (type == TOUCH_EVENT_MOVED)
	{
		m_bXunBaoMoved = true;
	}
	else if (type == TOUCH_EVENT_ENDED)
	{
		if (m_bIsJieFeng)
		{
			string infoStr = LFStrings::getValue("XuYaoJieFeng");
			UnblockWarnDialog* pDlg = UnblockWarnDialog::create(infoStr);
			CCDirector::sharedDirector()->getRunningScene()->addChild(pDlg, 300);
			return;
		}
		m_bDiscoverAll = false;
		addObserver(callfuncO_selector(TanSuoLayer::rsp_tansuo), MSG_discoverRsp);
		mCmdHlp->cmdDiscover(m_bDiscoverAll);
	}
	else if (type == TOUCH_EVENT_CANCELED)
	{
		m_sXunBaoSpr->setVisible(true);
	}
}
void TanSuoLayer::refreshTime(float dt)
{
	int xunBaoCount = CS::getDiscoverLimitTimes(MyselfManager::getManager()->getMyZhuJueData()->getVipLevel());//寻宝次数上限
	m_nRefreshTime --;

	if (m_nRefreshTime <= 0)
	{
		m_nHadCount ++;
		m_nRefreshTime = 360;
		
	}
	CCLabelTTF *label = (CCLabelTTF*)mBg->getChildByTag(Tag_Label_CurNum);
	if (label)
	{
		CCString temp;
		temp.initWithFormat("%u", m_nHadCount);
		label->setString(temp.getCString());
	}
	if (m_nHadCount >= xunBaoCount)
	{
		m_nRefreshTime = 0;
		if (m_tHaixuTTF)
		{
			//m_tHaixuTTF->setPosition(ccp( mBg->getContentSize().width / 2 - 15, 44));
		}

		unschedule(SEL_SCHEDULE(&TanSuoLayer::refreshTime));
		m_tRefreshTimeTTF->setVisible(false);
		return;
	}
	

	if (m_tHaixuTTF)
	{
		//m_tHaixuTTF->setPosition(ccp( mBg->getContentSize().width / 2, 44));
	}
	if (m_tRefreshTimeTTF)
	{
		m_tRefreshTimeTTF->setVisible(true);
		m_tRefreshTimeTTF->setString(CCString::createWithFormat("(%s)",getTimeForString(m_nRefreshTime).c_str())->getCString());
	}
	/*
	m_nHadCount = data->remainder_times;
	if (xunBaoCount == data->remainder_times)
	{
		haixuNumLabel->setPosition(ccp( mBg->getContentSize().width / 2 - 15, 44));
	}
	else
	{
		int tm = (ServerTime::getTime() - data->last_modify_timestamp) % 3600;
		string timeStr = getTimeForString(tm);
		m_nRefreshTime = tm;
		CCLabelTTF *timeTTF = CCLabelTTF::create(CCString::createWithFormat("(%s)",timeStr.c_str())->getCString(), fontStr_kaiti, m_nFontSize);
		mBg->addChild(timeTTF, 1);
		//haixuNumLabel->setAnchorPoint(ccp(0,0.5));
		timeTTF->setPosition(ccp( numLabel->getPositionX() + numLabel->getContentSize().width / 2 + timeTTF->getContentSize().width / 2 + 10, numLabel->getPositionY()));
		timeTTF->setColor(ccWHITE);
		m_tRefreshTimeTTF = timeTTF;
		schedule(SEL_SCHEDULE(&TanSuoLayer::refreshTime),1.0f);
		//str = CCString::createWithFormat("()")
	}
	*/
}