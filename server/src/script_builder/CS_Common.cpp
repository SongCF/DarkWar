#include "CS_Common.h"
#include "CommonDefine.h"
#include "protocol.h"

#ifdef  _CLIENT_
#include "EquipmentManager.h"
#include "SkillTree.h"
#include "cocos2d.h"
#include "EquipmentConfigInfoManager.h"
#include "Debug.h"
typedef void (*LogFunPt)(const char * pszFormat, ...);
LogFunPt LogMsg = CCLog;
#else
#include "logs.h"
#include <assert.h>
#define CCLog LogMsg
#define CCAssert assert_stub

#endif


void CS::getProfesionBloodAndGongJiAddPercent(unsigned int profession, int& bloodAdd,int& gongjiAdd)
{
	const int low	=	0;		// 低
	const int midel =	30;		// 中
	const int hight =	60;		// 高

	bloodAdd = 0;
	gongjiAdd = 0;

	switch (profession)
	{
	case Profession_Yemanren:
		bloodAdd = hight;
		gongjiAdd = midel;
		break;
	case Profession_Chike:
		bloodAdd = midel;
		gongjiAdd = hight;
		break;
	case Profession_Fashi:
		bloodAdd = low;
		gongjiAdd = hight;
		break;
	case Profession_Qishi:
		bloodAdd = hight;
		gongjiAdd = midel;
		break;
	case Profession_ShenJianShou:
		bloodAdd = low;
		gongjiAdd = midel;
		break;
	case Profession_MiShu:
		bloodAdd = low;
		gongjiAdd = midel;
		break;
	case Profession_WuSeng:
		bloodAdd = midel;
		gongjiAdd = low;
		break;
	case Profession_YouXia:
		bloodAdd = midel;
		gongjiAdd = hight;
		break;
	case Profession_LieShou:
		bloodAdd = low;
		gongjiAdd = midel;
		break;
	case Profession_WuShi:
		bloodAdd = low;
		gongjiAdd = low;
		break;
	default:
		CCLog("Error : getProfesionBloodAndGongJiAddPercent --> profession_id = %d",profession);
		CCAssert(false,"");
		break;
	}
}

float CS::getHeroGongjiJuLiByProfession(unsigned int profession)
{
	float juli = 1.0f;

	switch (profession)
	{
	case Profession_Yemanren:
		juli = 1.0f;
		break;
	case Profession_Chike:
		juli = 1.0f;
		break;
	case Profession_Fashi:
		juli = 2.0f;
		break;
	case Profession_Qishi:
		juli = 1.0f;
		break;
	case Profession_ShenJianShou:
		juli = 10.0f; // 10.0 就可以打全屏了，只要是4.5及其以上就可以了
		break;
	case Profession_MiShu:
		juli = 2.0f;
		break;
	case Profession_WuSeng:
		juli = 1.0f;
		break;
	case Profession_YouXia:
		juli = 1.0f;
		break;
	case Profession_LieShou:
		juli = 2.5f;
		break;
	case Profession_WuShi:
		juli = 2.0f;;
		break;
	default:
		CCLog("Error : getHeroGongjiJuLiByProfession --> profession_id = %d",profession);
		CCAssert(false,"");
		break;
	}

	return juli;
}

char* CS::ntoh_zhuangbei_detail_2_equipdata(ZhuangbeiDetail* detail_arr,int count,std::vector<EquipData>& out)
{
#if (defined(_CLIENT_) && !defined(SHOW_DEBUG_LABLE))
	EquipmentConfigInfoManager *_equipInfoConfig = EquipmentConfigInfoManager::getManager();
#endif

	// 装备个数
	int zhuangbeiNum = count;
	     
	ZhuangbeiDetail* zhuangbeiArray = detail_arr;
	for (int i=0;i<zhuangbeiNum;i++)
	{
		char* nextReadPos = NULL; 
		// 一个detail的转换
		{
			bool isChuanqi = false;
#ifdef _CLIENT_
			zhuangbeiArray->zhuangbei_id = LF::lf_htonll(zhuangbeiArray->zhuangbei_id);
#else
			zhuangbeiArray->zhuangbei_id = Utl::htonll(zhuangbeiArray->zhuangbei_id);
#endif
			zhuangbeiArray->group_id = htonl(zhuangbeiArray->group_id);

			zhuangbeiArray->zhuangbei_type = htonl(zhuangbeiArray->zhuangbei_type);
			zhuangbeiArray->zhuangbei_colour =htonl(zhuangbeiArray->zhuangbei_colour);
			isChuanqi = zhuangbeiArray->zhuangbei_colour == ZhuangbeiColour_Chuanqi;
			zhuangbeiArray->zhuangbei_hero_id = htonl(zhuangbeiArray->zhuangbei_hero_id);
			zhuangbeiArray->profession_id = htonl(zhuangbeiArray->profession_id);

			zhuangbeiArray->zhuangbei_level = htonl(zhuangbeiArray->zhuangbei_level);
			zhuangbeiArray->hole = htonl(zhuangbeiArray->hole);
			zhuangbeiArray->diamond_type_1 = htonl(zhuangbeiArray->diamond_type_1);
			zhuangbeiArray->diamond_type_2 = htonl(zhuangbeiArray->diamond_type_2);
			zhuangbeiArray->diamond_type_3 = htonl(zhuangbeiArray->diamond_type_3);

			zhuangbeiArray->gongji_min = htonl(zhuangbeiArray->gongji_min);
			zhuangbeiArray->gongji_max = htonl(zhuangbeiArray->gongji_max);
			zhuangbeiArray->hujia = htonl(zhuangbeiArray->hujia);

			// 全是char的，不用转化
			zhuangbeiArray->qianghua_data = zhuangbeiArray->qianghua_data;

			zhuangbeiArray->has_fumo = zhuangbeiArray->has_fumo;
			zhuangbeiArray->fumo_attr.attr_type = htonl(zhuangbeiArray->fumo_attr.attr_type);
			zhuangbeiArray->fumo_attr.attr_value = htonl(zhuangbeiArray->fumo_attr.attr_value);
			zhuangbeiArray->fumo_attr.cuiqu_suc_ratio = htonl(zhuangbeiArray->fumo_attr.cuiqu_suc_ratio);

			zhuangbeiArray->common_attr.attr_count = htonl(zhuangbeiArray->common_attr.attr_count);
			ZBAttr* attr_p = (ZBAttr*)zhuangbeiArray->common_attr.attr_arr;
			for(int attr_index=0; attr_index<zhuangbeiArray->common_attr.attr_count; ++attr_index)
			{
				attr_p[attr_index].attr_type = htonl(attr_p[attr_index].attr_type);
				attr_p[attr_index].attr_value = htonl(attr_p[attr_index].attr_value);
				attr_p[attr_index].cuiqu_suc_ratio = htonl(attr_p[attr_index].cuiqu_suc_ratio);
			}

			attr_p += zhuangbeiArray->common_attr.attr_count;
			char* desc_p = (char*)attr_p;
			string zhuangbei_desc = string(desc_p);
			nextReadPos = (char*)(desc_p + zhuangbei_desc.length() + 1); // 指向jinglian_data

			// 下面的全是变长的，每一个字段都得计算到offset里面
			if (zhuangbeiArray->zhuangbei_colour == ZhuangbeiColour_Chuanqi
                || zhuangbeiArray->zhuangbei_colour == ZhuangbeiColour_Xiyou)
			{
				JinglianData* jinglian = (JinglianData*)nextReadPos; //
				jinglian->jinglian_times = htonl(jinglian->jinglian_times);
				{
					int jinglian_attr_num = sizeof(jinglian->jinglian_attr_arr)/sizeof(ZBAttr); // 应该是2
					for (int jinglian_attr_index = 0;jinglian_attr_index < jinglian_attr_num;jinglian_attr_index++)
					{
						jinglian->jinglian_attr_arr[jinglian_attr_index].attr_type = htonl(jinglian->jinglian_attr_arr[jinglian_attr_index].attr_type);
						jinglian->jinglian_attr_arr[jinglian_attr_index].attr_value = htonl(jinglian->jinglian_attr_arr[jinglian_attr_index].attr_value);
						jinglian->jinglian_attr_arr[jinglian_attr_index].cuiqu_suc_ratio = htonl(jinglian->jinglian_attr_arr[jinglian_attr_index].cuiqu_suc_ratio);
					}
				}
				{
					/*int next_jinglian_attr_num = sizeof(jinglian->jinglian_attr_arr)/sizeof(ZBAttr); // 应该是2
					for (int next_jinglian_attr_index = 0;next_jinglian_attr_index < next_jinglian_attr_num;next_jinglian_attr_index++)
					{
						jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].attr_type = htonl(jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].attr_type);
						jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].attr_value = htonl(jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].attr_value);
						jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].cuiqu_suc_ratio = htonl(jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].cuiqu_suc_ratio);
					}*/
				}


				jinglian++;
				nextReadPos = (char*)jinglian; // 指向purify_hole

                unsigned char purify_hole_count = *(unsigned char*)(nextReadPos);
                nextReadPos++;
               
                ZBAttr* purify_arr_p = (ZBAttr*)(nextReadPos);
                for(int i=0; i<purify_hole_count; ++i) {
                    purify_arr_p[i].attr_type = htonl(purify_arr_p[i].attr_type);
                    purify_arr_p[i].attr_value = htonl(purify_arr_p[i].attr_value);
                    purify_arr_p[i].cuiqu_suc_ratio = htonl(purify_arr_p[i].cuiqu_suc_ratio);
                }

                nextReadPos = (char*)(purify_arr_p + purify_hole_count); // 指向is_unlocked
                
                nextReadPos ++; // 指向is_shenbing 或者下一个

                if (zhuangbeiArray->zhuangbei_colour == ZhuangbeiColour_Chuanqi)
                {
    				// 传奇才判断下面的
    				bool* shenbing = (bool*)nextReadPos;
    				bool isShenBing = *shenbing;

    				// 传奇全是神兵的逻辑
    				if (isChuanqi)
    					isShenBing = true;

    				if (isShenBing)
    				{
    					nextReadPos ++; // 指向identified_arr_count
    					unsigned char identified_arr_count = *((unsigned char*)(nextReadPos));
    					nextReadPos ++; // 指向hide_attr_arr

    					ZBAttr* hide_attr = (ZBAttr*)nextReadPos;
    					for (int hide_index = 0;hide_index < identified_arr_count;hide_index++)
    					{
    						hide_attr[hide_index].attr_type = htonl(hide_attr[hide_index].attr_type);
    						hide_attr[hide_index].attr_value = htonl(hide_attr[hide_index].attr_value);
    						hide_attr[hide_index].cuiqu_suc_ratio = htonl(hide_attr[hide_index].cuiqu_suc_ratio);
    					}
    					nextReadPos = (char*)(hide_attr + identified_arr_count); // 指向has_jingwen


    					bool hasZhuanKe = *((bool*)nextReadPos); // 不管篆刻与否，下面的都带，还是定长
    					if (hasZhuanKe)
    					{
    						int a= 0;
    					}
    					
    					nextReadPos ++; // 指向jingwen_attr_arr
    					ZBAttr* jingwen_attr_arr = (ZBAttr*)(nextReadPos);
    					int jingwen_attr_count = 2;
    					for (int jingwen_index = 0;jingwen_index < jingwen_attr_count;jingwen_index++)
    					{
    						jingwen_attr_arr[jingwen_index].attr_type = htonl(jingwen_attr_arr[jingwen_index].attr_type);
    						jingwen_attr_arr[jingwen_index].attr_value = htonl(jingwen_attr_arr[jingwen_index].attr_value);
    						jingwen_attr_arr[jingwen_index].cuiqu_suc_ratio = htonl(jingwen_attr_arr[jingwen_index].cuiqu_suc_ratio);
    					}
    					nextReadPos = (char*)(jingwen_attr_arr + jingwen_attr_count); // 指向下一个
    				}
                }
			}

		}


		EquipData euip;
		euip.initWithZhungbeiDetail(zhuangbeiArray);
#if (defined(_CLIENT_) && !defined(SHOW_DEBUG_LABLE))
		euip.equipName = _equipInfoConfig->getEquipName(euip.group_id);
		euip.equipPic = _equipInfoConfig->getEquipPicFile(euip.group_id);
		euip.equipDicrib = _equipInfoConfig->getEquipDescribe(euip.group_id);
#endif

		out.push_back(euip);

		zhuangbeiArray = (ZhuangbeiDetail*)nextReadPos;

		//for (int iTemp=-5;iTemp<10;iTemp++)
		//{
		//	ZhuangbeiDetail* test1 = (ZhuangbeiDetail*)(nextReadPos+iTemp);
		//	int a = 0;
		//}
	}

	return (char*)zhuangbeiArray;
}

