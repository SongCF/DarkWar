#include "LFDelayToMusic.h"
#include "Music.h"


LFDelayToMusic* LFDelayToMusic::create(float fDelay, string musicFile, bool isEffectSound/* = true*/)
{
	LFDelayToMusic *pMusic = new LFDelayToMusic();
	pMusic->initWithDuration(fDelay, musicFile,isEffectSound);
	pMusic->autorelease();
	return pMusic;
}

bool LFDelayToMusic::initWithDuration(float fDelay, string musicFile, bool isEffectSound)
{
	if (CCActionInterval::initWithDuration(fDelay))
	{
		m_sFileName = musicFile;
		m_bEffect = isEffectSound;
		m_bLoop = !isEffectSound;
		return true;
	}

	return false;
}

CCObject* LFDelayToMusic::copyWithZone(CCZone *pZone)
{
	CCZone* pNewZone = NULL;
	LFDelayToMusic* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (LFDelayToMusic*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new LFDelayToMusic();
		pZone = pNewZone = new CCZone(pCopy);
	}

	CCActionInterval::copyWithZone(pZone);

	pCopy->initWithDuration(m_fDuration, m_sFileName,m_bEffect);

	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void LFDelayToMusic::startWithTarget(CCNode *pTarget)
{
	CCActionInterval::startWithTarget(pTarget);
}

void LFDelayToMusic::update(float time)
{
	if (m_pTarget)
	{
		if (time == 1)
		{
			if (m_bEffect)
			{
				Music::sharedMusicPlayer()->playEffectSound(m_sFileName,m_bLoop);
			}
			else
			{
				Music::sharedMusicPlayer()->playBackgroundSound(m_sFileName,m_bLoop);
			}
		}
	}
}

CCActionInterval* LFDelayToMusic::reverse(void)
{
	CCLog("Warining,LFDelayToMusic reverse is the same as self!");
	return LFDelayToMusic::create(m_fDuration,m_sFileName,m_bEffect);
}


//////////////////////////////////////////////////////////////////////////
// LFImmediatelyMusic
//////////////////////////////////////////////////////////////////////////

void LFImmediatelyMusic::initWithFile(string musicFile,bool isEffect,bool loop)
{
	mMusicFile = musicFile;
	mIsEffect = isEffect;
	mLoop = loop;
}

void LFImmediatelyMusic::update(float time)
{
	CC_UNUSED_PARAM(time);
	if (mIsEffect)
	{
		Music::sharedMusicPlayer()->playEffectSound(mMusicFile,mLoop);
	}
	else
	{
		Music::sharedMusicPlayer()->playBackgroundSound(mMusicFile,mLoop);
	}
}
CCFiniteTimeAction * LFImmediatelyMusic::reverse(void)
{
	return (CCFiniteTimeAction*) (LFImmediatelyMusic::create(mMusicFile,mIsEffect,mLoop));
}
CCObject* LFImmediatelyMusic::copyWithZone(CCZone *pZone)
{
	CCZone *pNewZone = NULL;
	LFImmediatelyMusic *pRet = NULL;

	if (pZone && pZone->m_pCopyObject) {
		pRet = (LFImmediatelyMusic*) (pZone->m_pCopyObject);
	} else {
		pRet = new LFImmediatelyMusic();
		pZone = pNewZone = new CCZone(pRet);
	}

	CCActionInstant::copyWithZone(pZone);
	pRet->initWithFile(mMusicFile,mIsEffect,mLoop);
	CC_SAFE_DELETE(pNewZone);
	return pRet;
}
LFImmediatelyMusic * LFImmediatelyMusic::create(string musicFile,bool isEffect /*= true*/,bool loop /*= false*/)
{
	LFImmediatelyMusic *pRet = new LFImmediatelyMusic();
	pRet->initWithFile(musicFile,isEffect,loop);

	if (pRet) {
		pRet->autorelease();
	}
	return pRet;

}


