
#include "XueSeJiaoTangLayer.h"
#include "font.h"
#include "GameDirector.h"
#include "MyselfManager.h"
#include "LFAlert.h"
#include "XueSeJiaoTangFightPrepareLayer.h"
#include "XueZhanManager.h"
#include "MessageDisplay.h"
#include "flat.h"
#include "GuideLayerBase.h"
#include "UnblockManager.h"
enum 
{
	Tag_ContentLayer_1 = 1,
	Tag_ContentLayer_2 = 2,

	//content1
	Tag_MenuItem_FanHui = 10,
	Tag_MenuItem_PaiMing,
	Tag_MenuItem_ShuoMing,
	Tag_MenuItem_ChuangGuan,
	//content2
	Tag_content2_Menu,
	Tag_MenuItem_PuTong ,
	Tag_MenuItem_KunNan,
	Tag_MenuItem_DiYu,
	Tag_MenuItem_EMeng,
	//说明
	Tag_Board_ShuoMing,

};

XueSeJiaoTangLayer*  XueSeJiaoTangLayer::create()
 {
	 XueSeJiaoTangLayer *layer = new XueSeJiaoTangLayer;
	 if (layer && layer->init())
	 {
		 layer->autorelease();
	 }
	 else
	 {
		 CC_SAFE_DELETE(layer);
		 layer = NULL;
	 }

	 return layer;
 }

