#include "ChooseHerosLayer.h"
#include "EquipmentManager.h"
#include "GameDirector.h"
#include "MyselfManager.h"
#include "ImageManager.h"
#include "WuPinManager.h"
#include "MainHeroManager.h"
#include "CreateHeroLayer.h"
#include "LFUtile.h"
#include "font.h"
#include "BaseActorPropertyParser.h"

#define TAG_ITEM_CREATE		11111
#define TAG_ITEM_DELETE		11112
#define TAG_ITEM_OK			11113

ChooseHerosLayer::ChooseHerosLayer(unsigned int defaultId)
{
	// 分三段
	setHeadTopMost();
	setFootButtomMost();
	setBodyCenterMost();

	mDefautHeroId = defaultId;

	vector<HeroInfo> inforsAll = MainHeroManager::getManager()->getAllMainHero();

	BaseSprite* bg = BaseSprite::create("main_bg.png");
	addChild(bg,-1);
	bg->setPosition(ccp(mWinSize.width/2,mVisibleSize.height/2));


	BaseSprite* headBiaoJi = BaseSprite::create("main_biaozhi.png");
	headBiaoJi->setScale(0.56f);
	headBiaoJi->setPosition(ccp(mWinSize.width/2,960 - 80));
	addChildToHead(headBiaoJi);

	BaseSprite* listViewBg = BaseSprite::create("jiaose_liebiao_kuang.png");
	HeroListView* listView = new HeroListView(this,defaultId);
	listViewBg->addChild(listView);
	addChildToBody(listViewBg);
	listViewBg->setPosition(ccp(131,474));
	listView->setPosition(ccp(110,listViewBg->getContentSize().height/2));

	BaseSprite* jueSheTitle = BaseSprite::create("jiaose_liebiao_biaoti.png");
	addChildToBody(jueSheTitle);
	jueSheTitle->setPosition(ccp(124,674));

	HeroInfo hero = MainHeroManager::getManager()->getHeroByHeroId(defaultId);

	BaseSprite* xiangKuang = BaseSprite::create("qiehuan_renwu_kuang.png");
	addChildToBody(xiangKuang);
	xiangKuang->setPosition(ccp(452,491));

	ImageData data = ImageManager::getManager()->getImageData_JueSheQuanShen((Profession)hero.profession_id);
	photo = BaseSprite::create(data.getCurrentImageFile());
	photo->setScale(0.8f);
	xiangKuang->addChild(photo);
	photo->setPosition(ccp(xiangKuang->getContentSize().width/2,xiangKuang->getContentSize().height/2));

	// 创建按钮
	{
		CCMenuItemImage* item = CCMenuItemImage::create(
			"qiehuan_chuangjian.png",
			"qiehuan_chuangjian_select.png",
			this,SEL_MenuHandler(&ChooseHerosLayer::itemClicked)
			);
		item->setTag(TAG_ITEM_CREATE);
		CCMenu* menu = CCMenu::create(item,NULL);
		item->setPosition(ccp(131,290));
		menu->setPosition(CCPointZero);
		addChildToBody(menu);
	}

	// 删除按钮
	{
		CCMenuItemImage* item = CCMenuItemImage::create(
			"qiehuan_shanchu.png",
			"qiehuan_shanchu_select.png",
			this,SEL_MenuHandler(&ChooseHerosLayer::itemClicked)
			);
		item->setTag(TAG_ITEM_DELETE);
		item->setVisible(false);
		CCMenu* menu = CCMenu::create(item,NULL);
		item->setPosition(ccp(511,216));
		menu->setPosition(CCPointZero);
		addChildToBody(menu);
	}

	// 确定按钮
	{
		CCMenuItemImage* ok = CCMenuItemImage::create(
			"qiehuan_queren.png",
			"qiehuan_queren_select.png",
			this,SEL_MenuHandler(&ChooseHerosLayer::itemClicked));
		CCMenu* menu = CCMenu::create(ok,NULL);
		addChildToFoot(menu);
		menu->setPosition(CCPointZero);
		ok->setPosition(ccp(mWinSize.width/2,87));
		ok->setTag(TAG_ITEM_OK);
	}

	selectHeroById(defaultId);

	
}
void ChooseHerosLayer::selectHeroById(unsigned int heroId)
{
	mSelectedHeroId = heroId;
	HeroInfo hero = MainHeroManager::getManager()->getHeroByHeroId(heroId);
	ImageData data = ImageManager::getManager()->getImageData_JueSheQuanShen((Profession)hero.profession_id);
	photo->initWithFile(data.getCurrentImageFile());
}

