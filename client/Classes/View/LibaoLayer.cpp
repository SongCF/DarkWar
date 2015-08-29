#include "LibaoLayer.h"
#include "font.h"
#include "MyselfManager.h"
#include "ImageManager.h"
#include "FXScrollMenu.h"
#include "MarketManager.h"
#include "ImageManager.h"
#include "GameDirector.h"
#include "SystemGiftBagManager.h"
#include "MessageDisplay.h"
#include "WuPinManager.h"
#include "MainLayerStoreFrame.h"
#include "FXToast.h"
#include "AwardDisplay.h"
#define LabelSize 20

enum 
{
	Tag_TitleBar = 1,
	Tag_Label_NextVipNeed,   //再冲6元享受、、、
	Tag_Label_NextVip,

	Tag_ShouChongBar,

	Tag_CellBoard,
	Tag_Menu,
	Tag_MenuItem_Li_Qu,
};

LibaoLayer::LibaoLayer(MainLayerBase* footerLayer, CCSize contentSize)
{

    mContentHeight = contentSize.height;
	mFooterLayer = footerLayer;
	isgold = true;
	tabelViewHeight = contentSize.height-20;
	
	updateDataMarkitem(1);
	//下一帧创建
	if(GmLb.size()>0)
	  this->schedule( schedule_selector(LibaoLayer::createTable));

   /*
   AwardDisplay *layer = AwardDisplay::create("cs");
   CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
   */

}

void LibaoLayer::disableAllTouchBegin()
{
	mTableView->setTouchEnabled(false);
}
void LibaoLayer::disableAllTouchEnd()
{
	mTableView->setTouchEnabled(true);
}



LibaoLayer::~LibaoLayer(void)
{

}


CCSize LibaoLayer::tableCellSizeForIndex(CCTableView *table, unsigned int idx){

	
	return CCSizeMake(613, 144);

}


