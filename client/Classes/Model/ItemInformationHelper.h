/*
 道具信息
 解析配置文件中配置的道具信息
 使用hash表存储，key为道具ID，所以尽量保证道具的ID空白跨度不要太大，最好一次递增+1，否则会浪费空间
*/

#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;

struct ItemInformation
{
	unsigned int ID;
	unsigned int type;
	unsigned int rareLevel;
	string name;
	string imgFile;
	string describe;

	ItemInformation(){
		ID = 0;
	}
	const ItemInformation& operator=(const ItemInformation& _in){
		this->ID = _in.ID;
		this->type = _in.type;
		this->rareLevel = _in.rareLevel;
		this->name = _in.name;
		this->imgFile = _in.imgFile;
		this->describe = _in.describe;
		return *this;
	}
};

class ItemInformationHelper
{
private:
	static ItemInformationHelper* _itemInforHelper;
	ItemInformationHelper();
public:
	static ItemInformationHelper* getHelper();
	~ItemInformationHelper();

	//道具类型
	unsigned int getItemTypeByID(unsigned int id);
	//道具名
	string getItemNameByID(unsigned int id);
	//道具图片名
	string getItemFileNameByID(unsigned int id);
	//道具描述
	string getItemDescribeByID(unsigned int id);
	//道具的稀有度
	unsigned int getItemRareLevelByID(unsigned int id);

protected:
	void initData();


private:

	//bug： 使用hash 在Android上面对name imgFile describe赋值的时候会崩溃。
	//故改为 vector存储、使用二分查找 
//	ItemInformation *m_pData;

	vector<ItemInformation> _itemInfoVec;
};