char* CS::hton_equipdata_2_zhuangbei_detail(const std::vector<EquipData>& equips,ZhuangbeiDetail* detail_arr_out)
{
	const char* head = (char*)detail_arr_out;

	ZhuangbeiDetail* zhuangbeiArray = detail_arr_out;
	int num = equips.size();
	for (int i=0;i<num;i++)
	{
		string des = equips[i].equipDicrib;
		const EquipData* data =&(equips[i]);
		char* nextReadPos = NULL;
		// 一个detail的转换
		{
			bool isChuanqi = data->zhuangbei_colour == ZhuangbeiColour_Chuanqi;
#ifdef _CLIENT_
			zhuangbeiArray->zhuangbei_id = LF::lf_htonll(data->zhuangbei_id);
#else
			zhuangbeiArray->zhuangbei_id = Utl::htonll(data->zhuangbei_id);
#endif
			zhuangbeiArray->group_id = htonl(data->group_id);

			zhuangbeiArray->zhuangbei_type = htonl(data->zhuangbei_type);
			strcpy(zhuangbeiArray->zhuangbei_name, data->equipName.c_str());
			zhuangbeiArray->zhuangbei_colour =htonl(data->zhuangbei_colour);
			strcpy(zhuangbeiArray->zhuangbei_pic, data->equipPic.c_str());

			zhuangbeiArray->zhuangbei_hero_id = htonl(data->zhuangbei_hero_id);
			zhuangbeiArray->profession_id = htonl(data->profession_id);

			zhuangbeiArray->zhuangbei_level = htonl(data->zhuangbei_level);
			zhuangbeiArray->hole = htonl(data->hole);
			zhuangbeiArray->diamond_type_1 = htonl(data->diamond_type_1);
			zhuangbeiArray->diamond_type_2 = htonl(data->diamond_type_2);
			zhuangbeiArray->diamond_type_3 = htonl(data->diamond_type_3);

			zhuangbeiArray->gongji_min = htonl(data->gongji_min);
			zhuangbeiArray->gongji_max = htonl(data->gongji_max);
			zhuangbeiArray->hujia = htonl(data->hujia);

			// 全是char的，不用转化
			zhuangbeiArray->qianghua_data = data->qianghua_data;

			zhuangbeiArray->has_fumo = data->has_fumo;
			zhuangbeiArray->fumo_attr.attr_type = htonl(data->fumo_attr.attr_type);
			zhuangbeiArray->fumo_attr.attr_value = htonl(data->fumo_attr.attr_value);
			zhuangbeiArray->fumo_attr.cuiqu_suc_ratio = htonl(data->fumo_attr.cuiqu_suc_ratio);

			zhuangbeiArray->common_attr.attr_count = htonl(data->attr_vec.size());
			ZBAttr* attr_p = (ZBAttr*)zhuangbeiArray->common_attr.attr_arr;
			for(int attr_index=0; attr_index<data->attr_vec.size(); ++attr_index)
			{
				attr_p[attr_index].attr_type = htonl(data->attr_vec[attr_index].attr_type);
				attr_p[attr_index].attr_value = htonl(data->attr_vec[attr_index].attr_value);
				attr_p[attr_index].cuiqu_suc_ratio = htonl(data->attr_vec[attr_index].cuiqu_suc_ratio);
			}

			attr_p += data->attr_vec.size();
			char* desc_p = (char*)attr_p;
			strcpy(desc_p,data->equipDicrib.c_str());

			nextReadPos = desc_p + data->equipDicrib.length() + 1;

			// 下面的全是变长的，每一个字段都得计算到offset里面
			if (data->zhuangbei_colour == ZhuangbeiColour_Chuanqi 
                || data->zhuangbei_colour == ZhuangbeiColour_Xiyou)
			{
				JinglianData* jinglian = (JinglianData*)nextReadPos;
				jinglian->jinglian_times = htonl(data->jinglian_times);
				{
					int jinglian_attr_num = 2; // 应该是2
					for (int jinglian_attr_index = 0;jinglian_attr_index < jinglian_attr_num;jinglian_attr_index++)
					{
						if (jinglian_attr_index < data->jinglian_attr_vec.size())
						{
							jinglian->jinglian_attr_arr[jinglian_attr_index].attr_type = htonl(data->jinglian_attr_vec[jinglian_attr_index].attr_type);
							jinglian->jinglian_attr_arr[jinglian_attr_index].attr_value = htonl(data->jinglian_attr_vec[jinglian_attr_index].attr_value);
							jinglian->jinglian_attr_arr[jinglian_attr_index].cuiqu_suc_ratio = htonl(data->jinglian_attr_vec[jinglian_attr_index].cuiqu_suc_ratio);
						}
						else
						{
							jinglian->jinglian_attr_arr[jinglian_attr_index].attr_type = htonl(0);
							jinglian->jinglian_attr_arr[jinglian_attr_index].attr_value = htonl(0);
							jinglian->jinglian_attr_arr[jinglian_attr_index].cuiqu_suc_ratio = htonl(0);
						}
					}
				}

				{
					int next_jinglian_attr_num = 2; // 应该是2
					for (int next_jinglian_attr_index = 0;next_jinglian_attr_index < next_jinglian_attr_num;next_jinglian_attr_index++)
					{
						/*if (next_jinglian_attr_index < data->jinglian_attr_will_have_vec.size())
						{
							jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].attr_type = htonl(data->jinglian_attr_will_have_vec[next_jinglian_attr_index].attr_type);
							jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].attr_value = htonl(data->jinglian_attr_will_have_vec[next_jinglian_attr_index].attr_value);
							jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].cuiqu_suc_ratio = htonl(data->jinglian_attr_will_have_vec[next_jinglian_attr_index].cuiqu_suc_ratio);
						}
						else
						{
							jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].attr_type = htonl(0);
							jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].attr_value = htonl(0);
							jinglian->next_jinglian_attr_arr[next_jinglian_attr_index].cuiqu_suc_ratio = htonl(0);
						}*/
					}
				}
				
				jinglian++;
				nextReadPos = (char*)jinglian; // 指向purify_hole

                unsigned char *purify_hole_p = (unsigned char*)(nextReadPos);
                *purify_hole_p = (unsigned char)data->purify_attr_vec.size();
                nextReadPos++;
               
                ZBAttr* purify_arr_p = (ZBAttr*)(nextReadPos);
                for(int i=0; i<data->purify_attr_vec.size(); ++i) {
                    purify_arr_p[i].attr_type = htonl(data->purify_attr_vec[i].attr_type);
                    purify_arr_p[i].attr_value = htonl(data->purify_attr_vec[i].attr_value);
                    purify_arr_p[i].cuiqu_suc_ratio = htonl(data->purify_attr_vec[i].cuiqu_suc_ratio);
                }

                nextReadPos = (char*)(purify_arr_p + data->purify_attr_vec.size()); // 指向is_unlocked
                *(nextReadPos) = !data->isInFengYin;
                
                nextReadPos ++; // 指向is_shenbing 

                if (data->zhuangbei_colour == ZhuangbeiColour_Chuanqi)
                {
    				char* shenbing = (char*)nextReadPos;
    				*shenbing = data->isShenBing;

    				CCAssert(data->isShenBing,"是传奇就应该全是神兵！");

    				if (data->isShenBing)
    				{
    					nextReadPos ++; //指向identified_arr_count

    					int knownHideAttrNum = data->known_hide_attr_vec.size();
    					*(nextReadPos) = (unsigned char)knownHideAttrNum;
    					nextReadPos ++; //指向hide_attr_arr

    					ZBAttr* hide_attr = (ZBAttr*)nextReadPos;
    					for (int hide_index = 0;hide_index < knownHideAttrNum;hide_index++)
    					{
    						hide_attr[hide_index].attr_type = htonl(data->known_hide_attr_vec[hide_index].attr_type);
    						hide_attr[hide_index].attr_value = htonl(data->known_hide_attr_vec[hide_index].attr_value);
    						hide_attr[hide_index].cuiqu_suc_ratio = htonl(data->known_hide_attr_vec[hide_index].cuiqu_suc_ratio);
    					}
    					nextReadPos += knownHideAttrNum * sizeof(ZBAttr); // 指向has_jingwen

    					*nextReadPos = data->zhuanke_attr_vec.size()>0;
    					nextReadPos ++; // 指向jingwen_attr_arr

    					ZBAttr* jingwen_attr_arr = (ZBAttr*)nextReadPos;
    					int jingwen_attr_count = 2;
    					for (int jingwen_index = 0;jingwen_index < jingwen_attr_count;jingwen_index++)
    					{
    						if (jingwen_index < data->zhuanke_attr_vec.size())
    						{
    							jingwen_attr_arr[jingwen_index].attr_type = htonl(data->zhuanke_attr_vec[jingwen_index].attr_type);
    							jingwen_attr_arr[jingwen_index].attr_value = htonl(data->zhuanke_attr_vec[jingwen_index].attr_value);
    							jingwen_attr_arr[jingwen_index].cuiqu_suc_ratio = htonl(data->zhuanke_attr_vec[jingwen_index].cuiqu_suc_ratio);
    						}else
    						{
    							jingwen_attr_arr[jingwen_index].attr_type = htonl(0);
    							jingwen_attr_arr[jingwen_index].attr_value = htonl(0);
    							jingwen_attr_arr[jingwen_index].cuiqu_suc_ratio = htonl(0);
    						}
    					}
    					nextReadPos += jingwen_attr_count * sizeof(ZBAttr); // 指向下一个开头
    				}
                }
			}

		}

		zhuangbeiArray = (ZhuangbeiDetail*)nextReadPos;
	}

	return (char*)zhuangbeiArray;
}



unsigned int CS::ItemsNeedForQianghuaZhuangbei(unsigned int zhuangbei_level, unsigned int zhuangbei_colour)
{
    if(zhuangbei_colour == ZhuangbeiColour_Chuanqi)
        return 10;
    
    if(zhuangbei_level <= 10)
    {
        return 5;
    }
    if(zhuangbei_level <= 14)
    {
        return 6;
    }
    if(zhuangbei_level <= 18)
    {
        return 7;
    }
    if(zhuangbei_level <= 22)
    {
        return 8;
    }
    if(zhuangbei_level <= 26)
    {
        return 9;
    }
    if(zhuangbei_level <= 30)
    {
        return 10;
    }
    return 10;
}


unsigned int CS::ChaijieZhuangbeiNeedSilver(unsigned int zhuangbei_level, unsigned int zhuangbei_colour)
{
	return 0;

    unsigned int silver = zhuangbei_level*100;
    switch(zhuangbei_colour)
    {
        case ZhuangbeiColour_Xiyou:
            silver *= 2;
            break;
        case ZhuangbeiColour_Chuanqi:
            silver *= 4;
    }

    return silver;
}


void CS::getItemsForChaijieZhuangbei(unsigned int zhuangbei_level, unsigned int zhuangbei_colour,
    std::vector<ItemInfo> &item_vec)
{
    item_vec.clear();

    //强化粉
    ItemInfo item;
    item.type = ItemsType_QianghuaNormal;
    item.count = ItemsNeedForQianghuaZhuangbei(zhuangbei_level, zhuangbei_colour);
    item_vec.push_back(item);

    //洗炼精华
    if(zhuangbei_colour == ZhuangbeiColour_Xiyou) {
        item.type = ItemsType_Purify_Scroll;

        if(zhuangbei_level <= 15) {
            item.count = 1;
        }
        else if(zhuangbei_level <= 25) {
            item.count = 2;
        }
        else if(zhuangbei_level <= 30) {
            item.count = 3;
        }
        else {
            item.count = 0;
        }

        if(item.count > 0) {
            item_vec.push_back(item);
        }
    }

    return ;
}


unsigned int CS::QianghuaZhuangbeiNeedSilver(unsigned int zhuangbei_level, unsigned int zhuangbei_colour)
{
    unsigned int silver = zhuangbei_level*40;
    if(zhuangbei_colour == 4){
        silver = HERO_MAX_LEVEL *40;
    }
    return silver;
}