CCTableViewCell* LibaoLayer::tableCellAtIndex(CCTableView *table, unsigned int idx){

	
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

	

	if (table == mTableView)
	{

		vector<ChargePacketInfo> chargeVec = MyselfManager::getManager()->getChargePacketInfo();

		BaseSprite* board = BaseSprite::create("shangdian_shangpin_bg.png");
		cell->addChild(board, 0, Tag_CellBoard);
		board->setPosition(ccp(board->getContentSize().width/2, board->getContentSize().height/2));

		//libao img
		CCLOG("--------->%d",GmLb[idx].sell_item.gift.id);
		BaseSprite *LbImg = BaseSprite::create(ImageManager::getManager()->getGiftBagFileName(GmLb[idx].sell_item.gift.id));
		board->addChild(LbImg,2);
		LbImg->setPosition(ccp(72, board->getContentSize().height/2));

		//礼包背景
		BaseSprite *LbImgBg = BaseSprite::create(ImageManager::getManager()->getGiftBagBoardName(GmLb[idx].sell_item.gift.id));
		board->addChild(LbImgBg,1);
		LbImgBg->setPosition(ccp(72, board->getContentSize().height/2));


		//获取礼包名
		std::string lbm=SystemGiftBagManager::getManager()->getGiftBagName(GmLb[idx].sell_item.gift.id);
		CCLabelTTF *LbmLabel = CCLabelTTF::create(lbm.c_str(), fontStr_kaiti, LabelSize, CCSize(330, 0), kCCTextAlignmentLeft);
		board->addChild(LbmLabel);
		LbmLabel->setAnchorPoint(ccp(0, 1));
		LbmLabel->setPosition(ccp(142, 148-32));

		//获取礼包描述
		//
		std::string lbms=SystemGiftBagManager::getManager()->getGiftBagDescribe(GmLb[idx].sell_item.gift.id);
		CCLabelTTF *LbmsLabel = CCLabelTTF::create(lbms.c_str(), fontStr_kaiti, LabelSize, CCSize(330, 0), kCCTextAlignmentLeft);
		board->addChild(LbmsLabel);
		LbmsLabel->setAnchorPoint(ccp(0, 0.5));
		LbmsLabel->setPosition(ccp(LbmLabel->getPositionX(), 
			LbmLabel->getPositionY()-LbmLabel->getContentSize().height-LbmsLabel->getContentSize().height/2-5));
		LbmsLabel->setColor(ccc3(195,145,96));


		//可购买次数
		int hscs=0;
		int sale_time=GmLb[idx].sale_time;
		std::string buynum="";
		if(GmLb[idx].is_unlimit==true){
			buynum=LFStrings::getValue("goumaicishu")+LFStrings::getValue("wuxian");
			hscs=10;
		}else{
		    hscs=GmLb[idx].sell_count-GmLb[idx].bought_times;
			std::string strhscs=CCString::createWithFormat("%d",hscs)->getCString();
		    buynum=LFStrings::getValue("goumaicishu").c_str();
			buynum=buynum+strhscs;
		}

		CCLabelTTF *buynumlabel = CCLabelTTF::create(buynum.c_str(), fontStr_kaiti, LabelSize);
		board->addChild(buynumlabel);
		buynumlabel->setColor(ccc3(89,111,183));
		buynumlabel->setAnchorPoint(ccp(0, 0.5f));
		buynumlabel->setPosition(ccp(142,32));
		buynumap[GmLb[idx].id]=buynumlabel;

		FXScrollMenu *menu = FXScrollMenu::create(
			mTableView->getParent()->convertToWorldSpace(mTableView->getPosition()), mTableView->getViewSize());
		board->addChild(menu, 0, Tag_Menu);
		menu->setPosition(CCPointZero);

		CCMenuItemImage *item= CCMenuItemImage::create(
				"shangdian_goumai.png"
				, "shangdian_goumai_select.png"
				, this, menu_selector(LibaoLayer::menuItemClicked_Li_Qu));;
		item->setPosition(ccp(534, 148-70));
		item->setUserData((void*)idx);


		

        


		int viplevel=MyselfManager::getManager()->getCurrentVipLevel();
		int jueselevel=MyselfManager::getManager()->getMyZhuJueData()->getLevel();


		if((hscs>0&&isguoleTime(GmLb[idx].sale_time)==true)||(hscs>0&&sale_time==0)){


			if(GmLb[idx].level_type==0&&GmLb[idx].level<=viplevel)
		        menu->addChild(item, 0, Tag_MenuItem_Li_Qu);
			if(GmLb[idx].level_type==1&&GmLb[idx].level<=jueselevel)
				menu->addChild(item, 0, Tag_MenuItem_Li_Qu);

		}


		//限时
		string xiangshi=LF::getString("xianshi1");
		if(sale_time==0){
			   xiangshi=xiangshi+LF::getString("wuxianshi");
		}else{
			   xiangshi=xiangshi+getXiaoshitime(GmLb[idx].sale_time);
			   CCLabelTTF *xianshiLabel = CCLabelTTF::create(xiangshi.c_str(), fontStr_kaiti, 18, CCSize(330, 0), kCCTextAlignmentLeft);
		       board->addChild(xianshiLabel);
		       xianshiLabel->setPosition((ccp(item->getPositionX()-item->getContentSize().width/2-130+xianshiLabel->getContentSize().width/2,
			   item->getPositionY()+item->getContentSize().height/2+20-xianshiLabel->getContentSize().height/2)));
		       xianshiLabel->setColor(ccc3(245,0,0));
		}
        
		



		BaseSprite *spendsprite;
		if(GmLb[idx].need_type==AttachType_Gold){
			spendsprite = BaseSprite::create("jinbi_tubiao.png");
			
		}else if(GmLb[idx].need_type==AttachType_Silver){
			spendsprite = BaseSprite::create("yinbi_tubiao.png");
			
		}else{
			CCAssert(true,"NoYinbiHuoJinbi->TYPE!");
		}
		board->addChild(spendsprite);
		spendsprite->setScale(0.8);
		spendsprite->setPosition(ccp(item->getPositionX()-item->getContentSize().width/2,buynumlabel->getPositionY()));


		int Spend=GmLb[idx].need_item.item.count;
		std::string strspend=CCString::createWithFormat("%d",Spend)->getCString();
		CCLabelTTF *spendlabel = CCLabelTTF::create(strspend.c_str(), fontStr_kaiti, LabelSize);
		spendlabel->setPosition(ccp(spendsprite->getPositionX()+spendsprite->getContentSize().width/2+spendlabel->getContentSize().width/2,spendsprite->getPositionY()));
		board->addChild(spendlabel);

	}
	
	cellmap[GmLb[idx].id]=cell;

	return cell;
}


