#pragma once
#include "cocos2d.h"
#include <string>

using namespace std;
USING_NS_CC;

// 一个到时间了就播放音乐的动画
class LFDelayToMusic : public CCActionInterval
{
public:
	/** creates the action */
	static LFDelayToMusic* create(float fDelay, string musicFile, bool isEffectSound = true);
	/** initializes the action */
	bool initWithDuration(float fDelay, string musicFile, bool isEffectSound);

	// 设置是否重复播放，只用于Effect Sound
	// 背景音乐默认是true，音效默认是false
	LFDelayToMusic* setLoop(bool loop);

	virtual CCObject* copyWithZone(CCZone* pZone);
	virtual void startWithTarget(CCNode *pTarget);
	virtual void update(float time);
	virtual CCActionInterval* reverse(void);
protected:
	string m_sFileName;
	bool m_bEffect;
	bool m_bLoop;
};


class LFImmediatelyMusic : public CCActionInstant //<NSCopying>
{
public:
	LFImmediatelyMusic(){}
	virtual ~LFImmediatelyMusic(){}

	virtual void initWithFile(string musicFile,bool isEffect,bool loop);

	//super methods
	virtual void update(float time);
	virtual CCFiniteTimeAction * reverse(void);
	virtual CCObject* copyWithZone(CCZone *pZone);
public:
	static LFImmediatelyMusic * create(string musicFile,bool isEffect = true,bool loop = false);
protected:
	string mMusicFile;
	bool mIsEffect;
	bool mLoop;
};




