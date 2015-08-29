#pragma once
#include <vector>
#include <string.h>
using namespace std;

struct EquipData;
class Hero;

// �ӳɻ���	�ӳ�����	�ӳɹ���	�ӳɸ�ֵ	�ӳ��˺�����ֵ
enum GM_PropertyIndex
{
	GM_HuJiaAddPer = 0,
	GM_HPAddPer,
	GM_GongJiAddPer,
	GM_GeDangZhiAdd,
	GM_BaoShangYinZiAdd,

	GM_PropertyNum,
};
#define		GongMingLevelMAX	10 // ������10������ͬ�ļ���Ҫ���starNum��һ��
struct GongMingConfig
{
	// �����ȼ���Ҫ����������1������levelNeedStarNumConfig[1]��Ҫ��
	unsigned int levelNeedStarNumConfig[GongMingLevelMAX+1];
	// �ȼ�1����ȡproperyAdd[1]������
	unsigned int properyAdd[GongMingLevelMAX+1][GM_PropertyNum];

	GongMingConfig()
	{
		// ���
		memset(properyAdd,0,sizeof(unsigned int)*(GongMingLevelMAX+1)*(GM_PropertyNum));
		memset(levelNeedStarNumConfig,0,sizeof(unsigned int)*(GongMingLevelMAX+1));
	}
};

/**
 * װ�����������ù�����
 */
class ZhuangbeiGongMingManager
{
private:
	GongMingConfig mConfig;
	static ZhuangbeiGongMingManager* mInstance;
	ZhuangbeiGongMingManager(void);
public:
	static ZhuangbeiGongMingManager* getManager();

	// û�г�ʼ������Ĭ��ֵ
	void initGongMingConfig(GongMingConfig config);

	// ��ȡ��ǰװ�����Ŵ����Ķ���Ǹ���
	// ǿ��һ�����ǣ�����1�㹲���Ǽ�
	// ǿ��һ�Ž��ǣ�����3�㹲���Ǽ�
	// Ŀǰ������Ԫ�������
	unsigned int getGongMingStarNum(const vector<EquipData>& euqipsOnHero);
	// starNum��ȡ��ǰ��level
	unsigned int getGongMingLevel(unsigned int starNum);

	// ����װ����ȡ��ǰ��ĳ�����Լӳ�
	unsigned int getPropertyAdd(const vector<EquipData>& euqipsOnHero,GM_PropertyIndex index);

	// ��ȡ�����Ĺ�����������
	GongMingConfig getGongMingConfig();

	~ZhuangbeiGongMingManager(void);
};

