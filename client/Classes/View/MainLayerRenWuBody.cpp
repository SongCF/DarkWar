#include "MainLayerRenWuBody.h"
#include "ImageManager.h"
#include "MyselfManager.h"
#include "MainLayerBeibaoItemList.h"
#include "MainLayerTeamList.h"
#include "StateBar.h"
#include "ZhuangBeiMianBanBase.h"
#include "GongJiangBase.h"
#include "ClientChessBoard.h"
#include "MainLayerZhuangBeiBody.h"
#include "HeroRankList.h"
#include "GameDirector.h"
#include "flat.h"
#include "TaskManager.h"
#include "MessageDisplay.h"
#include "TanSuoLayer.h"
// #include "SystemGiftLayer.h"
// #include "LoginGiftLayer.h"
#include "BonusManager.h"
#include "DebrisSynthesiseLayer.h"
#include "SystemGiftManager.h"
#include "CS_Common.h"
#include "MapsManager.h"
#include "UnblockManager.h"
#include "ChatContentManager.h"
#include "History.h"
#include "GuideLayerBase.h"
#include "VarietyStoreLayer.h"
#include "JiaoTangLayer.h"
#include "BonusManager.h"
#include "FXCircleBy.h"
#include "TuJianLayer.h"
#include "VipExplainLayer.h"
#include "ZhuangbeiScanner.h"
#include "FX_CommonFunc.h"
#include "ShenBingShangDianDuanZao.h"
#include "GiftActivtyManager.h"
#include "FXToast.h"
#include "ChuanQiLuLayer.h"
#include "Music.h"
#include "ServerTime.h"
#include "EmailSystemUI.h"
#include "NetSystemRewardUI.h"
#include "NetLogin7DayLayer.h"
#include "GongJiangFenJie.h"
#include "AddFriendUI.h"
#include "WuPinManager.h"
#include "ChessBoardStateManager.h"
using namespace FX_CommonFunc;

#define SCALE_FullPhoto 0.85f

enum 
{
	Tag_ActorFullPhoto_1 = 1,

	Tag_Img_GongJiang_Xin = 10,

	Tag_MessageInfoBar = 101,
	Tag_MessageTipStr,

	TAG_ITEM_JiuGuan  = 111,
	TAG_ITEM_JiaoTang  ,
	TAG_ITEM_GongJiang  ,

	TAG_ITEM_BeiBao  ,
	TAG_ITEM_ZhuangBei ,
	TAG_ITEM_ZaHuoPu   ,
	TAG_ITEM_YingXiongBang ,

	TAG_ITEM_HeCheng ,
	TAG_ITEM_ChuanQiLu,
	TAG_ITEM_RenWu ,
	TAG_ITEM_TuJian ,
	TAG_ITEM_TanSuo,
	Tag_ITEM_ShengjiTishi,

	TAG_ITEM_XiTongJiangLi ,
	TAG_ITEM_DengLuJiangLi ,
	TAG_ITEM_TiLiZengSong ,
	TAG_ITEM_VIP_ShuoMing ,
	TAG_ITEM_ZhuYinXiTong , //铸银系统


	TAG_ITEM_ZhuYinShengyuCishu,
	TAG_ITEM_XunbaoShengyuCishu,
	Tag_ITEM_GetTili,
	Tag_ITEM_LoginJiangli,

	Tag_Window_GiftTiLi = 676,
	Tag_Window_ZhuYin,

	TAG_ITEM_Feijie,
	TAG_ITEM_Youjian,
	TAG_ITEM_Shouqi,
	TAG_ITEM_Tanchu,
};


extern void initGuideData();

static std::map<int, bool>g_ShowShengjiTipSprInfo;
MainLayerRenWuBody::MainLayerRenWuBody(void)
{
	initGuideData(); // 教学 标记量
	m_bIs7DayLogin = false;
	m_bIsFirstBaoShi = false;
	m_node = NULL;
	m_bIsGongJiangJiaoXueIng = false;
	m_bChuanqiTuJian = false;
	m_bIsFirstMaoxian = false;
	m_bChuanqiJiaoXue = false;
	m_dMessageDlg = NULL;
	m_nFirstShangrenCount = 0;
	m_nFirstTieJiangCount = 0;
	m_lNameLabelList.clear();
	m_bIsFirstGame = false;
	m_Guyide = false;
	//bg
	mBg = BaseSprite::create("background1.png");
	addChild(mBg);
	mBg->setPosition(ccp(mBg->getContentSize().width/2, mBg->getContentSize().height/2));

	//menu
	mMainMenu = CCMenu::create();
	mBg->addChild(mMainMenu,1);
	mMainMenu->setPosition(CCPointZero);
	//添加体力按钮 会被 教堂挡住
	mMainMenu->runAction(CCCallFuncND::create(this, callfuncND_selector(BaseLayer::changeMenuPriority_callfuncND), (void*)(-127)));
	mMenu = CCMenu::create();
	mBg->addChild(mMenu, 12);
	mMenu->setPosition(ccp(0, 0));
	mMenu->runAction(CCCallFuncND::create(this, callfuncND_selector(BaseLayer::changeMenuPriority_callfuncND), (void*)(-129)));
	//


	//item
	{
		//邮件
		CCSprite *yj1 = BaseSprite::create("zhuye_anniu_youjian.png");
		CCSprite *yj2 = BaseSprite::create("zhuye_anniu_youjian_select.png");
		CCMenuItemSprite *youjian = CCMenuItemSprite::create(yj1,yj2, NULL, this, menu_selector(MainLayerRenWuBody::menuItemClicked));

		//youjian->setOpacity(0);
		mMainMenu->addChild(youjian, 1, TAG_ITEM_Youjian);
		youjian->setPosition(ccp(yj1->getContentSize().width/2, mWinSize.height-176));

		m_mItemEmail = youjian;
		addObserver(SEL_CallFuncO(&MainLayerRenWuBody::addEmailInfo),MSG_QueryEmailRsp);
		CmdHelper::getHelper()->cmdQueryEmail();
		


		//工匠
		CCSprite *gongjiang1 = BaseSprite::create();
	
		CCSprite *gongjiang2 = BaseSprite::create("zhuye_anniu_gongjiang_select.png");
		CCMenuItemSprite *gongjiang = CCMenuItemSprite::create(gongjiang1, gongjiang2, NULL, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		gongjiang1->setOpacity(0);
		gongjiang->setContentSize(gongjiang2->getContentSize());
		mMainMenu->addChild(gongjiang, 1, TAG_ITEM_GongJiang);
		gongjiang->setPosition(ccp(98, 1152-565.8f));

		{
			float min = 1.6f;
			float max = 3.0f;

			float dt = (max - min) * CCRANDOM_0_1() + min;

			CCSprite *guangyunSpr = CCSprite::create("zhuye_xiaoguo_tejiang1.png");
			guangyunSpr->setPosition(gongjiang->getPosition());
			mBg->addChild(guangyunSpr);
			guangyunColor(guangyunSpr,gongjiang);
			guangyunSpr->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(dt),CCFadeIn::create(dt),NULL)));
		}
		
		//
		BaseSprite *gjLabelImg = BaseSprite::create("zhuye_biaoshi_gongjiang.png");
		mBg->addChild(gjLabelImg, 12);
		gjLabelImg->setPosition(ccp(130, 617));

		m_lNameLabelList.push_back(gjLabelImg);
		//杂货铺
		CCSprite *jiuguan1 = BaseSprite::create("zhuye_anniu_jiuguan_select.png");
		CCSprite *jiuguan2 = BaseSprite::create("zhuye_anniu_jiuguan_select.png");
		CCMenuItemSprite *jiuguan = CCMenuItemSprite::create(jiuguan1, jiuguan2, NULL, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		jiuguan1->setOpacity(0);
		mMainMenu->addChild(jiuguan, 1, TAG_ITEM_ZaHuoPu);
		jiuguan->setPosition(ccp(530.5, 1152-478));

		{
			float min = 1.6f;
			float max = 3.0f;

			float dt = (max - min) * CCRANDOM_0_1() + min;

			CCSprite *guangyunSpr = CCSprite::create("zhuye_xiaoguo_zahuopu.png");
			guangyunSpr->setPosition(ccp(480,617));
			mBg->addChild(guangyunSpr);
			guangyunColor(guangyunSpr,jiuguan);
			guangyunSpr->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(dt),CCFadeIn::create(dt),NULL)));
		}

		{
			float min = 1.6f;
			float max = 3.0f;

			float dt = (max - min) * CCRANDOM_0_1() + min;

			CCSprite *guangyunSpr = CCSprite::create("zhuye_xiaoguo_ludeng1.png");
			guangyunSpr->setPosition(ccp(445,672));
			mBg->addChild(guangyunSpr);
			guangyunColor(guangyunSpr,mBg);
			guangyunSpr->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(dt),CCFadeIn::create(dt),NULL)));
		}

		{
			float min = 1.6f;
			float max = 3.0f;

			float dt = (max - min) * CCRANDOM_0_1() + min;

			CCSprite *guangyunSpr = CCSprite::create("zhuye_xiaoguo_ludeng2.png");
			guangyunSpr->setPosition(ccp(172,718));
			mBg->addChild(guangyunSpr);
			guangyunColor(guangyunSpr,mBg);
			guangyunSpr->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(dt),CCFadeIn::create(dt),NULL)));
		}
		//
		BaseSprite *jgLabelImg = BaseSprite::create("zhuye_biaoshi_jiuguan.png");
		mBg->addChild(jgLabelImg, 12);
		jgLabelImg->setPosition(ccp(520, 657));

		m_lNameLabelList.push_back(jgLabelImg);
		//教堂
		CCString jiaotangStr1, jiaotangStr2;
		jiaotangStr1.initWithFormat("zhuye_anniu_jiaotang_%d.png", getJiaoTangFileIdxByLevel(MyselfManager::getManager()->getJiaoTangLevel()));
		jiaotangStr2.initWithFormat("zhuye_anniu_jiaotang_%d_select.png", getJiaoTangFileIdxByLevel(MyselfManager::getManager()->getJiaoTangLevel()));
		CCSprite *jiaotang1 = BaseSprite::create(jiaotangStr1.getCString());
		CCSprite *jiaotang2 = BaseSprite::create(jiaotangStr2.getCString());
		CCMenuItemSprite *jiaotang = CCMenuItemSprite::create(jiaotang1, jiaotang2, NULL, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMainMenu->addChild(jiaotang, 1, TAG_ITEM_JiaoTang);
		jiaotang->setPosition(ccp(336, 1152-400.5));

		{
			float min = 1.6f;
			float max = 3.0f;

			float dt = (max - min) * CCRANDOM_0_1() + min;

			CCSprite *guangyunSpr = CCSprite::create("zhuye_xiaoguo_jiaotang1.png");
			guangyunSpr->setPosition(ccp(334, 648));
			mBg->addChild(guangyunSpr,10);
			guangyunColor(guangyunSpr,jiaotang);
			guangyunSpr->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(dt),CCFadeIn::create(dt),NULL)));
		}
		//
		BaseSprite *jtLabelImg = BaseSprite::create("zhuye_biaoshi_jiaotang.png");
		mBg->addChild(jtLabelImg, 12);
		jtLabelImg->setPosition(ccp(332, 773));

		m_lNameLabelList.push_back(jtLabelImg);

		//menu points
		CCPoint leftPpint1 = ccp(49, 1152-994+78*3);
		CCPoint leftPpint2 = ccp(49, 1152-994+78*2);
		CCPoint leftPpint3 = ccp(49, 1152-994+78);
		CCPoint leftPpint4 = ccp(49, 1152-994);
		CCPoint rightPpint1 = ccp(590, 1152-984+95*3);
		CCPoint rightPpint2 = ccp(590, 1152-984+95*2);
		CCPoint rightPpint3 = ccp(590, 1152-984+95);
		CCPoint rightPpint4 = ccp(590, 1152-984);

	
		//传奇录
		CCMenuItemImage *chuanqilu = CCMenuItemImage::create(
			"zhuye_anniu_chuanqilu.png"
			, "zhuye_anniu_chuanqilu_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(chuanqilu, 1, TAG_ITEM_ChuanQiLu);
		chuanqilu->setPosition(rightPpint3);
		chuanqilu->setVisible(false);

		//寻宝
		CCMenuItemImage *tansuo = CCMenuItemImage::create(
			"zhuye_anniu_xunbao.png"
			, "zhuye_anniu_xunbao_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(tansuo, 1, TAG_ITEM_TanSuo);
		tansuo->setPosition(rightPpint4);

		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstXunBaoTeachPlay))
		{
			tansuo->setVisible(false);
			tansuo->setEnabled(false);
		}
			//英雄榜
		CCMenuItemImage *yingXiongBang = CCMenuItemImage::create(
			"zhuye_anniu_yingxiongbang.png",
			"zhuye_anniu_yingxiongbang_select.png",
			this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(yingXiongBang, 1, TAG_ITEM_YingXiongBang);
		yingXiongBang->setPosition(ccp(tansuo->getPositionX(),tansuo->getPositionY()+tansuo->getContentSize().height));

		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstYingXiongBangPlay))
		{
			yingXiongBang->setVisible(false);
			yingXiongBang->setEnabled(false);
		}

		MenuPopUpLayer *layer = MenuPopUpLayer::create();
		layer->ignoreAnchorPointForPosition(false);
		layer->setAnchorPoint(CCPointZero);
		layer->setPosition(ccp(0,110));
		mBg->addChild(layer,10);
		m_lMenuLayer = layer;

		bool isTaqnchu = false;
		if (!History::getHistory()->getBoolFromCurrentHero(Key_ZhuyeMenuTanchu,isTaqnchu))
		{
			isTaqnchu = true;
		}
		if (isTaqnchu)
		{
			History::getHistory()->putInCurrentHero(Key_ZhuyeMenuTanchu,true);
			History::getHistory()->save();
			m_lMenuLayer->setTanchu(true);
		}
		else
		{

			m_lMenuLayer->setTanchu(false);
		}


		layer->setCallBack(this,SEL_CallFuncO(&MainLayerRenWuBody::menuItemClicked));

		{
			float min = 12.0f;
			float max = 18.0f;

			float dt = (max - min) * CCRANDOM_0_1() + min;

			CCSprite *shandianSpr = CCSprite::create("zhuye_xiaoguo_shandian.png");
			shandianSpr->setPosition(ccp(mBg->getContentSize().width / 2, mBg->getContentSize().height / 2));
			mBg->addChild(shandianSpr,10);
			
			CCRenderTexture* rt = CCRenderTexture::create(shandianSpr->getContentSize().width, shandianSpr->getContentSize().height);//创建纹理渲染范围</pre>
			ccBlendFunc blendFunc;

			blendFunc.src = GL_DST_COLOR; //表示使用源颜色的alpha值来作为因子
			blendFunc.dst = GL_ONE; //不使用目标颜色

			shandianSpr->setBlendFunc(blendFunc);
			rt->begin();
			shandianSpr->visit();//注意，这里需要先绘制B，被遮罩图片
			mBg->visit();//注意渲染顺序，先渲染的为的精灵的颜色为目标颜色，后渲染的精灵的颜色为源颜色。
			rt->end();

			shandianSpr->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(0),CCDelayTime::create(dt),CCCallFunc::create(this,SEL_CallFunc(&MainLayerRenWuBody::playLei)),CCDelayTime::create(0.2f),CCFadeIn::create(0.1f),CCFadeOut::create(0),CCFadeIn::create(0.1f),CCFadeOut::create(0),NULL)));
		}
		/*
		//图鉴
		CCMenuItemImage *tujian = CCMenuItemImage::create(
			"zhuye_anniu_tujian.png", 
			"zhuye_anniu_tujian_select.png",
			this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(tujian, 1, TAG_ITEM_TuJian);
		tujian->setPosition(leftPpint1);
		//酒馆
		CCMenuItemImage *zaHuoPu = CCMenuItemImage::create(
			"zhuye_anniu_zahuopu.png"
			, "zhuye_anniu_zahuopu_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(zaHuoPu, 1, TAG_ITEM_JiuGuan);
		zaHuoPu->setPosition(leftPpint2);
		//背包
		CCMenuItemImage *beibao = CCMenuItemImage::create(
			"zhuye_anniu_beibao.png"
			, "zhuye_anniu_beibao_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(beibao, 1, TAG_ITEM_BeiBao);
		beibao->setPosition(leftPpint3);
		//装备
		CCMenuItemImage *zhuangbei = CCMenuItemImage::create(
			"zhuye_anniu_zhuangbei.png"
			, "zhuye_anniu_zhuangbei_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(zhuangbei, 1, TAG_ITEM_ZhuangBei);
		zhuangbei->setPosition(leftPpint4);  
		


		

		
		//vip
		CCMenuItemImage *vipItem = CCMenuItemImage::create(
			"zhuye_anniu_vip.png"
			, "zhuye_anniu_vip_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(vipItem, 1, TAG_ITEM_VIP_ShuoMing);
		vipItem->setPosition(ccp(51, mWinSize.height-135));
		{
			CCAnimation *animation = new CCAnimation;
			animation->init();
			char buf[50];
			for (int i=1; i!=8; ++i)
			{
				sprintf(buf, "zhuye_anniu_vip_xiaoguo_%d.png", i);
				animation->addSpriteFrameWithFileName(buf);
			}
			animation->setDelayPerUnit(0.1f);
//			animation->setRestoreOriginalFrame(true);

			BaseSprite *ef = BaseSprite::create("zhuye_anniu_vip_xiaoguo_1.png");
			ef->setOpacity(0);
			vipItem->addChild(ef, 1);
			ef->setPosition(ccp(vipItem->getContentSize().width/2, vipItem->getContentSize().height/2));
			ef->runAction(CCRepeatForever::create(CCSequence::create(
				CCDelayTime::create(1.0f),
				CCFadeTo::create(0.01f, 255),
				CCAnimate::create(animation),
				CCFadeTo::create(0.01f, 0),
				CCDelayTime::create(1.0f),
				NULL)));
			animation->release();
		}*/
		////碎片合成
		//CCMenuItemImage *hecheng = CCMenuItemImage::create(
		//	"zhuye_anniu_hecheng.png"
		//	, "zhuye_anniu_hecheng_select.png"
		//	, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		//mMenu->addChild(hecheng, 1, TAG_ITEM_HeCheng);
		//hecheng->setPosition(ccp(49, 1152-984+95));
