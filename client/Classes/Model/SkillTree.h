//#pragma once
//#include "CommonDefine.h"
//#include <vector>
//
//// 说明：符文和Skill的Type都放在SkillType一个枚举里面的。
//
//// 符文
//struct FuWen
//{
//	SkillType type;				// 符文的类型
//	unsigned int level;			// 符文需要的等级
//};
//
//// 一个技能的定义
//struct SkillItem
//{
//	SkillType type;					// 技能的类型
//	unsigned int level;				// 需要的等级
//	Profession forActorType;		// 哪个职业可以配备的，如果以后要一个技能可以配给多种角色的话，那么就改成vector即可
//	int fuWenIndex;					// 带有的符文的索引，-1就是没有符文
//	vector<FuWen> suppotedFuWen;	// 支持的符文，也就是这个技能可以有的符文
//
//	// 得到SkillType，根据当前的符文情况，返回枚举里面的值
//	SkillType toSkillType()
//	{
//		if (fuWenIndex > 0)
//			return suppotedFuWen[fuWenIndex].type;
//
//		return type;
//	}
//};
///**
// *	技能树，就是一个技能的配置结构。
// */
//class SkillTree
//{
//private:
//	vector<SkillItem> skillPool;
//public:
//	SkillTree(void);
//
//	// 获得Skill的名字
//	string getSkillName(SkillType skill);
//	// 获得Skill的描述
//	string getSkillDesc(SkillType skill);
//	// 获取某个职业的所有的技能
//	vector<SkillItem> getAllSkillForActor(Profession pro);
//
//	~SkillTree(void);
//};
//

#pragma once

#include "protocol.h"
#include <vector>
#include <string>
using namespace std;

struct SkillItem
{
	SkillType  skill_type;
	Profession actor_profession;
	int        level;
	string     name;
	string     describe;
};


class SkillTree
{
public:
	SkillTree();

	static SkillTree* sharedSkillTree();
	string getSkillNameByType(SkillType type);
	string getSkillNameAddInfo(SkillType type, bool simple=false);  // 法师 技能书：雷暴
	string getSkillDescribeByType(SkillType type);
	int getSkillLevelByType(SkillType type);
	SkillItem getSkillInfor(SkillType type);

	// 获得技能的plist文件
	static string getSkillPlistFile(SkillType type,string extrenName = ".plist");

	// itemType 跟 SKillType的互转
	SkillType itemsType2SkillType(int itemType);
	ItemsType skillType2ItemsType(int skillType);

private:

private:
	static SkillTree* s_skillTree;

	vector<SkillItem> mAllSkillVec;
};
