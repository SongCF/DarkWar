#include "VipExplainLayer.h"
#include "font.h"
#include "ImageManager.h"
#include "LFStrings.h"
#include "GameDirector.h"
#include <string>
using std::string;

#define Label_Size 20
#define Label_Color_DanHuang ccc3(195, 145, 96)

enum 
{
	Tag_MainMenuItem_ChengWeiVIP = 1,

};

VipExplainLayer::~VipExplainLayer()
{

}

VipExplainLayer::VipExplainLayer()
{
	CCSize bodySize = getBodySize();

	//底
	BaseSprite *di = BaseSprite::create("renwuliebiao_down_bg.png");
	this->addChild(di, 10);
	di->setPosition(ccp(bodySize.width/2, di->getContentSize().height/2));
	//头
	BaseSprite *tou = BaseSprite::create("VIP_bg.png");
	this->addChild(tou, 10);
	tou->setPosition(ccp(bodySize.width/2, bodySize.height - tou->getContentSize().height/2));
	//body
	float delta_height = bodySize.height - di->getContentSize().height - tou->getContentSize().height;
	BaseSprite *body = BaseSprite::create("renwuliebiao_middle_bg.png");
	this->addChild(body);
	body->setPosition(ccp(bodySize.width/2, di->getContentSize().height + delta_height/2));
	body->setScaleY(delta_height / body->getContentSize().height);


	mTableView = CCTableView::create(this, CCSizeMake(618, delta_height));
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	mTableView->setPosition(ccp((bodySize.width-618)/2, di->getContentSize().height));
	mTableView->setDelegate(this);
	this->addChild(mTableView, 1);

	//menu
	mMainMenu = CCMenu::create();
	di->addChild(mMainMenu);
	mMainMenu->setPosition(CCPointZero);
	//menu item
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"VIP_anniu_chengweivip.png"
		, "VIP_anniu_chengweivip_select.png"
		, this, menu_selector(VipExplainLayer::menuItemClicked_MainMenu));
	mMainMenu->addChild(fanhui, 1, Tag_MainMenuItem_ChengWeiVIP);
	fanhui->setPosition(ccp(di->getContentSize().width/2, di->getContentSize().height/2));

}

void VipExplainLayer::disableAllTouchBegin()
{
	mMainMenu->setEnabled(false);
}

void VipExplainLayer::disableAllTouchEnd()
{
	mMainMenu->setEnabled(true);
}

CCSize VipExplainLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return getCellSizeByIdx(idx);
}

CCTableViewCell* VipExplainLayer::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	CCTableViewCell *cell = table->dequeueCell();
	if (cell != NULL)
	{
		cell->removeAllChildren();
	}
	else 
	{
		cell = new CCTableViewCell;
		cell->autorelease();
	}

	CCSize cellSize = tableCellSizeForIndex(table, idx);
	CCString temp;

	//board
	CCScale9Sprite *board = CCScale9Sprite::create("VIP_kuang_bg.png");
	cell->addChild(board);
	board->setPosition(ccp(cellSize.width/2, cellSize.height/2));
	board->setContentSize(cellSize);
	//title
	CCLabelTTF *titleLabel = CCLabelTTF::create(getCellTitleByIdx(idx).getCString(), fontStr_kaiti, Label_Size);
	cell->addChild(titleLabel, 1);
	titleLabel->setPosition(ccp(cellSize.width/2, cellSize.height-23));
	titleLabel->setColor(ImageManager::getManager()->getShowColor(ZhuangbeiColour_Xiyou, false, false));
	//content label
	CCLabelTTF *conLabel = CCLabelTTF::create(getCellContentStringByIdx(idx).getCString(), fontStr_kaiti, 22);
	cell->addChild(conLabel, 1);
	conLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
	conLabel->setAnchorPoint(ccp(0, 1));
	conLabel->setPosition(ccp(30, cellSize.height - 60));
	conLabel->setColor(ccc3(195, 145, 96));

	return cell;
}

unsigned int VipExplainLayer::numberOfCellsInTableView(CCTableView *table)
{
/*	return 14; //0-10级vip  3个至尊礼包*/
	return 11;
}