void ChooseHerosLayer::itemClicked(CCObject* sender)
{
	CCMenuItem* item = (CCMenuItem*)sender;

	if (item->getTag() == TAG_ITEM_CREATE)
	{
		CreateHeroLayer* layer = new CreateHeroLayer(CreateHero_Dest_ChooseHerosLayer);
		CCScene* scene = CCScene::create();
		scene->addChild(layer);
		layer->release();
		CCDirector::sharedDirector()->pushScene(scene);
	}
	else if (item->getTag() == TAG_ITEM_DELETE)
	{

	}
	else if (item->getTag() == TAG_ITEM_OK)
	{
		GameDirector::getDirector()->goToDefaultMainHeroLayer(mSelectedHeroId);
	}
}

ChooseHerosLayer::~ChooseHerosLayer(void)
{
}


// 前缀
static string listItemPic = "jiaose_liebiao_";
#define TAG_ITEM_LABLE_NAME		222222
#define TAG_ITEM_LABLE_LEVEL	222223
HeroListView::HeroListView(ChooseHerosLayer* layer,unsigned int selectedHeroId)
{
	mLayer = layer;
	mDataVec = MainHeroManager::getManager()->getAllMainHero();
	selectedIndex = -1;
	int num = mDataVec.size();
	for (int i=0;i<num;i++)
	{
		if (mDataVec[i].hero_id == selectedHeroId)
		{
			selectedIndex = i;
		}
	}
	CCAssert(selectedIndex != -1,"没有找到指定Id的Hero");

	ListViewDataSource data;
	data.creator = this;
	data.dir = kCCScrollViewDirectionVertical;
	data.itemDefaultSize = CCSizeMake(213,66);
	data.listItemNum = mDataVec.size();
	data.listViewContainer->setContentSize(CCSizeMake(214,276));

	reloadData(data);
}

// 创建第i个元素
BaseSprite* HeroListView::getItemContentByIndex(unsigned int index)
{
	HeroInfo infor = mDataVec[index];
	HeroFixedPro pro = BaseActorPropertyParser::getParser()->getExtraProerty((Profession)infor.profession_id);
	
	string fileName = 
		listItemPic + 
		ImageManager::getManager()->getBaseImageName((Profession)infor.profession_id)+
		(selectedIndex == index?"_select.png":".png");
	BaseSprite* item = BaseSprite::create(fileName);

	LF::lable(item,mDataVec[index].name,ccp(61,46),20,fontStr_kaiti)->setTag(TAG_ITEM_LABLE_NAME);
	LF::lable(item,CCString::createWithFormat(
		"%s%d %s",LF::getString("dengji").c_str(),infor.level,pro.nameInChinese.c_str()),
		ccp(61,20),15,fontStr_kaiti)->setTag(TAG_ITEM_LABLE_LEVEL);

	return item;
};
// 刷新第i个元素，也可以简单的用getItemContentByIndex，再创建一个就是了
void HeroListView::refreshItem(CCNode* item,int index)
{
	HeroInfo infor = mDataVec[index];
	HeroFixedPro pro = BaseActorPropertyParser::getParser()->getExtraProerty((Profession)infor.profession_id);

	BaseSprite* s = (BaseSprite*)item;
	string fileName = 
		listItemPic + 
		ImageManager::getManager()->getBaseImageName((Profession)infor.profession_id)+
		(selectedIndex == index?"_select.png":".png");
	s->initWithFile(fileName);

	CCLabelTTF* labeName = (CCLabelTTF*)s->getChildByTag(TAG_ITEM_LABLE_NAME);
	CCLabelTTF* labeLevel = (CCLabelTTF*)s->getChildByTag(TAG_ITEM_LABLE_LEVEL);
	labeName->setString(infor.name);
	labeLevel->setString(CCString::createWithFormat(
		"%s%d %s",LF::getString("dengji").c_str(),infor.level,pro.nameInChinese.c_str())->getCString());

}


void HeroListView::listItemTouched(CCNode* listItem,int index)
{
	if (index != selectedIndex)
	{
		// 上次的还原
		if (selectedIndex != -1)
		{
			string fileName = 
				listItemPic + 
				ImageManager::getManager()->getBaseImageName((Profession)mDataVec[selectedIndex].profession_id)+
				".png";

			BaseSprite* last = (BaseSprite*)getListItemByIndex(selectedIndex);
			if (last != NULL)
			{
				last->initWithFile(fileName);
			}
		}

		// 刷新这次的
		string fileNameNow = 
			listItemPic + 
			ImageManager::getManager()->getBaseImageName((Profession)mDataVec[index].profession_id)+
			"_select.png";
		BaseSprite* now = (BaseSprite*)getListItemByIndex(index);
		now->initWithFile(fileNameNow);

		mLayer->selectHeroById(mDataVec[index].hero_id);
	}

	selectedIndex = index;
}
