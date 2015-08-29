#pragma once
#include <list>
#include <vector>
#include <memory.h>
#include "GameBuff.h"

using namespace std;

struct HeroBaseProperty;
struct EquipData;
struct GameBuff;
struct HeroPropertySummery;

// Ӣ�۵�����������Ϣ
// ȫ���Ǳ�������ԣ������κ�Ӱ�������
// ��֮��ȫ��Ӧ����HeroBaseProperty��HeroDetail
struct HeroPropertySummery
{
	bool is_yuanshen;					// �Ƿ���Ԫ��

	unsigned int profession;			// ְҵ
	unsigned int stage;					// ��
	unsigned int level;					// �ȼ�
	unsigned int base_hp;				// ����Ѫ�����������Ѫ����������װ���ȸ��ӵ�
	unsigned int liliang;				// ��������
	unsigned int mingjie;				// ��������
	unsigned int zhili;					// ��������
	unsigned int tineng;				// ��������
	unsigned int baojilv;				// ����������
	unsigned int baoshang_yinzi;		// ���������˺����ӣ�����200%������д200
	unsigned int effect_convert_per;	// ����Ч��ת���ʣ�Ĭ��100��100%

	// ��������
	unsigned int skill_type;			// �������ͣ�û�о���0
	unsigned int skill_level;			// ���ܵȼ�

	HeroPropertySummery()
	{
		memset(this,0,sizeof(HeroPropertySummery));
	};
};


// ��ɫ�������ݵĽṹ�壬PVP��PVE��
struct CalculateData
{
	HeroPropertySummery base_pro;
	vector<EquipData> allEquips;
	list<GameBuff> allBuffs;
	int mainPro;	// ������ֵ������װ��Ӱ���

	// ���������data���ݣ���������Ļ����Ϳ��ܰ�������Ԫ�������
	// �������CalculateData������allBuffsʼ���ǿ�
	// û��buff�����ݡ���Ҳ����ܣ���
	vector<CalculateData> includeCalDatas;

	CalculateData()
	{
		mainPro = 0;
	}
};
