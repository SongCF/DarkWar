#include "BonusManager.h"

BonusManager* BonusManager::mInstance = NULL;

BonusManager::BonusManager(void)
{
}

BonusManager* BonusManager::getManager()
{
	if (mInstance == NULL)
	{
		mInstance = new BonusManager();
	}
	return mInstance;
}

BonusManager::~BonusManager(void)
{
}

void BonusManager::setBonus(BonusType _type,const vector<Flat_BonusInfo>& jiangli)
{
	map<int,vector<Flat_BonusInfo> >::iterator it = bonusPool.find(_type);
	if (it == bonusPool.end())
	{
		bonusPool.insert(pair<int,vector<Flat_BonusInfo> >(_type,jiangli));
	}else
	{
		it->second = jiangli;
	}
}
void BonusManager::setBonus(const vector<Flat_BonusInfo>& vec)
{
	bonusPool.clear();
	for (int i=0,count=vec.size(); i<count; ++i)
	{
		bonusPool[vec.at(i).type].push_back(vec.at(i));
	}
}
void BonusManager::setOneBonusEnable(BonusType type, unsigned int id)
{
	map<int,vector<Flat_BonusInfo> >::iterator it = bonusPool.find(type);
	if (it == bonusPool.end())
	{
		CCAssert(false,"没找到");
		return;
	}else
	{
		for (int i=0,count=it->second.size(); i<count; i++)
		{
			if (it->second[i].bonus_id == id)
			{
				it->second[i].enable = true;
			}
		}
	}
}
void BonusManager::setIsGet(BonusType _type,unsigned int _id)
{
	map<int,vector<Flat_BonusInfo> >::iterator it = bonusPool.find(_type);
	if (it == bonusPool.end())
	{
		CCAssert(false,"没找到");
		return;
	}else
	{
		for (int i=0,count=it->second.size(); i<count; i++)
		{
			if (it->second[i].bonus_id == _id)
			{
				it->second[i].is_get = true;
			}
		}
	}
}
vector<Flat_BonusInfo> BonusManager::getAllBonus()
{
	vector<Flat_BonusInfo> ret;
	map<int,vector<Flat_BonusInfo> >::iterator it = bonusPool.begin();
	for (; it != bonusPool.end(); ++it)
	{
		ret.insert(ret.end(), it->second.begin(), it->second.end());
	}
	
	return ret;
}
vector<Flat_BonusInfo> BonusManager::getBonus(BonusType _type)
{
	map<int,vector<Flat_BonusInfo> >::iterator it = bonusPool.find(_type);
	if (it == bonusPool.end())
	{
		vector<Flat_BonusInfo> ret_null;
		return ret_null;
	}else
	{
		return it->second;
	}
}

bool BonusManager::getBonus(BonusType _type,unsigned int _id,Flat_BonusInfo& outData)
{
	vector<Flat_BonusInfo> data = getBonus(_type);

	for (int i=0,count=data.size(); i<count; i++)
	{
		if (data[i].bonus_id == _id)
		{
			outData = data[i];
			return true;
		}
	}

	return false;
}

bool BonusManager::haveNewBonus(BonusType type)
{
	if (type == BonusType_All){
		for (map<int,vector<Flat_BonusInfo> >::iterator it=bonusPool.begin(); it != bonusPool.end(); ++it)
		{
			for (int i=0,count=it->second.size(); i<count; ++i)
			{
				if (it->second.at(i).enable && ! it->second.at(i).is_get)
					return true;
			}
		}
	}else {
		map<int,vector<Flat_BonusInfo> >::iterator it = bonusPool.find(type);
		if (it != bonusPool.end())
		{
			for (int i=0; i!= it->second.size(); ++i)
			{
				if (it->second.at(i).enable && ! it->second.at(i).is_get)
					return true;
			}
		}
	}

	return false;
}