// 		//任务
// 		CCMenuItemImage *renwu = CCMenuItemImage::create(
// 			"zhuye_anniu_renwu.png"
// 			, "zhuye_anniu_renwu_select.png"
// 			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
// 		mMenu->addChild(renwu, 1, TAG_ITEM_RenWu);
// 		renwu->setPosition(ccp(49, 1152-984+95*3));
// 		{
// 			//目前不用  先去掉
// 			renwu->setVisible(false);
// 		}
		//登录奖励
		
		CCMenuItemImage *dlAward = CCMenuItemImage::create(
			"denglujiangli.png"
			, "denglujiangli_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(dlAward, 1, TAG_ITEM_DengLuJiangLi);
		//dlAward->setPosition(ccp(371, mWinSize.height-92));
		dlAward->setPosition(ccp(478, mWinSize.height-92));
		updateCircleLiziXiaoguo(dlAward, Tag_ITEM_LoginJiangli);
//  		if (BonusManager::getManager()->haveNewBonus(BonusType_All))
//  		{
//  			//partical
//  			{
//  				CCParticleSystemQuad* par = CCParticleSystemQuad::create("jiangli_xiaoguo_lizi.plist");
//  				dlAward->addChild(par, 2);
//  				par->setScale(0.5f);
//  				par->setSpeed(80);
//  				par->setPosition(ccp(dlAward->getContentSize().width/2 - 24, dlAward->getContentSize().height/2 + 5));
//  				par->runAction(
//  					CCRepeatForever::create(
//  					FXCircleBy::create(1.0f, ccp(dlAward->getContentSize().width/2, dlAward->getContentSize().height/2 + 5), 24, 360)
//  					)
//  					);
//  			}
//  		}
		//系统奖励
		/*CCMenuItemImage *sjAward = CCMenuItemImage::create(
			"shengjilibao.png"
			, "shengjilibao_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(sjAward, 1, TAG_ITEM_XiTongJiangLi);
		sjAward->setPosition(ccp(585, mWinSize.height-92));*/
// 		if (BonusManager::getManager()->haveNewBonus(BT_System))
// 		{
// 			//partical
// 			{
// 				CCParticleSystemQuad* par = CCParticleSystemQuad::create("jiangli_xiaoguo_lizi.plist");
// 				sjAward->addChild(par, 2);
// 				par->setScale(0.5f);
// 				par->setSpeed(80);
// 				par->setPosition(ccp(sjAward->getContentSize().width/2 + 24, sjAward->getContentSize().height/2 + 5));
// 				par->runAction(
// 					CCRepeatForever::create(
// 					FXCircleBy::create(1.0f, ccp(sjAward->getContentSize().width/2, sjAward->getContentSize().height/2 + 5), 24, 360)
// 					)
// 					);
// 			}
// 		}
		//体力赠送
		CCMenuItemImage *tlItem = CCMenuItemImage::create(
			"buchongtili.png"
			, "buchongtili_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(tlItem, 1, TAG_ITEM_TiLiZengSong);
		//tlItem->setPosition(ccp(478, mWinSize.height-92));
		tlItem->setPosition(ccp(585, mWinSize.height-176));




		//显示有体力可领取的粒子效果
		updateCircleLiziXiaoguo(tlItem, Tag_ITEM_GetTili);

		//银币购买
		CCMenuItemImage *zyItem = CCMenuItemImage::create(
			"zhuyin.png"
			, "zhuyin_select.png"
			, this, menu_selector(MainLayerRenWuBody::menuItemClicked));
		mMenu->addChild(zyItem, 1, TAG_ITEM_ZhuYinXiTong);
		zyItem->setPosition(ccp(585, mWinSize.height-92));

		//更新铸银剩余次数
		this->updateZhuyinCishuNum();
	}



	//查询寻宝次数
	addObserver(callfuncO_selector(MainLayerRenWuBody::rsp_query), MSG_queryDiscoverInfoRsp);
	mCmdHlp->cmdQueryDiscoverInfo();

	mActorPos[0] = ccp(250,1152-745);
	mActorPos[1] = ccp(390,1152-745);
	mActorPos[2] = ccp(215,1152-885);
	mActorPos[3] = ccp(425,1152-885);
	vector<BaseActor*> vec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
	for (int i=0; i!=vec.size(); ++i) 
	{
		int chessboardPos = vec.at(i)->getChessBoardPos();
		CCNode *node = ImageManager::getManager()->getFullPhotoOfActor(vec.at(i));
		mBg->addChild(node, chessboardPos, Tag_ActorFullPhoto_1 + chessboardPos);
		node->setPosition(mActorPos[chessboardPos-1]);
		node->getChildByTag(TAG_FULL_PHOTO_DIZUO)->setVisible(false);
		node->setUserData((void*)vec.at(i));
		node->setScale(SCALE_FullPhoto);

		CCSprite *jiazhiImg = (CCSprite*)node->getChildByTag(TAG_FULL_PHOTO_JIAZI);
		CCSprite *tishiImg = NULL;
		//好装备提示
		bool bMarked = false;
		TestReport actReport = ZhuangbeiScanner::getScanner()->getTestReportByActor(vec.at(i));
		if (actReport.reportList.size() > 0 && actReport.reportList.at(0).actor == vec.at(i))
		{
			vector<Equipment*> repEqVec = actReport.reportList.at(0).betterEquipVec;
			for (int iidx=0; iidx<repEqVec.size(); ++iidx)
			{
				if (bMarked) break;
				vector<EquipRequirePlace> placeVec = repEqVec.at(iidx)->getRequiredPlaces();
				for (int placeIdx=0; placeIdx<placeVec.size(); ++placeIdx)
				{
					//如果没有忽略
					//并且报表中这件装备就是跟当前位置在做比较，那么就算是有
 					if (! ZhuangbeiScanner::getScanner()->isIgnoreHint(actReport.reportList.at(0).actor, placeVec.at(placeIdx)) && 
  						actReport.reportList[0].oldEquipVec[iidx] == actReport.reportList.at(0).actor->getEquipmentAt(placeVec[placeIdx]))
  					{
						tishiImg = CCSprite::create("zhuye_gongjitisheng.png");
						node->addChild(tishiImg);
						CCPoint tishiPos = ccp(105*SCALE_FullPhoto, 160*SCALE_FullPhoto);
						if (jiazhiImg) tishiPos = ccp(jiazhiImg->getPositionX(), jiazhiImg->getPositionY()+jiazhiImg->getContentSize().height/2+tishiImg->getContentSize().height/2);
						tishiImg->setPosition(tishiPos);
						tishiImg->runAction(CCRepeatForever::create(
							CCSequence::create(CCMoveBy::create(0.5f, ccp(0, 20)), CCMoveBy::create(0.5f, ccp(0, -20)), NULL)));
						//
						bMarked = true;
						break;
					}
				}
			}
		}

		//升级提示
		bool bShowShengjiBtn = false;
		if (vec.at(i)->getCurrentProperty().exp_current >= vec.at(i)->getCurrentProperty().exp_need)
		{
			if (g_ShowShengjiTipSprInfo.size() < vec.size())
			{
				g_ShowShengjiTipSprInfo.insert(std::map<int, bool>::value_type(vec.at(i)->getCurrentProperty().hero_id, true));
				bShowShengjiBtn = true;
			}
			else
			{
				std::map<int, bool>::iterator it;
				for (it = g_ShowShengjiTipSprInfo.begin(); it != g_ShowShengjiTipSprInfo.end(); ++it)
				{
					if (it->first == vec.at(i)->getCurrentProperty().hero_id && it->second == true)
					{
						bShowShengjiBtn = true;
					}
				}
			}
		}

		if (bShowShengjiBtn)
		{
			CCSprite* pShengjiTishi = CCSprite::create("shengji_anniu_2.png");
			pShengjiTishi->setTag(Tag_ITEM_ShengjiTishi);
			pShengjiTishi->runAction(CCRepeatForever::create(
				CCSequence::create(CCMoveBy::create(0.5f, ccp(0, 20)), CCMoveBy::create(0.5f, ccp(0, -20)), NULL)));
			node->addChild(pShengjiTishi, 10);
			pShengjiTishi->setScale(SCALE_FullPhoto);
			if (bMarked)
			{
				tishiImg->setPosition(ccp(jiazhiImg->getPositionX()-tishiImg->getContentSize().width/2, tishiImg->getPositionY()));
				pShengjiTishi->setPosition(ccp(jiazhiImg->getPositionX()+pShengjiTishi->getContentSize().width/2, 
					jiazhiImg->getPositionY()+jiazhiImg->getContentSize().height/2*SCALE_FullPhoto+pShengjiTishi->getContentSize().height/2+10));
			}
			else
			{
				pShengjiTishi->setPosition(ccp(jiazhiImg->getPositionX(), 
						jiazhiImg->getPositionY()+jiazhiImg->getContentSize().height/2*SCALE_FullPhoto+pShengjiTishi->getContentSize().height/2+10));
			}
		}


		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJinXingTongGuan)
			&& vec.at(i)->getId() == MyselfManager::getManager()->getMyZhuJueData()->getId())
		{
			//为了方便才这样的、、
			m_Guyide = true;
			m_node = node;

			//等到 changeEquipLayer 再置false；
			//g_firstTaskEnd_to_changeEquip = false;
		}
		if (g_firstLearnSkillBook && isUI_Guide_LearnSkill 
			&& vec.at(i)->getId() == MyselfManager::getManager()->getMyZhuJueData()->getId())
		{
			//为了方便才这样的、、
			m_Guyide = true;
			m_node = node;
			//等到 JiNeng_Select 再置false；
			//g_firstLearnSkillBook = false;
		}
	}
	/*for (vector<BaseSprite *>::iterator iter = m_lNameLabelList.begin(); iter != m_lNameLabelList.end(); iter ++)
	{
	(*iter)->setVisible(false);
	}*/

	//system message info bar
