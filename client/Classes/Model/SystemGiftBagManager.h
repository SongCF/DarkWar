/*
 游戏礼包的一个管理类
*/

#pragma once

#include <vector>
#include "protocol.h"

using std::vector;

struct SystemGiftBagInfo
{
	unsigned int ID;
	unsigned int type;
	unsigned int rareLevel;
	string name;
	string imgFile;
	string describe;

	SystemGiftBagInfo(){
		ID = 0;
	}
	const SystemGiftBagInfo& operator=(const SystemGiftBagInfo& _in){
		this->ID = _in.ID;
		this->type = _in.type;
		this->rareLevel = _in.rareLevel;
		this->name = _in.name;
		this->imgFile = _in.imgFile;
		this->describe = _in.describe;
		return *this;
	}
};

class SystemGiftBagManager
{
	friend class CmdHelper;
private:
	SystemGiftBagManager(){}
	static SystemGiftBagManager* _giftBagManager;
	vector<GiftBag> _giftBag;
	vector<SystemGiftBagInfo> _infoVec;
public:
	static SystemGiftBagManager* getManager();
	const vector<GiftBag>* getAllGiftBag();

	void initInfo();
	//礼包类型
	unsigned int getGiftBagType(unsigned int id);
	//礼包名
	string getGiftBagName(unsigned int id);
	//礼包图片名
	string getGiftBagFileName(unsigned int id);
	//礼包描述
	string getGiftBagDescribe(unsigned int id);
	//礼包的稀有度
	unsigned int getGiftBagRareLevel(unsigned int id);


protected:
	void resetGiftBagList(const vector<GiftBag>& vec);
	void addOneGiftBag(const GiftBag& bag);
	void subOneGiftBag(unsigned int id);
};