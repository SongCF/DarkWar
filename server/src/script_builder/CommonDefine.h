#pragma once

#include <string>
#include "protocol.h"

using namespace std;

#ifdef  _CLIENT_
#else
#include "logs.h"
#include <assert.h>
inline void assert_stub(bool conf, const char* str)
{
	assert(conf);
}
#define CCLog LogMsg
#define CCAssert assert_stub
#endif


// �˺�����
#define SHANG_HAI_MO_FA 0x0001		// ħ���˺� 1
#define SHANG_HAI_WU_LI 0x0010		// �����˺� 16

#define MAX_EQUIP_HIDE_ARRTIBUTE_NUM		5 // װ�����������������5
#define MAX_EQUIP_ZHUANKE_ARRTIBUTE_NUM		2 // װ����׭�����������2

enum EquipType
{
	EquipType_WuQi = 0,
	EquipType_FangJu,
	EquipType_ShiPin,
};

enum EquipRequirePlace
{
	POS_NONE = -1,
	POS_Hushenfu = 0,
	POS_XiongJia,
	POS_JieZhi1,
	POS_JieZhi2,
	POS_ZhuWuQi,
	POS_FuShou,
};

// �Ӽ�Ѫ���ľ�û�������ˣ�����������
struct EquipData
{
	unsigned long long zhuangbei_id;
	unsigned int group_id; //װ��ģ����
	ZhuangbeiType zhuangbei_type;
	string equipName;
	ZhuangbeiColour zhuangbei_colour;
	string equipPic;	// �ؼ���
	unsigned int profession_id;    	//�󶨵�ְҵID, Ϊ0���κ�ְҵ

	bool isInFengYin;	// �ڷ�ӡ���棬��ô���е����ݶ��������ˣ��������ͣ����֣���ɫ��
	bool isShenBing;	// �Ƿ������
	//װ����˭������
	unsigned int zhuangbei_hero_id;

	unsigned int zhuangbei_level;
	unsigned int hole;
	unsigned int diamond_type_1;
	unsigned int diamond_type_2;
	unsigned int diamond_type_3;

	unsigned int gongji_min;
	unsigned int gongji_max;
	unsigned int hujia;

	//ǿ����Ϣ
	QianghuaData qianghua_data;

	// װ����ħ������
	std::vector<ZBAttr> attr_vec;

	unsigned int jinglian_times;// �����Ĵ�������ʼ��0����������0��
	// �������ṩ������
	std::vector<ZBAttr> jinglian_attr_vec;
	// ����ӵ�еľ������ݣ���һ�׵Ĳ����ˣ�������
	std::vector<ZBAttr> jinglian_attr_will_have_vec;
	// ׭�̾����ṩ������
	std::vector<ZBAttr> zhuanke_attr_vec;

	//δ֪�����Եĸ�������û�н���������Ը���
	int unknownHideAttriNum;	
	//�Ѿ��⿪����������
	std::vector<ZBAttr> known_hide_attr_vec;

	//����ϴ������ 
	std::vector<ZBAttr> purify_attr_vec; 

	//��ħ��Ϣ��һ��װ�����Զ��⸽ħһ��
	bool has_fumo;
	ZBAttr fumo_attr;

	string equipDicrib;

	EquipData();
	// ��ȡ���е�ħ������
	vector<ZBAttr> getAttributeVec(bool includeAddtional = true) const;
	// ���ĳ��ħ�����Ե�ֵ�����includeAddtional����ô�Ѹ�ħ,������׭�̵Ķ�Ӧ����Ҳ����������
	// ͨ������Ҫ����ģ�����Ĭ�Ͼ���true
	unsigned int getAtrriValueByType(AttrType type,bool includeAddtional = true) const;
	void initWithZhungbeiDetail(const ZhuangbeiDetail* data);
	bool isEqual(EquipData data);
};

// �书�ı��
enum WuGong
{

};

// �̶�������
struct HeroFixedPro
{
	string baseName;		// ���ǻ��������֣�����Ұ���˾���yeManRen,�����Ҹ�����ص�ͼƬ�õ�
	string nameInChinese;	// �������֣�����Ұ���ˣ�����Ұ����
	//int gongJiJuLi;
	float gongJiJuLi;
	int mShangHaiLeiXing;
	bool hero_or_guaiwu;	// hero��true
	WuGong wugong_putong;
	WuGong wugong_baoji;
	// ֻ����hero�����ֵ���װ��˫��װ
	float hotPosXPercentOfOneHand;
	float hotPosYPercentOfOneHand;
	float hotPosXPercentOfTwoHand;
	float hotPosYPercentOfTwoHand;
	// ֻ���ڹ���
	float hotPosXPercentForGuaiWu;
	float hotPosYPercentForGuaiWu;

	HeroFixedPro();
};

// û���κ�װ������������Ӱ���µ��������� ---> HeroDetail
// ���Ǽ���õ�ֵ
struct HeroBaseProperty
{
	Profession profession;
	string customName;		// �Զ�������

	unsigned int hero_id;
	unsigned int vip_level;
	unsigned int parent_hero_id;
	unsigned int level;

	unsigned int currentStage;	// �ף���ǰ�Ľף�����Ļ�ͨ�����ÿ���������Ԫ��Ļ�����֮��ͨ���������

	// ��ǰЧ��ֵ������ת���ʣ��������Դ��100�����ת������80����ôֻ�ܴ��80
	unsigned int currentEffectPercent;	
	// �Ƿ���Ԫ��,Ԫ��û�м��ܵ�����,Ҳû��վλ��Ҫ�󣬲�����ֱ��ӳ�����ǵ�վλ����
	bool isYuanShen;	

	//����վλ��0��ʾδ����
	unsigned int postion;

	bool isLocked;	// �Ƿ�����ס��
	bool canUnlock;	// �ܷ����

	//
	unsigned int exp_current;
	unsigned int exp_need;

	unsigned int mLiLiang;		// ����
	unsigned int mMinJie;		// ����
	unsigned int mZhiLi;		// ����
	unsigned int mTiNeng;		// ���� 
	//����
	unsigned int vit_current;

	unsigned int mHP;			// ����ֵ,

	unsigned short resonance_star; //װ��ǿ���Ǽ�
	
	unsigned int mBaoJiLv;		// ������
	unsigned int mBaoJiShangHaiYinZi;	// �������˺����ӣ�����150%�����ǹ�����150
	unsigned int mJiNengFaChuLv;	// ���ܷ�������

	// �������ԣ�ְҵ���ˣ�����Ͷ��ˡ�
	int mShangHaiLeiXing;	// �˺�����,Ĭ����SHANG_HAI_MO_FA
	float mGongJiJuLi;		// ��������
	string baseName;		// ���ǻ��������֣�����Ұ���˾���br***,�����Ҹ�����ص�ͼƬ�õ�
	string nameInChinese;	// �������֣�����Ұ���ˣ�����Ұ����


	HeroBaseProperty();
	HeroBaseProperty(const HeroDetail* detail);

	//ˢ�����ݣ�ֻ��ˢ�·ǻ�����Ϣ������
	//��ΪҪ���������Ϣ��һ���ԣ������ҵ��ã�����bug
	void refeshPro(const HeroDetail* detail);

	// ��ʼ���̶�������
	void initFixedProValue(HeroFixedPro data);
};

typedef HeroBaseProperty BaseActorProperty;