unsigned int LibaoLayer::numberOfCellsInTableView(CCTableView *table){


	
	return GmLb.size();

}



void LibaoLayer::menuItemClicked_Li_Qu(CCObject *pSender){

	
	unsigned int vipId = (unsigned int)((CCMenuItemImage*)pSender)->getUserData();
	p=(CCMenuItemImage*)pSender;
	Gmid=vipId;
	
	{
		addObserver(callfuncO_selector(LibaoLayer::rsp_ChargeGold),MSG_buyItemInMarketRsp);
		SPCmd_BuyItemInMarket data;
		data.id=GmLb[vipId].id;
		data.buy_times=1;
		CmdHelper::getHelper()->cmdBuyItemInMarket(data);
	}

	
}


void LibaoLayer::rsp_ChargeGold(CCObject *msg)
{

	GameDirector::getDirector()->hideWaiting();
	removeObserver(MSG_buyItemInMarketRsp);

	SPCmd_BuyItemInMarketRsp* data = (SPCmd_BuyItemInMarketRsp*)((MSG_Rsp*)msg)->getData();
	if(data->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(data->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		return;
	}

	string temp = LFStrings::getValue("goumai") + LFStrings::getValue("chenggong");
	FXToast* layer = FXToast::create(temp);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer);

				//刷新金币数量
			  {
				int gold = 0;
				WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Gold);
				if (wp != NULL)
				{
					gold = wp->getCount();
				}

				BaseSprite* header = mFooterLayer->getHeader()->getHeaderContent();
				CCLabelTTF* lable = (CCLabelTTF*)header->getChildByTag(STORE_FRAM_TAG_HEAD_JINBI);
				char str[64];
				sprintf(str, "%d", gold);
				lable->setString(str);
				mYuanShiJinBi = gold;
			  }


	
			//刷新银币数量
			    {
					int slip = 0;
					WuPin* wp = WuPinManager::getManager()->getWuPin(ItemsType_Silver);
					if (wp != NULL)
					{
						slip = wp->getCount();
					}

					BaseSprite* header = mFooterLayer->getHeader()->getHeaderContent();
					CCLabelTTF* lable = (CCLabelTTF*)header->getChildByTag(STORE_FRAM_TAG_HEAD_YINBI);
					char str[64];
					sprintf(str, "%d", slip);
					lable->setString(str);
			  }
		
    updateDataMarkitem(0);
	//数据更新.... 
    
	
	
}


void LibaoLayer::createTable(float time){

	this->unschedule(schedule_selector(LibaoLayer::createTable));
	mTableView = CCTableView::create(this, CCSizeMake(613,tabelViewHeight));
	addChild(mTableView);
	mTableView->setDirection(kCCScrollViewDirectionVertical);
	mTableView->setPosition(ccp(13.5,33));
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
	

}

bool  LibaoLayer::isguoleTime(int saletime){

	
	  bool isguoshi=false;
	  long timecha=0;

	  #if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	       time_t timep;  
	       time(&timep);  
		   timecha = timep;
      #else
	      struct cc_timeval now;    
		  CCTime::gettimeofdayCocos2d(&now, NULL);    
		  timecha =now.tv_sec;
	  #endif

	  if(saletime-timecha>0)
	  {
		  isguoshi=true;
	  }

      return isguoshi;
	  
}


string LibaoLayer::getXiaoshitime(int saletime){

	
	string time="";
	struct tm *tm;  
	
    #if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	    struct cc_timeval now;   
	    now.tv_sec = saletime;
		time_t timep = now.tv_sec;  
		tm = localtime(&timep); 
    #else
	    struct cc_timeval now;    
		//CCTime::gettimeofdayCocos2d(&now, NULL);    
		now.tv_sec=saletime;
	    tm = localtime(&now.tv_sec); 
    #endif
 
	int year = tm->tm_year + 1900;  
	int month = tm->tm_mon + 1;   
	int day = tm->tm_mday;   
	int hour=tm->tm_hour;   
	int minute=tm->tm_min;   
	int second=tm->tm_sec; 
	time=CCString::createWithFormat("%d-%d-%d %d:%d:%d",year,month,day,hour,minute,second)->getCString();
	return time;
}


