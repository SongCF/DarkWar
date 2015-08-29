#pragma once
#include <vector>
#include <list>
#include <string>
using namespace std;

// Buff的状态Id，相同状态的buff就不应该重叠而是更新
enum BuffStateId
{
	//(一般建议是不可移除的)，是一个控制器，用于附着在角色身上，但是不知不觉，客户端看不到
	//这样临时提高角色的数据就可以轻松实现了
	BuffState_Controller = 0,	// 此状态可以叠加
	BuffState_WhatEver = 1,		// 此状态可以叠加
	BuffState_JianShou = 10,	// 坚守状态
	BuffState_ShenZhiPiHu,		// 神之庇护状态
	BuffState_WenYi,			// 瘟疫状态
	BuffState_MangCi,			// 盲刺状态
	BuffState_ShenZhiZhuFu,		// 神之祝福状态
	BuffState_Huo,				// 火状态
	BuffState_KongJu,			// 恐惧状态
	BuffState_LiuXue,			// 流血状态
	BuffState_ShouHuZhe1,		// 守护者状态1
	BuffState_ShouHuZhe2,		// 守护者状态2
	BuffState_ZhongShenZhiLi,	// 众神之力状态
	BuffState_ZhongDu,			// 中毒状态
	BuffState_ShiHun,			// 失魂状态
	BuffState_BaoNu,			// 暴怒状态			
	BuffState_YinZhe,			// 隐者状态			
	BuffState_TianTangShiZhe,	// 天堂使者状态	
	BuffState_JiYun,			// 击晕状态
	BuffState_ShenZhiXinNian,	// 神之信念
};

// BuffEffect移除点，【也是直接属性修改的作用点，目前直接属性修改的只有血量变化，而且移除点是Pos_Step】
// 不同的影响值，需要配置不同的移除点，那么注释里面有推荐的配置
enum Effect_Remove_Pos 
{
	Pos_Self_Fight_End		=	1,		// 自身攻击完毕之后，打了就算了：暴击，攻击力，血量
	Pos_Self_Will_fight		=	2,		// 即将攻击的时候：眩晕
	Pos_Self_BeAttacked		=	3,		// 自身被攻击时 : 闪避，护甲，血量
	Pos_Step				=	5,		// 任何一个Step，持续性血量。这里面也只支持血量的变化，即persistent_hp_add
	Pos_ChangeRound			=	6,		// 回合变更的时候触发移除，在Pos_Self_Will_fight之前
	Pos_None				=	127,	// 不移除，一直挂着
};

// buff的数据影响，客户端只须挂上去就是了，不需要进行任何计算。
// 到时其实可以只给客户端一个Id，和buff的类型就是了
// 客户端自己判断怎么显示就是了
struct GameBuffEffect				// buff的一个效果,也是一次的作用，在移除的时候才作用。
{
	int effect_id;					// id，一场战斗的唯一标识，由GameBuffPro生成

	bool xuanyun;					// 眩晕,移除点是该fight的时候，也用
	int persistent_hp_add;			// 血量增加(负数就是减血),是真正的加(减)血，而其他项就只是状态,只用于持续性掉血，加血
	int baoji_add;					// 暴击率增加
	int gongjili_add_lv;				// 攻击增加
	//int hp_add;						// 血量增加，用于血量反弹和回增，这不是持续性的哈。也会真正的加血，减血而不是只挂一个附加值
	int shanbi_add;					// 闪避率的增加
	int hujia_add;					// 护甲值添加
	int jianshang_lv;				// 减伤率 -----> 神之信念, 负的就是增加了伤害
	int fantan_lv;					// 攻击的反弹的伤害

	Effect_Remove_Pos remove_pos;	// hp_add 的移除点是Pos_Step，其他的都是Pos_Self_Fight_End或Pos_Self_BeAttacked

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
// 这是游戏的Buff的数据
// 里面有很多效果，效果依次作用并移除 
struct GameBuff
{
	BuffStateId state;					// 状态类型，相同状态不应该相互叠加
	bool cumulative;					// 是否可以累积
	list<GameBuffEffect> effect_vec;	// buff的效果，依次作用的，上一个移除了，才用下一个，最后一个都移除了，就这个Buff就移除了
	
	unsigned int attachedToUId;	// 被附加上的角色的uniqueId
	unsigned int fireUId;		// 释放该buff的角色uniqueId

	// 默认不可叠加
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
// 下面是给客户端用的
//////////////////////////////////////////////////////////////////////////

// 11B,原来33B，节约3倍
struct ClientGameBuffEffect
{
	int effect_id;					// id，一场战斗的唯一标识，由GameBuffPro生成
	bool xuanyun;
	bool fantan;					// 反弹
	// change_state负数就是要减*，正数就是要加*
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
	BuffStateId state;					// 状态类型，相同状态不应该相互叠加
	bool cumulative;					// 是否可以累积
	unsigned int attachedToUId;			// 被附加上的角色的uniqueId
	unsigned int fireUId;				// 释放该buff的角色uniqueId

	list<ClientGameBuffEffect> effect_vec;	// buff的效果，依次作用的，上一个移除了，才用下一个，最后一个都移除了，就这个Buff就移除了


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