//	BaseSprite *messageBar = BaseSprite::create("zhuye_xitongtonggao.png");
//	mBg->addChild(messageBar, 100, Tag_MessageInfoBar);
//	messageBar->setPosition(ccp(320, 1152-1039));

	//添加动态效果
	addTieJiang();
	addShangren();
	addCloud();
	addRain();
}


void MainLayerRenWuBody::updateCircleLiziXiaoguo(CCNode* pParent, int iTag)
{
	if (!pParent)
	{
		return;
	}

	CCParticleSystemQuad* par = (CCParticleSystemQuad*)pParent->getChildByTag(iTag);
	if (!par)
	{
		//partical
		{
			par = CCParticleSystemQuad::create("jiangli_xiaoguo_lizi.plist");
			par->setTag(iTag);
			pParent->addChild(par, 2);
			par->setScale(0.5f);
			par->setSpeed(80);
			par->setPosition(ccp(pParent->getContentSize().width/2 + 24, pParent->getContentSize().height/2 + 5));
			par->runAction(
				CCRepeatForever::create(
				FXCircleBy::create(1.0f, ccp(pParent->getContentSize().width/2, pParent->getContentSize().height/2 + 5), 24, 360)
				)
				);
			par->setVisible(false);
		}
	}
	if (iTag == Tag_ITEM_GetTili)
	{
		if (GiftActivtyManager::getManager()->getActivities(GiftActivtyManager::AF_CanGet).size() > 0)
		{
			par->setVisible(true);
		}
		else
		{
			par->removeFromParent();
		}
	}
	else if (Tag_ITEM_LoginJiangli)
	{
		if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_First7DayLogin))
		{
			if (BonusManager::getManager()->haveNewBonus(BonusType_All))
			{
				par->setVisible(true);
			}
			else
			{
				par->removeFromParent();
			}
		}
		else
		{
			if (BonusManager::getManager()->haveNewBonus(BonusType_Level) || BonusManager::getManager()->haveNewBonus(BonusType_Reward) || BonusManager::getManager()->haveNewBonus(BonusType_DayWelfare))
			{
				par->setVisible(true);
			}
			else
			{
				par->removeFromParent();
			}
		}
	}
	else
	{
		par->removeFromParent();
	}
}

void MainLayerRenWuBody::removeCircleLiziXiaoguo(CCNode* pParent, int iTag)
{
	if (!pParent)
	{
		return;
	}

	pParent->removeChildByTag(iTag);
}

void MainLayerRenWuBody::rsp_query(CCObject *msg)
{
	removeObserver(MSG_queryDiscoverInfoRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_QueryDiscoverInfoRsp* data = (SPCmd_QueryDiscoverInfoRsp*) ((MSG_Rsp*)msg)->getData();

	if(data->err_code == Success)
	{
		CCLog("%d", data->remainder_times);
		//当前寻宝次数大于0时需要提示
		if (data->remainder_times > 0)
		{
			CCMenuItemImage* pXunbao = (CCMenuItemImage*)mMenu->getChildByTag(TAG_ITEM_TanSuo);
			if (pXunbao)
			{
				//提示背景
				CCSprite* pNumTimesBg =  CCSprite::create("sphc_shuliang_bg.png");
				//剩余寻宝次数
				CCLabelTTF* pTimesTTF = CCLabelTTF::create(CCString::createWithFormat("%d", data->remainder_times)->getCString(), fontStr_BookAntiqua, 24, CCSizeMake(60, 0), kCCTextAlignmentCenter);
				pXunbao->addChild(pTimesTTF, 1, TAG_ITEM_XunbaoShengyuCishu);
				pTimesTTF->setHorizontalAlignment(kCCTextAlignmentCenter);
				pTimesTTF->setColor(fonColor_FaGuang);
				if (data->remainder_times < 10)
				{
					pTimesTTF->setFontSize(28);
				}
				pTimesTTF->setPosition(ccp(pXunbao->getContentSize().width-pNumTimesBg->getContentSize().width/2,
					pXunbao->getContentSize().height-pNumTimesBg->getContentSize().height/2+10));
				//pTimesTTF->setScale(0.8f);
				pTimesTTF->addChild(pNumTimesBg, -1);
				pNumTimesBg->setPosition(ccp(pTimesTTF->getContentSize().width/2, pTimesTTF->getContentSize().height/2));
			}
		}
	}
}

void MainLayerRenWuBody::updateZhuyinCishuNum()
{
	CCMenuItemImage* ZhuyinItem = (CCMenuItemImage*)mMenu->getChildByTag(TAG_ITEM_ZhuYinXiTong);
	if (ZhuyinItem)
	{
		CCLabelTTF* pZhuyiTimes = (CCLabelTTF*)ZhuyinItem->getChildByTag(TAG_ITEM_ZhuYinShengyuCishu);
		if (pZhuyiTimes)
		{
			unsigned int curHaveNum = MyselfManager::getManager()->getRuntionData()->cast_silver_num;
			if (curHaveNum > 0)
			{
				pZhuyiTimes->setVisible(true);
				pZhuyiTimes->setString(CCString::createWithFormat("%d", curHaveNum)->getCString());
				pZhuyiTimes->setColor(fonColor_FaGuang);
				if (curHaveNum < 10)
				{
					pZhuyiTimes->setFontSize(28);
				}
			}
			else
			{
				pZhuyiTimes->removeFromParent();
			}
		}
		else
		{
			//提示背景
			CCSprite* pNumTimesBg =  CCSprite::create("sphc_shuliang_bg.png");

			//当前铸银次数大于0时需要提示
			unsigned int curHaveNum = MyselfManager::getManager()->getRuntionData()->cast_silver_num;
			if (curHaveNum > 0)
			{
				//剩余铸银次数
				CCLabelTTF* pTimesTTF = CCLabelTTF::create(CCString::createWithFormat("%d", curHaveNum)->getCString(), fontStr_BookAntiqua, 24, CCSizeMake(60, 0), kCCTextAlignmentCenter);
				ZhuyinItem->addChild(pTimesTTF, 1, TAG_ITEM_ZhuYinShengyuCishu);
				pTimesTTF->setHorizontalAlignment(kCCTextAlignmentCenter);
				pTimesTTF->setPosition(ccp(ZhuyinItem->getContentSize().width-pNumTimesBg->getContentSize().width/2,
					ZhuyinItem->getContentSize().height-pNumTimesBg->getContentSize().height/2+10));
				//pTimesTTF->setScale(0.8f);
				pTimesTTF->setColor(fonColor_FaGuang);
				if (curHaveNum < 10)
				{
					pTimesTTF->setFontSize(28);
				}
				pTimesTTF->addChild(pNumTimesBg, -1);
				pNumTimesBg->setPosition(ccp(pTimesTTF->getContentSize().width/2, pTimesTTF->getContentSize().height/2));
			}
		}
	}
}

void MainLayerRenWuBody::menuItemClicked(CCObject* sender)
{
	CCMenuItemImage* item = (CCMenuItemImage*)(sender);
	int tag = item->getTag();
	if (tag == TAG_ITEM_JiuGuan)
	{
		bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_JiuGuan);
		
		if (bUnblock)
		{
			MainLayerTeamList* layer = new MainLayerTeamList;
			mFooterLayer->setContaintLayer(layer, true);
			layer->release();
		}
		else
		{
			UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("jiuguan_weijiesuo"), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
			return;
		}
	}
    else if (tag == TAG_ITEM_GongJiang)
	{
		bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_GongJiang);
		
		{
			//1.  2.是否正在强化教学
			if(g_gongjiang_QhHt_Opend)
				bUnblock = true;
			//3.是否刚教学完、还没提交任务   此时 g_gongjiang_QhHt_Opend为false，但History标记量已经有了、且为false
			if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_GongJiang_QhHt))
			{
				bUnblock = true;
			}
				
		}
		//
		if (bUnblock)
		{
			GongJiangBase *layer = GongJiangBase::create(0);
			mFooterLayer->setContaintLayer(layer);
		}
		else
		{
			UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("gongjiang_weijiesuo"), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
			return;
		}
	}
	else if (tag == TAG_ITEM_JiaoTang)
	{
		string unblockmsg=LFStrings::getValue("jinrushibai")+LFStrings::getValue("BanBen_ZhanWeiKaiQi");
		UnblockWarnDialog *dialog = UnblockWarnDialog::create(unblockmsg, false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(dialog);
		return;

		JiaoTangLayer *layer = new JiaoTangLayer();
		mFooterLayer->setContaintLayer(layer);
		layer->release();
	}

///////////////////////////////////////////////////////////////////////////////////////
//right
	else if (tag == TAG_ITEM_BeiBao)
	{
		MainLayerBeibaoItemList* frame = new MainLayerBeibaoItemList();
		mFooterLayer->setContaintLayer(frame);
		frame->release();
	}
	else if (tag == TAG_ITEM_ZhuangBei)
	{
		MainLayerZhuangBeiBody *layer = new MainLayerZhuangBeiBody();
		mFooterLayer->setContaintLayer(layer);
		layer->release();
	}
	else if (tag == TAG_ITEM_ZaHuoPu)
	{

		string unblockmsg=LFStrings::getValue("jinrushibai")+LFStrings::getValue("BanBen_ZhanWeiKaiQi");
		UnblockWarnDialog *dialog = UnblockWarnDialog::create(unblockmsg, false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(dialog);
		return;
		//VarietyStoreLayer *layer = VarietyStoreLayer::create();
		//mFooterLayer->setContaintLayer(layer);
	}
	else if (tag == TAG_ITEM_YingXiongBang)
	{
		bool bUnblock = UnblockManager::getManager()->isDeblocking(unblock_YingXiongBang);

		if (bUnblock)
		{
			HeroRankList *layer = new HeroRankList();
			mFooterLayer->setContaintLayer(layer);
			layer->release();
		}
		else
		{
			UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("yingxiongbang_weijiesuo"), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
			return;
		}
	}

////////////////////////////////////////////////////////////////////////////
//left
	else if(tag == TAG_ITEM_RenWu)
	{
		//活动
// 		addObserver(callfuncO_selector(MainLayerRenWuBody::huodongChaXun_Rsp), MSG_QuerySubTaskListRsp_F);
// 		mCmdHlp->cmdQuerySubTaskList();
	}
	else if (tag == TAG_ITEM_TanSuo)
	{
		TanSuoLayer *layer = new TanSuoLayer();
		mFooterLayer->setContaintLayer(layer);
		layer->release();
	}
	else if (tag == TAG_ITEM_HeCheng)
	{
		DebrisSynthesiseLayer *layer = DebrisSynthesiseLayer::create();
		mFooterLayer->setContaintLayer(layer);
	}
	else if (tag == TAG_ITEM_TuJian)
	{
		TuJianLayer *layer = new TuJianLayer();
		mFooterLayer->setContaintLayer(layer);
		layer->release();

		if (isUI_Guide_MainLine && m_bChuanqiTuJian)
		{
			layer->setChatGuide();
			m_bChuanqiTuJian = false;
		}
	}
	else if (tag == TAG_ITEM_ChuanQiLu)
	{
		if (MyselfManager::getManager()->getMyZhuJueData()->getLevel() < LEVEL_LIMIT_LEGEND_NOTE)
		{
			CCString temp;
			temp.initWithFormat(LFStrings::getValue("ZhuJue_d_JiKaiQi").c_str(), LEVEL_LIMIT_LEGEND_NOTE);
			FXToast *layer = FXToast::create(temp.getCString());
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		ChuanQiLuLayer *layer = new ChuanQiLuLayer();
		mFooterLayer->setContaintLayer(layer);
		layer->release();
	}

/////////////////////////////////////////////////////////////////////////
//up
	else if (tag == TAG_ITEM_VIP_ShuoMing)
	{
		VipExplainLayer *layer = new VipExplainLayer;
		mFooterLayer->getHeader()->setContent(BaseSprite::create());
		mFooterLayer->setContaintLayer(layer,false);
		layer->release();
	}
	else if (tag == TAG_ITEM_XiTongJiangLi)
	{	
		
	}
	else if (tag == TAG_ITEM_DengLuJiangLi)
	{

		NetSystemRewardUI*layer = NetSystemRewardUI::create();
		mFooterLayer->getHeader()->setContent(BaseSprite::create());
		mFooterLayer->setContaintLayer(layer,false);
	}
	else if (tag == TAG_ITEM_TiLiZengSong)
	{
		display_TiLiZengSong();
	}
	else if (tag == TAG_ITEM_ZhuYinXiTong)
	{
		display_ZhuYinXiTong();
	}
	else if (tag == TAG_ITEM_Tanchu)
	{
		for (vector<BaseSprite *>::iterator iter = m_lNameLabelList.begin(); iter != m_lNameLabelList.end(); iter ++)
		{
			(*iter)->setVisible(true);
		}
	}
	else if (tag == TAG_ITEM_Shouqi)
	{
		for (vector<BaseSprite *>::iterator iter = m_lNameLabelList.begin(); iter != m_lNameLabelList.end(); iter ++)
		{
			(*iter)->setVisible(false);
		}
	}
	else if (tag == TAG_ITEM_Feijie)
	{

		GongJiangFenJie *gjLayer = GongJiangFenJie::create();
		mFooterLayer->setContaintLayer(gjLayer);
		return;
	}
	else if (tag == TAG_ITEM_Youjian)
	{
		EmailSystemUI *layer = EmailSystemUI::create();
		mFooterLayer->getHeader()->setContent(BaseSprite::create());
		mFooterLayer->setContaintLayer(layer,false);
		return;
	}
}

void MainLayerRenWuBody::display_TiLiZengSong()
{
	//弹框
	BaseSprite* dialog = BaseSprite::create("zhuye_buchongtili_bg.png");
	dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	//领取，2个
	CCMenu* menu = CCMenu::create();
	menu->setPosition(CCPointZero);
	{
		//12-14点
		CCMenuItemImage* left = CCMenuItemImage::create(
			"a1.png",
			"a2.png",
			"a3.png",
			this,SEL_MenuHandler(&MainLayerRenWuBody::menuItemClicked_GetTiLiGift));
		menu->addChild(left, 1, 10);
		left->setPosition(ccp(120, 386-315));
		const ActivityInfo *giftInfoLeft = GiftActivtyManager::getManager()->getActivity(ActivityType_12_14);
		if (GiftActivtyManager::AF_CanGet != GiftActivtyManager::getManager()->getActivityState(giftInfoLeft))
			left->setEnabled(false);

		CCMenuItemImage* right = CCMenuItemImage::create(
			"a1.png",
			"a2.png",
			"a3.png",
			this,SEL_MenuHandler(&MainLayerRenWuBody::menuItemClicked_GetTiLiGift));
		menu->addChild(right, 1, 20);
		right->setPosition(ccp(400, 386-315));
		const ActivityInfo *giftInfoRight = GiftActivtyManager::getManager()->getActivity(ActivityType_18_20);
		if (GiftActivtyManager::AF_CanGet != GiftActivtyManager::getManager()->getActivityState(giftInfoRight))
			right->setEnabled(false);

		CCMenuItemImage* closeItem = CCMenuItemImage::create(
			"zhuye_buchongtili_bg.png",
			"zhuye_buchongtili_bg.png",
			this,SEL_MenuHandler(&MainLayerRenWuBody::menuItemClicked_GetTiLiGift));
		menu->addChild(closeItem, 1, 100);
		closeItem->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
		closeItem->setOpacity(0);
		closeItem->setScaleX(mWinSize.width / dialog->getContentSize().width);
		closeItem->setScaleY(mWinSize.height / dialog->getContentSize().height);
	}
	//content
	{
		CCLabelTTF *infoLabel = CCLabelTTF::create(LFStrings::getValue("ZhuYe_BuChongTiLi_ShuoMing").c_str(), fontStr_kaiti, 20, 
			CCSize(0,0), kCCTextAlignmentLeft);
		dialog->addChild(infoLabel);
		infoLabel->setAnchorPoint(ccp(0,1));
		infoLabel->setPosition(ccp(64, 284));
		infoLabel->setColor(ccWHITE);
		//
		BaseSprite *info = BaseSprite::create("zhuye_buchongtili_shijian.png");
		dialog->addChild(info);
		info->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height - 262));
		//tilte
		BaseSprite *tilImg = BaseSprite::create("zhuye_buchongtili_icon.png");
		dialog->addChild(tilImg);
		tilImg->setPosition(ccp(info->getPositionX(), dialog->getContentSize().height-69));
	}
	//
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setTag(Tag_Window_GiftTiLi);
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_Window_GiftTiLi);
		lfAlert->release();
	}
}

