#pragma once
#include <vector>
#include <list>
#include <string>
using namespace std;

// Buff��״̬Id����ͬ״̬��buff�Ͳ�Ӧ���ص����Ǹ���
enum BuffStateId
{
	//(һ�㽨���ǲ����Ƴ���)����һ�������������ڸ����ڽ�ɫ���ϣ����ǲ�֪�������ͻ��˿�����
	//������ʱ��߽�ɫ�����ݾͿ�������ʵ����
	BuffState_Controller = 0,	// ��״̬���Ե���
	BuffState_WhatEver = 1,		// ��״̬���Ե���
	BuffState_JianShou = 10,	// ����״̬
	BuffState_ShenZhiPiHu,		// ��֮�ӻ�״̬
	BuffState_WenYi,			// ����״̬
	BuffState_MangCi,			// ä��״̬
	BuffState_ShenZhiZhuFu,		// ��֮ף��״̬
	BuffState_Huo,				// ��״̬
	BuffState_KongJu,			// �־�״̬
	BuffState_LiuXue,			// ��Ѫ״̬
	BuffState_ShouHuZhe1,		// �ػ���״̬1
	BuffState_ShouHuZhe2,		// �ػ���״̬2
	BuffState_ZhongShenZhiLi,	// ����֮��״̬
	BuffState_ZhongDu,			// �ж�״̬
	BuffState_ShiHun,			// ʧ��״̬
	BuffState_BaoNu,			// ��ŭ״̬			
	BuffState_YinZhe,			// ����״̬			
	BuffState_TianTangShiZhe,	// ����ʹ��״̬	
	BuffState_JiYun,			// ����״̬
	BuffState_ShenZhiXinNian,	// ��֮����
};

// BuffEffect�Ƴ��㣬��Ҳ��ֱ�������޸ĵ����õ㣬Ŀǰֱ�������޸ĵ�ֻ��Ѫ���仯�������Ƴ�����Pos_Step��
// ��ͬ��Ӱ��ֵ����Ҫ���ò�ͬ���Ƴ��㣬��ôע���������Ƽ�������
enum Effect_Remove_Pos 
{
	Pos_Self_Fight_End		=	1,		// ���������֮�󣬴��˾����ˣ���������������Ѫ��
	Pos_Self_Will_fight		=	2,		// ����������ʱ��ѣ��
	Pos_Self_BeAttacked		=	3,		// ��������ʱ : ���ܣ����ף�Ѫ��
	Pos_Step				=	5,		// �κ�һ��Step��������Ѫ����������Ҳֻ֧��Ѫ���ı仯����persistent_hp_add
	Pos_ChangeRound			=	6,		// �غϱ����ʱ�򴥷��Ƴ�����Pos_Self_Will_fight֮ǰ
	Pos_None				=	127,	// ���Ƴ���һֱ����
};

// buff������Ӱ�죬�ͻ���ֻ�����ȥ�����ˣ�����Ҫ�����κμ��㡣
// ��ʱ��ʵ����ֻ���ͻ���һ��Id����buff�����;�����
// �ͻ����Լ��ж���ô��ʾ������
struct GameBuffEffect				// buff��һ��Ч��,Ҳ��һ�ε����ã����Ƴ���ʱ������á�
{
	int effect_id;					// id��һ��ս����Ψһ��ʶ����GameBuffPro����

	bool xuanyun;					// ѣ��,�Ƴ����Ǹ�fight��ʱ��Ҳ��
	int persistent_hp_add;			// Ѫ������(�������Ǽ�Ѫ),�������ļ�(��)Ѫ�����������ֻ��״̬,ֻ���ڳ����Ե�Ѫ����Ѫ
	int baoji_add;					// ����������
	int gongjili_add_lv;				// ��������
	//int hp_add;						// Ѫ�����ӣ�����Ѫ�������ͻ������ⲻ�ǳ����ԵĹ���Ҳ�������ļ�Ѫ����Ѫ������ֻ��һ������ֵ
	int shanbi_add;					// �����ʵ�����
	int hujia_add;					// ����ֵ���
	int jianshang_lv;				// ������ -----> ��֮����, ���ľ����������˺�
	int fantan_lv;					// �����ķ������˺�

