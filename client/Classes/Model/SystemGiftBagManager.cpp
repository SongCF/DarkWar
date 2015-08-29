#include "SystemGiftBagManager.h"
#include "LFFileUtiles.h"


#define GIFTBAG_CONFIG_FILE "config/GiftInformation.txt"
//一级key
#define key_DIRECTION   "DIRECTION"
#define key_MAX_GIFT_ID "MAX_GIFT_ID"
#define key_GiftInfor   "GiftInfor"
//二级key ---> GiftBagInfor中每一个gift
#define key_ID			 "ID"
#define key_type		 "type"
#define key_rareLevel	 "rareLevel"
#define key_name		 "name"
#define key_imgFile		 "imgFile"
#define key_describe	 "describe"

SystemGiftBagManager* SystemGiftBagManager::_giftBagManager = 0;

SystemGiftBagManager* SystemGiftBagManager::getManager()
{
	if (! _giftBagManager) {
		_giftBagManager = new SystemGiftBagManager;
		_giftBagManager->initInfo();
	}
	return _giftBagManager;
}
const vector<GiftBag>* SystemGiftBagManager::getAllGiftBag()
{
	return &_giftBag;
}

void SystemGiftBagManager::resetGiftBagList(const vector<GiftBag>& vec)
{
	_giftBag = vec;
}
void SystemGiftBagManager::addOneGiftBag(const GiftBag& bag)
{
	for (int i=0, count=_giftBag.size(); i<count; ++i)
	{
		if (_giftBag[i].id == bag.id){
			_giftBag[i].count += bag.count;
			return;
		}
	}
	_giftBag.push_back(bag);
}

void SystemGiftBagManager::subOneGiftBag(unsigned int id)
{
	for (int i=0, count=_giftBag.size(); i<count; ++i)
	{
		if (_giftBag[i].id == id){
			_giftBag[i].count -= 1;
			if (_giftBag[i].count == 0) _giftBag.erase(_giftBag.begin()+i);
			break;
		}
	}
}

void SystemGiftBagManager::initInfo()
{
	unsigned long fileSize;
	const char *pData;
	string doc;
	pData = LFFileUtiles::getLFFileUtiles()->readResourceFile(GIFTBAG_CONFIG_FILE, &fileSize);
	//pData = CCFileUtils::sharedFileUtils()->getFileData(GIFTBAG_CONFIG_FILE, "r", &fileSize);
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
		SystemGiftBagInfo temp;
		for (int i=0,count=allItems.size(); i<count; ++i)
		{
			temp.ID = allItems[i][key_ID].asUInt();
			temp.type = allItems[i][key_type].asUInt();
			temp.rareLevel = allItems[i][key_rareLevel].asUInt();
			temp.name = allItems[i][key_name].asString();
			temp.imgFile = allItems[i][key_imgFile].asString();
			temp.describe = allItems[i][key_describe].asString();

			_infoVec.push_back(temp);
		}
	}
}

//礼包类型
unsigned int SystemGiftBagManager::getGiftBagType(unsigned int id)
{
	for (int i=0,count=_infoVec.size(); i<count; ++i)
	{
		if (_infoVec.at(i).ID == id) return _infoVec.at(i).type;
	}

	CCAssert(false, "");//没有找到该礼包
	return 0;
}
//礼包名
string SystemGiftBagManager::getGiftBagName(unsigned int id)
{
	for (int i=0,count=_infoVec.size(); i<count; ++i)
	{
		if (_infoVec.at(i).ID == id) return _infoVec.at(i).name;
	}

	CCAssert(false, "");//没有找到该礼包
	return "";
}
//礼包图片名
string SystemGiftBagManager::getGiftBagFileName(unsigned int id)
{
	for (int i=0,count=_infoVec.size(); i<count; ++i)
	{
		if (_infoVec.at(i).ID == id) return _infoVec.at(i).imgFile;
	}

	CCAssert(false, "");//没有找到该礼包
	return "";
}
//礼包描述
string SystemGiftBagManager::getGiftBagDescribe(unsigned int id)
{
	for (int i=0,count=_infoVec.size(); i<count; ++i)
	{
		if (_infoVec.at(i).ID == id) return _infoVec.at(i).describe;
	}

	CCAssert(false, "");//没有找到该礼包
	return "";
}
//礼包的稀有度
unsigned int SystemGiftBagManager::getGiftBagRareLevel(unsigned int id)
{
	for (int i=0,count=_infoVec.size(); i<count; ++i)
	{
		if (_infoVec.at(i).ID == id) return _infoVec.at(i).rareLevel;
	}

	CCAssert(false, "");//没有找到该礼包
	return 0;
}