void MainLayerRenWuBody::menuItemClicked_GetTiLiGift(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if (tag == 10)//12-14
	{
		addObserver(callfuncO_selector(MainLayerRenWuBody::rsp_GetTiLiGift), MSG_getGiftActivityRsp);
		mCmdHlp->cmdGetGiftActivity(ActivityType_12_14);
	}
	else if (tag == 20) 
	{
		addObserver(callfuncO_selector(MainLayerRenWuBody::rsp_GetTiLiGift), MSG_getGiftActivityRsp);
		mCmdHlp->cmdGetGiftActivity(ActivityType_18_20);
	}
	else
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_GiftTiLi);
	}
}

void MainLayerRenWuBody::rsp_GetTiLiGift(CCObject *msg)
{
	removeObserver(MSG_getGiftActivityRsp);
	GameDirector::getDirector()->hideWaiting();

	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_GiftTiLi);

	SPCmd_GetGiftActivityRsp *data = (SPCmd_GetGiftActivityRsp *)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	FXToast *layer = FXToast::create(LFStrings::getValue("LingQu")+LFStrings::getValue("chenggong"));
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);

	//刷新
	StateBar* bar = dynamic_cast<StateBar*>(mFooterLayer->getHeader()->getHeaderContent());
	if (bar) bar->getTiliCallBackFunc();

// 	if (GiftActivtyManager::getManager()->getActivities(GiftActivtyManager::AF_CanGet).size() <= 0)
// 	{
// 		mMenu->getChildByTag(TAG_ITEM_TiLiZengSong)->removeChildByTag(101);
// 	}
	updateCircleLiziXiaoguo(mMenu->getChildByTag(TAG_ITEM_TiLiZengSong), Tag_ITEM_GetTili);
	
}

void MainLayerRenWuBody::display_ZhuYinXiTong()
{
	//弹框背景
	BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
	dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
	dialog->setTag(Tag_Window_ZhuYin);
	//按键
	unsigned int curHaveNum = MyselfManager::getManager()->getRuntionData()->cast_silver_num;
	CCMenu* menu;
	{
		CCMenuItemImage* menuItemMakeSure = CCMenuItemImage::create(
			"zhuyin_anniu_zhuyin.png",
			"zhuyin_anniu_zhuyin_select.png",
			"zhuyin_anniu_zhuyin_suoding.png",
			this,SEL_MenuHandler(&MainLayerRenWuBody::menuItemClicked_ZhuYin));
		menu = CCMenu::create(menuItemMakeSure, NULL);
		menu->setPosition(CCPointZero);
		menuItemMakeSure->setPosition(ccp(dialog->getContentSize().width/2,70));
		if (curHaveNum <= 0 ) menuItemMakeSure->setEnabled(false);
	}
	//content
	{
		//title
		BaseSprite *titleImg = BaseSprite::create("zhuyin_biaoti.png");
		dialog->addChild(titleImg);
		titleImg->setPosition(ccp(250, 386-67));
		//board
		BaseSprite *b= BaseSprite::create("zhuyin_tiao.png");
		dialog->addChild(b);
		b->setPosition(ccp(257, 386-171));
		//
		CCString temp;
		//1. 获得银币
		temp.initWithFormat("%s%s:", LFStrings::getValue("huode").c_str(), LFStrings::getValue("yinbi").c_str());
		CCLabelTTF *yibiLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
		dialog->addChild(yibiLabel);
		yibiLabel->setColor(fonColor_PuTong);
		yibiLabel->setAnchorPoint(ccp(0, 0.5));
		yibiLabel->setPosition(ccp(66, 386-126));
		// yinbi num
		temp.initWithFormat("%u", CS::getCastSilverCount(
			MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level));
		CCLabelTTF *yibiNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
		dialog->addChild(yibiNumLabel);
		yibiNumLabel->setPosition(ccp(264, yibiLabel->getPositionY()));
		//2. 消耗金币
		temp.initWithFormat("%s%s:", LFStrings::getValue("XiaoHao").c_str(), LFStrings::getValue("jinbi").c_str());
		CCLabelTTF *jinbiLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
		dialog->addChild(jinbiLabel);
		jinbiLabel->setColor(fonColor_PuTong);
		jinbiLabel->setAnchorPoint(ccp(0, 0.5));
		jinbiLabel->setPosition(ccp(yibiLabel->getPositionX(), 386-171));
		//jinbi num
		temp.initWithFormat("%u", CS::castSilverNeedGold(MyselfManager::getManager()->getDefaultRuntimeData()->cast_silver_num - curHaveNum + 1));//已铸次数
		CCLabelTTF *jinbiNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
		dialog->addChild(jinbiNumLabel);
		jinbiNumLabel->setPosition(ccp(264, jinbiLabel->getPositionY()));
		//3.剩余次数
		temp.initWithFormat("%s:", (LFStrings::getValue("jinri")+LFStrings::getValue("shengyucishu")).c_str());
		CCLabelTTF *cishuLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 20);
		dialog->addChild(cishuLabel);
		cishuLabel->setColor(fonColor_PuTong);
		cishuLabel->setAnchorPoint(ccp(0, 0.5));
		cishuLabel->setPosition(ccp(yibiLabel->getPositionX(), 386-215));
		//cishu num
		temp.initWithFormat("%u", curHaveNum);
		CCLabelTTF *cishuNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 20);
		dialog->addChild(cishuNumLabel);
		cishuNumLabel->setPosition(ccp(264, cishuLabel->getPositionY()));
		if (curHaveNum <= 0) cishuNumLabel->setColor(fonColor_JingGao);
	}
	//弹框
	{
		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		lfAlert->setAutoDismiss("");
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128,Tag_Window_ZhuYin);
		lfAlert->release();
	}
}

void MainLayerRenWuBody::menuItemClicked_ZhuYin(CCObject *pSender)
{
	if (m_dMessageDlg)
	{
		m_dMessageDlg->removeFromParent();
		m_dMessageDlg = NULL;
	}
	addObserver(callfuncO_selector(MainLayerRenWuBody::rsp_ZhuYin), MSG_CastSilverRsp);
	mCmdHlp->cmdCastSilver();
}

