#include "CommonDefine.h"
#include "CS_Common.h"


inline unsigned int getZBAttriValueByType(AttrType type,const vector<ZBAttr>& attr_vec)
{
	unsigned int ret = 0;
	for (int i = 0,num = attr_vec.size(); i < num; i++)
	{
		if (attr_vec[i].attr_type == type)
		{
			ret += attr_vec[i].attr_value;
		}
	}
	return ret;
}
EquipData::EquipData()
{
	zhuangbei_id = 0;
	group_id = 0;
	profession_id = 0;
	zhuangbei_type = ZhuangbeiType_DanshouJian;
	zhuangbei_colour = ZhuangbeiColour_Baizhuang;
	zhuangbei_hero_id = 0;
	zhuangbei_level = 0;
	hole = 0;
	diamond_type_1 = 0;
	diamond_type_2 = 0;
	diamond_type_3 = 0;
	gongji_min = 0;
	gongji_max = 0;
	hujia = 0;
	jinglian_times = 0;
	unknownHideAttriNum = 0;
	isShenBing = false;
	isInFengYin = false;

	qianghua_data.max_qianghua_time = 0;
	qianghua_data.qianghua_count = 0;
	has_fumo = false;
	attr_vec.clear();
}

// ��ȡ���е�ħ������
vector<ZBAttr> EquipData::getAttributeVec(bool includeAddtional /*= true*/) const
{
	//CCAssert(!isInFengYin,"��ӡ״̬������");

	vector<ZBAttr> ret;

	ret.insert(ret.end(),attr_vec.begin(),attr_vec.end());

	if (includeAddtional)
	{
		if (has_fumo)
		{
			ret.push_back(fumo_attr);
		}

		ret.insert(ret.end(),jinglian_attr_vec.begin(),jinglian_attr_vec.end());
		ret.insert(ret.end(),zhuanke_attr_vec.begin(),zhuanke_attr_vec.end());
		ret.insert(ret.end(),known_hide_attr_vec.begin(),known_hide_attr_vec.end());
		ret.insert(ret.end(),purify_attr_vec.begin(),purify_attr_vec.end());
	}

	return ret;
}

// ���ĳ��ħ�����Ե�ֵ�����includeAddtional����ô�Ѹ�ħ,������׭�̵Ķ�Ӧ����Ҳ����������
// ͨ������Ҫ����ģ�����Ĭ�Ͼ���true
unsigned int EquipData::getAtrriValueByType(AttrType type,bool includeAddtional /*= true*/) const
{
	//CCAssert(!isInFengYin,"��ӡ״̬������");
	return getZBAttriValueByType(type,getAttributeVec(includeAddtional));
}

