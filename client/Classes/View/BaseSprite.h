#pragma once

#include "cocos2d.h"
#include <string>
#include "ResourcePathManager.h"

using namespace std;
using namespace cocos2d;

class BaseSprite : public CCSprite
{
private:
	string mFileName;
	string mDelayFileName;
	void timeToDelayInit(CCNode* mySelf);
protected:
	ResourcePathManager* mResMgr;
public:
	BaseSprite(void);
	
	static BaseSprite* create(string fileName);
	static BaseSprite* create();

	string getFileName(){return mFileName;};
	virtual bool initWithFile(const char *pszFilename);
	virtual bool initWithFile(string fileName);
	virtual bool initWithFileDelay(string fileName,float delay);
	virtual void removeNode(CCNode* node);
	~BaseSprite(void);
};