unsigned int CS::HuituiZhuangbeiNeedSilver(
    unsigned int zhuangbei_level,unsigned int zhuangbei_colour)
{
	return 0;

    unsigned int silver = zhuangbei_level*80;
    return silver;
}


unsigned int CS::HuituiZhuangbeiNeedGold(unsigned int retain_count)
{
    if(retain_count == 5){
        return 400;
    }
    else if(retain_count == 6){
        return 800;
    }
    else if(retain_count == 7){
        return 1200;
    }
    else if(retain_count == 8){
        return 1600;
    }

    return 0;
}


unsigned int CS::HechengBaoshiNeedGold(unsigned int baoshi_type)
{
	unsigned int baoshi_level = 0;
    switch(baoshi_type)
    {
        case ItemsType_Baoshi_hong_2:
        case ItemsType_Baoshi_lv_2:
        //case ItemsType_Baoshi_zhi_2:
        case ItemsType_Baoshi_huang_2:
            baoshi_level = 2;
            break;
        case ItemsType_Baoshi_hong_3:
        case ItemsType_Baoshi_lv_3:
        //case ItemsType_Baoshi_zhi_3:
        case ItemsType_Baoshi_huang_3:
            baoshi_level = 3;
            break;
        case ItemsType_Baoshi_hong_4:
        case ItemsType_Baoshi_lv_4:
        //case ItemsType_Baoshi_zhi_4:
        case ItemsType_Baoshi_huang_4:
            baoshi_level = 4;
            break;
        case ItemsType_Baoshi_hong_5:
        case ItemsType_Baoshi_lv_5:
        //case ItemsType_Baoshi_zhi_5:
        case ItemsType_Baoshi_huang_5:
            baoshi_level = 5;
            break;
        case ItemsType_Baoshi_hong_6:
        case ItemsType_Baoshi_lv_6:
        //case ItemsType_Baoshi_zhi_6:
        case ItemsType_Baoshi_huang_6:
            baoshi_level = 6;
            break;
        case ItemsType_Baoshi_hong_7:
        case ItemsType_Baoshi_lv_7:
        //case ItemsType_Baoshi_zhi_7:
        case ItemsType_Baoshi_huang_7:
            baoshi_level = 7;
            break;
/*
        case ItemsType_Baoshi_hong_8:
        case ItemsType_Baoshi_lv_8:
        //case ItemsType_Baoshi_zhi_8:
        case ItemsType_Baoshi_huang_8:
            baoshi_level = 8;
            break;
        case ItemsType_Baoshi_hong_9:
        case ItemsType_Baoshi_lv_9:
        //case ItemsType_Baoshi_zhi_9:
        case ItemsType_Baoshi_huang_9:
            baoshi_level = 9;
            break;
        case ItemsType_Baoshi_hong_10:
        case ItemsType_Baoshi_lv_10:
        //case ItemsType_Baoshi_zhi_10:
        case ItemsType_Baoshi_huang_10:
            baoshi_level = 10;
            break;
*/
        default:
            LogMsg("Error : %s, line %d,  baoshi_type=[%u]"
                    , __FILE__, __LINE__, baoshi_type);
            return 0x1fffffff;
            break;
        
    }

    return baoshi_level*50;
}
unsigned int CS::HechengBaoshiNeedBaoshiType(unsigned int baoshi_type)
{
	switch(baoshi_type)
    {
        case ItemsType_Baoshi_hong_2:
            return ItemsType_Baoshi_hong_1;
        case ItemsType_Baoshi_hong_3:
            return ItemsType_Baoshi_hong_2;
        case ItemsType_Baoshi_hong_4:
            return ItemsType_Baoshi_hong_3;
        case ItemsType_Baoshi_hong_5:
            return ItemsType_Baoshi_hong_4;
        case ItemsType_Baoshi_hong_6:
            return ItemsType_Baoshi_hong_5;
        case ItemsType_Baoshi_hong_7:
            return ItemsType_Baoshi_hong_6;
/*
        case ItemsType_Baoshi_hong_8:
            return ItemsType_Baoshi_hong_7;
        case ItemsType_Baoshi_hong_9:
            return ItemsType_Baoshi_hong_8;
        case ItemsType_Baoshi_hong_10:
            return ItemsType_Baoshi_hong_9;
*/
        case ItemsType_Baoshi_lv_2:
            return ItemsType_Baoshi_lv_1;
        case ItemsType_Baoshi_lv_3:
            return ItemsType_Baoshi_lv_2;
        case ItemsType_Baoshi_lv_4:
            return ItemsType_Baoshi_lv_3;
        case ItemsType_Baoshi_lv_5:
            return ItemsType_Baoshi_lv_4;
        case ItemsType_Baoshi_lv_6:
            return ItemsType_Baoshi_lv_5;
        case ItemsType_Baoshi_lv_7:
            return ItemsType_Baoshi_lv_6;
/*
        case ItemsType_Baoshi_lv_8:
            return ItemsType_Baoshi_lv_7;
        case ItemsType_Baoshi_lv_9:
            return ItemsType_Baoshi_lv_8;
        case ItemsType_Baoshi_lv_10:
            return ItemsType_Baoshi_lv_9;
*/
/*
        case ItemsType_Baoshi_zhi_2:
            return ItemsType_Baoshi_zhi_1;
        case ItemsType_Baoshi_zhi_3:
            return ItemsType_Baoshi_zhi_2;
        case ItemsType_Baoshi_zhi_4:
            return ItemsType_Baoshi_zhi_3;
        case ItemsType_Baoshi_zhi_5:
            return ItemsType_Baoshi_zhi_4;
        case ItemsType_Baoshi_zhi_6:
            return ItemsType_Baoshi_zhi_5;
        case ItemsType_Baoshi_zhi_7:
            return ItemsType_Baoshi_zhi_6;
        case ItemsType_Baoshi_zhi_8:
            return ItemsType_Baoshi_zhi_7;
        case ItemsType_Baoshi_zhi_9:
            return ItemsType_Baoshi_zhi_8;
        case ItemsType_Baoshi_zhi_10:
            return ItemsType_Baoshi_zhi_9;
*/
        case ItemsType_Baoshi_huang_2:
            return ItemsType_Baoshi_huang_1;
        case ItemsType_Baoshi_huang_3:
            return ItemsType_Baoshi_huang_2;
        case ItemsType_Baoshi_huang_4:
            return ItemsType_Baoshi_huang_3;
        case ItemsType_Baoshi_huang_5:
            return ItemsType_Baoshi_huang_4;
        case ItemsType_Baoshi_huang_6:
            return ItemsType_Baoshi_huang_5;
        case ItemsType_Baoshi_huang_7:
            return ItemsType_Baoshi_huang_6;
/*
        case ItemsType_Baoshi_huang_8:
            return ItemsType_Baoshi_huang_7;
        case ItemsType_Baoshi_huang_9:
            return ItemsType_Baoshi_huang_8;
        case ItemsType_Baoshi_huang_10:
            return ItemsType_Baoshi_huang_9;
*/
        default:
            LogMsg("Error:%s, line %d,  baoshi_type=[%u]"
                    , __FILE__, __LINE__, baoshi_type);
            return 0x1fffffff;
        
    }
}
bool CS::canBaoShiBeHeCheng(unsigned int baoshi_type)
{
	bool ret = true;

	switch (baoshi_type)
	{
		// 一级宝石不用合成
	case ItemsType_Baoshi_lv_1:
	case ItemsType_Baoshi_hong_1:
	//case ItemsType_Baoshi_zhi_1:
	case ItemsType_Baoshi_huang_1:
		ret = false;
		break;
	default:
		CCLog("Waring : %s--> baoshi_type = [%d]",__FUNCTION__,baoshi_type);
		break;
	}
	return ret;
}
unsigned int CS::HechengBaoshiNeedBaoshiCount(unsigned int baoshi_type)
{
	return 3;
}
unsigned int CS::XiangbaoshiNeedSilver(unsigned int zb_level, unsigned int baoshi_type)
{

	return 0;

	int need = zb_level*100;

    int baoshi_level = 0;
    switch(baoshi_type)
    {
        case ItemsType_Baoshi_hong_1:
        case ItemsType_Baoshi_lv_1:
        //case ItemsType_Baoshi_zhi_1:
        case ItemsType_Baoshi_huang_1:
            baoshi_level = 1;
            break;
        case ItemsType_Baoshi_hong_2:
        case ItemsType_Baoshi_lv_2:
        //case ItemsType_Baoshi_zhi_2:
        case ItemsType_Baoshi_huang_2:
            baoshi_level = 2;
            break;
        case ItemsType_Baoshi_hong_3:
        case ItemsType_Baoshi_lv_3:
        //case ItemsType_Baoshi_zhi_3:
        case ItemsType_Baoshi_huang_3:
            baoshi_level = 3;
            break;
        case ItemsType_Baoshi_hong_4:
        case ItemsType_Baoshi_lv_4:
        //case ItemsType_Baoshi_zhi_4:
        case ItemsType_Baoshi_huang_4:
            baoshi_level = 4;
            break;
        case ItemsType_Baoshi_hong_5:
        case ItemsType_Baoshi_lv_5:
        //case ItemsType_Baoshi_zhi_5:
        case ItemsType_Baoshi_huang_5:
            baoshi_level = 5;
            break;
        case ItemsType_Baoshi_hong_6:
        case ItemsType_Baoshi_lv_6:
        //case ItemsType_Baoshi_zhi_6:
        case ItemsType_Baoshi_huang_6:
            baoshi_level = 6;
            break;
        case ItemsType_Baoshi_hong_7:
        case ItemsType_Baoshi_lv_7:
        //case ItemsType_Baoshi_zhi_7:
        case ItemsType_Baoshi_huang_7:
            baoshi_level = 7;
            break;
        /*
        case ItemsType_Baoshi_hong_8:
        case ItemsType_Baoshi_lv_8:
        //case ItemsType_Baoshi_zhi_8:
        case ItemsType_Baoshi_huang_8:
            baoshi_level = 8;
            break;
        case ItemsType_Baoshi_hong_9:
        case ItemsType_Baoshi_lv_9:
        //case ItemsType_Baoshi_zhi_9:
        case ItemsType_Baoshi_huang_9:
            baoshi_level = 9;
            break;
        case ItemsType_Baoshi_hong_10:
        case ItemsType_Baoshi_lv_10:
        //case ItemsType_Baoshi_zhi_10:
        case ItemsType_Baoshi_huang_10:
            baoshi_level = 10;
            break;
        */
        default:
            LogMsg("Error:%s, line %d,  baoshi_type=[%u]"
                    , __FILE__, __LINE__, baoshi_type);
            return 0x1fffffff;
            break;
        
    }

    need += baoshi_level * 200;

    return need;
}


bool CS::canZBTypeBeQianghua(unsigned int zb_type)
{
    switch(zb_type)
    {
        case ZhuangbeiType_DanshouJian:
        case ZhuangbeiType_ShuangshouBishou:
        case ZhuangbeiType_ShuangshouFu:
        case ZhuangbeiType_DanshouFazhang:
        case ZhuangbeiType_ShuangshouFazhang:
        case ZhuangbeiType_Gong:
        //case ZhuangbeiType_Dunpai:
        //case ZhuangbeiType_Toukui:
        case ZhuangbeiType_KuijiaZhong:
        case ZhuangbeiType_KuijiaQin:
        //case ZhuangbeiType_KuziZhong:
        //case ZhuangbeiType_KuziQin:
        //case ZhuangbeiType_Xuezi:
            return true;
            
        case ZhuangbeiType_Jiezhi:
        case ZhuangbeiType_Hushenfu:
        case ZhuangbeiType_Faqi:
		case ZhuangbeiType_Dunpai:
            return false;
            
        default:
			CCLog("Waring : %s--> zb_type = [%d]",__FUNCTION__,zb_type);
            return false;
    }

}

