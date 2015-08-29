
#include "EquipmentXiangQingBody.h"
#include "RenWuXiangXiLayer.h"
#include "MyselfManager.h"
#include "BackgroudLayer.h"
#include "GameScene.h"
#include "ZhuangBeiMianBanBase.h"
#include "ChangeZhuangBeiBody.h"
#include "LFToast.h"
#include "CS_Common.h"
#include "GongJiangQianghua.h"
#include "GongJiangFenJie.h"
//#include "GongJiangCuiQuFuMo.h"
#include "GongJiangBaoShiXiangQian.h"
#include "GongJiangJingLianZhuanKe.h"
#include "font.h"
#include "SuitPool.h"
#include "WuPinManager.h"
#include "ZhuangBeiMianBanOther.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "ZhuangBeiMianBanYuanShen.h"
#include "ShenBingShengJi.h"
#include "UnblockManager.h"
#include "GuideLayerBase.h"
#include "History.h"
#include "FX_CommonFunc.h"
#include "ZhuangbeiXilianUI.h"
#include "GongJiangJianDing.h"

using namespace FX_CommonFunc;

#define TAG_ITEM_CLOSE           1111  //关闭
#define TAG_ITEM_QUEDING         1112 //确定
#define TAG_ITEM_CHANGE      	 1113  //更换
#define TAG_ITEM_XiangXi_BIJIAO	 1114	// 详细对比
#define TAG_ITEM_YuanShi_BIJIAO	 1115	// 原始对比
#define TAG_ITEM_TAOZHUANG       1116 //套装
#define TAG_ITEM_JIEFENG         1117 //解封

//工匠系统
#define Tag_Item_Qianghua  1121 //强化
#define Tag_Item_JiangLian 1122 //精炼
#define Tag_Item_ZhuanKe   1123 //篆刻
#define Tag_Item_BaoShi    1124 //宝石
#define Tag_Item_JianDing  1125 //鉴定
#define Tag_Item_ShenBingLevUp 1126 //神兵升级
#define Tag_Item_Xilian 1127 //洗炼


extern Equipment* g_YuanShiBiJiao_Equip;

enum GongJiang_MenuTag
{
	Tag_Board_YuanShiDuiBi_Bg = 10,
	Tag_Board_TaoZhuangShuxing,

	Tag_Board_GongJiang_Bg,
	Tag_GJ_Close,
	Tag_GL_BaoShiCaiXie,
	Tag_GL_CuiQuJingHua,
	Tag_GL_FenJie,
	Tag_GL_FuMo,
	Tag_GL_ZhuangBeiHuiTu,
	Tag_GL_ZhuangBeiQiangHua,
	Tag_GL_BaoShiXiangQian,

	Tag_bianhui_Layer,

	Tag_Board_JieFengYin = 130, //解封印框
	Tag_Board_JianDing   = 133, //鉴定框
	Tag_MenuItem_OK,
	Tag_MenuItem_Cancel,
};

extern unsigned long long g_last_gongjiang_op_equipid;

void EquipmentXiangQingBody::popOneChildLayer(RenWuXiangXiBaseLayer* layer)
{
	RenWuXiangXiLayer* newlayer = new RenWuXiangXiLayer(layer->getActor(), layer->getNewData(),comefrom_xiangxiDuiBi);
	layer->release();

	mFooterLayer->popUpContaintLayer(newlayer);
	newlayer->release();
}

void EquipmentXiangQingBody::dismissMyselfLayer(bool someThingChanged)
{
	MainLayerBase* footer = mFooterLayer;

	
	if(mFrom == From_SpecialScene)
	{
		this->removeFromParent();
		return;
	}
	
	if(someThingChanged)
	{
		footer->dismmisPopUp();
		footer->dismmisPopUp();
		footer->getContentLayer()->refreshLayer();
	}
	else
	{
		footer->dismmisPopUp();
	}
}
void EquipmentXiangQingBody::replaceMyselfLayer(BaseLayer* layer)
{
	mFooterLayer->dismmisPopUp();
	MainLayerBase::getCurrentMainBase()->popUpContaintLayer(layer);
	((ChangeZhuangBeiBody*)layer)->setFooter(MainLayerBase::getCurrentMainBase());

}

