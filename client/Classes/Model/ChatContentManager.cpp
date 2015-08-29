#include "ChatContentManager.h"
#include "MyselfManager.h"
#include "MessageDisplay.h"

#define CHAT_CONFIG_FILE "config/chatContent.txt"

 ChatContentManager* ChatContentManager::s_manager = NULL;

 ChatContentManager* ChatContentManager::sharedChatContentManager()
 {
	 if(s_manager == NULL)
	 {
		 s_manager = new ChatContentManager();
	 }
	
	 return s_manager;
 }

 ChatContentManager::ChatContentManager()
 {

	 bool success;
	 string doc = LFFileUtiles::getLFFileUtiles()->getResourceFile(CHAT_CONFIG_FILE,success);
	 CCAssert(success,"");

	 Json::Reader reader;
	 //Json::Value root;
	 //解析不成功就返回空;
	 if (!reader.parse(doc, mJson_Root))
	 {
		 CCLog("Error : %s --> parse json error!", __FUNCTION__);
		 CCAssert(false,"");
	 }

 }

 vector<ChatItem> ChatContentManager::getGuideTaskChatContent(int guideTaskIdx, int section)
 {
	 if (guideTaskIdx == 1)
		 return getMainTaskChatContentById(-guideTaskIdx, section);  //校验id == 1001
	 else if (guideTaskIdx == 2)
		 return getMainTaskChatContentById(-guideTaskIdx, section);  //校验id == 1002
	 else 
		 CCAssert(false, NULL);


	 vector<ChatItem> null_vec;
	 return null_vec;
 }

 vector<ChatItem> ChatContentManager::getMainTaskChatContentById(int id, int idx)
 {
	 int jiaoyanId = id;
	 if (id == 2 && (idx == 3 || idx == 4))  //助手引入剧情。
	 {
		 int allTaskSize = 40;
		 Profession profId = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;
		 if(profId == Profession_Yemanren)
		 {
			 id = allTaskSize + 1;   //野蛮人   41
			 jiaoyanId = 1102;
		 }
		 else if(profId == Profession_Chike)
		 {	 
			 id = allTaskSize + 3;   //刺客     43
			 jiaoyanId = 1112;
		 }
		 else if (profId == Profession_Fashi)
		 {
			id = allTaskSize + 5;   //法师     45
			jiaoyanId = 1122;
		 }
	 }
	 else if( id == 3 && (idx == 3 || idx == 4))
	 {
		 int allTaskSize = 40;
		 Profession profId = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;
		 if(profId == Profession_Yemanren)
		 {
			 id = allTaskSize + 2;   //野蛮人   42
			 jiaoyanId = 1103;
		 }
		 else if(profId == Profession_Chike)
		 {
			 id = allTaskSize + 4;   //刺客     44
			 jiaoyanId = 1113;
		 }
		 else if (profId == Profession_Fashi)
		 {
			 id = allTaskSize + 6;   //法师     46
			 jiaoyanId = 1123;
		 }
	 }
	 //如果是 教学  id == 0
	 else if (id <= 0)
	 {
		 if (id == -1)//教学1
		 {
			 id = 47;
			 jiaoyanId = 1001;
		 }
		 else if (id == -2) //教学2
		 {
			 id = 48;
			 jiaoyanId = 1002;
		 }
		 if (! (idx == 2 || idx == 4))
		 {
			 CCLog("Warrior: ---> guide chat content no the section [section == %d]", idx);
		 }
	 }




	 {
		 Json::Value allTaskChat = mJson_Root["MainTaskChatContent"];
		 Json::Value oneTask = allTaskChat[id];


		 if (oneTask["TaskId"].asInt() != jiaoyanId)
		 {
			 CCLog("Error : chat Id Error!");
			 CCAssert(false,"");
		 }


		 vector<ChatItem> vec;

		 char tempBuf[10] = "";
		 sprintf(tempBuf, "Section%d", idx);
		 Json::Value oneChat = oneTask[tempBuf];
		 if (oneChat.isNull())
		 {
			 CCLog("chat content is NULL");
		 }else
		 {
			 int chatNum = oneChat.size();
			 for(int i=0; i<chatNum; ++i)
			 {
				 ChatItem item;
				 item.name = oneChat[i]["HeroName"].asString();
				 item.chatStr = oneChat[i]["String"].asString();
				 if (item.name == "zhujue")
				 {
					Profession profId = MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty().profession;
					if (profId == Profession_Yemanren)
					{
						item.musicFile = oneChat[i]["music_ymr"].asString();
						if (!oneChat[i]["time_ymr"].isNull() && oneChat[i]["time_ymr"].asCString()) 
							item.deltaTime = atof(oneChat[i]["time_ymr"].asCString());
						else 
							item.deltaTime = 0;
					}
					else if (profId == Profession_Chike)
					{
						item.musicFile = oneChat[i]["music_ck"].asString();
						if (!oneChat[i]["time_ck"].isNull() && oneChat[i]["time_ck"].asCString()) 
							item.deltaTime = atof(oneChat[i]["time_ck"].asCString());
						else 
							item.deltaTime = 0;
					}
					else if (profId == Profession_Fashi)
					{
						item.musicFile = oneChat[i]["music_fs"].asString();
						if (!oneChat[i]["time_fs"].isNull() && oneChat[i]["time_fs"].asCString()) 
							item.deltaTime = atof(oneChat[i]["time_fs"].asCString());
						else 
							item.deltaTime = 0;
					}
					else
						CCAssert(false, "error main hero profession");
				 }
				 else 
				 {
					 item.musicFile = oneChat[i]["music"].asString();
					 if (!oneChat[i]["time"].isNull() && oneChat[i]["time"].asCString()) 
						 item.deltaTime = atof(oneChat[i]["time"].asCString());
					 else 
						 item.deltaTime = 0;
				 }
				 vec.push_back(item);
			 }
		 }

		 return vec;
	 }
 }

 vector<ChatItem> ChatContentManager::getGuideChatContent(string status)
 {
	 Json::Value allGuideChat = mJson_Root["guideChatContent"];
	 vector<ChatItem> vec;

	 if (status == "ZhuYeZhenZhangDuiHua")
	 {
		 {
			 ChatItem item;
			 item.name = allGuideChat[(Json::UInt)0]["HeroName"].asString();
			 item.chatStr = allGuideChat[(Json::UInt)0]["ZhuYeZhenZhangDuiHua"].asString();
			 item.musicFile = allGuideChat[(Json::UInt)0]["music"].asString();
			 if (!allGuideChat[(Json::UInt)0]["time"].isNull() && allGuideChat[(Json::UInt)0]["time"].asCString()) 
				 item.deltaTime = atof(allGuideChat[(Json::UInt)0]["time"].asCString());
			 else 
				 item.deltaTime = 0;
			 //		 item.deltaTime = atof(allGuideChat[(Json::UInt)0]["time"].asCString());
			 vec.push_back(item);
		 }
		 {
			 ChatItem item;
			 item.name = allGuideChat[(Json::UInt)11]["HeroName"].asString();
			 item.chatStr = allGuideChat[(Json::UInt)11]["FirstMaoxian"].asString();
			 if (!allGuideChat[(Json::UInt)11]["time"].isNull() && allGuideChat[(Json::UInt)11]["time"].asCString()) 
				 item.deltaTime = atof(allGuideChat[(Json::UInt)11]["time"].asCString());
			 else 
				 item.deltaTime = 0;
			 vec.push_back(item);
		 }/*
		 {
			 ChatItem item;
			 vector<BaseActor*> list = MyselfManager::getManager()->getMyZhuShous_ZhenShen();
			 if ( list.size() > 0 )
			 {
				 BaseActorProperty base = list[0]->getCurrentProperty();
				 
				 item.name = getPinyinName(base.profession);
				 //item.name = getPinyinName(base.baseName);
				 item.chatStr = allGuideChat[(Json::UInt)10]["ChuanQi_TuJianChat"].asString();
				 // item.musicFile = allGuideChat[(Json::UInt)10]["music"].asString();
				 if (!allGuideChat[(Json::UInt)10]["time"].isNull() && allGuideChat[(Json::UInt)10]["time"].asCString()) 
					 item.deltaTime = atof(allGuideChat[(Json::UInt)10]["time"].asCString());
				 else 
					 item.deltaTime = 0;
				 //		 item.deltaTime = atof(allGuideChat[(Json::UInt)0]["time"].asCString());
				 vec.push_back(item);
			 }
		 }*/
	 }
	 else if (status == "ChuanQi_Zhengsong")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)10]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)10]["ChuanQi_Zhengsong"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)10]["music"].asString();
		 if (!allGuideChat[(Json::UInt)10]["time"].isNull() && allGuideChat[(Json::UInt)10]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)10]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 //		 item.deltaTime = atof(allGuideChat[(Json::UInt)0]["time"].asCString());
		 vec.push_back(item);
	 }
	 else if (status == "gongjiang_qiangHuaHuiTui")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)1]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)1]["gongjiang_qiangHuaHuiTui"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)1]["music"].asString();