CCSize VipExplainLayer::getCellSizeByIdx(int idx)
{
// 	//(0)v0->2  (1)v1->4  (2)v2->6  (3)v3->7  (4)v4->8  (5)v5->7  
// 	//(6)z0->1  (7)v6->7  (8)z1->1  (9)v7->7  (10)z2->1  (11)v8->6  (12)v9->6  (13)v10->6
// 	int delta = 25;
// 	CCSize size(618, 129); //默认情况能放两排   ,如果是两排或一排 就不用改变大小了
// 	if (idx == 1) size.height += delta*2;
// 	if (idx == 2) size.height += delta*4;
// 	if (idx == 3) size.height += delta*5;
// 	if (idx == 4) size.height += delta*6;
// 	if (idx == 5) size.height += delta*5;
// 	if (idx == 7) size.height += delta*5;
// 	if (idx == 9) size.height += delta*5;
// 	if (idx == 11) size.height += delta*4;
// 	if (idx == 12) size.height += delta*4;
// 	if (idx == 13) size.height += delta*4;
// 	if (idx == 6 || idx == 8 || idx == 10) size.height -= delta;

	int delta = 24;
	CCSize size(618, 122); //默认情况能放两排 
	if (idx == 0) size.height += delta * 1; 
	else if (idx == 1) size.height += delta * 4;
	else if (idx == 3) size.height += delta * 6;
// 	if (idx == 2) size.height += delta*4;
// 	if (idx == 3) size.height += delta*4;
// 	if (idx == 4) size.height += delta*4;
// 	if (idx == 5) size.height += delta*4;
// 	if (idx == 6) size.height += delta*4;
// 	if (idx == 7) size.height += delta*4;
// 	if (idx == 8) size.height += delta*3;
// 	if (idx == 9) size.height += delta*3;
// 	if (idx == 10) size.height += delta*3;
	else size.height += delta * 5;

	return size;
}

CCString VipExplainLayer::getCellTitleByIdx(int idx)
{
	CCString temp;
	if (idx == 0)
		temp = "VIP0";
	else if (idx == 1)
		temp = "VIP1";
		//temp.initWithFormat("VIP1(%s6%s)", LFStrings::getValue("LeiJiChongZhi").c_str(), LFStrings::getValue("yuan").c_str());
	else if (idx == 2)
		temp = "VIP2";
		//temp.initWithFormat("VIP2(%s50%s)", LFStrings::getValue("LeiJiChongZhi").c_str(), LFStrings::getValue("yuan").c_str());
	else if (idx == 3)
		temp = "VIP3";
		//temp.initWithFormat("VIP3(%s200%s)", LFStrings::getValue("LeiJiChongZhi").c_str(), LFStrings::getValue("yuan").c_str());
	else if (idx == 4)
		temp = "VIP4";
		//temp.initWithFormat("VIP4(%s500%s)", LFStrings::getValue("LeiJiChongZhi").c_str(), LFStrings::getValue("yuan").c_str());
	else if (idx == 5)
		temp = "VIP5";
		//temp.initWithFormat("VIP5(%s1000%s)", LFStrings::getValue("LeiJiChongZhi").c_str(), LFStrings::getValue("yuan").c_str());
	else if (idx == 6)
		temp = "VIP6";
		//temp.initWithFormat("VIP6(%s3000%s)", LFStrings::getValue("LeiJiChongZhi").c_str(), LFStrings::getValue("yuan").c_str());
	else if (idx == 7)
		temp = "VIP7";
	else if (idx == 8)
		temp = "VIP8";
	else if (idx == 9)
		temp = "VIP9";
	else if (idx == 10)
		temp = "VIP10";


	return temp;
}

CCString VipExplainLayer::getCellContentStringByIdx(int idx)
{
	string str;
	CCString temp;
	char buf[128];

	if (idx == 0)
	{
		//>>每日可购买1次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 1);
		str = temp.getCString();
		str += "\n";
		//>>每日可扫荡副本3次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 3);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银3次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 3);
		str += temp.getCString();
	}
	else if (idx == 1)
	{
		//>>每日可购买2次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 2);
		str = temp.getCString();
		str += "\n";
		//>>每日可扫荡副本6次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 6);
		str += temp.getCString();
		str += "\n";
		//>>购买2次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 2);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银4次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 4);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限14
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 14);
		str += temp.getCString();
		str += "\n";
		//>>可领取VIP1礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 1);
		str += temp.getCString();
	}
	else if (idx == 2)
	{
		//>>每日可购买2次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 2);
		str = temp.getCString();
		str += "\n";
		//>>每日可扫荡副本10次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 10);
		str += temp.getCString();
		str += "\n";
		//>>购买4次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 4);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银8次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 8);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限16
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 16);
		str += temp.getCString();
		str += "\n";