void MainLayerRenWuBody::rsp_ZhuYin(CCObject *msg)
{
	removeObserver(MSG_CastSilverRsp);
	GameDirector::getDirector()->hideWaiting();

	SPCmd_CastSilverRsp *data = (SPCmd_CastSilverRsp*)((MSG_Rsp*)msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	StateBar * bar_ = (StateBar*)mFooterLayer->getHeader()->getHeaderContent();
	if (bar_) bar_->refreshGoldAndSilver();

// 	FXToast *layer = FXToast::create(LFStrings::getValue("ZhuYin")+LFStrings::getValue("chenggong"));
// 	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	unsigned int iCount = CS::getCastSilverCount(MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level);
	string infoStr = LFStrings::getValue("yinbi") + "+" + CCString::createWithFormat("%u", iCount)->getCString();
	UnblockWarnDialog* pDlg =UnblockWarnDialog::create(infoStr);
	CCDirector::sharedDirector()->getRunningScene()->addChild(pDlg, 300);
	m_dMessageDlg = pDlg;
	pDlg->setCallBack(this,SEL_CallFunc(&MainLayerRenWuBody::removeWarnDilog));
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Window_ZhuYin);
	ccBlendFunc tmp_oBlendFunc = {GL_DST_COLOR, GL_ONE};  
	pDlg->setBlendFunc(tmp_oBlendFunc);
	display_ZhuYinXiTong();
	updateZhuyinCishuNum();
}

MainLayerRenWuBody::~MainLayerRenWuBody(void)
{
	m_lNameLabelList.clear();
}

// 禁用触摸
void MainLayerRenWuBody::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
	mMenu->setEnabled(false);
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}
// 启用触摸
void MainLayerRenWuBody::disableAllTouchEnd()
{
	mMainMenu->setEnabled(true);
	mMenu->setEnabled(true);
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -129, true);
}

void MainLayerRenWuBody::teachGuide()
{
	//AddFriendUI *layer = AddFriendUI::create();
	//layer->ignoreAnchorPointForPosition(false);
	//layer->setAnchorPoint(CCPointZero);
	//layer->setPosition(CCPointZero);
	//layer->setFriendID(2);
	////layer->setFriendName(getNewFirendName(8));

	//CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);

	//return;
	m_bIsFirstBaoShi = false;
	bool isFirstIn = GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstComeInGame);
	if (isFirstIn)
	{
		ChatLayer *layer = ChatLayer::create(
			ChatContentManager::sharedChatContentManager()->getGuideChatContent("ZhuYeZhenZhangDuiHua"), this);
		mFooterLayer->getFooter()->addChild(layer, 1024, TAG_ChatLayer);
		return;
	}
	
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstMaoXian))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstMaoXian);
		//bool isTeach = GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstMaoXian);
		//if (isTeach)
		{
			GameGuideManager::getManager()->setCurGuideState(1);
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCMenuItem* menuItem = mFooterLayer->getFooter()->getMaoXianMenu();
			guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
				, LFStrings::getValue("kaiqinidemaoxianlvcheng"), NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
			return;
		}
	}
	
	int curTask = TaskManager::getManager()->getLastActivedMainTaskId();
	TaskStatus state = TaskManager::getManager()->getLastActivedMainTaskStatus();
	int index = TaskManager::getManager()->getCurrentFightGridIdx();

	bool isBoss = ChessBoardStateManager::getManager()->isBossSection(curTask,index);
	//主线教学
	if ((curTask == 1 && state == TaskStatus_Got && isBoss) || (curTask == 1 && state == TaskStatus_Finished))
	{

		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJinXingTongGuan) && GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstMaoXian))
		{
			if (!GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstJinXingTongGuan))
			{
				GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstJinXingTongGuan);
			}

		
			{
				vector<ShowMsg> list;
				ShowMsg msg;
				msg.msg = LFStrings::getValue("FirstHuanZhuang");
				msg.color = ccWHITE;

				list.push_back(msg);

				ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
					list, 1,this);
				if(layer != NULL)
				{
					CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
				}
				return;
			}
		}
	}

	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FenJieUnLock) && GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_FenJie))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FenJieUnLock);
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FenJieUnLock);
		m_lMenuLayer->setFenJiePlayCallBack(this,SEL_CallFunc(&MainLayerRenWuBody::play7DayLogin));
		m_lMenuLayer->UnLockFenJie();
		return;
	}

	if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstXunBaoTeach) && !GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstXunBaoTeachPlay))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstXunBaoTeachPlay);
		//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstXunBaoTeachPlay);
		UnLockXunBao();
		return;
	}
	if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstYingXiongBang) && !GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstYingXiongBangPlay))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstYingXiongBangPlay);
		//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstYingXiongBangPlay);
		UnLockYingXiongBang();
		return;
	}

	if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJingJiChang) && !GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJingJiChangPlay))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstJingJiChangPlay);
		//GameGuideManager::getManager()->setCurGuideState(1);
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = mFooterLayer->getFooter()->getJingJiMenu();
		guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue(""), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		return;
	}
	if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJiuGuan) && !GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJiuGuanPlay))
	{
		g_JiuGuan_Opend = false;
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstJiuGuanPlay);
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstJiuGuanPlay);
		m_lMenuLayer->setFenJiePlayCallBack(this,SEL_CallFunc(&MainLayerRenWuBody::playJiuGuan));
		m_lMenuLayer->UnLockJiuGuan();
		return;
	}
	if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_QhHt) && !GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_QiangHua))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_QiangHua);

		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(TAG_ITEM_GongJiang);
		guideLayer->init(
			CCSize(menuItem->getContentSize().width, menuItem->getContentSize().height)
			, menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue("gongjiang_gongJiangPu"), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		return;
	}
	
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstBuZhen) && GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstJiuGuanIn))
	{
		m_bIs7DayLogin = true;
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstBuZhen);
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstBuZhen);

		vector<ShowMsg> list;
		ShowMsg msg;
		msg.msg = LFStrings::getValue("BuZhenShuoMing");
		msg.color = ccWHITE;

		list.push_back(msg);

		ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
			list, 3,this);
		if(layer != NULL)
		{
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
		}
		return;
	}

	if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstXueSeJiaoTang) && !GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstXueSeJiaoTangPlay))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstXueSeJiaoTangPlay);
		//GameGuideManager::getManager()->setCurGuideState(1);
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = mFooterLayer->getFooter()->getHuoDongMenu();
		guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue(""), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		return;
	}
	if ( (WuPinManager::getManager()->getAllBaoshi().size() > 0) && (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GetFirstBaoShi)))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GetFirstBaoShi);
		vector<ShowMsg> list;
		ShowMsg msg;
		msg.msg = LFStrings::getValue("BaoShiShuoMing");
		msg.color = fonColor_PuTong;

		list.push_back(msg);

		ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
			list, 2,this);
		if(layer != NULL)
		{
			m_bIsFirstBaoShi = true;
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
		}
		return;
	}


	//if (!m_bIsFirstGame)
	//{//是否经历过冒险
	//	bool isMaoxianEnded = false;
	//	if(!History::getHistory()->getBoolFromCurrentHero(Key_HeroIsMaoxianEnded, isMaoxianEnded))
	//	{
	//		History::getHistory()->putInCurrentHero(Key_HeroIsMaoxianEnded,false);
	//		m_bIsFirstMaoxian = true;
	//		ChatLayer *layer = ChatLayer::create(
	//			ChatContentManager::sharedChatContentManager()->getGuideChatContent("FirstMaoxian"), this);
	//		mFooterLayer->getFooter()->addChild(layer, 1024, TAG_ChatLayer);
	//	}
	//	

	//}
	//主线 换装 教学
	/*
	if (m_Guyide && g_firstTaskEnd_to_changeEquip)
	{
		vector<ShowMsg> list;
		ShowMsg msg;
		msg.msg = LFStrings::getValue("FirstHuanZhuang");
		msg.color = ccWHITE;

		list.push_back(msg);

		ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
			list, 1,this);
		if(layer != NULL)
		{
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
		}
	}


	//技能教学
	else if (m_Guyide && g_firstLearnSkillBook)
	{ 
		bool isFirstLearnSkillBook = false;
		if(History::getHistory()->getBoolFromCurrentHero(Key_IsFirstLearnSkillBook, isFirstLearnSkillBook))
		{
			if(isFirstLearnSkillBook)
				isFirstLearnSkillBook = true;
		}
		else
		{
			isFirstLearnSkillBook = true;
		}
		if(isFirstLearnSkillBook)
		{
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCSize size = m_node->getChildByTag(TAG_FULL_PHOTO_JIAZI)->getContentSize();
			guideLayer->init(CCSize(size.width/2, size.height/3*2), m_node->getParent()->convertToWorldSpace(ccp(m_node->getPositionX(), m_node->getPositionY()+20))
				, LFStrings::getValue(""), NULL, false);
			mFooterLayer->getFooter()->addChild(guideLayer, 1024);
		}
	}


	//工匠教学
	else if (isUI_Guide_GongJiang && g_gongjiang_QhHt_Opend)
	{
		m_bIsGongJiangJiaoXueIng = true;
		ChatLayer *layer = ChatLayer::create(
			ChatContentManager::sharedChatContentManager()->getGuideChatContent("gongjiang_qiangHuaHuiTui"), this);
		if(layer != NULL)
			mFooterLayer->getFooter()->addChild(layer, 1024, TAG_ChatLayer);
	}

	//传奇录教学
	else if (g_ChuanQiLu_Opend)
	{
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem *item = (CCMenuItem*)mMenu->getChildByTag(TAG_ITEM_ChuanQiLu);
		if (item)
		{
			guideLayer->init(
				item->getContentSize(), 
				item->getParent()->convertToWorldSpace(item->getPosition()), 
				LFStrings::getValue("ChuanQiLu_DianJi"), 
				NULL, 
				false);
			mFooterLayer->getFooter()->addChild(guideLayer, 1024);
		}
	}
	//传奇碎片合成教学
	else if (g_ChuanQiSuiPianHeCheng_Guide)
	{
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem *item = (CCMenuItem*)mMenu->getChildByTag(TAG_ITEM_HeCheng);
		if (item)
		{
			guideLayer->init(
				item->getContentSize(), 
				item->getParent()->convertToWorldSpace(item->getPosition()), 
				LFStrings::getValue("chuanqifuben_SuiPianHecheng"), 
				NULL, 
				false);
			mFooterLayer->getFooter()->addChild(guideLayer, 1024);
		}
	}
	else if (g_ChuanQiSuiPianHeCheng_GuideEnd)
	{
		ChatLayer *layer = ChatLayer::create(
			ChatContentManager::sharedChatContentManager()->getGuideChatContent("ChuanQiFuBen_ZhenZhang_SongChuanQi"), this);
		if(layer != NULL)
			mFooterLayer->getFooter()->addChild(layer, 1024, TAG_ChatLayer);
	}
	else
	{
		bool isTaqnchu = false;
		if (History::getHistory()->getBoolFromCurrentHero("First_DisplayChuanQiEquip",isTaqnchu))
		{
			bool isMaoxianEnded = false;
			if(!History::getHistory()->getBoolFromCurrentHero(Key_First_ChuanChuanqi, isMaoxianEnded))
			{
				History::getHistory()->putInCurrentHero(Key_First_ChuanChuanqi,false);


				
				m_bChuanqiTuJian = true;
				ChatLayer *layer = ChatLayer::create(
					ChatContentManager::sharedChatContentManager()->getGuideChatContent("ChuanQi_TuJianChat"), this);
				mFooterLayer->getFooter()->addChild(layer, 1024, TAG_ChatLayer);
			}
		}
	}
	*/

	//暂时不用
// 	else if (isUI_Guide_GongJiang && g_gongjiang_BsXq_Opend)
// 	{
// 		ChatLayer *layer = ChatLayer::create(
// 			ChatContentManager::sharedChatContentManager()->getGuideChatContent("gongjiang_xiangQianBaoShi"), this);
// 		mFooterLayer->getFooter()->addChild(layer, 1024, TAG_ChatLayer);
// 	}
// 	else if (isUI_Guide_GongJiang && g_gongjiang_BsHc_Opend)
// 	{
// 		ChatLayer *layer = ChatLayer::create(
// 			ChatContentManager::sharedChatContentManager()->getGuideChatContent("gongjiang_baoShiHeCheng"), this);
// 		mFooterLayer->getFooter()->addChild(layer, 1024, TAG_ChatLayer);
// 	}



