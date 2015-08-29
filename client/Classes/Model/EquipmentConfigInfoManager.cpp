#include "EquipmentConfigInfoManager.h"
#include "include_json.h"
#include "cocos2d.h"
#include "LFFileUtiles.h"
#include <cstring>

using namespace cocos2d;

#define ITEM_CONFIG_FILE "config/EquipInfo.txt"

#define key_GroupID "GroupID"
#define key_name "name"
#define key_describe "describe"
#define key_picFile "picFile"

EquipmentConfigInfoManager* EquipmentConfigInfoManager::_equipConfigManager = 0;



int equipInfoCompareFunc(const void* a, const void* b)
{
	return ((EquipConfigInfo*)a)->GroupID - ((EquipConfigInfo*)b)->GroupID;
}

EquipmentConfigInfoManager::EquipmentConfigInfoManager()
{
//	m_pData = NULL;
	initData();
}

EquipmentConfigInfoManager::~EquipmentConfigInfoManager()
{
//	if (m_pData) delete [] m_pData;
}
EquipmentConfigInfoManager* EquipmentConfigInfoManager::getManager()
{
	if (! _equipConfigManager) _equipConfigManager = new EquipmentConfigInfoManager();
	return _equipConfigManager;
}
void EquipmentConfigInfoManager::initData()
{
	unsigned long fileSize;
	const char *pData;
	string doc;
	pData = LFFileUtiles::getLFFileUtiles()->readResourceFile(ITEM_CONFIG_FILE, &fileSize);
	if (pData && fileSize > 0){
		doc = (char*)pData;
		delete [] pData;
	}else {
		CCMessageBox("read data from EquipInfo.txt error!", "ERROR");
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
	Json::Value allEquips = root["Equipment"];
	if (! allEquips.isNull() && allEquips.isArray()){
		EquipConfigInfo temp;
		for (int i=0,count=allEquips.size(); i<count; ++i)
		{
			temp.GroupID = allEquips[i][key_GroupID].asUInt();
			temp.name = allEquips[i][key_name].asString();
			temp.describe = allEquips[i][key_describe].asString();
			temp.picFile = allEquips[i][key_picFile].asString();
			_equipInfoVec.push_back(temp);
			
		}
		qsort(&_equipInfoVec[0], _equipInfoVec.size(), sizeof(EquipConfigInfo), equipInfoCompareFunc);
	}
}


string EquipmentConfigInfoManager::getEquipName(unsigned int groupID)
{
	EquipConfigInfo* p = (EquipConfigInfo* )bsearch(&groupID, &_equipInfoVec[0], _equipInfoVec.size(), sizeof(EquipConfigInfo), equipInfoCompareFunc);
	if (p) {
		return p->name;
	}
	else {
		CCLOG("Error: %s ---> groupID = %u", __FUNCTION__, groupID);
		CCAssert(false, "not found equip by group Id");
		return "";
	}
}
string EquipmentConfigInfoManager::getEquipDescribe(unsigned int groupID)
{
	EquipConfigInfo* p = (EquipConfigInfo* )bsearch(&groupID, &_equipInfoVec[0], _equipInfoVec.size(), sizeof(EquipConfigInfo), equipInfoCompareFunc);
	if (p) {
		return p->describe;
	}
	else {
		CCLOG("Error: %s ---> groupID = %u", __FUNCTION__, groupID);
		CCAssert(false, "not found equip by group Id");
		return "";
	}
}

string EquipmentConfigInfoManager::getEquipPicFile(unsigned int groupID)
{
	EquipConfigInfo* p = (EquipConfigInfo* )bsearch(&groupID, &_equipInfoVec[0], _equipInfoVec.size(), sizeof(EquipConfigInfo), equipInfoCompareFunc);
	if (p) {
		return p->picFile;
	}
	else {
		CCLOG("Error: %s ---> groupID = %u", __FUNCTION__, groupID);
		CCAssert(false, "not found equip by group Id");
		return "";
	}
}