void LibaoLayer::updateDataMarkitem(int isfirst){

	int gmid1=Gmid;
	if(GmLb.size()>0){
		GmLb.clear();
	}

	vector<MarketItem> vec = MarketManager::getManager()->getAllItems();
	vector<MarketItem> vec1;
	for(int i=0;i<vec.size();i++){
		MarketItem marketitem=vec[i];
		if(marketitem.sell_type==8){
			vec1.push_back(marketitem);
		}
	}
	int num=vec1.size();
	//购买礼包id从小到大排序
	for(int i=1;i<=num;i++){
		  int index=0; 
		  int minux=0;
		  for(int j=0;j<vec1.size();j++){
			  MarketItem mi=vec1[j];
			  if(j==0)
			     minux=mi.id;
			  if(mi.id<=minux){
				  minux=mi.id;
				  index=j;
			  }
		  }
		  GmLb.push_back(vec1[index]);
		  std::vector<MarketItem>::iterator it = vec1.begin()+index; 
		  vec1.erase(it);
	}
	
	
			for(int i=0;i<GmLb.size();i++){
				//可购买次数
				int hscs=0;
				int sale_time=GmLb[i].sale_time;
				std::string buynum="";
				if(GmLb[i].is_unlimit==true){
					buynum=LFStrings::getValue("goumaicishu")+LFStrings::getValue("wuxian");
					hscs=10;
				}else{
					hscs=GmLb[i].sell_count-GmLb[i].bought_times;
					std::string strhscs=CCString::createWithFormat("%d",hscs)->getCString();
					buynum=LFStrings::getValue("goumaicishu").c_str();
					buynum=buynum+strhscs;
				}
				int viplevel=MyselfManager::getManager()->getCurrentVipLevel();
				int jueselevel=MyselfManager::getManager()->getMyZhuJueData()->getLevel();

				if((hscs>0&&isguoleTime(GmLb[i].sale_time)==true)||(hscs>0&&sale_time==0)){   
						if(GmLb[i].level_type==0){
							if(GmLb[i].level>viplevel){
								  std::vector<MarketItem>::iterator it = GmLb.begin()+i; 
								  GmLb.erase(it);
								  i--;
						
							}
						}else{
							if(GmLb[i].level>jueselevel){
								   std::vector<MarketItem>::iterator it = GmLb.begin()+i; 
								   GmLb.erase(it);
								   i--;
							}
						}

						if(isfirst==0&&i==Gmid){
							          if(GmLb.size()>0&&Gmid>0){
										    
											int opratebuy=0;
											opratebuy=Gmid;
											int buytype=GmLb[opratebuy].need_type;
											int Spend=GmLb[opratebuy].need_item.item.count;
											//更新购买次数
											{
														std::string buynum="";
														if(GmLb[opratebuy].is_unlimit==true){
															buynum==LFStrings::getValue("goumaicishu")+LFStrings::getValue("wuxian");
															
														}else{
															int hscs=GmLb[opratebuy].sell_count-GmLb[opratebuy].bought_times;
															std::string strhscs=CCString::createWithFormat("%d",hscs)->getCString();
															buynum=LFStrings::getValue("goumaicishu").c_str();
															buynum=buynum+strhscs;
															if(hscs==0&&p!=NULL){
																p->setVisible(false);
															}
															CCLabelTTF *buynumlabel=buynumap[GmLb[opratebuy].id];
														    buynumlabel->setString(buynum.c_str());
														}
														
											}

								   }


						}
				}else{

						std::vector<MarketItem>::iterator it = GmLb.begin()+i; 
						GmLb.erase(it);
						if(isfirst==0&&i==Gmid&&hscs==0){
							Gmid=-1;
					    }
						i--;
				}
		
			}
	

			if(isfirst==0&&Gmid==-1&&GmLb.size()>0){
				mTableView->reloadData();
			}
			if(isfirst==0&&Gmid==-1&&GmLb.size()==0){
				this->removeChild(mTableView);
			}



}