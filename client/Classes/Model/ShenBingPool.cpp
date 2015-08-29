#include "ShenBingPool.h"
#include "SuitPool.h"
#include "CS_Common.h"

ChuanqiPool* ChuanqiPool::pool = NULL;


bool ChuanQi::containsGroupId(unsigned int zbGroupId)
{
	for (int i=0,count=allChuanqiFenShenInLifeSpan.size(); i<count; i++)
	{
		if (allChuanqiFenShenInLifeSpan[i].zb_group_id == zbGroupId)
		{
			return true;
		}
	}
	return false;
}

// 获取该神兵某个等级的数据
// 没有就返回false
bool ChuanQi::getChuanqiDataByLevel(int chuanqiLevel,Flat_DuanzaoGroupZBInfo& out_data)
{
	bool levelOk = false;
	vector<int> levels = CS::getShenBingLevels();
	for (int i=0,count=levels.size(); i<count; i++)
	{
		if (levels[i] == chuanqiLevel)
		{
			levelOk = true;
			break;
		}
	}

	if (!levelOk)
	{
		return false;
	}

	for (int i=0,count=allChuanqiFenShenInLifeSpan.size(); i<count; i++)
	{
		if (allChuanqiFenShenInLifeSpan[i].level == chuanqiLevel)
		{
			out_data = allChuanqiFenShenInLifeSpan[i];
			return true;
		}
	}

	return false;
}

int ChuanQi::getSuitId()
{
	OneSuitInfo suit;
	// 把生命周期里面的所有groupId都拿去找，直到找到为止
	for (int i=0,count=allChuanqiFenShenInLifeSpan.size(); i<count; i++)
	{
		if (SuitPool::getPool()->getSuitInforByZBGroupId(allChuanqiFenShenInLifeSpan[i].zb_group_id,suit))
		{
			return suit.suit_id;
		}
	}
	return 0;
}

