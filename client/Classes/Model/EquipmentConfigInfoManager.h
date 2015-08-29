/*
 装备信息配置
*/

#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;

typedef struct tagEQUIPCONFIG
{
	unsigned int GroupID;
	string name;
	string describe;
	string picFile;

	tagEQUIPCONFIG(){
		GroupID = 0;
	}
	const tagEQUIPCONFIG& operator=(const tagEQUIPCONFIG& _in){
		this->GroupID = _in.GroupID;
		this->name = _in.name;
		this->describe = _in.describe;
		this->picFile = _in.picFile;

		return *this;
	}
}EquipConfigInfo;

class EquipmentConfigInfoManager
{
private:
	static EquipmentConfigInfoManager* _equipConfigManager;
	EquipmentConfigInfoManager();
public:
	static EquipmentConfigInfoManager* getManager();
	~EquipmentConfigInfoManager();

	string getEquipName(unsigned int groupID);
	string getEquipDescribe(unsigned int groupID);
	string getEquipPicFile(unsigned int groupID);

protected:
	void initData();


private:

	vector<EquipConfigInfo> _equipInfoVec;
};