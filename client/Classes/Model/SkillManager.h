#pragma once
#include "protocol.h"
#include <vector>
using namespace std;

class SkillData
{
private:
	SkillInfo infor;
public:
	SkillData(SkillInfo _infor)
	{
		infor = _infor;
	}

	SkillInfo* getInfor()
	{
		return &infor;
	}
};

class SkillManager
{
private:
	static SkillManager* manager;
	vector<SkillData*> skills;
	SkillManager(void);

	// 获得学习的技能
	// 外界需要知道的话就直接找那个角色，去获得
	// 这里不提供全局查找，在返回
	// 这个方法主要用于游戏初始化的
	vector<SkillData*> getSkillsOnActor(unsigned int actorId);

	void clearAll();
public:
	static SkillManager* getManager();
	// 初始化，也会把数据加到Actor上面
	void initSkills(vector<SkillInfo> _skills);
	vector<SkillData*> getAllSkills();
	SkillData* getSkillData(int skill_id);

	vector<SkillData*> getSkillsOnActor(int actor_id);

	// 学会一个新技能,会自动加到别人身上的
	void addNewSkill(SkillInfo skill);

	~SkillManager(void);
};