//		 item.deltaTime = atof(allGuideChat[(Json::UInt)1]["time"].asCString());
		 if (!allGuideChat[(Json::UInt)1]["time"].isNull() && allGuideChat[(Json::UInt)1]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)1]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 vec.push_back(item);
	 }
	 else if (status == "gongjiang_xiangQianBaoShi")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)2]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)2]["gongjiang_xiangQianBaoShi"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)2]["music"].asString();
//		 item.deltaTime = atof(allGuideChat[(Json::UInt)2]["time"].asCString());
		 if (!allGuideChat[(Json::UInt)2]["time"].isNull() && allGuideChat[(Json::UInt)2]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)2]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 vec.push_back(item);
	 }
	 else if (status == "gongjiang_baoShiHeCheng")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)3]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)3]["gongjiang_baoShiHeCheng"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)3]["music"].asString();
//		 item.deltaTime = atof(allGuideChat[(Json::UInt)3]["time"].asCString());
		 if (!allGuideChat[(Json::UInt)3]["time"].isNull() && allGuideChat[(Json::UInt)3]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)3]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 vec.push_back(item);
	 }
	 else if (status == "gongjiang_cuiQuFuMo")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)4]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)4]["gongjiang_cuiQuFuMo"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)4]["music"].asString();
		 if (!allGuideChat[(Json::UInt)4]["time"].isNull() && allGuideChat[(Json::UInt)4]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)4]["time"].asCString());
		 else 
			 item.deltaTime = 0;
