#include "Music.h"
#include "SpriteHelper.h"
#include "LFUtile.h"

#define USE_NEW_FORMAT	1	// 使用新的编码格式，1就直接使用新格式，0就不管

// 音效的根目录
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
static string FORMAT = ".wav";
static string dirRoot = "";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
static string FORMAT = ".ogg";
static string dirRoot = "sounds/android/";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
static string FORMAT = ".wav";
static string dirRoot = "sounds/win32/";
#else
#undef USE_NEW_FORMAT
static string dirRoot = "sounds/";
#endif

Music* Music::musicInstance;
Music::Music()
{
	simpleAudioEngine = SimpleAudioEngine::sharedEngine();
	initBgVolume = 1.0f;
	initEffectVolume = 1.0f;
	preloadEffectSounds.clear();
	type = MusicType_NULL;
	setMute(false);

	CCLog("\n\n");
	CCLog("%s --> Music construct,Effect sound format = '%s'",__FUNCTION__,FORMAT.c_str());
	CCLog("\n\n");

}

Music* Music::sharedMusicPlayer()
{
	if (musicInstance == NULL)
	{
		musicInstance = new Music();
	}
	return musicInstance;
}
void Music::safeRelease()
{
	std::list<std::string>::iterator it;
	for (it=preloadEffectSounds.begin(); it!=preloadEffectSounds.end(); ++it) 
	{
		std::string fileName = (*it);
		simpleAudioEngine->unloadEffect(fileName.c_str());
	}
	preloadEffectSounds.clear();

	delete musicInstance;

	musicInstance = NULL;
}
// ============================ 游戏常用函数 =============================//
void Music::playMainMusic()
{
	resumeBackgroundSound();
	if (type == MusicType_Main)
		return;

	playBackgroundSound("bg.mp3");
	type = MusicType_Main;
}
void Music::playPlayMusic()
{
	resumeBackgroundSound();
	if (type == MusicType_Play)
		return;
	
	type = MusicType_Play;
	playBackgroundSound("fight_bg.mp3");
}
//
void Music::buttonClicked()
{
	playEffectSound("button.wav");
}
// ============================ 游戏常用函数 =============================//



void Music::playBackgroundSound(std::string fileName,bool loop /* = true*/)
{
	fileName = dirRoot + fileName;

	simpleAudioEngine->playBackgroundMusic( fileName.c_str(),loop);
	setBackgroundMute(backgroudMute);
}
void Music::pauseBackgroundSound()
{ 
	simpleAudioEngine->pauseBackgroundMusic();
}
void Music::pauseAllEffects()
{
	simpleAudioEngine->pauseAllEffects();
}
void Music::resumeAllEffects()
{
	simpleAudioEngine->resumeAllEffects();
}

void Music::resumeBackgroundSound()
{
	simpleAudioEngine->resumeBackgroundMusic();
}
void Music::stopBackgroundSound(bool release)
{
	type = MusicType_NULL;
	simpleAudioEngine->stopBackgroundMusic(release);
}

inline string Music::getFileNameWithoutExtentsion(string fileName)
{
	vector<string> tokens = LF::string_split(fileName,".");
	int size = tokens.size();
	if (size <= 1)
	{
		return fileName;
	}
	else
	{
		string newName = "";
		for (int i=0;i<size-1;i++)
		{
			newName += tokens[i];
		}
		return newName;
	}
}
inline void Music::changeExtensionName(string& fileName,string extensionNameWithDot)
{
	fileName = getFileNameWithoutExtentsion(fileName);
	fileName += extensionNameWithDot;
}

