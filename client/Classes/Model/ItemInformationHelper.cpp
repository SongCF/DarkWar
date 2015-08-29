#include "ItemInformationHelper.h"
#include "include_json.h"
#include "cocos2d.h"
#include "LFFileUtiles.h"
#include <cstring>

using namespace cocos2d;

#define ITEM_CONFIG_FILE "config/ItemInformation.txt"

//一级key
#define key_DIRECTION   "DIRECTION"
#define key_MAX_GIFT_ID "MAX_ITEM_ID"
#define key_GiftInfor   "ItemInfor"
//二级key ---> ItemInfor中每一个item
#define key_ID			 "ID"
#define key_type		 "type"
#define key_rareLevel	 "rareLevel"
#define key_name		 "name"
#define key_imgFile		 "imgFile"
#define key_describe	 "describe"

ItemInformationHelper* ItemInformationHelper::_itemInforHelper = 0;



int itemInfoCompareFunc(const void* a, const void* b)
{
	return ((ItemInformation*)a)->ID - ((ItemInformation*)b)->ID;
}

ItemInformationHelper::ItemInformationHelper()
{
//	m_pData = NULL;
	initData();
}

ItemInformationHelper::~ItemInformationHelper()
{
//	if (m_pData) delete [] m_pData;
}
ItemInformationHelper* ItemInformationHelper::getHelper()
{
	if (! _itemInforHelper) _itemInforHelper = new ItemInformationHelper();
	return _itemInforHelper;
}
void ItemInformationHelper::initData()
{
	unsigned long fileSize;
	const char *pData;
	string doc;
	//pData = CCFileUtils::sharedFileUtils()->getFileData(ITEM_CONFIG_FILE, "r", &fileSize);
	pData = LFFileUtiles::getLFFileUtiles()->readResourceFile(ITEM_CONFIG_FILE, &fileSize);
	if (pData && fileSize > 0){
		doc = (char*)pData;
		delete [] pData;
	}else {
		CCMessageBox("read data from ItemInformation.txt error!", "ERROR");
		return;
	}

	Json::Reader reader;
	Json::Value root;
	//解析不成功就返回空;
	if (!reader.parse(doc, root))
	{
		CCLog("Error : %s --> parse json error!", __FUNCTION__);
		CCAssert(false,"parse json error!");
		return;
	}

	//解析数据
	unsigned int maxId = root[key_MAX_GIFT_ID].asUInt();
	Json::Value allItems = root[key_GiftInfor];
	if (! allItems.isNull() && allItems.isArray()){
//bug： 使用hash 在Android上面对name imgFile describe赋值的时候会崩溃。
// 		m_pData = new ItemInformation[maxId+1]; //使用hash, itemId作为key
// 		memset(m_pData, 0, sizeof(ItemInformation)*(maxId+1));
		ItemInformation temp;
		for (int i=0,count=allItems.size(); i<count; ++i)
		{
			temp.ID = allItems[i][key_ID].asUInt();
			temp.type = allItems[i][key_type].asUInt();
			temp.rareLevel = allItems[i][key_rareLevel].asUInt();
			temp.name = allItems[i][key_name].asCString();
			temp.imgFile = allItems[i][key_imgFile].asCString();
			temp.describe = allItems[i][key_describe].asCString();
			_itemInfoVec.push_back(temp);

// 			id = allItems[i][key_ID].asUInt();
// 			m_pData[id].ID = id;
// 			m_pData[id].type = allItems[i][key_type].asUInt();
// 			m_pData[id].rareLevel = allItems[i][key_rareLevel].asUInt();
// 			m_pData[id].name = allItems[i][key_name].asCString();
// 			m_pData[id].imgFile = allItems[i][key_imgFile].asCString();
// 			m_pData[id].describe = allItems[i][key_describe].asCString();
			
		}
		qsort(&_itemInfoVec[0], _itemInfoVec.size(), sizeof(ItemInformation), itemInfoCompareFunc);
	}
}

	//道具类型
unsigned int ItemInformationHelper::getItemTypeByID(unsigned int id)
{
	ItemInformation* p = (ItemInformation* )bsearch(&id, &_itemInfoVec[0], _itemInfoVec.size(), sizeof(ItemInformation), itemInfoCompareFunc);
	if (p) {
		return p->type;
	}
	else {
		CCAssert(false, "not found item by Id");
		return 0;
	}

//	return m_pData[id].type;
}
//道具的稀有度
unsigned int ItemInformationHelper::getItemRareLevelByID(unsigned int id)
{
	ItemInformation* p = (ItemInformation* )bsearch(&id, &_itemInfoVec[0], _itemInfoVec.size(), sizeof(ItemInformation), itemInfoCompareFunc);
	if (p) {
		return p->rareLevel;
	}
	else {
		CCAssert(false, "not found item by Id");
		return 0;
	}
//	return m_pData[id].rareLevel;
}
	//道具名
string ItemInformationHelper::getItemNameByID(unsigned int id)
{
	ItemInformation* p = (ItemInformation* )bsearch(&id, &_itemInfoVec[0], _itemInfoVec.size(), sizeof(ItemInformation), itemInfoCompareFunc);
	if (p) {
		return p->name;
	}
	else {
		CCAssert(false, "not found item by Id");
		return "";
	}
//	return m_pData[id].name;
}
	//道具图片名
string ItemInformationHelper::getItemFileNameByID(unsigned int id)
{
	ItemInformation* p = (ItemInformation* )bsearch(&id, &_itemInfoVec[0], _itemInfoVec.size(), sizeof(ItemInformation), itemInfoCompareFunc);
	if (p) {
		return p->imgFile;
	}
	else {
		CCAssert(false, "not found item by Id");
		return "";
	}
//	return m_pData[id].imgFile;
}
	//道具描述
string ItemInformationHelper::getItemDescribeByID(unsigned int id)
{
	ItemInformation* p = (ItemInformation* )bsearch(&id, &_itemInfoVec[0], _itemInfoVec.size(), sizeof(ItemInformation), itemInfoCompareFunc);
	if (p) {
		return p->describe;
	}
	else {
		CCAssert(false, "not found item by Id");
		return "";
	}
//	return m_pData[id].describe;
}