/*	//酒馆出新助手 ---> 和工匠闪烁一样的效果   6、10级
	//int 类型    =6 6级提示过了   =10 10级提示过了
	int lv = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level;
	if (lv >= 6)
	{
		int newJiuGuan = 0;
		if(History::getHistory()->getIntFromCurrentHero(Key_JiuGuanZhuShou, newJiuGuan))
		{
			if ((newJiuGuan < 6 && lv >= 6 && lv <= 10)
				|| (newJiuGuan == 6 && lv >= 10))
			{
				CCMenuItemSprite *jiuguanItem = (CCMenuItemSprite*)mMainMenu->getChildByTag(TAG_ITEM_JiuGuan);
				jiuguanItem->getNormalImage()->runAction(CCRepeatForever::create(
					CCSequence::create(CCFadeIn::create(0.5), CCFadeOut::create(0.5), NULL)));

				BaseSprite *tishiImg = BaseSprite::create("gongjiang_duanzao_xinjiesuo.png");
//				tishiImg->setFlipX(true);
				jiuguanItem->addChild(tishiImg, 0, Tag_Img_GongJiang_Xin);
				tishiImg->setPosition(ccp(jiuguanItem->getContentSize().width*0.4, jiuguanItem->getContentSize().height*0.6));
			}
		}
		else
		{
			if (newJiuGuan < 6 && lv >= 6 && lv <= 10)
			{
				CCMenuItemSprite *jiuguanItem = (CCMenuItemSprite*)mMainMenu->getChildByTag(TAG_ITEM_JiuGuan);
				jiuguanItem->getNormalImage()->runAction(CCRepeatForever::create(
					CCSequence::create(CCFadeIn::create(0.5), CCFadeOut::create(0.5), NULL)));

				BaseSprite *tishiImg = BaseSprite::create("gongjiang_duanzao_xinjiesuo.png");
//				tishiImg->setFlipX(true);
				jiuguanItem->addChild(tishiImg, 0, Tag_Img_GongJiang_Xin);
				tishiImg->setPosition(ccp(jiuguanItem->getContentSize().width*0.4, jiuguanItem->getContentSize().height*0.6));
			}
		}
	}*/

	/*
	int lv = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level;
	if (lv >= 5 && !m_bIsGongJiangJiaoXueIng)
	{
		bool newJiuGuan = false;
		
		if(!History::getHistory()->getBoolFromCurrentHero(Key_First_TuJian_TiShi, newJiuGuan))
		{
			m_bChuanqiJiaoXue = true;
			History::getHistory()->putInCurrentHero(Key_First_TuJian_TiShi, false);
			//
			ChatLayer *layer = ChatLayer::create(
				ChatContentManager::sharedChatContentManager()->getGuideChatContent("ChuanQi_Zhengsong"), this);
			mFooterLayer->getFooter()->addChild(layer, 1024, TAG_ChatLayer);
		}
	}
	*/
}

void  MainLayerRenWuBody::onEnter()
{
	BaseLayer::onEnter();
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
	//GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstComeInGame);
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstComeInGame))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstComeInGame);
	}
	//Show7DayShuoMing();
	play7DayLogin();
	teachGuide();
	
	/*
	*/
	//addObserver(callfuncO_selector(MainLayerRenWuBody::systemVersionCallBack), MSG_QueryNotifyInfoRsp);
	//CmdHelper::getHelper()->cmdQuerySystemNotifyInfo();
}
void MainLayerRenWuBody::play7DayLogin()
{
	//vector<Flat_BonusInfo> list = BonusManager::getManager()->getBonus(BonusType_DailyLogin);
	if (GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FenJieUnLock))
	{
		vector<Flat_BonusInfo> list = BonusManager::getManager()->getBonus(BonusType_DailyLogin);
		bool isShow7DayLogin = false;
		if (list.at(0).bonus_id == 1)
		{
			for (vector<Flat_BonusInfo>::iterator iter = list.begin() ; iter != list.end() ; iter ++)
			{
				if (iter->enable && !iter->is_get)
				{
					isShow7DayLogin = true;
				}
			}
		}
		if (isShow7DayLogin)
		{
			NetLogin7DayLayer *layer = NetLogin7DayLayer::create();

			if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_First7DayLogin))
			{
				layer->setCallBack(this,SEL_CallFunc(&MainLayerRenWuBody::Show7DayShuoMing));
			}
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer,1024);
		}
	}
	
}
void MainLayerRenWuBody::playJiuGuan()
{
	GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;
	StateBar* bar = dynamic_cast<StateBar*>(mFooterLayer->getHeader()->getHeaderContent());
	CCMenuItem* tujianItem = m_lMenuLayer->getJiuGuanItem();
	guideLayer2->init(tujianItem->getContentSize(), this->convertToWorldSpace(tujianItem->getParent()->convertToWorldSpace(tujianItem->getPosition()))
		, LFStrings::getValue(""), NULL, false);
	mFooterLayer->getFooter()->addChild(guideLayer2, 1024, TAG_ChatLayer);
}
void MainLayerRenWuBody::Show7DayShuoMing()
{
	m_bIs7DayLogin = true;
	vector<ShowMsg> list;
	ShowMsg msg1;
	/*msg1.msg = LFStrings::getValue("Jixianshoucheng_Shengli_Tip01");
	msg1.color = ccWHITE;
	list.push_back(msg1);
	ShowMsg msg2;
	msg2.msg = LFStrings::getValue("Jixianshoucheng_Shengli_Tip02");
	msg2.color = ccRED;
	list.push_back(msg2);

	ShowMsg msg3;
	msg3.msg = LFStrings::getValue("Test3");
	msg3.color = ccWHITE;
	list.push_back(msg3);

	ShowMsg msg4;
	msg4.msg = LFStrings::getValue("Test3");
	msg4.color = ccRED;
	list.push_back(msg4);

	ShowMsg msg5;
	msg5.msg = LFStrings::getValue("Test3");
	msg5.color = ccWHITE;
	list.push_back(msg5);
	*/
	
	msg1.msg = LFStrings::getValue("7DayShuoMing1");
	msg1.color = ccWHITE;
	list.push_back(msg1);

	ShowMsg msg2;
	msg2.msg = LFStrings::getValue("ChuanQiZhuangBei");
	msg2.color = ccc3(235,106,12);
	list.push_back(msg2);

	ShowMsg msg3;
	msg3.msg = LFStrings::getValue("7DayShuoMing2");
	msg3.color = ccWHITE;
	list.push_back(msg3);

	ShowMsg msg4;
	msg4.msg = LFStrings::getValue("ChuanQiZhuangBei");
	msg4.color = ccc3(235,106,12);
	list.push_back(msg4);

	ShowMsg msg5;
	msg5.msg = LFStrings::getValue("7DayShuoMing3");
	msg5.color = ccWHITE;
	list.push_back(msg5);
	
	ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
		list, 1,this);
	if(layer != NULL)
	{

		CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
	}
	return;
}
void  MainLayerRenWuBody::onExit()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	BaseLayer::onExit();
}

bool  MainLayerRenWuBody::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{

	CCPoint worldPos = pTouch->getLocation();

	vector<BaseActor*> vec = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
	//根据chessBoard排序，保证重叠的部分、点到的是zOrder大的
	for(int i=0; i!=vec.size(); ++i)
	{
		int iPos = vec.at(i)->getChessBoardPos();
		for (int j=i+1; j!=vec.size(); ++j)
		{
			int jPos = vec.at(j)->getChessBoardPos();
			if(iPos < jPos)
			{
				swap(vec.at(i), vec.at(j));
				iPos = jPos;
			}
		}
	}
	for(int i=0; i!=vec.size(); ++i)
	{
		int chessboardPos = vec.at(i)->getChessBoardPos();
		CCNode *node = mBg->getChildByTag(Tag_ActorFullPhoto_1 + chessboardPos);

		CCRect temp = node->getChildByTag(TAG_FULL_PHOTO_JIAZI)->boundingBox();
		CCRect photoRect(temp.getMidX()-temp.size.width*0.7/2, temp.getMidY()-temp.size.height*SCALE_FullPhoto/2
			, temp.size.width*0.7, temp.size.height*SCALE_FullPhoto);
		if(photoRect.containsPoint( node->convertToNodeSpace(worldPos) )  )
		{
			node->setScale(SCALE_FullPhoto + 0.15f);

			mTag_chessboardPos = chessboardPos;
			m_deltaPos = ccpSub(mBg->convertToNodeSpace(worldPos), node->getPosition());

			mBeginTouchPos = worldPos;
			mFullPhotoHasMoved = false;
			mHeroClickedId = ((BaseActor*)node->getUserData())->getId();

			return true;
		}
	}


	return false;
}

void  MainLayerRenWuBody::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint worldPos = pTouch->getLocation();

	CCPoint delta = ccpSub(mBeginTouchPos, worldPos);
	if(! (abs(delta.x)<16 && abs(delta.y)<16))
		mFullPhotoHasMoved = true;

	CCNode *node = mBg->getChildByTag(Tag_ActorFullPhoto_1 + mTag_chessboardPos);

	CCPoint desPos = ccpSub(mBg->convertToNodeSpace(worldPos), m_deltaPos);
	node->setPosition(desPos);

	mBg->reorderChild(node, 100);
}

void  MainLayerRenWuBody::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCNode *node = mBg->getChildByTag(Tag_ActorFullPhoto_1 + mTag_chessboardPos);
	node->setScale(SCALE_FullPhoto);

	CCPoint worldPos = pTouch->getLocation();

	if (! mFullPhotoHasMoved)
	{

		//人行面板
		ZhuangBeiMianBanBase* layer = ZhuangBeiMianBanBase::create(mHeroClickedId);
		mFooterLayer->setContaintLayer(layer);


		bool bShowYuanshenUI = false;
		if (node->getChildByTag(Tag_ITEM_ShengjiTishi))
		{
			if (MyselfManager::getManager()->getMyTeamActor(mHeroClickedId)->getMyYuanShen())
			{
				bShowYuanshenUI = true;
			}
			node->getChildByTag(Tag_ITEM_ShengjiTishi)->removeFromParent();
			std::map<int, bool>::iterator it;
			for (it = g_ShowShengjiTipSprInfo.begin(); it != g_ShowShengjiTipSprInfo.end(); ++it)
			{
				if (it->first == mHeroClickedId)
				{
					it->second = false;
					break;
				}
			}
		}
		if (bShowYuanshenUI)
		{
			vector<BaseActor*> mHeros = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
			CCArray *yuanshenActor = CCArray::create();
			for (int i=0; i!=mHeros.size(); ++i)
			{
				if (mHeros.at(i)->getMyYuanShen() != NULL)
					yuanshenActor->addObject(mHeros.at(i)->getMyYuanShen());
			}

			ZhuangBeiMianBanYuanShen *layer1 = ZhuangBeiMianBanYuanShen::create(yuanshenActor, MyselfManager::getManager()->getMyTeamActor(mHeroClickedId)->getMyYuanShen()->getId()
				, comefrom_base, layer);
			mFooterLayer->popUpContaintLayer(layer1);
		}
		return;
	}

	CCPoint curPos = ccpSub(mBg->convertToNodeSpace(worldPos), m_deltaPos);


	 // pos 0123
	if(curPos.x > 320-150 && curPos.x < 320 && curPos.y > mActorPos[0].y-68 && curPos.y < mActorPos[0].y+110)
	{
		CCNode *node = mBg->getChildByTag(Tag_ActorFullPhoto_1 + mTag_chessboardPos);
		node->setPosition(mActorPos[0]);

		CCNode *node2 = mBg->getChildByTag(Tag_ActorFullPhoto_1 + 1);
		if(node2 != NULL)
		{
			int chessboardPos = mTag_chessboardPos;
			node2->setPosition(mActorPos[chessboardPos-1]);
			mBg->reorderChild(node2, chessboardPos);
			node2->setTag(Tag_ActorFullPhoto_1 + chessboardPos);
		}

		node->setTag(Tag_ActorFullPhoto_1 + 1);
		mBg->reorderChild(node, 1);
		if(mTag_chessboardPos == 1)
			return;
	}
	else if (curPos.x > 320 && curPos.x < 320+150 && curPos.y > mActorPos[1].y-68 && curPos.y < mActorPos[1].y+110)
	{
		CCNode *node = mBg->getChildByTag(Tag_ActorFullPhoto_1 + mTag_chessboardPos);
		node->setPosition(mActorPos[1]);

		CCNode *node2 = mBg->getChildByTag(Tag_ActorFullPhoto_1 + 2);
		if(node2 != NULL)
		{
			int chessboardPos = mTag_chessboardPos;
			node2->setPosition(mActorPos[chessboardPos-1]);
			mBg->reorderChild(node2, chessboardPos);
			node2->setTag(Tag_ActorFullPhoto_1 + chessboardPos);
		}

		node->setTag(Tag_ActorFullPhoto_1 + 2);
		mBg->reorderChild(node, 2);
		if(mTag_chessboardPos == 2)
			return;
	}
	else if (curPos.x > 320-200 && curPos.x < 320 && curPos.y > mActorPos[2].y-50 && curPos.y < mActorPos[2].y+50)
	{
		CCNode *node = mBg->getChildByTag(Tag_ActorFullPhoto_1 + mTag_chessboardPos);
		node->setPosition(mActorPos[2]);

		CCNode *node2 = mBg->getChildByTag(Tag_ActorFullPhoto_1 + 3);
		if(node2 != NULL)
		{
			int chessboardPos = mTag_chessboardPos;
			node2->setPosition(mActorPos[chessboardPos-1]);
			mBg->reorderChild(node2, chessboardPos);
			node2->setTag(Tag_ActorFullPhoto_1 + chessboardPos);
		}

		node->setTag(Tag_ActorFullPhoto_1 + 3);
		mBg->reorderChild(node, 3);
		if(mTag_chessboardPos == 3)
			return;
	}
	else if (curPos.x > 320 && curPos.x < 320+200 && curPos.y > mActorPos[3].y-50 && curPos.y < mActorPos[3].y+50)
	{
		CCNode *node = mBg->getChildByTag(Tag_ActorFullPhoto_1 + mTag_chessboardPos);
		node->setPosition(mActorPos[3]);

		CCNode *node2 = mBg->getChildByTag(Tag_ActorFullPhoto_1 + 4);
		if(node2 != NULL)
		{
			int chessboardPos = mTag_chessboardPos;
			node2->setPosition(mActorPos[chessboardPos-1]);
			mBg->reorderChild(node2, chessboardPos);
			node2->setTag(Tag_ActorFullPhoto_1 + chessboardPos);
		}

		node->setTag(Tag_ActorFullPhoto_1 + 4);
		mBg->reorderChild(node, 4);
		if(mTag_chessboardPos == 4)
			return;
	}
	else
	{
		CCNode *node = mBg->getChildByTag(Tag_ActorFullPhoto_1 + mTag_chessboardPos);
		node->setPosition(mActorPos[mTag_chessboardPos-1]);
		mBg->reorderChild(node, mTag_chessboardPos);
		return;
	}


	/////////////////////////////发消息
	addObserver(SEL_CallFuncO(&MainLayerRenWuBody::buzhenCallBack),MSG_adjustFormationRsp);

	SPCmd_AdjustFormation req;
	for (int i=0; i<FormationHeroAcountMax; ++i)
	{
		req.hero_id_arr[i] = 0;
	}
	for(int i=0; i!=4; ++i)
	{
		CCNode *node = mBg->getChildByTag(Tag_ActorFullPhoto_1 + 1 + i);
		if (node != NULL)
		{
			req.hero_id_arr[i] = ((BaseActor*)node->getUserData())->getId();
		}
	}

	CmdHelper::getHelper()->cmdAdjustFormation(req.hero_id_arr);
}