	Effect_Remove_Pos remove_pos;	// hp_add ���Ƴ�����Pos_Step�������Ķ���Pos_Self_Fight_End��Pos_Self_BeAttacked

	GameBuffEffect(int _id)
	{
		effect_id = _id;
		xuanyun = false;
		persistent_hp_add = 0;
		baoji_add = 0;
		gongjili_add_lv = 0;
		//hp_add = 0;
		shanbi_add = 0;
		hujia_add = 0;
		jianshang_lv = 0;
		fantan_lv = 0;
		remove_pos = Pos_Step;
	}

	string getString()
	{
		char str[512];
		string log = "[effect_id = %d,remove_pos = %d";
		if (xuanyun)
			log += ",xuanyun = "+xuanyun?"true":"false";
		if(persistent_hp_add != 0)
		{
			sprintf(str,"%d",persistent_hp_add);
			log += ",persistent_hp_add = "+string(str);
		}
		if(baoji_add != 0)
		{
			sprintf(str,"%d",baoji_add);
			log += ",baoji_add = "+string(str);
		}
		if(shanbi_add != 0)
		{
			sprintf(str,"%d",shanbi_add);
			log += ",shanbi_add = "+string(str);
		}
		if(hujia_add != 0)
		{
			sprintf(str,"%d",hujia_add);
			log += ",hujia_add = "+string(str);
		}
		if(jianshang_lv != 0)
		{
			sprintf(str,"%d%%",jianshang_lv);
			log += ",jianshang_lv = "+string(str);
		}
		if(fantan_lv != 0)
		{
			sprintf(str,"%d%%",fantan_lv);
			log += ",fantan_lv = "+string(str);
		}
		log += "]";

		return log;
	}
};
// ������Ϸ��Buff������
// �����кܶ�Ч����Ч���������ò��Ƴ� 
struct GameBuff
{
	BuffStateId state;					// ״̬���ͣ���ͬ״̬��Ӧ���໥����
	bool cumulative;					// �Ƿ�����ۻ�
	list<GameBuffEffect> effect_vec;	// buff��Ч�����������õģ���һ���Ƴ��ˣ�������һ�������һ�����Ƴ��ˣ������Buff���Ƴ���
	
	unsigned int attachedToUId;	// �������ϵĽ�ɫ��uniqueId
	unsigned int fireUId;		// �ͷŸ�buff�Ľ�ɫuniqueId

	// Ĭ�ϲ��ɵ���
	GameBuff(BuffStateId _state,unsigned int _fireUId,unsigned int _attachedToUId,bool _cumulative = false)
	{
		state = _state;
		cumulative = _cumulative;
		fireUId = _fireUId;
		attachedToUId = _attachedToUId;
	}
	void initWithBuff(const GameBuff& buff)
	{
		this->cumulative = buff.cumulative;
		this->state = buff.state;
		this->fireUId = buff.fireUId;
		this->attachedToUId = buff.attachedToUId;
		this->effect_vec = buff.effect_vec;
	};


	bool haveEffctive(int _id)
	{
		for (list<GameBuffEffect>::iterator it = effect_vec.begin();it != effect_vec.end();it++)
		{
			if (it->effect_id == _id)
			{
				return true;
			}
		}
		return false;
	}
};


//////////////////////////////////////////////////////////////////////////
// �����Ǹ��ͻ����õ�
//////////////////////////////////////////////////////////////////////////

// 11B,ԭ��33B����Լ3��
struct ClientGameBuffEffect
{
	int effect_id;					// id��һ��ս����Ψһ��ʶ����GameBuffPro����
	bool xuanyun;
	bool fantan;					// ����
	// change_state��������Ҫ��*����������Ҫ��*
	char hp_change_state;			
	char baoji_change_state;		
	char gongjili_change_state;	
	char shanbi_change_state;	
	char fangyu_change_state;	
	