bool XueSeJiaoTangLayer::init()
{
	if( !BaseLayer::init() )
		return false;

	CCString temp;

	mBg = BaseSprite::create("xuesejiaotang_bg.png");
	addChild(mBg);
	mBg->setPosition(ccp(mWinSize.width/2, mBg->getContentSize().height/2));

	//1
	BaseSprite *contentLayer1 = BaseSprite::create();
	mBg->addChild(contentLayer1, 0, Tag_ContentLayer_1);
	contentLayer1->setPosition(ccp(0,0));
	{
		CCMenu *menu = CCMenu::create();
		contentLayer1->addChild(menu);
		menu->setPosition(ccp(0,0));
		//返回
		CCMenuItemImage *fanhui = CCMenuItemImage::create(
			"xuesejiaotang_anniu_chuangguan_tuichu.png"
			, "xuesejiaotang_anniu_chuangguan_tuichu_select.png"
			, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_MainMenu));
		menu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
		fanhui->setPosition(ccp(320, 860-792));
		//排名
		CCMenuItemImage *paiming = CCMenuItemImage::create(
			"xuesejiaotang_anniu_paiming.png"
			, "xuesejiaotang_anniu_paiming_select.png"
			, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_MainMenu));
		menu->addChild(paiming, 1, Tag_MenuItem_PaiMing);
		paiming->setPosition(ccp(110, 860-177));
		//说明
		CCMenuItemImage *shuoming = CCMenuItemImage::create(
			"xuesejiaotang_anniu_shuoming.png"
			, "xuesejiaotang_anniu_shuoming_select.png"
			, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_MainMenu));
		menu->addChild(shuoming, 1, Tag_MenuItem_ShuoMing);
		shuoming->setPosition(ccp(530, 860-177));
		//闯关
		CCMenuItemImage *chuangguan = CCMenuItemImage::create(
			"xuesejiaotang_anniu_chuangguan.png"
			, "xuesejiaotang_anniu_chuangguan_select.png"
			, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_MainMenu));
		menu->addChild(chuangguan, 1, Tag_MenuItem_ChuangGuan);
		chuangguan->setPosition(ccp(320, 860-467 - 30));
		//info bar
		BaseSprite *bar = BaseSprite::create("xuesejiaotang_nandu_kuang.png");
		contentLayer1->addChild(bar);
		bar->setPosition(ccp(320, 860-177));
		char buf[32];
		sprintf(buf, "NanDuLevel_%d", XueZhanManager::getManager()->getCurrentDifficult()+1);
		temp.initWithFormat("%s:", LFStrings::getValue("NanDu").c_str());
		CCLabelTTF *nanduLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 24);
		bar->addChild(nanduLabel);
		nanduLabel->setColor(fonColor_PuTong);
		nanduLabel->setPosition(ccp(50 + nanduLabel->getContentSize().width / 2, 70));

		CCLabelTTF *nanduLabel2 = CCLabelTTF::create(LFStrings::getValue(buf).c_str(), fontStr_kaiti, 24);
		bar->addChild(nanduLabel2);
		nanduLabel2->setColor(fonColor_FaGuang);
		nanduLabel2->setPosition(ccp(nanduLabel->getPositionX() + nanduLabel->getContentSize().width / 2 + nanduLabel2->getContentSize().width / 2, 70));
		
		temp.initWithFormat("%s%s:", LFStrings::getValue("jinri").c_str(), LFStrings::getValue("ChuangGuan").c_str());
		CCLabelTTF *chuangguanLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 24);
		chuangguanLabel->setColor(fonColor_PuTong);
		bar->addChild(chuangguanLabel);
		chuangguanLabel->setPosition(ccp(bar->getContentSize().width/2 - 14, 34));
		temp.initWithFormat("%u", XueZhanManager::getManager()->getPassedNum());
		CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 24);
		bar->addChild(numLabel);
		numLabel->setColor(fonColor_FaGuang);
		numLabel->setPosition(ccp(chuangguanLabel->getPositionX()+chuangguanLabel->getContentSize().width/2+numLabel->getContentSize().width/2
			, chuangguanLabel->getPositionY()));

		
		CCSprite *bgBar = CCSprite::create("xuesejiaotang_biaoti_bg.png");
		bgBar->setPosition(ccp(mBg->getContentSize().width / 2,mBg->getContentSize().height / 2 + 60));
		mBg->addChild(bgBar);
		
		//继续闯关 增加60能量提示
		if (XueZhanManager::getManager()->powerWillAddIfChlg() > 0)
		{
			CCLabelTTF *tishiLabel = CCLabelTTF::create(LFStrings::getValue("JiXuChuangGuanZengJiaZhuFu_").c_str(), fontStr_kaiti, 24);
			bgBar->addChild(tishiLabel);
			tishiLabel->setColor(fonColor_FaGuang);
			tishiLabel->setPosition(ccp(bgBar->getContentSize().width / 2,bgBar->getContentSize().height / 2));
			temp.initWithFormat("%u", XueZhanManager::getManager()->powerWillAddIfChlg());
			CCLabelTTF *addNLLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 24);
			tishiLabel->addChild(addNLLabel);
			addNLLabel->setPosition(ccp(351, tishiLabel->getContentSize().height/2));

		}
		else
		{
			CCLabelTTF *tishi1 = CCLabelTTF::create(LFStrings::getValue("XueSeJiaoTangZhu1").c_str(),fontStr_kaiti,24.0f);
			tishi1->setPosition(ccp(bgBar->getContentSize().width / 2,bgBar->getContentSize().height / 2));
			bgBar->addChild(tishi1);
			tishi1->setColor(fonColor_FaGuang);
		}
		//剩余次数
		temp.initWithFormat("%s:", LFStrings::getValue("shengyucishu").c_str());
		CCLabelTTF *sycsLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 24);
		contentLayer1->addChild(sycsLabel);
		sycsLabel->setColor(fonColor_PuTong);
		sycsLabel->setPosition(ccp(320, 860-537 - 40));
		temp.initWithFormat("%u", XueZhanManager::getManager()->getCanChlgNum());
		CCLabelTTF *sycsNumLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 24);
		contentLayer1->addChild(sycsNumLabel);
		sycsNumLabel->setPosition(ccp(sycsLabel->getPositionX()+sycsLabel->getContentSize().width/2+sycsNumLabel->getContentSize().width/2,
			sycsLabel->getPositionY()));
	}

	//2
	BaseSprite *contentLayer2 = BaseSprite::create("xuesejiaotang_paiming_bg.png");
	mBg->addChild(contentLayer2, 0, Tag_ContentLayer_2);
	contentLayer2->setPosition(ccp(320, 860-458));
	contentLayer2->setVisible(false);
	{
		mTableView = CCTableView::create(this, CCSizeMake(602, 370));
		contentLayer2->addChild(mTableView, 0);
		mTableView->setPosition(ccp(8, 20));
		mTableView->setDelegate(this);
		mTableView->setDirection(kCCScrollViewDirectionVertical);
		mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
		//menu
		CCMenu *menu = CCMenu::create();
		contentLayer2->addChild(menu, 0, Tag_content2_Menu);
		menu->setPosition(ccp(0,0));

		//返回
		CCMenuItemImage *fanhui = CCMenuItemImage::create(
			"shuxing_renwu_fanhui.png"
			, "shuxing_renwu_fanhui_select.png"
			, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_PaiMingMenu));
		menu->addChild(fanhui, 1, Tag_MenuItem_FanHui);
		fanhui->setPosition(ccp(309, -112));
		//putong
		CCMenuItemImage *putong = CCMenuItemImage::create(
			"xuesejiaotang_shuoming_nandu_1.png"
			, "xuesejiaotang_shuoming_nandu_1_select.png"
			, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_PaiMingMenu));
		menu->addChild(putong, 1, Tag_MenuItem_PuTong);
		putong->setPosition(ccp(130, 510));
		//kunnan
		CCMenuItemImage *kunnan = CCMenuItemImage::create(
			"xuesejiaotang_shuoming_nandu_2.png"
			, "xuesejiaotang_shuoming_nandu_2_select.png"
			, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_PaiMingMenu));
		menu->addChild(kunnan, 1, Tag_MenuItem_KunNan);
		kunnan->setPosition(ccp(250, 510));
		//diyu
		CCMenuItemImage *diyu = CCMenuItemImage::create(
			"xuesejiaotang_shuoming_nandu_3.png"
			, "xuesejiaotang_shuoming_nandu_3_select.png"
			, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_PaiMingMenu));
		menu->addChild(diyu, 1, Tag_MenuItem_DiYu);
		diyu->setPosition(ccp(370, 510));
		//emeng
		CCMenuItemImage *emeng = CCMenuItemImage::create(
			"xuesejiaotang_shuoming_nandu_4.png"
			, "xuesejiaotang_shuoming_nandu_4_select.png"
			, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_PaiMingMenu));
		menu->addChild(emeng, 1, Tag_MenuItem_EMeng);
		emeng->setPosition(ccp(490, 510));
	}

	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_FirstXueSeJiaoTangPlay))
	{
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstXueSeJiaoTangPlay);
		//XueSeJiaoTangShuoMing
		vector<ShowMsg> list;
		ShowMsg msg;
		msg.msg = LFStrings::getValue("XueSeJiaoTangShuoMing");
		msg.color = fonColor_PuTong;

		list.push_back(msg);

		ZhuShouChatLayer* layer = ZhuShouChatLayer::create(
			list, 1,this);
		if(layer != NULL)
		{
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1024, TAG_ZhuShouChatLayer);
		}
	}

	
	return true;
}