void MainLayerRenWuBody::buzhenCallBack(CCObject* msg)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_adjustFormationRsp);

	GameDirector::getDirector()->mainScene(mainlayer_ZhuYe);
}

void MainLayerRenWuBody::huodongChaXun_Rsp(CCObject *f_msg)
{
// 	removeObserver(MSG_QuerySubTaskListRsp_F);
// 	GameDirector::getDirector()->hideWaiting();
// 
// 	Flat_QuerySubTaskListRsp* data = (Flat_QuerySubTaskListRsp*)((Flat_MSG_Rsp*)f_msg)->getData();
// 	if(data->err_code != Success)
// 	{
// 		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
// 		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
// 		return;
// 	}
// 
// 	HuoDong_SubTaskList *layer = HuoDong_SubTaskList::create(TaskManager::getManager()->getNotFinishedSubTaskList());
// 	mFooterLayer->getHeader()->setContent(BaseSprite::create());
// 	mFooterLayer->setContaintLayer(layer,false);
}

void MainLayerRenWuBody::chatEndCallBack()
{
/*	if (g_gongjiang_QhHt_Opend || g_gongjiang_BsXq_Opend || g_gongjiang_BsHc_Opend)*/

	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstComeInGame))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstComeInGame);
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstMaoXian);
	}
	
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstMaoXian))
	{
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstMaoXian);
		//bool isTeach = GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstMaoXian);
		//if (isTeach)
		{
			GameGuideManager::getManager()->setCurGuideState(1);
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCMenuItem* menuItem = mFooterLayer->getFooter()->getMaoXianMenu();
			guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
				, LFStrings::getValue("kaiqinidemaoxianlvcheng"), NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
			return;
		}
	}
	/*
	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_OpeningStory))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_OpeningStory);
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstMaoXian);
	}*/
	
	if (g_gongjiang_QhHt_Opend)
	{/*
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_GongJiang_QhHt))
		{
			//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstComeInGame);
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_GongJiang_QhHt);
		}
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = (CCMenuItem*)mMainMenu->getChildByTag(TAG_ITEM_GongJiang);
		guideLayer->init(
			CCSize(menuItem->getContentSize().width, menuItem->getContentSize().height)
			, menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue("gongjiang_gongJiangPu"), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);*/
	}
	/*else if (g_ChuanQiSuiPianHeCheng_GuideEnd)
	{
		g_ChuanQiSuiPianHeCheng_GuideEnd = false;
		//
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* menuItem = (CCMenuItem*)mMenu->getChildByTag(TAG_ITEM_XiTongJiangLi);
		guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue("chuanqiSuiPian_guideEnd"), NULL, false);

		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}
	else if (isUI_Guide_MainLine)
	{

		if (m_bChuanqiJiaoXue)
		{
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCMenuItem* menuItem = (CCMenuItem *)mMenu->getChildByTag(TAG_ITEM_DengLuJiangLi);
			guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
				, LFStrings::getValue("LingQuChuanqi"), NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
			m_bChuanqiJiaoXue = false;
		}
		else if (m_bIsFirstMaoxian)
		{
			m_bIsFirstMaoxian = false;
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCMenuItem* menuItem = mFooterLayer->getFooter()->getMaoXianMenu();
			guideLayer->init(menuItem->getContentSize(), menuItem->getParent()->convertToWorldSpace(menuItem->getPosition())
			, LFStrings::getValue("kaiqinidemaoxianlvcheng"), NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
		else if (m_bChuanqiTuJian)
		{
			
			GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;
			StateBar* bar = dynamic_cast<StateBar*>(mFooterLayer->getHeader()->getHeaderContent());
			CCMenuItem* tujianItem = m_lMenuLayer->getTujianItem();
			guideLayer2->init(tujianItem->getContentSize(), this->convertToWorldSpace(tujianItem->getParent()->convertToWorldSpace(tujianItem->getPosition()))
				, LFStrings::getValue("chaeckTuJian"), NULL, false);
			mFooterLayer->getFooter()->addChild(guideLayer2, 1024, TAG_ChatLayer);
		}
	}*/
	

}
void MainLayerRenWuBody::addTieJiang()
{
	float dt = 0.05f;
	m_sTieJiangSpr1 = CCSprite::create("zhuye_xiaoguo_tiejiang1.png"); 
	m_sTieJiangSpr1->setPosition(ccp(165,550));
	addChild(m_sTieJiangSpr1, 1);
	m_sTieJiangSpr1->setTag(1);
	m_sTieJiangSpr1->runAction(CCSequence::create(CCDelayTime::create(dt),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverTieJiang)),NULL));

	m_sTieJiangSpr2 = CCSprite::create("zhuye_xiaoguo_tiejiang2.png"); 
	m_sTieJiangSpr2->setPosition(ccp(165,550));
	addChild(m_sTieJiangSpr2, 2);
	m_sTieJiangSpr2->setVisible(false);
	m_sTieJiangSpr2->setTag(2);

	m_sTieJiangSpr3 = CCSprite::create("zhuye_xiaoguo_tiejiang3.png"); 
	m_sTieJiangSpr3->setPosition(ccp(165,550));
	addChild(m_sTieJiangSpr3, 3);
	m_sTieJiangSpr3->setVisible(false);
	m_sTieJiangSpr3->setTag(3);
}
void MainLayerRenWuBody::firstOverTieJiang(CCNode *node)
{
	float dt = 0.3f;
	node->setVisible(false);
	m_nFirstTieJiangCount ++;

	if (m_nFirstTieJiangCount < 6)
	{
		if (node->getTag() == 1)
		{
			m_sTieJiangSpr2->setVisible(true);
			m_sTieJiangSpr2->runAction(CCSequence::create(CCDelayTime::create(dt),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverTieJiang)),NULL));	
		}
		else
		{
			m_sTieJiangSpr1->setVisible(true);
			m_sTieJiangSpr1->runAction(CCSequence::create(CCDelayTime::create(dt),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverTieJiang)),NULL));
		}
	}
	else if (m_nFirstTieJiangCount == 6)
	{
		if (node->getTag() == 1)
		{
			m_sTieJiangSpr2->setVisible(true);
			m_sTieJiangSpr2->runAction(CCSequence::create(CCDelayTime::create(dt * 2.0f),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverTieJiang)),NULL));	
		}
		else
		{
			m_sTieJiangSpr1->setVisible(true);
			m_sTieJiangSpr1->runAction(CCSequence::create(CCDelayTime::create(dt * 1.5f),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverTieJiang)),NULL));
		}
	}
	else if (m_nFirstTieJiangCount == 7)
	{
		if (node->getTag() == 1)
		{
			m_sTieJiangSpr2->setVisible(true);
			m_sTieJiangSpr2->runAction(CCSequence::create(CCDelayTime::create(dt / 2),CCCallFunc::create(this,SEL_CallFunc(&MainLayerRenWuBody::pauseTieJiang)),NULL));	
		}
		else
		{
			m_sTieJiangSpr1->setVisible(true);
			m_sTieJiangSpr1->runAction(CCSequence::create(CCDelayTime::create(dt / 2),CCCallFunc::create(this,SEL_CallFunc(&MainLayerRenWuBody::pauseTieJiang)),NULL));
		}
	}
}
void MainLayerRenWuBody::pauseTieJiang()
{
	m_sTieJiangSpr1->setVisible(false);
	m_sTieJiangSpr2->setVisible(false);
	m_sTieJiangSpr3->setVisible(true);
	m_sTieJiangSpr3->runAction(CCSequence::create(CCDelayTime::create(3.0f),CCCallFunc::create(this,SEL_CallFunc(&MainLayerRenWuBody::refreshTieJiang)),NULL));
}
void MainLayerRenWuBody::refreshTieJiang()
{
	m_sTieJiangSpr3->setVisible(false);
	m_nFirstTieJiangCount = 0;
	m_sTieJiangSpr2->setVisible(true);
	m_sTieJiangSpr2->runAction(CCSequence::create(CCDelayTime::create(0.3f),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverTieJiang)),NULL));
}
void MainLayerRenWuBody::addShangren()
{
	float dt = 5.0f;
	m_sShangrenSpr1 = CCSprite::create("zhuye_xiaoguo_shangren1.png"); 
	m_sShangrenSpr1->setPosition(ccp(453,607));
	addChild(m_sShangrenSpr1, 1);
	m_sShangrenSpr1->setTag(1);
	m_sShangrenSpr1->runAction(CCSequence::create(CCDelayTime::create(dt),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverShangren)),NULL));

	m_sShangrenSpr2 = CCSprite::create("zhuye_xiaoguo_shangren3.png"); 
	m_sShangrenSpr2->setPosition(m_sShangrenSpr1->getPosition());
	addChild(m_sShangrenSpr2, 2);
	m_sShangrenSpr2->setVisible(false);
	m_sShangrenSpr2->setTag(2);

	m_sShangrenSpr3 = CCSprite::create("zhuye_xiaoguo_shangren2.png"); 
	m_sShangrenSpr3->setPosition(m_sShangrenSpr1->getPosition());
	addChild(m_sShangrenSpr3, 3);
	m_sShangrenSpr3->setVisible(false);
	m_sShangrenSpr3->setTag(3);
}
void MainLayerRenWuBody::firstOverShangren(CCNode *node)
{
	float dt = 0.3f;
	node->setVisible(false);
	m_nFirstShangrenCount ++;

	if (m_nFirstShangrenCount < 6)
	{
		if (node->getTag() == 1)
		{
			m_sShangrenSpr2->setVisible(true);
			m_sShangrenSpr2->runAction(CCSequence::create(CCDelayTime::create(dt),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverShangren)),NULL));	
		}
		else if (node->getTag() == 2)
		{
			m_sShangrenSpr3->setVisible(true);
			m_sShangrenSpr3->runAction(CCSequence::create(CCDelayTime::create(dt),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverShangren)),NULL));
		}
		else if (node->getTag() == 3)
		{
			m_sShangrenSpr2->setVisible(true);
			m_sShangrenSpr2->runAction(CCSequence::create(CCDelayTime::create(dt),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverShangren)),NULL));
		}
	}
	else if (m_nFirstShangrenCount == 6)
	{
		if (node->getTag() == 3)
		{
			m_sShangrenSpr2->setVisible(true);
			m_sShangrenSpr2->runAction(CCSequence::create(CCDelayTime::create(dt),CCCallFunc::create(this,SEL_CallFunc(&MainLayerRenWuBody::pauseShangren)),NULL));	
		}
		else
		{
			m_sShangrenSpr3->setVisible(true);
			m_sShangrenSpr3->runAction(CCSequence::create(CCDelayTime::create(dt),CCCallFunc::create(this,SEL_CallFunc(&MainLayerRenWuBody::pauseShangren)),NULL));
		}
	}
}
void MainLayerRenWuBody::pauseShangren()
{
	m_sShangrenSpr3->setVisible(false);
	m_sShangrenSpr2->setVisible(false);
	m_sShangrenSpr1->setVisible(true);
	m_sShangrenSpr1->runAction(CCSequence::create(CCDelayTime::create(5.0f),CCCallFunc::create(this,SEL_CallFunc(&MainLayerRenWuBody::refreshShangren)),NULL));
}
void MainLayerRenWuBody::refreshShangren()
{
	m_sShangrenSpr3->setVisible(false);
	m_sShangrenSpr1->setVisible(false);
	m_nFirstShangrenCount = 0;
	m_sShangrenSpr2->setVisible(true);
	m_sShangrenSpr2->runAction(CCSequence::create(CCDelayTime::create(0.3f),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::firstOverShangren)),NULL));
}
void MainLayerRenWuBody::addCloud()
{
	float dt = 30.0f;
	CCSprite *cloudSpr1 = CCSprite::create("zhuye_xiaoguo_yun1.png"); 
	cloudSpr1->setAnchorPoint(ccp(1,0.5f));
	cloudSpr1->setPosition(ccp(0,914));
	addChild(cloudSpr1, 1);
	cloudSpr1->setTag(1);
	cloudSpr1->runAction(CCSequence::create(CCDelayTime::create(CCRANDOM_0_1()),CCMoveBy::create(dt,ccp(this->getContentSize().width  + cloudSpr1->getContentSize().width, 0)),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::refreshCloud)),NULL));

	CCSprite *cloudSpr2 = CCSprite::create("zhuye_xiaoguo_yun2.png"); 
	cloudSpr2->setAnchorPoint(ccp(1,0.5f));
	cloudSpr2->setPosition(ccp(0,cloudSpr1->getPositionY() + 90));
	addChild(cloudSpr2, 1);
	cloudSpr2->setTag(2);
	cloudSpr2->runAction(CCSequence::create(CCDelayTime::create(CCRANDOM_0_1() * 2),CCMoveBy::create(dt * 2,ccp(this->getContentSize().width + cloudSpr2->getContentSize().width, 0)),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::refreshCloud)),NULL));

	CCSprite *cloudSpr3 = CCSprite::create("zhuye_xiaoguo_yun2.png"); 
	cloudSpr3->setAnchorPoint(ccp(1,0.5f));
	cloudSpr3->setPosition(ccp(0,cloudSpr1->getPositionY() + 140));
	addChild(cloudSpr3, 1);
	cloudSpr3->setTag(3);
	cloudSpr3->runAction(CCSequence::create(CCDelayTime::create(CCRANDOM_0_1() * 3),CCMoveBy::create(dt * 3,ccp(this->getContentSize().width + cloudSpr3->getContentSize().width, 0)),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::refreshCloud)),NULL));
}
void MainLayerRenWuBody::refreshCloud(CCNode *node)
{
	int x = this->getContentSize().width  + node->getContentSize().width;
	float dt = 30.0f;
	node->setPositionX(0);
	node->runAction(CCSequence::create(CCDelayTime::create(CCRANDOM_0_1() * node->getTag()),CCMoveBy::create(dt * node->getTag(),ccp(x, 0)),CCCallFuncN::create(this,SEL_CallFuncN(&MainLayerRenWuBody::refreshCloud)),NULL));
}
void MainLayerRenWuBody::addRain()
{
	unsigned int now_time = ServerTime::getTime();

	now_time /= (10 * 60) ;

	int last_time = 0;
	last_time = CCUserDefault::sharedUserDefault()->getIntegerForKey("UserRainTime",0);
	bool isRain = false;
	isRain = CCUserDefault::sharedUserDefault()->getBoolForKey("PlayRain",false);
	CCSprite *rainSpr = NULL;
	if (isRain)
	{
		CCAnimation* pAnimation = CCAnimation::create();
		for (int i = 1; i <= 4; i++)
		{
			pAnimation->addSpriteFrameWithFileName(CCString::createWithFormat("zhuye_xiaoguo_xiayu%d.png", i)->getCString());
		}
		pAnimation->setDelayPerUnit(2.0f / 30.0f);    // 必须设置这个，要不就不会播放
		pAnimation->setRestoreOriginalFrame(true);
		pAnimation->setLoops(-1);

		CCAnimate* pAnimate = CCAnimate::create(pAnimation);

		CCSprite* pSprite2 = CCSprite::create();
		mBg->addChild(pSprite2, 1);
		pSprite2->setPosition(ccp(mBg->getContentSize().width / 2, mBg->getContentSize().height / 2));
		pSprite2->runAction(pAnimate);
		rainSpr = pSprite2;
	}
	if (last_time != 0)
	{
		if (now_time - last_time < 1)
		{
			return;
		}
	}
	CCUserDefault::sharedUserDefault()->setIntegerForKey("UserRainTime",now_time);
	CCUserDefault::sharedUserDefault()->flush();
	if (CCRANDOM_0_1() * 100 <= 50.0f)
	{
		if (isRain)
		{
			rainSpr->stopAllActions();
			rainSpr->removeFromParent();
		}
		CCUserDefault::sharedUserDefault()->setBoolForKey("PlayRain",false);
		CCUserDefault::sharedUserDefault()->flush();
		return;
	}
	CCUserDefault::sharedUserDefault()->setBoolForKey("PlayRain",true);
	CCUserDefault::sharedUserDefault()->flush();
	if (!isRain)
	{
		CCAnimation* pAnimation = CCAnimation::create();
		for (int i = 1; i <= 4; i++)
		{
			pAnimation->addSpriteFrameWithFileName(CCString::createWithFormat("zhuye_xiaoguo_xiayu%d.png", i)->getCString());
		}
		pAnimation->setDelayPerUnit(2.0f / 30.0f);    // 必须设置这个，要不就不会播放
		pAnimation->setRestoreOriginalFrame(true);
		pAnimation->setLoops(-1);

		CCAnimate* pAnimate = CCAnimate::create(pAnimation);

		CCSprite* pSprite2 = CCSprite::create();
		mBg->addChild(pSprite2, 1);
		pSprite2->setPosition(ccp(mBg->getContentSize().width / 2, mBg->getContentSize().height / 2));
		pSprite2->runAction(pAnimate);
	}
	//下雨效果  
	/*CCParticleSystem* particleSystem = CCParticleRain::create();  
	particleSystem->setTexture(CCTextureCache::sharedTextureCache()->addImage("rain.jpg"));  
	mBg->addChild(particleSystem); */
}

