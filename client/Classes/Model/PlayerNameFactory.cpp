#include "PlayerNameFactory.h"
#include <cstring>
#include "cocos2d.h"
// #include <fstream>
// using std::ifstream;

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#define FILE_FirstName_Male   "name_maleFirstName.txt"
#define FILE_FirstName_Female "name_femaleFirstName.txt"
#define FILE_LastName		  "name_lastName.txt"
#else
#define FILE_FirstName_Male   "config/name_maleFirstName.txt"
#define FILE_FirstName_Female "config/name_femaleFirstName.txt"
#define FILE_LastName		  "config/name_lastName.txt"
#endif


PlayerNameFactory* PlayerNameFactory::_playerNameFactory = 0;


PlayerNameFactory* PlayerNameFactory::getFactory()
{
	if (! _playerNameFactory) _playerNameFactory = new PlayerNameFactory;
	return _playerNameFactory;
}

void PlayerNameFactory::destroyFactory()
{
	if (_playerNameFactory){
		delete _playerNameFactory;
		_playerNameFactory = 0;
	}
}

PlayerNameFactory::PlayerNameFactory()
{
	srand(time(0));
#define MAX_BUF 256
// 	char temp[MAX_BUF] = {0};
// 	std::ifstream iFile;

	unsigned long fileSize = 0;
	char *pData = 0;
	char *part = 0;

	//male first name
	pData = (char*)cocos2d::CCFileUtils::sharedFileUtils()->getFileData(FILE_FirstName_Male, "r", &fileSize);
	if (pData && fileSize > 0){
		part = strtok(pData,"\n");
		while(part != NULL)
		{
			_maleFirstName.push_back(getNameWithout_NR(part));
			part = strtok(NULL,"\n");
		}

		delete [] pData;
		pData = 0;
	}

// 	iFile.open(cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(FILE_FirstName_Male).c_str());
// 	if (iFile.good()){
// 		while (iFile.getline(temp, MAX_BUF))
// 		{
// 			_maleFirstName.push_back(getNameWithout_NR(temp));
// 		}
// 		iFile.close();
// 	}

	//female first name
	pData = (char*)cocos2d::CCFileUtils::sharedFileUtils()->getFileData(FILE_FirstName_Female, "r", &fileSize);
	if (pData && fileSize > 0){
		part = strtok(pData,"\n");
		while(part != NULL)
		{
			_femaleFirstName.push_back(getNameWithout_NR(part));
			part = strtok(NULL,"\n");
		}

		delete [] pData;
		pData = 0;
	}

// 	iFile.open(cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(FILE_FirstName_Female).c_str());
// 	if (iFile.good()){
// 		while (iFile.getline(temp, MAX_BUF))
// 		{
// 			_femaleFirstName.push_back(getNameWithout_NR(temp));
// 		}
// 		iFile.close();
// 	}

	//last name
	pData = (char*)cocos2d::CCFileUtils::sharedFileUtils()->getFileData(FILE_LastName, "r", &fileSize);
	if (pData && fileSize > 0){
		part = strtok(pData,"\n");
		while(part != NULL)
		{
			_lastName.push_back(getNameWithout_NR(part));
			part = strtok(NULL,"\n");
		}

		delete [] pData;
		pData = 0;
	}

// 	iFile.open(cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(FILE_LastName).c_str());
// 	if (iFile.good()){
// 		while (iFile.getline(temp, MAX_BUF))
// 		{
// 			_lastName.push_back(getNameWithout_NR(temp));
// 		}
// 		iFile.close();
// 	}
}


string PlayerNameFactory::getNameWithout_NR(char *p)
{
	if (p[strlen(p) - 2] == '\r' ||
		p[strlen(p) - 2] == '\n'){
			p[strlen(p) - 2] = 0;
	}

	if (p[strlen(p) - 1] == '\r' ||
		p[strlen(p) - 1] == '\n'){
			p[strlen(p) - 1] = 0;
	}

	return p;
}

string PlayerNameFactory::getRandomName(bool male)
{
	string name;

	//last name
	int lastIdx = rand() % _lastName.size();
//	CCLOG("_lastName size = %d, lastIdx = %d", _lastName.size(), lastIdx);
	name = _lastName.at(lastIdx);
//	CCLOG("name = %d", lastIdx, name.c_str());
	//first name
	if (male){
		int firstIdx = rand() % _maleFirstName.size();
//		CCLOG("_maleFirstName size = %d, firstIdx = %d", _maleFirstName.size(), firstIdx);
		name += _maleFirstName.at(firstIdx);
	}else {
		int firstIdx = rand() % _femaleFirstName.size();
//		CCLOG("_femaleFirstName size = %d, firstIdx = %d", _femaleFirstName.size(), firstIdx);
		name += _femaleFirstName.at(firstIdx);
	}

//	CCLOG("name = %d", lastIdx, name.c_str());
	return name;
}
void PlayerNameFactory::addOneRepeatName(string name)
{
	_repeatName.push_back(name);
}