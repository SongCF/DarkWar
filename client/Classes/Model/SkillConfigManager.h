/*
 技能配置信息（图片 技能名字 释放技能时的动画文件。。。）
*/

#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;

struct SkillConfigInfor
{
	unsigned int ID;
	unsigned int type;
	string name;
	string imgFile;
	string describe;
	string musicFile;
	//技能动画
	string armatureName;
	unsigned int animationHitFrameNum;

	SkillConfigInfor(){
		ID = 0;
	}
	const SkillConfigInfor& operator=(const SkillConfigInfor& _in){
		this->ID = _in.ID;
		this->type = _in.type;
		this->name = _in.name;
		this->imgFile = _in.imgFile;
		this->describe = _in.describe;
		this->musicFile = _in.musicFile;

		this->armatureName = _in.armatureName;
		this->animationHitFrameNum = _in.animationHitFrameNum;

		return *this;
	}
};

class SkillConfigManager
{
private:
	static SkillConfigManager* _skillInfoManager;
	SkillConfigManager();
	void initData();
public:
	static SkillConfigManager* getManager();
	~SkillConfigManager();

	const vector<SkillConfigInfor>* getAllSkillInfo();
	const SkillConfigInfor* getOneSkillInfo(unsigned int skillId);


	int getSkillType(unsigned int skillId);
	string getSkillName(unsigned int skillId);
	string getImgFileName(unsigned int skillId);
	string getDescribe(unsigned int skillId);
	string getMusicFile(unsigned int skillId, int idx);

	//战斗动画
	string getFightArmatureKeyName(unsigned int skillId);
	unsigned int getFightAnimationHitFrameNum(unsigned int skillId);



private:
	vector<SkillConfigInfor> _skillInfoVec;
};