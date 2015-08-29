#include "ZhuangbeiXilianOverUI.h"
#include "font.h"
#include "FX_CommonFunc.h"
#include "SuitPool.h"
#include "ZhuangbeiXilianOverOption.h"
#include "UnblockManager.h"
#include "CS_Common.h"
#include "GameDirector.h"
#include "ZhuangbeiXilianUI.h"
using namespace FX_CommonFunc;
#define POS_EqName				ccp(320, 803)

#define POS_ShuoMing				ccp(320, 803 - 65)
ZhuangbeiXilianOverUI::ZhuangbeiXilianOverUI()
	:BaseLayer()
{
}
ZhuangbeiXilianOverUI::~ZhuangbeiXilianOverUI()
{
}
CCScene* ZhuangbeiXilianOverUI::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	ZhuangbeiXilianOverUI *layer = ZhuangbeiXilianOverUI::createWithEquipment(NULL);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
ZhuangbeiXilianOverUI *ZhuangbeiXilianOverUI::createWithEquipment(Equipment *equipment)
{
	ZhuangbeiXilianOverUI *layer = new ZhuangbeiXilianOverUI();
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
bool ZhuangbeiXilianOverUI::init(Equipment *equipment)
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
void ZhuangbeiXilianOverUI::onEnter()
{
	BaseLayer::onEnter();
	CCMenu *menu = CCMenu::create();
	menu->setAnchorPoint(CCPointZero);
	menu->setPosition(CCPointZero);
	addChild(menu);
	m_mMenu = menu;
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

	CCSprite *shuomingSpr = CCSprite::create("zhuangbei_xiangxi_xilian_shuoming_3.png");

	shuomingSpr->setPosition(POS_ShuoMing);
	m_sBackSpr->addChild(shuomingSpr);

	CCSprite *shuoming2Spr = CCSprite::create("zhuangbei_xiangxi_xilian_shuoming_2.png");

	shuoming2Spr->setPosition(ccp(this->getContentSize().width / 2, 200));
	m_sBackSpr->addChild(shuoming2Spr);

	CCMenuItemImage *item = CCMenuItemImage::create("shangpin_goumai_quxiao.png","shangpin_goumai_quxiao_select.png",this,SEL_MenuHandler(&ZhuangbeiXilianOverUI::clickSelectCancel));
	menu->addChild(item);
	item->setPosition(ccp(212,50));

	CCMenuItemImage *cancelitem = CCMenuItemImage::create("zhuangbei_xiangxi_queding.png","zhuangbei_xiangxi_queding_select.png",this,SEL_MenuHandler(&ZhuangbeiXilianOverUI::clickSelectOk));
	menu->addChild(cancelitem);
	cancelitem->setPosition(ccp(424,50));

	{
		CCSprite *spr = CCSprite::create("sphc_anniu_fanye.png");
		spr->setPosition(ccp(m_sBackSpr->getContentSize().width / 2, m_sBackSpr->getContentSize().height / 2));
		m_sBackSpr->addChild(spr);
	}
	

	int max_count = CS::getPurifyMaxHoles(mEquipment->getEquipData().zhuangbei_type,mEquipment->getEquipColor(),mEquipment->getLevel());

	{
		CCSprite *layer = CCSprite::create("zhuangbei_xiangxi_xilian_shuxing_bg.png");
		layer->setPosition(ccp(10 + layer->getContentSize().width / 2,465));
		m_sBackSpr->addChild(layer);

		CCSprite *spr = CCSprite::create("zhuangbei_xiangxi_xilian_xilianqian.png");
		spr->setPosition(ccp(10 + layer->getContentSize().width / 2,465 + layer->getContentSize().height / 2 + spr->getContentSize().height / 2));
		addChild(spr);

		int atter_type_index[5] = {AttrType_baoshang,AttrType_hp_percent,AttrType_skillresult,mEquipment->getMainProType(),0};

		float side = 0;
		{
			CCSprite *bgSpr = CCSprite::create("zhuangbei_xiangxi_xilian_shuxing.png");

			side = (layer->getContentSize().height - bgSpr->getContentSize().height * 5 ) / 10.0f;
		}
		int old_count = m_lOldData_list.size();
		

		for(int i=0; i!= max_count; ++i)
		{
			ZhuangbeiXilianOverOption *option = ZhuangbeiXilianOverOption::create();
			if (i < old_count)
			{
				option->setValue(CS::getPurifyLevel(m_lOldData_list[i].attr_value),m_lOldData_list[i].attr_value,atter_type_index[i]);
			}
			else
			{
				option->setValue(-1,0,0);
			}
			
			layer->addChild(option);
			option->setPosition(ccp(layer->getContentSize().width / 2,layer->getContentSize().height - (i * 2 + 1) * ( side + option->getContentSize().height / 2)));
		}
	}
	{
		CCSprite *layer = CCSprite::create("zhuangbei_xiangxi_xilian_shuxing_bg.png");
		layer->setPosition(ccp(this->getContentSize().width - 10 - layer->getContentSize().width / 2,465));
		m_sBackSpr->addChild(layer);

		CCSprite *spr = CCSprite::create("zhuangbei_xiangxi_xilian_xilianhou.png");
		spr->setPosition(ccp(this->getContentSize().width - 10 - layer->getContentSize().width / 2,465 + layer->getContentSize().height / 2 + spr->getContentSize().height / 2));
		addChild(spr);

		int atter_type_index[5] = {AttrType_baoshang,AttrType_hp_percent,AttrType_skillresult,mEquipment->getMainProType(),0};

		float side = 0;
		{
			CCSprite *bgSpr = CCSprite::create("zhuangbei_xiangxi_xilian_shuxing.png");

			side = (layer->getContentSize().height - bgSpr->getContentSize().height * 5 ) / 10.0f;
		}
		//vector<ZBAttr> data_list = mEquipment->getEquipData().purify_attr_vec;
		int count = m_lData_list.size();
		int old_count = m_lOldData_list.size();
		for(int i=0; i!= max_count; ++i)
		{
			ZhuangbeiXilianOverOption *option = ZhuangbeiXilianOverOption::create();
			if (i < count)
			{
				if (i < old_count)
				{
					if (m_lOldData_list[i].attr_value < m_lData_list[i].attr_value)
					{
						option->setValue(CS::getPurifyLevel(m_lData_list[i].attr_value),m_lData_list[i].attr_value,atter_type_index[i],false,true);
					}
					else
					{
						option->setValue(CS::getPurifyLevel(m_lData_list[i].attr_value),m_lData_list[i].attr_value,atter_type_index[i]);
					}
				}
				else
				{
					option->setValue(CS::getPurifyLevel(m_lData_list[i].attr_value),m_lData_list[i].attr_value,atter_type_index[i],true);
				}
			}
			else
			{
				option->setValue(-1,0,0);
			}
			layer->addChild(option);
			option->setPosition(ccp(layer->getContentSize().width / 2,layer->getContentSize().height - (i * 2 + 1) * ( side + option->getContentSize().height / 2)));
		}
	}

	
}
void ZhuangbeiXilianOverUI::clickSelectOk(CCObject *obj)
{
	addObserver(callfuncO_selector(ZhuangbeiXilianOverUI::getMessage), MSG_ConfirmPurifyRsp);
	mCmdHlp->cmdConfirmPurify(m_nZhuangbeiId,true);
}
void ZhuangbeiXilianOverUI::clickSelectCancel(CCObject *obj)
{
	addObserver(callfuncO_selector(ZhuangbeiXilianOverUI::getMessage), MSG_ConfirmPurifyRsp);
	mCmdHlp->cmdConfirmPurify(m_nZhuangbeiId,false);
}
void ZhuangbeiXilianOverUI::getMessage(CCObject *obj)
{
	removeObserver(MSG_ConfirmPurifyRsp);
	GameDirector::getDirector()->hideWaiting();

	ZhuangbeiXilianUI *layer = ZhuangbeiXilianUI::createWithEquipment(mEquipment);
	mFooterLayer->popUpContaintLayer(layer);
}