	ClientGameBuffEffect(const GameBuffEffect& eff)
	{
		effect_id = eff.effect_id;
		xuanyun = eff.xuanyun;
		fantan = eff.fantan_lv != 0;
		hp_change_state = calculateState(eff.persistent_hp_add);
		baoji_change_state = calculateState(eff.baoji_add);
		gongjili_change_state = calculateState(eff.gongjili_add_lv);
		shanbi_change_state = calculateState(eff.shanbi_add);

		if (eff.hujia_add > 0 || eff.jianshang_lv > 0)
		{
			fangyu_change_state = 1;
		}
		else if (eff.hujia_add < 0 && eff.jianshang_lv < 0)
		{
			fangyu_change_state = -1;
		}
		else
		{
			fangyu_change_state = 0;
		}
	}

	ClientGameBuffEffect(int _eff_id)
	{
		effect_id = _eff_id;
		xuanyun = false;
		fantan = false;					
		hp_change_state = 0;			
		baoji_change_state = 0;		
		gongjili_change_state = 0;	
		shanbi_change_state = 0;	
		fangyu_change_state = 0;	
	}

	char calculateState(int test)
	{
		if (test > 0)
		{
			return 1;
		}
		else if (test < 0)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	string getString()
	{
		char str[512];
		sprintf(str,"[effect_id = %d,",effect_id);
		string log = string(str);
		if (xuanyun)
			log += ",xuanyun = true";
		if (fantan)
			log += ",fantan = true";

		if(hp_change_state != 0)
		{
			sprintf(str,"%d",hp_change_state);
			log += ",hp_change_state = "+string(str);
		}
		if(baoji_change_state != 0)
		{
			sprintf(str,"%d",baoji_change_state);
			log += ",baoji_change_state = "+string(str);
		}
		if(gongjili_change_state != 0)
		{
			sprintf(str,"%d",gongjili_change_state);
			log += ",gongjili_change_state = "+string(str);
		}
		if(shanbi_change_state != 0)
		{
			sprintf(str,"%d",shanbi_change_state);
			log += ",shanbi_change_state = "+string(str);
		}
		if(fangyu_change_state != 0)
		{
			sprintf(str,"%d%%",fangyu_change_state);
			log += ",fangyu_change_state = "+string(str);
		}
		
		log += "]";

		return log;
	}
};

struct ClientGameBuff
{
	BuffStateId state;					// ״̬���ͣ���ͬ״̬��Ӧ���໥����
	bool cumulative;					// �Ƿ�����ۻ�
	unsigned int attachedToUId;			// �������ϵĽ�ɫ��uniqueId
	unsigned int fireUId;				// �ͷŸ�buff�Ľ�ɫuniqueId

	list<ClientGameBuffEffect> effect_vec;	// buff��Ч�����������õģ���һ���Ƴ��ˣ�������һ�������һ�����Ƴ��ˣ������Buff���Ƴ���


	ClientGameBuff(GameBuff& buf)
	{
		state = buf.state;
		cumulative = buf.cumulative;
		attachedToUId = buf.attachedToUId;
		fireUId = buf.fireUId;
		for (list<GameBuffEffect>::iterator it = buf.effect_vec.begin();it !=  buf.effect_vec.end();it++)
		{
			ClientGameBuffEffect eff(*it);
			effect_vec.push_back(eff);
		}
	}

	ClientGameBuff()
	{
	}

	void initWithBuff(const ClientGameBuff& buff)
	{
		this->cumulative = buff.cumulative;
		this->state = buff.state;
		this->attachedToUId = buff.attachedToUId;
		this->fireUId = buff.fireUId;
		this->effect_vec = buff.effect_vec;
	};
	bool haveEffctive(int _id)
	{
		for (list<ClientGameBuffEffect>::iterator it = effect_vec.begin();it != effect_vec.end();it++)
		{
			if (it->effect_id == _id)
			{
				return true;
			}
		}
		return false;
	}
	string getLog()
	{
		string ret = "";
		char stateStr[256];
		sprintf(stateStr,"BuffStateId = [%d]: cumulative-->[%s],fire_uid=[%u],attactToUid = [%u],effects-->",
			state,
			cumulative?"true":"false",
			fireUId,attachedToUId);
		ret += string(stateStr);

		for (list<ClientGameBuffEffect>::iterator it = effect_vec.begin();it != effect_vec.end();it ++)
		{
			ret += (*it).getString();
			ret += "\n";
		}
		return ret;
	}
};
