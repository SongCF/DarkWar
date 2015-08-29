#include "LFAutoReleasePool.h"
#include "cocos2d.h"
#include <vector>
#include "GlobalTickTrigle.h"
using namespace cocos2d;
class ReleaseTrigleNode : public CCNode
{
private:
	vector<ReleaseTrigle*> trigles;
public:
	ReleaseTrigleNode()
	{
		init();
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this,SEL_CallFuncO(&ReleaseTrigleNode::tick),MSG_GLOBAL_TICK,NULL);
	}

	void tick(CCObject* null_obj)
	{
		for (int i=0,count=trigles.size(); i<count; i++)
		{
			trigles[i]->do_release();
		}
	}

	void addTrigle(ReleaseTrigle* t)
	{
		for (int i=0,count=trigles.size(); i<count; i++)
		{
			if (trigles[i] == t)
			{
				return;
			}
		}

		trigles.push_back(t);
	}

	void removeTrigle(ReleaseTrigle* t)
	{
		vector<ReleaseTrigle*>::iterator it = trigles.begin();
		for (; it != trigles.end(); it++)
		{
			if ( (*it) == t )
			{
				trigles.erase(it);
				break;
			}
		}
	}
};

LFAutoReleasePool* LFAutoReleasePool::gPool = NULL;
static ReleaseTrigleNode* gReleaseNode = new ReleaseTrigleNode();
//static ReleaseTrigleNode* gReleaseNode = NULL;



LFAutoReleasePool::LFAutoReleasePool(void)
{
	itemPool.clear();
	gReleaseNode->addTrigle(this);
}
LFAutoReleasePool* LFAutoReleasePool::getPool()
{
	if (gPool == NULL)
	{
		gPool = new LFAutoReleasePool();
	}
	return gPool;
}

//bool LFAutoReleasePool::isLegal(AutoReleaseAble* p)
//{
//	if (p == NULL)
//	{
//		return false;
//	}
//
//	//CCPoolManager::sharedPoolManager()->
//	// 是cocos2dx管理的
//	CCObject* tst = dynamic_cast<CCObject*>(p);
//	//tst->m_uAutoReleaseCount
//	if (tst != NULL)
//	{
//		return false;
//	}
//	return true;
//}

void LFAutoReleasePool::retain(AutoReleaseAble* p)
{
	if (p == NULL)
		return;

	map<AutoReleaseAble*,RetainItem>::iterator it = itemPool.find(p);
	if (it == itemPool.end())
	{// 没有找到
		RetainItem newItem(p,1);
		itemPool.insert(pair<AutoReleaseAble*,RetainItem>(p,newItem));
	}
	else
	{
		it->second.retainCount ++;
	}
}

void LFAutoReleasePool::autorelease(AutoReleaseAble* p)
{
	if (p == NULL)
		return;

	map<AutoReleaseAble*,RetainItem>::iterator it = itemPool.find(p);
	if (it == itemPool.end())
	{// 没有找到
		RetainItem newItem(p,0);
		itemPool.insert(pair<AutoReleaseAble*,RetainItem>(p,newItem));
	}
	else
	{
		// 存在就不管了
		return;
	}
}

void LFAutoReleasePool::release(AutoReleaseAble* p)
{
	if (p == NULL)
		return;

	map<AutoReleaseAble*,RetainItem>::iterator it = itemPool.find(p);
	if (it == itemPool.end())
	{// 没有找到
		//CCAssert(false,"不可能没找到");
		CCLog("Waring : %s --> data not found in release pool!");
		// 直接释放
		delete p;
	}
	else
	{
		CCAssert(it->second.retainCount >= 1,"没有retain的Item是不能释放的");
		it->second.retainCount --;

		// 释放移除
		if (it->second.retainCount == 0)
		{
			it->second.release();
			itemPool.erase(it);
		}
	}
}

int LFAutoReleasePool::getRetainCount(AutoReleaseAble* p)
{
	map<AutoReleaseAble*,RetainItem>::iterator it = itemPool.find(p);
	if (it != itemPool.end())
	{
		return it->second.retainCount;
	}
	return -1;
}

void LFAutoReleasePool::do_release()
{
	map<AutoReleaseAble*,RetainItem>::iterator it = itemPool.begin();
	for (; it!= itemPool.end();)
	{
		if (it->second.retainCount == 0)
		{// 释放移除
			it->second.release();
			// 这种移除方式GCC上编译不过,需要改成下面的方式
			// itemPool.erase(it++);	
			//it = itemPool.erase(it); 

			// 这相当于 : 
			// map<AutoReleaseAble*,RetainItem>::iterator temp = it; 
			// it++; 
			// itemPool.erase(temp);
			itemPool.erase(it++);	

			continue;
		}
		else
		{
			it++;
		}
	}
}

void LFAutoReleasePool::clear()
{
	map<AutoReleaseAble*,RetainItem>::iterator it = itemPool.begin();
	for (; it!= itemPool.end();it++)
	{
		it->second.release();
	}
	itemPool.clear();
}

LFAutoReleasePool::~LFAutoReleasePool(void)
{
	gReleaseNode->removeTrigle(this);
	clear();
}
