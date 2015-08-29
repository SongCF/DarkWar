#include "CmdBuffer.h"


CmdBuffer::CmdBuffer(void)
{
	isArenaMarketBufferOK = false;
}

CmdBuffer::~CmdBuffer(void)
{
}

//void CmdBuffer::put(int cmd,NetworkRsp rspData,int lifeTime = 0)
//{
//	Flat_DuanzaoGroupZBQueryRsp* data = (Flat_DuanzaoGroupZBQueryRsp*)&rspData;
//	int a = 0;
//}
//
//bool CmdBuffer::get(NetworkReq req,NetworkRsp& bufferedData)
//{
//	return false;
//}

void CmdBuffer::putDuanzaoGroup(int groupId,Flat_DuanzaoGroupZBQueryRsp rsp)
{
	// 看是否存在了
	map<int,Flat_DuanzaoGroupZBQueryRsp>::iterator it;
	it = duanzaoBuffer.find(groupId);
	if (it!= duanzaoBuffer.end())
	{
		it->second = rsp;
	}else
	{
		duanzaoBuffer.insert(pair<int,Flat_DuanzaoGroupZBQueryRsp>(groupId,rsp));
	}
}
bool CmdBuffer::getDuanzaoGroup(int groupId,Flat_DuanzaoGroupZBQueryRsp& outData)
{
	map<int,Flat_DuanzaoGroupZBQueryRsp>::iterator it;
	it = duanzaoBuffer.find(groupId);
	if (it!= duanzaoBuffer.end())
	{
		outData = it->second;
		return true;
	}
	return false;
}


void CmdBuffer::putDiaoLuo(unsigned int taskId,Flat_QueryFubenDiaoluoRsp rsp)
{
	// 看是否存在了
	map<unsigned int,Flat_QueryFubenDiaoluoRsp>::iterator it;
	it = diaoluoBuffer.find(taskId);
	if (it!= diaoluoBuffer.end())
	{
		it->second = rsp;
	}else
	{
		diaoluoBuffer.insert(pair<unsigned int,Flat_QueryFubenDiaoluoRsp>(taskId,rsp));
	}
}
bool CmdBuffer::getDiaoLuo(unsigned int taskId,Flat_QueryFubenDiaoluoRsp& outData)
{
	map<unsigned int,Flat_QueryFubenDiaoluoRsp>::iterator it;
	it = diaoluoBuffer.find(taskId);
	if (it!= diaoluoBuffer.end())
	{
		outData = it->second;
		return true;
	}
	return false;
}

//void CmdBuffer::putZhuangBeiAlbum(vector<Flat_DuanzaoGroupZBInfo> data)
//{
//	zhuangbeiAlbumBuffer = data;
//}
//vector<Flat_DuanzaoGroupZBInfo> CmdBuffer::getZhuangBeiAlbum()
//{
//	return zhuangbeiAlbumBuffer;
//}

void CmdBuffer::putZhuangBeiAlbum(int profession,Flat_GetZhuangbeiAlbumRsp data)
{
	map<int,Flat_GetZhuangbeiAlbumRsp>::iterator it = zhuangbeiAlbumBuffer.find(profession);
	if (it == zhuangbeiAlbumBuffer.end())
	{
		zhuangbeiAlbumBuffer.insert(pair<int,Flat_GetZhuangbeiAlbumRsp>(profession,data));
	}
	else
	{
		it->second = data;
	}
}
bool CmdBuffer::getZhuangBeiAlbum(int profession,Flat_GetZhuangbeiAlbumRsp& outData)
{
	map<int,Flat_GetZhuangbeiAlbumRsp>::iterator it = zhuangbeiAlbumBuffer.find(profession);
	if (it == zhuangbeiAlbumBuffer.end())
	{
		return false;
	}
	else
	{
		outData = it->second;
	}

	return true;
}


//void CmdBuffer::putArenaMarketRsp(Flat_QueryArenaMarketRsp rsp)
//{
//	arenaMarketBuffer = rsp;
//	isArenaMarketBufferOK = true;
//}
//bool CmdBuffer::getArenaMarketRsp(Flat_QueryArenaMarketRsp& outData)
//{
//	if (isArenaMarketBufferOK)
//	{
//		outData = arenaMarketBuffer;
//	}
//	return isArenaMarketBufferOK;
//}


void CmdBuffer::clearDiaoLuo()
{
	diaoluoBuffer.clear();
}
void CmdBuffer::clearDuanZao()
{
	duanzaoBuffer.clear();
}
void CmdBuffer::clearZhuangBeiAlbum()
{
	zhuangbeiAlbumBuffer.clear();
}

void CmdBuffer::clearAll()
{
	clearDiaoLuo();
	clearDuanZao();
	clearZhuangBeiAlbum();
	isArenaMarketBufferOK = false;
}
