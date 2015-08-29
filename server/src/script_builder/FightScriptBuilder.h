#pragma once

#include "protocol.h"
#include "FightScriptDefine.h"
#include "ScriptDataProvider.h"
#include "ChessBoardHelper.h"


// ս���ű��������� 
// ��������������ս���Ĺ��̣��ͽ��
// ������
// ��n����˭����˭��������ʽ��ʲô(���ܣ��չ�������,��Щ��ʽ��Ҫ������ȷ���������ܹ������õ�),
//		 ����ʱ��˭��Ѫ�����ٶ���(��������Ϊ����),˭����һ��������˭����һ��ս���ɹ���ʧ�ܡ�
//		 ��Ҫ��ô��λ(˭�ƶ����ĸ�λ�ã�������˾Ϳ��Բ���Ҫ��һ�����ܵĹ���/��λ)
// ��n�������ܲ��ǹ�����������λ(����Ҫ���ǵ�ʱ����λ�͹�������һ��)
//// ��n����bufftime
//
//
class FightScriptBuilder
{
private:   
	int getBattel_FightScriptLength(const FightScript* A_script);

	// ��һ��Battel_FightScript�����ݽṹת����һ��FightScript
	FightScript Battel_FightScript_2_FightScript(const char* data);

	// ��һ��FightScriptת����һ��Battel_FightScript��char*
	char* FightScript_2_Battel_FightScript(FightScript f_script,unsigned long& length);

	// �������outData����
	// item ���Ǹ�Id�����̽�ɫ�����ΪNULL����ôoutData�ͻ��ʼ��Ϊ��Чֵ
	// outData�����ݶ���ֱ����Դ��item��ֻ��id��ֱ�ӰѲ���Id��ֵ���������о���hero_type��Ҫ��ѯ���ݿ�
	void fillHeroBaseDataByChessBoardItem(unsigned heroId,HeroBase& outData,const ChessBoardItem* item);

	void fillHeroBaseDataByAbstractItem(unsigned heroId,HeroBase& outData,const AbstractItem* item);
public:
	FightScriptBuilder(void);

	// ���� A_team �� B_team����ս�籾
	// A �� B �Ľű��ĳ�ʼ���ͽ���ǲ�ͬ�ģ��������ǹ���һ������
	// A_team : A �ӵĲ�������A_team[0]����վ��0��λ��A����Ա��Id,0����û���ˡ�Ҫ������4����
	// B_team : B �ӵ�...
	// A_script : ���ɵ�A�ӵĽű�����
	// B_script : ���ɵ�B�ӵĽű�����
	HeroState genScript(
		unsigned int A_team_id[],unsigned int B_team_id[],
		FightScript& A_script,FightScript& B_script,
		ScriptDataProvider* provider,
		ScriptBuilderUserData userData,
		FirstBloodBoard firstBloodBoard,
		int huiHeLimite = -1 //huiHeLimite ���ƻغϵĴ���������غϴ������������ݣ�A����ʧ�ܣ�-1���ǲ�����
		);


	// �����ŶӴ�Boss�ľ籾
	// TeamHero* ��Ҫ����Լ�delete
	void genTeamFightScript(
		TeamHero* hero[FormationHeroAcountMax],
		Guaiwu* boss,
		vector<FightScript>& script_vec_4,
		TeamEndParam& endData);


	// ��A_scriptVec������ת�����ֽ��򣬴����char��������
	// ����ĳ��ȷ���length��,A_scriptVecû�оͷ���NULL��length��Ϊ0
	char* FightScriptVec_2_CharArr(vector<FightScript> A_scriptVec,int & length);
	// ��char����������ֽ���Ľű���ת���ɱ����ֽ��򣬲������FightScript����
	// ��Ҫ�����ڲ���������������һ���ű����ſ��Խ���
	vector<FightScript> CharArr_2_FightScriptVec(const char* scirpt_char_arr,int fightScriptNum);


	// ֻ���SPCmd_BattleRsp��
	// ��FightScriptת���������������data�ξ���ת���������ֽ����SPCmd_BattleRspָ��
	//char* FightScript_2_Package(FightScript* A_script);
	// û�н����İ��� ���� FightScript�������м���ģ�getSPCmd_BattleRspLength���Լ���
	char* FightScript_2_Package(vector<FightScript> A_scriptVec);
	// ֻ���SPCmd_BattleRsp��
	// ��SPCmd_BattleRsp�����ݽṹ(�����ֽ���)ת����FightScript�����ֽ���
	// rspCode����SPCmd_BattleRsp��code
	// �������Success����ôvector�ǿ�ֵ
	vector<FightScript> SPCmd_BattleRsp_2_FightScript(const char* data,int& rspCode);

	// ��actor���ۺ�����ת����TeamBaseProperty
	TeamBaseProperty combineActorProperty(Hero* actor);

	~FightScriptBuilder(void);
};