// 		//>>可免费刷新悬赏任务2次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 2);
// 		str += temp.getCString();
// 		str += "\n";
// 		//>>每次刷副本时可跳过1格小怪
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenTiaoGuo").c_str());
// 		temp.initWithFormat(buf, 1);
// 		str += temp.getCString();
// 		str += "\n";
		//>>可领取VIP2礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 2);
		str += temp.getCString();
		str += "\n";
		//>>可享受VIP1所有功能
		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
		temp.initWithFormat(buf, 1);
		str += temp.getCString();
	}
	else if (idx == 3)
	{
		//>>每日可购买3次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 3);
		str = temp.getCString();
		str += "\n";
		//>>每日可扫荡副本15次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 15);
		str += temp.getCString();
		str += "\n";
		//>>购买6次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 6);
		str += temp.getCString();
		str += "\n";
		//>>可使用一键探索功能
		strcpy(buf, LFStrings::getValue("vipJieShao_YiJianTanSuo").c_str());
		temp.initWithFormat(buf);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银12次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 12);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限18
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 18);
		str += temp.getCString();
		str += "\n";
// 		//>>可免费刷新悬赏任务4次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 4);
// 		str += temp.getCString();
// 		str += "\n";
// 		//>>每次刷副本时可跳过2格小怪
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenTiaoGuo").c_str());
// 		temp.initWithFormat(buf, 2);
// 		str += temp.getCString();
// 		str += "\n";
		//>>可领取VIP3礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 3);
		str += temp.getCString();
		str += "\n";
// 		//>>开启荣誉快速兑换功能
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RongYuKuaiSuDuiHuan").c_str());
// 		temp.initWithFormat(buf);
// 		str += temp.getCString();
// 		str += "\n";
		//>>可享受VIP2所有功能
		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
		temp.initWithFormat(buf, 2);
		str += temp.getCString();
	}
	else if (idx == 4)
	{
		//>>每日可购买4次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 4);
		str = temp.getCString();
		str += "\n";
		//>>每日可扫荡副本15次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 15);
		str += temp.getCString();
		str += "\n";
		//>>购买8次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 8);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银20次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 20);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限16
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 20);
		str += temp.getCString();
		str += "\n";
// 		//>>可免费刷新悬赏任务4次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 4);
// 		str += temp.getCString();
// 		str += "\n";
// 		//>>每次刷副本时可跳过3格小怪
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenTiaoGuo").c_str());
// 		temp.initWithFormat(buf, 3);
// 		str += temp.getCString();
// 		str += "\n";
		//>> 可领取VIP4礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 4);
		str += temp.getCString();
		str += "\n";
		//>>可享受VIP3所有功能
		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
		temp.initWithFormat(buf, 3);
		str += temp.getCString();
		str += "\n";
// 		//>>可领取定制神兵套装单件(3金星强力武器)
// 		strcpy(buf, LFStrings::getValue("vipJieShao_LingQuShenBing").c_str());
// 		temp.initWithFormat(buf, LFStrings::getValue("wuqi").c_str());
// 		str += temp.getCString();
	}
	else if (idx == 5)
	{
		// 		>>每日可购买6次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 6);
		str = temp.getCString();
		str += "\n";
		// 			>>每日可扫荡副本30次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 30);
		str += temp.getCString();
		str += "\n";
		// 			>>购买10次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 10);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银30次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 30);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限22
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 22);
		str += temp.getCString();
		str += "\n";