void Music::stopAllEffect()
{
	simpleAudioEngine->stopAllEffects();
}
void Music::loadEffectSound(std::string fileName)
{
#if USE_NEW_FORMAT == 1
	changeExtensionName(fileName,FORMAT);
#endif

	fileName = dirRoot + fileName;

	list<string>::iterator it = preloadEffectSounds.begin();
	for (;it != preloadEffectSounds.end();it++)
	{
		if ((*it) == fileName)
		{
			// 已经载入过了
			return;
		}
	}
	//CCLog("Music::loadEffectSound---->[%s]",fileName.c_str());
	simpleAudioEngine->preloadEffect(fileName.c_str());
	preloadEffectSounds.insert(preloadEffectSounds.end(),fileName);
}
unsigned int Music::playEffectSound(std::string fileName,bool loop/* = false*/)
{
#if USE_NEW_FORMAT == 1
	changeExtensionName(fileName,FORMAT);
#endif

	fileName = dirRoot + fileName;
	//loadEffectSound(fileName);
	return simpleAudioEngine->playEffect(fileName.c_str(),loop);
}
void Music::stopEffectSound(unsigned int nSoundId)
{
	simpleAudioEngine->stopEffect(nSoundId);
}
void Music::pauseEffectSound(unsigned int nSoundId)
{
	simpleAudioEngine->pauseEffect(nSoundId);
}
void Music::resumeEffectiveSound(unsigned int nSoundId)
{
	simpleAudioEngine->resumeEffect(nSoundId);
}
void Music::unLoadEffectSound(std::string fileName)
{
#if USE_NEW_FORMAT == 1
	changeExtensionName(fileName,FORMAT);
#endif

	fileName = dirRoot + fileName;

    simpleAudioEngine->unloadEffect(fileName.c_str());

	list<string>::iterator it = preloadEffectSounds.begin();
	for (;it != preloadEffectSounds.end();it++)
	{
		if ((*it) == fileName)
		{
			preloadEffectSounds.erase(it);
			break;
		}
	}
}
void Music::unLoadAllEffectSound()
{
	list<string>::iterator it = preloadEffectSounds.begin();
	for (;it != preloadEffectSounds.end();it++)
	{
		simpleAudioEngine->unloadEffect((*it).c_str());
	}
	preloadEffectSounds.clear();
}

void Music::setMute(bool mute /* = true */)
{
	setBackgroundMute(mute);
	setEffectiveMute(mute);
	//CCAssert(this->mute == mute,"Music::setMute error !");
}

void Music::setBackgroundMute(bool mute /* = true */)
{
	backgroudMute = mute;
	if (mute)
		simpleAudioEngine->setBackgroundMusicVolume(0.0f);
	else
		simpleAudioEngine->setBackgroundMusicVolume(initBgVolume);

	this->mute = backgroudMute&&effetiveMute;
	
}
void Music::setEffectiveMute(bool mute /* = true */)
{
	effetiveMute = mute;
	if (mute)
		simpleAudioEngine->setEffectsVolume(0.0f);
	else
		simpleAudioEngine->setEffectsVolume(initEffectVolume);

	this->mute = backgroudMute&&effetiveMute;
}

void Music::setVolume(float volume)
{
	setEffectVolume(volume);
	setBgVolume(volume);
}

void Music::setEffectVolume(float volume)
{
	simpleAudioEngine->setEffectsVolume(volume);
	initEffectVolume = volume;

	//simpleAudioEngine->
}
void Music::setBgVolume(float volume)
{
	simpleAudioEngine->setBackgroundMusicVolume(volume);
	initBgVolume = volume;
}

string Music::getDefaultVolumeFile(string file,float volume)
{
#if USE_NEW_FORMAT == 1
	changeExtensionName(file,FORMAT);
#endif

	file = dirRoot + file;

	string suffix = "";
	int pre_part = (int)volume;
	int end_part = (int)((volume - pre_part )*10);
	
	char format[64];
	sprintf(format,"_%d_%d.wav",pre_part,end_part);

	string newName = SpriteHelper::replace_all_distinct(file,".wav",format);

	//CCLog("Music::getDefaultVolumeFile volume = %f ,file = [%s]",volume,newName.c_str());

	return newName;
}
