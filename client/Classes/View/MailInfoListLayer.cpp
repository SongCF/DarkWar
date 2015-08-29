#include "MailInfoListLayer.h"
#include "font.h"
#include "FX_CommonFunc.h"
#include "ImageManager.h"
#include "CS_Common.h"
#include "SuitPool.h"
#include "MailTanChuLayer.h"
#include "SystemGiftBagManager.h"
using namespace FX_CommonFunc;

MailInfoListLayer::MailInfoListLayer()
	:CCLayer(),
	m_nWidth(0),
	m_nHeight(104)
{
}

MailInfoListLayer::~MailInfoListLayer() {
}
bool MailInfoListLayer::init() {
	if (!CCLayer::init()) {
		return false;
	}

	return true;
}

void MailInfoListLayer::onEnter() {
	CCLayer::onEnter();
	
	
}
void MailInfoListLayer::onExit() {
	m_lThingsList.clear();
	//CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}
void MailInfoListLayer::setSystemMail(Email mail)
{
	
	//CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-132,true);
	ListMenu *menu = ListMenu::create();
	menu->setPosition(CCPointZero);
	menu->setAnchorPoint(CCPointZero);
	menu->setContentSize(this->getContentSize());
	addChild(menu);
	m_mMenu = menu;
	//m_mMenu->runAction(CCCallFuncND::create(this, callfuncND_selector(MailInfoListLayer::changeMenuPriority_callfuncND), (void*)(-133)));
	m_eEmail = mail;
	sortMail(mail);
	int count = 0;
	int height = m_nHeight;
	int width = count * 100;
	m_nWidth = width;
	count = 0;
	int side = 50;
	int bWidth = 0;

	int _width = 0;

	int pos_x = 0;
	for (map<string,string>::iterator iter = m_lThingsList.begin(); iter != m_lThingsList.end() ; iter ++)
	{
		CCSprite *cailiaoSpr = CCSprite::create("daoju_kuang_bg.png");
		cailiaoSpr->setScale(0.7f);
		bWidth = cailiaoSpr->getContentSize().width * cailiaoSpr->getScaleX();
		cailiaoSpr->setPosition(ccp(count * bWidth + ( side * ( count * 2 + 1 ) ) + bWidth / 2 ,height - cailiaoSpr->getContentSize().height * cailiaoSpr->getScaleY() / 2 - 5));
		addChild(cailiaoSpr);

		CCSprite *daojuSpr = CCSprite::create(iter->first.c_str());
		daojuSpr->setScale(0.7f);
		bWidth = daojuSpr->getContentSize().width * cailiaoSpr->getScaleX();
		daojuSpr->setPosition(ccp(cailiaoSpr->getContentSize().width / 2, cailiaoSpr->getContentSize().height / 2));
		cailiaoSpr->addChild(daojuSpr);

		CCLabelTTF *nameTTF = CCLabelTTF::create(iter->second.c_str(),fontStr_kaiti,25.0f);
		nameTTF->setPosition(ccp(cailiaoSpr->getPositionX(),nameTTF->getContentSize().height / 2 + 5));
		addChild(nameTTF);
		nameTTF->setColor(ccWHITE);

		_width += cailiaoSpr->getPositionX() - pos_x;
		pos_x = cailiaoSpr->getPositionX();
		count ++;
	}
	_width += bWidth / 2 + side;
	for (vector<Flat_DuanzaoGroupZBInfo>::iterator iter = mail.equipVec.begin(); iter != mail.equipVec.end() ; iter ++)
	{
		
		CCLabelTTF *nameTTF = CCLabelTTF::create(iter->zhuangbei_name.c_str(),fontStr_kaiti,25.0f);
		nameTTF->setPosition(ccp(count * bWidth + ( side * ( count * 2 + 1 ) ) + bWidth / 2 + nameTTF->getContentSize().height / 2 ,nameTTF->getContentSize().height / 2 + 20));
		addChild(nameTTF);
		nameTTF->setColor(ccWHITE);


		//string nameTemp = CS::getItemName(iter->zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(iter->suipian_id);
		string equipfile = iter->zhuangbei_pic;
		equipfile = "minute_" + equipfile + ".png";
		m_lThingsList.insert(map<string,string>::value_type(equipfile,iter->zhuangbei_name));

		string diwenStr = ImageManager::getManager()->getImageData_DiWen_WithBianKuang(iter->zhuangbei_colour);
		CCMenuItemImage *eqDiwenImg = CCMenuItemImage::create(diwenStr.c_str(), diwenStr.c_str(), this,
			menu_selector(MailInfoListLayer::menuItemClicked_Equip));
		m_mMenu->addChild(eqDiwenImg);
		eqDiwenImg->setTag(iter->zb_group_id);
		eqDiwenImg->setScale(0.8f);
		bWidth = eqDiwenImg->getContentSize().width * eqDiwenImg->getScaleX();
		eqDiwenImg->setPosition(ccp( nameTTF->getPositionX(),nameTTF->getPositionY() + nameTTF->getContentSize().height / 2 + eqDiwenImg->getContentSize().height / 2 + 5));
		
		CCSprite *daojuSpr = CCSprite::create(equipfile.c_str());
		daojuSpr->setScale(0.24f);
		bWidth = daojuSpr->getContentSize().width * eqDiwenImg->getScaleX();
		daojuSpr->setPosition(ccp(eqDiwenImg->getContentSize().width / 2, eqDiwenImg->getContentSize().height / 2));
		eqDiwenImg->addChild(daojuSpr);
		/*CCSprite *cailiaoSpr = CCSprite::create("daoju_kuang_bg.png");
		cailiaoSpr->setScale(0.7f);
		bWidth = cailiaoSpr->getContentSize().width * cailiaoSpr->getScaleX();
		cailiaoSpr->setPosition(ccp(count * bWidth + ( side * ( count * 2 + 1 ) ) + bWidth / 2 ,height - cailiaoSpr->getContentSize().height * cailiaoSpr->getScaleY() / 2 - 5));
		addChild(cailiaoSpr);*/

		

		_width += eqDiwenImg->getPositionX() - pos_x;
		pos_x = eqDiwenImg->getPositionX();
		count ++;
	}
	_width += bWidth / 2 + side;
	m_nWidth = _width;
	this->setContentSize(CCSizeMake(m_nWidth,m_nHeight));
}
void MailInfoListLayer::sortMail(Email mail)
{
	if (mail.vitality > 0)
	{
		m_lThingsList.insert(map<string,string>::value_type(ImageManager::getManager()->getItemFilename(ItemsType_PresentVitality),CCString::createWithFormat("%s * %d",CS::getItemName(ItemsType_PresentVitality).c_str(),mail.vitality)->getCString()));
	}

	for (vector<ItemInfo>::iterator iter = mail.itemVec.begin(); iter != mail.itemVec.end() ; iter ++)
	{
		m_lThingsList.insert(map<string,string>::value_type(ImageManager::getManager()->getItemFilename(iter->type),CCString::createWithFormat("%s x %d",CS::getItemName(iter->type).c_str(),iter->count)->getCString()));
	}

	for (vector<ItemSuipianInfo>::iterator iter = mail.itemSuiPianVec.begin(); iter != mail.itemSuiPianVec.end() ; iter ++)
	{
		string nameTemp = CS::getItemName(iter->item_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(iter->suipian_id);
		m_lThingsList.insert(map<string,string>::value_type(ImageManager::getManager()->getItemDebrisFileName(iter->item_id),CCString::createWithFormat("%s x %d",nameTemp.c_str(),iter->suipian_count)->getCString()));
	}

	for (vector<ShenbingSuipianInfo>::iterator iter = mail.shenBingSuiPianVec.begin(); iter != mail.shenBingSuiPianVec.end() ; iter ++)
	{
		string nameTemp = CS::getItemName(iter->zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(iter->suipian_id);
		m_lThingsList.insert(map<string,string>::value_type(ImageManager::getManager()->getItemDebrisFileName(iter->zb_group_id),CCString::createWithFormat("%s x %d",nameTemp.c_str(),iter->suipian_count)->getCString()));
	}
	for (vector<GiftBag>::iterator iter = mail.giftVec.begin(); iter != mail.giftVec.end() ; iter ++)
	{
		//string nameTemp = CS::getItemName(iter->zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(iter->suipian_id);
		m_lThingsList.insert(map<string,string>::value_type(SystemGiftBagManager::getManager()->getGiftBagFileName(iter->id).c_str(),CCString::createWithFormat("%s x %d",SystemGiftBagManager::getManager()->getGiftBagName(iter->id).c_str(),iter->count)->getCString()));
	}
	for (vector<Flat_DuanzaoGroupZBInfo>::iterator iter = mail.equipVec.begin(); iter != mail.equipVec.end() ; iter ++)
	{
		//string nameTemp = CS::getItemName(iter->zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(iter->suipian_id);
		//string equipfile = iter->zhuangbei_pic;
		//equipfile = "minute_" + equipfile + ".png";
		//m_lThingsList.insert(map<string,string>::value_type(equipfile,iter->zhuangbei_name));
	}
	//m_lThingsList
}
void MailInfoListLayer::menuItemClicked_Equip(CCObject *obj)
{
	m_mMenu->setTouchEnabled(false);
	this->setTouchEnabled(false);
	m_lMailTanChuLayer->getTanChuMenu()->setEnabled(false);
	int id = ((CCMenuItem *)obj)->getTag();
	for (vector<Flat_DuanzaoGroupZBInfo>::iterator iter = m_eEmail.equipVec.begin(); iter != m_eEmail.equipVec.end() ; iter ++)
	{
		if (id == iter->zb_group_id)
		{
			displayXiangQing((*iter));

		}
		//string nameTemp = CS::getItemName(iter->zb_group_id) + LFStrings::getValue("zhi") + LFStrings::getValue("suipian") + getStrByInt(iter->suipian_id);
		//string equipfile = iter->zhuangbei_pic;
		//equipfile = "minute_" + equipfile + ".png";
		//m_lThingsList.insert(map<string,string>::value_type(equipfile,iter->zhuangbei_name));
	}
	
}
void MailInfoListLayer::displayXiangQing(Flat_DuanzaoGroupZBInfo zbInfo)
{
	//this->disableAllTouchBegin();
	mCurTaoZhuang_Info = zbInfo;

	

	BaseSprite *board = BaseSprite::create("zhuangbei_xiangxi_mianban.png");
	
	/*BaseLayer *layer = new BaseLayer();
	layer->setContentSize(board->getContentSize());
	layer->setPosition(board->getPosition());
	layer->addChild(board);*/
	CCDirector::sharedDirector()->getRunningScene()->addChild(board,1030);

	//MainLayerBase::getCurrentMainBase()->getHeader()->setContent(BaseSprite::create());
	//MainLayerBase::getCurrentMainBase()->setContaintLayer(layer,false);
	//layer->release();
	refreshEquipBoard(board, zbInfo);
	board->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2, CCDirector::sharedDirector()->getWinSize().height/2 + 106));

	ListMenu *menu = ListMenu::create();
	board->addChild(menu, 10);
	menu->setPosition(CCPointZero);
	//	menu->setTouchPriority(-131);
	BaseSprite *norImg = BaseSprite::create("close.png");
	BaseSprite *selImg = BaseSprite::create("close_select.png");
	CCMenuItemSprite *item = CCMenuItemSprite::create(norImg, selImg, NULL, this, menu_selector(MailInfoListLayer::menuItemClicked_close));
	menu->addChild(item, 0);
	item->setPosition(ccp(board->getContentSize().width-44+12, board->getContentSize().height-36+2));
	//底纹
	ImageData diWenData = ImageManager::getManager()->getImageData_DiWen_XiangQing(
		zbInfo.zhuangbei_colour
		, false
		, SuitPool::getPool()->isSuit(zbInfo.zb_group_id));
	BaseSprite* diWen = BaseSprite::create(diWenData.getCurrentImageFile());
	diWen->setScale(0.7f);
	board->addChild(diWen);
	diWen->setPosition(ccp(123, 838-240+122));
	// 装备图片
	string equipfile = zbInfo.zhuangbei_pic;
	equipfile = "minute_" + equipfile + ".png";
	BaseSprite *s = BaseSprite::create(equipfile);
	diWen->addChild(s);
	s->setPosition(ccp(diWen->getContentSize().width/2,diWen->getContentSize().height/2));
	//套装按钮
	if(SuitPool::getPool()->isSuit(zbInfo.zb_group_id))
	{
		string norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang.png";
		string selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_select.png";
		// 		if (ChuanqiPool::getPool()->isShenBing(mEquipVec.at(idx).zb_group_id))
		// 		{
		// 			norStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing.png";
		// 			selStr = "zhuangbei_xiangxi_mianban_anniu_taozhuang_shenbing_select.png";
		// 		}  
		CCMenuItemImage *taozhuangItem = CCMenuItemImage::create(
			norStr.c_str()
			, selStr.c_str()
			, this ,menu_selector(MailInfoListLayer::menuItemClicked_equipDetailMenu));
		menu->addChild(taozhuangItem, 0);
		taozhuangItem->setAnchorPoint(ccp(1, 0));
		taozhuangItem->setPosition(ccp(diWen->getPositionX()+diWen->getContentSize().width/2*diWen->getScaleX()
			, diWen->getPositionY()-diWen->getContentSize().height/2*diWen->getScaleY()));
		taozhuangItem->setEnabled(false);
	}
	m_dEquimentDialog = board;
	m_mDEquipmentMenu = menu;
}
void MailInfoListLayer::menuItemClicked_equipDetailMenu(CCObject *obj)
{
	((CCMenu*)((CCMenuItem*)obj)->getParent())->setEnabled(false);
	m_mDEquipmentMenu->setEnabled(false);
	//穿在身上的能触发套装属性的套装件数、 groupId
	vector<unsigned int> _vec;
	EquipmentTaoZhuangXiangQing *layer = new EquipmentTaoZhuangXiangQing(
		mCurTaoZhuang_Info.zb_group_id, mCurTaoZhuang_Info.zhuangbei_name, _vec, this);
	layer->setPositionY(106);
	CCDirector::sharedDirector()->getRunningScene()->addChild(layer, 1030);
	//MainLayerBase::getCurrentMainBase()->getHeader()->setContent(BaseSprite::create());
	//MainLayerBase::getCurrentMainBase()->setContaintLayer(layer,false);
	layer->release();
	m_dEquimentInfoLayer = layer;
}
void MailInfoListLayer::menuItemClicked_close(CCObject *obj)
{
	m_dEquimentDialog->removeFromParent();
	m_mMenu->setTouchEnabled(true);
	this->setTouchEnabled(true);
	m_lMailTanChuLayer->getTanChuMenu()->setEnabled(true);
}
void MailInfoListLayer::callbackInterface_TaoZhuangLayer()
{
	m_dEquimentInfoLayer->removeFromParent();
	m_mDEquipmentMenu->setEnabled(true);
}