void EquipmentXiangQingBody::menuItemClicked(CCObject* sender)
{
	CCMenuItemImage* item = (CCMenuItemImage*)sender;
	int tag = item->getTag();

	if (tag == TAG_ITEM_CLOSE)
	{
		dismissMyselfLayer();
	}
	else if (tag == TAG_ITEM_XiangXi_BIJIAO)
	{
		// 换上当前装备的数据
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(mActorId);
		CalculateData data = actor->getCalculateDataWhenEquipOne(mEquip);

		RenWuXiangXiBaseLayer* layer = new RenWuXiangXiBaseLayer(actor,data, comefrom_xiangxiDuiBi);
		layer->autorelease();
		popOneChildLayer(layer);
	}
	else if (tag == TAG_ITEM_CHANGE)
	{
		EquipRequirePlace place;
		switch (mEquip->getType())
		{
		case ZhuangbeiType_Dunpai:
		case ZhuangbeiType_Faqi:
			place = POS_FuShou;
			break;
		case ZhuangbeiType_KuijiaQin:
		case ZhuangbeiType_KuijiaZhong:
			place = POS_XiongJia;
			break;
		case ZhuangbeiType_Hushenfu:
			place = POS_Hushenfu;
			break;
		case ZhuangbeiType_Jiezhi:
			place = POS_JieZhi1;
			break;
			break;
		case ZhuangbeiType_DanshouJian:
		case ZhuangbeiType_ShuangshouBishou:
		case ZhuangbeiType_ShuangshouFu:
		case ZhuangbeiType_DanshouFazhang:
		case ZhuangbeiType_Gong:
		case ZhuangbeiType_ShuangshouFazhang:
			place = POS_ZhuWuQi;
			break;
		default:
			break;
		}
		ChangeZhuangBeiBody* change = new ChangeZhuangBeiBody(mActorId, place, mEquip);
		replaceMyselfLayer(change);
		change->release();
	}
	else if(tag == TAG_ITEM_QUEDING)
	{
		BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(mActorId);
		CCAssert(actor!=NULL,"");
		if (! actor->isYuanShen() && mEquip->getLevel() > actor->getCurrentProperty().level)
		{
//			LFToast::makeToast(CCString::createWithFormat(LF::getString("dengjibuzu").c_str(),mEquip->getLevel())->getCString());
// 			MessageDisplay *layer = MessageDisplay::create(CCString::createWithFormat(LF::getString("dengjibuzu").c_str(),mEquip->getLevel())->getCString());
// 			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			UnblockWarnDialog *layer = UnblockWarnDialog::create(CCString::createWithFormat(LF::getString("xuyaodengji").c_str(), mEquip->getLevel())->getCString(), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 100);
			return;
		}
		// 本来就在自己身上不需要装备
		if (mEquip->getActorId() == mActorId)
		{
			CCLog("This is already on self,not need equip again!,return.");
			return;
		}


		TempEquipZhuangbei requestData;
		// 脱装备的数据
		{
			if (mEquip->getActorId() != INVALID_ID)
			{
				BaseActor* oldActor = MyselfManager::getManager()->getMyTeamActor(mEquip->getActorId());
				CCAssert(oldActor->getId() == mEquip->getActorId(),"");

				TempHeroAndZhuangbei heroData;
				heroData.hero_id = oldActor->getId();
				int equipNum = oldActor->getEquipments().size();
				for (int i=0;i<equipNum;i++)
				{
					// 要脱掉的就不要加入
					if (oldActor->getEquipments()[i]->getEquipId() == mEquip->getEquipId())
					{
						continue;
					}

					heroData.zb_id_arr.push_back(oldActor->getEquipments()[i]->getEquipId());
				}
				requestData.hero_zb_arr.push_back(heroData);//137 --->0
			}
		}

		// 移除需要替换的，并装上新的
		{
			// 移除需要被替换掉的
			TempHeroAndZhuangbei heroData;  
			heroData.hero_id = actor->getId();
			//vector<Equipment*> needTakeOff;

			vector<Equipment*> needTakeOff;
			if (g_YuanShiBiJiao_Equip == NULL)
			{// 装备面板没有点击装备，那么这里就是添加

				// 得到可装备的卡槽
				vector<EquipRequirePlace> requiresPlace = mEquip->getRequiredPlaces();
				int placeNum = requiresPlace.size();

				if (placeNum > 1)
				{// 戒子
					CCAssert(mEquip->getType() == ZhuangbeiType_Jiezhi,"只支持戒子");

					// 因为点击的是空格，那不用看需要脱那些装备，直接就可以装
					needTakeOff.clear();
				}else
				{
					needTakeOff = actor->getNeedTakeOffIfWareOnEqui(mEquip->getType(),requiresPlace[0]);
//					CCAssert(needTakeOff.empty(),"因为点击的就是空白地方，所有应该没有要脱的");
				}

			}else
			{// 这里就是更换
				CCAssert(g_YuanShiBiJiao_Equip->getCurrentPlace() != POS_NONE,"");
				needTakeOff  = actor->getNeedTakeOffIfWareOnEqui(mEquip->getType(),g_YuanShiBiJiao_Equip->getCurrentPlace());
				CCAssert(needTakeOff.size()>=1,"必须有动洗要脱");
			}
			int takeOffNum = needTakeOff.size();
			vector<Equipment*> actorEquips = actor->getEquipments();
			int actorEquipNum = actorEquips.size();
			for (int j=0;j<actorEquipNum;j++)
			{
				bool canEquip = true;
				for (int i=0;i<takeOffNum;i++)
				{
					if (actorEquips[j]->getEquipId() == needTakeOff[i]->getEquipId())
					{
						canEquip = false;
						break;
					}
				}

				if (canEquip)
				{
					heroData.zb_id_arr.push_back(actorEquips[j]->getEquipId());
				}
			}


			// 装上
			heroData.zb_id_arr.push_back(mEquip->getEquipId());

			// 测试校验
			{
				int jiezi_count = 0;
				for (int i=0; i<heroData.zb_id_arr.size(); i++)
				{
					if (EquipmentManager::getManager()->getOneEquipment(heroData.zb_id_arr[i])->getType() == ZhuangbeiType_Jiezhi)
					{
						jiezi_count ++;
					}
				}
				CCAssert(jiezi_count <= 2,"不可能多个戒子");
				CCAssert(heroData.zb_id_arr.size() <= 6,"装备个数不肯能多余6个");
			}

			requestData.hero_zb_arr.push_back(heroData);
		}

		// 跳转到人行面板， 再监听 和发送
		if (actor->isYuanShen())
		{
			ZhuangBeiMianBanYuanShen* layer = dynamic_cast<ZhuangBeiMianBanYuanShen*>(
				mFooterLayer->getTopLayer(-2));
			if (layer != NULL)
				layer->setChangeZhuangBei(requestData, mEquip);
		}
		else 
		{
			ZhuangBeiMianBanBase* layer = (ZhuangBeiMianBanBase*)mFooterLayer->getContentLayer();
			layer->setChangeZhuangBei(requestData, mEquip);
		}
		dismissMyselfLayer(true);

	}
	else if(tag == TAG_ITEM_YuanShi_BIJIAO)
	{
		this->disableAllTouchBegin();

		BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_yuanshiduibi_bg.png");
		mBoard->addChild(board, 10, Tag_Board_YuanShiDuiBi_Bg);
		board->setAnchorPoint(ccp(0.5, 0));
		board->setPosition(ccp(320, 0));
		refresh_YuanShiBiaoJiao_Layer(board);
	}
	else if(tag == TAG_ITEM_TAOZHUANG)
	{
// 		this->disableAllTouchBegin();
// 
// 		BaseSprite *board = NULL;
// 		if (mEquip->isShenBing())
// 			board = BaseSprite::create("zhuangbei_xiangxi_mianban_taozhuang_shenbing.png");
// 		else 
// 			board = BaseSprite::create("zhuangbei_xiangxi_mianban_taozhuang.png");
// 		mBoard->addChild(board, 10, Tag_Board_TaoZhuangShuxing);
// 		board->setAnchorPoint(ccp(0.5, 0));
// 		board->setPosition(ccp(320, 3));
// 
// 		create_TaoZhuangShuXing_Item(board, mEquip);	

		//穿在身上的能触发套装属性的套装件数、 groupId
		vector<unsigned int> _vec;
		if (mEquip->getActorId() != INVALID_ID)
		{
			//自己英雄的
			if (MyselfManager::getManager()->getMyTeamActor(mEquip->getActorId()) != NULL)
			{
				vector<Equipment*> vecEquip = MyselfManager::getManager()->getMyTeamActor(mEquip->getActorId())->getEquipments();
				vector<unsigned int> equipedOnHero_groupId;
				for (int curI=0; curI<vecEquip.size(); ++curI)
					equipedOnHero_groupId.push_back(vecEquip.at(curI)->getGroupId());
				_vec = SuitPool::getPool()->getInSuitedGroupIdsFromTheGroupId(equipedOnHero_groupId, mEquip->getGroupId());
			}
			//查询其他英雄时出现的
			else
			{
				ZhuangBeiMianBanOther *layer_other = dynamic_cast<ZhuangBeiMianBanOther*>(mFooterLayer->getTopLayer(-1));
				ZhuangBeiMianBanYuanShen *layer_yuanshen = dynamic_cast<ZhuangBeiMianBanYuanShen*>(mFooterLayer->getTopLayer(-1));
				if (layer_other || layer_yuanshen)
				{ 
					vector<EquipData> vecData;
					if (layer_other) vecData = layer_other->getEquipsDataByActorId(mEquip->getActorId());
					else             vecData = layer_yuanshen->getEquipsDataByActorId(mEquip->getActorId());

					vector<unsigned int> equipedOnHero_groupId;
					for (int curI=0; curI<vecData.size(); ++curI)
						equipedOnHero_groupId.push_back(vecData.at(curI).group_id);
					_vec = SuitPool::getPool()->getInSuitedGroupIdsFromTheGroupId(equipedOnHero_groupId, mEquip->getGroupId());
				}
				else
				{
					CCLog("Error: %s --> 查询其他英雄的套装时 没找到ZhuangBeiMianBanOther", __FUNCTION__);
				}
			}
		}

		this->disableAllTouchBegin();
		EquipmentTaoZhuangXiangQing *layer = new EquipmentTaoZhuangXiangQing(mEquip->getGroupId(), mEquip->getName(), _vec, this);
		mBoard->addChild(layer, 20, Tag_Board_TaoZhuangShuxing);
		layer->release();
	}
	else if (tag == TAG_ITEM_JIEFENG)
	{
// 		int label_size = 23;
// 		//解封所需
// 		vector<CostData> jfCost = mEquip->getShenBingJieFengyinCost();
// 		CostData data;
// 		data.count = 1;
// 		data.item = ItemsType_Baoshi_lv_1;
// 		jfCost.push_back(data);
// 		for (int i=0; i!=jfCost.size(); ++i) // 排序，让银币排在最后面，显示。
// 			for (int j=i+1; j!=jfCost.size(); ++j)
// 				if (jfCost.at(i).item < jfCost.at(j).item)
// 					swap(jfCost.at(i), jfCost.at(j));
// 	
// 		vector<bool> hasVec(jfCost.size(), false);////////////////////////////////////
// 		vector<ItemInfo> items = WuPinManager::getManager()->getAllItems();
// 		for (int i=0; i!=items.size(); ++i)
// 		{
// 			for (int j=0; j!=jfCost.size(); ++j)
// 			{
// 				if (items.at(i).type == jfCost.at(j).item && items.at(i).count >= jfCost.at(j).count)
// 				{
// 					hasVec.at(j) = true;
// 					break;
// 				}
// 			}
// 		}
// 		bool bHas = false;////////////////////////////////////////////////////////////
// 		for (int i=0; i!=hasVec.size(); ++i)
// 		{
// 			if (! hasVec.at(i)) break;
// 			if (i == hasVec.size()-1) bHas = true;
// 		}
// 
// 		//弹框
// 		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
// 		dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
// 		//确认 取消按键  
// 		CCMenu* menu = CCMenu::create();
// 		menu->setPosition(CCPointZero);
// 		{
// 			CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
// 				"shangpin_goumai_queren.png",
// 				"shangpin_goumai_queren_select.png",
// 				this,SEL_MenuHandler(&EquipmentXiangQingBody::menuItemClicked_JieFeng));
// 			CCMenuItemImage* menuItemCancel = CCMenuItemImage::create(
// 				"shangpin_goumai_quxiao.png",
// 				"shangpin_goumai_quxiao_select.png",
// 				this,SEL_MenuHandler(&EquipmentXiangQingBody::menuItemClicked_JieFeng));
// 			if (bHas)
// 			{
// 				menu->addChild(menuItemOk, 1, 10);
// 				menu->addChild(menuItemCancel, 1, 20);
// 				menuItemOk->setPosition(ccp(dialog->getContentSize().width/3,70));
// 				menuItemCancel->setPosition(ccp(dialog->getContentSize().width/3*2, 70));
// 			}
// 			else 
// 			{
// 				menu->addChild(menuItemCancel, 1, 20);
// 				menuItemCancel->setPosition(ccp(dialog->getContentSize().width/2,70));
// 			}
// 		}
// 		//content
// 		{
// 			CCSize size = dialog->getContentSize();
// 			//title
// 			//CCLabelTTF *titleLabel = CCLabelTTF::create(LFStrings::getValue("jiechufengyintishi").c_str(), fontStr_kaiti, label_size);
// 			string name = CCString::createWithFormat(LFStrings::getValue("IsJianDing_String").c_str(), mEquip->getName().c_str())->getCString();
// 			CCLabelTTF *titleLabel = CCLabelTTF::create(name.c_str(), fontStr_kaiti, label_size, CCSizeMake(400, 0), kCCTextAlignmentCenter);
// 			dialog->addChild(titleLabel);
// 			titleLabel->setAnchorPoint(ccp(0.5,0.5));
// 			titleLabel->setPosition(ccp(size.width/2, size.height-86));
// 			if (! bHas)
// 			{
// // 				titleLabel->setString(LFStrings::getValue("cailiaobuzu").c_str());
// // 				titleLabel->setColor(ccRED);
// 			}
// 			//cailiao
// 			for (int i=0; i!=jfCost.size(); ++i)
// 			{
// 				BaseSprite *icon = NULL;
// 				if (jfCost.at(i).item == ItemsType_Silver) icon = BaseSprite::create("yinbi_tubiao.png");
// 				else if (jfCost.at(i).item == ItemsType_Gold) icon = BaseSprite::create("jinbi_tubiao.png");
// 				else 
// 				{
// 					icon = BaseSprite::create(ImageManager::getManager()->getItemFilename(jfCost.at(i).item));
// 					icon->setScale(0.75);
// 				}
// 				dialog->addChild(icon);
// 				icon->setPosition(ccp(180, 221-70*i));
// 				//
// 				string info;
// 				if (jfCost.at(i).item > ItemsType_Silver) info = CS::getItemName(jfCost.at(i).item) + " x";
// 				char buf[20];
// 				sprintf(buf, "%d", jfCost.at(i).count);
// 				info += buf;
// 				CCLabelTTF *costLabel = CCLabelTTF::create(info.c_str(), fontStr_kaiti, label_size);
// 				dialog->addChild(costLabel);
// 				costLabel->setAnchorPoint(ccp(0, 0.5f));
// 				costLabel->setPosition(ccp(size.width/2, icon->getPositionY()));
// 				if (! bHas && ! hasVec.at(i))
// 					costLabel->setColor(ccRED);
// 			}
// 		}
// 		//
// 		{
// 			LFAlert* lfAlert = new LFAlert();
// 			lfAlert->setTag(Tag_Board_JieFengYin);
// 			lfAlert->setBaseContainer(dialog,menu);
// 			CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_Board_JieFengYin);
// 			lfAlert->release();
// 		}
		
		GongJiangXiYouZhuangBeiJianDing *layer = GongJiangXiYouZhuangBeiJianDing::create(gongjiang_jianding, mEquip);
		mFooterLayer->popUpContaintLayer(layer);
	}
	else
	{
		CCAssert(false,"Not implement!");
	}
}

