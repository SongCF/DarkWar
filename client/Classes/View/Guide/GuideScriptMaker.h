#pragma once

#include "FightScriptBuilder.h"

// 引导脚本的生成器
// 生成需要的写死的脚本
class GuideScriptMaker
{
public:
	GuideScriptMaker(void);

	vector<FightScript> genYemanrenInGuideFight();
	vector<FightScript> genYemanrenWithYouxiaInGuideFight();

	vector<FightScript> genFashiInGuideFight();
	vector<FightScript> genFashiWithQishiInGuideFight();

	vector<FightScript> genCikeInGuideFight();
	vector<FightScript> genCikeWithYouXiaInGuideFight();


	vector<FightScript> getYemanrenInGuideFight();
	vector<FightScript> getYemanrenWithYouxiaInGuideFight();

	vector<FightScript> getFashiInGuideFight();
	vector<FightScript> getFashiWithQishiInGuideFight();

	vector<FightScript> getCikeInGuideFight();
	vector<FightScript> getCikeWithYouXiaInGuideFight();

	// 测试
	void testGenAll();
	void testLoadAll();

	~GuideScriptMaker(void);
private:
	bool saveScript(string fileName,vector<FightScript> data);
	bool loadScript(string fileName,vector<FightScript>& data);
};

