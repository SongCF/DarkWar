#pragma once
#include "flat.h"

struct NetworkRsp
{
	int err_code;
};
struct NetworkReq
{
};

struct BufferedData
{
	int cmd;
	NetworkRsp rspData;
	int lifeTime;
};

class CmdBuffer
{
private:
	map<int,Flat_DuanzaoGroupZBQueryRsp> duanzaoBuffer;
	map<unsigned int ,Flat_QueryFubenDiaoluoRsp> diaoluoBuffer;

	map<int,Flat_GetZhuangbeiAlbumRsp> zhuangbeiAlbumBuffer;
	//vector<Flat_DuanzaoGroupZBInfo> zhuangbeiAlbumBuffer;

	bool isArenaMarketBufferOK;
	Flat_QueryArenaMarketRsp arenaMarketBuffer;
public:
	CmdBuffer(void);

	//// 缓存一个服务器的返回消息
	//// cmd:消息的命令
	//// rspData : 服务器的返回数据
	//// lifeTime : 缓存的生命周期，时间到了缓存就失效
	//void put(int cmd,NetworkRsp rspData,int lifeTime = 0);

	//// 获取缓存的数据
	//bool get(NetworkReq req,NetworkRsp& bufferedData);

	void putDuanzaoGroup(int groupId,Flat_DuanzaoGroupZBQueryRsp rsp);
	bool getDuanzaoGroup(int groupId,Flat_DuanzaoGroupZBQueryRsp& outData);

	void putDiaoLuo(unsigned int taskId,Flat_QueryFubenDiaoluoRsp rsp);
	bool getDiaoLuo(unsigned int taskId,Flat_QueryFubenDiaoluoRsp& outData);

	//void putArenaMarketRsp(Flat_QueryArenaMarketRsp rsp);
	//bool getArenaMarketRsp(Flat_QueryArenaMarketRsp& outData);

	void putZhuangBeiAlbum(int profession,Flat_GetZhuangbeiAlbumRsp data);
	bool getZhuangBeiAlbum(int profession,Flat_GetZhuangbeiAlbumRsp& outData);

	void clearDiaoLuo();
	void clearDuanZao();
	void clearZhuangBeiAlbum();

	void clearAll();

	~CmdBuffer(void);
};