void EquipmentXiangQingBody::menuItemGongJiang(CCObject* sender)
{
	int tag = ((CCMenuItemImage*)sender)->getTag();
	string info;
	CCString tempStr;

	if (tag == Tag_Item_Qianghua)
	{
		if (! UnblockManager::getManager()->isDeblocking(unblock_GongJiang_QiangHuaHuiTui))
		{
			tempStr.initWithFormat(LFStrings::getValue("WanChengRenWu_d_JiKaiQi").c_str(), 4);
			UnblockWarnDialog *layer = UnblockWarnDialog::create(tempStr.getCString(), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		else 
		{
			if(this!=NULL)
			  this->setVisible(false);
			BaseLayer *layer = GongJiangQianghua::create(gongjiang_qianghua, mEquip);
			mFooterLayer->popUpContaintLayer(layer);
		}
	}
	else if (tag == Tag_Item_JiangLian)
	{
		if (! UnblockManager::getManager()->isDeblocking(unblock_GongJiang_JingLianZhuanKe))
		{
			tempStr.initWithFormat(LFStrings::getValue("ZhuJue_d_JiKaiQi").c_str(), 10);
			UnblockWarnDialog *layer = UnblockWarnDialog::create(tempStr.getCString(), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		else 
		{
			if(this!=NULL)
			  this->setVisible(false);
			GongJiangJingLianZhuanKe *layer = GongJiangJingLianZhuanKe::create(gongjiang_jinglian, mEquip);
			mFooterLayer->popUpContaintLayer(layer);
		}
	}
	else if (tag == Tag_Item_ZhuanKe)
	{
		if (! UnblockManager::getManager()->isDeblocking(unblock_GongJiang_JingLianZhuanKe))
		{
			tempStr.initWithFormat(LFStrings::getValue("ZhuJue_d_JiKaiQi").c_str(), 10);
			UnblockWarnDialog *layer = UnblockWarnDialog::create(tempStr.getCString(), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		else
		{
			if(this!=NULL)
			  this->setVisible(false);
			GongJiangJingLianZhuanKe *layer = GongJiangJingLianZhuanKe::create(gongjiang_zhuanke, mEquip);
			mFooterLayer->popUpContaintLayer(layer);
		}
	}
	else if (tag == Tag_Item_BaoShi)
	{
		if (! UnblockManager::getManager()->isDeblocking(unblock_GongJiang_BaoShiXiangQian))
		{
			tempStr.initWithFormat(LFStrings::getValue("WanChengRenWu_d_JiKaiQi").c_str(), 15);
			UnblockWarnDialog *layer = UnblockWarnDialog::create(tempStr.getCString(), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		else
		{
			if(this!=NULL)
			  this->setVisible(false);
			BaseLayer *layer = NULL;
			if (CS::canZBXiangqianBaoshi(mEquip->getEquipId()))
				layer = GongJiangBaoShiXiangQian::create(gongjiang_xiangqian, mEquip);
			else 
				layer = GongJiangBaoShiXiangQian::create(gongjiang_caixie, mEquip);
			mFooterLayer->popUpContaintLayer(layer);
		}
	}
	else if (tag == Tag_Item_ShenBingLevUp)
	{
		ShenBingShengJi *layer = new ShenBingShengJi(mEquip);
		if (layer)
		{
			if(this!=NULL)
			  this->setVisible(false);
			mFooterLayer->popUpContaintLayer(layer);
			layer->release();
		}
		else 
		{
			delete layer;
		}
	}
	else if (tag == Tag_Item_Xilian)
	{

		/*if (! UnblockManager::getManager()->isDeblocking(unblock_GongJiang_JingLianZhuanKe))
		{
			tempStr.initWithFormat(LFStrings::getValue("ZhuJue_d_JiKaiQi").c_str(), 10);
			UnblockWarnDialog *layer = UnblockWarnDialog::create(tempStr.getCString(), false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		else */
		{
			if(this!=NULL)
			  this->setVisible(false);
			ZhuangbeiXilianUI *layer = ZhuangbeiXilianUI::createWithEquipment(mEquip);
			mFooterLayer->popUpContaintLayer(layer);
		}
	}

/*	else if (tag == Tag_Item_JianDing)
	{
		if (! mEquip->haveUnkownHidenAttribute())
		{
			MessageDisplay* layer = MessageDisplay::create(LFStrings::getValue("yishiZuiDaJianDing_info"));
			CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
			return;
		}
		int label_size = 23;
		//解封所需
		vector<CostData> jfCost = mEquip->getShenBingJianDingCurrentCost();
		for (int i=0; i!=jfCost.size(); ++i) // 排序，让银币排在最后面，显示。
			for (int j=i+1; j!=jfCost.size(); ++j)
				if (jfCost.at(i).item < jfCost.at(j).item)
					swap(jfCost.at(i), jfCost.at(j));

		vector<bool> hasVec(jfCost.size(), false);////////////////////////////////////
		vector<ItemInfo> items = WuPinManager::getManager()->getAllItems();
		for (int i=0; i!=items.size(); ++i)
		{
			for (int j=0; j!=jfCost.size(); ++j)
			{
				if (items.at(i).type == jfCost.at(j).item && items.at(i).count >= jfCost.at(j).count)
				{
					hasVec.at(j) = true;
					break;
				}
			}
		}
		bool bHas = false;////////////////////////////////////////////////////////////
		for (int i=0; i!=hasVec.size(); ++i)
		{
			if (! hasVec.at(i)) break;
			if (i == hasVec.size()-1) bHas = true;
		}

		//弹框
		BaseSprite* dialog = BaseSprite::create("shangpin_goumai_tishikuang.png");
		dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2));
		//确认 取消按键  
		CCMenu* menu = CCMenu::create();
		menu->setPosition(CCPointZero);
		{
			CCMenuItemImage* menuItemOk = CCMenuItemImage::create(
				"shangpin_goumai_queren.png",
				"shangpin_goumai_queren_select.png",
				this,SEL_MenuHandler(&EquipmentXiangQingBody::menuItemClicked_JianDing));
			CCMenuItemImage* menuItemCancel = CCMenuItemImage::create(
				"shangpin_goumai_quxiao.png",
				"shangpin_goumai_quxiao_select.png",
				this,SEL_MenuHandler(&EquipmentXiangQingBody::menuItemClicked_JianDing));
			if (bHas)
			{
				menu->addChild(menuItemOk, 1, 10);
				menu->addChild(menuItemCancel, 1, 20);
				menuItemOk->setPosition(ccp(dialog->getContentSize().width/3,74));
				menuItemCancel->setPosition(ccp(dialog->getContentSize().width/3*2, 70));
			}
			else 
			{
				menu->addChild(menuItemCancel, 1, 20);
				menuItemCancel->setPosition(ccp(dialog->getContentSize().width/2,70));
			}
		}
		//content
		{
			CCSize size = dialog->getContentSize();
			//title
			CCLabelTTF *titleLabel = CCLabelTTF::create(LFStrings::getValue("jiandingtishi").c_str(), fontStr_kaiti, label_size);
			dialog->addChild(titleLabel);
			titleLabel->setAnchorPoint(ccp(0.5,0.5));
			titleLabel->setPosition(ccp(size.width/2, size.height-52));
			if (! bHas)
			{
				titleLabel->setString(LFStrings::getValue("cailiaobuzu").c_str());
				titleLabel->setColor(ccRED);
			}
			//cailiao
			for (int i=0; i!=jfCost.size(); ++i)
			{
				BaseSprite *icon = NULL;
				if (jfCost.at(i).item == ItemsType_Silver) icon = BaseSprite::create("yinbi_tubiao.png");
				else if (jfCost.at(i).item == ItemsType_Gold) icon = BaseSprite::create("jinbi_tubiao.png");
				else 
				{
					icon = BaseSprite::create(ImageManager::getManager()->getItemFilename(jfCost.at(i).item));
					icon->setScale(0.75);
				}
				dialog->addChild(icon);
				icon->setPosition(ccp(180, 221-70*i));
				//
				string info;
				if (jfCost.at(i).item > ItemsType_Silver) info = CS::getItemName(jfCost.at(i).item) + " x";
				char buf[20];
				sprintf(buf, "%d", jfCost.at(i).count);
				info += buf;
				CCLabelTTF *costLabel = CCLabelTTF::create(info.c_str(), fontStr_kaiti, label_size);
				dialog->addChild(costLabel);
				costLabel->setAnchorPoint(ccp(0, 0.5f));
				costLabel->setPosition(ccp(size.width/2, icon->getPositionY()));
				if (! bHas && ! hasVec.at(i))
					costLabel->setColor(ccRED);
			}
		}
		//
		{
			LFAlert* lfAlert = new LFAlert();
			lfAlert->setTag(Tag_Board_JianDing);
			lfAlert->setBaseContainer(dialog,menu);
			CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128, Tag_Board_JianDing);
			lfAlert->release();
		}
	}*/

}

// void EquipmentXiangQingBody::menuItemClicked_GongjiangTouMing(CCObject *pSender)
// {
// 	mBoard->removeChildByTag(Tag_Board_GongJiang_Bg);
// 	mBoard->removeChildByTag(Tag_bianhui_Layer);
// 	this->disableAllTouchEnd();
// }

void EquipmentXiangQingBody::reportChangeZhuangBeiRsp(CCObject* msg_rsp)
{

}

// CCMenu* EquipmentXiangQingBody::getGongJiangMenu(CCSize size)
// {
// 	CCMenu *menu = CCMenu::create();
// 	menu->setPosition(CCPointZero);
// 	//关闭按钮
// 	CCMenuItemImage* closeItem = CCMenuItemImage::create(
// 		"close.png",
// 		"close_select.png",
// 		this,menu_selector(EquipmentXiangQingBody::menuItemClicked_GongJiang)    
// 		);
// 	closeItem->setPosition(ccp(size.width - closeItem->getContentSize().width/2, size.height - closeItem->getContentSize().height/2));
// 	menu->addChild(closeItem, 1, Tag_GJ_Close);
// 
// 	//
// 	int start_x = 117, start_y = 321-104;   
// 	int delta_x = 190, delta_y = 80;
// 	int itemNum = 0;
// 	string info;
// 	// 1 2 3
// 	// 4 5 6
// 
// 	//宝石拆卸
// 	{
// 		++itemNum;
// 		CCMenuItemImage *caixie = CCMenuItemImage::create(
// 			"zhuangbei_xiangxi_gongjiang_chaixie.png"
// 			, "zhuangbei_xiangxi_gongjiang_chaixie_select.png"
// 			, "zhuangbei_xiangxi_gongjiang_chaixie_suoding.png"
// 			, this, menu_selector(EquipmentXiangQingBody::menuItemClicked_GongJiang));
// 		menu->addChild(caixie, 1, Tag_GL_BaoShiCaiXie);
// 		caixie->setPosition(ccp(start_x + (itemNum-1)%3*delta_x, start_y - (itemNum-1)/3*delta_y));
// 
// 		if(mEquip->getBaoShiNum() == 0)
// 			caixie->setEnabled(false);
// 	}
// 	//萃取精华
// 	{
// 		++itemNum;
// 		CCMenuItemImage *item = CCMenuItemImage::create(
// 			"zhuangbei_xiangxi_gongjiang_cuiqu.png"
// 			, "zhuangbei_xiangxi_gongjiang_cuiqu_select.png"
// 			, "zhuangbei_xiangxi_gongjiang_cuiqu_suoding.png"
// 			, this, menu_selector(EquipmentXiangQingBody::menuItemClicked_GongJiang));
// 		menu->addChild(item, 1, Tag_GL_CuiQuJingHua);
// 		item->setPosition(ccp(start_x + (itemNum-1)%3*delta_x, start_y - (itemNum-1)/3*delta_y));
// 
// 		if(! mEquip->canBeCuiqu(info))
// 			item->setEnabled(false);
// 	}
// 	//装备分解
// 	{
// 		++itemNum;
// 		CCMenuItemImage *item = CCMenuItemImage::create(
// 			"zhuangbei_xiangxi_gongjiang_fenjie.png"
// 			, "zhuangbei_xiangxi_gongjiang_fenjie_select.png"
// 			, "zhuangbei_xiangxi_gongjiang_fenjie_suoding.png"
// 			, this, menu_selector(EquipmentXiangQingBody::menuItemClicked_GongJiang));
// 		menu->addChild(item, 1, Tag_GL_FenJie);
// 		item->setPosition(ccp(start_x + (itemNum-1)%3*delta_x, start_y - (itemNum-1)/3*delta_y));
// 
// 		if(mEquip->getActorId() != INVALID_ID)
// 		 item->setEnabled(false);
// 	}
// 	//附魔
// 	{
// 		++itemNum;
// 		CCMenuItemImage *item = CCMenuItemImage::create(
// 			"zhuangbei_xiangxi_gongjiang_fumo.png"
// 			, "zhuangbei_xiangxi_gongjiang_fumo_select.png"
// 			, "zhuangbei_xiangxi_gongjiang_fumo_suoding.png"
// 			, this, menu_selector(EquipmentXiangQingBody::menuItemClicked_GongJiang));
// 		menu->addChild(item, 1, Tag_GL_FuMo);
// 		item->setPosition(ccp(start_x + (itemNum-1)%3*delta_x, start_y - (itemNum-1)/3*delta_y));
// 
// 		if(! mEquip->canBeFumo(info))
// 			item->setEnabled(false);
// 	}
// 	//装备回退
// 	{
// 		++itemNum;
// 		CCMenuItemImage *item = CCMenuItemImage::create(
// 			"zhuangbei_xiangxi_gongjiang_huitui.png"
// 			, "zhuangbei_xiangxi_gongjiang_huitui_select.png"
// 			, "zhuangbei_xiangxi_gongjiang_huitui_suoding.png"
// 			, this, menu_selector(EquipmentXiangQingBody::menuItemClicked_GongJiang));
// 		menu->addChild(item, 1, Tag_GL_ZhuangBeiHuiTu);
// 		item->setPosition(ccp(start_x + (itemNum-1)%3*delta_x, start_y - (itemNum-1)/3*delta_y));
// 
// 		if(! mEquip->canBeHuiTui(info))
// 			item->setEnabled(false);
// 	}
// 	//装备强化
// 	{
// 		++itemNum;
// 		CCMenuItemImage *item = CCMenuItemImage::create(
// 			"zhuangbei_xiangxi_gongjiang_qianghua.png"
// 			, "zhuangbei_xiangxi_gongjiang_qianghua_select.png"
// 			, "zhuangbei_xiangxi_gongjiang_qianghua_suoding.png"
// 			, this, menu_selector(EquipmentXiangQingBody::menuItemClicked_GongJiang));
// 		menu->addChild(item, 1, Tag_GL_ZhuangBeiQiangHua);
// 		item->setPosition(ccp(start_x + (itemNum-1)%3*delta_x, start_y - (itemNum-1)/3*delta_y));
// 
// 		if(! mEquip->canBeQianghua(info))
// 			item->setEnabled(false);
// 	}
// 	//宝石镶嵌
// 	{
// 		++itemNum;
// 		CCMenuItemImage *item = CCMenuItemImage::create(
// 			"zhuangbei_xiangxi_gongjiang_xiangqian.png"
// 			, "zhuangbei_xiangxi_gongjiang_xiangqian_select.png"
// 			, "zhuangbei_xiangxi_gongjiang_xiangqian_suoding.png"
// 			, this, menu_selector(EquipmentXiangQingBody::menuItemClicked_GongJiang));
// 		menu->addChild(item, 1, Tag_GL_BaoShiXiangQian);
// 		item->setPosition(ccp(start_x + (itemNum-1)%3*delta_x, start_y - (itemNum-1)/3*delta_y));
// 
// 		if(! CS::canZBXiangqianBaoshi(mEquip->getEquipId()))
// 			item->setEnabled(false);
// 	}
// 
// 
// 	return menu;
// }
/*
void EquipmentXiangQingBody::menuItemClicked_GongJiang(CCObject *pSender)
{
	int tag = ((CCMenuItemImage*)pSender)->getTag();

	if(tag == Tag_GJ_Close)
	{
		mBoard->getChildByTag(Tag_Board_GongJiang_Bg)->removeFromParent();
		this->disableAllTouchEnd();
		return;
	}

	mBoard->removeChildByTag(Tag_Board_GongJiang_Bg);
	mBoard->removeChildByTag(Tag_bianhui_Layer);
	this->disableAllTouchBegin();

	BaseLayer *layer = NULL;
	switch (tag)
	{
	case Tag_GL_BaoShiCaiXie:
		layer = GongJiangBaoShiXiangQian::create(gongjiang_caixie, mEquip);
		break;
	case Tag_GL_BaoShiXiangQian:
		layer = GongJiangBaoShiXiangQian::create(gongjiang_xiangqian, mEquip);
		break;
	case Tag_GL_CuiQuJingHua:
		layer = GongJiangCuiQuFuMo::create(gongjiang_CuiQu, mEquip);
		break;
	case Tag_GL_FuMo:
		layer = GongJiangCuiQuFuMo::create(gongjiang_FuMo, mEquip);
		break;
	case Tag_GL_FenJie:
		layer = GongJiangFenJie::create(mEquip);
		break;
	case Tag_GL_ZhuangBeiHuiTu:
		layer = GongJiangQianghua::create(gongjiang_huitui, mEquip);
		break;
	case Tag_GL_ZhuangBeiQiangHua:
		layer = GongJiangQianghua::create(gongjiang_qianghua, mEquip);
		break;
	default:
		CCLog("Error: EquipmentXiangQingBody::menuItemClicked_GongJiang --- default tag = [%d]", tag);
		break;
	}

	if(layer != NULL)
		mFooterLayer->popUpContaintLayer(layer);
}*/

void EquipmentXiangQingBody::refresh_YuanShiBiaoJiao_Layer(BaseSprite* board)
{
	int iYoffset = 122;

	CCMenu *menu = CCMenu::create();
	board->addChild(menu);
	menu->setPosition(CCPointZero);
	//
	CCMenuItemImage *fanhui = CCMenuItemImage::create(
		"shuxing_renwu_fanhui.png"
		, "shuxing_renwu_fanhui_select.png"
		, this, menu_selector(EquipmentXiangQingBody::menuItemClicked_FanHui));
	menu->addChild(fanhui);
	fanhui->setPosition(ccp(board->getContentSize().width/2, 838-784));
	//

	Equipment *oldEquip = g_YuanShiBiJiao_Equip;
	Equipment *curEquip = mEquip;

	//标题，装备名称 
	{
		string nameStr = oldEquip->getName() + LFStrings::getValue("yizhuangbei");
		CCLabelTTF* lableName = LF::lable(board,nameStr,CCPointZero,28,fontStr_kaiti);
		lableName->setPosition(ccp(board->getContentSize().width/4,750+iYoffset));

		ccColor3B color = ImageManager::getManager()->getShowColor(oldEquip->getEquipColor(),false,oldEquip->isPartOfSuit());
		lableName->setColor(color);
	}
	//底纹
	{
		ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			oldEquip->getEquipColor(),
			false,
			oldEquip->isPartOfSuit(),
			oldEquip->haveZhuanke());
		BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
		diWen->setScale(0.35f);
		board->addChild(diWen);
		diWen->setAnchorPoint(ccp(0, 1));
		diWen->setPosition(ccp(20, 774-50+iYoffset));
		// 装备图片
		ImageData data = ImageManager::getManager()->getImageData_ZhuangBei_XiangQing(oldEquip);
		BaseSprite* s= ImageManager::getManager()->getSpriteOfZhuangbei(oldEquip);
		s->setScale(data.scale);
		diWen->addChild(s);
		s->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));
	}
	{//property
		BaseSprite *layer  = BaseSprite::create();
		layer->setContentSize(CCSize(600/4, 600));
		board->addChild(layer);
		layer->setAnchorPoint(ccp(0,1));
		layer->setPosition(ccp(131, 710+iYoffset));
		create_YuanShiBijiao_Item(layer, oldEquip);
	}

	//标题，装备名称 
	{
		string nameStr = curEquip->getName();
		CCLabelTTF* lableName = LF::lable(board,nameStr,CCPointZero,28,fontStr_kaiti);
		lableName->setPosition(ccp(board->getContentSize().width/4*3,750+iYoffset));

		ccColor3B color = ImageManager::getManager()->getShowColor(curEquip->getEquipColor(),false,curEquip->isPartOfSuit());
		lableName->setColor(color);
	}
	//底纹
	{
		ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_XiangQing(
			curEquip->getEquipColor(),
			false,
			curEquip->isPartOfSuit(),
			curEquip->haveZhuanke());
		BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
		diWen->setScale(0.35f);
		board->addChild(diWen);
		diWen->setAnchorPoint(ccp(0, 1));
		diWen->setPosition(ccp(board->getContentSize().width/2 + 10, 774-50+iYoffset));
		// 装备图片
		ImageData data = ImageManager::getManager()->getImageData_ZhuangBei_XiangQing(curEquip);
		BaseSprite* s= ImageManager::getManager()->getSpriteOfZhuangbei(curEquip);
		s->setScale(data.scale);
		diWen->addChild(s);
		s->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));
	}
	{//property
		BaseSprite *layer  = BaseSprite::create();
		layer->setContentSize(CCSize(600/4, 600));
		board->addChild(layer, 2);
		layer->setAnchorPoint(ccp(0,1));
		layer->setPosition(ccp(440, 710+iYoffset));
		create_YuanShiBijiao_Item(layer, curEquip);
	}
}

void EquipmentXiangQingBody::create_YuanShiBijiao_Item(BaseSprite *base, Equipment *equip)
{
#define LABEL_SIZE 20
	//类型
	float label_gap = 2;
	float label_y = base->getContentSize().height;
	float start_x = 6;
	{
		CCLabelTTF* label;
		string str = CS::getZhuangbeiTypeName(equip->getType(),equip->getEquipColor());
		label = LF::lable(base,str,ccp(0,0),20,fontStr_kaiti);
		label->setAnchorPoint(ccp(0, 0.5));
		label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));

		ccColor3B color = ImageManager::getManager()->getShowColor(equip->getEquipColor(),false, false);

		label->setColor(color);

		label_y -= label_gap + label->getContentSize().height;
	}
	//白字
//	if (CS::canZBTypeBeQianghua(equip->getType()))
	{
		CCLabelTTF* label;
		float baizi = 0;
		{
			switch(equip->getType())
			{
			case ZhuangbeiType_DanshouJian:
			case ZhuangbeiType_ShuangshouBishou:
			case ZhuangbeiType_ShuangshouFu:
			case ZhuangbeiType_DanshouFazhang:
			case ZhuangbeiType_ShuangshouFazhang:
			case ZhuangbeiType_Gong:
				baizi = (equip->getEquipData().gongji_min + equip->getEquipData().gongji_max) / 2.0;
//				baizi = equip->getGongjiLiAfterQianghua(false);
				break;
			case ZhuangbeiType_Dunpai:
				//case ZhuangbeiType_Toukui:
			case ZhuangbeiType_KuijiaZhong:
			case ZhuangbeiType_KuijiaQin:
				//case ZhuangbeiType_KuziZhong:
				//case ZhuangbeiType_KuziQin:
				//case ZhuangbeiType_Xuezi:
				baizi = equip->getEquipData().hujia;
//				baizi = equip->getHuJiaZhiAfterQianghua();
				break;
			case ZhuangbeiType_Jiezhi:
			case ZhuangbeiType_Hushenfu:
			case ZhuangbeiType_Faqi:
			default:
				break;
			}
		}

		char str[64];
		sprintf(str, "%.1f", baizi);
		label = LF::lable(base,str,ccp(0,0),70,fontStr_BookAntiqua);
		label->setColor(ccWHITE);
		label->setAnchorPoint(ccp(0, 0.5));
		label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));

		label_y -= label_gap + label->getContentSize().height;
	}
	//伤害详细
	if (equip->getEquipType() == EquipType_WuQi)
	{
		CCLabelTTF* label;
		char str[64];
		sprintf(str, "%u-%u ", equip->getEquipData().gongji_min, equip->getEquipData().gongji_max);
		label = LF::lable(base,str,ccp(0,0),LABEL_SIZE,fontStr);
		label->setColor(ccWHITE);
		label->setAnchorPoint(ccp(0, 0.5));
		label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));

		CCLabelTTF* label2;
		label2 = LF::lable(base,LFStrings::getValue("dianShangHai"),ccp(0,0),LABEL_SIZE,fontStr_kaiti);
		ccColor3B color;
		color.r = 100;
		color.g = 98;
		color.b = 97;
		label2->setColor(color);
		label2->setAnchorPoint(ccp(0, 0.5));
		label2->setPosition(ccp(label->getPosition().x + label->getContentSize().width, label->getPositionY()));

		label_y -= label_gap + label->getContentSize().height;
	}

	//等级
	{
		CCLabelTTF* label;
		char str[64];
		sprintf(str, "%s %u ", LFStrings::getValue("dengji").c_str(), equip->getEquipData().zhuangbei_level);
		label = LF::lable(base,str,ccp(0,0),LABEL_SIZE,fontStr_kaiti);
		ccColor3B color;
		color.r = 100;
		color.g = 98;
		color.b = 97;
		label->setColor(color);
		label->setAnchorPoint(ccp(0, 0.5));
		label->setPosition(ccp(start_x, label_y - label_gap - label->getContentSize().height/2));

		label_y -= label_gap + label->getContentSize().height;
	}
	//魔法属性
	{
		label_y -= label_gap;

		std::vector<std::string> vec = equip->getAllMofaShuxing(false);

		for(int i=0; i<vec.size(); ++i)
		{
			BaseSprite* biaoji = BaseSprite::create("zhuangbei_xiangxi_biaoji.png");
			base->addChild(biaoji);


			CCLabelTTF* label;
			label = LF::lable(base,vec[i],ccp(0,0),LABEL_SIZE,fontStr_kaiti);
			ccColor3B color;
			color.r = 79;
			color.g = 110;
			color.b = 217;
			label->setColor(color);
			label->setAnchorPoint(ccp(0, 0.5));
			label->setPosition(ccp(start_x+biaoji->getContentSize().width + 10, label_y - label_gap - label->getContentSize().height/2));
			//
			biaoji->setPosition(ccp(start_x+biaoji->getContentSize().width/2, label->getPositionY()));

			label_y -= label_gap + label->getContentSize().height;
		}
	}
}