#ifdef  _CLIENT_
bool CS::canZBXiangqianBaoshi(unsigned long long eqId)
{
	Equipment* eq = EquipmentManager::getManager()->getOneEquipment(eqId);
	{
		int hole = eq->getEquipData().hole;
		int baoshi_count = 0;
		if(eq->getEquipData().diamond_type_1 > 0)
		{
			baoshi_count++;
		}
		if(eq->getEquipData().diamond_type_2 > 0)
		{
			baoshi_count++;
		}
		if(eq->getEquipData().diamond_type_3 > 0)
		{
			baoshi_count++;
		}

		if(hole > baoshi_count)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
#endif

unsigned int CS::ZhuangkongNeedGold(
	unsigned int zhuangbei_level, unsigned int zhuangbei_colour)
{
	unsigned int gold = zhuangbei_level;
	switch(zhuangbei_colour)
	{
	case ZhuangbeiColour_Xiyou:
		gold *= 2;
		break;
	case ZhuangbeiColour_Chuanqi:
		gold *= 4;
	}

	return gold;
}

bool CS::canZBTypeBeZhuangkong(unsigned int zhuangbei_type,unsigned int hole_count)
{
	int hole_max = 0;
	switch(zhuangbei_type)
	{
	case ZhuangbeiType_DanshouJian:
	case ZhuangbeiType_ShuangshouBishou:
	case ZhuangbeiType_ShuangshouFu:
	case ZhuangbeiType_DanshouFazhang:
	case ZhuangbeiType_ShuangshouFazhang:
	case ZhuangbeiType_Gong:
	case ZhuangbeiType_Faqi:
	case ZhuangbeiType_Dunpai:
	//case ZhuangbeiType_Toukui:
	case ZhuangbeiType_Hushenfu:
	//case ZhuangbeiType_Xuezi:
	case ZhuangbeiType_Jiezhi:
		hole_max = 1;
		break;
	case ZhuangbeiType_KuijiaZhong:
	case ZhuangbeiType_KuijiaQin:
		hole_max = 3;
		break;
	//case ZhuangbeiType_KuziZhong:
	//case ZhuangbeiType_KuziQin:
		hole_max = 2;
		break;
	default:
		LogMsg("Error:%s, line %d, zhuangbei_type[%u] what is the hell"
			, __FILE__, __LINE__, zhuangbei_type);
		return false;
	}

	if(hole_count == hole_max)
	{
		return false;
	}
	if(hole_count > hole_max)
	{
		LogMsg("%s, line %d, zhuangbei_type[%u] what is the hell"
			, __FILE__, __LINE__, zhuangbei_type);
		return false;
	}

	return true;
}

bool CS::canEquipToHero(unsigned int zhuangbei_type,unsigned int profession_id)
{
	//
	bool match = false;
	switch(zhuangbei_type)
	{
	case ZhuangbeiType_DanshouJian:
		if((profession_id == Profession_Yemanren)
			|| (profession_id == Profession_Chike)
			|| profession_id == Profession_WuSeng
			|| profession_id == Profession_YouXia
			|| (profession_id == Profession_Qishi))
		{
			match = true;
		}
		break;
	case ZhuangbeiType_ShuangshouBishou:
		if(profession_id == Profession_Chike)
		{
			match = true;
		}
		break;
	case ZhuangbeiType_ShuangshouFu:
		if(profession_id == Profession_Yemanren)
		{
			match = true;
		}
		break;
	case ZhuangbeiType_ShuangshouFazhang:
		if(profession_id == Profession_Fashi
			|| profession_id == Profession_MiShu
			|| profession_id == Profession_WuShi)
		{
			match = true;
		}
		break;
	case ZhuangbeiType_DanshouFazhang:
		if(profession_id == Profession_Fashi)
		{
			match = true;
		}
		break;
	case ZhuangbeiType_Gong:
		if(profession_id == Profession_ShenJianShou 
			|| profession_id == Profession_LieShou)
		{
			match = true;
		}
		break;
	case ZhuangbeiType_Faqi:
		if(profession_id == Profession_Fashi)
		{
			match = true;
		}
		break;
	case ZhuangbeiType_Dunpai:
		if((profession_id == Profession_Yemanren)
			|| (profession_id == Profession_Chike)
			|| profession_id == Profession_WuSeng
			|| profession_id == Profession_YouXia
			|| (profession_id == Profession_Qishi))
		{
			match = true;
		}
		break;
	case ZhuangbeiType_Hushenfu:
		match = true;
		break;
	case ZhuangbeiType_KuijiaZhong:
		if((profession_id == Profession_Yemanren)
			|| (profession_id == Profession_Chike)
			|| profession_id == Profession_WuSeng
			|| profession_id == Profession_YouXia
			|| (profession_id == Profession_Qishi))
		{
			match = true;
		}
		break;
	case ZhuangbeiType_KuijiaQin:
		if(profession_id == Profession_Fashi
            || profession_id == Profession_MiShu
            || profession_id == Profession_WuShi
			|| profession_id == Profession_ShenJianShou
			|| profession_id == Profession_LieShou)
		{
			match = true;
		}
		break;
	case ZhuangbeiType_Jiezhi:
		match = true;
		break;
	default:
		LogMsg("Error : %s, line %d, zhuangbei_type[%u] what is the hell"
			, __FILE__, __LINE__, zhuangbei_type);
		return false;
		break;
	}
	if(!match)
	{
		return false;
	}


	return true;
}

//unsigned int CS::getQianghuaGailvWithTimes(unsigned int times)
//{
//    //与服务器数据库中的数据保持一致
//    static unsigned int gailv_arr[] = {100, 95, 90, 80, 70, 50, 30, 15, 5, 1};
//
//    if(times < sizeof(gailv_arr)/sizeof(unsigned int))
//    {
//        return gailv_arr[times];
//    }
//    else
//    {
//        return gailv_arr[sizeof(gailv_arr)/sizeof(unsigned int)-1];
//    }
//}

void CS::getBaoShiJiaCheng(unsigned int baoShiType,unsigned int& liLiang,unsigned int& minJie,unsigned int& zhiLi,unsigned int& tiNeng)
{
	liLiang = 0;
	minJie = 0;
	zhiLi = 0;
	tiNeng = 0;

	switch (baoShiType)
	{
	case ItemsType_Baoshi_hong_1:
	case ItemsType_Baoshi_hong_2:
	case ItemsType_Baoshi_hong_3:
	case ItemsType_Baoshi_hong_4:
	case ItemsType_Baoshi_hong_5:
	case ItemsType_Baoshi_hong_6:
	case ItemsType_Baoshi_hong_7:
	//case ItemsType_Baoshi_hong_8:
	//case ItemsType_Baoshi_hong_9:
	//case ItemsType_Baoshi_hong_10:
		liLiang = 10*(baoShiType - ItemsType_Baoshi_hong_1 + 1);
		break;
	case ItemsType_Baoshi_lv_1:
	case ItemsType_Baoshi_lv_2:
	case ItemsType_Baoshi_lv_3:
	case ItemsType_Baoshi_lv_4:
	case ItemsType_Baoshi_lv_5:
	case ItemsType_Baoshi_lv_6:
	case ItemsType_Baoshi_lv_7:
	//case ItemsType_Baoshi_lv_8:
	//case ItemsType_Baoshi_lv_9:
	//case ItemsType_Baoshi_lv_10:
		minJie = 10*(baoShiType - ItemsType_Baoshi_lv_1 + 1);
		break;
	case ItemsType_Baoshi_huang_1:
	case ItemsType_Baoshi_huang_2:
	case ItemsType_Baoshi_huang_3:
	case ItemsType_Baoshi_huang_4:
	case ItemsType_Baoshi_huang_5:
	case ItemsType_Baoshi_huang_6:
	case ItemsType_Baoshi_huang_7:
	//case ItemsType_Baoshi_huang_8:
	//case ItemsType_Baoshi_huang_9:
	//case ItemsType_Baoshi_huang_10:
		zhiLi = 10*(baoShiType - ItemsType_Baoshi_huang_1 + 1);
		break;
/*
	case ItemsType_Baoshi_zhi_1:
	case ItemsType_Baoshi_zhi_2:
	case ItemsType_Baoshi_zhi_3:
	case ItemsType_Baoshi_zhi_4:
	case ItemsType_Baoshi_zhi_5:
	case ItemsType_Baoshi_zhi_6:
	case ItemsType_Baoshi_zhi_7:
	//case ItemsType_Baoshi_zhi_8:
	//case ItemsType_Baoshi_zhi_9:
	//case ItemsType_Baoshi_zhi_10:
		tiNeng = 10*(baoShiType - ItemsType_Baoshi_zhi_1 + 1);
		break;
*/
	default:
		LogMsg("Error:%s, line %d, baoShiType[%u] what is the hell"
			, __FILE__, __LINE__, baoShiType);
		break;
	}
}

int CS::getSkillZhouQi(unsigned int skill_type)
{
	int zhouqi = 0;
	switch (skill_type)
	{
		// 天赋技能
	case SkillType_BaoLieZhan:
		zhouqi = 3;
		break;
	case SkillType_ShaLu:
		zhouqi = 3;
		break;
	case SkillType_ShanDian:
		zhouqi = 3;
		break;
	case SkillType_JianShou:
		zhouqi = 3;
		break;
	case SkillType_ShenZhiXingNian:
		zhouqi = 3;
		break;
	case SkillType_LiRenHuiXuan:
		zhouqi = 3;
		break;
	case SkillType_LieGong:
		zhouqi = 3;
		break;
	case SkillType_JianYu:
		zhouqi = 3;
		break;
	case SkillType_HuoQiuShu:
		zhouqi = 3;
		break;
	case SkillType_WenYi:
		zhouqi = 3;
		break;

		// 技能书1
	case SkillType_BanYueZhan:
		zhouqi = 4;
		break;
	case SkillType_CiSha:
		zhouqi = 4;
		break;
	case SkillType_LeiBao:
		zhouqi = 4;
		break;
	case SkillType_MangCi:
		zhouqi = 4;
		break;
	case SkillType_ShenZhiZhuFu:
		zhouqi = 4;
		break;
	case SkillType_ShunYinTuXi: // 跟利刃回旋对调了
		zhouqi = 4;
		break;
	case SkillType_JingZhunZhiCan:
		zhouqi = 4;
		break;
	case SkillType_JiSuDuJian:
		zhouqi = 4;
		break;
	case SkillType_BaoFengXue:
		zhouqi = 4;
		break;
	case SkillType_KongJu:
		zhouqi = 4;
		break;

		// 技能书2
	case SkillType_KuangBao:
		zhouqi = 5;
		break;
	case SkillType_ShiXueZhiRen:
		zhouqi = 5;
		break;
	case SkillType_YunShi:
		zhouqi = 5;
		break;
	case SkillType_ShouHu:
		zhouqi = 5;
		break;
	case SkillType_ZhongShenZhiLi:
		zhouqi = 5;
		break;
	case SkillType_XuanFengZhan:
		zhouqi = 5;
		break;
	case SkillType_ZhuiHunDuJian:
		zhouqi = 5;
		break;
	case SkillType_SiWangSaoShe:
		zhouqi = 5;
		break;
	case SkillType_HanBingCiGu:
		zhouqi = 5;
		break;
	case SkillType_ShiHun:
		zhouqi = 5;
		break;

		// 技能书3
	case SkillType_BaoNu:
		zhouqi = 6;
		break;
	case SkillType_YinZheHun:
		zhouqi = 6;
		break;
	case SkillType_TianTangZhiGuang:
		zhouqi = 6;
		break;

		// 怪物的
	case SkillType_Monster_XieE:
		zhouqi = 2;
		break;
        
		// BOSS
	case SkillType_BOSS_Sweep:
		zhouqi = 3;
		break;
	case SkillType_BOSS_Flog:
		zhouqi = 3;
		break;
	default:
		CCLog("Error : %s--> skill_type = [%d]",__FUNCTION__,skill_type);
		CCAssert(false,"没有实现");
		zhouqi = 8;
		break;
	}

	return zhouqi;
}
int CS::getSkillTurn(unsigned int skill_type)
{ // 从1开始，到周期数之前
	int turn_pos = 0;
	switch (skill_type)
	{
		// 天赋技能
	case SkillType_BaoLieZhan:
		turn_pos = 2;
		break;
	case SkillType_ShaLu:
		turn_pos = 2;
		break;
	case SkillType_ShanDian:
		turn_pos = 2;
		break;
	case SkillType_JianShou:
		turn_pos = 2;
		break;
	case SkillType_ShenZhiXingNian:
		turn_pos = 2;
		break;
	case SkillType_LiRenHuiXuan: // 跟瞬影突袭对调了
		turn_pos = 2;
		break;
	case SkillType_LieGong:
		turn_pos = 2;
		break;
	case SkillType_JianYu:
		turn_pos = 2;
		break;
	case SkillType_HuoQiuShu:
		turn_pos = 2;
		break;
	case SkillType_WenYi:
		turn_pos = 2;
		break;

		// 技能书1
	case SkillType_BanYueZhan:
		turn_pos = 3;
		break;
	case SkillType_CiSha:
		turn_pos = 3;
		break;
	case SkillType_LeiBao:
		turn_pos = 3;
		break;
	case SkillType_MangCi:
		turn_pos = 3;
		break;
	case SkillType_ShenZhiZhuFu:
		turn_pos = 3;
		break;
	case SkillType_ShunYinTuXi:
		turn_pos = 3;
		break;
	case SkillType_JingZhunZhiCan:
		turn_pos = 3;
		break;
	case SkillType_JiSuDuJian:
		turn_pos = 3;
		break;
	case SkillType_BaoFengXue:
		turn_pos = 3;
		break;
	case SkillType_KongJu:
		turn_pos = 3;
		break;

		// 技能书2
	case SkillType_KuangBao:
		turn_pos = 5;
		break;
	case SkillType_ShiXueZhiRen:
		turn_pos = 5;
		break;
	case SkillType_YunShi:
		turn_pos = 5;
		break;
	case SkillType_ShouHu:
		turn_pos = 5;
		break;
	case SkillType_ZhongShenZhiLi:
		turn_pos = 5;
		break;
	case SkillType_XuanFengZhan:
		turn_pos = 5;
		break;
	case SkillType_ZhuiHunDuJian:
		turn_pos = 5;
		break;
	case SkillType_SiWangSaoShe:
		turn_pos = 5;
		break;
	case SkillType_HanBingCiGu:
		turn_pos = 5;
		break;
	case SkillType_ShiHun:
		turn_pos = 5;
		break;

		// 技能书3
	case SkillType_BaoNu:
		turn_pos = 6;
		break;
	case SkillType_YinZheHun:
		turn_pos = 6;
		break;
	case SkillType_TianTangZhiGuang:
		turn_pos = 6;
		break;

		// 怪物的
	case SkillType_Monster_XieE:
		turn_pos = 1;
		break;
		// BOSS
	case SkillType_BOSS_Sweep:
		turn_pos = 3;
		break;
	case SkillType_BOSS_Flog:
		turn_pos = 3;
		break;
	default:
		CCLog("Error : %s--> skill_type = [%d]",__FUNCTION__,skill_type);
		CCAssert(false,"没有实现");
		turn_pos = 8;
		break;
	}

	return turn_pos;
}

//装备类型能萃取哪些属性
bool CS::isAttrTypeZBTypeCanCuiqu(unsigned int zb_type, unsigned int attr_type)
{
    std::vector<AttrType> attr_type_vec;
    
    switch((ZhuangbeiType)zb_type)
    {
        case ZhuangbeiType_DanshouJian:
        case ZhuangbeiType_ShuangshouBishou:
        case ZhuangbeiType_ShuangshouFu:
        case ZhuangbeiType_DanshouFazhang:
        case ZhuangbeiType_ShuangshouFazhang:
        case ZhuangbeiType_Gong:
            attr_type_vec.push_back(AttrType_gongji_min);
            attr_type_vec.push_back(AttrType_gongji_max);
            attr_type_vec.push_back(AttrType_baoji);
            attr_type_vec.push_back(AttrType_baoshang);
//            attr_type_vec.push_back(AttrType_skillratio);
            break;
        case ZhuangbeiType_Faqi:
            attr_type_vec.push_back(AttrType_baoji);
            attr_type_vec.push_back(AttrType_baoshang);
            attr_type_vec.push_back(AttrType_zhili);
            break;
        case ZhuangbeiType_Dunpai:
            attr_type_vec.push_back(AttrType_gedanglv);
            attr_type_vec.push_back(AttrType_gedangzhi);
            break;
        case ZhuangbeiType_Hushenfu:
            attr_type_vec.push_back(AttrType_gongji_min);
            attr_type_vec.push_back(AttrType_gongji_max);
            attr_type_vec.push_back(AttrType_liliang);
            attr_type_vec.push_back(AttrType_mingjie);
            attr_type_vec.push_back(AttrType_zhili);
            attr_type_vec.push_back(AttrType_baoji);
            attr_type_vec.push_back(AttrType_baoshang);
//            attr_type_vec.push_back(AttrType_skillratio);
            break;
        case ZhuangbeiType_Jiezhi:
            attr_type_vec.push_back(AttrType_gongji_min);
            attr_type_vec.push_back(AttrType_gongji_max);
            attr_type_vec.push_back(AttrType_liliang);
            attr_type_vec.push_back(AttrType_mingjie);
            attr_type_vec.push_back(AttrType_zhili);
            attr_type_vec.push_back(AttrType_tineng);
            attr_type_vec.push_back(AttrType_baoji);
            attr_type_vec.push_back(AttrType_baoshang);
//            attr_type_vec.push_back(AttrType_skillratio);
            break;
        case ZhuangbeiType_KuijiaZhong:
        case ZhuangbeiType_KuijiaQin:
            attr_type_vec.push_back(AttrType_tineng);
            break;
        default:
            LogMsg("Error : %s, line %d, zhuangbei_type[%u] what is the hell"
                        , __FILE__, __LINE__, zb_type);
            return false;
    }

    for(int i=0; i<attr_type_vec.size(); ++i)
    {
        if(attr_type == (unsigned int)attr_type_vec[i])
        {
            return true;
        }
    }

    return false;
}


//每个类型魔法精华的属性数值范围
void CS::getMofaJinghuaAttrValue(int zb_type, int attr_type, unsigned int mfjh_level
    , unsigned int& value_min, unsigned int& value_max)
{
    value_min = 0;
    value_max = 0;
    
    if(zb_type == ZhuangbeiType_DanshouJian)
    {
        if(attr_type == AttrType_gongji_min
            || attr_type == AttrType_gongji_max)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 1;
                    value_max = 7;
                    break;
                case 2:
                    value_min = 2;
                    value_max = 10;
                    break;
                case 3:
                    value_min = 2;
                    value_max = 16;
                    break;
                default:
                    value_min = 5;
                    value_max = 26;
                    break;
            }
            
            return;
        }
        if(attr_type == AttrType_baoji)
        {
            value_min = 2;
            value_max = 15;

            return;
        }
        if(attr_type == AttrType_baoshang)
        {
            value_min = 15;
            value_max = 70;

            return;
        }
//         if(attr_type == AttrType_skillratio)
//         {
//             value_min = 1;
//             value_max = 8;
// 
//             return;
//         }
    }

    if(zb_type == ZhuangbeiType_ShuangshouFu 
        || zb_type == ZhuangbeiType_ShuangshouBishou
        || zb_type == ZhuangbeiType_DanshouFazhang
        || zb_type == ZhuangbeiType_Gong)
    {
        if(attr_type == AttrType_gongji_min
            || attr_type == AttrType_gongji_max)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 1;
                    value_max = 9;
                    break;
                case 2:
                    value_min = 2;
                    value_max = 13;
                    break;
                case 3:
                    value_min = 3;
                    value_max = 20;
                    break;
                default:
                    value_min = 7;
                    value_max = 33;
                    break;
            }

            return;
        }
        if(attr_type == AttrType_baoji)
        {
            value_min = 2;
            value_max = 15;

            return;
        }
        if(attr_type == AttrType_baoshang)
        {
            value_min = 15;
            value_max = 70;

            return;
        }