int XueSeJiaoTangLayer::getCurrentDifficult()
{
	int level = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().level;

	int difficult = 0;
	//普通难度——需要等级：11级
	if (level >= 11 && level < 17) difficult = 1;
	else if (level >= 17 && level < 22) difficult = 2;
	else if (level >= 22 && level < 28) difficult = 3;
	else if (level >= 28) difficult = 4;

	return difficult;
}

CCSize XueSeJiaoTangLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSize(602, 68);
}

unsigned int XueSeJiaoTangLayer::numberOfCellsInTableView(CCTableView *table)
{
	return mOrder.size();
}

CCTableViewCell* XueSeJiaoTangLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();
	CCSize size = tableCellSizeForIndex(table, idx);
	CCString temp;

	if (cell != NULL)
	{	
		cell->removeAllChildren();
	}
	else 
	{
		cell = new CCTableViewCell();
		cell->autorelease();
	}

	if (table != mTableView) return cell;
	

	BaseSprite *board = BaseSprite::create("xuesejiaotang_paiming_kuang.png");
	cell->addChild(board);
	board->setPosition(ccp(size.width/2, size.height/2));

	ccColor3B color = fonColor_FaGuang;
	if (mOrder.at(idx).hero_info.hero_id == MyselfManager::getManager()->getMyZhuJueData()->getId())
		color = fonColor_ChengGong;
	// 排名       
	temp.initWithFormat("%u", mOrder.at(idx).order);
	CCLabelTTF *pmLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 24);
	board->addChild(pmLabel);
	pmLabel->setPosition(ccp(18, 22));
	pmLabel->setColor(color);
	//名字
	CCLabelTTF *nameLabel = CCLabelTTF::create(mOrder.at(idx).hero_info.name, fontStr_kaiti, 24);
	board->addChild(nameLabel);
	nameLabel->setAnchorPoint(ccp(0, 0.5));
	nameLabel->setPosition(ccp(40,22));
	nameLabel->setColor(color);
	//战绩
	temp.initWithFormat("%s:%u%s", LFStrings::getValue("ZhanJi").c_str(), mOrder.at(idx).checkpoints, LFStrings::getValue("guan").c_str());
	CCLabelTTF *zjLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 24);
	board->addChild(zjLabel);
	zjLabel->setColor(color);
	zjLabel->setAnchorPoint(ccp(0,0.5));
	zjLabel->setPosition(ccp(208, 22));
	//奖励
	temp.initWithFormat("%s:", LFStrings::getValue("jiangli").c_str());
	CCLabelTTF *jlLabel = CCLabelTTF::create(temp.getCString(), fontStr_kaiti, 24);
	board->addChild(jlLabel);
	jlLabel->setColor(color);
	jlLabel->setAnchorPoint(ccp(0,0.5));
	jlLabel->setPosition(ccp(326, 22));
	if (mOrder.at(idx).item.type == ItemsType_Gold)
	{//gold
		BaseSprite *icon = BaseSprite::create("jinbi_tubiao.png");
		board->addChild(icon);
		icon->setPosition(ccp(386, 22));
		icon->setScale(0.8f);
		temp.initWithFormat("%d", mOrder.at(idx).item.count);
		CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 24);
		board->addChild(numLabel);
		numLabel->setAnchorPoint(ccp(0,0.5));
		numLabel->setPosition(ccp(icon->getPositionX()+icon->getContentSize().width/2, icon->getPositionY()));
	}
	if (mOrder.at(idx).item.type == ItemsType_Silver)
	{//silver
		BaseSprite *icon = BaseSprite::create("yinbi_tubiao.png");
		board->addChild(icon);
		//icon->setPosition(ccp(469, 22));
		icon->setPosition(ccp(386, 22));
		icon->setScale(0.8f);
		temp.initWithFormat("%d", 0);
		CCLabelTTF *numLabel = CCLabelTTF::create(temp.getCString(), fontStr_BookAntiqua, 24);
		board->addChild(numLabel);
		numLabel->setAnchorPoint(ccp(0,0.5));
		numLabel->setPosition(ccp(icon->getPositionX()+icon->getContentSize().width/2, icon->getPositionY()));
	}

	return cell;
}

void XueSeJiaoTangLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_FanHui)
	{
		GameDirector::getDirector()->mainScene(mainlayer_HuoDong);
		return;
	}
	else if (tag == Tag_MenuItem_PaiMing)
	{
		BaseSprite *contentlayer1 = (BaseSprite*)mBg->getChildByTag(Tag_ContentLayer_1);
		BaseSprite *contentlayer2 = (BaseSprite*)mBg->getChildByTag(Tag_ContentLayer_2);
		contentlayer1->setVisible(false);
		contentlayer2->setVisible(true);
		CCMenu *menu = (CCMenu*)contentlayer2->getChildByTag(Tag_content2_Menu);
		if (menu)
		{
			int dif = getCurrentDifficult();
			if (dif >= 1 && dif <= 4)
			{
				menuItemClicked_PaiMingMenu(menu->getChildByTag(Tag_MenuItem_PuTong - 1 + dif));
			}
		}
	}
	else if (tag == Tag_MenuItem_ShuoMing)
	{
		//弹框
		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
		dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
		//确认 取消按键  
		CCMenu* menu = CCMenu::create();
		menu->setPosition(CCPointZero);
		{
			//返回
			CCMenuItemImage *fanhui = CCMenuItemImage::create(
				"shuxing_renwu_fanhui.png"
				, "shuxing_renwu_fanhui_select.png"
				, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_ShuoMingMenu));
			menu->addChild(fanhui, 1, 20);
			fanhui->setPosition(ccp(dialog->getContentSize().width/2,70));
		}
		//content
		{
			CCSize size = dialog->getContentSize();
			//title
			CCLabelTTF *titleLabel = CCLabelTTF::create(LFStrings::getValue("xuesejiaotang_JinRuDengJiTiShi").c_str(), fontStr_kaiti, 24,
				CCSize(400, 0), kCCTextAlignmentLeft);
			dialog->addChild(titleLabel);
			titleLabel->setAnchorPoint(ccp(0.5,1));
			titleLabel->setPosition(ccp(size.width/2, size.height-56));
			CCLabelTTF *shuoMingLabel = CCLabelTTF::create(LFStrings::getValue("xuesejiaotang_DengJiShuoMing").c_str(), fontStr_kaiti, 24);
			dialog->addChild(shuoMingLabel);
			shuoMingLabel->setColor(fonColor_PuTong);
			shuoMingLabel->setAnchorPoint(ccp(0.5,1));
			shuoMingLabel->setPosition(ccp(size.width/2, size.height-127));
		}
		//
		{
			LFAlert* lfAlert = new LFAlert();
			lfAlert->setTag(Tag_Board_ShuoMing);
			lfAlert->setBaseContainer(dialog,menu);
			CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_Board_ShuoMing);
			lfAlert->release();
		}
	}
	else if (tag == Tag_MenuItem_ChuangGuan)
	{
		if (XueZhanManager::getManager()->getCanChlgNum() <= 0)
		{
			UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("CEC_XueZhanChlgTimeOver"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}

		XueSeJiaoTangFightPrepareLayer *layer = new XueSeJiaoTangFightPrepareLayer();
		CCScene *scene = CCScene::create();
		scene->addChild(layer);
		layer->release();
		CCDirector::sharedDirector()->replaceScene(scene);
	}
}

