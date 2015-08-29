#include "SkillManager.h"
#include "MyselfManager.h"

SkillManager* SkillManager::manager = new SkillManager();

SkillManager::SkillManager(void)
{
}
SkillManager::~SkillManager(void)
{
	clearAll();
}

SkillManager* SkillManager::getManager()
{
	return manager;
}

void SkillManager::clearAll()
{
	int num = skills.size();
	for (int i=0;i<num;i++)
	{
		delete skills[i];
	}
	skills.clear();
}
void SkillManager::initSkills(vector<SkillInfo> _skills)
{
	// 要保证MySelfManager先初始化，这样就可以给角色附加skill
	CCAssert(MyselfManager::getManager()->getMyZhuJueData()!=NULL,"");
	clearAll();

	int num = _skills.size();
	for (int i=0;i<num;i++)
	{
		SkillData* data = new SkillData(_skills[i]);
		skills.push_back(data);
	}

	// 初始化技能
	{
		vector<BaseActor*> allActors = MyselfManager::getManager()->getAllTeamActors(Actor_All,LF_All,ACTOR_TYPE_ALL,false);
		int actorNum = allActors.size();
		for (int i=0;i<actorNum;i++)
		{
			// 挨个初始化,只有真身才有技能
			allActors[i]->initSkill(getSkillsOnActor(allActors[i]->getId()));
		}
	}

	CCLog("%s ----> init over!",__FUNCTION__);
}

vector<SkillData*> SkillManager::getSkillsOnActor(unsigned int actorId)
{
	vector<SkillData*> mRet;

	int num = skills.size();

	for (int i=0;i<num;i++)
	{
		if (skills[i] -> getInfor()->owner_hero_id == actorId)
		{
			mRet.push_back(skills[i]);
		}
	}
	return mRet;
}

vector<SkillData*> SkillManager::getAllSkills()
{
	return skills;
}

SkillData* SkillManager::getSkillData(int skill_id)
{
	int num = skills.size();
	for (int i=0;i<num;i++)
	{
		if (skills[i]->getInfor()->skill_id == skill_id)
		{
			return skills[i];
		}
	}

	return NULL;
}

void SkillManager::addNewSkill(SkillInfo skill)
{
	//CCAssert(skill.is_equiped == false,"");

	SkillData* data = new SkillData(skill);
	skills.push_back(data);

	// 附加技能
	if (skill.owner_hero_id != INVALID_ID)
	{
		//会自动把装备的技能设置成当前技能
		MyselfManager::getManager()->getMyTeamActor(skill.owner_hero_id)->attatchSkill(data);
	}
}

vector<SkillData*> SkillManager::getSkillsOnActor(int actor_id)
{
	return MyselfManager::getManager()->getMyTeamActor(actor_id)->getAllSkills();
}