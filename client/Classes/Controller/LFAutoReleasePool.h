#pragma once
#include <map>
#include <stdlib.h>
#include "AutoReleaseAble.h"
using namespace std;

//class AutoReleaseAble; // 注意，这里不能只声明一下，必须得include，不然delete不会调用析构函数的

class ReleaseTrigle
{
public:
	virtual void do_release() = 0;
};

struct RetainItem
{
	AutoReleaseAble* pData;
	unsigned int retainCount;

	RetainItem(AutoReleaseAble* _pData)
	{
		pData = _pData;
		retainCount = 0;
	}
	RetainItem(AutoReleaseAble* _pData,unsigned int _count)
	{
		pData = _pData;
		retainCount = _count;
	}
	RetainItem& operator = (const RetainItem& obj){
		pData = obj.pData;
		retainCount = obj.retainCount;
		return *this;
	};
	void release()
	{
		if (pData != NULL)
		{
			delete pData; 
			pData = NULL;
		}
	}
};

/**
 * 自动释放池，释放任意非cocos2dx管理的AutoReleaseAble.
 * (cocos2dx对象调用了autorelease就不要用LFAutoReleasePool来管理)
 * 每一帧，就会检查retainCount是0的指针，然后自动释放
 */
class LFAutoReleasePool : public ReleaseTrigle
{
private:
	static LFAutoReleasePool* gPool;

	map<AutoReleaseAble*,RetainItem> itemPool;

	void do_release();
public:
	LFAutoReleasePool(void);
	// 获取全局的pool
	static LFAutoReleasePool* getPool();

	// 增加引用次数,如果是NULL，直接返回
	void retain(AutoReleaseAble* p);
	// 减少引用次数,如果是NULL，直接返回
	void release(AutoReleaseAble* p);
	// 不修改引用次数，如果没有在释放池，那么retaincount=0存放
	// 下一帧就会释放
	void autorelease(AutoReleaseAble* p);
	// 获取引用次数，不存在就返回-1
	int getRetainCount(AutoReleaseAble* p);

	// 清除所有的
	void clear();

	//// 检测合法性，如果p是NULL，或者是cocos2dx管理的，都是不合法的
	//bool isLegal(AutoReleaseAble* p);

	~LFAutoReleasePool(void);
};


//////////////////////////////////////////////////////////////////////////
// 下面的宏都只针对全局的LFAutoReleasePool
// 如果需要用单独的LFAutoReleasePool，请调用LFAutoReleasePool的对应的方法
//
// 推荐用法
// PP* p = new PP();
// LF_AUTORELEASE(p);   // 到这里相当于cocos2dx的一个create
// 
//
// LF_RETAIN(p);
// LF_RELEASE(p);		// 这里就释放了
//////////////////////////////////////////////////////////////////////////

// 增加引用次数,如果池中不存在就retainCount = 1
#define LF_RETAIN(P_DATA) LFAutoReleasePool::getPool()->retain(P_DATA)
// 减少引用次数,如果池中不存在就直接delete
#define LF_RELEASE(P_DATA) LFAutoReleasePool::getPool()->release(P_DATA)
// 放入自动释放池（retainCount = 0），可以重复调用，只有第一次调用才有效果
#define LF_AUTORELEASE(P_DATA) LFAutoReleasePool::getPool()->autorelease(P_DATA)
// 获取引用次数，不存在就返回-1
#define LF_RETAINCOUNT(P_DATA) LFAutoReleasePool::getPool()->getRetainCount(P_DATA)
// 清空所有
#define LF_CLEARALLRETAIN() LFAutoReleasePool::getPool()->clear()