//         if(attr_type == AttrType_skillratio)
//         {
//             value_min = 1;
//             value_max = 8;
// 
//             return;
//         }
    }

    if(zb_type == ZhuangbeiType_ShuangshouFazhang)
    {
        if(attr_type == AttrType_gongji_min
            || attr_type == AttrType_gongji_max)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 2;
                    value_max = 10;
                    break;
                case 2:
                    value_min = 2;
                    value_max = 16;
                    break;
                case 3:
                    value_min = 3;
                    value_max = 25;
                    break;
                default:
                    value_min = 8;
                    value_max = 37;
                    break;
            }

            return;
        }
        if(attr_type == AttrType_baoji)
        {
            value_min = 2;
            value_max = 15;

            return;
        }
        if(attr_type == AttrType_baoshang)
        {
            value_min = 15;
            value_max = 70;

            return;
        }
//         if(attr_type == AttrType_skillratio)
//         {
//             value_min = 1;
//             value_max = 8;
// 
//             return;
//         }
    }

    if(zb_type == ZhuangbeiType_KuijiaZhong
        || zb_type == ZhuangbeiType_KuijiaQin)
    {
        if(attr_type == AttrType_tineng)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 5;
                    value_max = 36;
                    break;
                case 2:
                    value_min = 7;
                    value_max = 44;
                    break;
                case 3:
                    value_min = 10;
                    value_max = 65;
                    break;
                default:
                    value_min = 14;
                    value_max = 90;
                    break;
            }

            return;
        }
        
    }

    if(zb_type == ZhuangbeiType_Dunpai)
    {
        if(attr_type == AttrType_gedangzhi)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 3;
                    value_max = 15;
                    break;
                case 2:
                    value_min = 5;
                    value_max = 30;
                    break;
                case 3:
                    value_min = 10;
                    value_max = 60;
                    break;
                default:
                    value_min = 20;
                    value_max = 110;
                    break;
            }

            return;
        }
        if(attr_type == AttrType_gedanglv)
        {
            value_min = 3;
            value_max = 10;

            return;
        }
    }

    if(zb_type == ZhuangbeiType_Faqi)
    {
        if(attr_type == AttrType_baoji)
        {
            value_min = 1;
            value_max = 7;

            return;
        }
        if(attr_type == AttrType_baoshang)
        {
            value_min = 7;
            value_max = 35;

            return;
        }
        if(attr_type == AttrType_zhili)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 2;
                    value_max = 12;
                    break;
                case 2:
                    value_min = 3;
                    value_max = 23;
                    break;
                case 3:
                    value_min = 4;
                    value_max = 30;
                    break;
                default:
                    value_min = 6;
                    value_max = 50;
                    break;
            }

            return;
        }
    }

    if(zb_type == ZhuangbeiType_Hushenfu)
    {
        if(attr_type == AttrType_gongji_min
            || attr_type == AttrType_gongji_max)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 1;
                    value_max = 7;
                    break;
                case 2:
                    value_min = 2;
                    value_max = 10;
                    break;
                case 3:
                    value_min = 2;
                    value_max = 16;
                    break;
                default:
                    value_min = 5;
                    value_max = 26;
                    break;
            }

            return;
        }
        if(attr_type == AttrType_zhili
            || attr_type == AttrType_liliang
            || attr_type == AttrType_mingjie)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 4;
                    value_max = 25;
                    break;
                case 2:
                    value_min = 6;
                    value_max = 45;
                    break;
                case 3:
                    value_min = 8;
                    value_max = 60;
                    break;
                default:
                    value_min = 12;
                    value_max = 100;
                    break;
            }

            return;
        }
        if(attr_type == AttrType_baoji)
        {
            value_min = 2;
            value_max = 15;

            return;
        }
        if(attr_type == AttrType_baoshang)
        {
            value_min = 15;
            value_max = 70;

            return;
        }
//         if(attr_type == AttrType_skillratio)
//         {
//             value_min = 1;
//             value_max = 8;
// 
//             return;
//         }
    }

    if(zb_type == ZhuangbeiType_Jiezhi)
    {
        if(attr_type == AttrType_gongji_min
            || attr_type == AttrType_gongji_max)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 1;
                    value_max = 3;
                    break;
                case 2:
                    value_min = 1;
                    value_max = 5;
                    break;
                case 3:
                    value_min = 2;
                    value_max = 8;
                    break;
                default:
                    value_min = 3;
                    value_max = 13;
                    break;
            }

            return;
        }
        if(attr_type == AttrType_zhili
            || attr_type == AttrType_liliang
            || attr_type == AttrType_mingjie)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 2;
                    value_max = 12;
                    break;
                case 2:
                    value_min = 3;
                    value_max = 23;
                    break;
                case 3:
                    value_min = 4;
                    value_max = 30;
                    break;
                default:
                    value_min = 6;
                    value_max = 50;
                    break;
            }

            return;
        }
        if(attr_type == AttrType_tineng)
        {
            switch(mfjh_level)
            {
                case 1:
                    value_min = 5;
                    value_max = 36;
                    break;
                case 2:
                    value_min = 7;
                    value_max = 44;
                    break;
                case 3:
                    value_min = 10;
                    value_max = 65;
                    break;
                default:
                    value_min = 14;
                    value_max = 90;
                    break;
            }

            return;
        }
        if(attr_type == AttrType_baoji)
        {
            value_min = 1;
            value_max = 8;

            return;
        }
        if(attr_type == AttrType_baoshang)
        {
            value_min = 7;
            value_max = 35;

            return;
        }
