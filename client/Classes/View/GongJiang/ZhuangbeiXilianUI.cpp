#include "ZhuangbeiXilianUI.h"
#include "font.h"
#include "FX_CommonFunc.h"
#include "SuitPool.h"
#include "ZhuangbeiXilianOption.h"
#include "UnblockManager.h"
#include "ZhuangbeiXilianOverUI.h"
#include "CS_Common.h"
#include "MyselfManager.h"
#include "WuPinManager.h"
#include "GameDirector.h"
#include "MessageDisplay.h"
#include "History.h"
#include "GuideLayerBase.h"
using namespace FX_CommonFunc;
const int Tag_Board_ShuoMing = 178;
#define POS_EqName				ccp(320, 803)

#define POS_ShuoMing				ccp(320, 803 - 65)
ZhuangbeiXilianUI::ZhuangbeiXilianUI()
	:BaseLayer(),
	m_nTodayFreeCount(0),
	m_nSelectIndex(-1),
	m_tFreeCountsTTF(NULL),
	m_tXilianJinghuaCountTTF(NULL),
	m_bIsRefreshXilian(false),
	m_tXilianJinghuaTTF(NULL),
	m_dInfoDialog(NULL)
{
}
ZhuangbeiXilianUI::~ZhuangbeiXilianUI()
{
}
CCScene* ZhuangbeiXilianUI::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	ZhuangbeiXilianUI *layer = ZhuangbeiXilianUI::createWithEquipment(NULL);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
ZhuangbeiXilianUI *ZhuangbeiXilianUI::createWithEquipment(Equipment *equipment)
{
	ZhuangbeiXilianUI *layer = new ZhuangbeiXilianUI();
	if(layer && layer->init(equipment))
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
bool ZhuangbeiXilianUI::init(Equipment *equipment)
{
	if (!BaseLayer::init())
	{
		return false;
	}
	mEquipment = equipment;

	CCSprite *backSpr = CCSprite::create("zhuangbei_xiangxi_xilian_bg.png");
	backSpr->setPosition(ccp(backSpr->getContentSize().width/2, backSpr->getContentSize().height/2));
	addChild(backSpr,-1);
	m_sBackSpr = backSpr;
	
	return true;
}
void ZhuangbeiXilianUI::onEnter()
{
	BaseLayer::onEnter();

	CCMenu *menu = CCMenu::create();
	menu->setAnchorPoint(CCPointZero);
	menu->setPosition(CCPointZero);
	addChild(menu);
	m_mMenu = menu;
	/*
	CCMenuItemImage *item = CCMenuItemImage::create("CloseNormal.png","CloseSelected.png",this,SEL_MenuHandler(&MainLayerUI::click));
	menu->addChild(item);
	item->setAnchorPoint(CCPointZero);
	item->setPosition(CCPointZero);
	*/
	{
		int max = 0;
		if (mEquipment->getEquipColor() == ZhuangbeiColour_Xiyou)
		{
			max = 23;
		}
		else if (mEquipment->getEquipColor() == ZhuangbeiColour_Chuanqi)
		{
			max = 50;
		}

		CCLabelTTF *titleTTF = CCLabelTTF::create();
		CCLabelTTF *ttf1 = CCLabelTTF::create(LFStrings::getValue("XilianFanwei").c_str(),fontStr_kaiti, 25.0f);
		ttf1->setColor(ccc3(195,145,96));
		ttf1->setPosition(ccp(ttf1->getContentSize().width / 2, ttf1->getContentSize().height / 2));
		//
		titleTTF->addChild(ttf1);

		CCLabelTTF *ttf2 = CCLabelTTF::create(CCString::createWithFormat(" %s1~%d%s%%",LFStrings::getValue("dakuohaoLeft").c_str(),max,LFStrings::getValue("dakuohaoRight").c_str())->getCString(),fontStr_kaiti, 25.0f);
		ttf2->setColor(ccc3(79,110,217));
		ttf2->setPosition(ccp(ttf2->getContentSize().width / 2 + ttf1->getContentSize().width, ttf2->getContentSize().height / 2));
		//ttf1->setPosition(ccp(this->getContentSize().width / 2,645));
		titleTTF->addChild(ttf2);
		titleTTF->setPosition(ccp(this->getContentSize().width / 2 - ( ttf1->getContentSize().width + ttf2->getContentSize().width ) / 2,655));
		addChild(titleTTF);
	}
	

	
	CCLabelTTF *nameTTF = CCLabelTTF::create(mEquipment->getName().c_str(),fontStr_kaiti, 30.0f);
	nameTTF->setPosition(POS_EqName);

	switch (mEquipment->getEquipColor())
	{
	case ZhuangbeiColour_Xiyou:
		{
			nameTTF->setColor(ccc3(253,180,40));
		}
		break;
	case ZhuangbeiColour_Chuanqi:
		{
			nameTTF->setColor(ccc3(181,93,6));
		}
		break;
	default:
		break;
	}
	
	m_sBackSpr->addChild(nameTTF);

	{
		CCMenuItemImage* closeItem = CCMenuItemImage::create(
			"close.png",
			"close_select.png",
			this,menu_selector(ZhuangbeiXilianUI::cilickClose)    
			);
		CCPoint pos = POS_EqName;
		closeItem->setPosition(ccp(m_sBackSpr->getContentSize().width - closeItem->getContentSize().width,pos.y));
		m_mMenu->addChild(closeItem);
	}
	

	CCSprite *shuomingSpr = CCSprite::create("zhuangbei_xiangxi_xilian_shuoming_1.png");

	shuomingSpr->setPosition(POS_ShuoMing);
	m_sBackSpr->addChild(shuomingSpr);

	bool isFirstIn = false;
	if (!History::getHistory()->getBoolFromCurrentHero(Key_First_LookXilianShuoming,isFirstIn))
	{
		History::getHistory()->putInCurrentHero(Key_First_LookXilianShuoming,false);
		CCPoint pos = shuomingSpr->getParent()->convertToWorldSpace(shuomingSpr->getPosition());
		GuideLayerBase *guideLayer = new GuideLayer_SimpleLayer;
		guideLayer->init(shuomingSpr->getContentSize(), pos
			, LFStrings::getValue("first_chuanqi_shuoming"), NULL, false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(guideLayer, 1024);
	}

	CCMenuItemImage *item = CCMenuItemImage::create("zhuangbei_xiangxi_xilian.png","zhuangbei_xiangxi_xilian_select.png",this,SEL_MenuHandler(&ZhuangbeiXilianUI::clickXilian));
	menu->addChild(item);
	item->setPosition(ccp(POS_ShuoMing.x,50));

	CCMenuItemImage *shuomingItem = CCMenuItemImage::create("xuesejiaotang_anniu_shuoming.png","xuesejiaotang_anniu_shuoming_select.png",this,SEL_MenuHandler(&ZhuangbeiXilianUI::clickShuoming));
	menu->addChild(shuomingItem);
	shuomingItem->setPosition(ccp(shuomingItem->getContentSize().width / 2 + 10,POS_ShuoMing.y));

	vector<ZBAttr> data_list = mEquipment->getEquipData().purify_attr_vec;

	int max_count = CS::getPurifyMaxHoles(mEquipment->getEquipData().zhuangbei_type,mEquipment->getEquipColor(),mEquipment->getLevel());
	int count = data_list.size();

	CCSprite *layer = CCSprite::create("zhuangbei_xiangxi_xilian_shuxing_bg.png");
	layer->setPosition(ccp(POS_EqName.x,465));
	m_sBackSpr->addChild(layer);
	int atter_type_index[5] = {AttrType_baoshang,AttrType_hp_percent,AttrType_skillresult,mEquipment->getMainProType(),0};

	float side = 0;
	{
		CCSprite *bgSpr = CCSprite::create("zhuangbei_xiangxi_xilian_shuxing.png");

		side = (layer->getContentSize().height - bgSpr->getContentSize().height * 5 ) / 10.0f;
	}
	
	m_sShuxingShowSpr = layer;
	for(int i=0; i!= max_count; ++i)
	{
		ZhuangbeiXilianOption *option = ZhuangbeiXilianOption::create();
		option->setCallBack(this,SEL_CallFuncN(&ZhuangbeiXilianUI::clickSelect));
		if (i < count)
		{
			option->setValue(i,CS::getPurifyLevel(data_list[i].attr_value),data_list[i].attr_value,atter_type_index[i]);
			m_uOptionList.push_back(option);
		}
		else
		{
			option->setValue(i,-1,-1,-1);
		}
		layer->addChild(option);
		option->setPosition(ccp(layer->getContentSize().width / 2,layer->getContentSize().height - (i * 2 + 1) * ( side + option->getContentSize().height / 2)));
		
	}
	if (m_uOptionList.size() == 1)
	{
		for (vector<ZhuangbeiXilianOption *>::iterator iter = m_uOptionList.begin() ; iter != m_uOptionList.end() ; iter ++)
		{
			(*iter)->selectSelf();
		}
	}
	m_nTodayFreeCount = MyselfManager::getManager()->getRuntionData()->purify_free_times;
	if (m_nTodayFreeCount <= 0)
	{
		CCSprite *bgSpr = CCSprite::create("daoju_kuang_bg.png");
		bgSpr->setPosition(ccp(this->getContentSize().width / 2 + 2,185));
		m_sBackSpr->addChild(bgSpr);

		CCMenuItemImage *XilianJinghuaItem = CCMenuItemImage::create("daoju_youlanzhiye.png","daoju_youlanzhiye.png",this,SEL_MenuHandler(&ZhuangbeiXilianUI::clickXilianJingHua));
		XilianJinghuaItem->setPosition(ccp(this->getContentSize().width / 2,185));
		m_mMenu->addChild(XilianJinghuaItem);
		/*CCSprite *spr = CCSprite::create("daoju_youlanzhiye.png");
		spr->setPosition(ccp(this->getContentSize().width / 2,185));
		m_sBackSpr->addChild(spr);*/

		int haveCount =  WuPinManager::getManager()->getWuPin(ItemsType_Purify_Scroll)->getCount();
		int count = 1;

		CCLabelTTF *strTTF = CCLabelTTF::create(LFStrings::getValue("XilianJinghua").c_str(),fontStr_kaiti, 20.0f);
		strTTF->setPosition(ccp(bgSpr->getPositionX() - bgSpr->getContentSize().width / 2 + strTTF->getContentSize().width / 2 - 30,115));
		m_sBackSpr->addChild(strTTF);
		strTTF->setColor(ccc3(255,255,255));

		CCLabelTTF *xuTTF = CCLabelTTF::create(CCString::createWithFormat(" %d / ",count)->getCString(),fontStr_kaiti, 20.0f);
		xuTTF->setPosition(ccp(strTTF->getPositionX() + strTTF->getContentSize().width / 2 + xuTTF->getContentSize().width / 2,115));
		m_sBackSpr->addChild(xuTTF);
		xuTTF->setColor(ccc3(255,255,255));

		CCLabelTTF *countTTF = CCLabelTTF::create(CCString::createWithFormat("%d",haveCount)->getCString(),fontStr_kaiti, 20.0f);
		countTTF->setPosition(ccp(xuTTF->getPositionX() + xuTTF->getContentSize().width / 2 + countTTF->getContentSize().width / 2,115));
		m_sBackSpr->addChild(countTTF);
		if (haveCount <= 0)
		{
			countTTF->setColor(ccRED);
		}

		

		m_tXilianJinghuaCountTTF = countTTF; 
	}
	else
	{
		m_tXilianJinghuaTTF = CCLabelTTF::create(LFStrings::getValue("Free").c_str(),fontStr_kaiti, 30.0f);
		m_tXilianJinghuaTTF->setPosition(ccp(POS_EqName.x,172));
		m_sBackSpr->addChild(m_tXilianJinghuaTTF);
		m_tXilianJinghuaTTF->setColor(ccGREEN);
	}
	CCLabelTTF *countTTF = CCLabelTTF::create(CCString::createWithFormat("%s %u",LFStrings::getValue("todayFreeCount").c_str(),m_nTodayFreeCount)->getCString(),fontStr_kaiti, 20.0f);
	countTTF->setPosition(ccp(POS_EqName.x,252));
	m_sBackSpr->addChild(countTTF);
	countTTF->setColor(ccc3(255,255,255));

	m_tFreeCountsTTF = countTTF;

	m_lData_list = data_list;
}
void ZhuangbeiXilianUI::clickXilian(CCObject *obj)
{
	if (m_nSelectIndex == -1)
	{
		UnblockWarnDialog *layer = UnblockWarnDialog::create(LFStrings::getValue("SelectXilianIndex").c_str(), false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
	}
	else
	{
		addObserver(callfuncO_selector(ZhuangbeiXilianUI::getMessage), MSG_PurifyRsp);
		mCmdHlp->cmdPurify(mEquipment->getEquipId(),m_nSelectIndex + 1);
	}
}
void ZhuangbeiXilianUI::getMessage(CCObject *obj)
{
	removeObserver(MSG_PurifyRsp);
	GameDirector::getDirector()->hideWaiting();
	
	Flat_PurifyRsp* rsp = (Flat_PurifyRsp*)((Flat_MSG_Rsp*)obj)->getData();
	if (rsp->err_code != Success)
	{
		MessageDisplay* layer = MessageDisplay::create(mCmdHlp->getMessage(rsp->err_code));
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		
		return;
	}

	m_lXilianData_list = rsp->zbAttrVec;
	refreshShowUI(m_lData_list,m_lXilianData_list);
	/*
	ZhuangbeiXilianOverUI *layer = ZhuangbeiXilianOverUI::createWithEquipment(mEquipment);
	layer->setOldData(m_lData_list,rsp->zbAttrVec);
	layer->setZhuangbeiId(mEquipment->getEquipId());
	mFooterLayer->popUpContaintLayer(layer);*/
}
void ZhuangbeiXilianUI::clickSelect(CCNode *node)
{
	m_nSelectIndex = node->getTag();

	for (vector<ZhuangbeiXilianOption *>::iterator iter = m_uOptionList.begin() ; iter != m_uOptionList.end() ; iter ++)
	{
		(*iter)->selectOther(m_nSelectIndex);
	}
}
void ZhuangbeiXilianUI::refreshShowUI(vector<ZBAttr> old_list,vector<ZBAttr> list)
{


	
	if (list.size() > old_list.size())
	{
		
		m_bIsRefreshXilian = true;
		addObserver(callfuncO_selector(ZhuangbeiXilianUI::getQueRenMessage), MSG_ConfirmPurifyRsp);
		mCmdHlp->cmdConfirmPurify(mEquipment->getEquipId(),true);
	}
	else if (list[m_nSelectIndex].attr_value >old_list[m_nSelectIndex].attr_value)
	{
		
		m_bIsRefreshXilian = true;
		addObserver(callfuncO_selector(ZhuangbeiXilianUI::getQueRenMessage), MSG_ConfirmPurifyRsp);
		mCmdHlp->cmdConfirmPurify(mEquipment->getEquipId(),true);
	}
	else
	{
		
		
		m_bIsRefreshXilian = false;
		addObserver(callfuncO_selector(ZhuangbeiXilianUI::getQueRenMessage), MSG_ConfirmPurifyRsp);
		mCmdHlp->cmdConfirmPurify(mEquipment->getEquipId(),false);
	}
}
void ZhuangbeiXilianUI::addShuxingShow(vector<ZBAttr> old_list,vector<ZBAttr> list)
{
	if (m_bIsRefreshXilian)
	{
		m_uOptionList.clear();
		m_sShuxingShowSpr->removeAllChildren();

		int old_count = old_list.size();
		int count = list.size();
		int atter_type_index[5] = {AttrType_baoshang,AttrType_hp_percent,AttrType_skillresult,mEquipment->getMainProType(),0};
		float side = 0;
		{
			CCSprite *bgSpr = CCSprite::create("zhuangbei_xiangxi_xilian_shuxing.png");

			side = (m_sShuxingShowSpr->getContentSize().height - bgSpr->getContentSize().height * 5 ) / 10.0f;
		}
		int max_count = CS::getPurifyMaxHoles(mEquipment->getEquipData().zhuangbei_type,mEquipment->getEquipColor(),mEquipment->getLevel());
		for(int i=0; i!= max_count; ++i)
		{
			ZhuangbeiXilianOption *option = ZhuangbeiXilianOption::create();
			option->setCallBack(this,SEL_CallFuncN(&ZhuangbeiXilianUI::clickSelect));
			if (i < count)
			{
				if ((i < old_count && ( ( CS::getPurifyLevel(list[i].attr_value) + 1 ) / 2 > ( CS::getPurifyLevel(old_list[i].attr_value) + 1 ) / 2 )))
				{
					option->setLevelSheng();
				}

				if ((i < old_count && ( list[i].attr_value > old_list[i].attr_value)))
				{
					option->setValueSheng();
				}
				option->setValue(i,CS::getPurifyLevel(list[i].attr_value),list[i].attr_value,atter_type_index[i]);
				m_uOptionList.push_back(option);
			}
			else
			{
				option->setValue(i,-1,-1,-1);
			}
			m_sShuxingShowSpr->addChild(option);
			option->setPosition(ccp(m_sShuxingShowSpr->getContentSize().width / 2,m_sShuxingShowSpr->getContentSize().height - (i * 2 + 1) * ( side + option->getContentSize().height / 2)));
			if (i >= old_count && i < count)
			{
				option->addNewShuxing();
			}

			if (i == ( m_nSelectIndex ))
			{
				option->selectSelf();
			}
		}
		m_bIsRefreshXilian = false;
	}
	m_nTodayFreeCount = MyselfManager::getManager()->getRuntionData()->purify_free_times;
	if (m_nTodayFreeCount <= 0)
	{
		if (m_tXilianJinghuaTTF)
		{
			m_tXilianJinghuaTTF->removeFromParent();
			m_tXilianJinghuaTTF = NULL;
		}
		if (m_tXilianJinghuaCountTTF)
		{
			int haveCount =  WuPinManager::getManager()->getWuPin(ItemsType_Purify_Scroll)->getCount();
			int count = 1;
			m_tXilianJinghuaCountTTF->setString(CCString::createWithFormat(" %d",haveCount)->getCString());

			if (haveCount <= 0)
			{
				m_tXilianJinghuaCountTTF->setColor(ccRED);
			}
		}
		else
		{
			CCSprite *bgSpr = CCSprite::create("daoju_kuang_bg.png");
			bgSpr->setPosition(ccp(this->getContentSize().width / 2 + 2,185));
			m_sBackSpr->addChild(bgSpr);

			CCMenuItemImage *XilianJinghuaItem = CCMenuItemImage::create("daoju_youlanzhiye.png","daoju_youlanzhiye.png",this,SEL_MenuHandler(&ZhuangbeiXilianUI::clickXilianJingHua));
			XilianJinghuaItem->setPosition(ccp(this->getContentSize().width / 2,185));
			m_mMenu->addChild(XilianJinghuaItem);

			/*CCSprite *spr = CCSprite::create("daoju_youlanzhiye.png");
			spr->setPosition(ccp(this->getContentSize().width / 2,185));
			m_sBackSpr->addChild(spr);*/

			int haveCount =  WuPinManager::getManager()->getWuPin(ItemsType_Purify_Scroll)->getCount();
			int count = 1;

			CCLabelTTF *strTTF = CCLabelTTF::create(LFStrings::getValue("XilianJinghua").c_str(),fontStr_kaiti, 20.0f);
			strTTF->setPosition(ccp(bgSpr->getPositionX() - bgSpr->getContentSize().width / 2 - 30 + strTTF->getContentSize().width / 2,115));
			m_sBackSpr->addChild(strTTF);
			strTTF->setColor(ccc3(255,255,255));


			CCLabelTTF *xuTTF = CCLabelTTF::create(CCString::createWithFormat(" %d / ",count)->getCString(),fontStr_kaiti, 20.0f);
			xuTTF->setPosition(ccp(strTTF->getPositionX() + strTTF->getContentSize().width / 2 + xuTTF->getContentSize().width / 2,115));
			m_sBackSpr->addChild(xuTTF);
			xuTTF->setColor(ccc3(255,255,255));
			

			CCLabelTTF *countTTF = CCLabelTTF::create(CCString::createWithFormat("%d",haveCount)->getCString(),fontStr_kaiti, 20.0f);
			countTTF->setPosition(ccp(xuTTF->getPositionX() + xuTTF->getContentSize().width / 2 + countTTF->getContentSize().width / 2,115));
			m_sBackSpr->addChild(countTTF);
			if (haveCount <= 0)
			{
				countTTF->setColor(ccRED);
			}

			m_tXilianJinghuaCountTTF = countTTF;
		}
	}
	

	if (m_tFreeCountsTTF)
	{
		m_tFreeCountsTTF->setString(CCString::createWithFormat("%s %u",LFStrings::getValue("todayFreeCount").c_str(),m_nTodayFreeCount)->getCString());
	}
	else
	{
		CCLabelTTF *countTTF = CCLabelTTF::create(CCString::createWithFormat("%s %u",LFStrings::getValue("todayFreeCount").c_str(),m_nTodayFreeCount)->getCString(),fontStr_kaiti, 20.0f);
		countTTF->setPosition(ccp(POS_EqName.x,252));
		m_sBackSpr->addChild(countTTF);
		countTTF->setColor(ccc3(255,255,255));
		m_tFreeCountsTTF = countTTF;
	}
	
}
void ZhuangbeiXilianUI::getQueRenMessage(CCObject *obj)
{
	removeObserver(MSG_ConfirmPurifyRsp);
	GameDirector::getDirector()->hideWaiting();
	bool isRefresh = m_bIsRefreshXilian;
	addShuxingShow(m_lData_list,m_lXilianData_list);
	if (isRefresh)
	{
		
		m_lData_list.clear();
		m_lData_list = m_lXilianData_list;
		
	}
	else
	{
		CCString *str = CCString::createWithFormat("%s+%d %s",LFStrings::getValue("XilianJieguo").c_str(),m_lXilianData_list[m_nSelectIndex].attr_value,LFStrings::getValue("ShuxingXiajiang").c_str());
		UnblockWarnDialog *layer = UnblockWarnDialog::create(str->getCString(), false);
		CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
		m_lXilianData_list.clear();
	}
}
void ZhuangbeiXilianUI::clickShuoming(CCObject *obj)
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
			, this, menu_selector(ZhuangbeiXilianUI::menuItemClicked_ShuoMingMenu));
		menu->addChild(fanhui, 1, 20);
		fanhui->setPosition(ccp(dialog->getContentSize().width/2,70));
	}
	//content
	{
		CCSize size = dialog->getContentSize();
		//title
		CCLabelTTF *titleLabel = CCLabelTTF::create(LFStrings::getValue("XilianTitle").c_str(), fontStr_kaiti, 20);
		titleLabel->setHorizontalAlignment(kCCTextAlignmentCenter);
		dialog->addChild(titleLabel);
		titleLabel->setAnchorPoint(ccp(0.5,1));
		titleLabel->setPosition(ccp(size.width/2, size.height-56));
		CCLabelTTF *shuoMingLabel = CCLabelTTF::create(LFStrings::getValue("XilianInfo").c_str(), fontStr_kaiti, 20 );
		
		shuoMingLabel->setHorizontalAlignment(kCCTextAlignmentRight);
		dialog->addChild(shuoMingLabel);
		shuoMingLabel->setColor(fonColor_PuTong);
		shuoMingLabel->setAnchorPoint(ccp(0.5,1));
		shuoMingLabel->setPosition(ccp(size.width/2, size.height-110));


		CCPoint pos = shuoMingLabel->getPosition();
		pos.x += shuoMingLabel->getContentSize().width / 2 - 45;
		pos.y += 5;

		int side_y = 20;
		for (int i = 0 ; i < 4 ; i ++)
		{
			BaseSprite *xingxingImg = BaseSprite::create(CCString::createWithFormat("gongming_anniu_%d.png",i + 1)->getCString());
			dialog->addChild(xingxingImg);

			CCPoint point;
			point.x = pos.x - xingxingImg->getContentSize().width / 2 * 0.38f;
			point.y = pos.y - i * side_y - xingxingImg->getContentSize().height / 2 * 0.38f;
			xingxingImg->setPosition(point);

			xingxingImg->setScale(0.38f);
		}
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
void ZhuangbeiXilianUI::menuItemClicked_ShuoMingMenu(CCObject *pSender)
{
	CCDirector::sharedDirector()->getRunningScene()->removeChildByTag(Tag_Board_ShuoMing);
}
void ZhuangbeiXilianUI::cilickClose(CCObject *obj)
{
	MainLayerBase* footer = mFooterLayer;
	footer->dismmisPopUp();
	BaseLayer* topLayer = footer->getTopLayer();
	if(topLayer != NULL)
	{
		topLayer->refreshLayer();
	}
	else
	{
		footer->getContentLayer()->refreshLayer();
	}
}
void ZhuangbeiXilianUI::clickXilianJingHua(CCObject *obj)
{
	if (m_dInfoDialog)
	{
		m_dInfoDialog->removeFromParent();
		m_dInfoDialog = NULL;
	}
	string tishiStr = LFStrings::getValue("XilianJingHuaShuoMing");
	string cailiaoFileStr = "daoju_youlanzhiye.png";

	//弹框背景
	BaseSprite* dialog = BaseSprite::create("gongjiang_duanzao_cailiaobuzu_tanchukuang.png");
	dialog->setPosition(ccp(mWinSize.width/2, mWinSize.height/2 + 50));
	//提示
	{
		BaseSprite *cailiaoBg = BaseSprite::create("daoju_kuang_bg.png");
		dialog->addChild(cailiaoBg);
		cailiaoBg->setPosition(ccp(cailiaoBg->getContentSize().width/2 + 10, dialog->getContentSize().height/2));
		BaseSprite *cailiaoImg = BaseSprite::create(cailiaoFileStr);
		cailiaoBg->addChild(cailiaoImg);
		cailiaoImg->setPosition(ccp(cailiaoBg->getContentSize().width/2, cailiaoBg->getContentSize().height/2));

		CCLabelTTF* lable = CCLabelTTF::create(tishiStr.c_str(), fontStr_kaiti,20,CCSize(240, 0), kCCTextAlignmentLeft);
		dialog->addChild(lable);
		lable->setAnchorPoint(ccp(0,0.5));
		lable->setPosition(ccp(cailiaoBg->getPositionX()+cailiaoBg->getContentSize().width/2 , dialog->getContentSize().height/2));
	}
	//按键
	CCMenu* menu = CCMenu::create();
	{
		CCMenuItemImage* item = CCMenuItemImage::create(
			"gongjiang_duanzao_cailiaobuzu_tanchukuang.png",
			"gongjiang_duanzao_cailiaobuzu_tanchukuang.png",
			this, menu_selector(ZhuangbeiXilianUI::clickCiaoliaoShuoMing));
		menu->addChild(item);
		item->setPosition(ccp(dialog->getContentSize().width/2, dialog->getContentSize().height/2));
		item->setOpacity(0);
		item->setScaleX(mWinSize.width / dialog->getContentSize().width);
		item->setScaleY(mWinSize.height / dialog->getContentSize().height);
		// 		item->setScaleX(mWinSize.width / item->getContentSize().width);
		// 		item->setScaleY(mWinSize.height / item->getContentSize().height);
	}
	//弹框
	{

		LFAlert* lfAlert = new LFAlert();
		lfAlert->setBaseContainer(dialog,menu);
		CCDirector::sharedDirector()->getRunningScene()->addChild(lfAlert,128);
		lfAlert->release();
		m_dInfoDialog = lfAlert;
	}
}