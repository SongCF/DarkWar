#include "TexturePreLoad.h"
#include "GameInforEditer.h"
#include "SpriteHelper.h"
#include "Debug.h"

#define Default_TimeOut  10.0f	//10秒钟之后还没有找到图片就回调超时接口
#define IgnoreImageNum  5		//5张图片之后就可以忽略了，但是会延迟一段时间再忽略
#define IgnorePerImageDelay 2.0f	//每张延迟2秒钟

TexturePreLoad* TexturePreLoad::instance;

void TextAssert(bool condition,const char* msg)
{
	CCLog("TextAssert(%s,%s)",condition?"TRUE":"FALSE",msg);
#ifdef DEBUG_ENABLE_TEXTURE_ASSERT
	CCAssert(condition,msg);
#endif 
}

TexturePreLoad::TexturePreLoad()
{
	cache = CCTextureCache::sharedTextureCache();
	mNeedLoadTaskNum = 0;
	mLoadTextureCalledNum = 0;
	callback = NULL;
	tasks.clear();
	loadedText.clear();
	mBusy = false;
	mAutoClear = false;
	mTimeout = Default_TimeOut;
}

TexturePreLoad* TexturePreLoad::getInstance()
{
	if (!instance)
	{
		instance = new TexturePreLoad();
	}
	return instance;
}
bool TexturePreLoad::isBusy()
{
	return mBusy;
}
void TexturePreLoad::startSync()
{
	if (mBusy)
	{
		TextAssert(false,"任务还没有完成，不能再次开始");
		return;
	}
	mBusy = true;
	int _size = tasks.size();
	for (int i=0;i<_size;i++)
	{
		loadTexture(tasks[i],false);
	}
	tasks.clear();
	mNeedLoadTaskNum = 0;
	mBusy = false;
}
void TexturePreLoad::startAsync(TexturePreLoadCallBack* callback)
{
	if (mBusy)
	{
		TextAssert(false,"任务还没有完成，不能再次开始");
		return;
	}
	mBusy = true;
	// 不在一起载入了，而是一张一张的载入
	this->callback = callback;
	// 从第一张开始载入
	if ( tasks.size() > 0)
	{
		int _size = tasks.size();
		// 一次全部载入
		for (int i=0;i < _size;i++)
		{
			loadTexture(tasks[i],true);
		}

		// 看是否可以忽略剩下的图片
		if (mNeedLoadTaskNum <= IgnoreImageNum)
		{
			float delay = IgnorePerImageDelay*mNeedLoadTaskNum;

			CCLog("TexturePreLoad will ignore the rest %d image(s) after %f seconds",mNeedLoadTaskNum,delay);
			// 取消回调
			CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(
				schedule_selector(TexturePreLoad::timeToIgnoreTheRestImage),this);
			// 回调
			CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
				schedule_selector(TexturePreLoad::timeToIgnoreTheRestImage),this,delay,false);
		}

	}else
	{
		loadOver();
	}
}
void TexturePreLoad::cancelCallback()
{
	this->callback = NULL;
}
void TexturePreLoad::setAutoClearMemery()
{
	mAutoClear = true;
}

void TexturePreLoad::clearCurrentLoadedTexture()
{
	int _size = loadedText.size();
	for (int i=0;i<_size;i++)
	{
		cache->removeTextureForKey(loadedText[i].c_str());
	}
	loadedText.clear();
	// 正是IOS内存吃紧的解救办法
	CCDirector::sharedDirector()->purgeCachedData();
}	

//
void TexturePreLoad::addTask(vector<string> imageFiles)
{
	int num = imageFiles.size();
	for (int i=0;i<num;i++)
	{
		addTask(imageFiles[i]);
	}
}
void TexturePreLoad::addTask(string imageFile)
{
	if (mBusy)
	{
		TextAssert(false,"任务还没有完成，不能再次加入");
		return;
	}
	mLoadTextureCalledNum ++ ;
	mNeedLoadTaskNum ++;

	loadedText.insert(loadedText.end(),imageFile);
	tasks.insert(tasks.end(),imageFile);
}