void EquipmentXiangQingBody::menuItemClicked_FanHui(CCObject *pSender)
{
	mBoard->removeChildByTag(Tag_Board_YuanShiDuiBi_Bg);

//	mFooterLayer->dismmisPopUp();
	this->disableAllTouchEnd();
}


void EquipmentXiangQingBody::menuItemClicked_JieFeng(CCObject *pSender) // 10发消息   20直接移除
{
	int tag = dynamic_cast<CCMenuItem*>(pSender)->getTag();
	if (tag == 20)
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Board_JieFengYin);
	}
	else if (tag == 10)
	{
		addObserver(callfuncO_selector(EquipmentXiangQingBody::rsp_shenBingJieFengYin), MSG_shenbingJieFengyinRsp);
		mCmdHlp->cmdShenbingJieFengyin(mEquip->getEquipId());
	}
}

void EquipmentXiangQingBody::rsp_shenBingJieFengYin(CCObject *msg)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_shenbingJieFengyinRsp);

	SPCmd_ShenbingJieFengyinRsp *data = (SPCmd_ShenbingJieFengyinRsp*)dynamic_cast<MSG_Rsp*>(msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Board_JieFengYin);
	this->refreshLayer();
}

void EquipmentXiangQingBody::menuItemClicked_JianDing(CCObject *pSender) // 10发消息   20直接移除
{
	int tag = dynamic_cast<CCMenuItem*>(pSender)->getTag();
	if (tag == 20)
	{
		CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Board_JianDing);
	}
	else if (tag == 10)
	{
		addObserver(callfuncO_selector(EquipmentXiangQingBody::rsp_shenBingJianDing), MSG_shenbingJiandingRsp);
		mCmdHlp->cmdShenbingJianding(mEquip->getEquipId());
	}
}

