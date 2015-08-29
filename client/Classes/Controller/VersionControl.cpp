#include "VersionControl.h"
#include "include_json.h"
#include "LFFileUtiles.h"
#include "cocos2d.h"
using namespace cocos2d;

#define VERSION_FILE "infor/version.txt"

VersionControlHelper *VersionControlHelper::_versionHelper = 0;
VersionControlHelper::VersionControlHelper()
{

	LFFileUtiles* fileUtiles = LFFileUtiles::getLFFileUtiles();
	bool succ = fileUtiles->copyFileFromResToDocIfNotExist(VERSION_FILE);

	if (!succ)
	{
		CCAssert(false,"copy file from res to doc error!");
		return;
	}

	unsigned long size;
	const char* data = fileUtiles->readDocFile(VERSION_FILE,&size);
	//文件读取失败就返回NULL;
	if (size<=0)
	{
		CCAssert(false,"Load histroy file from doc error!");
		return;
	}
	string doc = string(data);
	delete [] data;

	//
	Json::Reader reader;
	Json::Value root;
	if (! reader.parse(doc.c_str(), root))
	{
		CCLOG("parse version file error");
		CCAssert(false, "");
		return;
	}

	_versionCode = root["version"].asString();
	_updateDate = root["date"].asString();
	_alreadyDisplay = root["has_display"].asBool();
	Json::Value arr = root["update_infor"];
	if (arr.isArray() && arr.size() > 0)
	{
		for (int i=0,count=arr.size(); i<count; ++i)
		{
			_updateInfo.push_back(arr[i].asString());
		}
	}

}

VersionControlHelper* VersionControlHelper::getHelper()
{
	if (! _versionHelper) _versionHelper = new VersionControlHelper;
	return _versionHelper;
}

bool VersionControlHelper::isDisplayNewVersionInfo()
{
	return !_alreadyDisplay;
}
void VersionControlHelper::setDisplayNewVersionInfo(bool isDisplay)
{
	if (_alreadyDisplay != isDisplay){
		_alreadyDisplay = isDisplay;
		save();
	}
}

string VersionControlHelper::getCurrentVersion()
{
	return _versionCode;
}
string VersionControlHelper::getUpdateDate()
{
	return _updateDate;
}
const vector<string>* VersionControlHelper::getUpdateInfo()
{
	return &_updateInfo;
}

void VersionControlHelper::save()
{
	Json::Value root, infor;

	for (int i=0,count=_updateInfo.size(); i<count; ++i)
	{
		infor.append(Json::Value(_updateInfo.at(i)));
	}

	root["update_infor"] = infor;
	root["version"] = Json::Value(_versionCode);
	root["date"] = Json::Value(_updateDate);
	root["has_display"] = Json::Value(_alreadyDisplay);

	Json::StyledWriter writer;
	bool success = LFFileUtiles::getLFFileUtiles()->writeToDocFile(VERSION_FILE, writer.write(root));
	if (!success)
	{
		CCLog("save error!");
		CCLog("=======================================================================");
		CCLog("%s", VERSION_FILE);
		CCLog("=======================================================================");
		CCAssert(false,"save version file error!");
	}
}