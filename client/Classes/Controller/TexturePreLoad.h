//
//  RedStarFall
//
//  Created by LiFeng.
//
//

#ifndef TexturePreLoad_h
#define TexturePreLoad_h

#include "cocos2d.h"
#include <vector>
#include "ResourcePathManager.h"

using namespace std;
using namespace cocos2d;

class TexturePreLoadCallBack
{
public:
	/**
	 *	图片载入就会回调，成功才回调
	 *	waitingTaskNum : 代表还在排队的任务数量，即没有完成的任务数量,则当前完成的任务索引是：[任务总量 - waitingTaskNum - 1]
	 */
	virtual void textureLoaded(int waitingTaskNum,string fileName) = 0;// 每载入一张图片都会回调，

	///**
	// *	载入超时就会回调，就不会再回调textureLoaded了。
	// */
	//virtual void textureTimeout(int taskTimeOutIndex,string fileName){};
	
	/**
	 *	图片载入完毕就会回调
	 *	如果有一张图片没有成功载入，比如超时了，那么不管怎样，loadover都会回调
	 *	对于没有载入的，就放弃了。
	 */
	virtual void loadOver() = 0;
};

//class TexturePreLoadOverCallBack
//{
//public:
//	/**
//	 *	图片载入完毕就会回调
//	 */
//	virtual void loadOver() = 0;
//};

/**
 *	一个图片载入类
 *	只能是每一个任务完成了之后再开始新的任务
 */
class TexturePreLoad : public CCObject
{
public:
	static TexturePreLoad* getInstance();

	void addTask(string imageFile);				// 添加任务	
	void addTask(vector<string> imageFiles);	// 添加任务
	// 强烈建议，在收到callback回调的时候，取消这个callback
	// 这样防止可能性的重复调用(cancelCallback)
	void startAsync(TexturePreLoadCallBack* callback);	// 开始异步载入图片
	void startSync();									// 开始同步载入
	void cancelCallback();								// 取消回调

	bool isBusy();										// 查看是否忙碌，忙碌则代表还没有完成任务

	void setAutoClearMemery();							// 设置自动清理，那么就会在载入后的

	void clearCurrentLoadedTexture();			// 清空自从上次清理以来载入的Texture
private:
	static TexturePreLoad* instance;

	TexturePreLoadCallBack* callback;

	vector<string> tasks;
	vector<string> loadedText;	// 载入的tasks集合，是tasks的超集
	bool mBusy;
	bool mAutoClear;
	float mTimeout;

	CCTextureCache* cache;
	int mNeedLoadTaskNum;					// 记录载入图片的任务数量,代表正在排队等候的数量
	long mLoadTextureCalledNum;				// 记录LoadTexture被调用的次数

	TexturePreLoad();

	void loadTexture(string file,bool async);	// 载入纹理
	void textureLoadedCallback(CCObject* tex);
	void loadOver();							// 载入完毕
	void toClearSomeBuffer(float t);

	// 忽略剩下的图片
	void timeToIgnoreTheRestImage(float t); 

	// 开始异步载入图片时，第一张图片的触发，回调
	// 因为有时候第一张会卡死。所有我们就把第一章载入2次
	// 头一次就回调triggerFirstTextureLoad
	// 第二次才真正的使用loadTexture的方法载入，而回调textureLoadedCallback
	//
	// 如果还不能解决这个bug，那么就不再按之前的方法(顺序的载入)而是直接全部载入
	// 到时候回到界面就是了
	void triggerFirstTextureLoad(CCObject* tex);
};

#endif