//		 item.deltaTime = atof(allGuideChat[(Json::UInt)4]["time"].asCString());
		 vec.push_back(item);
	 }
	 else if (status == "ChuanQiFuBen_ZhenZhang_Section1")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)5]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)5]["ChuanQiFuBen_ZhenZhang_Section1"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)5]["music"].asString();
		 if (!allGuideChat[(Json::UInt)5]["time"].isNull() && allGuideChat[(Json::UInt)5]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)5]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 //		 item.deltaTime = atof(allGuideChat[(Json::UInt)4]["time"].asCString());
		 vec.push_back(item);
	 }
	 else if (status == "ChuanQiFuBen_ZhenZhang_Section5")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)6]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)6]["ChuanQiFuBen_ZhenZhang_Section5"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)6]["music"].asString();
		 if (!allGuideChat[(Json::UInt)6]["time"].isNull() && allGuideChat[(Json::UInt)6]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)6]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 vec.push_back(item);
	 }
	 else if (status == "ChuanQiFuBen_ZhenZhang_SongChuanQi")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)7]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)7]["ChuanQiFuBen_ZhenZhang_SongChuanQi"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)7]["music"].asString();
		 if (!allGuideChat[(Json::UInt)7]["time"].isNull() && allGuideChat[(Json::UInt)7]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)7]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 vec.push_back(item);
	 }
	 else if (status == "ChuanQiLu_ZhenZhang_ZhiYin")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)8]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)8]["ChuanQiLu_ZhenZhang_ZhiYin"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)8]["music"].asString();
		 if (!allGuideChat[(Json::UInt)8]["time"].isNull() && allGuideChat[(Json::UInt)8]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)8]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 vec.push_back(item);
	 }
	 else if (status == "ChuanQiLu_ZhenZhang_suipian")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)9]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)9]["ChuanQiLu_ZhenZhang_suipian"].asString();
		 item.musicFile = allGuideChat[(Json::UInt)9]["music"].asString();
		 if (!allGuideChat[(Json::UInt)9]["time"].isNull() && allGuideChat[(Json::UInt)9]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)9]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 vec.push_back(item);
	 }
	 else if (status == "FirstMaoxian")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)11]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)11]["FirstMaoxian"].asString();
		 if (!allGuideChat[(Json::UInt)11]["time"].isNull() && allGuideChat[(Json::UInt)11]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)11]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 vec.push_back(item);
	 }
	 else if (status == "ChuanQi_TuJianChat")
	 {
		 ChatItem item;
		 item.name = allGuideChat[(Json::UInt)12]["HeroName"].asString();
		 item.chatStr = allGuideChat[(Json::UInt)12]["ChuanQi_TuJianChat"].asString();
		 if (!allGuideChat[(Json::UInt)12]["time"].isNull() && allGuideChat[(Json::UInt)12]["time"].asCString()) 
			 item.deltaTime = atof(allGuideChat[(Json::UInt)12]["time"].asCString());
		 else 
			 item.deltaTime = 0;
		 vec.push_back(item);
	 }
	 return vec;
 }

 string ChatContentManager::getMainTaskNameById(int id)
 {
	 if (id > 100) id -= 100;

	 Json::Value allTaskChat = mJson_Root["MainTaskChatContent"];
	 Json::Value oneTask = allTaskChat[id];

	 if (oneTask["TaskId"].asInt() != id)
	 {
		 CCLog("Error : chat Id Error!");
		 CCAssert(false,"");
	 }
	
	 return oneTask["TaskName"].asString();
 }

 string ChatContentManager::getNpcFileNameByNpcName(string name)
 {
	 string fileName;
	 Json::Value allNames = mJson_Root["npcNames"];

	 for(int i=0; i!=allNames.size(); ++i)
	 {
		 if(allNames[i]["name"] == name)
		 {
			 fileName = allNames[i]["file"].asString();
			 break; 
		 }
	 }

	 return fileName;
 }
 string ChatContentManager::getPinyinName(int id)
 {
	 string name[10] = {"remanren","cike","fashi","qishi","shenjianshou","mishushi","wuseng","youxia","lieren", "wushi"};
	 return name[id - 1];
 }