void EquipData::initWithZhungbeiDetail(const ZhuangbeiDetail* data)
{
	zhuangbei_id = data->zhuangbei_id;
	group_id = data->group_id;

	zhuangbei_type = (ZhuangbeiType)data->zhuangbei_type;
	equipName = data->zhuangbei_name;
	zhuangbei_colour = (ZhuangbeiColour)data->zhuangbei_colour;
	equipPic = data->zhuangbei_pic;

	//װ����˭������
	zhuangbei_hero_id = data->zhuangbei_hero_id;
	    
	profession_id = data->profession_id;

	if (profession_id > 10)
	{
		CCLog("Error : %s ---> profession bind,but profession_id > 10.zhuangbei_id = %lld, zhuangbei_type = %d",
			__FUNCTION__,zhuangbei_id,zhuangbei_type);
		CCAssert(false,"");
	}

	zhuangbei_level = data->zhuangbei_level;
	hole = data->hole;
	diamond_type_1 = data->diamond_type_1;
	diamond_type_2 = data->diamond_type_2;
	diamond_type_3 = data->diamond_type_3;

	gongji_min = data->gongji_min;
	gongji_max = data->gongji_max;
	hujia = data->hujia;

	qianghua_data = data->qianghua_data;

	has_fumo = data->has_fumo;
	fumo_attr = data->fumo_attr;

	int count = data->common_attr.attr_count;
	ZBAttr* attr_p = (ZBAttr*)data->common_attr.attr_arr;
	for(int i=0; i<count; ++i)
	{
		ZBAttr oneAttr;
		oneAttr.attr_type = attr_p[i].attr_type;
		oneAttr.attr_value = attr_p[i].attr_value;
		oneAttr.cuiqu_suc_ratio = attr_p[i].cuiqu_suc_ratio;
		attr_vec.push_back(oneAttr);
	}
	attr_p += attr_vec.size();
	char* desc_p = (char*)attr_p;
	equipDicrib = desc_p;

	unknownHideAttriNum = 0;

	// ����
	if (zhuangbei_colour == ZhuangbeiColour_Chuanqi 
        || zhuangbei_colour == ZhuangbeiColour_Xiyou)
	{
		JinglianData* jinglian = (JinglianData*) (((char*)attr_p) + equipDicrib.length()+1);
		jinglian_times = jinglian->jinglian_times;
		int jinglian_attr_num = sizeof(jinglian->jinglian_attr_arr)/sizeof(ZBAttr); // Ӧ����2
		for (int jinglian_attr_index = 0;jinglian_attr_index < jinglian_attr_num;jinglian_attr_index++)
		{
			if (jinglian->jinglian_attr_arr[jinglian_attr_index].attr_type > 0)
			{
				jinglian_attr_vec.push_back(jinglian->jinglian_attr_arr[jinglian_attr_index]);
			}
		}

		/*int jinglian_next_attr_num = sizeof(jinglian->next_jinglian_attr_arr)/sizeof(ZBAttr); // Ӧ����2
		for (int jinglian_next_attr_index = 0;jinglian_next_attr_index < jinglian_next_attr_num;jinglian_next_attr_index++)
		{
			if (jinglian->next_jinglian_attr_arr[jinglian_next_attr_index].attr_type > 0)
			{
				jinglian_attr_will_have_vec.push_back(jinglian->next_jinglian_attr_arr[jinglian_next_attr_index]);
			}
		}*/


        //ϴ��
        unsigned char* purify_hole = (unsigned char*)(jinglian +1);
            
        if(*purify_hole > 0) {
            ZBAttr* purify_attr = (ZBAttr*)(purify_hole +1);
            for(int i=0; i<*purify_hole; ++i) {
                purify_attr_vec.push_back(purify_attr[i]);
            }
            purify_hole += (*purify_hole)*sizeof(ZBAttr);
        }

		isInFengYin = !(*((bool*)(purify_hole + 1))); // ��ӡ is_unlocked
		
        if(zhuangbei_colour == ZhuangbeiColour_Chuanqi) {
    		// ���
    		char* shenbing = (char*)(purify_hole+2);
            
    		isShenBing = *((bool*)shenbing);

    		// ����ȫ��������߼�
    		isShenBing = true;
    		

    		if (isShenBing)
    		{
    			unsigned char identified_arr_count = *((unsigned char*)(shenbing+ 1));
                
    			unknownHideAttriNum = 5-identified_arr_count; // ֻ��5�����Ƕ�����

    			ZBAttr* hide_attr = (ZBAttr*)(shenbing + 2);
    			for (int hide_index = 0;hide_index < identified_arr_count;hide_index++)
    			{
    				known_hide_attr_vec.push_back(hide_attr[hide_index]);
    			}

    			char* zhuanke = ((char*)hide_attr) + identified_arr_count*sizeof(ZBAttr);
    			bool hasZhuanKe = *((bool*)zhuanke);
    			if (hasZhuanKe)
    			{
    				ZBAttr* jingwen_attr_arr = (ZBAttr*)(zhuanke + 1);
    				int jingwen_attr_count = 2;
    				for (int jingwen_index = 0;jingwen_index < jingwen_attr_count;jingwen_index++)
    				{
    					if (jingwen_attr_arr[jingwen_index].attr_type > 0)
    					{
    						zhuanke_attr_vec.push_back(jingwen_attr_arr[jingwen_index]);
    					}
    				}
    			}

    			unknownHideAttriNum = 5-known_hide_attr_vec.size(); // ֻ��5�����Ƕ�����
    		}
        }
	}

	CCAssert(unknownHideAttriNum >= 0,"");
};

