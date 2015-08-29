#include "MofaJinghuaManager.h"

MofaJinghuaManager* MofaJinghuaManager::manager = new MofaJinghuaManager();

MofaJinghuaManager::MofaJinghuaManager(void)
{
}

MofaJinghuaManager* MofaJinghuaManager::getManager()
{
	return manager;
}

void MofaJinghuaManager::init(vector<MofaJinghua> datas)
{
	allMoFaJingHua = datas;
}

vector<MofaJinghua> MofaJinghuaManager::getAllMoFaJingHua()
{
	vector<MofaJinghua> ret;
	int num = allMoFaJingHua.size();
	for (int i=0;i<num;i++)
	{
		if (allMoFaJingHua[i].count > 0)
		{
			ret.push_back(allMoFaJingHua[i]);
		}
	}

	return ret;
}

void MofaJinghuaManager::addOneJingHua(MofaJinghua jinghua)
{
	int num = allMoFaJingHua.size();
	// 有就更新
	for (int i=0;i<num;i++)
	{
		// 同一个
		if (allMoFaJingHua[i].attr_type == jinghua.attr_type &&
			allMoFaJingHua[i].zb_type == jinghua.zb_type &&
			allMoFaJingHua[i].level == jinghua.level)
		{
			allMoFaJingHua[i].count += jinghua.count;
			return;
		}
	}
	// 没有就新增
	allMoFaJingHua.push_back(jinghua);
}
void MofaJinghuaManager::removeOneJingHua(MofaJinghua jinghua)
{
	int num = allMoFaJingHua.size();
	for (int i=0;i<num;i++)
	{
		// 同一个
		if (allMoFaJingHua[i].attr_type == jinghua.attr_type &&
			allMoFaJingHua[i].zb_type == jinghua.zb_type &&
			allMoFaJingHua[i].level == jinghua.level)
		{
			if (allMoFaJingHua[i].count >= jinghua.count)
			{
				allMoFaJingHua[i].count -= jinghua.count;
			}else
			{
				allMoFaJingHua[i].count = 0;
				CCAssert(false,"不够用，不能remove");
			}
			return;
		}
	}

	CCAssert(false,"没有找到，不能remove");
}

MofaJinghuaManager::~MofaJinghuaManager(void)
{
}
