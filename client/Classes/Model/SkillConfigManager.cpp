#include "SkillConfigManager.h"
#include "include_json.h"
#include "cocos2d.h"
#include "LFFileUtiles.h"
#include <cstring>

using namespace cocos2d;

#define SKILL_CONFIG_FILE "config/SkillConfiguration.txt"

#define KEY_ROOT "Skill"

#define key_ID			 "ID"
#define key_type		 "type"
#define key_name		 "name"
#define key_imgFile		 "imgFile"
#define key_describe	 "describe"
#define key_music		 "musicFile"

#define key_animateName	 "armature_name"
#define key_animationNum "animationNum"

SkillConfigManager* SkillConfigManager::_skillInfoManager = 0;


int skillInfoCompareFunc(const void* a, const void* b)
{
	return ((SkillConfigInfor*)a)->ID - ((SkillConfigInfor*)b)->ID;
}


SkillConfigManager::SkillConfigManager()
{
	initData();
}

SkillConfigManager::~SkillConfigManager()
{
}
SkillConfigManager* SkillConfigManager::getManager()
{
	if (! _skillInfoManager) _skillInfoManager = new SkillConfigManager();
	return _skillInfoManager;
}

void SkillConfigManager::initData()
{
	unsigned long fileSize;
	const char *pData;
	string doc;
	pData = LFFileUtiles::getLFFileUtiles()->readResourceFile(SKILL_CONFIG_FILE, &fileSize);
	if (pData && fileSize > 0){
		doc = (char*)pData;
		delete [] pData;
	}else {
		CCMessageBox("read data from SkillConfiguration.txt error!", "ERROR");
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
	Json::Value allItems = root[KEY_ROOT];
	if (! allItems.isNull() && allItems.isArray()){
		SkillConfigInfor temp;
		for (int i=0,count=allItems.size(); i<count; ++i)
		{
			temp.ID = allItems[i][key_ID].asUInt();
			temp.type = allItems[i][key_type].asUInt();
			temp.name = allItems[i][key_name].asCString();
			temp.imgFile = allItems[i][key_imgFile].asCString();
			temp.describe = allItems[i][key_describe].asCString();
			temp.musicFile = allItems[i][key_music].asCString();

			if (! allItems[i][key_animateName].isNull())	temp.armatureName = allItems[i][key_animateName].asCString();
			if (! allItems[i][key_animationNum].isNull())	temp.animationHitFrameNum = allItems[i][key_animationNum].asUInt();

			_skillInfoVec.push_back(temp);
		}
		qsort(&_skillInfoVec[0], _skillInfoVec.size(), sizeof(SkillConfigInfor), skillInfoCompareFunc);
	}
}

const vector<SkillConfigInfor>* SkillConfigManager::getAllSkillInfo()
{
	return &_skillInfoVec;
}
const SkillConfigInfor* SkillConfigManager::getOneSkillInfo(unsigned int skillId)
{
	SkillConfigInfor* p = (SkillConfigInfor* )bsearch(&skillId, &_skillInfoVec[0], _skillInfoVec.size(), sizeof(SkillConfigInfor), skillInfoCompareFunc);
	if (p) {
		return p;
	}
	else {
		CCLOG("not found skill by Id");
		CCAssert(false, "");
		return 0;
	}
}
int SkillConfigManager::getSkillType(unsigned int skillId)
{
	const SkillConfigInfor* p = getOneSkillInfo(skillId);
	if (p) return p->type;
	else return 0;
}
string SkillConfigManager::getSkillName(unsigned int skillId)
{
	const SkillConfigInfor* p = getOneSkillInfo(skillId);
	if (p) return p->name;
	else return "";
}
string SkillConfigManager::getImgFileName(unsigned int skillId)
{
	const SkillConfigInfor* p = getOneSkillInfo(skillId);
	if (p) return p->imgFile;
	else return "";
}
string SkillConfigManager::getDescribe(unsigned int skillId)
{
	const SkillConfigInfor* p = getOneSkillInfo(skillId);
	if (p) return p->describe;
	else return "";
}

string SkillConfigManager::getMusicFile(unsigned int skillId, int idx)
{
	const SkillConfigInfor* p = getOneSkillInfo(skillId);
	if (p) {
		CCString temp;
		temp.initWithFormat("%s%d.wav", p->musicFile.c_str(), idx);
		return temp.getCString();
	}
	else return "";
}

//战斗动画
string SkillConfigManager::getFightArmatureKeyName(unsigned int skillId)
{
	const SkillConfigInfor* p = getOneSkillInfo(skillId);
	if (p) return p->armatureName;
	else return "";
}

unsigned int SkillConfigManager::getFightAnimationHitFrameNum(unsigned int skillId)
{
	const SkillConfigInfor* p = getOneSkillInfo(skillId);
	if (p) return p->animationHitFrameNum;
	else return 0;
}