// 		// 			>>可免费刷新悬赏任务6次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 6);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>每次刷副本时可跳过4格小怪
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenTiaoGuo").c_str());
// 		temp.initWithFormat(buf, 4);
// 		str += temp.getCString();
// 		str += "\n";
		// 			>> 可领取VIP5礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 5);
		str += temp.getCString();
		str += "\n";
		// 			>>可享受VIP4所有功能
		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
		temp.initWithFormat(buf, 4);
		str += temp.getCString();
	}
	else if (idx == 6)
	{
// 		//>>可领取定制神兵套装单件(3金星强力护甲)
// 		strcpy(buf, LFStrings::getValue("vipJieShao_LingQuShenBing").c_str());
// 		temp.initWithFormat(buf, LFStrings::getValue("huJia").c_str());
// 		str += temp.getCString();
		// 		>>每日可购买8次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 8);
		str = temp.getCString();
		str += "\n";
		// 			>>每日可扫荡副本40次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 40);
		str += temp.getCString();
		str += "\n";
		// 			>>购买15次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 15);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银40次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 40);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限24
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 24);
		str += temp.getCString();
		str += "\n";
// 		// 			>>可免费刷新悬赏任务8次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 8);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>每次刷副本时可跳过4格小怪
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenTiaoGuo").c_str());
// 		temp.initWithFormat(buf, 4);
// 		str += temp.getCString();
// 		str += "\n";
		// 			>> 可领取VIP6礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 6);
		str += temp.getCString();
		str += "\n";
		// 			>>可享受VIP5所有功能
		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
		temp.initWithFormat(buf, 5);
		str += temp.getCString();
	}
	else if (idx == 7)
	{
		// 		>>每日可购买10次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 10);
		str = temp.getCString();
		str += "\n";
		// 			>>每日可扫荡副本50次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 50);
		str += temp.getCString();
		str += "\n";
		// 			>>购买20次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 20);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银55次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 55);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限26
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 26);
		str += temp.getCString();
		str += "\n";
// 		// 			>>可免费刷新悬赏任务8次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 8);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>每次刷副本时可跳过4格小怪
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenTiaoGuo").c_str());
// 		temp.initWithFormat(buf, 4);
// 		str += temp.getCString();
// 		str += "\n";
		// 			>> 可领取VIP7礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 7);
		str += temp.getCString();
		str += "\n";
		// 			>>可享受VIP6所有功能
		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
		temp.initWithFormat(buf, 6);
		str += temp.getCString();
	}
	else if (idx == 8)
	{
// 		//>>可领取定制神兵套装单件(3金星强力首饰)
// 		strcpy(buf, LFStrings::getValue("vipJieShao_LingQuShenBing").c_str());
// 		temp.initWithFormat(buf, LFStrings::getValue("ShouShi").c_str());
// 		str += temp.getCString();
		// 		>>每日可购买12次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 12);
		str = temp.getCString();
		str += "\n";
		// 			>>每日可扫荡副本60次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 60);
		str += temp.getCString();
		str += "\n";
		// 			>>购买25次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 25);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银70次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 7);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限28
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 28);
		str += temp.getCString();
		str += "\n";
// 		// 			>>可免费刷新悬赏任务10次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 10);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>每次刷副本时可跳过4格小怪(直接BOSS战) 
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenTiaoGuo").c_str());
// 		temp.initWithFormat(buf, 4);
// 		str += temp.getCString();
// 		str += "\n";
		// 			>>可领取VIP8礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 8);
		str += temp.getCString();
		str += "\n";
		// 			>>可享受VIP7所有功能
		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
		temp.initWithFormat(buf, 7);
		str += temp.getCString();
	}
	else if (idx == 9)
	{
		// 		>>每日可购买15次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 15);
		str = temp.getCString();
		str += "\n";
		// 			>>每日可扫荡副本80次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 80);
		str += temp.getCString();
		str += "\n";
		// 			>>购买30次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 30);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银85次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 85);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限30
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 30);
		str += temp.getCString();
		str += "\n";