void XueSeJiaoTangLayer::menuItemClicked_ShuoMingMenu(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Board_ShuoMing);
}

void XueSeJiaoTangLayer::menuItemClicked_PaiMingMenu(CCObject *pSender)
{
	int tag = ((CCMenuItem*)pSender)->getTag();

	if (tag == Tag_MenuItem_FanHui)
	{
		BaseSprite *contentlayer1 = (BaseSprite*)mBg->getChildByTag(Tag_ContentLayer_1);
		BaseSprite *contentlayer2 = (BaseSprite*)mBg->getChildByTag(Tag_ContentLayer_2);
		contentlayer1->setVisible(true);
		contentlayer2->setVisible(false);
	}
	else
	{
		CCMenu *menu = (CCMenu*)((CCMenuItem*)pSender)->getParent();
		char buf1[128], buf2[128];
		for (int i=0; i<4 && menu; ++i)
		{
			CCMenuItemImage *item = (CCMenuItemImage*)menu->getChildByTag(Tag_MenuItem_PuTong + i);
			if (item)
			{
				sprintf(buf1, "xuesejiaotang_shuoming_nandu_%d.png", i+1);
				sprintf(buf2, "xuesejiaotang_shuoming_nandu_%d_select.png", i+1);
				if (tag == Tag_MenuItem_PuTong + i)
					item->initWithNormalImage(buf2, buf1, NULL, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_PaiMingMenu));
				else 
					item->initWithNormalImage(buf1, buf2, NULL, this, menu_selector(XueSeJiaoTangLayer::menuItemClicked_PaiMingMenu));
			}
		}
		//
		addObserver(callfuncO_selector(XueSeJiaoTangLayer::rsp_queryPaiMing), MSG_querySelfChallengeOrderRsp_F);
		mCmdHlp->cmdQuerySelfChallengeOrder((Difficult)(tag - Tag_MenuItem_PuTong));
	}
}


void XueSeJiaoTangLayer::rsp_queryPaiMing(CCObject *msg_f)
{
	removeObserver(MSG_querySelfChallengeOrderRsp_F);
	GameDirector::getDirector()->hideWaiting();

	Flat_QuerySelfChallengeOrderRsp* data = (Flat_QuerySelfChallengeOrderRsp*)((Flat_MSG_Rsp*)msg_f)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay *layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	mOrder = data->order_arr;

	mTableView->reloadData();
}
void XueSeJiaoTangLayer::ZhuShouchatEndCallBack()
{

}