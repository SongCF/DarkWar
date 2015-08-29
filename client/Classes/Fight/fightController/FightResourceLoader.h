#pragma once
#include <map>
#include "FightScriptDefine.h"

/**
 * 用于加载战斗资源的加载器。
 * 根据战斗脚本的定义，我们便可知道该战斗会遇到哪些资源，那么我们就可以加载这些资源。
 */
class FightResourceLoader
{
private:
	// 技能的资源映射
	map<SkillType,string> skill_res_mapping;

	// 获取技能对应的资源文件的key
	string getSkillPreLoadKey(SkillType skillType);

public:
	FightResourceLoader(void);
	// 根据脚本数据，载入资源
	void loadResource(vector<FightScript> scriptVec);
	~FightResourceLoader(void);
};