//         if(attr_type == AttrType_skillratio)
//         {
//             value_min = 1;
//             value_max = 4;
// 
//             return;
//         }
    }
}



#ifdef _CLIENT_
std::string CS::getBaoshiName(unsigned int baoshiType)
{
	std::string level;
	std::string type;
	switch (baoshiType)
	{
	case ItemsType_Baoshi_hong_1:
		type = LFStrings::getValue("hongbaoshi");
		level = "1";
		break;
	case ItemsType_Baoshi_hong_2:
		type = LFStrings::getValue("hongbaoshi");
		level = "2";
		break;
	case ItemsType_Baoshi_hong_3:
		type = LFStrings::getValue("hongbaoshi");
		level = "3";
		break;
	case ItemsType_Baoshi_hong_4:
		type = LFStrings::getValue("hongbaoshi");
		level = "4";
		break;
	case ItemsType_Baoshi_hong_5:
		type = LFStrings::getValue("hongbaoshi");
		level = "5";
		break;
	case ItemsType_Baoshi_hong_6:
		type = LFStrings::getValue("hongbaoshi");
		level = "6";
		break;
	case ItemsType_Baoshi_hong_7:
		type = LFStrings::getValue("hongbaoshi");
		level = "7";
		break;
/*
	case ItemsType_Baoshi_hong_8:
		type = LFStrings::getValue("hongbaoshi");
		level = "8";
		break;
	case ItemsType_Baoshi_hong_9:
		type = LFStrings::getValue("hongbaoshi");
		level = "9";
		break;
	case ItemsType_Baoshi_hong_10:
		type = LFStrings::getValue("hongbaoshi");
		level = "10";
		break;
*/

	case ItemsType_Baoshi_lv_1:
		type = LFStrings::getValue("lvbaoshi");
		level = "1";
		break;
	case ItemsType_Baoshi_lv_2:
		type = LFStrings::getValue("lvbaoshi");
		level = "2";
		break;
	case ItemsType_Baoshi_lv_3:
		type = LFStrings::getValue("lvbaoshi");
		level = "3";
		break;
	case ItemsType_Baoshi_lv_4:
		type = LFStrings::getValue("lvbaoshi");
		level = "4";
		break;
	case ItemsType_Baoshi_lv_5:
		type = LFStrings::getValue("lvbaoshi");
		level = "5";
		break;
	case ItemsType_Baoshi_lv_6:
		type = LFStrings::getValue("lvbaoshi");
		level = "6";
		break;
	case ItemsType_Baoshi_lv_7:
		type = LFStrings::getValue("lvbaoshi");
		level = "7";
		break;
/*
	case ItemsType_Baoshi_lv_8:
		type = LFStrings::getValue("lvbaoshi");
		level = "8";
		break;
	case ItemsType_Baoshi_lv_9:
		type = LFStrings::getValue("lvbaoshi");
		level = "9";
		break;
	case ItemsType_Baoshi_lv_10:
		type = LFStrings::getValue("lvbaoshi");
		level = "10";
		break;
*/

	case ItemsType_Baoshi_huang_1:
		type = LFStrings::getValue("huangbaoshi");
		level = "1";
		break;
	case ItemsType_Baoshi_huang_2:
		type = LFStrings::getValue("huangbaoshi");
		level = "2";
		break;
	case ItemsType_Baoshi_huang_3:
		type = LFStrings::getValue("huangbaoshi");
		level = "3";
		break;
	case ItemsType_Baoshi_huang_4:
		type = LFStrings::getValue("huangbaoshi");
		level = "4";
		break;
	case ItemsType_Baoshi_huang_5:
		type = LFStrings::getValue("huangbaoshi");
		level = "5";
		break;
	case ItemsType_Baoshi_huang_6:
		type = LFStrings::getValue("huangbaoshi");
		level = "6";
		break;
	case ItemsType_Baoshi_huang_7:
		type = LFStrings::getValue("huangbaoshi");
		level = "7";
		break;
/*
	case ItemsType_Baoshi_huang_8:
		type = LFStrings::getValue("huangbaoshi");
		level = "8";
		break;
	case ItemsType_Baoshi_huang_9:
		type = LFStrings::getValue("huangbaoshi");
		level = "9";
		break;
	case ItemsType_Baoshi_huang_10:
		type = LFStrings::getValue("huangbaoshi");
		level = "10";
		break;
*/
/*
	case ItemsType_Baoshi_zhi_1:
		type = LFStrings::getValue("zibaoshi");
		level = "1";
		break;
	case ItemsType_Baoshi_zhi_2:
		type = LFStrings::getValue("zibaoshi");
		level = "2";
		break;
	case ItemsType_Baoshi_zhi_3:
		type = LFStrings::getValue("zibaoshi");
		level = "3";
		break;
	case ItemsType_Baoshi_zhi_4:
		type = LFStrings::getValue("zibaoshi");
		level = "4";
		break;
	case ItemsType_Baoshi_zhi_5:
		type = LFStrings::getValue("zibaoshi");
		level = "5";
		break;
	case ItemsType_Baoshi_zhi_6:
		type = LFStrings::getValue("zibaoshi");
		level = "6";
		break;
	case ItemsType_Baoshi_zhi_7:
		type = LFStrings::getValue("zibaoshi");
		level = "7";
		break;
	case ItemsType_Baoshi_zhi_8:
		type = LFStrings::getValue("zibaoshi");
		level = "8";
		break;
	case ItemsType_Baoshi_zhi_9:
		type = LFStrings::getValue("zibaoshi");
		level = "9";
		break;
	case ItemsType_Baoshi_zhi_10:
		type = LFStrings::getValue("zibaoshi");
		level = "10";
		break;
*/
	}

	return level + LFStrings::getValue("ji") + type;
}

std::string CS::getBaoshiShuxing(unsigned int baoshi_type)
{
	unsigned int minjie = 0;
	unsigned int zhili = 0;
	unsigned int liliang = 0;
	unsigned int tineng = 0;
	CS::getBaoShiJiaCheng(baoshi_type, liliang, minjie, zhili, tineng);
	char str[64];
	if(minjie > 0)
	{
		sprintf(str, "+%u %s", minjie, LFStrings::getValue("minjie").c_str());
	}
	else if(liliang > 0)
	{
		sprintf(str, "+%u %s", liliang, LFStrings::getValue("liliang").c_str());
	}
	else if(zhili > 0)
	{
		sprintf(str, "+%u %s", zhili, LFStrings::getValue("zhili").c_str());
	}
	else
	{
		sprintf(str, "+%u %s", tineng, LFStrings::getValue("tineng").c_str());
	}

	return str;
}





std::string CS::getItemName(unsigned int item_type)
{
	if(item_type > ItemsType_Baoshi_min && item_type < ItemsType_Baoshi_max)
	{
		return getBaoshiName(item_type);
	}

	if (item_type > ItemsType_SkillBook_MinId && item_type < ItemsType_SkillBook_MaxId)
	{
		SkillType skillType = (SkillType)(item_type - ItemsType_SkillBook_MinId);
		// 技能书：雷暴
		return LFStrings::getValue("jinengshu")+string(":")+SkillTree::sharedSkillTree()->getSkillNameByType(skillType);
	}
	
	char tempDefaultRet[64];
	switch(item_type)
	{
	case ItemsType_Gold:
		return LFStrings::getValue("jinbi");
	case ItemsType_Silver:
		return LFStrings::getValue("yinbi");
	case ItemsType_QianghuaNormal:
		return LFStrings::getValue("zhutiefen");
	case ItemsType_QianghuaXiyou:
		return LFStrings::getValue("duanjinfen");
	case ItemsType_QianghuaChuangqi:
		return LFStrings::getValue("emogu");
	case ItemsType_QianghuaGailvTianjia:
		return LFStrings::getValue("duanzaoCailiaoGl");
	case ItemsType_HellStone:
		return LFStrings::getValue("diyurongyan");
	case ItemsType_GoldStar_Qianghua:
		return LFStrings::getValue("jinXingQiangHuaCaiLiao");
	case ItemsType_Shenbing_Jiefengyin:
		return LFStrings::getValue("shenBingJieFengCaiLiao");
	case ItemsType_Shenbing_Jianding:
		return LFStrings::getValue("jianDingCaiLiao");
	case ItemsType_Dalishi:
		return LFStrings::getValue("caiLiao_daLiShi");
	case ItemsType_Huagangyan:
		return LFStrings::getValue("caiLiao_huaGangYan");
	case ItemsType_Mucai:
		return LFStrings::getValue("caiLiao_muCai");
	case ItemsType_Shuijingshi:
		return LFStrings::getValue("caiLiao_shuiJingShi");
	case ItemsType_JingwenZhuanke:
		return LFStrings::getValue("zhuanKeJingWenCaiLiao");
	case ItemsType_Hero_LevelUp:
		return LFStrings::getValue("shengJiCaiLiao");
	case ItemsType_Hero_Upgrade:
		return LFStrings::getValue("cailiao_ShengShui");
	case ItemsType_DaiGouJuan:
		return LFStrings::getValue("DaoJu_DaiGouJuan");
	case ItemsType_PresentVitality:
		return LFStrings::getValue("DaoJu_SongTiLi");
	case ItemsType_PresentVIP1:
		return LFStrings::getValue("DaoJu_SongVip");
	case ItemsType_PresentEquip_Xiyou:
		return LFStrings::getValue("DaoJu_SongXiYou");
	case ItemsType_PresentEquip_Legend:
		return LFStrings::getValue("DaoJu_SongChuanQi");
	case ItemsType_Purify_Scroll:
		return LFStrings::getValue("DaoJu_XiYouJuanZhou");
	case ItemsType_Hammer:
		return LFStrings::getValue("DaoJu_GongJiangTieChui");
	default:
		CCLog("Waring : %s--> item_type = [%d]",__FUNCTION__,item_type);
		sprintf(tempDefaultRet,"%s-%d",LFStrings::getValue("daoju").c_str(),item_type);
		return tempDefaultRet;
	}

}

std::string CS::getItemShuxing(unsigned int item_type)
{
	if(item_type > ItemsType_Baoshi_min && item_type < ItemsType_Baoshi_max)
	{
		return getBaoshiShuxing(item_type);
	}

	switch(item_type)
	{
	case ItemsType_Gold:
		return LFStrings::getValue("jinbishuxing");
	case ItemsType_Silver:
		return LFStrings::getValue("yinbishuxing");
	case ItemsType_QianghuaNormal:
		return LFStrings::getValue("duanzaoCailiaoPt_shuxing");
	case ItemsType_QianghuaXiyou:
		return LFStrings::getValue("duanzaoCailiaoXy_shuxing");
	case ItemsType_QianghuaChuangqi:
		return LFStrings::getValue("duanzaoCailiaoCq_shuxing");
	case ItemsType_QianghuaGailvTianjia:
		return LFStrings::getValue("duanzaoCailiaoGl_shuxing");
	case ItemsType_HellStone:
		return LFStrings::getValue("diyurongyan_shuxing");
	case ItemsType_GoldStar_Qianghua:
		return LFStrings::getValue("qiangHuaJinXingCaiLiao_shuxing");
	case ItemsType_Shenbing_Jiefengyin:
		return LFStrings::getValue("jieFengCaiLiao_shuxing");
	case ItemsType_Shenbing_Jianding:
		return LFStrings::getValue("jianDingCaiLiao_shuxing");
	case ItemsType_Dalishi:
		return LFStrings::getValue("daLiShi_shuxing");
	case ItemsType_Huagangyan:
		return LFStrings::getValue("huaGangYan_shuxing");
	case ItemsType_Mucai:
		return LFStrings::getValue("muCai_shuxing");
	case ItemsType_Shuijingshi:
		return LFStrings::getValue("shuiJingShi_shuxing");
	case ItemsType_JingwenZhuanke:
		return LFStrings::getValue("jinWenZhuanKeCaiLiao_shuxing");
	case ItemsType_Hero_LevelUp:
		return LFStrings::getValue("shengJiCaiLiao_shuxing");
	case ItemsType_Hero_Upgrade:
		return LFStrings::getValue("shengJieCaiLiao_shuxing");
	case ItemsType_DaiGouJuan:
		return LFStrings::getValue("DaiGouJuan_shuxing");
	default:
		CCLog("Waring : %s--> item_type = [%d]",__FUNCTION__,item_type);
		return "";
	}

}