void EquipmentXiangQingBody::rsp_shenBingJianDing(CCObject *msg)
{
	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_shenbingJiandingRsp);

	SPCmd_ShenbingJiandingRsp *data = (SPCmd_ShenbingJiandingRsp*)dynamic_cast<MSG_Rsp*>(msg)->getData();
	if (data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Board_JianDing);
	this->refreshLayer();
}

void EquipmentXiangQingBody::disableAllTouchBegin()
{
	EquipmentXiangQingBaseBody::disableAllTouchBegin();
}

void EquipmentXiangQingBody::disableAllTouchEnd()
{
	EquipmentXiangQingBaseBody::disableAllTouchEnd();


	//教学
	if ((mFrom == From_RenXingMianBan || mFrom == From_ZhuangBeiList )
		&& mEquip->getEquipColor() == ZhuangbeiColour_Chuanqi
		&& g_FirstChuanQiEquipLook)
	{

		equipLevelUpGuide();

	}
}

void EquipmentXiangQingBody::callbackInterface_TaoZhuangLayer()
{
	mBoard->removeChildByTag(Tag_Board_TaoZhuangShuxing);
	disableAllTouchEnd();


	//教学
	bool firstDisplay_ChuanQiEquip = false;
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstDisplayChuanQiEquip))
	{
		//GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstDisplayChuanQiEquip)
		firstDisplay_ChuanQiEquip = true;
		//升级界面再设置  
		//History::getHistory()->putInCurrentHero("First_DisplayChuanQiEquip", false);
	}
	if ((mFrom == From_RenXingMianBan || mFrom == From_ZhuangBeiList )
		&& mEquip->getEquipColor() == ZhuangbeiColour_Chuanqi
		&& firstDisplay_ChuanQiEquip)
	{
		CCMenuItem *qhItem = (CCMenuItem*)mGongJiangMenu->getChildByTag(Tag_Item_Qianghua);

		if (qhItem)
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstDisplayChuanQiEquip);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstDisplayChuanQiEquip);
			GuideLayerBase *guideLayer= new GuideLayer_SimpleLayer;
			guideLayer->init(
				qhItem->getContentSize(), 
				qhItem->getParent()->convertToWorldSpace(qhItem->getPosition()), 
				LFStrings::getValue("chuanqi_QiangHuaAnNiuShuoMing"), NULL, false);

			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);

			g_FirstChuanQiEquipLook = true;
		}
		else  //如果第一次看到的是首饰、 没有强化按钮的
		{
			
			equipLevelUpGuide();
		}
	}
}