//邮件提示
void MainLayerRenWuBody::addEmailInfo(CCObject *obj){


	removeObserver(MSG_QueryEmailRsp);
	GameDirector::getDirector()->hideWaiting();
	
	int email_num = 0;
	const vector<Email>* list = EmailManager::getManager()->getAllEmail();
	for (vector<Email>::const_iterator iter = list->begin() ; iter != list->end() ; iter ++)
	{
		if (!iter->is_read)
		{
			email_num ++;
		}
	}
	if (email_num <= 0)
	{
		return;
	}
	CCSprite *numSpr = CCSprite::create("sphc_shuliang_bg.png");
	numSpr->setPosition(ccp(m_mItemEmail->getContentSize().width - numSpr->getContentSize().width / 2,m_mItemEmail->getContentSize().height -(numSpr->getContentSize().height-30) / 2));
	m_mItemEmail->addChild(numSpr);

	CCLabelTTF *ttf = CCLabelTTF::create(CCString::createWithFormat("%d",email_num)->getCString(),fontStr_kaiti,20.0f);
	ttf->setPosition(ccp(numSpr->getContentSize().width / 2, numSpr->getContentSize().height / 2));
	numSpr->addChild(ttf);

	numSpr->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(0.5f,1.0f,0.7f),CCScaleTo::create(0.3f,1.0f,1.0f),NULL)));





}




void MainLayerRenWuBody::playLei()
{
	Music::sharedMusicPlayer()->playEffectSound("thunder1.wav",false);
}
void MainLayerRenWuBody::ZhuShouchatEndCallBack()
{
	if (m_bIsFirstBaoShi)
	{
		CCMenuItem *item = (CCMenuItem *)mMainMenu->getChildByTag(TAG_ITEM_GongJiang);
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		if (item)
		{
			item->setEnabled(true);
			CCSize size = item->getContentSize();
			guideLayer->init(CCSize(size), item->getParent()->convertToWorldSpace(item->getPosition())
				, LFStrings::getValue(""), NULL, false);
			mFooterLayer->getFooter()->addChild(guideLayer, 1024);
		}
		m_bIsFirstBaoShi = false;
		return;
	}
	if (!m_bIs7DayLogin)
	{
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		if (m_node)
		{
			CCSize size = m_node->getChildByTag(TAG_FULL_PHOTO_JIAZI)->getContentSize();
			guideLayer->init(CCSize(size.width/2, size.height/3*2), m_node->getParent()->convertToWorldSpace(ccp(m_node->getPositionX(), m_node->getPositionY()+20))
				, LFStrings::getValue("dianjirenwugenghuanzhuangbei"), NULL, false);
			mFooterLayer->getFooter()->addChild(guideLayer, 1024);
		}
	}
}
void MainLayerRenWuBody::UnLockXunBao()
{
	CCMenuItem *item = (CCMenuItem *)mMenu->getChildByTag(TAG_ITEM_TanSuo);
	if (item)
	{

		CCPoint pos = item->getPosition();
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		item->setPosition(ccp(size.width / 2,size.height / 2 - 105));
		item->setVisible(true);
		item->runAction(CCSequence::create(CCScaleTo::create(0.5f,1.2f),CCScaleTo::create(0.5f,1.0f),CCScaleTo::create(0.5f,1.2f),CCMoveTo::create(1.0f,pos),CCScaleTo::create(0.5f,1.0f),CCCallFunc::create(this,SEL_CallFunc(&MainLayerRenWuBody::unLockXunBaoOver)),NULL));
	}
}
void MainLayerRenWuBody::unLockXunBaoOver()
{
	CCMenuItem *item = (CCMenuItem *)mMenu->getChildByTag(TAG_ITEM_TanSuo);
	GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
	if (item)
	{
		item->setEnabled(true);
		CCSize size = item->getContentSize();
		guideLayer->init(CCSize(size), item->getParent()->convertToWorldSpace(item->getPosition())
			, LFStrings::getValue(""), NULL, false);
		mFooterLayer->getFooter()->addChild(guideLayer, 1024);
	}
}
void MainLayerRenWuBody::UnLockYingXiongBang()
{
	CCMenuItem *item = (CCMenuItem *)mMenu->getChildByTag(TAG_ITEM_YingXiongBang);
	if (item)
	{

		CCPoint pos = item->getPosition();
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		item->setPosition(ccp(size.width / 2,size.height / 2 - 105));
		item->setVisible(true);
		item->runAction(CCSequence::create(CCScaleTo::create(0.5f,1.2f),CCScaleTo::create(0.5f,1.0f),CCScaleTo::create(0.5f,1.2f),CCMoveTo::create(1.0f,pos),CCScaleTo::create(0.5f,1.0f),CCCallFunc::create(this,SEL_CallFunc(&MainLayerRenWuBody::unLockYingXiongBangOver)),NULL));
	}
}
void MainLayerRenWuBody::unLockYingXiongBangOver()
{
	CCMenuItem *item = (CCMenuItem *)mMenu->getChildByTag(TAG_ITEM_YingXiongBang);
	GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
	if (item)
	{
		item->setEnabled(true);
		CCSize size = item->getContentSize();
		guideLayer->init(CCSize(size), item->getParent()->convertToWorldSpace(item->getPosition())
			, LFStrings::getValue(""), NULL, false);
		mFooterLayer->getFooter()->addChild(guideLayer, 1024);
	}
}