bool EquipData::isEqual(EquipData data)
{
	if (zhuangbei_id != data.zhuangbei_id ||  
		group_id != data.group_id ||
		isInFengYin != data.isInFengYin ||
		isShenBing != data.isShenBing ||
		jinglian_times != data.jinglian_times ||
		unknownHideAttriNum != data.unknownHideAttriNum ||
		zhuangbei_type != data.zhuangbei_type ||
		equipName != data.equipName ||
		zhuangbei_colour != data.zhuangbei_colour ||
		equipPic != data.equipPic ||
		zhuangbei_hero_id != data.zhuangbei_hero_id ||
		zhuangbei_level != data.zhuangbei_level ||
		hole != data.hole ||
		diamond_type_1 != data.diamond_type_1 ||
		diamond_type_2 != data.diamond_type_2 ||
		diamond_type_3 != data.diamond_type_3 ||
		gongji_max != data.gongji_max ||
		gongji_min != data.gongji_min ||
		hujia != data.hujia ||
		memcmp(&qianghua_data,&data.qianghua_data,sizeof(QianghuaData)) != 0 ||
		has_fumo != data.has_fumo ||
		memcmp(&fumo_attr,&data.fumo_attr,sizeof(ZBAttr)) != 0 ||
		attr_vec.size() != data.attr_vec.size() ||
		jinglian_attr_vec.size() != data.jinglian_attr_vec.size() ||
		known_hide_attr_vec.size() != data.known_hide_attr_vec.size() ||
		zhuanke_attr_vec.size() != data.zhuanke_attr_vec.size() ||
		equipDicrib != data.equipDicrib)
	{
		return false;
	}

	// У��ÿһ������
	int count = attr_vec.size();
	for (int i=0;i<count;i++)
	{
		if ( memcmp(&attr_vec[i],&data.attr_vec[i],sizeof(ZBAttr) != 0) )
		{
			return false;
		}
	}

	for (int jinglian_cout = jinglian_attr_vec.size(),i=0;i<jinglian_cout;i++)
	{
		if ( memcmp(&jinglian_attr_vec[i],&data.jinglian_attr_vec[i],sizeof(ZBAttr) != 0) )
		{
			return false;
		}
	}

	for (int zhuanke_count = zhuanke_attr_vec.size(),i=0;i<zhuanke_count;i++)
	{
		if ( memcmp(&zhuanke_attr_vec[i],&data.zhuanke_attr_vec[i],sizeof(ZBAttr) != 0) )
		{
			return false;
		}
	}

	for (int hide_count = known_hide_attr_vec.size(),i=0;i<hide_count;i++)
	{
		if ( memcmp(&known_hide_attr_vec[i],&data.known_hide_attr_vec[i],sizeof(ZBAttr) != 0) )
		{
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

HeroFixedPro::HeroFixedPro()
{
	hotPosXPercentOfOneHand = 0.5f;
	hotPosYPercentOfOneHand = 0.5f;
	hotPosXPercentOfTwoHand = 0.5f;
	hotPosYPercentOfTwoHand = 0.5f;
	hotPosXPercentForGuaiWu = 0.5f;
	hotPosYPercentForGuaiWu = 0.5f;
}






//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
HeroBaseProperty::HeroBaseProperty()
{
	//memset(this,0,sizeof(HeroBaseProperty));
}
HeroBaseProperty::HeroBaseProperty(const HeroDetail* detail)
{
	//�㶨�����
	hero_id = detail->basic_info.hero_id;
	profession = (Profession)detail->basic_info.profession_id; // ��ɫ����
	parent_hero_id = detail->parent_hero_id;
	customName = detail->basic_info.name;	// �Զ�������
	isYuanShen = detail->basic_info.is_yuanshen;

	refeshPro(detail);

	mGongJiJuLi = 0;			
	mShangHaiLeiXing = SHANG_HAI_WU_LI;
	baseName = "";
	nameInChinese = "";
}

void HeroBaseProperty::refeshPro(const HeroDetail* detail)
{
	CCAssert(hero_id == detail->basic_info.hero_id,"");
	CCAssert(parent_hero_id == detail->parent_hero_id,"");
	CCAssert(customName == detail->basic_info.name,"");
	CCAssert(profession == (Profession)detail->basic_info.profession_id,"");
	CCAssert(isYuanShen == detail->basic_info.is_yuanshen,"");

	//����վλ��0��ʾδ����
	postion = detail->postion;
	isLocked = !detail->is_unlocked;
	canUnlock = detail->unlockAble;
	level = detail->basic_info.level;
	vip_level = detail->basic_info.vip_level;
	//
	exp_current = detail->exp_current;
	exp_need = detail->exp_need;

	currentStage = detail->basic_info.stage;
	currentEffectPercent = 100;
	if (isYuanShen)
	{
		currentEffectPercent = CS::transferRatioOfYuanshenLevel(level);
	}

	mLiLiang = detail->hero_attr_without_zhuangbei.liliang;		// ����
	mMinJie = detail->hero_attr_without_zhuangbei.mingjie;		// ����
	mZhiLi = detail->hero_attr_without_zhuangbei.zhili;			// ����
	mTiNeng = detail->hero_attr_without_zhuangbei.tineng;		// ���� 
	vit_current = detail->vit_current;							// ����

	mHP = detail->hero_attr_without_zhuangbei.xueliang;			// ����ֵ
	mBaoJiLv = (detail->hero_attr_without_zhuangbei.baoji);		// ������
	mBaoJiShangHaiYinZi = (detail->hero_attr_without_zhuangbei.baoshang);	// �������˺����ӣ�����150%�����ǹ�����150
	mJiNengFaChuLv = (detail->hero_attr_without_zhuangbei.jinengchufa);	// ���ܷ�������
}

// ��ʼ���̶�������
void HeroBaseProperty::initFixedProValue(HeroFixedPro data)
{
	mGongJiJuLi = data.gongJiJuLi;			// ��������������,��ʱ�Ż�ȷ����������Ҫ��������֮��Ķ����Դ���ֻ�ܴ򹥻�������ͬ�Ķ���
	mShangHaiLeiXing = data.mShangHaiLeiXing;	// �˺�����,Ĭ����ERROR_SHANG_HAI
	baseName = data.baseName;
	nameInChinese = data.nameInChinese;
}