// 		// 			>>可免费刷新悬赏任务10次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 10);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>每次刷副本时可跳过4格小怪(直接BOSS战) 
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenTiaoGuo").c_str());
// 		temp.initWithFormat(buf, 4);
// 		str += temp.getCString();
// 		str += "\n";
		// 			>>可领取VIP9礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 9);
		str += temp.getCString();
		str += "\n";
		// 			>>可享受VIP8所有功能
		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
		temp.initWithFormat(buf, 8);
		str += temp.getCString();
	}
	else if (idx == 10)
	{
// 		//>>可领取定制高级神兵套装单件(3金星强力武器)
// 		strcpy(buf, LFStrings::getValue("vipJieShao_LingQuShenBing").c_str());
// 		temp.initWithFormat(buf, LFStrings::getValue("wuqi").c_str());
// 		str += temp.getCString();
		// 		>>每日可购买20次体力
		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
		temp.initWithFormat(buf, 20);
		str = temp.getCString();
		str += "\n";
		// 			>>每日可扫荡副本100次
		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
		temp.initWithFormat(buf, 100);
		str += temp.getCString();
		str += "\n";
		// 			>>购买40次竞技场挑战
		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
		temp.initWithFormat(buf, 40);
		str += temp.getCString();
		str += "\n";
		//>>每日可铸银100次
		strcpy(buf, LFStrings::getValue("vipJieShao_ZhuYinCiShu").c_str());
		temp.initWithFormat(buf, 100);
		str += temp.getCString();
		str += "\n";
		//>>探索次数上限32
		strcpy(buf, LFStrings::getValue("vipJieShao_TanSuoCiShu").c_str());
		temp.initWithFormat(buf, 32);
		str += temp.getCString();
		str += "\n";
// 		// 			>>可免费刷新悬赏任务12次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 12);
// 		str += temp.getCString();
// 		str += "\n";
		// 			>>可领取VIP10礼包
		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
		temp.initWithFormat(buf, 10);
		str += temp.getCString();
		str += "\n";
		// 			>>可享受VIP9所有功能
		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
		temp.initWithFormat(buf, 9);
		str += temp.getCString();
	}
// 	else if (idx == 11)
// 	{
// 		// 		>>每日可购买12次体力
// 		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
// 		temp.initWithFormat(buf, 12);
// 		str = temp.getCString();
// 		str += "\n";
// 		// 			>>每日可扫荡副本60次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
// 		temp.initWithFormat(buf, 60);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>购买80次竞技场挑战
// 		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
// 		temp.initWithFormat(buf, 80);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>可免费刷新悬赏任务12次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 12);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>可领取VIP8礼包
// 		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
// 		temp.initWithFormat(buf, 8);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>可享受VIP7所有功能
// 		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
// 		temp.initWithFormat(buf, 7);
// 		str += temp.getCString();
// 	}
// 	else if (idx == 12)
// 	{
// 		// 		>>每日可购买15次体力
// 		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
// 		temp.initWithFormat(buf, 15);
// 		str = temp.getCString();
// 		str += "\n";
// 		// 			>>每日可扫荡副本80次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
// 		temp.initWithFormat(buf, 80);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>购买90次竞技场挑战
// 		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
// 		temp.initWithFormat(buf, 90);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>可免费刷新悬赏任务15次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 15);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>可领取VIP9礼包
// 		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
// 		temp.initWithFormat(buf, 9);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>可享受VIP7所有功能
// 		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
// 		temp.initWithFormat(buf, 8);
// 		str += temp.getCString();
// 	}
// 	else if (idx == 13)
// 	{
// 		// 		>>每日可购买20次体力
// 		strcpy(buf, LFStrings::getValue("vipJieShao_TiLiGouMai").c_str());
// 		temp.initWithFormat(buf, 20);
// 		str = temp.getCString();
// 		str += "\n";
// 		// 			>>每日可扫荡副本100次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_FuBenSaoDang").c_str());
// 		temp.initWithFormat(buf, 100);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>购买100次竞技场挑战
// 		strcpy(buf, LFStrings::getValue("vipJieShao_JingJiChangCiShu").c_str());
// 		temp.initWithFormat(buf, 100);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>可免费刷新悬赏任务20次
// 		strcpy(buf, LFStrings::getValue("vipJieShao_RenWuShuaXin").c_str());
// 		temp.initWithFormat(buf, 20);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>可领取VIP10礼包
// 		strcpy(buf, LFStrings::getValue("vipJieShao_LiBaoLingQu").c_str());
// 		temp.initWithFormat(buf, 10);
// 		str += temp.getCString();
// 		str += "\n";
// 		// 			>>可享受VIP9所有功能
// 		strcpy(buf, LFStrings::getValue("vipJieShao_XiangShouVipGongNeng").c_str());
// 		temp.initWithFormat(buf, 9);
// 		str += temp.getCString();
// 	}
	
	return str;
}

void VipExplainLayer::menuItemClicked_MainMenu(CCObject *pSender)
{
	GameDirector::getDirector()->mainScene(mainlayer_ChongZhi);
}