vector<unsigned int> ChuanQi::getGroupIds()
{
	vector<unsigned int> ret;
	for (int i=0,count=allChuanqiFenShenInLifeSpan.size(); i<count; i++)
	{
		ret.push_back(allChuanqiFenShenInLifeSpan[i].zb_group_id);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ChuanqiPool::ChuanqiPool(void)
{
}

ChuanqiPool* ChuanqiPool::getPool()
{
	if (pool == NULL)
	{
		pool = new ChuanqiPool();
	}
	return pool;
}

void ChuanqiPool::initChuanqiPoolWithEndingData(const vector<Flat_DuanzaoGroupZBInfo>& data)
{
	//CCAssert(allChuanQi.size() == 0,"保证都是空的情况来，避免外界乱调用");
	allChuanQi.clear();

	int addedNum = 0;
	for (int i=0,count=data.size(); i<count; i++)
	{
		CCAssert(data[i].profession_id <= 10,"");
		if (data[i].next_group_id == 0) // 尾巴
		{
			linkToChuanqiPool(data[i]);
			addedNum ++;
		}
	}
	CCAssert(addedNum == allChuanQi.size(),"校验");
}

void ChuanqiPool::combineToChuanqiFromEnding(ChuanQi* chuanqi,const vector<Flat_DuanzaoGroupZBInfo>& data)
{
	// 为了避免错误，shenBing里面目前应该只有一个最末数据
	CCAssert(chuanqi->allChuanqiFenShenInLifeSpan.size() == 1 && 
		chuanqi->allChuanqiFenShenInLifeSpan[0].next_group_id == 0,"");

	bool found = false;
	unsigned int behindeGroupId = chuanqi->allChuanqiFenShenInLifeSpan[0].zb_group_id;
	while (true)
	{
		found = false;
		for (int i=0,count=data.size(); i<count; i++)
		{
			// 当前装备的Next是当前的behindeGroupId，就连上
			if (data[i].next_group_id == behindeGroupId) 
			{
				chuanqi->allChuanqiFenShenInLifeSpan.push_back(data[i]);
				// 更新Id,并重新从头找这个装备的上级
				behindeGroupId = data[i].zb_group_id;
				found = true;
				break;
			}
		}

		// 一圈都没找到，那么就是头部了，没有上级了，找完了
		if (!found)
			break;
	}
}

void ChuanqiPool::linkToChuanqiPool(Flat_DuanzaoGroupZBInfo oneInfor)
{
	// 链接
	for (int i=0,count=allChuanQi.size(); i<count; i++)
	{
		for (int partIndex=0,partCount=allChuanQi[i].allChuanqiFenShenInLifeSpan.size(); partIndex<partCount; partIndex++)
		{
			// 如果新来的这个下一节的Id是在allShenbingInLifeSpan中，那么就是一个生命周期的，直接push进去
			// 如果新来的是原来list中需要的下一结，那么就push进去
			if (oneInfor.next_group_id == allChuanQi[i].allChuanqiFenShenInLifeSpan[partIndex].zb_group_id ||
				allChuanQi[i].allChuanqiFenShenInLifeSpan[partIndex].next_group_id == oneInfor.zb_group_id)
			{
				allChuanQi[i].allChuanqiFenShenInLifeSpan.push_back(oneInfor);
				return; // 直接返回，简单些
			}
		}
	}

	// 链接不成功，就新增
	ChuanQi newShenBingGroup;
	newShenBingGroup.allChuanqiFenShenInLifeSpan.push_back(oneInfor);
	allChuanQi.push_back(newShenBingGroup);

}

void ChuanqiPool::initChuanQiData(vector<Flat_DuanzaoGroupZBInfo> data)
{
	//for (int i=0,count=data.size(); i<count; i++)
	//{
	//	if (data[i].zb_group_id == 1647)
	//	{
	//		int a = 0;
	//	}
	//}

	//1. 先把尾部找到初始化神兵
	initChuanqiPoolWithEndingData(data);
	//2. 根据尾部挨个去链接
	for (int i=0,count=allChuanQi.size(); i<count; i++)
	{
		combineToChuanqiFromEnding(&allChuanQi[i],data);
	}

	//for (int i=0,count=allChuanQi.size(); i<count; i++)
	//{
	//	for (int j=0,count2=allChuanQi[i].allChuanqiFenShenInLifeSpan.size(); j<count2; j++)
	//	{
	//		if (allChuanQi[i].allChuanqiFenShenInLifeSpan[j].zb_group_id == 1647)
	//		{
	//			int a = 0;
	//		}
	//	}
	//	
	//}

	//for (int itemIndex=0,itemCount=data.size(); itemIndex<itemCount; itemIndex++)
	//{
	//	// 链接，但是有个问题
	//	// 比如A->B->C->D
	//	// 来的顺序是A,B,D,C
	//	// 那么AB一组，D一组，最后C来了，就是ABC一组，D一组
	//	// 这样就永久分开了，没法粘合了
	//	//
	//	// 不过目前的状况是最坏的情况也是怎么都会有3个粘在一块，分开一个。反正就是分2组
	//	// 
	//	// 如果链再长点呢：ABCDEFG
	//	// 最多就分4组，每各一个断一个（(N+1)/2 取整的个数）
	//	linkToShenBingPool(data[itemIndex]);
	//}
}

void ChuanqiPool::modifySuitPoolToSurportShenBing()
{
	for (int i=0,count=allChuanQi.size(); i<count; i++)
	{
		int suitId = allChuanQi[i].getSuitId();
		if (suitId > 0) // 是套装
		{
			SuitPool::getPool()->combineGroupIdsIntoSuitPart(allChuanQi[i].getGroupIds(),suitId);
		}
	}
}

//bool ChuanqiPool::isShenBing(unsigned int zbGroupId)
//{
//	ChuanQi not_use;
//	return getChuanQiData(zbGroupId,not_use);
//}

bool ChuanqiPool::getChuanQiData(unsigned int zbGroupId,Flat_DuanzaoGroupZBInfo& outData,int level)
{
	ChuanQi data;
	if (getChuanQiData(zbGroupId,data))
	{
		if (data.getChuanqiDataByLevel(level,outData))
		{
			return true;
		}
	}

	return false;
}
// 获取神兵的数据，没有就返回false
bool ChuanqiPool::getChuanQiData(unsigned int zbGroupId,ChuanQi& outData)
{
	for (int i=0 ,count = allChuanQi.size(); i<count; i++)
	{
		for (int partIndex=0,partCount=allChuanQi[i].allChuanqiFenShenInLifeSpan.size(); partIndex<partCount; partIndex++)
		{
			if (zbGroupId == allChuanQi[i].allChuanqiFenShenInLifeSpan[partIndex].zb_group_id)
			{
				outData = allChuanQi[i];
				return true;
			}
		}
		
	}
	return false;
}

ChuanqiPool::~ChuanqiPool(void)
{
}

string ChuanqiPool::getChuanQiSuitName(Profession prof)
{
	for (int i=0,count=allChuanQi.size(); i<count; ++i)
	{
		if (allChuanQi.at(i).allChuanqiFenShenInLifeSpan.at(0).profession_id == prof){
			OneSuitInfo info;
			if (SuitPool::getPool()->getSuitInforBySuitId(allChuanQi.at(i).getSuitId(), info)){
				return info.suit_name;
			}else {
				CCAssert(false, "怎么会找不到~");
			}
		}
	}
	return "";
}

const ChuanQi* ChuanqiPool::getChuanQiByEquipType(Profession prof, ZhuangbeiType type)
{
	for (int i=0,count=allChuanQi.size(); i<count; ++i)
	{
		if (allChuanQi.at(i).allChuanqiFenShenInLifeSpan.at(0).profession_id == prof &&
			allChuanQi.at(i).allChuanqiFenShenInLifeSpan.at(0).zhuangbei_type == type){
			return &allChuanQi.at(i);
		}
	}

	CCAssert(false, "怎么会找不到~");
	return 0;
}