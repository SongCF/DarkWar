#pragma once
#include "include_json.h"
#include <vector>
#include <string>
#include "protocol.h"
#include "CommonDefine.h"

using namespace std;

class BaseActorPropertyParser
{
private:
	static BaseActorPropertyParser* parser;
	map<Profession,HeroFixedPro> mTemplateExtraPool;

	BaseActorPropertyParser(void);
public:
	static BaseActorPropertyParser* getParser();
	bool init();
	// 获得额外的数据
	HeroFixedPro getExtraProerty(Profession type,bool assertIfNotFound = true);
	~BaseActorPropertyParser(void);
};