std::string CS::getZhuangbeiTypeName(unsigned int zb_type, unsigned int zb_color)
{
	std::string chengse = getZhuangbeiChengSeName(zb_color);
	std::string leixing = getZhuangbeiTypeName(zb_type);
	char str[64];
	sprintf(str, "%s%s", chengse.c_str(), leixing.c_str());
	return str;
}

std::string CS::getZhuangbeiTypeName(unsigned int zb_type)
{
	std::string leixing;
	{
		switch(zb_type)
		{
		case ZhuangbeiType_DanshouJian:
			leixing = LFStrings::getValue("danshoujian");
			break;
		case ZhuangbeiType_ShuangshouBishou:
			leixing = LFStrings::getValue("shuangshoubishou");
			break;
		case ZhuangbeiType_ShuangshouFu:
			leixing = LFStrings::getValue("shuangshoufu");
			break;
		case ZhuangbeiType_DanshouFazhang:
			leixing = LFStrings::getValue("danshoufazhang");
			break;
		case ZhuangbeiType_ShuangshouFazhang:
			leixing = LFStrings::getValue("shuangshoufazhang");
			break;
		case ZhuangbeiType_Gong:
			leixing = LFStrings::getValue("gong");
			break;
		case ZhuangbeiType_Dunpai:
			leixing = LFStrings::getValue("dunpai");
			break;
		//case ZhuangbeiType_Toukui:
		//	leixing = LFStrings::getValue("toukui");
		//	break;
		case ZhuangbeiType_KuijiaZhong:
			leixing = LFStrings::getValue("zhongkuijia");
			break;
		case ZhuangbeiType_KuijiaQin:
			leixing = LFStrings::getValue("qingkuijia");
			break;
		/*case ZhuangbeiType_KuziZhong:
			leixing = LFStrings::getValue("zhongxinghutui");
			break;
		case ZhuangbeiType_KuziQin:
			leixing = LFStrings::getValue("qingxinghutui");
			break;
		case ZhuangbeiType_Xuezi:
			leixing = LFStrings::getValue("xuezi");
			break;*/
		case ZhuangbeiType_Jiezhi:
			leixing = LFStrings::getValue("jiezhi");
			break;
		case ZhuangbeiType_Hushenfu:
			leixing = LFStrings::getValue("hushenfu");
			break;
		case ZhuangbeiType_Faqi:
			leixing = LFStrings::getValue("faqi");
			break;
		default:
			CCLog("Waring : %s--> zb_type = [%d]",__FUNCTION__,zb_type);
			leixing = "NULL";
			break;
		}
	}

	return leixing;
}

std::string CS::getZhuangbeiChengSeName(unsigned int zb_color)
{
	std::string chengse;
	{
		switch(zb_color)
		{
		case ZhuangbeiColour_Baizhuang:
			chengse = LFStrings::getValue("putong");
			break;
		case ZhuangbeiColour_Fumo:
			chengse = LFStrings::getValue("mofa");
			break;
		case ZhuangbeiColour_Xiyou:
			chengse = LFStrings::getValue("xiyou");
			break;
		case ZhuangbeiColour_Chuanqi:
			chengse = LFStrings::getValue("chuanqi");
			break;
		default:
			CCLog("Waring : %s--> zb_color = [%d]",__FUNCTION__,zb_color);
			chengse = "NULL";
			break;
		}
	}
	return chengse;
}


std::string CS::getMofajinghuaName(unsigned int zb_type, unsigned int level)
{
	string strName;
	switch (level)
	{
	case 1:
		strName = LFStrings::getValue("yi");
		break;
	case 2:
		strName = LFStrings::getValue("er");
		break;
	case 3:
		strName = LFStrings::getValue("san");
		break;
	case 4:
		strName = LFStrings::getValue("si");
		break;
	default:
		CCLog("Waring : %s--> zb_type = [%d],level = [%d]",__FUNCTION__,zb_type,level);
		break;
	}
	strName += LFStrings::getValue("ji");
	switch (zb_type)
	{
	case ZhuangbeiType_DanshouJian:
		strName += LFStrings::getValue("danshoujian");
		break;
	case ZhuangbeiType_ShuangshouBishou:
		strName += LFStrings::getValue("shuangshoubishou");
		break;
	case ZhuangbeiType_ShuangshouFu:
		strName += LFStrings::getValue("shuangshoufu");
		break;
	case ZhuangbeiType_DanshouFazhang:
		strName += LFStrings::getValue("danshoufazhang");
		break;
	case ZhuangbeiType_Gong:
		strName += LFStrings::getValue("gong");
		break;
	case ZhuangbeiType_Faqi:
		strName += LFStrings::getValue("faqi");
		break;
	case ZhuangbeiType_Dunpai:
		strName += LFStrings::getValue("dunpai");
		break;
	case ZhuangbeiType_Hushenfu:
		strName += LFStrings::getValue("hushenfu");
		break;
	case ZhuangbeiType_KuijiaZhong:
		strName += LFStrings::getValue("zhongkuijia");
		break;
	case ZhuangbeiType_KuijiaQin:
		strName += LFStrings::getValue("qingkuijia");
		break;
	case ZhuangbeiType_Jiezhi:
		strName += LFStrings::getValue("jiezhi");
		break;
	case ZhuangbeiType_ShuangshouFazhang:
		strName += LFStrings::getValue("shuangshoufazhang");
		break;
	default:
		CCLog("Waring : %s--> zb_type = [%d],level = [%d]",__FUNCTION__,zb_type,level);
		break;
	}
	strName += LFStrings::getValue("zhi");
	strName += LFStrings::getValue("mofajinghua");

	return strName;
}

#endif // _CLIENT_

unsigned int CS::getEquipType(unsigned int type)
{
	if (_isFangju((ZhuangbeiType)type))
	{
		return EquipType_FangJu;
	}
	else if (_isWuqi((ZhuangbeiType)type))
	{
		return EquipType_WuQi;
	}
	else if (_isShipin((ZhuangbeiType)type))
	{
		return EquipType_ShiPin;
	}
	else
	{
		CCLog("Error : EquipmentManager::getEquipType ---> type = %d",type);
		CCAssert(false,"四不像！");
	}
	return 0x1fffffff;
}

bool CS::isShuangShouWuQi(unsigned int type)
{
	//如果是双手武器
	if(type == ZhuangbeiType_Gong
		|| type == ZhuangbeiType_ShuangshouFazhang
		|| type == ZhuangbeiType_ShuangshouFu
		|| type == ZhuangbeiType_ShuangshouBishou)
	{
		return true;
	}
	return false;
}

unsigned int CS::ShenbingJiefengyinNeedSilver()
{
    return 10000;
}
unsigned int CS::ShenbingJiefengyinNeedItemCount()
{
    return 1;
}
unsigned int CS::ShenbingJiandingNeedSilver(unsigned int identifiy_count)
{
    return 10000*identifiy_count;
}
unsigned int CS::ShenbingJiandingNeedItemCount(unsigned int identifiy_count)
{
    switch(identifiy_count)
    {
        case 1:
            return 2;
        case 2:
            return 3;
        case 3:
            return 4;
        case 4:
            return 6;
        default:
            return 8;
    }
}

unsigned int CS::ShenbingUpgradeNeedSilver(unsigned int shenbbing_level)
{
    if(shenbbing_level == 5 )
        return 10000;
    else if(shenbbing_level == 15)
        return 100000;
    else if(shenbbing_level == 25)
        return 500000;
    return 0;
}

unsigned int CS::ShenbingUpgradeNeedItemCount(unsigned int shenbbing_level)
{
    if(shenbbing_level == 5 )
        return 1;
    else if(shenbbing_level == 15)
        return 2;
    else if(shenbbing_level == 25)
        return 3;
    return 0;
}

unsigned int CS::ZBJinglianNeedSilver(unsigned int color, unsigned int jinglian_level)
{
    if(color == ZhuangbeiColour_Chuanqi) {
        return 10000*jinglian_level;
    }
    else if(color == ZhuangbeiColour_Xiyou) {
        return 5000*jinglian_level;
    }
    return 0;
}

unsigned int CS::ShenbingZhuankeJingwenNeedSilver()
{
    return 100000;
}

unsigned int CS::ShenbingZhuankeJingwenNeedItem()
{
    return 20;
}
unsigned int CS::AssistantUnLockNeedHonor(unsigned int locked_count)
{
    if(locked_count == 1) {
        return 1500;
    } 
    else if(locked_count == 2) {
        return 800;
    }
    else if(locked_count == 3) {
        return 400;
    }
    else if(locked_count == 4) {
        return 200;
    }
    return 0;
}

// 神兵的等级，按升序排列
std::vector<int> CS::getShenBingLevels()
{
	std::vector<int> levels;
	levels.push_back(5);
	levels.push_back(15);
	levels.push_back(25);
	levels.push_back(30);
	return levels;
}
// 获取神兵的下一个等级
// 没有/满级了 就返回-1
int CS::getNextShenBingLevel(unsigned int curLevel)
{
	int ret = -1;

	vector<int> levels = getShenBingLevels();
	for (int i=0,count=levels.size(); i<count; i++)
	{
		if (levels[i] > curLevel)
		{
			ret = levels[i];
			break;
		}
	}

	return ret;
}


int CS::getUnlockAbleHeroLevel()
{
    return 15;
}


void CS::getTaskChessboardFormat(unsigned int task_id
        , unsigned int& grid_count_heng, unsigned int& grid_count_shu)
{
    if(task_id >= HellTaskId_Min && task_id <= HellTaskId_Max) {
        grid_count_heng = 0;
        grid_count_shu = 0;

        return;
    }

    if(task_id == 1) {
        grid_count_heng = 2;
        grid_count_shu = 2;
    }
    else if(task_id>=2 && task_id<=8) {
        grid_count_heng = 3;
        grid_count_shu = 3;
    }
    else if(task_id==15 || task_id==25 || task_id==35  || task_id==40) {
        grid_count_heng = 0;
        grid_count_shu = 0;
    }
    else {
        grid_count_heng = 4;
        grid_count_shu = 4;
    }
    
    return;

#if 0  
    if(task_id >= HellTaskId_Min && task_id <= HellTaskId_Max) {
        grid_count_heng = 1;
        grid_count_shu = 1;

        return;
    }
    
    if(task_id == 1)
    {
        grid_count_heng = 2;
        grid_count_shu = 2;
    }
    else if(task_id == 2)
    {
        grid_count_heng = 2;
        grid_count_shu = 2;
    }
    else if(task_id == 3)
    {
        grid_count_heng = 2;
        grid_count_shu = 2;
    }
    else if(task_id == 4)
    {
        grid_count_heng = 2;
        grid_count_shu = 3;
    }
    else if(task_id == 5)
    {
        grid_count_heng = 2;
        grid_count_shu = 3;
    }
    else if(task_id == 6)
    {
        grid_count_heng = 2;
        grid_count_shu = 3;
    }
    else if(task_id == 15)
    {
        grid_count_heng = 1;
        grid_count_shu = 1;
    }
    else if(task_id == 25)
    {
        grid_count_heng = 1;
        grid_count_shu = 1;
    }
    else if(task_id == 27)
    {
        grid_count_heng = 1;
        grid_count_shu = 3;
    }
    else if(task_id == 29)
    {
        grid_count_heng = 1;
        grid_count_shu = 1;
    }
    else if(task_id == 40)
    {
        grid_count_heng = 1;
        grid_count_shu = 1;
    }
    else
    {
        grid_count_heng = 3;
        grid_count_shu = 3;
    }
    
    return;
#endif
    
}