void EquipmentXiangQingBody::equipLevelUpGuide()
{
	CCMenuItem *shengJiItem = (CCMenuItem*)mGongJiangMenu->getChildByTag(Tag_Item_ShenBingLevUp);
	CCMenuItem *jlItem = (CCMenuItem*)mGongJiangMenu->getChildByTag(Tag_Item_JiangLian);
	CCMenuItem *zkItem = (CCMenuItem*)mGongJiangMenu->getChildByTag(Tag_Item_ZhuanKe);
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstDisplayChuanQiEquipLevel))
	{
	
		GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstDisplayChuanQiEquipLevel);
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstDisplayChuanQiEquipLevel);
		if (shengJiItem)
		{
			GuideLayerBase *guideLayer2 = new GuideLayer_SimpleLayer;
			guideLayer2->init(
				shengJiItem->getContentSize(), 
				shengJiItem->getParent()->convertToWorldSpace(shengJiItem->getPosition()), 
				LFStrings::getValue("chuanqi_chuanQiZhuangShengJi"), NULL, false);

			vector<LightMaskPos> vec;
			LightMaskPos temp2, temp3;
			if (jlItem)
			{
				temp2.info = LFStrings::getValue("chuanqi_JingLianAnNiuShuoMing");
				temp2.pos  = jlItem->getParent()->convertToWorldSpace(jlItem->getPosition());
				temp2.size = jlItem->getContentSize();
				vec.push_back(temp2);
			}
			if (zkItem)
			{
				temp3.info = LFStrings::getValue("chuanqi_ZhuanKeAnNiuShuoMing");
				temp3.pos  = zkItem->getParent()->convertToWorldSpace(zkItem->getPosition());
				temp3.size = zkItem->getContentSize();
				vec.push_back(temp3);
			}

			if (vec.size() > 0)
			{
				GuideLayerBase *guideLayer1 = new GuideLayer_MultLightAndLabel;
				guideLayer1->init(vec, guideLayer2, true);

				CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer1, 1024);
			}
			else
			{
				CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer2, 1024);
			}

			g_FirstChuanQiEquipLook = true;
		}
	}
}