void TexturePreLoad::loadTexture(string file,bool async)
{
#ifdef DEBUG_ENABLE_TEXTURE_LOAD_LOG
	CCLog("TexturePreLoad::loadTexture ----> to load texture : [%s]",file.c_str());
#endif

	if (async)
	{
		cache->addImageAsync(file.c_str(), this, callfuncO_selector(TexturePreLoad::textureLoadedCallback));
	}else
	{
		cache->addImage(file.c_str());
	}
}
void TexturePreLoad::triggerFirstTextureLoad(CCObject* tex)
{
	CCLog("TexturePreLoad -----> triggerFirstTextureLoad load Sucess!");
}
void TexturePreLoad::textureLoadedCallback(CCObject* tex)
{
	mNeedLoadTaskNum --;
	if (mNeedLoadTaskNum < 0)
	{
		mNeedLoadTaskNum = 0;
		// bug 修复 2013-4-17
		// BUG : 修复了，纹理在home之后，又回来时，再次回调的bug。
		// 应该是当时失败的纹理，在下次
		// 这个完全是可以忽略的
		TextAssert(false,"运行时出现问题了，多半是超时引起的");
	}
	int index = tasks.size() - mNeedLoadTaskNum -1;

	CCLog("texture loaded success --> [total = %d,left = %d]",tasks.size(),mNeedLoadTaskNum);
	
	if (callback)
	{
		callback->textureLoaded(mNeedLoadTaskNum,"NULL-Str");
	}

	if (mNeedLoadTaskNum <= 0)
	{
		loadOver();
	}
	// 看是否可以忽略剩下的图片
	else if (mNeedLoadTaskNum <= IgnoreImageNum)
	{
		float delay = IgnorePerImageDelay*mNeedLoadTaskNum;

		CCLog("TexturePreLoad will ignore the rest %d image(s) after %f seconds",mNeedLoadTaskNum,delay);
		// 取消回调
		CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(
			schedule_selector(TexturePreLoad::timeToIgnoreTheRestImage),this);
		// 回调
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
			schedule_selector(TexturePreLoad::timeToIgnoreTheRestImage),this,delay,false);
	}
}
void TexturePreLoad::timeToIgnoreTheRestImage(float t)
{
	// 取消回调
	CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(
		schedule_selector(TexturePreLoad::timeToIgnoreTheRestImage),this);

	CCLog("TexturePreLoad timeToIgnoreTheRestImage called ,the rest need load image num is %d",mNeedLoadTaskNum);

	loadOver();	
}
void TexturePreLoad::loadOver()
{
	// 如果不是忙碌过程，那么代表之前已经loadOver 过了，不再重复loadOver
	// 主要是预防：timeToIgnoreTheRestImage回调之后，真正的LoadImage之后又完成了的bug
	if (!mBusy)
		return;

	// 取消回调
	CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(
		schedule_selector(TexturePreLoad::timeToIgnoreTheRestImage),this);

	if (callback)
		callback->loadOver();
	tasks.clear();
	mNeedLoadTaskNum = 0;
	mLoadTextureCalledNum = 0;

	CCLog("TexturePreLoad loadOver !");

#ifdef DEBUG_ENABLE_TEXTURE_LOAD_LOG
	CCLog("TexturePreLoad texture load over !");
#endif
	
	if (mAutoClear)
	{
		// 取消回调
		CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(
			schedule_selector(TexturePreLoad::toClearSomeBuffer),this);
		// 回调
		CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
			schedule_selector(TexturePreLoad::toClearSomeBuffer),this,0.5f,false);

		mAutoClear = false;
	}
	mBusy = false;
}
void TexturePreLoad::toClearSomeBuffer(float t)
{
	// 取消回调
	CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(
		schedule_selector(TexturePreLoad::toClearSomeBuffer),this);
	
	CCLog("TexturePreLoad release some not use data ......");
	// 正是IOS内存吃紧的解救办法
	CCDirector::sharedDirector()->purgeCachedData();
}