unsigned int CS::arenaBuyOneChlgTimeNeedGold()
{
    return 10;
}

unsigned int CS::zahuopuGetOneItemNeedGold(unsigned int coupons_balance)
{
    if(coupons_balance >= 5) {
        return 0;
    }
    else {
        return 10 *(5 -coupons_balance);
    }
}

unsigned int CS::heroLevelUpNeedItemCount(unsigned int level)
{
    if(level <= Level_limit_arena)
    {
        return 0;
    }
    if(level <= 15)
    {
        return 1;
    }
    if(level <= 20)
    {
        return 2;
    }
    if(level <= 25)
    {
        return 3;
    }
    if(level <= 30)
    {
        return 5;
    }

    return 5;
}

unsigned int CS::heroLevelUpNeedSilver(unsigned int level)
{
	return 0;

    if(level <= Level_limit_arena)
    {
        return 0;
    }
    if(level <= 10)
    {
        return 1000;
    }
    if(level <= 15)
    {
        return 2000;
    }
    if(level <= 20)
    {
        return 3000;
    }
    if(level <= 25)
    {
        return 4000;
    }
    if(level <= 30)
    {
        return 5000;
    }

    return 9999999;
}

unsigned int CS::heroUpgradeNeedSilver(unsigned int stage)
{
    return stage*10000;
}

unsigned int CS::heroUpgradeNeedItemCount(unsigned int stage)
{
    return stage*10;
}

unsigned int CS::yuanshenUpgradeNeedSilver(unsigned int stage)
{
    return stage*20000;
}


void CS::openZhuangbeiOfYuanshenLevel(unsigned int yuanshen_level
    ,bool& wuqi_open,bool& fushou_open,bool& kuijia_open,bool& hushenfu_open,bool& jiezhi_open)
{
    wuqi_open = false;
    fushou_open = false;
    kuijia_open = false;
    hushenfu_open = false;
    jiezhi_open = false;

    if(yuanshen_level >=1)
    {
        wuqi_open = true;
        fushou_open = true;
    }
    if(yuanshen_level >=2)
    {
        kuijia_open = true;
    }
    if(yuanshen_level >=3)
    {
        hushenfu_open = true;
    }
    if(yuanshen_level >=4)
    {
        jiezhi_open = true;
    }
    
}

unsigned int CS::transferRatioOfYuanshenLevel(unsigned int yuanshen_level)
{
    switch(yuanshen_level)
    {
		case 1:
		case 2:
		case 3:
		case 4:
            return 9;
            break;
        case 5:
            return 12;
            break;
            
        case 6:
            return 20;
            break;
        case 7:
            return 23;
            break;
        case 8:
            return 26;
            break;
        case 9:
            return 29;
            break;
        case 10:
            return 32;
            break;

            
        case 11:
            return 40;
            break;
        case 12:
            return 43;
            break;
        case 13:
            return 46;
            break;
        case 14:
            return 49;
            break;
        case 15:
            return 52;
            break;

            
        case 16:
            return 60;
            break;
        case 17:
            return 63;
            break;
        case 18:
            return 66;
            break;
        case 19:
            return 69;
            break;
        case 20:
            return 72;
            break;

            
        case 21:
            return 80;
            break;
        case 22:
            return 83;
            break;
        case 23:
            return 86;
            break;
        case 24:
            return 89;
            break;
        case 25:
            return 92;
            break;

        default:
            return 0;
    }

    return 0;
}

unsigned int CS::getSkillEffectRatioOfNormalAttack(unsigned int skillType)
{
	return 200; // 目前简单点全部写200
}

// 获取不同等级的技能的效果加成
unsigned int CS::getSkillEffectRatioAddBySkillLevel(unsigned int skillLevel)
{
	unsigned int ret = 0;

	switch (skillLevel)
	{
	case 1:
		ret = 0;
		break;
	case 2:
		ret = 10;
		break;
	default:
		CCLog("Error : %s --> skillLevel[=%d] is not surport",__FUNCTION__,skillLevel);
		break;
	}
	return ret;
}

unsigned int CS::getHeroStageEffectRatio(unsigned int stage)
{
    if(stage > HERO_MAX_STAGE)
    {
        return 0;
    }

    return 5*(stage-1);
    
}

unsigned int CS::skillLevelUpNeedSilver(unsigned int new_skill_level)
{
    return new_skill_level*10000;
}

void CS::churchLevelUpNeedItemCount(unsigned int new_church_level
    ,unsigned int & dalishi_need,unsigned int & huagangyan_need
    ,unsigned int & mocai_need,unsigned int & shuijinshi_need)
{
    shuijinshi_need = 10*new_church_level;

    dalishi_need = 30*new_church_level;
    huagangyan_need = 30*new_church_level;
    mocai_need = 30*new_church_level;
    shuijinshi_need = 30*new_church_level;
}

unsigned int CS::churchLevelUpNeedSilver(unsigned int new_church_level)
{
    return new_church_level*10000;
}


unsigned int CS::cashBonusNeedKilledMonsters(unsigned int type)
{
    if(type == 1)
        return 100;
    else if(type == 2) 
        return 30;

    return 0;
}


unsigned int CS::getTopArenaWager(unsigned int type)
{
    if(type == 1) {
        return 8000;
    }
    else if(type == 2)
        return 16000;

    return 8000;
}


unsigned int CS::castSilverNeedGold(unsigned int times)
{
    if(times < 10)
        return 5*times;
    else 
        return 50;
}


unsigned int CS::getCastSilverCount(unsigned int level)
{
    if(level > 30)
        level = 30;
    
    return (100*level +2000);
}


void CS::refreshArenaChlgListNeedItem(unsigned int times, ItemInfo &item)
{
    //2013-05-14 去除 刷新竞技场消耗
    item.type = 0;
    item.count = 0;
    return;
    
    if(times == 1) {
        item.type = 0;
        item.count = 0;
    }
    else if(times == 2) {
        item.type = ItemsType_Silver;
        item.count = 3000;
    } 
    else {
        item.type = ItemsType_Gold;
        item.count = 50;
    }
}


unsigned int CS::getSelfChlgDifficulty(unsigned int level)
{
    if(level < 11) {
        return SelfChlgDiffType_NULL;
    }
    else if(level < 17 ) {
        return SelfChlgDiffType_Common;
    }
    else if(level < 22 ) {
        return SelfChlgDiffType_Difficult;
    }
    else if(level < 28 ) {
        return SelfChlgDiffType_Hell;
    }
    else {
        return SelfChlgDiffType_Nightmare;
    }

    return SelfChlgDiffType_NULL;
}


void CS::getSelfChlgGain(unsigned int type, unsigned int degree,
    unsigned int &expend, unsigned int &gain)
{
    expend = 0;
    gain = 0;
    if(degree == SelfChlgGainDegree_Low) { 
        expend = 15;
        gain = 15;
    }
    else {
        expend = 30;
        gain = 30;
    }
    
    return ;
}


void CS::getProtectSpiritBonus(unsigned int chapter, unsigned int defend_days, std::vector<ItemInfo> &item_vec)
{   
    item_vec.clear();

    if(chapter > 4)
        chapter = 0;
    
    if(defend_days < 1)
        defend_days = 1;
    else if(defend_days > 5)
        defend_days = 5;
    
    ItemInfo info;
    info.type = ItemsType_Silver;
    info.count = 10000 *chapter *defend_days;
    item_vec.push_back(info);
    return;
}


void CS::worshiProtectSpirit(unsigned int level, std::vector<ItemInfo> &item_vec)
{
    item_vec.clear();
    if(level > 30)
        level = 30;
    
    ItemInfo info;
    info.type = ItemsType_QianghuaNormal;
    info.count = level;
    item_vec.push_back(info);
    return;
}


unsigned int CS::getProtectSpiritChapter(unsigned int level)
{
    if(level > 25)
        return 4;
    else if(level > 20)
        return 3;
    else if(level > 15)
        return 2;
    else
        return 1;
}


unsigned int CS::getDiscoverLimitTimes(unsigned int vip_level)
{
    if(vip_level > 10) {
        vip_level = 10;
    }
    return  12 + 2*vip_level;
}


unsigned int CS::clearTopHeroCDTimeNeedGold()
{
    return 50;
}


unsigned int CS::getCityGuardBattleSkipCDNeedGold()
{
	return 10;
}

unsigned int CS::getCityGuardBattleEndCDTime()
{
	return 30;
}

//守城，击杀boss奖励
void CS::getCityGuardKillBossReward(ItemInfo& item)
{
    item.type = ItemsType_Silver;
    item.count = 20000;
}


//守城，伤害输出奖励
void CS::getCityGuardHeroShanghaiOrderReward(
    ItemInfo& item_order_1
    , ItemInfo& item_order_2
    , ItemInfo& item_order_3
    , ItemInfo& item_order_4_10
    , ItemInfo& item_order_11_20)
{
    item_order_1.type = ItemsType_Silver;
    item_order_1.count = 100000;

    item_order_2.type = ItemsType_Silver;
    item_order_2.count = 60000;

    item_order_3.type = ItemsType_Silver;
    item_order_3.count = 40000;

    item_order_4_10.type = ItemsType_Silver;
    item_order_4_10.count = 30000;

    item_order_11_20.type = ItemsType_Silver;
    item_order_11_20.count = 20000;
}


//守城，幸运奖励
void CS::getCityGuardLuckyReward(ItemInfo& item)
{
    item.type = ItemsType_Silver;
    item.count = 10000;
}


void CS::getCityGuardJoinReward(ItemInfo& item)
{
    item.type = ItemsType_Silver;
    item.count = 2000;
}


unsigned int CS::getJingLianMaxTimes(unsigned int color, unsigned int level) 
{
    if(ZhuangbeiColour_Chuanqi == color) {
        return 9;
    }
    else if(ZhuangbeiColour_Xiyou == color) {
        if(level >= 10 && level < 20) {
            return 3;
        }
        else if(level >= 20 && level <= 30) {
            return 5;
        }
    }
    return 0;
}


unsigned int CS::getPurifyMaxHoles(unsigned int type, unsigned int colour, unsigned int level)
{
    if(ZhuangbeiType_Jiezhi == type || ZhuangbeiType_Hushenfu == type) {
        if(ZhuangbeiColour_Chuanqi == colour) { 
            if(5 == level) {
                return 2;
            }
            else if(15 == level) {
                return 3;
            }
            else if(25 == level || 30 == level) {
                return 4;
            }
        }
        else if(ZhuangbeiColour_Xiyou == colour) {
            if(level >= 14 && level <= 20) {
                return 2;
            }
            else if(level >= 21 && level <= 25) {
                return 3;
            }
            else if(level >= 26 && level <= 30) {
                return 4;
            }
        }
    }

    return 0;
}


PurifyLevel CS::getPurifyLevel(unsigned int value) 
{
    if(value > 0 && value <= 3) {
        return PurifyLevel_IronStar;
    }
    else if(value >= 5 && value <= 8) {
        return PurifyLevel_CuprumStar;
    }
    else if(value >= 10 && value <= 15) {
        return PurifyLevel_SilverStar;
    }
    else if(value >= 18 && value <= 23) {
        return PurifyLevel_GoldStar;
    }
    else if(value >= 25 && value <= 30) {
        return PurifyLevel_YellowDiamond;
    }
    else if(value >= 32 && value <= 38) {
        return PurifyLevel_GreenDiamond;
    }
    else if(value >= 40 && value <= 50) {
        return PurifyLevel_PurpleDiamond;
    }

    return PurifyLevel_Error;
}


PurifyLevel CS::getPurifyUpperLimitLevel(unsigned int colour, unsigned int level, unsigned int &upper_limit)
{
    upper_limit = 0;
    PurifyLevel re_level = PurifyLevel_Error;

    if(ZhuangbeiColour_Chuanqi == colour) {
        upper_limit = 50;
        re_level = PurifyLevel_PurpleDiamond;
    }
    else if(ZhuangbeiColour_Xiyou == colour) {
        if(level >= 26) {
            upper_limit = 30;
            re_level = PurifyLevel_YellowDiamond;
        }
        else if(level >= 16) {
            upper_limit = 23;
            re_level = PurifyLevel_GoldStar;
        }
        else {
            upper_limit = 15;
            re_level = PurifyLevel_SilverStar;
        }
    }
    
    return re_level;
}

