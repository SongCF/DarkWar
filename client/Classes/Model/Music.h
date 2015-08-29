#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include <string>
#include <list>

using namespace CocosDenshion;
using namespace std;

// 描述背景音乐的播放类型
enum MusicType
{
	MusicType_NULL,	// 无效
	MusicType_Main,
	MusicType_Play,
};

class Music
{
public:
	static Music* sharedMusicPlayer();
	void safeRelease();

	// 播放游戏外面的声音
	void playMainMusic();
	// 播放游戏里面的声音
	void playPlayMusic();
	void buttonClicked();

	void playBackgroundSound(std::string fileName,bool = true);
	void pauseBackgroundSound();
	void resumeBackgroundSound();
	void pauseAllEffects();
	void resumeAllEffects();
	void resumeEffectiveSound(unsigned int nSoundId);
	void stopBackgroundSound(bool release = true);
	void stopEffectSound(unsigned int nSoundId);
	void stopAllEffect();
	void pauseEffectSound(unsigned int nSoundId);

	void loadEffectSound(std::string fileName);
	unsigned int playEffectSound(std::string fileName,bool loop = false);
    void unLoadEffectSound(std::string fileName);
	void unLoadAllEffectSound();

	void setMute(bool mute = true);
	void setBackgroundMute(bool mute = true);
	void setEffectiveMute(bool mute = true);
	//[0,1.0]
	void setVolume(float volume);
	//[0,1.0]
	void setEffectVolume(float volume);
	//[0,1.0]
	void setBgVolume(float volume);

	// 获得一个音量大小对应的文件,只支持一位小数，而且是wav的
	string getDefaultVolumeFile(string file,float volume);

	bool isMute(){return mute;};
	bool isBackgroundMute(){return backgroudMute;};
	bool isEffectiveMute(){return effetiveMute;};
private:
	Music();
	
	list<string> preloadEffectSounds;
	float initBgVolume;
	float initEffectVolume;
	static Music* musicInstance;
	SimpleAudioEngine* simpleAudioEngine;
	bool mute;
	bool backgroudMute;
	bool effetiveMute;

	// 获取去掉扩展名的文件名
	string getFileNameWithoutExtentsion(string fileName);
	// 修改文件的扩展名
	// fileName:比如 kkk.wav
	// extensionNameWithDot : 比如.mp3
	// 新的fileName即为：kkk.mp3
	void changeExtensionName(string& fileName,string extensionNameWithDot);

	MusicType type;
};


#endif