void EquipmentXiangQingBody::onEnter()
{
	EquipmentXiangQingBaseBody::onEnter();

	teachGuide();
}

void EquipmentXiangQingBody::teachGuide()
{
	if (GameGuideManager::getManager()->isGuideNow(GameGuideManager::GID_ChangeZhuangBei) )
	{
		//详细面板还要 介绍属性==
		//g_firstTaskEnd_to_changeEquip = false;

		//
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		CCMenuItem* item = (CCMenuItem*)mMenu->getChildByTag(TAG_ITEM_QUEDING);
		if(item != NULL)
		{
			guideLayer->init(item->getContentSize(), item->getParent()->convertToWorldSpace(item->getPosition())
				, LFStrings::getValue(""), NULL, false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
		GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_ChangeZhuangBei);
		return;
	}


	//第一次在面板看到传奇装备
	if ((mFrom == From_RenXingMianBan || mFrom == From_ZhuangBeiList )
		&& mEquip->getEquipColor() == ZhuangbeiColour_Chuanqi)
	{
		bool firstDisplay_ChuanQiEquip = false;
		if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstDisplayChuanQiEquip))
		{
			firstDisplay_ChuanQiEquip = true;
			//升级界面再设置  
			//History::getHistory()->putInCurrentHero("First_DisplayChuanQiEquip", false);
		}
		if (firstDisplay_ChuanQiEquip)
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstDisplayChuanQiEquip);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstDisplayChuanQiEquip);
			/*GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			CCMenuItem *item = (CCMenuItem*)mMenu->getChildByTag(TAG_ITEM_TAOZHUANG);
			if (item)
			{
				GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstDisplayChuanQiEquip);
				GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstDisplayChuanQiEquip);
				g_FirstTaoZhuangLook = true;
				guideLayer->init(
					item->getContentSize(), 
					item->getParent()->convertToWorldSpace(item->getPosition()), 
					LFStrings::getValue("chuanqi_ChuanQiTaoZhuangShuoMing"), NULL, false);
				CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);

			}*/

			CCMenuItem *qhItem = (CCMenuItem*)mGongJiangMenu->getChildByTag(Tag_Item_Qianghua);

			if (qhItem)
			{
				//GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstDisplayChuanQiEquip);
				//GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstDisplayChuanQiEquip);
				GuideLayerBase *guideLayer= new GuideLayer_SimpleLayer;
				guideLayer->init(
					qhItem->getContentSize(), 
					qhItem->getParent()->convertToWorldSpace(qhItem->getPosition()), 
					LFStrings::getValue("chuanqi_QiangHuaAnNiuShuoMing"), NULL, true);
				guideLayer->setChatEndBack(this,SEL_CallFunc(&EquipmentXiangQingBody::chatEndCallBack));
				CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);

				g_FirstChuanQiEquipLook = true;
			}
		}
		else
		{
			bool isFirstXilian = false;
			if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstLockXiLian))
			{
				CCMenuItem *item = (CCMenuItem*)mGongJiangMenu->getChildByTag(Tag_Item_Xilian);
				if (item)
				{
					GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstLockXiLian);
					GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstLockXiLian);
					//History::getHistory()->putInCurrentHero(Key_First_LookXilian,false);
					CCPoint pos = item->getParent()->convertToWorldSpace(item->getPosition());
					//pos.y += 115;
					GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
					guideLayer->init(item->getContentSize(),pos
						, LFStrings::getValue("first_look_chuanqi_shuoming"), NULL, false);
					CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
				}
				else
				{
					equipLevelUpGuide();
				}
				
			}
		}
	}

}
void EquipmentXiangQingBody::chatEndCallBack()
{
	bool isFirstXilian = false;
	if (!GameGuideManager::getManager()->isAlreadyGuide(GameGuideManager::GID_FirstLockXiLian))
	{
		CCMenuItem *item = (CCMenuItem*)mGongJiangMenu->getChildByTag(Tag_Item_Xilian);
		if (item)
		{
			GameGuideManager::getManager()->setOneGuideStart(GameGuideManager::GID_FirstLockXiLian);
			GameGuideManager::getManager()->setOneGuideEnd(GameGuideManager::GID_FirstLockXiLian);
			//History::getHistory()->putInCurrentHero(Key_First_LookXilian,false);
			CCPoint pos = item->getParent()->convertToWorldSpace(item->getPosition());
			//pos.y += 115;
			GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
			guideLayer->init(item->getContentSize(),pos
				, LFStrings::getValue("first_look_chuanqi_shuoming"), NULL, false);
			CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
		}
		else
		{
			equipLevelUpGuide();
		}

	}
}