#pragma once
#include "FightScriptDefine.h"
class GameBuffItemGetter;

// GameBuff�Ĵ�����
// �������̵Ľ�ɫ��Buff����
class GameBuffProc
{
private:
	int mCurrentUsedBuffId;
	//ChessBoardHelper* mHelper;
	GameBuffItemGetter* mGetter;
public:
	GameBuffProc(GameBuffItemGetter* getter);
	~GameBuffProc(void);

	// ����false���Ͳ���fight��������ѣ�ε�buff
	// �Ƴ���buff(buffeffect)��id�����removedBuff��������
	bool beforeFight(int fightorUniqueId,BuffInStep& buff);

	// ս���Ļغϱ任
	// ˫��������һȦ�ˣ��ʹ�����
	// ��beforeFight֮ǰ����
	// �Ƴ���buff(buffeffect)��id�����removedBuff��������
	void fightRoundChange(BuffInStep& buff);

	// һ�����ֻࣺ����Ѫ����buff,move fight skill 3�֡�
	// �Ƴ���buff(buffeffect)��id�����removedBuff��������
	void one_visible_step(BuffInStep& buff);
	//// ��ɫ�����򱻹����ˣ�Ҳ���ǵ��˹�����Ҳ���Ƕ��ѵĸ��Ӱ���
	//// ֻ�������ͱ�����֮���buff�Ƴ���û������ѣ�Σ���Ϊ��������Ƕ�������֮����õ�
	//// �����ѣ�εĻ�����������ǲ�����õ�
	//// �Ƴ���buff(buffeffect)��id�����removedBuff��������
	//void fight_end_or_beattacked(int fightorUniqueId,int targetUniqueId,vector<int>& removedBuff);


	// ��ɫ�����򱻹����ˣ�Ҳ���ǵ��˹�����Ҳ���Ƕ��ѵĸ��Ӱ���
	// ֻ�������ͱ�����֮���buff�Ƴ���û������ѣ�Σ���Ϊ��������Ƕ�������֮����õ�
	// �����ѣ�εĻ�����������ǲ�����õ�
	// �Ƴ���buff(buffeffect)��id�����removedBuff��������
	void fight_end(int fightorUniqueId,BuffInStep& buff);
	void beattacked(int fightorUniqueId,int targetUniqueId,int targetBloodChange,BuffInStep& buff);

	// ����һ��Buff��Id
	int genOneBuffId(){return ++mCurrentUsedBuffId;};
};

