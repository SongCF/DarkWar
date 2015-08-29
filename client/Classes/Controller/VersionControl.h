#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;

// 当前版本号为2，如果保存的某些文件的版本跟这个号码不同，
// 那么就不会采用那个保存的数据
//#define CurrentVersionCode 2   //不再使用 20140630 scf


class VersionControlHelper
{
private:
	static VersionControlHelper *_versionHelper;
	VersionControlHelper();
	void save();
public:
	static VersionControlHelper* getHelper();

	bool isDisplayNewVersionInfo();
	void setDisplayNewVersionInfo(bool isDisplay=true);
	string getCurrentVersion();
	string getUpdateDate();
	const vector<string>* getUpdateInfo();

private:
	string _versionCode;
	string _updateDate;
	vector<string> _updateInfo;
	bool _alreadyDisplay;
};